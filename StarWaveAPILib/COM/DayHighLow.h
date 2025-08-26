// 1.2
// Unit1.h : Declaration of the TDayHighLowImpl

#ifndef DayHighLowH
#define DayHighLowH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
/////////////////////////////////////////////////////////////////////////////
// TDayHighLowImpl     Implements IDayHighLow, default interface of DayHighLow
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.DayHighLow
// Description    :
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TDayHighLowImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TDayHighLowImpl, &CLSID_DayHighLow>,
  public IDispatchImpl<IDayHighLow, &IID_IDayHighLow, &LIBID_StarWaveAPI>
{
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	double			FDayHighPx;
	double			FDayLowPx;
public:
  TDayHighLowImpl()
  {
  }
  void FromDayHighLow( DayHighLowPrice* DayHighLow );
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.DayHighLow");
  DECLARE_DESCRIPTION("Day high and day low message.");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TDayHighLowImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TDayHighLowImpl)
  COM_INTERFACE_ENTRY(IDayHighLow)
  COM_INTERFACE_ENTRY2(IDispatch, IDayHighLow)
END_COM_MAP()

// IDayHighLow
public:

  STDMETHOD(get_DayHighPx(double* Value));
  STDMETHOD(get_DayLowPx(double* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
};

#endif //Unit1H
