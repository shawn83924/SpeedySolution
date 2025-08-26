// 1.2
// Unit1.h : Declaration of the TTickInfoListImpl

#ifndef TickInfoListH
#define TickInfoListH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "ChartServerConnection.h"

/////////////////////////////////////////////////////////////////////////////
// TTickInfoListImpl     Implements ITickInfoList, default interface of TickInfoList
// ThreadingModel : tmApartment
// Dual Interface : FALSE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.TickInfoList
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TTickInfoListImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TTickInfoListImpl, &CLSID_TickInfoList>,
  public ITickInfoList
{
private:
  UFC::List<TTickInfo>* FTicks;
public:
  TTickInfoListImpl()
  :FTicks( NULL )
  {
  }
  void Ref( UFC::List<TTickInfo>* Ticks ) { FTicks = Ticks; }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.TickInfoList");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TTickInfoListImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TTickInfoListImpl)
  COM_INTERFACE_ENTRY(ITickInfoList)
END_COM_MAP()

// ITickInfoList
public:

  STDMETHOD(get_Count(long* Value));
  STDMETHOD(get_Items(long Index, ITickInfo** Value));
};

#endif //Unit1H
