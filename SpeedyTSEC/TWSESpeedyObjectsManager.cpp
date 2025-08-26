#include "TFixedFormat.h"
#include "TParseData.h"
#include "TWSESpeedyObjectsManager.h"
#include "Speedy.h"
#include "QueryStockPVC.h"
#include "Profiler.h"
#include "../ExchangeUtility/Utility.h"
#include "../TSECLib/LinkSubSystem.h"
#include "../SpeedyGateway/Administrator.h"

#ifdef __RMLib
#include "../TSECLib/USunRiskManager.h"  //RM
#include "../TSECLib/MegaRiskManager.h"  //RM
#endif
//-----------------------------------------------------------------------------------------------------------------------
//
//  FCM side( FCMObjectsManeger )
//
//  Objects Manager for AP code Order/Execution subsystem
//
//-----------------------------------------------------------------------------------------------------------------------
extern BOOL            RecoverModeExchange;
extern BOOL            MareketOrderGroup;  
extern BOOL            SkipTryOrder;
extern Int32           TransMax;
extern Int32           MAX_DATA_SIZE;
extern BOOL            ExecToQueue;
extern BOOL            SyncFillPVC;
extern BOOL            NotSpeedyToOthers;
extern BOOL            ExtremeVersion;
extern BOOL            UseLockLessQueue; 
extern BOOL            BENCHMARK_MODE;
extern BOOL            DynamicSessionFIX;
extern UFC::AnsiString OTHER_AE;
extern TWSE::ExecutionListener* gTSEConfirm;
extern TWSE::ExecutionListener* gOTCConfirm;
extern UFC::PMPMCQueue<OrderRecord>* TWSEQueue; 
extern UFC::PMPMCQueue<OrderRecord>* OTCQueue;
extern UFC::PThreadObjectPool<OrderRecord>* OrderRecodPoolPtr;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//---------------------------------------------------------------------------
UFC::AnsiString DescribeMarketStatus(TMarketStatus MarketStatus)
{
    UFC::AnsiString marketStatusStr = "";
    switch(MarketStatus)
    {
        case msWait:     marketStatusStr = "Wait";     break;  // 0
        case msTryOrder: marketStatusStr = "TryOrder"; break;  // 1
        case msOpened:   marketStatusStr = "Opened";   break;  // 2
        case msClosed:   marketStatusStr = "Closed";   break;  // 3
        case msStop:     marketStatusStr = "Stop";     break;  // 4
        default:         marketStatusStr = "UnKnown";  break;
    }
    return marketStatusStr;
}  //DescribeMarketStatus()
//---------------------------------------------------------------------------
UFC::AnsiString DescribeMarketType(TMarketType MarketType)
{
    UFC::AnsiString marketTypeStr = "";
    switch(MarketType)
    {
        case mtNormal:      marketTypeStr = "Normal";      break;
        case mtOddTrade:    marketTypeStr = "Odd-Post";    break;
        case mtOddTradeEx:  marketTypeStr = "Odd";         break;
        case mtPostTrade:   marketTypeStr = "Post";        break;
        case mtLend:        marketTypeStr = "Lend";        break;
        case mtTender:      marketTypeStr = "Tender";      break;
        case mtAuction:     marketTypeStr = "Auction";     break;
        case mtTenderEx:    marketTypeStr = "TenderEx";    break;
        case mtNegotiatePx: marketTypeStr = "NegotiatePx"; break;
        default:            marketTypeStr = "UnKnown";     break;
    }
    return marketTypeStr;
}  //DescribeMarketType()
//---------------------------------------------------------------------------
//
// Class TransferOrderThread
// Order transfer thread.
// Transfer all orders in queue to backup server.
//
//---------------------------------------------------------------------------
TransferOrderThread::TransferOrderThread( char AP, UFC::AnsiString& brokerID, SpeedyObjectsManager* ObjectsManager  )
:UFC::PThread(  NULL, TRUE  )
,FSystem( ObjectsManager->ToSubsystemID( AP ) )
,FBroker( brokerID.SubString(0,2).ToInt() )
,FObjectsManager( ObjectsManager )
{
    Start();
}
//---------------------------------------------------------------------------
void TransferOrderThread::Execute( void )
{
    int TransCount = 0;
    
    UFC::BufferedLog::Printf( " All trading session %d PVC broken. Start a TransferOrderThread.", FSystem );
    if( UseLockLessQueue == false )
        TransCount = FObjectsManager->TransferAll( FBroker, FSystem  );
    else
        TransCount = FObjectsManager->TransferAllEx( FBroker, FSystem  );
    UFC::BufferedLog::Printf( " Transfer %d Orders to AA Speedy.", TransCount );
    UFC::BufferedLog::FlushToFile();
}
//-----------------------------------------------------------------------------------------------------------------------
//
// Class SpeedyObjectsManager
//
// Object to manage all connection objects. 
//-----------------------------------------------------------------------------------------------------------------------
SpeedyObjectsManager::SpeedyObjectsManager( BOOL IsTWSE )
:OrderLog( NULL )
,FillLog( NULL )
,OrderQueueGroupCount( 1 )
,FTotalOrder( 0 )
,FTotalTime( 0 )
,FMaxTime( 0 )
,FMsgCount( 0 )
,FFIXReadyCount( 0 )
,FPrevTime( 0 )
,FStockTable( NULL )
,FRushPVCCount( 256 )
,FOverLimitStop( true )
,FDealerID( "0000")
,FModifyTime( 0 )
,FFIXManager( NULL )
,FIsTWSE( IsTWSE )
,FLoopCount( 0 )
//RM
,FIsRiskManagerEnabled( false )
,FRiskManagerID( "" )
,FRiskManagerConfigureFilePathName( "" )
,FRMObjPtr( 0 )
#ifdef __RMLib
,FRMName("")
,FRMConfigFileName("")
#endif
//RM
,OrderInfo( TransMax ,IsTWSE,RecoverModeExchange )
,ExecutionQueue( FALSE )
,FBrokerID2( 0 )
{    
    UFC::AnsiString MarketName;    
    bool isQueueOpenSuccess = false;
    
    if( FIsTWSE == TRUE )
    {   
        FCfgFileName    = "../cfg/SpeedyTSEC.cfg";
        MarketName      = "TSE";
        FMPMCQueue    = TWSEQueue; 
        FProfileID    = UFC::psTWTSE;
        try
        {
            UFC::BufferedLog::Printf( " SpeedyObjectsManager::SpeedyObjectsManager() Try to open TWSE Order Queue." );
            isQueueOpenSuccess = FQueue.Open( STOCK_ORD_QUEUE, TRUE );
        }
        catch( UFC::QueueException& QueuueEx )
        {   ///< Queue fulled(Up to the system limitation), Reject to client.(As a order reject )
            isQueueOpenSuccess = false;
            UFC::BufferedLog::Printf( " SpeedyObjectsManager::SpeedyObjectsManager() Open TSE Order QueueException:[%s]", QueuueEx.what() );
        }
        if (!isQueueOpenSuccess)
        {
            UFC::BufferedLog::Printf( " Open TSE Order queue failed.");
            UFC::BufferedLog::FlushToFile();
            exit( 0 );
        }                    
        if( ExecToQueue == TRUE ) ///< Executions direct to queue.
        {
            try 
            {
                isQueueOpenSuccess = ExecutionQueue.Open( STOCK_ORD_QUEUE + 1, TRUE );
            }
            catch( UFC::QueueException& QueuueEx )
            {   ///< Queue fulled(Up to the system limitation), Reject to client.(As a order reject )
                isQueueOpenSuccess = false;
                UFC::BufferedLog::Printf( " SpeedyObjectsManager::SpeedyObjectsManager() Open TWSE Execution QueueException:[%s]", QueuueEx.what() );
            }
            if (!isQueueOpenSuccess)
            {
                UFC::BufferedLog::Printf( " Open TWSE Execution queue failed." );
                UFC::BufferedLog::FlushToFile();
                exit( 0 );
            }
        }  //if( ExecToQueue == TRUE )        
        FStockTable = new Int16[ 9999 ];
        for( register  int i = 0; i < 9999; i++ )
             FStockTable[ i ] = 0;
    }
    else
    {
        FCfgFileName  = "../cfg/SpeedyOTC.cfg";        
        MarketName      = "OTC";
        FMPMCQueue    = OTCQueue; 
        FProfileID    = UFC::psTWOTC;
        try
        {
            UFC::BufferedLog::Printf( " SpeedyObjectsManager::SpeedyObjectsManager() Try to open OTC Order Queue." );
            isQueueOpenSuccess = FQueue.Open( OTC_ORD_QUEUE, TRUE );
        }
        catch( UFC::QueueException& QueuueEx )
        {   ///< Queue fulled(Up to the system limitation), Reject to client.(As a order reject )
            isQueueOpenSuccess = false;
            UFC::BufferedLog::Printf( " SpeedyObjectsManager::SpeedyObjectsManager() Open OTC Order QueueException:[%s]", QueuueEx.what() );
        }
        if (!isQueueOpenSuccess)
        {
            UFC::BufferedLog::Printf( " Open OTC Order queue failed.");
            UFC::BufferedLog::FlushToFile();
            exit( 0 );
        }
        
        if( ExecToQueue == TRUE ) ///< Executions direct to queue.
        {
            try 
            {
                isQueueOpenSuccess = ExecutionQueue.Open( OTC_ORD_QUEUE + 1, TRUE );
            }
            catch( UFC::QueueException& QueuueEx )
            {   ///< Queue fulled(Up to the system limitation), Reject to client.(As a order reject )
                isQueueOpenSuccess = false;
                UFC::BufferedLog::Printf( " SpeedyObjectsManager::SpeedyObjectsManager() Open OTC Execution QueueException:[%s]", QueuueEx.what() );
            }
            if (!isQueueOpenSuccess)
            {
                UFC::BufferedLog::Printf( " Open OTC Execution queue failed." );
                UFC::BufferedLog::FlushToFile();
                exit( 0 );
            }
        }
    }  
    FOrderSubject.Printf( "ORDER.%s", MarketName.c_str());
    FOrderConfirmSubject.Printf( "ORDER.CONFIRM.%s", MarketName.c_str());
    FFillReportSubject.Printf( "ORDER.FILL.%s", MarketName.c_str());
    FNotSpeedyFillSubject.Printf( "NOTSPEEDY.FILL.%s", MarketName.c_str());
    FSyncSubject.Printf( "SYNC.%s", MarketName.c_str());    
    FSimPVCSubject.Printf( "SIMFILL.%s", MarketName.c_str());
    
    OrderLog = new UFC::BufferedLog( "Speedy Trading Log for " + MarketName,"log", MarketName+".x","4", 2097152, TRUE, TRUE );
    FillLog  = new UFC::BufferedLog( "Speedy Execution Log for " + MarketName,"log", MarketName+".x","3", 2097152, TRUE, TRUE );            
    if( RecoverModeExchange == FALSE ) ///< Clear Orders in queue
        FQueue.Clear();   
    if( MessageObj != NULL )
    {
        MessageObj->AddListener( FOrderSubject, UFC::Hostname, this ); ///< Add Order Message listener.Use hostname as key.
        UFC::BufferedLog::Printf( " SpeedyObjectsManager::SpeedyObjectsManager() Add Listener::[%s][%s]", FOrderSubject.c_str(), UFC::Hostname );
        MessageObj->AddListener( FSimPVCSubject, "all", new SimPVCListener( this ) ); ///< Add Order Message listener.Use hostname as key.        
        UFC::BufferedLog::Printf( " SpeedyObjectsManager::SpeedyObjectsManager() Add SimPVCListener::[%s][all]", FSimPVCSubject.c_str() );
        MessageObj->AddListener( FSyncSubject, UFC::Hostname, new SyncListener( this ));   ///< Add execution report sync. listener to MessageObject.            
        UFC::BufferedLog::Printf( " SpeedyObjectsManager::SpeedyObjectsManager() Add SyncListener:[%s][all]", FSyncSubject.c_str() );
    }
    RemoveOldLogFile();
    
    UFC::UiniFile CfgFile( FCfgFileName );
    LoadSettings( CfgFile );    
//RM
    if( (FRiskManagerID.Length() > 0 ) && 
        ( FRiskManagerConfigureFilePathName.Length() > 0 ) && ( UFC::FileExists( FRiskManagerConfigureFilePathName ) == TRUE ) )
    {
        twserm::CTWSERiskManagerClass::SetRiskManagerID( FRiskManagerID );
        twserm::CTWSERiskManagerClass::SetConfigureFilePathName( FRiskManagerConfigureFilePathName );
        FIsRiskManagerEnabled = true;
        if( FIsTWSE == TRUE )
        {
            twserm::CTWSERiskManagerClass::SetTWSE(true);
            UFC::BufferedLog::Printf( " SpeedyObjectsManager::%s() Enable [%s] TSE RiskManager, Configure File[%s]", __func__, FRiskManagerID.c_str(), FRiskManagerConfigureFilePathName.c_str() );
        }
        else
        {
            twserm::CTWSERiskManagerClass::SetTWSE(false);
            UFC::BufferedLog::Printf( " SpeedyObjectsManager::%s() Enable [%s] OTC RiskManager, Configure File[%s]", __func__, FRiskManagerID.c_str(), FRiskManagerConfigureFilePathName.c_str() );
        }
                
        FRMObjPtr = static_cast<twserm::CTWSERiskManagerClass*>(CreateRM(RecoverModeExchange));
    }    
//RM
    ///< Create market schedule objects
    FNormalMarket   = new MarketSchedule( FIsTWSE, mtNormal,    "Normal",  CfgFile, RecoverModeExchange );
    FOddMarket      = new MarketSchedule( FIsTWSE, mtOddTrade,  "Odd",     CfgFile, RecoverModeExchange );
    FOddExMarket    = new MarketSchedule( FIsTWSE, mtOddTradeEx,"OddEx",   CfgFile, RecoverModeExchange );
    FPostMarket     = new MarketSchedule( FIsTWSE, mtPostTrade, "Post",    CfgFile, RecoverModeExchange );
    FLendMarket     = new MarketSchedule( FIsTWSE, mtLend,      "Lend",    CfgFile, RecoverModeExchange );
    FTenderMarket   = new MarketSchedule( FIsTWSE, mtTender,    "Tender",  CfgFile, RecoverModeExchange );    
    FTenderExMarket = new MarketSchedule( FIsTWSE, mtTenderEx,  "TenderEx",CfgFile, RecoverModeExchange );    
    if( FIsTWSE == TRUE )
        FAuctionMarket  = new MarketSchedule( FIsTWSE, mtAuction,  "Auction", CfgFile, RecoverModeExchange );
    else
        FAuctionMarket  = new MarketSchedule( FIsTWSE, mtNegotiatePx,  "NegotiatePx", CfgFile, RecoverModeExchange );
//RM
#ifdef __RMLib
    if ((RecoverModeExchange == TRUE) && rm::IBasicRiskManagerObject::IsRMControlSetting()) rm::rmObjPtr->RecoverRMData(FIsTWSE);
#endif
//RM
    UFC::BufferedLog::Printf( " ------- Load trade session Settings finished -------" );
    UFC::BufferedLog::FlushToFile();
}
//-----------------------------------------------------------------------------------------------------------------------
SpeedyObjectsManager::~SpeedyObjectsManager()
{
    if( OrderLog != NULL )
    	delete OrderLog;
    if( FillLog != NULL )
    	delete FillLog;
}
//---------------------------------------------------------------------------------------------------------------------
int SpeedyObjectsManager::ToSubsystemID( char AP )
{
    switch( AP )
    {
        ///< Normal trade session.
        case '0': return ((FIsTWSE == TRUE )? 30:93); ///< Normal
        case '4': return ((FIsTWSE == TRUE )? 40:94); ///< Odd stock
        case '9': return ((FIsTWSE == TRUE )? 32:98); ///< Post trade        	    
        ///< auto switch trade session.
        case '5': return ((FIsTWSE == TRUE )? 31:90); ///< Lend
        case '6': return ((FIsTWSE == TRUE )? 70:96); ///< TSE Aution /OTC Negotiate Px
        case '7': return ((FIsTWSE == TRUE )? 42:89); ///< TenderEx
        case '8': return ((FIsTWSE == TRUE )? 41:97); ///< Tender Offer
        case 'C': return ((FIsTWSE == TRUE )? 33:83); ///< Tender Offer
        default : return ((FIsTWSE == TRUE )? 10:91); ///< Link sub system
    }
}
//----------------------------------------------------------------------------------------------------------------------
MarketSchedule* SpeedyObjectsManager::GetScheduleObject( char APCode )
{
    switch( APCode )
    {
        case '4': return FOddMarket;
        case '9': return FPostMarket;
        case '5': return FLendMarket;
        case '6': return FAuctionMarket;
        case '7': return FTenderExMarket;
        case '8': return FTenderMarket;
        case 'C': return FOddExMarket;
        case '0': 
        default : return FNormalMarket;
    }
}
//---------------------------------------------------------------------------------------------------------------------
MarketSchedule* SpeedyObjectsManager::GetScheduleObjectBySubsystemID( int Subsys )
{    
    switch( Subsys )
    {              
        case 30:
        case 93: return FNormalMarket;
        case 40:
        case 94: return FOddMarket;
        case 32:
        case 98: return FPostMarket;
        case 31:
        case 90: return FLendMarket;        
        case 70: 
        case 96: return FAuctionMarket; ///< NegotiatePx same as AuctionMarket
        case 41:
        case 97: return FTenderMarket;
        case 42: 
        case 89: return FTenderExMarket;
        case 33: 
        case 83: return FOddExMarket;
        default: return FNormalMarket;
    }    
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::IsOrder( Int32 Type, MarketSchedule*& Market )
{
    switch( Type )    
    {
        case 930100:///< T010 OTC
        case 930200:
        case 930300:
        case 930400:
        case 930500:
        case 930600:
        case 300100:///< T010 TSEC
        case 300200:
        case 300300:
        case 300400:
        case 300500:
        case 300600:Market = FNormalMarket;
                     return TRUE;       
                     
        case 830100:///< 0110 OTC
        case 830200:
        case 830300:
        case 830400:
        case 830500:
        case 330100:///< 0110 TSEC
        case 330200:
        case 330300:
        case 330400:
        case 330500: Market = FOddExMarket;
                     return TRUE;                            
        case 940100:///< O010 OTC
        case 940200:
        case 940300:
        case 940400:
        case 940500:
        case 400100:///< O010 TSEC
        case 400200:
        case 400300:
        case 400400:
        case 400500: Market = FOddMarket;
                     return TRUE;
        case 980100:///< P010 OTC
        case 980200:
        case 980300:
        case 980400:
        case 980500:             
        case 320100:///< P010 TSEC
        case 320200:
        case 320300:
        case 320400:
        case 320500: Market = FPostMarket;                                          
                     return TRUE;        
        case 900100:///< V010 OTC
        case 900200:
        case 900300:
        case 900400:
        case 310100:///< V010 TSEC
        case 310200:
        case 310300:
        case 310400: Market = FLendMarket;                                          
                     return TRUE;                       
        case 700100:///< A010 TSEC
        case 700200:
        case 700300:
        case 700400: Market = FAuctionMarket;                                          
                     return TRUE;                                            
        case 960101:///< S010 OTC
        case 960201:
        case 960301:
        case 960401:
        case 960103:///< S030 OTC
        case 960203:
        case 960303:
        case 960403:            
        case 960503:
        case 960603:            
        case 960105:///< S050 OTC
        case 960205:
        case 960305:
        case 960405:                    
        case 960605:                        
        case 960407:///< S070 OTC
        case 960507:
        case 960607:
        case 960409:///< S090 OTC
        case 960809:
        case 960411:///< S110 OTC
        case 960711:        
        case 960811: Market = FAuctionMarket;  ///< NegotiatePx same as AuctionMarket                                        
                     return TRUE;                                                                    
        case 970100:///< E010 OTC
        case 970200:
        case 970300:
        case 970400:
        case 410100:///< E010 TSEC
        case 410200:
        case 410300:
        case 410400: Market = FTenderMarket;                                          
                     return TRUE;                                            
        case 890100:///< Ex010 OTC
        case 890200:
        case 890300:
        case 890400:                     
        case 420100:///< Ex010 TSEC
        case 420200:
        case 420300:
        case 420400: Market = FTenderExMarket;                                          
                     return TRUE;                       
        default:    UFC::BufferedLog::Printf( " Not an order message. Header:%d", Type );
                    return FALSE;
    }    
}
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::PVCReady( ConnectionParameter* cp )
{
    UFC::AnsiString Key;
    char            AP = cp->GetAPCode();
    
    ///< Normal:'0'   Odd stock:'4' Post trade:'9' OddEx stock:'C'       	     
    ///<
    ///< Auto switch trade session.
    ///< Lend '5'     TSE Aution /OTC Negotiate Px '6'
    ///< TenderEx '7' Tender Offer '8'
    if( AP == '0' || AP == '4' || AP =='5' || AP == '6' || AP == '7' || AP == '8'|| AP == '9'|| AP == 'C')
    {
        Key.Printf("%c%d%s%s",AP,cp->GetLink(), cp->GetPVCID().c_str(), cp->GetBrokerID().c_str() );    
        UFC::PLockObject Lock( FPVCCountCS);  
            
        if( FPVCSet.Exists( Key ) == FALSE )
        {
            FPVCSet.Add( Key );
            return TRUE;
        }        
    }
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::PVCBroken( ConnectionParameter* cp )
{
    UFC::AnsiString Key;
    char            AP = cp->GetAPCode();
        
    ///< Normal:'0'   Odd stock:'4' Post trade:'9' OddEx stock:'C'       	     
    ///<
    ///< Auto switch trade session.
    ///< Lend '5'     TSE Aution /OTC Negotiate Px '6'
    ///< TenderEx '7' Tender Offer '8'
    if( AP == '0' || AP == '4' || AP =='5' || AP == '6' || AP == '7' || AP == '8' || AP == '9'|| AP == 'C')
    {
        Key.Printf("%c%d%s%s", AP,cp->GetLink(), cp->GetPVCID().c_str(), cp->GetBrokerID().c_str());    
        UFC::PLockObject Lock( FPVCCountCS );

        if( FPVCSet.Exists( Key )  == TRUE )
        {
            FPVCSet.Delete( Key );
            return TRUE;
        }
    }
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::LoadRiskManagerSettings( UFC::UiniFile& cfg )
{
    UFC::AnsiString logHead = "LoadRiskManagerSettings()";
    UFC::AnsiString riskManagerSectionName = "RiskManagerControl";
    UFC::Section *rmCtrlSecPtr = cfg.GetSection(riskManagerSectionName);
    if (rmCtrlSecPtr == 0)
    {
        UFC::BufferedLog::Printf(" %s Section[%s] Not Exist.", logHead.c_str(), riskManagerSectionName.c_str());
        return;
    }
    
//RM
    UFC::AnsiString fieldName, fieldValue;
    fieldName = "RiskManagerID";
    if (rmCtrlSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRiskManagerID = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s] Field[%s] Not Exist.", logHead.c_str(), riskManagerSectionName.c_str(), fieldName.c_str());

    fieldName = "RiskManagerConfigureFilePathName";
    if (rmCtrlSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRiskManagerConfigureFilePathName = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s] Field[%s] Not Exist.", logHead.c_str(), riskManagerSectionName.c_str(), fieldName.c_str());

#ifdef __RMLib
    fieldName = "Name";
    if (rmCtrlSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMName = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s] Field[%s] Not Exist.", logHead.c_str(), riskManagerSectionName.c_str(), fieldName.c_str());
    
    fieldName = "ConfigFile";
    if (rmCtrlSecPtr->GetValue(fieldName, fieldValue) == TRUE)
        FRMConfigFileName = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s] Field[%s] Not Exist.", logHead.c_str(), riskManagerSectionName.c_str(), fieldName.c_str());
    if ((FRMName.Length() > 0) && (FRMConfigFileName.Length() > 0))
    {
        if (FRMName == "USun")
        {
            rm::OpenRiskLogFile(FIsTWSE, FRMName, "../log");
            rm::rmObjPtr = new rm::CUSunRiskManagerObject(FRMName, FRMConfigFileName, FIsTWSE);
        }
        
        if (FRMName == "Mega")
        {
            rm::OpenRiskLogFile(FIsTWSE, FRMName, "../log");
            rm::rmObjPtr = new rm::CMegaRiskManagerObject(FRMName, FRMConfigFileName, FIsTWSE);
        }
    }  //if ((FRMName.Length() > 0) && (FRMConfigFileName.Length() > 0))
#endif    

//RM
}  //SpeedyObjectsManager::LoadRiskManagerSettings()
//----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::LoadSettings( UFC::UiniFile& cfg )
{
    UFC::AnsiString Value;
        
    if( cfg.GetValue( "Rush", "MaxPVC", Value ) == TRUE )
        FRushPVCCount = Value.ToInt();    
    if( cfg.GetValue( "FIX", "OnOverLimitStop", Value ) == TRUE )
        FOverLimitStop = Value.ToInt();        
    if( cfg.GetValue( "FIX", "WarnningCode", Value ) == TRUE )
    {
        UFC::PStringList Codes;
        
        UFC::BufferedLog::Printf( " In section [FIX] WarnningCode" );        
        Codes.SetStrings( Value, ",\n" );
        for( int i = 0;i< Codes.ItemCount();i++ )
        {
           FWarningCode.Add( Codes[i] );     
           UFC::BufferedLog::Printf( " - Add Warnning Code[%s]", Codes[i].c_str() );                   
        }
    }
    UFC::BufferedLog::Printf( " In section [FIX] OnOverLimitStop[%d]", FOverLimitStop );
    UFC::BufferedLog::Printf( " In section [Rush] value MaxPVC" );
    UFC::BufferedLog::Printf( " Upto [%d] PVC blocking in one stock.", FRushPVCCount );
    if( FIsTWSE == FALSE && cfg.GetValue( "Default", "Dealer", FDealerID ) == FALSE )
    {
       UFC::BufferedLog::Printf( " Can not get value \"Dealer\" in \"Default\" section.");            
       exit( 0 );
    }

    if( cfg.GetValue( "OddEx", "Group", Value ) == TRUE )
        OddGroup = Value.ToInt();    
    else    
        OddGroup = 0;
    UFC::BufferedLog::Printf( " Intra-day Odd session use Group[%d].", OddGroup );            
    LoadRiskManagerSettings( cfg );  //RM
}  //SpeedyObjectsManager::LoadSettings()
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::Alarm( const UFC::AnsiString& Msg )
{
    if( Msg.Length() > 0 )
    {
        MTree Data;

        Data.append( "COMMAND", 9020 );
        Data.append( "MSG", Msg );
        MessageObj->Send( "SPEEDY.COMMAND", UFC::Hostname, Data, FALSE );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::WarnningStatusCode( const UFC::AnsiString& FIXText )
{
    UFC::AnsiString Code( FIXText.c_str() ,4 );
    
    if( FWarningCode.Exists( Code ) == true )
    {
        Alarm( FIXText );    
        UFC::BufferedLog::Printf( " Alarm Code[%s] Message[%s]", Code.c_str(), FIXText.c_str() );            
    }
}
//----------------------------------------------------------------------------------------------------------------------
TMarketStatus SpeedyObjectsManager::GetMarketStatus( char APCode )
{
    return GetScheduleObject( APCode )->GetStatus();        
}
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::CanITry( char APCode )
{
    if( SkipTryOrder == true )
        return false;
    return GetScheduleObject( APCode )->CanITry();        
}
//----------------------------------------------------------------------------------------------------------------------
Int32 SpeedyObjectsManager::RetryInterval( char APCode )
{
    return GetScheduleObject( APCode )->FRetryInterval;        
}
//----------------------------------------------------------------------------------------------------------------------
TMarketStatus SpeedyObjectsManager::SetMarketStatus( char APCode, TMarketStatus NewStatus )
{    
    return GetScheduleObject( APCode )->SetStatus( NewStatus );        
}        
//----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::IncreaseLockPVC( Int32 StockID )
{
    if( FStockTable != NULL && StockID < 9999 && StockID > 0 )        
    {
        UFC::PLockObject Lock( FLockPVCCS );
    
        FStockTable[ StockID ] ++;          
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::DecreaseLockPVC( Int32 StockID )
{
    if( FStockTable != NULL && StockID < 9999 && StockID > 0 )        
    {
        UFC::PLockObject Lock( FLockPVCCS );
        
        FStockTable[ StockID ] --;
        if( FStockTable[ StockID ] < 0 )
            FStockTable[ StockID ] = 0;
    }
}
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::IsStockRush( Int32 StockID )
{
    if( StockID >= 9999 || StockID <= 0 )
        return FALSE;
    
    if( FStockTable != NULL && FStockTable[ StockID ] >= FRushPVCCount )
    {
        UFC::BufferedLog::Printf( " Stock:%d Lock count:%d.", StockID, FStockTable[ StockID ] );
        return TRUE;   
    }
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::RemoveOldLogFile()
{
     UFC::UDate      Day60Ago;
     UFC::AnsiString DelFile;

     Day60Ago.setCurrent();
     Day60Ago -= 60;
     if( FIsTWSE == TRUE )
     {
        DelFile.Printf( "../log/TSE.x%02d%02d%02d.4",Day60Ago.getYear()-1911,Day60Ago.getMonth(),Day60Ago.getDate());
        unlink( DelFile.c_str() );
        DelFile.Printf( "../log/TSE.x%02d%02d%02d.3",Day60Ago.getYear()-1911,Day60Ago.getMonth(),Day60Ago.getDate());
        unlink( DelFile.c_str() );
        DelFile.Printf( "../log/TSE.x%02d%02d%02d.1.2",Day60Ago.getYear()-1911,Day60Ago.getMonth(),Day60Ago.getDate());
        unlink( DelFile.c_str() );     
        DelFile.Printf( "../log/SpeedyTSEScreen%02d%02d%02d",Day60Ago.getYear()-1911,Day60Ago.getMonth(),Day60Ago.getDate());     
        unlink( DelFile.c_str() );
     }
     else
     {
        DelFile.Printf( "../log/FUT.x%02d%02d%02d.4",Day60Ago.getYear()-1911,Day60Ago.getMonth(),Day60Ago.getDate());
        unlink( DelFile.c_str() );
        DelFile.Printf( "../log/FUT.x%02d%02d%02d.3",Day60Ago.getYear()-1911,Day60Ago.getMonth(),Day60Ago.getDate());
        unlink( DelFile.c_str() );
        DelFile.Printf( "../log/FUT.x%02d%02d%02d.1.2",Day60Ago.getYear()-1911,Day60Ago.getMonth(),Day60Ago.getDate());
        unlink( DelFile.c_str() );
        DelFile.Printf( "../log/SpeedyOTCScreen%02d%02d%02d",Day60Ago.getYear()-1911,Day60Ago.getMonth(),Day60Ago.getDate());     
        unlink( DelFile.c_str() );         
     }
}
//----------------------------------------------------------------------------------------------------------------------
UInt32 SpeedyObjectsManager::UniqueSHMID( ConnectionParameter& cp )
{
    if( cp.GetLocalPort() == -1 )
        return cp.GetLink()*1000 + cp.GetPVC();///< X.25
    else
        return cp.GetPVC()*100000 + cp.GetLocalPort();///< TCP/IP                
}    
//-----------------------------------------------------------------------------------------------------------------------
//  Create all connections for APCode 0,1,2,3,4,9
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::CreateConnectionObjects( int MaxPVC )
{
    TSECLines               Lines( FIsTWSE );
    UFC::AnsiString         LineKey;
    ConnectionObjectBase*   NewConnection;
    UFC::PHashedSet<UInt32> SHMIDSet;
    
    if( Lines.SaveFIXConfig() > 0 )
        FFIXManager = new SpeedyFIXManager( this );
    UFC::SleepMS( 500 );             
    ///< Create all connections for APCode 4 order system.
    UFC::BufferedLog::Printf( " -------- Create connection objects --------" );    
    FSessionindex = 0;
    if( (FFCMOrderPVCCount = Lines.GetLines( '0' ) ) > 0 ) ///< Normal
    {
        FBrokerID2 = Lines.GetBrokerID( 0 ).SubString( 0, 2 ).ToInt();
        for ( int i = 0; i < FFCMOrderPVCCount; i++ )
        {
            ConnectionParameter cp( Lines.GetPort( i ), 
                                    Lines.GetPVC( i ), 
                                    '0', 
                                    Lines.GetBrokerID( i ), 
                                    Lines.GetPassword( i ),
                                    Lines.GetIP( i ),
                                    Lines.GetLocalPort( i ),
                                    Lines.GetPVCID( i ),
                                    Lines.GetActive( i ),
                                    Lines.GetThreughput( i ));
            UInt32              SHMID = UniqueSHMID( cp );
            int                 Group = Lines.GetGroup( i );
            
            if( Group != PVC_ALL_GROUP && Group >= OrderQueueGroupCount-1 )
                OrderQueueGroupCount = Group + 1; 
            if( SHMIDSet.Exists( SHMID ) == FALSE )
            {
                SHMIDSet.Add( SHMID );
                NewConnection = new SpeedyOrderConnectionObject( this, cp, this, this, SHMID, Group, FIsTWSE );
                LineKey.Printf( "%s:%d:%s", Lines.GetBrokerID( i ).c_str(),Lines.GetPort( i ), Lines.GetPVCID(i).c_str() );
                FConnectionObjects.Add( LineKey, NewConnection );                      
                if( cp.IsFIX() == TRUE )
                    FFIXSession.Add( (SpeedyOrderConnectionObject*)NewConnection );
            }
            else
            {
                UFC::BufferedLog::Printf( " Connection BrokerID:%s PVCID:%s duplicate. ", Lines.GetBrokerID( i ).c_str(), Lines.GetPVCID(i).c_str() );
                exit( 0 );
            }
        }
        UFC::BufferedLog::Printf( " Create %d APCode 0 connection objects.", FFCMOrderPVCCount );
    }
    if( (FFCMOrderPVCCount = Lines.GetLines( '4' ) ) > 0 )///< Odd 
    {
        if( FBrokerID2 == 0 )
            FBrokerID2 = Lines.GetBrokerID( 0 ).SubString( 0, 2 ).ToInt();
        for ( int i = 0; i < FFCMOrderPVCCount; i++ )
        {
            ConnectionParameter cp( Lines.GetPort( i ), 
                                    Lines.GetPVC( i ), 
                                    '4', 
                                    Lines.GetBrokerID( i ), 
                                    Lines.GetPassword( i ),
                                    Lines.GetIP( i ),
                                    Lines.GetLocalPort( i ),
                                    Lines.GetPVCID(i),
                                    Lines.GetActive( i ),
                                    Lines.GetThreughput( i ));
            UInt32              SHMID = UniqueSHMID( cp );
            int                 Group = Lines.GetGroup( i );
            
            if( Group != PVC_ALL_GROUP && Group >= OrderQueueGroupCount-1 )
                OrderQueueGroupCount = Group + 1; 
            if( SHMIDSet.Exists( SHMID ) == FALSE )
            {
                SHMIDSet.Add( SHMID );
                NewConnection = new SpeedyOrderConnectionObject( this, cp, this, this, SHMID, Group, FIsTWSE ) ;
                LineKey.Printf( "%s:%d:%s", Lines.GetBrokerID( i ).c_str(),Lines.GetPort( i ), Lines.GetPVCID(i).c_str() );
                FConnectionObjects.Add( LineKey, NewConnection );                                                        
            }
            else
            {
                UFC::BufferedLog::Printf( " Connection BrokerID:%s PVCID:%s duplicate. ", Lines.GetBrokerID( i ).c_str(), Lines.GetPVCID(i).c_str() );
                exit( 0 );
            }
        }
        UFC::BufferedLog::Printf( " Create %d APCode 4 connection objects.", FFCMOrderPVCCount );
    }
    if( (FFCMOrderPVCCount = Lines.GetLines( 'C' ) ) > 0 )///< Odd Ex
    {
        if( FBrokerID2 == 0 )
            FBrokerID2 = Lines.GetBrokerID( 0 ).SubString( 0, 2 ).ToInt();
        for ( int i = 0; i < FFCMOrderPVCCount; i++ )
        {
            ConnectionParameter cp( Lines.GetPort( i ), 
                                    Lines.GetPVC( i ), 
                                    'C', 
                                    Lines.GetBrokerID( i ), 
                                    Lines.GetPassword( i ),
                                    Lines.GetIP( i ),
                                    Lines.GetLocalPort( i ),
                                    Lines.GetPVCID(i),
                                    Lines.GetActive( i ),
                                    Lines.GetThreughput( i ));
            UInt32              SHMID = UniqueSHMID( cp );
            int                 Group = Lines.GetGroup( i );
            
            if( Group != PVC_ALL_GROUP && Group >= OrderQueueGroupCount-1 )
                OrderQueueGroupCount = Group + 1; 
            if( SHMIDSet.Exists( SHMID ) == FALSE )
            {
                SHMIDSet.Add( SHMID );
                NewConnection = new SpeedyOrderConnectionObject( this, cp, this, this, SHMID, Group, FIsTWSE ) ;
                LineKey.Printf( "%s:%d:%s", Lines.GetBrokerID( i ).c_str(),Lines.GetPort( i ), Lines.GetPVCID(i).c_str() );
                FConnectionObjects.Add( LineKey, NewConnection );                                        
            }
            else
            {
                UFC::BufferedLog::Printf( " Connection BrokerID:%s PVCID:%s duplicate. ", Lines.GetBrokerID( i ).c_str(), Lines.GetPVCID(i).c_str() );
                exit( 0 );
            }
        }
        UFC::BufferedLog::Printf( " Create %d APCode 4 connection objects.", FFCMOrderPVCCount );
    }
    if( (FFCMOrderPVCCount = Lines.GetLines( '9' ) ) > 0 )// Post trade
    {
        if( FBrokerID2 == 0 )
            FBrokerID2 = Lines.GetBrokerID( 0 ).SubString( 0, 2 ).ToInt();
        for ( int i = 0; i < FFCMOrderPVCCount; i++ )
        {
            ConnectionParameter cp( Lines.GetPort( i ), 
                                    Lines.GetPVC( i ), 
                                    '9', 
                                    Lines.GetBrokerID( i ), 
                                    Lines.GetPassword( i ),
                                    Lines.GetIP( i ),
                                    Lines.GetLocalPort( i ),
                                    Lines.GetPVCID( i ),
                                    Lines.GetActive( i ),
                                    Lines.GetThreughput( i ));
            UInt32              SHMID = UniqueSHMID( cp );            
            int                 Group = Lines.GetGroup( i );
            
            if( Group != PVC_ALL_GROUP && Group >= OrderQueueGroupCount-1 )
                OrderQueueGroupCount = Group + 1; 
            if( SHMIDSet.Exists( SHMID ) == FALSE )
            {
                SHMIDSet.Add( SHMID );
                NewConnection = new SpeedyOrderConnectionObject( this, cp, this, this, SHMID, Group, FIsTWSE );
                LineKey.Printf( "%s:%d:%s", Lines.GetBrokerID( i ).c_str(),Lines.GetPort( i ), Lines.GetPVCID(i).c_str() );
                FConnectionObjects.Add( LineKey, NewConnection );                        
            }
            else
            {
                UFC::BufferedLog::Printf( " Connection BrokerID:%s PVCID:%s duplicate. ", Lines.GetBrokerID( i ).c_str(), Lines.GetPVCID(i).c_str() );
                exit( 0 );
            }
        }
        UFC::BufferedLog::Printf( " Create %d APCode 9 connection objects.", FFCMOrderPVCCount );
    }    
    ///< Create all connections for APCode 3 execution report system.
    if( (FFCMConfirmPVCCount = Lines.GetLines( '3' )) > 0 )
    {        
        if( FBrokerID2 == 0 )
            FBrokerID2 = Lines.GetBrokerID( 0 ).SubString( 0, 2 ).ToInt();
        for ( int i = 0; i < FFCMConfirmPVCCount; i++ )
        {
            ConnectionParameter cp( Lines.GetPort( i ), 
                                    Lines.GetPVC( i ), 
                                    '3', 
                                    Lines.GetBrokerID( i ), 
                                    Lines.GetPassword( i ),
                                    Lines.GetIP( i ),
                                    Lines.GetLocalPort( i ),
                                    Lines.GetPVCID( i ),
                                    Lines.GetActive( i ));
            UInt32              SHMID = UniqueSHMID( cp );
            
            if( SHMIDSet.Exists( SHMID ) == FALSE )
            {
                SHMIDSet.Add( SHMID );
                NewConnection =  new SpeedyConfirmConnectionObject( this, cp, this, this, '3', FIsTWSE  );
                LineKey.Printf( "%s:%d:%s", Lines.GetBrokerID( i ).c_str(),Lines.GetPort( i ), Lines.GetPVCID(i).c_str() );
                FConnectionObjects.Add( LineKey, NewConnection );        
            }
            else
            {
                UFC::BufferedLog::Printf( " Connection BrokerID:%s PVCID:%s duplicate. ", Lines.GetBrokerID( i ).c_str(), Lines.GetPVCID(i).c_str() );
                exit( 0 );
            }
        }
        UFC::BufferedLog::Printf( " Create %d APCode 3 connection objects.", FFCMConfirmPVCCount );
    } 
    UFC::BufferedLog::Printf( " *** Max Order Group:%d. ***", OrderQueueGroupCount - 1  );        
    
    
    BindingCore( );    
    if( FFIXManager != NULL )
    {
        UFC::SleepMS( 1000 ); 
        UFC::BufferedLog::Printf( " ***** Start running FIX Engine *****" );
        FFIXManager->Run();
        UFC::SleepMS( 1000 ); 
    }
    UFC::BufferedLog::Printf( " ***** Start running session threads *****" );
    
}
//---------------------------------------------------------------------------------------------------------------------
SpeedyOrderConnectionObject*  SpeedyObjectsManager::NextSession( void )
{
    int SC = FFIXSession.ItemCount();
    
    if( SC > 0 )
    {
        FSessionindex++;
        if( FSessionindex >= SC )
            FSessionindex = 0;
        return FFIXSession[ FSessionindex ];
    }
    return NULL;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::SendFIXOrder( OrderRecord* OI )
{    
    for( register int i = 0; i < FFIXSession.ItemCount();i++ )
    {
        if( NextSession()->SendFIXOrder( OI ) == TRUE )
            return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::SendFIXOrder( OrderRecord* OI, Int64& UseUS )
{
    Int64  Begin = UFC::GetTickCountUS();                          
    Int64  Sent;

    UFC::BufferedLog::Printf( " ---------------- Benchmark call stack End ----------------" );    
    for( register int i = 0; i < FFIXSession.ItemCount();i++ )
    {
        if( NextSession()->SendFIXOrder( OI, Sent ) == TRUE )
        {
            UseUS = Sent - Begin;
            UFC::BufferedLog::Printf( " #5 [SpeedyObjectsManager::SendFIXOrder] total #3+#4 use[%d]us", UseUS );                    
            return TRUE;
        }
    }
    return FALSE;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::SendFIXOrder( SMPMessage& SMPMsg, BOOL AutoSession )
{     
    if( AutoSession == FALSE )
    {
        FIX::Session* FIXClient;

        UFC::BufferedLog::DebugPrintf(  UFC::dlOther, "  3.--- [%05d] Find FIX Session ---", SMPMsg.GetSMPSequence()  );                      
        if( SMPMsg.FSID[1] == 'X' &&  SMPMsg.FSID[2] == 'X' ) ///< Auto FIX Session
        {
            UFC::BufferedLog::Printf( " [%05d] SessionID[%s], Use auto session.", SMPMsg.GetSMPSequence(), SMPMsg.FSID  );              
            return FFIXManager->SendFIXOrder( SMPMsg );
        }
        else
        {    
            if( (FIXClient = FFIXManager->FindFIXSession(  SMPMsg.GetSenderID(),  SMPMsg.GetTargetID() )) != NULL )
                return FIXClient->sendSMP( SMPMsg.GetFIXMessage(), SMPMsg.GetFIXLength(), SMPMsg.GetSMPSequence() ); 
            else
                UFC::BufferedLog::Printf( " [%05d] FIX Session[%s]->[%s] Not found", SMPMsg.GetSMPSequence(), SMPMsg.GetSenderID(), SMPMsg.GetTargetID()  );              
            return FALSE;
        }
    }
    else ///< Auto FIX Session
        return FFIXManager->SendFIXOrder( SMPMsg );
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::BindingCore( void )
{
    ConnectionObjectBase* NewConnection;
    UFC::AnsiString       RuleName( "SpeedyTSESession" );
    UFC::PInt32List       Cores;
    int                   Oi = 0;
    bool                  NeedBind;
    
    if( FIsTWSE == FALSE )
        RuleName = "SpeedyOTCSession";
    NeedBind = UFC::PThread::ThreadBindingCores( RuleName, Cores );            
    NewConnection = FConnectionObjects.First();    
    while( NewConnection != NULL )
    {
        NewConnection->Active();
        if( NeedBind == true )
        {
            SpeedyOrderConnectionObject* OrdConnection = dynamic_cast<SpeedyOrderConnectionObject*>(NewConnection);
            if( OrdConnection != NULL )
            {
                int UseCore = Cores[ Oi % Cores.ItemCount() ];                
                
                OrdConnection->PThread_setaffinity( UseCore );
                UFC::BufferedLog::Printf( " Session[%s] bind to core[%d]", OrdConnection->GetConnectionParameter()->GetPVCID().c_str(), UseCore );
                Oi++;
            }
        }
        NewConnection = FConnectionObjects.Next();        
    }    
}
//-----------------------------------------------------------------------------------------------------------------------
Int32 SpeedyObjectsManager::PVCStateToTradeState( char AP, Int32 State )
{
	switch( AP )
    {
       	case '4': return State += 100;
       	case '9': return State += 200;       	
        case '5': return State += 300; /// Lend
        case '8': return State += 400; /// Tender offer      	
        case '6': return State += 500; /// Aution or NegotiatePx       	
        case '7': return State += 600; /// TenderEx
        case 'C': return State += 700; /// OddEx
       	default : return State;
    }
}
//-----------------------------------------------------------------------------------------------------------------------
int SpeedyObjectsManager::GetLineState( const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID, int& inSeq,int& OutSeq )
{
    int State = PVC_STATUS_UNKNOWNPVC;
    ConnectionObjectBase * co = GetConnection( BrokerID, Link, PVCID );
    if ( co != NULL )
    {
        int Value;
        co->OnCommand( COMMAND_GET_IN_SEQ, ( void * ) &Value );
        inSeq  = Value;
        co->OnCommand( COMMAND_GET_OUT_SEQ, ( void * ) &Value );
        OutSeq = Value;
        co->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
        return PVCStateToTradeState( co->GetConnectionParameter()->GetAPCode(), State );
    }
    return PVC_NOT_EXISTS;
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ReplyLineState( const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID )
{
    Int32 InSeq  = 1;
    Int32 OutSeq = 1;                
    Int32 State = GetLineState( BrokerID, Link, PVCID, InSeq, OutSeq );
    MTree Tree;

    Tree.append( "COMMAND", (Int32)AGENT_REPLY_STATE );
    Tree.append( "LINK", Link );
    Tree.append( "PVCID", PVCID );
    Tree.append( "BROKERID", BrokerID );
    Tree.append( "SYS", 3 - FIsTWSE );
    Tree.append( "PVC_STATE", State );
    Tree.append( "IN", InSeq );
    Tree.append( "OUT", OutSeq );
    MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, Tree, FALSE );                    
}
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase * SpeedyObjectsManager::GetConnection( const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID )
{    
    UFC::AnsiString       LineKey;
    
    LineKey.Printf( "%s:%d:%s", BrokerID.c_str(),Link, PVCID.c_str() );
    UFC::PLockObject Lock( FCriticalSection );

    return FConnectionObjects.GetObjectByKey( LineKey );
}
//---------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ListenConnection( const UFC::AnsiString& BrokerID, int, const UFC::AnsiString& PVCID )
{
     ConnectionParameter * cp;
     ConnectionObjectBase * co;
     UFC::PLockObject Lock( FCriticalSection );
     
     if( (co = FConnectionObjects.First() )!=NULL )
     {    
        do
        {            
            cp = co->GetConnectionParameter();
            if ( cp->GetPVCID() == PVCID && cp->GetBrokerID() == BrokerID )
                 co->Listen( TRUE );
            else
                 co->Listen( FALSE );
            co = FConnectionObjects.Next(); 
         }while( co != NULL );
     }
}
//---------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::EnableConnection( const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID, BOOL Enabled )
{     
     UFC::PLockObject Lock( FCriticalSection );
 
     ConnectionObjectBase * co = GetConnection( BrokerID, Link, PVCID );
     if ( co != NULL )
        co->Enable( Enabled );

}
//---------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::SetFIXSequence( const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID, int InSeq, int OurSeq )
{     
     UFC::PLockObject Lock( FCriticalSection );
 
     ConnectionObjectBase * co = GetConnection( BrokerID, Link, PVCID );
     if( co != NULL )
         co->SetFIXSeq( InSeq, OurSeq );
}
//---------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::SetAPCode( const UFC::AnsiString& BrokerID, int Link, const UFC::AnsiString& PVCID, char NewAPCode )
{
     ConnectionParameter * cp;
     ConnectionObjectBase * co;     
     UFC::PLockObject Lock( FCriticalSection );
      
     if ( (co = GetConnection( BrokerID, Link, PVCID )) != NULL )
     {
         cp = co->GetConnectionParameter();
         UFC::BufferedLog::Printf( " Port:%d PVCID:%s change AP code from %c to %c", Link,PVCID.c_str(), cp->GetAPCode(), NewAPCode );
         if( cp->GetAPCode() != NewAPCode )
             co->SetTradeSession( NewAPCode );
     }
}
//---------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ReportPerformance( void )
{
    MTree  Data;
    int    MsgSec = 0;
    UInt32 Status = 0;
    long   TimeNow = UFC::GetTickCountMS();    
    int    TimeDiff = TimeNow - FPrevTime;
    
    if( FPrevTime != 0 && TimeDiff > 0 )
        MsgSec = ( (Int32)FTotalOrder - FMsgCount ) * 100000 / TimeDiff;
    Status = (UInt32)GetMarketStatus( '0' ) +      ///< Normal    
             ((UInt32)GetMarketStatus( '4' )<<8 )+ ///< Odd Market
             ((UInt32)GetMarketStatus( '9' )<<16); ///< PostMarket;
    FPrevTime = TimeNow;
    FMsgCount = (Int32)FTotalOrder;
    Data.append( COMMAND_STRING, AGENT_PERFORMANCE );
    Data.append( TRADE_SYS_STRING, 3- FIsTWSE );
    Data.append( COUNT_STRING, (Int32)FTotalOrder );
    Data.append( MSG_SEC_STRING, MsgSec );
    if( UseLockLessQueue == TRUE )
        Data.append( "QUEUE", (Int32)FMPMCQueue->Count() );
    else
        Data.append( "QUEUE", (Int32)FQueue.Count() );
    Data.append( "TIME",  UFC::GetHHMMSS() );
    Data.append( "STATUS", Status );            
    MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, Data, FALSE );
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::CheckPVC( const char* APCodes )
{
    Int32                 State,DisconnectCount = 0;
    ConnectionParameter*  cp;
    ConnectionObjectBase* co;
    UFC::PLockObject Lock( FCriticalSection );
    
    if( (co = FConnectionObjects.First() )!=NULL )
    {    
        do
        {
            cp = co->GetConnectionParameter();
            if( strchr( APCodes,cp->GetAPCode( )) != NULL  )///< AP Code match !
            {
                co->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
                switch( State )
                {
                    case PVC_STATUS_TRY_OPEN     :
                    case PVC_STATUS_WAIT_TRY     :
                    case PVC_STATUS_WAIT_OPEN    : 
                    case PVC_STATUS_ONLINE_BUSY  :
                    case PVC_STATUS_ONLINE_READY : break;
                    default : DisconnectCount++;   break;  
                }
            }
            co = FConnectionObjects.Next(); 
         }while( co != NULL );
    }
    if( DisconnectCount > 0 )
    {
        UFC::AnsiString ErrMsg;    
    
        ErrMsg.Printf( "Host:%s %d PVC disconnected", UFC::Hostname, DisconnectCount  );
        UFC::WriteErrorLog( "SpeedySolution", "Speedy" , ErrMsg );
    }    
}
//--------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::CheckConfigModifiaction( void )
{
    struct stat FileStat;
    
    stat( FCfgFileName.c_str(), &FileStat );    
    if( FModifyTime == 0 ) ///< First time get modify time.   
    {        
        FModifyTime = FileStat.st_mtime;
        return FALSE;
    }
    else
    {
        if( FModifyTime != FileStat.st_mtime )
        {
            FModifyTime = FileStat.st_mtime;
            return TRUE;    
        }
        return FALSE;
    }
}
//---------------------------------------------------------------------------
int SpeedyObjectsManager::TransferAll( int Broker, int System )
{
    UFC::UQueueData Order( 0, MAX_DATA_SIZE );
    UFC::PLongList  MTypes;
    int             TransCount = 0;
    
    for( register int i = 0; i < OrderQueueGroupCount; i ++ )
         MTypes.Add( i * 10000 + Broker * 100 + System ); ///< Use SybSystem number as key.
    while( TRUE )
    {    
        BOOL isRecvSuccess = FALSE;
        
        try
        {
           isRecvSuccess = FQueue.Recv( Order, MTypes );
        }
        catch( UFC::QueueException& QueuueEx )
        {   ///< Queue fulled(Up to the system limitation), Reject to client.(As a order reject )
            UFC::BufferedLog::Printf( " TransferOrderThread::Execute() Receive QueueException:[%s]", QueuueEx.what() );
        }
        if( isRecvSuccess == FALSE ) 
            break; ///< No order in queue, return                
        OrderRecord  OrderObj;     
    
        OrderObj.FromBinary( FALSE, (UInt8*)Order.GetData() );         
        if( OrderObj.GetMessageType() == mtReplacePrice )
            SpeedyReject( OrderObj, SPEEDY_ERROR_NO_PVC, TRUE );///< Reject Replace PX Orders
        else if( OrderInfo.ClusterEnable() == FALSE) 
            SpeedyReject( OrderObj, SPEEDY_ERROR_NO_PVC, TRUE );///< No cluster. direct reject it.
        else
            SpeedyReject( OrderObj, SPEEDY_ERROR_NO_PVC, FALSE );///< Reject to Cluster servers.            
        TransCount++;
        UFC::SleepMS( 5 );
    }
    return TransCount;    
}
//---------------------------------------------------------------------------
int SpeedyObjectsManager::TransferAllEx( int Broker, int System )
{
    OrderRecord*    OrderObj;
    UFC::PLongList  MTypes;
    int             TransCount = 0;
    int             FTS = TWSE::MessageHeader::GetTradingSession( System );
    
    for( register int i = 0; i < OrderQueueGroupCount; i ++ )
         MTypes.Add(  FTS*10000 + i * 100 + Broker ); ///< Use SybSystem number as key.    
    while( TRUE )
    {    
        if( FMPMCQueue->pop( MTypes,&OrderObj, 0 ) == true )
        {
            if( OrderObj->GetMessageType() == mtReplacePrice )
                SpeedyReject( *OrderObj, SPEEDY_ERROR_NO_PVC, TRUE );///< Reject Replace PX Orders
            else if( OrderInfo.ClusterEnable() == FALSE) 
                SpeedyReject( *OrderObj, SPEEDY_ERROR_NO_PVC, TRUE );///< No cluster. direct reject it.
            else
                SpeedyReject( *OrderObj, SPEEDY_ERROR_NO_PVC, FALSE );///< Reject to Cluster servers.            
            TransCount++;
            UFC::SleepMS( 5 );
            delete OrderObj;
        }
        else
            break;///< No order in queue, return        
    }
    return TransCount;    
}
//---------------------------------------------------------------------------
void SpeedyObjectsManager::ProcessPendingOrder( char AP )
{
    int System = ToSubsystemID( AP );
    int TransCount = 0;    
    
    
    if( UseLockLessQueue == false )
        TransCount = TransferAll( FBrokerID2, System  );
    else
        TransCount = TransferAllEx( FBrokerID2, System  );
    if( TransCount > 0 )
    {
        UFC::BufferedLog::Printf( " All trading session %d PVC broken. Transfer all orders to AA Server.", System );
        UFC::BufferedLog::Printf( " Transfer %d Orders to AA Speedy.", TransCount );    
        UFC::BufferedLog::FlushToFile();
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::CheckPendingOrder( void )
{
    MarketSchedule* NormalSession = GetScheduleObject( '0' );
    ///< Normal session broken.    
    if( NormalSession->GetPVCCount() <= 0 &&  NormalSession->IsOpened() )
        ProcessPendingOrder( '0' );            
    
    MarketSchedule* OddSession    = GetScheduleObject( '4' );
    ///< Odd session broken.    
    if( OddSession->GetPVCCount( ) <= 0 && OddSession->IsOpened() )
        ProcessPendingOrder( '4' );            
    
    MarketSchedule* PostSession   = GetScheduleObject( '9' );    
    ///< Post session broken.    
    if( PostSession->GetPVCCount( ) <= 0 && PostSession->IsOpened() )
        ProcessPendingOrder( '9' );       
}            
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::Process(  int UseMS, int Times1Sec )
{
    
    Int32 Times = UseMS / 50;          
    Int32 TimeNow = UFC::GetHHMMSS();///< Check Market status.
            
    FLoopCount ++;        
    for( int i = 0; i < Times; i ++ )
    {
        FNormalMarket->OnTimer( TimeNow );
        FOddMarket->OnTimer( TimeNow );
        FOddExMarket->OnTimer( TimeNow );
        FPostMarket->OnTimer( TimeNow );  
        FLendMarket->OnTimer( TimeNow );  
        FAuctionMarket->OnTimer( TimeNow );  
        FTenderMarket->OnTimer( TimeNow );  
        FTenderExMarket->OnTimer( TimeNow );              
        if( FNormalMarket->IsTimeToCheckPVC( TimeNow ) == TRUE )
            CheckPVC( "049" );
        if( FOddMarket->IsTimeToCheckPVC( TimeNow ) == TRUE )
            CheckPVC( "4" );
        if( FPostMarket->IsTimeToCheckPVC( TimeNow ) == TRUE )
            CheckPVC( "9" );
        UFC::SleepMS( 50 );
        if( i%2 == 0 )
            CheckPendingOrder();
    }
    
    if( FLoopCount % (7*Times1Sec) == 0 )
    {
        if (CheckConfigModifiaction() == TRUE)
        {
            UFC::UiniFile CfgFile( FCfgFileName );

            UFC::BufferedLog::Printf( "______________________________________________________" );
            UFC::BufferedLog::Printf( "   [%s] Modified, reload it.", FCfgFileName.c_str() );       
            FNormalMarket->Reload( CfgFile );
            FOddMarket->Reload( CfgFile );
            FOddExMarket->Reload( CfgFile );
            FPostMarket->Reload( CfgFile );
            FLendMarket->Reload( CfgFile );
            FAuctionMarket->Reload( CfgFile );
            FTenderMarket->Reload( CfgFile );
            FTenderExMarket->Reload( CfgFile );
            UFC::BufferedLog::Printf( "______________________________________________________" );
        }
//RM
#ifdef __RMLib
        if (rm::IBasicRiskManagerObject::IsRMControlSetting() && (rm::rmObjPtr->GetRiskManagerName() == "Mega"))
        {
            rm::CMegaRiskManagerObject *megaRMObjPtr = static_cast<rm::CMegaRiskManagerObject*>(rm::rmObjPtr);
            if (megaRMObjPtr->LoadRiskManagerControlData(true)) megaRMObjPtr->IncreaseRMControlFileSeqNo();
            if (megaRMObjPtr->LoadOrderDeleteData()) megaRMObjPtr->IncreaseRMDeleteFileSeqNo();
        }
#endif
//RM
    }  
    if( FLoopCount % Times1Sec == 0 )
    {
        FLendMarket->FOrdersPerSec     = 0;
        FAuctionMarket->FOrdersPerSec  = 0;
        FTenderMarket->FOrdersPerSec   = 0;
        FTenderExMarket->FOrdersPerSec = 0;            
        ReportPerformance();                
    }
    //OrderLog->Flush();               ///< Flush the APCode 4 log to file
    //FillLog->Flush();                ///< Flush the APCode 3,8,9 log to file
    //UFC::BufferedLog::FlushToFile(); ///< Flush the screen log to file.            
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::Run()
{
    while ( TRUE )
    {
        Process( 1000, 1 );
    }
}        
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::SendRWDataToBus( char* IOData )
{
    UFC::AnsiString Time( IOData, 12 );
    UFC::AnsiString IOString;
    MTree      Data;

    IOString.Printf( "%s*%s",Time.c_str(), IOData + 29 );
    Data.append( COMMAND_STRING, AGENT_PVC_DAT );
    Data.append( IO_DATA_STRING, IOString );
    MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, Data, FALSE );
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ReportPVCState(  ConnectionObjectBase * CObj, int State  )
{
    ConnectionParameter * cp = CObj->GetConnectionParameter();
    UFC::AnsiString& BrokerID = cp->GetBrokerID();     
    UFC::AnsiString& PVCID = cp->GetPVCID();        
    MTree            Tree;
    int              InSeq,OutSeq;
    
    CObj->OnCommand( COMMAND_GET_IN_SEQ, ( void * ) &InSeq );
    CObj->OnCommand( COMMAND_GET_OUT_SEQ, ( void * ) &OutSeq );
    Tree.append( COMMAND_STRING,   AGENT_REPLY_STATE );
    Tree.append( LINK_STRING,      cp->GetLink() );
    Tree.append( "PVCID",          PVCID );
    Tree.append( "BROKERID",       BrokerID );
    Tree.append( SYS_STRING,       3- FIsTWSE );
    Tree.append( PVC_STATE_STRING, PVCStateToTradeState( cp->GetAPCode(), State ) );
    Tree.append( "IN",             InSeq );
    Tree.append( "OUT",            OutSeq );
    MessageObj->Send( COMMAND_SUBJECT, UFC::Hostname, Tree, FALSE );
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::OnConnectionNotify( ConnectionObjectBase * CObj, int Reason, void * Data )
{
    ConnectionParameter * cp = CObj->GetConnectionParameter();
    int State;
    

    switch ( Reason )
    {
        case CONNECTION_PVC_SLEEPING:
             ReportPVCState( CObj, PVC_STATUS_SLEEPING );
             break;
        case CONNECTION_PVC_WAIT_OPEN:
             ReportPVCState( CObj, PVC_STATUS_WAIT_OPEN );
             break;
        case CONNECTION_PVC_TRY_OPEN:
             ReportPVCState( CObj, PVC_STATUS_TRY_OPEN );
             break;
        case CONNECTION_PVC_UPDATE_STATE:
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( CObj, State );
             break;
        case CONNECTION_PVC_ENABLED:
             UFC::BufferedLog::Printf( " Enable PVC %s Port:%d.", cp->GetPVCID().c_str(), cp->GetLink() );
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( CObj, State );
             break;
        case CONNECTION_PVC_DISABLED:
             CheckPVCBroken( cp );
             UFC::BufferedLog::Printf( " Disabled PVC %s Port:%d.", cp->GetPVCID().c_str(), cp->GetLink() );
             ReportPVCState( CObj, PVC_STATUS_DISABLE );
             break;
        case CONNECTION_OBJECT_NORMAL_CLOSE:
             CheckPVCBroken( cp );
             UFC::BufferedLog::Printf( " PVC %s Normal close.", cp->GetPVCID().c_str() );
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( CObj, State);
             break;
        case CONNECTION_OBJECT_X25ERROR:
             CheckPVCBroken( cp );
             UFC::BufferedLog::Printf( " PVC %s X.25 error.", cp->GetPVCID().c_str() );
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( CObj, State);
             break;
        case CONNECTION_OBJECT_UNKNOWN_ERROR:
             CheckPVCBroken( cp );
             UFC::BufferedLog::Printf( " PVC %s Unknown error.", cp->GetPVCID().c_str() );
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( CObj, State);
             break;
        case CONNECTION_PVC_LOCKED:
             CheckPVCBroken( cp );
             UFC::BufferedLog::Printf( " PVC %s Locked.", cp->GetPVCID().c_str() );
             CObj->OnCommand( COMMAND_GET_STATE, ( void * ) & State );
             ReportPVCState( CObj, State);
             break;
        case CONNECTION_OBJECT_PVC_READY:
             OnPVCReady( cp );
             ReportPVCState( CObj, PVC_STATUS_ONLINE_READY);                 
             break;
        case CONNECTION_OBJECT_PVC_BROKEN:             
             CheckPVCBroken( cp );             
             ReportPVCState( CObj, PVC_STATUS_BROKEN );
             break;
        case CONNECTION_PVC_READ_WRITE:
             SendRWDataToBus((char*)Data);
             break;        
    }
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::OnPVCReady( ConnectionParameter * cp )
{
    if( PVCReady( cp ) == TRUE )
    {       
        int PVCCount;
        
        if( cp->GetPVC() == FIX_SESSION_PVC ) ///< FIX session support Normal/Odd/Post
        {
            MarketSchedule* NormalSession = GetScheduleObject( '0' );
            MarketSchedule* OddSession    = GetScheduleObject( '4' );
            MarketSchedule* OddExSession  = GetScheduleObject( 'C' );
            MarketSchedule* PostSession   = GetScheduleObject( '9' );
            
            PVCCount = NormalSession->IncreasePVCCount();
            UFC::BufferedLog::Printf( " FIX %s Ready!. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),NormalSession->FName.c_str(),PVCCount );    
            PVCCount = OddSession->IncreasePVCCount();
            UFC::BufferedLog::Printf( " FIX %s Ready!. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),OddSession->FName.c_str(),PVCCount );    
            PVCCount = OddExSession->IncreasePVCCount();
            UFC::BufferedLog::Printf( " FIX %s Ready!. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),OddExSession->FName.c_str(),PVCCount );    
            PVCCount = PostSession->IncreasePVCCount();
            UFC::BufferedLog::Printf( " FIX %s Ready!. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),PostSession->FName.c_str(),PVCCount );    
            if( DynamicSessionFIX == TRUE )
            {
                MarketSchedule*  LendSession = GetScheduleObject( '5' );
                MarketSchedule*  AuctionSession = GetScheduleObject( '6' );
                MarketSchedule*  TenderExSession = GetScheduleObject( '7' );
                MarketSchedule*  TenderSession = GetScheduleObject( '8' );
                
                PVCCount = LendSession->IncreasePVCCount();
                UFC::BufferedLog::Printf( " FIX %s Ready!. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),LendSession->FName.c_str(),PVCCount );    
                PVCCount = AuctionSession->IncreasePVCCount();
                UFC::BufferedLog::Printf( " FIX %s Ready!. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),AuctionSession->FName.c_str(),PVCCount );    
                PVCCount = TenderExSession->IncreasePVCCount();
                UFC::BufferedLog::Printf( " FIX %s Ready!. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),TenderExSession->FName.c_str(),PVCCount );    
                PVCCount = TenderSession->IncreasePVCCount();
                UFC::BufferedLog::Printf( " FIX %s Ready!. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),TenderSession->FName.c_str(),PVCCount );                    
            }
            FFIXReadyCount++;
        }
        else
        {
            MarketSchedule* Session  = GetScheduleObject( cp->GetAPCode() );
            
            PVCCount = Session->IncreasePVCCount();         
            UFC::BufferedLog::Printf( " PVC %s Ready!. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),Session->FName.c_str(),PVCCount );
        }    
    }    
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::CheckPVCBroken( ConnectionParameter * cp )
{
     if( PVCBroken( cp ) == TRUE )
     {             
        UFC::AnsiString Msg;
        int             PrevCount,Count;
        
        if( cp->GetPVC() == FIX_SESSION_PVC )///< FIX session support Normal/Odd/Post
        {
            MarketSchedule* NormalSession = GetScheduleObject( '0' );
            MarketSchedule* OddSession    = GetScheduleObject( '4' );
            MarketSchedule* OddExSession  = GetScheduleObject( 'C' );
            MarketSchedule* PostSession   = GetScheduleObject( '9' );
            
            ///< Normal session broken.
            PrevCount = NormalSession->GetPVCCount( );
            Count     = NormalSession->DecreasePVCCount();         
            if( Count == 0 && PrevCount != 0 )
            {
                UFC::BufferedLog::Printf( " FIX %s broken. %s Avaliable:0 Start TransferOrderThread.", cp->GetPVCID().c_str(), NormalSession->FName.c_str() );
                new TransferOrderThread( '0', cp->GetBrokerID(), this  );
            }
            else    
                UFC::BufferedLog::Printf( " FIX %s broken. (Session:%s Avaliable:%d).", cp->GetPVCID().c_str(), NormalSession->FName.c_str(), Count );            
            ///< Odd session broken.
            PrevCount = OddSession->GetPVCCount( );
            Count     = OddSession->DecreasePVCCount();         
            if( Count == 0 && PrevCount != 0 )
            {
                UFC::BufferedLog::Printf( " FIX %s broken. %s Avaliable:0 Start TransferOrderThread.", cp->GetPVCID().c_str(), OddSession->FName.c_str() );
                new TransferOrderThread( '4', cp->GetBrokerID(), this  );
            }
            else    
                UFC::BufferedLog::Printf( " FIX %s broken. (Session:%s Avaliable:%d).", cp->GetPVCID().c_str(), OddSession->FName.c_str(), Count );                        
            ///< OddEx session broken.
            PrevCount = OddExSession->GetPVCCount( );
            Count     = OddExSession->DecreasePVCCount();         
            if( Count == 0 && PrevCount != 0 )
            {
                UFC::BufferedLog::Printf( " FIX %s broken. %s Avaliable:0 Start TransferOrderThread.", cp->GetPVCID().c_str(), OddExSession->FName.c_str() );
                new TransferOrderThread( 'C', cp->GetBrokerID(), this  );
            }
            else    
                UFC::BufferedLog::Printf( " FIX %s broken. (Session:%s Avaliable:%d).", cp->GetPVCID().c_str(), OddExSession->FName.c_str(), Count );                                    
            ///< Post session broken.
            PrevCount = PostSession->GetPVCCount( );
            Count     = PostSession->DecreasePVCCount();         
            if( Count == 0 && PrevCount != 0 )
            {
                UFC::BufferedLog::Printf( " FIX %s broken. %s Avaliable:0 Start TransferOrderThread.", cp->GetPVCID().c_str() , PostSession->FName.c_str() );
                new TransferOrderThread( '9', cp->GetBrokerID(), this  );
            }
            else    
                UFC::BufferedLog::Printf( " FIX %s broken. (Session:%s Avaliable:%d).", cp->GetPVCID().c_str(), PostSession->FName.c_str(), Count );                                    
            Msg.Printf( "FIX:%s at %s broken.",cp->GetPVCID().c_str(),UFC::Hostname );            
            if( DynamicSessionFIX == TRUE )
            {
                MarketSchedule*  LendSession = GetScheduleObject( '5' );
                MarketSchedule*  AuctionSession = GetScheduleObject( '6' );
                MarketSchedule*  TenderExSession = GetScheduleObject( '7' );
                MarketSchedule*  TenderSession = GetScheduleObject( '8' );
                
                Count = LendSession->DecreasePVCCount();
                UFC::BufferedLog::Printf( " FIX %s broken. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),LendSession->FName.c_str(),Count );    
                Count = AuctionSession->DecreasePVCCount();
                UFC::BufferedLog::Printf( " FIX %s broken. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),AuctionSession->FName.c_str(),Count );    
                Count = TenderExSession->DecreasePVCCount();
                UFC::BufferedLog::Printf( " FIX %s broken. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),TenderExSession->FName.c_str(),Count );    
                Count = TenderSession->DecreasePVCCount();
                UFC::BufferedLog::Printf( " FIX %s broken. (Session:%s Avaliable:%d)", cp->GetPVCID().c_str(),TenderSession->FName.c_str(),Count );                    
            }
            FFIXReadyCount--;
        }
        else
        {
            MarketSchedule* Session   = GetScheduleObject( cp->GetAPCode() );
            
            PrevCount = Session->GetPVCCount( );
            Count     = Session->DecreasePVCCount();         
            if( Count == 0 && PrevCount != 0 )
            {
                UFC::BufferedLog::Printf( " PVC %s broken. %s PVC Avaliable:0 Start TransferOrderThread.", cp->GetPVCID().c_str(), Session->FName.c_str() );
                new TransferOrderThread( cp->GetAPCode(), cp->GetBrokerID(), this  );
            }
            else    
                UFC::BufferedLog::Printf( " PVC %s broken. (Session:%s Avaliable:%d).", cp->GetPVCID().c_str(), Session->FName.c_str(), Count );            
            Msg.Printf( "PVC:%s at %s broken.",cp->GetPVCID().c_str(),UFC::Hostname );
        }
        
        if( FIsTWSE == TRUE )
            UFC::WriteErrorLog( "SpeedyTSEC", "PVC", Msg );
        else
            UFC::WriteErrorLog( "SpeedyOTC", "PVC", Msg );
     }    
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::CheckQueueFill( Int32 Type )
{    
    MarketSchedule* Market;
    Int32           OrderCount;
    
    if( IsOrder( Type, Market ) == TRUE )
    {   
        if( UseLockLessQueue == TRUE )
            OrderCount = FMPMCQueue->Count();
        else
            OrderCount = FQueue.Count();   
        if( Market->IsOpened() == FALSE && OrderCount >= Market->FMaxQueueBeforeOpenMarket )
            return FALSE; ///< Queue fill
        else if( Market->IsOpened() == TRUE && OrderCount < (Market->FMaxQueue + 10) && OrderCount >= Market->FMaxQueue )
            return FALSE; ///< Queue fill
        return TRUE;
    }
    return TRUE;
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::NeedSessionCount( MarketSchedule* Market, Int32 Broker, int& Need, int& Changing )
{    
    char                  SwitchToAPCode = Market->GetAPCode();    
    UFC::AnsiString       PVCBrokerID;    
    UFC::PLockObject      Lock( FCriticalSection );
    ConnectionParameter*  cp;    
    ConnectionObjectBase* co = FConnectionObjects.First();    
    int                   TotalNeedLine  = (Market->FOrdersPerSec/Market->FSwitchPVCThroughput) + 1;    
    int                   ExistReadyLine = Market->GetPVCCount();
    int                   ExistLine      = 0;

    while( co != NULL )
    {
        cp          = co->GetConnectionParameter();            
        PVCBrokerID = cp->GetBrokerID();
        ///< First 2 digi of BrokerID is the same, means same broker and A PVC of this AP code already exists.
        if( PVCBrokerID.SubString( 0, 2 ).ToInt() == Broker && cp->GetAPCode() == SwitchToAPCode )
            ExistLine++;                        
        co = FConnectionObjects.Next();                                
    };
    Need     = TotalNeedLine - ExistLine;
    Changing = ExistLine - ExistReadyLine;
    if( Need < 0 )
        Need = 0;
    if( Changing < 0 )
        Changing = 0;
}    
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::SwitchPVC( MarketSchedule* Market, Int32 Broker )
{    
    int  LineBroker, NeedLine, SwitchingCount;
    char LineAPCode, SwitchToAPCode = Market->GetAPCode();    
        
    NeedSessionCount( Market, Broker, NeedLine, SwitchingCount ); 
    if( NeedLine > 0 ) ///< Still need lines ?
    {        
        UFC::PLockObject Lock( FCriticalSection );
        UFC::AnsiString PVCBrokerID;
        ConnectionParameter*   cp;
        ConnectionObjectBase * co = FConnectionObjects.First() ;    

        while( co != NULL )
        {
            cp = co->GetConnectionParameter();                        
            
            PVCBrokerID = cp->GetBrokerID();
            LineBroker  = PVCBrokerID.SubString( 0, 2 ).ToInt();                
            if( SwitchToAPCode == '6'  ) ///< Switch to OTC Negotiate Price session.
            {   ///< OTC Negotiate Price session, broker ID should be xxxT
                if( FIsTWSE == FALSE && PVCBrokerID[3] != 'T' )
                {                        
                    co = FConnectionObjects.Next();
                    continue;
                }
            }
            if( LineBroker == Broker &&             ///< First 2 digi of BrokerID is the same, means same broker. 
                cp->GetPVC() != FIX_SESSION_PVC &&  ///< Not FIX Session
                co->IsEnable() == TRUE  )           ///< Is an active session.
            {
                LineAPCode = cp->GetAPCode();                                    
                ///< Switch Normal, Odd or Post trade PVC to dynamic trading session PVc.
                if( LineAPCode == '0' || LineAPCode == '4' || LineAPCode == '9' )
                {
                    co->SetTradeSession( SwitchToAPCode );
                    NeedLine--;     
                    SwitchingCount++;
                }
                if( NeedLine <= 0 )///< No more need line, break.
                    break;
            }                
            co = FConnectionObjects.Next();                
        }     
    }
    ///< Already have lines of this AP coode or changing a line to this AP code.
    if( Market->GetPVCCount() > 0 || SwitchingCount > 0 )
        return TRUE;
    return FALSE;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::CheckPVC( Int32 Type, Int32 Broker, Int32 & Reason, BOOL& AsConfirm  )
{
    MarketSchedule* Market;
        
    if( IsOrder( Type, Market ) == TRUE )
    {     
        if( Market->IsAutoSwitchMarket())///< Is a auto switch trading session?
        {
            if( Market->GetStatus( ) < msTryOrder )
            {
                Reason = 2;
                AsConfirm = TRUE;
                return FALSE; ///< Market Not opened.
            }
            Market->FOrdersPerSec ++;
            if( DynamicSessionFIX == FALSE && SwitchPVC( Market, Broker ) == TRUE )
                return TRUE;
        }                
        ///< Chack market opened. and Session available.
        if( Market->IsOpened() && Market->FPVCCount == 0 )
        {
             UFC::BufferedLog::Printf( " Market:%s opened but no available order PVC.", Market->GetName().c_str() );        
             Reason    = SPEEDY_ERROR_NO_PVC;
             AsConfirm = FALSE;
             return FALSE; ///< No PVC available.
        }
        return TRUE;
    }
    return TRUE;
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ModifyOID( char* Order, const UFC::AnsiString& OID )
{
    ///< Copy to T,O,P,A,E,V010
     for( register int i = 0; i < 5; i++ )
          Order[ 20 + i ] = OID[ i ];
}
//---------------------------------------------------------------------------------------------------------------------
bool SpeedyObjectsManager::IsDynamicSession( int SubSystem )
{ 
    switch( SubSystem )
    {
        case 30:
        case 93: ///< Normal
        case 40:
        case 94: ///< Odd post        
        case 33:
        case 83: ///< Intra-day
        case 32:
        case 98: ///< Post 
                 return false;
        default: return true;
    }
}
//---------------------------------------------------------------------------------------------------------------------
bool SpeedyObjectsManager::IsIntradayOddSession( int SubSystem )
{ 
    if( SubSystem == 33 || SubSystem == 83 ) ///< Intra-day Odd
        return true;
    return false;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::IsMarketOrder( const char* Order, int Size )
{
    if( GetOrderType(Order, Size ) == '1' )
        return TRUE;
    return FALSE;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::NoFIXSession( void )
{
    if( FFIXReadyCount <= 0)
        return TRUE;
    return FALSE;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::GetMType( Int32 SubSystem, Int32  Broker, OrderRecord& OrderObj, long& MType )
{
    if( IsDynamicSession( SubSystem ) == false ) ///<  Normal,Odd,Intra-day Odd and Post session.     
    {
        int UseGroup = OrderObj.Group;

        if( IsIntradayOddSession( SubSystem ) == true  ) ///< 33 or 83
        {
            if( OrderObj.Group != OddGroup )
            {
                SpeedyReject( OrderObj, SPEEDY_ERROR_NO_PVC, TRUE, "No Intra-day Odd session use this group." ); 
                return FALSE;
            }
            else
                UseGroup = OddGroup;
        }
        else
        {
            if( MareketOrderGroup == TRUE && IsMarketOrder( (const char*)OrderObj.GetData(), OrderObj.GetSize() ) == TRUE )
                UseGroup = PVC_MARKET_GROUP;             
        }
        MType = UseGroup * 10000 + Broker * 100 + SubSystem;         
    }
    else ///< Special Session       
    {        
        MType = Broker * 100 + SubSystem;    
        UFC::BufferedLog::Printf( " Special Session[%d] Broker[%d] Order MType[%d].", SubSystem, Broker, MType );        
    }
    return TRUE;
}
//---------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::OnMigoMessage( const UFC::AnsiString&, const UFC::AnsiString&, MTree * Data )
{
    Int32           CancelByNID, RejectReason;
    UFC::AnsiString OrderID( "null" ), IB;
   
    try
    {
        OrderRecord     OrderObj( FALSE, Data );///< FALSE: for TWSE/OTC       
        Int32           SubSystem  = UFC::AnsiString( (char*)OrderObj.GetData(), 2 ).ToInt();
        MarketSchedule* ThisMarket = GetScheduleObjectBySubsystemID( SubSystem );       
        Int32           Broker;                        
        long            MType; 
       
        Data->get( "OID", OrderID );
        
        UFC::Profiler::SetPerformanceFlag( FProfileID, UFC::ppSpeedyOrdInQueue, UFC::AnsiString(OrderObj.NID), OrderID );            
        
        ///< Make MType by trading session, BrokerID, Group.
        if( FIsTWSE == FALSE && SubSystem == 96 )///< Negotiate Px session, Always use dealer ID.
            Broker = FDealerID.SubString( 0, 2 ).ToInt();
        else
            Broker = UFC::AnsiString::StrToInt32( ((char*)OrderObj.GetData()) + 14, 2 );///< Get broker ID from outgoig message.
        ///< Get Order MType
        if( GetMType(  SubSystem, Broker, OrderObj, MType ) == FALSE )
            return;
             
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " MBus Recv[%06d][%010u][%s]", MType , OrderObj.NID, (char*)OrderObj.GetData() );
        ///< *1. Check traning session.
        if( ThisMarket == NULL )
        {
            UFC::BufferedLog::Printf( " Invalid sub-system:%d ", SubSystem );
            UFC::BufferedLog::FlushToFile();
            return;
        }            
        else ///< Reject order if market closed.
        {
            if( ( ThisMarket->IsAutoSwitchMarket() == TRUE ) && ( ThisMarket->IsOpened( ) == FALSE ) ) ///< Market closeed
            {
                SpeedyReject( OrderObj, SPEEDY_ERROR_MARKET_CLOSED, TRUE ); ///< Market closeed
                return;
            }
        }
       
        ///< *2. Check cancel by NID.
        if( Data->get( "CNID", CancelByNID ) == TRUE )///< Use NID to Cancel/Query the original Order.
        {                                             ///< BINARY format not support this function.
            UFC::BufferedLog::Printf( " Cancel or Query by NID[%d]", CancelByNID);
            if( OrderInfo.GetOID( CancelByNID, OrderID, IB, true ) == FALSE )///< Find the original order by NID.
            {
                UFC::BufferedLog::Printf( " NID[%d] not found! Reject it." );
                SpeedyReject( OrderObj, SPEEDY_ERROR_NOT_SPEEDY, TRUE );
                return; ///< Reject it. Speedy don't known this order.
            }
            else
            {
                UFC::BufferedLog::Printf( " NID[%d] map to OrderID[%s]", CancelByNID,  OrderID.c_str() );
                ModifyOID( (char*)OrderObj.GetData(), OrderID );
            }
        }
       
        ///< *3. Check dirty flag.
        if( OrderObj.IsDirty == FALSE ) ///< No dirty byte exists.
        {
            Int32 Type = UFC::AnsiString( (char*)OrderObj.GetData(), 6 ).ToInt();
            Int32 Reason;
            BOOL  AsConfirm;
           
            if( CheckQueueFill( Type ) == FALSE ) ///< Queue fulled, reject to AA servers.
            {
                SpeedyReject( OrderObj, SPEEDY_ERROR_QUEUE_FILLED ); ///< Queue fulled
                return;
            }
            ///< Any PVC available?
            ///< Check any auto switch trading session PVC exists ? if not switch one. 
            if( CheckPVC( Type, Broker , Reason, AsConfirm ) == FALSE )
            {
                SpeedyReject( OrderObj, Reason, AsConfirm ); ///< Check PVC failed.
                return;
            }                          
        }
        else ///< Dirty byte exists, reject it directly.
        {
            if( Data->get( "REASON", RejectReason ))
            {               
                SpeedyReject( OrderObj, RejectReason, TRUE ); ///< Queue fulled
                return;                                       ///< No PVC available in these Speedy cluster
            }                                                 ///< Reject to client.(As a order reject )
        }

        ///< *4. Check order group.
        if( OrderObj.Group >= OrderQueueGroupCount )
        {
            SpeedyReject( OrderObj, SPEEDY_ERROR_NO_PVC, TRUE ); 
            return;  ///< No PVC available of this group.
        }
       
        try
        {
            UFC::UQueueData QueueOrderData( MType, MAX_DATA_SIZE );
            
            if( OrderObj.ToBinary( (UInt8*)QueueOrderData.GetData(), MAX_DATA_SIZE ) == TRUE )
                FQueue.Send( QueueOrderData );
            UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Send MType[%06d] OrderID[%s] To Queue Success.", MType, OrderID.c_str() );
        }
        catch( UFC::QueueException& QueuueEx )
        {   ///< Queue fulled(Up to the system limitation), Reject to client.(As a order reject )
            UFC::BufferedLog::Printf( " SpeedyObjectsManager::OnMigoMessage() Send QueueException:[%s]", QueuueEx.what() );
            SpeedyReject( OrderObj, SPEEDY_ERROR_QUEUE_FILLED );
        }
    }
    catch( UFC::Exception& e )
    {    ///< Queue fulled(Up to the system limitation), Reject to client.(As a order reject )
        UFC::BufferedLog::Printf( " Get order failed Reason:%d", e.what() );
    }
    UFC::BufferedLog::FlushToFile();
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::SpeedyReject( OrderRecord& OrderObj, Int32 ErrCode, BOOL AsConfirm, const UFC::AnsiString& ErrorMessage )
{
    MTree Data;
    
    if( AsConfirm == TRUE ) ///< Reject as a order confirm
    {
        UFC::AnsiString X25Time, FateR020;              
                       
        X010ToX020( (char*)OrderObj.GetData() , ErrCode, FateR020, TWSE::MessageHeader::FNewVersion, TWSE::MessageHeader::FNewExVersion );
        UFC::GetTimeString( X25Time, FALSE );
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " Reject NID[%010u][%s] as a order confirm, Reason:%d[%s]", OrderObj.NID, (char*)OrderObj.GetData(), ErrCode, ErrorMessage.c_str() );
        Data.append( "CONFIRM_ORDER", FateR020 );
        Data.append( "PVC", "00" );
        Data.append( "PBNO", UFC::AnsiString( (char*)OrderObj.GetData() +14, 4 ) );        
        Data.append( "NID", OrderObj.NID );
        Data.append( "TTIME", X25Time );
        Data.append( "XTIME", X25Time );
        if (ErrorMessage.Length() > 0) Data.append( "MSG", ErrorMessage );
        if( strlen( OrderObj.Key ) > 0 )
            Data.append( "KEY", OrderObj.Key );
        if( strlen( OrderObj.Hostname ) > 0  )
            Data.append( "HOST", OrderObj.Hostname );
        else
            Data.append( "HOST", UFC::Hostname );
        Data.append( "PHOST", UFC::Hostname );                                
        if( ExtremeVersion == TRUE )
        {
            if( FIsTWSE == TRUE )    
                gTSEConfirm->OnMigoMessage( "Q", UFC::Hostname, &Data );
            else
                gOTCConfirm->OnMigoMessage( "Q", UFC::Hostname, &Data );
        }
        MessageObj->Send( FOrderConfirmSubject, UFC::Hostname, Data );
    }
    else ///< REJECT to HA/Load balance controlor.( like SpeedyGateway, MegaMAPPER... )
    {   
        Data.append( "ORDER", (char*)OrderObj.GetData() );
        Data.append( "NID", OrderObj.NID );    
        Data.append( "GROUP",  OrderObj.Group );    ///< Get Session group.        
        Data.append( "REASON", ErrCode );
        if( strlen( OrderObj.Key ) > 0 )
            Data.append( "KEY", OrderObj.Key );
        if( strlen( OrderObj.Hostname ) > 0  )
            Data.append( "HOST", OrderObj.Hostname );
        else
            Data.append( "HOST", UFC::Hostname );
        if( OrderInfo.ClusterEnable() == FALSE ) ///< No cluster. append dirty to reject it.
            Data.append( "DIRTY", 1 );
        if (ErrorMessage.Length() > 0) Data.append( "MSG", ErrorMessage );
        if( FIsTWSE == TRUE )
            MessageObj->Send( "ORDER.REJECT.TSE", UFC::Hostname, Data  );
        else
            MessageObj->Send( "ORDER.REJECT.OTC", UFC::Hostname, Data  );
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " Reject NID[%010u][%s] Length:%d to Balance controller, Reason:%d[%s]", OrderObj.NID, (char*)OrderObj.GetData(), OrderObj.GetSize(), ErrCode, ErrorMessage.c_str() );
    }
}
//---------------------------------------------------------------------------------------------------------------------
void SimPVCListener::OnMigoMessage( const UFC::AnsiString&, const UFC::AnsiString& Key,  MTree* Data )
{
    UFC::AnsiString Fill,PVCID,LineBroker;

    UFC::BufferedLog::Printf( " Receive Fill Execution from Host[%s]", Key.c_str() );
    if( Data->get( "FILL_ORDER", Fill ) && Data->get( "PVC", PVCID ) &&  Data->get( "PBNO", LineBroker ) )
    {
        UFC::BufferedLog::Printf( " Fill[%s]", Fill.c_str());
        FManager->ReceiveR3( (char*)Fill.c_str(), (char*)LineBroker.c_str(), (char*)PVCID.c_str() );
    }
}
//----------------------------------------------------------------------------------------------------------------------
BOOL SpeedyObjectsManager::FetchOrderInfo( R3Body* R3, Int32& OrigNID, UFC::AnsiString& Key, UFC::AnsiString& OID )
{  
    UFC::AnsiString BrokerID( R3->FBroker, 4 );
    ///< Use brokerID, Order ID as order unique key.
    OID = UFC::AnsiString( R3->FOrderNo, 5 );    
    if( OrderInfo.GetOrderInfo( OID, BrokerID, Key, OrigNID ) == FALSE )
    {   ///< Order not found.
        UFC::BufferedLog::Printf( " Can not get NID and Key for BrokerID[%s] OrderID[%s]", BrokerID.c_str(), OID.c_str() );
        Key = "null";
        OrigNID = 0;
        return FALSE;
    }
    return TRUE;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::ReceiveR3( char* Data, char* LinkBrokerID, char* PVCID )
{
    UFC::AnsiString logHead = "SpeedyObjectsManager::ReceiveR3()";
    int             Offset;
    R3Body          Msg( Data, Offset );
    int             OrigNID;
    BOOL            IsSpeedy;
    UFC::AnsiString RecvTime = "", TFXTime = "", Key = "", ExecKey = "", OID = "";    

    UFC::GetTimeString( RecvTime, FALSE );
    UFC::AnsiString StockNo(Msg.FStockID, 6);
    StockNo.TrimRight(' ');
    UFC::AnsiString OrderNo(Msg.FOrderNo, 5);
    //int SeqNo = Msg.GetSeq();
    int RecNo = Msg.GetRecNo();
    int MatchTime = Msg.ProcessingTime();
    char side = Msg.FBuySellCode;
    ///< Use line broker ID + report sequence + match time as unique key.
    ///< ExecKey.Printf( "%s_%08d_%08d%c", LinkBrokerID, MatchTime, Seq, side );
    ///< Use line report StockNo + RecNo + side as unique key.
    if (side == 'B')
        ExecKey.Printf( "%s_%s_1%08d", OrderNo.c_str(), StockNo.c_str(), RecNo );
    else if (side == 'S')
        ExecKey.Printf( "%s_%s_2%08d", OrderNo.c_str(), StockNo.c_str(), RecNo );
    else
        ExecKey.Printf( "%s_%s_%c%08d", OrderNo.c_str(), StockNo.c_str(), side, RecNo );
    UFC::BufferedLog::Printf( " %s Execution Key:%s.", logHead.c_str(), ExecKey.c_str() );
    if( OrderInfo.IsExecExist( ExecKey ) == FALSE ) ///< Execution not sent
    {
        MTree   FillData;
        TFXTime.Printf( "%09d", MatchTime * 10 );               
        IsSpeedy = FetchOrderInfo( /*In*/&Msg, /*Out*/OrigNID, /*Out*/Key, /*Out*/OID );
        FillData.append( "FILL_ORDER", Data );
        FillData.append( "PVC", PVCID );
        FillData.append( "PBNO",  LinkBrokerID );
        FillData.append( "NID",   OrigNID );
        FillData.append( "TTIME", TFXTime );
        FillData.append( "XTIME", RecvTime );
        FillData.append( "PHOST", UFC::Hostname );
        if( Key.Length() > 0 ) FillData.append( "KEY", Key );
        
        if( IsSpeedy == TRUE )
        {
//RM
            if( IsRiskManagerEnabled() && ( FRMObjPtr != 0 ) )
            {
                if( !FRMObjPtr->IsFillRptExecExist( ExecKey, UFC::rwLockForRead ) )
                {
                    FRMObjPtr->AddFillRptExec(ExecKey, UFC::rwLockForWrite);
                    FRMObjPtr->AddRiskMatchQty(&Msg, ExecKey); 
                }
                else
                    twserm::CTWSERiskManagerClass::GetLogObject()->fprintf(" %s [%s][%s] has Processed. skip it.", logHead.c_str(), OID.c_str(), ExecKey.c_str());
            }            
#ifdef __RMLib
            if (rm::IBasicRiskManagerObject::IsRMControlSetting())
            {
                if (!rm::rmObjPtr->IsFillRptExecExist(ExecKey, UFC::rwLockForRead))
                {
                    rm::rmObjPtr->AddFillRptExec(ExecKey, UFC::rwLockForWrite);
                    rm::rmObjPtr->AddRiskMatchQty(&Msg, "", ExecKey);
                }
                else
                    rm::WriteRiskLog(logHead, "[%s][%s] has Processed. skip it.", OID.c_str(), ExecKey.c_str());
            }
#endif 
//RM            
            MessageObj->Send( FFillReportSubject, UFC::Hostname, FillData );
            OrderInfo.WriteExecutionLog( FALSE, OrigNID, Key, Data, LinkBrokerID, UFC::Hostname, PVCID ); ///< Write log                       
        }
        else if( NotSpeedyToOthers == TRUE )
        {
            FillData.append( "KEY", "Others" );
            MessageObj->Send( FFillReportSubject, UFC::Hostname, FillData );
            OrderInfo.WriteExecutionLog( FALSE, OrigNID, Key, Data, LinkBrokerID, UFC::Hostname, PVCID ); ///< Write log                       
        }
        else
            MessageObj->Send( FNotSpeedyFillSubject, UFC::Hostname, FillData );                           
    }
    else
        UFC::BufferedLog::Printf( " %s Execution:%s already exist, skip it.", logHead.c_str(), ExecKey.c_str() );
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::SendExecutionToQueue( long MType, MTree* Data )
{
    UFC::MemoryStream QueueData( 1024 );

    try
    {
        Data->SaveToStream( &QueueData );
        UFC::UQueueData ExecData( MType, (char*)QueueData.GetBuffer(), 1024 );

        ExecutionQueue.Send( ExecData );
    }
    catch( UFC::QueueException& e )
    {
        UFC::BufferedLog::DebugPrintf( UFC::dlWarning, " Send Execution to queue failed:%s", e.what() );
    }
}
//------------------------------------------------------------------------------
void SpeedyObjectsManager::PushOrderRecord( Int32 NID, OrderRecord* OR )
{   
    UFC::PLockObject Lock( FOrdRecordCS );
    
    if( FOrderRecords.Add( NID, OR ) == FALSE )
        UFC::BufferedLog::Printf( " AddOrderRecord NID:%010u duplicate", (UInt32)NID );    
}
//------------------------------------------------------------------------------
OrderRecord* SpeedyObjectsManager::PopOrderRecord( Int32 NID )
{
    OrderRecord* OR = NULL;
    UFC::PLockObject Lock( FOrdRecordCS );
    
    if( FOrderRecords.GetObjectByKey( NID, OR ) == FALSE )            
        return NULL;
    FOrderRecords.DeleteByKey( NID );
    return OR;
}    
//------------------------------------------------------------------------------
void SpeedyObjectsManager::PushOrderRecord( const UFC::AnsiString& ClOrdID, OrderRecord* OR )
{   
    UFC::PLockObject Lock( FOrdRecordCS );
    
    if( FOrderRecordsGen5.Add( ClOrdID, OR ) == FALSE )
        UFC::BufferedLog::Printf( " AddOrderRecord ClOrdID[%s] duplicate", ClOrdID.c_str() );    
}
//------------------------------------------------------------------------------
OrderRecord* SpeedyObjectsManager::PopOrderRecord( const UFC::AnsiString& ClOrdID )
{
    OrderRecord* OR = NULL;
    UFC::PLockObject Lock( FOrdRecordCS );
    
    if( FOrderRecordsGen5.GetObjectByKey( ClOrdID, OR ) == FALSE )            
        return NULL;
    FOrderRecords.DeleteByKey( ClOrdID );
    return OR;
}    
//------------------------------------------------------------------------------
void SpeedyObjectsManager::RejectAllOrders( Int32 ErrCode )
{
    int RejectCount = 0;    
    
    UFC::BufferedLog::Printf( " Reject all orders in queue to client with error[%d].", ErrCode );    
    if( UseLockLessQueue == FALSE )
    {        
        UFC::UQueueData Order( 0, MAX_DATA_SIZE );
	    Order.SetMType( 0 );
    	while( FQueue.RecvNoWait( Order ) == TRUE  )///< No order in queue, return
	    {    
    	    OrderRecord  OI;         
        	RejectCount++;
	        OI.FromBinary( FALSE, (UInt8*)Order.GetData() );  
    	    SpeedyReject( OI, ErrCode, TRUE );        
        	UFC::SleepMS( 5 );
	        Order.SetMType( 0 );
	    }
    }
    else
    {
        OrderRecord* Order;
    
        while( FMPMCQueue->pop(&Order ) == true )
        {    
            RejectCount++;            
            SpeedyReject( *Order, ErrCode, TRUE );        
            UFC::SleepMS( 5 );
            delete Order;
        }    
    }
    UFC::BufferedLog::Printf( " Reject %d Orders to client.", RejectCount );    
}
//-----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::OtherSysExecution( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  Int32 NID, const UFC::AnsiString& Msg, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrokerID, const UFC::AnsiString& ExTime, Int32 IsConfirm )
{   
      
    if( IsConfirm == TRUE ) ///< R020, R100
    {        
        TWSE::MessageHeader*  ReplyMsg = TWSE::MessageHeader::CreateMessage( ( const char*)Msg.c_str(), Msg.Length() );
        if( ReplyMsg != NULL )
        {   
            OrderRecord* OI = OrderRecodPoolPtr->GetInstance();
            if( OI != NULL )
            {            
                BOOL IsNew;
                int  KeyLen = Key.Length();
            
                if( KeyLen > 0 && KeyLen < 256 )
                {
                    OI->InitTWSE( NID, ReplyMsg->GetFunctionCode(), Key, Msg );                                            
                    RecordInfo( ReplyMsg, *OI, IsNew, FALSE );        
                    SendReply( Subject, Key, NID , Msg, PVCID, BrokerID, ExTime );
                }
                else
                    UFC::BufferedLog::Printf( " OtherSysExecution wrong Key ." );
            }
            else
                UFC::BufferedLog::Printf( " OtherSysExecution new  OrderRecord failed." );
        }
        else
            UFC::BufferedLog::Printf( " Unknown TMP[%s]", Msg.c_str() );
    }
    else ///< Filled
        SendFill( Subject, Key, NID , Msg, PVCID, BrokerID, ExTime );
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::SendFill( const UFC::AnsiString& Subject, const UFC::AnsiString& AE,  Int32 NID, const UFC::AnsiString& Data, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrokerID, const UFC::AnsiString& ExTime)
{
    UFC::AnsiString logHead = "SpeedyObjectsManager::SendFill()";
    int             Offset;
    R3Body          Msg( Data.c_str(), Offset );
    int             OrigNID = NID;
    UFC::AnsiString RecvTime, ExecKey, OID;     
    UFC::AnsiString Key = AE;
    

    UFC::GetTimeString( RecvTime, FALSE );
    UFC::AnsiString StockNo( Msg.FStockID, 6 );
    UFC::AnsiString OrderNo( Msg.FOrderNo, 5 );    
    StockNo.TrimRight(' ');

    int RecNo = Msg.GetRecNo();
    char side = Msg.FBuySellCode;
    ///< Use line broker ID + report sequence + match time as unique key.
    ///< ExecKey.Printf( "%s_%08d_%08d%c", LinkBrokerID, MatchTime, Seq, side );
    ///< Use line report StockNo + RecNo + side as unique key.
    if (side == 'B')
        ExecKey.Printf( "%s_%s_1%08d", OrderNo.c_str(), StockNo.c_str(), RecNo );
    else if (side == 'S')
        ExecKey.Printf( "%s_%s_2%08d", OrderNo.c_str(), StockNo.c_str(), RecNo );
    else
        ExecKey.Printf( "%s_%s_%c%08d", OrderNo.c_str(), StockNo.c_str(), side, RecNo );
    UFC::BufferedLog::Printf( " %s Execution Key:%s.", logHead.c_str(), ExecKey.c_str() );
    if( OrderInfo.IsExecExist( ExecKey ) == FALSE ) ///< Execution not sent
    {
        MTree   FillData;
        
        FetchOrderInfo( /*In*/&Msg, /*Out*/OrigNID, /*Out*/Key, /*Out*/OID );
        FillData.append( "FILL_ORDER", Data );
        FillData.append( "PVC", PVCID );
        FillData.append( "PBNO",  BrokerID );
        FillData.append( "NID",   OrigNID );
        FillData.append( "TTIME", ExTime );
        FillData.append( "XTIME", RecvTime );
        FillData.append( "PHOST", UFC::Hostname );
        FillData.append( "KEY", Key );
        
        MessageObj->Send( Subject, UFC::Hostname, FillData );
        OrderInfo.WriteExecutionLog( FALSE, OrigNID, Key, Data, BrokerID, UFC::Hostname, PVCID ); ///< Write log                       
        
    }
    else
        UFC::BufferedLog::Printf( " %s Execution:%s already exist, skip it.", logHead.c_str(), ExecKey.c_str() );
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyObjectsManager::SendReply(  const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  Int32 NID, const UFC::AnsiString& Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrokerID, const UFC::AnsiString& ExTime )
{
    MTree           Data;
    UFC::AnsiString X25Time;    
    
    UFC::GetTimeString( X25Time, FALSE );
    Data.append( "CONFIRM_ORDER", Order );
    Data.append( "PVC" , PVCID );
    Data.append( "PBNO", BrokerID );
    Data.append( "NID",  NID );
    Data.append( "TTIME", ExTime );
    Data.append( "XTIME", X25Time );
    Data.append( "KEY", Key );
    Data.append( "HOST", UFC::Hostname );         
    Data.append( "PHOST", UFC::Hostname );         
    OrderInfo.WriteExecutionLog( TRUE, NID, Key, Order, BrokerID,UFC::Hostname, PVCID ); ///< Write log
    MessageObj->Send( Subject, UFC::Hostname, Data  );         
}
//---------------------------------------------------------------------------------------------------------------------
// MsgType(mtT010,mtO010,mtP010)
// FuncCode(1,2,3,4,5)
// *OrderID   OK
// *NID,      OK
// *Key,      OK  
// Host name 
//---------------------------------------------------------------------------------------------------------------------
TWSE::OrderInfo* SpeedyObjectsManager::AddTOPRecordInfo( Int32 Func, const UFC::AnsiString& OID, const UFC::AnsiString& BrokerID, OrderRecord& OI, BOOL& IsNew, BOOL Force )
{    
    TWSE::OrderInfo* OrdInfo = NULL;
    switch( Func )
    {
        case 1: ///< New Order, Buy                
        case 2: ///< New Order, Sell
                IsNew = TRUE;
                OrdInfo = OrderInfo.AddOrder( /*In*/OID, /*In*/BrokerID, /*In*/OI, /*In*/Force );
                break;
        case 3: ///< Reduce Qty   
        case 4: ///< Cancel Order
        case 6: ///< Replace Price
                IsNew = FALSE;
                if( OrderInfo.GetOrderInfo( OID, BrokerID, OrdInfo ) == FALSE ) 
                    OrdInfo = OrderInfo.AddOrder( /*In*/OID, /*In*/BrokerID, /*In*/OI, /*In*/Force );///< Not Speedy Order.
                break;
        case 5: ///< Skip order query
                IsNew = FALSE;
                break;  
    }    
    return OrdInfo;
}
//---------------------------------------------------------------------------------------------------------------------
TWSE::OrderInfo* SpeedyObjectsManager::AddAEVRecordInfo( Int32 Func, const UFC::AnsiString& OID, const UFC::AnsiString& BrokerID, OrderRecord& OI, BOOL& IsNew, BOOL Force )
{
    TWSE::OrderInfo* OrdInfo = NULL;
    switch( Func )
    {
        case 1: ///< New Order
                IsNew = TRUE;
                OrdInfo = OrderInfo.AddOrder( /*In*/OID, /*In*/BrokerID, /*In*/OI, /*In*/Force );///< New Order, Sell
                break;
        case 2: ///<  Cancel Order 
        case 3: ///<  Reduce Qty
                IsNew = FALSE;
                if( OrderInfo.GetOrderInfo( OID, BrokerID, OrdInfo ) == FALSE ) 
                    OrdInfo = OrderInfo.AddOrder( /*In*/OID, /*In*/BrokerID, /*In*/OI, /*In*/Force );///< Not Speedy Order.
                break;
        case 4: ///< Skip order status query 
                IsNew = FALSE;
                break; 
    }    
    return OrdInfo;
}
//---------------------------------------------------------------------------------------------------------------------
TWSE::OrderInfo* SpeedyObjectsManager::RecordInfo( MessageHeader* OrderMessage, OrderRecord& OI, BOOL& IsNew, BOOL Force )
{
    TMessageType    Type = OrderMessage->MessageType( );
    int             Func = OrderMessage->GetFunctionCode();
    UFC::AnsiString OID,BrokerID;
    MessageT020*    T020;
    MessageO020*    O020;
    MessageO120*    O120;
    MessageP020*    P020;
    MessageA020*    A020;
    MessageE020*    E020;
    MessageEx020*   Ex020;
    MessageV020*    V020;    
    TWSE::OrderInfo*    OrdInfo = NULL;
    
    switch( Type )
    {
        case mtT020:   T020     = (MessageT020*)OrderMessage;
                       OID      = UFC::AnsiString( T020->OrderNo, 5 );
                       BrokerID = UFC::AnsiString( T020->BrokerID, 4 );
                       OrdInfo = AddTOPRecordInfo( Func,OID,BrokerID,OI,IsNew, Force);
                       break;
        case mtO020:   O020     = (MessageO020*)OrderMessage;
                       OID      = UFC::AnsiString( O020->OrderNo, 5 );
                       BrokerID = UFC::AnsiString( O020->BrokerID, 4 );
                       OrdInfo = AddTOPRecordInfo( Func,OID,BrokerID,OI,IsNew, Force);
                       break;
        case mtO120:   O120     = (MessageO120*)OrderMessage;
                       OID      = UFC::AnsiString( O120->OrderNo, 5 );
                       BrokerID = UFC::AnsiString( O120->BrokerID, 4 );
                       OrdInfo = AddTOPRecordInfo( Func,OID,BrokerID,OI,IsNew, Force);
                       break;                       
        case mtP020:   P020     = (MessageP020*)OrderMessage;
                       OID      = UFC::AnsiString( P020->OrderNo, 5 );
                       BrokerID = UFC::AnsiString( P020->BrokerID, 4 );
                       OrdInfo = AddTOPRecordInfo( Func,OID,BrokerID,OI,IsNew, Force);
                       break;
        case mtA020:   A020     = (MessageA020*)OrderMessage;
                       OID      = UFC::AnsiString( A020->OrderNo, 5 );
                       BrokerID = UFC::AnsiString( A020->BrokerID, 4 );
                       OrdInfo = AddAEVRecordInfo( Func,OID,BrokerID,OI,IsNew, Force);                       
                       break;
        case mtE020:   E020     = (MessageE020*)OrderMessage;
                       OID      = UFC::AnsiString( E020->OrderNo, 5 );
                       BrokerID = UFC::AnsiString( E020->BrokerID, 4 );
                       OrdInfo = AddAEVRecordInfo( Func,OID,BrokerID,OI,IsNew, Force);
                       break;
        case mtEx020:  Ex020    = (MessageEx020*)OrderMessage;
                       OID      = UFC::AnsiString( Ex020->OrderNo, 5 );
                       BrokerID = UFC::AnsiString( Ex020->BrokerID, 4 );
                       OrdInfo = AddAEVRecordInfo( Func,OID,BrokerID,OI,IsNew, Force);
                       break;
        case mtV020:   V020     = (MessageV020*)OrderMessage;
                       OID      = UFC::AnsiString( V020->OrderNo, 5 );
                       BrokerID = UFC::AnsiString( V020->BrokerID, 4 );
                       OrdInfo = AddAEVRecordInfo( Func,OID,BrokerID,OI,IsNew, Force);
                       break;
        default:       break; ///< Not an Order message    
    }
    return OrdInfo;
}
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------
