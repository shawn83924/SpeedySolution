//------------------------------------------------------------------------------------------------------------------------
#ifndef __RCRoutingNodeLocal_H
#define __RCRoutingNodeLocal_H
//------------------------------------------------------------------------------------------------------------------------
#include "RCRoutingBase.h"
//------------------------------------------------------------------------------------------------------------------------
// class RCRoutingReplyReceiver
//------------------------------------------------------------------------------------------------------------------------
class RCRoutingReplyReceiver : public UQueueListener 
{
private:
	UQueueSubject*		FQueueSubject;
	RCReportReplyListener*  FReplyListener;
private:
	void OnQueueDataArrived( UQueueStruct *QueueData, int Len );
public:
	RCRoutingReplyReceiver( RCReportReplyListener* ReplyListener );
	~RCRoutingReplyReceiver( );
public:
	void Run( void );
};
//------------------------------------------------------------------------------------------------------------------------
// class RCRoutingNodeLocal
//------------------------------------------------------------------------------------------------------------------------
class RCRoutingNodeLocal : public UFC::PThread, public UFC::SocketClientListener, public UFC::UQueueListener, public RCRoutingNodeBase
{
private:
	enRoutingType			FRoutingType;
	BOOL				FIsSpeedyUse;
	RCCheckResultListener*		FResultListener;
        RCReportReplyListener*		FReplyListener;
	RCSpeedyCheckResultListener*	FSpeedyResultListener;
	UFC::UQueueSubject*		FQueueSubject;
        RCRoutingReplyReceiver*         FReportReply;
private:
	UFC::PQueue			FQueueRCCheck;		///< 0x78D90000
	UFC::PQueue			FQueueRCCheckResult;	///< 0x78DA0000
	UFC::PQueue			FQueueRCReport;		///< 0x78DC0000
        UFC::PQueue			FQueueRCReportReply;	///< 0x78DE0000
private:
	UFC::AnsiString			FRoutingIP;
	int				FRCSourceTag;
	UFC::AnsiString			FRCSourceStr;
        BOOL                            FIsLogon;
        int                             FLogConnectFailedStatus;
	UFC::PClientSocket*		FClientSocket;
	int				FHeartbeatCountDown;
private: ///< For Detect RCServer if live use
	BOOL				ConnectAndLogonRCGateway( void );
	BOOL				DoRecvLogonConfirm( RCGPacket& Packet, UFC::PClientSocket* Socket );
	void				DoSendHeartbeat( UFC::PClientSocket* Socket );
	void				DoSendHeartbeatConfirm( UFC::PClientSocket* Socket );
	void				DoRejectDataRemainInQueue( void );
private: /// Implement interface SocketClientListener
	void				OnConnect( UFC::PClientSocket * Socket );
	void				OnDisconnect( UFC::PClientSocket * Socket, BOOL NeedReconnect = FALSE );
	BOOL				OnDataArrived( UFC::PClientSocket * Socket );
	void				OnIdle( UFC::PClientSocket * Socket );
private: ///< Implement interface UQueueListener
	void				OnQueueDataArrived( UFC::UQueueStruct *QueueData, int Len );
private: ///< Implement interface PThread
	void				Execute( void );
public: ///< Implement RCRoutingNodeBase
	void				StopInternalThread( void );
	enRtnCheckRequest	SendCheckRequest( BOOL                  IsOpt,
						  int	  	        CheckLevel,	///< 1 ~ 9
						  UFC::UQueueStruct*	QueueData,	///< A3 format
						  int			Len );
	enRtnOrderReport	SendOrderReport( BOOL			IsOpt,
						 int			CheckLevel,	///< 1 ~ 9
						 UFC::UQueueStruct*	QueueData,	///< A5 format
						 int			Len );
        BOOL                    IsLogon( void ) { return FIsLogon; }
//--------------------------------------------------------------------
// For Speedy use
//--------------------------------------------------------------------
public: ///< For Speedy use
	enRtnCheckRequest	SpeedySendCheckRequest( int                     CheckLevel,	///< 1 ~ 9
							RCSpeedyCheckParm&	CheckParm );
public: ///< For Speedy use
	RCRoutingNodeLocal( enRoutingType                       RoutingType,
                            RCSpeedyCheckResultListener*	SpeedyResultListener,
                            RCReportReplyListener*              ReplyListener,
			    const UFC::AnsiString&		RoutingIP,
		            int					RCSourceTag );
//--------------------------------------------------------------------
public:
	RCRoutingNodeLocal( enRoutingType               RoutingType,
			    RCCheckResultListener*	ResultListener,
                            RCReportReplyListener*      ReplyListener,
			    const UFC::AnsiString&	RoutingIP,
			    int				RCSourceTag );
	virtual ~RCRoutingNodeLocal();
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
