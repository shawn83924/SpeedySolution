/**@file OrderInfo.cpp
*  @brief Core Class to process all kinds of orders.
*  @author Simon Chaang
*
*/
#include "TransactionObject.h"
#include "ExchangeConst.h"
//----------------------------------------------------------------------------------------------------------------------
extern BOOL IsTSEC;
extern BOOL IsOldVersion;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------    
UFC::PHashMap< UFC::AnsiString, int* >  OrderInfo::FStockLastPxs;        
//----------------------------------------------------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& OID, MessageT010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX )
:FSession( tsNormal ),
 FOID( OID ),
 FSymbol( Order->StockID, 6 ),
 FBrokerID( Order->BrokerID, 4 ),
 FAccount( Order->InvestorAcno, 7 ),
 FObjectBroker( "    " ),
 FInvestorFlag( Order->InvestorFlag ),
 FSide( Order->BuySellCode ), 
 FFillQty( 0 ),
 FReduceQty( 0 ),
 FExchangeCode( Order->ExchangeCode ),
 FOrderType( Order->TWSEOrderType ), 
 FTradeKind( ' ' ),
 FStkSeqNo( 0 ),
 FKeep( FALSE ),        
 FTimeInforce( Order->TimeInForce ),
 FOrdType( Order->OrderType ),
 FPVCID( PVCID ),
 FLineBrokerID( BrkID ),
 FIsFIX( IsFIX ),
 FCountDown(0),
 FFilledQty(0),
 FFilledPx(0.0)
{
    if( IsOldVersion == true )
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 6 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 3 ).ToInt();
    }
    else
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 9 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 6 ).ToInt();
    }
    if( FPrice == 0 ) ///< Market order
    {
        int* LastPx;
        if( FStockLastPxs.GetObjectByKey( FSymbol, LastPx) == true )        
            FPrice = *LastPx;
    }
    else
    {
        int* LastPx;
        if( FStockLastPxs.GetObjectByKey( FSymbol, LastPx) == true )
            *LastPx = FPrice;
        else
            FStockLastPxs.Add( FSymbol, new int(FPrice) );
    }
}
//----------------------------------------------------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& OID, MessageP010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX )
:FSession( tsPost ),
 FOID( OID ),
 FSymbol( Order->StockID, 6 ),
 FBrokerID( Order->BrokerID, 4 ),
 FAccount( Order->InvestorAcno, 7 ),
 FObjectBroker( "    " ),
 FInvestorFlag( Order->InvestorFlag ),
 FSide( Order->BuySellCode ), 
 FFillQty( 0 ),
 FReduceQty( 0 ),
 FExchangeCode( Order->ExchangeCode ),
 FOrderType( Order->TWSEOrderType ),        
 FTradeKind( ' ' ),
 FStkSeqNo( 0 ),
 FKeep( FALSE ),        
 FTimeInforce( Order->TimeInForce ),
 FOrdType( Order->OrderType ),
 FPVCID( PVCID ),
 FLineBrokerID( BrkID ),
 FIsFIX( IsFIX ),
 FCountDown(0),
 FFilledQty(0),
 FFilledPx(0.0)
{
    if( IsOldVersion == true )
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 6 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 3 ).ToInt();
    }
    else
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 9 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 6 ).ToInt();
    }
}
//----------------------------------------------------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& OID, MessageO010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX )
:FSession( tsOdd ),
 FOID( OID ),
 FSymbol( Order->StockID, 6 ),
 FBrokerID( Order->BrokerID, 4 ),
 FAccount( Order->InvestorAcno, 7 ),
 FObjectBroker( Order->ObjectBroker, 4 ),
 FInvestorFlag( ' ' ),
 FSide( Order->BuySellCode ), 
 FFillQty( 0 ),
 FReduceQty( 0 ) ,
 FExchangeCode( '2' ),
 FOrderType( Order->TWSEOrderType ),        
 FTradeKind( Order->TradeKind ),
 FStkSeqNo( 0 ),
 FKeep( FALSE ),
 FTimeInforce( Order->TimeInForce ),
 FOrdType( Order->OrderType ),        
 FPVCID( PVCID ),
 FLineBrokerID( BrkID ),
 FIsFIX( IsFIX ),
 FCountDown(0),
 FFilledQty(0),
 FFilledPx(0.0)
{
    if( IsOldVersion == true )
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 6 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 8 ).ToInt();
    }
    else
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 9 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 6 ).ToInt();
    }
}
//----------------------------------------------------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& OID, MessageO110* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX )
:FSession( tsOddEx ),
 FOID( OID ),
 FSymbol( Order->StockID, 6 ),
 FBrokerID( Order->BrokerID, 4 ),
 FAccount( Order->InvestorAcno, 7 ),
 FObjectBroker( "    " ),
 FInvestorFlag( Order->InvestorFlag ),
 FSide( Order->BuySellCode ), 
 FFillQty( 0 ),
 FReduceQty( 0 ),
 FExchangeCode( '2' ),
 FOrderType( Order->TWSEOrderType ), 
 FTradeKind( ' ' ),
 FStkSeqNo( 0 ),
 FKeep( FALSE ),        
 FTimeInforce( Order->TimeInForce ),
 FOrdType( Order->OrderType ),
 FPVCID( PVCID ),
 FLineBrokerID( BrkID ),
 FIsFIX( IsFIX ),
 FCountDown(0),
 FFilledQty(0),
 FFilledPx(0.0)
{
    FPrice = UFC::AnsiString( Order->OrderPrice, 9 ).ToInt();
    FQty   = UFC::AnsiString( Order->OrderQty , 6 ).ToInt();
}
//----------------------------------------------------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& OID, MessageA010* Order )
:FSession( tsAuction ),
 FOID( OID ),
 FSymbol( Order->StockID, 6 ),
 FBrokerID( Order->BrokerID, 4 ),
 FAccount( Order->InvestorAcno, 7 ),
 FObjectBroker( "    " ),
 FInvestorFlag( ' ' ),
 FSide( 'B' ), 
 FFillQty( 0 ),
 FReduceQty( 0 ) ,
 FExchangeCode( '0' ),
 FOrderType( '0' ),        
 FTradeKind( ' ' ),
 FStkSeqNo( 0 ),
 FKeep( FALSE ),
 FTimeInforce( '0' ),
 FOrdType( '2' ),
 FIsFIX( FALSE ),
 FCountDown(0),
 FFilledQty(0),
 FFilledPx(0.0)
{
    if( IsOldVersion == true )
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 6 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 12 ).ToInt();
    }
    else
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 9 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 12 ).ToInt();
    }    
}
//----------------------------------------------------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& OID, MessageE010* Order )
:FSession( tsTender ),
 FOID( OID ),
 FSymbol( Order->StockID, 6 ),
 FBrokerID( Order->BrokerID, 4 ),
 FAccount( Order->InvestorAcno, 7 ),
 FObjectBroker( "    " ),
 FInvestorFlag( ' ' ),
 FSide( 'S' ), 
 FFillQty( 0 ),
 FReduceQty( 0 ),
 FExchangeCode( '0' ),
 FOrderType( '0' ),        
 FTradeKind( ' ' ),
 FStkSeqNo( UFC::AnsiString( Order->StkSeqNo , 2 ).ToInt() ),
 FKeep( FALSE ),
 FTimeInforce( '0' ),
 FOrdType( '2' ),
 FIsFIX( FALSE ),
 FCountDown(0),
 FFilledQty(0),
 FFilledPx(0.0)
{
    if( IsOldVersion == true )
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 6 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 12 ).ToInt();
    }
    else
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 9 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 12 ).ToInt();
    }        
}
//----------------------------------------------------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& OID, MessageEx010* Order )
:FSession( tsTenderEx ),
 FOID( OID ),
 FSymbol( Order->StockID, 6 ),
 FBrokerID( Order->BrokerID, 4 ),
 FAccount( Order->InvestorAcno, 7 ),
 FObjectBroker( "    " ),
 FInvestorFlag( ' ' ),
 FSide( 'S' ),
 FFillQty( 0 ),
 FReduceQty( 0 ),
 FExchangeCode( '0' ),
 FOrderType( '0' ),        
 FTradeKind( ' ' ),
 FStkSeqNo( UFC::AnsiString( Order->StkSeqNo , 2 ).ToInt() ),
 FKeep( FALSE ),
 FTimeInforce( '0' ),
 FOrdType( '2' ),
 FIsFIX( FALSE ),
 FCountDown(0),
 FFilledQty(0),
 FFilledPx(0.0)        
{
    if( IsOldVersion == true )
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 6 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 12 ).ToInt();
    }
    else
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 9 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 12 ).ToInt();
    }    
}
//----------------------------------------------------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& OID, MessageV010* Order )
:FSession( tsLend ),
 FOID( OID ),
 FSymbol( Order->StockID, 6 ),
 FBrokerID( Order->BrokerID, 4 ),
 FAccount( Order->InvestorAcno, 7 ),
 FObjectBroker( "    " ),
 FInvestorFlag( ' ' ),
 FSide( 'B' ), 
 FFillQty( 0 ),
 FReduceQty( 0 ),
 FExchangeCode( '0' ),
 FOrderType( '0' ),        
 FTradeKind( ' ' ),
 FStkSeqNo( 0 ),
 FKeep( (Order->Keep == 'Y') ),
 FTimeInforce( '0' ),
 FOrdType( '2' ),
 FIsFIX( FALSE ),
 FCountDown(0),
 FFilledQty(0),
 FFilledPx(0.0)        
{
    if( IsOldVersion == true )
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 7 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 6 ).ToInt();
    }
    else
    {
        FPrice = UFC::AnsiString( Order->OrderPrice, 8 ).ToInt();
        FQty   = UFC::AnsiString( Order->OrderQty , 6 ).ToInt();
    }  
}
//----------------------------------------------------------------------------------------------------------------------
//
//   Fill functions
//
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::FillT020( MessageT020* T020, int cQty, int Before,int After, const UFC::AnsiString& PVCID )
{
    UFC::AnsiString Price,Qty,BeforeQty,AfterQty;    
    UFC::UDateTime  Now;
    
    
    memcpy( T020->BrokerID, FBrokerID.c_str(), 4 );
    memcpy( T020->PVCID, PVCID.c_str(), 2 );
    memcpy( T020->OrderNo, FOID.c_str(), 5 );
    memcpy( T020->InvestorAcno, FAccount.c_str(), 7 );
    T020->InvestorFlag = FInvestorFlag;
    memcpy( T020->StockID, FSymbol.c_str(), 6 );
    T020->BuySellCode = FSide;
    T020->ExchangeCode = FExchangeCode;
    T020->TWSEOrderType = FOrderType;    
    T020->TimeInForce = FTimeInforce;
    T020->OrderType = FOrdType;
    
    T020->SetDate( Now.getYear(), Now.getMonth(), Now.getDate() );
    T020->SetTime( Now.getHour(), Now.getMinute(), Now.getSecond(), Now.getMillisecond() );
    if( IsOldVersion == true )
    {
        if( FOrdType == '1' ) ///< Market 
            Price.Printf( "%06d", 0 );
        else
            Price.Printf( "%06d", FPrice );
        Qty.Printf( "%03d", cQty );
        BeforeQty.Printf( "%03d", Before );
        AfterQty.Printf( "%03d", After );
        memcpy( T020->OrderPrice, Price.c_str(), 6 );
        memcpy( T020->OrderQty, Qty.c_str(), 3 );
        memcpy( T020->BeforeQty, BeforeQty.c_str(),  3 );
        memcpy( T020->AfterQty, AfterQty.c_str(), 3 );    
    }
    else
    {
        if( FOrdType == '1' ) ///< Market 
            Price.Printf( "%09d", 0 );
        else
            Price.Printf( "%09d", FPrice );
        Qty.Printf( "%06d", cQty );
        BeforeQty.Printf( "%06d", Before );
        AfterQty.Printf( "%06d", After );        
        memcpy( T020->OrderPrice, Price.c_str(), 9 );
        memcpy( T020->OrderQty, Qty.c_str(), 6 );
        memcpy( T020->BeforeQty, BeforeQty.c_str(),  6 );
        memcpy( T020->AfterQty, AfterQty.c_str(), 6 );    
    }    
}
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::FillP020( MessageP020* P020, int cQty, int Before,int After, const UFC::AnsiString& PVCID )
{
    UFC::AnsiString Price,Qty,BeforeQty,AfterQty;
    UFC::UDateTime  Now;    
    
    memcpy( P020->BrokerID, FBrokerID.c_str(), 4 );
    memcpy( P020->PVCID, PVCID.c_str(), 2 );
    memcpy( P020->OrderNo, FOID.c_str(), 5 );
    memcpy( P020->InvestorAcno, FAccount.c_str(), 7 );
    P020->InvestorFlag = FInvestorFlag;
    memcpy( P020->StockID, FSymbol.c_str(), 6 );
    P020->BuySellCode = FSide;
    P020->ExchangeCode = FExchangeCode;
    P020->TWSEOrderType = FOrderType;    
    P020->TimeInForce = FTimeInforce;
    P020->OrderType = FOrdType;
    
    P020->SetDate( Now.getYear(), Now.getMonth(), Now.getDate() );
    P020->SetTime( Now.getHour(), Now.getMinute(), Now.getSecond(), Now.getMillisecond() );
    if( IsOldVersion == true )
    {
        Price.Printf( "%06d", FPrice );
        Qty.Printf( "%03d", cQty );
        BeforeQty.Printf( "%03d", Before );
        AfterQty.Printf( "%03d", After );
        memcpy( P020->OrderPrice, Price.c_str(), 6 );
        memcpy( P020->OrderQty, Qty.c_str(), 3 );
        memcpy( P020->BeforeQty, BeforeQty.c_str(),  3 );
        memcpy( P020->AfterQty, AfterQty.c_str(), 3 );    
    }
    else
    {
        Price.Printf( "%09d", FPrice );
        Qty.Printf( "%06d", cQty );
        BeforeQty.Printf( "%06d", Before );
        AfterQty.Printf( "%06d", After );        
        memcpy( P020->OrderPrice, Price.c_str(), 9 );
        memcpy( P020->OrderQty, Qty.c_str(), 6 );
        memcpy( P020->BeforeQty, BeforeQty.c_str(),  6 );
        memcpy( P020->AfterQty, AfterQty.c_str(), 6 );    
    }      
    
}
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::FillO020( MessageO020* O020, int cQty, int Before,int After, const UFC::AnsiString& PVCID )
{
    UFC::AnsiString Price,Qty,BeforeQty,AfterQty,YYMMDD,HHMMSS;
    UFC::UDateTime  Now;    
    
    memcpy( O020->BrokerID, FBrokerID.c_str(), 4 );
    memcpy( O020->PVCID, PVCID.c_str(), 2 );
    memcpy( O020->OrderNo, FOID.c_str(), 5 );
    memcpy( O020->InvestorAcno, FAccount.c_str(), 7 );
    memcpy( O020->StockID, FSymbol.c_str(), 6 );
    O020->BuySellCode = FSide;
    O020->TradeKind = FTradeKind;
    O020->ExchangeCode = '2';
    O020->TWSEOrderType = FOrderType;    
    memcpy( O020->ObjectBroker, FObjectBroker, 4 );
    O020->TimeInForce = FTimeInforce;
    O020->OrderType = FOrdType;    
    
    O020->SetDate( Now.getYear(), Now.getMonth(), Now.getDate() );
    O020->SetTime( Now.getHour(), Now.getMinute(), Now.getSecond(), Now.getMillisecond() );
    if( IsOldVersion == true )
    {
        Price.Printf( "%06d", FPrice );
        Qty.Printf( "%08d", cQty );
        BeforeQty.Printf( "%08d", Before );
        AfterQty.Printf( "%08d", After );
        memcpy( O020->OrderPrice, Price.c_str(), 6 );
        memcpy( O020->OrderQty, Qty.c_str(), 8 );
        memcpy( O020->BeforeQty, BeforeQty.c_str(),  8 );
        memcpy( O020->AfterQty, AfterQty.c_str(), 8 );    
    }
    else
    {
        Price.Printf( "%09d", FPrice );
        Qty.Printf( "%06d", cQty );
        BeforeQty.Printf( "%06d", Before );
        AfterQty.Printf( "%06d", After );        
        memcpy( O020->OrderPrice, Price.c_str(), 9 );
        memcpy( O020->OrderQty,   Qty.c_str(), 6 );
        memcpy( O020->BeforeQty,  BeforeQty.c_str(),  6 );
        memcpy( O020->AfterQty,   AfterQty.c_str(), 6 );    
    }      
}
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::FillO120( MessageO120* O120, int cQty, int Before,int After, const UFC::AnsiString& PVCID )
{
    UFC::AnsiString Price,Qty,BeforeQty,AfterQty;    
    UFC::UDateTime  Now;
    
    
    memcpy( O120->BrokerID, FBrokerID.c_str(), 4 );
    memcpy( O120->PVCID, PVCID.c_str(), 2 );
    memcpy( O120->OrderNo, FOID.c_str(), 5 );
    memcpy( O120->InvestorAcno, FAccount.c_str(), 7 );
    memcpy( O120->StockID, FSymbol.c_str(), 6 );
    O120->InvestorFlag  = FInvestorFlag;    
    O120->BuySellCode   = FSide;
    O120->ExchangeCode  = '2';
    O120->TWSEOrderType = FOrderType;    
    O120->TimeInForce   = FTimeInforce;
    O120->OrderType     = FOrdType;
    
    O120->SetDate( Now.getYear(), Now.getMonth(), Now.getDate() );
    O120->SetTime( Now.getHour(), Now.getMinute(), Now.getSecond(), Now.getMillisecond() );
    Price.Printf( "%09d", FPrice );
    Qty.Printf( "%06d", cQty );
    BeforeQty.Printf( "%06d", Before );
    AfterQty.Printf( "%06d", After );        
    memcpy( O120->OrderPrice, Price.c_str(), 9 );
    memcpy( O120->OrderQty,   Qty.c_str(), 6 );
    memcpy( O120->BeforeQty,  BeforeQty.c_str(),  6 );
    memcpy( O120->AfterQty,   AfterQty.c_str(), 6 );    
}
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::FillA020( MessageA020* A020, int cQty, int Before,int After, const UFC::AnsiString& PVCID )
{
    UFC::AnsiString Price,Qty,BeforeQty,AfterQty,YYMMDD,HHMMSS;

    Qty.Printf( "%012d", cQty );    
    BeforeQty.Printf( "%012d", Before );
    AfterQty.Printf( "%012d", After );    
    memcpy( A020->BrokerID, FBrokerID.c_str(), 4 );
    memcpy( A020->PVCID, PVCID.c_str(), 2 );
    memcpy( A020->OrderNo, FOID.c_str(), 5 );
    memcpy( A020->InvestorAcno, FAccount.c_str(), 7 );
    memcpy( A020->StockID, FSymbol.c_str(), 6 );
    memcpy( A020->OrderQty, Qty.c_str(), 12 );
    memcpy( A020->BeforeQty, BeforeQty.c_str(),  12 );
    memcpy( A020->AfterQty, AfterQty.c_str(), 12 );    
    if( IsOldVersion == true )
    {
        Price.Printf( "%06d", FPrice );
        GetHHMMSS( HHMMSS );
        GetYYMMDD( YYMMDD );    
        memcpy( A020->OrderDate, YYMMDD.c_str(), 6 );
        memcpy( A020->OrderTime, HHMMSS.c_str(), 6 );    
        memcpy( A020->OrderPrice, Price.c_str(), 6 );    
        memcpy( A020->BeforePrice, Price.c_str(),  6 );
        memcpy( A020->AfterPrice, Price.c_str(), 6 );        
    }
    else
    {
        Price.Printf( "%09d", FPrice );        
        GetHHMMSSmm( HHMMSS );
        GetYYYYMMDD( YYMMDD );    
        memcpy( A020->OrderDate, YYMMDD.c_str(), 8 );
        memcpy( A020->OrderTime, HHMMSS.c_str(), 8 );    
        memcpy( A020->OrderPrice, Price.c_str(), 9 );    
        memcpy( A020->BeforePrice, Price.c_str(), 9 );
        memcpy( A020->AfterPrice, Price.c_str(), 9 );        
    }
}
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::FillE020( MessageE020* E020, int cQty, int Before,int After, const UFC::AnsiString& PVCID )
{
    UFC::AnsiString Price,Qty,BeforeQty,AfterQty,YYMMDD,HHMMSS,StkSeq;
    

    Qty.Printf( "%012d", cQty );
    StkSeq.Printf( "%02d", FStkSeqNo );    
    BeforeQty.Printf( "%012d", Before );
    AfterQty.Printf( "%012d", After );
    memcpy( E020->BrokerID, FBrokerID.c_str(), 4 );
    memcpy( E020->PVCID, PVCID.c_str(), 2 );
    memcpy( E020->OrderNo, FOID.c_str(), 5 );
    memcpy( E020->InvestorAcno, FAccount.c_str(), 7 );
    memcpy( E020->StockID, FSymbol.c_str(), 6 );
    memcpy( E020->StkSeqNo, StkSeq.c_str(), 2 );    
    memcpy( E020->OrderQty, Qty.c_str(), 12 );        
    memcpy( E020->BeforeQty, BeforeQty.c_str(),  12 );
    memcpy( E020->AfterQty, AfterQty.c_str(), 12 );    
    if( IsOldVersion == true )
    {
        Price.Printf( "%06d", FPrice );
        GetHHMMSS( HHMMSS );
        GetYYMMDD( YYMMDD );    
        memcpy( E020->OrderPrice, Price.c_str(), 6 );
        memcpy( E020->OrderDate, YYMMDD.c_str(), 6 );
        memcpy( E020->OrderTime, HHMMSS.c_str(), 6 );    
        memcpy( E020->BeforePrice, Price.c_str(),  6 );
        memcpy( E020->AfterPrice, Price.c_str(), 6 );            
    }        
    else
    {
        Price.Printf( "%09d", FPrice );
        GetHHMMSSmm( HHMMSS );
        GetYYYYMMDD( YYMMDD );    
        memcpy( E020->OrderPrice, Price.c_str(), 9 );
        memcpy( E020->OrderDate, YYMMDD.c_str(), 8 );
        memcpy( E020->OrderTime, HHMMSS.c_str(), 8 );    
        memcpy( E020->BeforePrice, Price.c_str(),  9 );
        memcpy( E020->AfterPrice, Price.c_str(), 9 );            
    }        
    
}
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::FillEx020( MessageEx020* Ex020, int cQty, int Before,int After, const UFC::AnsiString& PVCID )
{
    UFC::AnsiString Price,Qty,BeforeQty,AfterQty,YYYYMMDD,HHMMSSmmm,StkSeq;
    
    
    Qty.Printf( "%012d", cQty );
    StkSeq.Printf( "%02d", FStkSeqNo );    
    BeforeQty.Printf( "%012d", Before );
    AfterQty.Printf( "%012d", After );    
    GetTimeString( HHMMSSmmm, FALSE );
    GetYYYYMMDD( YYYYMMDD );    
    memcpy( Ex020->BrokerID, FBrokerID.c_str(), 4 );
    memcpy( Ex020->PVCID, PVCID.c_str(), 2 );
    memcpy( Ex020->OrderNo, FOID.c_str(), 5 );
    memcpy( Ex020->InvestorAcno, FAccount.c_str(), 7 );
    memcpy( Ex020->StockID, FSymbol.c_str(), 6 );
    memcpy( Ex020->StkSeqNo, StkSeq.c_str(), 2 );        
    memcpy( Ex020->OrderQty, Qty.c_str(), 12 );    
    memcpy( Ex020->OrderDate, YYYYMMDD.c_str(), 8 );
    memcpy( Ex020->OrderTime, HHMMSSmmm.c_str(), 8 );
    memcpy( Ex020->BeforeQty, BeforeQty.c_str(),  12 );
    memcpy( Ex020->AfterQty, AfterQty.c_str(), 12 );    
    if( IsOldVersion == true )
    {
        Price.Printf( "%06d", FPrice );
        memcpy( Ex020->OrderPrice, Price.c_str(), 6 );    
        memcpy( Ex020->BeforePrice, Price.c_str(),  6 );
        memcpy( Ex020->AfterPrice, Price.c_str(), 6 );        
    }
    else
    {
        Price.Printf( "%09d", FPrice );
        memcpy( Ex020->OrderPrice, Price.c_str(), 9 );    
        memcpy( Ex020->BeforePrice, Price.c_str(),  9 );
        memcpy( Ex020->AfterPrice, Price.c_str(), 9 );        
    }
    
}
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::FillV020( MessageV020* V020, int cQty, int Before,int After, const UFC::AnsiString& PVCID )
{
    UFC::AnsiString Price,Qty,BeforeQty,AfterQty,YYMMDD,HHMMSS;
    
    
    Qty.Printf( "%06d", cQty );
    BeforeQty.Printf( "%06d", Before );
    AfterQty.Printf( "%06d", After );    
    memcpy( V020->BrokerID, FBrokerID.c_str(), 4 );
    memcpy( V020->PVCID, PVCID.c_str(), 2 );
    memcpy( V020->OrderNo, FOID.c_str(), 5 );
    memcpy( V020->InvestorAcno, FAccount.c_str(), 7 );
    memcpy( V020->StockID, FSymbol.c_str(), 6 );
    memcpy( V020->OrderQty, Qty.c_str(), 6 );    
    memcpy( V020->BeforeQty, BeforeQty.c_str(),  6 );
    memcpy( V020->AfterQty, AfterQty.c_str(), 6 );        
    if( FKeep == TRUE )
        V020->Keep = 'Y';
    else
        V020->Keep = ' ';
    if( IsOldVersion == true )
    {
        Price.Printf( "%07d", FPrice );
        GetHHMMSS( HHMMSS );
        GetYYMMDD( YYMMDD );    
        memcpy( V020->OrderPrice, Price.c_str(), 7 );    
        memcpy( V020->OrderDate, YYMMDD.c_str(), 6 );
        memcpy( V020->OrderTime, HHMMSS.c_str(), 6 );
    }
    else
    {
        Price.Printf( "%09d", FPrice );
        GetHHMMSSmm( HHMMSS );
        GetYYYYMMDD( YYMMDD );    
        memcpy( V020->OrderPrice, Price.c_str(), 9 );    
        memcpy( V020->OrderDate, YYMMDD.c_str(), 8 );
        memcpy( V020->OrderTime, HHMMSS.c_str(), 8 );
    }
    
}
//----------------------------------------------------------------------------------------------------------------------
//
//   Confirm functions
//
//----------------------------------------------------------------------------------------------------------------------
MessageT020* OrderInfo::ConfirmT020( const UFC::AnsiString& PVCID )
{
    MessageT020* T020 = new MessageT020( IsTSEC );
    
    FillT020( T020, FQty, 0, FQty, PVCID );
    if( FSide == 'B' )
        T020->SetFunctionCode( 1 );
    else
        T020->SetFunctionCode( 2 );
    return T020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageP020* OrderInfo::ConfirmP020( const UFC::AnsiString& PVCID )
{
    MessageP020* P020 = new MessageP020( IsTSEC );
    
    FillP020( P020, FQty, 0, FQty, PVCID );
    if( FSide == 'B' )
        P020->SetFunctionCode( 1 );
    else
        P020->SetFunctionCode( 2 );
    return P020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageO020* OrderInfo::ConfirmO020( const UFC::AnsiString& PVCID )
{
    MessageO020* O020 = new MessageO020(  IsTSEC );
    
    FillO020( O020, FQty, 0, FQty, PVCID );
    if( FSide == 'B' )
        O020->SetFunctionCode( 1 );
    else
        O020->SetFunctionCode( 2 );
    return O020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageO120* OrderInfo::ConfirmO120( const UFC::AnsiString& PVCID )
{
    MessageO120* O120 = new MessageO120( IsTSEC );
    
    FillO120( O120, FQty, 0, FQty, PVCID );
    if( FSide == 'B' )
        O120->SetFunctionCode( 1 );
    else
        O120->SetFunctionCode( 2 );
    return O120;
}
//----------------------------------------------------------------------------------------------------------------------
MessageA020* OrderInfo::ConfirmA020( const UFC::AnsiString& PVCID )
{
    MessageA020* A020 = new MessageA020();
    
    FillA020( A020, FQty, 0, FQty, PVCID );
    A020->SetFunctionCode( 1 );
    return A020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageE020* OrderInfo::ConfirmE020( const UFC::AnsiString& PVCID )
{
    MessageE020* E020 = new MessageE020( IsTSEC );
    
    FillE020( E020, FQty, 0, FQty, PVCID );
    E020->SetFunctionCode( 1 );
    return E020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageEx020* OrderInfo::ConfirmEx020( const UFC::AnsiString& PVCID )
{
    MessageEx020* Ex020 = new MessageEx020( IsTSEC );
    
    FillEx020( Ex020, FQty, 0, FQty, PVCID );
    Ex020->SetFunctionCode( 1 );
    return Ex020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageV020* OrderInfo::ConfirmV020( const UFC::AnsiString& PVCID )
{
    MessageV020* V020 = new MessageV020( IsTSEC );
    
    FillV020( V020, FQty, 0, FQty, PVCID );
    V020->SetFunctionCode( 1 );
    return V020;
}
//----------------------------------------------------------------------------------------------------------------------
//
//   Cancel functions
//
//----------------------------------------------------------------------------------------------------------------------
MessageT020* OrderInfo::CancelT020(  const UFC::AnsiString& PVCID )
{
    MessageT020* T020 = new MessageT020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    
    FillT020( T020, Qty, Qty, 0, PVCID );    
    T020->SetFunctionCode( 4 );
    return T020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageP020* OrderInfo::CancelP020(  const UFC::AnsiString& PVCID )
{
    MessageP020* P020 = new MessageP020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    
    FillP020( P020, Qty, Qty, 0, PVCID );
    P020->SetFunctionCode( 4 );
    return P020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageO020* OrderInfo::CancelO020(  const UFC::AnsiString& PVCID )
{
    MessageO020* O020 = new MessageO020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    
    FillO020( O020, Qty, Qty, 0, PVCID );
    O020->SetFunctionCode( 4 );
    return O020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageO120* OrderInfo::CancelO120(  const UFC::AnsiString& PVCID )
{
    MessageO120* O120 = new MessageO120( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    
    FillO120( O120, Qty, Qty, 0, PVCID );
    O120->SetFunctionCode( 4 );
    return O120;
}
//----------------------------------------------------------------------------------------------------------------------
MessageA020* OrderInfo::CancelA020(  const UFC::AnsiString& PVCID )
{
    MessageA020* A020 = new MessageA020( );
    int Qty = FQty - FFillQty - FReduceQty;
    
    FillA020( A020, Qty, Qty, 0, PVCID );
    A020->SetFunctionCode( 2 );
    return A020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageE020* OrderInfo::CancelE020(  const UFC::AnsiString& PVCID )
{
    MessageE020* E020 = new MessageE020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    
    FillE020( E020, Qty, Qty, 0, PVCID );
    E020->SetFunctionCode( 2 );
    return E020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageEx020* OrderInfo::CancelEx020(  const UFC::AnsiString& PVCID )
{
    MessageEx020* Ex020 = new MessageEx020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    
    FillEx020( Ex020, Qty, Qty, 0, PVCID );
    Ex020->SetFunctionCode( 2 );
    return Ex020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageV020* OrderInfo::CancelV020(  const UFC::AnsiString& PVCID )
{
    MessageV020* V020 = new MessageV020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    
    FillV020( V020, Qty, Qty, 0, PVCID );
    V020->SetFunctionCode( 2 );
    return V020;
}
//----------------------------------------------------------------------------------------------------------------------
//
//   Replace functions
//
//----------------------------------------------------------------------------------------------------------------------
MessageT020* OrderInfo::ReplaceT020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled )
{
    int BeforeQty = FQty - FFillQty - FReduceQty;
    int AfterQty;    
    MessageT020* T020 = new MessageT020( IsTSEC );
        
    if( Qty > BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
        T020->SetStatusCode( 32 );
    }
    else if( Qty == BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
    }
    else
    {
        Canceled = FALSE;
        FReduceQty += Qty;
        AfterQty = FQty - FFillQty - FReduceQty;
    }    
    FillT020( T020, Qty, BeforeQty, AfterQty, PVCID );
    T020->SetFunctionCode( 3 );
    return T020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageT020* OrderInfo::ReplaceT020(  const UFC::AnsiString& PVCID, int Px )
{
    int Qty = FQty - FFillQty;    
    MessageT020* T020 = new MessageT020( IsTSEC );
    
    FPrice = Px;
    FillT020( T020, Qty, Qty, Qty, PVCID );
    T020->SetFunctionCode( 6 );
    return T020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageP020* OrderInfo::ReplaceP020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled )
{
    int BeforeQty = FQty - FFillQty - FReduceQty;
    int AfterQty;    
    MessageP020* P020 = new MessageP020( IsTSEC );
        
    if( Qty > BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
        P020->SetStatusCode( 32 );
    }
    else if( Qty == BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
    }
    else
    {
        Canceled = FALSE;
        FReduceQty += Qty;
        AfterQty = FQty - FFillQty - FReduceQty;
    }    
    FillP020( P020, Qty, BeforeQty, AfterQty, PVCID );
    P020->SetFunctionCode( 3 );
    return P020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageO020* OrderInfo::ReplaceO020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled )
{
    int BeforeQty = FQty - FFillQty - FReduceQty;
    int AfterQty;    
    MessageO020* O020 = new MessageO020( IsTSEC );
        
    if( Qty > BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
        O020->SetStatusCode( 32 );
    }
    else if( Qty == BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
    }
    else
    {
        Canceled = FALSE;
        FReduceQty += Qty;
        AfterQty = FQty - FFillQty - FReduceQty;
    }    
    FillO020( O020, Qty, BeforeQty, AfterQty, PVCID );
    O020->SetFunctionCode( 3 );
    return O020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageO120* OrderInfo::ReplaceO120(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled )
{
    int BeforeQty = FQty - FFillQty - FReduceQty;
    int AfterQty;    
    MessageO120* O120 = new MessageO120( IsTSEC );
        
    if( Qty > BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
        O120->SetStatusCode( 32 );
    }
    else if( Qty == BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
    }
    else
    {
        Canceled = FALSE;
        FReduceQty += Qty;
        AfterQty = FQty - FFillQty - FReduceQty;
    }    
    FillO120( O120, Qty, BeforeQty, AfterQty, PVCID );
    O120->SetFunctionCode( 3 );
    return O120;
}
//----------------------------------------------------------------------------------------------------------------------
MessageA020* OrderInfo::ReplaceA020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled )
{
    int BeforeQty = FQty - FFillQty - FReduceQty;
    int AfterQty;    
    MessageA020* A020 = new MessageA020();
        
    if( Qty > BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
        A020->SetStatusCode( 25 );
    }
    else if( Qty == BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
    }
    else
    {
        Canceled = FALSE;
        FReduceQty += Qty;
        AfterQty = FQty - FFillQty - FReduceQty;
    }    
    FillA020( A020, Qty, BeforeQty, AfterQty, PVCID );
    A020->SetFunctionCode( 3 );
    return A020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageE020* OrderInfo::ReplaceE020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled )
{
    int BeforeQty = FQty - FFillQty - FReduceQty;
    int AfterQty;    
    MessageE020* E020 = new MessageE020( IsTSEC );
        
    if( Qty > BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
        E020->SetStatusCode( 25 );
    }
    else if( Qty == BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
    }
    else
    {
        Canceled = FALSE;
        FReduceQty += Qty;
        AfterQty = FQty - FFillQty - FReduceQty;
    }    
    FillE020( E020, Qty, BeforeQty, AfterQty, PVCID );
    E020->SetFunctionCode( 3 );
    return E020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageEx020* OrderInfo::ReplaceEx020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled )
{
    int BeforeQty = FQty - FFillQty - FReduceQty;
    int AfterQty;    
    MessageEx020* Ex020 = new MessageEx020( IsTSEC );
        
    if( Qty > BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
        Ex020->SetStatusCode( 25 );
    }
    else if( Qty == BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
    }
    else
    {
        Canceled = FALSE;
        FReduceQty += Qty;
        AfterQty = FQty - FFillQty - FReduceQty;
    }    
    FillEx020( Ex020, Qty, BeforeQty, AfterQty, PVCID );
    Ex020->SetFunctionCode( 3 );
    return Ex020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageV020* OrderInfo::ReplaceV020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled )
{
    int BeforeQty = FQty - FFillQty - FReduceQty;
    int AfterQty;    
    MessageV020* V020 = new MessageV020( IsTSEC );
        
    if( Qty > BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
        V020->SetStatusCode( 20 );
    }
    else if( Qty == BeforeQty )
    {
        Canceled = TRUE;
        AfterQty = 0;
    }
    else
    {
        Canceled = FALSE;
        FReduceQty += Qty;
        AfterQty = FQty - FFillQty - FReduceQty;
    }    
    FillV020( V020, Qty, BeforeQty, AfterQty, PVCID );
    V020->SetFunctionCode( 3 );
    return V020;
}
//----------------------------------------------------------------------------------------------------------------------
//
//   Query Order Status functions
//
//----------------------------------------------------------------------------------------------------------------------
MessageT020* OrderInfo::QueryT020(  const UFC::AnsiString& PVCID )
{
    MessageT020* T020 = new MessageT020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    FillT020( T020, 0, Qty, Qty, PVCID );    
    T020->SetFunctionCode( 5 );
    return T020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageP020* OrderInfo::QueryP020(  const UFC::AnsiString& PVCID )
{
    MessageP020* P020 = new MessageP020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    FillP020( P020, 0, Qty, Qty, PVCID );    
    P020->SetFunctionCode( 5 );
    return P020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageO020* OrderInfo::QueryO020(  const UFC::AnsiString& PVCID )
{
    MessageO020* O020 = new MessageO020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    FillO020( O020, 0, Qty, Qty, PVCID );    
    O020->SetFunctionCode( 5 );
    return O020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageO120* OrderInfo::QueryO120(  const UFC::AnsiString& PVCID )
{
    MessageO120* O120 = new MessageO120( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    FillO120( O120, 0, Qty, Qty, PVCID );    
    O120->SetFunctionCode( 5 );
    return O120;
}
//----------------------------------------------------------------------------------------------------------------------
MessageA020* OrderInfo::QueryA020(  const UFC::AnsiString& PVCID )
{
    MessageA020* A020 = new MessageA020();
    int Qty = FQty - FFillQty - FReduceQty;
    FillA020( A020, 0, Qty, Qty, PVCID );    
    A020->SetFunctionCode( 4 );
    return A020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageE020* OrderInfo::QueryE020(  const UFC::AnsiString& PVCID )
{
    MessageE020* E020 = new MessageE020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    FillE020( E020, 0, Qty, Qty, PVCID );    
    E020->SetFunctionCode( 4 );
    return E020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageEx020* OrderInfo::QueryEx020(  const UFC::AnsiString& PVCID )
{
    MessageEx020* Ex020 = new MessageEx020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    FillEx020( Ex020, 0, Qty, Qty, PVCID );    
    Ex020->SetFunctionCode( 4 );
    return Ex020;
}
//----------------------------------------------------------------------------------------------------------------------
MessageV020* OrderInfo::QueryV020(  const UFC::AnsiString& PVCID )
{
    MessageV020* V020 = new MessageV020( IsTSEC );
    int Qty = FQty - FFillQty - FReduceQty;
    FillV020( V020, 0, Qty, Qty, PVCID );    
    V020->SetFunctionCode( 4 );
    return V020;
}
//----------------------------------------------------------------------------------------------------------------------
const char* OrderInfo::GetFIXSenderSubID( void )
{
    switch( FSession )
    {
        case tsNormal: return "0";
        case tsPost:   return "7";
        case tsOdd:    return "2";
        case tsOddEx:  return "C";
        default:       return "0";
    }
}
//----------------------------------------------------------------------------------------------------------------------
//
//  Make fill execution
//
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::FillR3Body( R3Body* R3, int cQty, int Seq, int TotalSeq )
{
    UFC::AnsiString Price,Qty,SeqStr,TotalSeqStr;
    UFC::UDateTime  Now;    
    
    SeqStr.Printf( "%06d", Seq );   
    TotalSeqStr.Printf( "%08d", TotalSeq );       
    Qty.Printf( "%08d", cQty );   
    memcpy( R3->FStockID,FSymbol.c_str(), 6 );    
    switch( FSession )
    {
        case tsNormal:
        case tsPost:   R3->FEXCD = '0';break;
        case tsOdd:    
        case tsOddEx:  R3->FEXCD = '2';break;
        default:       R3->FEXCD = '0';break;
    }
    R3->FBuySellCode = FSide;
    memcpy( R3->FOrderNo, FOID.c_str(), 5 );
    memcpy( R3->FInvestorACNo, FAccount.c_str(), 7 );
    memcpy( R3->FSeq, SeqStr.c_str(), 6 );
    memcpy( R3->FBroker, FBrokerID.c_str(), 4 );
    memcpy( R3->FRecNo, TotalSeqStr.c_str(), 8 );
    memcpy( R3->FQuntity, Qty.c_str(), 8 );            
    R3->FOrderType = FOrderType;    
    R3->Fmarks = ' ';
    if( IsOldVersion == true )
    {
        Price.Printf( "%06d", FPrice );
        memcpy( R3->FPrice, Price.c_str(), 6 );
    }
    else
    {
        Price.Printf( "%09d", FPrice );        
        memcpy( R3->FPrice, Price.c_str(), 9 );        
    }
    R3->SetTime( Now.getHour(), Now.getMinute(), Now.getSecond(), Now.getMillisecond() );
}
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::Fill( char* FillStr, int Seq, int TotalSeq )
{
    int FillQty = FQty - FFillQty - FReduceQty;
    FFillQty += FillQty;
    R3Body R3;
    
    FillR3Body( &R3, FillQty, Seq, TotalSeq );
    R3.AsString( FillStr );
}
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::PartialFill(  char* FillStr, int Seq, int TotalSeq, BOOL& Canceled )
{
    int FillQty = FQty - FFillQty - FReduceQty;
    R3Body R3;    
    
    if( FillQty == 1 )
    {        
        Canceled = TRUE;
        FFillQty += FillQty;
    }
    else
    {
        Canceled = FALSE;
        FillQty /= 2;
        FFillQty += FillQty;
    }    
    FillR3Body( &R3, FillQty, Seq, TotalSeq );
    R3.AsString( FillStr );
}
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::PartialFill( char* FillStr, int Seq, int TotalSeq, int Qty, BOOL& Canceled )
{
    int LeavesQty = FQty - FFillQty - FReduceQty;
    R3Body R3;    
    
    if( Qty >= LeavesQty ) ///< Filled.
    {        
        Canceled = TRUE;
        FFillQty += LeavesQty;
    }
    else ///<PartialFill
    {
        Canceled = FALSE;        
        FFillQty += Qty;
    }    
    FillR3Body( &R3, Qty, Seq, TotalSeq );
    R3.AsString( FillStr );
}
//----------------------------------------------------------------------------------------------------------------------
void OrderInfo::FillByRule( char* FillStr, int Seq, int TotalSeq, BOOL& Canceled )
{
    int LeavesQty = FQty - FFillQty - FReduceQty;
    R3Body R3;    
    
    if( FFilledQty >= LeavesQty ) ///< Filled.
    {        
        FFilledQty = LeavesQty;
        Canceled = TRUE;
        FFillQty += LeavesQty;
    }
    else ///<PartialFill
    {
        Canceled = FALSE;        
        FFillQty += FFilledQty;
    }       
    if( IsOldVersion == true )
        FPrice = (int)(FFilledPx*100);
    else
        FPrice = (int)(FFilledPx*10000);
    FillR3Body( &R3, FFilledQty, Seq, TotalSeq );
    R3.AsString( FillStr );
}
//-----------------------------------------------------------------------------------------    
int OrderInfo::CountDown( void )
{
    FCountDown--;
    if( FCountDown > 0 )
        UFC::BufferedLog::Printf(" Order[%s] Matched after[%d] sec.", FOID.c_str(), FCountDown );    
    return FCountDown;
}
//-----------------------------------------------------------------------------------------    
BOOL OrderInfo::IsSetMatchRule( void )
{
    if( FFilledQty == 0 && FCountDown == 0 )
        return FALSE;
    return TRUE;
}       
//-----------------------------------------------------------------------------------------            
void OrderInfo::SetMatchRule( int Sec, int Qty, double Px )
{
    FCountDown = Sec;
    FFilledQty = Qty;
    FFilledPx  = Px;    
}
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
