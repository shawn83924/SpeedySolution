//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "XBIODefine.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Trading sessions for TSEC and OTC
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//        Normal    Odd       Post     
// 08:25  - Pre open 
// 08:30  _ Open
//        |  
//        |
// 12:00  |
//        |
// 13:30  - Close
// 13:40            _ Open
// 13:55            |         _ Open
// 14:00            |         | 
// 14:35            - Close   - Close
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int        OS_ACCEPT_ORDER          = 10;      ///< Pre Open market. 08:25
int        OS_OPEN_MARKET           = 30;      ///< Open market. 08:30
int        OS_REJECT_ORDER          = 100;     ///< Close market. 13:30
int        OS_POST_ORDER            = 110;     ///< Pre Open Post trade order session. 13:50
int        OS_OPEN_POST             = 130;     ///< Open Post trade order session. 13:55
int        OS_ODD_ORDER             = 210;     ///< Pre Open Odd trade order session. 13:35
int        OS_OPEN_ODD              = 230;     ///< Pre Open Odd trade order session. 13:40
int        OS_CLOSE_AFTERNOON       = 300;     ///< Close Odd/Post trade session. 14:35
int        OS_SHOUTDOWN_SYS         = 700;     ///< Close trading system. 14:35 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int        PVC_BUSY_RETRY_TIMES     = 500;     
int        PVC_MAX_CONTROLBUFFER    = 1024;    
int        PVC_MAX_PACKET_SIZE      = 256;     
int        MAX_MESSAGE_SIZE         = 1025;    
long       IDLEHANDSHAKE_TIME_XBIO4 = 62L;    
long       IDLEHANDSHAKE_TIME_XBIO3 = 58L;   
long       LINKSUBSYSTEM_TIMEOUT    = 180L;  
long       ORDERSUBSYSTEM_TIMEOUT   = 90L; 
long       TAIFEX_INTERNAL_TIMEOUT  = 12L; 
long       TAIFEX_MIGOMSG_TIMEOUT   = 20L; 
long       TXIFEX_PULL_INTERVAL     = 100L;
int        TXIFEX_PULL_TIMES        = 12;  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL       TAP_TEST_MODE = 0;                
long       TAP_MESSAHE_QUEUE_TIMEOUT = 330L; 
int        MAX_PVC = 240;                   
int        XBIO3_MESSAGEQUEUE_SIZE = 14;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString PVC_RESET_SUBSCRIBER    = "TFX.UNI.PROD.XTC.RST";    // Migo message for PVC reset.
UFC::AnsiString PVC_BACKUP_SUBSCRIBER   = "TFX.UNI.PROD.XTC.BAK";
UFC::AnsiString PVC_STATE_PUBLISHER     = "TFX.OPT.PROD.XBIO.GUI";
UFC::AnsiString PVC_STATE_SUBSCRIBER    = "TFX.OPT.PROD.GUI.XBIO";
UFC::AnsiString OPEN_SWITCH_SUBSCRIBER  = "TFX.UNI.PROD.SCH";
//----------------------------------------------------------------------------------------------------------------------

