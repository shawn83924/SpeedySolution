//---------------------------------------------------------------------------

#ifndef CACGCTSObjectH
#define CACGCTSObjectH
#include "UFC.h"

#include "CGCAPIDll.h"
#include "ApiCADllObject.h"
//#include "CABasicObjects.h"

/*
__declspec(dllimport) int CGCAPI_CertEncrypt(const char* strCert, const void* pbData, unsigned long lData, int iFlags, char** pstrCipher);
__declspec(dllimport) int CGCAPI_Sign(const void* pbData, unsigned long lData, const char* strSubject, int iFlags, int iKeyUsage, char** pstrSignature);
__declspec(dllimport) int CGCAPI_SignEx(const void* pbData, unsigned long lData, const char* strSubject, const char* strIssuerSubject, const char* strProvider, const char* strPin, int iFlags, int iHashFlag, int iKeyUsage, char** pstrSignature);
__declspec(dllimport) int CGCAPI_SignAndEncrypt(char* signerSubject, char* strEncryptCert, const void* pbData, unsigned long lData, int iFlags, int iHashFlags, int iKeyUsage, char** pstrSignCipher);
__declspec(dllimport) int CGCAPI_Verify(const char* strSignature, const void* pbData, unsigned long lData, const char* strSubject, int iFlags, int iKeyUsage, time_t iTime, char** pstrSignerInfo, char** pstrCert);
__declspec(dllimport) int CGCAPI_ComposeSignature(const char* strCert, const char* strDigest, int iFlags, char** pstrSignature);
__declspec(dllimport) int CGCAPI_VerifyDigest(const char* strCert, const char* strDigest, const void* pbData, unsigned long lData, const char* strSubject, int iFlags, time_t iTime);
__declspec(dllimport) int CGCAPI_Encrypt(const char* strSubject, int iStores, int iSelCertFlags, const void* pbData, unsigned long lData, int iFlags, int iKeyUsage, char** pstrCipher);
__declspec(dllimport) int CGCAPI_Decrypt(const char* strCipher, int iFlags, int iKeyUsage, void** ppbClear, unsigned long* plClear);
__declspec(dllimport) int CGCAPI_DecryptAndVerify(const char* strCipher, int iSignerIndex, int iFlags, void** ppbClear, unsigned long* plClear, char** strSignerCert, char** strEncryptCert);
__declspec(dllimport) int CGCAPI_Hash(const void* pbData, unsigned long lData, int iFlags, char** pstrDigest);
__declspec(dllimport) int CGCAPI_ExportPKCS12(const char* strSubject, int iFlags, int iKeyUsage, const char* strPassword, void** ppbP12, unsigned long* plP12);
__declspec(dllimport) int CGCAPI_CertGetDigest(const char* strCert, int iFlags, char** pstrDigest);
__declspec(dllimport) int CGCAPI_CertGetIssuer(const char* strCert, int iFlags, char** pstrIssuer);
__declspec(dllimport) int CGCAPI_CertGetNotAfter(const char* strCert, int iFlags, char* strNotAfter);
__declspec(dllimport) int CGCAPI_CertGetNotBefore(const char* strCert, int iFlags, char* strNotBefore);
__declspec(dllimport) int CGCAPI_CertGetFXMLExtension(const char* X509Cert, int index, char** extvalue);
__declspec(dllimport) int CGCAPI_CertGetSubject(const char* strCert, int iFlags, char** pstrSubject);
__declspec(dllimport) int CGCAPI_RegisterCertificate(const char* strProvider, int iFlags);
__declspec(dllimport) int CGCAPI_ComposePKCS7(const char* strCert, const char* strRawSig, const char* pbData, unsigned long lData, int iFlags, int iHashFlags, char** pstrSignature);
__declspec(dllimport) int CGCAPI_PKCS7GetUserCertificate(char* strP7Cert, char** strX509Cert);
__declspec(dllimport) int CGCAPI_ShowCertificate(WCHAR* strMessage, const char* strCert, int iFlags);
__declspec(dllimport) int CGCAPI_PureSign(const void* pbData, unsigned long lData, const char* strSubject, const char* strIssuerSubject, const char* strProvider, const char* strPin, int iFlags, int iHashalg, int iKeyUsage, char** pstrSignature, int* lSignature, char** pstrCert);
__declspec(dllimport) int CGCAPI_PFXGetCertCount(unsigned char* pbP12, unsigned long lP12, const char* strPassword, int* lOutCntCert);
__declspec(dllimport) void* CGCAPIMemAlloc(unsigned long lSize);
__declspec(dllimport) void* CGCAPIMemRealloc(void* pMem, unsigned long lSize);
__declspec(dllimport) void CGCAPIMemFree(void* pMem);
__declspec(dllimport) int CGXCAPI_SignAndEncrypt(CGCertHandle hSignCert, CGCertHandle hEncryptCert, CGStoreHandle hStore, const void* pbData, unsigned long lData, int iFlags, int iHashFlags, CGBlob* pstSignCipherBlob);
__declspec(dllimport) int CGXCAPI_ComposeSignature(CGCertHandle hCert, CGSignerInfoHandle hDigest, const void* pbData, unsigned long lData, int iFlags, CGPKCS7Handle* phSignature);
__declspec(dllimport) int CGXCAPI_DecryptAndVerify(CGStoreHandle hStore, const void* pbCipher, unsigned long lCipher, int iSignerIndex, int iFlags, CGBlob* pstClearBlob, CGCertHandle* phSignerCert, CGCertHandle* phEncryptCert);
__declspec(dllimport) int CGXCAPI_GetUserCertificate(const char* strSubject, const char* strIssuerSubject, const char* strSubjectAltName, int iFlags, int iKeyUsage, CGCertHandle* phCert);
__declspec(dllimport) int CGXCAPI_GetCertificate(CGStoreHandle hStore, const char* strSubject, const char* strIssuerSubject, const char* strSubjectAltName, int iFlags, int iKeyUsage, CGCertHandle* phCert);
__declspec(dllimport) int CGXCAPI_GenerateKey(int iAlgor, const char* strPassword, const void* pbSalt, unsigned long lSalt, unsigned long lIteration, int iFlags, CGKeyHandle* phKey);
__declspec(dllimport) int CGXCAPI_GetPrivateKey(CGCertHandle hCert, CGKeyHandle* hKey);
__declspec(dllimport) int CGXCAPI_CertGetPublicKey(CGCertHandle hCert, CGKeyHandle* phKey);
__declspec(dllimport) int CGXCAPI_Encrypt(CGCertHandle* hCerts, int iCerts, const void* pbData, unsigned long lData, int iFlags, CGBlob* pstCipherBlob);
__declspec(dllimport) int CGXCAPI_EncryptInit(CGKeyHandle hKey, const void* iv, int iFlags);
__declspec(dllimport) int CGXCAPI_EncryptUpdate(CGKeyHandle hKey, const void* pbData, unsigned long lData, void* pbCipher, unsigned long* plCipher, int iFlags);
__declspec(dllimport) int CGXCAPI_EncryptFinal(CGKeyHandle hKey, void* pbCipher, unsigned long* plCipher, int iFlags);
__declspec(dllimport) int CGXCAPI_Decrypt(CGStoreHandle hStore, const void* pbCipher, unsigned long lCipher, int iFlags, CGBlob* pstClearBlob, CGCertHandle* phEncryptCert);
__declspec(dllimport) int CGXCAPI_DecryptInit(CGKeyHandle hKey, const void* iv, int iFlags);
__declspec(dllimport) int CGXCAPI_DecryptUpdate(CGKeyHandle hKey, const void* pbCipher, unsigned long lCipher, void* pbClear, unsigned long* plClear, int iFlags);
__declspec(dllimport) int CGXCAPI_DecryptFinal(CGKeyHandle hKey, void* pbClear, unsigned long* plClear, int iFlags);
__declspec(dllimport) int CGXCAPI_HashInit(CGHashHandle* phHash, int iHashFlags);
__declspec(dllimport) int CGXCAPI_HashUpdate(CGHashHandle hHash, const void* pbData, unsigned long lData, int iFlags);
__declspec(dllimport) int CGXCAPI_HashFinal(CGHashHandle hHash, int iFlags, CGBlob* pstHashBlob);
__declspec(dllimport) int CGXCAPI_CertGetSerialNumber(CGCertHandle hCert, int iFlags, CGBlob* pstSerialNumber);
__declspec(dllimport) int CGXCAPI_CertGetSubject(CGCertHandle hCert, int iFlags, char** pstrSubject);
__declspec(dllimport) int CGXCAPI_CertGetIssuer(CGCertHandle hCert, int iFlags, char** pstrIssuer);
__declspec(dllimport) int CGXCAPI_CertGetNotBefore(CGCertHandle hCert, int iFlags, char* strNotBefore);
__declspec(dllimport) int CGXCAPI_CertGetNotAfter(CGCertHandle hCert, int iFlags, char* strNotAfter);
__declspec(dllimport) int CGXCAPI_CertGetDigest(CGCertHandle hCert, int iFlags, CGBlob* pstHashBlob);
__declspec(dllimport) int CGXCAPI_ExportPKCS12(CGCertHandle hCert, CGStoreHandle hStore, int iFlags, const char* strPassword, CGBlob* pstP12Blob);
__declspec(dllimport) int CGXCAPI_Sign(CGCertHandle hCert, CGStoreHandle hStore, const void* pbData, unsigned long lData, const char* strPin, int iFlags, int iHashFlags, CGPKCS7Handle* phSignature);
__declspec(dllimport) int CGXCAPI_Verify(CGStoreHandle hStore, CGPKCS7Handle hSignature, const void* pbData, unsigned long lData, const char* strSubject, int iSignerIndex, int iFlags, time_t iTime, CGSignerInfoHandle* phSignerInfo, CGCertHandle* phCert);
__declspec(dllimport) int CGXCAPI_PKCS7GetSignerInfoCount(CGPKCS7Handle hSignature, int* piCount);
__declspec(dllimport) int CGXCAPI_PKCS7GetSignerInfo(CGPKCS7Handle hSignature, int iSignerIndex, CGSignerInfoHandle* phDigest);
__declspec(dllimport) int CGXCAPI_PKCS7GetSignerCertificate(CGPKCS7Handle hSignature, int iSignerIndex, CGCertHandle* phCert);
__declspec(dllimport) int CGXCAPI_PKCS7GetContent(CGPKCS7Handle hSignature, CGBlob* pstContentBlob);
__declspec(dllimport) int CGXCAPI_PKCS7GetUserCertificate(CGPKCS7Handle hCerts, CGCertHandle* phCert);
__declspec(dllimport) int CGXCAPI_PKCS7AddSignerCertificate(CGPKCS7Handle hSignature, CGCertHandle hCert);
__declspec(dllimport) int CGXCAPI_PureSign(CGKeyHandle hPriKey, const void* pbData, unsigned long lData, const char* strPin, int iFlags, CGBlob* pstSignatureBlob);
__declspec(dllimport) int CGXCAPI_PureVerify(CGKeyHandle hPubKey, const void* pbSignature, unsigned long lSignature, const void* pbData, unsigned long lData, int iFlags);
__declspec(dllimport) int CGXCAPI_Encode(const void* pbData, unsigned long lData, CGBlob* pstEncodedBlob, const char* pemtitle);
__declspec(dllimport) int CGXCAPI_EncodeObject(CGHandle hHandle, int iType, CGBlob* pstBlob, int iFlags, const void* pParam);
__declspec(dllimport) int CGXCAPI_Decode(const char* pbEncoded, unsigned long lEncoded, CGBlob* pstDecodedBlob, const char* pemtitle);
__declspec(dllimport) int CGXCAPI_DecodeObject(CGHandle* pHandle, int iType, const void* pbEncoded, unsigned long lEncoded, int iFlags, const void* pParam);
__declspec(dllimport) int CGXCAPI_LoadStore(CGStoreHandle* pStore, int iStoreFlags, int iFlags);
__declspec(dllimport) int CGXCAPI_LoadStoreByName(CGStoreHandle* pStore, int iFlags, const wchar_t* strName);
__declspec(dllimport) int CGXCAPI_VerifyCertChain(CGCertHandle hCert, CGStoreHandle hStore, int iFlags, int iTime);
__declspec(dllimport) int CGXCAPI_AcceptPKCS7(char* cstrCertificate, int iFlags, char* pin, WCHAR* wfriendly_name, long* result);
__declspec(dllimport) int CGXCAPI_DeleteUsrCert(const char* strSubject, int iFlags, int iKeyUsage);
__declspec(dllimport) int CGXCAPI_ChangeP12Pwd(unsigned char* ppfx, int pfxlen, char* strOldPFXPasswd, char* strNewPFXPasswd, CGBlob* pstP12Blob);
__declspec(dllimport) void CGXCAPIFreeHandle(CGHandle hHandle);
__declspec(dllimport) void CGXCAPIFreeBlob(CGBlob* pstBlob);
__declspec(dllimport) int CGCAPI_PFXP7Sign(unsigned char* pbP12, unsigned long lP12, const char* strPassword, const void* pbData, unsigned long	lData, int iFlags, int iHashFlag, int iKeyUsage, char** pstrSignature);
__declspec(dllimport) int CGCAPI_CertGetSerialNumber(const char* strCert, int iFlags, char** pstrSerialNumber);
__declspec(dllimport) int CGCAPI_CertGetSubjectW(const char* strCert, int iFlags, wchar_t** pstrSubject);
__declspec(dllimport) int CGXCAPI_GetCertificates(CGStoreHandle hStore, const char* strSubject, const char* strIssuerSubkect, int iFlags, int iKeyUsage, CGCertHandle** phCerts, int* piCerts);
__declspec(dllimport) int CGCAPI_GetUserCertificate(const char* strProvider, const char* strSubject, const char* strSubjectAltName, int iFlags, int iKeyUsage, char** pCert);
__declspec(dllimport) int CGCAPI_GetUserCertificates(const char* strProvider, const char* strSubject, int iFlags, int iKeyUsage, int iDisableATSpec, char*** pCerts, int* piCerts);
__declspec(dllimport) int GetDLLVersion(long iFlags, char** ver);
*/

typedef int (*CGCAPI_CertEncryptPtr) (const char*, const void*, unsigned long, int, char**);
typedef int (*CGCAPI_SignPtr) (const void*, unsigned long, const char*, int, int, char**);
typedef int (*CGCAPI_SignExPtr) (const void*, unsigned long, const char*, const char*, const char*, const char*, int, int, int, char**);
typedef int (*CGCAPI_SignAndEncryptPtr) (char*, char*, const void*, unsigned long, int, int, int, char**);
typedef int (*CGCAPI_VerifyPtr) (const char*, const void*, unsigned long, const char*, int, int, time_t, char**, char**);
typedef int (*CGCAPI_ComposeSignaturePtr) (const char*, const char*, int, char**);
typedef int (*CGCAPI_VerifyDigestPtr) (const char*, const char*, const void*, unsigned long, const char*, int, time_t);
typedef int (*CGCAPI_EncryptPtr) (const char*, int, int, const void*, unsigned long, int, int, char**);
typedef int (*CGCAPI_DecryptPtr) (const char*, int, int, void**, unsigned long*);
typedef int (*CGCAPI_DecryptAndVerifyPtr) (const char*, int, int, void**, unsigned long*, char**, char**);
typedef int (*CGCAPI_HashPtr) (const void*, unsigned long, int, char**);
typedef int (*CGCAPI_ExportPKCS12Ptr) (const char*, int, int, const char*, void**, unsigned long*);
typedef int (*CGCAPI_CertGetDigestPtr) (const char*, int, char**);
typedef int (*CGCAPI_CertGetIssuerPtr) (const char*, int, char**);
typedef int (*CGCAPI_CertGetNotAfterPtr) (const char*, int, char*);
typedef int (*CGCAPI_CertGetNotBeforePtr) (const char*, int, char*);
typedef int (*CGCAPI_CertGetFXMLExtensionPtr) (const char*, int, char**);
typedef int (*CGCAPI_CertGetSubjectPtr) (const char*, int, char**);
typedef int (*CGCAPI_RegisterCertificatePtr) (const char*, int);
typedef int (*CGCAPI_ComposePKCS7Ptr) (const char*, const char*, const char*, unsigned long, int, int, char**);
typedef int (*CGCAPI_PKCS7GetUserCertificatePtr) (char*, char**);
typedef int (*CGCAPI_ShowCertificatePtr) (WCHAR*, const char*, int);
typedef int (*CGCAPI_PureSignPtr) (const void*, unsigned long, const char*, const char*, const char*, const char*, int, int, int, char**, int*, char**);
typedef int (*CGCAPI_PFXGetCertCountPtr) (unsigned char*, unsigned long, const char*, int*);
typedef void* (*CGCAPIMemAllocPtr) (unsigned long);
typedef void* (*CGCAPIMemReallocPtr) (void*, unsigned long);
typedef void (*CGCAPIMemFreePtr) (void*);
typedef int (*CGXCAPI_SignAndEncryptPtr) (CGCertHandle, CGCertHandle, CGStoreHandle, const void*, unsigned long, int, int, CGBlob*);
typedef int (*CGXCAPI_ComposeSignaturePtr) (CGCertHandle, CGSignerInfoHandle, const void*, unsigned long, int, CGPKCS7Handle*);
typedef int (*CGXCAPI_DecryptAndVerifyPtr) (CGStoreHandle, const void*, unsigned long, int, int, CGBlob*, CGCertHandle*, CGCertHandle*);
typedef int (*CGXCAPI_GetUserCertificatePtr) (const char*, const char*, const char*, int, int, CGCertHandle*);
typedef int (*CGXCAPI_GetCertificatePtr) (CGStoreHandle, const char*, const char*, const char*, int, int, CGCertHandle*);
typedef int (*CGXCAPI_GenerateKeyPtr) (int, const char*, const void*, unsigned long, unsigned long, int, CGKeyHandle*);
typedef int (*CGXCAPI_GetPrivateKeyPtr) (CGCertHandle, CGKeyHandle*);
typedef int (*CGXCAPI_CertGetPublicKeyPtr) (CGCertHandle, CGKeyHandle*);
typedef int (*CGXCAPI_EncryptPtr) (CGCertHandle*, int, const void*, unsigned long, int, CGBlob*);
typedef int (*CGXCAPI_EncryptInitPtr) (CGKeyHandle, const void*, int);
typedef int (*CGXCAPI_EncryptUpdatePtr) (CGKeyHandle, const void*, unsigned long, void*, unsigned long*, int);
typedef int (*CGXCAPI_EncryptFinalPtr) (CGKeyHandle, void*, unsigned long*, int);
typedef int (*CGXCAPI_DecryptPtr) (CGStoreHandle, const void*, unsigned long, int, CGBlob*, CGCertHandle*);
typedef int (*CGXCAPI_DecryptInitPtr) (CGKeyHandle, const void*, int);
typedef int (*CGXCAPI_DecryptUpdatePtr) (CGKeyHandle, const void*, unsigned long, void*, unsigned long*, int);
typedef int (*CGXCAPI_DecryptFinalPtr) (CGKeyHandle, void*, unsigned long*, int);
typedef int (*CGXCAPI_HashInitPtr) (CGHashHandle*, int);
typedef int (*CGXCAPI_HashUpdatePtr) (CGHashHandle, const void*, unsigned long, int);
typedef int (*CGXCAPI_HashFinalPtr) (CGHashHandle, int, CGBlob*);
typedef int (*CGXCAPI_CertGetSerialNumberPtr) (CGCertHandle, int, CGBlob*);
typedef int (*CGXCAPI_CertGetSubjectPtr) (CGCertHandle, int, char**);
typedef int (*CGXCAPI_CertGetIssuerPtr) (CGCertHandle, int, char**);
typedef int (*CGXCAPI_CertGetNotBeforePtr) (CGCertHandle, int, char*);
typedef int (*CGXCAPI_CertGetNotAfterPtr) (CGCertHandle, int, char*);
typedef int (*CGXCAPI_CertGetDigestPtr) (CGCertHandle, int, CGBlob*);
typedef int (*CGXCAPI_ExportPKCS12Ptr) (CGCertHandle, CGStoreHandle, int, const char*, CGBlob*);
typedef int (*CGXCAPI_SignPtr) (CGCertHandle, CGStoreHandle, const void*, unsigned long, const char*, int, int, CGPKCS7Handle*);
typedef int (*CGXCAPI_VerifyPtr) (CGStoreHandle, CGPKCS7Handle, const void*, unsigned long, const char*, int, int, time_t, CGSignerInfoHandle*, CGCertHandle*);
typedef int (*CGXCAPI_PKCS7GetSignerInfoCountPtr) (CGPKCS7Handle, int*);
typedef int (*CGXCAPI_PKCS7GetSignerInfoPtr) (CGPKCS7Handle, int, CGSignerInfoHandle*);
typedef int (*CGXCAPI_PKCS7GetSignerCertificatePtr) (CGPKCS7Handle, int, CGCertHandle*);
typedef int (*CGXCAPI_PKCS7GetContentPtr) (CGPKCS7Handle, CGBlob*);
typedef int (*CGXCAPI_PKCS7GetUserCertificatePtr) (CGPKCS7Handle, CGCertHandle*);
typedef int (*CGXCAPI_PKCS7AddSignerCertificatePtr) (CGPKCS7Handle, CGCertHandle);
typedef int (*CGXCAPI_PureSignPtr) (CGKeyHandle, const void*, unsigned long, const char*, int, CGBlob*);
typedef int (*CGXCAPI_PureVerifyPtr) (CGKeyHandle, const void*, unsigned long, const void*, unsigned long, int);
typedef int (*CGXCAPI_EncodePtr) (const void*, unsigned long, CGBlob*, const char*);
typedef int (*CGXCAPI_EncodeObjectPtr) (CGHandle, int, CGBlob*, int, const void*);
typedef int (*CGXCAPI_DecodePtr) (const char*, unsigned long, CGBlob*, const char*);
typedef int (*CGXCAPI_DecodeObjectPtr) (CGHandle*, int, const void*, unsigned long, int, const void*);
typedef int (*CGXCAPI_LoadStorePtr) (CGStoreHandle*, int, int);
typedef int (*CGXCAPI_LoadStoreByNamePtr) (CGStoreHandle*, int, const wchar_t*);
typedef int (*CGXCAPI_VerifyCertChainPtr) (CGCertHandle, CGStoreHandle, int, int);
typedef int (*CGXCAPI_AcceptPKCS7Ptr) (char*, int, char*, WCHAR*, long*);
typedef int (*CGXCAPI_DeleteUsrCertPtr) (const char*, int, int);
typedef int (*CGXCAPI_ChangeP12PwdPtr) (unsigned char*, int, char*, char*, CGBlob*);
typedef void (*CGXCAPIFreeHandlePtr) (CGHandle);
typedef void (*CGXCAPIFreeBlobPtr) (CGBlob*);
typedef int (*CGCAPI_PFXP7SignPtr) (unsigned char*, unsigned long, const char*, const void*, unsigned long, int, int, int, char**);
typedef int (*CGCAPI_CertGetSerialNumberPtr) (const char*, int, char**);
typedef int (*CGCAPI_CertGetSubjectWPtr) (const char*, int, wchar_t**);
typedef int (*CGXCAPI_GetCertificatesPtr) (CGStoreHandle, const char*, const char*, int, int, CGCertHandle**, int*);
typedef int (*CGCAPI_GetUserCertificatePtr) (const char*, const char*, const char*, int, int, char**);
typedef int (*CGCAPI_GetUserCertificatesPtr) (const char*, const char*, int, int, int, char***, int*);
typedef int (*GetDLLVersionPtr) (long, char**);

//---------------------------------------------------------------------------
class CTSCGCCAObject : public CApiCADllObject
{
public:
	static UFC::AnsiString GetErrorMsgA(int ErrorID);
protected:
	HINSTANCE FDllInstance;

	CGCAPI_CertEncryptPtr                FfnCGCAPI_CertEncrypt;
	CGCAPI_SignPtr                       FfnCGCAPI_Sign;  //
	CGCAPI_SignExPtr                     FfnCGCAPI_SignEx;  //
	CGCAPI_SignAndEncryptPtr             FfnCGCAPI_SignAndEncrypt;
	CGCAPI_VerifyPtr                     FfnCGCAPI_Verify;
	CGCAPI_ComposeSignaturePtr           FfnCGCAPI_ComposeSignature;
	CGCAPI_VerifyDigestPtr               FfnCGCAPI_VerifyDigest;
	CGCAPI_EncryptPtr                    FfnCGCAPI_Encrypt;
	CGCAPI_DecryptPtr                    FfnCGCAPI_Decrypt;
	CGCAPI_DecryptAndVerifyPtr           FfnCGCAPI_DecryptAndVerify;
	CGCAPI_HashPtr                       FfnCGCAPI_Hash;
	CGCAPI_ExportPKCS12Ptr               FfnCGCAPI_ExportPKCS12;
	CGCAPI_CertGetDigestPtr              FfnCGCAPI_CertGetDigest;
	CGCAPI_CertGetIssuerPtr              FfnCGCAPI_CertGetIssuer;
	CGCAPI_CertGetNotAfterPtr            FfnCGCAPI_CertGetNotAfter;
	CGCAPI_CertGetNotBeforePtr           FfnCGCAPI_CertGetNotBefore;
	CGCAPI_CertGetFXMLExtensionPtr       FfnCGCAPI_CertGetFXMLExtension;
	CGCAPI_CertGetSubjectPtr             FfnCGCAPI_CertGetSubject;
	CGCAPI_RegisterCertificatePtr        FfnCGCAPI_RegisterCertificate;
	CGCAPI_ComposePKCS7Ptr               FfnCGCAPI_ComposePKCS7;
	CGCAPI_PKCS7GetUserCertificatePtr    FfnCGCAPI_PKCS7GetUserCertificate;
	CGCAPI_ShowCertificatePtr            FfnCGCAPI_ShowCertificate;
	CGCAPI_PureSignPtr                   FfnCGCAPI_PureSign;
	CGCAPI_PFXGetCertCountPtr            FfnCGCAPI_PFXGetCertCount;
	CGCAPIMemAllocPtr                    FfnCGCAPIMemAlloc;
	CGCAPIMemReallocPtr                  FfnCGCAPIMemRealloc;
	CGCAPIMemFreePtr                     FfnCGCAPIMemFree;
	CGXCAPI_SignAndEncryptPtr            FfnCGXCAPI_SignAndEncrypt;
	CGXCAPI_ComposeSignaturePtr          FfnCGXCAPI_ComposeSignature;
	CGXCAPI_DecryptAndVerifyPtr          FfnCGXCAPI_DecryptAndVerify;
	CGXCAPI_GetUserCertificatePtr        FfnCGXCAPI_GetUserCertificate;
	CGXCAPI_GetCertificatePtr            FfnCGXCAPI_GetCertificate;
	CGXCAPI_GenerateKeyPtr               FfnCGXCAPI_GenerateKey;
	CGXCAPI_GetPrivateKeyPtr             FfnCGXCAPI_GetPrivateKey;
	CGXCAPI_CertGetPublicKeyPtr          FfnCGXCAPI_CertGetPublicKey;
	CGXCAPI_EncryptPtr                   FfnCGXCAPI_Encrypt;
	CGXCAPI_EncryptInitPtr               FfnCGXCAPI_EncryptInit;
	CGXCAPI_EncryptUpdatePtr             FfnCGXCAPI_EncryptUpdate;
	CGXCAPI_EncryptFinalPtr              FfnCGXCAPI_EncryptFinal;
	CGXCAPI_DecryptPtr                   FfnCGXCAPI_Decrypt;
	CGXCAPI_DecryptInitPtr               FfnCGXCAPI_DecryptInit;
	CGXCAPI_DecryptUpdatePtr             FfnCGXCAPI_DecryptUpdate;
	CGXCAPI_DecryptFinalPtr              FfnCGXCAPI_DecryptFinal;
	CGXCAPI_HashInitPtr                  FfnCGXCAPI_HashInit;
	CGXCAPI_HashUpdatePtr                FfnCGXCAPI_HashUpdate;
	CGXCAPI_HashFinalPtr                 FfnCGXCAPI_HashFinal;
	CGXCAPI_CertGetSerialNumberPtr       FfnCGXCAPI_CertGetSerialNumber;
	CGXCAPI_CertGetSubjectPtr            FfnCGXCAPI_CertGetSubject;
	CGXCAPI_CertGetIssuerPtr             FfnCGXCAPI_CertGetIssuer;
	CGXCAPI_CertGetNotBeforePtr          FfnCGXCAPI_CertGetNotBefore;
	CGXCAPI_CertGetNotAfterPtr           FfnCGXCAPI_CertGetNotAfter;
	CGXCAPI_CertGetDigestPtr             FfnCGXCAPI_CertGetDigest;
	CGXCAPI_ExportPKCS12Ptr              FfnCGXCAPI_ExportPKCS12;
	CGXCAPI_SignPtr                      FfnCGXCAPI_Sign;
	CGXCAPI_VerifyPtr                    FfnCGXCAPI_Verify;
	CGXCAPI_PKCS7GetSignerInfoCountPtr   FfnCGXCAPI_PKCS7GetSignerInfoCount;
	CGXCAPI_PKCS7GetSignerInfoPtr        FfnCGXCAPI_PKCS7GetSignerInfo;
	CGXCAPI_PKCS7GetSignerCertificatePtr FfnCGXCAPI_PKCS7GetSignerCertificate;
	CGXCAPI_PKCS7GetContentPtr           FfnCGXCAPI_PKCS7GetContent;
	CGXCAPI_PKCS7GetUserCertificatePtr   FfnCGXCAPI_PKCS7GetUserCertificate;
	CGXCAPI_PKCS7AddSignerCertificatePtr FfnCGXCAPI_PKCS7AddSignerCertificate;
	CGXCAPI_PureSignPtr                  FfnCGXCAPI_PureSign;
	CGXCAPI_PureVerifyPtr                FfnCGXCAPI_PureVerify;
	CGXCAPI_EncodePtr                    FfnCGXCAPI_Encode;
	CGXCAPI_EncodeObjectPtr              FfnCGXCAPI_EncodeObject;
	CGXCAPI_DecodePtr                    FfnCGXCAPI_Decode;
	CGXCAPI_DecodeObjectPtr              FfnCGXCAPI_DecodeObject;
	CGXCAPI_LoadStorePtr                 FfnCGXCAPI_LoadStore;
	CGXCAPI_LoadStoreByNamePtr           FfnCGXCAPI_LoadStoreByName;
	CGXCAPI_VerifyCertChainPtr           FfnCGXCAPI_VerifyCertChain;
	CGXCAPI_AcceptPKCS7Ptr               FfnCGXCAPI_AcceptPKCS7;
	CGXCAPI_DeleteUsrCertPtr             FfnCGXCAPI_DeleteUsrCert;
	CGXCAPI_ChangeP12PwdPtr              FfnCGXCAPI_ChangeP12Pwd;
	CGXCAPIFreeHandlePtr                 FfnCGXCAPIFreeHandle;
	CGXCAPIFreeBlobPtr                   FfnCGXCAPIFreeBlob;
	CGCAPI_PFXP7SignPtr                  FfnCGCAPI_PFXP7Sign;
	CGCAPI_CertGetSerialNumberPtr        FfnCGCAPI_CertGetSerialNumber;
	CGCAPI_CertGetSubjectWPtr            FfnCGCAPI_CertGetSubjectW;
	CGXCAPI_GetCertificatesPtr           FfnCGXCAPI_GetCertificates;
	CGCAPI_GetUserCertificatePtr         FfnCGCAPI_GetUserCertificate;
	CGCAPI_GetUserCertificatesPtr        FfnCGCAPI_GetUserCertificates;
	GetDLLVersionPtr                     FfnGetDLLVersion;
/*
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
*/
	UFC::AnsiString FOrganizationalUnit;
	UFC::AnsiString FCommonName;

public:
	CTSCGCCAObject(const UFC::AnsiString& CommonName, const UFC::AnsiString& DllFileName, UFC::BufferedLog* LogPtr);

	virtual bool LoadCADynamicLibrary();

};
#endif
