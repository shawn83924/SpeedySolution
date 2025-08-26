#include "FubonFTConnection.h"
#include "../../UFC/NetUtility.h"
#include "../../UFC/FileStream.h"
#include "../../UFC/iniFile.h"
#include "../../Migo/Sigo.h"
//------------------------------------------------------------------------------
extern UFC::AnsiString FAppendName;
//------------------------------------------------------------------------------
UFC::PEvent  ProcessExistEvent;
//----------------------------------------------------------------------------------------------------------------------
class ProcessMonitor : public MonitorListener
{
public:
    virtual void OnProcessStartup( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessStopped( const UFC::AnsiString& Host, const UFC::AnsiString& AppName ){}
    virtual void OnProcessConnected( BOOL IsTheFirstOne );
    virtual void OnProcessList( UFC::PStringList& Processs ){}
    virtual void OnConnected( void ){ UFC::BufferedLog::Printf( " *** MBus Connected ***"  ); }
    virtual void OnDisconnected( void ){}
};
//----------------------------------------------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        UFC::BufferedLog::Printf( " ***** This process already exists *****"  );
        sleep( 1 );
        //exit( 0 );
    }
    UFC::BufferedLog::Printf( " *** Process logon ***"  );
    ProcessExistEvent.SetEvent( );
}
//------------------------------------------------------------------------------
MessageObject*  MessageObj    = NULL;
//------------------------------------------------------------------------------
TFubonOrderConnection::TFubonOrderConnection( const UFC::AnsiString& ConfigFileName )
:UFC::PThread( NULL, FALSE )
,FDebug( "" )
,FLogFile( "" )
,FLockFile( "" )
,FIP( "127.0.0.1" )
,FHostname( UFC::Hostname )
,FPort( 12345 )
,FReadBufferSize( 0 )
,FReqQueue( NULL )
,FReadBuffer( NULL )
{   
    UFC::AnsiString FFTCName;
    
    ReadConfigFile( ConfigFileName );
    LockFile();    
    FFTCName.Printf( "FFTC_%s@%s",FAppendName.c_str(), UFC::Hostname );    
    FReadBuffer = new char[ FReadBufferSize ];
    FReqQueue   = new TSysque( FQueIP, FQuePort, FReqQueName );        
    MessageObj  = new MessageObject( FFTCName, "1.0", "FCM system.",FPort );   ///< Create Migo Message object.                    
    MessageObj->SetHost( FIP );
    OpenQueues( ConfigFileName );                 
    MessageObj->SetMonitorListener( new ProcessMonitor() );        
    MessageObj->Start();     
    ProcessExistEvent.WaitFor( 30 );
    if( FReqQueue->Open() == FALSE )
    {
        UFC::BufferedLog::Printf( " Open sys queue [%s] failed.", FReqQueName.c_str());
        exit(0);            
    }
    Start();         
}
//------------------------------------------------------------------------------
TFubonOrderConnection::~TFubonOrderConnection( void )
{   
    if( FReqQueue != NULL )
        delete FReqQueue;    
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::SetLogObject( void )
{
    UFC::AnsiString LogFileName;
    LogFileName.Printf( "%s%sFubonFTConnection%s.log", FLogFile.c_str(), UFC::Hostname, UFC::GetDateString().c_str() );

    UFC::BufferedLog::DebugPrintf( " Write log to file:[%s]", LogFileName.c_str() );    
    UFC::BufferedLog::SetDebugMode( (FDebug == "1") );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogFileName, 1024, TRUE ) );
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OpenQueues( const UFC::AnsiString& ConfigFileName )
{
    try
    {
        UFC::UiniFile iniFile( ConfigFileName );
        int Count = 0;
        
        UFC::BufferedLog::Printf( " [ brokers ]" );
        while( TRUE )
        {
            UFC::AnsiString ID( Count );
            UFC::AnsiString Broker;
            if( iniFile.GetValue( "brokers", ID, Broker ) == TRUE )
            {
                UFC::BufferedLog::Printf( " %03d        = [%s]", Count, Broker.c_str() );
                TSysque* NewQueue; 
                UFC::AnsiString QueueName = Broker + "_FTrv";
                NewQueue = new TSysque( FQueIP, FQuePort, QueueName );                            
                if( NewQueue->Open() == FALSE )
                    exit(0);
                FBrokerTable.Add( Broker, NewQueue );
                MessageObj->AddListener( "FT.RESPONSE.TSE", Broker, this );        ///< Add State query Message listener to MessageObject.                
                MessageObj->AddListener( "FT.RESPONSE.OTC", Broker, this );        ///< Add State query Message listener to MessageObject.                
                Count++;
            }
            else
                break;
        }    
        MessageObj->AddListener( "FT.RESPONSE.TSE", "0000", this );        ///< Add State query Message listener to MessageObject.                
        MessageObj->AddListener( "FT.RESPONSE.OTC", "0000", this );        ///< Add State query Message listener to MessageObject.                
    }
    catch( UFC::Exception& ex )
    {
        UFC::BufferedLog::DebugPrintf( " %s", ex.what() );
    }
}    
//------------------------------------------------------------------------------
BOOL TFubonOrderConnection::SaveFile( MTree* Data, const UFC::AnsiString& FileCode, UFC::AnsiString& FileName )
{
    FileName.Printf( "%s%s.%u.%u", FInbox.c_str(), FileCode.c_str(), getpid(), UFC::GetTickCountMS() );
    Int8* Buffer;
    Int32  Size;
    
    if( Data->get( "Data", Buffer, Size ) == TRUE )
    {
       try
       {
           UFC::FileStream Tmpfile( FileName, O_CREAT|O_TRUNC|O_WRONLY );
           Tmpfile.Write( Buffer, Size );
           Tmpfile.Flush();
           UFC::BufferedLog::Printf( " Save file[%s] Size[%d] OK.",FileName.c_str(),Tmpfile.GetSize() );
           return true;
       }
       catch( ... )
       {
           return FALSE;
       }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::ReadConfigFile( const UFC::AnsiString& ConfigFileName )
{
    try
    {
        UFC::UiniFile iniFile( ConfigFileName );
        UFC::AnsiString Value,BCCode;

        iniFile.GetValue( "main", "debug", FDebug );
        iniFile.GetValue( "main", "log_path", FLogFile );
        iniFile.GetValue( "main", "lock_path", FLockFile );
        iniFile.GetValue( "main", "inbox_path", FInbox );
        iniFile.GetValue( "main", "Broadcast", BCCode );
        
        if( BCCode.Length() > 0 )
        {
            UFC::PStringList Strings;
        
            Strings.SetStrings( BCCode, ",\n" );
            if( Strings.ItemCount() > 0 )
            {
                for( int i = 0; i < Strings.ItemCount(); i++ ) 
                     FBroadcastSet.Add( Strings[i] );
            }
        }
        
        iniFile.GetValue( "MBus", "ip", FIP );
        iniFile.GetValue( "MBus", "port", Value );
        iniFile.GetValue( "MBus", "Hostname", FHostname );
        FPort = Value.ToInt();
        
        Value = "0";                
        iniFile.GetValue( "sysque", "name", FReqQueName );
        iniFile.GetValue( "sysque", "ip", FQueIP );
        iniFile.GetValue( "sysque", "port", Value );
        FQuePort = Value.ToInt();
        
        Value = "0";		        
        iniFile.GetValue( "sysque", "read_buffer_size", Value );
        FReadBufferSize = Value.ToInt();
        

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
        if( FInbox.Length() <= 0 )
        {
            FInbox = "./";
            iniFile.SetValue( "main", "inbox_path", FInbox );
        }                
        if( FIP.Length() <= 0 )
        {
            FIP = "127.0.0.1";
            iniFile.SetValue( "MBus", "ip", FIP );
        }
        if( FPort <= 0 )
        {
            Value = "23456";
            FPort = Value.ToInt();
            iniFile.SetValue( "MBus", "port", Value );
        }        
        if( FReqQueName.Length() <= 0 )
        {
            FReqQueName = "9600";
            iniFile.SetValue( "sysque", "name", FIP );
        }
        if( FReadBufferSize <= 0 )
        {
            Value = "512";
            FReadBufferSize = Value.ToInt();
            iniFile.SetValue( "sysque", "read_buffer_size", Value );
        }   

        iniFile.Save();
        
        UFC::BufferedLog::Printf( " [ main ]" );
        UFC::BufferedLog::Printf( " debug              = [%s]", FDebug.c_str() );
        UFC::BufferedLog::Printf( " log_path           = [%s]", FLogFile.c_str() );
        UFC::BufferedLog::Printf( " lock_path          = [%s]", FLockFile.c_str() );
        UFC::BufferedLog::Printf( " inbox_path         = [%s]", FInbox.c_str() );
        UFC::BufferedLog::Printf( " " );        
        UFC::BufferedLog::Printf( " [speedy]" );
        UFC::BufferedLog::Printf( " ip                 = [%s]", FIP.c_str() );
        UFC::BufferedLog::Printf( " Hostname           = [%s]", FHostname.c_str() );
        UFC::BufferedLog::Printf( " port               = [%d]", FPort );        
        
        UFC::BufferedLog::Printf( " " );
        UFC::BufferedLog::Printf( " [sysque]" );        
        UFC::BufferedLog::Printf( " name               = [%s]", FReqQueName.c_str() );        
        UFC::BufferedLog::Printf( " read_buffer_size   = [%d]", FReadBufferSize );
        UFC::BufferedLog::Printf( " " );
        SetLogObject();        
    }
    catch( UFC::Exception& ex )
    {
        UFC::BufferedLog::DebugPrintf( " %s", ex.what() );
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::LockFile( void )
{
    UFC::AnsiString LockFileName;
    LockFileName.Printf( "%s%sfepFT_%s_lock", FLockFile.c_str(), UFC::Hostname, FAppendName.c_str() );
    UFC::BufferedLog::DebugPrintf( " Try to open and lock file:[%s].", LockFileName.c_str() );
    
    int fd = open( LockFileName.c_str(), O_WRONLY | O_CREAT, 0600 );
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
void TFubonOrderConnection::ToSysQueue(  const UFC::AnsiString& BrokerID,  TSQMessage& ReplyMessage )
{
    TSysque*        ReplyQueue; 
    UFC::AnsiString Value = ReplyMessage.ToString();
    
    if( BrokerID == "0000" )///< To all brokers.
    {
        ReplyQueue = FBrokerTable.First();
        while( ReplyQueue != NULL )
        {
            ReplyQueue->Write( (char*)(Value.c_str()), Value.Length() );        
            ReplyQueue = FBrokerTable.Next();
        };
    }
    else
    {
        if( (ReplyQueue = FBrokerTable.GetObjectByKey( BrokerID )) != NULL )
             ReplyQueue->Write( (char*)(Value.c_str()), Value.Length() );        
    }    
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{
    UFC::AnsiString BrokerID, RequestID, FileCode,TimeNow;
    Int32           Status, IsFile;
    TSQMessage      ReplyMessage;
    UFC::UDateTime  Today;
    
    if( Data->get( "Sender", BrokerID ) &&
        Data->get( "FileCode", FileCode ) &&        
        Data->get( "IsFile", IsFile ) && 
        Data->get( "Status", Status ) )
    {
        GetHHMMSS( TimeNow );        
        
        ReplyMessage.Append( "00", "0" );    
        ReplyMessage.Append( "08", Key.SubString( 0,2 ) );
        ReplyMessage.Append( "09", Key.SubString( 2,2 ) );
    
        if( Subject == "FT.RESPONSE.TSE" )
            ReplyMessage.Append( "13", "T" );
        else
            ReplyMessage.Append( "13", "O" );
        Today.setCurrent();    
        ReplyMessage.Append( "10", (long)Today.getDate() ); ///<Day*/    
        ReplyMessage.Append( "L9", TimeNow );
        ReplyMessage.Append( "05", FileCode );
        if( Data->get( "ReqID", RequestID ) )        
            ReplyMessage.Append( "65", RequestID );       
        ReplyMessage.Append( "40", (long)Status );                               
        
        UFC::BufferedLog::Printf(" ------------------ Reply to SysQueue --------------" );
        UFC::BufferedLog::Printf(" Sender[%s]", BrokerID.c_str());
        UFC::BufferedLog::Printf(" FileCode[%s]", FileCode.c_str());
        UFC::BufferedLog::Printf(" RequestID[%s]", RequestID.c_str());        
        UFC::BufferedLog::Printf(" Status[%s]", Status);                
        if( IsFile == TRUE )
        {
            UFC::AnsiString DLFile;
            if( SaveFile( Data, FileCode, DLFile ) == FALSE )
            {
                UFC::BufferedLog::Printf(" Save downloaded file [%s]failed.",DLFile.c_str() );
                return;
            }
            else
                ReplyMessage.Append( "N2", DLFile );      
            UFC::BufferedLog::Printf(" FileName[%s]", DLFile.c_str() );
        }
        else
        {
            char* MsgData;
            Int32 Size;
            if( Data->get( "Data", MsgData, Size ) )
            {
                ReplyMessage.Append( "HQ", UFC::AnsiString( MsgData, Size ) );
        	UFC::BufferedLog::Printf(" Message[%s] size[%d].", MsgData, Size );
            }
            else
                UFC::BufferedLog::Printf(" Without Data field." );
        }
        UFC::BufferedLog::Printf(" ---------------------------------------------------" );                
        if( FBroadcastSet.Exists( FileCode ) == TRUE )
            ToSysQueue( "0000", ReplyMessage );
        else
            ToSysQueue( Key, ReplyMessage );
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::Execute( void )
{
  
    while( !IsTerminated() )
    {
        int ReadSize = FReqQueue->Read( FReadBuffer, FReadBufferSize );
        if( ReadSize > 0 )
        {
            FReadBuffer[ReadSize] = 0x00;
            OnData();
        }
        else
            UFC::SleepMS( 1 );        
    }
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnData( void )
{
    TSQMessage      SQMessage;
    UFC::AnsiString MessageString( FReadBuffer );
    UFC::AnsiString FileName;
    
    SQMessage.FromString( MessageString );
    if( SQMessage.Get( "N2", FileName ) == TRUE  ) 
        FileUpload( SQMessage );
    else
        FileRequest( SQMessage );    
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::FileRequest( TSQMessage& SQMessage )
{
    UFC::AnsiString CompanyID, BranchID, Market, RequestID, FileCode, ReqData, Force;
    
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "13", Market ) &&
        SQMessage.Get( "65", RequestID ) &&
        SQMessage.Get( "05", FileCode ) &&
        SQMessage.Get( "HQ", ReqData ) &&
        SQMessage.Get( "M1", Force )) 
    {
        UFC::AnsiString BrokerID;
        MTree Data;
                
        BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Data.append( "Sender", BrokerID );        
        Data.append( "FileCode", FileCode );                
        Data.append( "ReqID", RequestID );                 
        Data.append( "IsFile", 0 ); 
        if( ReqData.Length() > 3 )
            Data.append( "Data", ReqData.c_str() + 3 );                        
        
        UFC::BufferedLog::Printf(" ----------- Send to Speedy File Request -----------" );
        UFC::BufferedLog::Printf(" Market[%s]", Market.c_str());
        UFC::BufferedLog::Printf(" Sender[%s]", BrokerID.c_str());
        UFC::BufferedLog::Printf(" FileCode[%s]", FileCode.c_str());
        UFC::BufferedLog::Printf(" RequestID[%s]", RequestID.c_str());        
        UFC::BufferedLog::Printf(" ---------------------------------------------------" );        
        if( Market == "T" || Market == "t" )
            MessageObj->Send( "FT.REQUEST.TSE", FHostname, Data, FALSE );
        else
            MessageObj->Send( "FT.REQUEST.OTC", FHostname, Data, FALSE );
    }
    else
    	UFC::BufferedLog::Printf(" Miss tag. 08,09,13,65,05,HQ or M1");
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::FileUpload( TSQMessage& SQMessage )
{
    UFC::AnsiString CompanyID, BranchID, Market, RequestID, FileCode, FileName;
    
    if( SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "13", Market ) &&
        SQMessage.Get( "65", RequestID ) &&
        SQMessage.Get( "05", FileCode ) &&
        SQMessage.Get( "N2", FileName ) )
    {
        UFC::AnsiString BrokerID;
        MTree Data;
        
        BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Data.append( "Sender", BrokerID );
        Data.append( "FileCode", FileCode );        
        Data.append( "ReqID", RequestID );         
        Data.append( "IsFile", 1 );                        
        try
        {
  	    UFC::FileStream UploadFile( FileName, O_RDONLY );
            Int32           DataSize = UploadFile.GetSize();
            unsigned char*  buffer;
            int             ReadSize;
            
            if( DataSize == 0 )
            {
                buffer   = new unsigned char[ 16 ];
                ReadSize = 16;
                memset( buffer, ' ', 16 );
                UFC::BufferedLog::Printf(" Upload file[%s] is an empty file.", FileName.c_str());
            }
            else
            {
                buffer   = new unsigned char[ DataSize ];
                ReadSize = UploadFile.Read( buffer, DataSize );
            }                               
            Data.append( "Data", buffer, ReadSize, TRUE );            
            delete [] buffer;
        }
        catch(...)
        {
            UFC::BufferedLog::Printf(" Upload file[%s] not found.", FileName.c_str());
            return;
        }           
        UFC::BufferedLog::Printf(" ---------- Send to Speedy Upload Request ----------" );
        UFC::BufferedLog::Printf(" Market[%s]", Market.c_str());
        UFC::BufferedLog::Printf(" Sender[%s]", BrokerID.c_str());
        UFC::BufferedLog::Printf(" FileCode[%s]", FileCode.c_str());
        UFC::BufferedLog::Printf(" RequestID[%s]", RequestID.c_str());        
        UFC::BufferedLog::Printf(" FileName[%s]", FileName.c_str());        
        UFC::BufferedLog::Printf(" ---------------------------------------------------" );        
        if( Market == "T" || Market == "t" )
            MessageObj->Send( "FT.REQUEST.TSE", FHostname, Data, FALSE );
        else
            MessageObj->Send( "FT.REQUEST.OTC", FHostname, Data, FALSE );
    }
    else
    	UFC::BufferedLog::Printf(" Miss tag. 08,09,13,65,05 or N2");
}
//------------------------------------------------------------------------------
