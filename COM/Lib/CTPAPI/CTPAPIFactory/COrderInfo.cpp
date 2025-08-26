//---------------------------------------------------------------------------
#ifdef _CTP
//---------------------------------------------------------------------------
#include "COrderInfo.h"
#include "TBaseMessage.h"
#include <math.h>
//---------------------------------------------------------------------------
//
// Class CTPOrderInfo
// Object to keep CTP Order status, UserDefine data...
//
//---------------------------------------------------------------------------
UInt32 SEQ_MAX = 16777216; ///< 2^24 (24 bites)
UInt32 FID_MAX = 256; ///< 2^8 (8 bites)
//---------------------------------------------------------------------------
// CTP implement
// Construct from execution data struct
//---------------------------------------------------------------------------
COrderInfo::COrderInfo( CThostFtdcOrderField *Ord )
:FStatus( cosPending )
,FFrontID( Ord->FrontID )
,FSessionID( Ord->SessionID )
,FOrderRef( atoi( Ord->OrderRef ) )
,FCancelRef( 0 )
,FBrokerID( Ord->BrokerID )
,FAccount( Ord->InvestorID )
,FSymbol( Ord->InstrumentID )
,FPrice( Ord->LimitPrice )
,FData( "" )
,FOrderQty( Ord->VolumeTotalOriginal )
,FCumQty( 0 )
,FLeavesQty( 0 )
{
	if( Ord->OrderPriceType == THOST_FTDC_OPT_AnyPrice )
		FOrderType = nsOrderMessageDefine::otMarket;
	else
		FOrderType = nsOrderMessageDefine::otLimit;
	if( Ord->Direction == THOST_FTDC_D_Buy )
		FSide = nsOrderMessageDefine::sBuy;
	else
		FSide = nsOrderMessageDefine::sSell;
	if(	Ord->CombOffsetFlag[0] == THOST_FTDC_OF_Open ) // Open
		FPositionEffect = nsOrderMessageDefine::peOpen;
	else if( Ord->CombOffsetFlag[0] == THOST_FTDC_OF_Close )//Close
		FPositionEffect = nsOrderMessageDefine::peClose;
	else if( Ord->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday )//Close Today
		FPositionEffect = nsOrderMessageDefine::peDayTrade;
	if( Ord->TimeCondition == THOST_FTDC_TC_IOC )
		FTimeInForce = nsOrderMessageDefine::tifIOC;
	else
		FTimeInForce = nsOrderMessageDefine::tifROD;
}
//---------------------------------------------------------------------------
// KSft / KStrd implement
// Construct from execution data struct
//---------------------------------------------------------------------------
COrderInfo::COrderInfo( KS_::CThostFtdcOrderField *pOrder )
: FStatus( cosPending )
, FFrontID( pOrder->FrontID )
, FSessionID( pOrder->SessionID )
, FOrderRef( atoi( pOrder->OrderRef ) )
, FCancelRef( 0 )
, FBrokerID( pOrder->BrokerID )
, FAccount( pOrder->InvestorID )
, FSymbol( pOrder->InstrumentID )
, FPrice( pOrder->LimitPrice )
, FData( "" )
, FOrderQty( pOrder->VolumeTotalOriginal )
, FCumQty( 0 )
, FLeavesQty( 0 )
{
	// OrderType
	if ( pOrder->OrderPriceType == THOST_FTDC_OPT_AnyPrice )
		FOrderType = nsOrderMessageDefine::otMarket;
	else
		FOrderType = nsOrderMessageDefine::otLimit;
	// Side
	if ( pOrder->Direction == THOST_FTDC_D_Buy )
		FSide = nsOrderMessageDefine::sBuy;
	else
		FSide = nsOrderMessageDefine::sSell;
	// PositionEffect
	if ( pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Open ) ///< Open
		FPositionEffect = nsOrderMessageDefine::peOpen;
	else if ( pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_Close ) ///< Close
		FPositionEffect = nsOrderMessageDefine::peClose;
	else if ( pOrder->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday ) ///< Close Today
		FPositionEffect = nsOrderMessageDefine::peDayTrade;
	// TimeInForce
	if ( pOrder->TimeCondition == THOST_FTDC_TC_IOC )
		FTimeInForce = nsOrderMessageDefine::tifIOC;
	else
		FTimeInForce = nsOrderMessageDefine::tifROD;
}
//---------------------------------------------------------------------------
// HSufx implement
// Construct from execution data struct
//---------------------------------------------------------------------------
COrderInfo::COrderInfo( int BranchNo, int SID, const UFC::AnsiString& ExchangeID, HSufxFutOpt::dataReportConfirm& Data )
: FStatus( cosPending )
, FFrontID( 0 )
, FSessionID( SID )
, FOrderRef( 0 )
, FCancelRef( 0 )
, FBrokerID( BranchNo )
, FAccount( Data.fund_account )
, FSymbol( Data.futu_code )
, FPrice( Data.futu_entrust_price )
, FData( "" )
, FExchangeID( ExchangeID )
, FOrderQty( Data.entrust_amount )
, FCumQty( 0 )
, FLeavesQty( 0 )
{
	// entrust_prop ( OrderType & TimeInForce )
	if ( Data.entrust_prop == "F1" )
	{
		FOrderType   = nsOrderMessageDefine::otMarket;
		FTimeInForce = nsOrderMessageDefine::tifROD;
	}
	else ///< "F0"
	{
		FOrderType   = nsOrderMessageDefine::otLimit;
		FTimeInForce = nsOrderMessageDefine::tifROD;
	}
	// Side
	if ( Data.entrust_bs == '1' )
		FSide = nsOrderMessageDefine::sBuy;
	else ///< '2'
		FSide = nsOrderMessageDefine::sSell;
	// PositionEffect
	if ( Data.futures_direction == '2' )
		FPositionEffect = nsOrderMessageDefine::peClose;
	else ///< '1'
		FPositionEffect = nsOrderMessageDefine::peOpen;
}
//---------------------------------------------------------------------------
// HSufx implement
// Construct from query order result
//---------------------------------------------------------------------------
COrderInfo::COrderInfo( int BranchNo, int SID, const UFC::AnsiString& ExchangeID, HSufxFutOpt::dataQryOrderResult *pOrder )
: FStatus( cosPending )
, FFrontID( 0 )
, FSessionID( SID )
, FOrderRef( 0 )
, FCancelRef( 0 )
, FBrokerID( BranchNo )
, FAccount( pOrder->fund_account )
, FSymbol( pOrder->futu_code )
, FPrice( pOrder->futu_entrust_price )
, FData( "" )
, FExchangeID( ExchangeID )
, FOrderQty( (int)pOrder->entrust_amount )
, FCumQty( 0 )
, FLeavesQty( 0 )
{
	// entrust_prop ( OrderType & TimeInForce )
	if ( memcmp( pOrder->entrust_prop, "F1", 3 ) == 0 )
	{
		FOrderType   = nsOrderMessageDefine::otMarket;
		FTimeInForce = nsOrderMessageDefine::tifROD;
	}
	else ///< "F0"
	{
		FOrderType   = nsOrderMessageDefine::otLimit;
		FTimeInForce = nsOrderMessageDefine::tifROD;
	}
	// Side
	if ( pOrder->entrust_bs == '1' )
		FSide = nsOrderMessageDefine::sBuy;
	else ///< '2'
		FSide = nsOrderMessageDefine::sSell;
	// PositionEffect
	if ( pOrder->futures_direction == '2' )
		FPositionEffect = nsOrderMessageDefine::peClose;
	else ///< '1'
		FPositionEffect = nsOrderMessageDefine::peOpen;
}
//---------------------------------------------------------------------------
COrderInfo::COrderInfo( int BranchNo, int SID, HSufxStockOpt::dataQryOrderResult *pOrder )
: FStatus( cosPending )
, FFrontID( 0 )
, FSessionID( SID )
, FOrderRef( 0 )
, FCancelRef( 0 )
, FBrokerID( BranchNo )
, FAccount( pOrder->fund_account )
, FSymbol( pOrder->option_code )
, FPrice( pOrder->opt_entrust_price )
, FData( "" )
, FOrderQty( (int)pOrder->entrust_amount )
, FCumQty( 0 )
, FLeavesQty( 0 )
{
	// entrust_prop ( OrderType & TimeInForce )
	if ( pOrder->entrust_prop[0] == '0' )
	{
		FOrderType   = nsOrderMessageDefine::otLimit;
		FTimeInForce = nsOrderMessageDefine::tifROD;
	}
	else if ( memcmp( pOrder->entrust_prop, "OPA", 3 ) == 0 )
	{
		FOrderType   = nsOrderMessageDefine::otLimit;
		FTimeInForce = nsOrderMessageDefine::tifFOK;
	}
	else if ( memcmp( pOrder->entrust_prop, "OPB", 3 ) == 0 )
	{
		FOrderType   = nsOrderMessageDefine::otMarket;
		FTimeInForce = nsOrderMessageDefine::tifIOC;
	}
	else if ( memcmp( pOrder->entrust_prop, "OPC", 3 ) == 0 )
	{
		FOrderType   = nsOrderMessageDefine::otMarket;
		FTimeInForce = nsOrderMessageDefine::tifFOK;
	}
	else if ( memcmp( pOrder->entrust_prop, "OPD", 3 ) == 0 )
	{
		FOrderType   = nsOrderMessageDefine::otMLtoLimit;
		FTimeInForce = nsOrderMessageDefine::tifROD;
	}
	else
	{
		FOrderType   = nsOrderMessageDefine::otLimit;
		FTimeInForce = nsOrderMessageDefine::tifROD;
	}
	// Side
	if ( pOrder->entrust_bs == '1' )
		FSide = nsOrderMessageDefine::sBuy;
	else ///< '2'
		FSide = nsOrderMessageDefine::sSell;
	// PositionEffect
	if ( pOrder->entrust_oc == 'C' )
		FPositionEffect = nsOrderMessageDefine::peClose;
	else
		FPositionEffect = nsOrderMessageDefine::peOpen;
}
//---------------------------------------------------------------------------
// CTP implement
// Construct from User place new order data struct
//---------------------------------------------------------------------------
COrderInfo::COrderInfo( int FID, int SID, CThostFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine )
:FStatus( cosPending )
,FFrontID( FID )
,FSessionID( SID )
,FOrderRef( atoi( Ord.OrderRef ) )
,FCancelRef( 0 )
,FBrokerID( Ord.BrokerID )
,FAccount( Ord.InvestorID )
,FSymbol( Ord.InstrumentID )
,FPrice( Ord.LimitPrice )
,FData( UserDefine )
,FOrderQty( Ord.VolumeTotalOriginal )
,FCumQty( 0 )
,FLeavesQty( 0 )
{
	if( Ord.OrderPriceType == THOST_FTDC_OPT_AnyPrice )
		FOrderType = nsOrderMessageDefine::otMarket;
	else
		FOrderType = nsOrderMessageDefine::otLimit;
	if( Ord.Direction == THOST_FTDC_D_Buy )
		FSide = nsOrderMessageDefine::sBuy;
	else
		FSide = nsOrderMessageDefine::sSell;
	if(	Ord.CombOffsetFlag[0] == THOST_FTDC_OF_Open ) // Open
		FPositionEffect = nsOrderMessageDefine::peOpen;
	else if( Ord.CombOffsetFlag[0] == THOST_FTDC_OF_Close )//Close
		FPositionEffect = nsOrderMessageDefine::peClose;
	else if( Ord.CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday )//Close Today
		FPositionEffect = nsOrderMessageDefine::peDayTrade;
	if( Ord.TimeCondition == THOST_FTDC_TC_IOC )
		FTimeInForce = nsOrderMessageDefine::tifIOC;
	else
		FTimeInForce = nsOrderMessageDefine::tifROD;
}
//---------------------------------------------------------------------------
// Femats implement
// Construct from User place new order data struct
//---------------------------------------------------------------------------
COrderInfo::COrderInfo( int FID, int SID, CUstpFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine )
:FStatus( cosPending )
,FFrontID( FID )
,FSessionID( SID )
,FOrderRef( atoi( Ord.UserOrderLocalID ) )
,FCancelRef( 0 )
,FBrokerID( Ord.BrokerID )
,FAccount( Ord.InvestorID )
,FSymbol( Ord.InstrumentID )
,FPrice( Ord.LimitPrice )
,FData( UserDefine )
,FOrderQty( Ord.Volume )
,FCumQty( 0 )
,FLeavesQty( 0 )
{
	if( Ord.OrderPriceType == USTP_FTDC_OPT_AnyPrice )
		FOrderType = nsOrderMessageDefine::otMarket;
	else
		FOrderType = nsOrderMessageDefine::otLimit;
	if( Ord.Direction == USTP_FTDC_D_Buy )
		FSide = nsOrderMessageDefine::sBuy;
	else
		FSide = nsOrderMessageDefine::sSell;
	if(	Ord.OffsetFlag == USTP_FTDC_OF_Open ) // Open
		FPositionEffect = nsOrderMessageDefine::peOpen;
	else if( Ord.OffsetFlag == USTP_FTDC_OF_Close )//Close
		FPositionEffect = nsOrderMessageDefine::peClose;
	else if( Ord.OffsetFlag == USTP_FTDC_OF_CloseToday )//Close Today
		FPositionEffect = nsOrderMessageDefine::peDayTrade;
	if( Ord.TimeCondition == USTP_FTDC_TC_IOC )
		FTimeInForce = nsOrderMessageDefine::tifIOC;
	else
		FTimeInForce = nsOrderMessageDefine::tifROD;
}
//---------------------------------------------------------------------------
COrderInfo::COrderInfo( int FID, int SID, CUstpFtdcOrderField *Ord )
:FStatus( cosPending )
,FFrontID( FID )
,FSessionID( SID )
,FOrderRef( atoi( Ord->UserOrderLocalID ) )
,FCancelRef( 0 )
,FBrokerID( Ord->BrokerID )
,FAccount( Ord->InvestorID )
,FSymbol( Ord->InstrumentID )
,FPrice( Ord->LimitPrice )
,FData( Ord->UserCustom )
,FOrderQty( Ord->Volume )
,FCumQty( 0 )
,FLeavesQty( 0 )
{
	if( Ord->OrderPriceType == USTP_FTDC_OPT_AnyPrice )
		FOrderType = nsOrderMessageDefine::otMarket;
	else
		FOrderType = nsOrderMessageDefine::otLimit;
	if( Ord->Direction == USTP_FTDC_D_Buy )
		FSide = nsOrderMessageDefine::sBuy;
	else
		FSide = nsOrderMessageDefine::sSell;
	if(	Ord->OffsetFlag == USTP_FTDC_OF_Open ) // Open
		FPositionEffect = nsOrderMessageDefine::peOpen;
	else if( Ord->OffsetFlag == USTP_FTDC_OF_Close )//Close
		FPositionEffect = nsOrderMessageDefine::peClose;
	else if( Ord->OffsetFlag == USTP_FTDC_OF_CloseToday )//Close Today
		FPositionEffect = nsOrderMessageDefine::peDayTrade;
	if( Ord->TimeCondition == USTP_FTDC_TC_IOC )
		FTimeInForce = nsOrderMessageDefine::tifIOC;
	else
		FTimeInForce = nsOrderMessageDefine::tifROD;
}
//---------------------------------------------------------------------------
// KSft / KStrd implement
// Construct from User place new order data struct
//---------------------------------------------------------------------------
COrderInfo::COrderInfo( int FID, int SID, KS_::CThostFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine )
: FStatus( cosPending )
, FFrontID( FID )
, FSessionID( SID )
, FOrderRef( atoi( Ord.OrderRef ) )
, FCancelRef( 0 )
, FBrokerID( Ord.BrokerID )
, FAccount( Ord.InvestorID )
, FSymbol( Ord.InstrumentID )
, FPrice( Ord.LimitPrice )
, FData( UserDefine )
, FOrderQty( Ord.VolumeTotalOriginal )
, FCumQty( 0 )
, FLeavesQty( 0 )
{
	// OrderType
	if ( Ord.OrderPriceType == THOST_FTDC_OPT_AnyPrice )
		FOrderType = nsOrderMessageDefine::otMarket;
	else
		FOrderType = nsOrderMessageDefine::otLimit;
	// Side
	if ( Ord.Direction == THOST_FTDC_D_Buy )
		FSide = nsOrderMessageDefine::sBuy;
	else
		FSide = nsOrderMessageDefine::sSell;
	// PositionEffect
	if ( Ord.CombOffsetFlag[0] == THOST_FTDC_OF_Open ) ///< Open
		FPositionEffect = nsOrderMessageDefine::peOpen;
	else if ( Ord.CombOffsetFlag[0] == THOST_FTDC_OF_Close ) ///< Close
		FPositionEffect = nsOrderMessageDefine::peClose;
	else if ( Ord.CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday ) ///< Close Today
		FPositionEffect = nsOrderMessageDefine::peDayTrade;
	// TimeInForce
	if ( Ord.TimeCondition == THOST_FTDC_TC_IOC )
		FTimeInForce = nsOrderMessageDefine::tifIOC;
	else
		FTimeInForce = nsOrderMessageDefine::tifROD;
}
//---------------------------------------------------------------------------
// HSufx implement
// Construct from User place new order data struct
//---------------------------------------------------------------------------
COrderInfo::COrderInfo( int FID, int SID, TNewOrderMessage* Ord, const UFC::AnsiString& UserDefine )
: FStatus( cosPending )
, FFrontID( FID )
, FSessionID( SID )
, FOrderRef( Ord->GetNID() )
, FCancelRef( 0 )
, FBrokerID( Ord->GetBrokerID() )
, FAccount( Ord->GetAccount() )
, FSymbol( Ord->GetSymbol() )
, FPrice( Ord->GetPrice() )
, FData( UserDefine )
, FOrderQty( Ord->GetOrderQty() )
, FCumQty( 0 )
, FLeavesQty( 0 )
{
	FOrderType      = Ord->GetOrderType();		///< OrderType
	FSide           = Ord->GetSide();			///< Side
	FPositionEffect = Ord->GetPositionEffect();	///< PositionEffect
	FTimeInForce    = Ord->GetTimeInForce();	///< TimeInForce
	FExchangeID     = Ord->GetExchangeCode();	///< ExchangeCode
}
//---------------------------------------------------------------------------
// Construct from Trading log
//---------------------------------------------------------------------------
COrderInfo::COrderInfo( UFC::NameValueMessage& TradeLog )
:FStatus( cosPending )
,FCancelRef( 0 )
,FCumQty( 0 )
,FLeavesQty( 0 )
,FTimeInForce( nsOrderMessageDefine::tifROD )
,FPositionEffect( nsOrderMessageDefine::peOpen )
{
	UFC::AnsiString Value;

	TradeLog.Get("FID", Value, true );
	FFrontID = Value.ToInt();
	TradeLog.Get("SID", Value, true );
	FSessionID = Value.ToInt();
	TradeLog.Get("OREF", Value, true );
	FOrderRef = Value.ToInt();
	TradeLog.Get("PX", Value, true );
	FPrice = Value.ToDouble();
	TradeLog.Get("QTY", Value, true );
	FOrderQty = Value.ToInt();
	TradeLog.Get("BRK", FBrokerID, true );
	TradeLog.Get("ACC", FAccount, true );
	TradeLog.Get("SYM", FSymbol, true );
	TradeLog.Get("UDD", FData, true );
	if( TradeLog.Get("OT", Value ) == true && Value[0] == 'M' )
		FOrderType = nsOrderMessageDefine::otMarket;
	else
		FOrderType = nsOrderMessageDefine::otLimit;
	if( TradeLog.Get("BS", Value ) == true && Value[0] == 'B')
		FSide = nsOrderMessageDefine::sBuy;
	else
		FSide = nsOrderMessageDefine::sSell;
	if( TradeLog.Get("TIF", Value ) == true && Value[0] == 'I')
		FTimeInForce = nsOrderMessageDefine::tifIOC;
	else
		FTimeInForce = nsOrderMessageDefine::tifROD;
	if( TradeLog.Get("PE", Value ) == true )
	{
		if( Value[0] == 'C' )
			FPositionEffect = nsOrderMessageDefine::peClose;
		else if( Value[0] == 'c' )
			FPositionEffect = nsOrderMessageDefine::peDayTrade;
		else
			FPositionEffect = nsOrderMessageDefine::peOpen;
	}
}
//---------------------------------------------------------------------------
// CTP Accept this order.
//
// Update ExchangeID and OrderLocalID
//---------------------------------------------------------------------------
void COrderInfo::CTPAccepted( const UFC::AnsiString& ExchangeID, const UFC::AnsiString& OrderLocalID )
{
	FExchangeID   = ExchangeID;
	FOrderLocalID = OrderLocalID;
	FStatus       = cosConfirm;
	FLeavesQty    = FOrderQty;
}
//---------------------------------------------------------------------------
void COrderInfo::FemasAccepted( const UFC::AnsiString& ExchangeID, const UFC::AnsiString& OrderSysID, const UFC::AnsiString& OrderLocalID )
{
	FExchangeID   = ExchangeID;
	FOrderSysID  = OrderSysID;
	if( OrderLocalID.Length() > 0 )
		FOrderLocalID = OrderLocalID;
	FStatus       = cosExchangeConfirm;
	FLeavesQty    = FOrderQty;
}
//---------------------------------------------------------------------------
// HSufx Accept this order.
//
// Update OrderLocalID
//---------------------------------------------------------------------------
void COrderInfo::HSufxAccepted( const UFC::AnsiString& OrderLocalID )
{
	FOrderLocalID = OrderLocalID;
	FStatus       = cosConfirm;
	FLeavesQty    = FOrderQty;
}
//---------------------------------------------------------------------------
// Exchange Accept this order.
//
// Update OrderSysID
//---------------------------------------------------------------------------
void COrderInfo::EachangeAccepted( const UFC::AnsiString& OrderSysID )
{
	FOrderSysID  = OrderSysID;
	FStatus      = cosExchangeConfirm;
}
//---------------------------------------------------------------------------
void COrderInfo::EachangeAccepted( const UFC::AnsiString& OrderSysID, const UFC::AnsiString& OrderLocalID )
{
	FOrderLocalID = OrderLocalID;
	FOrderSysID  = OrderSysID;
	FStatus      = cosExchangeConfirm;
}
//---------------------------------------------------------------------------
void COrderInfo::EachangeAccepted( const UFC::AnsiString& ExchangeID, const UFC::AnsiString& OrderSysID, const UFC::AnsiString& OrderLocalID )
{
	FExchangeID   = ExchangeID;
	FOrderLocalID = OrderLocalID;
	FOrderSysID   = OrderSysID;
	FStatus       = cosExchangeConfirm;
	FLeavesQty    = FOrderQty;
}
//---------------------------------------------------------------------------
// User want to cancel this order.
//
// Update Cancel NID and Cancel UserDefine
//---------------------------------------------------------------------------
CxlStyle COrderInfo::Cancel( const UFC::AnsiString& UserDefine, int COrdRef )
{
	FCxlData    = UserDefine;
	FCancelRef  = COrdRef;
	switch( FStatus )
	{
		case cosPending:     	return csByOrderRef;
		case cosConfirm:    	return csByOrderLocalID;
		case cosExchangeConfirm:
		case cosClosed:         return csByOrderSysID;
		default:			    return csNone;
	}
}
//---------------------------------------------------------------------------
//  Change the order status to cosClosed
//
//  Update the Leaves Qty to Zero
//---------------------------------------------------------------------------
void COrderInfo::CloseOrder( void )
{
	FStatus = cosClosed;
	FLeavesQty = 0;
}
//---------------------------------------------------------------------------
// Order fill.
//
// Increase CumQty and decrease leaves Qty.
//---------------------------------------------------------------------------
void COrderInfo::Fill( int qty )
{
	FLeavesQty -= qty; ///< Decrease leaves Qty.
	if ( FLeavesQty < 0 ) ///< Validate Leaves Qty
		FLeavesQty = 0;
	FCumQty    += qty; ///< Increase Cum Qty
	if ( FCumQty > FOrderQty ) ///< Validate Cum Qty
		FCumQty = FOrderQty;
	if( FLeavesQty <= 0 )
		FStatus = cosClosed;
}
//---------------------------------------------------------------------------
Int64 COrderInfo::GetOrderNID( void )
{
	return COrderInfo::ToNID( FFrontID, FSessionID, FOrderRef );
}
//---------------------------------------------------------------------------
Int64 COrderInfo::GetCancelNID( void )
{
	return COrderInfo::ToNID( FFrontID, FSessionID, FCancelRef );
}
//---------------------------------------------------------------------------
//  CTP NID binary layout 64 bits
//
//  | 32 Bites | 8 Bites | 24 Bites |
//     |           |         |____ Order Reference number: from 0 to 16,777,216
//     |           |
//     |           |____ FrontID.( from CTP logon)
//     |
//     |__________ SessionID.( from CTP logon)
//
//---------------------------------------------------------------------------
Int64 COrderInfo::ToNID( int FrontID, int SessionID, int OrderRef )
{
	Int64 NID64 = (((Int64)SessionID)<<32 |
				   ((Int64)(FrontID%FID_MAX))<<24 |
					(Int64)(OrderRef%SEQ_MAX) );
	return NID64;
}
//---------------------------------------------------------------------------
int COrderInfo::ToOrderRef( Int64 NID )
{
	return (int)((NID << 40)>>40);
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
