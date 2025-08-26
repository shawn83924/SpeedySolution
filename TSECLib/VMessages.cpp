#include "VMessages.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
TMessageType MessageHeader::GetLendTransactionSubsystemMessage( int FuncCode, int MsgType )
{   ///< Function code 1 buy, 2 Cancel, 3 Reduce, 4 Query
    switch( MsgType )
    {
        case 0: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 )
                    return mtV010;
	case 1: if( FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 ||  FuncCode == 4 )
                    return mtV020;				
	case 2: if( FuncCode == 0 )
                    return mtV040;
	case 3: if( FuncCode == 0 || FuncCode == 1 ||  FuncCode == 2 ||  FuncCode == 3 || FuncCode == 4 )
        	    return mtV030;
	case 4: if( FuncCode == 0 )
                    return mtV060;
	case 5: if( FuncCode == 0 )
                    return mtV050;        
	default: return mtUnknown;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageV010::MessageV010( BOOL IsTWSE, char* FunctionCode,char* Body )
:MessageHeader((char*)((IsTWSE)?"31":"90"),FunctionCode,"00")
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageV010::MessageV010( const char* Buffer )
:MessageHeader( Buffer )
{
    LoadBody( Buffer  + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV010::LoadBody( const char *Body )
{
    MemoryStreamReader MemReader( Body );
    MemReader.Read( BrokerID, 4 );     /* Broker ID */
    MemReader.Read( PVCID, 2 );        /* PVC ID */
    MemReader.Read( OrderNo, 5 );      /* order # */
    MemReader.Read( InvestorAcno,7);   /* investor account # */
    MemReader.Read( StockID,   6 );    /* Stock ID */
    if( MessageHeader::FNewExVersion == TRUE )
        MemReader.Read( OrderPrice,8 );    /* order price */
    else
        MemReader.Read( OrderPrice,7 );    /* order price */
    MemReader.Read( OrderQty,  6 );   /* quantity */
    MemReader.Read( &Keep,  1 );   /* quantity */
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV010::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( BrokerID, 4 );     /* Broker ID */
    MemWriter.Write( PVCID,    2 );     /* PVC ID */
    MemWriter.Write( OrderNo,  5 );     /* order # */
    MemWriter.Write( InvestorAcno,7);   /* investor account # */
    MemWriter.Write( StockID,   6 );    /* Stock ID */
    if( MessageHeader::FNewExVersion == TRUE )
        MemWriter.Write( OrderPrice,8 );    /* order price */
    else
        MemWriter.Write( OrderPrice,7 );    /* order price */
    MemWriter.Write( OrderQty,  6 );   /* quantity */
    MemWriter.Write( &Keep,     1 );   
    MemWriter.AppendZero();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int MessageV010::GetOrderQty( void )
{
    return UFC::AnsiString::StrToInt32( OrderQty, 6 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double MessageV010::GetOrderPrice( void )
{
    return ((double)UFC::AnsiString::StrToInt32( OrderPrice, 8 ))/10000.0;    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV010::print( void )
{
    SocketPrintf("---------------------- Order message V010 ---------------------\n" );
    SocketPrintf("[ Broker-ID %s ]\n",UFC::AnsiString(BrokerID,4).c_str());
    SocketPrintf("[ PVC-ID %s ]\n",UFC::AnsiString(PVCID,2).c_str());
    SocketPrintf("[ Order_No %s ]\n",UFC::AnsiString(OrderNo,5).c_str());
    SocketPrintf("[ Inv_acno %s ]\n",UFC::AnsiString(InvestorAcno,7).c_str());
    SocketPrintf("[ Prod_ID %s ]\n",UFC::AnsiString(StockID,6).c_str());
    if( MessageHeader::FNewExVersion == TRUE )
        SocketPrintf("[ Order_Price %s ]\n",UFC::AnsiString(OrderPrice,8).c_str());
    else
        SocketPrintf("[ Order_Price %s ]\n",UFC::AnsiString(OrderPrice,7).c_str());
    SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,6).c_str());
    SocketPrintf("[ Keep %c ]\n",Keep );
    SocketPrintf("-------------------------- End of V010 ------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Class MessageV020
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageV020::MessageV020( const char *Data )
:MessageHeader( Data )
{
    LoadBody( Data + MESSAGE_HEADER_SIZE );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageV020::MessageV020( BOOL IsTWSE, char* Func, char *Body )
:MessageHeader( (char*)((IsTWSE)?"31":"90"),Func,"01" )
{
    LoadBody( Body );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV020::LoadBody( const char *Body )
{
    MemoryStreamReader MemReader( Body );
    MemReader.Read( BrokerID,4);
    MemReader.Read( PVCID, 2 );
    MemReader.Read( OrderNo,5);
    MemReader.Read( InvestorAcno,7);
    MemReader.Read( StockID,6);
    if( MessageHeader::FNewExVersion == TRUE )
    {
        MemReader.Read( OrderPrice,8);
        MemReader.Read( OrderQty,6 );
        MemReader.Read( &Keep, 1 );    
        MemReader.Read( OrderDate,8);
        MemReader.Read( OrderTime,8);
    }
    else
    {
        MemReader.Read( OrderPrice,7);
        MemReader.Read( OrderQty,6 );
        MemReader.Read( &Keep, 1 );    
        MemReader.Read( OrderDate,6);
        MemReader.Read( OrderTime,6);
    }
    MemReader.Read( BeforeQty,6);
    MemReader.Read( AfterQty,6);    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV020::SetPrice( double Px )
{
    char buffer[16];
    
    snprintf( buffer, 9, "%08d", (int)((Px +0.00001) * 10000.0) );
    memcpy( OrderPrice, buffer, 8 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV020::SetQty( int Qty )
{
    char buffer[16];
    snprintf( buffer, 7, "%06d", Qty );
    memcpy( OrderQty, buffer, 6);    
}    
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV020::SetBeforeQty( int Qty )
{
    char buffer[16];
    snprintf( buffer, 7, "%06d", Qty );
    memcpy( BeforeQty, buffer, 6);        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV020::SetAfterQty( int Qty )
{
    char buffer[16];
    snprintf( buffer, 7, "%06d", Qty );
    memcpy( AfterQty, buffer, 6);            
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV020::SetDate( int YYYY, int MM, int DD )
{
    char buffer[16];
    snprintf( buffer, 9, "%04d%02d%02d", YYYY, MM, DD);
    memcpy( OrderDate, buffer, 8);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV020::SetTime( int H, int M, int S, int mmm )
{
    char buffer[16];    
    
    snprintf( buffer, 15, "%02d%02d%02d%02d", H%100,M%100,S%100, (mmm/10)%100 );
    memcpy( OrderTime, buffer, 8);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV020::AsString( char* Buffer )
{
    MessageHeader::AsString( Buffer );
    MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
    MemWriter.Write( BrokerID,4);
    MemWriter.Write( PVCID, 2 );
    MemWriter.Write( OrderNo,5);
    MemWriter.Write( InvestorAcno,7);
    MemWriter.Write( StockID,6);
    if( MessageHeader::FNewExVersion == TRUE )
    {
        MemWriter.Write( OrderPrice,8);
        MemWriter.Write( OrderQty,6);    
        MemWriter.Write( &Keep, 1 );        
        MemWriter.Write( OrderDate,8);
        MemWriter.Write( OrderTime,8);
    }
    else
    {
        MemWriter.Write( OrderPrice,7);
        MemWriter.Write( OrderQty,6);    
        MemWriter.Write( &Keep, 1 );        
        MemWriter.Write( OrderDate,6);
        MemWriter.Write( OrderTime,6);
    }
    MemWriter.Write( BeforeQty,6);
    MemWriter.Write( AfterQty,6);    
    MemWriter.AppendZero();
}
//---------------------------------------------------------------------------
void MessageV020::print( void )
{
    SocketPrintf("--------------- Accept order report mesage V020 ---------------\n" );
    SocketPrintf("[ Broker No %s ]\n",UFC::AnsiString(BrokerID,4).c_str());
    SocketPrintf("[ PVC-ID %s ]\n",UFC::AnsiString(PVCID,2).c_str());
    SocketPrintf("[ Order No %s ]\n",UFC::AnsiString(OrderNo,5).c_str());
    SocketPrintf("[ Inv_acno %s ]\n",UFC::AnsiString(InvestorAcno,7).c_str());
    SocketPrintf("[ Stock ID %s ]\n",UFC::AnsiString(StockID,6).c_str());
    if( MessageHeader::FNewExVersion == TRUE )
    {
        SocketPrintf("[ Order Price %s ]\n",UFC::AnsiString(OrderPrice,8).c_str());
        SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,6).c_str());
        SocketPrintf("[ Keep %c ]\n",Keep );    
        SocketPrintf("[ Order Date %s ]\n",UFC::AnsiString(OrderDate,8).c_str());
        SocketPrintf("[ Order Time %s ]\n",UFC::AnsiString(OrderTime,8).c_str());
    }
    else
    {
        SocketPrintf("[ Order Price %s ]\n",UFC::AnsiString(OrderPrice,7).c_str());
        SocketPrintf("[ QNTY %s ]\n",UFC::AnsiString(OrderQty,6).c_str());
        SocketPrintf("[ Keep %c ]\n",Keep );    
        SocketPrintf("[ Order Date %s ]\n",UFC::AnsiString(OrderDate,6).c_str());
        SocketPrintf("[ Order Time %s ]\n",UFC::AnsiString(OrderTime,6).c_str());
    }
    SocketPrintf("[ Before_QNTY %s ]\n",UFC::AnsiString(BeforeQty,6).c_str());
    SocketPrintf("[ After_QNTY %s ]\n",UFC::AnsiString(AfterQty,6).c_str());
    SocketPrintf("----------------------- End of V020 ---------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void MessageV030::print( void )
{
    char TmpStr[3];
    strncpynt( TmpStr, GetStatusCodeStr(), 2 );
    SocketPrintf("----- Error Order message V030 code: %s -----------------\n",TmpStr );
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
