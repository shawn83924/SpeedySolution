#include "PMessages.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetPostTransactionSubsystemMessage( int FuncCode, int MsgType )
{   ///< Function code 1 buy, 2 Sell, 3 Reduce, 4 Cancel, 5 Query
    switch( MsgType )
    {
        case 0: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5 )
                    return mtP010;
        case 1: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5 )
                    return mtP020;				
        case 2: if( FuncCode == 0 )
                    return mtP040;
        case 3: if( FuncCode == 0 || FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 || FuncCode == 4 || FuncCode == 5 )
                    return mtP030;
        case 4: if( FuncCode == 0 )
                    return mtP060;
        case 5: if( FuncCode == 0 )
                    return mtP050;
        case 6: if( FuncCode == 0 )
                    return mtP070;
        case 7: if( FuncCode == 0 )
                    return mtP080;
	default: return mtUnknown;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageP010::MessageP010( BOOL IsTWSE, const char* FunctionCode )
:MessageHeader((char*)((IsTWSE)?"32":"98"),FunctionCode,"00")
,FQty( 0 )
,FPx( 0.0 )
{
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageP010::MessageP010( BOOL IsTWSE, char* FunctionCode,char* Body )
:MessageHeader((char*)((IsTWSE)?"32":"98"),FunctionCode,"00")
,FQty( 0 )
,FPx( 0.0 )
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageP010::MessageP010( const char* Buffer )
:MessageHeader( Buffer )
,FQty( 0 )
,FPx( 0.0 )
{
    LoadBody( Buffer  + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageP010::GetOrderQty( void )
{
    if( FQty == 0 )
        FQty = atoi( OrderQty );
    return FQty;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double MessageP010::GetOrderPrice( void )
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
void MessageP010::SetPrice( double Px )
{
    snprintf( OrderPrice, 10, "%09d", (int)((Px +0.00001) * 10000.0) );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP010::SetQty( int Qty )
{
   snprintf( OrderQty, 7, "%06d", Qty );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP010::LoadBody( const char *Body )
{
    MemoryStreamReader MemReader( Body );
    
    MemReader.ReadString( BrokerID, 4 );     /* Broker ID */
    MemReader.ReadString( PVCID, 2 );        /* PVC ID */
    MemReader.ReadString( OrderNo, 5 );      /* order # */
    MemReader.ReadString( InvestorAcno, 7);  /* investor account # */
    MemReader.Read( &InvestorFlag, 1 );      /* investor flag */
    MemReader.ReadString( StockID, 6 );      /* Stock ID */
    if( MessageHeader::FNewVersion == TRUE )
    {
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
        MemReader.ReadString( OrderPrice,6 );    /* order price */
        MemReader.ReadString( OrderQty,  3 );    /* quantity */
        MemReader.Read( &BuySellCode,1);         /* B/S code */
        MemReader.Read( &ExchangeCode,1);        /* order condition */
        MemReader.Read( &TWSEOrderType,1);           /* order type */
        OrderType = '2';
        TimeInForce = '0';
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
void MessageP010::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    
    MemWriter.Write( BrokerID, 4 );     /* Broker ID */
    MemWriter.Write( PVCID,    2 );     /* PVC ID */
    MemWriter.Write( OrderNo,  5 );     /* order # */
    MemWriter.Write( InvestorAcno,7);   /* investor account # */
    MemWriter.Write( &InvestorFlag,1);  /* investor flag */
    MemWriter.Write( StockID,   6 );    /* Stock ID */
    if( MessageHeader::FNewVersion == TRUE )
    {
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
        MemWriter.Write( OrderPrice,6 );    /* order price */
        MemWriter.Write( OrderQty,  3 );    /* quantity */
        MemWriter.Write( &BuySellCode,1);   /* B/S code */
        MemWriter.Write( &ExchangeCode,1);
        MemWriter.Write( &TWSEOrderType,1);     /* order type */        
    }
    MemWriter.AppendZero();    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP010::print( void )
{
    SocketPrintf("---------------------- Order message P010 ---------------------\n" );
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
    if( MessageHeader::FNewVersion == TRUE )
    {
        SocketPrintf("[ Order_Type %c ]\n",OrderType );
        SocketPrintf("[ TimeInForce %c ]\n",TimeInForce );
    }
    SocketPrintf("-------------------------- End of P010 ------------------------\n");       
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class MessageP020
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageP020::MessageP020( const char *Data )
:MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageP020::MessageP020( BOOL IsTWSE, char* Func, char *Body )
:MessageHeader( (char*)((IsTWSE)?"32":"98"),Func,"01" )
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP020::SetPrice( double Px )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderPrice, 10, "%09d", (int)((Px +0.00001) * 10000.0) );
    else
        snprintf( OrderPrice, 10, "%06d", (int)((Px +0.001) * 100.0) );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP020::SetQty( int Qty )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderQty, 7, "%06d", Qty );
    else
        snprintf( OrderQty, 7, "%03d", Qty );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP020::SetBeforeQty( int Qty )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( BeforeQty, 7, "%06d", Qty );
    else
        snprintf( BeforeQty, 7, "%03d", Qty );        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP020::SetAfterQty( int Qty )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( AfterQty, 7, "%06d", Qty );
    else
        snprintf( AfterQty, 7, "%03d", Qty );         
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP020::SetDate( int YYYY, int MM, int DD )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderDate, 9, "%04d%02d%02d", YYYY, MM, DD);
    else    
        snprintf( OrderDate, 9, "00%02d%02d", MM,DD );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP020::SetTime( int H, int M, int S, int mmm )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderTime, 10, "%02d%02d%02d%03d", H,M,S, mmm );
    else    
        snprintf( OrderTime, 10, "%02d%02d%02d", H,M,S );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP020::LoadBody( const char *Body )
{
    MemoryStreamReader MemReader( Body );
    
    MemReader.Read( BrokerID,4);
    MemReader.Read( PVCID, 2 );
    MemReader.Read( OrderNo,5);
    MemReader.Read( InvestorAcno,7);
    MemReader.Read(&InvestorFlag,1);
    MemReader.Read( StockID,6);
    if( MessageHeader::FNewVersion == TRUE )
    {
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
        MemReader.Read( OrderPrice,6);
        MemReader.Read( OrderQty,3);
        MemReader.Read(&BuySellCode,1);
        MemReader.Read(&ExchangeCode,1);
        MemReader.Read(&TWSEOrderType,1);
        MemReader.Read( OrderDate,6);
        MemReader.Read( OrderTime,6);
        MemReader.Read( BeforeQty,3);
        MemReader.Read( AfterQty,3);
    }    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP020::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( BrokerID,4);
    MemWriter.Write( PVCID, 2 );
    MemWriter.Write( OrderNo,5);
    MemWriter.Write( InvestorAcno,7);
    MemWriter.Write(&InvestorFlag,1);
    MemWriter.Write( StockID,6);
    if( MessageHeader::FNewVersion == TRUE )
    {
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
        MemWriter.Write( OrderPrice,6);
        MemWriter.Write( OrderQty,3);
        MemWriter.Write(&BuySellCode,1);
        MemWriter.Write(&ExchangeCode,1);
        MemWriter.Write(&TWSEOrderType,1);
        MemWriter.Write( OrderDate,6);
        MemWriter.Write( OrderTime,6);
        MemWriter.Write( BeforeQty,3);
        MemWriter.Write( AfterQty,3);
    }    
    MemWriter.AppendZero();  
}
//---------------------------------------------------------------------------
void MessageP020::print( void )
{
    SocketPrintf("--------------- Accept order report message P020 ---------------\n" );
    SocketPrintf("[ Broker No %s ]\n",UFC::AnsiString(BrokerID,4).c_str());
    SocketPrintf("[ PVC-ID %s ]\n",UFC::AnsiString(PVCID,2).c_str());
    SocketPrintf("[ Order No %s ]\n",UFC::AnsiString(OrderNo,5).c_str());
    SocketPrintf("[ Inv_acno %s ]\n",UFC::AnsiString(InvestorAcno,7).c_str());
    SocketPrintf("[ Inv_flag %c ]\n",InvestorFlag );
    SocketPrintf("[ Stock ID %s ]\n",UFC::AnsiString(StockID,6).c_str());
    if( MessageHeader::FNewVersion == TRUE )
    {        
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
    }
    else
    {
        SocketPrintf("[ Order Price %s ]\n",UFC::AnsiString(OrderPrice,6).c_str());
        SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,3).c_str());
        SocketPrintf("[ B/S Code %c ]\n",BuySellCode);
        SocketPrintf("[ Exchange Code %c ]\n",ExchangeCode);
        SocketPrintf("[ Order Type %c ]\n",TWSEOrderType);
        SocketPrintf("[ Order Date %s ]\n",UFC::AnsiString(OrderDate,6).c_str());
        SocketPrintf("[ Order Time %s ]\n",UFC::AnsiString(OrderTime,6).c_str());
        SocketPrintf("[ Before_QNTY %s ]\n",UFC::AnsiString(BeforeQty,3).c_str());
        SocketPrintf("[ After_QNTY %s ]\n",UFC::AnsiString(AfterQty,3).c_str());
    }
    SocketPrintf("----------------------- End of P020 ---------------------------\n");        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageP030::print( void )
{
    char TmpStr[3];
    strncpynt( TmpStr, GetStatusCodeStr(), 2 );
    SocketPrintf("----- Error Order message P030 code: %s -----------------\n",TmpStr );
}
//---------------------------------------------------------------------------
//  P070 Query message
//---------------------------------------------------------------------------
MessageP070::MessageP070( const char *Data )
:MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//---------------------------------------------------------------------------
void MessageP070::LoadBody( const char* Body )
{
    MemoryStreamReader MemReader( Body );    
    MemReader.Read( StockID,6);    
}
//---------------------------------------------------------------------------
void MessageP070::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( StockID,6);
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageP070::print( void )
{
    SocketPrintf("--------------------- Query message P070 ------------------------\n" );    
    SocketPrintf("[ Stock ID %s ]\n",UFC::AnsiString(StockID,6).c_str());
    SocketPrintf("----------------------- End of P070 ---------------------------\n");
}
//---------------------------------------------------------------------------
//  P080 Query message
//---------------------------------------------------------------------------
MessageP080::MessageP080( const char *Data )
:MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//---------------------------------------------------------------------------
void MessageP080::LoadBody( const char* Body )
{
    MemoryStreamReader MemReader( Body );    
    MemReader.Read( StockID,6);    
    MemReader.Read( StaticTime,4);
    MemReader.Read( BuyCNT,10);
    MemReader.Read( BuySHR,10);
    MemReader.Read( SellCNT,10);
    MemReader.Read( SellSHR,10);
}
//---------------------------------------------------------------------------
void MessageP080::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( StockID,6);
    MemWriter.Write( StaticTime,4);
    MemWriter.Write( BuyCNT,10);
    MemWriter.Write( BuySHR,10);
    MemWriter.Write( SellCNT,10);
    MemWriter.Write( SellSHR,10);
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageP080::print( void )
{
    SocketPrintf("----------------- Query Reply message P080 ---------------------\n" );    
    SocketPrintf("[ Stock ID %s ]\n",UFC::AnsiString(StockID,6).c_str());
    SocketPrintf("[ Static Time %s ]\n",UFC::AnsiString(StaticTime,4).c_str());
    SocketPrintf("[ Buy CNT %s ]\n",UFC::AnsiString(BuyCNT,10).c_str());
    SocketPrintf("[ Buy SHR %s ]\n",UFC::AnsiString(BuySHR,10).c_str());
    SocketPrintf("[ Sell CNT %s ]\n",UFC::AnsiString(SellCNT,10).c_str());
    SocketPrintf("[ Sell SHR %s ]\n",UFC::AnsiString(SellSHR,10).c_str());
    SocketPrintf("----------------------- End of P080 ---------------------------\n");
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
