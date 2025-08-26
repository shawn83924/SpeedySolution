
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
//------------------------------------------------------------------------------
extern MessageObject*	      MessageObj;       
extern BOOL                   RecoverModeExchange;
extern BOOL                   FIXMultiThread;
extern BOOL                   FIXFileStore;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------
SpeedyFIXManager::SpeedyFIXManager( SpeedyObjectsManager* ObjectManager )
:UFC::PThread( NULL, FALSE )
,FIsMonitoring( FALSE )
,FObjectManager( ObjectManager)
,FSetingTimeTick( 0 )
{
    if( FObjectManager->IsTWSE() == TRUE )   
        FSettingFile = "../cfg/SpeedyFIXTSE.cfg";
    else
        FSettingFile = "../cfg/SpeedyFIXOTC.cfg";
    FSettings     = new FIX::SessionSettings( FSettingFile.c_str() );      
    if( FIXFileStore == TRUE )
        FStoreFactory = new FIX::FileStoreFactory( *FSettings );          
    else
        FStoreFactory = new FIX::MemoryStoreFactory( );          
    FLogFactory   = new FIX::UFCFileLogFactory( FObjectManager, *FSettings, RecoverModeExchange );
    if( FIXMultiThread == TRUE )
        FInitiator    = new FIX::ThreadedSocketInitiator( *this, *FStoreFactory, *FSettings, *FLogFactory );           
    else    
        FInitiator    = new FIX::SocketInitiator( *this, *FStoreFactory, *FSettings, *FLogFactory );           
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
        UFC::BufferedLog::Printf( " [onLogon] Client for session[%s] not found!", SIDString.c_str() );        
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
void SpeedyFIXManager::AddFIXClient( const string& SessionID, OrderListener* OrderClient )
{
    UFC::AnsiString SIDString( SessionID.c_str() );
    
    FFIXOrderClients.Add( SIDString, OrderClient );   
    UFC::BufferedLog::Printf( " Add Session[%s] to FIX Manager.", SIDString.c_str() );            
    
    UFC::PLockObject lk( FFIXSessionLock );
    
    FFIXOrderClientsBackup.Add( SIDString, OrderClient );       
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
        UFC::BufferedLog::Printf( " FIX Exception:%s", e.what() );
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
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
