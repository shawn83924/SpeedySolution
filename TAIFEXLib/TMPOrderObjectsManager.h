#ifndef __TMPOrderObjectsManager_H
#define __TMPOrderObjectsManager_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../ExchangeUtility/Utility.h"
#include "../TMP34/QuerySessions.h"
#include "TMPTransactionObject.h"
#include "TMPOrderConnectionObject.h"
#include "../TMP34/ExecReader.h"
#include "../TMP34/ExecWriter.h"
#include "../../inc/odm.h"
#include "../../inc/TMP_stats.h"
#include "../TMP34/MexObject.h"
#include "../TMP34/ExecutionContainer.h"
//------------------------------------------------------------------------------
class TMPOrderManager;
//------------------------------------------------------------------------------
class TreadStatRegInfo
{
public:
    UInt16 FCMID;
    UInt16 FSessionID;
    UInt16 FReqID;
public:    
    TreadStatRegInfo( UInt16 FCM, UInt16 SID, UInt16 ReqID )
    :FCMID( FCM ),FSessionID(SID),FReqID( ReqID ){}
};
//------------------------------------------------------------------------------
class ProductGroup
{
private:
    UFC::PCriticalSection                      FRegCS;
    UFC::PHashedSet<UInt32>                    FRegSet;
    UFC::PHashedList<UInt32,TreadStatRegInfo*> FRegSessionsMap;
    UFC::List<TreadStatRegInfo*>               FRegSessionsList;
private:    
    UInt8 FGroup;
    UInt8 FOpenSwitch;
    Int32 FSeq;
public:
    ProductGroup( UInt8 Group ):FGroup( Group ),FOpenSwitch( 0 ),FSeq( 0 ){}
    void  RegTradingSession( UInt16 FCM, UInt16 Session,UInt16 ReqID );
    BOOL  UnregTradingSession( UInt16 FCM, UInt16 Session );
    void  SetOpenSwitch( UInt8 Val, Int32 Seq  ) { FOpenSwitch = Val; FSeq = Seq; }
    UInt8 GetOpenSwitch( void )      { return FOpenSwitch; }
    UInt8 GetGroup( void )           { return FGroup; }
    Int32 GetOpenSwitchSeq( void )   { return FSeq; }
    Int32 ItemCount( void )          { return FRegSessionsList.ItemCount(); }
    BOOL  GetSession( Int32 Index, UInt16& FCM, UInt16& SID, UInt16& ReqID );
};
//------------------------------------------------------------------------------
class ReportListener : public TransactionListener
{
private:
    UFC::PHashedList<UInt32,ExecWriter*>  FSessionWriters;     
    odm_Rpt*                              FReport;
    ExecWriter*                           FRptWriter;    
    TMPOrderManager*                      FTMPOrderManager;
public:    
    ExecWriter* GetExecWriter( UInt32 FCMID, UInt32 SessionID );    
    virtual void OnTransaction( UInt8 Type, void* Data );
    ReportListener( TMPOrderManager* Manager ):FTMPOrderManager( Manager ){}
    ~ReportListener( void ){}
};
//----------------------------------------------------------------------------------------------------------------------
class TMPOrderManager : public UFC::ThreadListener, public ConnectionListener, public UFC::SocketClientListener, public UFC::PUDPListener
{
private:
	UFC::PEvent             FEvent;
	UFC::PList              FConnectionObjects;
	UFC::PCriticalSection   FCriticalSection;
	int	                FListenPort;
	int			FGroupCount;
	TMPTransactionObject*   FTransactionObject;
        LogManager*             FLog;
        UFC::PClientSocket*     FOdrSocketClient;
        ReportListener*         FExecReportListener;
        ExecReader*             FExecReportReader;
        UFC::PUDPClient*        FUDPClient;
private:
        UFC::AnsiString         FOdrServerIP;
        int                     FOdrServerPort;
        BOOL                    IsODRSConnected;
        BOOL                    FOdrIdleTime;
public:
        UInt16                  FMaxOrderTimeFCM;
        UInt16                  FMaxOrderTimeSessionID;
        UInt32                  FMaxOrderResponseTime;  
        UInt32                  FOrderCount;
        UInt32                  FTotalOrderResponseTime;        
        UInt16                  FMaxQueryTimeFCM;
        UInt16                  FMaxQueryTimeSessionID;
        UInt32                  FMaxQueryResponseTime;  
        UInt32                  FQueryCount;
        UInt32                  FTotalQueryResponseTime;
        BOOL                    FRecovering;
        BOOL                    FBreakTrading;
private:        
        UFC::PHashedList<Int32, ProductGroup*>           FGroups;
private:
        UFC::PCriticalSection                            FRegTableCS;
        UFC::PHashedList<UInt32, ConnectionObjectBase*>  FSessionsTable;
        UFC::List<ConnectionObjectBase*>                 FSessionsArray;
        UFC::PHashedSet<UInt32>                          FRegTable;
private:
        void ConnectToServer( void );
        void GetOdrIPPort( void );
public:
	TMPOrderManager( int LinkNumber, const UFC::AnsiString& OrdIP, int OrdPort, const UFC::AnsiString& DBFileName, const UFC::AnsiString& LogDate );
	virtual ~TMPOrderManager();
	void CreateConnectionObjects( TMPSessions* Sessions );
	void ResetPVC( int BrokerIndex,int PVC );
	int  GetPVCState( int BrokerIndex,int PVC, Int32& Connected, Int32& Bandwidth  );
	void Run();
	void OnOpenSwitch( int Group, int OpenSW, int Seq );
 	void StopConnection( int BrokerIndex,int PVC );	
	// Implement ThreadListener interface.
	virtual void OnTerminate( UFC::PThread* TerminateThread  );
	// Implement ConnectionListener interface.
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);	
public:
        BOOL IsOdrServerConnected( void ) { return IsODRSConnected; }
        void SendRegFCM( ConnectionParameter* cp, BOOL Force = FALSE );
        void SendUnregFCM( ConnectionParameter* cp );
        BOOL IsRegistered( UInt32 FCMID, UInt16 SID );
        ConnectionObjectBase* GetSession(  UInt32 FCMID, UInt16 SID  );
        ConnectionObjectBase* GetSession(  UInt32 SID  );
        void RegFCM( UInt32 FCMID, UInt16 SID );
        void UnregFCM( UInt32 FCMID, UInt16 SID );
public:
        Int16 SubscribeTradeSessionStatus( UInt32 FCMID, UInt16 SID, UInt8 Group, UInt16 ReqID );
        Int16 UnsubscribeTradeSessionStatus( UInt32 FCMID, UInt16 SID, UInt8 Group, UInt16 ReqID );
        Int16 AskTradeSessionStatus( UInt32 FCMID, UInt16 SID, UInt8 Group );
        void  BroadcastTradingStatus( ProductGroup* ProdGroup, UInt8 Group, Int32 ForceOS = -1 );
        void OnExecution( ExecutionSource Src, odm_Rpt* Report );
        void OnQuoreRequestConfirm( MTree* Data );
        void OnFTRequestResponse( MTree* Data );
        void OnTradingBreak( int OS  );
private:
        void HandleLogin( odm_RegConfirm*  Login );
        void HandleRegFCM( odm_FCM_RegConfirm* Reg );
        void HandleUnregFCM( odm_FCM_UnregConfirm* Unreg );
private:        
        virtual void OnDataArrived( UFC::PUDPClient* Socket, UFC::MemoryStream* Stream, const UFC::AnsiString& Address );
public:
        virtual void OnConnect( UFC::PClientSocket * Socket );
        virtual void OnDisconnect( UFC::PClientSocket * Socket );
        virtual BOOL OnDataArrived( UFC::PClientSocket * Socket );
        virtual void OnIdle( UFC::PClientSocket * ) {}
public:
        UInt16 GetProcessID( void ) { return FListenPort%1000; }
        BOOL SendToOdrServer( odm_In* Order );
        void GetStates( TMP34States* States );
};        
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
