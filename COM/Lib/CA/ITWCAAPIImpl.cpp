#include <comdef.h>
#include "ITWCAAPIImpl.h"

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

long ITWCAAPIImpl::SelectSigner(const char* cert_filter, long flag, long key_usage)
{
	BSTR filter = MultiBytesToBSTR(cert_filter);
	long retVal = 0;
	mlcapi->SelectSigner(filter, flag, key_usage, &retVal);
	
	BSTR bstr_errmsg = 0;
	mlcapi->GetErrorMsg(&bstr_errmsg);
	if (bstr_errmsg)
	{
		char* errmsg = BSTRToMultiBytes(bstr_errmsg);
		lastErrMsg = errmsg;
		SysFreeString(bstr_errmsg);
		delete[] errmsg;
	}
	else
		lastErrMsg = "";

	SysFreeString(filter);
	return retVal;
}

long ITWCAAPIImpl::SelectSignerEx(const char* cert_filter,
	                const char* pfx_path,
	                const char* pfx_name,
	                const char* password,
	                const char* compare_date,
	                long flag,
	                long key_usage)
{
	BSTR filter = MultiBytesToBSTR(cert_filter);
	BSTR path = MultiBytesToBSTR(pfx_path);
	BSTR name = MultiBytesToBSTR(pfx_name);
	BSTR pwd = MultiBytesToBSTR(password);
	BSTR comp_date = MultiBytesToBSTR(compare_date);

	long retVal = 0;
	mlcapi->SelectSignerEx(filter, path, name, pwd, comp_date, flag, key_usage, &retVal);
	
	BSTR bstr_errmsg = 0;
	mlcapi->GetErrorMsg(&bstr_errmsg);
	if (bstr_errmsg)
	{
		char* errmsg = BSTRToMultiBytes(bstr_errmsg);
		lastErrMsg = errmsg;
		SysFreeString(bstr_errmsg);
		delete[] errmsg;
	}
	else
		lastErrMsg = "";

	SysFreeString(filter);
	SysFreeString(path);
	SysFreeString(name);
	SysFreeString(pwd);
	SysFreeString(comp_date);

	return retVal;
}

long ITWCAAPIImpl::SignPkcs7(const char* plain_text,
	                         long flag,
	                         char** p_sign)
{
	BSTR text = MultiBytesToBSTR(plain_text);
	BSTR result = SysAllocString(L"");
	long retVal = 0;

	mlcapi->SignPkcs7(text, flag, &result);
	mlcapi->GetErrorCode(&retVal);
	
	BSTR bstr_errmsg = 0;
	mlcapi->GetErrorMsg(&bstr_errmsg);
	if (bstr_errmsg)
	{
		char* errmsg = BSTRToMultiBytes(bstr_errmsg);
		lastErrMsg = errmsg;
		SysFreeString(bstr_errmsg);
		delete[] errmsg;
	}
	else
		lastErrMsg = "";
	
	SysFreeString(text);

	if (retVal == 0)
	{
		char* u8Sign = BSTRToMultiBytes(result);
		*p_sign = u8Sign;		
	}

	SysFreeString(result);

	return retVal;
}

const char* ITWCAAPIImpl::GetErrorMsg()
{
	return lastErrMsg.c_str();
}

bool ITWCAAPIImpl::CreateInstance(ITWCAAPI** api_instance)
{
	IMLCapi* mlcapi = 0;

	HRESULT hr = CoCreateInstance(__uuidof(MLCapi), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMLCapi), (void**)&mlcapi);
	
	if (!SUCCEEDED(hr))
	{
		/*
		   TODO ... logit
		   
		   _com_error err(hr);

		*/

		return false;
	}

	*api_instance = new ITWCAAPIImpl(mlcapi);

	return true;
}