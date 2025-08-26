// TAIFEXCONNECTIONIMPL.H : Declaration of the TTaifexConnectionImpl

#ifndef TaifexConnectionImplH
#define TaifexConnectionImplH

#define ATL_APARTMENT_THREADED

#include "OrderConnection_TLB.H"
#include <TTaifexConnection.h>

/////////////////////////////////////////////////////////////////////////////
// TTaifexConnectionImpl     Implements ITaifexConnection, default interface of TaifexConnection
//                  and ITaifexConnectionEvents, the default source interface
// ThreadingModel : Apartment
// Dual Interface : TRUE
// Event Support  : TRUE
// Default ProgID : OrderConnection.TaifexConnection
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TTaifexConnectionImpl : 
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TTaifexConnectionImpl, &CLSID_TaifexConnection>,
  public IConnectionPointContainerImpl<TTaifexConnectionImpl>,
  public TEvents_TaifexConnection<TTaifexConnectionImpl>,
  public IDispatchImpl<ITaifexConnection, &IID_ITaifexConnection, &LIBID_OrderConnection>,
  public IOrderConnectionEventListener
{
private:
	TTaifexConnection* FConnection;
	int GetVersion( const char* Binary, bool IsProxy );
public:
	virtual void OnConnected( void );
	virtual void OnDisconnected( void );
	virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
	virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup ExecPossDup );
	virtual void OnNews( TNewsMessage* Msg );
	virtual void OnRecoverFinished( int Count );
public:
  TTaifexConnectionImpl();

  // Data used when registering Object 
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderConnection.TaifexConnection");
  DECLARE_DESCRIPTION("Order Connection");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
	TTypedComServerRegistrarT<TTaifexConnectionImpl>
	regObj(GetObjectCLSID(), GetProgID(), GetDescription());
	return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TTaifexConnectionImpl)
  COM_INTERFACE_ENTRY(ITaifexConnection)
  COM_INTERFACE_ENTRY2(IDispatch, ITaifexConnection)
  COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(TTaifexConnectionImpl)
  CONNECTION_POINT_ENTRY(DIID_ITaifexConnectionEvents)
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
  STDMETHOD(get_IsAdministrator(VARIANT_BOOL* Value));
  STDMETHOD(set_ClearMemberID(BSTR Value));
  STDMETHOD(SetLanguage(MessageLanguageEnum Lang));
  STDMETHOD(GenerateUniqueID(MarketEnum Market, MessageTypeEnum Type, __int64* NID));















  STDMETHOD(GetBrokerID(MarketEnum Market, BSTR* BrokerID));
  STDMETHOD(SetBrokerID(MarketEnum Market, BSTR Value));
  STDMETHOD(Create2(BSTR AppName));
  STDMETHOD(get_BrokerID(BSTR* Value));
  STDMETHOD(set_BrokerID(BSTR Value));
  STDMETHOD(get_UseNewFuturesSymbol(VARIANT_BOOL* Value));
  STDMETHOD(set_UseNewFuturesSymbol(VARIANT_BOOL Value));
  STDMETHOD(SetDebugLog(BSTR FileName));
  STDMETHOD(OrderStatusRequest(IOrderStatusRequestMessage* Msg));
  STDMETHOD(Recover2(BSTR BeginTime, BSTR EndTime, RecoverTypeEnum Type, RecoverMarketEnum Market));


















  STDMETHOD(set_SendFillZeroQty(VARIANT_BOOL Param1));
  STDMETHOD(NewsRequest(INewsMessage* NewsReq));
  STDMETHOD(LockThisAE(VARIANT_BOOL Locked));
  STDMETHOD(set_EnableFilledStatus(VARIANT_BOOL Value));
  STDMETHOD(get_EnablePandingNewAck(VARIANT_BOOL* Value));
  STDMETHOD(set_EnablePandingNewAck(VARIANT_BOOL Value));
  STDMETHOD(AddSellSide(SellSideTypeEnum SellSide));
  STDMETHOD(RemoveSellSide(SellSideTypeEnum SellSide));
  STDMETHOD(get_X25StylePxQty(VARIANT_BOOL* Value));
  STDMETHOD(set_X25StylePxQty(VARIANT_BOOL Value));
  STDMETHOD(set_Stdout(VARIANT_BOOL Value));
  STDMETHOD(LogonProxy(BSTR ID, BSTR Password, BSTR Account, BSTR Token1, BSTR Token2));


















  STDMETHOD(LockAE(BSTR AE, VARIANT_BOOL IsLock));
  STDMETHOD(Connect2(BSTR RemoteIP, long RemotePort, long TimeoutSec));
  STDMETHOD(URLEncode(BSTR URL, BSTR* EncodeURL));
  STDMETHOD(get_Version(BSTR* Value));
  STDMETHOD(get_WhichAPI(APIEnum* Value));
  STDMETHOD(set_DetectAPI(VARIANT_BOOL Value));
  STDMETHOD(Create3(BSTR AppName, APIEnum UseAPI, BSTR* Message, VARIANT_BOOL* Result));









};

#endif //TaifexConnectionImplH
