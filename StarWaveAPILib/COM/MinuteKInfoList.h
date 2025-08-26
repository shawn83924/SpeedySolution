// 1.2
// Unit1.h : Declaration of the TMinuteKInfoListImpl

#ifndef MinuteKInfoListH
#define MinuteKInfoListH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "ChartServerConnection.h"

/////////////////////////////////////////////////////////////////////////////
// TMinuteKInfoListImpl     Implements IMinuteKInfoList, default interface of MinuteKInfoList
// ThreadingModel : tmApartment
// Dual Interface : FALSE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.MinuteKInfoList
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TMinuteKInfoListImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TMinuteKInfoListImpl, &CLSID_MinuteKInfoList>,
  public IMinuteKInfoList
{
private:
  UFC::List<TMinuteKInfo>* FKBars;
public:
  TMinuteKInfoListImpl()
  :FKBars( NULL )
  {
  }
  void Ref( UFC::List<TMinuteKInfo>* KBars ) { FKBars = KBars; }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.MinuteKInfoList");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TMinuteKInfoListImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TMinuteKInfoListImpl)
  COM_INTERFACE_ENTRY(IMinuteKInfoList)
END_COM_MAP()

// IMinuteKInfoList
public:

  STDMETHOD(get_Count(long* Value));
  STDMETHOD(get_Items(long Index, IMinuteKInfo** Value));
};

#endif //Unit1H
