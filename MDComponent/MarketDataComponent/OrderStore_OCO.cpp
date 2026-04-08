//---------------------------------------------------------------------------
#pragma hdrstop

#include "OrderStore_OCO.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TOrderStore_OCO *)
{
	new TOrderStore_OCO(NULL, NULL, NULL);
}
//---------------------------------------------------------------------------
__fastcall TOrderStore_OCO::TOrderStore_OCO(
	TComponent* Owner,
	TCMarketDataStore* MarketDataStore,
	TOrderStore* OrderStore)
	: TComponent(Owner)
    , FStrategyNum(0)
{
	FMarketDataStore = MarketDataStore;
	FOrderStore = OrderStore;
	FOrderStore->AddOrderMessageListener(this);
}
//---------------------------------------------------------------------------
__fastcall TOrderStore_OCO::~TOrderStore_OCO()
{
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::Register(IOCOOrderStoreListener* Listener)
{
	//Check Listener is exist in FListeners.
	//If exist, remove it and add again.
	if(std::find(FListeners.begin(), FListeners.end(), Listener) != FListeners.end())
		UnRegister(Listener);

	FListeners.push_back(Listener);
	SendAllDataToListener(Listener);
	SubscribeMarketDataStore(Listener);
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::UnRegister(IOCOOrderStoreListener* Listener)
{
	FListeners.remove(Listener);
	UnsubscribeMarketDataStore(Listener);
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::SubscribeMarketDataStore(IOCOOrderStoreListener* Listener)
{
	if(FMarketDataStore == NULL)
		return;

	UFC::AnsiString Ex( Listener->GetEx().c_str() );
	UFC::AnsiString Sym( Listener->GetSymbol().c_str() );
	FMarketDataStore->Subscribe(Ex, Sym, this);
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::UnsubscribeMarketDataStore(IOCOOrderStoreListener* Listener)
{
	if(FMarketDataStore == NULL)
		return;

	UFC::AnsiString Ex( Listener->GetEx().c_str() );
	UFC::AnsiString Sym( Listener->GetSymbol().c_str() );
	FMarketDataStore->Unsubscribe(Ex, Sym, this);
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::OrderOCO(
	const AnsiString& Caption,
	const AnsiString& Ex,
	const AnsiString& Sym,
	nsOrderMessageDefine::SideEnum Side,
	const double Price,
	int Qty)
{
	int position = GetHoldPosition(Ex, Sym);
	if( position <= 0)
	{
		OnOrderOCOFailed(this, Mdcomponentstrings_MD_ORDERBOOK_NOPOSITION);
		return;
	}
	AnsiString key = Ex + "_" + Sym;
	// Find FPairingOCO have any key match Ex_Sym
	// If find pair, update pair's property and remove from FPairingOCO and insert to FOCOPairs
	if( FPairingOCO.find(key) != FPairingOCO.end() )
	{
		TOCOPair* pair = FPairingOCO[key];
		pair->OrderQty2 = Qty;
		pair->ConditionPrice2 = Price;
		pair->OrderSide2 = Side;
		pair->State = OCOState::Pending;
		pair->StrategyName = GetStrategyName();
		FOCOPairs.push_back( pair );
		FPairingOCO.erase(key);

		SendDataToListener(Ex, Sym, pair, true);
		return;
	}

	TOCOPair* pair = new TOCOPair();
	pair->OrderQty1 = Qty;
	pair->ConditionPrice1 = Price;
	pair->OrderSide1 = Side;
	pair->State = OCOState::None;
	pair->Caption = Caption;
	pair->Ex = Ex;
	pair->Symbol = Sym;
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::CancelPairingOCO(
	const AnsiString& Ex,
	const AnsiString& Sym,
	nsOrderMessageDefine::SideEnum Side,
	const double Price)
{
	AnsiString key = Ex + "_" + Sym;
	std::map<AnsiString, TOCOPair*>::iterator it = FPairingOCO.find(key);
	if (it == FPairingOCO.end())
		return;

	TOCOPair* pair = it->second;
	bool matchFirst  = (pair->OrderSide1 == Side && pair->ConditionPrice1 == Price);
	bool matchSecond = (pair->OrderSide2 == Side && pair->ConditionPrice2 == Price);

	if (!matchFirst && !matchSecond)
		return;

	FPairingOCO.erase(it);
	SendUnPairToListener(Ex, Sym);
	delete pair;
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::CancelOCO(
		const AnsiString& Ex,
		const AnsiString& Sym,
		nsOrderMessageDefine::SideEnum Side,
		const double Price)
{
    for( std::list<TOCOPair*>::iterator it = FOCOPairs.begin(); it != FOCOPairs.end(); ++it )
	{
		TOCOPair* pair = *it;
		if(pair == NULL)
			continue;

		if(pair->Ex != Ex || pair->Symbol != Sym )
			continue;

		bool matchFirst  = (pair->OrderSide1 == Side && pair->ConditionPrice1 == Price);
		bool matchSecond = (pair->OrderSide2 == Side && pair->ConditionPrice2 == Price);

		if( !matchFirst && !matchSecond )
			continue;

        pair->State = OCOState::Canceled;
		SendDataToListener(Ex, Sym, pair, OCOUpdateType::Edit);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::DeleteOCO(
	const AnsiString& Ex,
	const AnsiString& Sym,
	nsOrderMessageDefine::SideEnum Side,
	const double Price)
{
	for( std::list<TOCOPair*>::iterator it = FOCOPairs.begin(); it != FOCOPairs.end(); ++it )
	{
		TOCOPair* pair = *it;
		if(pair == NULL)
			continue;

		if(pair->Ex != Ex || pair->Symbol != Sym )
			continue;

		bool matchFirst  = (pair->OrderSide1 == Side && pair->ConditionPrice1 == Price);
		bool matchSecond = (pair->OrderSide2 == Side && pair->ConditionPrice2 == Price);

		if( !matchFirst && !matchSecond )
			continue;

		if(pair->State == OCOState::Triggered)
		{
			TExecution* orderData = pair->OrderData;
			FOrderStore->CancelOrder(
				orderData->GetMarket(),
				orderData->GetExchangeCode(),
				orderData->GetOrderID(),
				s888::ConvertUnicodeToUFCAnsiStr( orderData->GetStrategyName())
				);
		}
		FOCOPairs.erase(it);
		SendDataToListener(Ex, Sym, pair, OCOUpdateType::Delete);
		delete pair;
	}
}
//---------------------------------------------------------------------------
bool __fastcall TOrderStore_OCO::UpdateLastPrice(const AnsiString& Ex, const AnsiString& Sym, double LastPrice)
{
	AnsiString key = Ex + "_" + Sym;
	bool isUpdated = false;

	if( FPairingOCO.find(key) != FPairingOCO.end() )
	{
		double oldPrice = FLastPriceMap[key];
		if( oldPrice != LastPrice )
		{
			FLastPriceMap[key] = LastPrice;
			isUpdated = true;
		}
		return isUpdated;
	}

	FLastPriceMap[key] = LastPrice;
	isUpdated = true;

	return isUpdated;
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::RemoveLastPrice(const AnsiString& Ex, const AnsiString& Sym)
{
	AnsiString key = Ex + "_" + Sym;
	FLastPriceMap.erase(key);
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::SendAllDataToListener(IOCOOrderStoreListener* Listener)
{
	AnsiString Ex( Listener->GetEx().c_str() );
	AnsiString Sym( Listener->GetSymbol().c_str() );
	//send FOCOPairs data
	for( std::list<TOCOPair*>::iterator it = FOCOPairs.begin(); it != FOCOPairs.end(); ++it )
	{
		TOCOPair* pair = *it;
		if(pair == NULL)
			continue;

		if(pair->Ex == Ex && pair->Symbol == Sym )
			Listener->OnOCOOrderUpdate( pair, true );
	}
	// send FPairingOCO data
	AnsiString key = Ex + "_" + Sym;
	if( FPairingOCO.find(key) != FPairingOCO.end() )
	{
		TOCOPair* pair = FPairingOCO[key];
		Listener->OnOCOOrderUpdate( pair, true );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::SendDataToListener(
	const AnsiString& Ex,
	const AnsiString& Sym,
	TOCOPair* Pair,
	OCOUpdateType Type)
{
	for( std::list<IOCOOrderStoreListener*>::iterator it = FListeners.begin(); it != FListeners.end(); ++it )
	{
		IOCOOrderStoreListener* Listener = *it;
		if( Listener->GetEx() != Ex || Listener->GetSymbol() != Sym )
			continue;
		Listener->OnOCOOrderUpdate( Pair, Type );
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::SendUnPairToListener(const AnsiString& Ex, const AnsiString& Sym)
{
	for( std::list<IOCOOrderStoreListener*>::iterator it = FListeners.begin(); it != FListeners.end(); ++it )
	{
		IOCOOrderStoreListener* Listener = *it;
		if( Listener->GetEx() != Ex || Listener->GetSymbol() != Sym )
			continue;
		Listener->OnUnPairingOCO();
	}
}
//---------------------------------------------------------------------------
std::list<TOCOPair*>* __fastcall TOrderStore_OCO::GetPriceMatchPairs(
		const AnsiString& Ex,
		const AnsiString& Sym,
		double FillPrice)
{
	std::list<TOCOPair*>* MatchPairs = new std::list<TOCOPair*>();
	for( std::list<TOCOPair*>::iterator it = FOCOPairs.begin(); it != FOCOPairs.end(); ++it )
	{
		TOCOPair* pair = *it;
		if(pair == NULL)
			continue;

		if(pair->Ex != Ex || pair->Symbol != Sym )
			continue;

		bool match1	= (pair->ConditionPrice1 >= FillPrice && pair->ConditionPrice2 >= FillPrice);
		bool match2 = (pair->ConditionPrice1 <= FillPrice && pair->ConditionPrice2 <= FillPrice);
		if( !match1 || !match2 )
			continue;

		MatchPairs->push_back( pair );
	}
	return MatchPairs;
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore_OCO::OrderingOCO(std::list<TOCOPair*>* TOCOPairs, double fillPrice)
{
	//loop every pairs and get orderQty, side and price to send order
	for( std::list<TOCOPair*>::iterator it = TOCOPairs->begin(); it != TOCOPairs->end(); ++it )
	{
		TOCOPair* pair = *it;
		if(pair == NULL)
			continue;
		if(pair->State != OCOState::Pending)
			continue;

		int orderQty;
		double matchPrice;
		SideEnum side;
		double diff1 = std::fabs(pair->ConditionPrice1 - fillPrice);
		double diff2 = std::fabs(pair->ConditionPrice2 - fillPrice);
		if( diff1 < diff2 )
		{
			orderQty = pair->OrderQty1;
			matchPrice = pair->ConditionPrice1;
			side = pair->OrderSide1;
		}
		else
		{
			orderQty = pair->OrderQty2;
			matchPrice = pair->ConditionPrice2;
			side = pair->OrderSide2;
		}
        // Ordering OCO to market and get NID by reference
		bool success = OrderingOCO(
			pair->Ex,
			pair->Symbol,
			pair->StrategyName,
			side,
			orderQty,
			matchPrice,
			pair->NID );

		if(!success)
		{
			pair->State = OCOState::Failed;
			SendDataToListener( pair->Ex, pair->Symbol, pair, OCOUpdateType::Edit );
			OnOrderOCOFailed(this, Mdcomponentstrings_MD_ORDERSTORE_OCO_ORDER_NEW_ERROR);
			continue;
		}

		pair->State = OCOState::Triggered;
		SendDataToListener( pair->Ex, pair->Symbol, pair, OCOUpdateType::Edit );
	}
}
//---------------------------------------------------------------------------
bool __fastcall TOrderStore_OCO::OrderingOCO(
	const AnsiString& Ex,
	const AnsiString& Sym,
	const AnsiString& StrategyName,
	SideEnum Side,
	int OrderQty,
	double Price,
	UFCType::Int64& NID)
{
	int holdPosition = 	GetHoldPosition( Ex, Sym);
	//if side is sell and OrderQty bigger than position, return
	if( Side == nsOrderMessageDefine::sSell && OrderQty > holdPosition )
		return false;

	int OCOType = 0;
	int LimitOrderTick = 0;
	PositionEffectEnum pe = nsOrderMessageDefine::peAuto;
	char twseOrdType = '0';
	if( GetOrderProperty != NULL )
		GetOrderProperty(this, OCOType, LimitOrderTick);

	nsOrderMessageDefine::OrderTypeEnum orderType;
    switch(OCOType)
	{
		case 0:
			orderType = nsOrderMessageDefine::otLimit;
			break;
		case 1:
		{
			orderType = nsOrderMessageDefine::otMarket;
			AnsiString ex = Ex;
			AnsiString sym = Sym;
			Price = GetTickPrice(ex, sym, Side, Price, LimitOrderTick);
			break;
		}
		case 2:
			orderType = nsOrderMessageDefine::otMarketWithProtection;
			break;
	}

	return PlaceOrder(
		Ex,
		Sym,
		Side,
		Price,
		OrderQty,
		orderType,
		pe,
		twseOrdType,
		StrategyName,
		NID);
}
//---------------------------------------------------------------------------
int __fastcall TOrderStore_OCO::GetHoldPosition( const AnsiString& Ex, const AnsiString& Sym)
{
	s888::CPositionStatisticRecord* record = FOrderStore->FindPosStatisticRecByTradeSymbol( Ex, Sym, s888::rwLockForRead);
	// get position by record
	int holdPosition = 	record->GetBuyHoldOpenQuantity(s888::rwLockForRead) -
						record->GetSellHoldOpenQuantity(s888::rwLockForRead);
	return holdPosition;
}
//---------------------------------------------------------------------------
bool __fastcall TOrderStore_OCO::PlaceOrder(
	const AnsiString& Ex,
	const AnsiString& Sym,
	SideEnum side,
	double Price,
	int Qty,
	OrderTypeEnum OrderType,
	PositionEffectEnum PE,
	char TWSEOrdType,
	const AnsiString& StrategyName,
	UFCType::Int64& NID )
{
	if( FOrderStore == NULL || FMarketDataStore == NULL )
		return false;

	BasicInformation* info = FMarketDataStore->GetBasicInformation( Ex, Sym, false );
	if( info == NULL )
		return false;

	MarketEnum market = info->GetMarket();
	EventTypeEnum callPut = (EventTypeEnum)info->GetCallPut();
	double strikePx = info->GetStrikePrice();

	return FOrderStore->NewOrder(
		market,
		Ex,
		Sym,
		side,
		Price,
		Qty,
		OrderType,
		nsOrderMessageDefine::tifROD,
		PE,
		callPut,
		strikePx,
		0,
		0.0,
		StrategyName.c_str(),
		TWSEOrdType,
		NID);
}
//---------------------------------------------------------------------------
double __fastcall TOrderStore_OCO::GetTickPrice(AnsiString& Ex, AnsiString& Sym, SideEnum Side, double Price, int Step )
{
	if( Step == 0 )
		return Price;

	TTickList* TickList = const_cast<TTickList*>( FMarketDataStore->GetTickList( Ex, Sym ) );
	if( TickList == NULL )
		return Price;

	int CurrIndex = TickList->PxToTickIndex( Price, false );
	int TargetIndex = CurrIndex;
	if( Side == nsOrderMessageDefine::sBuy )
		TargetIndex = CurrIndex - Step; // Buy protection moves to higher price.
	else
		TargetIndex = CurrIndex + Step; // Sell protection moves to lower price.
	return TickList->GetPxFromIndex( TargetIndex, false );
}
//---------------------------------------------------------------------------
AnsiString __fastcall TOrderStore_OCO::GetStrategyName(void)
{
	if( FStrategyNum == INT_MAX )
		FStrategyNum = 0;
	else
		FStrategyNum++;
	return AnsiString( FStrategyNum );
}
//---------------------------------------------------------------------------
TOCOPair* __fastcall TOrderStore_OCO::GetMatchedPairFromExecution(TExecution* OrderRootPtr)
{
	if(OrderRootPtr == NULL)
		return NULL;

	for( std::list<TOCOPair*>::iterator it = FOCOPairs.begin(); it != FOCOPairs.end(); ++it )
	{
		TOCOPair* pair = *it;
		if(pair == NULL)
			continue;

		if( pair->NID != OrderRootPtr->GetNID())
			continue;

        return pair;
	}
}
//---------------------------------------------------------------------------
// Implement IMarketDataListener function
void TOrderStore_OCO::OnMarketDataUpdate( MarketDataMessage* Msg )
{
	double fillPrice = Msg->GetTradePrice();
	double bullPrice = Msg->GetBullPx();
	double bearPrice = Msg->GetBearPx();
	if( fillPrice > bullPrice || fillPrice < bearPrice )
		return;

	AnsiString Ex( Msg->GetExchange().c_str());
	AnsiString Sym( Msg->GetSymbol().c_str());
	bool isPriceUpdate = UpdateLastPrice(Ex, Sym, fillPrice);
	if( !isPriceUpdate )
		return;

	std::list<TOCOPair*>* matchPairs = GetPriceMatchPairs(Ex, Sym, fillPrice);
	OrderingOCO( matchPairs, fillPrice);

	delete matchPairs;
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::OnMarketDataUpdate( MatchInfo* Msg )
{
	double fillPrice = Msg->GetMatchPrice();

	AnsiString Ex( Msg->GetExchange().c_str() );
	AnsiString Sym( Msg->GetSymbol().c_str() );

	bool isPriceUpdate = UpdateLastPrice(Ex, Sym, fillPrice);
	if( !isPriceUpdate )
		return;

	std::list<TOCOPair*>* matchPairs = GetPriceMatchPairs(Ex, Sym, fillPrice);
	OrderingOCO( matchPairs, fillPrice);

	delete matchPairs;
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::OnMarketDataUpdate( BasicData* Msg )
{
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::OnMarketDataUpdate( TotalMatch* Msg )
{
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::OnMarketDataUpdate( DayHighLowPrice* Msg )
{
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::OnMarketDataUpdate( OpeningInfo* Msg )
{
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::OnMarketDataUpdate( OrderBookData* Msg )
{
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::OnMarketDataUpdate( ClosingMarketData* Msg )
{
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::OnMarketDataUpdate( UnderlyingIndexInfo* Msg )
{
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::OnMarketDataUpdate( SumOfOrderInfo* Msg )
{
}
//---------------------------------------------------------------------------
// Implement TOrderMessageListener function
void TOrderStore_OCO::OrderMessageArrived( TExecution* OrderRootPtr )
{
	if (OrderRootPtr == NULL )
		return;

	TOCOPair* pair = GetMatchedPairFromExecution(OrderRootPtr);
	if(pair == NULL)
		return;

	switch (OrderRootPtr->GetOrderStatus())
	{
		case nsOrderMessageDefine::osPartiallyFilled:
			pair->State = PartialFilled;
			break;
		case nsOrderMessageDefine::osFilled:
			pair->State = Filled;
			break;
		default:
			break;
	}

	pair->OrderData = OrderRootPtr;
	SendDataToListener( pair->Ex, pair->Symbol, pair, OCOUpdateType::Edit );
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::DeleteRejectedOrderRoot( TExecution* OrderRootPtr)
{
	if (OrderRootPtr ==  NULL )
		return;

	TOCOPair* pair = GetMatchedPairFromExecution(OrderRootPtr);
	if(pair == NULL)
		return;

	pair->OrderData = OrderRootPtr;
	pair->State = Failed;
	SendDataToListener( pair->Ex, pair->Symbol, pair, OCOUpdateType::Edit );
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::RefreshDataNotice()
{
}
//---------------------------------------------------------------------------
void TOrderStore_OCO::StopOrderChanged(TExecution* OrderRootPtr, StopOrderStatusEnum StopOrderStatus)
{
}
//---------------------------------------------------------------------------
Classes::TStringList* __fastcall TOrderStore_OCO::GetSymbols( void )
{
	Classes::TStringList* symbols = new Classes::TStringList();
	for( std::list<TOCOPair*>::iterator it = FOCOPairs.begin(); it != FOCOPairs.end(); ++it )
	{
		TOCOPair* pair = *it;
		if(pair == NULL)
			continue;

		if( symbols->IndexOf( pair->Symbol.c_str() ) == -1 )
			symbols->Add( pair->Symbol.c_str() );
	}
	return symbols;
}
//---------------------------------------------------------------------------
__fastcall TOCOPair::TOCOPair(void)
{
    NID = 0;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TOCOPair::GetStringField(int index)
{
	UnicodeString ResultString;
	switch(index)
	{
		case 1:
			if(State == OCOState::Pending)
				ResultString = L"待觸發";
			else if(State == OCOState::Triggered)
				ResultString = L"已觸發";
			else if(State == OCOState::Filled)
				ResultString = L"已成交";
			else if(State = OCOState::PartialFilled)
				ResultString = L"部分成交";
			else if(State = OCOState::Failed)
				ResultString = L"成交失敗";
			else if(State = OCOState::Canceled)
                ResultString = L"已取消";
		case 2:
			ResultString = Caption;
			break;
		case 3:
			ResultString = Symbol.c_str();
			break;
		case 4:
			ResultString = IntToStr(OrderQty1);
			break;
		case 5:
			ResultString = FloatToStr(ConditionPrice1);
			break;
		case 6:
			if(OrderSide1 == nsOrderMessageDefine::sBuy)
				ResultString = L"委買";
			else
				ResultString = L"委賣";
			break;
		case 7:
			ResultString = IntToStr(OrderQty2);
			break;
		case 8:
			ResultString = FloatToStr(ConditionPrice2);
			break;
		case 9:
			if(OrderSide2 == nsOrderMessageDefine::sBuy)
				ResultString = L"委買";
			else
				ResultString = L"委賣";
			break;

	}

	return ResultString;
}
//---------------------------------------------------------------------------
namespace Orderstore_oco
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TOrderStore_OCO)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
