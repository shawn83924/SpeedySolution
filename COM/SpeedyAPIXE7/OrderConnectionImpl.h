// ---------------------------------------------------------------------------
// OrderConnectionImpl.h : Declaration of the TOrderConnectionImpl
// ---------------------------------------------------------------------------
#ifndef OrderConnectionImplH
#define OrderConnectionImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "SpeedyAPI_TLB.h"
#include "ExecutionReportMessageImpl.h"
#include "NewsMessageImpl.h"
#include <TTaifexConnection.h>

// ---------------------------------------------------------------------------
class EventHandler;
// ---------------------------------------------------------------------------
// TOrderConnectionImpl     Implements IOrderConnection, default interface of OrderConnection
// Events         : Implements IOrderConnectionEvents, the default source interface
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : TRUE
// Description    : Object to establish order connection
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TOrderConnectionImpl : public TCppAutoObjectEvent<IOrderConnection>
{
  typedef _COM_CLASS inherited;
  typedef std::vector<IOrderConnectionEventsDisp*> TOCEventList;
private:
	TOCEventList                 FEventList;
	EventHandler*                FEventHandler;
	TTaifexConnection*           FConnection;
private:
	int GetVersion( bool IsProxy );
	void PrintCaption( bool detail );
public:
	HRESULT __fastcall FireOnConnected();
	HRESULT __fastcall FireOnDisconnected();
	HRESULT __fastcall FireOnExecutionReport(IExecutionReportMessage* Msg ,  Speedyapi_tlb::ExecDupEnum ExecPossDup );
	HRESULT __fastcall FireOnRecoverFinished( long Count );
	HRESULT __fastcall FireOnNews(INewsMessage* News );
	HRESULT __fastcall FireOnCancelWorking( Speedyapi_tlb::CancelWorkingResultEnum  Result, long Count  );
	HRESULT __fastcall FireOnChangePasswordReply( Speedyapi_tlb::ChangePasswordEnum Result, BSTR Message );
	HRESULT __fastcall FireOnLogonReply(BSTR Message,Speedyapi_tlb::LogonResultEnum Result, long ConnectionID);
public:
  __fastcall TOrderConnectionImpl();
  __fastcall TOrderConnectionImpl(const System::_di_IInterface Controller);
  __fastcall TOrderConnectionImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  void __fastcall EventSinkChanged(const System::_di_IInterface EventSink);
protected:// IOrderConnection
  STDMETHOD(AddSellSide(SellSideTypeEnum SellSide));
  STDMETHOD(CancelOrder(ICancelOrderMessage* Msg));
  STDMETHOD(Connect(BSTR RemoteIP, long RemotePort));
  STDMETHOD(Connect2(BSTR RemoteIP, long RemotePort, long TimeoutSec));
  STDMETHOD(Create(BSTR AppName));
  STDMETHOD(Create2(BSTR AppName));
  STDMETHOD(Create3(BSTR AppName, long QueueSize));
  STDMETHOD(Destroy());
  STDMETHOD(Disconnect());
  STDMETHOD(GenerateUniqueID(MarketEnum Market, MessageTypeEnum Type, __int64* NID));





  STDMETHOD(get_BrokerID(BSTR* Value));
  STDMETHOD(get_ClearMemberID(BSTR* Value));
  STDMETHOD(get_EnablePandingNewAck(VARIANT_BOOL* Value));
  STDMETHOD(get_IsAdministrator(VARIANT_BOOL* Value));
  STDMETHOD(get_UseNewFuturesSymbol(VARIANT_BOOL* Value));
  STDMETHOD(get_Version(BSTR* Value));
  STDMETHOD(get_X25StylePxQty(VARIANT_BOOL* Value));
  STDMETHOD(GetBrokerID(MarketEnum Market, BSTR* BrokerID));
  STDMETHOD(LockAE(BSTR AE, VARIANT_BOOL IsLock));
  STDMETHOD(LockThisAE(VARIANT_BOOL Locked));
  STDMETHOD(Logon(BSTR ID, BSTR PASSWD, BSTR ACCOUNT, ConnectionTypeEnum Type));
  STDMETHOD(LogonProxy(BSTR ID, BSTR Password, BSTR Account, BSTR Token1, BSTR Token2));





  STDMETHOD(NewOrder(INewOrderMessage* Msg));
  STDMETHOD(NewsRequest(INewsMessage* NewsReq));
  STDMETHOD(OrderStatusRequest(IOrderStatusRequestMessage* Msg));
  STDMETHOD(Quote(IQuoteMessage* Msg));
  STDMETHOD(QuoteCancel(IQuoteCancelMessage* Msg));
  STDMETHOD(QuoteRequest(IQuoteRequestMessage* Msg));
  STDMETHOD(Recover(BSTR BeginTime, RecoverTypeEnum Type, RecoverMarketEnum Market));





  STDMETHOD(Recover2(BSTR BeginTime, BSTR EndTime, RecoverTypeEnum Type, RecoverMarketEnum Market));





  STDMETHOD(RemoveSellSide(SellSideTypeEnum SellSide));
  STDMETHOD(ReplaceOrder(IReplaceOrderMessage* Msg));
  STDMETHOD(set_BrokerID(BSTR Value));
  STDMETHOD(set_ClearMemberID(BSTR Value));
  STDMETHOD(set_DetectAPI(VARIANT_BOOL Value));
  STDMETHOD(set_EnableFilledStatus(VARIANT_BOOL Value));
  STDMETHOD(set_EnablePandingNewAck(VARIANT_BOOL Value));
  STDMETHOD(set_SendFillZeroQty(VARIANT_BOOL Param1));
  STDMETHOD(set_Stdout(VARIANT_BOOL Value));
  STDMETHOD(set_UseNewFuturesSymbol(VARIANT_BOOL Value));
  STDMETHOD(set_X25StylePxQty(VARIANT_BOOL Value));
  STDMETHOD(SetBrokerID(MarketEnum Market, BSTR Value));
  STDMETHOD(SetDebugLog(BSTR FileName));
  STDMETHOD(SetLanguage(MessageLanguageEnum Lang));
  STDMETHOD(URLEncode(BSTR URL, BSTR* EncodeURL));
  STDMETHOD(get_AlwaysGenerateNID(VARIANT_BOOL* Value));
  STDMETHOD(set_AlwaysGenerateNID(VARIANT_BOOL Value));
  STDMETHOD(get_BuildDate(BSTR* Value));
  STDMETHOD(SupportAPI(APIEnum Value, VARIANT_BOOL* Rtn));
  STDMETHOD(IsTAIFEXFutures(BSTR Symbol, VARIANT_BOOL* Result));
  STDMETHOD(IsTAIFEXOptions(BSTR Symbol, VARIANT_BOOL* Result));
  STDMETHOD(CancelWorking(CancelMarketEnum Market, BSTR AE, BSTR Account, BSTR UserData,
          BSTR ProdID));
  STDMETHOD(Recover3(BSTR Date, BSTR BeginTime, RecoverTypeEnum Type, RecoverMarketEnum Market,
          RecoverSessionEnum Session));
  STDMETHOD(get_CADLLFileName(BSTR* Value));
  STDMETHOD(get_CAPassword(BSTR* Value));
  STDMETHOD(get_CAPFXFilePathName(BSTR* Value));
  STDMETHOD(set_CADLLFileName(BSTR Value));
  STDMETHOD(set_CAPassword(BSTR Value));
  STDMETHOD(set_CAPFXFilePathName(BSTR Value));
  STDMETHOD(CreateCAObject());
  STDMETHOD(get_CAOrganizationalUnit(BSTR* Value));
  STDMETHOD(set_CAOrganizationalUnit(BSTR Value));
  STDMETHOD(get_CACommonName(BSTR* Value));
  STDMETHOD(set_CACommonName(BSTR Value));
  STDMETHOD(DeleteCAObject());
  STDMETHOD(CreateMT(BSTR AppName));
  STDMETHOD(NewOrderDirect(MarketEnum Market, __int64 NID, BSTR Order, BSTR Data, long Group));




  STDMETHOD(SetDebugLogEx(BSTR FileName));
  STDMETHOD(get_LocalIP(BSTR* Value));
  STDMETHOD(ChangePassword(BSTR Password, BSTR NewPassword));

  STDMETHOD(GetMarginPosition(BSTR BrokerID, BSTR Account, BSTR* Value, VARIANT_BOOL* SUCCEED));



  STDMETHOD(TouchOrderControl(ITouchOrderCommand* TouchOrderCmd, VARIANT_BOOL* Result));

};
// ---------------------------------------------------------------------------
class EventHandler :public  IOrderConnectionEventListener
{
private:
	TOrderConnectionImpl* FOwner;
public:
	EventHandler( TOrderConnectionImpl* Owner );
public:
	virtual void OnConnected( void );
	virtual void OnDisconnected( void );
	virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
	virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
	virtual void OnRecoverFinished( int Count );
	virtual void OnCancelWorking( CancelWorkingResult Result, int Count );
	virtual void OnChangePassword( ChangePwdResult Result );
	virtual void OnNews( TNewsMessage* Msg );
};
// ---------------------------------------------------------------------------
#endif //OrderConnectionImplH

