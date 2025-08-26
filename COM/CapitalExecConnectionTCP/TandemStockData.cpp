
#include "TandemObject.h"
#include "TMPMessage.h"      //TAIFEXLib
#include "MultilegSymbol.h"  //TaifexLib
//------------------------------------------------------------------------------
extern UFC::AnsiString gSpeedySourceID;    
extern UFC::AnsiString gSpeedySeqPrefix; 
//------------------------------------------------------------------------------
namespace capital
{
//------------------------------------------------------------------------------
TandemStockData::TandemStockData( TExecutionReportMessage *ExecRptPtr )
{
    FExecRptMarket      = ExecRptPtr->GetMarket();
    FExecRptOrderStatus = ExecRptPtr->GetOrderStatus(); 
    int             Code = UFC::AnsiString( ExecRptPtr->GetStatusCode()).ToInt();   
    CUserData       userData( ExecRptPtr->GetData() );    
    UFC::AnsiString OID( ExecRptPtr->GetOrderID());
    UFC::AnsiString BrokerID( ExecRptPtr->GetBrokerID());
    UFC::AnsiString Account( ExecRptPtr->GetAccount());
    UFC::AnsiString Symbol( ExecRptPtr->GetSymbol() );
    UFC::AnsiString TTime( ExecRptPtr->GetTransactTime());
    UFC::AnsiString TWSEOrdType( ExecRptPtr->GetTSEOrderType() );
    Int64 NID = ExecRptPtr->GetNID();
    Int64 ONID = ExecRptPtr->GetOrigNID();
    UFC::UDateTime Now;
    
    OID.PadThis( 5, '0' );
    BrokerID.PadThis( 4, '0' );
    Account.PadThis( 7, '0' );
    Symbol.PadThis( 6, ' ' );
    TTime.PadThis( 9, '0' );
    TWSEOrdType.PadThis( 1, '0' );
    if( FExecRptMarket == nsOrderMessageDefine::mTSE )
        FMO_MARKET_MARK2 = "S"; //X(1) S: TWSE O: OTC P: ES
    else if( FExecRptMarket == nsOrderMessageDefine::mOTC )
        FMO_MARKET_MARK2 = "O"; //X(1) S: TWSE O: OTC P: ES
    else if( FExecRptMarket == nsOrderMessageDefine::mES )
        FMO_MARKET_MARK2 = "P"; //X(1) S: TWSE O: OTC P: ES
    switch( FExecRptOrderStatus )  //X(1) 2: Fill D: Cancel by Ex Z: Confirm
    {
        case nsOrderMessageDefine::osNew:
                                   ONID = NID;
                                   UFC::BufferedLog::Printf( " New NID[%010u] Orig NID[%010u]", NID, ONID );
                                   FMO_RPT_KIND = "Z";
                                   break;
        case nsOrderMessageDefine::osReplaced:
                                   UFC::BufferedLog::Printf( " Replaced NID[%010u] Orig NID[%010u]", NID, ONID );
                                   FMO_RPT_KIND = "Z";
                                   break;
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:
                                   ONID = NID;
                                   UFC::BufferedLog::Printf( " Fill NID[%010u] Orig NID[%010u]", NID, ONID );
                                   FMO_RPT_KIND = "2";
                                   break;        
        case nsOrderMessageDefine::osCanceled:                                       
                                   UFC::BufferedLog::Printf( " Canceled NID[%010u] Orig NID[%010u]", NID, ONID );
                                   if( Code == 51 ) 
                                       FMO_RPT_KIND = "D";
                                   else
                                       FMO_RPT_KIND = "Z";
                                   break;
        default: FMO_RPT_KIND = "Z"; break;                           
    }    
    UniqueExecID( FMO_EXECID , ExecRptPtr );            //X(12) 
    FMO_FILLER1.PadThis(4,' ');      //X(4)
    FRO_INPUT_SEQNO.Printf( "%s%010u", gSpeedySeqPrefix.c_str(), NID );  //X(13)           
    FRO_INPUT_KEY.Printf( "%s%010u", gSpeedySeqPrefix.c_str(), ONID  );  //X(13)           
    FRO_ORDER_NO = OID;     //X(5)           
    FRO_FILLER2 = " ";      //X(1) 
    switch( FExecRptOrderStatus )  //X(2) "01": Buy "02": Sell "03": Reduce "04": Cancel  
    {
        case nsOrderMessageDefine::osNew:
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:  
                                   if( ExecRptPtr->GetSide() == nsOrderMessageDefine::sBuy ) 
                                       FMO_FUNCTION_CODE = "01";
                                   else
                                       FMO_FUNCTION_CODE = "02";
                                   break;
        case nsOrderMessageDefine::osReplaced:
                                   if( ExecRptPtr->IsReplacePx() == TRUE )
                                       FMO_FUNCTION_CODE = "06";
                                   else
                                       FMO_FUNCTION_CODE = "03";
                                   break;        
        case nsOrderMessageDefine::osCanceled:                                                                          
                                   FMO_FUNCTION_CODE = "04";
                                   break;
        default:  FMO_FUNCTION_CODE = "01"; break;                           
    }         
    FMO_MARKET_MARK = FMO_MARKET_MARK2;  //X(1) S: TWSE O: OTC P: ES          
    switch ( ExecRptPtr->GetTradingSessionID() )//X(1) 0:Normal 2:Odd 7:Post          
    {
        case nsOrderMessageDefine::tsNormal:      FMO_AP_CODE = "0";
                                                  FMO_EXCHANGE_CODE = "0";
                                                  break;
	case nsOrderMessageDefine::tsOddLot:
        case nsOrderMessageDefine::tsIntradayOdd: FMO_AP_CODE = "2";
                                                  FMO_EXCHANGE_CODE = "2";
                                                  break;
	case nsOrderMessageDefine::tsOffHour:	  FMO_AP_CODE = "7";
                                                  FMO_EXCHANGE_CODE = "0";
                                                  break;
        default: FMO_AP_CODE = "0";
                 FMO_EXCHANGE_CODE = "0";
                 break;        
    }
          
    FMO_BROKER_ID = BrokerID;    //X(4)            
    FMO_ACNO = Account;         //9(7)           
    FMO_SUB_ACNO.PadThis( 7, '0' );     //9(7) "0000000"          
    FMO_OTA_MARK = " ";     //X(1) " "          
    FMO_OTA_ORDER_ID.PadThis( 10, ' ' ); //X(10)                    
    if( ExecRptPtr->GetSide() == nsOrderMessageDefine::sBuy ) //X(1) B:Buy S:Sell           
        FMO_BUYSELL = "B";
    else
        FMO_BUYSELL = "S";         
    FMO_COMPANY_NO = Symbol;   //X(6) Symbol         
    FMO_STOCK_NOS.Printf("%09d", ExecRptPtr->GetOrderQty() );    //9(9)          
    FMO_PRICE.Printf("%09lld", DoubleToInt64( ExecRptPtr->GetPrice(), 4 ) );        //9(5)V9(4)              ;
    FMO_PRICE_MARK = " ";   //X(1) " "          
    FMO_ORDER_TYPE = TWSEOrdType;   //X(1) TWSE OrderType           
    if( ExecRptPtr->GetOrderType() == nsOrderMessageDefine::otMarket)//X(1) 1: Market 2:Limit         
        FMO_PRICE_TYPE = "1";
    else 
        FMO_PRICE_TYPE = "2";
    switch(ExecRptPtr->GetTimeInForce())//X(1) 0: ROD 3:IOC 4:FOK
    {
        case nsOrderMessageDefine::tifROD:  FMO_TIMEINFORCE = "0"; break;
        case nsOrderMessageDefine::tifIOC:  FMO_TIMEINFORCE = "3"; break;
        case nsOrderMessageDefine::tifFOK:  FMO_TIMEINFORCE = "4"; break;
        default:                            FMO_TIMEINFORCE = "0"; break;
    }
    FMO_SELLER       = "0000";       //9(4)     
    FMO_C_KEY.Printf("  %03d%02d%02d000000000", Now.getYear()-1911, Now.getMonth(), Now.getDate());  //9(18) "  YYYMMDD000000000"  
    FMO_SettlType_63 = " "; //X(1) 0: Regular
    FMO_FILLER3.PadThis( 5,' ');      //X(5) 
    FMO_ORDER_DATE.Printf("%04d%02d%02d", Now.getYear()-1911, Now.getMonth(), Now.getDate());  // 1.ODK-DATE 9(8) 0    
    FMO_ORDER_TIME = TTime;   //9(9) HHMMSSmmm         
    if( FExecRptOrderStatus == nsOrderMessageDefine::osPartiallyFilled ||
        FExecRptOrderStatus == nsOrderMessageDefine::osFilled )
        TSEFillArea( FMO_TSE_AREA, ExecRptPtr );      //X(25)
    else
        TSEConfirmArea( FMO_TSE_AREA, ExecRptPtr );     //X(25)
    FMO_INPUT_SOURCE = gSpeedySourceID; //X(2) " s"          
    FMO_INPUT_DEVICE = userData.GetSourceIP(); //X(16) IP 
    FMO_INPUT_DEVICE.PadThis( 16, ' ' );
    FMO_INPUT_TEL    = "000000";    //9(6) "000000"
    FMO_ARDAY_FILEID = "  "; //X(2) "  "
    FMO_ORDAY_GROUP  = "0000";  //9(4) "0000" 
    FMO_CHANNEL      = "3";      //X(1) '3' DMA
    // FMO_EXCHANGE_CODE;//X(1) 0: Normal 1: Mass 2:Odd
    FMO_RM_MARK      = " ";      //X(1) " "
    FMO_CURRENCY     = "   ";     //X(3) "   "
    GetStockSource( FMO_STOCK_SOURCE, ExecRptPtr ); // FMO_STOCK_SOURCE = "1"; //9(1) 0: Normal 1:TSCD 2:Custodian bank 8:Day Trade
    FMO_OFFSET_MARK  = " ";  //X(1) " "
    FMO_BOOKING_MARK = " "; //X(1) " " 
    FMO_FILLER4.PadThis( 19, ' ');      //X(19)       
}
//------------------------------------------------------------------------------
TandemStockData::~TandemStockData( )
{
}
//------------------------------------------------------------------------------
void TandemStockData::GetStockSource( UFC::AnsiString& FMO_STOCK_SOURCE, TExecutionReportMessage *ExecRptPtr )
{
    UFC::AnsiString Str;
    UFC::PStringList StringList;
    StringList.SetStrings( ExecRptPtr->GetData(), ",^" );

    FMO_STOCK_SOURCE = "1";
    for( register int i = 0; i < StringList.ItemCount(); i++ )
    {
        Str = StringList.GetItem( i );
        Int32 Pos = Str.AnsiPos( "=" );

        if( Pos > 0 && Str.LastChar() != '=' )
        {
            UFC::AnsiString Name  = Str.SubString( 0, Pos );
            UFC::AnsiString Value = Str.SubString( Pos + 1, Str.Length() - Name.Length() - 1 );

            if( Name.AnsiCompare( "SSRC" ) == 0 )
                FMO_STOCK_SOURCE = Value;
        }
    }
    FMO_STOCK_SOURCE.PadThis( 1, '1' );
}
//------------------------------------------------------------------------------
void TandemStockData::UniqueExecID( UFC::AnsiString& FMO_EXECID, TExecutionReportMessage *ExecRptPtr )
{
    if( ExecRptPtr->GetOrderStatus() == nsOrderMessageDefine::osPartiallyFilled ||
        ExecRptPtr->GetOrderStatus() == nsOrderMessageDefine::osFilled )
    {
        UFC::AnsiString RecNo( ExecRptPtr->GetRecNo() );
        
        FMO_EXECID.Printf( "%04d%08d", ExecRptPtr->GetReportSequence() , RecNo.ToInt() );
    }
    else
    {
        FMO_EXECID.Printf( "%s%010lld", gSpeedySourceID.c_str(), ExecRptPtr->GetNID() );
    }
}
//------------------------------------------------------------------------------
void TandemStockData::TSEFillArea( UFC::AnsiString& FMO_TSE_AREA, TExecutionReportMessage *ExecRptPtr )
{
    FMO_TSE_AREA.Printf( "%09d%09lld       ", ExecRptPtr->GetOrderQty(), DoubleToInt64( ExecRptPtr->GetPrice(), 4 ) );
}
//------------------------------------------------------------------------------
void TandemStockData::TSEConfirmArea( UFC::AnsiString& FMO_TSE_AREA, TExecutionReportMessage *ExecRptPtr )
{
    UFC::AnsiString Code( ExecRptPtr->GetStatusCode() );
    UFC::AnsiString PVC( ExecRptPtr->GetPVC() );
    
    Code.PadThis( 2, '0' );
    PVC.PadThis( 2, '0' );
    FMO_TSE_AREA.Printf( "%09d%09d%s %s  ", ExecRptPtr->GetBeforeQty(), ExecRptPtr->GetAfterQty(), Code.c_str(), PVC.c_str() ); 
}
//------------------------------------------------------------------------------
void TandemStockData::GenerateTandemData( Int8* TandemDataBuffer ) 
{
    Int8 *CurPosition = TandemDataBuffer;
    
    std::memcpy( CurPosition, FMO_MARKET_MARK2.c_str(), 1 ); //X(1) S: TWSE O: OTC P: ES
    CurPosition += 1;        
    std::memcpy( CurPosition, FMO_RPT_KIND.c_str(), 1 );     //X(1) 2: Fill D: Cancel by Ex Z: Confirm
    CurPosition += 1;        
    std::memcpy( CurPosition, FMO_EXECID.c_str(), 12 );       //X(12) 
    CurPosition += 12;        
    std::memcpy( CurPosition, FMO_FILLER1.c_str(), 4 );      //X(4)
    CurPosition += 4;        
    std::memcpy( CurPosition, FRO_INPUT_SEQNO.c_str(), 13 );  //X(13)           
    CurPosition += 13;        
    std::memcpy( CurPosition, FRO_INPUT_KEY.c_str(), 13 );    //X(13)           
    CurPosition += 13;        
    std::memcpy( CurPosition, FRO_ORDER_NO.c_str(), 5 );     //X(5)           
    CurPosition += 5;        
    std::memcpy( CurPosition, FRO_FILLER2.c_str(), 1 );      //X(1)           
    CurPosition += 1;        
    std::memcpy( CurPosition, FMO_FUNCTION_CODE.c_str(), 2 );//X(2) "01": Buy "02": Sell "03": Reduce "04": Cancel  
    CurPosition += 2;        
    std::memcpy( CurPosition, FMO_MARKET_MARK.c_str(), 1 );  //X(1) S: TWSE O: OTC P: ES          
    CurPosition += 1;        
    std::memcpy( CurPosition, FMO_AP_CODE.c_str(), 1 );      //X(1) 0:Normal 2:Odd 7:Post          
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_BROKER_ID.c_str(), 4 );    //X(4)            
    CurPosition += 4;      
    std::memcpy( CurPosition, FMO_ACNO.c_str(), 7 );         //9(7)           
    CurPosition += 7;      
    std::memcpy( CurPosition, FMO_SUB_ACNO.c_str(), 7 );     //9(7) "0000000"          
    CurPosition += 7;      
    std::memcpy( CurPosition, FMO_OTA_MARK.c_str(), 1 );     //X(1) " "          
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_OTA_ORDER_ID.c_str(), 10 ); //X(10)                    
    CurPosition += 10;      
    std::memcpy( CurPosition, FMO_BUYSELL.c_str(), 1 );      //X(1) B:Buy S:Sell          
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_COMPANY_NO.c_str(), 6 );   //X(6) Symbol         
    CurPosition += 6;      
    std::memcpy( CurPosition, FMO_STOCK_NOS.c_str(), 9 );    //9(9)          
    CurPosition += 9;      
    std::memcpy( CurPosition, FMO_PRICE.c_str(), 9 );        //9(5)V9(4)          
    CurPosition += 9;      
    std::memcpy( CurPosition, FMO_PRICE_MARK.c_str(), 1 );   //X(1) " "          
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_ORDER_TYPE.c_str(), 1 );   //X(1) TWSE OrderType           
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_PRICE_TYPE.c_str(), 1 );   //X(1) 1: Market 2:Limit         
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_TIMEINFORCE.c_str(), 1 );  //X(1) 0: ROD 3:IOC 4:FOK
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_SELLER.c_str(), 4 );       //9(4) 
    CurPosition += 4;      
    std::memcpy( CurPosition, FMO_C_KEY.c_str(), 18 );        //9(18)"  0000000         "  
    CurPosition += 18;      
    std::memcpy( CurPosition, FMO_SettlType_63.c_str(), 1 ); //X(1) 0: Regular
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_FILLER3.c_str(), 5 );      //X(5) 
    CurPosition += 5;      
    std::memcpy( CurPosition, FMO_ORDER_DATE.c_str(), 8 );   //9(8) YYYYMMDD         
    CurPosition += 8;      
    std::memcpy( CurPosition, FMO_ORDER_TIME.c_str(), 9 );   //9(9) HHMMSSmmm         
    CurPosition += 9;      
    std::memcpy( CurPosition, FMO_TSE_AREA.c_str(), 25 );     //X(25)
    CurPosition += 25;      
    std::memcpy( CurPosition, FMO_INPUT_SOURCE.c_str(), 2 ); //X(2) "s "          
    CurPosition += 2;      
    std::memcpy( CurPosition, FMO_INPUT_DEVICE.c_str(), 16 ); //X(16) IP 
    CurPosition += 16;      
    std::memcpy( CurPosition, FMO_INPUT_TEL.c_str(), 6 );    //9(6) "000000"
    CurPosition += 6;      
    std::memcpy( CurPosition, FMO_ARDAY_FILEID.c_str(), 2 ); //X(2) "  "
    CurPosition += 2;      
    std::memcpy( CurPosition, FMO_ORDAY_GROUP.c_str(), 4 );  //9(4) "0000" 
    CurPosition += 4;      
    std::memcpy( CurPosition, FMO_CHANNEL.c_str(), 1 );      //X(1) '3' DMA
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_EXCHANGE_CODE.c_str(), 1 );//X(1) 0: Normal 1: Mass 2:Odd
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_RM_MARK.c_str(), 1 );      //X(1) " "
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_CURRENCY.c_str(), 3 );     //X(3) "   "
    CurPosition += 3;      
    std::memcpy( CurPosition, FMO_STOCK_SOURCE.c_str(), 1 ); //9(1) 0: Normal 1: 2: 8:Day Trade
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_OFFSET_MARK.c_str(), 1 );  //X(1) " "
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_BOOKING_MARK.c_str(), 1 ); //X(1) " " 
    CurPosition += 1;      
    std::memcpy( CurPosition, FMO_FILLER4.c_str(), 19 );      //X(19)       
    CurPosition += 19;
    *CurPosition = 0;          
}        
//------------------------------------------------------------------------------    
};
//------------------------------------------------------------------------------
    
