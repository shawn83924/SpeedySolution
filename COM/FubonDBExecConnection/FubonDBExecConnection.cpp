#include "FubonDBExecConnection.h"
#include "../../UFC/NetUtility.h"
//------------------------------------------------------------------------------
extern BOOL DumpOnly;
extern UInt16 COMP_6(const UFC::AnsiString& in, UInt8*& out);
//------------------------------------------------------------------------------
FubonDBExecConnection::FubonDBExecConnection(int RecoverBeginTime)
: UFC::PThread(NULL, FALSE)
, FAppName("FubonDBExecConnection")
, FIP("127.0.0.1")
, FPort(23456)
, FID("Admin")
, FPassword("1234rewq")
, FAccount("0000001")
, FBrokerID("F021999")
, FRecoverBeginTime(RecoverBeginTime)
, FConnection(NULL)
, FIsLogon(FALSE)
{
    UFC::AnsiString LogName = "../log/FubonDBExec" + UFC::GetDateString() + ".log";
    LoadSetting();
    if (DumpOnly == TRUE)
        UFC::BufferedLog::Printf(" Dump to screen only.");
    UFC::BufferedLog::Printf("__________________________________________");
    UFC::BufferedLog::Printf("                                          ");
    UFC::BufferedLog::Printf("   FubonDB backend interface module           ");
    UFC::BufferedLog::Printf("   startup at: %s.             ", UFC::Hostname);
    UFC::BufferedLog::Printf("                                          ");
    UFC::BufferedLog::Printf("   Build Date:%s ", __DATE__);
    if (DumpOnly == TRUE)
        UFC::BufferedLog::Printf("   Debug Mode:On (Dump to stdout only)");
    else
        UFC::BufferedLog::Printf("   Debug Mode:Off");
    UFC::BufferedLog::Printf("__________________________________________");
    ///< Create the connection object.
    FConnection = new TTaifexConnection(NULL, &FResource, FAppName, this, TRUE);
    FConnection->SetUseNewFuturesSymbol(TRUE);
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID(nsOrderMessageDefine::mFutures, FBrokerID.c_str());
    FConnection->SetCMID(FBrokerID.SubString(0, 4).c_str());
    ///< Load TAIFEX message format config files.
    FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
    FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
    FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
    FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );
    //FConnection->SetFutDataFormatResourceID(IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI");
    //FConnection->SetOptDataFormatResourceID(IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI");
    //FConnection->SetQuoteDataFormatResourceID(IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI");
    ///< Set log file name.    
    FConnection->SetLogFile(LogName);
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------

void FubonDBExecConnection::LoadSetting(void)
{
    try
    {
        UFC::AnsiString MHOKFilePath;
        UFC::AnsiString BSOKFilePath;
        UFC::AnsiString filename;
        UFC::UDateTime  DateTime;        
        UFC::UiniFile   Config("../cfg/FubonDBExecConnection.cfg");
        
        FDate.Printf( "%02d", DateTime.getDate() );
        ///< Load basic setting for connect to Speedy server.
        if (Config.GetValue("Setting", "IP", FIP) == FALSE)
        {
            UFC::BufferedLog::Printf(" Speedy Server IP not set.");
            exit(1);
        }
        if (Config.GetValue("Setting", "ID", FID) == FALSE)
        {
            UFC::BufferedLog::Printf(" Logon ID not set.");
            exit(1);
        }
        if (Config.GetValue("Setting", "Password", FPassword) == FALSE)
        {
            UFC::BufferedLog::Printf(" Logon Password not set.");
            exit(1);
        }
        if (Config.GetValue("Setting", "Account", FAccount) == FALSE)
        {
            UFC::BufferedLog::Printf(" Logon Account not set.");
            exit(1);
        }
        if (Config.GetValue("Setting", "BrokerID", FBrokerID) == FALSE)
        {
            UFC::BufferedLog::Printf(" BrokerID not set.");
            exit(1);
        }
        ////< Load setting for write BSOK
        if (Config.GetValue("BSOK", "SLINE", FSLINE) == FALSE)
        {
            UFC::BufferedLog::Printf(" SLINE not set.");
            exit(1);
        }
        FSLINE.PadThis( 2, ' ');
        if (Config.GetValue("BSOK", "HTSQ", FHTSQ) == FALSE)
        {
            UFC::BufferedLog::Printf(" HTSQ not set.");
            exit(1);
        }
        else
        {
            FHTSQ.Printf("%04d", FHTSQ.ToInt() % 10000);
        }
        if (Config.GetValue("BSOK", "INSQ", FINSQ) == FALSE)
        {
            UFC::BufferedLog::Printf(" INSQ not set.");
            exit(1);
        }
        else
        {
            FINSQ.Printf("%06d", FINSQ.ToInt() % 1000000);
        }
        if (Config.GetValue("BSOK", "MGSQ", FMGSQ) == FALSE)
        {
            UFC::BufferedLog::Printf(" MGSQ not set.");
            exit(1);
        }
        else
        {
            FMGSQ.Printf("%06d", FMGSQ.ToInt() % 1000000);
        }
        if (Config.GetValue("BSOK", "SCODE", FSCODE) == FALSE)
        {
            UFC::BufferedLog::Printf(" SCODE not set.");
            exit(1);
        }
        FSLINE.PadThis(1, ' ');
        if (Config.GetValue("BSOK", "TLNO", FTLNO) == FALSE)
        {
            UFC::BufferedLog::Printf(" TLNO not set.");
            exit(1);
        }
        FTLNO.PadThis(6, ' ');

        
        if (Config.GetValue("BSOK", "FilePath", BSOKFilePath) == FALSE)
        {
            UFC::BufferedLog::Printf(" BSOK FilePath not set.");
            exit(1);
        }
        ////< Load setting for write MHOK
        if (Config.GetValue("MHOK", "LINE", FMHOKLNO ) == FALSE)
        {
            UFC::BufferedLog::Printf(" LINE not set in [MHOK].");
            exit(1);
        }
        FMHOKLNO.PadThis(2, ' ');      
        
        if (Config.GetValue("MHOK", "FilePath", MHOKFilePath) == FALSE)
        {
            UFC::BufferedLog::Printf(" MHOK FilePath not set in [MHOK].");
            exit(1);
        }
        
        try
        {
            filename.Printf("%s/%s%02d%02d.%02d", BSOKFilePath.c_str(), "BSOK", DateTime.getMonth(), DateTime.getDate(), DateTime.getYear() - 1911);
            FBSOKDataFile = new UFC::FileStreamEx(filename.c_str(), "a+" );
            UFC::BufferedLog::Printf(" Open BSOK File:%s", filename.c_str());
            
            filename.Printf("%s/%s%02d%02d.%02d", MHOKFilePath.c_str(), "RMHOK", DateTime.getMonth(), DateTime.getDate(), DateTime.getYear() - 1911);
            FMHOKDataFile = new UFC::FileStreamEx( filename.c_str(), "a+" );
            UFC::BufferedLog::Printf(" Open MHOK File:%s", filename.c_str());
        }
        catch (...)
        {
            UFC::BufferedLog::Printf(" Open File %s error", filename.c_str());
            exit(1);
        }
    }
    catch (...)
    {
        UFC::BufferedLog::Printf(" FubonDBExecConnection.cfg not found.");
        exit(1);
    }
}
//------------------------------------------------------------------------------
FubonDBExecConnection::~FubonDBExecConnection(void)
{
    if (FConnection != NULL)
        delete FConnection;
}
//------------------------------------------------------------------------------

void FubonDBExecConnection::OnConnected(void)
{
    UFC::BufferedLog::Printf(" logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str());
    FConnection->Logon(FID.c_str(), FPassword.c_str(), FAccount.c_str());
}
//------------------------------------------------------------------------------

void FubonDBExecConnection::OnDisconnected(void)
{
    UFC::BufferedLog::Printf(" *** Connection broken ***");
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void FubonDBExecConnection::OnLogonReply(const char* ReplyString, bool IsAccepted, int CID)
{
    if (IsAccepted)
    {
        UFC::BufferedLog::Printf(" %s[CID=%d]....logon ok", ReplyString, CID);
        FIsLogon = TRUE;
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf("%06d", FRecoverBeginTime);

        UFC::BufferedLog::DebugPrintf(" FConnection->Recover( %s, rdBoth, rmAll  )", BeginTime.c_str());
        FConnection->Recover(BeginTime.c_str(), rdBoth, rmAll);
    }
    else
    {
        UFC::BufferedLog::Printf(" Logon failed reason[%s]", ReplyString);
        exit(0);
    }
}
//------------------------------------------------------------------------------
void FubonDBExecConnection::Recover(UFC::AnsiString BeginTime)
{
    if( FIsLogon == TRUE )
    {
        FConnection->Recover(BeginTime.c_str(), rdBoth, rmAll);
        UFC::BufferedLog::Printf(" Recover from:%s", BeginTime.c_str());
    }
}
//------------------------------------------------------------------------------
void FubonDBExecConnection::OnRecoverFinished(int Count)
{
    UFC::BufferedLog::DebugPrintf(" FubonDBExecConnection::OnRecoverFinished( Count = %d )", Count);
    UFC::SleepMS(1000);
}
//------------------------------------------------------------------------------
void FubonDBExecConnection::ConfirmToBackend(TExecutionReportMessage* Msg)
{
    UInt8 Data[BSOKTOTALSIZE];
    memset(Data, ' ', BSOKTOTALSIZE);

    UInt16 msgpos = 0;

    memcpy(Data + msgpos, Msg->GetPVC(), 2);
    msgpos += 2;

    UInt16 subsys = 0;
    UInt16 ecode = 0;
    switch (Msg->GetTradingSessionID())
    {
        case nsOrderMessageDefine::tsNormal :
            subsys = 30;
            ecode = 0;
            break;
        case nsOrderMessageDefine::tsOddLot :
            subsys = 40;
            ecode = 2;
            break;
        default: break;
    }
    UInt16 fncode = 0;
    if( Msg->GetOrderStatus() == nsOrderMessageDefine::osNew )
    {
        switch (Msg->GetSide())
        {
            default: break;
            case nsOrderMessageDefine::sBuy :
                fncode = 1;
            break;
            case nsOrderMessageDefine::sSell :
                fncode = 2;
            break;
        }
    }
    else
    {
        switch (Msg->GetOrderStatus())
        {
            default: break;
            case nsOrderMessageDefine::osReplaced :
                    fncode = 3;
            break;
            case nsOrderMessageDefine::osCanceled :
                    fncode = 4;
                break;
        }
    }
    UInt16 msgtype = 0;
    UFC::AnsiString data;
    data.Printf("%02d%02d%02d", subsys, fncode, msgtype);
    memcpy(Data + msgpos, data.c_str(), 6);
    msgpos += 6;

    UFC::UDateTime DateTime;
    data.Printf("%02d%02d%02d", DateTime.getHour(), DateTime.getMinute(), DateTime.getSecond());
    UInt8* compdata;
    UInt16 size = COMP_6(data.c_str(), compdata);
    memcpy(Data + msgpos, compdata, 3);
    msgpos += 3;

    memcpy(Data + msgpos, Msg->GetStatusCode(), 2);
    msgpos += 2;

    memcpy(Data + msgpos, Msg->GetBrokerID(), 4);
    msgpos += 4;

    memcpy(Data + msgpos, Msg->GetPVC(), 2);
    msgpos += 2;

    memcpy(Data + msgpos, Msg->GetOrderID(), 1);
    msgpos += 1;

    size = COMP_6(Msg->GetOrderID() + 1, compdata);
    memcpy(Data + msgpos, compdata, 2);
    msgpos += 2;

    data.Printf("%s", Msg->GetAccount());
    size = COMP_6(data.SubString(0, 6).c_str(), compdata);
    memcpy(Data + msgpos, compdata, 3);
    msgpos += 3;
    memcpy(Data + msgpos, data.c_str() + 6, 1);
    msgpos += 1;

    memcpy(Data + msgpos, Msg->GetAccountFlag(), 1);
    msgpos += 1;
    
    UFC::AnsiString stockid(Msg->GetSymbol());
    stockid.PadThis(6, ' ');
    memcpy(Data + msgpos, stockid.c_str(), 6);
    msgpos += 6;

    Int32 price = UFC::DoubleToInt(Msg->GetPrice(), 2);
    data.Printf("%06d", price);
    size = COMP_6(data.c_str(), compdata);
    memcpy(Data + msgpos, compdata, 3);
    msgpos += 3;

    data.Printf("%06d", Msg->GetOrderQty());
    size = COMP_6(data.c_str(), compdata);
    memcpy(Data + msgpos, compdata, 3);
    msgpos += 3;

    UInt8 c = 'Y';
    switch (Msg->GetSide())
    {
        default: break;
        case nsOrderMessageDefine::sBuy :
            c = 'B';
        break;
        case nsOrderMessageDefine::sSell :
            c = 'S';
        break;
    }
    memcpy(Data + msgpos, &c, 1);
    msgpos += 1;

    c = '0' + ecode;
    memcpy(Data + msgpos, &c, 1);
    msgpos += 1;
    
    memcpy(Data + msgpos, Msg->GetTSEOrderType(), 1);
    msgpos += 1;

    //memcpy(Data + msgpos, "    ", 4);
    msgpos += 4;

    data.Printf("%04d%02d%02d", DateTime.getYear(), DateTime.getMonth(), DateTime.getDate());
    size = COMP_6(data.c_str(), compdata);
    memcpy(Data + msgpos, compdata, 4);
    msgpos += 4;

    data.Printf("%s", Msg->GetTransactTime());
    size = COMP_6(data.SubString(0, 6).c_str(), compdata);
    memcpy(Data + msgpos, compdata, 3);
    msgpos += 3;

    data.Printf("%06d", Msg->GetBeforeQty());
    size = COMP_6(data.c_str(), compdata);
    memcpy(Data + msgpos, compdata, 3);
    msgpos += 3;

    data.Printf("%06d", Msg->GetAfterQty());
    size = COMP_6(data.c_str(), compdata);
    memcpy(Data + msgpos, compdata, 3);
    msgpos += 3;

    memcpy(Data + msgpos, "Y", 1);
    msgpos += 1;

    memcpy(Data + msgpos, FSLINE.c_str(), 2);
    msgpos += 2;

    size = COMP_6(FHTSQ.c_str(), compdata);
    memcpy(Data + msgpos, compdata, 2);
    msgpos += 2;

    size = COMP_6(FINSQ.c_str(), compdata);
    memcpy(Data + msgpos, compdata, 3);
    msgpos += 3;

    size = COMP_6(FMGSQ.c_str(), compdata);
    memcpy(Data + msgpos, compdata, 3);
    msgpos += 3;

    data.Printf("%02d%02d%02d", DateTime.getHour(), DateTime.getMinute(), DateTime.getSecond());
    size = COMP_6(data.c_str(), compdata);
    memcpy(Data + msgpos, compdata, 3);
    msgpos += 3;

    memcpy(Data + msgpos, FSCODE.c_str(), 1);
    msgpos += 1;

    memcpy(Data + msgpos, "1", 1);
    msgpos += 1;

    memcpy(Data + msgpos, FTLNO.c_str(), 6);
    msgpos += 6;
    
    UFC::AnsiString AES(Msg->GetAE());
    AES.PadThis(3, ' ');
    memcpy(Data + msgpos, AES.c_str(), 3);
    msgpos += 3;

    UFC::AnsiString DataS(Msg->GetData());
    DataS.PadThis(6, ' ');
    memcpy(Data + msgpos, DataS.c_str(), 6);
    msgpos += 6;

    FBSOKDataFile->Write(Data, BSOKTOTALSIZE);
    FBSOKDataFile->Flush();
}
//------------------------------------------------------------------------------
void FubonDBExecConnection::FillToBackend(TExecutionReportMessage* Msg)
{
    UFC::AnsiString Type,R3Message;    
    UFC::AnsiString Body(Msg->GetSrc());
      
    if( Msg->GetMarket() == nsOrderMessageDefine::mTSE )
        Type = "1 ";
    else if( Msg->GetMarket() == nsOrderMessageDefine::mOTC )
        Type = "2 ";        

    Body.PadThis(62, ' ');
    R3Message.Printf( "%s%s%s10%s",FDate.c_str(),Type.c_str(),FMHOKLNO.c_str(),Body.c_str() );
    ///< Length: 8(Header)+62(R3 Body)
    FMHOKDataFile->Write( R3Message.c_str(), MHOKTOTALSIZE);
    FMHOKDataFile->Flush();
}
//------------------------------------------------------------------------------
void FubonDBExecConnection::OnExecutionReport(TExecutionReportMessage* Msg, bool PossDup)
{
    switch (Msg->GetOrderStatus())
    {
        case nsOrderMessageDefine::osNew :
        case nsOrderMessageDefine::osReplaced :
        case nsOrderMessageDefine::osCanceled :  ConfirmToBackend( Msg );break;                                   
        case nsOrderMessageDefine::osPartiallyFilled :
        case nsOrderMessageDefine::osFilled :    FillToBackend( Msg );break;
        case nsOrderMessageDefine::osPendingNew :
        case nsOrderMessageDefine::osPendingReplace :
        case nsOrderMessageDefine::osPendingCancel :
        case nsOrderMessageDefine::osRejected :
        default: break;
    }
}
//------------------------------------------------------------------------------
void FubonDBExecConnection::Execute(void)
{
    while (!IsTerminated())
    {
        if (FIsLogon == FALSE)
            FConnection->Connect(FIP.c_str(), FPort);
        sleep(10); ///< wait 10 sec.
    }
}
//------------------------------------------------------------------------------
