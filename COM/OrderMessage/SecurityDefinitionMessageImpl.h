// Unit1.h : Declaration of the TSecurityDefinitionMessageImpl

#ifndef SecurityDefinitionMessageImplH
#define SecurityDefinitionMessageImplH

#define _ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.h"
#include "TSecurityDefinition.h"

/////////////////////////////////////////////////////////////////////////////
// TSecurityDefinitionMessageImpl     Implements ISecurityDefinitionMessage, default interface of SecurityDefinitionMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.SecurityDefinitionMessage
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TSecurityDefinitionMessageImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TSecurityDefinitionMessageImpl, &CLSID_SecurityDefinitionMessage>,
  public IDispatchImpl<ISecurityDefinitionMessage, &IID_ISecurityDefinitionMessage, &LIBID_OrderMessage>
{
private:
	TSecurityDefinitionMessage	FMessage;
public:
  TSecurityDefinitionMessageImpl()
  {
  }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.SecurityDefinitionMessage");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TSecurityDefinitionMessageImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TSecurityDefinitionMessageImpl)
  COM_INTERFACE_ENTRY(ISecurityDefinitionMessage)
  COM_INTERFACE_ENTRY2(IDispatch, ISecurityDefinitionMessage)
END_COM_MAP()

// IMDBaseMessage
public:

  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_Market(BSTR* Value));

// ISecurityDefinitionMessage
public:
 
  STDMETHOD(get_SecurityID(long Index, BSTR* Value));
  STDMETHOD(get_SecurityIDCount(long* Value));
  STDMETHOD(get_MaturityDate(long Index, long* Value));
  STDMETHOD(get_Symbol(long Index, BSTR* Value));
  STDMETHOD(get_ReferencePrice(long Index, double* Value));
  STDMETHOD(get_FallLimitPrice(long Index, double* Value));
  STDMETHOD(get_RiseLimitPrice(long Index, double* Value));
  STDMETHOD(get_StrikePrice(long Index, double* Value));
};

#endif //Unit1H
