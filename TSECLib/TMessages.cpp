#include "TMessages.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetTransactionSubsystemMessage( int FuncCode, int MsgType )
{   ///< Function code 1 buy, 2 Sell, 3 Reduce, 4 Cancel, 5 Query
    switch( MsgType )
    {
        case 0: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5  || FuncCode == 6 )
                    return mtT010;
                else
                    return mtUnknown;
        case 1: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 || FuncCode == 5  || FuncCode == 6 )
                    return mtT020;
                else
                    return mtUnknown;
        case 2: if( FuncCode == 0 )
                    return mtT040;
                else
                    return mtUnknown;
        case 3: if( FuncCode == 0 || FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 || FuncCode == 4 || FuncCode == 5  || FuncCode == 6 )
                    return mtT030;
                else
                    return mtUnknown;
        case 4: if( FuncCode == 0 )
                    return mtT060;
                else
                    return mtUnknown;
        case 5: if( FuncCode == 0 )
                    return mtT050;
                else
                    return mtUnknown;
        default: return mtUnknown;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageT010::MessageT010( void )
:MessageHeader( )
,FQty( 0 )
,FPx( 0.0 )
{
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageT010::MessageT010( BOOL IsTWSE, const char* FunctionCode )
:MessageHeader((char*)((IsTWSE)?"30":"93"),FunctionCode,"00")
,FQty( 0 )
,FPx( 0.0 )
{
    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageT010::MessageT010( BOOL IsTWSE, char* FunctionCode,char* Body )
:MessageHeader((char*)((IsTWSE)?"30":"93"),FunctionCode,"00")
,FQty( 0 )
,FPx( 0.0 )
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageT010::MessageT010( const char* Buffer )
:MessageHeader( Buffer )
,FQty( 0 )
,FPx( 0.0 )
{
    LoadBody( Buffer  + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageT010::GetOrderQty( void )
{
    if( FQty == 0 )
        FQty = atoi( OrderQty );
    return FQty;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double MessageT010::GetOrderPrice( void )
{
    if( FPx == 0.0 )
        FPx = ((double)atoi( OrderPrice ))/10000.0;
    return FPx;    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT010::Load( const char *Data )
{
    LoadHeader( Data );
    LoadBody( Data  + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT010::LoadBody( const char *Body )
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
        MemReader.Read( &ExchangeCode,1);        /* Exchange Code */
        MemReader.Read( &TWSEOrderType,1);       /* TWSEorder type */        
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
void MessageT010::SetPrice( double Px )
{
    snprintf( OrderPrice, 10, "%09d", (int)((Px +0.00001) * 10000.0) );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT010::SetQty( int Qty )
{
   snprintf( OrderQty, 7, "%06d", Qty );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT010::AsString( char* Buffer )
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
        MemWriter.Write( &TWSEOrderType,1); /* TWSE order type */
    }
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT010::print( void )
{
   
    SocketPrintf("---------------------- Order message T010 ---------------------\n" );
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
    SocketPrintf("-------------------------- End of T010 ------------------------\n");    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class MessageT020
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageT020::MessageT020( const char *Data )
:MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageT020::MessageT020( BOOL IsTWSE, char* Func, char *Body )
:MessageHeader( (char*)((IsTWSE)?"30":"93"),Func,"01" )
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT020::SetPrice( double Px )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderPrice, 10, "%09d", (int)((Px +0.00001) * 10000.0) );
    else
        snprintf( OrderPrice, 10, "%06d", (int)((Px +0.001) * 100.0) );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT020::SetQty( int Qty )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderQty, 7, "%06d", Qty );
    else
        snprintf( OrderQty, 7, "%03d", Qty );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT020::SetBeforeQty( int Qty )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( BeforeQty, 7, "%06d", Qty );
    else
        snprintf( BeforeQty, 7, "%03d", Qty );        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT020::SetAfterQty( int Qty )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( AfterQty, 7, "%06d", Qty );
    else
        snprintf( AfterQty, 7, "%03d", Qty );         
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT020::SetDate( int YYYY, int MM, int DD )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderDate, 9, "%04d%02d%02d", YYYY, MM, DD);
    else    
        snprintf( OrderDate, 9, "00%02d%02d", MM,DD );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT020::SetTime( int H, int M, int S, int mmm )
{
    if( MessageHeader::FNewVersion == TRUE )    
        snprintf( OrderTime, 10, "%02d%02d%02d%03d", H,M,S, mmm );
    else    
        snprintf( OrderTime, 10, "%02d%02d%02d", H,M,S );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT020::LoadBody( const char *Body )
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
void MessageT020::AsString( char* Buffer )
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
void MessageT020::print( void )
{   
    SocketPrintf("--------------- Accept order report message T020 ---------------\n" );
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
    SocketPrintf("----------------------- End of T020 ---------------------------\n");    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageT030::print( void )
{
    char TmpStr[3];
    strncpynt( TmpStr, GetStatusCodeStr(), 2 );
    SocketPrintf("----- Error Order message T030 code: %s -----------------\n",TmpStr );
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
