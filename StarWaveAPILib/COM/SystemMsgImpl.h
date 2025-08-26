// ---------------------------------------------------------------------------
// SystemMsgImpl.h : Declaration of the TSystemMsgImpl
// ---------------------------------------------------------------------------
#ifndef SystemMsgImplH
#define SystemMsgImplH


#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
/////////////////////////////////////////////////////////////////////////////
// TSystemMsgImpl     Implements ISystemMsg, default interface of SystemMsg
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TSystemMsgImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TSystemMsgImpl, &CLSID_SystemMsg>,
  public IDispatchImpl<ISystemMsg, &IID_ISystemMsg, &LIBID_StarWaveAPI>
{
private:
	UFC::AnsiString FExchange;
	int 			FCode;
	UFC::AnsiString FData;
	EnumMarket		FMarket;
public:
  TSystemMsgImpl()
  {
  }
  void FromMarketDataMessage( SystemMessage* Msg );
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.SystemMsg");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TSystemMsgImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TSystemMsgImpl)
  COM_INTERFACE_ENTRY(ISystemMsg)
  COM_INTERFACE_ENTRY2(IDispatch, ISystemMsg)
END_COM_MAP()

// ISystemMsg
public:
  STDMETHOD(get_Code(long* Value));
  STDMETHOD(get_Data(BSTR* Value));

  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
};


#endif //SystemMsgImplH
