#include "PHashedSet.h"
#include "SpeedyObjectsManager.h"
#include "../TAIFEXLib/LinkSubSystem.h"
#include "../TAIFEXLib/TMPMessage.h"
#include "Speedy.h"
#include "QueryPVC.h"
#include "Profiler.h"
//---------------------------------------------------------------------------
extern BOOL  IsTestTMP;
extern Int32 TransMax;
//------------------------------------------------------------------------------
TP01DecimalLocator::TP01DecimalLocator( const UFC::AnsiString& DataFormat, const UFC::AnsiString& P01File )
:FDataFormat( DataFormat )
,FP01FileName( P01File )
{

}
//------------------------------------------------------------------------------
void TP01DecimalLocator::Save( const UFC::AnsiString& iniFileName, const UFC::AnsiString& SectionName )
{
    CreateFileIfNotExist( iniFileName );

    UFC::TParseData pd;
    UFC::FileStream fs( FP01FileName, O_RDONLY );

    pd.SetDataFormat( &FDataFormat );
    UFC::TDataset* pDataset = pd.Parse( &fs );

    if( pDataset != NULL )
    {
        UFC::UiniFile ini( iniFileName );
        int nCount = pDataset->GetRecordCount();

        for( int i = 0; i < nCount; i++ )
            SetValueFromRecord( pDataset->GetRecord(i), &ini, SectionName );
        ini.Save();
    }
}
//------------------------------------------------------------------------------
void TP01DecimalLocator::CreateFileIfNotExist( const UFC::AnsiString& FileName )
{
    if( !UFC::FileExists(FileName) )
        UFC::FileStream( FileName, O_CREAT );
}
//------------------------------------------------------------------------------
void TP01DecimalLocator::SetValueFromRecord( UFC::TRecord* pRecord, UFC::UiniFile* pIniFile, const UFC::AnsiString& SectionName )
{
    if( (pRecord != NULL) && (pIniFile != NULL) )
    {
        UFC::AnsiString Symbol, DecimalLocator;
        pRecord->GetField( "Symbol", Symbol );
        pRecord->GetField( "DecimalLocator", DecimalLocator );
        pIniFile->SetValue( SectionName, Symbol, DecimalLocator );
    }
}
//------------------------------------------------------------------------------
void DecimalLocatorThread::Execute( void )
{
    UFC::AnsiString FormatFile;
    UFC::AnsiString P01File;
    UFC::AnsiString OutputFile;
    UFC::AnsiString IniSessionName;

    if( FIsOption == TRUE )
    {
        if( FIsTMP == TRUE )
        {
            FormatFile     = "../cfg/Options_P08.ini";
            P01File        = "../ftOPT/P08.10";
        }
        else
        {
            FormatFile     = "../cfg/Options_P01.ini";
            P01File        = "../ftOPT/P01";
        }
        OutputFile     = "../cfg/OPTSymbol.ini";
        IniSessionName = "OptionProducts";
    }
    else
    {
        if( FIsTMP == TRUE )
        {
            FormatFile     = "../cfg/Futures_P08.ini";
            P01File        = "../ftFUT/P08.20";
        }
        else
        {
            FormatFile     = "../cfg/Futures_P01.ini";
            P01File        = "../ftFUT/P01";
        }
        OutputFile     = "../cfg/FUTSymbol.ini";
        IniSessionName = "FuturesProducts";
    }
    UFC::SleepMS( 5000 );
    UFC::BufferedLog::Printf( " *** Try to generate file [%s] ***",OutputFile.c_str() );

    if( UFC::FileExists( FormatFile ) ==TRUE )
    {
        if( UFC::FileExists( P01File ) ==TRUE )
        {
            TP01DecimalLocator P01DecimalLocator( FormatFile, P01File );

            P01DecimalLocator.Save( OutputFile, IniSessionName );
            UFC::BufferedLog::Printf( " Generate file [%s] OK.",OutputFile.c_str() );
        }
        else
            UFC::BufferedLog::Printf( " File [%s] not found.Stop generate [%s] from this file.",P01File.c_str(),OutputFile.c_str() );
    }
    else
        UFC::BufferedLog::Printf( " File [%s] not found.Stop generate [%s] from this file.",FormatFile.c_str(),OutputFile.c_str() );

}
//---------------------------------------------------------------------------
// Order transfer thread.
// Transfer all orders in queue to backup server.
//---------------------------------------------------------------------------
TransferOrderThread::TransferOrderThread( BOOL IsOpt )
:UFC::PThread(  NULL, TRUE  )
,FIsOptions( IsOpt )
{
    if( FIsOptions == TRUE )
    {       
        if( !FQueue.Open( OPT_X25_QUEUE, TRUE ) )
        {
            UFC::BufferedLog::Printf( " Open Option Order queue failed.");
            exit( 0 );
        }        
    }
    else
    {       
        if( !FQueue.Open( FUT_X25_QUEUE, TRUE ) )
        {
            UFC::BufferedLog::Printf( " Open Futures Order queue failed.");
            exit( 0 );
        }        
    }      
    Start();
}
//---------------------------------------------------------------------------------------------------------------------
void TransferOrderThread::SendToBackup( UInt8* Data )
{
    char  Key[ MAX_DATA_SIZE ];
    char  Msg[ MAX_DATA_SIZE ];
    char  Host[ MAX_DATA_SIZE ];
    Int32 KeySize,HostSize,DataSize,NID,Tick;       
    MTree ReplyData;     
    MemoryStreamReader MemReader( (char*)Data );
    
    MemReader.Read( (char*)&NID, sizeof(Int32) );     /// Fetch the NID
    MemReader.Read( (char*)&Tick, sizeof(Int32) );    /// Fetch the Inqueue Tick.    
    ReplyData.append( "NID", NID );    
    ReplyData.append( "REASON", SPEEDY_ERROR_NO_PVC );    
    MemReader.Read( (char*)&KeySize, sizeof(Int32) ); /// Fetch the Key String size.   
    if( KeySize > 0 )
    {
        MemReader.ReadString( Key, KeySize );
        ReplyData.append( "KEY", Key );
    }
    MemReader.Read( (char*)&HostSize, sizeof(Int32) );/// Fetch the Host String size.    
    if( HostSize > 0 )
    {
        MemReader.ReadString( Host, HostSize );
        ReplyData.append( "HOST", Host );
    }
    else
        ReplyData.append( "HOST", UFC::Hostname );
    MemReader.Read( (char*)&DataSize, sizeof(Int32) ); /// Fetch the Message String size.   
    MemReader.ReadString( Msg, DataSize );        
    ReplyData.append( "ORDER", Msg);    
    if( FIsOptions == TRUE )
	MessageObj->Send( "ORDER.REJECT.OPT", UFC::Hostname, ReplyData  );
    else
	MessageObj->Send( "ORDER.REJECT.FUT", UFC::Hostname, ReplyData  );            
}
//---------------------------------------------------------------------------
void TransferOrderThread::Execute( void )
{
    while( TRUE )
    {    
        UFC::UQueueData Order( 0, MAX_DATA_SIZE );        
        
        Order.SetMType( 0 );
        if( FQueue.RecvNoWait( Order ) == FALSE )        
            return; ///< No order in queue, return
        SendToBackup( (UInt8*)Order.GetData() );
        UFC::SleepMS( 5 );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
//
//  FCM side( FCMObjectsManeger )
//
//  Objects Manager for AP code 4 Order subsystem
//
//-----------------------------------------------------------------------------------------------------------------------
SpeedyObjectsManager::SpeedyObjectsManager(  BOOL IsOpt )
: FTotalOrder( 0 ),
  FTotalTime( 0 ),
  FMaxTime( 0 ),
  FtpSendObj( NULL ),
  FtpRecvObj( NULL ),
  FPVCCount( 0 ),
  FIsOptions( IsOpt ),
  FTSSReqID( 1 ),
  OrderInfo( TransMax,FIsOptions, RecoverMode )
{
    UFC::AnsiString Log4FileName;
    UFC::AnsiString Log389FileName;
    UFC::AnsiString Log12FileName;
    UFC::AnsiString Log10FileName;

    if( FIsOptions == TRUE )
    {
        if( !FQueue.Open( OPT_X25_QUEUE, TRUE ) )
            UFC::BufferedLog::Printf( " Open Options X.25 Order queue failed.");
        Log4FileName   = "../log/OPT.x" + UFC::GetDateString()+".4";
        Log389FileName = "../log/OPT.x" + UFC::GetDateString()+".3.8.9";
        Log12FileName  = "../log/OPT.x" + UFC::GetDateString()+".1.2";
        Log10FileName  = "../log/OPT.x" + UFC::GetDateString()+".10";
        FFTPSubject     = "SPEEDY.FTP.OPT";
        FSTXFTPSubject  = "FT.RESPONSE.OPT";
        FOrderSubject   = "ORDER.OPT";
        FConfirmSubject = "ORDER.CONFIRM.OPT";
        FFillSubject    = "ORDER.FILL.OPT";
        FSyncSubject    = "SYNC.OPT";
        new DecimalLocatorThread( TRUE, TRUE );
    }
    else
    {
        if( !FQueue.Open( FUT_X25_QUEUE, TRUE ) )
            UFC::BufferedLog::Printf( " Open Futures X.25 Order queue failed.");
        Log4FileName    = "../log/FUT.x" + UFC::GetDateString()+".4";
        Log389FileName  = "../log/FUT.x" + UFC::GetDateString()+".3.8.9";
        Log12FileName   = "../log/FUT.x" + UFC::GetDateString()+".1.2";
        Log10FileName   = "../log/FUT.x" + UFC::GetDateString()+".10";
        FFTPSubject     = "SPEEDY.FTP.FUT";
        FSTXFTPSubject  = "FT.RESPONSE.FUT";
        FOrderSubject   = "ORDER.FUT";
        FConfirmSubject = "ORDER.CONFIRM.FUT";
        FFillSubject    = "ORDER.FILL.FUT";
        FSyncSubject    = "SYNC.FUT";
        new DecimalLocatorThread( FALSE , TRUE );
        
    }    
    FOrderLog        = new LogManager( Log4FileName );
    FFillLog         = new LogManager( Log389FileName );
    FFileTransferLog = new LogManager( Log12FileName );
    FMarketDataLog   = new LogManager( Log10FileName );
   
    if( MessageObj != NULL )
    {
        MessageObj->AddListener( FOrderSubject, UFC::Hostname, this ); ///< Add Order Message listener.Use hostname as key.    
        MessageObj->AddListener( FFTPSubject,  UFC::Hostname,  new FtpListener());        ///< Add State query Message listener to MessageObject.
        MessageObj->AddListener( FSyncSubject, UFC::Hostname,  new SyncListener( this ));              ///< Add execution report sync. listener to MessageObject.                
    }
}
//-----------------------------------------------------------------------------------------------------------------------
SpeedyObjectsManager::~SpeedyObjectsManager()
{
    delete FOrderLog;
    delete FFillLog;
    delete FFileTransferLog;
    delete FMarketDataLog;
    if( FtpSendObj )
        delete FtpSendObj;
    if( FtpRecvObj )
        delete FtpRecvObj;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::PVCReady( ConnectionParameter* cp )
{
    UFC::AnsiString Key;
    BOOL            IsBroken; 
    char            AP = cp->GetAPCode();
    
    if( AP == '4' )
    {
        Key.Printf("%d_%d",cp->GetLink(), cp->GetPVC() );    
        UFC::PLockObject Lock( FPVCCountCS);  
    
        IsBroken = !FPVCSet.Exists( Key );
        if( IsBroken == TRUE )
            FPVCSet.Add( Key );
        return IsBroken;
    }
    return FALSE;
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::CheckPVCBroken( ConnectionParameter * cp )
{
     if( PVCBroken( cp ) == TRUE )
     {                                              
         FPVCCount--;
         if( FPVCCount.GetValue()  == 0 )
             new TransferOrderThread( FIsOptions );
         UFC::BufferedLog::Printf( " PVC %d broken. (Avaliable:%d).", cp->GetPVC(), FPVCCount.GetValue() );
     }    
}
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::PVCBroken( ConnectionParameter* cp )
{
    UFC::AnsiString Key;
    char            AP = cp->GetAPCode();
    BOOL            IsReady;  
    
    if( AP == '4'  )
    {
        Key.Printf("%d_%d",cp->GetLink(), cp->GetPVC());    
        UFC::PLockObject Lock( FPVCCountCS );

        IsReady = FPVCSet.Exists( Key );
        if( IsReady == TRUE )
            FPVCSet.Delete( Key );
        return IsReady;
    }
    return FALSE;
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::CreateConnectionObjects( int MaxPVC )
{
	///< Create all connections for APCode 1, 2
#ifdef _WIN32
	UFC::AnsiString FolderName = "..\\";

	if ( FIsOptions == TRUE )
		FolderName += "ftOPT";
	else
		FolderName += "ftFUT";
	CreateDirectory(  FolderName.c_str(), NULL );
#else
	UFC::AnsiString FolderName = "../";

	if ( FIsOptions == TRUE )
		FolderName += "ftOPT";
	else
		FolderName += "ftFUT";
	mkdir( FolderName.c_str(), 0755 );
#endif
    UFC::AnsiString Key,SettingFile, SectionName,Value;
    TFXSessions     Sessions( FIsOptions, IsTestTMP ); 
    ConnectionObjectBase *NewConnection;    

    if( FIsOptions == TRUE )
        SettingFile = "../cfg/SpeedyOPTSessionInfo.cfg";
    else
        SettingFile = "../cfg/SpeedyFUTSessionInfo.cfg";

    UFC::UiniFile   Config( SettingFile, TRUE );
            

    CreateFtpSendObject( FolderName, Sessions );
    CreateFtpRecvObject( FolderName, Sessions );
    ///< Create all connections for APCode 4
    FFCMOrderPVCCount = Sessions.GetLines( 4 );
    for ( int i = 0; i < FFCMOrderPVCCount; i++ )
    {
        UFC::AnsiString     BrokerID( Sessions.GetBrokerID( i ) );
        int                 SessionID = Sessions.GetPVC( i );
        int                 Group = 0;
        BOOL                Activate = TRUE;

        SectionName.Printf("%s,%03d", BrokerID.c_str(), SessionID );
        if( Config.GetValue( SectionName, "Enable", Value ))
            Activate = Value.ToInt();
        if( Config.GetValue( SectionName, "Group", Value ))
            Group = Value.ToInt();
        ConnectionParameter cp( FIsOptions, Sessions.GetLink( i ), SessionID, '4', BrokerID, Sessions.GetLinePassword( i ), Group, Activate, Sessions.GetURL( i )  );

        if( cp.IsTCPConnection() == FALSE )
        {
            UFC::BufferedLog::DebugPrintf( " Create X.25 Order Connection Port[%d] PVC[%d].", Sessions.GetLink( i ), SessionID );
            NewConnection = new SpeedyOrderConnectionObject( this, cp, this, this, FIsOptions ) ; 
        }
        else
        {
            UFC::BufferedLog::DebugPrintf( " Create TCP Order Connection Port[%d] SessionID[%d] Active[%d].", Sessions.GetLink( i ), SessionID, Activate );
            NewConnection = new SpeedyTMPOrderConnectionObject( this, cp, this, this,FIsOptions ); 
        }	
        Key.Printf("%05d%04d", Sessions.GetLink( i ), Sessions.GetPVC( i ) );
        FConnectionObjects.Add( Key, NewConnection );
        NewConnection->Active();
    }
    ///< Create all connections for APCode 3
    FFCMConfirmPVCCount = Sessions.GetLines( 3 );
    for ( int i = 0; i < FFCMConfirmPVCCount; i++ )
    {
        UFC::AnsiString     BrokerID( Sessions.GetBrokerID( i ) );
        int                 SessionID = Sessions.GetPVC( i );
        BOOL                Activate = TRUE;

        SectionName.Printf("%s,%03d", BrokerID.c_str(), SessionID );
        if( Config.GetValue( SectionName, "Enable", Value ))
            Activate = Value.ToInt();
        ConnectionParameter cp( FIsOptions, Sessions.GetLink( i ), SessionID, '3', BrokerID, Sessions.GetLinePassword( i ),0, Activate, Sessions.GetURL( i ) );

        if( cp.IsTCPConnection() == FALSE )
        {
            UFC::BufferedLog::DebugPrintf( " Create X.25 Execution Connection Port[%d] PVC[%d].", Sessions.GetLink( i ), Sessions.GetPVC( i ) );
            NewConnection = new SpeedyConfirmConnectionObject( this, cp, this, this, '3',FIsOptions );
        }
        else
        {
            UFC::BufferedLog::DebugPrintf( " Create TCP Execution Connection Port[%d] SessionID[%d].", Sessions.GetLink( i ), Sessions.GetPVC( i ) );
            NewConnection = new SpeedyTMPCMConnectionObject( this, cp, this, this, '3', FIsOptions );
        }
        Key.Printf("%05d%04d", Sessions.GetLink( i ), Sessions.GetPVC( i ) );
        FConnectionObjects.Add( Key, NewConnection );
        NewConnection->Active();
    }
    ///< Create all connections for APCode 8
    FCMOrderConfirmPVCCount = Sessions.GetLines( 8 );
    for ( int i = 0; i < FCMOrderConfirmPVCCount; i++ )
    {
        UFC::AnsiString     BrokerID( Sessions.GetBrokerID( i ) );
        int                 SessionID = Sessions.GetPVC( i );
        BOOL                Activate = TRUE;

        SectionName.Printf("%s,%03d", BrokerID.c_str(), SessionID );
        if( Config.GetValue( SectionName, "Enable", Value ))
            Activate = Value.ToInt();
        ConnectionParameter cp( FIsOptions, Sessions.GetLink( i ), SessionID, '8', BrokerID, Sessions.GetLinePassword( i ),0, Activate, Sessions.GetURL( i ));

        if( cp.IsTCPConnection() == FALSE )
        {
            UFC::BufferedLog::DebugPrintf( " Create X.25 CM Connection Port[%d] PVC[%d].", Sessions.GetLink( i ), Sessions.GetPVC( i ) );
            NewConnection = new SpeedyConfirmConnectionObject( this, cp, this, this, '8', FIsOptions ) ;
        }
        else
        {
            UFC::BufferedLog::DebugPrintf( " Create TCP CM Connection Port[%d] SessionID[%d].", Sessions.GetLink( i ), Sessions.GetPVC( i ) );
            NewConnection = new SpeedyTMPCMConnectionObject( this, cp, this, this, '8', FIsOptions );
        }
        Key.Printf("%05d%04d", Sessions.GetLink( i ), Sessions.GetPVC( i ) );
        FConnectionObjects.Add( Key, NewConnection );
        NewConnection->Active();
    }
    ///< Create all connections for APCode 9
    FCMConfirmPVCCount = Sessions.GetLines( 9 );
    for ( int i = 0; i < FCMConfirmPVCCount; i++ )
    {
        UFC::AnsiString     BrokerID( Sessions.GetBrokerID( i ) );
        int                 SessionID = Sessions.GetPVC( i );
        BOOL                Activate = TRUE;

        SectionName.Printf("%s,%03d", BrokerID.c_str(), SessionID );
        if( Config.GetValue( SectionName, "Enable", Value ))
            Activate = Value.ToInt();
        ConnectionParameter cp( FIsOptions, Sessions.GetLink( i ), SessionID, '9', BrokerID, Sessions.GetLinePassword( i ),0, Activate );

        NewConnection = new SpeedyConfirmConnectionObject( this, cp, this, this, '9', FIsOptions );
	Key.Printf("%05d%04d", Sessions.GetLink( i ), Sessions.GetPVC( i ) );
	FConnectionObjects.Add( Key, NewConnection );
        NewConnection->Active();
    }    
    ///< Create all connections for APCode 10
    if ( Sessions.GetLines( 10 ) > 0 )
    {
        ConnectionParameter cp( FIsOptions, Sessions.GetLink( 0 ), Sessions.GetPVC( 0 ), '0', Sessions.GetBrokerID( 0 ), Sessions.GetLinePassword( 0 ) );

        NewConnection = new SpeedyMarketDataConnectionObject( this, cp, this, this, FIsOptions );
	Key.Printf("%05d%04d", Sessions.GetLink( 0 ), Sessions.GetPVC( 0 ) );
	FConnectionObjects.Add( Key, NewConnection );
        NewConnection->Active();
    }

}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ReplyLineState( int Link, int PVC )
{
    ConnectionObjectBase* co;
	MTree                 ReplyData;
    int                   State = PVC_STATUS_UNKNOWNPVC;

    if( (co = GetConnection( Link, PVC )) != NULL )
         co->OnCommand( COMMAND_GET_STATE, ( void * ) & State );

    ReplyData.append( "COMMAND", AGENT_REPLY_STATE );
    ReplyData.append( "LINK", Link );
    ReplyData.append( "PVC", PVC );
    ReplyData.append( "SYS", (Int32) FIsOptions );
    ReplyData.append( "PVC_STATE", State );
    MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, ReplyData, FALSE ); 
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ReplyLineState( void )
{
    ConnectionParameter * cp;
    ConnectionObjectBase * co;

    for ( register int i = 0; i < FConnectionObjects.ItemCount(); i++ )
    {
        MTree ReplyData;
        int   State = PVC_STATUS_UNKNOWNPVC;

        if( (co = static_cast < ConnectionObjectBase * > ( FConnectionObjects.GetItem( i ) )) != NULL )
        {
            cp = co->GetConnectionParameter();
            co->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
            ReplyData.append( "COMMAND", AGENT_REPLY_STATE );
            ReplyData.append( "LINK", cp->GetLink() );
            ReplyData.append( "PVC", cp->GetPVC() );
            ReplyData.append( "SYS", (Int32) FIsOptions );
            ReplyData.append( "PVC_STATE", State );
            MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, ReplyData, FALSE ); 
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::TMPFTRquest( Int32 TradeKind, Int32 ReqID, const UFC::AnsiString& Data )
{
    ConnectionParameter * cp;
    ConnectionObjectBase * co;

    for ( register int i = 0; i < FConnectionObjects.ItemCount(); i++ )
    {
        if( (co = static_cast < ConnectionObjectBase * > ( FConnectionObjects.GetItem( i ) )) != NULL )
        {
            cp = co->GetConnectionParameter();            
            if( cp->GetAPCode() == '4' && cp->IsTCPConnection() && co->IsReady( ) )
            {
                TMP::TMPMessage R013( TMP::tmtR13 );
        
                R013["fcm_req_id"]  = ReqID;
                if( IsOptions() == TRUE )
                    R013["system_type"] = 10 + TradeKind; ///< Options
                else
                    R013["system_type"] = 20 + TradeKind; ///< Futures
                R013["data"]        = Data;    
                co->OnCommand( EVENT_TMP_GET_MSG, (void*)&R013);
                return;
            }
        }
    }
    UFC::BufferedLog::Printf( " FT Request failed,No available TMP Session ReqID[%d].",ReqID );
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::TMPTSSRquest( Int32 Group, Int32 SubType, Int32 ReqID )
{
    ConnectionParameter * cp;
    ConnectionObjectBase * co;

    for ( register int i = 0; i < FConnectionObjects.ItemCount(); i++ )
    {
        if( (co = static_cast < ConnectionObjectBase * > ( FConnectionObjects.GetItem( i ) )) != NULL )
        {
            cp = co->GetConnectionParameter();            
            if( cp->IsTCPConnection() && co->IsReady( ) )
            {                
                TMP::TMPMessage R11( TMP::tmtR11 );
    
                if( SubType != 2 ) 
                {
                    R11[ "TradeReqID" ]  = FTSSReqID;
                    FTSSReqID++;
                }
                else///< Unsubscribe
                    R11[ "TradeReqID" ]  = ReqID;
                R11["flow_group_no"] = Group; 
                R11["SubscriptionRequestType"] = SubType;
                co->OnCommand( EVENT_TMP_GET_MSG, (void*)&R11);
                return;
            }
        }
    }
    UFC::BufferedLog::Printf( " Trading Session Status Request failed,No available TMP Session." );
}
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase * SpeedyObjectsManager::GetConnection( int Link, int PVC )
{
    UFC::AnsiString      Key;

    Key.Printf("%05d%04d", Link, PVC );
    UFC::PLockObject Lock( FCriticalSection );

    return FConnectionObjects.GetObjectByKey( Key );
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::RemoveConnection( ConnectionObjectBase* Remove )
{
    ConnectionObjectBase *co;
    UFC::AnsiString       Key;
    UFC::PLockObject Lock( FCriticalSection );

    for ( register int i = 0; i < FConnectionObjects.ItemCount(); i++ )
    {
        if( FConnectionObjects.GetItem(i,Key,co) && co == Remove )
        {
            FConnectionObjects.DeleteByKey( Key );
            return;
        }
    }
}
//---------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ListenConnection( int Link, int PVC )
{
     ConnectionParameter * cp;
     ConnectionObjectBase * co;
     UFC::PLockObject Lock( FCriticalSection );

     for ( register int i = 0; i < FConnectionObjects.ItemCount(); i++ )
     {
         co = static_cast < ConnectionObjectBase * > ( FConnectionObjects.GetItem( i ) );
         cp = co->GetConnectionParameter();
         if ( cp->GetPVC() == PVC && cp->GetLink() == Link )
              co->Listen( TRUE );
         else
              co->Listen( FALSE );
     }
}
//---------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::EnableConnection( int Link, int PVC, BOOL Enabled )
{
     ConnectionObjectBase * co = GetConnection( Link, PVC );
     if( co != NULL )
     {
		UFC::PLockObject Lock( FCriticalSection );

        co->Enable( Enabled );              
     }
}
//---------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ReportPerformance( void )
{
    MTree Data;
    static int MsgCount = 0;
    static long PrevTime = 0;
    long TimeNow;
    int MsgSec = 0;

    TimeNow = GetTimeMS();
    if ( PrevTime != 0 )
        MsgSec = ( (Int32)FTotalOrder - MsgCount ) * 100000 / ( TimeNow - PrevTime );
    PrevTime = TimeNow;
    MsgCount = (Int32)FTotalOrder;
    Data.append( "COMMAND", AGENT_PERFORMANCE );
    Data.append( "TRADE_SYS", FIsOptions );
    Data.append( "COUNT", (Int32)FTotalOrder );
    Data.append( "MSG_SEC", MsgSec );
    Data.append( "QUEUE", (Int32)FQueue.Count() );
    Data.append( "TIME",  UFC::GetHHMMSS() );
    MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, Data, FALSE );
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::Flush()
{
    FOrderLog->Flush();          ///< Flush the APCode 4 log to file
    FFillLog->Flush();           ///< Flush the APCode 3,8,9 log to file
    FFileTransferLog->Flush();   ///< Flush the APCode 1,2 log to file
    FMarketDataLog->Flush();     ///< Flush the APCode 10 log to file
    UFC::BufferedLog::FlushToFile(); ///< Flush the screen log to file.
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::Run()
{
    while ( TRUE )
    {
        sleep( 1 );
        Flush( );
        ReportPerformance();
        if( IOCExpired  > 0 )
            CheckIOCOrderQty(); ///< Check the IOC partial filled order auto canceled timer.
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::Stop()
{	     
     ConnectionObjectBase * co;
     
     for ( register int i = 0; i < FConnectionObjects.ItemCount(); i++ )
     {
         co = static_cast < ConnectionObjectBase * > ( FConnectionObjects.GetItem( i ) );
		 co->Close();
     }
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::SendRWDataToBus( char* IOData )
{
    UFC::AnsiString Time( IOData, 12 );
    UFC::AnsiString IOString;
    MTree      Data;

    IOString.Printf( "%s*%s",Time.c_str(), IOData + 29 );
    Data.append( "COMMAND", AGENT_PVC_DAT );
    Data.append( "IO_DATA", IOString );
    MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, Data, FALSE );
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ReportPVCState( int Link, int PVC, int State  )
{
    MTree Tree;

    Tree.append( "COMMAND", AGENT_REPLY_STATE );
    Tree.append( "LINK", Link );
    Tree.append( "PVC", PVC );
    Tree.append( "SYS", FIsOptions );
    Tree.append( "PVC_STATE", State );
    MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, Tree, FALSE );
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::OnConnectionNotify( ConnectionObjectBase * CObj, int Reason, void * Data )
{
    ConnectionParameter * cp = CObj->GetConnectionParameter();
    int State;

    switch ( Reason )
    {
        case CONNECTION_PVC_UPDATE_STATE:
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( cp->GetLink(),cp->GetPVC(),State );
             break;
        case CONNECTION_PVC_ENABLED:
             UFC::BufferedLog::Printf( " Enable PVC %d Port:%d.", cp->GetPVC(),cp->GetLink() );
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( cp->GetLink(),cp->GetPVC(),State );
             break;
        case CONNECTION_PVC_DISABLED:
             CheckPVCBroken( cp );
             UFC::BufferedLog::Printf( " Disabled PVC %d Port:%d.", cp->GetPVC(),cp->GetLink() );
             ReportPVCState( cp->GetLink(),cp->GetPVC(),PVC_STATUS_DISABLE );
             break;
        case CONNECTION_OBJECT_NORMAL_CLOSE:
             CheckPVCBroken( cp );
             UFC::BufferedLog::Printf( " PVC %d Normal close.", cp->GetPVC() );
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( cp->GetLink(),cp->GetPVC(),State);
             RemoveConnection( CObj );
             break;
        case CONNECTION_OBJECT_X25ERROR:
             CheckPVCBroken( cp );
             UFC::BufferedLog::Printf( " PVC %d X.25 error.", cp->GetPVC() );
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( cp->GetLink(),cp->GetPVC(),State);
             break;
        case CONNECTION_OBJECT_UNKNOWN_ERROR:
             CheckPVCBroken( cp );
             UFC::BufferedLog::Printf( " PVC %d Unknown error.", cp->GetPVC() );
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( cp->GetLink(),cp->GetPVC(),State);
             break;
        case CONNECTION_PVC_LOCKED:
             CheckPVCBroken( cp );
             UFC::BufferedLog::Printf( " PVC %d Locked.", cp->GetPVC() );
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( cp->GetLink(),cp->GetPVC(),State);
             break;
        case CONNECTION_OBJECT_PVC_READY:             
             if( PVCReady( cp ) == TRUE )             
             {                            
                FPVCCount++;
                if( cp->IsTCPConnection() == TRUE )
                    UFC::BufferedLog::Printf( " SessionID[%d] Ready!. (Avaliable:%d)", cp->GetSessionID(),FPVCCount.GetValue() );
                else
                    UFC::BufferedLog::Printf( " PVC[%d] Ready!. (Avaliable:%d)", cp->GetPVC(),FPVCCount.GetValue() );
             }                          
             ReportPVCState( cp->GetLink(),cp->GetPVC(), PVC_STATUS_ONLINE_READY);
             break;
        case CONNECTION_OBJECT_PVC_BROKEN:
             CheckPVCBroken( cp );
             UFC::BufferedLog::Printf( " PVC %d broken.", cp->GetPVC() );
             ReportPVCState( cp->GetLink(),cp->GetPVC(), PVC_STATUS_BROKEN );
             break;
        case CONNECTION_PVC_READ_WRITE:
             SendRWDataToBus((char*)Data);
             break;
        case FTP_STATUS_REPORT:
             FtpStatusReport( (TFtpStatusObject*)Data );             
             break;        
    }
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree * Data )
{
    if ( Subject == FOrderSubject )
    {        
        char*  Order;
        char*  Key  = NULL;
        char*  Host = NULL;
        Int32  NID,DataSize,KeySize,HostSize,TotalSize,Group,Dirty,CancelByNID,Tick;

        if( Data->get( "ORDER", Order, DataSize ) == TRUE && Data->get( "NID", NID ) == TRUE )
        {
            UFC::AnsiString OrderID("null");
            
            Data->get( "OID", OrderID );
            if( FIsOptions == TRUE )
                UFC::Profiler::SetPerformanceFlag( 2, 2, UFC::AnsiString(NID),OrderID );
            else
                UFC::Profiler::SetPerformanceFlag( 1, 2, UFC::AnsiString(NID),OrderID );
            if( Data->get( "KEY", Key, KeySize ) == FALSE ) ///< Get the Key
                KeySize = 0;
            if( Data->get( "HOST", Host, HostSize ) == FALSE ) ///< get the host
                HostSize = 0;            
            if( Data->get( "GROUP", Group ) == FALSE )
                Group = 0;
            TotalSize = 20 + KeySize + HostSize + DataSize;
            
            if( Data->get( "CNID", CancelByNID ) == TRUE )///< Use NID to cancel the original Order.
            {
                 UFC::AnsiString OID,IB;
                 if( OrderInfo.GetOID( CancelByNID, OID, IB ) == TRUE )
                     ModifyOID( Order, OID );
                 else
                     return; ///< Speedy don't known this order.
            }
            if( (UInt32)NID == 0 ) ///< Check NID
            {
                UFC::BufferedLog::Printf( " Invalid NID:%d Order:%s", NID, Order );
                return;
            }
            if( TotalSize >= MAX_DATA_SIZE ) ///< Data size larger than MAX_DATA_SIZE ?
            {
                UFC::BufferedLog::Printf( " Invalid Order NID:[%d] Order:[%s] size larger then %d", NID, Order,MAX_DATA_SIZE );
                return;
            }                      
            if( Data->get( "DIRTY", Dirty ) == FALSE ) ///< No dirty byte exists
            {
                if( FQueue.Count() >= QueueMax )
                {
                    SpeedyReject( Order, NID, Key, Host, SPEEDY_ERROR_QUEUE_FILL ); ///< Queue fulled
                    return;
                }
                if( ( Int32 )FPVCCount <= 0 )
                {
                    SpeedyReject( Order, NID, Key, Host, SPEEDY_ERROR_NO_PVC ); ///< Any PVC available?
                    return;
                }    
            }
            else
            {
                Int32 RejectReason;
                if( Data->get( "REASON", RejectReason ) && RejectReason == SPEEDY_ERROR_NO_PVC )
                {
                    SpeedyReject( Order, NID, Key, Host, SPEEDY_ERROR_NO_PVC, TRUE ); ///< Queue fulled
                    return;                                                    ///< No PVC available in these Speedy cluster 
                }                                                              ///< Reject to client.(As a order reject )  
            }
            ///< If user gives a Tick, use user's tick.
            if( Data->get( "TICK", Tick ) == FALSE )
                Tick = UFC::GetTickCountMS();
            try
            {               
                ///< Binary Layout: NID( 4 bytes )| Tick ( 4 bytes )| Key size( 4 Bytes) | Key data |Host size(4 bytes)| Host data|Data size(4 bytes)| Order
                char QueueData[ MAX_DATA_SIZE ];
                MemoryStreamWriter MemWriter( QueueData );
    
                MemWriter.Write( (char*)&NID, sizeof(Int32) );     /// Fetch the NID
                MemWriter.Write( (char*)&Tick, sizeof(Int32) );    /// Fetch the Inqueue Tick.                
                MemWriter.Write( (char*)&KeySize, sizeof(Int32) ); /// Fetch the Inqueue Tick.                
                if( KeySize > 0 )
                    MemWriter.Write( Key, KeySize );
                MemWriter.Write( (char*)&HostSize, sizeof(Int32) );
                if( HostSize > 0 )
                    MemWriter.Write( Host, HostSize );
                MemWriter.Write( (char*)&DataSize, sizeof(Int32) );
                MemWriter.Write( Order, DataSize );                
                ///< Use the Sub-System name as MType of message                                
                UFC::UQueueData OrderData( (long)(100+Group), QueueData, MAX_DATA_SIZE );
                FQueue.Send( OrderData ); 
            }
            catch( UFC::QueueException&  )
            {
                ///< Queue fulled(Up to the system limitation), Reject to client.(As a order reject )
                SpeedyReject( Order, NID, Key, Host, SPEEDY_ERROR_QUEUE_FILL, TRUE ); 
            }
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ModifyOID( char* Order, const UFC::AnsiString& OID )
{
    ///< Copy to R010
     for( register int i = 0; i < 5; i++ )
          Order[ 21 + i ] = OID[ i ];
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::SpeedyReject( const UFC::AnsiString& Order, Int32 NID, char* Key, char* Host, Int32 ErrCode, BOOL AsConfirm )
{
    MTree Data;
    if( AsConfirm == TRUE ) ///< Reject as a order confirm
    {
        UFC::AnsiString X25Time,HHMMSS,FateR020;

        GetHHMMSS( HHMMSS );
        FateR020.Printf( "%s03%s%02d%s%04d%04d%08d%08d",Order.SubString( 0, 4 ).c_str(),HHMMSS.c_str(),ErrCode,Order.c_str()+14,0,0,0,0 );
        GetTimeString( X25Time, FALSE );
        Data.append( "CONFIRM_ORDER", FateR020 );        
        Data.append( "PVC", "00" );
        Data.append( "PBNO", "F000000" );        
        Data.append( "NID", NID );
        Data.append( "TTIME", X25Time );
        Data.append( "XTIME", X25Time );
        if( Key != NULL )
            Data.append( "KEY", Key );       
        if( Host != NULL && strlen( Host ) > 0  )
            Data.append( "HOST", Host );      
        else
            Data.append( "HOST", UFC::Hostname );
        Data.append( "PHOST", UFC::Hostname );
        MessageObj->Send( FConfirmSubject, UFC::Hostname, Data  );
    }
    else ///< REJECT to HA/Load balance controlor.( like SpeedyGateway, MegaMAPPER... )
    {   
        Data.append( "ORDER", Order );
        Data.append( "NID", NID );    
        Data.append( "REASON", ErrCode );
        if( Key != NULL && strlen( Key ) > 0 )
            Data.append( "KEY", Key );
        if( Host != NULL && strlen( Host ) > 0  )
            Data.append( "HOST", Host );      
        else
            Data.append( "HOST", UFC::Hostname );
        if( OrderInfo.ClusterEnable() == FALSE )
            Data.append( "DIRTY", 1 );
        if( FIsOptions == TRUE )
            MessageObj->Send( "ORDER.REJECT.OPT", UFC::Hostname, Data  );
        else
            MessageObj->Send( "ORDER.REJECT.FUT", UFC::Hostname, Data  );    
        UFC::BufferedLog::Printf( " Reject order[%s] Length:%d", Order.c_str(), Order.Length() );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::CreateFtpSendObject( const UFC::AnsiString & FolderName, TFXSessions& Sessions  )
{
    if ( Sessions.GetLines( 1 ) > 0 )
    {
        UFC::AnsiString Key;
        ConnectionParameter cp( FIsOptions, Sessions.GetLink( 0 ), Sessions.GetPVC( 0 ), '1', Sessions.GetBrokerID( 0 ), Sessions.GetLinePassword( 0 ) );

        FtpSendObj = new TSpeedyFTPSendObject( this, Sessions.GetBrokerID( 0 ), "F000000", cp, this, this, FIsOptions );
        FtpSendObj->SetFolderName( FolderName );
    	Key.Printf("%05d%04d", Sessions.GetLink( 0 ), Sessions.GetPVC( 0 ) );
        FConnectionObjects.Add( Key, FtpSendObj );
        FtpSendObj->Active();
    }
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::CreateFtpRecvObject( const UFC::AnsiString & FolderName, TFXSessions& Sessions  )
{
    if ( Sessions.GetLines( 2 ) > 0 )
    {
        UFC::AnsiString Key;
        ConnectionParameter cp( FIsOptions, Sessions.GetLink( 0 ), Sessions.GetPVC( 0 ), '2', Sessions.GetBrokerID( 0 ), Sessions.GetLinePassword( 0 ) );

        FtpRecvObj = new TSpeedyFTPRecvObject( this, Sessions.GetBrokerID( 0 ), "F000000", cp, this, this, FIsOptions );
        FtpRecvObj->SetFolderName( FolderName );
        Key.Printf("%05d%04d", Sessions.GetLink( 0 ), Sessions.GetPVC( 0 ) );
        FConnectionObjects.Add( Key, FtpRecvObj );
        FtpRecvObj->Active();
    }
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::Download( const UFC::AnsiString & FileCode, int StartSeq, int EndSeq )
{
    if( FtpSendObj )
    {
        FtpSendObj->AddNewJob( new TFtpJobObject( FtpSendObj->GetSourceID(), FileCode, StartSeq, EndSeq ) );
        UFC::BufferedLog::Printf( " Download %s Start:%d End:%d", FileCode.c_str(), StartSeq, EndSeq);
    }
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::Download( const UFC::AnsiString& FileCode, const UFC::AnsiString& Data )
{
    if( FtpSendObj )
    {
        FtpSendObj->AddNewJob( new TFtpJobObject( FileCode, Data ) );
        UFC::BufferedLog::Printf( " Download %s Data:%s", FileCode.c_str(), Data.c_str() );
    }
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::FtpStatusReport( TFtpStatusObject* pObj )
{
    if( pObj )
    {
        MTree Data;
        Int32 StatusCode = pObj->GetStatusCode();

        Data.append( "FileCode", pObj->GetFileCode() );
        Data.append( "StatusCode", StatusCode );
        Data.append( "BrokerID", FtpSendObj->GetConnectionParameter()->GetBrokerID());
        MessageObj->Send( FFTPSubject, UFC::Hostname, Data, FALSE );
        
        if( StatusCode != 0 && StatusCode >=1 && StatusCode <= 99 )///< TAIFEX error        
        {
            MTree DataForSTX;

            DataForSTX.append( "Sender", FtpSendObj->GetConnectionParameter()->GetBrokerID());
            DataForSTX.append( "FileCode", pObj->GetFileCode() );
            DataForSTX.append( "Status", StatusCode );            
            MessageObj->Send( FSTXFTPSubject, FtpSendObj->GetConnectionParameter()->GetBrokerID(), DataForSTX, FALSE );
        }
        delete pObj;
    }
}
//------------------------------------------------------------------------------
//  Functions about auto delete IOC partial filled orders.
//------------------------------------------------------------------------------
void SpeedyObjectsManager::CheckIOCOrderQty( void )
{
    IOCOrder*        OrderInf ;
    UFC::PLockObject LockObject( FIOC_CS );        

    for( register int i = 0; i < FIOCList.ItemCount(); i++ )
    {
        if( (OrderInf = FIOCList.GetItem( i )) != NULL &&  OrderInf->C030 != NULL )  ///< It's a partially filled orders 
        {
            OrderInf->RefCount--;	      ///< Decrease timer.  
            if( OrderInf->RefCount <= 0 ) ///< Time to delete it
            {
                SendCancelIOCOrderMessage( OrderInf ); ///< Generate the cancel execution.
                DeleteIOCOrderInfo( OrderInf );        ///< Delete it form hashedmap and list
            }		
        }
    }
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::SendCancelIOCOrderMessage( IOCOrder* pOrder )
{
    if( pOrder->ConnectionObject != NULL )
    {
        Int32 OrigNID;
        UFC::AnsiString RecvTime,Key;

        GetTimeString( RecvTime, FALSE );    
        OrderInfo.GetOrderInfo( pOrder->OrderID, pOrder->IB, Key, OrigNID );
        pOrder->ConnectionObject->SendFillMessage( pOrder->C030, RecvTime.c_str(), RecvTime.c_str(), OrigNID, Key );
        OrderInfo.WriteExecutionLog( FALSE, OrigNID, Key, pOrder->C030, pOrder->PBNO, UFC::Hostname, "00", -1 ); ///< Write log
    }
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::DeleteIOCOrderInfo( IOCOrder* DeleteOrder )
{
    FIOCOrders.DeleteByKey( DeleteOrder->IB + DeleteOrder->OrderID );
    for( register int i = 0; i < FIOCList.ItemCount(); i++ )
    {
         if( FIOCList.GetItem( i ) == DeleteOrder )
         {
             FIOCList.Delete( i );
             break;
         }
    }
    delete DeleteOrder;
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::NewIOC( const UFC::AnsiString& OID, const UFC::AnsiString& IB, int Qty )
{
    UFC::PLockObject LockObject( FIOC_CS );    
  
    if( FIOCOrders.GetObjectByKey( IB+OID ) == NULL ) ///< Not exists
    {
        IOCOrder* NewOrder = new IOCOrder( OID, IB, Qty );
        FIOCOrders.Add( IB + OID, NewOrder );
        FIOCList.Add( NewOrder );
    }
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::SingleOrderFill( SpeedyConfirmConnectionObject* pConnectionObject, C030SingleBody* C030Body, IOCOrder* pOrder,  const UFC::AnsiString& PBNO )
{
    int MatchQty = UFC::AnsiString( C030Body->FQuntity, 4 ).ToInt();
        
    pOrder->LeftQty -= MatchQty;            
    if( MatchQty <= 0 || pOrder->LeftQty <= 0 ) ///< Canceled or filled.
    {
        DeleteIOCOrderInfo( pOrder );
    }    
    else ///< Partially fill.
    {               
        pOrder->RefCount = IOCExpired;               ///< Set the auto delete timer.
        pOrder->ConnectionObject = pConnectionObject;///< Which connection will send this execution report.
        if( pOrder->C030 == NULL )
        {
            pOrder->PBNO = PBNO;                    
            pOrder->C030 = new char[ 512];           ///< Generate a canceled execution, and keep in memory. 
            memset( C030Body->FQuntity,'0', 4 );     ///< fill qty. 
            C030Body->AsString( pOrder->C030 );      ///< Save execution as a string.                           
        }
    }    
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::MultilegOrderFill( SpeedyConfirmConnectionObject* pConnectionObject, C030ComboBody* C030Body, IOCOrder* pOrder,  const UFC::AnsiString& PBNO )
{
    int  MatchQty = UFC::AnsiString( C030Body->FQuntity, 4 ).ToInt();

    pOrder->LeftQty -= MatchQty;            
    if( MatchQty <= 0 || pOrder->LeftQty <= 0 ) ///< Canceled or filled.
    {
        DeleteIOCOrderInfo( pOrder );
    }    
    else ///< Partially fill.
    {                                
        pOrder->RefCount = IOCExpired;                ///< Set the auto delete timer.
        pOrder->ConnectionObject = pConnectionObject; ///< Which connection will send this execution report.
        if( pOrder->C030 == NULL )
        {
            pOrder->PBNO = PBNO;                      
            pOrder->C030 =  new char[ 512 ];           ///< Generate a canceled execution, and keep in memory.
            memset( C030Body->FQuntity,'0', 4 );
            memset( C030Body->FMatchQuntity1,'0', 4 );
            memset( C030Body->FMatchQuntity2,'0', 4 );///< fill qtys. 
            C030Body->AsString( pOrder->C030 );       ///< Save execution as a string.             
        }
    }
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::FillIOC( SpeedyConfirmConnectionObject* pConnectionObject, C03XBody *Msg, const UFC::AnsiString& OID, const UFC::AnsiString& IB,  const UFC::AnsiString& PBNO )
{
    IOCOrder*        pOrder;        
    UFC::PLockObject LockObject( FIOC_CS );    

    if( ( pOrder = FIOCOrders.GetObjectByKey( IB + OID )) != NULL ) ///< It's a IOC Order
    {
        switch( Msg->GetType() )
        {
            case '1': ///< Single order filled.
                      SingleOrderFill(  pConnectionObject, static_cast<C030SingleBody*>(Msg), pOrder, PBNO );
                      break;
            case '2': ///< Multi-leg order filled
                      MultilegOrderFill(  pConnectionObject, static_cast<C030ComboBody*>(Msg), pOrder, PBNO );
                      break;
        }
    }
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::AddQuote( TMP::TMPMessage& Order )
{
    Int32            BrokerID = Order["fcm_id"].ToInteger();
    UFC::AnsiString  OID      = Order["order_no"].ToString();
    UFC::AnsiString  Key;
    UFC::PLockObject LockObject( FQuoteSetCS );

    Key.Printf( "%s_%d",OID.c_str(),BrokerID );
    FQuoteSet.Add( Key );
}
//------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::IsQuote( TMP::TMPMessage& Exec )
{
    Int32            BrokerID = Exec["fcm_id"].ToInteger();
    UFC::AnsiString  OID      = Exec["order_no"].ToString();
    UFC::AnsiString  Key;

    Key.Printf( "%s_%d",OID.c_str(),BrokerID );
    return FQuoteSet.Exists( Key );
}
//------------------------------------------------------------------------------
// class IOCOrder
//------------------------------------------------------------------------------
SpeedyObjectsManager::IOCOrder::IOCOrder( const UFC::AnsiString& OID,const UFC::AnsiString& ib, int Qty)
:OrderID( OID ),
 IB( ib ),
 LeftQty( Qty ), 
 RefCount( IOCExpired ), 
 C030( NULL ), 
 ConnectionObject( NULL )
{
}
//------------------------------------------------------------------------------
SpeedyObjectsManager::IOCOrder::~IOCOrder( void ) 
{ 
    if( C030 != NULL ) 
        delete [ ] C030; 
}
//------------------------------------------------------------------------------


