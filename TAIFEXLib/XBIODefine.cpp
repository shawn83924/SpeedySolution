//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "XBIODefine.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int        OS_ACCEPT_ORDER          = 10;      // Open/Switch values.
int        OS_OPEN_MARKET           = 30;
int        OS_REJECT_ORDER          = 100;
int        OS_SHOUTDOWN_SYS         = 700;

//int        PVC_BUSY_RETRY_TIMES     = 50000;      // X.25\uFFFDs\uFFFDu\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD,\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD
int        PVC_MAX_CONTROLBUFFER    = 1024;    //
int        PVC_MAX_PACKET_SIZE      = 256;     // \uFFFD\uFFFD\uFFFD\uFFFD\uFFFDn\uFFFDM\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDq\uFFFDHX.25\uFFFD\uFFFDPacket Size \uFFFD\uFFFD\uFFFDP.
//int        MAX_MESSAGE_SIZE         = 1025;    // \uFFFD\uFFFD\uFFFDj\uFFFD\uFFFD\uFFFDq\uFFFD\uFFFDSize.
long       IDLEHANDSHAKE_TIME_XBIO4 = 62L;    // XBIO4 \uFFFD\uFFFD\uFFFDm\uFFFDh\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDn\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDT\uFFFD{\uFFFDs\uFFFDu\uFFFDT\uFFFD\uFFFD( Default 62 sec ).
long       IDLEHANDSHAKE_TIME_XBIO3 = 58L;    // XBIO3 \uFFFD\uFFFD\uFFFDm\uFFFDh\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDn\uFFFDo\uFFFDX\uFFFDT\uFFFD{\uFFFDs\uFFFDu\uFFFDT\uFFFD\uFFFD( Default 58 sec ).
//long       LINKSUBSYSTEM_TIMEOUT    = 180L;     // \uFFFDs\uFFFDu\uFFFDl\uFFFDt\uFFFD\uFFFD\uFFFD\uFFFDTimeout\uFFFD\uFFFD( Default 180 sec ). 36
long       ORDERSUBSYSTEM_TIMEOUT   = 90L;     // \uFFFDe\uFFFDU\uFFFD\uFFFD\uFFFD\uFFFDTimeout\uFFFD\uFFFD( Default 90 sec ).      18
long       TAIFEX_INTERNAL_TIMEOUT  = 12L;     // \uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDB\uFFFDz\uFFFD\uFFFDTimeout\uFFFD\uFFFD.
long       TAIFEX_MIGOMSG_TIMEOUT   = 20L;     // \uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDB\uFFFDzMigo Message\uFFFD\uFFFDTimeout\uFFFD\uFFFD.
//long       TXIFEX_PULL_INTERVAL     = 100L;      // \uFFFDC\uFFFDj\uFFFDh\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDn\uFFFD\uFFFD\uFFFDd\uFFFD\uFFFD\uFFFDf\uFFFD\uFFFD\uFFFDO\uFFFD_\uFFFD\uFFFD\uFFFDe\uFFFD}\uFFFD\uFFFD\uFFFDq\uFFFD\uFFFD\uFFFDT\uFFFD\uFFFD(L010).
int        TXIFEX_PULL_TIMES        = 12;      // \uFFFD\uFFFD\uFFFDd\uFFFDh\uFFFD\uFFFD\uFFFD\uFFFD"\uFFFD}\uFFFD\uFFFD\uFFFDq\uFFFD\uFFFD\uFFFDT\uFFFD\uFFFD"\uFFFD\uFFFD\uFFFDD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDe"\uFFFD}\uFFFD\uFFFD\uFFFDq\uFFFD\uFFFD\uFFFDT\uFFFD\uFFFD".
                                              // TXIFEX_PULL_INTERVAL X TXIFEX_PULL_TIMES\uFFFDn\uFFFD\uFFFD\uFFFD\uFFFD60\uFFFD\uFFFD.
BOOL       TAP_TEST_MODE = 0;                 // \uFFFDO\uFFFD_\uFFFDOTAP Client\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD( TRUE\uFFFDO\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD ).
long       TAP_MESSAHE_QUEUE_TIMEOUT = 330L;  // \uFFFDW\uFFFDL\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDTAP Client\uFFFDn\uFFFDj\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDeBuffer\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDTAP Server.
int        MAX_PVC = 240;                     //
int        XBIO3_MESSAGEQUEUE_SIZE = 14;
UFC::AnsiString PVC_RESET_SUBSCRIBER    = "TFX.UNI.PROD.XTC.RST";    // Migo message for PVC reset.
UFC::AnsiString PVC_BACKUP_SUBSCRIBER   = "TFX.UNI.PROD.XTC.BAK";   // \uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDMigo Subject.
UFC::AnsiString PVC_STATE_PUBLISHER     = "TFX.OPT.PROD.XBIO.GUI";
UFC::AnsiString PVC_STATE_SUBSCRIBER    = "TFX.OPT.PROD.GUI.XBIO";
UFC::AnsiString PCM_RESET_SUBSCRIBER    = "TFX.OPT.PROD.PCM";    // \uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFDMigo Subject.
UFC::AnsiString DB_PUBLISHER_SUBJECT    = "TFX.OPT.PROD.DBS.LGN";    // Query DB connection parameters.
UFC::AnsiString DB_SUBSCRIBER_SUBJECT   = "TFX.OPT.PROD.ADS.DBS.LGI";   // Load PROD table event.
UFC::AnsiString OPEN_SWITCH_SUBSCRIBER  = "TFX.UNI.PROD.SCH";
UFC::AnsiString OQ_QUERY_SUBSCRIBER     = "TFX.OPT.PROD.QRY.XBIO";     // Receive R100,R020 from Qif010.
UFC::AnsiString OQ_QUERY_PUBLISHER      = "TFX.OPT.PROD.XBIO.QRY";      // Publish R090,R010 to Qif010.
UFC::AnsiString Q_REQUEST_SUBSCRIBER    = "TFX.OPT.PROD.REQ.XBIO";    // for R070
UFC::AnsiString Q_REQUEST_PUBLISHER     = "TFX.OPT.PROD.XBIO.REQ";     // for R070
UFC::AnsiString DB_SERVER   			   = "OPTIONSDB1";
UFC::AnsiString DB_NAME     			   = "apusr1";
UFC::AnsiString DB_USERNAME 			   = "apusr1";
UFC::AnsiString DB_PASSWORD 			   = "options";
//----------------------------------------------------------------------------------------------------------------------
void LoadXBIOConfig( char* FileName )
{
	ConfigReader cfg( FileName );
	OS_ACCEPT_ORDER           = cfg.GetValueByName("OS_ACCEPT_ORDER").ToInt();
	OS_OPEN_MARKET            = cfg.GetValueByName("OS_OPEN_MARKET").ToInt();
	OS_REJECT_ORDER           = cfg.GetValueByName("OS_REJECT_ORDER").ToInt();
	OS_SHOUTDOWN_SYS          = cfg.GetValueByName("OS_SHOUTDOWN_SYS").ToInt();
	PVC_BUSY_RETRY_TIMES      = cfg.GetValueByName("PVC_BUSY_RETRY_TIMES").ToInt();
	PVC_MAX_CONTROLBUFFER     = cfg.GetValueByName("PVC_MAX_CONTROLBUFFER").ToInt();
    PVC_MAX_PACKET_SIZE       = cfg.GetValueByName("PVC_MAX_PACKET_SIZE").ToInt();
    MAX_MESSAGE_SIZE          = cfg.GetValueByName("MAX_MESSAGE_SIZE").ToInt();
    IDLEHANDSHAKE_TIME_XBIO4  = cfg.GetValueByName("IDLEHANDSHAKE_TIME_XBIO4").ToUInt();
    IDLEHANDSHAKE_TIME_XBIO3  = cfg.GetValueByName("IDLEHANDSHAKE_TIME_XBIO3").ToUInt();
    LINKSUBSYSTEM_TIMEOUT     = cfg.GetValueByName("LINKSUBSYSTEM_TIMEOUT").ToUInt();
    ORDERSUBSYSTEM_TIMEOUT    = cfg.GetValueByName("ORDERSUBSYSTEM_TIMEOUT").ToUInt();
    TAIFEX_INTERNAL_TIMEOUT   = cfg.GetValueByName("TAIFEX_INTERNAL_TIMEOUT").ToUInt();
    TAIFEX_MIGOMSG_TIMEOUT    = cfg.GetValueByName("TAIFEX_MIGOMSG_TIMEOUT").ToUInt();
    TXIFEX_PULL_INTERVAL      = cfg.GetValueByName("TXIFEX_PULL_INTERVAL").ToUInt();
    TXIFEX_PULL_TIMES         = cfg.GetValueByName("TXIFEX_PULL_TIMES").ToInt();
    TAP_TEST_MODE             = cfg.GetValueByName("TAP_TEST_MODE").ToInt();
    TAP_MESSAHE_QUEUE_TIMEOUT = cfg.GetValueByName("TAP_MESSAHE_QUEUE_TIMEOUT").ToUInt();
    MAX_PVC 				  = cfg.GetValueByName("MAX_PVC").ToInt();
    XBIO3_MESSAGEQUEUE_SIZE   = cfg.GetValueByName("XBIO3_MESSAGEQUEUE_SIZE").ToInt();
    PVC_RESET_SUBSCRIBER      = cfg.GetValueByName("PVC_RESET_SUBSCRIBER");
    PVC_BACKUP_SUBSCRIBER     = cfg.GetValueByName("PVC_BACKUP_SUBSCRIBER");
    PVC_STATE_PUBLISHER       = cfg.GetValueByName("PVC_STATE_PUBLISHER");
	PVC_STATE_SUBSCRIBER      = cfg.GetValueByName("PVC_STATE_SUBSCRIBER");
    PCM_RESET_SUBSCRIBER      = cfg.GetValueByName("PCM_RESET_SUBSCRIBER");
    DB_PUBLISHER_SUBJECT      = cfg.GetValueByName("DB_PUBLISHER_SUBJECT");
    DB_SUBSCRIBER_SUBJECT     = cfg.GetValueByName("DB_SUBSCRIBER_SUBJECT");
    OPEN_SWITCH_SUBSCRIBER    = cfg.GetValueByName("OPEN_SWITCH_SUBSCRIBER");
    OQ_QUERY_SUBSCRIBER       = cfg.GetValueByName("OQ_QUERY_SUBSCRIBER");
    OQ_QUERY_PUBLISHER        = cfg.GetValueByName("OQ_QUERY_PUBLISHER");
    Q_REQUEST_SUBSCRIBER      = cfg.GetValueByName("Q_REQUEST_SUBSCRIBER");
    Q_REQUEST_PUBLISHER       = cfg.GetValueByName("Q_REQUEST_PUBLISHER");
	DB_SERVER       		  = cfg.GetValueByName("DB_SERVER");
	DB_NAME         		  = cfg.GetValueByName("DB_NAME");
	DB_USERNAME     		  = cfg.GetValueByName("DB_USERNAME");
	DB_PASSWORD     		  = cfg.GetValueByName("DB_PASSWORD");

    printf( "---------------------- Configuration --------------------------\n" );
    printf( "OS_ACCEPT_ORDER           = %d\n",OS_ACCEPT_ORDER );
	printf( "OS_OPEN_MARKET            = %d\n",OS_OPEN_MARKET );
	printf( "OS_REJECT_ORDER           = %d\n",OS_REJECT_ORDER );
	printf( "OS_SHOUTDOWN_SYS          = %d\n",OS_SHOUTDOWN_SYS );
	printf( "PVC_BUSY_RETRY_TIMES      = %d\n",PVC_BUSY_RETRY_TIMES );
	printf( "PVC_MAX_CONTROLBUFFER     = %d\n",PVC_MAX_CONTROLBUFFER );
    printf( "PVC_MAX_PACKET_SIZE       = %d\n",PVC_MAX_PACKET_SIZE );
    printf( "MAX_MESSAGE_SIZE          = %d\n",MAX_MESSAGE_SIZE );
    printf( "IDLEHANDSHAKE_TIME_XBIO4  = %ld\n",IDLEHANDSHAKE_TIME_XBIO4 );
    printf( "IDLEHANDSHAKE_TIME_XBIO3  = %ld\n",IDLEHANDSHAKE_TIME_XBIO3 );
    printf( "ORDERSUBSYSTEM_TIMEOUT    = %ld\n",ORDERSUBSYSTEM_TIMEOUT );
    printf( "TAIFEX_INTERNAL_TIMEOUT   = %ld\n",TAIFEX_INTERNAL_TIMEOUT );
    printf( "TAIFEX_MIGOMSG_TIMEOUT    = %ld\n",TAIFEX_MIGOMSG_TIMEOUT );
    printf( "TXIFEX_PULL_INTERVAL      = %ld\n",TXIFEX_PULL_INTERVAL );
    printf( "TXIFEX_PULL_TIMES         = %d\n",TXIFEX_PULL_TIMES );
    printf( "TAP_TEST_MODE             = %d\n",TAP_TEST_MODE );
    printf( "TAP_MESSAHE_QUEUE_TIMEOUT = %ld\n",TAP_MESSAHE_QUEUE_TIMEOUT );
    printf( "MAX_PVC                   = %d\n",MAX_PVC );
    printf( "XBIO3_MESSAGEQUEUE_SIZE   = %d\n",XBIO3_MESSAGEQUEUE_SIZE);
    printf( "PVC_RESET_SUBSCRIBER      = %s\n",PVC_RESET_SUBSCRIBER.c_str());
    printf( "PVC_BACKUP_SUBSCRIBER     = %s\n",PVC_BACKUP_SUBSCRIBER.c_str());
    printf( "PVC_STATE_PUBLISHER       = %s\n",PVC_STATE_PUBLISHER.c_str());
    printf( "PVC_STATE_SUBSCRIBER      = %s\n",PVC_STATE_PUBLISHER.c_str());
    printf( "PCM_RESET_SUBSCRIBER      = %s\n",PCM_RESET_SUBSCRIBER.c_str());
    printf( "DB_PUBLISHER_SUBJECT      = %s\n",DB_PUBLISHER_SUBJECT.c_str());
    printf( "DB_SUBSCRIBER_SUBJECT     = %s\n",DB_SUBSCRIBER_SUBJECT.c_str());
    printf( "OPEN_SWITCH_SUBSCRIBER    = %s\n",OPEN_SWITCH_SUBSCRIBER.c_str());
    printf( "OQ_QUERY_SUBSCRIBER       = %s\n",OQ_QUERY_SUBSCRIBER.c_str());
    printf( "OQ_QUERY_PUBLISHER        = %s\n",OQ_QUERY_PUBLISHER.c_str());
    printf( "Q_REQUEST_SUBSCRIBER      = %s\n",Q_REQUEST_SUBSCRIBER.c_str());
    printf( "Q_REQUEST_PUBLISHER       = %s\n",Q_REQUEST_PUBLISHER.c_str());
	printf( "DB_SERVER                 = %s\n",DB_SERVER.c_str());
	printf( "DB_NAME                   = %s\n",DB_NAME.c_str());
	printf( "DB_USERNAME               = %s\n",DB_USERNAME.c_str());
	printf( "DB_PASSWORD               = %s\n",DB_PASSWORD.c_str());
    printf( "--------------------------------------------------------------\n" );
}
//----------------------------------------------------------------------------------------------------------------------

