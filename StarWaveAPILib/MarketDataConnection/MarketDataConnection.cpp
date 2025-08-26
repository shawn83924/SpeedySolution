//---------------------------------------------------------------------------
#include "MarketDataConnection.h"
//---------------------------------------------------------------------------
const UFC::AnsiString gLocalhostIP("127.0.0.0");
UFC::AnsiString RECOVER_SUBJECT( "SW.RECOVER" );
UFC::AnsiString RECOVER_DYNAMIC_SUBJECT( "SW.RECOVER.DYNAMIC" );
UFC::AnsiString RECOVER2( "SW.RECOVER2" ); // expiremental for recover performance improvement.
UFC::AnsiString RECOVER_RESPONSE( "RECOVER.RESPONSE" );
UFC::AnsiString DOWNLOAD_SUBJECT( "FT.REQUEST" );
UFC::AnsiString DEWNLOAD_RESPONSE( "FT.RESPONSE" );
UFC::AnsiString SERVER_ACK( "SW.ACK" );
UFC::AnsiString SERVER_RELOAD( "SW.RELOAD" );
UFC::AnsiString SERVER_RELOAD_DATA( "SW.RELOAD.DATA" );
UFC::AnsiString RECOVER_FINISHED( "RECOVER.FINISHED" ); // finish message for subscribe2
UFC::AnsiString LOGIN_REQ( "SW.LOGIN" );
UFC::AnsiString LOGIN_REPLY( "SW.LOGIN.REPLY" );
//---------------------------------------------------------------------------
TExRegInfo::TExRegInfo( const UFC::AnsiString& Exchange, int Max, bool RegAll )
:FExchange( Exchange )
,FRegMax( Max )
,FRegAll( RegAll )
{

}
//---------------------------------------------------------------------------
TExRegInfo::~TExRegInfo( void )
{
    Clear();
}
//---------------------------------------------------------------------------
void TExRegInfo::Clear( void )
{
    TSymRegInfo* RegInfo = FRegTable.First();
    while( RegInfo != NULL)
    {
            delete RegInfo;
            RegInfo = FRegTable.Next();
    }
    FRegTable.Clear();
}
//---------------------------------------------------------------------------
 bool TExRegInfo::RegAll( void )
 {
    return FRegAll;
 }
//---------------------------------------------------------------------------
bool TExRegInfo::AddReg( const UFC::AnsiString& Symbol )
{
    if( FRegTable.ItemCount() >= FRegMax )
            return false;
    TSymRegInfo* RegInfo;
    if( FRegTable.GetObjectByKey( Symbol,RegInfo ) == false )
    {
            RegInfo = new TSymRegInfo();
            FRegTable.Add( Symbol, RegInfo );
    }
    RegInfo->Add();
    return true;
}
//---------------------------------------------------------------------------
void TExRegInfo::DelReg( const UFC::AnsiString& Symbol )
{
    TSymRegInfo* RegInfo;
    if( FRegTable.GetObjectByKey( Symbol,RegInfo ) == TRUE )
    {
            int Count = RegInfo->Del();
            if( Count <= 0 )
            {
                    FRegTable.DeleteByKey( Symbol );
                    delete RegInfo;
            }
    }
}
//---------------------------------------------------------------------------
//
//  Class MarketDataConnection
//
//---------------------------------------------------------------------------
MarketDataConnection::MarketDataConnection( HINSTANCE Instance,
											const UFC::AnsiString& AppName,
											IMarketDataConnectionEventListener* pListener,
											BOOL IsWin32GUIApp,
											BOOL IsDebugMode,
											BOOL IsDebugPerformance,
											int QueueSize )
:UFC::PThread( NULL, FALSE )
,FInstance( Instance )
,FAppName( AppName )
,FListener( pListener )
,FIsDebugMode( IsDebugMode )
,FIsDebugPerformance( IsDebugPerformance )
,FTickCount( 0 )
,FAcuRecvBytes( 0 )
,FAcuRecvMsgs( 0 )
,FOnRecoverMsg( &MarketDataConnection::OnRecoverMsg )
,FOnContractDownload( &MarketDataConnection::OnContractDownload )
,FOnServerAck( &MarketDataConnection::OnServerAck )
,FOnReloadMessage( &MarketDataConnection::OnReloadMessage )
,FOnReloadComplete( &MarketDataConnection::OnReloadComplete )
,FOnSelfEvent( &MarketDataConnection::OnSelfEvent )
,FOnCMEMarketData( &MarketDataConnection::OnCMEMarketData )
,FOnTWSEMessage( &MarketDataConnection::OnTWSEMessage )
,FOnTAIFEXMessage( &MarketDataConnection::OnTAIFEXMessage )
,FOnHKExMessage( &MarketDataConnection::OnHKExMessage )
,FOnChinaMarketData( &MarketDataConnection::OnChinaMarketData )
,FOnCQGMarketData( &MarketDataConnection::OnCQGMessage )
,FOnNews( &MarketDataConnection::OnNews )
,FOnRecoverFinished( &MarketDataConnection::OnRecoverFinished )
,FOnLoginReply( &MarketDataConnection::OnLoginReply )
,FAppVersion( "2.0.0.99" )
{
    if( FIsDebugMode )
    {
        Int32 DEBUG_LEVEL = 0;
        UFC::AnsiString Today, FileName;

        DEBUG_LEVEL |= UFC::dlInformation;
        UFC::GetYYYYMMDD( Today );

        UFC::CreateDir( "./log" );
#ifndef _WIN32
        FileName.Printf( "./log/%s.%s.%d.log", "StarWaveAPI", Today.c_str(), getpid() );
#else
        FileName.Printf( "./log/%s.%s.%d.log", "StarWaveAPI", Today.c_str(), GetCurrentProcessId() );
#endif
        UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( FileName, 10240, TRUE, FALSE ) );
        UFC::BufferedLog::SetDebugMode( DEBUG_LEVEL );
    }

    for(int i = 0; i < MAX_TICK_COUNT_NO; i++)
    {
        FTickCountUS[i] = 0;
        FTickLagUS[i] = 0;
    }

#ifndef _WIN32
	FApp = new MApp();
#else
	if( IsWin32GUIApp == TRUE )
		FApp = new MWinApp( Instance );
	else
		FApp =  new MApp( MApp::MAPP_DEF_PORT, QueueSize );
#endif
    FApp->SetAppName( AppName );
    FApp->SetListener( this );
    FThread = new TMDThreadApp( FApp );
    ///< Listen Recover subject
    FRecoverKey.Printf( "%s@%s.%d", FAppName.c_str(), UFC::Hostname, UFC::GetTickCountMS() );
    MSubscriber* NewSubscriber = new MSubscriber( "RECOVER.OBJECT", RECOVER_RESPONSE, FRecoverKey );
    NewSubscriber->addListener( this );
    FApp->AddSubscriber( NewSubscriber );

    ///< Listen File transfer subject.
    NewSubscriber = new MSubscriber( "FT.OBJECT", DEWNLOAD_RESPONSE, FRecoverKey );
    NewSubscriber->addListener( this );
    FApp->AddSubscriber( NewSubscriber );

    ///< Listen Server Ack
    NewSubscriber = new MSubscriber( "SW.ACK.ONJECT", SERVER_ACK, FRecoverKey );
    NewSubscriber->addListener( this );
    FApp->AddSubscriber( NewSubscriber );

    ///< Listen Server Ack
    NewSubscriber = new MSubscriber( "SelfSubscribe","Self", "Event" );
    NewSubscriber->addListener( this );
    FApp->AddSubscriber( NewSubscriber );

    ///< Listen Server Reload Message
    NewSubscriber = new MSubscriber( "SW.RELOAD", SERVER_RELOAD, "all" );
    //NewSubscriber = new MSubscriber( "SW.RELOAD", SERVER_RELOAD, FRecoverKey );
    NewSubscriber->addListener( this );
    FApp->AddSubscriber( NewSubscriber );

    ///< Listen Server Reload Data
    //NewSubscriber = new MSubscriber( "SE.RELOAD.COMPLETE", SERVER_RELOAD_DATA, "all" );
    NewSubscriber = new MSubscriber( "SE.RELOAD.COMPLETE", SERVER_RELOAD_DATA, FRecoverKey );
    NewSubscriber->addListener( this );
    FApp->AddSubscriber( NewSubscriber );

    ///< Public News
    NewSubscriber = new MSubscriber( "NEWS.PUBLIC", "NEWS", "Public" );
    NewSubscriber->addListener( this );
    FApp->AddSubscriber( NewSubscriber );

    ///< Recover2 finished event
    NewSubscriber = new MSubscriber( "RECOVER.FINISHED", RECOVER_FINISHED, FRecoverKey );
    NewSubscriber->addListener( this );
    FApp->AddSubscriber( NewSubscriber );

    ///< LogonReply finished event
    NewSubscriber = new MSubscriber( "SW.LOGIN.REPLY", LOGIN_REPLY, FRecoverKey );
    NewSubscriber->addListener( this );
    FApp->AddSubscriber( NewSubscriber );

    ///< Command message handler
    FCallbackFuncs.Add( RECOVER_RESPONSE,  &FOnRecoverMsg );
    FCallbackFuncs.Add( DEWNLOAD_RESPONSE, &FOnContractDownload );
    FCallbackFuncs.Add( SERVER_ACK, 	   &FOnServerAck );
    FCallbackFuncs.Add( SERVER_RELOAD, 	   &FOnReloadMessage );
    FCallbackFuncs.Add( SERVER_RELOAD_DATA,&FOnReloadComplete );
    FCallbackFuncs.Add( RECOVER_FINISHED,  &FOnRecoverFinished );
    FCallbackFuncs.Add( LOGIN_REPLY,       &FOnLoginReply );

    FCallbackFuncs.Add( "NEWS",            &FOnNews );
    FCallbackFuncs.Add( "Self",            &FOnSelfEvent );
    //** Steps to add new Exchange **
    //   Add callback function for each Exchange
    ///< CME Group
    FCallbackFuncs.Add( "CME",             &FOnCMEMarketData );//1
    FCallbackFuncs.Add( "CBOT",            &FOnCMEMarketData );//2
    FCallbackFuncs.Add( "SGX",             &FOnCMEMarketData );//3
    FCallbackFuncs.Add( "NYMEX",           &FOnCMEMarketData );//4
    FCallbackFuncs.Add( "NYBOT",           &FOnCMEMarketData );//5
    FCallbackFuncs.Add( "HKE",             &FOnCMEMarketData );//6 //CTF
    FCallbackFuncs.Add( "TOCOM",           &FOnCMEMarketData );//7
    FCallbackFuncs.Add( "OSE",             &FOnCMEMarketData );//8
    FCallbackFuncs.Add( "TGE",             &FOnCMEMarketData );//9
    FCallbackFuncs.Add( "LIF",             &FOnCMEMarketData );//10
    FCallbackFuncs.Add( "EUX",             &FOnCMEMarketData );//11
    FCallbackFuncs.Add( "LME",             &FOnCMEMarketData );//12
    FCallbackFuncs.Add( "KRX",             &FOnCMEMarketData );//13
    FCallbackFuncs.Add( "ICE",             &FOnCMEMarketData );//21
    FCallbackFuncs.Add( "BMD",             &FOnCMEMarketData );//22
    FCallbackFuncs.Add( "ASX",             &FOnCMEMarketData );//23
    ///< Taiwan
    FCallbackFuncs.Add( "TWSE",            &FOnTWSEMessage );//14
    FCallbackFuncs.Add( "OTC",             &FOnTWSEMessage );//15
    FCallbackFuncs.Add( "TWSEOdd",         &FOnTWSEMessage );//26
    FCallbackFuncs.Add( "OTCOdd",          &FOnTWSEMessage );//27
    FCallbackFuncs.Add( "ES",              &FOnTWSEMessage );//15
    FCallbackFuncs.Add( "TAIFEX",          &FOnTAIFEXMessage );//16
    FCallbackFuncs.Add( "TFX",             &FOnTAIFEXMessage );//16.1
    FCallbackFuncs.Add( "HKEx",            &FOnHKExMessage );//6    


    ///< China
    FCallbackFuncs.Add( "SHFE",            &FOnChinaMarketData );//17
    FCallbackFuncs.Add( "CFFEX",           &FOnChinaMarketData );//18
    FCallbackFuncs.Add( "DCE",             &FOnChinaMarketData );//19
    FCallbackFuncs.Add( "CZCE",            &FOnChinaMarketData );//20
    FCallbackFuncs.Add( "SSE",             &FOnChinaMarketData );//23
    FCallbackFuncs.Add( "INE",             &FOnChinaMarketData );//24

    ///< CQG
    FCallbackFuncs.Add( "CQG.CME",       &FOnCQGMarketData );//25
    FCallbackFuncs.Add( "CQG.COMEX",     &FOnCQGMarketData );//26
    FCallbackFuncs.Add( "CQG.CBOT",      &FOnCQGMarketData );//27
    FCallbackFuncs.Add( "CQG.CBT",       &FOnCQGMarketData );//28
    FCallbackFuncs.Add( "CQG.NYMEX",     &FOnCQGMarketData );//29
    FCallbackFuncs.Add( "CQG.NYM",		 &FOnCQGMarketData );//30	
    FCallbackFuncs.Add( "CQG.SGX",       &FOnCQGMarketData );//31
    FCallbackFuncs.Add( "CQG.HKF",       &FOnCQGMarketData );//32
    FCallbackFuncs.Add( "CQG.HKE",       &FOnCQGMarketData );//33
    FCallbackFuncs.Add( "CQG.EURONEXT",	 &FOnCQGMarketData );//34

    FCallbackFuncs.Add( "CQG.EUREX",     &FOnCQGMarketData );//35
    FCallbackFuncs.Add( "CQG.EUX",		 &FOnCQGMarketData );//36
    FCallbackFuncs.Add( "CQG.EUXTW",	 &FOnCQGMarketData );//37

    FCallbackFuncs.Add( "CQG.TOCOM",	 &FOnCQGMarketData );//38
    FCallbackFuncs.Add( "CQG.TCE",       &FOnCQGMarketData );//39
    FCallbackFuncs.Add( "CQG.OSE",       &FOnCQGMarketData );//40


    FCallbackFuncs.Add( "CQG.NYB",		 &FOnCQGMarketData );//41
    FCallbackFuncs.Add( "CQG.NYBOT",	 &FOnCQGMarketData );//42

    FCallbackFuncs.Add( "CQG.ICE",		 &FOnCQGMarketData );//43
    FCallbackFuncs.Add( "CQG.ICEUS",	 &FOnCQGMarketData );//44
    FCallbackFuncs.Add( "CQG.ICEEU",	 &FOnCQGMarketData );//45
    FCallbackFuncs.Add( "CQG.ICEUK",	 &FOnCQGMarketData );//46
    FCallbackFuncs.Add( "CQG.ICESG",	 &FOnCQGMarketData );//47

    FCallbackFuncs.Add( "CQG.IPE",	 	 &FOnCQGMarketData );//48
    FCallbackFuncs.Add( "CQG.KBT",		 &FOnCQGMarketData );//49
    FCallbackFuncs.Add( "CQG.LIF",		 &FOnCQGMarketData );//50
    FCallbackFuncs.Add( "CQG.LME",		 &FOnCQGMarketData );//51
    FCallbackFuncs.Add( "CQG.MTF",		 &FOnCQGMarketData );//52
    FCallbackFuncs.Add( "CQG.SFE",		 &FOnCQGMarketData );//53
    FCallbackFuncs.Add( "CQG.CBOE",		 &FOnCQGMarketData );//54

    ///<PATS
    FCallbackFuncs.Add( "PATS.CME",		 &FOnCQGMarketData);//55
    FCallbackFuncs.Add( "PATS.COMEX",	 &FOnCQGMarketData);//56
    FCallbackFuncs.Add( "PATS.CBOT",	 &FOnCQGMarketData);//57
    FCallbackFuncs.Add( "PATS.CBT",		 &FOnCQGMarketData);//58
    FCallbackFuncs.Add( "PATS.NYMEX",	 &FOnCQGMarketData);//59
    FCallbackFuncs.Add( "PATS.NYM",		 &FOnCQGMarketData);//60
    FCallbackFuncs.Add( "PATS.SGX",		 &FOnCQGMarketData);//61
    FCallbackFuncs.Add( "PATS.HKF",		 &FOnCQGMarketData);//62
    FCallbackFuncs.Add( "PATS.HKE",		 &FOnCQGMarketData);//63
    FCallbackFuncs.Add( "PATS.EURONEXT", &FOnCQGMarketData);//64

    FCallbackFuncs.Add( "PATS.EUREX",	 &FOnCQGMarketData);//65
    FCallbackFuncs.Add( "PATS.EUX",		 &FOnCQGMarketData);//66
    FCallbackFuncs.Add( "PATS.EUXTW",    &FOnCQGMarketData);//67

    FCallbackFuncs.Add( "PATS.TOCOM",	 &FOnCQGMarketData);//68
    FCallbackFuncs.Add( "PATS.TCE",		 &FOnCQGMarketData);//69
    FCallbackFuncs.Add( "PATS.OSE",		 &FOnCQGMarketData);//70

    FCallbackFuncs.Add( "PATS.NYB",		 &FOnCQGMarketData);//71
    FCallbackFuncs.Add( "PATS.NYBOT",	 &FOnCQGMarketData);//72

    FCallbackFuncs.Add( "PATS.ICE",		 &FOnCQGMarketData);//73
    FCallbackFuncs.Add( "PATS.ICEUS",	 &FOnCQGMarketData);//74
    FCallbackFuncs.Add( "PATS.ICEEU",	 &FOnCQGMarketData);//75
    FCallbackFuncs.Add( "PATS.ICEUK",	 &FOnCQGMarketData);//76
    FCallbackFuncs.Add( "PATS.ICESG",    &FOnCQGMarketData);//77

    FCallbackFuncs.Add( "PATS.IPE",		 &FOnCQGMarketData);//78
    FCallbackFuncs.Add( "PATS.KBT",		 &FOnCQGMarketData);//79
    FCallbackFuncs.Add( "PATS.LIF",		 &FOnCQGMarketData);//80
    FCallbackFuncs.Add( "PATS.LME",		 &FOnCQGMarketData);//81
    FCallbackFuncs.Add( "PATS.MTF",		 &FOnCQGMarketData);//82
    FCallbackFuncs.Add( "PATS.SFE",		 &FOnCQGMarketData);//83
    FCallbackFuncs.Add( "PATS.CBOE",	 &FOnCQGMarketData);//84

    ///< TT
    FCallbackFuncs.Add( "TT.CME",       &FOnCQGMarketData );//85
    FCallbackFuncs.Add( "TT.COMEX",     &FOnCQGMarketData );//86
    FCallbackFuncs.Add( "TT.CBOT",      &FOnCQGMarketData );//87
    FCallbackFuncs.Add( "TT.CBT",       &FOnCQGMarketData );//88
    FCallbackFuncs.Add( "TT.NYMEX",     &FOnCQGMarketData );//89
    FCallbackFuncs.Add( "TT.NYM",		&FOnCQGMarketData );//90	
    FCallbackFuncs.Add( "TT.SGX",       &FOnCQGMarketData );//91
    FCallbackFuncs.Add( "TT.HKF",       &FOnCQGMarketData );//92
    FCallbackFuncs.Add( "TT.HKE",       &FOnCQGMarketData );//93
    FCallbackFuncs.Add( "TT.EURONEXT",	&FOnCQGMarketData );//94

    FCallbackFuncs.Add( "TT.EUREX",     &FOnCQGMarketData );//95
    FCallbackFuncs.Add( "TT.EUX",		&FOnCQGMarketData );//96
    FCallbackFuncs.Add( "TT.EUXTW",		&FOnCQGMarketData );//97

    FCallbackFuncs.Add( "TT.TOCOM",		&FOnCQGMarketData );//98
    FCallbackFuncs.Add( "TT.TCE",       &FOnCQGMarketData );//99
    FCallbackFuncs.Add( "TT.OSE",       &FOnCQGMarketData );//100

    FCallbackFuncs.Add( "TT.NYB",		&FOnCQGMarketData );//101
    FCallbackFuncs.Add( "TT.NYBOT",		&FOnCQGMarketData );//102

    FCallbackFuncs.Add( "TT.ICE",		&FOnCQGMarketData );//103
    FCallbackFuncs.Add( "TT.ICEUS",		&FOnCQGMarketData );//104
    FCallbackFuncs.Add( "TT.ICEEU",		&FOnCQGMarketData );//105
    FCallbackFuncs.Add( "TT.ICEUK",		&FOnCQGMarketData );//106
    FCallbackFuncs.Add( "TT.ICESG",		&FOnCQGMarketData );//107

    FCallbackFuncs.Add( "TT.IPE",	 	 &FOnCQGMarketData );//108
    FCallbackFuncs.Add( "TT.KBT",		 &FOnCQGMarketData );//109
    FCallbackFuncs.Add( "TT.LIF",		 &FOnCQGMarketData );//110
    FCallbackFuncs.Add( "TT.LME",		 &FOnCQGMarketData );//111
    FCallbackFuncs.Add( "TT.MTF",		 &FOnCQGMarketData );//112
    FCallbackFuncs.Add( "TT.SFE",		 &FOnCQGMarketData );//113
    FCallbackFuncs.Add( "TT.CBOE",		 &FOnCQGMarketData );//114
}
//---------------------------------------------------------------------------
MarketDataConnection::~MarketDataConnection()
{
    Clear();
//	delete FThread;
}
//------------------------------------------------------------------------------
void MarketDataConnection::SetLogFile( const char* FileName )
{
    Int32 DEBUG_LEVEL = 0;
    DEBUG_LEVEL |= UFC::dlInformation;

    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( FileName, 10240, TRUE, FALSE ) );
    UFC::BufferedLog::SetDebugMode( DEBUG_LEVEL );
}
//---------------------------------------------------------------------------
void MarketDataConnection::Clear( void )
{
    FExchangeTable.Clear(); ///< Clear Exchange code set.

    UFC::PHashedList<UFC::AnsiString, BasicInformation*>* SymbolTable = FTable.First();

    while( SymbolTable != NULL ) ///< for each Exchange symbol list
    {
        BasicInformation* Info = SymbolTable->First();

        while( Info != NULL ) ///< for each symbol
        {
                delete Info;      ///< Delete the BasicInformation of this symbol.
                Info = SymbolTable->Next( );
        }
        SymbolTable->Clear();
        delete SymbolTable;   ///< Delete the symbol list of this exchange.
        SymbolTable = FTable.Next();
    }
    FTable.Clear(); ///< Clear Exchange symbol list table.

    UFC::AnsiString* DelStr = FSpotTable.First( );
    while( DelStr != NULL )
    {
        delete DelStr;
        DelStr =  FSpotTable.Next( );
    }
    FSpotTable.Clear();
}
//---------------------------------------------------------------------------
void MarketDataConnection::Connect( const UFC::AnsiString& IPAddress, Int32 Port, int Timeout )
{
    FLimitSubscribe = false;
    Disconnect();
    FApp->SetHost( IPAddress );
    FApp->SetPort( Port );
    FApp->Connect( Timeout );
    FApp->WaitForConnected( Timeout );

    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Connect StarWave:[%s:%d]",
            IPAddress.c_str(), Port );
    UFC::BufferedLog::FlushToFile();
}
//---------------------------------------------------------------------------
bool MarketDataConnection::Logon( const UFC::AnsiString& ID, const UFC::AnsiString& Password, UFC::AnsiString& Msg )
{
    FLimitSubscribe = true;
    Disconnect();
    FApp->Connect( 5, 8000 );

    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Connect StarWave:[%s:%d]",	GetIP().c_str(), GetPort() );
    if( FApp->WaitForConnected( 5 ) == TRUE )
    {
        MTree Data;

        Data.append( "ID", ID );
        Data.append( "Password", Password );
        Data.append( "Version", FAppVersion );
        Send( LOGIN_REQ, FRecoverKey, &Data );

        printf( " Send login[%s:%s]", LOGIN_REQ.c_str(), FRecoverKey.c_str() );
        Msg.Printf( "StarWave[%s:%d] Connected!",	GetIP().c_str(), GetPort());
        UFC::BufferedLog::FlushToFile();
        return true;
    }
    Msg.Printf( "Connect StarWave[%s:%d] failed!",	GetIP().c_str(), GetPort());
    UFC::BufferedLog::FlushToFile();
    return false;
}
//---------------------------------------------------------------------------
void MarketDataConnection::Disconnect( void )
{
    if( FApp->IsConnected() == TRUE )
        FApp->Disconnect();
}
//---------------------------------------------------------------------------
BOOL MarketDataConnection::Send( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* pTree )
{
    return FApp->send_by_key( Subject, Key, *pTree, false );
}
//---------------------------------------------------------------------------
void MarketDataConnection::RecoverSnapshot( const UFC::AnsiString& ExString, const UFC::AnsiString& SymString, BOOL NeedAddListener )
{
    MTree Data;

    Data.append( "Exchange", ExString );
    Data.append( "Symbol", SymString );
    Data.append( "NAL", NeedAddListener );
    Send( RECOVER_SUBJECT, FRecoverKey, &Data );
}
//---------------------------------------------------------------------------
void MarketDataConnection::AddListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key )
{
    UFC::PLockObject Lock( FCS );
    UFC::AnsiString  SubscriberName;

    SubscriberName.Printf( "%s.%s", Subject.c_str(), Key.c_str() );
    if( FSubscribeTable.IsExists( SubscriberName ) == FALSE )
    {
        MSubscriber* NewSubscriber = new MSubscriber( SubscriberName, Subject, Key );
        NewSubscriber->addListener( this );
        FApp->AddSubscriber( NewSubscriber );
        FSubscribeTable.Add( SubscriberName, NewSubscriber );

        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " AddListener:[%s][%s]",
                Subject.c_str(), Key.c_str() );
    }
}
//---------------------------------------------------------------------------
void MarketDataConnection::DelListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key )
{
    UFC::PLockObject Lock( FCS );
    UFC::AnsiString  SubscriberName;

    SubscriberName.Printf( "%s.%s", Subject.c_str(), Key.c_str() );
    if( FSubscribeTable.IsExists( SubscriberName ) )
    {
        MSubscriber* DelSubscriber = FSubscribeTable.GetObjectByKey( SubscriberName );
        FApp->DeleteSubscriber( DelSubscriber );
        FSubscribeTable.DeleteByKey( SubscriberName );
        delete DelSubscriber;

        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " DelListener:[%s][%s]",
                Subject.c_str(), Key.c_str() );
    }
}
//---------------------------------------------------------------------------
void MarketDataConnection::RemoveAllListener( void )
{
    UFC::PLockObject Lock( FCS );
    MSubscriber*     DelSubscriber = FSubscribeTable.First();

    while( DelSubscriber != NULL )
    {
        FApp->DeleteSubscriber( DelSubscriber );
        delete DelSubscriber;
        DelSubscriber = FSubscribeTable.Next();
    }
    FSubscribeTable.Clear();
}
//---------------------------------------------------------------------------
void MarketDataConnection::UnsubscribeAll( void )
{
    if( FLimitSubscribe == TRUE)
    {
        TExRegInfo* RegInfo = FSubscribeExchange.First();
        while( RegInfo != NULL)
        {
                RegInfo->Clear();
                RegInfo = FSubscribeExchange.Next();
        }
    }
    RemoveAllListener();
}
//---------------------------------------------------------------------------
//
//  Call By OCX Subscribe
//
//---------------------------------------------------------------------------
bool MarketDataConnection::Subscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, SubscribeType Type )
{
    bool Okay = false;
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Subscribe:[%s:%s], Type:[%d]",	Exchange.c_str(), Symbol.c_str(), Type );
    switch( Type )
    {
        case stSnapshot:            RecoverSnapshot( Exchange, Symbol, FALSE );
                                    Okay = true;
                                    break;
        case stSubscribe:           if( RegSymbol( Exchange, Symbol ) == true )
                                    {
                                        AddListener( Exchange, Symbol );
                                        Okay = true;
                                    }
                                    break;
        case stSnapshotWithUpdate : if( RegSymbol( Exchange, Symbol ) == true )
                                    {
                                        RecoverSnapshot( Exchange, Symbol, TRUE );
                                        Okay = true;
                                    }
                                    break;
        default: break;
    }
    UFC::BufferedLog::FlushToFile();
    return Okay;
}
//---------------------------------------------------------------------------
//
//  Call By OCX SubscribeX
//
//---------------------------------------------------------------------------
void MarketDataConnection::SubscribeX( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, const UFC::AnsiString& SettlementMonth,
									   CallPutCode cpCode, double StrikePx, SubscribeType Type )
{
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " SubscribeX:[%s:%s][%s][%d][%f], Type:[%d]",
            Exchange.c_str(), ProductID.c_str(), SettlementMonth.c_str(), cpCode, StrikePx, Type );
    switch( Type )
    {
        case stSnapshot:            RecoverDynamicSnapshot( Exchange, ProductID, SettlementMonth, cpCode, StrikePx, FALSE );
                                                                break;
        case stSubscribe:           RecoverDynamicSnapshot( Exchange, ProductID, SettlementMonth, cpCode, StrikePx, TRUE, FALSE );
                                                                break;
        case stSnapshotWithUpdate : RecoverDynamicSnapshot( Exchange, ProductID, SettlementMonth, cpCode, StrikePx, TRUE );
                                                                break;
        default: break;
    }
    UFC::BufferedLog::FlushToFile();
}
//---------------------------------------------------------------------------
//
//  Call By OCX Recover
//  Recover all contracts by Exchange
//
//---------------------------------------------------------------------------
bool MarketDataConnection::Recover( const UFC::AnsiString& Exchange )
{
    if( CanRegExchange( Exchange ) == true )
    {
        MTree Data;

        Data.append( "Exchange", Exchange );
        Send( RECOVER2, FRecoverKey, &Data );
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void MarketDataConnection::RecoverDynamicSnapshot( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, const UFC::AnsiString& SettlementMonth,
												   CallPutCode cpCode, double StrikePx, BOOL NeedAddListener, BOOL NeedSnapshot )
{
    MTree Data;

    Data.append( "Exchange", Exchange );
    Data.append( "ProductID", ProductID );
    Data.append( "SettlementMonth", SettlementMonth );
    Data.append( "CallPut", cpCode );
    Data.append( "StrikePx", StrikePx );
    Data.append( "NAL", NeedAddListener );
    Data.append( "NSS", NeedSnapshot );
    Send( RECOVER_DYNAMIC_SUBJECT, FRecoverKey, &Data );
}
//---------------------------------------------------------------------------
void MarketDataConnection::Recover( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID )
{
    MTree Data;
    
    Data.append( "Exchange", Exchange );
    Data.append( "ProductID", ProductID );
    Send( RECOVER2, FRecoverKey, &Data );
}
//---------------------------------------------------------------------------
void MarketDataConnection::RecoverSubscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& AppIdentification )
{
    MTree Data;
    BOOL NeedAddListener = TRUE;
    BOOL UseBatchRegister = TRUE;
    
    Data.append( "Exchange", Exchange );
    Data.append( "AppID", AppIdentification );
    Data.append( "IP", GetLocalIPAddress() );
    Data.append( "NAL", NeedAddListener );
    Data.append( "UBR", UseBatchRegister );
    Send( RECOVER2, FRecoverKey, &Data );
}
//---------------------------------------------------------------------------
void MarketDataConnection::RecoverSubscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& ProductID, const UFC::AnsiString& AppIdentification )
{
    MTree Data;
    BOOL NeedAddListener = TRUE;
    BOOL UseBatchRegister = TRUE;
    
    Data.append( "Exchange", Exchange );
    Data.append( "ProductID", ProductID );
    Data.append( "AppID", AppIdentification );
    Data.append( "IP", GetLocalIPAddress() );
    Data.append( "NAL", NeedAddListener );
    Data.append( "UBR", UseBatchRegister );
    Send( RECOVER2, FRecoverKey, &Data );
}
//---------------------------------------------------------------------------
void MarketDataConnection::Unsubscribe( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
    UnregSymbol( Exchange, Symbol );
    DelListener( Exchange, Symbol );
}
//---------------------------------------------------------------------------
bool MarketDataConnection::SubscribeExchangeNews( const UFC::AnsiString& Exchange )
{
    Subscribe( Exchange, "NEWS.STATUS", stSnapshot );
    AddListener( Exchange, "NEWS" );
    return TRUE;
}
//---------------------------------------------------------------------------
void MarketDataConnection::UnsubscribeExchangeNews( const UFC::AnsiString& Exchange )
{
    DelListener( Exchange, "NEWS" );
}
//---------------------------------------------------------------------------
bool MarketDataConnection::SubscribeNews( const UFC::AnsiString& Group )
{
    AddListener( "NEWS", Group );
    return TRUE;
}
//---------------------------------------------------------------------------
void MarketDataConnection::UnsubscribeNews( const UFC::AnsiString& Group )
{
    DelListener( "NEWS", Group );
}
//---------------------------------------------------------------------------
void MarketDataConnection::SendNews( const UFC::AnsiString& Group, const UFC::AnsiString& Message )
{
    MTree Data;

    Data.append( "MSG", Message );
    Send( "NEWS", Group, &Data );
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnMAppConnected( void )
{
    if( FListener != NULL )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " OnConnected()" );
        UFC::SleepMS( 330 );
        FListener->OnConnected();
    }
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnMAppDisconnected( void )
{
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " OnDisconnected()" );
    if( FListener != NULL )
        FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnMAppError( PMAppError err )
{
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " OnError() Code[%d]",(int)err );
    if( FListener != NULL )
        FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
void MarketDataConnection::FetchHeader( UFC::PStream* Stream, BaseMessage& Msg )
{
    UFC::Int8 Version;
    UFC::NInt32 Time, Seq;
    UFC::AnsiString TimeStamp;

    ///<Header
    Time.LoadFromStream( Stream );
    Seq.LoadFromStream( Stream );
    Stream->Read( (void*)&Version, 1 );
    DigitToTimeStamp( Time.ToInt32(), TimeStamp );
    Msg.SetMessageTime( TimeStamp );
    Msg.SetSequence( Seq.ToInt32() );
    Msg.SetVersion( Version );
}
//---------------------------------------------------------------------------
void MarketDataConnection::FillCloseMarketData( UFC::PStream* Stream, ClosingMarketData& Msg, UFC::Int32 DecimalLocator )
{
    UFC::NInt32 TermHighPx, TermLowPx, DayHighPx, DayLowPx, OpenPx, BuyPx, SellPx, ClosePx;
    UFC::NInt32 BuyTotalCount, BuyTotalQty, SellTotalCount, SellTotalQty, TotalTradeCount, TotalTradeQty;
    UFC::NInt32 CombineBuyTotalCount, CombineBuyTotalQty, CombineSellTotalCount, CombineSellTotalQty, CombineTotalQty;

    TermHighPx.LoadFromStream( Stream );
    TermLowPx.LoadFromStream( Stream );
    DayHighPx.LoadFromStream( Stream );
    DayLowPx.LoadFromStream( Stream );
    OpenPx.LoadFromStream( Stream );
    BuyPx.LoadFromStream( Stream );
    SellPx.LoadFromStream( Stream );
    ClosePx.LoadFromStream( Stream );
    BuyTotalCount.LoadFromStream( Stream );
    BuyTotalQty.LoadFromStream( Stream );
    SellTotalCount.LoadFromStream( Stream );
    SellTotalQty.LoadFromStream( Stream );
    TotalTradeCount.LoadFromStream( Stream );
    TotalTradeQty.LoadFromStream( Stream );
    CombineBuyTotalCount.LoadFromStream( Stream );
    CombineBuyTotalQty.LoadFromStream( Stream );
    CombineSellTotalCount.LoadFromStream( Stream );
    CombineSellTotalQty.LoadFromStream( Stream );
    CombineTotalQty.LoadFromStream( Stream );

    Msg.SetTermHighPx( UFC::IntToDouble( TermHighPx.ToInt32() , DecimalLocator ) );
    Msg.SetTermLowPx( UFC::IntToDouble( TermLowPx.ToInt32(), DecimalLocator ) );
    Msg.SetDayHighPx( UFC::IntToDouble( DayHighPx.ToInt32(), DecimalLocator ) );
    Msg.SetDayLowPx( UFC::IntToDouble( DayLowPx.ToInt32(), DecimalLocator ) );
    Msg.SetOpenPx( UFC::IntToDouble( OpenPx.ToInt32(), DecimalLocator ) );
    Msg.SetBuyPx(  UFC::IntToDouble( BuyPx.ToInt32(), DecimalLocator )  );
    Msg.SetSellPx(  UFC::IntToDouble( SellPx.ToInt32(), DecimalLocator )  );
    Msg.SetClosePx(  UFC::IntToDouble( ClosePx.ToInt32(), DecimalLocator )  );
    Msg.SetTotalTradeCount( TotalTradeCount.ToInt32() );
    Msg.SetTotalTradeQty( TotalTradeQty.ToInt32() );
    Msg.SetCombineBuyTotalCount( CombineBuyTotalCount.ToInt32() );
    Msg.SetCombineBuyTotalQty( CombineBuyTotalQty.ToInt32() );
    Msg.SetCombineSellTotalCount( CombineSellTotalCount.ToInt32() );
    Msg.SetCombineSellTotalQty( CombineSellTotalQty.ToInt32() );
    Msg.SetCombineTotalQty( CombineTotalQty.ToInt32() );
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnSelfEvent( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{
    Int32 UseMS, Count;

    Data->get( "COUNT", Count );
    Data->get( "UseMS", UseMS );
    if( FListener != NULL )
        FListener->OnContractDownloadComplete( Count, UseMS );
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " OnContractDownloadComplete() Key[%s]",Key.c_str() );
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{
    MDEventFunc* Callback;

    if( FListener != NULL )
    {
        if( (Callback = FCallbackFuncs.GetObjectByKey( Subject )) != NULL )
            (this->**Callback)( Subject, Key, Data );
        FAcuRecvBytes += Data->getSize();
        FAcuRecvMsgs ++;
    }
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnRecoverMsg( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    UFC::AnsiString Exchange, Symbol;
    Int32 NeedAddListener;
    int   CFlag, Size, Count;
    char* Buffer;
    Int32 Type;
    Int32 mkt;
    Int32 ErrorCode;
    Int32 TSession = tsNormal;
    UFC::AnsiString asErrorMsg;

    if( Data->get( "Exchange", Exchange ) &&
        Data->get( "NAL", NeedAddListener ) &&
        Data->get( "MsgType", Type ) )
    {
        if( Type == mtRecover ) ///< Recover OK
        {
            if( Data->get( "CFlag", CFlag )	&& CFlag == 1
                    && Data->get( "DATA", Buffer, Size )
                    && Data->get( "Count", Count ) ) // Recover by Market
            {
                UFC::MemoryReadStream Stream( Buffer, Size );

                Data->get( "Sim",    TSession );
                for( register int i = 0; i < Count; i++ )
                {
                    MarketDataMessage Msg( &Stream );

                    Msg.SetTradingSession( (TradingSession)TSession );
                    Data->get( "Symbol", Symbol );
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnRecoverMsg(), Exchange:[%s], CFlag=1", Symbol.c_str(), Exchange.c_str() );
                    if( FListener != NULL )
                        FListener->OnMarketDataMessage( Exchange, Msg.GetSymbol(), &Msg );
                    AddListener( Exchange, Msg.GetSymbol() );
                }
            }
            else if( Data->get( "Symbol", Symbol ) &&  Data->get( "DATA", Buffer, Size ) )
            {
                UFC::MemoryReadStream Stream( Buffer, Size );
                MarketDataMessage     Msg( &Stream );
                Int32                 NeedSnapshot = TRUE;
                UFC::AnsiString		  ActualSymbol;

                Data->get( "Sim",    TSession );
                Msg.SetTradingSession( (TradingSession)TSession );
                Data->get( "NSS", NeedSnapshot );
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnRecoverMsg(), Exchange:[%s], NeedSnapshot:[%d], NeedAddListener:[%d]",
                        Symbol.c_str(), Exchange.c_str(), NeedSnapshot, NeedAddListener );
                if (Data->get( "ActualSymbol", ActualSymbol ) == TRUE ) //vicky
                    Msg.SetActualSymbol(ActualSymbol);
                if( FListener != NULL && NeedSnapshot )
                    FListener->OnMarketDataMessage( Exchange, Symbol, &Msg );
                if( NeedAddListener == TRUE ) ///< Snapshut with continue update
                {
                    Int32 UseBatchRegister = FALSE;

                    if( Data->get( "UBR", UseBatchRegister ) && UseBatchRegister == TRUE )
                    {
                        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " [%s] OnRecoverMsg(), Exchange:[%s], UseBatchRegister:[%d], BeginAddSubscriber()",
                                Symbol.c_str(), Exchange.c_str(), UseBatchRegister );
                        FApp->BeginAddSubscriber();
                    }
                    if ( ActualSymbol.Length() > 0 )
                        AddListener( Exchange, ActualSymbol );
                    else
                        AddListener( Exchange, Symbol );
                }
            }
        }
        else if( Type == mtErrorMsg ) ///< Recover failed
        {
            if( Data->get( "MKT", mkt ) &&
                Data->get( "ErrorCode", ErrorCode ) &&
                Data->get( "ErrorMsg", asErrorMsg ) )
            {
                UFC::AnsiString ProductID, SettlementMonth;
                UFC::Int32      IntCallPutCode;
                double          dStrikePx;

                ErrorMessage ErrorMsg( Exchange, (Market)mkt );
                if( Data->get( "ProductID", ProductID ) )
                       ErrorMsg.SetProductID( ProductID );
                if( Data->get( "SettlementMonth", SettlementMonth ) )
                       ErrorMsg.SetSettleMonth( SettlementMonth );
                if( Data->get( "CallPut", IntCallPutCode ) )
                    ErrorMsg.SetCallPut( (CallPutCode)IntCallPutCode );
                if( Data->get( "StrikePx", dStrikePx ) )
                    ErrorMsg.SetStrikePrice( dStrikePx );
                ErrorMsg.SetErrorCode( ErrorCode );
                ErrorMsg.SetErrorMsg( asErrorMsg );
                if( FListener != NULL )
                    FListener->OnErrorMessage( Exchange, &ErrorMsg );
            }
        }
        else if ( Type == mtNews)
        {
            UFC::AnsiString Status;
            if (Data->get("Status", Status) == TRUE )
                FListener->OnNews(Exchange, Status);	
        }
    }
}
//---------------------------------------------------------------------------
void MarketDataConnection::DownloadContract( void )
{
    MTree Data;
    Send( DOWNLOAD_SUBJECT, FRecoverKey, &Data );
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnContractDownload( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data )
{
    UFC::AnsiString FilePath, Exchange;
    char* Buffer;

    if( Data->get( "DATA", Buffer, FDataSize ) && FDataSize > 0 )
    {
        FData = new char[ FDataSize ];
        memcpy( FData, Buffer, FDataSize );
        Start();
        ///< Save file.
        UFC::CreateDir( "Temp" );
        UFC::FileStreamEx File( "./Temp/Contract.dat", "w" );
        File.Write( Buffer, FDataSize );
    }
}
//---------------------------------------------------------------------------
void MarketDataConnection::ReloadExchangeContract( const UFC::AnsiString& Exchange, UFC::PStringList& IDList )
{
    MTree Data;

    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " ReloadExchangeContract, Exchange:[%s] Count:[%d]", Exchange.c_str(), IDList.ItemCount() );
    Data.append( "Exchange", Exchange );
    if( IDList.ItemCount() > 0 )
    {
        Data.append( "Count", IDList.ItemCount() );

        for( register int i = 0; i < IDList.ItemCount(); i++ )
        {
            UFC::AnsiString Name;
            Name.Printf( "ID%d", i + 1 );
            Data.append( Name, IDList[i] );
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Add [%s]", IDList[i].c_str() );
        }
    }
    Send( "SW.RELOAD.REQ", FRecoverKey, &Data );
}
//---------------------------------------------------------------------------
void MarketDataConnection::Execute( void )
{
    try
    {
        UInt32 BeginTime = UFC::GetTickCountMS( );
        UFC::MemoryReadStream MStream( FData, FDataSize );
        UFC::PHashedList<UFC::AnsiString, BasicInformation*>* SymbolMap;
        UFC::AnsiString       Buffer;
        MTree Data;

        while( MStream.ReadLine( Buffer ) )
        {
                BasicInformation* Msg = new BasicInformation( Buffer );
                UFC::PStringList* IndexList = NULL;
                UFC::AnsiString   Exchange = Msg->GetExchange();

                if( Msg->GetMarket() ==	mTWFutures  ||	Msg->GetMarket() == mTWOptions || 
                        Msg->GetMarket() == mPATSFutures || Msg->GetMarket() ==	mPATSOptions )
                {
                    if( Msg->GetMaturityDate().ToInt() < 200012 )
                            continue;
                }

                if( SupportExchange( Exchange ) == true )
                {
                    if(	(SymbolMap = FTable.GetObjectByKey( Exchange )) == NULL )
                    {
                        SymbolMap = new UFC::PHashedList<UFC::AnsiString, BasicInformation*>();
                        FTable.Add( Exchange,  SymbolMap );
                        FExchangeTable.Add( Exchange );
                    }

                    if( ( IndexList = FIndexTable.GetObjectByKey( Msg->GetExchange() ) ) == NULL )
                        FIndexTable.Add( Msg->GetExchange(), IndexList = new UFC::PStringList() );
                    IndexList->Add( Msg->GetSymbol() );

                    if( Msg->GetTradeFlag() == false  )
                {
                        UFC::PStringList& SpotList = Msg->GetSpotList();
                        UFC::AnsiString   Key;

                        for( register int i = 0; i < SpotList.ItemCount(); i++ )
                        {
                            Key.Printf( "%s.%s", Msg->GetExchange().c_str(), SpotList[i].c_str() ) ;
                            FSpotTable.Add( Key, new UFC::AnsiString( Msg->GetSymbol() ) );
                        }
                    }
                    if( SymbolMap->IsExists( Msg->GetSymbol() ) == FALSE )
                        SymbolMap->Add( Msg->GetSymbol(), Msg );
                    else
                        delete Msg;
                }
        }
        SetUnderlying();

        Data.append( "COUNT", (Int32)FExchangeTable.ItemCount() );
        Data.append( "UseMS", (Int32)(UFC::GetTickCountMS( )- BeginTime) );
        FApp->SendSelf( "Self", "Event", Data );
    }
    catch(...)
    {
            UFC::BufferedLog::Printf( "Parse contract file fail." );
    }
    delete [] FData;
}
//---------------------------------------------------------------------------
void MarketDataConnection::SetUnderlying( void )
{
    UFC::PHashedList<UFC::AnsiString, BasicInformation*>* Table = FTable.First(  );

    while( Table != NULL )
    {
        UFC::AnsiString   Key;
        UFC::AnsiString*  underlying;
        BasicInformation* Info = Table->First();

        while( Info != NULL )
        {
            if( Info->GetUnderlying().IsEmpty() )
            {
                if( Info->GetTradeFlag() == true )
                {
                    Key.Printf( "%s.%s", Info->GetExchange().c_str(), Info->GetProductID().c_str() );
                    if( (underlying = FSpotTable.GetObjectByKey( Key )) != NULL )
                        Info->SetUnderlying( *underlying );
                    else
                        Info->SetUnderlying( Info->GetProductID() );
                }
                else
                    Info->SetUnderlying( Info->GetProductID() );
            }
            else // add by joe
            {
                UFC::AnsiString asUnderlying = Info->GetUnderlying();
                BasicInformation *pUnderlyingInfo = GetBasicInformation( Info->GetExchange(), asUnderlying );
                if( pUnderlyingInfo != NULL )
                {
                    int iPreNCloseCount = pUnderlyingInfo->GetPreNCloseCount();
                    Info->SetPreNCloseCount( iPreNCloseCount );
                    for( int i = 0; i < iPreNCloseCount; i++ )
                            Info->SetPreNClosePx( i, pUnderlyingInfo->GetPreNClosePx( i ) );
                    Info->SetPreNChangeRange();
                }
            }
            Info = Table->Next();
        }
        Table = FTable.Next( );
    }
}
//---------------------------------------------------------------------------
Int32 MarketDataConnection::ExchangeCount( void )
{
	return FExchangeTable.ItemCount();
}
//---------------------------------------------------------------------------
Int32 MarketDataConnection::SymbolCount( const UFC::AnsiString& Exchange )
{
    UFC::PHashedList<UFC::AnsiString, BasicInformation*>* SymbolMap = FTable.GetObjectByKey( Exchange );
    if( SymbolMap != NULL )
        return SymbolMap->ItemCount();
    
    return 0;
}
//---------------------------------------------------------------------------
UFC::AnsiString MarketDataConnection::GetExchange( Int32 Index )
{
    if( Index < FExchangeTable.ItemCount() )
        return FExchangeTable[Index];
    
    return "" ;
}
//---------------------------------------------------------------------------
UFC::AnsiString MarketDataConnection::GetSymbol( const UFC::AnsiString& Exchange, Int32 Index )
{
    UFC::PHashedList<UFC::AnsiString, BasicInformation*>* SymbolMap = FTable.GetObjectByKey( Exchange );
    if( SymbolMap != NULL && Index < SymbolMap->ItemCount() )
    {
        UFC::AnsiString Symbol;
        BasicInformation* Msg = SymbolMap->GetItem( Index );

        return Msg->GetSymbol();
    }
    return "";
}
//---------------------------------------------------------------------------
BasicInformation* MarketDataConnection::GetBasicInformation( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
    UFC::PHashedList<UFC::AnsiString, BasicInformation*>* SymbolMap = FTable.GetObjectByKey( Exchange );
    if( SymbolMap != NULL  )
    {
        return SymbolMap->GetObjectByKey( Symbol );
    }
    return NULL;
}
//---------------------------------------------------------------------------
BasicInformation* MarketDataConnection::GetBasicInformation( const UFC::AnsiString& Exchange, UFC::Int32 Index )
{
    UFC::PStringList* IndexList = FIndexTable.GetObjectByKey( Exchange );
    UFC::PHashedList<UFC::AnsiString, BasicInformation*>* SymbolMap = FTable.GetObjectByKey( Exchange );

    if( IndexList != NULL && SymbolMap != NULL )
    {
        if( Index < IndexList->ItemCount() )
            return SymbolMap->GetObjectByKey( IndexList->GetItem( Index ) );
    }
    return NULL;
}
//---------------------------------------------------------------------------
UFC::PHashedList<UFC::AnsiString, BasicInformation*>* MarketDataConnection::GetSymbolsByExchange( const UFC::AnsiString& Exchange )
{
    return FTable.GetObjectByKey( Exchange );
}
//---------------------------------------------------------------------------
bool MarketDataConnection::IsConnected( void )
{
    return FApp->IsConnected();
}
//---------------------------------------------------------------------------
void MarketDataConnection::DigitToTimeStamp( UFC::Int32 Time, UFC::AnsiString& TimeStamp )
{
//	if( Time > 23595999 ) //< HHMMSSmmm(9 digi)
//	{
	int hour   = Time / 10000000;
	int Minute = ( Time % 10000000 ) / 100000;
	int Second = ( Time % 100000 ) / 1000;
	int ms     = Time % 1000;
	TimeStamp.Printf( "%02d:%02d:%02d.%03d", hour, Minute, Second, ms );	
//	}
//	else  ///< HHMMSSmm(8 digi)
//	{
//		int hour   = Time / 1000000;
//		int Minute = ( Time % 1000000 ) / 10000;
//		int Second = ( Time % 10000 ) / 100;
//		int ms     = Time % 100 * 10;
//		TimeStamp.Printf( "%02d:%02d:%02d.%03d", hour, Minute, Second, ms );
//	}
}
//---------------------------------------------------------------------------
void MarketDataConnection::IntToTimeStamp( UFC::Int32 Time, UFC::Int32 MS, UFC::AnsiString& TimeStamp )
{
    int hour = Time / 10000;
    int Minute = ( Time % 10000 ) / 100;
    int Second = ( Time % 100 );
    TimeStamp.Printf( "%02d:%02d:%02d.%03d", hour, Minute, Second, MS );
}
//---------------------------------------------------------------------------
void MarketDataConnection::ToTimeStamp( const UFC::UDateTime& DateTime, UFC::AnsiString& TimeStamp )
{
    TimeStamp.Printf( "%02d:%02d:%02d.%03d", DateTime.getHour(), DateTime.getMinute(), DateTime.getSecond(), DateTime.getMillisecond() );
}
//---------------------------------------------------------------------------
void MarketDataConnection::SendMessageToServer( const UFC::AnsiString& Msg, int Func )
{
    MTree Data;

    Data.append( "MSG", Msg );
    if( Func != 0 )
        Data.append( "Func", Func );
    FTickCount = UFC::GetTickCountMS();
    Send( "SW.APIMSG", FRecoverKey, &Data );
}
//---------------------------------------------------------------------------
void MarketDataConnection::SendTickCountToServer( void )
{
    MTree Data;
    Data.append( "TUS0", (Int64)UFC::GetTickCountUS() );
    Send( "SW.APIMSG", FRecoverKey, &Data );
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnNews( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data )
{
    UFC::AnsiString Msg;

    Data->get( "MSG", Msg );
    if( FListener != NULL )
        FListener->OnNews( Key, Msg );
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnServerAck( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    int RTT = UFC::GetTickCountMS() - FTickCount;
    if( FListener != NULL )
        FListener->OnServerAck( RTT );

    int Func;
    UFC::AnsiString RankingStr;
    if(  Data->get( "Func", Func ) && Func == 1 && Data->get( "TOP", RankingStr ) )
    {
        UFC::PStringList Board;

        Board.SetStrings( RankingStr, "|\n" );
        if( FListener != NULL && Board.ItemCount() >0 )
            FListener->OnLeaderboard( Board );
    }

    Int64 TUS0, TUS1, TimeDiffUS;
    if( Data->get( "TUS0", TUS0 ) && Data->get( "TUS1", TUS1 ) )
    {
        UInt64 TUS2 = UFC::GetTickCountUS();
        Int64 RTTUS = TUS2 - TUS0;
        TimeDiffUS = TUS2 - TUS1 - RTTUS/2;
        if( FListener != NULL )
            FListener->OnServerTimeDiffUS( TimeDiffUS, RTTUS );
    }
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnReloadMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
	UFC::AnsiString Exchange, ReloadID;
	UFC::PStringList IDList;
	int Count = 0;

	Data->get( "Exchange", Exchange );
    Data->get( "ID", ReloadID );    
    
    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " On Reolad Message Exchange:[%s] ID:[%s]", Exchange.c_str(), ReloadID.c_str() );
    
    if( Data->get( "Count", Count ) )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Count:[%d]", Count );
        for( register int i = 0; i < Count; i++ )
        {
            UFC::AnsiString Name, Value;
            Name.Printf( "ID%d", i + 1 );
            Data->get( Name, Value );
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Value:[%s]", Value.c_str() );
            IDList.Add( Value );
            if( FListener )
				FListener->OnReload( Exchange, Value );
        }
    }
    
    if( FListener )
        FListener->OnReload( Exchange, IDList, ReloadID );
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnReloadComplete( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    int Count, Size;
    char *Buffer;
    UFC::AnsiString Exchange;
    UFC::PStringList SymbolList;

    if( Data->get( "Count", Count ) && Data->get( "Data", Buffer, Size ) && Data->get( "Exchange", Exchange ) )
    {
        UFC::PHashedList<UFC::AnsiString, BasicInformation*>* SymbolMap;
        UFC::MemoryReadStream Stream( Buffer, Size );
        BasicInformation *Info;
        UFC::PStringList* IndexList;

        SymbolMap = FTable.GetObjectByKey( Exchange );
        IndexList = FIndexTable.GetObjectByKey( Exchange );

        if( SymbolMap && IndexList )
        {
            UFC::BufferedLog::Printf(  " Reload Count:[%d]", Count );
            for( register int i = 0; i < Count; i++ )
            {
                UFC::AnsiString InfoStr;

                InfoStr.LoadFromStream( &Stream );

                BasicInformation Info( InfoStr );

                BasicInformation* OrigInfo = SymbolMap->GetObjectByKey( Info.GetSymbol() );

                if( OrigInfo ) // update basic information
                {
                    OrigInfo->LoadFromString( InfoStr );
                }
                else // new commodity
                {
                    SymbolMap->Add( Info.GetSymbol(), new BasicInformation( InfoStr ) );
                    IndexList->Add( Info.GetSymbol() );
                }
                SymbolList.Add( Info.GetSymbol() );

            }
            if( FListener )
                FListener->OnReloadComplete( Exchange, SymbolList );
        }
        else
        {
            if( FListener )
                FListener->OnReloadComplete( Exchange, SymbolList );
        }
    }
}
//---------------------------------------------------------------------------
void MarketDataConnection::SubscribeSystemMessage( const UFC::AnsiString& Exchange )
{
    AddListener( Exchange, "SYS" );
}
//---------------------------------------------------------------------------
void MarketDataConnection::UnsubscribeSystemMessage( const UFC::AnsiString& Exchange )
{
    DelListener( Exchange, "SYS" );
}
//---------------------------------------------------------------------------
void MarketDataConnection::AddSupportExchange( const UFC::AnsiString& Exchange )
{
    FSupportExchange.Add( Exchange );
}
//---------------------------------------------------------------------------
void MarketDataConnection::ClearSupportExchange( void )
{
    FSupportExchange.Clear( );
}
//---------------------------------------------------------------------------
bool MarketDataConnection::SupportExchange( const UFC::AnsiString& Exchange )
{
    if( FSupportExchange.ItemCount() == 0  )
        return true;
    return FSupportExchange.Exists( Exchange );
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnRecoverFinished( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data )
{
    int Count;
    UFC::AnsiString Exchange, ProductID;
    BOOL bGetProductID = FALSE;
    Int32 UseBatchRegister = FALSE;

    Data->get( "Exchange", Exchange );
    bGetProductID = Data->get( "ProductID", ProductID );
    Data->get( "Count", Count );

    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " OnRecoverFinished, Exchange:[%s] Count:[%d]", Exchange.c_str(), Count );

    if( Data->get( "UBR", UseBatchRegister ) && UseBatchRegister == TRUE )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " OnRecoverFinished, Exchange:[%s], UseBatchRegister:[%d], EndAddSubscriber()",
                Exchange.c_str(), UseBatchRegister );
        FApp->EndAddSubscriber();
    }

    if( FListener )
    {
        if( bGetProductID == FALSE )
            FListener->OnRecoverFinished( Exchange, Count );
        else
            FListener->OnRecoverFinished( Exchange, ProductID, Count );
    }
}
//---------------------------------------------------------------------------
void MarketDataConnection::ClearRegInfo( void )
{
    if( FLimitSubscribe == TRUE)
    {
        TExRegInfo* RegInfo = FSubscribeExchange.First();
        while( RegInfo != NULL)
        {
            delete RegInfo;
            RegInfo = FSubscribeExchange.Next();
        }
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " ClearRegInfo, clear previous subscribed Exchange ItemCount:[%d]", FSubscribeExchange.ItemCount() );
        FSubscribeExchange.Clear();
    }
}
//---------------------------------------------------------------------------
bool MarketDataConnection::RegSymbol( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
    TExRegInfo* RegInfo;

    if( FLimitSubscribe == false )
            return true;
    if( FSubscribeExchange.GetObjectByKey( Exchange, RegInfo ) == TRUE)
            return RegInfo->AddReg( Symbol );
    return false;
}
//---------------------------------------------------------------------------
void MarketDataConnection::UnregSymbol( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
    TExRegInfo* RegInfo;

    if( FLimitSubscribe == TRUE)
    {
        if( FSubscribeExchange.GetObjectByKey( Exchange, RegInfo ) == TRUE)
            RegInfo->DelReg( Symbol );
    }
}
//---------------------------------------------------------------------------
bool MarketDataConnection::CanRegExchange( const UFC::AnsiString& Exchange )
{
	TExRegInfo* RegInfo;

	if( FLimitSubscribe == false )
            return true;
	if( FSubscribeExchange.GetObjectByKey( Exchange, RegInfo ) == TRUE)
            return RegInfo->RegAll();
    return false;
}
//---------------------------------------------------------------------------
void MarketDataConnection::OnLoginReply( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,MTree* Data )
{
    UFC::AnsiString  LogonErrMsg;
    int              Result = 0;
    int              LimitReg = 0;
    bool             LogonOK = false;

    printf( " Recv OnLoginReply[%s:%s]\n", Subject.c_str(), Key.c_str() );
    if( Data->get( "Result", Result ) == TRUE  )
    {
       if( Data->get( "Limit", LimitReg ) == TRUE )
               FLimitSubscribe = LimitReg;
       ClearRegInfo();
       Data->get( "Message", LogonErrMsg );
       if( Result == TRUE )
       {
            if( FLimitSubscribe == TRUE)
            {
                UFC::AnsiString ExName,SubName,CountName;
                UFC::AnsiString Exchange;
                int             ExCount = 0;
                int             CanSubAll,RegMax;

                Data->get( "ExCount", ExCount );
                printf( " - ExCount[%d]\n", ExCount );
                for( int i = 0; i < ExCount; i++ )
                {
                    ExName.Printf( "Exchange%d", i+1);
                    SubName.Printf( "Subscribe%d", i+1);
                    CountName.Printf( "Count%d", i+1);
                    if(	Data->get( ExName, Exchange ) == TRUE &&
                        Data->get( SubName, CanSubAll ) == TRUE &&
                        Data->get( CountName, RegMax ) == TRUE)
                    {
                        FSubscribeExchange.Add( Exchange, new TExRegInfo(  Exchange, RegMax, CanSubAll ) );
                        printf( " -- %s[%s]\n", ExName.c_str(), Exchange.c_str() );
                        printf( "   |- %s[%d]\n", SubName.c_str(), CanSubAll );
                        printf( "   |- %s[%d]\n", CountName.c_str(), RegMax );
                    }
                }
            }
            if ( LogonErrMsg.Length() == 0 )
                         LogonErrMsg = "Logon OK!";
            LogonOK = true;
       }
       //else
       //	Data->get( "Message", LogonErrMsg );
    }
    else
        LogonErrMsg = "Missing value Result or Limit.";
    if( FListener != NULL )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " OnLogonReply()" );
        printf( "OnLoginReply[%d:%s]\n", LogonOK, LogonErrMsg.c_str() );
        FListener->OnLogonReply( LogonOK, LogonErrMsg );
    }
    if( LogonOK == false )
        Disconnect();
}
//---------------------------------------------------------------------------
void MarketDataConnection::SetIP( const UFC::AnsiString& IP )
{
    if( FApp != NULL )
        FApp->SetHost( IP );
}
//---------------------------------------------------------------------------
void MarketDataConnection::SetPort( UFC::Int32 port )
{
    if( FApp != NULL )
        FApp->SetPort( port );
}
//---------------------------------------------------------------------------
const UFC::AnsiString& MarketDataConnection::GetIP( void )
{
    if( FApp != NULL )
        return FApp->GetHost( );
    return gLocalhostIP;
}
//---------------------------------------------------------------------------
UFC::Int32 MarketDataConnection::GetPort( void )
{
    if( FApp != NULL )
        return FApp->GetPort( );
    else
        return DEFAULT_PORT;
}
//---------------------------------------------------------------------------

