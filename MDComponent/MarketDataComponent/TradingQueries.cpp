//---------------------------------------------------------------------------


#pragma hdrstop

#include "TradingQueries.h"
#include "TradingObjectPool.h"
#include "iniFile.h"
#pragma package(smart_init)
namespace s888
{
//------------------------ CBasicTradingQueryObject -------------------------
//---------------------------------------------------------------------------
CBasicTradingQueryObject::CBasicTradingQueryObject(const String& QueryURL, int ConnectTimeOut, const String& DumpFileName, bool DoDump, bool CreateSuspended)
:TThread(CreateSuspended),
 FQueryName(L"CBasicTradingQueryObject"),
 FQueryURL(QueryURL),
 FConnectTimeOut(ConnectTimeOut),
 FDumpFileName(DumpFileName),
 FDoDump(DoDump),
 FIsTestMode(false),
 FOnError(0),
 FQueryStatus(qsNone),
 FQueryMessage(L""),
 FHttpPtr(0),
 FSSLIOHandlerPtr(0)
{
	if (FDumpFileName.Length() <= 0)
		FDumpFileName = L"Super888_QueryResponse.xml";
}  //CBasicTradingQueryObject::CBasicTradingQueryObject()
//---------------------------------------------------------------------------
String CBasicTradingQueryObject::CombineQueryURL(String& QueryURL)
{
	return QueryURL;
}  //CBasicTradingQueryObject::CombineQueryURL()
//---------------------------------------------------------------------------
bool CBasicTradingQueryObject::PrepareHttpObject(const String& QueryURL, int ConnectTimeOut)
{
	SetQueryStatus(qsPrepareRequest);
	bool isSuccess = false;
	try
	{
		FHttpPtr = new TIdHTTP(0);
		if (QueryURL.Pos(L"https") != 0)  //< Use https
		{
			FSSLIOHandlerPtr = new TIdSSLIOHandlerSocketOpenSSL(0);
			FSSLIOHandlerPtr->SSLOptions->Method = sslvSSLv23;
			FHttpPtr->IOHandler = FSSLIOHandlerPtr;
		}  //if (QueryURL.Pos(L"https") != 0)
		FHttpPtr->ConnectTimeout = ConnectTimeOut;
        FHttpPtr->ReadTimeout = ConnectTimeOut;
		isSuccess = true;
	} catch (Exception& e) {
		String logHead = FQueryName + L"::PrepareHttpObject()";
		String logStr;
		logStr.printf(L"Exception:%s. URL=[%s]", e.Message, QueryURL);
		WriteUFCLog(logHead, logStr, constFlush);
		FHttpPtr = 0;
		SetQueryStatus(qsFailed);
		SetQueryMessage(logStr);
		if (FOnError != NULL) FOnError(logStr);
	}
	return isSuccess;
}  //CBasicTradingQueryObject::PrepareHttpObject()
//---------------------------------------------------------------------------
bool CBasicTradingQueryObject::SendQueryURL(TIdHTTP *HttpPtr, const String& QueryURL, TStringStream *QueryResponse)
{
	String logHead = FQueryName + L"::SendQueryURL()";
	String logStr;
	logStr.printf(L"[%s]", QueryURL);
	WriteUFCLog(logHead, logStr, constFlush);
	logStr = L"";
	bool isSuccess = false;
	try
	{
		HttpPtr->Get(QueryURL, QueryResponse);
		isSuccess = true;
		SetQueryStatus(qsRequestSended);
	} catch (EIdException& e) {
		logStr.printf(L"EIdException:%s. URL=[%s]", e.Message, QueryURL);
	} catch (Exception& e) {
		logStr.printf(L"Exception:%s. URL=[%s]", e.Message, QueryURL);
	} catch (...) {
		logStr.printf(L"Unknown Exception. URL=[%s]", QueryURL);
	}  //try

	if (!isSuccess)
	{
		SetQueryStatus(qsFailed);
		if (logStr.Length() > 0)
		{
			SetQueryMessage(logStr);
			WriteUFCLog(logHead, logStr, constFlush);
			if (FOnError != NULL) FOnError(logStr);
		}  //if (logMessage.Length() > 0)
	}  //if (!isSuccess)
	return isSuccess;
}  //CBasicTradingQueryObject::SendQueryURL()
//---------------------------------------------------------------------------
bool CBasicTradingQueryObject::ParseResponseXML(_di_IXMLDocument ResponseXML)
{
	return true;
}  //CBasicTradingQueryObject::ProcessQueryResponse()
//---------------------------------------------------------------------------
_di_IXMLDocument CBasicTradingQueryObject::CreateXMLDocument(TStringStream *DataSourcePtr)
{
	SetQueryStatus(qsResponseArrived);
	String logHead = FQueryName + L"::CreateXMLDocument()";
	String logStr = L"";
	String logMessage = L"";
	_di_IXMLDocument newXMLDoc = 0;
	try
	{
		newXMLDoc = NewXMLDocument();
		newXMLDoc->LoadFromStream(DataSourcePtr);
		if (newXMLDoc->Active && FDoDump && (FDumpFileName.Length() > 0))
			newXMLDoc->SaveToFile(FDumpFileName);
	} catch (EXMLDocError& e) {
		logStr.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message,FQueryURL);
	} catch (Exception& e) {
		logStr.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logStr.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}

	if (logStr.Length() > 0)
	{
		newXMLDoc = 0;
		SetQueryStatus(qsFinished);
		SetQueryMessage(logStr);
		WriteUFCLog(logHead, logStr, constFlush);
		if (FOnError != NULL) FOnError(logStr);
	}  //if (logMessage.Length() > 0)
	return newXMLDoc;
}  //CBasicTradingQueryObject::CreateXMLDocument()
//---------------------------------------------------------------------------
void __fastcall CBasicTradingQueryObject::Execute(void)
{
	SetQueryStatus(qsNone);
	SetQueryMessage(L"");
	SetParseMessage(L"");
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	FHttpPtr = 0;
	FSSLIOHandlerPtr = 0;
	String completeQueryURL = CombineQueryURL(FQueryURL);
	if (!PrepareHttpObject(completeQueryURL, FConnectTimeOut)) return;
	TStringStream *queryResponse = new TStringStream();
	if (SendQueryURL(FHttpPtr, completeQueryURL, queryResponse))
	{
		_di_IXMLDocument responseXML = CreateXMLDocument(queryResponse);
		if (responseXML != 0)
		{
			SetQueryStatus(qsParsingResponse);
			String ParseMessage = L"";
			ParseResponseXML(responseXML);
		}  //if (responseXML != 0)
	}  //if (SendQueryURL(httpPtr, completeQueryURL, queryResponse))
	if (FQueryStatus != qsFailed) SetQueryStatus(qsFinished);
	delete FHttpPtr;
	delete FSSLIOHandlerPtr;
	delete queryResponse;
	CoUninitialize();
	String logMessage;
	logMessage.printf(L"%s::Execute() QureyEnded.", FQueryName);
	WriteUFCLog(logMessage, constFlush);
}  //CBasicTradingQueryObject::Execute()

//----------------------- CTaifexContractQueryObject ------------------------
String CTaifexContractQueryObject::FExchangeCode = L"TAIFEX";
//---------------------------------------------------------------------------
CTaifexContractQueryObject::CTaifexContractQueryObject(const String& QueryURL,
													   int ConnectTimeOut,
													   const String& DumpFileName,
													   bool DoDump,
													   bool CreateSuspended)
:CBasicTradingQueryObject(QueryURL, ConnectTimeOut, DumpFileName, DoDump, CreateSuspended),
 FExchangePtr(0),
 FCommodityCount(0),
 FContractCount(0)
{
	FQueryName = L"QueryTaifexContract";
	gQueryObjsPool.LockExchange(rwLockForWrite);
	FExchangePtr = gQueryObjsPool.FindExchangeFromCodeIndex(FExchangeCode, rwNotLock);
	if (FExchangePtr == 0)
	{
		FExchangePtr = new CTradingExchange(FExchangeCode);
		gQueryObjsPool.InsertExchangeIntoCodeIndex(FExchangeCode, FExchangePtr, constNotCheckExist, rwNotLock);
	}
	gQueryObjsPool.UnlockExchange(rwLockForWrite);
}  //CTaifexContractQueryObject::CTaifexContractQueryObject()
//---------------------------------------------------------------------------
void CTaifexContractQueryObject::ParseCommodityNode(_di_IXMLNode CommodityNode)
{
	String logHead = FQueryName + L"::ParseCommodityNode()";
	String logMessage = L"";
	bool isExceptionHappened = true;
	try
	{
		String brokerContractCode = L"";
		String contractCode = L"";
		String commodityName = L"";
		String contractKind = L"";
		String currency = L"";

		//Parse Commodity
		_di_IXMLNode curFieldNode = CommodityNode->ChildNodes->First();
		while (curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				String curFieldName = curFieldNode->GetNodeName();
				if (curFieldName == L"comno")  //Old Contract Code
					brokerContractCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"mktcomno")
					contractCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"comname")
					commodityName = curFieldNode->GetText().Trim();
				else if (curFieldName == L"comkind")
				{
					contractKind = curFieldNode->GetText();
					if ((contractKind != "I") && (contractKind != "R") && (contractKind != "B") &&
						(contractKind != "C") && (contractKind != "S"))
						contractKind = L"";
				}
				else if (curFieldName == L"curr")
					currency = curFieldNode->GetText();
			}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)

		if (contractCode.Length() <= 0)
		{
			logMessage.printf(L"%s::ParseCommodityNode(): No Contract Code.", FQueryName);
			WriteUFCLog(logHead, L"No Contract Code.", constFlush);
			return;
		}

		CTradingCommodity *curCommodityPtr = gQueryObjsPool.FindCommodityFromCodeIndex(FExchangeCode, contractCode, rwLockForRead);
		if (curCommodityPtr == 0)
		{
			if (brokerContractCode.Length() <= 0) brokerContractCode = contractCode;
			String tradeContractCode = contractCode;
			bool isDayTradeOpenSupported = false;
			bool isStopOrderSupported = false;
			double taxRate = 0.0;
			if (FExchangePtr != 0)
			{
				isDayTradeOpenSupported = FExchangePtr->IsDayTradeOpenSupported();
				isStopOrderSupported = FExchangePtr->IsStopOrderSupported();
			}
			curCommodityPtr = new CTradingCommodity(FExchangeCode, contractCode, FMarket,
												isDayTradeOpenSupported, isStopOrderSupported, taxRate, FExchangePtr);
			curCommodityPtr->SetBrokerContractCode(brokerContractCode, rwNotLock);
			curCommodityPtr->SetTradeContractCode(tradeContractCode, rwNotLock);
			if (commodityName.Length() > 0) curCommodityPtr->SetCommodityName(commodityName, rwNotLock);
			if (currency.Length() > 0) curCommodityPtr->SetCurrency(currency, rwNotLock);

			gQueryObjsPool.LockCommodity(rwLockForWrite);
			if (gQueryObjsPool.InsertCommodityIntoCodeIndex(FExchangeCode, contractCode, curCommodityPtr, constNotCheckExist, rwNotLock))
			{
				FCommodityCount++;
				if (!gQueryObjsPool.InsertCommodityIntoTradeCodeIndex(FExchangeCode, tradeContractCode, curCommodityPtr, constCheckExist, rwNotLock))
				{
					logMessage.printf(L"Insert Commodity[%s-%s] Trade Code[%s] Index Failed.", FExchangeCode, contractCode, tradeContractCode);
					WriteUFCLog(logHead, logMessage, constFlush);
				}

				if (!gQueryObjsPool.InsertCommodityIntoBrokerCodeIndex(FExchangeCode, brokerContractCode, curCommodityPtr, constCheckExist, rwNotLock))
				{
					logMessage.printf(L"Insert Commodity[%s-%s] Broker Code[%s] Index Failed.", FExchangeCode, contractCode, brokerContractCode);
					WriteUFCLog(logHead, logMessage, constFlush);
				}

				if (!gQueryObjsPool.InsertCommodityIntoMarketCodeIndex(FMarket, contractCode, curCommodityPtr, constCheckExist, rwNotLock))
				{
					logMessage.printf(L"Insert Commodity[%s-%s] Market[%s] Code Index Failed.", FExchangeCode, contractCode, DescribeMarket(FMarket));
					WriteUFCLog(logHead, logMessage, constFlush);
				}
			}
			else
			{
				logMessage.printf(L"Insert Commodity[%s-%s] Code Index Failed.", FExchangeCode, contractCode);
				WriteUFCLog(logHead, logMessage, constFlush);
				delete curCommodityPtr;
				curCommodityPtr = 0;
			}
			gQueryObjsPool.UnlockCommodity(rwLockForWrite);
		}  //if (curCommodityPtr == 0)

		//Parse Contract
		curFieldNode = CommodityNode->ChildNodes->First();
		while (curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes)
			{
				String curFieldName = curFieldNode->GetNodeName();
				if (curFieldName == L"fbmcom") ParseContractNode(curCommodityPtr, curFieldNode);
			}  //if (curFieldNode->HasChildNodes)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)
		isExceptionHappened = false;
	} catch (EXMLDocError& e) {
		logMessage.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMessage.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMessage.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened)
	{
		if (logMessage.Length() > 0)
		{
			SetParseMessage(logMessage);
			WriteUFCLog(logHead, logMessage, constFlush);
			if (FOnError != NULL) FOnError(logMessage);
		}
	}  //if (isExceptionHappened)
	UFC::BufferedLog::FlushToFile();
}  //CTaifexContractQueryObject::ParseCommodityNode()
//---------------------------------------------------------------------------
bool CTaifexContractQueryObject::ParseResponseXML(_di_IXMLDocument ResponseXML)
{
	FCommodityCount = 0;
	FContractCount = 0;
	bool isSuccess = false;
	String logHead = FQueryName + L"::ParseResponseXML()";
	String logMessage = L"";
	try
	{
		_di_IXMLNode rootNode = ResponseXML->DocumentElement;
		if (rootNode != 0)
		{
			_di_IXMLNode curChildNode = rootNode->ChildNodes->First();
			while (curChildNode != 0)
			{
				if (curChildNode->HasChildNodes)
				{
					String curChildNodeName = curChildNode->GetNodeName();
					if (curChildNodeName == L"fbmcob") ParseCommodityNode(curChildNode);
				}  //if (curChildNodeName == L"fbmcob")
				curChildNode = curChildNode->NextSibling();
			}  //while (curChildNode != 0)
		}  //if (rootNode != 0)
		isSuccess = true;
	} catch (EXMLDocError& e) {
		logMessage.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMessage.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMessage.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (!isSuccess)
	{
		if (logMessage.Length() > 0)
		{
			SetParseMessage(logMessage);
			WriteUFCLog(logHead, logMessage, constFlush);
			if (FOnError != NULL) FOnError(logMessage);
		}  //if (logMessage.Length() > 0)
	}  //if (!isSuccess)

	logMessage.printf(L"%d Commodities, %d Contracts.", FCommodityCount, FContractCount);
	WriteUFCLog(logHead, logMessage, constFlush);
	return isSuccess;
}  //CTaifexContractQueryObject::ParseResponseXML()

//-------------------- CTaifexFuturesContractQueryObject --------------------
//---------------------------------------------------------------------------
CTaifexFuturesContractQueryObject::CTaifexFuturesContractQueryObject(const String& QueryURL,
																	 int ConnectTimeOut,
																	 const String& DumpFileName,
																	 bool DoDump,
																	 bool CreateSuspended)
:CTaifexContractQueryObject(QueryURL, ConnectTimeOut, DumpFileName, DoDump, CreateSuspended)
{
	FQueryName = L"QueryTaifexFutures";
	FMarket = nsOrderMessageDefine::mTWFutures;
	if (DumpFileName.Length() <= 0)
		FDumpFileName = L"Super888_TaifexFuturesContract.xml";
}  //CQueryTaifexFuturesContractObject::CQueryTaifexFuturesContractObject()
//---------------------------------------------------------------------------
void CTaifexFuturesContractQueryObject::ParseContractNode(CTradingCommodity *CommodityPtr, _di_IXMLNode ContractNode)
{
	String maturityYearMonth = L"";
	String symbol = L"";
	String logHead = FQueryName + L"::ParseContractNode()";
	String logMessage = L"";
	bool isExceptionHappened = true;
	try
	{
		_di_IXMLNode curFieldNode = ContractNode->ChildNodes->First();
		while (curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				String curFieldName = curFieldNode->GetNodeName();
				if (curFieldName == L"comym")
					maturityYearMonth = curFieldNode->GetText().Trim();
				else if (curFieldName == L"comid")
					symbol = curFieldNode->GetText().Trim();
			}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)
		isExceptionHappened = false;
	} catch (EXMLDocError& e) {
		logMessage.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMessage.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMessage.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened)
	{
		if (logMessage.Length() > 0)
		{
			WriteUFCLog(logHead, logMessage, constFlush);
			SetParseMessage(logMessage);
			if (FOnError != NULL) FOnError(logMessage);
		}  //if (logMessage.Length() > 0)
		return;
	}

	int symbolLength = symbol.Length();
	if (symbolLength > 2)
	{
		String monthYearCode = symbol.SubString(symbolLength - 1, 2);
		String brokerContractCode = CommodityPtr->GetBrokerContractCode(rwLockForRead);
		String brokerSymbol = brokerContractCode + monthYearCode;
		String tradeSymbol = symbol;
		double strikePrice = 0.0;
		CTradingContract *newContractPtr = new CTradingContract(FExchangeCode, symbol, FMarket, strikePrice,
																nsOrderMessageDefine::evtNone, CommodityPtr, FExchangePtr);
		newContractPtr->SetTradeSymbol(tradeSymbol, rwNotLock);
		newContractPtr->SetBrokerSymbol(brokerSymbol, rwNotLock);
		if (maturityYearMonth.Length() > 0) newContractPtr->SetMaturityYearMonthStr(maturityYearMonth, rwNotLock);
		if (!gQueryObjsPool.AddContract(newContractPtr, s888::rwLockForWrite))
		{
			logMessage.printf(L"Add Contract[%s-%s] Failed.", FExchangeCode, symbol);
			WriteUFCLog(logHead, logMessage, constFlush);
			delete newContractPtr;
		}
	}
	else
	{
		logMessage.printf(L"Wrong Symbol[%s]. URL=[%s]", symbol, FQueryURL);
		WriteUFCLog(logHead, logMessage, constFlush);
	}  //if (symbolLength > 2)
}  //CTaifexFuturesContractQueryObject::ParseContractNode()
//---------------------------------------------------------------------------
void CTaifexFuturesContractQueryObject::SetQueryStatus(QueryStatusEnum QueryStatus)
{
	FQueryStatus = QueryStatus;
	gQueryObjsPool.SetTaifexFuturesContractQueryStatus(QueryStatus);
}  //CTaifexFuturesContractQueryObject::SetQueryStatus()
//---------------------------------------------------------------------------
void CTaifexFuturesContractQueryObject::SetQueryMessage(const String& QueryMessage)
{
	FQueryMessage = QueryMessage;
	gQueryObjsPool.SetTaifexFuturesContractQueryMessage(QueryMessage);
}  //CTaifexFuturesContractQueryObject::SetQueryMessage()
//---------------------------------------------------------------------------
void CTaifexFuturesContractQueryObject::SetParseMessage(const String& ParseMessage)
{
	FParseMessage = ParseMessage;
	gQueryObjsPool.SetTaifexFuturesContractQueryMessage(ParseMessage);
}  //CTaifexFuturesContractQueryObject::SetParseMessage()

//-------------------- CTaifexOptionsContractQueryObject --------------------
//---------------------------------------------------------------------------
CTaifexOptionsContractQueryObject::CTaifexOptionsContractQueryObject(const String& QueryURL,
																	 int ConnectTimeOut,
																	 const String& DumpFileName,
																	 bool DoDump,
																	 bool CreateSuspended)
:CTaifexContractQueryObject(QueryURL, ConnectTimeOut, DumpFileName, DoDump, CreateSuspended)
{
	FQueryName = L"QueryTaifexOptions";
	FMarket = nsOrderMessageDefine::mTWOptions;
	if (DumpFileName.Length() <= 0)
		FDumpFileName = L"Super888_TaifexOptionsContract.xml";
}  //CTaifexOptionsContractQueryObject::CTaifexOptionsContractQueryObject()
//---------------------------------------------------------------------------
void CTaifexOptionsContractQueryObject::ParseStrikePriceNode(CTradingCommodity *CommodityPtr,
															 const String& ContractCode,
															 const String& MaturityYearMonth,
															 nsOrderMessageDefine::EventTypeEnum CallOrPut,
															 _di_IXMLNode StrikePriceNode)
{
	String logHead = FQueryName + L"::ParseStrikePriceNode()";
	String logMessage = L"";
	bool isExceptionHappened = true;
	try
	{
		_di_IXMLNode curFieldNode = StrikePriceNode->ChildNodes->First();
		while (curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				String curFieldName = curFieldNode->GetNodeName().Trim();
				if (curFieldName == L"stkprc")
				{
					String curPriceStr = curFieldNode->GetText().Trim();
					TStringList *curPriceListPtr = new TStringList();
					curPriceListPtr->Delimiter = L',';
					curPriceListPtr->DelimitedText = curPriceStr;

					String curStrikePriceStr = L"";
					double curStrikePrice = 0.0;
					if (curPriceListPtr->Count > 0)
					{
						curStrikePriceStr = (*curPriceListPtr)[0];
						if (curStrikePriceStr.Length() > 0)
							curStrikePrice = curStrikePriceStr.ToDouble();
					}

					String curSymbol = L"";
					if (curPriceListPtr->Count >= 5) curSymbol = (*curPriceListPtr)[4];

					if ((curSymbol.Length() > 0) && (curStrikePrice > 0.0))
					{
						CTradingContract *newContractPtr = new CTradingContract(FExchangeCode, curSymbol, FMarket,
																				curStrikePrice, CallOrPut, CommodityPtr, FExchangePtr);
						if (MaturityYearMonth.Length() > 0) newContractPtr->SetMaturityYearMonthStr(MaturityYearMonth, rwNotLock);
						if (!gQueryObjsPool.AddContract(newContractPtr, s888::rwLockForWrite))
						{
							logMessage.printf(L"Add Contract[%s-%s] Failed.", FQueryName, FExchangeCode, curSymbol);
							WriteUFCLog(logHead, logMessage, constFlush);
							delete newContractPtr;
						}  //if (!gQueryObjsPool.AddContract(newContractPtr, s888::rwLockForWrite))
					}
					else
					{
						logMessage.printf(L"Wrong Symbol[%s] or StrikePrice[%s]. URL=[%s]", curSymbol, curStrikePriceStr, FQueryURL);
						WriteUFCLog(logHead, logMessage, constFlush);
					}  //if ((curSymbol.Length() > 0) && (curStrikePrice > 0.0))
				}  //if (curFieldName == L"stkprc")
			}  //if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)
		isExceptionHappened = false;
	} catch (EXMLDocError& e) {
		logMessage.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMessage.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMessage.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened)
	{
		if (logMessage.Length() > 0)
		{
			WriteUFCLog(logHead, logMessage, constFlush);
			SetParseMessage(logMessage);
			if (FOnError != NULL) FOnError(logMessage);
		}  //if (logMessage.Length() > 0)
	}  //if (isExceptionHappened)
}  //CTaifexOptionsContractQueryObject::ParseStrikePriceNode()
//---------------------------------------------------------------------------
void CTaifexOptionsContractQueryObject::ParseContractNode(CTradingCommodity *CommodityPtr, _di_IXMLNode ContractNode)
{
	String logHead = FQueryName + L"::ParseContractNode()";
	String logMessage = L"";
	bool isExceptionHappened = true;
	try
	{
		String curFieldName = L"";
		String maturityYearMonth = L"";
		String contractCode = L"";
		_di_IXMLNode curFieldNode = ContractNode->ChildNodes->First();
		while (curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				curFieldName = curFieldNode->GetNodeName().Trim();
				if (curFieldName == L"comno")
					contractCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"comym")
					maturityYearMonth = curFieldNode->GetText().Trim();
			}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)

		if ((contractCode.Length() > 0) && (maturityYearMonth.Length() >= 6))
		{
			curFieldNode = ContractNode->ChildNodes->First();
			while (curFieldNode != 0)
			{
				if (curFieldNode->HasChildNodes)
				{
					curFieldName = curFieldNode->GetNodeName();
					if (curFieldName == L"C")
						ParseStrikePriceNode(CommodityPtr, contractCode, maturityYearMonth,
											 nsOrderMessageDefine::evtCall, curFieldNode);
					else if (curFieldName == L"P")
						ParseStrikePriceNode(CommodityPtr, contractCode, maturityYearMonth,
											 nsOrderMessageDefine::evtPut, curFieldNode);
				}  //if (curFieldNode->HasChildNodes)
				curFieldNode = curFieldNode->NextSibling();
			}  //while (curFieldNode != 0)
		}
		else
		{
			logMessage.printf(L"Missing Contract Code[%s] or MaturityYearMonth[%s]", contractCode, maturityYearMonth);
			WriteUFCLog(logHead, logMessage, constFlush);
		}  //if ((contractCode.Length() > 0) && (maturityYearMonth.Length() >= 6))
		isExceptionHappened = false;
	} catch (EXMLDocError& e) {
		logMessage.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMessage.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMessage.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened)
	{
		if (logMessage.Length() > 0)
		{
			WriteUFCLog(logHead, logMessage, constFlush);
			SetParseMessage(logMessage);
			if (FOnError != NULL) FOnError(logMessage);
		}  //if (logMessage.Length() > 0)
	}  //if (isExceptionHappened)
}  //CTaifexOptionsContractQueryObject::ParseContractNode()
//---------------------------------------------------------------------------
void CTaifexOptionsContractQueryObject::SetQueryStatus(QueryStatusEnum QueryStatus)
{
	FQueryStatus = QueryStatus;
	gQueryObjsPool.SetTaifexOptionsContractQueryStatus(QueryStatus);
}  //CTaifexOptionsContractQueryObject::SetQueryStatus()
//---------------------------------------------------------------------------
void CTaifexOptionsContractQueryObject::SetQueryMessage(const String& QueryMessage)
{
	FQueryMessage = QueryMessage;
	gQueryObjsPool.SetTaifexOptionsContractQueryMessage(QueryMessage);
}  //CTaifexOptionsContractQueryObject::SetQueryMessage()
//---------------------------------------------------------------------------
void CTaifexOptionsContractQueryObject::SetParseMessage(const String& ParseMessage)
{
	FParseMessage = ParseMessage;
	gQueryObjsPool.SetTaifexOptionsContractQueryMessage(ParseMessage);
}  //CTaifexOptionsContractQueryObject::SetParseMessage()

//---------------------- COverseasContractQueryObject -----------------------
//---------------------------------------------------------------------------
COverseasContractQueryObject::COverseasContractQueryObject(const String& QueryURL,
														   int ConnectTimeOut,
														   const String& DumpFileName,
														   bool DoDump,
														   bool CreateSuspended)
:CBasicTradingQueryObject(QueryURL, ConnectTimeOut, DumpFileName, DoDump, CreateSuspended),
 FExchangeCount(0),
 FCommodityCount(0),
 FContractCount(0)
{
	FQueryName = L"QueryOverseasContract";
	if (DumpFileName.Length() <= 0)
		FDumpFileName = L"Super888_OverseasContract.xml";
}  //COverseasContractQueryObject::CTaifexContractQueryObject()
//---------------------------------------------------------------------------
void COverseasContractQueryObject::ParseContractNode(CTradingExchange *ExchangePtr, CTradingCommodity *CommodityPtr,
													 nsOrderMessageDefine::MarketEnum Market,
													 nsOrderMessageDefine::EventTypeEnum CallPut,
													 _di_IXMLNode ContractNode)
{
	String logHead = FQueryName + L"::ParseContractNode()";
	String logMessage = L"";
	String brokerExchangeCode = L"";
	String brokerContractCode = L"";
	String maturityYearMonth = L"";
	String tradeSymbol = L"";
	bool isExceptionHappened = true;
	try
	{
		_di_IXMLNode curFieldNode = ContractNode->ChildNodes->First();
		while (curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				String curFieldName = curFieldNode->GetNodeName();
				if (curFieldName == L"exh")
					brokerExchangeCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"comno")
					brokerContractCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"comym")
					maturityYearMonth = curFieldNode->GetText().Trim();
				else if (curFieldName == L"comid")
					tradeSymbol = curFieldNode->GetText().Trim();
			}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)
		isExceptionHappened = false;
	} catch (EXMLDocError& e) {
		logMessage.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMessage.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMessage.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened)
	{
		if (logMessage.Length() > 0)
		{
			WriteUFCLog(logHead, logMessage, constFlush);
			SetParseMessage(logMessage);
			if (FOnError != NULL) FOnError(logMessage);
		}
		return;
	}  //if (isExceptionHappened)

	String exchangeCode = ExchangePtr->GetExchangeCode(rwLockForRead);
	if (brokerContractCode.Length() <= 0) brokerContractCode = CommodityPtr->GetBrokerContractCode(rwLockForRead);
	if (tradeSymbol.Length() <= 0)
	{
		logMessage.printf(L"No Contract tradeSymbol. Exchange[%s] Broker[%s-%s]", exchangeCode, brokerExchangeCode, brokerContractCode);
		WriteUFCLog(logHead, logMessage, constFlush);
		return;
	}

	double strikePrice = 0.0;
	String symbol = CTradingContract::GenerateRfaSymbol(CommodityPtr->GetContractCode(rwLockForRead), strikePrice, maturityYearMonth, CallPut);
	CTradingContract *newContractPtr = new CTradingContract(exchangeCode, symbol, Market, strikePrice, CallPut, CommodityPtr, ExchangePtr);
	newContractPtr->SetTradeSymbol(tradeSymbol, rwNotLock);
	newContractPtr->SetBrokerSymbol(tradeSymbol, rwNotLock);
	if (gQueryObjsPool.AddContract(newContractPtr, rwLockForWrite))
		FContractCount++;
	else
	{
		logMessage.printf(L"Add Contract[%s-%s] Failed. Trade[%s]", FQueryName, exchangeCode, symbol, tradeSymbol);
		WriteUFCLog(logHead, logMessage, constFlush);
		delete newContractPtr;
	}  //if (!gQueryObjsPool.AddContract(newContractPtr, s888::rwLockForWrite))
}  //COverseasContractQueryObject::ParseContractNode()
//---------------------------------------------------------------------------
void COverseasContractQueryObject::ParseCommodityNode(_di_IXMLNode CommodityNode)
{
	//Declare Commodity Node Data Variable
	String brokerExchangeCode = L"";
	String brokerContractCode = L"";
	nsOrderMessageDefine::MarketEnum market;
	nsOrderMessageDefine::EventTypeEnum callPut = nsOrderMessageDefine::evtNone;
	String commodityName = L"";
	String currency = L"";
	String contractCode = L"";
	double contractMultiplier = 1.0;

	bool isExceptionHappened = true;
	String logHead = FQueryName + L"::ParseCommodityNode()";
	String logMsg = L"";
	try
	{   //Parse Commodity
		_di_IXMLNode curFieldNode = CommodityNode->ChildNodes->First();
		while (curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				String curFieldName = curFieldNode->GetNodeName().Trim();
				if (curFieldName == L"comtype")
				{
					String marketStr = curFieldNode->GetText().Trim();
					if (marketStr == L"1")
						market = nsOrderMessageDefine::mForeignOptions;
					else
						market = nsOrderMessageDefine::mForeignFutures;
				}
				else if (curFieldName == L"exh")
					brokerExchangeCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"comno")
					brokerContractCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"callput")
				{
					String callOrPutStr = curFieldNode->GetText().Trim();
					if (callOrPutStr == L"C")
						callPut = nsOrderMessageDefine::evtCall;
					else if (callOrPutStr == L"P")
						callPut = nsOrderMessageDefine::evtPut;
					else
						callPut = nsOrderMessageDefine::evtNone;
				}
				else if (curFieldName == L"comname")
					commodityName = curFieldNode->GetText().Trim();
				else if (curFieldName == L"currency")
					currency = curFieldNode->GetText().Trim();
				else if (curFieldName == L"CommodityID")  //RIC Root
					contractCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"decp")  //報價換算乘數(PATS價格 = TANDEM價格 * 換算乘數)
				{
					String contractMultiplierStr = curFieldNode->GetText().Trim();
					if (contractMultiplierStr.Length() > 0)
						contractMultiplier = contractMultiplierStr.ToDouble();
				}
			}  //if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)
		isExceptionHappened = false;
	}
	catch (EXMLDocError& e) {
		logMsg.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMsg.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened)
	{
		if (logMsg.Length() > 0)
		{
			WriteUFCLog(logHead, logMsg, constFlush);
			SetParseMessage(logMsg);
			if (FOnError != NULL) FOnError(logMsg);
		}
		return;
	}  //if (isExceptionHappened)

	if (brokerContractCode.Length() <= 0)
	{
		logMsg.printf(L"No Broker Contract Code.");
		WriteUFCLog(logHead, logMsg, constFlush);
		return;
	}  //if (brokerContractCode.Length() <= 0)

	String exchangeCode = brokerExchangeCode;
	bool isStopOrderSupported = false;
	bool isDayTradeOpenSupported = false;
	CTradingExchange *exchangePtr = gQueryObjsPool.FindExchangeFromBrokerCodeIndex(brokerExchangeCode, rwLockForRead);
	if (exchangePtr != 0)
	{
		exchangePtr->LockForRead();
		exchangeCode = exchangePtr->GetExchangeCode(rwNotLock);
		isStopOrderSupported = exchangePtr->IsStopOrderSupported(rwNotLock);
		isDayTradeOpenSupported = exchangePtr->IsDayTradeOpenSupported(rwNotLock);
		exchangePtr->UnlockForRead();
	}
	else
	{
		logMsg.printf(L"Wrong Borker Exchange Code[%s].", brokerExchangeCode);
		WriteUFCLog(logHead, logMsg, constFlush);
	}  //if (exchangePtr != 0)

	if (contractCode.Length() <= 0) contractCode = brokerContractCode;
	String tradeContractCode = brokerContractCode;
	CTradingCommodity *curCommodityPtr = gQueryObjsPool.FindCommodityFromCodeIndex(exchangeCode, contractCode, rwLockForRead);
	if (curCommodityPtr == 0)
	{
		double taxRate = 0.0;
		curCommodityPtr = new CTradingCommodity(exchangeCode, contractCode, market,
												isDayTradeOpenSupported, isStopOrderSupported,
												taxRate, exchangePtr);
		curCommodityPtr->SetBrokerContractCode(brokerContractCode, rwNotLock);
		curCommodityPtr->SetTradeContractCode(tradeContractCode, rwNotLock);
		if (commodityName.Length() > 0) curCommodityPtr->SetCommodityName(commodityName, rwNotLock);
		if (currency.Length() > 0) curCommodityPtr->SetCurrency(currency, rwNotLock);
		curCommodityPtr->SetContractMultiplier(contractMultiplier, rwNotLock);

		if (!gQueryObjsPool.AddCommodity(curCommodityPtr, rwLockForWrite))
		{
			logMsg.printf(L"Add Commodity[%s-%s] Failed.  Broker[%s]", exchangeCode, contractCode, brokerContractCode);
			WriteUFCLog(logHead, logMsg, constFlush);
			delete curCommodityPtr;
			curCommodityPtr = 0;
		}
		FCommodityCount++;
	}  //if (curCommodityPtr == 0)

	isExceptionHappened = true;
	logMsg = L"";
	try
	{   //Parse Contract
		_di_IXMLNode curFieldNode = CommodityNode->ChildNodes->First();
		while (curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes)
			{
				String curFieldName = curFieldNode->GetNodeName();
				if (curFieldName == L"fbmcom") ParseContractNode(exchangePtr, curCommodityPtr, market, callPut, curFieldNode);
			}  //if (curFieldNode->HasChildNodes)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)
		isExceptionHappened = false;
	}
	catch (EXMLDocError& e) {
		logMsg.printf(L"Parse Contract Loop, EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMsg.printf(L"Parse Contract Loop, Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"Parse Contract Loop, Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened && (logMsg.Length() > 0))
	{
		SetParseMessage(logMsg);
		WriteUFCLog(logHead, logMsg, constFlush);
		if (FOnError != NULL) FOnError(logMsg);
	}  //if (isExceptionHappened && (logMessage.Length() > 0))
}  //COverseasContractQueryObject::ParseCommodityNode()
//---------------------------------------------------------------------------
void COverseasContractQueryObject::ParseExchangeNode(_di_IXMLNode ExchangeNode)
{
	String brokerExchangeCode = L"";
	String exchangeName = L"";
	bool isStopOrderSupported = false;
	bool isDayTradeOpenSupported = false;
	String logHead = FQueryName + L"::ParseExchangeNode()";
	String logMsg = L"";
	bool isExceptionHappened = true;
	try
	{
		_di_IXMLNode curFieldNode = ExchangeNode->ChildNodes->First();
		while (curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				String curFieldName = curFieldNode->GetNodeName().Trim();
				if (curFieldName == L"exh")
					brokerExchangeCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"exhname")
					exchangeName = curFieldNode->GetText().Trim();
				else if (curFieldName == L"stpflag")
				{
					String stopOrderSupportedStr = curFieldNode->GetText().Trim();
					if (stopOrderSupportedStr == L"1") isStopOrderSupported = true;
				}
				else if (curFieldName == L"dtoverflag")
				{
					String dayTradeOpenSupportedStr = curFieldNode->GetText().Trim();
					if (dayTradeOpenSupportedStr == L"1") isDayTradeOpenSupported = true;
				}
			}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)
		isExceptionHappened = false;
	} catch (EXMLDocError& e) {
		logMsg.printf(L"EXMLDocError Exception:%s. URL[%s]", e.Message, FQueryURL);
	} catch (Exception& e1) {
		logMsg.printf(L"Exception:%s. URL[%s]", e1.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"Unknown Exception. URL[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened)
	{
		if (logMsg.Length() > 0)
		{
			WriteUFCLog(logHead, logMsg, constFlush);
			SetParseMessage(logMsg);
		}
		if (FOnError != NULL) FOnError(logMsg);
		return;
	}  //if (isExceptionHappened)

	String exchangeCode = CTradingExchange::ConvertBrokerExchangeCodeToCode(brokerExchangeCode);
	if (exchangeCode.Length() > 0)
	{
		gQueryObjsPool.LockExchange(rwLockForWrite);
		CTradingExchange *exchangePtr = gQueryObjsPool.FindExchangeFromCodeIndex(exchangeCode, rwNotLock);
		if (exchangePtr == 0)
			exchangePtr = gQueryObjsPool.FindExchangeFromBrokerCodeIndex(brokerExchangeCode, rwNotLock);
		if (exchangePtr == 0)
		{
			exchangePtr = new CTradingExchange(exchangeCode, exchangeName, isDayTradeOpenSupported, isStopOrderSupported, brokerExchangeCode);
			if (gQueryObjsPool.InsertExchangeIntoCodeIndex(exchangeCode, exchangePtr, constNotCheckExist, rwNotLock))
			{
				FExchangeCount++;
				if (!gQueryObjsPool.InsertExchangeIntoBrokerCodeIndex(brokerExchangeCode, exchangePtr, constCheckExist, rwNotLock))
				{
					logMsg.printf(L"Insert Exchange[%s] Broker[%s] Code Index Failed.", exchangeCode, brokerExchangeCode);
					WriteUFCLog(logHead, logMsg, constNotFlush);
				}
			}
			else
			{
				delete exchangePtr;
				exchangePtr = 0;
				logMsg.printf(L"Insert Exchange[%s] Code Index Failed. Broker[%s]", exchangeCode, brokerExchangeCode);
				WriteUFCLog(logHead, logMsg, constNotFlush);
			}
		}  //if (exchangePtr == 0)
		gQueryObjsPool.UnlockExchange(rwLockForWrite);
	}
	else
	{
		logMsg.printf(L"Missing ExchangeCode. URL=[%s]", FQueryURL);
		WriteUFCLog(logHead, logMsg, constNotFlush);
	}  //if (exchangeCode.Length() > 0)
	FlushUFCLog();
}  //COverseasContractQueryObject::ParseExchangeNode()
//---------------------------------------------------------------------------
bool COverseasContractQueryObject::ParseResponseXML(_di_IXMLDocument ResponseXML)
{
	bool isSuccess = false;
	String logHead = FQueryName + L"::ParseResponseXML()";
	String logStr = L"";
	FParseMessage = L"";
	FExchangeCount = 0;
	FCommodityCount = 0;
	FContractCount = 0;
	try
	{
		_di_IXMLNode rootNode = ResponseXML->DocumentElement;
		if (rootNode != 0)
		{   //Parse Exchange Node
			_di_IXMLNode curChildNode = rootNode->ChildNodes->First();
			String curChildNodeName = L"";
			while (curChildNode != 0)
			{
				if (curChildNode->HasChildNodes)
				{
					curChildNodeName = curChildNode->GetNodeName();
					if (curChildNodeName == L"fomexh") ParseExchangeNode(curChildNode);
				}  //if (curChildNodeName == L"fbmcob")
				curChildNode = curChildNode->NextSibling();
			}  //while (curChildNode != 0)

			//Parse Commodity Node
			curChildNode = rootNode->ChildNodes->First();
			while (curChildNode != 0)
			{
				if (curChildNode->HasChildNodes)
				{
					curChildNodeName = curChildNode->GetNodeName();
					if (curChildNodeName == L"fbmcob") ParseCommodityNode(curChildNode);
				}  //if (curChildNodeName == L"fbmcob")
				curChildNode = curChildNode->NextSibling();
			}  //while (curChildNode != 0)
		}  //if (rootNode != 0)
		isSuccess = true;
	} catch (EXMLDocError& e) {
		logStr.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logStr.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logStr.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (!isSuccess && (logStr.Length() > 0))
	{
		WriteUFCLog(logHead, logStr, constNotFlush);
		SetParseMessage(logStr);
		if (FOnError != NULL) FOnError(logStr);
	}  //if (!isSuccess && (logMessage.Length() > 0))
	logStr.printf(L"%d Commodities, %d Contracts.", FCommodityCount, FContractCount);
	WriteUFCLog(logHead, logStr, constFlush);
	return isSuccess;
}  //COverseasContractQueryObject::ParseResponseXML()
//---------------------------------------------------------------------------
void COverseasContractQueryObject::SetQueryStatus(QueryStatusEnum QueryStatus)
{
	FQueryStatus = QueryStatus;
	gQueryObjsPool.SetOverseasContractQueryStatus(QueryStatus);
}  //COverseasContractQueryObject::SetQueryStatus()
//---------------------------------------------------------------------------
void COverseasContractQueryObject::SetQueryMessage(const String& QueryMessage)
{
	FQueryMessage = QueryMessage;
	gQueryObjsPool.SetOverseasContractQueryMessage(QueryMessage);
}  //COverseasContractQueryObject::SetQueryMessage()
//---------------------------------------------------------------------------
void COverseasContractQueryObject::SetParseMessage(const String& ParseMessage)
{
	FParseMessage = ParseMessage;
	gQueryObjsPool.SetOverseasContractQueryMessage(ParseMessage);
}  //COverseasContractQueryObject::SetParseMessage()

//------------------------ CTaifexTaxRateQueryObject ------------------------
String CTaifexTaxRateQueryObject::FExchangeCode = L"TAIFEX";
//---------------------------------------------------------------------------
CTaifexTaxRateQueryObject::CTaifexTaxRateQueryObject(const String& QueryURL, int ConnectTimeOut, const String& DumpFileName, bool DoDump, bool CreateSuspended)
:CBasicTradingQueryObject(QueryURL, ConnectTimeOut, DumpFileName, DoDump, CreateSuspended)
{
	FQueryName = L"TaifexTaxRate";
	if (DumpFileName.Length() <= 0)
		FDumpFileName = L"Super888_TaifexTaxRate.xml";
}  //CTaifexTaxRateQueryObject::CTaifexContractQueryObject()

//---------------------------------------------------------------------------
CTaifexTaxRateQueryObject::CTaifexTaxRateQueryObject(const String& XMLFile )
:CBasicTradingQueryObject(L" ", 30, L"SpeedyUnify_TaifexTax.xml", false, true )
{
	_di_IXMLDocument newXMLDoc = NewXMLDocument();
	newXMLDoc->LoadFromFile( XMLFile );
	ParseResponseXML( newXMLDoc );
}
//---------------------------------------------------------------------------
void CTaifexTaxRateQueryObject::ParseTaxRateNode(_di_IXMLNode RateNode)
{
	String logHead = FQueryName + L"::ParseTaxRateNode()";
	String logStr = L"";
	bool isExceptionHappened = true;
	int processCount = 0;
	int createCount = 0;
	try
	{
		_di_IXMLNode curFieldNode = RateNode->ChildNodes->First();
		while (curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				processCount++;
				nsOrderMessageDefine::MarketEnum market = nsOrderMessageDefine::mTWFutures;
				String brokerContractCode = curFieldNode->GetNodeName().Trim();
				String contractCode = brokerContractCode;
				String taxRateStr = curFieldNode->GetText().Trim();
				double taxRate = 0.0;
				if( taxRateStr.Length() > 0 )
					taxRate = taxRateStr.ToDouble();

				if( contractCode[3] == 'F' )
					market = nsOrderMessageDefine::mTWFutures;
				else if( contractCode[3] == 'O' )
					market = nsOrderMessageDefine::mTWOptions;

				CTaxRateObject *newTaxRateObjPtr = new CTaxRateObject(FExchangeCode, contractCode, taxRate, brokerContractCode, market );
				if( gTradingObjsPool.AddTaxRate(newTaxRateObjPtr, rwLockForWrite) )
					createCount++;
				else
				{
					logStr.printf(L"Add Tax Rate[%s-%s] Failed. taxRate[%s], broker[%s]", FExchangeCode, contractCode, taxRateStr, brokerContractCode);
					WriteUFCLog(logHead, logStr, constNotFlush);
					delete newTaxRateObjPtr;
				}
			}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)
		isExceptionHappened = false;
	} catch (EXMLDocError& e) {
		logStr.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logStr.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logStr.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened && (logStr.Length() > 0))
	{
		WriteUFCLog(logHead, logStr, constNotFlush);
		SetParseMessage(logStr);
		if (FOnError != NULL) FOnError(logStr);
	}  //if (isExceptionHappened && (logMessage.Length() > 0))
	logStr.printf(L"Process %d TaxRate Node, Create %d TaxRate Object", processCount, createCount);
	WriteUFCLog(logHead, logStr, constFlush);
}  //CTaifexTaxRateQueryObject::ParseTaxRateNode()
//---------------------------------------------------------------------------
bool CTaifexTaxRateQueryObject::ParseResponseXML(_di_IXMLDocument ResponseXML )
{
	bool isSuccess = false;
	String logHead = FQueryName + L"::ParseResponseXML()";
	String logMsg = L"";
	int rowsCount = 0;
	FParseMessage = L"";
	try
	{
		_di_IXMLNode rootNode = ResponseXML->DocumentElement;
		if (rootNode != 0)
		{
			_di_IXMLNode curChildNode = rootNode->ChildNodes->First();
			String curChildNodeName = L"";
			while (curChildNode != 0)
			{
				if (curChildNode->HasChildNodes && curChildNode->IsTextElement)
				{
					curChildNodeName = curChildNode->GetNodeName().Trim();
					if (curChildNodeName == L"result")
					{
						String resultStr = curChildNode->GetText().Trim();
						if ((resultStr == L"Y") || (resultStr == L"y")) isSuccess = true;
					}
					else if (curChildNodeName == L"rows")
					{
						String rowsCountStr = curChildNode->GetText().Trim();
						rowsCount = rowsCountStr.ToInt();
					}
				}  //if (curChildNode->HasChildNodes && curChildNode->IsTextElement)
				curChildNode = curChildNode->NextSibling();
			}  //while (curChildNode != 0)

			if (!isSuccess)
			{
				logMsg.printf(L"Query result is not \"Y\".");
				WriteUFCLog(logHead, logMsg, constFlush);
			}
			else
			{   //Parse TaxRate Node
				curChildNode = rootNode->ChildNodes->First();
				while (curChildNode != 0)
				{
					if (curChildNode->HasChildNodes)
					{
						curChildNodeName = curChildNode->GetNodeName().Trim();
						if (curChildNodeName == L"rate") ParseTaxRateNode(curChildNode);
					}  //if (curChildNodeName == L"fbmcob")
					curChildNode = curChildNode->NextSibling();
				}  //while (curChildNode != 0)
			}  //if (!isSuccess)
		}  //if (rootNode != 0)
	} catch (EXMLDocError& e) {
		logMsg.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMsg.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (!isSuccess && (logMsg.Length() > 0))
	{
		WriteUFCLog(logHead, logMsg, constFlush);
		SetParseMessage(logMsg);
		if (FOnError != NULL) FOnError(logMsg);
	}  //if (!isSuccess && (logMessage.Length() > 0))
	logMsg.printf(L"Rows Count=%d.", rowsCount);
	WriteUFCLog(logHead, logMsg, constFlush);
	return isSuccess;
}  //CTaifexTaxRateQueryObject::ParseResponseXML()
//---------------------------------------------------------------------------
void CTaifexTaxRateQueryObject::SetQueryStatus(QueryStatusEnum QueryStatus)
{
	FQueryStatus = QueryStatus;
	gQueryObjsPool.SetTaifexTaxRateQueryStatus(QueryStatus);
}  //CTaifexTaxRateQueryObject::SetQueryStatus()
//---------------------------------------------------------------------------
void CTaifexTaxRateQueryObject::SetQueryMessage(const String& QueryMessage)
{
	FQueryMessage = QueryMessage;
	gQueryObjsPool.SetTaifexTaxRateQueryMessage(QueryMessage);
}  //CTaifexTaxRateQueryObject::SetQueryMessage()
//---------------------------------------------------------------------------
void CTaifexTaxRateQueryObject::SetParseMessage(const String& ParseMessage)
{
	FParseMessage = ParseMessage;
	gQueryObjsPool.SetTaifexTaxRateQueryMessage(ParseMessage);
}  //CTaifexTaxRateQueryObject::SetParseMessage()

//-------------------------- CTaifexFeeQueryObject --------------------------
String CTaifexFeeQueryObject::FExchangeCode = L"TAIFEX";
//---------------------------------------------------------------------------
CTaifexFeeQueryObject::CTaifexFeeQueryObject(const String& XMLFile, const String& LogonID )
:CBasicTradingQueryObject( L"", 30, L"SpeedyUnify_TaifexFee.xml", false, true ),
 FLogonID(LogonID),
 FBrokerID(L"F888000")
{
	FQueryName = L"TaifexFee";
	FAccountPtr = gTradingObjsPool.FindAccountFromLogonIdIndex(LogonID, rwLockForRead);
	if (FAccountPtr != 0)
	{
		FAccountPtr->LockForWrite();
		FAccountPtr->SetTaifexFeeQueryStatus(qsNone, rwNotLock);
		FAccountPtr->SetTaifexFeeQueryMessage(L"", rwNotLock);
		FAccountPtr->UnlockForWrite();
	}
	_di_IXMLDocument newXMLDoc = NULL;
	newXMLDoc = NewXMLDocument();
	newXMLDoc->LoadFromFile( XMLFile );
	ParseResponseXML( newXMLDoc );
}
//---------------------------------------------------------------------------
CTaifexFeeQueryObject::CTaifexFeeQueryObject(const String& QueryURL, const String& LogonID, const String& BrokerID, int ConnectTimeOut, const String& DumpFileName, bool DoDump, bool CreateSuspended)
:CBasicTradingQueryObject(QueryURL, ConnectTimeOut, DumpFileName, DoDump, CreateSuspended),
 FLogonID(LogonID),
 FBrokerID(BrokerID),
 FAccountPtr(0)
{
	FQueryName.printf(L"TaifexFee_%s", FLogonID);
	if (DumpFileName.Length() <= 0) FDumpFileName.printf(L"Super888_%s_TaifexFee.xml", FLogonID);

	FAccountPtr = gTradingObjsPool.FindAccountFromLogonIdIndex(FLogonID, rwLockForRead);
	if (FAccountPtr != 0)
	{
		FAccountPtr->LockForWrite();
		FAccountPtr->SetTaifexFeeQueryStatus(qsNone, rwNotLock);
		FAccountPtr->SetTaifexFeeQueryMessage(L"", rwNotLock);
		FAccountPtr->UnlockForWrite();
	}
}  //CTaifexFeeQueryObject::CTaifexFeeQueryObject()
//---------------------------------------------------------------------------
String CTaifexFeeQueryObject::CombineQueryURL(String& QueryURL)
{
	//Paul Edit 20140408 EDS TaifexFee need use actno + company to URL.
	String combinedURL = QueryURL + L"?actno=" + FLogonID + "&company=" + FBrokerID;
	return combinedURL;
}  //CTaifexFeeQueryObject::CombineQueryURL()
//---------------------------------------------------------------------------
nsOrderMessageDefine::MarketEnum CTaifexFeeQueryObject::FindMarket( const String& ProdID )
{
	if( ProdID[2] == 'F' ) ///< XXF
		return nsOrderMessageDefine::mTWFutures;
	else if( ProdID[2] == 'O' ) ///< XXO
		return nsOrderMessageDefine::mTWOptions;
	else
	{
		String Prefix = ProdID.SubString(1, 2);
		if( Prefix == L"TX" )
			return nsOrderMessageDefine::mTWOptions;
		else  if( Prefix == L"MX" )
			return nsOrderMessageDefine::mTWFutures;
	}
	return nsOrderMessageDefine::mTWFutures;
}
//---------------------------------------------------------------------------
void CTaifexFeeQueryObject::ParseFeeNode(_di_IXMLNode FeeNode)
{
	int processCount = 0;
	int createCount = 0;
	String logHead = FQueryName + L"::ParseFeeNode()";
	String logMsg = L"";
	bool isExceptionHappened = true;
	try
	{
		_di_IXMLNode curFieldNode = FeeNode->ChildNodes->First();
		while( curFieldNode != 0 )
		{
			processCount++;
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				String brokerContractCode = curFieldNode->GetNodeName().Trim();
				String contractCode       = brokerContractCode;
				String tradeContractCode  = brokerContractCode;
				nsOrderMessageDefine::MarketEnum market;
				String unitFeeStr = curFieldNode->GetText().Trim();
				double unitFee = 0.0;

				if( unitFeeStr.Length() > 0 ) unitFee = unitFeeStr.ToDouble();
				if( brokerContractCode == L"STOCKFUT")
					market = nsOrderMessageDefine::mTWFutures;
				else if (brokerContractCode == L"STOCKOPT")
					market = nsOrderMessageDefine::mTWOptions;
				else if (brokerContractCode == L"STOCK")
					market = nsOrderMessageDefine::mTSE;
				else
					market = FindMarket( brokerContractCode );
				if (FAccountPtr != 0)
				{
					CFeeObject *feeObjPtr = new CFeeObject( FLogonID, FExchangeCode, contractCode,0.0, unitFee, 0.0, unitFee, 0.0, unitFee,market );
					feeObjPtr->SetTradeContractCode(tradeContractCode, rwNotLock);
					feeObjPtr->SetBrokerContractCode(brokerContractCode, rwNotLock);
					if (brokerContractCode == L"STOCKFUT") ///< Stock Futures
					{
						FAccountPtr->SetStockFutFee(feeObjPtr, rwLockForWrite);
						createCount++;
					}
					else if (brokerContractCode == L"STOCKOPT") ///< Stock Options
					{
						FAccountPtr->SetStockOptFee(feeObjPtr, rwLockForWrite);
						createCount++;
					}
					else if(brokerContractCode == L"STOCK") ///< Stock
					{
						feeObjPtr->SetOpenRateByMoney( 0.001425 * unitFee );
						feeObjPtr->SetOpenRateByVolume( 0.0 );
						feeObjPtr->SetCloseRateByMoney( 0.001425 * unitFee );
						feeObjPtr->SetCloseRateByVolume( 0.0 );
						feeObjPtr->SetCloseTodayRateByMoney( 0.001425 * unitFee );
						feeObjPtr->SetCloseTodayRateByVolume( 0.0 );
						FAccountPtr->SetStockFee( feeObjPtr, rwLockForWrite );
						createCount++;
					}
					else ///< Buy Product ID
					{
						if (FAccountPtr->AddFee( feeObjPtr, rwLockForWrite))
							createCount++;
						else
						{
							delete feeObjPtr;
							logMsg.printf(L"Add commodity[%s-%s] fee into Account[%s] Falied. Broker[%s]", FExchangeCode, contractCode, FLogonID, brokerContractCode);
							WriteUFCLog(logHead, logMsg, constFlush);
						}
					}  //if (brokerContractCode == L"STOCKFUT")
				}
				else
				{
					logMsg.printf(L"Account[%s] Not Exist. commodity[%s-%s] Broker[%s]", FLogonID, FExchangeCode, contractCode, brokerContractCode);
					WriteUFCLog(logHead, logMsg, constFlush);
				}

			}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)
		isExceptionHappened = false;
	} catch (EXMLDocError& e) {
		logMsg.printf(L" EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMsg.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened && (logMsg.Length() > 0))
	{
		SetParseMessage(logMsg);
		WriteUFCLog(logHead, logMsg, constFlush);
		if (FOnError != NULL) FOnError(logMsg);
	}  //if (isExceptionHappened && (logMessage.Length() > 0))
	logMsg.printf(L"Process %d Fee Node, Create %d Fee Object", processCount, createCount);
	WriteUFCLog(logHead, logMsg, constFlush);
	gTradingObjsPool.SetTaifexCommodityFeeObject(FAccountPtr);
}  //CTaifexFeeQueryObject::ParseFeeNode()
//---------------------------------------------------------------------------
bool CTaifexFeeQueryObject::ParseResponseXML(_di_IXMLDocument ResponseXML )
{
	String logHead = FQueryName + L"::ParseResponseXML()";
	String logMsg = L"";
	bool isSuccess = false;
	String responseAccount = L"";
	FParseMessage = L"";
	try
	{
		_di_IXMLNode rootNode = ResponseXML->DocumentElement;
		if (rootNode != 0)
		{   //Parse Exchange Node
			_di_IXMLNode curChildNode = rootNode->ChildNodes->First();
			String curChildNodeName = L"";
			while (curChildNode != 0)
			{
				if (curChildNode->HasChildNodes && curChildNode->IsTextElement)
				{
					curChildNodeName = curChildNode->GetNodeName().Trim();
					if (curChildNodeName == L"result")
					{
						String resultStr = curChildNode->GetText().Trim();
						if ((resultStr == L"Y") || (resultStr == L"y")) isSuccess = true;
					}
					else if (curChildNodeName == L"actno")
						responseAccount = curChildNode->GetText().Trim();
				}  //if (curChildNode->HasChildNodes && curChildNode->IsTextElement)
				curChildNode = curChildNode->NextSibling();
			}  //while (curChildNode != 0)

/*			if( CheckAccount == true && responseAccount != FLogonID)
			{
				logMsg.printf(L"different Account. ResponseAcc[%s], QueryLogonID[%s]", responseAccount, FLogonID);
				WriteUFCLog(logHead, logMsg, constFlush);
			}
			if (!isSuccess)
				WriteUFCLog(logHead, L"Query result is not \"Y\".", constFlush);
			else*/
			{   //Parse Fee Node
				curChildNode = rootNode->ChildNodes->First();
				while (curChildNode != 0)
				{
					if (curChildNode->HasChildNodes)
					{
						curChildNodeName = curChildNode->GetNodeName().Trim();
						if (curChildNodeName == L"fee") ParseFeeNode(curChildNode);
					}  //if (curChildNode->HasChildNodes)
					curChildNode = curChildNode->NextSibling();
				}  //while (curChildNode != 0)
			}  //if (!isSuccess)
		}  //if (rootNode != 0)
	} catch (EXMLDocError& e) {
		logMsg.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMsg.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (!isSuccess && (logMsg.Length() > 0))
	{
		WriteUFCLog(logHead, logMsg, constFlush);
		SetParseMessage(logMsg);
		if (FOnError != NULL) FOnError(logMsg);
	}  //if (!isSuccess && (logMessage.Length() > 0))
	return isSuccess;
}  //CTaifexFeeQueryObject::ParseResponseXML()
//---------------------------------------------------------------------------
void CTaifexFeeQueryObject::SetQueryStatus(QueryStatusEnum QueryStatus)
{
	FQueryStatus = QueryStatus;
	if (FAccountPtr != 0) FAccountPtr->SetTaifexFeeQueryStatus(QueryStatus, rwLockForWrite);
}  //CTaifexFeeQueryObject::SetQueryStatus()
//---------------------------------------------------------------------------
void CTaifexFeeQueryObject::SetQueryMessage(const String& QueryMessage)
{
	FQueryMessage = QueryMessage;
	if (FAccountPtr != 0) FAccountPtr->SetTaifexFeeQueryMessage(QueryMessage, rwLockForWrite);
}  //CTaifexFeeQueryObject::SetQueryMessage()
//---------------------------------------------------------------------------
void CTaifexFeeQueryObject::SetParseMessage(const String& ParseMessage)
{
	FParseMessage = ParseMessage;
	if (FAccountPtr != 0) FAccountPtr->SetTaifexFeeQueryMessage(ParseMessage, rwLockForWrite);
}  //CTaifexFeeQueryObject::SetParseMessage()

//------------------------- COverseasFeeQueryObject -------------------------
//---------------------------------------------------------------------------
COverseasFeeQueryObject::COverseasFeeQueryObject(const String& QueryURL, const String& LogonID, int ConnectTimeOut, const String& DumpFileName, bool DoDump, bool CreateSuspended)
:CBasicTradingQueryObject(QueryURL, ConnectTimeOut, DumpFileName, DoDump, CreateSuspended),
 FLogonID(LogonID),
 FUserKey(LogonID),
 FAccountPtr(0)
{
	FQueryName.printf(L"OverseasFee_%s", FLogonID);
	if (DumpFileName.Length() <= 0)
		FDumpFileName.printf(L"Super888_%s_OverseasFee.xml", FLogonID);

	FAccountPtr = gTradingObjsPool.FindAccountFromLogonIdIndex(FLogonID, rwLockForRead);
	if (FAccountPtr != 0)
	{
		FAccountPtr->LockForWrite();
		FAccountPtr->SetOverseasFeeQueryStatus(qsNone, rwNotLock);
		FAccountPtr->SetOverseasFeeQueryMessage(L"", rwNotLock);
		FUserKey = FAccountPtr->GetUserKey(rwNotLock);
		FAccountPtr->UnlockForWrite();
	}
	else
		FUserKey = FLogonID;
}  //COverseasFeeQueryObject::COverseasFeeQueryObject()
//---------------------------------------------------------------------------
String COverseasFeeQueryObject::CombineQueryURL(String& QueryURL)
{
	String combinedURL = QueryURL + L"?uk=" + FUserKey + "&rtntype=F";  //F:完整, S:簡單
	return combinedURL;
}  //COverseasFeeQueryObject::CombineQueryURL()
//---------------------------------------------------------------------------
bool COverseasFeeQueryObject::ParseFeeNode(_di_IXMLNode FeeNode)
{
	String logHead = FQueryName + L"::ParseFeeNode()";
	String logMsg = L"";
	String brokerExchangeCode = L"";
	String brokerContractCode = L"";
	String unitFeeStr = L"";
	double unitFee = 0.0;
	bool isExceptionHappened = true;
	try
	{
		_di_IXMLNode curFieldNode = FeeNode->ChildNodes->First();
		while (curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				String curFieldName = curFieldNode->GetNodeName().Trim();
				if (curFieldName == L"exh")
					brokerExchangeCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"comno")
					brokerContractCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"fee")
				{
					unitFeeStr = curFieldNode->GetText();
					if (unitFeeStr.Length() > 0) unitFee = unitFeeStr.ToDouble();
				}
			}  //if (FieldNode->HasChildNodes && FieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while (curFieldNode != 0)
		isExceptionHappened = false;
	} catch (EXMLDocError& e) {
		logMsg.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMsg.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened)
	{
		if (logMsg.Length() > 0)
		{
			WriteUFCLog(logHead, logMsg, constFlush);
			SetParseMessage(logMsg);
			if (FOnError != NULL) FOnError(logMsg);
		}
		return false;
	}  //if (isExceptionHappened)

	String exchangeCode = brokerExchangeCode;
	gTradingObjsPool.LockExchange(rwLockForRead);
	CTradingExchange *exchangePtr = gTradingObjsPool.FindExchangeFromBrokerCodeIndex(brokerExchangeCode, rwNotLock);
	if (exchangePtr == 0)
		exchangePtr = gTradingObjsPool.FindExchangeFromCodeIndex(brokerExchangeCode, rwNotLock);
	gTradingObjsPool.UnlockExchange(rwLockForRead);
	if (exchangePtr != 0)
		exchangeCode = exchangePtr->GetExchangeCode(rwLockForRead);
	else
	{
		logMsg.printf(L"Exchange[%s] not found in ExchangeCode Index and broker ExchangeCode Index.", exchangeCode);
		WriteUFCLog(logHead, logMsg, constFlush);
	}

	String contractCode = brokerContractCode;
	String tradeContractCode = brokerContractCode;
	nsOrderMessageDefine::MarketEnum market = nsOrderMessageDefine::mOTC;
	gTradingObjsPool.LockCommodity(rwLockForRead);
	CTradingCommodity *commodityPtr = gTradingObjsPool.FindCommodityFromBrokerCodeIndex(exchangeCode, brokerContractCode, rwNotLock);
	if (commodityPtr == 0)
		commodityPtr = gTradingObjsPool.FindCommodityFromCodeIndex(exchangeCode, contractCode, rwNotLock);
	gTradingObjsPool.UnlockCommodity(rwLockForRead);

	if (commodityPtr != 0)
	{
		commodityPtr->LockForRead();
		contractCode = commodityPtr->GetContractCode(rwNotLock);
		tradeContractCode = commodityPtr->GetTradeContractCode(rwNotLock);
		market = commodityPtr->GetMarket(rwNotLock);
		commodityPtr->UnlockForRead();
	}
	else
	{
		logMsg.printf(L"Commodity[%s-%s] Not Found in ContractCode Index and broker ContractCode Index. broker[%s]", exchangeCode, contractCode, brokerContractCode);
		WriteUFCLog(logHead, logMsg, constFlush);
	}

	if (FAccountPtr != 0)
	{
		double openRateByMoney = 0.0;
		double openRateByVolume = unitFee;
		double closeRateByMoney = 0.0;
		double closeRateByVolume = unitFee;
		double closeTodayRateByMoney = 0.0;
		double closeTodayRateByVolume = unitFee;
		CFeeObject *feeObjPtr = new CFeeObject(FLogonID, exchangeCode, contractCode,
											   openRateByMoney, openRateByVolume,
											   closeRateByMoney, closeRateByVolume,
											   closeTodayRateByMoney, closeTodayRateByVolume,
											   market);
		feeObjPtr->SetTradeContractCode(tradeContractCode, rwNotLock);
		feeObjPtr->SetBrokerContractCode(brokerContractCode, rwNotLock);
		if (!FAccountPtr->AddFee(feeObjPtr, rwLockForWrite))
		{
			delete feeObjPtr;
			logMsg.printf(L"Add commodity[%s-%s] fee into Account[%s] Falied. Broker[%s]", exchangeCode, contractCode, FLogonID, brokerContractCode);
			WriteUFCLog(logHead, logMsg, constFlush);
		}
		return true;
	}
	else
	{
		logMsg.printf(L"Account[%s] Not Exist. commodity[%s-%s] Broker[%s]", FLogonID, exchangeCode, contractCode, brokerContractCode);
		WriteUFCLog(logHead, logMsg, constFlush);
		return false;
	}
}  //COverseasFeeQueryObject::ParseFeeNode()
//---------------------------------------------------------------------------
bool COverseasFeeQueryObject::ParseResponseXML(_di_IXMLDocument ResponseXML)
{
	bool isSuccess = true;
	String logHead = FQueryName + L"::ParseResponseXML()";
	String logMsg = L"";
	String responseAccount = L"";
	String responseCompany = L"";
	String responseUserKey = L"";
	String errorMessage = L"";
	int processCount = 0;
	int createCount = 0;
	try
	{
		_di_IXMLNode rootNode = ResponseXML->DocumentElement;
		if (rootNode != 0)
		{   //Parse Exchange Node
			_di_IXMLNode curChildNode = rootNode->ChildNodes->First();
			String curChildNodeName = L"";
			while (curChildNode != 0)
			{
				if (curChildNode->HasChildNodes && curChildNode->IsTextElement)
				{
					curChildNodeName = curChildNode->GetNodeName().Trim();
					if (curChildNodeName == L"result")
					{   //不一定會有這個欄位，故預設為true
						String resultStr = curChildNode->GetText().Trim();
						if ((resultStr != L"Y") && (resultStr != L"y")) isSuccess = false;
					}
					else if (curChildNodeName == L"actno")
						responseAccount = curChildNode->GetText().Trim();
					else if (curChildNodeName == L"errmsg")
						errorMessage = curChildNode->GetText().Trim();
					else if (curChildNodeName == L"UK")
						responseUserKey = curChildNode->GetText().Trim();
					else if (curChildNodeName == L"company")
						responseCompany = curChildNode->GetText().Trim();
				}  //if (curChildNode->HasChildNodes && curChildNode->IsTextElement)
				curChildNode = curChildNode->NextSibling();
			}  //while (curChildNode != 0)

			if (responseAccount != FLogonID)
			{
				logMsg.printf(L"different Account. ResponseAcc[%s], QueryLogonID[%s]", responseAccount, FLogonID);
				WriteUFCLog(logHead, logMsg, constFlush);
			}

			if (!isSuccess)
			{
				logMsg.printf(L"Query result is not \"Y\", error:[%s], userKey[%s], company[%s]", errorMessage, responseUserKey, responseCompany);
				WriteUFCLog(logHead, logMsg, constFlush);
			}
			else
			{   //Parse Fee Node
				curChildNode = rootNode->ChildNodes->First();
				while (curChildNode != 0)
				{
					if (curChildNode->HasChildNodes)
					{
						curChildNodeName = curChildNode->GetNodeName();
						if (curChildNodeName == L"fbmcom")
						{
							processCount++;
							if (ParseFeeNode(curChildNode)) createCount++;
						}
					}  //if (curChildNode->HasChildNodes)
					curChildNode = curChildNode->NextSibling();
				}  //while (curChildNode != 0)
			}  //if (!isSuccess)
		}  //if (rootNode != 0)
	} catch (EXMLDocError& e) {
		logMsg.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMsg.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (!isSuccess && (logMsg.Length() > 0))
	{
		SetParseMessage(logMsg);
		WriteUFCLog(logHead, logMsg, constNotFlush);
		if (FOnError != NULL) FOnError(logMsg);
	}  //if (!isSuccess && (logMessage.Length() > 0))

	logMsg.printf(L"Process %d Fee Node, Create %d Fee Object", processCount, createCount);
	WriteUFCLog(logHead, logMsg, constFlush);
	return isSuccess;
}  //COverseasFeeQueryObject::ParseResponseXML()
//---------------------------------------------------------------------------
void COverseasFeeQueryObject::SetQueryStatus(QueryStatusEnum QueryStatus)
{
	FQueryStatus = QueryStatus;
	if (FAccountPtr != 0) FAccountPtr->SetOverseasFeeQueryStatus(QueryStatus, rwLockForWrite);
}  //COverseasFeeQueryObject::SetQueryStatus()
//---------------------------------------------------------------------------
void COverseasFeeQueryObject::SetQueryMessage(const String& QueryMessage)
{
	FQueryMessage = QueryMessage;
	if (FAccountPtr != 0) FAccountPtr->SetOverseasFeeQueryMessage(QueryMessage, rwLockForWrite);
}  //COverseasFeeQueryObject::SetQueryMessage()
//---------------------------------------------------------------------------
void COverseasFeeQueryObject::SetParseMessage(const String& ParseMessage)
{
	FParseMessage = ParseMessage;
	if (FAccountPtr != 0) FAccountPtr->SetOverseasFeeQueryMessage(ParseMessage, rwLockForWrite);
}  //COverseasFeeQueryObject::SetParseMessage()

//--------------------------- CWaitQueryFeeObject ---------------------------
//---------------------------------------------------------------------------
CWaitQueryFeeObject::CWaitQueryFeeObject(const String& LogonID, bool WaitQueryFee, bool CreateSuspended)
:TThread(CreateSuspended)
,FLogonID(LogonID)
,FIsTestMode(false)
,FWaitQueryFee(WaitQueryFee)
,FAccountPtr(0)
{
	FAccountPtr = gTradingObjsPool.FindAccountFromLogonIdIndex(FLogonID, rwLockForRead);
}  //CWaitQueryFeeObject::CWaitQueryFeeObject()
//---------------------------------------------------------------------------
void __fastcall CWaitQueryFeeObject::Execute(void)
{
	if (FAccountPtr == 0) return;
	if (FWaitQueryFee)
	{
		FAccountPtr->LockForRead();
		QueryStatusEnum taifexFeeQueryStatus = FAccountPtr->GetTaifexFeeQueryStatus(rwNotLock);
		QueryStatusEnum overseasFeeQueryStatus = FAccountPtr->GetOverseasFeeQueryStatus(rwNotLock);
		FAccountPtr->UnlockForRead();
		bool isTaifexFeeQueried = ((taifexFeeQueryStatus == qsFinished) || (taifexFeeQueryStatus == qsFailed));
		bool isOverseasFeeQueried = ((overseasFeeQueryStatus == qsFinished) || (overseasFeeQueryStatus == qsFailed));
		while (!isTaifexFeeQueried || !isOverseasFeeQueried)
		{
			UFC::SleepMS(500);
			FAccountPtr->LockForRead();
			taifexFeeQueryStatus = FAccountPtr->GetTaifexFeeQueryStatus(rwNotLock);
			overseasFeeQueryStatus = FAccountPtr->GetOverseasFeeQueryStatus(rwNotLock);
			FAccountPtr->UnlockForRead();
			isTaifexFeeQueried = ((taifexFeeQueryStatus == qsFinished) || (taifexFeeQueryStatus == qsFailed));
			isOverseasFeeQueried = ((overseasFeeQueryStatus == qsFinished) || (overseasFeeQueryStatus == qsFailed));
		}  //while (!isTaifexFeeQueried || !isOverseasFeeQueried)
	}  //if (FWaitQueryFee)
	UFC::BufferedLog::FlushToFile();
	if (FIsTestMode) FAccountPtr->LoadTestFeeData();
    FAccountPtr->DumpData(constFlush, rwNotLock);
}  //CWaitQueryFeeObject::Execute()

//----------------------- CTaifexHoldPositionQueryObject ----------------------
String CTaifexHoldPositionQueryObject::FExchangeCode = L"TAIFEX";
//---------------------------------------------------------------------------
CTaifexHoldPositionQueryObject::CTaifexHoldPositionQueryObject(const String& QueryURL, const String& Account, int ConnectTimeOut, const String& DumpFileName, bool DoDump, bool CreateSuspended)
:CBasicTradingQueryObject(QueryURL, ConnectTimeOut, DumpFileName, DoDump, CreateSuspended),
 FAccount(Account),
 FUserKey(Account),
 FAccountPtr(0),
 FExchangePtr(0)
{
	FQueryName.printf(L"TaifexHoldPosition_%s", FAccount);
	if (DumpFileName.Length() <= 0)
		FDumpFileName.printf(L"Super888_%s_TaifexHoldPosition.xml", FAccount);
	FExchangePtr = gTradingObjsPool.FindExchangeFromCodeIndex(FExchangeCode, rwLockForRead);
	FAccountPtr = gTradingObjsPool.FindAccountFromLogonIdIndex(Account, rwLockForRead);
	if (FAccountPtr != 0)
	{
		FAccountPtr->LockForWrite();
		FAccountPtr->SetTaifexHoldPositionQueryStatus(qsNone, rwNotLock);
		FAccountPtr->SetTaifexHoldPositionQueryMessage(L"", rwNotLock);
		FAccountPtr->ClearTaifexHoldPositionArray(rwNotLock);
		FUserKey = FAccountPtr->GetUserKey(rwNotLock);
		FAccountPtr->UnlockForWrite();
	}
}  //CTaifexHoldPositionQueryObject::CTaifexHoldPositionQueryObject()
//---------------------------------------------------------------------------
String CTaifexHoldPositionQueryObject::CombineQueryURL(String& QueryURL)
{
	String combinedURL = QueryURL + L"?UK=" + FUserKey;
	return combinedURL;
}  //CTaifexHoldPositionQueryObject::CombineQueryURL()
//---------------------------------------------------------------------------
bool CTaifexHoldPositionQueryObject::ParseHoldPositionNode(_di_IXMLNode HoldPositionNode)
{
	String logHead = FQueryName + L"::ParseHoldPositionNode()";
	String logMsg = L"";
	bool isExceptionHappened = true;
	String                              orderID = L"";                       //委託單號
	String                              tradingDate = L"";                   //交易日期
	nsOrderMessageDefine::SideEnum      side = nsOrderMessageDefine::sNone;  //買賣別
	String                              contractCode = L"";                  //合約代碼
	String                              maturityYearMonth = L"";             //到期年月
	double                              strikePrice = 0.0;                   //履約價
	int                                 quantity = 0;                        //未平倉數量
	double                              matchPrice = 0.0;                    //成交價
	bool                                isSpread = false;                    //價差單
	bool                                isDayTradeOver = false;              //當日平倉
	nsOrderMessageDefine::MarketEnum    market;                              //市場別
	nsOrderMessageDefine::EventTypeEnum callOrPut = nsOrderMessageDefine::evtNone;  //Call or Put
	try
	{
		_di_IXMLNode curFieldNode = HoldPositionNode->ChildNodes->First();
		while(curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				String curFieldName = curFieldNode->GetNodeName().Trim();
				if (curFieldName == L"ordno")  //委託單號
					orderID = curFieldNode->GetText().Trim();
				else if (curFieldName == L"trddt")  //交易日期
					tradingDate = curFieldNode->GetText().Trim();
				else if (curFieldName == L"ps")  //買賣別
				{
					String buySellStr = curFieldNode->GetText().Trim();
					if (buySellStr.Length() > 0)
					{
						if (buySellStr == L"B")
							side = nsOrderMessageDefine::sBuy;
						else if (buySellStr == L"S")
							side = nsOrderMessageDefine::sSell;
					}
				}
				else if (curFieldName == L"comno")  //合約代碼
					contractCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"comym")  //到期年月
					maturityYearMonth = curFieldNode->GetText().Trim();
				else if (curFieldName == L"stkprc")  //履約價
				{
					String strikePriceStr = curFieldNode->GetText().Trim();
					if (strikePriceStr.Length() > 0) strikePrice = strikePriceStr.ToDouble();
				}
				else if (curFieldName == L"callput")  //Call or Put
				{
					String callOrPutStr = curFieldNode->GetText().Trim();
					if (callOrPutStr.Length() > 0)
					{
						if (callOrPutStr == L"C")
							callOrPut = nsOrderMessageDefine::evtCall;
						else if (callOrPutStr == L"P")
							callOrPut = nsOrderMessageDefine::evtPut;
						else
							callOrPut = nsOrderMessageDefine::evtNone;
					}
					else
						callOrPut = nsOrderMessageDefine::evtNone;
				}
				else if (curFieldName == L"qty")  //未平倉數量
				{
					String quantityStr = curFieldNode->GetText().Trim();
					if (quantityStr.Length() > 0)
						quantity = quantityStr.ToInt();
				}
				else if (curFieldName == L"trdprc1")  //成交價
				{
					String matchPriceStr = curFieldNode->GetText().Trim();
					if (matchPriceStr.Length() > 0)
						matchPrice = matchPriceStr.ToDouble();
				}
				else if (curFieldName == L"spread")  //價差單
				{
					String isSpreadStr = curFieldNode->GetText().Trim();
					if (isSpreadStr.Length() > 0)
					{
						if (isSpreadStr == L"Y")
							isSpread = true;
						else
							isSpread = false;
					}
				}
				else if (curFieldName == L"dtover")  //當日平倉
				{
					String isDatTradeOverStr = curFieldNode->GetText().Trim();
					if (isDatTradeOverStr.Length() > 0)
					{
						if (isDatTradeOverStr == L"Y")
							isDayTradeOver = true;
						else
							isDayTradeOver = false;
					}
				}
				else if (curFieldName == L"comtype")  //交易種類  0:期貨  1:期權
				{
					String marketStr = curFieldNode->GetText().Trim();
					if (marketStr.Length() > 0)
					{
						if (marketStr == L"0")
							market = nsOrderMessageDefine::mTWFutures;
						else if (marketStr == L"1")
							market = nsOrderMessageDefine::mTWOptions;
					}  //if (CommodityTypeStr.Length() > 0)
				}
			}  //if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while(curFieldNode != 0)
		isExceptionHappened = false;
	} catch (EXMLDocError& e) {
		logMsg.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMsg.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"%Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened)
	{
		if (logMsg.Length() > 0)
		{
			WriteUFCLog(logHead, logMsg, constNotFlush);
			SetParseMessage(logMsg);
			if (FOnError != NULL) FOnError(logMsg);
		}
		return false;
	}  //if (isExceptionHappened)

	if ((quantity > 0) && (FAccountPtr != 0))
	{
		gTradingObjsPool.LockCommodity(rwLockForRead);
		CTradingCommodity *commodityPtr = gTradingObjsPool.FindCommodityFromCodeIndex(FExchangeCode, contractCode, rwNotLock);
		if (commodityPtr == 0)
		{
			commodityPtr = gTradingObjsPool.FindCommodityFromBrokerCodeIndex(FExchangeCode, contractCode, rwNotLock);
			if (commodityPtr != 0) contractCode = commodityPtr->GetContractCode(rwLockForRead);
		}
		gTradingObjsPool.UnlockCommodity(rwLockForRead);
		if (commodityPtr == 0)
		{
			logMsg.printf(L"commodity[%s-%s] not Found.", FExchangeCode, contractCode);
			WriteUFCLog(logHead, logMsg, constNotFlush);
			return false;
		}

		String symbol = CTradingContract::GenerateTaifexSymbol(contractCode, strikePrice, maturityYearMonth, callOrPut);
		CTradingContract *contractPtr = gTradingObjsPool.FindContractFromSymbolIndex(FExchangeCode, symbol, rwLockForRead);
		if (contractPtr == 0)
		{
			logMsg.printf(L"contract[%s-%s] not Found.", FExchangeCode, symbol);
			WriteUFCLog(logHead, logMsg, constNotFlush);
			return false;
		}

		String tradeSymbol = contractPtr->GetTradeSymbol(rwLockForRead);
		CHoldPositionData *holdPositionPtr = new CHoldPositionData(FExchangeCode, symbol, market, side, quantity, matchPrice, commodityPtr, FExchangePtr);
		holdPositionPtr->SetTradeSymbol(tradeSymbol, rwNotLock);
		if (orderID.Length() > 0) holdPositionPtr->SetOrderID(orderID, rwNotLock);
		if (tradingDate.Length() > 0) holdPositionPtr->SetTradeDate(tradingDate, rwNotLock);
		holdPositionPtr->SetDayTradeOver(isDayTradeOver, rwNotLock);
		FAccountPtr->AppendTaifexHoldPositionToArray(holdPositionPtr, rwLockForWrite);
	}
	return true;
}  //CTaifexHoldPositionQueryObject::ParseHoldPositionNode()
//---------------------------------------------------------------------------
bool CTaifexHoldPositionQueryObject::ParseResponseXML(_di_IXMLDocument ResponseXML)
{
	bool isSuccess = false;
	String logHead = FQueryName + L"::ParseResponseXML()";
	String logMsg = L"";
	String responseAccount = L"";
	String responseCompany = L"";
	String responseUserKey = L"";
	int processCount = 0;
	int holdPositionCount = 0;
	try
	{
		_di_IXMLNode rootNode = ResponseXML->DocumentElement;
		if (rootNode != 0)
		{	//Parse Account Data
			String curChildNodeName = L"";
			_di_IXMLNode curChildNode = rootNode->ChildNodes->First();
			while (curChildNode != 0)
			{
				if (curChildNode->HasChildNodes && curChildNode->IsTextElement)
				{
					curChildNodeName = curChildNode->GetNodeName().Trim();
					if (curChildNodeName == L"actno")
						responseAccount = curChildNode->GetText().Trim();
					else if (curChildNodeName == L"UK")
						responseUserKey = curChildNode->GetText().Trim();
					else if (curChildNodeName == L"company")
						responseCompany = curChildNode->GetText().Trim();
				}  //if (curChildNode->HasChildNodes && curChildNode->IsTextElement)
				curChildNode = curChildNode->NextSibling();
			}  //while (curChildNode != 0)
			if (responseAccount.Length() <= 0) responseAccount = FAccount;
			if (responseUserKey.Length() <= 0) responseUserKey = FUserKey;

			//Parse Fee Node
			curChildNode = rootNode->ChildNodes->First();
			while (curChildNode != 0)
			{
				if (curChildNode->HasChildNodes)
				{
					curChildNodeName = curChildNode->GetNodeName().Trim();
					if (curChildNodeName == L"fbtopd")
					{
						processCount++;
						if (ParseHoldPositionNode(curChildNode)) holdPositionCount++;
					}
				}  //if (curChildNode->HasChildNodes)
				curChildNode = curChildNode->NextSibling();
			}  //while (curChildNode != 0)
		}  //if (rootNode != 0)
		isSuccess = true;
	} catch (EXMLDocError& e) {
		logMsg.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMsg.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (!isSuccess && (logMsg.Length() > 0))
	{
		SetParseMessage(logMsg);
		WriteUFCLog(logHead, logMsg, constNotFlush);
		if (FOnError != NULL) FOnError(logMsg);
	}  //if (!isSuccess && (logMessage.Length() > 0))

	logMsg.printf(L"Account[%s], UK[%s], Company[%s],Process %d Hold Position Node, Create %d Hold Position Object", responseAccount, responseUserKey, responseCompany, processCount, holdPositionCount);
	WriteUFCLog(logHead, logMsg, constFlush);
	return isSuccess;
}  //CTaifexHoldPositionQueryObject::ParseResponseXML()
//---------------------------------------------------------------------------
void CTaifexHoldPositionQueryObject::SetQueryStatus(QueryStatusEnum QueryStatus)
{
	FQueryStatus = QueryStatus;
	if (FAccountPtr != 0) FAccountPtr->SetTaifexHoldPositionQueryStatus(QueryStatus, rwLockForWrite);
}  //CTaifexHoldPositionQueryObject::SetQueryStatus()
//---------------------------------------------------------------------------
void CTaifexHoldPositionQueryObject::SetQueryMessage(const String& QueryMessage)
{
	FQueryMessage = QueryMessage;
	if (FAccountPtr != 0) FAccountPtr->SetTaifexHoldPositionQueryMessage(QueryMessage, rwLockForWrite);
}  //CTaifexHoldPositionQueryObject::SetQueryMessage()
//---------------------------------------------------------------------------
void CTaifexHoldPositionQueryObject::SetParseMessage(const String& ParseMessage)
{
	FParseMessage = ParseMessage;
	if (FAccountPtr != 0) FAccountPtr->SetTaifexHoldPositionQueryMessage(ParseMessage, rwLockForWrite);
}  //CTaifexHoldPositionQueryObject::SetParseMessage()

//-------------------- COverseasHoldPositionQueryObject ---------------------
//---------------------------------------------------------------------------
COverseasHoldPositionQueryObject::COverseasHoldPositionQueryObject(const String& QueryURL, const String& Account, int ConnectTimeOut, const String& DumpFileName, bool DoDump, bool CreateSuspended)
:CBasicTradingQueryObject(QueryURL, ConnectTimeOut, DumpFileName, DoDump, CreateSuspended),
 FAccount(Account),
 FUserKey(Account),
 FAccountPtr(0)
{
	FQueryName.printf(L"OverseasHoldPosition_%s", FAccount);
	if (DumpFileName.Length() <= 0)
		FDumpFileName.printf(L"Super888_%s_OverseasHoldPosition.xml", FAccount);

	FAccountPtr = gTradingObjsPool.FindAccountFromLogonIdIndex(Account, rwLockForRead);
	if (FAccountPtr != 0)
	{
		FAccountPtr->LockForWrite();
		FAccountPtr->SetOverseasHoldPositionQueryStatus(qsNone, rwNotLock);
		FAccountPtr->SetOverseasHoldPositionQueryMessage(L"", rwNotLock);
		FAccountPtr->ClearOverseasHoldPositionArray(rwNotLock);
		FUserKey = FAccountPtr->GetUserKey(rwNotLock);
		FAccountPtr->UnlockForWrite();
	}
}  //COverseasHoldPositionQueryObject::COverseasHoldPositionQueryObject()
//---------------------------------------------------------------------------
String COverseasHoldPositionQueryObject::CombineQueryURL(String& QueryURL)
{
	String combinedURL = QueryURL + L"?UK=" + FUserKey + L"&flg=Y";  //Y:只要昨日留倉，N:目前留倉
	return combinedURL;
}  //COverseasHoldPositionQueryObject::CombineQueryURL()
//---------------------------------------------------------------------------
bool COverseasHoldPositionQueryObject::ParseHoldPositionNode(_di_IXMLNode HoldPositionNode)
{
	bool   isExceptionHappened = true;
	String logHead = FQueryName + L"::ParseHoldPositionNode()";
	String logMsg = L"";
	String                              orderID = L"";                                //委託單號
	String                              tradingDate = L"";                            //交易日期
	nsOrderMessageDefine::SideEnum      side = nsOrderMessageDefine::sNone;           //買賣別
	String                              exchangeCode = L"";                           //交易所代碼
	String                              tradeContractCode = L"";                      //交易合約代碼
	String                              maturityYearMonth = L"";                      //到期年月
	String                              strikePriceStr = L"";                         //履約價字串
	int                                 quantity = 0;                                 //未平倉數量
	double                              matchPrice = 0.0;                             //成交價
	double                              matchPriceIntegerPart = 0.0;                  //成交價整數部分
	double                              matchNumerator = 0.0;                         //成交價分子
	int                                 matchDenominator = 0;                         //成交價分母
	nsOrderMessageDefine::MarketEnum    market = nsOrderMessageDefine::mForeignOptions;  //市場別
	nsOrderMessageDefine::EventTypeEnum callOrPut = nsOrderMessageDefine::evtNone;    //Call or Put
	try
	{
		_di_IXMLNode curFieldNode = HoldPositionNode->ChildNodes->First();
		while(curFieldNode != 0)
		{
			if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			{
				String curFieldName = curFieldNode->GetNodeName().Trim();
				if (curFieldName == L"ordno")  //委託單號
					orderID = curFieldNode->GetText().Trim();
				else if (curFieldName == L"trddt")  //交易日期
					tradingDate = curFieldNode->GetText().Trim();
				else if (curFieldName == L"ps")  //買賣別
				{
					String buySellStr = curFieldNode->GetText().Trim();
					if (buySellStr.Length() > 0)
					{
						if (buySellStr == L"B")
							side = nsOrderMessageDefine::sBuy;
						else if (buySellStr == L"S")
							side = nsOrderMessageDefine::sSell;
					}
				}
				else if (curFieldName == L"exh")
				{
					String origExchangeCode = curFieldNode->GetText().Trim();
					exchangeCode = origExchangeCode.Trim();
				}
				else if (curFieldName == L"comno")  //合約代碼
					tradeContractCode = curFieldNode->GetText().Trim();
				else if (curFieldName == L"comym")  //到期年月
					maturityYearMonth = curFieldNode->GetText().Trim();
				else if (curFieldName == L"stkprice")  //履約價
					strikePriceStr = curFieldNode->GetText().Trim();
				else if (curFieldName == L"cp")  //Call or Put(買賣權 C-Call, P-PUT, N-期貨)
				{
					String callOrPutStr = curFieldNode->GetText().Trim();
					if (callOrPutStr.Length() > 0)
					{
						if (callOrPutStr == L"C")
							callOrPut = nsOrderMessageDefine::evtCall;
						else if (callOrPutStr == L"P")
							callOrPut = nsOrderMessageDefine::evtPut;
						else
						{
							callOrPut = nsOrderMessageDefine::evtNone;
							market = nsOrderMessageDefine::mForeignFutures;
						}
					}
					else
					{
						callOrPut = nsOrderMessageDefine::evtNone;
						market = nsOrderMessageDefine::mForeignFutures;
					}
				}
				else if (curFieldName == L"qty")  //未平倉數量
				{
					String quantityStr = curFieldNode->GetText().Trim();
					if (quantityStr.Length() > 0)
						quantity = quantityStr.ToInt();
				}
				else if (curFieldName == L"trdprc1")  //成交價
				{
					String matchPriceStr = curFieldNode->GetText().Trim();
					if (matchPriceStr.Length() > 0)
						matchPrice = matchPriceStr.ToDouble();
				}
				else if (curFieldName == L"trdpc1")  //成交價整數部分
				{
					String matchPriceIntegerPartStr = curFieldNode->GetText().Trim();
					if (matchPriceIntegerPartStr.Length() > 0)
						matchPriceIntegerPart = matchPriceIntegerPartStr.ToDouble();
				}
				else if (curFieldName == L"trdpc2")  //成交價分子
				{
					String matchNumeratorStr = curFieldNode->GetText().Trim();
					if (matchNumeratorStr.Length() > 0)
						matchNumerator = matchNumeratorStr.ToDouble();
				}
				else if (curFieldName == L"trdpc3")  //成交價分母
				{
					String matchDenominatorStr = curFieldNode->GetText().Trim();
					if (matchDenominatorStr.Length() > 0)
						matchDenominator = matchDenominatorStr.ToInt();
				}
			}  //if (curFieldNode->HasChildNodes && curFieldNode->IsTextElement)
			curFieldNode = curFieldNode->NextSibling();
		}  //while(curFieldNode != 0)
		isExceptionHappened = false;
	} catch (EXMLDocError& e) {
		logMsg.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMsg.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (isExceptionHappened)
	{
		if (logMsg.Length() > 0)
		{
			SetParseMessage(logMsg);
			WriteUFCLog(logHead, logMsg, constFlush);
			if (FOnError != NULL) FOnError(logMsg);
		}
		return false;
	}  //if (isExceptionHappened)

	if ((quantity > 0) && (FAccountPtr != 0))
	{
		gTradingObjsPool.LockExchange(rwLockForRead);
		CTradingExchange *exchangePtr = gTradingObjsPool.FindExchangeFromCodeIndex(exchangeCode, rwNotLock);
		if (exchangePtr == 0)
		{
			exchangePtr = gTradingObjsPool.FindExchangeFromBrokerCodeIndex(exchangeCode, rwNotLock);
			if (exchangePtr != 0) exchangeCode = exchangePtr->GetExchangeCode(rwLockForRead);
		}
		gTradingObjsPool.UnlockExchange(rwLockForRead);
		if (exchangePtr == 0)
		{
			logMsg.printf(L"exchange[%s] not Found.", exchangeCode);
			WriteUFCLog(logHead, logMsg, constFlush);
			return false;
		}

		String tradeSymbol = CTradingContract::GenerateOverseasTradeSymbol(tradeContractCode, strikePriceStr, maturityYearMonth, callOrPut);
		CTradingContract *contractPtr = gTradingObjsPool.FindContractFromTradeSymbolIndex(exchangeCode, tradeSymbol, rwLockForRead);
		if (contractPtr == 0)
		{
			logMsg.printf(L"contract[%s-%s] not Found in TradeSymbolIndex.", exchangeCode, tradeSymbol);
			WriteUFCLog(logHead, logMsg, constFlush);
			return false;
		}
		String symbol = contractPtr->GetSymbol(rwLockForRead);

		if ((matchNumerator > 0.0) && (matchDenominator > 0))  //分子,分母都大於零
			matchPrice = matchPriceIntegerPart + (matchNumerator / static_cast<double>(matchDenominator));
		CTradingCommodity *commodityPtr = contractPtr->GetCommodity(rwLockForRead);
		if (commodityPtr != 0) matchPrice *= commodityPtr->GetContractMultiplier(rwLockForRead);

		if (FAccountPtr != 0)
		{
			CHoldPositionData *holdPositionPtr = new CHoldPositionData(exchangeCode, symbol, market, side, quantity, matchPrice, commodityPtr, exchangePtr);
			holdPositionPtr->SetTradeSymbol(tradeSymbol, rwNotLock);
			if (orderID.Length() > 0) holdPositionPtr->SetOrderID(orderID, rwNotLock);
			if (tradingDate.Length() > 0) holdPositionPtr->SetTradeDate(tradingDate, rwNotLock);
			FAccountPtr->AppendOverseasHoldPositionToArray(holdPositionPtr, rwLockForWrite);
		}
	}  //if ((quantity > 0) && (FAccountPtr != 0))
	return true;
}  //COverseasHoldPositionQueryObject::ParseHoldPositionNode()
//---------------------------------------------------------------------------
bool COverseasHoldPositionQueryObject::ParseResponseXML(_di_IXMLDocument ResponseXML)
{
	bool isSuccess = false;
	String logHead = FQueryName + L"::ParseResponseXML()";
	String logMsg = L"";
	String responseAccount = L"";
	String responseCompany = L"";
	String responseUserKey = L"";
	String calculateTime = L"";
	int processCount = 0;
	int holdPositionCount = 0;
	try
	{
		_di_IXMLNode rootNode = ResponseXML->DocumentElement;
		if (rootNode != 0)
		{	//Parse Account Data
			String curChildNodeName = L"";
			_di_IXMLNode curChildNode = rootNode->ChildNodes->First();
			while (curChildNode != 0)
			{
				if (curChildNode->HasChildNodes && curChildNode->IsTextElement)
				{
					curChildNodeName = curChildNode->GetNodeName().Trim();
					if (curChildNodeName == L"actno")
						responseAccount = curChildNode->GetText().Trim();
					else if (curChildNodeName == L"UK")
						responseUserKey = curChildNode->GetText().Trim();
					else if (curChildNodeName == L"company")
						responseCompany = curChildNode->GetText().Trim();
					else if (curChildNodeName == L"caltime")
						calculateTime = curChildNode->GetText().Trim();
				}  //if (curChildNode->HasChildNodes && curChildNode->IsTextElement)
				curChildNode = curChildNode->NextSibling();
			}  //while (curChildNode != 0)
			if (responseAccount.Length() <= 0) responseAccount = FAccount;
			if (responseUserKey.Length() <= 0) responseUserKey = FUserKey;

			//Parse Fee Node
			curChildNode = rootNode->ChildNodes->First();
			while (curChildNode != 0)
			{
				if (curChildNode->HasChildNodes)
				{
					curChildNodeName = curChildNode->GetNodeName().Trim();
					if (curChildNodeName == L"fowopt")
					{
						processCount++;
						if (ParseHoldPositionNode(curChildNode)) holdPositionCount++;
					}
				}  //if (curChildNode->HasChildNodes)
				curChildNode = curChildNode->NextSibling();
			}  //while (curChildNode != 0)
		}  //if (rootNode != 0)
		isSuccess = true;
	} catch (EXMLDocError& e) {
		logMsg.printf(L"EXMLDocError Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (Exception& e) {
		logMsg.printf(L"Exception:%s. URL=[%s]", e.Message, FQueryURL);
	} catch (...) {
		logMsg.printf(L"Unknown Exception. URL=[%s]", FQueryURL);
	}  //try

	if (!isSuccess && (logMsg.Length() > 0))
	{
		SetParseMessage(logMsg);
		WriteUFCLog(logHead, logMsg, constNotFlush);
		if (FOnError != NULL) FOnError(logMsg);
	}  //if (!isSuccess && (logMessage.Length() > 0))

	logMsg.printf(L"Account[%s], UK[%s], Company[%s], CalculateTime[%s] Process %d Hold Position Node, Create %d Hold Position Object", responseAccount, responseUserKey, responseCompany, calculateTime, processCount, holdPositionCount);
	WriteUFCLog(logHead, logMsg, constFlush);
	return isSuccess;
}  //COverseasHoldPositionQueryObject::ParseResponseXML()
//---------------------------------------------------------------------------
void COverseasHoldPositionQueryObject::SetQueryStatus(QueryStatusEnum QueryStatus)
{
	FQueryStatus = QueryStatus;
	if (FAccountPtr != 0) FAccountPtr->SetOverseasHoldPositionQueryStatus(QueryStatus, rwLockForWrite);
}  //COverseasHoldPositionQueryObject::SetQueryStatus()
//---------------------------------------------------------------------------
void COverseasHoldPositionQueryObject::SetQueryMessage(const String& QueryMessage)
{
	FQueryMessage = QueryMessage;
	if (FAccountPtr != 0) FAccountPtr->SetOverseasHoldPositionQueryMessage(QueryMessage, rwLockForWrite);
}  //COverseasHoldPositionQueryObject::SetQueryMessage()
//---------------------------------------------------------------------------
void COverseasHoldPositionQueryObject::SetParseMessage(const String& ParseMessage)
{
	FParseMessage = ParseMessage;
	if (FAccountPtr != 0) FAccountPtr->SetOverseasHoldPositionQueryMessage(ParseMessage, rwLockForWrite);
}  //COverseasHoldPositionQueryObject::SetParseMessage()

//---------------------- CWaitQueryHoldPositionObject -----------------------
//---------------------------------------------------------------------------
CWaitQueryHoldPositionObject::CWaitQueryHoldPositionObject(const String& Account, int OrderStroeIndex, bool CreateSuspended)
:TThread(CreateSuspended),
 FAccount(Account),
 FOrderStroeIndex(OrderStroeIndex),
 FAccountPtr(0)
{
	FAccountPtr = gTradingObjsPool.FindAccountFromLogonIdIndex(FAccount, rwLockForRead);
}  //CWaitQueryHoldPositionObject::CWaitQueryHoldPositionObject()
//---------------------------------------------------------------------------
void __fastcall CWaitQueryHoldPositionObject::Execute(void)
{
	if (FAccountPtr == 0) return;
	FAccountPtr->LockForRead();
	QueryStatusEnum taifexHoldPositionQueryStatus = FAccountPtr->GetTaifexHoldPositionQueryStatus(rwNotLock);
	QueryStatusEnum overseasHoldPositionQueryStatus = FAccountPtr->GetOverseasHoldPositionQueryStatus(rwNotLock);
	FAccountPtr->UnlockForRead();
	bool isTaifexHoldPositionQueried = ((taifexHoldPositionQueryStatus == qsFinished) || (taifexHoldPositionQueryStatus == qsFailed));
	bool isOverseasHoldPositionQueried = ((overseasHoldPositionQueryStatus == qsFinished) || (overseasHoldPositionQueryStatus == qsFailed));

	while (!isTaifexHoldPositionQueried || !isOverseasHoldPositionQueried)
	{
		UFC::SleepMS(500);
		FAccountPtr->LockForRead();
		taifexHoldPositionQueryStatus = FAccountPtr->GetTaifexHoldPositionQueryStatus(rwNotLock);
		overseasHoldPositionQueryStatus = FAccountPtr->GetOverseasHoldPositionQueryStatus(rwNotLock);
		FAccountPtr->UnlockForRead();
		isTaifexHoldPositionQueried = ((taifexHoldPositionQueryStatus == qsFinished) || (taifexHoldPositionQueryStatus == qsFailed));
		isOverseasHoldPositionQueried = ((overseasHoldPositionQueryStatus == qsFinished) || (overseasHoldPositionQueryStatus == qsFailed));
	}  //while (!isTaifexHoldPositionQueried || !isOverseasHoldPositionQueried)

	String logMessage = L"";
	logMessage.printf(L"CWaitQueryHoldPositionObject::Execute() Account[%s] OrderStore[%d] Query Open Interest Terminate.", FAccount, FOrderStroeIndex);
	WriteUFCLog(logMessage, constFlush);
	UFC::BufferedLog::Printf(" %s", ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
}  //CWaitQueryHoldPositionObject::Execute()
};  //namespace s888
