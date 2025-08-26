#include "SystexFTConnection.h"
#include "../../UFC/NetUtility.h"
#include "../../UFC/FileStream.h"
#include "../../UFC/iniFile.h"
#include "../../Migo/Sigo.h"
//------------------------------------------------------------------------------
UFC::PEvent  ProcessExistEvent;
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        UFC::BufferedLog::Printf( " ***** This process already exists *****"  );
        sleep( 2 );
        exit( 0 );
    }
    UFC::BufferedLog::Printf( " *** Process logon ***"  );
    ProcessExistEvent.SetEvent( );
}
//------------------------------------------------------------------------------
MessageObject*  MessageObj    = NULL;
//------------------------------------------------------------------------------
// Constarctor
//------------------------------------------------------------------------------
TSystexFTConnection::TSystexFTConnection( const UFC::AnsiString& ConfigFileName )
:FDebug( "" )
,FLogFile( "" )
,FIP( "" )
,FPort( 0 )
{    
    ReadConfigFile( ConfigFileName );
    MessageObj  = new MessageObject( "SystexFTConnection", "1.0", "FCM system.",FPort );   ///< Create Migo Message object.                    
    MessageObj->SetHost( FIP );
    AddListeners( ConfigFileName );
    MessageObj->SetMonitorListener( new ProcessMonitor() );
    MessageObj->Start();
    ProcessExistEvent.WaitFor( 30 );
}
//------------------------------------------------------------------------------
TSystexFTConnection::~TSystexFTConnection( void )
{   
    
}
//------------------------------------------------------------------------------
void TSystexFTConnection::SetLogObject( void )
{
    UFC::AnsiString LogFileName;
    LogFileName.Printf( "%s%sSystexFTConnection%s.log", FLogFile.c_str(), UFC::Hostname, UFC::GetDateString().c_str() );

    UFC::BufferedLog::DebugPrintf( " Write log to file:[%s]", LogFileName.c_str() );    
    UFC::BufferedLog::SetDebugMode( (FDebug == "1") );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogFileName, 1024, TRUE ) );
}
//------------------------------------------------------------------------------
void TSystexFTConnection::AddListeners( const UFC::AnsiString& ConfigFileName )
{
    try
    {
        UFC::UiniFile iniFile( ConfigFileName );
        int Count = 0;
        
		MessageObj->AddListener( "FT.FROM400.TSE", UFC::Hostname, this );        ///< Add file request Message listener to MessageObject.                
        MessageObj->AddListener( "FT.FROM400.OTC", UFC::Hostname, this );        ///< Add file request Message listener to MessageObject.                                    
		MessageObj->AddListener( "FT.FROM400.FUT", UFC::Hostname, this );        ///< Add file request Message listener to MessageObject.                                
        MessageObj->AddListener( "FT.FROM400.OPT", UFC::Hostname, this );        ///< Add file request Message listener to MessageObject.                                
        MessageObj->AddListener( "FT.RESPONSE.TSE", "0000", this );        ///< Add State query Message listener to MessageObject.                
        MessageObj->AddListener( "FT.RESPONSE.OTC", "0000", this );        ///< Add State query Message listener to MessageObject.                
        UFC::BufferedLog::Printf( " [brokers]" );
        while( TRUE )
        {
            UFC::AnsiString ID( Count );
            UFC::AnsiString Broker;
            if( iniFile.GetValue( "brokers", ID, Broker ) == TRUE )
            {
                UFC::BufferedLog::Printf( " %03d        = [%s]", Count, Broker.c_str() );                
                if( Broker.Length() == 4 )
                {
                    MessageObj->AddListener( "FT.RESPONSE.TSE", Broker, this );        ///< Add State query Message listener to MessageObject.                
                    MessageObj->AddListener( "FT.RESPONSE.OTC", Broker, this );        ///< Add State query Message listener to MessageObject.                                                       
                }
                else if( Broker.Length() == 7 )
                {
                    MessageObj->AddListener( "FT.RESPONSE.FUT", Broker, this );        ///< Add State query Message listener to MessageObject.                
                    MessageObj->AddListener( "FT.RESPONSE.OPT", Broker, this );        ///< Add State query Message listener to MessageObject.                                   
                }
                Count++;
            }
            else
                break;
        }    		
    }
    catch( UFC::Exception& ex )
    {
        UFC::BufferedLog::DebugPrintf( " %s", ex.what() );
    }
}    
//------------------------------------------------------------------------------
BOOL TSystexFTConnection::SaveTSEFile( bool IsTSE, MTree* Data, const UFC::AnsiString& BrokerID,const UFC::AnsiString& FileCode, UFC::AnsiString& FileName )
{
    FileName.Printf( "%s.%u", FileCode.c_str(), UFC::GetTickCountMS() );
    Int8* Buffer;
    Int32  Size;
    UFC::AnsiString FullFileName;
    
    if( IsTSE == true )
 	FullFileName.Printf( "%s%s/%s", FTSEInbox.c_str(),BrokerID.c_str(), FileName.c_str() );
    else
    	FullFileName.Printf( "%s%s/%s", FOTCInbox.c_str(),BrokerID.c_str(), FileName.c_str() );
    if( Data->get( "Data", Buffer, Size ) == TRUE )
    {
       try
       {
           UFC::FileStream Tmpfile( FullFileName, O_CREAT|O_TRUNC|O_WRONLY );
           Tmpfile.Write( Buffer, Size );
           Tmpfile.Flush();
           return true;
       }
       catch( ... )
       {
           UFC::BufferedLog::Printf(" Save file[%s] failed.",FullFileName.c_str() );           
           return FALSE;
       }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL TSystexFTConnection::SaveTFXFile( bool IsOPT, MTree* Data, const UFC::AnsiString& BrokerID,const UFC::AnsiString& FileCode, UFC::AnsiString& FileName   )
{
    UFC::AnsiString DesFileName,SrcFileName,CMD;    
    
    if( Data->get( "FileName", SrcFileName ) == TRUE )
    {
        FileName.Printf( "%s.%u", FileCode.c_str(), UFC::GetTickCountMS() );
        if( IsOPT == true )
            DesFileName.Printf( "%s%s", FOPTInbox.c_str(), FileName.c_str());
        else
            DesFileName.Printf( "%s%s", FFUTInbox.c_str(), FileName.c_str());
        CMD.Printf( "cp %s %s", SrcFileName.c_str(), DesFileName.c_str() );
        system( CMD.c_str() );
        UFC::BufferedLog::Printf(" Exec command[%s].",CMD.c_str() );
        return UFC::FileExists( DesFileName );
    }
    else
        UFC::BufferedLog::Printf(" Download file[%s] not found.", SrcFileName.c_str() );           
    return FALSE;
}
//------------------------------------------------------------------------------
void TSystexFTConnection::ReadConfigFile( const UFC::AnsiString& ConfigFileName )
{
    try
    {
        UFC::UiniFile iniFile( ConfigFileName );
        UFC::AnsiString Value;

        iniFile.GetValue( "main", "debug", FDebug );
        iniFile.GetValue( "main", "log_path", FLogFile );
        
        iniFile.GetValue( "main", "TSE_inbox_path", FTSEInbox );
        iniFile.GetValue( "main", "TSE_outbox_path",FTSEOutbox );
        
        iniFile.GetValue( "main", "OTC_inbox_path", FOTCInbox );
        iniFile.GetValue( "main", "OTC_outbox_path",FOTCOutbox );

        iniFile.GetValue( "main", "FUT_inbox_path", FFUTInbox );
        iniFile.GetValue( "main", "OPT_inbox_path", FOPTInbox );
        
        iniFile.GetValue( "MBus", "ip", FIP );
        iniFile.GetValue( "MBus", "port", Value );
        FPort = Value.ToInt();        
        Value = "";        

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
        if( FTSEInbox.Length() <= 0 )
        {
            FTSEInbox = "./";
            iniFile.SetValue( "main", "TSE_inbox_path", FTSEInbox );
        }                
        if( FTSEOutbox.Length() <= 0 )
        {
            FTSEOutbox = "./";
            iniFile.SetValue( "main", "TSE_outbox_path", FTSEOutbox );
        }                
        if( FOTCInbox.Length() <= 0 )
        {
            FOTCInbox = "./";
            iniFile.SetValue( "main", "OTC_inbox_path", FOTCInbox );
        }                
        if( FOTCOutbox.Length() <= 0 )
        {
            FOTCOutbox = "./";
            iniFile.SetValue( "main", "OTC_outbox_path", FOTCOutbox );
        }                
        if( FFUTInbox.Length() <= 0 )
        {
            FFUTInbox = "./";
            iniFile.SetValue( "main", "FUT_inbox_path", FFUTInbox );
        }                
        if( FOPTInbox.Length() <= 0 )
        {
            FOPTInbox = "./";
            iniFile.SetValue( "main", "OPT_inbox_path", FOPTInbox );
        }                
        if( FIP.Length() <= 0 )
        {
            FIP = "127.0.0.1";
            iniFile.SetValue( "MBus", "ip", FIP );
        }
        if( FPort <= 0 )
        {
            Value = "12345";
            FPort = Value.ToInt();
            iniFile.SetValue( "MBus", "port", Value );
        }        
        
        UFC::BufferedLog::Printf( " [ main ]" );
        UFC::BufferedLog::Printf( " debug       = [%s]", FDebug.c_str() );
        UFC::BufferedLog::Printf( " log_path    = [%s]", FLogFile.c_str() );        
        UFC::BufferedLog::Printf( " TSE_inbox_path  = [%s]", FTSEInbox.c_str() );
        UFC::BufferedLog::Printf( " TSE_outbox_path = [%s]", FTSEOutbox.c_str() );
        UFC::BufferedLog::Printf( " OTC_inbox_path  = [%s]", FOTCInbox.c_str() );
        UFC::BufferedLog::Printf( " OTC_outbox_path = [%s]", FOTCOutbox.c_str() );
        
        UFC::BufferedLog::Printf( " FUT_inbox_path  = [%s]", FFUTInbox.c_str() );
        UFC::BufferedLog::Printf( " OPT_inbox_path  = [%s]", FOPTInbox.c_str() );;
        UFC::BufferedLog::Printf( " " );        
        UFC::BufferedLog::Printf( " [MBus]" );
        UFC::BufferedLog::Printf( " ip          = [%s]", FIP.c_str() );
        UFC::BufferedLog::Printf( " port        = [%d]", FPort );        
        UFC::BufferedLog::Printf( " " );        
        SetLogObject();        
    }
    catch( UFC::Exception& ex )
    {
				UFC::BufferedLog::DebugPrintf(" %s", ex.what());
    }
}
//------------------------------------------------------------------------------
void TSystexFTConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{
    if( Subject == "FT.RESPONSE.TSE" )
        HandleTSEMessage( Subject, Key, Data, /*TSE*/TRUE, /*Is response*/ TRUE  );
    else if( Subject == "FT.RESPONSE.OTC" )
        HandleTSEMessage( Subject, Key, Data, /*OTC*/FALSE,/*Is response*/ TRUE  );
    else if( Subject == "FT.FROM400.TSE" )
        HandleTSEMessage( Subject, Key, Data, /*TSE*/TRUE,/*Is request*/ FALSE  );
    else if( Subject == "FT.FROM400.OTC" )
        HandleTSEMessage( Subject, Key, Data, /*OTC*/FALSE,/*Is request*/ FALSE  );
    else if( Subject == "FT.RESPONSE.OPT" ) ///-------------------------------- Futures/Options ------------------------------- ///
        HandleTFXMessage( Subject, Key, Data, /*OPT*/TRUE, /*Is response*/ TRUE  );
    else if( Subject == "FT.RESPONSE.FUT" )
        HandleTFXMessage( Subject, Key, Data, /*FUT*/FALSE,/*Is response*/ TRUE  );
    else if( Subject == "FT.FROM400.OPT" )
        HandleTFXMessage( Subject, Key, Data, /*OPT*/TRUE,/*Is request*/ FALSE );
    else if( Subject == "FT.FROM400.FUT" )
        HandleTFXMessage( Subject, Key, Data, /*FUT*/FALSE,/*Is request*/ FALSE );
    return; ///< Unhandled subject.    	    
}
//------------------------------------------------------------------------------
BOOL IsAlphaNum( char ch )
{
    if( ch >= '0' && ch <= '9' )
        return TRUE;
    if( ch >= 'a' && ch <= 'z' )
        return TRUE;
    if( ch >= 'A' && ch <= 'Z' )
        return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------
void ConvertString( UFC::AnsiString& String )
{
    char* str = (char*)String.c_str();
    
    for ( int i=0;i<String.Length(); i++ )
    {
        if( IsAlphaNum(  *(str + i ) ) == FALSE )
            *(str + i ) = '@';
    }            
}
//------------------------------------------------------------------------------
void TSystexFTConnection::HandleTSEMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data, BOOL IsTSE, BOOL IsResponse  )
{
    UFC::AnsiString BrokerID, RequestID, FileCode,TimeNow, Host;
    Int32           Status, IsFile;
    TSQMessage      ReplyMessage;
    UFC::UDateTime  Today;    
    
    if( IsResponse ) ///< Reply to AS400 via MBus
    {
        if( Data->get( "Sender", BrokerID ) &&
            Data->get( "FileCode", FileCode ) &&        
            Data->get( "IsFile", IsFile ) && 
            Data->get( "Status", Status ) &&
            Data->get( "Host", Host ) )
        {
			if( Host.AnsiCompare( UFC::Hostname ) == 0 )
			{
				GetHHMMSS( TimeNow );                    
				ReplyMessage.Append( "00", "0" );    
				ReplyMessage.Append( "02", Host );    
				ReplyMessage.Append( "08", Key.SubString( 0,2 ) );
				ReplyMessage.Append( "09", Key.SubString( 2,2 ) );                
				if( IsTSE == TRUE )
					ReplyMessage.Append( "13", "T" );
				else
					ReplyMessage.Append( "13", "O" );
				Today.setCurrent();    
				ReplyMessage.Append( "10", (long)Today.getDate() ); ///<Day*/    
				ReplyMessage.Append( "L9", TimeNow );
				ReplyMessage.Append( "05", FileCode );
				if( Data->get( "ReqID", RequestID ) )        
					ReplyMessage.Append( "65", RequestID );       
				if( IsFile == TRUE )
				{
					UFC::AnsiString DLFile;
					if( SaveTSEFile( IsTSE, Data, Key, FileCode, DLFile ) == FALSE )
					{
						UFC::BufferedLog::Printf(" Save downloaded file [%s]failed.",DLFile.c_str() );
						return;
					}
					else
						ReplyMessage.Append( "N2", DLFile );      
				}
				else
				{
					char* MsgData;
					Int32 Size;
					if( Data->get( "Data", MsgData, Size ) )
					{
						UFC::AnsiString TagHQ( MsgData, Size );
	                    
						ConvertString( TagHQ );
						ReplyMessage.Append( "HQ", TagHQ );
						UFC::BufferedLog::Printf(" Received message[%s] size[%d].", MsgData, Size );
					}
				}
				ReplyMessage.Append( "40", (long)Status );               

				UFC::AnsiString Value = ReplyMessage.ToString();
				MTree           SendData;

				SendData.append( "Data", Value );                
				UFC::BufferedLog::Printf(" Send FT Response[%s].", Value.c_str() );
				if( IsTSE == TRUE )
					MessageObj->Send( "FT.TO400.TSE", Key, SendData, FALSE );
				else
					MessageObj->Send( "FT.TO400.OTC", Key, SendData, FALSE );
			}
        }
    }
    else if( Subject == "FT.FROM400.TSE" || Subject ==  "FT.FROM400.OTC" ) ///< Request from AS400
    {
        UFC::AnsiString MessageString;

        if( Data->get( "Data", MessageString ) == TRUE )
        {
			MessageString.TrimRight();
            TSQMessage SQMessage;

            SQMessage.FromString( MessageString );            
            UFC::BufferedLog::Printf(" Recv FT Request[%s].", MessageString.c_str() );
            UFC::AnsiString FileName;
            if( SQMessage.Get( "N2", FileName ) == TRUE  ) 
                FileUpload( IsTSE, SQMessage );
            else
                TSEFileRequest( SQMessage );    
        }
    }    
}
//------------------------------------------------------------------------------
void TSystexFTConnection::HandleTFXMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data, BOOL IsOPT, BOOL IsResponse  )
{
    UFC::AnsiString BrokerID, RequestID, FileCode,TimeNow, SrcFile;
    Int32           Status;
    TSQMessage      ReplyMessage;
    UFC::UDateTime  Today;    
    
    if( IsResponse )
    {
        if( Data->get( "Sender", BrokerID ) &&
            Data->get( "FileCode", FileCode ) &&                    
            Data->get( "Status", Status ) )
        {
				GetHHMMSS( TimeNow );                    
				ReplyMessage.Append( "00", "0" );    
				ReplyMessage.Append( "07", BrokerID.SubString( 0, 1 ) );
				ReplyMessage.Append( "08", BrokerID.SubString( 1, 3 ) );
				ReplyMessage.Append( "09", BrokerID.SubString( 4, 3 ) );    
				if( IsOPT == TRUE )
					ReplyMessage.Append( "13", "OPT" );
				else
					ReplyMessage.Append( "13", "FUT" );
				Today.setCurrent();    
				ReplyMessage.Append( "10", (long)Today.getDate() ); ///< Day
				ReplyMessage.Append( "L9", TimeNow );
				ReplyMessage.Append( "05", FileCode );
				if( Status == 0 ) ///< Download file OK.
				{
					UFC::AnsiString DLFile;
	                
					if( SaveTFXFile( IsOPT, Data, Key, FileCode, DLFile ) == FALSE )                    
					{
						UFC::BufferedLog::Printf(" Save downloaded OPT file [%s] failed.", DLFile.c_str() );
						ReplyMessage.Append( "40", (long)17 ); ///  FTP_FILE_IS_READY_BUT_EMPTY
					}
					else
					{
						ReplyMessage.Append( "N2", DLFile ); ///< Download filename.                   
						ReplyMessage.Append( "40", (long)0 );///< OK!!                
					}
				}    
				else
					ReplyMessage.Append( "40", (long)Status );               
				UFC::AnsiString Value = ReplyMessage.ToString();
				MTree           SendData;

				SendData.append( "Data", Value );                
				UFC::BufferedLog::Printf(" Send FT Response[%s].", Value.c_str() );
				if( IsOPT == TRUE )
					MessageObj->Send( "FT.TO400.OPT", Key, SendData, FALSE );
				else
					MessageObj->Send( "FT.TO400.FUT", Key, SendData, FALSE );
        }
    }
    else if( Subject == "FT.FROM400.FUT" || Subject ==  "FT.FROM400.OPT" )
    {        
        UFC::AnsiString MessageString;
        if( Data->get( "Data", MessageString ) == TRUE )
        {
			MessageString.TrimRight();
            TSQMessage SQMessage;    
            SQMessage.FromString( MessageString );
            
            UFC::BufferedLog::Printf(" Recv FT Request[%s].", MessageString.c_str() );
            UFC::AnsiString FileName;
            if( SQMessage.Get( "N2", FileName ) == FALSE  )                 
                TFXFileRequest( SQMessage );    
            else
                UFC::BufferedLog::Printf(" TAIFEX not support File upload." );
        }
    }    
}
//------------------------------------------------------------------------------
void TSystexFTConnection::Run( void )
{
    while( TRUE )
    {
        UFC::SleepMS( 1000 );       
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
    }
}
//------------------------------------------------------------------------------
void TSystexFTConnection::TSEFileRequest( TSQMessage& SQMessage )
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
	UFC::AnsiString ExtraData( ReqData.c_str() + 3 );
        MTree Data;
                
        BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Data.append( "Sender", BrokerID );
        UFC::BufferedLog::Printf(" Sender:[%s]", BrokerID.c_str());
        Data.append( "FileCode", FileCode );        
        UFC::BufferedLog::Printf(" FileCode:[%s]", FileCode.c_str());
        Data.append( "ReqID", RequestID );         
        UFC::BufferedLog::Printf(" RequestID:[%s]", RequestID.c_str());
        Data.append( "IsFile", 0 );                
	if( ExtraData.Length() > 0 )
	    Data.append( "Data", ExtraData );	
        if( Market == "T" || Market == "t" )
            MessageObj->Send( "FT.REQUEST.TSE", UFC::Hostname, Data, FALSE );
        else
            MessageObj->Send( "FT.REQUEST.OTC", UFC::Hostname, Data, FALSE );
    }
    else
    	UFC::BufferedLog::Printf(" Miss tag(08,09,13,65,05,HQ or M1) .");
}
//------------------------------------------------------------------------------
void TSystexFTConnection::TFXFileRequest( TSQMessage& SQMessage )
{
    UFC::AnsiString TFXID,CompanyID, BranchID, Market, RequestID, FileCode, ReqData, Force;
    
    if( SQMessage.Get( "07", TFXID ) &&
        SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "13", Market ) &&
        SQMessage.Get( "05", FileCode )) 
    {
        
        MTree Msg;
        
        Msg.append( "FileCode", FileCode );
        if( SQMessage.Get( "HQ", ReqData ) == TRUE )            
            Msg.append( "Data", ReqData );
        else
            Msg.append( "Data", FileCode );
        if( Market == "OPT"  )
        {
        		UFC::BufferedLog::Printf(" Request Option file[%s]", FileCode.c_str() );
            MessageObj->Send(  "SPEEDY.FTP.OPT", UFC::Hostname, Msg );
        }
        else
        {
        		UFC::BufferedLog::Printf(" Request Futures file[%s]", FileCode.c_str() );
            MessageObj->Send(  "SPEEDY.FTP.FUT", UFC::Hostname, Msg );
        }
    }
    else
    	UFC::BufferedLog::Printf(" Miss tag(07,08,09 or 13).");
}
//------------------------------------------------------------------------------
void TSystexFTConnection::FileUpload( bool IsTSE, TSQMessage& SQMessage )
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
        UFC::AnsiString FullPathFileName;
        MTree Data;
        
        BrokerID.Printf( "%s%s", CompanyID.c_str(), BranchID.c_str() );
        Data.append( "Sender", BrokerID );
        Data.append( "FileCode", FileCode );        
        Data.append( "ReqID", RequestID );         
        Data.append( "IsFile", 1 );             
        if( IsTSE == true ) 
        	  FullPathFileName.Printf( "%s%s/%s", FTSEOutbox.c_str(), BrokerID.c_str(), FileName.c_str());
        else
        	  FullPathFileName.Printf( "%s%s/%s", FOTCOutbox.c_str(), BrokerID.c_str(), FileName.c_str());
        
        try
        {
  			UFC::FileStream UploadFile( FullPathFileName, O_RDONLY );
            Int32           DataSize = UploadFile.GetSize();
            unsigned char*  buffer;
            int             ReadSize;
            
            if( DataSize == 0 )
            {
                buffer   = new unsigned char[ 16 ];
                ReadSize = 16;
                memset( buffer, ' ', 16 );
                UFC::BufferedLog::Printf(" Upload file [%s] is an empty file.", FullPathFileName.c_str());
            }
            else
            {
                UFC::AnsiString RealSize;
                if( SQMessage.Get( "Z5", RealSize ) )
                {
                    UFC::BufferedLog::Printf(" Set upload file size from[%d] to[%d].", DataSize ,RealSize.ToInt() );
                    DataSize = RealSize.ToInt();
                }
                buffer   = new unsigned char[ DataSize ];
                ReadSize = UploadFile.Read( buffer, DataSize );
            }                               
            Data.append( "Data", buffer, ReadSize, TRUE );            
            delete [] buffer;
        } 
        catch(...)
        {
            UFC::BufferedLog::Printf(" Upload file [%s] not found", FullPathFileName.c_str());
            return;
        }                
        if( Market == "T" || Market == "t" )
            MessageObj->Send( "FT.REQUEST.TSE", UFC::Hostname, Data, FALSE );
        else
            MessageObj->Send( "FT.REQUEST.OTC", UFC::Hostname, Data, FALSE );
    }
}
//------------------------------------------------------------------------------
