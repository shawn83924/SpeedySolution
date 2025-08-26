

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for C:\Users\User\AppData\Local\Temp\IDL5BA3.tmp:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include <rpc.h>
#include <rpcndr.h>
#include <combaseapi.h>

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __FSCAPIATL2_h__
#define __FSCAPIATL2_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFSCAPI_FWD_DEFINED__
#define __IFSCAPI_FWD_DEFINED__
typedef interface IFSCAPI IFSCAPI;

#endif 	/* __IFSCAPI_FWD_DEFINED__ */


#ifndef __FSCAPI_FWD_DEFINED__
#define __FSCAPI_FWD_DEFINED__

#ifdef __cplusplus
typedef class FSCAPI FSCAPI;
#else
typedef struct FSCAPI FSCAPI;
#endif /* __cplusplus */

#endif 	/* __FSCAPI_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __FSCAPIATLLib_LIBRARY_DEFINED__
#define __FSCAPIATLLib_LIBRARY_DEFINED__

/* library FSCAPIATLLib */
/* [custom][custom][helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_FSCAPIATLLib;

#ifndef __IFSCAPI_INTERFACE_DEFINED__
#define __IFSCAPI_INTERFACE_DEFINED__

/* interface IFSCAPI */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IFSCAPI;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("022AE37C-225D-4A8B-A4DF-798B86EB34AA")
    IFSCAPI : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISign( 
            /* [in] */ BSTR bData,
            /* [in] */ BSTR Subject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignature) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetCodePage( 
            /* [in] */ int codepage) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIVerify( 
            /* [in] */ BSTR strSignature,
            /* [in] */ BSTR bData,
            /* [in] */ BSTR strSubject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [in] */ int nTime,
            /* [retval][out] */ int *Return) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIEncrypt( 
            /* [in] */ BSTR strSubject,
            /* [in] */ int istore,
            /* [in] */ int iSelCertFlags,
            /* [in] */ BSTR bData,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *strCipher) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIDecrypt( 
            /* [in] */ BSTR strCipher,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *clear) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIExportPKCS12( 
            /* [in] */ BSTR strSubject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [in] */ BSTR strPassword,
            /* [in] */ BSTR strFilePath,
            /* [in] */ BSTR strFileName,
            /* [retval][out] */ int *RtnError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetErrorCode( 
            /* [retval][out] */ long *ErrorCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISymmetricEncrypt( 
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bData,
            /* [retval][out] */ BSTR *RtnCipher) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISymmetricDecrypt( 
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR cipher,
            /* [retval][out] */ BSTR *RtnClear) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSXCAPICertGetSerialNumber( 
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *SerialNumber) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CheckKeyUsage( 
            /* [in] */ int iCheck) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVerifyRtnSignerInfo( 
            /* [retval][out] */ BSTR *SignerInfo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetVerifyRtnCert( 
            /* [retval][out] */ BSTR *Cert) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPICertGetDigest( 
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *pstrDigest) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIShowCertificateDlg( 
            /* [in] */ BSTR strMsg,
            /* [in] */ BSTR strCert,
            /* [retval][out] */ int *iRtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISetDlgLogo( 
            /* [in] */ BSTR strLog) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSXCAPISetLanguage( 
            /* [in] */ BSTR strLanguage,
            /* [retval][out] */ int *Ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIImportPKCS12( 
            /* [in] */ BSTR strPwd,
            /* [in] */ BSTR strFilePath,
            /* [in] */ BSTR strFileName,
            /* [in] */ int iFlags,
            /* [retval][out] */ int *RetError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSXCAPIEncode( 
            /* [in] */ BSTR strData,
            /* [in] */ VARIANT strPemTitle,
            /* [retval][out] */ BSTR *strEncoded) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSXCAPIDecode( 
            /* [in] */ BSTR strEncoded,
            /* [in] */ VARIANT strPemTitle,
            /* [retval][out] */ BSTR *strDecoded) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPICertGetSubject( 
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strSubject) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPICertGetIssuer( 
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strIssuer) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPICertGetNotBefore( 
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strNotBefore) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPICertGetNotAfter( 
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strNotAfter) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSFCAPIP7Sign( 
            /* [in] */ BSTR strbDataFilePath,
            /* [in] */ BSTR strSubject,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [in] */ BSTR strSignatureFilePath,
            /* [retval][out] */ int *Ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSFCAPIP7Verify( 
            /* [in] */ BSTR strSignatureFilePath,
            /* [in] */ BSTR strbDataFilePath,
            /* [in] */ BSTR strSubject,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [in] */ int iTime,
            /* [in] */ BSTR strSignerCertFilePath,
            /* [retval][out] */ int *Ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSXCAPISetParentWindowHandle( 
            /* [in] */ int hParent) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPICertEncrypt( 
            /* [in] */ BSTR strCert,
            /* [in] */ BSTR bData,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strCipher) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISetDisplayMessage( 
            /* [in] */ BSTR strMessage) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIPureSign( 
            /* [in] */ BSTR strData,
            /* [in] */ BSTR strSubject,
            /* [in] */ int iFlags,
            /* [in] */ int iHashalg,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ int *Ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIGetRtnSignature( 
            /* [retval][out] */ BSTR *strSignature) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIGetRtnCert( 
            /* [retval][out] */ BSTR *strCert) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISignEx( 
            /* [in] */ BSTR strData,
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR strPin,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignature) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIRegisterCertificate( 
            /* [in] */ BSTR strProvider,
            /* [retval][out] */ long *RetError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSXCAPIPKCS7GetUserCertificate( 
            /* [in] */ BSTR p7cert,
            /* [retval][out] */ BSTR *x509) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSXCAPIGetCertSubject( 
            /* [in] */ BSTR Subject,
            /* [in] */ BSTR field,
            /* [in] */ int flag,
            /* [in] */ int keyUsage,
            /* [retval][out] */ VARIANT *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIShowDebugMessage( 
            /* [in] */ long bShow) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSXCAPIGetCertsEx( 
            /* [in] */ BSTR provider,
            /* [in] */ BSTR Subject,
            /* [in] */ int flag,
            /* [in] */ int keyUsage,
            /* [in] */ int disableATKeySpec,
            /* [retval][out] */ VARIANT *rtns) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIRegisterCertificateEx( 
            /* [in] */ BSTR strProvider,
            /* [in] */ long iDisableFlag,
            /* [retval][out] */ long *RetError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIFileSign( 
            /* [in] */ BSTR strFilePath,
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR strPin,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignature) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_MultiSignInit( 
            /* [in] */ BSTR strProvider,
            /* [in] */ int iFlags,
            /* [retval][out] */ int *Ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_MultiSignEnd( 
            /* [in] */ int iFlags) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSXCAPISelectCert( 
            /* [in] */ BSTR Subject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrCertSN) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSignRtnContent( 
            /* [retval][out] */ BSTR *strContent) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIFileEncryptByCert( 
            /* [in] */ BSTR strCert,
            /* [in] */ BSTR strFilePath,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strCipher) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIDecrypt2File( 
            /* [in] */ BSTR strCipher,
            /* [in] */ BSTR strOutFilePath,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ int *iRtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIPFXImportSC( 
            /* [in] */ BSTR sPFXfilepath,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR strCSP,
            /* [in] */ int iFlags,
            /* [retval][out] */ long *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ssInit( 
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ long *iRtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ssGetEncSessionKey( 
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *RtnCipher) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_UsrCertGetProvName( 
            /* [in] */ BSTR Subject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pStrProvName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_DeleteUsrCert( 
            /* [in] */ BSTR Subject,
            /* [in] */ int flag,
            /* [in] */ int keyUsage,
            /* [retval][out] */ long *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIAuth( 
            /* [in] */ BSTR strData,
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR strPin,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignature) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE purePKCS1Sign( 
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR strCertIssuer,
            /* [in] */ BSTR strCertSerial,
            /* [in] */ long iFlags,
            /* [in] */ long iKeyUsage,
            /* [in] */ BSTR strBase64Data,
            /* [in] */ long iSignFlags,
            /* [retval][out] */ BSTR *strBase64Signature) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_XGetUserCertificate( 
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR Subject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrBase64Cert) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_AcceptPKCS7( 
            /* [in] */ BSTR strCertificate,
            /* [in] */ long iFlags,
            /* [retval][out] */ long *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPICertGetPublicKey( 
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strPublicKey) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_ChangeP12Pwd( 
            /* [in] */ BSTR strPFXFilepath,
            /* [in] */ BSTR strOldPwd,
            /* [in] */ BSTR strNewPwd,
            /* [in] */ long iFlags,
            /* [retval][out] */ long *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_PFXSign( 
            /* [in] */ BSTR sPFXfilepath,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bData,
            /* [in] */ long iFlags,
            /* [in] */ long iKeyUsage,
            /* [retval][out] */ BSTR *strPKCS7) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_GetPFXCertCount( 
            /* [in] */ BSTR sPFXfilepath,
            /* [in] */ BSTR strPasswd,
            /* [retval][out] */ long *count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FS_GETVersion( 
            /* [in] */ long flag,
            /* [retval][out] */ BSTR *ver) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_ReadFile( 
            /* [in] */ BSTR strFilePath,
            /* [in] */ long flags,
            /* [retval][out] */ BSTR *bytesFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_GetFilePath( 
            /* [in] */ BSTR title,
            /* [in] */ BSTR typeDesc,
            /* [in] */ BSTR fileType,
            /* [in] */ BSTR initPath,
            /* [in] */ BSTR initFile,
            /* [retval][out] */ BSTR *filePath) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISignEx2( 
            /* [in] */ BSTR strData,
            /* [in] */ BSTR signcodepage,
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR bstrIssuerSubject,
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR strPin,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignature) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIVerifyEx2( 
            /* [in] */ BSTR strSignature,
            /* [in] */ BSTR bData,
            /* [in] */ BSTR signcodepage,
            /* [in] */ BSTR strSubject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [in] */ int nTime,
            /* [retval][out] */ int *Return) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_PKCS7GetSignerCert( 
            /* [in] */ BSTR pkcs7signature,
            /* [in] */ long index,
            /* [in] */ long flags,
            /* [retval][out] */ BSTR *Cert) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIGetVerifiedContent( 
            /* [in] */ BSTR pkcs7signature,
            /* [in] */ BSTR reserved,
            /* [in] */ long flags,
            /* [retval][out] */ BSTR *content) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIVerifiedContent2File( 
            /* [in] */ BSTR pkcs7signature,
            /* [in] */ BSTR reserved,
            /* [in] */ long flags,
            /* [in] */ BSTR outfile,
            /* [retval][out] */ long *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIComposePKCS7( 
            /* [in] */ BSTR strCert,
            /* [in] */ BSTR B64RawSignature,
            /* [in] */ BSTR strData,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *pstrSignature) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSGetTempPath( 
            /* [retval][out] */ BSTR *path) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_HashAlgorithm( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_HashAlgorithm( 
            /* [in] */ BSTR pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_providerType( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_providerType( 
            /* [in] */ long pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_providerName( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_providerName( 
            /* [in] */ BSTR pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_KeySpec( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_KeySpec( 
            /* [in] */ long pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_GenKeyFlags( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_GenKeyFlags( 
            /* [in] */ long pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_DeleteRequestCert( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_DeleteRequestCert( 
            /* [in] */ long pVal) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_WriteCertToCSP( 
            /* [retval][out] */ long *pVal) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_WriteCertToCSP( 
            /* [in] */ long pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE enumProviders( 
            /* [in] */ long dwIndex,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *pbstrProvName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE acceptPKCS7( 
            /* [in] */ BSTR strCert) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE createPKCS10( 
            /* [in] */ BSTR dn,
            /* [in] */ BSTR oid,
            /* [retval][out] */ BSTR *strPKCS10) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_PFXSign2( 
            /* [in] */ BSTR sB64PFX,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bData,
            /* [in] */ long iFlags,
            /* [in] */ long iKeyUsage,
            /* [retval][out] */ BSTR *strPKCS7) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_CheckPFX( 
            /* [in] */ BSTR sB64PFX,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR strSubject,
            /* [in] */ long iFlags,
            /* [in] */ long iKeyUsage,
            /* [retval][out] */ long *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_DeleteUsrCertEx( 
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR Subject,
            /* [in] */ int flag,
            /* [in] */ int keyUsage,
            /* [retval][out] */ long *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_CheckPermission( 
            /* [retval][out] */ long *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIPureVerify( 
            /* [in] */ BSTR strSignature,
            /* [in] */ BSTR strData,
            /* [in] */ BSTR strSubject,
            /* [in] */ int iFlags,
            /* [in] */ int iHashalg,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ int *Ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISetDisplayMessageFont( 
            /* [in] */ BSTR font,
            /* [in] */ int size) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetURL( 
            /* [retval][out] */ BSTR *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISignAndEncrypt( 
            /* [in] */ BSTR bSignerSubject,
            /* [in] */ BSTR bEncryptCert,
            /* [in] */ BSTR bData,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignCipher) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIDecryptAndVerify( 
            /* [in] */ BSTR bCipher,
            /* [in] */ int iSignerIndex,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *pstrClear) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDecryptRtnCert( 
            /* [retval][out] */ BSTR *Cert) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_PFXPureSign( 
            /* [in] */ BSTR sPFXfilepath,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bData,
            /* [in] */ int iFlags,
            /* [in] */ int iHashalg,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ int *Ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ BSTR bInitCode,
            /* [in] */ BSTR bInitPath,
            /* [retval][out] */ int *Ret) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIUnRegisterCertificateEx( 
            /* [in] */ BSTR strProvider,
            /* [in] */ long iFlags,
            /* [retval][out] */ long *RetError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE acceptPKCS7Ex( 
            /* [in] */ BSTR bCert,
            /* [in] */ BSTR bPIN) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISymmetricRawKeyEncrypt( 
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strRawKey,
            /* [in] */ BSTR strRawIV,
            /* [in] */ BSTR bData,
            /* [retval][out] */ BSTR *RtnCipher) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISymmetricRawKeyDecrypt( 
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strRawKey,
            /* [in] */ BSTR strRawIV,
            /* [in] */ BSTR bCipher,
            /* [retval][out] */ BSTR *RtnClear) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIPKCS7GetEncryptedDigest( 
            /* [in] */ BSTR pkcs7signature,
            /* [in] */ long flags,
            /* [retval][out] */ BSTR *content) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_PFXSignEx( 
            /* [in] */ BSTR bPFXfilepath,
            /* [in] */ BSTR bPFXPassWord,
            /* [in] */ BSTR bSubject,
            /* [in] */ BSTR bData,
            /* [in] */ int iSignFlags,
            /* [in] */ int iKeyUsage,
            /* [in] */ int iImportP12Flags,
            /* [retval][out] */ BSTR *pstrSignature) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_PFXSignEx1( 
            /* [in] */ BSTR sPFXfilepath,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bData,
            /* [in] */ long iFlags,
            /* [in] */ long iKeyUsage,
            /* [retval][out] */ BSTR *strPKCS7) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPIP12XGetUserCertificate( 
            /* [in] */ BSTR strPwd,
            /* [in] */ BSTR strFilePath,
            /* [in] */ BSTR strFileName,
            /* [retval][out] */ BSTR *pstrBase64Cert) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_InstallCert( 
            /* [in] */ BSTR strCertificate,
            /* [in] */ long iFlags,
            /* [retval][out] */ long *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_WriteFile( 
            /* [in] */ BSTR strB64Data,
            /* [in] */ BSTR strFilePath,
            /* [in] */ long iFlags,
            /* [retval][out] */ long *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSGetMyDocumentsPath( 
            /* [retval][out] */ BSTR *path) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISymmetricEncryptFile( 
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bSrcPath,
            /* [in] */ BSTR bDstPath,
            /* [retval][out] */ int *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPISymmetricDecryptFile( 
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bSrcPath,
            /* [in] */ BSTR bDstPath,
            /* [retval][out] */ int *rtn) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FSCAPI_UsrCertGetContainerName( 
            /* [in] */ BSTR Subject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pStrContainerName) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IFSCAPIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFSCAPI * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFSCAPI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFSCAPI * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFSCAPI * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFSCAPI * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFSCAPI * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFSCAPI * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISign )( 
            IFSCAPI * This,
            /* [in] */ BSTR bData,
            /* [in] */ BSTR Subject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignature);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetCodePage )( 
            IFSCAPI * This,
            /* [in] */ int codepage);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIVerify )( 
            IFSCAPI * This,
            /* [in] */ BSTR strSignature,
            /* [in] */ BSTR bData,
            /* [in] */ BSTR strSubject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [in] */ int nTime,
            /* [retval][out] */ int *Return);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIEncrypt )( 
            IFSCAPI * This,
            /* [in] */ BSTR strSubject,
            /* [in] */ int istore,
            /* [in] */ int iSelCertFlags,
            /* [in] */ BSTR bData,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *strCipher);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIDecrypt )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCipher,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *clear);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIExportPKCS12 )( 
            IFSCAPI * This,
            /* [in] */ BSTR strSubject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [in] */ BSTR strPassword,
            /* [in] */ BSTR strFilePath,
            /* [in] */ BSTR strFileName,
            /* [retval][out] */ int *RtnError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetErrorCode )( 
            IFSCAPI * This,
            /* [retval][out] */ long *ErrorCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISymmetricEncrypt )( 
            IFSCAPI * This,
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bData,
            /* [retval][out] */ BSTR *RtnCipher);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISymmetricDecrypt )( 
            IFSCAPI * This,
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR cipher,
            /* [retval][out] */ BSTR *RtnClear);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSXCAPICertGetSerialNumber )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *SerialNumber);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CheckKeyUsage )( 
            IFSCAPI * This,
            /* [in] */ int iCheck);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetVerifyRtnSignerInfo )( 
            IFSCAPI * This,
            /* [retval][out] */ BSTR *SignerInfo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetVerifyRtnCert )( 
            IFSCAPI * This,
            /* [retval][out] */ BSTR *Cert);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPICertGetDigest )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *pstrDigest);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIShowCertificateDlg )( 
            IFSCAPI * This,
            /* [in] */ BSTR strMsg,
            /* [in] */ BSTR strCert,
            /* [retval][out] */ int *iRtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISetDlgLogo )( 
            IFSCAPI * This,
            /* [in] */ BSTR strLog);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSXCAPISetLanguage )( 
            IFSCAPI * This,
            /* [in] */ BSTR strLanguage,
            /* [retval][out] */ int *Ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIImportPKCS12 )( 
            IFSCAPI * This,
            /* [in] */ BSTR strPwd,
            /* [in] */ BSTR strFilePath,
            /* [in] */ BSTR strFileName,
            /* [in] */ int iFlags,
            /* [retval][out] */ int *RetError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSXCAPIEncode )( 
            IFSCAPI * This,
            /* [in] */ BSTR strData,
            /* [in] */ VARIANT strPemTitle,
            /* [retval][out] */ BSTR *strEncoded);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSXCAPIDecode )( 
            IFSCAPI * This,
            /* [in] */ BSTR strEncoded,
            /* [in] */ VARIANT strPemTitle,
            /* [retval][out] */ BSTR *strDecoded);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPICertGetSubject )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strSubject);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPICertGetIssuer )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strIssuer);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPICertGetNotBefore )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strNotBefore);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPICertGetNotAfter )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strNotAfter);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSFCAPIP7Sign )( 
            IFSCAPI * This,
            /* [in] */ BSTR strbDataFilePath,
            /* [in] */ BSTR strSubject,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [in] */ BSTR strSignatureFilePath,
            /* [retval][out] */ int *Ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSFCAPIP7Verify )( 
            IFSCAPI * This,
            /* [in] */ BSTR strSignatureFilePath,
            /* [in] */ BSTR strbDataFilePath,
            /* [in] */ BSTR strSubject,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [in] */ int iTime,
            /* [in] */ BSTR strSignerCertFilePath,
            /* [retval][out] */ int *Ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSXCAPISetParentWindowHandle )( 
            IFSCAPI * This,
            /* [in] */ int hParent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPICertEncrypt )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert,
            /* [in] */ BSTR bData,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strCipher);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISetDisplayMessage )( 
            IFSCAPI * This,
            /* [in] */ BSTR strMessage);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIPureSign )( 
            IFSCAPI * This,
            /* [in] */ BSTR strData,
            /* [in] */ BSTR strSubject,
            /* [in] */ int iFlags,
            /* [in] */ int iHashalg,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ int *Ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIGetRtnSignature )( 
            IFSCAPI * This,
            /* [retval][out] */ BSTR *strSignature);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIGetRtnCert )( 
            IFSCAPI * This,
            /* [retval][out] */ BSTR *strCert);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISignEx )( 
            IFSCAPI * This,
            /* [in] */ BSTR strData,
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR strPin,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignature);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIRegisterCertificate )( 
            IFSCAPI * This,
            /* [in] */ BSTR strProvider,
            /* [retval][out] */ long *RetError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSXCAPIPKCS7GetUserCertificate )( 
            IFSCAPI * This,
            /* [in] */ BSTR p7cert,
            /* [retval][out] */ BSTR *x509);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSXCAPIGetCertSubject )( 
            IFSCAPI * This,
            /* [in] */ BSTR Subject,
            /* [in] */ BSTR field,
            /* [in] */ int flag,
            /* [in] */ int keyUsage,
            /* [retval][out] */ VARIANT *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIShowDebugMessage )( 
            IFSCAPI * This,
            /* [in] */ long bShow);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSXCAPIGetCertsEx )( 
            IFSCAPI * This,
            /* [in] */ BSTR provider,
            /* [in] */ BSTR Subject,
            /* [in] */ int flag,
            /* [in] */ int keyUsage,
            /* [in] */ int disableATKeySpec,
            /* [retval][out] */ VARIANT *rtns);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIRegisterCertificateEx )( 
            IFSCAPI * This,
            /* [in] */ BSTR strProvider,
            /* [in] */ long iDisableFlag,
            /* [retval][out] */ long *RetError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIFileSign )( 
            IFSCAPI * This,
            /* [in] */ BSTR strFilePath,
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR strPin,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignature);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_MultiSignInit )( 
            IFSCAPI * This,
            /* [in] */ BSTR strProvider,
            /* [in] */ int iFlags,
            /* [retval][out] */ int *Ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_MultiSignEnd )( 
            IFSCAPI * This,
            /* [in] */ int iFlags);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSXCAPISelectCert )( 
            IFSCAPI * This,
            /* [in] */ BSTR Subject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrCertSN);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSignRtnContent )( 
            IFSCAPI * This,
            /* [retval][out] */ BSTR *strContent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIFileEncryptByCert )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert,
            /* [in] */ BSTR strFilePath,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strCipher);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIDecrypt2File )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCipher,
            /* [in] */ BSTR strOutFilePath,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ int *iRtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIPFXImportSC )( 
            IFSCAPI * This,
            /* [in] */ BSTR sPFXfilepath,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR strCSP,
            /* [in] */ int iFlags,
            /* [retval][out] */ long *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ssInit )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ long *iRtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ssGetEncSessionKey )( 
            IFSCAPI * This,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *RtnCipher);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_UsrCertGetProvName )( 
            IFSCAPI * This,
            /* [in] */ BSTR Subject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pStrProvName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_DeleteUsrCert )( 
            IFSCAPI * This,
            /* [in] */ BSTR Subject,
            /* [in] */ int flag,
            /* [in] */ int keyUsage,
            /* [retval][out] */ long *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIAuth )( 
            IFSCAPI * This,
            /* [in] */ BSTR strData,
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR strPin,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignature);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *purePKCS1Sign )( 
            IFSCAPI * This,
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR strCertIssuer,
            /* [in] */ BSTR strCertSerial,
            /* [in] */ long iFlags,
            /* [in] */ long iKeyUsage,
            /* [in] */ BSTR strBase64Data,
            /* [in] */ long iSignFlags,
            /* [retval][out] */ BSTR *strBase64Signature);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_XGetUserCertificate )( 
            IFSCAPI * This,
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR Subject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrBase64Cert);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_AcceptPKCS7 )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCertificate,
            /* [in] */ long iFlags,
            /* [retval][out] */ long *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPICertGetPublicKey )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *strPublicKey);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_ChangeP12Pwd )( 
            IFSCAPI * This,
            /* [in] */ BSTR strPFXFilepath,
            /* [in] */ BSTR strOldPwd,
            /* [in] */ BSTR strNewPwd,
            /* [in] */ long iFlags,
            /* [retval][out] */ long *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_PFXSign )( 
            IFSCAPI * This,
            /* [in] */ BSTR sPFXfilepath,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bData,
            /* [in] */ long iFlags,
            /* [in] */ long iKeyUsage,
            /* [retval][out] */ BSTR *strPKCS7);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_GetPFXCertCount )( 
            IFSCAPI * This,
            /* [in] */ BSTR sPFXfilepath,
            /* [in] */ BSTR strPasswd,
            /* [retval][out] */ long *count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FS_GETVersion )( 
            IFSCAPI * This,
            /* [in] */ long flag,
            /* [retval][out] */ BSTR *ver);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_ReadFile )( 
            IFSCAPI * This,
            /* [in] */ BSTR strFilePath,
            /* [in] */ long flags,
            /* [retval][out] */ BSTR *bytesFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_GetFilePath )( 
            IFSCAPI * This,
            /* [in] */ BSTR title,
            /* [in] */ BSTR typeDesc,
            /* [in] */ BSTR fileType,
            /* [in] */ BSTR initPath,
            /* [in] */ BSTR initFile,
            /* [retval][out] */ BSTR *filePath);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISignEx2 )( 
            IFSCAPI * This,
            /* [in] */ BSTR strData,
            /* [in] */ BSTR signcodepage,
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR bstrIssuerSubject,
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR strPin,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignature);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIVerifyEx2 )( 
            IFSCAPI * This,
            /* [in] */ BSTR strSignature,
            /* [in] */ BSTR bData,
            /* [in] */ BSTR signcodepage,
            /* [in] */ BSTR strSubject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [in] */ int nTime,
            /* [retval][out] */ int *Return);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_PKCS7GetSignerCert )( 
            IFSCAPI * This,
            /* [in] */ BSTR pkcs7signature,
            /* [in] */ long index,
            /* [in] */ long flags,
            /* [retval][out] */ BSTR *Cert);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIGetVerifiedContent )( 
            IFSCAPI * This,
            /* [in] */ BSTR pkcs7signature,
            /* [in] */ BSTR reserved,
            /* [in] */ long flags,
            /* [retval][out] */ BSTR *content);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIVerifiedContent2File )( 
            IFSCAPI * This,
            /* [in] */ BSTR pkcs7signature,
            /* [in] */ BSTR reserved,
            /* [in] */ long flags,
            /* [in] */ BSTR outfile,
            /* [retval][out] */ long *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIComposePKCS7 )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert,
            /* [in] */ BSTR B64RawSignature,
            /* [in] */ BSTR strData,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *pstrSignature);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSGetTempPath )( 
            IFSCAPI * This,
            /* [retval][out] */ BSTR *path);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_HashAlgorithm )( 
            IFSCAPI * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_HashAlgorithm )( 
            IFSCAPI * This,
            /* [in] */ BSTR pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_providerType )( 
            IFSCAPI * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_providerType )( 
            IFSCAPI * This,
            /* [in] */ long pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_providerName )( 
            IFSCAPI * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_providerName )( 
            IFSCAPI * This,
            /* [in] */ BSTR pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_KeySpec )( 
            IFSCAPI * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_KeySpec )( 
            IFSCAPI * This,
            /* [in] */ long pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_GenKeyFlags )( 
            IFSCAPI * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_GenKeyFlags )( 
            IFSCAPI * This,
            /* [in] */ long pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_DeleteRequestCert )( 
            IFSCAPI * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_DeleteRequestCert )( 
            IFSCAPI * This,
            /* [in] */ long pVal);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_WriteCertToCSP )( 
            IFSCAPI * This,
            /* [retval][out] */ long *pVal);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_WriteCertToCSP )( 
            IFSCAPI * This,
            /* [in] */ long pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *enumProviders )( 
            IFSCAPI * This,
            /* [in] */ long dwIndex,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *pbstrProvName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *acceptPKCS7 )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCert);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *createPKCS10 )( 
            IFSCAPI * This,
            /* [in] */ BSTR dn,
            /* [in] */ BSTR oid,
            /* [retval][out] */ BSTR *strPKCS10);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_PFXSign2 )( 
            IFSCAPI * This,
            /* [in] */ BSTR sB64PFX,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bData,
            /* [in] */ long iFlags,
            /* [in] */ long iKeyUsage,
            /* [retval][out] */ BSTR *strPKCS7);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_CheckPFX )( 
            IFSCAPI * This,
            /* [in] */ BSTR sB64PFX,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR strSubject,
            /* [in] */ long iFlags,
            /* [in] */ long iKeyUsage,
            /* [retval][out] */ long *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_DeleteUsrCertEx )( 
            IFSCAPI * This,
            /* [in] */ BSTR strProvider,
            /* [in] */ BSTR Subject,
            /* [in] */ int flag,
            /* [in] */ int keyUsage,
            /* [retval][out] */ long *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_CheckPermission )( 
            IFSCAPI * This,
            /* [retval][out] */ long *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIPureVerify )( 
            IFSCAPI * This,
            /* [in] */ BSTR strSignature,
            /* [in] */ BSTR strData,
            /* [in] */ BSTR strSubject,
            /* [in] */ int iFlags,
            /* [in] */ int iHashalg,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ int *Ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISetDisplayMessageFont )( 
            IFSCAPI * This,
            /* [in] */ BSTR font,
            /* [in] */ int size);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetURL )( 
            IFSCAPI * This,
            /* [retval][out] */ BSTR *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISignAndEncrypt )( 
            IFSCAPI * This,
            /* [in] */ BSTR bSignerSubject,
            /* [in] */ BSTR bEncryptCert,
            /* [in] */ BSTR bData,
            /* [in] */ int iFlags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pstrSignCipher);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIDecryptAndVerify )( 
            IFSCAPI * This,
            /* [in] */ BSTR bCipher,
            /* [in] */ int iSignerIndex,
            /* [in] */ int iFlags,
            /* [retval][out] */ BSTR *pstrClear);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDecryptRtnCert )( 
            IFSCAPI * This,
            /* [retval][out] */ BSTR *Cert);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_PFXPureSign )( 
            IFSCAPI * This,
            /* [in] */ BSTR sPFXfilepath,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bData,
            /* [in] */ int iFlags,
            /* [in] */ int iHashalg,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ int *Ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Init )( 
            IFSCAPI * This,
            /* [in] */ BSTR bInitCode,
            /* [in] */ BSTR bInitPath,
            /* [retval][out] */ int *Ret);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIUnRegisterCertificateEx )( 
            IFSCAPI * This,
            /* [in] */ BSTR strProvider,
            /* [in] */ long iFlags,
            /* [retval][out] */ long *RetError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *acceptPKCS7Ex )( 
            IFSCAPI * This,
            /* [in] */ BSTR bCert,
            /* [in] */ BSTR bPIN);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISymmetricRawKeyEncrypt )( 
            IFSCAPI * This,
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strRawKey,
            /* [in] */ BSTR strRawIV,
            /* [in] */ BSTR bData,
            /* [retval][out] */ BSTR *RtnCipher);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISymmetricRawKeyDecrypt )( 
            IFSCAPI * This,
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strRawKey,
            /* [in] */ BSTR strRawIV,
            /* [in] */ BSTR bCipher,
            /* [retval][out] */ BSTR *RtnClear);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIPKCS7GetEncryptedDigest )( 
            IFSCAPI * This,
            /* [in] */ BSTR pkcs7signature,
            /* [in] */ long flags,
            /* [retval][out] */ BSTR *content);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_PFXSignEx )( 
            IFSCAPI * This,
            /* [in] */ BSTR bPFXfilepath,
            /* [in] */ BSTR bPFXPassWord,
            /* [in] */ BSTR bSubject,
            /* [in] */ BSTR bData,
            /* [in] */ int iSignFlags,
            /* [in] */ int iKeyUsage,
            /* [in] */ int iImportP12Flags,
            /* [retval][out] */ BSTR *pstrSignature);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_PFXSignEx1 )( 
            IFSCAPI * This,
            /* [in] */ BSTR sPFXfilepath,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bData,
            /* [in] */ long iFlags,
            /* [in] */ long iKeyUsage,
            /* [retval][out] */ BSTR *strPKCS7);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPIP12XGetUserCertificate )( 
            IFSCAPI * This,
            /* [in] */ BSTR strPwd,
            /* [in] */ BSTR strFilePath,
            /* [in] */ BSTR strFileName,
            /* [retval][out] */ BSTR *pstrBase64Cert);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_InstallCert )( 
            IFSCAPI * This,
            /* [in] */ BSTR strCertificate,
            /* [in] */ long iFlags,
            /* [retval][out] */ long *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_WriteFile )( 
            IFSCAPI * This,
            /* [in] */ BSTR strB64Data,
            /* [in] */ BSTR strFilePath,
            /* [in] */ long iFlags,
            /* [retval][out] */ long *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSGetMyDocumentsPath )( 
            IFSCAPI * This,
            /* [retval][out] */ BSTR *path);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISymmetricEncryptFile )( 
            IFSCAPI * This,
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bSrcPath,
            /* [in] */ BSTR bDstPath,
            /* [retval][out] */ int *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPISymmetricDecryptFile )( 
            IFSCAPI * This,
            /* [in] */ int iAlgor,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR bSrcPath,
            /* [in] */ BSTR bDstPath,
            /* [retval][out] */ int *rtn);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FSCAPI_UsrCertGetContainerName )( 
            IFSCAPI * This,
            /* [in] */ BSTR Subject,
            /* [in] */ int flags,
            /* [in] */ int iKeyUsage,
            /* [retval][out] */ BSTR *pStrContainerName);
        
        END_INTERFACE
    } IFSCAPIVtbl;

    interface IFSCAPI
    {
        CONST_VTBL struct IFSCAPIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFSCAPI_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFSCAPI_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFSCAPI_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFSCAPI_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IFSCAPI_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IFSCAPI_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IFSCAPI_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IFSCAPI_FSCAPISign(This,bData,Subject,flags,iKeyUsage,pstrSignature)	\
    ( (This)->lpVtbl -> FSCAPISign(This,bData,Subject,flags,iKeyUsage,pstrSignature) ) 

#define IFSCAPI_SetCodePage(This,codepage)	\
    ( (This)->lpVtbl -> SetCodePage(This,codepage) ) 

#define IFSCAPI_FSCAPIVerify(This,strSignature,bData,strSubject,flags,iKeyUsage,nTime,Return)	\
    ( (This)->lpVtbl -> FSCAPIVerify(This,strSignature,bData,strSubject,flags,iKeyUsage,nTime,Return) ) 

#define IFSCAPI_FSCAPIEncrypt(This,strSubject,istore,iSelCertFlags,bData,flags,iKeyUsage,strCipher)	\
    ( (This)->lpVtbl -> FSCAPIEncrypt(This,strSubject,istore,iSelCertFlags,bData,flags,iKeyUsage,strCipher) ) 

#define IFSCAPI_FSCAPIDecrypt(This,strCipher,flags,iKeyUsage,clear)	\
    ( (This)->lpVtbl -> FSCAPIDecrypt(This,strCipher,flags,iKeyUsage,clear) ) 

#define IFSCAPI_FSCAPIExportPKCS12(This,strSubject,flags,iKeyUsage,strPassword,strFilePath,strFileName,RtnError)	\
    ( (This)->lpVtbl -> FSCAPIExportPKCS12(This,strSubject,flags,iKeyUsage,strPassword,strFilePath,strFileName,RtnError) ) 

#define IFSCAPI_GetErrorCode(This,ErrorCode)	\
    ( (This)->lpVtbl -> GetErrorCode(This,ErrorCode) ) 

#define IFSCAPI_FSCAPISymmetricEncrypt(This,iAlgor,strPasswd,bData,RtnCipher)	\
    ( (This)->lpVtbl -> FSCAPISymmetricEncrypt(This,iAlgor,strPasswd,bData,RtnCipher) ) 

#define IFSCAPI_FSCAPISymmetricDecrypt(This,iAlgor,strPasswd,cipher,RtnClear)	\
    ( (This)->lpVtbl -> FSCAPISymmetricDecrypt(This,iAlgor,strPasswd,cipher,RtnClear) ) 

#define IFSCAPI_FSXCAPICertGetSerialNumber(This,strCert,iFlags,SerialNumber)	\
    ( (This)->lpVtbl -> FSXCAPICertGetSerialNumber(This,strCert,iFlags,SerialNumber) ) 

#define IFSCAPI_CheckKeyUsage(This,iCheck)	\
    ( (This)->lpVtbl -> CheckKeyUsage(This,iCheck) ) 

#define IFSCAPI_GetVerifyRtnSignerInfo(This,SignerInfo)	\
    ( (This)->lpVtbl -> GetVerifyRtnSignerInfo(This,SignerInfo) ) 

#define IFSCAPI_GetVerifyRtnCert(This,Cert)	\
    ( (This)->lpVtbl -> GetVerifyRtnCert(This,Cert) ) 

#define IFSCAPI_FSCAPICertGetDigest(This,strCert,iFlags,pstrDigest)	\
    ( (This)->lpVtbl -> FSCAPICertGetDigest(This,strCert,iFlags,pstrDigest) ) 

#define IFSCAPI_FSCAPIShowCertificateDlg(This,strMsg,strCert,iRtn)	\
    ( (This)->lpVtbl -> FSCAPIShowCertificateDlg(This,strMsg,strCert,iRtn) ) 

#define IFSCAPI_FSCAPISetDlgLogo(This,strLog)	\
    ( (This)->lpVtbl -> FSCAPISetDlgLogo(This,strLog) ) 

#define IFSCAPI_FSXCAPISetLanguage(This,strLanguage,Ret)	\
    ( (This)->lpVtbl -> FSXCAPISetLanguage(This,strLanguage,Ret) ) 

#define IFSCAPI_FSCAPIImportPKCS12(This,strPwd,strFilePath,strFileName,iFlags,RetError)	\
    ( (This)->lpVtbl -> FSCAPIImportPKCS12(This,strPwd,strFilePath,strFileName,iFlags,RetError) ) 

#define IFSCAPI_FSXCAPIEncode(This,strData,strPemTitle,strEncoded)	\
    ( (This)->lpVtbl -> FSXCAPIEncode(This,strData,strPemTitle,strEncoded) ) 

#define IFSCAPI_FSXCAPIDecode(This,strEncoded,strPemTitle,strDecoded)	\
    ( (This)->lpVtbl -> FSXCAPIDecode(This,strEncoded,strPemTitle,strDecoded) ) 

#define IFSCAPI_FSCAPICertGetSubject(This,strCert,iFlags,strSubject)	\
    ( (This)->lpVtbl -> FSCAPICertGetSubject(This,strCert,iFlags,strSubject) ) 

#define IFSCAPI_FSCAPICertGetIssuer(This,strCert,iFlags,strIssuer)	\
    ( (This)->lpVtbl -> FSCAPICertGetIssuer(This,strCert,iFlags,strIssuer) ) 

#define IFSCAPI_FSCAPICertGetNotBefore(This,strCert,iFlags,strNotBefore)	\
    ( (This)->lpVtbl -> FSCAPICertGetNotBefore(This,strCert,iFlags,strNotBefore) ) 

#define IFSCAPI_FSCAPICertGetNotAfter(This,strCert,iFlags,strNotAfter)	\
    ( (This)->lpVtbl -> FSCAPICertGetNotAfter(This,strCert,iFlags,strNotAfter) ) 

#define IFSCAPI_FSFCAPIP7Sign(This,strbDataFilePath,strSubject,iFlags,iKeyUsage,strSignatureFilePath,Ret)	\
    ( (This)->lpVtbl -> FSFCAPIP7Sign(This,strbDataFilePath,strSubject,iFlags,iKeyUsage,strSignatureFilePath,Ret) ) 

#define IFSCAPI_FSFCAPIP7Verify(This,strSignatureFilePath,strbDataFilePath,strSubject,iFlags,iKeyUsage,iTime,strSignerCertFilePath,Ret)	\
    ( (This)->lpVtbl -> FSFCAPIP7Verify(This,strSignatureFilePath,strbDataFilePath,strSubject,iFlags,iKeyUsage,iTime,strSignerCertFilePath,Ret) ) 

#define IFSCAPI_FSXCAPISetParentWindowHandle(This,hParent)	\
    ( (This)->lpVtbl -> FSXCAPISetParentWindowHandle(This,hParent) ) 

#define IFSCAPI_FSCAPICertEncrypt(This,strCert,bData,iFlags,strCipher)	\
    ( (This)->lpVtbl -> FSCAPICertEncrypt(This,strCert,bData,iFlags,strCipher) ) 

#define IFSCAPI_FSCAPISetDisplayMessage(This,strMessage)	\
    ( (This)->lpVtbl -> FSCAPISetDisplayMessage(This,strMessage) ) 

#define IFSCAPI_FSCAPIPureSign(This,strData,strSubject,iFlags,iHashalg,iKeyUsage,Ret)	\
    ( (This)->lpVtbl -> FSCAPIPureSign(This,strData,strSubject,iFlags,iHashalg,iKeyUsage,Ret) ) 

#define IFSCAPI_FSCAPIGetRtnSignature(This,strSignature)	\
    ( (This)->lpVtbl -> FSCAPIGetRtnSignature(This,strSignature) ) 

#define IFSCAPI_FSCAPIGetRtnCert(This,strCert)	\
    ( (This)->lpVtbl -> FSCAPIGetRtnCert(This,strCert) ) 

#define IFSCAPI_FSCAPISignEx(This,strData,strSubject,strProvider,strPin,iFlags,iKeyUsage,pstrSignature)	\
    ( (This)->lpVtbl -> FSCAPISignEx(This,strData,strSubject,strProvider,strPin,iFlags,iKeyUsage,pstrSignature) ) 

#define IFSCAPI_FSCAPIRegisterCertificate(This,strProvider,RetError)	\
    ( (This)->lpVtbl -> FSCAPIRegisterCertificate(This,strProvider,RetError) ) 

#define IFSCAPI_FSXCAPIPKCS7GetUserCertificate(This,p7cert,x509)	\
    ( (This)->lpVtbl -> FSXCAPIPKCS7GetUserCertificate(This,p7cert,x509) ) 

#define IFSCAPI_FSXCAPIGetCertSubject(This,Subject,field,flag,keyUsage,rtn)	\
    ( (This)->lpVtbl -> FSXCAPIGetCertSubject(This,Subject,field,flag,keyUsage,rtn) ) 

#define IFSCAPI_FSCAPIShowDebugMessage(This,bShow)	\
    ( (This)->lpVtbl -> FSCAPIShowDebugMessage(This,bShow) ) 

#define IFSCAPI_FSXCAPIGetCertsEx(This,provider,Subject,flag,keyUsage,disableATKeySpec,rtns)	\
    ( (This)->lpVtbl -> FSXCAPIGetCertsEx(This,provider,Subject,flag,keyUsage,disableATKeySpec,rtns) ) 

#define IFSCAPI_FSCAPIRegisterCertificateEx(This,strProvider,iDisableFlag,RetError)	\
    ( (This)->lpVtbl -> FSCAPIRegisterCertificateEx(This,strProvider,iDisableFlag,RetError) ) 

#define IFSCAPI_FSCAPIFileSign(This,strFilePath,strSubject,strProvider,strPin,iFlags,iKeyUsage,pstrSignature)	\
    ( (This)->lpVtbl -> FSCAPIFileSign(This,strFilePath,strSubject,strProvider,strPin,iFlags,iKeyUsage,pstrSignature) ) 

#define IFSCAPI_FSCAPI_MultiSignInit(This,strProvider,iFlags,Ret)	\
    ( (This)->lpVtbl -> FSCAPI_MultiSignInit(This,strProvider,iFlags,Ret) ) 

#define IFSCAPI_FSCAPI_MultiSignEnd(This,iFlags)	\
    ( (This)->lpVtbl -> FSCAPI_MultiSignEnd(This,iFlags) ) 

#define IFSCAPI_FSXCAPISelectCert(This,Subject,flags,iKeyUsage,pstrCertSN)	\
    ( (This)->lpVtbl -> FSXCAPISelectCert(This,Subject,flags,iKeyUsage,pstrCertSN) ) 

#define IFSCAPI_GetSignRtnContent(This,strContent)	\
    ( (This)->lpVtbl -> GetSignRtnContent(This,strContent) ) 

#define IFSCAPI_FSCAPIFileEncryptByCert(This,strCert,strFilePath,iFlags,strCipher)	\
    ( (This)->lpVtbl -> FSCAPIFileEncryptByCert(This,strCert,strFilePath,iFlags,strCipher) ) 

#define IFSCAPI_FSCAPIDecrypt2File(This,strCipher,strOutFilePath,flags,iKeyUsage,iRtn)	\
    ( (This)->lpVtbl -> FSCAPIDecrypt2File(This,strCipher,strOutFilePath,flags,iKeyUsage,iRtn) ) 

#define IFSCAPI_FSCAPIPFXImportSC(This,sPFXfilepath,strPasswd,strCSP,iFlags,rtn)	\
    ( (This)->lpVtbl -> FSCAPIPFXImportSC(This,sPFXfilepath,strPasswd,strCSP,iFlags,rtn) ) 

#define IFSCAPI_ssInit(This,strCert,iFlags,iRtn)	\
    ( (This)->lpVtbl -> ssInit(This,strCert,iFlags,iRtn) ) 

#define IFSCAPI_ssGetEncSessionKey(This,iFlags,RtnCipher)	\
    ( (This)->lpVtbl -> ssGetEncSessionKey(This,iFlags,RtnCipher) ) 

#define IFSCAPI_FSCAPI_UsrCertGetProvName(This,Subject,flags,iKeyUsage,pStrProvName)	\
    ( (This)->lpVtbl -> FSCAPI_UsrCertGetProvName(This,Subject,flags,iKeyUsage,pStrProvName) ) 

#define IFSCAPI_FSCAPI_DeleteUsrCert(This,Subject,flag,keyUsage,rtn)	\
    ( (This)->lpVtbl -> FSCAPI_DeleteUsrCert(This,Subject,flag,keyUsage,rtn) ) 

#define IFSCAPI_FSCAPIAuth(This,strData,strSubject,strProvider,strPin,iFlags,iKeyUsage,pstrSignature)	\
    ( (This)->lpVtbl -> FSCAPIAuth(This,strData,strSubject,strProvider,strPin,iFlags,iKeyUsage,pstrSignature) ) 

#define IFSCAPI_purePKCS1Sign(This,strProvider,strCertIssuer,strCertSerial,iFlags,iKeyUsage,strBase64Data,iSignFlags,strBase64Signature)	\
    ( (This)->lpVtbl -> purePKCS1Sign(This,strProvider,strCertIssuer,strCertSerial,iFlags,iKeyUsage,strBase64Data,iSignFlags,strBase64Signature) ) 

#define IFSCAPI_FSCAPI_XGetUserCertificate(This,strProvider,Subject,flags,iKeyUsage,pstrBase64Cert)	\
    ( (This)->lpVtbl -> FSCAPI_XGetUserCertificate(This,strProvider,Subject,flags,iKeyUsage,pstrBase64Cert) ) 

#define IFSCAPI_FSCAPI_AcceptPKCS7(This,strCertificate,iFlags,rtn)	\
    ( (This)->lpVtbl -> FSCAPI_AcceptPKCS7(This,strCertificate,iFlags,rtn) ) 

#define IFSCAPI_FSCAPICertGetPublicKey(This,strCert,iFlags,strPublicKey)	\
    ( (This)->lpVtbl -> FSCAPICertGetPublicKey(This,strCert,iFlags,strPublicKey) ) 

#define IFSCAPI_FSCAPI_ChangeP12Pwd(This,strPFXFilepath,strOldPwd,strNewPwd,iFlags,rtn)	\
    ( (This)->lpVtbl -> FSCAPI_ChangeP12Pwd(This,strPFXFilepath,strOldPwd,strNewPwd,iFlags,rtn) ) 

#define IFSCAPI_FSCAPI_PFXSign(This,sPFXfilepath,strPasswd,bData,iFlags,iKeyUsage,strPKCS7)	\
    ( (This)->lpVtbl -> FSCAPI_PFXSign(This,sPFXfilepath,strPasswd,bData,iFlags,iKeyUsage,strPKCS7) ) 

#define IFSCAPI_FSCAPI_GetPFXCertCount(This,sPFXfilepath,strPasswd,count)	\
    ( (This)->lpVtbl -> FSCAPI_GetPFXCertCount(This,sPFXfilepath,strPasswd,count) ) 

#define IFSCAPI_FS_GETVersion(This,flag,ver)	\
    ( (This)->lpVtbl -> FS_GETVersion(This,flag,ver) ) 

#define IFSCAPI_FSCAPI_ReadFile(This,strFilePath,flags,bytesFile)	\
    ( (This)->lpVtbl -> FSCAPI_ReadFile(This,strFilePath,flags,bytesFile) ) 

#define IFSCAPI_FSCAPI_GetFilePath(This,title,typeDesc,fileType,initPath,initFile,filePath)	\
    ( (This)->lpVtbl -> FSCAPI_GetFilePath(This,title,typeDesc,fileType,initPath,initFile,filePath) ) 

#define IFSCAPI_FSCAPISignEx2(This,strData,signcodepage,strSubject,bstrIssuerSubject,strProvider,strPin,iFlags,iKeyUsage,pstrSignature)	\
    ( (This)->lpVtbl -> FSCAPISignEx2(This,strData,signcodepage,strSubject,bstrIssuerSubject,strProvider,strPin,iFlags,iKeyUsage,pstrSignature) ) 

#define IFSCAPI_FSCAPIVerifyEx2(This,strSignature,bData,signcodepage,strSubject,flags,iKeyUsage,nTime,Return)	\
    ( (This)->lpVtbl -> FSCAPIVerifyEx2(This,strSignature,bData,signcodepage,strSubject,flags,iKeyUsage,nTime,Return) ) 

#define IFSCAPI_FSCAPI_PKCS7GetSignerCert(This,pkcs7signature,index,flags,Cert)	\
    ( (This)->lpVtbl -> FSCAPI_PKCS7GetSignerCert(This,pkcs7signature,index,flags,Cert) ) 

#define IFSCAPI_FSCAPIGetVerifiedContent(This,pkcs7signature,reserved,flags,content)	\
    ( (This)->lpVtbl -> FSCAPIGetVerifiedContent(This,pkcs7signature,reserved,flags,content) ) 

#define IFSCAPI_FSCAPIVerifiedContent2File(This,pkcs7signature,reserved,flags,outfile,rtn)	\
    ( (This)->lpVtbl -> FSCAPIVerifiedContent2File(This,pkcs7signature,reserved,flags,outfile,rtn) ) 

#define IFSCAPI_FSCAPIComposePKCS7(This,strCert,B64RawSignature,strData,iFlags,pstrSignature)	\
    ( (This)->lpVtbl -> FSCAPIComposePKCS7(This,strCert,B64RawSignature,strData,iFlags,pstrSignature) ) 

#define IFSCAPI_FSGetTempPath(This,path)	\
    ( (This)->lpVtbl -> FSGetTempPath(This,path) ) 

#define IFSCAPI_get_HashAlgorithm(This,pVal)	\
    ( (This)->lpVtbl -> get_HashAlgorithm(This,pVal) ) 

#define IFSCAPI_put_HashAlgorithm(This,pVal)	\
    ( (This)->lpVtbl -> put_HashAlgorithm(This,pVal) ) 

#define IFSCAPI_get_providerType(This,pVal)	\
    ( (This)->lpVtbl -> get_providerType(This,pVal) ) 

#define IFSCAPI_put_providerType(This,pVal)	\
    ( (This)->lpVtbl -> put_providerType(This,pVal) ) 

#define IFSCAPI_get_providerName(This,pVal)	\
    ( (This)->lpVtbl -> get_providerName(This,pVal) ) 

#define IFSCAPI_put_providerName(This,pVal)	\
    ( (This)->lpVtbl -> put_providerName(This,pVal) ) 

#define IFSCAPI_get_KeySpec(This,pVal)	\
    ( (This)->lpVtbl -> get_KeySpec(This,pVal) ) 

#define IFSCAPI_put_KeySpec(This,pVal)	\
    ( (This)->lpVtbl -> put_KeySpec(This,pVal) ) 

#define IFSCAPI_get_GenKeyFlags(This,pVal)	\
    ( (This)->lpVtbl -> get_GenKeyFlags(This,pVal) ) 

#define IFSCAPI_put_GenKeyFlags(This,pVal)	\
    ( (This)->lpVtbl -> put_GenKeyFlags(This,pVal) ) 

#define IFSCAPI_get_DeleteRequestCert(This,pVal)	\
    ( (This)->lpVtbl -> get_DeleteRequestCert(This,pVal) ) 

#define IFSCAPI_put_DeleteRequestCert(This,pVal)	\
    ( (This)->lpVtbl -> put_DeleteRequestCert(This,pVal) ) 

#define IFSCAPI_get_WriteCertToCSP(This,pVal)	\
    ( (This)->lpVtbl -> get_WriteCertToCSP(This,pVal) ) 

#define IFSCAPI_put_WriteCertToCSP(This,pVal)	\
    ( (This)->lpVtbl -> put_WriteCertToCSP(This,pVal) ) 

#define IFSCAPI_enumProviders(This,dwIndex,dwFlags,pbstrProvName)	\
    ( (This)->lpVtbl -> enumProviders(This,dwIndex,dwFlags,pbstrProvName) ) 

#define IFSCAPI_acceptPKCS7(This,strCert)	\
    ( (This)->lpVtbl -> acceptPKCS7(This,strCert) ) 

#define IFSCAPI_createPKCS10(This,dn,oid,strPKCS10)	\
    ( (This)->lpVtbl -> createPKCS10(This,dn,oid,strPKCS10) ) 

#define IFSCAPI_FSCAPI_PFXSign2(This,sB64PFX,strPasswd,bData,iFlags,iKeyUsage,strPKCS7)	\
    ( (This)->lpVtbl -> FSCAPI_PFXSign2(This,sB64PFX,strPasswd,bData,iFlags,iKeyUsage,strPKCS7) ) 

#define IFSCAPI_FSCAPI_CheckPFX(This,sB64PFX,strPasswd,strSubject,iFlags,iKeyUsage,rtn)	\
    ( (This)->lpVtbl -> FSCAPI_CheckPFX(This,sB64PFX,strPasswd,strSubject,iFlags,iKeyUsage,rtn) ) 

#define IFSCAPI_FSCAPI_DeleteUsrCertEx(This,strProvider,Subject,flag,keyUsage,rtn)	\
    ( (This)->lpVtbl -> FSCAPI_DeleteUsrCertEx(This,strProvider,Subject,flag,keyUsage,rtn) ) 

#define IFSCAPI_FSCAPI_CheckPermission(This,rtn)	\
    ( (This)->lpVtbl -> FSCAPI_CheckPermission(This,rtn) ) 

#define IFSCAPI_FSCAPIPureVerify(This,strSignature,strData,strSubject,iFlags,iHashalg,iKeyUsage,Ret)	\
    ( (This)->lpVtbl -> FSCAPIPureVerify(This,strSignature,strData,strSubject,iFlags,iHashalg,iKeyUsage,Ret) ) 

#define IFSCAPI_FSCAPISetDisplayMessageFont(This,font,size)	\
    ( (This)->lpVtbl -> FSCAPISetDisplayMessageFont(This,font,size) ) 

#define IFSCAPI_GetURL(This,rtn)	\
    ( (This)->lpVtbl -> GetURL(This,rtn) ) 

#define IFSCAPI_FSCAPISignAndEncrypt(This,bSignerSubject,bEncryptCert,bData,iFlags,iKeyUsage,pstrSignCipher)	\
    ( (This)->lpVtbl -> FSCAPISignAndEncrypt(This,bSignerSubject,bEncryptCert,bData,iFlags,iKeyUsage,pstrSignCipher) ) 

#define IFSCAPI_FSCAPIDecryptAndVerify(This,bCipher,iSignerIndex,iFlags,pstrClear)	\
    ( (This)->lpVtbl -> FSCAPIDecryptAndVerify(This,bCipher,iSignerIndex,iFlags,pstrClear) ) 

#define IFSCAPI_GetDecryptRtnCert(This,Cert)	\
    ( (This)->lpVtbl -> GetDecryptRtnCert(This,Cert) ) 

#define IFSCAPI_FSCAPI_PFXPureSign(This,sPFXfilepath,strPasswd,bData,iFlags,iHashalg,iKeyUsage,Ret)	\
    ( (This)->lpVtbl -> FSCAPI_PFXPureSign(This,sPFXfilepath,strPasswd,bData,iFlags,iHashalg,iKeyUsage,Ret) ) 

#define IFSCAPI_Init(This,bInitCode,bInitPath,Ret)	\
    ( (This)->lpVtbl -> Init(This,bInitCode,bInitPath,Ret) ) 

#define IFSCAPI_FSCAPIUnRegisterCertificateEx(This,strProvider,iFlags,RetError)	\
    ( (This)->lpVtbl -> FSCAPIUnRegisterCertificateEx(This,strProvider,iFlags,RetError) ) 

#define IFSCAPI_acceptPKCS7Ex(This,bCert,bPIN)	\
    ( (This)->lpVtbl -> acceptPKCS7Ex(This,bCert,bPIN) ) 

#define IFSCAPI_FSCAPISymmetricRawKeyEncrypt(This,iAlgor,strRawKey,strRawIV,bData,RtnCipher)	\
    ( (This)->lpVtbl -> FSCAPISymmetricRawKeyEncrypt(This,iAlgor,strRawKey,strRawIV,bData,RtnCipher) ) 

#define IFSCAPI_FSCAPISymmetricRawKeyDecrypt(This,iAlgor,strRawKey,strRawIV,bCipher,RtnClear)	\
    ( (This)->lpVtbl -> FSCAPISymmetricRawKeyDecrypt(This,iAlgor,strRawKey,strRawIV,bCipher,RtnClear) ) 

#define IFSCAPI_FSCAPIPKCS7GetEncryptedDigest(This,pkcs7signature,flags,content)	\
    ( (This)->lpVtbl -> FSCAPIPKCS7GetEncryptedDigest(This,pkcs7signature,flags,content) ) 

#define IFSCAPI_FSCAPI_PFXSignEx(This,bPFXfilepath,bPFXPassWord,bSubject,bData,iSignFlags,iKeyUsage,iImportP12Flags,pstrSignature)	\
    ( (This)->lpVtbl -> FSCAPI_PFXSignEx(This,bPFXfilepath,bPFXPassWord,bSubject,bData,iSignFlags,iKeyUsage,iImportP12Flags,pstrSignature) ) 

#define IFSCAPI_FSCAPI_PFXSignEx1(This,sPFXfilepath,strPasswd,bData,iFlags,iKeyUsage,strPKCS7)	\
    ( (This)->lpVtbl -> FSCAPI_PFXSignEx1(This,sPFXfilepath,strPasswd,bData,iFlags,iKeyUsage,strPKCS7) ) 

#define IFSCAPI_FSCAPIP12XGetUserCertificate(This,strPwd,strFilePath,strFileName,pstrBase64Cert)	\
    ( (This)->lpVtbl -> FSCAPIP12XGetUserCertificate(This,strPwd,strFilePath,strFileName,pstrBase64Cert) ) 

#define IFSCAPI_FSCAPI_InstallCert(This,strCertificate,iFlags,rtn)	\
    ( (This)->lpVtbl -> FSCAPI_InstallCert(This,strCertificate,iFlags,rtn) ) 

#define IFSCAPI_FSCAPI_WriteFile(This,strB64Data,strFilePath,iFlags,rtn)	\
    ( (This)->lpVtbl -> FSCAPI_WriteFile(This,strB64Data,strFilePath,iFlags,rtn) ) 

#define IFSCAPI_FSGetMyDocumentsPath(This,path)	\
    ( (This)->lpVtbl -> FSGetMyDocumentsPath(This,path) ) 

#define IFSCAPI_FSCAPISymmetricEncryptFile(This,iAlgor,strPasswd,bSrcPath,bDstPath,rtn)	\
    ( (This)->lpVtbl -> FSCAPISymmetricEncryptFile(This,iAlgor,strPasswd,bSrcPath,bDstPath,rtn) ) 

#define IFSCAPI_FSCAPISymmetricDecryptFile(This,iAlgor,strPasswd,bSrcPath,bDstPath,rtn)	\
    ( (This)->lpVtbl -> FSCAPISymmetricDecryptFile(This,iAlgor,strPasswd,bSrcPath,bDstPath,rtn) ) 

#define IFSCAPI_FSCAPI_UsrCertGetContainerName(This,Subject,flags,iKeyUsage,pStrContainerName)	\
    ( (This)->lpVtbl -> FSCAPI_UsrCertGetContainerName(This,Subject,flags,iKeyUsage,pStrContainerName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFSCAPI_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_FSCAPI;

#ifdef __cplusplus

class DECLSPEC_UUID("4B6C50D7-0E3F-4DB3-8998-F7AC9684C105")
FSCAPI;
#endif
#endif /* __FSCAPIATLLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


