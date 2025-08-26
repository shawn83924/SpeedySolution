//---------------------------------------------------------------------------
#include "OrderInfoStore.h"
#include "Speedy.h"
#include "../ExchangeUtility/Utility.h"
//---------------------------------------------------------------------------
// Class OrderInfo
// class to store information for an order.
// Fields: OrderID, IB, Nework ID, Key string, Isfrom speedy.
//---------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& OID, const UFC::AnsiString& IB, const UFC::AnsiString& Key, Int32 NID, BOOL IsSpeedy )
:FNewOrderNID( NID )
,FOrderID( OID )
,FIB( IB )
,FKeyString( Key )
,FIsSpeedy( IsSpeedy )
{
}
//---------------------------------------------------------------------------
OrderInfo::OrderInfo( const UFC::AnsiString& DataString )
{
    FromString( DataString );
}
//---------------------------------------------------------------------------
void OrderInfo::FromString( const UFC::AnsiString& DataString )
{
    char Seps[] = "|\n";

    FOrderID      = strtok( (char*) DataString.c_str(), Seps );
    FIB           = strtok( NULL, Seps );
    FNewOrderNID  = atoi( strtok( NULL, Seps) );
    FKeyString    = strtok( NULL, Seps);
    FIsSpeedy     = atoi( strtok( NULL, Seps) );
}
//---------------------------------------------------------------------------
void OrderInfo::ToString( UFC::AnsiString& DataString )
{
    DataString.Printf("%s|%s|%010d|%s|%01d\n", FOrderID.c_str(), FIB.c_str(), FNewOrderNID, FKeyString.c_str(), FIsSpeedy );
}
//---------------------------------------------------------------------------
UFC::PCriticalSection  OrderInfoStore::FUpdateCS;
UFC::PCriticalSection  OrderInfoStore::FFileCS;
UFC::PCriticalSection  OrderInfoStore::FExecKeyCS;
UFC::PCriticalSection  OrderInfoStore::FExecCS;
//---------------------------------------------------------------------------
// Class OrderInfoStore
// Store all order status.
//---------------------------------------------------------------------------
OrderInfoStore::OrderInfoStore( Int32 HashTableSize, BOOL IsOpt, BOOL RecoverMode )
:FOIDMap( HashTableSize )
,FNIDMap( HashTableSize*2 )
,FHostToSyncObj( 16 )
,FExecKeySet( HashTableSize*2 )
,FCluster( FALSE )
{
    if( IsOpt == TRUE )
    {
        FNewOrderRecoverFile   = "SpeedyOPT.OrderInfo.DATA";
        FCancelNIDRecoverFile  = "SpeedyOPT.CancelNID.DATA";
        FReplaceNIDRecoverFile = "SpeedyOPT.ReplaceInfo.DATA";
        FExecutionIDRecoverFile= "SpeedyOPT.ExecutionKey.DATA";
        FExecutionLogFile      = "SpeedyOPT.Execution." + UFC::GetDateString()+".DATA";
    }
    else
    {
        FNewOrderRecoverFile   = "SpeedyFUT.OrderInfo.DATA";
        FCancelNIDRecoverFile  = "SpeedyFUT.CancelNID.DATA";
        FReplaceNIDRecoverFile = "SpeedyFUT.ReplaceInfo.DATA";
        FExecutionIDRecoverFile= "SpeedyFUT.ExecutionKey.DATA";
        FExecutionLogFile      = "SpeedyFUT.Execution." + UFC::GetDateString()+".DATA";
    }

    if( RecoverMode == TRUE )
    {
        FOrderInfoFile  = new UFC::FileStreamEx( FNewOrderRecoverFile, "r+" );
        FCancelNIDFile  = new UFC::FileStreamEx( FCancelNIDRecoverFile, "r+" );
        FReplaceNIDFile = new UFC::FileStreamEx( FReplaceNIDRecoverFile, "r+" );
        FExecutionIDFile= new UFC::FileStreamEx( FExecutionIDRecoverFile, "r+" );
        FOrderInfoFile->Seek( 0, UFC::soFromEnd );
        FCancelNIDFile->Seek( 0, UFC::soFromEnd );
        FReplaceNIDFile->Seek( 0, UFC::soFromEnd );
        FExecutionIDFile->Seek( 0, UFC::soFromEnd );        
        Recover();
    }
    else
    {
        FOrderInfoFile  = new UFC::FileStreamEx( FNewOrderRecoverFile, "w+" );
        FCancelNIDFile  = new UFC::FileStreamEx( FCancelNIDRecoverFile, "w+" );
        FReplaceNIDFile = new UFC::FileStreamEx( FReplaceNIDRecoverFile, "w+" );
        FExecutionIDFile= new UFC::FileStreamEx( FExecutionIDRecoverFile, "w+" );
        DeleteFile( FExecutionLogFile );
    }
    FExecutionLog = new UFC::FileStreamEx( FExecutionLogFile, "a" );
    UFC::BufferedLog::Printf( " ***** Check Speedy Cluster function ****" );               			                    
    if( UFC::FileExists( ClusterConfigFile ) == TRUE )
    {
	UFC::UiniFile  Config( ClusterConfigFile );
	UFC::Section*  HostSection;
	if( Config.SectionCount() >= 2 && Config.SectionExists( UFC::Hostname ) )
	{            
            for( int i = 0; i < Config.SectionCount(); i++ )
            {
            	HostSection = Config.GetSection( i );
		if( HostSection->GetSectionName() != UFC::Hostname )
                {
                    UFC::BufferedLog::Printf( " Add Server:[%s] into cluster.",HostSection->GetSectionName().c_str() );               			
                    FHostToSyncObj.Add( HostSection->GetSectionName(), new SyncExecThread( IsOpt, RecoverMode, HostSection->GetSectionName(), this ));		
                }
            }
            if( FHostToSyncObj.ItemCount() > 0 )
            {
                FCluster = TRUE;
                UFC::BufferedLog::Printf( " *** Speedy Cluster function Enabled ***" );               			
            }
            else
                UFC::BufferedLog::Printf( " *** Speedy Cluster function Disabled ***" );               			
	}
	if( RecoverMode == TRUE )
            RecoverExecutions();
    }	
    else
        UFC::BufferedLog::Printf( " *** Speedy Cluster function Disabled ***" );               			
    
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
            UFC::AnsiString OID,IB;
            ExecutionParser Exec( Buffer );
			
            if( Exec.GetHostname() == UFC::Hostname )
            {   ///< Recover local execution list.
		if( FExecutions.ItemCount() != Exec.GetSequence() && Exec.GetSequence() != 0 )
                    UFC::BufferedLog::Printf( " Error execution seq,Orig:%d now:%d.", Exec.GetSequence(), FExecutions.ItemCount() );               			
		if( Exec.IsConfirm() == TRUE &&               ///< It's a confirm message
                    IsNewOrder( Exec.GetData(), TRUE ) &&     ///< Check the message data.
                    GetOID( Exec.GetNID(), OID, IB ) == TRUE )///< Get OID,IB from OrderRecord hashtable. 
                    FExecutions.Add( new NewExec( Exec.GetNID(), OID, IB,  Exec.GetKey(), Buffer ) );///< Is a new order.
		else	
                    FExecutions.Add( new ExecInfo( Buffer ) );
                Count++;
            }
            else
            {
		if( (SyncObj = FHostToSyncObj.GetObjectByKey( Exec.GetHostname()))!=NULL)///< Find the ExecSeq set of the given server.	
                    SyncObj->SetMessageSeq( Exec.GetSequence() );				
            }
	}
	catch( ... )
	{
            UFC::BufferedLog::Printf( " Error execution log[%s].", Buffer );               			
	}        
    }
    fclose( FileHandle );
    UFC::BufferedLog::Printf( " Recover %d executions, use %d ms.", Count, UFC::GetTickCountMS() - Begin );               			
}
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

            OrderKey.Printf("%s%s", NewOrderInfo->GetIB().c_str(), NewOrderInfo->GetOrderID().c_str() );
            FOIDMap.Add( OrderKey, NewOrderInfo );
            FNIDMap.Add( NewOrderInfo->GetNID(), NewOrderInfo );
            Count++;
        }
    }
    fclose( FileHandle );
    UFC::BufferedLog::Printf( " Recover %d NID <-> OID records, use %d ms.", Count, UFC::GetTickCountMS() - Begin );               			
    RecoverExecutionID();
    RecoverCancelNID();
    RecoverReplaceNID();
}
//---------------------------------------------------------------------------
void OrderInfoStore::DeleteItemInList( UFC::PInt32List& List, Int32 NID )
{
    for( register int i=0;i<List.ItemCount();i++)
    {
         if( List[i] == NID )
         {
             List.Delete(i);
             return;
         }
    }
}
//---------------------------------------------------------------------------
void OrderInfoStore::DeleteItemInList( UFC::PStringList& List, const UFC::AnsiString& CKey )
{
    for( register int i=0;i<List.ItemCount();i++)
    {
         if( List[i] == CKey )
         {
             List.Delete(i);
             return;
         }
    }
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
void OrderInfoStore::RecoverCancelNID()
{
    UFC::AnsiString    Act,OID,CKey,IB,OrderKey;
    Int32              NID;
    char               Buffer[ 256 ];
    char               Seps[] = "|\n";
    FILE*              FileHandle;
    OrderInfo*         OrigOrderInfo;

    if( (FileHandle = fopen( FCancelNIDRecoverFile.c_str(), "r" )) == NULL )
    {
        printf( "Can not open recover file %s", FCancelNIDRecoverFile.c_str() );
        exit( 1 );
    }
    while( fgets( Buffer, 256, FileHandle ) != NULL )
    {
        if( strlen( Buffer ) >= 18 ) ///< need 18 digi ( +/-, OID 5, NID 10 )
        {
            Act  = strtok( Buffer, Seps );
            OID  = strtok( NULL, Seps);
            IB   = strtok( NULL, Seps);
            NID  = atoi( strtok( NULL, Seps));
            CKey = strtok( NULL, Seps);
            OrderKey.Printf("%s%s", IB.c_str(), OID.c_str() );

            OrigOrderInfo = FOIDMap.GetObjectByKey( OrderKey );
            if( OrigOrderInfo != NULL )
            {
                if( Act[0] == '+' )
                {
                    OrigOrderInfo->FCancleNIDs.Add( NID );
                    OrigOrderInfo->FCancleKeys.Add( NID );                    
                }
                else
                    DeleteItemInList( OrigOrderInfo->FCancleNIDs,NID );
            }
        }
    }
    fclose( FileHandle );
}
//---------------------------------------------------------------------------
void OrderInfoStore::RecoverReplaceNID()
{
    UFC::AnsiString    Act,OID,CKey,IB,OrderKey;
    Int32              NID;
    char               Buffer[ 256 ];
    char               Seps[] = "|\n";
    FILE*              FileHandle;
    OrderInfo*         OrigOrderInfo;

    if( (FileHandle = fopen( FReplaceNIDRecoverFile.c_str(), "r" )) == NULL )
    {
        printf( "Can not open recover file %s", FReplaceNIDRecoverFile.c_str() );
        exit( 1 );
    }
    while( fgets( Buffer, 256, FileHandle ) != NULL )
    {
        if( strlen( Buffer ) >= 18 ) ///< need 18 digi ( +/-, OID 5, NID 10 )
        {
            Act  = strtok( Buffer, Seps );
            OID  = strtok( NULL, Seps);
            IB   = strtok( NULL, Seps);
            NID  = atoi( strtok( NULL, Seps));
            CKey = strtok( NULL, Seps);
            OrderKey.Printf("%s%s", IB.c_str(), OID.c_str() );

            OrigOrderInfo = FOIDMap.GetObjectByKey( OrderKey );
            if( OrigOrderInfo != NULL )
            {
                if( Act[0] == '+' )
                {
                    OrigOrderInfo->FReplaceNIDs.Add( NID );
                    OrigOrderInfo->FReplaceKeys.Add( CKey );
                }
                else                
                    DeleteItemInList( OrigOrderInfo->FReplaceNIDs,NID );
            }
        }
    }
    fclose( FileHandle );
}
//---------------------------------------------------------------------------
void OrderInfoStore::LogModify( BOOL IsAdd, const UFC::AnsiString& OID, const UFC::AnsiString& IB, Int32 NID, const UFC::AnsiString& Key, UFC::FileStreamEx* File )
{
    UFC::AnsiString Line;
    if( IsAdd == TRUE )
        Line.Printf( "+|%s|%s|%010d|%s\n", OID.c_str(),IB.c_str(), NID,Key.c_str() );
    else
        Line.Printf( "-|%s|%s|%010d|%s\n", OID.c_str(),IB.c_str(),NID,Key.c_str() );

    UFC::PLockObject Lock( FFileCS );

    File->Write( Line.c_str(), Line.Length()  );
    File->Flush();
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
        FExecutionIDFile->Flush();    
        return FALSE;
    }
    return TRUE;
}
//---------------------------------------------------------------------------
OrderInfo* OrderInfoStore::AddOrder( const UFC::AnsiString& OID, const UFC::AnsiString& IB, const UFC::AnsiString& Key, Int32 NID, BOOL IsSpeedy )
{
    UFC::AnsiString Line,OrderKey;
    OrderInfo*      NewOrderInfo = new OrderInfo( OID, IB, Key, NID, IsSpeedy );

    OrderKey.Printf("%s%s", IB.c_str(), OID.c_str() );
    NewOrderInfo->ToString( Line );
    ///< Enter critical section.
    UFC::PLockObject Lock( FUpdateCS );
    ///< Add to oid to info map.
    FOIDMap.Add( OrderKey, NewOrderInfo );
    ///< Add to NID to info map.
    FNIDMap.Add( NID, NewOrderInfo );
    ///< Serialize to disk.
    FOrderInfoFile->Write( Line.c_str(), Line.Length() );
    FOrderInfoFile->Flush();
    return NewOrderInfo;
}
//---------------------------------------------------------------------------
BOOL OrderInfoStore::GetOrderInfo( const UFC::AnsiString& OID, const UFC::AnsiString& IB, UFC::AnsiString& Key, Int32& NID )
{
    UFC::AnsiString OrderKey;
    OrderInfo*      OrigOrderInfo;

    OrderKey.Printf("%s%s", IB.c_str(), OID.c_str());
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
BOOL OrderInfoStore::GetOrderInfo( const Int32 NID, Int32& OrigNID, UFC::AnsiString& OID, UFC::AnsiString& IB, UFC::AnsiString& Key  )
{
    OrderInfo* OrigOrderInfo = FNIDMap.GetObjectByKey( NID );
    if( OrigOrderInfo != NULL )
    {
        OID     = OrigOrderInfo->GetOrderID();
        IB      = OrigOrderInfo->GetIB();
        Key     = OrigOrderInfo->GetKeyString();
        OrigNID = OrigOrderInfo->GetNID();
        return TRUE;
    }
    OrigNID = 0;
    OID = "     ";
    IB  = "000";
    Key = "     ";
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL OrderInfoStore::GetOID( Int32 NewOrderNID, UFC::AnsiString& OID, UFC::AnsiString& IB )
{
    OrderInfo* OrigOrderInfo = FNIDMap.GetObjectByKey( NewOrderNID );
    if( OrigOrderInfo != NULL )
    {
        OID =  OrigOrderInfo->GetOrderID();
        IB  =  OrigOrderInfo->GetIB();
        return TRUE;
    }
    OID = "     ";
    IB  = "000";
    return FALSE;
}
//---------------------------------------------------------------------------
void OrderInfoStore::AddNID( const UFC::AnsiString& OID, const UFC::AnsiString& IB, Int32 NID, const UFC::AnsiString& Key )
{
    UFC::AnsiString OrderKey;
    OrderInfo*      OrigOrderInfo;

    OrderKey.Printf("%s%s", IB.c_str(), OID.c_str());
    if( (OrigOrderInfo = FOIDMap.GetObjectByKey( OrderKey )) == NULL )
    {
        OrigOrderInfo = AddOrder( OID, IB, Key, NID, FALSE );
        UFC::BufferedLog::DebugPrintf( " OrderID[%s] IB[%s] Not a Speedy Order.", OID.c_str(), IB.c_str() );
        UFC::BufferedLog::DebugPrintf( " Add a record that we can query this order.");
    }
    UFC::PLockObject Lock( FUpdateCS );
    ///< Add to NID to info map.
    FNIDMap.Add( NID, OrigOrderInfo );
}
//---------------------------------------------------------------------------
void OrderInfoStore::PushCancelNID(  const UFC::AnsiString& OID, const UFC::AnsiString& IB, Int32 CancelNID, const UFC::AnsiString& CKey, BOOL IsTMP )
{
    UFC::AnsiString OrderKey;
    OrderInfo*      OrigOrderInfo;

    OrderKey.Printf("%s%s", IB.c_str(), OID.c_str());
    if( (OrigOrderInfo = FOIDMap.GetObjectByKey( OrderKey )) == NULL ) ///< Original Order Not exists, means it's not Speedy Order.
    {
        OrigOrderInfo = AddOrder( OID, IB, CKey, CancelNID, FALSE );
        UFC::BufferedLog::DebugPrintf( " OrderID[%s] IB[%s] Not a Speedy Order.", OID.c_str(), IB.c_str() );
        UFC::BufferedLog::DebugPrintf( " Add a record that we can cancel this order.");
    }
    UFC::PLockObject Lock( FUpdateCS );

    OrigOrderInfo->FCancleNIDs.Add( CancelNID );
    OrigOrderInfo->FCancleKeys.Add( CKey );
    if( IsTMP == FALSE )
        LogModify( TRUE, OID, IB, CancelNID, CKey, FCancelNIDFile );
    else
        FNIDMap.Add( CancelNID, OrigOrderInfo );
}
//---------------------------------------------------------------------------
void OrderInfoStore::PushReplaceNID( const UFC::AnsiString& OID,  const UFC::AnsiString& IB,  Int32 ReplaceNID, const UFC::AnsiString& CKey, BOOL IsTMP )
{
    UFC::AnsiString OrderKey;
    OrderInfo*      OrigOrderInfo;

    OrderKey.Printf("%s%s", IB.c_str(), OID.c_str());
    if( (OrigOrderInfo = FOIDMap.GetObjectByKey( OrderKey )) == NULL )
    {
        OrigOrderInfo = AddOrder( OID, IB, CKey, ReplaceNID, FALSE );
        UFC::BufferedLog::DebugPrintf( " OrderID[%s] IB[%s] Not a Speedy Order.", OID.c_str(), IB.c_str() );
        UFC::BufferedLog::DebugPrintf( " Add a record that we can replace this order.");
    }
    UFC::PLockObject Lock( FUpdateCS );

    OrigOrderInfo->FReplaceNIDs.Add( ReplaceNID );
    OrigOrderInfo->FReplaceKeys.Add( CKey );
    if( IsTMP == FALSE )
        LogModify( TRUE, OID, IB, ReplaceNID, CKey, FReplaceNIDFile );
    else
        FNIDMap.Add( ReplaceNID, OrigOrderInfo );
}
//------------------------------------------------------------------------------
BOOL OrderInfoStore::PopCancelNID(  const UFC::AnsiString& OID,  const UFC::AnsiString& IB, Int32& CancelNID, UFC::AnsiString& Key, UFC::AnsiString& CKey )
{
    UFC::AnsiString OrderKey;
    OrderInfo*      OrigOrderInfo;

    OrderKey.Printf("%s%s", IB.c_str(), OID.c_str());
    if( (OrigOrderInfo = FOIDMap.GetObjectByKey( OrderKey )) != NULL )
    {
        UFC::PLockObject Lock( FUpdateCS );

        if( OrigOrderInfo->FCancleNIDs.ItemCount() > 0 )
        {
            CancelNID = OrigOrderInfo->FCancleNIDs.GetItem( 0 );
            CKey =  OrigOrderInfo->FCancleKeys.GetItem( 0 );
            OrigOrderInfo->FCancleNIDs.Delete( 0 );
            OrigOrderInfo->FCancleKeys.Delete( 0 );            
            Key =  OrigOrderInfo->GetKeyString();
            LogModify( FALSE, OID, IB, CancelNID,CKey, FCancelNIDFile );
            return TRUE;
        }
        else
        {
            CancelNID = OrigOrderInfo->GetNID();
            Key  =  OrigOrderInfo->GetKeyString();
            CKey =  OrigOrderInfo->GetKeyString();
            return TRUE;
        }
    }
    CancelNID = -1;
    Key = "";
    CKey = "";
    return FALSE;
}
//------------------------------------------------------------------------------
// For TCP/IP TMP use
//------------------------------------------------------------------------------
BOOL OrderInfoStore::PopCancelNID(  const Int32& CancelNID,const UFC::AnsiString& OID,  const UFC::AnsiString& IB, UFC::AnsiString& Key, UFC::AnsiString& CKey )
{
    OrderInfo* OrigOrderInfo = FNIDMap.GetObjectByKey( CancelNID ); ///< Search by NID
    if( OrigOrderInfo == NULL ) ///< Search by IB + OID
    {
        UFC::AnsiString OrderKey;
        OrderKey.Printf("%s%s", IB.c_str(), OID.c_str());
        OrigOrderInfo = FOIDMap.GetObjectByKey( OrderKey );
    }
    if( OrigOrderInfo != NULL )
    {
        UFC::PLockObject Lock( FUpdateCS );

        Key  =  OrigOrderInfo->GetKeyString();
        for( register int i = 0; i< OrigOrderInfo->FCancleNIDs.ItemCount(); i++ )
        {
            if( CancelNID == OrigOrderInfo->FCancleNIDs.GetItem( i ) )
            {
                CKey =  OrigOrderInfo->FCancleKeys.GetItem( i );
                OrigOrderInfo->FCancleNIDs.Delete( i );
                OrigOrderInfo->FCancleKeys.Delete( i );             
                return TRUE;
            }
        }
        CKey =  OrigOrderInfo->GetKeyString();
        return TRUE;
    }    
    Key = "NULL";
    CKey = "NULL";
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL OrderInfoStore::PopReplaceNID( const UFC::AnsiString& OID,  const UFC::AnsiString& IB, Int32& ReplaceNID, UFC::AnsiString& Key, UFC::AnsiString& CKey )
{
    UFC::AnsiString OrderKey;
    OrderInfo*      OrigOrderInfo;

    OrderKey.Printf("%s%s", IB.c_str(), OID.c_str());
    if( (OrigOrderInfo = FOIDMap.GetObjectByKey( OrderKey )) != NULL )
    {
        UFC::PLockObject Lock( FUpdateCS );

        if( OrigOrderInfo->FReplaceNIDs.ItemCount() > 0 )
        {
            ReplaceNID = OrigOrderInfo->FReplaceNIDs.GetItem( 0 );
            CKey       = OrigOrderInfo->FReplaceKeys.GetItem( 0 );
            OrigOrderInfo->FReplaceNIDs.Delete( 0 );
            OrigOrderInfo->FReplaceKeys.Delete( 0 );
            Key =  OrigOrderInfo->GetKeyString();
            LogModify( FALSE, OID, IB, ReplaceNID,CKey, FReplaceNIDFile );
            return TRUE;
        }
        else
        {
            ReplaceNID = OrigOrderInfo->GetNID();
            Key  =  OrigOrderInfo->GetKeyString();
            CKey =  OrigOrderInfo->GetKeyString();
            return TRUE;
        }
    }
    ReplaceNID = -1;
    Key = "";
    CKey = "";
    return FALSE;
}
//------------------------------------------------------------------------------
// For TCP/IP TMP use
//------------------------------------------------------------------------------
BOOL OrderInfoStore::PopReplaceNID(  const Int32& ReplaceNID, const UFC::AnsiString& OID,  const UFC::AnsiString& IB, UFC::AnsiString& Key, UFC::AnsiString& CKey )
{
    OrderInfo* OrigOrderInfo = FNIDMap.GetObjectByKey( ReplaceNID );///< Search by NID
    if( OrigOrderInfo == NULL )///< Search by IB + OID
    {
        UFC::AnsiString OrderKey;
        OrderKey.Printf("%s%s", IB.c_str(), OID.c_str());
        OrigOrderInfo = FOIDMap.GetObjectByKey( OrderKey );
    }
    if( OrigOrderInfo != NULL )
    {
        UFC::PLockObject Lock( FUpdateCS );

        Key =  OrigOrderInfo->GetKeyString();
        for( register int i = 0; i< OrigOrderInfo->FReplaceNIDs.ItemCount(); i++ )
        {   ///< Find the Replace key by NID.
            if( ReplaceNID == OrigOrderInfo->FReplaceNIDs.GetItem( i ) )
            {
                CKey =  OrigOrderInfo->FReplaceKeys.GetItem( i );                
                OrigOrderInfo->FReplaceNIDs.Delete( i );
                OrigOrderInfo->FReplaceKeys.Delete( i );                
                return TRUE;
            }
        }
        ///< Not found!
        CKey =  OrigOrderInfo->GetKeyString();
        return TRUE;
    }
    Key = "NULL";
    CKey = "NULL";
    return FALSE;
}
//------------------------------------------------------------------------------
// Executions from Speedys on other servers.
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
void OrderInfoStore::WriteExecutionLog( BOOL IsConfirm, Int32 NID, const UFC::AnsiString& Key,const UFC::AnsiString& Data, const UFC::AnsiString& PBNO, const UFC::AnsiString& Host, const UFC::AnsiString& PVCID, Int32 TMPRptSeq )
{
    UFC::AnsiString Msg,OID,IB;
        
    if( FCluster == TRUE ) ///< Enable Speedy Cluster
    {
        TMPExtFields TMPFields;
        UFC::PLockObject Lock( FExecCS );
        ///< Render the execution log line.

        TMPFields.RptSeq = TMPRptSeq;
        ExecutionParser::Render( Msg, IsConfirm, NID, FExecutions.ItemCount(), Key, Data, PBNO, Host, PVCID, &TMPFields );
        ///< Add to Execution list.
        if( IsConfirm == TRUE &&             ///< It's a order confirm message 
            IsNewOrder( Data, TRUE ) &&      ///< Check the message data. 
            GetOID( NID, OID, IB ) == TRUE ) ///< Get the OID,IB from memory.
            FExecutions.Add( new NewExec( NID, OID, IB, Key, Msg) );///< Is a new order.
        else
            FExecutions.Add( new ExecInfo( Msg ) );
        FExecutionLog->Write( Msg.c_str(), Msg.Length() );
    }
    else
    {
        TMPExtFields TMPFields;
        ///< Render the execution log line.
        TMPFields.RptSeq = TMPRptSeq;
        ExecutionParser::Render( Msg, IsConfirm, NID, 0, Key,Data, PBNO,Host,PVCID, &TMPFields );
        FExecutionLog->Write( Msg.c_str(), Msg.Length() );
    }
    FExecutionLog->Flush();	
}
//---------------------------------------------------------------------------
void OrderInfoStore::OnSpeedyStartup( const UFC::AnsiString& Host)
{
    SyncExecThread* SyncObj = FHostToSyncObj.GetObjectByKey( Host );///< Find the ExecSeq set of the given server.
	
    if( SyncObj != NULL ) 
    {
        SyncObj->SendSequence();
        SyncObj->SetActive( TRUE );		
    }
}
//---------------------------------------------------------------------------
void OrderInfoStore::OnSpeedyStop( const UFC::AnsiString& Host)
{
    SyncExecThread* SyncObj = FHostToSyncObj.GetObjectByKey( Host );///< Find the ExecSeq set of the given server.
	
    if( SyncObj != NULL ) 
    {
	SyncObj->SetActive( FALSE );		
        UFC::BufferedLog::Printf( " Speedy on Host:[%s] stopped, suspend the sending thread.", Host.c_str() );
    }
}
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
void OrderInfoStore::SendSequenceToAllSpeedy( void )
{
    for( int i=0; i< FHostToSyncObj.ItemCount(); i ++ )
    {
	SyncExecThread* SyncObj = FHostToSyncObj.GetItem( i );
	if( SyncObj != NULL ) 
            SyncObj->SendSequence( );		
    }
}
//---------------------------------------------------------------------------
void OrderInfoStore::SetExecSequence( const UFC::AnsiString& Host, Int32 Seq )
{
    SyncExecThread* SyncObj = FHostToSyncObj.GetObjectByKey( Host );///< Find the ExecSeq set of the given server.
	
    if( SyncObj != NULL ) 
    {
        if( Seq > FExecutions.ItemCount() )
        {            
            UFC::BufferedLog::Printf( " Request seq:[%d] larger than current seq:[%d]", Seq, FExecutions.ItemCount());
            Seq = FExecutions.ItemCount();
        }
        SyncObj->SetRecoverFrom( Seq );		
        SyncObj->SetActive( TRUE );		
        UFC::BufferedLog::Printf( " Recover executions to Host:[%s] from seq:[%d] to seq:[%d]", Host.c_str(), Seq, FExecutions.ItemCount());
    }
}
//---------------------------------------------------------------------------
// Execution Sync thread
//---------------------------------------------------------------------------
SyncExecThread::SyncExecThread( BOOL IsOpt, BOOL RecoverMode, const UFC::AnsiString& Host, OrderInfoStore* Store )
:UFC::PThread( NULL, TRUE )
,FHostname( Host )
,FIsActive( FALSE )
,FRecovering( FALSE )
,FStore( Store )
{
    UFC::AnsiString SeqName;

    if( IsOpt == TRUE )
    {
        FSyncSubject = "SYNC.OPT";
        SeqName.Printf( "%s.%s", "SpeedyOptions", Host.c_str() );
    }
    else
    {
        FSyncSubject = "SYNC.FUT";
        SeqName.Printf( "%s.%s", "SpeedyFutures", Host.c_str() );
    }
    FOutSeq = new UFC::PInt32( SeqName, 0 );///< Set sequence to 1 if share memory not exists.
    if( RecoverMode == FALSE )
    {
        *FOutSeq = 0  ;
        FSeqSet.Clear();
    }
    Start();
}
//---------------------------------------------------------------------------
void SyncExecThread::SetMessageSeq( Int32 Seq )
{
    UFC::PLockObject Lock( FSeqCS );

    FSeqSet.Add( Seq );
}
//---------------------------------------------------------------------------
BOOL SyncExecThread::MessageSeqExists( Int32 Seq )
{ 
    UFC::PLockObject Lock( FSeqCS );

    return FSeqSet.Exists( Seq ); 
}
//---------------------------------------------------------------------------
void SyncExecThread::SetRecoverFrom( Int32 Seq )
{
    if( Seq > FStore->FExecutions.ItemCount() )
    {            
        *FOutSeq = FStore->FExecutions.ItemCount();
        UFC::BufferedLog::Printf( " Request seq:[%d] larger than current seq:[%d]", Seq, FStore->FExecutions.ItemCount());
    }
    else if( Seq == FStore->FExecutions.ItemCount() )
    {              
        *FOutSeq = Seq;
        UFC::BufferedLog::Printf( " Request seq:[%d] equals to current seq, Recover finished.", Seq );
    }
    else
    {
        *FOutSeq = Seq;
        FRecovering = TRUE;
        FBeginTime = UFC::GetTickCountMS( );
    }
}
//---------------------------------------------------------------------------
void SyncExecThread::SendSequence( void )
{
    Int32 MaxSeq = 0;
    MTree SyncData;

    while( TRUE ) ///< Find the seq "Hole" or the last seq.
    {
	if( FSeqSet.Exists( MaxSeq ) == FALSE )
            break;
	MaxSeq++;
    }
    SyncData.append( "CMD", 0 );               
    SyncData.append( "SEQ", MaxSeq );               
    SyncData.append( "HOST", UFC::Hostname );///< Send by this server               
    MessageObj->Send( FSyncSubject, FHostname, SyncData, FALSE );
    UFC::BufferedLog::Printf( " Ask Host:[%s] to recover executions from seq:[%d]", FHostname.c_str(), MaxSeq );
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
		MTree     SyncData;
		int       MsgSeq = FOutSeq->ToInt32();
		ExecInfo* Exec   = FStore->FExecutions.GetItem( MsgSeq );

		SyncData.append( "CMD", 1 );               
		SyncData.append( "SEQ", MsgSeq );               
		SyncData.append( "HOST", UFC::Hostname );///< Send by this server               
		if( Exec->IsNewOrder( ) == TRUE  )///< Is a new order?
		{                   
                    ///< You can use NID or IB+OID as a unique key to find an order.
                    SyncData.append( "NID", ((NewExec*)Exec)->GetNID() );                
                    SyncData.append( "OID", ((NewExec*)Exec)->GetOID() ); 
                    SyncData.append( "IB",  ((NewExec*)Exec)->GetIB()  );
                    SyncData.append( "Key", ((NewExec*)Exec)->GetKey() );            
		}				
		SyncData.append( "Log", Exec->GetLog() );               
		MessageObj->Send( FSyncSubject, FHostname, SyncData, FALSE );
		*FOutSeq += 1;
                SendingCount++;
                if( SendingCount%5 == 0 )
                    UFC::SleepMS( 1 ); ///< After sent 5 messages, sleep 1 ms.
            }
            if( FRecovering == TRUE && SendingCount > 0 )
            {
                FRecovering = FALSE;
                UFC::BufferedLog::Printf( " Recover %d executions finished, use %d ms",SendingCount, UFC::GetTickCountMS( ) - FBeginTime  );
            }
            UFC::SleepMS( 50 );
	}
	else
            UFC::SleepMS( 50 );
    }
}
//---------------------------------------------------------------------------

