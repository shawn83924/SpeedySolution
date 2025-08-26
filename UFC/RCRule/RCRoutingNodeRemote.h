//------------------------------------------------------------------------------------------------------------------------
#ifndef __RCRoutingNodeRemote_H
#define __RCRoutingNodeRemote_H
//------------------------------------------------------------------------------------------------------------------------
#include "RCRoutingBase.h"
//------------------------------------------------------------------------------------------------------------------------
// class RCRoutingNodeRemote
//------------------------------------------------------------------------------------------------------------------------
class RCRoutingNodeRemote : public UFC::PThread, public UFC::SocketClientListener, public RCRoutingNodeBase
{
private:
	enRoutingType                FRoutingType;
	BOOL                         FIsSpeedyUse;
	RCCheckResultListener*       FResultListener;
	RCReportReplyListener*       FReplyListener;
        RCSpeedyCheckResultListener* FSpeedyResultListener;
	UFC::AnsiString              FRoutingIP;
	int                          FRCSourceTag;
	UFC::AnsiString              FRCSourceStr;
private:
        BOOL                         FIsLogon;
        int                          FLogConnectFailedStatus;
	UFC::PClientSocket*          FClientSocket;
        UFC::PClientSocket*          FReplyClientSocket;
	int                          FHeartbeatCountDown;
	std::queue<UFC::AnsiString*> FRemainDataQueue;
	UFC::PCriticalSection	     FCS_RemainDataQueue;
private:
	BOOL				ConnectAndLogonRCGateway( void );
	void				ReSendRemainDataQueue( void );
	void				PushToRemainDataQueue( UFC::AnsiString* DataPtr );
	BOOL				DoRecvLogonConfirm( RCGPacket& Packet, UFC::PClientSocket* Socket );
	BOOL				DoRecvCheckResult( RCGPacket& Packet, UFC::PClientSocket* Socket );
        BOOL				DoRecvReportReply( RCGPacket& Packet, UFC::PClientSocket* Socket );
	void				DoSendHeartbeat( UFC::PClientSocket* Socket );
	void				DoSendHeartbeatConfirm( UFC::PClientSocket* Socket );
private: ///< Implement interface SocketClientListener
	void				OnConnect( UFC::PClientSocket* Socket );
	void				OnDisconnect( UFC::PClientSocket* Socket, BOOL NeedReconnect = FALSE );
	BOOL				OnDataArrived( UFC::PClientSocket* Socket );
	void				OnIdle( UFC::PClientSocket* Socket );
private: ///< Implement interface PThread
	void				Execute( void );
public: ///< Implement RCRoutingNodeBase
	void				StopInternalThread( void );
	enRtnCheckRequest	SendCheckRequest( BOOL                  IsOpt,
                                                  int                   CheckLevel,	///< 1 ~ 9
                                                  UFC::UQueueStruct*	QueueData,	///< A3 format
                                                  int                   Len );
	enRtnOrderReport	SendOrderReport( BOOL                   IsOpt,
                                                 int			CheckLevel,	///< 1 ~ 9
                                                 UFC::UQueueStruct*	QueueData,	///< A5 format
                                                 int                    Len );
        BOOL                    IsLogon( void ) { return FIsLogon; }
//--------------------------------------------------------------------
// For Speedy use
//--------------------------------------------------------------------
public:
	enRtnCheckRequest	SpeedySendCheckRequest( int                CheckLevel,	///< 1 ~ 9
                                                        RCSpeedyCheckParm& CheckParm );
public:
	RCRoutingNodeRemote( enRoutingType                RoutingType,
                             RCSpeedyCheckResultListener* SpeedyResultListener,
                             RCReportReplyListener*       ReplyListener,
                             const UFC::AnsiString&	  RoutingIP,
                             int                          RCSourceTag );
//--------------------------------------------------------------------
public:
    
	RCRoutingNodeRemote( enRoutingType          RoutingType,
                             RCCheckResultListener* ResultListener,
                             RCReportReplyListener* ReplyListener,
                             const UFC::AnsiString& RoutingIP,
                             int                    RCSourceTag );
	virtual ~RCRoutingNodeRemote();
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
