/**
 * @file Sigo.h
 *  @brief Header file for Sigo
 *  @author Simon Chang
 *
 */
//----------------------------------------------------------------------------------------------------------------------
#ifndef __SIGO_H
#define __SIGO_H
//----------------------------------------------------------------------------------------------------------------------
#include "../Migo/MBus.h"
#include "../UFC/UFC.h"
//----------------------------------------------------------------------------------------------------------------------
class MTree;
class MessageObject;
//------------------------------------------------------------------------------
typedef enum
{
    rrOK           = 0,
    rrTimeout      = 1,
    rrMaxReq       = 2,
    rrNotConnected = 3
            
} RequestResult;
//----------------------------------------------------------------------------------------------------------------------
// Class MonitorListener
// Interface to monitoring MBus process.
//----------------------------------------------------------------------------------------------------------------------
class MonitorListener
{
public:
	virtual void OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ) = 0;
	virtual void OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ) = 0;
	virtual void OnProcessConnected( BOOL IsTheFirstOne ) = 0;
	virtual void OnProcessList( UFC::PStringList& Processs ) = 0;
	virtual void OnConnected( void ) = 0;
	virtual void OnDisconnected( void ) = 0;
	virtual void OnAppExit( bool& CanExit ) { CanExit = true; };
	virtual ~MonitorListener( void ) {}
};
//----------------------------------------------------------------------------------------------------------------------
class EmptyListener : public MonitorListener
{
public:
	virtual void OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
	virtual void OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
	virtual void OnProcessConnected( BOOL IsTheFirstOne ) {}
	virtual void OnProcessList( UFC::PStringList& Processs ) {}
	virtual void OnConnected( void ) {}
	virtual void OnDisconnected( void ) {}
};
//----------------------------------------------------------------------------------------------------------------------
class AppExitThread : public UFC::PThread
{
private:
    int FMS;
public:
    AppExitThread( int MS );
    void Execute( void );
};
//---------------------------------------------------------------------------
class ResponseData
{
private:    
    UFC::PEvent FRequestEvent;
public:        
    MTree&      Data;
public:    
    ResponseData( MTree& Response ):Data(Response) { }
    BOOL WaitFor( int ms )         { return FRequestEvent.WaitFor( ms/1000, ms%1000 ); }    
    void SetEvent( void )          { FRequestEvent.SetEvent(); }
};  
//---------------------------------------------------------------------------
class ThroughputCtrl
{
private:
    int FReqMax;
    int FReqCount;
    UFC::PCriticalSection FCountLock;
public:
    ThroughputCtrl( int Max )
    :FReqMax( Max )
    ,FReqCount( 0 ) {}
    BOOL Increase( void )
    {
        UFC::PLockObject Lock( FCountLock );
        
        if( FReqCount >= FReqMax )
            return FALSE;
        FReqCount ++;
        return TRUE;
    }
    void Decrease( void )
    {
        UFC::PLockObject Lock( FCountLock );
        
        FReqCount --;
        if( FReqCount < 0 )
            FReqCount = 0;       
    }
    int  GetMaxRequest( void )    { return FReqMax; }
    void SetMaxRequest( int Val ) { FReqMax = Val;  }
};
//---------------------------------------------------------------------------
class ResponseHandler : public MessageListener
{
private:    
    MessageObject* FOwner;    
private: ///< Implement MessageListener
    void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* refMEvent );                    
public:    
    ResponseHandler( MessageObject* Owner ):FOwner( Owner ) {}
};
//---------------------------------------------------------------------------
class SyncMessageHandler : public MessageListener
{
private:    
    MessageObject*          FOwner;        
    RequestMessageListener* FEvent;    
private: ///< Implement MessageListener
    void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* refMEvent );                    
public:    
    SyncMessageHandler( MessageObject* Owner, RequestMessageListener* Listener )
    :FOwner( Owner )
    ,FEvent( Listener ){}
};
//----------------------------------------------------------------------------------------------------------------------
//   Class MessageObject. Warp the Migo objects.
//----------------------------------------------------------------------------------------------------------------------
class MessageObject : public UFC::PThread, MessageListener, MAppListener
{
private:
        EmptyListener                                       FEmptyListener;
        MApp*                                               FApp; ///< Migo Application object.
        UFC::PHashedList<UFC::AnsiString,ProcessStateObj*>  FProcesss;        
        MonitorListener*                                    FListener;
        UFC::AnsiString                                     FAppName; 
private:        
        int                                               FReqSeq; 
        UFC::AnsiString		                          FResponseSubject;
        UFC::PHashMap<UFC::AnsiString,ThroughputCtrl*>    FMaxRequest;  
        UFC::PHashMap<UFC::AnsiString,ResponseData*>      FRequestEvents;      
        UFC::PCriticalSection                             FMapLock;
        UFC::PCriticalSection                             FUIDLock;          
        ResponseHandler                                   FResponseListener;  
        void         AddReqEvent( const UFC::AnsiString& ReqID, ResponseData* );
        void         DeleteReqEvent( const UFC::AnsiString& ReqID );        
        void         UniqueReqID( UFC::AnsiString& ReqID  );
        BOOL         IncreaseRequestCount( const UFC::AnsiString& Subject );
        void         DecreaseRequestCount( const UFC::AnsiString& Subject );
public:        
        void         HandleResponse( const UFC::AnsiString& ReqID, MTree* Data );                 
private:
        void          Execute(); ///< Implement PThred Execute virtual function.
        MSubscriber*  GetSubscriber( const UFC::AnsiString& Subject, const UFC::AnsiString& Key );///< Get MSubscriber object from Migo framework.
private: // Functions for dependency process monitoring.
        void CheckStat( MTree* Data );
        void OnStatChange( BOOL IsOnline, MTree* Data );
        void OnKill( MTree* Data );
private: // Implement MessageListener
        virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* ) ;
private: // Implement MAppListener
        virtual void OnMAppConnected();
        virtual void OnMAppError( PMAppError Error );
        virtual void OnMAppDisconnected();
public:
    /**
     * Constructor. Create a Migo message object.
     *
     * @param AppName: Unique name for this MBus connection.
     * @param Version: Version string.
     * @param Description:
     * @param Port: MBus service port.( Default: MAPP_DEF_PORT )
     */
	MessageObject( UFC::AnsiString AppName, UFC::AnsiString Version,UFC::AnsiString Description, UFCType::Int32 Port = MApp::MAPP_DEF_PORT );
    MessageObject( UFC::AnsiString AppName );
    /**
     * Destructor for Migo Object.
     */
    ~MessageObject( );
    /**
     * Set the MBus Server IP.
     *
     * @param HostIP: IP address.
	 */
    void SetHost( const UFC::AnsiString& HostIP, int Port = MApp::MAPP_DEF_PORT );
    /**
     * Add a MBus message listener.
     *
     * @param Subject: Subscribe subject.("all" for all subjects.)
     * @param Key: Subscribe key.("all" for all keys.)
     * @param Listener: A MessageListener object.
     */
    void AddListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MessageListener* Listener );
    void AddListener( const UFC::AnsiString& Subject, RequestMessageListener* Listener );
    int  SetMaxRequest( const UFC::AnsiString& Subject, int Max );
    int  GetMaxRequest( const UFC::AnsiString& Subject );    
    /**
     * Add a MBus message listener.
     *
     * @param Subject: Message subject.
     * @param Key: Message key.
     * @param Data: A MTree object
     * @param KeepUnsent: Keep the message if MBus service is not available.
     *                    and resend it when MBus service is available.
     */
    BOOL Send( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree &Data, BOOL KeepUnsent = TRUE );
    RequestResult SendRequest( const UFC::AnsiString& Subject,  MTree& Data, MTree& ResponseData, int TimeoutMS );
    /**
     * Whether connected or not.
     * @return TRUE for connected.
     *         FALSE for disconnected.
     */
    BOOL IsConnected( void ) { return FApp->IsConnected();}
    /**
     * Wait for MBus connected.
     */
    void WaitForConnected( int TimeoutSec = 30 );
    /**
     *  Add a process to the dependency process table.
     *
     * @param Host: Application in which host?
     * @param AppName: Which application?
     */
    void AddMonitoringProcess( const UFC::AnsiString& Host, const UFC::AnsiString& AppName );
    /**
     * Set the listener which want to receive the process state changed event.
     *
     * @param Listener: MonitorListener object.
     */
    void SetMonitorListener( MonitorListener* Listener );
    /**
     *  Is the process alive ?
     *
     * @param Host: Application in which host?
     * @param AppName: Which application?
     * @return psStopped: This MBus process was stooped.
     *         psRunning: This MBus process is running.
     *         psNotMonitoring: Not monitoring this process.( Call AddMonitoringProcess first.)
     */
    ProcessState IsAlive( const UFC::AnsiString& Host, const UFC::AnsiString& AppName );
    /**
     *  Ask host MBus server for process list.
     */
    void  SendProcessListRequest( void );
    /**
     *  Get the MApp object instance.
     */
    MApp* GetMApp( void ) { return FApp; }    
    
};
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
