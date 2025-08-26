// NETWORIDIMPL.H : Declaration of the TNetworIDImpl

#ifndef NetworIDImplH
#define NetworIDImplH

#define ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.H"


/////////////////////////////////////////////////////////////////////////////
// TNetworIDImpl     Implements INetworID, default interface of NetworID
// ThreadingModel : Apartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.NetworID
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TNetworkIDImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TNetworkIDImpl, &CLSID_NetworkID>,
  public IDispatchImpl<INetworkID, &IID_INetworkID, &LIBID_OrderMessage>
{
public:
  TNetworkIDImpl()
  {
  }

  // Data used when registering Object 
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.NetworkID");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TNetworkIDImpl> 
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TNetworkIDImpl)
  COM_INTERFACE_ENTRY(INetworkID)
  COM_INTERFACE_ENTRY2(IDispatch, INetworkID)
END_COM_MAP()

// INetworID
public:
 
  STDMETHOD(Init(long CID));
};

#endif //NetworIDImplH
