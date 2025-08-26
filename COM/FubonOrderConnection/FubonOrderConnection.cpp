#include "FubonOrderConnection.h"
#include "../../UFC/NetUtility.h"
#include "../../UFC/FileStream.h"
//------------------------------------------------------------------------------
extern UFC::AnsiString FLogonID;
extern UFC::PInt32     FRecoverBeginTime;
extern Int32           FOverlapTime;
//------------------------------------------------------------------------------
UFC::AnsiString    gStockFormatPath               = "../rmdata/";
UFC::AnsiString    gStockT30SFormatFileName       = "Fubon_T30VTSE.ini";
UFC::AnsiString    gStockT30OFormatFileName       = "Fubon_T30VOTC.ini";
UFC::AnsiString    gStockT30SFormatFullFileName;
UFC::AnsiString    gStockT30OFormatFullFileName;

UFC::AnsiString    gStockDataFilePath             = "../rmdata/";
UFC::AnsiString    gStockT30SDataFileName         =  "T30V.TSE";
UFC::AnsiString    gStockT30ODataFileName         =  "T30V.OTC";
UFC::AnsiString    gStockT30SDataFullFileName;
UFC::AnsiString    gStockT30ODataFullFileName;

int                gPriceDigi                     =   4;
//------------------------------------------------------------------------------
// MsgUserData
//------------------------------------------------------------------------------
MsgUserData::MsgUserData()
:FHTSQ("0000")
,FINSQ("00000000")
,FMGSQ("00000000")
,FKTIME("")
,FSCODE("9")
,FOCODE("D")
,FTLNO("      ")
,FSALE("   ")
,FLINE("")
,FGROUP("      ")
,FAREA("")
,FTERM("")
,FKTYPE("")
,FKTIME9("")
,FMCODE("   ")
,FPRTYPE("")
,FORCT("")
,FBATCHNO("        ")
,FBATEND(" ")
,FCRPER("00")
,FSTIX(" ")
,FPBSAMT("000000000000")
,FLAMT("00000000")
,FFFLAG(" ")
,FAGROUP("        ")
,FP0SQTY("0000000000")
,FP0SAMT("000000000000")
,FFSTSEQ("00")
,FE01USE("          ")
,FLPRICE("00000000000000")
,FTQTY("")
,FCUGC(" ")
,FSTLM(" ")
,FRTICKET("     ")
,FROSEQ("     ")
,FSTUSCD("    ")
,FMQD3("000")
,FHOT(" ")
,FSNCODE("5")
,FFILLER("                                                                   ")
{}
//------------------------------------------------------------------------------
void MsgUserData::ToString( UFC::AnsiString& UserData )
{
    UserData.Printf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s"
        ,FHTSQ.c_str()
        ,FINSQ.c_str()
        ,FMGSQ.c_str()
        ,FKTIME.c_str()
        ,FSCODE.c_str()
        ,FOCODE.c_str()
        ,FTLNO.c_str()
        ,FSALE.c_str()
        ,FLINE.c_str()
        ,FGROUP.c_str()
        ,FAREA.c_str()
        ,FTERM.c_str()
        ,FKTYPE.c_str()
        ,FKTIME9.c_str()
        ,FMCODE.c_str()
        ,FPRTYPE.c_str()
        ,FORCT.c_str()
        ,FBATCHNO.c_str()
        ,FBATEND.c_str()
        ,FCRPER.c_str()
        ,FSTIX.c_str()
        ,FPBSAMT.c_str()
        ,FLAMT.c_str()
        ,FFFLAG.c_str()
        ,FAGROUP.c_str()
        ,FP0SQTY.c_str()
        ,FP0SAMT.c_str()
        ,FFSTSEQ.c_str()
        ,FE01USE.c_str()
        ,FLPRICE.c_str()
        ,FTQTY.c_str()
        ,FCUGC.c_str()
        ,FSTLM.c_str()
        ,FRTICKET.c_str()
        ,FROSEQ.c_str()
        ,FSTUSCD.c_str()
        ,FMQD3.c_str()
        ,FHOT.c_str()
        ,FSNCODE.c_str()
        ,FFILLER.c_str()
        );
}
//------------------------------------------------------------------------------
void MsgUserData::CreateUserData( TExecutionReportMessage* Msg )
{   
    UFC::GetHHMMSSmmm( FKTIME9 );
    FKTIME9.PadThis( 9, ' ');
    FKTIME = FKTIME9.SubString( 0, 6 );
        
    FAREA = Msg->GetBrokerID();
    FAREA.PadThis( 4, ' ' );
    
    UFC::AnsiString value;
    value = Msg->GetOrderID();
    FTERM = value[0];
    
    value = Msg->GetTSEOrderType();
    value.TrimRight();
    if( value == "5" || value == "6" )
        FKTYPE = value;
    else
    {
        if( Msg->GetSide() == nsOrderMessageDefine::sBuy )
            FKTYPE = "0";
        else
            FKTYPE = "9"; 
    }
    
    if( Msg->GetOrderType() == nsOrderMessageDefine::otMarket )
        FPRTYPE = "1";
    else if( Msg->GetOrderType() == nsOrderMessageDefine::otLimit )
        FPRTYPE = "2";
    if( Msg->GetTimeInForce( ) == nsOrderMessageDefine::tifIOC )
        FORCT = "3";
    else if( Msg->GetTimeInForce( ) == nsOrderMessageDefine::tifFOK )
        FORCT = "4";
    else
        FORCT = "0";
    
    FFFLAG.Printf("%c", Msg->GetAccountFlag()[0]);
    
    if( Msg->GetOrderStatus() == nsOrderMessageDefine::osNew || Msg->GetOrderStatus() == nsOrderMessageDefine::osRejected )
        FTQTY.Printf("%06d", Msg->GetOrderQty());
    else
        FTQTY.Printf("%06d", 0);
}
//------------------------------------------------------------------------------
void MsgUserData::PriceToString( UFC::AnsiString& PriceStr, double dPrice )
{
    int iDecimals = 0;
    if( dPrice > 0 )
    {
        iDecimals = UFC::DoubleToInt( dPrice, 6 );
        PriceStr.Printf("%014d", iDecimals);
    }
}
//------------------------------------------------------------------------------
// FubonOrderConnection
//------------------------------------------------------------------------------
TFubonOrderConnection::TFubonOrderConnection( const UFC::AnsiString& ConfigFileName, const UFC::AnsiString& BrokerID )
:UFC::PThread( NULL, FALSE )
,FBrokerID( BrokerID )
,FDebug( "" )
,FLogFile( "" )
,FLockFile( "" )
,FAppName( "" )
,FIP( "" )
,FPort( 0 )
,FID( FLogonID )
,FPassword( "" )
,FGroup( 0 )
,FSentLineFileName("../cfg/SentLineNo.ini")
,FSkipESExec( TRUE )
,FSkipExec( FALSE )
,FSkipAE( "OTHERS" )
,FUsePlusVK( FALSE )
,FReadBufferSize( 0 )
,FTradeTimeout( 60 )
,FRecoverCount( 0 )
,FToday( "" )
,FConnection( NULL )
,FOrderQueue( NULL )
,FConfirmQueue( NULL )
,FFilledQueue( NULL )
,FReadBuffer( NULL )
,FConnectionState( focDisconnect )
{
    UFC::GetYYYYMMDD( FToday );
    ReadConfigFile( ConfigFileName );
    LockFile();
    BuildSentLineNumberTable(); 
        
    FReadBuffer   = new char[ FReadBufferSize ];
    FConnection   = new TTaifexConnection( FAppName, this );
    FOrderQueue   = new TSysque( FQueIP, FQuePort, FOrderQueName );
    FConfirmQueue = new TSysque( FQueIP, FQuePort, FConfirmQueName );
    FFilledQueue  = new TSysque( FQueIP, FQuePort, FFilledQueName );

    if( FOrderQueue->Open() && FConfirmQueue->Open() && FFilledQueue->Open() )
    {
        ///< Load TSEC/OTC  message format config files.
        FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
        FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
        FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
        FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
        Start();
    }
    else
        exit(0);    
}
//------------------------------------------------------------------------------
TFubonOrderConnection::~TFubonOrderConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;   
    if( FOrderQueue != NULL )
        delete FOrderQueue;    
    if( FConfirmQueue != NULL )
        delete FConfirmQueue;    
    if( FFilledQueue != NULL )
        delete FFilledQueue;
    for( int i = 0; i < FIPPool.ItemCount(); i++ )
    {
        UFC::AnsiString* str = FIPPool.DequeueNoWait();
        if( str )
            delete str;
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::BuildSentLineNumberTable( void )
{
    system( "touch ./SentLineNoTemp.ini" );
    try
    {   
        char Buffer[ 128 ];        
        
        UFC::FileStreamEx SentLineNoFile( FSentLineFileName, "r+" );
        UFC::RemoveFile( "./SentLineNoTemp.ini" );
        FSentLineNoTable = new UFC::UiniFile( "./SentLineNoTemp.ini", TRUE );
        while( SentLineNoFile.Read( Buffer, 64 ) == 64 )
        {
            if( Buffer[3]!= 'Y' &&  Buffer[3]!= 'y'  && Buffer[3]!= '0' ) ///< Valid data record.
            {
                UFC::AnsiString Key;
                UFC::AnsiString Host( Buffer+4, 24);
                UFC::AnsiString BrokerID( Buffer+28, 4);
                UFC::AnsiString PVCID( Buffer+32, 2 );
                UFC::AnsiString Market( Buffer+34,1 );
                
                Host.TrimRight();
                if( Market[0] == '2' )                
                    Key.Printf( "%s_O", BrokerID.c_str() );
                else
                    Key.Printf( "%s_T", BrokerID.c_str() );
                FSentLineNoTable->SetValue( Host, Key, PVCID );                
            }            
        }                
        FSentLineNoTable->Save();
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Can't open sent line number setting file:[%s]",FSentLineFileName.c_str() );
        exit(0);
    }    
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::LockFile( void )
{
    UFC::AnsiString LockFileName;
    LockFileName.Printf( "%s%s_%s_lock", FLockFile.c_str(), FBrokerID.c_str(), FID.c_str() );
    UFC::BufferedLog::DebugPrintf( " Try to open and lock file:[%s].", LockFileName.c_str() );
    
    int fd = open( LockFileName.c_str(), O_WRONLY | O_CREAT, 664 );//0600 );
    if( fd > 0 )
    {
        flock fl;
        fl.l_type   = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start  = 0;
        fl.l_len    = 0;
        fl.l_pid    = getpid();
        
        if( fcntl( fd, F_SETLKW, &fl ) == 0 )
            UFC::BufferedLog::DebugPrintf( " File:[%s] locked.", LockFileName.c_str() );
        else
        {
            UFC::BufferedLog::DebugPrintf( " Unable to lock file:[%s].", LockFileName.c_str() );
            exit(0);
        }
    }
    else
    {
        UFC::BufferedLog::DebugPrintf( " Can not open file:[%s]", LockFileName.c_str() );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::SetLogObject( void )
{
    UFC::AnsiString LogFileName;
    LogFileName.Printf( "%s/%s_%s_FOC%s.log", FLogFile.c_str(), FBrokerID.c_str(),FID.c_str(), FToday.c_str() );

    UFC::BufferedLog::DebugPrintf( " Write log to file:[%s]", LogFileName.c_str() );    
    UFC::BufferedLog::SetDebugMode( (FDebug == "1") );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogFileName, 102400, TRUE ) );
}
//------------------------------------------------------------------------------
//
// Config file sample:
//
// [main]
// debug =0
// log_path=../log
// lock_path=.
// LocalLineNoConfigFile=./LocalLineNo.ini
// SentLineNoConfigFile=./SentLineNo.ini
//
// [speedy]
// app_name=FubonOrderConnection
// ip=192.168.0.3,192.168.0.120
// port=23456
// password=1234rewq
// group=0
//
// [sysque]
// read_buffer_size=512
// trade_timeout=60
//
//------------------------------------------------------------------------------
void TFubonOrderConnection::ReadConfigFile( const UFC::AnsiString& ConfigFileName )
{
    try
    {
        UFC::UiniFile iniFile( ConfigFileName );
        UFC::AnsiString Value;

        iniFile.GetValue( "main", "debug", FDebug );
        iniFile.GetValue( "main", "log_path", FLogFile );
        iniFile.GetValue( "main", "lock_path", FLockFile );
        iniFile.GetValue( "main", "SentLineNoConfigFile", FSentLineFileName );
        
        if( iniFile.ValueExists( "main", "ExecSkipAE") == TRUE )
        {
            iniFile.GetValue( "main", "ExecSkipAE", FSkipAE );
            FSkipExec = TRUE;
        }
        else
            FSkipExec = FALSE;            
        Value = "1";
        iniFile.GetValue( "main", "ExecSkipES", Value );        
        FSkipESExec = Value.ToInt();
        
        Value = "1";
        if( iniFile.ValueExists( "main", "UsePlusVK") == TRUE )
        {
            iniFile.GetValue( "main", "UsePlusVK", Value );
                FUsePlusVK = Value.ToInt();
        }
        else
            FUsePlusVK = FALSE;
        
        iniFile.GetValue( "speedy", "app_name", FAppName );
        iniFile.GetValue( "speedy", "ip", FIP );
        iniFile.GetValue( "speedy", "port", Value );
        FPort = Value.ToInt();
        
        Value = "0";        
        iniFile.GetValue( "speedy", "password", FPassword );
        if( iniFile.GetValue( "speedy", "group", Value ) )
            FGroup = Value.ToInt();        
        
        Value = "0";
        iniFile.GetValue( "sysque", "ip", FQueIP );
        iniFile.GetValue( "sysque", "port", Value );
        FQuePort = Value.ToInt();        
		
        Value = "512";
        iniFile.GetValue( "sysque", "read_buffer_size", Value );
        FReadBufferSize = Value.ToInt();
        
        Value = "60";        
        iniFile.GetValue( "sysque", "trade_timeout", Value );
        FTradeTimeout = Value.ToInt();
        
        if( FDebug.Length() <= 0 )
        {
            FDebug = "0";
            iniFile.SetValue( "main", "debug", FDebug );
        }
        if( FLogFile.Length() <= 0 )
        {
            FLogFile = "../log/";
            iniFile.SetValue( "main", "log_path", FLogFile );
        }        
        if( FLockFile.Length() <= 0 )
        {
            FLockFile = "./";
            iniFile.SetValue( "main", "lock_path", FLockFile );
        }        
        if( FAppName.Length() <= 0 )
        {
            FAppName = "FubonOrderConnection";
            iniFile.SetValue( "speedy", "app_name", FAppName );
        }
        if( FIP.Length() <= 0 )
        {
            FIP = "127.0.0.1";
            iniFile.SetValue( "speedy", "ip", FIP );
        }
        if( FPort <= 0 )
        {
            Value = "23456";
            FPort = Value.ToInt();
            iniFile.SetValue( "speedy", "port", Value );
        }
        if( FPassword.Length() <= 0 )
        {
            FPassword = "mdbs";
            iniFile.SetValue( "speedy", "password", FPassword );
        }
        if( FReadBufferSize <= 0 )
        {
            Value = "512";
            FReadBufferSize = Value.ToInt();
            iniFile.SetValue( "sysque", "read_buffer_size", Value );
        }
        if( FTradeTimeout <= 0 )
        {
            Value = "60";
            FTradeTimeout = Value.ToInt();
            iniFile.SetValue( "sysque", "trade_timeout", Value );
        }        

        FOrderQueName.Printf( "%s_send", FBrokerID.c_str() );
        FConfirmQueName.Printf( "%s_recv", FBrokerID.c_str() );
        FFilledQueName.Printf( "%s_recv", FBrokerID.c_str() );
        SetLogObject();                
        UFC::BufferedLog::Printf(" _________________________________________________");
        UFC::BufferedLog::Printf("                                                  ");
        UFC::BufferedLog::Printf("    Fubon Order Connection                        ");
        UFC::BufferedLog::Printf("    Copyright 2004-%d by MDBS Software Inc.     ", UFC::ThisYear());
        UFC::BufferedLog::Printf("    All right reserved.                           ");
        UFC::BufferedLog::Printf("                                                  ");
        UFC::BufferedLog::Printf("    Ver:%s Build Date:%s","1.3",__DATE__ );
        UFC::BufferedLog::Printf("                                             ");
        UFC::BufferedLog::Printf( " [ main ]" );
        UFC::BufferedLog::Printf( " debug              = [%s]", FDebug.c_str() );
        UFC::BufferedLog::Printf( " log_path           = [%s]", FLogFile.c_str() );
        UFC::BufferedLog::Printf( " lock_path          = [%s]", FLockFile.c_str() );
        UFC::BufferedLog::Printf( " SentLineNoConfigFile  = [%s]", FSentLineFileName.c_str() );
        if( FSkipESExec == TRUE )
            UFC::BufferedLog::Printf( " ExecSkipES         = [TRUE]" );            
        else
            UFC::BufferedLog::Printf( " ExecSkipES         = [FALSE]" );            
        if( FSkipExec == TRUE )
            UFC::BufferedLog::Printf( " ExecSkipAE         = [%s]", FSkipAE.c_str() );
        if( FUsePlusVK == TRUE )
            UFC::BufferedLog::Printf( " UsePlusVK          = [TRUE]");
        UFC::BufferedLog::Printf( " " );        
        UFC::BufferedLog::Printf( " [speedy]" );
        UFC::BufferedLog::Printf( " app_name           = [%s]", FAppName.c_str() );
        UFC::BufferedLog::Printf( " ip                 = [%s]", FIP.c_str() );
        UFC::BufferedLog::Printf( " port               = [%d]", FPort );
        UFC::BufferedLog::Printf( " id                 = [%s]", FID.c_str() );
//        UFC::BufferedLog::Printf( " password           = [%s]", FPassword.c_str() );        
        UFC::BufferedLog::Printf( " group              = [%d]", FGroup );        
        UFC::BufferedLog::Printf( " " );
        UFC::BufferedLog::Printf( " [sysque]" );        
        UFC::BufferedLog::Printf( " order_queue_name   = [%s]", FOrderQueName.c_str() );
        UFC::BufferedLog::Printf( " confirm_queue_name = [%s]", FConfirmQueName.c_str() );
        UFC::BufferedLog::Printf( " filled_queue_name  = [%s]", FFilledQueName.c_str() );
        UFC::BufferedLog::Printf( " read_buffer_size   = [%d]", FReadBufferSize );
        UFC::BufferedLog::Printf( " trade_timeout      = [%d]", FTradeTimeout );
        UFC::BufferedLog::Printf(" _________________________________________________");        
        
        if( FUsePlusVK == TRUE )
        {
            LoadStockData( iniFile );
            UFC::BufferedLog::Printf(" _________________________________________________");
        }
    }
    catch( UFC::Exception& ex )
    {
        UFC::BufferedLog::DebugPrintf( " %s\n", ex.what() );
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::LoadStockData( UFC::UiniFile& iniFile )
{
    UFC::AnsiString logHead = "LoadStockData";
    UFC::AnsiString SectionName, FieldName, Value;
    SectionName = "StockData";
    FieldName = "StockFormatPath";
    if( iniFile.GetValue(SectionName, "StockFormatPath", Value) == TRUE )
        gStockFormatPath = Value;
    else
        UFC::BufferedLog::Printf(" %s missing [%s]%s. Use default: %s ", logHead.c_str(), SectionName.c_str(), FieldName.c_str(), gStockFormatPath.c_str());
    FieldName = "StockT30SFormatFileName";
    if( iniFile.GetValue(SectionName, "StockT30SFormatFileName", Value) == TRUE )
        gStockT30SFormatFileName = Value;
    else
        UFC::BufferedLog::Printf(" %s missing [%s]%s. Use default: %s ", logHead.c_str(), SectionName.c_str(), FieldName.c_str(), gStockT30SFormatFileName.c_str());
    FieldName = "StockT30OFormatFileName";
    if( iniFile.GetValue(SectionName, "StockT30OFormatFileName", Value) == TRUE )
        gStockT30OFormatFileName = Value;
    else
        UFC::BufferedLog::Printf(" %s missing [%s]%s. Use default: %s ", logHead.c_str(), SectionName.c_str(), FieldName.c_str(), gStockT30OFormatFileName.c_str());
    
    gStockT30SFormatFullFileName.Printf("%s%s", gStockFormatPath.c_str(), gStockT30SFormatFileName.c_str());
    gStockT30OFormatFullFileName.Printf("%s%s", gStockFormatPath.c_str(), gStockT30OFormatFileName.c_str());
    
    FieldName = "StockDataFilePath";
    if( iniFile.GetValue(SectionName, "StockDataFilePath", Value) == TRUE )
        gStockDataFilePath = Value;
    else
        UFC::BufferedLog::Printf(" %s missing [%s]%s. Use default: %s ", logHead.c_str(), SectionName.c_str(), FieldName.c_str(), gStockDataFilePath.c_str());
    FieldName = "StockT30SDataFileName";
    if( iniFile.GetValue(SectionName, "StockT30SDataFileName", Value) == TRUE )
        gStockT30SDataFileName = Value;
    else
        UFC::BufferedLog::Printf(" %s missing [%s]%s. Use default: %s ", logHead.c_str(), SectionName.c_str(), FieldName.c_str(), gStockT30SDataFileName.c_str());
    FieldName = "StockT30ODataFileName";
    if( iniFile.GetValue(SectionName, "StockT30ODataFileName", Value) == TRUE )
        gStockT30ODataFileName = Value;
    else
        UFC::BufferedLog::Printf(" %s missing [%s]%s. Use default: %s ", logHead.c_str(), SectionName.c_str(), FieldName.c_str(), gStockT30ODataFileName.c_str());
    
    gStockT30SDataFullFileName.Printf("%s%s", gStockDataFilePath.c_str(), gStockT30SDataFileName.c_str());
    gStockT30ODataFullFileName.Printf("%s%s", gStockDataFilePath.c_str(), gStockT30ODataFileName.c_str());

    UFC::TFixedFormat   StockT30SFormat;    
    UFC::TParseData     StockT30SParser;        
    UFC::FileStreamEx   StockT30SFileStream( gStockT30SDataFullFileName, "r");

    UFC::TFixedFormat   StockT30OFormat;    
    UFC::TParseData     StockT30OParser;        
    UFC::FileStreamEx   StockT30OFileStream( gStockT30ODataFullFileName, "r");
    
    UFC::TDataset*      DatasetPtr;
    UFC::AnsiString     StockNo, UpLimitPrice;
    double              dUpLimitPrice;
    int                 RecordCount, iUpLimitPrice;
    
    StockT30SFormat.SetIniFile( gStockT30SFormatFullFileName );
    if( StockT30SFormat.IsValidStream( &StockT30SFileStream ))
    {
        UFC::BufferedLog::Printf(" ======== Add UP-LIMIT-PRICE from T30V.TSE ======== ");
        StockT30SParser.SetDataFormat( &StockT30SFormat );
        DatasetPtr = StockT30SParser.Parse( &StockT30SFileStream );
        RecordCount = DatasetPtr->GetRecordCount();
        for( int i = 0; i < RecordCount; i++ )
        {
            UFC::TRecord* curRecPtr = DatasetPtr->GetRecord( i );
            if( curRecPtr != NULL )
            {
                UFC::AnsiString value;
                curRecPtr->GetField( "COMPANY-NO", StockNo );
                StockNo.TrimRight( );
                curRecPtr->GetField( "UP-LIMIT-PRICE", value );
                UpLimitPrice.Printf("000%s00", value.c_str());
                iUpLimitPrice = value.ToInt();
                dUpLimitPrice = UFC::IntToDouble( iUpLimitPrice, gPriceDigi );  
                if( FUpLimitPriceMap.IsExists( StockNo ) )
                    UFC::BufferedLog::Printf(" ***** Duplicate Stock[%s] ***** ", StockNo.c_str());
                else{
                    UFC::BufferedLog::DebugPrintf(" Stock[%s] BullPx[%7.2lf] ", StockNo.c_str(), dUpLimitPrice );
                    FUpLimitPriceMap.Add( StockNo, UpLimitPrice );
                }
            }
        }
        UFC::BufferedLog::Printf(" ====== Add UP-LIMIT-PRICE from T30V.TSE end ====== ");
    }
    else
    {
        UFC::BufferedLog::Printf(" Warning: Invalid T30V.TSE format. ");
        UFC::BufferedLog::FlushToFile();
        exit(1);
    }
    StockT30OFormat.SetIniFile( gStockT30OFormatFullFileName );
    if( StockT30OFormat.IsValidStream( &StockT30OFileStream ))
    {
        UFC::BufferedLog::Printf(" ======== Add UP-LIMIT-PRICE from T30V.OTC ======== ");
        StockT30OParser.SetDataFormat( &StockT30OFormat );
        DatasetPtr = StockT30OParser.Parse( &StockT30OFileStream );
        RecordCount = DatasetPtr->GetRecordCount();
        for( int i = 0; i < RecordCount; i++ )
        {
            UFC::TRecord* curRecPtr = DatasetPtr->GetRecord( i );
            if( curRecPtr != NULL )
            {
                UFC::AnsiString value;
                curRecPtr->GetField( "COMPANY-NO", StockNo );
                StockNo.TrimRight();
                curRecPtr->GetField( "UP-LIMIT-PRICE", value );
                UpLimitPrice.Printf("000%s00", value.c_str());
                iUpLimitPrice = value.ToInt();
                dUpLimitPrice = UFC::IntToDouble( iUpLimitPrice, gPriceDigi );  
                if( FUpLimitPriceMap.IsExists( StockNo ) )
                    UFC::BufferedLog::Printf(" ***** Duplicate Stock[%s] ***** ", StockNo.c_str());
                else{
                    UFC::BufferedLog::DebugPrintf(" Stock[%s] BullPx[%7.2lf] ", StockNo.c_str(), dUpLimitPrice );
                    FUpLimitPriceMap.Add( StockNo, UpLimitPrice );
                }   
            }
        }
        UFC::BufferedLog::Printf(" ====== Add UP-LIMIT-PRICE from T30V.OTC end ====== ");
    }
    else
    {
        UFC::BufferedLog::Printf(" Warning: Invalid T30V.OTC format. ");
        UFC::BufferedLog::FlushToFile();
        exit(1);
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnConnected( void )
{
//    UFC::BufferedLog::Printf( " logon( id=[%s], password=[%s] )", FID.c_str(), FPassword.c_str() );
    UFC::BufferedLog::Printf( " logon( id=[%s] )", FID.c_str() );
    SetConnectionState( focConnected );    
    FConnection->Logon( FID.c_str(), FPassword.c_str(), "0000000" );
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *Connection broken, Wait for 2 sec to reconnect." );    
    SetConnectionState( focDisconnect );
    UFC::SleepMS( 2000 );
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnLogonReply( const char* ReplyString,  LogonResult Result, int CID )
{
    if( Result == lrOk )
    {
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );        
        UFC::AnsiString BeginTime;
        UFC::UTime      RecoverTime; ///< Create last time.
        Int32           LastTime = FRecoverBeginTime.ToInt32(); 
        
        RecoverTime.setHour( LastTime / 10000 );
        RecoverTime.setMinute( (LastTime % 10000) / 100 );
        RecoverTime.setSecond(  LastTime % 100 );
        RecoverTime -= FOverlapTime;
        LastTime =  (RecoverTime.getHour()*10000) + (RecoverTime.getMinute()* 100) + RecoverTime.getSecond();
        
        BeginTime.Printf( "%06d",  LastTime );        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, TSE & OTC  )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmTSE );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmOTC );
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::Recover( UFC::AnsiString BeginTime )
{
    if( GetConnectionState() == focReady )
    {
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmTSE );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmOTC );
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnRecoverFinished( int Count )
{
    FRecoverCount++;
    UFC::BufferedLog::DebugPrintf( " TFubonTAIFEXConnection::OnRecoverFinished( Count = %d )", Count );
    
    if( FRecoverCount == 2 )
    {
        SetConnectionState( focReady );        
        UFC::BufferedLog::DebugPrintf( " Ready to place order!");        
        FRecoverCount = 0;
        UFC::SleepMS( 1000 );
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
    if( FSkipESExec == TRUE &&  Msg->GetMarket() == nsOrderMessageDefine::mES )
    {
        UFC::BufferedLog::DebugPrintf( " Skip ES Market Exec[%s]",  Msg->GetSrc() );    
        return;        
    }
    if( FSkipExec == TRUE && FSkipAE == Msg->GetAE() )
    {
        UFC::BufferedLog::DebugPrintf( " Skip AE[%s] Exec[%s]", Msg->GetAE(), Msg->GetSrc() );    
        return;
    }
    
    UFC::AnsiString rStatusCode = Msg->GetStatusCode();
    rStatusCode.TrimRight();
    // Lock AE  = 91
    if( FUsePlusVK == TRUE && (rStatusCode == "97" || rStatusCode == "91" ) )
    {
        UFC::BufferedLog::DebugPrintf(" Skip Rejected OrderID[%s] Exec[%s]", Msg->GetOrderID(), Msg->GetSrc());
        return;
    }
    
    UFC::PLockObject LockObject( FExecutionCS );
    
    UFC::BufferedLog::DebugPrintf( " TFubonOrderConnection::OnExecutionReport" );    
    if( Msg->GetTradingSessionID() == nsOrderMessageDefine::tsNegotiatePx )
    {
        OnNegotiatePxConfirm( Msg );
        return;
    }
    TSQMessage SQMessage;
    
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osPendingNew:        		
            SQMessage.Append( "00", "2" );
            SQMessage.Append( "14", "I" );
            SQMessage.Append( "19", "S" );
            break;
        case nsOrderMessageDefine::osPendingReplace:
            SQMessage.Append( "00", "2" );
            if( Msg->IsReplacePx() == true )
                SQMessage.Append( "14", "P" ); ///< Replace price
            else
                SQMessage.Append( "14", "C" ); ///< Reduce qty
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osPendingCancel:
            SQMessage.Append( "00", "2" );
            SQMessage.Append( "14", "D" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osNew:
            if( Msg->GetExecType( ) == nsOrderMessageDefine::etOrderStatus )
                SQMessage.Append( "00", "9" );
            else
                SQMessage.Append( "00", "3" );        	
            SQMessage.Append( "14", "I" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osReplaced:
            SQMessage.Append( "00", "3" );
            if( Msg->IsReplacePx() == true )
                SQMessage.Append( "14", "P" ); ///< Replace price
            else
                SQMessage.Append( "14", "C" ); ///< Reduce qty
//            SQMessage.Append( "14", "C" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osCanceled:
            SQMessage.Append( "00", "3" );
            SQMessage.Append( "14", "D" );
            SQMessage.Append( "19", "R" );
            break;
        case nsOrderMessageDefine::osPartiallyFilled:
            SQMessage.Append( "00", "4" );
            SQMessage.Append( "14", "I" );
            break;
        case nsOrderMessageDefine::osFilled:
            SQMessage.Append( "00", "4" );
            SQMessage.Append( "14", "I" );
            break;
        case nsOrderMessageDefine::osRejected:
            switch( Msg->GetCxlRejResponseTo() )
            {
                case nsOrderMessageDefine::crrNew:
                    SQMessage.Append( "00", "3" );
                    SQMessage.Append( "14", "I" );
                    SQMessage.Append( "19", "E" );
                    break;
                case nsOrderMessageDefine::crrReplace:
                    SQMessage.Append( "00", "3" );
                    if( Msg->IsReplacePx() == true )
                        SQMessage.Append( "14", "P" ); ///< Replace price
                    else
                        SQMessage.Append( "14", "C" ); ///< Reduce qty
                    SQMessage.Append( "19", "E" );
                    break;
                case nsOrderMessageDefine::crrCancel:
                    SQMessage.Append( "00", "3" );
                    SQMessage.Append( "14", "D" );
                    SQMessage.Append( "19", "E" );
                    break;
                case nsOrderMessageDefine::crrOrderStatus:
                    SQMessage.Append( "00", "9" );
                    SQMessage.Append( "14", "I" );
                    SQMessage.Append( "19", "E" );
                    break;
                default: break;
            }
            break;
        default: break;
    }

    UFC::AnsiString BrokerID( Msg->GetBrokerID() );
    if( BrokerID.Length() == 4 )
    {
        UFC::AnsiString CompanyID = BrokerID.SubString( 0, 2 );
        UFC::AnsiString BranchID = BrokerID.SubString( 2, 2 );
        SQMessage.Append( "08", CompanyID );
        SQMessage.Append( "09", BranchID );
    }

    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTSE:
            SQMessage.Append( "13", "T" );
            break;
        case nsOrderMessageDefine::mOTC:
            SQMessage.Append( "13", "O" );
            break;
        default:
        	break;
    }

    UFC::AnsiString Account( Msg->GetAccount() );
    UFC::AnsiString AccountCode, AccountChecksum;
    
    AccountCode.Printf( "%06d", Account.ToInt() / 10 );
    AccountChecksum.Printf( "%d", Account.ToInt() % 10 );    
    SQMessage.Append( "17", AccountCode );
    SQMessage.Append( "18", AccountChecksum );
    SQMessage.Append( "27", Msg->GetTSEOrderType() );
    SQMessage.Append( "29", Msg->GetSymbol() );
    SQMessage.Append( "32", (Msg->GetSide() == nsOrderMessageDefine::sBuy) ? "B" : "S" );
    SQMessage.Append( "36", Msg->GetOrderID() );
    SQMessage.Append( "40", Msg->GetStatusCode() );
    switch( Msg->GetTradingSessionID() )
    {
        case nsOrderMessageDefine::tsNormal:
            SQMessage.Append( "O3", "0" );
            break;
        case nsOrderMessageDefine::tsOddLot:
            SQMessage.Append( "O3", "2" );
            break;
        case nsOrderMessageDefine::tsOffHour:
            SQMessage.Append( "O3", "3" );
            break;
        case nsOrderMessageDefine::tsAuction:
            SQMessage.Append( "O3", "4" );
            break;
        case nsOrderMessageDefine::tsLend:
            SQMessage.Append( "O3", "5" );
            break;
        case nsOrderMessageDefine::tsTender:
            SQMessage.Append( "O3", "6" );
            break;
        case nsOrderMessageDefine::tsTenderEx:
            SQMessage.Append( "O3", "7" );
            break;
        case nsOrderMessageDefine::tsIntradayOdd:
            SQMessage.Append( "O3", "9" );
            break;
        default:
            SQMessage.Append( "O3", "0" );
            break;            
    }      
    UFC::AnsiString PBNO( Msg->GetLINBRN() );
    UFC::AnsiString PVC( Msg->GetPVC() );
    UFC::AnsiString PHost( Msg->GetProcessHost() );
    UFC::AnsiString LineNo,LKey;
    UFC::AnsiString Data( Msg->GetData());
    
    ///< Data attach to the order.
    if( FUsePlusVK == FALSE )
    {    
        if( Data.Length() > 0 ) 
            SQMessage.Append( "VK", Data.c_str() );
    }
    else
    {
        //------- Edit by Chelsea 20250415 -------
        UFC::AnsiString OrderID = Msg->GetOrderID();
        UFC::AnsiString OrderData = "";
        UFC::AnsiString sPrice = "";
        nsOrderMessageDefine::OrderStatusEnum rptOrderStatus = Msg->GetOrderStatus();

        if( rptOrderStatus == nsOrderMessageDefine::osPartiallyFilled || rptOrderStatus == nsOrderMessageDefine::osFilled )
        {
            if( FUserDataMap.IsExists( OrderID ) == TRUE )
            {
                if( FUserDataMap.GetObjectByKey( OrderID, OrderData ) )
                    SQMessage.Append( "VK", OrderData.c_str() ); 
            }
            else
                UFC::BufferedLog::Printf(" OrderID[%s] cannot find Original Order Userdata. ", OrderID.c_str() );
        }
        else
        {
            MsgUserData* UserData = new MsgUserData();
            UserData->CreateUserData( Msg );
            if( FSentLineNoTable->GetValue( PHost, LKey, LineNo ) )
                UserData->SetLineNo( LineNo );
            else
                UserData->SetLineNo( "XX" );

            if( Msg->GetOrderType() == nsOrderMessageDefine::otMarket )
            {
                if( rptOrderStatus == nsOrderMessageDefine::osNew || rptOrderStatus == nsOrderMessageDefine::osCanceled || 
                    rptOrderStatus == nsOrderMessageDefine::osReplaced || rptOrderStatus == nsOrderMessageDefine::osRejected )
                {
                    UFC::AnsiString StockNo = Msg->GetSymbol();
                    StockNo.TrimRight();
                    if( FUpLimitPriceMap.IsExists( StockNo ) )
                    {
                        if( FUpLimitPriceMap.GetObjectByKey( StockNo, sPrice ) )
                        {
                            UserData->SetPrice( sPrice );
                        }
                    }
                    else
                        UFC::BufferedLog::Printf(" StockNo[%s] not exist in T30. ", StockNo.c_str());
                }
            }
            else if( Msg->GetOrderType() == nsOrderMessageDefine::otLimit )
            {    
                if( FOrigPriceMap.IsExists( OrderID ) == TRUE )
                {
                    if( rptOrderStatus == nsOrderMessageDefine::osReplaced && Msg->IsReplacePx() == true )
                    {
                        if( FOrigPriceMap.GetObjectByKey( OrderID, sPrice ) )
                        {
                            UserData->SetPrice( sPrice );
                            UFC::AnsiString replacePx;
                            UserData->PriceToString( replacePx, Msg->GetPrice() );
                            FOrigPriceMap.SetObjectByKey( OrderID, replacePx );
                        }
                    }
                }
                else
                {
                    // New Order
                    UserData->PriceToString( sPrice, Msg->GetPrice() );
                    FOrigPriceMap.Add( OrderID, sPrice );
                }
            }
            UserData->ToString( OrderData );
            if( FUserDataMap.IsExists( OrderID ) == FALSE )
                FUserDataMap.Add( OrderID, OrderData );
            delete UserData;
            SQMessage.Append( "VK", OrderData.c_str() );
        }
    }
    //---------------------------------------
    ///< PVC ID processed this order.
    if( PVC.Length() > 0 ) 
      	SQMessage.Append( "VI", PVC.c_str() );
    else
        SQMessage.Append( "VI", "xx" );    	    
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTSE:
            LKey.Printf( "%s_T", PBNO.c_str() );
            break;
        case nsOrderMessageDefine::mOTC:
            LKey.Printf( "%s_O", PBNO.c_str() );
            break;
        default:
            LKey.Printf( "%s", PBNO.c_str() );
            break;
    }    
    ///< Line No processed this order.
    UFC::BufferedLog::DebugPrintf( " Exec from Host[%s] Broker[%s] Key[%s].", PHost.c_str(), PBNO.c_str(), LKey.c_str() );            
    if( FSentLineNoTable->GetValue( PHost, LKey, LineNo ) )
        SQMessage.Append( "VH", LineNo.c_str() );
    else
        SQMessage.Append( "VH", "XX" );            

    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:
             OnFilled( Msg, SQMessage );
             break;
        default:
             OnConfirm( Msg, SQMessage );
             break;
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnNegotiatePxConfirm( TExecutionReportMessage* Msg )
{
    TSQMessage      SQMessage;
    UFC::UTime      CurrentTime;
    UFC::AnsiString Data( Msg->GetData() );   
    UFC::AnsiString CompanyID = Data.SubString( 0, 2 );
    UFC::AnsiString BranchID = Data.SubString( 2, 2 );
    UFC::AnsiString UniqueKey( Data.c_str() + 4 );   
    UFC::AnsiString PBNO( Msg->GetLINBRN() );
    UFC::AnsiString PVC( Msg->GetPVC() );
    UFC::AnsiString PHost( Msg->GetProcessHost() );
    UFC::AnsiString LineNo,TimeString;
    UFC::AnsiString LKey;
    
    TimeString.Printf( "%02d%02d%02d%03d", CurrentTime.getHour(),  CurrentTime.getMinute(),  CurrentTime.getSecond(),  CurrentTime.getMillisecond() );
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::osPendingNew:        		
            SQMessage.Append( "00", "2" );            
            SQMessage.Append( "14", "I" );
            SQMessage.Append( "19", "S" );
            break;    
        case nsOrderMessageDefine::osNew:
            SQMessage.Append( "00", "3" );        	
            
            SQMessage.Append( "14", "I" );
            SQMessage.Append( "19", "R" );
            break;
        default: break;
    }
    SQMessage.Append( "08", CompanyID );
    SQMessage.Append( "09", BranchID );
    SQMessage.Append( "13", "O" ); ///< Market, always OTC        
    SQMessage.Append( "21", FToday );
    SQMessage.Append( "22", TimeString.c_str() );    
    SQMessage.Append( "24", "Q" );
    SQMessage.Append( "O3", "8" ); ///< Trade Session, 8 for Negotiate Price   
    ///< PVC ID processed this order.
    if( PVC.Length() > 0 ) 
      	SQMessage.Append( "VI", PVC.c_str() );
    else
        SQMessage.Append( "VI", "xx" );    	
    ///< Line No processed this order.
    switch( Msg->GetMarket() )
    {
        case nsOrderMessageDefine::mTSE:
            LKey.Printf( "%s_T", PBNO.c_str() );
            break;
        case nsOrderMessageDefine::mOTC:
            LKey.Printf( "%s_O", PBNO.c_str() );
            break;
        default:
            LKey.Printf( "%s", PBNO.c_str() );
            break;
    }    
    if( FSentLineNoTable->GetValue( PHost, LKey, LineNo ) )
        SQMessage.Append( "VH", LineNo.c_str() );
    else
        SQMessage.Append( "VH", "XX" );            
    SQMessage.Append( "VL", Msg->GetSrc() );        
    SQMessage.Append( "VM", UniqueKey );        
    UFC::AnsiString MessageString = SQMessage.ToString();
    FConfirmQueue->Write( (char*)(MessageString.c_str()), MessageString.Length() );    
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnConfirm( TExecutionReportMessage* Msg, TSQMessage& SQMessage )
{
    SQMessage.Append( "21", FToday );
    UFC::AnsiString TimeString( Msg->GetTransactTime());    
    
    int ErrorCode = UFC::AnsiString( Msg->GetStatusCode() ).ToInt();
    
    if( FConnection->IsTWSENewVersion() == false )
    {
        if( TimeString.Length() >= 6 )
            SQMessage.Append( "22", TimeString.c_str() );
        else
            SQMessage.Append( "22", UFC::AnsiString( Msg->GetSrc() ).SubString( 6, 6 ) );
    }
    else
    {
        if( TimeString.Length() < 8 )
        {
            TimeString = UFC::AnsiString( Msg->GetSrc()).SubString( 6, 6 );
            TimeString.PadThis( 8, '0' );
        }   
        SQMessage.Append( "22", TimeString.c_str() );
    }    
    if( Msg->GetOrderStatus() == nsOrderMessageDefine::osRejected && ErrorCode == 89 )
        SQMessage.Append( "24", "q" );
    else
        SQMessage.Append( "24", "Q" );
    if( Msg->GetOrderType( ) == nsOrderMessageDefine::otMarket )
        SQMessage.Append( "OG", "1" );
    else
        SQMessage.Append( "OG", "2" );
    if( Msg->GetTimeInForce( ) == nsOrderMessageDefine::tifIOC )
        SQMessage.Append( "TF", "3" );
    else if( Msg->GetTimeInForce( ) == nsOrderMessageDefine::tifFOK )
        SQMessage.Append( "TF", "4" );
    else
        SQMessage.Append( "TF", "0" );
    switch( Msg->GetTradingSessionID() )
    {
        case nsOrderMessageDefine::tsNormal:
        case nsOrderMessageDefine::tsOffHour:
        case nsOrderMessageDefine::tsAuction:
        case nsOrderMessageDefine::tsLend:
        case nsOrderMessageDefine::tsTender:
        case nsOrderMessageDefine::tsTenderEx:
        case nsOrderMessageDefine::tsIntradayOdd:
            SQMessage.Append( "30", Msg->GetOrderQty()  );
            SQMessage.Append( "42", Msg->GetBeforeQty() );
            SQMessage.Append( "43", Msg->GetAfterQty()  );
            break;
        case nsOrderMessageDefine::tsOddLot:
            SQMessage.Append( "30", Msg->GetOrderQty() );
            SQMessage.Append( "42", Msg->GetBeforeQty() );
            SQMessage.Append( "43", Msg->GetAfterQty() );
            SQMessage.Append( "EO", Msg->GetStockSeqNo() );
            break;
       default:
            return;           
    }
    if( FUsePlusVK == TRUE )
        SQMessage.Append( "P2", " " );
    else
        SQMessage.Append( "P2", Msg->GetAccountFlag());    
    SQMessage.Append( "31", Msg->GetPrice() );
    UFC::AnsiString MessageString = SQMessage.ToString();
    FConfirmQueue->Write( (char*)(MessageString.c_str()), MessageString.Length() );
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnFilled( TExecutionReportMessage* Msg, TSQMessage& SQMessage )
{
    switch( Msg->GetTradingSessionID() )
    {
        case nsOrderMessageDefine::tsNormal:
        case nsOrderMessageDefine::tsOffHour:
        case nsOrderMessageDefine::tsIntradayOdd:
             SQMessage.Append( "47", Msg->GetOrderQty() );
             break;
        case nsOrderMessageDefine::tsOddLot:
             SQMessage.Append( "47", Msg->GetOrderQty() );
             break;
        default:
        	break;
    }
    SQMessage.Append( "48", Msg->GetPrice() );
    SQMessage.Append( "50", FToday );
    SQMessage.Append( "51", Msg->GetTransactTime() );
    SQMessage.Append( "72", Msg->GetRecNo() );    
    SQMessage.Append( "24", "Q" );    
    UFC::AnsiString ExecID( Msg->GetExecID() );
    if( ExecID.Length() > 6  )
    	SQMessage.Append( "Q5", ExecID.SubString( ExecID.Length() - 6, 6 ).c_str() );
    else
    	SQMessage.Append( "Q5", ExecID.c_str() );    
    UFC::AnsiString MessageString = SQMessage.ToString();
    FFilledQueue->Write( (char*)(MessageString.c_str()), MessageString.Length() );
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnData( void )
{
    TSQMessage SQMessage;
    UFC::AnsiString MessageString( FReadBuffer );
    SQMessage.FromString( MessageString );
    
    if( CheckTradeDate(SQMessage) )
    {
        if( CheckTradeTime(SQMessage) )
        {
            UFC::AnsiString OrderType,Session;
            
            if( SQMessage.Get( "14", OrderType ) && SQMessage.Get( "O3", Session )) 
            {
                if( Session == "8" ) ///< Negotiate Price
                    NegotiatePx( SQMessage );
                else if( OrderType == "I" ) ///< New Order
                    New( SQMessage );
                else if( OrderType == "C" ) ///< Reduce qty
                    Replace( SQMessage, FALSE );
                else if( OrderType == "D" ) ///< Cancel
                    Cancel( SQMessage );
                else if( OrderType == "Q" )                    
                    OrderStatusRequest( SQMessage );
                else if( OrderType == "P" )
                    Replace( SQMessage, TRUE );
            }
        }
        else
        {
            RejectOvertimeOrder( SQMessage );
            UFC::BufferedLog::Printf( " Order expired, Message:[%s].", MessageString.c_str() );
        }
    }
    else
    {
        RejectOvertimeOrder( SQMessage );
        UFC::BufferedLog::Printf( " Wrong trade date,Message:[%s].", MessageString.c_str() );
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::NegotiatePx( TSQMessage& SQMessage )
{
    TNewOrderMessage Msg;
    UFC::AnsiString CompanyID, BranchID, Body, UniqueKey, Data;
    
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "VL", Body ) &&
        SQMessage.Get( "VM", UniqueKey ) )
    {
        Data.Printf( "%s%s%s",CompanyID.c_str(),BranchID.c_str(),UniqueKey.c_str() ); 
        FConnection->NegotiatePriceMessage( Body, Data );
    }
}            
//------------------------------------------------------------------------------
void TFubonOrderConnection::New( TSQMessage& SQMessage )
{
    TNewOrderMessage Msg;
    UFC::AnsiString CompanyID, BranchID, Market, AccountCode, AccountChecksum, TradeDate, Symbol, Quantity, Price, Side, OrderID, OrderType, fflag;
    
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "13", Market ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "21", TradeDate ) &&        
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "30", Quantity ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) &&
        SQMessage.Get( "O3", OrderType ) ) 
    {
        UFC::AnsiString BrokerID, Account, TSEOrderType, StockSeqNo, Keep, Data, PriceType, TimeInForce;
        
        
        BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );
        
        Msg.SetGroup( FGroup );
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );
        Msg.SetMarket( (Market == "T") ? nsOrderMessageDefine::mTSE : nsOrderMessageDefine::mOTC );
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetPrice( Price.ToDouble() );        
        if( SQMessage.Get( "27", TSEOrderType ) )
            Msg.SetTSEOrderType( TSEOrderType );
        else
            Msg.SetTSEOrderType( "0" );        
        if( SQMessage.Get( "OG", PriceType ))
        {
            if( PriceType == "1") 
                Msg.SetOrderType( nsOrderMessageDefine::otMarket ); 
            else    
                Msg.SetOrderType( nsOrderMessageDefine::otLimit ); 
        }
        else
            Msg.SetOrderType( nsOrderMessageDefine::otLimit ); 
        if( SQMessage.Get( "TF", TimeInForce ))
        {
            if( TimeInForce == "3" )
                Msg.SetTimeInForce( nsOrderMessageDefine::tifIOC );  
            else if( TimeInForce == "4" )       
                Msg.SetTimeInForce( nsOrderMessageDefine::tifFOK );
            else
                Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );
        }
        else
            Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );
        
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );        
        if( SQMessage.Get( "P2",fflag))
            Msg.SetAccountFlag( fflag );
        else
            Msg.SetAccountFlag( " " );
        switch( OrderType.ToInt() ) 
        {
            case 0:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
                Msg.SetOrderQty( Quantity.ToInt() );
                break;
            case 2:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsOddLot );
                Msg.SetOrderQty( Quantity.ToInt() );
                break;            
            case 3:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsOffHour );
                Msg.SetOrderQty( Quantity.ToInt() );
                break;
            case 4:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuction );
                Msg.SetOrderQty( Quantity.ToInt() );
                break;
            case 5:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsLend );
                Msg.SetOrderQty( Quantity.ToInt() );
                break;
            case 6:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsTender );
                Msg.SetOrderQty( Quantity.ToInt() );
                break;
            case 7:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsTenderEx );
                Msg.SetOrderQty( Quantity.ToInt()  );
                break;
            case 9:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsIntradayOdd );
                Msg.SetOrderQty( Quantity.ToInt() );
                break;
        }        
        if( SQMessage.Get( "EN", Keep ) )
            Msg.SetKeep( Keep );        
        if( SQMessage.Get( "EO", StockSeqNo ) )
            Msg.SetStockSeqNo( StockSeqNo.ToInt() );        
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );        
        FConnection->NewOrder( &Msg );
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::Replace( TSQMessage& SQMessage, BOOL ReplacePx )
{
    UFC::BufferedLog::DebugPrintf( " TFubonOrderConnection::Replace" );
    
    TReplaceOrderMessage Msg;
    UFC::AnsiString CompanyID, BranchID, Market, AccountCode, AccountChecksum, TradeDate, Symbol, Quantity, Price, Side, OrderID, OrderType, TSEOrderType,fflag;
    
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "13", Market ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "21", TradeDate ) &&
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "30", Quantity ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) &&
        SQMessage.Get( "O3", OrderType ) ) 
    {
        UFC::AnsiString BrokerID, Account, StockSeqNo, Keep, Data, PriceType, TimeInForce;
        BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );
        
        Msg.SetGroup( FGroup );
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );
        Msg.SetMarket( (Market == "T") ? nsOrderMessageDefine::mTSE : nsOrderMessageDefine::mOTC );
        Msg.SetSymbol( Symbol.c_str() );
        //if( ReplacePx == TRUE )
        //    Msg.SetPrice( Price.ToDouble() ); ///< Replace price
       // else
        //    Msg.SetPrice( 0.0 );              ///< Replace Qty, Set price to 0.
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );        
        if( SQMessage.Get( "P2",fflag))
            Msg.SetAccountFlag( fflag );
        else
            Msg.SetAccountFlag( " " );
        if( SQMessage.Get( "27", TSEOrderType ) )
            Msg.SetTSEOrderType( TSEOrderType );
        else
            Msg.SetTSEOrderType( "0" );        
        if( SQMessage.Get( "OG", PriceType ))
        {
            if( PriceType == "1") 
                Msg.SetOrderType( nsOrderMessageDefine::otMarket ); 
            else    
                Msg.SetOrderType( nsOrderMessageDefine::otLimit ); 
        }
        else
            Msg.SetOrderType( nsOrderMessageDefine::otLimit ); 
        if( SQMessage.Get( "TF", TimeInForce ))
        {
            if( TimeInForce == "3" )
                Msg.SetTimeInForce( nsOrderMessageDefine::tifIOC );  
            else if( TimeInForce == "4" )       
                Msg.SetTimeInForce( nsOrderMessageDefine::tifFOK );
            else
                Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );
        }
        else
            Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );
        switch( OrderType.ToInt() ) 
        {
            case 0:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
                if( ReplacePx == TRUE )
                    Msg.SetOrderQty( 0 ); ///< Replace price. set qty to 0
                else
                    Msg.SetOrderQty( Quantity.ToInt() ); ///< Replace qty
                Msg.SetPrice( Price.ToDouble() ); ///< Replace price
                break;
            case 2:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsOddLot );
                Msg.SetOrderQty( Quantity.ToInt() );
                Msg.SetPrice( Price.ToDouble() ); ///< Replace Qty
                break;
            case 3:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsOffHour );
                Msg.SetOrderQty( Quantity.ToInt() );
                Msg.SetPrice( Price.ToDouble() ); ///< Replace Qty
                break;
            case 4:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuction );
                Msg.SetOrderQty( Quantity.ToInt() );
                Msg.SetPrice( Price.ToDouble() ); ///< Replace Qty
                break;
            case 5:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsLend );
                Msg.SetOrderQty( Quantity.ToInt() );
                Msg.SetPrice( Price.ToDouble() ); ///< Replace Qty
                break;
            case 6:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsTender );
                Msg.SetOrderQty( Quantity.ToInt() );
                Msg.SetPrice( Price.ToDouble() ); ///< Replace Qty
                break;
            case 7:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsTenderEx );
                Msg.SetOrderQty( Quantity.ToInt() );
                Msg.SetPrice( Price.ToDouble() ); ///< Replace Qty
                break;
            case 9:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsIntradayOdd );
                Msg.SetOrderQty( Quantity.ToInt() );
                Msg.SetPrice( Price.ToDouble() ); ///< Replace Qty
                break;    
                
        }        
        if( SQMessage.Get( "EN", Keep ) )
            Msg.SetKeep( Keep );        
        if( SQMessage.Get( "EO", StockSeqNo ) )
            Msg.SetStockSeqNo( StockSeqNo.ToInt() );        
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );        
        FConnection->ReplaceOrder( &Msg );
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::Cancel( TSQMessage& SQMessage )
{
    TCancelOrderMessage Msg;
    UFC::AnsiString CompanyID, BranchID, Market, AccountCode, AccountChecksum, TradeDate, Symbol, Price, Side, OrderID, OrderType, TSEOrderType,fflag;
    
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "13", Market ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "21", TradeDate ) &&
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) &&
        SQMessage.Get( "O3", OrderType ) ) 
    {
        UFC::AnsiString BrokerID, Account, StockSeqNo, Keep, Data, PriceType, TimeInForce;
        BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );
        
        Msg.SetGroup( FGroup );
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );
        Msg.SetMarket( (Market == "T") ? nsOrderMessageDefine::mTSE : nsOrderMessageDefine::mOTC );
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetPrice( Price.ToDouble() );
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );        
        if( SQMessage.Get( "P2",fflag))
            Msg.SetAccountFlag( fflag );
        else
            Msg.SetAccountFlag( " " );
        if( SQMessage.Get( "27", TSEOrderType ) )
            Msg.SetTSEOrderType( TSEOrderType );
        else
            Msg.SetTSEOrderType( "0" );     
        if( SQMessage.Get( "OG", PriceType ))
        {
            if( PriceType == "1") 
                Msg.SetOrderType( nsOrderMessageDefine::otMarket ); 
            else    
                Msg.SetOrderType( nsOrderMessageDefine::otLimit ); 
        }
        else
            Msg.SetOrderType( nsOrderMessageDefine::otLimit ); 
        if( SQMessage.Get( "TF", TimeInForce ))
        {
            if( TimeInForce == "3" )
                Msg.SetTimeInForce( nsOrderMessageDefine::tifIOC );  
            else if( TimeInForce == "4" )       
                Msg.SetTimeInForce( nsOrderMessageDefine::tifFOK );
            else
                Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );
        }
        else
            Msg.SetTimeInForce( nsOrderMessageDefine::tifROD );
        switch( OrderType.ToInt() ) 
        {
            case 0:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
                break;
            case 2:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsOddLot );
                break;
            case 3:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsOffHour );
                break;
            case 4:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuction );
                break;
            case 5:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsLend );
                break;
            case 6:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsTender );
                break;
            case 7:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsTenderEx );
                break;
            case 9:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsIntradayOdd );
                break;
        }        
        if( SQMessage.Get( "EN", Keep ) )
            Msg.SetKeep( Keep.c_str() );        
        if( SQMessage.Get( "EO", StockSeqNo ) )
            Msg.SetStockSeqNo( StockSeqNo.ToInt() );        
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );        
        FConnection->CancelOrder( &Msg );
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OrderStatusRequest( TSQMessage& SQMessage )
{
    TOrderStatusRequest Msg;
    UFC::AnsiString CompanyID, BranchID, Market, AccountCode, AccountChecksum, TSEOrderType, Symbol, Price, Side, OrderID, OrderType,fflag;
    
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "13", Market ) &&
        SQMessage.Get( "17", AccountCode ) &&
        SQMessage.Get( "18", AccountChecksum ) &&
        SQMessage.Get( "27", TSEOrderType ) &&
        SQMessage.Get( "29", Symbol ) &&
        SQMessage.Get( "31", Price ) &&
        SQMessage.Get( "32", Side ) &&
        SQMessage.Get( "36", OrderID ) &&
        SQMessage.Get( "O3", OrderType ) ) 
    {
        UFC::AnsiString BrokerID, Account, StockSeqNo, Keep, Data;
        BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Account.Printf( "%s%s", AccountCode.c_str(), AccountChecksum.c_str() );
        
        Msg.SetBrokerID( BrokerID.c_str() );
        Msg.SetAccount( Account.c_str() );
        Msg.SetMarket( (Market == "T") ? nsOrderMessageDefine::mTSE : nsOrderMessageDefine::mOTC );
        Msg.SetSymbol( Symbol.c_str() );
        Msg.SetPrice( Price.ToDouble() );
        Msg.SetSide( (Side == "B") ? nsOrderMessageDefine::sBuy : nsOrderMessageDefine::sSell );
        Msg.SetOrderID( OrderID.c_str() );
        UFC::BufferedLog::Printf( " Ask Order Status for:%s", OrderID.c_str() );
        if( SQMessage.Get( "P2",fflag))
            Msg.SetAccountFlag( fflag );
        else
            Msg.SetAccountFlag( " " );
        if( SQMessage.Get( "27", TSEOrderType ) )
            Msg.SetTSEOrderType( TSEOrderType );
        else
            Msg.SetTSEOrderType( "0" );        

        switch( OrderType.ToInt() ) 
        {
            case 0:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsNormal );
                break;
            case 2:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsOddLot );
                break;
            case 3:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsOffHour );
                break;
            case 4:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsAuction );
                break;
            case 5:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsLend );
                break;
            case 6:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsTender );
                break;
            case 7:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsTenderEx );
                break;
            case 9:
                Msg.SetTradingSessionID( nsOrderMessageDefine::tsIntradayOdd );
                break;                    
        }        
        if( SQMessage.Get( "EN", Keep ) )
            Msg.SetKeep( Keep.c_str() );        
        if( SQMessage.Get( "EO", StockSeqNo ) )
            Msg.SetStockSeqNo( StockSeqNo.ToInt() );        
        if( SQMessage.Get( "VK", Data ) )
            Msg.SetData( Data.c_str() );        
        UFC::BufferedLog::Printf( " Ask Order Status for:%s", Msg.GetOrderID() );
        FConnection->OrderStatusRequest( &Msg );
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::SetConnectionState( TFubonOrderConnectionStateEnum ConnectionState )
{
    UFC::PLockObject LockObject( FConnectionStateCS );
    FConnectionState = ConnectionState;
}
//------------------------------------------------------------------------------
TFubonOrderConnectionStateEnum TFubonOrderConnection::GetConnectionState( void )
{
    UFC::PLockObject LockObject( FConnectionStateCS );
    return FConnectionState;
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::CheckConnectionState( void )
{
    if( GetConnectionState() == focDisconnect ) 
    {
        UFC::AnsiString* IP = FIPPool.DequeueNoWait();
        
        while( IP == NULL )
        {
            SetConnectionState( focDisconnect );
            UFC::PStringList IPList;

            IPList.SetStrings( FIP, ", " );
            for( int i = 0; i < IPList.ItemCount(); i++ ) 
            {
                IP = new UFC::AnsiString( IPList[i] );
                UFC::BufferedLog::Printf( " FIPPool.InqueueNoWait( %s )", IP->c_str() );
                FIPPool.InqueueNoWait( IP );
            }            
            IP = FIPPool.DequeueNoWait();
        }
        UFC::BufferedLog::Printf( " Try to connect to IP[%s,%d]", IP->c_str(), FPort );
        UFC::BufferedLog::Printf( " FConnection->Connect( %s, %d )", IP->c_str(), FPort );
        FConnection->Connect( IP->c_str(), FPort, 10 );
        delete IP;        
    }    
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::Execute( void )
{
    UFC::BufferedLog::DebugPrintf( " TFubonOrderConnection::Execute" );

    while( !IsTerminated() )
    {
        CheckConnectionState();
        if( GetConnectionState() == focReady ) 
        {
            Dequeue();
            UFC::SleepMS( 5 );
        }
        else
            sleep( 10 ); ///< sleep 10 sec
    }    
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::Dequeue( void )
{
    int ReadSize;
    
    while( GetConnectionState() == focReady )
    {
        ReadSize = FOrderQueue->Read( FReadBuffer, FReadBufferSize );
        if( ReadSize > 0 )
        {
            FReadBuffer[ReadSize] = 0x00;
            OnData();
        }
        else
            return;
    }
}
//------------------------------------------------------------------------------
BOOL TFubonOrderConnection::CheckTradeDate( TSQMessage& SQMessage )
{
    UFC::AnsiString TradeDate;
    
    if( SQMessage.Get( "21", TradeDate ) )
    {				
        if( FToday.ToInt() == TradeDate.ToInt() )
            return TRUE;
        UFC::BufferedLog::Printf( " Reject order, Tag 21[%s] is not today[%s].", TradeDate.c_str(), FToday.c_str() );
    }	
    else
        UFC::BufferedLog::Printf( " Miss tag 21 (Trade date)." );
    return FALSE;    
}
//------------------------------------------------------------------------------
BOOL TFubonOrderConnection::CheckTradeTime( TSQMessage& SQMessage )
{
    UFC::AnsiString TradeTime;
    
    if( SQMessage.Get( "22", TradeTime ) ) 
    {
        TradeTime.PadThis( 6, '0' );
        int HHMMSS = TradeTime.ToInt();
        
        UFC::UTime CheckTime( HHMMSS / 10000, (HHMMSS % 10000) / 100, HHMMSS % 100 );
        UFC::UTime CurrentTime;
        
        if( (CurrentTime - CheckTime) > FTradeTimeout )
        {                        
            UFC::BufferedLog::Printf( " Reject expired order, Tag 21[%s]", TradeTime.c_str() );        
            return FALSE;
        }
    }    
    return TRUE;
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::RejectOvertimeOrder( TSQMessage& OvertimeOrder )
{
    TSQMessage RejectMessage;
    UFC::AnsiString Value;
    
    RejectMessage.Append( "00", "3" );    
    if( OvertimeOrder.Get( "08", Value ) )
        RejectMessage.Append( "08", Value );
    if( OvertimeOrder.Get( "09", Value ) )
        RejectMessage.Append( "09", Value );
    if( OvertimeOrder.Get( "13", Value ) )
        RejectMessage.Append( "13", Value );
    if( OvertimeOrder.Get( "14", Value ) )
        RejectMessage.Append( "14", Value );
    if( OvertimeOrder.Get( "17", Value ) )
        RejectMessage.Append( "17", Value );
    if( OvertimeOrder.Get( "18", Value ) )
        RejectMessage.Append( "18", Value );
    RejectMessage.Append( "19", "E" ); ///< It's a error execution.
    RejectMessage.Append( "40", "87" );///< Order Expired. FConnection->Connect( 192.168.0.120, 23456 )
    RejectMessage.Append( "21", FToday );
    UFC::UTime CurrentTime;
    UFC::AnsiString TimeString;
    TimeString.Printf( "%02d%02d%02d%02d", CurrentTime.getHour(),  CurrentTime.getMinute(),  CurrentTime.getSecond(),  CurrentTime.getMillisecond()/10 );
    RejectMessage.Append( "22", TimeString.c_str() );
    RejectMessage.Append( "24", "q" );
    if( OvertimeOrder.Get( "27", Value ) )
        RejectMessage.Append( "27", Value );
    if( OvertimeOrder.Get( "29", Value ) )
        RejectMessage.Append( "29", Value );
    if( OvertimeOrder.Get( "30", Value ) )
        RejectMessage.Append( "30", Value );
    if( OvertimeOrder.Get( "31", Value ) )
        RejectMessage.Append( "31", Value );
    if( OvertimeOrder.Get( "32", Value ) )
        RejectMessage.Append( "32", Value );
    if( OvertimeOrder.Get( "36", Value ) )
        RejectMessage.Append( "36", Value );
    if( OvertimeOrder.Get( "42", Value ) )
        RejectMessage.Append( "42", Value );
    if( OvertimeOrder.Get( "43", Value ) )
        RejectMessage.Append( "43", Value );
    if( OvertimeOrder.Get( "O3", Value ) )
        RejectMessage.Append( "O3", Value );
    if( OvertimeOrder.Get( "VK", Value ) )
        RejectMessage.Append( "VK", Value );        
    if( OvertimeOrder.Get( "OG", Value ) )
        RejectMessage.Append( "OG", Value );        
    if( OvertimeOrder.Get( "TF", Value ) )
        RejectMessage.Append( "TF", Value );        
    if( OvertimeOrder.Get( "P2",Value))
        RejectMessage.Append( "P2", Value );    
    ///< PVC ID processed this order.
    RejectMessage.Append( "VI", "XX" );    	
    ///< Line No processed this order.
    RejectMessage.Append( "VH", "XX" );             
    Value = RejectMessage.ToString();
    FConfirmQueue->Write( (char*)(Value.c_str()), Value.Length() );
}
//------------------------------------------------------------------------------
