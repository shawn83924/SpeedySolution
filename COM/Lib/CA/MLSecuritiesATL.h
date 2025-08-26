

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for C:\Users\User\AppData\Local\Temp\IDL31AE.tmp:
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


#ifndef __MLSecuritiesATL_h__
#define __MLSecuritiesATL_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMLCapi_FWD_DEFINED__
#define __IMLCapi_FWD_DEFINED__
typedef interface IMLCapi IMLCapi;

#endif 	/* __IMLCapi_FWD_DEFINED__ */


#ifndef __MLCapi_FWD_DEFINED__
#define __MLCapi_FWD_DEFINED__

#ifdef __cplusplus
typedef class MLCapi MLCapi;
#else
typedef struct MLCapi MLCapi;
#endif /* __cplusplus */

#endif 	/* __MLCapi_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __MLSECURITIESATLLib_LIBRARY_DEFINED__
#define __MLSECURITIESATLLib_LIBRARY_DEFINED__

/* library MLSECURITIESATLLib */
/* [custom][custom][helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_MLSECURITIESATLLib;

#ifndef __IMLCapi_INTERFACE_DEFINED__
#define __IMLCapi_INTERFACE_DEFINED__

/* interface IMLCapi */
/* [object][oleautomation][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IMLCapi;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DF4862FE-8FCA-49FB-898B-434104010000")
    IMLCapi : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetVersion( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetErrorCode( 
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetErrorMsg( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCapiErrorCode( 
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCapiErrorMsg( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetMacAddress( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetOSName( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetUserHomePath( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RegReadHKCU( 
            /* [in] */ BSTR strName,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RegWriteHKCUInt( 
            /* [in] */ BSTR strName,
            /* [in] */ long dw,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RegWriteHKCUStr( 
            /* [in] */ BSTR strName,
            /* [in] */ BSTR str,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RegReadHKLM( 
            /* [in] */ BSTR strName,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Hash( 
            /* [in] */ BSTR strPlainText,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE HashFile( 
            /* [in] */ BSTR strFileName,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetProviderName( 
            /* [in] */ BSTR strProviderName,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CreatePKCS10( 
            /* [in] */ BSTR commonName,
            /* [in] */ long keyBits,
            /* [in] */ long genKeyFlags,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CreatePKCS10Pair( 
            /* [in] */ BSTR commonName,
            /* [in] */ long keyBits,
            /* [in] */ long genKeyFlags,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCsrForSign( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCsrForEncrypt( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AcceptPKCS7( 
            /* [in] */ BSTR strPkcs7,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetDefaultPfxFileName( 
            /* [in] */ BSTR strPrefix,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PresetPasswdLengthRange( 
            /* [in] */ long min,
            /* [in] */ long max,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ChangePfxPasswd( 
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strOldPass,
            /* [in] */ BSTR strNewPass,
            /* [in] */ long dwFlags,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ChangePfxPasswdEx( 
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strOldPass,
            /* [in] */ BSTR strNewPass,
            /* [in] */ long dwPort,
            /* [in] */ long dwFlags,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE QueryPfxInfo( 
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strPasswd,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ImportPfxToIE( 
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strPasswd,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetImportedPfxFilePath( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetImportedPfxFileName( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ExportPfxFromIE( 
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPassword,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ExportMobileFromPfx( 
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strPasswd,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ExportMobileFromIE( 
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR strPasswd,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetExportedPfxFilePath( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetExportedPfxFileName( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SelectSigner( 
            /* [in] */ BSTR certFilter,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SelectSignerEx( 
            /* [in] */ BSTR certFilter,
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR strCompareDate,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetSignerPfxFilePath( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetSignerPfxFileName( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SignPkcs1( 
            /* [in] */ BSTR strContent,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SignPkcs7( 
            /* [in] */ BSTR strContent,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SelectSignerEx2( 
            /* [in] */ BSTR certFilter,
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ long dwPort,
            /* [in] */ BSTR strCompareDate,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPortEx( 
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCertB64( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCertSubject( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCertSubjectCN( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCertIssuer( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCertSerial( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCertNotBefore( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCertNotAfter( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCertFinger( 
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SymEncrypt( 
            /* [in] */ BSTR strPlainText,
            /* [in] */ BSTR strPasswdHex,
            /* [in] */ long iAlgorithm,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SymDecrypt( 
            /* [in] */ BSTR strCipher,
            /* [in] */ BSTR strPasswdHex,
            /* [in] */ long iAlgorithm,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CertEncryptPkcs7( 
            /* [in] */ BSTR strPlainText,
            /* [in] */ BSTR strCertB64,
            /* [in] */ long iAlgorithm,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CertDecryptPkcs7( 
            /* [in] */ BSTR strCipher,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetIECertCount( 
            /* [retval][out] */ long *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetIECertSerial( 
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetIECertCommonName( 
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetIECertSubject( 
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetIECertIssuer( 
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetIECertNotBefore( 
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetIECertNotAfter( 
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetIECertFinger( 
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DeleteIECert( 
            /* [in] */ long dwIndex,
            /* [retval][out] */ long *Ret) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IMLCapiVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMLCapi * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMLCapi * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMLCapi * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMLCapi * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMLCapi * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMLCapi * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMLCapi * This,
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
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVersion )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetErrorCode )( 
            IMLCapi * This,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetErrorMsg )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCapiErrorCode )( 
            IMLCapi * This,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCapiErrorMsg )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetMacAddress )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetOSName )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUserHomePath )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RegReadHKCU )( 
            IMLCapi * This,
            /* [in] */ BSTR strName,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RegWriteHKCUInt )( 
            IMLCapi * This,
            /* [in] */ BSTR strName,
            /* [in] */ long dw,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RegWriteHKCUStr )( 
            IMLCapi * This,
            /* [in] */ BSTR strName,
            /* [in] */ BSTR str,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RegReadHKLM )( 
            IMLCapi * This,
            /* [in] */ BSTR strName,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Hash )( 
            IMLCapi * This,
            /* [in] */ BSTR strPlainText,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *HashFile )( 
            IMLCapi * This,
            /* [in] */ BSTR strFileName,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetProviderName )( 
            IMLCapi * This,
            /* [in] */ BSTR strProviderName,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreatePKCS10 )( 
            IMLCapi * This,
            /* [in] */ BSTR commonName,
            /* [in] */ long keyBits,
            /* [in] */ long genKeyFlags,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreatePKCS10Pair )( 
            IMLCapi * This,
            /* [in] */ BSTR commonName,
            /* [in] */ long keyBits,
            /* [in] */ long genKeyFlags,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCsrForSign )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCsrForEncrypt )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AcceptPKCS7 )( 
            IMLCapi * This,
            /* [in] */ BSTR strPkcs7,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetDefaultPfxFileName )( 
            IMLCapi * This,
            /* [in] */ BSTR strPrefix,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PresetPasswdLengthRange )( 
            IMLCapi * This,
            /* [in] */ long min,
            /* [in] */ long max,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ChangePfxPasswd )( 
            IMLCapi * This,
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strOldPass,
            /* [in] */ BSTR strNewPass,
            /* [in] */ long dwFlags,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ChangePfxPasswdEx )( 
            IMLCapi * This,
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strOldPass,
            /* [in] */ BSTR strNewPass,
            /* [in] */ long dwPort,
            /* [in] */ long dwFlags,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *QueryPfxInfo )( 
            IMLCapi * This,
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strPasswd,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ImportPfxToIE )( 
            IMLCapi * This,
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strPasswd,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetImportedPfxFilePath )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetImportedPfxFileName )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ExportPfxFromIE )( 
            IMLCapi * This,
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPassword,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ExportMobileFromPfx )( 
            IMLCapi * This,
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strPasswd,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ExportMobileFromIE )( 
            IMLCapi * This,
            /* [in] */ BSTR strSubject,
            /* [in] */ BSTR strPasswd,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetExportedPfxFilePath )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetExportedPfxFileName )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SelectSigner )( 
            IMLCapi * This,
            /* [in] */ BSTR certFilter,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SelectSignerEx )( 
            IMLCapi * This,
            /* [in] */ BSTR certFilter,
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ BSTR strPasswd,
            /* [in] */ BSTR strCompareDate,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetSignerPfxFilePath )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetSignerPfxFileName )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SignPkcs1 )( 
            IMLCapi * This,
            /* [in] */ BSTR strContent,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SignPkcs7 )( 
            IMLCapi * This,
            /* [in] */ BSTR strContent,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SelectSignerEx2 )( 
            IMLCapi * This,
            /* [in] */ BSTR certFilter,
            /* [in] */ BSTR strPfxPath,
            /* [in] */ BSTR strPfxName,
            /* [in] */ long dwPort,
            /* [in] */ BSTR strCompareDate,
            /* [in] */ long dwFlags,
            /* [in] */ long dwKeyUsage,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPortEx )( 
            IMLCapi * This,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCertB64 )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCertSubject )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCertSubjectCN )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCertIssuer )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCertSerial )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCertNotBefore )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCertNotAfter )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCertFinger )( 
            IMLCapi * This,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SymEncrypt )( 
            IMLCapi * This,
            /* [in] */ BSTR strPlainText,
            /* [in] */ BSTR strPasswdHex,
            /* [in] */ long iAlgorithm,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SymDecrypt )( 
            IMLCapi * This,
            /* [in] */ BSTR strCipher,
            /* [in] */ BSTR strPasswdHex,
            /* [in] */ long iAlgorithm,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CertEncryptPkcs7 )( 
            IMLCapi * This,
            /* [in] */ BSTR strPlainText,
            /* [in] */ BSTR strCertB64,
            /* [in] */ long iAlgorithm,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CertDecryptPkcs7 )( 
            IMLCapi * This,
            /* [in] */ BSTR strCipher,
            /* [in] */ long dwFlags,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetIECertCount )( 
            IMLCapi * This,
            /* [retval][out] */ long *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetIECertSerial )( 
            IMLCapi * This,
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetIECertCommonName )( 
            IMLCapi * This,
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetIECertSubject )( 
            IMLCapi * This,
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetIECertIssuer )( 
            IMLCapi * This,
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetIECertNotBefore )( 
            IMLCapi * This,
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetIECertNotAfter )( 
            IMLCapi * This,
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetIECertFinger )( 
            IMLCapi * This,
            /* [in] */ long dwIndex,
            /* [retval][out] */ BSTR *Ret);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DeleteIECert )( 
            IMLCapi * This,
            /* [in] */ long dwIndex,
            /* [retval][out] */ long *Ret);
        
        END_INTERFACE
    } IMLCapiVtbl;

    interface IMLCapi
    {
        CONST_VTBL struct IMLCapiVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMLCapi_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMLCapi_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMLCapi_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMLCapi_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMLCapi_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMLCapi_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMLCapi_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IMLCapi_GetVersion(This,Ret)	\
    ( (This)->lpVtbl -> GetVersion(This,Ret) ) 

#define IMLCapi_GetErrorCode(This,Ret)	\
    ( (This)->lpVtbl -> GetErrorCode(This,Ret) ) 

#define IMLCapi_GetErrorMsg(This,Ret)	\
    ( (This)->lpVtbl -> GetErrorMsg(This,Ret) ) 

#define IMLCapi_GetCapiErrorCode(This,Ret)	\
    ( (This)->lpVtbl -> GetCapiErrorCode(This,Ret) ) 

#define IMLCapi_GetCapiErrorMsg(This,Ret)	\
    ( (This)->lpVtbl -> GetCapiErrorMsg(This,Ret) ) 

#define IMLCapi_GetMacAddress(This,Ret)	\
    ( (This)->lpVtbl -> GetMacAddress(This,Ret) ) 

#define IMLCapi_GetOSName(This,Ret)	\
    ( (This)->lpVtbl -> GetOSName(This,Ret) ) 

#define IMLCapi_GetUserHomePath(This,Ret)	\
    ( (This)->lpVtbl -> GetUserHomePath(This,Ret) ) 

#define IMLCapi_RegReadHKCU(This,strName,Ret)	\
    ( (This)->lpVtbl -> RegReadHKCU(This,strName,Ret) ) 

#define IMLCapi_RegWriteHKCUInt(This,strName,dw,Ret)	\
    ( (This)->lpVtbl -> RegWriteHKCUInt(This,strName,dw,Ret) ) 

#define IMLCapi_RegWriteHKCUStr(This,strName,str,Ret)	\
    ( (This)->lpVtbl -> RegWriteHKCUStr(This,strName,str,Ret) ) 

#define IMLCapi_RegReadHKLM(This,strName,Ret)	\
    ( (This)->lpVtbl -> RegReadHKLM(This,strName,Ret) ) 

#define IMLCapi_Hash(This,strPlainText,dwFlags,Ret)	\
    ( (This)->lpVtbl -> Hash(This,strPlainText,dwFlags,Ret) ) 

#define IMLCapi_HashFile(This,strFileName,dwFlags,Ret)	\
    ( (This)->lpVtbl -> HashFile(This,strFileName,dwFlags,Ret) ) 

#define IMLCapi_SetProviderName(This,strProviderName,Ret)	\
    ( (This)->lpVtbl -> SetProviderName(This,strProviderName,Ret) ) 

#define IMLCapi_CreatePKCS10(This,commonName,keyBits,genKeyFlags,Ret)	\
    ( (This)->lpVtbl -> CreatePKCS10(This,commonName,keyBits,genKeyFlags,Ret) ) 

#define IMLCapi_CreatePKCS10Pair(This,commonName,keyBits,genKeyFlags,Ret)	\
    ( (This)->lpVtbl -> CreatePKCS10Pair(This,commonName,keyBits,genKeyFlags,Ret) ) 

#define IMLCapi_GetCsrForSign(This,Ret)	\
    ( (This)->lpVtbl -> GetCsrForSign(This,Ret) ) 

#define IMLCapi_GetCsrForEncrypt(This,Ret)	\
    ( (This)->lpVtbl -> GetCsrForEncrypt(This,Ret) ) 

#define IMLCapi_AcceptPKCS7(This,strPkcs7,Ret)	\
    ( (This)->lpVtbl -> AcceptPKCS7(This,strPkcs7,Ret) ) 

#define IMLCapi_SetDefaultPfxFileName(This,strPrefix,Ret)	\
    ( (This)->lpVtbl -> SetDefaultPfxFileName(This,strPrefix,Ret) ) 

#define IMLCapi_PresetPasswdLengthRange(This,min,max,Ret)	\
    ( (This)->lpVtbl -> PresetPasswdLengthRange(This,min,max,Ret) ) 

#define IMLCapi_ChangePfxPasswd(This,strPfxPath,strPfxName,strOldPass,strNewPass,dwFlags,Ret)	\
    ( (This)->lpVtbl -> ChangePfxPasswd(This,strPfxPath,strPfxName,strOldPass,strNewPass,dwFlags,Ret) ) 

#define IMLCapi_ChangePfxPasswdEx(This,strPfxPath,strPfxName,strOldPass,strNewPass,dwPort,dwFlags,Ret)	\
    ( (This)->lpVtbl -> ChangePfxPasswdEx(This,strPfxPath,strPfxName,strOldPass,strNewPass,dwPort,dwFlags,Ret) ) 

#define IMLCapi_QueryPfxInfo(This,strPfxPath,strPfxName,strPasswd,dwFlags,dwKeyUsage,Ret)	\
    ( (This)->lpVtbl -> QueryPfxInfo(This,strPfxPath,strPfxName,strPasswd,dwFlags,dwKeyUsage,Ret) ) 

#define IMLCapi_ImportPfxToIE(This,strPfxPath,strPfxName,strPasswd,dwFlags,dwKeyUsage,Ret)	\
    ( (This)->lpVtbl -> ImportPfxToIE(This,strPfxPath,strPfxName,strPasswd,dwFlags,dwKeyUsage,Ret) ) 

#define IMLCapi_GetImportedPfxFilePath(This,Ret)	\
    ( (This)->lpVtbl -> GetImportedPfxFilePath(This,Ret) ) 

#define IMLCapi_GetImportedPfxFileName(This,Ret)	\
    ( (This)->lpVtbl -> GetImportedPfxFileName(This,Ret) ) 

#define IMLCapi_ExportPfxFromIE(This,strSubject,strPfxPath,strPassword,dwFlags,dwKeyUsage,Ret)	\
    ( (This)->lpVtbl -> ExportPfxFromIE(This,strSubject,strPfxPath,strPassword,dwFlags,dwKeyUsage,Ret) ) 

#define IMLCapi_ExportMobileFromPfx(This,strPfxPath,strPfxName,strPasswd,dwFlags,dwKeyUsage,Ret)	\
    ( (This)->lpVtbl -> ExportMobileFromPfx(This,strPfxPath,strPfxName,strPasswd,dwFlags,dwKeyUsage,Ret) ) 

#define IMLCapi_ExportMobileFromIE(This,strSubject,strPasswd,dwFlags,dwKeyUsage,Ret)	\
    ( (This)->lpVtbl -> ExportMobileFromIE(This,strSubject,strPasswd,dwFlags,dwKeyUsage,Ret) ) 

#define IMLCapi_GetExportedPfxFilePath(This,Ret)	\
    ( (This)->lpVtbl -> GetExportedPfxFilePath(This,Ret) ) 

#define IMLCapi_GetExportedPfxFileName(This,Ret)	\
    ( (This)->lpVtbl -> GetExportedPfxFileName(This,Ret) ) 

#define IMLCapi_SelectSigner(This,certFilter,dwFlags,dwKeyUsage,Ret)	\
    ( (This)->lpVtbl -> SelectSigner(This,certFilter,dwFlags,dwKeyUsage,Ret) ) 

#define IMLCapi_SelectSignerEx(This,certFilter,strPfxPath,strPfxName,strPasswd,strCompareDate,dwFlags,dwKeyUsage,Ret)	\
    ( (This)->lpVtbl -> SelectSignerEx(This,certFilter,strPfxPath,strPfxName,strPasswd,strCompareDate,dwFlags,dwKeyUsage,Ret) ) 

#define IMLCapi_GetSignerPfxFilePath(This,Ret)	\
    ( (This)->lpVtbl -> GetSignerPfxFilePath(This,Ret) ) 

#define IMLCapi_GetSignerPfxFileName(This,Ret)	\
    ( (This)->lpVtbl -> GetSignerPfxFileName(This,Ret) ) 

#define IMLCapi_SignPkcs1(This,strContent,dwFlags,Ret)	\
    ( (This)->lpVtbl -> SignPkcs1(This,strContent,dwFlags,Ret) ) 

#define IMLCapi_SignPkcs7(This,strContent,dwFlags,Ret)	\
    ( (This)->lpVtbl -> SignPkcs7(This,strContent,dwFlags,Ret) ) 

#define IMLCapi_SelectSignerEx2(This,certFilter,strPfxPath,strPfxName,dwPort,strCompareDate,dwFlags,dwKeyUsage,Ret)	\
    ( (This)->lpVtbl -> SelectSignerEx2(This,certFilter,strPfxPath,strPfxName,dwPort,strCompareDate,dwFlags,dwKeyUsage,Ret) ) 

#define IMLCapi_GetPortEx(This,Ret)	\
    ( (This)->lpVtbl -> GetPortEx(This,Ret) ) 

#define IMLCapi_GetCertB64(This,Ret)	\
    ( (This)->lpVtbl -> GetCertB64(This,Ret) ) 

#define IMLCapi_GetCertSubject(This,Ret)	\
    ( (This)->lpVtbl -> GetCertSubject(This,Ret) ) 

#define IMLCapi_GetCertSubjectCN(This,Ret)	\
    ( (This)->lpVtbl -> GetCertSubjectCN(This,Ret) ) 

#define IMLCapi_GetCertIssuer(This,Ret)	\
    ( (This)->lpVtbl -> GetCertIssuer(This,Ret) ) 

#define IMLCapi_GetCertSerial(This,Ret)	\
    ( (This)->lpVtbl -> GetCertSerial(This,Ret) ) 

#define IMLCapi_GetCertNotBefore(This,Ret)	\
    ( (This)->lpVtbl -> GetCertNotBefore(This,Ret) ) 

#define IMLCapi_GetCertNotAfter(This,Ret)	\
    ( (This)->lpVtbl -> GetCertNotAfter(This,Ret) ) 

#define IMLCapi_GetCertFinger(This,Ret)	\
    ( (This)->lpVtbl -> GetCertFinger(This,Ret) ) 

#define IMLCapi_SymEncrypt(This,strPlainText,strPasswdHex,iAlgorithm,dwFlags,Ret)	\
    ( (This)->lpVtbl -> SymEncrypt(This,strPlainText,strPasswdHex,iAlgorithm,dwFlags,Ret) ) 

#define IMLCapi_SymDecrypt(This,strCipher,strPasswdHex,iAlgorithm,dwFlags,Ret)	\
    ( (This)->lpVtbl -> SymDecrypt(This,strCipher,strPasswdHex,iAlgorithm,dwFlags,Ret) ) 

#define IMLCapi_CertEncryptPkcs7(This,strPlainText,strCertB64,iAlgorithm,dwFlags,Ret)	\
    ( (This)->lpVtbl -> CertEncryptPkcs7(This,strPlainText,strCertB64,iAlgorithm,dwFlags,Ret) ) 

#define IMLCapi_CertDecryptPkcs7(This,strCipher,dwFlags,Ret)	\
    ( (This)->lpVtbl -> CertDecryptPkcs7(This,strCipher,dwFlags,Ret) ) 

#define IMLCapi_GetIECertCount(This,Ret)	\
    ( (This)->lpVtbl -> GetIECertCount(This,Ret) ) 

#define IMLCapi_GetIECertSerial(This,dwIndex,Ret)	\
    ( (This)->lpVtbl -> GetIECertSerial(This,dwIndex,Ret) ) 

#define IMLCapi_GetIECertCommonName(This,dwIndex,Ret)	\
    ( (This)->lpVtbl -> GetIECertCommonName(This,dwIndex,Ret) ) 

#define IMLCapi_GetIECertSubject(This,dwIndex,Ret)	\
    ( (This)->lpVtbl -> GetIECertSubject(This,dwIndex,Ret) ) 

#define IMLCapi_GetIECertIssuer(This,dwIndex,Ret)	\
    ( (This)->lpVtbl -> GetIECertIssuer(This,dwIndex,Ret) ) 

#define IMLCapi_GetIECertNotBefore(This,dwIndex,Ret)	\
    ( (This)->lpVtbl -> GetIECertNotBefore(This,dwIndex,Ret) ) 

#define IMLCapi_GetIECertNotAfter(This,dwIndex,Ret)	\
    ( (This)->lpVtbl -> GetIECertNotAfter(This,dwIndex,Ret) ) 

#define IMLCapi_GetIECertFinger(This,dwIndex,Ret)	\
    ( (This)->lpVtbl -> GetIECertFinger(This,dwIndex,Ret) ) 

#define IMLCapi_DeleteIECert(This,dwIndex,Ret)	\
    ( (This)->lpVtbl -> DeleteIECert(This,dwIndex,Ret) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMLCapi_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_MLCapi;

#ifdef __cplusplus

class DECLSPEC_UUID("C0888C8F-A4F1-480A-B970-434104010000")
MLCapi;
#endif
#endif /* __MLSECURITIESATLLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


