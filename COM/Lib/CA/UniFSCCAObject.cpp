//---------------------------------------------------------------------------

#pragma hdrstop

#include "UniFSCCAObject.h"
#include "../OrderMessage/TNewOrderMessage.h"
#include "../OrderMessage/TCancelOrderMessage.h"
#include "../OrderMessage/TReplaceOrderMessage.h"

#ifdef WIN32
#include "IFSCATLAPIImpl.h"
#endif  //#ifdef WIN32

//----------------------------- CUniFSCCAObject -----------------------------
//---------------------------------------------------------------------------
CUniFSCCAObject::CUniFSCCAObject(const UFC::AnsiString& CommonName, UFC::BufferedLog* LogPtr)
:CApiCADllObject(caFSC, CommonName, LogPtr)
#ifdef WIN32
,FFSCObjPtr(0)
#endif  //#ifdef WIN32
{
	if (CreateActiveXCAObject())
	{
		FIsWorking = true;
		FSignerCertFilter.Printf("OU=PSCNET\nC=TW\nCN=%s", FCommonName.c_str());
	}
}  //CUniFSCCAObject::CUniFSCCAObject()
//---------------------------------------------------------------------------
bool CUniFSCCAObject::CreateActiveXCAObject()
{
	bool isSuccess = false;
#ifdef WIN32
#if !defined(_WIN64)
   if (IFSCATLAPIImpl::CreateInstance(&FFSCObjPtr))
	{
		FLogPtr->fprintf("%s() Create FSC Object Success.", __func__);
		isSuccess = true;
	}
	else
		FLogPtr->fprintf("%s() Create FSC Object Failed.", __func__);
#else
	FLogPtr->fprintf("%s() FSC CA Object not Support 64 bit Program.", __func__);
#endif //#if !defined(_WIN64)
#endif  //#ifdef WIN32
	return isSuccess;
}  //CUniFSCCAObject::CreateActiveXCAObject()
//---------------------------------------------------------------------------
int CUniFSCCAObject::GenerateSignatureAndCAData(const UFC::AnsiString& PlainText, CAResultData& ResultData)
{
	int resultCode = -1;
	UFC::AnsiString csMsg;
#ifdef WIN32
#if !defined(_WIN64)
	if (FFSCObjPtr == 0)
	{
		ResultData.SetErrorCode(resultCode);
		ResultData.SetResultMsg("FSC CA Object is NULL.");
		return resultCode;
	}

	LockForWrite();
	if (FIsWorking)
	{
		char* signatureBuff = 0;
		long signatureErrCode = FFSCObjPtr->SignEx(PlainText.c_str(), FSignerCertFilter.c_str(), "", "", FSCAPI_FLAG_SUBJECT_PARTIALMATCH, 0, &signatureBuff);
		if (signatureErrCode == 0)
		{
			ResultData.SetSignature(UFC::AnsiString(signatureBuff));
			FFSCObjPtr->FreeAllocString(signatureBuff);

			char* certBuff = 0;
			long certErrCode = FFSCObjPtr->XGetUserCertificate("", FSignerCertFilter.c_str(), FSCAPI_FLAG_SUBJECT_PARTIALMATCH, 0, &certBuff);
			if (certErrCode == 0)
			{
				char* subjectBuff = 0;
				long subjectErrCode = FFSCObjPtr->CertGetSubject(certBuff, 0, &subjectBuff);
				if (subjectErrCode == 0)
				{
					ResultData.SetSubject(UFC::AnsiString(subjectBuff));
					FFSCObjPtr->FreeAllocString(subjectBuff);
				}
				else
					FLogPtr->fprintf(" %s() Get Subject failed: %s.", __func__, FFSCObjPtr->GetErrorMsg());

				char* notBeforeBuff = 0;
				long notBeforeErrCode = FFSCObjPtr->CertGetNotBefore(certBuff, 0, &notBeforeBuff);
				if (notBeforeErrCode == 0)
				{
					ResultData.SetNotBefore(UFC::AnsiString(notBeforeBuff));
					FFSCObjPtr->FreeAllocString(notBeforeBuff);
				}
				else
					FLogPtr->fprintf(" %s() Get NotBefore failed: %s.", __func__, FFSCObjPtr->GetErrorMsg());

				char* notAfterBuff = 0;
				long notAfterErrCode = FFSCObjPtr->CertGetNotAfter(certBuff, 0, &notAfterBuff);
				if (notAfterErrCode == 0)
				{
					ResultData.SetNotAfter(UFC::AnsiString(notAfterBuff));
					FFSCObjPtr->FreeAllocString(notAfterBuff);
				}
				else
					FLogPtr->fprintf(" %s() Get NotAfter failed: %s.", __func__, FFSCObjPtr->GetErrorMsg());

				char* serialNumberBuff = 0;
				long serialNumberErrCode = FFSCObjPtr->CertGetSerialNumber(certBuff, 0, &serialNumberBuff);
				if (serialNumberErrCode == 0)
				{
					ResultData.SetSerialNumber(UFC::AnsiString(serialNumberBuff));
					FFSCObjPtr->FreeAllocString(serialNumberBuff);
				}
				else
					FLogPtr->fprintf(" %s() Get SerialNumber failed: %s.", __func__, FFSCObjPtr->GetErrorMsg());

				FFSCObjPtr->FreeAllocString(certBuff);
			}
			else
			{
				FLogPtr->fprintf(" %s() Get Cert failed: %s.", __func__, FFSCObjPtr->GetErrorMsg());
				ResultData.SetResultMsg(UFC::AnsiString(FFSCObjPtr->GetErrorMsg()));
			}

			resultCode = certErrCode;
		}
		else
		{
			ResultData.SetResultMsg(UFC::AnsiString(FFSCObjPtr->GetErrorMsg()));
			resultCode = signatureErrCode;
		}
	}
	else
		ResultData.SetResultMsg("FSCA Object is not working.");

	UnLockForWrite();
#else
	ResultData.SetResultMsg("FSC CA Object not Support 64 bit Program.");
#endif  //#if !defined(_WIN64)
#else
	ResultData.SetResultMsg("FSC CA Object not Support none Windows Program.");
#endif  //#ifdef WIN32
	ResultData.SetErrorCode(resultCode);
	FLogPtr->fprintf(" %s() %s", __func__, ResultData.GetResultMsg().c_str());

	return resultCode;
}  //CUniFSCCAObject::GenerateSignatureAndCAData()
//---------------------------------------------------------------------------
int CUniFSCCAObject::GenerateLogonSignatureAndCAData(const UFC::AnsiString& LogonID, CAResultData& ResultData)
{
	UFC::UDateTime now;
	UFC::PStringBuffer caPlainBuffer;
	if (FCommonName.Length() > 0)
		caPlainBuffer.Printf("%s%02d%02d%02d%03d",
							 FCommonName.c_str(), now.getHour(), now.getMinute(), now.getSecond(), now.getMillisecond());
	UFC::AnsiString plainText = caPlainBuffer.ToString();
	ResultData.SetPlainText(plainText);
	return GenerateSignatureAndCAData(plainText, ResultData);
}  //CUniFSCCAObject::GenerateLogonSignatureAndCAData()
//---------------------------------------------------------------------------
int CUniFSCCAObject::GenerateReqMsgSignatureAndCAData(TBaseMessage* ReqPtr, const UFC::AnsiString& ReqTelegram, CAResultData& ResultData)
{
	UFC::PStringBuffer caPlainBuffer;
	if (FCommonName.Length() > 0) caPlainBuffer.Printf("%s", FCommonName.c_str());

	if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.AppendPrintf("NID=%010d", ReqPtr->GetNID());

	UFC::AnsiString brokerID(ReqPtr->GetBrokerID());
	if (brokerID.Length() <= 0) brokerID = ResultData.GetDefaultBrokerID();
	if (brokerID.Length() > 0)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.AppendPrintf("BrokerID=%s", brokerID.c_str());
	}

	UFC::AnsiString accountNo(ReqPtr->GetAccount());
	if (accountNo.Length() > 0)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.AppendPrintf("acc=%s", accountNo.c_str());
	}

	nsOrderMessageDefine::MarketEnum market = ReqPtr->GetMarket();
	nsOrderMessageDefine::MessageTypeEnum messageType = ReqPtr->GetMessageType();
	nsOrderMessageDefine::TradingSessionIDEnum tradingSessionID = nsOrderMessageDefine::tsAuto;
	nsOrderMessageDefine::OrderTypeEnum orderType = nsOrderMessageDefine::otNone;
	long qty = 0;
	double price = 0.0;
	if (messageType == nsOrderMessageDefine::mtNew)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.Append("trade=I");
		if ((market == nsOrderMessageDefine::mTSE) || (market == nsOrderMessageDefine::mOTC))
			tradingSessionID = ((TNewOrderMessage*)ReqPtr)->GetTradingSessionID();
		orderType = ((TNewOrderMessage*)ReqPtr)->GetOrderType();
		qty = ((TNewOrderMessage*)ReqPtr)->GetOrderQty();
		price = ((TNewOrderMessage*)ReqPtr)->GetPrice();
	}
	else if (messageType == nsOrderMessageDefine::mtReplace)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		qty = ((TReplaceOrderMessage*)ReqPtr)->GetOrderQty();
		if (qty > 0)
			caPlainBuffer.Append("trade=C");  //Reduce Qty
		else
			caPlainBuffer.Append("trade=P");  //Replace Price

		if ((market == nsOrderMessageDefine::mTSE) || (market == nsOrderMessageDefine::mOTC))
			tradingSessionID = ((TReplaceOrderMessage*)ReqPtr)->GetTradingSessionID();
		orderType = ((TReplaceOrderMessage*)ReqPtr)->GetOrderType();
		price = ((TReplaceOrderMessage*)ReqPtr)->GetPrice();
	}
	else if (messageType == nsOrderMessageDefine::mtCancel)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.Append("trade=D");
		if ((market == nsOrderMessageDefine::mTSE) || (market == nsOrderMessageDefine::mOTC))
			tradingSessionID = ((TCancelOrderMessage*)ReqPtr)->GetTradingSessionID();
		orderType = ((TCancelOrderMessage*)ReqPtr)->GetOrderType();
		qty = ((TCancelOrderMessage*)ReqPtr)->GetOrderQty();
		price = ((TCancelOrderMessage*)ReqPtr)->GetPrice();
	}
	else if (messageType == nsOrderMessageDefine::mtReplacePx)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.Append("trade=P");
		if ((market == nsOrderMessageDefine::mTSE) || (market == nsOrderMessageDefine::mOTC))
			tradingSessionID = ((TReplaceOrderMessage*)ReqPtr)->GetTradingSessionID();
		orderType = ((TReplaceOrderMessage*)ReqPtr)->GetOrderType();
		qty = ((TReplaceOrderMessage*)ReqPtr)->GetOrderQty();
		price = ((TReplaceOrderMessage*)ReqPtr)->GetPrice();
	}

	if (tradingSessionID == nsOrderMessageDefine::tsNormal)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.Append("Round=1");
	}
	else if (tradingSessionID == nsOrderMessageDefine::tsOddLot)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.Append("Round=3");
	}
	else if (tradingSessionID == nsOrderMessageDefine::tsOffHour)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.Append("Round=2");
	}
	else if (tradingSessionID == nsOrderMessageDefine::tsIntradayOdd)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.Append("Round=5");
	}
	else
		caPlainBuffer.Append("9");

	UFC::AnsiString symbol(ReqPtr->GetSymbol());
	if (symbol.Length() > 0)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.AppendPrintf("symbol=%s", symbol.c_str());
	}

	if (orderType == nsOrderMessageDefine::otMarket)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.Append("OdrType=1");
	}
	else if (orderType == nsOrderMessageDefine::otLimit)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.Append("OdrType=2");
	}
	else if (orderType == nsOrderMessageDefine::otStop)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.Append("OdrType=4");
	}
	else if (orderType == nsOrderMessageDefine::otStopLimit)
	{
		if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
		caPlainBuffer.Append("OdrType=5");
	}

	if (caPlainBuffer.Length() > 0) caPlainBuffer.Append('|');
	caPlainBuffer.AppendPrintf("Qty=%d", qty);
	caPlainBuffer.AppendPrintf("|Price=%020.9lf", price);

	UFC::UDateTime now;
	now.setCurrent();
	caPlainBuffer.AppendPrintf("|DateTime=%04d%02d%02d-%02d%02d%02d%03d", now.getYear(), now.getMonth(), now.getDate(), now.getHour(), now.getMinute(), now.getSecond(), now.getMillisecond());

	UFC::AnsiString plainText = caPlainBuffer.ToString();
	ResultData.SetPlainText(plainText);
	return GenerateSignatureAndCAData(plainText, ResultData);
}  //CUniFSCCAObject::GenerateReqMsgSignatureAndCAData()

#pragma package(smart_init)
