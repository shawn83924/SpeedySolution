#include "IFSCATLAPIImpl.h"

const char* err_0_Msg = "Success.";
const char*	err_5001_Msg = "general error.";
const char*	err_5002_Msg = "Memory Allocation Error.";
const char*	err_5003_Msg = "Buffer too small.";
const char* err_5004_Msg = "Function unsupport.";
const char* err_5005_Msg = "Invalid parameter.";
const char* err_5006_Msg = "Invalid handle.";
const char* err_5007_Msg = "TrialVersion Library is expired.";
const char* err_5008_Msg = "Base64 Encoding/Decoding Error.";
const char* err_5010_Msg = "certificate not found.";
const char* err_5011_Msg = "Certicate expired.";
const char* err_5012_Msg = "Certificate can not be used now.";
const char* err_5013_Msg = "Certificate Expire or not yet use.";
const char* err_5014_Msg = "Certificate subject not match.";
const char* err_5015_Msg = "Unable to find certificate issuer.";
const char* err_5016_Msg = " Certificate signature is invalid.";
const char* err_5017_Msg = "Invalid ertificate keyusage.";
const char* err_5020_Msg = "Certicate revoked.";
const char* err_5021_Msg = "Certicate key Compromised.";
const char* err_5022_Msg = "Certicate ca Compromised.";
const char* err_5023_Msg = "Certicate affiliation changed.";
const char* err_5024_Msg = "Certicate superseded.";
const char* err_5025_Msg = "Certicate cessation.";
const char* err_5026_Msg = "Certicate Hold.";
const char* err_5028_Msg = "Certicate remove from CRL.";
const char* err_5030_Msg = "Certicate expired.";
const char* err_5031_Msg = "CRL not yet valid.";
const char* err_5032_Msg = "CRL not found.";
const char* err_5034_Msg = "CRL bad signature.";
const char* err_5035_Msg = "Get digest error.";
const char* err_5036_Msg = "Bad signature.";
const char* err_5037_Msg = "Bad content.";
const char* err_5038_Msg = "Incorrect captcha.";
const char* err_5040_Msg = "Incorrect Certificate format.";
const char* err_5041_Msg = "Incorrect CRL format.";
const char* err_5042_Msg = "Incorrect PKCS7 format.";
const char* err_5043_Msg = "Invalid key.";
const char* err_5044_Msg = "Invalid certreq.";
const char* err_5045_Msg = "Invalid format.";
const char* err_5046_Msg = "Invalid PKCS12.";
const char* err_5050_Msg = "Object not found.";
const char* err_5051_Msg = "PKCS7 no content.";
const char* err_5052_Msg = "PKCS7 no certicate.";
const char* err_5053_Msg = "PKCS7 no signerinfo.";
const char* err_5060_Msg = "Unmatch certicate key.";
const char* err_5061_Msg = "Signature error.";
const char* err_5062_Msg = "Verify error.";
const char* err_5063_Msg = "Encrypt error.";
const char* err_5064_Msg = "Decrypt error.";
const char* err_5065_Msg = "Generate key error.";
const char* err_5066_Msg = "Delete user Certicate error.";
const char* err_5070_Msg = "Operation Canceled.";
const char* err_5071_Msg = "Password Invalid.";
const char* err_5080_Msg = "Parse error.";
const char* err_5081_Msg = "Tag not found.";
const char* err_5201_Msg = "Open store error.";
const char* err_5202_Msg = "Create chain error.";
const char* err_5203_Msg = "Crypt context error.";
const char* err_5204_Msg = "No private key.";
const char* err_5205_Msg = "Unexportable.";
const char* err_5206_Msg = "Store access deny.";
const char* err_5901_Msg = "Unicode error.";
const char* err_5902_Msg = "File not found.";
const char* err_5903_Msg = "Path not found.";
const char* err_5904_Msg = "Bad not path.";
const char* err_5905_Msg = "Logon failure.";
const char* err_5906_Msg = "Access Denied.";
const char* err_9100_Msg = "Object not exist.";
const char* err_9101_Msg = "Object exist.";
const char* err_9102_Msg = "Object has problem.";
const char* err_9110_Msg = "Library not load.";
const char* err_9111_Msg = "Library not load.";
const char* err_9112_Msg = "Slot not found.";
const char* err_unknown_Msg = "Unknown Error";

static BSTR MultiBytesToBSTR(const char* mb_data)
{
	// 計算轉換後需要的 buffer 長度
	int u16CharNum = MultiByteToWideChar(CP_ACP, 0, mb_data, -1, NULL, 0);
	// 轉換失敗直接回傳NULL
	if (0 == u16CharNum)
		return NULL;

	wchar_t* u16Buf = new wchar_t[u16CharNum];
	// ASCII 轉 UNICODE
	MultiByteToWideChar(CP_ACP, 0, mb_data, -1, u16Buf, u16CharNum);
	BSTR result = SysAllocString(u16Buf);
	delete[] u16Buf;

	return result;
}

static char* BSTRToMultiBytes(BSTR bstr_data)
{
	// 計算轉換後需要的 buffer 
	int u8CharNum = WideCharToMultiByte(CP_ACP, 0, bstr_data, -1, NULL, 0, NULL, NULL);
	// 轉換失敗直接回傳NULL
	if (0 == u8CharNum)
		return NULL;

	char* u8Buf = new char[u8CharNum];
	WideCharToMultiByte(CP_ACP, 0, bstr_data, -1, u8Buf, u8CharNum, NULL, NULL);
	return u8Buf;
}

long IFSCATLAPIImpl::XGetUserCertificate(const char* provider,
	                                  const char* subject,
	                                  int flag,
	                                  int key_usage,
	                                  char** pp_cert)
{
	BSTR bstr_prodider = MultiBytesToBSTR(provider);
	BSTR bstr_subject = MultiBytesToBSTR(subject);
	BSTR bstr_cert = SysAllocString(L"");
	long retVal = 0;
	
	fscapi->FSCAPI_XGetUserCertificate(bstr_prodider, bstr_subject, flag, key_usage, &bstr_cert);
	fscapi->GetErrorCode(&retVal);

	lastError = retVal;

	SysFreeString(bstr_prodider);
	SysFreeString(bstr_subject);

	if (retVal == 0)
	{
		char* cert = BSTRToMultiBytes(bstr_cert);
		*pp_cert = cert;		
	}

	SysFreeString(bstr_cert);

	return retVal;
}

long IFSCATLAPIImpl::CertGetSubject(const char* cert,
	                             int flag,
	                             char** pp_subject)
{
	BSTR bstr_cert = MultiBytesToBSTR(cert);
	BSTR bstr_subject = SysAllocString(L"");
	long retVal = 0;

	fscapi->FSCAPICertGetSubject(bstr_cert, flag, &bstr_subject);
	fscapi->GetErrorCode(&retVal);
	
	lastError = retVal;

	SysFreeString(bstr_cert);

	if (retVal == 0)
	{
		char* subject = BSTRToMultiBytes(bstr_subject);
		*pp_subject = subject;		
	}
	
	SysFreeString(bstr_subject);

	return retVal;
}

long IFSCATLAPIImpl::CertGetNotAfter(const char* cert,
	                              int flag,
	                              char** pp_notafter)
{
	BSTR bstr_cert = MultiBytesToBSTR(cert);
	BSTR bstr_notafter = SysAllocString(L"");
	long retVal = 0;

	fscapi->FSCAPICertGetNotAfter(bstr_cert, flag, &bstr_notafter);
	fscapi->GetErrorCode(&retVal);

	lastError = retVal;

	SysFreeString(bstr_cert);

	if (retVal == 0)
	{
		char* not_after = BSTRToMultiBytes(bstr_notafter);
		*pp_notafter = not_after;		
	}

	SysFreeString(bstr_notafter);

	return retVal;
}

long IFSCATLAPIImpl::CertGetNotBefore(const char* cert,
	                               int flag,
	                               char** pp_notbefore)
{
	BSTR bstr_cert = MultiBytesToBSTR(cert);
	BSTR bstr_notbefore = SysAllocString(L"");
	long retVal = 0;

	fscapi->FSCAPICertGetNotBefore(bstr_cert, flag, &bstr_notbefore);
	fscapi->GetErrorCode(&retVal);

	lastError = retVal;

	SysFreeString(bstr_cert);

	if (retVal == 0)
	{
		char* not_before = BSTRToMultiBytes(bstr_notbefore);
		*pp_notbefore = not_before;		
	}

	SysFreeString(bstr_notbefore);

	return retVal;
}

long IFSCATLAPIImpl::CertGetSerialNumber(const char* cert,
	                                  int flag,
	                                  char** pp_sno)
{
	BSTR bstr_cert = MultiBytesToBSTR(cert);
	BSTR bstr_sno = SysAllocString(L"");
	long retVal = 0;

	fscapi->FSXCAPICertGetSerialNumber(bstr_cert, flag, &bstr_sno);
	fscapi->GetErrorCode(&retVal);

	lastError = retVal;

	SysFreeString(bstr_cert);

	if (retVal == 0)
	{
		char* sno = BSTRToMultiBytes(bstr_sno);
		*pp_sno = sno;		
	}

	SysFreeString(bstr_sno);

	return retVal;
}

long IFSCATLAPIImpl::SignEx(const char* data,
	                        const char* subject,
	                        const char* provider,
	                        const char* pin,
	                        int flag,
	                        int key_usage,
	                        char** pp_sign)
{
	BSTR bstr_data = MultiBytesToBSTR(data);
	BSTR bstr_subj = MultiBytesToBSTR(subject);
	BSTR bstr_provider = MultiBytesToBSTR(provider);
	BSTR bstr_pin = MultiBytesToBSTR(pin);
	BSTR bstr_sign = SysAllocString(L"");
	long retVal = 0;

	fscapi->FSCAPISignEx(bstr_data, bstr_subj, bstr_provider, bstr_pin, flag, key_usage, &bstr_sign);
	fscapi->GetErrorCode(&retVal);

	lastError = retVal;

	SysFreeString(bstr_data);
	SysFreeString(bstr_subj);
	SysFreeString(bstr_provider);
	SysFreeString(bstr_pin);

	if (retVal == 0)
	{
		char* signature = BSTRToMultiBytes(bstr_sign);
		*pp_sign = signature;		
	}

	SysFreeString(bstr_sign);

	return retVal;
}

const char* IFSCATLAPIImpl::GetErrorMsg()
{
	switch (lastError)
	{
	case 0:
		return  err_0_Msg;
	case 5001:
		return err_5001_Msg;
	case 5002:
		return err_5002_Msg;
	case 5003:
		return err_5003_Msg;
	case 5004:
		return err_5004_Msg;
	case 5005:
		return err_5005_Msg;
	case 5006:
		return err_5006_Msg;
	case 5007:
		return err_5007_Msg;
	case 5008:
		return err_5008_Msg;
	case 5010:
		return err_5010_Msg;
	case 5011:
		return err_5011_Msg;
	case 5012:
		return err_5012_Msg;
	case 5013:
		return err_5013_Msg;
	case 5014:
		return err_5014_Msg;
	case 5015:
		return err_5015_Msg;
	case 5016:
		return err_5016_Msg;
	case 5017:
		return err_5017_Msg;
	case 5020:
		return err_5020_Msg;
	case 5021:
		return err_5021_Msg;
	case 5022:
		return err_5022_Msg;
	case 5023:
		return err_5023_Msg;
	case 5024:
		return err_5024_Msg;
	case 5025:
		return err_5025_Msg;
	case 5026:
		return err_5026_Msg;
	case 5028:
		return err_5028_Msg;
	case 5030:
		return err_5030_Msg;
	case 5031:
		return err_5031_Msg;
	case 5032:
		return err_5032_Msg;
	case 5034:
		return err_5034_Msg;
	case 5035: 
		return err_5035_Msg;
	case 5036: 
		return err_5036_Msg;
	case 5037: 
		return err_5037_Msg;
	case 5038: 
		return err_5038_Msg;
	case 5040: 
		return err_5040_Msg;
	case 5041: 
		return err_5041_Msg;
	case 5042: 
		return err_5042_Msg;
	case 5043: 
		return err_5043_Msg;
	case 5044: 
		return err_5044_Msg;
	case 5045: 
		return err_5045_Msg;
	case 5046: 
		return err_5046_Msg;
	case 5050: 
		return err_5050_Msg;
	case 5051: 
		return err_5051_Msg;
	case 5052: 
		return err_5052_Msg;
	case 5053: 
		return err_5053_Msg;
	case 5060: 
		return err_5060_Msg;
	case 5061: 
		return err_5061_Msg;
	case 5062: 
		return err_5062_Msg;
	case 5063: 
		return err_5063_Msg;
	case 5064: 
		return err_5064_Msg;
	case 5065: 
		return err_5065_Msg;
	case 5066: 
		return err_5066_Msg;
	case 5070: 
		return err_5070_Msg;
	case 5071: 
		return err_5071_Msg;
	case 5080: 
		return err_5080_Msg;
	case 5081: 
		return err_5081_Msg;
	case 5201: 
		return err_5201_Msg;
	case 5202: 
		return err_5202_Msg;
	case 5203: 
		return err_5203_Msg;
	case 5204: 
		return err_5204_Msg;
	case 5205: 
		return err_5205_Msg;
	case 5206: 
		return err_5206_Msg;
	case 5901: 
		return err_5901_Msg;
	case 5902: 
		return err_5902_Msg;
	case 5903: 
		return err_5903_Msg;
	case 5904: 
		return err_5904_Msg;
	case 5905: // Unknown logon user name or bad password
		return err_5905_Msg;
	case 5906: // Access Denied, no right
		return err_5906_Msg;
	case 9100:
		return err_9100_Msg;
	case 9101:
		return err_9101_Msg;
	case 9102:
		return err_9102_Msg;
	case 9110:
		return err_9110_Msg;
	case 9111:
		return err_9111_Msg;
	case 9112:
		return err_9112_Msg;
	default:
		return err_unknown_Msg;
	}	
}

bool IFSCATLAPIImpl::CreateInstance(IFSCATLAPI** api_instance)
{
	IFSCAPI* fscapi = 0;

	HRESULT hr = CoCreateInstance(__uuidof(FSCAPI), NULL, CLSCTX_INPROC_SERVER, __uuidof(IFSCAPI), (void**)&fscapi);
	/*
	if (hr == CO_E_NOTINITIALIZED)
	{
		CoInitialize(nullptr);
		hr = CoCreateInstance(__uuidof(FSCAPI), NULL, CLSCTX_INPROC_SERVER, __uuidof(IFSCAPI), (void**)&fscapi);
	}
	*/
	if (!SUCCEEDED(hr))
	{
		/*
		   TODO ... logit

		   _com_error err(hr);

		*/

		return false;
	}

	*api_instance = new IFSCATLAPIImpl(fscapi);

	return true;
}