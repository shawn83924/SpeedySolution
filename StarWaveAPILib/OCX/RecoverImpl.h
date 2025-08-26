// ---------------------------------------------------------------------------
// RecoverImpl.h : Declaration of the TRecoverImpl
// ---------------------------------------------------------------------------
#ifndef RecoverImplH
#define RecoverImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"

#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TRecoverImpl     Implements IRecover, default interface of Recover
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TRecoverImpl : public TCppAutoObject<IRecover>
{
  typedef _COM_CLASS inherited;
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FProductID;
	UFC::AnsiString FSymbol;
	UFC::AnsiString FActualSymbol;
	EnumMarket		FMarket;
	double			FBuyPx[MAX_DEPTH];
	int				FBuyQty[MAX_DEPTH];
	double			FSellPx[MAX_DEPTH];
	int				FSellQty[MAX_DEPTH];
	double			FMatchPx;
	int				FMatchQty;
	UFC::AnsiString	FMatchTime;
	UFC::AnsiString	FMatchDate;
	double			FOpenPx;
	double			FDayHighPx;
	double			FDayLowPx;
	double			FClosePx;
	double			FSettlementPx;
	int				FOpenInterest;
	int				FTotalVolume;
	int				FBuyOrderCount;
	int				FBuyOrderQty;
	int				FSellOrderCount;
	int             FSellOrderQty;
	bool			FDerivedFlag;
	double			FDerivedBuyPx;
	int				FDerivedBuyQty;
	double			FDerivedSellPx;
	int				FDerivedSellQty;
	double			FPreClosePx;
	double			FBullPx;
	double			FBearPx;
	double			FRefPx;
	double			FStrikePx;
	EnumCallPut		FCallPut;
	UFC::AnsiString	FSettleMonth;
	UFC::AnsiString	FEndDate;
	EnumTradingSession FTradingSession;
    int                FSuspensionIndicator;
private:
	int    FTrdCount;
	int    FTrdTime[10];
	double FTrdPx[10];
	int    FTrdQty[10];
	int    FTrdType[10];
	UFCType::UInt32          FBuyNumOfOrders[MAX_DEPTH];
	UFCType::UInt32          FSellNumOfOrders[MAX_DEPTH];
public:
	void FromMarketDataMessage( MarketDataMessage* Msg );
public:
  __fastcall TRecoverImpl();
  __fastcall TRecoverImpl(const System::_di_IInterface Controller);
  __fastcall TRecoverImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // IRecover
protected:
  STDMETHOD(get_BuyOrderCount(long* Value));
  STDMETHOD(get_BuyOrderQty(long* Value));
  STDMETHOD(get_BuyPx(long Depth, double* Value));
  STDMETHOD(get_BuyQty(long Depth, long* Value));
  STDMETHOD(get_ClosingPx(double* Value));
  STDMETHOD(get_DayHighPx(double* Value));
  STDMETHOD(get_DayLowPx(double* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_MatchPx(double* Value));
  STDMETHOD(get_MatchQty(long* Value));
  STDMETHOD(get_MatchTime(BSTR* Value));
  STDMETHOD(get_OpeningPx(double* Value));
  STDMETHOD(get_OpenInterest(long* Value));
  STDMETHOD(get_SellOrderCount(long* Value));
  STDMETHOD(get_SellOrderQty(long* Value));
  STDMETHOD(get_SellPx(long Depth, double* Value));
  STDMETHOD(get_SellQty(long Depth, long* Value));
  STDMETHOD(get_SettlementPx(double* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_TotalVolume(long* Value));
  STDMETHOD(get_DerivedBuyPx(long* Value));
  STDMETHOD(get_DerivedBuyQty(long* Value));
  STDMETHOD(get_DerivedFlag(VARIANT_BOOL* Value));
  STDMETHOD(get_DerivedSellPx(long* Value));
  STDMETHOD(get_DerivedSellQty(long* Value));
  STDMETHOD(get_PreClosePx(double* Value));
  STDMETHOD(get_BearPx(double* BearPx));
  STDMETHOD(get_BullPx(double* BullPx));
  STDMETHOD(get_CallPut(EnumCallPut* CallorPut));
  STDMETHOD(get_ProductID(BSTR* ProductID));
  STDMETHOD(get_RefPx(double* RefPx));
  STDMETHOD(get_StrikePx(double* StrikePx));
  STDMETHOD(get_SettleDate(BSTR* Value));
  STDMETHOD(get_EndDate(BSTR* Value));
  STDMETHOD(get_TradingSession(EnumTradingSession* Value));
  STDMETHOD(get_MatchDate(BSTR* Value));
  STDMETHOD(get_TradeCount(long* Value));
  STDMETHOD(get_TradeTime(long index, long* Value));
  STDMETHOD(get_TradePrice(long index, double* Value));
  STDMETHOD(get_TradeQty(long index, long* Value));
  STDMETHOD(get_TradeType(long index, EnumHKExTrdType* Value));
  STDMETHOD(get_ActualSymbol(BSTR* Value));
  STDMETHOD(get_SuspensionIndicator(long* Value));
  STDMETHOD(get_BuyNumberOfOrders(long Depth, long* Value));
  STDMETHOD(get_SellNumberOfOrders(long Depth, long* Value));
};




#endif //RecoverImplH
