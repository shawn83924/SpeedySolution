//------------------------------------------------------------------------------------------------------------------------
#ifndef __RCRouting_H
#define __RCRouting_H
//------------------------------------------------------------------------------------------------------------------------
#include "RCRoutingNodeRemote.h"
#include "RCRoutingNodeLocal.h"
//------------------------------------------------------------------------------------------------------------------------
class SpeedyRCCommandReceiver;
class RCRoutingManager;
//------------------------------------------------------------------------------------------------------------------------
class ParsedKeyParam
{
public:    
        UFC::AnsiString	AE;
        UFC::AnsiString	IB;
        UFC::AnsiString	CNC;
        UFC::AnsiString	IP;
        UFC::AnsiString	CustomAE;
};
//------------------------------------------------------------------------------------------------------------------------
// Check Level Description
//------------------------------------------------------------------------------------------------------------------------
// Check                        RCServer failed  Confirm  Fill 
// Level     Check behavior     behavior         To DB    To DB 
// -----  --------------------  ---------------  -------  -----
//   0    No Check (UI Check)   X                   V       V
//   6    RCServer Fully check  Reject              X       V
//   7    RCServer Fully check  No Check            X       V
//   8    RCServer Fully check  No Check            V       V
//   9    RCServer Fully check  DB Check            V       V
//  10    Backend system check  X                   V       V
//------------------------------------------------------------------------------------------------------------------------
// class RCRoutingObject
//------------------------------------------------------------------------------------------------------------------------
class RCRoutingObject
{
public:
	RCRoutingNodeBase* Node1;
	RCRoutingNodeBase* Node2;
	RCRoutingObject() : Node1( NULL ), Node2( NULL ) {}
};
//----------------------------------------------------------------------------------------------------------------------
// Interface DBResultListener
//----------------------------------------------------------------------------------------------------------------------
class DBResultListener
{
public:
	virtual void OnQueueDataArrived( BOOL IsOption, UFC::UQueueStruct *QueueData, int Len ) = 0;
	virtual ~DBResultListener() {}
};
//------------------------------------------------------------------------------------------------------------------------
// class DBResultSubject
//------------------------------------------------------------------------------------------------------------------------
class DBResultSubject : public UFC::PThread
{
private:
	UFC::UMessageQueue	FQueue;
	key_t			FQKey;
	long			FQmtype;
	int			FQPermission;
	int			FQBytes;
	BOOL			FQIsCreate;
	/////////////////////////////////////////////////////////////////
	DBResultListener*	FQListenerData;
	UFC::UQueueStruct	FQRcvData;
private:
	BOOL			FIsOption;
private:	/// PThread implementation
	void	Execute( void );
private:
	void	AttachQueue( void );
public:
	void	SetQueueListener( DBResultListener* QListener );
	void	Run( void );
public:
	DBResultSubject( BOOL IsOption, key_t Key, long MessageType, int Q_Permission = 0666, int nbytes = IPCSMQ_DEFAULTSIZE, BOOL IsCreateQueue = TRUE );
	~DBResultSubject();
};
//------------------------------------------------------------------------------------------------------------------------
// class SpeedyReportReceiver
//------------------------------------------------------------------------------------------------------------------------
class SpeedyReportReceiver : public UQueueListener 
{
private:
	BOOL				FIsOption;
        long                            FQueueKey;
        UQueueSubject*	  	        FQueueSubject;
        RCRoutingManager*               FReportRoutingManager;
private:
        enRtnOrderReport SendOrderReport( BOOL IsOpt, UFC::UQueueStruct *QueueData , int Len, int& CheckLevel );
private: /// Implement interface UQueueListener
	void OnQueueDataArrived( UQueueStruct *QueueData, int Len );
public:
	SpeedyReportReceiver( BOOL IsOption, RCRoutingManager* ReportRoutingManager );
	~SpeedyReportReceiver(){};
public:
	void Run( void );
};
//------------------------------------------------------------------------------------------------------------------------
// class RCRoutingManager
//------------------------------------------------------------------------------------------------------------------------
class RCRoutingManager : public DBResultListener, public RCReportReplyListener
{
private:
	typedef hash_map< UFC::AnsiString, RCRoutingNodeBase*, hash< UFC::AnsiString > >		TIpToRoutingNode;
private:
	int                             FRCGroup;
	int                             FRCSourceTag;
	UFC::AnsiString                 FRCSourceStr;
	enRoutingType                   FRoutingType;
	BOOL                            FIsSpeedyUse;
	RCCheckResultListener*		FResultListener;
	RCSpeedyCheckResultListener*	FSpeedyResultListener;
	UFC::AnsiString			FMDSHomeDir;
	BOOL				FIsSendReportAnyCL;
	BOOL				FIsBreakCheck;
	BOOL				FIsBreakReport;
        UFC::AnsiString                 FClientSystem;
        wsThresholdCfgParm              FThresholdCfgParm;
private:
	Int32				FTRC_Routing_PKSize;
        Int32				FTRC_Routing_Query_PKSize;
        Int32				FTCustomAccount_PKSize;
        MDS::MTable*			FTRC_Listing;
	MDS::MTable*			FTRC_Routing;
        MDS::MTable*			FTRC_Routing_Query;
        MDS::MTable*			FTCustomAccount;   
	TIpToRoutingNode		FNodeContainer;
	UFC::PCriticalSection           FCS_RoutingStruct;
        UFC::PCriticalSection           FCS_CustomAccount;
private: ///< For Speedy use
        SpeedyReportReceiver*           FFutSpeedyReportReceiver;
        SpeedyReportReceiver*           FOptSpeedyReportReceiver;
	UFC::PQueue*			FQueueDBCheck_Fut;	///< 0x72A30000
	UFC::PQueue*			FQueueDBCheck_Opt;	///< 0x73A30000
	DBResultSubject*		FDBSubject_Fut;		///< 0x72A40000
	DBResultSubject*		FDBSubject_Opt;		///< 0x73A40000
private:
	void				NewAllTable( void );
        void				NewAllTableSpeedy( void );
        void                            CreateSpeedyRCCommandReceiver( void );
        void                            CreateSpeedyReportReceiver( void );
        void                            CreateThresholdManager( void );
	void				DeleteAllTableAndAttach( void );
	void				CreateNodesByListingTable( void );
	BOOL				GetRowByRoutingTable( const char* IB, const char* Account, MDS::MRow& Row );
        BOOL                            GetRowByQueryRoutingTable( const char* IB, const char* Account, MDS::MRow& Row );
        BOOL                            GetRowByCustomAccountTable( const char* BrokerID, const char* Account, MDS::MRow& Row );
	RCRoutingObject*                CreateRoutingObject( MDS::MRow& Row, UFC::AnsiString& IP1, UFC::AnsiString& IP2 );
private:
	int				GetRoutingInfo( const char*		IB,             /* Input parm */
							const char*		Account,        /* Input parm */
							MDS::MRow&		Row,            /* Output parm */
                                                        RCRoutingObject&	RoutingObj,     /* Output parm */
                                                        BOOL                    IsQuery = FALSE );	
public:
        UFC::PCriticalSection           FReportReceiverCS;
public:
	void				SetRCCenterBreakState( BOOL IsBreakCheck, BOOL IsBreakReport );
	void				GetRCCenterBreakState( BOOL& IsBreakCheck, BOOL& IsBreakReport );
public:
	enRtnCheckRequest               SendCheckRequest( BOOL			IsOpt,
                                                          UFC::UQueueStruct*	QueueData,	/* A3 format */
                                                          int			Len,
                                                          RCCheckExtendData&	ExtendData );	/* For check level 0 7 8 use */
	enRtnOrderReport                SendOrderReport( BOOL			IsOpt,
                                                         UFC::UQueueStruct*	QueueData,	/* A5 format */
                                                         int			Len,
                                                         int&			CheckLevel );
	void				ReLoadRouting( void );
private: ///< Implement interface ReportReplyListener
        void                    OnReportReplyArrive( BOOL IsOpt, UFC::UQueueData *Data, int Len );
//--------------------------------------------------------------------
// For Speedy use
//--------------------------------------------------------------------
private: ///< Implement interface DBResultListener
	void			OnQueueDataArrived( BOOL IsOption, UFC::UQueueStruct *QueueData, int Len );
private:
	void			CreateDBCheckObjects( void );
	void			SpeedyDoUnCheck( RCSpeedyCheckParm& CheckParm, RCCheckExtendData& ExtendData );
	void			SpeedyDoRejectOrder( RCSpeedyCheckParm& CheckParm, int RtnVal );
	void			SpeedyDoRejectOrder( RCSpeedyCheckParm& CheckParm, int RtnVal, const char* Message );
	enRtnCheckRequest	SpeedyDoSendCheckToDB( RCSpeedyCheckParm& CheckParm );
	enRtnCheckRequest	SpeedySendCheckRequest( RCSpeedyCheckParm&	CheckParm,
                                                        RCCheckExtendData&	ExtendData ); /* For check level 0 7 8 use */
        ///< GDD
	BOOL			ParserGDDUDD( RCSpeedyCheckParm& CheckParm ,            UFC::AnsiString&   ErrorMsg);
        BOOL                    GDD_FromStandard( RCSpeedyCheckParm& CheckParm,         UFC::AnsiString&   ErrorMsg);
        BOOL                    GDD_FromSuper888_PMF( RCSpeedyCheckParm& CheckParm,     UFC::AnsiString&   ErrorMsg, BOOL IsAdminTool);
        BOOL                    GDD_FromSpeedyAPI_PMF( RCSpeedyCheckParm& CheckParm,    UFC::AnsiString&   ErrorMsg);                
        BOOL                    GDD_FromSpeedyAPI_YUANTA( RCSpeedyCheckParm& CheckParm, UFC::AnsiString&   ErrorMsg);
        void                    SetGDD( RCSpeedyCheckParm& CheckParm, const ParsedKeyParam ParsedKey);        
public:
	void			SpeedySendCheckRequest( RCSpeedyCheckParm& CheckParm );
	void			SpeedySendOrderReport( RCSpeedyReportParm& ReportParm ) { };
public:
	RCRoutingManager( int                           RCGroup,
			  int                           RCSourceTag,      ///< 01 ~99
                          enRoutingType                 RoutingType,
                          RCSpeedyCheckResultListener*	SpeedyResultListener,
                          const UFC::AnsiString&	MDSHomeDir,                                   
                          const UFC::AnsiString&        ClientSystem,
                          wsThresholdCfgParm&           ThresholdCfgParm );
        
//--------------------------------------------------------------------
public:
	RCRoutingManager( int				RCGroup,
			  int				RCSourceTag, ///< 01 ~99
			  enRoutingType			RoutingType,
			  RCCheckResultListener*	ResultListener,
			  const UFC::AnsiString&	MDSHomeDir,
			  BOOL				IsSendReportAnyCL,
                          wsThresholdCfgParm&           ThresholdCfgParm);
	virtual ~RCRoutingManager( void );
};

//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
