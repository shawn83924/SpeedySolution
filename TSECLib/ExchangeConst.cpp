//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ExchangeConst.h"
#include "../ExchangeUtility/Utility.h"
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
int        TWSE_PVC_BUSY_RETRY_TIMES          = 50000;  ///< PVC broken retry times.   
int        TWSE_MAX_MESSAGE_SIZE              = 4096;    
long       TWSE_PULL_INTERVAL                 = 100L;
int        TWSE_PULL_TIMES                    = 12;  
long       TWSE_LINKSUBSYSTEM_TIMEOUT         = 180L;   ///< Link sub-system timeout.
long       TWSE_INTERNAL_TIMEOUT              = 12L; 
long       TWSE_EX_IDLEHANDSHAKE_TIME_XBIO3   = 58L;   
long       FILESUBSYSTEM_TIMEOUT      = 90L; 
long       FILESUBSYSTEM_HEARTBEAT    = 60L; 
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString TWSE_EX_PVC_RESET_SUBSCRIBER    = "TFX.UNI.PROD.XTC.RST";    // Migo message for PVC reset.
UFC::AnsiString TWSE_EX_PVC_STATE_PUBLISHER     = "TFX.OPT.PROD.XBIO.GUI";
UFC::AnsiString TWSE_EX_PVC_STATE_SUBSCRIBER    = "TFX.OPT.PROD.GUI.XBIO";
UFC::AnsiString TWSE_EX_OPEN_SWITCH_SUBSCRIBER  = "TFX.UNI.PROD.SCH";
//----------------------------------------------------------------------------------------------------------------------


