#ifndef _SPEEDY_H
#define _SPEEDY_H
//---------------------------------------------------------------------------
const int MAX_PROCESS        = 64;
const int PROCESS_STOPED     = 0;
const int PROCESS_STARTING   = 1;
const int PROCESS_STOPING    = 2;
const int PROCESS_RESTARTING = 3;
const int PROCESS_RUNNING    = 4;
//---------------------------------------------------------------------------
const int SPEEDY_AGENT_ACK             = 1;
const int SPEEDY_AGENT_REPLY           = 2;
const int SPEEDY_GET_LIST              = 3;
const int SPEEDY_REPLY_LIST            = 4;
const int SPEEDY_GET_PROCESS           = 5;
const int SPEEDY_REPLY_PROCESS         = 6;
const int SPEEDY_RUN_PROCESS           = 7;
const int SPEEDY_KILL_PROCESS          = 8;
const int SPEEDY_RESTART_PROCESS       = 9;
const int SPEEDY_GET_PROCESS_LIST      = 21;
const int SPEEDY_REPLY_PROCESS_LIST    = 22;
const int SPEEDY_MODIFY_PROCESS        = 23;
const int SPEEDY_GET_CONFIG_FILE       = 24;
const int SPEEDY_REPLY_CONFIG_FILE     = 25;
const int SPEEDY_SET_CONFIG_FILE       = 26;
const int SPEEDY_CONFIG_FILE_SAVED     = 27;
const int SPEEDY_CANCEL_FUT_ORDERS     = 31;
const int SPEEDY_CANCEL_OPT_ORDERS     = 32;
//---------------------------------------------------------------------------
const int SPEEDY_MODIFY_PVC            = 10;
const int SPEEDY_CHANGE_FIX_SETTING    = 13;
const int SPEEDY_GET_FIX_SETTING       = 14;
const int SPEEDY_REPLY_FIX_SETTING     = 15;
const int SPEEDY_MONITER_PVC           = 16;
const int SPEEDY_REPLY_PVC_LOG         = 17;
//---------------------------------------------------------------------------
const int IMFIX_ASK_SESSIONS_STATE   = 0;
const int IMFIX_REPLY_SESSIONS_STATE = 1;
const int IMFIX_ASK_SESSION_STATE    = 2;
const int IMFIX_REPLY_SESSION_STATE  = 3;
const int IMFIX_SET_SEQUENCE_NO      = 4;
const int IMFIX_SESSION_EOD          = 5;
const int IMFIX_DISCONNECT_SESSION   = 6;
const int IMFIX_ASK_ALL_STATE        = 7;
const int IMFIX_ENABLE_SESSION       = 8;

const int IMFIX_RECONNECT_SESSION    = 12;
//---------------------------------------------------------------------------
const int AGENT_ACK              = 9000;
const int AGENT_REPLY            = 9001;
const int AGENT_REQUEST_STATE    = 9004;
const int AGENT_REPLY_STATE      = 9005;
const int AGENT_LISTEN_PVC       = 9006;
const int AGENT_PVC_DATA         = 9007;
const int AGENT_PERFORMANCE      = 9010;
const int AGENT_PATS_PERFORMANCE = 9011;
const int AGENT_EVENT            = 9020;
const int AGENT_PVC_ENABLED      = 9022;
const int AGENT_CHANGE_APCODE    = 9023;
const int AGENT_SET_SEQUENCE     = 9024;
const int AGENT_REJECT_ALL_ORD   = 9025;
const int AGENT_SET_SESSION      = 9026;
const int AGENT_SESSION_STATUS   = 9027;
const int AGENT_ASK_AE_STATUS    = 9028;
const int AGENT_AE_STATUS_REPORT = 9029;
const int AGENT_PVC_KILLSWITCH   = 9030;
const int AGENT_REQ_KILLSWITCH   = 9032;
const int AGENT_KILLSWITCH_LIST  = 9033;
const int AGENT_PVC_BACKUPSWITCH = 9034;
const int CMD_PROXY_LOCK_USER    = 10005;
const int CMD_PROXY_UNLOCK_USER  = 10006;
const int CMD_PROXY_STATUS_REQ   = 10007;
const int CMD_PROXY_THROUGHPUT_REQ    = 10009;
const int CMD_PROXY_THROUGHPUT_UPDATE = 10010;
const int CMD_PROXY_BROKEN_USER  = 10011;
//---------------------------------------------------------------------------
const int CMD_LOGIN             = 1;
const int CMD_LOGIN_RESULT      = 2;
const int CMD_LOGOUT            = 3;
const int CMD_GET_CONFIG        = 4;
const int CMD_UPDATE_CONFIG     = 5;
const int CMD_GET_LIST          = 6;
const int CMD_USERS_LIST        = 7;
const int CMD_ADD_USER          = 8;
const int CMD_ADD_RESULT        = 9;
const int CMD_DELETE_USER       = 10;
const int CMD_DELETE_RESULT     = 11;
const int CMD_MODIFY_USER       = 12;
const int CMD_MODIFY_RESULT     = 13;
const int CMD_MODIFY_PWD        = 14;
const int CMD_MODIFY_PWD_RESULT = 15;
const int CMD_QUERY_LOG         = 18;
const int CMD_REPLY_LOG         = 19;
const int CMD_WRITE_LOG         = 20;
//---------------------------------------------------------------------------
const int AGENT_HEARTBEAT_INTERVAL = 6000;
//---------------------------------------------------------------------------
#define WM_UPDATE_LINES     (WM_USER + 100)
#define WM_ADMIN_LOGON      (WM_USER + 101)
#define WM_OP_LOGON         (WM_USER + 102)
#define WM_UPDATE_PVC       (WM_USER + 103)
#define WM_UPDATE_STATE     (WM_USER + 104)
#define WM_PROCESS_STATE    (WM_USER + 105)
#define WM_UPDATE_OPT_PEF   (WM_USER + 106)
#define WM_UPDATE_FUT_PEF   (WM_USER + 107)
#define WM_UPDATE_STATUS    (WM_USER + 108)
#define WM_ADD_HOSTS        (WM_USER + 109)
#define WM_UPDATE_FIX       (WM_USER + 110)
#define WM_FIX_STATE_CHANGE (WM_USER + 111)
#define WM_UPDATE_PVC_LOG   (WM_USER + 112)
#define WM_UPDATE_FTP_STAT  (WM_USER + 113)
#define WM_NEW_PVC_LOG      (WM_USER + 114)
#define WM_FORCE_LOGOUT     (WM_USER + 115)
#define WM_UPDATE_PROCESS   (WM_USER + 116)
#define WM_FIX_PERFORMANCE  (WM_USER + 117)
#define WM_UPDATE_USER      (WM_USER + 118)
#define WM_LOAD_USER_LOG    (WM_USER + 119)
#define WM_USER_LOGON       (WM_USER + 120)
#define WM_ADD_FIX_MSG      (WM_USER + 121)
#define WM_EVENT_MSG        (WM_USER + 122)
#define WM_RESTORE_WIN      (WM_USER + 123)
#define WM_CONFIG_FILE      (WM_USER + 124)
#define WM_LOGON            (WM_USER + 125)
#define WM_DISCONNECT_MBUS  (WM_USER + 126)
//---------------------------------------------------------------------------
#endif