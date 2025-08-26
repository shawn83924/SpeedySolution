#include "KGIExecConnection.h"
#include "../../SpeedyAPI/UFC/NetUtility.h"
//------------------------------------------------------------------------------
extern BOOL            DumpOnly;
extern MessageObject*  MessageObj;
//------------------------------------------------------------------------------
KGIExecConnection::KGIExecConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName( "KGIExecConnection" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "Admin" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F021999" )
,FWEBID( "000" )
,FDefaultWEBID( "000" )
,FRecoverBeginTime( RecoverBeginTime )
,FConnection( NULL )
,FIsLogon( FALSE )
{      
    UFC::AnsiString LogName = "../log/KGIExec" + UFC::GetDateString() + ".log";
    LoadSetting( );
    LoadFilterCust();
//    LoadMappingAE(  );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( " Dump to screen only.");
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   KGI backend interface module           " );
    UFC::BufferedLog::Printf( "   startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s ",__DATE__ );
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On (Dump to stdout only)");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");    
    UFC::BufferedLog::Printf( "__________________________________________" );       
    ///< Create the connection object.
//    FConnection = new TTaifexConnection( NULL, &FResource, FAppName, this, TRUE );
    FConnection = new TTaifexConnection( NULL, &FResource, FAppName, this );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FBrokerID.SubString(0,4).c_str() );
    ///< Load TAIFEX message format config files.
    FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
    FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
    FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
    FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );
    ///< Set log file name.    
    FConnection->SetLogFile( LogName );
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
void KGIExecConnection::LoadSetting( void )
{
    try
    {        
        UFC::UiniFile   Config( "../cfg/KGIExecConnection.cfg" );
        if( Config.GetValue( "Setting", "IP", FIP ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Speedy Server IP not set.");
            exit(1);
        }
        if( Config.GetValue( "Setting", "ID",FID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon ID not set.");
            exit(1);
        }    
        if( Config.GetValue( "Setting", "Password", FPassword ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon Password not set.");
            exit(1);
        }    
        if( Config.GetValue( "Setting", "Account", FAccount ) == FALSE )
        {
            UFC::BufferedLog::Printf(" Logon Account not set.");
            exit(1);
        }    
        if( Config.GetValue( "Setting", "BrokerID", FBrokerID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" BrokerID not set.");
            exit(1);
        }
        if( Config.GetValue( "Setting", "WebID", FDefaultWEBID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" DefaultWebID not set.");
            exit(1);
        }
        FDefaultWEBID.PadThis( 3, ' ' );
        if( Config.GetValue( "Setting", "WebID", FWEBID ) == FALSE )
        {
            UFC::BufferedLog::Printf(" WebID not set.");
            exit(1);
        }
        FWEBID.PadThis( 3, ' ' );
        if( Config.GetValue( "Setting", "PTDesk", FPTDesk ) == FALSE )
        {
            UFC::BufferedLog::Printf(" PTDesk not set. Default use [N].");
            FPTDesk = "N";
        }
        if( Config.GetValue( "Setting", "Key", FKey ) == FALSE )
        {
        	UFC::BufferedLog::Printf(" Key not set. Default use [To400].");
        	FKey = "To400";
        }
        if( Config.GetValue( "Setting", "Scan", FScan ) == FALSE )
        {
        	UFC::BufferedLog::Printf(" Scan not set. Default use [N].");
        	FScan = "N";
        }
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" KGIExecConnection.cfg not found.");
        exit(1);
    }

    if( UFC::FileExists( "../cfg/AE400.cfg" ) )
    {
        UFC::UiniFile AEConfig( "../cfg/AE400.cfg" );
        UFC::Section* pSec = AEConfig.GetSection( "AE" );
        if( pSec )
        {
            for( register int i = 0; i < pSec->ItemCount(); i++ )
            {
                UFC::AnsiString Name, Value;

                pSec->GetNameValue( i, Name, Value );

                UFC::AnsiString AE, WebID;
                AE = Value.SubString( 0, Value.AnsiPos( ',' ) );
                WebID = Value.SubString( Value.AnsiPos( ',' ) + 1, Value.Length() - Value.AnsiPos( ',' ) );
                UFC::BufferedLog::Printf( " AE:[%s] WebID:[%s]", AE.c_str(), WebID.c_str() );
                AE.PadThis( 10, ' ' );
                WebID.PadThis( 2, ' ' );
                UFC::BufferedLog::Printf( " AE:[%s] WebID:[%s]", AE.c_str(), WebID.c_str() );
                FAETable.Add( Name, new AEInfo( AE, WebID ) );
            }
        }
    }
    if ( FScan == "Y" )
    {
    	if ( UFC::FileExists( "../cfg/SpeedyGatewayUsers.cfg") )
    	{
    		UFC::UiniFile GUConfig("../cfg/SpeedyGatewayUsers.cfg");
            if( GUConfig.GetValue( "Speedy", "Terminals", FTerminals ) == FALSE )
            {
                UFC::BufferedLog::Printf(" Terminals not set.");
                exit(1);
            }
            else
            {
            	UFC::AnsiString term;
            	for (int i=0; i<FTerminals.Length(); i+=3)
            	{
            		term = FTerminals.SubString(i, 2);
            		setTerminal.insert(term);
            	}
            }
    	}
    	else
    	{
    		UFC::BufferedLog::Printf( "../cfg/SpeedyGatewayUsers.cfg not found." );
    		exit(1);
    	}
    }
}
//------------------------------------------------------------------------------
void KGIExecConnection::LoadMappingAE( void )
{
	try
	{
		ifstream fin("../cfg/KGIExecMappingAe.cfg", ios::in);
		if (!fin.is_open())	{
			UFC::BufferedLog::Printf("../cfg/KGIExecMappingAe.cfg");
			return;
		}
		fin.unsetf(ios::skipws);
		string sLine;
		unsigned short pos;
		string sOrderWay;
		string sAE;

		while (getline(fin, sLine))	{
            pos = sLine.find('=',0);
            sOrderWay = sLine.substr(0, pos);
            sAE = sLine.substr(pos+1, sLine.length()-pos-1);
			map<string, string>::iterator it = FAEMap.find(sOrderWay);
		    if (it != FAEMap.end())	{
		    	if ( sAE != it->second)	{
		    		FAEMap.erase(sOrderWay);
		    		FAEMap.insert(make_pair(sOrderWay, sAE));
		    	}
		    }
		    else	{
		    	FAEMap.insert(make_pair(sOrderWay, sAE));
		    }
		}
		fin.close();
	}
	catch(...)
	{
		UFC::BufferedLog::Printf(" KGIExecMappingAE.cfg not found.");
	}
}
//------------------------------------------------------------------------------
void KGIExecConnection::LoadFilterCust( void )
{
	try
	{
		ifstream fin("../cfg/KGIExecFilter.cfg", ios::in);
		if (!fin.is_open())	{
			UFC::BufferedLog::Printf("../cfg/KGIExecFilter.cfg");
			return;
		}
		fin.unsetf(ios::skipws);
		string sLine;
		unsigned short pos;
		string sAE,sAccount;

		while (getline(fin, sLine))	{
            pos = sLine.find(',',0);
            sAE = sLine.substr(0, pos);
            sAccount = sLine.substr(pos+1, sLine.length()-pos-1);
			map<string, string>::iterator it = FFilterCustMap.find(sAE);
		    if (it != FFilterCustMap.end())	{
		    	if ( sAccount != it->second)	{
		    		FFilterCustMap.erase(sAE);
		    		FFilterCustMap.insert(make_pair(sAE, sAccount));
		    	}
		    }
		    else	{
		    	FFilterCustMap.insert(make_pair(sAE, sAccount));
		    }
		}
		fin.close();
	}
	catch(...)
	{
		UFC::BufferedLog::Printf(" KGIExecFilter.cfg not found.");
	}
}
//------------------------------------------------------------------------------
KGIExecConnection::~KGIExecConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
void KGIExecConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str(),  ctRecvExecution, 20083 );
    FIsLogon = TRUE;
}
//------------------------------------------------------------------------------
void KGIExecConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void KGIExecConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( lrOk )
    {
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime );
        
        UFC::BufferedLog::DebugPrintf( " FConnection->Recover( %s, rdBoth, rmAll )", BeginTime.c_str() );
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll  );
    }
    else
    {
        UFC::BufferedLog::Printf( " %s....logon failed", ReplyString );
        exit(0);
    }
}
//------------------------------------------------------------------------------
void KGIExecConnection::Recover( UFC::AnsiString BeginTime )
{
    if( FIsLogon == TRUE )
    {
        FConnection->Recover( BeginTime.c_str(), rdFill, rmAll );
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void KGIExecConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " KGIExecConnection::OnRecoverFinished( Count = %d )", Count );
    
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
void KGIExecConnection::ConfirmToBackend( TExecutionReportMessage* Msg )
{
	if ( FScan == "Y" )	{
		UFC::AnsiString Terminal( Msg->GetSrc() );
		Terminal = Terminal.SubString(21, 2);
		strSet::iterator pos;
		if ( (pos = setTerminal.find(Terminal)) == setTerminal.end())	{
			return;
		}
	}
	FWEBID = FDefaultWEBID;
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString GIGO( Msg->GetData() );
    UFC::AnsiString Trader;
    UFC::AnsiString TFX, Backend, SSNID;
    UFC::AnsiString OrderType( Msg->GetSrc() );
    UFC::AnsiString Account( Msg->GetAccount() );

    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;

    char QueueData[ 513 ];
    ///< Order Type
    OrderType = OrderType.SubString(5, 1);
    if ( OrderType == "9" )	{
    	UFC::AnsiString OrderID( Msg->GetSrc() );
    	OrderID = OrderID.SubString(21, 5);

    	map <UFC::AnsiString, UFC::AnsiString>::iterator it = FQuoteData.find(OrderID);
    	if (it != FQuoteData.end())	{		//found previous confirm,take and erase
    		UFC::AnsiString BidPrice( it->second.SubString(0, 9) );
    		UFC::AnsiString BidQty( it->second.SubString(9, 4) );
    		FQuoteData.erase(OrderID);
    	    ///< if the 2nd quote confirm then replace the bid price and bid qty
    		///TAIFEX part
        	UFC::AnsiString Header( Msg->GetSrc()+2 );
        	Header = Header.SubString(0, 52);
        	UFC::AnsiString AskPrice ( Msg->GetSrc() );
        	AskPrice = AskPrice.SubString(63, 9);
        	UFC::AnsiString AskQty ( Msg->GetSrc() );
        	AskQty = AskQty.SubString(76, 4);
        	UFC::AnsiString Trailer( Msg->GetSrc()+80);
        	if (Market == nsOrderMessageDefine::mTWFutures)
        		TFX.Printf( "%s30%s%s%s%s%s%s", "        ", Header.c_str(), BidPrice.c_str(), AskPrice.c_str(), BidQty.c_str(), AskQty.c_str(), Trailer.c_str() );
        	else
        		TFX.Printf( "%s93%s%s%s%s%s%s", "        ", Header.c_str(), BidPrice.c_str(), AskPrice.c_str(), BidQty.c_str(), AskQty.c_str(), Trailer.c_str() );
    	}
    	else	{							//no found, keep first confirm and return
        	UFC::AnsiString BidPrice( Msg->GetSrc()), BidQty( Msg->GetSrc());
        	BidPrice = BidPrice.SubString(54, 9);
        	BidQty = BidQty.SubString(72, 4);
        	FQuoteData.insert(make_pair(OrderID, BidPrice + BidQty));
        	return;
    	}
    }
    else	{
		///< TAIFEX part.
		if( Market == nsOrderMessageDefine::mTWFutures )
			TFX.Printf( "%s30%s", "        ", Msg->GetSrc()+2 );
		else
			TFX.Printf( "%s93%s", "        ", Msg->GetSrc()+2 );
    }
    ///< SSNID
    SSNID.Printf( "%05d", atoi( Msg->GetPVC() ) ) ;
    UFC::BufferedLog::Printf( " SSNID:[%s]", SSNID.c_str() );
    ///< Backend part.


    if ( AE == "xICE" || AE == "yICE" )		//艾揚MBus API
        AE = "      ";
    else if ( AE == "GETS" )	{			//艾揚MBus API
    	AE = "      ";
    	GIGO = "                       ";	// length = 23
    }
    else if ( (AE == "fip") || (AE == "tip"))	{				//智能平台
    	AE = "      ";
		//分帳營業員
		if (GIGO.Length() >= 28)	{
			if (GIGO.SubString(22,6) == "")
				Trader = "";
			else	{
				Trader = GIGO.SubString(22, 6);
				Trader.TrimLeft();
				Trader.TrimRight();
			}
			AE = Trader;
			AE.PadThis( 6, ' ' );
		}
		if (GIGO.Length() >= 21)	{
			FWEBID = GIGO.SubString(18, 3);	//下單管道
		}
//		map<string, string>::iterator it = FAEMap.find(AE.c_str());
//		if (it != FAEMap.end())	{			//為下單管道
//			AE = it->second.c_str();		//預設AE為空白
//		}
    }
    else
        GIGO = "                       ";  	// length = 23

    //20110803
    if ( FPTDesk == "Y" )	{
        AE = Msg->GetData();
        AE = AE.SubString(0, 6);
    }

    AE.PadThis( 6, ' ' );
    GIGO = GIGO.SubString(0, 17);
    GIGO.PadThis( 23, ' ' );
    UFC::BufferedLog::Printf( " AE:[%s]", AE.c_str() );
    UFC::BufferedLog::Printf( " WEBID:[%s]", FWEBID.c_str() );
    UFC::BufferedLog::Printf( " Account:[%s]", Msg->GetAccount());
    UFC::BufferedLog::Printf( " TimeInForce:[%d]", Msg->GetTimeInForce());
   	Backend.Printf( "%s%011d %04d%s%s",FWEBID.c_str(),0,0,GIGO.c_str(),AE.c_str());

    ///< Compose to a full As400 report.
    memset( QueueData, ' ', 512 );
    QueueData[512] = 0;
    memcpy( QueueData, TFX.c_str(), TFX.Length());
    memcpy( QueueData + 424, SSNID.c_str(), 5 );
    memcpy( QueueData + 462, Backend.c_str(), Backend.Length());        
    Data.append("Data", QueueData );
    UFC::BufferedLog::Printf( " Px=%5.3f", Msg->GetPrice() );
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
        	UFC::BufferedLog::Printf( " <FUT CONFIRM> <%s>", QueueData );
            if( DumpOnly == FALSE )	{
            	if ( IsFilterCust((string)AE.c_str(), (string)Account.c_str()) )	{
            		if ( (Msg->GetTimeInForce() == 2) || (Msg->GetTimeInForce() == 3) )
            			MessageObj->Send( conCONFIRM1.c_str(), FKey.c_str(), Data );
            		else if ( (Msg->GetTimeInForce() == 1) )
            			MessageObj->Send( conCONFIRM.c_str(), FKey.c_str(), Data );
            	}
            	else
            		MessageObj->Send( conCONFIRM.c_str(), FKey.c_str(), Data );
            }
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " <OPT CONFIRM> <%s>", QueueData );                                                            
            if( DumpOnly == FALSE )	{
            	if ( IsFilterCust((string)AE.c_str(), (string)Account.c_str()) )	{
            		if ( (Msg->GetTimeInForce() == 2) || (Msg->GetTimeInForce() == 3) )
            			MessageObj->Send( conCONFIRM1.c_str(), FKey.c_str(), Data );
            		else if ( (Msg->GetTimeInForce() == 1) )
            			MessageObj->Send( conCONFIRM.c_str(), FKey.c_str(), Data );
            	}
            	else
            		MessageObj->Send( conCONFIRM.c_str(), FKey.c_str(), Data );
            }
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void KGIExecConnection::FillToBackend( TExecutionReportMessage* Msg )
{
	if ( FScan == "Y" )	{
		UFC::AnsiString Terminal( Msg->GetSrc() );

	    if( Terminal[0] == '1' )						//單式
	        Terminal = Terminal.SubString( 8, 2 );
	    else if( Terminal[0] == '2' )					//複式
	        Terminal = Terminal.SubString( 8 , 2 );
	    else
	        Terminal = Terminal.SubString( 10, 2 );		//委託, 報價

		strSet::iterator pos;
		if ( (pos = setTerminal.find(Terminal)) == setTerminal.end())	{
			return;
		}
	}
	FWEBID = FDefaultWEBID;
    UFC::AnsiString Src( Msg->GetSrc() );
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString GIGO( Msg->GetData() );
    UFC::AnsiString Trader;
    UFC::AnsiString TFX,Backend,TimeNow, MatchTime, SSNID;
    UFC::AnsiString Account( Msg->GetAccount() );
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;
    char QueueData[ 513 ];     

    if( Src[0] == '1' )							//單式
        MatchTime = Src.SubString( 61, 8 );
    else if( Src[0] == '2' )					//複式
        MatchTime = Src.SubString( 109 , 8 );
    else
        MatchTime = Src.SubString( 57, 8 );		//委託, 報價

    UFC::BufferedLog::DebugPrintf( " MatchTime:[%s]", MatchTime.c_str() );
    UFC::BufferedLog::DebugPrintf( " BrokerID:[%s]", Msg->GetBrokerID() );
    ///< TAIFEX part.
    //UFC::GetHHMMSS( TimeNow );
    if( Market == nsOrderMessageDefine::mTWFutures )
        TFX.Printf( "        500002%s00%s01%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetBrokerID(), Msg->GetSrc() );
    else
        TFX.Printf( "        950002%s00%s01%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetBrokerID(), Msg->GetSrc() );
    ///< Backend part.1

    //艾揚MBus API
    if ( AE == "xICE" || AE == "yICE" )
        AE = "      ";
    else if ( AE == "GETS" )	{
    	AE = "      ";
    	GIGO = "                       ";  // length = 23
    }
    else if ( (AE == "fip") || (AE == "tip"))	{
    	AE = "      ";
		//分帳營業員
		if (GIGO.Length() >= 28)	{
			if (GIGO.SubString(22,6) == "")
				Trader = "";
			else	{
				Trader = GIGO.SubString(22,6);
				Trader.TrimLeft();
				Trader.TrimRight();
			}
			AE = Trader;
			AE.PadThis( 6, ' ' );
		}
		if (GIGO.Length() >= 21)	{
			FWEBID = GIGO.SubString(18, 3);	//下單管道
		}
//		map<string, string>::iterator it = FAEMap.find(AE.c_str());
//		if (it != FAEMap.end())	{			//為下單管道
//			AE = it->second.c_str();		//預設AE為空白
//		}
    }
    else
        GIGO = "                       ";  // length = 23
    
    //20110803
    if ( FPTDesk == "Y" )	{
        AE = Msg->GetData();
        AE = AE.SubString(0, 6);
    }

    AE.PadThis( 6, ' ' );
    GIGO = GIGO.SubString(0, 17);
    GIGO.PadThis( 23, ' ' );
    UFC::BufferedLog::Printf( " AE:[%s]", AE.c_str() );
    UFC::BufferedLog::Printf( " WEBID:[%s]", FWEBID.c_str() );
    Backend.Printf( "%s%011d %04d%s%s",FWEBID.c_str(),0,0,GIGO.c_str(), AE.c_str());

    char TIF;
    if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC )
        TIF = 'I';
    else if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifFOK )
        TIF = 'F';
    else
        TIF = 'R';

    SSNID.Printf( "%05d%c", atoi( Msg->GetPVC()), TIF );
    UFC::BufferedLog::Printf( " SSNID:[%s]", SSNID.c_str() );
    ///< Compose to a full As400 report.
    memset( QueueData, ' ', 512 );
    QueueData[512] = 0;
    memcpy( QueueData, TFX.c_str(), TFX.Length());
    memcpy( QueueData + 424, SSNID.c_str(), 6 );
    memcpy( QueueData + 462, Backend.c_str(), Backend.Length());        
    Data.append("Data", QueueData );
    
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::Printf( " <FUT Fill> <%s>", QueueData );                                                
            if( DumpOnly == FALSE )	{
            	if ( IsFilterCust((string)AE.c_str(), (string)Account.c_str()) )	{
            		if ( ((Msg->GetTimeInForce() == 2) || (Msg->GetTimeInForce() == 3)) && (Msg->GetCumQty() > 0) )
            			MessageObj->Send( conFILL1.c_str(), FKey.c_str(), Data );
            		else if ( (Msg->GetTimeInForce() == 1) )
            			MessageObj->Send( conFILL.c_str(), FKey.c_str(), Data );
            	}
            	else
            		MessageObj->Send( conFILL.c_str(), FKey.c_str(), Data );
            }
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " <OPT Fill> <%s>", QueueData );
            if( DumpOnly == FALSE )	{
            	if ( IsFilterCust((string)AE.c_str(), (string)Account.c_str()) )	{
            		if ( ((Msg->GetTimeInForce() == 2) || (Msg->GetTimeInForce() == 3)) && (Msg->GetCumQty() > 0) )
            			MessageObj->Send( conFILL1.c_str(), FKey.c_str(), Data );
            		else if ( (Msg->GetTimeInForce() == 1) )
            			MessageObj->Send( conFILL.c_str(), FKey.c_str(), Data );
            	}
            	else
            		MessageObj->Send( conFILL.c_str(), FKey.c_str(), Data );
            }
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void KGIExecConnection::TSEConfirmToBackend( TExecutionReportMessage* Msg )
{
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;
    UFC::AnsiString QueueData( Msg->GetSrc() );

    UFC::AnsiString Key;
    UFC::AnsiString AE = "          ";
    UFC::AnsiString WebID = "  ";
    AEInfo *info;
    QueueData.PadThis( 256, ' ' );

    Key.Printf( "%s%s", Msg->GetAccount(), Msg->GetAE() );
    if( (info = FAETable.GetObjectByKey( Key )) != NULL )
    {
        AE = info->AE;
        WebID = info->WebID;
        //AE.PadThis( 11, ' ' );
    }
    UFC::BufferedLog::DebugPrintf( " [%s]=>[%s]:[%s]", Key.c_str(), AE.c_str(), WebID.c_str() );

    //memcpy( (void*)(QueueData.c_str() + 150), AE.c_str(), 11 );
    memcpy( (void*)(QueueData.c_str() + 244), AE.c_str(), 10 );
    memcpy( (void*)(QueueData.c_str() + 254), WebID.c_str(), 2 );

    Data.append("Data", QueueData );

    switch( Market )
    {
        case nsOrderMessageDefine::mTSE:
            UFC::BufferedLog::Printf( " <TSE CONFIRM> <%s>", QueueData.c_str() );
            if( DumpOnly == FALSE )
                MessageObj->Send( "TSEConfirm", FKey.c_str(), Data );
            break;
        case nsOrderMessageDefine::mOTC:
            UFC::BufferedLog::Printf( " <OTC CONFIRM> <%s>", QueueData.c_str() );
            if( DumpOnly == FALSE )
                MessageObj->Send( "TSEConfirm", FKey.c_str(), Data );
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void KGIExecConnection::TSEFillToBackend( TExecutionReportMessage* Msg )
{
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;
    UFC::AnsiString QueueData;
    UFC::AnsiString MatchTime( Msg->GetTransactTime() );
    UFC::AnsiString Key;
    UFC::AnsiString AE = "          ";
    UFC::AnsiString WebID = "  ";
    AEInfo *info;

    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE )
        QueueData.Printf( "501000%06s00006201%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetSrc() );
    else
        QueueData.Printf( "951000%06s00006201%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetSrc() );
    QueueData.PadThis( 256, ' ' );

    Key.Printf( "%s%s", Msg->GetAccount(), Msg->GetAE() );
    if( (info = FAETable.GetObjectByKey( Key )) != NULL )
    {
        AE = info->AE;
        WebID = info->WebID;
        //AE.PadThis( 11, ' ' );
    }
    UFC::BufferedLog::DebugPrintf( " [%s]=>[%s]:[%s]", Key.c_str(), AE.c_str(), WebID.c_str() );

    //memcpy( (void*)(QueueData.c_str() + 150), AE.c_str(), 11 );
    memcpy( (void*)(QueueData.c_str() + 244), AE.c_str(), 10 );
    memcpy( (void*)(QueueData.c_str() + 254), WebID.c_str(), 2 );

    Data.append("Data", QueueData );



    switch( Market )
    {
        case nsOrderMessageDefine::mTSE:
            UFC::BufferedLog::Printf( " <TSE Fill> <%s>", QueueData.c_str() );
            if( DumpOnly == FALSE )
                MessageObj->Send( "TSEFill", FKey.c_str(), Data );
            break;
        case nsOrderMessageDefine::mOTC:
            UFC::BufferedLog::Printf( " <OTC Fill> <%s>", QueueData.c_str() );
            if( DumpOnly == FALSE )
                MessageObj->Send( "TSEFill", FKey.c_str(), Data );
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void KGIExecConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    
    if( Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mOTC )
    {
        switch( Msg->GetOrderStatus() )
        {
            case nsOrderMessageDefine::osNew:
            case nsOrderMessageDefine::osReplaced:
            case nsOrderMessageDefine::osCanceled:
                 TSEConfirmToBackend( Msg );
                 break;
            
            case nsOrderMessageDefine::osPartiallyFilled:
            case nsOrderMessageDefine::osFilled:
                TSEFillToBackend( Msg );
                break;
            case nsOrderMessageDefine::osPendingNew:
            case nsOrderMessageDefine::osPendingReplace:
            case nsOrderMessageDefine::osPendingCancel:
            case nsOrderMessageDefine::osRejected:
            default: break;
        }
    }
    else
    {
//    	UFC::AnsiString BrokerID( Msg->GetBrokerID() );
//        if ( (BrokerID == "F004000") || (BrokerID =="F004002") )
//        {
    		UFC::BufferedLog::Printf( " BrokerID [%s]", Msg->GetBrokerID() );
			switch( Msg->GetOrderStatus() )
			{
				case nsOrderMessageDefine::osNew:
				case nsOrderMessageDefine::osQuoteAccept:
					 ConfirmToBackend( Msg );
					 break;
				case nsOrderMessageDefine::osReplaced:
					if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
						Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
						ConfirmToBackend( Msg );
					else
						FillToBackend( Msg );
					break;
				case nsOrderMessageDefine::osCanceled:
					if ( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC && Msg->GetCumQty() > 0 && (Msg->GetLeavesQty() == 0) && (Msg->GetOrderQty() == 0))
						return;
					else
						FillToBackend( Msg );
					break;
				case nsOrderMessageDefine::osPartiallyFilled:
				case nsOrderMessageDefine::osFilled:
					 FillToBackend( Msg );
					 break;
				case nsOrderMessageDefine::osPendingNew:
				case nsOrderMessageDefine::osPendingReplace:
				case nsOrderMessageDefine::osPendingCancel:
				case nsOrderMessageDefine::osRejected:
				default: break;
			}
//        }
//        else
//        {
//        	UFC::BufferedLog::Printf( " BrokerID [%s] -> ignored", Msg->GetBrokerID() );
//        }
    }
}
//------------------------------------------------------------------------------
void KGIExecConnection::Execute( void )
{      
    while( !IsTerminated() )
    {
        if( FIsLogon == FALSE )
            FConnection->Connect( FIP.c_str() , FPort );    
        sleep( 10 ); ///< wait 10 sec.
    }
}
//------------------------------------------------------------------------------
std::string& KGIExecConnection::trim(std::string &s)	{
    if (s.empty()) {
        return s;
    }
    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}
//------------------------------------------------------------------------------
bool KGIExecConnection::IsFilterCust(string AE, string Account)
{
	map <string, string>::iterator it = FFilterCustMap.find(trim(AE));
	if (it != FFilterCustMap.end())	{
		if ( Account == it->second)	{
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------
