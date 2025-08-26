// Unit1.h : Declaration of the TNewsMessageImpl

#ifndef NewsMessageImplH
#define NewsMessageImplH

#define _ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.h"
#include "TNewsMessage.h"

/////////////////////////////////////////////////////////////////////////////
// TNewsMessageImpl     Implements INewsMessage, default interface of NewsMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.NewsMessage
// Description    :
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TNewsMessageImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TNewsMessageImpl, &CLSID_NewsMessage>,
  public IDispatchImpl<INewsMessage, &IID_INewsMessage, &LIBID_OrderMessage>
{
private:
	TNewsMessage	FMessage;
public:
  TNewsMessageImpl()
  {
  }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.NewsMessage");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TNewsMessageImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TNewsMessageImpl)
  COM_INTERFACE_ENTRY(INewsMessage)
  COM_INTERFACE_ENTRY2(IDispatch, INewsMessage)
END_COM_MAP()

// IMDBaseMessage
public:

  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_Market(BSTR* Value));



// INewsMessage
public:
 
  STDMETHOD(get_Headline(BSTR* Value));
  STDMETHOD(get_Text(BSTR* Value));
  STDMETHOD(get_ID(long* Value));
  STDMETHOD(set_ID(long Value));
  STDMETHOD(set_Headline(BSTR Value));
  STDMETHOD(set_Text(BSTR Value));
  STDMETHOD(get_MsgCount(long* Value));
  STDMETHOD(get_MsgNum(long* Value));
};

#endif //Unit1H
