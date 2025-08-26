#include "OMessages.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetOddTransactionSubsystemMessage( int FuncCode, int MsgType )
{   ///< Function code 1 buy, 2 Sell, 3 Reduce, 4 Cancel, 5 Query
    switch( MsgType )
    {
        case 0: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5 )
                    return mtO010;
        case 1: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5 )
                    return mtO020;
        case 2: if( FuncCode == 0 )
                    return mtO040;
        case 3: if( FuncCode == 0 || FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 || FuncCode == 4 || FuncCode == 5 )
                    return mtT030;	
        case 4: if( FuncCode == 0 )
                    return mtO070;
        case 6: if( FuncCode == 0 )
                    return mtO090;
        case 7: if( FuncCode == 0 )
                    return mtO060;
        case 9: if( FuncCode == 0 )
                    return mtO080;
        default: return mtUnknown;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageO010::MessageO010( BOOL IsTWSE, const char* FunctionCode )
:MessageHeader((char*)((IsTWSE)?"40":"94"),FunctionCode,"00")
,FQty( 0 )
,FPx( 0.0 )
{    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageO010::MessageO010( BOOL IsTWSE, char* FunctionCode,char* Body )
:MessageHeader((char*)((IsTWSE)?"40":"94"),FunctionCode,"00")
,FQty( 0 )
,FPx( 0.0 )
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageO010::MessageO010( const char* Buffer )
:MessageHeader( Buffer )
,FQty( 0 )
,FPx( 0.0 )
{
    LoadBody( Buffer  + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageO010::GetOrderQty( void )
{
    if( FQty == 0 )
        FQty = atoi( OrderQty );
    return FQty;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double MessageO010::GetOrderPrice( void )
{
    if( FPx == 0.0 )
    {
        if( MessageHeader::FNewVersion == TRUE )
            FPx = ((double)atoi( OrderPrice ))/10000.0;
        else
            FPx = ((double)atoi( OrderPrice ))/100.0;
    }
    return FPx;        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO010::SetPrice( double Px )
{
    snprintf( OrderPrice, 10, "%09d", (int)((Px +0.00001) * 10000.0) );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO010::SetQty( int Qty )
{
   snprintf( OrderQty, 7, "%06d", Qty );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO010::LoadBody( const char *Body )
{
    MemoryStreamReader MemReader( Body );
    
    MemReader.ReadString( BrokerID, 4 );     /* Broker ID */
    MemReader.ReadString( PVCID, 2 );        /* PVC ID */
    MemReader.ReadString( OrderNo, 5 );      /* order # */
    MemReader.ReadString( InvestorAcno, 7);  /* investor account # */        
    if( MessageHeader::FNewVersion == TRUE )
    {
        MemReader.Read( &InvestorFlag, 1 );      /* investor flag */
        MemReader.ReadString( StockID, 6 );      /* Stock ID */        
        MemReader.ReadString( OrderPrice, 9 );   /* order price */
        MemReader.ReadString( OrderQty,  6 );    /* quantity */
        MemReader.Read( &BuySellCode,1);         /* B/S code */
        MemReader.Read( &ExchangeCode,1);        /* Exchange Code */
        MemReader.Read( &TWSEOrderType,1);       /* TWSEorder type */
        MemReader.Read( &OrderType,1);           /* order type */
        MemReader.Read( &TimeInForce,1);         /* Time in force */
    }   
    else
    {
        MemReader.ReadString( StockID,   6 );    /* Stock ID */
        MemReader.ReadString( OrderPrice,6 );    /* order price */
        MemReader.ReadString( OrderQty,  8 );    /* quantity */
        MemReader.Read( &BuySellCode,1);         /* B/S code */
        MemReader.Read( &TradeKind,1); 
        MemReader.ReadString( ObjectBroker,4); 
    }
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
void MessageO010::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );    
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    
    MemWriter.Write( BrokerID, 4 );     /* Broker ID */
    MemWriter.Write( PVCID,    2 );     /* PVC ID */
    MemWriter.Write( OrderNo,  5 );     /* order # */
    MemWriter.Write( InvestorAcno,7);   /* investor account # */
    if( MessageHeader::FNewVersion == TRUE )
    {
        MemWriter.Write( &InvestorFlag,1);  /* investor flag */
        MemWriter.Write( StockID,   6 );    /* Stock ID */        
        MemWriter.Write( OrderPrice,9 );    /* order price */
        MemWriter.Write( OrderQty,  6 );    /* quantity */
        MemWriter.Write( &BuySellCode,1);   /* B/S code */
        MemWriter.Write( &ExchangeCode,1);
        MemWriter.Write( &TWSEOrderType,1); /* TWSE order type */
        MemWriter.Write( &OrderType,1);     /* order type */
        MemWriter.Write( &TimeInForce,1);   /* Time in force */
    }
    else
    {
        MemWriter.Write( StockID,   6 );    /* Stock ID */
        MemWriter.Write( OrderPrice,6 );    /* order price */
        MemWriter.Write( OrderQty,  8 );    /* quantity */
        MemWriter.Write( &BuySellCode,1);   /* B/S code */
        MemWriter.Write( &TradeKind,1);
        MemWriter.Write( ObjectBroker,4);     /* order type */
    }
    MemWriter.AppendZero();    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO010::print( void )
{
    SocketPrintf("---------------------- Order message O010 ---------------------\n" );
    SocketPrintf("[ Broker-ID %s ]\n", BrokerID );
    SocketPrintf("[ PVC-ID %s ]\n", PVCID );
    SocketPrintf("[ Order_No %s ]\n", OrderNo );
    SocketPrintf("[ Inv_acno %s ]\n", InvestorAcno );
    SocketPrintf("[ Prod_ID %s ]\n", StockID );
    SocketPrintf("[ Order_Price %s ]\n", OrderPrice );
    SocketPrintf("[ QNTY %s ]\n", OrderQty );
    SocketPrintf("[ B/S Code %c ]\n",BuySellCode );
    if( MessageHeader::FNewVersion == TRUE )
    {
        SocketPrintf("[ Inv_flag %c ]\n",InvestorFlag );        
        SocketPrintf("[ Order_Type %c ]\n",OrderType );
        SocketPrintf("[ TimeInForce %c ]\n",TimeInForce );
        SocketPrintf("[ Exchange Code %c ]\n",ExchangeCode );
        SocketPrintf("[ TWSE Order_Type %c ]\n",TWSEOrderType );        
    }    
    else
    {
        SocketPrintf("[ Trade Kind %c ]\n",TradeKind );
        SocketPrintf("[ Object Broker %s ]\n", ObjectBroker );
    }
    SocketPrintf("-------------------------- End of O010 ------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class MessageO020
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageO020::MessageO020( const char *Data )
:MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageO020::MessageO020( BOOL IsTWSE, char* Func, char *Body )
:MessageHeader( (char*)((IsTWSE)?"40":"94"),Func,"01" )
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO020::SetPrice( double Px )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderPrice, 10, "%09d", (int)((Px +0.00001) * 10000.0) );
    else
        snprintf( OrderPrice, 10, "%06d", (int)((Px +0.001) * 100.0) );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO020::SetQty( int Qty )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderQty, 9, "%06d", Qty );
    else
        snprintf( OrderQty, 9, "%08d", Qty );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO020::SetBeforeQty( int Qty )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( BeforeQty, 9, "%06d", Qty );
    else
        snprintf( BeforeQty, 9, "%08d", Qty );        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO020::SetAfterQty( int Qty )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( AfterQty, 9, "%06d", Qty );
    else
        snprintf( AfterQty, 9, "%08d", Qty );         
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO020::SetDate( int YYYY, int MM, int DD )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderDate, 9, "%04d%02d%02d", YYYY, MM, DD);
    else    
        snprintf( OrderDate, 9, "00%02d%02d", MM,DD );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO020::SetTime( int H, int M, int S, int mmm )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderTime, 10, "%02d%02d%02d%03d", H,M,S, mmm );
    else    
        snprintf( OrderTime, 10, "%02d%02d%02d", H,M,S );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO020::LoadBody( const char *Body )
{
    MemoryStreamReader MemReader( Body );
    
    MemReader.Read( BrokerID,4);
    MemReader.Read( PVCID, 2 );
    MemReader.Read( OrderNo,5);
    MemReader.Read( InvestorAcno,7);    
    if( MessageHeader::FNewVersion == TRUE )
    {
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
    else
    {
        MemReader.Read( StockID,6);
        MemReader.Read( OrderPrice,6);
        MemReader.Read( OrderQty,8);
        MemReader.Read(&BuySellCode,1);
        MemReader.Read(&TradeKind,1);
        MemReader.Read( ObjectBroker,4);
        MemReader.Read( OrderDate,6);
        MemReader.Read( OrderTime,6);
        MemReader.Read( BeforeQty,8);
        MemReader.Read( AfterQty,8);
    }    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO020::AsString( char* Buffer )
{   
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    
    MemWriter.Write( BrokerID,4);
    MemWriter.Write( PVCID, 2 );
    MemWriter.Write( OrderNo,5);
    MemWriter.Write( InvestorAcno,7);
    if( MessageHeader::FNewVersion == TRUE )
    {
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
    }
    else
    {
        MemWriter.Write( StockID,6);
        MemWriter.Write( OrderPrice,6);
        MemWriter.Write( OrderQty,8);
        MemWriter.Write(&BuySellCode,1);
        MemWriter.Write(&TradeKind,1);
        MemWriter.Write( ObjectBroker,4);
        MemWriter.Write( OrderDate,6);
        MemWriter.Write( OrderTime,6);
        MemWriter.Write( BeforeQty,8);
        MemWriter.Write( AfterQty,8);
    }    
    MemWriter.AppendZero();        
}
//---------------------------------------------------------------------------
void MessageO020::print( void )
{
    SocketPrintf("--------------- Accept order report message O020 ---------------\n" );
    SocketPrintf("[ Broker No %s ]\n",UFC::AnsiString(BrokerID,4).c_str());
    SocketPrintf("[ PVC-ID %s ]\n",UFC::AnsiString(PVCID,2).c_str());
    SocketPrintf("[ Order No %s ]\n",UFC::AnsiString(OrderNo,5).c_str());
    SocketPrintf("[ Inv_acno %s ]\n",UFC::AnsiString(InvestorAcno,7).c_str());    
    SocketPrintf("[ Stock ID %s ]\n",UFC::AnsiString(StockID,6).c_str());
    SocketPrintf("[ B/S Code %c ]\n",BuySellCode);    
    if( MessageHeader::FNewVersion == TRUE )
    {        
        SocketPrintf("[ Order Price %s ]\n",UFC::AnsiString(OrderPrice,9).c_str());
        SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,6).c_str());        
        SocketPrintf("[ Exchange Code %c ]\n",ExchangeCode);
        SocketPrintf("[ TWSE Order Type %c ]\n",TWSEOrderType);
        SocketPrintf("[ Order Type %c ]\n",OrderType);
        SocketPrintf("[ Time In Force %c ]\n",TimeInForce);
        SocketPrintf("[ Order Date %s ]\n",UFC::AnsiString(OrderDate,8).c_str());
        SocketPrintf("[ Order Time %s ]\n",UFC::AnsiString(OrderTime,9).c_str());
        SocketPrintf("[ Before_QNTY %s ]\n",UFC::AnsiString(BeforeQty,6).c_str());
        SocketPrintf("[ After_QNTY %s ]\n",UFC::AnsiString(AfterQty,6).c_str());
    }
    else
    {
        SocketPrintf("[ Order Price %s ]\n",UFC::AnsiString(OrderPrice,6).c_str());
        SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,8).c_str());
        SocketPrintf("[ Trade Kind %c ]\n",TradeKind);
        SocketPrintf("[ Object Broker %s ]\n",UFC::AnsiString(ObjectBroker,4).c_str() );
        SocketPrintf("[ Order Date %s ]\n",UFC::AnsiString(OrderDate,6).c_str());
        SocketPrintf("[ Order Time %s ]\n",UFC::AnsiString(OrderTime,6).c_str());
        SocketPrintf("[ Before_QNTY %s ]\n",UFC::AnsiString(BeforeQty,8).c_str());
        SocketPrintf("[ After_QNTY %s ]\n",UFC::AnsiString(AfterQty,8).c_str());
    }
    SocketPrintf("----------------------- End of O020 ---------------------------\n");
        
          
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageO030::print( void )
{
    char TmpStr[3];
    strncpynt( TmpStr, GetStatusCodeStr(), 2 );
    SocketPrintf("----- Error Order message O030 code: %s -----------------\n",TmpStr );
}
//---------------------------------------------------------------------------
//  O040 Query message
//---------------------------------------------------------------------------
MessageO040::MessageO040( const char *Data )
:MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//---------------------------------------------------------------------------
void MessageO040::LoadBody( const char* Body )
{
    MemoryStreamReader MemReader( Body );    
    MemReader.Read( StockID,6);    
    MemReader.Read( StickSHR,3);
}
//---------------------------------------------------------------------------
void MessageO040::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( StockID,6);
    MemWriter.Write( StickSHR,3);
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageO040::print( void )
{
    SocketPrintf("--------------------- Query message O040 ------------------------\n" );    
    SocketPrintf("[ Stock ID %s ]\n",UFC::AnsiString(StockID,6).c_str());
    SocketPrintf("[ Stock SHR %s ]\n",UFC::AnsiString(StickSHR,3).c_str());    
    SocketPrintf("----------------------- End of O040 ---------------------------\n");
}
//---------------------------------------------------------------------------
//  O060 Query Response message
//---------------------------------------------------------------------------
MessageO060::MessageO060( const char *Data )
:MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//---------------------------------------------------------------------------
void MessageO060::LoadBody( const char* Body )
{
    MemoryStreamReader MemReader( Body );    
    MemReader.Read( StockID, 6);    
    MemReader.Read( StickSHR,3);
    MemReader.Read( BuyCNT,  9);
    MemReader.Read( Filler1, 3);
    MemReader.Read( SellCNT, 9);
    MemReader.Read( BuySHR,  9);
    MemReader.Read( Filler2, 3);
    MemReader.Read( SellSHR, 9);
}
//---------------------------------------------------------------------------
void MessageO060::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( StockID,6);
    MemWriter.Write( StickSHR,3);
    MemWriter.Write( BuyCNT,  9);
    MemWriter.Write( Filler1, 3);
    MemWriter.Write( SellCNT, 9);
    MemWriter.Write( BuySHR,  9);
    MemWriter.Write( Filler2, 3);
    MemWriter.Write( SellSHR, 9);
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageO060::print( void )
{
    SocketPrintf("--------------- Query Response message O060 --------------------\n" );    
    SocketPrintf("[ Stock ID %s ]\n",UFC::AnsiString(StockID,6).c_str());
    SocketPrintf("[ Stock SHR %s ]\n",UFC::AnsiString(StickSHR,3).c_str());    
    SocketPrintf("[ Buy CNT %s ]\n",UFC::AnsiString(BuyCNT,9).c_str());
    SocketPrintf("[ Filler %s ]\n",UFC::AnsiString(Filler1,3).c_str());        
    SocketPrintf("[ Sell CNT %s ]\n",UFC::AnsiString(SellCNT,9).c_str());
    SocketPrintf("[ BuySHR SHR %s ]\n",UFC::AnsiString(BuySHR,9).c_str());    
    SocketPrintf("[ Filler %s ]\n",UFC::AnsiString(Filler2,3).c_str());
    SocketPrintf("[ Sell SHR %s ]\n",UFC::AnsiString(SellSHR,9).c_str());        
    SocketPrintf("----------------------- End of O060 ---------------------------\n");
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
