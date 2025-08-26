//------------------------------------------------------------------------------------------------------------------------
#include "ThresholdManager.h"
//------------------------------------------------------------------------------------------------------------------------
ThresholdManager*               gThresholdManager = NULL;
UFC::PCriticalSection           gThresholdManagerCS;
//------------------------------------------------------------------------------------------------------------------------
// class ThresholdManager
//------------------------------------------------------------------------------------------------------------------------
ThresholdManager::ThresholdManager( int                    RCGroup,
                                    const UFC::AnsiString& MDSHomeDir,
                                    wsThresholdCfgParm&    ThresholdCfgParm,
                                    MDS::MTable*           TRC_Routing )
:FRCGroup( RCGroup )
,FMDSHomeDir( MDSHomeDir )
,FThresholdCfgParm( ThresholdCfgParm )
,FTThreshold( NULL )
,FTRC_Routing( TRC_Routing )
{
        // Create Threshold MDS table and add data
        NewTableThreshold();
}
//------------------------------------------------------------------------------------------------------------------------
ThresholdManager::~ThresholdManager( void )
{
}
//------------------------------------------------------------------------------------------------------------------------
void ThresholdManager::ResetThreshold( MDS::MTable* TRC_Routing )
{
        // Set New table pointer
        FTRC_Routing = TRC_Routing;
	// Clear THRESHOLD table
        ClearTableThreshold( );
        // Add Init data
        InitTableThreshold( );    
}
//------------------------------------------------------------------------------------------------------------------------
void ThresholdManager::InitTableThreshold( void )
{
        UFC::AnsiString IB;
        UFC::AnsiString Account;
        int CheckLevel = 0;
        MDS::MRow	RowRCRouting;        
        // Loop RC_ROUTING Table
        if ( FTRC_Routing->MoveFirst( ) == TRUE )
        {
                do
                {
                        RowRCRouting = FTRC_Routing->GetCurrRow( );
                        CheckLevel = RowRCRouting.GetColumnAsInt( mtdRC_ROUTING::CHECK_LEVEL );
                        if ( CheckLevel == 1 || CheckLevel == 2 )
                        {
                                // Insert into THRESHOLD Table defalut 0.0
                                UpdateTableThresholdByPK( RowRCRouting, CheckLevel, 0.0, 0.0, TRUE );
                        }
                }
                while ( FTRC_Routing->MoveNext( ) == TRUE );
        }
        UFC::BufferedLog::Printf( " [RCRouting] THRESHOLD table Rows=%d", FTThreshold->GetRowCount() );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL ThresholdManager::ClearTableThreshold( void )
{
        MDS::MRow    RowThreshold;
        MDS::MRowSet RsThreshold;
        if ( FTThreshold->GetRowCount( ) == 0 )
                return TRUE;
        if ( FTThreshold->MoveFirst( ) == FALSE )
                return FALSE;
        // Copy all rows add to RowSet
        do
        {
                RowThreshold = FTThreshold->GetCurrRow( );
                RsThreshold.AddRow( &RowThreshold );
        }
        while ( FTThreshold->MoveNext( ) == TRUE );        

        // Delete all THRESHOLD table rows by RowSet
        if ( RsThreshold.MoveFirst( ) == FALSE )
                return FALSE;
        do {
                RowThreshold = RsThreshold.GetCurrRow( );
                RowThreshold.Delete();
                if ( FTThreshold->Update( RowThreshold ) == FALSE )
                {
                       UFC::BufferedLog::Printf( " [RCRouting] Update THRESHOLD fail. <IB:%s> <ACCT:%s> ",
                                                RowThreshold.GetColumnAsString( mtdTHRESHOLD::IB ).c_str(),
                                                RowThreshold.GetColumnAsString( mtdTHRESHOLD::ACCOUNT ).c_str() );
                }
        } while ( RsThreshold.MoveNext() == TRUE );
        return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void ThresholdManager::NewTableThreshold( void )
{
        UFC::AnsiString SchemaDir, TableDir;
        SchemaDir.Printf( "%s/schema", FMDSHomeDir.c_str( ) );
        TableDir.Printf( "%s/table", FMDSHomeDir.c_str( ) );
        // Create THRESHOLD Table
        try
        {
                FTThreshold = new MDS::MTable( "THRESHOLD", SchemaDir, TableDir );
                FTThreshold_PKSize = FTThreshold->GetIndexSizeByIndexName( "PK" );
        }
        catch ( MDS::MDSException& e )
        {
                UFC::BufferedLog::Printf( " [RCRouting] Error! When Create THRESHOLD table failed! <Reason:%s>", e.what( ) );
        }
        // Add Init data
        InitTableThreshold( );
}
//------------------------------------------------------------------------------------------------------------------------
void ThresholdManager::UpdateTableThresholdByPK( MDS::MRow&  RowRCRouting,
                                                 int         CheckLevel,
                                                 double      CanOrder,
                                                 double      RiskRate,
                                                 BOOL        IsInit )
{
        // Get Routing value
        UFC::AnsiString IB      = RowRCRouting.GetColumnAsString( mtdRC_ROUTING::IB );
        UFC::AnsiString Account = RowRCRouting.GetColumnAsString( mtdRC_ROUTING::ACCOUNT );
        double CanOrderLevel    = RowRCRouting.GetColumnAsDouble( mtdRC_ROUTING::CAN_ORDER_LEVEL );
        double RiskRateLevel    = RowRCRouting.GetColumnAsDouble( mtdRC_ROUTING::RISK_RATE_LEVEL );
       
        // Define variable
        MDS::MRow RowThreshold;
        MDS::MDSBinaryObj BinaryObj( FTThreshold_PKSize );
        BinaryObj.AppendInt( FRCGroup ); ///< Group
        BinaryObj.AppendBuffer( IB.c_str(), 3 ); ///< IB
        BinaryObj.AppendBuffer( Account.c_str(), 7 ); ///< Account
        // Get Row from THRESHOLD by < RC_GROUP + IB + ACCOUNT >
        if ( FTThreshold->GetRowByPK( RowThreshold, BinaryObj ) == FALSE ) ///< New Row
        {
                RowThreshold = FTThreshold->NewRow( );
                RowThreshold.SetColumn( mtdTHRESHOLD::RC_GROUP,    FRCGroup );
                RowThreshold.SetColumn( mtdTHRESHOLD::IB,          IB );
                RowThreshold.SetColumn( mtdTHRESHOLD::ACCOUNT,     Account );
                if ( IsInit == TRUE )
                {
                        RowThreshold.SetColumn( mtdTHRESHOLD::FLAG, '0' );
                }
                else
                {
                        RowThreshold.SetColumn( mtdTHRESHOLD::FLAG, '1' );
                }
        }
        // Set CanOrder and RiksRate
        RowThreshold.SetColumn( mtdTHRESHOLD::CAN_ORDER, CanOrder );
        RowThreshold.SetColumn( mtdTHRESHOLD::RISK_RATE, RiskRate );
        // Initial Flag set 0
        if ( IsInit == FALSE  )
        {
                // 0 to 1
                if ( RowThreshold.GetColumnAsChar( mtdTHRESHOLD::FLAG ) == '0' )
                        RowThreshold.SetColumn( mtdTHRESHOLD::FLAG, '1' );
                // 1 to 2 and Check threshold value, RiskRate maybe 0
                if ( ( RowThreshold.GetColumnAsChar( mtdTHRESHOLD::FLAG ) == '1' )
                && ( ( CanOrder < CanOrderLevel ) || ( RiskRate != 0 && RiskRate < RiskRateLevel ) ) )
                {
                        RowThreshold.SetColumn( mtdTHRESHOLD::FLAG, '2' ); ///< Below threshold and RC check.
                        UFC::BufferedLog::Printf( " [RCRouting] Below threshod <IB:%s> <ACCT:%s> <CheckLevel:%d> <CanOrderLevel:%14.2f> <RiskRateLevel:%10.2f> <CanOrder:%14.2f> <RiskRate:%10.2f> ",
                                                  IB.c_str(), Account.c_str(), CheckLevel, CanOrderLevel, RiskRateLevel, CanOrder, RiskRate );
                }
        }
       // Update
        if ( FTThreshold->Update( RowThreshold ) == FALSE )
        {
                UFC::BufferedLog::Printf( " [RCRouting] Update THRESHOLD fail. <IB:%s> <ACCT:%s> ",
                                         RowThreshold.GetColumnAsString( mtdTHRESHOLD::IB ).c_str(),
                                         RowThreshold.GetColumnAsString( mtdTHRESHOLD::ACCOUNT ).c_str() );
        }
//        UFC::BufferedLog::Printf(    " [RCRouting] UpdateTableThreshold <%s> <%s> <CheckLevel:%d> <Flag:%c> <%14.2f> <%10.2f> <%14.2f> <%10.2f> ",
//                                       RowThreshold.GetColumnAsString( mtdTHRESHOLD::IB ).c_str(),
//                                       RowThreshold.GetColumnAsString( mtdTHRESHOLD::ACCOUNT ).c_str(),
//                                       CheckLevel,
//                                       RowThreshold.GetColumnAsChar( mtdTHRESHOLD::FLAG ),
//                                       CanOrderLevel,
//                                       RiskRateLevel,
//                                       RowThreshold.GetColumnAsDouble( mtdTHRESHOLD::CAN_ORDER ),
//                                       RowThreshold.GetColumnAsDouble( mtdTHRESHOLD::RISK_RATE ) );                   
        
        UFC::BufferedLog::DebugPrintf( UFC::dlInformation,
                                       " [RCRouting] UpdateTableThreshold <%s> <%s> <CheckLevel:%d> <Flag:%c> <%14.2f> <%10.2f> <%14.2f> <%10.2f> ",
                                       RowThreshold.GetColumnAsString( mtdTHRESHOLD::IB ).c_str(),
                                       RowThreshold.GetColumnAsString( mtdTHRESHOLD::ACCOUNT ).c_str(),
                                       CheckLevel,
                                       RowThreshold.GetColumnAsChar( mtdTHRESHOLD::FLAG ),
                                       CanOrderLevel,
                                       RiskRateLevel,
                                       RowThreshold.GetColumnAsDouble( mtdTHRESHOLD::CAN_ORDER ),
                                       RowThreshold.GetColumnAsDouble( mtdTHRESHOLD::RISK_RATE ) );           
}
//------------------------------------------------------------------------------------------------------------------------
BOOL ThresholdManager::CheckThreshold( const char* IB, const char* Account )
{
        MDS::MRow RowThreshold;
        MDS::MDSBinaryObj BinaryObj( FTThreshold_PKSize );
        // Get Row from THRESHOLD by < RC_GROUP + IB + ACCOUNT >
        BinaryObj.AppendInt( FRCGroup ); ///< Group
        BinaryObj.AppendBuffer( IB, 3 ); ///< IB
        BinaryObj.AppendBuffer( Account, 7 ); ///< Account
        if ( FTThreshold->GetRowByPK( RowThreshold, BinaryObj ) == TRUE )
        {
                if ( RowThreshold.GetColumnAsChar( mtdTHRESHOLD::FLAG ) == '1' )
                        return TRUE;
        }
        return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL ThresholdManager::QueryRemoteThreshold( UFC::AnsiString& IB, UFC::AnsiString& Account, UFC::PSocket* pSocket, double& CanOrder, double& RiskRate )
{
        // Define Varable
        UFC::AnsiString  AcctGrpAe;
        RCQGNameValueSet NameValuePairs;
        AcctGrpAe.Printf( "%7s0000000", Account.c_str() );
        double CanUse       = 0.0;
        double OrderIM      = 0.0;
        double OrderPremium = 0.0;
        // Set NameValue
       	NameValuePairs.SetValue( "TABLE", "CUST_SUM" );
	NameValuePairs.SetValue( "INDEX", "KEY1" );
	NameValuePairs.SetValue( "ACTION", "Q" );
        NameValuePairs.SetValue( "IB", IB.c_str() );
        NameValuePairs.SetValue( "ACC_GRP_AE", AcctGrpAe.c_str() );
        RCQGPacket SendPacket( RCQGP_QUERY_COMMAND, NameValuePairs );
        SendPacket.SetReserveBytes( 0 );
        char   Splits[] = "|\n";
        char*  StatusPoint;  ///< the status variable to record its strtok_r() progress
        char*  TokenStr;
        RCQGPacket      ReceivePacket;
        UFC::AnsiString Name, Value;
        // Send Data to Socket
        SendPacket.SerializeToSocket( pSocket );
        // Receive Data from Socket
        while ( TRUE )
        {
                ReceivePacket.LoadFromSocket( pSocket );
                ReceivePacket.GetData( )->GetNameValueByIndex( 0, Name, Value );
                if ( Name == "H" )
                {
                        continue;
                }
                else if ( Name == "D" )
                {
                        // Split Value
                        for ( int i = 0; i < 34; i++ )
                        {
                                if ( i == 0 )
                                        TokenStr = strtok_r( (char*)Value.c_str(), Splits, &StatusPoint );
                                else
                                        TokenStr = strtok_r( NULL, Splits, &StatusPoint );
                                if ( TokenStr == NULL )
                                        break;
                                // Get CanUse
                                if ( i == mtdCUST_SUM::CANUSE_MARGIN )
                                {
                                        CanUse = atof( TokenStr );
                                        continue;
                                }
                                // Get ORDER_IM
                                if ( i == mtdCUST_SUM::ORDER_IM )
                                {
                                        OrderIM = atof( TokenStr );
                                        continue;
                                }
                                // Get ORDER_PREMIUM
                                if ( i == mtdCUST_SUM::ORDER_PREMIUM )
                                {
                                        OrderPremium = atof( TokenStr );
                                        continue;
                                }
                               // Get RISK_RATE_TOT
                                if ( i == mtdCUST_SUM::RISK_RATE_TOT )
                                {
                                        RiskRate = atof( TokenStr );
                                        break;
                                }
                        }
                        // CanOrder = CANUSE_MARGIN - ORDER_IM - ORDER_PREMIUM
                        CanOrder = CanUse - OrderIM - OrderPremium;
                }
                else if ( Name == "T" || Name == "P" ) ///< Get data end
                {
                        break;
                }
                else if ( Name == "E" )                ///< Get data fail
                {
                        return FALSE;
                }
        }
        return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void ThresholdManager::QueryRCThreshod( UFC::PSocket* pSocket, UFC::AnsiString IndexIP, UFC::AnsiString Key, int CheckLevel )
{
        // Define variable
        UFC::AnsiString IB;
        UFC::AnsiString Account;
        double CanOrder = 0.0;
        double RiskRate = 0.0;
        int KeySize = FTRC_Routing->GetIndexSizeByIndexName( Key.c_str() );
	MDS::MRowSet RsRCRouting;
	MDS::MRow    RowRCRouting;
	MDS::MDSBinaryObj BinaryObj( KeySize );
        // Set Key
        BinaryObj.AppendInt( CheckLevel );
        BinaryObj.AppendAnsiString( IndexIP, 15 );
	FTRC_Routing->GetRowSetByIndexName( RsRCRouting, Key, BinaryObj );
	if ( RsRCRouting.GetRowCount() > 0 )
	{
		RsRCRouting.MoveFirst();
		do {
			RowRCRouting = RsRCRouting.GetCurrRow( );
                        IB      = RowRCRouting.GetColumnAsString( mtdRC_ROUTING::IB );
                        Account = RowRCRouting.GetColumnAsString( mtdRC_ROUTING::ACCOUNT );
                        if ( QueryRemoteThreshold( IB, Account, pSocket, CanOrder, RiskRate ) == TRUE )
                        {
                                // Update THRESHOLD Table
                                UpdateTableThresholdByPK( RowRCRouting, CheckLevel, CanOrder, RiskRate );
                        }
		} while ( RsRCRouting.MoveNext() == TRUE );
	}
}
//------------------------------------------------------------------------------------------------------------------------
void ThresholdManager::QueryRCThreshodByIP( const char* IP )
{
        // IPAddr = IP and trim it
        UFC::AnsiString IndexIP( IP );
        IndexIP.TrimLeft( );
        IndexIP.TrimRight( );
        UFC::AnsiString IPAddr( IndexIP );
        // IP 127.0.0.1 to local
        if ( IndexIP == "127.0.0.1" )
                IndexIP = "local";
        // Pad to MDS table index find use
        IndexIP.PadThis( 15, ' ' );
        // IPAddr local to 127.0.0.1
        if ( IPAddr == "local" )
                IPAddr = "127.0.0.1";
        // Query RCServer
        UFC::PSocket *pSocket = new UFC::PSocket( SOCK_STREAM, FALSE );
        if ( pSocket->OpenSocket( ) > 0 )
        {
                // Create Connection
                try
                {
                        pSocket->Connect( IPAddr, FThresholdCfgParm.QueryGatewayPort, 10 );
                        pSocket->SetTCPNoDelay( TRUE );        
                        // Send and recive
                        QueryRCThreshod( pSocket, IndexIP, "KEY1", 1 ); ///< RCServer Host1 and CheckLevel 1
                        QueryRCThreshod( pSocket, IndexIP, "KEY1", 2 ); ///< RCServer Host1 and CheckLevel 2
                        QueryRCThreshod( pSocket, IndexIP, "KEY2", 1 ); ///< RCServer Host2 and CheckLevel 1
                        QueryRCThreshod( pSocket, IndexIP, "KEY2", 2 ); ///< RCServer Host2 and CheckLevel 2
                }       
                catch ( UFC::SocketException &e )
                {
                        UFC::BufferedLog::Printf( " Socket err :%s", e.what( ) );
                }
        }
        else
        {
                UFC::BufferedLog::Printf( " [IP:%s] Socket open fail.", IP );
        }
        // Remove client socket
        pSocket->CloseSocket();
        delete pSocket;        
}
//------------------------------------------------------------------------------------------------------------------------
