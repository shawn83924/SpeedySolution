// 1.2
// Unit1.h : Declaration of the TUnderlyingIndexImpl

#ifndef UnderlyingIndexH
#define UnderlyingIndexH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
/////////////////////////////////////////////////////////////////////////////
// TUnderlyingIndexImpl     Implements IUnderlyingIndex, default interface of UnderlyingIndex
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.UnderlyingIndex
// Description    : Underlying Index Information
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TUnderlyingIndexImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TUnderlyingIndexImpl, &CLSID_UnderlyingIndex>,
  public IDispatchImpl<IUnderlyingIndex, &IID_IUnderlyingIndex, &LIBID_StarWaveAPI>
{
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FIndexKind;
	double			FIndexValue;
	UFC::AnsiString FIndexTime;
	EnumMarket		FMarket;

public:
  TUnderlyingIndexImpl()
  {
  }
  void FromIndexMessage( UnderlyingIndexInfo* Info );
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.UnderlyingIndex");
  DECLARE_DESCRIPTION("Underlying Index Information");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TUnderlyingIndexImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TUnderlyingIndexImpl)
  COM_INTERFACE_ENTRY(IUnderlyingIndex)
  COM_INTERFACE_ENTRY2(IDispatch, IUnderlyingIndex)
END_COM_MAP()

// IUnderlyingIndex
public:

  STDMETHOD(get_IndexKind(BSTR* Value));
  STDMETHOD(get_IndexTime(BSTR* Value));
  STDMETHOD(get_IndexValue(double* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
};

#endif //Unit1H
