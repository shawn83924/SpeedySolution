// ---------------------------------------------------------------------------
// OrderBookImpl.h : Declaration of the TOrderBookImpl
// ---------------------------------------------------------------------------
#ifndef OrderBookImplH
#define OrderBookImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"

// ---------------------------------------------------------------------------
// TOrderBookImpl     Implements IOrderBook, default interface of OrderBook
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TOrderBookImpl : public TCppAutoObject<IOrderBook>
{
  typedef _COM_CLASS inherited;
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	UFC::AnsiString	FTime; ///< HH:MM:ss.mmm
	double			FBuyPx[MAX_DEPTH];
	int				FBuyQty[MAX_DEPTH];
	double			FSellPx[MAX_DEPTH];
	int				FSellQty[MAX_DEPTH];
	UFCType::UInt32          FBuyNumOfOrders[MAX_DEPTH];
	UFCType::UInt32          FSellNumOfOrders[MAX_DEPTH];
	bool			FDerivedFlag;
	double			FDerivedBuyPx;
	int				FDerivedBuyQty;
	double			FDerivedSellPx;
	int				FDerivedSellQty;
	bool            FIsTestMatch;
	EnumTradingSession FTradingSession;
public:
	void FromOrderBookData( OrderBookData* Data );
public:
  __fastcall TOrderBookImpl();
  __fastcall TOrderBookImpl(const System::_di_IInterface Controller);
  __fastcall TOrderBookImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);


  // IOrderBook
protected:
  STDMETHOD(get_DerivedBuyPx(double* Value));
  STDMETHOD(get_DerivedBuyQty(long* Value));
  STDMETHOD(get_DerivedFlag(VARIANT_BOOL* Value));
  STDMETHOD(get_DerivedSellPx(double* Value));
  STDMETHOD(get_DerivedSellQty(long* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));

  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_BuyPx(long Depth, double* Value));
  STDMETHOD(get_BuyQty(long Depth, long* Value));
  STDMETHOD(get_SellPx(long Depth, double* Value));
  STDMETHOD(get_SellQty(long Depth, long* Value));
  STDMETHOD(get_Time(BSTR* Value));
  STDMETHOD(get_IsTestMatch(VARIANT_BOOL* Value));
  STDMETHOD(get_TradingSession(EnumTradingSession* Value));
  STDMETHOD(get_BuyNumberOfOrders(long Depth, long* Value));
  STDMETHOD(get_SellNumberOfOrders(long Depth, long* Value));
};




#endif //OrderBookImplH
