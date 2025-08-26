#include "Speedy.h"
#include "TWSESpeedyObjectsManager.h"
#include "PQueue.h"
#include "../ExchangeUtility/Utility.h"
#include "../Version.h"
#include "Profiler.h"
#include "TLicenseKey.h"
#include "../SpeedyGateway/Administrator.h"
#ifdef __RMLib
#include "RiskManagerObjects.h"
#include "../TSECLib/USunRiskManager.h"
#include "../TSECLib/MegaRiskManager.h"
#endif
//------------------------------------------------------------------------------
TWSE::ExecutionListener* gTSEConfirm;
TWSE::ExecutionListener* gOTCConfirm;
TWSE::ExecutionListener* gTSEFilled;
TWSE::ExecutionListener* gOTCFilled;
//------------------------------------------------------------------------------
Int32                        TWSEVersion       = 0;//< 0:New 1:Old 2:New3/2  
BOOL                         ExtremeVersion    = FALSE;
BOOL                         UseLockLessQueue  = FALSE;
BOOL                         MareketOrderGroup = TRUE;  
BOOL                         SkipTryOrder      = FALSE; 
BOOL                         IsSMPEnable       = FALSE;
BOOL                         SMP_FIllED_POS    = FALSE;
//------------------------------------------------------------------------------
TWSE::SpeedyObjectsManager*  ObjectManager     = NULL;
MessageObject*               MessageObj        = NULL;
BOOL                         RecoverModeExchange       = TRUE;
BOOL                         ExecToQueue       = FALSE;
BOOL                         NotSpeedyToOthers = FALSE;
BOOL                         CachedLog         = TRUE;
Int32                        MBusPort;
Int32                        ExpiredMS         = 300000;
Int32                        TransMax          = 200000;
Int32                        DebugLevel        = 0;
Int32                        MAX_DATA_SIZE     = 384;
double                       ThroughputRatio   = 1.5;
int                          TMPSocketBuffer   = 8*1024;    ///< 8K    
SpeedyTWSEVersion            sVersion          = svStandard;
//------------------------------------------------------------------------------
BOOL                         IsTSEC            = TRUE;
BOOL                         SyncFillPVC       = FALSE;
BOOL                         FIXRejStaleOrd    = FALSE;
BOOL                         FIXMultiThread    = FALSE;
BOOL                         FIXUseDictionary  = FALSE;
BOOL                         FIXResetOnLogon   = FALSE;
int                          FIXStoreType      = 1; ///< Use memory store. (Performance issue)
BOOL                         EnableRPXText     = FALSE; 
BOOL                         BENCHMARK_MODE    = FALSE;
BOOL                         DynamicSessionFIX = FALSE;
BOOL                         GSMPFixLength     = FALSE;
//------------------------------------------------------------------------------
UFC::AnsiString DEPENDENCY_PROCESS( "SpeedyAgent");
//------------------------------------------------------------------------------
UFC::AnsiString COMMAND_SUBJECT("SPEEDY.COMMAND");
UFC::AnsiString COMMAND_KEY;
//------------------------------------------------------------------------------
UFC::AnsiString FTP_SUBJECT;
//------------------------------------------------------------------------------
UFC::AnsiString SYS_STRING("SYS");
UFC::AnsiString LINK_STRING("LINK");
UFC::AnsiString COMMAND_STRING("COMMAND");
UFC::AnsiString ENABLE_STRING("ENABLE");
UFC::AnsiString TRADE_SYS_STRING("TRADE_SYS");
UFC::AnsiString PVC_STATE_STRING("PVC_STATE");
///< For SpeedyObjectManager
UFC::AnsiString COUNT_STRING("COUNT");
UFC::AnsiString MSG_SEC_STRING("MSG_SEC");
UFC::AnsiString IO_DATA_STRING("IO_DATA");
UFC::AnsiString CONFIRM_STRING( "CONFIRM_ORDER" );
UFC::AnsiString ORDER_STRING( "ORDER" );
UFC::AnsiString PROCESS_PVC_STRING( "PROCESS_PVC" );
UFC::AnsiString NID_STRING( "NID" );
UFC::AnsiString ClusterConfigFile( "../cfg/SpeedyCluster.cfg" ); 

UFC::AnsiString OTHER_HOST( "BACKEND" );
UFC::AnsiString OTHER_AE( "OTHERS" );

UFC::UDateTime  StartupTime;
UFC::PMPMCQueue<OrderRecord>* TWSEQueue; 
UFC::PMPMCQueue<OrderRecord>* OTCQueue;
//UFC::PThreadObjectPool<OrderRecord> OrderRecodPool( 2048, 256 );
UFC::PThreadObjectPool<OrderRecord>* OrderRecodPoolPtr = 0;
//------------------------------------------------------------------------------
//UFC::PEvent ProcessStateEvent;
//UFC::PEvent ProcessExistEvent;
//UFC::PEvent* ProcessStateEventPtr = 0;
UFC::PEvent* ProcessExistEventPtr = 0;
//------------------------------------------------------------------------------
// Report the given PVC link state.
//---------------------------------------------------------------------------------------------------------------
void ReplyPVCState( MTree* Data )
{
    MTree Tree;
    Int32 Link,Market,InSeq,OutSeq;
    UFC::AnsiString BrokerID,PVCID;
    
    if( ObjectManager != NULL && Data->get( TRADE_SYS_STRING, Market ))
    {
        if( Market == 3-IsTSEC && Data->get( LINK_STRING, Link ) && Data->get( "PVCID", PVCID ) && Data->get( "BROKERID", BrokerID )  )
        {
            Int32 State = ObjectManager->GetLineState( BrokerID, Link, PVCID,InSeq,OutSeq );
            if( State != PVC_NOT_EXISTS )
            {
                Tree.append( COMMAND_STRING, (Int32)AGENT_REPLY_STATE );
                Tree.append( LINK_STRING, Link );
                Tree.append( "PVCID", PVCID );
                Tree.append( "BROKERID", BrokerID );
                Tree.append( SYS_STRING, 3-IsTSEC );
                Tree.append( PVC_STATE_STRING, State );
                Tree.append( "IN", InSeq );
                Tree.append( "OUT", OutSeq );
                MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, Tree, FALSE );
            }
        }
    }
}
//---------------------------------------------------------------------------------------------------------------
// Monitoring a PVC.
//---------------------------------------------------------------------------------------------------------------
void ListenPVC( MTree* Data )
{
    Int32 Link,IsOpt;
    UFC::AnsiString BrokerID,PVCID;
    
    if( Data->get( SYS_STRING, IsOpt ))
    {
        if( IsOpt == 3-IsTSEC && Data->get( LINK_STRING, Link ) &&  Data->get( "PVCID", PVCID ) && Data->get( "BROKERID", BrokerID ) )
        {
            UFC::BufferedLog::Printf( " Monitoring Link[%d] PVCID[%s] BrokerID[%s]", Link,PVCID.c_str(), BrokerID.c_str() );            
            ObjectManager->ListenConnection( BrokerID, Link , PVCID );
        }
        else
            ObjectManager->ListenConnection( "0000", 0, "00" );
    }
}
//---------------------------------------------------------------------------------------------------------------
// Enable or disable a PVC.
//---------------------------------------------------------------------------------------------------------------
void EnablePVC( MTree* Data )
{
    Int32 Link,IsOpt,IsEnable;
    UFC::AnsiString BrokerID,PVCID;
    
    if( Data->get( SYS_STRING, IsOpt ))
    {
        if( IsOpt == 3-IsTSEC && Data->get( LINK_STRING, Link ) &&  Data->get( "PVCID", PVCID ) && Data->get( "BROKERID", BrokerID ) && Data->get( ENABLE_STRING, IsEnable) )
            ObjectManager->EnableConnection( BrokerID, Link, PVCID, IsEnable );
    }
}
//---------------------------------------------------------------------------------------------------------------
// Change AP code for the given PVC.
//---------------------------------------------------------------------------------------------------------------
void SetAPCode( MTree* Data )
{
    Int32 Link,IsOpt;
    UFC::AnsiString AP,BrokerID,PVCID;

    if( Data->get( SYS_STRING, IsOpt ))
    {
        if( IsOpt == 3-IsTSEC && Data->get( LINK_STRING, Link ) &&  Data->get( "PVCID", PVCID ) && Data->get( "BROKERID", BrokerID ) && Data->get( "APCODE", AP ) )
        {
            UFC::BufferedLog::Printf( " Port:%d PVCID:%s change AP code to %c", Link,PVCID.c_str(), AP[0] );
            ObjectManager->SetAPCode( BrokerID, Link, PVCID, AP[0] );
        }
    }
}
//---------------------------------------------------------------------------------------------------------------
void SetFIXSequence( MTree* Data )
{
    Int32 Link,IsOpt;
    UFC::AnsiString PVCID,BrokerID;

    if( Data->get( SYS_STRING, IsOpt ))
    {
        if( IsOpt == 3-IsTSEC && 
            Data->get( LINK_STRING, Link ) && 
            Data->get( "BROKERID", BrokerID ) && 
            Data->get( "PVCID", PVCID ) )            
        {
            Int32 InSeq  = 1;
            Int32 OutSeq = 1;
            Data->get( "INBOUND",  InSeq );
            Data->get( "OUTBOUND", OutSeq );
            UFC::BufferedLog::Printf( " Set FIX Port:%d PVCID:%s InSeq[%d] OutSeq[%d]", Link,PVCID.c_str(), InSeq, OutSeq );
            ObjectManager->SetFIXSequence( BrokerID, Link, PVCID, InSeq, OutSeq );
        }
    }    
}
//---------------------------------------------------------------------------------------------------------------
void RejectAllOrders( MTree* Data )
{
    Int32 ErroCode = SPEEDY_ERROR_NO_PVC;
    Int32 IsOpt;

    if( Data->get( SYS_STRING, IsOpt ))
    {
        if( IsOpt == 3-IsTSEC )            
        {    
            Data->get( "ERROR", ErroCode );
            ObjectManager->RejectAllOrders( ErroCode );            
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
const char* StateString( TWSE::TMarketStatus Status )
{
    switch( Status )    
   {
       case TWSE::msWait:     return "Waiting";
       case TWSE::msTryOrder: return "Trying";
       case TWSE::msOpened:   return "Market Opened";
       case TWSE::msClosed:   return "Market Closed";
       case TWSE::msStop:     return "Stopped";
       default:         return "Unknown";
   }           
} 
//---------------------------------------------------------------------------------------------------------------
void SetTradingSessionStatus( MTree* Data )
{
    UFC::AnsiString APCode;
    Int32 IsOpt,Status;

    if( Data->get( SYS_STRING, IsOpt ) )
        
    {
        if( IsOpt == 3-IsTSEC &&
            Data->get( "APCODE", APCode ) &&
            Data->get( "STATUS", Status ) )
        {            
            TWSE::TMarketStatus NewStatus = (TWSE::TMarketStatus)Status;
            ObjectManager->SetMarketStatus( APCode[0] , NewStatus);
            if( APCode == "0")
                UFC::BufferedLog::Printf( " Set trading session[Normal] status to[%s]", StateString(NewStatus));
            else if( APCode == "4")
                UFC::BufferedLog::Printf( " Set trading session[Odd] status to[%s]", StateString(NewStatus));
            else if( APCode == "9")
                UFC::BufferedLog::Printf( " Set trading session[Post] status to[%s]", StateString(NewStatus));
            else
                UFC::BufferedLog::Printf( " Set trading session[%s] status to[%s]",APCode.c_str(), StateString(NewStatus));
        }    
    }    
}
//---------------------------------------------------------------------------------------------------------------
//
//  Receive control command from Speedy Center
//
//------------------------------------------------------------------------------
void CommandListener::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{
    MTree      ReplyData;
    Int32      CMD;

    if( Data->get( COMMAND_STRING, CMD ))
    {
      switch( CMD )
      {
         case AGENT_ACK:           ReplyData.append( COMMAND_STRING, (Int32)AGENT_REPLY);
                                   MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, ReplyData, FALSE );
                                   break;
         case AGENT_REQUEST_STATE: ReplyPVCState( Data );
                                   break;
         case AGENT_LISTEN_PVC:    ListenPVC( Data );
                                   break;
         case AGENT_PVC_ENABLED:   EnablePVC(  Data  );
                                   break;
         case AGENT_CHANGE_APCODE: SetAPCode( Data );
                                   break;	
         case AGENT_SET_SEQUENCE:  SetFIXSequence( Data );
                                   break; 
         case AGENT_REJECT_ALL_ORD:RejectAllOrders( Data );
                                   break;
         case AGENT_SESSION_STATUS:SetTradingSessionStatus( Data );
                                   break; 
      }
    }
}
//------------------------------------------------------------------------------
// 
//  Receive Sync execution messages from None Speedy system
//
//------------------------------------------------------------------------------
TWSE::OtherExecutionListener::OtherExecutionListener( MarketType mkt )
:FMarket( mkt )
{
    switch( FMarket )
    {
        case mtTSE: FConfirmSubject = "ORDER.CONFIRM.TSE";
                    FFillSubject = "ORDER.FILL.TSE";
                    break;
        case mtOTC:FConfirmSubject = "ORDER.CONFIRM.OTC";
                    FFillSubject = "ORDER.FILL.OTC";
                    break;
        case mtES:  FConfirmSubject = "ORDER.CONFIRM.ES";
                    FFillSubject = "ORDER.FILL.ES";
                    break;
        default:break;
    }
}
//----------------------------------------------------------------------------------------------------------------------
void TWSE::OtherExecutionListener::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString&, MTree* Data )
{
    UFC::AnsiString Order,BrokerID,Key,ExTime,PVCID;
    Int32 IsConfirm,NID;
    
    if( Data->get( "ORDER", Order ) == TRUE &&  Data->get( "PVCID", PVCID ) == TRUE && Data->get( "PBNO", BrokerID ) == TRUE && 
        Data->get( "CONFIRM", IsConfirm ) == TRUE && Data->get( "NID", NID ) == TRUE &&  Data->get( "EXTIME", ExTime ) == TRUE &&
        Data->get( "KEY", Key ) == TRUE )
    {
        if( IsConfirm == true )
        {
            UFC::BufferedLog::Printf( " ----------------- Other system Confirm --------------------"  );                
            UFC::BufferedLog::Printf( " Confirm[%s]",Order.c_str() );                        
        }
        else
        {
            UFC::BufferedLog::Printf( " ----------------- Other system Filled ---------------------"  );            
            UFC::BufferedLog::Printf( " Filled[%s]",Order.c_str() );                        
        }
        UFC::BufferedLog::Printf( " NID[%010u] Time[%s]", NID, ExTime.c_str() );                        
        UFC::BufferedLog::Printf( " BrokerID[%s] PVC[%s]", BrokerID.c_str(), PVCID.c_str() );       
        UFC::BufferedLog::Printf( " Key[%s]", Key.c_str() );       
        UFC::BufferedLog::Printf( " -----------------------------------------------------------"  );        
        if( IsConfirm == true )
            ObjectManager->OtherSysExecution( FConfirmSubject, Key, NID, Order, PVCID, BrokerID, ExTime, IsConfirm  );        
        else
            ObjectManager->OtherSysExecution( FFillSubject, Key, NID, Order, PVCID, BrokerID, ExTime, IsConfirm  );        
    }
}   
//------------------------------------------------------------------------------
// 
//  Receive Sync message from other Speedy Server
//
//------------------------------------------------------------------------------
TWSE::SyncListener::SyncListener( TWSE::SpeedyObjectsManager* ObjManager )
:FObjectsManager( ObjManager )
{
}
//------------------------------------------------------------------------------
void TWSE::SyncListener::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{
    UFC::AnsiString logHead = "TWSE::SyncListener::OnMigoMessage()";
    UFC::BufferedLog::Printf( " %s [%s][%s]", logHead.c_str(), Subject.c_str(), Key.c_str());
    UFC::BufferedLog::FlushToFile();

    Int32 NID, Seq, CMD, Func, Type;
    UFC::AnsiString LOG, KEY, OID, FromHost, BrokerID;
    if( ( Data->get( "CMD", CMD ) == TRUE ) && ( Data->get( "SEQ", Seq ) == TRUE ) && ( Data->get( "HOST", FromHost ) == TRUE ) )
    {
        UFC::BufferedLog::Printf( " %s CMD[%d] Seq[%d] FromHost[%s]", logHead.c_str(), CMD, Seq, FromHost.c_str());
        UFC::BufferedLog::FlushToFile();
        if( Data->get( "NID", NID ) == TRUE )
        {
            UFC::BufferedLog::Printf( " %s NID[%d]", logHead.c_str(), NID);
            UFC::BufferedLog::FlushToFile();
        }
        else
        {
            NID = 0;
            UFC::BufferedLog::Printf( " %s Miss node NID", logHead.c_str());
            UFC::BufferedLog::FlushToFile();
        }

        if( CMD == 1 ) ///< Received a execution from the Speedy on the server "FromHost".
        {
            UFC::BufferedLog::Printf( " %s Process CMD[%d]", logHead.c_str(), CMD);
            UFC::BufferedLog::FlushToFile();
            ///< It's a new order, Add OrderID, Key, NID to OrderInfoStore.
            if( ( NID != 0 )                       && 
                Data->get( "Key",    KEY ) == TRUE && 
                Data->get( "OID",    OID ) == TRUE && 
                Data->get( "FUNC",   Func) == TRUE &&                    
                Data->get( "TYPE",   Type) == TRUE &&                    
                Data->get( "Broker", BrokerID) == TRUE  )                    
            {
                UFC::BufferedLog::Printf( " %s Add Order KEY[%s] OID[%s] Func[%d] Type[%d] BrokerID[%s]", logHead.c_str(), KEY.c_str(), OID.c_str(), Func, Type, BrokerID.c_str());
                UFC::BufferedLog::FlushToFile();
                FObjectsManager->OrderInfo.AddOrder( OID, BrokerID, Func, Type, NID, KEY, FromHost );
            }
            
            ///< Write this record to log.
            if( Data->get( "Log", LOG ) == TRUE )
            {
                UFC::BufferedLog::DebugPrintf( UFC::dlPerformance, " %s Recv Log[%s] from Host[%s]", logHead.c_str(), LOG.c_str(), FromHost.c_str() );
                UFC::BufferedLog::FlushToFile();
                FObjectsManager->OrderInfo.WriteExecutionLog( Seq, FromHost, LOG );
//RM
                if( ObjectManager->IsRiskManagerEnabled() && ( ObjectManager->FRMObjPtr != 0 ) )
                {
                    ObjectManager->FRMObjPtr->ParseExecRtpLogStr(LOG, true);
                }
#ifdef __RMLib
                if ( rm::IBasicRiskManagerObject::IsRMControlSetting() ) rm::rmObjPtr->ParseExecRtpLogStr(LOG, true);
#endif
//RM
            }
            else
            {
                UFC::BufferedLog::Printf( " %s Miss node Log", logHead.c_str());
                UFC::BufferedLog::FlushToFile();
            }
        }
        else if( CMD == 0 )///< The Speedy on the server "FromHost" want to get executions from the "Seq" to the end.
        {
            UFC::BufferedLog::Printf( " %s Process CMD[%d]", logHead.c_str(), CMD);
            UFC::BufferedLog::FlushToFile();
            if( Key.AnsiCompare( UFC::Hostname ) == 0 )///< Ask me !!
            {
                UFC::BufferedLog::Printf( " [%s] ask me to recover Executions to seq:%d", FromHost.c_str(), Seq  );
                FObjectsManager->OrderInfo.RecoverExecFromSequence( FromHost, Seq );
            }
        }
        else
            UFC::BufferedLog::Printf( " %s Ignore CMD[%d].", logHead.c_str(), CMD);            
    }    
    else
        UFC::BufferedLog::Printf( " %s Miss node CMD or SEQ or HOST.", logHead.c_str());
    UFC::BufferedLog::FlushToFile();
}
//----------------------------------------------------------------------------------------------------------------------    
void CheckSystemListener::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    MTree                  ReplyData;
    UFC::AnsiString        Value,ReplyKey;
    UFC::UDateTime         Now;


    if( Data->get("ReplyKey",ReplyKey) == TRUE  )
    {
        Now.setCurrent();
        if( IsTSEC )
        {
            ReplyData.append( "Name", "Speedy for TWSE" );
            UFC::SleepMS( 20 );
        }
        else
        {
            ReplyData.append( "Name", "Speedy for OTC" );
            UFC::SleepMS( 25 );
        }
        ReplyData.append( "COUNT", 5 );
        Value.Printf( "inf,Version:%s Build Date:%s ",SPEEDY_TSEC_VER,__DATE__ );
        ReplyData.append( "0", Value );
        if( UFC::Is64Bits()== TRUE )
            ReplyData.append( "1", "inf,64Bit Version" );        
        else
            ReplyData.append( "1", "inf,32Bit Version" );          
        if( Now.getYear()  != StartupTime.getYear() ||
            Now.getMonth() != StartupTime.getMonth() ||
            Now.getDate()  != StartupTime.getDate() )
            Value.Printf( "war,Startup:%04d/%02d/%02d %02d:%02d:%02d", StartupTime.getYear(), StartupTime.getMonth(),StartupTime.getDate(),
                                                                       StartupTime.getHour(), StartupTime.getMinute(), StartupTime.getSecond() );
        else
            Value.Printf( "inf,Startup:%04d/%02d/%02d %02d:%02d:%02d", StartupTime.getYear(), StartupTime.getMonth(),StartupTime.getDate(),
                                                                       StartupTime.getHour(), StartupTime.getMinute(), StartupTime.getSecond() );
        ReplyData.append( "2", Value );
        if( RecoverModeExchange == FALSE )
            ReplyData.append( "3", "inf,Start with clear flag: Yes" );
        else
            ReplyData.append( "3", "war,Start with clear flag: No" );
        ReplyData.append( "4", "inf,Support FIX protocol: Yes" );        

        MessageObj->Send( "CHECK.REPLY", ReplyKey, ReplyData, FALSE );
    }
}
//------------------------------------------------------------------------------
// Check the args for SpeedyTSEC
//------------------------------------------------------------------------------
void CheckArgs( char *argv[], int argc )
{
    MBusPort = MApp::MAPP_DEF_PORT;

    for( int i = 1; i < argc; i++ )
    {
         UFC::AnsiString Param( argv[i] );
         UFC::AnsiString upperParam = Param.UpperCase();
         if( Param == "-H" || Param == "-h" || Param == "-?" )
         {
             printf( "Speedy TSEC/OTC TCP/IP link program Copyright(c) 2006~%d MDBS \n", UFC::ThisYear());
             printf( "Version:%s Build Date:%s %s\n", SPEEDY_TSEC_VER, __DATE__, __TIME__ );             
             printf( "Usage: SpeedyTSEC [options]\n" );
             printf( "       -H         Show this page.\n" );
             printf( "       -AO        Sent not Speedy fill to AE[Others].\n" );                          
             printf( "       -d(fipw)   Debug mode switch.\n" );
             printf( "                        f: print fatal error debug information.\n" );
             printf( "                        i: print debug information.\n" );
             printf( "                        p: print performance debug information.\n" );
             printf( "                        w: print warning debug information.\n" );             
             printf( "       -D(Name)   Dependency process MBus name. (Default SpeedyAgent)\n" );             
             printf( "       -F         First time startup.( Reset the filled report seq,\n" );
             printf( "                                       and the filled report seq table ).\n" );             
             printf( "       -M(Market) T for TSE O for OTC.(Default T)\n" );             
             printf( "       -OS(Size)  Order data size in queue.(Default:%d).\n", MAX_DATA_SIZE );
             printf( "       -P(Port)   Service port.(default 12345)\n" );
             printf( "       -R(Ratio)  Throughput control rate.(for FIX only)\n" );
             printf( "       -SF        Sync fill PVC data to cluster server.\n" );
             printf( "       -T(ms)     Order expired time.(default 300000 ms)\n" );             
             printf( "       -TWSEOLD   Use TWSE Old version.(Before 2020/3/02)\n" );
             printf( "       -TWSE302   Use TWSE version.(Between 3/23 and 3/02)\n" );
             printf( "       -XMG       Disable Market order Group.\n" );
             printf( "       -XTRY      Disable try order before market open.\n" );
             printf( " Options for TWSE FIX\n" );             
             printf( "       -FSTS             Enable FIX Special Trading session .\n" );             
             printf( "       -FIXMT            Use multi-thread FIX engine.(default single thread)\n" );             
             printf( "       -FIXFS            Use file store to keep FIX message.(default memory store)\n" );                                       
             printf( "       -FIXNS            Use Null store to keep FIX message.(not store)\n" );                                                    
             printf( "       -FIXROL           Sequence numbers should be reset before sending/receiving a logon request.(default N)\n" );                                                                 
             printf( "       -RPXQP            Add 58=[Qty,Px] to FIX Replace message.\n" );                      
             if( sVersion > svStandard )
             {
                printf( " Options for Speedy Platinum Version\n" );
                printf( "       -P_EDQ                Send Execution to queue directly.\n" );
             }             
             exit( 1 );
         }         
         else if( Param == "-XTRY" || Param == "-xtry" ) 
         {
            SkipTryOrder = TRUE;
         }
         else if( Param == "-XMG" || Param == "-xmg" )
         {
            MareketOrderGroup = 0;
         }   
         else if( Param == "-TWSEOLD" || Param == "-twseold" )
         {
             TWSEVersion = 1;
         }
         else if( Param == "-TWSE302" || Param == "-twse302" )
         {
             TWSEVersion = 2;
         }
	 else if( Param == "-P_EDQ" )
         {
             ExecToQueue = TRUE;
         }
         else if( Param == "-F" || Param == "-f" )
         {
             RecoverModeExchange = FALSE;
         }
         else if( Param == "-FSTS" )
         {
             DynamicSessionFIX = TRUE;
         }
         else if( Param == "-SF" || Param == "-sf" )
         {
            SyncFillPVC = TRUE;
         }
         else if( Param.AnsiPos("-P") != -1 || Param.AnsiPos("-p") != -1 )
         {
             UFC::AnsiString PortStr( (const char*)(Param.c_str() + 2) );
             MBusPort = PortStr.ToInt();
         }
         else if( Param.AnsiPos("-T") != -1 || Param.AnsiPos("-t") != -1 )
         {
             UFC::AnsiString Sec( (const char*)(Param.c_str() + 2) );
             ExpiredMS = Sec.ToInt( );
         }
         else if( Param.AnsiPos("-M") != -1 || Param.AnsiPos("-m") != -1)
         {
             if( Param[2] == 'o' || Param[2] == 'O' )
                 IsTSEC = FALSE;
             else    
                 IsTSEC = TRUE;      
             //TWSE::MessageHeader::SetTradeSystem( IsTSEC );
	 }
         else if( Param == "-RPXQP" )
         {
            EnableRPXText = TRUE;
         }
         else if( Param.AnsiPos("-R") != -1 )
         {
             UFC::AnsiString TPRatio( (const char*)(Param.c_str() + 2) );
             ThroughputRatio = TPRatio.ToDouble();
         }
         else if( Param.AnsiPos("-d") !=-1 )
         {
             if( Param.AnsiPos("f")!=-1 )
                 DebugLevel |= UFC::dlFatalError;
             if( Param.AnsiPos("i")!=-1 )
                 DebugLevel |= UFC::dlInformation;
             if( Param.AnsiPos("p")!=-1 )
             {
                 DebugLevel |= UFC::dlPerformance;
                 BENCHMARK_MODE = TRUE;
             }
             if( Param.AnsiPos("w")!=-1 )        
                 DebugLevel |= UFC::dlWarning;
         }
         else if( Param.AnsiPos("-D") != -1 )
         {
             UFC::AnsiString PName( (const char*)(Param.c_str() + 2) );
             DEPENDENCY_PROCESS = PName;
         }
         else if( Param.AnsiPos("-OS") != -1 || Param.AnsiPos("-os") != -1 )
         {
             UFC::AnsiString QSStr( (const char*)(Param.c_str() + 3) );
             int QSize = QSStr.ToInt();
             
             if( QSize < 256 )
                 MAX_DATA_SIZE = 256;
             else if( QSize > 4096 )
                 MAX_DATA_SIZE = 4096;
             else 
                 MAX_DATA_SIZE = QSize;
         }
         else if( Param == "-AO" || Param == "-ao" )
         {
            NotSpeedyToOthers = TRUE;
         }
         else if( Param == "-FIXMT" )
         {
            FIXMultiThread  = TRUE; ///< Use Multi-thread Initiator.
         }
         else if( Param == "-FIXFS" )
         {
             FIXStoreType  = 2; ///< Use file store.                
         }
         else if( Param == "-FIXNS" )
         {
             FIXStoreType  = 0; ///< Use Null store.                
         }
         else if( Param == "-FIXROL" )
         {
            FIXResetOnLogon = TRUE;
         }
//RM
         else if( upperParam == "-NODATE" )
         {
#ifdef __RMLib
            rm::CMegaRiskManagerObject::SetNeedCheckRiskDataDate(false);
#endif
         }
#ifdef __RMLib
        rm::SetRiskLogLevel(DebugLevel);
#endif
//RM
     }
}
//------------------------------------------------------------------------------
// Monitoring SpeedyTSECs on other Speedy servers.
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
                    if( IsTSEC )
                        MessageObj->AddMonitoringProcess( HostSection->GetSectionName(),"SpeedyTSEC");
                    else
                        MessageObj->AddMonitoringProcess( HostSection->GetSectionName(),"SpeedyOTC");        
                }
            }
        }
    }	
    MessageObj->AddMonitoringProcess( UFC::Hostname, DEPENDENCY_PROCESS );        
}
//------------------------------------------------------------------------------
void CheckLicenseKey( void )
{
    if( !(UFC::FileExists("License")) )
    {
        printf( " License file not found.\n" );
        exit(1);
    }    	
    UFC::AnsiString Today, Plat, Date, Key, LicenseName;
    UFC::AnsiString ThisHost( UFC::Hostname );
    UFC::UiniFile iniFile( "License" );
    
    UFC::GetYYYYMMDD( Today );
#ifdef _WIN32
	Plat = "WIN32";
#endif
#ifdef __SOLARIS
	Plat = "SOLARIS";
#endif
#ifdef __AIX
	Plat = "AIX";
#endif
#ifdef __LINUX
	Plat = "LINUX";
#endif
    LicenseName = "SpeedyTWSEPro";
    if( iniFile.GetValue( LicenseName, "Date", Date ) == TRUE && 
        iniFile.GetValue( LicenseName, "Key", Key ) == TRUE )
    {
        if( Date.ToInt() >= Today.ToInt() )
    	{
            UFC::TLicenseKey LicenseKey( Plat, LicenseName, ThisHost, Date );
            if( LicenseKey.CompareKey( Key ) )
            {
                sVersion = svProfessional;
                UFC::Profiler::EnableProfiler( TRUE );
                printf( "*Get license for Speedy Platinum version.\n" );
                return;
            }
            else
                printf( "*Bad Speedy Platinum license key.\n" );
    	}
    	else
            printf( "*Speedy Platinum license expired.\n" );
    }
    else
    {
        UFC::Profiler::EnableProfiler( FALSE );
    	printf( "*Get license for Speedy standard version.\n" );
    }
}   
//------------------------------------------------------------------------------
// You should known "main"!!
//------------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
    ProcessExistEventPtr = new UFC::PEvent();
    OrderRecodPoolPtr = new UFC::PThreadObjectPool<OrderRecord>( 4096, 512 );
    CheckLicenseKey( );
    CheckArgs( argv, argc );
    
    UFC::AnsiString currentDateStr;
    UFC::GetYYYYMMDD(currentDateStr, FALSE);  //Without Slash
    if( IsTSEC == TRUE )
        UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( "../log/SpeedyTSEScreen" + currentDateStr, 1048576, TRUE ));
    else
        UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( "../log/SpeedyOTCScreen" + currentDateStr, 1048576, TRUE ));
    UFC::BufferedLog::Printf( "_______________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    if( sVersion == svStandard )
        UFC::BufferedLog::Printf( "   Speedy Standard,TWSE/OTC Trading module." );
    else if( sVersion == svProfessional)
        UFC::BufferedLog::Printf( "   Speedy Platinum,The most powerful Speedy." );
    UFC::BufferedLog::Printf( "   Copyright(c) 2006-%d by MDBS Software Inc.     ", UFC::ThisYear());
    UFC::BufferedLog::Printf( "   %d bit mode                 ", sizeof(void*)*8 );                    
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
#ifdef __RMLib
    UFC::BufferedLog::Printf( "   RM Lib Version." );
#endif  
    UFC::BufferedLog::Printf( "   Version:%s Build Date:%s %s", SPEEDY_TSEC_VER, __DATE__, __TIME__ );
    
    if( TWSEVersion == 0 )
    {
        UFC::BufferedLog::Printf( "   TWSE New Version   :(after 2020/03/23)" );        
        TWSE::MessageHeader::SetNewVersion( TRUE, TRUE  );        
    }
    else if( TWSEVersion == 1 )
    {
        UFC::BufferedLog::Printf( "   TWSE old Version   :(before 2020/03/23)" );        
        TWSE::MessageHeader::SetNewVersion( FALSE, FALSE  );        
    }
    else
    {
        UFC::BufferedLog::Printf( "   TWSE old Version   :(between 2020/03/02 and 03/23)" );        
        TWSE::MessageHeader::SetNewVersion( FALSE, TRUE  );                
    }
    if( IsTSEC == TRUE )
        UFC::BufferedLog::Printf( "   For TSE market" );
    else
        UFC::BufferedLog::Printf( "   For OTC market" );
    if( RecoverModeExchange == TRUE )
        UFC::BufferedLog::Printf( "   Recover Mode : On" );
    else
        UFC::BufferedLog::Printf( "   Recover Mode : Off" );    
    if( MareketOrderGroup == TRUE )
        UFC::BufferedLog::Printf( "   Market Group : On" );
    else
        UFC::BufferedLog::Printf( "   Market Group : Off" );    
    if( DynamicSessionFIX == FALSE )
        UFC::BufferedLog::Printf( "   Special Trading Session : TMP" );
    else
        UFC::BufferedLog::Printf( "   Special Trading Session : FIX" );
    if( SkipTryOrder == FALSE )
        UFC::BufferedLog::Printf( "   Try order : On" );
    else
        UFC::BufferedLog::Printf( "   Try order : Off" );
    if( SyncFillPVC == TRUE )
        UFC::BufferedLog::Printf( "   Sync Fill PVC: On" );
    if( ExecToQueue == TRUE )
        UFC::BufferedLog::Printf( "   Send execution to Q: On" );
    UFC::BufferedLog::Printf( "   Order  size: %d bytes", MAX_DATA_SIZE );    
    UFC::BufferedLog::Printf( "_______________________________________________" );   
    
    
    
    UFC::BufferedLog::SetDebugMode( DebugLevel );
    if( IsTSEC == TRUE )    
        MessageObj = new MessageObject( "SpeedyTSEC", "1.0", "FCM system.",MBusPort );   ///< Create Migo Message object.            
    else
        MessageObj = new MessageObject( "SpeedyOTC", "1.0", "FCM system.",MBusPort );   ///< Create Migo Message object.    
    MessageObj->AddListener( COMMAND_SUBJECT, UFC::Hostname,  new CommandListener());///< Add State query Message listener to MessageObject.
    MessageObj->AddListener( "SYSTEM.CHECK", UFC::Hostname, new CheckSystemListener() );        
    if( IsTSEC == FALSE )
    {
        MessageObj->AddListener( "OTHER.EXEC.OTC", UFC::Hostname, new TWSE::OtherExecutionListener( mtOTC ) );
        MessageObj->AddListener( "OTHER.EXEC.ES", UFC::Hostname, new TWSE::OtherExecutionListener( mtES ) );
    }
    else    
        MessageObj->AddListener( "OTHER.EXEC.TSE", UFC::Hostname, new TWSE::OtherExecutionListener( mtTSE ) );
    ObjectManager = new TWSE::SpeedyObjectsManager( IsTSEC );    
    MessageObj->SetMonitorListener( new ProcessMonitor( ObjectManager, IsTSEC ) );    
    MessageObj->Start(); ///< Start Migo Message pump.                       
    ProcessExistEventPtr->WaitFor( 10 );
    UFC::BufferedLog::Printf( " MBus Connected. Ask the dependency process state." );
    MonitoringOtherSpeedy(); ///< Monitoring Speedys on other servers.    
    ObjectManager->CreateConnectionObjects();
    ObjectManager->Run();    
    delete ObjectManager;
    return 1;
}
//------------------------------------------------------------------------------
