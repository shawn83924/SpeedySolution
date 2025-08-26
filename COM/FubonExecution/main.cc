// 
// File:   main.cc
// Author: yuan
//
// Created on November 13, 2007, 4:50 PM
//
//------------------------------------------------------------------------------
#include "FillExecution.h"
#include "Sysque.h"
//------------------------------------------------------------------------------
UFC::AnsiString   HOST_NAME;
UFC::AnsiString   HOST_IP;
UFC::AnsiString   SYSQUEUE_NAME;
UFC::AnsiString   CONFIG_FILE_NAME;
UFC::AnsiString   LOG_PATH,LockPath;
UFC::AnsiString   TODAY;
UFC::AnsiString   FSentLineFileName( "../cfg/SentLineNo.ini" );
UFC::AnsiString   FAppendName( "Def" );
BOOL              TWSENewVersion = TRUE;
//------------------------------------------------------------------------------
UFC::TFixedFormat FTSER3Format;
UFC::TParseData   FTSER3Parser;
UFC::TFixedFormat FOTCR3Format;
UFC::TParseData   FOTCR3Parser;
//------------------------------------------------------------------------------
UFC::UiniFile*    FSentLineNoTable = NULL;
MessageObject*    MessageObj       = NULL;
TSysque*          FFilledQueue     = NULL;
//------------------------------------------------------------------------------
UFC::PEvent       ProcessExistEvent;
//----------------------------------------------------------------------------------------------------------------------
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
void FillExecutionBase( BOOL IsTSEC, UFC::TRecord* pRecord,TSQMessage& SQMessage  )
{
    UFC::AnsiString BrokerID,Account,Qty,Value;
    
    ///< PartiallyFilled or Filled
    SQMessage.Append( "00", "4" ); ///< MsgType
    ///< Add CompanyID and BranchID.
    if( pRecord->GetField( "BrokerID", BrokerID ) == TRUE )        
    {
        UFC::AnsiString CompanyID = BrokerID.SubString( 0, 2 );
        UFC::AnsiString BranchID = BrokerID.SubString( 2, 2 );
        SQMessage.Append( "08", CompanyID );
        SQMessage.Append( "09", BranchID );
    }
    else
        UFC::BufferedLog::Printf( " Parse error: BrokerID not found." );
    ///< Add the market.
    if( IsTSEC == TRUE )
        SQMessage.Append( "13", "T" );///< For TSEC
    else
        SQMessage.Append( "13", "O" ); ///< For OTC        
    ///< Add Account and checksum
    if( pRecord->GetField( "Account", Account ) == TRUE )
    {
        UFC::AnsiString AccountCode, AccountChecksum;
        AccountCode.Printf( "%06d", Account.ToInt() / 10 );
        AccountChecksum.Printf( "%d", Account.ToInt() % 10 );
    
        SQMessage.Append( "17", AccountCode );
        SQMessage.Append( "18", AccountChecksum );
    }
    else
        UFC::BufferedLog::Printf( " Parse error: Account not found." );
    ///< Add Symbol.
    if( pRecord->GetField( "Symbol", Value ) )
    {
        Value.TrimRight();
        SQMessage.Append( "29", Value );		
    }
    else
        UFC::BufferedLog::Printf( " Parse error: Symbol not found." );    
    ///< Add Side.
    if( pRecord->GetField( "Side", Value ) )
        SQMessage.Append( "32", Value );
    else    
        UFC::BufferedLog::Printf( " Parse error: Side not found." );            
    ///< Add OrderID
    if( pRecord->GetField( "OrderID", Value ) )
	SQMessage.Append( "36", Value );
    else
	UFC::BufferedLog::Printf( " Parse error: OrderID not found." );            
    ///< Add TSEC order type.
    if( pRecord->GetField( "TSEOrderType", Value ) )    
        SQMessage.Append( "27", Value );    
    ///< Add Price
    if( pRecord->GetField( "Price", Value ) )    
    {
        if( TWSENewVersion == TRUE )
            SQMessage.Append( "48", UFC::IntToDouble( Value.ToInt(), 4 ) );    
        else
            SQMessage.Append( "48", UFC::IntToDouble( Value.ToInt(), 2 ) );    
    }
    else
        UFC::BufferedLog::Printf( " Parse error: Price not found." );         
    ///< Process time
    if( pRecord->GetField( "ProcessTime", Value ) )
        SQMessage.Append( "51", Value );
    else
	UFC::BufferedLog::Printf( " Parse error: ProcessTime not found." );            
    SQMessage.Append( "50", TODAY );
    
    if( pRecord->GetField( "EXCD", Value )  && pRecord->GetField( "Qty", Qty ) )
    {
        if( Value == "2" )
        {
            SQMessage.Append( "O3", "2" ); ///< OddLot
            SQMessage.Append( "47", (long)Qty.ToInt() );
        }
        else
        {
            SQMessage.Append( "O3", "0" ); ///< Normal or OffHour 
            SQMessage.Append( "47", (long)Qty.ToInt() );
        }        
    }
    SQMessage.Append( "24", "q" ); ///< q means from FubonExecution
}   
//------------------------------------------------------------------------------
void TSEFillListener::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* pTree )
{
    UFC::AnsiString ExecuteMessage, AE, Seq, RecNo, PBNO, PVC,LineNo,SysExecMsg,LKey;
    UFC::TRecord*   pRecord;
    Int32           NID;

    if( pTree->get( "FILL_ORDER", ExecuteMessage ) &&
	pTree->get( "NID", NID ) &&
	pTree->get( "KEY", AE ) &&
	pTree->get( "PBNO", PBNO ) &&
	pTree->get( "PVC", PVC ) )
    {
        UFC::BufferedLog::Printf( " TWSE R3[%s]", ExecuteMessage.c_str() );
        pRecord = FTSER3Parser.Parse( ExecuteMessage.c_str(), ExecuteMessage.Length() );	
	if( pRecord !=NULL && pRecord->GetField( "Seq", Seq ) && pRecord->GetField( "RecNo", RecNo ) )
	{
            TSQMessage SQMessage;            
            
            FillExecutionBase( TRUE, pRecord, SQMessage );            
            SQMessage.Append( "72", RecNo );    
            SQMessage.Append( "Q5", Seq );
            ///< Which PVCID received thie fill.
            if( PVC.Length() > 0 ) 
                SQMessage.Append( "VI", PVC.c_str() );
            else
                SQMessage.Append( "VI", "xx" );    	            
            ///< Line No processed this order.
            LKey.Printf( "%s_T", PBNO.c_str() );            
            if( FSentLineNoTable->GetValue( HOST_NAME, LKey, LineNo ) )
                SQMessage.Append( "VH", LineNo.c_str() );
            else
                SQMessage.Append( "VH", "XX" ); 
            SysExecMsg = SQMessage.ToString();
            UFC::BufferedLog::Printf( " TWSE Exec from Host[%s] Broker[%s] Key[%s].", HOST_NAME.c_str(), PBNO.c_str(), LKey.c_str() );                        
            UFC::BufferedLog::Printf( " To Backend[%s]", SysExecMsg.c_str() );            
            FFilledQueue->Write( (char*)(SysExecMsg.c_str()), SysExecMsg.Length() );
	}
    }    
}
//------------------------------------------------------------------------------
void OTCFillListener::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* pTree )
{
    UFC::AnsiString ExecuteMessage, AE, Seq, RecNo, PBNO, PVC,LineNo,SysExecMsg,LKey;
    UFC::TRecord*   pRecord;
    Int32           NID;

    if( pTree->get( "FILL_ORDER", ExecuteMessage ) &&
	pTree->get( "NID", NID ) &&
	pTree->get( "KEY", AE ) &&
	pTree->get( "PBNO", PBNO ) &&
	pTree->get( "PVC", PVC ) )
    {
        UFC::BufferedLog::Printf( " OTC R3[%s]", ExecuteMessage.c_str() );        
	pRecord = FOTCR3Parser.Parse( ExecuteMessage.c_str(), ExecuteMessage.Length() );	
	if( pRecord !=NULL && pRecord->GetField( "Seq", Seq ) && pRecord->GetField( "RecNo", RecNo ) )
	{
            TSQMessage SQMessage;            
            
            FillExecutionBase( FALSE, pRecord, SQMessage );            
            SQMessage.Append( "72", RecNo );    
            SQMessage.Append( "Q5", Seq );
            ///< Which PVCID received thie fill.
            if( PVC.Length() > 0 ) 
                SQMessage.Append( "VI", PVC.c_str() );
            else
                SQMessage.Append( "VI", "xx" );    	            

            ///< Line No processed this order.
            LKey.Printf( "%s_O", PBNO.c_str() );
            if( FSentLineNoTable->GetValue( HOST_NAME, LKey, LineNo ) )
                SQMessage.Append( "VH", LineNo.c_str() );
            else
                SQMessage.Append( "VH", "XX" );                                                
            SysExecMsg = SQMessage.ToString();
            UFC::BufferedLog::Printf( " OTC Exec from Host[%s] Broker[%s] Key[%s].", HOST_NAME.c_str(), PBNO.c_str(), LKey.c_str() );            
            UFC::BufferedLog::Printf( " To Backend[%s]", SysExecMsg.c_str() );            
            FFilledQueue->Write( (char*)(SysExecMsg.c_str()), SysExecMsg.Length() );
	}
    }      
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char * argv[] )
{
    LOG_PATH = "./";
    CONFIG_FILE_NAME.Printf("%s.cfg",PROGRAM_NAME );
    for( int i = 1; i < argc; i++ )
    {
        UFC::AnsiString Param( argv[i] );
        if( Param == "-H" || Param == "-h" || Param == "-?" ) ///< Help screen
        {
            printf( "%s Copyright (c) 2003~%d MDBS\n",PROGRAM_NAME, UFC::ThisYear() );
            printf( "Usage: %s [options]\n",PROGRAM_NAME );
            printf( "   -H         Show this page.\n" );
            printf( "   -F[file]   Config file name.(Default:%s.cfg)\n",PROGRAM_NAME );            
            printf( "   -N[name]   Append name.(Default:%s)\n",FAppendName.c_str() );
            printf( "   -TWSEOLD   Use TWSE Old version.(Before 2020/3/02)\n" );            
            exit( 1 );
        }	
        else if( Param == "-TWSEOLD" || Param == "-twseold" )
        {
             TWSENewVersion = FALSE;
        }
        else if( Param.AnsiPos("-F") != -1 || Param.AnsiPos("-f") != -1 )
        {
             UFC::AnsiString PName( (const char*)(Param.c_str() + 2) );
             CONFIG_FILE_NAME = PName;
        }
        else if( Param.AnsiPos("-N") != -1 || Param.AnsiPos("-n") != -1 )
        {
             UFC::AnsiString AppName( (const char*)(Param.c_str() + 2) );
             FAppendName = AppName;
        }
    }
}
//------------------------------------------------------------------------------
void BuildSentLineNumberTable( void )
{
    try
    {   
        char Buffer[ 128 ];        
        UFC::AnsiString TmpIniFileName,CMD;
    
        TmpIniFileName.Printf( "./FE%s_%sTemp.ini",FAppendName.c_str(), HOST_NAME.c_str() );        
        UFC::FileStreamEx SentLineNoFile( FSentLineFileName, "r+" );
        UFC::RemoveFile( TmpIniFileName );
        FSentLineNoTable = new UFC::UiniFile( TmpIniFileName, TRUE );
        while( SentLineNoFile.Read( Buffer, 64 ) == 64 )
        {
            if( Buffer[3]!= 'Y' &&  Buffer[3]!= 'y'  && Buffer[3]!= '0' ) ///< Valid data record.
            {
                UFC::AnsiString Key;
                UFC::AnsiString Host( Buffer+4, 24);
                UFC::AnsiString BrokerID( Buffer+28, 4);
                UFC::AnsiString PVCID( Buffer+32, 2 );
                UFC::AnsiString Market( Buffer+34,1 );
                
                Host.TrimRight();
                if( Market[0] == '2' )                
                    Key.Printf( "%s_O", BrokerID.c_str() );
                else
                    Key.Printf( "%s_T", BrokerID.c_str() );
                FSentLineNoTable->SetValue( Host, Key, PVCID );                
            }            
        }                
        FSentLineNoTable->Save();
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " Can't open sent line number setting file:[%s]",FSentLineFileName.c_str() );
        exit(0);
    }    
}
//------------------------------------------------------------------------------
void LockFile( void )
{
    UFC::AnsiString LockFileName;
    LockFileName.Printf( "%sFubonExecution_%s_%s_lock", LockPath.c_str(), FAppendName.c_str(), HOST_NAME.c_str() );
    UFC::BufferedLog::DebugPrintf( " Try to open and lock file:[%s].", LockFileName.c_str() );
    
    int fd = open( LockFileName.c_str(), O_WRONLY | O_CREAT, 664 );//0600 );
    if( fd > 0 )
    {
        flock fl;
        fl.l_type   = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start  = 0;
        fl.l_len    = 0;
        fl.l_pid    = getpid();
        
        if( fcntl( fd, F_SETLKW, &fl ) == 0 )
            UFC::BufferedLog::Printf( " File:[%s] locked.", LockFileName.c_str() );
        else
        {
            UFC::BufferedLog::Printf( " Unable to lock file:[%s].", LockFileName.c_str() );
            exit(0);
        }
    }
    else
    {
        UFC::BufferedLog::Printf( " Can not open file:[%s]", LockFileName.c_str() );
        exit(0);
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
        if( Config.GetValue( "Setting", "Sysqueue",SYSQUEUE_NAME ) == FALSE )            
            throw UFC::Exception( "Setting \"Sysqueue\" not found." );        
        if( Config.GetValue( "Setting", "LockPath", LockPath )== FALSE )      
            throw UFC::Exception( "Setting \"LockPath\" not found." );        
        
        Config.GetValue( "Setting", "LogPath",LOG_PATH );                
        if( TWSENewVersion == TRUE )
        {
            if( UFC::FileExists("Temp/TSE_R030Ex.ini") == FALSE )
                throw UFC::Exception( "File \"Temp/TSE_R030Ex.ini\" not found." );
            FTSER3Format.SetIniFile( "Temp/TSE_R030Ex.ini");
            FTSER3Parser.SetDataFormat( &FTSER3Format );
            
            if( UFC::FileExists("Temp/OTC_R030Ex.ini") == FALSE )
                throw UFC::Exception( "File \"Temp/OTC_R030Ex.ini\" not found." );        
            FOTCR3Format.SetIniFile( "Temp/OTC_R030Ex.ini");        
            FOTCR3Parser.SetDataFormat( &FOTCR3Format );
        }
        else
        {
            if( UFC::FileExists( "Temp/TSE_R030.ini") == FALSE )
                throw UFC::Exception( "File \"Temp/TSE_R030.ini\" not found." );
            FTSER3Format.SetIniFile( "Temp/TSE_R030.ini");
            FTSER3Parser.SetDataFormat( &FTSER3Format );
            
            if( UFC::FileExists("Temp/OTC_R030.ini") == FALSE )
                throw UFC::Exception( "File \"Temp/OTC_R030.ini\" not found." );        
            FOTCR3Format.SetIniFile( "Temp/OTC_R030.ini");        
            FOTCR3Parser.SetDataFormat( &FOTCR3Format );            
        }
        UFC::GetYYYYMMDD( TODAY );
    }
    catch( UFC::Exception& e )
    {
        UFC::BufferedLog::Printf( " Exception:%s", e.what() );        
        exit( 0 );
    }
}
//------------------------------------------------------------------------------
int main(int argc, char** argv) 
{
    UFC::AnsiString LogFile;    
    UFC::AnsiString MBusName;
    
    CheckArgs( argc, argv );     ///< Parse command line arguments.
    LoadSetting();
    LogFile.Printf("%sFE%s_%s.%s.log",LOG_PATH.c_str(), FAppendName.c_str(), HOST_NAME.c_str(), UFC::GetDateString().c_str() );    
    MBusName.Printf( "FE%s@%s", FAppendName.c_str(), UFC::Hostname ); 
    
    UFC::BufferedLog Log( LogFile, 1024, TRUE );
    UFC::BufferedLog::SetLogObject( &Log );
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   Execution Listen Module,%s",PROGRAM_NAME );
    UFC::BufferedLog::Printf( "   Copyright 2004-%d by MDBS Software Inc.", UFC::ThisYear() );
    UFC::BufferedLog::Printf( "   All right reserved.                      ");
    UFC::BufferedLog::Printf( "   Startup at: %s.", UFC::Hostname );
    UFC::BufferedLog::Printf( "   Build Date:%s ", __DATE__ );		
    UFC::BufferedLog::Printf( "    " );		
    UFC::BufferedLog::Printf( "   Connect to server:[%s]", HOST_IP.c_str() );
    UFC::BufferedLog::Printf( "   Locol Hostname:[%s]", UFC::Hostname );
    UFC::BufferedLog::Printf( "   Server Hostname:[%s]", HOST_NAME.c_str() );
    UFC::BufferedLog::Printf( "   SysQueue Name:[%s]", SYSQUEUE_NAME.c_str() );
    UFC::BufferedLog::Printf( "   Log:[%s]", LogFile.c_str() );
    UFC::BufferedLog::Printf( "   AppName:[%s]", MBusName.c_str() );
    if( TWSENewVersion == TRUE )
        UFC::BufferedLog::Printf( "   TWSE New Version   :(after 2020/03/23)" );        
    else 
        UFC::BufferedLog::Printf( "   TWSE old Version   :(before 2020/03/23)" );            
    UFC::BufferedLog::Printf( "__________________________________________" );
    BuildSentLineNumberTable();     
    LockFile();
        
    FFilledQueue = new TSysque( "", 0, SYSQUEUE_NAME );
    if( FFilledQueue->Open() == FALSE )
    {
        UFC::BufferedLog::Printf( " Open Sysqueue[%s] failed.", SYSQUEUE_NAME.c_str() );        
        exit( 0 );    
    }
    MessageObj = new MessageObject( MBusName, "1.0", "XXXXX" );
    MessageObj->AddListener( "NOTSPEEDY.FILL.TSE", HOST_NAME, new TSEFillListener() );
    MessageObj->AddListener( "NOTSPEEDY.FILL.OTC", HOST_NAME, new OTCFillListener() );    
    MessageObj->SetMonitorListener( new ProcessMonitor() );    
    MessageObj->SetHost( HOST_IP );
    MessageObj->Start(); ///< Start Migo Message pump.    
    ProcessExistEvent.WaitFor( 30 );
    
    while( TRUE )
    {
        sleep( 5 );
        UFC::BufferedLog::FlushToFile();///< Force write the screen log to file.
    }
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
