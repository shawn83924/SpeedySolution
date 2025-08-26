#include <math.h>
#include "GCRMConnection.h"
//------------------------------------------------------------------------------
extern BOOL DumpOnly;
extern BOOL RecoverMode;
extern UFC::AnsiString gProgramName;
extern UFC::AnsiString gApplicationNamePrefix;
extern UFC::AnsiString gAppName;

extern UFC::AnsiString gFuturesP14FilePathName;
extern UFC::AnsiString gOptionsP14FilePathName;
extern UFC::AnsiString gDayFuturesP14FilePathName;
extern UFC::AnsiString gDayOptionsP14FilePathName;

extern UFC::BufferedLog* gUniRMLogPtr;

#define EXECID_LEN 64

//------------------------------------------------------------------------------
int GetContractStrIndexMapCount(CStrContractMap& StrContractMap) 
{
    return static_cast<int>(StrContractMap.size());
}  //GetContractStrIndexMapCount()
//------------------------------------------------------------------------------
CContractObject *FindContractFromStrIndexMap(CStrContractMap& StrContractMap, const UFC::AnsiString& KeyValue)
{
    CContractObject *contractPtr = 0;
    CStrContractMap::iterator it = StrContractMap.find(KeyValue);
    if (it != StrContractMap.end()) contractPtr = it->second;
    return contractPtr;
}  //FindContractFromStrIndexMap()
//------------------------------------------------------------------------------
bool InsertContractToStrIndexMap(CStrContractMap& StrContractMap, const UFC::AnsiString& KeyValue, CContractObject *ContractPtr)
{
    std::pair<CStrContractMap::iterator, bool> insertPair = StrContractMap.insert(CStrContractMap::value_type(KeyValue, ContractPtr));
    return insertPair.second;
}  //InsertContractToStrIndexMap()
//------------------------------------------------------------------------------
int ClearContractStrIndexMap(CStrContractMap& StrContractMap, bool DoDelete)
{
    int deleteCount = 0;
    CStrContractMap::iterator loopIt;
    for (loopIt = StrContractMap.begin(); loopIt != StrContractMap.end(); loopIt++)
    {
        CContractObject *curContractPtr = loopIt->second;
        if (curContractPtr == 0) continue;
        deleteCount++;
        if (DoDelete) delete curContractPtr;
    }
    StrContractMap.clear();
    return deleteCount;    
}  //ClearContractStrIndexMap()
//------------------------------------------------------------------------------
CContractObject *GetFirstContractFromStrIndexMap(CStrContractMap& StrContractMap)
{
    CContractObject *contractPtr = 0;
    if (static_cast<int>(StrContractMap.size()) <= 0) return contractPtr;
    CStrContractMap::iterator it = StrContractMap.begin();
    if (it != StrContractMap.end()) contractPtr = it->second;
    return contractPtr;    
}  //GetFirstContractFromStrIndexMap()
//------------------------------------------------------------------------------
CContractObject *GetSecondContractFromStrIndexMap(CStrContractMap& StrContractMap)
{
    CContractObject *contractPtr = 0;
    if (static_cast<int>(StrContractMap.size()) <= 1) return contractPtr;
    CStrContractMap::iterator it = StrContractMap.begin();
    it++;
    if (it != StrContractMap.end()) contractPtr = it->second;
    return contractPtr;
}  //GetSecondContractFromStrIndexMap()
//------------------------------------------------------------------------------
void DumpContractStrIndexMap(CStrContractMap& StrContractMap)
{
    int loopCount = 0;
    CStrContractMap::iterator loopIt;
    for (loopIt = StrContractMap.begin(); loopIt != StrContractMap.end(); loopIt++)
    {
        CContractObject *curContractPtr = loopIt->second;
        if (curContractPtr != 0)
        {
            loopCount++;
            UFC::BufferedLog::Printf(" Contract %d[%s][%s]", loopCount, curContractPtr->GetSymbol().c_str(), curContractPtr->GetDeliveryYearMonth().c_str());
        }
    }
}  //DumpContractStrIndexMap()
//------------------------------------------------------------------------------
int TranslateDigitalChar(char DigitalChar)
{
    int digitalValue = 0;
    switch (DigitalChar)
    {
        case '0': digitalValue = 0; break;
        case '1': digitalValue = 1; break;
        case '2': digitalValue = 2; break;
        case '3': digitalValue = 3; break;
        case '4': digitalValue = 4; break;
        case '5': digitalValue = 5; break;
        case '6': digitalValue = 6; break;
        case '7': digitalValue = 7; break;
        case '8': digitalValue = 8; break;
        case '9': digitalValue = 9; break;
        default : digitalValue = 0;
    }
    return digitalValue;
}  //TranslateDigitalChar()
//------------------------------------------------------------------------------
UFC::AnsiString BuildDoubleStr(const UFC::AnsiString& OrigStr, int DecimalLocator)
{
    UFC::AnsiString doubleStr;
    int origStrLength = OrigStr.Length();
    if (DecimalLocator > 0)
    {
        if (origStrLength > DecimalLocator)
        {
            UFC::AnsiString integerPartStr, decimalPartStr;
            int integerPartLength = origStrLength - DecimalLocator;
            integerPartStr = OrigStr.SubString(0, integerPartLength);
            decimalPartStr = OrigStr.SubString(integerPartLength, DecimalLocator);
            doubleStr.Printf("%s.%s", integerPartStr.c_str(), decimalPartStr.c_str());
        }
        else
            doubleStr.Printf("0.%s", OrigStr.c_str());
    }
    else 
        doubleStr = OrigStr;
    return doubleStr; 
}  //BuildDoubleStr()

//------------------------------ CCurrencyObject -------------------------------
//------------------------------------------------------------------------------
CCurrencyObject::CCurrencyObject(const UFC::AnsiString& CurrencyCode, char CurrencyType)
:FCurrencyCode(CurrencyCode)
,FCurrencyType(CurrencyType)
{
}  //CCurrencyObject::CCurrencyObject()
//------------------------------------------------------------------------------
UFC::AnsiString CCurrencyObject::ToString()
{
    UFC::AnsiString objStr;
    objStr.Printf("Currency[%s][%c]", FCurrencyCode.c_str(), FCurrencyType);
    return objStr;
}  //CCurrencyObject::ToString()

//------------------------------ CCommodityObject ------------------------------
//------------------------------------------------------------------------------
UFC::AnsiString CCommodityObject::DescribeCurrency(char CurrencyType)
{
    UFC::AnsiString currencyStr = "";
    switch (CurrencyType)
    {
        case '1': currencyStr = "NTD"; break;
        case '2': currencyStr = "USD"; break;
        case '3': currencyStr = "EUR"; break;
        case '4': currencyStr = "JPY"; break;
        case '5': currencyStr = "GBP"; break;
        case '6': currencyStr = "AUD"; break;
        case '7': currencyStr = "HKD"; break;
        case '8': currencyStr = "CNY"; break;
        case 'A': currencyStr = "ZAR"; break;
        default : currencyStr = "";
    }
    return currencyStr;
}  //CCommodityObject::DescribeCurrency()
//------------------------------------------------------------------------------
CCommodityObject::CCommodityObject(const UFC::AnsiString& ContractCode, bool IsFutures)
:FContractCode(ContractCode)
,FStockID("")
,FContractKind(' ')
,FContractMultiplier(1.0)
,FStatusCode(' ')
,FCurrencyType(' ')
,FStrikePriceDecimalLocator(0)
,FOrderDecimalLocator(0)
,FAcceptQuoteFlag(' ')
,FBeginDate("")
,FBlockTradeFlag(' ')
,FExpiryType(' ')
,FIsFutures(IsFutures)
,FIsEnabled(false)
,FABType(' ')
,FMarginType(' ')
,FClearingMargin(0.0)
,FMaintenanceMargin(0.0)
,FInitialMargin(0.0)
{  
}  //CCommodityObject::CCommodityObject()
//------------------------------------------------------------------------------
CCommodityObject::CCommodityObject(const char *P09RecBuffer, bool IsFutures)
:FContractCode("")
,FStockID("")
,FContractKind(' ')
,FContractMultiplier(1.0)
,FStatusCode(' ')
,FCurrencyType(' ')
,FStrikePriceDecimalLocator(0)
,FOrderDecimalLocator(0)
,FAcceptQuoteFlag(' ')
,FBeginDate("")
,FBlockTradeFlag(' ')
,FExpiryType(' ')
,FIsFutures(IsFutures)
,FIsEnabled(false)
,FABType(' ')
,FMarginType(' ')
,FClearingMargin(0.0)
,FMaintenanceMargin(0.0)
,FInitialMargin(0.0)
{
    ParseP09Record(P09RecBuffer);
}  //CCommodityObject::CCommodityObject()
//------------------------------------------------------------------------------
void CCommodityObject::ParseP09Record(const char *P09RecBuffer)
{
    UFC::AnsiString p09RecStr(P09RecBuffer);
    FContractCode = p09RecStr.SubString(0, 4);    
    FContractCode.TrimRight(' ');
    
    FStockID = p09RecStr.SubString(34, 6);
    FStockID.TrimRight(' ');
            
    FContractKind = p09RecStr[40];
            
    UFC::AnsiString contractMultiplierStr = p09RecStr.SubString(41, 11);  //9(7)V9(4)
    UFC::AnsiString integerPartStr = contractMultiplierStr.SubString(0, 7);
    UFC::AnsiString decimalPartStr = contractMultiplierStr.SubString(7, 4);
    FContractMultiplier = integerPartStr.ToDouble() + decimalPartStr.ToDouble() / 10000.0;
            
    FStatusCode = p09RecStr[52];
    FCurrencyType = p09RecStr[53];
    FStrikePriceDecimalLocator = TranslateDigitalChar(p09RecStr[54]);
    FOrderDecimalLocator = TranslateDigitalChar(p09RecStr[55]);
    FAcceptQuoteFlag = p09RecStr[56]; 
    
    FBeginDate = p09RecStr.SubString(57, 8);
    FBeginDate.TrimRight(' ');
    
    FBlockTradeFlag = p09RecStr[65];  //Y, N
    FExpiryType = p09RecStr[66];  //S:standard, W:week
}  //CCommodityObject::ParseP09Record()
//------------------------------------------------------------------------------
CCommodityObject::~CCommodityObject()
{
    ClearContractStrIndexMap(FContractSymbolIndex, true);
}  //CCommodityObject::~CCommodityObject()
//------------------------------------------------------------------------------
int CCommodityObject::GetContractCount() 
{
    return GetContractStrIndexMapCount(FContractSymbolIndex);
}  //CCommodityObject::GetContractCount()
//------------------------------------------------------------------------------
CContractObject *CCommodityObject::FindContractFromSymbolIndex(const UFC::AnsiString& Symbol)
{
    return FindContractFromStrIndexMap(FContractSymbolIndex, Symbol);
}  //CCommodityObject::FindContractFromDeliveryYMIndex()
//------------------------------------------------------------------------------
bool CCommodityObject::InsertContractToSymbolIndex(const UFC::AnsiString& Symbol, CContractObject *ContractPtr)
{
    return InsertContractToStrIndexMap(FContractSymbolIndex, Symbol, ContractPtr);
}  //CCommodityObject::InsertContractToDeliveryYMIndex()
//------------------------------------------------------------------------------
UFC::AnsiString CCommodityObject::ToString()
{
    UFC::AnsiString kindStr = "";
    if (FIsFutures)
        kindStr = "Futures";
    else
        kindStr = "Options";

    UFC::AnsiString aValueStr = " ";
    if (FMarginType == '0')  // Amount
        aValueStr.Printf("AValue[%9.3lf]", FInitialMargin);
    else if (FMarginType == '1')  // Percent
        aValueStr.Printf("AValue[%10.3lf%%]", FInitialMargin);
    else
        aValueStr = " ";
            
    UFC::AnsiString commodityStr = " ";
    if (FContractKind != 'S')
        commodityStr.Printf("%s Commodity[%s]%c Multiplier[%11.4lf] Status[%c] [%c:%s] StkPxDecimal[%d] Decimal[%d] AcceptQuote[%c] BlockTrade[%c] %s", 
                            kindStr.c_str(), FContractCode.c_str(), FContractKind, FContractMultiplier, FStatusCode, FCurrencyType, DescribeCurrency(FCurrencyType).c_str(), FStrikePriceDecimalLocator, FOrderDecimalLocator, FAcceptQuoteFlag, FBlockTradeFlag, aValueStr.c_str());
    else
        commodityStr.Printf("%s Commodity[%s]%c Multiplier[%11.4lf] Status[%c] [%c:%s] StkPxDecimal[%d] Decimal[%d] AcceptQuote[%c] BlockTrade[%c] Stock[%s][%s] %s", 
                            kindStr.c_str(), FContractCode.c_str(), FContractKind, FContractMultiplier, FStatusCode, FCurrencyType, DescribeCurrency(FCurrencyType).c_str(), FStrikePriceDecimalLocator, FOrderDecimalLocator, FAcceptQuoteFlag, FBlockTradeFlag, FStockID.c_str(), FBeginDate.c_str(), aValueStr.c_str());
    return commodityStr;
}  //CCommodityObject::ToString()
//------------------------------------------------------------------------------
void CCommodityObject::DumpData()
{
    UFC::BufferedLog::Printf(" [%s]", ToString().c_str());
}  //CCommodityObject::DumpData()
//------------------------------------------------------------------------------
void CCommodityObject::DumpContract()
{
    UFC::BufferedLog::Printf(" Dump %d Contracts", FContractSymbolIndex.size());
    DumpContractStrIndexMap(FContractSymbolIndex);
}  //CCommodityObject::DumpContract()

//--------------------------- CContractYearMonthSet ----------------------------
//------------------------------------------------------------------------------
UFC::AnsiString CContractYearMonthSet::GeterateContractSetKey(const UFC::AnsiString& ContractCode, char CallPut, const UFC::AnsiString& StrikePrice)
{
    UFC::AnsiString yearMonthKey = "";
    if ((CallPut == 'C') || (CallPut == 'c'))
        yearMonthKey.Printf("%s%sC", ContractCode.c_str(), StrikePrice.c_str());
    else if ((CallPut == 'P') || (CallPut == 'p'))
        yearMonthKey.Printf("%s%sP", ContractCode.c_str(), StrikePrice.c_str());
    else
        yearMonthKey.Printf("%s", ContractCode.c_str());
    return yearMonthKey;
}  //CContractYearMonthSet::GeterateContractSetKey()
//------------------------------------------------------------------------------
CContractYearMonthSet::CContractYearMonthSet(CContractObject *ContractPtr)
:FContractYearMonthKey("")
,FContractCode("")
,FStrikePriceStr(ContractPtr->GetStrikePriceStr())
,FCallPut(ContractPtr->GetCallPut())
{
    FContractCode = ContractPtr->GetSymbol().SubString(0, 3);
    FContractYearMonthKey = GeterateContractSetKey(FContractCode, FCallPut, FStrikePriceStr);
}  //CContractYearMonthSet::CContractYearMonthSet()
//------------------------------------------------------------------------------
CContractYearMonthSet::CContractYearMonthSet(const UFC::AnsiString& ContractCode, char CallPut, const UFC::AnsiString& StrikePriceStr)
:FContractYearMonthKey("")
,FContractCode(ContractCode)
,FStrikePriceStr(StrikePriceStr)
,FCallPut(CallPut)
{
    FContractYearMonthKey = GeterateContractSetKey(FContractCode, FCallPut, FStrikePriceStr);
}  //CContractYearMonthSet::CContractYearMonthSet()
//------------------------------------------------------------------------------
CContractYearMonthSet::~CContractYearMonthSet()
{
    ClearContractStrIndexMap(FContractYMIndex, true);
}  //CContractYearMonthSet::~CContractYearMonthSet()
//------------------------------------------------------------------------------
int CContractYearMonthSet::GetContractYearMonthCount()
{
    return GetContractStrIndexMapCount(FContractYMIndex);    
}  //CContractYearMonthSet::GetContractYearMonthCount()
//------------------------------------------------------------------------------
CContractObject *CContractYearMonthSet::FindContractFromYearMonthIndex(const UFC::AnsiString& DeliveryYearMonth)
{
    return FindContractFromStrIndexMap(FContractYMIndex, DeliveryYearMonth);
}  //CContractYearMonthSet::FindContractFromYearMonthIndex()
//------------------------------------------------------------------------------
bool CContractYearMonthSet::InsertContractToYearMonthIndex(const UFC::AnsiString& DeliveryYearMonth, CContractObject *ContractPtr)
{
    return InsertContractToStrIndexMap(FContractYMIndex, DeliveryYearMonth, ContractPtr);
}  //CContractYearMonthSet::InsertContractToYearMonthIndex()
//------------------------------------------------------------------------------
CContractObject *CContractYearMonthSet::GetFirstYearMonthContract()
{
    return GetFirstContractFromStrIndexMap(FContractYMIndex);
}  //CContractYearMonthSet::GetFirstYearMonthContract()
//------------------------------------------------------------------------------
CContractObject *CContractYearMonthSet::GetSecondYearMonthContract()
{
    return GetSecondContractFromStrIndexMap(FContractYMIndex);
}  //CContractYearMonthSet::GetSecondYearMonthContract()
//------------------------------------------------------------------------------
UFC::AnsiString CContractYearMonthSet::ToString()
{
    UFC::AnsiString contractStr;
    if ((FCallPut == 'C') || (FCallPut == 'P'))
        contractStr.Printf("ContractYMSet[%s] StrikePx[%s] %c", FContractCode.c_str(), FStrikePriceStr.c_str(), FCallPut);
    else
        contractStr.Printf("ContractYMSet[%s]", FContractCode.c_str());
    return contractStr;
}  //CContractYearMonthSet::ToString()

//------------------------------ CContractObject -------------------------------
//------------------------------------------------------------------------------
CContractObject::CContractObject()
:FSymbol("")
,FDeliveryYearMonth("")
,FStrikePriceStr("")
,FStrikePrice(0.0)
,FCallPut('N')
,FBeginDate("")
,FEndDate("")
,FRaisePrice1Str("")
,FRaisePrice1(0.0)
,FFallPrice1Str("")
,FFallPrice1(0.0)
,FPremiumStr("")
,FPremium(0.0)
,FRaisePrice2Str("")
,FRaisePrice2(0.0)
,FFallPrice2Str("")
,FFallPrice2(0.0)
,FRaisePrice3Str("")
,FRaisePrice3(0.0)
,FFallPrice3Str("")
,FFallPrice3(0.0)
,FProductKind(' ')
,FAcceptQuoteFlag('N')
,FDecimalLocator(0)
,FProductSequence(0)
,FContractCode("")
{    
}  //CContractObject::CContractObject()
//------------------------------------------------------------------------------
CContractObject::CContractObject(const char *P08RecBuffer)
:FSymbol("")
,FDeliveryYearMonth("")
,FStrikePriceStr("")
,FStrikePrice(0.0)
,FCallPut('N')
,FBeginDate("")
,FEndDate("")
,FRaisePrice1Str("")
,FRaisePrice1(0.0)
,FFallPrice1Str("")
,FFallPrice1(0.0)
,FPremiumStr("")
,FPremium(0.0)
,FRaisePrice2Str("")
,FRaisePrice2(0.0)
,FFallPrice2Str("")
,FFallPrice2(0.0)
,FRaisePrice3Str("")
,FRaisePrice3(0.0)
,FFallPrice3Str("")
,FFallPrice3(0.0)
,FProductKind(' ')
,FAcceptQuoteFlag('N')
,FDecimalLocator(0)
,FProductSequence(0)
,FContractCode("")
{
    ParseP08Record(P08RecBuffer);
}  //CContractObject::CContractObject()
//------------------------------------------------------------------------------
CContractObject::CContractObject(UFC::TRecord *P08RecPtr)
:FSymbol("")
,FDeliveryYearMonth("")
,FStrikePriceStr("")
,FStrikePrice(0.0)
,FCallPut('N')
,FBeginDate("")
,FEndDate("")
,FRaisePrice1Str("")
,FRaisePrice1(0.0)
,FFallPrice1Str("")
,FFallPrice1(0.0)
,FPremiumStr("")
,FPremium(0.0)
,FRaisePrice2Str("")
,FRaisePrice2(0.0)
,FFallPrice2Str("")
,FFallPrice2(0.0)
,FRaisePrice3Str("")
,FRaisePrice3(0.0)
,FFallPrice3Str("")
,FFallPrice3(0.0)
,FProductKind(' ')
,FAcceptQuoteFlag('N')
,FDecimalLocator(0)
,FProductSequence(0)
,FContractCode("")
{
    ParseP08Record(P08RecPtr);
}  ////CContractObject::CContractObject()
//------------------------------------------------------------------------------
void CContractObject::ParseP08Record(const char *P08RecBuffer)
{
    UFC::AnsiString p08RecStr(P08RecBuffer);
    FSymbol = p08RecStr.SubString(0, 10);
    FSymbol.TrimRight(' ');
    
    FDeliveryYearMonth = p08RecStr.SubString(10, 6);
    
    UFC::AnsiString tempStr = p08RecStr.SubString(16, 9);
    FStrikePriceStr = BuildDoubleStr(tempStr, 4);
    FStrikePrice = FStrikePriceStr.ToDouble();
    
    FCallPut = p08RecStr[25];
    if ((FCallPut != 'C') && (FCallPut != 'P'))
    {
        if (FCallPut == 'c')
            FCallPut = 'C';
        else if (FCallPut == 'p')
            FCallPut = 'P';
        else
            FCallPut = 'N';
    }
    
    FBeginDate = p08RecStr.SubString(26, 8);
    FEndDate = p08RecStr.SubString(34, 8);;

    FDecimalLocator = TranslateDigitalChar(p08RecStr[107]);

    tempStr = p08RecStr.SubString(42, 9);
    FRaisePrice1Str = BuildDoubleStr(tempStr, FDecimalLocator);
    FRaisePrice1 = FRaisePrice1Str.ToDouble();
    
    tempStr = p08RecStr.SubString(51, 9);
    FFallPrice1Str = BuildDoubleStr(tempStr, FDecimalLocator);
    FFallPrice1 = FRaisePrice1Str.ToDouble();
    
    tempStr = p08RecStr.SubString(60, 9);
    FPremiumStr = BuildDoubleStr(tempStr, FDecimalLocator);
    FPremium = FPremiumStr.ToDouble();
    
    tempStr = p08RecStr.SubString(69, 9);
    FRaisePrice2Str = BuildDoubleStr(tempStr, FDecimalLocator);
    FRaisePrice2 = FRaisePrice2Str.ToDouble();
    
    tempStr = p08RecStr.SubString(78, 9);
    FFallPrice2Str = BuildDoubleStr(tempStr, FDecimalLocator);
    FFallPrice2 = FFallPrice2Str.ToDouble();
    
    tempStr = p08RecStr.SubString(87, 9);
    FRaisePrice3Str = BuildDoubleStr(tempStr, FDecimalLocator);
    FRaisePrice3 = FRaisePrice3Str.ToDouble();
    
    tempStr = p08RecStr.SubString(96, 9);
    FFallPrice3Str = BuildDoubleStr(tempStr, FDecimalLocator);
    FFallPrice3 = FFallPrice3Str.ToDouble();
    
    FProductKind = p08RecStr[105];

    FAcceptQuoteFlag = p08RecStr[106];

    tempStr = p08RecStr.SubString(108, 5);
    FProductSequence = tempStr.ToInt();
    
    FContractCode = FSymbol.SubString(0, 3);
}  //CContractObject::ParseP08Record()
//------------------------------------------------------------------------------
void CContractObject::ParseP08Record(UFC::TRecord *P08RecPtr)
{
    UFC::AnsiString logHead = "CContractObject::ParseP08Record()";
    if (P08RecPtr == 0) return;
    UFC::AnsiString fieldName = "CommId";
    if (P08RecPtr->GetField(fieldName, FSymbol)) 
        FSymbol.TrimRight(' ');
    else
        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
        
    fieldName = "SettMonth";
    if (!P08RecPtr->GetField(fieldName, FDeliveryYearMonth)) 
        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
    
    UFC::AnsiString tempStr = "";
    fieldName = "StrikePrice";    
    if (P08RecPtr->GetField(fieldName, tempStr)) 
    {
        FStrikePriceStr = BuildDoubleStr(tempStr, 4);
        FStrikePrice = FStrikePriceStr.ToDouble();
    }
    else
        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());

    fieldName = "CP";
    if (P08RecPtr->GetField(fieldName, tempStr)) 
    {
        FCallPut = tempStr[0];
        if ((FCallPut != 'C') && (FCallPut != 'P'))
        {
            if (FCallPut == 'c')
                FCallPut = 'C';
            else if (FCallPut == 'p')
                FCallPut = 'P';
            else
                FCallPut = 'N';
        }
    }
    else
        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
    
    fieldName = "BeginDate";
    if (!P08RecPtr->GetField(fieldName, FBeginDate)) 
        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
    
    fieldName = "EndDate";
    if (!P08RecPtr->GetField(fieldName, FEndDate)) 
        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());

    fieldName = "DecimalLocator";
    if (P08RecPtr->GetField(fieldName, tempStr)) 
        FDecimalLocator = TranslateDigitalChar(tempStr[0]);
    else
        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());

//    fieldName = "UpperPrice";
//    if (P08RecPtr->GetField(fieldName, tempStr)) 
//    {
//        FRaisePrice1Str = BuildDoubleStr(tempStr, FDecimalLocator);
//        FRaisePrice1 = FRaisePrice1Str.ToDouble();
//    }
//    else
//        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
    
//    fieldName = "LowerPrice";
//    if (P08RecPtr->GetField(fieldName, tempStr)) 
//    {
//        FFallPrice1Str = BuildDoubleStr(tempStr, FDecimalLocator);
//        FFallPrice1 = FFallPrice1Str.ToDouble();
//    }
//    else
//        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
    
    fieldName = "Premium";
    if (P08RecPtr->GetField(fieldName, tempStr)) 
    {
        FPremiumStr = BuildDoubleStr(tempStr, FDecimalLocator);
        FPremium = FPremiumStr.ToDouble();
    }
    else
        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
    
//    fieldName = "RaisePrice2";
//    if (P08RecPtr->GetField(fieldName, tempStr)) 
//    {
//        FRaisePrice2Str = BuildDoubleStr(tempStr, FDecimalLocator);
//        FRaisePrice2 = FRaisePrice2Str.ToDouble();
//    }
//    else
//        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
    
//    fieldName = "FallPrice2";
//    if (P08RecPtr->GetField(fieldName, tempStr)) 
//    {
//        FFallPrice2Str = BuildDoubleStr(tempStr, FDecimalLocator);
//        FFallPrice2 = FFallPrice2Str.ToDouble();
//    }
//    else
//        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
    
//    fieldName = "RaisePrice3";
//    if (P08RecPtr->GetField(fieldName, tempStr)) 
//    {
//        FRaisePrice3Str = BuildDoubleStr(tempStr, FDecimalLocator);
//        FRaisePrice3 = FRaisePrice3Str.ToDouble();
//    }
//    else
//        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
    
//    fieldName = "FallPrice3";
//    if (P08RecPtr->GetField(fieldName, tempStr)) 
//    {
//        FFallPrice3Str = BuildDoubleStr(tempStr, FDecimalLocator);
//        FFallPrice3 = FFallPrice3Str.ToDouble();
//    }
//    else
//        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
    
    fieldName = "ProdKind";
    if (P08RecPtr->GetField(fieldName, tempStr)) 
        FProductKind = tempStr[0];
    else
        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());

    fieldName = "AcceptQuoteFlag";
    if (P08RecPtr->GetField(fieldName, tempStr)) 
        FAcceptQuoteFlag = tempStr[0];
    else
        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());

    
    fieldName = "PSeq";
    if (P08RecPtr->GetField(fieldName, tempStr)) 
        FProductSequence = tempStr.ToInt();
    else
        UFC::BufferedLog::Printf(" %s Get field[%s] failed", logHead.c_str(), fieldName.c_str());
    
    FContractCode = FSymbol.SubString(0, 3);    
}  //CContractObject::ParseP08Record()
//------------------------------------------------------------------------------
UFC::AnsiString CContractObject::ToString()
{
    UFC::AnsiString contractStr;
    if ((FCallPut == 'C') || (FCallPut == 'P'))
        contractStr.Printf("Contract[%s] YM[%s] StrikePx[%s][%c] ProdID[%c] Decimal[%d] raisePx1[%s][%10.*lf]", FSymbol.c_str(), FDeliveryYearMonth.c_str(), FStrikePriceStr.c_str(), FCallPut, FProductKind, FDecimalLocator, FRaisePrice1Str.c_str(), FDecimalLocator, FRaisePrice1);
    else
        contractStr.Printf("Contract[%s] YM[%s] ProdID[%c] Decimal[%d] raisePx1[%s][%10.*lf]", FSymbol.c_str(), FDeliveryYearMonth.c_str(), FProductKind, FDecimalLocator, FRaisePrice1Str.c_str(), FDecimalLocator, FRaisePrice1);
    return contractStr;
}  //CContractObject::ToString()

//-------------------------------- AccountInfo ---------------------------------
Int32 AccountInfo::FMaxQuoteOrderQty = 25;
//------------------------------------------------------------------------------
UFC::AnsiString AccountInfo::BuildAccountKey( const UFC::AnsiString& Account, const UFC::AnsiString& AE, char CurrencyType )
{
    UFC::AnsiString accountKey;
    accountKey.Printf( "%s%s%c", Account.c_str(), AE.c_str(), CurrencyType );
    return accountKey;
}  //AccountInfo::BuildAccountKey()
//------------------------------------------------------------------------------
AccountInfo::AccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE )
:FAE( AE )
,FAccount( Account )
,FOrderAmount( 0.0 )
,FQuoteOrderAmount( 0.0 )
,FMatchAmount( 0.0 )
,FQuoteMatchAmount( 0.0 )
,FQuoteMatchQty( 0 )
,FCanUse( 0.0 )
,FCurrencyCode( "" )
,FCurrencyType( 0 )
,FAEPtr(0)
{
}  //AccountInfo::AccountInfo()
//------------------------------------------------------------------------------
AccountInfo::AccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE, const UFC::AnsiString& CurrencyCode )
:FAE( AE )
,FAccount( Account )
,FOrderAmount( 0.0 )
,FQuoteOrderAmount( 0.0 )
,FMatchAmount( 0.0 )
,FQuoteMatchAmount( 0.0 )
,FQuoteMatchQty( 0 )
,FCanUse( 0.0 )
,FCurrencyCode( CurrencyCode )
,FCurrencyType( 0 )
,FAEPtr( 0 )
{
    if( FCurrencyCode == "TWA" )
        FCurrencyType = '1';
    else if( FCurrencyCode == "USA" )
        FCurrencyType = '2';
    else if( FCurrencyCode == "RMB" )
        FCurrencyType = '8';        
}  //AccountInfo::AccountInfo()
//------------------------------------------------------------------------------
AccountInfo::AccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE, char CurrencyType )
:FAE( AE )
,FAccount( Account )
,FOrderAmount( 0.0 )
,FQuoteOrderAmount( 0.0 )
,FMatchAmount( 0.0 )
,FQuoteMatchAmount( 0.0 )
,FQuoteMatchQty( 0 )
,FCanUse( 0.0 )
,FCurrencyCode( "" )
,FCurrencyType( CurrencyType )
,FAEPtr( 0 )
{
}  //AccountInfo::AccountInfo()
//------------------------------------------------------------------------------
UFC::AnsiString AccountInfo::GetAE()
{
    UFC::PLockObject Lock( FCS );
    return FAE;
}  //AccountInfo::GetAE()
//------------------------------------------------------------------------------
double AccountInfo::GetOrderAmount()
{
    UFC::PLockObject Lock( FCS );
    return FOrderAmount;
}  //AccountInfo::GetOrderAmount()
//------------------------------------------------------------------------------
void AccountInfo::SetOrderAmount( double OrderAmount )
{
    UFC::PLockObject Lock( FCS );
    FOrderAmount = OrderAmount;
}  //AccountInfo::SetOrderAmount()
//------------------------------------------------------------------------------
double AccountInfo::GetQuoteOrderAmount()
{
    UFC::PLockObject Lock( FCS );
    return FQuoteOrderAmount;
}  //AccountInfo::GetQuoteOrderAmount()
//------------------------------------------------------------------------------
void AccountInfo::SetQuoteOrderAmount( double OrderAmount )
{
    UFC::PLockObject Lock( FCS );
    FQuoteOrderAmount = OrderAmount;
}  //AccountInfo::SetQuoteOrderAmount()
//------------------------------------------------------------------------------
double AccountInfo::GetMatchAmount()
{
    UFC::PLockObject Lock( FCS );
    return FMatchAmount;
}  //AccountInfo::GetMatchAmount()
//------------------------------------------------------------------------------
void AccountInfo::SetMatchAmount( double MatchAmount )
{
    UFC::PLockObject Lock( FCS );
    FMatchAmount = MatchAmount;
}  //AccountInfo::SetMatchAmount()
//------------------------------------------------------------------------------
double AccountInfo::GetQuoteMatchAmount()
{
    UFC::PLockObject Lock( FCS );
    return FQuoteMatchAmount;
}  //AccountInfo::GetQuoteMatchAmount()
//------------------------------------------------------------------------------
void AccountInfo::SetQuoteMatchAmount( double MatchAmount )
{
    UFC::PLockObject Lock( FCS );
    FQuoteMatchAmount = MatchAmount;
}  //AccountInfo::SetQuoteMatchAmount()
//------------------------------------------------------------------------------
double AccountInfo::GetCanUse()
{
    UFC::PLockObject Lock( FCS );
    return FCanUse;
}  //AccountInfo::GetCanUse()
//------------------------------------------------------------------------------
void AccountInfo::SetCanUse( double CanUse )
{
    UFC::PLockObject Lock( FCS );
    FCanUse = CanUse;
}  //AccountInfo::SetCanUse()
//------------------------------------------------------------------------------
UFC::AnsiString AccountInfo::GetCurrencyCode()
{
    UFC::PLockObject Lock( FCS );
    return FCurrencyCode;
}  //AccountInfo::GetCurrencyCode()
//------------------------------------------------------------------------------
void AccountInfo::SetCurrencyCode( const UFC::AnsiString& NewValue )
{
    UFC::PLockObject Lock( FCS );
    FCurrencyCode = NewValue;
}  //AccountInfo::SetCurrencyCode()
//------------------------------------------------------------------------------
char AccountInfo::GetCurrencyType()
{
    UFC::PLockObject Lock( FCS );
    return FCurrencyType;
}  //AccountInfo::GetCurrencyType()
//------------------------------------------------------------------------------
void AccountInfo::SetCurrencyType( char NewValue )
{
    UFC::PLockObject Lock( FCS );
    FCurrencyType = NewValue;
}  //AccountInfo::SetCurrencyType()
//------------------------------------------------------------------------------
CAccountExecutive *AccountInfo::GetAEObj()
{
    UFC::PLockObject Lock( FCS );
    return FAEPtr;
}  //AccountInfo::GetAEObj()
//------------------------------------------------------------------------------
void AccountInfo::SetAEObj(CAccountExecutive *AEPtr)
{
    UFC::PLockObject Lock( FCS );
    FAEPtr = AEPtr;
}  //AccountInfo::SetAEObj()
//------------------------------------------------------------------------------
int AccountInfo::GetQuoteMatchQty()
{
    UFC::PLockObject Lock( FCS );
    return FQuoteMatchQty;
}  //AccountInfo::GetQuoteMatchQty()
//------------------------------------------------------------------------------
void AccountInfo::SetQuoteMatchQty(int QuoteMatchQty)
{
    UFC::PLockObject Lock( FCS );
    FQuoteMatchQty = QuoteMatchQty;
}  //AccountInfo::SetQuoteMatchQty()
//------------------------------------------------------------------------------
void AccountInfo::AddQuoteMatchQty(int MatchQty)
{
    UFC::PLockObject Lock( FCS );
    FQuoteMatchQty += MatchQty;
}  //AccountInfo::AddQuoteMatchQty()
//------------------------------------------------------------------------------
BOOL AccountInfo::CanPlaceOrder( double NewOrderAmount )
{
    BOOL checkResult = FALSE;
    UFC::PLockObject Lock( FCS );
    double restMargin = FCanUse - FOrderAmount;
    if( restMargin < NewOrderAmount )
        UFC::BufferedLog::Printf( " Reject! Margin not enough. Need[%0.4lf], restMargin[%0.4lf] = CanUse[%0.4lf] - Order[%0.4lf], Match[%0.4lf]", NewOrderAmount, restMargin, FCanUse, FOrderAmount, FMatchAmount);
    else
    {
        UFC::BufferedLog::Printf( " Accept! Margin is enough. Need[%0.4lf], restMargin[%0.4lf] = CanUse[%0.4lf] - Order[%0.4lf], Match[%0.4lf]", NewOrderAmount, restMargin, FCanUse, FOrderAmount, FMatchAmount);
        FOrderAmount += NewOrderAmount;
        UFC::BufferedLog::Printf( " leaves Margin[%0.4lf]", restMargin - NewOrderAmount );
        checkResult = TRUE;
    }
    return checkResult;
}  //AccountInfo::CanPlaceOrder()
//------------------------------------------------------------------------------
bool AccountInfo::CanPlaceFuturesOrder(int OrderQty, double CommodMargins, bool IsMultiLeg, const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol, char Side, double Price, const UFC::AnsiString& OrigAccount)
{
    double reserveDeposit = 0.0;
    if (FAEPtr != 0) reserveDeposit = FAEPtr->GetReserveDeposit();

    double newOrderAmount = CommodMargins * static_cast<double>(OrderQty);
    if (!IsMultiLeg) 
        UFC::BufferedLog::Printf(" Need[%0.4lf] = CommodMargins[%0.4lf] * Qty[%d]", newOrderAmount, CommodMargins, OrderQty);
    else
        UFC::BufferedLog::Printf(" MultiLeg Need[%0.4lf] = CommodMargins[%0.4lf] * Qty[%d]", newOrderAmount, CommodMargins, OrderQty);
    
    UFC::AnsiString outputAccount;
    if (OrigAccount.Length() > 0)
        outputAccount = OrigAccount;
    else
        outputAccount = FAccount;
    double restMargin = FCanUse - reserveDeposit - FOrderAmount - FQuoteOrderAmount;
    double leavesMargin = restMargin - newOrderAmount;
    UFC::AnsiString checkLogStr;
    UFC::UDateTime checkTime;
    if( leavesMargin < 0.0 )
    {
        UFC::BufferedLog::Printf(" Reject! Margin not enough. Need[%0.4lf], restMargin[%0.4lf] = CanUse[%0.4lf] - reserveDeposit[%0.4lf] - Order[%0.4lf] - QuoteOrder[%0.4lf], Match[%0.4lf]", 
                                 newOrderAmount, restMargin, FCanUse, reserveDeposit, FOrderAmount, FQuoteOrderAmount, FMatchAmount);
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  %c  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Reject\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), Side, Price, OrderQty, restMargin, newOrderAmount, reserveDeposit, restMargin, 0, 0);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        return false;
    }
    else
    {
        UFC::BufferedLog::Printf(" Accept! Margin is enough. Need[%0.4lf], restMargin[%0.4lf] = CanUse[%0.4lf] - reserveDeposit[%0.4lf] - Order[%0.4lf] - QuoteOrder[%0.4lf], Match[%0.4lf]", 
                                 newOrderAmount, restMargin, FCanUse, reserveDeposit, FOrderAmount, FQuoteOrderAmount, FMatchAmount);
        FOrderAmount += newOrderAmount;
        UFC::BufferedLog::Printf(" leaves Margin[%0.4lf]", leavesMargin);
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  %c  %16.4lf %5d %116.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Accept\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), Side, Price, OrderQty, restMargin, newOrderAmount, reserveDeposit, leavesMargin, 0, 0);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        return true;
    }
}  //AccountInfo::CanPlaceFuturesOrder()
//------------------------------------------------------------------------------
bool AccountInfo::CanPlaceFuturesQuoteOrder(int BidOrderQty, double BidCommodMargins, int AskOrderQty, double AskCommodMargins, const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol, double BidPrice, double AskPrice, const UFC::AnsiString& OrigAccount)
{
    UFC::AnsiString outputAccount;
    if (OrigAccount.Length() > 0)
        outputAccount = OrigAccount;
    else
        outputAccount = FAccount;
    UFC::UDateTime checkTime;
    UFC::AnsiString checkLogStr;
    double restMargin = FCanUse - FOrderAmount - FQuoteOrderAmount;
    double reserveDeposit = 0.0;
    int quoteMatchQtyLimit = 0;
    if (FAEPtr != 0)
    {
        reserveDeposit = FAEPtr->GetReserveDeposit();
        quoteMatchQtyLimit = FAEPtr->GetQuoteMatchQtyLimit();
        restMargin -= reserveDeposit;
        if (FQuoteMatchQty >= quoteMatchQtyLimit)
        {
            UFC::BufferedLog::Printf( " Reject! QuoteMatchQty[%d] Exceed LimitQty[%d]", FQuoteMatchQty, quoteMatchQtyLimit);
            checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  B  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Reject\n",
                               outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                               Symbol.c_str(), BidPrice, BidOrderQty, restMargin, 0.0, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
            if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
            checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  S  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Reject\n",
                               outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                               Symbol.c_str(), AskPrice, AskOrderQty, restMargin, 0.0, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
            if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
            return false;
        }
        else
        {
            if ((BidOrderQty <= FMaxQuoteOrderQty) && (AskOrderQty <= FMaxQuoteOrderQty))
            {
                UFC::BufferedLog::Printf(" Accept! QuoteMatchQty[%d] < LimitQty[%d]", FQuoteMatchQty, quoteMatchQtyLimit);
                checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  B  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Accept\n",
                               outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                               Symbol.c_str(), BidPrice, BidOrderQty, restMargin, 0.0, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
                if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
                checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  S  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Accept\n",
                                outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                                Symbol.c_str(), AskPrice, AskOrderQty, restMargin, 0.0, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
                if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
                return true;
            }
        }
    }  //if (FAEPtr != 0)
    
    int bidNormalOrderQty = 0;
    if (BidOrderQty > FMaxQuoteOrderQty ) bidNormalOrderQty = BidOrderQty - FMaxQuoteOrderQty;
    int askNormalOrderQty = 0;
    if (AskOrderQty > FMaxQuoteOrderQty ) askNormalOrderQty = AskOrderQty - FMaxQuoteOrderQty;

    double bidOrderAmount = BidCommodMargins * static_cast<double>(bidNormalOrderQty);
    double askOrderAmount = AskCommodMargins * static_cast<double>(askNormalOrderQty);
    double newOrderAmount = bidOrderAmount + askOrderAmount;
    UFC::BufferedLog::Printf( " Need[%0.4lf] = Bid(CommodMargins[%0.4lf] * Qty[%d]) + Ask(CommodMargins[%0.4lf] * Qty[%d])", 
                              newOrderAmount, BidCommodMargins, bidNormalOrderQty, AskCommodMargins, askNormalOrderQty);
    double leavesMargin = restMargin - newOrderAmount;
    if( leavesMargin < 0.0 )
    {
        UFC::BufferedLog::Printf( " Reject! Margin not enough. Need[%0.4lf], restMargin[%0.4lf] = CanUse[%0.4lf] - reserveDeposit[%0.4lf] - Order[%0.4lf] - QuoteOrder[%0.4lf], Match[%0.4lf]", 
                                  newOrderAmount, restMargin, FCanUse, reserveDeposit, FOrderAmount, FQuoteOrderAmount, FMatchAmount);
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  B  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Reject\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), BidPrice, BidOrderQty, restMargin, newOrderAmount, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  S  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Reject\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), AskPrice, AskOrderQty, restMargin, newOrderAmount, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        return false;
    }
    else
    {
        UFC::BufferedLog::Printf( " Accept! Margin is enough. Need[%0.4lf], restMargin[%0.4lf] = CanUse[%0.4lf] - reserveDeposit[%0.4lf] - Order[%0.4lf] - QuoteOrder[%0.4lf], Match[%0.4lf]", 
                                  newOrderAmount, restMargin, FCanUse, reserveDeposit, FOrderAmount, FQuoteOrderAmount, FMatchAmount);
        FQuoteOrderAmount += newOrderAmount;
        UFC::BufferedLog::Printf( " leaves Margin[%0.3lf]", leavesMargin );
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  B  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Accept\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), BidPrice, BidOrderQty, restMargin, newOrderAmount, reserveDeposit, leavesMargin, quoteMatchQtyLimit, FQuoteMatchQty);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  S  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Accept\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), AskPrice, AskOrderQty, restMargin, newOrderAmount, reserveDeposit, leavesMargin, quoteMatchQtyLimit, FQuoteMatchQty);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        return true;
    }
}  //AccountInfo::CanPlaceFuturesQuoteOrder()
//------------------------------------------------------------------------------
bool AccountInfo::CanPlaceOptionsOrder(int OrderQty, double CommodMargins, bool IsMultiLeg, const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol, char Side, double Price, const UFC::AnsiString& OrigAccount)
{
    double reserveDeposit = 0.0;
    if (FAEPtr != 0) reserveDeposit = FAEPtr->GetReserveDeposit();
    
    double newOrderAmount = CommodMargins * static_cast<double>(OrderQty);
    if (!IsMultiLeg)
        UFC::BufferedLog::Printf( " Need[%0.4lf] = CommodMargins[%0.4lf] * Qty[%d]", newOrderAmount, CommodMargins, OrderQty);
    else
        UFC::BufferedLog::Printf(" MultiLeg Need[%0.4lf] = CommodMargins[%0.4lf] * Qty[%d]", newOrderAmount, CommodMargins, OrderQty);
    
    UFC::AnsiString outputAccount;
    if (OrigAccount.Length() > 0)
        outputAccount = OrigAccount;
    else
        outputAccount = FAccount;
    double restMargin = FCanUse - reserveDeposit - FOrderAmount - FQuoteOrderAmount;
    double leavesMargin = restMargin - newOrderAmount;
    UFC::AnsiString checkLogStr;
    UFC::UDateTime checkTime;
    if (leavesMargin < 0.0)
    {
        UFC::BufferedLog::Printf( " Reject! Margin not enough. Need[%0.4lf], restMargin[%0.4lf] = CanUse[%0.4lf] - reserveDeposit[%0.4lf] - Order[%0.4lf] - QuoteOrder[%0.4lf], Match[%0.4lf]", 
                                  newOrderAmount, restMargin, FCanUse, reserveDeposit, FOrderAmount, FQuoteOrderAmount, FMatchAmount);
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  %c  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Reject\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), Side, Price, OrderQty, restMargin, newOrderAmount, reserveDeposit, restMargin, 0, 0);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
         return false;
    }
    else
    {
        UFC::BufferedLog::Printf( " Accept! Margin is enough. Need[%0.4lf], restMargin[%0.4lf] = CanUse[%0.4lf] - reserveDeposit[%0.4lf] - Order[%0.4lf] - QuoteOrder[%0.4lf], Match[%0.4lf]", 
                                  newOrderAmount, restMargin, FCanUse, reserveDeposit, FOrderAmount, FQuoteOrderAmount, FMatchAmount);
        FOrderAmount += newOrderAmount;
        UFC::BufferedLog::Printf( " leaves Margin[%0.3lf]", leavesMargin );
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  %c  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Accept\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), Side, Price, OrderQty, restMargin, newOrderAmount, reserveDeposit, leavesMargin, 0, 0);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        return true;
    }
}  //AccountInfo::CanPlaceOptionsOrder()
//------------------------------------------------------------------------------
bool AccountInfo::CanPlaceOptionsQuoteOrder(int BidOrderQty, double BidCommodMargins, int AskOrderQty, double AskCommodMargins, const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol, double BidPrice, double AskPrice, const UFC::AnsiString& OrigAccount)
{
    UFC::AnsiString outputAccount;
    if (OrigAccount.Length() > 0)
        outputAccount = OrigAccount;
    else
        outputAccount = FAccount;
    UFC::UDateTime checkTime;
    UFC::AnsiString checkLogStr;
    double restMargin = FCanUse - FOrderAmount - FQuoteOrderAmount;
    double reserveDeposit = 0.0;
    int quoteMatchQtyLimit = 0;
    if (FAEPtr != 0)
    {
        reserveDeposit = FAEPtr->GetReserveDeposit();
        quoteMatchQtyLimit = FAEPtr->GetQuoteMatchQtyLimit();
        restMargin -= reserveDeposit;
        if (FQuoteMatchQty >= quoteMatchQtyLimit)
        {
            UFC::BufferedLog::Printf( " Reject! QuoteMatchQty[%d] Exceed LimitQty[%d]", FQuoteMatchQty, quoteMatchQtyLimit);
            checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  B  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Reject\n",
                               outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                               Symbol.c_str(), BidPrice, BidOrderQty, restMargin, 0.0, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
            if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
            checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  S  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Reject\n",
                               outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                               Symbol.c_str(), AskPrice, AskOrderQty, restMargin, 0.0, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
            if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
            return false;
        }
        else
        {
            if ((BidOrderQty <= FMaxQuoteOrderQty) && (AskOrderQty <= FMaxQuoteOrderQty))
            {
                UFC::BufferedLog::Printf(" Accept! QuoteMatchQty[%d] < LimitQty[%d]", FQuoteMatchQty, quoteMatchQtyLimit);
                checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  B  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Accept\n",
                                   outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                                   Symbol.c_str(), BidPrice, BidOrderQty, restMargin, 0.0, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
                if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
                checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  S  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Accept\n",
                                   outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                                   Symbol.c_str(), AskPrice, AskOrderQty, restMargin, 0.0, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
                if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
                return true;
            }
        }
        reserveDeposit = FAEPtr->GetReserveDeposit();
    }  //if (FAEPtr != 0)
    
    int bidNormalOrderQty = 0;
    if (BidOrderQty > FMaxQuoteOrderQty ) bidNormalOrderQty = BidOrderQty - FMaxQuoteOrderQty;
    int askNormalOrderQty = 0;
    if (AskOrderQty > FMaxQuoteOrderQty ) askNormalOrderQty = AskOrderQty - FMaxQuoteOrderQty;

    double bidOrderAmount = BidCommodMargins * static_cast<double>(bidNormalOrderQty);
    double askOrderAmount = AskCommodMargins * static_cast<double>(askNormalOrderQty);
    double newOrderAmount = bidOrderAmount + askOrderAmount;
    UFC::BufferedLog::Printf( " Need[%0.4lf] = Bid(CommodMargins[%0.4lf] * Qty[%d]) + Ask(CommodMargins[%0.4lf] * Qty[%d])", 
                              newOrderAmount, BidCommodMargins, bidNormalOrderQty, AskCommodMargins, askNormalOrderQty);
    double leavesMargin = restMargin - newOrderAmount;
    if( leavesMargin < 0.0 )
    {
        UFC::BufferedLog::Printf( " Reject! Margin not enough. Need[%0.4lf], restMargin[%0.4lf] = CanUse[%0.4lf] - reserveDeposit[%0.4lf] - Order[%0.4lf] - QuoteOrder[%0.4lf], Match[%0.4lf]", 
                                  newOrderAmount, restMargin, FCanUse, reserveDeposit, FOrderAmount, FQuoteOrderAmount, FMatchAmount);
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  B  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Reject\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), BidPrice, BidOrderQty, restMargin, newOrderAmount, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  S  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Reject\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), AskPrice, AskOrderQty, restMargin, newOrderAmount, reserveDeposit, restMargin, quoteMatchQtyLimit, FQuoteMatchQty);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        return false;
    }
    else
    {
        UFC::BufferedLog::Printf( " Accept! Margin is enough. Need[%0.4lf], restMargin[%0.4lf] = CanUse[%0.4lf] - reserveDeposit[%0.4lf] - Order[%0.4lf] - QuoteOrder[%0.4lf], Match[%0.4lf]", 
                                  newOrderAmount, restMargin, FCanUse, reserveDeposit, FOrderAmount, FQuoteOrderAmount, FMatchAmount);
        FQuoteOrderAmount += newOrderAmount;
        UFC::BufferedLog::Printf( " leaves Margin[%0.4lf]", leavesMargin);
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  B  %16.4f %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Accept\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), BidPrice, BidOrderQty, restMargin, newOrderAmount, reserveDeposit, leavesMargin, quoteMatchQtyLimit, FQuoteMatchQty);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  S  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Accept\n",
                           outputAccount.c_str(), FAE.c_str(), FCurrencyType, OrderID.c_str(), checkTime.getHour(), checkTime.getMinute(), checkTime.getSecond(), checkTime.getMillisecond(), 
                           Symbol.c_str(), AskPrice, AskOrderQty, restMargin, newOrderAmount, reserveDeposit, leavesMargin, quoteMatchQtyLimit, FQuoteMatchQty);
        if (GCRMConnection::FCheckLogPtr != 0) GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        return true;
    }
}  //AccountInfo::CanPlaceOptionsQuoteOrder()
//------------------------------------------------------------------------------
void AccountInfo::NewOrder( double NewOrderAmount )
{
    FOrderAmount += NewOrderAmount;
}  //AccountInfo::NewOrder()
//------------------------------------------------------------------------------
void AccountInfo::NewQuoteOrder( double NewOrderAmount )
{
    FQuoteOrderAmount += NewOrderAmount;
}  //AccountInfo::NewQuoteOrder()
//------------------------------------------------------------------------------
void AccountInfo::CancelOrder( double CancelAmount )
{
    UFC::PLockObject Lock( FCS );
    if (FOrderAmount >= CancelAmount)
        FOrderAmount -= CancelAmount;
    else
        FOrderAmount = 0.0;
}  //AccountInfo::CancelOrder()
//------------------------------------------------------------------------------
void AccountInfo::CancelQuoteOrder( double CancelAmount )
{
    UFC::PLockObject Lock( FCS );
    if (FQuoteOrderAmount >= CancelAmount)
        FQuoteOrderAmount -= CancelAmount;
    else
        FQuoteOrderAmount = 0.0;
}  //AccountInfo::CancelOrder()
//------------------------------------------------------------------------------
void AccountInfo::FilledOrder( double MatchAmount )
{
    UFC::PLockObject Lock( FCS );
    FMatchAmount += MatchAmount;
}  //AccountInfo::FilledOrder()
//------------------------------------------------------------------------------
void AccountInfo::FilledQuoteOrder( int MatchQty )
{
    UFC::PLockObject Lock( FCS );
    FQuoteMatchQty += MatchQty;
}  //AccountInfo::FilledQuoteOrder()
//------------------------------------------------------------------------------
void AccountInfo::FilledQuoteOrder( double MatchAmount )
{
    UFC::PLockObject Lock( FCS );
    FQuoteMatchAmount += MatchAmount;
}  //AccountInfo::FilledQuoteOrder()
//------------------------------------------------------------------------------
UFC::AnsiString AccountInfo::ToString()
{
    UFC::AnsiString accountStr;
    accountStr.Printf( " AccountInformation[%s][%s]%c Amount: Order[%0.4lf] QuoteOrder[%0.4lf] Match[%0.4lf] QuoteMatch[%0.4lf] CanUse[%0.4lf] QuoteMatchQty[%d]", FAccount.c_str(), FAE.c_str(), FCurrencyType, FOrderAmount, FQuoteOrderAmount, FMatchAmount, FQuoteMatchAmount, FCanUse, FQuoteMatchQty );
    return accountStr;
}  //AccountInfo::ToString()
//------------------------------------------------------------------------------
void AccountInfo::DumpData()
{
    UFC::PLockObject Lock( FCS );
    UFC::BufferedLog::Printf( " %s", ToString().c_str() );
}  //AccountInfo::DumpData()

//----------------------------- CAccountExecutive ------------------------------
UFC::AnsiString CAccountExecutive::BuildAccountExecutiveKey(const UFC::AnsiString& AE, char CurrencyType)
{
    UFC::AnsiString objKey = "";
    objKey.Printf("%s%c", AE.c_str(), CurrencyType);
    return objKey;
}  //CAccountExecutive::BuildAccountExecutiveKey()
//------------------------------------------------------------------------------
CAccountExecutive::CAccountExecutive(const UFC::AnsiString& AE, char CurrencyType)
:FAE(AE)
,FCurrencyType(CurrencyType)
,FCurrencyCode("")
,FIsOnlyNearMonth(false)
,FReserveDeposit(0.0)
,FQuoteMatchQtyLimit(20)
{
}  //CAccountExecutive::CAccountExecutive()
//------------------------------------------------------------------------------
UFC::AnsiString CAccountExecutive::ToString()
{
    UFC::AnsiString aeStr;
    if (FIsOnlyNearMonth)
        aeStr.Printf( " SubAccount[%s][%c] ReserveDeposit:[%16.4lf] QuoteOrderQtyLimit[%d] OnlyNearMonth", FAE.c_str(), FCurrencyType, FReserveDeposit, FQuoteMatchQtyLimit );
    else
        aeStr.Printf( " SubAccount[%s][%c] ReserveDeposit:[%16.4lf] QuoteOrderQtyLimit[%d] ", FAE.c_str(), FCurrencyType, FReserveDeposit, FQuoteMatchQtyLimit );
    return aeStr;
}
//------------------------------------------------------------------------------
void CAccountExecutive::DumpData()
{
    UFC::BufferedLog::Printf( " %s", ToString().c_str() );
}  //CAccountExecutive::DumpData()

//------------------------------- CBasicRequest --------------------------------
//------------------------------------------------------------------------------
CBasicRequest::CBasicRequest()
:UFC::CMainKeyObject(UFC::cstCreateLock)
,FMarket(nsOrderMessageDefine::mES)
,FNetworkID(0)
,FTelegram("")
,FUserKey("")
,FSubSystemName("")
,FFunctionCode("")
,FMessageType("")
,FMessageTime("")
,FStatusCode("")
,FFCMNo("")
,FOrderNo("")
,FInvestorAcNo("")
,FInvestorFlag(0)
,FProdID("")
,FOrderCondition(0)
,FOpenOffsetFlag(0)
,FClearMeneberID("")
{
}  //CBasicRequest::CBasicRequest()
//------------------------------------------------------------------------------
CBasicRequest::CBasicRequest(nsOrderMessageDefine::MarketEnum Market, Int32 NetworkID, const UFC::AnsiString& Telegram, const UFC::AnsiString& UserKey)
:UFC::CMainKeyObject(UFC::cstCreateLock)
,FMarket(Market)
,FNetworkID(NetworkID)
,FTelegram(Telegram)
,FUserKey(UserKey)
,FSubSystemName("")
,FFunctionCode("")
,FMessageType("")
,FMessageTime("")
,FStatusCode("")
,FFCMNo("")
,FOrderNo("")
,FInvestorAcNo("")
,FInvestorFlag(0)
,FProdID("")
,FOrderCondition(0)
,FOpenOffsetFlag(0)
,FClearMeneberID("")
{
}  //CBasicRequest::CBasicRequest()
//------------------------------------------------------------------------------
void CBasicRequest::ParseHeadTelegram(const UFC::AnsiString& Telegram)
{
    int telegramLen = Telegram.Length();
    if (telegramLen >= 2) FSubSystemName = Telegram.SubString(0, 2);
    if (telegramLen >= 4) FFunctionCode = Telegram.SubString(2, 2);
    if (telegramLen >= 6) FMessageType = Telegram.SubString(4, 2);
    if (telegramLen >= 12) FMessageTime = Telegram.SubString(6, 6);
    if (telegramLen >= 14) FStatusCode = Telegram.SubString(12, 2);
}  //CBasicRequest::ParseHeadTelegram()
//------------------------------------------------------------------------------
void CBasicRequest::ParseBodyTelegram(const UFC::AnsiString& Telegram)
{
}  //CBasicRequest::ParseBodyTelegram()

//------------------------------- COrderRequest --------------------------------
//------------------------------------------------------------------------------
COrderRequest::COrderRequest()
:CBasicRequest()
,FBuySellCode(0)
,FOrderType(0)
,FOrderPriceStr("")
,FOrderPrice(0.0)
,FOrderQuantity(0)
{
}  //COrderRequest::COrderRequest()
//------------------------------------------------------------------------------
COrderRequest::COrderRequest(nsOrderMessageDefine::MarketEnum Market, Int32 NetworkID, const UFC::AnsiString& Telegram, const UFC::AnsiString& UserKey)
:CBasicRequest(Market, NetworkID, Telegram, UserKey)
,FBuySellCode(0)
,FOrderType(0)
,FOrderPriceStr("")
,FOrderPrice(0.0)
,FOrderQuantity(0)
{
    ParseHeadTelegram(FTelegram);
    ParseBodyTelegram(FTelegram);
}  //COrderRequest::COrderRequest()
//------------------------------------------------------------------------------
void COrderRequest::ParseBodyTelegram(const UFC::AnsiString& Telegram)
{
    int telegramLen = Telegram.Length();
    UFC::AnsiString tempStr;
    if (telegramLen >= 21)
    {
        FFCMNo = Telegram.SubString(14, 7);
        FFCMNo.TrimRight(' ');
    }
    
    if (telegramLen >= 26)
    {
        FOrderNo = Telegram.SubString(21, 5);
        FOrderNo.TrimRight(' ');
    }
    
    if (telegramLen >= 33)
    {
        FInvestorAcNo = Telegram.SubString(26, 7);
        FInvestorAcNo.TrimRight(' ');
    }
    
    if (telegramLen >= 34) FInvestorFlag = Telegram[33];
    
    if (telegramLen >= 54)
    {
        FProdID = Telegram.SubString(34, 20);
        FProdID.TrimRight(' ');
    }
    
    if (telegramLen >= 55) FBuySellCode = Telegram[54];
    
    if (telegramLen >= 56) FOrderType = Telegram[55];
    
    if (telegramLen >= 65)
    {
        FOrderPriceStr = Telegram.SubString(56, 9);
        FOrderPriceStr.TrimRight(' ');
        FOrderPriceStr.TrimLeft(' ');
        if (FOrderPriceStr.Length() > 0) FOrderPrice = FOrderPriceStr.ToDouble();
    }
    
    if (telegramLen >= 69)
    {
        tempStr = Telegram.SubString(65, 4);
        tempStr.TrimRight(' ');
        tempStr.TrimLeft(' ');
        if (tempStr.Length() > 0) FOrderQuantity = tempStr.ToInt();
    }
    
    if (telegramLen >= 70) FOrderCondition = Telegram[69];
    if (telegramLen >= 71) FOpenOffsetFlag = Telegram[70];
    if (telegramLen >= 75) FClearMeneberID = Telegram.SubString(71, 4);
}  //COrderRequest::ParseBodyTelegram()

//------------------------------- CQuoteRequest --------------------------------
//------------------------------------------------------------------------------
CQuoteRequest::CQuoteRequest()
:CBasicRequest()
,FBidQuotePriceStr("")
,FBidQuotePrice(0.0)
,FAskQuotePriceStr("")
,FAskQuotePrice(0.0)
,FBidQuoteQuantity(0)
,FAskQuoteQuantity(0)
{
}  //CQuoteRequest::CQuoteRequest()
//------------------------------------------------------------------------------
CQuoteRequest::CQuoteRequest(nsOrderMessageDefine::MarketEnum Market, Int32 NetworkID, const UFC::AnsiString& Telegram, const UFC::AnsiString& UserKey)
:CBasicRequest(Market, NetworkID, Telegram, UserKey)
,FBidQuotePriceStr("")
,FBidQuotePrice(0.0)
,FAskQuotePriceStr("")
,FAskQuotePrice(0.0)
,FBidQuoteQuantity(0)
,FAskQuoteQuantity(0)
{
    ParseHeadTelegram(FTelegram);
    ParseBodyTelegram(FTelegram);
}  //COrderRequest::COrderRequest()
//------------------------------------------------------------------------------
void CQuoteRequest::ParseBodyTelegram(const UFC::AnsiString& Telegram)
{
    int telegramLen = Telegram.Length();
    UFC::AnsiString tempStr;
    if (telegramLen >= 21)
    {
        FFCMNo = Telegram.SubString(14, 7);
        FFCMNo.TrimRight(' ');
    }
    
    if (telegramLen >= 26)
    {
        FOrderNo = Telegram.SubString(21, 5);
        FOrderNo.TrimRight(' ');
    }
    
    if (telegramLen >= 33)
    {
        FInvestorAcNo = Telegram.SubString(26, 7);
        FInvestorAcNo.TrimRight(' ');
    }
    
    if (telegramLen >= 34) FInvestorFlag = Telegram[33];
    
    if (telegramLen >= 54)
    {
        FProdID = Telegram.SubString(34, 20);
        FProdID.TrimRight(' ');
    }
    
    if (telegramLen >= 63)
    {
        FBidQuotePriceStr = Telegram.SubString(54, 9);
        FBidQuotePriceStr.TrimRight(' ');
        FBidQuotePriceStr.TrimLeft(' ');
        if (FBidQuotePriceStr.Length() > 0) FBidQuotePrice = FBidQuotePriceStr.ToDouble();
    }
    
    if (telegramLen >= 72)
    {
        FAskQuotePriceStr = Telegram.SubString(63, 9);
        FAskQuotePriceStr.TrimRight(' ');
        FAskQuotePriceStr.TrimLeft(' ');
        if (FAskQuotePriceStr.Length() > 0) FAskQuotePrice = FAskQuotePriceStr.ToDouble();
    }
    
    if (telegramLen >= 76)
    {
        tempStr = Telegram.SubString(72, 4);
        tempStr.TrimRight(' ');
        tempStr.TrimLeft(' ');
        if (tempStr.Length() > 0) FBidQuoteQuantity = tempStr.ToInt();
    }
    
    if (telegramLen >= 80)
    {
        tempStr = Telegram.SubString(76, 4);
        tempStr.TrimRight(' ');
        tempStr.TrimLeft(' ');
        if (tempStr.Length() > 0) FAskQuoteQuantity = tempStr.ToInt();
    }
    
    if (telegramLen >= 81) FOrderCondition = Telegram[80];
    if (telegramLen >= 82) FOpenOffsetFlag = Telegram[81];
    if (telegramLen >= 86) FClearMeneberID = Telegram.SubString(82, 4);
}  //CQuoteRequest::ParseBodyTelegram()

//----------------------------- CQuoteOrderRecord ------------------------------
//------------------------------------------------------------------------------
CQuoteOrderRecord::CQuoteOrderRecord(nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& OrderID, const UFC::AnsiString& Symbol, double BidPrice, int BidQty, double AskPrice, int AskQty)
:FMarket(Market)
,FOrderID(OrderID)
,FSymbol(Symbol)
,FBidPrice(BidPrice)
,FBidQty(BidQty)
,FBidMatchQty(0)
,FBidCumQty(0)
,FBidLeavesQty(BidQty)
,FAskPrice(AskPrice)
,FAskQty(AskQty)
,FAskMatchQty(0)
,FAskCumQty(0)
,FAskLeavesQty(AskQty)
{
}  //CQuoteOrderRecord::CQuoteOrderRecord()

//----------------------------- COptionAValueInfo ------------------------------
//------------------------------------------------------------------------------
COptionAValueInfo::COptionAValueInfo(const UFC::AnsiString& ContractCode, char MarginType, double MarginInitial)
:FContractCode(ContractCode)
,FMarginType(MarginType)
,FMarginInitial(MarginInitial)
{ 
}  //COptionAValueInfo::COptionAValueInfo()
//------------------------------------------------------------------------------
UFC::AnsiString COptionAValueInfo::ToString()
{
    UFC::AnsiString objStr;
    if (FMarginType == '0')
        objStr.Printf("Commodity[%s] AValue=%10.0lf", FContractCode.c_str(), FMarginInitial);
    else
        objStr.Printf("Commodity[%s] AValue=%10.3lf%%", FContractCode.c_str(), FMarginInitial);
    return objStr;
}  //COptionAValueInfo::ToString()
//------------------------------------------------------------------------------
void COptionAValueInfo::DumpData()
{
    UFC::BufferedLog::Printf(" %s", ToString().c_str());
}  //COptionAValueInfo::DumpData()

//------------------------------- GCRMConnection -------------------------------
UFC::FileStreamEx *GCRMConnection::FCheckLogPtr = 0;
bool GCRMConnection::FDoTrimAEZero = false;
//------------------------------------------------------------------------------
UFC::AnsiString GCRMConnection::TrimBeginZero(const UFC::AnsiString& SourceStr)
{
    UFC::AnsiString resultStr = SourceStr;
    if ((resultStr.Length() >= 1) && (resultStr[0] == '0')) resultStr.TrimLeft('0');
    return resultStr;
}  //GCRMConnection::TrimBeginZero()

//------------------------------------------------------------------------------
UFC::AnsiString GCRMConnection::FClearAmountFileName = "";

//------------------------------------------------------------------------------
UFC::PReadWriteLock GCRMConnection::FCurrencyObjLock;
UFC::PHashMap<UFC::AnsiString, CCurrencyObject*> GCRMConnection::FCurrencyObjCodeIndex;
UFC::PHashMap<char, CCurrencyObject*> GCRMConnection::FCurrencyObjTypeIndex;
//------------------------------------------------------------------------------
void GCRMConnection::DumpCurrencyObject()
{
    FCurrencyObjLock.LockForRead();
    int currencyCount = FCurrencyObjTypeIndex.ItemCount();
    CCurrencyObject* curCurrencyObjPtr = FCurrencyObjTypeIndex.First();
    int curIndex = 0;
    while (curCurrencyObjPtr != 0)
    {
        curIndex++;
        UFC::BufferedLog::Printf(" %3d/%3d %s", curIndex, currencyCount, curCurrencyObjPtr->ToString().c_str());
        curCurrencyObjPtr = FCurrencyObjTypeIndex.Next();
    }
    FCurrencyObjLock.UnlockForRead();
}  //GCRMConnection::DumpCurrencyObject()
//------------------------------------------------------------------------------
std::map<UFC::AnsiString, CCommodityObject*> GCRMConnection::FCommodityContractCodeIndex;
//------------------------------------------------------------------------------
int GCRMConnection::GetCommodityCount()
{
    return static_cast<int>(FCommodityContractCodeIndex.size());
}  //GCRMConnection::GetCommodityCount()
//------------------------------------------------------------------------------
CCommodityObject *GCRMConnection::FindCommodity(const UFC::AnsiString& ContractCode)
{
    CCommodityObject *commodityPtr = 0;
    std::map<UFC::AnsiString, CCommodityObject*>::iterator it = FCommodityContractCodeIndex.find(ContractCode);
    if (it != FCommodityContractCodeIndex.end()) commodityPtr = it->second;
    return commodityPtr;
}  //GCRMConnection::FindCommodity()
//------------------------------------------------------------------------------
bool GCRMConnection::InsertCommodity(const UFC::AnsiString& ContractCode, CCommodityObject *CommodityPtr)
{
    std::pair<std::map<UFC::AnsiString, CCommodityObject*>::iterator, bool> insertPair = 
        FCommodityContractCodeIndex.insert(std::map<UFC::AnsiString, CCommodityObject*>::value_type(ContractCode, CommodityPtr));
    return insertPair.second;    
}  //GCRMConnection::InsertCommodity()
//------------------------------------------------------------------------------
int GCRMConnection::ClearCommodity(bool DoDelete)
{
    int deleteCount = 0;
    if (DoDelete)
    {
        std::map<UFC::AnsiString, CCommodityObject*>::iterator loopIt;
        for (loopIt = FCommodityContractCodeIndex.begin(); loopIt != FCommodityContractCodeIndex.end(); loopIt++)
        {
            CCommodityObject *curCommodityPtr = loopIt->second;
            if (curCommodityPtr == 0) continue;
            deleteCount++;
            delete curCommodityPtr;
        }
    }
    FCommodityContractCodeIndex.clear();
    return deleteCount;
}  //GCRMConnection::ClearCommodity()
//------------------------------------------------------------------------------
void GCRMConnection::DumpCommodity(bool enabledOnly)
{
    int totalCount = static_cast<int>(FCommodityContractCodeIndex.size());
    int loopCount = 0;
    std::map<UFC::AnsiString, CCommodityObject*>::iterator loopIt;
    for (loopIt = FCommodityContractCodeIndex.begin(); loopIt != FCommodityContractCodeIndex.end(); loopIt++)
    {
        CCommodityObject *curCommodityPtr = loopIt->second;
        loopCount++;
        if (curCommodityPtr == 0) continue;
        if (!enabledOnly || curCommodityPtr->IsEnabled())
            UFC::BufferedLog::Printf(" %3d/%3d %s", loopCount, totalCount, curCommodityPtr->ToString().c_str());
    }
}  //GCRMConnection::DumpCommodity()
//------------------------------------------------------------------------------
void GCRMConnection::LoadP09File(const UFC::AnsiString& P09FileName, bool IsFutures)
{
    if (IsFutures)
        UFC::BufferedLog::Printf(" LoadP09File() Process Futures P09 file[%s].", P09FileName.c_str());
    else
        UFC::BufferedLog::Printf(" LoadP09File() Process Options P09 file[%s].", P09FileName.c_str());
    int p09RecordSize = 100;
    int p09BufferSize = p09RecordSize + 1;
    char p09Buffer[p09BufferSize];
    try 
    {
        UFC::FileStreamEx *p09FilePtr = new UFC::FileStreamEx(P09FileName, "r");
        int recCount = 0;
        while (p09FilePtr->Read(p09Buffer, p09RecordSize) == p09RecordSize)
        {
            recCount++;
            p09Buffer[p09RecordSize] = 0;
            CCommodityObject *curCommodityPtr = new CCommodityObject(p09Buffer, IsFutures);
            UFC::AnsiString curContractCode = curCommodityPtr->GetContractCode();
            bool isCorrectCommodity = false;
            if (FindCommodity(curContractCode) == 0)
            {
                if (InsertCommodity(curContractCode, curCommodityPtr))
                    isCorrectCommodity = true;
                else
                    UFC::BufferedLog::Printf(" LoadP09File() %3d Insert Commodity[%s] failed. [%s]", recCount, curContractCode.c_str(), p09Buffer);
            }
            else
                UFC::BufferedLog::Printf(" LoadP09File() %3d Duplicate Commodity[%s]. [%s]", recCount, curContractCode.c_str(), p09Buffer);

            if (!isCorrectCommodity) delete curCommodityPtr;
        }  //while (p09FilePtr->Read(p09Buffer, p09RecordSize) == p09RecordSize)
        delete p09FilePtr;
    }
    catch (UFC::FileException& fe)
    {
        UFC::BufferedLog::Printf(" LoadP09File() Process P09 file[%s] Exception:[%s]", P09FileName.c_str(), fe.what());
    }
    UFC::BufferedLog::FlushToFile();    
}  //GCRMConnection::LoadP09File()
//------------------------------------------------------------------------------
std::map<UFC::AnsiString, CContractObject*> GCRMConnection::FContractSymbolIndex;
//------------------------------------------------------------------------------
int GCRMConnection::GetContractCount()
{
    return static_cast<int>(FContractSymbolIndex.size());
}  //GCRMConnection::GetContractCount()
//------------------------------------------------------------------------------
CContractObject *GCRMConnection::FindContract(const UFC::AnsiString& Symbol)
{
    CContractObject *contractPtr = 0;
    std::map<UFC::AnsiString, CContractObject*>::iterator it = FContractSymbolIndex.find(Symbol);
    if (it != FContractSymbolIndex.end()) contractPtr = it->second;
    return contractPtr;
}  //GCRMConnection::FindContract()
//------------------------------------------------------------------------------
bool GCRMConnection::InsertContract(const UFC::AnsiString& Symbol, CContractObject *ContractPtr)
{
    std::pair<std::map<UFC::AnsiString, CContractObject*>::iterator, bool> insertPair = 
        FContractSymbolIndex.insert(std::map<UFC::AnsiString, CContractObject*>::value_type(Symbol, ContractPtr));
    return insertPair.second;    
}  //GCRMConnection::InsertContract()
//------------------------------------------------------------------------------
int GCRMConnection::ClearContract(bool DoDelete)
{
    int deleteCount = 0;
    if (DoDelete) 
    {
        std::map<UFC::AnsiString, CContractObject*>::iterator loopIt;
        for (loopIt = FContractSymbolIndex.begin(); loopIt != FContractSymbolIndex.end(); loopIt++)
        {
            CContractObject *curContractPtr = loopIt->second;
            if (curContractPtr == 0) continue;
            deleteCount++;
            delete curContractPtr;
        }
    }
    FContractSymbolIndex.clear();
    return deleteCount;
}  //GCRMConnection::ClearContract()
//------------------------------------------------------------------------------
void GCRMConnection::DumpContract()
{
    int totalCount = static_cast<int>(FContractSymbolIndex.size());
    int loopCount = 0;
    std::map<UFC::AnsiString, CContractObject*>::iterator loopIt;
    for (loopIt = FContractSymbolIndex.begin(); loopIt != FContractSymbolIndex.end(); loopIt++)
    {
        CContractObject *curContractPtr = loopIt->second;
        loopCount++;
        if (curContractPtr == 0) continue;
        UFC::BufferedLog::Printf(" %5d/%5d %s", loopCount, totalCount, curContractPtr->ToString().c_str());
    }
}  //GCRMConnection::DumpContract()
//------------------------------------------------------------------------------
std::map<UFC::AnsiString, CContractYearMonthSet*> GCRMConnection::FContractYMSetIndex;
//------------------------------------------------------------------------------
int GCRMConnection::GetContractYMSetCount()
{
    return static_cast<int>(FContractYMSetIndex.size());
}  //GCRMConnection::GetContractYMSetCount()
//------------------------------------------------------------------------------
CContractYearMonthSet *GCRMConnection::FindContractYMSet(const UFC::AnsiString& KeyValue)
{
    CContractYearMonthSet *contractYMSetPtr = 0;
    std::map<UFC::AnsiString, CContractYearMonthSet*>::iterator it = FContractYMSetIndex.find(KeyValue);
    if (it != FContractYMSetIndex.end()) contractYMSetPtr = it->second;
    return contractYMSetPtr;
}  //GCRMConnection::FindContractYMSet()
//------------------------------------------------------------------------------
bool GCRMConnection::InsertContractYMSet(const UFC::AnsiString& KeyValue, CContractYearMonthSet *ContractYMSetPtr)
{
    std::pair<std::map<UFC::AnsiString, CContractYearMonthSet*>::iterator, bool> insertPair = 
        FContractYMSetIndex.insert(std::map<UFC::AnsiString, CContractYearMonthSet*>::value_type(KeyValue, ContractYMSetPtr));
    return insertPair.second;    
}  //GCRMConnection::InsertContractYMSet()
//------------------------------------------------------------------------------
int GCRMConnection::ClearContractYMSet(bool DoDelete)
{
    int deleteCount = 0;
    if (DoDelete) 
    {
        std::map<UFC::AnsiString, CContractYearMonthSet*>::iterator loopIt;
        for (loopIt = FContractYMSetIndex.begin(); loopIt != FContractYMSetIndex.end(); loopIt++)
        {
            CContractYearMonthSet *curContractYMSetPtr = loopIt->second;
            if (curContractYMSetPtr == 0) continue;
            deleteCount++;
            delete curContractYMSetPtr;
        }
    }
    FContractYMSetIndex.clear();
    return deleteCount;
}  //GCRMConnection::ClearContractYMSet()
//------------------------------------------------------------------------------
void GCRMConnection::DumpContractYMSet()
{
    int totalCount = static_cast<int>(FContractYMSetIndex.size());
    int loopCount = 0;
    std::map<UFC::AnsiString, CContractYearMonthSet*>::iterator loopIt;
    for (loopIt = FContractYMSetIndex.begin(); loopIt != FContractYMSetIndex.end(); loopIt++)
    {
        CContractYearMonthSet *curContractYMSetPtr = loopIt->second;
        loopCount++;
        if (curContractYMSetPtr == 0) continue;
        UFC::BufferedLog::Printf(" %5d/%5d %s", loopCount, totalCount, curContractYMSetPtr->ToString().c_str());
    }
}  //GCRMConnection::DumpContractYMSet()
//------------------------------------------------------------------------------
std::map<UFC::AnsiString, CContractObject*> GCRMConnection::FNearMonthContractSymbolndex;
//------------------------------------------------------------------------------
CContractObject *GCRMConnection::FindNearMonthContract(const UFC::AnsiString& Symbol)
{
    CContractObject *contractPtr = 0;
    std::map<UFC::AnsiString, CContractObject*>::iterator it = FNearMonthContractSymbolndex.find(Symbol);
    if (it != FNearMonthContractSymbolndex.end()) contractPtr = it->second;
    return contractPtr;
}  //GCRMConnection::FindNearMonthContract()
//------------------------------------------------------------------------------
bool GCRMConnection::InsertNearMonthContract(const UFC::AnsiString& Symbol, CContractObject *ContractPtr)
{
    std::pair<std::map<UFC::AnsiString, CContractObject*>::iterator, bool> insertPair;
    insertPair = FNearMonthContractSymbolndex.insert(std::map<UFC::AnsiString, CContractObject*>::value_type(Symbol, ContractPtr));
    return insertPair.second;    
}  //GCRMConnection::InsertNearMonthContract()
//------------------------------------------------------------------------------
void GCRMConnection::DumpNearMonthContract()
{
    int totalCount = static_cast<int>(FNearMonthContractSymbolndex.size());
    int loopCount = 0;
    std::map<UFC::AnsiString, CContractObject*>::iterator loopIt;
    for (loopIt = FNearMonthContractSymbolndex.begin(); loopIt != FNearMonthContractSymbolndex.end(); loopIt++)
    {
        loopCount++;
        CContractObject *curContractPtr = loopIt->second;
        if (curContractPtr == 0) continue;
        UFC::BufferedLog::Printf(" %5d/%5d Near Month Contract [%s][%s]", loopCount, totalCount, curContractPtr->GetSymbol().c_str(), curContractPtr->GetDeliveryYearMonth().c_str());
    }
    if (loopCount <= 0) UFC::BufferedLog::Printf(" GCRMConnection::DumpNearMonthContract() No Near Month Contract Data.");
}  //GCRMConnection::DumpNearMonthContract()
//------------------------------------------------------------------------------
void GCRMConnection::LoadP08File(const UFC::AnsiString& P08FormatFileName)
{
    UFC::AnsiString logHead = "LoadP08File()";
    if (UFC::FileExists(P08FormatFileName) == FALSE)
    {
        UFC::BufferedLog::Printf(" %s P08 Format File[%s] not Exists.", logHead.c_str(), P08FormatFileName.c_str());
        return;
    }

    UFC::UiniFile *p08FormatIniPtr = 0;
    try
    {
        p08FormatIniPtr = new UFC::UiniFile(P08FormatFileName);
    }
    catch(UFC::FileException& fe)
    {
        UFC::BufferedLog::Printf(" %s Load P08 Format INI File[%s] failed. %s", logHead.c_str(), P08FormatFileName.c_str(), fe.what() );
        return;
    }
    
    UFC::AnsiString p08FileName = "";
    if (p08FormatIniPtr->GetValue("FixedFormatConfig", "FilePath", p08FileName) == FALSE)
    {
        p08FileName = "";
        UFC::BufferedLog::Printf(" %s [FixedFormatConfig-FilePath] not Setting in P08 Format INI File[%s]", logHead.c_str(), P08FormatFileName.c_str());
    }

    UFC::AnsiString p08RecordLengthStr = "";
    int p08RecordLength = 0;
    if (p08FormatIniPtr->GetValue("FixedFormatConfig", "LineLength", p08RecordLengthStr) == TRUE)
        p08RecordLength = p08RecordLengthStr.ToInt();
    else
    {
        p08RecordLengthStr = "";
        UFC::BufferedLog::Printf(" %s [FixedFormatConfig-LineLength] not Setting in P08 Format INI File[%s]", logHead.c_str(), P08FormatFileName.c_str());
    }
    if (p08FormatIniPtr != 0) delete p08FormatIniPtr;
    if ((p08FileName.Length() <= 0) || (p08RecordLengthStr.Length() <= 0)) return;
    
    if (UFC::FileExists(p08FileName) == FALSE)
    {
        UFC::BufferedLog::Printf(" %s P08 File[%s] not Exists.", logHead.c_str(), p08FileName.c_str());
        return;
    }
    
    UFC::FileStream *p08FilePtr = 0;
    try 
    {
        p08FilePtr = new UFC::FileStream(p08FileName, O_RDONLY);
    }
    catch (UFC::FileException& e)
    {
        UFC::BufferedLog::Printf(" %s Open P08 File[%s] failed: %s.", logHead.c_str(), p08FileName.c_str(), e.what());
        return;
    }
    
    UFC::BufferedLog::Printf(" %s P08 Format File[%s], Data File[%s], Record Length %d", logHead.c_str(), P08FormatFileName.c_str(), p08FileName.c_str(), p08RecordLength);
    
    UFC::TFixedFormat p08Format(P08FormatFileName);
    UFC::TParseData parseObj;
    parseObj.SetDataFormat(&p08Format);
    UFC::TDataset *p08DataSetPtr = parseObj.Parse(p08FilePtr);
    if (p08DataSetPtr != NULL)
    {
        int recCount = p08DataSetPtr->GetRecordCount();
        for (int curRecIndex = 0; curRecIndex < recCount; curRecIndex++)
        {
            UFC::TRecord *curP08RecPtr = p08DataSetPtr->GetRecord(curRecIndex);
            CContractObject *newContractPtr = new CContractObject(curP08RecPtr);
            UFC::AnsiString curSymbol = newContractPtr->GetSymbol();
            UFC::AnsiString curContractCode = newContractPtr->GetContractCode();
            UFC::AnsiString curDeliveryYearMonth = newContractPtr->GetDeliveryYearMonth();
            char curCallPut = newContractPtr->GetCallPut();
            UFC::AnsiString curStrikePriceStr = newContractPtr->GetStrikePriceStr();
            
            CCommodityObject *curCommodityPtr = FindCommodity(curContractCode);
            if (curCommodityPtr == 0)
            {
                UFC::BufferedLog::Printf(" %s Commodity[%s] not Exist. %d/%d Symbol[%s]", logHead.c_str(), curContractCode.c_str(), curRecIndex + 1, recCount, curSymbol.c_str());
                delete newContractPtr;
                continue;
            }  //if (curCommodityPtr == 0)
            
            if(!InsertContract(curSymbol, newContractPtr))
            {
                UFC::BufferedLog::Printf(" %s Insert %d/%d Contract[%s] failed.", logHead.c_str(), curRecIndex + 1, recCount, curSymbol.c_str());
                delete newContractPtr;
                continue;
            }
            
            UFC::AnsiString curContractSetKey = CContractYearMonthSet::GeterateContractSetKey(curContractCode, curCallPut, curStrikePriceStr);
            CContractYearMonthSet *curContractYMSetPtr = FindContractYMSet(curContractSetKey);
            if (curContractYMSetPtr == 0)
            {
                curContractYMSetPtr = new CContractYearMonthSet(curContractCode, curCallPut, curStrikePriceStr);
                if (!InsertContractYMSet(curContractSetKey, curContractYMSetPtr))
                {
                    UFC::BufferedLog::Printf(" %s Insert %d/%d ContractYearMonthSet[%s] failed.", logHead.c_str(), curRecIndex + 1, recCount, curContractSetKey.c_str());
                    delete curContractYMSetPtr;
                    continue;
                }
            }
            
            if (!curContractYMSetPtr->InsertContractToYearMonthIndex(curDeliveryYearMonth, newContractPtr))
            {
                UFC::BufferedLog::Printf(" %s Insert %d/%d Contract[%s] To ContractYearMonthSet[%s] failed.", logHead.c_str(), curRecIndex + 1, recCount, curSymbol.c_str(), curContractSetKey.c_str());
                continue;
            }
            UFC::BufferedLog::Printf(" %s process %d/%d [%s][%s] ", logHead.c_str(), curRecIndex + 1, recCount, curSymbol.c_str(), curDeliveryYearMonth.c_str());
        }  //for (int curRecIndex = 0; curRecIndex < recCount; curRecIndex++)
    }  //if (p08DataSetPtr != NULL)
    
    
/*    
//    int p08RecordSize = 115;
    int p08RecordSize = 160;
    int p08BufferSize = p08RecordSize + 1;
    char p08Buffer[p08BufferSize];
    try 
    {
        UFC::FileStreamEx *p08FilePtr = new UFC::FileStreamEx(P08FileName, "r");
        int recCount = 0;
        while (p08FilePtr->Read(p08Buffer, p08RecordSize) == p08RecordSize)
        {
            recCount++;
            p08Buffer[p08RecordSize] = 0;
            CContractObject *newContractPtr = new CContractObject(p08Buffer);
            UFC::AnsiString curSymbol = newContractPtr->GetSymbol();
            UFC::AnsiString curContractCode = newContractPtr->GetContractCode();
            UFC::AnsiString curDeliveryYearMonth = newContractPtr->GetDeliveryYearMonth();
            char curCallPut = newContractPtr->GetCallPut();
            UFC::AnsiString curStrikePriceStr = newContractPtr->GetStrikePriceStr();
            
            CCommodityObject *curCommodityPtr = FindCommodity(curContractCode);
            if (curCommodityPtr == 0)
            {
                UFC::BufferedLog::Printf(" %s %5d Commodity[%s] not Exist. Symbol[%s]", logHead.c_str(), recCount, curContractCode.c_str(), curSymbol.c_str());
                delete curCommodityPtr;
                continue;
            }  //if (curCommodityPtr == 0)
            
            if(!InsertContract(curSymbol, newContractPtr))
            {
                UFC::BufferedLog::Printf(" %s Insert Contract[%s] failed.", logHead.c_str(), recCount, curSymbol.c_str());
                delete newContractPtr;
                continue;
            }
            UFC::AnsiString curContractSetKey = CContractYearMonthSet::GeterateContractSetKey(curContractCode, curCallPut, curStrikePriceStr);
            CContractYearMonthSet *curContractYMSetPtr = FindContractYMSet(curContractSetKey);
            if (curContractYMSetPtr == 0)
            {
                curContractYMSetPtr = new CContractYearMonthSet(curContractCode, curCallPut, curStrikePriceStr);
                if (!InsertContractYMSet(curContractSetKey, curContractYMSetPtr))
                {
                    UFC::BufferedLog::Printf(" %s %5d Insert ContractYearMonthSet[%s] failed.", logHead.c_str(), recCount, curContractSetKey.c_str());
                    delete curContractYMSetPtr;
                    continue;
                }
            }
            
            if (!curContractYMSetPtr->InsertContractToYearMonthIndex(curDeliveryYearMonth, newContractPtr))
            {
                UFC::BufferedLog::Printf(" %s %5d Insert Contract[%s] To ContractYearMonthSet[%s] failed.", logHead.c_str(), recCount, curSymbol.c_str(), curContractSetKey.c_str());
                continue;
            }
            UFC::BufferedLog::Printf(" %s process %5d[%s][%s] ", logHead.c_str(), recCount, curSymbol.c_str(), curDeliveryYearMonth.c_str());
        }  //while (p08FilePtr->Read(p08Buffer, p08RecordSize) == p08RecordSize)
        UFC::BufferedLog::Printf(" %s Process %5d Record from [%s].", logHead.c_str(), recCount, P08FileName.c_str());
        delete p08FilePtr;
    }
    catch (UFC::FileException fe)
    {
        UFC::BufferedLog::Printf(" %s Process file[%s] Exception:[%s]", logHead.c_str(), P08FileName.c_str(), fe.what());
    }
    UFC::BufferedLog::FlushToFile();
 */
}  //GCRMConnection::LoadP08File()
//------------------------------------------------------------------------------
void GCRMConnection::BuildNearMonthIndex()
{
    UFC::AnsiString logHead = "BuildNearMonthIndex()";
    int loopCount = 0;
    std::map<UFC::AnsiString, CContractYearMonthSet*>::iterator loopIt;
    for (loopIt = FContractYMSetIndex.begin(); loopIt != FContractYMSetIndex.end(); loopIt++)
    {
        CContractYearMonthSet *curContractYMSetPtr = loopIt->second;
        if (curContractYMSetPtr != 0)
        {
            loopCount++;
            CContractObject *firstContractPtr = curContractYMSetPtr->GetFirstYearMonthContract();
            if (firstContractPtr != 0)
            {
                UFC::AnsiString firstSymbol = firstContractPtr->GetSymbol();
                if (!InsertNearMonthContract(firstSymbol, firstContractPtr))
                    UFC::BufferedLog::Printf(" %s Insert first Contract[%s] to NearMonthIndex failed.", logHead.c_str(), firstSymbol.c_str());
                
                CContractObject *secondContractPtr = curContractYMSetPtr->GetSecondYearMonthContract();
                if (secondContractPtr != 0)
                {
                    UFC::AnsiString secondSymbol = secondContractPtr->GetSymbol();
                    if (!InsertNearMonthContract(secondSymbol, secondContractPtr))
                        UFC::BufferedLog::Printf(" %s Insert second Contract[%s] to NearMonthIndex failed.", logHead.c_str(), secondSymbol.c_str());
                }
            }
        }  //if (curCommodityPtr != 0)
    }  //for (loopIt = FContractYMSetIndex.begin(); loopIt != FContractYMSetIndex.end(); loopIt++)
}  //GCRMConnection::BuildNearMonthIndex()
//------------------------------------------------------------------------------
void GCRMConnection::LoadP14File(const UFC::AnsiString& P14FileName, bool NeedCopy, const UFC::AnsiString& P14CopyFileName)
{//TXO A01000016000000017000000022000
 //TEO A01000014000000015000000019000
 //TFO A01000009000000010000000013000
 //OEO A11000010000000010350000013500
 //LOO A11000010000000010350000013500
 //TXF 001000068000000071000000092000
 //XIF 001000042000000044000000057000
    UFC::AnsiString logHead = "LoadP14File()";
    UFC::FileStreamEx *p14FilePtr = 0;
    try
    {
        p14FilePtr = new UFC::FileStreamEx(P14FileName, "r");
    }
    catch (UFC::FileException& fe)
    {
        UFC::BufferedLog::Printf(" %s Open P14 file[%s] Exception:[%s]", logHead.c_str(), P14FileName.c_str(), fe.what());
        UFC::BufferedLog::FlushToFile();
        return;
    }
    
    UFC::FileStreamEx *p14CopyFilePtr = 0;
    if (NeedCopy && (P14CopyFileName.Length() > 0))
    {
        try
        {
            p14CopyFilePtr = new UFC::FileStreamEx(P14CopyFileName, "w");
        }
        catch (UFC::FileException& fe)
        {
            UFC::BufferedLog::Printf(" %s Open P14 copyfile[%s] Exception:[%s]", logHead.c_str(), P14CopyFileName.c_str(), fe.what());
            p14CopyFilePtr = 0;
        }
    }  //if (NeedCopy && (P14CopyFileName.Length() > 0))
    
    int p14RecordSize = 34;
    int p14BufferSize = p14RecordSize + 1;
    char p14Buffer[p14BufferSize];
    try 
    {
        int recCount = 0;
        int aValueCount = 0;
        while (p14FilePtr->Read(p14Buffer, p14RecordSize) == p14RecordSize)
        {
            if (NeedCopy && (p14CopyFilePtr != 0))
            {
                try
                {
                    p14CopyFilePtr->Write(p14Buffer, p14RecordSize);
                }
                catch (UFC::FileException& fe)
                {
                    UFC::BufferedLog::Printf(" %s P14 copyfile[%s] write Exception:[%s]", logHead.c_str(), P14CopyFileName.c_str(), fe.what());
                }
            }
            
            recCount++;
            p14Buffer[p14RecordSize] = 0;
            UFC::AnsiString curP14RecStr(p14Buffer);
            UFC::AnsiString curContractCode = curP14RecStr.SubString(0, 4);
            curContractCode.TrimRight(' ');
            char curContractABType = curP14RecStr[4];  //A, B, 0
            char curMarginType = curP14RecStr[5];  //0: Amount, 1: Percent
            char curCurrencyType = curP14RecStr[6];
            UFC::AnsiString curClearingMarginStr = curP14RecStr.SubString(7, 9);
            UFC::AnsiString curMaintenanceMarginStr = curP14RecStr.SubString(16, 9);
            UFC::AnsiString curInitialMarginStr = curP14RecStr.SubString(25, 9);
            if ((curContractABType != 'A') && (curContractABType != '0')) continue;
            
            double clearingMargin = 0.0;
            double maintenanceMargin = 0.0;
            double initialMargin = 0.0;
            if (curMarginType == '0')  //Amount
            {
                curClearingMarginStr.TrimLeft('0');
                clearingMargin = curClearingMarginStr.ToDouble();
                
                curMaintenanceMarginStr.TrimLeft('0');
                maintenanceMargin = curMaintenanceMarginStr.ToDouble();
                
                curInitialMarginStr.TrimLeft('0');
                initialMargin = curInitialMarginStr.ToDouble();
            }
            else if (curMarginType == '1')  //Percent
            {
                UFC::AnsiString integerStr = curClearingMarginStr.SubString(0, 6);
                UFC::AnsiString fractionStr = curClearingMarginStr.SubString(6, 3);
                clearingMargin = integerStr.ToDouble() + fractionStr.ToDouble() / 1000.0;
                
                integerStr = curMaintenanceMarginStr.SubString(0, 6);
                fractionStr = curMaintenanceMarginStr.SubString(6, 3);
                maintenanceMargin = integerStr.ToDouble() + fractionStr.ToDouble() / 1000.0;
                
                integerStr = curInitialMarginStr.SubString(0, 6);
                fractionStr = curInitialMarginStr.SubString(6, 3);
                initialMargin = integerStr.ToDouble() + fractionStr.ToDouble() / 1000.0;
            }
            
            CCommodityObject *curCommodityPtr = FindCommodity(curContractCode);
            if (curCommodityPtr != 0)
            {
                curCommodityPtr->SetABType(curContractABType);
                curCommodityPtr->SetMarginType(curMarginType);
                curCommodityPtr->SetClearingMargin(clearingMargin);
                curCommodityPtr->SetMaintenanceMargin(maintenanceMargin);
                curCommodityPtr->SetInitialMargin(initialMargin);
            }
            else
            {
                UFC::BufferedLog::Printf(" %s %d Commodity[%s] not Found.", logHead.c_str(), recCount, curContractCode.c_str());
                continue;
            } 
            aValueCount++;

            if (curMarginType == '0')  //Amount
                UFC::BufferedLog::Printf(" %s %5d[%s]%c Currency[%c] Margin{Clearing[%9.0lf] Maintenance[%9.0lf] Initial[%9.0lf]}",
                                         logHead.c_str(), aValueCount, curContractCode.c_str(), curContractABType, curCurrencyType, clearingMargin, maintenanceMargin, initialMargin);
            else 
                UFC::BufferedLog::Printf(" %s %5d[%s]%c Currency[%c] Margin{Clearing[%10.3lf%%] Maintenance[%10.3lf%%] Initial[%10.3lf%%]}",
                                         logHead.c_str(), aValueCount, curContractCode.c_str(), curContractABType, curCurrencyType, clearingMargin, maintenanceMargin, initialMargin);
        }  //while (p14FilePtr->Read(p16Buffer, p14RecordSize) == p14RecordSize)
        UFC::BufferedLog::Printf(" Process P14 %5d/%5d Record from [%s].", aValueCount, recCount, P14FileName.c_str());
        delete p14FilePtr;
        if (NeedCopy && (p14CopyFilePtr != 0)) delete p14CopyFilePtr;
    }
    catch (UFC::FileException& fe)
    {
        UFC::BufferedLog::Printf(" %s Process P14 file[%s] Exception:[%s]", logHead.c_str(), P14FileName.c_str(), fe.what());
    }
    UFC::BufferedLog::FlushToFile();    
}  //GCRMConnection::LoadP14File()
//------------------------------------------------------------------------------
std::map<UFC::AnsiString, CAccountExecutive*> GCRMConnection::FSubAccountAEIndex;
//------------------------------------------------------------------------------
CAccountExecutive *GCRMConnection::FindSubAccount(const UFC::AnsiString& AE, char CurrencyType)
{
    UFC::AnsiString aeKey = CAccountExecutive::BuildAccountExecutiveKey(AE, CurrencyType);
    CAccountExecutive *aePtr = 0;
    std::map<UFC::AnsiString, CAccountExecutive*>::iterator it = FSubAccountAEIndex.find(aeKey);
    if (it != FSubAccountAEIndex.end()) aePtr = it->second;
    return aePtr;
}  //GCRMConnection::FindSubAccount()
//------------------------------------------------------------------------------
bool GCRMConnection::InsertSubAccount(const UFC::AnsiString& AE, char CurrencyType, CAccountExecutive* SubAccountPtr)
{
    UFC::AnsiString aeKey = CAccountExecutive::BuildAccountExecutiveKey(AE, CurrencyType);
    std::pair<std::map<UFC::AnsiString, CAccountExecutive*>::iterator, bool> insertPair;
    insertPair = FSubAccountAEIndex.insert(std::map<UFC::AnsiString, CAccountExecutive*>::value_type(aeKey, SubAccountPtr));
    return insertPair.second;    
}  //GCRMConnection::InsertSubAccount()
//------------------------------------------------------------------------------
void GCRMConnection::DumpSubAccount()
{
    int totalCount = static_cast<int>(FSubAccountAEIndex.size());
    int loopCount = 0;
    std::map<UFC::AnsiString, CAccountExecutive*>::iterator loopIt;
    for (loopIt = FSubAccountAEIndex.begin(); loopIt != FSubAccountAEIndex.end(); loopIt++)
    {
        loopCount++;
        CAccountExecutive *aePtr = loopIt->second;
        if (aePtr == 0) continue;
        UFC::BufferedLog::Printf(" %5d/%5d %s", loopCount, totalCount, aePtr->ToString().c_str());
    }
    
    UFC::BufferedLog::Printf(" GCRMConnection::DumpSubAccount() Dump %d SubAccount Data.", totalCount);
    UFC::BufferedLog::Printf(" ");
}  //GCRMConnection::DumpSubAccount()
//------------------------------------------------------------------------------
std::map<UFC::AnsiString, CQuoteOrderRecord*> GCRMConnection::FQuoteOrderIdIndex;
//------------------------------------------------------------------------------
CQuoteOrderRecord *GCRMConnection::FindQuoteOrder(const UFC::AnsiString& OrderID)
{
    CQuoteOrderRecord *orderPtr = 0;
    std::map<UFC::AnsiString, CQuoteOrderRecord*>::iterator it = FQuoteOrderIdIndex.find(OrderID);
    if (it != FQuoteOrderIdIndex.end()) orderPtr = it->second;
    return orderPtr;
}  //GCRMConnection::FindQuoteOrder()
//------------------------------------------------------------------------------
bool GCRMConnection::InsertQuoteOrder(const UFC::AnsiString& OrderID, CQuoteOrderRecord *QuoteOrderPtr)
{
    std::pair<std::map<UFC::AnsiString, CQuoteOrderRecord*>::iterator, bool> insertPair;
    insertPair = FQuoteOrderIdIndex.insert(std::map<UFC::AnsiString, CQuoteOrderRecord*>::value_type(OrderID, QuoteOrderPtr));
    return insertPair.second;    
}  //GCRMConnection::InsertQuoteOrder()
//------------------------------------------------------------------------------
bool GCRMConnection::ParseFuturesMultiLegSymbol(const UFC::AnsiString& Symbol, UFC::AnsiString& Leg1Symbol, UFC::AnsiString& Leg2Symbol)
{
    bool isMultiLeg = false;
    int symbolLength = Symbol.Length();
    if (symbolLength <= 5) return isMultiLeg;
    int delimiterPos = Symbol.AnsiPos('/');
    if (delimiterPos == -1) return isMultiLeg;
    isMultiLeg = true;
    UFC::AnsiString contractCode = Symbol.SubString(0, 3);
    Leg1Symbol = Symbol.SubString(0, delimiterPos);
    Leg2Symbol = contractCode + Symbol.SubString(delimiterPos + 1, symbolLength - delimiterPos - 1);
    return isMultiLeg;
}  //GCRMConnection::ParseFuturesMultiLegSymbol()
//------------------------------------------------------------------------------
bool GCRMConnection::ParseOptionsMultiLegSymbol(const UFC::AnsiString& Symbol, UFC::AnsiString& Leg1Symbol, UFC::AnsiString& Leg2Symbol)
{
    bool isMultiLeg = false;
    int symbolLength = Symbol.Length();
    if (symbolLength <= 10) return isMultiLeg;
    UFC::AnsiString contractCode = Symbol.SubString(0, 3);
    if (symbolLength == 13)
    {
        if ((Symbol[10] == '/') || (Symbol[10] == ':') || (Symbol[10] == '-')) //case 2, case 3, case 5
        {
            isMultiLeg = true;
            Leg1Symbol = Symbol.SubString(0, 10);
            Leg2Symbol = Symbol.SubString(0, 8) + Symbol.SubString(11, 2);
        }
    }
    else if (symbolLength == 16)
    {
        if (Symbol[8] == '/')  //case 1
        {
            isMultiLeg = true;
            Leg1Symbol = Symbol.SubString(0, 8) + Symbol.SubString(14, 2);
            Leg2Symbol = contractCode + Symbol.SubString(9, 7);
        }        
        if (Symbol[10] == '/')  //case 6
        {
            isMultiLeg = true;
            Leg1Symbol = Symbol.SubString(0, 10);
            Leg2Symbol = Symbol.SubString(11, 3) + Symbol.SubString(3, 5) + Symbol.SubString(14, 2);
        }        
    }
    else if (symbolLength == 18)
    {
        if (Symbol[10] == ':')  //case 4
        {
            isMultiLeg = true;
            Leg1Symbol = Symbol.SubString(0, 10);
            Leg2Symbol = contractCode + Symbol.SubString(11, 7);
        }
    }
    
    return isMultiLeg;
}  //GCRMConnection::ParseOptionsMultiLegSymbol()
//------------------------------------------------------------------------------
bool GCRMConnection::ParseMultiLegSymbol(nsOrderMessageDefine::MarketEnum Market, const UFC::AnsiString& Symbol, UFC::AnsiString& Leg1Symbol, UFC::AnsiString& Leg2Symbol)
{
    if (Market == nsOrderMessageDefine::mTWFutures)
        return ParseFuturesMultiLegSymbol(Symbol, Leg1Symbol, Leg2Symbol);
    else if (Market == nsOrderMessageDefine::mTWOptions)
        return ParseOptionsMultiLegSymbol(Symbol, Leg1Symbol, Leg2Symbol);
    else
        return false;
}  //GCRMConnection::ParseMultiLegSymbol()

//------------------------------------------------------------------------------
GCRMConnection::GCRMConnection( int RecoverBeginTime )
:FIP( "127.0.0.1" )
,FPort( 12345 )
,FGatewayIP( "127.0.0.1" )
,FGatewayPort( 23456 )
,FID( "yuan" )
,FPassword( "1234rewq" )
,FAccount( "0000000" )
,FBrokerID( "F002000" )
,FClearMemberID( "" )
,FConfig( 0 )
,FRecoverBeginTime( RecoverBeginTime )
,FIsLogon( FALSE )
,FRejectMarketOrd( TRUE )
,FReleaseMarginOnFilled( FALSE )
,FReleaseMarginOnRODCxl( FALSE )
,FReleaseMarginOnFOKIOCCxl( TRUE )
,FMaxQuoteOrderQty(25)
,FToSpeedySubject( "Risk") ///.TOSPEEY" )
,FToSpeedyKey( "ToSpeedy") ///.TOSPEEY" )
,FFutOrderSubject( gAppName + ".FUT" )
,FOptOrderSubject( gAppName + ".OPT")
{    
    LoadSetting();
    AccountInfo::FMaxQuoteOrderQty = FMaxQuoteOrderQty;

    UFC::AnsiString checkLogFileName, ExecIDFile;
    UFC::UDateTime now;
    checkLogFileName.Printf( "../log/UniRiskCheck.%04d%02d%02d.log", now.getYear(), now.getMonth(), now.getDate() );
    ExecIDFile.Printf( "ExecID.%s.DATA", UFC::GetDateString().c_str() );
    UFC::AnsiString appDescription;
    appDescription.Printf("%s RM connection", gApplicationNamePrefix.c_str());

    UFC::BufferedLog::Printf( "______________________________________________" );
    UFC::BufferedLog::Printf( "                                              " );
    UFC::BufferedLog::Printf( "   %s Risk control module ", gApplicationNamePrefix.c_str() );
    UFC::BufferedLog::Printf( "   ApplicationName[%s] Description[%s]", gAppName.c_str(), appDescription.c_str() );
    UFC::BufferedLog::Printf( "   Startup at: %s.             ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   Build Date:%s %s, %ld bit mode", __DATE__, __TIME__, sizeof(void*) * 8 );
    UFC::BufferedLog::Printf( "   BrokerID:[%s] ClearMemberID:[%s]", FBrokerID.c_str(), FClearMemberID.c_str() );
    UFC::BufferedLog::Printf( "   MBus:[%s:%d] Gateway:[%s:%d]", FIP.c_str(), FPort, FGatewayIP.c_str(), FGatewayPort );
    
    if( DumpOnly == TRUE ) 
        UFC::BufferedLog::Printf( "   Debug Mode:On");    
    else
        UFC::BufferedLog::Printf( "   Debug Mode:Off");
    
    if( FRejectMarketOrd == TRUE )
        UFC::BufferedLog::Printf( "   Market Order:Reject");
    else
        UFC::BufferedLog::Printf( "   Market Order:Accept");
    
    UFC::BufferedLog::Printf( "   AS400 To Speedy [%s][%s]", FToSpeedySubject.c_str(), FToSpeedyKey.c_str() );
    UFC::BufferedLog::Printf( "   Recover BeginTime:[%d]", FRecoverBeginTime );    
    UFC::BufferedLog::Printf( "                       " );
    UFC::BufferedLog::Printf( "   Release Margin on Filled[%s]", (FReleaseMarginOnFilled == TRUE)? "Yes" : "No" );
    UFC::BufferedLog::Printf( "   Release Margin on ROD Canceled[%s]", (FReleaseMarginOnRODCxl == TRUE)? "Yes" : "No" );
    UFC::BufferedLog::Printf( "   Release Margin on FOK,IOC Canceled[%s]", (FReleaseMarginOnFOKIOCCxl == TRUE)? "Yes" : "No" );
    UFC::BufferedLog::Printf( "   MaxQuoteOrderQty [%d]", FMaxQuoteOrderQty);

    if (FClearAmountFileName.Length() > 0)
        UFC::BufferedLog::Printf( "   Clear Amount File[%s].", FClearAmountFileName.c_str());
    else
        UFC::BufferedLog::Printf( "   Clear Amount File[].");
            
    if( FDoTrimAEZero == TRUE ) 
        UFC::BufferedLog::Printf( "   Trim AE Left Zero Character.");    

    UFC::BufferedLog::Printf( "__________________________________________" );
    
    DumpCurrencyObject();
    DumpSubAccount();
    DumpCommodity(true);
//    DumpOptionAObj();

    ///< Create the connection object.
    FMessageObject = new MessageObject( gAppName, "1.0.0", appDescription, FPort );
    FMessageObject->SetHost(FIP);
    FMessageObject->AddListener( FFutOrderSubject, UFC::Hostname, this );
    FMessageObject->AddListener( FOptOrderSubject, UFC::Hostname, this );
    FMessageObject->AddListener( FToSpeedySubject, FToSpeedyKey, this );

    if( RecoverMode == FALSE )
    {
        FExecIDFile = new UFC::FileStreamEx( ExecIDFile, "w" );
    }
    else
    {
        FExecIDFile = new UFC::FileStreamEx( ExecIDFile, "r+" );
        RecoverExecID();
    }
    
    if (UFC::FileExists(checkLogFileName) == TRUE)
        FCheckLogPtr = new UFC::FileStreamEx(checkLogFileName, "a+");
    else
    {
        FCheckLogPtr = new UFC::FileStreamEx(checkLogFileName, "w");
        UFC::AnsiString checkLogStr; 
        checkLogStr.Printf("Account_____________ OrderID Time_____ Symbol______________ B/S Price___________ Qty__ Available_______ Needed__________ Reserved________ Rest____________ QuoteLimit MatchQty Message\n");
//checkLogStr.Printf("[%7s][%6s][%c] %5s   %02d%02d%02d%03d %-20s  %c  %16.4lf %5d %16.4lf %16.4lf %16.4lf %16.4lf      %5d    %5d Accept\n",
        GCRMConnection::FCheckLogPtr->Write(checkLogStr.c_str(), checkLogStr.Length());
        GCRMConnection::FCheckLogPtr->Flush();
    }
    
    ///< Create the connection object.
    FConnection = new TTaifexConnection( gAppName, this, atSpeedy );
    FConnection->SetUseNewFuturesSymbol( TRUE );
    ///< Set the broker ID of this connection.
    FConnection->SetBrokerID( nsOrderMessageDefine::mTWFutures, FBrokerID.c_str() );
    FConnection->SetCMID( FClearMemberID.c_str() );
    ///< Load TAIFEX message format config files.
    FConnection->SetTSEDataFormatResourceID(IDR_TSE_T010, IDR_TSE_O010, IDR_TSE_P010, IDR_TSE_T020, IDR_TSE_O020, IDR_TSE_P020, IDR_TSE_R030, "INI");
    FConnection->SetOTCDataFormatResourceID(IDR_OTC_T010, IDR_OTC_O010, IDR_OTC_P010, IDR_OTC_T020, IDR_OTC_O020, IDR_OTC_P020, IDR_OTC_R030, "INI");
    FConnection->SetTSEDataFormatResourceID2(IDR_TSE_A010, IDR_TSE_V010, IDR_TSE_E010, IDR_TSE_Ex010, IDR_TSE_A020, IDR_TSE_V020, IDR_TSE_E020, IDR_TSE_Ex020, "INI");
    FConnection->SetOTCDataFormatResourceID2(IDR_OTC_V010, IDR_OTC_E010, IDR_OTC_Ex010, IDR_OTC_V020, IDR_OTC_E020, IDR_OTC_Ex020, "INI");
    FConnection->SetTAIFEXDataFormatResourceID(IDR_OPT_R010, IDR_OPT_R020, IDR_OPT_C030, IDR_OPT_C030M, IDR_OPT_C030CR, IDR_OPT_C030QCR, "INI");
    FConnection->SetTAIFEXQuoteDataFormatResourceID(IDR_OPT_R070, IDR_OPT_R080, IDR_OPT_R090, IDR_OPT_R100, "INI");
    FConnection->SetEnablePendingNewAck(false);
    ///< Start running thread.
    FMessageObject->Start();
    FMessageObject->WaitForConnected();
    
    Glog = gUniRMLogPtr;
    
//    SetAccountInfo( "0000000", "yuan  ", '1', "TWA", 100000000.0, 0 );
//SetAccountInfo( "0000000", "1U1   ", 49285959.0, 0 );
//SetAccountInfo( "8888881", "1U1   ", 49285959.0, 0 );
//SetAccountInfo( "8888881", "1U1   ", 20001000.0, 0 );
//0         1         2         3         4         5         6         7         8
//012345678901234567890123456789012345678901234567890123456789012345678901234567890
//Quote Option
//UFC::AnsiString orderStr = "30010812165900S585999E000488888818TXO09800H5          00000010000000210000250025Q9F008";
//UFC::AnsiString orderStr = "30010812165900S585999E000488888818TXO09800L5          00000010000000210000250025Q9F008";

//Sell Option
//UFC::AnsiString orderStr = "30010011581000S585999b000600000002TXO09800L5          SL0009000000010R0F008";
//UFC::AnsiString orderStr = "30010011581000S585999b000600000002CAO09800L5          SL0009000000010R0F008";
//UFC::AnsiString orderStr = "30010011581000S585999b000600000002TXO07200C6          SL0009000000010R0F008";

//Buy Option
//UFC::AnsiString orderStr = "30010011581000S585999b000688888812TXO09800L5          BL0009000000010R0F008";
//UFC::AnsiString orderStr = "30010011581000S585999b000600000002CAO09800L5          BL0009000000010R0F008";

//MultiLeg Option
//UFC::AnsiString orderStr = "30010011581000S585999b000600000002TXO09800L5/I5       BL0009000000010R0F008";

//Buy Future
//UFC::AnsiString orderStr = "30010011581000S585999b000600000002TXFI5               BL0009000000010R0F008";

/*
Int32 netID = 4212;
UFC::AnsiString userdata = "1U1,sendid=000000,12:16:59.915";
MTree Data;
Data.append( "ORDER", orderStr );
Data.append( "NID", netID );
Data.append( "KEY", userdata );
*/
    
//CheckOptionsOrderRisk(nsOrderMessageDefine::mTWOptions, &Data);
//CheckFuturesOrderRisk(nsOrderMessageDefine::mTWFutures, &Data);
 
}
//------------------------------------------------------------------------------
void GCRMConnection::RecoverExecID( void )
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
void GCRMConnection::LoadSetting( void )
{
    UFC::AnsiString logHead = "GCRMConnection::LoadSetting()";
    UFC::AnsiString configFileName;
    configFileName.Printf( "../cfg/%s.cfg", gAppName.c_str() );
    UFC::UiniFile *configFilePtr = 0;
    try
    {
        configFilePtr = new UFC::UiniFile(configFileName);
        FConfig = configFilePtr;
    }
    catch(UFC::FileException& fe)
    {
        UFC::BufferedLog::Printf(" Load Configure File[%s] failed. %s", configFileName.c_str(), fe.what() );
        exit(1);
    }
    
    UFC::AnsiString sectionName = "Setting";
    UFC::AnsiString fieldName = "MBusIP";
    UFC::AnsiString fieldValue = "";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FIP = fieldValue;
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%s]", sectionName.c_str(), fieldName.c_str(), FIP.c_str());

    fieldName = "MBusPort";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FPort = fieldValue.ToInt();
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%d]", sectionName.c_str(), fieldName.c_str(), FPort);
    
    fieldName = "GatewayIP";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FGatewayIP = fieldValue;
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%s]", sectionName.c_str(), fieldName.c_str(), FGatewayIP.c_str());
    
    fieldName = "GatewayPort";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FGatewayPort = fieldValue.ToInt();
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%d]", sectionName.c_str(), fieldName.c_str(), FGatewayPort);
    
    fieldName = "RejectMarketOrder";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FRejectMarketOrd = fieldValue.ToInt();
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%d]", sectionName.c_str(), fieldName.c_str(), FRejectMarketOrd);
    
    fieldName = "ID";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FID = fieldValue;
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%s]", sectionName.c_str(), fieldName.c_str(), FID.c_str());

    fieldName = "Password";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FPassword = fieldValue;
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%s]", sectionName.c_str(), fieldName.c_str(), FPassword.c_str());

    fieldName = "Account";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FAccount = fieldValue;
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%s]", sectionName.c_str(), fieldName.c_str(), FAccount.c_str());

    fieldName = "BrokerID";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FBrokerID = fieldValue;
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%s]", sectionName.c_str(), fieldName.c_str(), FBrokerID.c_str());

    fieldName = "ClearMemberID";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FClearMemberID = fieldValue;
    else
    {
        if (FBrokerID.Length() > 0)
            FClearMemberID = FBrokerID.SubString(0, 4);
        else
            UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%s]", sectionName.c_str(), fieldName.c_str(), FClearMemberID.c_str());
    }

    fieldName = "TrimAEZero";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
    {
        if (fieldValue.Length() > 0)
        {
            char valueChar = fieldValue[0];
            if ((valueChar == '1') || (valueChar == 'Y') || (valueChar == 'y'))
                FDoTrimAEZero = true;
            else
                FDoTrimAEZero = false;
        }
    }
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[false]", sectionName.c_str(), fieldName.c_str());
    
    fieldName = "ReleaseMarginOnFilled";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FReleaseMarginOnFilled = fieldValue.ToInt();
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%d]", sectionName.c_str(), fieldName.c_str(), FReleaseMarginOnFilled);
    
    fieldName = "ReleaseMarginOnRODCanceled";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FReleaseMarginOnRODCxl = fieldValue.ToInt();
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%d]", sectionName.c_str(), fieldName.c_str(), FReleaseMarginOnRODCxl);

    fieldName = "ReleaseMarginOnIOCFOKCanceled";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FReleaseMarginOnFOKIOCCxl = fieldValue.ToInt();
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%d]", sectionName.c_str(), fieldName.c_str(), FReleaseMarginOnFOKIOCCxl);

    fieldName = "MaxQuoteOrderQty";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FMaxQuoteOrderQty = fieldValue.ToInt();
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%d]", sectionName.c_str(), fieldName.c_str(), FMaxQuoteOrderQty);

    fieldName = "ClearFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FClearAmountFileName.Printf("../cfg/%s", fieldValue.c_str());
    else
    {
        if ((gUniRMLogPtr != 0) && (FClearAmountFileName.Length() > 0))
            gUniRMLogPtr->fprintf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), FClearAmountFileName.c_str());
    }
    
    sectionName = "WhiteList";
    fieldName = "Future";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
    {
        UFC::PStringList futureList;
        futureList.SetStrings(fieldValue, ",");
        for (int i = 0; i < futureList.ItemCount(); i++)
        {
            UFC::AnsiString curContractCode = futureList.GetItem(i);
            CCommodityObject *curCommodityPtr = FindCommodity(curContractCode);
            if (curCommodityPtr != 0)
                curCommodityPtr->SetEnabled(true);
            else
                UFC::BufferedLog::Printf(" WhiteList %d Future[%s] not Found.", i, curContractCode.c_str());
        }        
    }
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting", sectionName.c_str(), fieldName.c_str());
    
    fieldName = "Option";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
    {
        UFC::PStringList optionList;
        optionList.SetStrings(fieldValue, ",");
        for (int i = 0; i < optionList.ItemCount(); i++)
        {
            UFC::AnsiString curContractCode = optionList.GetItem(i);
            CCommodityObject *curCommodityPtr = FindCommodity(curContractCode);
            if (curCommodityPtr != 0)
                curCommodityPtr->SetEnabled(true);
            else
                UFC::BufferedLog::Printf(" WhiteList %d Option[%s] not Found.", i, curContractCode.c_str());
        }        
    }
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting", sectionName.c_str(), fieldName.c_str());
    
    sectionName = "ToSpeedy";
    fieldName = "Subject";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FToSpeedySubject = fieldValue;
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%s]", sectionName.c_str(), fieldName.c_str(), FToSpeedySubject.c_str());
    
    fieldName = "Key";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FToSpeedyKey = fieldValue;
    else
        UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%s]", sectionName.c_str(), fieldName.c_str(), FToSpeedyKey.c_str());

    sectionName = "Futures";
    UFC::Section *futuresSecPtr = configFilePtr->GetSection(sectionName);
    if (futuresSecPtr != 0)
    {
        for (int i = 0; i < futuresSecPtr->ItemCount(); i++)
        {
            if (futuresSecPtr->GetNameValue(i, fieldName, fieldValue) == TRUE)
            {
                if (fieldName == "Default") continue;
                CCommodityObject *commodityPtr = FindCommodity(fieldName);
                if (commodityPtr != 0)
                {
                    int valueLength = fieldValue.Length();
                    double futDeposit = 0.0;
                    if ((valueLength > 1) && (fieldValue[valueLength - 1] == '%'))  //Percent
                    {
                        futDeposit = fieldValue.SubString(0, valueLength - 1).ToDouble();
                        commodityPtr->SetMarginType('1');
                    }
                    else
                    {
                        futDeposit = fieldValue.ToDouble();
                        commodityPtr->SetMarginType('0');
                    }
                    commodityPtr->SetInitialMargin(futDeposit);
                }
                else
                    UFC::BufferedLog::Printf( " Section[%s] Commodity[%s] not Found.", sectionName.c_str(), fieldName.c_str());
            }
        }  //for (int i = 0; i < futuresSecPtr->ItemCount(); i++)
    }
    else
        UFC::BufferedLog::Printf(" Section[%s] not Exist.", sectionName.c_str());
        
    sectionName = "OptionsA";
    UFC::Section *optionASecPtr = configFilePtr->GetSection(sectionName);
    if (optionASecPtr != 0)
    {
        for (int i = 0; i < optionASecPtr->ItemCount(); i++)
        {
            if (optionASecPtr->GetNameValue(i, fieldName, fieldValue) == TRUE)
            {
                if (fieldName == "Default") continue;
                CCommodityObject *commodityPtr = FindCommodity(fieldName);
                if (commodityPtr != 0)
                {
                    int valueLength = fieldValue.Length();
                    double futDeposit = 0.0;
                    if ((valueLength > 1) && (fieldValue[valueLength - 1] == '%'))  //Percent
                    {
                        futDeposit = fieldValue.SubString(0, valueLength - 1).ToDouble();
                        commodityPtr->SetMarginType('1');
                    }
                    else
                    {
                        futDeposit = fieldValue.ToDouble();
                        commodityPtr->SetMarginType('0');
                    }
                    commodityPtr->SetInitialMargin(futDeposit);
                }
                else
                    UFC::BufferedLog::Printf( " Section[%s] Commodity[%s] not Found.", sectionName.c_str(), fieldName.c_str());
            }
        }  //for (int i = 0; i < optionASecPtr->ItemCount(); i++)
    }
    else
        UFC::BufferedLog::Printf(" Section[%s] not Exist.", sectionName.c_str());

    sectionName = "Currency";
    UFC::Section *currencySecPtr = configFilePtr->GetSection(sectionName);
    if (currencySecPtr != 0)
    {
        for (int i = 0; i < currencySecPtr->ItemCount(); i++)
        {
            if (currencySecPtr->GetNameValue(i, fieldName, fieldValue) == TRUE)
            {
                if ((fieldName.Length() > 0) && (fieldValue.Length() > 0))
                {
                    char curCurrencyType = fieldValue[0];
                    CCurrencyObject* currencyObjPtr = new CCurrencyObject(fieldName, curCurrencyType);
                    LockCurrencyObject(UFC::rwLockForWrite);
                    bool isInsertCurrencySuccess = InsertCurrencyToCodeIndex(fieldName, currencyObjPtr, UFC::cstCheck, UFC::rwNotLock);
                    if (isInsertCurrencySuccess)
                    {
                        isInsertCurrencySuccess = InsertCurrencyToTypeIndex(curCurrencyType, currencyObjPtr, UFC::cstCheck, UFC::rwNotLock);
                        if (!isInsertCurrencySuccess)
                        {
                            RemoveCurrencyFromCodeIndex(fieldName, UFC::rwNotLock);
                            UFC::BufferedLog::Printf(" %s Duplicate CurrencyType[%c].", logHead.c_str(), curCurrencyType);
                        }
                    }
                    else
                        UFC::BufferedLog::Printf(" %s Duplicate CurrencyCode[%s].", logHead.c_str(), fieldName.c_str());
                    UnlockCurrencyObject(UFC::rwLockForWrite);
                    if (!isInsertCurrencySuccess) delete currencyObjPtr;
                }  //if ((fieldName.Length() > 0) && (fieldValue.Length() > 0))
            }  //if (currencySecPtr->GetNameValue(i, fieldName, fieldValue) == TRUE)
        }  //for (int i = 0; i < currencySecPtr->ItemCount(); i++)
    }
    else
        UFC::BufferedLog::Printf(" %s Section[%s] not Exist.", logHead.c_str(), sectionName.c_str());
    
    UFC::AnsiString subAccountSectionPrefix = "SubAccount_";
    for (int sectionIndex = 0; sectionIndex < configFilePtr->SectionCount(); sectionIndex++)
    {
        UFC::Section *subAccountSecPtr = configFilePtr->GetSection(sectionIndex);
        if (subAccountSecPtr == 0) continue;
        
        UFC::AnsiString curSectionName = subAccountSecPtr->GetSectionName();
        if (curSectionName.AnsiPos(subAccountSectionPrefix) != 0) continue;            
        
        UFC::PStringList subAccountNameList;
        subAccountNameList.SetStrings(curSectionName, "_", false);  //Comma-Separated Values, ignore empty field
        int subAccountNameListCount = subAccountNameList.ItemCount();
        if (subAccountNameListCount < 3)
        {
            UFC::BufferedLog::Printf(" %s Wrong SubAccountSection[%s].", logHead.c_str(), curSectionName.c_str());
            continue;
        }
        
        UFC::AnsiString curSubAccount = subAccountNameList[1];
        curSubAccount.TrimLeft('0');
        curSubAccount.PadThis(6, ' ');
        
        UFC::AnsiString curCurrencyCode = subAccountNameList[2];
        curCurrencyCode.TrimRight(' ');
        if (curCurrencyCode.Length() <= 0) 
        {
            UFC::BufferedLog::Printf(" %s SubAccountSection[%s] CurrenctCode is NULL.", logHead.c_str(), curSectionName.c_str());
            continue;
        }
        
        char curCurrencyType = 0;
        CCurrencyObject* curCurrencyObjPtr = FindCurrencyFromCodeIndex(curCurrencyCode, UFC::rwLockForRead);
        if (curCurrencyObjPtr != 0)
            curCurrencyType = curCurrencyObjPtr->GetCurrencyType();
        else
        {
            UFC::BufferedLog::Printf(" %s Wrong SubAccountSection[%s]: UnSupport CurrenctCode[%s].", logHead.c_str(), curSectionName.c_str(), curCurrencyCode.c_str());
            continue;
        }
        
        UFC::AnsiString curFieldName = "ReserveDeposit";
        UFC::AnsiString curFieldValue = "";
        double curReserveDeposit = 0.0;
        if (subAccountSecPtr->GetValue(curFieldName, curFieldValue) == TRUE)
            curReserveDeposit = curFieldValue.ToDouble();
        else
            UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%0.4lf]", logHead.c_str(), curSectionName.c_str(), curFieldName.c_str(), curReserveDeposit);
            
        curFieldName = "NearMonthOnly";
        curFieldValue = "";
        bool curIsNearMonthOnly = false;
        if (subAccountSecPtr->GetValue(curFieldName, curFieldValue) == TRUE)
        {
            if (curFieldValue == "1") 
                curIsNearMonthOnly = true;
            else
                curIsNearMonthOnly = false;
        }
        else
            UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:false", logHead.c_str(), curSectionName.c_str(), curFieldName.c_str());

        curFieldName = "QuoteMatchQtyLimit";
        curFieldValue = "";
        int curQuoteMatchQtyLimit = 0;
        if (subAccountSecPtr->GetValue(curFieldName, curFieldValue) == TRUE)
            curQuoteMatchQtyLimit = curFieldValue.ToInt();
        else
            UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%d]", logHead.c_str(), curSectionName.c_str(), curFieldName.c_str(), curQuoteMatchQtyLimit);

        CAccountExecutive *curAEPtr = FindSubAccount(curSubAccount, curCurrencyType);
        if (curAEPtr == 0)
        {
            curAEPtr = new CAccountExecutive(curSubAccount, curCurrencyType);
            if (InsertSubAccount(curSubAccount, curCurrencyType, curAEPtr))
                curAEPtr->SetCurrencyCode(curCurrencyCode);
            else
            {
                UFC::BufferedLog::Printf(" %s Insert SubAccount[%s] Currency[%c][%s] failed.", logHead.c_str(), curSubAccount.c_str(), curCurrencyType, curCurrencyCode.c_str());
                delete curAEPtr;
                curAEPtr = 0;
            }
        }
        else
            UFC::BufferedLog::Printf(" %s Duplicate SubAccount[%s] Currency[%c][%s]", logHead.c_str(), curSubAccount.c_str(), curCurrencyType, curCurrencyCode.c_str());
        
        if (curAEPtr != 0)
        {
            curAEPtr->SetOnlyNearMonth(curIsNearMonthOnly);
            curAEPtr->SetQuoteMatchQtyLimit(curQuoteMatchQtyLimit);
            curAEPtr->SetReserveDeposit(curReserveDeposit);
        }
    }  //for (int sectionIndex = 0; sectionIndex < configFilePtr->SectionCount(); sectionIndex++)
/*    
    UFC::AnsiString subAccountSectionPrefix = "SubAccount_";
    int subAccountSectionPrefixLength = subAccountSectionPrefix.Length();
    for (int sectionIndex = 0; sectionIndex < configFilePtr->SectionCount(); sectionIndex++)
    {
        UFC::Section *subAccountSecPtr = configFilePtr->GetSection(sectionIndex);
        if (subAccountSecPtr == 0) continue;
        
        UFC::AnsiString curSectionName = subAccountSecPtr->GetSectionName();
        if (curSectionName.SubString(0, subAccountSectionPrefixLength) != subAccountSectionPrefix) continue;
        UFC::BufferedLog::Printf(" Load Section[%s].", curSectionName.c_str());
        
        UFC::AnsiString curSubAccount = curSectionName.SubString(subAccountSectionPrefixLength, curSectionName.Length() - subAccountSectionPrefixLength);
        curSubAccount.TrimLeft('0');
        curSubAccount.PadThis(6, ' ');
        
        UFC::AnsiString curFieldName = "ReserveDeposit";
        UFC::AnsiString curFieldValue = "";
        double curReserveDeposit = 0.0;
        if (subAccountSecPtr->GetValue(curFieldName, curFieldValue) == TRUE)
            curReserveDeposit = curFieldValue.ToDouble();
        else
            UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%0.3lf]", curSectionName.c_str(), curFieldName.c_str(), curReserveDeposit);
            
        curFieldName = "NearMonthOnly";
        curFieldValue = "";
        bool curIsNearMonthOnly = false;
        if (subAccountSecPtr->GetValue(curFieldName, curFieldValue) == TRUE)
        {
            if (curFieldValue == "1") 
                curIsNearMonthOnly = true;
            else
                curIsNearMonthOnly = false;
        }
        else
            UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:false", curSectionName.c_str(), curFieldName.c_str());

        curFieldName = "QuoteMatchQtyLimit";
        curFieldValue = "";
        int curQuoteMatchQtyLimit = 0;
        if (subAccountSecPtr->GetValue(curFieldName, curFieldValue) == TRUE)
            curQuoteMatchQtyLimit = curFieldValue.ToInt();
        else
            UFC::BufferedLog::Printf(" [%s-%s] not Setting, use Default Value:[%d]", curSectionName.c_str(), curFieldName.c_str(), curQuoteMatchQtyLimit);

        CAccountExecutive *curAEPtr = FindSubAccount(curSubAccount);
        if (curAEPtr == 0)
        {
            curAEPtr = new CAccountExecutive(curSubAccount);
            if (!InsertSubAccount(curSubAccount, curAEPtr))
            {
                UFC::BufferedLog::Printf(" GCRMConnection::LoadSetting() Insert SubAccount[%s] failed.", curSubAccount.c_str());
                delete curAEPtr;
                curAEPtr = 0;
            }
        }
        else
            UFC::BufferedLog::Printf(" GCRMConnection::LoadSetting() Duplicate SubAccount[%s]", curSubAccount.c_str());
        
        if (curAEPtr != 0)
        {
            curAEPtr->SetOnlyNearMonth(curIsNearMonthOnly);
            curAEPtr->SetQuoteMatchQtyLimit(curQuoteMatchQtyLimit);
            curAEPtr->SetReserveDeposit(curReserveDeposit);
        }
    }  //for (int sectionIndex = 0; sectionIndex < configFilePtr->SectionCount(); sectionIndex++)    
 */
}  //GCRMConnection::LoadSetting()
//------------------------------------------------------------------------------
GCRMConnection::~GCRMConnection( void )
{
    ///< Delete TTaifexConnection object if exists.    
    if( FConnection != NULL )
    {
        FConnection->Logoff();
        delete FConnection;
    }
    
    if (FCheckLogPtr != 0) delete FCheckLogPtr;
}
//------------------------------------------------------------------------------
bool GCRMConnection::ClearSubAccountOrderAmount(const UFC::AnsiString& AccountNo, const UFC::AnsiString& AENo, const UFC::AnsiString& CurrencyCode)
{
    bool isSuccess = false;
    char currencyType = 0;
    CCurrencyObject* currencyObjPtr = 0;
    if (CurrencyCode.Length() > 0)
    {
        char currencyType = CurrencyCode[0];
        if (CurrencyCode.Length() >= 3)
            currencyObjPtr = FindCurrencyFromCodeIndex(CurrencyCode, UFC::rwLockForRead);
        if (currencyObjPtr == 0) currencyObjPtr = FindCurrencyFromTypeIndex(currencyType, UFC::rwLockForRead);
    }
            
    if (currencyObjPtr != 0)
    {
        currencyType = currencyObjPtr->GetCurrencyType();        
        AccountInfo *accInfoPtr = GetAccountInfo( AccountNo, AENo, currencyType );   //AccountInfoTable.GetObjectByKey( accountKey );
        if( accInfoPtr != 0 )
        {
//            accInfoPtr->LockForWrite();
            double origMatchAmount = accInfoPtr->GetMatchAmount();
            double origQuoteMatchAmount = accInfoPtr->GetQuoteMatchAmount();
            double origOrderAmount = accInfoPtr->GetOrderAmount();
            double origQuoteOrderAmount = accInfoPtr->GetQuoteOrderAmount();
            int origQuoteMatchQty = accInfoPtr->GetQuoteMatchQty();
            double newOrderAmount = 0.0;
            if ((origMatchAmount > 0.0) && (origOrderAmount >= origMatchAmount))
                newOrderAmount = origOrderAmount - origMatchAmount;
            double newQuoteOrderAmount = 0.0;
            accInfoPtr->SetOrderAmount( newOrderAmount );
            accInfoPtr->SetQuoteOrderAmount( newQuoteOrderAmount );
            accInfoPtr->SetMatchAmount( 0.0 );
            accInfoPtr->SetQuoteMatchAmount( 0.0 );
            accInfoPtr->SetQuoteMatchQty( 0 );
//            accInfoPtr->UnLockForWrite();
            UFC::BufferedLog::Printf(" %s() Set Account[%s][%s][%c] Information: order[%0.4lf=>%0.4lf] quoteOrder[%0.4lf=>%0.4lf] match[%0.4lf=>0.00] quoteMatch[%0.4lf=>0.00] quoteMatchQty[%d=>0]",
                                     __func__, AccountNo.c_str(), AENo.c_str(), currencyType, origOrderAmount, newOrderAmount, origQuoteOrderAmount, newQuoteOrderAmount, origMatchAmount, origQuoteMatchAmount, origQuoteMatchQty);
        }
        else
            UFC::BufferedLog::Printf(" %s() Account[%s] AE[%s] Currency[%s]'%c' not exist.", __func__, AccountNo.c_str(), AENo.c_str(), CurrencyCode.c_str(), currencyType );
    }
    else
        UFC::BufferedLog::Printf( " Ignore Account[%s] AE[%s], not Support Currency[%s]", AccountNo.c_str(), AENo.c_str(), CurrencyCode.c_str() );
    return isSuccess;
}  //GCRMConnection::ClearSubAccountOrderAmount()
//------------------------------------------------------------------------------
bool GCRMConnection::ClearOrderAmount(const UFC::AnsiString& ClearFileName)
{
    bool isSuccess = false;
    if (ClearFileName.Length() <= 0)
    {
        UFC::BufferedLog::Printf(" %s() Clear FileName is NULL.", __func__);
        return isSuccess;
    }
    
    if (UFC::FileExists(ClearFileName) == FALSE)
    {
        UFC::BufferedLog::Printf(" %s() Clear File[%s] not Exist.", __func__, ClearFileName.c_str());
        return isSuccess;
    }
        
    UFC::UiniFile *clearFilePtr = 0;
    try
    {
        clearFilePtr = new UFC::UiniFile(ClearFileName);
    }
    catch(UFC::FileException& fe)
    {
        UFC::BufferedLog::Printf(" %s() Open Clear File[%s] failed. Exception:%s", __func__, ClearFileName.c_str(), fe.what());
        clearFilePtr = 0;
    }
    
    if (clearFilePtr == 0) return isSuccess;
    isSuccess = true;
    
    UFC::AnsiString sectionName = "Clear";
    UFC::AnsiString fieldName = "SubAccount";
    UFC::AnsiString fieldValueStr = "";
    if (clearFilePtr->GetValue(sectionName, fieldName, fieldValueStr) == TRUE)
    {
        UFC::PStringList subAccountList;
        subAccountList.SetStrings(fieldValueStr, ",", false);  //Comma-Separated Values, Ignore empty field
        int subAccountCount = subAccountList.ItemCount();
        for (int j = 0; j < subAccountCount; j++)
        {
            UFC::AnsiString curSubAccountStr = subAccountList.GetItem(j);
            UFC::PStringList subAccountNoPair;
            subAccountNoPair.SetStrings(curSubAccountStr, "-", false);  //Comma-Separated Values, Ignore empty field
            if (subAccountNoPair.ItemCount() == 3)
            {
                UFC::AnsiString curAccountNo = subAccountNoPair.GetItem(0);
                UFC::AnsiString curSubAccountNo = subAccountNoPair.GetItem(1);
                UFC::AnsiString curCurrencyCode = subAccountNoPair.GetItem(2);
                if (curSubAccountNo.Length() < 6) curSubAccountNo.PadThis(6, ' ', true);  //pad right
                gUniRMLogPtr->fprintf(" %s() %d SubAccount[%s][%s][%s].", __func__, j + 1, curAccountNo.c_str(), curSubAccountNo.c_str(), curCurrencyCode.c_str());
                ClearSubAccountOrderAmount(curAccountNo, curSubAccountNo, curCurrencyCode);
            }
            else
                UFC::BufferedLog::Printf(" %s() Wrong SubAccount No Pair[%s].", __func__, curSubAccountStr.c_str());
        }
    }
    else
        UFC::BufferedLog::Printf(" %s() [%s][%s] not Setting.", __func__, sectionName.c_str(), fieldName.c_str());

    delete clearFilePtr;
    return isSuccess;
}  //GCRMConnection::ClearOrderAmount()
//------------------------------------------------------------------------------
double GCRMConnection::GetCommodityDeposit(const UFC::AnsiString& Symbol, double Price, bool IsBuy, bool IsMultiLeg, const UFC::AnsiString& Leg1Symbol, const UFC::AnsiString& Leg2Symbol)
{
    UFC::AnsiString logHead = "GetCommodityDeposit()";
    double commodityDeposit = -1.0;
    UFC::AnsiString contractCode = Symbol.SubString(0, 3);
    CCommodityObject *commodityPtr = FindCommodity(contractCode);
    if (commodityPtr == 0)
    {
        UFC::BufferedLog::Printf(" %s Commodity[%s] of Symbol[%s] not exist.", logHead.c_str(), contractCode.c_str(), Symbol.c_str());
        return commodityDeposit;
    }
    
    if (!commodityPtr->IsEnabled())
    {
        UFC::BufferedLog::Printf(" %s Commodity[%s] of Symbol[%s] not in WhiteList.", logHead.c_str(), contractCode.c_str(), Symbol.c_str());
        return commodityDeposit;
    }

    char currencyType = commodityPtr->GetCurrencyType();
    char marginType = commodityPtr->GetMarginType();  //0:Amount 1:Percent
    double initialMargin = commodityPtr->GetInitialMargin();
    char productKind = commodityPtr->GetContractKind();  //I:Index, R:Rate, B:Bond, C:Commodity, S:Stock
    UFC::BufferedLog::Printf(" %s Commodity[%s] of Symbol[%s] Currency[%c] ProdKind[%c] MarginType[%c] InitialMargin[%16.4lf].", logHead.c_str(), contractCode.c_str(), Symbol.c_str(), currencyType, productKind, marginType, initialMargin);
    CCommodityObject *defaultCommodityPtr = 0;
    if ((marginType != '0') && (marginType != '1'))
    {
        if ((contractCode == "TX1") || (contractCode == "TX2") || (contractCode == "TX4") || (contractCode == "TX5") ||
            (contractCode == "TXU") || (contractCode == "TXV") || (contractCode == "TXX") || (contractCode == "TXY") ||
            (contractCode == "TXZ") || (contractCode == "TFO") || (contractCode == "TGO") || (contractCode == "XIO"))
        {
            UFC::AnsiString defaultContractCode = "TXO";
            UFC::BufferedLog::Printf(" Commodity[%s] not set MarginType. Use Default Commodity[%s]", contractCode.c_str(), defaultContractCode.c_str());
            defaultCommodityPtr = FindCommodity(defaultContractCode);
            if (defaultCommodityPtr != 0) 
            {
                marginType = defaultCommodityPtr->GetMarginType();
                initialMargin = defaultCommodityPtr->GetInitialMargin();
            }
            else
                UFC::BufferedLog::Printf(" Default Commodity[%s] not exist.", defaultContractCode.c_str());
        }
    }  //if ((marginType != '0') && (marginType != '1'))
    
    if (IsMultiLeg && commodityPtr->IsFutures() && (productKind == 'S') && (marginType == '1'))  //'S':Stock '1':Percent
    {
        if (Leg1Symbol.Length() > 0)
        {
            CContractObject* contractPtr = FindContract(Leg1Symbol);
            if (contractPtr != 0)
            {
                double contractMultiplier = commodityPtr->GetContractMultiplier();
                double premium = contractPtr->GetPremium();
                commodityDeposit = premium * contractMultiplier * (initialMargin / 100.0);
                UFC::BufferedLog::Printf(" Deposit[%13.3lf] = Premium[%13.3lf] * CM[%13.3lf] * InitialMargin[%13.3lf%%].", premium, contractMultiplier, initialMargin);
            }
            else
                UFC::BufferedLog::Printf(" %s MultiLeg[%s] Leg1[%s] not Exist.", logHead.c_str(), Symbol.c_str(), Leg1Symbol.c_str());
        }
        else
            UFC::BufferedLog::Printf(" %s MultiLeg[%s] Leg1 is NULL.", logHead.c_str(), Symbol.c_str());
    }
    else if (IsMultiLeg && !commodityPtr->IsFutures())
    {
        if (marginType == '0')  //Amount
        {
            commodityDeposit = 3.0 * initialMargin;
            UFC::BufferedLog::Printf(" MultiLeg[%s] CommodityDeposit[%13.3lf] = 3.0 * InitialMargin[%13.3lf].", Symbol.c_str(), commodityDeposit, initialMargin);
        }
        else  // 1:Percent
            UFC::BufferedLog::Printf(" %s MultiLeg[%s] has no InitialMargin.", logHead.c_str(), Symbol.c_str());
    }
    else
    {
        double contractMultiplier = commodityPtr->GetContractMultiplier();
        if (!commodityPtr->IsFutures() && IsBuy)
        {
            commodityDeposit = Price * contractMultiplier;
            UFC::BufferedLog::Printf(" Buy[%s] Deposit[%13.3lf] = Px[%13.3lf] * CM[%13.3lf]", Symbol.c_str(), commodityDeposit, Price, contractMultiplier);
        }
        else
        {
            double aValue = 0.0;
            if (marginType == '0')  //0:Amount
                aValue = initialMargin;
            else  //1:Percent
            {
                aValue = Price * contractMultiplier * (initialMargin / 100.0);
                UFC::BufferedLog::Printf(" AValue[%13.3lf] = Price[%13.3lf] * CM[%13.3lf] * InitialMargin[%13.3lf%%].", aValue, Price, contractMultiplier, initialMargin);
            }
        
            if (commodityPtr->IsFutures())
                commodityDeposit = aValue;
            else
            {
                if (!IsBuy)
                {
                    CCommodityObject *mxfCommodityPtr = FindCommodity("MXF");
                    if (mxfCommodityPtr != 0)
                    {
                        commodityDeposit = mxfCommodityPtr->GetInitialMargin();
                        UFC::BufferedLog::Printf(" Sell[%s] Deposit[%13.3lf] = MXF InitialMargin", Symbol.c_str(), commodityDeposit);
                    }
                    else
                    {
                        commodityDeposit = (Price * contractMultiplier) + aValue;
                        UFC::BufferedLog::Printf(" Sell[%s] Deposit[%13.3lf] = Px[%13.3lf] * CM[%13.3lf] + AVal[%13.3lf]", Symbol.c_str(), commodityDeposit, Price, contractMultiplier, aValue);
                    }
                }
            }  //if (commodityPtr->IsFutures())
        }  //if (!commodityPtr->IsFutures() && IsBuy)
    }  //if (IsMultiLeg && !commodityPtr->IsFutures())
    UFC::BufferedLog::FlushToFile();
    return commodityDeposit;
}  //GCRMConnection::GetCommodityDeposit()
//------------------------------------------------------------------------------
bool GCRMConnection::IsReplacePx( const UFC::AnsiString& Order )
{
    if( Order.SubString( 2, 2 ).ToInt() == 6 )
        return true;
    return false;
}
//---------------------------------------------------------------------------
void GCRMConnection::GetAE( const UFC::AnsiString& Key, UFC::AnsiString& AE )
{   ///< Layout for the "KEY" field.
    ///< AE,Data
    Int32 SplitPos = Key.AnsiPos(',');

    if( SplitPos !=  -1 )
        AE = Key.SubString( 0, SplitPos );
    else
        AE = Key;
}
//------------------------------------------------------------------------------
void GCRMConnection::UseMargin( const UFC::AnsiString& Account, UFC::AnsiString& AE )
{
    UFC::AnsiString Value;
    int MarginLevel = 3; ///< Default: AE Margin

    if( ( FConfig != 0 ) && ( FConfig->GetValue( Account, AE, Value ) == TRUE ) )
        MarginLevel = Value.ToInt();
    if( MarginLevel == 1 )
    {
        AE = "      ";
        UFC::BufferedLog::Printf( " Use Company margin. Account[%s] AE[%s] ", Account.c_str(), AE.c_str() );
    }
    else if( MarginLevel == 2 )
    {
        AE.SetLength( 3 );
        AE.PadThis( 6, ' ');
        UFC::BufferedLog::Printf( " Use Group margin. Account[%s] AE[%s] ", Account.c_str(), AE.c_str() );
    }
    else
        UFC::BufferedLog::Printf( " Use AE margin. Account[%s] AE[%s] ", Account.c_str(), AE.c_str() );
}
//------------------------------------------------------------------------------
bool GCRMConnection::GetOrderTreeNode(MTree *Data, UFC::AnsiString& OrderStr, Int32& NID, UFC::AnsiString& UserKey)
{
    UFC::AnsiString logHead = "GCRMConnection::GetOrderTreeNode()";
    if (Data->get( "ORDER", OrderStr ) == TRUE)
    {   // 0         1         2         3         4         5         6         7         8         9
        // 0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
        //[30010012261700F001000XZa060000003DXIFI8               BL0000002550010R0F001]            KEY[yuan,yuan;1Y;700T] NID[26641]
        //[30010815432400F001000XZa090000000 XIFI8               00000025500000026700100010R9F001] KEY[yuan] NID[2068]
        if (Data->get( "NID", NID ) == TRUE)
        {
            if (Data->get( "KEY", UserKey ) == TRUE)
            {
                UFC::BufferedLog::Printf( " Receive from Speedy Gateway: ORDER[%s] KEY[%s] NID[%ld]", OrderStr.c_str(), UserKey.c_str(), NID);
                return true;
            }
            else
                UFC::BufferedLog::Printf(" %s Missing KEY node.", logHead.c_str());
        }
        else
            UFC::BufferedLog::Printf(" %s Missing NID node.", logHead.c_str());
    }
    else
        UFC::BufferedLog::Printf(" %s Missing ORDER node.", logHead.c_str());
    return false;
}  //GCRMConnection::GetOrderTreeNode()
//------------------------------------------------------------------------------
void GCRMConnection::PassReplacePriceOrder(nsOrderMessageDefine::MarketEnum OrderMarket, MTree *Data)
{
    Data->append( "CODE", 1 );
    if (OrderMarket == nsOrderMessageDefine::mTWFutures)
        FMessageObject->Send("RESULT.FUT", UFC::Hostname, *Data);
    else
        FMessageObject->Send("RESULT.OPT", UFC::Hostname, *Data);
}  //GCRMConnection::PassReplacePriceOrder()
//------------------------------------------------------------------------------
bool GCRMConnection::RejectMarketOrder(nsOrderMessageDefine::MarketEnum OrderMarket, char OrderType, MTree *Data)
{
    if ((OrderType == 'M') &&  ///< Reject Market Order.
        ((FRejectMarketOrd == TRUE) || (OrderMarket == nsOrderMessageDefine::mTWOptions))) 
    {
        UFC::BufferedLog::Printf(" Speedy RM Reject Market Order.");
        Data->append("CODE", 0);
        Data->append("MSG", "Can't place market order.");
        if (OrderMarket == nsOrderMessageDefine::mTWFutures)
            FMessageObject->Send("RESULT.FUT", UFC::Hostname, *Data);
        else
            FMessageObject->Send("RESULT.OPT", UFC::Hostname, *Data);
        return true;
    }
    return false;
}  //GCRMConnection::RejectMarketOrder()
//------------------------------------------------------------------------------
void GCRMConnection::RejectCommodityNotExist(nsOrderMessageDefine::MarketEnum OrderMarket, const UFC::AnsiString& ContractCode, MTree *Data)
{
    UFC::BufferedLog::Printf(" Can't fetch the record: [%s]", ContractCode.c_str());    
    UFC::AnsiString errorMsg = "Commodity not exists.";
    UFC::BufferedLog::Printf(" Speedy RM Reject Order:[%s]", errorMsg.c_str());    
    
    Data->append("CODE", 0);
    Data->append("MSG", errorMsg);
    if (OrderMarket == nsOrderMessageDefine::mTWFutures)
        FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
    else
        FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
}  //GCRMConnection::RejectCommodityNotExist()
//------------------------------------------------------------------------------
void GCRMConnection::RejectAccountInfoNotExist(nsOrderMessageDefine::MarketEnum OrderMarket, const UFC::AnsiString& Account, const UFC::AnsiString& AE, char currencyType, MTree *Data)
{
    UFC::BufferedLog::Printf(" Can't fetch the record: [%s][%s][%c]", Account.c_str(), AE.c_str(), currencyType);
    UFC::BufferedLog::FlushToFile();
    UFC::AnsiString errorMsg = "Account not exists.";
    UFC::BufferedLog::Printf(" Speedy RM Reject Order:[%s]", errorMsg.c_str());
    Data->append("CODE", 0);
    Data->append("MSG", errorMsg);
    if (OrderMarket == nsOrderMessageDefine::mTWFutures)
        FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
    else
        FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
}  //GCRMConnection::RejectAccountInfoNotExist()
//------------------------------------------------------------------------------
void GCRMConnection::RejectNearMonthOrder(nsOrderMessageDefine::MarketEnum OrderMarket, MTree *Data)
{
    UFC::BufferedLog::Printf( " Speedy RM Reject far month Order.");
    UFC::BufferedLog::FlushToFile();
    Data->append( "CODE", 0 );
    Data->append( "MSG", "place near month order only." );
    if (OrderMarket == nsOrderMessageDefine::mTWFutures )
        FMessageObject->Send("RESULT.FUT", UFC::Hostname, *Data);
    else
        FMessageObject->Send("RESULT.OPT", UFC::Hostname, *Data);
}  //GCRMConnection::RejectNearMonthOrder()
//------------------------------------------------------------------------------
void GCRMConnection::RejectDepositNotExist(nsOrderMessageDefine::MarketEnum OrderMarket, const UFC::AnsiString& Symbol, MTree *Data)
{
    Data->append("CODE", 0);
    UFC::AnsiString errorMsg;
    errorMsg.Printf("Commodity[%s] Deposit not exists.", Symbol.c_str());
    Data->append("MSG", errorMsg);
    UFC::BufferedLog::Printf(" Speedy RM Reject Order:[%s]", errorMsg.c_str());
    UFC::BufferedLog::FlushToFile();
    if (OrderMarket == nsOrderMessageDefine::mTWFutures )
        FMessageObject->Send("RESULT.FUT", UFC::Hostname, *Data);
    else
        FMessageObject->Send("RESULT.OPT", UFC::Hostname, *Data);
}  //GCRMConnection::RejectDepositNotExist()
//------------------------------------------------------------------------------
void GCRMConnection::CheckFuturesOrderRisk(nsOrderMessageDefine::MarketEnum OrderMarket, MTree *Data)
{
    UFC::AnsiString orderStr, userKey;
    Int32 NID;
    if (!GetOrderTreeNode(Data, orderStr, NID, userKey)) return;
    
    UFC::AnsiString functionCode = orderStr.SubString(2, 2);  //01:New 02:Replace Qty 03:Cancel 06:Replace Price
    if (functionCode == "06")
    {
        PassReplacePriceOrder(OrderMarket, Data);
        return;
    }
    
    UFC::AnsiString messageType = orderStr.SubString(4, 2);   //00:Order 01:Report 08:Quote 09:QuoteReport
    UFC::AnsiString brokerID = orderStr.SubString(14, 7);
    UFC::AnsiString orderID = orderStr.SubString(21, 5);
    UFC::AnsiString account = orderStr.SubString(26, 7);
    UFC::AnsiString symbol = orderStr.SubString(34, 20);
    symbol.TrimRight(' ');
    UFC::AnsiString contractCode = symbol.SubString(0, 3);
    CCommodityObject *commodityPtr = FindCommodity(contractCode);
    char currencyType = ' ';
    if (commodityPtr != 0) 
        currencyType = commodityPtr->GetCurrencyType();
    else
    {
        RejectCommodityNotExist(OrderMarket, contractCode, Data);
        return;
    }

    UFC::AnsiString orderAE, backendAE;
    GetAE(userKey, orderAE);
    if ( FDoTrimAEZero )
        backendAE = TrimBeginZero( orderAE );
    else
        backendAE = orderAE;
    backendAE.PadThis(6, ' ');
    UseMargin(account, backendAE);
    CAccountExecutive *aePtr = 0;
    AccountInfo *accInfoPtr = GetAccountInfo(account, backendAE, currencyType);
    if (accInfoPtr == 0) ///< Account information not exists.
    {
        RejectAccountInfoNotExist(OrderMarket, account, backendAE, currencyType, Data);
        return;
    }
    else
    {
        aePtr = accInfoPtr->GetAEObj();
        if (aePtr == 0) aePtr = FindSubAccount(backendAE, currencyType);
    }
    
    UFC::AnsiString leg1Symbol = "";
    UFC::AnsiString leg2Symbol = "";
    bool isMultiLge = ParseMultiLegSymbol(OrderMarket, symbol, leg1Symbol, leg2Symbol);
    if ((aePtr != 0) && aePtr->IsOnlyNearMonth())
    {
        bool isCorrectOrder = false;
        if (isMultiLge)
        {
            CContractObject *leg1ContractPtr = FindNearMonthContract(leg1Symbol);
            CContractObject *leg2ContractPtr = FindNearMonthContract(leg2Symbol);
            if ((leg1ContractPtr != 0) && (leg2ContractPtr != 0)) isCorrectOrder = true;
        }
        else
            if (FindNearMonthContract(symbol) != 0) isCorrectOrder = true;
                    
        if (!isCorrectOrder)
        {
            RejectNearMonthOrder(OrderMarket, Data);
            return;
        }
    }  //if( aePtr != 0 )
    
    int precision = 2;
    if (messageType == "08")
    {
        CQuoteRequest* quoteReqPtr = new CQuoteRequest(OrderMarket, NID, orderStr, userKey);
        
        precision = FConnection->GetPricePrecision(OrderMarket, nsOrderMessageDefine::tsNormal, symbol);
        
        UFC::AnsiString bidPriceStr = orderStr.SubString(54, 9);
        double bidPrice = UFC::IntToDouble(bidPriceStr.ToInt(), precision);
        quoteReqPtr->SetBidQuotePrice(bidPrice);
        
        UFC::AnsiString askPriceStr = orderStr.SubString(63, 9);
        double askPrice = UFC::IntToDouble(askPriceStr.ToInt(), precision);
        quoteReqPtr->SetAskQuotePrice(askPriceStr);
        
        double bidCommodMargins = GetCommodityDeposit(symbol, bidPrice, true, false);
        double askCommodMargins = GetCommodityDeposit(symbol, askPrice, false, false);
        if ((bidCommodMargins < 0.0) || (askCommodMargins < 0.0))
        {
            RejectDepositNotExist(OrderMarket, symbol, Data);
            return;
        }
        
        Int32 bidQty = orderStr.SubString(72, 4).ToInt();
        Int32 askQty = orderStr.SubString(76, 4).ToInt();
        char openOffset = orderStr[81];
        UFC::BufferedLog::Printf(" %s() Future Quote Order[%s] Symbol[%s] Bid(Qty[%d] Px[%0.4lf] CommodMargins[%0.4lf]) Ask(Qty[%d] Px[%0.4lf] CommodMargins[%0.4lf]) BrokerID[%s] Account[%s] AE[%s][%c] OpenOffset[%c] precision[%d]", 
                                 __func__, orderID.c_str(), symbol.c_str(), bidQty, bidPrice, bidCommodMargins, askQty, askPrice, askCommodMargins, brokerID.c_str(), account.c_str(), backendAE.c_str(), currencyType, openOffset, precision);
        
        if (accInfoPtr->CanPlaceFuturesQuoteOrder(bidQty, bidCommodMargins, askQty, askCommodMargins, orderID, symbol, bidPrice, askPrice, account))
        {
            Data->append("CODE", 1);
            FMessageObject->Send("RESULT.FUT", UFC::Hostname, *Data);
            CQuoteOrderRecord *newQuoteOrderPtr = new CQuoteOrderRecord(OrderMarket, orderID, symbol, 0.0, bidQty, 0.0, askQty);
            if (!InsertQuoteOrder(orderID, newQuoteOrderPtr))
            {
                delete newQuoteOrderPtr;
                UFC::BufferedLog::Printf(" %s() Insert Future Quote Order[%s] failed.", __func__, orderID.c_str());
            }            
            
            if (!InsertRequestToNIDIndex(NID, quoteReqPtr, UFC::cstCheck, UFC::rwLockForWrite))
            {
                UFC::BufferedLog::Printf(" %s() Insert Future Quote Order[%s] to NID[%d] Index Failed.", __func__, orderID.c_str(), NID);
                delete quoteReqPtr;
            }
        }
        else
        {
            delete quoteReqPtr;
            Data->append("CODE", 0);
            Data->append("MSG", "Exceed Margin.");
            FMessageObject->Send("RESULT.FUT", UFC::Hostname, *Data);
        }
    }
    else
    {
        COrderRequest* orderReqPtr = new COrderRequest(OrderMarket, NID, orderStr, userKey);
        
        char orderType = orderStr[55];
        if (RejectMarketOrder(OrderMarket, orderType, Data)) return;

        char side = orderStr[54];
        UFC::AnsiString leg1Symbol = "";
        UFC::AnsiString leg2Symbol = "";
        bool isMultiLge = ParseMultiLegSymbol(OrderMarket, symbol, leg1Symbol, leg2Symbol);       
        UFC::AnsiString symbolStr;
        if (isMultiLge) 
            symbolStr.Printf("[%s(%s, %s)]", symbol.c_str(), leg1Symbol.c_str(), leg2Symbol.c_str());
        else
            symbolStr = symbol;
        UFC::AnsiString orderPriceStr = orderStr.SubString(56, 9);
        if (isMultiLge)
            precision = FConnection->GetPricePrecision(OrderMarket, nsOrderMessageDefine::tsNormal, leg1Symbol);
        else
            precision = FConnection->GetPricePrecision(OrderMarket, nsOrderMessageDefine::tsNormal, symbol);
        double orderPrice = UFC::IntToDouble(orderPriceStr.ToInt(), precision);
        orderReqPtr->SetOrderPrice(orderPrice);
        
        double commodMargins = GetCommodityDeposit(symbol, orderPrice, (side == 'B'), isMultiLge, leg1Symbol, leg2Symbol);
        if (commodMargins < 0.0)
        {
            RejectDepositNotExist(OrderMarket, symbol, Data);
            return;
        }
    
        Int32 orderQty = orderStr.SubString(65, 4).ToInt();
        char openOffset = orderStr[70];
        UFC::BufferedLog::Printf(" %s() Future Order[%s] Symbol[%s] Side[%c] Qty[%d] Px[%0.4lf] BrokerID[%s] Account[%s] AE[%s][%c] OpenOffset[%c] CommodMargins[%0.4lf] precision[%d]", 
                                 __func__, orderID.c_str(), symbolStr.c_str(), side, orderQty, orderPrice, brokerID.c_str(), account.c_str(), backendAE.c_str(), currencyType, openOffset, commodMargins, precision);

        if (accInfoPtr->CanPlaceFuturesOrder(orderQty, commodMargins, isMultiLge, orderID, symbol, side, orderPrice, account))
        {
            Data->append("CODE", 1);
            FMessageObject->Send("RESULT.FUT", UFC::Hostname, *Data);
            
            if (!InsertRequestToNIDIndex(NID, orderReqPtr, UFC::cstCheck, UFC::rwLockForWrite))
            {
                delete orderReqPtr;
                UFC::BufferedLog::Printf(" %s() Insert Future Order[%s] to NID[%d] Index Failed.", __func__, orderID.c_str(), NID);
            }
        }
        else
        {
            delete orderReqPtr;
            Data->append("CODE", 0);
            Data->append("MSG", "Exceed Margin.");
            FMessageObject->Send("RESULT.FUT", UFC::Hostname, *Data);
        }
    }  //if (messageType == "08")
}  //GCRMConnection::CheckFuturesOrderRisk()
//------------------------------------------------------------------------------
void GCRMConnection::CheckOptionsOrderRisk(nsOrderMessageDefine::MarketEnum OrderMarket, MTree *Data)
{
    UFC::AnsiString orderStr, userKey;
    Int32 NID;
    if (!GetOrderTreeNode(Data, orderStr, NID, userKey)) return;
    
    UFC::AnsiString functionCode = orderStr.SubString(2, 2);  //01:New 02:Replace Qty 03:Cancel 06:Replace Price
    if (functionCode == "06")
    {
        PassReplacePriceOrder(OrderMarket, Data);
        return;
    }
    
    UFC::AnsiString messageType = orderStr.SubString(4, 2);   //00:Order 01:Report 08:Quote 09:QuoteReport
    UFC::AnsiString brokerID = orderStr.SubString(14, 7);
    UFC::AnsiString orderID = orderStr.SubString(21, 5);
    UFC::AnsiString account = orderStr.SubString(26, 7);
    UFC::AnsiString symbol = orderStr.SubString(34, 20);
    symbol.TrimRight(' ');
    UFC::AnsiString contractCode = symbol.SubString(0, 3);
    CCommodityObject *commodityPtr = FindCommodity(contractCode);
    char currencyType = ' ';
    if (commodityPtr != 0) 
        currencyType = commodityPtr->GetCurrencyType();
    else
    {
        RejectCommodityNotExist(OrderMarket, contractCode, Data);
        return;
    }
    
    UFC::AnsiString orderAE, backendAE;
    GetAE(userKey, orderAE);
    if ( FDoTrimAEZero )
        backendAE = TrimBeginZero( orderAE );
    else
        backendAE = orderAE;
    backendAE.PadThis(6, ' ');
    UseMargin(account, backendAE);
    CAccountExecutive *aePtr = 0;
    AccountInfo *accInfoPtr = GetAccountInfo(account, backendAE, currencyType);
    if (accInfoPtr == 0) ///< Account information not exists.
    {
        RejectAccountInfoNotExist(OrderMarket, account, backendAE, currencyType, Data);
        return;
    }
    else
    {
        aePtr = accInfoPtr->GetAEObj();
        if (aePtr == 0) aePtr = FindSubAccount(backendAE, currencyType);
    }
    
    UFC::AnsiString leg1Symbol = "";
    UFC::AnsiString leg2Symbol = "";
    bool isMultiLge = ParseMultiLegSymbol(OrderMarket, symbol, leg1Symbol, leg2Symbol);
    if (isMultiLge)
        UFC::BufferedLog::Printf(" %s() [%s] is MultiLeg-->Leg1[%s] Leg2[%s]", __func__, symbol.c_str(), leg1Symbol.c_str(), leg2Symbol.c_str());
    else
        UFC::BufferedLog::Printf(" %s() [%s] is Single Leg.", __func__, symbol.c_str());
    UFC::BufferedLog::FlushToFile();
    
    if ((aePtr != 0) && aePtr->IsOnlyNearMonth())
    {
        bool isCorrectOrder = false;
        if (isMultiLge)
        {
            CContractObject *leg1ContractPtr = FindNearMonthContract(leg1Symbol);
            CContractObject *leg2ContractPtr = FindNearMonthContract(leg2Symbol);
            if ((leg1ContractPtr != 0) && (leg2ContractPtr != 0)) isCorrectOrder = true;
        }
        else
        {    
            if (FindNearMonthContract(symbol) != 0) isCorrectOrder = true;
        }
                    
        if (!isCorrectOrder)
        {
            RejectNearMonthOrder(OrderMarket, Data);
            return;
        }
    }  //if ((aePtr != 0) && aePtr->IsOnlyNearMonth())
    int precision = 3;
    if (messageType == "08")
    {
        CQuoteRequest* quoteReqPtr = new CQuoteRequest(OrderMarket, NID, orderStr, userKey);

        precision = FConnection->GetPricePrecision(OrderMarket, nsOrderMessageDefine::tsNormal, symbol);
        
        UFC::AnsiString bidPriceStr = orderStr.SubString(54, 9);
        double bidPrice = UFC::IntToDouble(bidPriceStr.ToInt(), precision);
        quoteReqPtr->SetBidQuotePrice(bidPrice);
        
        UFC::AnsiString askPriceStr = orderStr.SubString(63, 9);
        double askPrice = UFC::IntToDouble(askPriceStr.ToInt(), precision);
        quoteReqPtr->SetAskQuotePrice(askPrice);
        
        Int32 bidQty = orderStr.SubString(72, 4).ToInt();
        Int32 askQty = orderStr.SubString(76, 4).ToInt();
        char openOffset = orderStr[81];
        
        double bidCommodMargins = GetCommodityDeposit(symbol, bidPrice, true, false);
        double askCommodMargins = GetCommodityDeposit(symbol, askPrice, false, false);
        if ((bidCommodMargins < 0.0) || (askCommodMargins < 0.0))
        {
            RejectDepositNotExist(OrderMarket, symbol, Data);
            return;
        }
        
        UFC::BufferedLog::Printf(" %s() Option Quote Order[%s] Symbol[%s] Bid(Qty[%d] Px[%0.4lf] CommodMargins[%0.4lf]) Ask(Qty[%d] Px[%0.4lf] CommodMargins[%0.4lf]) BrokerID[%s] Account[%s] AE[%s][%c] OpenOffset[%c]", 
                                 __func__, orderID.c_str(), symbol.c_str(), bidQty, bidPrice, bidCommodMargins, askQty, askPrice, askCommodMargins, brokerID.c_str(), account.c_str(), backendAE.c_str(), currencyType, openOffset);
        UFC::BufferedLog::FlushToFile();
        
        if (accInfoPtr->CanPlaceOptionsQuoteOrder(bidQty, bidCommodMargins, askQty, askCommodMargins, orderID, symbol, bidPrice, askPrice, account))
        {
            Data->append("CODE", 1);
            FMessageObject->Send("RESULT.OPT", UFC::Hostname, *Data);
            CQuoteOrderRecord *newQuoteOrderPtr = new CQuoteOrderRecord(OrderMarket, orderID, symbol, bidPrice, bidQty, askPrice, askQty);
            if (!InsertQuoteOrder(orderID, newQuoteOrderPtr))
            {
                delete newQuoteOrderPtr;
                UFC::BufferedLog::Printf(" %s() Insert Option Quote Order[%s] failed.", __func__, orderID.c_str());
            }            
            
            if (!InsertRequestToNIDIndex(NID, quoteReqPtr, UFC::cstCheck, UFC::rwLockForWrite))
            {
                UFC::BufferedLog::Printf(" %s() Insert Option Quote Order[%s] to NID[%d] Index Failed.", __func__, orderID.c_str(), NID);
                delete quoteReqPtr;
            }
        }
        else
        {
            delete quoteReqPtr;
            Data->append("CODE", 0);
            Data->append("MSG", "Exceed Margin.");
            FMessageObject->Send("RESULT.OPT", UFC::Hostname, *Data);
        }
    }
    else
    {
        COrderRequest* orderReqPtr = new COrderRequest(OrderMarket, NID, orderStr, userKey);
        
        char orderType = orderStr[55];
        if (RejectMarketOrder(OrderMarket, orderType, Data)) return;
        
        UFC::AnsiString symbolStr;
        if (isMultiLge) 
            symbolStr.Printf("[%s(%s, %s)]", symbol.c_str(), leg1Symbol.c_str(), leg2Symbol.c_str());
        else
            symbolStr = symbol;
        UFC::AnsiString orderPriceStr = orderStr.SubString(56, 9);
        if (isMultiLge)
            precision = FConnection->GetPricePrecision(OrderMarket, nsOrderMessageDefine::tsNormal, leg1Symbol);
        else
            precision = FConnection->GetPricePrecision(OrderMarket, nsOrderMessageDefine::tsNormal, symbol);
        double orderPrice = UFC::IntToDouble(orderPriceStr.ToInt(), precision);
        orderReqPtr->SetOrderPrice(orderPrice);
        
        Int32 orderQty = orderStr.SubString(65, 4).ToInt();
        char openOffset = orderStr[70];
        char side = orderStr[54];
        
        double commodMargins = GetCommodityDeposit(symbol, orderPrice, (side == 'B'), isMultiLge);
        if (commodMargins < 0.0)
        {
            RejectDepositNotExist(OrderMarket, symbol, Data);
            return;
        }
        
        UFC::BufferedLog::Printf(" %s() Option Order[%s] Symbol[%s] Side[%c] Qty[%d] Px[%0.4lf] commodMargins[%0.4lf] BrokerID[%s] Account[%s] AE[%s][%c] OpenOffset[%c]",
                                 __func__, orderID.c_str(), symbolStr.c_str(), side, orderQty, orderPrice, commodMargins, brokerID.c_str(), account.c_str(), backendAE.c_str(), currencyType, openOffset);        
        UFC::BufferedLog::FlushToFile();
        if (accInfoPtr->CanPlaceOptionsOrder(orderQty, commodMargins, isMultiLge, orderID, symbol, side, orderPrice, account))
        {
            Data->append("CODE", 1);
            FMessageObject->Send("RESULT.OPT", UFC::Hostname, *Data);
            
            if (!InsertRequestToNIDIndex(NID, orderReqPtr, UFC::cstCheck, UFC::rwLockForWrite))
            {
                delete orderReqPtr;
                UFC::BufferedLog::Printf(" %s() Insert Option Order[%s] to NID[%d] Index Failed.", __func__, orderID.c_str(), NID);
            }
        }
        else
        {
            delete orderReqPtr;
            Data->append("CODE", 0);
            Data->append("MSG", "Exceed Margin.");
            FMessageObject->Send("RESULT.OPT", UFC::Hostname, *Data);
        }
    }
}  //GCRMConnection::CheckOptionsOrderRisk()
//------------------------------------------------------------------------------
void GCRMConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    if( Subject == FFutOrderSubject )
        CheckFuturesOrderRisk(nsOrderMessageDefine::mTWFutures, Data);
    else if ( Subject == FOptOrderSubject )
        CheckOptionsOrderRisk(nsOrderMessageDefine::mTWOptions, Data);
    else if( Subject == FToSpeedySubject ) ///< Recv Margin from Backend.
    {   //012345678901234567890123456789
        //0000000      +0064887861000TWA
        //00000001E    +0001065864200TWA
        //00000001M1   +0000000000000USA
        // Account     [ 7  ] 
        // Sub-Account [ 6  ]
        // Sign        [ 1  ]    
        // Integer     [ 11 ]
        // Fraction    [ 2  ]
        UFC::AnsiString RiskInfo;
        if( Data->get( "Data", RiskInfo ) == TRUE )
        {
            int riskInfoLength = RiskInfo.Length();
            if( riskInfoLength >= 27 )
            {
                UFC::AnsiString Account( RiskInfo.c_str() , 7 );
                UFC::AnsiString AE( RiskInfo.c_str() + 7, 6 );
                char            Sign = RiskInfo[13];
                int             IntVal = UFC::AnsiString( RiskInfo.c_str() + 14, 11 ).ToInt();
                int             FractionVal = UFC::AnsiString( RiskInfo.c_str() + 25, 2 ).ToInt();
                double          CanUse = static_cast<double>(IntVal) + static_cast<double>(FractionVal) / 100.0;
                char            currencyType = RiskInfo[27];
                UFC::AnsiString currencyCode = "";
                if( riskInfoLength >= 30 ) currencyCode = UFC::AnsiString( RiskInfo.c_str() + 27, 3 );
//                UFC::AnsiString currencyCode( RiskInfo.c_str() + 27, 1 );
                UFC::AnsiString signStr = "+";
                if( Sign == '-' ) 
                {
                    CanUse = -CanUse;
                    signStr = "-";
                }
                else if( Sign == ' ' )
                    signStr = " ";
                
                CCurrencyObject* currencyObjPtr = FindCurrencyFromCodeIndex(currencyCode, UFC::rwLockForRead);
                if (currencyObjPtr == 0) currencyObjPtr = FindCurrencyFromTypeIndex(currencyType, UFC::rwLockForRead);
                if (currencyObjPtr != 0)
                {
                    char currencyType = currencyObjPtr->GetCurrencyType();
                    AccountInfo *accInfoPtr = GetAccountInfo( Account, AE, currencyType );   //AccountInfoTable.GetObjectByKey( accountKey );
                    if( accInfoPtr == 0 )
                        UFC::BufferedLog::Printf( " Receive New Risk Information From AS400: Account[%s] AE[%s] Sign[%c] CanUse[%d.%02d] Currency[%c]", Account.c_str(), AE.c_str(), Sign, IntVal, FractionVal, currencyType );
                    SetAccountInfo( Account, AE, currencyType, currencyCode, CanUse, accInfoPtr );
                }
                else
                {
                    if (currencyCode.Length() > 0)
                        UFC::BufferedLog::DebugPrintf( UFC::dlOther, " Ignore Info[%s], not Support currencyCode[%s]", RiskInfo.c_str(), currencyCode.c_str() );
                    else
                        UFC::BufferedLog::DebugPrintf( UFC::dlOther, " Ignore Info[%s], not Support currencyType[%c]", RiskInfo.c_str(), currencyType );
                }
//                    UFC::BufferedLog::DebugPrintf( UFC::dlOther, " Ignore Info[%s], currencyCode[%s]", RiskInfo.c_str(), currencyCode.c_str() );
            }
            else
                UFC::BufferedLog::Printf( " Ignore Info[%s], Wrong Length %d", RiskInfo.c_str(), RiskInfo.Length() );
        }
        else
            UFC::BufferedLog::Printf( " Missing Node \"Data\"" );
    }  //if( Subject == FFutOrderSubject )
}  //GCRMConnection::OnMigoMessage()
//-------------------------------------------------------------------------------
AccountInfo* GCRMConnection::GetAccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE, char CurrencyType )
{
    UFC::AnsiString logHead = "GCRMConnection::GetAccountInfo()";
    UFC::AnsiString tempAccount;
    if ((Account == "8888881") || (Account == "8888810"))
    {
        tempAccount = "0000000";
        UFC::BufferedLog::Printf( " %s Get Account[%s]%c for [%s]", logHead.c_str(), tempAccount.c_str(), CurrencyType, Account.c_str() );
    }
    else
        tempAccount = Account;   
    UFC::AnsiString accountKey = AccountInfo::BuildAccountKey( tempAccount, AE, CurrencyType );
    return AccountInfoTable.GetObjectByKey( accountKey );
}
//-------------------------------------------------------------------------------
void GCRMConnection::SetAccountInfo( const UFC::AnsiString& Account, const UFC::AnsiString& AE, char CurrencyType, const UFC::AnsiString& CurrencyCode, double NewCanUse, AccountInfo *AccInfoPtr )
{
    UFC::AnsiString logHead = "GCRMConnection::SetAccountInfo()";
    AccountInfo *targetAccInfoPtr = 0;
    if( AccInfoPtr == 0 )
    {
        UFC::AnsiString tempAccount;
        if (Account == "8888881")
        {
            tempAccount = "0000000";
            UFC::BufferedLog::Printf( " %s Set Account[%s] Data to [%s], Currency[%c]", logHead.c_str(), Account.c_str(), tempAccount.c_str(), CurrencyType );
        }
        else
            tempAccount = Account;   
        UFC::AnsiString accountKey = AccountInfo::BuildAccountKey( tempAccount, AE, CurrencyType );
        targetAccInfoPtr = new AccountInfo( tempAccount, AE, CurrencyType );
        targetAccInfoPtr->SetCurrencyCode(CurrencyCode);
        if ( AccountInfoTable.Add( accountKey, targetAccInfoPtr ) == FALSE)
        {
            UFC::BufferedLog::Printf( " %s Add Account[%s][%s] Currency[%c] Information failed.", logHead.c_str(), tempAccount.c_str(), AE.c_str(), CurrencyType);
            delete targetAccInfoPtr;
            return;
        }
    }
    else
        targetAccInfoPtr = AccInfoPtr;
    
    if ( targetAccInfoPtr->GetAEObj() == 0 )
    {
        CAccountExecutive *aePtr = FindSubAccount(AE, CurrencyType);
        if (aePtr != 0) targetAccInfoPtr->SetAEObj(aePtr);
    }
    
    double canUse = targetAccInfoPtr->GetCanUse();
    double matchAmount = targetAccInfoPtr->GetMatchAmount();
    double quoteMatchAmount = targetAccInfoPtr->GetQuoteMatchAmount();
    double orderAmount = targetAccInfoPtr->GetOrderAmount();
    double quoteOrderAmount = targetAccInfoPtr->GetQuoteOrderAmount();
    int quoteMatchQty = targetAccInfoPtr->GetQuoteMatchQty();
    
    targetAccInfoPtr->SetCanUse( NewCanUse );
    double newOrderAmount = 0.0;
    if (orderAmount >= matchAmount)
        newOrderAmount = orderAmount - matchAmount;
    double newQuoteOrderAmount = 0.0;
//    newQuoteOrderAmount = quoteOrderAmount - quoteMatchAmount;
    targetAccInfoPtr->SetOrderAmount( newOrderAmount );
    targetAccInfoPtr->SetQuoteOrderAmount( newQuoteOrderAmount );
    targetAccInfoPtr->SetMatchAmount( 0.0 );
    targetAccInfoPtr->SetQuoteMatchAmount( 0.0 );
    targetAccInfoPtr->SetQuoteMatchQty( 0 );
    if( ( canUse != NewCanUse ) || ( orderAmount != newOrderAmount ) || ( quoteOrderAmount != newQuoteOrderAmount ) || ( matchAmount != 0.0 ) || ( quoteMatchAmount != 0.0 ) || ( quoteMatchQty != 0 ) )
        UFC::BufferedLog::Printf( " Set Account[%s][%s][%c] Information: canUse[%0.4lf=>%0.4lf] order[%0.4lf=>%0.4lf] quoteOrder[%0.4lf=>%0.4lf] match[%0.4lf=>0.00] quoteMatch[%0.4lf=>0.00] quoteMatchQty[%d=>0]", 
                                  Account.c_str(), AE.c_str(), CurrencyType, canUse, NewCanUse, orderAmount, newOrderAmount, quoteOrderAmount, newQuoteOrderAmount, matchAmount, quoteMatchAmount, quoteMatchQty );
}  //GCRMConnection::SetAccountInfo()
//-------------------------------------------------------------------------------    
void GCRMConnection::OnConnected( void )
{
    UFC::BufferedLog::Printf( " logon( ID=[%s], Password=[%s] )", FID.c_str(), FPassword.c_str() );
    FConnection->Logon( FID.c_str(), FPassword.c_str(), FAccount.c_str() );    
}
//------------------------------------------------------------------------------
void GCRMConnection::OnDisconnected( void )
{
    UFC::BufferedLog::Printf( " *** Connection broken ***" );
    FIsLogon = FALSE;
}
//------------------------------------------------------------------------------
void GCRMConnection::OnLogonReply( const char* ReplyString, LogonResult Result, int CID )
{
    if( Result == lrOk)
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
void GCRMConnection::Recover( const UFC::AnsiString& BeginTime )
{
    if( FIsLogon == TRUE )
    {
        FConnection->Recover( BeginTime.c_str(), rdBoth, rmAll );    
        UFC::BufferedLog::Printf( " Recover from:%s", BeginTime.c_str() );
    }
}    
//------------------------------------------------------------------------------
void GCRMConnection::OnRecoverFinished( int Count )
{
    FIsLogon = TRUE;
    UFC::BufferedLog::DebugPrintf( " SinoPacExecConnection::OnRecoverFinished( Count = %d )", Count );    
    UFC::SleepMS( 1000 );    
}    
//------------------------------------------------------------------------------
void GCRMConnection::CheckConnection( void )
{
    if( FIsLogon == FALSE )
        FConnection->Connect( FGatewayIP.c_str() , FGatewayPort, 10 );    
}
//------------------------------------------------------------------------------
void GCRMConnection::OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup )
{
    switch( Msg->GetOrderStatus() )
    {
        case nsOrderMessageDefine::etRejected:
        case nsOrderMessageDefine::osPartiallyFilled:
        case nsOrderMessageDefine::osFilled:
        case nsOrderMessageDefine::osCanceled:            
        case nsOrderMessageDefine::osReplaced:
             if( ( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced ) || ( Msg->GetTMPExecType() == nsOrderMessageDefine::tetPxReplaced2 ) )
             {
                UFC::BufferedLog::DebugPrintf( " Replace price. Drop this execution report." );
                return;
             }
             OnCancelReplace( Msg );
             break;
        default: break;
    }      
}
//------------------------------------------------------------------------------
void GCRMConnection::UpdateExecID( const UFC::AnsiString& ExecID )
{
    FExecIDTable.Add( ExecID );
    UFC::MemoryStream Buffer( EXECID_LEN, EXECID_LEN );

    memset( (void*)Buffer.GetBuffer(), '\0', EXECID_LEN );
    ExecID.SaveToStream( &Buffer );
    FExecIDFile->Write( Buffer.GetBuffer(), EXECID_LEN );
    FExecIDFile->Flush();
}
//------------------------------------------------------------------------------
void GCRMConnection::OnCancelReplace( TExecutionReportMessage* Msg )
{    
    UFC::AnsiString logHead = "GCRMConnection::OnCancelReplace()";
    Int32 NID = Msg->GetNID();
    UFC::AnsiString ExecID( Msg->GetExecID() );
    UFC::AnsiString orderID( Msg->GetOrderID() );
    nsOrderMessageDefine::ExecTypeEnum ExecType = Msg->GetExecType();
    if( FExecIDTable.Exists( ExecID ) == FALSE )
        UpdateExecID( ExecID );
    else
    {
        UFC::BufferedLog::Printf( " %s Duplicate ExecID[%s], OrderID[%s] ", logHead.c_str(), ExecID.c_str(), orderID.c_str() );
        if( ExecType != nsOrderMessageDefine::etCanceled ) return;
    }  //if( FExecIDTable.Exists( ExecID ) == FALSE )

    CQuoteOrderRecord *quoteOrderPtr = FindQuoteOrder(orderID);
    nsOrderMessageDefine::MarketEnum msgMarket = Msg->GetMarket();
    nsOrderMessageDefine::SideEnum msgSide = Msg->GetSide();
    UFC::AnsiString symbol( Msg->GetSymbol() );
    UFC::AnsiString leg1Symbol = "";
    UFC::AnsiString leg2Symbol = "";
    bool isMultiLeg = ParseMultiLegSymbol(msgMarket, symbol, leg1Symbol, leg2Symbol);

    UFC::AnsiString contractCode = symbol.SubString(0, 3);
    CCommodityObject* commodityPtr = FindCommodity(contractCode);
    char currencyType = ' ';
    if (commodityPtr != 0) 
        currencyType = commodityPtr->GetCurrencyType();
    else
        UFC::BufferedLog::Printf( " %s Commodity[%s] of Symbol[%s] not exist.", logHead.c_str(), contractCode.c_str(), symbol.c_str() );
    
    double orderPrice = Msg->GetPrice();
    if( ( ExecType == nsOrderMessageDefine::etRejected ) && 
        ( Msg->GetCxlRejResponseTo() == nsOrderMessageDefine::crrNew ) )
    {
        CBasicRequest* reqPtr = FindRequestFromNIDIndex(NID, UFC::rwLockForRead);
        if (reqPtr != 0)
        {
            UFC::AnsiString msgType = reqPtr->GetMessageType();
            if (msgType == "00")
            {
                COrderRequest* ordReqPtr = static_cast<COrderRequest*>(reqPtr);
                orderPrice = ordReqPtr->GetOrderPrice();
                UFC::BufferedLog::Printf(" %s Get [%012d]OrderReq[%s] OrderPrice[%14.4lf].", logHead.c_str(), NID, ordReqPtr->GetOrderNo().c_str(), orderPrice);
            }
            else if (msgType == "08")
            {
                CQuoteRequest* quoteReqPtr = static_cast<CQuoteRequest*>(reqPtr);
                if (msgSide == nsOrderMessageDefine::sBuy)
                {
                    orderPrice = quoteReqPtr->GetBidQuotePrice();
                    UFC::BufferedLog::Printf(" %s Get [%012d]OuoteReq[%s] BidPrice[%14.4lf].", logHead.c_str(), NID, quoteReqPtr->GetOrderNo().c_str(), orderPrice);
                }
                else
                {
                    orderPrice = quoteReqPtr->GetAskQuotePrice();
                    UFC::BufferedLog::Printf(" %s Get [%012d]OuoteReq[%s] AskPrice[%14.4lf].", logHead.c_str(), NID, quoteReqPtr->GetOrderNo().c_str(), orderPrice);
                }
            }
        }
        else
            UFC::BufferedLog::Printf(" %s Request NID[%012d] not found.", logHead.c_str(), NID);
    }
    
    double CommodMargins = GetCommodityDeposit(symbol, orderPrice, (msgSide == nsOrderMessageDefine::sBuy), isMultiLeg, leg1Symbol, leg2Symbol);
    if (CommodMargins < 0.0)
    {
        UFC::BufferedLog::Printf(" %s Commodity[%s] Deposit not exists.", logHead.c_str(), symbol.c_str());
        return;
    }
    
    UFC::AnsiString msgAE( Msg->GetAE() );
    UFC::AnsiString backendAE;
    if (FDoTrimAEZero)
        backendAE = TrimBeginZero( msgAE );
    else
        backendAE = msgAE;
    backendAE.PadThis(6, ' ');
    UFC::AnsiString Account( Msg->GetAccount() );
    UseMargin( Account, backendAE );
    AccountInfo *accInfoPtr = GetAccountInfo( Account, backendAE, currencyType );
    if( accInfoPtr == 0 )
    {
        UFC::BufferedLog::Printf( " %s Account[%s][%s(%s)][%c] information not Found ", logHead.c_str(), Account.c_str(), backendAE.c_str(), msgAE.c_str(), currencyType );
        return;
    }
    accInfoPtr->DumpData();
    
    if( ( ( ExecType == nsOrderMessageDefine::etPartiallyFilled ) || ( ExecType == nsOrderMessageDefine::etFilled ) ) && 
        ( FReleaseMarginOnFilled == TRUE ) )
    {
        Int32 beforeQty = Msg->GetBeforeQty();
        Int32 afterQty = Msg->GetAfterQty();
        Int32 Qty = beforeQty - afterQty;
        if (quoteOrderPtr == 0)
        {
            if (isMultiLeg) Qty *= 2;
            double matchAmount = CommodMargins * Qty;
            accInfoPtr->FilledOrder(matchAmount);
            UFC::BufferedLog::Printf( " --- Order[%s] Filled ---", orderID.c_str() );
            UFC::BufferedLog::Printf( " Symbol:[%s] Before[%d] After[%d] Filled Qty[%d], FilledAmount[%0.4lf]", symbol.c_str(), beforeQty, afterQty, Qty, matchAmount );
        }
        else
        {
            accInfoPtr->FilledQuoteOrder(Qty);
            UFC::BufferedLog::Printf( " --- Quote Order[%s] Filled ---", orderID.c_str() );
            UFC::BufferedLog::Printf( " Symbol:[%s] Before[%d] After[%d] Filled Qty[%d]", symbol.c_str(), beforeQty, afterQty, Qty);
            if( msgSide == nsOrderMessageDefine::sBuy )
            {
                int leavesQty = quoteOrderPtr->GetBidLeavesQty();
                int cumQty = quoteOrderPtr->GetBidCumQty();
                quoteOrderPtr->SetBidLeavesQty(leavesQty - Qty);
                quoteOrderPtr->SetBidCumQty(cumQty + Qty);
            }
            else
            {
                int leavesQty = quoteOrderPtr->GetAskLeavesQty();
                int cumQty = quoteOrderPtr->GetAskCumQty();
                quoteOrderPtr->SetAskLeavesQty(leavesQty - Qty);
                quoteOrderPtr->SetAskCumQty(cumQty + Qty);
            }
        }
        accInfoPtr->DumpData();
    }
    else if( ExecType == nsOrderMessageDefine::etCanceled )
    {
        if( Msg->GetTimeInForce() == nsOrderMessageDefine::tifROD ) ///<  ROD
        {
            if( FReleaseMarginOnRODCxl == FALSE )
                return;
        }
        else ///FOK, IOC
        {
            if( FReleaseMarginOnFOKIOCCxl == FALSE  )
                return;
        }
        
        Int32 beforeQty = Msg->GetBeforeQty();
        Int32 afterQty = Msg->GetAfterQty();
        if (quoteOrderPtr == 0)
        {
            Int32 Qty = beforeQty - afterQty;
            if (isMultiLeg) Qty *= 2;
            double cancelAmount = CommodMargins * Qty;
            accInfoPtr->CancelOrder( cancelAmount );
            UFC::BufferedLog::Printf( " --- Order[%s] Canceled ---", orderID.c_str() );
            UFC::BufferedLog::Printf( " Before[%d] After[%d] Cancel Qty[%d], CancelAmount[%lf]", beforeQty, afterQty, Qty, cancelAmount );
        }
        else
        {
            if (beforeQty > FMaxQuoteOrderQty)
            {
                Int32 Qty = beforeQty - FMaxQuoteOrderQty;
                double cancelAmount = CommodMargins * Qty;
                accInfoPtr->CancelQuoteOrder( cancelAmount );
                UFC::BufferedLog::Printf( " --- Quote Order[%s] Canceled ---", orderID.c_str() );
                UFC::BufferedLog::Printf( " Before[%d] After[%d] Cancel Qty[%d], CancelAmount[%lf]", beforeQty, afterQty, Qty, cancelAmount );
            }
        }
        accInfoPtr->DumpData();
    }
    else if( ( ExecType == nsOrderMessageDefine::etReplaced ) && ( Msg->GetTMPExecType() == nsOrderMessageDefine::tetReplaced ) )
    {
        Int32 Qty = Msg->GetBeforeQty() - Msg->GetAfterQty();
        if (isMultiLeg) Qty *= 2;
        double cancelAmount = CommodMargins * Qty;
        accInfoPtr->CancelOrder( cancelAmount );
        UFC::BufferedLog::Printf( " --- Order[%s] Replaced ---", Msg->GetOrderID() );
        UFC::BufferedLog::Printf( " Before[%d] After[%d] Reduce Qty[%d], CancelAmount[%lf]", Msg->GetBeforeQty(), Msg->GetAfterQty(), Qty, cancelAmount );
        accInfoPtr->DumpData();
    }
    else if( ( ExecType == nsOrderMessageDefine::etRejected ) && 
             ( Msg->GetCxlRejResponseTo() == nsOrderMessageDefine::crrNew ) )
    {
        Int32 ErrCode = UFC::AnsiString( Msg->GetStatusCode() ).ToInt();
        if( ErrCode != 97 ) ///< Not reject by RiskManager, need release margin.
        {
            if (quoteOrderPtr == 0)
            {
                Int32 Qty = Msg->GetOrderQty();
                double cancelAmount = CommodMargins * Qty;
                accInfoPtr->CancelOrder( cancelAmount );
                UFC::BufferedLog::Printf( " --- Order[%s] Rejected Qty[%d], Amount[%lf] ---", Msg->GetOrderID(), Qty, cancelAmount );
            }
            else
            {
                Int32 Qty = Msg->GetOrderQty();
                if (Qty > FMaxQuoteOrderQty)
                {
                    Qty = Qty - FMaxQuoteOrderQty;
                    double cancelAmount = CommodMargins * Qty;
                    accInfoPtr->CancelQuoteOrder( cancelAmount );
                    UFC::BufferedLog::Printf( " --- Quote Order[%s] Rejected Qty[%d], Amount[%lf] ---", Msg->GetOrderID(), Qty, cancelAmount );
                }
            }
            accInfoPtr->DumpData();
        }
    }
}
//------------------------------------------------------------------------------

