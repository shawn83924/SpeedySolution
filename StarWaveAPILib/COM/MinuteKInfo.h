// 1.2
// Unit1.h : Declaration of the TMinuteKInfoImpl

#ifndef MinuteKInfoH
#define MinuteKInfoH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "ChartServerConnection.h"

/////////////////////////////////////////////////////////////////////////////
// TMinuteKInfoImpl     Implements IMinuteKInfo, default interface of MinuteKInfo
// ThreadingModel : tmApartment
// Dual Interface : FALSE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.MinuteKInfo
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TMinuteKInfoImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TMinuteKInfoImpl, &CLSID_MinuteKInfo>,
  public IMinuteKInfo
{
private:
	TMinuteKInfo FInfo;
public:
  TMinuteKInfoImpl()
  {
  }
  void Copy( TMinuteKInfo& NewVal ) {  FInfo = NewVal; }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.MinuteKInfo");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TMinuteKInfoImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TMinuteKInfoImpl)
  COM_INTERFACE_ENTRY(IMinuteKInfo)
END_COM_MAP()

// IMinuteKInfo
public:

  STDMETHOD(get_ClosePx(double* Value));
  STDMETHOD(get_Hour(long* Value));
  STDMETHOD(get_MaxPx(double* Value));
  STDMETHOD(get_MinPx(double* Value));
  STDMETHOD(get_Minutes(long* Value));
  STDMETHOD(get_OpenPx(double* Value));
  STDMETHOD(get_Qty(long* Value));
};

#endif //Unit1H
