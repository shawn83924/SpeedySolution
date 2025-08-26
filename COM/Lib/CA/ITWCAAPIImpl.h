#pragma once

#include <Windows.h>
#include <string>
#include "ITWCAAPI.h"
#include "MLSecuritiesATL.h"

class ITWCAAPIImpl : public ITWCAAPI
{
private:
	/*
	class ITWCA_StringImpl : public ITWCA_String
	{
	private:
		LONG refCnt;
		std::string content;
	protected:
		void AddRef() override { InterlockedIncrement(&refCnt); }
		void Release() override 
		{
			if (InterlockedDecrement(&refCnt) <= 0) 
				delete this;
		}
		long GetLength() override { return content.size(); }
		const char* GetContent() override { return content.c_str(); }
	public:
		ITWCA_StringImpl(const char* signature) : refCnt{ 1 }, content{ signature }{}
	};
	*/

protected:
	void AddRef() { InterlockedIncrement(&refCnt); }
	void Release() 
	{
		if (InterlockedDecrement(&refCnt) <= 0)
		{
			mlcapi->Release();
			delete this;
		}
	}
	long SelectSigner(const char* cert_filter, long flag, long key_usage);
	long SelectSignerEx(const char* cert_filter, const char* pfx_path, const char* pfx_name, const char* password, const char* compare_date, long flag, long key_usage);
	long SignPkcs7(const char* plain_text, long flag, char** p_sign);
	void FreeAllocString(char* data) { delete[] data; }
	const char* GetErrorMsg();

private:
	IMLCapi* mlcapi;
	LONG refCnt;
	std::string lastErrMsg;

public:
	ITWCAAPIImpl(IMLCapi* api) : mlcapi(api), refCnt(1) {}
	static bool CreateInstance(ITWCAAPI** api_instance);
};

