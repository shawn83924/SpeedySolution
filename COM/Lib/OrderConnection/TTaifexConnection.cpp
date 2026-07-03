#include "TTaifexConnection.h"
#include "TLicenseKey.h"
#include "RecoverThread.h"
//------------------------------------------------------------------------------
#ifdef WIN32
#include "../CA/UniFSCCAObject.h"
#include "../CA/MLTWCAObject.h"
#endif
//------------------------------------------------------------------------------
BOOL              UseRes;
UFC::BufferedLog* GStdout = NULL;
UFC::BufferedLog* Glog = NULL;
//------------------------------------------------------------------------------
const int THROUGHPUT_DEF = 3000;
//------------------------------------------------------------------------------
UFC::PSet<int>    TTaifexConnection::FAPISet;
bool              TTaifexConnection::FDetectAPI = false;
UFC::AnsiString   TTaifexConnection::FTSEOTCBrokerID( "0000" );
UFC::AnsiString   TTaifexConnection::FTAIFEXBrokerID( "F000000" );
//------------------------------------------------------------------------------
// Recover subjects
//------------------------------------------------------------------------------
const UFC::AnsiString SUBJECT_RECOVER           = "RECOVER";
const UFC::AnsiString SUBJECT_ADMIN             = "OG.ADMIN";
//------------------------------------------------------------------------------
// Add a new Exchange Ex: MDBS
// 1. Define order subject SUBJECT_ORDER_MDBS
// 2. Define execution subject SUBJECT_CONFIRM_MDBS,SUBJECT_FILL_MDBS.
// 3. define callback function pointer FOnMDBSConfirm,FOnMDBSFill.
// 4. Modify NewOrder,CancelOrder,ReplaceOrder,SendToOrderServerEx...
// 5. Implement ReceiveMDBSConfirmMessage,ReceiveMDBSExecuteMessage
// 6. Add execution listener, use function AddExecListener
//------------------------------------------------------------------------------
// Place order subjects
//------------------------------------------------------------------------------
const UFC::AnsiString SUBJECT_ORDER_FUT         = "TO.TFX.FUT";
const UFC::AnsiString SUBJECT_ORDER_OPT         = "TO.TFX.OPT";
const UFC::AnsiString SUBJECT_ORDER_TSE         = "TO.TSE";
const UFC::AnsiString SUBJECT_ORDER_OTC         = "TO.OTC";
const UFC::AnsiString SUBJECT_ORDER_ES          = "TO.ES";
const UFC::AnsiString SUBJECT_ORDER_FOREIGN_FUT = "TO.PATS";
const UFC::AnsiString SUBJECT_ORDER_FOREIGN_OPT = "TO.PATS.OPT";
const UFC::AnsiString SUBJECT_ORDER_FOREIGN_STK = "TO.PATS.STK";
//------------------------------------------------------------------------------
//  TAIFEX ask for file transfer subjects
//------------------------------------------------------------------------------
const UFC::AnsiString SUBJECT_FT_FUT            = "TO.FT.FUT";
const UFC::AnsiString SUBJECT_FT_OPT            = "TO.FT.OPT";
//------------------------------------------------------------------------------
// Order confirm subjects
//------------------------------------------------------------------------------
const UFC::AnsiString SUBJECT_CONFIRM_FUT        = "CONFIRM.FUT";
const UFC::AnsiString SUBJECT_CONFIRM_OPT        = "CONFIRM.OPT";
const UFC::AnsiString SUBJECT_CONFIRM_TSE        = "CONFIRM.TSE";
const UFC::AnsiString SUBJECT_CONFIRM_OTC        = "CONFIRM.OTC";
const UFC::AnsiString SUBJECT_CONFIRM_ES         = "CONFIRM.ES";
const UFC::AnsiString SUBJECT_CONFIRM_FOREIGN    = "CONFIRM.PATS";
const UFC::AnsiString SUBJECT_TOUCH_RESPONSE     = "TOUCH.RESPONSE";// added bu Kenny to support Touch Order. 2026/03/12
//------------------------------------------------------------------------------
// Order filled subjects
//------------------------------------------------------------------------------
const UFC::AnsiString SUBJECT_FILL_FUT          = "FILL.FUT";
const UFC::AnsiString SUBJECT_FILL_OPT          = "FILL.OPT";
const UFC::AnsiString SUBJECT_FILL_TSE          = "FILL.TSE";
const UFC::AnsiString SUBJECT_FILL_OTC          = "FILL.OTC";
const UFC::AnsiString SUBJECT_FILL_ES           = "FILL.ES";
const UFC::AnsiString SUBJECT_FILL_FOREIGN      = "FILL.PATS";
//------------------------------------------------------------------------------
const UFC::AnsiString SUBJECT_FILE_DOWNLOAD     = "FILE.DOWNLOAD";
const UFC::AnsiString SUBJECT_NEWS_REQUEST      = "NEWS.REQUEST";
const UFC::AnsiString SUBJECT_NEWS_RESPONSE     = "NEWS.RESPONSE";
//------------------------------------------------------------------------------
bool  IsUseByProxy                              = false; //Chelsea 20260702
//------------------------------------------------------------------------------
void TTaifexConnection::InitGlobal()
{
    if( GStdout == NULL )        
        GStdout = new UFC::BufferedLog();
	if( Glog == NULL )
        Glog = GStdout;    
}
//------------------------------------------------------------------------------
void TTaifexConnection::DetectAPI( bool Enable )
{
    FDetectAPI = Enable;
    FAPISet.Clear();
    FAPISet.Add( atSpeedy );
}
//------------------------------------------------------------------------------
bool TTaifexConnection::SupportAPI( APIType APITp )
{
    return FAPISet.Exists( APITp );
}
//------------------------------------------------------------------------------
// Constructor for COM only.
//------------------------------------------------------------------------------
TTaifexConnection::TTaifexConnection( HINSTANCE AppInstance,
                                      HMODULE ResourceInstance,
                                      const char* AppName,
                                      IOrderConnectionEventListener* pOrderConnectionEventListener,
                                      APIType WhichAPI,
                                      BOOL  IsWin32GUIApp )
:FOutCount( 0 )
,FOrderPerSec( THROUGHPUT_DEF )
,FFirstOrderTick(0)
,FInstance( AppInstance )
,FResourceInstance( ResourceInstance )
,FUseAPI( WhichAPI )
,FID( "" )
,FToken( "")
,FLocalIP( "127.0.0.1" )
,FAdmin( 0 )
,FIsDMA( 0 )
,FCurrentConnectionID( 0 )
,FIsConnected( false )
,FIsLogon( false)
,FUseNID64( FALSE )
,FTradeingDate( "" )
,FListener( pOrderConnectionEventListener )
,FSendEventListener( NULL )
,FAppName( AppName )
,FUniquekey( "" )
,FCMID( "F999" )
,FIsTWSE8DigiVersion( FALSE ) ///< Default 6 digi fill sequence
,FNewFutSymbol(  FALSE  )
,FEnablePendingNewAck( TRUE )
,FTriggerExec( TRUE )
,FCanSendOrder( TRUE )
,FFillZeroToCancel( TRUE )
,FEnableFilledState( FALSE )
,FX25StyleQtyPx( TRUE )
,FAlwaysGenNID( FALSE )
,FReportType( rdBoth )
,FOnAdminMsg( &TTaifexConnection::ReceiveAdminMessage )
,FOnNewsMsg( &TTaifexConnection::ReceiveNews )
,FOnRecoverMsg( &TTaifexConnection::ReceiveRecoverExecution )
,FOnOptConfirm( &TTaifexConnection::ReceiveOptConfirmMessage )
,FOnFutConfirm( &TTaifexConnection::ReceiveFutConfirmMessage )
,FOnTSEConfirm( &TTaifexConnection::ReceiveTSEConfirmMessage )
,FOnOTCConfirm( &TTaifexConnection::ReceiveOTCConfirmMessage )
,FOnESConfirm( &TTaifexConnection::ReceiveESConfirmMessage )
,FOnForeignConfirm( &TTaifexConnection::ReceiveForeignConfirmMessage )
,FOnOptFill( &TTaifexConnection::ReceiveOptExecuteMessage )
,FOnFutFill( &TTaifexConnection::ReceiveFutExecuteMessage )
,FOnTSEFill( &TTaifexConnection::ReceiveTSEExecuteMessage )
,FOnOTCFill( &TTaifexConnection::ReceiveOTCExecuteMessage )
,FOnESFill( &TTaifexConnection::ReceiveESExecuteMessage )
,FOnForeignFill( &TTaifexConnection::ReceiveForeignExecuteMessage )
,FOnTouchOrderResponse(&TTaifexConnection::ReceiveTouchOrderResponse) // added by Kenny to support Touch Order. 2026/03/16
,FTransport( NULL )
,FAdminListener( NULL )
,FTWSERender( &TTaifexConnection::RenderTWSET010Ex )
,FExchangeError( NULL )
,FFUTSymbol( NULL )
,FOPTSymbol( NULL )
,FIsProxy( false )
,FSupportFLEX( false )
,FCAPFXFilePathName( "" )
,FCAPassword( "" )
,FCADLLFileName( "" )
,FCAOrganizationalUnit( "" )
,FCACommonName( "" )
,FApiCAObjPtr( 0 )
,FNeedCheckOrdQty0( true )
{
    InitGlobal();
#ifdef _USE_RES
    UseRes = TRUE; ///< Use the message format file in resource.
#else
    UseRes = FALSE; ///< Use the message format file in file.
#endif
    if( UFC::BufferedLogData::FLogObject != NULL && Glog == GStdout )
        Glog = (UFC::BufferedLog*) UFC::BufferedLogData::FLogObject ;
    FLogonEvent   = new UFC::PEvent();
    FRequestEvent = new UFC::PEvent();
    FLastReqUID   = 0;
    FResponseUID  = -1;
    if( FUseAPI == atSpeedy )
    {
        FTransport = TTransport::CreateTransport( FInstance, this, FAppName,  IsWin32GUIApp );
        FSupportSellSide.Add( ssTAIFEX );
        FSupportSellSide.Add( ssTSEOTC );
        SetLanguage( mlEnglish );
        FCallbackFuncs.Add( SUBJECT_ADMIN,        &FOnAdminMsg   ); ///< Admin message
        FCallbackFuncs.Add( SUBJECT_NEWS_RESPONSE,&FOnNewsMsg    ); ///< News
        FCallbackFuncs.Add( SUBJECT_RECOVER,      &FOnRecoverMsg ); ///< Recover executions
        UFC::SleepMS( 5 );
    }
}
//------------------------------------------------------------------------------
// Constructor for using C++ library.
//------------------------------------------------------------------------------
TTaifexConnection::TTaifexConnection( const char* AppName,
                                      IOrderConnectionEventListener* pOrderConnectionEventListener,
                                      APIType WhichAPI,
                                      BOOL IsWin32GUIApp )
:FOutCount( 0 )
,FOrderPerSec( THROUGHPUT_DEF )
,FFirstOrderTick(0)
,FInstance( NULL )
,FResourceInstance( NULL)
,FUseAPI( WhichAPI )
,FID( "" )
,FToken( "")
,FAdmin( 0 )
,FIsDMA( 0 )
,FCurrentConnectionID( 0 )
,FIsConnected( false )
,FIsLogon( false)
,FUseNID64( FALSE )
,FTradeingDate( "" )
,FListener( pOrderConnectionEventListener )
,FSendEventListener( NULL )
,FAppName( AppName )
,FUniquekey( "" )
,FCMID( "F999" )
,FIsTWSE8DigiVersion( FALSE )
,FNewFutSymbol( FALSE )
,FEnablePendingNewAck( TRUE )
,FTriggerExec( TRUE )
,FCanSendOrder( TRUE )
,FFillZeroToCancel( TRUE )
,FEnableFilledState( FALSE )
,FX25StyleQtyPx( TRUE )
,FAlwaysGenNID( FALSE )
,FReportType( rdBoth )
,FOnAdminMsg( &TTaifexConnection::ReceiveAdminMessage )
,FOnNewsMsg( &TTaifexConnection::ReceiveNews )
,FOnRecoverMsg( &TTaifexConnection::ReceiveRecoverExecution )
,FOnOptConfirm( &TTaifexConnection::ReceiveOptConfirmMessage )
,FOnFutConfirm( &TTaifexConnection::ReceiveFutConfirmMessage )
,FOnTSEConfirm( &TTaifexConnection::ReceiveTSEConfirmMessage )
,FOnOTCConfirm( &TTaifexConnection::ReceiveOTCConfirmMessage )
,FOnESConfirm( &TTaifexConnection::ReceiveESConfirmMessage )
,FOnForeignConfirm( &TTaifexConnection::ReceiveForeignConfirmMessage )
,FOnOptFill( &TTaifexConnection::ReceiveOptExecuteMessage )
,FOnFutFill( &TTaifexConnection::ReceiveFutExecuteMessage )
,FOnTSEFill( &TTaifexConnection::ReceiveTSEExecuteMessage )
,FOnOTCFill( &TTaifexConnection::ReceiveOTCExecuteMessage )
,FOnESFill( &TTaifexConnection::ReceiveESExecuteMessage )
,FOnForeignFill( &TTaifexConnection::ReceiveForeignExecuteMessage )
,FOnTouchOrderResponse(&TTaifexConnection::ReceiveTouchOrderResponse) // added by Kenny to support Touch Order. 2026/03/16
,FTransport( NULL )
,FAdminListener( NULL )
,FTWSERender( &TTaifexConnection::RenderTWSET010Ex )
,FExchangeError( NULL )
,FFUTSymbol( NULL )
,FOPTSymbol( NULL )
,FIsProxy( false )
,FSupportFLEX( false )
,FCAPFXFilePathName( "" )
,FCAPassword( "" )
,FCADLLFileName( "" )
,FCAOrganizationalUnit( "" )
,FCACommonName( "" )
,FApiCAObjPtr( 0 )
,FNeedCheckOrdQty0( true )
{    
        InitGlobal();
#ifdef   _USE_RES
	UseRes = TRUE; ///< Use the message format file in resource.
#else
	UseRes = FALSE; ///< Use the message format file in file.
#endif
    if( UFC::BufferedLogData::FLogObject != NULL && Glog == GStdout )
        Glog = (UFC::BufferedLog*) UFC::BufferedLogData::FLogObject ;
    FLogonEvent   = new UFC::PEvent();
    FRequestEvent = new UFC::PEvent();
    FLastReqUID   = 0;
    FResponseUID  = -1;
    if( FUseAPI == atSpeedy )
    {
        FTransport = TTransport::CreateTransport( FInstance, this, FAppName,  IsWin32GUIApp  );
        FSupportSellSide.Add( ssTAIFEX );
        FSupportSellSide.Add( ssTSEOTC );
        SetLanguage( mlEnglish );
        FCallbackFuncs.Add( SUBJECT_ADMIN,        &FOnAdminMsg   ); ///< Admin message
        FCallbackFuncs.Add( SUBJECT_NEWS_RESPONSE,&FOnNewsMsg    ); ///< News
        FCallbackFuncs.Add( SUBJECT_RECOVER,      &FOnRecoverMsg ); ///< Recover executions
        UFC::SleepMS( 5 );
    }
}
//------------------------------------------------------------------------------
TTaifexConnection::~TTaifexConnection( void )
{
    if( FUseAPI == atSpeedy )
    {
        Disconnect();
        if( FTransport != NULL )
            delete FTransport;
    }
    delete FLogonEvent;
    delete FRequestEvent;
//#ifdef __UNICA_WIN
#ifdef WIN32
//	if ( FCAObjPtr != 0 ) delete FCAObjPtr;
#endif
    UFC::BufferedLog::FlushToFile();
}
//------------------------------------------------------------------------------
void TTaifexConnection::SetTWSE8DigiSeqVersion( bool Is8Digi )
{
    FIsTWSE8DigiVersion = Is8Digi;
}
//------------------------------------------------------------------------------
void TTaifexConnection::SetLogFile( const char* FileName )
{
    try
    {
        UFC::BufferedLog* OldLogObj = Glog;
        
        if ( FileName == NULL || (FileName != NULL && strcmp(FileName, "") == 0))
        {
            Glog = GStdout ;
            Glog->SetToStdout(FALSE);
        }
        else
        {
            Glog = new UFC::BufferedLog( FileName, 10240, TRUE );
        }
        if( OldLogObj != GStdout && OldLogObj != UFC::BufferedLogData::FLogObject)
            delete OldLogObj; ///< Close old file
        else
            OldLogObj->FlushToFile();
        Glog->fprintf( " Build Date[%s]", __DATE__ );
    }
    catch( UFC::FileException&  )
    {
        Glog->fprintf( " Open log file [%s] failed.", FileName );
    }
}
//------------------------------------------------------------------------------
void TTaifexConnection::SetLogFileEx( const char* FileName )
{
    try
    {
        UFC::BufferedLog* OldLogObj = Glog;

        if ( FileName == NULL || (FileName != NULL && strcmp( FileName, "" ) == 0))
        {
            Glog = GStdout ;
            Glog->SetToStdout(FALSE);
        }
        else
        {
            Glog = new UFC::BufferedLog( FileName, 10240, TRUE );
            UFC::BufferedLog::SetLogObject( Glog );
        }
        if( OldLogObj != GStdout && OldLogObj != UFC::BufferedLogData::FLogObject)
            delete OldLogObj; ///< Close old file
        else
            OldLogObj->FlushToFile();
        Glog->fprintf( " Build Date[%s]", __DATE__ );
    }
    catch( UFC::FileException&  )
    {
        Glog->fprintf( " Open log file [%s] failed.", FileName );
    }
}
//------------------------------------------------------------------------------
BOOL TTaifexConnection::CheckDecimalLocatorSetting( UFC::UiniFile* ini, const UFC::AnsiString& Sect )
{
    UFC::Section* DigiSect = ini->GetSection( Sect );
    int Count = 0;
            
    if( DigiSect != NULL )
    {
        UFC::AnsiString Name,Value;
        
        Glog->fprintf( " ------ Decimal Locators for[%s] -----", Sect.c_str() );
        Count = DigiSect->ItemCount();
        for( int i = 0; i < Count; i ++ )
        {
            DigiSect->GetNameValue( i, Name, Value );
            Glog->fprintf( " Symbol[%s] Decimal Locator[%s].", Name.c_str(), Value.c_str() );
        }
        Glog->fprintf( " ----------------------------------------------------" );        
    }    
    if( ini->SectionExists( "Info" ) == true )
    {
        UFC::AnsiString Value;
        
        if( ini->GetValue( "Info", Sect, Value ) == TRUE )
        {
            int InfoCount = Value.ToInt();
            if( Count != InfoCount )
            {
                Glog->fprintf( " In [Info] %s=%d, but in [%s] has %d product ID.", Sect.c_str(),InfoCount, Sect.c_str(), Count );
                return FALSE;
            }
        }
        else
        {
            if( Count != 0 )
            {
                Glog->fprintf( " In [Info] %s=0, but in [%s] has %d product ID.", Sect.c_str(), Sect.c_str(), Count );
                return FALSE;
            }
        }            
        if( ini->GetValue( "Info", "Update", Value ) == TRUE )
            Glog->fprintf( " Pass check! Last Update:%s", Value.c_str() );
    }
    else    
        Glog->fprintf( " Old version[%s], skip product count check", Sect.c_str() );
    return TRUE;///<Pass Check    
}
//------------------------------------------------------------------------------
BOOL TTaifexConnection::LoadSymbol( MTree* Data )
{
    UFC::AnsiString TempFileName;
    BOOL  Result;
    BOOL  ReturnVal = TRUE;

    UFC::CreateDir( "Temp" );
    if( FSupportSellSide.Exists( ssTAIFEX ) == TRUE )
    {
        FSupportFLEX = false;
        Result = CreateTempFileFromMNode( Data, "FUT", TempFileName ) ;
        if( Result == TRUE )
        {
            Glog->fprintf( " Load Futures decimal locator[%s] OK.", TempFileName.c_str() );
            if( FFUTSymbol != NULL )
                delete FFUTSymbol;
            FFUTSymbol = new UFC::UiniFile( TempFileName, TRUE );
            UFC::RemoveFile( TempFileName.c_str() );
            if( FFUTSymbol->SectionExists( "FLEX") == true )
                FSupportFLEX  = true;
            ReturnVal = CheckDecimalLocatorSetting( FFUTSymbol,"FuturesProducts" );
        }
        else
        {
            Glog->fprintf( " Load Futures decimal locator[%s] failed.", TempFileName.c_str() );
            ReturnVal = FALSE;
        }

        Result = CreateTempFileFromMNode( Data, "OPT", TempFileName ) ;
        if( Result == TRUE )
        {
            Glog->fprintf( " Load Options decimal locator[%s] OK.", TempFileName.c_str() );
            if( FOPTSymbol != NULL )
                delete FOPTSymbol;
            FOPTSymbol = new UFC::UiniFile( TempFileName, TRUE  );
            UFC::RemoveFile( TempFileName.c_str() );
            if( FOPTSymbol->SectionExists( "FLEX") == true )
                FSupportFLEX  = true;
            ReturnVal = CheckDecimalLocatorSetting( FOPTSymbol,"OptionProducts" );
        }
        else
        {
            Glog->fprintf( " Load Options decimal locator[%s] failed.", TempFileName.c_str() );
            ReturnVal = FALSE;
        }
        if( FSupportFLEX == true )
            Glog->fprintf( " Server Support TAIFEX FLEX Protocol." );
        else
            Glog->fprintf( " Server Not Support TAIFEX FLEX Protocol." );
    }
    return ReturnVal;
}
//------------------------------------------------------------------------------
void TTaifexConnection::SetLanguage( MessageLanguage Lenguage )
{
    UFC::AnsiString TempFileName,TempFileName2;

    FLanguage = Lenguage;
    UFC::CreateDir( "Temp" );
    if( FExchangeError != NULL )
        delete FExchangeError;
    if( Lenguage == mlEnglish )
        TempFileName  = CreateTempFileFromResource( IDR_ERROR_ENG, "INI" );
    else
        TempFileName  = CreateTempFileFromResource( IDR_ERROR_CHT, "INI" );
    if( TempFileName.Length() > 0 )
    {
        FExchangeError = new UFC::UiniFile( TempFileName );
        if( UseRes == TRUE )
            UFC::RemoveFile( TempFileName.c_str() );
    }
}
//------------------------------------------------------------------------------
void TTaifexConnection::SetTAIFEXDataFormatResourceID( DWORD R010, DWORD R020, DWORD C030, DWORD C030_MultiLeg, DWORD C030_CancelReplace, DWORD C030_QuoteCancelReplace, const char* ResourceType )
{
    if( FSupportSellSide.Exists( ssTAIFEX ) == TRUE )
    {
        LoadDataFormatFromResourceFile( R010, ResourceType, FR010Format );
        FR010Render.SetDataFormat( &FR010Format );
        LoadDataFormatFromResourceFile( R020, ResourceType, FR020Format );
        FR020Parser.SetDataFormat( &FR020Format );
        LoadDataFormatFromResourceFile( C030, ResourceType, FC030Format );
        FC030Parser.SetDataFormat( &FC030Format );
        LoadDataFormatFromResourceFile( C030_MultiLeg, ResourceType, FC030MFormat );
        FC030MParser.SetDataFormat( &FC030MFormat );
        LoadDataFormatFromResourceFile( C030_CancelReplace, ResourceType, FC030CRFormat );
        FC030CRParser.SetDataFormat( &FC030CRFormat );
        LoadDataFormatFromResourceFile( C030_QuoteCancelReplace, ResourceType, FC030QCRFormat );
        FC030QCRParser.SetDataFormat( &FC030QCRFormat );
    }
}
//------------------------------------------------------------------------------
void TTaifexConnection::SetTAIFEXQuoteDataFormatResourceID( DWORD R070, DWORD R080, DWORD R090, DWORD R100, const char* ResourceType )
{
    if( FSupportSellSide.Exists( ssTAIFEX ) == TRUE )
    {
        LoadDataFormatFromResourceFile( R070, ResourceType, FR070Format );
        FR070Render.SetDataFormat( &FR070Format );
        LoadDataFormatFromResourceFile( R080, ResourceType, FR080Format );
        FR080Parser.SetDataFormat( &FR080Format );
        LoadDataFormatFromResourceFile( R090, ResourceType, FR090Format );
        FR090Render.SetDataFormat( &FR090Format );
        LoadDataFormatFromResourceFile( R100, ResourceType, FR100Format );
        FR100Parser.SetDataFormat( &FR100Format );
    }
}
//------------------------------------------------------------------------------
void TTaifexConnection::SetTSEDataFormatResourceID( DWORD T010, DWORD O010, DWORD P010, DWORD T020, DWORD O020, DWORD P020, DWORD R030, const char* ResourceType )
{
    if( FSupportSellSide.Exists( ssTSEOTC ) == TRUE )
    {
        ///< New Order for Normal,Odd,Post trading session
        LoadDataFormatFromResourceFile( T010, ResourceType, FTSET010Format );
        FTSET010Render.SetDataFormat( &FTSET010Format );        
        LoadDataFormatFromResourceFile( O010 + 100, ResourceType, FTSEO010ExFormat );
        FTSEO010ExRender.SetDataFormat( &FTSEO010ExFormat );
        LoadDataFormatFromResourceFile( P010 + 100, ResourceType, FTSEP010ExFormat );
        FTSEP010ExRender.SetDataFormat( &FTSEP010ExFormat );
        ///< Confirm execution.
        LoadDataFormatFromResourceFile( T020 + 100, ResourceType, FTSET020ExFormat );        
        FTSET020ExParser.SetDataFormat( &FTSET020ExFormat );
        LoadDataFormatFromResourceFile( O020 + 100, ResourceType, FTSEO020ExFormat );
        FTSEO020ExParser.SetDataFormat( &FTSEO020ExFormat );
        LoadDataFormatFromResourceFile( P020 + 100, ResourceType, FTSEP020ExFormat );
        FTSEP020ExParser.SetDataFormat( &FTSEP020ExFormat );
        ///< Filled execution.
        LoadDataFormatFromResourceFile( R030, ResourceType, FTSER03Format ); ///< 8 Digi Sequence
        FTSER03Parser.SetDataFormat( &FTSER03Format );
        LoadDataFormatFromResourceFile( R030 + 100, ResourceType, FTSER03ExFormat );///< 6 Digi Sequence
        FTSER03ExParser.SetDataFormat( &FTSER03ExFormat );
    }
}
//------------------------------------------------------------------------------
void TTaifexConnection::SetTSEDataFormatResourceID2( DWORD A010, DWORD V010, DWORD E010,DWORD Ex010, DWORD A020, DWORD V020, DWORD E020, DWORD Ex020,const char* ResourceType )
{
    if( FSupportSellSide.Exists( ssTSEOTC ) == TRUE )
    {        
        int VerDiff = 100;
        ///< New Order
        LoadDataFormatFromResourceFile( A010 + VerDiff, ResourceType, FTSEA010ExFormat );
        FTSEA010ExRender.SetDataFormat( &FTSEA010ExFormat );
        LoadDataFormatFromResourceFile( V010 + VerDiff, ResourceType, FTSEV010ExFormat );
        FTSEV010ExRender.SetDataFormat( &FTSEV010ExFormat );
        LoadDataFormatFromResourceFile( E010 + VerDiff, ResourceType, FTSEE010ExFormat );
        FTSEE010ExRender.SetDataFormat( &FTSEE010ExFormat );
        LoadDataFormatFromResourceFile( Ex010 + VerDiff, ResourceType, FTSEEx010ExFormat );
        FTSEEx010ExRender.SetDataFormat( &FTSEEx010ExFormat );
        ///< Comfirm
        LoadDataFormatFromResourceFile( A020 + VerDiff, ResourceType, FTSEA020ExFormat );
        FTSEA020ExParser.SetDataFormat( &FTSEA020ExFormat );
        LoadDataFormatFromResourceFile( V020 + VerDiff, ResourceType, FTSEV020ExFormat );
        FTSEV020ExParser.SetDataFormat( &FTSEV020ExFormat );
        LoadDataFormatFromResourceFile( E020 + VerDiff, ResourceType, FTSEE020ExFormat );
        FTSEE020ExParser.SetDataFormat( &FTSEE020ExFormat );
        LoadDataFormatFromResourceFile( Ex020 + VerDiff, ResourceType, FTSEEx020ExFormat );
        FTSEEx020ExParser.SetDataFormat( &FTSEEx020ExFormat );
    }
}
//------------------------------------------------------------------------------
void TTaifexConnection::SetOTCDataFormatResourceID( DWORD T010, DWORD O010, DWORD P010, DWORD T020, DWORD O020, DWORD P020, DWORD R030, const char* ResourceType )
{
    if( FSupportSellSide.Exists( ssTSEOTC ) == TRUE )
    {       
        ///< New Order for Normal,Odd,Post trading session      
        LoadDataFormatFromResourceFile( T010, ResourceType, FOTCT010Format );
        FOTCT010Render.SetDataFormat( &FOTCT010Format );
        LoadDataFormatFromResourceFile( O010 + 100, ResourceType, FOTCO010ExFormat );
        FOTCO010ExRender.SetDataFormat( &FOTCO010ExFormat );
        LoadDataFormatFromResourceFile( P010 + 100, ResourceType, FOTCP010ExFormat );
        FOTCP010ExRender.SetDataFormat( &FOTCP010ExFormat );
        ///< Confirm execution.
        LoadDataFormatFromResourceFile( T020 + 100, ResourceType, FOTCT020ExFormat );
        FOTCT020ExParser.SetDataFormat( &FOTCT020ExFormat );
        LoadDataFormatFromResourceFile( O020 + 100, ResourceType, FOTCO020ExFormat );
        FOTCO020ExParser.SetDataFormat( &FOTCO020ExFormat );
        LoadDataFormatFromResourceFile( P020 + 100, ResourceType, FOTCP020ExFormat );
        FOTCP020ExParser.SetDataFormat( &FOTCP020ExFormat );
        ///< Filled execution.
        LoadDataFormatFromResourceFile( R030 , ResourceType, FOTCR03Format ); ///< 8 Digi Sequence
        FOTCR03Parser.SetDataFormat( &FOTCR03Format );
        LoadDataFormatFromResourceFile( R030 + 100, ResourceType, FOTCR03ExFormat ); ///< 6 Digi Sequence
        FOTCR03ExParser.SetDataFormat( &FOTCR03ExFormat );
    }
}
//------------------------------------------------------------------------------
void TTaifexConnection::SetOTCDataFormatResourceID2( DWORD V010, DWORD E010, DWORD Ex010, DWORD V020, DWORD E020, DWORD Ex020, const char* ResourceType )
{
    if( FSupportSellSide.Exists( ssTSEOTC ) == TRUE )
    {
        int VerDiff = 100;
        
        LoadDataFormatFromResourceFile( V010 + VerDiff, ResourceType, FOTCV010ExFormat );
        FOTCV010ExRender.SetDataFormat( &FOTCV010ExFormat );
        LoadDataFormatFromResourceFile( E010 + VerDiff, ResourceType, FOTCE010ExFormat );
        FOTCE010ExRender.SetDataFormat( &FOTCE010ExFormat );
        LoadDataFormatFromResourceFile( Ex010 + VerDiff, ResourceType, FOTCEx010ExFormat );
        FOTCEx010ExRender.SetDataFormat( &FOTCEx010ExFormat );
        LoadDataFormatFromResourceFile( V020 + VerDiff, ResourceType, FOTCV020ExFormat );
        FOTCV020ExParser.SetDataFormat( &FOTCV020ExFormat );
        LoadDataFormatFromResourceFile( E020 + VerDiff, ResourceType, FOTCE020ExFormat );
        FOTCE020ExParser.SetDataFormat( &FOTCE020ExFormat );
        LoadDataFormatFromResourceFile( Ex020 + VerDiff, ResourceType, FOTCEx020ExFormat );
        FOTCEx020ExParser.SetDataFormat( &FOTCEx020ExFormat );
    }
}
//------------------------------------------------------------------------------
bool TTaifexConnection::LoadDataFormatFromResourceFile( DWORD ResourceID, const char* ResourceType, UFC::TFixedFormat& DataFormat )
{
    UFC::CreateDir( "Temp" );
    UFC::AnsiString TempFileName = CreateTempFileFromResource( ResourceID, ResourceType );
    if( TempFileName.Length() > 0   )
    {
        if( UFC::FileExists( TempFileName ) == TRUE )
        {
            DataFormat.SetIniFile( TempFileName.c_str() );
            if( UseRes == TRUE )
                UFC::RemoveFile( TempFileName.c_str() );
            return true;
        }
        else
        {
            Glog->fprintf( " Format file[%s] not found!", TempFileName.c_str() );
            UFC::BufferedLog::Printf( " ________________________________________________" );
            UFC::BufferedLog::Printf( "  Format file[%s] not found! ", TempFileName.c_str() );
            UFC::BufferedLog::Printf( " ________________________________________________" );
            exit( 0 );
        }
    }
    return false;
}
//---------------------------------------------------------------------------
 BOOL TTaifexConnection::CreateTempFileFromMNode( MTree* Data, const char* NodeName, UFC::AnsiString& FileName )
{
    char* Buffer;
    int   Size;
    
    UFC::CreateDir( "Temp" );	
    FileName.Printf( "./Temp/%s.%010u.ini", NodeName, UFC::GetTickCountMS() );    
    if( Data->get( NodeName, Buffer, Size ) && Size > 0 ) 
    {
        try 
        {
            UFC::FileStreamEx TempFile( FileName.c_str(), "w");
    	    TempFile.Write( Buffer, Size );
            TempFile.Flush();
            return TRUE;
    	}
        catch(UFC::FileException& fe)
        {
            Glog->fprintf( " Create TempFile[%s] From MNode[%s] failed, %s.", FileName.c_str(), NodeName, fe.what() );
        }
    }
    return FALSE;
}
//---------------------------------------------------------------------------
UFC::AnsiString TTaifexConnection::CreateTempFileFromResource( DWORD ResourceID, const char* ResourceType )
{
    UFC::AnsiString ResourceName, FileName, ResourceFileName;

    try
    {
        if( GetResourceName( ResourceID, ResourceFileName ) == TRUE )
        {
            ResourceName.Printf( "#%d", ResourceID );
            FileName.Printf( "./Temp/%s", ResourceFileName.c_str() );
            if( UseRes == TRUE ) 
            {
                TFormatResource Resource( FResourceInstance, ResourceName.c_str(), ResourceType );
                 Resource.SaveToFile( FileName );
            }
            return FileName;
        }
        else
        {
            Glog->fprintf( " Resource:[%d] not exists.", ResourceID );
            return "";
        }        
    }
    catch( UFC::Exception& e )
    {
        Glog->fprintf( " Get Resource [%d]failed:%s", ResourceID, e.what() );
        return "";
    }    
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnMAppConnected( void )
{
    FIsConnected = true;
    if( FListener != NULL )
    {
        UFC::SleepMS( 330 );
        FListener->OnConnected();
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnMAppDisconnected( void )
{
    FIsConnected = false;
    FIsLogon = false;
    if( FListener != NULL )
            FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
void TTaifexConnection::OnMAppError( PMAppError )
{
    FIsConnected = false;
    if( FListener != NULL )
            FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
// Dispatch message by Subject.
//---------------------------------------------------------------------------
void TTaifexConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString&,  MTree* pTree )
{
    if( FListener != NULL )
    {
        EventFunc* Callback;

        if( FCallbackFuncs.GetObjectByKey( Subject, Callback ) == TRUE )
            (this->**Callback)( pTree );
        else
            Glog->fprintf( " Un-handled Subject[%s]", Subject.c_str() );
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::ReceiveNews( MTree* pTree )
{
    Int32            NewsID;
    Int32            MsgCount = 1;
    Int32            MsgIndex = 1;
    UFC::AnsiString  NewsData,Func;
    TNewsMessage     NewsMsg;

    if( pTree->get( "UID", NewsID ) )
    {
        NewsMsg.SetID( NewsID );
        if( pTree->get( "FUNC", Func ) )
            NewsMsg.SetHeadline( Func.c_str( ) );
        if( pTree->get( "DATA", NewsData ) )
            NewsMsg.SetText( NewsData.c_str( ) );
        if( pTree->get( "COUNT", MsgCount ) )
            NewsMsg.SetMsgCount( MsgCount );
        if( pTree->get( "INDEX", MsgIndex ) )
            NewsMsg.SetMsgNum( MsgIndex );
        if( Func == "ReqFUTMargin" )
        {
            if( FLastReqUID == NewsID )
            {
                FResponseData = NewsData;
                FResponseUID  = NewsID; ///< Must be written before SetEvent so the waiter can verify it.
                FRequestEvent->SetEvent();
            }
            return;
        }
        if( FListener != NULL )
            FListener->OnNews( &NewsMsg );
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::ReceiveRecoverExecution( MTree* pTree )
{
    Int32 Market,Type;
    
    if( pTree->get( "MARKET", Market ) && pTree->get( "TYPE", Type ) )
    {
        RecoverMarket RMkt = (RecoverMarket)Market;

        if( Type == 0 ) ///< Confirm
        {
            if( FReportType == rdFill )
                return; ///< Recover filled only, but receive confirm
            switch( RMkt )
            {
                case rmFutures:     	ReceiveFutConfirmMessage( pTree ); break;///< Order confirmed
                case rmOption:		ReceiveOptConfirmMessage( pTree ); break;///< Order confirmed
                case rmTSE: 		ReceiveTSEConfirmMessage( pTree ); break;///< Order confirmed
                case rmOTC:		ReceiveOTCConfirmMessage( pTree ); break;///< Order confirmed
                case rmForeignFutures:  ///< Foreign Futures/Options/Stock
                case rmForeignOptions:
                case rmForeignStock:    ReceiveForeignConfirmMessage( pTree );break;///< Order confirmed
                default:                break;
            }
        }
        else ///< Fill execution
        {
            if( FReportType == rdConfirm )
                return; ///< Recover confirm only, but receive filled
            switch( RMkt )
            {
                case rmFutures:		ReceiveFutExecuteMessage( pTree ); break;///< Execution Report
                case rmOption:		ReceiveOptExecuteMessage( pTree ); break;///< Execution Report
                case rmTSE:		ReceiveTSEExecuteMessage( pTree ); break;///< Execution Report
                case rmOTC:		ReceiveOTCExecuteMessage( pTree ); break;///< Execution Report
                case rmForeignFutures:  ///< Foreign Futures/Options/Stock
                case rmForeignOptions:
                case rmForeignStock:	ReceiveForeignExecuteMessage( pTree );break;///< Execution Report
                default:                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::Connect( const char* RemoteIP, long RemotePort, int TimeoutSec )
{
    Glog->fprintf( " Connect to[%s, %d]", RemoteIP, RemotePort );
    if( FUseAPI == atSpeedy )
    {
        FExecIDCS.Enter();
        FFUTExecIDSet.Clear();
        FOPTExecIDSet.Clear();
        FTSEExecIDSet.Clear();
        FOTCExecIDSet.Clear();
        FESExecIDSet.Clear();
        FPATSExecIDSet.Clear();
        FCNExecIDSet.Clear();
        FExecIDCS.Leave();
        FCurrentConnectionID = 0;
        FTransport->Connect( RemoteIP, RemotePort, TimeoutSec );
    }
}
//------------------------------------------------------------------------------
void SplitIPPort( const UFC::AnsiString& Str, UFC::AnsiString& IP, int& Port)
{
    UFC::PStringList Strs;

    Strs.SetStrings( Str, ":" );
    IP = Strs[0];
    if( Strs.ItemCount( ) > 1 )
        Port = Strs[1].ToInt();
}
//---------------------------------------------------------------------------
void TTaifexConnection::Disconnect( void )
{
    if( FUseAPI == atSpeedy  )
    {
        Glog->fprintf( " FTransport->Disconnect( )");
        Logoff();
        if( FTransport != NULL )
            FTransport->Disconnect();
    }
}
//---------------------------------------------------------------------------
///< SpeedyProxyLogon
///< ID:      Logon ID, Should be citizen ID or Ecxchange account.
///< PASSWD:  Password for logon.
///< ACCOUNT: Exchange account.( could be "Account1,Account2,Account3..." )
///< Token1 default: SpeedyProxy
///< Token2 default: 601008
///< ConnectionType: Connection type.(Place oder, Receive ececution...)
///< Version: API version string.( Default SPEEDY_API_PROXY_VERSION )
///< Encode: Compress the order string.
//---------------------------------------------------------------------------
void TTaifexConnection::SpeedyProxyLogon( const char* ID,
                                          const char* PASSWD,
                                          const char* ACCOUNT, ///< Account1,Account2,Account3.....
                                          const char* Token1,  ///< Default "SpeedyProxy"
                                          const char* Token2,  ///< Default "601008"
                                          ConnectionType Type, ///< ConnectionType Type = ctBoth
                                          int Version,         ///< Version = SPEEDY_API_PROXY_VERSION
                                          BOOL Encode )
{
    if( FUseAPI == atSpeedy )
    {
        ///< ID:      Logon ID, Should be citizen ID or Ecxchange account.
        ///< PASSWD:
        ///< ACCOUNT: Exchange account.( could be "Account1,Account2,Account3..." )
        ///< Token1 default: SpeedyProxy
        ///< Token2 default: 601008
        UFC::AnsiString id( ID ), passwd2( PASSWD ), TradeDate = "", TimeNow = "";
        int MAppFD = FTransport->GetMApp()->GetSocketHandle();

        FAccounts.SetStrings( ACCOUNT, ",\n" );
        FAccountSet.Clear();
        UFC::AnsiString firstAccountNo = "";
        UFC::PStringBuffer accountNoCSVBuffer;
        for( int i = 0; i < FAccounts.ItemCount(); i++ )  //ZhenFan 2026/01/22
        {
            UFC::AnsiString curAccountNo = FAccounts[ i ];
            if( i == 0 ) firstAccountNo = curAccountNo;
            if( FAccountSet.Exists( curAccountNo ) == FALSE )  //Check Duplicate AccountNo
            {
                FAccountSet.Add( curAccountNo );
                if( accountNoCSVBuffer.Length() > 0 ) accountNoCSVBuffer.Append( ',' );
                accountNoCSVBuffer.Append( curAccountNo );
            }
            else
                Glog->fprintf( " %s() Duplicate AccountNo[%s].", __func__, curAccountNo.c_str() );
        }
        UFC::AnsiString accountNoCSV = accountNoCSVBuffer.ToString();
        
        UFC::GetTradeYYYYMMDD( TradeDate , FALSE );
        if( id.Length() == 0 )
            id = "NULL";
        FIDs.Clear();
        FIDs.Add( id );
        UFC::GetTimeString_us( TimeNow );
        FUniquekey.Printf( "%s@%s.%s_%d", id.c_str(), UFC::Hostname, TimeNow.c_str(), MAppFD );
       
        ///< Account: Exchange account.
        ///< TradeDate: YYYYMMDD
        ///< Logon MD5 and order MD5.
        UFC::TLicenseKey OrderKey( Token1, firstAccountNo, TradeDate, Token2 );
        UFC::TLicenseKey LogonKey( Token1, firstAccountNo, FUniquekey, Token2 );

        Logoff();
        FToken    = OrderKey.ToString();
        FID       = id;//FAccounts[0];
        FUserName = id;
        switch( Type )
        {
            case ctSendOrder:       FReportType   = rdBoth;
                                    FTriggerExec  = FALSE;
                                    FCanSendOrder = TRUE;
                                    break;
            case ctRecvFillOnly:    FReportType   = rdFill;
                                    FTriggerExec  = TRUE;
                                    FCanSendOrder = FALSE;
                                    break;
            case ctRecvComfirmOnly: FReportType   = rdConfirm;
                                    FTriggerExec  = TRUE;
                                    FCanSendOrder = FALSE;
                                    break;
            case ctRecvExecution:   FReportType   = rdBoth;
                                    FTriggerExec  = TRUE;
                                    FCanSendOrder = FALSE;
                                    break;
            case ctBoth:            FReportType   = rdBoth;
                                    FTriggerExec  = TRUE;
                                    FCanSendOrder = TRUE;
                                    break;
        }
        FAdminListener = new TMdListener( FTransport, this, SUBJECT_ADMIN.c_str(), FUniquekey.c_str() );

        UFC::AnsiString signatureStr = "";
        UFC::AnsiString caErrorMessage = "";
        UFC::AnsiString caPlainText = "";

        Glog->fprintf( " %s() Check CA Logon Data.", __func__ );
        CAResultData caResult;
        caResult.SetPlainText( id );
        if ( !CheckCALogonData( id, caResult) )
        {
            caErrorMessage = caResult.GetResultMsg();
            Glog->fprintf( " %s() Check CA Error:%s.", __func__, caErrorMessage.c_str() );
            if( FListener != NULL )
                FListener->OnLogonReply( caErrorMessage, lrFailed, FCurrentConnectionID );
            if( FCurrentConnectionID <= 0 )
                new LogoffThread( this );
            return;
        }
        ///< Create a thread to send the logon message.
        FIsProxy = true;
        Glog->fprintf( " User[%s][%s] create logon thread.", id.c_str(), firstAccountNo.c_str() );
        Glog->Flush();
        Glog->FlushToFile();

        new LogonThread( FTransport, FLogonEvent, Version, SUBJECT_ADMIN, FUniquekey, id, LogonKey.ToString(), accountNoCSV, passwd2, caResult, Encode );  //ZhenFan 2026/01/22
    }
}
//---------------------------------------------------------------------------
///< SpeedyGatewayLogon
///< ID:      Logon ID, AE,Admin or channel ID.( could be "ID1,ID2,ID3..." )
///< PASSWD:  Password for logon.
///< ACCOUNT: Exchange account.
///< ConnectionType: Connection type.(Place oder, Receive ececution...)
///< Version: API version string.(Default SPEEDY_API_GATEWAY_VERSION)
///< Encode: Compress the order string.
//---------------------------------------------------------------------------
void TTaifexConnection::Logon( const char* ID,
                               const char* PASSWD,
                               const char* ACCOUNT,
                               ConnectionType Type,
                               int Version,
                               BOOL Encode )
{
    if( FUseAPI == atSpeedy  )
    {
        FIDs.SetStrings( ID, ",\n" );
        ///< ID:      Logon AE ID .
        ///< PASSWD:  password for ID
        ///< ACCOUNT: Exchange account.
        //UFC::AnsiString LocalIpAddress = FTransport->GetMApp()->GetLocalIPAddress();
        UFC::AnsiString id( FIDs[0] );
        UFC::AnsiString passwd( PASSWD );
        UFC::AnsiString Account( ACCOUNT );
        UFC::AnsiString TimeNow;
        int MAppFD = FTransport->GetMApp()->GetSocketHandle();

        int  APIVer = Version;

        if( APIVer > 999999 ) ///< Gateway forrmat  MM.MM.RR  Proxy MM.MM.RR.BB
            APIVer = Version/100;
        if( id.Length() == 0 )
            id = "NULL";
        if( passwd.Length() == 0 )
            passwd = "NULL";
        FAccounts.Clear();
        FAccounts.Add( Account );
        FAccountSet.Clear();
        FAccountSet.Add( Account );
        UFC::GetTimeString_us( TimeNow );
        FUniquekey.Printf( "%s@%s.%s_%d", id.c_str(), UFC::Hostname, TimeNow.c_str(),MAppFD );
        UFC::SleepMS( 1 );
        Logoff();
        FID = id;
        FUserName = id;
        switch( Type )
        {
            case ctSendOrder:       FReportType   = rdBoth;
                                    FTriggerExec  = FALSE;
                                    FCanSendOrder = TRUE;
                                    break;
            case ctRecvFillOnly:    FReportType   = rdFill;
                                    FTriggerExec  = TRUE;
                                    FCanSendOrder = FALSE;
                                    break;
            case ctRecvComfirmOnly: FReportType   = rdConfirm;
                                    FTriggerExec  = TRUE;
                                    FCanSendOrder = FALSE;
                                    break;
            case ctRecvExecution:   FReportType   = rdBoth;
                                    FTriggerExec  = TRUE;
                                    FCanSendOrder = FALSE;
                                    break;
            case ctBoth:            FReportType   = rdBoth;
                                    FTriggerExec  = TRUE;
                                    FCanSendOrder = TRUE;
                                    break;
        }
        FAdminListener = new TMdListener( FTransport, this, SUBJECT_ADMIN.c_str(), FUniquekey.c_str() );

        CAResultData caResult;
        if ( !CheckCALogonData( id, caResult) )
        {
            UFC::AnsiString caErrorMessage = caResult.GetResultMsg();
            Glog->fprintf( " Check CA Error:%s.", caErrorMessage.c_str() );
            if( FListener != NULL )
                FListener->OnLogonReply( caErrorMessage, lrFailed, FCurrentConnectionID );
            if( FCurrentConnectionID <= 0 )
                new LogoffThread( this );
            return;
        }
        ///< Create a thread to send the logon message.
        FIsProxy = false;
        Glog->fprintf( " User:[%s] create logon thread.", FID.c_str() );

        UFC::AnsiString passwd2 = "";
        new LogonThread( FTransport, FLogonEvent, APIVer, SUBJECT_ADMIN, FUniquekey, id, passwd, Account, passwd2, caResult, Encode );
    }
}
//---------------------------------------------------------------------------
LogonThread::LogonThread( TTransport*   Transport,
                          UFC::PEvent*  WaitEvent,
                          int   Version,
                          const UFC::AnsiString& Subject,
                          const UFC::AnsiString& Key,
                          const UFC::AnsiString& ID,
                          const UFC::AnsiString& Passwd,
                          const UFC::AnsiString& Account,
                          const UFC::AnsiString& Passwd2,
                          CAResultData& CAResult,
                          BOOL Encode )
:UFC::PThread( NULL )
,FTransport( Transport )
,FReplyEvent( WaitEvent )
,FSubject( Subject )
,FKey( Key )
,FID( ID )
,FPasswd( Passwd )
,FPasswd2( Passwd2 )
,FAccount( Account )
,FVersion( Version )
,FEncode( Encode )
,FApiCAObjPtr( 0 )
,FCAPlainText( "" )
,FCASignature( "" )
,FCASubject( "" )
,FCANotBefore( "" )
,FCANotAfter( "" )
,FCASerialNumber( "" )
{
    FApiCAObjPtr = CAResult.GetCAObj();
    FCAPlainText = CAResult.GetPlainText();
    FCASignature = CAResult.GetSignature();
    FCASubject = CAResult.GetSubject();
    FCANotBefore = CAResult.GetNotBefore();
    FCANotAfter = CAResult.GetNotAfter();
    FCASerialNumber = CAResult.GetSerialNumber();
    Start();
}
//---------------------------------------------------------------------------
void LogonThread::Execute( void )
{
    UFC::AnsiString LocalIpAddress = FTransport->GetMApp()->GetLocalIPAddress();
    TMdMessage      LogonMsg;

    while( FTransport->IsConnected() == FALSE )
           UFC::SleepMS( 30 ); ///< Wait for connected to server.
    UFC::SleepMS( 330 );
    Glog->fprintf( " User:[%s] send logon message.", FID.c_str() );
    FReplyEvent->ResetEvent();
    LogonMsg.SetIntegerValue( "CMD", CMD_LOGON_REQUEST );
    LogonMsg.SetIntegerValue( "Ver",FVersion );
    
#ifdef NO_DMA_PRIVILEDGE
    LogonMsg.SetStringValue( "DMA","0" );
#endif
    
    if( FEncode == FALSE )
    {
        LogonMsg.SetStringValue( "ID", FID.c_str() );
        LogonMsg.SetStringValue( "PASSWD", FPasswd.c_str() );
        if( FPasswd2.Length() > 0 )
            LogonMsg.SetStringValue( "PASSWD2", FPasswd2.c_str() );
        LogonMsg.SetStringValue( "ACCOUNT", FAccount.c_str() );
        LogonMsg.SetStringValue( "IP", LocalIpAddress.c_str() );   
    }
    else
    {
        LogonMsg.SetZIPStringValue( "ID", FID );
        LogonMsg.SetZIPStringValue( "PASSWD", FPasswd );
        if( FPasswd2.Length() > 0 )
            LogonMsg.SetZIPStringValue( "PASSWD2", FPasswd2 );
        LogonMsg.SetZIPStringValue( "ACCOUNT", FAccount );
        LogonMsg.SetZIPStringValue( "IP", LocalIpAddress.c_str() );   
    }       
    if( FApiCAObjPtr != 0 )
    {
	if( FCAPlainText.Length() > 0 )
            LogonMsg.SetStringValue( "CAPLAINTEXT", FCAPlainText.c_str() );
        else
            Glog->fprintf( " No CA PlainText.");        
        if( FCASignature.Length() > 0 )
            LogonMsg.SetStringValue( "CASIGNATURE", FCASignature.c_str() );
        else
            Glog->fprintf( " No CA Signature.");        
	if( FCASubject.Length() > 0 )
            LogonMsg.SetStringValue( "CASUBJECT", FCASubject.c_str() );
        else
            Glog->fprintf( " No CA Subject.");
	if( FCASerialNumber.Length() > 0 )
            LogonMsg.SetStringValue( "CASERIALNUMBER", FCASerialNumber.c_str() );
        else
            Glog->fprintf( " No CA Serial Number.");
	if( FCANotAfter.Length() > 0 )
            LogonMsg.SetStringValue( "CANOTAFTER", FCANotAfter.c_str() );
        else
            Glog->fprintf( " No CA Not After.");
    }
    FTransport->Send( FSubject, FKey, &LogonMsg );
    if( FReplyEvent->WaitFor( 10 ) == FALSE )
    {
        Glog->fprintf( " Wait for login reply timeout.(10sec)");
        FTransport->Disconnect();
    }
    else
        FReplyEvent->ResetEvent();
}
//---------------------------------------------------------------------------
LogoffThread::LogoffThread( TTaifexConnection* Connection )
:UFC::PThread( NULL ),
 FConnection( Connection )
{
    Start();
}
//---------------------------------------------------------------------------
void LogoffThread::Execute( void )
{
	UFC::SleepMS( 500 );
	FConnection->Disconnect();
}
//---------------------------------------------------------------------------
bool TTaifexConnection::CheckPassword( const UFC::AnsiString& Password, const UFC::AnsiString& NewPassword, ChangePwdResult& Result )
{
	bool UpperChar = false;
	bool LowerChar = false;
	bool HasNumber = false;
	bool HasSymbol = false;

	///< Login first
	if( FIsLogon == false )
	{
            Result = crNotLogin;
            return false;
	}
	///< Password length should >= 8
	if( NewPassword.Length() < 8 )
	{
            Result = crLengthNeeds8;
            return false;
	}
	///< The new password must be different from the old password
	if( Password == NewPassword )
	{
            Result = crSameAsOld;
            return false;
	}
	///< Must contain numbers, symbols, and English uppercase and lowercase letters
	for( int i=0; i < NewPassword.Length(); i++ )
	{
            char ch = NewPassword[i];

            if( ch >='0' && ch <= '9') HasNumber = true;
            if( ch >='a' && ch <= 'z') LowerChar = true;
            if( ch >='A' && ch <= 'Z') UpperChar = true;
            if((ch >='!' && ch <= '/') || (ch >=':' && ch <= '@') ||
               (ch >='[' && ch <= '`') || (ch >='{' && ch <= '~')) HasSymbol = true;
	}
	if( HasNumber == false || HasSymbol == false || UpperChar == false || LowerChar == false )
	{
            if( HasNumber == false )
                Result = crNeedsNumbers;
            else if( HasSymbol == false )
                Result = crNeedsSymbols;
            else if( UpperChar == false || LowerChar == false )
                Result = crUpperLowerCase;
            return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void TTaifexConnection::ChangePassword( const UFC::AnsiString& Password, const UFC::AnsiString& NewPassword )
{
	ChangePwdResult Result;
	TMdMessage      PwdMsg;

	if( CheckPassword( Password, NewPassword, Result ) == false )
	{
            if( FListener != NULL )
		FListener->OnChangePassword( Result );
            return;
	}
	Glog->fprintf( " User:[%s] Change password.", FID.c_str() );
	PwdMsg.SetIntegerValue( "CMD", CMD_CHANGE_PASSWORD );
	PwdMsg.SetStringValue(  "ID", FID.c_str() );
	PwdMsg.SetStringValue( "PASSWD", Password.c_str() );
	PwdMsg.SetStringValue( "NEWPASSWD", NewPassword.c_str() );
	FTransport->Send( SUBJECT_ADMIN, FUniquekey, &PwdMsg );
}
//---------------------------------------------------------------------------
void TTaifexConnection::AddRecoverListener( void )
{
    UFC::AnsiString curRecoverKey = "", idRecoverKey = "";
    bool IDisAccount = false;
//ZhenFan 2026/01/22 begin
    UFC::AnsiString curAccountNo = "";
    BOOL isAccountNoExist = FAccountSet.First( curAccountNo );
    while ( isAccountNoExist == TRUE )
    {
        curRecoverKey.Printf( "%s%d", curAccountNo.c_str(), FCurrentConnectionID );
        FRecoverListeners.Add( new TMdListener( FTransport, this, SUBJECT_RECOVER.c_str(), curRecoverKey.c_str()) );
        Glog->fprintf( " %s() Add Account[%s] Recover Listener [%s][%s].", __func__, curAccountNo.c_str(), SUBJECT_RECOVER.c_str(), curRecoverKey.c_str() );
        if( FID == curAccountNo )
        {
            IDisAccount = true;
            idRecoverKey = curRecoverKey;
        }
        isAccountNoExist = FAccountSet.Next( curAccountNo );
    }
//ZhenFan 2026/01/22 end

//    for( int i = 0; i < FAccounts.ItemCount(); i ++ )
//    {
//        UFC::AnsiString ExecAccount = FAccounts[ i ];

//        curRecoverKey.Printf( "%s%d", ExecAccount.c_str(), FCurrentConnectionID );
//        FRecoverListeners.Add( new TMdListener( FTransport, this, SUBJECT_RECOVER.c_str(), curRecoverKey.c_str()) );
//        if( FID == ExecAccount )
//            IDisAccount = true;
//    }
    if( IDisAccount == false )
    {
        idRecoverKey.Printf( "%s%d", FID.c_str(), FCurrentConnectionID );
        FRecoverListeners.Add( new TMdListener( FTransport, this, SUBJECT_RECOVER.c_str(), idRecoverKey.c_str() ));
    }
    Glog->fprintf( " %s logon,CID[%d] Admin[%s] RecoverKey:[%s]", FID.c_str(), FCurrentConnectionID, ( FAdmin == 0 )? "false" : "true", idRecoverKey.c_str() );
}
//---------------------------------------------------------------------------
LogonResult TTaifexConnection::CreateShareMemory( MTree* pTree, int CIDBits, UFC::AnsiString& ReplyString )
{
    try
    {        
        FNetworkID.Init( FCurrentConnectionID, CIDBits );            
        FIsLogon = true;
        if( LoadSymbol( pTree ) == FALSE )
        {
            ReplyString = ReplyString +"(Load decimal locator file failed.)";
            return lrDecimalError;        
        }
        return lrOk;        
    }
    catch( UFC::Exception& ex )
    {
        Glog->fprintf( "Exception[%s]", ex.what() );
        ReplyString.Printf( "User %s already login.", FID.c_str() );
        FCurrentConnectionID = 0;
        FIsLogon = false;
        return lrAlreadyLogon;
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::ReceiveAdminMessage( MTree* pTree )
{
    TMdMessage Msg;
    Msg.Attach( pTree );
    int CMD = Msg.GetIntegerValue( "CMD", 0 );

    if( CMD == CMD_LOGON_REPLY )
    {
        int             CIDBits = Msg.GetIntegerValue( "BITS", 9 );
        int             Use6DigiSeq;
        LogonResult     Result = lrFailed;
        UFC::AnsiString ReplyString = Msg.GetStringValue( "MSG" );

        if( FIsProxy == TRUE )
            FEncode = Msg.GetIntegerValue( "ENCODE", 0 );
        else
            FEncode = 0;
        FLogonEvent->SetEvent();
        FCurrentConnectionID  = Msg.GetIntegerValue( "CID", 0 );
        if( FCurrentConnectionID > 0 ) ///< CID > 0 means Logon OK!
	{
            UFC::GetTradeYYYYMMDD( FTradeingDate );
            FOrderPerSec = Msg.GetIntegerValue( "LIMIT", THROUGHPUT_DEF );
            FAdmin       = Msg.GetIntegerValue( "ADMIN", 0 );
            FIsDMA       = Msg.GetIntegerValue( "DMA", 0 );
            FUseNID64    = Msg.GetIntegerValue( "NID64", 0 );
            FLocalIP     = Msg.GetStringValue( "ClientIP" );
            Use6DigiSeq  = Msg.GetIntegerValue( "SEQ_6", 1 );

            Glog->fprintf( " Encode order message[%s]", (FEncode == 1)?"Yes":"No" );
            Glog->fprintf( " Throughput limit [%d]Orders per sec.", FOrderPerSec );
            Glog->fprintf( " %d digi filled sequence.", (Use6DigiSeq==1)? 6: 8 );
            if( Use6DigiSeq == TRUE ) ///< Default use 6 digi seq
                SetTWSE8DigiSeqVersion( FALSE );
            else
                SetTWSE8DigiSeqVersion( TRUE );                
            if( FAdmin == 1 )    ///< In Logon func: FID = FUserName = Logon ID.
                FID = FUserName; ///< In LogonProxy func: FID = Account, FUserName = Logon ID.
            FUserName = Msg.GetStringValue( "NAME", FUserName.c_str() );
            ///< Add Recover listeners.
            AddRecoverListener();
            ///< Add Execution reports listener.
            CreateReportListener( );
            ///< Init seq share memory.
            Result = CreateShareMemory( pTree, CIDBits, ReplyString );
            UFC::BufferedLog::Printf( " NID use [%d]bits rule", CIDBits );
        }
        else ///< Logon failed.
        {
            FIsLogon = false;
            Result = lrFailed;
        }
        if( FListener != NULL )
            FListener->OnLogonReply( ReplyString, Result, FCurrentConnectionID );
        if( FCurrentConnectionID <= 0 )
            new LogoffThread( this );
    }
    else if( CMD == CMD_RECOVER_FINISHED )
    {
        int COUNT = Msg.GetIntegerValue( "COUNT", 0 );
        if( FListener != NULL )
            FListener->OnRecoverFinished( COUNT );
    }
    else if( CMD == CMD_CXL_WORKING_REPLY  )
    {
        int COUNT = Msg.GetIntegerValue( "COUNT", 0 );
        int CODE  = Msg.GetIntegerValue( "CODE", cwrNoRights );
        if( FListener != NULL )
                FListener->OnCancelWorking( (CancelWorkingResult) CODE, COUNT );
    }
    else if( CMD == CMD_CHANGE_PASSWORD_REPLY )
    {
        ChangePwdResult Result;
        int Rtn = Msg.GetIntegerValue( "RESULT", 0 );
        
        Glog->fprintf( " Change Password result[%d]", Rtn );
        if( Rtn == -1 )
            Result = crWrongPassword;
        else if( Rtn == -2 )
            Result = crLockedTryAgain;
        else if( Rtn == -3 )
            Result = crSameAsPrevious;
        else if( Rtn == 1 )
            Result = crModifyOk;
        else
            Result = crFailed;
        if( FListener != NULL )
            FListener->OnChangePassword( Result );
        if( Result == crModifyOk )
        new LogoffThread( this );
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::AddExecListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, EventFunc* CBFunc )
{
    ///< Listen main account (Proxy) or AE(Gateway) Key = AE
    FExecListeners.Add( new TMdListener( FTransport, this, Subject, Key ) );
    Glog->fprintf( " %s() Add Exec Listener [%s][%s].", __func__, Subject.c_str(), Key.c_str() );  //ZhenFan 2026/01/22 begin
    ///< Listen others accounts.
    if( Key.AnsiCompare( "all" ) != 0  )
    {
        if( FIsProxy == TRUE )
        {
//ZhenFan 2026/01/22 begin
            UFC::AnsiString curAccountNo = "";
            BOOL isAccountNoExist = FAccountSet.First( curAccountNo );
            while ( isAccountNoExist == TRUE )
            {
                FExecListeners.Add( new TMdListener( FTransport, this, Subject, curAccountNo ) );
                Glog->fprintf( " %s() Add Proxy Exec Listener [%s][%s].", __func__, Subject.c_str(), curAccountNo.c_str() );
                isAccountNoExist = FAccountSet.Next( curAccountNo );
            }
//ZhenFan 2026/01/22 end
//            for( int i = 0; i < FAccounts.ItemCount(); i ++ )
//            {
//                UFC::AnsiString ExecAccount = FAccounts[ i ];
//                FExecListeners.Add( new TMdListener( FTransport, this, Subject, ExecAccount ) );
//            }
    	}
    }
    FCallbackFuncs.Add( Subject, CBFunc ); ///< Add Subject to callback function map.
}
//---------------------------------------------------------------------------
void TTaifexConnection::AddTAIFEXReportListener( const UFC::AnsiString& ListenKey )
{
    ///< Support Market TAIFEX
    if( FSupportSellSide.Exists( ssTAIFEX ) == TRUE )
    {
        Glog->fprintf( " Support TAIFEX" );
        if( FReportType == rdConfirm || FReportType == rdBoth )///< Add confirm listener.
        {
            Glog->fprintf( " - Add TAIFEX Confirm Listerner." );
            AddExecListener( SUBJECT_CONFIRM_FUT, ListenKey, &FOnFutConfirm ); ///< Futures Confirm
            AddExecListener( SUBJECT_CONFIRM_OPT, ListenKey, &FOnOptConfirm ); ///< Option Confirm
        }
        if( FReportType == rdFill || FReportType == rdBoth )///< Add fill listener.
        {
            Glog->fprintf( " - Add TAIFEX Filled Listerner." );
            AddExecListener( SUBJECT_FILL_FUT, ListenKey, &FOnFutFill ); ///< Futures filled
            AddExecListener( SUBJECT_FILL_OPT, ListenKey, &FOnOptFill ); ///< Option filled
        }
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::AddTWSEReportListener( const UFC::AnsiString& ListenKey )
{
    ///< Support Market TSE/OTC/ES
    if( FSupportSellSide.Exists( ssTSEOTC ) == TRUE )
    {
        Glog->fprintf( " Support TWSE/OTC/ES" );
        if( FReportType == rdConfirm || FReportType == rdBoth )///< Add confirm listener.
        {
            Glog->fprintf( " - Add TSE/OTC Confirm Listerner." );
            AddExecListener( SUBJECT_CONFIRM_TSE, ListenKey, &FOnTSEConfirm ); ///< TSE Confirm
            AddExecListener( SUBJECT_CONFIRM_OTC, ListenKey, &FOnOTCConfirm ); ///< OTC Confirm
            AddExecListener( SUBJECT_CONFIRM_ES, ListenKey, &FOnESConfirm );   ///< ES Confirm
        }

        if( FReportType == rdFill || FReportType == rdBoth )///< Add fill listener.
        {
            Glog->fprintf( " - Add TSE/OTC Filled Listerner." );
            AddExecListener( SUBJECT_FILL_TSE, ListenKey, &FOnTSEFill); ///< TSE filled
            AddExecListener( SUBJECT_FILL_OTC, ListenKey, &FOnOTCFill); ///< OTC filled
            AddExecListener( SUBJECT_FILL_ES,  ListenKey, &FOnESFill);  ///< ESfilled
        }
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::AddForeignExReportListener( const UFC::AnsiString& ListenKey )
{
    ///< Support Foreign Exchange
    if( FSupportSellSide.Exists( ssForeignExchange ) == TRUE )
    {
        Glog->fprintf( " Support Foreign Exchange" );
        if( FReportType == rdConfirm || FReportType == rdBoth )///< Add confirm listener.
        {
            Glog->fprintf( " - Add Foreign Confirm Listerner." );
            AddExecListener( SUBJECT_CONFIRM_FOREIGN, ListenKey, &FOnForeignConfirm );///< PATS Confirm
        }
        if( FReportType == rdFill || FReportType == rdBoth )///< Add fill listener.
        {
            Glog->fprintf( " - Add Foreign Filled Listerner." );
            AddExecListener( SUBJECT_FILL_FOREIGN, ListenKey, &FOnForeignFill );      ///< PATS filled
        }
    }
}
//---------------------------------------------------------------------------
//  added by Kenny to support Touch Order. 2026/03/16
void TTaifexConnection::AddTouchOrderResponseListener(const UFC::AnsiString& ListenKey)
{
    Glog->fprintf(" - Add Touch Order Response Listerner.");
    AddExecListener(SUBJECT_TOUCH_RESPONSE, ListenKey, &FOnTouchOrderResponse);///< Touch Order response
}
//---------------------------------------------------------------------------
void TTaifexConnection::CreateReportListener( void )
{
    if( FTriggerExec == TRUE )
    {
        UFC::AnsiString ListenKey;

        if( FAdmin == 1 ) ///< Admin receive all execution
        {
            ListenKey = "all";
            AddTAIFEXReportListener( ListenKey );
            AddTWSEReportListener( ListenKey );
            AddForeignExReportListener( ListenKey );
            AddTouchOrderResponseListener(ListenKey); // added by Kenny to support Touch Order. 2026/03/16
        }
        else  ///< Subscribes all IDs execution report.
        {
            for( int i = 0;i < FIDs.ItemCount(); i++ )
            {
                ListenKey = FIDs[i];
                AddTAIFEXReportListener( ListenKey );
                AddTWSEReportListener( ListenKey );
                AddForeignExReportListener( ListenKey );
                AddTouchOrderResponseListener(ListenKey); // added by Kenny to support Touch Order. 2026/03/16
            }
        }
    }
    ///< Add News Listener.
    FNewsListener = new TMdListener( FTransport, this, SUBJECT_NEWS_RESPONSE, FID.c_str() );
    FProxyNewsListener = new TMdListener( FTransport, this, SUBJECT_NEWS_RESPONSE, "Proxy" );
}
//---------------------------------------------------------------------------
void TTaifexConnection::RemoveListener( TMdListener*& Listener )
{
    if( Listener != NULL ) 
    {
        delete Listener;
        Listener = NULL;
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::Logoff( void )
{
    if( FUseAPI == atSpeedy  )
    {
        TMdListener* DelListener;

        FIsLogon = false;
        FID      = "";
        FToken   = "";
        FAdmin   = 0;
        FIsDMA   = 0;
        FNetworkID.Uninit();
        ///< Listeners for Administration.
        RemoveListener( FAdminListener );
        ///< Listener for Recover.
        for( int i = 0 ;i < FRecoverListeners.ItemCount(); i++ )
        {
            DelListener = FRecoverListeners[i];
            if( DelListener != NULL )
                    delete DelListener;
        }
        FRecoverListeners.Clear();
        ///< Clear all call back functions in map.
        FCallbackFuncs.Clear();
        ///< Add Admin,Recover,News call back functions to map.
        FCallbackFuncs.Add( SUBJECT_ADMIN,        &FOnAdminMsg );   ///< Admin message
        FCallbackFuncs.Add( SUBJECT_NEWS_RESPONSE,&FOnNewsMsg );    ///< News
        FCallbackFuncs.Add( SUBJECT_RECOVER,      &FOnRecoverMsg ); ///< Recover executions
        if( FTriggerExec == TRUE )
        {
            for( int i = 0 ;i < FExecListeners.ItemCount(); i++ )
            {
                DelListener = FExecListeners[i];
                if( DelListener != NULL )
                    delete DelListener;
            }
            FExecListeners.Clear();
        }
    }
}
//---------------------------------------------------------------------------
const char* DataTypeStr ( RecoverDataType DType )
{
    switch( DType )
    {
        case rdConfirm: return "Confirm"; ///< Recover order confirmation
	case rdFill:    return "Fill";    ///< Recover order filled execution.
	case rdBoth:    return "Confirm/Fill"; 
    }
    return "Unknown Type";
}
//---------------------------------------------------------------------------
const char* MarketStr ( RecoverMarket Mkt )
{
    switch( Mkt )
    {
        case rmFutures        : return "TAIFEX Futures";
	case rmOption         : return "TAIFEX Options";
	case rmTSE            : return "TWSE";
	case rmOTC            : return "OTC";
	case rmAll            : return "All Market";
	case rmForeignFutures : return "Foreign Futures";
	case rmForeignOptions : return "Foreign Options";
	case rmForeignStock   : return "Foreign Stock";
    }
    return "Unknown Market";
}
//---------------------------------------------------------------------------
const char* SessionStr( RecoverSession Session )
{
    switch( Session )
    {
	case rsNormal      : return "Normal Session";
	case rsOffHour     : return "Off-hour Session";
	case rsAll         : return "All Session";
        case rsPrevOffHour : return "Prev Off-hour Session";
    }
    return "Unknown Session";
} 
//---------------------------------------------------------------------------
void TTaifexConnection::DoRecover(  const char* ID,
                                    const char* BeginTime,
                                    const char* EndTime,
                                    RecoverDataType Type,
                                    RecoverMarket   Market,
                                    RecoverSession  Session,
                                    UFC::AnsiString RecoverYYYYMMDD)
{
    TMdMessage Msg;
    UFC::AnsiString ETime(EndTime);

    Msg.SetIntegerValue( "CMD", CMD_RECOVER_REQUEST );
    Msg.SetStringValue( "BEGINTIME", BeginTime );
    if(	ETime.Length() >= 6 )
        Msg.SetStringValue( "ENDTIME", EndTime );
    else
        ETime = "Now";
    Msg.SetIntegerValue( "TYPE",   (Int32)Type );
    Msg.SetIntegerValue( "MARKET", (Int32)Market );
    if( Session != rsAll )
        Msg.SetIntegerValue( "TRADINGSESSION", (Int32)Session );
    if (RecoverYYYYMMDD.IsEmpty() == FALSE)
        Msg.SetStringValue( "RECOVERDATE", RecoverYYYYMMDD );
    Msg.SetIntegerValue( "CID",   FCurrentConnectionID );
    Msg.SetStringValue( "ID",     ID );
    Msg.SetIntegerValue( "ADMIN", FAdmin );    
    if( RecoverYYYYMMDD.Length() > 0 )
        UFC::BufferedLog::Printf( " Recover( ID[%s] Time[%s]~[%s] Execution[%s][%s][%d] Date[%s])", ID, BeginTime, ETime.c_str(), DataTypeStr(Type), MarketStr(Market), SessionStr(Session), RecoverYYYYMMDD.c_str() );
    else    
        UFC::BufferedLog::Printf( " Recover( ID[%s] Time[%s]~[%s] Execution[%s][%s][%s] Date[Today])", ID, BeginTime, ETime.c_str(), DataTypeStr(Type), MarketStr(Market), SessionStr(Session) );
    FTransport->Send( SUBJECT_ADMIN.c_str(), FUniquekey.c_str(), &Msg );
}
//---------------------------------------------------------------------------
void TTaifexConnection::RecoverFromLocalFile( const char* BeginTime, RecoverDataType Type, RecoverMarket Market, RecoverSession SessionType  )
{
    if( FIsLogon == true )
    {    
        new APIRecoverExecution(  BeginTime,
                                  FUniquekey.c_str(),
                                  FID.c_str(),
                                  FCurrentConnectionID,
                                  Market,
                                  Type,
                                  SessionType,                                
                                  FAdmin,
                                  5000,
                                  this );
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::Recover( const char* BeginTime, RecoverDataType Type, RecoverMarket Market, RecoverSession Session, UFC::AnsiString RecoverYYYYMMDD )
{
    if( FIsLogon == true )
    {
        if( FUseAPI == atSpeedy )
        {
            if( FTriggerExec == true ) ///< Accept Executioons
            {
                Glog->fprintf( " %s() Recover ID[%s] Type[%d] Market[%d] from [%s][%s].", __func__, FID.c_str(), Type, Market, RecoverYYYYMMDD.c_str(), BeginTime );  //ZhenFan 2026/01/22 begin
                DoRecover( FID.c_str(),BeginTime, "", Type, Market, Session, RecoverYYYYMMDD );
                if( FIsProxy == true )
                {
//ZhenFan 2026/01/22 begin
                    UFC::AnsiString curAccountNo = "";
                    BOOL isAccountNoExist = FAccountSet.First( curAccountNo );
                    while ( isAccountNoExist == TRUE )
                    {
                        if( curAccountNo != FID )
                        {
                            Glog->fprintf( " %s() Recover Proxy Account[%s] Type[%d] Market[%d] from [%s][%s].", __func__, curAccountNo.c_str(), Type, Market, RecoverYYYYMMDD.c_str(), BeginTime );
                            DoRecover( curAccountNo.c_str(), BeginTime, "", Type, Market, Session, RecoverYYYYMMDD );
                            UFC::SleepMS( 100 );
                        }
                        isAccountNoExist = FAccountSet.Next( curAccountNo );
                    }
//ZhenFan 2026/01/22 end
//                    for( int i = 0; i < FAccounts.ItemCount(); i ++ )
//                    {
//                        UFC::AnsiString ExecAccount = FAccounts[ i ];
//
//                        if( ExecAccount != FID )
//                        {
//                            DoRecover( ExecAccount.c_str(), BeginTime, "", Type, Market, Session, RecoverYYYYMMDD );
//                            UFC::SleepMS( 100 );
//                        }
//                    }
                }
            }
            else ///< Session to place order only.
            {
                if( FListener != NULL )
                    FListener->OnRecoverFinished( 0 );
            }
        }
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::Recover( const char* BeginTime, const char* EndTime, RecoverDataType Type, RecoverMarket Market, RecoverSession Session, UFC::AnsiString RecoverYYYYMMDD )
{
    if( FIsLogon == true )
    {
        if( FUseAPI == atSpeedy )
        {
            if( FTriggerExec == true ) ///< Accept Executioons
            {
                DoRecover( FID.c_str(), BeginTime, EndTime, Type, Market, Session, RecoverYYYYMMDD );
            }
            else ///< Session to place order only.
            {
                if( FListener != NULL )
                    FListener->OnRecoverFinished( 0 );
            }
        }
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::CancelWorking( CancelMarket Market, const UFC::AnsiString& AE, const UFC::AnsiString& Account, const UFC::AnsiString& UserData, const UFC::AnsiString& ProdID, const UFC::AnsiString& CancelUDD  )
{   ///< Every one can lock yourself.
    if( FIsLogon == true && FUseAPI == atSpeedy )
    {
        TMdMessage Msg;

        Msg.SetIntegerValue( "CMD", CMD_CXL_WORKING );
        Msg.SetIntegerValue( "Market", (Int32)Market );
        if( Account.Length() > 0 && Account != "all" && Account != "ALL" )
            Msg.SetStringValue( "Account", Account );
        if( AE.Length() > 0  )///< given an AE
        {
            if( AE == "all" || AE == "ALL" ) ///< AE can cancel it's own orders
            {
                if( FAdmin == false  )
                    Msg.SetStringValue( "AE", FID );
            }
            else
            {
                if( FAdmin == false && FID != AE )
                {
                    if( FListener != NULL )
                        FListener->OnCancelWorking( cwrNoRights, 0 );
                    return;
                }
                Msg.SetStringValue( "AE", AE );
            }
        }
        else ///< All AE
        {
            if( FAdmin == false  )
                Msg.SetStringValue( "AE", FID );
        }
        if( UserData.Length() > 0 && UserData != "all" && UserData != "ALL" )
            Msg.SetStringValue( "UserData", UserData );
        if( ProdID.Length() > 0 && ProdID != "all" && ProdID != "ALL" )
            Msg.SetStringValue( "ProdID", ProdID );
        if (CancelUDD.Length() > 0)
        {
            Msg.SetStringValue( "CancelUDD", CancelUDD );
        }
        Msg.SetStringValue( "Key", FUniquekey );
        FTransport->Send( SUBJECT_ADMIN.c_str(), FUniquekey.c_str(), &Msg );
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::Lock( bool Locked )
{   ///< Every one can lock yourself.
    if( FIsLogon == true && FUseAPI == atSpeedy )
    {
        TMdMessage Msg;

        Msg.SetIntegerValue( "CMD", CMD_LOCK_AE );
        Msg.SetStringValue( "ID", FID.c_str() );
        if( Locked == true )
            Msg.SetIntegerValue( "LOCK", (Int32)1 );
        else
            Msg.SetIntegerValue( "LOCK", (Int32)0 );
        FTransport->Send( SUBJECT_ADMIN.c_str(), FUniquekey.c_str(), &Msg );
    }
}
//---------------------------------------------------------------------------
void  TTaifexConnection::Lock( const char* ID, bool Locked )
{   ///< Only Admin can lock every AE.
	if( FIsLogon == TRUE && FAdmin == TRUE && FUseAPI == atSpeedy )
    {
        TMdMessage Msg;

        Msg.SetIntegerValue( "CMD", CMD_LOCK_AE );
        Msg.SetStringValue( "ID", ID );
        if( Locked == true )
            Msg.SetIntegerValue( "LOCK", (Int32)1 );
        else
            Msg.SetIntegerValue( "LOCK", (Int32)0 );
        FTransport->Send( SUBJECT_ADMIN.c_str(), FUniquekey.c_str(), &Msg );
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::RequestFile( nsOrderMessageDefine::MarketEnum Market, const char* FileCode, const char* ReqData )
{
    if( FUseAPI == atSpeedy  )
    {
        TMdMessage Msg;

        Msg.SetStringValue( "FileCode", FileCode );
        Msg.SetStringValue( "Data", ReqData );
        if( Market  == nsOrderMessageDefine::mTWFutures )
            FTransport->Send( SUBJECT_FT_FUT.c_str(), UFC::Hostname, &Msg );
        else if( Market  == nsOrderMessageDefine::mTWOptions )
            FTransport->Send( SUBJECT_FT_OPT.c_str(), UFC::Hostname, &Msg );
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::RequestFile( nsOrderMessageDefine::MarketEnum Market, BOOL IsMorningSession, Int32 ReqID, const char* Body )
{
    if( FUseAPI == atSpeedy  )
    {
        TMdMessage Msg;

        if( IsMorningSession == TRUE )
            Msg.SetIntegerValue( "TradeKind", 0 );
        else
            Msg.SetIntegerValue( "TradeKind", 1 );
        Msg.SetIntegerValue( "ReqID", ReqID );
        Msg.SetStringValue( "Body", Body );
        if( Market  == nsOrderMessageDefine::mTWFutures )
            FTransport->Send( SUBJECT_FT_FUT.c_str(), UFC::Hostname, &Msg );
        else if( Market  == nsOrderMessageDefine::mTWOptions )
            FTransport->Send( SUBJECT_FT_OPT.c_str(), UFC::Hostname, &Msg );
    }
}
//---------------------------------------------------------------------------
void TTaifexConnection::GetRejectMsg(  const UFC::AnsiString&   StatusCode,
                                       const UFC::AnsiString&   ErrMsg,
                                       TExecutionReportMessage* Msg,
                                       UFC::AnsiString&         ErrorStr )
{
	UFC::AnsiString ExchangeCode;

	///< Get Exchange code.
	switch( Msg->GetMarket() )
	{
            case nsOrderMessageDefine::mOTC:
            case nsOrderMessageDefine::mTSE:      if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsOddLot )
                                                      ExchangeCode = "TWSEOdd";
                                                  else
                                                      ExchangeCode = "TWSE";
                                                  break;
            case nsOrderMessageDefine::mTWFutures:
            case nsOrderMessageDefine::mTWOptions:ExchangeCode = "TAIFEX";
                                                  break;
            default:                              ExchangeCode = Msg->GetExchangeCode();
						  break;
	}
	if( ErrMsg.Length() > 0 && ErrMsg != "Reject" ) ///< Has reject message.
	{
            Msg->SetStringValue( "MSG", ErrMsg );
            if( StatusCode.ToInt() == 97 ) ///< Reject by RiskManager.
                ErrorStr.Printf( "RM Reject[%s]", ErrMsg.c_str());            
            else
                ErrorStr = ErrMsg;
	}
	else ///< Only Exchange error code. No reject message, look up the error code table.
	{
            if( FExchangeError->GetValue( ExchangeCode, StatusCode, ErrorStr ) == FALSE )
                ErrorStr.Printf( "Exchange error code:%s", StatusCode.c_str() );
	}	
}        
//---------------------------------------------------------------------------
void TTaifexConnection::FillRejectMsg( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo,
                                        const UFC::AnsiString& StatusCode,
                                        const UFC::AnsiString& ErrMsg,
                                        TExecutionReportMessage* Msg )
{
    UFC::AnsiString ErrorStr;

    GetRejectMsg( StatusCode, ErrMsg, Msg, ErrorStr );        	
    ///< Set error message to execution report message.
    Msg->SetText( ErrorStr );
    Msg->SetOrderStatus( nsOrderMessageDefine::osRejected );
    Msg->SetExecType( nsOrderMessageDefine::etRejected );
    Msg->SetCxlRejResponseTo( CxlRejResponseTo );
}
//---------------------------------------------------------------------------
void TTaifexConnection::Reject( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo,
								const UFC::AnsiString& ErrMsg,
                                const UFC::AnsiString& UDD,
                                TBaseMessage* Msg,
                                nsOrderMessageDefine::TradingSessionIDEnum TradingSession )
{
    TExecutionReportMessage ExecutionReport;
    UFC::AnsiString SendTime;

    UFC::GetTimeString( SendTime , false );	
    ExecutionReport.SetStatusCode( "89" ); ///< Not send to Speedy
    ExecutionReport.SetAccount( Msg->GetAccount() );
    ExecutionReport.SetBrokerID( Msg->GetBrokerID() );
    ExecutionReport.SetSymbol( Msg->GetSymbol() );
    ExecutionReport.SetMarket( Msg->GetMarket() );
    ExecutionReport.SetUserData( UDD.c_str() );
    ExecutionReport.SetNID( Msg->GetNID() );
    ExecutionReport.SetOrderID( Msg->GetOrderID() );
    ExecutionReport.SetPositionEffect( Msg->GetPositionEffect() );
    ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osRejected );
    ExecutionReport.SetExecType( nsOrderMessageDefine::etRejected );
    ExecutionReport.SetCxlRejResponseTo( CxlRejResponseTo );
    ExecutionReport.SetTransactTime( SendTime.c_str());
    ExecutionReport.SetText( ErrMsg.c_str() );
    ExecutionReport.SetTradingSessionID( TradingSession );
    ExecutionReport.SetPrice( Msg->GetPrice() );
    ExecutionReport.SetOrderQty( Msg->GetOrderQty() );
    ExecutionReport.SetSide( Msg->GetSide() );
    Glog->fprintf( " Reject Reason[%s]", ErrMsg.c_str() );
    TrigerOnExecutionReport( &ExecutionReport, edSpeedyGenerate );
    Glog->fprintf( " NID[%d] Callback Reject OnExecutionReport.", Msg->GetNID() );
}
//---------------------------------------------------------------------------
void TTaifexConnection::TrigerOnExecutionReport( TExecutionReportMessage* ExecutionReport, ExecDup PosDup )
{
    UFC::PLockObject Locker( FExecCS );

    if( FListener != NULL )
        FListener->OnExecutionReport( ExecutionReport, PosDup );
}
//---------------------------------------------------------------------------
void TTaifexConnection::PendingNew( TNewOrderMessage* Msg )
{
    TExecutionReportMessage ExecutionReport;
    UFC::AnsiString SendTime;

    UFC::GetTimeString( SendTime , false );
    SendTime.PadThis( 8,'0' );
    ExecutionReport.SetAccount( Msg->GetAccount() );
    ExecutionReport.SetBrokerID( Msg->GetBrokerID() );
    ExecutionReport.SetSymbol( Msg->GetSymbol() );
    ExecutionReport.SetMarket( Msg->GetMarket() );
    ExecutionReport.SetNID( Msg->GetNID() );
    ExecutionReport.SetOrderID( Msg->GetOrderID() );
    ExecutionReport.SetPositionEffect( Msg->GetPositionEffect() );
    ExecutionReport.SetOrderStatus( nsOrderMessageDefine::osPendingNew );
    ExecutionReport.SetExecType( nsOrderMessageDefine::etPendingNew );
    ExecutionReport.SetTransactTime( SendTime.c_str());
    ExecutionReport.SetTSEOrderType( Msg->GetTSEOrderType() );
    ExecutionReport.SetTradingSessionID( Msg->GetTradingSessionID());
    ExecutionReport.SetOrderType( Msg->GetOrderType() );
    ExecutionReport.SetTimeInForce( Msg->GetTimeInForce() );
    TrigerOnExecutionReport( &ExecutionReport, edSpeedyGenerate );
}
//---------------------------------------------------------------------------
void TTaifexConnection::SetBrokerID( nsOrderMessageDefine::MarketEnum Market, const char* FCMID)
{
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
        case nsOrderMessageDefine::mTWOptions:
        case nsOrderMessageDefine::mForeignFutures:
        case nsOrderMessageDefine::mForeignOptions:FTAIFEXBrokerID = FCMID; break;
        case nsOrderMessageDefine::mTSE:
        case nsOrderMessageDefine::mOTC:
        case nsOrderMessageDefine::mES:
        case nsOrderMessageDefine::mForeignStock:  FTSEOTCBrokerID = FCMID; break;
        default: break;
    }
}
//---------------------------------------------------------------------------
const UFC::AnsiString& TTaifexConnection::GetBrokerID( nsOrderMessageDefine::MarketEnum Market )
{
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
        case nsOrderMessageDefine::mTWOptions:
        case nsOrderMessageDefine::mForeignFutures:
        case nsOrderMessageDefine::mForeignOptions: return FTAIFEXBrokerID;
        case nsOrderMessageDefine::mTSE:
        case nsOrderMessageDefine::mOTC:
        case nsOrderMessageDefine::mES:
        case nsOrderMessageDefine::mForeignStock:   return FTSEOTCBrokerID;
        default: break;
    }
    return FTAIFEXBrokerID;
}
//---------------------------------------------------------------------------
void TTaifexConnection::AddSellSide( SellSideType SellSide )
{
    FSupportSellSide.Add( SellSide );
}
//---------------------------------------------------------------------------
void TTaifexConnection::RemoveSellSide( SellSideType SellSide )
{
    FSupportSellSide.Delete( SellSide );
}
//---------------------------------------------------------------------------
int TTaifexConnection::GetQueueSize( void )
{
    if( FUseAPI == atSpeedy && FTransport != NULL )
        return FTransport->GetQueuesize();
    return 0;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::IsLogon( void )
{
    return FIsLogon;
}
//---------------------------------------------------------------------------
bool TTaifexConnection::IsConnected( void )
{
    return FIsConnected;
}
//---------------------------------------------------------------------------
void TTaifexConnection::CreateCAObject()
{
//#ifdef __UNICA_WIN
#ifdef WIN32
/// Allen Modify at 20190615
#ifndef _MSC_VER
/*
	if (FCAObjPtr != 0) DeleteCAObject();
	FCAOrganizationalUnit = "PSCNET";
	FCAObjPtr = new CCACGCBrowserObject(FCADLLFileName, FCAOrganizationalUnit, FCACommonName);
	if (FCAObjPtr->IsWork())
		Glog->fprintf( " Create CA Object." );
	else
		Glog->fprintf( " Create CA Object failed. %s", FCAObjPtr->GetErrorMessage().c_str() );
*/
#endif
#endif
}  //TTaifexConnection::CreateCAObject()
//---------------------------------------------------------------------------
void TTaifexConnection::DeleteCAObject()
{
//#ifdef __UNICA_WIN
#ifdef WIN32
/*
	if ( FCAObjPtr != 0 )
	{
		delete FCAObjPtr;
		FCAObjPtr = 0;
	}
*/
	if( FApiCAObjPtr != 0 )
	{
		delete FApiCAObjPtr;
		FApiCAObjPtr = 0;
	}

#endif
}
//---------------------------------------------------------------------------
bool TTaifexConnection::CreateUniFSCAObject()
{
	bool isSuccess = false;
#ifdef WIN32
	FApiCAObjPtr = new CUniFSCCAObject(FCACommonName, Glog);
	if (FApiCAObjPtr->IsWorking())
	{
		isSuccess = true;
		Glog->fprintf( " Create Uni FSCA Object success." );
	}
	else
		Glog->fprintf( " Create Uni FSCA Object failed." );
#endif
	return isSuccess;
}  //TTaifexConnection::CreateUniFSCAObject()
//---------------------------------------------------------------------------
bool TTaifexConnection::CreateMLTWCAObject()
{
	bool isSuccess = false;
#ifdef WIN32
	FApiCAObjPtr = new CMLTWCAObject(FCACommonName, Glog);
	if (FApiCAObjPtr->IsWorking())
	{
		isSuccess = true;
		Glog->fprintf( " Create ML TWCA Object success." );
	}
	else
		Glog->fprintf( " Create ML TWCA Object failed." );
#endif
	return isSuccess;
}  //TTaifexConnection::CreateMLTWCAObject()
//---------------------------------------------------------------------------
bool TTaifexConnection::CheckCALogonData( const UFC::AnsiString& LogonData, CAResultData& CAResult )
{
	bool isSuccess = false;
	UFC::AnsiString checkMsg = "";
	if (FApiCAObjPtr != 0)
	{
		if( FApiCAObjPtr->IsWorking() )
		{
                    CAResult.SetCAObj( FApiCAObjPtr );
                    if (FApiCAObjPtr->GenerateLogonSignatureAndCAData(LogonData, CAResult) == 0)
                    {
                        isSuccess = true;
                        Glog->fprintf( " %s() PlainText[%s].", __func__, CAResult.GetPlainText().c_str() );
                        Glog->fprintf( " %s() Subject[%s].", __func__, CAResult.GetSubject().c_str() );
                        Glog->fprintf( " %s() NotBefore[%s].", __func__, CAResult.GetNotBefore().c_str() );
                        Glog->fprintf( " %s() NotAfter[%s].", __func__, CAResult.GetNotAfter().c_str() );
                        Glog->fprintf( " %s() SerialNumber[%s].", __func__, CAResult.GetSerialNumber().c_str() );
                    }
                    else
                        Glog->fprintf( " %s() Error:%s.", __func__, CAResult.GetResultMsg().c_str() );
		}
		else
                    checkMsg.Printf( "CA Object is not Working." );
		CAResult.SetResultMsg(checkMsg);
	}
	else
	{
            Glog->fprintf( " %s() No CA Object.", __func__ );
            isSuccess = true;
	}
	return isSuccess;
}  //TTaifexConnection::CheckCALogonData()
//---------------------------------------------------------------------------
void  TTaifexConnection::Stdout( BOOL ToStdOut )
{
    if( ToStdOut == false )
    {
        UFC::BufferedLog::SetPrintToStdout( FALSE );
        Glog->SetToStdout(FALSE);
    }
    else
    {
        UFC::BufferedLog::SetPrintToStdout( TRUE );
        Glog->SetToStdout(TRUE);
    }
}
//---------------------------------------------------------------------------
