#include "MegaFTConnection.h"
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
    }
    UFC::BufferedLog::Printf( " *** Process logon ***"  );
    ProcessExistEvent.SetEvent( );
}
//------------------------------------------------------------------------------------------------------------------------
void ToLocalTime( time_t tv_sec, struct tm *pttm )
{
    int Today_sec = tv_sec % 86400;
    
    pttm->tm_hour =  ((Today_sec / 3600 ) + 8 ) % 24;
    pttm->tm_min  =  ( Today_sec % 3600 ) / 60;
    pttm->tm_sec  =  Today_sec % 60;    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GetTimeHHMMSSmm( UFC::AnsiString& TimeNow )
{
    struct tm    FTime;
    struct timeb tb;

    ftime( &tb );
    ToLocalTime( tb.time, &FTime );
    TimeNow.Printf( "%02d%02d%02d%02d", FTime.tm_hour, FTime.tm_min, FTime.tm_sec, tb.millitm/10 );
}
//------------------------------------------------------------------------------
MessageObject*  MessageObj    = NULL;
//------------------------------------------------------------------------------
TMegaFTConnection::TMegaFTConnection( const UFC::AnsiString& ConfigFileName )
:UFC::PThread( NULL, FALSE )
,FDebug( "" )
,FLogFile( "" )
,FIP( "" )
,FHostname( UFC::Hostname )
,FPort( 0 )
,FReqQueue( FALSE )
{   
    UFC::AnsiString FFTCName;
    
    ReadConfigFile( ConfigFileName );
    FFTCName.Printf( "MFTC_%s@%s",FAppendName.c_str(), UFC::Hostname );    
    if( !FReqQueue.Open( FReqQueueID, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open request queue 0x%x failed.", FReqQueueID );
        exit( 0 );
    }        
    if( !FRespQueue.Open( FRespQueueID, TRUE ) )
    {
        UFC::BufferedLog::Printf( " Open response queue 0x%x failed.", FRespQueueID );
        exit( 0 );
    }        
    MessageObj  = new MessageObject( FFTCName, "1.0", "FCM system.",FPort );   ///< Create Migo Message object.                    
    MessageObj->SetHost( FIP );
    OpenQueues( ConfigFileName );                 
    MessageObj->AddListener( "SPEEDY.FTP.FUT", FHostname, this );
    MessageObj->AddListener( "SPEEDY.FTP.OPT", FHostname, this );
    MessageObj->AddListener( "FLEX.RESPONSE.FUT", FHostname, this );
    MessageObj->AddListener( "FLEX.RESPONSE.OPT", FHostname, this );
    MessageObj->SetMonitorListener( new ProcessMonitor() );        
    MessageObj->Start();     
    ProcessExistEvent.WaitFor( 30 );
    Start();         
}
//------------------------------------------------------------------------------
TMegaFTConnection::~TMegaFTConnection( void )
{   
    
}
//------------------------------------------------------------------------------
void TMegaFTConnection::SetLogObject( void )
{
    UFC::AnsiString LogFileName;
    LogFileName.Printf( "%s%sMegaFTConnection%s.log", FLogFile.c_str(), UFC::Hostname, UFC::GetDateString().c_str() );

    UFC::BufferedLog::DebugPrintf( " Write log to file:[%s]", LogFileName.c_str() );    
    UFC::BufferedLog::SetDebugMode( (FDebug == "1") );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogFileName, 1024, TRUE ) );
}
//------------------------------------------------------------------------------
void TMegaFTConnection::OpenQueues( const UFC::AnsiString& ConfigFileName )
{
    try
    {
        UFC::UiniFile iniFile( ConfigFileName );
	UFC::Section* iniSection =iniFile.GetSection("Brokers" );        
        
	if( iniSection == NULL )
	{
	    UFC::BufferedLog::Printf( " Config file %s missing [Brokers] section.", ConfigFileName.c_str() );
	    exit( 0 );	    
	}
        UFC::BufferedLog::Printf( " [Brokers]" );
        for( int i = 0; i < iniSection->ItemCount(); i++ )
        {            
            UFC::AnsiString Broker,QueueID;	    
	    unsigned int QID;

	    
            if( iniSection->GetNameValue( i, Broker, QueueID ) == TRUE )
            {
		sscanf( QueueID.c_str(), "%x", &QID );			
                UFC::BufferedLog::Printf( " %s        = [%x]", Broker.c_str(), QID );
		UFC::PQueue* NewQueue = new UFC::PQueue( FALSE );
		if( !NewQueue->Open( QID, TRUE ) )
		{
		    UFC::BufferedLog::Printf( " Open Response queue 0x%x failed.", QID );
		    exit( 0 );
		}        
                FBrokerTable.Add( Broker, NewQueue );
                MessageObj->AddListener( "FT.RESPONSE.TSE", Broker, this );        ///< Add State query Message listener to MessageObject.                
                MessageObj->AddListener( "FT.RESPONSE.OTC", Broker, this );        ///< Add State query Message listener to MessageObject.                                
            }            
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
BOOL TMegaFTConnection::SaveFile( MTree* Data, const UFC::AnsiString& FileCode, UFC::AnsiString& FileName )
{
    Int8* Buffer;
    Int32  Size;
    
    FileName.Printf( "%s%s.%u.%u", FInbox.c_str(), FileCode.c_str(), getpid(), UFC::GetTickCountMS() );
    if( Data->get( "Data", Buffer, Size ) == TRUE )
    {
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
void TMegaFTConnection::ReadConfigFile( const UFC::AnsiString& ConfigFileName )
{
    try
    {
        UFC::UiniFile iniFile( ConfigFileName );
        UFC::AnsiString Value,BCCode;

        iniFile.GetValue( "main", "debug", FDebug );
        iniFile.GetValue( "main", "log_path", FLogFile );
        iniFile.GetValue( "main", "inbox_path", FInbox );
        iniFile.GetValue( "main", "Broadcast", BCCode );
	if( iniFile.GetValue( "main", "RequestQueue", Value )== FALSE )
	{
	    UFC::BufferedLog::Printf(" RequestQueue not set.");
	    exit(1);
	} 
	key_t QID;
	sscanf( Value.c_str(), "%x", &QID );	
	FReqQueueID = QID;
        
        if( iniFile.GetValue( "TAIFEX", "ResponseQueue", Value )== FALSE )
	{
	    UFC::BufferedLog::Printf(" TAIFEX ResponseQueue not set.");
	    exit(1);
	} 
        sscanf( Value.c_str(), "%x", &QID );	
	FRespQueueID = QID;
        
        
        
        
        UFC::PStringList Strings;
        
        Strings.SetStrings( BCCode, ",\n" );
        if( Strings.ItemCount() > 0 )
        {
            for( int i = 0; i < Strings.ItemCount(); i++ ) 
                 FBroadcastSet.Add( Strings[i] );
        }
        
        iniFile.GetValue( "MBus", "ip", FIP );
        iniFile.GetValue( "MBus", "port", Value );
        iniFile.GetValue( "MBus", "Hostname", FHostname );
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
        iniFile.Save();                
        
        UFC::BufferedLog::Printf( " [main]" );
        UFC::BufferedLog::Printf( " debug              = [%s]", FDebug.c_str() );
        UFC::BufferedLog::Printf( " log_path           = [%s]", FLogFile.c_str() );
        UFC::BufferedLog::Printf( " inbox_path         = [%s]", FInbox.c_str() );
	UFC::BufferedLog::Printf( " RequestQueue       = [%x]", FReqQueueID );
        UFC::BufferedLog::Printf( " " );        
        UFC::BufferedLog::Printf( " [TAIFEX]" );
        UFC::BufferedLog::Printf( " ResponseQueue      = [%x]", FRespQueueID );        
        UFC::BufferedLog::Printf( " " );        
        UFC::BufferedLog::Printf( " [MBus]" );
        UFC::BufferedLog::Printf( " ip                 = [%s]", FIP.c_str() );
        UFC::BufferedLog::Printf( " Hostname           = [%s]", FHostname.c_str() );
        UFC::BufferedLog::Printf( " port               = [%d]", FPort );        

        SetLogObject();        
    }
    catch( UFC::Exception& ex )
    {
        UFC::BufferedLog::DebugPrintf( " %s", ex.what() );
    }
}
//------------------------------------------------------------------------------
void TMegaFTConnection::Execute( void )
{ 
    while( !IsTerminated() )
    {
	UFC::UQueueData OrderData( 0, 1024 );		    	
	try 
        {   
	    OrderData.SetMType( 0 );
	    FReqQueue.Recv( OrderData );	    
            OnData( (char*)OrderData.GetData() );
        }
	catch( UFC::Exception& e )
        {
            UFC::BufferedLog::Printf( " FReqQueue.Recv Exception:%s", e.what() );
            UFC::BufferedLog::Printf( " MType:%d Len:%d", OrderData.GetMType(), OrderData.GetSize() );
        }	
        UFC::SleepMS( 50 );        
    }
}
//------------------------------------------------------------------------------
void TMegaFTConnection::ToQueue(  const UFC::AnsiString& BrokerID,  const UFC::AnsiString& ReplyMessage )
{
    UFC::PQueue*    ReplyQueue; 
    UFC::UQueueData QData( 1, (char*)ReplyMessage.c_str(), ReplyMessage.Length() + 1 );
    
    if( BrokerID == "0000" )///< To all brokers.
    {
        ReplyQueue = FBrokerTable.First();
        while( ReplyQueue != NULL )
        {
            ReplyQueue->Send( QData );
            ReplyQueue = FBrokerTable.Next();
        };
    }
    else
    {
        if( (ReplyQueue = FBrokerTable.GetObjectByKey( BrokerID )) != NULL )
             ReplyQueue->Send( QData );
    }    
}
//------------------------------------------------------------------------------
//             FT Response Format
//
//   1-  8  X(8)   TDATE      YYYYMMDD   
//   9- 16  9(8)   TTIME      HHMMSSmm
//  17- 20  X(4)   BKNO       7001 BrokerID
//  21- 24  X(4)   SBKNO      7001 Which line handle this request.
//  25- 25  X(1)   MTYPE      S: Message F: Upload file
//  26- 28  X(3)   FCODE      T30 File Code
//  29-540  X(512) FDATA      For Message: Full exchange message,include header. For File: full file  path name.
// 541-541  X(1)   MKT        T:TWSE O:OTC
// 542-543  9(2)   STATUS     00: OK  
// 544-544  X(1)   TFLAFG     Y: Backend processed. N: Not processed
// 545-554  9(10)  INSQ       Sequence
//
// Total: 543
//------------------------------------------------------------------------------
void TMegaFTConnection::TWSEFTResponse( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    UFC::AnsiString BrokerID, RequestID, FileCode,TimeNow,DateNow,ReplyMessage,RespData;
    Int32           Status, IsFile,Size;
    char            Market,MsgType;    
    char* MsgData;            
    
    if( Data->get( "Sender",   BrokerID ) &&
        Data->get( "FileCode", FileCode ) &&        
        Data->get( "IsFile",   IsFile ) && 
        Data->get( "Status",   Status ) )
    {
	Data->get( "ReqID", RequestID );
	RequestID.PadThis( 10,'0', FALSE );
	UFC::GetYYYYMMDD( DateNow, FALSE ); ///< 20120810
        GetTimeHHMMSSmm( TimeNow );             ///< 12595999      
        if( Subject == "FT.RESPONSE.TSE" )
            Market = 'T';
        else
            Market = 'O';        	
        if( IsFile == TRUE )
        {	    
            if( SaveFile( Data, FileCode, RespData ) == FALSE )
            {
                UFC::BufferedLog::Printf(" Save downloaded file [%s]failed.",RespData.c_str() );
                return;
            }
	    MsgType = 'F';	    
        }
        else
        {
            
            if( Data->get( "Data", MsgData, Size ) )
            {
                RespData = UFC::AnsiString( MsgData, Size );
        	UFC::BufferedLog::Printf(" Received message[%s] size[%d].", MsgData, Size );
            }
	    MsgType = 'S';	    
        }
	RespData.PadThis( 512, ' ', TRUE );	        
	ReplyMessage.Printf( "%s%s%s%s%c%s%s%c%02dN%s", 
                             DateNow.c_str(),  TimeNow.c_str(),
			     BrokerID.c_str(), BrokerID.c_str(), 	
			     MsgType, FileCode.c_str(), RespData.c_str(),
		             Market, Status, RequestID.c_str());
	ReplyMessage.PadThis( 543, ' ', TRUE );
	UFC::BufferedLog::Printf( " FT Reply[%s]", ReplyMessage.c_str() );
        if( FBroadcastSet.Exists( FileCode ) == TRUE )
            ToQueue( "0000", ReplyMessage );
        else
            ToQueue( Key, ReplyMessage );
    }    
}
//------------------------------------------------------------------------------
void TMegaFTConnection::FLEXResponse( char Market, const UFC::AnsiString& Subject, MTree* Data )
{
    Int32           ReqID,ErrorCode;    
    UFC::AnsiString ReplyMessage,time,Type;
    
    UFC::BufferedLog::Printf(" ------- FLEX Product Response -------" );        
    if( Data->get( "SecurityReqID", ReqID ) &&        
        Data->get( "status_code", ErrorCode ) )
    {        
        Data->get( "SecurityRequestType",Type );
        UFC::GetHHMMSSmmm(time);
        
        if( Type == "3" )
            ReplyMessage.Printf("%s%cNR3%010u%05d    ", time.c_str(), Market, (UInt32)ReqID, ErrorCode );
        else
            ReplyMessage.Printf("%s%cNPR%010u%05d    ", time.c_str(), Market, (UInt32)ReqID, ErrorCode );
     
        UFC::BufferedLog::Printf(" Time:[%s]", time.c_str());	        
        UFC::BufferedLog::Printf(" Market:[%c]", Market );	
        UFC::BufferedLog::Printf(" RequestID:[%u]", (UInt32)ReqID);	
        UFC::BufferedLog::Printf(" status code:[%d]", ErrorCode);	
        
        UFC::UQueueData QData( 1, (char*)ReplyMessage.c_str(), ReplyMessage.Length() );
        FRespQueue.Send( QData );
    }    
    UFC::BufferedLog::Printf(" -------------------------------------" );            
}
//------------------------------------------------------------------------------
void TMegaFTConnection::TAIFEXFTResponse( char Market, const UFC::AnsiString& Subject, MTree* Data )
{
    UFC::AnsiString FileCode,TimeNow,Src,FileName,ErrMsg,Filler;
    Int32           ReqID,ErrorCode,IsOffHour = 0,IsFLEX=0;
    UFC::AnsiString ReplyMessage;
    UFC::UDateTime  Today;
    
    
    if( Data->get( "Src", Src ) &&
        Data->get( "ReqID", ReqID ) &&        
        Data->get( "FileCode", FileCode ) && 
        Data->get( "ErrorCode", ErrorCode ) )
    {
        Data->get( "TradeKind", IsOffHour );
        Data->get( "FLEX", IsFLEX );
	UFC::GetHHMMSSmmm( TimeNow );  
        FileCode.PadThis( 4, ' ' );
	if( Src == "TMP" ) ///< Reject by TAIFEX
	{
	    if( ErrorCode == 0 )
		return;    
            FileName.PadThis( 128, ' ' );
            ErrMsg.PadThis( 128, ' ' );
            Filler.PadThis( 93, ' ' );
            ReplyMessage.Printf("%s%cFTR%d%d%010u%s%05d%s%s%s", TimeNow.c_str(), Market, IsOffHour,IsFLEX, (UInt32)ReqID, FileCode.c_str(), ErrorCode,FileName.c_str(),ErrMsg.c_str(),Filler.c_str() );
	}
	else if( Src == "FTP"  ) ///< Download by SpeedyFTDownload
	{
            Data->get( "FileName", FileName );
            Data->get( "ErrMsg", ErrMsg );
            FileName.PadThis( 128, ' ' );
            ErrMsg.PadThis( 128, ' ' );
            Filler.PadThis( 93, ' ' );
            ReplyMessage.Printf("%s%cFTR%d%d%010u%s%05d%s%s%s", TimeNow.c_str(), Market, IsOffHour,IsFLEX, (UInt32)ReqID, FileCode.c_str(), ErrorCode,FileName.c_str(),ErrMsg.c_str(),Filler.c_str() );    
	}    
        UFC::UQueueData QData( 1, (char*)ReplyMessage.c_str(), ReplyMessage.Length() );
        
        FRespQueue.Send( QData );
    }    
}
//------------------------------------------------------------------------------    
void TMegaFTConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{
    if( Subject == "FLEX.RESPONSE.FUT" ) 
        FLEXResponse( 'F' , Subject, Data );
    else if(Subject == "FLEX.RESPONSE.OPT" )
        FLEXResponse( 'O' , Subject, Data );
    else if( Subject == "SPEEDY.FTP.FUT" )
        TAIFEXFTResponse( 'F', Subject, Data );
    else if( Subject == "SPEEDY.FTP.OPT" )
        TAIFEXFTResponse( 'O', Subject, Data );
    else
        TWSEFTResponse( Subject, Key, Data );
}
//------------------------------------------------------------------------------
//             FT Request Format
//
//   1-  8  X(8)   TDATE      YYYYMMDD   
//   9- 16  9(8)   TTIME      HHMMSSmm
//  17- 20  X(4)   BKNO       7001 BrokerID
//  21- 24  X(4)   SBKNO      7001 Which line handle this request.
//  25- 25  X(1)   MTYPE      S: Message F: Upload file
//  26- 28  X(3)   FCODE      T30 File Code
//  29-540  X(512) FDATA      For Message: Full exchange message,include header. For File: full file  path name.
// 541-541  X(1)   MKT        T:TWSE O:OTC
// 542-551  9(10)  INSQ       Sequence
// 552-640  X989)  FILLER
//
// Total: 640
//------------------------------------------------------------------------------
void TMegaFTConnection::OnTWSEOTCData( char* Buffer )
{    
    UFC::AnsiString BrokerID( Buffer + 16, 4 ) ;
    //UFC::AnsiString LineBrokerID( Buffer + 20, 4 );
    UFC::AnsiString FileCode( Buffer + 25, 3 );
    UFC::AnsiString ReqData( Buffer + 28, 512 );
    char            Type = Buffer[ 25 ];
    char            Market = Buffer[ 540 ];
    UFC::AnsiString RequestID( Buffer + 541, 10 );
        
    ReqData.TrimRight();
    if( Type == 'F' || Type == 'f'  ) 
        FileUpload( BrokerID,  FileCode, RequestID, ReqData, Market );
    else
        FileRequest(  BrokerID,  FileCode, RequestID, ReqData, Market );    
}
//------------------------------------------------------------------------------
//            TAIFEX FT Request Format
//
//    1- 9   9(9) TTIME      Format:HHMMSSmmm
//  10- 10   X(1) Market     F:for Futures O: for Options
//   11-13   X(3) Function   “FTQ” 
//   14-14   9(1) Off-Hour    0 : Normal 1 : Off-Hour
//  15- 15   9(1) FLEX        0: TMP 1:FLEX FT request
//   16-25  9(10) INSQ        Unique Sequence generate by sender
//  26-483 X(458) DATA        File request TAIFEX Data field.
// 484-512  X(29) FILLER      Filler
//
// Total: 512
//
//------------------------------------------------------------------------------
void TMegaFTConnection::OnTAIFEXFTData( char* Buffer )
{
    UFC::AnsiString  Market( Buffer+9, 1 );
    UFCType::Int32   IsOffhour = UFC::AnsiString(Buffer+13, 1 ).ToInt();
    UFCType::Int32   IsFLEX = UFC::AnsiString(Buffer+14, 1 ).ToInt();
    UFCType::Int32   ReqID = UFC::AnsiString(Buffer+15, 10 ).ToInt();
    UFC::AnsiString  Body(Buffer+25, 458 );
    MTree Data;
    
    Body.TrimRight();    
    UFC::BufferedLog::Printf(" ---------- FT Request ----------" );        
    UFC::BufferedLog::Printf(" RequestID:[%u]", (UInt32)ReqID );	
    UFC::BufferedLog::Printf(" IsOffhour:[%d]", IsOffhour );	
    UFC::BufferedLog::Printf(" IsFLEX:[%d]",      IsFLEX );	
    UFC::BufferedLog::Printf(" Data:[%s]", Body.c_str() );	        
        
    Data.append( "FLEX",  IsFLEX );         
    Data.append( "TradeKind", IsOffhour );                 
    Data.append( "ReqID", ReqID );         
    if( Body.Length() > 0 )
        Data.append( "Body", Body );
    if( Market == "F" || Market == "f" ) ///< Futures 
        MessageObj->Send( "SPEEDY.FTP.FUT", FHostname, Data, FALSE );
    else ///< Options
        MessageObj->Send( "SPEEDY.FTP.OPT", FHostname, Data, FALSE );
    UFC::BufferedLog::Printf(" --------------------------------" );    
}
//------------------------------------------------------------------------------
//            TAIFEX New FLEX product Request Format
//
//    1- 9 9(9)  TTIME           Format:HHMMSSmmm
//   10-10 X(1)  Market          F:for Futures O: for Options
//   11-13 X(3)  Function        “NPQ” New FLEX product Request
//   14-23 9(10) SecurityReqID   Unique Sequence generate by sender
//   24-26 9(3)  Root_symbol     Product root symbol.(example:TXO,MXF)
//   27-33 X(7)  BrokerID        Broker ID.(example F001000)
//   34-40 9(7) investor_acno    Account
//   41-41 9(1) application_type Applicate by 1:account 2:prop trade 3:FCM
//   42-42 X(1) expiry_type      Type X:Euro cash Settlement
//   43-50 9(8) contract_date    Date (yyyymmdd)
//   51-60 9(10) strike_price    Strike price. ( Futures filled space. )
//   61-61 X(1) call_put_code    C:CALL，P:PUT. ( Futures space. )
//   62-64 X(3) FILLER Filler
//   
//   Total: 64
//   
//------------------------------------------------------------------------------
void TMegaFTConnection::OnTAIFEXFLEXData( bool IsNew, char* Buffer )
{
    UFC::AnsiString  Market( Buffer+9, 1 );
    UFCType::Int32   ReqID = UFC::AnsiString(Buffer+13, 10 ).ToInt();
    UFC::AnsiString  ROOTSYM( Buffer+23, 3 );
    UFC::AnsiString  FCMID( Buffer+26, 7 );
    UFC::AnsiString  ACNO( Buffer+33, 7 );
    UFC::AnsiString  APPTYPE( Buffer+40, 1 );
    UFC::AnsiString  CDDATE( Buffer+42, 8 );
    UFC::AnsiString  STRIKEPX( Buffer+50, 10 );
    UFC::AnsiString  CP( Buffer+60, 1 );
    MTree Data;
        
    if( IsNew == true )
        Data.append( "SecurityRequestType", "1" );
    else
        Data.append( "SecurityRequestType", "3" );
    Data.append( "SecurityReqID", ReqID );         
    Data.append( "type", Market );         
    Data.append( "root_symbol", ROOTSYM );         
    Data.append( "BrokerID", FCMID );
    Data.append( "investor_acno", ACNO.ToInt() );
    Data.append( "application_type", APPTYPE );
    Data.append( "contract_date", CDDATE );
        
    UFC::BufferedLog::Printf(" ------- FLEX Product Request -------" );        
    UFC::BufferedLog::Printf(" RequestID:[%u]", (UInt32)ReqID);	
    UFC::BufferedLog::Printf(" type:[%s]", Market.c_str() );	
    UFC::BufferedLog::Printf(" root_symbol:[%s]", ROOTSYM.c_str() );	
    UFC::BufferedLog::Printf(" BrokerID:[%s]", FCMID.c_str() );	
    UFC::BufferedLog::Printf(" investor_acno:[%d]", ACNO.ToInt());	
    UFC::BufferedLog::Printf(" application_type:[%s]", APPTYPE.c_str());	
    UFC::BufferedLog::Printf(" contract_date:[%s]", CDDATE.c_str() );
    if( Market == "O" || Market == "o" )
    {
        UFC::BufferedLog::Printf(" strike_price:[%d]", STRIKEPX.ToInt() );
        UFC::BufferedLog::Printf(" call_put_code:[%s]", CP.c_str() );
        Data.append( "strike_price", STRIKEPX.ToInt() );
        Data.append( "call_put_code", CP );
        MessageObj->Send( "FLEX.REQUEST.OPT", FHostname, Data, FALSE );
    }
    else
        MessageObj->Send( "FLEX.REQUEST.FUT", FHostname, Data, FALSE );
    UFC::BufferedLog::Printf(" ------------------------------------" );    
}
//------------------------------------------------------------------------------
//  Received data from Queue
//------------------------------------------------------------------------------
void TMegaFTConnection::OnData( char* Buffer )
{    
    UFC::AnsiString Function( Buffer + 10, 3 ) ;
    
    if( Function == "FTQ" )      ///< TAIFEX File request. (R13/RX13)
        OnTAIFEXFTData( Buffer );
    else if( Function == "NPQ" ) ///< TAIFEX FLEX new product request. (RX19)
        OnTAIFEXFLEXData( true, Buffer );
    else if( Function == "NP3" ) ///< TAIFEX FLEX new product query request. (RX19)
        OnTAIFEXFLEXData( false, Buffer );
    else
        OnTWSEOTCData( Buffer ); ///< TWSE/OTC file request
}
//------------------------------------------------------------------------------
void TMegaFTConnection::FileRequest(const UFC::AnsiString&  BrokerID,
				    const UFC::AnsiString&  FileCode,
				    const UFC::AnsiString&  RequestID,
				    const UFC::AnsiString&  ReqData,
				    char Market )
{
        MTree Data;
                
        Data.append( "Sender", BrokerID );
        Data.append( "FileCode", FileCode );        
        Data.append( "ReqID", RequestID );                 
        Data.append( "IsFile", 0 );                
        UFC::BufferedLog::Printf(" Sender:[%s]", BrokerID.c_str() );	
        UFC::BufferedLog::Printf(" FileCode:[%s]", FileCode.c_str() );	
	UFC::BufferedLog::Printf(" RequestID:[%s]", RequestID.c_str() );
	if( ReqData.Length() > 0 )
	    Data.append( "Data", ReqData );
        if( Market == 'T' || Market == 't' )
            MessageObj->Send( "FT.REQUEST.TSE", FHostname, Data, FALSE );
        else
            MessageObj->Send( "FT.REQUEST.OTC", FHostname, Data, FALSE );    
}
//------------------------------------------------------------------------------
void TMegaFTConnection::FileUpload( const UFC::AnsiString&  BrokerID,
				    const UFC::AnsiString&  FileCode,
				    const UFC::AnsiString&  RequestID,
				    const UFC::AnsiString&  FileName,
				    char Market )
{
    MTree Data;

    Data.append( "Sender",   BrokerID );
    Data.append( "FileCode", FileCode );        
    Data.append( "ReqID",    RequestID );         
    Data.append( "IsFile",  1 );                        
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
	    UFC::BufferedLog::Printf(" Upload file \"%s\" is an empty file.", FileName.c_str());
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
	UFC::BufferedLog::Printf(" Upload file \"%s\" not found", FileName.c_str());
	return;
    }                
    if( Market == 'T' || Market == 't' )
	MessageObj->Send( "FT.REQUEST.TSE", FHostname, Data, FALSE );
    else
	MessageObj->Send( "FT.REQUEST.OTC", FHostname, Data, FALSE );
}
//------------------------------------------------------------------------------
