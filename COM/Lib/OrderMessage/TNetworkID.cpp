#include "TNetworkID.h"
#include <math.h>
//------------------------------------------------------------------------------
extern UFC::BufferedLog* Glog;
//------------------------------------------------------------------------------
const int              TNetworkID::TYPE_MASK     = 0x00000007; ///< Binary: 00000000,00000111
const int              TNetworkID::ORD_TYPE_BITS = 3;
int 		       TNetworkID::CID_BITS      = 9;
int                    TNetworkID::ATTRIB_BITS   = TNetworkID::CID_BITS + TNetworkID::ORD_TYPE_BITS;
int                    TNetworkID::SEQ_MAX       = (int)pow((double)2.0, 29 - TNetworkID::CID_BITS  ); ///< default 2^20 (20 bites)
int                    TNetworkID::CID_MAX       = (int)pow((double)2.0, TNetworkID::CID_BITS  );;     ///< default 2^9  (9 bites)
//------------------------------------------------------------------------------
//        Default binary layout (SpeedyGateway before 2011/08/29)
//
//        | 20 Bites | 12Bites |
//           |           |____ Order Attribute
//           |
//           |__________ Order Sequence number: from 00000 to 1,048,576
//
//------------------------------------------------------------------------------
//             Order Attribute
//         | 9 bites | 3 bites |
//           |            |____ Order type I,R,D,Q,QR
//           |_________________ Connection ID: from 0 to 512
//
//  [ Order type I,R,D,Q,QR ](3 bites)
//  1      001  New Order
//  2      010  Cancel Order
//  3      011  Replace Order
//  4      100  Quote
//  5      101  Quote Cancel
//  6      110  Quote Request
//  7      111  Order Status Request
//
//  [ Connection ID ](9 bites)
//  Connection can up to 512 per day.
//
//  [ Order Sequence number ] ( 20 bites)
//  An AE can place up to 1,048,576 orders,Order Cancel.
//
//---------------------------------------------------------------------------
//        New binary layout ( SpeedyProxy supports this after 2011/08/29)
//                          ( SpeedyGateway supports this layout after 2020/06/07 )
//
//        | 29 - N Bites | NBites |3 Bites |
//           |                |      |____ Order Attribute
//           |                |
//           |                |____________ Connection ID bits
//           |
//           |__________ Order Sequence number: from 0 to pow( 2, 29-N )
//
//----------------------------------------------------------------------------
//             Order Attribute
//         | N bites | 3 bites |
//           |            |____ Order type I,R,D,Q,QR
//           |_________________ Connection ID: from 0 to pow( 2, N )
//
//  N can be 9,10,11,12,13,14 ( Default 9 )
//
//  [ Connection ID ](N bites)
//  Connection can up to (32 ~ 16,384) per day.
//
//  [ Order Sequence number ] ( 32 -N bites)
//  An AE can place up to (1,048,576 ~ 32,768) orders, Order Cancel.
//
//    CID Count       NID Count        CID Bites
// ----------------------------------------------
//           32	      16,777,216          5   <---- 1677 10K Orders
//           64	       8,388,608          6
//          128	       4,194,304          7
//          256	       2,097,152          8
//          512	       1,048,576          9   <---- Default 104.8 10K Orders
//        1,024	         524,288         10
//        2,048	         262,144         11
//        4,096	         131,072         12
//        8,192	          65,536         13
//       16,384	          32,768         14   <---- 3.2 10K Orders 
//
//---------------------------------------------- -----------------------------
void TNetworkID::Init( int CID, int CIDBits )
{
    TNetworkID::CID_BITS        = CIDBits; ///< Default = 9
    TNetworkID::ATTRIB_BITS     = TNetworkID::CID_BITS + TNetworkID::ORD_TYPE_BITS;
    TNetworkID::SEQ_MAX         = (int)pow((double)2.0, 29 - TNetworkID::CID_BITS  ); ///< 2^20 (20 bites)
    TNetworkID::CID_MAX         = (int)pow((double)2.0, TNetworkID::CID_BITS  );;     ///< 2^9  (9 bites)

    FSequence       = 0;
    FCancelSeq      = 0;
    FQuoteSequence  = 0;
    FQuoteCancelSeq = 0;
    FReplaceSeq     = 0;
    FNewsReqSeq     = 0;
    FConnectionID = CID;
    //Glog->fprintf( " ************ Use %d bits CID rule ***********", CIDBits );
}
//---------------------------------------------------------------------------
void TNetworkID::Uninit( void )
{
    TNetworkID::FConnectionID = 0;
}
//---------------------------------------------------------------------------
UInt32 TNetworkID::ToNID( nsOrderMessageDefine::MessageTypeEnum MessageType, int Seq )
{
    UInt32 UNID = (((Seq%SEQ_MAX)<<ATTRIB_BITS)|((FConnectionID%CID_MAX)<<ORD_TYPE_BITS)|((UInt32)MessageType));
    return UNID;
}
//---------------------------------------------------------------------------
UFCType::Int32 TNetworkID::GetSequenceNo( UFCType::UInt32 NID )
{
    return (NID >> ATTRIB_BITS);
}  //TNetworkID::GetSequenceNo()
//---------------------------------------------------------------------------
nsOrderMessageDefine::MessageTypeEnum TNetworkID::GetMessageType( UInt32 NID )
{
    return (nsOrderMessageDefine::MessageTypeEnum)(NID & TYPE_MASK);
}
//---------------------------------------------------------------------------
Int32 TNetworkID::GetConnectionID( UInt32 NID )
{
    return (( NID << (32-ATTRIB_BITS) )>> (32-CID_BITS) );
}
//---------------------------------------------------------------------------
UInt32 TNetworkID::CreateClientNewsID( int CID )
{
    DWORD NewID;
    UFC::PLockObject Lock( TNetworkID::FSeqCS );

    if( FNewsReqSeq >= 999999 || FNewsReqSeq< 0 )
            FNewsReqSeq = 0;
    NewID = FNewsReqSeq + ( CID*1000000 );
    FNewsReqSeq += 1;
    return NewID;
}
//---------------------------------------------------------------------------
UInt32 TNetworkID::GenNewNID( Int32& Seq, int CID, nsOrderMessageDefine::MessageTypeEnum MessageType )
{
    UInt32 NewNID;
    UFC::PLockObject Lock( TNetworkID::FSeqCS );

    if( Seq >= SEQ_MAX )
        Seq = 0;
    NewNID = ((Seq << ATTRIB_BITS)|((CID%CID_MAX)<<ORD_TYPE_BITS)|((int)MessageType));
    Seq += 1;
    return NewNID;
}
//---------------------------------------------------------------------------
UInt32 TNetworkID::CreateClientOrderID( int CID, nsOrderMessageDefine::MessageTypeEnum MessageType )
{
    UInt32 NewNID;

    switch( MessageType )
    {
        case nsOrderMessageDefine::mtNew:                ///< 1: New Order
                        NewNID = GenNewNID( FSequence, CID, MessageType );break;
        case nsOrderMessageDefine::mtCancel:            ///< 2: Cancel
                        NewNID = GenNewNID( FCancelSeq, CID, MessageType );	break;
        case nsOrderMessageDefine::mtQuote:             ///< 4: New Quote
                        NewNID = GenNewNID( FQuoteSequence, CID, MessageType );	break;
        case nsOrderMessageDefine::mtQuoteCancel:       ///< 5: Quote Cancel
                        NewNID = GenNewNID( FQuoteCancelSeq, CID, MessageType );break;
        case nsOrderMessageDefine::mtReplacePx:         ///< 0: ReplacePx
        case nsOrderMessageDefine::mtReplace:           ///< 3: Replace
        case nsOrderMessageDefine::mtQuoteRequest:      ///< 6: Quote Request
        case nsOrderMessageDefine::mtOrderStatusRequest:///< 7: Order Status Request
                        NewNID = GenNewNID( FReplaceSeq, CID, MessageType ); break;
        case nsOrderMessageDefine::mtNews:              ///<  News
                        NewNID = CreateClientNewsID( CID );	break;
        default:NewNID = 0;	break;
    }
    return NewNID;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

