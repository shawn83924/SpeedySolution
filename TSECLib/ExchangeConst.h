//----------------------------------------------------------------------------------------------------------------------
#ifndef __TWSE_XBIODEFINE_H
#define __TWSE_XBIODEFINE_H
//----------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern int        OS_ACCEPT_ORDER;      ///< Pre Open market. 08:25
extern int        OS_OPEN_MARKET;      ///< Open market. 08:30
extern int        OS_REJECT_ORDER;     ///< Close market. 13:30
extern int        OS_POST_ORDER;     ///< Pre Open Post trade order session. 13:50
extern int        OS_OPEN_POST;     ///< Open Post trade order session. 13:55
extern int        OS_ODD_ORDER;     ///< Pre Open Odd trade order session. 13:35
extern int        OS_OPEN_ODD;     ///< Pre Open Odd trade order session. 13:40
extern int        OS_CLOSE_AFTERNOON;     ///< Close Odd/Post trade session. 14:35
extern int        OS_SHOUTDOWN_SYS;     ///< Close trading system. 14:35 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern int        TWSE_PVC_BUSY_RETRY_TIMES;     
extern int        TWSE_MAX_MESSAGE_SIZE;    
extern long       TWSE_PULL_INTERVAL;
extern int        TWSE_PULL_TIMES;  
extern long       TWSE_LINKSUBSYSTEM_TIMEOUT;  
extern long       TWSE_INTERNAL_TIMEOUT; 
const  long       TWSE_ORDERSUBSYSTEM_TIMEOUT   = 90L; 
const  long       TWSE_IDLEHANDSHAKE_TIME_ORDER = 50L;    

//extern int        PVC_MAX_CONTROLBUFFER;    
//extern int        PVC_MAX_PACKET_SIZE;     
//extern long       IDLEHANDSHAKE_TIME_XBIO4;    
extern long       TWSE_EX_IDLEHANDSHAKE_TIME_XBIO3;   
//extern long       IDLEHANDSHAKE_TIME_EXECUTE;   
extern long       FILESUBSYSTEM_TIMEOUT; 
extern long       FILESUBSYSTEM_HEARTBEAT; 
//extern long       TAIFEX_MIGOMSG_TIMEOUT; 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString TWSE_EX_PVC_RESET_SUBSCRIBER;    // Migo message for PVC reset.
//extern UFC::AnsiString PVC_BACKUP_SUBSCRIBER;
extern UFC::AnsiString TWSE_EX_PVC_STATE_PUBLISHER;
extern UFC::AnsiString TWSE_EX_PVC_STATE_SUBSCRIBER;
extern UFC::AnsiString TWSE_EX_OPEN_SWITCH_SUBSCRIBER;
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
