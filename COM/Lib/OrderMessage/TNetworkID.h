#ifndef _TNetworkID_H_
#define _TNetworkID_H_
//--------------------------------------------------------------------------
#include "TBaseMessage.h"
#include "PInt32.h"
//--------------------------------------------------------------------------
class TNetworkID
{
private:
	UFC::PCriticalSection  FSeqCS;
	UFCType::Int32         FSequence;
	UFCType::Int32         FCancelSeq;
	UFCType::Int32         FQuoteSequence;
	UFCType::Int32         FQuoteCancelSeq;
	UFCType::Int32         FReplaceSeq;
	UFCType::Int32         FNewsReqSeq;
	UFCType::Int32         FConnectionID;
	static const int TYPE_MASK;     ///< Binary: 00000000,00000111
	static const int ORD_TYPE_BITS; ///< 3
	static int SEQ_MAX;             ///< 2^20 (20 bites)
	static int CID_MAX;             ///< 2^9  (9 bites)
	static int CID_BITS;            ///< 9
	static int ATTRIB_BITS;         ///< 12
private:
	UFCType::UInt32 GenNewNID( UFCType::Int32& Seq, int CID, nsOrderMessageDefine::MessageTypeEnum MessageType );
public:
	TNetworkID()
	:FSequence(0)
	,FCancelSeq(0)
	,FQuoteSequence(0)
	,FQuoteCancelSeq(0)
	,FReplaceSeq(0)
	,FNewsReqSeq(0)
	{}
public:
	UFCType::UInt32                       ToNID( nsOrderMessageDefine::MessageTypeEnum MessageType, int Seq );
	void                                  Init( int CID,int CIDBits = 9 );
	void                                  Uninit( void );
	UFCType::UInt32                       CreateClientOrderID( int CID, nsOrderMessageDefine::MessageTypeEnum MessageType );
	UFCType::UInt32                       CreateClientNewsID( int CID );
public:
	static UFCType::Int32                        GetSequenceNo( UFCType::UInt32 NID );
	static UFCType::Int32                        GetConnectionID( UFCType::UInt32 NID );
	static nsOrderMessageDefine::MessageTypeEnum GetMessageType( UFCType::UInt32 NID );

};
//--------------------------------------------------------------------------
#endif

