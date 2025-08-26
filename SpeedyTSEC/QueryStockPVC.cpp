//--------------------------------------------------------------------------------------------------------------------
#include "QueryStockPVC.h"
#include "SessionSettings.h" ///< for FIX
#include "iniFile.h"

const int   TWSE_HEARTBT_INT = 10;
extern BOOL FIXUseDictionary;
//--------------------------------------------------------------------------------------------------------------------
//        |  Port,         PVC      APCode   BrokerID  Password  PVCID   IP           LocalPort   Group   Enable  Throughput
//--------------------------------------------------------------------------------------------------------------------
// X25    |  X25 Link      PVC No   0:Order  7000      9999      E1      -----         ---        ---     ---      ---
// TCP/IP |  server port   ---      0:Order  7000      9999      E1      192.168.1.1  29001       1       1        --- 
// FIX    |  server port   99       0:Order  7000      9999      E1      192.168.1.1  29001       2       1        160    
//        | 
//--------------------------------------------------------------------------------------------------------------------
//        For FIX       
//--------------------------------------------------------------------------------------------------------------------
//
//        SenderCompID(49): T/O + BrokerID + PVCID   Example: T7000E1
//        SenderSubID(50):  BrokerID                 Example: 7001
//        TargetCompID(56): XTAI/ROCO                Example: XTAI (TWSE)  
//        TargetSubID(57):  0                        Example: 0
//
//--------------------------------------------------------------------------------------------------------------------
//0         1         2         3         4         5
//012345678901234567890123456789012345678901234567890
//2007,3,3,7000,8888,13,192.168.0.22,29003,0,1
//2007,5,0,7000,8888,15,192.168.0.22,29005,0,1
//2007,6,0,7000,8888,16,192.168.0.22,29006,0,1
BOOL TSECLines::ParseLine( char* Line, char APCode, int Index )
{
    if( Index >= MAX_LINES - 1 )
        return FALSE;
    if( strlen( Line ) <  13 )
        return FALSE;
    if( strstr( Line, "#") != NULL )
        return FALSE;
    char  Seps[] = ",\n";
    char* Port      = strtok( Line, Seps ); ///< Server listen port.
    char* PVC       = strtok( NULL, Seps ); ///< 99 means FIX connection.
    char* APC       = strtok( NULL, Seps ); ///< AP Code. 0:Order 3:Fill ...
    char* Broker    = strtok( NULL, Seps ); ///< BrokerID.
    char* Pwd       = strtok( NULL, Seps ); ///< Password 4 digi. 
    char* PVCID     = strtok( NULL, Seps ); ///< PVC/Session ID
    char* IP        = strtok( NULL, Seps ); ///< Server IP
    char* LocalPort = strtok( NULL, Seps ); ///< Local TCP port.
    char* Group     = strtok( NULL, Seps ); ///< PVC group.
    char* Active    = strtok( NULL, Seps ); ///< Default active.
    char* Throughput = strtok( NULL, Seps ); ///< FIX throughput.


    if( Port!= NULL && PVC != NULL &&  APC != NULL && 
        Broker != NULL && Pwd != NULL && PVCID!= NULL && APCode == *APC )
    {
        PORT_NOs[ Index ]   = UFC::AnsiString( Port  , PVC-Port ).ToInt();
        PVC_NOs[ Index ]    = UFC::AnsiString( PVC   , APC-PVC ).ToInt();
        BROKER_IDs[ Index ] = UFC::AnsiString( Broker, Pwd-Broker );
        PASSWORDs[ Index ]  = UFC::AnsiString( Pwd   , PVCID-Pwd );
        GROUP[ Index ]      = 0;    ///< Default Group 0.
        ACTIVE[ Index ]     = TRUE; ///< Default active.               
        THROUGHPUT[ Index ] = 20;   ///< Default throughput 20 order/sec.               
        
        if( IP == NULL ) /// X.25 session
        {
            PVC_IDs[ Index ]        = UFC::AnsiString( PVCID );
            PEER_IPs[ Index ]       = "NULL";
            LOCAL_PORT_NOs[ Index ] = ANY_LOCAL_PORT;
        }
        else ///< TCP/IP,FIX session
        {
            PVC_IDs[ Index ]        = UFC::AnsiString( PVCID , IP - PVCID );
            if( LocalPort != NULL )
            {
                PEER_IPs[ Index ]  = UFC::AnsiString( IP, LocalPort-IP );
                if( Group != NULL )
                {
                    LOCAL_PORT_NOs[ Index ] = UFC::AnsiString( LocalPort, Group - LocalPort ).ToInt();
                    if( Active != NULL )
                    {
                        GROUP[ Index ] = UFC::AnsiString( Group, Active - Group ).ToInt();
                        if( Throughput != NULL )
                        {
                            ACTIVE[ Index ] = UFC::AnsiString( Active, Throughput - Active ).ToInt(); 
                            THROUGHPUT[ Index ] = UFC::AnsiString( Throughput ).ToInt(); 
                        }
                        else
                            ACTIVE[ Index ] = UFC::AnsiString( Active ).ToInt(); 
                    }
                    else
                        GROUP[ Index ] = UFC::AnsiString( Group ).ToInt();
                }
                else
                    LOCAL_PORT_NOs[ Index ] = UFC::AnsiString( LocalPort ).ToInt();
            }
            else
            {
                PEER_IPs[ Index ]  = UFC::AnsiString( IP );
                LOCAL_PORT_NOs[ Index ] = ANY_LOCAL_PORT;
            }
        }
        if( PVC_NOs[ Index ] == 99 )
            IsFIX[ Index ] = TRUE;
        else
            IsFIX[ Index ] = FALSE;
        return TRUE;
    }
    return FALSE;
}
//--------------------------------------------------------------------------------------------------------------------
BOOL TSECLines::ParseFIXLine( char* Line, int Index )
{
    if( Index >= MAX_LINES - 1 )
        return FALSE;
    if( strlen( Line ) <  13 )
        return FALSE;
    if( strstr( Line, "#") != NULL )
        return FALSE;
    char  Seps[] = ",\n";
    char* Port       = strtok( Line, Seps ); ///< Server listen port.
    char* PVC        = strtok( NULL, Seps ); ///< 99 means FIX connection.
    char* APC        = strtok( NULL, Seps ); ///< AP Code. 0:Order 3:Fill ...
    char* Broker     = strtok( NULL, Seps ); ///< BrokerID.
    char* Pwd        = strtok( NULL, Seps ); ///< Password 4 digi. 
    char* PVCID      = strtok( NULL, Seps ); ///< PVC/Session ID
    char* IP         = strtok( NULL, Seps ); ///< Server IP
    char* LocalPort  = strtok( NULL, Seps ); ///< Local TCP port.
    char* Group      = strtok( NULL, Seps ); ///< PVC group.
    char* Active     = strtok( NULL, Seps ); ///< Default active.
    char* Throughput = strtok( NULL, Seps ); ///< FIX throughput.


    if( Port!= NULL    && PVC != NULL &&  APC != NULL && 
        Broker != NULL && Pwd != NULL && PVCID!= NULL  )
    {
        PORT_NOs[ Index ]   = UFC::AnsiString( Port  , PVC-Port ).ToInt();
        PVC_NOs[ Index ]    = UFC::AnsiString( PVC   , APC-PVC ).ToInt();
        BROKER_IDs[ Index ] = UFC::AnsiString( Broker, Pwd-Broker );
        PASSWORDs[ Index ]  = UFC::AnsiString( Pwd   , PVCID-Pwd );
        GROUP[ Index ]      = 0;    ///< Default Group 0.
        ACTIVE[ Index ]     = TRUE; ///< Default active.               
        THROUGHPUT[ Index ] = 20;   ///< Default throughput 20 order/sec.               
        
        if( PVC_NOs[ Index ] == 99 )///< Is FIX Session
        {
            IsFIX[ Index ] = TRUE;                
            if( IP != NULL ) ///< TCP/IP,FIX session
            {
                PVC_IDs[ Index ]        = UFC::AnsiString( PVCID , IP - PVCID );
                if( LocalPort != NULL )
                {
                    PEER_IPs[ Index ]  = UFC::AnsiString( IP, LocalPort-IP );
                    if( Group != NULL )
                    {
                        LOCAL_PORT_NOs[ Index ] = UFC::AnsiString( LocalPort, Group - LocalPort ).ToInt();
                        if( Active != NULL )
                        {
                            GROUP[ Index ] = UFC::AnsiString( Group, Active - Group ).ToInt();
                            if( Throughput != NULL )
                            {
                                ACTIVE[ Index ] = UFC::AnsiString( Active, Throughput - Active ).ToInt(); 
                                THROUGHPUT[ Index ] = UFC::AnsiString( Throughput ).ToInt(); 
                            }
                            else
                                ACTIVE[ Index ] = UFC::AnsiString( Active ).ToInt(); 
                        }
                        else
                            GROUP[ Index ] = UFC::AnsiString( Group ).ToInt();
                    }
                    else
                        LOCAL_PORT_NOs[ Index ] = UFC::AnsiString( LocalPort ).ToInt();
                }
                else
                {
                    PEER_IPs[ Index ]  = UFC::AnsiString( IP );
                    LOCAL_PORT_NOs[ Index ] = ANY_LOCAL_PORT;
                }
                return TRUE;
            }        
            return FALSE; ///< X.25 session            
        }
        return FALSE; ///< Not FIX Session
    }
    return FALSE; ///< Bad config
}
//--------------------------------------------------------------------------------------------------------------------
TSECLines::TSECLines( BOOL IsTWSE)
:FIsTWSE( IsTWSE )
{
    if( FIsTWSE==TRUE )    
    {
        FTMPCfgFileName    = "../cfg/SpeedyPVCTSE.cfg";
        FFIXCfgFileName    = "../cfg/SpeedyFIXTSE.cfg";
        FMarketCfgFile     = "../cfg/SpeedyTSEC.cfg";
        FExCode            = 'T';
        FTargetCompID      = "XTAI";
    }
    else    
    {
        FTMPCfgFileName    = "../cfg/SpeedyPVCOTC.cfg";    
        FFIXCfgFileName    = "../cfg/SpeedyFIXOTC.cfg";            
        FMarketCfgFile     = "../cfg/SpeedyOTC.cfg";
        FExCode            = 'O';
        FTargetCompID      = "ROCO";        
    }
}
//--------------------------------------------------------------------------------------------------------------------
//
// Get PVC setting for given AP code.
//
//--------------------------------------------------------------------------------------------------------------------
int TSECLines::GetLines( char AP )
{
    int   FetchCount = 0;
    char  Buffer[ 512 ];
    FILE* FFileHandle;
    
    if( (FFileHandle = fopen( FTMPCfgFileName.c_str(), "r" )) == NULL )
    {
        printf("Can't open PVC configuration file \"%s\".", FTMPCfgFileName.c_str() );
        exit( 0 );
    }
    while( fgets( Buffer, 512, FFileHandle ) != NULL )
    {         
        if( ParseLine( Buffer, AP, FetchCount ) == TRUE )
            FetchCount++;
    }
    fclose( FFileHandle );
    return FetchCount;
}
//--------------------------------------------------------------------------------------------------------------------
int TSECLines::GetFIXLines( void )
{
    int   FetchCount = 0;
    char  Buffer[ 512 ];
    FILE* FFileHandle;
    
    if( (FFileHandle = fopen( FTMPCfgFileName.c_str(), "r" )) == NULL )
    {
        printf("Can't open PVC configuration file \"%s\".", FTMPCfgFileName.c_str() );
        exit( 0 );
    }
    while( fgets( Buffer, 512, FFileHandle ) != NULL )
    {         
        if( ParseFIXLine( Buffer, FetchCount ) == TRUE )
            FetchCount++;
    }
    fclose( FFileHandle );
    return FetchCount;
}
//--------------------------------------------------------------------------------------------------------------------
FIX::UtcTimeOnly ToUTC( int Time )
{
    int h = Time/10000;
    int m = (Time%10000)/100;
    int s = Time%100;
    int Sdiff = -8*60*60;
    FIX::UtcTimeOnly  UTC( h, m, s );     
    UTC += Sdiff; 
    return UTC;
}
//--------------------------------------------------------------------------------------------------------------------
int TSECLines::SaveFIXConfig( void )
{
    
    UFC::AnsiString   StartTime( "073030" ); ///<  7:30:30
    UFC::AnsiString   StopTime( "133459" );  ///< 13:34:59  
    UFC::UiniFile     SessionINI( FFIXCfgFileName, TRUE );
    int               SessionCount  = GetFIXLines( );
    UFC::UiniFile     Cfg( FMarketCfgFile );    
    
    Cfg.GetValue( "FIX", "Start", StartTime );
    UFC::BufferedLog::Printf( " FIX Start at local time[%s]", StartTime.c_str() );                
    FIX::UtcTimeOnly  UTCStart = ToUTC( StartTime.ToInt() );
    StartTime.Printf( "%02d:%02d:%02d", UTCStart.getHour(),UTCStart.getMinute(), UTCStart.getSecond() );        
    
    Cfg.GetValue( "FIX", "Stop", StopTime );
    UFC::BufferedLog::Printf( " FIX Stop  at local time[%s]",StopTime.c_str());     
    FIX::UtcTimeOnly  UTCStop = ToUTC( StopTime.ToInt() );
    StopTime.Printf( "%02d:%02d:%02d", UTCStop.getHour(),UTCStop.getMinute(), UTCStop.getSecond() );        
    
    UFC::BufferedLog::Printf( " FIX Start at UTC time[%s]", StartTime.c_str() );        
    UFC::BufferedLog::Printf( " FIX Stop  at UTC time[%s]", StopTime.c_str() );        
    SessionINI.Clear();
    SessionINI.SetValue("DEFAULT",FIX::FILE_STORE_PATH,          "../FIXStore" );
    SessionINI.SetValue("DEFAULT",FIX::FILE_LOG_PATH,            "../FIXLog");
    SessionINI.SetValue("DEFAULT",FIX::CHECK_LATENCY,            "N");
    SessionINI.SetValue("DEFAULT",FIX::SOCKET_NODELAY,           "Y");
    SessionINI.SetValue("DEFAULT",FIX::SOCKET_REUSE_ADDRESS,     "Y");
    SessionINI.SetValue("DEFAULT",FIX::MILLISECONDS_IN_TIMESTAMP,"Y" );    
    SessionINI.SetValue("DEFAULT",FIX::HEARTBTINT,               TWSE_HEARTBT_INT );
   

    if( FIXUseDictionary == TRUE )
    {
        SessionINI.SetValue("DEFAULT",FIX::USE_DATA_DICTIONARY,      "Y" );
        SessionINI.SetValue("DEFAULT",FIX::DATA_DICTIONARY,          "../FIXSPEC/FIX44_TWSE.xml" );
    }
    else
        SessionINI.SetValue("DEFAULT",FIX::USE_DATA_DICTIONARY,      "N" );
    UFC::BufferedLog::Printf( " -------------------- Create FIX connections --------------------" );
    if( SessionCount > 0 )
    {
        for( int i = 0; i < SessionCount ; i++ )
        {
            UFC::AnsiString SenderCompID;
            UFC::Section* NewSection = new UFC::Section("SESSION");

            SenderCompID.Printf( "%c%s%s",FExCode, GetBrokerID( i ).c_str(), GetPVCID( i ).c_str() );
            NewSection->SetValue( FIX::CONNECTION_TYPE,     "initiator" );
            NewSection->SetValue( FIX::BEGINSTRING,         "FIX.4.4" );
            NewSection->SetValue( FIX::RECONNECT_INTERVAL,  "10" );
            NewSection->SetValue( FIX::LOGON_TIMEOUT,       "5" );
            NewSection->SetValue( FIX::SENDERCOMPID,        SenderCompID );
            NewSection->SetValue( FIX::TARGETCOMPID,        FTargetCompID );
            NewSection->SetValue( FIX::START_TIME,          StartTime );
            NewSection->SetValue( FIX::END_TIME,            StopTime );
            NewSection->SetValue( FIX::THROUGHPUT,          UFC::AnsiString( (Int32)GetThreughput( i ) ) );
            NewSection->SetValue( FIX::SOCKET_CONNECT_HOST, GetIP(i).c_str()); 
            NewSection->SetValue( FIX::SOCKET_CONNECT_PORT, GetPort(i) );
            NewSection->SetValue( FIX::SOCKET_LOCAL_PORT,   GetLocalPort(i));
            NewSection->SetValue( FIX::PASSWORD,            GetPassword(i).c_str() );
            NewSection->SetValue( FIX::MILLISECONDS_IN_TIMESTAMP, "Y" );
            UFC::BufferedLog::Printf( " Create FIX Session SenderCompID[%s] TargetCompID[%s] ", SenderCompID.c_str(), FTargetCompID.c_str() );
            UFC::BufferedLog::Printf( "                    Connect to[%s:%d] local port[%d]",GetIP(i).c_str(),GetPort(i), GetLocalPort(i) );
            UFC::BufferedLog::Printf( "                    HeartBtInt[%d sec] Throughput[%d Order/sec]",TWSE_HEARTBT_INT,GetThreughput( i ));
            SessionINI.AddSection( NewSection );
        }
    }
    UFC::BufferedLog::Printf( " --------------------------- Finished ---------------------------" );
    SessionINI.Save();  
    return SessionCount;
}
//--------------------------------------------------------------------------------------------------------------------




