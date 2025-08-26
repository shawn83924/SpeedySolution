// TAIFEXCONNECTIONIMPL.H : Declaration of the TTaifexConnectionImpl

#ifndef TaifexTMPConnectionImplH
#define TaifexTMPConnectionImplH

#define ATL_APARTMENT_THREADED

#include "TMPOrderConnection_TLB.h"
#include <TTaifexTMPConnection.h>

/////////////////////////////////////////////////////////////////////////////
// TTaifexConnectionImpl     Implements ITaifexConnection, default interface of TaifexConnection
//                  and ITaifexConnectionEvents, the default source interface
// ThreadingModel : Apartment
// Dual Interface : TRUE
// Event Support  : TRUE
// Default ProgID : OrderConnection.TaifexConnection
// Description    :
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TTaifexTMPConnectionImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TTaifexTMPConnectionImpl, &CLSID_TaifexTMPConnection>,
  public IConnectionPointContainerImpl<TTaifexTMPConnectionImpl>,
  public TEvents_TaifexTMPConnection<TTaifexTMPConnectionImpl>,
  public IDispatchImpl<ITaifexTMPConnection, &IID_ITaifexTMPConnection, &LIBID_TMPOrderConnection>,
  public IOrderConnectionEventListener
{
private:
	TTaifexTMPConnection* FConnection;
public:
	virtual void OnConnected( void );
	virtual void OnDisconnected( void );
	virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
	virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
	virtual void OnRecoverFinished( int Count );
public:
  TTaifexTMPConnectionImpl();

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderConnection.TaifexConnection");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
	TTypedComServerRegistrarT<TTaifexTMPConnectionImpl>
	regObj(GetObjectCLSID(), GetProgID(), GetDescription());
	return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TTaifexTMPConnectionImpl)
  COM_INTERFACE_ENTRY(ITaifexTMPConnection)
  COM_INTERFACE_ENTRY2(IDispatch, ITaifexTMPConnection)
  COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(TTaifexTMPConnectionImpl)
  CONNECTION_POINT_ENTRY(DIID_ITaifexTMPConnectionEvents)
END_CONNECTION_POINT_MAP()

// ITaifexConnection
public:
  STDMETHOD(Create(BSTR AppName));
  STDMETHOD(Destroy());
  STDMETHOD(Connect(BSTR RemoteIP, long RemotePort));
  STDMETHOD(Disconnect());
  STDMETHOD(Logon(BSTR ID, BSTR PASSWD, BSTR ACCOUNT, ConnectionTypeEnum Type));
  STDMETHOD(NewOrder(INewOrderMessage* Msg));
  STDMETHOD(ReplaceOrder(IReplaceOrderMessage* Msg));
  STDMETHOD(CancelOrder(ICancelOrderMessage* Msg));
  STDMETHOD(Quote(IQuoteMessage* Msg));
  STDMETHOD(QuoteCancel(IQuoteCancelMessage* Msg));
  STDMETHOD(QuoteRequest(IQuoteRequestMessage* Msg));
  STDMETHOD(Recover(BSTR BeginTime, RecoverTypeEnum Type, RecoverMarketEnum Market));









  STDMETHOD(get_ClearMemberID(BSTR* Value));
  STDMETHOD(set_ClearMemberID(BSTR Value));
  STDMETHOD(get_IsAdministrator(VARIANT_BOOL* Value));
  STDMETHOD(SetLanguage(MessageLanguageEnum Lang));
  STDMETHOD(GenerateUniqueID(MessageTypeEnum Type, long* NID));
  STDMETHOD(get_BrokerID(BSTR* Value));
  STDMETHOD(set_BrokerID(BSTR Value));
  STDMETHOD(SetDebugLog(BSTR FileName));
  STDMETHOD(OrderStatusRequest(IOrderStatusRequestMessage* Msg));
  STDMETHOD(Recover2(BSTR BeginTime, BSTR EndTime, RecoverTypeEnum Type, RecoverMarketEnum Market));









};
#endif //TaifexConnectionImplH
