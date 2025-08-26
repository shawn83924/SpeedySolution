// 1.2
// Unit1.h : Declaration of the TOpenInfoImpl

#ifndef OpeningInfoH
#define OpeningInfoH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
/////////////////////////////////////////////////////////////////////////////
// TOpenInfoImpl     Implements IOpenInfo, default interface of OpenInfo
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.OpenInfo
// Description    : Opening Price and Opening Qty
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TOpenInfoImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TOpenInfoImpl, &CLSID_OpenInfo>,
  public IDispatchImpl<IOpenInfo, &IID_IOpenInfo, &LIBID_StarWaveAPI>
{
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	double			FOpenPx;
	int				FOpenQty;
	UFC::AnsiString FTime;

public:
  TOpenInfoImpl()
  {
  }
  void FromOpeningInfo( OpeningInfo* Info );
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.OpenInfo");
  DECLARE_DESCRIPTION("Opening Price and Opening Qty");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TOpenInfoImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TOpenInfoImpl)
  COM_INTERFACE_ENTRY(IOpenInfo)
  COM_INTERFACE_ENTRY2(IDispatch, IOpenInfo)
END_COM_MAP()

// IOpenInfo
public:

  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_OpeningPx(double* Value));
  STDMETHOD(get_OpeningQty(long* Value));
  STDMETHOD(get_OpeningTime(BSTR* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
};

#endif //Unit1H
