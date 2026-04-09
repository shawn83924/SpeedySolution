//---------------------------------------------------------------------------
#ifndef OrderStore_OCOH
#define OrderStore_OCOH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <list>
#include <map>
#include "OrderStore.h"
#include "MarketDataStore.h"
//---------------------------------------------------------------------------
typedef enum
{
    None,
	Pending,
	Triggered,
	Filled,
    PartialFilled,
	Failed,
	Canceled
}OCOState;
//---------------------------------------------------------------------------
typedef enum
{
	New,
	Delete,
	Edit
}OCOUpdateType;
//---------------------------------------------------------------------------
class TOCOPair
{
private:
public:
	AnsiString StrategyName;
    UFCType::Int64 NID;
    System::UnicodeString Caption;
	OCOState State;
	int OrderQty1;
	double ConditionPrice1;
	SideEnum OrderSide1;
	int OrderQty2;
	double ConditionPrice2;
	SideEnum OrderSide2;
	AnsiString Ex;
	AnsiString Symbol;
	AnsiString Description;
	TExecution* OrderData;
public:
	__fastcall TOCOPair(void);
	UnicodeString __fastcall GetStringField(int index);
};
//---------------------------------------------------------------------------
class IOCOOrderStoreListener
{
public:
	virtual AnsiString GetEx( void ) = 0;
	virtual AnsiString GetSymbol( void ) = 0;
	virtual void OnOCOOrderUpdate(TOCOPair* Pair, OCOUpdateType Type) = 0;
};
//---------------------------------------------------------------------------
class PACKAGE TOrderStore_OCO : public TComponent, public IMarketDataListener, public TOrderMessageListener
{
typedef void __fastcall (__closure *TOnOrderOCOFailed)(System::TObject* Sender, const String& ReplyMessage);
typedef void __fastcall (__closure *TGetOrderProperty)(
	System::TObject* Sender,
	int& OrderType,
	int& LimitOrderTick);

private:
	std::list<TOCOPair*> FOCOPairs;
	std::map<AnsiString, TOCOPair*> FPairingOCO;
	TCMarketDataStore* FMarketDataStore;
	TOrderStore* FOrderStore;
	std::list<IOCOOrderStoreListener*> FListeners;
	std::map<AnsiString, double> FLastPriceMap;
    int FStrategyNum;
	// Event
	TOnOrderOCOFailed FOnOrderOCOFailed;
	TGetOrderProperty FGetOrderProperty;
private:
	void __fastcall SubscribeMarketDataStore(IOCOOrderStoreListener* Listener);
	void __fastcall UnsubscribeMarketDataStore(IOCOOrderStoreListener* Listener);
	bool __fastcall CheckListenersHaveSameExchangeAndSymbol(const AnsiString Ex, const AnsiString Symbol);
	bool __fastcall UpdateLastPrice(const AnsiString& Ex, const AnsiString& Sym, double LastPrice);
	void __fastcall RemoveLastPrice(const AnsiString& Ex, const AnsiString& Sym);
	void __fastcall SendAllDataToListener(IOCOOrderStoreListener* Listener);
	void __fastcall SendDataToListener(
		const AnsiString& Ex,
		const AnsiString& Sym,
		TOCOPair* Pair,
		OCOUpdateType Type);
	std::list<TOCOPair*>* __fastcall GetPriceMatchPairs(
		const AnsiString& Ex,
		const AnsiString& Sym,
		double FillPrice);
	void __fastcall OrderingOCO(std::list<TOCOPair*>* OCOPairs, double FillPrice);
	bool __fastcall OrderingOCO(
		const AnsiString& Ex,
		const AnsiString& Sym,
		const AnsiString& StrategyName,
		SideEnum Side,
		int OrderQty,
		double Price,
		UFCType::Int64& NID );
	bool __fastcall PlaceOrder(
		const AnsiString& Ex,
		const AnsiString& Sym,
		SideEnum side,
		double Price,
		int Qty,
		OrderTypeEnum OrderType,
		PositionEffectEnum PE,
		char TWSEOrdType,
		const AnsiString& StrategyName,
		UFCType::Int64& NID);
	int __fastcall GetHoldPosition( const AnsiString& Ex, const AnsiString& Sym);
	double __fastcall GetTickPrice(
		AnsiString& Ex,
		AnsiString& Sym,
		SideEnum Side,
		double Price,
		int Step);
	AnsiString __fastcall GetStrategyName(void);
	TOCOPair* __fastcall GetMatchedPairFromExecution(TExecution* OrderRootPtr);

	// IMarketDataListener
	virtual Classes::TStringList* __fastcall GetSymbols( void );
	virtual void OnMarketDataUpdate( MarketDataMessage* Msg );
	virtual void OnMarketDataUpdate( MatchInfo* Msg );
	virtual void OnMarketDataUpdate( BasicData* Msg );
	virtual void OnMarketDataUpdate( TotalMatch* Msg );
	virtual void OnMarketDataUpdate( DayHighLowPrice* Msg );
	virtual void OnMarketDataUpdate( OpeningInfo* Msg );
	virtual void OnMarketDataUpdate( OrderBookData* Msg );
	virtual void OnMarketDataUpdate( ClosingMarketData* Msg );
	virtual void OnMarketDataUpdate( UnderlyingIndexInfo* Msg );
	virtual void OnMarketDataUpdate( SumOfOrderInfo* Msg );
	// TOrderMessageListener
	virtual void OrderMessageArrived( TExecution* OrderRootPtr);
	virtual void DeleteRejectedOrderRoot( TExecution* OrderRootPtr);
	virtual void RefreshDataNotice();
	virtual void StopOrderChanged(TExecution* OrderRootPtr, StopOrderStatusEnum StopOrderStatus);

protected:
public:
	__fastcall TOrderStore_OCO(
		TComponent* Owner,
		TCMarketDataStore* MarketDataStore,
		TOrderStore* OrderStore);
	__fastcall ~TOrderStore_OCO();
	void __fastcall Subscribe(IOCOOrderStoreListener* Listener);
	void __fastcall UnSubscribe(IOCOOrderStoreListener* Listener);
	void __fastcall OrderOCO(
		const AnsiString& Caption,
		const AnsiString& Ex,
		const AnsiString& Sym,
		nsOrderMessageDefine::SideEnum Side,
		const double Price,
		int Qty);
	void __fastcall CancelPairingOCO(
		const AnsiString& Ex,
		const AnsiString& Sym,
		nsOrderMessageDefine::SideEnum Side,
		const double Price);
	void __fastcall CancelOCO(
		const AnsiString& Ex,
		const AnsiString& Sym,
		nsOrderMessageDefine::SideEnum Side,
		const double Price);
	void __fastcall DeleteOCO(
		const AnsiString& Ex,
		const AnsiString& Sym,
		nsOrderMessageDefine::SideEnum Side,
		const double Price);

__published:
	__property TOnOrderOCOFailed OnOrderOCOFailed = { read = FOnOrderOCOFailed, write = FOnOrderOCOFailed };
	__property TGetOrderProperty GetOrderProperty = { read = FGetOrderProperty, write = FGetOrderProperty };
};
//---------------------------------------------------------------------------
#endif
