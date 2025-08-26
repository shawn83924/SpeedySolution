// 1.2
// Unit1.h : Declaration of the TTickInfoImpl

#ifndef TickInfoH
#define TickInfoH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "ChartServerConnection.h"

/////////////////////////////////////////////////////////////////////////////
// TTickInfoImpl     Implements ITickInfo, default interface of TickInfo
// ThreadingModel : tmApartment
// Dual Interface : FALSE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.TickInfo
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TTickInfoImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TTickInfoImpl, &CLSID_TickInfo>,
  public ITickInfo
{
private:
	TTickInfo FInfo;
public:
  TTickInfoImpl()
  {
  }
  void Copy( TTickInfo& NewInfo ) { FInfo = NewInfo; }
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.TickInfo");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TTickInfoImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TTickInfoImpl)
  COM_INTERFACE_ENTRY(ITickInfo)
END_COM_MAP()

// ITickInfo
public:

  STDMETHOD(get_BuyPx(double* Value));
  STDMETHOD(get_FillPx(double* Value));
  STDMETHOD(get_FillQty(long* Value));
  STDMETHOD(get_ID(long* Value));
  STDMETHOD(get_SellPx(double* Value));
  STDMETHOD(get_Seq(long* Value));
  STDMETHOD(get_Time(long* Value));
};

#endif //Unit1H
