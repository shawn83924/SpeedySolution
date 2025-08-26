// 
// File:   Main.cpp
// Author: yuan
//
// Created on November 2, 2008, 11:08 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "CaptialExecConnection.h"
#include "PClientSocket.h"
//------------------------------------------------------------------------------
const Int32     RECOVER_BEGIN_TIME   = 61530;
UFC::PInt32     FRecoverBeginTime;
UFC::UTime      FTime;
BOOL            DumpOnly             = FALSE;
BOOL            NoSocket             = FALSE;
BOOL            EnableFuturesAdapter = FALSE;
BOOL            EnableOptionsAdapter = FALSE;
BOOL            EnableServer         = FALSE;
BOOL            EnableB50Cancel      = FALSE;
Int32           SendFromFirst = 0;
UFC::AnsiString AppName;
ExecSessionType ExecSession = esAll;

MessageObject*  MessageObj;
UFC::AnsiString DisconnectTime1;
UFC::AnsiString DisconnectTime2;
UFC::AnsiString DisconnectTime3;
UFC::AnsiString DisconnectTime4;
Int32 HeartbeatTime1 = 0;
Int32 HeartbeatTime2 = 0;
Int32 HeartbeatTime3 = 0;
Int32 HeartbeatTime4 = 0;
Int32 HeartbeatTimeCount1 = 0;
Int32 HeartbeatTimeCount2 = 0;
Int32 HeartbeatTimeCount3 = 0;
Int32 HeartbeatTimeCount4 = 0;
UFC::MultiplexingServer* server = NULL;
//UFC::PtrList<UFC::PClientSocket> socketlist;

UFC::PClientSocket* client1 = NULL;
UFC::PClientSocket* client2 = NULL;
UFC::PClientSocket* client3 = NULL;
UFC::PClientSocket* client4 = NULL;
SocketMonitor* ConfirmMonitor;
SocketMonitor* FillMonitor;
SocketMonitor* ConfirmOPTMonitor;
SocketMonitor* FillOPTMonitor;
UFC::BufferedLog* ScreenLog;

UFC::AnsiString YECTime;
//----------------------------------------------------------------------------------------------------------------------
void HeartbeatFill(UFC::PClientSocket* client)
{
    Int8 data[FILL_SIZE + 1];
    memset(data,' ',FILL_SIZE);
    data[FILL_SIZE] = 0;
    memcpy(data,"18888888",8);
    
    UFC::BufferedLog::DebugPrintf(" HeartbeatFill begin");
    client->BlockSend(data,FILL_SIZE);
    client->BlockRecv((UInt8*)data,FILL_SIZE);
    UFC::BufferedLog::DebugPrintf(" HeartbeatFill send");// [%s]",data);
}
//----------------------------------------------------------------------------------------------------------------------
void HeartbeatConfirm(UFC::PClientSocket* client)
{
    Int8 data[CONFIRM_SIZE + 1];
    memset(data,' ',CONFIRM_SIZE);
    data[CONFIRM_SIZE] = 0;
    memcpy(data,"18888888",8);
    
    UFC::BufferedLog::DebugPrintf(" HeartbeatConfirm begin");
    client->BlockSend(data,CONFIRM_SIZE);
    client->BlockRecv((UInt8*)data,CONFIRM_SIZE);
    UFC::BufferedLog::DebugPrintf(" HeartbeatConfirm send");// [%s]",data);
}
//----------------------------------------------------------------------------------------------------------------------
void SendAlert(const UFC::AnsiString& Msg)
{
  MTree Data;
  Data.append( "COMMAND", 9020 );
  Data.append( "MSG", Msg );
  MessageObj->Send( "SPEEDY.COMMAND", UFC::Hostname, Data, FALSE );
}
//----------------------------------------------------------------------------------------------------------------------
void GetReconnectBeginTime( UFC::AnsiString& DisconnectTime )
{
    if( DisconnectTime.Length() != 6 )
    {
        UFC::UTime time;
        time.setCurrent();
        time -= 60;
        DisconnectTime.Printf( "%02d%02d%02d",time.getHour(),time.getMinute(),time.getSecond() );
    }
}
//------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        UFC::BufferedLog::Printf( " ***** Process CaptialExecConnection already exists *****"  );
        sleep( 2 );
        exit( 0 );
    }
}
//------------------------------------------------------------------------------
BOOL SocketMonitor::OnDataArrived( UFC::PClientSocket * Socket )
{
    return TRUE;
}
//------------------------------------------------------------------------------
void SocketMonitor::OnConnect( UFC::PClientSocket* Socket )
{
    UFC::AnsiString log;
    log.Printf(" Connection established [%s:%d]", Socket->GetPeerIPAddress().c_str(), Socket->GetPort());//Connection
    UFC::BufferedLog::Printf( log.c_str() );
    if( Socket == client1)
    {
        HeartbeatTimeCount1 = 0;
        HeartbeatConfirm(client1);

        log.Printf("Confirm%s", log.c_str());
        SendAlert(log);
    }
    else if( Socket == client2)
    {
        HeartbeatTimeCount2 = 0;
        HeartbeatFill(client2);

        log.Printf("Fill%s", log.c_str());
        SendAlert(log);
    }
    else if( Socket == client3)
    {
        HeartbeatTimeCount3 = 0;
        HeartbeatConfirm(client3);

        log.Printf("Confirm_OPT%s", log.c_str());
        SendAlert(log);
    }
    else if( Socket == client4)
    {
        HeartbeatTimeCount4 = 0;
        HeartbeatFill(client4);

        log.Printf("Fill_OPT%s", log.c_str());
        SendAlert(log);
    }
}
//------------------------------------------------------------------------------
void SocketMonitor::OnDisconnect( UFC::PClientSocket* Socket, BOOL NeedReconnect )
{
    UFC::AnsiString log;
    log.Printf(" Connection broken [%s:%d]", Socket->GetPeerIPAddress().c_str(), Socket->GetPort());//Connection
    UFC::BufferedLog::Printf( log.c_str() );
    if( Socket == client1)
    {
        HeartbeatTimeCount1 = 0;
        GetReconnectBeginTime(DisconnectTime1);
        log.Printf("Confirm%s", log.c_str());
        SendAlert(log);
    }
    else if( Socket == client2)
    {
        HeartbeatTimeCount2 = 0;
        GetReconnectBeginTime(DisconnectTime2);
        log.Printf("Fill%s", log.c_str());
        SendAlert(log);
    }
    else if( Socket == client3)
    {
        HeartbeatTimeCount3 = 0;
        GetReconnectBeginTime(DisconnectTime3);
        log.Printf("Confirm_OPT%s", log.c_str());
        SendAlert(log);
    }
    else if( Socket == client4)
    {
        HeartbeatTimeCount4 = 0;
        GetReconnectBeginTime(DisconnectTime4);
        log.Printf("Fill_OPT%s", log.c_str());
        SendAlert(log);
    }
}
//------------------------------------------------------------------------------
void ServerSocketMonitor::OnClientConnect( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket )
{
    //ClientSocketList.Add(ClientSocket);
    UFC::BufferedLog::Printf( " %s Connect :%d", ClientSocket->GetPeerIPAddress().c_str() , ServSocket->GetPort() );
}
//------------------------------------------------------------------------------
void ServerSocketMonitor::OnClientDisconnect( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket )
{
    //ClientSocketList.Remove(ClientSocket);
    UFC::BufferedLog::Printf( " %s Disconnect :%d", ClientSocket->GetPeerIPAddress().c_str() , ServSocket->GetPort() );
}
//------------------------------------------------------------------------------
BOOL ServerSocketMonitor::OnClientWrite( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket )
{
    UInt8 QueueData[ NOTICE_SIZE ];
    UFC::PLockObject   Lock( ClientSocket->ReadLock );
    ClientSocket->BlockRecv( QueueData, NOTICE_SIZE );
    Int8 CharData[ NOTICE_SIZE + 1 ];
    CharData[ NOTICE_SIZE ] = 0;
    memcpy( CharData, QueueData, NOTICE_SIZE);
    UFC::BufferedLog::Printf( " <Notice Flag> <%s>", CharData );
    Int8 reCharData[ 21 ];
    reCharData[ 20 ] = 0;
    memcpy( reCharData, QueueData, 20);
    ClientSocket->BlockSend( reCharData, 20 );
    UFC::BufferedLog::Printf( " <BlockSend Notice Flag> <%s>", reCharData );

    MTree Data;
    Data.append("Data", CharData );
    
    MessageObj->Send( "FromBackend", UFC::Hostname, Data );
    return TRUE;
}
//------------------------------------------------------------------------------
void ServerSocketMonitor::OnAcceptFailed( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket )
{
    UFC::BufferedLog::Printf( " ServerSocketMonitor::OnAcceptFailed() exit Program." );
    UFC::BufferedLog::FlushToFile();
    exit(0);
}  //ServerSocketMonitor::OnAcceptFailed();
//----------------------------------------------------------------------------------------------------------------------
void Send( int Type, const UFC::AnsiString& data )
{
    if ( ( Type == 1 ) && ( EnableFuturesAdapter == TRUE ) )
    {
        UFC::BufferedLog::Printf( " <SEND> Confirm" );
        if( client1 != NULL && client1->IsConnect() )
        {
            char Buffer[data.Length() + 1 ];
            memset( Buffer, '\0', data.Length() + 1 );
            try
            {
                client1->BlockSend( (Int8*)data.c_str(),data.Length());
                client1->BlockRecv( (UInt8*)Buffer, data.Length() );
                UFC::BufferedLog::DebugPrintf( " Recv:[%s]", Buffer );
                return;
            }
            catch(UFC::SocketException& ex)
            {
                UFC::BufferedLog::Printf( " Socket Exception:[%s]", ex.what() );
                client1->Disconnect( TRUE );
            }
            catch(UFC::Exception& ex)
            {
                UFC::BufferedLog::Printf( " Exception:[%s]", ex.what() );
                client1->Disconnect( TRUE );
            }
        }

        UFC::AnsiString TimeNow;
        UFC::GetHHMMSS(TimeNow);
        UFC::AnsiString logdata;
        UFC::BufferedLog::Printf( " Confirm send fail:[%s]", data.c_str() );
        logdata.Printf("Confirm time:%s DATA[%s]\n",TimeNow.c_str(),data.c_str());
        ScreenLog->WriteString( logdata );
    }
    else if( ( Type == 3 ) && ( EnableOptionsAdapter == TRUE ) )
    {
        UFC::BufferedLog::Printf( " <SEND> Confirm_OPT" );
        if( client3 != NULL &&  client3->IsConnect() )
        {
            char Buffer[data.Length() + 1 ];
            memset( Buffer, '\0', data.Length() + 1 );
            try
            {
                client3->BlockSend( (Int8*)data.c_str(),data.Length());
                client3->BlockRecv( (UInt8*)Buffer, data.Length() );
                UFC::BufferedLog::DebugPrintf( " Recv:[%s]", Buffer );
                return;
            }
            catch(UFC::SocketException& ex)
            {
                UFC::BufferedLog::Printf( " Socket Exception:[%s]", ex.what() );
                client3->Disconnect( TRUE );
            }
            catch(UFC::Exception& ex)
            {
                UFC::BufferedLog::Printf( " Exception:[%s]", ex.what() );
                client3->Disconnect( TRUE );
            }
        }

        UFC::AnsiString TimeNow;
        UFC::GetHHMMSS(TimeNow);
        UFC::AnsiString logdata;
        UFC::BufferedLog::Printf( " Confirm_OPT send fail:[%s]", data.c_str() );
        logdata.Printf("Confirm_OPT time:%s DATA[%s]\n",TimeNow.c_str(),data.c_str());
        ScreenLog->WriteString( logdata );
    }
    else if( ( Type == 2 ) && ( EnableFuturesAdapter == TRUE ) )
    {
        UFC::BufferedLog::Printf( " <SEND> Fill" );
        if( client2 != NULL &&  client2->IsConnect() )
        {
            char Buffer[data.Length() + 1 ];
            memset( Buffer, '\0', data.Length() + 1 );            
            try
            {
                client2->BlockSend( (Int8*)data.c_str(), data.Length() );
                client2->BlockRecv( (UInt8*)Buffer, data.Length() );
                UFC::BufferedLog::DebugPrintf( " Recv:[%s]", Buffer );
                return;
            }
            catch( UFC::SocketException& ex )
            {
                UFC::BufferedLog::Printf( " Socket Exception:[%s]", ex.what() );
                client2->Disconnect( TRUE );
            }
            catch( UFC::Exception& ex )
            {
                UFC::BufferedLog::Printf( " Exception:[%s]", ex.what() );
                client2->Disconnect( TRUE );
            }
        }

        UFC::AnsiString TimeNow;
        UFC::GetHHMMSS(TimeNow);
        UFC::AnsiString logdata;
        UFC::BufferedLog::Printf( " Fill send fail:[%s]", data.c_str() );
        logdata.Printf("Fill time:%s DATA[%s]\n",TimeNow.c_str(),data.c_str());
        ScreenLog->WriteString( logdata );
    }
    else if( ( Type == 4 ) && ( EnableOptionsAdapter == TRUE ) )
    {
        UFC::BufferedLog::Printf( " <SEND> Fill_OPT" );
        if( client4 != NULL && client4->IsConnect() )
        {
            char Buffer[data.Length() + 1 ];
            memset( Buffer, '\0', data.Length() + 1 );
            try
            {
                client4->BlockSend( (Int8*)data.c_str(), data.Length() );
                client4->BlockRecv( (UInt8*)Buffer, data.Length() );
                UFC::BufferedLog::DebugPrintf( " Recv:[%s]", Buffer );
                return;
            }
            catch( UFC::SocketException& ex )
            {
                UFC::BufferedLog::Printf( " Socket Exception:[%s]", ex.what() );
                client4->Disconnect( TRUE );
            }
            catch( UFC::Exception& ex )
            {
                UFC::BufferedLog::Printf( " Exception:[%s]", ex.what() );
                client4->Disconnect( TRUE );
            }
        }

        UFC::AnsiString TimeNow;
        UFC::GetHHMMSS(TimeNow);
        UFC::AnsiString logdata;
        UFC::BufferedLog::Printf( " Fill_OPT send fail:[%s]", data.c_str() );
        logdata.Printf("Fill_OPT time:%s DATA[%s]\n",TimeNow.c_str(),data.c_str());
        ScreenLog->WriteString( logdata );
    }

}  //Send()
//------------------------------------------------------------------------------
void WriteUserlog( MTree *Data )
{
    UFC::AnsiString Account,Type,Msg;

    if( Data->get( "ID", Account ) && Data->get( "TYPE", Type ) && Data->get( "MSG", Msg ))
    {
        UFC::BufferedLog::Printf(",%s,%s,%s", Account.c_str(),Type.c_str(),Msg.c_str());
    }
}  //WriteUserlog()
//------------------------------------------------------------------------------
Int32           FOverlapTime = 30; //30 sec
//------------------------------------------------------------------------------
Int32 TimeToInt( void )
{
    return ( (FTime.getHour() * 10000) + (FTime.getMinute() * 100) + FTime.getSecond() );
}
//------------------------------------------------------------------------------
void UpdateRecoverBeginTime( void )
{
    FTime.setCurrent();
    FTime -= FOverlapTime;
    FRecoverBeginTime = TimeToInt( );
}
//------------------------------------------------------------------------------
void GetRecoverBeginTime( void )
{
    FTime.setCurrent();
    UFC::AnsiString ShmName;
    
    ShmName.Printf( "%s.RecoverBeginTime",AppName.c_str() );
    UFC::BufferedLog::DebugPrintf( " ShmName:[%s]", ShmName.c_str() );
    FRecoverBeginTime.Attach( ShmName );
    int t = FRecoverBeginTime.ToInt32();
    int Now  = TimeToInt( );
    
    if( Now < t ) ///< Over one day.
    {
        FRecoverBeginTime = RECOVER_BEGIN_TIME;
    }
    else
    {
        FTime.setHour( t / 10000 );
        FTime.setMinute( (t % 10000) / 100 );
        FTime.setSecond( t % 100 );
        FTime -= FOverlapTime;
        FRecoverBeginTime = TimeToInt( );    
    }    
}
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "CapitalExecConnection Copyright (c) September 2007~%d MDBS\n", UFC::ThisYear() );
    printf( "    -N         AppName.\n" );
    printf( "    -F         First time start up.\n" );
    printf( "    -D         Dump execution to screen only.(Not send to MBus.)\n" );
    printf( "    -R(HHMMSS) Resend execution from HHMMSS.\n" );
    printf( "    -B50       Send B50 cancel execution.\n" );
    exit( 1 );
}
//------------------------------------------------------------------------------
//   Modify recover time file to trigger recover action.
//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr )
{
    Int32 Time = TimeStr.ToInt();
    if( Time == 0 )
    {
        printf( " Wrong time string.\n" );
        return;
    }
    if( (Time/10000)>23 )
    {
        printf( " Hour should between 0 to 23.\n" );
        return;
    }
    if( ((Time%10000)/100)>59 )
    {
        printf( " Minutes should between 0 to 59.\n" );
        return;
    }    
    UFC::FileStreamEx File( YECTime.c_str(), "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    File.Write( TimeStr.c_str() , 7 );    
}    
//------------------------------------------------------------------------------
//   Check recover time file Modifaction
//------------------------------------------------------------------------------ 
BOOL CheckConfigModifiaction( void )
{
    static time_t ModifyTime = 0;
    struct stat FileStat;
    
    stat( YECTime.c_str(), &FileStat );    
    if( ModifyTime == 0 )    
    {        
        ModifyTime = FileStat.st_mtime;
        return FALSE;
    }
    else
    {
        if( ModifyTime != FileStat.st_mtime )
        {
            ModifyTime = FileStat.st_mtime;
            return TRUE;    
        }
        return FALSE;
    }
}
//------------------------------------------------------------------------------
//   Create recover time file. (Use startup time)
//------------------------------------------------------------------------------
void CreateTimeFile( void )
{
    char Time[7];
    UFC::FileStreamEx File( YECTime.c_str(), "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    FTime.setCurrent();
    sprintf( Time, "%02d%02d%02d",FTime.getHour(),FTime.getMinute(),FTime.getSecond() );    
    File.Write( Time , 7 );    
}    
//------------------------------------------------------------------------------
//   Get recover time from recover file.
//------------------------------------------------------------------------------
UFC::AnsiString GetRecoverTime( void )
{
    UFC::FileStreamEx File( YECTime.c_str(), "r+" );
    char Time[7];    
    
    File.Read( Time , 7 );    
    return UFC::AnsiString( Time ); 
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{    
    for( int i = 1; i < argc; i++ )
    {
        UFC::AnsiString Param( argv[i] );
        if( Param.AnsiPos("-N") != -1 )
        {
            UFC::AnsiString ExtStr( (const char*)(Param.c_str() + 2) );
            if( ExtStr.Length() == 0 )
                AppName = "CapitalExecConnection";
            else
                AppName.Printf("CapitalExecConnection%s", ExtStr.c_str());
            //break;
        }
    }
    if( AppName.Length() == 0 )
        AppName = "CapitalExecConnection";
        
    YECTime.Printf( "YECTime.%s",AppName.c_str() );
    GetRecoverBeginTime();
    
    try
    {
        for( int i = 1; i < argc; i++ )
        {
            UFC::AnsiString Param( argv[i] );
            if( Param == "-H" || Param == "-h" )
            {
                PrintHelp();
                exit( 0 );
            }
            else if( Param.AnsiPos("-R") != -1  )
            {
                UFC::AnsiString TimeStr( (const char*)(Param.c_str() + 2) );
                Resend( TimeStr );
                exit( 0 );
            }
            else if( Param == "-F" || Param == "-f" )
            {
                FRecoverBeginTime = RECOVER_BEGIN_TIME;                
            }
            else if( Param == "-D" || Param == "-d" )
            {
                DumpOnly = TRUE;
            }
            else if( Param == "-S" || Param == "-s" )
            {
                NoSocket = TRUE;
            }
            else if( Param == "-B50" )
            {
                EnableB50Cancel = TRUE;
            }
        }

        if( FRecoverBeginTime.ToInt32() != RECOVER_BEGIN_TIME )
        {
            UFC::AnsiString ConfigName;
            ConfigName.Printf( "../cfg/%s.cfg", AppName.c_str());
            try
            {
                UFC::UiniFile   Config(ConfigName);
                UFC::AnsiString Value;
                
                if( Config.GetValue( "Setting", "SendFromFirst", Value ) == TRUE && Value.Length() > 0 )                
                    FRecoverBeginTime = Value.ToInt();
            }
            catch(...)
            {
                UFC::BufferedLog::Printf(" %s not found.", ConfigName.c_str());
                exit(1);
            }
        }
    }
    catch( UFC::Exception& ex )
    {
        printf( ex.what() );
        PrintHelp();
    }
}
//--------------------------------------------------------------------------------------------------------------------
void InitSocket()
{
    UFC::AnsiString ConfigName;
    ConfigName.Printf( "../cfg/%s.cfg" ,AppName.c_str());
    try
    {
        DisconnectTime1 = "";
        DisconnectTime2 = "";
        DisconnectTime3 = "";
        DisconnectTime4 = "";
        UFC::UiniFile   Config(ConfigName);
        UFC::AnsiString ConfirmIP,ConfirmOPTIP;
        UFC::AnsiString FillIP,FillOPTIP;
        Int32 ConfirmPort,ConfirmOPTPort;
        Int32 FillPort,FillOPTPort;

        UFC::AnsiString Value;
        if( Config.GetValue("Setting", "EnableFuturesAdapter", Value) == TRUE )
        {
            if ((Value == "Y") || (Value == "y")) EnableFuturesAdapter = TRUE;
        }
        else
        {
            UFC::BufferedLog::Printf(" EnableFuturesAdapter not set.");
            exit(1);
        }
        
        if( Config.GetValue("Setting", "EnableOptionsAdapter", Value) == TRUE )
        {
            if ((Value == "Y") || (Value == "y")) EnableOptionsAdapter = TRUE;
        }
        else
        {
            UFC::BufferedLog::Printf(" EnableOptionsAdapter not set.");
            exit(1);
        }
        
        if( Config.GetValue("Setting", "SendOrderExec", Value) == TRUE )
        {
            if ((Value == "Y") || (Value == "y")) 
                ExecSession = esOffHour;
            else if ((Value == "N") || (Value == "N")) 
                ExecSession = esNormal;
            else
                ExecSession = esAll;
        }
        switch( ExecSession )
        {
            case esAll:     UFC::BufferedLog::Printf(" Send all Trading Session executions.");break;
            case esNormal:  UFC::BufferedLog::Printf(" Send Normal Trading Session executions.");break;
            case esOffHour: UFC::BufferedLog::Printf(" Send Off-Hour Trading Session executions.");break;
        }

        if( Config.GetValue("Setting", "EnableServer", Value) == TRUE )
        {
            if ((Value == "Y") || (Value == "y")) EnableServer = TRUE;
        }
        else
        {
            UFC::BufferedLog::Printf(" EnableServer not set.");
            exit(1);
        }
        
        if( Config.GetValue("Confirm", "IP", ConfirmIP ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Confirm IP not set.");
            exit(1);
        }

        if( Config.GetValue( "Confirm", "Port", Value ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Confirm Port not set.");
            exit(1);
        }
        else
            ConfirmPort = Value.ToInt();

        if( Config.GetValue( "Confirm", "HeartbeatTime", Value ) == FALSE )
        {
            //UFC::BufferedLog::Printf(" Confirm HeartbeatTime not set.");
            //exit(1);
        }
        else
            HeartbeatTime1 = Value.ToInt() * 60;

        if( Config.GetValue("Confirm_OPT", "IP", ConfirmOPTIP ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Confirm_OPT IP not set.");
            exit(1);
        }

        if( Config.GetValue( "Confirm_OPT", "Port", Value ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Confirm_OPT Port not set.");
            exit(1);
        }
        else
            ConfirmOPTPort = Value.ToInt();

        if( Config.GetValue( "Confirm_OPT", "HeartbeatTime", Value ) == FALSE )
        {
            //UFC::BufferedLog::Printf(" Confirm HeartbeatTime not set.");
            //exit(1);
        }
        else
            HeartbeatTime3 = Value.ToInt() * 60;

        if( Config.GetValue("Fill", "IP", FillIP ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Fill IP not set.");
            exit(1);
        }

        if( Config.GetValue( "Fill","Port", Value ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Fill Port not set.");
            exit(1);
        }
        else
            FillPort = Value.ToInt();

        if( Config.GetValue( "Fill", "HeartbeatTime", Value ) == FALSE )
        {
            //UFC::BufferedLog::Printf(" Confirm HeartbeatTime not set.");
            //exit(1);
        }
        else
            HeartbeatTime2 = Value.ToInt() * 60;

        if( Config.GetValue("Fill_OPT", "IP", FillOPTIP ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Fill_OPT IP not set.");
            exit(1);
        }

        if( Config.GetValue( "Fill_OPT","Port", Value ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Fill_OPT Port not set.");
            exit(1);
        }
        else
            FillOPTPort = Value.ToInt();

        if( Config.GetValue( "Fill_OPT", "HeartbeatTime", Value ) == FALSE )
        {
            //UFC::BufferedLog::Printf(" Confirm HeartbeatTime not set.");
            //exit(1);
        }
        else
            HeartbeatTime4 = Value.ToInt() * 60;

        UFC::AnsiString ServerPort( "20001" );
        if( Config.GetValue( "ServerSetting", "Port", ServerPort ) == FALSE )
        {
            UFC::BufferedLog::Printf(" ServerSetting Port not set.");
            exit(1);
        }

        if (EnableServer == TRUE)
        {
            server = new UFC::MultiplexingServer( ServerPort.ToInt(), 3, 2 );
            server->SetListener( new ServerSocketMonitor(/*socketlist*/) );
            server->EnableHeartbeat( FALSE );
            server->Run();
        }

        if (EnableFuturesAdapter == TRUE)
        {
            client1 = new UFC::PClientSocket( ConfirmIP, ConfirmPort, FALSE );
            ConfirmMonitor = new SocketMonitor();
            client1->SetListener( ConfirmMonitor );
            try
            {
                UFC::BufferedLog::Printf( " Confirm socket client connect to %s:%d", ConfirmIP.c_str(), ConfirmPort );
                client1->Connect();
                client1->SetBlockMode( FALSE );
            }
            catch( UFC::Exception& ex )
            {
                UFC::AnsiString msg;msg.Printf( " Confirm socket client connect %s:%d fail:[%s]", ConfirmIP.c_str(), ConfirmPort, ex.what() );
                UFC::BufferedLog::Printf( "%s",msg.c_str() );
                //SendAlert( msg );
            }
            catch(...)
            {
                UFC::AnsiString msg;msg.Printf( " Confirm socket client connect %s:%d fail.", ConfirmIP.c_str(), ConfirmPort );
                UFC::BufferedLog::Printf( "%s",msg.c_str() );
                //SendAlert( msg );
            }

            client2 = new UFC::PClientSocket( FillIP, FillPort, FALSE );
            FillMonitor = new SocketMonitor();
            client2->SetListener( FillMonitor );
            try
            {
                UFC::BufferedLog::Printf( " Fill socket client connect to %s:%d", FillIP.c_str(), FillPort );
                client2->Connect();
                client2->SetBlockMode( FALSE );
            }
            catch( UFC::Exception& ex )
            {
                UFC::AnsiString msg;msg.Printf( " Fill socket client connect %s:%d fail:[%s]", FillIP.c_str(), FillPort, ex.what() );
                UFC::BufferedLog::Printf( "%s",msg.c_str() );
                //SendAlert( msg );
            }
            catch(...)
            {
                UFC::AnsiString msg;msg.Printf( " Fill socket client connect %s:%d fail.", FillIP.c_str(), FillPort );
                UFC::BufferedLog::Printf( "%s",msg.c_str() );
                //SendAlert( msg );
            }
        }  //if (EnableFuturesAdapter == TRUE)
        
        if (EnableOptionsAdapter == TRUE)
        {
            client3 = new UFC::PClientSocket( ConfirmOPTIP, ConfirmOPTPort, FALSE );
            ConfirmOPTMonitor = new SocketMonitor();
            client3->SetListener( ConfirmOPTMonitor );
            try
            {
                UFC::BufferedLog::Printf( " Confirm_OPT socket client connect to %s:%d", ConfirmOPTIP.c_str(), ConfirmOPTPort );
                client3->Connect();
                client3->SetBlockMode( FALSE );
            }
            catch( UFC::Exception& ex )
            {
                UFC::AnsiString msg;msg.Printf( " Confirm_OPT socket client connect %s:%d fail:[%s]", ConfirmOPTIP.c_str(), ConfirmOPTPort, ex.what() );
                UFC::BufferedLog::Printf( "%s",msg.c_str() );
                //SendAlert( msg );
            }
            catch(...)
            {
                UFC::AnsiString msg;msg.Printf( " Confirm_OPT socket client connect %s:%d fail.", ConfirmOPTIP.c_str(), ConfirmOPTPort );
                UFC::BufferedLog::Printf( "%s",msg.c_str() );
                //SendAlert( msg );
            }

            client4 = new UFC::PClientSocket( FillOPTIP, FillOPTPort, FALSE );
            FillOPTMonitor = new SocketMonitor();
            client4->SetListener( FillOPTMonitor );
            try
            {
                UFC::BufferedLog::Printf( " Fill_OPT socket client connect to %s:%d", FillOPTIP.c_str(), FillOPTPort );
                client4->Connect();
                client4->SetBlockMode( FALSE );
            }
            catch( UFC::Exception& ex )
            {
                UFC::AnsiString msg;msg.Printf( " Fill_OPT socket client connect %s:%d fail:[%s]", FillOPTIP.c_str(), FillOPTPort, ex.what() );
                UFC::BufferedLog::Printf( "%s",msg.c_str() );
                //SendAlert( msg );
            }
            catch(...)
            {
                UFC::AnsiString msg;msg.Printf( " Fill_OPT socket client connect %s:%d fail.", FillOPTIP.c_str(), FillOPTPort );
                UFC::BufferedLog::Printf( "%s",msg.c_str() );
                //SendAlert( msg );
            }
        }  //if (EnableOptionsAdapter == TRUE)
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" %s not found.",ConfigName.c_str());
        exit(1);
    }
}  //InitSocket()
//------------------------------------------------------------------------------
void SocketConnect(UFC::PClientSocket* client, const char* msg)
{
    try
    {
        if( client != NULL && client->IsConnect() == FALSE )
        {
            UFC::BufferedLog::DebugPrintf( " %s connect to Backend...",msg );
            client->Connect();
            client->SetBlockMode( FALSE );
            return;
        }
    }
    catch( UFC::Exception& ex )
    {
        UFC::BufferedLog::DebugPrintf( " %s socket client connect fail:[%s]",msg, ex.what() );
    }
    catch(...)
    {
        UFC::BufferedLog::DebugPrintf( " %s socket client connect fail.",msg );
    }
}  //SocketConnect()
//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    UFC::BufferedLog::SetDebugMode( TRUE );
    CheckArgs( argc, argv );    

    MessageObj = new MessageObject( AppName, "1.0", "XXXXX" );
    MessageObj->SetMonitorListener( new ProcessMonitor() );
    MessageObj->Start();
    MessageObj->WaitForConnected();

    UFC::AnsiString LogName;
    LogName.Printf("../log/%s_%s.log", AppName.c_str(), UFC::GetDateString().c_str());
    
    UFC::BufferedLog* LogObject = new UFC::BufferedLog( LogName, 1024, TRUE );
    UFC::BufferedLog::SetLogObject( LogObject );

    if( NoSocket == FALSE )
        InitSocket();

    LogName.Printf("../log/%sSendMiss%s.log",AppName.c_str(),UFC::GetDateString().c_str());
    ScreenLog = new UFC::BufferedLog( LogName, 1024, TRUE );
    
    CaptialExecConnection *ExecConnectionPtr = 0;
    
    if ((EnableFuturesAdapter == TRUE) || (EnableOptionsAdapter == TRUE))
        ExecConnectionPtr = new CaptialExecConnection(FRecoverBeginTime.ToInt32());
    CreateTimeFile( );
    
    while( TRUE )
    {
        if ((server != NULL) && (server->IsTerminated() == TRUE) ) exit(0);
        if (EnableFuturesAdapter == TRUE)
        {
            SocketConnect(client1, "Confirm");
            SocketConnect(client2, "Fill");
        }
        if (EnableOptionsAdapter == TRUE)
        {
            SocketConnect(client3, "Confirm_OPT");
            SocketConnect(client4, "Fill_OPT");
        }

        //log->FlushToFile();
        UFC::SleepMS( 1000 );
        //FTime.setCurrent();

        if ( ( ExecConnectionPtr != 0 ) && ExecConnectionPtr->IsConnected() )
            UpdateRecoverBeginTime();
        
        ScreenLog->Flush();
        UFC::BufferedLog::FlushToFile();

        if( ( ExecConnectionPtr != 0 ) && ( CheckConfigModifiaction() == TRUE ) )
            ExecConnectionPtr->Recover( GetRecoverTime());

        if ((EnableFuturesAdapter == TRUE) && (client1 != NULL) && client1->IsConnect())
        {
            if( ( HeartbeatTime1 > 0 ) && ( HeartbeatTimeCount1 == HeartbeatTime1 ) )
            {
                HeartbeatTimeCount1 = 0;
                HeartbeatConfirm(client1);
            }
            HeartbeatTimeCount1++;

            if( DisconnectTime1.Length() == 6 ) 
            {
                if( ExecConnectionPtr != 0 ) ExecConnectionPtr->Recover(DisconnectTime1);
                DisconnectTime1 = "";
            }
        }
        if ((EnableFuturesAdapter == TRUE) && (client2 != NULL ) && client2->IsConnect())
        {
            if( ( HeartbeatTime2 > 0 ) && ( HeartbeatTimeCount2 == HeartbeatTime2 ) )
            {
                HeartbeatTimeCount2 = 0;
                HeartbeatFill(client2);
            }
            HeartbeatTimeCount2++;

            if( DisconnectTime2.Length() == 6 ) 
            {
                if( ExecConnectionPtr != 0 ) ExecConnectionPtr->Recover(DisconnectTime2);
                DisconnectTime2 = "";
            }
        }
        if( ( EnableOptionsAdapter == TRUE ) && ( client3 != NULL ) && client3->IsConnect() )
        {
            if( ( HeartbeatTime3 > 0 ) && ( HeartbeatTimeCount3 == HeartbeatTime3 ) )
            {
                HeartbeatTimeCount3 = 0;
                HeartbeatConfirm(client3);
            }
            HeartbeatTimeCount3++;

            if( DisconnectTime3.Length() == 6 ) 
            {
                if( ExecConnectionPtr != 0 ) ExecConnectionPtr->Recover(DisconnectTime3);
                DisconnectTime3 = "";
            }
        }
        if( ( EnableOptionsAdapter == TRUE ) && ( client4 != NULL ) && client4->IsConnect() )
        {
            if( ( HeartbeatTime4 > 0 ) && ( HeartbeatTimeCount4 == HeartbeatTime4 ) )
            {
                HeartbeatTimeCount4 = 0;
                HeartbeatFill(client4);
            }
            HeartbeatTimeCount4++;

            if( DisconnectTime4.Length() == 6 ) 
            {
                if( ExecConnectionPtr != 0 ) ExecConnectionPtr->Recover(DisconnectTime4);
                DisconnectTime4 = "";
            }
        }
    }  //while( TRUE )
    delete ScreenLog;

    if ( ExecConnectionPtr != 0 ) delete ExecConnectionPtr;
    if( client1 != NULL )
        delete client1;
    if( client2 != NULL )
        delete client2;
    if( client3 != NULL )
        delete client3;
    if( client4 != NULL )
        delete client4;

    if( server != NULL )
    {
        server->Stop();
        delete server;
    }
    delete MessageObj;
    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
