// Unit1.h : Declaration of the TMDBaseMessageImpl

#ifndef MDBaseMessageImplH
#define MDBaseMessageImplH

#define _ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.h"
#include "TMDBaseMessage.h"

/////////////////////////////////////////////////////////////////////////////
// TMDBaseMessageImpl     Implements IMDBaseMessage, default interface of MDBaseMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.MDBaseMessage
// Description    :
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TMDBaseMessageImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TMDBaseMessageImpl, &CLSID_MDBaseMessage>,
  public IDispatchImpl<IMDBaseMessage, &IID_IMDBaseMessage, &LIBID_OrderMessage>
{
private:
	TMDBaseMessage	FMessage;
public:
  TMDBaseMessageImpl()
  {
  }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.MDBaseMessage");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TMDBaseMessageImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TMDBaseMessageImpl)
  COM_INTERFACE_ENTRY(IMDBaseMessage)
  COM_INTERFACE_ENTRY2(IDispatch, IMDBaseMessage)
END_COM_MAP()

// IMDBaseMessage
public:
 
  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_Market(BSTR* Value));

};

#endif //Unit1H
