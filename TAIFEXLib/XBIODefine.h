//----------------------------------------------------------------------------------------------------------------------
#ifndef __XBIODEFINE_H
#define __XBIODEFINE_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "../UFC/AnsiString.h"
#include "../ExchangeUtility/Utility.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern int  OS_ACCEPT_ORDER;
extern int  OS_OPEN_MARKET;
extern int  OS_REJECT_ORDER;
extern int  OS_SHOUTDOWN_SYS;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern int  PVC_BUSY_RETRY_TIMES;
extern int  PVC_MAX_CONTROLBUFFER;
extern int  PVC_MAX_PACKET_SIZE;
extern int  MAX_MESSAGE_SIZE;
//----------------------------------------------------------------------------------------------------------------------
extern long IDLEHANDSHAKE_TIME_XBIO4;
extern long IDLEHANDSHAKE_TIME_XBIO3;

extern long LINKSUBSYSTEM_TIMEOUT;
extern long ORDERSUBSYSTEM_TIMEOUT;
extern long TAIFEX_INTERNAL_TIMEOUT;
extern long TAIFEX_MIGOMSG_TIMEOUT;
extern long TXIFEX_PULL_INTERVAL;
extern int  TXIFEX_PULL_TIMES;
extern BOOL TAP_TEST_MODE;
extern long TAP_MESSAHE_QUEUE_TIMEOUT;
extern int  MAX_PVC;
extern int  XBIO3_MESSAGEQUEUE_SIZE;
//----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString PVC_RESET_SUBSCRIBER;
extern UFC::AnsiString PVC_BACKUP_SUBSCRIBER;
extern UFC::AnsiString PVC_STATE_PUBLISHER;
extern UFC::AnsiString PVC_STATE_SUBSCRIBER;
extern UFC::AnsiString PCM_RESET_SUBSCRIBER;
//----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString DB_PUBLISHER_SUBJECT;
extern UFC::AnsiString DB_SUBSCRIBER_SUBJECT;
extern UFC::AnsiString OPEN_SWITCH_SUBSCRIBER;
//----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString OQ_QUERY_SUBSCRIBER;
extern UFC::AnsiString OQ_QUERY_PUBLISHER;
//----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString Q_REQUEST_SUBSCRIBER;
extern UFC::AnsiString Q_REQUEST_PUBLISHER;
//----------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString DB_SERVER;
extern UFC::AnsiString DB_NAME;
extern UFC::AnsiString DB_USERNAME;
extern UFC::AnsiString DB_PASSWORD;
//----------------------------------------------------------------------------------------------------------------------
extern void LoadXBIOConfig( const char* FileName = "XBIOCfg.ini" );
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
