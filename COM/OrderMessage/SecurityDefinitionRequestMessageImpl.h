// Unit1.h : Declaration of the TSecurityDefinitionRequestMessageImpl

#ifndef SecurityDefinitionRequestMessageImplH
#define SecurityDefinitionRequestMessageImplH

#define _ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.h"
#include "TsecurityDefinitionRequest.h"

/////////////////////////////////////////////////////////////////////////////
// TSecurityDefinitionRequestMessageImpl     Implements ISecurityDefinitionRequestMessage, default interface of SecurityDefinitionRequestMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.SecurityDefinitionRequestMessage
// Description    :
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TSecurityDefinitionRequestMessageImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TSecurityDefinitionRequestMessageImpl, &CLSID_SecurityDefinitionRequestMessage>,
  public IDispatchImpl<ISecurityDefinitionRequestMessage, &IID_ISecurityDefinitionRequestMessage, &LIBID_OrderMessage>
{
private:
	TSecurityDefinitionRequestMessage FMessage;
public:
  TSecurityDefinitionRequestMessageImpl()
  {
  }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.SecurityDefinitionRequestMessage");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TSecurityDefinitionRequestMessageImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TSecurityDefinitionRequestMessageImpl)
  COM_INTERFACE_ENTRY(ISecurityDefinitionRequestMessage)
  COM_INTERFACE_ENTRY2(IDispatch, ISecurityDefinitionRequestMessage)
END_COM_MAP()

// IMDBaseMessage
public:

  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_Market(BSTR* Value));

// ISecurityDefinitionRequestMessage
  STDMETHOD(set_RequestMarket(BSTR Param1));
public:

};

#endif //Unit1H
