#ifndef _TTaifexConnection_H_
#define _TTaifexConnection_H_
//------------------------------------------------------------------------------
#include <iniFile.h>
#include <TMPTypes.h>
#include <TMPMessage.h>
//------------------------------------------------------------------------------
#include "../MessageDeliver/TMdMessage.h"
#include "../MessageDeliver/TMdTransport.h"
#include "../MessageDeliver/TMdListener.h"
//------------------------------------------------------------------------------
#include "../../lib/OrderMessage/TNetworkID.h"
#include "../../lib/OrderMessage/TNewOrderMessage.h"
#include "../../lib/OrderMessage/TReplaceOrderMessage.h"
#include "../../lib/OrderMessage/TCancelOrderMessage.h"
#include "../../lib/OrderMessage/TQuoteMessage.h"
#include "../../lib/OrderMessage/TQuoteCancelMessage.h"
#include "../../lib/OrderMessage/TQuoteRequestMessage.h"
#include "../../lib/OrderMessage/TExecutionReportMessage.h"
#include "../../lib/OrderMessage/TOrderStatusRequest.h"
//------------------------------------------------------------------------------
#include "../EmbadedResource.h"
#include "../../TMPOrderConnection/MessageDataFormat.h"
//------------------------------------------------------------------------------
typedef enum
{
	edNewExecution       = 0,
	edPossibleDuplicate  = 1,
	edSpeedyGenerate     = 2

} ExecDup;
//------------------------------------------------------------------------------
typedef enum
{
	lrOk           = 1,
	lrFailed       = 0,
	lrAlreadyLogon = -1

} LogonResult;
//------------------------------------------------------------------------------
typedef enum
{
    mlEnglish = 0, ///< Error message in English
    mlChinese = 1  ///< Error message in Chinese

} MessageLanguage;
//------------------------------------------------------------------------------
typedef enum
{
	ctSendOrder     = 0, ///< Can place order only.(New/Cancel/Replace/Quote)
	ctRecvExecution = 1, ///< Only receive execution reports.
	ctBoth          = 2  ///< Can place order and receive executions.

} ConnectionType;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    rmFutures = 0,
    rmOption  = 1,
    rmTSE     = 2,
    rmOTC     = 3,
    rmAll     = 4

} RecoverMarket;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
	rdConfirm = 0, ///< Recover order confirmation
	rdFill    = 1, ///< Recover order filled execution.
	rdBoth    = 2

} RecoverDataType;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
	smtNew          = 0,
	smtCancel       = 1,
	smtReplace      = 2,
	smtQuoteNew     = 3,
	smtQuoteCancel  = 4,

} SendMessageType;
//------------------------------------------------------------------------------
class IOrderConnectionEventListener
{
public:
    virtual ~IOrderConnectionEventListener( void ) {}
    virtual void OnConnected( void ) = 0;
    virtual void OnDisconnected( void ) = 0;
	virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID ) = 0;
	virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup ) = 0;
    virtual void OnRecoverFinished( int Count ) = 0;
};
//------------------------------------------------------------------------------
class ISendMessageListener
{
public:
	virtual ~ISendMessageListener( void ) {}
	virtual void OnBeforeSend( nsOrderMessageDefine::MarketEnum Market,
							   SendMessageType Type,
							   TBaseMessage* Msg,
							   bool& CanSend,
							   UFC::AnsiString& RejectMsg ) = 0;
};
//------------------------------------------------------------------------------
class LogonThread : public UFC::PThread
{
private:
    TMdTransport*   FTransport;
	UFC::PEvent*    FLogonEvent;
	int				FVersion;
	UFC::AnsiString FSubject;
	UFC::AnsiString FKey;
	UFC::AnsiString FID;
	UFC::AnsiString FPasswd;
	UFC::AnsiString FAccount;
public:
	LogonThread( TMdTransport* Transport,
				 UFC::PEvent*  LogonEvent,
				 int           Version,
                 const UFC::AnsiString& Subject,
                 const UFC::AnsiString& Key ,
                 const UFC::AnsiString& ID ,
				 const UFC::AnsiString& Passwd,
                 const UFC::AnsiString& Account );

protected:
    virtual void Execute( void );
};
class TTaifexTMPConnection ;
//------------------------------------------------------------------------------
class LogoffThread : public UFC::PThread
{
private:
	TTaifexTMPConnection*   FConnection;
public:
	LogoffThread( TTaifexTMPConnection* Connection );
	void Execute( void );
};
//------------------------------------------------------------------------------
const long CMD_LOGON_REQUEST                = 10001;
const long CMD_LOGON_REPLY                  = 10002;
const long CMD_RECOVER_REQUEST              = 10003;
const long CMD_RECOVER_FINISHED             = 10004;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_RECOVER;
extern const UFC::AnsiString SUBJECT_ADMIN;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_ORDER_FUT;
extern const UFC::AnsiString SUBJECT_ORDER_OPT;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_CONFIRM_FUT;
extern const UFC::AnsiString SUBJECT_CONFIRM_OPT;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_FILL_FUT;
extern const UFC::AnsiString SUBJECT_FILL_OPT;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_FT_FUT;
extern const UFC::AnsiString SUBJECT_FT_OPT;
//------------------------------------------------------------------------------
class TTaifexTMPConnection : public MAppListener, public MessageListener
{
typedef void (TTaifexTMPConnection::*MessageHandlerFunc)( MTree* Msg );
private:
	HINSTANCE                       FInstance;
	HMODULE                         FResourceInstance;
	UFC::AnsiString                 FAppName;
	UFC::AnsiString                 FID;
	UFC::AnsiString                 FUniquekey;
	UFC::AnsiString                 FTAIFEXBrokerID;
	UFC::AnsiString                 FCMID;
	int								FCMIDIndex;
	int								FBrokerIndex;
	int                             FAdmin;
	int                             FCurrentConnectionID;
	int                             FVersion;
	bool                            FIsLogon;
	bool                            FTriggerExec;
	bool                            FCanSendOrder;
	UFC::PEvent                     FLogonEvent;
	UFC::PCriticalSection           FExecCS;
private:
	MessageHandlerFunc              FOnOptConfirm;
	MessageHandlerFunc              FOnFutConfirm;
	MessageHandlerFunc              FOnOptFill;
	MessageHandlerFunc              FOnFutFill;
	MessageHandlerFunc              FOnAdminMsg;
	MessageHandlerFunc              FOnRecoverMsg;
	UFC::PHashedList<UFC::AnsiString, MessageHandlerFunc*> FCallbackFuncs;
private:
	UFC::PStringHashedSet           FFUTExecIDSet;
	UFC::PStringHashedSet           FOPTExecIDSet;
	UFC::PCriticalSection           FFUTExecIDCS;
	UFC::PCriticalSection           FOPTExecIDCS;
private:
	IOrderConnectionEventListener*  FListener;
	TMdTransport*                   FTransport;
	TMdListener*                    FAdminListener;
	TMdListener*                    FRecoverListener;
	TMdListener*                    FFutConfirmListener;
	TMdListener*                    FOptConfirmListener;
	TMdListener*                    FFutExecuteListener;
	TMdListener*                    FOptExecuteListener;
private:
	UFC::UiniFile*                  FTAIFEXError;
	UFC::UiniFile*                  FFUTSymbol;
	UFC::UiniFile*                  FOPTSymbol;
	ISendMessageListener*           FSendEventListener;
private: ///< Impelment interface MAppListener
	virtual void                    OnMAppConnected( void );
	virtual void                    OnMAppDisconnected( void );
	virtual void                    OnMAppError( PMAppError Error );
public:  ///< Impelment interface MessageListener
	virtual void                    OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* pTree );
private:
	UFC::AnsiString                 CreateFileFromMNode( MTree* Data, const char* Dir, const char* Name );
	UFC::AnsiString                 CreateFileFromResource( const char* Dir, DWORD ResourceID, const char* ResourceType );
	void                            CreateSymbolFile( MTree* Data );
	void							CreateFormatFiles( void );
	void                            CreateReportListener( const char* ListenKey );
private:
	BOOL                            SendToOrderServer( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& Data, TMP::TMPMessage& Order, long NID );
	bool                            TrigerBeforeSendEvent( nsOrderMessageDefine::MarketEnum,SendMessageType,nsOrderMessageDefine::CxlRejResponseToEnum RejTo, TBaseMessage* Msg );
	void                            Reject( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo, const UFC::AnsiString& Text, const UFC::AnsiString& ErrMsg, TExecutionReportMessage* Msg );
	void 							Reject( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo, const UFC::AnsiString& ErrMsg, TBaseMessage* Msg );
	void 							RejectByAPI( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo, TBaseMessage* Msg, int  );
private:///< for place order.
	const char*                     GetExchangePositionEffect( nsOrderMessageDefine::PositionEffectEnum pe );
	unsigned short                  GetBrokerIDIndex( TBaseMessage* Msg );
	int                             FillOrderBase(  nsOrderMessageDefine::MarketEnum Market, TBaseMessage* Msg, TMP::TMPMessage& R01 );
private:///< For Filled execution.
	void                            FillExecution( nsOrderMessageDefine::MarketEnum Market,TMP::TMPMessage& R02,TExecutionReportMessage* ExecutionReport );
private:///< For Confirm execution.
	bool                            IsExecutionDup( nsOrderMessageDefine::MarketEnum  Market, const UFC::AnsiString& ExecID );
	void                            FillRejectExecution( nsOrderMessageDefine::MessageTypeEnum MessageType, TMP::TMPMessage& R03,UFC::AnsiString StatusCode,UFC::AnsiString ErrMsg,TExecutionReportMessage* ExecutionReport );
	void                            FillExecution( nsOrderMessageDefine::MessageTypeEnum MessageType,nsOrderMessageDefine::MarketEnum Market,TMP::TMPMessage& R02,TExecutionReportMessage* ExecutionReport );
private:
	void                            ReceiveAdminMessage( MTree* pTree );
	void                            ReceiveFutConfirmMessage( MTree* pTree );
	void                            ReceiveOptConfirmMessage( MTree* pTree );
	void                            ReceiveFutExecuteMessage( MTree* pTree );
	void                            ReceiveOptExecuteMessage( MTree* pTree );
	void                            ReceiveRecoverExecution( MTree* pTree );
	void                            RemoveListener( TMdListener*& Listener );
	BOOL                            ParseUserID( const UFC::AnsiString& String, UFC::AnsiString& AE, UFC::AnsiString& Data );
public:
	int                             GetPricePrecision( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& Symbol );
	void                            TrigerOnExecutionReport( TExecutionReportMessage* ExecutionReport, ExecDup PosDup );
	void                            GenerateNID( TBaseMessage* Msg );
	int                             GenerateNID( nsOrderMessageDefine::MessageTypeEnum Type ) { return TNetworkID::CreateClientOrderID( FCurrentConnectionID, Type ); }
public: ///< Public functions for order connection.
	/**
	 * Try to connect a Speedy Order Gateway.
     *  @param RemoteIP IP addreess.
     *  @Param RemotePort Port.(Default 23456)
     */
    void                            Connect( const char* RemoteIP, long RemotePort );
    /**
     * Try to close a Speedy Order Gateway connection.
     *
     */
    void                            Disconnect( void );
    /**
     * Logon to a Speedy Order Gateway.
     *  @Param ID ID of the connection.
     *  @Param PASSWD Password of this ID.
     *  @Param ACCOUNT Trading account.
     *  @Param RecvExec TRUE for receiving executions, FALSE for send order only connection.
     *
	 */
	void                            Logon( int Version, const char* ID, const char* PASSWD, const char* ACCOUNT, ConnectionType Type = ctBoth );
    /**
     * Logoff the Speedy Order Gateway.
     *
     */
    void                            Logoff( void );
    /**
	 * Recover executions from BeginTime to now.
     *  @Param BeginTime Begin time HHMMSS
     *  @Param Type Order confirmation ,fill execution or both.
     *  @Param Market Futures,Options,TSEor OTC
     */
    void                            Recover( const char* BeginTime, RecoverDataType Type, RecoverMarket Market  );
    /**
     * Recover executions from BeginTime to EndTime.
     *  @Param BeginTime Begin time HHMMSS
     *  @Param EndTime End time HHMMSS
     *  @Param Type Order confirmation ,fill execution or both.
     *  @Param Market Futures,Options,TSEor OTC
     */
    void                            Recover( const char* BeginTime, const char* EndTime,RecoverDataType Type, RecoverMarket Market  );
public:  ///< Public functions for order placement.
    /**
     * To place a new order
     *  @param Msg New order message.
     */
    void                            NewOrder( TNewOrderMessage* Msg );
    /**
     * To replace an order.
     * In TAIFEX, you can reduce order qty only, not support increasing qty or changing price.
     *  @param Msg replace order message.
     */
    void                            ReplaceOrder( TReplaceOrderMessage* Msg );
    /**
     * To cancel an order.
     *  @param Msg Cancel order message.
     */
    void                            CancelOrder( TCancelOrderMessage* Msg );
    /**
     * To place a quote. ( for Market maker only)
     *  @param Msg quote message.
     */
    void                            Quote( TQuoteMessage* Msg );
    /**
     * To cancel a quote.
     *  @param Msg quote cancel message.
     */
    void                            QuoteCancel( TQuoteCancelMessage* Msg );
    /**
     * Quote request.
     *  @param Msg Quote request message.
     */
	void                            QuoteRequest( TQuoteRequestMessage* Msg );
    /**
     * To ask order status.
	 *  @param Msg order status request message.
	 */
	void                            OrderStatusRequest( TOrderStatusRequest* Msg );
	/**
	 * To request a file.(for TAIFEX TCP/IP TMP only)
	 *  @param Market Futures or Options.
	 *  @param IsMorningSession TRUE fo morning trading session, FALSE for afternoon trading session.
	 *  @Param ReqID TMP FTP request ID.
	 *  @Param Body Data body for request file.
	 */
	void                            RequestFile( nsOrderMessageDefine::MarketEnum Market, BOOL IsMorningSession, int ReqID, const char* Body );
public: ///< Functions to setup order connection.
	/**
	 * Write logs to given file
	 *  @param FileName log file name.
	 */
	void                            SetLogFile( const char* FileName );
	/**
	 *  Is this a administraction connection?( Administraction connection will received all executions.)
	 *  @return TRUE for administracton connection.
	 */
	BOOL                            IsAdmin()                      { return FAdmin; }
	/**
	 *  The lagnuage of error message.(default english)
	 *  @param Lenguage Language.
	*/
	void                            SetLanguage( MessageLanguage Lenguage );
	/**
	 *  Set broker ID.
	 *  @param Market which market.
	 *  @param FCMID broker ID.
	 */
	void                            SetBrokerID( const char* FCMID);
	const UFC::AnsiString&          GetBrokerID( void ) 			{ return FTAIFEXBrokerID; }
	/**
	 *  Set clear member ID.
	 *  @param Market which market.
	 *  @param CMID clear member ID.
	 */
	void                            SetCMID( const char* CMID );
	const UFC::AnsiString&          GetCMID( void ) { return FCMID; }
public:
	/** Constractor for COM only.
	 */
	TTaifexTMPConnection(  HINSTANCE AppInstance,
						   HMODULE ResourceInstance,
						   const char* ClientAppName,
						   IOrderConnectionEventListener* pOrderConnectionEventListener,
						   BOOL  IsWin32GUIApp = FALSE );
	/**
	* Constractor for using C++ library.
	*  @Param ClientAppName given this client application a name.
	*  @Param pOrderConnectionEventListener Listener to handle execution and connection events.
	*  @Param  IsWin32GUIApp TRUE for Win32 GUI Application, FALSE for Linux, Unix, Win32 console application.
	*/
	TTaifexTMPConnection(  const char* ClientAppName,
						   IOrderConnectionEventListener* pOrderConnectionEventListener,
						   BOOL  IsWin32GUIApp = FALSE );
	/**
	* Destructor.
	*/
	virtual ~TTaifexTMPConnection( void );
	/**
	 * Set OnBeforeSend event listener.
	 * @Param Listener Object pointor which implement ISendMessageListener interface.
	 */
	void    SetBeforeSendListener( ISendMessageListener* Listener ) { FSendEventListener = Listener; }
};
//------------------------------------------------------------------------------
#endif

