//---------------------------------------------------------------------------
#ifndef LinkFrameH
#define LinkFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include "..\UFC\iniFile.h"
#include "..\UFC\PSocket.h"
//-----------------------------------------------------------------------------------------------------------------------
const int PVC_STATUS_UNKNOWN      		=  256;///< Unknow state.
const int PVC_STATUS_UNKNOWNPVC   		=  128;///< Unknow PVC.
//-----------------------------------------------------------------------------------------------------------------------
const int PVC_STATUS_WAIT_TRY        	=  19;
const int PVC_STATUS_TRY_OPEN        	=  18;
const int PVC_STATUS_WAIT_OPEN        	=  17;
const int PVC_STATUS_SLEEPING        	=  16;
const int PVC_STATUS_DISABLEING      	=  15;
const int PVC_STATUS_ENABLE     		=  14;
const int PVC_STATUS_DISABLE    		=  13;
const int PVC_STATUS_SUBSYS_ERR   		=  10;
const int PVC_STATUS_FINISHED      		=  9;
const int PVC_STATUS_BACKUP      		=  8; ///< Password error.
const int PVC_STATUS_PW_ERR       		=  7; ///< Password error.
const int PVC_STATUS_FCM_ERR      		=  6; ///< Invalidate Broker ID.
const int PVC_STATUS_AP_ERR       		=  5; ///< Invalidate AP Code.
//-----------------------------------------------------------------------------------------------------------------------
const int PVC_STATUS_BROKEN       		=  4; ///< Link Subsystem Broken.
//-----------------------------------------------------------------------------------------------------------------------
const int PVC_STATUS_OFFLINE_BUSY    	=  3;
const int PVC_STATUS_OFFLINE_READY      =  2;
//-----------------------------------------------------------------------------------------------------------------------
const int PVC_STATUS_ONLINE_BUSY   		=  1;
const int PVC_STATUS_ONLINE_READY       =  0;
const int PVC_STATUS_INIT               =  -1;
//-----------------------------------------------------------------------------------------------------------------------
extern bool IsValidIP( const AnsiString& IP );
//-----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    mtFutures = 0,
    mtOption  = 1,
    mtStock   = 2,
	mtOTC     = 3,
	mtTAIFEX  = 4,
	mtTW      = 5

} TMarket;
//---------------------------------------------------------------------------
typedef Set<TMarket,mtFutures,mtOTC> MarketSet;
//-----------------------------------------------------------------------------------------------------------------------
class TLineInfo : public TObject
{
private:	// User declarations
    int        FLink;
    int        FPVC;
    int        FAPCode;
    int        FRequestCount;
    int        FState;
    int        FLocalPort;
    TMarket    FMarket;
    AnsiString FPVCID;
    AnsiString FPassword;
    AnsiString FFCMNo;
    AnsiString FCMNo;
	AnsiString FCurrIP;
	TStringList* FIPList;
    TColor     FColor;
	bool       FIsTCP;
	int        FGroup;
	bool       FIsEnabled;
	int        FThroughput;
	bool       FIsTFXOffhour;
private:	// User declarations
	int        FInSeq;
	int        FOutSeq;
	int        FBackupIndex;
public:		// User declarations
	__fastcall TLineInfo( AnsiString FCMNo,
                          AnsiString Pwd,
                          int        Link,
						  int        PVCNo,
                          int        APCode,
						  TMarket    Market,
						  int        Group,
						  AnsiString CMorIP = "",
						  int        LocalPort = ANY_LOCAL_PORT,
						  AnsiString PVCID  = "00",
						  bool       Enable = true,
						  int        Throughput = 16 );
	int         __fastcall GetLink( void )       { return FLink; }
	int         __fastcall GetLocalPort( void )  { return FLocalPort; }
	int         __fastcall GetPVC( void )        { return FPVC; }
	int         __fastcall GetAPCode( void )     { return FAPCode; }
	int         __fastcall GetStateCode( void )  { return FState; }
	int         __fastcall GetGroup( void )      { return FGroup; }
	int         __fastcall GetThroughput( void ) { return FThroughput; }
	int         __fastcall GetInboundSeq( void ) { return FInSeq; }
	int         __fastcall GetOutboundSeq( void ){ return FOutSeq; }
	int         __fastcall GetBackupIndex( void ){ return FBackupIndex; }
	int         __fastcall GetIPCount( void );
	AnsiString& __fastcall GetPassword( void )   { return FPassword; }
	AnsiString& __fastcall GetBrokerID( void )   { return FFCMNo; }
	AnsiString& __fastcall GetCMID( void )       { return FCMNo; }
	AnsiString& __fastcall GetIP( int Index = -1 );
	AnsiString& __fastcall GetPVCID( void )      { return FPVCID; }
	String      __fastcall GetState( void );
	String      __fastcall GetApplication( void );
	String      __fastcall GetMessage( void );
	TMarket     __fastcall GetSystem( void )     { return FMarket; }
	TColor      __fastcall GetColor( bool Selected  );
	bool        __fastcall IsConnected( void );
	bool        __fastcall GetOffHour( void )    { return FIsTFXOffhour;}
	bool        __fastcall SetState( int State );
	void        __fastcall SetLink( int Link )      { FLink = Link;}
	void        __fastcall SetLocalPort( int Port ) { FLocalPort = Port;}
	void        __fastcall SetPVC( int PVC )        { FPVC  = PVC; }
	void        __fastcall SetGroup( int group )    { FGroup  = group; }
	void        __fastcall SetThroughput( int Val ) { FThroughput = Val; }
	void        __fastcall ClearState( void )       { FState = PVC_STATUS_INIT; }
	void        __fastcall SetPVCID( const AnsiString& PVCID ){ FPVCID  = PVCID; }
	void        __fastcall SetPassword( const AnsiString& Pwd){ FPassword = Pwd; }
	void        __fastcall SetBrokerID( const AnsiString& FCM){ FFCMNo = FCM; }
	void        __fastcall SetCMID( const AnsiString& CM )    { FCMNo = CM; }
	void        __fastcall SetInboundSeq( int Val ) { FInSeq = Val; }
	void        __fastcall SetOutboundSeq( int Val ){ FOutSeq = Val; }
	void        __fastcall SetBackupIndex( int Val ){ FBackupIndex = Val; }
	void        __fastcall SetIP( const AnsiString& IP );
	void        __fastcall SetAPCode( int AP )                { FAPCode = AP;  }
	void        __fastcall SetIsEnabled( bool TF )            { FIsEnabled = TF; }
	void        __fastcall SetOffHour( bool OffHour )         { FIsTFXOffhour = OffHour;}
	bool        __fastcall IsTCP( void )                      { return FIsTCP; }
	bool        __fastcall IsFIX( void );
	bool        __fastcall IsFTLine( void );
	bool        __fastcall IsConfirmLine( void );
	bool        __fastcall IsOrderLine( void );
	bool        __fastcall IsEnabled( void )                  { return FIsEnabled; }
};
//-----------------------------------------------------------------------------------------------------------------------
const int SESSION_STATUS_UNKNOWN = -1;
const int SESSION_STATUS_ONLINE  = 1;
const int SESSION_STATUS_BROKEN  = 2;
const int SESSION_STATUS_LISTEN  = 3;
const int SESSION_STATUS_STOP    = 0;
//-------------------------------------------------------------------------------------------------------------------------
class TFIXSession : public TObject
{
protected:
	AnsiString FVersion;
	AnsiString FSenderID;
	AnsiString FTargetID;
	AnsiString FSessionKey;
	AnsiString FDictonary;
	AnsiString FDictonaryFullPath;
	AnsiString FAppVersion;
	AnsiString FStartDay;
	AnsiString FEndDay;
	int        FInSeq;
	int        FOutSeq;
	int        FState;
	int 	   FStartTime;
	int 	   FEndTime;
	bool       FEnabled;
	bool       FPersistMessages;
	bool	   FMillisecondsInTimeStamp;
	bool       FResetOnLogon;
	bool       FVerifyMsgSeqNum;
private:
	void __fastcall UpdateSessionKey();
public:
	__fastcall TFIXSession( const char* Ver, const char* Sender, const char* Target );
	__fastcall TFIXSession( UFC::Section* iniSection );
	virtual __fastcall ~TFIXSession(){}
	const AnsiString& __fastcall GetFIXVersion()   { return FVersion;  }
	const AnsiString& __fastcall GetFIXAppVersion(){ return FAppVersion;  }
	const AnsiString& __fastcall GetDictonary()    { return FDictonary;}
	const AnsiString& __fastcall GetSenderCompID() { return FSenderID; }
	const AnsiString& __fastcall GetTargetCompID() { return FTargetID; }
	const AnsiString& __fastcall GetSessionID()    { return FSessionKey; }
	const AnsiString& __fastcall GetStartDay()     { return FStartDay;  }
	const AnsiString& __fastcall GetEndDay()       { return FEndDay;  }
	int  __fastcall GetStartTime()                 { return FStartTime;  }
	int  __fastcall GetEndTime()                   { return FEndTime;  }
	bool __fastcall GetPersistMessages()           { return FPersistMessages; }
	bool __fastcall GetMillisecondsInTimeStamp()   { return FMillisecondsInTimeStamp; }
	bool __fastcall GetResetOnLogon()              { return FResetOnLogon; }
	bool __fastcall GetVerifyMsgSeqNum()           { return FVerifyMsgSeqNum; }

	void  __fastcall SetVerifyMsgSeqNum( bool YorN){ FVerifyMsgSeqNum = YorN; }
	void  __fastcall SetResetOnLogon( bool YorN )  { FResetOnLogon = YorN; }
	void  __fastcall SetFIXVersion( const AnsiString& Ver );
	void  __fastcall SetFIXAppVersion( const AnsiString& Ver );
	void  __fastcall SetDictonaryFullPath( const AnsiString& Dict );
	void  __fastcall SetDictonary( const AnsiString& Ver );
	void  __fastcall SetSenderCompID( const AnsiString& Sender );
	void  __fastcall SetTargetCompID( const AnsiString& Target );
	void  __fastcall SetStartDay( const AnsiString& Day );
	void  __fastcall SetEndDay( const AnsiString& Day );
	bool  __fastcall operator ==( const TFIXSession& rhs )     { return IsSession( rhs.FSessionKey ); }
	bool  __fastcall IsSession( const AnsiString& Session ) { return (FSessionKey.AnsiCompare(Session) == 0 );}

	int   __fastcall GetInboundSeq()  { return FInSeq;}
	void  __fastcall SetInboundSeq( int Seq )  { FInSeq  = Seq;}

	int   __fastcall GetOutboundSeq() { return FOutSeq;}
	void  __fastcall SetOutboundSeq( int Seq ) { FOutSeq = Seq;}

	int   __fastcall GetState()                 { return FState; }
	void  __fastcall SetState( int State )      { FState = State;}
	bool  __fastcall IsEnabled()                { return FEnabled; }
	bool  __fastcall IsSame( const char* SID)   { return (FSessionKey.AnsiCompare( SID )==0);}
	void  __fastcall SetEnabled( bool Enabled ) { FEnabled = Enabled;}
	void  __fastcall SetStartTime( int hhmmss)  { FStartTime = hhmmss;  }
	void  __fastcall SetEndTime( int hhmmss )   { FEndTime = hhmmss;  }
	void  __fastcall SetPersistMessages( bool Persist )   { FPersistMessages = Persist;  }

	virtual UFC::Section* __fastcall CreateConfigSession() = 0;
};
//-------------------------------------------------------------------------------------------------------------------------
class TAcceptor : public TFIXSession
{
private:
	static int FAcceptorPort;
public:
	__fastcall TAcceptor( const char* Ver, const char* Sender, const char* Target );
	__fastcall TAcceptor( UFC::Section* iniSection );
	__fastcall ~TAcceptor( ){}
	virtual UFC::Section* __fastcall CreateConfigSession();
	static int  __fastcall GetAcceptorPort()           { return TAcceptor::FAcceptorPort; }
	static void __fastcall SetAcceptorPort( int Port ) { TAcceptor::FAcceptorPort = Port; }
};
//-------------------------------------------------------------------------------------------------------------------------
class TInitiator : public TFIXSession
{
private:
    int          FHeartbeatInterval;
    int          FReconnectInterval;
    int          FLogonTimeout;
    TStringList* FIPList;
    TList*       FPorts;
public:
	__fastcall TInitiator( const char* Ver, const char* Sender, const char* Target );
	__fastcall TInitiator( UFC::Section* iniSection );
    __fastcall ~TInitiator( );
    virtual UFC::Section* __fastcall CreateConfigSession();
    void        __fastcall AddHost( AnsiString NewIP, int Port );
    int         __fastcall HostCount() { return FIPList->Count; }
    void        __fastcall ClearHosts();
    void        __fastcall DeleteHosts( int index );
	int         __fastcall GetHostPort( int index );
    void        __fastcall SetHostPort( int index, int Port );
    AnsiString  __fastcall GetHostIP( int index );
    void        __fastcall SetHostIP( int index, AnsiString& NewIP );
    int  __fastcall GetHeartbeatInterval()          { return FHeartbeatInterval;}
    void __fastcall SetHeartbeatInterval( int sec ) { FHeartbeatInterval = sec; }
    int  __fastcall GetReconnectInterval()          { return FReconnectInterval;}
    void __fastcall SetReconnectInterval( int sec ) { FReconnectInterval = sec; }
    int  __fastcall GetLogonTimeout( )              { return FLogonTimeout;}
    void __fastcall SetLogonTimeout( int sec )      { FLogonTimeout = sec; }
};
//-------------------------------------------------------------------------------------------------------------------------
#ifndef DEF_FTP_STATUS_CODE
#define DEF_FTP_STATUS_CODE
//DocDef
const int FTP_NORMAL_RESPONSE           = 0;
const int FTP_ILLEGAL_FILE_CODE         = 10;
const int FTP_ILLEGAL_EOF_VALUE         = 11;
const int FTP_ILLEGAL_FILE_SIZE         = 12;
const int FTP_TIMING_ERROR              = 13;
const int FTP_FILE_IS_NOT_READY         = 14;
const int FTP_FILE_IS_READY_BUT_EMPTY   = 17;
const int FTP_ABORT_BY_INITIATOR        = 19;
const int FTP_DUPLICATE_ACCESS_REQUEST  = 79;
const int FTP_ILLEGAL_SUBSYSTEM_NAME    = 81;
const int FTP_ILLEGAL_FUNCTION_CODE     = 82;
const int FTP_ILLEGAL_MESSAGE_TYPE      = 83;
const int FTP_ILLEGAL_MESSAGE_TIME      = 84;
const int FTP_ILLEGAL_STATUS_CODE       = 85;
const int FTP_ILLEGAL_SOURCE_ID         = 86;
const int FTP_ILLEGAL_OBJECT_ID         = 87;
const int FTP_ILLEGAL_BODY_LENGTH       = 88;
const int FTP_INTERNAL_ERROR            = 89;
const int FTP_CALL_COMPUTER_CENTER      = 99;
const int FTP_TMP_TOO_MANEY_REQ         = 121;


const int SFTP_ILLEGAL_FILE_CODE         = 110;
const int SFTP_ILLEGAL_SEQ_VALUE         = 113;
const int SFTP_FILE_IS_NOT_READY         = 114;
const int SFTP_FILE_IS_READY_BUT_EMPTY   = 117;
const int SFTP_FILE_BROKER_NOT_READY     = 120;

//CustDef
const int FTP_WAITING                   = -1;
const int FTP_JOB_EXISTS                = 198;
const int FTP_UNKNOWN_ERROR             = 199;
const int FTP_DOWNLOAD_FINISHED         = 200;


const int TMP_FILE_NOT_EXISTS           = 4;
const int TMP_READY_NOT_EXISTS          = 5;
#endif
//-------------------------------------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------------------------------------
