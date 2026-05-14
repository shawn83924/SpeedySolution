//---------------------------------------------------------------------------
#ifndef MarketDataConnectionH
#define MarketDataConnectionH
//---------------------------------------------------------------------------
#include "MarketDataMessage.h"
#include "TMDThreadApp.h"
#include "Utility.h"
#include "NameValueMessage.h"
//------------------------------------------------------------------------------
#define MAX_TICK_COUNT_NO   5
//------------------------------------------------------------------------------
const UFC::Int32 DEFAULT_PORT = 34567;
//------------------------------------------------------------------------------
extern UFC::AnsiString RECOVER_SUBJECT;
extern UFC::AnsiString RECOVER_DYNAMIC_SUBJECT;
extern UFC::AnsiString RECOVER2; // expiremental for recover performance improvement.
extern UFC::AnsiString RECOVER_RESPONSE;
extern UFC::AnsiString DOWNLOAD_SUBJECT;
extern UFC::AnsiString DEWNLOAD_RESPONSE;
extern UFC::AnsiString SERVER_ACK;
extern UFC::AnsiString SERVER_RELOAD;
extern UFC::AnsiString SERVER_RELOAD_DATA;
extern UFC::AnsiString RECOVER_FINISHED; // finish message for subscribe2
//------------------------------------------------------------------------------
typedef enum
{
    stSnapshot = 0,
    stSubscribe = 1,
    stSnapshotWithUpdate = 2
}SubscribeType;
//------------------------------------------------------------------------------
class TSymRegInfo
{
private:
    int FRef;
public:
    TSymRegInfo():FRef(0){}
    int Add( void )
    {
        FRef++;
        return FRef;
    }
    int Del( void )
    {
        FRef--;
        return FRef;
    }
};
//------------------------------------------------------------------------------
class TExRegInfo
{
private:
    UFC::AnsiString FExchange;
    int  FRegMax;
    bool FRegAll;
    UFC::PHashMap<UFC::AnsiString, TSymRegInfo* > FRegTable;
public:
    TExRegInfo( const UFC::AnsiString& Exchange, int Max, bool RegAll = false );
    ~TExRegInfo( void );
    void Clear( void );
    bool RegAll( void );
    bool AddReg( const UFC::AnsiString& Symbol );
    void DelReg( const UFC::AnsiString& Symbol );
};
//------------------------------------------------------------------------------
class IMarketDataConnectionEventListener
{
public:
    virtual void OnConnected( void ) = 0;
    virtual void OnLogonReply( bool OK,const UFC::AnsiString& Msg ) = 0;
    virtual void OnDisconnected( void ) = 0;
    virtual void OnContractDownloadComplete( int ExchangeCount, int UseMS ) = 0;
    virtual void OnServerAck( int RTT ) = 0;
    virtual void OnLeaderboard( UFC::PStringList& Board ) = 0;
    virtual void OnServerTimeDiffUS( UFCType::Int64 TimeDiffUS, UFCType::Int64 RTTUS ) = 0;
    virtual void OnNews(  const UFC::AnsiString& Group, const UFC::AnsiString& Message ) = 0;
    virtual void OnReload( const UFC::AnsiString& Exchange, UFC::PStringList& IDList, const UFC::AnsiString& ReloadID ) = 0;
    virtual void OnReload( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID ) = 0;
    virtual void OnReloadComplete( const UFC::AnsiString& Exchange, UFC::PStringList& SymbolList ) = 0;
    virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MarketDataMessage* Msg ) = 0;
    virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, BasicData* Msg ) = 0;
    virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MatchInfo* Msg ) = 0;
    virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, TotalMatch* Msg )  = 0;
    virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, DayHighLowPrice* Msg ) = 0;
    virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OpeningInfo* Msg ) = 0;
    virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, OrderBookData* Msg )= 0;
    virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, ClosingMarketData* Msg ) = 0;
    virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, UnderlyingIndexInfo* Msg ) = 0;
    virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, SumOfOrderInfo* Msg ) = 0;
    virtual void OnMarketDataMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, HKBrokerQueue* Msg ) = 0;
    virtual void OnSystemMessage( const UFC::AnsiString& Exchange, SystemMessage* Message ) =0;
    virtual void OnErrorMessage( const UFC::AnsiString& Exchange, ErrorMessage* Message ) =0;
    virtual void OnRecoverFinished( const UFC::AnsiString& Exchange, int Count ) = 0;
    virtual void OnRecoverFinished( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, int Count ) = 0;
};
//------------------------------------------------------------------------------
class IAdvancedMarketDataEventListener
{
public:
    virtual void OnMarketDataMessage(const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, AdvancedMessage* Msg) = 0;
};
//------------------------------------------------------------------------------
class MarketDataConnection : public UFC::PThread, public MessageListener, public MAppListener
{
private:
    MApp* 						FApp;
    UFC::AnsiString				FAppName;
    UFC::AnsiString				FRecoverKey;
    UFC::PCriticalSection		FCS;
    UFC::PHashedList<UFC::AnsiString, MSubscriber*> FSubscribeTable;
    UFC::PHashedList<UFC::AnsiString, UFC::PHashedList<UFC::AnsiString, BasicInformation*>*> FTable;
    UFC::PHashedSet<UFC::AnsiString> FExchangeTable;
    UFC::PHashedSet<UFC::AnsiString> FSupportExchange;
    UFC::PHashedList<UFC::AnsiString, UFC::AnsiString*>  FSpotTable;
    UFC::PHashedList<UFC::AnsiString, UFC::PStringList*> FIndexTable;
    TMDThreadApp*   			FThread;
    HINSTANCE 					FInstance;
    UFC::UInt32					FTickCount;
    UFC::UInt64					FAcuRecvBytes;
    UFC::UInt64					FAcuRecvMsgs;
    UFC::Int64					FTickCountUS[MAX_TICK_COUNT_NO];
    UFC::Int64					FTickLagUS[MAX_TICK_COUNT_NO];
    char*                       FData;
    int							FDataSize;
    BOOL                        FIsDebugMode;
    BOOL                        FIsDebugPerformance;
    BOOL                        FLimitSubscribe;
    BOOL                        FIsAdvancedMode;
    UFC::AnsiString				FAppVersion;
    UFC::AnsiString				FServerMode;
private:
    IMarketDataConnectionEventListener* FListener;
    IAdvancedMarketDataEventListener*   FAdvancedListener;
private: ///< Impelment interface MAppListener
    virtual void                    OnMAppConnected( void );
    virtual void                    OnMAppDisconnected( void );
    virtual void                    OnMAppError( PMAppError Error );
private:  ///< Impelment interface MessageListener
    virtual void                    OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* pTree );
    virtual void Execute( void );
private:
    BOOL Send( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Msg );
    void RecoverSnapshot( const UFC::AnsiString& ExString, const UFC::AnsiString& SymString, BOOL NeedAddListener );
    void AddListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key );
    void DelListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key );
    void RemoveAllListener( void );
    void RecoverDynamicSnapshot( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, const UFC::AnsiString& SettlementMonth,
         CallPutCode cpCode, double StrikePx, BOOL NeedAddListener, BOOL NeedSnapshot = TRUE );

    UFC::Int32 ParseContract( char* Data, int Size  );
    void FillCloseMarketData( UFC::PStream* Stream, ClosingMarketData& Msg, UFC::Int32 DecimalLocator );
    void LoadFromBinary( UFC::MemoryReadStream* Stream, MarketDataMessage& Msg );
    void OnOrderBookMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, MTree* Data  );
    void OnMatchMessage( const UFC::AnsiString& Exchcnage, const UFC::AnsiString& Symbol, Market mkt, MTree* Data );
    void OnUnderlyingIndex( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, MTree* Data );
    void OnDayHighLow( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, MTree* Data );
    void OnSumOfOrderInfo( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, MTree* Data );
    void OnOpeningInfo( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, MTree* Data );
    void OnCloseMarketData( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, MTree* Data );
    void OnCloseMarketDataWithSettlementPx( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, MTree* Data );
    void OnCloseMarketDataWithSettlementPxAndOpenInterest( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, MTree* Data );
    void DigitToTimeStamp( UFC::Int32 Time, UFC::AnsiString& TimeStamp );
    void IntToTimeStamp( UFC::Int32 Time, UFC::Int32 MS, UFC::AnsiString& TimeStamp );
    void ToTimeStamp( const UFC::UDateTime& DateTime, UFC::AnsiString& TimeStamp );
    void FetchHeader( UFC::PStream* Stream, BaseMessage& Msg );
    void SetUnderlying( void );
    void ClearRegInfo( void );
private:
    typedef void (MarketDataConnection::*MDEventFunc)( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Msg );
private:
    MDEventFunc                                     FOnRecoverMsg;
    MDEventFunc                                     FOnContractDownload;
    MDEventFunc                                     FOnServerAck;
    MDEventFunc                                     FOnReloadMessage;
    MDEventFunc                                     FOnReloadComplete;
    MDEventFunc                                     FOnSelfEvent;
    MDEventFunc                                     FOnCMEMarketData;
    MDEventFunc                                     FOnTWSEMessage;
    MDEventFunc                                     FOnTAIFEXMessage;
    MDEventFunc                                     FOnHKExMessage;
    MDEventFunc                                     FOnChinaMarketData;
    MDEventFunc                                     FOnCQGMarketData; 
    MDEventFunc                                     FOnNews;
    MDEventFunc                                     FOnRecoverFinished;
    MDEventFunc                                     FOnLoginReply;
    UFC::PHashedList<UFC::AnsiString, MDEventFunc*> FCallbackFuncs;
private:
    UFC::AnsiString			                    	FLogonID;
    UFC::PHashMap<UFC::AnsiString,TExRegInfo*>      FSubscribeExchange;
    bool                                            RegSymbol( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
    void                                            UnregSymbol( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
    bool                                            CanRegExchange( const UFC::AnsiString& Exchange );
private:
    void OnNews( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data );
    void OnRecoverMsg( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data );
    void OnContractDownload( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data );
    void OnServerAck( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data );
    void OnReloadMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data );
    void OnReloadComplete( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data );
    void OnSelfEvent( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data );
    void OnRecoverFinished( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data );
    void OnLoginReply( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data );
    ///< TAIFEX Message Handler
    void OnTAIFEXMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MTree* Data );
    void OnTAIFEXOrderBookMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream, int TSession );
    void OnTAIFEXMatchMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream, int TSession );
    void OnTAIFEXUnderlyingIndex( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnTAIFEXDayHighLow( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnTAIFEXSumOfOrderInfo( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnTAIFEXOpeningInfo( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnTAIFEXCloseMarketData( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnTAIFEXCloseMarketDataWithSettlementPx( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnTAIFEXCloseMarketDataWithSettlementPxAndOpenInterest( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnTAIFEXSystemMsg( const UFC::AnsiString& Exchange, Market mkt, UFC::PStream* Stream, MTree* Data );
    ///< CME like Message Handler
    void OnCMEMarketData( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MTree* Data );
    void OnCMETrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnCMETotalVol( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnCMEOrderBook( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnCMEDayHighLow( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnCMEOpen( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnCMEIndex( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnCMESettlePx( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    ///< TWSE/OTC Message Handler
    void OnTWSEMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MTree* Data );
    void OnTWSETrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream, int TSession );
    void OnTWSETotalTrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream, int TSession );
    void OnTWSEOrderBook( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream, int TSession );
    void OnTWSEDayHighLow( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnTWSEOpen( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnTWSEIndex( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnTWSEClose( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnTWSEAdvancedMessage(const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream, MTree* Data);
    ///< TWSE/OTC Message Handler
    void OnHKExMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MTree* Data );
    void OnHKExTrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnHKExTotalTrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnHKExOrderBook( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream, int TSession );
    void OnHKExDayHighLow( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnHKExOpen( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnHKExIndex( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnHKExClose( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnHKExNews( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnHKExBrokerQueue( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, bool IsBuy, UFC::PStream* Stream );
    void OnHKExSuspensionIndicator( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    ///< China Message Handler
    void OnChinaMarketData( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MTree* Data );
    void OnChinaFullSnapShot( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    ///< CQG Message Handler
    void OnCQGMessage( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, MTree* Data );
    void OnCQGNews( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnCQGOpen( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnCQGOrderBookData( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream);
    void OnCQGSettlementPx( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream);
    void OnCQGSettlementPxWithOpenInterest( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream);
    void OnCQGDayHighLow( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream );
    void OnCQGTrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream);
    void OnCQGTotalTrade( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, Market mkt, UFC::PStream* Stream);
        
public:
    /**
    * Constractor for using C++ library.
    *  @Param Instance For Win32 GUI Applications. Linux, Unix ignore this param.
    *  @Param AppName given this client application a name.
    *  @Param pListener Listener to handle marketdata events.
    *  @Param IsWin32GUIApp TRUE for Win32 GUI Application, FALSE for Linux, Unix, Win32 console application.
    *  @Param QueueSize Size of the queue to store marketdata message.( default: 8192)
    */
    MarketDataConnection( HINSTANCE                           Instance,
                                              const UFC::AnsiString&              AppName,
                                              IMarketDataConnectionEventListener* pListener,
                                              BOOL                                IsWin32GUIApp,
                                              BOOL                                IsDebugMode = FALSE,
                      BOOL                                IsDebugPerformance = FALSE,
                                              int                                 QueueSize = MApp::MSG_QUEUE_SIZE );
    /**
    * Destructor.
    */
    ~MarketDataConnection();
public:
    /**
     * Try to connect a StarWave Gateway.
     *  @param IPAddress IP addreess.
     *  @Param Port Port.(Default 34567)
     *  @Param Timeout Connection timeout.(Default 10 sec)
     */
    void Connect( const UFC::AnsiString& IPAddress, UFC::Int32 Port = DEFAULT_PORT, int Timeout = 10 );
    /**
     * Try to connect a StarWave Gateway.
     *  @param IPAddress IP addreess.
     *  @Param Port Port.(Default 34567)
     *  @Param Timeout Connection timeout.(Default 10 sec)
     */
    bool Logon(  const UFC::AnsiString& ID, const UFC::AnsiString& Password, UFC::AnsiString& Msg );

    /**
     * Try to close a StarWave Marketdata Gateway connection.
     *
     */
    void Disconnect( void );

    /**
     *  Is connected?
     */
    bool IsConnected( void );

    /**
     * Subscribe marketdata.
     *  @param Exchange Exahange code.
     *  @Param Symbol   Contract symbol.
     *  @Param Type    stSnapshot:  Snapshot only.
     *	               stSubscribe: new update only.
     *                 stSnapshotWithUpdate: Snapshot with update.
     */
    bool Subscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, SubscribeType Type );
    void SubscribeX( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, const UFC::AnsiString& SettlementMonth,
                     CallPutCode cpCode, double StrikePx, SubscribeType Type );

    /**
     * Unsubscribe marketdata.
     *  @param Exchange Exahange code.
     *  @Param Symbol   Contract symbol.
     */
    void Unsubscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );

    /**
     * Unsubscribe all marketdata.
     *
     */
    void UnsubscribeAll( void );

    /**
     *   Recover the marketdata smapshot by exchange.
     *  * Will trigger OnRecoverFinished when recover finished.
     */
    bool Recover( const UFC::AnsiString& Exchange );
    void Recover( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID );
    void RecoverSubscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& AppIdentification );
    void RecoverSubscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, const UFC::AnsiString& AppIdentification );

    /**
     *  Ask StarWave to download all contract information.
     *  * Will trigger OnContractDownloadComplete when download finished.
     */
    void DownloadContract( void );
    void ReloadExchangeContract( const UFC::AnsiString& Exchange, UFC::PStringList& IDList );

    /**
     * Clear all downloaded contracts information.
     *
     */
    void Clear( void );

    /**
     *  How many exhanges does this StarWave support.
     */
    UFC::Int32        ExchangeCount( void );
    /**
     *  Get the exchange code by index
     *  @Param Index Index of exchange.( Zero based )
     */
    UFC::AnsiString   GetExchange( UFC::Int32 Index );
    /**
     *  How many symbols does this Excahnge has.
     *  @Param Exchange Exchange code
     */
    UFC::Int32        SymbolCount( const UFC::AnsiString& Exchange );
    /**
     *  Get the symbol string by index
     *  @Param Exchange Exchange code
     *  @Param Index Index of symbol.( Zero based )
     */
    UFC::AnsiString   GetSymbol( const UFC::AnsiString& Exchange, UFC::Int32 Index );
    /**
     *  Get the contract basic information
     *  @Param Exchange Exchange code
     *  @Param Symbol contract symbol.
     */
    BasicInformation* GetBasicInformation( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
    /**
     *  Get the contract basic information
     *  @Param Exchange Exchange code
     *  @Param Index Index of symbol.( Zero based )
     */
    BasicInformation* GetBasicInformation( const UFC::AnsiString& Exchange, UFC::Int32 Index );
    /**
     *  Get the contract basic information list of an exchange
     *  @Param Exchange Exchange code
     */
    UFC::PHashedList<UFC::AnsiString, BasicInformation*>* GetSymbolsByExchange( const UFC::AnsiString& Exchange );
    /**
     * Write logs to given file
     *  @param FileName log file name.
     */
    void                            SetLogFile( const char* FileName );

    /**
     *  Market data connection support exchanges.
     *
     */
    void AddSupportExchange( const UFC::AnsiString& Exchange );
    void ClearSupportExchange( void );
    bool SupportExchange( const UFC::AnsiString& Exchange );
    /**
     *  Subscribe System Message
     *
     */
    void SubscribeSystemMessage( const UFC::AnsiString& Exchange );
    void UnsubscribeSystemMessage( const UFC::AnsiString& Exchange );
    /**
     *  Subscribe News
     *
     */
    void SendNews( const UFC::AnsiString& Group, const UFC::AnsiString& Message );
    bool SubscribeNews( const UFC::AnsiString& Group );
    void UnsubscribeNews( const UFC::AnsiString& Group );
    bool SubscribeExchangeNews( const UFC::AnsiString& Exchange );
    void UnsubscribeExchangeNews( const UFC::AnsiString& Exchange );


    void SendMessageToServer( const UFC::AnsiString& Msg ,int Func );
    void SendTickCountToServer( void );
    /**
     *  Utility functions
     *
     */
    UFC::AnsiString& GetLocalIPAddress( void ) { return FApp->GetLocalIPAddress() ; }
    UFC::UInt64      GetAcuRecvBytes( void ) { return FAcuRecvBytes; }
    UFC::UInt64      GetAcuRecvMsgs( void )  { return FAcuRecvMsgs; }
    void SetAppversion( const UFC::AnsiString& AppVersion ) { FAppVersion = AppVersion;}
    const UFC::AnsiString& GetAppversion( void ) { return FAppVersion;}
    void                   SetIP( const UFC::AnsiString& IP );
    const UFC::AnsiString& GetIP( void );
    void                   SetPort( UFC::Int32 port );
    UFC::Int32             GetPort( void );
};
//------------------------------------------------------------------------------
#endif
