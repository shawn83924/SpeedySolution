//---------------------------------------------------------------------------
#ifndef MessageObjectH
#define MessageObjectH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <MWinApp.h>
#include "MSubscriber.h"
//---------------------------------------------------------------------------
class PThreadMApp : public TThread
{
private:
	virtual void  __fastcall Execute( void );
	MApp*           FApp;
public:
	__fastcall PThreadMApp( MApp* App );
	__fastcall ~PThreadMApp( void );
};
//------------------------------------------------------------------------------
class TMessageObject;
//------------------------------------------------------------------------------
class DoAdminListener : public MessageListener
{
private:
	TMessageObject* FMessageObject;
public:
	void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* refMEvent );
	void SetMessageObject( TMessageObject* Obj ) {FMessageObject = Obj;}
};
//------------------------------------------------------------------------------
typedef void __fastcall (__closure *TMBusEvent)(const MString& Hostname );
typedef void __fastcall (__closure *TProcessEvent)(const MString& Hostname, const MString& AppName );
typedef void __fastcall (__closure *TMBusConnectEvent)( const MString& Hostname, bool IsTheFirstOne );
typedef void __fastcall (__closure *TProcessListEvent)( const MString& Hostname, TStringList* List );
//---------------------------------------------------------------------------
class PACKAGE TMessageObject : public TComponent, public MAppListener
{
friend class TPublisher;
friend class TSubscriber;
friend class DoAdminListener;
private:
	virtual void OnMAppConnected();
	virtual void OnMAppError( PMAppError Error );
	virtual void OnMAppDisconnected();
private:
	MApp*                              FApp;
	PThreadMApp*                       FThreadMApp;
	MSubscriber*                       FSubscriber;
	DoAdminListener					   FDoAdminListener;
	System::UnicodeString              FErrorMessage;
	System::UnicodeString              FAppName;
	System::UnicodeString              FHost;
	UFC::AnsiString                    FConnectedHost;
	int				                   FPort;
	bool                               FIsConnected;
	UFC::PHashedList<UFC::AnsiString,ProcessStateObj*>  FProcesss;
	UFC::PHashedSet<UFC::AnsiString>                    FMonioringSet;
	///< Connection events.
	TNotifyEvent                       FOnConnected;
	TNotifyEvent                       FOnError;
	TNotifyEvent                       FOnDisconnected;
	///< Process events.
	TProcessEvent           		   FOnProcessStartup;
	TProcessEvent           		   FOnProcessStopped;
	TMBusEvent                         FOnMBusStartup;
	TMBusEvent                         FOnMBusStopped;
	TMBusConnectEvent           	   FOnMBusConnected;
	TProcessListEvent      			   FOnProcessList;
	TProcessListEvent      			   FOnMBusList;
private:
	int   __fastcall                   AddSubscriber( MSubscriber* Subscriber );
	void  __fastcall                   DeleteSubscriber( MSubscriber* Subscriber );
	virtual void __fastcall            Loaded( void );
	void __fastcall                    OnAdminMessage(const MString &Subject,const MString &Key, MTree *Tree);
	void __fastcall                    OnStatChange( BOOL IsOnline, MTree* Data );
	void __fastcall                    TriggerMBusListEvent( MTree* Data );
	void __fastcall                    TriggerOnStartup( const UFC::AnsiString& Host, const UFC::AnsiString& App );
	void __fastcall                    TriggerOnStopped( const UFC::AnsiString& Host, const UFC::AnsiString& App );
	void __fastcall                    OnProcessListReceived( MTree* Data );
	void __fastcall                    MakeKeyString( UFC::AnsiString& Key, const UFC::AnsiString& Host, const UFC::AnsiString& App );
	void __fastcall                    OnMBusOnline( const UFC::AnsiString& Host );
	void __fastcall                    OnMBusOffline( const UFC::AnsiString& Host );
	void __fastcall 				   OnAppChanged( BOOL IsOnline, const UFC::AnsiString& Host, const UFC::AnsiString& App );
	void __fastcall                    ClearProcessState( void );
	void __fastcall                    CheckUnrepliedProcess( const UFC::AnsiString& Host );
protected:
	bool __fastcall                    InitializeWinSock( void );
	void __fastcall                    SetHost( const System::UnicodeString Host );
	void __fastcall                    SetAppName( const System::UnicodeString Host );
	void __fastcall                    SetMBusPort( int Port );
	unsigned long long  __fastcall     GetAcuRecvBytes( void );
	unsigned long long  __fastcall     GetAcuRecvMsgs( void );
__published:
	__property System::UnicodeString   AppName           = {read = FAppName, write = SetAppName};
	__property System::UnicodeString   Host              = {read = FHost, write = SetHost};
	__property int                     Port              = {read = FPort, write = SetMBusPort};
	__property TNotifyEvent            OnConnected       = {read = FOnConnected, write = FOnConnected };
	__property TNotifyEvent            OnError           = {read = FOnError, write = FOnError };
	__property TNotifyEvent            OnDisconnected    = {read = FOnDisconnected, write = FOnDisconnected };
	__property TProcessEvent           OnProcessStartup  = {read = FOnProcessStartup, write = FOnProcessStartup };
	__property TProcessEvent           OnProcessStopped  = {read = FOnProcessStopped, write = FOnProcessStopped };
	__property TMBusEvent              OnMBusStartup     = {read = FOnMBusStartup, write = FOnMBusStartup };
	__property TMBusEvent              OnMBusStopped     = {read = FOnMBusStopped, write = FOnMBusStopped };
	__property TMBusConnectEvent       OnMBusConnected   = {read = FOnMBusConnected, write = FOnMBusConnected };
	__property TProcessListEvent       OnProcessList     = {read = FOnProcessList, write = FOnProcessList };
	__property TProcessListEvent       OnMBusList        = {read = FOnMBusList, write = FOnMBusList };
	__property unsigned long long      AcuRecvBytes      = {read = GetAcuRecvBytes };
	__property unsigned long long      AcuRecvMsgs       = {read = GetAcuRecvMsgs };
public:
	/**
	 *   Constructor for TMessageObject.
	 */
	__fastcall                         TMessageObject( TComponent* Owner );
	/**
	 *   Distructor.
	 */
	__fastcall                         ~TMessageObject( void );
	/**
	 *  Connect to a MBus server.
	 */
	void  __fastcall                   Connect( int Timeout  = 10  );
	/**
	 *  Disconnect from the MBus connection.
	 */
	void  __fastcall                   Disconnect( void );
	/**
	 *  Whether connected to MBus or not.
	 */
	bool  __fastcall                   IsConnected( void );
	/**
	 *  Send a MBus message to MBus.
	 */
	bool  __fastcall                   Send( const UFC::AnsiString& Subject,const UFC::AnsiString& Key,MTree* Data, bool KeepUnsend = false );
	/**
	 *  Ask MBus for connected MBus list.
	 */
	void  __fastcall                   RequestMBusList( void );
	/**
	 *  Ask MBus for active process list.
	 */
	void  __fastcall                   RequestProcessList( const UFC::AnsiString& Hostname );
	/**
	 *  Add a process to the monitoring process table.
	 */
	void  __fastcall                   MonitoringProcess( const UFC::AnsiString& Hostname, const UFC::AnsiString& AppName );
	/**
	 *  Is the process alive?
	 */
	ProcessState __fastcall            IsAlive( const UFC::AnsiString& Host, const UFC::AnsiString& AppName );
	/**
	 * Get the last connection error message.
	 */
	const String&  __fastcall          GetErrorMessage( void );
	void __fastcall                    SetRecvDelay( int ms );
};
//---------------------------------------------------------------------------
#endif
