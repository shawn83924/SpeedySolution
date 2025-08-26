#include "TMPOrderObjectsManager.h"
#include "LinkSubSystem.h"
#include "../TMP34/OdrUtility.h"
//-----------------------------------------------------------------------------------------------------------------------
const int ORD_HEARTBEAT_INTERVAL  =55;
//-----------------------------------------------------------------------------------------------------------------------
extern BOOL                     SendKnife;
extern UFC::AnsiString          MatchFileName;
extern UFC::AnsiString          OutputDir;
extern UFC::AnsiString          MulticastAddress;
extern UFC::AnsiString          IOLogPath;
extern Int32                    WriteTransBufferSize;
extern Int32                    WriteTransTimeout; 
extern Int32                    ExecSrc;
extern Int32                    RejectOrderSec;
extern Int32                    AppPrefixIndex;
//------------------------------------------------------------------------------    
void  ProductGroup::RegTradingSession( UInt16 FCM, UInt16 Session,UInt16 ReqID )
{
    UInt32 UID = FCM << 16 | Session;    
    TreadStatRegInfo* RegInfo;
    UFC::PLockObject Lock( FRegCS );
    
    if( FRegSet.Exists( UID ))
    {
        RegInfo = FRegSessionsMap.GetObjectByKey( UID );
        RegInfo->FReqID = ReqID; ///< Overwrite.
    }
    else
    {
        RegInfo = new TreadStatRegInfo( FCM, Session ,ReqID );
        FRegSet.Add( UID );
        FRegSessionsMap.Add( UID, RegInfo ); 
        FRegSessionsList.Add( RegInfo ); 
    }
}
//------------------------------------------------------------------------------    
BOOL  ProductGroup::UnregTradingSession( UInt16 FCM, UInt16 Session )
{
    UInt32 UID = FCM << 16 | Session;    
    TreadStatRegInfo* RegInfo;
    UFC::PLockObject Lock( FRegCS );
    
    if( FRegSet.Exists( UID ))
        FRegSet.Delete( UID );
    if( (RegInfo = FRegSessionsMap.GetObjectByKey( UID )) != NULL )
        FRegSessionsMap.DeleteByKey( UID );
    for( Int32 i = 0; i < FRegSessionsList.ItemCount(); i++ )
    {
        RegInfo = FRegSessionsList.GetItem( i );
        if(RegInfo != NULL && RegInfo->FCMID == FCM && RegInfo->FSessionID == Session )
        {
            FRegSessionsList.Delete( i );
            delete RegInfo;
            return TRUE;
        }
    }
    return FALSE;
}        
//------------------------------------------------------------------------------            
BOOL  ProductGroup::GetSession( Int32 Index, UInt16& FCM, UInt16& SID, UInt16& ReqID  )
{
    TreadStatRegInfo* RegInfo;
    UFC::PLockObject Lock( FRegCS );
   
    RegInfo = FRegSessionsList.GetItem( Index );
    if( RegInfo != NULL )
    {
        FCM   = RegInfo->FCMID;
        SID   = RegInfo->FSessionID;
        ReqID = RegInfo->FReqID;
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
ExecWriter* ReportListener::GetExecWriter(UInt32 FCMID, UInt32 SessionID)
{
    ExecWriter* Writer;
    UInt32 SID = FCMID << 16 | SessionID;

    if ((Writer = FSessionWriters.GetObjectByKey(SID)) == NULL)
    { ///< Writer not exists, create a new one
        UFC::AnsiString FileName;

        FileName.Printf("%sFCM_%05d_Session_%04d", OutputDir.c_str(), FCMID, SessionID);
        Writer = new ExecWriter(FileName, WriteTransTimeout);
        FSessionWriters.Add(SID, Writer);
        return Writer;
    }
    else
        return Writer;
}
//------------------------------------------------------------------------------
void ReportListener::OnTransaction(UInt8 Type, void* Data)
{
    if( Type == TAG_RPT )
    {
        FReport = (odm_Rpt*) Data;                
        if( FTMPOrderManager->GetSession( FReport->header.source_fcm_id, FReport->header.session_id ) !=NULL )
        {            
            FRptWriter = GetExecWriter(FReport->header.source_fcm_id, FReport->header.session_id);
            FRptWriter->WriteTransaction((UInt8*) Data);        
            if( ( ExecSrc & esMatchFile ) != 0 )
                FTMPOrderManager->OnExecution( esMatchFile, FReport );
        }        
    }
}
//-----------------------------------------------------------------------------------------------------------------------
//
//  TAIFEX side( TAIFEXOrderManeger )
//
//  Objects Manager for AP code 4 Order subsystem
//  
//-----------------------------------------------------------------------------------------------------------------------
TMPOrderManager::TMPOrderManager( int Port, const UFC::AnsiString& OdrIP, int OdrPort, const UFC::AnsiString& DBFileName, const UFC::AnsiString& LogDate )
:FListenPort( (Port%10000)+20000 ),
 FGroupCount( 0 ),
 FOdrServerIP( OdrIP ),
 FOdrServerPort( OdrPort ),        
 IsODRSConnected( FALSE ),
 FOdrIdleTime( 0 ),
 FMaxOrderTimeFCM( 0 ),
 FMaxOrderTimeSessionID( 0 ),
 FMaxOrderResponseTime( 0 ),
 FOrderCount( 0 ),
 FTotalOrderResponseTime( 0 ),
 FMaxQueryTimeFCM( 0 ),
 FMaxQueryTimeSessionID( 0 ),
 FMaxQueryResponseTime( 0 ),
 FQueryCount( 0 ),
 FTotalQueryResponseTime( 0 ),
 FRecovering( TRUE ),
 FBreakTrading( FALSE )
{
    UFC::AnsiString LogFileName;
    OdrUtility::InitOdrUtility();

    LogFileName.Printf( "%s/t%s.4.%d",IOLogPath.c_str(),LogDate.c_str(), FListenPort );
    FLog                = new LogManager( LogFileName );
    FTransactionObject  = new TMPTransactionObject( this, DBFileName );
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Create Execution report listener." );
    FExecReportListener = new ReportListener( this );
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Create Execution report reader." );
    FExecReportReader   = new ExecReader( MatchFileName, FExecReportListener, 0 );       
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Create Execution Multicast listener,IP[%s] Port[%d]", MulticastAddress.c_str(), FListenPort );
    FUDPClient = new UFC::PUDPClient( "any", FListenPort, TRUE );
    FUDPClient->SetListener( this );
    FUDPClient->Open();
    FUDPClient->SetMulticastAddress( MulticastAddress, 0 );
    FUDPClient->SetRecvBufferSize( 512*1024 );
    FUDPClient->SetMulticastTTL( 8 );
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Execution Multicast IP[%s] Port[%d]", MulticastAddress.c_str(), FListenPort );
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Create Odr_server socket,IP[%s] Port[%d]", OdrIP.c_str(), OdrPort );
    FOdrSocketClient    = new UFC::PClientSocket( OdrIP, OdrPort, TRUE );
    FOdrSocketClient->SetListener( this );
}
//-----------------------------------------------------------------------------------------------------------------------
TMPOrderManager::~TMPOrderManager()
{
    if( FTransactionObject != NULL )
	delete FTransactionObject;	
    if( FLog != NULL )		
	delete FLog;
}
//------------------------------------------------------------------------------
void TMPOrderManager::OnDataArrived( UFC::PUDPClient* , UFC::MemoryStream* Stream, const UFC::AnsiString& Address )
{
    UFC::BufferedLog::DebugPrintf( UFC::dlPerformance,AppPrefixIndex," Tick:%u Get Multicast notify size:%d from:%s", UFC::GetTickCountMS(),Stream->GetSize(), Address.c_str()   );
    
    if( Stream->GetSize() >= (Int32)sizeof( tag_rpt_t ) )    
    {
        UInt8* CurrentPos = (UInt8*)Stream->GetBuffer();
        
        if( *CurrentPos == TAG_RPT && ( ExecSrc & esMulticast ) != 0 )             
            OnExecution( esMulticast, (odm_Rpt*) CurrentPos );                    
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::BroadcastTradingStatus( ProductGroup* ProdGroup, UInt8 Group, Int32 ForceOS )
{
    UInt16 FCM,SID,ReqID;
    Int32  NewOS = ProdGroup->GetOpenSwitch();

    if( ForceOS > 0 )
        NewOS = ForceOS;
    ///< Broadcast Trading session.
    for( Int32 i=0;i< ProdGroup->ItemCount(); i++ )
    {
       if( ProdGroup->GetSession( i, FCM, SID, ReqID ) )
       {
           ConnectionObjectBase* ConnectionObj = GetSession( FCM, SID );
           if( ConnectionObj != NULL )
           {        
               TMP::TMPMessage R12( TMP::tmtR12 );

               R12["status_code"]   = 0; ///< Always OK.
               R12["TradeReqID"]    = ReqID;
               R12["flow_group_no"] = Group;               
               switch( NewOS )
               {
                   case 0:   R12["TradeStatus"] = 1; break; ///< Break
                   case 10:  R12["TradeStatus"] = 4; break;
                   case 30:  R12["TradeStatus"] = 2; break;
                   case 100: R12["TradeStatus"] = 3; break;
                   default:  R12["TradeStatus"] = 0; break;
               }
               ConnectionObj->OnCommand( EVENT_TMP_EXECUTION, (void*)&R12 );
           }
       }    
   }            
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::OnTradingBreak( int OS  )
{
    ProductGroup* ProdGroup;
    if( OS == 0 ) ///< Stop trading.
    {
        FBreakTrading = TRUE;
        for( int i = 0; i < FGroups.ItemCount(); i++ )
        {
            ProdGroup = FGroups.GetItem( i );
            FTransactionObject->OnTradingStatusChange( ProdGroup->GetGroup(), 0 );
            BroadcastTradingStatus( ProdGroup, ProdGroup->GetGroup(), 0 ); ///< Send to
        }
    }
    else if( OS == 10 ) ///< Accept order
    {
        for( int i = 0; i < FGroups.ItemCount(); i++ )
        {
            ProdGroup = FGroups.GetItem( i );
            if( ProdGroup->GetOpenSwitch() >= 100 )
            {
                FTransactionObject->OnTradingStatusChange( ProdGroup->GetGroup(), 100 );
                BroadcastTradingStatus( ProdGroup, ProdGroup->GetGroup(), 100 ); ///< Send to
            }
            else
            {
                FTransactionObject->OnTradingStatusChange( ProdGroup->GetGroup(), 10 );
                BroadcastTradingStatus( ProdGroup, ProdGroup->GetGroup(), 100 ); ///< Send to
            }
        }
    }
    else if( OS == 30 ) ///< Restore
    {
        FBreakTrading = FALSE;
        for( int i = 0; i < FGroups.ItemCount(); i++ )
        {
            ProdGroup = FGroups.GetItem( i );
            FTransactionObject->OnTradingStatusChange( ProdGroup->GetGroup(), ProdGroup->GetOpenSwitch() );
            BroadcastTradingStatus( ProdGroup, ProdGroup->GetGroup(), ProdGroup->GetOpenSwitch() ); ///< Send to
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::OnOpenSwitch( int Group, int Value, int Seq   )
{
    if( Value == 0 || Value == 10 || Value == 30 || Value == 95 || Value == 100 )
    {
        ProductGroup* ProdGroup   = FGroups.GetObjectByKey( Group );
        Int32         MarketState = 0;
    
        if( ProdGroup == NULL ) ///< Group use 10,30,100 as open switch values.
        {
            ProdGroup = new ProductGroup( Group );
            FGroups.Add( Group, ProdGroup );
        }    
        switch( Value ) ///< Convert OpenSwitch value to match market state value.
        {
            case 10:  MarketState = ST_RECV;//< Begin receive order.
                      UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Set Group[%d] to state[%d,ST_RECV]",Group, Value );
                      break;
            case 30:  MarketState = ST_OPEN;//< Market Opened.
                      UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Set Group[%d] to state[%d,ST_OPEN]",Group, Value );
                      break;
            case 95:  if( Value > ProdGroup->GetOpenSwitch() )
                      {
                          ProdGroup->SetOpenSwitch( Value, Seq );
                          if( FTransactionObject != NULL )
                              FTransactionObject->OnTradingStatusChange( Group, Value );
                      }
                      UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Group[%d] Day trade off",Group );
                      return;
            case 100: MarketState = ST_CLOSE;//< Market closed.
                      UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Set Group[%d] to state[%d,ST_CLOSE]",Group,Value );
                      break;
            default:  if( Value > 100 )
                      {
                          MarketState = ST_CLOSE;//< Market closed.
                          break;
                      }
                      else
                        return;
        }
        if( Value > ProdGroup->GetOpenSwitch() )
        {
            ProdGroup->SetOpenSwitch( Value, Seq );
            if( FBreakTrading == FALSE )
            {
                if( FTransactionObject != NULL )
                    FTransactionObject->OnTradingStatusChange( Group, Value );
                if( FOdrSocketClient->IsConnect() == TRUE )
                    OdrUtility::SendOpenMarketKnife( FOdrSocketClient, FListenPort, Group, MarketState, Seq );
                BroadcastTradingStatus( ProdGroup, Group ); ///< Send to
            }
        }        
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::CreateConnectionObjects( TMPSessions* Sessions )
{
    int Count =  Sessions->GetCount();
    
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " ---------------------- Create connections ----------------------" );
    for( int i = 0; i < Count ; i++ ) 
    {
        ConnectionParameter cp( FListenPort , Sessions->GetSessionID( i ), '4', Sessions->GetBrokerID(i), Sessions->GetPassword(i)
                                ,Sessions->GetPeerIP( i ), TMP_HEARTBT_INT, Sessions->GetThroughput(i), Sessions->FTPNotify(i) );
        TMPOrderConnectionObject* ConnectionObject;
        UFC::PLockObject   Lock( FCriticalSection );
        
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Create TMP Session FCM:[%s] FCMID[%d] SessionID[%d]",cp.GetBrokerID().c_str(), cp.GetBrokerIndex(),cp.GetSessionID());
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, "                    HeartBtInt[%d sec] Throughput[%d Odr/sec]",cp.GetHeartBtInt(),cp.GetThroughput());
        ConnectionObject = new TMPOrderConnectionObject( cp, FTransactionObject, this, this );
        FConnectionObjects.Add((void*)ConnectionObject);        
        ConnectionObject->SetLogManager( FLog );
        UInt32 SessionKey = ( cp.GetBrokerIndex() << 16 | cp.GetSessionID());
        FSessionsTable.Add( SessionKey,ConnectionObject );
        FSessionsArray.Add( ConnectionObject );
    }
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " -------------- Recover Ececutins from match file ----------------" );
    FExecReportReader->ReadToEOF();
    FRecovering = FALSE;
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " ----------------- Activate all Session threads ------------------" );
    for( int i = 0; i < FSessionsArray.ItemCount(); i++ )
         FSessionsArray.GetItem( i )->Active( );
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " ---------------- Start Execution reader thread ------------------" );
    if( ( ExecSrc & esMatchFile ) != 0 )
        FExecReportReader->Start();            
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::ResetPVC( int BrokerIndex, int SID )
{
    ConnectionObjectBase* co = GetSession( BrokerIndex,SID );
    if( co != NULL )
    {
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Unlock FCM[%d] TMP Session[%d]", BrokerIndex, SID );
        co->OnCommand( COMMAND_UNLOCK_PVC, NULL );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
int TMPOrderManager::GetPVCState( int BrokerIndex, int SID, Int32& Connected, Int32& Bandwidth )
{
    int State = PVC_STATUS_UNKNOWN;
    ConnectionObjectBase* co = GetSession( BrokerIndex,SID );
    
    if( co != NULL )
    {
        co->OnCommand( COMMAND_GET_STATE, (void*) &State );
        co->OnCommand( COMMAND_GET_BANDWIDTH,(void*) &Bandwidth );
        if( co->IsReady() )
            Connected = 1; ///<Connected.
        else
            Connected = 0; ///< Disconnect
        if( State == PVC_STATUS_SUBSYS_ERR  ||
            State == PVC_STATUS_PW_ERR  ||
            State == PVC_STATUS_FCM_ERR ||
            State == PVC_STATUS_AP_ERR  ||
            State > 1000  )
        Connected = 2; ///< Error

        return State;
    }
    else
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " This TMP34 not own Session[%d] FCM[%d]", SID, BrokerIndex );
    Connected = 0;
    return PVC_STATUS_UNKNOWNPVC;
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::StopConnection( int BrokerIndex,int SID )
{
    ConnectionObjectBase* lCobj = GetSession(BrokerIndex, SID );
    if( lCobj != NULL )
    {
	ConnectionParameter* cp = lCobj->GetConnectionParameter();
	UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " FCM:%s SID:%d perform backup procedure.(Locked)",cp->GetBrokerID().c_str(),SID);
	lCobj->OnCommand( COMMAND_LOCK_PVC, NULL );
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::OnTerminate( UFC::PThread* TerminateThread  )
{
    UFC::PLockObject Lock( FCriticalSection );
    
    FConnectionObjects.Remove((void*)TerminateThread);
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::SendRegFCM( ConnectionParameter* cp, BOOL Force )
{
    int FCMID = cp->GetBrokerIndex();
    int SessionID = cp->GetSessionID();
    
    if( IsRegistered( FCMID,SessionID ) == FALSE ||  Force == TRUE )
    {
        if( IsODRSConnected == TRUE )
        {
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Send register FCM:%d SessionID:%d ",FCMID, SessionID);
            OdrUtility::SendFCMReg( FOdrSocketClient, FCMID, SessionID, FListenPort );
        }
        else
        {
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Register FCM:%d SessionID:%d when odr_server disconnected",FCMID, SessionID);
            RegFCM( FCMID, SessionID );
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::SendUnregFCM( ConnectionParameter* cp )
{
    int FCMID = cp->GetBrokerIndex();
    int SessionID = cp->GetSessionID();
    
    if( IsRegistered( FCMID,SessionID ) == TRUE )
    {
        if( IsODRSConnected == TRUE )
        {
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Send unregister FCM:%d SessionID:%d ",FCMID, SessionID);
            OdrUtility::SendFCMUnreg( FOdrSocketClient, FCMID, SessionID, FListenPort );
        }
        else
        {
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex, " Unregister FCM:%d SessionID:%d when odr_server disconnected",FCMID, SessionID);
            UnregFCM( FCMID, SessionID );
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::RegFCM( UInt32 FCMID, UInt16 SID )
{    
    UInt32 RegKey = (FCMID<< 16 | SID );
            
    UFC::PLockObject Lock( FRegTableCS );
    FRegTable.Add( RegKey );
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::UnregFCM( UInt32 FCMID, UInt16 SID )
{  
    UInt32 RegKey = (FCMID<< 16 | SID );
            
    UFC::PLockObject Lock( FRegTableCS );
    FRegTable.Delete( RegKey );
}
//-----------------------------------------------------------------------------------------------------------------------
BOOL TMPOrderManager::IsRegistered( UInt32 FCMID, UInt16 SID )
{
    UInt32 RegKey = (FCMID<< 16 | SID );
            
    UFC::PLockObject Lock( FRegTableCS );
    return FRegTable.Exists( RegKey );
}
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase* TMPOrderManager::GetSession(  UInt32 SID  )
{
    UFC::PLockObject Lock( FRegTableCS );
    return FSessionsTable.GetObjectByKey( SID );    
}        
//-----------------------------------------------------------------------------------------------------------------------
ConnectionObjectBase* TMPOrderManager::GetSession( UInt32 FCMID, UInt16 SID  )
{
    UInt32 RegKey = (FCMID<< 16 | SID );
            
    UFC::PLockObject Lock( FRegTableCS );
    return FSessionsTable.GetObjectByKey( RegKey );
}        
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::HandleLogin( odm_RegConfirm*  Login )
{
    if( Login->status_code == 0 )
    {
        IsODRSConnected = TRUE;
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Login odr_server ok.");
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Register all connected FCMs." );
        UFC::PLockObject Lock( FRegTableCS );
        
        for( int i=0; i < FRegTable.ItemCount(); i++ )
        {
            ConnectionObjectBase* RegConnection = GetSession( FRegTable.GetItem(i) );       
            if(  RegConnection != NULL )
                 SendRegFCM( RegConnection->GetConnectionParameter(), TRUE );
        }
        if( SendKnife == TRUE )
        {
            OdrUtility::SendOpenMarketKnife( FOdrSocketClient, FListenPort,1, ST_RECV );
            OnOpenSwitch( 1, 30, 0 );
            OdrUtility::SendOpenMarketKnife( FOdrSocketClient, FListenPort,2, ST_RECV );
            OnOpenSwitch( 2, 30, 0 );
            SendKnife = FALSE;             
        }
        else
        {
            ProductGroup* Group;
            int MarketState;

            for( int i = 0; i < FGroups.ItemCount(); i ++ )
            {
                if( (Group = FGroups.GetItem( i )) != NULL )
                {
                    if( Group->GetOpenSwitchSeq() != 0  )
                    {
                        switch( Group->GetOpenSwitch() ) ///< Convert OpenSwitch value to match market state value.
                        {
                            case 10:  MarketState = ST_RECV;  break; //< Begin receive order.
                            case 30:  MarketState = ST_OPEN;  break; //< Market Opened.
                            case 100: MarketState = ST_CLOSE; break; //< Market closed.
                            default:  continue;;
                        }
                        OdrUtility::SendOpenMarketKnife( FOdrSocketClient, FListenPort, Group->GetGroup(), MarketState, Group->GetOpenSwitchSeq() );
                    }
                }
            }
        }
    }
    else
    {
        IsODRSConnected = FALSE;
        switch( Login->status_code )
        {
            case 1:UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," client_id:%d already logon.",FListenPort%1000);break;
            case 2:UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," source_fcm_id out of bound.");break;
            case 3:UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," session_id out of bound.");break;
            case 4:UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," client_id out of bound.");break;
            case 5:UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," invalid protocol value.");break;
        }
    }
}
//-----------------------------------------------------------------------------------------------------------------------
Int16 TMPOrderManager::SubscribeTradeSessionStatus( UInt32 FCMID, UInt16 SID, UInt8 Group, UInt16 ReqID )
{
    ProductGroup* ProdGroup = FGroups.GetObjectByKey( Group );    
    
    if( ProdGroup != NULL ) 
    {
        ProdGroup->RegTradingSession( FCMID, SID, ReqID );
        switch( ProdGroup->GetOpenSwitch() )
        {
           case 10:  return 4;
           case 30:  return 2;
           case 100: return 3;
           default:  return 0;
        }
    }
    return -1;
}
//-----------------------------------------------------------------------------------------------------------------------
Int16 TMPOrderManager::UnsubscribeTradeSessionStatus( UInt32 FCMID, UInt16 SID, UInt8 Group, UInt16  )
{
    ProductGroup* ProdGroup = FGroups.GetObjectByKey( Group );    
    
    if( ProdGroup != NULL ) 
    {
        ProdGroup->UnregTradingSession( FCMID, SID );
        switch( ProdGroup->GetOpenSwitch() )
        {
           case 10:  return 4;
           case 30:  return 2;
           case 100: return 3;
           default:  return 0;
        }
    }
    return -1;
}
//-----------------------------------------------------------------------------------------------------------------------
Int16 TMPOrderManager::AskTradeSessionStatus( UInt32 , UInt16 , UInt8 Group )
{
    ProductGroup* ProdGroup = FGroups.GetObjectByKey( Group );    
    
    if( ProdGroup != NULL ) 
        return ProdGroup->GetOpenSwitch();
    return -1;
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::HandleRegFCM( odm_FCM_RegConfirm* Reg )
{
    if( Reg->status_code == 0 )
    {
        RegFCM( Reg->header.source_fcm_id, Reg->header.session_id );
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Register FCM:%d Session:%d OK,Seq[%d].", Reg->header.source_fcm_id, Reg->header.session_id, Reg->last_sess_seq );
    }
    else
    {
        switch( Reg->status_code )
        {
            case  1:UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," FCM:%d Session:%d already logon.",Reg->header.source_fcm_id, Reg->header.session_id);break;
            case  2:UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," TMP34[%d] not logon.",FListenPort%1000 );break;
            default:UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Register FCM[%d] Session[%d] failed, Code[%d].",Reg->header.source_fcm_id, Reg->header.session_id, Reg->status_code);
                    break;
        }        
    }    
}
//-----------------------------------------------------------------------------------------------------------------------                                         
void TMPOrderManager::HandleUnregFCM( odm_FCM_UnregConfirm* Unreg )
{
    if( Unreg->status_code == 0 )
    {
        UnregFCM( Unreg->header.source_fcm_id, Unreg->header.session_id );
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Unegister FCM:%d Session:%d OK.", Unreg->header.source_fcm_id, Unreg->header.session_id );
    }
    else
    {
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Unregister FCM[%d] Session[%d] failed, Code[%d].",Unreg->header.source_fcm_id, Unreg->header.session_id, Unreg->status_code);
    }
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::OnExecution( ExecutionSource Src, odm_Rpt* Report )
{
    TMPOrderConnectionObject* ConnectionObj;
    int BrokerIndex = Report->header.source_fcm_id;
    int SessionID   = Report->header.session_id;   
        
    if( (ConnectionObj=static_cast<TMPOrderConnectionObject*>(GetSession( BrokerIndex, SessionID ))) != NULL )
    {   
        if( Report->session_seq == 0  ) ///< Reply for Order ststus request
        {
            if( Src == esMatchFile && FRecovering == FALSE ) ///< ignore execution from multicase.
            {
                TMP::TMPMessage* ReportMsg = OdrUtility::odmRptToTMPMessage( Report, ConnectionObj->IsCompactExec() );
                ConnectionObj->SendExecution( ReportMsg );
                delete ReportMsg;
            }
        }
        else
        {
            odm_Rpt* CopyReport = new odm_Rpt();
        
            memcpy( CopyReport, Report, sizeof( odm_Rpt ) ); 
            if( ConnectionObj->AddExecution( CopyReport, Src ) == TRUE ) ///< Not duplicate
            {
                if( FRecovering == FALSE )
                {
                    UInt32 Now      = UFC::GetTickCountMS();
                    UInt32 Use      = Now - (UInt32)Report->header.network_id;
                    char   ExecType = OdrUtility::OdmToTMPExectype( Report->header.ExecType,Report->combined_match_type );

                    switch( ExecType )
                    {
                        case '0': ///< New
                        case '4': ///< Canceled
                        case '5': ///< Replaced
                        case '6': ///< New and Filled
                                  FOrderCount++;
                                  FTotalOrderResponseTime += Use;
                                  if( Use > FMaxOrderResponseTime )
                                  {     
                                      FMaxOrderResponseTime  = Use;            
                                      FMaxOrderTimeFCM       = BrokerIndex;
                                      FMaxOrderTimeSessionID = SessionID;
                                  }
                                  UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, AppPrefixIndex," Tick:%u Order Use[%u ms] Max[%u ms] Avg[%d ms]", Now, Use, FMaxOrderResponseTime, FTotalOrderResponseTime/FOrderCount );
                                  break;
                        case 'I': ///< Query
                                  FQueryCount++;
                                  FTotalQueryResponseTime += Use;
                                  if( Use > FMaxQueryResponseTime )
                                  {
                                      FMaxOrderResponseTime  = Use;            
                                      FMaxQueryTimeFCM       = BrokerIndex;
                                      FMaxQueryTimeSessionID = SessionID;
                                  }
                                  UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, AppPrefixIndex," Tick:%u Query Use[%u ms] Max[%u ms] Avg[%d ms]", Now, Use, FMaxQueryResponseTime, FTotalQueryResponseTime/FQueryCount );
                                  break;
                    }                
                }                
            }
            else
            {
                delete CopyReport;
            }
        }
    }
    else
        UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," FCM[%d] SID[%d] Execution not belong to this TMP34.", BrokerIndex, SessionID );
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::OnQuoreRequestConfirm( MTree* Data )
{
    UInt32 BrokerIndex,SessionID;    
    MNode  Commodity_IDNode,order_noNode;   
    UInt32 symbol_type,fcm_id,pseq,ord_id,status_code;
    
    if(Data->get("source_fcm_id",BrokerIndex ) && Data->get( "session_id",SessionID ))
    {
        ConnectionObjectBase* ConnectionObj = GetSession( BrokerIndex, SessionID );
        if( ConnectionObj != NULL )
        {
            if( Data->get("Commodity_ID",Commodity_IDNode ) && Data->get( "order_no",order_noNode ) && 
                Data->get("symbol_type",symbol_type ) && Data->get( "fcm_id",fcm_id ) &&
                Data->get("pseq",pseq ) && Data->get( "ord_id",ord_id ) && Data->get( "status_code",status_code) )
            {
                try
                {
                    TMP::TMPMessage* TMPReply = new TMP::TMPMessage( TMP::tmtR08 );
                    TMP::TMPMessage& R08 = *TMPReply;
                    UFC::AnsiString Commodity_ID((char*)Commodity_IDNode.getData(),Commodity_IDNode.getSize());
                    UFC::AnsiString order_no((char*)order_noNode.getData(),order_noNode.getSize());

                    R08["order_no"]    = order_no.c_str();
                    R08["order_id"]    = ord_id;
                    R08["fcm_id"]      = fcm_id;
                    if( symbol_type == 1 ) ///< Numbex
                        R08["Symbol"]  = TMP::TMPSymbol( pseq, TMP::lsBuy, 0, TMP::lsBuy, TMP::coSingle );
                    else
                        R08["Symbol"]  = Commodity_ID;
                    R08["status_code"] = status_code;
                    ConnectionObj->OnCommand( EVENT_TMP_EXECUTION, (void*)TMPReply );
                    delete TMPReply;
                }
                catch( UFC::Exception& e )
                {
                    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Send Quote response message failed. Reason[%s]", e.what() );
                }
            }
            else
                UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Quote Requese message missing field." );
        }
        else
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Session not found when dispatch Quote Requese message. FCM[%d] Session[%d]",BrokerIndex,SessionID );
    }
    else
        UFC::BufferedLog::DebugPrintf(UFC::dlWarning, AppPrefixIndex," Quote Requese message missing source_fcm_id or session_id." );
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::OnFTRequestResponse( MTree* Data )
{
    Int32  BrokerIndex,SessionID,FCM_REQ_ID,EPOCH,MS;
    MNode  BODYNode, BROKER_ID_NODE,STATUS_CODE_NODE;   
    Int32  TRADE_KIND;
    ConnectionObjectBase* ConnectionObj;

    if( Data->get("FCM_REQ_ID", FCM_REQ_ID ) ) 
    {
        if( Data->get( "SOURCE_ID", BROKER_ID_NODE )   && Data->get( "SESSION_ID", SessionID ) &&
            Data->get( "BODY", BODYNode )              && Data->get( "TRADE_KIND", TRADE_KIND ) &&
            Data->get( "STATUS_CODE",STATUS_CODE_NODE) && Data->get( "TIME_EPOCH_S", EPOCH ) &&
            Data->get( "TIME_MS", MS) )
        {
            try
            {
                TMP::TMPMessage R14( TMP::tmtR14 );
                UFC::AnsiString Broker( (char*)BROKER_ID_NODE.getData(),BROKER_ID_NODE.getSize());
                BrokerIndex = TMP::TMPMessage::GetBrokerID( Broker );
            
                R14[ "status_code"  ] = (UInt8)UFC::AnsiString((char*)STATUS_CODE_NODE.getData(),STATUS_CODE_NODE.getSize()).ToInt();
                R14[ "fcm_req_id"   ] = FCM_REQ_ID;
                R14[ "bulletin_time"] = TMP::TMPTime( EPOCH, MS );
                R14[ "system_type"  ] = TMP::TMPMessage::GetSystemType() + (UInt8)TRADE_KIND;
                R14[ "data"         ] = UFC::AnsiString( (char*)BODYNode.getData(), BODYNode.getSize() );
                UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " fcm_req_id[%d] data[%s]",FCM_REQ_ID, R14[ "data" ].ToString().c_str() );
                if( FCM_REQ_ID == 0 && Broker == "ALL" ) ///< TAIFEX broadcast.
                {
                    for( int i = 0;i< FSessionsArray.ItemCount(); i++ )
                    {
                        if( ( ConnectionObj = FSessionsArray[i] ) != NULL )
                        {
                            if( ConnectionObj->GetConnectionParameter()->RecvBoardcast() == TRUE )
                                ConnectionObj->OnCommand( EVENT_TMP_NOTIFY, (void*)&R14 );
                        }
                    }
                }
                else ///< FCM Request
                {
                    if( (ConnectionObj = GetSession( BrokerIndex, SessionID )) != NULL )
                         ConnectionObj->OnCommand( EVENT_TMP_NOTIFY, (void*)&R14 );
                    else
                        UFC::BufferedLog::DebugPrintf( UFC::dlWarning, " Can't find FCM[%d] Session[%d] to deliver FT Response.",BrokerIndex, SessionID);
                }
            }
            catch( UFC::Exception& e )
            {
                UFC::BufferedLog::DebugPrintf(UFC::dlWarning, AppPrefixIndex," Send FTP response message failed. Reason[%s]", e.what() );
            }
        }
        else
            UFC::BufferedLog::DebugPrintf(UFC::dlWarning, AppPrefixIndex," FT Mex message missing field." );
    }
    else
        UFC::BufferedLog::DebugPrintf(UFC::dlWarning, AppPrefixIndex, " Get FT Response missing FCM_REQ_ID." );
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::OnConnectionNotify( ConnectionObjectBase* CObj, int Reason,void* Data )
{
    ConnectionParameter* cp     = CObj->GetConnectionParameter();    
    UFC::AnsiString      Broker = cp->GetBrokerID();
    int                  SID    = cp->GetPVC();
    int                  FCMID  = cp->GetBrokerIndex();
    TMPRecoverMessages*  RecoverMsg;    
    ExecWriter*          Writer;
            
    switch( Reason ) 
    {
        case CONNECTION_OBJECT_X25ERROR:      UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," FCM:%s SID:%d Can not establish the connection.", Broker.c_str(), SID );
                                              SendUnregFCM( cp );
                                              break;
        case CONNECTION_OBJECT_UNKNOWN_ERROR: UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," FCM:%s SID:%d Unknown error.(Locked)", Broker.c_str(), SID );
                                              SendUnregFCM( cp );
                                              break;  
        case CONNECTION_PVC_LOCKED:           UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," FCM:%s SID:%d Locked.", Broker.c_str(), SID );
                                              SendUnregFCM( cp );
                                              break;        
        case CONNECTION_OBJECT_PVC_BROKEN:    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," FCM:%s SID:%d broken.", Broker.c_str(), SID );
                                              SendUnregFCM( cp );
                                              break;        
        case CONNECTION_OBJECT_NORMAL_CLOSE:  UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," FCM:%s SID:%d Normal close.", Broker.c_str(), SID);
                                              SendUnregFCM( cp );
                                              break;        
        case CONNECTION_OBJECT_PVC_READY:     UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," FCM:%s SID:%d Ready!.", Broker.c_str(), SID );
                                              SendRegFCM( cp );
                                              break;         
        case CONNECTION_TMP_SESSION_SEQ:      if( (Writer = FExecReportListener->GetExecWriter(FCMID, SID )) != NULL )
                                              {   
                                                  FExecReportReader->ReadToEOF(); ///<Force reader read to EOF.
                                                  *((UInt32*)Data) = Writer->GetSessionSeq();
                                              }
                                              break;         
        case CONNECTION_TMP_RECOVER_MSG:      RecoverMsg = (TMPRecoverMessages*)Data;
                                              if( (Writer = FExecReportListener->GetExecWriter(FCMID, SID )) != NULL )                              
                                                  RecoverMsg->BinSize = Writer->RecoverMessages( RecoverMsg->BeginSeq, RecoverMsg->EndSeq, RecoverMsg->Messages, RecoverMsg->CompactFormat );
                                              break;
    }
}
//------------------------------------------------------------------------------
void TMPOrderManager::GetOdrIPPort( void )
{
    char* IPFileName;

    if( (IPFileName = getenv( "ODRIP_FILE" ))!= NULL )
    {
        if( UFC::FileExists( IPFileName ) == TRUE )
        {
            char  Buffer[ 512 ];
            FILE* FFileHandle;

            if( (FFileHandle = fopen( IPFileName, "r" )) != NULL )
            {
                if( fgets( Buffer, 512, FFileHandle ) != NULL )
                {
                    UFC::PStringList Strings;

                    Strings.SetStrings( Buffer, ",:|\n" );
                    if( Strings.ItemCount() == 2 )
                    {
                        FOdrServerIP   = Strings[0];
                        FOdrServerPort = Strings[1].ToInt();
                    }
                }
                fclose( FFileHandle );
            }
        }
    }
}
//------------------------------------------------------------------------------
void TMPOrderManager::ConnectToServer( void )
{
    while( FOdrSocketClient->IsConnect() == FALSE ) ///< Try to connect to server until succeed.
    {
        try
        {
            IsODRSConnected = FALSE;
            GetOdrIPPort();
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Try to connect server[%s] port[%d].", FOdrServerIP.c_str(), FOdrServerPort );
            FOdrSocketClient->Connect( FOdrServerIP, FOdrServerPort, 10 );
            FOdrSocketClient->SetTCPNoDelay( TRUE );
            FOdrSocketClient->SetBlockMode( FALSE );
            FOdrSocketClient->SetBufferSize( SO_RCVBUF, 1048576 ); ///< 1024K Solaris default
            FOdrSocketClient->SetBufferSize( SO_SNDBUF, 1048576 ); ///<   64k AIX default 
            UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Connected to odr_server FD:%d", FOdrSocketClient->GetHandle() );
            FOdrIdleTime = 0;
        }
        catch( UFC::SocketException& se )
        {
            UFC::BufferedLog::DebugPrintf(UFC::dlFatalError, AppPrefixIndex," OdrServer connection failed:%s", se.what());
            sleep( 1 );  ///< Try...after 1 sec.
        }
    }
    FOdrIdleTime ++;
    if( FOdrIdleTime > ORD_HEARTBEAT_INTERVAL && FOdrSocketClient->IsConnect() == TRUE )
    {
        try
        {
            OdrUtility::SendHeartbeat( FOdrSocketClient, FListenPort );    
            FOdrIdleTime = 0;
        }
        catch( UFC::SocketException& se )
        {
            UFC::BufferedLog::DebugPrintf(UFC::dlFatalError, AppPrefixIndex, " Send heartbeat to OdrServer failed:%s", se.what());
        }
    }
    
}
//-----------------------------------------------------------------------------------------------------------------------
// Function to handle connection with odr_server
// This function will be called when odr_server connected.
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::OnConnect( UFC::PClientSocket * Socket )
{
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," OdrServer Connected, Send Login message." );
    OdrUtility::SendLogin( Socket, FListenPort );      
}
//-----------------------------------------------------------------------------------------------------------------------
// Function to handle connection with odr_server
// This function will be called when odr_server disconnected.
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::OnDisconnect( UFC::PClientSocket * )
{
    IsODRSConnected = FALSE;
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," OdrServer connection broken." );
}
//-----------------------------------------------------------------------------------------------------------------------
BOOL TMPOrderManager::OnDataArrived( UFC::PClientSocket * Socket ) 
{
    ODR_MSG_PTR  pOdm   = NULL;
    odm_header*  Header = NULL;

    try
    {
        if( ( pOdm = OdrUtility::CreateFromSocket( Socket )) != NULL )
        {
            Header = (odm_header*)(void*)pOdm;
            switch( Header->msg_tag )
            {
                
                case mtHeartbeat:        OdrUtility::SendHeartbeatConfirm( Socket,FListenPort );
                                         break;
                case mtHeartbeatConfirm: UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Receive a mtHeartbeatConfirm from odr_server.");
                                         break;
                case mtRegConfirm:       HandleLogin( (odm_RegConfirm*)pOdm);                                         
                                         break;
                case mtUnregConfirm:     IsODRSConnected = FALSE;
                                         UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Receive a mtUnregConfirm from odr_server.");
                                         break;
                case mtFCM_RegConfirm:   HandleRegFCM( (odm_FCM_RegConfirm*)pOdm);                                         
                                         break;
                case mtFCM_UnregConfirm: HandleUnregFCM( (odm_FCM_UnregConfirm*)pOdm);
                                         break;	
                case mtRpt:              if( IsODRSConnected == FALSE)   
                                             UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Receive tag[%d] msg before receive mtRegConfirm.");
                                         else if( ExecSrc & esOdrServer )
                                             OnExecution( esOdrServer, (odm_Rpt*)pOdm);
                                         break;
                default:                 UFC::BufferedLog::DebugPrintf(UFC::dlInformation, AppPrefixIndex," Receive unknown message from odr_server tag[%d].", Header->msg_tag );
                                         break;
            }
	    OdrUtility::DestroyMsg( pOdm );
            return TRUE;
        }
        Socket->Purge();
        return TRUE;
    }
    catch( UFC::SocketException & )
    {
        if( pOdm != NULL )
            OdrUtility::DestroyMsg( pOdm );
        UFC::BufferedLog::DebugPrintf(UFC::dlFatalError, AppPrefixIndex," odr_client received data failed, try to reconnect server.");
        return FALSE;
    }    
}
//-----------------------------------------------------------------------------------------------------------------------
BOOL TMPOrderManager::SendToOdrServer( odm_In* Order ) 
{
    if( IsODRSConnected == TRUE )    
    {
        OdrUtility::SerializeToSocket( FOdrSocketClient, Order );    
        FOdrIdleTime = 0; 
        return TRUE;
    }
    return FALSE;
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::GetStates( TMP34States* States )
{
    ///< Order count.
    States->CurrentOrderCount = FOrderCount;
    ///< Max order confirmation time.
    States->MaxOrderResponseTime = FMaxOrderResponseTime;
    ///< Max order confirmation FCM ID.
    States->MaxOrderResponseTimeFCMID = FMaxOrderTimeFCM;
    ///< Max order confirmation Session ID.
    States->MaxOrderResponseTimeSessionID = FMaxOrderTimeSessionID;

    ///< Query count
    States->CurrentQueryCount = FQueryCount;
    ///< Max order query response time.
    States->MaxQueryResponseTime = FMaxQueryResponseTime;
    ///< Max order query FCMID.
    States->MaxQueryResponseTimeFCMID = FMaxQueryTimeFCM;
    ///< Max order query Session ID.
    States->MaxQueryResponseTimeSessionID = FMaxQueryTimeSessionID;
    if( FOrderCount == 0 )
        States->OrderAverageResponseTime = 0;
    else
        States->OrderAverageResponseTime = FTotalOrderResponseTime/FOrderCount;
    if( FQueryCount == 0 )
        States->QueryAverageResponseTime = 0;
    else
        States->QueryAverageResponseTime = FTotalQueryResponseTime/FQueryCount;
    ///< Total session count.
    States->TotalSession = FSessionsArray.ItemCount();
    ///< Connected session count.
    States->SessionNo    = FRegTable.ItemCount();
}
//-----------------------------------------------------------------------------------------------------------------------
void TMPOrderManager::Run()
{
    int FlushCount = 0;
    do 
    {
        ConnectToServer( ); ///< Try to connect to odr_server.
        UFC::SleepMS( 1000 );
        FlushCount ++;
        fflush( stdout );
        UFC::BufferedLog::FlushToFile();
        if( FlushCount >= 10 && FLog != NULL ) 
        {
            FlushCount = 0;
            FLog->Flush();
        }        
            
    }while( TRUE );
    FTransactionObject->Report();
    FLog->Flush();
}
//----------------------------------------------------------------------------------------------------------------------

