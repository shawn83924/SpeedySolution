// 1.2
// Unit1.h : Declaration of the TCloseMarketDataImpl

#ifndef CloseMarketDataH
#define CloseMarketDataH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
/////////////////////////////////////////////////////////////////////////////
// TCloseMarketDataImpl     Implements ICloseMarketData, default interface of CloseMarketData
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.CloseMarketData
// Description    : Closing Market Data
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TCloseMarketDataImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TCloseMarketDataImpl, &CLSID_CloseMarketData>,
  public IDispatchImpl<ICloseMarketData, &IID_ICloseMarketData, &LIBID_StarWaveAPI>
{
private:
	UFC::AnsiString			FExchange;
	UFC::AnsiString			FSymbol;
	EnumMarket				FMarket;
	double					FTermHighPx;
	double					FTermLowPx;
	double					FDayHighPx;
	double					FDayLowPx;
	double					FOpenPx;
	double					FBuyPx;
	double					FSellPx;
	double					FClosePx;
	int						FBuyCountTotal;
	int						FBuyQtyTotal;
	int						FSellCountTotal;
	int						FSellQtyTotal;
	int						FTotalCount;
	int						FTotalQty;
	double					FSettlementPx;
	int						FOpenInterest;

public:
  TCloseMarketDataImpl()
  {
  }
	void FromI070( ClosingMarketData* Data );
	void FromI071( ClosingMarketDataWithSettlementPrice* Data );
	void FromI072( ClosingMarketDataWithSettlementPriceAndOpenInterest* Data );
	void FromI073( ClosingMarketData* Data ){}
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.CloseMarketData");
  DECLARE_DESCRIPTION("Closing Market Data");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TCloseMarketDataImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TCloseMarketDataImpl)
  COM_INTERFACE_ENTRY(ICloseMarketData)
  COM_INTERFACE_ENTRY2(IDispatch, ICloseMarketData)
END_COM_MAP()

// ICloseMarketData
public:

  STDMETHOD(get_BuyCountTotal(long* Value));
  STDMETHOD(get_BuyPx(double* Value));
  STDMETHOD(get_BuyQtyTotal(long* Value));
  STDMETHOD(get_ClosePx(double* Value));
  STDMETHOD(get_DayHighPx(double* Value));
  STDMETHOD(get_DayLowPx(double* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_OpenInterest(long* Value));
  STDMETHOD(get_OpenPx(double* Value));
  STDMETHOD(get_SellCountTotal(long* Value));
  STDMETHOD(get_SellPx(double* Value));
  STDMETHOD(get_SellQtyTotal(long* Value));
  STDMETHOD(get_SettlementPx(double* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_TermHighPx(double* Value));
  STDMETHOD(get_TermLowPx(double* Value));
  STDMETHOD(get_TotalCount(long* Value));
  STDMETHOD(get_TotalQty(long* Value));
private:
	void FillClosingData( ClosingMarketData* Data );
};

#endif //Unit1H
