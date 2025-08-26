// 1.2
// Unit1.h : Declaration of the TOrderBookImpl

#ifndef OrderBookDataH
#define OrderBookDataH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"

/////////////////////////////////////////////////////////////////////////////
// TOrderBookImpl     Implements IOrderBook, default interface of OrderBook
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.OrderBook
// Description    : Orderr Book Data
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TOrderBookImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TOrderBookImpl, &CLSID_OrderBook>,
  public IDispatchImpl<IOrderBook, &IID_IOrderBook, &LIBID_StarWaveAPI>
{
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	UFC::AnsiString	FTime; ///< HH:MM:ss.mmm
	double			FBuyPx[MAX_DEPTH];
	int				FBuyQty[MAX_DEPTH];
	double			FSellPx[MAX_DEPTH];
	int				FSellQty[MAX_DEPTH];
	bool			FDerivedFlag;
	double			FDerivedBuyPx;
	int				FDerivedBuyQty;
	double			FDerivedSellPx;
	int				FDerivedSellQty;

public:
  TOrderBookImpl()
  {
  }
  void FromOrderBookData( OrderBookData* Data );
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.OrderBook");
  DECLARE_DESCRIPTION("Orderr Book Data");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TOrderBookImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TOrderBookImpl)
  COM_INTERFACE_ENTRY(IOrderBook)
  COM_INTERFACE_ENTRY2(IDispatch, IOrderBook)
END_COM_MAP()

// IOrderBook
public:

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
};

#endif //Unit1H
