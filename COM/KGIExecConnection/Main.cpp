// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "UFC.h"
#include "KGIExecConnection.h"
//------------------------------------------------------------------------------
UFC::PInt32     FRecoverBeginTime;
UFC::UTime      FTime;
BOOL            DumpOnly = FALSE;
bool            gIsUSMapper = false;
MessageObject*  MessageObj;
const char*     START_TIME_FILENAME = "KGIExecConnectionTime";
//----------------------------------------------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected( BOOL IsTheFirstOne )
{
    if( IsTheFirstOne == FALSE )
    {
        UFC::BufferedLog::Printf( " ***** Process already exists *****"  );
        sleep( 2 );
        exit( 0 );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void WriteUserlog( MTree *Data )
{
    UFC::AnsiString Account,Type,Msg;

    if( Data->get( "ID", Account ) && Data->get( "TYPE", Type ) && Data->get( "MSG", Msg ))
    {
        UFC::BufferedLog::Printf(",%s,%s,%s", Account.c_str(),Type.c_str(),Msg.c_str());
    }
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
void GetRecoverBeginTime( void )
{
    UFC::AnsiString ShmName( "KGIExecConnection.RecoverBeginTime" );
    FRecoverBeginTime.Attach( ShmName );
    
    int t = FRecoverBeginTime.ToInt32();
    FTime.setHour( t / 10000 );
    FTime.setMinute( (t % 10000) / 100 );
    FTime.setSecond( t % 100 );
    FTime -= 60;    
}
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "KGIExecConnection Copyright (c) July 2007~%d MDBS.\n", UFC::ThisYear() );
    printf( "Build Date:%s Time:%s, %ld bit mode\n", __DATE__, __TIME__, sizeof(void*) * 8 );        
    printf( "    \n" );         
    printf( "    -F            First time start up.\n" );
    printf( "    -D            Dump execution to screen only.(Not send to MBus.)\n" );
    printf( "    -us           for US Mapper.)\n" );
    printf( "    -R(HHMMSS~hhmmss)    Resend all execution from HHMMSS to hhmmss.(All TAIFEX, TWSE and PATS)\n" );
    printf( "    -RP(HHMMSS~hhmmss)   Resend PATS execution from HHMMSS to hhmmss.\n" );
    printf( "    -RTF(HHMMSS~hhmmss)  Resend TAIFEX Futures execution from HHMMSS to hhmmss.\n" );
    printf( "    -RTO(HHMMSS~hhmmss)  Resend TAIFEX Options execution from HHMMSS to hhmmss.\n" );
    printf( "    -RST(HHMMSS~hhmmss)  Resend TSE execution from HHMMSS to hhmmss.\n" );
    printf( "    -RSO(HHMMSS~hhmmss)  Resend OTC execution from HHMMSS to hhmmss.\n" );
    printf( "    \n" );
    printf( "    -RC(HHMMSS~hhmmss)   Resend all confirm execution from HHMMSS to hhmmss.(All TAIFEX, TWSE and PATS)\n" );
    printf( "    -RPC(HHMMSS~hhmmss)  Resend PATS confirm execution from HHMMSS to hhmmss.\n" );
    printf( "    -RTFC(HHMMSS~hhmmss) Resend TAIFEX Futures confirm execution from HHMMSS to hhmmss.\n" );
    printf( "    -RTOC(HHMMSS~hhmmss) Resend TAIFEX Options confirm execution from HHMMSS to hhmmss.\n" );
    printf( "    -RSTC(HHMMSS~hhmmss) Resend TSE confirm execution from HHMMSS to hhmmss.\n" );
    printf( "    -RSOC(HHMMSS~hhmmss) Resend OTC confirm execution from HHMMSS to hhmmss.\n" );
    printf( "    \n" );
    printf( "    -RF(HHMMSS~hhmmss)   Resend all filled execution from HHMMSS to hhmmss.(All TAIFEX, TWSE and PATS)\n" );
    printf( "    -RPF(HHMMSS~hhmmss)  Resend PATS filled execution from HHMMSS to hhmmss.\n" );
    printf( "    -RTFF(HHMMSS~hhmmss) Resend TAIFEX Futures filled execution from HHMMSS to hhmmss.\n" );
    printf( "    -RTOF(HHMMSS~hhmmss) Resend TAIFEX Options filled execution from HHMMSS to hhmmss.\n" );
    printf( "    -RSTF(HHMMSS~hhmmss) Resend TSE filled execution from HHMMSS to hhmmss.\n" );
    printf( "    -RSOF(HHMMSS~hhmmss) Resend OTC filled execution from HHMMSS to hhmmss.\n\n" );
    printf( "    -TWSERECVOLD  Use TWSE Order version.(Before 2020 Mar)\n\n" );
    printf( "    -TWSESENDOLD  Use TWSE Order version.(Before 2020 Mar)\n\n" );
    exit( 1 );
}
//------------------------------------------------------------------------------
bool CheckTimeStr( const UFC::AnsiString& TimeStr )
{
    if( TimeStr.Length() <= 0 ) return false;
    
    int timeValue = TimeStr.ToInt();
    if( timeValue <= 0 )
    {
        printf("Wrong time string[%s].\n", TimeStr.c_str());
        return false;
    }
    
    int hourValue = timeValue / 10000;
    if ((hourValue >= 0) && (hourValue <= 23))
    {
        int minuteValue = ( timeValue % 10000 ) / 100;
        if ((minuteValue >= 0) && (minuteValue <= 59))
        {
            int secondValue = timeValue % 1000000;
            if ((secondValue >= 0) && (secondValue <= 59))
                return true;
            else
                printf("Second[%d] should between 0 to 59.\n", secondValue);
        }
        else
            printf("Minutes[%d] should between 0 to 59.\n", minuteValue);
    }
    else
        printf("Hour[%d] should between 0 to 23.\n", hourValue);
    return false;
}  //CheckTimeStr()
//------------------------------------------------------------------------------
bool CheckTimeValue(int TimeValue, int& HourValue, int& MinuteValue, int& SecondValue)
{
    if( TimeValue > 0 )
    {
        HourValue = TimeValue / 10000;
        if ((HourValue >= 0) && (HourValue <= 23))
        {
            MinuteValue = (TimeValue % 10000) / 100;
            if ((MinuteValue >= 0) && (MinuteValue <= 59))
            {
                SecondValue = TimeValue % 100;
                if ((SecondValue >= 0) && (SecondValue <= 59))
                    return true;
                else
                    printf("Second[%d] should between 0 to 59.\n", SecondValue);
            }
            else
                printf("Minutes[%d] should between 0 to 59.\n", MinuteValue);
        }
        else
            printf("Hour[%d] should between 0 to 23.\n", HourValue);
    }
    else
        printf("Wrong time Value[%d].\n", TimeValue);
    return false;
}  //CheckTimeStr()
//------------------------------------------------------------------------------
void Resend( const UFC::AnsiString& TimeStr, char Type, char EType )
{
    UFC::AnsiString beginTimeStr = "", endTimeStr = "";
    int beginTimeValue = -1, beginHourValue = -1, beginMinuteValue = -1, beginSecondValue = -1;
    int endTimeValue = -1, endHourValue = -1, endMinuteValue = -1, endSecondValue = -1;
    bool isCorrectBeginTime = false, isCorrectEndTime = false;
    if (TimeStr.Length() >= 6)
    {
        beginTimeStr = TimeStr.SubString(0, 6);
        beginTimeValue = beginTimeStr.ToInt();
        isCorrectBeginTime = CheckTimeValue(beginTimeValue, beginHourValue, beginMinuteValue, beginSecondValue);
        if (isCorrectBeginTime)
        {
            if (TimeStr.Length() >= 13)
            {
                endTimeStr = TimeStr.SubString(7, 6);
                endTimeValue = endTimeStr.ToInt();
                isCorrectEndTime = CheckTimeValue(endTimeValue, endHourValue, endMinuteValue, endSecondValue);
            }
            else
            {
                endHourValue = 0;
                endMinuteValue = 0;
                endSecondValue = 0;
                isCorrectEndTime = true;
            }
        }
    }  //if (TimeStr.Length() >= 6)
    
    if (isCorrectBeginTime && isCorrectEndTime)
    {
        UFC::PStringBuffer resendBuffer;
        resendBuffer.Printf("%c%c%02d%02d%02d%02d%02d%02d", Type, EType,
                            beginHourValue, beginMinuteValue, beginSecondValue,
                            endHourValue, endMinuteValue, endSecondValue);
        UFC::FileStreamEx File(START_TIME_FILENAME, "w+");  // "w+" = O_CREAT|O_TRUNC|O_RDWR
        File.Write(resendBuffer.c_str(), resendBuffer.Length());        
        printf( "\n\nAsk running KGIExecConnection resend\n" );
        UFC::AnsiString ETStr;
        switch(EType)
        {
            case 'A': ETStr = " ";        break;
            case 'C': ETStr = " Confirm"; break;
            case 'F': ETStr = " Filled";  break;
        }
        
        switch(Type)
        {
            case 'A': printf("[All]%s Executions\n",     ETStr.c_str()); break;
            case 'P': printf("[PATS]%s Executions\n",    ETStr.c_str()); break;
            case 'O': printf("[Options]%s Executions\n", ETStr.c_str()); break;
            case 'F': printf("[Futures]%s Executions\n", ETStr.c_str()); break;
            case 'S': printf("[TSE]%s Executions\n",     ETStr.c_str()); break;
            case 's': printf("[OTC]%s Executions\n",     ETStr.c_str()); break;
        }
        if (endTimeStr.Length() > 0)
            printf("from [%02d:%02d:%02d] to [%02d:%02d:%02d]\n",
                   beginHourValue, beginMinuteValue, beginSecondValue,
                   endHourValue, endMinuteValue, endSecondValue);
        else
            printf(" from [%02d:%02d:%02d]~[%02d:%02d:%02d]\n", beginHourValue, beginMinuteValue, beginSecondValue, endHourValue, endMinuteValue, endSecondValue);
        printf(" Resend File[%s]\n", START_TIME_FILENAME);
        printf("--------------------------------------------------\n");
    }
    
    
/*
    Int32 Time = TimeStr.ToInt();
    if( Time == 0 )
    {
        printf( " Wrong time string[%s].\n", TimeStr.c_str() );
        return;
    }
    if( ( Time / 10000 ) > 23 || ( Time / 10000 ) < 1 )
    {
        printf( " Hour should between 0 to 23.\n" );
        return;
    }
    if( ( ( Time % 10000 ) / 100) > 59 )
    {
        printf( " Minutes should between 0 to 59.\n" );
        return;
    }
    UFC::FileStreamEx File( START_TIME_FILENAME, "w+" );  // "w+" = O_CREAT|O_TRUNC|O_RDWR
    UFC::AnsiString   DataStr, ETStr;
    
    DataStr.Printf("%06d%c%c", Time, Type, EType );
    File.Write( DataStr.c_str() , 9 );
    
    printf( "\n\nAsk running KGIExecConnection resend\n" );
    switch( EType )
    {
        case 'A': ETStr = " ";        break;
        case 'C': ETStr = " Confirm"; break;
        case 'F': ETStr = " Filled";  break;
    }
    switch( Type )
    {
        case 'A': printf( "[All]%s Executions\n",     ETStr.c_str() ); break;
        case 'P': printf( "[PATS]%s Executions\n",    ETStr.c_str() ); break;
        case 'O': printf( "[Options]%s Executions\n", ETStr.c_str() ); break;
        case 'F': printf( "[Futures]%s Executions\n", ETStr.c_str() ); break;
        case 'S': printf( "[TSE]%s Executions\n",     ETStr.c_str() ); break;
        case 's': printf( "[OTC]%s Executions\n",     ETStr.c_str() ); break;
    }
    printf( "From[%02d:%02d:%02d]\n", Time / 10000, (Time / 100) % 100, Time % 100 );
    printf( "--------------------------------------------------\n" );
 */
}    
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{    
    KGIExecConnection::FIsTWSERECVNewVersion = true;
    KGIExecConnection::FIsTWSESENDNewVersion = true;
    GetRecoverBeginTime();
    try
    {
        for( int i = 1; i < argc; i++ )
        {
            UFC::AnsiString curParam( argv[i] );
            UFC::AnsiString curUpperParam = curParam.UpperCase();
            if( curUpperParam == "-H" )
            {
                PrintHelp();
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RTFC") != -1 )
            {
                if( curParam.Length() > 5 )
                {      
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 5) );
                    Resend( TimeStr, 'F', 'C' );
                }
                else
                {
                    printf( "\nResend TAIFEX Futures confirm execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RTFC123059\n");
                }
                exit( 0 );                    
            }
            else if( curUpperParam.AnsiPos("-RTOC") != -1 )
            {
                if( curParam.Length() > 5 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 5) );
                    Resend( TimeStr, 'O', 'C' );
                }
                else
                {                    
                    printf( "\nResend TAIFEX Options confirm execution from HHMMSS failed.\n");                    
                    printf( "Example:KGIExecConnection -RTOC123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RTFF") != -1 )
            {
                if( curParam.Length() > 5 )
                {      
                     UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 5) );
                     Resend( TimeStr, 'F', 'F' );
                }
                else
                {
                    printf( "\nResend TAIFEX Futures filled execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RTFF123059\n");
                }
                exit( 0 );                    
            }
            else if( curUpperParam.AnsiPos("-RTOF") != -1 )
            {
                if( curParam.Length() > 5 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 5) );
                    Resend( TimeStr, 'O', 'F' );
                }
                else
                {                    
                    printf( "\nResend TAIFEX Options filled execution from HHMMSS failed.\n");                    
                    printf( "Example:KGIExecConnection -RTOF123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RSTC") != -1 )
            {
                if( curParam.Length() > 5 )
                {      
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 5) );
                    Resend( TimeStr, 'S', 'C' );
                }
                else
                {
                    printf( "\nResend TSE confirm execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RSTC123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RSOC") != -1 )
            {
                if( curParam.Length() > 5 )
                {      
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 5) );
                    Resend( TimeStr, 's', 'C' );
                }
                else
                {
                    printf( "\nResend OTC confirm execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RSOC123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RSTF") != -1 )
            {
                if( curParam.Length() > 5 )
                {      
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 5) );
                    Resend( TimeStr, 'S', 'F' );
                }
                else
                {
                    printf( "\nResend TSE filled execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RSTF123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RSOF") != -1 )
            {
                if( curParam.Length() > 5 )
                {      
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 5) );
                    Resend( TimeStr, 's', 'F' );
                }
                else
                {
                    printf( "\nResend OTC filled execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RSOF123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RPC") != -1 )
            {
                if( curParam.Length() > 4 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 4 ) );
                    Resend( TimeStr, 'P', 'C' );
                }
                else
                {                    
                    printf( "\nResend PATS confirm execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RPC123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RTF") != -1 )
            {
                if( curParam.Length() > 4 )
                {      
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 4) );
                    Resend( TimeStr, 'F', 'A' );
                }
                else
                {
                    printf( "\nResend TAIFEX Futures execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RTF123059\n");
                }
                exit( 0 );                    
            }
            else if( curUpperParam.AnsiPos("-RTO") != -1 )
            {
                if( curParam.Length() > 4 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 4) );
                    Resend( TimeStr, 'O', 'A' );
                }
                else
                {                    
                    printf( "\nResend TAIFEX Options execution from HHMMSS failed.\n");                    
                    printf( "Example:KGIExecConnection -RTO123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RST") != -1 )
            {
                if( curParam.Length() > 4 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 4) );
                    Resend( TimeStr, 'S', 'A' );
                }
                else
                {
                    printf( "\nResend TSE execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RST123059\n");
                }
                exit( 0 );                    
            }
            else if( curUpperParam.AnsiPos("-RSO") != -1 )
            {
                if( curParam.Length() > 4 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 4) );
                    Resend( TimeStr, 's', 'A' );
                }
                else
                {
                    printf( "\nResend OTC execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RSO123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RPF") != -1 )
            {
                if( curParam.Length() > 4 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 4 ) );
                    Resend( TimeStr, 'P', 'F' );
                }
                else
                {                    
                    printf( "\nResend PATS filled execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RPF123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RP") != -1 )
            {
                if( curParam.Length() > 3 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 3 ) );
                    Resend( TimeStr, 'P', 'A' );
                }
                else
                {                    
                    printf( "\nResend PATS execution from HHMMSS failed.\n");
                    printf( "Example:KGIExecConnection -RP123059\n");
                }
                exit( 0 );
            }            
            else if( curUpperParam.AnsiPos("-RC") != -1 )
            {
                if( curParam.Length() > 3 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 3) );
                    Resend( TimeStr, 'A', 'C' );
                }
                else
                {                    
                    printf( "\nResend all confirm execution from HHMMSS failed.\n");                                        
                    printf( "Example:KGIExecConnection -RC123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-RF") != -1 )
            {
                if( curParam.Length() > 3 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 3) );
                    Resend( TimeStr, 'A', 'F' );
                }
                else
                {                    
                    printf( "\nResend all filled execution from HHMMSS failed.\n");                                        
                    printf( "Example:KGIExecConnection -RF123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam.AnsiPos("-R") != -1 )
            {
                if( curParam.Length() > 2 )
                {
                    UFC::AnsiString TimeStr( (const char*)(curParam.c_str() + 2) );
                    Resend( TimeStr, 'A', 'A' );
                }
                else
                {                    
                    printf( "\nResend all execution from HHMMSS failed.\n");                                        
                    printf( "Example:KGIExecConnection -R123059\n");
                }
                exit( 0 );
            }
            else if( curUpperParam == "-F" )
            {
                FRecoverBeginTime = 053000;///< 5:30:00
            }
            else if( curUpperParam == "-D" )
            {
                DumpOnly = TRUE;
            }
            else if( curUpperParam == "-US" )
            {
                gIsUSMapper = true;
            }
            else if( curUpperParam == "-TWSERECVOLD" )
            {
                KGIExecConnection::FIsTWSERECVNewVersion = false;
            }
            else if( curUpperParam == "-TWSESENDOLD" )
            {
                KGIExecConnection::FIsTWSESENDNewVersion = false;
            }
        }             
    }
    catch( UFC::Exception& ex )
    {
        printf( ex.what() );
        PrintHelp();
    }
    
    if (KGIExecConnection::FIsTWSERECVNewVersion && !KGIExecConnection::FIsTWSESENDNewVersion)
    {
        printf( "Can not Receive New Version Report and Send Old One!\n" );
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
    char Buffer[16];
    UFC::FileStreamEx File( START_TIME_FILENAME, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
    
    FTime.setCurrent();
    sprintf( Buffer, "AA%02d%02d%02d000000", FTime.getHour(), FTime.getMinute(), FTime.getSecond() );    
    File.Write( Buffer , 14 );    
}    
//------------------------------------------------------------------------------
BOOL GetRecoverTime( UFC::AnsiString& BeginTimeStr, UFC::AnsiString& EndTimeStr, char& Type, char& EType  )
{
    UFC::FileStreamEx File(START_TIME_FILENAME, "r+");
    char Buffer[ 16 ];
    
    if( File.Read( Buffer , 14 ) == 14 ) ///< Time:6 Type:1 ExecType:1 total:14
    {
        Buffer[14] = 0;
        Type = Buffer[0 ];
        EType = Buffer[ 1 ];
        BeginTimeStr = UFC::AnsiString(Buffer + 2, 6);
        EndTimeStr = UFC::AnsiString(Buffer + 8, 6);
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    CheckArgs( argc, argv );
    
    MessageObj = new MessageObject( "KGIExecConnection", "1.0", "XXXXX" );   
    MessageObj->SetMonitorListener( new ProcessMonitor() );    
    MessageObj->Start(); ///< Start Migo Message pump.    
    MessageObj->WaitForConnected();

    UFC::UDateTime today;
    UFC::AnsiString logFileName;
    logFileName.Printf("../log/KGIExec%04d%02d%02d.log", today.getYear(), today.getMonth(), today.getDate());
    UFC::BufferedLog::SetLogObject(new UFC::BufferedLog( logFileName, 1024, TRUE));
    
    KGIExecConnection ExecConnection( FRecoverBeginTime.ToInt32() );
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
            UFC::AnsiString beginTimeStr = "", endTimeStr = "";
            char Type, EType;
            UFC::BufferedLog::Printf( " ----- Receive Recover Event -----" );    
            if( GetRecoverTime( beginTimeStr, endTimeStr, Type ,EType ) == TRUE )
                ExecConnection.Recover( beginTimeStr, endTimeStr, Type ,EType );        
        }
    }    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
