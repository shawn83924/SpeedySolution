//---------------------------------------------------------------------------

#pragma hdrstop

#include "MLTWCAObject.h"
#include "../OrderMessage/TNewOrderMessage.h"
#include "../OrderMessage/TCancelOrderMessage.h"
#include "../OrderMessage/TReplaceOrderMessage.h"

#ifdef WIN32
#include "ITWCAAPIImpl.h"
#endif  //#ifdef WIN32
//------------------------------ CMLTWCAObject ------------------------------
//---------------------------------------------------------------------------
CMLTWCAObject::CMLTWCAObject(const UFC::AnsiString& CommonName, UFC::BufferedLog* LogPtr)
:CApiCADllObject(caFSC, CommonName, LogPtr)
#ifdef WIN32
,FTWCAObjPtr(0)
#endif  //#ifdef WIN32
{
	if (CreateActiveXCAObject())
	{
		FIsWorking = true;
		FSignerCertFilter.Printf("S_OU=MasterLink,S_OU=RA-mlfutures01,S_CN=TW%s1,S_O=Certification Service Provider,S_O=TaiCA Secure CA,S_C=TW", FCommonName.c_str());
	}
}  //CUniFSCCAObject::CUniFSCCAObject()
//---------------------------------------------------------------------------
bool CMLTWCAObject::CreateActiveXCAObject()
{
	bool isSuccess = false;
#ifdef WIN32
#if !defined(_WIN64)
   if (ITWCAAPIImpl::CreateInstance(&FTWCAObjPtr))
	{
		FLogPtr->fprintf(" %s() Create TWCA Object Success.", __func__);
		isSuccess = true;
	}
	else
		FLogPtr->fprintf(" %s() Create TWCA Object Failed.", __func__);
#else
	FLogPtr->fprintf("%s() TWCA Object not Support 64 bit Program.", __func__);
#endif //#if !defined(_WIN64)
#endif  //#ifdef WIN32
	return isSuccess;
}  //CMLTWCAObject::CreateActiveXCAObject()
//---------------------------------------------------------------------------
int CMLTWCAObject::GenerateSignatureAndCAData(const UFC::AnsiString& PlainText, CAResultData& ResultData)
{
	int resultCode = -1;
	UFC::AnsiString csMsg;
#ifdef WIN32
#if !defined(_WIN64)
	if (FTWCAObjPtr == 0)
	{
		ResultData.SetErrorCode(resultCode);
		ResultData.SetResultMsg("TWCA Object is NULL.");
		return resultCode;
	}

	LockForWrite();
	if (FIsWorking)
	{
		long selectCode = FTWCAObjPtr->SelectSigner(FSignerCertFilter.c_str(), 2, 0);
		if (selectCode == 0)
		{
			char* signatureBuff = 0;
			long signatureErrCode = FTWCAObjPtr->SignPkcs7(PlainText.c_str(), 0, &signatureBuff);
			if (signatureErrCode == 0)
			{
				ResultData.SetSignature(UFC::AnsiString(signatureBuff));
				FTWCAObjPtr->FreeAllocString(signatureBuff);
			}
			else
			{
				FLogPtr->fprintf(" %s() SignPkcs7:%d", __func__, signatureErrCode);
				ResultData.SetResultMsg(UFC::AnsiString(FTWCAObjPtr->GetErrorMsg()));
			}
			resultCode = signatureErrCode;
		}
		else
		{
			FLogPtr->fprintf(" %s() SelectSigner:%d", __func__, selectCode);
			ResultData.SetResultMsg(UFC::AnsiString(FTWCAObjPtr->GetErrorMsg()));
			resultCode = selectCode;
		}
	}
	else
		ResultData.SetResultMsg("TWCA Object is not working.");

	UnLockForWrite();
#else
	ResultData.SetResultMsg("TWCA Object not Support 64 bit Program.");
#endif  //#if !defined(_WIN64)
#else
	ResultData.SetResultMsg("TWCA Object not Support none Windows Program.");
#endif  //#ifdef WIN32
	ResultData.SetErrorCode(resultCode);
	FLogPtr->fprintf(" %s() %d:%s", __func__, resultCode, ResultData.GetResultMsg().c_str());

	return resultCode;
}  //CMLTWCAObject::GenerateSignatureAndCAData()
//---------------------------------------------------------------------------
int CMLTWCAObject::GenerateLogonSignatureAndCAData(const UFC::AnsiString& LogonID, CAResultData& ResultData)
{
	UFC::AnsiString plainText = LogonID;
	ResultData.SetPlainText(plainText);
	return GenerateSignatureAndCAData(plainText, ResultData);
}  //CMLTWCAObject::GenerateLogonSignatureAndCAData()
//---------------------------------------------------------------------------
int CMLTWCAObject::GenerateReqMsgSignatureAndCAData(TBaseMessage* ReqPtr, const UFC::AnsiString& ReqTelegram, CAResultData& ResultData)
{
	UFC::AnsiString plainText = ReqTelegram;
	ResultData.SetPlainText(plainText);
	return GenerateSignatureAndCAData(plainText, ResultData);
}  //CMLTWCAObject::GenerateReqMsgSignatureAndCAData()
#pragma package(smart_init)
