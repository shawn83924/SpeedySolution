//---------------------------------------------------------------------------

#ifndef TradingQueriesH
#define TradingQueriesH
#include "System.hpp"
#include "Classes.hpp"
#include "IdHTTP.hpp"
#include "IdSSL.hpp"
#include "IdSSLOpenSSL.hpp"
#include  <Xml.XMLDoc.hpp>
#include "TradingCommodities.h"
#include "TradingInvestors.h"
//---------------------------------------------------------------------------
namespace s888
{
//----------------------------------------------------------------------------
class CBasicTradingQueryObject : public TThread
{
public:
	typedef void __fastcall (__closure *TOnQueryThreadError)(const String& ErrorMessage);

private:

protected:
	String              FQueryName;
	String              FQueryURL;
	int                 FConnectTimeOut;  //MilliSecond
	String              FDumpFileName;
	bool                FDoDump;
	bool                FIsTestMode;
	TOnQueryThreadError FOnError;
	QueryStatusEnum     FQueryStatus;
	String              FQueryMessage;
	String              FParseMessage;

	TIdHTTP                      *FHttpPtr;
	TIdSSLIOHandlerSocketOpenSSL *FSSLIOHandlerPtr;

	virtual String CombineQueryURL(String& QueryURL);
	virtual bool PrepareHttpObject(const String& QueryURL, int ConnectTimeOut);
	virtual bool SendQueryURL(TIdHTTP *HttpPtr, const String& QueryURL, TStringStream *QueryResponse);
	virtual _di_IXMLDocument CreateXMLDocument(TStringStream *DataSourcePtr);
	virtual bool ParseResponseXML(_di_IXMLDocument ResponseXML);

public:
	CBasicTradingQueryObject(const String& QueryURL, int ConnectTimeOut = 90000, const String& DumpFileName = L"", bool DoDump = false, bool CreateSuspended = true);

	String GetQueryName() {return FQueryName;};
	void SetQueryName(const String& QueryName) {FQueryName = QueryName;};
	String GetQueryURL() {return FQueryURL;};
	void SetQueryURL(const String& QueryURL) {FQueryURL = QueryURL;};
	int GetConnectTimeOut() {return FConnectTimeOut;};
	void SetConnectTimeOut(int ConnectTimeOut) {FConnectTimeOut = ConnectTimeOut;};
	String GetDumpFileName() {return FDumpFileName;};
	void SetDumpFileName(const String&  DumpFileName) {FDumpFileName = DumpFileName;};
	bool IsTestMode() {return FIsTestMode;};
	void SetTestMode(bool IsTestMode) {FIsTestMode = IsTestMode;};
	bool GetDoDump() {return FDoDump;};
	void SetDoDump(bool DoDump) {FDoDump = DoDump;};

	QueryStatusEnum GetQueryStatus() {return FQueryStatus;};
	virtual void SetQueryStatus(QueryStatusEnum QueryStatus) {FQueryStatus = QueryStatus;};
	String GetQueryMessage() {return FQueryMessage;};
	virtual void SetQueryMessage(const String& QueryMessage) {FQueryMessage = QueryMessage;};
	String GetParseMessage() {return FParseMessage;};
	virtual void SetParseMessage(const String& ParseMessage) {FParseMessage = ParseMessage;};

	__property TOnQueryThreadError OnError = {read = FOnError, write = FOnError};

	virtual void __fastcall Execute(void);
};  //CBasicTradingQueryObject
//----------------------------------------------------------------------------
class CTaifexContractQueryObject : public CBasicTradingQueryObject
{
protected:
	static String                           FExchangeCode;

	nsOrderMessageDefine::MarketEnum FMarket;
	CTradingExchange *FExchangePtr;
	int               FCommodityCount;
	int               FContractCount;

	virtual void ParseContractNode(CTradingCommodity *CommodityPtr, _di_IXMLNode ContractNode) {};
	virtual void ParseCommodityNode(_di_IXMLNode CommodityNode);
	virtual bool ParseResponseXML(_di_IXMLDocument ResponseXML);

public:
	CTaifexContractQueryObject(const String& QueryURL, int ConnectTimeOut = 90000, const String& DumpFileName = L"", bool DoDump = false, bool CreateSuspended = true);
};  //CTaifexContractQueryObject
//----------------------------------------------------------------------------
class CTaifexFuturesContractQueryObject : public CTaifexContractQueryObject
{
protected:
	void ParseContractNode(CTradingCommodity *CommodityPtr, _di_IXMLNode ContractNode);

public:
	virtual void SetQueryStatus(QueryStatusEnum QueryStatus);
	virtual void SetQueryMessage(const String& QueryMessage);
	virtual void SetParseMessage(const String& ParseMessage);
	CTaifexFuturesContractQueryObject(const String& QueryURL, int ConnectTimeOut = 90000, const String& DumpFileName = L"", bool DoDump = false, bool CreateSuspended = true);
};  //CTaifexFuturesContractQueryObject()
//----------------------------------------------------------------------------
class CTaifexOptionsContractQueryObject : public CTaifexContractQueryObject
{
protected:
	void ParseStrikePriceNode(CTradingCommodity *CommodityPtr,
							  const String& ContractCode,
							  const String& MaturityYearMonth,
							  nsOrderMessageDefine::EventTypeEnum CallOrPut,
							  _di_IXMLNode StrikePriceNode);
	void ParseContractNode(CTradingCommodity *CommodityPtr, _di_IXMLNode ContractNode);

public:
	virtual void SetQueryStatus(QueryStatusEnum QueryStatus);
	virtual void SetQueryMessage(const String& QueryMessage);
	virtual void SetParseMessage(const String& ParseMessage);
	CTaifexOptionsContractQueryObject(const String& QueryURL, int ConnectTimeOut = 90000, const String& DumpFileName = L"", bool DoDump = false, bool CreateSuspended = true);
};  //CTaifexOptionsContractQueryObject()
//----------------------------------------------------------------------------
class COverseasContractQueryObject : public CBasicTradingQueryObject
{
protected:
	int FExchangeCount;
	int FCommodityCount;
	int FContractCount;

	void ParseContractNode(CTradingExchange *ExchangePtr, CTradingCommodity *CommodityPtr,
						   nsOrderMessageDefine::MarketEnum Market,
						   nsOrderMessageDefine::EventTypeEnum CallPut,
						   _di_IXMLNode ContractNode);
	void ParseCommodityNode(_di_IXMLNode CommodityNode);
	void ParseExchangeNode(_di_IXMLNode ExchangeNode);
	virtual bool ParseResponseXML(_di_IXMLDocument ResponseXML);

public:
	virtual void SetQueryStatus(QueryStatusEnum QueryStatus);
	virtual void SetQueryMessage(const String& QueryMessage);
	virtual void SetParseMessage(const String& ParseMessage);
	COverseasContractQueryObject(const String& QueryURL, int ConnectTimeOut = 90000, const String& DumpFileName = L"", bool DoDump = false, bool CreateSuspended = true);
};  //COverseasContractQueryObject()
//----------------------------------------------------------------------------
class CTaifexTaxRateQueryObject : public CBasicTradingQueryObject
{
protected:
	static String FExchangeCode;
protected:
	int FTaxRateCount;
	void ParseTaxRateNode(_di_IXMLNode RateNode);
	virtual bool ParseResponseXML(_di_IXMLDocument ResponseXML );
public:
	virtual void SetQueryStatus(QueryStatusEnum QueryStatus);
	virtual void SetQueryMessage(const String& QueryMessage);
	virtual void SetParseMessage(const String& ParseMessage);
	CTaifexTaxRateQueryObject(const String& QueryURL, int ConnectTimeOut , const String& DumpFileName = L"", bool DoDump = false, bool CreateSuspended = true);
	CTaifexTaxRateQueryObject(const String& FileName );
};  //CTaifexTaxRateQueryObject
//----------------------------------------------------------------------------
class CTaifexFeeQueryObject : public CBasicTradingQueryObject
{
protected:
	static String FExchangeCode;

protected:
	String          FLogonID;
	String          FBrokerID;
	CTradingAccount *FAccountPtr;
	virtual String CombineQueryURL(String& QueryURL);
	void ParseFeeNode(_di_IXMLNode FeeNode);
	virtual bool ParseResponseXML(_di_IXMLDocument ResponseXML );
    nsOrderMessageDefine::MarketEnum FindMarket( const String& ProdID );
public:
	CTaifexFeeQueryObject(const String& QueryURL, const String& LogonID, const String& BrokerID, int ConnectTimeOut = 90000, const String& DumpFileName = L"", bool DoDump = false, bool CreateSuspended = true);
	CTaifexFeeQueryObject(const String& FileName, const String& LogonID );
	virtual void SetQueryStatus(QueryStatusEnum QueryStatus);
	virtual void SetQueryMessage(const String& QueryMessage);
	virtual void SetParseMessage(const String& ParseMessage);
};  //CTaifexFeeQueryObject
//----------------------------------------------------------------------------
class COverseasFeeQueryObject : public CBasicTradingQueryObject
{
protected:
	String          FLogonID;
	String          FUserKey;
	CTradingAccount *FAccountPtr;

	virtual String CombineQueryURL(String& QueryURL);
	bool ParseFeeNode(_di_IXMLNode FeeNode);
	virtual bool ParseResponseXML(_di_IXMLDocument ResponseXML);

public:
	COverseasFeeQueryObject(const String& QueryURL, const String& LogonID, int ConnectTimeOut = 90000, const String& DumpFileName = L"", bool DoDump = false, bool CreateSuspended = true);

	virtual void SetQueryStatus(QueryStatusEnum QueryStatus);
	virtual void SetQueryMessage(const String& QueryMessage);
	virtual void SetParseMessage(const String& ParseMessage);
};  //COverseasFeeQueryObject

//----------------------------------------------------------------------------
class CWaitQueryFeeObject : public TThread
{
protected:
	String          FLogonID;
	bool            FIsTestMode;
	bool            FWaitQueryFee;  //中國部分不查詢費率，不必等
	CTradingAccount *FAccountPtr;

public:
	CWaitQueryFeeObject(const String& LogonID, bool WaitQueryFee, bool CreateSuspended = true);
	void SetTestMode(bool IsTestMode) {FIsTestMode = IsTestMode;};
	virtual void __fastcall Execute(void);
};  //CWaitQueryFeeObject

//----------------------------------------------------------------------------
class CTaifexHoldPositionQueryObject : public CBasicTradingQueryObject
{
protected:
	static String FExchangeCode;

protected:
	String          FAccount;
	String          FUserKey;
	CTradingAccount *FAccountPtr;
	CTradingExchange *FExchangePtr;

	virtual String CombineQueryURL(String& QueryURL);
	bool ParseHoldPositionNode(_di_IXMLNode HoldPositionNode);
	virtual bool ParseResponseXML(_di_IXMLDocument ResponseXML);

public:
	CTaifexHoldPositionQueryObject(const String& QueryURL, const String& Account, int ConnectTimeOut = 90000, const String& DumpFileName = L"", bool DoDump = false, bool CreateSuspended = true);

	virtual void SetQueryStatus(QueryStatusEnum QueryStatus);
	virtual void SetQueryMessage(const String& QueryMessage);
	virtual void SetParseMessage(const String& ParseMessage);
};  //CTaifexHoldPositionQueryObject

//----------------------------------------------------------------------------
class COverseasHoldPositionQueryObject : public CBasicTradingQueryObject
{
protected:
	String          FAccount;
	String          FUserKey;
	CTradingAccount *FAccountPtr;

	virtual String CombineQueryURL(String& QueryURL);
	bool ParseHoldPositionNode(_di_IXMLNode HoldPositionNode);
	virtual bool ParseResponseXML(_di_IXMLDocument ResponseXML);

public:
	COverseasHoldPositionQueryObject(const String& QueryURL, const String& Account, int ConnectTimeOut = 90000, const String& DumpFileName = L"", bool DoDump = false, bool CreateSuspended = true);

	virtual void SetQueryStatus(QueryStatusEnum QueryStatus);
	virtual void SetQueryMessage(const String& QueryMessage);
	virtual void SetParseMessage(const String& ParseMessage);
};  //COverseasHoldPositionQueryObject

//----------------------------------------------------------------------------
class CWaitQueryHoldPositionObject : public TThread
{
protected:
	String          FAccount;
	int             FOrderStroeIndex;
	bool            FWaitQueryHoldPosition;
	CTradingAccount *FAccountPtr;

public:
	CWaitQueryHoldPositionObject(const String& Account, int OrderStroeIndex, bool CreateSuspended = true);
	virtual void __fastcall Execute(void);

	String GetAccount() {return FAccount;};
	int    GetOrderStroeIndex() {return FOrderStroeIndex;};
};  //CWaitQueryHoldPositionObject

};  //namespace s888
#endif
