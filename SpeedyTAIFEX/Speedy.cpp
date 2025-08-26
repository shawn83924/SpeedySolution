
#include "AnsiString.h"

//------------------------------------------------------------------------------
#include "Speedy.h"
#include "SpeedyObjectsManager.h"
#include "../ExchangeUtility/Utility.h"
#include "../TAIFEXLib/X25Connection.h"
#include "../TAIFEXLib/TMPMessage.h"
#include "../Version.h"
//------------------------------------------------------------------------------
#ifdef _WIN32
    #pragma link "UFC.lib"
    #pragma link "Migo.lib"
    #pragma link "TAIFEXLib.lib"
    #pragma link "EX25_BCB.lib"
#endif
//------------------------------------------------------------------------------
SpeedyObjectsManager*  FutObjectManager = NULL;
SpeedyObjectsManager*  OptObjectManager = NULL;
MessageObject*         MessageObj       = NULL;
BOOL                   IsOptions        = FALSE;
BOOL                   IsTestTMP        = FALSE;
BOOL                   RecoverMode      = TRUE;
BOOL                   EnableChecksum   = FALSE;
BOOL                   UseIndexsymbol   = FALSE;
//------------------------------------------------------------------------------
Int32                  MBusPort        = MAPP_DEF_PORT;
Int32                  ExpiredMS       = 30000;
Int32                  IOCExpired      = 5;
Int32                  QueueMax        = 200;
Int32                  TransMax        = 200000;
double                 ThroughputRatio = 0.9;
//------------------------------------------------------------------------------
UFC::AnsiString        COMMAND_SUBJECT = "SPEEDY.COMMAND";
UFC::AnsiString        COMMAND_KEY;
//------------------------------------------------------------------------------
UFC::PEvent            ProcessExistEvent;
Int32                  DebugLevel        = 0;
const UFC::AnsiString  FUT_TMP_FTP_SUBJECT = "SPEEDY.TMP.FTP.FUT";
const UFC::AnsiString  OPT_TMP_FTP_SUBJECT = "SPEEDY.TMP.FTP.OPT";
const UFC::AnsiString  FUT_TMP_STATUS_SUBJECT = "SPEEDY.TMP.TSS.FUT";
const UFC::AnsiString  OPT_TMP_STATUS_SUBJECT = "SPEEDY.TMP.TSS.OPT";
//------------------------------------------------------------------------------
// Reply pvc state
//------------------------------------------------------------------------------
void ReplyPVCState( MTree* Data )
{
    Int32 Link,PVC,Market;
    SpeedyObjectsManager*  ObjManager;    

    if( Data->get( "TRADE_SYS", Market ) )
    {
        if( Market == 1 )
            ObjManager = OptObjectManager;
        else
            ObjManager = FutObjectManager;        
        if( ObjManager != NULL )
        {    
            if( Data->get( "LINK", Link ) && Data->get( "PVC", PVC ) )
                ObjManager->ReplyLineState( Link, PVC );
            else
                ObjManager->ReplyLineState();
        }
    }
}
//------------------------------------------------------------------------------
// Enable/Disable listen a specify PVC data.
//------------------------------------------------------------------------------
void ListenPVC( MTree* Data )
{
    Int32 Link,PVC,Market;
    SpeedyObjectsManager*  ObjManager;    
    
    if( Data->get( "SYS", Market ))
    {
        if( Market == 1 )
            ObjManager = OptObjectManager;
        else
            ObjManager = FutObjectManager;                
        if( ObjManager != NULL )
        {    
            if( Data->get( "LINK", Link ) &&  Data->get( "PVC", PVC ) )
                ObjManager->ListenConnection( Link, PVC );
            else
                ObjManager->ListenConnection( 0, 0 );
        }
    }
}
//------------------------------------------------------------------------------
// Enable/Disable a PVC connection.
//---------------------------------------------------------------------------------------------------------------
void EnablePVC( MTree* Data )
{
    Int32 Link,PVC,Market,IsEnable;
    SpeedyObjectsManager*  ObjManager;    

    if( Data->get( "SYS", Market ))
    {
        if( Market == 1 )
            ObjManager = OptObjectManager;
        else
            ObjManager = FutObjectManager;                
        if( ObjManager != NULL && Data->get( "LINK", Link ) && 
            Data->get( "PVC", PVC ) && Data->get( "ENABLE", IsEnable) )
            ObjManager->EnableConnection( Link, PVC, IsEnable );
    }
}
//---------------------------------------------------------------------------------------------------------------
void CommandListener::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data )
{
    MTree      ReplyData;
    Int32      CMD;

    if( Data->get("COMMAND", CMD ))
    {
      switch( CMD )
      {
         case AGENT_ACK:           ReplyData.append("COMMAND", AGENT_REPLY);
                                   MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, ReplyData, FALSE );
                                   break;
         case AGENT_REQUEST_STATE: ReplyPVCState( Data );
                                   break;
         case AGENT_LISTEN_PVC:    ListenPVC( Data );
                                   break;
         case AGENT_PVC_ENABLED:   EnablePVC(  Data  );
                                   break;
      }
    }
}
//------------------------------------------------------------------------------
void FtpListener::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data )
{
    UFC::AnsiString FileCode;
    int StartSeq = 0;
    int EndSeq = 0;

    ///< X.25 FT Request
    if( Data->get("FileCode",FileCode) && Data->get("StartSeq",StartSeq) && Data->get("EndSeq",EndSeq) )        
    {
        if( FutObjectManager != NULL && FutObjectManager->GetFTPSubject() == Subject )
            FutObjectManager->Download( FileCode, StartSeq, EndSeq );
        else if( OptObjectManager != NULL && OptObjectManager->GetFTPSubject() == Subject )
            OptObjectManager->Download( FileCode, StartSeq, EndSeq );
    }
    ///< TMP FT Request
    Int32 TradeKind,ReqID;
    UFC::AnsiString Body;

    if( Data->get("TradeKind", TradeKind ) == TRUE && Data->get( "ReqID", ReqID) == TRUE && Data->get( "Body", Body ) == TRUE )
    {
        if( IsOptions == FALSE && FutObjectManager != NULL )
        {
            UFC::BufferedLog::Printf( " Futures FTP Request:TradeKind[%d] ReqID[%d] Body[%s]",TradeKind,ReqID,Body.c_str() );
            FutObjectManager->TMPFTRquest( TradeKind, ReqID, Body );
        }
        else if(  IsOptions == TRUE && OptObjectManager != NULL )
        {
            UFC::BufferedLog::Printf( " Options FTP Request:TradeKind[%d] ReqID[%d] Body[%s]",TradeKind,ReqID,Body.c_str() );
            OptObjectManager->TMPFTRquest( TradeKind, ReqID, Body );
        }
    }
}
//------------------------------------------------------------------------------
void ToBackEnd( const UFC::AnsiString& FileCode, const UFC::AnsiString& FileName )
{
    UFC::AnsiString Command;
    UFC::AnsiString Script;

    if( IsOptions == TRUE )
    {
        if( UFC::FileExists( "../cfg/OptionFT.cfg" )== TRUE )
        {
            UFC::UiniFile   Config( "../cfg/OptionFT.cfg" );

            if( Config.SectionExists( FileCode ) == TRUE && Config.GetValue( FileCode, "Script", Script ) )
            {
                Command.Printf("%s %s %s &",Script.c_str(), FileCode.c_str(), FileName.c_str() );
                UFC::BufferedLog::Printf( " Options FTP Execute command:%s",Command.c_str());
                system( Command.c_str());
            }
            else
                UFC::BufferedLog::Printf( " Can not find a script for Options file code:%s.", FileCode.c_str() );
        }
        else
             UFC::BufferedLog::Printf( " File ../cfg/OptionFT.cfg not found." );
            
    }
    else
    {
        if( UFC::FileExists( "../cfg/FuturesFT.cfg" )== TRUE )
        {
            UFC::UiniFile Config( "../cfg/FuturesFT.cfg" );

            if( Config.SectionExists( FileCode ) == TRUE && Config.GetValue( FileCode, "Script", Script ) )
            {
                Command.Printf("%s %s %s &",Script.c_str(), FileCode.c_str(), FileName.c_str() );
                UFC::BufferedLog::Printf( " Futures FTP Execute command:%s",Command.c_str());
                system( Command.c_str());
            }
            else
                UFC::BufferedLog::Printf( " Can not find a script for Futures file code:%s.", FileCode.c_str() );
        }
        else
            UFC::BufferedLog::Printf( " File ../cfg/FuturesFT.cfg not found." );
    }
}
//------------------------------------------------------------------------------
void JavaFTPListener::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data )
{
    UFC::AnsiString ReqID,FileCode,Market,ErrCode,Msg,FileName;

    if( Data->get("ReqID",ReqID ) && Data->get("FileCode",FileCode ) &&
        Data->get("Market",Market ) && Data->get("ErrCode",ErrCode ) )
    {
        MTree ReplyData;
        Int32 Code  = ErrCode.ToInt();

        if( Code == 0 )
            Code = 200;
        else if( Code == 4 )
        {
            if( Data->get("ErrMsg",Msg ) && Msg.AnsiPos(".ready") != -1 )
                Code = 5; ///< .ready not found.
        }
        ReplyData.append( "Src", "FTP" );
        ReplyData.append( "FileCode", FileCode );
        ReplyData.append( "ErrorCode", Code  );
        ReplyData.append( "ReqID", ReqID.ToInt() );
        if( IsOptions == FALSE && FutObjectManager != NULL && Market == "FUT")
        {
            UFC::BufferedLog::Printf( " Futures FTP response FileCode[%s] ErrorCode[%d].", FileCode.c_str(), Code );
            MessageObj->Send( FutObjectManager->GetFTPSubject(), UFC::Hostname, ReplyData, FALSE );
            if( Data->get("FileName",FileName ) == TRUE && Code == 200 )
                ToBackEnd( FileCode, FileName );
        }
        else if(  IsOptions == TRUE && OptObjectManager != NULL && Market == "OPT" )
        {
            UFC::BufferedLog::Printf( " Options FTP response FileCode[%s] ErrorCode[%d].", FileCode.c_str(), Code );
            MessageObj->Send( OptObjectManager->GetFTPSubject(), UFC::Hostname, ReplyData, FALSE );
            if( Data->get("FileName",FileName ) == TRUE && Code == 200 )
                ToBackEnd( FileCode, FileName );
        }
    }
}
//------------------------------------------------------------------------------
void TMPTSSListener::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data )
{
    Int32 Group,SubType,ReqID = 0;
        
    UFC::BufferedLog::Printf( " Receive Tradeing Session State Request" );
    if( Data->get( "Group", Group ) == TRUE && Data->get( "SubType", SubType) == TRUE )
    {
        if( SubType == 2 && Data->get( "ReqID", ReqID ) == FALSE )
        {
            UFC::BufferedLog::Printf( " Unsubscribe Tradeing Session Status without ReqID" );        
            return;
        }            
        UFC::BufferedLog::Printf( " Group[%d] SubType[%d]",Group,SubType );        
        
        if( IsOpt == FALSE && FutObjectManager != NULL )
            FutObjectManager->TMPTSSRquest( Group, SubType, ReqID );
        else if(  IsOpt == TRUE && OptObjectManager != NULL )
            OptObjectManager->TMPTSSRquest( Group, SubType, ReqID );
    }
}
//------------------------------------------------------------------------------
SyncListener::SyncListener( SpeedyObjectsManager* ObjManager )
:FObjectsManager( ObjManager )
{
}
//------------------------------------------------------------------------------
void SyncListener::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data )
{
    Int32 NID,Seq,CMD;
    UFC::AnsiString LOG,KEY,OID,FromHost,IB;
    
    if( Data->get( "CMD", CMD ) == TRUE && Data->get( "SEQ", Seq) == TRUE && Data->get( "HOST", FromHost) == TRUE  )
    {
        if( CMD == 1 ) ///< Received a execution from the Speedy on the server "FromHost".
        {
            ///< It's a new order, Add OrderID, Key, NID to OrderInfoStore.
            if( Data->get( "NID", NID ) == TRUE && Data->get( "Key", KEY ) == TRUE && Data->get( "OID", OID ) == TRUE && Data->get( "IB", IB ) == TRUE)
                FObjectsManager->OrderInfo.AddOrder( OID,IB, KEY, NID, TRUE ); ///< Sync from other Speedy, It's a Speedy order.
            ///< Write this record to log.
            if( Data->get( "Log", LOG ) == TRUE )
                FObjectsManager->OrderInfo.WriteExecutionLog( Seq, FromHost, LOG );
        }
        else if( CMD == 0 )///< The Speedy on the server "FromHost" want to get executions from the "Seq" to the end.
        {
            FObjectsManager->OrderInfo.SetExecSequence( FromHost,Seq );
        }
    }
}
//------------------------------------------------------------------------------
void CheckArgs( char *argv[], int argc )
{   
    for( int i = 1; i < argc; i++ )
    {
         UFC::AnsiString Param( argv[i] );
         if( Param == "-H" || Param == "-h" || Param == "-?" )
         {
             printf( "Speedy TAIFEX TCP/IP X.25 link program Copyright(c) 2003,2009 MDBS\n");
             printf( "Usage: Speedy  [options]\n" );
             printf( "       -H               Show this page.\n" );
             printf( "       -M(Market)       F for Futures O for Option.(Default F)\n" );
             printf( "       -d(fipw)         Debug mode switch.\n" );
             printf( "                        f: print fatal error debug information.\n" );
             printf( "                        i: print debug information.\n" );
             printf( "                        p: print perfotmance debug information.\n" );
             printf( "                        w: print warning debug information.\n" );
             printf( "       -F               First time startup.( Reset the filled report seq,\n" );
             printf( "                                             and the filled report seq table ).\n" );
             printf( "       -I(sec)          IOC Partially fill Order expired time.(default 5 sec)\n" );
             printf( "                        Set to 0 to disable this function.(for X.25 only)\n" );
             printf( "       -EC              Enable TMP checksum.(default disable)\n" );             
             printf( "       -t               Connect to TAIFEX testing session.(for TCP only)\n\n" );
             printf( "       -P(Port)         Service port.(default 12345)\n" );
             printf( " Advanced options for speedy\n" );
             printf( "       -Q(Orders)       Maximum order queue size.(default 200 orders)\n" );
             printf( "       -T(ms)           Ordr expired time.(default 30000 ms)\n" );
             printf( " Optimize options for speedy\n" );
             printf( "       -R(Ratio)         Throughput control rate.(default 0.9,for TCP only)\n" );
             printf( "       -B                Use index symbol.(for TCP only)\n" );
             printf( "       -ET(Transactions) Estimate the max Transactions.(Default 200,000)\n" );
             
             exit( 1 );
         }
         else if( Param == "-B" || Param == "-b" )
         {
             UseIndexsymbol = TRUE;
         }
         else if( Param == "-F" || Param == "-f" )
         {
             RecoverMode = FALSE;
         }
         else if( Param.AnsiPos("-P") != -1  )
         {
             UFC::AnsiString PortStr( (const char*)(Param.c_str() + 2) );
             MBusPort = PortStr.ToInt();
         }
         else if( Param.AnsiPos("-T") != -1  )
         {
             UFC::AnsiString Sec( (const char*)(Param.c_str() + 2) );
             ExpiredMS = Sec.ToInt( );
         }
         else if( Param.AnsiPos("-t") != -1  )
         {
            IsTestTMP = TRUE;
         }
         else if( Param.AnsiPos("-I") != -1  )
         {
             UFC::AnsiString Sec( (const char*)(Param.c_str() + 2) );
             IOCExpired = Sec.ToInt( );
         }
         else if( Param.AnsiPos("-Q") != -1  )
         {
             UFC::AnsiString QSize( (const char*)(Param.c_str() + 2) );
             QueueMax = QSize.ToInt( );
         }         
         else if( Param.AnsiPos("-M") != -1 )
         {
             if( Param[2] == 'o' || Param[2] == 'O' )
                 IsOptions = TRUE;
             else
                 IsOptions = FALSE;
         }
         else if( Param.AnsiPos("-R") != -1 )
         {
             UFC::AnsiString TPRatio( (const char*)(Param.c_str() + 2) );
             ThroughputRatio = TPRatio.ToDouble();
         }
         else if( Param.AnsiPos("-ET") != -1  )
         {
             UFC::AnsiString MaxTransaction( (const char*)(Param.c_str() + 2) );
             TransMax = MaxTransaction.ToInt( );
             if( TransMax < 20000 )
                 TransMax = 20000;
         }
         else if(Param.AnsiPos("-d")!=-1 )
         {
             if( Param.AnsiPos("f")!=-1 )
                 DebugLevel |= UFC::dlFatalError;
             if( Param.AnsiPos("i")!=-1 )
                 DebugLevel |= UFC::dlInformation;
             if( Param.AnsiPos("p")!=-1 )
                 DebugLevel |= UFC::dlPerformance;
             if( Param.AnsiPos("w")!=-1 )        
                 DebugLevel |= UFC::dlWarning;
         }
         else if( Param.AnsiPos( "-EC" ) != -1 )
             EnableChecksum = TRUE;
    }
    MessageHeader::SetTradeSystem( IsOptions, TRUE );///< Set Message market.
}
//------------------------------------------------------------------------------
#ifdef _WIN32
//------------------------------------------------------------------------------
BOOL WINAPI OnWin32Exit( DWORD dwCtrlType)
{
    switch( dwCtrlType )
    {
	case CTRL_C_EVENT:       UFC::BufferedLog::Printf( " Ctrl-C event" );
				 break;
	case CTRL_CLOSE_EVENT: 	 UFC::BufferedLog::Printf( " Ctrl-Close event" );
				 break;
	case CTRL_BREAK_EVENT:   UFC::BufferedLog::Printf( " Ctrl-Break event" );
				 break;
	case CTRL_LOGOFF_EVENT:  UFC::BufferedLog::Printf( " Ctrl-Logoff event" );
				 break;
	case CTRL_SHUTDOWN_EVENT:UFC::BufferedLog::Printf( " Ctrl-Shutdown event" );
				 break;
	default:		 UFC::BufferedLog::Printf( " Unknown event:%d", dwCtrlType );
				 return FALSE;
    }
    if( ObjectManager != NULL ) 
    {        
	UFC::BufferedLog::Printf( " Terminate all connection threads." );	
	ObjectManager->Stop();
	ObjectManager->Flush();
    }
    X25Connection::CloseX25();	
    UFC::BufferedLog::Printf( " Exit Speedy." );	
    return FALSE;
}
//------------------------------------------------------------------------------
#else
//------------------------------------------------------------------------------
void OnExit()
{
    if( FutObjectManager != NULL ) 
        FutObjectManager->Flush();
    if( OptObjectManager != NULL ) 
        OptObjectManager->Flush();    
    X25Connection::CloseX25();
}
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
void MonitoringOtherSpeedy( void )
{
    if( UFC::FileExists( ClusterConfigFile ) == TRUE )
    {
	UFC::UiniFile  Config( ClusterConfigFile );
	UFC::Section*  HostSection;
	if( Config.SectionCount() >= 2 && Config.SectionExists( UFC::Hostname ) )
	{
            for( int i = 0; i < Config.SectionCount(); i++ )
            {
                HostSection = Config.GetSection( i );
		if( HostSection->GetSectionName() != UFC::Hostname )
		{
                    if( IsOptions )
			MessageObj->AddMonitoringProcess( HostSection->GetSectionName(),"SpeedyOPT");
                    else
			MessageObj->AddMonitoringProcess( HostSection->GetSectionName(),"SpeedyFUT");        
		}
            }
	}
    }	
}
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    try
    {
        CheckArgs( argv, argc );
        #ifdef _WIN32
                SetConsoleCtrlHandler( OnWin32Exit, TRUE );
        #else
                atexit( OnExit );
        #endif

        if( IsOptions )
            UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( "../log/SpeedyOPTScreen" + UFC::GetDateString(), 1024, TRUE ));
        else
            UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( "../log/SpeedyFUTScreen" + UFC::GetDateString(), 1024, TRUE ));
        UFC::BufferedLog::Printf( "__________________________________________" );
        UFC::BufferedLog::Printf( "                                          " );
        UFC::BufferedLog::Printf( "   TAIFEX X.25 Trading module,Speedy      " );
        UFC::BufferedLog::Printf( "   startup at %s.             ", UFC::Hostname );
        UFC::BufferedLog::Printf( "                                          ");
        UFC::BufferedLog::Printf( "   Ver:%s Build Date:%s ",SPEEDY_TAIFEX_VER,__DATE__ );
        if( IsOptions )
            UFC::BufferedLog::Printf( "   For Option market:%d", MBusPort );
        else
            UFC::BufferedLog::Printf( "   For Futures market:%d", MBusPort  );
        if( RecoverMode == TRUE )
            UFC::BufferedLog::Printf( "   Recover Mode: On" );
        else
            UFC::BufferedLog::Printf( "   Recover Mode: Off" );
        UFC::BufferedLog::Printf( "__________________________________________" );
        
        UFC::BufferedLog::SetDebugMode( DebugLevel );
        if( IsOptions )
        {
            MessageObj = new MessageObject( "SpeedyOPT", "1.0", "FCM system.", MBusPort );   ///< Create Migo Message object.        
            MessageObj->AddListener( COMMAND_SUBJECT, UFC::Hostname,  new CommandListener());///< Add State query Message listener to MessageObject.                        
            MessageObj->AddListener( FUT_TMP_STATUS_SUBJECT, UFC::Hostname,  new TMPTSSListener( TRUE ));///< Add State query Message listener to MessageObject.
            MessageObj->AddListener( "TCP.FT.RESPONSE", UFC::Hostname, new JavaFTPListener() );
            TMP::TMPMessage::InitTMPMessageLibrary( TMP::tlmOptions );
            TMP::TMPMessage::SetEnableCheckSum( EnableChecksum );                        
            OptObjectManager = new SpeedyObjectsManager( TRUE );            
            MessageObj->SetMonitorListener( new ProcessMonitor(TRUE,OptObjectManager ) );    
            MessageObj->Start();     ///< Start Migo Message pump.    
            ProcessExistEvent.WaitFor( 30 );
            MonitoringOtherSpeedy(); ///< Monitoring Speedys on other servers.
            UFC::SleepMS( 500 );
            OptObjectManager->CreateConnectionObjects( );
            OptObjectManager->Run();
        }
        else
        {
            MessageObj = new MessageObject( "SpeedyFUT", "1.0", "FCM system.", MBusPort );   ///< Create Migo Message object.        
            MessageObj->AddListener( COMMAND_SUBJECT, UFC::Hostname,  new CommandListener());///< Add State query Message listener to MessageObject.            
            MessageObj->AddListener( FUT_TMP_STATUS_SUBJECT, UFC::Hostname,  new TMPTSSListener( FALSE ));///< Add State query Message listener to MessageObject.
            MessageObj->AddListener( "TCP.FT.RESPONSE", UFC::Hostname, new JavaFTPListener() );

            TMP::TMPMessage::InitTMPMessageLibrary( TMP::tlmFutures );            
            TMP::TMPMessage::SetEnableCheckSum( EnableChecksum );              
            FutObjectManager = new SpeedyObjectsManager( FALSE );            
            MessageObj->SetMonitorListener( new ProcessMonitor(FALSE,FutObjectManager ) );    
            MessageObj->Start();     ///< Start Migo Message pump.    
            ProcessExistEvent.WaitFor( 30 );
            MonitoringOtherSpeedy(); ///< Monitoring Speedys on other servers.
            UFC::SleepMS( 500 );
            FutObjectManager->CreateConnectionObjects( );
            FutObjectManager->Run();            
        }
        return 1;
    }
    catch( std::exception &e  )
    {
        printf( "Speedy Exception:%s\n", e.what());
    }
}
//------------------------------------------------------------------------------
