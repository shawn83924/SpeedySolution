#include "RecoverThread.h"
#include "../TAIFEXLib/TMPDefines.h"
//---------------------------------------------------------------------------
extern UFC::BufferedLog* Glog;
//------------------------------------------------------------------------------
//  Format for recover executions
//  for Futures,Option,TSE and OTC
//------------------------------------------------------------------------------
//                                __ Key (AE for SpeedyGateway, Veriable length)
//                                |
//  Index:     0   |1|    2     | 3 |      4         |   5   |   6    |   7 
//  Field:   hhmmss|C|0123456789|Key|Exchange message|F006000|Hostname| PVCID ---- PVCID(Length:2)
//             |    |    |                 |            |_ Broker ID (Veriable length)  
//             |    |    |                 |_ Message from exchange. (Veriable length)
//             |    |    |_ Network ID. (Length:10)
//             |    |
//             |    |_ C/F: Confirm or fill execution. (Length:1)
//             |    
//             |_____ Time to write this log. (Length:6)
//
// Samples:
//
//  195217|C|0016860387|yuan|30010119521700F006000E100780232142FITX   20061200000000B20060000000100F0060000001000000008|S132000|MD1500
//  195217|F|0016860387|yuan|1F006000E100780232142FITX   20061200000000B20060000000050F0061952172800000006F|S132000|MD1500
//  195220|C|0016860397|yuan|30010119522000F006000E100880232142FITX   20061200000000B20060000000100F0060000001000000009|S132000|MD1500
//
// String Field 0: Time
// String Field 1: Type, confirm or fill.
// String Field 2: Network ID
// String Field 3: Key (AE)
// String Field 4: Exchange message
// String Field 5: Broker ID
//---------------------------------------------------------------------------
void APIRecoverExecution::SplitUserID( const UFC::AnsiString& String, UFC::AnsiString& AE )
{
    ///< Layout for the "KEY" field.
    ///< AE,Data
    Int32 SplitPos = String.AnsiPos(',');
    
    if( SplitPos !=  -1 )
        AE = String.SubString( 0, SplitPos );
    else
        AE = String;
}
//------------------------------------------------------------------------------
const unsigned int TIME_LENGTH = 6;
const unsigned int EXEC_LOG_MAX_LEN = 2048;
const int TYPE_POS    = 7;
const int AE_OFFSET   = 20;
//------------------------------------------------------------------------------
APIRecoverExecution::APIRecoverExecution(   const UFC::AnsiString& STime,
                                      const UFC::AnsiString& Key,
                                      const UFC::AnsiString& AE,
                                      Int32 CID,
                                      RecoverMarket Market,
                                      RecoverDataType DataType,
                                      RecoverSession SessionType,
                                      BOOL  IsAdmin,
                                      Int32 RecoverSpeed,
                                      TTaifexConnection* ConObj )
:UFC::PThread( NULL )
,FStartTime( STime )
,FEndTime( "475959" )
,FAE( AE )
,FKey( Key )
,FMarket( Market )
,FDataType( DataType )
,FSessionType( SessionType )
,FCID( CID )
,FCount( 0 )
,FAELength( FAE.Length() )
,FIsAdmin( IsAdmin )
,FExecOutCount( 0 )
,FFirstExecTick( 0 )
,FThroughputLimit( RecoverSpeed )
,FObj( ConObj )
{
	if( STime.IsEmpty())
		FStartTime = "0";
	FReplyKey.Printf( "%s%d", FAE.c_str(), FCID );
    Start();
}
//------------------------------------------------------------------------------
void APIRecoverExecution::RecoverIt( RecoverMarket Market, ExecutionParser& Exec )
{
    MTree Data;

    Data.append( "KEY",  Exec.GetKey() );
    Data.append( "NID",  Exec.GetNID() );
    Data.append( "PVC",  Exec.GetPVCID() );
    Data.append( "PBNO", Exec.GetPBNO() );
    Data.append( "PHOST",Exec.GetHostname());
    Data.append( "MARKET", (int)Market );
    Data.append( "OFFHOUR", (int)Exec.IsOffHour() );
    Data.append( "PossDup", 1 );
    if( Exec.IsBinary() == TRUE )
    {
        UInt8* BinData;
        Int32  Size;

        Data.append( "BIN", 1 );
        Exec.GetData( BinData, Size );
        if( Exec.IsConfirm() == TRUE ) ///< Order confirm
        {
            Data.append( "CONFIRM_ORDER", BinData, Size );
            Data.append( "TYPE", 0 ); ///< Confirm

        }
        else ///< Filled executions.
        {
			UFC::AnsiString OrderID( (char*)BinData + TMP_R02_ORDERID_OFFSET , 5 );
            
            Data.append( "FILL_ORDER", BinData, Size );
            Data.append( "OID", OrderID );
            Data.append( "TYPE", 1 ); ///< Execution Report
        }
    }
    else
    {
        Data.append( "TTIME", "000000000" );
        Data.append( "XTIME", "000000000" );
        Data.append( "BIN", 0 );
        if( Exec.IsTFXTMP()) ///< TMP rpt_seq field. for TMP only.
        {
            Data.append( "RPTSEQ", Exec.GetTMPSequence() );
            Data.append( "TMPExt", Exec.GetTMPExtFields() );
        }
        if( Exec.IsConfirm() == TRUE ) ///< Order confirm
        {
            Data.append( "CONFIRM_ORDER", Exec.GetData() );
            Data.append( "TYPE", 0 ); ///< Confirm
        }
        else ///< Filled executions.
        {
            UFC::AnsiString OrderID;
            if( Market == rmFutures || Market == rmOption )
            {
                switch( Exec.GetData()[0] )
                {
                    case '1':
                    case '2': OrderID  = Exec.GetData().SubString( 8, 5 );
                              break;
                    case '3':
                    case '4': OrderID  = Exec.GetData().SubString( 10, 5 );
                              break;
                }
            }
            else
                OrderID  = Exec.GetData().SubString( 30, 5 );
            Data.append( "FILL_ORDER", Exec.GetData() );
            Data.append( "OID", OrderID );
            Data.append( "TYPE", 1 ); ///< Execution Report
        }
    }
    if( FObj != NULL )
        FObj->ReceiveRecoverExecution( &Data  );     
}
//------------------------------------------------------------------------------
void  APIRecoverExecution::ControlTroughput( void )
{
    Int32 Now      = UFC::GetTickCountMS();
    Int32 TimeDiff = Now - FFirstExecTick;

    FExecOutCount ++;
    FCount ++;
    if( FFirstExecTick == 0  ) ///< First execution
    {
        FFirstExecTick = Now;
        return;
    }
    if( TimeDiff >= 1000 ) ///< more then 1 sec
    {
        FFirstExecTick = Now; ///< Reset timer.
        FExecOutCount  = 0;  ///< Reset counter.
    }
    else
    {
        if( FExecOutCount >= FThroughputLimit )
            UFC::SleepMS( 1000 - TimeDiff );
    }
}
//------------------------------------------------------------------------------
// The recover speedy is about 500 msg/sec
//------------------------------------------------------------------------------
void APIRecoverExecution::Recover( RecoverMarket Market, char* Buffer )
{
    try
    {        
        UFC::AnsiString LogAE;
        ExecutionParser Exec( Buffer );

        if( Exec.IsOffHour() == TRUE )
        {   ///< Recover Normal execution only.
            if( FSessionType == rsNormal )
                return;
        }
        else
        {   ///< Recover OffHour execution only.
            if( FSessionType == rsOffHour )
                return;
        }
        SplitUserID( Exec.GetKey(), LogAE );
        if( FAE == LogAE || FIsAdmin == TRUE )
        {    
            if( FDataType == rdBoth ) ///< Recover both confirm and fill executions.
            {
                RecoverIt( Market, Exec );
                ControlTroughput();
            }
            else if( FDataType == rdConfirm && Exec.IsConfirm() == TRUE )///< Its confirm execution.
            {
                RecoverIt( Market, Exec );
                ControlTroughput();
            }
            else if( FDataType == rdFill && Exec.IsConfirm() == FALSE )///< Its filled execution.
            {
                RecoverIt( Market, Exec );
                ControlTroughput();
            }            
        }
    }
    catch( UFC::Exception& e)
    {
        Glog->fprintf( " Parse Exception[%s]", e.what() );
        Glog->fprintf( " Error Execution[%s]", Buffer );
    }
}

/*
//------------------------------------------------------------------------------
void  RecoverExecution::ControlTroughput( void )
{
    Int32 Now      = UFC::GetTickCountMS();
    Int32 TimeDiff = Now - FFirstExecTick;

    FExecOutCount ++;
    FCount ++;
    if( FFirstExecTick == 0  ) ///< First execution
    {
        FFirstExecTick = Now;
        return;
    }
    if( TimeDiff >= 1000 ) ///< more then 1 sec
    {
        FFirstExecTick = Now; ///< Reset timer.
        FExecOutCount  = 0;  ///< Reset counter.
    }
    else
    {
        if( FExecOutCount >= FThroughputLimit )
            UFC::SleepMS( 1000 - TimeDiff );
    }
}
//------------------------------------------------------------------------------
// The recover speedy is about 500 msg/sec
//------------------------------------------------------------------------------
void RecoverExecution::Recover( RecoverMarket Market, char* Buffer )
{
    try
    {        
        UFC::AnsiString LogAE;
        ExecutionParser Exec( Buffer );
        
        if( Exec.IsOffHour() == TRUE )
        {   ///< Recover Normal execution only.
            if( FSessionType == rsNormal )
                return;
        }
        else
        {   ///< Recover OffHour execution only.
            if( FSessionType == rsOffHour || FSessionType == rsPrevOffHour )
                return;
        }    
        GetUserID( Exec.GetKey(), LogAE );
        if( FAE == LogAE || FIsAdmin == TRUE )
        {    
            if( FDataType == rdBoth ) ///< Recover both confirm and fill executions.
            {
                RecoverIt( Market, Exec );
                ControlTroughput();
            }
            else if( FDataType == rdConfirm && Exec.IsConfirm() == TRUE )///< Its confirm execution.
            {
                RecoverIt( Market, Exec );
                ControlTroughput();
            }
            else if( FDataType == rdFill && Exec.IsFill() == TRUE )///< Its filled execution.
            {
                RecoverIt( Market, Exec );
                ControlTroughput();
            }            
        }
    }
    catch( UFC::Exception& e)
    {
        UFC::BufferedLog::Printf( " Parse Exception[%s]", e.what() );
        UFC::BufferedLog::Printf( " Error Execution[%s]", Buffer );
    }
}
*/ 
//------------------------------------------------------------------------------
void APIRecoverExecution::GetRecoverFileName( RecoverMarket Market, UFC::AnsiString& RecoverFileName )
{
    UFC::AnsiString TradeDate;    
        
    UFC::GetTradeYYYMMDD( TradeDate );
    switch( Market )
    {
		case rmFutures:        RecoverFileName.Printf( "SpeedyFUT.Execution.%s.DATA", TradeDate.c_str() ); break;
		case rmOption:         RecoverFileName.Printf( "SpeedyOPT.Execution.%s.DATA", TradeDate.c_str() ); break;
		case rmTSE:            RecoverFileName.Printf( "SpeedyTSE.Execution.%s.DATA", TradeDate.c_str() ); break;
		case rmOTC:            RecoverFileName.Printf( "SpeedyOTC.Execution.%s.DATA", TradeDate.c_str() ); break;
		case rmForeignFutures:
		case rmForeignOptions:
		case rmForeignStock:   RecoverFileName.Printf( "SpeedyOS.Execution.%s.DATA",  TradeDate.c_str() ); break;
        default: return; ///< No matched market.
    }        
}
//------------------------------------------------------------------------------
void APIRecoverExecution::RecoverFromFile(  RecoverMarket Market )
{
    UFC::AnsiString RecoverFileName,TradeDate;    
    FILE*  FFileHandle;
    char   Buffer[ EXEC_LOG_MAX_LEN ];
    Int32  BeginTimeInt = FStartTime.ToInt();
    Int32  EndTimeInt   = FEndTime.ToInt();

    if( EndTimeInt < BeginTimeInt ) ///< Cross a day.
        EndTimeInt += 2400;    
    UFC::GetTradeYYYMMDD( TradeDate );
    GetRecoverFileName( Market, RecoverFileName );    
    if( (FFileHandle = fopen( RecoverFileName.c_str(), "r" )) != NULL )
    {     
         if( fgets( Buffer, EXEC_LOG_MAX_LEN, FFileHandle ) != NULL ) ///< Get the first execution.
         {             
            Int32 FirstExecTime = UFC::AnsiString(Buffer,TIME_LENGTH).ToInt();
            
            do
            {
                Int32 ExecTime = UFC::AnsiString(Buffer,TIME_LENGTH).ToInt();
                
                if( ExecTime < FirstExecTime )
                    ExecTime += 2400;
                if( ExecTime >= BeginTimeInt && ExecTime <= EndTimeInt )
                    Recover( Market, Buffer );
            }
			while( fgets( Buffer, EXEC_LOG_MAX_LEN, FFileHandle ) != NULL );
         }
         fclose( FFileHandle );
    }
    else
        Glog->fprintf(" Can't open Executions recover file %s", RecoverFileName.c_str() );
}
//------------------------------------------------------------------------------
void APIRecoverExecution::Execute( void )
{
    MTree  ReplyData;

    ///<  Sleep for a while.
    UFC::SleepMS( 50 + rand()%100 );

    ///< Begin recover.
    switch( FMarket )
    {
        case rmFutures:     RecoverFromFile( rmFutures );
                            break;
        case rmOption:      RecoverFromFile( rmOption );
                            break;
        case rmTSE:         RecoverFromFile( rmTSE );
                            break;
        case rmOTC:         RecoverFromFile( rmOTC );
                            break;
		case rmForeignFutures: RecoverFromFile( rmForeignFutures );
							   break;
		case rmForeignOptions: RecoverFromFile( rmForeignOptions );
							   break;
		case rmForeignStock:   RecoverFromFile( rmForeignStock );
							   break;
		case rmAll:         RecoverFromFile( rmFutures );
							RecoverFromFile( rmOption );
                            RecoverFromFile( rmTSE );
                            RecoverFromFile( rmOTC );
							RecoverFromFile( rmForeignFutures );
							RecoverFromFile( rmForeignOptions );
							RecoverFromFile( rmForeignStock );
							break;
    }
    ///< Send OnRecoverFinished message.
    ///< COMMAND_RECOVER_DONE  = 10004;
    ReplyData.append( "CMD", 10004 );
    ReplyData.append( "COUNT", FCount );    
    if( FObj != NULL )
        FObj->ReceiveAdminMessage( &ReplyData );

    Glog->fprintf(" Key: %s ", FKey.c_str() );
    if( FEndTime == "475959" )
        Glog->fprintf(" Recover Market[%d] %d Executions from %s finished", FMarket, FCount, FStartTime.c_str() );
    else
        Glog->fprintf(" Recover Market[%d] %d Executions from %s to %s finished", FMarket, FCount, FStartTime.c_str(), FEndTime.c_str() );
}
//------------------------------------------------------------------------------
