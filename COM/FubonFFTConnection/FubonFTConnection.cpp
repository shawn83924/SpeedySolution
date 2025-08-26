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
,FIP( "" )
,FHostname( UFC::Hostname )
,FPort( 0 )
,FReadBufferSize( 0 )
,FReqQueue( NULL )
,FReadBuffer( NULL )
{   
    UFC::AnsiString FFTCName;
    
    ReadConfigFile( ConfigFileName );
    LockFile();    
    FFTCName.Printf( "FFTC_%s@%s",FAppendName.c_str(), UFC::Hostname );    
    FReadBuffer = new char[ FReadBufferSize ];
    FReqQueue      = new TSysque( FQueIP, FQuePort, FReqQueName );        
    FResponseQueue = new TSysque( FQueIP, FQuePort, FResponseQueName );                            
    MessageObj  = new MessageObject( FFTCName, "1.0", "FCM system.",FPort );   ///< Create Migo Message object.                    
    MessageObj->SetHost( FIP );
    MessageObj->AddListener( "SPEEDY.FTP.FUT", FHostname, this );
    MessageObj->AddListener( "SPEEDY.FTP.OPT", FHostname, this );
    MessageObj->AddListener( "FLEX.RESPONSE.FUT", FHostname, this );
    MessageObj->AddListener( "FLEX.RESPONSE.OPT", FHostname, this );
    MessageObj->SetMonitorListener( new ProcessMonitor() );        
    MessageObj->Start();     
    ProcessExistEvent.WaitFor( 30 );
    if( FReqQueue->Open() == FALSE || FResponseQueue->Open() == FALSE )
    {
        UFC::BufferedLog::Printf(" Open sys queue failed!" );
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
    LogFileName.Printf( "%s%sFubonFFTConnection%s.log", FLogFile.c_str(), UFC::Hostname, UFC::GetDateString().c_str() );

    UFC::BufferedLog::DebugPrintf( " Write log to file:[%s]", LogFileName.c_str() );    
    UFC::BufferedLog::SetDebugMode( (FDebug == "1") );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogFileName, 1024, TRUE ) );
}
//------------------------------------------------------------------------------
BOOL TFubonOrderConnection::SaveFile( MTree* Data, const UFC::AnsiString& FileCode, UFC::AnsiString& FileName )
{       
    Int8* Buffer;
    Int32  Size;
    UFC::AnsiString FullPathName;
 
    
    if( Data->get( "Data", Buffer, Size ) == TRUE && Data->get( "FileName", FullPathName )  == TRUE )
    {
	UFC::BufferedLog::Printf(" Server File[%s]",FullPathName.c_str() );	
	UFC::AnsiString Name = ExtractFileName( FullPathName );
	FileName.Printf( "%s%s", FInbox.c_str(), Name.c_str() );
	UFC::BufferedLog::Printf(" Local File[%s]",FileName.c_str() );	
       try
       {
           UFC::FileStream Tmpfile( FileName, O_CREAT|O_TRUNC|O_WRONLY );
           Tmpfile.Write( Buffer, Size );
           Tmpfile.Flush();
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
        UFC::AnsiString Value,BCCode,BrokerID;

        iniFile.GetValue( "main", "debug", FDebug );
        iniFile.GetValue( "main", "log_path", FLogFile );
        iniFile.GetValue( "main", "lock_path", FLockFile );
        iniFile.GetValue( "main", "inbox_path", FInbox );
	iniFile.GetValue( "main", "BrokerID", BrokerID );
        
        
        iniFile.GetValue( "MBus", "ip", FIP );
        iniFile.GetValue( "MBus", "port", Value );
        iniFile.GetValue( "MBus", "Hostname", FHostname );
        FPort = Value.ToInt();
        Value = "";
                
        iniFile.GetValue( "sysque", "ReqName", FReqQueName );
	iniFile.GetValue( "sysque", "ResponseName", FResponseQueName );
        iniFile.GetValue( "sysque", "ip", FQueIP );
        iniFile.GetValue( "sysque", "port", Value );
        FQuePort = Value.ToInt();
        Value = "";		
        iniFile.GetValue( "sysque", "read_buffer_size", Value );
        FReadBufferSize = Value.ToInt();
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
        if( FLockFile.Length() <= 0 )
        {
            FLockFile = "./";
            iniFile.SetValue( "main", "lock_path", FLockFile );
        }                
        if( BrokerID.Length() != 7 )
        {
            BrokerID = "F026000";
            iniFile.SetValue( "main", "BrokerID", BrokerID );
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
            FReqQueName = "F026000_Req";
            iniFile.SetValue( "sysque", "ReqName", FReqQueName );
        }
	if( FResponseQueName.Length() <= 0 )
	{
	    FResponseQueName = "F026000_Response";
            iniFile.SetValue( "sysque", "ResponseName", FResponseQueName );
        }
	    
        if( FReadBufferSize <= 0 )
        {
            Value = "512";
            FReadBufferSize = Value.ToInt();
            iniFile.SetValue( "sysque", "read_buffer_size", Value );
        }   
	FPrefix    = BrokerID.SubString( 0, 1 );
	FCompanyID = BrokerID.SubString( 1, 3);
	FBranchID  = BrokerID.SubString( 4, 3);
        iniFile.Save();                
        
        UFC::BufferedLog::Printf( " [ main ]" );
        UFC::BufferedLog::Printf( " debug              = [%s]", FDebug.c_str() );
        UFC::BufferedLog::Printf( " log_path           = [%s]", FLogFile.c_str() );
        UFC::BufferedLog::Printf( " lock_path          = [%s]", FLockFile.c_str() );
        UFC::BufferedLog::Printf( " inbox_path         = [%s]", FInbox.c_str() );
	UFC::BufferedLog::Printf( " BrokerID           = [%s%s%s]", FPrefix.c_str(), FCompanyID.c_str(), FBranchID.c_str() );
        UFC::BufferedLog::Printf( " " );        
        UFC::BufferedLog::Printf( " [speedy]" );
        UFC::BufferedLog::Printf( " ip                 = [%s]", FIP.c_str() );
        UFC::BufferedLog::Printf( " Hostname           = [%s]", FHostname.c_str() );
        UFC::BufferedLog::Printf( " port               = [%d]", FPort );        
        
        UFC::BufferedLog::Printf( " " );
        UFC::BufferedLog::Printf( " [sysque]" );        
        UFC::BufferedLog::Printf( " ReqName            = [%s]", FReqQueName.c_str() );        
	UFC::BufferedLog::Printf( " ResponseName       = [%s]", FResponseQueName.c_str() );        
        UFC::BufferedLog::Printf( " read_buffer_size   = [%d]", FReadBufferSize );	
        UFC::BufferedLog::Printf( " " );
        FIniFile = new UFC::UiniFile( ConfigFileName );
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
    LockFileName.Printf( "%s%sfepFFT_%s_lock", FLockFile.c_str(), UFC::Hostname, FAppendName.c_str() );
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
void TFubonOrderConnection::ToSysQueue(  TSQMessage& SQM )
{
    UFC::AnsiString Value = SQM.ToString();
    
    UFC::BufferedLog::Printf(" Reply[%s]", Value.c_str() );
    FResponseQueue->Write( (char*)(Value.c_str()), Value.Length() );        
 }
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{
    if( Subject == "FLEX.RESPONSE.FUT" || Subject == "FLEX.RESPONSE.OPT" )
        FLEXResponse( Subject, Data );
    else
        FTResponse( Subject, Data );
}        
//------------------------------------------------------------------------------
void TFubonOrderConnection::FLEXResponse(  const UFC::AnsiString& Subject, MTree* Data )
{           
    Int32           ReqID,ErrorCode;
    UFC::AnsiString time,seq,code,ReqType;
    TSQMessage      ReplyMessage;
    
    if( Data->get( "SecurityRequestType", ReqType ) &&
        Data->get( "SecurityReqID", ReqID ) &&        
        Data->get( "status_code", ErrorCode ) )
    {
        UFC::GetHHMMSS(time);
        seq.Printf( "%d", ReqID );
        code.Printf( "%03d", ErrorCode );
        ReplyMessage.Append( "00", "F" );    
        ReplyMessage.Append( "04", "F" );     
        if( Subject == "FLEX.RESPONSE.FUT" )
            ReplyMessage.Append( "13", "FUT" );
        else
            ReplyMessage.Append( "13", "OPT" );        
        ReplyMessage.Append( "L9", time );   
        ReplyMessage.Append( "F0", "F" );    
        ReplyMessage.Append( "65", seq );        
        ReplyMessage.Append( "40", code );
        UFC::BufferedLog::Printf(" ------- FLEX Product Response -------" );        
        UFC::BufferedLog::Printf(" RequestType:[%s]", ReqType.c_str() );	
        UFC::BufferedLog::Printf(" RequestID:[%d]", ReqID);
        UFC::BufferedLog::Printf(" status_code:[%03d]", ErrorCode);
        UFC::BufferedLog::Printf(" Subject:[%s]", Subject.c_str() );	
        UFC::BufferedLog::Printf(" ------------------------------------" );
	ToSysQueue( ReplyMessage );        
    }
    else
    	UFC::BufferedLog::Printf(" Speedy FLEX Prod Response Missing fields SecurityRequestType,SecurityReqID or status_code");    
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::FTResponse(  const UFC::AnsiString& Subject, MTree* Data )
{           
    UFC::AnsiString FileCode,TimeNow,Src,Zero( "0" );
    Int32           ReqID,ErrorCode;
    TSQMessage      ReplyMessage;
    UFC::UDateTime  Today;
    
    
    if( Data->get( "Src", Src ) &&
        Data->get( "ReqID", ReqID ) &&        
        Data->get( "FileCode", FileCode ) && 
        Data->get( "ErrorCode", ErrorCode ) )
    {
	if( Src == "TMP" ) ///< Reject by TAIFEX
	{
	    if( ErrorCode == 0 )
		return;    
	    GetHHMMSS( TimeNow );        

	    Today.setCurrent();    
	    ReplyMessage.Append( "00", Zero );    
	    ReplyMessage.Append( "07", FPrefix );    
	    ReplyMessage.Append( "08", FCompanyID );
	    ReplyMessage.Append( "09", FBranchID );    
	    if( Subject == "SPEEDY.FTP.FUT" )
		ReplyMessage.Append( "13", "FUT" );
	    else
		ReplyMessage.Append( "13", "OPT" );        
	    ReplyMessage.Append( "10", (long)Today.getDate() ); ///<Day*/    
	    ReplyMessage.Append( "L9", TimeNow );
	    ReplyMessage.Append( "05", FileCode );
	    ReplyMessage.Append( "65", (long)ReqID );               
	    ReplyMessage.Append( "40", (long)ErrorCode );               
	    ToSysQueue( ReplyMessage );
	}
	else if( Src == "FTP"  ) ///< Download by SpeedyFTDownload
	{
	    GetHHMMSS( TimeNow );        

	    Today.setCurrent();    
	    ReplyMessage.Append( "00", Zero );    
	    ReplyMessage.Append( "07", FPrefix );    
	    ReplyMessage.Append( "08", FCompanyID );
	    ReplyMessage.Append( "09", FBranchID );    
	    if( Subject == "SPEEDY.FTP.FUT" )
		ReplyMessage.Append( "13", "FUT" );
	    else
		ReplyMessage.Append( "13", "OPT" );        
	    ReplyMessage.Append( "10", (long)Today.getDate() ); ///<Day*/    
	    ReplyMessage.Append( "L9", TimeNow );
	    ReplyMessage.Append( "05", FileCode );
	    ReplyMessage.Append( "65", (long)ReqID );               
	    if( ErrorCode == 200 ) ///< download Ok
	    {
		UFC::AnsiString LocalFile;
		
		if( SaveFile( Data, FileCode, LocalFile ) == FALSE )
		{
		    UFC::BufferedLog::Printf(" Save downloaded file [%s]failed.",LocalFile.c_str() );
		    ReplyMessage.Append( "40", (long)200 );    
		}
		else
		{
		    ReplyMessage.Append( "40", (long)0 );    
		    ReplyMessage.Append( "N2", LocalFile );      		
		}
	    }
	    else
		ReplyMessage.Append( "40", (long)ErrorCode );    
	    ToSysQueue( ReplyMessage );
	}    
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
        UFC::SleepMS( 50 );        
    }
}
//------------------------------------------------------------------------------
// Send R13 to TAIFEX
//------------------------------------------------------------------------------
void TFubonOrderConnection::OnData( void )
{
    TSQMessage      SQMessage;
    UFC::AnsiString MessageString( FReadBuffer );
    UFC::AnsiString MsgType;
    
    SQMessage.FromString( MessageString );    
    if( SQMessage.Get( "00", MsgType ) )
    {
        if( MsgType == "F" )
            FLEXProdRequest(  SQMessage );
        else
            FileRequest( SQMessage );    
    }    
    else
        UFC::BufferedLog::Printf(" Missing Tag[00] Message[%s]",FReadBuffer );
}
//------------------------------------------------------------------------------
int  TFubonOrderConnection::ToIndexBrokerID(  const UFC::AnsiString& BrokerID )
{
    UFC::AnsiString Value;
    
    if( FIniFile->GetValue("BrokerID",BrokerID,Value) == true )
    {
        int index =  Value.ToInt();
        UFC::BufferedLog::Printf(" BrokerID[%s] -> Index[%d]", BrokerID.c_str(), index );	
        return index;
    }
    else
        UFC::BufferedLog::Printf(" BrokerID [%s] not found", BrokerID.c_str() );	
    return 0;
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::FLEXProdRequest( TSQMessage& SQMessage )
{
    UFC::AnsiString PreFix,CompanyID, BranchID, Market, RequestID,RequestType;
    UFC::AnsiString FCMID,ACNO,APPTYPE,ROOTSYM,EXPIRY,CDDATE,STRIKEPX,CP;
    
    if( SQMessage.Get( "07", PreFix ) &&
	SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&        
        SQMessage.Get( "65", RequestID ) &&
        SQMessage.Get( "F0", RequestType ) &&
        SQMessage.Get( "F1", Market ) &&
        SQMessage.Get( "F2", FCMID ) &&
        SQMessage.Get( "F3", ACNO ) &&
        SQMessage.Get( "F4", APPTYPE ) &&
        SQMessage.Get( "F5", ROOTSYM ) &&            
        SQMessage.Get( "F6", EXPIRY ) &&                    
        SQMessage.Get( "F7", CDDATE ) )                   
        
    {    
        UFCType::Int32 ReqID = RequestID.ToInt();
        //UFCType::Int32 brokerid = ToIndexBrokerID( FCMID );
        MTree Data;        
        
        Data.append( "SecurityRequestType", RequestType );         
        Data.append( "SecurityReqID", ReqID );         
        Data.append( "type", Market );         
        Data.append( "root_symbol", ROOTSYM );         
        Data.append( "BrokerID", FCMID );
        Data.append( "investor_acno", ACNO.ToInt() );
        Data.append( "application_type", APPTYPE );
        Data.append( "contract_date", CDDATE );
        
        UFC::BufferedLog::Printf(" ------- FLEX Product Request -------" );        
        UFC::BufferedLog::Printf(" RequestType:[%s]", RequestType.c_str() );	
        UFC::BufferedLog::Printf(" RequestID:[%d]", ReqID);	
        UFC::BufferedLog::Printf(" type:[%s]", Market.c_str() );	
        UFC::BufferedLog::Printf(" root_symbol:[%s]", ROOTSYM.c_str() );	
        UFC::BufferedLog::Printf(" BrokerID:[%s]", FCMID.c_str() );	
        UFC::BufferedLog::Printf(" investor_acno:[%d]", ACNO.ToInt());	
        UFC::BufferedLog::Printf(" application_type:[%s]", APPTYPE.c_str());	
        UFC::BufferedLog::Printf(" contract_date:[%s]", CDDATE.c_str() );
        if( Market == "O" || Market == "o" )
        {
            if( SQMessage.Get( "F8", STRIKEPX ) && SQMessage.Get( "F9", CP )) 
            {
                UFC::BufferedLog::Printf(" strike_price:[%d]", STRIKEPX.ToInt() );
                UFC::BufferedLog::Printf(" call_put_code:[%s]", CP.c_str() );
                Data.append( "strike_price", STRIKEPX.ToInt() );
                Data.append( "call_put_code", CP );
                MessageObj->Send( "FLEX.REQUEST.OPT", FHostname, Data, FALSE );
            }
            else
                UFC::BufferedLog::Printf(" FLEX Options Prod Request Missing tag F8,F9.");    
        }
        else
            MessageObj->Send( "FLEX.REQUEST.FUT", FHostname, Data, FALSE );
        UFC::BufferedLog::Printf(" ------------------------------------" );
    }
    else
    	UFC::BufferedLog::Printf(" FLEX Prod Request Missing tag.(Need 07,08,09,65,F0,F1,F2,F3,F4,F5,F6,F7,F8,F9)");    
}
//------------------------------------------------------------------------------
void TFubonOrderConnection::FileRequest( TSQMessage& SQMessage )
{
    UFC::AnsiString PreFix,CompanyID, BranchID, Market, RequestID, FileCode, ReqData, Force,TS("0");
    
    if( SQMessage.Get( "07", PreFix ) &&
	SQMessage.Get( "08", CompanyID ) &&
        SQMessage.Get( "09", BranchID ) &&
        SQMessage.Get( "13", Market ) &&
        SQMessage.Get( "65", RequestID ) &&
        SQMessage.Get( "05", FileCode ) &&
        SQMessage.Get( "HQ", ReqData ) &&
        SQMessage.Get( "M1", Force )) 
    {
        UFC::AnsiString BrokerID;
	UFCType::Int32  ReqID = RequestID.ToInt();
        UFCType::Int32  TradSession,IsFLEX;
        MTree Data;

        SQMessage.Get( "TS", TS );                
        TradSession = TS.ToInt();
        if( FileCode.Length() == 4 &&  FileCode[0] == 'X')
            IsFLEX = true;
        else
            IsFLEX = false;
        BrokerID.Printf( "%s%s%s", PreFix.c_str(), CompanyID.c_str(), BranchID.c_str() );
        UFC::BufferedLog::Printf(" ---------- FT Request ----------" );        
        UFC::BufferedLog::Printf(" Sender:[%s]",    BrokerID.c_str());
        UFC::BufferedLog::Printf(" FileCode:[%s]",  FileCode.c_str());
        UFC::BufferedLog::Printf(" RequestID:[%d]", ReqID);	
        UFC::BufferedLog::Printf(" TradeKind:[%d]", TradSession );	
        UFC::BufferedLog::Printf(" FLEX:[%d]",      IsFLEX );	
	UFC::BufferedLog::Printf(" Body:[%d]", ReqData.c_str() );	        
        
        Data.append( "FLEX", (UFCType::Int32) IsFLEX );         
        Data.append( "TradeKind", (UFCType::Int32) TradSession );                 
        Data.append( "ReqID", ReqID );         
	if( ReqData.Length() > 0 )
	    Data.append( "Body", ReqData );
        if( Market == "FUT" || Market == "fut" ) ///< Futures 
            MessageObj->Send( "SPEEDY.FTP.FUT", FHostname, Data, FALSE );
        else ///< Options
            MessageObj->Send( "SPEEDY.FTP.OPT", FHostname, Data, FALSE );
        UFC::BufferedLog::Printf(" --------------------------------" );
    }
    else
    	UFC::BufferedLog::Printf(" File Request Missing tag.(Need 07,08,09,13,65,05,HQ,M1)");
}
//------------------------------------------------------------------------------
