//---------------------------------------------------------------------------
#include "TWSEOrderInfoStore.h"
#include "Speedy.h"
#include "../TSECLib/Messages.h"
#include "../ExchangeUtility/Utility.h"
//-----------------------------------------------------------------------------------------
extern BOOL          CachedLog;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//---------------------------------------------------------------------------
void GetBrokerIDFromOrder( const UFC::AnsiString& Order, 
                           UFC::AnsiString& BrokerID, 
                           const UFC::AnsiString& DefaultBroker, 
                           int& Func, TMessageType& MsgType )
{
    MessageHeader*   Msg = MessageHeader::CreateMessage( Order.c_str(), Order.Length());

    if( Msg != NULL )
    {        
        Func = Msg->GetFunctionCode();
        MsgType = Msg->MessageType();
        switch( MsgType )
        {
           case mtT020:  BrokerID = UFC::AnsiString( ((MessageT020*)Msg)->BrokerID, 4 );break;                         
           case mtO020:  BrokerID = UFC::AnsiString( ((MessageO020*)Msg)->BrokerID, 4 );break;                         
           case mtO120:  BrokerID = UFC::AnsiString( ((MessageO120*)Msg)->BrokerID, 4 );break;                                    
           case mtP020:  BrokerID = UFC::AnsiString( ((MessageP020*)Msg)->BrokerID, 4 );break;                         
           case mtA020:  BrokerID = UFC::AnsiString( ((MessageA020*)Msg)->BrokerID, 4 );break;                         
           case mtE020:  BrokerID = UFC::AnsiString( ((MessageE020*)Msg)->BrokerID, 4 );break;                         
           case mtEx020: BrokerID = UFC::AnsiString( ((MessageEx020*)Msg)->BrokerID, 4 );break;            
           case mtV020:  BrokerID = UFC::AnsiString( ((MessageV020*)Msg)->BrokerID, 4 );break;                         
           default:      BrokerID = DefaultBroker;
        }
        delete Msg;
    }    
}
//---------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& OID, const UFC::AnsiString& BPNO, OrderRecord& OI )
:FNewOrderNID( OI.NID )
,FMsgType( OI.MsgType )    
,FFuncCode( OI.FuncCode )
,FLeavesQty(0)
,FLastPx(0.0)
{
    UFC::StrLCpy( FOrderID,   OID.c_str(),  MAX_OID_LEN );
    UFC::StrLCpy( FKeyString, OI.Key,       MAX_UDD_LEN );
    UFC::StrLCpy( FBPNO,      BPNO.c_str(), MAX_BRK_LEN );              
    if( strlen(OI.Hostname) > 0 )
        UFC::StrLCpy( FHost, OI.Hostname,  MAX_HOST_LEN );           
    else
        UFC::StrLCpy( FHost, UFC::Hostname,  MAX_HOST_LEN );           
}
//---------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& OID,
                      const UFC::AnsiString& BrokerID, 
                      int Func,
                      int MsgType,
                      int NID, 
                      const UFC::AnsiString& Key,
                      const UFC::AnsiString& Host )
:FNewOrderNID( NID )
,FMsgType( MsgType )    
,FFuncCode( Func )
,FLeavesQty(0)
,FLastPx(0.0)
{
    UFC::StrLCpy( FOrderID,   OID.c_str(),      MAX_OID_LEN );
    UFC::StrLCpy( FKeyString, Key.c_str(),      MAX_UDD_LEN );
    UFC::StrLCpy( FBPNO,      BrokerID.c_str(), MAX_BRK_LEN );    
    if( Host.Length() > 0 )
        UFC::StrLCpy( FHost, Host.c_str(),  MAX_HOST_LEN );           
    else
        UFC::StrLCpy( FHost, UFC::Hostname,  MAX_HOST_LEN );           
}
//---------------------------------------------------------------------------
OrderInfo::OrderInfo( void )
:FLeavesQty(0)
,FLastPx(0.0)
{
    FOrderID[0] = 0;
    FKeyString[0] = 0;
    FBPNO[0] = 0;    
    FHost[0] = 0;    
}
//---------------------------------------------------------------------------
void OrderInfo::Init( const UFC::AnsiString& OID, const UFC::AnsiString& BPNO, OrderRecord& OI)
{
    FNewOrderNID = OI.NID;
    FMsgType     = OI.MsgType;
    FFuncCode    = OI.FuncCode;
    UFC::StrLCpy( FOrderID,   OID.c_str(),  MAX_OID_LEN );
    UFC::StrLCpy( FKeyString, OI.Key,       MAX_UDD_LEN );
    UFC::StrLCpy( FBPNO,      BPNO.c_str(), MAX_BRK_LEN );    
    if( strlen(OI.Hostname) > 0 )
        UFC::StrLCpy( FHost, OI.Hostname,  MAX_HOST_LEN );           
    else
        UFC::StrLCpy( FHost, UFC::Hostname,  MAX_HOST_LEN );           
}
//---------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& DataString )
{
    FromString( DataString );
}
//---------------------------------------------------------------------------
void OrderInfo::FromString( const UFC::AnsiString& DataString )
{
    UFC::PStringList Values;
    
    Values.SetStrings(DataString, "|\n" );
    if( Values.ItemCount() >= 7 )
    {
        FNewOrderNID  = Values[ 1 ].ToInt();
        FMsgType      = Values[ 4 ].ToInt();
        FFuncCode     = Values[ 5 ].ToInt();
        UFC::StrLCpy( FOrderID,   Values[ 0 ].c_str(), MAX_OID_LEN );        
        UFC::StrLCpy( FKeyString, Values[ 2 ].c_str(), MAX_UDD_LEN );
        UFC::StrLCpy( FBPNO,      Values[ 3 ].c_str(), MAX_BRK_LEN );
        UFC::StrLCpy( FHost,      Values[ 6 ].c_str(), MAX_HOST_LEN );
    }
    if( Values.ItemCount() >= 9 )
    {
        FLeavesQty =  Values[ 7 ].ToInt();        
        FLastPx    =  Values[ 8 ].ToDouble();         
    }
    else
    {        
        FLeavesQty = 0;
        FLastPx    = 0;
    }
}
//---------------------------------------------------------------------------
void OrderInfo::ToString( UFC::AnsiString& DataString )
{
    DataString.Printf( "%s|%010u|%s|%s|%d|%d|%s|%06d|%0.4f|\n",
                       FOrderID,
                       FNewOrderNID, 
                       FKeyString,
                       FBPNO,
                       FMsgType,
                       FFuncCode,
                       FHost,
                       FLeavesQty,
                       FLastPx + 0.00001 );
}
//---------------------------------------------------------------------------
void OrderInfo::ToString( char* DataString,int len  )
{    
    snprintf( DataString,len,"%s|%010u|%s|%s|%d|%d|%s|%06d|%0.4f|\n",
                                FOrderID,
                                FNewOrderNID, 
                                FKeyString,
                                FBPNO,
                                FMsgType,
                                FFuncCode,
                                FHost,
                                FLeavesQty,
                                FLastPx + 0.00001 );
}
//---------------------------------------------------------------------------
//
// Class OrderInfoStore
//
//---------------------------------------------------------------------------
OrderInfoStore::OrderInfoStore( Int32 TransactionMax, BOOL IsTSE, BOOL RecoverMode )
:FOIDMap( TransactionMax )
,FNIDMap( TransactionMax*2 )
,FHostToSyncObj( 32 )
,FExecKeySet( TransactionMax*2 )
,FCluster( FALSE )
{
    
    FOrderInfoPoolPtr = new UFC::PThreadObjectPool<OrderInfo>( 4096, 1024 );
    if( IsTSE == TRUE ) ///< File names for TSE.
    {
        FNewOrderRecoverFile   = "SpeedyTSE.OrderInfo.DATA";        
        FExecutionIDRecoverFile= "SpeedyTSE.ExecutionKey.DATA";
        UFC::GenerateLogFileName( FExecutionLogFile,  "", "SpeedyTSE.Execution.","DATA", TRUE );        
    }
    else  ///< File names for OTC.
    {
        FNewOrderRecoverFile   = "SpeedyOTC.OrderInfo.DATA";
        FExecutionIDRecoverFile= "SpeedyOTC.ExecutionKey.DATA";
        UFC::GenerateLogFileName( FExecutionLogFile,  "", "SpeedyOTC.Execution.","DATA", TRUE );        
    }
    ///< Open files.
    FOrderInfoFile   = new UFC::BufferedLog( FNewOrderRecoverFile, 10240, RecoverMode, FALSE  );
    FExecutionIDFile = new UFC::BufferedLog( FExecutionIDRecoverFile, 10240, RecoverMode, CachedLog );    
    if( RecoverMode == TRUE )    
        Recover();    
    else    
        UFC::RemoveFile( FExecutionLogFile );    
    if( IsTSE == TRUE )
        FExecutionLog = new UFC::BufferedLog( "Speedy TSE Execution Store", ///< SectionName in LogCleaner
                                              "bin",                  ///< Reference directory
                                              "SpeedyTSE.Execution.", ///< File name perfix   
                                              "DATA",                 ///< File extention    
                                              10240,                  ///< File buffer size
                                              TRUE,                   ///< Append data to the end of file. F
                                              TRUE,                   ///< Use TW year. 
                                              FALSE );                 ///< Enable buffer cache. 
    else
        FExecutionLog = new UFC::BufferedLog( "Speedy OTC Execution Store", ///< SectionName in LogCleaner
                                              "bin",                  ///< Reference directory
                                              "SpeedyOTC.Execution.", ///< File name perfix   
                                              "DATA",                 ///< File extention    
                                              10240,                  ///< File buffer size
                                              TRUE,                   ///< Append data to the end of file. F
                                              TRUE,                   ///< Use TW year. 
                                              FALSE );                 ///< Enable buffer cache.         
    SetupCluster( RecoverMode, IsTSE );
    if( RecoverMode == TRUE )
        RecoverExecutions( );    
}
//---------------------------------------------------------------------------
// SpeedyCluster.cfg  sample
//
// [ServerThis] <-- My Host.(equals UFC::Hostname)
// 0=ServerThis
// 1=ServerAA
//
// [ServerAA] <--- Add this host to cluster
// 0=ServerAA 
// 1=ServerThis
//
//---------------------------------------------------------------------------
void OrderInfoStore::SetupCluster( BOOL RecoverMode, BOOL IsTSE )
{
    UFC::BufferedLog::Printf( " ****** Begin Setup Speedy Cluster ******" );               			                
    
    if( UFC::FileExists( ClusterConfigFile ) == TRUE )///< Cluster confg file exist
    {
	UFC::UiniFile  Config( ClusterConfigFile );
	UFC::Section*  HostSection;
        
        if( Config.SectionCount() >= 2 && Config.SectionExists( UFC::Hostname ) )
        {            
            for( int i = 0; i < Config.SectionCount(); i++ )
            {
                HostSection = Config.GetSection( i );
                ///< Find a HA Speedy Server
                if( HostSection->GetSectionName() != UFC::Hostname )
                {
                    UFC::BufferedLog::Printf( " Add Server:[%s] into cluster.", HostSection->GetSectionName().c_str() );               			
                    FHostToSyncObj.Add( HostSection->GetSectionName(), new SyncExecThread( IsTSE, RecoverMode, HostSection->GetSectionName(), this ));		
                }
            }
            if( FHostToSyncObj.ItemCount() > 0 )
            {
                FCluster = TRUE;
                UFC::BufferedLog::Printf( " **** Speedy Cluster function Enabled ***" );               			
            }
            else
                UFC::BufferedLog::Printf( " *** Speedy Cluster function Disabled ***" );               			
        }
        else
            UFC::BufferedLog::Printf( " *** Not a valid Cluster setting file! ***" );               			                    
    }	
    else
    {
        UFC::BufferedLog::Printf( " SpeedyCluster.cfg file not found." );               			        
        UFC::BufferedLog::Printf( " *** Speedy Cluster function Disabled ***" );               			    
    }
    UFC::BufferedLog::Printf( " ***** Setup Speedy Cluster Finished ****" );               			                    
}    
//---------------------------------------------------------------------------
void OrderInfoStore::RecoverExecutions()
{
    char            Buffer[ 512 ];
    FILE*           FileHandle;
    SyncExecThread* SyncObj;
    Int32           Begin = UFC::GetTickCountMS();
    Int32           Count = 0;

    if( (FileHandle = fopen( FExecutionLogFile.c_str(), "r" )) == NULL )
    {
        printf( "Can not open recover file %s", FExecutionLogFile.c_str() );
        exit( 1 );
    }
    while( fgets( Buffer, 512, FileHandle ) != NULL )
    {
        try
        {
            UFC::AnsiString OID, BrokerID;
            ExecutionParser Exec( Buffer );			
            
            if( Exec.GetHostname() == UFC::Hostname )
            {   ///< Recover local execution list.
                if( FExecutions.ItemCount() != Exec.GetSequence() &&
                    Exec.GetSequence() != 0 )  ///< Enable AA mode
                    UFC::BufferedLog::Printf( " Error execution seq, Original:%d now:%d.", Exec.GetSequence(), FExecutions.ItemCount() );               			
                if( ( Exec.IsConfirm() == TRUE ) && IsNewOrder( Exec.GetData(), FALSE ) && ( GetOID( Exec.GetNID(), OID, BrokerID, true ) == TRUE ) ) ///< Is a new order?
                {
                    int Func;
                    TMessageType MsgType;
                    
                    GetBrokerIDFromOrder( Exec.GetData(), BrokerID, Exec.GetPBNO(), Func, MsgType );
                    FExecutions.Add( new ExecInfo( Exec.GetNID(), OID, Exec.GetKey(), Buffer, Exec.GetPBNO(), BrokerID, Func, MsgType) );
                }
                else
                    FExecutions.Add( new ExecInfo( Buffer ) );
                Count++;
            }
            else
            {   ///< Recover Cluster execution list.
                if( (SyncObj = FHostToSyncObj.GetObjectByKey( Exec.GetHostname()))!=NULL)///< Find the ExecSeq set of the given server.	
                    SyncObj->SetMessageSeq( Exec.GetSequence() );				
            }
        }
        catch( UFC::Exception& e)
        {
            UFC::BufferedLog::Printf( " Parse execution log failed[%s].", e.what() );               			
            UFC::BufferedLog::Printf( " Execution log[%s].", Buffer );               			
        }        
    }
    fclose( FileHandle );
    UFC::BufferedLog::Printf( " Recover %d executions, use %d ms.", Count, UFC::GetTickCountMS() - Begin );               			
}
//---------------------------------------------------------------------------
// 
//  Recover Order information 
//
// 1. Recover  order information from files
//    SpeedyTSE.OrderInfo.DATA, SpeedyOTC.OrderInfo.DATA 
//
// 2. Recover execution keys from files 
//    SpeedyTSE.ExecutionKey.DATA, SpeedyOTC.ExecutionKey.DATA
//
//---------------------------------------------------------------------------
void OrderInfoStore::Recover()
{
    char            Buffer[ 512 ];
    FILE*           FileHandle;
    Int32           Begin = UFC::GetTickCountMS();
    Int32           Count = 0;    

    if( (FileHandle = fopen( FNewOrderRecoverFile.c_str(), "r" )) == NULL )
    {
        printf( "Can not open recover file %s", FNewOrderRecoverFile.c_str() );
        exit( 1 );
    }
    UFC::PLockObject Lock( FUpdateCS );

    while( fgets( Buffer, 512, FileHandle ) != NULL )
    {
        if( strlen( Buffer ) >= 17 ) ///< need 17 digi (OID 5, NID 10, Key 0 )
        {
            UFC::AnsiString OrderKey;
            OrderInfo* NewOrderInfo = new OrderInfo( Buffer );
            
            OrderKey.Printf( "%s_%s", NewOrderInfo->GetOrderID(), NewOrderInfo->GetBrokerID());
            FOIDMap.Add( OrderKey, NewOrderInfo );
            FNIDMap.Add( NewOrderInfo->GetNID(), NewOrderInfo );
            Count++;
        }
    }
    fclose( FileHandle );
    UFC::BufferedLog::Printf( " Recover %d NID <-> OID records, use %d ms.", Count, UFC::GetTickCountMS() - Begin );               			    
    RecoverExecutionID();
}
//---------------------------------------------------------------------------
void OrderInfoStore::RecoverExecutionID()
{
    char            Buffer[ 256 ];
    FILE*           FileHandle;
    Int32           Begin = UFC::GetTickCountMS();
    Int32           Count = 0;    

    if( (FileHandle = fopen( FExecutionIDRecoverFile.c_str(), "r" )) == NULL )
    {
        printf( "Can not open recover file %s", FExecutionIDRecoverFile.c_str() );
        exit( 1 );
    }
    UFC::PLockObject Lock( FExecKeyCS );
    
    while( fgets( Buffer, 256, FileHandle ) != NULL )
    {
        if( strlen( Buffer ) >= 15 ) ///< need 15 digi (BrokerID 7, Seq 8 or RXXX_X_00000000 )
        {
            UFC::AnsiString Key( Buffer,15 );
            FExecKeySet.Add( Key );
            Count ++;            
        }
    }
    fclose( FileHandle );
    UFC::BufferedLog::Printf( " Recover %d executionIDs, use %d ms.", Count, UFC::GetTickCountMS() - Begin );               			    
}
//---------------------------------------------------------------------------
BOOL OrderInfoStore::IsExecExist( const UFC::AnsiString& Key )
{
    UFC::PLockObject Lock( FExecKeyCS );
    
    if( FExecKeySet.Exists( Key ) == FALSE ) ///< Key not exists
    {
        UFC::AnsiString Line;
        
        Line.Printf( "%s\n", Key.c_str() );    
        FExecKeySet.Add( Key );
        FExecutionIDFile->Write( Line.c_str(), Line.Length() ); ///< Save to disk.
        return FALSE;
    }
    return TRUE;
}
//---------------------------------------------------------------------------
 OrderInfo* OrderInfoStore::AddOrder( const UFC::AnsiString& OID, const UFC::AnsiString& BrokerID, OrderRecord& OI, BOOL Force )
{
    char            OrderKey[ 16 ];          
    OrderInfo*      NewOrderInfo = FOrderInfoPoolPtr->GetInstance();
            
    
    NewOrderInfo->Init( OID, BrokerID, OI );
    snprintf( OrderKey, 16, "%s_%s", OID.c_str(), BrokerID.c_str() );    
    {   ///< Enter critical section.
        UFC::PLockObject Lock( FUpdateCS );     

        ///< Add to NID to info map.
        if( Force == FALSE ) ///< Not force add, check existing.
        {
            if( FNIDMap.SetObjectByKeyIfExists( OI.NID, NewOrderInfo ) == FALSE )
            {   ///< Duplicate NID.
                UFC::BufferedLog::DebugPrintf( " AddOrder(1) Duplicate NID[%ld].", (long)OI.NID );
                FOrderInfoPoolPtr->Recycle( NewOrderInfo );
                return NULL;
            }
        }
        ///< Add to order id - order info map.
        if( FOIDMap.SetObjectByKey( OrderKey, NewOrderInfo ) != NULL )
            UFC::BufferedLog::DebugPrintf( " AddOrder(1) Duplicate order OrderID[%s] BreokerID[%s].", OID.c_str(), BrokerID.c_str() );
    }
    return NewOrderInfo;
}
//---------------------------------------------------------------------------
void OrderInfoStore::LogNewOrder( OrderInfo* NewOrderInfo )
{
    char            Line[ 2048 ];
    ///< Serialize to disk.
    NewOrderInfo->ToString( Line, 2048 );
    FOrderInfoFile->WriteString( Line, strlen( Line ) - 1 );    
    FOrderInfoFile->Flush();    
}
//---------------------------------------------------------------------------
OrderInfo* OrderInfoStore::AddOrder( const UFC::AnsiString& OID, const UFC::AnsiString& OrderBrokerID, Int32 Func, Int32 Msgtype, Int32 NewOrderNID, const UFC::AnsiString& Key, const UFC::AnsiString& Host )
{
    UFC::AnsiString Line;
    UFC::AnsiString OrderKey;    
    OrderInfo*      OldOrderInfo;
    OrderInfo*      NewOrderInfo = new OrderInfo( OID, OrderBrokerID, Func, Msgtype, NewOrderNID, Key, Host );
    
    NewOrderInfo->ToString( Line );
    OrderKey.Printf( "%s_%s", OID.c_str(), OrderBrokerID.c_str() );    
    ///< Enter critical section.
    UFC::PLockObject Lock( FUpdateCS );     

    ///< Add to oid to info map.
    if( (OldOrderInfo = FOIDMap.SetObjectByKey( OrderKey, NewOrderInfo )) != NULL )
        UFC::BufferedLog::DebugPrintf( " AddOrder(2) Duplicate order OrderID[%s] BreokerID[%s].", OID.c_str(), OrderBrokerID.c_str() );
    ///< Add to NID to info map.
    FNIDMap.SetObjectByKey( NewOrderNID, NewOrderInfo );    
    ///< Serialize to disk.
    FOrderInfoFile->Write( Line.c_str(), Line.Length() );
    //FOrderInfoFile->Flush();
    return NewOrderInfo;
}
//---------------------------------------------------------------------------
BOOL OrderInfoStore::GetOrderInfo( const UFC::AnsiString& OID, const UFC::AnsiString& OrderBrokerID, UFC::AnsiString& Key, Int32& NID )
{
    UFC::AnsiString OrderKey;   
    OrderInfo*      OrigOrderInfo;
    
    OrderKey.Printf( "%s_%s", OID.c_str(), OrderBrokerID.c_str() );    
    UFC::PLockObject Lock( FUpdateCS );     
    
    if( (OrigOrderInfo = FOIDMap.GetObjectByKey( OrderKey )) != NULL )
    {
        Key =  OrigOrderInfo->GetKeyString();
        NID =  OrigOrderInfo->GetNID();
        return TRUE;
    }
    Key = "";
    NID = -1;
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL OrderInfoStore::GetOrderInfo( const UFC::AnsiString& OID, const UFC::AnsiString& OrderBrokerID, OrderInfo*& OrigOrderInfo )
{
    UFC::AnsiString OrderKey;   
    
    OrderKey.Printf( "%s_%s", OID.c_str(), OrderBrokerID.c_str() );
    UFC::PLockObject Lock( FUpdateCS );     
    
    OrigOrderInfo = FOIDMap.GetObjectByKey( OrderKey );
    if( OrigOrderInfo != NULL )
        return TRUE;
    return FALSE;    
}
//---------------------------------------------------------------------------
BOOL OrderInfoStore::IsOrderExists( const UFC::AnsiString& OID, const UFC::AnsiString& OrderBrokerID )
{
    UFC::AnsiString OrderKey;   
    
    OrderKey.Printf( "%s_%s", OID.c_str(), OrderBrokerID.c_str() );
    UFC::PLockObject Lock( FUpdateCS );     
    
    if( FOIDMap.GetObjectByKey( OrderKey ) != NULL )
        return TRUE;
    return FALSE;
}    
//---------------------------------------------------------------------------
BOOL OrderInfoStore::GetOID( Int32 NewOrderNID, UFC::AnsiString& OID, UFC::AnsiString& IB, bool NeedLock )
{
    BOOL isSuccess = FALSE;
    if( NeedLock ) FUpdateCS.Acquire();
    OrderInfo* OrigOrderInfo = FNIDMap.GetObjectByKey( NewOrderNID );
    if( OrigOrderInfo != NULL )
    {
        OID = OrigOrderInfo->GetOrderID();
        IB  = OrigOrderInfo->GetBrokerID();
        isSuccess = TRUE;
    }
    else
    {
        OID = "     ";
        IB  = "0000";
    }
    if( NeedLock ) FUpdateCS.Release();
    return isSuccess;
}
//------------------------------------------------------------------------------
// Executions from Speedy on other servers.
//------------------------------------------------------------------------------
void OrderInfoStore::WriteExecutionLog( Int32 Seq, const UFC::AnsiString& Host,const UFC::AnsiString& Log )
{    
    SyncExecThread* SyncObj = FHostToSyncObj.GetObjectByKey( Host );///< Find the ExecSeq set of the given server.
	
    if( SyncObj != NULL )
    {
        if( SyncObj->MessageSeqExists( Seq ) == FALSE ) ///< This execution not exists.
        {
            SyncObj->SetMessageSeq( Seq );	
            FExecutionLog->Write( Log.c_str(), Log.Length() );
            FExecutionLog->Flush();
        }
        else
            UFC::BufferedLog::Printf( " Execution seq:[%d] already exists, Skip it.", Seq );
    }
    else
        UFC::BufferedLog::Printf( " Host:[%s] NOT IN Speedy ClUSTER !!!", Host.c_str() );        
}
//------------------------------------------------------------------------------
// Executions from local Speedy.
//------------------------------------------------------------------------------
void OrderInfoStore::WriteExecutionLog( BOOL IsConfirm, Int32 NID, const UFC::AnsiString& Key,const UFC::AnsiString& Data, const UFC::AnsiString& PBNO, const UFC::AnsiString& Host, const UFC::AnsiString& PVCID )
{
    UFC::AnsiString Msg, OID, BrokerID;
    ExecInfo*       ExecInfomation;
        
    if( FCluster == TRUE ) ///< Enable Speedy Cluster
    {
        int Func;
        TMessageType MsgType;
        
        GetBrokerIDFromOrder( Data/*In*/, BrokerID, PBNO/*In*/, Func, MsgType ); ///< Get the BrokerID,IB from Order string.         
        ///< Add to Execution list.
        if( IsConfirm == TRUE &&            ///< It's a order confirm message 
            IsNewOrder( Data, FALSE ) &&    ///< Check the message data. 
            ( GetOID( NID, OID, BrokerID, true ) == TRUE ) )   ///<  Get the OID from memory.
            ExecInfomation = new ExecInfo( NID, OID, Key, Msg, PBNO, BrokerID, Func, MsgType );///< Is a new order.
        else
            ExecInfomation = new ExecInfo( ) ;
        ///< Add Execution object to Execution list.        
        UFC::PLockObject Lock( FExecCS );        
        ///< Render the execution log line.
        ExecutionParser::Render( ExecInfomation->FLog, IsConfirm, NID, FExecutions.ItemCount(), Key, Data, PBNO, Host, PVCID );
        FExecutions.Add( ExecInfomation );///< Add to Execution list.        
        FExecutionLog->Write( ExecInfomation->FLog.c_str(), ExecInfomation->FLog.Length());
    }
    else
    {
        ///< Render the execution log line.
        ExecutionParser::Render( Msg, IsConfirm, NID, 0, Key,Data, PBNO,Host,PVCID );
        FExecutionLog->Write( Msg.c_str(), Msg.Length());
    }    
    FExecutionLog->Flush();	        
}
//---------------------------------------------------------------------------
//
//  Functions to control Execution threads.
//
//---------------------------------------------------------------------------
//
//  Speedy on other server startup.
//  - Find the execution thread.
//  - Tell the server that the maximum sequence execution this server had.
//  - Activate the sync thread.
//
//---------------------------------------------------------------------------
void OrderInfoStore::OnSpeedyStartup( const UFC::AnsiString& Host)
{
    SyncExecThread* SyncObj = FHostToSyncObj.GetObjectByKey( Host );///< Find the Execution thread of the given server.
	
    if( SyncObj != NULL ) 
    {
        SyncObj->AskSequenceFrom(); 
        SyncObj->SetActive( TRUE ); ///< Activate the sync thread.		 
        UFC::BufferedLog::Printf( " Speedy on Host:[%s] Startup, activate the sending thread.", Host.c_str() );        
    }
}
//---------------------------------------------------------------------------
//
//  Speedy on other server stopped.
//  - Find the execution thread.
//  - Inactivate the sync thread.
//
//---------------------------------------------------------------------------
void OrderInfoStore::OnSpeedyStop( const UFC::AnsiString& Host)
{
    SyncExecThread* SyncObj = FHostToSyncObj.GetObjectByKey( Host );///< Find the Execution thread of the given server.
	
    if( SyncObj != NULL ) 
    {
	SyncObj->SetActive( FALSE ); ///< Inactivate the sync thread.
        UFC::BufferedLog::Printf( " Speedy on Host:[%s] stopped, suspend the sending thread.", Host.c_str() );
    }
}
//---------------------------------------------------------------------------
//
// Stop all sync thread.
// Call this function when lose MBus connection.
//
//---------------------------------------------------------------------------
void OrderInfoStore::StopAllSyncThread( void )
{
    for( int i=0; i< FHostToSyncObj.ItemCount(); i ++ )
    {
	SyncExecThread* SyncObj = FHostToSyncObj.GetItem( i );
	if( SyncObj != NULL ) 
            SyncObj->SetActive( FALSE );		
    }
}
//---------------------------------------------------------------------------
//
//  Send recovered executions to the given host from the given sequence
//  Params ToHost:   To which host.
//         BeginSeq: From the sequence.
//  Steps:
//  - Find the execution thread.
//  - Compare the given sequence and the actual sequence.
//  - Tell the sync thread sending executions from which sequence.
//  - Activate the sync thread.
//
//---------------------------------------------------------------------------
void OrderInfoStore::RecoverExecFromSequence( const UFC::AnsiString& ToHost, Int32 BeginSeq )
{
    SyncExecThread* SyncObj = FHostToSyncObj.GetObjectByKey( ToHost );///< Find the Execution thread of the given server.
	
    if( SyncObj != NULL ) 
    {
        int CurrentSeq = FExecutions.ItemCount();
        int StartSeq = BeginSeq;
        
        if( BeginSeq > CurrentSeq )
        {            
            UFC::BufferedLog::Printf( " [%s] Request seq:[%d] larger than the current seq:[%d]", ToHost.c_str(), BeginSeq, CurrentSeq );
            StartSeq = CurrentSeq;
        }
        SyncObj->SetRecoverFrom( StartSeq );		
        SyncObj->SetActive( TRUE );		                
    }
}
//---------------------------------------------------------------------------
//
// Class SyncExecThread
// Execution Sync thread
//
// - Sync local executions in OrderInfoStore to other servers.
// - Keep other server's execution sequence.
//
// If there 4 servers in a cluster.
// The OrderInfoStore will create 3 Execution Sync threads for other 3 servers. 
//
// IsTSE :        TWSE or OTC.
// RecoverMode:   Need recover previous executions.
// Host:          Sync server name.
// Store:         Local OrderInfoStore object instance.
//---------------------------------------------------------------------------
SyncExecThread::SyncExecThread( BOOL IsTSE, BOOL RecoverMode, const UFC::AnsiString& Host, OrderInfoStore* Store )
:UFC::PThread( NULL, TRUE )
,FHostname( Host )
,FIsActive( FALSE )
,FRecovering( FALSE )
,FStore( Store )
{
    UFC::AnsiString marketName, seqName;
    if( IsTSE == TRUE )
        marketName = "TSE";
    else
        marketName = "OTC";    
    FSyncSubject.Printf( "SYNC.%s", marketName.c_str() );
    seqName.Printf( "%s.%s", marketName.c_str(), Host.c_str() );
    FOutSeq = new UFC::PInt32( seqName, 0 );  ///< Set sequence to 1 if share memory not exists.
    
    if( RecoverMode == FALSE )
    {
        *FOutSeq = 0  ;
	 FSeqSet.Clear();
    }
    Start();
}
//---------------------------------------------------------------------------
//
// Is the given execution sequence exists?
//
//---------------------------------------------------------------------------
BOOL SyncExecThread::MessageSeqExists( Int32 Seq )
{ 
    UFC::PLockObject Lock( FSeqCS );

    return FSeqSet.Exists( Seq ); 
}
//---------------------------------------------------------------------------
//
// Add an execution sequence into Sequence set.
//
//---------------------------------------------------------------------------
void SyncExecThread::SetMessageSeq( Int32 Seq )
{
    UFC::PLockObject Lock( FSeqCS );

    FSeqSet.Add( Seq );
}
//---------------------------------------------------------------------------
//
//   Recover Executions to other server from the given sequence.
//   call this function will sync executions to FHostname from the given sequence.
//
//---------------------------------------------------------------------------
void SyncExecThread::SetRecoverFrom( Int32 Seq )
{
    Int32 CurrentSeq = FStore->FExecutions.ItemCount();
    if( Seq > CurrentSeq )
    {            
        *FOutSeq = CurrentSeq;
        UFC::BufferedLog::Printf( " [%s] Request seq[%d] larger than current seq[%d], Recover finished.", FHostname.c_str(), Seq, CurrentSeq );
    }
    else if( Seq == CurrentSeq )
    {              
        *FOutSeq = Seq;
        UFC::BufferedLog::Printf( " [%s] Request seq[%d] equals to current seq, Recover finished.",FHostname.c_str(), Seq );
    }
    else
    {
        *FOutSeq = Seq;
        FRecovering = TRUE;
        FBeginTime = UFC::GetTickCountMS( );
        UFC::BufferedLog::Printf( " [%s] Request seq[%d] recover from [%d] to [%d].",FHostname.c_str(), Seq, Seq,  CurrentSeq );        
    }
}
//---------------------------------------------------------------------------
//
// Ask Speedy in FHostname to send me executions from the "Sequence hole"
// or the max sequence number.
//
//---------------------------------------------------------------------------
void SyncExecThread::AskSequenceFrom( void )
{
    Int32 MaxSeq = 0;
    MTree SyncData;

    while( TRUE ) ///< Find the seq "Hole" or the last seq.
    {
	if( FSeqSet.Exists( MaxSeq ) == FALSE )
            break;
	MaxSeq++;
    }
    SyncData.append( "CMD", 0 ); ///< CMD = 0, Ask other Speedy Server to recover executions.
    SyncData.append( "SEQ", MaxSeq );               
    SyncData.append( "HOST", UFC::Hostname );///< Send by this server               
    MessageObj->Send( FSyncSubject, FHostname, SyncData, FALSE ); ///< To FHostname
    UFC::BufferedLog::Printf( " Ask Host:[%s] to recover executions from seq:[%d]", FHostname.c_str(), MaxSeq );
}
//---------------------------------------------------------------------------
//
//  Private functions
//
//---------------------------------------------------------------------------
 ExecInfo* SyncExecThread::GetExecInfo( Int32 MsgSeq )
 {
    UFC::PLockObject Lock( FStore->FExecCS );       
        
    return FStore->FExecutions.GetItem( MsgSeq );
 }     
//---------------------------------------------------------------------------
void SyncExecThread::SendSyncMessage( void )
{
    MTree       SyncData;
    int         MsgSeq = FOutSeq->ToInt32();
    ExecInfo*   Exec = GetExecInfo( MsgSeq ); 
        
    if( Exec != NULL )
    {        
        SyncData.append( "CMD", 1 ); ///< CMD = 1, Sync an execution to other Speedy Servers.              
        SyncData.append( "SEQ", MsgSeq );               
        SyncData.append( "HOST", UFC::Hostname );///< Send by this server               
        SyncData.append( "NID", Exec->GetNID() );
        if( Exec->IsNewOrder( ) == TRUE  )///< Is a new order?
        {   ///< You can use NID or Broker+OID as a unique key to find an order.
            SyncData.append( "OID",    Exec->GetOID() );
            SyncData.append( "Key",    Exec->GetKey() );
            SyncData.append( "BPNO",   Exec->GetBPNO() );                    
            SyncData.append( "FUNC",   Exec->GetFunc() );                    
            SyncData.append( "TYPE",   Exec->GetMsgType() );                    
            if( Exec->GetBrokerID().Length() < 4 )///< BrokerID of TSEC is 4 digi.
            {
                UFC::BufferedLog::Printf( " Error New Order confirm log[%s], missing BrokerID.", Exec->GetLog().c_str() );
                *FOutSeq += 1;                        
                return;
            }
            else
                SyncData.append( "Broker", Exec->GetBrokerID() );                    
        }
        SyncData.append( "Log", Exec->GetLog() );
        MessageObj->Send( FSyncSubject, FHostname, SyncData, FALSE );
        *FOutSeq += 1;
    }
    else
        UFC::BufferedLog::Printf( " *** Get a NULL Execution info, index[%d] ***", MsgSeq );
}    
//---------------------------------------------------------------------------
void SyncExecThread::Execute( void )
{
    while( !IsTerminated() ) ///< Thread not terminated.
    {
        if( FIsActive == TRUE )	 ///< Speedy on that host is active.
        {
            Int32 SendingCount = 0;
            
            while( FStore->FExecutions.ItemCount() > FOutSeq->ToInt32() )
            {
                SendSyncMessage(); ///< Send local executions to other AA Host.
                SendingCount++;
                if( SendingCount % 2 == 0 )
                    UFC::SleepMS( 1 ); ///< Sleep 1 ms,after 2 messages had been sent. 
            }
            if( FRecovering == TRUE && SendingCount > 0 )
            {
                FRecovering = FALSE;
                UFC::BufferedLog::Printf( " Send %d Recover executions to [%s] finished, use %d ms",SendingCount, FHostname.c_str(), UFC::GetTickCountMS( ) - FBeginTime  );
            }
            UFC::SleepMS( 10 );
        }
        else
            UFC::SleepMS( 50 );
    }
}
//-----------------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

