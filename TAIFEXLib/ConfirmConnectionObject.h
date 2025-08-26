#ifndef __CONFIRM_CONNECTION_OBJECT_H
#define __CONFIRM_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "ConnectionObject.h"
#include "../UFC/UFC.h"
#include "../../inc/cch.h"
#include "../../inc/cch_reader.h"
//-----------------------------------------------------------------------------------------------------------------------
//   TAIFEX Side: Connection Object for AP code 3 ( Matched Confirm report )
//--------------------------------------------------------------------------------------------------------------------
extern UFC::SInt TotalCount;
extern UFC::SInt TotalProcessingTime;
extern UFC::SInt MaxProcessingTime;
//-----------------------------------------------------------------------------------------------------------------------
class LinkSubSystemTAIFEX;
//-----------------------------------------------------------------------------------------------------------------------
class TAIFEXConfirmConnectionObject : public ConnectionObjectBase, public UFC::PThread
{
private: // For CCH client
	RPTLOG_MSG_LAST      FLastReport;
	CCH_DF_Reader*       FCCHReader;
	RPTLOG*              FReportBuffer;
	BROKER_PVC_DEF       FPVCInfo;
private:
 	BOOL                  FHandshakeReady;
	BOOL                  FNeedLock;
	BOOL                  FNeedReset;
	ConnectionListener*   FConnectionListener;
	int                   FPreviousTime;
	LinkSubSystemTAIFEX*  FConnection;
	UFC::PList*  		  FMessages;
	UFC::PCriticalSection FMsgCriticalSection;
	int					  FSendPosition;
	int					  FReadPosition;
	int					  FEndPosition;
	int					  FTotalCount;
	BOOL                  FConnected;
	BOOL                  FIsTSCD;
	BOOL                  FBackup;
	MessageHeader*        FLastMsg;
    LogManager*           FLog;
private:
	void IdleHandshake( void );
	void ConfirmsubSystemHandshake( BOOL ReceivedL010 );
	void MessageLoop( void );
	void ClearMessageList();
	void AddLastMessage();
	BOOL ReadMessage();
	BOOL MessageOut();
	BOOL SendConfirmMessage( );
	MessageHeader* CheckC010( MessageC010* C010 );
	MessageHeader* CheckC011( MessageC011* C011 );
	void AddReportMessage( int ReportCount, char ApCode );
	void LockLoop( void );
	virtual void Execute( void );
public:
	TAIFEXConfirmConnectionObject( ConnectionParameter& info,
	                  		       ConnectionListener*  Listener,
	                  		       UFC::ThreadListener*      TListener );
	virtual ~TAIFEXConfirmConnectionObject();
    void SetLogManager( LogManager* LogObj );
public:
	virtual void Close( void );
        virtual void Active( void ) { PThread::Start(); }
	virtual void OnCommand( int Command, void* Data );
	virtual void UpdateConnectionParameter( ConnectionParameter& Info );
};
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
