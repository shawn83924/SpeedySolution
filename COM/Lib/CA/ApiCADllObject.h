//---------------------------------------------------------------------------

#ifndef ApiCADllObjectH
#define ApiCADllObjectH
#include "UFC.h"

#ifdef WIN32
#include <Windows.h>
#endif

//#include "CAAPIBridge.h"
#include "../OrderMessage/TBaseMessage.h"
//--------------------------------------------------------------------------
typedef enum
{
	caNone = 0,
	caFSC  = 1,
	caTWCA = 2,
	caCGC  = 3
} ApiCAIDEnum;

//typedef bool (*CreateFSCAPIInstanceFunc)(IFSCATLAPI** ppv);
//typedef bool (*CreateTWCAAPIInstanceFunc)(ITWCAAPI** ppv);
//typedef bool (*CreateCGCAPIInstanceFunc)(ICGCDLLAPI** ppv);

class CApiCADllObject;
//---------------------------------------------------------------------------
class CAResultData
{
protected:
	CApiCADllObject* FApiCAObjPtr;
	UFC::AnsiString FDefaultBrokerID;
	UFC::AnsiString FPlainText;
	UFC::AnsiString FSignature;
	UFC::AnsiString FCertificate;
	UFC::AnsiString FSubject;
	UFC::AnsiString FNotBefore;
	UFC::AnsiString FNotAfter;
	UFC::AnsiString FSerialNumber;
	int             FErrorCode;
	UFC::AnsiString FResultMsg;

public:
	CAResultData()
		:FApiCAObjPtr(0)
		,FDefaultBrokerID(0)
		,FPlainText("")
		,FSignature("")
		,FCertificate("")
		,FSubject("")
		,FNotBefore("")
		,FNotAfter("")
		,FSerialNumber("")
		,FErrorCode(0)
		,FResultMsg("")
		{}

	CApiCADllObject* GetCAObj() {return FApiCAObjPtr;};
	void SetCAObj(CApiCADllObject* CAObjPtr) {FApiCAObjPtr = CAObjPtr;};

	UFC::AnsiString GetDefaultBrokerID() {return FDefaultBrokerID;};
	void SetDefaultBrokerID(const UFC::AnsiString& NewValue) {FDefaultBrokerID = NewValue;};

	UFC::AnsiString GetPlainText() {return FPlainText;};
	void SetPlainText(const UFC::AnsiString& NewValue) {FPlainText = NewValue;};

	UFC::AnsiString GetSignature() {return FSignature;};
	void SetSignature(const UFC::AnsiString& NewValue) {FSignature = NewValue;};

	UFC::AnsiString GetCertificate() {return FCertificate;};
	void SetCertificate(const UFC::AnsiString& NewValue) {FCertificate = NewValue;};

	UFC::AnsiString GetSubject() {return FSubject;};
	void SetSubject(const UFC::AnsiString& NewValue) {FSubject = NewValue;};

	UFC::AnsiString GetNotBefore() {return FNotBefore;};
	void SetNotBefore(const UFC::AnsiString& NewValue) {FNotBefore = NewValue;};

	UFC::AnsiString GetNotAfter() {return FNotAfter;};
	void SetNotAfter(const UFC::AnsiString& NewValue) {FNotAfter = NewValue;};

	UFC::AnsiString GetSerialNumber() {return FSerialNumber;};
	void SetSerialNumber(const UFC::AnsiString& NewValue) {FSerialNumber = NewValue;};

	int GetErrorCode() {return FErrorCode;};
	void SetErrorCode(int NewValue) {FErrorCode = NewValue;};

	UFC::AnsiString GetResultMsg() {return FResultMsg;};
	void SetResultMsg(const UFC::AnsiString& NewValue) {FResultMsg = NewValue;};
};  //CAResultData

//---------------------------------------------------------------------------
class CApiCADllObject : public UFC::CMainKeyObject
{
protected:
	ApiCAIDEnum       FCAID;
	UFC::AnsiString   FCommonName;
	UFC::AnsiString   FDLLFileName;
	UFC::BufferedLog* FLogPtr;

	UFC::AnsiString FSignerCertFilter;

#ifdef WIN32
	HMODULE FDllHandle;
#endif
//	CreateFSCAPIInstanceFunc FCreateFSCAPIInstance;
//	IFSCATLAPI* FFSCObjPtr;

//	CreateTWCAAPIInstanceFunc FCreateTWCAAPIInstance;
//	ITWCAAPI* FTWCAObjPtr;

	bool FIsWorking;

public:
	CApiCADllObject(ApiCAIDEnum CAID, const UFC::AnsiString& CommonName, UFC::BufferedLog* LogPtr);
	CApiCADllObject(ApiCAIDEnum CAID, const UFC::AnsiString& CommonName, const UFC::AnsiString& DllFileName, UFC::BufferedLog* LogPtr);
	~CApiCADllObject();

	bool LoadCADynamicLibrary();

//	bool CreateFSCObject();
//	bool CreateTWCAObject();
	virtual bool CreateActiveXCAObject();

	bool IsWorking(){return FIsWorking;};

	ApiCAIDEnum GetCAID() {return FCAID;};

	UFC::AnsiString GetCommonName() {return FCommonName;};
	void SetCommonName(const UFC::AnsiString& NewValue) {FCommonName = NewValue;};

	UFC::AnsiString GetDLLFileName() {return FDLLFileName;};
	void SetDLLFileName(const UFC::AnsiString& NewValue) {FDLLFileName = NewValue;};

	UFC::AnsiString GetSignerCertFilter() {return FSignerCertFilter;};
	void SetSignerCertFilter(const UFC::AnsiString& NewValue) {FSignerCertFilter = NewValue;};

	virtual int GenerateSignatureAndCAData(const UFC::AnsiString& PlainText, CAResultData& ResultData) {return 0;};
	virtual int GenerateLogonSignatureAndCAData(const UFC::AnsiString& LogonID, CAResultData& ResultData) {return 0;};
	virtual int GenerateOrderSignatureAndCAData(const UFC::AnsiString& OrderTelegram, CAResultData& ResultData) {return 0;};
	virtual int GenerateReqMsgSignatureAndCAData(TBaseMessage* ReqPtr, const UFC::AnsiString& OrderTelegram, CAResultData& ResultData) {return 0;};
};  //CApiCADllObject
#endif
