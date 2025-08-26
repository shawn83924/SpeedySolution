#include "EMessages.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetTenderTransactionSubsystemMessage( int FuncCode, int MsgType )
{   ///< Function code 1 buy, 2 Cancel, 3 Reduce, 4 Query
    switch( MsgType )
    {
        case 0: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 )
                    return mtE010;
	case 1: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 )
                    return mtE020;				
	case 2: if( FuncCode == 0 )
                    return mtE040;
	case 3: if( FuncCode == 0 || FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 || FuncCode == 4 )
        	    return mtE030;
	case 4: if( FuncCode == 0 )
                    return mtE060;
	case 5: if( FuncCode == 0 )
                    return mtE050;        
	default: return mtUnknown;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageE010::MessageE010( BOOL IsTWSE,  char* FunctionCode,char* Body )
:MessageHeader((char*)((IsTWSE)?"41":"97"),FunctionCode,"00")
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageE010::MessageE010( const char* Buffer )
:MessageHeader( Buffer )
{
    LoadBody( Buffer  + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE010::LoadBody( const char *Body )
{
    MemoryStreamReader MemReader( Body );
    MemReader.Read( BrokerID, 4 );     /* Broker ID */
    MemReader.Read( PVCID, 2 );        /* PVC ID */
    MemReader.Read( OrderNo, 5 );      /* order # */
    MemReader.Read( InvestorAcno,7);   /* investor account # */
    MemReader.Read( StockID,   6 );    /* Stock ID */
    MemReader.Read( StkSeqNo,  2 );    /* Stock ID */    
    if( MessageHeader::FNewExVersion == TRUE )
        MemReader.Read( OrderPrice,9 );    /* order price */
    else
        MemReader.Read( OrderPrice,6 );    /* order price */
    MemReader.Read( OrderQty,  12 );   /* quantity */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE010::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( BrokerID, 4 );     /* Broker ID */
    MemWriter.Write( PVCID,    2 );     /* PVC ID */
    MemWriter.Write( OrderNo,  5 );     /* order # */
    MemWriter.Write( InvestorAcno,7);   /* investor account # */
    MemWriter.Write( StockID,   6 );    /* Stock ID */
    MemWriter.Write( StkSeqNo,  2 );       
    if( MessageHeader::FNewExVersion == TRUE )
        MemWriter.Write( OrderPrice,9 );    /* order price */
    else
        MemWriter.Write( OrderPrice,6 );    /* order price */
    MemWriter.Write( OrderQty,  12 );   /* quantity */
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double MessageE010::GetOrderPrice( void )
{
    return ((double)UFC::AnsiString::StrToInt32( OrderPrice, 9 ))/10000.0;    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int64 MessageE010::GetOrderQty( void )
{
    UFC::AnsiString Qty( OrderQty, 12 );
    return  Qty.ToInt64();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE010::print( void )
{
    SocketPrintf("---------------------- Order message V010 ---------------------\n" );
    SocketPrintf("[ Broker-ID %s ]\n",UFC::AnsiString(BrokerID,4).c_str());
    SocketPrintf("[ PVC-ID %s ]\n",UFC::AnsiString(PVCID,2).c_str());
    SocketPrintf("[ Order_No %s ]\n",UFC::AnsiString(OrderNo,5).c_str());
    SocketPrintf("[ Inv_acno %s ]\n",UFC::AnsiString(InvestorAcno,7).c_str());
    SocketPrintf("[ Prod_ID %s ]\n",UFC::AnsiString(StockID,6).c_str());
    SocketPrintf("[ StkSeqNo %s ]\n",UFC::AnsiString(StkSeqNo,2).c_str());    
    if( MessageHeader::FNewExVersion == TRUE )
        SocketPrintf("[ Order_Price %s ]\n",UFC::AnsiString(OrderPrice,9).c_str());
    else
        SocketPrintf("[ Order_Price %s ]\n",UFC::AnsiString(OrderPrice,6).c_str());
    SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,12).c_str());
    SocketPrintf("-------------------------- End of V010 ------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class MessageE020
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageE020::MessageE020( const char *Data )
:MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageE020::MessageE020( BOOL IsTWSE, char* Func, char *Body )
:MessageHeader( (char*)((IsTWSE)?"41":"97"),Func,"01" )
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE020::LoadBody( const char *Body )
{
    MemoryStreamReader MemReader( Body );
    MemReader.Read( BrokerID,4);
    MemReader.Read( PVCID, 2 );
    MemReader.Read( OrderNo,5);
    MemReader.Read( InvestorAcno,7);
    MemReader.Read( StockID,6);
    MemReader.Read( StkSeqNo, 2 );
    if( MessageHeader::FNewExVersion == TRUE )
    {
        MemReader.Read( OrderPrice,9);
        MemReader.Read( OrderQty,12 );
        MemReader.Read( OrderDate,8);
        MemReader.Read( OrderTime,8);
        MemReader.Read( BeforeQty,12);
        MemReader.Read( AfterQty,12);    
        MemReader.Read( BeforePrice,9);
        MemReader.Read( AfterPrice,9);        
    }
    else
    {
        MemReader.Read( OrderPrice,6);
        MemReader.Read( OrderQty,12 );
        MemReader.Read( OrderDate,6);
        MemReader.Read( OrderTime,6);
        MemReader.Read( BeforeQty,12);
        MemReader.Read( AfterQty,12);    
        MemReader.Read( BeforePrice,6);
        MemReader.Read( AfterPrice,6);        
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE020::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( BrokerID,4);
    MemWriter.Write( PVCID, 2 );
    MemWriter.Write( OrderNo,5);
    MemWriter.Write( InvestorAcno,7);
    MemWriter.Write( StockID,6);
    MemWriter.Write( StkSeqNo, 2 );   
    if( MessageHeader::FNewExVersion == TRUE )
        {
        MemWriter.Write( OrderPrice,9);
        MemWriter.Write( OrderQty,12);    
        MemWriter.Write( OrderDate,8);
        MemWriter.Write( OrderTime,8);
        MemWriter.Write( BeforeQty,12);
        MemWriter.Write( AfterQty,12);    
        MemWriter.Write( BeforePrice,9);
        MemWriter.Write( AfterPrice,9);            
    }
    else
    {
        MemWriter.Write( OrderPrice,6);
        MemWriter.Write( OrderQty,12);    
        MemWriter.Write( OrderDate,6);
        MemWriter.Write( OrderTime,6);
        MemWriter.Write( BeforeQty,12);
        MemWriter.Write( AfterQty,12);    
        MemWriter.Write( BeforePrice,6);
        MemWriter.Write( AfterPrice,6);            
    }
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE020::SetPrice( double Px )
{
    char buffer[16];
    
    snprintf( buffer, 10, "%09d", (int)((Px +0.00001) * 10000.0) );
    memcpy( OrderPrice, buffer, 9 );
    memcpy( BeforePrice, buffer, 9 );
    memcpy( AfterPrice, buffer, 9 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE020::SetQty( Int64 Qty )
{
    char buffer[16];
    snprintf( buffer, 13, "%012lld", Qty );
    memcpy( OrderQty, buffer, 12);    
}    
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE020::SetBeforeQty( Int64 Qty )
{
    char buffer[16];
    snprintf( buffer, 13, "%012lld", Qty );
    memcpy( BeforeQty, buffer, 12);        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE020::SetAfterQty( Int64 Qty )
{
    char buffer[16];
    snprintf( buffer, 13, "%012lld", Qty );
    memcpy( AfterQty, buffer, 12);            
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE020::SetDate( int YYYY, int MM, int DD )
{
    char buffer[16];
    snprintf( buffer, 9, "%04d%02d%02d", YYYY, MM, DD);
    memcpy( OrderDate, buffer, 8);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE020::SetTime( int H, int M, int S, int mmm )
{
    char buffer[16];
    
    snprintf( buffer, 9, "%02d%02d%02d%02d", (UInt16)H%100,(UInt16)M%100,(UInt16)S%100, (UInt16)(mmm/10)%100 );    
    memcpy( OrderTime, buffer, 8);
}
//---------------------------------------------------------------------------
void MessageE020::print( void )
{
    SocketPrintf("--------------- Accept order report mesage E020 ---------------\n" );
    SocketPrintf("[ Broker No %s ]\n",UFC::AnsiString(BrokerID,4).c_str());
    SocketPrintf("[ PVC-ID %s ]\n",UFC::AnsiString(PVCID,2).c_str());
    SocketPrintf("[ Order No %s ]\n",UFC::AnsiString(OrderNo,5).c_str());
    SocketPrintf("[ Inv_acno %s ]\n",UFC::AnsiString(InvestorAcno,7).c_str());
    SocketPrintf("[ Stock ID %s ]\n",UFC::AnsiString(StockID,6).c_str());
    SocketPrintf("[ StkSeqNo %s ]\n",UFC::AnsiString(StkSeqNo,2).c_str());        
    if( MessageHeader::FNewExVersion == TRUE )
    {
        SocketPrintf("[ Order Price %s ]\n",UFC::AnsiString(OrderPrice,9).c_str());
        SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,12).c_str());
        SocketPrintf("[ Order Date %s ]\n",UFC::AnsiString(OrderDate,8).c_str());
        SocketPrintf("[ Order Time %s ]\n",UFC::AnsiString(OrderTime,8).c_str());
        SocketPrintf("[ Before_QNTY %s ]\n",UFC::AnsiString(BeforeQty,12).c_str());
        SocketPrintf("[ After_QNTY %s ]\n",UFC::AnsiString(AfterQty,12).c_str());
        SocketPrintf("[ Before_Price %s ]\n",UFC::AnsiString(BeforePrice,9).c_str());
        SocketPrintf("[ After_Price %s ]\n",UFC::AnsiString(AfterPrice,9).c_str());        
    }
    else
    {
        SocketPrintf("[ Order Price %s ]\n",UFC::AnsiString(OrderPrice,6).c_str());
        SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,12).c_str());
        SocketPrintf("[ Order Date %s ]\n",UFC::AnsiString(OrderDate,6).c_str());
        SocketPrintf("[ Order Time %s ]\n",UFC::AnsiString(OrderTime,6).c_str());
        SocketPrintf("[ Before_QNTY %s ]\n",UFC::AnsiString(BeforeQty,12).c_str());
        SocketPrintf("[ After_QNTY %s ]\n",UFC::AnsiString(AfterQty,12).c_str());
        SocketPrintf("[ Before_Price %s ]\n",UFC::AnsiString(BeforePrice,6).c_str());
        SocketPrintf("[ After_Price %s ]\n",UFC::AnsiString(AfterPrice,6).c_str());        
    }
    SocketPrintf("----------------------- End of E020 ---------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageE030::print( void )
{
    char TmpStr[3];
    strncpynt( TmpStr, GetStatusCodeStr(), 2 );
    SocketPrintf("----- Error Order message E030 code: %s -----------------\n",TmpStr );
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
