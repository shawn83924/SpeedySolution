// 
// File:   Main.cpp
// Author: Zhen Fan
//
// Created on 2015/03/31, 16:21:00
// Source from GCExecConnection
//------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "UniExecConnection.h"
//------------------------------------------------------------------------------
UFC::UDateTime gAppStartTime;
UFC::AnsiString gConfigFileName = "UniExecConnection.cfg";
UFC::AnsiString gAppName = "UniExecConnection"; 
UFC::AnsiString gAppVersion = "1.0.0";
UFC::AnsiString gAppDescription = "Uni-PSG Execution Connection";
UFC::AnsiString gMBusIP = "127.0.0.1";
int gMBusPort = 12345;
UFC::AnsiString gGatewayIP = "127.0.0.1";
int gGatewayPort = 23456;
UFC::AnsiString gGatewayID = "yuan";
UFC::AnsiString gGatewayPassword = "1234rewq";
UFC::AnsiString gGatewayAccount = "0000000";
UFC::AnsiString gGatewayBrokerID = "F01300";
UFC::AnsiString gGatewayWebID = "1234";
char gGatewayOrderIDPrefix = 'x';
UFC::AnsiString gTradeDate = "";
UFC::AnsiString gSendSubject = "Confirm";
UFC::AnsiString gSendKey = "To400";
UFC::AnsiString gSendFrontKey = "To400Front";
bool gIsFillReportOnly = false;

UFC::PInt32 FRecoverBeginTime;
UFC::UTime  FTime;
const char  *START_TIME_FILENAME = "UniECTime";

//------------------------------------------------------------------------------
void GetRecoverBeginTime( void )
{
    UFC::AnsiString ShmName( "UniExecConnection.RecoverBeginTime" );
    FRecoverBeginTime.Attach( ShmName );
    
    int t = FRecoverBeginTime.ToInt32();
    FTime.setHour( t / 10000 );
    FTime.setMinute( (t % 10000) / 100 );
    FTime.setSecond( t % 100 );
    FTime -= 60;
}
//------------------------------------------------------------------------------
void UpdateRecoverBeginTime( void )
{
    FTime.setCurrent();
    FRecoverBeginTime = ( (FTime.getHour() * 10000) +
                          (FTime.getMinute() * 100) +
                          (FTime.getSecond()) );
}
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "UniExecConnection Copyright (c) 2016~%d MDBS\n", UFC::ThisYear() );
    printf( "Build: %s %s, %ld bit mode\n", __DATE__, __TIME__, sizeof(void*) * 8 );
    printf( "    -F         First time start up.\n" );
    printf( "    -D         Dump execution to screen only.(Not send to MBus.)\n" );
    printf( "    -R(HHMMSS) Resend all execution from HHMMSS.(Both TAIFEX and PATS)\n" );    
    printf( "    -RP(HHMMSS) Resend PATS execution from HHMMSS.\n" );
    printf( "    -RTF(HHMMSS) Resend TAIFEX Futures execution from HHMMSS.\n" );
    printf( "    -RTO(HHMMSS) Resend TAIFEX Options execution from HHMMSS.\n" );
    exit( 1 );
}
//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr, char Type )
{
    Int32 Time = TimeStr.ToInt();
    if( Time == 0 )
    {
        printf( " Wrong time string[%s].\n", TimeStr.c_str() );
        return;
    }
    if( ( ( Time / 10000 ) > 23 ) || ( ( Time / 10000 ) < 1 ) )
    {
        printf( " Hour should between 0 to 23.\n" );
        return;
    }
    if( ( ( Time % 10000 ) / 100 ) > 59 )
    {
        printf( " Minutes should between 0 to 59.\n" );
        return;
    }    
    UFC::FileStreamEx File( START_TIME_FILENAME, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    UFC::AnsiString   DataStr;
    
    DataStr.Printf("%06d%c", Time, Type );
    File.Write( DataStr.c_str() , 8 );    
    printf( "\n\nAsk running UniExecConnection resend\n" );
    switch( Type )
    {
        case 'A': printf( "[All] Executions\n" );     break;
        case 'P': printf( "[PATS] Executions\n" );    break;
        case 'O': printf( "[Options] Executions\n" ); break;
        case 'F': printf( "[Futures] Executions\n" ); break;
    }
    printf( "From [%02d:%02d:%02d]\n", Time / 10000, ( Time / 100 ) % 100, Time % 100 );
    printf( "--------------------------------------------------\n" );
}    
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{
    try
    {
        for( int i = 1; i < argc; i++ )
        {
            UFC::AnsiString curParam( argv[i] );
            UFC::AnsiString upperParam = curParam.UpperCase();
            if( upperParam == "-H" )
                PrintHelp();
            else if( upperParam.AnsiPos("-RTF") != -1 )
            {
                if( curParam.Length() > 4 )
                {      
                     UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 4) );
                     Resend( TimeStr, 'F' );
                }
                else
                {
                    printf( "\nResend TAIFEX Futures execution from HHMMSS failed.\n");
                    printf( "Example:UniExecConnection -RTF123059\n");
                }
                exit( 0 );                    
            }
            else if( upperParam.AnsiPos("-RTO") != -1 )
            {
                if( curParam.Length() > 4 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 4) );
                    Resend( TimeStr, 'O' );
                }
                else
                {                    
                    printf( "\nResend TAIFEX Options execution from HHMMSS failed.\n");                    
                    printf( "Example:UniExecConnection -RTO123059\n");
                }
                exit( 0 );
            }
            else if( upperParam.AnsiPos("-RP") != -1  )
            {
                if( curParam.Length() > 3 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 3 ) );
                    Resend( TimeStr, 'P' );
                }
                else
                {                    
                    printf( "\nResend PATS execution from HHMMSS failed.\n");
                    printf( "Example:UniExecConnection -RP123059\n");
                }
                exit( 0 );
            }
            else if( upperParam.AnsiPos("-R") != -1  )
            {
                if( curParam.Length() > 2 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 2) );
                    Resend( TimeStr, 'A' );
                }
                else
                {                    
                    printf( "\nResend all execution from HHMMSS failed.\n");                                        
                    printf( "Example:UniExecConnection -R123059\n");
                }
                exit( 0 );
            }
            else if( upperParam == "-F" )
                FRecoverBeginTime = 0;
            else if( upperParam == "-D" )
                UniExecConnection::FIsDumpOnly = true;
        }  //for( int i = 1; i < argc; i++ )
    }
    catch( UFC::Exception& ex )
    {
        printf( ex.what() );
        PrintHelp();
    }
}
//--------------------------------------------------------------------------------------------------------------------
BOOL CheckConfigModifiaction( void )
{
    static time_t ModifyTime = 0;
    struct stat FileStat;
    
    stat( START_TIME_FILENAME, &FileStat );
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
void CreateTimeFile( void )
{
    char Buffer[8];
    UFC::FileStreamEx File( START_TIME_FILENAME, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    FTime.setCurrent();
    sprintf( Buffer, "%02d%02d%02dA",FTime.getHour(),FTime.getMinute(),FTime.getSecond() );    
    File.Write( Buffer , 8 );    
}    
//------------------------------------------------------------------------------
BOOL GetRecoverTime( UFC::AnsiString& Time, char& Type  )
{
    UFC::FileStreamEx File( START_TIME_FILENAME, "r+" );
    char Buffer[ 8 ];    
    
    if( File.Read( Buffer , 8 ) == 8 )
    {
        Time = UFC::AnsiString( Buffer, 6 ); 
        Type = Buffer[ 6 ];
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
void LoadSetting(const UFC::AnsiString& ConfigFileName)
{    
    UFC::AnsiString fullConfigFileName;
    fullConfigFileName.Printf("../cfg/%s", ConfigFileName.c_str());
    UFC::UiniFile *configFilePtr = 0;
    try 
    {
        configFilePtr = new UFC::UiniFile(fullConfigFileName);
    }
    catch (UFC::FileException& fe)
    {
        UFC::BufferedLog::Printf("Config File [%s] Open Error: %s\n", fullConfigFileName.c_str(), fe.what());
        configFilePtr = 0;
    }
    if (configFilePtr == 0) exit(-1);

    UFC::AnsiString sectionName = "Setting";
    UFC::AnsiString fieldName = "AppName";
    UFC::AnsiString valueStr;
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gAppName = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gAppName.c_str());

    fieldName = "AppVersion";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gAppVersion = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gAppVersion.c_str());

    fieldName = "AppDescription";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gAppDescription = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gAppDescription.c_str());
    
    fieldName = "MBusIP";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gMBusIP = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gMBusIP.c_str());
    
    fieldName = "MBusPort";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gMBusPort = valueStr.ToInt();
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value %d.", sectionName.c_str(), fieldName.c_str(), gMBusPort);

    fieldName = "GatewayIP";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gGatewayIP = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gGatewayIP.c_str());
    
    fieldName = "GatewayPort";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gGatewayPort = valueStr.ToInt();
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value %d.", sectionName.c_str(), fieldName.c_str(), gGatewayPort);

    fieldName = "GatewayID";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gGatewayID = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gGatewayID.c_str());
    
    fieldName = "GatewayPassword";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gGatewayPassword = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gGatewayPassword.c_str());
    
    fieldName = "GatewayAccount";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gGatewayAccount = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gGatewayAccount.c_str());
    
    fieldName = "GatewayBrokerID";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gGatewayBrokerID = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gGatewayBrokerID.c_str());
    
    fieldName = "GatewayWebID";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gGatewayWebID = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gGatewayWebID.c_str());
    
    fieldName = "GatewayOrderIDPrefix";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gGatewayOrderIDPrefix = valueStr[0];
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value '%c'.", sectionName.c_str(), fieldName.c_str(), gGatewayOrderIDPrefix);
    
    fieldName = "SendSubject";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gSendSubject = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gSendSubject.c_str());
    
    fieldName = "SendKey";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gSendKey = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gSendKey.c_str());
    
    fieldName = "SendFrontKey";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
        gSendFrontKey = valueStr;
    else
        UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value \"%s\".", sectionName.c_str(), fieldName.c_str(), gSendFrontKey.c_str());
    
    fieldName = "FillReportOnly";
    if (configFilePtr->GetValue(sectionName, fieldName, valueStr) == TRUE)
    {
        if ((valueStr == "Y") || (valueStr == "y") || (valueStr == "1") || (valueStr == "TRUE") || (valueStr == "true"))
            gIsFillReportOnly = true;
        else
            gIsFillReportOnly = false;
    }
    else
    {
        if (gIsFillReportOnly)
            UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value true.", sectionName.c_str(), fieldName.c_str());
        else
            UFC::BufferedLog::Printf("LoadSetting() Missing [%s]%s, Use default value false.", sectionName.c_str(), fieldName.c_str());
    }
    
    delete configFilePtr;
}  //LoadSetting()

//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    gAppStartTime.setCurrent();
    UFC::GetTradeYYYYMMDD(gTradeDate);
    
    GetRecoverBeginTime();
    CheckArgs( argc, argv );
    LoadSetting(gConfigFileName);
    UFC::AnsiString logFileName;
    logFileName.Printf("../log/%s%s.log", gAppName.c_str(), gTradeDate.c_str());
    UFC::BufferedLog::SetLogObject(new UFC::BufferedLog( logFileName, 1024, TRUE));
    UFC::BufferedLog::Printf( " __________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "  Uni-President Securities Group backend interface module " );
    UFC::BufferedLog::Printf( "  startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "  Build at:%s %s, %ld bit mode", __DATE__, __TIME__, sizeof(void*) * 8 );
    UFC::BufferedLog::Printf( "  Application[%s] Version[%s] %s", gAppName.c_str(), gAppVersion.c_str(), gAppDescription.c_str() );
    UFC::BufferedLog::Printf( "  TradeDate[%s]", gTradeDate.c_str() );    
    UFC::BufferedLog::Printf( "  MBus Server[%s:%d]", gMBusIP.c_str(), gMBusPort );
    UFC::BufferedLog::Printf( "  Gateway[%s:%d] ID[%s] Account[%s] Broker[%s] WebID[%s] OrderIDPrefix[%c]", gGatewayIP.c_str(), gGatewayPort, gGatewayID.c_str(), gGatewayAccount.c_str(), gGatewayBrokerID.c_str(), gGatewayWebID.c_str(), gGatewayOrderIDPrefix );
    UFC::BufferedLog::Printf( "  Send [%s][%s] ", gSendSubject.c_str(), gSendKey.c_str() );
    UFC::BufferedLog::Printf( "  Send Front[%s][%s] ", gSendSubject.c_str(), gSendFrontKey.c_str() );
    
    if( UniExecConnection::FIsDumpOnly ) 
        UFC::BufferedLog::Printf( "  Debug Mode:On (Dump to standard output only)");    
    else
        UFC::BufferedLog::Printf( "  Debug Mode:Off");
    
    if (gIsFillReportOnly)
        UFC::BufferedLog::Printf("  Fill Report Only.");
    UFC::BufferedLog::Printf( " __________________________________________" );

    UniExecConnection::FMsgObjPtr = new MessageObject( gAppName, gAppVersion, gAppDescription, gMBusPort );
    UniExecConnection::FMsgObjPtr->SetHost(gMBusIP);
    UniExecConnection::FMsgObjPtr->SetMonitorListener( new ProcessMonitor() );    
    UniExecConnection::FMsgObjPtr->Start(); ///< Start Migo Message pump.    
    UniExecConnection::FMsgObjPtr->WaitForConnected();            
    
    UniExecConnection execConnection(gAppName, gGatewayIP, gGatewayPort, gGatewayID, gGatewayPassword, gGatewayAccount, gGatewayBrokerID, gGatewayWebID, gGatewayOrderIDPrefix);
    CreateTimeFile( );
    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );
        
        FTime.setCurrent();
        UpdateRecoverBeginTime();        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
        if( CheckConfigModifiaction() == TRUE )
        {
            UFC::AnsiString Time;
            char Type;
            
            if( GetRecoverTime( Time, Type ) == TRUE ) 
                execConnection.Recover( Time, Type );
                //ExecConnection.Recover( Time, Type );        
        }
            
    }    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
