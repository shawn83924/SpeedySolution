//---------------------------------------------------------------------------
#include <vcl.h>
#include <string.h>
#include <math.h>
#pragma hdrstop
#include "XMLDoc.hpp"
#include "IdHTTP.hpp"
#include "IdSSL.hpp"
#include "IdSSLOpenSSL.hpp"
#include "TExecutionReportMessage.h"
#include "OrderStore.h"
#include "TradingObjects.h"
#include "TradingObjectPool.h"
#include <WideStrUtils.hpp>
#pragma package(smart_init)
//---------------------------------------------------------------------------
bool TOrderStore::CheckStopOrder(double CurrentMarketPrice, double StopPrice,
								 nsOrderMessageDefine::SideEnum Side,
								 nsOrderMessageDefine::OrderTypeEnum OrderType,
								 double OrderPrice, String& ErrorMessage)
{
	bool IsCorrect = false;
	ErrorMessage = L"";
	String EMsge1;
	String EMsge2;
	String EMsge3(Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_SUFFIX);
	double absMarketPrice = abs(CurrentMarketPrice);
	double absStopPrice = abs(StopPrice);
	double absOrderPrice = abs(OrderPrice);

	if (Side == nsOrderMessageDefine::sBuy)
	{
		if (absStopPrice <= absMarketPrice)
		{
			EMsge1 = Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_BUY_STOP_PRICE1;
			EMsge2 = Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_BUY_STOP_PRICE2;
			ErrorMessage.printf(L"%s%8.3f%s%8.3f%s", EMsge1.c_str(), StopPrice, EMsge2.c_str(), CurrentMarketPrice, EMsge3.c_str());
		}
		else if ((OrderType == nsOrderMessageDefine::otLimit) && (absOrderPrice < absStopPrice))
		{
			EMsge1 = Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_BUY_ORDER_PRICE1;
			EMsge2 = Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_BUY_ORDER_PRICE2;
			ErrorMessage.printf(L"%s%8.3f%s%8.3f%s", EMsge1.c_str(), OrderPrice, EMsge2.c_str(), StopPrice, EMsge3.c_str());
		}
		else
			IsCorrect = true;
	}
	else if (Side == nsOrderMessageDefine::sSell)
	{
		if (absStopPrice >= absMarketPrice)
		{
			EMsge1 = Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_SELL_STOP_PRICE1;
			EMsge2 = Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_SELL_STOP_PRICE2;
			ErrorMessage.printf(L"%s%8.3f%s%8.3f%s", EMsge1.c_str(), StopPrice, EMsge2.c_str(), CurrentMarketPrice, EMsge3.c_str());
		}
		else if ((OrderType == nsOrderMessageDefine::otLimit) && (absOrderPrice > absStopPrice))
		{
			EMsge1 = Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_SELL_ORDER_PRICE1;
			EMsge2 = Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_SELL_ORDER_PRICE2;
			ErrorMessage.printf(L"%s%8.3f%s%8.3f%s", EMsge1.c_str(), OrderPrice, EMsge2.c_str(), StopPrice, EMsge3.c_str());
		}
		else
			IsCorrect = true;
	}
	return IsCorrect;
}
//---------------------------------------------------------------------------
bool TOrderStore::RemoveStopOrderTimeNIDIndex( TExecution* StopOrderPtr )
{
	if( StopOrderPtr != NULL )
	{
		String TimeNIDKey;

		TimeNIDKey.printf(L"%s%lld", StopOrderPtr->GetLocalMessageTime(), StopOrderPtr->GetNID());
		CStringTExecutionMap::iterator it = FStopOrderTimeNIDIndex.find( TimeNIDKey );
		if( it != FStopOrderTimeNIDIndex.end())
		{
			FStopOrderTimeNIDIndex.erase( it );
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------
TExecution* TOrderStore::FindStopOrderConditionIndex(const UnicodeString& StopExchangeCode,
													 const UnicodeString& StopSymbol,
													 double StopPrice,
													 const UnicodeString& ExchangeCode,
													 const UnicodeString& Symbol,
													 double Price,
													 nsOrderMessageDefine::SideEnum Side)
{
	TExecution*   StopOrderPtr = NULL;
	TExecution*   CurStopOrderPtr;
	UnicodeString PriceString;
	UnicodeString StopOrderKey;
	UnicodeString OrderPriceString;

	if ((StopExchangeCode.Length() <= 0) || (StopSymbol.Length() <= 0) ||
		(ExchangeCode.Length() <= 0) || (Symbol.Length() <= 0))
		return NULL;

	StopOrderKey.printf(L"%s%s%020.5lf", StopExchangeCode, StopSymbol,StopPrice );
	PriceString.printf(L"%020.5lf", Price);

	typedef CStringTExecutionMultiMap::iterator MI;
	std::pair<MI, MI> g = FStopOrderConditionIndex.equal_range( StopOrderKey );
	MI CurStopOrderIt;


	for( CurStopOrderIt = g.first; CurStopOrderIt != g.second; CurStopOrderIt++)
	{
		CurStopOrderPtr = CurStopOrderIt->second;
		OrderPriceString.printf( L"%020.5lf", CurStopOrderPtr->GetPrice());
		if ((CurStopOrderPtr->GetExchangeCode() == ExchangeCode) &&
			(CurStopOrderPtr->GetSymbol()->GetSymbol(0) == Symbol) &&
			(PriceString == OrderPriceString) &&
			(Side == CurStopOrderPtr->GetSide()))
		{
			StopOrderPtr = CurStopOrderPtr;
			break;
		}
	}
	return StopOrderPtr;
}
//---------------------------------------------------------------------------
TExecution* TOrderStore::FindStopOrderByTimeNID( UnicodeString TimeNIDKey )
{
	if ((FStopOrderTimeNIDIndex.size() > 0) && (TimeNIDKey.Length() > 0))
	{
		CStringTExecutionMap::iterator it = FStopOrderTimeNIDIndex.find(TimeNIDKey);
		if( it != FStopOrderTimeNIDIndex.end() )
			return it->second;
	}
	return NULL;
}
//---------------------------------------------------------------------------
bool TOrderStore::FindStopOrder( const String& StopOrderKey,
								 const String& Exchange,
								 const String& Symbol,
								 double Price,
								 nsOrderMessageDefine::SideEnum Side,
								 CStringTExecutionMultiMap::iterator& OrdIt )
{
	typedef CStringTExecutionMultiMap::iterator MI;
	std::pair<MI, MI> g = FStopOrderConditionIndex.equal_range( StopOrderKey );
	MI CurStopOrderIt = g.first;

	while( CurStopOrderIt != g.second )
	{
		TExecution* CurStopOrderPtr = CurStopOrderIt->second;
		String CurExchange( CurStopOrderPtr->GetExchangeCode());
		String CurSymbol( CurStopOrderPtr->GetSymbol()->GetSymbol(0));

		if( Exchange == CurExchange &&	Symbol == CurSymbol &&
			Price == CurStopOrderPtr->GetPrice() &&	CurStopOrderPtr->GetSide() == Side )
		{
			OrdIt = CurStopOrderIt;
			return true;
		}
		else
			CurStopOrderIt++;
	}
	return false;
}
//---------------------------------------------------------------------------
bool TOrderStore::FindStopOrder( const String& StopOrderKey,
								 nsOrderMessageDefine::SideEnum Side,
								 CStringTExecutionMultiMap::iterator& OrdIt )
{
	typedef CStringTExecutionMultiMap::iterator MI;
	std::pair<MI, MI> g = FStopOrderConditionIndex.equal_range( StopOrderKey );
	MI CurStopOrderIt = g.first;

	while( CurStopOrderIt != g.second )
	{
		TExecution* CurStopOrderPtr = CurStopOrderIt->second;

		if( CurStopOrderPtr->GetSide() == Side )
		{
			OrdIt = CurStopOrderIt;
			return true;
		}
		else
			CurStopOrderIt++;
	}
	return false;
}
//---------------------------------------------------------------------------
bool TOrderStore::AddStopOrder( TExecution* NewStopOrderPtr )
{
	if( NewStopOrderPtr != NULL )
	{
		String TimeNIDKey;
		String StopOrderKey;

		TimeNIDKey.printf( L"%s%lld", NewStopOrderPtr->GetLocalMessageTime(), NewStopOrderPtr->GetNID());
		if( FindStopOrderByTimeNID( TimeNIDKey ) == 0 )
		{
			StopOrderKey.printf( L"%s%s%020.5lf", NewStopOrderPtr->GetStopExchangeCode(), NewStopOrderPtr->GetStopSymbol(), NewStopOrderPtr->GetStopPrice() );
			FStopOrderTimeNIDIndex.insert( CStringTExecutionMap::value_type( TimeNIDKey, NewStopOrderPtr ));
			FStopOrderConditionIndex.insert( CStringTExecutionMultiMap::value_type(StopOrderKey, NewStopOrderPtr));
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool TOrderStore::RemoveStopOrder( const String& StopExchangeCode,
								   const String& StopSymbol,
								   double StopPrice,
								   const String& Exchange,
								   const String& Symbol,
								   double Price,
								   nsOrderMessageDefine::SideEnum Side)
{
	CStringTExecutionMultiMap::iterator CurStopOrderIt;
	String StopOrderKey;

	StopOrderKey.printf(L"%s%s%020.5lf", StopExchangeCode, StopSymbol, StopPrice );
	if( FindStopOrder( StopOrderKey, Exchange, Symbol, Price,Side, CurStopOrderIt ) == true )
	{
		FStopOrderConditionIndex.erase( CurStopOrderIt );
		RemoveStopOrderTimeNIDIndex( CurStopOrderIt->second );
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
void TOrderStore::NewStopOrder( TExecution* NewStopOrderPtr, bool UpQtyOnly)
{
	if( NewStopOrderPtr != NULL )
	{
		String StopExchangeCode( NewStopOrderPtr->GetStopExchangeCode() );
		String StopSymbol( NewStopOrderPtr->GetStopSymbol() );
		String OrderExchangeCode( NewStopOrderPtr->GetExchangeCode() );
		String OrderSymbol( NewStopOrderPtr->GetSymbol()->GetSymbol(0) );
		double StopPrice  = NewStopOrderPtr->GetStopPrice();
		double OrderPrice = NewStopOrderPtr->GetPrice();
		nsOrderMessageDefine::SideEnum OrderSide = NewStopOrderPtr->GetSide();
		int OrderQty = NewStopOrderPtr->GetOrderQty();
		String StopOrderKey;
		CStringTExecutionMultiMap::iterator CurStopOrderIt;

		StopOrderKey.printf(L"%s%s%020.5lf", StopExchangeCode, StopSymbol, StopPrice );
		//Find and Delete Old Stop Order with Same Condition
		if( FindStopOrder( StopOrderKey, OrderExchangeCode, OrderSymbol, OrderPrice, OrderSide, CurStopOrderIt ) == true )
		{
			TExecution* CurStopOrderPtr = CurStopOrderIt->second;
			if( UpQtyOnly )
			{
				OrderQty += CurStopOrderPtr->GetOrderQty();
				NewStopOrderPtr->SetOrderQty(OrderQty);
			}
			FStopOrderConditionIndex.erase( CurStopOrderIt );
			RemoveStopOrderTimeNIDIndex( CurStopOrderIt->second );
			if( RemoveStopOrder( StopExchangeCode, StopSymbol, StopPrice, OrderExchangeCode, OrderSymbol, OrderPrice, OrderSide) == true  )
			{
				CurStopOrderPtr->SetOrderStatus( nsOrderMessageDefine::osCanceled );
				CurStopOrderPtr->WriteToOrderLogStream( FStopOrderFileStream );
				ExecuteStopOrderChanged( CurStopOrderPtr, soRemove );
				delete CurStopOrderPtr;
			}
		}
		if( AddStopOrder( NewStopOrderPtr ) == true )
		{
			NewStopOrderPtr->WriteToOrderLogStream( FStopOrderFileStream );
			ExecuteStopOrderChanged( NewStopOrderPtr, soNew );
			ExecuteOrderQtyListenerForStopOrder( NewStopOrderPtr->GetMarket(), OrderExchangeCode, OrderSymbol, StopPrice, OrderSide, OrderPrice, OrderQty, NewStopOrderPtr->GetOrderType() );
		}
	}
}  //TOrderStore::NewStopOrder()
//---------------------------------------------------------------------------
void TOrderStore::NewStopOrderRequest(const String& StopExchangeCode,
									  const String& StopSymbol,
									  double StopPrice,
									  nsOrderMessageDefine::MarketEnum Market,
									  const String& ExchangeCode,
									  const String& Symbol,
									  nsOrderMessageDefine::SideEnum Side,
									  double Px, int Qty,
									  nsOrderMessageDefine::OrderTypeEnum OrderType,
									  nsOrderMessageDefine::TimeInForceEnum TimeInForce,
									  nsOrderMessageDefine::PositionEffectEnum PositionEffect,
									  nsOrderMessageDefine::EventTypeEnum EventType,
									  double StrikePx,
									  bool MustTrigerOnClient,
									  bool UpQtyOnly )
{
	if ((StopExchangeCode.Length() <= 0) || (StopSymbol.Length() <= 0) ||
		(ExchangeCode.Length() <= 0) || (Symbol.Length() <= 0) ||
		(FAdapter == 0) || (FMarketDataStore == 0))
		return;

	String        tradeSymbol = GetTradeSymbol( ExchangeCode, Symbol, true);  //Check Symbol Index First
	ExchangeInfo* ExInfoPtr = FMarketDataStore->GetExchangeInfo(ExchangeCode);

	if( ExInfoPtr != NULL )///< Exchange exists
	{
		if( !MustTrigerOnClient &&  ///< Not force client side stop order.
			( ExInfoPtr->CanPlaceStopOrder() || ExInfoPtr->CanPlaceStopWithLimit()) )///< Support Server side Stop Order
		{
			///< Exchange side stop order
			NewOrder( Market, ExchangeCode, Symbol, Side, Px, Qty, OrderType, TimeInForce, PositionEffect, EventType, StrikePx, 0, StopPrice, '0' );
		}
		else
		{   //Generate a New Stop Order
			UFC::AnsiString LocalMessageTime;
			TExecution*     NewStopOrderPtr = new TExecution();
			bool            IsAccepted = true;
			UFCType::Int64  NewNID = FAdapter->GenerateNID();
			String          PriceString,TempText;

			UFC::GetTimeString( LocalMessageTime, TRUE );
			NewStopOrderPtr->SetMarket( Market );
			NewStopOrderPtr->SetExchangeCode(ExchangeCode);
			NewStopOrderPtr->SetSymbol(Symbol);
			NewStopOrderPtr->SetSide(Side);
			NewStopOrderPtr->SetPrice(Px); ///< Execute Price
			NewStopOrderPtr->SetOrderQty(Qty);
			NewStopOrderPtr->SetOrderType(OrderType);
			NewStopOrderPtr->SetTimeInForce(TimeInForce);
			NewStopOrderPtr->SetPositionEffect(PositionEffect);
			NewStopOrderPtr->SetNID(NewNID);
			NewStopOrderPtr->SetLocalMessageTime( LocalMessageTime.c_str() );
			NewStopOrderPtr->SetStopExchangeCode(StopExchangeCode);
			NewStopOrderPtr->SetStopSymbol(StopSymbol);
			NewStopOrderPtr->SetStopPrice(StopPrice); ///< Trigger Stop Price
			NewStopOrderPtr->SetStopMarketPrice( GetNewestPrice( StopExchangeCode, StopSymbol) ); ///< Filled Price at this time.
			NewStopOrderPtr->SetOrderStatus(nsOrderMessageDefine::osNew);
			NewStopOrderPtr->SetRoot( true );
			NewStopOrderPtr->SetExpanded( false );
			NewStopOrderPtr->SetCumQty(0);
			NewStopOrderPtr->SetTradeSymbol( tradeSymbol );
			NewStopOrderPtr->SetHHMM( MarketDataStore->GetClock( ExchangeCode )->GetHHMM() );
			PriceString.printf(L"%.5lf", StopPrice);
			TempText = Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_NEW_TEXT1 + StopSymbol +
					   Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_NEW_TEXT2 + PriceString +
					   Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_NEW_TEXT3;
			NewStopOrderPtr->SetText( TempText );
			if( FOnStopOrderChange != 0 )
				FOnStopOrderChange( this, StopExchangeCode, StopSymbol, StopPrice, Symbol, Px, Qty,	TimeInForce, Side, soNew, IsAccepted);
			if (IsAccepted)
			{
				AnsiString Ex( StopExchangeCode );
				AnsiString Sym( StopSymbol );
				AnsiString SideStr;

				if( Side == nsOrderMessageDefine::sBuy)
					SideStr = "Buy";
				else
					SideStr = "Sell";
				UFC::BufferedLog::Printf(" New Stop Order[%lld] %s[%s %s] Qty[%d] StopPx[%15.3lf] ExecutePx[%15.3lf]", NewNID, SideStr.c_str(), Ex.c_str(),Sym.c_str(),Qty,StopPrice,Px );
				NewStopOrder( NewStopOrderPtr, UpQtyOnly );
				SubscribeSymbol( StopExchangeCode, StopSymbol, true, false );  //need IncreaseCount, Subscribe if need
			}
			else
			{
				delete NewStopOrderPtr;
				NewStopOrderPtr = 0;
			}
		}
	}
}
//---------------------------------------------------------------------------
void TOrderStore::CancelStopOrder(const UnicodeString& StopExchangeCode,
								  const UnicodeString& StopSymbol,
								  double StopPrice,
								  const UnicodeString& ExchangeCode,
								  const UnicodeString& Symbol, double Price,
								  nsOrderMessageDefine::SideEnum Side)
{
	BasicInformation* StopInfoPtr;
	BasicInformation* InfoPtr;
	TExecution*       curStopOrderPtr;

	if ((FMarketDataStore == NULL ) ||
		(StopExchangeCode.Length() <= 0) || (StopSymbol.Length() <= 0) ||
		(ExchangeCode.Length() <= 0) || (Symbol.Length() <= 0)) return;

	if( (StopInfoPtr = GetCommodityBasicInformation(StopExchangeCode, StopSymbol)) != NULL &&
		(InfoPtr = GetCommodityBasicInformation(ExchangeCode, Symbol)) != NULL )
	{
		nsOrderMessageDefine::MarketEnum StopMarket = (nsOrderMessageDefine::MarketEnum)StopInfoPtr->GetMarket();
		nsOrderMessageDefine::MarketEnum Market = (nsOrderMessageDefine::MarketEnum)InfoPtr->GetMarket();

		if( (curStopOrderPtr = FindStopOrderConditionIndex(StopExchangeCode, StopSymbol, StopPrice, ExchangeCode, Symbol, Price, Side)) != NULL)
		{
			if (FOnStopOrderChange != NULL )
			{
				bool IsAccepted = false;
				FOnStopOrderChange(this, StopExchangeCode, StopSymbol, StopPrice,
								   curStopOrderPtr->GetSymbol()->GetSymbol(0),
								   curStopOrderPtr->GetPrice(), curStopOrderPtr->GetOrderQty(),
								   curStopOrderPtr->GetTimeInForce(), curStopOrderPtr->GetSide(),
								   soRemove, IsAccepted);
				if (!IsAccepted) return;
			}
		//	RemoveStopOrderConditionIndex(StopExchangeCode, StopSymbol, StopPrice, ExchangeCode, Symbol, Price, Side);
		//	RemoveStopOrderTimeNIDIndex(curStopOrderPtr);
			if( RemoveStopOrder( StopExchangeCode, StopSymbol, StopPrice, ExchangeCode, Symbol, Price, Side ) == true )
			{
				curStopOrderPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
				curStopOrderPtr->WriteToOrderLogStream(FStopOrderFileStream);
				ExecuteStopOrderChanged(curStopOrderPtr, soRemove);
				ExecuteOrderQtyListenerForStopOrder(curStopOrderPtr->GetMarket(), ExchangeCode, StopSymbol, StopPrice, Side, Price, 0, curStopOrderPtr->GetOrderType());
				delete curStopOrderPtr;
			}
			//UnSubscribe Symbol Price Data
			UnsubscribeSymbol(StopExchangeCode, StopSymbol, false);  //Unsubscribe if need
			///< Keep log
			AnsiString ansiStopExchangeCode( StopExchangeCode );
			AnsiString ansiStopSymbol( StopSymbol );
			AnsiString ansiSymbol( Symbol );
			UFC::BufferedLog::DebugPrintf(" Cancel Stop Order Condition[%s-%s][%.3lf]  Product[%s] Px[%.3lf]",  ansiStopExchangeCode.c_str(), ansiStopSymbol.c_str(), StopPrice, ansiSymbol.c_str(), Price);
		}
	}
}
//---------------------------------------------------------------------------
int TOrderStore::CancelStopOrder(const UnicodeString& StopExchangeCode,
								 const UnicodeString& StopSymbol,
								 double StopPrice,
								 nsOrderMessageDefine::SideEnum Side)
{
	BasicInformation*                StopInfoPtr;
	nsOrderMessageDefine::MarketEnum StopMarket;
	int    DeleteCount = 0;
	String StopOrderKey;
	TExecution* CurStopOrderPtr;

	if((StopExchangeCode.Length() <= 0) || (StopSymbol.Length() <= 0))
		return DeleteCount;
	if( (StopInfoPtr = GetCommodityBasicInformation(StopExchangeCode, StopSymbol)) == NULL )
		return DeleteCount;
	StopMarket = (nsOrderMessageDefine::MarketEnum)StopInfoPtr->GetMarket();
	StopOrderKey.printf(L"%s%s%020.5lf", StopExchangeCode, StopSymbol, StopPrice);

	//Do Search
	typedef CStringTExecutionMultiMap::iterator MI;
	std::pair<MI, MI> g = FStopOrderConditionIndex.equal_range( StopOrderKey );
	MI   CurStopOrderIt = g.first;

	while( CurStopOrderIt != g.second )
	{
		CurStopOrderPtr = CurStopOrderIt->second;
		if (Side == CurStopOrderPtr->GetSide())
		{
			if (FOnStopOrderChange != 0)
			{
				bool IsAccepted = false;
				FOnStopOrderChange(this, StopExchangeCode, StopSymbol, StopPrice,
								   CurStopOrderPtr->GetSymbol()->GetSymbol(0),
								   CurStopOrderPtr->GetPrice(), CurStopOrderPtr->GetOrderQty(),
								   CurStopOrderPtr->GetTimeInForce(), CurStopOrderPtr->GetSide(),
								   soRemove, IsAccepted);
				if (!IsAccepted)
				{
					CurStopOrderIt++;
					continue;
				}
			}
			AnsiString ansiOrderExchangeCode( CurStopOrderPtr->GetExchangeCode() );
			AnsiString ansiOrderSymbol( CurStopOrderPtr->GetSymbol()->GetSymbol(0));
			AnsiString ansiStopSymbol( StopSymbol );
			double     OrderPrice = CurStopOrderPtr->GetPrice();

			///< Remove Stop Order
			CurStopOrderIt = FStopOrderConditionIndex.erase( CurStopOrderIt );
			RemoveStopOrderTimeNIDIndex( CurStopOrderPtr );
			CurStopOrderPtr->SetOrderStatus( nsOrderMessageDefine::osCanceled );
			CurStopOrderPtr->WriteToOrderLogStream( FStopOrderFileStream );
			DeleteCount++;

			//UnSubscribe Symbol Price Data
			int subscribeCount = UnsubscribeSymbol( StopExchangeCode, StopSymbol, false);  //Unsubscribe if need
			ExecuteStopOrderChanged( CurStopOrderPtr, soRemove );
			ExecuteOrderQtyListenerForStopOrder( CurStopOrderPtr->GetMarket(), StopExchangeCode, StopSymbol, StopPrice, Side,
												 OrderPrice, 0, CurStopOrderPtr->GetOrderType());

			UFC::BufferedLog::DebugPrintf(" Cancel Stop Order[%lld] Condition[%s][%s][%.3lf]  Product[%s] Px[%.3lf] SubscribeCount %d",
											CurStopOrderPtr->GetNID(),
											ansiOrderExchangeCode.c_str(),
											ansiStopSymbol.c_str(),
											StopPrice,
											ansiOrderSymbol.c_str(),
											OrderPrice,
											subscribeCount );
			delete CurStopOrderPtr;
		}
		else
			CurStopOrderIt++;
	}
	return DeleteCount;
}
//---------------------------------------------------------------------------
int TOrderStore::CancelStopOrder( const UnicodeString& ExchangeCode,
								  const UnicodeString& Symbol,
								  nsOrderMessageDefine::SideEnum Side)
{
	int deleteCount = 0;
	if ((ExchangeCode.Length() <= 0) || (Symbol.Length() <= 0)) return deleteCount;
	BasicInformation *stopInfoPtr = GetCommodityBasicInformation(ExchangeCode, Symbol);
	if (stopInfoPtr == 0) return deleteCount;
	nsOrderMessageDefine::MarketEnum stopMarket = static_cast<nsOrderMessageDefine::MarketEnum>(stopInfoPtr->GetMarket());

	String stopPriceStr;
	stopPriceStr.printf(L"%020.5lf", 0.0);
	String lowerKey = ExchangeCode + Symbol + stopPriceStr;
	stopPriceStr.printf(L"%020.5lf", 999999999.0);
	String upperKey = ExchangeCode + Symbol + stopPriceStr;

	CStringTExecutionMultiMap::iterator curMit, lowMit, upMit;
	lowMit = FStopOrderConditionIndex.lower_bound(lowerKey);
	upMit = FStopOrderConditionIndex.upper_bound(upperKey);
	curMit = lowMit;
	while (curMit != upMit)
	{
		TExecution *curStopOrderPtr = curMit->second;
		if (curStopOrderPtr->GetSide() == Side)
		{
			RemoveStopOrderTimeNIDIndex(curStopOrderPtr);
			curMit = FStopOrderConditionIndex.erase(curMit);
			curStopOrderPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
			curStopOrderPtr->WriteToOrderLogStream(FStopOrderFileStream);
			ExecuteStopOrderChanged(curStopOrderPtr, soRemove);
			ExecuteOrderQtyListenerForStopOrder(curStopOrderPtr->GetMarket(), ExchangeCode, Symbol, curStopOrderPtr->GetStopPrice(), curStopOrderPtr->GetSide(),
												curStopOrderPtr->GetPrice(), 0, curStopOrderPtr->GetOrderType());
			UFC::AnsiString ansiStopSymbol = s888::ConvertUnicodeToUFCAnsiStr(curStopOrderPtr->GetStopSymbol());
			UFC::AnsiString ansiOrderSymbol = s888::ConvertUnicodeToUFCAnsiStr(curStopOrderPtr->GetSymbol()->GetSymbol(0));
			UFC::AnsiString ansiSideStr = s888::ConvertUnicodeToUFCAnsiStr(s888::DescribeBuySell(Side));
			UFC::BufferedLog::DebugPrintf(" Cancel Stop Order Condition[%s][%.5lf] Product[%s] Px[%.3lf] Side[%s]",
										  ansiStopSymbol.c_str(), curStopOrderPtr->GetStopPrice(), ansiOrderSymbol.c_str(), curStopOrderPtr->GetPrice(), ansiSideStr.c_str());
			delete curStopOrderPtr;
			deleteCount++;
		}
		else
			curMit ++;
	}  //while (curMit != upMit)
	return deleteCount;
}  //TOrderStore::CancelStopOrder()
//---------------------------------------------------------------------------
int TOrderStore::CancelStopOrder(const String& ExchangeCode, const String& Symbol)
{
	int deleteCount = 0;
	if ((ExchangeCode.Length() <= 0) || (Symbol.Length() <= 0)) return deleteCount;
	BasicInformation *stopInfoPtr = GetCommodityBasicInformation(ExchangeCode, Symbol);
	if (stopInfoPtr == 0) return deleteCount;
	nsOrderMessageDefine::MarketEnum stopMarket = static_cast<nsOrderMessageDefine::MarketEnum>(stopInfoPtr->GetMarket());

	String stopPriceStr;
	stopPriceStr.printf(L"%020.5lf", 0.0);
	String lowerKey = ExchangeCode + Symbol + stopPriceStr;
	stopPriceStr.printf(L"%020.5lf", 999999999.0);
	String upperKey = ExchangeCode + Symbol + stopPriceStr;

	CStringTExecutionMultiMap::iterator curMit, lowMit, upMit;
	lowMit = FStopOrderConditionIndex.lower_bound(lowerKey);
	upMit = FStopOrderConditionIndex.upper_bound(upperKey);
	curMit = lowMit;
	while (curMit != upMit)
	{
		TExecution *curStopOrderPtr = curMit->second;

		RemoveStopOrderTimeNIDIndex(curStopOrderPtr);
		curMit = FStopOrderConditionIndex.erase(curMit);
		curStopOrderPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
		curStopOrderPtr->WriteToOrderLogStream(FStopOrderFileStream);
		ExecuteStopOrderChanged(curStopOrderPtr, soRemove);
		ExecuteOrderQtyListenerForStopOrder(curStopOrderPtr->GetMarket(), ExchangeCode, Symbol, curStopOrderPtr->GetStopPrice(), curStopOrderPtr->GetSide(),
											curStopOrderPtr->GetPrice(), 0, curStopOrderPtr->GetOrderType());
		///< Keep log
		AnsiString ansiStopSymbol(curStopOrderPtr->GetStopSymbol());
		AnsiString ansiOrderSymbol( curStopOrderPtr->GetSymbol()->GetSymbol(0));
		UFC::BufferedLog::DebugPrintf(" Cancel Stop Order Condition[%s][%.5lf]  Product[%s] Px[%.3lf]", ansiStopSymbol.c_str(), curStopOrderPtr->GetStopPrice(), ansiOrderSymbol.c_str(), curStopOrderPtr->GetPrice());
		delete curStopOrderPtr;
		deleteCount++;
	}
	return deleteCount;
}
//---------------------------------------------------------------------------
void TOrderStore::CancelAllStopOrder()
{
	CStringTExecutionMap::iterator MI;
	TExecution* StopOrderPtr;

	for (MI = FStopOrderTimeNIDIndex.begin(); MI != FStopOrderTimeNIDIndex.end(); MI++)
	{
		StopOrderPtr = MI->second;
		//UnSubscribe Symbol Price Data
		AnsiString StopExchangeCode( StopOrderPtr->GetStopExchangeCode());
		AnsiString StopSymbol( StopOrderPtr->GetStopSymbol());
		AnsiString OrderExchangeCode(  StopOrderPtr->GetExchangeCode());
		AnsiString OrderSymbol( StopOrderPtr->GetSymbol()->GetSymbol(0));

		UnsubscribeSymbol( StopExchangeCode, StopSymbol, false );  //Unsubscribe if need
		StopOrderPtr->SetOrderStatus( nsOrderMessageDefine::osCanceled );
		StopOrderPtr->WriteToOrderLogStream( FStopOrderFileStream );
		ExecuteStopOrderChanged( StopOrderPtr, soRemove );
		ExecuteOrderQtyListenerForStopOrder( StopOrderPtr->GetMarket(), StopExchangeCode, StopSymbol,
											 StopOrderPtr->GetStopPrice(), StopOrderPtr->GetSide(),
											 StopOrderPtr->GetPrice(), 0, StopOrderPtr->GetOrderType());
		UFC::BufferedLog::DebugPrintf(" Cancel Stop Order Condition[%s][%.3lf]  Product[%s] Px[%.3lf]", StopSymbol.c_str(), StopOrderPtr->GetStopPrice(), OrderSymbol.c_str(), StopOrderPtr->GetPrice());
		delete StopOrderPtr;
	}
	FStopOrderTimeNIDIndex.clear();
	FStopOrderConditionIndex.clear();
}
//---------------------------------------------------------------------------
bool TOrderStore::CheckStopOrder( const AnsiString& Ex, const AnsiString& Sym, double MatchPrice, double StopPrice )
{
	double diff = fabs( StopPrice - MatchPrice );

	if( diff / StopPrice >= 0.1 )
	{
		UFC::BufferedLog::Printf(" ***** Incorrect MarketData[%s %s] StopPx[%15.3lf] but get MarketPx[%15.3lf] *****", Ex.c_str(), Sym.c_str(), StopPrice, MatchPrice );
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
int TOrderStore::RemoveStopOrderConditionIndex(const UnicodeString& StopExchangeCode,
											   const UnicodeString& StopSymbol,
											   double StopPrice,
											   const UnicodeString& ExchangeCode,
											   const UnicodeString& Symbol,
											   double Price,
											   nsOrderMessageDefine::SideEnum Side)
{
	int DeleteCount = 0;
	String StopOrderKey;//,PriceString;


	StopOrderKey.printf(L"%s%s%020.5lf", StopExchangeCode, StopSymbol, StopPrice );
//	PriceString.printf(L"%020.5lf", Price);

	typedef CStringTExecutionMultiMap::iterator MI;
	std::pair<MI, MI> g = FStopOrderConditionIndex.equal_range( StopOrderKey );
	MI CurStopOrderIt = g.first;

	while( CurStopOrderIt != g.second )
	{
		TExecution* CurStopOrderPtr = CurStopOrderIt->second;
		String CurExchange( CurStopOrderPtr->GetExchangeCode());
		String CurSymbol( CurStopOrderPtr->GetSymbol()->GetSymbol(0));
		String OrderPriceString;

//		OrderPriceString.printf( L"%020.5lf", CurStopOrderPtr->GetPrice());
		if ((ExchangeCode == CurExchange) && (Symbol == CurSymbol) && (Price == CurStopOrderPtr->GetPrice() ))
		{
			CurStopOrderIt = FStopOrderConditionIndex.erase( CurStopOrderIt );
			DeleteCount++;
		}
		else
			CurStopOrderIt++;
	}
	return DeleteCount;
}
//---------------------------------------------------------------------------
void TOrderStore::InsertStopOrderConditionIndex(const UnicodeString& StopExchangeCode,
												const UnicodeString& StopSymbol,
												double StopPrice, TExecution* StopOrderPtr)
{
	UnicodeString StopOrderKey;
	UnicodeString StopPriceString;
	StopPriceString.printf(L"%020.5lf", StopPrice);
	StopOrderKey = StopExchangeCode + StopSymbol + StopPriceString;
	FStopOrderConditionIndex.insert(CStringTExecutionMultiMap::value_type(StopOrderKey, StopOrderPtr));
}  //InsertStopOrderConditionIndex()

//---------------------------------------------------------------------------
void TOrderStore::ClearStopOrderData()
{
	TExecution* StopOrderPtr;
	CStringTExecutionMap::iterator MI;

	for (MI = FStopOrderTimeNIDIndex.begin(); MI != FStopOrderTimeNIDIndex.end(); MI++)
	{
		StopOrderPtr = MI->second;
		delete StopOrderPtr;
	}  //for (MI = FStopOrderTimeNIDIndex.begin(); MI != FStopOrderTimeNIDIndex.end(); MI++)

	FStopOrderTimeNIDIndex.clear();
	FStopOrderConditionIndex.clear();
}  //TOrderStore::ClearStopOrderData()
//---------------------------------------------------------------------------
bool TOrderStore::HasStopOrder()
{
	if( FStopOrderTimeNIDIndex.begin() == FStopOrderTimeNIDIndex.end() )
		return false;
	return true;
}
//---------------------------------------------------------------------------
bool TOrderStore::IsStopOrderExist()
{
	if (FStopOrderConditionIndex.size() > 0)
		return true;
	else
		return false;
}  //TOrderStore::IsStopOrderExist()

//---------------------------------------------------------------------------
bool TOrderStore::IsStopOrderExist(const UnicodeString& StopExchangeCode, const UnicodeString& StopSymbol)
{
	bool IsExist = false;
	if ((StopExchangeCode <= 0) || (StopSymbol.Length() <= 0)) return IsExist;
	BasicInformation *StopInfoPtr = GetCommodityBasicInformation(StopExchangeCode, StopSymbol);
	if (StopInfoPtr == 0) return false;
	nsOrderMessageDefine::MarketEnum Market = static_cast<nsOrderMessageDefine::MarketEnum>(StopInfoPtr->GetMarket());

	CStringTExecutionMultiMap::iterator MI;
	TExecution* CurStopOrderPtr;
	UnicodeString CurStopExchange;
	UnicodeString CurStopSymbol;
	for (MI = FStopOrderConditionIndex.begin(); MI != FStopOrderConditionIndex.end(); MI++)
	{
		CurStopOrderPtr = MI->second;
		CurStopExchange = CurStopOrderPtr->GetStopExchangeCode();
		CurStopSymbol = CurStopOrderPtr->GetStopSymbol();
		if (((CurStopExchange.Length() > 0) && (CurStopExchange == StopExchangeCode)) &&
			((CurStopSymbol.Length() > 0) && (CurStopSymbol == StopSymbol)))
		{
			IsExist = true;
			break;
		}
	}  //for (MI = FStopOrderConditionIndex.begin(); MI != FStopOrderConditionIndex.end(); MI++)

	return IsExist;
}  //TOrderStore::IsStopOrderExist()
//---------------------------------------------------------------------------
void TOrderStore::WriteStopOrderToFile()
{
	if (FStopOrderFileStream != 0) delete FStopOrderFileStream;

	if (FileExists(FStopOrderFileName))
	{
		FStopOrderFileStream = new TFileStream(FStopOrderFileName, fmOpenWrite);
		FStopOrderFileStream->Seek(0, soFromEnd);
	}
	else
		FOrderMsgFileStream = new TFileStream(FOrderMsgLogFileName, fmCreate);

	delete FStopOrderFileStream;
}  //TOrderStore::WriteStopOrderToFile()
//---------------------------------------------------------------------------
void TOrderStore::ProcessStopOrder(const String& MatchExchange, const String& MatchSymbol, double MatchPrice)
{
	while( ExecuteStopOrder( MatchExchange, MatchSymbol, MatchPrice ) == true )
	{
		UFC::BufferedLog::FlushToFile();
	};
}
//---------------------------------------------------------------------------
void  TOrderStore::StopOrderToNewOrder( TExecution* CurStopOrderPtr )
{
	AnsiString ansiStrategyName( CurStopOrderPtr->GetStrategyName() );
	///< Send NewOrder to
	if( CurStopOrderPtr->GetOrderType() == nsOrderMessageDefine::otMarket )
	{
		if( CurStopOrderPtr->GetTimeInForce() == nsOrderMessageDefine::tifFOK )
			UFC::BufferedLog::Printf(" Send new order Qty[%d] Market[FOK]", CurStopOrderPtr->GetOrderQty() );
		else if( CurStopOrderPtr->GetTimeInForce() == nsOrderMessageDefine::tifIOC )
			UFC::BufferedLog::Printf(" Send new order Qty[%d] Market[IOC]", CurStopOrderPtr->GetOrderQty() );
		else
			UFC::BufferedLog::Printf(" Send new order Qty[%d] Market[ROD]", CurStopOrderPtr->GetOrderQty() );
	}
	else
		UFC::BufferedLog::Printf(" Send new order Qty[%d] Price[%15.3lf]", CurStopOrderPtr->GetOrderQty(), CurStopOrderPtr->GetPrice() );
	FAdapter->NewSingleOrder(  CurStopOrderPtr->GetMarket(),
							   CurStopOrderPtr->GetExchangeCode(),
							   CurStopOrderPtr->GetSymbol()->GetSymbol(0),
							   CurStopOrderPtr->GetSide(),
							   CurStopOrderPtr->GetPrice(),
							   CurStopOrderPtr->GetOrderQty(),
							   CurStopOrderPtr->GetOrderType(),
							   CurStopOrderPtr->GetTimeInForce(),
							   CurStopOrderPtr->GetPositionEffect(),
							   nsOrderMessageDefine::evtNone,
							   0.0,
							   GetMarketAccountForOrder(CurStopOrderPtr->GetMarket()),
							   FActiveExecutive,
							   FIBID, 0, CurStopOrderPtr->GetStopPrice(), ansiStrategyName.c_str(), '0' );
}
//---------------------------------------------------------------------------
bool  TOrderStore::ExecuteStopOrder(const String& MatchExchange, const String& MatchSymbol, double MatchPrice)
{
	CStringTExecutionMultiMap::iterator CurStopOrderIt;

	for( CurStopOrderIt = FStopOrderConditionIndex.begin(); CurStopOrderIt != FStopOrderConditionIndex.end();)
	{
		TExecution* CurStopOrderPtr;

		if( (CurStopOrderPtr = CurStopOrderIt->second )!= NULL )
		{
			String CurStopExchange( CurStopOrderPtr->GetStopExchangeCode());
			String CurStopSymbol( CurStopOrderPtr->GetStopSymbol());
			AnsiString ansiExchangeCode( CurStopExchange);
			AnsiString ansiSymbolCode( CurStopSymbol );
			double StopPrice = CurStopOrderPtr->GetStopPrice();
			double StopMarketPrice = CurStopOrderPtr->GetStopMarketPrice();
			///< MatchPrice: Last filled Price
			///< StopPrice:  Trigger Price
			///< StopMarketPrice: Filled Price when Stop Order placed
			if( CurStopExchange == MatchExchange && CurStopSymbol == MatchSymbol &&   ///< Exchange, Symbol matched.
				((StopPrice == StopMarketPrice) ||  ///< Place Stop order at current match price
				((StopPrice < StopMarketPrice) && (MatchPrice <= StopPrice)) ||
				((StopPrice > StopMarketPrice) && (MatchPrice >= StopPrice))) &&
				CheckStopOrder( ansiExchangeCode, ansiSymbolCode, MatchPrice, StopPrice ) == true )  ///< Price in 10%
			{
				bool IsAccepted = true;

				if( FOnStopOrderChange != NULL )
					FOnStopOrderChange(this, MatchExchange, MatchSymbol, MatchPrice,
									   CurStopOrderPtr->GetSymbol()->GetSymbol(0),
									   CurStopOrderPtr->GetPrice(), CurStopOrderPtr->GetOrderQty(),
									   CurStopOrderPtr->GetTimeInForce(), CurStopOrderPtr->GetSide(),
									   soSend, IsAccepted);
				if( IsAccepted == true )
				{
					AnsiString ansiSideStr = (CurStopOrderPtr->GetSide() == nsOrderMessageDefine::sBuy)? "Buy" : "Sell";

					CurStopOrderIt = FStopOrderConditionIndex.erase( CurStopOrderIt );
					RemoveStopOrderTimeNIDIndex( CurStopOrderPtr );
					ExecuteStopOrderChanged( CurStopOrderPtr, soSend );
					CurStopOrderPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
					CurStopOrderPtr->WriteToOrderLogStream(FStopOrderFileStream);
					ExecuteOrderQtyListenerForStopOrder( CurStopOrderPtr->GetMarket(),
														 MatchExchange, MatchSymbol,
														 StopPrice,
														 CurStopOrderPtr->GetSide(),
														 CurStopOrderPtr->GetPrice(),
														 0,
														 CurStopOrderPtr->GetOrderType());
					UFC::BufferedLog::Printf( " Stop Order[%lld] Touched %s[%s %s] MarketPx[%15.3lf] StopPx[%15.3lf] Place Stop order MarketPx[%15.3lf]",
											  CurStopOrderPtr->GetNID(), ansiSideStr.c_str(), ansiExchangeCode.c_str(), ansiSymbolCode.c_str(), MatchPrice, StopPrice, StopMarketPrice);
					///< Unsubscribe market data
					UnsubscribeSymbol( CurStopExchange, CurStopSymbol, false);
					///< Excute StopOrder to NewOrder
					StopOrderToNewOrder( CurStopOrderPtr );
					delete CurStopOrderPtr;
					return true;
				}
			}
		}
		CurStopOrderIt++;
	}
	return false;
}
//---------------------------------------------------------------------------

