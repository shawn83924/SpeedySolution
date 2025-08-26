#include "GlobalVariable.h"
#include "PMFExecToFile.h"
#include "../../UFC/NetUtility.h"
#include <math.h>
//------------------------------------------------------------------------------
const UFC::AnsiString ChangeLine( "\n" );
UFC::AnsiString       Dir( "/Speedy/ulog" );
//------------------------------------------------------------------------------        
//
// Class TExecFile 
//
//------------------------------------------------------------------------------    
TExecFile::TExecFile( const UFC::AnsiString& FutureFileName, 
                      const UFC::AnsiString& OptionFileName,
                      const UFC::AnsiString& StockConfirmFileName,
                      const UFC::AnsiString& StockFillFileName,
                      const UFC::AnsiString& ForeignConfirmFileName, 
                      const UFC::AnsiString& ForeignFillFileName )
:FFutureFilePtr(0)
,FOptionFilePtr(0)
,FForeignConfirmFilePtr(0)
,FForeignFillFilePtr(0)
,FStockConfirmPtr(0)
,FStockFillPtr(0)
{
    if( FutureFileName.Length() > 0)     
        FFutureFilePtr = OpenExecutionReportFile(FutureFileName);
    if( OptionFileName.Length() > 0)     
        FOptionFilePtr = OpenExecutionReportFile(OptionFileName);
    if( StockConfirmFileName.Length() > 0) 
        FStockConfirmPtr = OpenExecutionReportFile(StockConfirmFileName);
    if( StockFillFileName.Length() > 0) 
        FStockFillPtr = OpenExecutionReportFile(StockFillFileName);
    if( ForeignConfirmFileName.Length() > 0) 
        FForeignConfirmFilePtr = OpenExecutionReportFile(ForeignConfirmFileName);
    if( ForeignFillFileName.Length() > 0) 
        FForeignFillFilePtr = OpenExecutionReportFile(ForeignFillFileName);
}
//-----------FOptionFilePtr-------------------------------------------------------------------    
TExecFile::~TExecFile()
{
    if (FFutureFilePtr != 0)   delete FFutureFilePtr;
    if (FOptionFilePtr != 0)   delete FOptionFilePtr;
    if (FStockConfirmPtr != 0) delete FStockConfirmPtr;
    if (FStockFillPtr != 0)    delete FStockFillPtr;    
    if (FForeignConfirmFilePtr != 0) delete FForeignConfirmFilePtr;
    if (FForeignFillFilePtr != 0)    delete FForeignFillFilePtr;
}  //TExecFile::~TExecFile()
//------------------------------------------------------------------------------    
UFC::FileStream* TExecFile::OpenExecutionReportFile(const UFC::AnsiString& ExecRptFileName)
{
    UFC::FileStream *execRptFilePtr = 0;
    if (ExecRptFileName.Length() > 0)
    {
        try 
        {
            if (UFC::FileExists(ExecRptFileName) == FALSE)
                execRptFilePtr = new UFC::FileStream(ExecRptFileName, O_WRONLY | O_CREAT | O_APPEND);
            else
                execRptFilePtr = new UFC::FileStream(ExecRptFileName, O_WRONLY | O_APPEND);    
            UFC::BufferedLog::Printf( " TExecFile::%s() Open Execution Report File:%s", __func__, ExecRptFileName.c_str());    
        }
        catch (UFC::FileException& fe)
        {
            UFC::BufferedLog::Printf( " TExecFile::%s() Open Execution Report File[%s] Exception:%s ", __func__, ExecRptFileName.c_str(), fe.what());
            execRptFilePtr = 0;
        }
    }
    else
        UFC::BufferedLog::Printf( " TExecFile::%s() Open Execution Report File failed, wrong file name", __func__);
    return execRptFilePtr;
}  //TExecFile::OpenExecutionReportFile()
//------------------------------------------------------------------------------    
void TExecFile::Write( nsOrderMessageDefine::MarketEnum Market, nsOrderMessageDefine::ExecTypeEnum ExecType, nsOrderMessageDefine::OrderStatusEnum OrderStatus ,const char* Data, int Length )
{
    int writeDataSize = 0;
    UFC::AnsiString marketName = "";
    UFC::FileStream* ExecFile = NULL;
    try
    {
        if( Market == nsOrderMessageDefine::mTWFutures ) 
        {
            marketName = "Future";
            ExecFile = FFutureFilePtr;            
        }
        else if( Market == nsOrderMessageDefine::mTWOptions )
        {
            marketName = "Option";
            ExecFile = FOptionFilePtr;                        
        }
        else if( Market == nsOrderMessageDefine::mTSE || Market == nsOrderMessageDefine::mOTC )
        {            
            if ( ( OrderStatus == nsOrderMessageDefine::osFilled ) || ( OrderStatus == nsOrderMessageDefine::osPartiallyFilled ) )
            {
                marketName = "Stock Filled";
                ExecFile = FStockFillPtr;
            }
            else
            {
                marketName = "Stock Confirm";
                ExecFile = FStockConfirmPtr;                
            }
        }
        else if( ( Market == nsOrderMessageDefine::mForeignFutures ) || ( Market == nsOrderMessageDefine::mForeignOptions ) )
        {
            if ( ( OrderStatus == nsOrderMessageDefine::osFilled ) || ( OrderStatus == nsOrderMessageDefine::osPartiallyFilled ) )
            {
                if( ExecType == nsOrderMessageDefine::etReplaced )
                {
                    marketName = "Foreign Replaced Confirm";
                    ExecFile = FForeignConfirmFilePtr;                                    
                }
                else if( ExecType == nsOrderMessageDefine::etRejected )
                {
                    marketName = "Foreign Reject Confirm";
                    ExecFile = FForeignConfirmFilePtr;                                    
                }
                else
                {
                    if( OrderStatus == nsOrderMessageDefine::osFilled )
                        marketName = "Foreign Filled";
                    else if( OrderStatus == nsOrderMessageDefine::osPartiallyFilled )
                        marketName = "Foreign Partially Filled";
                    else
                        marketName = "Foreign Fill";
                    ExecFile = FForeignFillFilePtr;                                                        
                }
            }
            else
            {
                marketName = "Foreign Confirm";
                ExecFile = FForeignConfirmFilePtr;                                                        
            }
        }
        else
            UFC::BufferedLog::Printf( " TExecFile::%s() ignore Market[%d] report", __func__, Market );
        if( ExecFile != NULL )
        {
            writeDataSize = ExecFile->Write( Data, Length );
            ExecFile->Flush();
            UFC::BufferedLog::Printf( " TExecFile::%s() Write %d / %d bytes to [%s] File.", __func__, writeDataSize, Length, marketName.c_str() );
        }
    }
    catch (UFC::FileException& fe)
    {
        UFC::BufferedLog::Printf( " TExecFile::%s() Exception: %s", __func__, fe.what() );
    }
}  //TExecFile::Write()

//------------------------------------------------------------------------------
//
// Class PMFExecToFile
//    
//------------------------------------------------------------------------------
UFC::PInt32 PMFExecToFile::FReportSeqNo;
UFC::AnsiString PMFExecToFile::FReportSeqNoFileName = "PMFExec2FileSeqNo";
UFC::FileStreamEx *PMFExecToFile::FExecIDSeqNoFilePtr = 0;
UFC::PHashMap<UFC::AnsiString, Int32> PMFExecToFile::FSeqNoExecIDIndex;
UFC::AnsiString PMFExecToFile::FPreRptSeqPrefix = "";
bool PMFExecToFile::FIsRecovering = false;

//------------------------------------------------------------------------------    
Int32 PMFExecToFile::GetReportSeqNo()
{
    return FReportSeqNo.ToInt32();
}  //PMFExecToFile::GetSequenceNo()
//------------------------------------------------------------------------------    
void PMFExecToFile::SetReportSeqNo(Int32 NewValue)
{
    FReportSeqNo = NewValue;
}  //PMFExecToFile::SetSequenceNo()
//------------------------------------------------------------------------------    
Int32 PMFExecToFile::IncreaseReportSeqNo(Int32 IncreaseValue)
{
    Int32 newValue = FReportSeqNo.ToInt32() + IncreaseValue;
    if ((newValue > 99999) || (newValue <= 0)) newValue = 1;
    FReportSeqNo = newValue;
    return newValue;
}  //PMFExecToFile::IncreaseSequenceNo()
//------------------------------------------------------------------------------    
Int32 PMFExecToFile::GetNextReportSeqNo()
{
    return IncreaseReportSeqNo(1);
}  //PMFExecToFile::GetNextSequenceNo()
//------------------------------------------------------------------------------
Int32 PMFExecToFile::PrepareExecIDSeqNoFile( int RecoverBeginTime )
{
    Int32 maxSeqNo = 0;
    UFC::AnsiString logHead = "PMFExecToFile::PrepareExecIDSeqNoFile()";
    UFC::AnsiString execIDSeqNoFileName = "ExecIDSeqNo.data";
    execIDSeqNoFileName.Printf("%s_ExecIDSeqNo.data", gAppName.c_str());
    try
    {
        if (UFC::FileExists(execIDSeqNoFileName) == FALSE)
        {
            UFC::BufferedLog::Printf(" %s File[%s] not Exists, Create with \"w\"", logHead.c_str(), execIDSeqNoFileName.c_str() );
            FExecIDSeqNoFilePtr = new UFC::FileStreamEx(execIDSeqNoFileName, "w");
        }
        else
        {
            if (RecoverBeginTime == 0) //first time
            {
                UFC::BufferedLog::Printf(" %s First Time start, Create File[%s] with \"w\"", logHead.c_str(), execIDSeqNoFileName.c_str() );
                FExecIDSeqNoFilePtr = new UFC::FileStreamEx(execIDSeqNoFileName, "w");
            }
            else
            {
                UFC::BufferedLog::Printf(" %s not First Time start, Create File[%s] with \"r+\"", logHead.c_str(), execIDSeqNoFileName.c_str() );
                FExecIDSeqNoFilePtr = new UFC::FileStreamEx(execIDSeqNoFileName, "r+");
                int recordCount = 0;
                BOOL isNotEndOfFile = FALSE;
                do 
                {
                    UFC::AnsiString curRecordLine = "";
                    isNotEndOfFile = FExecIDSeqNoFilePtr->ReadLine(curRecordLine);
                    recordCount++;
                    curRecordLine.TrimRight('\n');
                    curRecordLine.TrimRight('\r');
                    int curRecordLength = curRecordLine.Length();
                
                    UFC::AnsiString curExecID = "";
                    if (curRecordLength >= 70) 
                    {
                        curExecID = curRecordLine.SubString(0, 70);
                        curExecID.TrimRight(' ');
                    }
                
                    Int32 curSeqNo = 0;
                    if (curRecordLength > 70) 
                        curSeqNo = curRecordLine.SubString(70, curRecordLength - 70).ToInt(); 
                    
                    if ((curExecID.Length() > 0) && (curSeqNo > 0)) 
                    {                                                
                        if (curSeqNo > maxSeqNo) maxSeqNo = curSeqNo;
                        FSeqNoExecIDIndex.Add(curExecID, curSeqNo);
                        UFC::BufferedLog::Printf(" %s %d ExecID[%s] SeqNo[%d] maxSeqNo[%d]", logHead.c_str(), recordCount, curExecID.c_str(), curSeqNo, maxSeqNo );
                    }
                } while(isNotEndOfFile == TRUE);
            }  //if (RecoverBeginTime == 0)
        }  //if (UFC::FileExists(execIDSeqNoFileName) == FALSE)
    }
    catch(UFC::FileException& fe)
    {
        UFC::BufferedLog::Printf(" %s Load File[%s] failed. %s", logHead.c_str(), execIDSeqNoFileName.c_str(), fe.what() );
    }
        
    UFC::BufferedLog::Printf(" %s return maxSeqNo[%d]", logHead.c_str(), maxSeqNo);
    return maxSeqNo;
}  //PMFExecToFile::PrepareExecIDSeqNoFile()
//------------------------------------------------------------------------------
UFC::AnsiString PMFExecToFile::GetOverseasFieldValueStr(const UFC::AnsiString& ReportStr, const UFC::AnsiString& FieldName)
{
    UFC::AnsiString fieldValue = "";
    UFC::PStringList *reportFieldsListPtr = new UFC::PStringList();
    reportFieldsListPtr->SetStrings(ReportStr, "^");
    for (int i = 0; i < reportFieldsListPtr->ItemCount(); i++)
    {
        UFC::AnsiString curNameValueStr = reportFieldsListPtr->GetItem(i);
        UFC::PStringList nameValuePair;
        nameValuePair.SetStrings(curNameValueStr, "=");
        if (nameValuePair.ItemCount() == 2)
        {
            UFC::AnsiString fieldName = nameValuePair.GetItem(0);
            if (fieldName == FieldName)
            {
                fieldValue = nameValuePair.GetItem(1);
                break;
            }
        }  //if (nameValuePair.ItemCount() == 2)        
    }  
    delete reportFieldsListPtr;
    return fieldValue;
}  //PMFExecToFile::GetOverseasFieldValueStr()
//------------------------------------------------------------------------------
UFC::AnsiString PMFExecToFile::GenerateOverseasRptSeqNameValueStr(const UFC::AnsiString& SourceData)
{
    UFC::AnsiString logHead = "PMFExecToFile::GenerateOverseasRptSeqNameValueStr()";
    UFC::AnsiString nameValueStr = "";
    UFC::AnsiString osExecID = GetOverseasFieldValueStr(SourceData, "eid");
    if (osExecID.Length() <= 0)
    {
        UFC::BufferedLog::Printf(" %s field \"eid\" not exist in [%s].", logHead.c_str(), SourceData.c_str());
        return nameValueStr;
    }

    Int32 osSeqNo = 1;
    UFC::AnsiString curRptSeqStr = "";
    if (FSeqNoExecIDIndex.GetObjectByKey(osExecID, osSeqNo) == FALSE)
    {
        UFC::UDateTime now;
        int nowWeekDay = now.getWeekDay();
        int nowHour = now.getHour();
        if (gAppID > 1) nowHour = nowHour + ((gAppID - 1) * 24);
        UFC::AnsiString curRptSeqPrefix = "";
        curRptSeqPrefix.Printf("%1d%02d", nowWeekDay, nowHour);
        if ((FPreRptSeqPrefix.Length() <= 0) || (curRptSeqPrefix != FPreRptSeqPrefix))
        {
            FPreRptSeqPrefix = curRptSeqPrefix;
            SetReportSeqNo(osSeqNo);
        }
        else
            osSeqNo = GetNextReportSeqNo();
        curRptSeqStr.Printf("%3s%05d", curRptSeqPrefix.c_str(), osSeqNo);

        FSeqNoExecIDIndex.Add(osExecID, curRptSeqStr.ToInt());
        UFC::AnsiString recordStr;
        recordStr.Printf("%-70s%8s\n", osExecID.c_str(), curRptSeqStr.c_str());
        if (FExecIDSeqNoFilePtr != 0)
        {
            FExecIDSeqNoFilePtr->Write(recordStr.c_str(), recordStr.Length());
            FExecIDSeqNoFilePtr->Flush();
        }
    }
    else
        curRptSeqStr.Printf("%08d", osSeqNo);
        
    nameValueStr.Printf("useq=%s", curRptSeqStr.c_str());  //2022/05/17 Set useq is first Field
//    if (SourceData[SourceData.Length() - 1] == '^')
//        nameValueStr.Printf("useq=%s", curRptSeqStr.c_str());
//    else
//        nameValueStr.Printf("^useq=%s", curRptSeqStr.c_str());                
    return nameValueStr;
}  //PMFExecToFile::GenerateOverseasRptSeqNameValueStr()

//------------------------------------------------------------------------------
PMFExecToFile::PMFExecToFile( int RecoverBeginTime )
:FRecoverBeginTime( RecoverBeginTime )
,FConnection( NULL )
,FIsLogon( FALSE )
{
    if (gAppName.Length() > 0) FReportSeqNoFileName.Printf("%s_SeqNo", gAppName.c_str());
    UFC::AnsiString seqNoName = FReportSeqNoFileName;
    FReportSeqNo.Attach(seqNoName);
    
    Int32 maxFileSeqNo = PrepareExecIDSeqNoFile(RecoverBeginTime);
    if (maxFileSeqNo > 0)
    {
        UFC::AnsiString maxFileSeqNoStr = "";
        maxFileSeqNoStr.Printf("%08d", maxFileSeqNo);
        UFC::AnsiString curRptSeqPrefix = maxFileSeqNoStr.SubString(0, 3);
        UFC::AnsiString curRptSeqSuffix = maxFileSeqNoStr.SubString(3, 5);
        if ((FPreRptSeqPrefix.Length() <= 0) || (FPreRptSeqPrefix < curRptSeqPrefix))
            FPreRptSeqPrefix = curRptSeqPrefix;
        
        Int32 curRptSeqNo = GetReportSeqNo();
        Int32 maxRptSeqNo = curRptSeqSuffix.ToInt();
        if (curRptSeqNo < maxRptSeqNo) SetReportSeqNo(maxRptSeqNo);
    }
    
    UFC::BufferedLog::Printf( "______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   Execution to file utility. " );
    UFC::BufferedLog::Printf( "     ***For backend R6 version***        " );
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s %s", __DATE__, __TIME__ );
    UFC::BufferedLog::Printf( "   Recover Begin Time %d", RecoverBeginTime );
    if( gDumpOnly == TRUE )
        UFC::BufferedLog::Printf( "   Debug Mode:On");
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");
    UFC::BufferedLog::Printf( "   PATS_ORDER_LENGTH=%d  PATS_USERDATA_LENGTH=%d", PATS_ORDER_LENGTH, PATS_USERDATA_LENGTH );
    UFC::BufferedLog::Printf( "   RptSeqNo[%s][%d] [%s]", FPreRptSeqPrefix.c_str(), GetReportSeqNo(), FReportSeqNoFileName.c_str() );
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( " " );
    UFC::BufferedLog::FlushToFile();
    
    FDefExecFile = new TExecFile( gFUTFilePathName , gOPTFilePathName, gStockConfirmFilePathName, gStockFillFilePathName, gPATSConfirmFilePathName, gPATSFillFilePathName ); 
        
    ///< Create the connection object.
    FConnection = new TTaifexConnection( gAppName, this, atSpeedy, false );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    FConnection->AddSellSide(ssForeignExchange);
    ///< Load TAIFEX message format config files.
    FConnection->SetTAIFEXDataFormatResourceID( IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI" );
    FConnection->SetTAIFEXQuoteDataFormatResourceID( IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI" );                            
    ///< Load TWSE/OTC message format config files.
    FConnection->SetTSEDataFormatResourceID( IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI" );
    FConnection->SetOTCDataFormatResourceID( IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI" );
    FConnection->SetTSEDataFormatResourceID2( IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI" );
    FConnection->SetOTCDataFormatResourceID2( IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI" );    
}
//------------------------------------------------------------------------------
PMFExecToFile::~PMFExecToFile( void )
{
    ///< Delete TTaifexConnection object if exists.
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
TExecFile* PMFExecToFile::GetFileObj( const UFC::AnsiString& BrokerID )
{
    TExecFile* FileObj = FBrokerToFile.GetObjectByKey( BrokerID );
    if( FileObj == NULL)
        return FDefExecFile;
    return FileObj;
}
//------------------------------------------------------------------------------
void PMFExecToFile::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID:[%s], Password:[%s] )", gGatewayID.c_str(), gGatewayPassword.c_str() );
    FConnection->Logon( gGatewayID.c_str(), gGatewayPassword.c_str(), gGatewayAccount.c_str() );    
}
//------------------------------------------------------------------------------
void PMFExecToFile::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void PMFExecToFile::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
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
void PMFExecToFile::Recover( UFC::AnsiString BeginTime )
{
    if( FIsLogon == TRUE )
    {
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );
        FIsRecovering = true;
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void PMFExecToFile::OnRecoverFinished( int Count )
{
    FIsRecovering = false;
    UFC::BufferedLog::Printf( " %s( Count = %d )", __func__, Count );
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
void PMFExecToFile::CheckConnection( void )
{
    if( FIsLogon == FALSE )
        FConnection->Connect( gGatewayIP.c_str(), gGatewayPort, 5 );    
}
//------------------------------------------------------------------------------
void PMFExecToFile::GenUserDef( TExecutionReportMessage* Msg, UFC::AnsiString& UserDef )
{
    UFC::AnsiString AE( Msg->GetAE(), 3 );
    UFC::AnsiString TimeNow;
    Int32 SID = UFC::AnsiString( Msg->GetPVC() ).ToInt();
    
    if( Msg->GetOrderStatus() != nsOrderMessageDefine::osNew )
    {	
	UFC::AnsiString TempTime;
	UFC::GetTimeString( TempTime, FALSE );
	TimeNow = UFC::AnsiString( TempTime.c_str(), 8 );
    }
    else
        TimeNow = "00000000";    
    UserDef.Printf("4900000000000%sCCP00%s%s                000%03d", AE.c_str(), AE.c_str(), TimeNow.c_str(), SID % 1000 );
    UserDef.PadThis( 128, ' ' );
}    
//------------------------------------------------------------------------------
void PMFExecToFile::StockConfirmToFile( TExecutionReportMessage* Msg )
{
    nsOrderMessageDefine::MarketEnum msgMarket = Msg->GetMarket();
    nsOrderMessageDefine::ExecTypeEnum msgExecType = Msg->GetExecType();
    nsOrderMessageDefine::OrderStatusEnum msgOrderStatus = Msg->GetOrderStatus();

    UFC::AnsiString ExecString = "", Space33 = "", Space13 = "", User = "", AE = "", Seq = "";    
    Space33.PadThis( 33, ' ' );
    Space13.PadThis( 13, ' ' );
    Seq.Printf( "%014u", Msg->GetNID() );
    
    UFC::AnsiString msgBrokerID( Msg->GetBrokerID() );
    UFC::AnsiString msgAccount( Msg->GetAccount() );
    UFC::AnsiString userData = "", userInformation = " ";
    if( ( Msg->GetData() != NULL ) && ( strlen( Msg->GetData() ) > 0 ) )
    {
        userData = Msg->GetData();
        int userDataLen = userData.Length();
        int proxyDataStartPos = userData.AnsiPos( "<Proxy" );
        int proxyDataEndPos = userData.AnsiPos( ">" );
        if( ( proxyDataStartPos == 0 ) && ( proxyDataEndPos > proxyDataStartPos ) && ( userDataLen > proxyDataEndPos ) )
            userInformation = userData.SubString( proxyDataEndPos + 1, userDataLen - proxyDataEndPos - 1 );
        else
            userInformation = userData;
    }
    gGetAE( msgBrokerID, msgAccount, userInformation, AE );
    AE.PadThis( 6, ' ');
    
    ExecString.Printf( "3a%s%sd  %s00000000%s%s      55  ", Msg->GetSrc(), Space33.c_str(), AE.c_str(), Space13.c_str(), Seq.c_str() );
    GetFileObj( Msg->GetBrokerID() )->Write( msgMarket, msgExecType, msgOrderStatus, ExecString.c_str(), ExecString.Length() );
}
//------------------------------------------------------------------------------
void PMFExecToFile::ConfirmToFile( TExecutionReportMessage* Msg )
{
    nsOrderMessageDefine::MarketEnum msgMarket = Msg->GetMarket();
    nsOrderMessageDefine::ExecTypeEnum msgExecType = Msg->GetExecType();
    nsOrderMessageDefine::OrderStatusEnum msgOrderStatus = Msg->GetOrderStatus();
    UFC::AnsiString Exchange( Msg->GetSrc() );
    UFC::AnsiString PVC( Msg->GetPVC() );    
    if( PVC.Length() > 2 ) PVC.Delete( 0, PVC.Length() - 2 );
    if ( ( msgMarket != nsOrderMessageDefine::mForeignFutures ) && ( msgMarket != nsOrderMessageDefine::mForeignOptions ) )
    {
        Exchange[91] = PVC[0];
        Exchange[92] = PVC[1];
    }
    
    UFC::AnsiString userData = "", userInformation = " ";
    if( ( Msg->GetData() != NULL ) && ( strlen( Msg->GetData() ) > 0 ) )
    {
        userData = Msg->GetData();
        int userDataLen = userData.Length();
        int proxyDataStartPos = userData.AnsiPos( "<Proxy" );
        int proxyDataEndPos = userData.AnsiPos( ">" );
        if( ( proxyDataStartPos == 0 ) && ( proxyDataEndPos > proxyDataStartPos ) && ( userDataLen > proxyDataEndPos ) )
            userInformation = userData.SubString( proxyDataEndPos + 1, userDataLen - proxyDataEndPos - 1 );
        else
            userInformation = userData;
    }
    
    UFC::AnsiString Execution = "";
    if ( ( msgMarket != nsOrderMessageDefine::mForeignFutures ) && ( msgMarket != nsOrderMessageDefine::mForeignOptions ) )
    {
        if( userData.Length() <= 0 ) GenUserDef( Msg, userInformation );
        Execution.Printf( "%s%s", Exchange.c_str(), userInformation.c_str() );
    }
    else
    {
        UFC::AnsiString nameValueStr = GenerateOverseasRptSeqNameValueStr( Exchange );
        if( nameValueStr.Length() > 0 )
        {
            userInformation.PadThis( PATS_USERDATA_LENGTH, ' ' );
            UFC::AnsiString telegram = "";
            telegram.Printf("%s^%s", nameValueStr.c_str(), Exchange.c_str());  //2022/05/17 Set useq is first Field
            telegram.PadThis(PATS_ORDER_LENGTH, ' ');
//            Exchange = Exchange + nameValueStr;
//            Exchange.PadThis(PATS_ORDER_LENGTH, ' ');
            Int64 NID = Msg->GetNID();
            Execution.Printf( "%s%10lld%s", telegram.c_str(), NID, userInformation.c_str() );
            UFC::BufferedLog::Printf( " PMFExecToFile::ConfirmToFile() Qty[%d] cumQty[%d] leavesQty[%d]", Msg->GetOrderQty(), Msg->GetCumQty(), Msg->GetLeavesQty() );
        }
    }
        
    if (Execution.Length() > 0)
    {
        if ( ( msgMarket != nsOrderMessageDefine::mForeignFutures ) && ( msgMarket != nsOrderMessageDefine::mForeignOptions ) )
        {
            Execution.PadThis( 399, ' ' );
            UFC::AnsiString msgStatusCode( Msg->GetStatusCode() );
            if( msgStatusCode.Length() >= 1 ) 
                Execution[397] = msgStatusCode[0];
            else
                Execution[397] = '0';
            if( msgStatusCode.Length() >= 2 ) 
                Execution[398] = msgStatusCode[1];
            else
                Execution[398] = '0';
        }
        UFC::BufferedLog::Printf( " Confirm Exec:[%s] %d", Execution.c_str(), Execution.Length() );
        Execution += "\n";
        GetFileObj( Msg->GetBrokerID() )->Write( msgMarket, msgExecType, msgOrderStatus, Execution.c_str(), Execution.Length());
    }
}
//------------------------------------------------------------------------------
void PMFExecToFile::FillToFile( TExecutionReportMessage* Msg )
{
    nsOrderMessageDefine::MarketEnum msgMarket = Msg->GetMarket();
    nsOrderMessageDefine::ExecTypeEnum msgExecType = Msg->GetExecType();
    nsOrderMessageDefine::OrderStatusEnum msgOrderStatus = Msg->GetOrderStatus();
    UFC::AnsiString Exchange( Msg->GetSrc() );
    UFC::AnsiString Execution = "", UserData = "", Data = "";
    UFC::AnsiString PVC( Msg->GetPVC() );
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString ForGW;
    char            TIF;
    
    if( PVC.Length() > 2 ) PVC.Delete( 0, PVC.Length() - 2 );
    
    if ( ( msgMarket != nsOrderMessageDefine::mForeignFutures ) && ( msgMarket != nsOrderMessageDefine::mForeignOptions ) )
    {
        if( Exchange[0] == '1' ) 
        {
            Exchange[69] = PVC[0];
            Exchange[70] = PVC[1];
        }
        else if( Exchange[0] == '2' )
        {
            Exchange[117] = PVC[0];
            Exchange[118] = PVC[1];
            Exchange.SetLength( Exchange.Length() -1  );
        }
        else if( Exchange[0] == '3' || Execution[0] == '4' )
        {
            Exchange[65] = PVC[0];
            Exchange[66] = PVC[1];
        }
    }
    
    switch( Msg->GetTimeInForce() )
    {
        case nsOrderMessageDefine::tifIOC: TIF = 'I';break;
        case nsOrderMessageDefine::tifFOK: TIF = 'F';break;        
        case nsOrderMessageDefine::tifROD:
        default:                           TIF = 'R';break;
    }
    ForGW.Printf( "%c%04d%04d", TIF, Msg->GetBeforeQty(), Msg->GetLeavesQty() );
    
    if( ( Msg->GetData() != NULL ) && ( strlen( Msg->GetData() ) > 0 ) )
    {        
        UserData = Msg->GetData();
        int userDataLen = UserData.Length();
        int proxyDataStartPos = UserData.AnsiPos( "<Proxy" );
        int proxyDataEndPos = UserData.AnsiPos( ">" );
        if( ( proxyDataStartPos == 0 ) && ( proxyDataEndPos > proxyDataStartPos ) && ( userDataLen > proxyDataEndPos ) )
        {
            Data = UserData.SubString( proxyDataEndPos + 1, userDataLen - proxyDataEndPos - 1 );
            UserData = Data;
        }

        if ( ( msgMarket != nsOrderMessageDefine::mForeignFutures ) && ( msgMarket != nsOrderMessageDefine::mForeignOptions ) )
            Execution.Printf( "%s%s%s", Exchange.c_str(), ForGW.c_str(), UserData.c_str() );
        else
        {
            UserData.PadThis( PATS_USERDATA_LENGTH, ' ' );
            UFC::BufferedLog::Printf( " UserData:[%s]", UserData.c_str() );
            UFC::AnsiString nameValueStr = GenerateOverseasRptSeqNameValueStr( Exchange );
            if (nameValueStr.Length() > 0) 
            {
                UFC::AnsiString telegram = "";
                telegram.Printf("%s^%s", nameValueStr.c_str(), Exchange.c_str());  //2022/05/17 Set useq is first Field
                telegram.PadThis(PATS_ORDER_LENGTH, ' ');
//                Exchange = Exchange + nameValueStr;
//                Exchange.PadThis(PATS_ORDER_LENGTH, ' ');
                Int64 NID = Msg->GetNID();
                Execution.Printf( "%s%10lld%s", telegram.c_str(), NID, UserData.c_str() );
            }  //if (nameValueStr.Length() > 0)
        }
    }
    else
    {
        if ( ( msgMarket == nsOrderMessageDefine::mForeignFutures ) || ( msgMarket == nsOrderMessageDefine::mForeignOptions ) )
        {
            UFC::AnsiString nameValueStr = GenerateOverseasRptSeqNameValueStr(Exchange);
            if (nameValueStr.Length() > 0) 
            {
                UFC::AnsiString telegram = "";
                telegram.Printf("%s^%s", nameValueStr.c_str(), Exchange.c_str());  //2022/05/17 Set useq is first Field
                telegram.PadThis(PATS_ORDER_LENGTH, ' ');
//                Exchange = Exchange + nameValueStr;
//                Exchange.PadThis(PATS_ORDER_LENGTH, ' ');
                Int64 NID = Msg->GetNID();
                UserData = " ";
                UserData.PadThis(PATS_USERDATA_LENGTH, ' ');
                Execution.Printf( "%s%10lld%s", telegram.c_str(), NID, UserData.c_str() );
            }
        }
        else
        {
	    GenUserDef( Msg, UserData );
	    Execution.Printf( "%s%s%s", Exchange.c_str(), ForGW.c_str(), UserData.c_str() );
        }
    }
    
    if (Execution.Length() > 0)
    {
        if ( ( msgMarket != nsOrderMessageDefine::mForeignFutures ) && ( msgMarket != nsOrderMessageDefine::mForeignOptions ) )
        {
            Execution.PadThis( 399, ' ' );
            UFC::AnsiString msgStatusCode( Msg->GetStatusCode() );
            if( msgStatusCode.Length() >= 1 ) 
                Execution[397] = msgStatusCode[0];
            else
                Execution[397] = '0';
                
            if( msgStatusCode.Length() >= 2 ) 
                Execution[398] = msgStatusCode[1];
            else
                Execution[398] = '0';
        }
        UFC::BufferedLog::Printf( " Fill Exec:[%s] %d", Execution.c_str(), Execution.Length() ); 
        Execution += "\n";
        GetFileObj( Msg->GetBrokerID() )->Write( msgMarket, msgExecType, msgOrderStatus, Execution.c_str(), Execution.Length());
    }
}
//------------------------------------------------------------------------------
bool PMFExecToFile::IsStockExec( nsOrderMessageDefine::MarketEnum market )
{
    if( ( market == nsOrderMessageDefine::mOTC ) || ( market == nsOrderMessageDefine::mTSE ) )
        return true;
    return false;
}
//------------------------------------------------------------------------------
void PMFExecToFile::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
    nsOrderMessageDefine::MarketEnum market = Msg->GetMarket();
    nsOrderMessageDefine::ExecTypeEnum execType = Msg->GetExecType();
    nsOrderMessageDefine::OrderStatusEnum orderStatus = Msg->GetOrderStatus();
    nsOrderMessageDefine::CxlRejResponseToEnum cxlRejResponseTo = Msg->GetCxlRejResponseTo();
    
    switch( orderStatus )
    {
        case nsOrderMessageDefine::osNew:
            if( IsStockExec( market ) == false  )
                ConfirmToFile( Msg );
            else
                StockConfirmToFile( Msg );
            break;
        case nsOrderMessageDefine::osRejected:
            if( IsStockExec( market ) == false  )
            {                
                if( !strcmp( Msg->GetStatusCode(), "32" ) )
                    FillToFile( Msg );
                else
                    ConfirmToFile( Msg );
            }
            else
                StockConfirmToFile( Msg );
            break;
        case nsOrderMessageDefine::osReplaced:
            if( IsStockExec( market ) == false  )
            {                
                if( ( market == nsOrderMessageDefine::mTWFutures ) || ( market == nsOrderMessageDefine::mTWOptions ) )
                {
                    if( ( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ) || ( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 ) )
                        ConfirmToFile( Msg );
                    else
                        FillToFile( Msg );
                }
                else
                    ConfirmToFile( Msg );
            }
            else
                StockConfirmToFile( Msg );
            break;
        case nsOrderMessageDefine::osCanceled:
            if( IsStockExec( market ) == false  )
                FillToFile( Msg );
            else
                StockConfirmToFile( Msg );
            break;
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:
            if( IsStockExec( market ) == false  )
            {
                if( ( execType == nsOrderMessageDefine::etPartiallyFilled ) ||
                    ( execType == nsOrderMessageDefine::etFilled ) )
                    FillToFile( Msg );
                else if( execType == nsOrderMessageDefine::etReplaced )
                    ConfirmToFile( Msg );
                else if ( execType == nsOrderMessageDefine::etRejected )
                {
                    if( ( cxlRejResponseTo == nsOrderMessageDefine::crrCancel ) || 
                        ( cxlRejResponseTo == nsOrderMessageDefine::crrReplace ) )
                        ConfirmToFile( Msg );
                }
            }
            break;
        case nsOrderMessageDefine::osExpired:
            if( IsStockExec( market ) == false  )
                FillToFile( Msg );
            else
                StockConfirmToFile( Msg );
            break;
        case nsOrderMessageDefine::osPendingNew:
        case nsOrderMessageDefine::osPendingReplace:
        case nsOrderMessageDefine::osPendingCancel:
            break;
        default: break;
    }  //switch( orderStatus )
}
//------------------------------------------------------------------------------
