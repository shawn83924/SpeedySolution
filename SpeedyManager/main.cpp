/* 
 * File:   main.cpp
 * Author: yuan
 *
 * Created on 2015年2月16日, 下午 5:17
 */
#include "SpeedyManager.h"
//----------------------------------------------------------------------------------------------------------------------
MessageObject* MessageObj;
SpeedyManager* Manager;
//----------------------------------------------------------------------------------------------------------------------    
//  0:Ver:2.0.4 Build Date:Sep 28 2009
//  1:License expired:20091010
//  2:Startup:2009/09/29 08:35:40
//  3:Start with clear flag: Yes
//----------------------------------------------------------------------------------------------------------------------
class CheckSystemListener : public MessageListener
{
public:
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
    {
        MTree           ReplyData;
        UFC::AnsiString Value,ReplyKey;
        UFC::UDateTime  Now;

        if( Data->get("ReplyKey",ReplyKey) == TRUE  )
        {
            Now.setCurrent();
            ReplyData.append( "Name", "Speedy Manager" );
            ReplyData.append( "COUNT", 2 );
            Value.Printf( "inf,%s", Manager->VersionBuidDate.c_str() );
            ReplyData.append( "0", Value );
            if( Now.getYear()  != Manager->StartupTime.getYear() ||
                Now.getMonth() != Manager->StartupTime.getMonth() ||
                Now.getDate()  != Manager->StartupTime.getDate() )
                Value.Printf( "war,Startup:%04d/%02d/%02d %02d:%02d:%02d", Manager->StartupTime.getYear(), Manager->StartupTime.getMonth(),  Manager->StartupTime.getDate(),
                                                                           Manager->StartupTime.getHour(), Manager->StartupTime.getMinute(), Manager->StartupTime.getSecond() );
            else
                Value.Printf( "inf,Startup:%04d/%02d/%02d %02d:%02d:%02d", Manager->StartupTime.getYear(), Manager->StartupTime.getMonth(),  Manager->StartupTime.getDate(),
                                                                           Manager->StartupTime.getHour(), Manager->StartupTime.getMinute(), Manager->StartupTime.getSecond() );
            ReplyData.append( "1", Value );
            UFC::SleepMS( 5 );
            MessageObj->Send( "CHECK.REPLY", ReplyKey, ReplyData, FALSE );
        }
    }      
};
//----------------------------------------------------------------------------------------------------------------------
class ProcessMonitor : public MonitorListener
{
public:
    virtual void OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}    
    virtual void OnProcessList( UFC::PStringList& Processs ){}
    virtual void OnConnected( void ){}
    virtual void OnDisconnected( void ){}
    virtual void OnProcessConnected( BOOL IsTheFirstOne )
    {
        if( IsTheFirstOne == FALSE )
        {
            UFC::BufferedLog::Printf( " ***** Process SpeedyManager already exists *****"  );
            sleep( 2 );
            exit( 0 );
        }
    }
};
//----------------------------------------------------------------------------------------------------------------------    
//  Main function
//---------------------------------------------------------------------------------------------------------------------- 
int main(int argc, char** argv) 
{
    UFC::BufferedLog Log( "SpeedyManager Log","log","SpeedyManager","log", 102400, TRUE );
    
    UFC::BufferedLog::SetLogObject( &Log );    
    Manager    = new SpeedyManager( );
    MessageObj = new MessageObject( "SpeedyManager", "1.0", "XXXXXX" );    
    MessageObj->AddListener( "ADMIN", "all", Manager );
    MessageObj->AddListener( "SYSTEM.CHECK", UFC::Hostname, new CheckSystemListener() );
    MessageObj->SetMonitorListener( new ProcessMonitor() );    
    MessageObj->Start(); ///< Start Migo Message pump.    
    MessageObj->WaitForConnected();    
    while( TRUE )
    {
        sleep( 5 );
        UFC::BufferedLog::FlushToFile();///< Force write the screen log to file.
    }
    return 1;
}
//----------------------------------------------------------------------------------------------------------------------

