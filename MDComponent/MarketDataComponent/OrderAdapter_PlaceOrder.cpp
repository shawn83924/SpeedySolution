//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "OrderAdapter.h"
#include "MessageDataFormat.h"
#include "TNewOrderMessage.h"
#include "TCancelOrderMessage.h"
#include "TReplaceOrderMessage.h"
#include "TExecutionUnit.h"
#include "TradingObjects.h"
#include "TradingCommodities.h"
#include "TradingObjectPool.h"
#include "OrderStore.h"
#include <WideStrUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
UFCType::Int64 TOrderAdapter::GenerateNID( nsOrderMessageDefine::MessageTypeEnum MT )
{
	return FConnection->GenerateNID( MT );
}
//---------------------------------------------------------------------------
/*TExecution *TOrderAdapter::CreateOrderRootForNewOrder( nsOrderMessageDefine::MarketEnum Market,
													   const String& Exchange,
													   const String& Symbol,
													   nsOrderMessageDefine::SideEnum Side,
													   double Px,
													   int Qty,
													   nsOrderMessageDefine::OrderTypeEnum OrderType,
													   nsOrderMessageDefine::TimeInForceEnum TimeInForce,
													   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
													   const String& Account,
													   double StopPx,
													   int TickCount,
													   const UFC::AnsiString& StrategyName,
													   UFC::AnsiString& AnsiTradeSymbol,      ///< Out
													   UFC::AnsiString& AnsiMaturityMonthYear,///< Out
													   char OrdKind )
{
	String tradeSymbol = Symbol;
	String orderSymbol = Symbol;
	AnsiTradeSymbol = s888::ConvertUnicodeToUFCAnsiStr(tradeSymbol);
	String maturityMonthYear = L"";
	AnsiMaturityMonthYear = "";
	int fractionLength = 0;
	nsOrderMessageDefine::MarketEnum contractMarket = Market;
	s888::CTradingContract *contractPtr = s888::gTradingObjsPool.FindContractFromSymbolIndex( Exchange, Symbol, s888::rwLockForRead);

	if( contractPtr == NULL )
		contractPtr = s888::gTradingObjsPool.FindContractFromTradeSymbolIndex( Exchange, Symbol, s888::rwLockForRead);
	if( contractPtr != NULL )
	{
		contractPtr->LockForRead();
		orderSymbol = contractPtr->GetSymbol(s888::rwNotLock);
		tradeSymbol = contractPtr->GetTradeSymbol(s888::rwNotLock);
		AnsiTradeSymbol = s888::ConvertUnicodeToUFCAnsiStr(tradeSymbol);
		maturityMonthYear = contractPtr->GetMaturityYearMonthStr(s888::rwNotLock);
		AnsiMaturityMonthYear = s888::ConvertUnicodeToUFCAnsiStr(maturityMonthYear);
		fractionLength = contractPtr->GetFractionPartLength(s888::rwNotLock);
		contractMarket = contractPtr->GetMarket(s888::rwNotLock);
		contractPtr->UnlockForRead();
	}
	else
	{
		BasicInformation *infoPtr = FOrderStore->GetCommodityBasicInformation(Exchange, Symbol);
		if (infoPtr != 0)
		{
			if ((Market == nsOrderMessageDefine::mForeignFutures) ||
				(Market == nsOrderMessageDefine::mForeignOptions))
			{
				AnsiTradeSymbol = infoPtr->GetTradeSymbol();
				tradeSymbol = UTF8ToUnicodeString(AnsiTradeSymbol.c_str());
				AnsiMaturityMonthYear = infoPtr->GetMaturityDate();
				maturityMonthYear = UTF8ToUnicodeString(AnsiMaturityMonthYear.c_str());
			}
			fractionLength = infoPtr->GetDigit();
			contractMarket = (nsOrderMessageDefine::MarketEnum)infoPtr->GetMarket();
		}
	}  //if (contractPtr != 0)

	if (tradeSymbol.Length() <= 0)
	{
		tradeSymbol = Symbol;
		AnsiTradeSymbol = s888::ConvertUnicodeToUFCAnsiStr(tradeSymbol);
	}

	TExecution *orderRootPtr = new TExecution();
	orderRootPtr->SetPxDigit(fractionLength);
	orderRootPtr->SetMarket(contractMarket);
	orderRootPtr->SetExchangeCode(Exchange);
	orderRootPtr->SetSide(Side);
	orderRootPtr->SetSymbol(orderSymbol);
	orderRootPtr->SetPrice(Px);
	orderRootPtr->SetOrderQty(Qty);
	orderRootPtr->SetLeavesQty(Qty);
	orderRootPtr->SetCumQty(0);
	orderRootPtr->SetOrderType(OrderType);
	orderRootPtr->SetTimeInForce(TimeInForce);
	orderRootPtr->SetPositionEffect(PositionEffect);
	orderRootPtr->SetAccount(Account);
	orderRootPtr->SetNID( GenerateNID(nsOrderMessageDefine::mtNew));
	orderRootPtr->SetTickCount(TickCount);
	orderRootPtr->SetStopPrice(StopPx);
	orderRootPtr->SetRoot(true);
	orderRootPtr->SetTradeSymbol(tradeSymbol);
	orderRootPtr->SetOrderStatus(nsOrderMessageDefine::osNone);
	orderRootPtr->SetOrderKind( ToOrderKind( contractMarket, PositionEffect, OrdKind ) );
	if( StopPx > 0.0)
		orderRootPtr->SetStopSymbol( orderSymbol );
	if( StrategyName.Length() > 0 )
		orderRootPtr->SetStrategyName( UTF8ToUnicodeString(StrategyName) );
	return orderRootPtr;
}*/
//---------------------------------------------------------------------------
TExecution* TOrderAdapter::CreateRootNewOrder( TNewOrderMessage& NewMsg, int TickCount, const UFC::AnsiString& StrategyName )
{
	nsOrderMessageDefine::MarketEnum Market = NewMsg.GetMarket();
	String                  Exchange( NewMsg.GetExchangeCode() );
	String                  Symbol( NewMsg.GetSymbol() );
	BasicInformation*       infoPtr;
	s888::CTradingContract* contractPtr = s888::gTradingObjsPool.FindContractFromSymbolIndex( Exchange, Symbol, s888::rwLockForRead);
	TExecution*             orderRootPtr = new TExecution( NewMsg );

	orderRootPtr->SetRoot(true);
	orderRootPtr->SetNID( (NewMsg.GetNID() == 0)? GenerateNID(nsOrderMessageDefine::mtNew) : NewMsg.GetNID() );
	orderRootPtr->SetTickCount(TickCount);
	orderRootPtr->SetStrategyName( UTF8String( StrategyName.c_str()) );
	orderRootPtr->SetOrderKind( ToOrderKind( Market, NewMsg.GetPositionEffect(), NewMsg.GetTSEOrderType()[0] ) );
	if( contractPtr != NULL )
	{
		contractPtr->LockForRead();
		if ((Market == nsOrderMessageDefine::mForeignFutures) || (Market == nsOrderMessageDefine::mForeignOptions))
		{
			AnsiString maturityMonthYear = contractPtr->GetMaturityYearMonthStr(s888::rwNotLock);
			NewMsg.SetMaturityMonthYear( maturityMonthYear.c_str() );
		}
		orderRootPtr->SetPxDigit( contractPtr->GetFractionPartLength(s888::rwNotLock) );
		contractPtr->UnlockForRead();
	}
	else
	{
		if( (infoPtr = FOrderStore->GetCommodityBasicInformation(Exchange, Symbol)) != 0)
		{
			if ((Market == nsOrderMessageDefine::mForeignFutures) || (Market == nsOrderMessageDefine::mForeignOptions))
				NewMsg.SetMaturityMonthYear( infoPtr->GetMaturityDate() );
			orderRootPtr->SetPxDigit( infoPtr->GetDigit() );
		}
		else
			UFC::BufferedLog::Printf(" [!!!Fatal Error!!] TOrderAdapter::NewSingleOrder( %s,%s ) not found!!", NewMsg.GetExchangeCode(),NewMsg.GetSymbol());
	}
	return orderRootPtr;
}
//---------------------------------------------------------------------------
UFCType::Int64 TOrderAdapter::SendNewOrderToSpeedy( TNewOrderMessage& NewMsg )
{
	if( TOrderStore::IsTestMode() ) ///< Sim Exchange
	{
		if( FOrderStore != NULL )
		{
			TExecutionReportMessage *newRptPtr = new TExecutionReportMessage();
			newRptPtr->SetMarket(NewMsg.GetMarket());
			newRptPtr->SetExchangeCode(NewMsg.GetExchangeCode());
			newRptPtr->SetSymbol(NewMsg.GetSymbol());
			newRptPtr->SetAccount(NewMsg.GetAccount());
			newRptPtr->SetBrokerID(NewMsg.GetBrokerID());
			newRptPtr->SetSide(NewMsg.GetSide());
			newRptPtr->SetPrice(NewMsg.GetPrice());
			newRptPtr->SetOrderQty(NewMsg.GetOrderQty());
			newRptPtr->SetOrderType(NewMsg.GetOrderType());
			newRptPtr->SetTimeInForce(NewMsg.GetTimeInForce());
			newRptPtr->SetPositionEffect(NewMsg.GetPositionEffect());
			newRptPtr->SetNID(NewMsg.GetNID());
			newRptPtr->SetStopPx(NewMsg.GetStopPx());
			newRptPtr->SetAllUserData(NewMsg.GetAllUserData());
			newRptPtr->SetMaturityMonthYear(NewMsg.GetMaturityMonthYear());
			newRptPtr->SetStrikePrice( NewMsg.GetStrikePrice());
			newRptPtr->SetEventType( NewMsg.GetEventType() );
			FOrderStore->AppendTestExecRptToList(newRptPtr);
		}
	}
	else  ////< Send to Speedy
	{
		try
		{
			if( IsConnectionReady() == true )
				FConnection->NewOrder(&NewMsg);
		}
		catch (...)
		{
			throw OSNewOrderException();
		}
	}
	return NewMsg.GetNID();
}
//---------------------------------------------------------------------------
void TOrderAdapter::MarginPositionRequest(
								const UFC::AnsiString& BrokerID,
								const UFC::AnsiString& Account,
								UFC::AnsiString& Result )
{
	FConnection->MarginPositionRequest(BrokerID, Account, Result);
}
//---------------------------------------------------------------------------
UFCType::Int64 TOrderAdapter::UniqueNewNID( void )
{
	UFCType::Int64   NewNID;

	for( register int i = 0; i < 10000; i++ )
	{
		NewNID = GenerateNID( nsOrderMessageDefine::mtNew );
		if( FOrderStore->FindOrderRootByNID( NewNID ) == NULL )
			return NewNID;
	}
	return 0;
}
//---------------------------------------------------------------------------
UFCType::Int64 TOrderAdapter::NewSingleOrder( nsOrderMessageDefine::MarketEnum Market,
											  const String& Exchange,
											  const String& Symbol,
											  nsOrderMessageDefine::SideEnum Side,
											  double Px,
											  int Qty,
											  nsOrderMessageDefine::OrderTypeEnum OrderType,
											  nsOrderMessageDefine::TimeInForceEnum TimeInForce,
											  nsOrderMessageDefine::PositionEffectEnum PositionEffect,
											  nsOrderMessageDefine::EventTypeEnum EventType,
											  double StrikePx,
											  const String& Account,
											  const String& AE,
											  const String& IB,
											  int TickCount,
											  double StopPx,
											  UFC::AnsiString StrategyName,
											  char TWSEOrdType )
{
	AnsiString       ansiExchange( Exchange );
	AnsiString       ansiSymbol( Symbol );
	AnsiString       ansiAccount( Account );
	AnsiString       ansiAE( AE );
	AnsiString       ansiIB( IB );
	UFC::AnsiString  logMessage,userData,ansiTWSEOrdType( TWSEOrdType );
	TNewOrderMessage newMsg;
	UFCType::Int64   NewNID = UniqueNewNID();

	if( NewNID != 0 )
	{
		newMsg.SetNID( NewNID );
		newMsg.SetMarket( Market );
		newMsg.SetExchangeCode( ansiExchange.c_str());
		newMsg.SetSymbol( ansiSymbol.c_str());
		newMsg.SetAccount( ansiAccount.c_str());
		newMsg.SetBrokerID( GetBrokerID( Market ));
		newMsg.SetSide( Side );
		newMsg.SetPrice( Px );
		newMsg.SetOrderQty( Qty );
		newMsg.SetOrderType( OrderType );
		newMsg.SetTimeInForce( TimeInForce );
		newMsg.SetPositionEffect( PositionEffect );
		if( Market == nsOrderMessageDefine::mTWFutures || Market == nsOrderMessageDefine::mTWOptions )
			newMsg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );
		else
			newMsg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
		if( (OrderType == nsOrderMessageDefine::otStopLimit) || (OrderType == nsOrderMessageDefine::otStop))
			newMsg.SetStopPx(StopPx);
		newMsg.SetAENO( ansiAE.c_str());
		newMsg.SetIBNO( ansiIB.c_str());
		newMsg.SetEventType( EventType );
		newMsg.SetStrikePrice( StrikePx );
		newMsg.SetTSEOrderType( ansiTWSEOrdType.c_str() );
		GetUserData( Market, ansiExchange.c_str(), userData, StrategyName, PositionEffect, TWSEOrdType );
		newMsg.SetAllUserData( userData.c_str());
		///< Create root execution.
		TExecution* orderRootPtr = CreateRootNewOrder( newMsg, TickCount, StrategyName );
		FOrderStore->AddOrderRoot( orderRootPtr );
		if( !FOrderStore->InsertOrderRootNIDIndex( orderRootPtr ) )
		{
			logMessage.Printf( "Insert OrderRoot NID[%lld] Index Failed.", orderRootPtr->GetNID());
			UFC::BufferedLog::Printf(" TOrderAdapter::NewSingleOrder() %s", logMessage.c_str());
			return 0;
		}
		else
		{	///< Send New Order to Speedy
			SendNewOrderToSpeedy( newMsg );
			return orderRootPtr->GetNID();
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
UFCType::Int64 TOrderAdapter::CancelOrder( nsOrderMessageDefine::MarketEnum Market,
										   const String& Exchange,
										   const String& OrderID,
										   const String& AE,
										   const String& IB,
										   UFC::AnsiString StrategyName)
{
	if( FConnection == NULL) throw OSConnectionNotExistException();
	if( FOrderStore == NULL) throw OSConnectionNotExistException();
	if( !FConnection->IsConnected()) throw OSConnectionNotConnectedException();
	if( !FConnection->IsLogon()) throw OSConnectionNotLogonException();

	AnsiString     ansiOrderID( OrderID );
	UFCType::Int64 CancelNID;
	TExecution*    OrderRootPtr = FOrderStore->FindOrderRootByOrderID( OrderID );

	if ( OrderRootPtr != 0 )
	{
		nsOrderMessageDefine::MarketEnum tempMarket = Market;
		String symbol = OrderRootPtr->GetSymbol()->GetSymbol(0);
		s888::CTradingContract *contractPtr = s888::gTradingObjsPool.FindContractBySymbol(Exchange, symbol, true, s888::rwLockForRead);  //Check Symbol Index First
		if (contractPtr != 0) tempMarket = contractPtr->GetMarket(s888::rwLockForRead);

		UFC::AnsiString ansiUserData;
		UFC::AnsiString ansTWSEOrdType(ToTWSEOrderType( Market, OrderRootPtr->GetSide(), OrderRootPtr->GetOrderKind()));
		AnsiString ansiSymbol(symbol);
		AnsiString ansiAEID(AE);
		AnsiString ansiIBID(IB);
		AnsiString ansiExchange(Exchange);
		AnsiString tempAccount( FOrderStore->GetMarketAccountForOrder(tempMarket) );
		TCancelOrderMessage CancelMsg;

		if( ansiExchange.Length() <= 0 )
			ansiExchange = AnsiString( OrderRootPtr->GetExchangeCode());
		GetUserData( tempMarket, ansiExchange.c_str(), ansiUserData, StrategyName, nsOrderMessageDefine::peAuto, OrderRootPtr->GetOrderKind() );
		CancelMsg.SetMarket( tempMarket );
		CancelMsg.SetExchangeCode( ansiExchange.c_str() );
		CancelMsg.SetAccount( tempAccount.c_str());
		if( Market == nsOrderMessageDefine::mTWFutures || Market == nsOrderMessageDefine::mTWOptions )
			CancelMsg.SetTradingSessionID( nsOrderMessageDefine::tsAuto );
		else
			CancelMsg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
		CancelMsg.SetSymbol( ansiSymbol.c_str());
		CancelMsg.SetOrderID( ansiOrderID.c_str());
		CancelMsg.SetBrokerID( GetBrokerID( Market ) );
		CancelMsg.SetSide( OrderRootPtr->GetSide());
		CancelMsg.SetAllUserData( ansiUserData.c_str() );
		CancelMsg.SetAENO( ansiAEID.c_str( ) );
		CancelMsg.SetIBNO( ansiIBID.c_str( ) );
		CancelMsg.SetPrice( OrderRootPtr->GetPrice() );
		CancelMsg.SetOrderQty( OrderRootPtr->GetOrderQty() );
		CancelMsg.SetTSEOrderType( ansTWSEOrdType.c_str() );

		if ( ( Market == nsOrderMessageDefine::mForeignFutures ) ||	 ( Market == nsOrderMessageDefine::mForeignOptions ) )
		{
			BasicInformation *infoPtr = FOrderStore->GetCommodityBasicInformation( Exchange, OrderRootPtr->GetSymbol()->GetSymbol( 0 ) );
			if (infoPtr != 0)
			{
				AnsiString ansiMaturityMonthYear = infoPtr->GetMaturityDate().c_str();
				CancelMsg.SetMaturityMonthYear(ansiMaturityMonthYear.c_str());
			}
		}
		///< Place cancel order
		if( TOrderStore::IsTestMode() ) ///< Sim match
		{
			FOrderStore->AppendTestExecRptToList(CancelMsg);
			FOrderStore->increaseCancelCommandCount();
		}
		else ///< Send to exchange
		{
			FConnection->CancelOrder(&CancelMsg);
			CancelNID = CancelMsg.GetNID();
			FOrderStore->increaseCancelCommandCount();
		}
	}
	else
	{
		if (StrategyName.Length() > 0)
		{
			UFC::AnsiString RejectMsg;

			CancelNID = GenerateNID(nsOrderMessageDefine::mtCancel);
			RejectMsg.Printf("Order[%s] Not Exist.", ansiOrderID.c_str());
			RejectStrategyRequestDirectly(1, 98, RejectMsg, CancelNID, StrategyName);
		}
		else
			throw OSCancelOrderException();
	}
	return CancelNID;
}
//---------------------------------------------------------------------------
void TOrderAdapter::AmendOrder(nsOrderMessageDefine::MarketEnum Market,
							   const String& Exchange,
							   const String& OrderID,
							   int Qty)
{
	if( IsConnectionReady() == true )
	{
		AnsiString ansiOrderID( OrderID );
		TExecution *OrderRootPtr = FOrderStore->FindOrderRootByOrderID(OrderID);
		if (OrderRootPtr != NULL )
		{
			try
			{
				AnsiString ansiExchange(Exchange);
				AnsiString ansiSymbol = OrderRootPtr->GetSymbol()->GetSymbol(0);
				AnsiString ansiStrategyName( OrderRootPtr->GetStrategyName() );
				UFC::AnsiString ansiUserData;
				UFC::AnsiString ansTWSEOrdType(ToTWSEOrderType( Market, OrderRootPtr->GetSide(), OrderRootPtr->GetOrderKind()));
				TReplaceOrderMessage ReplaceMsg;

				GetUserData( Market, ansiExchange.c_str(), ansiUserData, ansiStrategyName.c_str() , nsOrderMessageDefine::peAuto, OrderRootPtr->GetOrderKind() );
				ReplaceMsg.SetMarket(Market);
				ReplaceMsg.SetExchangeCode(ansiExchange.c_str());
				ReplaceMsg.SetTradingSessionID(nsOrderMessageDefine::tsNormal);
				ReplaceMsg.SetSymbol(ansiSymbol.c_str());
				ReplaceMsg.SetOrderID(ansiOrderID.c_str());
				ReplaceMsg.SetBrokerID( GetBrokerID( Market ));
				ReplaceMsg.SetSide(OrderRootPtr->GetSide());
				ReplaceMsg.SetOrderQty(Qty);
				ReplaceMsg.SetAllUserData( ansiUserData.c_str() );
				ReplaceMsg.SetTSEOrderType( ansTWSEOrdType.c_str() );
				if( TOrderStore::IsTestMode() ) ///< Sim match
					FOrderStore->AppendTestExecRptToList( ReplaceMsg );
				else ///< Send to exchange
					FConnection->ReplaceOrder(&ReplaceMsg);
			}
			catch (...)
			{
				throw OSAmendOrderException();
			}
		}
		else
		{
			throw OSAmendOrderException();
		}
	}
}
//---------------------------------------------------------------------------
void TOrderAdapter::ReplaceOrderPrice(nsOrderMessageDefine::MarketEnum Market,
									  const String& Exchange,
									  const String& OrderID,
									  nsOrderMessageDefine::OrderTypeEnum OrderType,
									  nsOrderMessageDefine::TimeInForceEnum TimeInForce,
									  double NewPrice )
{
	if( IsConnectionReady() == true )
	{
		AnsiString  ansiOrderID( OrderID );
		TExecution* OrderRootPtr = FOrderStore->FindOrderRootByOrderID( OrderID );
		if( OrderRootPtr != NULL )
		{
			try
			{
				AnsiString ansiAccount =FOrderStore->GetMarketAccountForOrder(Market);
				AnsiString ansiExchange(Exchange);
				AnsiString ansiSymbol(OrderRootPtr->GetSymbol()->GetSymbol(0));
				AnsiString ansiStrategyName( OrderRootPtr->GetStrategyName() );
				UFC::AnsiString ansiUserData;
				UFC::AnsiString ansTWSEOrdType(ToTWSEOrderType( Market, OrderRootPtr->GetSide(), OrderRootPtr->GetOrderKind()));
				TReplaceOrderMessage ReplaceMsg;

				GetUserData( Market, ansiExchange.c_str(), ansiUserData, ansiStrategyName.c_str(), nsOrderMessageDefine::peAuto, OrderRootPtr->GetOrderKind() );
				ReplaceMsg.SetMarket(Market);
				ReplaceMsg.SetExchangeCode(ansiExchange.c_str());
				if( Market == nsOrderMessageDefine::mTWFutures || Market == nsOrderMessageDefine::mTWOptions )
					ReplaceMsg.SetTradingSessionID(nsOrderMessageDefine::tsAuto);
				else
					ReplaceMsg.SetTradingSessionID(nsOrderMessageDefine::tsNormal);
				ReplaceMsg.SetSymbol(ansiSymbol.c_str());
				ReplaceMsg.SetOrderID(ansiOrderID.c_str());
				ReplaceMsg.SetAccount(ansiAccount.c_str());
				ReplaceMsg.SetBrokerID( GetBrokerID( Market ) );
				ReplaceMsg.SetSide(OrderRootPtr->GetSide());
				ReplaceMsg.SetOrderType(OrderType);
				ReplaceMsg.SetTimeInForce(TimeInForce);
				ReplaceMsg.SetPrice(NewPrice);
				ReplaceMsg.SetOrderQty( 0 );
				ReplaceMsg.SetAllUserData(ansiUserData.c_str() );
				ReplaceMsg.SetTSEOrderType( ansTWSEOrdType.c_str() );
				if( TOrderStore::IsTestMode() ) ///< Sim match
					FOrderStore->AppendTestExecRptToList( ReplaceMsg );
				else ///< Send to exchange
					FConnection->ReplaceOrder(&ReplaceMsg);
			}
			catch (...)
			{
				throw OSAmendOrderException();
			}
		}
		else
		{
			throw OSReplaceOrderPriceException();
		}
	}
}
//---------------------------------------------------------------------------
void TOrderAdapter::RequestOrderStatus(UFCType::Int64 OrderNID,
									   nsOrderMessageDefine::MarketEnum Market,
									   const String& Exchange,
									   const String& OrderID,
									   const String& Symbol,
									   nsOrderMessageDefine::SideEnum Side,
									   double Px)
{
	if( IsConnectionReady() == true )
	{
		try
		{
			UFCType::Int64      newNID = GenerateNID( nsOrderMessageDefine::mtOrderStatusRequest );
			AnsiString ansiTradeSymbol( Symbol );
			AnsiString ansiOrderID( OrderID );
			AnsiString ansiAccount( FOrderStore->GetMarketAccountForOrder(Market));
			AnsiString ansiExchange( Exchange );
			UFC::AnsiString     ansiUserData;
			TOrderStatusRequest ReqMsg;

			GetUserData( Market, ansiExchange.c_str(), ansiUserData, "       ", nsOrderMessageDefine::peAuto );
			ReqMsg.SetMarket(Market);
			ReqMsg.SetExchangeCode( ansiExchange.c_str());
			ReqMsg.SetAccount( ansiAccount.c_str());
			ReqMsg.SetSymbol( ansiTradeSymbol.c_str());
			ReqMsg.SetSide( Side );
			ReqMsg.SetPrice( Px );
			ReqMsg.SetBrokerID( GetBrokerID( Market ) );
			ReqMsg.SetNID( newNID );
			ReqMsg.SetAllUserData( ansiUserData.c_str());
			if ((ansiOrderID.Length() <= 0) || (OrderID == L"00000"))///< Request by NID
			{
				ReqMsg.SetOrigNID(OrderNID);
				InsertQueryNIDIndex(newNID, OrderNID);
			}
			else
				ReqMsg.SetOrderID(ansiOrderID.c_str());
			FConnection->OrderStatusRequest(&ReqMsg);
		}
		catch (...)
		{
			throw OSRequestOrderStatusException();
		}
	}
}
//---------------------------------------------------------------------------
//
//  Functions for Plug-in API
//  Order placement for Name=Value format order string
//
//---------------------------------------------------------------------------
UFCType::Int64 TOrderAdapter::NewOrder( nsOrderMessageDefine::MarketEnum Market,
										UFCType::Int64 NewNID,
										UFC::NameValueMessage& FieldData,
										const UFC::AnsiString& ansiStrategyName,
										const UFC::AnsiString& ansiAccount,
										const String& AE,
										const String& IB,
										int TickCount )
{
	UFC::AnsiString ansiFieldValue = "";
	String exchangeCode = L"";
	if (FieldData.Get("exh", ansiFieldValue) == TRUE) exchangeCode = UTF8ToUnicodeString(ansiFieldValue);

	String symbolCode = L"";
	if (FieldData.Get("sym", ansiFieldValue) == TRUE) symbolCode = UTF8ToUnicodeString(ansiFieldValue);

	nsOrderMessageDefine::SideEnum side = nsOrderMessageDefine::sNone;
	if (FieldData.Get("bs", ansiFieldValue) == TRUE) side = TranslateSide(ansiFieldValue);

	double orderPrice = 0.0;
	if (FieldData.Get("px", ansiFieldValue) == TRUE) orderPrice = ansiFieldValue.ToDouble();

	int orderQty = 0;
	if (FieldData.Get("qty", ansiFieldValue) == TRUE) orderQty = ansiFieldValue.ToInt();

	nsOrderMessageDefine::OrderTypeEnum orderType = nsOrderMessageDefine::otNone;
	if (FieldData.Get("oty", ansiFieldValue) == TRUE) orderType = TranslateOrderType(ansiFieldValue);

	nsOrderMessageDefine::TimeInForceEnum timeInForce = nsOrderMessageDefine::tifNone;
	if (FieldData.Get("tif", ansiFieldValue) == TRUE) timeInForce = TranslateTimeInForce(ansiFieldValue);

	nsOrderMessageDefine::PositionEffectEnum positionEffect = nsOrderMessageDefine::peOpen;
	if (FieldData.Get("pe", ansiFieldValue) == TRUE) positionEffect = TranslatePositionEffect(ansiFieldValue);

	nsOrderMessageDefine::EventTypeEnum eventType = nsOrderMessageDefine::evtNone;
	if (FieldData.Get("cp", ansiFieldValue) == TRUE)
	{
		if (ansiFieldValue[0] == 'C')
			eventType = nsOrderMessageDefine::evtCall;
		else if (ansiFieldValue[0] == 'P')
			eventType = nsOrderMessageDefine::evtPut;
		else
			eventType = nsOrderMessageDefine::evtNone;
	}

	double strikePrice = 0.0;
	if ((FieldData.Get("stkpx", ansiFieldValue) == TRUE) && (ansiFieldValue.Length() > 0))
		strikePrice = ansiFieldValue.ToDouble();

	double stopPrice = 0.0;
	if ((FieldData.Get("spx", ansiFieldValue) == TRUE) && (ansiFieldValue.Length() > 0))
		stopPrice = ansiFieldValue.ToDouble();

	String Account = L"";
	if (ansiAccount.Length() > 0) Account = UTF8ToUnicodeString(ansiAccount);

	return NewSingleOrder( Market, exchangeCode, symbolCode, side, orderPrice, orderQty,
						   orderType, timeInForce, positionEffect, eventType,
						   strikePrice, Account, AE, IB, TickCount, stopPrice, ansiStrategyName, '0' );
}
//---------------------------------------------------------------------------
UFCType::Int64 TOrderAdapter::CancelOrder( const UFC::AnsiString& FieldValueStr,
										   const UFC::AnsiString& StrategyName,
										   const String& AE,
										   const String& IB)
{
	UFC::NameValueMessage FieldData("^\n");
	UFC::AnsiString AnsiExchangeCode( "" );
	UFC::AnsiString AnsiSymbolCode( "" );
	UFC::AnsiString AnsiOrderId( "" );
	UFCType::Int64  CancelNID = 0;

	if( IsConnectionReady() == true )
	{
		FieldData.FromString( FieldValueStr );
		FieldData.Get("exh", AnsiExchangeCode);
		FieldData.Get("sym", AnsiSymbolCode);
		if( FieldData.Get("oid", AnsiOrderId ) == TRUE)
		{
			nsOrderMessageDefine::MarketEnum MarketId;
			String ExchangeCode = L"";
			String OrderId( AnsiOrderId.c_str() );

			if ((AnsiExchangeCode.Length() > 0) && (AnsiSymbolCode.Length() > 0) &&
				GetMarketId( AnsiExchangeCode, AnsiSymbolCode, MarketId))
				ExchangeCode = AnsiExchangeCode.c_str();
			CancelNID = CancelOrder( MarketId, ExchangeCode, OrderId, AE, IB, StrategyName);
		}
		else
		{
			CancelNID = GenerateNID(nsOrderMessageDefine::mtCancel );
			UFC::AnsiString RejectMsg;
			RejectMsg.Printf(" node \"oid\"(Order ID) not Found.");
			RejectStrategyRequestDirectly( 1, 97, RejectMsg, CancelNID, StrategyName);
		}
	}
	return CancelNID;
}
//---------------------------------------------------------------------------
void TOrderAdapter::RejectStrategyRequestDirectly(int RejectResponse,
												  int StatusCode,
												  const UFC::AnsiString& RejectMsg,
												  UFCType::Int64 NID,
												  const UFC::AnsiString& StrategyName)
{
	if ((StrategyName.Length() <= 0) || (FOrderStore->OnStrategyReport == 0))
		return;
	UFC::AnsiString NowStr;
	UFC::GetTimeString(NowStr, TRUE);
	UFC::AnsiString NameValueStr;

	NameValueStr.Printf("os=8^rjr=%d^rct=%s^txt=%s", StatusCode, NowStr.c_str(), RejectMsg.c_str());
	if( RejectResponse == 1 )
		NameValueStr = NameValueStr + "^rjt=1";
	TStrategyReport* NewRptPtr = new TStrategyReport(StrategyName, NameValueStr, NID);
	FStrategyReportLocker.Acquire();
	FStrategyReportQueue.push(NewRptPtr);
	FStrategyReportTimer->Enabled = true;
	FStrategyReportLocker.Release();
}
//---------------------------------------------------------------------------
bool TOrderAdapter::GetMarketId(const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,	nsOrderMessageDefine::MarketEnum& MarketId)
{
	BasicInformation *Info = FOrderStore->MarketDataStore->GetBasicInformation(Exchange, Symbol, false );
	if( Info != NULL )
	{
		MarketId = (nsOrderMessageDefine::MarketEnum)Info->GetMarket();
		return true;
	}
	else
	{
		UFC::BufferedLog::Printf(" TOrderAdapter::GetMarketId() Get Exchange[%s] Symbol[%s] Market Enum Error.", Exchange.c_str(), Symbol.c_str());
		UFC::BufferedLog::FlushToFile();
		return false;
	}

}
//---------------------------------------------------------------------------
