//---------------------------------------------------------------------------

#ifndef CACGCObjectH
#define CACGCObjectH
#include "UFC.h"

#include "CGCAPI.h"
#include "CABasicObjects.h"
__declspec(dllimport) int CGCAPI_PFXP7Sign(unsigned char* pbP12, unsigned long lP12, const char* strPassword, const void* pbData, unsigned long lData, int iFlags, int iHashFlag, int iKeyUsage, char** pstrSignature);
__declspec(dllimport) int CGCAPI_PKCS7GetUserCertificate(char *strP7Cert, char **strX509Cert);
__declspec(dllimport) int CGCAPI_CertGetSubject(const char* strCert, int iFlags, char** pstrSubject);
__declspec(dllimport) int CGCAPI_CertGetNotBefore(const char* strCert, int iFlags, char* strNotBefore);
__declspec(dllimport) int CGCAPI_CertGetNotAfter(const char* strCert, int iFlags, char* strNotAfter);
__declspec(dllimport) int CGCAPI_CertGetSerialNumber(const char* strCert, int iFlags, char** pstrSerialNumber);
__declspec(dllimport) int CGXCAPI_LoadStore(CGStoreHandle *pStore, int iStoreFlags, int iFlags);
__declspec(dllimport) int CGXCAPI_GetCertificates(CGStoreHandle hStore, const char* strSubject, const char* strIssuerSubkect, int iFlags, int iKeyUsage, CGCertHandle** phCerts, int* piCerts);
__declspec(dllimport) int CGCAPI_SignEx(const void* pbData, unsigned long lData, const char* strSubject, const char* strIssuerSubject, const char* strProvider, const char* strPin, int iFlags, int iHashFlag, int iKeyUsage, char** pstrSignature);
__declspec(dllimport) void CGCAPIMemFree(void *pMem);
__declspec(dllimport) void CGXCAPIFreeHandle(CGHandle hHandle);

typedef int (*CGCAPI_PFXP7SignPtr) (unsigned char*, unsigned long, const char*, const void*, unsigned long, int, int, int, char**);
typedef int (*CGCAPI_PKCS7GetUserCertificatePtr) (char*, char**);
typedef int (*CGCAPI_CertGetSubjectPtr) (const char*, int, char**);
typedef int (*CGCAPI_CertGetNotBeforePtr) (const char*, int, char*);
typedef int (*CGCAPI_CertGetNotAfterPtr) (const char*, int, char*);
typedef int (*CGCAPI_CertGetSerialNumberPtr) (const char*, int, char**);
typedef int (*CGXCAPI_LoadStorePtr) (CGStoreHandle*, int, int);
typedef int (*CGXCAPI_GetCertificatesPtr) (CGStoreHandle, const char*, const char*, int, int, CGCertHandle**, int*);
typedef int (*CGCAPI_SignExPtr) (const void*, unsigned long, const char*, const char*, const char*, const char*, int, int, int, char**);
typedef void (*CGCAPIMemFreePtr) (void*);
typedef void (*CGXCAPIFreeHandlePtr) (CGHandle);

//---------------------------------------------------------------------------
class CCACGCObject : public ICAFunctions
{
protected:
	UFC::AnsiString FDLLFileName;

	HINSTANCE                         FDllInstance;
	CGCAPI_PFXP7SignPtr               FfnCGCAPI_PFXP7Sign;
	CGCAPI_PKCS7GetUserCertificatePtr FfnCGCAPI_PKCS7GetUserCertificate;
	CGCAPI_CertGetSubjectPtr          FfnCGCAPI_CertGetSubject;
	CGCAPI_CertGetNotBeforePtr        FfnCGCAPI_CertGetNotBefore;
	CGCAPI_CertGetNotAfterPtr         FfnCGCAPI_CertGetNotAfter;
	CGCAPI_CertGetSerialNumberPtr     FfnCGCAPI_CertGetSerialNumber;
	CGXCAPI_LoadStorePtr              FfnCGXCAPI_LoadStore;
	CGXCAPI_GetCertificatesPtr        FfnCGXCAPI_GetCertificates;
	CGCAPI_SignExPtr                  FfnCGCAPI_SignEx;
	CGCAPIMemFreePtr                  FfnCGCAPIMemFree;
	CGXCAPIFreeHandlePtr              FfnCGXCAPIFreeHandle;

	bool            FIsWork;
	UFC::AnsiString FErrorMessage;
	UFC::AnsiString FCASubject;
	UFC::AnsiString FCANotBefore;
	UFC::AnsiString FCANotAfter;
	UFC::AnsiString FCASerialNumber;

	bool LoadCGCDLLLibrary();

public:
	CCACGCObject(const UFC::AnsiString& DLLFileName);
	virtual ~CCACGCObject();

	UFC::AnsiString GetDLLFileName() {return FDLLFileName;};

	virtual bool IsWork() {return FIsWork;};
	virtual UFC::AnsiString GetErrorMessage() {return FErrorMessage;};
	virtual UFC::AnsiString GetCASubject() {return FCASubject;};
	virtual UFC::AnsiString GetCANotBefore() {return FCANotBefore;};
	virtual UFC::AnsiString GetCANotAfter() {return FCANotAfter;};
	virtual UFC::AnsiString GetCASerialNumber() {return FCASerialNumber;};

	virtual UFC::AnsiString DescribeReturnCode(int ReturnCode);
	virtual int GenerateSignature(const UFC::AnsiString& DataStr, UFC::AnsiString& SignatureStr);
	virtual int GetCertificateBufferFromSignature(char* SignatureBuffer, char** CertificateBuffer);
	virtual int GetCertificateFromSignature(const UFC::AnsiString& SignatureStr, UFC::AnsiString& Certificate);
	virtual int GenerateSignatureAndCAData(const UFC::AnsiString& DataStr, UFC::AnsiString& SignatureStr);
};  //CCACGCObject
//---------------------------------------------------------------------------
class CCACGCFileObject : public CCACGCObject
{
protected:
	UFC::AnsiString FPFXFilePathName;
	UFC::AnsiString FPFXPassword;

	unsigned char* FPFXBuffer;
	int            FPFXDataSize;

	bool LoadPFXFile();

public:
	CCACGCFileObject(const UFC::AnsiString& DLLFileName, const UFC::AnsiString& PFXFilePathName, const UFC::AnsiString& PFXPassword);
	virtual ~CCACGCFileObject();

	UFC::AnsiString GetPFXFilePathName() {return FPFXFilePathName;};
	UFC::AnsiString GetPFXPassword() {return FPFXPassword;};

	virtual int GenerateSignatureBuffer(const char* DataBuffer, int DataLength, char** SignatureBuffer);
};  //CCACGCFileObject

//---------------------------------------------------------------------------
class CCACGCBrowserObject : public CCACGCObject
{
protected:
	UFC::AnsiString FOrganizationalUnit;
	UFC::AnsiString FCommonName;

	CGHandle      FCAStoreHandle;
	CGCertHandle* FCACertsHandlePtr;
	int           FCACertsNumber;

	char* FSubjectFilterBuffer;

public:
	CCACGCBrowserObject(const UFC::AnsiString& DLLFileName, const UFC::AnsiString& OrganizationalUnit, const UFC::AnsiString& CommonName);
	virtual ~CCACGCBrowserObject();

	UFC::AnsiString GetOrganizationalUnit() {return FOrganizationalUnit;};
	UFC::AnsiString GetCommonName() {return FCommonName;};

	virtual int GenerateSignatureBuffer(const char* DataBuffer, int DataLength, char** SignatureBuffer);
};  //CCACGCBrowserObject

#endif
