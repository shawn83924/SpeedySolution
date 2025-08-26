#include "O1Messages.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetOddExTransactionSubsystemMessage( int FuncCode, int MsgType )
{   ///< Function code 1 buy, 2 Sell, 3 Reduce, 4 Cancel, 5 Query
    switch( MsgType )
    {
        case 0: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5  || FuncCode == 6 )
                    return mtO110;
                else
                    return mtUnknown;
        case 1: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5  || FuncCode == 6 )
                    return mtO120;
                else
                    return mtUnknown;
        case 2: if( FuncCode == 0 )
                    return mtO140;
                else
                    return mtUnknown;
        case 3: if( FuncCode == 0 || FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 || FuncCode == 4 || FuncCode == 5  || FuncCode == 6 )
                    return mtO130;
                else
                    return mtUnknown;
        case 4: if( FuncCode == 0 )
                    return mtO160;
                else
                    return mtUnknown;
        case 5: if( FuncCode == 0 )
                    return mtO150;
                else
                    return mtUnknown;
        default: return mtUnknown;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageO110::MessageO110( BOOL IsTWSE, const char* FunctionCode )
:MessageHeader((char*)((IsTWSE)?"33":"83"),FunctionCode,"00")
,FQty( 0 )
,FPx( 0.0 )
{
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageO110::MessageO110( BOOL IsTWSE, char* FunctionCode,char* Body )
:MessageHeader((char*)((IsTWSE)?"33":"83"),FunctionCode,"00")
,FQty( 0 )
,FPx( 0.0 )
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageO110::MessageO110( const char* Buffer )
:MessageHeader( Buffer )
,FQty( 0 )
,FPx( 0.0 )
{
    LoadBody( Buffer  + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageO110::GetOrderQty( void )
{
    if( FQty == 0 )
        FQty = atoi( OrderQty );
    return FQty;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double MessageO110::GetOrderPrice( void )
{
    if( FPx == 0.0 )
        FPx = ((double)atoi( OrderPrice ))/10000.0;
    return FPx;    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO110::SetPrice( double Px )
{
    snprintf( OrderPrice, 10, "%09d", (int)((Px +0.00001) * 10000.0) );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO110::SetQty( int Qty )
{
   snprintf( OrderQty, 7, "%06d", Qty );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO110::LoadBody( const char *Body )
{
    MemoryStreamReader MemReader( Body );
    
    MemReader.ReadString( BrokerID, 4 );     /* Broker ID */
    MemReader.ReadString( PVCID, 2 );        /* PVC ID */
    MemReader.ReadString( OrderNo, 5 );      /* order # */
    MemReader.ReadString( InvestorAcno, 7);  /* investor account # */
    MemReader.Read( &InvestorFlag, 1 );      /* investor flag */
    MemReader.ReadString( StockID, 6 );      /* Stock ID */
    MemReader.ReadString( OrderPrice, 9 );   /* order price */
    MemReader.ReadString( OrderQty,  6 );    /* quantity */
    MemReader.Read( &BuySellCode,1);         /* B/S code */
    MemReader.Read( &ExchangeCode,1);        /* Exchange Code */
    MemReader.Read( &TWSEOrderType,1);       /* TWSEorder type */
    MemReader.Read( &OrderType,1);           /* order type */
    MemReader.Read( &TimeInForce,1);         /* Time in force */

    for( register int i = 0; i < 7; i++ )
    {
        if( StockID[i]==0 || StockID[i]==' '  )
        {
            Symbol[i] = 0;
            break;
        }
        else
            Symbol[i] = StockID[i];
    }    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO110::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( BrokerID, 4 );     /* Broker ID */
    MemWriter.Write( PVCID,    2 );     /* PVC ID */
    MemWriter.Write( OrderNo,  5 );     /* order # */
    MemWriter.Write( InvestorAcno,7);   /* investor account # */
    MemWriter.Write( &InvestorFlag,1);  /* investor flag */
    MemWriter.Write( StockID,   6 );    /* Stock ID */
    MemWriter.Write( OrderPrice,9 );    /* order price */
    MemWriter.Write( OrderQty,  6 );    /* quantity */
    MemWriter.Write( &BuySellCode,1);   /* B/S code */
    MemWriter.Write( &ExchangeCode,1);
    MemWriter.Write( &TWSEOrderType,1); /* TWSE order type */
    MemWriter.Write( &OrderType,1);     /* order type */
    MemWriter.Write( &TimeInForce,1);   /* Time in force */
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO110::print( void )
{
   
    SocketPrintf("---------------------- Order message O110 ---------------------\n" );
    SocketPrintf("[ Broker-ID %s ]\n",BrokerID);
    SocketPrintf("[ PVC-ID %s ]\n",PVCID);
    SocketPrintf("[ Order_No %s ]\n",OrderNo);
    SocketPrintf("[ Inv_acno %s ]\n",InvestorAcno);
    SocketPrintf("[ Inv_flag %c ]\n",InvestorFlag );
    SocketPrintf("[ Prod_ID %s ]\n",StockID);
    SocketPrintf("[ Order_Price %s ]\n",OrderPrice);
    SocketPrintf("[ QNTY %s ]\n",OrderQty);
    SocketPrintf("[ B/S Code %c ]\n",BuySellCode );
    SocketPrintf("[ Exchange Code %c ]\n",ExchangeCode );
    SocketPrintf("[ TWSE Order_Type %c ]\n",TWSEOrderType );
    SocketPrintf("[ Order_Type %c ]\n",OrderType );
    SocketPrintf("[ TimeInForce %c ]\n",TimeInForce );
    SocketPrintf("-------------------------- End of O110 ------------------------\n");    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class MessageT020
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageO120::MessageO120( const char *Data )
:MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageO120::MessageO120( BOOL IsTWSE, char* Func, char *Body )
:MessageHeader( (char*)((IsTWSE)?"33":"83"),Func,"01" )
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO120::SetPrice( double Px )
{
    snprintf( OrderPrice, 10, "%09d", (int)((Px +0.00001) * 10000.0) );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO120::SetQty( int Qty )
{
    snprintf( OrderQty, 7, "%06d", Qty );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO120::SetBeforeQty( int Qty )
{
    snprintf( BeforeQty, 7, "%06d", Qty );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO120::SetAfterQty( int Qty )
{
    snprintf( AfterQty, 7, "%06d", Qty );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO120::SetDate( int YYYY, int MM, int DD )
{
    snprintf( OrderDate, 9, "%04d%02d%02d", YYYY, MM, DD);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO120::SetTime( int H, int M, int S, int mmm )
{    
    snprintf( OrderTime, 10, "%02d%02d%02d%03d", H,M,S, mmm );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO120::LoadBody( const char *Body )
{
    MemoryStreamReader MemReader( Body );
    
    MemReader.Read( BrokerID,4);
    MemReader.Read( PVCID, 2 );
    MemReader.Read( OrderNo,5);
    MemReader.Read( InvestorAcno,7);
    MemReader.Read(&InvestorFlag,1);
    MemReader.Read( StockID,6);    
    MemReader.Read( OrderPrice,9);
    MemReader.Read( OrderQty,6);
    MemReader.Read(&BuySellCode,1);
    MemReader.Read(&ExchangeCode,1);
    MemReader.Read(&TWSEOrderType,1);
    MemReader.Read(&OrderType,1);
    MemReader.Read(&TimeInForce,1);
    MemReader.Read( OrderDate,8);
    MemReader.Read( OrderTime,9);
    MemReader.Read( BeforeQty,6);
    MemReader.Read( AfterQty,6);        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO120::AsString( char* Buffer )
{        
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( BrokerID,4);
    MemWriter.Write( PVCID, 2 );
    MemWriter.Write( OrderNo,5);
    MemWriter.Write( InvestorAcno,7);
    MemWriter.Write(&InvestorFlag,1);
    MemWriter.Write( StockID,6);    
    MemWriter.Write( OrderPrice,9);
    MemWriter.Write( OrderQty,6);
    MemWriter.Write(&BuySellCode,1);
    MemWriter.Write(&ExchangeCode,1);
    MemWriter.Write(&TWSEOrderType,1);
    MemWriter.Write(&OrderType,1);
    MemWriter.Write(&TimeInForce,1);    
    MemWriter.Write( OrderDate,8);
    MemWriter.Write( OrderTime,9);
    MemWriter.Write( BeforeQty,6);
    MemWriter.Write( AfterQty,6);        
    MemWriter.AppendZero();    
}
//---------------------------------------------------------------------------
void MessageO120::print( void )
{   
    SocketPrintf("--------------- Accept order report message O120 ---------------\n" );
    SocketPrintf("[ Broker No %s ]\n",UFC::AnsiString(BrokerID,4).c_str());
    SocketPrintf("[ PVC-ID %s ]\n",UFC::AnsiString(PVCID,2).c_str());
    SocketPrintf("[ Order No %s ]\n",UFC::AnsiString(OrderNo,5).c_str());
    SocketPrintf("[ Inv_acno %s ]\n",UFC::AnsiString(InvestorAcno,7).c_str());
    SocketPrintf("[ Inv_flag %c ]\n",InvestorFlag );
    SocketPrintf("[ Stock ID %s ]\n",UFC::AnsiString(StockID,6).c_str());    
    SocketPrintf("[ Order Price %s ]\n",UFC::AnsiString(OrderPrice,9).c_str());
    SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,6).c_str());
    SocketPrintf("[ B/S Code %c ]\n",BuySellCode);
    SocketPrintf("[ Exchange Code %c ]\n",ExchangeCode);
    SocketPrintf("[ TWSE Order Type %c ]\n",TWSEOrderType);
    SocketPrintf("[ Order Type %c ]\n",OrderType);
    SocketPrintf("[ Time In Force %c ]\n",TimeInForce);
    SocketPrintf("[ Order Date %s ]\n",UFC::AnsiString(OrderDate,8).c_str());
    SocketPrintf("[ Order Time %s ]\n",UFC::AnsiString(OrderTime,9).c_str());
    SocketPrintf("[ Before_QNTY %s ]\n",UFC::AnsiString(BeforeQty,6).c_str());
    SocketPrintf("[ After_QNTY %s ]\n",UFC::AnsiString(AfterQty,6).c_str());    
    SocketPrintf("----------------------- End of O120 ---------------------------\n");    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO130::print( void )
{
    char TmpStr[3];
    strncpynt( TmpStr, GetStatusCodeStr(), 2 );
    SocketPrintf("----- Error Order message O130 code: %s -----------------\n",TmpStr );
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
