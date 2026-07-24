#ifndef _TTaifexConnection_H_
#define _TTaifexConnection_H_
//------------------------------------------------------------------------------
#include "TFixedFormat.h"
#include "TParseData.h"
#include "TRenderData.h"
#include "iniFile.h"
//------------------------------------------------------------------------------
#include "../MessageDeliver/TMdMessage.h"
#include "../MessageDeliver/TMdTransport.h"
#include "../MessageDeliver/TMdListener.h"
//------------------------------------------------------------------------------
#include "../OrderMessage/TNetworkID.h"
#include "../OrderMessage/TNewOrderMessage.h"
#include "../OrderMessage/TReplaceOrderMessage.h"
#include "../OrderMessage/TCancelOrderMessage.h"
#include "../OrderMessage/TQuoteMessage.h"
#include "../OrderMessage/TQuoteCancelMessage.h"
#include "../OrderMessage/TQuoteRequestMessage.h"
#include "../OrderMessage/TExecutionReportMessage.h"
#include "../OrderMessage/TOrderStatusRequest.h"
#include "../MarketDataMessage/TNewsMessage.h"
#include "../OrderMessage/TNetworkID.h"
#include "../OrderMessage/TTouchOrderCommand.h"  // add by Kenny to support TouchOrder management. 2026/03/11
//------------------------------------------------------------------------------
#include "../EmbadedResource.h"
#include "../../SpeedyAPIXE7/MessageDataFormat.h"
#include "NameValueMessage.h"
//------------------------------------------------------------------------------
//#ifdef __UNICA_WIN
#ifdef WIN32
	#include "../CA/ApiCADllObject.h"
	#include "../CA/CABasicObjects.h"
/// Allen Modify at 20190615
///#ifndef _MSC_VER
///	#include "../CA/CACGCObject.h"
///#endif
#else
    #include "../CA/ApiCADllObject.h"
    #include "../CA/CABasicObjects.h"
#endif
//------------------------------------------------------------------------------
const int SPEEDY_API_PROXY_VERSION   = 3050801;
const int SPEEDY_API_GATEWAY_VERSION = 30508;
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
    lrDecimalError = 2, //< OK, but TAIFEX Decimal serring error.
    lrOk           = 1, //< OK 
    lrFailed       = 0,
    lrAlreadyLogon = -1

} LogonResult;
//------------------------------------------------------------------------------
typedef enum
{
    crModifyOk       = 0,
    crFailed         = 1,
    crNotLogin       = 2, ///< API side
    crSameAsOld      = 3, ///< API side
    crLengthNeeds8   = 4, ///< API side
    crNeedsNumbers   = 5, ///< API side
    crNeedsSymbols   = 6, ///< API side
    crUpperLowerCase = 7, ///< API side
    crRequestTimeOut = 8,
    crWrongPassword  = 9,
    crSameAsPrevious = 10,
    crLockedTryAgain = 11

} ChangePwdResult;
//------------------------------------------------------------------------------
typedef enum
{
    ssTAIFEX          = 0,
    ssTSEOTC          = 1,
    ssForeignExchange = 2

} SellSideType;
//------------------------------------------------------------------------------
typedef enum
{
    mlEnglish = 0, ///< Error message in English
    mlChinese = 1  ///< Error message in Chinese

} MessageLanguage;
//------------------------------------------------------------------------------
typedef enum
{
    ctSendOrder       = 0, ///< Can place order only.(New/Cancel/Replace/Quote)
    ctRecvExecution   = 1, ///< Only receive execution reports.
    ctBoth            = 2, ///< Can place order and receive executions.
    ctRecvFillOnly    = 3, ///< Only receive filled executions.
    ctRecvComfirmOnly = 4  ///< Only receive confirm executions.

} ConnectionType;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    rmFutures        =  0,
    rmOption         =  1,
    rmTSE            =  2,
    rmOTC            =  3,
    rmAll            =  4,
    rmForeignFutures =  5,
    rmForeignOptions =  6,
    rmForeignStock   =  7

} RecoverMarket;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    rsNormal      =  0,
    rsOffHour     =  1,
    rsAll         =  2,
    rsPrevOffHour =  3 

} RecoverSession;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    cmTWFutures      =  0,
    cmTWOptions      =  1,
    cmTWSE           =  2,
    cmOTC            =  3,	
    cmForeignFutures =  4,
    cmForeignOptions =  5,
    cmCNFutures      =  6,	
    cmCNOptions      =  7,
    cmAll            =  8

} CancelMarket;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
  cwrOK               = 0,
  cwrProcessNotExists = 1,
  cwrNoWorking        = 2,
  cwrNoRights         = 3

} CancelWorkingResult;
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
    smtNew           = 0,
    smtCancel        = 1,
    smtReplace       = 2,
    smtQuoteNew      = 3,
    smtQuoteCancel   = 4,
    smtQuoteRequest  = 5

} SendMessageType;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    atSpeedy        = 0, ///< Taiwan MDBS Speedy API
    atCTP           = 1, ///< China CTP API
    atFemas         = 2, ///< China Femas API
    atKSft          = 3, ///< China SunGard API. (Futures/Options)
    atKStrd         = 4, ///< China SunGard API. (Futures/Options/Stock-Options Integrated version)
    atHSctp         = 5, ///< China HundSun API. (Futures/Options) (CTP Like version)
    atHSufxFutOpt   = 6, ///< China HundSun API. (Futures/Options) (Native UFX API)
    atHSufxStockOpt = 7, ///< China HundSun API. (Stock-Option)    (Native UFX API)
    atKNctp         = 8, ///< China KingNew API. (CTP Like version)
    atCliOpt        = 9, ///< China maCliOpt API (CTP Like version)
    atJasStk        = 10 ///< Taiwan Jasper API. (Stock)

} APIType;
//------------------------------------------------------------------------------
const long CMD_LOGON_REQUEST              = 10001;
const long CMD_LOGON_REPLY                = 10002;
const long CMD_RECOVER_REQUEST            = 10003;
const long CMD_RECOVER_FINISHED           = 10004;
const long CMD_LOCK_AE                    = 10005;
const long CMD_ASK_LOCK_AE                = 10006;
const long CMD_CXL_WORKING                = 10007;
const long CMD_CXL_WORKING_REPLY          = 10008;
const long CMD_CHANGE_PASSWORD            = 10009;
const long CMD_CHANGE_PASSWORD_REPLY      = 10010;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_RECOVER;
extern const UFC::AnsiString SUBJECT_ADMIN;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_ORDER_FUT;
extern const UFC::AnsiString SUBJECT_ORDER_OPT;
extern const UFC::AnsiString SUBJECT_ORDER_TSE;
extern const UFC::AnsiString SUBJECT_ORDER_OTC;
extern const UFC::AnsiString SUBJECT_ORDER_ES;
extern const UFC::AnsiString SUBJECT_ORDER_FOREIGN_FUT;
extern const UFC::AnsiString SUBJECT_ORDER_FOREIGN_OPT;
extern const UFC::AnsiString SUBJECT_ORDER_FOREIGN_STK;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_FT_FUT;
extern const UFC::AnsiString SUBJECT_FT_OPT;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_CONFIRM_FUT;
extern const UFC::AnsiString SUBJECT_CONFIRM_OPT;
extern const UFC::AnsiString SUBJECT_CONFIRM_TSE;
extern const UFC::AnsiString SUBJECT_CONFIRM_OTC;
extern const UFC::AnsiString SUBJECT_CONFIRM_ES;
extern const UFC::AnsiString SUBJECT_CONFIRM_FOREIGN;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_FILL_FUT;
extern const UFC::AnsiString SUBJECT_FILL_OPT;
extern const UFC::AnsiString SUBJECT_FILL_TSE;
extern const UFC::AnsiString SUBJECT_FILL_OTC;
extern const UFC::AnsiString SUBJECT_FILL_ES;
extern const UFC::AnsiString SUBJECT_FILL_FOREIGN;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_FILE_DOWNLOAD;
extern const UFC::AnsiString SUBJECT_NEWS_REQUEST;
extern const UFC::AnsiString SUBJECT_NEWS_RESPONSE;
//------------------------------------------------------------------------------
extern const UFC::AnsiString SUBJECT_TSE_STRATEGY; // added by Kenny. 2026/01/15
extern const UFC::AnsiString SUBJECT_OTC_STRATEGY; // added by Kenny. 2026/01/15
//------------------------------------------------------------------------------
extern UFC::BufferedLog* Glog;
//------------------------------------------------------------------------------
class FuturesSymbolUtility
{
private:
	int FBaseYear;
	int FYearDigi;
	UFC::PHashMap<UFC::AnsiString,UFC::AnsiString*> FNewToOldFutSymbols;
	UFC::PHashMap<UFC::AnsiString,UFC::AnsiString*> FOldToNewFutSymbols;
public:
	FuturesSymbolUtility();
	void ConvertToNewSymbol( UFC::AnsiString& Symbol, const UFC::AnsiString& OldSymbol, const UFC::AnsiString& Date );
	void ConvertToOldSymbol( const UFC::AnsiString& New, UFC::AnsiString& Old,UFC::AnsiString& Date );
};
//------------------------------------------------------------------------------
class IOrderConnectionEventListener
{
public:
	virtual void OnConnected( void ) = 0;
	virtual void OnDisconnected( void ) = 0;
	virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID ) = 0;
	virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup ) = 0;
	virtual void OnRecoverFinished( int Count ) = 0;
	virtual void OnCancelWorking( CancelWorkingResult Result, int Count ) {}
	virtual void OnChangePassword( ChangePwdResult Result ){}
	virtual void OnNews( TNewsMessage* Msg ) {}
#ifndef _WIN32
	virtual ~IOrderConnectionEventListener( void ) {}
#endif
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
    TTransport*     FTransport;
    UFC::PEvent*    FReplyEvent;
    UFC::AnsiString FSubject;
    UFC::AnsiString FKey;
    UFC::AnsiString FID;
    UFC::AnsiString FPasswd;
    UFC::AnsiString FPasswd2;
    UFC::AnsiString FAccount;
    int             FVersion;
    BOOL            FEncode;
    CApiCADllObject* FApiCAObjPtr;
    UFC::AnsiString FCAPlainText;
    UFC::AnsiString FCASignature;
    UFC::AnsiString FCASubject;
    UFC::AnsiString FCANotBefore;
    UFC::AnsiString FCANotAfter;
    UFC::AnsiString FCASerialNumber;
public:
    LogonThread( TTransport* Transport,
							 UFC::PEvent*  LogonEvent,
                             int   Version,
                             const UFC::AnsiString& Subject,
                             const UFC::AnsiString& Key ,
                             const UFC::AnsiString& ID ,
                             const UFC::AnsiString& Passwd,
                             const UFC::AnsiString& Account,
                             const UFC::AnsiString& Passwd2,
                             CAResultData& CAResult,
                             BOOL Encode );
protected:
    virtual void Execute( void );
};
//------------------------------------------------------------------------------
class TTaifexConnection;
//------------------------------------------------------------------------------
class LogoffThread : public UFC::PThread
{
private:
	TTaifexConnection*   FConnection;
public:
	LogoffThread( TTaifexConnection* Connection );
	void Execute( void );
};
//------------------------------------------------------------------------------
class TTaifexConnection : public MAppListener, public MessageListener
{
public:
typedef void (TTaifexConnection::*EventFunc)( MTree* Msg );
typedef bool (TTaifexConnection::*TWSERenderFunc)( int Func, TBaseMessage* Msg, char* T010, char Ex, char Ot );
private:
	int FOutCount;
	int FOrderPerSec;
	UFCType::UInt32 FFirstOrderTick;
	MessageLanguage FLanguage;
private:
	HINSTANCE                       FInstance;
	HMODULE                         FResourceInstance;
	UFC::PSet<SellSideType>         FSupportSellSide;
public:
	static UFC::PSet<int>           FAPISet;
	static void                     InitGlobal( void );
	static void                     DetectAPI( bool Enable );
	static bool                     SupportAPI( APIType APITp );
private: ///< For CTP API
	static bool                     FDetectAPI;
	APIType                         FUseAPI;
	TNetworkID                      FNetworkID;
private:
	static UFC::AnsiString          FTSEOTCBrokerID;
	static UFC::AnsiString          FTAIFEXBrokerID;
public:
	UFC::AnsiString                 FUserProductInfo;
	UFC::AnsiString                 FID;
	UFC::AnsiString                 FPassword;
	UFC::AnsiString                 FToken;
	UFC::AnsiString                 FLocalIP;
	int                             FAdmin;
	int                             FIsDMA;
	int                             FCurrentConnectionID;
	bool                            FIsConnected;
	bool                            FIsLogon;
	bool                            FUseNID64;
	UFC::AnsiString                 FTradeingDate;
	IOrderConnectionEventListener*  FListener;
	ISendMessageListener*           FSendEventListener;
private:
	UFC::AnsiString                 FAppName;
	UFC::AnsiString                 FUniquekey;
	UFC::AnsiString                 FCMID;
	UFC::AnsiString                 FUserName;
	bool                            FIsTWSE8DigiVersion;
	bool                            FNewFutSymbol;
	bool                            FEnablePendingNewAck;
	bool                            FTriggerExec;
	bool                            FCanSendOrder;
	bool                            FFillZeroToCancel;
	bool                            FEnableFilledState;
	bool                            FX25StyleQtyPx;
	bool                            FAlwaysGenNID;
	RecoverDataType                 FReportType;
	UFC::PEvent*                    FLogonEvent;
	UFC::PCriticalSection           FExecCS;
	FuturesSymbolUtility            FFutSymbol;
	UFC::Int32                      FLastReqUID;
	UFC::Int32                      FResponseUID;
	UFC::PEvent*                    FRequestEvent;
	UFC::PCriticalSection           FNewsRequestCS;
	UFC::AnsiString                 FResponseData;
private: ///< Callback functions and container
	EventFunc                       FOnAdminMsg;
	EventFunc                       FOnNewsMsg;
	EventFunc                       FOnRecoverMsg;
	EventFunc                       FOnOptConfirm;
	EventFunc                       FOnFutConfirm;
	EventFunc                       FOnTSEConfirm;
	EventFunc                       FOnOTCConfirm;
	EventFunc                       FOnESConfirm;
	EventFunc                       FOnForeignConfirm;
	EventFunc                       FOnOptFill;
	EventFunc                       FOnFutFill;
	EventFunc                       FOnTSEFill;
	EventFunc                       FOnOTCFill;
	EventFunc                       FOnESFill;
	EventFunc                       FOnForeignFill;
	EventFunc                       FOnTouchOrderResponse; // added by Kenny to support Touch Order. 2026/03/16
	UFC::PHashMap<UFC::AnsiString, EventFunc*> FCallbackFuncs;
private: ///< ExecID hash set.
	UFC::PStringHashedSet           FFUTExecIDSet; ///< TAIFEX Futures
	UFC::PStringHashedSet           FOPTExecIDSet; ///< TAIFEX Options
	UFC::PStringHashedSet           FTSEExecIDSet; ///< TWSE
	UFC::PStringHashedSet           FOTCExecIDSet; ///< OTC
	UFC::PStringHashedSet           FESExecIDSet;  ///< ES
	UFC::PStringHashedSet           FPATSExecIDSet;///< PATS
	UFC::PStringHashedSet           FCNExecIDSet;  ///< China Futures
private: ///< Critical Section.
	UFC::PCriticalSection           FExecIDCS; ///< Lock for Execution ID
	UFC::PCriticalSection           FRenderCS; ///< Lock for order string rander.
private:
	TTransport*                     FTransport;      ///< MBus TCP Transport.
	TMdListener*                    FAdminListener;  ///< Listener for admin message.(logon/off, recover execution.)
	TMdListener*                    FNewsListener;
	TMdListener*                    FProxyNewsListener;
	UFC::List<TMdListener*>         FExecListeners;  ///< Listener list for executions.
	UFC::List<TMdListener*>         FRecoverListeners;///< Listener for Recover executions.
	UFC::PStringList                FAccounts;       ///< Other Accounts need to recv executions.
	UFC::PStringList                FIDs;
private:
	///<
	///< TAIFEX Render/Parser/Format
	///<
	///< R010 Render
	UFC::TFixedFormat               FR010Format;
	UFC::TRenderData                FR010Render;
	///< R020 Parser
	UFC::TFixedFormat               FR020Format;
	UFC::TParseData                 FR020Parser;
	///< Fill execution Parser and DataFormat object.
	UFC::TFixedFormat               FC030Format;
	UFC::TParseData                 FC030Parser;
	///< Multi-leg execution Parser and DataFormat object.
	UFC::TFixedFormat               FC030MFormat;
	UFC::TParseData                 FC030MParser;
	///< Canceled/Replaced execution Parser and DataFormat object.
	UFC::TFixedFormat               FC030CRFormat;
	UFC::TParseData                 FC030CRParser;
	///< Quote Canceled execution Parser and DataFormat object.
	UFC::TFixedFormat               FC030QCRFormat;
	UFC::TParseData                 FC030QCRParser;
	///< Parser and DataFormat object for Quote request .
	UFC::TFixedFormat               FR070Format;
	UFC::TRenderData                FR070Render;
	UFC::TFixedFormat               FR080Format;
	UFC::TParseData                 FR080Parser;
	///< Parser and DataFormat object for Quote.
	UFC::TFixedFormat               FR090Format;
	UFC::TRenderData                FR090Render;
	UFC::TFixedFormat               FR100Format;
	UFC::TParseData                 FR100Parser;
	///<
	///< TSE/OTC Render/Parser/Format
	///<
	///< TSE T010,O010,P010 Rendera for NewOrder( Normal,Odd,Post session)
	UFC::TFixedFormat               FTSET010Format;
	UFC::TRenderData                FTSET010Render;	
	///< OTC T010,O010,P010 Render for NewOrder( Normal,Odd,Post session)
	UFC::TFixedFormat               FOTCT010Format;
	UFC::TRenderData                FOTCT010Render;
	///< TSE A010, V010, E010, Ex010 Render        
	UFC::TFixedFormat               FTSEA010ExFormat; ///< New for 2020/03/02
	UFC::TRenderData                FTSEA010ExRender; ///< New for 2020/03/02
	UFC::TFixedFormat               FTSEV010ExFormat; ///< New for 2020/03/02
	UFC::TRenderData                FTSEV010ExRender; ///< New for 2020/03/02
	UFC::TFixedFormat               FTSEE010ExFormat; ///< New for 2020/03/02
	UFC::TRenderData                FTSEE010ExRender; ///< New for 2020/03/02
	UFC::TFixedFormat               FTSEEx010ExFormat;///< New for 2020/03/02
	UFC::TRenderData                FTSEEx010ExRender;///< New for 2020/03/02
	///< TSE A020, V020, E020, Ex020 Parser	
	UFC::TFixedFormat               FTSEA020ExFormat; ///< New for 2020/03/02
	UFC::TParseData                 FTSEA020ExParser; ///< New for 2020/03/02
	UFC::TFixedFormat               FTSEV020ExFormat; ///< New for 2020/03/02
	UFC::TParseData                 FTSEV020ExParser; ///< New for 2020/03/02
	UFC::TFixedFormat               FTSEE020ExFormat; ///< New for 2020/03/02
	UFC::TParseData                 FTSEE020ExParser; ///< New for 2020/03/02
	UFC::TFixedFormat               FTSEEx020ExFormat;///< New for 2020/03/02
	UFC::TParseData                 FTSEEx020ExParser;///< New for 2020/03/02
	///< OTC V010, E010, Ex010 Render	
	UFC::TFixedFormat               FOTCV010ExFormat; ///< New for 2020/03/02
	UFC::TRenderData                FOTCV010ExRender; ///< New for 2020/03/02
	UFC::TFixedFormat               FOTCE010ExFormat; ///< New for 2020/03/02
	UFC::TRenderData                FOTCE010ExRender; ///< New for 2020/03/02
	UFC::TFixedFormat               FOTCEx010ExFormat;///< New for 2020/03/02
	UFC::TRenderData                FOTCEx010ExRender;///< New for 2020/03/02
	///< OTC V020, E020, Ex020 Parser	
	UFC::TFixedFormat               FOTCV020ExFormat; ///< New for 2020/03/02
	UFC::TParseData                 FOTCV020ExParser; ///< New for 2020/03/02
	UFC::TFixedFormat               FOTCE020ExFormat; ///< New for 2020/03/02
	UFC::TParseData                 FOTCE020ExParser; ///< New for 2020/03/02
	UFC::TFixedFormat               FOTCEx020ExFormat;///< New for 2020/03/02
	UFC::TParseData                 FOTCEx020ExParser;///< New for 2020/03/02	        
	
        ///< New version for 2020/03 continuely match.(Normal/Odd/Post)
	UFC::TFixedFormat               FTSET020ExFormat; ///< New for 2020/03/23
	UFC::TParseData                 FTSET020ExParser; ///< New for 2020/03/23
	UFC::TFixedFormat               FOTCT020ExFormat; ///< New for 2020/03/23
	UFC::TParseData                 FOTCT020ExParser; ///< New for 2020/03/23
        
	UFC::TFixedFormat               FTSEO010ExFormat; ///< New for 2020/03/23
	UFC::TRenderData                FTSEO010ExRender; ///< New for 2020/03/23
	UFC::TFixedFormat               FOTCO010ExFormat; ///< New for 2020/03/23
	UFC::TRenderData                FOTCO010ExRender; ///< New for 2020/03/23

	UFC::TFixedFormat               FTSEO020ExFormat; ///< New for 2020/03/23
	UFC::TParseData                 FTSEO020ExParser; ///< New for 2020/03/23
	UFC::TFixedFormat               FOTCO020ExFormat; ///< New for 2020/03/23
	UFC::TParseData                 FOTCO020ExParser; ///< New for 2020/03/23

	UFC::TFixedFormat               FTSEP010ExFormat; ///< New for 2020/03/23
	UFC::TRenderData                FTSEP010ExRender; ///< New for 2020/03/23
	UFC::TFixedFormat               FOTCP010ExFormat; ///< New for 2020/03/23
	UFC::TRenderData                FOTCP010ExRender; ///< New for 2020/03/23

	UFC::TFixedFormat               FTSEP020ExFormat; ///< New for 2020/03/23
	UFC::TParseData                 FTSEP020ExParser; ///< New for 2020/03/23
	UFC::TFixedFormat               FOTCP020ExFormat; ///< New for 2020/03/23
	UFC::TParseData                 FOTCP020ExParser; ///< New for 2020/03/23
        
        ///< Filled Execution Parser and DataFormat object.
        UFC::TFixedFormat               FTSER03Format;  ///< New for 2020/06/15 8 digi fill sequence
	UFC::TParseData                 FTSER03Parser;  ///< New for 2020/06/15 8 digi fill sequence
	UFC::TFixedFormat               FOTCR03Format;  ///< New for 2020/06/15 8 digi fill sequence
	UFC::TParseData                 FOTCR03Parser;  ///< New for 2020/06/15 8 digi fill sequence
	UFC::TFixedFormat               FTSER03ExFormat;  ///< New for 2026/03/23
	UFC::TParseData                 FTSER03ExParser;  ///< New for 2026/03/23
	UFC::TFixedFormat               FOTCR03ExFormat;  ///< New for 2026/03/23
	UFC::TParseData                 FOTCR03ExParser;  ///< New for 2026/03/23        
        ///< Render function pointor
	TWSERenderFunc                  FTWSERender;
	///< Error code ini file.
	UFC::UiniFile*                  FExchangeError;
	UFC::UiniFile*                  FFUTSymbol;
	UFC::UiniFile*                  FOPTSymbol;
	bool                            FIsProxy;
	int                             FEncode;
	bool                            FSupportFLEX;
    UFC::PStringHashedSet           FAccountSet;
private: ///< for CA
	UFC::AnsiString FCAPFXFilePathName;
	UFC::AnsiString FCAPassword;
	UFC::AnsiString FCADLLFileName;
	UFC::AnsiString FCAOrganizationalUnit;
	UFC::AnsiString FCACommonName;
	CApiCADllObject* FApiCAObjPtr;
        
        bool FNeedCheckOrdQty0;  //for KGI RiskManager
        
private:
	bool ControlTroughput( void );
	char OrderSourceCode( nsOrderMessageDefine::OrderSourceEnum OrdSrc );
	bool SupportSellSide( TBaseMessage* Msg  );
private:
	bool RenderTAIFEXR010( int Func, TBaseMessage* Msg, char* R010, nsOrderMessageDefine::TradingSessionIDEnum  TradingSession );
	bool RenderTAIFEXNewOrder( TNewOrderMessage* Msg, char* R010, UFC::AnsiString& Source );
	bool RenderTAIFEXCancelOrder( TCancelOrderMessage* Msg, char* R010, UFC::AnsiString& Source );
	bool RenderTAIFEXReplaceOrder( TReplaceOrderMessage* Msg, char* R010, UFC::AnsiString& Source );
	bool RenderTAIFEXOrderStatus( TOrderStatusRequest* Msg, char* R010, UFC::AnsiString& Source );

	bool RenderTWSET010Ex( int Func, TBaseMessage* Msg, char* T010, char Ex, char Ot );
	bool RenderTWSEO110Ex( int Func, TBaseMessage* Msg, char* R010 );

	bool RenderTSEOTCNewOrder( TNewOrderMessage* Msg, char* R010 );
	bool RenderTSEOTCCancelOrder( TCancelOrderMessage* Msg, char* R010 );
	bool RenderTSEOTCReplaceOrder( TReplaceOrderMessage* Msg, char* R010 );
        bool RenderTSEOTCOrderStatus( TOrderStatusRequest* Msg, char* R010 );

	bool RenderChinaNewOrder(  TNewOrderMessage* Msg, char* R010 );
	bool RenderChinaCancelOrder( TCancelOrderMessage* Msg, char* R010 );
	bool RenderChinaReplaceOrder( TReplaceOrderMessage* Msg, char* R010 );

	bool RenderForeignNewOrder(   TNewOrderMessage* Msg, char* R010 );
	bool RenderForeignCancelOrder( TCancelOrderMessage* Msg, char* R010 );
	bool RenderForeignReplaceOrder( TReplaceOrderMessage* Msg, char* R010 );

	void WriteStringToStream( const UFC::AnsiString& Name, const UFC::AnsiString& Value, UFC::MemoryStream& WriteStream );
	void WriteIntToStream( const UFC::AnsiString& Name, int Value, UFC::MemoryStream& WriteStream );
	void WriteInt64ToStream( const UFC::AnsiString& Name, UFCType::Int64 Value, UFC::MemoryStream& WriteStream );
private: ///< Impelment interface MAppListener
	virtual void                    OnMAppConnected( void );
	virtual void                    OnMAppDisconnected( void );
	virtual void                    OnMAppError( PMAppError Error );
public: ///< Impelment interface MessageListener
	virtual void                    OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* pTree );
private:
	BOOL                            CreateTempFileFromMNode( MTree* Data, const char* Name, UFC::AnsiString& FileName );
	UFC::AnsiString                 CreateTempFileFromResource( DWORD ResourceID, const char* ResourceType );
	bool                            LoadDataFormatFromResourceFile( DWORD ResourceID, const char* ResourceType, UFC::TFixedFormat& DataFormat );
	int                             GetCurrentMessageTime( void );
	int                             StringToInt( const UFC::AnsiString& StringValue, long DefaultValue );
	void                            PendingNew( TNewOrderMessage* Msg );
	BOOL                            SendToOrderServerEx( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& Data, const UFC::AnsiString& OrderString, nsOrderMessageDefine::TradingSessionIDEnum TradingSession,const UFC::AnsiString& Source, int Group, UFCType::Int64 NID,
															 UFCType::Int64 CNID = 0,
															 const UFC::AnsiString& PeerIP = "",
															 const UFC::AnsiString& CASignature = "",
															 const UFC::AnsiString& CAPlainText = "",
															 const UFC::AnsiString& CASessionID = "",
															 const UFC::AnsiString& CASerialNumber = "",
                                                                                                                         const UFC::AnsiString& StopPriceStrategy = "");
	UFC::TRecord*                   ParseConfirmReport( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::MessageTypeEnum MessageType, const UFC::AnsiString&  Msg );
	UFC::TRecord*                   ParseExecuteReport( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString&  Msg );
	void                            NoOrderIDReject( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo, TBaseMessage* Msg,const UFC::AnsiString& UserData, nsOrderMessageDefine::TradingSessionIDEnum TradingSession );
	bool                            TriggerEventAndSend( TBaseMessage* Msg, const UFC::AnsiString&, const UFC::AnsiString&, SendMessageType, nsOrderMessageDefine::CxlRejResponseToEnum, nsOrderMessageDefine::TradingSessionIDEnum TradingSession,const UFC::AnsiString& Source, UFCType::Int64 CNID = 0 );
	bool                            CheckOrderID( const char*OrderID );
	bool                            CheckOrderID( nsOrderMessageDefine::MarketEnum OrderMarket, const char*OrderID );
	void                            AddExecListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, EventFunc* CBFunc );
	void                            AddRecoverListener( void );
	void                            CreateReportListener( void );
	void                            AddTAIFEXReportListener( const UFC::AnsiString& ListenKey );
	void                            AddTWSEReportListener( const UFC::AnsiString& ListenKey );
	void                            AddTouchOrderResponseListener(const UFC::AnsiString& ListenKey); // addded by Kenny to support Touch Order. 2026/03/16
	void                            AddForeignExReportListener( const UFC::AnsiString& ListenKey );
	LogonResult                     CreateShareMemory( MTree* pTree, int CIDBits, UFC::AnsiString& ReplyString );
public:
	bool                            IsExecutionDup( nsOrderMessageDefine::MarketEnum  Market, const UFC::AnsiString& ExecID );
	void                            TrigerOnExecutionReport( TExecutionReportMessage* ExecutionReport, ExecDup PosDup );
	void                            FillRejectMsg( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo, const UFC::AnsiString& Text, const UFC::AnsiString& ErrMsg, TExecutionReportMessage* Msg );
	void                            GetRejectMsg(  const UFC::AnsiString& StatusCode, const UFC::AnsiString&  ErrMsg, TExecutionReportMessage* Msg, UFC::AnsiString& ErrorStr );
	void 	          			    Reject( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo, const UFC::AnsiString& ErrMsg,const UFC::AnsiString& UDD, TBaseMessage* Msg, nsOrderMessageDefine::TradingSessionIDEnum TradingSession );
private:
	void 							GetAPIError( UFC::AnsiString& ErrorStr, int ErrorCode, const UFC::AnsiString& DefStr );
	void                            SetExecSide( UFC::TRecord* pRecord,  TExecutionReportMessage* ExecutionReport );
	void                            SetExecOrderType( UFC::TRecord* pRecord,  TExecutionReportMessage* ExecutionReport );
	void                            SetExecTimeInForce( nsOrderMessageDefine::MarketEnum Market, UFC::TRecord* pRecord,  TExecutionReportMessage* ExecutionReport );
	const char*                     GetExchangePositionEffect( nsOrderMessageDefine::PositionEffectEnum pe );
	UFC::AnsiString                 GetExchangeOrderType( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::OrderTypeEnum Type );
	UFC::TRenderData*               GetR010Render( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::TradingSessionIDEnum  TradingSession );
	void                            FillNewOrderFunctionCode( nsOrderMessageDefine::TradingSessionIDEnum TradingSession, nsOrderMessageDefine::MarketEnum Market, TNewOrderMessage* Msg, UFC::TRenderData* R010Render );
	void                            FillCancelOrderFunctionCode( nsOrderMessageDefine::TradingSessionIDEnum TradingSession, nsOrderMessageDefine::MarketEnum Market, TCancelOrderMessage* Msg, UFC::TRenderData* R010Render );
	void                            FillReplaceOrderFunctionCode( nsOrderMessageDefine::TradingSessionIDEnum TradingSession, nsOrderMessageDefine::MarketEnum Market, TReplaceOrderMessage* Msg, UFC::TRenderData* R010Render );
	void                            FillOrderStatusRequestFunctionCode( nsOrderMessageDefine::TradingSessionIDEnum TradingSession, nsOrderMessageDefine::MarketEnum Market, TOrderStatusRequest* Msg, UFC::TRenderData* R010Render );
	void                            FillPATSOrderBase( TBaseMessage* Msg, UFC::NameValueMessage* PATSMessage );
	void                            FillChinaOrderBase( TBaseMessage* Msg, UFC::NameValueMessage* GLQHMessage );
	void                            FillOrderBase( nsOrderMessageDefine::TradingSessionIDEnum TradingSession, nsOrderMessageDefine::MarketEnum Market, TBaseMessage* Msg, UFC::TRenderData* R010Render );
	void                            FillRejectExecution( nsOrderMessageDefine::MessageTypeEnum MessageType,nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::TradingSessionIDEnum TradingSession,UFC::TRecord* pRecord,UFC::AnsiString StatusCode,UFC::AnsiString ErrMsg,TExecutionReportMessage* ExecutionReport, int& Precision  );
	void                            FillExecution( nsOrderMessageDefine::MessageTypeEnum MessageType,nsOrderMessageDefine::MarketEnum Market,nsOrderMessageDefine::TradingSessionIDEnum TradingSession,UFC::TRecord* pRecord,TExecutionReportMessage* ExecutionReport, int& Precision  );
	void                            FillStockExecution( nsOrderMessageDefine::MessageTypeEnum MessageType,nsOrderMessageDefine::MarketEnum Market,nsOrderMessageDefine::TradingSessionIDEnum TradingSession,UFC::TRecord* pRecord,TExecutionReportMessage* ExecutionReport, int& Precision  );
	void                            FillFilledExecution( UFC::TRecord* pRecord,TExecutionReportMessage* ExecutionReport, int Precision);
	void                            FillMultilegExecution( UFC::TRecord* pRecord,TExecutionReportMessage* ExecutionReport, int Precision);
	void                            FillCancelReplaceExecution( UFC::TRecord* pRecord,TExecutionReportMessage* ExecutionReport );
	void                            FillQuoteCancelReplaceExecution( UFC::TRecord* pRecord,TExecutionReportMessage* ExecutionReport );
	void                            FillExecutionBase( nsOrderMessageDefine::MarketEnum Market,nsOrderMessageDefine::TradingSessionIDEnum TradingSession, UFC::TRecord* pRecord,TExecutionReportMessage* ExecutionReport,int& );
	nsOrderMessageDefine::TradingSessionIDEnum GetMessageTradeingSession( const UFC::AnsiString& Msg );
	void 				TAIFEXFilledExecID( bool IsFut, UFC::AnsiString& Seq, const UFC::AnsiString& PBNO, int NID, int PartID, UFC::AnsiString& ExecID );
	void 				TAIFEXConfirmExecID( bool IsFut, UFC::AnsiString& Seq, const UFC::AnsiString& PBNO, int NID, int PartID, UFC::AnsiString& ExecID );
	void 				UpdateTAIFEXAEUDD( MTree* pTree, UFC::AnsiString& Key, UFC::AnsiString& AE, TExecutionReportMessage& ExecutionReport );
	void 				UpdateTAIFEXTradingSession( MTree* pTree, TExecutionReportMessage& ExecutionReport );
	void 				UpdateTAIFEXTMPExt( MTree* pTree, TExecutionReportMessage& ExecutionReport, int Precision );
public:
	void                            ReceiveRecoverExecution( MTree* pTree );
	void                            ReceiveAdminMessage( MTree* pTree );
	bool    	            	IsTAIFEXSucceed( const UFC::AnsiString& StatusCode );
	bool 	            		IsTWSESucceed( const UFC::AnsiString& StatusCode );
private: ///< Handle Executions
	///< Confirm messages
	void                            ReceiveFutConfirmMessage( MTree* pTree );
	void                            ReceiveOptConfirmMessage( MTree* pTree );
	void                            ReceiveTSEConfirmMessage( MTree* pTree );
	void                            ReceiveOTCConfirmMessage( MTree* pTree );
	void                            ReceiveESConfirmMessage( MTree* pTree );
	void                            ReceiveForeignConfirmMessage( MTree* pTree );
	void                            ReceiveCNConfirmMessage( MTree* pTree );
	///< Filled messages
	void                            ReceiveFutExecuteMessage( MTree* pTree );
	void                            ReceiveOptExecuteMessage( MTree* pTree );
	void                            ReceiveTSEExecuteMessage( MTree* pTree );
	void                            ReceiveOTCExecuteMessage( MTree* pTree );
	void                            ReceiveESExecuteMessage( MTree* pTree );
	void                            ReceivePATSExecuteMessage( MTree* pTree );
	void                            ReceiveCNExecuteMessage( MTree* pTree );
	void                        	ReceiveForeignExecuteMessage( MTree* pTree );
	///<
	void                            ReceiveTouchOrderResponse(MTree* pTree); // added by Kenny to support Touch Order. 2026/03/16
	void                            TouchOrderUserLogon(); // added by Kenny to support user online notification. 2026/07/24
	void                            UpdateTMPFields( UFC::AnsiString& TMPExtStr, TExecutionReportMessage& ExecutionReport, int Precision );
	void                            ReceiveNews( MTree* pTree );
	void                            RemoveListener( TMdListener*& Listener );
	BOOL                            LoadSymbol( MTree* Data );
        BOOL                            CheckDecimalLocatorSetting( UFC::UiniFile* ini, const UFC::AnsiString& Sect );
	BOOL                            ParseUserID( const UFC::AnsiString& String, UFC::AnsiString& AE, UFC::AnsiString& Data );
	BOOL                            ParseCNExecution( TExecutionReportMessage& ExecutionReport, UFC::NameValueMessage& GLQHConfirm );
	bool                            IsTAIWAN( nsOrderMessageDefine::MarketEnum Market );
	bool                            IsPATS( nsOrderMessageDefine::MarketEnum Market );
	bool                            IsChina( nsOrderMessageDefine::MarketEnum Market );
	void                            DoRecover( const char* ID, const char* BeginTime, const char* EndTime, RecoverDataType Type, RecoverMarket Market, RecoverSession Session, UFC::AnsiString RecoverYYYYMMDD = "");
	bool                            CheckFLEXSymbol( const UFC::AnsiString& Symbol );
private: ///< Handle change password
	bool                            CheckPassword( const UFC::AnsiString& Password, const UFC::AnsiString& NewPassword, ChangePwdResult& Result );
public:
	void                            NegotiatePriceMessage( const UFC::AnsiString& Msg, const UFC::AnsiString& Data );
	void                            GenerateNID( TBaseMessage* Msg );
	UFCType::Int64                  GenerateNID( nsOrderMessageDefine::MessageTypeEnum Type );
	int                             GetPricePrecision( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::TradingSessionIDEnum TradeSession,const UFC::AnsiString& Symbol );
	int                             GetTAIFEXPricePrecision( nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& Symbol );
	void                            SetTAIFEXDataFormatResourceID( DWORD R010, DWORD R020, DWORD C030, DWORD C030_MultiLeg, DWORD C030_CancelReplace,DWORD C030_QuoteCancelReplace, const char* ResourceType );
	void                            SetTAIFEXQuoteDataFormatResourceID( DWORD R070, DWORD R080, DWORD R090, DWORD R100, const char* ResourceType );
	void                            SetTSEDataFormatResourceID( DWORD T010, DWORD O010, DWORD P010, DWORD T020, DWORD O020, DWORD P020, DWORD R030, const char* ResourceType );
	void                            SetTSEDataFormatResourceID2( DWORD A010, DWORD V010, DWORD E010, DWORD Ex010, DWORD A020, DWORD V020, DWORD E020, DWORD Ex020, const char* ResourceType );
	void                            SetOTCDataFormatResourceID( DWORD T010, DWORD O010, DWORD P010, DWORD T020, DWORD O020, DWORD P020, DWORD R030, const char* ResourceType );
	void                            SetOTCDataFormatResourceID2( DWORD V010, DWORD E010, DWORD Ex010, DWORD V020, DWORD E020, DWORD Ex020, const char* ResourceType );
	BOOL                            SendDirect( UFCType::Int64 NID, nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& OrderString,const UFC::AnsiString& Data, int Group );
public:
	/**
	 *  TAIFEX X.25 TMP protocol will send fill message with qty=0.(order was canceled)
	 *
	 *  @param YOrN true or false.
	 *  Default: true.
	 */
	void                            SetFillZeroQtyToCancel( bool YOrN ) { FFillZeroToCancel  = YOrN; }
	/**
	 *  TAIFEX X.25 TMP protocol can't tell PartiallyFilled and Filled status.
	 *  always send the PartiallyFilled order status.
	 *
	 *  @param YOrN true or false.
	 *  Default: false.
	 */
	void                            SetEnableFilledState( bool YOrN )   { FEnableFilledState = YOrN; }
	/**
	 *  Use X.25 or TMP/FIX style price/Qty
	 *
	 *  @param YOrN Yes or No.
	 *  Default: true.
	 *
	 *  For a filled execution, the difference between these two styles are:
	 *  1. OrderQty, for X.25 style it means "Last filled qty".
	 *  2. Price, for X.25 style it means "Last filled price".
	 *  3. LastPx, for X.25 style it means "The Original order price".
	 *
	 *  For a canceled execution, the difference between these two styles is:
	 *  1. OrderQty, for X.25 style it equals 0.
	 *               for TMP/FIX style it means "The Original order Qty".
	 */
	void                            SetX25StyleQtyPx( bool YOrN )       { FX25StyleQtyPx = YOrN; }
	bool                            GetX25StyleQtyPx( void )            { return FX25StyleQtyPx; }
	void                            SetAlwaysGenerateNID( bool YOrN )   { FAlwaysGenNID = YOrN; }
	bool                            GetAlwaysGenerateNID( void )        { return FAlwaysGenNID; }
public: ///< Public functions for order connection.
	/**
	 *  Add the given sell side support.
	 */
	void                            AddSellSide( SellSideType SellSide );
	/**
	 *  Remove the given sell side support.
	 */
	void                            RemoveSellSide( SellSideType SellSide );
	/**
	 * Try to connect a Speedy Order Gateway.
	 *  @param RemoteIP IP address.
	 *  @Param RemotePort Port.(Default 23456)
	 */
	void                            Connect( const char* RemoteIP, long RemotePort, int TimeoutSec );
	/**
	 * Try to close a Speedy Order Gateway connection.
	 *
	 */
	void                            Disconnect( void );
	/**
	 *  Login to a Speedy Order Gateway.
	 *  @Param ID ID of the connection.
	 *  @Param PASSWD Password of this ID.
	 *  @Param ACCOUNT Trading account.
	 *  @Param RecvExec TRUE for receiving executions, FALSE for send order only connection.
	 *
	 */
	void                            SpeedyProxyLogon( const char* ID, const char* PASSWD, const char* ACCOUNT, const char* Token1 = "SpeedyProxy", const char* Token2 = "601008", ConnectionType Type = ctBoth, int Version = SPEEDY_API_PROXY_VERSION, BOOL Encode = FALSE );
	void                            Logon( const char* ID, const char* PASSWD, const char* ACCOUNT, ConnectionType Type = ctBoth, int Version = SPEEDY_API_GATEWAY_VERSION, BOOL Encode = FALSE );
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
	 *  @Param Session Normal,Off-Hour session or all session.
	 *  @Param RecoverYYYYMMDD  Recover date.(Default this trading date)
	 */
	void                            Recover( const char* BeginTime, RecoverDataType Type, RecoverMarket Market, RecoverSession Session = rsAll, UFC::AnsiString RecoverYYYYMMDD = ""  );
	/**
	 * Recover executions from BeginTime to EndTime.
	 *  @Param BeginTime Begin time HHMMSS
	 *  @Param EndTime End time HHMMSS
	 *  @Param Type Order confirmation ,fill execution or both.
	 *  @Param Market Futures,Options,TSEor OTC
	 */
	void                            Recover( const char* BeginTime, const char* EndTime,RecoverDataType Type, RecoverMarket Market, RecoverSession Session = rsAll, UFC::AnsiString RecoverYYYYMMDD = ""  );
	void                            RecoverFromLocalFile( const char* BeginTime, RecoverDataType Type, RecoverMarket Market, RecoverSession RecoverType );
	void                            Lock( bool Locked );
	void                            Lock( const char* ID, bool Locked );
public:  ///< Public functions for order placement.
	/**
	 * To place a new order
	 *  @param Msg New order message.
	 */
	void                            NewOrder( TNewOrderMessage* Msg );
	/**
	 * To replace an order.
	 * In TAIFEX, you can reduce order qty or change price, not support increasing qty.
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
	 * To request a file.(for TAIFEX X.25 only)
	 *  @param Market Futures or Options.
	 *  @param FileCode File code for request file.
	 *  @Param ReqData Data body for request file.
	 */
	void                            RequestFile( nsOrderMessageDefine::MarketEnum Market, const char* FileCode, const char* ReqData );
	/**
	 * To request a file.(for TAIFEX TCP/IP TMP only)
	 *  @param Market Futures or Options.
	 *  @param IsMorningSession TRUE for morning trading session, FALSE for afternoon trading session.
	 *  @Param ReqID TMP FTP request ID.
	 *  @Param Body Data body for request file.
	 */
	void                            RequestFile( nsOrderMessageDefine::MarketEnum Market, BOOL IsMorningSession, int ReqID, const char* Body );
	int                             SendNewsRequest( TNewsMessage* ReqMsg );
	int                             SendNewsRequest( const UFC::AnsiString& FuncStr, const UFC::AnsiString& DataStr, int Count,int index );
	/**
	* To Add,Pause,Active,Remove,Query TouchOrder
	*   @param TouchOrderCmd command to control touch order   
	*/
	BOOL                            TouchOrderControl(TTouchOrderCommand* TouchOrderCmd); // added by Kenny to support touch order management.
public: ///< Functions to setup order connection.
	bool                            GetEnablePendingNewAck( void )      { return FEnablePendingNewAck; }
	void                            SetEnablePendingNewAck( bool Value ){ FEnablePendingNewAck = Value; }
	bool                            GetUseNewFuturesSymbol( void )      { return FNewFutSymbol; }
	void                            SetUseNewFuturesSymbol( bool Value ){ FNewFutSymbol = Value; }
	/**
	 * Write logs to given file
	 *  @param FileName log file name.
	 */
	void                            SetLogFile( const char* FileName );
	void                            SetLogFileEx( const char* FileName );
	/**
	 *  Is this a administration connection?( Administration connection will received all executions.)
	 *  @return TRUE for administration connection.
	 */
	BOOL                            IsAdmin()                      { return FAdmin; }
	/**
	 *  The language of error message.(default English)
	 *  @param Lenguage Language.
	*/
	void                            SetLanguage( MessageLanguage Lenguage );
	/**
	 *  Set broker ID.
	 *  @param Market which market.
	 *  @param FCMID broker ID.
	 */
	MessageLanguage                 GetLanguage( ) { return FLanguage; }
	static void                     SetBrokerID( nsOrderMessageDefine::MarketEnum Market, const char* FCMID);
	static const UFC::AnsiString&   GetBrokerID( nsOrderMessageDefine::MarketEnum Market );
	/**
	 *  Set clear member ID.
	 *  @param Market which market.
	 *  @param CMID clear member ID.
	 */
	void                            SetCMID( const char* CMID )    { FCMID = CMID; }
	const UFC::AnsiString&          GetCMID( void )                { return FCMID; }
	const UFC::AnsiString&          GetUserName( void )            { return FUserName; }
	void                            Stdout( BOOL ToStdOut );

public:
	/** Constructor for COM only.
	 */
	TTaifexConnection(  HINSTANCE AppInstance,
						HMODULE ResourceInstance,
						const char* ClientAppName,
						IOrderConnectionEventListener* pOrderConnectionEventListener,
						APIType WhichAPI,
						BOOL  IsWin32GUIApp );
	/**
	 * Constructor for using C++ library.
	*  @Param ClientAppName given this client application a name.
	*  @Param pOrderConnectionEventListener Listener to handle execution and connection events.
	 *  @Param WhichAPI use Speedy,CTP,Femas or SunGard API
	*  @Param  IsWin32GUIApp TRUE for Win32 GUI Application, FALSE for Linux, Unix, Win32 console application.
	*/
	TTaifexConnection(  const char* ClientAppName,
						IOrderConnectionEventListener* pOrderConnectionEventListener,
						APIType WhichAPI    = atSpeedy,
						BOOL  IsWin32GUIApp = false );
	/**
	* Destructor.
	*/
	virtual ~TTaifexConnection( void );
	/**
	 * Set OnBeforeSend event listener.
	 * @Param Listener Object pointer which implement ISendMessageListener interface.
	 */
	void    SetBeforeSendListener( ISendMessageListener* Listener ) { FSendEventListener = Listener; }
	int     GetQueueSize( void );
	/**
	 *  Indicates whether the user is login?
	 *  @return TRUE for login.
	 */
	bool    IsLogon( void );
	/**
	 *  Indicates whether the connection to the Order Server is established.
	 *  @return TRUE for already connected.
	 */
	bool    IsConnected( void );
	/**
	 *  Get the trading date.
	 *  @return trading date string.
	 */
	const char* GetTradingDate( void ) { return FTradeingDate.c_str(); }
	/**
	 *  Indicates which API this connection used.
	 *  @return APIType Speedy,CTP,Femas,SunGard API.
	 */
	APIType     UseAPI( void ) { return FUseAPI; }

	bool IsTWOptions( const UFC::AnsiString& Symbol );
	bool IsTWFutures( const UFC::AnsiString& Symbol );
        void CancelWorking( CancelMarket Market, const UFC::AnsiString& AE, const UFC::AnsiString& Account, const UFC::AnsiString& UserData, const UFC::AnsiString& ProdID, const UFC::AnsiString& CancelUDD );
        UFC::BufferedLog* GetLogObj() { return Glog; }
	const UFC::AnsiString& GetLocalIP( void ) { return FLocalIP; }
	void SetTWSE8DigiSeqVersion( bool Is8Digi );        
	bool IsTWSE8DigiSeq( void ) { return FIsTWSE8DigiVersion; }
	void SetThroughput( int PerSec ) { FOrderPerSec = PerSec; }
	void ChangePassword( const UFC::AnsiString& Password, const UFC::AnsiString& NewPassword );
	bool MarginPositionRequest( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, UFC::AnsiString& Result );
	bool GetFNeedCheckOrdQty0() { return FNeedCheckOrdQty0; };
	void SetFNeedCheckOrdQty0( bool NewValue ) { FNeedCheckOrdQty0 = NewValue; };        
public: ///< CA Functions
	UFC::AnsiString GetCAPFXFilePathName() {return FCAPFXFilePathName;};
	void SetCAPFXFilePathName(const UFC::AnsiString& NewValue) {FCAPFXFilePathName = NewValue;};
	UFC::AnsiString GetCAPassword() {return FCAPassword;};
	void SetCAPassword(const UFC::AnsiString& NewValue) {FCAPassword = NewValue;};
	UFC::AnsiString GetCADLLFileName() {return FCADLLFileName;};
	void SetCADLLFileName(const UFC::AnsiString& NewValue) {FCADLLFileName = NewValue;};
	UFC::AnsiString GetCAOrganizationalUnit() {return FCAOrganizationalUnit;};
	void SetCAOrganizationalUnit(const UFC::AnsiString& NewValue) {FCAOrganizationalUnit = NewValue;};
	UFC::AnsiString GetCACommonName() {return FCACommonName;};
	void SetCACommonName(const UFC::AnsiString& NewValue) {FCACommonName = NewValue;};
	void CreateCAObject();
	void DeleteCAObject();
	bool CreateUniFSCAObject();
	bool CreateMLTWCAObject();
	bool CreateTSCGCCAObject();
	bool CheckCALogonData( const UFC::AnsiString& LogonData, CAResultData& CAResult );
};
//------------------------------------------------------------------------------
#endif

