// 1.2
// Unit1.h : Declaration of the TNewsImpl

#ifndef NewsH
#define NewsH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
/////////////////////////////////////////////////////////////////////////////
// TNewsImpl     Implements INews, default interface of News
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.News
// Description    : News Message
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TNewsImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TNewsImpl, &CLSID_News>,
  public IDispatchImpl<INews, &IID_INews, &LIBID_StarWaveAPI>
{
private:
	int FKey;
	UFC::AnsiString FGroup;
	UFC::AnsiString FData;
public:
  TNewsImpl()
  :FKey( 0 )
  {
  }
  void FromNews( const UFC::AnsiString& Group,const UFC::AnsiString& Msg );
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.News");
  DECLARE_DESCRIPTION("News Message");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TNewsImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TNewsImpl)
  COM_INTERFACE_ENTRY(INews)
  COM_INTERFACE_ENTRY2(IDispatch, INews)
END_COM_MAP()

// INews
public:

  STDMETHOD(get_Data(BSTR* Value));
  STDMETHOD(get_Key(long* Value));
  STDMETHOD(get_Group(BSTR* Value));
};

#endif //Unit1H
