#include "SinoPacRMConnection.h"
#include "iniFile.h"
#include <math.h>
#include <float.h>
#include<limits.h>
//------------------------------------------------------------------------------
extern BOOL DumpOnly;
extern BOOL RecoverMode;
extern UFC::AnsiString AppName;
#define EXECID_LEN 64
UFC::AnsiString LEVEL_FILE( "CheckLevel.ini" );
//------------------------------------------------------------------------------
AccountInfo::AccountInfo( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, const UFC::AnsiString& SubAccount )
:FBrokerID( BrokerID )
,FAccount( Account )
,FSubAccount( SubAccount )
,FCanUse( -DBL_MAX )
{}
//------------------------------------------------------------------------------
void AccountInfo::SetCanUse( double CanUse )
{
    UFC::PLockObject Lock( FCS );
    
    FCanUse = CanUse;
}
//------------------------------------------------------------------------------
BOOL AccountInfo::CanPlaceOrder( double SubAmt, double Limit )
{
    UFC::PLockObject Lock( FCS );
    
    if( FCanUse - SubAmt < Limit )
        return FALSE;
    else
    {
        FCanUse -= SubAmt;
        return TRUE;
    }
}
//------------------------------------------------------------------------------
double AccountInfo::GetCanUse()
{
    UFC::PLockObject Lock( FCS );
    return FCanUse;
}
//------------------------------------------------------------------------------
void AccountInfo::CancelOrder( double AddAmt )
{
    UFC::PLockObject Lock( FCS );
    FCanUse += AddAmt;
}
//------------------------------------------------------------------------------
SinoPacRMConnection::SinoPacRMConnection( int RecoverBeginTime )
:UFC::PThread( NULL , FALSE )
,FWaitForR6( TRUE )
,FSpeedyRM( TRUE )
,FToR6Checker( FALSE )
,FIP( "127.0.0.1" )
,FPort( 12345 )
,FGatewayIP( "127.0.0.1" )
,FGatewayPort( 23456 )
,FService("")          
,FNetwork("")           
,FDaemon("")
,FR6Subject("SPEEY.RISK.REQUEST")
,FSpeedySubject( "SPEEY.RISK.REPLY" )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "0000000" )
,FBrokerID( "F002000" )
,FRMSubject( "STG1.SPDT.INFO.T" )
,FRecoverBeginTime( RecoverBeginTime )
{    
    LoadSetting();
    UFC::AnsiString LogName, ExecIDFile;
    LogName.Printf( "../log/%s.%s.log", AppName.c_str(), UFC::GetDateString().c_str() );
    ExecIDFile.Printf( "ExecID.%s.DATA", UFC::GetDateString().c_str() );
    UFC::BufferedLog::Printf( " LogName:[%s]", LogName.c_str() );
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( LogName, 1024, TRUE ) );
    
    UFC::BufferedLog::Printf( "___________________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   SinoPac Risk control backend interface module " );
    UFC::BufferedLog::Printf( "   Use Tibco RV 7.5.4 AIX 5.1 Version         " );
    UFC::BufferedLog::Printf( "          ***For backend R6 version***        " );        
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");    
    
    UFC::BufferedLog::Printf( "   Speedy To R6 Subject[%s]", FR6Subject.c_str() );
    UFC::BufferedLog::Printf( "   R6 To Speedy Subject[%s]", FSpeedySubject.c_str() );
    UFC::BufferedLog::Printf( "   RM Subject:[%s]", FRMSubject.c_str() );
    UFC::BufferedLog::Printf( "   Service[%s]", FService.c_str() );        
    UFC::BufferedLog::Printf( "   Network[%s]", FNetwork.c_str() );        
    UFC::BufferedLog::Printf( "   Daemon[%s]", FDaemon.c_str() );
    UFC::BufferedLog::Printf( "   Recover BeginTime:[%d]", FRecoverBeginTime );
    if( FWaitForR6 )
        UFC::BufferedLog::Printf( "   RMConnection will wait for R6 check." );
    else
        UFC::BufferedLog::Printf( "   RMConnection will not wait for R6 check." );
        
    UFC::BufferedLog::Printf( "___________________________________________________" );       
    ///< Create the connection object.
    RecoverLevelFile();
    FMessageObject = new MessageObject( AppName, "1.0.0", "SinoPac RM connection." ); 
    FMessageObject->AddListener( "SinoPacRMConnection.FUT", UFC::Hostname, this );
    FMessageObject->AddListener( "SinoPacRMConnection.OPT", UFC::Hostname, this );
    
    ///< Set log file name.        
    InitRV( );
    CreateRVMsg();
    
    ///< Load commodity deposite and account limit.
    CommodityDepositTable = new UFC::UiniFile( "./CommodityDeposit.ini", TRUE );
    AccountLimitTable = new UFC::UiniFile( "./AccountLimit.ini", TRUE );
    
    if( RecoverMode == FALSE )
    {
        CommodityDepositTable->Clear();
        AccountLimitTable->Clear();
        FExecIDFile = new UFC::FileStreamEx( ExecIDFile, "w" );
    }
    else
    {
        UFC::BufferedLog::Printf( " Load Commodity Deposit Table:" );
        UFC::Section* TFXSection = CommodityDepositTable->GetSection( "TAIFEX" );
        if( TFXSection != NULL )
        {
            for( register int i = 0; i < TFXSection->ItemCount(); i++ )
            {
                UFC::AnsiString Name, Value;
                TFXSection->GetNameValue( i, Name, Value );
                UFC::BufferedLog::Printf( " [%s]=>[%s]", Name.c_str(), Value.c_str() );
            }
        }
        
        UFC::BufferedLog::Printf( " Load Account Limit:" );
        for( register int i = 0; i < AccountLimitTable->SectionCount(); i++ )
        {
            UFC::Section* TFXSection = CommodityDepositTable->GetSection( i );
            if( TFXSection != NULL )
            {
                UFC::BufferedLog::Printf( " BrokerID:[%s]", TFXSection->GetSectionName().c_str() );
                for( register int i = 0; i < TFXSection->ItemCount(); i++ )
                {
                    UFC::AnsiString Name, Value;
                    TFXSection->GetNameValue( i, Name, Value );
                    UFC::BufferedLog::Printf( "     [%s]=>[%s]", Name.c_str(), Value.c_str() );
                }
            }
        }
        FExecIDFile = new UFC::FileStreamEx( ExecIDFile, "r+" );
        RecoverExecID();
    }
    CommodityDepositTable->Save();
    AccountLimitTable->Save();
    ///< Create the connection object.
    FConnection = new TTaifexConnection( AppName, this );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );                            
    ///< Start running thread.
    FMessageObject->Start();
    FMessageObject->WaitForConnected();
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::RecoverExecID( void )
{
    FExecIDFile->Seek( 0, UFC::soFromBeginning );
    UFC::AnsiString ExecID;
    while( FExecIDFile->GetPosition() < FExecIDFile->GetSize() )
    {
        UFC::MemoryStream Buffer( EXECID_LEN, EXECID_LEN );
        memset( (void*)Buffer.GetBuffer(), '\0', EXECID_LEN );
        FExecIDFile->Read( (void*)Buffer.GetBuffer(), EXECID_LEN );
        ExecID.LoadFromStream( &Buffer );
        ExecID.TrimRight();
        UFC::BufferedLog::DebugPrintf( " ExecID:[%s]", ExecID.c_str() );
        FExecIDTable.Add( ExecID );
    }
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::SaveLevelFile( void )
{
    UFC::FileStreamEx LevelFile( LEVEL_FILE, "w" );
    
    for( register int i = 0; i < AccountRiskLevelMap.ItemCount(); i++ )
    {
        UFC::AnsiString Key, Str;
        UFC::AnsiString* Value;
            
        AccountRiskLevelMap.GetItem( i, Key, Value );
        Str.Printf( "%s=%s\n", Key.c_str(), Value->c_str() );
        LevelFile.Write( Str.c_str(), Str.Length() );
    }
    LevelFile.Flush();
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::RecoverLevelFile( void )
{
    try
    {
        UFC::FileStreamEx LevelFile( LEVEL_FILE, "r" );
        UFC::AnsiString Buffer;
            
        while( LevelFile.ReadLine( Buffer ) )
        {
            UFC::PStringList StrList;
            StrList.SetStrings( Buffer, "=" );
            
            if( StrList.ItemCount() == 2 )
            {
                AccountRiskLevelMap.Add( StrList[0], new UFC::AnsiString( StrList[1].SubString( 0, StrList[1].Length() - 1 ) ) );
                UFC::BufferedLog::Printf( " Recover risk level:[%s]->[%s]", StrList[0].c_str(), StrList[1].SubString( 0, StrList[1].Length() - 1 ).c_str() );
            }
        }
    }
    catch( UFC::FileException& ex )
    {
        UFC::BufferedLog::Printf( " Recover level file exception:[%s]", ex.what() );
    }
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::InitRV( void )
{
    TibrvStatus status;        
    // Open Tibrv
    status = Tibrv::open();
    if (status != TIBRV_OK)
    {
        UFC::BufferedLog::Printf( " Error: could not open TIB/RV, status=%d, text=%s", (int)status, status.getText());
        exit(-1);
    }
    // Initialize the transport with the given parameters or default NULLs.
    status = FRVTransport.create( FService.c_str() ,FNetwork.c_str(), FDaemon.c_str() );
    if (status != TIBRV_OK)
    {
        UFC::BufferedLog::Printf( " Error: could not create transport, status=%d, text=%s",(int)status,status.getText());
        Tibrv::close();
        exit(-1);
    }
    FRVTransport.setDescription( AppName.c_str() );
    
    // Create listeners for ORDER subject.
    status = FOrderlistener.create( Tibrv::defaultQueue(), this, &FRVTransport, FSpeedySubject.c_str());
    if (status != TIBRV_OK)
    {
       UFC::BufferedLog::Printf( " Error: could not create listener on %s, status=%d, text=%s",  FSpeedySubject.c_str(),(int)status,status.getText());
       Tibrv::close();
       exit(-1);
    }
    UFC::BufferedLog::Printf( " Listening on: %s", FSpeedySubject.c_str() );
    
    // Create listeners for Risk Manager subject.
    
    status = FRMInfoListener.create( Tibrv::defaultQueue(), this, &FRVTransport, FRMSubject.c_str());
    if (status != TIBRV_OK)
    {
       UFC::BufferedLog::Printf( " Error: could not create listener on %s, status=%d, text=%s",  FRMSubject.c_str(),(int)status,status.getText());
       Tibrv::close();
       exit(-1);
    }
    UFC::BufferedLog::Printf( " Listening on: %s", FRMSubject.c_str() );
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::CreateRVMsg( void )
{
    TibrvStatus status;        
    UFC::AnsiString Today;
    
    UFC::GetYYYYMMDD( Today );
    FPrefixMsg.addString("default",FPrefixDef.c_str());
    FTrackingMsg.addString("^id^",FTrackingID.c_str());
    FTrackingMsg.addString("^1^[1]",FTracking1.c_str());
    
    FDataMsg.addString("^class^", FClassName.c_str() );
    FDataMsg.addString("TDATE",Today.c_str());
    FDataMsg.addString("BODY","                            ");    
    
    status = FMsg.setSendSubject( FR6Subject.c_str() );
    
    if( status != TIBRV_OK )
    {   // likely wrong subject specified        
        UFC::BufferedLog::Printf( "Error: could not set subject %s into message, status=%d, text=%s\n",FR6Subject.c_str(),(int)status,status.getText());
        Tibrv::close();
        exit(-1);
    }

    FMsg.addI32( "^type^", 1 );
    FMsg.addI32( "^pfmt^", 10 );
    FMsg.addI32( "^ver^", 30 );
    FMsg.addI32( "^encoding^", 2 );
    FMsg.addMsg( "^prefixList^", FPrefixMsg );
    FMsg.addMsg( "^tracking^", FTrackingMsg );
    FMsg.addMsg( "^data^", FDataMsg );
}   
//------------------------------------------------------------------------------
void SinoPacRMConnection::LoadSetting( void )
{
    UFC::AnsiString FileName;
    FileName.Printf( "../cfg/%s.cfg", AppName.c_str() );
    try
    {
        UFC::UiniFile   Config( FileName );
        UFC::AnsiString Value;
        Config.GetValue( "Setting", "MBusIP", FIP );
        UFC::BufferedLog::DebugPrintf( " MBusIP:[%s]", FIP.c_str() );
        
        Config.GetValue( "Setting", "Service", FService );
        Config.GetValue( "Setting", "Network", FNetwork );
        Config.GetValue( "Setting", "Daemon",  FDaemon  );        
        
        Config.GetValue( "Setting", "WaitForR6",  Value  );
        if( Value.ToInt() == 0 )
            FWaitForR6 = FALSE;
        
        Config.GetValue( "Setting", "ToR6Checker",  Value  );
        if( Value.ToInt() > 0 )
            FToR6Checker = TRUE;
        
        Config.GetValue( "ToSpeedy", "Subject",  FSpeedySubject );
        
        Config.GetValue( "Setting", "SpeedyRM", Value );
        if( Value.ToInt() > 0 )
            FSpeedyRM = TRUE;
        else
            FSpeedyRM = FALSE;
        Config.GetValue( "Setting", "RMSubject",  FRMSubject  );
        Config.GetValue( "Setting", "GatewayIP", FGatewayIP );
        if( Config.GetValue( "Setting", "GatewayPort", Value ) )
            FGatewayPort = Value.ToInt();
        
        Config.GetValue( "Setting", "ID", FID );
        Config.GetValue( "Setting", "Password", FPassword );
        Config.GetValue( "Setting", "Account", FAccount );
        Config.GetValue( "ToR6", "Subject",  FR6Subject );
        Config.GetValue( "ToR6", "PrefixDefault", FPrefixDef );
        Config.GetValue( "ToR6", "TrackingID",    FTrackingID );
        Config.GetValue( "ToR6", "Tracking1",     FTracking1 );
        Config.GetValue( "ToR6", "ClassName", FClassName );
        
        Config.GetValue( "RMInfo", "Subject", FRMSubject );
        
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" %s not found.", FileName.c_str() );
        exit(1);
    }
}
//------------------------------------------------------------------------------
SinoPacRMConnection::~SinoPacRMConnection( void )
{
    // Tibrv::close() will destroy the transport and guarantee delivery
    Tibrv::close();
    ///< Delete TTaifexConnection object if exists.    
    if( FConnection != NULL )
    {
        FConnection->Logoff();
        delete FConnection;
    }
}   
//------------------------------------------------------------------------------
void SinoPacRMConnection::onMsg(TibrvListener* listener, TibrvMsg& msg) 
{
    const char* msgString    = NULL;           
    // Convert the incoming message to a string
    msg.convertToString( msgString );    
    if( msgString != NULL )
    {   
        if( listener == &FOrderlistener ) ///< Order from R6
        {
            UFC::BufferedLog::Printf( " Recv:[%s]", msgString);    
            UFC::AnsiString rvmsg( msgString );
            int Pos = rvmsg.AnsiPos( "BODY=" );
            UFC::AnsiString Order( rvmsg.c_str() + Pos + 6, 256 ); 
            
            UFC::BufferedLog::Printf( " Recv result:[%s]",Order.c_str() );

            UFC::AnsiString Value( Order.SubString( 75, 4 ) );
            Int32 ErrorCode = Value.ToInt();            

            char Market = Order[79];
            Value = Order.SubString( 80, 10 );
            Int32 NID = Value.ToInt();
            UFC::AnsiString Key = Order.SubString( 90, 128 );
            UFC::AnsiString OrderID = Order.SubString( 21, 5 );
            Key.TrimRight();
            UFC::BufferedLog::DebugPrintf( " ErrorCode:[%04d] OrderID:[%s]", ErrorCode, OrderID.c_str());
            UFC::BufferedLog::DebugPrintf( " Market:[%c]", Market );
            UFC::BufferedLog::DebugPrintf( " NID:[%010d]", NID );
            UFC::BufferedLog::DebugPrintf( " Key:[%s]", Key.c_str() );

            MTree Data;
            Data.append( "NID", NID );
            Data.append( "KEY", Key );
            Data.append( "ORDER", Order.SubString( 0, 75 ) );
            Data.append( "OID", OrderID );
            if( ErrorCode == 0 ) ///< Pass
                Data.append( "CODE", 1 );
            else
            {
                Data.append( "CODE", 0 );
                UFC::AnsiString MSG;
                MSG.Printf( "Backend office reject this order, Error Code:[%d]", ErrorCode );
                Data.append( "MSG", MSG );
            }
            
            if( Market == 'F' )
                FMessageObject->Send( "RESULT.FUT", UFC::Hostname, Data ); 
            else
                FMessageObject->Send( "RESULT.OPT", UFC::Hostname, Data );
            UFC::BufferedLog::FlushToFile();
        }
        else if( listener == &FRMInfoListener ) ///< CMD from R6
        {
            UFC::BufferedLog::Printf( " Recv:[%s]", msgString);    
            UFC::AnsiString rvmsg( msgString );
            int Pos = rvmsg.AnsiPos( "FNCODE" );
            UFC::AnsiString FNCODE( rvmsg.c_str() + Pos + 8, 4 );
            UFC::BufferedLog::Printf( " FNCODE:[%s]", FNCODE.c_str() );
            Pos = rvmsg.AnsiPos( "BODY=" );
            UFC::AnsiString Body( rvmsg.c_str() + Pos + 6, 256 ); 
                        
            
            if( FNCODE == "9013" )
            {
                UFC::AnsiString Key;   
                UFC::AnsiString BrokerID    = Body.SubString( 0, 7 );
                UFC::AnsiString Account     = Body.SubString( 7, 7 );
                UFC::AnsiString SubAccount  = Body.SubString( 14, 7 ); 
                UFC::AnsiString DoubleValue = Body.SubString( 21, 18 );                                
                double          Price = DoubleValue.ToDouble();
                
                UFC::BufferedLog::Printf( " BrokerID:[%s]", BrokerID.c_str() );
                UFC::BufferedLog::Printf( " Account: [%s]", Account.c_str() );
                UFC::BufferedLog::Printf( " SubAccount:[%s]", SubAccount.c_str() );
                UFC::BufferedLog::Printf( " CANUSE:[%014.2lf]", Price );
                
                Key.Printf( "%s%s%s", BrokerID.c_str(), Account.c_str(),SubAccount.c_str() );
                AccountInfo* Info = AccountInfoTable.GetObjectByKey( Key );
                
                if( Info == NULL ) 
                {
                    Info = new AccountInfo( BrokerID, Account, SubAccount );
                    AccountInfoTable.Add( Key, Info );
                }
                
                Info->SetCanUse( Price );
            }
            else if( FNCODE == "9012" )
            {
                char            Sign,Type;
                UFC::AnsiString DoubleValue,BrokerID,Account,SubAccount;
                double          Price;

                ///< New format 
                BrokerID    = Body.SubString( 0, 7 );
                Account     = Body.SubString( 7, 7 );                
                SubAccount  = Body.SubString( 14, 7 ); 
                Sign        = Body[21];
                DoubleValue = Body.SubString( 22, 17 );                    
                Type        = Body[39];                
                
                Price    = DoubleValue.ToDouble();
                
                if( Sign == '-')
                    Price *= -1;
                    
                UFC::BufferedLog::Printf( " BrokerID:[%s]",    BrokerID.c_str() );
                UFC::BufferedLog::Printf( " Account: [%s]",    Account.c_str() );
                UFC::BufferedLog::Printf( " SubAccount: [%s]", SubAccount.c_str() );
                UFC::BufferedLog::Printf( " LMT:[%014.2lf]", Price );
                UFC::BufferedLog::Printf( " Type:[%c]", Type );
                
                if( Type == 'A' || Type == 'U' )
                    SetAccountLimit( BrokerID, Account, SubAccount,  Price );
                else if( Type == 'D' )
                    DeleteAccountLimit( BrokerID, Account, SubAccount );
                else
                    UFC::BufferedLog::Printf( " Unknown type:[%c]", Type );                
            }
            else if( FNCODE == "9011" )
            {
                UFC::AnsiString DoubleValue = Body.SubString( 11, 17 );
                double Price = DoubleValue.ToDouble();
                UFC::AnsiString Symbol = Body.SubString( 0, 11 );
                Symbol.TrimRight();
                char Type = Body[28];
                UFC::BufferedLog::Printf( " Symbol: [%s]", Symbol.c_str() );
                UFC::BufferedLog::Printf( " OTAMT:[%014.2lf]", Price );
                UFC::BufferedLog::Printf( " Type:[%c]", Type );
                    
                if( Type == 'A' || Type == 'U' )
                    SetCommodOTAMT( Symbol.SubString( 0, 3 ), Price );
                else if( Type == 'D' )
                    DeleteCommodOTAMT( Symbol.SubString( 0, 3 ) );
                else
                    UFC::BufferedLog::Printf( " Unknown type:[%c]", Type );
            }
            else if( FNCODE == "9014" )
            {
                UFC::AnsiString ChkSubAccount, Key;
                UFC::AnsiString BrokerID    = Body.SubString( 0, 7 );
                UFC::AnsiString Account     = Body.SubString( 7, 7 );
                UFC::AnsiString SubAccount  = Body.SubString( 14, 7 ); 
                char            RMLevel     = Body[21];  
                
                if( RMLevel == '0' )
                {
                    ChkSubAccount = "       ";
                    UFC::BufferedLog::Printf( " Check Main Account Margin [%s]", SubAccount.c_str() );
                }
                else if( RMLevel == '1' )
                {
                    ChkSubAccount = SubAccount.SubString( 0, 3 );
                    ChkSubAccount.PadThis( 7,' ' );
                    UFC::BufferedLog::Printf( " Check Department Margin [%s]", SubAccount.c_str() );
                }
                else
                {
                    ChkSubAccount = SubAccount;
                    ChkSubAccount.PadThis( 7,' ' );
                    UFC::BufferedLog::Printf( " Check Trader Margin [%s]", SubAccount.c_str());                    
                }                
                UFC::BufferedLog::Printf( " Map SubAccount[%s]->[%s]", SubAccount.c_str(), ChkSubAccount.c_str());                    
                Key.Printf("%s%s%s", BrokerID.c_str(), Account.c_str(), SubAccount.c_str() );
                AccountRiskLevelMap.Add( Key, new UFC::AnsiString( ChkSubAccount ) );
                
                SaveLevelFile();
            }
        }
        else
        {
            UFC::BufferedLog::DebugPrintf( " Recv un-handled subject." );
        }
    }
    else
    	UFC::BufferedLog::Printf( " Can't convert msg to string.");    
}
//------------------------------------------------------------------------------
int SinoPacRMConnection::CheckSubAccount( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, UFC::AnsiString& SubAccount )
{
    UFC::AnsiString Key;    
    UFC::AnsiString* ChkSubAccount;
    
    Key.Printf("%s%s%s", BrokerID.c_str(), Account.c_str(), SubAccount.c_str() );
    
    if( AccountRiskLevelMap.IsExists( Key ) == TRUE )
    {
        ChkSubAccount = AccountRiskLevelMap.GetObjectByKey( Key );
        UFC::BufferedLog::Printf( " Account[%s] SubAccount[%s]->[%s]", Account.c_str(), SubAccount.c_str(), ChkSubAccount->c_str());                    
        SubAccount = *ChkSubAccount;
        
        if( SubAccount == "       " )
            return 0;
        else if( SubAccount.SubString( 3, 4 ) == "    " )
            return 1; ///< Deparment Account. 
        else
            return 2;///< Trader Account.
    }
    return 0; ///< Exchange Account.
}    
//------------------------------------------------------------------------------
double SinoPacRMConnection::GetCommodOTAMT( const UFC::AnsiString& SymbolPrefix )
{
    UFC::PLockObject Lock( FCommodCS );
    
    UFC::AnsiString Value;
    if( CommodityDepositTable->GetValue( "TAIFEX", SymbolPrefix, Value ) )
        return Value.ToDouble();
    else
        return -1.0;
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::SetCommodOTAMT( const UFC::AnsiString& SymbolPrefix, double Price )
{
    UFC::PLockObject Lock( FCommodCS );
    
    UFC::AnsiString Value;
    Value.Printf( "%014.2lf", Price );
    CommodityDepositTable->SetValue( "TAIFEX", SymbolPrefix, Value );
    CommodityDepositTable->Save();
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::DeleteCommodOTAMT( const UFC::AnsiString& SymbolPrefix )
{
    UFC::PLockObject Lock( FCommodCS );
    
    UFC::Section* TFXSection = CommodityDepositTable->GetSection( "TAIFEX" );
    if( TFXSection != NULL )
    {
        if( TFXSection->ValueExists( SymbolPrefix ) )
        {
            TFXSection->DeleteValue( SymbolPrefix );
            CommodityDepositTable->Save();
        }
    }
}
//------------------------------------------------------------------------------
bool SinoPacRMConnection::IsReplacePx( const UFC::AnsiString& Order )
{
    if( Order.SubString( 2, 2 ).ToInt() == 6 )
        return true;
    return false;
}
//------------------------------------------------------------------------------
UFC::AnsiString SinoPacRMConnection::GetSubAccount( const UFC::AnsiString& Key )
{
    ///< Layout for the "KEY" field.
    ///< AE,Data
    ///< SubAccount in Data offset 12
    UFC::AnsiString SubAccount( "       " );
    Int32           SplitPos = Key.AnsiPos(',');
    
    if( SplitPos !=  -1 )
        SubAccount = Key.SubString( SplitPos + 1 + 12, 7 );
    return SubAccount;
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& , MTree* Data )
{
    if( Subject == "SinoPacRMConnection.FUT" || Subject == "SinoPacRMConnection.OPT" )
    {
        UFC::AnsiString OrderTime, Key, Order, SendMsg;
        Int32 NID;
        
        UFC::GetTimeString( OrderTime, FALSE );        
        Data->get( "ORDER", Order ); 
        Data->get( "NID", NID );
        Data->get( "KEY", Key );
        
        if( IsReplacePx( Order ) )
        {
            Data->append( "CODE", 1 );
            if( Subject == "SinoPacRMConnection.FUT" )
                FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
            else
                FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
            return;
        }
        
        Key.PadThis( 128, ' ' );
        UFC::BufferedLog::Printf( " Recv from Speedy Gateway:[%s]", Order.c_str() );
        Int32 Qty = Order.SubString( 65, 4 ).ToInt();
        UFC::BufferedLog::DebugPrintf( " OrderQty:[%d]", Qty );
        
        char OpenOffset = Order[70];
        if( OpenOffset == '3' )
        {
            OpenOffset = ' ';
            Order[70] = ' ';
        }
        if( Subject == "SinoPacRMConnection.FUT" ) ///< Check Futures Order
        {
            UFC::BufferedLog::DebugPrintf( " Wait for R6:[%d]", FWaitForR6 );
            SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'F', OrderTime.c_str(), ( FWaitForR6 == TRUE )?'Y':'N', NID, Key.c_str() );
            
            if( FSpeedyRM == FALSE )///< Disable Speedy RM 
            {
                
                SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'F', OrderTime.c_str(), ( FWaitForR6 == TRUE )?'Y':'N', NID, Key.c_str() );
                if( FWaitForR6 == FALSE )
                {
                    Data->append( "CODE", 1 );
                    FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                }
            }
            else
            {
                UFC::AnsiString BrokerID( Order.c_str() + 14, 7 );
                UFC::AnsiString Account( Order.c_str() + 26, 7 );                
                UFC::AnsiString SymbolPrefix( Order.c_str() + 34, 3 );
                UFC::AnsiString SubAccount = GetSubAccount( Key );
                UFC::AnsiString SearchKey;
                    
                double          AccountLimit  = 0.0;
                double          ExAccountLimit  = 0.0;
                double          DepAccountLimit  = 0.0;
                double          CommodMargins = GetCommodOTAMT( SymbolPrefix );                                    
                int             Level;                    
                AccountInfo*    Info;
                AccountInfo*    ExInfo;
                AccountInfo*    DepInfo;
                bool            GetInfoFailed = FALSE;
                
                UFC::BufferedLog::Printf( " BrokerID:[%s], Account:[%s][%s] SymbolPrefix:[%s] OpenOffset:[%c]", BrokerID.c_str(), Account.c_str(), SubAccount.c_str(), SymbolPrefix.c_str(), OpenOffset );
                Level = CheckSubAccount( BrokerID, Account, SubAccount );                                
                SearchKey.Printf( "%s%s%s", BrokerID.c_str(), Account.c_str(), SubAccount.c_str() );
                Info = AccountInfoTable.GetObjectByKey( SearchKey );
                
                if( Info == NULL || GetAccountLimit( BrokerID, Account, SubAccount, AccountLimit ) == FALSE )
                {
                    UFC::BufferedLog::Printf( " Can't fetch the Account: [%s]", SearchKey.c_str() );                
                    GetInfoFailed = TRUE;            
                }
                
                if( Level > 1 )///< Need Check Department account
                {                    
                   UFC::AnsiString DepAccount( SubAccount.c_str(), 3 );
                   
                   DepAccount.PadThis( 7,' ');
                   SearchKey.Printf( "%s%s%s", BrokerID.c_str(), Account.c_str(), DepAccount.c_str() );
                   DepInfo = AccountInfoTable.GetObjectByKey( SearchKey ); 
                   
                   if( DepInfo == NULL || GetAccountLimit( BrokerID, Account, DepAccount, DepAccountLimit ) == FALSE  )
                   {
                       UFC::BufferedLog::Printf( " Can't fetch the Department Account: [%s]", SearchKey.c_str() );                                   
                       GetInfoFailed = TRUE;                                                     
                   }
                }
                                               
                if( Level > 0 )///< Need Check Exchange account
                {
                  UFC::AnsiString ExAccount( "       " );
                   
                   ExAccount.PadThis( 7,' ');
                   SearchKey.Printf( "%s%s%s", BrokerID.c_str(), Account.c_str(), ExAccount.c_str() );
                   ExInfo = AccountInfoTable.GetObjectByKey( SearchKey ); 
                   
                   if( ExInfo == NULL || GetAccountLimit( BrokerID, Account, ExAccount, ExAccountLimit ) == FALSE  )
                   {
                       UFC::BufferedLog::Printf( " Can't fetch the Exchange Account: [%s]", SearchKey.c_str() );                                   
                       GetInfoFailed = TRUE;                                                     
                   }
                }
                
                if( GetInfoFailed == TRUE || CommodMargins < 0  || OpenOffset == ' '  ) ///< Send to R6, always wait for R6 checking result.
                {                    
                    UFC::BufferedLog::Printf( " OpenOffset:[%c]", OpenOffset );
                    UFC::BufferedLog::Printf( " AccountLimit:[%lf]", AccountLimit );
                    UFC::BufferedLog::Printf( " CommodOTAMT:[%lf]", CommodMargins );
                    UFC::BufferedLog::Printf( " Send to R6, always wait for R6 checking result." );    
                    SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'F', OrderTime.c_str(), 'Y', NID, Key.c_str() );
                }
                else
                {
                    bool PassCheck ;
                    
                    if( Info->CanPlaceOrder( CommodMargins * Qty, AccountLimit ) ) ///< Pass Account check
                    {
			switch( Level )
			{
                		case 0: UFC::BufferedLog::Printf( " Pass Exchange Account check, CANUSE:[%lf]  LMT:[%lf]", Info->GetCanUse(), AccountLimit );break;
               			case 1: UFC::BufferedLog::Printf( " Pass Department Account check, CANUSE:[%lf]  LMT:[%lf]", Info->GetCanUse(), AccountLimit ); break;
                		case 2: UFC::BufferedLog::Printf( " Pass Trader Account check, CANUSE:[%lf]  LMT:[%lf]", Info->GetCanUse(), AccountLimit );break;
			}
                            
                        if( Level > 0 )///< Need Check Department account
                        {                            
                            if( ExInfo->CanPlaceOrder( CommodMargins * Qty, ExAccountLimit ) ) ///< Pass Exchange Account check
                            {    
                                UFC::BufferedLog::Printf( " Pass Exchange Account check, CANUSE:[%lf]  LMT:[%lf]", ExInfo->GetCanUse(), ExAccountLimit );
                                    
                                if( Level > 1 )///< Need Check Department account
                                {                            
                                    if( DepInfo->CanPlaceOrder( CommodMargins * Qty, DepAccountLimit ) ) ///< Pass Department Account check
                                    {    
                                        UFC::BufferedLog::Printf( " Pass Department Account check ,CANUSE:[%lf]  LMT:[%lf]", DepInfo->GetCanUse(), DepAccountLimit );
                                        PassCheck = TRUE;
                                        SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'F', OrderTime.c_str(), 'N', NID, Key.c_str() );
                                        Data->append( "CODE", 1 );
                                        FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                                    }
                                    else
                                    {
                                        PassCheck = FALSE;
                                        Info->CancelOrder( CommodMargins * Qty );                                
                                        ExInfo->CancelOrder( CommodMargins * Qty );
                                    }
                                }
                                else ///< Pass (Is a Department account )
                                {                                
                                    PassCheck = TRUE;
                                    SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'F', OrderTime.c_str(), 'N', NID, Key.c_str() );
                                    Data->append( "CODE", 1 );
                                    FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                                }
                            }
                            else
                            {
                                PassCheck = FALSE;
                                Info->CancelOrder( CommodMargins * Qty );                                
                            }
                        }
                        else ///< Pass (Is an Exchange account )
                        {                                
                            PassCheck = TRUE;
                            SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'F', OrderTime.c_str(), 'N', NID, Key.c_str() );
                            Data->append( "CODE", 1 );
                            FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                        }
                    }           
                    else 
                        PassCheck = FALSE;
                    if( PassCheck == FALSE )
                    {
                        if( FToR6Checker )
                        {    
                            UFC::BufferedLog::Printf( " CANUSE:[%lf]  LMT:[%lf] ==> R6", Info->GetCanUse(), AccountLimit );
                            SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'F', OrderTime.c_str(), 'Y', NID, Key.c_str() );
                        }
                        else
                        {
                            Data->append( "CODE", 0 );
                            Data->append( "MSG", "Speedy Risk Manager reject this Order." );
                            FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
                            UFC::BufferedLog::DebugPrintf( " Speedy RM Reject Order:[%s]", Order.c_str() );
                            return;
                        }
                    }
                }
                
            }
        }
        else if( Subject == "SinoPacRMConnection.OPT" ) ///< Check Options Order
        {
            SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'O', OrderTime.c_str(), ( FWaitForR6 == TRUE )?'Y':'N', NID, Key.c_str() );
            UFC::BufferedLog::DebugPrintf( " Msg:[%s]", SendMsg.c_str() );            
            
            if( FSpeedyRM == FALSE )///< Disable Speedy RM 
            {
                SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'O', OrderTime.c_str(), ( FWaitForR6 == TRUE )?'Y':'N', NID, Key.c_str() );
                if( FWaitForR6 == FALSE )
                {
                    Data->append( "CODE", 1 );
                    FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                }
            }
            else
            {
                UFC::AnsiString BrokerID( Order.c_str() + 14, 7 );
                UFC::AnsiString Account( Order.c_str() + 26, 7 );
                UFC::AnsiString SymbolPrefix( Order.c_str() + 34, 3 );
                UFC::AnsiString SubAccount = GetSubAccount( Key );
                UFC::AnsiString SearchKey;
                double          AccountLimit  = 0.0;
                double          ExAccountLimit  = 0.0;
                double          DepAccountLimit  = 0.0;
                double          CommodMargins = GetCommodOTAMT( SymbolPrefix );                                    
                int             Level;                    
                AccountInfo*    Info;
                AccountInfo*    ExInfo;
                AccountInfo*    DepInfo;
                bool            GetInfoFailed = FALSE;
                
                UFC::BufferedLog::Printf( " BrokerID:[%s], Account:[%s][%s] SymbolPrefix:[%s] OpenOffset:[%c]", BrokerID.c_str(), Account.c_str(), SubAccount.c_str(), SymbolPrefix.c_str(), OpenOffset );
                Level = CheckSubAccount( BrokerID, Account, SubAccount );                                
                SearchKey.Printf( "%s%s%s", BrokerID.c_str(), Account.c_str(), SubAccount.c_str() );
                Info = AccountInfoTable.GetObjectByKey( SearchKey );
                
                if( Info == NULL || GetAccountLimit( BrokerID, Account, SubAccount, AccountLimit ) == FALSE )
                {
                    UFC::BufferedLog::Printf( " Can't fetch the Account: [%s]", SearchKey.c_str() );                
                    GetInfoFailed = TRUE;            
                }
                
                if( Level > 1 )///< Need Check Department account
                {                    
                   UFC::AnsiString DepAccount( SubAccount.c_str(), 3 );
                   
                   DepAccount.PadThis( 7,' ');
                   SearchKey.Printf( "%s%s%s", BrokerID.c_str(), Account.c_str(),DepAccount.c_str() );                   
                   DepInfo = AccountInfoTable.GetObjectByKey( SearchKey );
                    
                   if( DepInfo == NULL || GetAccountLimit( BrokerID, Account, DepAccount, DepAccountLimit ) == FALSE  )
                   {
                       UFC::BufferedLog::Printf( " Can't fetch the Department Account: [%s]", SearchKey.c_str() );                                   
                       GetInfoFailed = TRUE;                                                     
                   }
                }                               
                if( Level > 0 )///< Need Check Exchange account
                {
                   UFC::AnsiString ExAccount( "       " );
                   
                   ExAccount.PadThis( 7,' ');
                   SearchKey.Printf( "%s%s%s", BrokerID.c_str(), Account.c_str(), ExAccount.c_str() );                   
                   ExInfo = AccountInfoTable.GetObjectByKey( SearchKey ); 
                   
                   if( ExInfo == NULL || GetAccountLimit( BrokerID, Account, ExAccount, ExAccountLimit ) == FALSE  )
                   {
                       UFC::BufferedLog::Printf( " Can't fetch the Exchange Account: [%s]", SearchKey.c_str() );                                   
                       GetInfoFailed = TRUE;                                                     
                   }
                }                
                if( GetInfoFailed == TRUE || CommodMargins < 0  || OpenOffset == ' '  ) ///< Send to R6, always wait for R6 checking result.
                {                    
                    UFC::BufferedLog::Printf( " OpenOffset:[%c]", OpenOffset );
                    UFC::BufferedLog::Printf( " AccountLimit:[%lf]", AccountLimit );
                    UFC::BufferedLog::Printf( " CommodOTAMT:[%lf]", CommodMargins );
                    SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'O', OrderTime.c_str(), 'Y', NID, Key.c_str() );
                }
                else
                {
                    bool PassCheck;
                    
                    if( Info->CanPlaceOrder( CommodMargins * Qty, AccountLimit ) ) ///< Pass Exchange Account check
                    {
                        switch( Level )
                        {
                                case 0: UFC::BufferedLog::Printf( " Pass Exchange Account check, CANUSE:[%lf]  LMT:[%lf]", Info->GetCanUse(), AccountLimit );break;
                                case 1: UFC::BufferedLog::Printf( " Pass Department Account check, CANUSE:[%lf]  LMT:[%lf]", Info->GetCanUse(), AccountLimit ); break;
                                case 2: UFC::BufferedLog::Printf( " Pass Trader Account check, CANUSE:[%lf]  LMT:[%lf]", Info->GetCanUse(), AccountLimit );break;
                        }
			//UFC::BufferedLog::Printf( " Pass Trader Account check, CANUSE:[%lf]  LMT:[%lf]", Info->GetCanUse(), AccountLimit );
                        if( Level > 0 )///< Need Check Department account
                        {                            
                            if( ExInfo->CanPlaceOrder( CommodMargins * Qty, ExAccountLimit ) ) ///< Pass Department Account check
                            {    
                                UFC::BufferedLog::Printf( " Pass Exchange Account check ,CANUSE:[%lf]  LMT:[%lf]", ExInfo->GetCanUse(), ExAccountLimit );
                                if( Level > 1 )///< Need Check Trader account
                                {                            
                                    if( DepInfo->CanPlaceOrder( CommodMargins * Qty, DepAccountLimit ) ) ///< Pass Trader Account check
                                    {    
                                        UFC::BufferedLog::Printf( " Pass Department Account check, CANUSE:[%lf]  LMT:[%lf]", DepInfo->GetCanUse(), DepAccountLimit );
                                        PassCheck = TRUE;
                                        SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'O', OrderTime.c_str(), 'N', NID, Key.c_str() );
                                        Data->append( "CODE", 1 );
                                        FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                                    }
                                    else
                                    {
                                        PassCheck = FALSE;
                                        Info->CancelOrder( CommodMargins * Qty );                                
                                        ExInfo->CancelOrder( CommodMargins * Qty );
                                    }
                                }
                                else ///< Pass (Is a Department account )
                                {                                
                                    PassCheck = TRUE;
                                    SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'O', OrderTime.c_str(), 'N', NID, Key.c_str() );
                                    Data->append( "CODE", 1 );
                                    FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                                }
                            }
                            else
                            {
                                PassCheck = FALSE;
                                Info->CancelOrder( CommodMargins * Qty );                                
                            }
                        }
                        else ///< Pass (Is an Exchange account )
                        {                                
                            PassCheck = TRUE;
                            SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'O', OrderTime.c_str(), 'N', NID, Key.c_str() );
                            Data->append( "CODE", 1 );
                            FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                        }
                    }           
                    else 
                        PassCheck = FALSE;
                    if( PassCheck == FALSE )
                    {                    
                        if( FToR6Checker )
                        {
                            UFC::BufferedLog::Printf( " CANUSE:[%lf]  LMT:[%lf] ==> R6", Info->GetCanUse(), AccountLimit );
                            SendMsg.Printf( "%s%c%s%c%010d%s", Order.c_str(), 'O', OrderTime.c_str(), 'Y', NID, Key.c_str() );
                        }
                        else
                        {
                            Data->append( "CODE", 0 );
                            Data->append( "MSG", "Speedy Risk Manager reject this Order." );
                            FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
                            UFC::BufferedLog::DebugPrintf( " Speedy RM Reject Order:[%s]", Order.c_str() );
                            return;
                        }
                    }
                }
            }
        }
        
        TibrvStatus status;        
        
        UFC::BufferedLog::DebugPrintf( " Msg:[%s]", SendMsg.c_str() );
        FDataMsg.updateString( "BODY", SendMsg.c_str() );
        FMsg.updateMsg( "^data^", FDataMsg );
        FMsg.setSendSubject( FR6Subject.c_str() );
        status = FRVTransport.send( FMsg );
        if (status != TIBRV_OK)               
            UFC::BufferedLog::Printf( " Error sending message, status=%d, text=%s\n",(int)status,status.getText());    
        UFC::BufferedLog::FlushToFile();
    }
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::Execute( void )
{      
    TibrvStatus status;            
    
    while( !IsTerminated() )
    {        
        status=Tibrv::defaultQueue()->dispatch();
        if( status != TIBRV_OK )
        {
            UFC::BufferedLog::Printf( " Error: dispatch failed, status=%d, text=%s\n", (int)status,status.getText());
            Tibrv::close();
            exit(-1);
        }
    }
}
//------------------------------------------------------------------------------
BOOL SinoPacRMConnection::GetAccountLimit( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, const UFC::AnsiString& SubAccount, double& Price )
{
    UFC::PLockObject Lock( FAccountLimitCS );    
    UFC::AnsiString Value,TotalAccount;
    
    TotalAccount.Printf("^%s%s^", Account.c_str(), SubAccount.c_str());
    if( AccountLimitTable->GetValue( BrokerID, TotalAccount, Value ) )
    {
        Price = Value.ToDouble();
        return TRUE;
    }
    else
        return FALSE;
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::SetAccountLimit( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, const UFC::AnsiString& SubAccount, double Limit )
{
    UFC::PLockObject Lock( FAccountLimitCS );    
    UFC::AnsiString Value,TotalAccount;
    
    TotalAccount.Printf("^%s%s^", Account.c_str(), SubAccount.c_str());
    Value.Printf( "%014.2lf", Limit );
    AccountLimitTable->SetValue( BrokerID, TotalAccount, Value );
    AccountLimitTable->Save();
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::DeleteAccountLimit( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, const UFC::AnsiString& SubAccount )
{
    UFC::PLockObject Lock( FAccountLimitCS );
    UFC::AnsiString  TotalAccount;
    
    UFC::Section* TFXSection = AccountLimitTable->GetSection( BrokerID );
    if( TFXSection != NULL )
    {
        TotalAccount.Printf("^%s%s^", Account.c_str(), SubAccount.c_str());
        if( TFXSection->ValueExists( TotalAccount ) )
            TFXSection->DeleteValue( TotalAccount );
        if( TFXSection->ItemCount() == 0 )
            AccountLimitTable->DeleteSection( BrokerID );        
        AccountLimitTable->Save();
    }
}
//-------------------------------------------------------------------------------
AccountInfo* SinoPacRMConnection::GetAccountInfo( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Account, const UFC::AnsiString& SubAccount )
{
    UFC::AnsiString Key;
    Key.Printf( "%s%s", BrokerID.c_str(), Account.c_str(), SubAccount.c_str()  );
    return AccountInfoTable.GetObjectByKey( Key );
}
//-------------------------------------------------------------------------------    
void SinoPacRMConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );    
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk )
    {        
        FIsLogon = TRUE;
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime );
        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, rmAll )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::Recover( const UFC::AnsiString& BeginTime )
{
    if( FIsLogon == TRUE )
    {
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );    
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void SinoPacRMConnection::OnRecoverFinished( int Count )
{
    FIsLogon = TRUE;
    UFC::BufferedLog::DebugPrintf( " SinoPacExecConnection::OnRecoverFinished( Count = %d )", Count );    
    UFC::SleepMS( 1000 );    
}    
//------------------------------------------------------------------------------
void SinoPacRMConnection::CheckConnection( void )
{
    if( FIsLogon == FALSE )
        FConnection->Connect( FGatewayIP.c_str() , FGatewayPort, 10 );    
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup  )
{
    switch( Msg->GetOrderStatus() )
    {       
        case nsOrderMessageDefine::osCanceled:            
        case nsOrderMessageDefine::osReplaced:
            if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced || Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
            {
                UFC::BufferedLog::DebugPrintf( " Replace px. Drop this execution report." );
                return;
            }
             OnCancelReplace( Msg );
             break;
           
        default: break;
    }      
}
//------------------------------------------------------------------------------
void SinoPacRMConnection::OnCancelReplace( TExecutionReportMessage* Msg )
{    
    UFC::AnsiString ExecID( Msg->GetExecID() );
    UFC::AnsiString BrokerID( Msg->GetBrokerID() );
    UFC::AnsiString Account( Msg->GetAccount() );
    UFC::AnsiString Symbol( Msg->GetSymbol() );
	UFC::AnsiString SubAccount("");
	UFC::AnsiString UserData( Msg->GetData() );
	if( UserData.Length() > 18 )
	{
		SubAccount = UserData.SubString( 12, 7 );
	}
	else
	{
		SubAccount.PadThis( 7, ' ' );
	}
    UFC::AnsiString Key;
    double CommodMargins = GetCommodOTAMT( Symbol.SubString( 0,3 ) ); 
    
    SubAccount.PadThis( 7, ' ' );
	UFC::BufferedLog::Printf( " SubAccount:[%s]", SubAccount.c_str() ); 
	nsOrderMessageDefine::ExecTypeEnum ExecType = Msg->GetExecType();
    if( ExecType == nsOrderMessageDefine::etCanceled || ( ExecType == nsOrderMessageDefine::etReplaced && Msg->GetTMPExecType() == nsOrderMessageDefine::tetReplaced ) && CommodMargins > 0 )
    {
        if( !FExecIDTable.Exists( ExecID ) )
        {
            int Level;
            FExecIDTable.Add( ExecID );
            UFC::MemoryStream Buffer( EXECID_LEN, EXECID_LEN );
            memset( (void*)Buffer.GetBuffer(), '\0', EXECID_LEN );
            ExecID.SaveToStream( &Buffer );            
            FExecIDFile->Write( Buffer.GetBuffer(), EXECID_LEN );
            FExecIDFile->Flush();
            
            Level = CheckSubAccount( BrokerID, Account, SubAccount );
            
            switch( Level )
            {
                case 0: UFC::BufferedLog::Printf( " ---- Exchange account level ---- " );break;
                case 1: UFC::BufferedLog::Printf( " --- Department account level --- " );break;
                case 2: UFC::BufferedLog::Printf( " ----- Trader account level ----- " );break;
            }
            
            Key.Printf( "%s%s%s", BrokerID.c_str(), Account.c_str(), SubAccount.c_str() );
            AccountInfo* Info = AccountInfoTable.GetObjectByKey( Key );
            
            if( Info )
            {                
                Int32 Qty = Msg->GetBeforeQty() - Msg->GetAfterQty();
                Info->CancelOrder( CommodMargins * Qty );
                UFC::BufferedLog::Printf( " Cancel/Replace Qty:[%d]", Qty );
                UFC::BufferedLog::Printf( " BrokerID:[%s], Account:[%s][%s] SymbolPrefix:[%s] ", BrokerID.c_str(), Account.c_str(), SubAccount.c_str(), Symbol.c_str() );
                UFC::BufferedLog::Printf( " CANUSE:[%lf] ", Info->GetCanUse() );
            
                if( Level >= 2 )///< Department account
                {
                    UFC::AnsiString DepAccount( SubAccount.c_str(), 3 );   
                    DepAccount.PadThis( 7, ' ' );
                    UFC::BufferedLog::Printf( " ---- Department account level ---- " );
                    Key.Printf( "%s%s%s", BrokerID.c_str(), Account.c_str(),DepAccount.c_str() );
                    AccountInfo* DepInfo = AccountInfoTable.GetObjectByKey( Key );
                    
                    if( DepInfo )
                    {                
                        Int32 Qty = Msg->GetBeforeQty() - Msg->GetAfterQty();
                        DepInfo->CancelOrder( CommodMargins * Qty );
                        UFC::BufferedLog::Printf( " Cancel/Replace Qty:[%d]", Qty );
                        UFC::BufferedLog::Printf( " BrokerID:[%s], Account:[%s][%s] SymbolPrefix:[%s] ", BrokerID.c_str(), Account.c_str(), SubAccount.c_str(), Symbol.c_str() );
                        UFC::BufferedLog::Printf( " CANUSE:[%lf] ", DepInfo->GetCanUse() );
                    }
                    else
                        UFC::BufferedLog::Printf( " Department account not exist" );
                }
                            
                if( Level >= 1)///< Exchange account
                {
                    UFC::BufferedLog::Printf( " --- Exchange account level --- " );
                    Key.Printf( "%s%s       ", BrokerID.c_str(), Account.c_str() );
                    AccountInfo* ExInfo = AccountInfoTable.GetObjectByKey( Key );
                    
                    if( ExInfo )
                    {                
                        Int32 Qty = Msg->GetBeforeQty() - Msg->GetAfterQty();
                        ExInfo->CancelOrder( CommodMargins * Qty );
                        UFC::BufferedLog::Printf( " Cancel/Replace Qty:[%d]", Qty );
                        UFC::BufferedLog::Printf( " BrokerID:[%s], Account:[%s][%s] SymbolPrefix:[%s] ", BrokerID.c_str(), Account.c_str(), SubAccount.c_str(), Symbol.c_str() );
                        UFC::BufferedLog::Printf( " CANUSE:[%lf] ", ExInfo->GetCanUse() );
                    }
                    else
                        UFC::BufferedLog::Printf( " Exchange account not exist" );
                }
            }
        }
    }
}
//------------------------------------------------------------------------------

