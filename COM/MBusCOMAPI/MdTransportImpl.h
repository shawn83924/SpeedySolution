// ---------------------------------------------------------------------------
// MdTransportImpl.h : Declaration of the TMdTransportImpl
// ---------------------------------------------------------------------------
#ifndef MdTransportImplH
#define MdTransportImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "MBusAPI_TLB.h"
#include "TThreadApp.h"
#include <MSubscriber.h>
//------------------------------------------------------------------------------
extern void InitSubjectTable( const AnsiString& Vendor );
extern bool CanHandleSubject( const AnsiString& Subject );
class EventHandler;
// ---------------------------------------------------------------------------
// TMdTransportImpl     Implements IMdTransport, default interface of MdTransport
// Events         : Implements IMdTransportEvents, the default source interface
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : TRUE
// Description    : MBus transport object
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TMdTransportImpl : public TCppAutoObjectEvent<IMdTransport>
//									   public MAppListener,
//									   public MessageListener
{
  typedef _COM_CLASS inherited;
  std::vector<IMdTransportEventsDisp*> FEventList;

public:
  __fastcall TMdTransportImpl();
  __fastcall TMdTransportImpl(const System::_di_IInterface Controller);
  __fastcall TMdTransportImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  void __fastcall EventSinkChanged(const System::_di_IInterface EventSink);
private:
	TThreadApp*  FThread;
	MApp*   	 FApp;
	VARIANT      FInstance;
	WideString	 FAppName;
	WideString   FLocalHostName;
	WideString   FLocalIPAddress;
	WideString   FRemoteIP;
	long         FRemotePort;
	MSubscriber* FAdminSubscriber;
	UFC::PHashedList<UFC::AnsiString,ProcessStateObj*>  FProcesss;
	EventHandler* FHandler;
private:
	void StartMAppThread( long TimeoutSec );
	void StopMAppThread();
	void CheckStat( MTree* Data );
	void OnStatChange( BOOL IsOnline, MTree* Data );
public: // Implement MessageListener
	void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* ) ;
public: ///< Implement  MAppListener
	void OnMAppConnected();
	void OnMAppDisconnected();
	void OnMAppError( PMAppError Error );
private:
	HRESULT FireOnConnected(Mbusapi_tlb::IMdTransport* Transport);
	HRESULT FireOnDisconnected(Mbusapi_tlb::IMdTransport* Transport);
	HRESULT FireOnError(Mbusapi_tlb::IMdTransport* Transport, BSTR ErrorMessage);
	HRESULT FireOnProcessConnected(Mbusapi_tlb::IMdTransport* Transport, Mbusapi_tlb::ProcessStatusEnum IsTheFirstOne);
	HRESULT FireOnProcessStartup(Mbusapi_tlb::IMdTransport* Transport, BSTR Host,	BSTR AppName);
	HRESULT FireOnProcessStopped(Mbusapi_tlb::IMdTransport* Transport, BSTR Host,	BSTR AppName);
protected:// IMdTransport
	STDMETHOD(AddMonitoringProcess(BSTR Host, BSTR AppName));
	STDMETHOD(Connect(long Timeout));
	STDMETHOD(Create(BSTR Vendor, BSTR Name, BSTR IP, long Port, VARIANT_BOOL IsGUIApp));

	STDMETHOD(Destroy());
	STDMETHOD(Disconnect());
	STDMETHOD(get_AppName(BSTR* Value));
	STDMETHOD(get_Host(BSTR* Value));
	STDMETHOD(get_LocalHostname(BSTR* Value));
	STDMETHOD(get_LocalIPAddress(BSTR* Value));
	STDMETHOD(get_Port(long* Value));
	STDMETHOD(get_SocketFD(long* Value));
	STDMETHOD(GetTransportInstance(VARIANT* Value));
	STDMETHOD(IsConnected(VARIANT_BOOL* Value));
	STDMETHOD(IsValid(VARIANT_BOOL* Value));
	STDMETHOD(Send(BSTR Subject, BSTR Key, IMdMessage* Msg));
};
// ---------------------------------------------------------------------------
class EventHandler :public  MAppListener,  public MessageListener
{
private:
	TMdTransportImpl* FOwner;
public:
	EventHandler( TMdTransportImpl* Owner ):FOwner( Owner ){}
private: // Implement MessageListener
	virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Msg ){FOwner->OnMigoMessage( Subject, Key, Msg );}
private: ///< Implement  MAppListener
	virtual void OnMAppConnected()               { FOwner->OnMAppConnected();}
	virtual void OnMAppDisconnected()            { FOwner->OnMAppDisconnected();}
	virtual void OnMAppError( PMAppError Error ) { FOwner->OnMAppError( Error );}
};
// ---------------------------------------------------------------------------
#endif //MdTransportImplH
