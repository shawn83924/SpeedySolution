#include "MLExecConnection.h"
#include "../../UFC/NetUtility.h"
#include "../../TAIFEXLib/TMPMessage.h"
#include "TUserData.h"
#include <math.h>

//------------------------------------------------------------------------------
extern UFC::PInt32 FRecoverBeginTime;
extern Int32 FOverseasSeqNo;
extern BOOL DumpOnly;
extern void makeCheckWord(UFC::AnsiString &result);
extern MLExecConnection* ExecConnection;
extern void GetReconnectBeginTime(UFC::AnsiString& DisconnectTime);
extern UFC::PEvent*    MBusConnectedEvent;
extern UFC::AnsiString ConfigName;
extern UFC::AnsiString AppName;
extern UFC::AnsiString LogName;
extern ConnectionType  FExecType;
extern ExecutionMarket FExecMarket;
UFC::AnsiString revicetime;
MessageObject*  MLExecConnection::FMessageObject = NULL;
int    NextDayTime = 55820;
//------------------------------------------------------------------------------
UFC::UiniFile* pConfig;
//---------------------------------------------------------------------------
const UFC::AnsiString PROXY_PREFIX( "<Proxy:" );
const int             PROXY_PREFIX_LEN = PROXY_PREFIX.Length(); ///< 7
const int             PROXY_NID_LEN      = 10;
const int             PROXY_MINUS_AE_LEN = PROXY_PREFIX_LEN + PROXY_NID_LEN;
//------------------------------------------------------------------------------
Int64 DoubleToInt64( double DoubleVal, int Digi)
{
    double precision = pow(0.1, (double) (Digi + 1));
    double multiple = pow(10.0, (double) Digi);

    if (DoubleVal >= 0.0)
        return (Int64) ((DoubleVal + precision) * multiple);
    else
        return (Int64) ((DoubleVal - precision) * multiple);
}
//------------------------------------------------------------------------------
void SendAlert(const UFC::AnsiString& Msg)
{
    MTree Data;
    Data.append("COMMAND", 9020);
    Data.append("MSG", Msg);

    if( MLExecConnection::FMessageObject != NULL )
        MLExecConnection::FMessageObject->Send("SPEEDY.COMMAND", UFC::Hostname, Data, FALSE);
}
//------------------------------------------------------------------------------
void ProcessMonitor::OnProcessConnected(BOOL IsTheFirstOne)
{
		
    if( IsTheFirstOne == FALSE )
    {
        UFC::BufferedLog::Printf(" ***** Process MLExecConnection already exists *****");        
        exit(0);
    }
    else
    {
    	MBusConnectedEvent->SetEvent();
    }
    MBusConnectedEvent->SetEvent();
}
//------------------------------------------------------------------------------
BOOL SocketMonitor::OnDataArrived(UFC::PClientSocket * Socket)
{
    return TRUE;
}
//------------------------------------------------------------------------------
void SocketMonitor::OnDisconnect(UFC::PClientSocket* Socket, BOOL NeedReconnect)
{
    ExecConnection->clientIsConnect = FALSE;
    UFC::AnsiString log;
    log.Printf(" MLExecConnection Connection broken [%s:%d]", Socket->GetPeerIPAddress().c_str(), Socket->GetPort()); //Connection
    UFC::BufferedLog::Printf(log.c_str());
    SendAlert(log.SubString(1,-1));
    GetReconnectBeginTime(ExecConnection->DisconnectTime1);
}
//------------------------------------------------------------------------------
void SocketMonitor::OnConnect(UFC::PClientSocket* Socket)
{
    ExecConnection->clientIsConnect = TRUE;
    UFC::AnsiString log;
    log.Printf(" Connection established [%s:%d]", Socket->GetPeerIPAddress().c_str(), Socket->GetPort()); //Connection
    UFC::BufferedLog::Printf(log.c_str());
    char Buffer[18];
    
    memset(Buffer,0,18);
    UFC::BufferedLog::DebugPrintf(" Waiting for handshake");
    Socket->BlockRecv((UInt8*) Buffer, 17);
    UFC::BufferedLog::DebugPrintf(" get [%s]",Buffer);
    UFC::AnsiString data,data2;
    revicetime.Copy( Buffer + 8, 6 );
    data2.Printf("0009ACK%s",revicetime.c_str());
    makeCheckWord(data2);
    data.Printf("\2%s\3",data2.c_str());
    UFC::BufferedLog::DebugPrintf(" send [%s]",data.c_str());
    Socket->BlockSend((Int8*) data.c_str(), data.Length());
    UFC::BufferedLog::DebugPrintf(" handshake finish");
    if( ExecConnection->DisconnectTime1.Length() == 6 )
        revicetime.SetLength(0);
}
//------------------------------------------------------------------------------
MLExecConnection::MLExecConnection( void ): UFC::PThread(NULL, FALSE)
,sn(0)
,client1(NULL)
,Client1Monitor(NULL)
,FIP("127.0.0.1")
,FPort(23456)
,FID("Admin")
,FPassword("1234rewq")
,FAccount("0000001")
,FBrokerID("F018000")
,FSocketTimeout(10)
,FExitOnDisconnect( true )
,FConnection(NULL)
,FIsLogon(FALSE)
,FFirstRecover(TRUE)
,FFilterSID(0)
,FFilterSeq(0)
,FFilterEndSeq(0)
,DisconnectTime1("")
,clientIsConnect(FALSE)
,HeartbeatTime1(0)
,HeartbeatTimeCount1(0)
{
    FRecoverTime = FRecoverBeginTime.ToInt32();
    LoadSetting();
    UFC::BufferedLog::Printf("__________________________________________");
    UFC::BufferedLog::Printf("                                          ");
    UFC::BufferedLog::Printf("   MasterLink backend interface module           ");
    UFC::BufferedLog::Printf("   Copyright (c)2011~%d MDBS",UFC::ThisYear());
    UFC::BufferedLog::Printf("   %d bit mode                 ", sizeof(void*)*8 );                        
    UFC::BufferedLog::Printf("   %s startup at: %s. ", AppName.c_str(), UFC::Hostname);
    UFC::BufferedLog::Printf("                                          ");
    UFC::BufferedLog::Printf("   Build Date:%s %s", __DATE__, __TIME__);
    if (DumpOnly == TRUE)
        UFC::BufferedLog::Printf("   Dump to screen only.");
    UFC::BufferedLog::Printf("   Heartbeat  Time:%d ", HeartbeatTime1 );
    UFC::BufferedLog::Printf("   Receive timeout:%d", FSocketTimeout );
    UFC::BufferedLog::Printf("   Exit On Disconnect:%s ", FExitOnDisconnect? "Yes":"No" );
    UFC::BufferedLog::Printf("   Time to change day:%02d:%02d.%02d", NextDayTime/10000, (NextDayTime/100)%100, NextDayTime%100 );
    UFC::BufferedLog::Printf("__________________________________________");
    
    
    FNormalLog  = new UFC::BufferedLog( "ML Exce normal log","log","MLExec.","log", 10240, TRUE,FALSE,FALSE);    
    FOffHourLog = new UFC::BufferedLog( "ML Exce offhour log","log","MLExec.1.","log", 10240, TRUE,FALSE,FALSE);    
    ///< connect to MBus
    FMessageObject = new MessageObject( AppName, "1.0", "XXXXX" );
    FMessageObject->SetMonitorListener(new ProcessMonitor());
    FMessageObject->Start();
    if( MBusConnectedEvent->WaitFor( 30 ) == FALSE )
    {
    	UFC::BufferedLog::Printf( " Connect to MBus failed." );
    	exit( 0 );
    }

    ///< Create the connection object.
    FConnection = new TTaifexConnection( AppName, this  );
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
        
    FConnection->SetEnableFilledState( TRUE );
    FConnection->AddSellSide( ssForeignExchange );
  

    UFC::AnsiString FileName = "OrderIDToSymbol" + UFC::GetDateString() + ".DATA";
    UFC::BufferedLog::DebugPrintf( " OID file:[%s]", FileName.c_str() );
    
    if( UFC::FileExists( FileName ) == FALSE )
        FSymbolFile = new UFC::FileStreamEx( FileName, "w" );
    else
    {
        LoadSymbolTable( FileName );
        FSymbolFile = new UFC::FileStreamEx( FileName, "a" );
    }
    ///< Start running thread.
    UFC::BufferedLog::DebugPrintf( " OID file ready" );
    Start();
}
//------------------------------------------------------------------------------
void MLExecConnection::LoadSymbolTable( const UFC::AnsiString& FileName )
{
    UFC::FileStreamEx File( FileName, "r");
    UFC::AnsiString Buffer;
    
    while( File.ReadLine( Buffer ) )
    {
        if( Buffer.Length() > 20 )
        {
            UFC::AnsiString OrderID = Buffer.SubString( 0, 5 );
            UFC::AnsiString Symbol = Buffer.SubString( 5, 20 );
            Symbol.TrimRight();
            UFC::BufferedLog::DebugPrintf( " OrderID:[%s]==>[%s]", OrderID.c_str(), Symbol.c_str() );
            FSymbolTable.Add( OrderID, new UFC::AnsiString( Symbol ) );
        }
    }
}
//------------------------------------------------------------------------------
void MLExecConnection::AddToSymbolTable( const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol )
{
    if( FSymbolTable.IsExists( OrderID ) == FALSE )
    {
        UFC::MemoryStream Stream;
        UFC::AnsiString SymStr( Symbol );
        FSymbolTable.Add( OrderID, new UFC::AnsiString( Symbol ) );
        Stream.Write( OrderID.c_str(), OrderID.Length() );
        SymStr.PadThis( 20, ' ' );
        SymStr += "\n";
        Stream.Write( SymStr.c_str(), SymStr.Length() );
        FSymbolFile->Write( Stream.GetBuffer(), Stream.GetSize() );
        FSymbolFile->Flush();
    }
}
//------------------------------------------------------------------------------
void MLExecConnection::LoadSetting(void)
{
    UFC::AnsiString Value;
    
    try
    {
        pConfig = new UFC::UiniFile(ConfigName);
        if (pConfig->GetValue("Setting", "IP", FIP) == FALSE)
        {
            UFC::BufferedLog::Printf(" Speedy Server IP not set.");
            exit(1);
        }
        if (pConfig->GetValue("Setting", "ID", FID) == FALSE)
        {
            UFC::BufferedLog::Printf(" Logon ID not set.");
            exit(1);
        }
        if (pConfig->GetValue("Setting", "Password", FPassword) == FALSE)
        {
            UFC::BufferedLog::Printf(" Logon Password not set.");
            exit(1);
        }
        if (pConfig->GetValue("Setting", "Account", FAccount) == FALSE)
        {
            UFC::BufferedLog::Printf(" Logon Account not set.");
            exit(1);
        }
        if (pConfig->GetValue("Setting", "BrokerID", FBrokerID) == FALSE)
        {
            UFC::BufferedLog::Printf(" BrokerID not set.");
            exit(1);
        }
        if (pConfig->GetValue("Setting", "NextDayTime", Value ) == TRUE )
            NextDayTime = Value.ToInt();
        
        if( pConfig->GetValue("Setting", "ExitOnDisconnect", Value) == TRUE )
            FExitOnDisconnect = Value.ToInt();
        if( pConfig->GetValue( "Socket", "HeartbeatTime", Value ) == TRUE )
        {
            HeartbeatTime1 = Value.ToInt();
            UFC::BufferedLog::Printf(" Heartbeat interval:%d.", HeartbeatTime1 );            
        }
        if( pConfig->GetValue( "Socket", "SocketTimeout", Value ) )
            FSocketTimeout = Value.ToInt();
        UFC::BufferedLog::DebugPrintf( " Socket Timeout:[%d] sec. ", FSocketTimeout );
    }
    catch (...)
    {
        UFC::BufferedLog::Printf(" %s not found.", ConfigName.c_str());
        exit(1);
    }
}
//------------------------------------------------------------------------------
MLExecConnection::~MLExecConnection(void)
{
    if (FConnection != NULL)
        delete FConnection;
    if (client1 != NULL)
        delete client1;
    if (Client1Monitor != NULL)
        delete Client1Monitor;
    if( pConfig != NULL )
        delete pConfig;
}
//------------------------------------------------------------------------------
void MLExecConnection::OnConnected(void)
{
    UFC::BufferedLog::Printf(" logon( ID:[%s], Password:[%s] )", FID.c_str(), FPassword.c_str());
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str(), FExecType );
    FIsLogon = TRUE;
}
//------------------------------------------------------------------------------
void MLExecConnection::OnDisconnected(void)
{
    if( FFirstRecover == FALSE )
        FRecoverTime = FRecoverBeginTime.ToInt32();
    UFC::BufferedLog::Printf("-- *** Connection broken *** --");
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void MLExecConnection::OnLogonReply(const char* ReplyString, LogonResult Result, int CID)
{
    if( Result == lrOk )
    {
        UFC::BufferedLog::Printf(" %s( CID:%d )....logon ok", ReplyString, CID);

        if( revicetime.Length() == 6 && revicetime.ToInt() > 0 )
        {
            UFC::BufferedLog::DebugPrintf(" FConnection->Recover( %s, rdBoth, rmAll  )", revicetime.c_str());
            FConnection->Recover(revicetime.c_str(), rdBoth, rmAll);
            //FConnection->Recover("080000", rdBoth, rmAll);
            revicetime.SetLength(0);
        }
        else
        {

            UFC::AnsiString BeginTime;
            
            BeginTime.Printf( "%06d", FRecoverTime);
            UFC::BufferedLog::DebugPrintf(" FConnection->Recover( %s, rdBoth, rmAll  )", BeginTime.c_str());
            FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll);
        }
        FFirstRecover = FALSE;
    }
    else
    {
        UFC::BufferedLog::Printf(" %s....logon failed", ReplyString);
        exit(0);
    }
}
//------------------------------------------------------------------------------
void MLExecConnection::OnRecoverFinished(int Count)
{
    FFilterSID = 0;
    FFilterSeq = 0;
    FFilterEndSeq = 0;
    UFC::BufferedLog::DebugPrintf(" MLExecConnection::OnRecoverFinished( Count = %d )", Count);
    UFC::SleepMS(1000);
}
//------------------------------------------------------------------------------
bool MLExecConnection::CheckReportMarket(TExecutionReportMessage *Msg)
{
    nsOrderMessageDefine::MarketEnum msgMarket = Msg->GetMarket();
    
    if ((msgMarket == nsOrderMessageDefine::mTWFutures) || (msgMarket == nsOrderMessageDefine::mTWOptions))
    {        
        if ((FExecMarket == emOTC) || (FExecMarket == emTSE) || (FExecMarket == emPATS))
        {
            UFC::BufferedLog::Printf(" Skip TAIFEX executions");
            return false;
        }        
        if (FFilterSID > 0) ///< Filter Session ID
        {
            Int32 SID = UFC::AnsiString(Msg->GetPVC()).ToInt();
            if (FFilterSID != SID)
            {
                UFC::BufferedLog::Printf(" Skip this execution, SessionID should be %d but get %d.", FFilterSID, SID);
                return false;
            }
        }        
        if ((FFilterSeq > 0) || (FFilterEndSeq > 0))
        {
            Int32 msgSeq = Msg->GetReportSequence();
            if (msgSeq < FFilterSeq)
            {
                UFC::BufferedLog::Printf(" Skip this execution, rpt_seq should > %d but get %d.", FFilterSeq, msgSeq);
                return false;  ///< skip rpt_seq < Seq
            }
            
            if ((FFilterEndSeq != 0) && (msgSeq > FFilterEndSeq)) 
            {
                UFC::BufferedLog::Printf(" Skip this execution, rpt_seq should < %d but get %d.", FFilterEndSeq,  msgSeq);
                return false; ///< skip rpt_seq > EndSeq
            }
        }
    }
    else if (msgMarket == nsOrderMessageDefine::mTSE)
    {
        if ((FExecMarket == emTAIFEX) || (FExecMarket == emOTC) || (FExecMarket == emPATS))
        {
            UFC::BufferedLog::Printf(" Skip TSE executions" );
            return false;
        }
    }
    else if (msgMarket == nsOrderMessageDefine::mOTC)
    {
        if ((FExecMarket == emTAIFEX) || (FExecMarket == emTSE) || (FExecMarket == emPATS))
        {
            UFC::BufferedLog::Printf(" Skip OTC executions");
            return false;
        }
    }  
    else if ((msgMarket == nsOrderMessageDefine::mForeignFutures) || (msgMarket == nsOrderMessageDefine::mForeignOptions))
    {
        if ((FExecMarket == emTAIFEX) || (FExecMarket == emOTC) || (FExecMarket == emTSE))
        {
            UFC::BufferedLog::Printf(" Skip PATS executions");
            return false;
        }
    }
    else
        return false;
    
    return true;
}  //MLExecConnection::CheckReportMarket()
//------------------------------------------------------------------------------
void MLExecConnection::OnExecutionReport(TExecutionReportMessage* Msg, ExecDup Dup )
{
    nsOrderMessageDefine::MarketEnum msgMarket = Msg->GetMarket();
    
    if (!CheckReportMarket(Msg)) return;    
    if( (msgMarket == nsOrderMessageDefine::mTWFutures || msgMarket == nsOrderMessageDefine::mTWOptions) &&
        (Msg->GetReportSequence() > 10000000) )
    {
        UFC::BufferedLog::Printf( " Skip B50 Execution[%s]", Msg->GetSrc() );
        return;
    }
    if( Dup == edPossibleDuplicate )
    {
        UFC::BufferedLog::Printf( " Skip Duplicate Execution[%s]", Msg->GetSrc() );
        return;
    }
    
    nsOrderMessageDefine::ExecTypeEnum apiExecType = Msg->GetExecType();
    switch (Msg->GetOrderStatus())
    {
        case nsOrderMessageDefine::osNew :
            if ((msgMarket == nsOrderMessageDefine::mTWFutures) || (msgMarket == nsOrderMessageDefine::mTWOptions))
                ToBackend( Msg, 1 );
            else if ((msgMarket == nsOrderMessageDefine::mForeignFutures) || (msgMarket == nsOrderMessageDefine::mForeignOptions))
                OverseasConfirmToBackend(Msg);
            else
                TSEConfirmToBackend( Msg );
            break;
        case nsOrderMessageDefine::osReplaced :
        case nsOrderMessageDefine::osCanceled :
        case nsOrderMessageDefine::osExpired :
            if ((msgMarket == nsOrderMessageDefine::mTWFutures) || (msgMarket == nsOrderMessageDefine::mTWOptions))
                ToBackend(Msg, 0);
            else if ((msgMarket == nsOrderMessageDefine::mForeignFutures) || (msgMarket == nsOrderMessageDefine::mForeignOptions))
                OverseasConfirmToBackend(Msg);
            else
                TSEConfirmToBackend( Msg );
            break;
        case nsOrderMessageDefine::osPartiallyFilled :
        case nsOrderMessageDefine::osFilled :
            if ((msgMarket == nsOrderMessageDefine::mTWFutures) || (msgMarket == nsOrderMessageDefine::mTWOptions))
                ToBackend(Msg, 2);
            else if ((msgMarket == nsOrderMessageDefine::mForeignFutures) || (msgMarket == nsOrderMessageDefine::mForeignOptions))
            {
                if (apiExecType == nsOrderMessageDefine::etReplaced)
                    OverseasConfirmToBackend(Msg);
                else
                    OverseasFillToBackend(Msg);
            }
            else
                TSEFilledToBackend( Msg );
            break;
        case nsOrderMessageDefine::osRejected :
            if ((msgMarket == nsOrderMessageDefine::mTWFutures) || (msgMarket == nsOrderMessageDefine::mTWOptions))
                RejectToBackend( Msg );
            else if ((msgMarket == nsOrderMessageDefine::mForeignFutures) || (msgMarket == nsOrderMessageDefine::mForeignOptions))
                OverseasConfirmToBackend( Msg );
            else
                TSEConfirmToBackend( Msg );
            break;
        case nsOrderMessageDefine::osQuoteAccept :
            ToBackend(Msg, 3);
            break;
        case nsOrderMessageDefine::osPendingNew :
        case nsOrderMessageDefine::osPendingReplace :
        case nsOrderMessageDefine::osPendingCancel :
        case nsOrderMessageDefine::osNone:
        case nsOrderMessageDefine::osQuoteRequestAccept:
        default: break;
    }  //switch (Msg->GetOrderStatus())
}  
//------------------------------------------------------------------------------
void MLExecConnection::Execute(void)
{
    while (!IsTerminated())
    {
        if ( FIsLogon == FALSE )
            FConnection->Connect( FIP.c_str(), FPort, 10 );
        sleep( 5 ); ///< wait 10 sec.
    }
}
//------------------------------------------------------------------------------
void MLExecConnection::Recover(const UFC::AnsiString& BeginTime, int SID, int Seq, int EndSeq )
{
    if (FIsLogon == TRUE)
    {
        FFilterSID    = SID;
        FFilterSeq    = Seq;
        FFilterEndSeq = EndSeq;
        UFC::BufferedLog::Printf(" Recover from:%s", BeginTime.c_str());
        if( SID != 0 )
        {
            if( EndSeq == 0)
                UFC::BufferedLog::Printf(" Recover Session:%d from sequence:%d", SID ,Seq );
            else
                UFC::BufferedLog::Printf(" Recover Session:%d from sequence %d to %d.", SID ,Seq, EndSeq );
        }
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll);
    }
}
//------------------------------------------------------------------------------
void MLExecConnection::ConnectToBeckend( void )
{
        if (DumpOnly == FALSE)
        {
            UFC::AnsiString Value,SocketIP;
            Int32 SocketPort;

            if (pConfig->GetValue("Socket", "IP", SocketIP) == FALSE)
            {
                UFC::BufferedLog::Printf(" Socket IP not set.");
                exit(1);
            }

            if (pConfig->GetValue("Socket", "Port", Value) == FALSE)
            {
                UFC::BufferedLog::Printf(" Socket Port not set.");
                exit(1);
            }
            else
                SocketPort = Value.ToInt();

            client1 = new UFC::PClientSocket(SocketIP, SocketPort, FALSE);
            Client1Monitor = new SocketMonitor();
            client1->SetListener(Client1Monitor);

            try
            {
                UFC::BufferedLog::Printf(" Socket client connect to %s:%d", SocketIP.c_str(), SocketPort);                
                client1->Connect();
                client1->SetBlockMode(FALSE);
                client1->SetTCPNoDelay(TRUE);
            }
            catch (UFC::Exception& ex)
            {
                clientIsConnect = FALSE;
                UFC::AnsiString msg;
                msg.Printf(" Socket client connect %s:%d fail:[%s]", SocketIP.c_str(), SocketPort, ex.what());
                UFC::BufferedLog::Printf("%s", msg.c_str());
            }
            catch (...)
            {
                clientIsConnect = FALSE;
                UFC::AnsiString msg;
                msg.Printf(" Socket client connect %s:%d fail.", SocketIP.c_str(), SocketPort);
                UFC::BufferedLog::Printf("%s", msg.c_str());
            }
        }
}
//------------------------------------------------------------------------------
void MLExecConnection::Update( void )
{
    if( client1 == NULL )
    {
    	if( HeartbeatTime1 > 0 && HeartbeatTimeCount1 >= HeartbeatTime1 )
        {
            HeartbeatTimeCount1 = 0;
            UFC::BufferedLog::DebugPrintf(" Should send heartbeat...");
        }
        HeartbeatTimeCount1++;
        return;
    }
    try
    {        
        if( !clientIsConnect )
        {
            UFC::BufferedLog::DebugPrintf(" Socket reconnect to Backend...");
            client1->Connect();
            client1->SetBlockMode(FALSE);
            client1->SetTCPNoDelay(TRUE);
        }
    }
    catch (UFC::Exception& ex)
    {
        UFC::BufferedLog::DebugPrintf(" Socket client reconnect fail:[%s]", ex.what());
    }
    catch (...)
    {
        UFC::BufferedLog::DebugPrintf(" Socket client reconnect fail.");
    }
}
//------------------------------------------------------------------------------
void MLExecConnection::Heartbeat(UFC::PClientSocket* client)
{
    OKD okd;
    memset(&okd,' ',sizeof(OKD) );
    memcpy(okd.BROKER_ID,"8888888",7);    
    okd.END = 0;
    UFC::BufferedLog::DebugPrintf(" Heartbeat");
    Send((char*)&okd, etTAIFEXConfirm);
    //UFC::BufferedLog::DebugPrintf("Heartbeat send");// [%s]",data);
}
//---------------------------------------------------------------------------
bool MLExecConnection::IsProxyUserData(const UFC::AnsiString& OrigUserData,
                                       UFC::AnsiString& AE, Int32& NID, UFC::AnsiString& Data)
{
    if( OrigUserData.Length() >= 21 ) ///< Need 21 bytes "<Proxy:"(7) + AE(>3) + NID(10) + ">"(1)
    {
        Int32 RightQuote = OrigUserData.AnsiPos( ">" );

        if( OrigUserData.AnsiNCompare( PROXY_PREFIX, PROXY_PREFIX_LEN ) == 0 &&  RightQuote > PROXY_PREFIX_LEN )
        {
            AE  = UFC::AnsiString( OrigUserData.c_str() + PROXY_PREFIX_LEN , RightQuote - PROXY_MINUS_AE_LEN ); ///< Use account as AE
            NID = UFC::AnsiString( OrigUserData.c_str() + RightQuote - PROXY_NID_LEN, PROXY_NID_LEN ).ToInt();
            if( OrigUserData.Length() > RightQuote + 1 )
                Data = UFC::AnsiString( OrigUserData.c_str() + RightQuote + 1 );
            else
                Data = "";
            return true;
        }        
    }
    Data = OrigUserData;
    return false;
}  //MLExecConnection::IsProxyUserData()
//------------------------------------------------------------------------------
void MLExecConnection::Send( const char* okd, ExecutionType et )
{
    UFC::AnsiString TimeNow,data,header;
    
    UFC::GetHHMMSS( TimeNow );    
    switch( et )
    {
        case etOverseaConfirm:header.Printf( "0500ADD%s",TimeNow.c_str()); ///0500 oversea confirm  16 + 481 + 3 = 500
                              break;
        case etOverseaFilled: header.Printf( "0510ADD%s",TimeNow.c_str()); ///0510 oversea filled  16 + 491 + 3 = 510
                              break;
        case etTAIFEXConfirm:
        case etTAIFEXFilled:
        default:              header.Printf( "0415ADD%s",TimeNow.c_str()); //Length 1+ 4(length) + 3(ADD) + 6(HHMMSS) + 2(check bytes) = 16
                              break;
    }
    makeCheckWord( header );
    data.Printf("\2%s%s  \3",header.c_str(),okd);    
    UFC::BufferedLog::Printf(" MLExecConnection::Send() data[%d]",data.Length());        
    if( DumpOnly == FALSE )
    {
        if (client1 != NULL && client1->IsConnect())
        {
            try
            {
                char Buffer[ 18 ];
                
                memset( Buffer, '\0', 18 );
                UFC::BufferedLog::DebugPrintf(" Send:[%s]", okd);
                UFC::BufferedLog::FlushToFile();
                client1->BlockSend((Int8*) data.c_str(), data.Length());
                HeartbeatTimeCount1 = 0;
                if( client1->RecvBufferWithTimeout((UInt8*) Buffer, 17, FSocketTimeout ) > 0)
                {
                    UFC::BufferedLog::Printf(" Received ack:[%s]", Buffer);
                    UFC::BufferedLog::FlushToFile();
                    return;
                }
                else
                {
                    SendAlert( "Received ack timeout! ");
                    UFC::BufferedLog::Printf(" Received ack timeout!");
                    UFC::BufferedLog::FlushToFile();
                }
            }
            catch (UFC::Exception& ex)
            {
                UFC::BufferedLog::Printf(" Exception:[%s]", ex.what());
                clientIsConnect = FALSE;
                client1->Disconnect(TRUE);

                UFC::BufferedLog::Printf(" Recv Timeout!");
                UFC::BufferedLog::FlushToFile();
                UFC::SleepMS( 3000 );
                if( FExitOnDisconnect == true )
                    exit(0);
            }            
        }
        UFC::BufferedLog::Printf(" Send fail:[%s]", okd);
    }
    else
    {
        UFC::BufferedLog::Printf(" Send dump:[%s]", okd);
    }
    UFC::BufferedLog::FlushToFile();
}
//------------------------------------------------------------------------------
//
//  TWSE/OTC Executions
//
//------------------------------------------------------------------------------
void MLExecConnection::TSEConfirmToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString UniqueKey( Msg->GetExecID() );    
    UFC::AnsiString AE;
    int             AEInt = UFC::AnsiString( Msg->GetAE()).ToInt();
    RO_TSE          RO;    
    
    AE.Printf( "%04d", AEInt );
    memset( &RO, ' ', sizeof( RO_TSE ) );
    memcpy( RO.RO_T020,       Msg->GetSrc() + 14 , 55 );
    memcpy( RO.FUNCTION_CODE, Msg->GetSrc() + 2  , 2 );    
    memcpy( RO.T030_STATUS,   Msg->GetSrc() + 12 , 2 );        
    ///< Skip TIME_OUT[1];     ///< X(1)  offset(59)    
    memcpy( RO.SELLER,        AE.c_str()         , 4 );                
    ///< Skip NETNO[7];        ///< X(7)  offset(64) 
    ///< Skip NET_SOURCE[1];   ///< X(1)  offset(71)
    ///< Skip NET_KIND[1];     ///< X(1)  offset(72)
    UFC::AnsiString userData;
    Int32 NID = 0;    
    
    if( IsProxyUserData( Msg->GetData(), AE, NID, userData ) == TRUE )    
        UFC::BufferedLog::DebugPrintf( " Proxy UserData:[%s]", userData.c_str());
    else
        UFC::BufferedLog::DebugPrintf( " Gateway UserData:[%s]", userData.c_str());
    if( userData.Length() >= 7 )
    {
        UFC::AnsiString SubAccount( userData.SubString( 0, 7 ) );
        UFC::BufferedLog::DebugPrintf( " SubAccount:[%s]", SubAccount.c_str() );
        memcpy( RO.INNER_ACNO, SubAccount.c_str(), 7 );
    }      
    UniqueKey.PadThis(20,'0');
    memcpy( RO.UNI_DATA,UniqueKey.c_str(),20);    
    RO.EOS = 0;    
    Send((char*)&RO, etTAIFEXConfirm);
}
//------------------------------------------------------------------------------    
void MLExecConnection::TSEFilledToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString UniqueKey( Msg->GetExecID() );
    UFC::AnsiString AE;
    RE_TSE          RE;        
    
    memset( &RE, ' ', sizeof( RE_TSE ) );    
    memcpy( RE.RE_R030, Msg->GetSrc(), 62 );
    memcpy( RE.RECEIVE_TIME, Msg->GetTransactTime(), 8);    
    UFC::AnsiString userData;
    Int32 NID = 0;    
    
    if( IsProxyUserData( Msg->GetData(), AE, NID, userData ) == TRUE )    
        UFC::BufferedLog::DebugPrintf( " Proxy UserData:[%s]", userData.c_str());
    else
        UFC::BufferedLog::DebugPrintf( " Gateway UserData:[%s]", userData.c_str());
    if( userData.Length() >= 7 )
    {
        UFC::AnsiString SubAccount( userData.SubString( 0, 7 ) );
        UFC::BufferedLog::DebugPrintf( " SubAccount:[%s]", SubAccount.c_str() );
        memcpy( RE.INNER_ACNO, SubAccount.c_str(), 7 );
    }    
    UniqueKey.PadThis(20,'0');
    memcpy( RE.UNI_DATA,UniqueKey.c_str(),20);
    RE.EOS = 0;
    Send((char*)&RE, etTAIFEXFilled );    
}        
//------------------------------------------------------------------------------



