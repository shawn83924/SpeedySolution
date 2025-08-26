unit SCStrings;
interface
uses Windows;
resourcestring
  MINIMIZE_CAPTION               = 'SpeedyCenter minimize';{最小化';}
  MINIMIZE_HINT                  = 'Minimize SpeedyCenter in the system tray, double click tray icon to restore.';{'SpeedyCenter最小化在System Tray中,雙擊Tray Icon可以還原.';}
  MAIN_LOGON_FAILED_DISCONNECTED = 'login cancelled; the connection is broken';{'取消登入,通訊模組已斷線';}

  MAIN_HOSTNAME                  = 'Hostname';{'主機名稱';}
  MAIN_CPATION                   = 'Speedy Control Center';{'Speedy 控制台';}
  MAIN_CONNECTED                 = 'Connected';{'通訊模組已連線'}
  MAIN_LOGOUT                    = 'Logout';
  MAIN_ALL                       = 'All';
  MAIN_OPTIONS                   = 'Options';///'選擇權';
  MAIN_FUTURES                   = 'Futures';///'期貨';
  MAIN_TSEC                      = 'Equities';///'集中';
  MAIN_OTC                       = 'OTC';///'櫃檯';
  MAIN_NORMAL                    = 'Normal ';///'普通';
  MAIN_ODD                       = 'Odd ';///'零股';
  MAIN_POST                      = 'Post ';///'定價';
  MAIN_AUCTION                   = 'Auction ';///'拍賣';
  MAIN_NEGOTIATE_PX              = 'NegotiatePx ';///'議價';
  MAIN_LEND                      = 'Lend ';///'標借';
  MAIN_TENDER                    = 'Tender ';///標購';
  MAIN_TENDER_EX                 = 'TenderEx ';///證金標購';
  MAIN_FTP_SEND		               = 'File(Send)';///'檔案(傳送)';
	MAIN_FTP_RECV	                 = 'File(Recv)';///'檔案(接收)';
	MAIN_EXECUTION                 = 'Execution';///'成交回報';
  MAIN_ODD_NORMAL                = 'Odd/Normal';///'零股/普通';
  MAIN_POST_NORMAL               = 'Post/Normal';///定價/普通';
  MAIN_UNKNOWN_AP_CODE           = 'Unknown AP Code';///不明的APCode';
  MAIN_IP_ADDRESS                = 'IP Address';///IP 位址';
  MAIN_TSEC_BROKER_ID            = 'BrokerID';///証券商代號';
  MAIN_TAIFEX_BROKER_ID          = 'BrokerID';///期貨商代號';
  MAIN_CLEAR_MEMBER              = 'Clearing member';///結算會員';
  MAIN_SYSTEM_INFO               = 'Sytem Info:';///系統資訊:';
  MAIN_STATUS                    = 'Status:';///狀態:';
  MAIN_FUT_TCPIP                 = 'TMP[%s,%03d][Futures TCP/IP]';
  MAIN_FUT_X25                   = 'Link:%03d PVC:%03d BrokerID:%s[Futures]';
  MAIN_OPT_TCPIP                 = 'TMP[%s,%03d][Options TCP/IP]';
  MAIN_OPT_X25                   = 'Link:%03d PVC:%03d BrokerID:%s[Options]';
  MAIN_TSE_PVC                   = 'Port%05d PVC:%s BrokerID:%s[Equity]';
  MAIN_OTC_PVC                   = 'Port%05d PVC:%s BrokerID:%s[OTC]';
  MAIN_ADMIN_LOGON_OK            = 'Admin login Ok';///管理者登入成功';
  MAIN_PASSWD_EXP_WARNING        = 'Your password will expire in %d days, contact your system Admin for assistance.';///您的密碼將在%d天後過期,請儘快找系統管理員重設密碼.';
  MAIN_ADMIN                     = 'Administrator';///管理者';
	MAIN_USER                      = 'User';///使用者';
	MAIN_OP                        = 'Operator';///監控人員';
  MAIN_LOGON_OK                  = '%s Logon Ok';
  MAIN_GROUP_LOGON_OK            = '%d Logon Ok';
  MAIN_ORDER_LINE_BROKEN         = 'Order connection Link:%d PVC:%d broken';
  MAIN_TWSE_ORDER_LINE_BROKEN    = 'Order connection Link:%d PVCID:%s broken';
  MAIN_FILLED_LINE_BROKEN        = 'Execution connection Link:%d PVC:%d broken';
  MAIN_SESSION_BEFORE_OPEN       = 'Before Open';///盤前';
	MAIN_SESSION_TRY_OPEN          = 'Trying';//試單';
	MAIN_SESSION_MKT_OPENED        = 'Opened';///盤中';
	MAIN_SESSION_MKT_CLOSED        = 'Closed';///收盤';
  MAIN_SESSION_STATUS            = 'N[%s]O[%s]P[%s]';
  MAIN_ORDER_PER_SEC             = '%5.1f ord/sec';
  MAIN_ORDER_COUNT               = '%d ord';
  MAIN_ORDER_QUEUE_COUNT         = '%d ord(%d ord)';
  MAIN_QUEUE_WARNING             = 'Server:%s %d orders in queue.';///停留在%s主機上的委託已有%d筆.';
  MAIN_CID_WARNING               = 'Leaves %d Connection ID.';///連線ID剩下%d個.';
  MAIN_RM_NOT_EXISTS             = 'No risk management';///無檢核程式';
  MAIN_ACCOUNT_NOT_EXISTS        = 'This account does not exist.';///帳號不存在';
  MAIN_QUERY_ORDER_TIMEOUT       = 'Server:%s query order timed out.';///伺服器:%s 查單回應逾時';
  MAIN_MONITOR_ANOTHER_SVR_ASK   = 'Monitoring another sever[%s] on a cluster?';///否要改監控同一欉集中的主機[%s]?';
  MAIN_CALCEL_OPT_ALL_BY_ACCOUNT = '%s@%sCancel all Options orders of %s.';///%s@%s刪除帳號[%s]的選擇權委託.';
  MAIN_CALCEL_FUT_ALL_BY_ACCOUNT = '%s@%sCancel all Futures orders of %s.';///%s@%s刪除帳號[%s]的期貨委託.';
  MAIN_CALCEL_ALL_OPT_ORDERS     = '%s@%sCancel all Options orders.';///%s@%s刪除所有的選擇權委託.';
  MAIN_CALCEL_ALL_FUT_ORDERS     = '%s@%sCancel all Futures orders.';///%s@%s刪除所有的期貨委託.';
  MAIN_LICENSE_EXPIRED_DATE      = 'License expiration date:';///授權到期日:';
  MAIN_SEL_MONITORING_HOST       = 'Please select the Speedy server.';///請選擇欲監控的Speedy主機';

  MAIN_TSE_SEL_NORMAL_PVC        = 'Please select the normal PVC.';///'請先選取欲切換成普通線路的PVC.';
  MAIN_TSE_SEL_ODD_PVC           = 'Please select the odd PVC.';///'請先選取欲切換成零股線路的PVC.';
  MAIN_TSE_SEL_POST_PVC          = 'Please select the post PVC.';///'請先選取欲切換成定價線路的PVC.';
  MAIN_TSE_SEL_LEND_PVC          = 'Please select the lend PVC.';///'請先選取欲切換成標借線路的PVC.';
  MAIN_TSE_SEL_TENDER_PVC        = 'Please select the tender offer PVC.';///'請先選取欲切換成標購線路的PVC.';
  MAIN_TSE_SEL_TENDER_EX_PVC     = 'Please select the tender PVC.';///'請先選取欲切換成證金標購線路的PVC.';
  MAIN_TSE_SEL_AUCTION_PVC       = 'Please select the auction PVC.';///請先選取欲切換成拍賣線路的PVC.';

  MAIN_MONITORING_SESSION              = 'Monitoring';///監控線路';
  MAIN_STOP_MONITORING_SESSION         = 'Stop Monitoring';///停止監控';
  MAIN_ADMIN_QUERY_DATE_ERR            = 'Query condition error.';///查詢條件錯誤!迄日早於啟日.';
{ Resource strings for logon }
resourcestring
	MAIN_LOGON_FAILED_ACCOUNT_NOT_EXISTS = 'Account does not exist.';///'登入失敗,帳號不存在';
	MAIN_LOGON_FAILED_PASSWORD_ERROR     = 'Incorrect password.';///'登入失敗,密碼錯誤';
	MAIN_LOGON_FAILED_PASSWORD_EXPIRED   = 'Password expired.';///登入失敗,密碼已過期';
	MAIN_LOGON_FAILED_ACCOUNT_LOCKED     = 'The account is locked.';///登入失敗,帳號被鎖住';
	MAIN_LOGON_FAILED_ADMIN_LOGONED      = 'Some administrator has logined.';///登入失敗,已有管理者登入';
	MAIN_LOGON_FAILED_NOT_ALLOW_ADMIN    = 'The server does not support the function for administrators to login.';///此伺服器不支援管理者登入功能';
	MAIN_LOGON_FAILED_VERSION_TOO_OLD    = 'SpeedyCenter version too okd.';///SpeedyCenter版本太舊  ';  ///
{ Resource strings for add account }
resourcestring
  MAIN_ADD_FAILED_UPTO_LIMIT           = 'The maximum number of accounts has reached.';///帳號數目已達上限';
	MAIN_ADD_FAILED_ALREADY_EXISTS       = 'The account has already existed.';///帳號已存在';
	MAIN_ADD_ACCOUNT_FAILED              = 'Adding a new account failed.';///'新增帳號失敗';
	MAIN_ADD_FAILED_NO_RIGHTS            = 'The server has no right to add a new account.';///此主機無權限新增帳號';
	MAIN_ADD_ACCOUNT_OK                  = 'New account has been successfully created.';///新增帳號成功';
{ Resource strings for delete account }
resourcestring
  MAIN_DEL_FAILED_CANT_DEL_ALL_ADMIN   = 'Fail to delete all the administrators';///不能刪除所有的管理者';
	MAIN_DEL_FAILED_CANT_DEL_ALL_ACCOUNT = 'Fail to delete all the accounts';///不能刪除所有的帳號';
	MAIN_DEL_FAILED_NO_RIGHTS            = 'The server has no right to delete accounts.';///此主機無權限刪除帳號';
  MAIN_DEL_ACCOUNT_OK                  = 'The account has been successfully deleted.';///刪除帳號成功';
  MAIN_MODIFY_FAILED_NO_RIGHTS         = 'The server has no right to modify accounts.';///此主機無權限修改帳號';
  MAIN_MODIFY_ACCOUNT_OK               = 'The account has been successfully modified.';///修改帳號成功';
  MAIN_MODIFY_PASSWORD_NO_RIGHTS       = 'The server has no right to modify passwords.';///此主機無權限修改密碼';
  MAIN_MODIFY_PASSWORD_OK              = 'The password has been successfully modified.';///修改密碼成功';

  MAIN_NO_RESPONSE_CHECK_AGENT_MANAGER = 'No response received. Please check if SpeedyManager and SpeedyAgent operate properly.';///無回應,請確定伺服器上SpeedyManager及SpeedyAgent運作正常.';
  MAIN_NO_RESPONSE_CHECK_SPEEDYAGENT   = 'No response received. Please check if SpeedyManager operates properly.';///指令無回應,請確定伺服器上SpeedyManager運作正常.';
  MAIN_SPEEDYAGENT_NOT_EXIST           = 'The SpeedyAgent of %s does not exist.';///主機:%s 上的SpeedyAgent不存在';
  MAIN_ASK_LOGOUT_BEFORE_EXIT          = 'You have not logged out of the %s. Do you want to log out of the system?';///尚未登出主機[%s],是否要登出並關閉程式?';

  MAIN_ASK_DELETE_AE                    = 'Do you want to delete your account?';///是否要刪除帳號:';
  MAIN_ASK_SELECT_DEL_AE                = 'Please select the account you would like to delete.';///請先選取欲刪除的帳號.';
  MAIN_ASK_SELECT_MODIFY_AE             = 'Please select the account you would like to modify.';///請先選取欲修改的帳號.';
  MAIN_ASK_SELECT_MODIFY_PASSWORD_AE    = 'Please select the account of which the password you would like to modify.';///請先選取欲修改密碼的帳號.';
  MAIN_LOGON_ID_STATUS                  = 'ID status';///身份';
  MAIN_LOGON_ID_STATUS_NORMAL           = 'Normal';///正常';
  MAIN_LOGON_ID_STATUS_LOCKED           = 'Locked';///鎖住';
  MAIN_LOGON_ID_STATUS_EXPIRED          = 'Expired';///過期';
  MAIN_AE_ORDER_CHANNEL                 = 'Order channel';///下單管道';
  MAIN_AE_ACCOUNT_EXECUTIVE             = 'Account executive';///營業員';
  MAIN_AE_IP_NOT_FIXED                  = 'Not-fixed IP';///不指定';
  MAIN_AE_SEL_MODIFY_AE                 = 'Please select the account to be modified.';///請先選取欲修改的帳號.';
  MAIN_AE_SEL_DELETE_AE                 = 'Please select the account to be deleted.';///請先選取欲刪除的帳號.';
  MAIN_AE_SEL_MODIFY_PASSWORD_AE        = 'Please select the account of which the password you would like to modify.';///請先選取欲設定密碼的帳號.';
  MAIN_AE_DELETE_AE_WARNING             = 'Please note that it will cause some functional error if the account executive is deleted at market opening session.';///(注意:盤中請勿刪除營業員,會造成自動編單號功能錯誤)';
  MAIN_AE_CURRENT_MAX_CONNECTION        = 'Connections:%d Maximum:%d';///連線數:%d 最大連線數:%d';
  MAIN_AE_CURRENT_MAX_CONNECTION_CID    = 'Connections:%d Maximum:%d Leaves CID:%d';///連線數:%d 最大連線數:%d';

  MAIN_READ_FILE_FAILED                     = 'failed to read the file:%s.';///'讀取檔案:%s 失敗.';
  MAIN_PROCESS_STAT_SLEEPING                = 'Sleeping';///休眠';
  MAIN_PROCESS_STAT_STOPED                  = 'Stop';///停止';
  MAIN_PROCESS_STAT_DISABLED                = 'Disable';///停用';
  MAIN_PROCESS_STAT_DISABLEING              = 'Disabling';///停用中';
  MAIN_PROCESS_STAT_NOT_ACTIVE              = 'Not active';///無啟動';
  MAIN_PROCESS_STAT_ACTIVEING               = 'Active';///啟用中';
  MAIN_PROCESS_STAT_SATRTING                = 'Starting';///起動中';
  MAIN_PROCESS_STAT_STOPING                 = 'Stopping';///停止中';
  MAIN_PROCESS_STAT_RESTART                 = 'Restarting';///重起中';
  MAIN_PROCESS_STAT_RUNNING                 = 'Running';///執行中';
  MAIN_PROCESS_STAT_UNKNOWN                 = 'Unknown';///狀態不明';
  MAIN_PROCESS_CRASHED                      = 'Program %s abnormal termination';///程式%s不正常終止';
  MAIN_PROCESS_NO_RIGHTS_CHANGE_TIME        = 'No right to set start-up and shut-down time.';///無權限設定程式啟動停止時間';
  MAIN_PROCESS_NO_RIGHTS_CHANGE_SETTING     = 'No right to change settings';///無權限設定程式';
  MAIN_PROCESS_NO_CONFIG_FILE               = 'No configuration files';///這個程式沒有相對的設定檔案.';
  MAIN_PROCESS_NO_RIGHTS_STOP_START_PROCESS = 'No right for start/stop programs';///無權限啟動或停止程式';
  MAIN_PROCESS_NO_RIGHTS_CHANGE_STOP_START  = 'No right for setting program start/stop time';///無權限設定程式啟動停止時間';
  MAIN_PROCESS_NO_RIGHTS_STOP_PROCESS       = 'No right for start/stop programs';///無權限啟動停止程式';

  MAIN_PROCESS_START_ASK                    = 'Are you sure you want the start [%s] ?';///確定要啟動程式:%s';
  MAIN_PROCESS_RUNNING_STOP_RESTERT_FIRST   = 'The program [%s] is running. To restart, please click the stop button or the restart one.';///程式 %s 已在執行中.如要重新啟動,請先按[停止]鍵,或直接按[重起]鍵';
  MAIN_PROCESS_STOP_ASK                     = 'Are you sure you want the stop [%s] ?';///確定要停止程式:%s';
  MAIN_PROCESS_ALREADY_STOPED               = 'The program [%s] is already stopped.';///程式 %s 已在停止狀態.';
  MAIN_PROCESS_RESTART_ASK                  = 'Are you sure to restart the program [%s]?';///確定要重起程式:%s';

  MAIN_SESSION_ENABLE                       = 'Enable';///啟用';
  MAIN_SESSION_DISABLE                      = 'Disable';///停用';
  MAIN_SESSION_STAT_CONNECTED               = 'Connected';///已連線';
  MAIN_SESSION_STAT_BROKEN                  = 'Disconnected';///斷線';
  MAIN_SESSION_STAT_WAITING                 = 'Waiting';///等待連線';
  MAIN_SESSION_STAT_STOPED                  = 'Not-started';///未啟動';
  MAIN_SESSION_STAT_THREAD_STOPED           = 'The thread has stopped.';///線路Thread已停止';
  MAIN_SESSION_STAT_INITIALIZE              = 'Initializing';///起始中...';
	MAIN_SESSION_STATUS_FINISHED     	        = 'Finished';///成回已送完';
	MAIN_SESSION_STATUS_SUBSYS_ERR          	= 'Subsystem connection failed';///連線子系統錯誤';
  MAIN_SESSION_STATUS_PW_ERR       	        = 'Incorrect password';///密碼錯誤';
  MAIN_SESSION_STATUS_FCM_ERR      	        = 'Incorrect FCM code';///券商代號錯誤';
  MAIN_SESSION_STATUS_AP_ERR       	        = 'Incorrect AP code';///AP Code 錯誤';
  MAIN_SESSION_STATUS_BROKEN       	        = 'Reconnecting';///重新連線中';
  MAIN_SESSION_STATUS_OFFLINE_BUSY          = 'Waiting';///等待連線中';
  MAIN_SESSION_STATUS_SLEEPING              = 'Sleeping';///盤別變換中';
  MAIN_SESSION_STATUS_TRY_OPEN              = 'Trying';///試單中';
  MAIN_SESSION_STATUS_WAIT_OPEN             = 'Waiting for opening';///等待開盤中';
  MAIN_SESSION_STATUS_WAIT_TRY              = 'Waiting for try order';///等待試單中';
  MAIN_SESSION_STATUS_OFFLINE_READY	        = 'Ready';///線路閒置中';
  MAIN_SESSION_STATUS_ONLINE_BUSY  	        = 'Busy';///傳送資料中';
  MAIN_SESSION_STATUS_ONLINE_READY 	        = 'Slack';///線路閒置';
  MAIN_SESSION_STATUS_DISABLE      	        = 'Disable';///線路停用';
  MAIN_SESSION_STATUS_DISABLEING     	      = 'Disabling';///線路停用中';
  MAIN_SESSION_STATUS_ENABLE          	    = 'Enabling';///線路啟用中';
  MAIN_SESSION_STATUS_UNKNOWN               = 'Unknown code';///不明的狀態碼:';

  MAIN_FIX_SELECT_MOTORING_SESSION          = 'Please select the FIX Session.';///請先選取欲監控的FIX線路.';
  MAIN_FIX_SELECT_SET_SEQ_SESSION           = 'Please select the FIX Session of which the sequence number is to be set.';///請先點選欲設定Sequence Number的FIX Session.';
  MAIN_FIX_ASK_END_OF_DAY                   = 'Do you want to run End of Day of session:';///是否要執行End Of Day於Session:';
  MAIN_FIX_SELECT_EOD_SESSION               = 'Please select the FIX Session of which the End of Day you would like to run.';///請先點選欲執行End Of Day的FIX Session.';
  MAIN_FIX_SELECT_STOP_SESSION              = 'Please select the FIX Session to be stopped.';///請先點選欲停止的FIX Session.';
  MAIN_FIX_SELECT_ACTIVATE_SESSION          = 'Please select the FIX Session to be activated.';///請先點選欲啟用的FIX Session.';
  MAIN_FIX_NO_RIGHTS_ADD_SESSION            = 'There is no right to add new FIX Session.';///沒有新增FIX線路的權限.';
  MAIN_FIX_ASK_DELETE_SESSION               = 'Do you want to delete the FIX Session[%s]?';///是否要刪除FIX線路:%s?';
  MAIN_FIX_RUNNING_CAN_NOT_ADD_SESSION      = 'The SpiderNet is running. You cannot add FIX Session.';///SpiderNet正在執行中,不能新增FIX Session.';
  MAIN_FIX_RUNNING_CAN_NOT_MODIFY_SESSION   = 'The SpiderNet is running. You cannot modify FIX Session.';///SpiderNet正在執行中,不能修改FIX Session.';
  MAIN_FIX_RUNNING_CAN_NOT_DEL_SESSION      = 'The SpiderNet is running. You cannot delete FIX Session.';///SpiderNet正在執行中,不能刪除FIX Session.';
  MAIN_FIX_ADD_FAILED_ALREADY_EXISTS        = 'Failed to add a FIX Session[%s]. it has already existed.';///新增失敗!FIX Session:%s已存在.';
  MAIN_FIX_SELECT_MODIFY_SESSION            = 'Please select the FIX Session you would like to modify.';///請先選取欲修改的FIX線路.';
  MAIN_FIX_SELECT_DEL_SESSION               = 'Please select the FIX Session you would like to delete.';///請先選取欲刪除的FIX線路.';
  MAIN_FIX_NO_SESSION_STATUS_NOT_RUNNING    = 'SpiderNet is not running. Connection status:unknown.';///SpiderNet沒有執行,無法取得連線狀態.';
  MAIN_FIX_SEQ_NUM_MUST_LARGER_ZERO         = 'Incorrect format! Sequence number should be numerals larger than zero.';///格式錯誤! Sequence number是大於0的數字.';

  MAIN_SPEEDY_RUNNING_CANT_ADD_SESSION      = 'Speedy is running. You cannot add new session.';///Speedy 交易所連線程式正在執行中,不能新增線路.';
  MAIN_SPEEDY_NO_RIGHTS_ADD_SESSION         = 'You have no right to add new sessions.';///沒有新增線路權限.';
  MAIN_SPEEDY_NO_RIGHTS_MODIFY_SESSION      = 'You have no right to modify sessions.';///沒有修改線路的權限.';
  MAIN_SPEEDY_NO_RIGHTS_DEL_SESSION         = 'You have no right to delete sessions.';///沒有刪除線路的權限.';
  MAIN_SPEEDY_ADD_FAILED_ALREADY_EXISTS     = 'Adding new sessions failed. The session has already existed.';///新增交易所線路失敗!此線路已存在.';
  MAIN_SPEEDY_MODIFY_FAILED_ALREADY_EXISTS  = 'Modifying sessions failed.  The session has already existed.';///修改線路失敗!此線路已存在.';

  MAIN_SPEEDY_SELECT_MODIFY_SESSION         = 'Please select the session you would like to modify.';///請先選取欲修改的線路.';
  MAIN_SPEEDY_SELECT_DEL_SESSION            = 'Please select the session you would like to delete.';///請先選取欲刪除的線路.';
  MAIN_SPEEDY_SELECT_COPY_SESSION           = 'Please select the session you would like to copy.';///請先選取欲複製的線路.';
  MAIN_SPEEDY_SELECT_MONITOR_SESSION        = 'Please select the session you would like to monitor.';///請先選取欲監控的線路.';
  MAIN_SPEEDY_SELECT_STOP_SESSION           = 'Please select the session you would like to stop.';///請先選取欲停用的線路.';
  MAIN_SPEEDY_SELECT_ACTIVATE_SESSION       = 'Please select the session you would like to activate.';///請先選取欲啟用的線路.';
  MAIN_SPEEDY_ASK_DELETE_SESSION            = 'Do you want to delete the session port:';///是否要刪除線路 Port:';
  MAIN_SPEEDY_ASK_STOP_SESSION              = 'Are you sure you want to stop the session?';///確定要停用此線路?';
  MAIN_SPEEDY_ASK_ACTIVATE_SESSION          = 'Are you sure you want to activate the session?';///確定要啟用此線路?';
  MAIN_SPEEDY_ASK_STOP_SEL_SESSIONS         = 'Are you sure you want to stop all the selected sessions?';///確定要停用所有選定線路?';
  MAIN_SPEEDY_ASK_ACTIVATE_SEL_SESSIONS     = 'Are you sure you want to activate all the selected sessions?';///確定要啟用所有選定的線路?';
  MAIN_SPEEDY_SESSION_ALREADY_STOPED        = 'The session is already stopped.';///此線路已停用.';
  MAIN_SPEEDY_SESSION_NOT_STOPED            = 'The session has not been stopped yet.';///此線路尚未停用.';

  MAIN_ALARM_SEL_MODIFY_SETTING             = 'Please select the setting you would like to modify.';///請先選取欲修改的設定.';
  MAIN_ALARM_SEL_DELETE_SETTING             = 'Please select the setting you would like to delete.';///請先選取欲刪除的設定.';
  MAIN_ALARM_SEL_DELETE_SETTING_ASK         = 'Would you like to delete the warning setting?';///是否要刪除此警示設定?';

  MAIN_FTP_NORMAL_RESPONSE                = 'Normal response';///正常回應';
  MAIN_FTP_ILLEGAL_FILE_CODE              = 'Incorrect file code';///錯誤的File Code';
  MAIN_FTP_ILLEGAL_EOF_VALUE              = 'Incorrect EOF value';///錯誤的EOF值';
  MAIN_FTP_ILLEGAL_FILE_SIZE              = 'Incorrect file size';///錯誤的檔案大小';
  MAIN_FTP_TIMING_ERROR                   = 'Timing error';///錯誤的傳輸時機';
  MAIN_FTP_FILE_IS_NOT_READY              = 'The file is not ready.';///檔案尚未準備好';
  MAIN_FTP_FILE_IS_READY_BUT_EMPTY        = 'The file is empty.';///檔案已準備好但是是空檔';
  MAIN_FTP_ABORT_BY_INITIATOR             = 'Aborted by the initiator.';///被使用者中斷';
  MAIN_FTP_DUPLICATE_ACCESS_REQUEST       = 'Repetitive download request';///重複的下載請求';
  MAIN_FTP_ILLEGAL_SUBSYSTEM_NAME         = 'Incorrect subsystem code';///錯誤的系統別';
  MAIN_FTP_ILLEGAL_FUNCTION_CODE          = 'Incorrect function code';///錯誤的 Function Code';
  MAIN_FTP_ILLEGAL_MESSAGE_TYPE           = 'Incorrect message type';///錯誤的 Message Type';
  MAIN_FTP_ILLEGAL_MESSAGE_TIME           = 'Incorrect message time';///錯誤的 Message Time';
  MAIN_FTP_ILLEGAL_STATUS_CODE            = 'Incorrect status code';///錯誤的狀態碼';
  MAIN_FTP_ILLEGAL_SOURCE_ID              = 'Incorrect source ID';///錯誤的Source ID';
  MAIN_FTP_ILLEGAL_OBJECT_ID              = 'Incorrect object ID';///錯誤的ID';
  MAIN_FTP_ILLEGAL_BODY_LENGTH            = 'Incorrect text length';///錯誤的資料長度';
  MAIN_FTP_INTERNAL_ERROR                 = 'Internal error';///期交所內部錯誤';
  MAIN_FTP_CALL_COMPUTER_CENTER           = 'A blunder! Call computer center.';///重大錯誤!連絡期交所';
  MAIN_FTP_TMP_TOO_MANEY_REQ              = 'Frequent requests made';///檔案請求太頻繁';
  MAIN_FTP_WAITING                        = 'Waiting';///等待中';
  MAIN_FTP_JOB_EXISTS                     = 'Request for download existed';///下載請求已存在';
  MAIN_FTP_DOWNLOAD_FINISHED              = 'Finished';///完成';
  MAIN_TMP_FILE_NOT_EXISTS                = 'The file does not exist.';///檔案不存在';
  MAIN_TMP_READY_NOT_EXISTS               = 'The file .ready does not exist.';///.ready檔不存在';
  MAIN_FTP_UNKNOWN_ERROR                  = 'Unknown error';///不明錯誤';
{ Resource strings for Account input form  }
resourcestring
  MAIN_ACCOUNT_INPUT_LENGTH_SHOULD_7      = 'Your account number should be 7 digits in length.';///帳號長度為七碼.';
  MAIN_ACCOUNT_INPUT_FLAG_LEN_SHOULD_1    = 'The investor’s ID number is one digit in length.';///投資人身分碼長度為一碼.';

{ Resource strings for Alarm Setting form  }
resourcestring
  MAIN_ALARM_BEGIN_SHOULD_BEFORE_STOP_TIME = 'Start monitoring time should begin earlier than stop time.';///開始監控時間要早於停止監控時間';
  MAIN_ALARM_BEGIN_STOP_TIME_CANT_EQUAL    = 'Start/stop monitoring time should not be set at the same time.';///開始監控時間不能和停止監控時間相同';
  MAIN_ALARM_OVERLAPED_TIME_SETTING        = 'The time to be set monitoring and the time already set monitoring overlapped.';///'欲設定的監控時間和已設定的監控時間重疊';

{ Resource strings for App Info form  }
resourcestring
  MAIN_APP_INFO_FIXENGINE_NAME_NEED_LEN5   = 'The name for FIX Engine should be at least 5 digits in length.';///FIX Engine的名稱至少要五碼';
  MAIN_APP_INFO_INPUT_SC_PASSWORD          = 'Please enter your password for Speedy Center.';///請輸入您登入 Speedy Center 的密碼.';
  MAIN_APP_INFO_PWD_ERR_INPUT_SC_PASSWORD  = 'Incorrect password. Please enter your password for Speedy Center.';///密碼錯誤!請輸入您登入 Speedy Center 的密碼.';
  MAIN_APP_INFO_TFX_ACCOUNT_LEN_SHOULD_7   = 'The account for an Futures client should be 7 digits in length.';///期權客戶帳號的長度為7碼.(不填帳號,表示全刪)';
{ Resource strings for Event viewer form  }
resourcestring
  MAIN_EVENT_VIEWER_EVENTS                 = ' events';/// 個事件';

{ Resource strings for PVC Setting form  }
resourcestring
  MAIN_PVC_SETTING_ADD_NEW_SESSION         = 'New added sessions';///新增連線';
  MAIN_PVC_SETTING_MODIFY_SESSION          = 'Modifying sessions';//修改連線屬性';
	MAIN_PVC_SETTING_TSE_NORMAL              = '0:Normal(Exchange0)';///普通(交易所0)';
	MAIN_PVC_SETTING_TSE_FTP_RECV            = '1:FT(Recv Exchange1)';///1:檔案傳輸(接收,交易所1)';
	MAIN_PVC_SETTING_TSE_FTP_SEND            = '2:FT(Send Exchange1)';///2:檔案傳輸(傳送,交易所1)';
	MAIN_PVC_SETTING_TSE_EXECUTION           = '3:Execution Report(Exchange3)';///3:成交回報(交易所3)';
	MAIN_PVC_SETTING_TSE_NORMAL_ODD          = '4:Odd/Normal(Exchange2)'; ///4:零股/普通(交易所2)';
	MAIN_PVC_SETTING_TSE_LEND                = '5:Lend(Exchange4)';///標借(交易所4)';
	MAIN_PVC_SETTING_TSE_AUCTION             = '6:Auction(Exchange5)';///6:拍賣(交易所5)';
	MAIN_PVC_SETTING_TSE_TENDER              = '8:Tender(Exchange6)';///8:標購(交易所6)';
	MAIN_PVC_SETTING_TSE_NORMAL_POST         = '9:Post/Normal(Exchange7)';///9:定價/普通(交易所7)';
	MAIN_PVC_SETTING_TMP_ORDER               = '4:Order';///4:委託';
	MAIN_PVC_SETTING_TMP_CLEAR_MEMBER        = '8:Clearing Member';///8:結算會員';
	MAIN_PVC_SETTING_TMP_EXECUTION           = '3:Execution report';///3:成交回報';
	MAIN_PVC_SETTING_TMP_CM_FILLED           = '9:CM Fill execution';///9:結算成回';
	MAIN_PVC_SETTING_X25_FTP_RECV            = '1:FT(Recv)';///1:檔案傳輸';
	MAIN_PVC_SETTING_X25_FTP_SEND            = '2:FT(Send)';///2:檔案傳輸';
	MAIN_PVC_SETTING_X25_EXECUTION           = '3:Execution report';///3:成交回報';
	MAIN_PVC_SETTING_X25_ORDER               = '4:Order';///4:委託';
	MAIN_PVC_SETTING_X25_CM_ORDER            = '8:CM Order confirm';///8:結算會員委回';
	MAIN_PVC_SETTING_X25_CM_EXECUTION        = '9:CM Order filled';///9:結算會員成回';
	MAIN_PVC_SETTING_X25_MARKETDATA          = '10:Market data';///10:行情傳輸';
  MAIN_PVC_SETTING_PASSWORD_LEN_SHOULD_4   = 'The password for session should be 4 digits in length.';///線路密碼必須為四位數字.';
  MAIN_PVC_SETTING_BROKERID_LEN_SHOULD_7   = 'The ID for broker should be in 7 characters.';///期貨商代號必須為七碼.';
  MAIN_PVC_SETTING_BROKERID_FIRST_A_2_Z    = 'The first character for a broker’s ID should be in English alphabet.';///期貨商代號第一碼必須為A~Z.';
  MAIN_PVC_SETTING_BROKERID_LAST_6_NUM     = 'The last 6 characters for a broker’s  ID should be in numbers.';///期貨商代號後六碼必須為數字.';
  MAIN_PVC_SETTING_SESSIONID_3DIGI         = 'The Session ID should be a 3-digit number.';///SessionID必須是三位數.';
  MAIN_PVC_SETTING_PVC_ID_2DIGI            = 'PVC ID should be limited to 2 characters in length.';///PVC ID必須為兩碼.';
  MAIN_PVC_SETTING_PVC_BETWEEN_1_120       = 'PVC should be a number set between 1 and 120.';///PVC必須介於1~120之間.';
  MAIN_PVC_SETTING_SESSIONID_SHOULD_NUM    = 'The session ID should be numerals.';///SessionID必須為數字.';
  MAIN_PVC_SETTING_PVC_SHOULD_NUM          = 'PVC should be numerals.';///PVC必須為數字.';
  MAIN_PVC_SETTING_PORT_MUST_NUM           = 'Port should be numerals.';///Port必須為數字.';
  MAIN_PVC_SETTING_CM_MUST_LEN4            = 'The clearing member ID should be in 4 characters.';///結算會員代號必須為四碼.';
  MAIN_PVC_SETTING_CM_FIRST_MUST_A_2_Z     = 'The first character for a clearing member ID should be in English alphabet.';///結算會員代號第一碼必須為A~Z.';
  MAIN_PVC_SETTING_CM_LAST_3_MUST_NUM      = 'The last 3 characters for a clearing member ID should be in numerals.';///結算會員代號後三碼必須為數字.';
  MAIN_PVC_SETTING_TSEC_BROKERID_SHOULD_4  = 'The TSEC broker ID should be 4 characters in length.';///証券商代號必須為四碼.';
  MAIN_PVC_SETTING_TSEC_PVC_BETWEEN_1_99   = 'PVC should be numerals between 1 and 99.';///PVC必須介於1~99之間.';
  MAIN_PVC_SETTING_PORT_BETWEEN_0_65535    = 'Port should be numerals between 1 and 65535.';///Port必須介於0~65535間.';
  MAIN_PVC_SETTING_TCP_PORT_BETWEEN_1_65535= 'Port should be numerals between 1 and 65535.';///Port必須介於1~65535間.';
  MAIN_PVC_SETTING_WRONG_IP_ADDRESS        = 'Incorrect IP address';///錯誤的IP位址.';

{ Resource strings for Terminal Number Input form  }
resourcestring
  MAIN_TERM_TAIFEX_3DIGI                   = 'Enter term number of Future(3 digits)';///期權櫃號輸入(長度3碼)';
  MAIN_TERM_TAIFEX_2DIGI                   = 'Enter term number of Future(2 digits)';///期權櫃號輸入(長度2碼)';
  MAIN_TERM_TSEC_1DIGI                     = 'Enter term number of stocks(1 digit)';///證券櫃號輸入(長度1碼)';
  MAIN_TERM_TSEC_2DIGI                     = 'Enter term number of stocks(2 digit)';///證券櫃號輸入(長度2碼)';  ///
  MAIN_TERM_TERM_AND_ORDERID_COUNT         = '%d terms,Total %d OrderID';///%d櫃,共%d張委託';

{ Resource strings for New AE form (Account Executive) }
resourcestring
  MAIN_AE_ADD_AE                           = 'Add new account executive';///新增營業員';
  MAIN_AE_MODIFY_AE                        = 'Modify information of an AE';///修改營業員資料';
  MAIN_AE_MODIFY_PASSWORD                  = 'Modify the password';///修改密碼';
  MAIN_AE_TERM_POOL_API_VER_SETTING        = 'Version of API and share terms setting';///共用櫃號及API版本設定';
  MAIN_AE_DEL_AE_ASK                       = 'Do you want to delete the account?';///是否要刪除帳號:';
  MAIN_AE_SEL_DEL_AE                       = 'Please select the account code you would like to delete.';///請先選取欲刪除的帳號.';
  MAIN_AE_TOTAL_ORDERID_COUNT              = 'A total of %d orderIDs are produced';///共可產生[%d]個委託單號';
  MAIN_AE_API_VER_ALREADY_EXISTS           = 'The version has already existed.';///此版本已存在.';
  MAIN_AE_SEL_DEL_API_VER                  = 'Please select the version you would like to delete.';///請先選取欲刪除的版本.';
  MAIN_AE_ID_AT_LEAST_LEN_3                = 'Account codes should be at least 3 characters in length.';///帳號長度至少要三碼.';
  MAIN_AE_PASSWORD_AT_LEAST_LEN_5          = 'Passwords should be at least 5 characters in length.';///密碼長度至少要五碼.';
  MAIN_AE_INPUT_VALID_TAIFEX_BROKERID      = 'Please enter a valid TAIFEX broker ID.';///請填寫正確指定的期貨商代號.';
  MAIN_AE_INPUT_VALID_TSEC_BROKERID        = 'Please enter a valid TSEC broker ID.';///請填寫正確指定的証券商代號.';
  MAIN_AE_TAIFEX_ORDERID_LEN_5             = 'The TAIFEX order ID should be 5 characters in length.';///期交所單號長度必須是五碼.';
  MAIN_AE_TSEC_ORDERID_LEN_5               = 'The TSEC order ID should be 5 characters in length.';///證交所單號長度必須是五碼.';
  MAIN_AE_ORDERID_LEN_5                    = 'The order ID should be 5 characters in length.';///單號長度必須是五碼.';  ///
  MAIN_AE_INVALID_TAIFEX_ORDERID           = 'Incorrect TAIFEX order ID format';///錯誤的期交所單號格式.';
  MAIN_AE_INVALID_TSEC_ORDERID             = 'Incorrect TSEC order ID format';///錯誤的證交所單號格式.';
  MAIN_AE_INVALID_ORDERID                  = 'Incorrect order ID format';///錯誤的證交所單號格式.';
  MAIN_AE_TAIFEX_ORDERID_END_LESS_BEGIN    = 'The last TAIFEX order ID should be smaller than the one that starts.';///期交所最末的單號小於起始的單號.';
  MAIN_AE_TSEC_ORDERID_END_LESS_BEGIN      = 'The last TSEC order ID should be smaller than the one that starts.';///證交所最末的單號小於起始的單號.';
  MAIN_AE_ORDERID_END_LESS_BEGIN           = 'The last order ID should be smaller than the one that starts.';///最末的單號小於起始的單號.';  ///
  MAIN_AE_ORDERID_RANGE_OVERLAP            = 'The range of TAIFEX order ID and that of the AE[%s] overlapped.';/// 所設定的期交所單號範圍與營業員[%s]重疊.';
  MAIN_AE_TSEC_ORDERID_RANGE_OVERLAP       = 'The range of TSEC order ID and that of the AE[%s] overlapped.';///所設定的證交所單號範圍與營業員[%s]重疊.';
  MAIN_AE_INPUT_TAIFEX_TERM_LIST           = 'Please enter TAIFEX term list.';///請輸入期交所櫃號清單';
  MAIN_AE_INPUT_TSEC_TERM_LIST             = 'Please enter TSEC term list.';///請輸入證交所櫃號清單';
  MAIN_AE_TAIFEX_TERM_LEM_MUST             = 'The length of TAIFEX term must be';///期交所櫃號長度必須是';
  MAIN_AE_TSEC_TERM_LEM_MUST               = 'The length of TSEC term must be';///證交所櫃號長度必須是';

{ Resource strings for New FIX Session form}
resourcestring
  MAIN_FIX_SESSION_ADD                     = 'New added session';///新增連線';
  MAIN_FIX_SESSION_MODIFY                  = 'Modify the setting of session';///修改連線設定';
  MAIN_FIX_SESSION_INPUT_VALID_SENDER_ID   = 'Please enter correct Sender Comp ID.';///請輸入正確的 SenderCompID.';
  MAIN_FIX_SESSION_INPUT_VALID_TARGET_ID   = 'Please enter correct Target Comp ID.';///請輸入正確的 TargetCompID.';
  MAIN_FIX_SESSION_INTERVAL_MUST_NUM       = 'Heartbeat/Reconnect interval must be numerals.';///Heartbeat/Reconnect Intival必須是數字';
  MAIN_FIX_SESSION_INPUT_VALID_IP_PORT     = 'Enter at least a valid set of Host/Port.';///至少要輸入一組正確的Host/Port';

{ Resource strings for New SpeedyCenter User form}
resourcestring
  MAIN_USER_ADD_NEW                        = 'New added users';///新增使用者';
  MAIN_USER_MODIFY                         = 'Modify the information of users';///修改使用者資料';
  MAIN_USER_MODIFY_PASSWORD                = 'Modify passwords';///修改密碼';
  MAIN_USER_ID_AT_LEAST_LEN_5              = 'Account codes should be at least 5 characters in length.';///帳號長度至少要五碼.';
  MAIN_USER_PASSWORD_AT_LEAST_LEN_6        = 'Passwords should be at least 6 characters in length.';///密碼長度至少要六碼.';
  MAIN_USER_PASSWORD_CANT_SAME_AS_ID       = 'Passwords should not be the same as account codes.';///密碼不得與帳號相同.';
  MAIN_USER_PASSWORD_MUST_CONTAINS_NUM_CHAR= 'The characters contained in passwords should include both English alphabet and Roman numerals.';///密碼中的字元必須要包含英數字.';
  MAIN_USER_CANT_USE_WEAK_PASSWORD         = 'Please avoid a weak password for security reasons.';///不可使用易猜的密碼,如1234,6543,abcd,3333等.';
  MAIN_USER_PASSWORD_CONFIRM_NOT_EQUAL     = 'Passwords differ from [confirm passwords]';///密碼欄位與確認密碼欄位不同.';

resourcestring
  MAIN_MISS_GROUP_NAME                     = 'Missing group name.';
  MAIN_NEED_2_SERVER_IN_GROUP              = 'At least two servers in group.';
  MAIN_REJACT_ALL_ORDER_IN_Q               = 'Reject all %s orders in queue?';
implementation
end.

