#include "TFXCMessages.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetConfirmSubsystemMessage( int FuncCode, int MsgType )
{    
        switch( FuncCode )
        {
            case 0: switch( MsgType ) // FCM
                    {
                        case  0: return mtC010;
                        case  1: return mtC020;
                        case  2: return mtC030;
                        case  4: return mtC040;
                        case  5: return mtC050;
                        case 10: return mtC060;
                        default: return mtUnknown;
                    }
            case 11: switch( MsgType ) //CM
                    {
                        case  0: return mtC011;
                        case  1: return mtC021;
                        case  2: return mtC031;
                        case  3: return mtC032;
                        default: return mtUnknown;
                    }
            default: return mtUnknown;
        }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC010::MessageC010( char* Data )
:MessageHeader( Data )
{
	MemoryStreamReader MemReader( Data + MESSAGE_HEADER_SIZE );
	MemReader.Read( BrokerID, 7 );
	MemReader.Read( StartSeq, 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC010::MessageC010( char* Broker, int SeqNo )
:MessageHeader( "50", "00", "00" )
{
	char SwqNoStr[9];

	sprintf( SwqNoStr, "%08d", SeqNo );
	memcpy( BrokerID, Broker,   7 );
	memcpy( StartSeq, SwqNoStr, 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageC010::GetSeq()
{
	UFC::AnsiString SeqStr(StartSeq,8);
	return SeqStr.ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC010::AsString( char* Buffer )
{
	MessageHeader::AsString( Buffer );
	MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
	MemWriter.Write( BrokerID, 7 );
	MemWriter.Write( StartSeq, 8 );
	MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC010::print( void )
{
	char     TmpStr[ 10 ];
	SocketPrintf("---------------- Message C010 -------------------\n" );
	strncpynt( TmpStr, BrokerID, 7 );
	SocketPrintf("[ Broker ID %s ]\n",TmpStr);
	strncpynt( TmpStr, StartSeq, 8 );
	SocketPrintf("[ Start sequence %s ]\n",TmpStr);
	SocketPrintf("-------------- Message C010 End -----------------\n" );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC011::MessageC011( char* Data )
:MessageHeader( Data )
{
	MemoryStreamReader MemReader( Data + MESSAGE_HEADER_SIZE );
	MemReader.Read( FCMID, 4 );
	MemReader.Read( FStartSeq, 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC011::MessageC011( char* CMID, int SeqNo )
:MessageHeader("50","11","00")
{
    char SwqNoStr[9];

    sprintf( SwqNoStr, "%08d", SeqNo );
    memcpy( FCMID, CMID,   4 );
    memcpy( FStartSeq, SwqNoStr, 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageC011::GetSeq()
{
	UFC::AnsiString SeqStr( FStartSeq,8);
	return SeqStr.ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC011::AsString( char* Buffer )
{
	MessageHeader::AsString( Buffer );
	MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
	MemWriter.Write( FCMID, 4 );
	MemWriter.Write( FStartSeq, 8 );
	MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC011::print( void )
{
	char     TmpStr[ 10 ];
	SocketPrintf("---------------- Message C011 -------------------\n" );
	strncpynt( TmpStr, FCMID, 4 );
	SocketPrintf("[ Clear Mamber ID %s ]\n",TmpStr);
	strncpynt( TmpStr, FStartSeq, 8 );
	SocketPrintf("[ Start sequence %s ]\n",TmpStr);
	SocketPrintf("-------------- Message C011 End -----------------\n" );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC020::MessageC020( char* Data )
:MessageHeader( Data )
{
	MemoryStreamReader MemReader( Data + MESSAGE_HEADER_SIZE );
	MemReader.Read( BrokerID, 7 );
	MemReader.Read( StartSeq, 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC020::MessageC020( char* Broker, int SeqNo )
:MessageHeader("50","00","01")
{
	char SwqNoStr[9];

	sprintf( SwqNoStr, "%08d", SeqNo );
	memcpy( BrokerID, Broker,   7 );
	memcpy( StartSeq, SwqNoStr, 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageC020::GetSeq()
{
	UFC::AnsiString SeqStr( StartSeq, 8 );
	return SeqStr.ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC020::AsString( char* Buffer )
{
	MessageHeader::AsString( Buffer );
	MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
	MemWriter.Write( BrokerID, 7 );
	MemWriter.Write( StartSeq, 8 );
	MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC020::print( void )
{
	char     TmpStr[ 10 ];
	SocketPrintf("---------------- Message C020 -------------------\n" );
	strncpynt( TmpStr, BrokerID, 7 );
	SocketPrintf("[ Broker ID %s ]\n",TmpStr);
	strncpynt( TmpStr, StartSeq, 8 );
	SocketPrintf("[ Start sequence %s ]\n",TmpStr);
	SocketPrintf("-------------- Message C020 End -----------------\n" );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC021::MessageC021( char* Data )
:MessageHeader( Data )
{
	MemoryStreamReader MemReader( Data + MESSAGE_HEADER_SIZE );
	MemReader.Read( FCMID, 4 );
	MemReader.Read( FStartSeq, 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC021::MessageC021( char* CMID, int SeqNo )
:MessageHeader("50","11","01")
{
	char SwqNoStr[9];

	sprintf( SwqNoStr, "%08d", SeqNo );
	memcpy( FCMID, CMID,   4 );
	memcpy( FStartSeq, SwqNoStr, 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageC021::GetSeq()
{
	UFC::AnsiString SeqStr( FStartSeq,8);
	return SeqStr.ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC021::AsString( char* Buffer )
{
	MessageHeader::AsString( Buffer );
	MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
	MemWriter.Write( FCMID, 4 );
	MemWriter.Write( FStartSeq, 8 );
	MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC021::print( void )
{
	char     TmpStr[ 10 ];
	SocketPrintf("---------------- Message C021 -------------------\n" );
	strncpynt( TmpStr, FCMID, 4 );
	SocketPrintf("[ Clear Mamber ID %s ]\n",TmpStr);
	strncpynt( TmpStr, FStartSeq, 8 );
	SocketPrintf("[ Start sequence %s ]\n",TmpStr);
	SocketPrintf("-------------- Message C021 End -----------------\n" );
}
//----------------------------------------------------------------------------------------------------------------------
// Confirm report message C030
//----------------------------------------------------------------------------------------------------------------------
MessageC030::MessageC030( )
:MessageHeader( "50", "00", "02" ),
 FMsgCount( 0 )
{
    memset( FMessages, 0, sizeof(C03XBody*)*3 );
}
//----------------------------------------------------------------------------------------------------------------------
MessageC030::MessageC030( char* Data )
:MessageHeader( Data )
{
    char* Buffer = Data + MESSAGE_HEADER_SIZE;
    int   DataOffset;

    memset( FMessages, 0, sizeof(C03XBody*)*3 );    // Set message pointers to NULL.
    memcpy( FBrokerID, Buffer, 7 );                 // Get the BrokerID from given string.
    Buffer += 7;
    FMsgCount = UFC::AnsiString( Buffer, 2 ).ToInt();    // Get the message count from given string.
    Buffer += 2;
    for( int i = 0; i < FMsgCount; i++ ) // Create attached messages.
    {
        FMessages[i] = C03XBody::CreateFromString( Buffer, DataOffset, '3' );
        Buffer += DataOffset;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// For TSCD last report message ONLY!
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC030::MessageC030( int TotalBuy, int TotalSell, int Seq )
:MessageHeader( "50", "00", "02" ),
 FMsgCount( 0 )
{
    memset( FMessages, 0, sizeof(C03XBody*)*3 );    // Set message pointers to NULL.
    memcpy( FBrokerID, "F099000", 7 );                 // Get the BrokerID from given string.
    AddMessage( new C030STSCDLastBody( TotalBuy, TotalSell, Seq ));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC030::~MessageC030( )
{
    for( int i = 0; i < FMsgCount; i++ )
             delete FMessages[ i ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL MessageC030::AddMessage( C03XBody* NewMessage )
{
    if( FMsgCount > 2 )
        return FALSE;
    else if( FMsgCount == 2 )
    {
        FMessages[ 2 ] = NewMessage;
        FMsgCount++;
        return FALSE;
    }
    else
    {
        FMessages[ FMsgCount ] = NewMessage;
        FMsgCount++;
        return TRUE;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC030::AsString( char* Buffer )
{
    char* StrBuffer = Buffer;
    int   Offset;

    MessageHeader::AsString( StrBuffer );
    StrBuffer = Buffer + strlen( Buffer );
    strncpy( StrBuffer, FBrokerID, 7 );
    StrBuffer += 7;
    sprintf( StrBuffer,"%02d",FMsgCount );
    StrBuffer += 2;
    for( int i = 0; i < FMsgCount; i++ )
    {
            Offset = FMessages[ i ]->AsString( StrBuffer );
            StrBuffer += Offset;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC030::print( void )
{
     char Buffer[256];
     SocketPrintf("---------------- Confirm Report Message C030 -----------------\n" );
     SocketPrintf("[ Broker_ID %s ]\n",UFC::AnsiString( FBrokerID, 7).c_str());
     SocketPrintf("[ Message_Count %02d ]\n", FMsgCount );
     for( int i = 0; i < FMsgCount; i++ )
     {
          FMessages[ i ]->AsString( Buffer );
          printf( "Report %d:%s\n",i+1, Buffer );
     }
     SocketPrintf("-------------------------- End of C030 ------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Confirm report message C030 body class.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
C03XBody* C03XBody::CreateFromString( char* Data, int &Offset, char APCode )
{
    if( APCode == '3' ) /// For both Futures & Options
    {
        switch( *Data )
        {
            case '1': return new C030SingleBody( Data, Offset );
            case '2': return new C030ComboBody( Data, Offset );
            case '3': return new C030ReduceCancelBody( Data, Offset );
            case '4': return new C030QuoteReduceCancelBody( Data, Offset );
            case '9': return new C030STSCDLastBody( Data, Offset );
        }
    }
    else
    {
        if( APCode == '8' ) ///< Order comfirm for APcode 8
        {
            switch( *Data )
            {
                case '1': return new C032OrderBody( Data, Offset );
                case '2': return new C032QuoteBody( Data, Offset );
            }
        }
        else if( APCode == '9' )
        {
            switch( *Data )
            {
                case '1': return new C031SingleBody( Data, Offset );
                case '2': return new C031ComboBody( Data, Offset );
            }
        }
    }
    throw( UFC::Exception( "Bed Report data "));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _FOR_FCM
C03XBody* C03XBody::CreateFromRPTLog( RPTLOG* RPTData, char APCode, BOOL IsTSCD )
{
    if( APCode == '3' )
    {
        if( RPTData->IDR.LINE_TAG == 'I')
        {
            if( RPTData->IDR.SC_CODE == 'S')
                return new C030SingleBody( RPTData, IsTSCD );
            else
                return new C030ComboBody( RPTData, IsTSCD );
        }
        else
        {
            if( RPTData->IDR.OQ_CODE == 'O' )
                return new C030ReduceCancelBody( RPTData, IsTSCD );
            else
                return new C030QuoteReduceCancelBody( RPTData, IsTSCD );
        }
    }
    else if( APCode == '8' )
    {
        if( RPTData->IDR.OQ_CODE == 'O' )
            return new C032OrderBody( RPTData );
        else
            return new C032QuoteBody( RPTData );        
    }
    else if( APCode == '9' )
    {
        if( RPTData->IDR.SC_CODE == 'S')
            return new C031SingleBody( RPTData );
        else
            return new C031ComboBody( RPTData );        
    }
    printf("########## Error RPTLOG ############\n");
    return NULL;
}
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
C030STSCDLastBody::C030STSCDLastBody( char* Data, int& Offset )
:C03XBody( '9' )
{
	MemoryStreamReader MemReader( Data );
	MemReader.Read( &FRecordType,  1  );
	MemReader.Read( FTotalBuy,      12 );
	MemReader.Read( FTotalSell,     12 );
	MemReader.Read( FLastSeq,      8  );
	Offset = MemReader.GetPosition();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
C030STSCDLastBody::C030STSCDLastBody( int TotalBuy, int TotalSell, int Seq )
:C03XBody( '9' )
{
	char TmpStr[ 16 ];

	sprintf( TmpStr, "%012d",TotalBuy );
	memcpy( FTotalBuy,TmpStr, 12 );
	sprintf( TmpStr, "%012d",TotalSell );
	memcpy( FTotalSell,TmpStr, 12 );
	sprintf( TmpStr, "%08d",Seq );
	memcpy( FLastSeq,TmpStr, 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int C030STSCDLastBody::AsString( char* Buffer )
{
	MemoryStreamWriter MemWriter( Buffer );
	MemWriter.Write( &FRecordType, 1  );
	MemWriter.Write( FTotalBuy,    12 );
	MemWriter.Write( FTotalSell,   12 );
	MemWriter.Write( FLastSeq,     8  );
	MemWriter.AppendZero();
	return MemWriter.GetPosition() - 1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void C030STSCDLastBody::print( void )
{
	SocketPrintf("-------------------- TSCD Last Message C030 ------------------\n" );
	SocketPrintf("[ TotalBuy %s ]\n"     , UFC::AnsiString( FTotalBuy,12 ).c_str());
	SocketPrintf("[ TotalSell %s ]\n", UFC::AnsiString( FTotalSell, 12 ).c_str());
	SocketPrintf("[ Last Seq %s ]\n",  UFC::AnsiString( FLastSeq, 8 ).c_str());
	SocketPrintf("-------------------------- End of C030 ------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Single order report message body.
//----------------------------------------------------------------------------------------------------------------------
C030SingleBody::C030SingleBody( char* Data, int& Offset )
:C03XBody( '1' )
{
    MemoryStreamReader MemReader( Data );
    MemReader.Read( &FRecordType,  1 );
    MemReader.Read( FBroker,       7 );
    MemReader.Read( FOrderNo,      5 );
    MemReader.Read( FInvestorACNo, 7 );
    MemReader.Read( &FInvestorFlag,1 );
    MemReader.Read( FProdID, 20 );
    MemReader.Read( &FBuySellCode, 1 );
    MemReader.Read( &FOrderType,   1 );
    MemReader.Read( FPrice,        9 );
    MemReader.Read( FQuntity,      4 );
    MemReader.Read( &FOCCode,      1 );
    MemReader.Read( FCMID,         4 );
    MemReader.Read( FMatchTime,    8 );
    MemReader.Read( FSeqNo,        8 );
    MemReader.Read( &FProdType,    1 ); //F: Future O:Options.
    Offset = MemReader.GetPosition();
    FProcessingTime = UFC::AnsiString( FMatchTime, 8 ).ToInt();
}
//----------------------------------------------------------------------------------------------------------------------
#ifndef _FOR_FCM
C030SingleBody::C030SingleBody( RPTLOG* rpt, BOOL IsTSCD )
:C03XBody( '1' )
{
    char buf[ 10 ];

    memcpy( FBroker,       rpt->IDR.FCM_NO, 7 );
    memcpy( FOrderNo,      rpt->IDR.ORDER_NO, 5);
    memcpy( FInvestorACNo, rpt->IDR.ACC_NO, 7);
    FInvestorFlag = rpt->IDR.ACC_CODE;    
    strncpy( FProdID, rpt->IDR.PROD_ID, 20 );
    FBuySellCode = rpt->IDR.BS_CODE;
    FOrderType   = rpt->IDR.TYPE;
    sprintf( buf, "%09d", rpt->IDR.PRICE );
    memcpy( FPrice, buf, 9 );        
    sprintf( buf, "%04d", rpt->IDR.QNTY );
    memcpy( FQuntity, buf, 4);
    FOCCode = rpt->IDR.OC_CODE;
    memcpy( FCMID ,rpt->IDR.CM_NO, 4 );
    MillIntToHHMMSSMMString( buf, rpt->MATCH_TIME );
    memcpy( FMatchTime, buf, 8);
    if( IsTSCD == TRUE )
        sprintf(buf,"%08d",rpt->TSCD_SEQ );
    else
        sprintf(buf,"%08d",rpt->FCM_SEQ );
    memcpy( FSeqNo, buf, 8 );
    FProdType = rpt->PROD_TYPE;
    FProcessingTime = ThreadClock::SecondOfToday() -  (rpt->MATCH_TIME/1000);
}
#endif
//----------------------------------------------------------------------------------------------------------------------
int C030SingleBody::AsString( char* Buffer )
{
    MemoryStreamWriter MemWriter( Buffer );
    MemWriter.Write( &FRecordType,  1 );
    MemWriter.Write( FBroker,       7 );
    MemWriter.Write( FOrderNo,      5 );
    MemWriter.Write( FInvestorACNo, 7 );
    MemWriter.Write( &FInvestorFlag,1 );
    MemWriter.Write( FProdID,      20 );
    MemWriter.Write( &FBuySellCode, 1 );
    MemWriter.Write( &FOrderType,   1 );
    MemWriter.Write( FPrice,        9 );    
    MemWriter.Write( FQuntity,      4 );
    MemWriter.Write( &FOCCode,      1 );
    MemWriter.Write( FCMID,         4 );
    MemWriter.Write( FMatchTime,    8 );
    MemWriter.Write( FSeqNo,        8 );
    MemWriter.Write( &FProdType,    1 ); //F: Future O:Options.
    MemWriter.AppendZero();
    return MemWriter.GetPosition() - 1;
}
//----------------------------------------------------------------------------------------------------------------------
void C030SingleBody::print( void )
{
    SocketPrintf("[ RecordType %c ]\n"  , FRecordType );
    SocketPrintf("[ Broker %s ]\n"      , UFC::AnsiString( FBroker,7 ).c_str());
    SocketPrintf("[ OrderNo %s ]\n"     , UFC::AnsiString( FOrderNo,5 ).c_str());
    SocketPrintf("[ InvestorACNo %s ]\n", UFC::AnsiString( FInvestorACNo, 7 ).c_str());
    SocketPrintf("[ InvestorFlag %c ]\n", FInvestorFlag );    
    SocketPrintf("[ ProdID %s ]\n"      , UFC::AnsiString( FProdID,      20 ).c_str());
    SocketPrintf("[ BuySellCode %c ]\n" , FBuySellCode );
    SocketPrintf("[ OrderType %c ]\n"   , FOrderType );
    SocketPrintf("[ Price %s ]\n"       , UFC::AnsiString( FPrice,        9 ).c_str());    
    SocketPrintf("[ Quantity %s ]\n"    , UFC::AnsiString( FQuntity,      4 ).c_str());
    SocketPrintf("[ OCCode %c ]\n"      , FOCCode );
    SocketPrintf("[ CMID %s ]\n"        , UFC::AnsiString( FCMID,         4 ).c_str());
    SocketPrintf("[ MatchTime %s ]\n"   , UFC::AnsiString( FMatchTime,    8 ).c_str());
    SocketPrintf("[ SeqNo %s ]\n"       , UFC::AnsiString( FSeqNo,        8 ).c_str());
    SocketPrintf("[ ProdType %c ]\n"    , FProdType );//F: Future O:Options.
}
//----------------------------------------------------------------------------------------------------------------------
//  Combo order report message body.
//----------------------------------------------------------------------------------------------------------------------
C030ComboBody::C030ComboBody( char* Data, int& Offset )
:C03XBody( '2' )
{
    MemoryStreamReader MemReader( Data );
    MemReader.Read( &FRecordType,   1 );
    MemReader.Read( FBroker,        7 );
    MemReader.Read( FOrderNo,       5 );
    MemReader.Read( FInvestorACNo,  7 );
    MemReader.Read( &FInvestorFlag, 1 );
    MemReader.Read( FProdID,       20 );
    MemReader.Read( FProdID1,      10 );
    MemReader.Read( &FBuySellCode1, 1 );
    MemReader.Read( FMatchPrice1,   9 );
    MemReader.Read( FMatchQuntity1, 4 );
    MemReader.Read( FProdID2,      10 );
    MemReader.Read( &FBuySellCode2, 1 );
    MemReader.Read( FMatchPrice2,   9 );
    MemReader.Read( FMatchQuntity2, 4 );
    MemReader.Read( &FBuySellCode,  1 );
    MemReader.Read( &FOrderType,    1 );
    MemReader.Read( FPrice,         9 );
    MemReader.Read( FQuntity,       4 );
    MemReader.Read( &FOCCode,       1 );
    MemReader.Read( FCMID,          4 );
    MemReader.Read( FMatchTime,     8 );
    MemReader.Read( FSeqNo,         8 );
    MemReader.Read( &FProd1Type,    1 );
    MemReader.Read( &FProd2Type,    1 );
    Offset = MemReader.GetPosition();
    FProcessingTime = UFC::AnsiString( FMatchTime, 8 ).ToInt();
}
//----------------------------------------------------------------------------------------------------------------------
#ifndef _FOR_FCM
C030ComboBody::C030ComboBody( RPTLOG* rpt, BOOL IsTSCD )
:C03XBody( '2' )
{
    char buf[ 10 ];

    memcpy( FBroker, rpt->IDR.FCM_NO, 7 );
    memcpy( FOrderNo, rpt->IDR.ORDER_NO, 5 );
    memcpy( FInvestorACNo, rpt->IDR.ACC_NO, 7 );
    FInvestorFlag = rpt->IDR.ACC_CODE;
    memcpy( FProdID, rpt->IDR.PROD_ID, 20 );
    memcpy( FProdID1, rpt->IDR.PROD_ID_1, 10 );
    FBuySellCode1 = rpt->IDR.BS_CODE_1;
    sprintf(buf,"%09d",rpt->IDR.PRICE_1);
    memcpy( FMatchPrice1, buf, 9 );
    sprintf(buf,"%04d",rpt->IDR.QNTY_1);
    memcpy( FMatchQuntity1, buf, 4 );
    memcpy( FProdID2, rpt->IDR.PROD_ID_2,10);
    FBuySellCode2 = rpt->IDR.BS_CODE_2;
    sprintf(buf,"%09d",rpt->IDR.PRICE_2);
    memcpy( FMatchPrice2,buf,9);
    sprintf(buf,"%04d",rpt->IDR.QNTY_2);
    memcpy( FMatchQuntity2,buf,4);
    FBuySellCode = rpt->IDR.BS_CODE;
    FOrderType   = rpt->IDR.TYPE;
    sprintf(buf,"%09d",rpt->IDR.PRICE);
    memcpy( FPrice, buf, 9);
    sprintf(buf,"%04d",rpt->IDR.QNTY);
    memcpy( FQuntity, buf, 4);
    FOCCode = rpt->IDR.OC_CODE;
    memcpy( FCMID, rpt->IDR.CM_NO, 4);
    MillIntToHHMMSSMMString( buf, rpt->MATCH_TIME );
    memcpy( FMatchTime, buf, 8 );
    if( IsTSCD == TRUE )
        sprintf(buf,"%08d",rpt->TSCD_SEQ );
    else
        sprintf(buf,"%08d",rpt->FCM_SEQ );
    memcpy( FSeqNo, buf, 8 );
    FProd1Type = rpt->PROD1_TYPE;
    FProd2Type = rpt->PROD2_TYPE;
    FProcessingTime = ThreadClock::SecondOfToday() -  (rpt->MATCH_TIME/1000);
}
#endif
//----------------------------------------------------------------------------------------------------------------------
int C030ComboBody::AsString( char* Buffer )
{
    MemoryStreamWriter MemWriter( Buffer );
    MemWriter.Write( &FRecordType,   1 );
    MemWriter.Write( FBroker,        7 );
    MemWriter.Write( FOrderNo,       5 );
    MemWriter.Write( FInvestorACNo,  7 );
    MemWriter.Write( &FInvestorFlag, 1 );
    MemWriter.Write( FProdID,       20 );
    MemWriter.Write( FProdID1,      10 );
    MemWriter.Write( &FBuySellCode1, 1 );
    MemWriter.Write( FMatchPrice1,   9 );
    MemWriter.Write( FMatchQuntity1, 4 );
    MemWriter.Write( FProdID2,      10 );
    MemWriter.Write( &FBuySellCode2, 1 );
    MemWriter.Write( FMatchPrice2,   9 );
    MemWriter.Write( FMatchQuntity2, 4 );
    MemWriter.Write( &FBuySellCode,  1 );
    MemWriter.Write( &FOrderType,    1 );
    MemWriter.Write( FPrice,         9 );
    MemWriter.Write( FQuntity,       4 );
    MemWriter.Write( &FOCCode,       1 );
    MemWriter.Write( FCMID,          4 );
    MemWriter.Write( FMatchTime,     8 );
    MemWriter.Write( FSeqNo,         8 );
    MemWriter.Write( &FProd1Type,    1 );
    MemWriter.Write( &FProd2Type,    1 );
    MemWriter.AppendZero();
    return MemWriter.GetPosition() - 1;
}
//----------------------------------------------------------------------------------------------------------------------
void C030ComboBody::print( void )
{
    SocketPrintf("[ RecordType %c ]\n"    ,FRecordType );
    SocketPrintf("[ Broker %s ]\n"        ,UFC::AnsiString( FBroker,        7 ).c_str());
    SocketPrintf("[ OrderNo %s ]\n"       ,UFC::AnsiString( FOrderNo,       5 ).c_str());
    SocketPrintf("[ InvestorACNo %s ]\n"  ,UFC::AnsiString( FInvestorACNo,  7 ).c_str());
    SocketPrintf("[ InvestorFlag %c ]\n"  ,FInvestorFlag );
    SocketPrintf("[ ProdID %s ]\n"        ,UFC::AnsiString( FProdID,       20 ).c_str());
    SocketPrintf("[ ProdID1 %s ]\n"       ,UFC::AnsiString( FProdID1,      10 ).c_str());
    SocketPrintf("[ BuySellCode1 %c ]\n"  ,FBuySellCode1 );
    SocketPrintf("[ MatchPrice1 %s ]\n"   ,UFC::AnsiString( FMatchPrice1,   9 ).c_str());
    SocketPrintf("[ MatchQuntity1 %s ]\n" ,UFC::AnsiString( FMatchQuntity1, 4 ).c_str());
    SocketPrintf("[ ProdID2 %s ]\n"       ,UFC::AnsiString( FProdID2,      10 ).c_str());
    SocketPrintf("[ BuySellCode2 %c ]\n"  ,FBuySellCode2 );
    SocketPrintf("[ MatchPrice2 %s ]\n"   ,UFC::AnsiString( FMatchPrice2,   9 ).c_str());
    SocketPrintf("[ MatchQuntity2 %s ]\n" ,UFC::AnsiString( FMatchQuntity2, 4 ).c_str());
    SocketPrintf("[ BuySellCode %c ]\n"   ,FBuySellCode );
    SocketPrintf("[ OrderType %c ]\n"     ,FOrderType );
    SocketPrintf("[ Price %s ]\n"         ,UFC::AnsiString( FPrice,         9 ).c_str());
    SocketPrintf("[ Quntity %s ]\n"       ,UFC::AnsiString( FQuntity,       4 ).c_str());
    SocketPrintf("[ OCCode %c ]\n"        ,FOCCode );
    SocketPrintf("[ CMID %s ]\n"          ,UFC::AnsiString( FCMID,          4 ).c_str());
    SocketPrintf("[ MatchTime %s ]\n"     ,UFC::AnsiString( FMatchTime,     8 ).c_str());
    SocketPrintf("[ FSeqNo %s ]\n"        ,UFC::AnsiString( FSeqNo,         8 ).c_str());
    SocketPrintf("[ FProd1Type %c ]\n"    ,FProd1Type );
    SocketPrintf("[ FProd2Type %c ]\n"    ,FProd2Type );
}
//----------------------------------------------------------------------------------------------------------------------
//  Order Reduce/Cancel report message.
//----------------------------------------------------------------------------------------------------------------------
C030ReduceCancelBody::C030ReduceCancelBody( char* Data, int& Offset )
:C03XBody( '3' )
{
    MemoryStreamReader MemReader( Data );
    MemReader.Read( &FRecordType,   1 );
    MemReader.Read( FStatusCode,    2 );
    MemReader.Read( FBroker,        7 );
    MemReader.Read( FOrderNo,       5 );
    MemReader.Read( FInvestorACNo,  7 );
    MemReader.Read( &FInvestorFlag, 1 );
    MemReader.Read( FProdID,       20 );
    MemReader.Read( &FBuySellCode,  1 );
    MemReader.Read( FCMID,          4 );
    MemReader.Read( &FTransCode,    1 );		//2: Change Quntity 3:Cancel
    MemReader.Read( FBeforeQuntity, 4 );
    MemReader.Read( FAfterQuntity,  4 );
    MemReader.Read( FReceiveTime,   8 );
    MemReader.Read( FSeqNo,         8 );
    Offset = MemReader.GetPosition();
    FProcessingTime = UFC::AnsiString( FReceiveTime, 8 ).ToInt();
}
//----------------------------------------------------------------------------------------------------------------------
#ifndef _FOR_FCM
C030ReduceCancelBody::C030ReduceCancelBody( RPTLOG* rpt, BOOL IsTSCD )
:C03XBody( '3' )
{
    char buf[ 10 ];

    sprintf( buf, "%02d", rpt->IDR.RETURN_CODE );
    memcpy( FStatusCode, buf, 2 );
    memcpy( FBroker, rpt->IDR.FCM_NO, 7 );
    memcpy( FOrderNo, rpt->IDR.ORDER_NO, 5 );
    memcpy( FInvestorACNo, rpt->IDR.ACC_NO, 7 );
    FInvestorFlag = rpt->IDR.ACC_CODE;
    memcpy( FProdID, rpt->IDR.PROD_ID,20);
    memcpy( FProdID, rpt->IDR.PROD_ID,20);    
    FBuySellCode = rpt->IDR.BS_CODE;
    memcpy( FCMID, rpt->IDR.CM_NO, 4 );
    if( rpt->IDR.LINE_TAG == 'R' )
        FTransCode = '2';
    else
        FTransCode = '3';
    sprintf( buf,"%04d",rpt->IDR.BEFORE_QNTY );
    memcpy( FBeforeQuntity, buf, 4 );
    sprintf( buf, "%04d", rpt->IDR.AFTER_QNTY );
    memcpy( FAfterQuntity, buf, 4 );
    MillIntToHHMMSSMMString( buf, rpt->MATCH_TIME );
    memcpy( FReceiveTime, buf, 8 );
    if( IsTSCD == TRUE )
        sprintf(buf,"%08d",rpt->TSCD_SEQ );
    else
        sprintf(buf,"%08d",rpt->FCM_SEQ );
    memcpy( FSeqNo, buf, 8 );
    FProcessingTime = ThreadClock::SecondOfToday() -  (rpt->MATCH_TIME/1000);
}
#endif
//----------------------------------------------------------------------------------------------------------------------
int C030ReduceCancelBody::AsString( char* Buffer )
{
    MemoryStreamWriter MemWriter( Buffer );
    MemWriter.Write( &FRecordType,   1 );
    MemWriter.Write( FStatusCode,    2 );
    MemWriter.Write( FBroker,        7 );
    MemWriter.Write( FOrderNo,       5 );
    MemWriter.Write( FInvestorACNo,  7 );
    MemWriter.Write( &FInvestorFlag, 1 );
    MemWriter.Write( FProdID,       20 );    
    MemWriter.Write( &FBuySellCode,  1 );
    MemWriter.Write( FCMID,          4 );
    MemWriter.Write( &FTransCode,    1 );		//2: Change Quntity 3:Cancel
    MemWriter.Write( FBeforeQuntity, 4 );
    MemWriter.Write( FAfterQuntity,  4 );
    MemWriter.Write( FReceiveTime,   8 );
    MemWriter.Write( FSeqNo,         8 );
    MemWriter.AppendZero();
    return MemWriter.GetPosition() - 1;
}
//----------------------------------------------------------------------------------------------------------------------
void C030ReduceCancelBody::print( void )
{
    SocketPrintf("[ Record Type %c ]\n"    ,FRecordType );
    SocketPrintf("[ Status Code %s ]\n"    ,UFC::AnsiString( FStatusCode,    2 ).c_str());
    SocketPrintf("[ Broker ID %s ]\n"      ,UFC::AnsiString( FBroker,        7 ).c_str());
    SocketPrintf("[ Order No %s ]\n"       ,UFC::AnsiString( FOrderNo,       5 ).c_str());
    SocketPrintf("[ Investor AC No %s ]\n" ,UFC::AnsiString( FInvestorACNo,  7 ).c_str());
    SocketPrintf("[ Investor Flag %c ]\n"  ,FInvestorFlag );
    SocketPrintf("[ Product ID %s ]\n"     ,UFC::AnsiString( FProdID,       20 ).c_str());    
    SocketPrintf("[ Buy Sell Code %c ]\n"  ,FBuySellCode );
    SocketPrintf("[ CM ID %s ]\n"          ,UFC::AnsiString( FCMID,          4 ).c_str());
    SocketPrintf("[ Trans Code %c ]\n"     ,FTransCode );		//2: Change Quntity 3:Cancel
    SocketPrintf("[ Before Quantity %s ]\n",UFC::AnsiString( FBeforeQuntity, 4 ).c_str());
    SocketPrintf("[ After Quantity %s ]\n" ,UFC::AnsiString( FAfterQuntity,  4 ).c_str());
    SocketPrintf("[ Receive Time %s ]\n"   ,UFC::AnsiString( FReceiveTime,   8 ).c_str());
    SocketPrintf("[ Seq No %s ]\n"         ,UFC::AnsiString( FSeqNo,         8 ).c_str());
}
//----------------------------------------------------------------------------------------------------------------------
//  Quote Reduce/Cancel report message.
//----------------------------------------------------------------------------------------------------------------------
C030QuoteReduceCancelBody::C030QuoteReduceCancelBody( char* Data, int& Offset )
:C03XBody( '4' )
{
    MemoryStreamReader MemReader( Data );
    MemReader.Read( &FRecordType,   1 );
    MemReader.Read( FStatusCode,    2 );
    MemReader.Read( FBroker,        7 );
    MemReader.Read( FOrderNo,       5 );
    MemReader.Read( FInvestorACNo,  7 );
    MemReader.Read( &FInvestorFlag, 1 );
    MemReader.Read( FProdID,       20 );
    MemReader.Read( &FBuySellCode,  1 );
    MemReader.Read( FCMID,          4 );
    MemReader.Read( &FTransCode,    1 );		//2: Change Quntity 3:Cancel
    MemReader.Read( FBeforeQuntity, 4 );
    MemReader.Read( FAfterQuntity,  4 );
    MemReader.Read( FReceiveTime,   8 );
    MemReader.Read( FSeqNo,         8 );
    Offset = MemReader.GetPosition();
    FProcessingTime = UFC::AnsiString( FReceiveTime, 8 ).ToInt();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _FOR_FCM
C030QuoteReduceCancelBody::C030QuoteReduceCancelBody(  RPTLOG* rpt, BOOL IsTSCD  )
:C03XBody( '4' )
{
    char buf[ 10 ];

    sprintf( buf, "%02d", rpt->IDR.RETURN_CODE );
    memcpy( FStatusCode, buf, 2 );
    memcpy( FBroker, rpt->IDR.FCM_NO, 7 );
    memcpy( FOrderNo, rpt->IDR.ORDER_NO, 5 );
    memcpy( FInvestorACNo, rpt->IDR.ACC_NO, 7 );
    FInvestorFlag = rpt->IDR.ACC_CODE;
    memcpy( FProdID, rpt->IDR.PROD_ID,20);
    FBuySellCode = rpt->IDR.BS_CODE;
    memcpy( FCMID, rpt->IDR.CM_NO, 4 );
    if( rpt->IDR.LINE_TAG == 'R' )
            FTransCode = '2';
    else
            FTransCode = '3';
    sprintf( buf,"%04d",rpt->IDR.BEFORE_QNTY );
    memcpy( FBeforeQuntity, buf, 4 );
    sprintf( buf, "%04d", rpt->IDR.AFTER_QNTY );
    memcpy( FAfterQuntity, buf, 4 );
    MillIntToHHMMSSMMString( buf, rpt->MATCH_TIME );
    memcpy( FReceiveTime, buf, 8 );
    if( IsTSCD == TRUE )
            sprintf(buf,"%08d",rpt->TSCD_SEQ );
    else
            sprintf(buf,"%08d",rpt->FCM_SEQ );
    memcpy( FSeqNo, buf, 8 );
    FProcessingTime = ThreadClock::SecondOfToday() -  (rpt->MATCH_TIME/1000);
}
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int C030QuoteReduceCancelBody::AsString( char* Buffer )
{
    MemoryStreamWriter MemWriter( Buffer );
    MemWriter.Write( &FRecordType,   1 );
    MemWriter.Write( FStatusCode,    2 );
    MemWriter.Write( FBroker,        7 );
    MemWriter.Write( FOrderNo,       5 );
    MemWriter.Write( FInvestorACNo,  7 );
    MemWriter.Write( &FInvestorFlag, 1 );
    MemWriter.Write( FProdID,       20 );
    MemWriter.Write( &FBuySellCode,  1 );
    MemWriter.Write( FCMID,          4 );
    MemWriter.Write( &FTransCode,    1 );		//2: Change Quntity 3:Cancel
    MemWriter.Write( FBeforeQuntity, 4 );
    MemWriter.Write( FAfterQuntity,  4 );
    MemWriter.Write( FReceiveTime,   8 );
    MemWriter.Write( FSeqNo,         8 );
    MemWriter.AppendZero();
    return MemWriter.GetPosition() - 1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void C030QuoteReduceCancelBody::print( void )
{
    SocketPrintf("[ Record Type %c ]\n"    ,FRecordType );
    SocketPrintf("[ Status Code %s ]\n"    ,UFC::AnsiString( FStatusCode,    2 ).c_str());
    SocketPrintf("[ Broker ID%s ]\n"       ,UFC::AnsiString( FBroker,        7 ).c_str());
    SocketPrintf("[ Order No %s ]\n"       ,UFC::AnsiString( FOrderNo,       5 ).c_str());
    SocketPrintf("[ Investor AC No %s ]\n" ,UFC::AnsiString( FInvestorACNo,  7 ).c_str());
    SocketPrintf("[ Investor Flag %c ]\n"  ,FInvestorFlag );
    SocketPrintf("[ Product ID %s ]\n"     ,UFC::AnsiString( FProdID,       20 ).c_str());
    SocketPrintf("[ Buy Sell Code %c ]\n"  ,FBuySellCode );
    SocketPrintf("[ CM ID %s ]\n"          ,UFC::AnsiString( FCMID,          4 ).c_str());
    SocketPrintf("[ Trans Code %c ]\n"     ,FTransCode );
    SocketPrintf("[ Before Quntity %s ]\n" ,UFC::AnsiString( FBeforeQuntity, 4 ).c_str());
    SocketPrintf("[ After Quntity %s ]\n"  ,UFC::AnsiString( FAfterQuntity,  4 ).c_str());
    SocketPrintf("[ Receive Time %s ]\n"   ,UFC::AnsiString( FReceiveTime,   8 ).c_str());
    SocketPrintf("[ Seq No %s ]\n"         ,UFC::AnsiString( FSeqNo,         8 ).c_str());
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Clear Member Confirm report message C031
//----------------------------------------------------------------------------------------------------------------------
MessageC031::MessageC031( )
:MessageHeader( "50", "11", "02" ),
 FMsgCount( 0 )
{
    memset( FMessages, 0, sizeof( C03XBody* )* 3 );
}
//----------------------------------------------------------------------------------------------------------------------
MessageC031::MessageC031( char* Data )
:MessageHeader( Data )
{
    char* Buffer = Data + MESSAGE_HEADER_SIZE;
    int   DataOffset;

    memset( FMessages, 0, sizeof( C03XBody* )* 3 ); // Set message pointers to NULL.
    memcpy( FBrokerID, Buffer, 4 );                 // Get the CM ID from given string.
    Buffer += 4;
    FMsgCount = UFC::AnsiString( Buffer, 2 ).ToInt();    // Get the message count from given string.
    Buffer += 2;
    for( int i = 0; i < FMsgCount; i++ )            // Create attached messages.
    {
        FMessages[i] = C03XBody::CreateFromString( Buffer, DataOffset, '9' );
        Buffer += DataOffset;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC031::~MessageC031( )
{
    for( int i = 0; i < FMsgCount; i++ )
             delete FMessages[ i ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL MessageC031::AddMessage( C03XBody* NewMessage )
{
    if( FMsgCount > 2 )
        return FALSE;
    else if( FMsgCount == 2 )
    {
        FMessages[ 2 ] = NewMessage;
        FMsgCount++;
        return FALSE;
    }
    else
    {
        FMessages[ FMsgCount ] = NewMessage;
        FMsgCount++;
        return TRUE;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC031::AsString( char* Buffer )
{
    char* StrBuffer = Buffer;
    int   Offset;

    MessageHeader::AsString( StrBuffer );
    StrBuffer = Buffer + strlen( Buffer );
    strncpy( StrBuffer, FBrokerID, 4 );
    StrBuffer += 4;
    sprintf( StrBuffer,"%02d",FMsgCount );
    StrBuffer += 2;
    for( int i = 0; i < FMsgCount; i++ )
    {
        Offset = FMessages[ i ]->AsString( StrBuffer );
        StrBuffer += Offset;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC031::print( void )
{
     char Buffer[ 256 ];
     SocketPrintf("--------------- CM Confirm Report Message C031 ----------------\n" );
     SocketPrintf("[ Broker ID %s ]\n",UFC::AnsiString( FBrokerID, 4 ).c_str());
     SocketPrintf("[ Message Count %02d ]\n", FMsgCount );
     for( int i = 0; i < FMsgCount; i++ )
     {
              FMessages[ i ]->AsString( Buffer );
              printf( "Report %d:%s\n",i+1, Buffer );
     }
     SocketPrintf("-------------------------- End of C031 ------------------------\n");
}
//----------------------------------------------------------------------------------------------------------------------
//   Clear Member single order/order confirm  message body for FUTURES.
//----------------------------------------------------------------------------------------------------------------------
C031FutureBody::C031FutureBody(  char* Data, int& Offset )
:C03XBody( 'O' )///< O or M
{
    MemoryStreamReader MemReader( Data );
    MemReader.Read( &FRecordType,   1 );
    MemReader.Read( FBroker,        7 );
    MemReader.Read( FInvestorACNo,  7 );
    MemReader.Read( &FInvestorFlag, 1 );
    MemReader.Read( FProdID,        7 );
    MemReader.Read( SettlementMonth,6 );
    MemReader.Read( StrikePrice,    8 );
    MemReader.Read(&TransCoode,     1 );
    MemReader.Read(&FBuySellCode,   1 );
    MemReader.Read(&OrderType,      1 );
    MemReader.Read(&OpenOffsetFlag, 1 );
    MemReader.Read( OrdMthPrice,    8 );
    MemReader.Read( OrdMthQty,      4 );
    MemReader.Read( FReceiveTime,   8 );
    MemReader.Read( FSeqNo,         8 );
    Offset = MemReader.GetPosition();
    FProcessingTime = UFC::AnsiString( FReceiveTime, 8 ).ToInt();
}
//----------------------------------------------------------------------------------------------------------------------
#ifndef _FOR_FCM
C031FutureBody::C031FutureBody(  RPTLOG* rpt, char ReplyType  )
:C03XBody( ReplyType )///< O or M
{
    char buf[ 10 ];

    memcpy( FBroker, rpt->IDR.FCM_NO, 7 );
    memcpy( FInvestorACNo, rpt->IDR.ACC_NO, 7);
    FInvestorFlag = rpt->IDR.ACC_CODE;
    memcpy( FProdID, rpt->IDR.PROD_ID, 7 );
    memcpy( SettlementMonth, rpt->IDR.PROD_ID_1, 6 );/// PROD_ID_1 as SettlementMonth
    sprintf( buf, "%08d", rpt->IDR.PRICE_1 ); ///Price_1 as StrikePrice
    memcpy(  StrikePrice, buf, 8 );
    if( rpt->IDR.LINE_TAG == 'I')
        TransCoode = '1';
    else if( rpt->IDR.LINE_TAG == 'R' )
        TransCoode = '2';
    else
        TransCoode = '3';
    FBuySellCode   = rpt->IDR.BS_CODE;
    OrderType      = rpt->IDR.TYPE;
    OpenOffsetFlag = rpt->IDR.OC_CODE;
    sprintf( buf, "%09d", rpt->IDR.PRICE );
    memcpy( OrdMthPrice, buf, 8 );
    sprintf( buf, "%04d", rpt->IDR.QNTY );
    memcpy( OrdMthQty, buf, 4);
    MillIntToHHMMSSMMString( buf, rpt->MATCH_TIME );
    memcpy( FReceiveTime, buf, 8);
    sprintf( buf, "%08d", rpt->CM_SEQ );
    memcpy( FSeqNo, buf, 8 );
    FProcessingTime = UFC::AnsiString( FReceiveTime, 8 ).ToInt();
}
#endif
//----------------------------------------------------------------------------------------------------------------------
int C031FutureBody::AsString( char* Buffer )
{
    MemoryStreamWriter MemWriter( Buffer );
    MemWriter.Write( &FRecordType,   1 );
    MemWriter.Write( FBroker,        7 );
    MemWriter.Write( FInvestorACNo,  7 );
    MemWriter.Write( &FInvestorFlag, 1 );
    MemWriter.Write( FProdID,        7 );
    MemWriter.Write( SettlementMonth,6 );
    MemWriter.Write( StrikePrice,    8 );
    MemWriter.Write(&TransCoode,     1 );
    MemWriter.Write(&FBuySellCode,   1 );
    MemWriter.Write(&OrderType,      1 );
    MemWriter.Write(&OpenOffsetFlag, 1 );
    MemWriter.Write( OrdMthPrice,    8 );
    MemWriter.Write( OrdMthQty,      4 );
    MemWriter.Write( FReceiveTime,   8 );
    MemWriter.Write( FSeqNo,         8 );
    MemWriter.AppendZero();
    return MemWriter.GetPosition() - 1;
}
//----------------------------------------------------------------------------------------------------------------------
void C031FutureBody::print( void )
{
    SocketPrintf("[ Record Type %c ]\n"    ,FRecordType );
    SocketPrintf("[ Broker ID %s ]\n"      ,UFC::AnsiString( FBroker,       7  ).c_str());
    SocketPrintf("[ Investor AC No %s ]\n" ,UFC::AnsiString( FInvestorACNo, 7  ).c_str());
    SocketPrintf("[ Investor Flag %c ]\n"  ,FInvestorFlag );
    SocketPrintf("[ Product ID %s ]\n"     ,UFC::AnsiString(  FProdID,       7 ).c_str());
    SocketPrintf("[ SettlementMonth %s ]\n",UFC::AnsiString( SettlementMonth,6 ).c_str());
    SocketPrintf("[ StrikePrice %s ]\n"    ,UFC::AnsiString( StrikePrice,    8 ).c_str());
    SocketPrintf("[ TransCoode %c ]\n"     ,TransCoode );
    SocketPrintf("[ BuySellCode %c ]\n"    ,FBuySellCode );
    SocketPrintf("[ Order Type %c ]\n"     ,OrderType );
    SocketPrintf("[ OpenOffsetFlag%c ]\n"  ,OpenOffsetFlag );
    SocketPrintf("[ OrdMthPrice %s ]\n"    ,UFC::AnsiString( OrdMthPrice,    8 ).c_str());
    SocketPrintf("[ OrdMthQty %s ]\n"      ,UFC::AnsiString( OrdMthQty,      4 ).c_str());
    SocketPrintf("[ FReceiveTime %s ]\n"   ,UFC::AnsiString( FReceiveTime,   8 ).c_str());
    SocketPrintf("[ Seq No %s ]\n"         ,UFC::AnsiString( FSeqNo,         8 ).c_str());
}
//----------------------------------------------------------------------------------------------------------------------
//   Clear Member single order report message body.
//----------------------------------------------------------------------------------------------------------------------
C031SingleBody::C031SingleBody( char* Data, int& Offset )
:C03XBody( '1' )
{
    MemoryStreamReader MemReader( Data );
    MemReader.Read( &FRecordType,  1 );
    MemReader.Read( FBroker,       7 );
    MemReader.Read( FOrderNo,      5 );
    MemReader.Read( FInvestorACNo, 7 );
    MemReader.Read( &FInvestorFlag,1 );
    MemReader.Read( FProdID,      20 );
    MemReader.Read( &FBuySellCode, 1 );
    MemReader.Read( &FOrderType,   1 );
    MemReader.Read( FPrice,        9 );
    MemReader.Read( FQuntity,      4 );
    MemReader.Read( &FOCCode,      1 );
    MemReader.Read( FCMID,         4 );
    MemReader.Read( FMatchTime,    8 );
    MemReader.Read( FSeqNo,        8 );
    MemReader.Read( &FProdType,    1 );//F: Future O:Options.
    Offset = MemReader.GetPosition();
    FProcessingTime = UFC::AnsiString( FMatchTime, 8 ).ToInt();
}
//----------------------------------------------------------------------------------------------------------------------
#ifndef _FOR_FCM
C031SingleBody::C031SingleBody( RPTLOG* rpt )
:C03XBody( '1' )
{
    char buf[ 10 ];

    memcpy( FBroker, rpt->IDR.FCM_NO, 7 );
    memcpy( FOrderNo, rpt->IDR.ORDER_NO, 5);
    memcpy( FInvestorACNo, rpt->IDR.ACC_NO, 7);
    FInvestorFlag = rpt->IDR.ACC_CODE;
    memcpy( FProdID, rpt->IDR.PROD_ID, 20 );
    FBuySellCode = rpt->IDR.BS_CODE;
    FOrderType   = rpt->IDR.TYPE;
    sprintf( buf, "%09d", rpt->IDR.PRICE );
    memcpy( FPrice, buf, 9 );
    sprintf( buf, "%04d", rpt->IDR.QNTY );
    memcpy( FQuntity, buf, 4);
    FOCCode = rpt->IDR.OC_CODE;
    memcpy( FCMID ,rpt->IDR.CM_NO, 4 );
    MillIntToHHMMSSMMString( buf, rpt->MATCH_TIME );
    memcpy( FMatchTime, buf, 8 );
    sprintf( buf, "%08d", rpt->CM_SEQ );
    memcpy( FSeqNo, buf, 8 );
    FProdType = rpt->PROD_TYPE;
    FProcessingTime = ThreadClock::SecondOfToday() -  (rpt->MATCH_TIME/1000);
}
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int C031SingleBody::AsString( char* Buffer )
{
    MemoryStreamWriter MemWriter( Buffer );
    MemWriter.Write( &FRecordType,  1 );
    MemWriter.Write( FBroker,       7 );
    MemWriter.Write( FOrderNo,      5 );
    MemWriter.Write( FInvestorACNo, 7 );
    MemWriter.Write( &FInvestorFlag,1 );
    MemWriter.Write( FProdID,      20 );
    MemWriter.Write( &FBuySellCode, 1 );
    MemWriter.Write( &FOrderType,   1 );
    MemWriter.Write( FPrice,        9 );
    MemWriter.Write( FQuntity,      4 );
    MemWriter.Write( &FOCCode,      1 );
    MemWriter.Write( FCMID,         4 );
    MemWriter.Write( FMatchTime,    8 );
    MemWriter.Write( FSeqNo,        8 );
    MemWriter.Write( &FProdType,    1 );//F: Future O:Options.
    MemWriter.AppendZero();
    return MemWriter.GetPosition() - 1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void C031SingleBody::print( void )
{
    SocketPrintf("[ Record Type %c ]\n"    ,FRecordType );
    SocketPrintf("[ Broker ID %s ]\n"      ,UFC::AnsiString( FBroker,       7 ).c_str());
    SocketPrintf("[ Order No %s ]\n"       ,UFC::AnsiString( FOrderNo,      5 ).c_str());
    SocketPrintf("[ Investor AC No %s ]\n" ,UFC::AnsiString( FInvestorACNo, 7 ).c_str());
    SocketPrintf("[ Investor Flag %c ]\n"  ,FInvestorFlag );
    SocketPrintf("[ Product ID %s ]\n"     ,UFC::AnsiString(  FProdID,      20 ).c_str());
    SocketPrintf("[ Buy Sell Code %c ]\n"  ,FBuySellCode );
    SocketPrintf("[ Order Type %c ]\n"     ,FOrderType );
    SocketPrintf("[ Price %s ]\n"          ,UFC::AnsiString( FPrice,        9 ).c_str());
    SocketPrintf("[ Quntity %s ]\n"        ,UFC::AnsiString( FQuntity,      4 ).c_str());
    SocketPrintf("[ OC Code %c ]\n"        ,FOCCode );
    SocketPrintf("[ CM ID %s ]\n"          ,UFC::AnsiString( FCMID,         4 ).c_str());
    SocketPrintf("[ Match Time %s ]\n"     ,UFC::AnsiString( FMatchTime,    8 ).c_str());
    SocketPrintf("[ Seq No %s ]\n"         ,UFC::AnsiString( FSeqNo,        8 ).c_str());
    SocketPrintf("[ Product Type %c ]\n"   ,FProdType );//F: Future O:Options.
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//   Clear Member Combo order report message body.
//----------------------------------------------------------------------------------------------------------------------
C031ComboBody::C031ComboBody( char* Data, int& Offset )
:C03XBody( '2' )
{
    MemoryStreamReader MemReader( Data  );
    MemReader.Read( &FRecordType,   1 );
    MemReader.Read( FBroker,        7 );
    MemReader.Read( FOrderNo,       5 );
    MemReader.Read( FInvestorACNo,  7 );
    MemReader.Read( &FInvestorFlag, 1 );
    MemReader.Read( FProdID,       20 );
    MemReader.Read( FProdID1,      10 );
    MemReader.Read( &FBuySellCode1, 1 );
    MemReader.Read( FMatchPrice1,   9 );
    MemReader.Read( FMatchQuntity1, 4 );
    MemReader.Read( FProdID2,      10 );
    MemReader.Read( &FBuySellCode2, 1 );
    MemReader.Read( FMatchPrice2,   9 );
    MemReader.Read( FMatchQuntity2, 4 );
    MemReader.Read( &FBuySellCode,  1 );
    MemReader.Read( &FOrderType,    1 );
    MemReader.Read( FPrice,         9 );
    MemReader.Read( FQuntity,       4 );
    MemReader.Read( &FOCCode,       1 );
    MemReader.Read( FCMID,          4 );
    MemReader.Read( FMatchTime,     8 );
    MemReader.Read( FSeqNo,         8 );
    MemReader.Read( &FProd1Type,    1 );
    MemReader.Read( &FProd2Type,    1 );
    Offset = MemReader.GetPosition();
    FProcessingTime = UFC::AnsiString( FMatchTime, 8 ).ToInt();
}
//----------------------------------------------------------------------------------------------------------------------
#ifndef _FOR_FCM
C031ComboBody::C031ComboBody( RPTLOG* rpt )
:C03XBody( '2' )
{
    char buf[ 10 ];

    memcpy( FBroker, rpt->IDR.FCM_NO, 7 );
    memcpy( FOrderNo, rpt->IDR.ORDER_NO, 5 );
    memcpy( FInvestorACNo, rpt->IDR.ACC_NO, 7 );
    FInvestorFlag = rpt->IDR.ACC_CODE;
    memcpy( FProdID, rpt->IDR.PROD_ID, 20 );
    memcpy( FProdID1, rpt->IDR.PROD_ID_1, 10 );
    FBuySellCode1 = rpt->IDR.BS_CODE_1;
    sprintf(buf,"%09d",rpt->IDR.PRICE_1);
    memcpy( FMatchPrice1, buf, 9 );
    sprintf(buf,"%04d",rpt->IDR.QNTY_1);
    memcpy( FMatchQuntity1, buf, 4 );
    memcpy( FProdID2, rpt->IDR.PROD_ID_2,10);
    FBuySellCode2 = rpt->IDR.BS_CODE_2;
    sprintf(buf,"%09d",rpt->IDR.PRICE_2);
    memcpy( FMatchPrice2,buf,9);
    sprintf(buf,"%04d",rpt->IDR.QNTY_2);
    memcpy( FMatchQuntity2,buf,4);
    FBuySellCode = rpt->IDR.BS_CODE;
    FOrderType   = rpt->IDR.TYPE;
    sprintf(buf,"%09d",rpt->IDR.PRICE);
    memcpy( FPrice, buf, 9);
    sprintf(buf,"%04d",rpt->IDR.QNTY);
    memcpy( FQuntity, buf, 4);
    FOCCode = rpt->IDR.OC_CODE;
    memcpy( FCMID, rpt->IDR.CM_NO, 4);
    MillIntToHHMMSSMMString( buf, rpt->MATCH_TIME );
    memcpy( FMatchTime, buf, 8 );
    sprintf(buf,"%08d",rpt->CM_SEQ );
    memcpy( FSeqNo, buf, 8 );
    FProd1Type = rpt->PROD1_TYPE;
    FProd2Type = rpt->PROD2_TYPE;
    FProcessingTime = ThreadClock::SecondOfToday() -  (rpt->MATCH_TIME/1000);
}
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int C031ComboBody::AsString( char* Buffer )
{
    MemoryStreamWriter MemWriter( Buffer );
    MemWriter.Write( &FRecordType,   1 );
    MemWriter.Write( FBroker,        7 );
    MemWriter.Write( FOrderNo,       5 );
    MemWriter.Write( FInvestorACNo,  7 );
    MemWriter.Write( &FInvestorFlag, 1 );
    MemWriter.Write( FProdID,       20 );
    MemWriter.Write( FProdID1,      10 );
    MemWriter.Write( &FBuySellCode1, 1 );
    MemWriter.Write( FMatchPrice1,   9 );
    MemWriter.Write( FMatchQuntity1, 4 );
    MemWriter.Write( FProdID2,      10 );
    MemWriter.Write( &FBuySellCode2, 1 );
    MemWriter.Write( FMatchPrice2,   9 );
    MemWriter.Write( FMatchQuntity2, 4 );
    MemWriter.Write( &FBuySellCode,  1 );
    MemWriter.Write( &FOrderType,    1 );
    MemWriter.Write( FPrice,         9 );
    MemWriter.Write( FQuntity,       4 );
    MemWriter.Write( &FOCCode,       1 );
    MemWriter.Write( FCMID,          4 );
    MemWriter.Write( FMatchTime,     8 );
    MemWriter.Write( FSeqNo,         8 );
    MemWriter.Write( &FProd1Type,    1 );
    MemWriter.Write( &FProd2Type,    1 );
    MemWriter.AppendZero();
    return MemWriter.GetPosition() - 1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void C031ComboBody::print( void )
{
    SocketPrintf("[ Record Type %c ]\n"     ,FRecordType );
    SocketPrintf("[ Broker ID %s ]\n"       ,UFC::AnsiString( FBroker,        7 ).c_str());
    SocketPrintf("[ Order No %s ]\n"        ,UFC::AnsiString( FOrderNo,       5 ).c_str());
    SocketPrintf("[ Investor AC No %s ]\n"  ,UFC::AnsiString( FInvestorACNo,  7 ).c_str());
    SocketPrintf("[ Investor Flag %c ]\n"   ,FInvestorFlag );
    SocketPrintf("[ Product ID %s ]\n"      ,UFC::AnsiString( FProdID,       20 ).c_str());
    SocketPrintf("[ Product ID1 %s ]\n"     ,UFC::AnsiString( FProdID1,      10 ).c_str());
    SocketPrintf("[ Buy Sell Code1 %c ]\n"  ,FBuySellCode1 );
    SocketPrintf("[ Match Price1 %s ]\n"    ,UFC::AnsiString( FMatchPrice1,   9 ).c_str());
    SocketPrintf("[ Match Quntity1 %s ]\n"  ,UFC::AnsiString( FMatchQuntity1, 4 ).c_str());
    SocketPrintf("[ Product ID2 %s ]\n"     ,UFC::AnsiString( FProdID2,      10 ).c_str());
    SocketPrintf("[ Buy Sell Code2 %c ]\n"  ,FBuySellCode2 );
    SocketPrintf("[ Match Price2 %s ]\n"    ,UFC::AnsiString( FMatchPrice2,   9 ).c_str());
    SocketPrintf("[ Match Quntity2 %s ]\n"  ,UFC::AnsiString( FMatchQuntity2, 4 ).c_str());
    SocketPrintf("[ Buy Sell Code %c ]\n"   ,FBuySellCode );
    SocketPrintf("[ Order Type %c ]\n"      ,FOrderType );
    SocketPrintf("[ Price %s ]\n"           ,UFC::AnsiString( FPrice,         9 ).c_str());
    SocketPrintf("[ Quntity %s ]\n"         ,UFC::AnsiString( FQuntity,       4 ).c_str());
    SocketPrintf("[ OC Code %c ]\n"         ,FOCCode );
    SocketPrintf("[ CM ID %s ]\n"           ,UFC::AnsiString( FCMID,          4 ).c_str());
    SocketPrintf("[ Match Time %s ]\n"      ,UFC::AnsiString( FMatchTime,     8 ).c_str());
    SocketPrintf("[ Seq No %s ]\n"          ,UFC::AnsiString( FSeqNo,         8 ).c_str());
    SocketPrintf("[ Product1 Type %c ]\n"   ,FProd1Type );
    SocketPrintf("[ Product2 Type %c ]\n"   ,FProd2Type );
}
//----------------------------------------------------------------------------------------------------------------------
// CM Confirm Order message C032
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC032::MessageC032( )
:MessageHeader( "50", "11", "03" ),
 FMsgCount( 0 )
{
    memset( FMessages, 0, sizeof( C03XBody* )* 3 );
}
//----------------------------------------------------------------------------------------------------------------------
MessageC032::MessageC032( char* Data )
:MessageHeader( Data )
{
    char* Buffer = Data + MESSAGE_HEADER_SIZE;
    int   DataOffset;

    memset( FMessages, 0, sizeof( C03XBody* )* 3 ); // Set message pointers to NULL.
    memcpy( FBrokerID, Buffer, 4 );                 // Get the BrokerID from given string.
    Buffer += 4;
    FMsgCount = UFC::AnsiString( Buffer, 2 ).ToInt();    // Get the message count from given string.
    Buffer += 2;
    for( int i = 0; i < FMsgCount; i++ ) // Create attached messages.
    {
        FMessages[i] = C03XBody::CreateFromString( Buffer, DataOffset, '8' );
        Buffer += DataOffset;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL MessageC032::AddMessage( C03XBody* NewMessage )
{
    if( FMsgCount > 2 )
        return FALSE;
    else if( FMsgCount == 2 )
    {
        FMessages[ 2 ] = NewMessage;
        FMsgCount++;
        return FALSE;
    }
    else
    {
        FMessages[ FMsgCount ] = NewMessage;
        FMsgCount++;
        return TRUE;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageC032::~MessageC032( )
{
    for( int i = 0; i < FMsgCount; i++ )
             delete FMessages[ i ];
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC032::AsString( char* Buffer )
{
    char* StrBuffer = Buffer;
    int   Offset;

    MessageHeader::AsString( StrBuffer );
    StrBuffer = Buffer + strlen( Buffer );
    strncpy( StrBuffer, FBrokerID, 4 );
    StrBuffer += 4;
    sprintf( StrBuffer,"%02d",FMsgCount );
    StrBuffer += 2;
    for( int i = 0; i < FMsgCount; i++ )
    {
            Offset = FMessages[ i ]->AsString( StrBuffer );
            StrBuffer += Offset;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageC032::print( void )
{
     char Buffer[256];
     SocketPrintf("---------------- Confirm Order Message C032 -----------------\n" );
     SocketPrintf("[ Broker_ID %s ]\n", UFC::AnsiString( FBrokerID, 4).c_str());
     SocketPrintf("[ Message_Count %02d ]\n", FMsgCount );
     for( int i = 0; i < FMsgCount; i++ )
     {
              FMessages[ i ]->AsString( Buffer );
              printf( "Report %d:%s\n",i+1, Buffer );
     }
     SocketPrintf("-------------------------- End of C030 ------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//   Clear Member Confirm order report message body.
//----------------------------------------------------------------------------------------------------------------------
C032OrderBody::C032OrderBody( char* Data, int& Offset )
:C03XBody( '1' )
{
    MemoryStreamReader MemReader( Data  );
    MemReader.Read( &FRecordType,   1 );
    MemReader.Read( FBroker,        7 );
    MemReader.Read( FOrderNo,       5 );
    MemReader.Read( FInvestorACNo,  7 );
    MemReader.Read( &FInvestorFlag, 1 );
    MemReader.Read( FProdID,       20 );
    MemReader.Read( &FBuySellCode,  1 );
    MemReader.Read( &FOrderType,    1 );
    MemReader.Read( FPrice,         9 );
    MemReader.Read( FQuntity,       4 );
    MemReader.Read( &FCondition,    1 );
    MemReader.Read( &FOCCode,       1 );
    MemReader.Read( &FTransCode,    1 );
    MemReader.Read( FBeforeQuntity, 4 );
    MemReader.Read( FAfterQuntity,  4 );
    MemReader.Read( FReceiveTime,   8 );
    MemReader.Read( FSeqNo,         8 );
    Offset = MemReader.GetPosition();
    FProcessingTime = UFC::AnsiString( FReceiveTime, 8 ).ToInt();
}
//----------------------------------------------------------------------------------------------------------------------
#ifndef _FOR_FCM
C032OrderBody::C032OrderBody(  RPTLOG* rpt  )
:C03XBody( '1' )
{
    char buf[ 10 ];

    memcpy( FBroker, rpt->IDR.FCM_NO, 7 );
    memcpy( FOrderNo, rpt->IDR.ORDER_NO, 5);
    memcpy( FInvestorACNo, rpt->IDR.ACC_NO, 7);
    FInvestorFlag = rpt->IDR.ACC_CODE;
    memcpy( FProdID, rpt->IDR.PROD_ID, 20 );
    FBuySellCode = rpt->IDR.BS_CODE;
    FOrderType   = rpt->IDR.TYPE;
    sprintf( buf, "%09d", rpt->IDR.PRICE );
    memcpy( FPrice, buf, 9 );
    sprintf( buf, "%04d", rpt->IDR.QNTY );
    memcpy( FQuntity, buf, 4);
    FCondition = rpt->IDR.STAGE_CODE; ///STAGE_CODE as OrderCondition in SimTFX
    FOCCode = rpt->IDR.OC_CODE;
    if( rpt->IDR.LINE_TAG == 'I')
        FTransCode = '1';
    else if( rpt->IDR.LINE_TAG == 'R' )
        FTransCode = '2';
    else
        FTransCode = '3';
    sprintf( buf, "%04d", rpt->IDR.BEFORE_QNTY );
    memcpy( FBeforeQuntity, buf, 4 );
    sprintf( buf, "%04d", rpt->IDR.AFTER_QNTY );
    memcpy( FAfterQuntity, buf, 4 );
    MillIntToHHMMSSMMString( buf, rpt->MATCH_TIME );
    memcpy( FReceiveTime, buf, 8);
    sprintf( buf, "%08d", rpt->CM_SEQ );
    memcpy( FSeqNo, buf, 8 );
    FProcessingTime = UFC::AnsiString( FReceiveTime, 8 ).ToInt();
}
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int C032OrderBody::AsString( char* Buffer )
{
	MemoryStreamWriter MemWriter( Buffer );
	MemWriter.Write( &FRecordType,   1 );
	MemWriter.Write( FBroker,        7 );
	MemWriter.Write( FOrderNo,       5 );
	MemWriter.Write( FInvestorACNo,  7 );
	MemWriter.Write( &FInvestorFlag, 1 );
	MemWriter.Write( FProdID,       20 );
	MemWriter.Write( &FBuySellCode,  1 );
	MemWriter.Write( &FOrderType,    1 );
	MemWriter.Write( FPrice,         9 );
	MemWriter.Write( FQuntity,       4 );
	MemWriter.Write( &FCondition,    1 );
	MemWriter.Write( &FOCCode,       1 );
	MemWriter.Write( &FTransCode,    1 );
	MemWriter.Write( FBeforeQuntity, 4 );
	MemWriter.Write( FAfterQuntity,  4 );
	MemWriter.Write( FReceiveTime,   8 );
	MemWriter.Write( FSeqNo,         8 );
	MemWriter.AppendZero();
	return MemWriter.GetPosition() - 1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void C032OrderBody::print( void )
{
	SocketPrintf("[ Record Type %c ]\n"     ,FRecordType );
	SocketPrintf("[ Broker ID %s ]\n"       ,UFC::AnsiString( FBroker,        7 ).c_str());
	SocketPrintf("[ Order No %s ]\n"        ,UFC::AnsiString( FOrderNo,       5 ).c_str());
	SocketPrintf("[ Investor AC No %s ]\n"  ,UFC::AnsiString( FInvestorACNo,  7 ).c_str());
	SocketPrintf("[ Investor Flag %c ]\n"   ,FInvestorFlag );
	SocketPrintf("[ Product ID %s ]\n"      ,UFC::AnsiString( FProdID,       20 ).c_str());
	SocketPrintf("[ Buy Sell Code1 %c ]\n"  ,FBuySellCode );
	SocketPrintf("[ Order Type %c ]\n"      ,FOrderType );
	SocketPrintf("[ Price %s ]\n"           ,UFC::AnsiString( FPrice,         9 ).c_str());
	SocketPrintf("[ Quntity %s ]\n"         ,UFC::AnsiString( FQuntity,       4 ).c_str());
	SocketPrintf("[ Condition %c ]\n"       ,FCondition );
	SocketPrintf("[ OC Code %c ]\n"         ,FOCCode );
	SocketPrintf("[ TransCode %c ]\n"       ,FTransCode );
	SocketPrintf("[ Before Quntity %s ]\n"  ,UFC::AnsiString( FBeforeQuntity, 4 ).c_str());
	SocketPrintf("[ After Quntity %s ]\n"   ,UFC::AnsiString( FAfterQuntity,  4 ).c_str());
	SocketPrintf("[ Receive Time %s ]\n"    ,UFC::AnsiString( FReceiveTime,   8 ).c_str());
	SocketPrintf("[ Seq No %s ]\n"          ,UFC::AnsiString( FSeqNo,         8 ).c_str());
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//   Clear Member Confirm quote report message body.
//----------------------------------------------------------------------------------------------------------------------
C032QuoteBody::C032QuoteBody( char* Data, int& Offset )
:C03XBody( '2' )
{
	MemoryStreamReader MemReader( Data  );
	MemReader.Read( &FRecordType,    1 );
	MemReader.Read( FBroker,         7 );
	MemReader.Read( FOrderNo,        5 );
	MemReader.Read( FInvestorACNo,   7 );
	MemReader.Read( &FInvestorFlag,  1 );
	MemReader.Read( FProdID,        20 );
	MemReader.Read( &FBuySellCode,   1 );
	MemReader.Read( FBidOrderPrice,  9 );
	MemReader.Read( FAskOrderPrice,  9 );
	MemReader.Read( FBidOrderQuntity,4 );
	MemReader.Read( FAskOrderQuntity,4 );
	MemReader.Read( &FOrderCondition,1 );
	MemReader.Read( &FOCCode,        1 );
	MemReader.Read( &FTransCode,     1 );
	MemReader.Read( FBeforeQuntity,  4 );
	MemReader.Read( FAfterQuntity,   4 );
	MemReader.Read( FReceiveTime,    8 );
	MemReader.Read( FSeqNo,          8 );
	Offset = MemReader.GetPosition();
    FProcessingTime = UFC::AnsiString( FReceiveTime, 8 ).ToInt();
}
//----------------------------------------------------------------------------------------------------------------------
#ifndef _FOR_FCM
C032QuoteBody::C032QuoteBody(  RPTLOG* rpt  )
:C03XBody( '2' )
{
    char buf[ 10 ];

    memcpy( FBroker, rpt->IDR.FCM_NO, 7 );
    memcpy( FOrderNo, rpt->IDR.ORDER_NO, 5);
    memcpy( FInvestorACNo, rpt->IDR.ACC_NO, 7);
    FInvestorFlag = rpt->IDR.ACC_CODE;
    memcpy( FProdID, rpt->IDR.PROD_ID, 20 );
    FBuySellCode = rpt->IDR.BS_CODE;
    sprintf( buf, "%09d", rpt->IDR.PRICE );
    memcpy( FBidOrderPrice, buf, 9 );
    sprintf( buf, "%09d", rpt->IDR.PRICE_2 );///< For SimTFX
    memcpy( FAskOrderPrice, buf, 9 );
    sprintf( buf, "%04d", rpt->IDR.QNTY );
    memcpy( FBidOrderQuntity, buf, 4);
    sprintf( buf, "%04d", rpt->IDR.QNTY_2 );///< For SimTFX
    memcpy( FBidOrderQuntity, buf, 4);
    FOrderCondition = rpt->IDR.STAGE_CODE; ///STAGE_CODE as OrderCondition in SimTFX
    FOCCode = rpt->IDR.OC_CODE;
    if( rpt->IDR.LINE_TAG == 'I')
        FTransCode = '1';
    else if( rpt->IDR.LINE_TAG == 'R' )
        FTransCode = '2';
    else
        FTransCode = '3';
    sprintf( buf, "%04d", rpt->IDR.BEFORE_QNTY );
    memcpy( FBeforeQuntity, buf, 4 );
    sprintf( buf, "%04d", rpt->IDR.AFTER_QNTY );
    memcpy( FAfterQuntity, buf, 4 );
    MillIntToHHMMSSMMString( buf, rpt->MATCH_TIME );
    memcpy( FReceiveTime, buf, 8);
    sprintf( buf, "%08d", rpt->CM_SEQ );
    memcpy( FSeqNo, buf, 8 );
    FProcessingTime = UFC::AnsiString( FReceiveTime, 8 ).ToInt();
}
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int C032QuoteBody::AsString( char* Buffer )
{
	MemoryStreamWriter MemWriter( Buffer );
	MemWriter.Write( &FRecordType,    1 );
	MemWriter.Write( FBroker,         7 );
	MemWriter.Write( FOrderNo,        5 );
	MemWriter.Write( FInvestorACNo,   7 );
	MemWriter.Write( &FInvestorFlag,  1 );
	MemWriter.Write( FProdID,        20 );
	MemWriter.Write( &FBuySellCode,   1 );
	MemWriter.Write( FBidOrderPrice,  9 );
	MemWriter.Write( FAskOrderPrice,  9 );
	MemWriter.Write( FBidOrderQuntity,4 );
	MemWriter.Write( FAskOrderQuntity,4 );
	MemWriter.Write( &FOrderCondition,1 );
	MemWriter.Write( &FOCCode,        1 );
	MemWriter.Write( &FTransCode,     1 );
	MemWriter.Write( FBeforeQuntity,  4 );
	MemWriter.Write( FAfterQuntity,   4 );
	MemWriter.Write( FReceiveTime,    8 );
	MemWriter.Write( FSeqNo,          8 );
	MemWriter.AppendZero();
	return MemWriter.GetPosition() - 1;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void C032QuoteBody::print( void )
{
	SocketPrintf("[ Record Type %c ]\n"      ,FRecordType );
	SocketPrintf("[ Broker ID %s ]\n"        ,UFC::AnsiString( FBroker,        7 ).c_str());
	SocketPrintf("[ Order No %s ]\n"         ,UFC::AnsiString( FOrderNo,       5 ).c_str());
	SocketPrintf("[ Investor AC No %s ]\n"   ,UFC::AnsiString( FInvestorACNo,  7 ).c_str());
	SocketPrintf("[ Investor Flag %c ]\n"    ,FInvestorFlag );
	SocketPrintf("[ Product ID %s ]\n"       ,UFC::AnsiString( FProdID,       20 ).c_str());
	SocketPrintf("[ Buy Sell Code1 %c ]\n"   ,FBuySellCode );
	SocketPrintf("[ Bid Order Price %s ]\n"  ,UFC::AnsiString( FBidOrderPrice,         9 ).c_str());
	SocketPrintf("[ Ask Order Price %s ]\n"  ,UFC::AnsiString( FAskOrderPrice,         9 ).c_str());
	SocketPrintf("[ Bid Order Quntity %s ]\n",UFC::AnsiString( FBidOrderQuntity,       4 ).c_str());
	SocketPrintf("[ Ask Order Quntity %s ]\n",UFC::AnsiString( FAskOrderQuntity,       4 ).c_str());
	SocketPrintf("[ Condition %c ]\n"        ,FOrderCondition );
	SocketPrintf("[ OC Code %c ]\n"          ,FOCCode );
	SocketPrintf("[ TransCode %c ]\n"        ,FTransCode );
	SocketPrintf("[ Before Quntity %s ]\n"   ,UFC::AnsiString( FBeforeQuntity, 4 ).c_str());
	SocketPrintf("[ After Quntity %s ]\n"    ,UFC::AnsiString( FAfterQuntity,  4 ).c_str());
	SocketPrintf("[ Receive Time %s ]\n"     ,UFC::AnsiString( FReceiveTime,   8 ).c_str());
	SocketPrintf("[ Seq No %s ]\n"           ,UFC::AnsiString( FSeqNo,         8 ).c_str());
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
