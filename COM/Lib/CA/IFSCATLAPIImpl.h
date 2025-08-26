#pragma once
#include <Windows.h>
#include <string>
#include "IFSCATLAPI.h"
#include "FSCAPIATL2.h"

class IFSCATLAPIImpl : public IFSCATLAPI
{
private:
	/*
	class IFSCATLAPI_StringImpl : public IFSCATLAPI_String
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
		IFSCATLAPI_StringImpl(const char* signature) : refCnt{ 1 }, content{ signature }{}
	};
	*/
protected:
	void AddRef() { InterlockedIncrement(&refCnt); }
	void Release()
	{
		if (InterlockedDecrement(&refCnt) <= 0)
		{
			fscapi->Release();
			delete this;
		}
	}
	long XGetUserCertificate(const char* provider, const char* subject, int flag, int key_usage, char** pp_cert);
	long CertGetSubject(const char* cert, int flag, char** pp_subject);
	long CertGetNotAfter(const char* cert, int flag, char** pp_notafter);
	long CertGetNotBefore(const char* cert, int flag, char** pp_notbefore);
	long CertGetSerialNumber(const char* cert, int flag, char** pp_sno);
	long SignEx(const char* data, const char* subject, const char* provider, const char* pin, int flag, int key_usage, char** pp_sign);
	void FreeAllocString(char* data) { delete[] data; }
	const char* GetErrorMsg();

private:
	IFSCAPI* fscapi;
	LONG refCnt;
	long lastError;

public:
	IFSCATLAPIImpl(IFSCAPI* api) : fscapi(api), refCnt(1), lastError(0) {}
	static bool CreateInstance(IFSCATLAPI** api_instance);
};

