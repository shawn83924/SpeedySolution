
//------------------------------------------------------------------------------
#include "TWSESpeedyFIXManager.h"
#include "TWSESpeedyObjectsManager.h"
#include "Session.h"
#include "SessionSettings.h"
#include <iostream>
#include "iniFile.h"
#include "FileStore.h"
#include "FileLog.h"
#include "SessionSettings.h"
#include "TWSESpeedyOrderClient.h"
#include "Settings.h"
#include <stdlib.h>
#include "EnyxTCPInitiator.h"

#ifdef GEN6_FPGA
#include "FPGATCPInitiator.h"
#endif
//------------------------------------------------------------------------------
#ifdef __EXTREME_VERSION   
#include "../SpeedyTWPro/OrderServer.h"
extern SMPServer*             FSMPGateway;
#endif
//------------------------------------------------------------------------------
extern MessageObject*	      MessageObj;       
extern BOOL                   RecoverModeExchange;
extern BOOL                   FIXMultiThread;
extern int                    FIXStoreType;
#ifdef GEN6_FPGA
extern FPGARiskManager* FpgaRM;
#endif
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------
SpeedyFIXManager::SpeedyFIXManager( SpeedyObjectsManager* ObjectManager )
:UFC::PThread( NULL, FALSE )
,FSessionindex( 0 )
,FIsMonitoring( FALSE )
,FObjectManager( ObjectManager)
,FSetingTimeTick( 0 )
{
    if( FObjectManager->IsTWSE() == TRUE )   
        FSettingFile = "../cfg/SpeedyFIXTSE.cfg";
    else
        FSettingFile = "../cfg/SpeedyFIXOTC.cfg";
    FSettings     = new FIX::SessionSettings( FSettingFile.c_str() );      
    if( FIXStoreType == 0 )
        FStoreFactory = new FIX::NullStoreFactory( );          
    else if( FIXStoreType == 1 )
        FStoreFactory = new FIX::MemoryStoreFactory( );                  
    else
        FStoreFactory = new FIX::FileStoreFactory( *FSettings );          
    FLogFactory   = new FIX::UFCFileLogFactory( FObjectManager, *FSettings, RecoverModeExchange );
    
#ifdef GEN6_FPGA
    if (FpgaRM)
    {
        UFC::BufferedLog::Printf(" GEN6 FPGA RM enabling...");
        FInitiator  = new FPGATCPInitiator(FpgaRM, *this, *FStoreFactory, *FSettings, *FLogFactory ); 
        UFC::BufferedLog::Printf(" GEN6 FPGA RM enabled.");
        return;
    }
    else
    {
        UFC::BufferedLog::Printf(" GEN6 FPGA RM is not available. Fall back to regular Socket Connection.");        
    }
#endif
    
    if( FIXMultiThread == TRUE )
    {
        FInitiator  = new FIX::ThreadedSocketInitiator( *this, *FStoreFactory, *FSettings, *FLogFactory );           
        BindCore();
    }
    else
    {     
        FPGATCPContext* tcpCtx = EnyxTCPInitiator::CreateEnyxTCPContext();
        if (!tcpCtx)
        {
             UFC::BufferedLog::Printf("Enyx SmartNIC not available.");                
             FInitiator = new FIX::SocketInitiator( *this, *FStoreFactory, *FSettings, *FLogFactory );    
        }
        else
        {
             UFC::BufferedLog::Printf("Enyx SmartNIC enabled.");                
             EnyxTCPInitiator* enyxInitiator = new EnyxTCPInitiator( *this, *FStoreFactory, *FSettings, *FLogFactory );
             FInitiator = enyxInitiator;
        }                  
    }    
}
//------------------------------------------------------------------------------
void SpeedyFIXManager::BindCore( void )
{
    UFC::AnsiString ConfigFile( "../cfg/BindingCore.cfg" );
            
    if( UFC::FileExists( ConfigFile ) == TRUE )
    {
        UFC::AnsiString CoreString,SectionName;
        UFC::UiniFile   Config( ConfigFile );        
        
        if( FObjectManager->IsTWSE() == TRUE )   
            SectionName = "TWSEFIXRecv";
         else
            SectionName = "OTCFIXRecv";        
        if( Config.GetValue( SectionName, "core", CoreString ) == TRUE )
        {
            UFC::PStringList CoreList;
            
            CoreList.SetStrings( CoreString, ",\n" );
            if( CoreList.ItemCount() > 0 )
            {
                int CMax = UFC::GetCPUs( );
                int Core = CoreList[0].ToInt();
              
                if( Core > 0 && Core <= CMax-1 ) 
                    FInitiator->setBindingCore( Core );            
            }   
        }
    }
    
}
//------------------------------------------------------------------------------
void SpeedyFIXManager::onCreate( const FIX::SessionID& sessionID )
{
    UFC::BufferedLog::Printf( " Created - %s", sessionID.toString().c_str());
}
//------------------------------------------------------------------------------
void SpeedyFIXManager::onLogon( const FIX::SessionID& sessionID )
{
    UFC::AnsiString SIDString( sessionID.toString().c_str() );
    OrderListener*  OrderClient;

    if( FFIXOrderClients.GetObjectByKey( SIDString, OrderClient ) == TRUE )
        OrderClient->OnLogon( );
    else if( ( OrderClient = FindFIXClient( SIDString ) ) != NULL )
        OrderClient->OnLogon( );
    else
    {
        UFC::BufferedLog::Printf( " [onLogon] Client for session[%s] not found!", SIDString.c_str() );        
        UFC::BufferedLog::FlushToFile();
        exit(1);
    }
    UFC::BufferedLog::Printf( " Login - %s", SIDString.c_str());
}
//------------------------------------------------------------------------------
void SpeedyFIXManager::onLogout( const FIX::SessionID& sessionID )
{
    UFC::AnsiString SIDString( sessionID.toString().c_str() );
    OrderListener*  OrderClient;
    
    if( FFIXOrderClients.GetObjectByKey( SIDString, OrderClient ) == TRUE )
        OrderClient->OnLogoff();
    else if( ( OrderClient = FindFIXClient( SIDString ) ) != NULL )
        OrderClient->OnLogoff( );
    else
        UFC::BufferedLog::Printf( " [onLogout] Client for session[%s] not found!", SIDString.c_str() );    
    UFC::BufferedLog::Printf( " Logoff - %s", SIDString.c_str() );    
}
//------------------------------------------------------------------------------
//
// Send NewOrderSingle, OrderCancelRequest, OrderReplaceRequest to TWSE
//
//------------------------------------------------------------------------------
void SpeedyFIXManager::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::DoNotSend )
{    
    if( FIsMonitoring == TRUE )
    {
        UFC::AnsiString SIDString( sessionID.toString().c_str() );
        OrderListener*  OrderClient;
        
        if( FFIXOrderClients.GetObjectByKey( SIDString, OrderClient ) == TRUE )
            OrderClient->onMessage( TRUE, message );    
    }
}
//------------------------------------------------------------------------------
//
// Receive ExecutionReport from TWSE
//
//------------------------------------------------------------------------------
void SpeedyFIXManager::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
    ///< Dispatch to message handling virtual function.
    crack( message , sessionID );
    if( FIsMonitoring == TRUE )
    {
        UFC::AnsiString SIDString( sessionID.toString().c_str() );
        OrderListener*  OrderClient;
        
        if( FFIXOrderClients.GetObjectByKey( SIDString, OrderClient ) == TRUE )
            OrderClient->onMessage( FALSE, message );    
    }    
}
//------------------------------------------------------------------------------
void SpeedyFIXManager::onMessage( const FIX44::ExecutionReport& ExecutionReport, const FIX::SessionID& sessionID) 
{
    UFC::AnsiString SIDString( sessionID.toString().c_str() );
    OrderListener*  OrderClient;

    if( FFIXOrderClients.GetObjectByKey( SIDString, OrderClient ) == TRUE )
        OrderClient->onMessage( ExecutionReport );    
    else
        UFC::BufferedLog::Printf( " [ExecutionReport] Client for session[%s] not found!", SIDString.c_str() );            
}        
//------------------------------------------------------------------------------
void SpeedyFIXManager::onMessage( const FIX44::OrderCancelReject& OrderCancelReject, const FIX::SessionID& sessionID) 
{
    UFC::AnsiString SIDString( sessionID.toString().c_str() );
    OrderListener*  OrderClient;

    if( FFIXOrderClients.GetObjectByKey( SIDString, OrderClient ) == TRUE )
         OrderClient->onMessage( OrderCancelReject );        
    else
        UFC::BufferedLog::Printf( " [OrderCancelReject] Client for session[%s] not found!", SIDString.c_str() );            
}        
//------------------------------------------------------------------------------
void SpeedyFIXManager::onMessage( const FIX44::BusinessMessageReject& BusinessMessageReject, const FIX::SessionID& sessionID ) 
{
    UFC::AnsiString SIDString( sessionID.toString().c_str() );
    OrderListener*  OrderClient;

    if( FFIXOrderClients.GetObjectByKey( SIDString, OrderClient ) == TRUE )
        OrderClient->onMessage( BusinessMessageReject );        
    else
        UFC::BufferedLog::Printf( " [BusinessMessageReject] Client for session[%s] not found!", SIDString.c_str() );            
}        
//------------------------------------------------------------------------------
//
// Send Administrator message to TWSE.
// Add ID/Password 
//
//------------------------------------------------------------------------------
void SpeedyFIXManager::toAdmin( FIX::Message& Msg, const FIX::SessionID& sessionID )
{
    FIX::MsgType msgType;

    Msg.getHeader().getField( msgType );

    if( msgType.getValue() == "A" ) ///< Logon message, Add Password.
    {  
        srand( UFC::GetTickCountMS() );
        const FIX::Dictionary& SessionSetting = FSettings->get( sessionID );        
        int Password = SessionSetting.getLong( FIX::PASSWORD );
        int AppedNo  = (rand( ) % 999) + 1;
        UFC::AnsiString RowDataStr;
       
        UFC::BufferedLog::Printf( " FIX Session[%s] try to login.", sessionID.toString().c_str() );        
        RowDataStr.Printf( "%03d%02d", AppedNo, (AppedNo*Password/100)%100 );        
        Msg.setField( FIX::RawDataLength( 5 ));
        Msg.setField( FIX::RawData( RowDataStr.c_str() ));
        UFC::BufferedLog::Printf( " Password[%04d] RawData[%s] .",Password, RowDataStr.c_str() );        
        //UFC::SleepMS( 2000 );
    }    
    if( FIsMonitoring == TRUE )
    {
        UFC::AnsiString SIDString( sessionID.toString().c_str() );
        OrderListener*  OrderClient;
        
        if( FFIXOrderClients.GetObjectByKey( SIDString, OrderClient ) == TRUE )
            OrderClient->onMessage( TRUE, Msg );    
    }
}
//------------------------------------------------------------------------------
void SpeedyFIXManager::fromAdmin( const FIX::Message& Msg, const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon )
{
    FIX::MsgType msgType;

    Msg.getHeader().getField( msgType );
    if( msgType.getValue() == "0" ) ///< Heartbeat message, check time difference.
    {  
        int                TickNow = UFC::GetTickCountMS();
        FIX::SendingTime   SendingTime;
                
        Msg.getHeader().getField( SendingTime );
        if( TickNow - FSetingTimeTick > 60*1000 ) ///< check 1 time/minute
        {        
            FIX::UtcTimeStamp  UTCTime   = SendingTime.getValue();
            FIX::UtcTimeStamp  LocalTime = SendingTime.getValue();
            FIX::UtcTimeStamp  Now;
            int                Diff = Now - UTCTime;
        
            FSetingTimeTick = TickNow;
            LocalTime += (8*60*60);
            if( Diff >= 1 || Diff <= -1 )
            {
                MTree   Data;     
                int     hhmmss = LocalTime.getHour()*10000+LocalTime.getMinute()*100+UTCTime.getSecond();
                
                Data.append( "Now", hhmmss );
                MessageObj->Send( "SPEEDY.TIME", UFC::Hostname, Data );       
                UFC::BufferedLog::Printf( " Exchange Time[%02d:%02d:%02d.%03d]", LocalTime.getHour(),LocalTime.getMinute(),UTCTime.getSecond(),LocalTime.getMillisecond()  );                 
                UFC::BufferedLog::Printf( " Time Difference [%d]sec", Diff  );                 
                UFC::BufferedLog::Printf( " Ask SpeedyTop to set time[%02d:%02d:%02d]", hhmmss/10000, (hhmmss%10000)/100, hhmmss%100);
            } 
        }
    }
    if( FIsMonitoring == TRUE )
    {
        UFC::AnsiString SIDString( sessionID.toString().c_str() );
        OrderListener*  OrderClient;
        
        if( FFIXOrderClients.GetObjectByKey( SIDString, OrderClient ) == TRUE )
            OrderClient->onMessage( FALSE, Msg );    
    }
}
//------------------------------------------------------------------------------
OrderListener* SpeedyFIXManager::FindFIXClient( const UFC::AnsiString& SIDString )
{
    OrderListener*   OrderClient;    
    UFC::PLockObject lk( FFIXSessionLock );
    
    if( FFIXOrderClientsBackup.GetObjectByKey( SIDString, OrderClient ) == TRUE )
    {    
        FFIXOrderClients.Add( SIDString, OrderClient );   
        UFC::BufferedLog::Printf( " Add Session[%s] to FIX Manager from backup list.", SIDString.c_str() );            
        return OrderClient;
    }
    return NULL;
}
//------------------------------------------------------------------------------
FIX::Session* SpeedyFIXManager::FindFIXSession( const UFC::AnsiString& SenderCompID, const UFC::AnsiString& TargetCompID )
{
    OrderListener*  OrderClient;    
    UFC::AnsiString SIDString;
    
    SIDString.Printf( "FIX.4.4:%s->%s", SenderCompID.c_str(),TargetCompID.c_str());
    if( FFIXOrderClients.GetObjectByKey( SIDString, OrderClient ) == TRUE )           
        return OrderClient->GetFIXSession();    
    return NULL;    
}    
//---------------------------------------------------------------------------------------------------------------------
FIX::Session* SpeedyFIXManager::NextSession( void )
{
    int SC = FFIXSession.ItemCount();
    
    if( SC > 0 )
    {
        FSessionindex++;
        if( FSessionindex >= SC )
            FSessionindex = 0;
        return FFIXSession[ FSessionindex ]->GetFIXSession();
    }
    return NULL;
}
//---------------------------------------------------------------------------------------------------------------------
BOOL SpeedyFIXManager::SendFIXOrder( SMPMessage& SMPMsg )
{
    FIX::Session* FIXClient;        
    
    for( register int i = 0; i < FFIXSession.ItemCount();i++ )
    {
        if( (FIXClient = NextSession()) != NULL )
            if( FIXClient->sendSMP( SMPMsg.GetFIXMessage(), SMPMsg.GetFIXLength(), SMPMsg.GetSMPSequence() ) == TRUE )        
                return TRUE;        
    }
    return FALSE;
}
//------------------------------------------------------------------------------
void SpeedyFIXManager::AddFIXClient( const string& SessionID, OrderListener* OrderClient )
{
    UFC::AnsiString SIDString( SessionID.c_str() );
    
    FFIXOrderClients.Add( SIDString, OrderClient );   
    UFC::BufferedLog::Printf( " Add Session[%s] to FIX Manager.", SIDString.c_str() );            
    
    UFC::PLockObject lk( FFIXSessionLock );
    
    FFIXOrderClientsBackup.Add( SIDString, OrderClient );       
    if( FFIXSession.IndexOf( OrderClient ) == -1 )
        FFIXSession.Add( OrderClient );
}
//------------------------------------------------------------------------------
void SpeedyFIXManager::UpdateFIXStatus( void )
{
    OrderListener*  OrderClient = FFIXOrderClients.First();
    
    while( OrderClient != NULL )
    {
        if( FObjectManager != NULL )
            FObjectManager->ReportPVCState( OrderClient->GetOwner(), OrderClient->GetStatus() );
        OrderClient = FFIXOrderClients.Next();
        UFC::SleepMS( 5 );
    };
}
//------------------------------------------------------------------------------
void SpeedyFIXManager::Execute()
{
    try
    {
        UFC::BufferedLog::Printf( " Start FIX Initiators..." );
        FInitiator->start( );
        UFC::BufferedLog::Printf( " FIX Engine running" );
        while( TRUE )
        {
            sleep( 5 );  ///< Sleep 1 sec
            UpdateFIXStatus();
            ///< Dequeue from ordr queue.            
        }
        FInitiator->stop( );
    }
    catch( FIX::Exception& e )
    {
        UFC::BufferedLog::Printf( " SpeedyFIXManager FIX Exception:%s", e.what() );
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::Printf( " UFC Exception:%s", e.what() );
    }
}
//------------------------------------------------------------------------------
void SpeedyFIXManager::Run( void )
{
    UFC::BufferedLog::Printf( " SpeedyFIXManager running." );        
    Start( );
}
//------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
