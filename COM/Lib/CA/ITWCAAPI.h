#pragma once

/* 元富安控元件 MLSecuritiesATL ACTIVEX API Brideg */
struct ITWCAAPI
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual long SelectSigner(const char* cert_filter, long flag, long key_usage) = 0;
	virtual long SelectSignerEx(const char* cert_filter, const char* pfx_path, const char* pfx_name, const char* password, const char* compare_date, long flag, long key_usage) = 0;
	virtual long SignPkcs7(const char* plain_text, long flag, char** p_sign) = 0;
	virtual void FreeAllocString(char* data) = 0;
	virtual const char* GetErrorMsg() = 0;
};