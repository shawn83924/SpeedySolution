//------------------------------------------------------------------------------
#include <stdlib.h>
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
#include "../../Migo/Sigo.h"
//------------------------------------------------------------------------------
BOOL                DebugOut = FALSE;
BOOL                isFisrt = FALSE;
UInt32              FRecoverBeginItem = 0;
MessageObject*      MessageObj;
UFC::AnsiString     FMessageIP;
UFC::AnsiString     FMessageHOSTNAME;
UFC::AnsiString     FMHOKDataFileName;
UFC::AnsiString     FLINE_BROKERID;
//------------------------------------------------------------------------------
typedef struct body_s{//62
    Int8  STK_NO[6];
    Int8  MTHQTY[8];
    Int8  MTHPR[6];
    Int8  MTHTIME[8];
    Int8  EXCD[1];
    Int8  BUY_SELL[1];
    Int8  ODRNO[5];
    Int8  IVACNO[7];
    Int8  ODRTPE[1];
    Int8  SEQ_NO[6];
    Int8  BROKER_ID[4];
    Int8  RECNO[8];
    Int8  MARK_S[1];
} body;
typedef struct mh_data_s {
    Int8  TDATE[2];
    Int8  TYPE[1];
    Int8  FILLER;
    Int8  LINE[2];
    Int8  FNCODE[2];
    body  BODY;
    Int8  END;
} mh_data;
        
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
        UFC::BufferedLog::Printf( " ***** Process SpeedyManager already exists *****"  );
        sleep( 2 );
        exit( 0 );
    }
}
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "KGIExecConnection Copyright (c) September 2007 MDBS\n" );
    printf( "    -F         First time start up.\n" );
    printf( "    -D         Debog mode\n" );
    exit( 1 );
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{    
    try
    {
        for( int i = 1; i < argc; i++ )
        {
            UFC::AnsiString Param( argv[i] );
            if( Param == "-H" )
            {
                PrintHelp();
                exit( 0 );
            }
            else if( Param == "-F" )
            {
                isFisrt = TRUE;
            }
            else if( Param == "-D" )
            {
                DebugOut = TRUE;
            }
        }             
    }
    catch( UFC::Exception& ex )
    {
        printf( ex.what() );
        PrintHelp();
    }
}
//------------------------------------------------------------------------------
void LoadSetting(void)
{
    try
    {
        UFC::UiniFile Config("../cfg/FubonDBMHSender.cfg");
        UFC::AnsiString FMHOKFilePath;
        if (Config.GetValue("Setting", "FilePath", FMHOKFilePath) == FALSE)
        {
            UFC::BufferedLog::Printf(" Setting FilePath not set.");
            exit(1);
        }
        if (Config.GetValue("Setting", "IP", FMessageIP) == FALSE)
        {
            UFC::BufferedLog::Printf(" Setting IP not set.");
            exit(1);
        }
        if (Config.GetValue("Setting", "HOSTNAME", FMessageHOSTNAME) == FALSE)
        {
            UFC::BufferedLog::Printf(" Setting HOSTNAME not set.");
            exit(1);
        }
        if (Config.GetValue("Setting", "LINE_BROKERID", FLINE_BROKERID) == FALSE)
        {
            UFC::BufferedLog::Printf(" Setting LINE_BROKERID not set.");
            exit(1);
        }
        UFC::UDateTime DateTime;
        FMHOKDataFileName.Printf("%s/%s%02d%02d.%02d", FMHOKFilePath.c_str(), "MHOK", DateTime.getMonth(), DateTime.getDate(), DateTime.getYear() - 1911);
    }
    catch (...)
    {
        UFC::BufferedLog::Printf(" FubonDBMHSender.cfg not found.");
        exit(1);
    }
}
//------------------------------------------------------------------------------
void updateReadSEQFile(UInt32 item)
{
    FRecoverBeginItem = item;
    UFC::FileStreamEx FMHOKDataReadSEQFile("MHRead.seq", "w+");
    UFC::AnsiString nowtime(UFC::GetDateString());
    nowtime.AppendPrintf("\n%d\n", item);
    FMHOKDataReadSEQFile.Write(nowtime.c_str(),nowtime.Length());
    FMHOKDataReadSEQFile.Flush();
    FRecoverBeginItem = item;
}
void CreateReadSEQFile(void)
{
    BOOL needreset = TRUE;
    UFC::AnsiString nowtime(UFC::GetDateString());
    try
    {
        UFC::FileStreamEx FMHOKDataReadSEQFile("MHRead.seq", "r");
        if( isFisrt == false )
        {
            UFC::AnsiString FileTime;
            FMHOKDataReadSEQFile.ReadLine(FileTime);
            if( nowtime.AnsiNCompare(FileTime,6) == 0 )
            {
                UFC::AnsiString BeginItem;
                FMHOKDataReadSEQFile.ReadLine(BeginItem);
                FRecoverBeginItem = BeginItem.ToUInt();
                needreset = FALSE;
                UFC::BufferedLog::Printf(" BeginItem %d",FRecoverBeginItem);
            }
        }
    }
    catch(...)
    {
    }
    if( needreset )
    {
        UFC::FileStreamEx FMHOKDataReadSEQFile("MHRead.seq", "w+");
        UFC::BufferedLog::Printf(" reset ReadSEQFile");
        nowtime.AppendPrintf("\n0\n");
        FMHOKDataReadSEQFile.Write(nowtime.c_str(),nowtime.Length());
        FMHOKDataReadSEQFile.Flush();
    }
}
void ReadMHSendMseeage(void)
{
    try
    {
        UFC::FileStreamEx file(FMHOKDataFileName.c_str(), "r");
        int size = file.GetSize();
        UInt32 itemsize = 70;
        UInt32 items = size / itemsize;
        if( items > FRecoverBeginItem )
        {
            file.Seek(FRecoverBeginItem*itemsize,SEEK_SET);
            UFC::BufferedLog::Printf(" get File %s begin=%d end=%d", FMHOKDataFileName.c_str(), FRecoverBeginItem, items);
            int count = 0;
            char* Subject;
            UFC::AnsiString str;
            mh_data data;
            UInt32 bodysize = 62;
            for( UInt32 i = FRecoverBeginItem; i < items ; i++ )
            {
                file.Read(&data,itemsize);
                data.END = 0;
                str.Copy(data.FNCODE,2);
                if( str.AnsiCompare("20") != 0 )
                {
                    MTree Data;
                    /*str.Copy(data.STK_NO,6);
                    Data.append("STK-NO",str.c_str());
                    str.Copy(data.MTHQTY,8);
                    Data.append("MTHQTY",str.c_str());
                    str.Copy(data.MTHPR,6);
                    Data.append("MTHPR",str.c_str());
                    str.Copy(data.MTHTIME,8);
                    Data.append("MTHTIME",str.c_str());
                    str.Copy(data.EXCD,1);
                    Data.append("EXCD",str.c_str());
                    str.Copy(data.BUY_SELL,1);
                    Data.append("BUY-SELL",str.c_str());
                    str.Copy(data.ODRNO,5);
                    Data.append("ODRNO",str.c_str());
                    str.Copy(data.IVACNO,7);
                    Data.append("IVACNO",str.c_str());
                    str.Copy(data.ODRTPE,1);
                    Data.append("ODRTPE",str.c_str());
                    str.Copy(data.SEQ_NO,6);
                    Data.append("SEQ-NO",str.c_str());
                    str.Copy(data.BROKER_ID,4);
                    Data.append("BROKER-ID",str.c_str());
                    str.Copy(data.RECNO,8);
                    Data.append("RECNO",str.c_str());
                    str.Copy(data.MARK_S,1);
                    Data.append("MARK-S",str.c_str());
                    str.Printf("Data[%d]",i);
                    */
                    str.Copy((char*)&data.BODY,bodysize);
                    Data.append("FILL_ORDER",str.c_str());
                    str.Copy(data.LINE,2);
                    Data.append("PVC",str.c_str());
                    Data.append("PBNO",FLINE_BROKERID.c_str());
                    if( data.TYPE[0] == '1' )
                    {
                        Subject = "SIMFILL.TSE";
                    }
                    else if( data.TYPE[0] == '2' )
                    {
                        Subject = "SIMFILL.OTC";
                    }
                    else
                    {
                        Subject = "SIMFILL";
                    }
                    str.Printf("Send Subject[%s],Key[%s],",Subject,FMessageHOSTNAME.c_str());
                    for( Int32 j = 0 ; j < Data.getNodeCount() ; j++ )
                    {
                        MNode* node = Data.get( j );
                        str.AppendPrintf("%s=[%s],", node->getName().c_str(),(char *)node->getData());
                    }
                    //if( DumpOnly == FALSE )
                    //{
                        if( MessageObj->Send(Subject,FMessageHOSTNAME,Data) )
                        {
                            count++;
                            //if( DebugOut == TRUE )
                            {
                                UFC::BufferedLog::DebugPrintf(" %s OK",str.c_str());
                            }
                        }
                        else
                        {
                            UFC::BufferedLog::Printf(" %s Error",str.c_str());
                        }
                    //}
                    /*else
                    {
                        UFC::BufferedLog::Printf(" %sEND",str.c_str());
                    }*/
                }
                updateReadSEQFile(i+1);
            }
            if( count > 0 )
            {
                UFC::BufferedLog::Printf(" SendOK[%d]",count);
            }
        }
    }
    catch (const exception& e)
    {
        UFC::BufferedLog::Printf(" get File %s error %s", FMHOKDataFileName.c_str(), e.what());
    }
}
//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    UFC::AnsiString LogName = "../log/FubonDBMH" + UFC::GetDateString() + ".log";
    try
    {
        UFC::BufferedLog* log = new UFC::BufferedLog(LogName,1024,TRUE);
        UFC::BufferedLog::SetLogObject(log);
    }
    catch (const exception& e)
    {
        UFC::BufferedLog::Printf(" log File %s error", LogName.c_str(), e.what());
    }
    
    CheckArgs( argc, argv );
    LoadSetting();

    MessageObj = new MessageObject( "FubonMHSender", "1.0", "XXXXX" );
    MessageObj->SetHost(FMessageIP);
    MessageObj->SetMonitorListener( new ProcessMonitor() );    
    MessageObj->Start(); ///< Start Migo Message pump.    
    MessageObj->WaitForConnected();            
    
    CreateReadSEQFile( );
    
    if( isFisrt == TRUE )
        UFC::BufferedLog::Printf(" Set to fisrt time");
    //if( DumpOnly == TRUE ) 
    //    UFC::BufferedLog::Printf( " Dump to screen only.");
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   FubonDB backend interface module           " );
    UFC::BufferedLog::Printf( "   startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "   Server IP: %s.             ", FMessageIP.c_str() );
    UFC::BufferedLog::Printf( "   Server HOSTNAME: %s.             ", FMessageHOSTNAME.c_str() );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DebugOut == TRUE ) 
    {
        UFC::BufferedLog::Printf( "   Debug Mode:On");    
        UFC::BufferedLog::SetDebugMode(TRUE);
    }
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");
    UFC::BufferedLog::Printf( "__________________________________________" );       

    while( TRUE )
    {
        UFC::SleepMS( 50 );
        ReadMHSendMseeage();
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
    }
    
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
