// 
// File:   main.cc
// Author: yuan
//
// Created on November 13, 2007, 4:50 PM
//
//------------------------------------------------------------------------------
#include "UFC.h"
#include "Sigo.h"
#include "iniFile.h"
//------------------------------------------------------------------------------
UFC::AnsiString   HOST_NAME;
UFC::AnsiString   HOST_IP;
UFC::AnsiString   MARGIN_FILE_NAME;
UFC::AnsiString   DEPOSIT_FILE_NAME;
UFC::AnsiString   CONFIG_FILE_NAME;
UFC::AnsiString   LOG_PATH = "../log/DBRiskinfo";
UFC::PHashedSet<UFC::AnsiString> FBackupServer;
const char*       PROGRAM_NAME = {"FubonDBRiskinfo"};
const int         SPEEDY_SET_CONFIG_FILE    = 26;
//------------------------------------------------------------------------------
MessageObject*     MessageObj         = NULL;
UFC::FileStreamEx* AccountMarginFile  = NULL;
//------------------------------------------------------------------------------
UFC::PEvent       ProcessExistEvent;
//------------------------------------------------------------------------------
class ProcessMonitor : public MonitorListener
{
public:
    virtual void OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessConnected( BOOL IsTheFirstOne );
    virtual void OnProcessList( UFC::PStringList& Processs ){}
    virtual void OnConnected( void ){}
    virtual void OnDisconnected( void ){}
};
//------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        UFC::BufferedLog::Printf( " ***** Process %s already exists *****", PROGRAM_NAME );
        sleep( 2 );
        exit( 0 );
    }    
    ProcessExistEvent.SetEvent( );
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char * argv[] )
{
    CONFIG_FILE_NAME.Printf( "../cfg/%s.cfg",PROGRAM_NAME );
    for( int i = 1; i < argc; i++ )
    {
        UFC::AnsiString Param( argv[i] );
        if( Param == "-H" || Param == "-h" || Param == "-?" ) ///< Help screen
        {
            printf( "%s Copyright (c) 2003,2008 MDBS\n",PROGRAM_NAME);
            printf( "Usage: %s [options]\n",PROGRAM_NAME );
            printf( "   -H         Show this page.\n" );
            printf( "   -C         Config file name.(Default:../cfg/%s.cfg)\n",PROGRAM_NAME );            
            exit( 1 );
        }	
        else if( Param.AnsiPos("-C") != -1 || Param.AnsiPos("-c") != -1 )
        {
             UFC::AnsiString PName( (const char*)(Param.c_str() + 2) );
             CONFIG_FILE_NAME = PName;
        }
    }
}
//------------------------------------------------------------------------------
void LoadSetting( void )
{
    try
    {
        UFC::AnsiString Value;
        UFC::UiniFile   Config( CONFIG_FILE_NAME );       
        
        if( Config.GetValue( "Setting", "IP",HOST_IP ) == FALSE )            
            throw UFC::Exception( "Setting \"IP\" not found." );
        if( Config.GetValue( "Setting", "Hostname",HOST_NAME ) == FALSE )            
            throw UFC::Exception( "Setting \"Hostname\" not found." );
        if( Config.GetValue( "Setting", "MarginFile",MARGIN_FILE_NAME ) == FALSE )            
            throw UFC::Exception( "Setting \"MarginFile\" not found." );        
        if( Config.GetValue( "Setting", "DepositFile", DEPOSIT_FILE_NAME ) == FALSE )            
            throw UFC::Exception( "Setting \"DepositFile\" not found." );                
        Config.GetValue( "Setting", "LogName",LOG_PATH );
        
        UFC::BufferedLog::Printf( " [Setting]" );        
        UFC::BufferedLog::Printf( " IP          = %s", HOST_IP.c_str() );        
        UFC::BufferedLog::Printf( " Hostname    = %s", HOST_NAME.c_str() );
        UFC::BufferedLog::Printf( " MarginFile  = %s", MARGIN_FILE_NAME.c_str() );        
        UFC::BufferedLog::Printf( " DepositFile = %s", DEPOSIT_FILE_NAME.c_str() );        
        UFC::BufferedLog::Printf( " LogName     = %s", LOG_PATH.c_str() );                
        
        ///< Load backup server hostname
        UFC::Section* pSection = Config.GetSection( "Backup" );
        if( pSection != NULL )
        {
            int Count = pSection->ItemCount();
            UFC::AnsiString Name, Value;
            
            for( register int i = 1; i <= Count; i++ )
            {
                Name.Printf( "Backup%d", i );
                if( pSection->GetValue( Name, Value ) == TRUE )
                {
                    FBackupServer.Add( Value );
                    UFC::BufferedLog::Printf( " Backup Server %d:[%s]", i, Value.c_str() );
                }
            }
        }
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::Printf( " Exception:%s", e.what() );        
        exit( 0 );
    }
}
//------------------------------------------------------------------------------
void UploadDepositTable( void )
{
    try
    {   
        char Buffer[ 128 ];
        memset( Buffer, 0x20, 128 );
        ///<Open file 
        UFC::FileStreamEx DepositFile( DEPOSIT_FILE_NAME, "r+" );
        ///< Delete old ini file
        if( UFC::FileExists( "./CommodityDeposit.ini" ) )
            UFC::DeleteFile( "./CommodityDeposit.ini" );        
        UFC::UiniFile* CommodityDepositTable = new UFC::UiniFile( "./CommodityDeposit.ini", TRUE );
        while( DepositFile.Read( Buffer, 21 ) == 21 )
        {
            
            UFC::AnsiString Commodity( Buffer, 10 );
            UFC::AnsiString Deposit( Buffer+10, 10);                
                
            Commodity.TrimRight();                
            CommodityDepositTable->SetValue( "TAIFEX", Commodity, Deposit );                
        }                
        CommodityDepositTable->Save();
        
        UFC::FileStream iniFile( "./CommodityDeposit.ini", O_RDONLY );
        Int32           DataSize = iniFile.GetSize();        
            
        if( DataSize > 0 )
        {
            MTree           ReplyData;
            unsigned char*  buffer = new unsigned char[ DataSize ];
            int             ReadSize = iniFile.Read( buffer, DataSize );
            
            ReplyData.append( "COMMAND",SPEEDY_SET_CONFIG_FILE );
            ReplyData.append( "HOST", HOST_NAME );
            ReplyData.append( "FILENAME", "CommodityDeposit.ini" );                        
            ReplyData.append( "FILE", buffer, ReadSize, TRUE );   
            MessageObj->Send( "SPEEDY.AGENT", HOST_NAME, ReplyData, FALSE );
            
            for( register int i = 0; i < FBackupServer.ItemCount(); i++ )
            {
                MTree Data;
                UFC::AnsiString BackupServerHost( FBackupServer.GetItem( i ) ); 
                Data.append( "COMMAND",SPEEDY_SET_CONFIG_FILE );
                Data.append( "HOST", BackupServerHost );
                Data.append( "FILENAME", "CommodityDeposit.ini" );                        
                Data.append( "FILE", buffer, ReadSize, TRUE );   
                MessageObj->Send( "SPEEDY.AGENT", BackupServerHost, Data, FALSE );
            }
            
            delete [] buffer;
        }                               
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Can't open products deposit setting file[%s]",DEPOSIT_FILE_NAME.c_str() );
        exit(0);
    }    
}
//------------------------------------------------------------------------------
void OpenAccountMarginFile( void )
{
    try
    {   
        AccountMarginFile = new UFC::FileStreamEx( MARGIN_FILE_NAME, "r+" );
        AccountMarginFile->Seek( 0, UFC::soFromEnd ); 
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Can't open accounts margin setting file[%s]", MARGIN_FILE_NAME.c_str() );
        exit(0);
    }        
}
//------------------------------------------------------------------------------
void CheckAccountMargin( void )
{
    char Buffer[30];
    memset( Buffer, 0x20, 30 );
    
    while( AccountMarginFile->Read( Buffer, 29 ) == 29 )
    {
        Buffer[29] = '\0';
        UFC::BufferedLog::Printf( " Read[%s]", Buffer );
        
        UFC::AnsiString BrokerID( Buffer, 7 );                
        UFC::AnsiString Account( Buffer + 7, 7 );
        UFC::AnsiString Margin( Buffer + 14, 14 );
        MTree           ReplyData;
        
        ReplyData.append( "BrokerID", BrokerID );
        ReplyData.append( "Account",  Account );                        
        ReplyData.append( "Margins",   Margin.ToDouble() / 100.0 );   
        MessageObj->Send( "TAIFEX.RISK", HOST_NAME, ReplyData, FALSE );                        
        
    }                
}
//------------------------------------------------------------------------------
int main(int argc, char** argv) 
{
    UFC::AnsiString LogFile;    
    
    CheckArgs( argc, argv );     ///< Parse command line arguments.
    LoadSetting();
    LogFile.Printf("%s.%s.%s.log",LOG_PATH.c_str(), HOST_NAME.c_str(), UFC::GetDateString().c_str() );    
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   Riskinfo Publish Module,%s",PROGRAM_NAME );
    UFC::BufferedLog::Printf( "   Copyright 2004-2009 by MDBS Software Inc.");
    UFC::BufferedLog::Printf( "   All right reserved.                      ");
    UFC::BufferedLog::Printf( "   Startup at: %s.", UFC::Hostname );
    UFC::BufferedLog::Printf( "   Build Date:%s ", __DATE__ );		
    UFC::BufferedLog::Printf( "    " );		
    UFC::BufferedLog::Printf( "   Connect to server:[%s]", HOST_IP.c_str() );
    UFC::BufferedLog::Printf( "   Server Hostname:[%s]", HOST_NAME.c_str() );
    UFC::BufferedLog::Printf( "   Log:[%s]", LogFile.c_str() );
    UFC::BufferedLog::Printf( "__________________________________________" );
        
    
    UFC::BufferedLog Log( LogFile, 1024, TRUE );
    MessageObj = new MessageObject( "DBRiskinfo", "1.0", "XXXXX" );    
    MessageObj->SetMonitorListener( new ProcessMonitor() );    
    MessageObj->SetHost( HOST_IP );
    MessageObj->Start(); ///< Start Migo Message pump.    
    ProcessExistEvent.WaitFor( 30 );    
    UFC::BufferedLog::SetLogObject( &Log );    
    UploadDepositTable();    
    OpenAccountMarginFile();
    
    while( TRUE )
    {
        CheckAccountMargin();
        UFC::SleepMS( 100 );        
    }
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
