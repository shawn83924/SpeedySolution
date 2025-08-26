// Unit1.h : Declaration of the TMarketDataConnectionImpl

#ifndef MarketDataConnectionImplH
#define MarketDataConnectionImplH

#define _ATL_APARTMENT_THREADED

#include "MarketDataConnection_TLB.h"
#include "TMarketDataConnection.h"

/////////////////////////////////////////////////////////////////////////////
// TMarketDataConnectionImpl     Implements IMarketDataConnection, default interface of MarketDataConnection
//                  and IMarketDataConnectionEvents, the default source interface
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : TRUE
// Default ProgID : OrderConnection.MarketDataConnection
// Description    :
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TMarketDataConnectionImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TMarketDataConnectionImpl, &CLSID_MarketDataConnection_>,
  public IConnectionPointContainerImpl<TMarketDataConnectionImpl>,
  public TEvents_MarketDataConnection_<TMarketDataConnectionImpl>,
  public IDispatchImpl<IMarketDataConnection, &IID_IMarketDataConnection, &LIBID_MarketDataConnection>,
  public TMarketDataConnectionEventListener
{
private:
	TMarketDataConnection*	FConnection;
public:
	virtual void OnConnected( void );
	virtual void OnDisconnected( void );
	virtual void OnSecurityDefinition( TSecurityDefinitionMessage* Msg ) ;
	virtual void OnNews( TNewsMessage* Msg );
	virtual void OnMarketDataMessage( TMarketDataMessage* Msg );
	virtual void OnQuoteRequest( TMDQuoteRequestMessage* Msg ) ;
public:
  TMarketDataConnectionImpl();

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("MarketDataConnection.MarketDataConnection");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
	TTypedComServerRegistrarT<TMarketDataConnectionImpl>
	regObj(GetObjectCLSID(), GetProgID(), GetDescription());
	return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TMarketDataConnectionImpl)
  COM_INTERFACE_ENTRY(IMarketDataConnection)
  COM_INTERFACE_ENTRY2(IDispatch, IMarketDataConnection)
  COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(TMarketDataConnectionImpl)
  CONNECTION_POINT_ENTRY(DIID_IMarketDataConnectionEvents)
END_CONNECTION_POINT_MAP()

// IMarketDataConnection
public:

  STDMETHOD(Connect(BSTR RemoteIP, long RemotePort));
  STDMETHOD(Create(BSTR AppName));
  STDMETHOD(Destroy());
  STDMETHOD(Disconnect());
  STDMETHOD(Subscribe(BSTR Market, BSTR Symbol));
  STDMETHOD(Unsubscribe(BSTR Market, BSTR Symbol));
  STDMETHOD(SecurityDefinitionRequest(ISecurityDefinitionRequestMessage* Msg));
};

#endif //Unit1H
