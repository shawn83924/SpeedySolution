#include "GCExecConnection.h"
#include "../../UFC/NetUtility.h"
#include <math.h>
//------------------------------------------------------------------------------
extern BOOL            DumpOnly;
bool                   FilterIOCFOKExec = false;
bool                   FilterIOCFOKCxlExecOnly = false;
extern MessageObject*  MessageObj;
extern UFC::PInt32     FRecoverBeginTime;

//------------------------------------------------------------------------------
bool IsDigitalChar(char OrigChar)
{
    if ((OrigChar >= '0') && (OrigChar <= '9')) 
        return true;
    else
        return false;
}  //IsDigitalChar()
//------------------------------------------------------------------------------
bool IsMonthStr(const UFC::AnsiString& OrigStr)
{
    UFC::AnsiString upperStr = OrigStr.UpperCase();
    upperStr.TrimLeft();
    upperStr.TrimRight();
    if (OrigStr.Length() != 3) return false;
    if ((upperStr == "JAN") || (upperStr == "FEB") || (upperStr == "MAR") || (upperStr == "APR") ||
        (upperStr == "MAY") || (upperStr == "JUN") || (upperStr == "JUL") || (upperStr == "AUG") ||
        (upperStr == "SEP") || (upperStr == "OCT") || (upperStr == "NOV") || (upperStr == "DEC"))
        return true;
    else
        return false;
}  //IsMonthStr()
//------------------------------------------------------------------------------
UFC::AnsiString DescribeOrderStatus(nsOrderMessageDefine::OrderStatusEnum OrderStatus)
{
    UFC::AnsiString orderStatusStr = "";
    switch (OrderStatus)
    {
        case nsOrderMessageDefine::osNone:               orderStatusStr = "None";               break;  //  0
        case nsOrderMessageDefine::osPendingNew:         orderStatusStr = "PendingNew";         break;  //  1
        case nsOrderMessageDefine::osNew:                orderStatusStr = "New";                break;  //  2
        case nsOrderMessageDefine::osPendingReplace:     orderStatusStr = "PendingReplace";     break;  //  3
        case nsOrderMessageDefine::osReplaced:           orderStatusStr = "Replaced";           break;  //  4
        case nsOrderMessageDefine::osPendingCancel:      orderStatusStr = "PendingCancel";      break;  //  5
        case nsOrderMessageDefine::osCanceled:           orderStatusStr = "Canceled";           break;  //  6
        case nsOrderMessageDefine::osPartiallyFilled:    orderStatusStr = "PartiallyFilled";    break;  //  7
        case nsOrderMessageDefine::osFilled:             orderStatusStr = "Filled";             break;  //  8
        case nsOrderMessageDefine::osQuoteAccept:        orderStatusStr = "QuoteAccept";        break;  //  9
        case nsOrderMessageDefine::osExpired:            orderStatusStr = "Expired";            break;  // 10
        case nsOrderMessageDefine::osRejected:           orderStatusStr = "Rejected";           break;  // 11
        case nsOrderMessageDefine::osQuoteRequestAccept: orderStatusStr = "QuoteRequestAccept"; break;  // 12
        default:                                         orderStatusStr = "N/A"; 
    }  //switch(OrderStatus)
    return orderStatusStr;
}  //DescribeOrderStatus()
//------------------------------------------------------------------------------
UFC::AnsiString DescribeExecType(nsOrderMessageDefine::ExecTypeEnum ExecType)
{
    UFC::AnsiString execTypeStr = "";
    switch (ExecType)
    {
        case nsOrderMessageDefine::etNone:               execTypeStr = "None";               break;  //  0
        case nsOrderMessageDefine::etPendingNew:         execTypeStr = "PendingNew";         break;  //  1
        case nsOrderMessageDefine::etNew:                execTypeStr = "New";                break;  //  2
        case nsOrderMessageDefine::etPendingReplace:     execTypeStr = "PendingReplace";     break;  //  3
        case nsOrderMessageDefine::etReplaced:           execTypeStr = "Replaced";           break;  //  4
        case nsOrderMessageDefine::etPendingCancel:      execTypeStr = "PendingCancel";      break;  //  5
        case nsOrderMessageDefine::etCanceled:           execTypeStr = "Canceled";           break;  //  6
        case nsOrderMessageDefine::etPartiallyFilled:    execTypeStr = "PartiallyFilled";    break;  //  7
        case nsOrderMessageDefine::etFilled:             execTypeStr = "Filled";             break;  //  8
        case nsOrderMessageDefine::etQuoteAccept:        execTypeStr = "QuoteAccept";        break;  //  9
        case nsOrderMessageDefine::etExpired:            execTypeStr = "Expired";            break;  // 10
        case nsOrderMessageDefine::etRejected:           execTypeStr = "Rejected";           break;  // 11
        case nsOrderMessageDefine::etOrderStatus:        execTypeStr = "OrderStatus";        break;  // 12
        case nsOrderMessageDefine::etQuoteRequestAccept: execTypeStr = "QuoteRequestAccept"; break;  // 13
        default:                                         execTypeStr = "N/A"; 
    }  //switch(ExecType)
    return execTypeStr;
}  //DescribeExecType()
//------------------------------ CPatsDateObject -------------------------------
UFC::AnsiString DescribeSide(nsOrderMessageDefine::SideEnum Side)
{
    UFC::AnsiString sideStr = "";
    switch (Side)
    {
        case nsOrderMessageDefine::sNone: sideStr = "None"; break;  //  0
        case nsOrderMessageDefine::sBuy:  sideStr = "Buy";  break;  //  1
        case nsOrderMessageDefine::sSell: sideStr = "Sell"; break;  //  2
        default:                          sideStr = "N/A"; 
    }  //switch(Side)
    return sideStr;
}  //DescribeSide()
//------------------------------ CPatsDateObject -------------------------------
int CPatsDateObject::GetDigitalStr(const UFC::AnsiString& OrigStr, int BeginIndex, int MaxLength, UFC::AnsiString& DigitalStr)
{
    DigitalStr = "";
    int startIndex = BeginIndex;
    int lastIndex = OrigStr.Length() - 1;

    //找尋數字字元起始位置
    while ((startIndex <= lastIndex) && !IsDigitalChar(OrigStr[startIndex]))
        startIndex++;
    if (startIndex > lastIndex) return lastIndex;  //沒找到數字字元
    
    int curIndex = startIndex;
    char digitalArray[MaxLength + 1];
    int digitalIndex = 0;
    bool isDigitalStrExist = false;
    while (curIndex <= lastIndex)
    {
        char tempChar = OrigStr[curIndex];
        if (IsDigitalChar(tempChar))
        {
            digitalArray[digitalIndex++] = tempChar;
            isDigitalStrExist = true;
            if (digitalIndex >= MaxLength) break;  //避免超過最大長度限制
        }
        else
            break;
        curIndex++;
    }  //while (curIndex <= lastIndex)
    if (curIndex > lastIndex) curIndex = lastIndex;

    digitalArray[MaxLength] = 0;
    if (isDigitalStrExist) 
    {
        if (digitalIndex <= MaxLength) digitalArray[digitalIndex] = 0;
        DigitalStr = digitalArray;
    }
    return curIndex;  //傳回最後搜尋到的位置
}  //CPatsDateObject::GetDigitalStr()
//------------------------------ CPatsDateObject -------------------------------
bool CPatsDateObject::ParsePatsDateStr(const UFC::AnsiString& PatsDateStr, UFC::AnsiString& MaturityMonthStr, UFC::AnsiString& MaturityYearStr, UFC::AnsiString& StrikePriceStr, char& CallPutChar)
{   
    if (PatsDateStr.Length() < 3) return false;
    
    MaturityMonthStr = "";
    MaturityYearStr = "";
    StrikePriceStr = ""; 
    CallPutChar = 'N';
    
    UFC::AnsiString upperStr = PatsDateStr.UpperCase();
    upperStr.TrimLeft();
    upperStr.TrimRight();
    int upperStrLength = upperStr.Length();
    int beginIndex = 0;
    int lastIndex = upperStrLength - 1;
    
    //取得月份字串
    UFC::AnsiString monthStr = "";
    bool isMonthStrExist = false;
    while (beginIndex <= (upperStrLength - 3))
    {
        monthStr = upperStr.SubString(beginIndex, 3);
        isMonthStrExist = IsMonthStr(monthStr);
        if (isMonthStrExist)
        {
            MaturityMonthStr = monthStr;
            beginIndex += monthStr.Length();
            break;
        }
        else
            beginIndex++;
    }  //while (beginIndex <= (upperStrLength - 3))
    if (!isMonthStrExist) return false;  //無月份字串
    if (beginIndex > lastIndex) return false;  //剩餘長度不足

    //取得年份字串（最大長度 2）
    int maturityYearLastIndex = GetDigitalStr(upperStr, beginIndex, 2, MaturityYearStr);
    if (MaturityYearStr.Length() <= 0) return false;  //無年份字串
    beginIndex = maturityYearLastIndex + 1;
    if (beginIndex > lastIndex) return true;  //剩餘長度不足(期貨)
    
    //取得履約價字串（最大長度 10）    
    int strikePriceLastIndex = GetDigitalStr(upperStr, beginIndex, 10, StrikePriceStr);
    if (StrikePriceStr.Length() <= 0) return true;  //無履約價字串(期貨)
    beginIndex = strikePriceLastIndex + 1;
    if (beginIndex > lastIndex) return false;  //剩餘長度不足(有履約價字串，沒有 Call/Put 字元)
    
    while (beginIndex <= lastIndex)
    {
        char tempChar = upperStr[beginIndex];
        if ((tempChar == 'C') || (tempChar == 'P'))
        {
            CallPutChar = tempChar;
            return true;  //完整選擇權字串
        }    
        beginIndex++;
    }  //while (beginIndex <= lastIndex)
    return false;  //有履約價字串，沒有 Call/Put 字元
}  //CPatsDateObject::ParsePatsDateStr()
//------------------------------------------------------------------------------
void CPatsDateObject::DupmData()
{
    UFC::AnsiString dumpStr = "";
    if (FMaturityMonthStr.Length() > 0) dumpStr = FMaturityMonthStr;
    if (FMaturityYearStr.Length() > 0) dumpStr += FMaturityYearStr;
    if (FStrikePriceStr.Length() > 0) dumpStr = dumpStr + " " + FStrikePriceStr;
    UFC::AnsiString validStr = "wrong";
    if (FIsCorrectDateStr) validStr = "valid";
    if (dumpStr.Length() > 0)
        UFC::BufferedLog::Printf(" PatsDate = [%s %c] %s", dumpStr.c_str(), FCallPutChar, validStr.c_str());
    else
        UFC::BufferedLog::Printf(" PatsDate = [%s] %s", FPatsDateStr.c_str(), validStr.c_str());
}  //CPatsDateObject::DupmData()
//------------------------------------------------------------------------------
CPatsDateObject::CPatsDateObject(const UFC::AnsiString& PatsDateStr)
:FPatsDateStr(PatsDateStr),
 FMaturityMonthStr(""),
 FMaturityYearStr(""),
 FMaturityMonthYearStr(""),
 FStrikePriceStr(""),
 FCallPutChar('N'),
 FIsCorrectDateStr(false)
{
    FIsCorrectDateStr = ParsePatsDateStr(FPatsDateStr, FMaturityMonthStr, FMaturityYearStr, FStrikePriceStr, FCallPutChar);
    FMaturityMonthYearStr = FMaturityMonthStr + FMaturityYearStr;
}  //CPatsDateObject::CPatsDateObject()
//------------------------------------------------------------------------------
int ClearPatsContractDateArray(std::vector<CPatsDateObject*>& DateObjArray)
{
    int deleteCount = 0;
    if (DateObjArray.size() > 0)
    {
        for (unsigned int i = 0; i < DateObjArray.size(); i++)
        {
            CPatsDateObject *curPatsDateObjPtr = DateObjArray[i];
            if (curPatsDateObjPtr != 0) 
            {
                delete curPatsDateObjPtr;
                deleteCount++;
            }
        }
    }
    DateObjArray.clear();
    return deleteCount;
}  //ClearPatsContractDateArray()
//------------------------------------------------------------------------------
int ParsePatsContractDate(const UFC::AnsiString& PatsContractDate, std::vector<CPatsDateObject*>& DateObjArray)
{
    UFC::AnsiString upperPatsContractDate = PatsContractDate.UpperCase();
    int patsContractDateLength = upperPatsContractDate.Length();
    if (patsContractDateLength < 3) return 0;
    
    int contractDateCount = 0;
    int beginIndex = 0;
    int patsContractDateLastIndex = patsContractDateLength - 1;
    std::vector<int> beginIndexArray;  //月份字串起始位置
    std::vector<int> lastIndexArray;   //月份字串結束位置    
    while (beginIndex <= (patsContractDateLength - 3))
    {
        UFC::AnsiString monthStr = upperPatsContractDate.SubString(beginIndex, 3);
        if (IsMonthStr(monthStr))
        {
            if ((beginIndex > 0) && (beginIndexArray.size() > 0) && (lastIndexArray.size() < beginIndexArray.size()))
                lastIndexArray.push_back(beginIndex - 1);
            beginIndexArray.push_back(beginIndex);
            beginIndex += 3;
        }  //if (IsMonthStr(monthStr))
        else
            beginIndex++;
    }  //while (beginIndex <= (patsContractDateLength - 3))    
    if (beginIndexArray.size() <= 0) return contractDateCount;
    if (lastIndexArray.size() < beginIndexArray.size())
        lastIndexArray.push_back(patsContractDateLastIndex);
    
    if (lastIndexArray.size() != beginIndexArray.size())
    {
        UFC::BufferedLog::Printf( " ParsePatsContractDate() Parse [%s] error. Begin Index Array Size=%d, Last Index Array Size=%d", PatsContractDate.c_str(), beginIndexArray.size(), lastIndexArray.size());
        return contractDateCount;
    }

    for (unsigned int i = 0; i < beginIndexArray.size(); i++)
    {
        beginIndex = beginIndexArray[i];
        int lastIndex = lastIndexArray[i];
        if ((beginIndex < 0) || (lastIndex < beginIndex) || (beginIndex > patsContractDateLastIndex) || (lastIndex > patsContractDateLastIndex))
        {
            UFC::BufferedLog::Printf( " ParsePatsContractDate() Parse [%s] error. Array Index=%d, Begin Index=%d, Last Index=%d", PatsContractDate.c_str(), i, beginIndex, lastIndex);
            return contractDateCount;
        }    
        int singleDateLength = lastIndex - beginIndex + 1;
        UFC::AnsiString singleDateStr = upperPatsContractDate.SubString(beginIndex, singleDateLength);
        singleDateStr.TrimRight();
        CPatsDateObject *patsDateObjPtr = new CPatsDateObject(singleDateStr);
        DateObjArray.push_back(patsDateObjPtr);
        UFC::BufferedLog::Printf( " ParsePatsContractDate() %d[%s]", i, singleDateStr.c_str());
        contractDateCount++;
    }  //for (int i = 0; i < beginIndexArray.size(); i++)        
    return contractDateCount;
}  //ParsePatsContractDate()

//------------------------ CPATSConfirmExecutionReport -------------------------
//------------------------------------------------------------------------------
CPATSConfirmExecutionReport::CPATSConfirmExecutionReport(const UFC::AnsiString& OrderId, const UFC::AnsiString& PATSContractDate, PATSExec *ExecRptDataPtr)
:FOrderId(OrderId),
 FPATSContractDate(PATSContractDate),
 FLeg1Sended(false),
 FLeg2Sended(false)
{
    if (ExecRptDataPtr == 0)
        memset(&FExecRptData, ' ', sizeof(PATSExec));
    else
        memcpy(&FExecRptData, ExecRptDataPtr, sizeof(PATSExec));
}  //CPATSConfirmExecutionReport::CPATSConfirmExecutionReport()

//------------------------------ GCExecConnection ------------------------------
//------------------------------------------------------------------------------
UFC::PInt32 GCExecConnection::FSequence;
//------------------------------------------------------------------------------
GCExecConnection::GCExecConnection( int RecoverBeginTime )
:UFC::PThread( NULL, FALSE )
,FAppName( "KGIExecConnection" )
,FIP( "127.0.0.1" )
,FPort( 23456 )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "0000001" )
,FBrokerID( "F021999" )
,FOrderIDPrefix( 'A' )
,FWEBID( "000" )
,FIsKGI( true )
,FIsBrokerModeEnabled( false )
,FConfirmSubject( "Confirm" )
,FConfirmKey( "To400" )
,FConfirmRODKey("To400Front")
,FFillSubject( "Fill" )
,FFillKey( "To400" )
,FFillRODKey("To400Front")
,FTSEConfirmSubject( "Confirm" )
,FTSEConfirmKey( "To400" )
,FOTCConfirmSubject( "Confirm" )
,FOTCConfirmKey( "To400" )
,FTSEFillSubject( "Fill" )
,FTSEFillKey( "To400" )
,FOTCFillSubject( "Fill" )
,FOTCFillKey( "To400" )
,FConnection( NULL )
,FIsLogon( FALSE )
{
    UFC::AnsiString ShmName( "KGI_PATS_RPTSEQ" );
    UFC::AnsiString LogName = "../log/KGIExec" + UFC::GetDateString() + ".log";
    LoadSetting(  );        
    ///< Create the connection object.    
    UFC::BufferedLog::SetDebugMode( TRUE );
    FConnection = new TTaifexConnection( NULL, &FResource, FAppName, this, atSpeedy,TRUE );

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
    FConnection->AddSellSide( ssForeignExchange );
    
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "  KGI backend interface module           " );
    UFC::BufferedLog::Printf( "  startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "  Build Date:%s %s", __DATE__, __TIME__ );
    if( FIsKGI == true )
        UFC::BufferedLog::Printf( "  Use KGI format.             " );
    else
        UFC::BufferedLog::Printf( "  Use Grand Cathay format.             " );
    UFC::BufferedLog::Printf( "                                          ");
    if( FilterIOCFOKExec == true )
    {
        if( FilterIOCFOKCxlExecOnly == true )
            UFC::BufferedLog::Printf( "  Filter IOC/FOK Canceled executions.");    
        else
            UFC::BufferedLog::Printf( "  Filter IOC/FOK Confirm/Cancel executions.");    
    }
    else
        UFC::BufferedLog::Printf( "  Filter IOC/FOK executions: off.");               
    UFC::BufferedLog::Printf( "  OrderID Prefix:%c", FOrderIDPrefix );    
    UFC::BufferedLog::Printf( "  Gateway: Address[%s:%d] ID[%s] Account[%s] Broker[%s] WebID[%s]", FIP.c_str(), FPort, FID.c_str(), FAccount.c_str(), FBrokerID.c_str(), FWEBID.c_str() );
    UFC::BufferedLog::Printf( "  Confirm: Subject[%s] Key[%s] RODKey[%s], Fill Subject[%s] Key[%s] RODKey[%s]", FConfirmSubject.c_str(), FConfirmKey.c_str(), FConfirmRODKey.c_str(), FFillSubject.c_str(), FFillKey.c_str(), FFillRODKey.c_str() );
    UFC::BufferedLog::Printf( "  TSE Confirm: Subject[%s] Key[%s], Fill Subject[%s] Key[%s]", FTSEConfirmSubject.c_str(), FTSEConfirmKey.c_str(), FTSEFillSubject.c_str(), FTSEFillKey.c_str() );
    UFC::BufferedLog::Printf( "  OTC Confirm: Subject[%s] Key[%s], Fill Subject[%s] Key[%s]", FOTCConfirmSubject.c_str(), FOTCConfirmKey.c_str(), FOTCFillSubject.c_str(), FOTCFillKey.c_str() );
    if( FIsBrokerModeEnabled )
        UFC::BufferedLog::Printf( "   Broker Mode:On");
    else
        UFC::BufferedLog::Printf( "   Broker Mode:Off");
    for (int curIndex = 0;  curIndex < FSymbolConvertTable.ItemCount(); curIndex++)
    {
        UFC::AnsiString curPatsSymbol, curTWPatsSymbol;
        if (FSymbolConvertTable.GetItem(curIndex, curPatsSymbol, curTWPatsSymbol) == TRUE)
            UFC::BufferedLog::Printf(" %d Convert PatSymbol[%s] to [%s]", curIndex, curPatsSymbol.c_str(), curTWPatsSymbol.c_str());
    }
    for (int curAEIndex = 0; curAEIndex < FAETable.ItemCount(); curAEIndex++)
    {
        UFC::AnsiString curAEKey;
        AEInfo *aeInfoPtr;
        if (FAETable.GetItem(curAEIndex, curAEKey, aeInfoPtr) == TRUE)
            UFC::BufferedLog::Printf(" %d Convert [%s] to AE[%s] WebID[%s]", curAEIndex, aeInfoPtr->AE.c_str(), aeInfoPtr->WebID.c_str());
    }
    //FCommodityMultiplierTable
    for (int curIndex = 0;  curIndex < FCommodityMultiplierTable.ItemCount(); curIndex++)
    {
        UFC::AnsiString curContract;
        double curMultiplier;
        if (FCommodityMultiplierTable.GetItem(curIndex, curContract, curMultiplier) == TRUE)
            UFC::BufferedLog::Printf(" %d Contract[%s] Multiplier[%20.6lf]", curIndex, curContract.c_str(), curMultiplier);
    }
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "  Debug Mode:On (Dump to stdout only)");    
    else
        UFC::BufferedLog::Printf( "  Debug Mode:Off");    
    UFC::BufferedLog::Printf( "__________________________________________" );           
    GCExecConnection::FSequence.Attach( ShmName );    
    
    ///< Start running thread.
    Start();
}
//------------------------------------------------------------------------------
void GCExecConnection::LoadSetting( void )
{
    UFC::AnsiString logHead = "LoadSetting()";
    UFC::AnsiString configFileName = "../cfg/KGIExecConnection.cfg";
    UFC::UiniFile *configFilePtr = 0;
    try
    {
        configFilePtr = new UFC::UiniFile( configFileName, FALSE );  //Do not Create
    }
    catch(UFC::FileException e)
    {
        UFC::BufferedLog::Printf(" %s Process [%s] Failed, FileException:%s.", logHead.c_str(), configFileName.c_str(), e.what());
        UFC::BufferedLog::FlushToFile();
        exit(1);
    }
    
    bool isFieldMissed = false;
    UFC::AnsiString sectionName = "Setting";
    UFC::AnsiString fieldName = "IP";
    UFC::AnsiString tempValue = "";
    if( configFilePtr->GetValue( sectionName, fieldName, FIP ) == FALSE ) isFieldMissed = true;
    
    if( !isFieldMissed )
    {
        fieldName = "ID";
        if( configFilePtr->GetValue( sectionName, fieldName, FID ) == FALSE ) isFieldMissed = true;
    }
        
    if( !isFieldMissed )
    {
        fieldName = "Password";
        if( configFilePtr->GetValue( sectionName, fieldName, FPassword ) == FALSE ) isFieldMissed = true;
    }
        
    if( !isFieldMissed )
    {
        fieldName = "Account";
        if( configFilePtr->GetValue( sectionName, fieldName, FAccount ) == FALSE ) isFieldMissed = true;
    }
    
    if( !isFieldMissed )
    {
        fieldName = "BrokerID";
        if( configFilePtr->GetValue( sectionName, fieldName, FBrokerID ) == FALSE ) isFieldMissed = true;
    }
    
    if( !isFieldMissed )
    {
        fieldName = "WebID";
        if( configFilePtr->GetValue( sectionName, fieldName, FWEBID ) == TRUE )
            FWEBID.PadThis( 3, ' ' ); 
        else
            isFieldMissed = true;
    }
                
    if( !isFieldMissed )
    {
        fieldName = "Format";
        if( configFilePtr->GetValue( sectionName, fieldName, tempValue ) == TRUE )
        {
            tempValue.UpperThis();
            if( tempValue == "KGI")
                FIsKGI = true;
            else if( tempValue == "GC")
                FIsKGI = false;
            else
            {
                FIsKGI = true;
                UFC::BufferedLog::Printf( " %s Wrong [%s]%s Value[%s], Use default value true.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), tempValue.c_str());
            }
        }
        else
            UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value true.", logHead.c_str(), sectionName.c_str(), fieldName.c_str());
        
        fieldName = "FilterIOCFOK";
        if( configFilePtr->GetValue( sectionName, fieldName, tempValue ) == TRUE )
        {
            tempValue.UpperThis();
            if( tempValue == "CANCELED")
            {
                FilterIOCFOKExec = true;
                FilterIOCFOKCxlExecOnly = true;
            }
            else if( tempValue == "ALL")
            {
                FilterIOCFOKExec = true;
                FilterIOCFOKCxlExecOnly = false;
            }
            else
            {
                FilterIOCFOKExec = false;
                FilterIOCFOKCxlExecOnly = false;
                UFC::BufferedLog::Printf( " %s Wrong [%s]%s Value[%s], Use default value: FilterIOCFOKExec = false, FilterIOCFOKCxlExecOnly = false.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), tempValue.c_str());
            }
        }
        else
        {
            FilterIOCFOKExec = false;
            FilterIOCFOKCxlExecOnly = false;
            UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: FilterIOCFOKExec = false, FilterIOCFOKCxlExecOnly = false.", logHead.c_str(), sectionName.c_str(), fieldName.c_str());
        }

        fieldName = "OrderIDPrefix";
        if( configFilePtr->GetValue( sectionName, fieldName, tempValue ) == TRUE )
        {
            if (tempValue.Length() > 0) 
                FOrderIDPrefix = tempValue[0];
            else
                UFC::BufferedLog::Printf( " %s Wrong[%s]%s, Use default value: '%c'.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FOrderIDPrefix);
        }
        else
            UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: '%c'.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FOrderIDPrefix);
        
        sectionName = "PATSSymbol";
        UFC::Section *patsSymbolSectionPtr = configFilePtr->GetSection(sectionName);
        if (patsSymbolSectionPtr != 0)
        {
            UFC::AnsiString patsSymbol, twSymbol;
            for (register int i = 0; i <= patsSymbolSectionPtr->ItemCount(); i++)
            {
                if (patsSymbolSectionPtr->GetNameValue(i, patsSymbol, twSymbol) == TRUE)
                {
                    FSymbolConvertTable.Add(patsSymbol, twSymbol);
                    UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " %s Converted PatSymbol[%s] to [%s]", logHead.c_str(), patsSymbol.c_str(), twSymbol.c_str());
                }
            }  //for (register int i = 0; i <= patsSymbolSectionPtr->ItemCount(); i++)
        }
        else
            UFC::BufferedLog::Printf( " %s Section[%s] not Exists.", logHead.c_str(), sectionName.c_str());
        
        sectionName = "CommodityMultiplier";
        UFC::Section *multiplierSectionPtr = configFilePtr->GetSection(sectionName);
        if (multiplierSectionPtr != 0)
        {
            for (register int i = 0; i <= multiplierSectionPtr->ItemCount(); i++)
            {
                UFC::AnsiString exchangeContractCode, multiplierStr;
                if (multiplierSectionPtr->GetNameValue(i, exchangeContractCode, multiplierStr) == TRUE)
                {
                    double multiplier = multiplierStr.ToDouble();
                    FCommodityMultiplierTable.Add(exchangeContractCode, multiplier);
                    UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " %s Contract[%s] multiplier[%20.6lf]", logHead.c_str(), exchangeContractCode.c_str(), multiplier);
                }
            }  //for (register int i = 0; i <= multiplierSectionPtr->ItemCount(); i++)
        }
        else
            UFC::BufferedLog::Printf( " %s Section[%s] not Exists.", logHead.c_str(), sectionName.c_str());

        sectionName = "BackEnd";
        UFC::Section *backendSectionPtr = configFilePtr->GetSection(sectionName);
        if (backendSectionPtr != 0)
        {
            sectionName = "EnableBrokerMode";
            if (backendSectionPtr->GetValue(sectionName, tempValue) == TRUE)
            {
                if ((tempValue == "1") || (tempValue == "Y") || (tempValue == "y"))
                    FIsBrokerModeEnabled = true; 
                else
                    FIsBrokerModeEnabled = false; 
            }
            else
                UFC::BufferedLog::Printf( " %s [%s]%s Missed, Use default value: false.", logHead.c_str(), sectionName.c_str(), fieldName.c_str());
            
            fieldName = "ConfirmSubject";
            if( backendSectionPtr->GetValue( fieldName, FConfirmSubject ) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FConfirmSubject.c_str());
            
            fieldName = "ConfirmKey";
            if( backendSectionPtr->GetValue( fieldName, FConfirmKey ) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FConfirmKey.c_str());
            
            fieldName = "ConfirmRODKey";
            if( backendSectionPtr->GetValue( fieldName, FConfirmRODKey ) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FConfirmRODKey.c_str());
            
            fieldName = "FillSubject";
            if( backendSectionPtr->GetValue( fieldName, FFillSubject ) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FFillSubject.c_str());
            
            fieldName = "FillKey";
            if( backendSectionPtr->GetValue( fieldName, FFillKey ) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FFillKey.c_str());
            
            fieldName = "FillRODKey";
            if( backendSectionPtr->GetValue( fieldName, FFillRODKey ) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FFillRODKey.c_str());
            
            fieldName = "TSEConfirmSubject";
            if( backendSectionPtr->GetValue( fieldName, FTSEConfirmSubject ) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FTSEConfirmSubject.c_str());
            
            fieldName = "TSEConfirmKey";
            if( backendSectionPtr->GetValue( fieldName, FTSEConfirmKey ) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FTSEConfirmKey.c_str());
                        
            fieldName = "OTCConfirmSubject";
            if( backendSectionPtr->GetValue( fieldName, FOTCConfirmSubject) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FOTCConfirmSubject.c_str());
            
            fieldName = "OTCConfirmKey";
            if( backendSectionPtr->GetValue( fieldName, FOTCConfirmKey) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FOTCConfirmKey.c_str());
            
            fieldName = "TSEFillSubject";
            if( backendSectionPtr->GetValue( fieldName, FTSEFillSubject) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FTSEFillSubject.c_str());
            
            fieldName = "TSEFillKey";
            if( backendSectionPtr->GetValue( fieldName, FTSEFillKey) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FTSEFillKey.c_str());
            
            fieldName = "OTCFillSubject";
            if( backendSectionPtr->GetValue( fieldName, FOTCFillSubject) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FOTCFillSubject.c_str());
            
            fieldName = "OTCFillKey";
            if( backendSectionPtr->GetValue( fieldName, FOTCFillKey) == FALSE )
                UFC::BufferedLog::Printf( " %s [%s]%s not Exists, Use default value: %s.", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FOTCFillKey.c_str());
        }
        else
            UFC::BufferedLog::Printf( " %s Section[%s] not Exists.", logHead.c_str(), sectionName.c_str());
    }  //if( !isFieldMissed )

    delete configFilePtr;
    if (isFieldMissed)
    {
        UFC::BufferedLog::Printf(" %s ConfigFile[%s] [%s][%s] not Exists.", logHead.c_str(), configFileName.c_str(), sectionName.c_str(), fieldName.c_str());
        UFC::BufferedLog::FlushToFile();
        exit(1);
    }
    
    UFC::AnsiString aeConfigFileName = "../cfg/AE400.cfg";    
    if( UFC::FileExists( aeConfigFileName ) )
    {
        UFC::UiniFile *aeConfigPtr = 0;
        try
        {
            configFilePtr = new UFC::UiniFile( aeConfigFileName, FALSE );  //Do not Create
        }
        catch(UFC::FileException e)
        {
            UFC::BufferedLog::Printf(" %s Process [%s] Failed, FileException:%s.", logHead.c_str(), aeConfigFileName.c_str(), e.what());
            UFC::BufferedLog::FlushToFile();
            return;
        }
        
        UFC::Section* pSec = configFilePtr->GetSection( "AE" );
        if( pSec != 0 )
        {
            for( register int i = 0; i < pSec->ItemCount(); i++ )
            {
                UFC::AnsiString Name, Value;
                pSec->GetNameValue( i, Name, Value );

                UFC::AnsiString AE, WebID;
                AE = Value.SubString( 0, Value.AnsiPos( ',' ) );
                WebID = Value.SubString( Value.AnsiPos( ',' ) + 1, Value.Length() - Value.AnsiPos( ',' ) );
                AE.PadThis( 10, ' ' );
                WebID.PadThis( 2, ' ' );
                UFC::BufferedLog::DebugPrintf(UFC::dlPerformance, " %s AE:[%s] WebID:[%s]", logHead.c_str(), AE.c_str(), WebID.c_str() );
                FAETable.Add( Name, new AEInfo( AE, WebID ) );
            }  //for( register int i = 0; i < pSec->ItemCount(); i++ )
        }  //if( pSec != 0 )
        
        delete aeConfigPtr;
    }  //if( UFC::FileExists( aeConfigFileName ) )
}  //GCExecConnection::LoadSetting()
//------------------------------------------------------------------------------
GCExecConnection::~GCExecConnection( void )
{
    if( FConnection != NULL )
        delete FConnection;     
}
//------------------------------------------------------------------------------
void GCExecConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str(),  ctBoth );
    FIsLogon = TRUE;
}
//------------------------------------------------------------------------------
void GCExecConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void GCExecConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk )
    {
        UFC::BufferedLog::Printf( " %s( CID=%d )....logon ok", ReplyString, CID );
        
        UFC::AnsiString BeginTime;
        BeginTime.Printf( "%06d", FRecoverBeginTime.ToInt32() );
        
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
void GCExecConnection::Recover( UFC::AnsiString BeginTime, char Type, char EType )
{
    RecoverDataType REType;
    UFC::AnsiString EName;
    
    switch( EType )
    {
        case 'F': REType = rdFill;    EName ="filled ";  break;
        case 'C': REType = rdConfirm; EName ="confirm "; break;
        default:  REType = rdBoth;    EName =" ";
    }
    
    if( FIsLogon == TRUE )
    {
        if( Type == 'P' )
        {
            FConnection->Recover( BeginTime.c_str(), REType, rmForeignFutures );
            UFC::BufferedLog::Printf( " Recover PATS %sExecution from:%s", EName.c_str(), BeginTime.c_str() );
        }
        else if( Type == 'F' )
        {
            FConnection->Recover( BeginTime.c_str(), REType, rmFutures );
            UFC::BufferedLog::Printf( " Recover TAIFEX %sFutures Execution from:%s",EName.c_str(), BeginTime.c_str() );
        }
        else if( Type == 'O' )
        {
            FConnection->Recover( BeginTime.c_str(), REType, rmOption );
            UFC::BufferedLog::Printf( " Recover TAIFEX %sOptions Execution from:%s",EName.c_str(), BeginTime.c_str() );
        }
        else
        {
            FConnection->Recover( BeginTime.c_str(), REType, rmAll );
            UFC::BufferedLog::Printf( " Recover all %sExecution from:%s",EName.c_str(), BeginTime.c_str() );
        }
    }
}    
//------------------------------------------------------------------------------
void GCExecConnection::OnRecoverFinished( int Count )
{
    UFC::BufferedLog::DebugPrintf( " KGIExecConnection::OnRecoverFinished( Count = %d )", Count );
    
    UFC::SleepMS( 1000 );    
}
//------------------------------------------------------------------------------
void GCExecConnection::ConfirmToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString GIGO( Msg->GetData() );
    UFC::AnsiString TFX, Backend, SSNID;
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;

    char QueueData[ 513 ];     
    ///< TAIFEX part.
    if( Market == nsOrderMessageDefine::mTWFutures )
        TFX.Printf( "%s30%s", "        ", Msg->GetSrc()+2 );///< QTIME
    else
        TFX.Printf( "%s93%s", "        ", Msg->GetSrc()+2 );///< QTIME
    ///< SSNID
    SSNID.Printf( "%05d", atoi( Msg->GetPVC() ) ) ;
    UFC::BufferedLog::Printf( " SSNID:[%s]", SSNID.c_str() );
    ///< Backend part.    
    AE.PadThis( 6, ' ' );
    GIGO.PadThis( 23, ' ' );
    char TIF;
    if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC )
        TIF = 'I';
    else if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifFOK )
        TIF = 'F';
    else
        TIF = 'R';
    ///< WEBID(FWEBID), ORDERIM(0),UPD1(' '),OFFQTY(0),QEMP+QDEV(GIGO),QSAL(AE)
    Backend.Printf( "%s%011d %04d%s%s", FWEBID.c_str(), 0, 0, GIGO.c_str(), AE.c_str());
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
            if( DumpOnly == FALSE )
            {
                MessageObj->Send( FConfirmSubject, FConfirmKey, Data );
                if( TIF == 'R')
                    MessageObj->Send( FConfirmSubject, FConfirmRODKey, Data );
            }
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " <OPT CONFIRM> <%s>", QueueData );                                                            
            if( DumpOnly == FALSE )
            {
                MessageObj->Send( FConfirmSubject, FConfirmKey, Data );
                if( TIF == 'R')
                    MessageObj->Send( FConfirmSubject, FConfirmRODKey, Data );
            }
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void GCExecConnection::FillToBackend( TExecutionReportMessage* Msg )
{
    UFC::AnsiString Src( Msg->GetSrc() );
    UFC::AnsiString AE( Msg->GetAE() );
    UFC::AnsiString GIGO( Msg->GetData() );
    UFC::AnsiString TFX,Backend,TimeNow, MatchTime, SSNID;
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    MTree Data;
    char QueueData[ 513 ];     

    if( Src[0] == '1' )
        MatchTime = Src.SubString( 61, 8 );
    else if( Src[0] == '2' )
        MatchTime = Src.SubString( 109 , 8 );
    else
        MatchTime = Src.SubString( 57, 8 );

    UFC::BufferedLog::DebugPrintf( " MatchTime:[%s]", MatchTime.c_str() );
    UFC::BufferedLog::DebugPrintf( " BrokerID:[%s]", Msg->GetBrokerID() );
    ///< TAIFEX part.
    //UFC::GetHHMMSS( TimeNow );
    if( Market == nsOrderMessageDefine::mTWFutures )
        TFX.Printf( "        500002%s00%s01%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetBrokerID(), Msg->GetSrc() );
    else
        TFX.Printf( "        950002%s00%s01%s", MatchTime.SubString( 0, 6 ).c_str(), Msg->GetBrokerID(), Msg->GetSrc() );
    ///< Backend part.1
    AE.PadThis( 6, ' ' );
    GIGO.PadThis( 23, ' ' );
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
    memcpy( QueueData + 424, SSNID.c_str(), SSNID.Length() );
    memcpy( QueueData + 462, Backend.c_str(), Backend.Length());
    if( Src[0] == '3' || Src[0] == '4') ///< Canceled.
        QueueData[460] = 'S';
    Data.append("Data", QueueData );
    
    switch( Market )
    {
        case nsOrderMessageDefine::mTWFutures:
            UFC::BufferedLog::Printf( " <FUT Fill> <%s>", QueueData );                                                
            if( DumpOnly == FALSE )
            {
                MessageObj->Send( FFillSubject, FFillKey, Data );
                if( TIF == 'R')
                    MessageObj->Send( FFillSubject, FFillRODKey, Data );
            }
            break;
        case nsOrderMessageDefine::mTWOptions:
            UFC::BufferedLog::Printf( " <OPT Fill> <%s>", QueueData );
            if( DumpOnly == FALSE )
            {
                MessageObj->Send( FFillSubject, FFillKey, Data );
                if( TIF == 'R')
                    MessageObj->Send( FFillSubject, FFillRODKey, Data );
            }
            break;
        default:
            break;
    }        
}
//------------------------------------------------------------------------------
void GCExecConnection::TSEConfirmToBackend( TExecutionReportMessage* Msg )
{
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    UFC::AnsiString QueueData( Msg->GetSrc() );
    QueueData.PadThis( 256, ' ' );

    UFC::AnsiString Key;
    Key.Printf( "%s%s", Msg->GetAccount(), Msg->GetAE() );
    UFC::AnsiString AE = "          ";
    UFC::AnsiString WebID = "  ";
    AEInfo *info;
    if( (info = FAETable.GetObjectByKey( Key )) != NULL )
    {
        AE = info->AE;
        WebID = info->WebID;
    }
    UFC::BufferedLog::DebugPrintf( " [%s]=>[%s]:[%s]", Key.c_str(), AE.c_str(), WebID.c_str() );
    memcpy( (void*)(QueueData.c_str() + 244), AE.c_str(), 10 );
    memcpy( (void*)(QueueData.c_str() + 254), WebID.c_str(), 2 );

    MTree Data;
    Data.append("Data", QueueData );
    UFC::AnsiString brokerId = QueueData.SubString(14, 4);
    UFC::AnsiString sendKey;
    switch( Market )
    {
        case nsOrderMessageDefine::mTSE:
            UFC::BufferedLog::Printf( " <TSE CONFIRM> <%s> broker[%s]", QueueData.c_str(), brokerId.c_str() );
            if( DumpOnly == FALSE )
            {
                if (FIsBrokerModeEnabled) 
                    sendKey = FTSEConfirmKey + brokerId;
                else
                    sendKey = FTSEConfirmKey;
                MessageObj->Send( FTSEConfirmSubject, sendKey, Data );
            }
            break;
        case nsOrderMessageDefine::mOTC:
            UFC::BufferedLog::Printf( " <OTC CONFIRM> <%s> broker[%s]", QueueData.c_str(), brokerId.c_str() );
            if( DumpOnly == FALSE )
            {
                if (FIsBrokerModeEnabled) 
                    sendKey = FOTCConfirmKey + brokerId;
                else
                    sendKey = FOTCConfirmKey;
                MessageObj->Send( FOTCConfirmSubject, sendKey, Data );
            }
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void GCExecConnection::TSEFillToBackend( TExecutionReportMessage* Msg )
{
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();
    UFC::AnsiString MatchTime( Msg->GetTransactTime() );
    UFC::AnsiString msgSource = Msg->GetSrc();
    UFC::AnsiString QueueData;
    if( Market == nsOrderMessageDefine::mTSE )
        QueueData.Printf( "501000%06s00006201%s", MatchTime.SubString( 0, 6 ).c_str(), msgSource.c_str() );
    else
        QueueData.Printf( "951000%06s00006201%s", MatchTime.SubString( 0, 6 ).c_str(), msgSource.c_str() );
    QueueData.PadThis( 256, ' ' );
    
    UFC::AnsiString Key;
    Key.Printf( "%s%s", Msg->GetAccount(), Msg->GetAE() );
    UFC::AnsiString AE = "          ";
    UFC::AnsiString WebID = "  ";
    AEInfo *info;
    if( (info = FAETable.GetObjectByKey( Key )) != NULL )
    {
        AE = info->AE;
        WebID = info->WebID;
    }
    UFC::BufferedLog::DebugPrintf( " [%s]=>[%s]:[%s]", Key.c_str(), AE.c_str(), WebID.c_str() );

    memcpy( (void*)(QueueData.c_str() + 244), AE.c_str(), 10 );
    memcpy( (void*)(QueueData.c_str() + 254), WebID.c_str(), 2 );

    MTree Data;
    Data.append("Data", QueueData );
    UFC::AnsiString brokerId = msgSource.SubString(49, 4);
    UFC::AnsiString sendKey;
    switch( Market )
    {
        case nsOrderMessageDefine::mTSE:
            UFC::BufferedLog::Printf( " <TSE Fill> <%s> broker[%s]", QueueData.c_str(), brokerId.c_str() );
            if( DumpOnly == FALSE )
            {
                if (FIsBrokerModeEnabled) 
                    sendKey = FTSEFillKey + brokerId;
                else
                    sendKey = FTSEFillKey;
                MessageObj->Send( FTSEFillSubject, sendKey, Data );
            }
            break;
        case nsOrderMessageDefine::mOTC:
            UFC::BufferedLog::Printf( " <OTC Fill> <%s> broker[%s]", QueueData.c_str(), brokerId.c_str() );
            if( DumpOnly == FALSE )
            {
                if (FIsBrokerModeEnabled) 
                    sendKey = FOTCFillKey + brokerId;
                else
                    sendKey = FOTCFillKey;
                MessageObj->Send( FOTCFillSubject, sendKey, Data );
            }
            break;
        default:
            break;
    }
}
//------------------------------------------------------------------------------
void GCExecConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
    nsOrderMessageDefine::MarketEnum Market = Msg->GetMarket();    
    UFC::BufferedLog::Printf( " <Market> <%d>", Market ); 
    
    if( ( Market == nsOrderMessageDefine::mTSE ) || ( Market == nsOrderMessageDefine::mOTC ) )
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
        }  //switch( Msg->GetOrderStatus() )
    }
    else
    {
        switch( Msg->GetOrderStatus() )
        {
            case nsOrderMessageDefine::osNew:
                if( ( Market != nsOrderMessageDefine::mForeignFutures ) &&
                    ( Market != nsOrderMessageDefine::mForeignOptions ) )
                {
                    if( ( FilterIOCFOKExec == true ) && 
                        ( FilterIOCFOKCxlExecOnly == false ) &&
                        ( Msg->GetCumQty() == 0 ) &&
                        ( ( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC ) ||
                          ( Msg->GetTimeInForce() == nsOrderMessageDefine::tifFOK ) ) )
                    {
                        UFC::BufferedLog::Printf( " Skip IOC/FOK Confirm[%s]", Msg->GetOrderID() );
                        return;
                    }
                    if( FIsKGI == true )
                        KGIConfirmToBackend( Msg );
                    else
                        ConfirmToBackend( Msg );
                }
                else
                {
                    PATSExecutionReport( Msg, TRUE );
                }
                break;
            case nsOrderMessageDefine::osQuoteAccept:
                if( FilterIOCFOKExec == true && 
                    FilterIOCFOKCxlExecOnly == false &&
                    Msg->GetCumQty() == 0 &&
                    ( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC ||
                      Msg->GetTimeInForce() == nsOrderMessageDefine::tifFOK ) )
                {
                    UFC::BufferedLog::Printf( " Skip IOC/FOK Confirm[%s]", Msg->GetOrderID() );
                    return;
                }
                if( FIsKGI == true )
                    KGIConfirmToBackend( Msg, TRUE );
                else
                    ConfirmToBackend( Msg );
                break;
            case nsOrderMessageDefine::osReplaced:
                if( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ||
                    Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 )
                {                    
                    if( FIsKGI == true )
                        KGIConfirmToBackend( Msg );
                    else
                        ConfirmToBackend( Msg );
                }                    
                else
                {
                    if( FIsKGI == true )
                        KGIFillToBackend( Msg );
                    else
                        FillToBackend( Msg );
                }
                break;
            case nsOrderMessageDefine::osCanceled:
                if( ( Market != nsOrderMessageDefine::mForeignFutures ) &&
                    ( Market != nsOrderMessageDefine::mForeignOptions ) )

                {
                    if( ( FilterIOCFOKExec == true ) &&
                        ( ( Msg->GetTimeInForce() == nsOrderMessageDefine::tifIOC ) ||
                          ( Msg->GetTimeInForce() == nsOrderMessageDefine::tifFOK ) ) && 
                        ( Msg->GetLeavesQty() == 0 ) )
                    {
                        UFC::BufferedLog::Printf( " Skip IOC/FOK Canceled[%s]", Msg->GetOrderID() );                        
                        return;
                    }
                    if( FIsKGI == true )
                        KGIFillToBackend( Msg );
                    else
                        FillToBackend( Msg );
                }
                else
                {
                    PATSExecutionReport( Msg, FALSE );                
                }
                break;
            case nsOrderMessageDefine::osPartiallyFilled:
            case nsOrderMessageDefine::osFilled:
                if( ( Market != nsOrderMessageDefine::mForeignFutures ) &&
                    ( Market != nsOrderMessageDefine::mForeignOptions ) )
                {
                    if( FIsKGI == true )
                        KGIFillToBackend( Msg );
                    else
                        FillToBackend( Msg );
                }
                else
                {
                    PATSExecutionReport( Msg, FALSE );
                }
                break;
            case nsOrderMessageDefine::osPendingNew:
            case nsOrderMessageDefine::osPendingReplace:
            case nsOrderMessageDefine::osPendingCancel:
                break;
            case nsOrderMessageDefine::osRejected:
                if( ( Market == nsOrderMessageDefine::mForeignFutures ) ||
                    ( Market == nsOrderMessageDefine::mForeignOptions ) )
                {
                    PATSExecutionReport( Msg, FALSE );
                }
                break;
            default: break;
        }  //switch( Msg->GetOrderStatus() )
    }  //if( ( Market == nsOrderMessageDefine::mTSE ) || ( Market == nsOrderMessageDefine::mOTC ) )
}  //GCExecConnection::OnExecutionReport()
//------------------------------------------------------------------------------
void GCExecConnection::Execute( void )
{      
    while( !IsTerminated() )
    {
        if( FIsLogon == FALSE )
            FConnection->Connect( FIP.c_str() , FPort, 10 );    
        sleep( 10 ); ///< wait 10 sec.
    }
}
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
double GCExecConnection::FindCommodityMultiplier(const UFC::AnsiString ExchangeCode, const UFC::AnsiString Symbol)
{
    double commodityMultiplier = 1.0;
    UFC::AnsiString keyValue;
    keyValue.Printf("%s,%s", ExchangeCode.c_str(), Symbol.c_str());
    for (int i = 0; i < FCommodityMultiplierTable.ItemCount(); i++)
    {
        UFC::AnsiString curKeyValue;
        double curCommodityMultiplier;
        if (FCommodityMultiplierTable.GetItem(i, curKeyValue, curCommodityMultiplier) == FALSE) continue;
        if (curKeyValue.AnsiPos(keyValue) == 0)
        {
            commodityMultiplier = curCommodityMultiplier;
            break;
        }  //if (keyValue.AnsiPos(keyValue) == 0)
    }  //for (int i = 0; i < FCommodityMultiplierTable.ItemCount(); i++)
    return commodityMultiplier;
}  //GCExecConnection::FindCommodityMultiplier()
//------------------------------------------------------------------------------
void GCExecConnection::PATSExecutionReport( TExecutionReportMessage* Msg, BOOL IsConfirm )
{
/*
0                                                 0                                                 1                                                 1                                                 2                                                 2                                                 3                                                 3                                                 4                                                 4                                                 5                                                 5                                                 6
0         1         2         3         4         5         6         7         8         9         0         1         2         3         4         5         6         7         8         9         0         1         2         3         4         5         6         7         8         9         0         1         2         3         4         5         6         7         8         9         0         1         2         3         4         5         6         7         8         9         0         1         2         3         4         5         6         7         8         9         0
0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
20100862609UOB                                                                   9736691             Du0002              9736691             PATS9736691_2                                                          QUEST                0        SIM                 J-Yen               FUT  DEC14     0000000000000 100000000040009191000000000000000000020                     000000000000000000000000000000004000000000000000000000001        N9990000000W04003801A20140922160713                                                                                              
20300862610UOB                                                                   9736691             Du0002              9736691             SIM       101882    2336140487:9736691            20140922101          QUEST                1        SIM                 J-Yen               FUT  DEC14     0000000000000 100000000040000000009192000000000000020                     000000000400000000091920000000000000000000400000000000001        N9990000000W04003801A20140922160713                                                                                              
*/
    UFC::AnsiString logHeader = "GCExecConnection::PATSExecutionReport()";
    nsOrderMessageDefine::OrderStatusEnum msgOrderStatus = Msg->GetOrderStatus();
    nsOrderMessageDefine::ExecTypeEnum msgExecType = Msg->GetExecType();
    nsOrderMessageDefine::SideEnum msgSide = Msg->GetSide();

    UFC::AnsiString msgOrderId(Msg->GetOrderID());
    UFC::AnsiString orderId;
    orderId.Printf("%c%s", FOrderIDPrefix, msgOrderId.c_str());
    orderId.PadThis(20, ' ');
    
    UFC::AnsiString msgPatsOrderID(Msg->GetClOrdID());
    UFC::AnsiString patsOrderID = msgPatsOrderID;
    patsOrderID.PadThis( 20, ' ');

    UFC::AnsiString msgExecId(Msg->GetExecID());

    UFC::AnsiString msgExchangeCode(Msg->GetExchangeCode());
    UFC::AnsiString exchange = msgExchangeCode;
    exchange.PadThis(20, ' ');
    
    UFC::AnsiString msgPatsSymbol(Msg->GetSymbol());
    
    int msgOrderQty = Msg->GetOrderQty();
    int msgLastQty = Msg->GetLastQty();
    
    double msgPrice = Msg->GetPrice();
    double msgStopPrice = Msg->GetStopPx();
    double msgLastPrice = Msg->GetLastPx();
    double msgAveragePrice = Msg->GetAvgPx();
    double commodityMultiplier = FindCommodityMultiplier(msgExchangeCode, msgPatsSymbol);
    
    UFC::AnsiString twSymbol;
    UFC::AnsiString symbol;
    if (FSymbolConvertTable.GetObjectByKey(msgPatsSymbol, twSymbol) == TRUE)
        symbol = twSymbol;
    else
        symbol = msgPatsSymbol;
    symbol.PadThis(20, ' ');
    
    UFC::AnsiString msgMaturityMonthYear(Msg->GetMaturityMonthYear());
    std::vector<CPatsDateObject*> dateObjArray;
    int monthCount = ParsePatsContractDate(msgMaturityMonthYear, dateObjArray);
    CPatsDateObject *firstPatsDateObjPtr = 0;
    if (dateObjArray.size() >= 1) firstPatsDateObjPtr = dateObjArray[0];
    
    UFC::BufferedLog::Printf(" %s Process Report ExecID[%s] OrderID[%s] PATSOrderID[%s] [%s][%s][%s] Side=%d[%s] OrderStatus=%d[%s] ExecType=%d[%s] OrderQty=%d LastQty=%d Px=%15.6lf LastPx=%15.6lf monthCount=%d multiplier=%16.5lf.", 
                             logHeader.c_str(), msgExecId.c_str(), msgOrderId.c_str(), msgPatsOrderID.c_str(),
                             msgExchangeCode.c_str(), msgPatsSymbol.c_str(), msgMaturityMonthYear.c_str(),
                             msgSide, DescribeSide(msgSide).c_str(),
                             msgOrderStatus, DescribeOrderStatus(msgOrderStatus).c_str(),
                             msgExecType, DescribeExecType(msgExecType).c_str(), msgOrderQty, msgLastQty, msgPrice, msgLastPrice, monthCount, commodityMultiplier);
    if ((twSymbol.Length() > 0) && (twSymbol != msgPatsSymbol))
        UFC::BufferedLog::Printf( " %s Converted PatSymbol[%s] to [%s]", logHeader.c_str(), msgPatsSymbol.c_str(), twSymbol.c_str());
    UFC::BufferedLog::FlushToFile();
    
for (unsigned int i = 0; i < dateObjArray.size(); i++)
{
CPatsDateObject *curPatsDateObjPtr = dateObjArray[i];
curPatsDateObjPtr->DupmData();
}  //for (int i = 0; i < dateObjArray.size(); i++)
UFC::BufferedLog::FlushToFile();
    
    if (msgExecId.Length() > 0)
    {
        if (FPatsExecIdSet.Exists(msgExecId) == TRUE)
        {
            UFC::BufferedLog::Printf(" %s Duplicate ExecID[%s], Ignore this Execution Report. OrderID[%s] PATSOrderID[%s].", logHeader.c_str(), msgExecId.c_str(), msgOrderId.c_str(), msgPatsOrderID.c_str());
            UFC::BufferedLog::FlushToFile();
            return;
        }
        FPatsExecIdSet.Add(msgExecId);
    }  //if (msgExecId.Length() > 0)
    UFC::AnsiString execId = msgExecId;
    execId.PadThis(70, ' ');
    
    UFC::AnsiString rptTime(Msg->GetTransactTime());
    
    UFC::AnsiString ae(Msg->GetAE());
    ae.PadThis(6, ' ');
    
    UFC::AnsiString today;
    UFC::GetYYYYMMDD(today);
    
    UFC::AnsiString tempValue;
    PATSExec Rpt;
    memset(&Rpt, ' ', sizeof(PATSExec));
    if(Msg->GetOrderStatus() == nsOrderMessageDefine::osNew)
        memcpy(Rpt.FunctionCode, "201", 3);  ///< New Order
    else 
        memcpy(Rpt.FunctionCode, "203", 3);  ///< Canceled and Filled.   
    GCExecConnection::FSequence += 1;
    if(FSequence.ToInt32() > 99999999) FSequence = 1;
    tempValue.Printf("%08d", FSequence.ToInt32());
    memcpy( Rpt.SeqNo, tempValue.c_str(), 8 );  ///?
    memcpy( Rpt.FCM, "UOB", 3 );
    memcpy( Rpt.OrderID, patsOrderID.c_str(), 20 );
    memcpy( Rpt.OOrderID, orderId.c_str(), 20 ); //X(20) Speedy Order ID x(6)
    memcpy( Rpt.SecondaryOrderID, patsOrderID.c_str(), 20 );  //X(20) ----- Not Use ----- ??    
    memcpy( Rpt.ExecID, execId.c_str(), 70 );  //X(70) PATS FIll execid
    memcpy( Rpt.OrderKind, "QUEST", 5 );  //X(20) QUEST,QUEST+1,EDS,REG... ??
    switch(msgOrderStatus)
    {
        case nsOrderMessageDefine::osNew:             Rpt.OrderStatus[0] = '0'; break;
        case nsOrderMessageDefine::osPartiallyFilled: Rpt.OrderStatus[0] = '1'; break;
        case nsOrderMessageDefine::osFilled:          Rpt.OrderStatus[0] = '2'; break;
        case nsOrderMessageDefine::osCanceled:        Rpt.OrderStatus[0] = '4'; break;
        case nsOrderMessageDefine::osRejected:        Rpt.OrderStatus[0] = '8'; break;
        case nsOrderMessageDefine::osPendingNew:      Rpt.OrderStatus[0] = 'A'; break;
        case nsOrderMessageDefine::osPendingReplace:  Rpt.OrderStatus[0] = 'E'; break;
        case nsOrderMessageDefine::osReplaced:        Rpt.OrderStatus[0] = '5'; break;
        case nsOrderMessageDefine::osPendingCancel:   Rpt.OrderStatus[0] = '6'; break;
        case nsOrderMessageDefine::osExpired:         Rpt.OrderStatus[0] = 'C'; break;
        default:                                      Rpt.OrderStatus[0] = ' ';  //osNone, osQuoteAccept, osQuoteRequestAccept
    }   //X(1)  New:0 Partially Filled:1 Filled:2 Canceled:4 Reject:8
    memcpy( Rpt.Exchange, exchange.c_str(), 20 );  //X(20) Exchange
    memcpy( Rpt.Symbol, symbol.c_str(), 20 );  //X(20) Symbol
    
    UFC::AnsiString maturityMonthYear = msgMaturityMonthYear;
    UFC::AnsiString strikePriceStr = "0000000000000";
    UFC::AnsiString securityType = "FUT";
    char callPutChar = ' ';
    if (firstPatsDateObjPtr != 0)
    {
        double strikePrice = 0.0;
        maturityMonthYear = firstPatsDateObjPtr->GetMaturityMonthYearStr();
        tempValue = firstPatsDateObjPtr->GetStrikePriceStr();
        if (tempValue.Length() > 0) strikePrice = tempValue.ToDouble();
        if (firstPatsDateObjPtr->GetCallPutChar() == 'C')
        {
            strikePriceStr.Printf("%013lld", DoubleToInt64(strikePrice, 6));
            securityType = "OPT";
            callPutChar = '1';
        }
        else if (firstPatsDateObjPtr->GetCallPutChar() == 'P')
        {
            strikePriceStr.Printf("%013lld", DoubleToInt64(strikePrice, 6));
            securityType = "OPT";
            callPutChar = '0';
        }
    }  //if (firstPatsDateObjPtr != 0)
    UFC::AnsiString padMaturityMonthYear = maturityMonthYear;
    padMaturityMonthYear.PadThis(10, ' ');
    memcpy(Rpt.MaturityMonthYear, padMaturityMonthYear.c_str(), 10);  //X(10) Maturity Month Year
    memcpy(Rpt.SecurityType, securityType.c_str(), 3);  //X(5) "FUT"
    memcpy(Rpt.StrikePrice, strikePriceStr, 13);  //9(7)V9(6) Strike Price
    Rpt.PutOrCall[0] = callPutChar;  //X(1)  Put:'0' Call:'1'
    
    char sideChar = ' ';
    if( msgSide == nsOrderMessageDefine::sBuy )
        sideChar = '1';  //X(1) Buy:'1'
    else
        sideChar = '2';  //X(1) Sell:'2'
    Rpt.Side[0] = sideChar; 
        
    tempValue.Printf( "%010d", msgOrderQty);
    memcpy(Rpt.OrderQuantity, tempValue.c_str(), 10);  //9(10) Qty
    
    double tempPrice = msgPrice * commodityMultiplier;
    tempValue.Printf("%013lld", DoubleToInt64( tempPrice, 6 ) );    
    memcpy( Rpt.Price, tempValue.c_str(), 13 ); //9(7)V9(6) Price
    
    tempPrice = msgStopPrice * commodityMultiplier;
    tempValue.Printf("%013lld", DoubleToInt64( tempPrice, 6 ) );    
    memcpy( Rpt.StopPrice, tempValue.c_str(), 13 ); //9(7)V9(6) Stop Price
    switch( Msg->GetOrderType() )
    {
        case nsOrderMessageDefine::otLimit:     Rpt.OrderType[0] = '2'; break;
        case nsOrderMessageDefine::otMarket:    Rpt.OrderType[0] = '1'; break; 
        case nsOrderMessageDefine::otStop:      Rpt.OrderType[0] = '3'; break; 
        case nsOrderMessageDefine::otStopLimit: Rpt.OrderType[0] = '4'; break;
        default:                                Rpt.OrderType[0] = ' ';  //otNone, otMarketWithProtection
    } //X(1) Market:1 Limit:2 Stop:3 StopLimit:4  
    
    Rpt.TimeInForce[0] = '0'; ///< ROD Only
    tempValue.Printf( "%010d", msgLastQty);
    memcpy( Rpt.LastShares, tempValue.c_str(), 10 );  //9(10) LastQty
    
    tempPrice = msgLastPrice * commodityMultiplier;
    tempValue.Printf("%013lld", DoubleToInt64( tempPrice, 6 ) );
    memcpy( Rpt.LastPrice, tempValue.c_str(), 13 );  //9(7)V9(6) Last Price
    
    tempValue.Printf( "%010d", Msg->GetLeavesQty() );
    memcpy( Rpt.LeavesQuantity, tempValue.c_str(), 10 );  //9(10) Leaves Qty
    
    tempValue.Printf( "%010d", Msg->GetCumQty() );
    memcpy( Rpt.CumQuantity, tempValue.c_str(), 10 );  //9(10) Cum Qty
    
    tempPrice = msgAveragePrice * commodityMultiplier;
    tempValue.Printf("%013lld", DoubleToInt64( tempPrice, 6 ) );    
    memcpy( Rpt.AveragePrice, tempValue.c_str(), 13 );    //9(7)V9(6) ----- Not Use -----*/
    
    Rpt.RejectResponseTo[0] = '1';     //X(1) Cancel:'1' Replace:'2'
    if( Msg->GetPositionEffect() == nsOrderMessageDefine::peDayTradeOpen )
        Rpt.Day_Trade[0] = 'Y';        //X(1)  Yes
    else
        Rpt.Day_Trade[0] = 'N';        //X(1)  No
    memcpy( Rpt.Branch, "999", 3 );    //X(3) "999"
    memcpy( Rpt.Cust_No,"0000000", 7 );//X(7) "0000000"
    memcpy( Rpt.Ae, ae.c_str(), 6);    //X(6) "A01001"
    memcpy( Rpt.Web, "801", 3 );        //X(3) "801"
    Rpt.Original[0] = 'A';             //X(1) 'A'
    memcpy( Rpt.OrdDate, today.c_str(), today.Length());  //X(8) "20111227"
    
    if( rptTime.Length() != 6 ) UFC::GetHHMMSS( rptTime );
    memcpy( Rpt.OrdTime, rptTime.c_str(), rptTime.Length());  //X(6) "HHMMSS"
    Rpt.EOS = 0;
    
    if( DumpOnly == FALSE )
    {
        if( IsConfirm == TRUE )
        {   //委託回報
            if (monthCount <= 1)
            {   //非組合單，直接送出
                UFC::BufferedLog::Printf(" <PATSExec> <%s>", (char*)&Rpt);
                MTree sendData;
                sendData.append("Data", (char*)&Rpt);
                MessageObj->Send("PATSConfirm", "To400", sendData);
            }
            else  //組合單
            {
                if ((msgOrderStatus == nsOrderMessageDefine::osNew) || (msgOrderStatus == nsOrderMessageDefine::osPendingNew))
                {
                    UFC::BufferedLog::Printf(" %s Save Composed Confirm ExecutionReport OrderId[%s] [%s][%s][%s], Wait for Fill Report.", logHeader.c_str(), msgOrderId.c_str(), msgExchangeCode.c_str(), msgPatsSymbol.c_str(), msgMaturityMonthYear.c_str());
                    CPATSConfirmExecutionReport *newConfirmExecRptPtr = new CPATSConfirmExecutionReport(msgOrderId, msgMaturityMonthYear, &Rpt);
                    if (FPATSConfirmExecRptTable.Add(msgOrderId, newConfirmExecRptPtr) == FALSE)
                    {
                        UFC::BufferedLog::Printf(" %s Add Composed Confirm ExecutionReport OrderId[%s] [%s][%s][%s] failed. OrderId Duplicated.", logHeader.c_str(), msgOrderId.c_str(), msgExchangeCode.c_str(), msgPatsSymbol.c_str(), msgMaturityMonthYear.c_str());
                        delete newConfirmExecRptPtr;
                    }    
                    
                }
                else
                    UFC::BufferedLog::Printf(" %s Ignore Confirm Composed Execution Report. OrderStatus%d[%s]", logHeader.c_str(), msgOrderStatus, DescribeOrderStatus(msgOrderStatus).c_str());                
            }  //if (monthCount <= 1)
        }
        else
        {   //非委託回報（刪單，成交，退單回報）
            if ((msgOrderStatus == nsOrderMessageDefine::osFilled) || (msgOrderStatus == nsOrderMessageDefine::osPartiallyFilled))
            {   //成交回報要先尋找之前的委託回報，確認是不是組合單
                CPATSConfirmExecutionReport *confirmExecRptPtr = FPATSConfirmExecRptTable.GetObjectByKey(msgOrderId);
                if (confirmExecRptPtr != 0)
                {   //找到委託回報，是組合單
                    //解開組合單的日期字串
                    UFC::AnsiString confirmContractDate = confirmExecRptPtr->FPATSContractDate;
                    std::vector<CPatsDateObject*> confirmDateObjArray;
                    int confirmMonthCount = ParsePatsContractDate(confirmContractDate, confirmDateObjArray);
                    //判斷是哪一隻腳
                    int confirmLeg = 0;
                    if (confirmMonthCount > 1)
                    {
                        for (unsigned int i = 0; i < confirmDateObjArray.size(); i++)
                        {
                            CPatsDateObject *curPatsDateObjPtr = confirmDateObjArray[i];
                            UFC::AnsiString confirmMonthYear = curPatsDateObjPtr->GetMaturityMonthYearStr();
                            if (maturityMonthYear.AnsiPos(confirmMonthYear) >= 0)
                            {
                                if (i == 0)
                                    confirmLeg = 1;  //第一隻腳
                                else if (i == 1)
                                    confirmLeg = 2;  //第二隻腳
                                break;
                            }
                            if (i >= 1) break;
                        }  //for (unsigned int i = 0; i < confirmDateObjArray.size(); i++)
                    }  //if (confirmMonthCount > 1)
                    ClearPatsContractDateArray(confirmDateObjArray);

                    if ((confirmLeg == 1) || (confirmLeg == 2))
                    {   //確定是哪一隻腳
                        tempValue.Printf("%c", FOrderIDPrefix);
                        if (confirmLeg == 2) tempValue.LowerThis();  //第二隻腳前置字元改小寫
                        orderId.Printf("%s%s", tempValue.c_str(), msgOrderId.c_str());
                        orderId.PadThis(20, ' ');
                        
                        //修改委回資料
                        memcpy(Rpt.OOrderID, orderId.c_str(), 20); //X(20) Speedy Order ID x(6)
                        memcpy(confirmExecRptPtr->FExecRptData.OOrderID, orderId.c_str(), 20); //X(20) Speedy Order ID x(6)
                        memcpy(confirmExecRptPtr->FExecRptData.MaturityMonthYear, padMaturityMonthYear.c_str(), 10);  //X(10) Maturity Month Year
                        memcpy(confirmExecRptPtr->FExecRptData.SecurityType, securityType.c_str(), 3);  //X(5) "FUT"
                        memcpy(confirmExecRptPtr->FExecRptData.StrikePrice, strikePriceStr, 13);  //9(7)V9(6) Strike Price
                        confirmExecRptPtr->FExecRptData.PutOrCall[0] = callPutChar;  //X(1)  Put:'0' Call:'1'
                        confirmExecRptPtr->FExecRptData.Side[0] = sideChar;  //X(1) Buy:'1' Sell:'2'
                        
                        tempPrice = Msg->GetLastPx() * commodityMultiplier;
                        tempValue.Printf("%013lld", DoubleToInt64( tempPrice, 6 ) );    
                        memcpy(confirmExecRptPtr->FExecRptData.Price, tempValue.c_str(), 13 ); //9(7)V9(6) Price

                        //送出委回資料
                        UFC::BufferedLog::Printf(" %s Send Leg%d Confirm Execution Report.", logHeader.c_str(), confirmLeg);
                        UFC::BufferedLog::Printf(" <PATSExec> <%s>", (char*)&(confirmExecRptPtr->FExecRptData));                                                            
                        MTree confirmData;
                        confirmData.append("Data", (char*)&(confirmExecRptPtr->FExecRptData));
                        MessageObj->Send("PATSConfirm", "To400", confirmData);
                    }
                    else
                        UFC::BufferedLog::Printf(" %s Confirm Contract Date[%s] and Fill Contract Date[%s] not Match, Ignore Confirm Execution Report.", logHeader.c_str(), confirmContractDate.c_str(), maturityMonthYear.c_str());
                }  //if (confirmExecRptPtr != 0)
            }  //if ((msgOrderStatus == nsOrderMessageDefine::osFilled) || (msgOrderStatus == nsOrderMessageDefine::osPartiallyFilled))
            UFC::BufferedLog::FlushToFile();
            
            if (((msgOrderStatus == nsOrderMessageDefine::osFilled) || (msgOrderStatus == nsOrderMessageDefine::osPartiallyFilled)) ||
                (monthCount <= 1))
            {
                UFC::BufferedLog::Printf(" <PATSExec> <%s>", (char*)&Rpt);                                                            
                MTree sendData;
                sendData.append("Data", (char*)&Rpt);
                MessageObj->Send("PATSFill", "To400", sendData);
            }
            else  //忽略組合單的刪單，退單回報
                UFC::BufferedLog::Printf(" %s Ignore Composed Execution Report. OrderStatus%d[%s]", logHeader.c_str(), msgOrderStatus, DescribeOrderStatus(msgOrderStatus).c_str());
        }  //if( IsConfirm == TRUE )
    }  //if( DumpOnly == FALSE )
    else
        UFC::BufferedLog::Printf(" <PATSExec> <%s>", (char*)&Rpt);                                                            
    UFC::BufferedLog::FlushToFile();
    ClearPatsContractDateArray(dateObjArray);
}  //GCExecConnection::PATSExecutionReport()
//------------------------------------------------------------------------------
