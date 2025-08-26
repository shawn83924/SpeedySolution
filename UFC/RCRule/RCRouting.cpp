//------------------------------------------------------------------------------------------------------------------------
#include "RCRouting.h"
#include "SpeedyRCCommandReceiver.h"
//------------------------------------------------------------------------------------------------------------------------
SpeedyRCCommandReceiver*        gSpeedyRCCommandReceiver = NULL;
//------------------------------------------------------------------------------------------------------------------------
// class DBResultSubject
//------------------------------------------------------------------------------------------------------------------------
DBResultSubject::DBResultSubject( BOOL IsOption, key_t Key, long MessageType, int Q_Permission, int nbytes, BOOL IsCreateQueue )
: UFC::PThread( NULL )
, FQKey( Key )
, FQmtype( MessageType )
, FQPermission( Q_Permission )
, FQBytes( nbytes )
, FQIsCreate( IsCreateQueue )
, FIsOption( IsOption )
{
	AttachQueue();
}
//------------------------------------------------------------------------------------------------------------------------
DBResultSubject::~DBResultSubject()
{
}
//------------------------------------------------------------------------------------------------------------------------
void DBResultSubject::Execute( void )
{
	int n;

	memset(&FQRcvData, 0, sizeof(FQRcvData));
	while ( TRUE )
	{
		if ( (n = FQueue.DeQueue(&FQRcvData, FQBytes, FQmtype, FALSE)) < 0 )
		{
			BufferedLog::Printf(" Receive Queue 0x%x failed. <Reason:%s(%d)>"
				, FQKey
				, FQueue.LastErrStr
				, FQueue.LastErrNo );
			exit(0);
		}                
		FQRcvData.mtext[n] = 0;
		FQListenerData->OnQueueDataArrived( FIsOption, &FQRcvData, n );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void DBResultSubject::AttachQueue( void )
{
	if ( !FQueue.AttachQueue(FQKey, FQPermission, FQIsCreate) )
	{
		UFC::BufferedLog::Printf(" Attach Queue 0x%x failed. <Reason:%s(%d)>"
			,FQKey
			,FQueue.LastErrStr
			,FQueue.LastErrNo );
		exit(0);
	}
		
	UFC::BufferedLog::Printf(" AttachQueue 0x%x success.", FQKey);
}
//------------------------------------------------------------------------------------------------------------------------
void DBResultSubject::SetQueueListener( DBResultListener* QListener )
{
	FQListenerData = QListener;
}
//------------------------------------------------------------------------------------------------------------------------
void DBResultSubject::Run( void )
{
	Start();
}
//------------------------------------------------------------------------------------------------------------------------
// class SpeedyReportReceiver
//------------------------------------------------------------------------------------------------------------------------
SpeedyReportReceiver::SpeedyReportReceiver( BOOL IsOption, RCRoutingManager* ReportRoutingManager )
: FIsOption( IsOption )
, FReportRoutingManager( ReportRoutingManager )
{
        if( FIsOption == TRUE )
                FQueueKey = 0x73A51000;
        else
                FQueueKey = 0x72A51000;
}
//------------------------------------------------------------------------------------------------------------------------
void SpeedyReportReceiver::Run( void )
{
	UFC::BufferedLog::Printf( " [SpeedyReportReceiver] thread is start up.");
        UFC::PQueue ReportQueue;
        if ( ReportQueue.Open( FQueueKey, TRUE ) )
        {            
           ReportQueue.Clear(); 
           UFC::BufferedLog::Printf( " [SpeedyReportReceiver] Key=0x%x is clear.", FQueueKey );
        }
        else
        {
           UFC::BufferedLog::Printf( " [SpeedyReportReceiver] Key=0x%x open fail.", FQueueKey );
        }
	FQueueSubject = new UFC::UQueueSubject( FQueueKey, 0, 0666 );
	FQueueSubject->SetQueueListener( this );
	FQueueSubject->Run( );
}
//------------------------------------------------------------------------------------------------------------------------
enRtnOrderReport SpeedyReportReceiver::SendOrderReport( BOOL IsOpt, UFC::UQueueStruct *QueueData , int Len, int& CheckLevel )
{
	UFC::PLockObject Lock( FReportRoutingManager->FReportReceiverCS );
	return FReportRoutingManager->SendOrderReport( IsOpt, QueueData, Len, CheckLevel );
}
//------------------------------------------------------------------------------------------------------------------------
void SpeedyReportReceiver::OnQueueDataArrived( UQueueStruct *QueueData, int Len )
{
        UFC::AnsiString LogStr;
        UFC::AnsiString OrderNo;
        UFC::AnsiString Account;
        UFC::AnsiString NID;
        UFC::AnsiString RCMsg;
        UFC::AnsiString SendStr;
        int		RtnVal;
        int             CheckLevel;
        BOOL		IsConfirm = FALSE;

        // Is Confirm
        if( QueueData->mtext[0] == '3' )
        {
                IsConfirm = TRUE;
        }

        // 1. Send to RCCenter
        RtnVal = SendOrderReport( FIsOption, QueueData, Len, CheckLevel );
        switch ( RtnVal )
        {
                case orErr_NotConnected            : RCMsg = "[RCRoutingNodeRemote] - Connection not connected yet"; break;
                case orErr_SendSocketError         : RCMsg = "[RCRoutingNodeRemote] - Send socket error"; break;
                case orErr_SendQueueError          : RCMsg = "[RCRoutingNodeLocal] - Send queue error"; break;
                case orErr_IPNotInRCListing        : RCMsg = "[RCRoutingManager] - IP not in RC_LISTING table"; break;
                case orErr_GarbageData             : RCMsg = "[RCRoutingManager] - Garbage data, source queue data length error"; break;
                case orErr_RoutingTypeError        : RCMsg = "[RCRoutingManager] - Design time error"; break;                        
                case orErr_RoutingNotFound         : RCMsg = "Routing not found"; break;
                case orSend_NoOneOK  	           : RCMsg = "No one OK"; break;
                case orSend_OK1  	           : RCMsg = "OK1"; break;
                case orSend_OK2      	           : RCMsg = "OK2"; break;
                case orSend_OKAll	           : RCMsg = "OK by all node"; break;
                case orSend_QueryOK1	           : RCMsg = "OK by 1st Query"; break;
                case orSend_OK1AndQueryOK1         : RCMsg = "OK by 1st node and 1st Query"; break;
                case orSend_OK2AndQueryOK1         : RCMsg = "OK by 2st node and 1st Query"; break;
                case orSend_OKAllAndQueryOK1       : RCMsg = "OK by all node and 1st Query"; break;
                case orSend_QueryOK2	           : RCMsg = "No node OK and OK by 2nd Query"; break;
                case orSend_OK1AndQueryOK2         : RCMsg = "OK by all node and 2nd Query"; break;
                case orSend_OK2AndQueryOK2         : RCMsg = "OK by 2nd node and 2nd Query"; break;
                case orSend_OKAllAndQueryOK2       : RCMsg = "OK by all node and 2nd Query"; break;
                case orSend_QueryAllOK	           : RCMsg = "No node OK and OK by all Query"; break;
                case orSend_OK1andQueryAllOK       : RCMsg = "OK by 1st node and all Query"; break;
                case orSend_OK2AndQueryAllOK       : RCMsg = "OK by 2nd node and all Query"; break;
                case orSend_OKAllAndQueryAllOK     : RCMsg = "OK by all node and all Query"; break;
                case orSend_InternalOK             : RCMsg = "Internal OK"; break;             
        }

        // 2. Decompose Text for RC return Log
        if( IsConfirm == TRUE )
        {
                OrderNo	= UFC::AnsiString( QueueData->mtext+21, 5 );
                Account = UFC::AnsiString( QueueData->mtext+26, 7 );
                if( Len == 246 )
                        NID = UFC::AnsiString( QueueData->mtext+130, 8 );
                else
                        NID = UFC::AnsiString( QueueData->mtext+119, 8 );

        }
        else
        {
                if ( QueueData->mtext[0] == '4' && Len == 199 )
                {
                        OrderNo	= UFC::AnsiString( QueueData->mtext+10, 5 );
                        Account = UFC::AnsiString( QueueData->mtext+15, 7 );
                        NID	= UFC::AnsiString( QueueData->mtext+93, 8 );
                }
                else
                {
                        OrderNo	= UFC::AnsiString( QueueData->mtext+8,  5 );
                        Account = UFC::AnsiString( QueueData->mtext+13, 7 );
                        if ( QueueData->mtext[0] == '1' )
                                NID	= UFC::AnsiString( QueueData->mtext+98, 8 );  ///< 1001(206)
                        else
                                NID	= UFC::AnsiString( QueueData->mtext+147, 8 ); ///< 2001(253)
                }
        }
        UFC::BufferedLog::Printf( " *%s* [RPT] [RC REQUEST] [NID:%s] [OrderNo:%s] [Acct:%s] <RC:%3d> <%s> ",
                                        ( RtnVal  >= -1 ) ? "MSG":"ERR",
                                        NID.c_str(),
                                        OrderNo.c_str(),
                                        Account.c_str(),
                                        RtnVal,
                                        RCMsg.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
// class RCRoutingManager
//------------------------------------------------------------------------------------------------------------------------
RCRoutingManager::RCRoutingManager( int                                 RCGroup,
			  	    int                                 RCSourceTag,      ///< 01 ~99
                                    enRoutingType                       RoutingType,
                                    RCSpeedyCheckResultListener*	SpeedyResultListener,
				    const UFC::AnsiString&		MDSHomeDir,                                   
                                    const UFC::AnsiString&              ClientSystem,
                                    wsThresholdCfgParm&                 ThresholdCfgParm )
: FRCGroup( RCGroup )
, FRCSourceTag( RCSourceTag )
, FRoutingType( RoutingType )
, FIsSpeedyUse( TRUE )
, FResultListener( NULL )
, FSpeedyResultListener( SpeedyResultListener )
, FMDSHomeDir( MDSHomeDir )
, FIsSendReportAnyCL( FALSE  )
, FIsBreakCheck( FALSE )
, FIsBreakReport( FALSE )
, FClientSystem( ClientSystem )   
, FThresholdCfgParm( ThresholdCfgParm )
, FTCustomAccount_PKSize(0)       
, FTRC_Listing( NULL )            
, FTRC_Routing( NULL ) 
, FTRC_Routing_Query( NULL ) 
, FTCustomAccount( NULL ) 
, FFutSpeedyReportReceiver( NULL ) 
, FOptSpeedyReportReceiver( NULL ) 
, FQueueDBCheck_Fut( NULL )
, FQueueDBCheck_Opt( NULL )
, FDBSubject_Fut( NULL )
, FDBSubject_Opt( NULL )
{
	// 1. Set RCSource format:xxx.xxx.xxx.xxx-xx 
	FRCSourceStr.Printf( "%s-%02d", UFC::PSocket::GetLocalIP().c_str(), FRCSourceTag );

	// 2. Create RC_Listing & RC_Routing table
        NewAllTable();
	NewAllTableSpeedy( );

	// 3. Set RC_Routing table PK size
	FTRC_Routing_PKSize        = FTRC_Routing->GetIndexSizeByIndexName( "PK" );
        if ( FRoutingType == rtReport )
                FTRC_Routing_Query_PKSize  = FTRC_Routing_Query->GetIndexSizeByIndexName( "PK" );
        FTCustomAccount_PKSize     = FTCustomAccount->GetIndexSizeByIndexName( "PK" );
        
        // 5. Create only one ThresholdManager for Speedy check and report use 
        CreateThresholdManager( );

	// 6. Create all RCRouting nodes by RC_LISTING table
	CreateNodesByListingTable( );

        // 7. Create only one RCCommandReceiver for Speedy check and report use 
        CreateSpeedyRCCommandReceiver( );

	// 8. Create DB Check Objects
	CreateDBCheckObjects( );

        // 9. Create FUT and OPT ReportReceiver for Speedy use
        CreateSpeedyReportReceiver( );
}
//------------------------------------------------------------------------------------------------------------------------
RCRoutingManager::RCRoutingManager( int                                 RCGroup,
				    int                                 RCSourceTag, ///< 01 ~99
				    enRoutingType                       RoutingType,
				    RCCheckResultListener*              ResultListener,
				    const UFC::AnsiString&              MDSHomeDir,
				    BOOL                                IsSendReportAnyCL,
                                    wsThresholdCfgParm&                 ThresholdCfgParm )
: FRCGroup( RCGroup )
, FRCSourceTag( RCSourceTag )
, FRoutingType( RoutingType )
, FIsSpeedyUse( FALSE )
, FResultListener( ResultListener )
, FSpeedyResultListener( NULL )
, FMDSHomeDir( MDSHomeDir )
, FIsSendReportAnyCL( IsSendReportAnyCL )
, FIsBreakCheck( FALSE )
, FIsBreakReport( FALSE )
, FThresholdCfgParm( ThresholdCfgParm )
, FTCustomAccount_PKSize(0)  
, FTRC_Listing( NULL )         
, FTRC_Routing( NULL )
, FTRC_Routing_Query( NULL )
, FTCustomAccount( NULL )
, FFutSpeedyReportReceiver( NULL ) 
, FOptSpeedyReportReceiver( NULL ) 
, FQueueDBCheck_Fut( NULL )
, FQueueDBCheck_Opt( NULL )
, FDBSubject_Fut( NULL )
, FDBSubject_Opt( NULL )
{
	// 1. Set RCSource format:xxx.xxx.xxx.xxx-xx 
	FRCSourceStr.Printf( "%s-%02d", UFC::PSocket::GetLocalIP().c_str(), FRCSourceTag );
	// 2. Create RC_Listing & RC_Routing & RC_Routing_Query table
	NewAllTable( );
	// 3. Set RC_Routing table PK size
	FTRC_Routing_PKSize       = FTRC_Routing->GetIndexSizeByIndexName( "PK" );
        if ( FRoutingType == rtReport )
                FTRC_Routing_Query_PKSize = FTRC_Routing_Query->GetIndexSizeByIndexName( "PK" );
        // 5. Create ThresholdManager
        CreateThresholdManager( );
	// 6. Create all RCRouting nodes by RC_LISTING table
	CreateNodesByListingTable( );
}
//------------------------------------------------------------------------------------------------------------------------
RCRoutingManager::~RCRoutingManager( void )
{
	// 1. Delete all Table and AttachObject
	DeleteAllTableAndAttach( );

	// 2. Stop internal thread first if exist, contain (local) node.
	TIpToRoutingNode::iterator Iter;
	for ( Iter = FNodeContainer.begin(); Iter != FNodeContainer.end(); Iter++ )
	{
		RCRoutingNodeBase* NodePtr = Iter->second;
		NodePtr->StopInternalThread();
	}

	// 3. delete all element of FNodeContainer, contain (local) node.
	for ( Iter = FNodeContainer.begin(); Iter != FNodeContainer.end(); Iter++ )
	{
		delete Iter->second;
	}
	FNodeContainer.clear();

	// 4. DB Check Objects
	if ( FQueueDBCheck_Fut )
		delete FQueueDBCheck_Fut;
	if ( FQueueDBCheck_Opt )
		delete FQueueDBCheck_Opt;
	if ( FDBSubject_Fut )
		delete FDBSubject_Fut;
	if ( FDBSubject_Opt )
		delete FDBSubject_Opt;
        if ( gSpeedyRCCommandReceiver )
            delete gSpeedyRCCommandReceiver;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::NewAllTable( void )
{
	UFC::AnsiString SchemaDir, TableDir;
	SchemaDir.Printf( "%s/schema", FMDSHomeDir.c_str() );
	TableDir.Printf( "%s/table", FMDSHomeDir.c_str() );

	// Create RC_Listing table
	try
	{
		FTRC_Listing = new MDS::MTable( "RC_LISTING", SchemaDir, TableDir );
	}
	catch ( MDS::MDSException& e )
	{
		UFC::BufferedLog::Printf( " [RCRouting] Error! When Create RC_Listing table failed! <Reason:%s>", e.what() );
	}

	// Create RC_Routing table
	try
	{
		FTRC_Routing = new MDS::MTable( "RC_ROUTING", SchemaDir, TableDir );
	}
	catch ( MDS::MDSException& e )
	{
		UFC::BufferedLog::Printf( " [RCRouting] Error! When Create RC_Routing table failed! <Reason:%s>", e.what() );
	}
        
	// Create RC_Routing_Query table
        if ( FRoutingType == rtCheck )
                return;
	try
	{
		FTRC_Routing_Query = new MDS::MTable( "RC_ROUTING_QUERY", SchemaDir, TableDir );
	}
	catch ( MDS::MDSException& e )
	{
		UFC::BufferedLog::Printf( " [RCRouting] Error! When Create RC_Routing_Query table failed! <Reason:%s>", e.what() );
	}        
        
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::NewAllTableSpeedy( void )
{
    UFC::AnsiString SchemaDir, TableDir;
    SchemaDir.Printf( "%s/schema", FMDSHomeDir.c_str() );
    TableDir.Printf( "%s/table", FMDSHomeDir.c_str() );

    try
    {       
        FTCustomAccount = new MDS::MTable( "CUSTOMACCOUNT", SchemaDir, TableDir );
        UFC::BufferedLog::Printf( " [RCRouting] CUSTOMACCOUNT Rows=%d", FTCustomAccount->GetRowCount());
    }
    catch ( MDS::MDSException& e )
    {
        UFC::BufferedLog::Printf( " [RCRouting] Error! When Create CUSTOMACCOUNT table failed! <Reason:%s>", e.what() );
    }               

}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::CreateSpeedyRCCommandReceiver( void )
{
        if ( FRoutingType == rtCheck )
                gSpeedyRCCommandReceiver->GetInstance()->FChkRC = this;
        if ( FRoutingType == rtReport )
                gSpeedyRCCommandReceiver->GetInstance()->FRptRC = this;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::CreateSpeedyReportReceiver( void )
{
        if ( FIsSpeedyUse != TRUE || FThresholdCfgParm.IsActive != TRUE || FRoutingType != rtReport )
                 return;
        PLockObject Lock( FReportReceiverCS );
        if ( FFutSpeedyReportReceiver == NULL )
        {
                FFutSpeedyReportReceiver = new SpeedyReportReceiver( FALSE, this );
                FFutSpeedyReportReceiver->Run();
        }
        if ( FOptSpeedyReportReceiver == NULL )
        { 
                FOptSpeedyReportReceiver = new SpeedyReportReceiver( TRUE, this );
                FOptSpeedyReportReceiver->Run();
        }
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::CreateThresholdManager( void )
{
        if ( FThresholdCfgParm.IsActive != TRUE || FRoutingType != rtReport )
                 return;
        UFC::PLockObject Lock( gThresholdManagerCS );
        if ( gThresholdManager == NULL )
        {
                gThresholdManager = new ThresholdManager( FRCGroup,
                                                          FMDSHomeDir,
                                                          FThresholdCfgParm,
                                                          FTRC_Routing );
        }
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::DeleteAllTableAndAttach( void )
{
	// Delete all RCRoutingObject attach to RC_ROUTING table
	MDS::MRow		Row;
	RCRoutingObject*	RoutingPtr;
	if ( FTRC_Routing!=NULL && FTRC_Routing->GetRowCount() > 0 && FTRC_Routing->MoveFirst() == TRUE )
	{
            do {
                Row = FTRC_Routing->GetCurrRow( );
                if ( ( RoutingPtr = (RCRoutingObject*)Row.GetAttachObject() ) != NULL )
                {
                        delete RoutingPtr;
                        Row.SetAttachObject( NULL );
                }
            } while ( FTRC_Routing->MoveNext() == TRUE );
	}
        
	// Delete all RCRoutingObject attach to RC_ROUTING_QUERY table
	if ( FTRC_Routing_Query!=NULL && FTRC_Routing_Query->GetRowCount() > 0 && FTRC_Routing_Query->MoveFirst() == TRUE )
	{
            do {
                Row = FTRC_Routing_Query->GetCurrRow( );
                if ( ( RoutingPtr = (RCRoutingObject*)Row.GetAttachObject() ) != NULL )
                {
                        delete RoutingPtr;
                        Row.SetAttachObject( NULL );
                }
            } while ( FTRC_Routing_Query->MoveNext() == TRUE );
	}        

	// Delete RC_LISTING & RC_ROUTING table
	if ( FTRC_Listing )       delete FTRC_Listing;
	if ( FTRC_Routing )       delete FTRC_Routing;
        if ( FTRC_Routing_Query ) delete FTRC_Routing_Query;
        FTRC_Listing       = NULL;
        FTRC_Routing       = NULL;
        FTRC_Routing_Query = NULL;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::CreateNodesByListingTable( void )
{
	TIpToRoutingNode::iterator	Iter;
	MDS::MRowSet			RsRCListing;
	MDS::MRow			Row;
	MDS::MDSBinaryObj		BinaryObj( FTRC_Listing->GetIndexSizeByIndexName( "KEY1" ) );
	BinaryObj.AppendInt( FRCGroup ); ///< Group

	FTRC_Listing->GetRowSetByIndexName( RsRCListing, "KEY1", BinaryObj );
	if ( RsRCListing.GetRowCount() > 0 && RsRCListing.MoveFirst() == TRUE )
	{
            do {
                Row = RsRCListing.GetCurrRow( );
                UFC::AnsiString IP = Row.GetColumnAsString( mtdRC_LISTING::IP );
                IP.TrimLeft();
                IP.TrimRight();
                // Create Node when not exist for Reload ReLoadRouting
                if ( ( Iter = FNodeContainer.find( IP ) ) == FNodeContainer.end() )
                {
                        RCRoutingNodeBase* Node;
                        if ( IP == "local" )
                        {
                                if ( FIsSpeedyUse )
                                {
                                        Node = new RCRoutingNodeLocal( FRoutingType, FSpeedyResultListener, this, "127.0.0.1", FRCSourceTag );
                                }
                                else
                                        Node = new RCRoutingNodeLocal( FRoutingType, FResultListener, this, "127.0.0.1", FRCSourceTag );
                        }
                        else
                        {
                                if ( FIsSpeedyUse )
                                        Node = new RCRoutingNodeRemote( FRoutingType, FSpeedyResultListener, this, IP, FRCSourceTag );
                                else
                                        Node = new RCRoutingNodeRemote( FRoutingType, FResultListener, this, IP, FRCSourceTag );
                        }
                        FNodeContainer[ IP ] = Node; ///< Add to Container
                }
                else ///< For threshold old IP have to query again
                {
                        if ( gThresholdManager != NULL && FRoutingType == rtReport )
                                gThresholdManager->QueryRCThreshodByIP( IP.c_str() );
                }
            } while ( RsRCListing.MoveNext() == TRUE );
	}
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingManager::GetRowByRoutingTable( const char* IB, const char* Account, MDS::MRow& Row )
{
	MDS::MDSBinaryObj BinaryObj( FTRC_Routing_PKSize );
	// 1. Get Row from RC_Routing by < RC_GROUP + IB + ACCOUNT >
	BinaryObj.AppendInt( FRCGroup ); ///< Group
	BinaryObj.AppendBuffer( IB,      3 ); ///< IB
	BinaryObj.AppendBuffer( Account, 7 ); ///< Account
	if ( FTRC_Routing->GetRowByPK( Row, BinaryObj ) == TRUE )
		return TRUE;
	// 2. Get Row from RC_Routing by < RC_GROUP + IB + ******* >
	BinaryObj.Seek( ( sizeof(int)+3 ), UFC::soFromBeginning );
	BinaryObj.AppendBuffer( "*******", 7 ); ///< Account : *******
	if ( FTRC_Routing->GetRowByPK( Row, BinaryObj ) == TRUE )
		return TRUE;

	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingManager::GetRowByQueryRoutingTable( const char* IB, const char* Account, MDS::MRow& Row )
{
	MDS::MDSBinaryObj BinaryObj( FTRC_Routing_Query_PKSize );
	// 1. Get Row from RC_Routing_Query by <  IB + ACCOUNT >
	BinaryObj.AppendBuffer( IB,      3 ); ///< IB
	BinaryObj.AppendBuffer( Account, 7 ); ///< Account
	if ( FTRC_Routing_Query->GetRowByPK( Row, BinaryObj ) == TRUE )
		return TRUE;
	// 2. Get Row from RC_Routing by <  IB + ******* >
	BinaryObj.Seek( 3, UFC::soFromBeginning );
	BinaryObj.AppendBuffer( "*******", 7 ); ///< Account : *******
	if ( FTRC_Routing_Query->GetRowByPK( Row, BinaryObj ) == TRUE )
		return TRUE;

	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------
RCRoutingObject* RCRoutingManager::CreateRoutingObject( MDS::MRow& Row, UFC::AnsiString& IP1, UFC::AnsiString& IP2 )
{
	RCRoutingNodeBase *Node1 = NULL;
	RCRoutingNodeBase *Node2 = NULL;
	TIpToRoutingNode::iterator Iter;

	// Process IP1
	IP1.TrimLeft();
	IP1.TrimRight();
    
	if ( IP1.Length() != 0 )
	{
		if ( ( Iter = FNodeContainer.find( IP1 ) ) == FNodeContainer.end() )
		{
			return NULL;
		}
		else
		{
			Node1 = (RCRoutingNodeBase*)Iter->second;
		}
	}

	// Process IP2
	IP2.TrimLeft();
	IP2.TrimRight();
	if ( IP2.Length() != 0 )
	{
		if ( ( Iter = FNodeContainer.find( IP2 ) ) == FNodeContainer.end() )
		{
			return NULL;
		}
		else
		{
			Node2 = (RCRoutingNodeBase*)Iter->second;
		}
	}

	// Attach Node to RCRoutingObject
	RCRoutingObject* RoutingPtr = new RCRoutingObject();
	RoutingPtr->Node1 = Node1;
	RoutingPtr->Node2 = Node2;

	// Set AttachObject of Row
	Row.SetAttachObject( (void*)RoutingPtr );

	return RoutingPtr;
}
//------------------------------------------------------------------------------------------------------------------------
int RCRoutingManager::GetRoutingInfo( const char*	IB,	     /* Input parm */
                                      const char*	Account,     /* Input parm */
                                      MDS::MRow&	Row,	     /* Output parm */
                                      RCRoutingObject&	RoutingObj,  /* Output parm */
                                      BOOL IsQuery )
{
	// 1. Entry FCS_RoutingStruct critical section for ReLoadRouting() case
	UFC::PLockObject Lock( FCS_RoutingStruct );
	RCRoutingObject* RoutingPtr;
        UFC::AnsiString IP1;
        UFC::AnsiString IP2;        
	// 2. Get Row from RC_ROUTING table
        if ( IsQuery == FALSE )
        {
                if ( GetRowByRoutingTable( IB, Account, Row ) == FALSE )
                        return -1; ///< Err_RoutingNotFound
                IP1 = Row.GetColumnAsString( mtdRC_ROUTING::IP1 );
                IP2 = Row.GetColumnAsString( mtdRC_ROUTING::IP2 );
        }
        else ///< 2. Get Row from RC_ROUTING_QUERY table
        {
                if ( GetRowByQueryRoutingTable( IB, Account, Row ) == FALSE )
                        return -1; ///< Err_RoutingNotFound
                IP1 = Row.GetColumnAsString( mtdRC_ROUTING_QUERY::IP1 );
                IP2 = Row.GetColumnAsString( mtdRC_ROUTING_QUERY::IP2 );                
        }
	// 3. Create RCRoutingObject attach to Row of RC_ROUTING when null
	if ( ( RoutingPtr = (RCRoutingObject*)Row.GetAttachObject() ) == NULL )
	{
		if ( ( RoutingPtr = CreateRoutingObject( Row, IP1, IP2 ) ) == NULL )
                {
			return -6; ///< Err_IPNotInRCListing
                }
	}
	// 4. Copy Node1 & Node2 Point, Prevent RoutingPtr been delete when ReLoadRouting
	RoutingObj.Node1 = RoutingPtr->Node1;
	RoutingObj.Node2 = RoutingPtr->Node2;
        return 0;
}        
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::SetRCCenterBreakState( BOOL IsBreakCheck, BOOL IsBreakReport )
{
	FIsBreakCheck  = IsBreakCheck;
	FIsBreakReport = IsBreakReport;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::GetRCCenterBreakState( BOOL& IsBreakCheck, BOOL& IsBreakReport )
{
	IsBreakCheck  = FIsBreakCheck;
	IsBreakReport = FIsBreakReport;
}
//------------------------------------------------------------------------------------------------------------------------
enRtnCheckRequest RCRoutingManager::SendCheckRequest( BOOL                      IsOpt,
                                                      UFC::UQueueStruct*        QueueData,
                                                      int                       Len,
                                                      RCCheckExtendData&        ExtendData )
{
	enRtnCheckRequest RtnVal;
	char IB[4];
	char Account[8];

	// 1-1. Check RoutingType
	if ( FIsSpeedyUse )
		return crErr_RoutingTypeError;
	if ( FRoutingType != rtCheck )
		return crErr_RoutingTypeError;

	// 1-2. Prepare IB Account
	switch ( Len )
	{
	case 213: ///< Normal Order
		strncpy_zero( IB, QueueData->mtext+92, 3 ); ///< IB
		break;
	case 224: ///< Quote Order
		strncpy_zero( IB, QueueData->mtext+103, 3 ); ///< IB
		break;
	default:
		return crErr_GarbageData;
	}
	strncpy_zero( Account, QueueData->mtext+26, 7 ); ///< Account

	// 1-3. Get Routing Info
	MDS::MRow Row;
	RCRoutingObject RoutingObj;
	if ( ( RtnVal = (enRtnCheckRequest)GetRoutingInfo( IB, Account, Row, RoutingObj, FALSE ) ) != 0 )
		return RtnVal;

	int CheckLevel        = Row.GetColumnAsInt( mtdRC_ROUTING::CHECK_LEVEL );
	ExtendData.CheckLevel = CheckLevel;

	// 1-4. Check level 0 10 process
	// Check level 0 -- no check
	if ( CheckLevel == 0 )
	{
		ExtendData.AccountFlag    = Row.GetColumnAsChar( mtdRC_ROUTING::CUSTOM_KIND );
		ExtendData.DayTradePolicy = Row.GetColumnAsChar( mtdRC_ROUTING::DAYTRADE_POLICY );
		return crL0_CheckByClient;
	}
        
	// Check level 10 -- check by DB
	if ( CheckLevel == 10 || ( CheckLevel == 8 || CheckLevel == 9 )  )
	{
		ExtendData.AccountFlag    = ' ';
		ExtendData.DayTradePolicy = ' ';
		return crL10_NeedToDB;
	}
        
	// 1-5. Break RCCenter process
	if ( FIsBreakCheck == TRUE )
	{
		// Check level 2 or 7 or 8 -- no check
		if ( CheckLevel == 2 || CheckLevel == 7 || CheckLevel == 8 )
		{
			ExtendData.AccountFlag    = Row.GetColumnAsChar( mtdRC_ROUTING::CUSTOM_KIND );
			ExtendData.DayTradePolicy = Row.GetColumnAsChar( mtdRC_ROUTING::DAYTRADE_POLICY );
			return crL0_CheckByClient;
		}
		// Check level 1 or 6 -- reject order
		// Check level 9 -- DB eheck
		ExtendData.AccountFlag    = ' ';
		ExtendData.DayTradePolicy = ' ';
		if ( CheckLevel == 1 || CheckLevel == 6 )
			return crErr_RejectOrder;
		else
			return crL10_NeedToDB;
	}

	// 1-6. Validate CheckLevel range
	if ( CheckLevel < 1 || CheckLevel > 9 )
	{
		ExtendData.AccountFlag    = ' ';
		ExtendData.DayTradePolicy = ' ';
		return crErr_CheckLevelError;
	}

        // Check level 1 or 2 --- check by threshold
        if ( gThresholdManager != NULL
        && ( ( RoutingObj.Node1 !=NULL && RoutingObj.Node1->IsLogon() == TRUE )
          || ( RoutingObj.Node2 != NULL && RoutingObj.Node2->IsLogon() == TRUE ) ) ///< RC is connected.
        && ( CheckLevel == 1 || CheckLevel == 2 )
        && FIsBreakReport == FALSE )
        {
          if ( gThresholdManager->CheckThreshold( IB, Account ) == TRUE )
          {
                  ExtendData.AccountFlag    = Row.GetColumnAsChar( mtdRC_ROUTING::CUSTOM_KIND );
                  ExtendData.DayTradePolicy = Row.GetColumnAsChar( mtdRC_ROUTING::DAYTRADE_POLICY );
                  return (enRtnCheckRequest)( CheckLevel + 30 ); ///< Threshold check OK by front
          }
        }

	// 2-1. Try Node1
	RtnVal = crErr_NoRoutingNode;
	if ( RoutingObj.Node1 != NULL )
	{
		RtnVal = RoutingObj.Node1->SendCheckRequest( IsOpt, CheckLevel, QueueData, Len );
		if ( RtnVal == CheckLevel )
			return (enRtnCheckRequest)CheckLevel; ///< SendCheckRequest OK by 1st node
	}

	// 2-2. Try Node2
	if ( RoutingObj.Node2 != NULL )
	{
		RtnVal = RoutingObj.Node2->SendCheckRequest( IsOpt, CheckLevel, QueueData, Len );
		if ( RtnVal == CheckLevel )
			return (enRtnCheckRequest)( CheckLevel + 20 ); ///< SendCheckRequest OK by 2nd node
	}

	// 3. Error process
	// Check level 2 or 7 or 8 -- no check
	if ( CheckLevel == 2 || CheckLevel == 7 || CheckLevel == 8 )
	{
		ExtendData.AccountFlag    = Row.GetColumnAsChar( mtdRC_ROUTING::CUSTOM_KIND );
		ExtendData.DayTradePolicy = Row.GetColumnAsChar( mtdRC_ROUTING::DAYTRADE_POLICY );
		return crL0_CheckByClient;
	}
	// Check level 1 or 6 -- reject order
	ExtendData.AccountFlag    = ' ';
	ExtendData.DayTradePolicy = ' ';
	if ( CheckLevel == 1 || CheckLevel == 6 )
		return crErr_RejectOrder;

	// Check level 9 -- return error, Backend Manager will check by DB
	return RtnVal;
}
//------------------------------------------------------------------------------------------------------------------------
enRtnOrderReport RCRoutingManager::SendOrderReport( BOOL		IsOpt,
                                                    UFC::UQueueStruct*	QueueData,
                                                    int			Len,
                                                    int&		CheckLevel )
{
	int  RtnVal = 0;
        int  SuccessMask = 0;
	char IB[4];
	char Account[8];

	// 1-1. Set CheckLevel default value
	CheckLevel = 10;

	// 1-2. Check RoutingType
	if ( FRoutingType != rtReport )
		return orErr_RoutingTypeError;

	// 1-3. Prepare IB Account
	switch ( Len )
	{
	case 235: ///< Order Confirm
		strncpy_zero( IB, QueueData->mtext+116, 3 );	///< IB
		strncpy_zero( Account, QueueData->mtext+26, 7 );///< Account
		break;
	case 182: ///< QuoteRequest Confirm
		// Do nothing, just return OK All
		return orSend_OKAll;
	case 246: ///< Quote Confirm
		strncpy_zero( IB, QueueData->mtext+127, 3 );	///< IB
		strncpy_zero( Account, QueueData->mtext+26, 7 );///< Account
		break;
	case 204: ///< SingleLeg Fill
		strncpy_zero( IB, QueueData->mtext+95, 3 );	///< IB
		strncpy_zero( Account, QueueData->mtext+13, 7 );///< Account
		break;
	case 253: ///< MultiLeg Fill
		strncpy_zero( IB, QueueData->mtext+144, 3 );	///< IB
		strncpy_zero( Account, QueueData->mtext+13, 7 );///< Account
		break;
	case 199: ///< DR Report
		strncpy_zero( IB, QueueData->mtext+90, 3 );	///< IB
		strncpy_zero( Account, QueueData->mtext+15, 7 );///< Account
		break;
	default:
		return orErr_GarbageData;
	}

        // 2-1. Check if Break RCCenter of Report
        if ( FIsBreakReport == TRUE )
                return orSend_NoOneOK;

	// 2-2. Get Routing Info
	MDS::MRow Row;
	RCRoutingObject RoutingObj;
	if ( ( RtnVal = (enRtnOrderReport)GetRoutingInfo( IB, Account, Row, RoutingObj, FALSE ) ) == 0 )
        {
                // 2-3. Get CheckLevel
                CheckLevel = Row.GetColumnAsInt( mtdRC_ROUTING::CHECK_LEVEL );
                if ( FIsSendReportAnyCL == TRUE || ( CheckLevel != 0 && CheckLevel != 10 ) )
                {
                        // 2-4. Send Node1
                        if ( RoutingObj.Node1 != NULL )
                        {
                                RtnVal = RoutingObj.Node1->SendOrderReport( IsOpt, CheckLevel, QueueData, Len );
                                if ( RtnVal == orSend_InternalOK )
                                        SuccessMask += orSend_OK1;
                        }

                        // 2-5. Send Node2
                        if ( RoutingObj.Node2 != NULL )
                        {
                                RtnVal = RoutingObj.Node2->SendOrderReport( IsOpt, CheckLevel, QueueData, Len );
                                if ( RtnVal == orSend_InternalOK )
                                        SuccessMask += orSend_OK2;
                        }
                }
        }
        
        // 3.1. Get Routing Query Info
        int CheckLevelQry = 10;
        RCRoutingObject RoutingQueryObj;
	if ( ( RtnVal = (enRtnOrderReport)GetRoutingInfo( IB, Account, Row, RoutingQueryObj, TRUE ) ) == 0 )
        {
                // 3.2. Get CheckLevel
                CheckLevelQry = Row.GetColumnAsInt( mtdRC_ROUTING_QUERY::CHECK_LEVEL );
                if ( FIsSendReportAnyCL == TRUE || ( CheckLevelQry != 0 && CheckLevelQry != 10 ) )
                {
                        // 3-3. Send Query Node1
                        if ( RoutingQueryObj.Node2 != NULL )
                        {
                                RtnVal = RoutingQueryObj.Node1->SendOrderReport( IsOpt, CheckLevelQry, QueueData, Len );
                                if ( RtnVal == orSend_InternalOK )
                                        SuccessMask += orSend_QueryOK1;
                        }

                        // 3-4. Send Query Node2
                        if ( RoutingQueryObj.Node1 != NULL )
                        {
                                RtnVal = RoutingQueryObj.Node2->SendOrderReport( IsOpt, CheckLevelQry, QueueData, Len );
                                if ( RtnVal == orSend_InternalOK )
                                        SuccessMask += orSend_QueryOK2;
                        }
                }
        } 

	return (enRtnOrderReport)SuccessMask;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::ReLoadRouting( void )
{
	// 1. Entry FCS_RoutingStruct critical section for GetRoutingInfo() case
	//    Long critical section
	UFC::PLockObject Lock( FCS_RoutingStruct );
	// 2. Delete all Table and AttachObject
	DeleteAllTableAndAttach( );
	// 3. Create RC_Listing & RC_Routing table
	NewAllTable( );
	// 4. Reset Threshold table
        if ( FThresholdCfgParm.IsActive == TRUE && FRoutingType == rtReport && gThresholdManager != NULL )
        {
                gThresholdManager->ResetThreshold( FTRC_Routing );
        }
	// 5. Create all RCRouting nodes by RC_LISTING table
	CreateNodesByListingTable( );       
        UFC::BufferedLog::Printf(" [%s] RCRounting reloaded.", FRoutingType != rtReport ? "Check" : "Report" );
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::OnReportReplyArrive( BOOL IsOption, UFC::UQueueData *QueueData, int Len )
{
        // Define variable
        char* DataPtr = NULL;
        char IB[4];
        char Account[8];
        int CheckLevel = 0;
        double CanOrder = 0.0;
        double RiskRate = 0.0;
        MDS::MRow RowRCRouting;
        // Decompose
        DataPtr = ( char* )QueueData->GetData( );
        strncpy_zero( IB, DataPtr+13, 3 );
        strncpy_zero( Account, DataPtr+2, 7 );
        CheckLevel = DataPtr[0] - 48;
        CanOrder = UFC::AnsiString( DataPtr+24, 14 ).ToDouble();
        RiskRate = UFC::AnsiString( DataPtr+38, 10 ).ToDouble();
        // Update Table
        if ( gThresholdManager != NULL )
        {
                MDS::MDSBinaryObj BinaryObj( FTRC_Routing_PKSize );
                // Get Row from RC_Routing by < RC_GROUP + IB + ACCOUNT >
                BinaryObj.AppendInt( FRCGroup ); ///< Group
                BinaryObj.AppendBuffer( IB,      3 ); ///< IB
                BinaryObj.AppendBuffer( Account, 7 ); ///< Account
                if ( FTRC_Routing->GetRowByPK( RowRCRouting, BinaryObj ) == TRUE )
                {
                        gThresholdManager->UpdateTableThresholdByPK( RowRCRouting, CheckLevel, CanOrder, RiskRate );
                }
                else
                {
                        UFC::BufferedLog::Printf(" Not in RC_ROUTING. [Data:%-48.48s] ", DataPtr );               
                }
        }
        else
        {
                UFC::BufferedLog::Printf(" No ThresholdManager. [Data:%-48.48s] ", DataPtr );
        }
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::OnQueueDataArrived( BOOL IsOption, UFC::UQueueStruct *QueueData, int Len )
{
	// DB Check Result from 0x7xA40000 queue ( mtype is 0x78888866 )
	RCSpeedyCheckResultParm SpeedyParm;
	SpeedyParm.IsOpt = IsOption;
	if ( IsOption == TRUE )
		SpeedyParm.MarketType = rcmTWOptions;
	else
		SpeedyParm.MarketType = rcmTWFutures;
        
	if ( Len == LEN_SpeedyCheckDBResult_R010 ) ///< Order
	{
        UFC::AnsiString     CodeStr( QueueData->mtext+2, 3 );
        UFC::AnsiString     OrderStr(QueueData->mtext+5, 75 ); ///< R010
        UFC::AnsiString     OffHour( QueueData->mtext+5+75+84, 1); 
        UFC::AnsiString     TickStr( QueueData->mtext+206, 11 );
        UFC::AnsiString     MsgStr(  QueueData->mtext+222, 80 );
        UFC::AnsiString     SpeedyNIDStr( QueueData->mtext+303, 11 );
        UFC::AnsiString     SpeedySource( QueueData->mtext+314,  4 );
        UFC::AnsiString     SpeedyKeyLen( QueueData->mtext+318,  3 );                
        UFC::AnsiString     SpeedyKeyStr( QueueData->mtext+321, SpeedyKeyLen.ToInt());
                
        SpeedyParm.Tick      =  TickStr.ToInt64();
        SpeedyParm.SpeedyNID =  SpeedyNIDStr.ToInt();
        SpeedyParm.Order     =  OrderStr;
        SpeedyParm.Key       =  SpeedyKeyStr;
        SpeedyParm.Source    =  SpeedySource;
        SpeedyParm.Code      =  CodeStr;
        SpeedyParm.Msg       =  MsgStr;
                SpeedyParm.IsOffHour =  (OffHour=="1") ;
                
	}
	else if ( Len == LEN_SpeedyCheckDBResult_R090 ) ///< Quote
	{
        UFC::AnsiString     CodeStr( QueueData->mtext+2, 3 );
        UFC::AnsiString     OrderStr( QueueData->mtext+5, 86 );         ///< R090
        UFC::AnsiString     OffHour( QueueData->mtext+5+86+84, 1); 
        UFC::AnsiString     TickStr( QueueData->mtext+217, 11 );
        UFC::AnsiString     MsgStr( QueueData->mtext+233, 80 );
        UFC::AnsiString     SpeedyNIDStr( QueueData->mtext+314, 11 );
        UFC::AnsiString     SpeedySource( QueueData->mtext+325,  4 );
        UFC::AnsiString     SpeedyKeyLen( QueueData->mtext+329,  3 );                
        UFC::AnsiString     SpeedyKeyStr( QueueData->mtext+332, SpeedyKeyLen.ToInt());
		SpeedyParm.Tick      = TickStr.ToInt64();
		SpeedyParm.SpeedyNID = SpeedyNIDStr.ToInt();
		SpeedyParm.Order     = OrderStr;
		SpeedyParm.Key       = SpeedyKeyStr;
                SpeedyParm.Source    = SpeedySource;
        SpeedyParm.Code      = CodeStr;
        SpeedyParm.Msg       = MsgStr;
        SpeedyParm.IsOffHour =  (OffHour=="1") ;
	}
	else ///< garbage data
        {
            UFC::BufferedLog::Printf(" ERROR: RCRoutingManager() DBCheckResult recv: garbage data [%s](%d)", QueueData->mtext, Len);
            return;
        }
        
	SpeedyParm.Key.TrimRight( );
	FSpeedyResultListener->OnCheckResultArrive( SpeedyParm );
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::CreateDBCheckObjects( void )
{
	if ( FIsSpeedyUse == TRUE && FRoutingType == rtCheck )
	{
		// Create DBResultSubject
		FDBSubject_Fut = new DBResultSubject( FALSE, 0x72A40000, DBMTypeFromSpeedy, 0666 );
		FDBSubject_Opt = new DBResultSubject( TRUE,  0x73A40000, DBMTypeFromSpeedy, 0666 );
		FDBSubject_Fut->SetQueueListener( this );
		FDBSubject_Opt->SetQueueListener( this );
		FDBSubject_Fut->Run( );
		FDBSubject_Opt->Run( );
                
		// Create DBCheckQueue
		FQueueDBCheck_Fut = new UFC::PQueue( );
		FQueueDBCheck_Opt = new UFC::PQueue( );
		if ( FQueueDBCheck_Fut->Open( 0x72A30000, true ) == false )
		{
			UFC::BufferedLog::Printf( " [RCRouting] Open FUT DBCheck queue(0x72A30000) failed.\n" ); 
			exit(-1);
		}
		if ( FQueueDBCheck_Opt->Open( 0x73A30000, true ) == false )
		{
			UFC::BufferedLog::Printf( " [RCRouting] Open OPT DBCheck queue(0x73A30000) failed.\n" ); 
			exit(-1);
		}
	}
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::SpeedyDoUnCheck( RCSpeedyCheckParm& CheckParm, RCCheckExtendData& ExtendData )
{
	RCSpeedyCheckResultParm SpeedyParm;
	SpeedyParm.MarketType = CheckParm.MarketType;
	SpeedyParm.IsOpt      = CheckParm.IsOpt;
    SpeedyParm.IsOffHour  = CheckParm.IsOffHour;
	SpeedyParm.Tick       = CheckParm.Tick;
	SpeedyParm.SpeedyNID  = CheckParm.SpeedyNID;
	SpeedyParm.Order      = CheckParm.Order;
	SpeedyParm.Key        = CheckParm.Key;
    SpeedyParm.Source     = CheckParm.Source;       
	SpeedyParm.Code       = "00";
	SpeedyParm.Msg        = "Check By Client";
	SpeedyParm.Order[33] = ExtendData.AccountFlag; ///< Replace AccountFlag
	if ( SpeedyParm.Order.Length() == 75 ) ///< R010 Order
	{
		char OpenOffset = SpeedyParm.Order[70];
		if ( SpeedyParm.IsOpt == TRUE ) ///< Option
		{
			if ( OpenOffset != '0' && OpenOffset != '1' )
				SpeedyParm.Order[70] = '1'; ///< Option default 1
		}
		else ///< Future
		{
			if ( OpenOffset != '0' && OpenOffset != '1' && OpenOffset != '2' )
				SpeedyParm.Order[70] = '0'; ///< Future default 0
		}
	}
	else ///< R090 Quote
	{
		SpeedyParm.Order[81] = '9';
	}
	FSpeedyResultListener->OnCheckResultArrive( SpeedyParm );
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::SpeedyDoRejectOrder( RCSpeedyCheckParm& CheckParm, int RtnVal )
{
	RCSpeedyCheckResultParm SpeedyParm;
	SpeedyParm.MarketType = CheckParm.MarketType;
	SpeedyParm.IsOpt      = CheckParm.IsOpt;
    SpeedyParm.IsOffHour  = CheckParm.IsOffHour;
	SpeedyParm.Tick       = CheckParm.Tick;
	SpeedyParm.SpeedyNID  = CheckParm.SpeedyNID;
	SpeedyParm.Order      = CheckParm.Order;
	SpeedyParm.Key        = CheckParm.Key;
        SpeedyParm.Source     = CheckParm.Source;
	switch ( RtnVal )
	{
        case crErr_NotConnected     : SpeedyParm.Code = "31"; SpeedyParm.Msg = "[RCRoutingManager] - Connection not connected yet"; break;
        case crErr_SendSocketError  : SpeedyParm.Code = "32"; SpeedyParm.Msg = "[RCRoutingManager] - Send socket error"; break;
        case crErr_IPNotInRCListing : SpeedyParm.Code = "06"; SpeedyParm.Msg = "[RCRoutingManager] - IP not in RC_LISTING talbe"; break;
        case crErr_NoRoutingNode    : SpeedyParm.Code = "05"; SpeedyParm.Msg = "[RCRoutingManager] - No Routing Node exist"; break;
        case crErr_SendQueueError   : SpeedyParm.Code = "21"; SpeedyParm.Msg = "[RCRoutingManager] - Send queue error"; break;
        case crErr_RejectOrder      : SpeedyParm.Code = "08"; SpeedyParm.Msg = "[RCRoutingManager] - RCCenter system error, reject order"; break;
        case crErr_OrdNoRangeError  : SpeedyParm.Code = "07"; SpeedyParm.Msg = "[RCRoutingManager] - OrderNo range error"; break;
        case crErr_GarbageData      : SpeedyParm.Code = "04"; SpeedyParm.Msg = "[RCRoutingManager] - Garbage data, source queue data length error"; break;
        case crErr_RoutingTypeError : SpeedyParm.Code = "03"; SpeedyParm.Msg = "[RCRoutingManager] - Design time error"; break;
        default                     : SpeedyParm.Code = "99"; SpeedyParm.Msg = "[RCRoutingManager] - Unknow Error"; break;
        }
        FSpeedyResultListener->OnCheckResultArrive( SpeedyParm );
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::SpeedyDoRejectOrder( RCSpeedyCheckParm& CheckParm, int RtnVal, const char* Message )
{
	RCSpeedyCheckResultParm SpeedyParm;
	SpeedyParm.MarketType = CheckParm.MarketType;
	SpeedyParm.IsOpt      = CheckParm.IsOpt;
    SpeedyParm.IsOffHour  = CheckParm.IsOffHour;
	SpeedyParm.Tick       = CheckParm.Tick;
	SpeedyParm.SpeedyNID  = CheckParm.SpeedyNID;
	SpeedyParm.Order      = CheckParm.Order;
	SpeedyParm.Key        = CheckParm.Key;
        SpeedyParm.Source     = CheckParm.Source;
	SpeedyParm.Code.Printf( "%02d", RtnVal );
	SpeedyParm.Msg        = Message;
	FSpeedyResultListener->OnCheckResultArrive( SpeedyParm );
}
//------------------------------------------------------------------------------------------------------------------------
enRtnCheckRequest RCRoutingManager::SpeedyDoSendCheckToDB( RCSpeedyCheckParm& CheckParm )
{
    UFC::AnsiString CheckStr;
    CheckStr.Printf( "%s%-94.94s%-32.32s%011lld%c%011d%-4.4s%03d%-200.200s"
                    , CheckParm.Order.c_str()
                    , (CheckParm.GDD.IsEmpty()) ? "" : CheckParm.GDD.c_str()	///< GDD
                    , (CheckParm.UDD.IsEmpty()) ? "" : CheckParm.UDD.c_str()	///< UDD
                    , CheckParm.Tick                                            ///< TICK
                    , 'N'                                                       ///< YN_FO
                    , CheckParm.SpeedyNID                                       ///< SPEEDY_NID
                    , CheckParm.Source.c_str()                                  ///< SPEEDY_SOURCE(4 bytes)
                    , CheckParm.Key.Length()                                    ///< SPEEDY_KEY length    
                    , CheckParm.Key.c_str() );                                  ///< SPEEDY_KEY    

    // Send Data to Queue
    UFC::UQueueData Data( DBMTypeFromSpeedy, (char*)CheckStr.c_str(), CheckStr.Length() );
    if ( CheckParm.IsOpt ) ///< Option
    {
            try
            {
                    FQueueDBCheck_Opt->Send( Data );
                    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " RCDB[%s](%d)", CheckStr.c_str(), CheckStr.Length() );                    
            }
            catch ( UFC::QueueException& e )
            {
                    UFC::BufferedLog::Printf( " [RCRouting] Error! Write to OPT DBCheck queue(0x73A30000) failed! <Reason:%s>", e.what() );
                    return crErr_SendQueueError;
            }
    }
    else ///< Future
    {
            try
            {
                    FQueueDBCheck_Fut->Send( Data );
                    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " RCDB[%s](%d)", CheckStr.c_str(), CheckStr.Length() );                    
            }
            catch ( UFC::QueueException& e )
            {
                    UFC::BufferedLog::Printf( " [RCRouting] Error! Write to FUT DBCheck queue(0x72A30000) failed! <Reason:%s>", e.what() );
                    return crErr_SendQueueError;
            }
    }

    return crL10_NeedToDB;
}
//------------------------------------------------------------------------------------------------------------------------
enRtnCheckRequest RCRoutingManager::SpeedySendCheckRequest( RCSpeedyCheckParm& CheckParm,
                                                            RCCheckExtendData& ExtendData)
{
	enRtnCheckRequest RtnVal;        
	char  IB[4];
	char  Account[8];

	// 1-1. Check RoutingType
	if ( FIsSpeedyUse == FALSE )
		return crErr_RoutingTypeError;
	if ( FRoutingType != rtCheck )
		return crErr_RoutingTypeError;        

	// 1-2. Prepare IB Account
	strncpy_zero( IB,      CheckParm.GDD.c_str()+13+4, 3 );   ///< IB
	strncpy_zero( Account, CheckParm.Order.c_str()+26, 7 ); ///< Account
        
	int OrderLen = CheckParm.Order.Length();
	// Check Order length
	if ( OrderLen != 75 &&	///< R010 Order
             OrderLen != 86 )	///< R090 Quote
	   return crErr_GarbageData;
	// Check Key length
	if ( FClientSystem=="YUANTA" && CheckParm.Key.Length() < 49 )
		return crErr_GarbageData;

	// 1-3. Get Routing Info
	MDS::MRow Row;
	RCRoutingObject RoutingObj;
	if ( ( RtnVal = (enRtnCheckRequest)GetRoutingInfo( IB, Account, Row, RoutingObj ) ) != 0 )
        {
		return RtnVal;
        }
	int CheckLevel        = Row.GetColumnAsInt( mtdRC_ROUTING::CHECK_LEVEL );
	ExtendData.CheckLevel = CheckLevel;
        
	// 1-4. Check level 0 10 process
	// Check level 0 -- no check
	if ( CheckLevel == 0 )
	{
		ExtendData.AccountFlag    = Row.GetColumnAsChar( mtdRC_ROUTING::CUSTOM_KIND );
		ExtendData.DayTradePolicy = Row.GetColumnAsChar( mtdRC_ROUTING::DAYTRADE_POLICY );
		return crL0_CheckByClient;
	}
        // Check level 10 -- check by DB
        if ( CheckLevel == 10 || ( CheckLevel == 8 || CheckLevel == 9 ) )
	{
		ExtendData.AccountFlag    = ' ';
		ExtendData.DayTradePolicy = ' ';
		return crL10_NeedToDB;
	}
	// 1-5. Break RCCenter process
	if ( FIsBreakCheck == TRUE )
	{
		// Check level 7 or 8 -- no check
		if ( CheckLevel == 2 || CheckLevel == 7 || CheckLevel == 8 )
		{
			ExtendData.AccountFlag    = Row.GetColumnAsChar( mtdRC_ROUTING::CUSTOM_KIND );
			ExtendData.DayTradePolicy = Row.GetColumnAsChar( mtdRC_ROUTING::DAYTRADE_POLICY );
			return crL0_CheckByClient;
		}
		// Check level 1 or 6 -- reject order
		// Check level 9 -- DB check
		ExtendData.AccountFlag    = ' ';
		ExtendData.DayTradePolicy = ' ';
		if ( CheckLevel == 1 || CheckLevel == 6 )
			return crErr_RejectOrder;
		else
			return crL10_NeedToDB;
	}
	// 1-6. Validate CheckLevel range
	if ( CheckLevel < 1 || CheckLevel > 9 )
	{
		ExtendData.AccountFlag    = ' ';
		ExtendData.DayTradePolicy = ' ';
		return crErr_CheckLevelError;
	}
        
        // Check level 1 or 2 --- check by threshold
        if ( gThresholdManager != NULL
        && ( ( RoutingObj.Node1 !=NULL && RoutingObj.Node1->IsLogon() == TRUE )
          || ( RoutingObj.Node2 !=NULL && RoutingObj.Node2->IsLogon() == TRUE ) ) ///< RC is connected.
        && ( CheckLevel == 1 || CheckLevel == 2 ) 
        && FIsBreakReport == FALSE )  
        {
          if ( gThresholdManager->CheckThreshold( IB, Account ) == TRUE )
          {
                  ExtendData.AccountFlag    = Row.GetColumnAsChar( mtdRC_ROUTING::CUSTOM_KIND );
                  ExtendData.DayTradePolicy = Row.GetColumnAsChar( mtdRC_ROUTING::DAYTRADE_POLICY );
                  return (enRtnCheckRequest)( CheckLevel + 30 ); ///< Threshold check OK by front
          }
        }        
        
	// 2-1. Try Node1
	RtnVal = crErr_NoRoutingNode;
	if ( RoutingObj.Node1 != NULL )
	{
		RtnVal = RoutingObj.Node1->SpeedySendCheckRequest( CheckLevel, CheckParm );
		if ( RtnVal == CheckLevel )
	           return (enRtnCheckRequest)CheckLevel; ///< SpeedySendCheckRequest OK by 1st node
	}
	// 2-2. Try Node2
	if ( RoutingObj.Node2 != NULL )
	{
		RtnVal = RoutingObj.Node2->SpeedySendCheckRequest( CheckLevel, CheckParm );
		if ( RtnVal == CheckLevel )
			return (enRtnCheckRequest)( CheckLevel + 20 ); ///< SpeedySendCheckRequest OK by 2nd node
	}
	// 3. Error process
	// Check level 2 or 7 or 8 -- no check
	if ( CheckLevel == 2 || CheckLevel == 7 || CheckLevel == 8 )
	{
		ExtendData.AccountFlag    = Row.GetColumnAsChar( mtdRC_ROUTING::CUSTOM_KIND );
		ExtendData.DayTradePolicy = Row.GetColumnAsChar( mtdRC_ROUTING::DAYTRADE_POLICY );
		return crL0_CheckByClient;
	}
	// Check level 1 or 6 -- reject order
	ExtendData.AccountFlag    = ' ';
	ExtendData.DayTradePolicy = ' ';
	if ( CheckLevel == 1 || CheckLevel == 6 )
		return crErr_RejectOrder;

	// Check level 9 -- return error, Backend Manager will check by DB
	return RtnVal;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::SetGDD( RCSpeedyCheckParm& CheckParm, const ParsedKeyParam ParsedKey)
{	    
        CheckParm.GDD.Printf( "#0D0x78888866%-4.4s%-3.3s%08d%-3.3s%-23.23s%-15.15s%-11.11s%-4.4s%-10.10s"
                            , ParsedKey.AE.c_str()	///< AE
                            , ParsedKey.IB.c_str() 	///< IB
                            , CheckParm.OrderServerNID	///< NID
                            , ParsedKey.CNC.c_str() 	///< CNC
                            , ""   			///< CA
                            , (ParsedKey.IP.IsEmpty()) ? "" : ParsedKey.IP.c_str()	///< IP
                            , ""			///< ID
                            , ""			///< SALE
                            , ""                        ///< Reserve
                 );
}   
//------------------------------------------------------------------------------------------------------------------------
//?????????????????????(51+n)= Len(2) + ?????????????????(7) + ???????????????????(7) + ????(1) + ????(1) + ??????????????(1) + AE(5) + Time(9) + IP(15) + Checksum(3)
//???????????????(51+n)= Len(2) + Blank(6) + GWNID(8) + ????(1) + ????(1) + ??????????????(1) + AE(5) + Time(9) + IP(15) + Checksum(3)
//GDD(94): Version(2) + SysType(1) + "0x" + MType(8) + AE(4) + IB(3) + GWNID(8) + CNC(3) + CA(23) + IP(15) + ID(11) + SALE(4) + Reserve(10)
//------------------------------------------------------------------------------------------------------------------------
//Yuanta OldFormat : 
//       Super8    : AE(4) + "," + X(69)  = "<Proxy:XXX>"(25) + IB(3) + CNS(3) + IP(15) + ForFIX(15) + GWNID(8)
//       SpeedyAPI : AE(4) + "," + X(44)  = IB(3) + CNS(3) + IP(15) + ForFIX(15) + GWNID(8)
//PMF    OldFormat : 
//       Super8    : AE(n) + "," + X(76+n)= "<Proxy:XXX>"(25) + ???????????????(51+n)
//       AdminTools: AE(n) + "," + X(51+n)  = ???????????????(51+n)
//       SpeedyAPI : AE(n) + "," + X( 4+8)= SubAccount(3) + IsKeyIn(1) + GWNID(8)
//
//------------------------------------------------------------------------------------------------------------------------
//Standard Format  : AE(n) + "," + X(n)   = "[<GDD=" + X(94) + "^UDD=" + X(32) + ( "" ??? ^NVS=X(n)" ) + ">]"
//         Super8  : AE(n) + "," + X(25+n)= "<Proxy:XXX>(25) + "[<GDD=" + X(94) + "^UDD=" + X(32) + ( "" ??? ^NVS=X(n)" ) + ">]"
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingManager::ParserGDDUDD( RCSpeedyCheckParm& CheckParm, UFC::AnsiString&   ErrorMsg)
{
    BOOL                StandardGDD     = FALSE;
    UFC::AnsiString     UserData;

    StandardGDD = (CheckParm.Key.AnsiPos("[<GDD=")>0 && CheckParm.Key.AnsiPos(">]")>0)  ;        
    if (StandardGDD)  
    {   
        return GDD_FromStandard(CheckParm, ErrorMsg);        
    }//end if
        
    UserData        = CheckParm.Key.SubString( CheckParm.Key.AnsiPos(',')+1, CheckParm.Key.Length()) ;        
    if (FClientSystem=="PMF")
    {        
       ///< SpeedyProxy+Super888: "<Proxy:XXX>"(25) + ???????????????(51+n)
       if ((UserData.Length() >= 25+51) && (UserData.AnsiPos("<Proxy:") == 0) && (UserData.AnsiPos(">") == 24))
       {
           return GDD_FromSuper888_PMF(CheckParm, ErrorMsg, FALSE);
       }
       ///< AdminTool:            ???????????????(51+n)
       else if ((UserData.Length() >= 51) && (UserData.AnsiPos("46      ") == 0))  // "46" + Blank(6)
       {
           return GDD_FromSuper888_PMF(CheckParm, ErrorMsg, TRUE);
       }
       ///< MONEY888AP or SpeedyAPI or FIX-Mapper(QFX)
       else if (UserData.Length() >= 3)
       {
           return GDD_FromSpeedyAPI_PMF(CheckParm, ErrorMsg);
       }
    }
    else if (FClientSystem=="YUANTA")
    {
            return GDD_FromSpeedyAPI_YUANTA(CheckParm, ErrorMsg) ;            
    }
    return FALSE;
}
//AExx,[<GDD=X(94)^UDD=X(32)^NVS=X(n)>] 
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingManager::GDD_FromStandard( RCSpeedyCheckParm& CheckParm, UFC::AnsiString&   ErrorMsg)
{
    ///< update GWNID to Key    
    UFC::Int32          UserDataPos = CheckParm.Key.AnsiPos(",") + 1;
    char*               UserData    = NULL;
    UFC::AnsiString     GWNIDStr;    
      
    if ( CheckParm.Key.AnsiPos("<Proxy:") == UserDataPos )           ///< Super888+Proxy
       UserData = (char*) CheckParm.Key.c_str() + UserDataPos + 25 ; ///< "<Proxy:XXX>"(25) + "[<GDD="(6) + Version(2) + SysType(1) + "0x" + MType(8) + AE(4) + IB(3) + GWNID(8) + CNC(3) + CA(23) + IP(15) + ID(15) + SALE(4) + Reserve(10)
    else 
       UserData = (char*) CheckParm.Key.c_str() + UserDataPos ;      ///< "[<GDD="(6) + Version(2) + SysType(1) + "0x"(2) + MType(8) + AE(4) + IB(3) + GWNID(8) + CNC(3) + CA(23) + IP(15) + ID(15) + SALE(4) + Reserve(10) 

    GWNIDStr.Printf("%08d", CheckParm.OrderServerNID);
    memcpy( UserData+6+20, GWNIDStr.c_str(), GWNIDStr.Length() );    ///< GWNID(8)      

    //Parser    
    UFC::AnsiString Data( UserData + 2, strlen(UserData) - 4 );    
    UFC::NameValueMessage NameValues( "^" );
    NameValues.FromString( Data );
    
    NameValues.Get( "GDD", CheckParm.GDD );
    NameValues.Get( "UDD", CheckParm.UDD );
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingManager::GDD_FromSuper888_PMF( RCSpeedyCheckParm& CheckParm, UFC::AnsiString&   ErrorMsg, BOOL IsAdminTool)
{
    ParsedKeyParam      ParsedKey;    
    UFC::AnsiString     UserData;
    UFC::Int32          ProxyOffset     = 0;

    // IB
    MDS::MRow Row;
    if (GetRowByCustomAccountTable( (char*) CheckParm.Order.c_str()+14, (char*) CheckParm.Order.c_str()+26, Row ))
    {                    
        ParsedKey.IB = Row.GetColumnAsString(mtdCUSTOMACCOUNT::IBNO);
        ParsedKey.CustomAE = Row.GetColumnAsString(mtdCUSTOMACCOUNT::AENO);
    }
    else
    {
        ErrorMsg.Printf("can't find IB BrokerID[%-7.7s] Account[%-7.7s]", (char*) CheckParm.Order.c_str()+14, (char*) CheckParm.Order.c_str()+26);        
        return FALSE;
    }

    // AE, CNC
    // --------------------------------------------------------------------------           
    if (IsAdminTool)    ///< AdminTool:            ???????????????(51+n)
    {
        ProxyOffset  = 0;        
    }
    else                ///< SpeedyProxy+Super888: "<Proxy:12345671234567890>"(25) + ???????????????(51+n)
    {        
        ProxyOffset  = 25;        
    }
    
    // update GWNID to Key
    UFC::Int32          HeadLen = CheckParm.Key.AnsiPos(",") + 1 ;
    UFC::AnsiString     GWNIDStr;
    GWNIDStr.Printf("%08d", CheckParm.OrderServerNID);
    memcpy( (char*) CheckParm.Key.c_str() + HeadLen + ProxyOffset + 8 , GWNIDStr.c_str(), GWNIDStr.Length());  ///< AE(n)+ ","+ "<Proxy:...>"(25) +        
    UserData        = CheckParm.Key.SubString( CheckParm.Key.AnsiPos(',')+1, CheckParm.Key.Length()) ;            
   
    ParsedKey.AE  = "0000";
    if (UserData.Length() > ProxyOffset+18)           ///< ??????????????
    {               
       char OrderChannel = UserData[ ProxyOffset+18 ];
       if (OrderChannel=='1')           ///< Money888AP
            ParsedKey.CNC = "NW8";
       else if (OrderChannel=='2')      ///< Money888Web
            ParsedKey.CNC = "NW8";
       else if (OrderChannel=='0')      ///< KeyIn
            ParsedKey.CNC = "KSA";
       else
            ParsedKey.CNC = "NS8";      ///< Super888    
    }

    SetGDD(CheckParm, ParsedKey);
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingManager::GDD_FromSpeedyAPI_PMF( RCSpeedyCheckParm& CheckParm, UFC::AnsiString&   ErrorMsg)
{
    ParsedKeyParam      ParsedKey;    
    UFC::AnsiString     UserData;
    
    // update GWNID to Key    
    UFC::AnsiString     GWNIDStr;
    UFC::Int32          HeadLen = CheckParm.Key.AnsiPos(",") + 1 ;
    if (CheckParm.Key.Length() < HeadLen + 12 )
        CheckParm.Key.PadThis( HeadLen + 12, ' ');        
    GWNIDStr.Printf("%08d", CheckParm.OrderServerNID);    
    memcpy( (char*) CheckParm.Key.c_str() + HeadLen + 4 , GWNIDStr.c_str(), GWNIDStr.Length() );   ///< AE(n) + "," + SubAccount(3) + IsKeyIn(1)        
    UserData        = CheckParm.Key.SubString( CheckParm.Key.AnsiPos(',')+1, CheckParm.Key.Length()) ;                

    // IB
    MDS::MRow Row;
    if (GetRowByCustomAccountTable( (char*) CheckParm.Order.c_str()+14, (char*) CheckParm.Order.c_str()+26, Row ))
    {                    
        ParsedKey.IB = Row.GetColumnAsString(mtdCUSTOMACCOUNT::IBNO);
        ParsedKey.CustomAE = Row.GetColumnAsString(mtdCUSTOMACCOUNT::AENO);
    }
    else
    {
        ErrorMsg.Printf("can't find IB BrokerID[%-7.7s] Account[%-7.7s]", (char*) CheckParm.Order.c_str()+14, (char*) CheckParm.Order.c_str()+26);
        return FALSE;
    }

    // --------------------------------------------------------------------------
    //AE
    if (UserData.SubString(0, 3)=="   ")              ///< ?????????????
        ParsedKey.AE = "0000" ;
    else
        ParsedKey.AE.Printf("0%-3.3s",  UserData.c_str());
    //CNC
    if (UserData.Length() >= 4)
    {
        if (UserData[3]=='N')
           ParsedKey.CNC = "NW8"; 
        else
           ParsedKey.CNC = "QFX";  // FIX
    }
    SetGDD(CheckParm, ParsedKey);
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingManager::GDD_FromSpeedyAPI_YUANTA( RCSpeedyCheckParm& CheckParm, UFC::AnsiString& ErrorMsg)
{
    //AExx,IBxCNCIP3456789012345FIX456789012345NID45678
    //----------------------------------------------------------
    //Head="AExx,"
    //Data(44)="IBxCNCIP3456789012345FIX456789012345NID45678"
    ParsedKeyParam      ParsedKey;    
    UFC::Int32          HeadLen = CheckParm.Key.AnsiPos(",") + 1 ;
    UFC::AnsiString     GWNIDStr;
    
    if (CheckParm.Key.Length() < HeadLen + 44)
        CheckParm.Key.PadThis( HeadLen + 44 , ' ');
    
    ///< update GWNID to Key
    GWNIDStr.Printf("%08d", CheckParm.OrderServerNID);        
    memcpy( (char*) CheckParm.Key.c_str() + HeadLen + 36, GWNIDStr.c_str(), GWNIDStr.Length());    

    ParsedKey.AE.Printf("%-4.4s",    CheckParm.Key.c_str());
    ParsedKey.IB.Printf("%-3.3s",    CheckParm.Key.c_str() + HeadLen);
    ParsedKey.CNC.Printf("%-3.3s",   CheckParm.Key.c_str() + HeadLen + 3);
    ParsedKey.IP.Printf("%-15.15s",  CheckParm.Key.c_str() + HeadLen + 3 + 3);
    if (CheckParm.Key.Length() > HeadLen + 44)
        CheckParm.UDD.Printf("%-15.15s", CheckParm.Key.c_str() + HeadLen + 44);

    SetGDD(CheckParm, ParsedKey);    
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void RCRoutingManager::SpeedySendCheckRequest( RCSpeedyCheckParm& CheckParm )
{
	RCCheckExtendData ExtendData;
	enRtnCheckRequest RtnVal;
        UFC::AnsiString   ErrorMsg;

	// Replace SpeedyAPI PositionEffect(OCCode) when ' ' '3' to 'X'
    if ( CheckParm.Order.Length() == 75 ) ///<  If R010 Order 
    {
       if ( CheckParm.Order[70] == ' ' || CheckParm.Order[70] == '3' || CheckParm.Order[70] == 'A' )
          CheckParm.Order[70] = 'X';
    }
	if (ParserGDDUDD( CheckParm, ErrorMsg )==FALSE)
    {
            SpeedyDoRejectOrder( CheckParm, 99, ErrorMsg );
            return ;
    }
    CheckParm.GDD[84] = (CheckParm.IsOffHour) ? '1' : '0' ;
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " GDD[%s] UDD[%s]", CheckParm.GDD.c_str(), (CheckParm.UDD.IsEmpty()) ? "" : CheckParm.UDD.c_str() ) ;
    UFC::BufferedLog::DebugPrintf(UFC::dlInformation, " KEY[%s](%d)",     CheckParm.Key.c_str(), CheckParm.Key.Length() ) ;
	try
	{
		RtnVal = SpeedySendCheckRequest( CheckParm, ExtendData );
	}
	catch ( UFC::Exception& e )
	{
		SpeedyDoRejectOrder( CheckParm, 99, e.what() );
		return;
	}
	catch ( ... )
	{
		SpeedyDoRejectOrder( CheckParm, 99, "Unknow System Error" );
		return;
	}
        
        if ( RtnVal == crL0_CheckByClient  ///< CheckLevel = 0 and failed RCCenter for 2 7 8
          || RtnVal == crL1_OKByFront      ///< CheckLevel = 1 by front
          || RtnVal == crL2_OKByFront )    ///< CheckLevel = 2 by front        
	{
		SpeedyDoUnCheck( CheckParm, ExtendData );
	}
	else if ( RtnVal > 0 && RtnVal != crL10_NeedToDB ) ///< not Error crL0_CheckByClient crL10_NeedToDB call OnCheckResultArrive by RCRoutingNode
	{
		return;
	}
	else if ( RtnVal == crErr_RejectOrder      ||
                  RtnVal == crErr_OrdNoRangeError  ||
                  RtnVal == crErr_GarbageData      ||
                  RtnVal == crErr_RoutingTypeError )
	{
		// Reject Order
		SpeedyDoRejectOrder( CheckParm, RtnVal );
	}
	else ///< check by DB or Routing Error
	{
		enRtnCheckRequest DBRtnVal = SpeedyDoSendCheckToDB( CheckParm );
		if ( DBRtnVal != crL10_NeedToDB )
			SpeedyDoRejectOrder( CheckParm, DBRtnVal );
	}
}
//------------------------------------------------------------------------------------------------------------------------
BOOL RCRoutingManager::GetRowByCustomAccountTable( const char* BrokerID, const char* Account, MDS::MRow& Row )
{
        UFC::PLockObject Lock( FCS_CustomAccount );        
	MDS::MDSBinaryObj BinaryObj( FTCustomAccount_PKSize );
        
	// 1. Get Row from Speedy_CustAccount by < BrokerID + ACCOUNT >
	BinaryObj.AppendBuffer( BrokerID,      7 ); ///< IB
	BinaryObj.AppendBuffer( Account,       7 ); ///< Account
	if ( FTCustomAccount->GetRowByPK( Row, BinaryObj ) == TRUE )
                return TRUE;
        else
                return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------
