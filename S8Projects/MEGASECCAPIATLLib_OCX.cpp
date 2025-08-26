// ************************************************************************ //
// WARNING                                                                    
// -------                                                                    
// The types declared in this file were generated from data read from a       
// Type Library. If this type library is explicitly or indirectly (via        
// another type library referring to this type library) re-imported, or the   
// 'Refresh' command of the Type Library Editor activated while editing the   
// Type Library, the contents of this file will be regenerated and all        
// manual modifications will be lost.                                         
// ************************************************************************ //

// $Rev: 87174 $
// File generated on 2019/7/23 下午 08:35:29 from Type Library described below.

// ************************************************************************  //
// Type Lib: C:\MEGASECCAPIATL.dll (1)
// LIBID: {B44F8C2D-F257-43B4-8904-961E87889EC6}
// LCID: 0
// Helpfile: 
// HelpString: MEGASECCAPIATL 1.0 ���O�{���w
// DepndLst: 
//   (1) v2.0 stdole, (C:\Windows\SysWOW64\stdole2.tlb)
// SYS_KIND: SYS_WIN32
// ************************************************************************ //

#include <vcl.h>
#pragma hdrstop

#include <olectrls.hpp>
#include <oleserver.hpp>
#if defined(USING_ATL)
#include <atl\atlvcl.h>
#endif

#pragma option -w-8122
#include "MEGASECCAPIATLLib_OCX.h"

#if !defined(__PRAGMA_PACKAGE_SMART_INIT)
#define      __PRAGMA_PACKAGE_SMART_INIT
#pragma package(smart_init)
#endif

namespace Megaseccapiatllib_tlb
{



// *********************************************************************//
// OCX PROXY CLASS IMPLEMENTATION
// (The following variables/methods implement the class TMEGASECCAPI which
// allows "MEGASECCAPI Class" to be hosted in CBuilder IDE/apps).
// *********************************************************************//
Vcl::Olectrls::TControlData2 TMEGASECCAPI::CControlData =
{
  // GUID of CoClass and Event Interface of Control
  {0x1309DF47, 0xA41D, 0x47A1,{ 0xBF, 0xAF, 0x2B,0x76, 0x86, 0xCC,0xC1, 0x9E} }, // CoClass
  {0x00000000, 0x0000, 0x0000,{ 0x00, 0x00, 0x00,0x00, 0x00, 0x00,0x00, 0x00} }, // Events

  // Count of Events and array of their DISPIDs
  0, NULL,

  // Pointer to Runtime License string
  NULL,  // HRESULT(0x80004002)

  // Flags for OnChanged PropertyNotification
  0x00000000,
  401,// (IDE Version)

  // Count of Font Prop and array of their DISPIDs
  0, Winapi::Activex::PDispIDList(NULL),

  // Count of Pict Prop and array of their DISPIDs
  0, Winapi::Activex::PDispIDList(NULL),
  0, // Reserved
  0, // Instance count (used internally)
  0, // List of Enum descriptions (internal)
};

GUID     TMEGASECCAPI::DEF_CTL_INTF = {0xC704E610, 0x71C5, 0x4808,{ 0x96, 0x06, 0x02,0x75, 0x99, 0x1B,0xAB, 0xBB} };
TNoParam TMEGASECCAPI::OptParam;

static inline void ValidCtrCheck(TMEGASECCAPI *)
{
   delete new TMEGASECCAPI((System::Classes::TComponent*)(0));
};

void __fastcall TMEGASECCAPI::InitControlData()
{
  ControlData = (Vcl::Olectrls::TControlData*)&CControlData;
};

void __fastcall TMEGASECCAPI::CreateControl()
{
  if (!m_OCXIntf)
  {
    _ASSERTE(DefaultDispatch);
    DefaultDispatch->QueryInterface(DEF_CTL_INTF, (LPVOID*)&m_OCXIntf);
  }
};

TCOMIMEGASECCAPI __fastcall TMEGASECCAPI::GetDefaultInterface()
{
  CreateControl();
  return m_OCXIntf;
};

void __fastcall TMEGASECCAPI::SetCodePage(int codepage/*[in]*/)
{
  GetDefaultInterface()->SetCodePage(codepage/*[in]*/);
}

BSTR __fastcall TMEGASECCAPI::CAPIEncrypt(BSTR strSubject/*[in]*/, int istore/*[in]*/, 
                                          int iSelCertFlags/*[in]*/, BSTR bData/*[in]*/, 
                                          int Flags/*[in]*/, int iKeyUsage/*[in]*/)
{
  BSTR strCipher = 0;
  OLECHECK(GetDefaultInterface()->CAPIEncrypt(strSubject, istore, iSelCertFlags, bData, Flags, 
                                              iKeyUsage, (BSTR*)&strCipher));
  return strCipher;
}

BSTR __fastcall TMEGASECCAPI::CAPIDecrypt(BSTR strCipher/*[in]*/, int Flags/*[in]*/, 
                                          int iKeyUsage/*[in]*/)
{
  BSTR clear = 0;
  OLECHECK(GetDefaultInterface()->CAPIDecrypt(strCipher, Flags, iKeyUsage, (BSTR*)&clear));
  return clear;
}

int __fastcall TMEGASECCAPI::CAPIExportPKCS12(BSTR strSubject/*[in]*/, int Flags/*[in]*/, 
                                              int iKeyUsage/*[in]*/, BSTR strPassword/*[in]*/, 
                                              BSTR strFilePath/*[in]*/, BSTR strFileName/*[in]*/)
{
  int RtnError;
  OLECHECK(GetDefaultInterface()->CAPIExportPKCS12(strSubject, Flags, iKeyUsage, strPassword, 
                                                   strFilePath, strFileName, (int*)&RtnError));
  return RtnError;
}

long __fastcall TMEGASECCAPI::GetErrorCode(void)
{
  long ErrorCode;
  OLECHECK(GetDefaultInterface()->GetErrorCode((long*)&ErrorCode));
  return ErrorCode;
}

BSTR __fastcall TMEGASECCAPI::CAPISymmetricEncrypt(int iAlgor/*[in]*/, BSTR strPasswd/*[in]*/, 
                                                   BSTR bData/*[in]*/)
{
  BSTR RtnCipher = 0;
  OLECHECK(GetDefaultInterface()->CAPISymmetricEncrypt(iAlgor, strPasswd, bData, (BSTR*)&RtnCipher));
  return RtnCipher;
}

BSTR __fastcall TMEGASECCAPI::CAPISymmetricDecrypt(int iAlgor/*[in]*/, BSTR strPasswd/*[in]*/, 
                                                   BSTR cipher/*[in]*/)
{
  BSTR RtnClear = 0;
  OLECHECK(GetDefaultInterface()->CAPISymmetricDecrypt(iAlgor, strPasswd, cipher, (BSTR*)&RtnClear));
  return RtnClear;
}

BSTR __fastcall TMEGASECCAPI::CAPICertGetSerialNumber(BSTR strCert/*[in]*/, int iFlags/*[in]*/)
{
  BSTR SerialNumber = 0;
  OLECHECK(GetDefaultInterface()->CAPICertGetSerialNumber(strCert, iFlags, (BSTR*)&SerialNumber));
  return SerialNumber;
}

void __fastcall TMEGASECCAPI::CheckKeyUsage(int iCheck/*[in]*/)
{
  GetDefaultInterface()->CheckKeyUsage(iCheck/*[in]*/);
}

BSTR __fastcall TMEGASECCAPI::GetVerifyRtnSignerInfo(void)
{
  BSTR SignerInfo = 0;
  OLECHECK(GetDefaultInterface()->GetVerifyRtnSignerInfo((BSTR*)&SignerInfo));
  return SignerInfo;
}

BSTR __fastcall TMEGASECCAPI::GetVerifyRtnCert(void)
{
  BSTR Cert = 0;
  OLECHECK(GetDefaultInterface()->GetVerifyRtnCert((BSTR*)&Cert));
  return Cert;
}

BSTR __fastcall TMEGASECCAPI::CAPICertGetDigest(BSTR strCert/*[in]*/, int iFlags/*[in]*/)
{
  BSTR pstrDigest = 0;
  OLECHECK(GetDefaultInterface()->CAPICertGetDigest(strCert, iFlags, (BSTR*)&pstrDigest));
  return pstrDigest;
}

int __fastcall TMEGASECCAPI::CAPIShowCertificateDlg(BSTR strMsg/*[in]*/, BSTR strCert/*[in]*/)
{
  int iRtn;
  OLECHECK(GetDefaultInterface()->CAPIShowCertificateDlg(strMsg, strCert, (int*)&iRtn));
  return iRtn;
}

int __fastcall TMEGASECCAPI::CAPISetLanguage(BSTR strLanguage/*[in]*/)
{
  int Ret;
  OLECHECK(GetDefaultInterface()->CAPISetLanguage(strLanguage, (int*)&Ret));
  return Ret;
}

int __fastcall TMEGASECCAPI::CAPIImportPKCS12(BSTR strPwd/*[in]*/, BSTR strFilePath/*[in]*/, 
                                              BSTR strFileName/*[in]*/, int iFlags/*[in]*/)
{
  int RetError;
  OLECHECK(GetDefaultInterface()->CAPIImportPKCS12(strPwd, strFilePath, strFileName, iFlags, (int*)&RetError));
  return RetError;
}

BSTR __fastcall TMEGASECCAPI::CAPIEncode(BSTR strData/*[in]*/, VARIANT strPemTitle/*[in]*/)
{
  BSTR strEncoded = 0;
  OLECHECK(GetDefaultInterface()->CAPIEncode(strData, strPemTitle, (BSTR*)&strEncoded));
  return strEncoded;
}

BSTR __fastcall TMEGASECCAPI::CAPIDecode(BSTR strEncoded/*[in]*/, VARIANT strPemTitle/*[in]*/)
{
  BSTR strDecoded = 0;
  OLECHECK(GetDefaultInterface()->CAPIDecode(strEncoded, strPemTitle, (BSTR*)&strDecoded));
  return strDecoded;
}

BSTR __fastcall TMEGASECCAPI::CAPICertGetSubject(BSTR strCert/*[in]*/, int iFlags/*[in]*/)
{
  BSTR strSubject = 0;
  OLECHECK(GetDefaultInterface()->CAPICertGetSubject(strCert, iFlags, (BSTR*)&strSubject));
  return strSubject;
}

BSTR __fastcall TMEGASECCAPI::CAPICertGetIssuer(BSTR strCert/*[in]*/, int iFlags/*[in]*/)
{
  BSTR strIssuer = 0;
  OLECHECK(GetDefaultInterface()->CAPICertGetIssuer(strCert, iFlags, (BSTR*)&strIssuer));
  return strIssuer;
}

BSTR __fastcall TMEGASECCAPI::CAPICertGetNotBefore(BSTR strCert/*[in]*/, int iFlags/*[in]*/)
{
  BSTR strNotBefore = 0;
  OLECHECK(GetDefaultInterface()->CAPICertGetNotBefore(strCert, iFlags, (BSTR*)&strNotBefore));
  return strNotBefore;
}

BSTR __fastcall TMEGASECCAPI::CAPICertGetNotAfter(BSTR strCert/*[in]*/, int iFlags/*[in]*/)
{
  BSTR strNotAfter = 0;
  OLECHECK(GetDefaultInterface()->CAPICertGetNotAfter(strCert, iFlags, (BSTR*)&strNotAfter));
  return strNotAfter;
}

BSTR __fastcall TMEGASECCAPI::CAPICertEncrypt(BSTR strCert/*[in]*/, BSTR bData/*[in]*/, 
                                              int iFlags/*[in]*/)
{
  BSTR strCipher = 0;
  OLECHECK(GetDefaultInterface()->CAPICertEncrypt(strCert, bData, iFlags, (BSTR*)&strCipher));
  return strCipher;
}

void __fastcall TMEGASECCAPI::CAPISetDisplayMessage(BSTR strMessage/*[in]*/)
{
  GetDefaultInterface()->CAPISetDisplayMessage(strMessage/*[in]*/);
}

int __fastcall TMEGASECCAPI::CAPIPureSign(BSTR strData/*[in]*/, BSTR strSubject/*[in]*/, 
                                          int iFlags/*[in]*/, int iHashalg/*[in,def,opt]*/, 
                                          int iKeyUsage/*[in,def,opt]*/)
{
  int Ret;
  OLECHECK(GetDefaultInterface()->CAPIPureSign(strData, strSubject, iFlags, iHashalg, iKeyUsage, (int*)&Ret));
  return Ret;
}

BSTR __fastcall TMEGASECCAPI::CAPIGetRtnSignature(void)
{
  BSTR strSignature = 0;
  OLECHECK(GetDefaultInterface()->CAPIGetRtnSignature((BSTR*)&strSignature));
  return strSignature;
}

BSTR __fastcall TMEGASECCAPI::CAPIGetRtnCert(void)
{
  BSTR strCert = 0;
  OLECHECK(GetDefaultInterface()->CAPIGetRtnCert((BSTR*)&strCert));
  return strCert;
}

long __fastcall TMEGASECCAPI::CAPIRegisterCertificate(BSTR strProvider/*[in]*/)
{
  long RetError;
  OLECHECK(GetDefaultInterface()->CAPIRegisterCertificate(strProvider, (long*)&RetError));
  return RetError;
}

BSTR __fastcall TMEGASECCAPI::CAPIPKCS7GetUserCertificate(BSTR p7cert/*[in]*/)
{
  BSTR x509 = 0;
  OLECHECK(GetDefaultInterface()->CAPIPKCS7GetUserCertificate(p7cert, (BSTR*)&x509));
  return x509;
}

VARIANT __fastcall TMEGASECCAPI::CAPIGetCertSubject(BSTR subject/*[in]*/, BSTR field/*[in]*/, 
                                                    int flag/*[in]*/, int keyUsage/*[in]*/)
{
  VARIANT rtn;
  OLECHECK(GetDefaultInterface()->CAPIGetCertSubject(subject, field, flag, keyUsage, (VARIANT*)&rtn));
  return rtn;
}

void __fastcall TMEGASECCAPI::CAPIShowDebugMessage(long bShow/*[in]*/)
{
  GetDefaultInterface()->CAPIShowDebugMessage(bShow/*[in]*/);
}

VARIANT __fastcall TMEGASECCAPI::CAPIGetCertsEx(BSTR provider/*[in]*/, BSTR subject/*[in]*/, 
                                                int flag/*[in]*/, int keyUsage/*[in]*/, 
                                                int disableATKeySpec/*[in]*/)
{
  VARIANT rtns;
  OLECHECK(GetDefaultInterface()->CAPIGetCertsEx(provider, subject, flag, keyUsage, disableATKeySpec, (VARIANT*)&rtns));
  return rtns;
}

long __fastcall TMEGASECCAPI::CAPIRegisterCertificateEx(BSTR strProvider/*[in]*/, 
                                                        long iDisableFlag/*[in]*/)
{
  long RetError;
  OLECHECK(GetDefaultInterface()->CAPIRegisterCertificateEx(strProvider, iDisableFlag, (long*)&RetError));
  return RetError;
}

BSTR __fastcall TMEGASECCAPI::CAPIFileSign(BSTR strFilePath/*[in]*/, BSTR strSubject/*[in]*/, 
                                           BSTR strProvider/*[in]*/, BSTR strPin/*[in]*/, 
                                           int iFlags/*[in]*/, int iHashFlag/*[in]*/, 
                                           int iKeyUsage/*[in]*/)
{
  BSTR pstrSignature = 0;
  OLECHECK(GetDefaultInterface()->CAPIFileSign(strFilePath, strSubject, strProvider, strPin, iFlags, 
                                               iHashFlag, iKeyUsage, (BSTR*)&pstrSignature));
  return pstrSignature;
}

int __fastcall TMEGASECCAPI::CAPI_MultiSignInit(BSTR strProvider/*[in]*/, int iFlags/*[in]*/)
{
  int Ret;
  OLECHECK(GetDefaultInterface()->CAPI_MultiSignInit(strProvider, iFlags, (int*)&Ret));
  return Ret;
}

void __fastcall TMEGASECCAPI::CAPI_MultiSignEnd(int iFlags/*[in]*/)
{
  GetDefaultInterface()->CAPI_MultiSignEnd(iFlags/*[in]*/);
}

BSTR __fastcall TMEGASECCAPI::CAPISelectCert(BSTR subject/*[in]*/, int Flags/*[in]*/, 
                                             int iKeyUsage/*[in]*/)
{
  BSTR pstrCertSN = 0;
  OLECHECK(GetDefaultInterface()->CAPISelectCert(subject, Flags, iKeyUsage, (BSTR*)&pstrCertSN));
  return pstrCertSN;
}

BSTR __fastcall TMEGASECCAPI::GetSignRtnContent(void)
{
  BSTR strContent = 0;
  OLECHECK(GetDefaultInterface()->GetSignRtnContent((BSTR*)&strContent));
  return strContent;
}

BSTR __fastcall TMEGASECCAPI::CAPIFileEncryptByCert(BSTR strCert/*[in]*/, BSTR strFilePath/*[in]*/, 
                                                    int iFlags/*[in]*/)
{
  BSTR strCipher = 0;
  OLECHECK(GetDefaultInterface()->CAPIFileEncryptByCert(strCert, strFilePath, iFlags, (BSTR*)&strCipher));
  return strCipher;
}

int __fastcall TMEGASECCAPI::CAPIDecrypt2File(BSTR strCipher/*[in]*/, BSTR strOutFilePath/*[in]*/, 
                                              int Flags/*[in]*/, int iKeyUsage/*[in]*/)
{
  int iRtn;
  OLECHECK(GetDefaultInterface()->CAPIDecrypt2File(strCipher, strOutFilePath, Flags, iKeyUsage, (int*)&iRtn));
  return iRtn;
}

long __fastcall TMEGASECCAPI::CAPIPFXImportSC(BSTR sPFXfilepath/*[in]*/, BSTR strPasswd/*[in]*/, 
                                              BSTR strCSP/*[in]*/, int iFlags/*[in]*/)
{
  long rtn;
  OLECHECK(GetDefaultInterface()->CAPIPFXImportSC(sPFXfilepath, strPasswd, strCSP, iFlags, (long*)&rtn));
  return rtn;
}

long __fastcall TMEGASECCAPI::ssInit(BSTR strCert/*[in]*/, int iFlags/*[in]*/)
{
  long iRtn;
  OLECHECK(GetDefaultInterface()->ssInit(strCert, iFlags, (long*)&iRtn));
  return iRtn;
}

BSTR __fastcall TMEGASECCAPI::ssGetEncSessionKey(int iFlags/*[in]*/)
{
  BSTR RtnCipher = 0;
  OLECHECK(GetDefaultInterface()->ssGetEncSessionKey(iFlags, (BSTR*)&RtnCipher));
  return RtnCipher;
}

BSTR __fastcall TMEGASECCAPI::CAPI_UsrCertGetProvName(BSTR subject/*[in]*/, int Flags/*[in]*/, 
                                                      int iKeyUsage/*[in]*/)
{
  BSTR pStrProvName = 0;
  OLECHECK(GetDefaultInterface()->CAPI_UsrCertGetProvName(subject, Flags, iKeyUsage, (BSTR*)&pStrProvName));
  return pStrProvName;
}

long __fastcall TMEGASECCAPI::CAPI_DeleteUsrCert(BSTR subject/*[in]*/, int flag/*[in]*/, 
                                                 int keyUsage/*[in]*/)
{
  long rtn;
  OLECHECK(GetDefaultInterface()->CAPI_DeleteUsrCert(subject, flag, keyUsage, (long*)&rtn));
  return rtn;
}

BSTR __fastcall TMEGASECCAPI::CAPI_XGetUserCertificate(BSTR strProvider/*[in]*/, 
                                                       BSTR subject/*[in]*/, int Flags/*[in]*/, 
                                                       int iKeyUsage/*[in]*/, 
                                                       BSTR strSubjectAltName/*[in,def,opt]*/)
{
  BSTR pstrBase64Cert = 0;
  OLECHECK(GetDefaultInterface()->CAPI_XGetUserCertificate(strProvider, subject, Flags, iKeyUsage, 
                                                           strSubjectAltName, (BSTR*)&pstrBase64Cert));
  return pstrBase64Cert;
}

long __fastcall TMEGASECCAPI::CAPI_AcceptPKCS7(BSTR strCertificate/*[in]*/, long iFlags/*[in]*/)
{
  long rtn;
  OLECHECK(GetDefaultInterface()->CAPI_AcceptPKCS7(strCertificate, iFlags, (long*)&rtn));
  return rtn;
}

BSTR __fastcall TMEGASECCAPI::CAPICertGetPublicKey(BSTR strCert/*[in]*/, int iFlags/*[in]*/)
{
  BSTR strPublicKey = 0;
  OLECHECK(GetDefaultInterface()->CAPICertGetPublicKey(strCert, iFlags, (BSTR*)&strPublicKey));
  return strPublicKey;
}

long __fastcall TMEGASECCAPI::CAPI_ChangeP12Pwd(BSTR strPFXFilepath/*[in]*/, BSTR strOldPwd/*[in]*/, 
                                                BSTR strNewPwd/*[in]*/, long iFlags/*[in]*/)
{
  long rtn;
  OLECHECK(GetDefaultInterface()->CAPI_ChangeP12Pwd(strPFXFilepath, strOldPwd, strNewPwd, iFlags, (long*)&rtn));
  return rtn;
}

BSTR __fastcall TMEGASECCAPI::CAPI_PFXSign(BSTR sPFXfilepath/*[in]*/, BSTR strPasswd/*[in]*/, 
                                           BSTR bData/*[in]*/, long iFlags/*[in]*/, 
                                           int iHashFlags/*[in,def,opt]*/, 
                                           long iKeyUsage/*[in,def,opt]*/)
{
  BSTR strPKCS7 = 0;
  OLECHECK(GetDefaultInterface()->CAPI_PFXSign(sPFXfilepath, strPasswd, bData, iFlags, iHashFlags, 
                                               iKeyUsage, (BSTR*)&strPKCS7));
  return strPKCS7;
}

long __fastcall TMEGASECCAPI::CAPI_GetPFXCertCount(BSTR sPFXfilepath/*[in]*/, BSTR strPasswd/*[in]*/)
{
  long count;
  OLECHECK(GetDefaultInterface()->CAPI_GetPFXCertCount(sPFXfilepath, strPasswd, (long*)&count));
  return count;
}

BSTR __fastcall TMEGASECCAPI::GetVersion(long flag/*[in]*/)
{
  BSTR ver = 0;
  OLECHECK(GetDefaultInterface()->GetVersion(flag, (BSTR*)&ver));
  return ver;
}

BSTR __fastcall TMEGASECCAPI::CAPI_ReadFile(BSTR strFilePath/*[in]*/, long Flags/*[in]*/)
{
  BSTR bytesFile = 0;
  OLECHECK(GetDefaultInterface()->CAPI_ReadFile(strFilePath, Flags, (BSTR*)&bytesFile));
  return bytesFile;
}

BSTR __fastcall TMEGASECCAPI::CAPI_GetFilePath(BSTR title/*[in]*/, BSTR typeDesc/*[in]*/, 
                                               BSTR fileType/*[in]*/, BSTR initPath/*[in]*/, 
                                               BSTR initFile/*[in]*/)
{
  BSTR filePath = 0;
  OLECHECK(GetDefaultInterface()->CAPI_GetFilePath(title, typeDesc, fileType, initPath, initFile, (BSTR*)&filePath));
  return filePath;
}

BSTR __fastcall TMEGASECCAPI::CAPISign(BSTR strData/*[in]*/, BSTR signcodepage/*[in]*/, 
                                       BSTR strSubject/*[in]*/, BSTR bstrIssuer, 
                                       BSTR strProvider/*[in]*/, BSTR strPin/*[in]*/, 
                                       int iFlags/*[in]*/, int iHashFlag/*[in,def,opt]*/, 
                                       int iKeyUsage/*[in,def,opt]*/)
{
  BSTR pstrSignature = 0;
  OLECHECK(GetDefaultInterface()->CAPISign(strData, signcodepage, strSubject, bstrIssuer, 
                                           strProvider, strPin, iFlags, iHashFlag, iKeyUsage, (BSTR*)&pstrSignature));
  return pstrSignature;
}

int __fastcall TMEGASECCAPI::CAPIVerify(BSTR strSignature/*[in]*/, BSTR bData/*[in]*/, 
                                        BSTR signcodepage/*[in]*/, BSTR strSubject/*[in]*/, 
                                        int Flags/*[in]*/, int iKeyUsage/*[in]*/, int nTime/*[in]*/)
{
  int Return;
  OLECHECK(GetDefaultInterface()->CAPIVerify(strSignature, bData, signcodepage, strSubject, Flags, 
                                             iKeyUsage, nTime, (int*)&Return));
  return Return;
}

BSTR __fastcall TMEGASECCAPI::CAPI_PKCS7GetSignerCert(BSTR pkcs7signature/*[in]*/, 
                                                      long index/*[in]*/, long Flags/*[in]*/)
{
  BSTR Cert = 0;
  OLECHECK(GetDefaultInterface()->CAPI_PKCS7GetSignerCert(pkcs7signature, index, Flags, (BSTR*)&Cert));
  return Cert;
}

BSTR __fastcall TMEGASECCAPI::CAPIGetVerifiedContent(BSTR pkcs7signature/*[in]*/, 
                                                     BSTR reserved/*[in]*/, long Flags/*[in]*/)
{
  BSTR content = 0;
  OLECHECK(GetDefaultInterface()->CAPIGetVerifiedContent(pkcs7signature, reserved, Flags, (BSTR*)&content));
  return content;
}

long __fastcall TMEGASECCAPI::CAPIVerifiedContent2File(BSTR pkcs7signature/*[in]*/, 
                                                       BSTR reserved/*[in]*/, long Flags/*[in]*/, 
                                                       BSTR outfile/*[in]*/)
{
  long rtn;
  OLECHECK(GetDefaultInterface()->CAPIVerifiedContent2File(pkcs7signature, reserved, Flags, outfile, (long*)&rtn));
  return rtn;
}

BSTR __fastcall TMEGASECCAPI::CAPIComposePKCS7(BSTR strCert/*[in]*/, BSTR B64RawSignature/*[in]*/, 
                                               BSTR strData/*[in]*/, int iFlags/*[in]*/)
{
  BSTR pstrSignature = 0;
  OLECHECK(GetDefaultInterface()->CAPIComposePKCS7(strCert, B64RawSignature, strData, iFlags, (BSTR*)&pstrSignature));
  return pstrSignature;
}

BSTR __fastcall TMEGASECCAPI::GetTempPath(void)
{
  BSTR path = 0;
  OLECHECK(GetDefaultInterface()->GetTempPath((BSTR*)&path));
  return path;
}

BSTR __fastcall TMEGASECCAPI::enumProviders(long dwIndex/*[in]*/, long dwFlags/*[in]*/)
{
  BSTR pbstrProvName = 0;
  OLECHECK(GetDefaultInterface()->enumProviders(dwIndex, dwFlags, (BSTR*)&pbstrProvName));
  return pbstrProvName;
}

void __fastcall TMEGASECCAPI::acceptPKCS7(BSTR strCert/*[in]*/)
{
  GetDefaultInterface()->acceptPKCS7(strCert/*[in]*/);
}

BSTR __fastcall TMEGASECCAPI::createPKCS10(BSTR dn/*[in]*/, BSTR oid/*[in]*/)
{
  BSTR strPKCS10 = 0;
  OLECHECK(GetDefaultInterface()->createPKCS10(dn, oid, (BSTR*)&strPKCS10));
  return strPKCS10;
}

BSTR __fastcall TMEGASECCAPI::CAPI_PFXSignEx(BSTR sB64PFX/*[in]*/, BSTR strPasswd/*[in]*/, 
                                             BSTR bData/*[in]*/, long iFlags/*[in]*/, 
                                             int iHashFlag/*[in,def,opt]*/, 
                                             long iKeyUsage/*[in,def,opt]*/)
{
  BSTR strPKCS7 = 0;
  OLECHECK(GetDefaultInterface()->CAPI_PFXSignEx(sB64PFX, strPasswd, bData, iFlags, iHashFlag, 
                                                 iKeyUsage, (BSTR*)&strPKCS7));
  return strPKCS7;
}

long __fastcall TMEGASECCAPI::CAPI_CheckPFX(BSTR sB64PFX/*[in]*/, BSTR strPasswd/*[in]*/, 
                                            BSTR strSubject/*[in]*/, long iFlags/*[in]*/, 
                                            long iKeyUsage/*[in]*/)
{
  long rtn;
  OLECHECK(GetDefaultInterface()->CAPI_CheckPFX(sB64PFX, strPasswd, strSubject, iFlags, iKeyUsage, (long*)&rtn));
  return rtn;
}

long __fastcall TMEGASECCAPI::CAPI_DeleteUsrCertEx(BSTR strProvider/*[in]*/, BSTR subject/*[in]*/, 
                                                   int flag/*[in]*/, int keyUsage/*[in]*/)
{
  long rtn;
  OLECHECK(GetDefaultInterface()->CAPI_DeleteUsrCertEx(strProvider, subject, flag, keyUsage, (long*)&rtn));
  return rtn;
}

long __fastcall TMEGASECCAPI::CAPI_CheckPermission(void)
{
  long rtn;
  OLECHECK(GetDefaultInterface()->CAPI_CheckPermission((long*)&rtn));
  return rtn;
}

int __fastcall TMEGASECCAPI::CAPIPureVerify(BSTR strSignature/*[in]*/, BSTR strData/*[in]*/, 
                                            BSTR strSubject/*[in]*/, int iFlags/*[in]*/, 
                                            int iHashalg/*[in,def,opt]*/, 
                                            int iKeyUsage/*[in,def,opt]*/)
{
  int Ret;
  OLECHECK(GetDefaultInterface()->CAPIPureVerify(strSignature, strData, strSubject, iFlags, iHashalg, 
                                                 iKeyUsage, (int*)&Ret));
  return Ret;
}

void __fastcall TMEGASECCAPI::CAPISetDisplayMessageFont(BSTR font/*[in]*/, int size/*[in]*/)
{
  GetDefaultInterface()->CAPISetDisplayMessageFont(font/*[in]*/, size/*[in]*/);
}

BSTR __fastcall TMEGASECCAPI::CAPISignAndEncrypt(BSTR bSignerSubject/*[in]*/, 
                                                 BSTR bEncryptCert/*[in]*/, BSTR bData/*[in]*/, 
                                                 int iFlags/*[in]*/, int iHashFlags/*[in,def,opt]*/, 
                                                 int iKeyUsage/*[in,def,opt]*/)
{
  BSTR pstrSignCipher = 0;
  OLECHECK(GetDefaultInterface()->CAPISignAndEncrypt(bSignerSubject, bEncryptCert, bData, iFlags, 
                                                     iHashFlags, iKeyUsage, (BSTR*)&pstrSignCipher));
  return pstrSignCipher;
}

BSTR __fastcall TMEGASECCAPI::CAPIDecryptAndVerify(BSTR bCipher/*[in]*/, int iSignerIndex/*[in]*/, 
                                                   int iFlags/*[in]*/)
{
  BSTR pstrClear = 0;
  OLECHECK(GetDefaultInterface()->CAPIDecryptAndVerify(bCipher, iSignerIndex, iFlags, (BSTR*)&pstrClear));
  return pstrClear;
}

BSTR __fastcall TMEGASECCAPI::GetDecryptRtnCert(void)
{
  BSTR Cert = 0;
  OLECHECK(GetDefaultInterface()->GetDecryptRtnCert((BSTR*)&Cert));
  return Cert;
}

int __fastcall TMEGASECCAPI::CAPI_PFXPureSign(BSTR sPFXfilepath/*[in]*/, BSTR strPasswd/*[in]*/, 
                                              BSTR bData/*[in]*/, int iFlags/*[in]*/, 
                                              int iHashalg/*[in,def,opt]*/, 
                                              int iKeyUsage/*[in,def,opt]*/)
{
  int Ret;
  OLECHECK(GetDefaultInterface()->CAPI_PFXPureSign(sPFXfilepath, strPasswd, bData, iFlags, iHashalg, 
                                                   iKeyUsage, (int*)&Ret));
  return Ret;
}

long __fastcall TMEGASECCAPI::CAPIUnRegisterCertificateEx(BSTR strProvider/*[in]*/, 
                                                          long iFlags/*[in]*/)
{
  long RetError;
  OLECHECK(GetDefaultInterface()->CAPIUnRegisterCertificateEx(strProvider, iFlags, (long*)&RetError));
  return RetError;
}

void __fastcall TMEGASECCAPI::acceptPKCS7Ex(BSTR bCert/*[in]*/, BSTR bPIN/*[in]*/)
{
  GetDefaultInterface()->acceptPKCS7Ex(bCert/*[in]*/, bPIN/*[in]*/);
}

BSTR __fastcall TMEGASECCAPI::CAPISymmetricRawKeyEncrypt(int iAlgor/*[in]*/, BSTR strRawKey, 
                                                         BSTR strRawIV/*[in]*/, BSTR bData/*[in]*/, 
                                                         int iFlags/*[in,def,opt]*/)
{
  BSTR RtnCipher = 0;
  OLECHECK(GetDefaultInterface()->CAPISymmetricRawKeyEncrypt(iAlgor, strRawKey, strRawIV, bData, 
                                                             iFlags, (BSTR*)&RtnCipher));
  return RtnCipher;
}

BSTR __fastcall TMEGASECCAPI::CAPISymmetricRawKeyDecrypt(int iAlgor/*[in]*/, BSTR strRawKey, 
                                                         BSTR strRawIV/*[in]*/, BSTR bCipher/*[in]*/, 
                                                         int iFlags/*[in,def,opt]*/)
{
  BSTR RtnClear = 0;
  OLECHECK(GetDefaultInterface()->CAPISymmetricRawKeyDecrypt(iAlgor, strRawKey, strRawIV, bCipher, 
                                                             iFlags, (BSTR*)&RtnClear));
  return RtnClear;
}

BSTR __fastcall TMEGASECCAPI::CAPIPKCS7GetEncryptedDigest(BSTR pkcs7signature/*[in]*/, 
                                                          long Flags/*[in]*/)
{
  BSTR content = 0;
  OLECHECK(GetDefaultInterface()->CAPIPKCS7GetEncryptedDigest(pkcs7signature, Flags, (BSTR*)&content));
  return content;
}

long __fastcall TMEGASECCAPI::CAPI_InstallCert(BSTR strCertificate/*[in]*/, long iFlags/*[in]*/)
{
  long rtn;
  OLECHECK(GetDefaultInterface()->CAPI_InstallCert(strCertificate, iFlags, (long*)&rtn));
  return rtn;
}

long __fastcall TMEGASECCAPI::CAPI_WriteFile(BSTR strB64Data/*[in]*/, BSTR strFilePath/*[in]*/, 
                                             long iFlags/*[in]*/)
{
  long rtn;
  OLECHECK(GetDefaultInterface()->CAPI_WriteFile(strB64Data, strFilePath, iFlags, (long*)&rtn));
  return rtn;
}

BSTR __fastcall TMEGASECCAPI::GetMyDocumentsPath(void)
{
  BSTR path = 0;
  OLECHECK(GetDefaultInterface()->GetMyDocumentsPath((BSTR*)&path));
  return path;
}

int __fastcall TMEGASECCAPI::CAPISymmetricEncryptFile(int iAlgor/*[in]*/, BSTR strPasswd/*[in]*/, 
                                                      BSTR bSrcPath/*[in]*/, BSTR bDstPath/*[in]*/)
{
  int rtn;
  OLECHECK(GetDefaultInterface()->CAPISymmetricEncryptFile(iAlgor, strPasswd, bSrcPath, bDstPath, (int*)&rtn));
  return rtn;
}

int __fastcall TMEGASECCAPI::CAPISymmetricDecryptFile(int iAlgor/*[in]*/, BSTR strPasswd/*[in]*/, 
                                                      BSTR bSrcPath/*[in]*/, BSTR bDstPath/*[in]*/)
{
  int rtn;
  OLECHECK(GetDefaultInterface()->CAPISymmetricDecryptFile(iAlgor, strPasswd, bSrcPath, bDstPath, (int*)&rtn));
  return rtn;
}

BSTR __fastcall TMEGASECCAPI::CAPI_UsrCertGetContainerName(BSTR subject/*[in]*/, int Flags/*[in]*/, 
                                                           int iKeyUsage/*[in]*/)
{
  BSTR pStrContainerName = 0;
  OLECHECK(GetDefaultInterface()->CAPI_UsrCertGetContainerName(subject, Flags, iKeyUsage, (BSTR*)&pStrContainerName));
  return pStrContainerName;
}

long __fastcall TMEGASECCAPI::CAPIPFXImportSCEx(BSTR sPFXfilepath/*[in]*/, BSTR strPasswd/*[in]*/, 
                                                BSTR strCSP/*[in]*/, BSTR strPin/*[in]*/, 
                                                int iFlags/*[in]*/)
{
  long rtn;
  OLECHECK(GetDefaultInterface()->CAPIPFXImportSCEx(sPFXfilepath, strPasswd, strCSP, strPin, iFlags, (long*)&rtn));
  return rtn;
}

int __fastcall TMEGASECCAPI::CAPIImportPKCS12Ex(BSTR strPwd/*[in]*/, BSTR strFilePath/*[in]*/, 
                                                BSTR strFileName/*[in]*/, 
                                                BSTR strFriendlyName/*[in]*/, int iFlags/*[in]*/)
{
  int RetError;
  OLECHECK(GetDefaultInterface()->CAPIImportPKCS12Ex(strPwd, strFilePath, strFileName, 
                                                     strFriendlyName, iFlags, (int*)&RetError));
  return RetError;
}

void __fastcall TMEGASECCAPI::CGCAPIStockCertRegist(BSTR bUserID/*[in]*/, BSTR bConsoleCode/*[in]*/, 
                                                    BSTR bSubject/*[in]*/, BSTR bCAPath/*[in]*/, 
                                                    BSTR bPassword/*[in]*/, 
                                                    BSTR bCertSetFlag/*[in]*/)
{
  GetDefaultInterface()->CGCAPIStockCertRegist(bUserID/*[in]*/, bConsoleCode/*[in]*/, 
                                               bSubject/*[in]*/, bCAPath/*[in]*/, bPassword/*[in]*/, 
                                               bCertSetFlag/*[in]*/);
}

void __fastcall TMEGASECCAPI::CGCAPIStockCertConfig(BSTR bUserID/*[in]*/, BSTR bConsoleCode/*[in]*/, 
                                                    BSTR bSubject/*[in]*/, int iFlags/*[in]*/)
{
  GetDefaultInterface()->CGCAPIStockCertConfig(bUserID/*[in]*/, bConsoleCode/*[in]*/, 
                                               bSubject/*[in]*/, iFlags/*[in]*/);
}

BSTR __fastcall TMEGASECCAPI::CGCAPIStockSign(BSTR bUserID/*[in]*/, BSTR bConsoleCode/*[in]*/, 
                                              BSTR bSubject/*[in]*/, BSTR bData/*[in]*/, 
                                              int iFlags/*[in]*/, int iKeyUsage/*[in]*/, 
                                              int iHashFlags/*[in,def,opt]*/)
{
  BSTR pstrSignature = 0;
  OLECHECK(GetDefaultInterface()->CGCAPIStockSign(bUserID, bConsoleCode, bSubject, bData, iFlags, 
                                                  iKeyUsage, iHashFlags, (BSTR*)&pstrSignature));
  return pstrSignature;
}

BSTR __fastcall TMEGASECCAPI::CGCAPIStockPureSign(BSTR bUserID/*[in]*/, BSTR bConsoleCode/*[in]*/, 
                                                  BSTR bSubject/*[in]*/, BSTR bData/*[in]*/, 
                                                  int iFlags/*[in]*/, int iKeyUsage/*[in]*/, 
                                                  int iHashFlags/*[in,def,opt]*/)
{
  BSTR pstrSignature = 0;
  OLECHECK(GetDefaultInterface()->CGCAPIStockPureSign(bUserID, bConsoleCode, bSubject, bData, iFlags, 
                                                      iKeyUsage, iHashFlags, (BSTR*)&pstrSignature));
  return pstrSignature;
}

BSTR __fastcall TMEGASECCAPI::CGCAPIStockGetSN(void)
{
  BSTR pstrSignature = 0;
  OLECHECK(GetDefaultInterface()->CGCAPIStockGetSN((BSTR*)&pstrSignature));
  return pstrSignature;
}

BSTR __fastcall TMEGASECCAPI::CGCAPIStockGetCert(void)
{
  BSTR RtnCert = 0;
  OLECHECK(GetDefaultInterface()->CGCAPIStockGetCert((BSTR*)&RtnCert));
  return RtnCert;
}

BSTR __fastcall TMEGASECCAPI::CGCAPIStockPFXPureSign(BSTR bFilepath/*[in]*/, BSTR bPassword/*[in]*/, 
                                                     BSTR bData/*[in]*/, BSTR bSubject/*[in]*/, 
                                                     int iFlags/*[in]*/, int iHashFlag/*[in]*/, 
                                                     int iKeyUsage/*[in]*/)
{
  BSTR pstrSignature = 0;
  OLECHECK(GetDefaultInterface()->CGCAPIStockPFXPureSign(bFilepath, bPassword, bData, bSubject, 
                                                         iFlags, iHashFlag, iKeyUsage, (BSTR*)&pstrSignature));
  return pstrSignature;
}

int __fastcall TMEGASECCAPI::createFolder(BSTR bPath/*[in]*/)
{
  int rtn;
  OLECHECK(GetDefaultInterface()->createFolder(bPath, (int*)&rtn));
  return rtn;
}

BSTR __fastcall TMEGASECCAPI::selectFolder(BSTR bTitle/*[in]*/)
{
  BSTR rtn = 0;
  OLECHECK(GetDefaultInterface()->selectFolder(bTitle, (BSTR*)&rtn));
  return rtn;
}

BSTR __fastcall TMEGASECCAPI::CAPISymmetricRawKeyEncryptEx(int iAlgor/*[in]*/, int iMode/*[in]*/, 
                                                           BSTR strRawKey/*[in]*/, 
                                                           BSTR strRawIV/*[in]*/, BSTR bData/*[in]*/, 
                                                           int iFlags/*[in,def,opt]*/)
{
  BSTR RtnCipher = 0;
  OLECHECK(GetDefaultInterface()->CAPISymmetricRawKeyEncryptEx(iAlgor, iMode, strRawKey, strRawIV, 
                                                               bData, iFlags, (BSTR*)&RtnCipher));
  return RtnCipher;
}

BSTR __fastcall TMEGASECCAPI::CAPISymmetricRawKeyDecryptEx(int iAlgor/*[in]*/, int iMode/*[in]*/, 
                                                           BSTR strRawKey/*[in]*/, 
                                                           BSTR strRawIV/*[in]*/, 
                                                           BSTR bCipher/*[in]*/, 
                                                           int iFlags/*[in,def,opt]*/)
{
  BSTR RtnClear = 0;
  OLECHECK(GetDefaultInterface()->CAPISymmetricRawKeyDecryptEx(iAlgor, iMode, strRawKey, strRawIV, 
                                                               bCipher, iFlags, (BSTR*)&RtnClear));
  return RtnClear;
}

BSTR __fastcall TMEGASECCAPI::createPKCS10Ex(BSTR dn/*[in]*/, BSTR oid/*[in]*/, BSTR bPIN/*[in]*/)
{
  BSTR strPKCS10 = 0;
  OLECHECK(GetDefaultInterface()->createPKCS10Ex(dn, oid, bPIN, (BSTR*)&strPKCS10));
  return strPKCS10;
}

int __fastcall TMEGASECCAPI::WriteReg(BSTR key/*[in]*/, BSTR path/*[in]*/, BSTR name/*[in]*/, 
                                      BSTR type/*[in]*/, BSTR value/*[in]*/, int iFlags/*[in]*/)
{
  int res;
  OLECHECK(GetDefaultInterface()->WriteReg(key, path, name, type, value, iFlags, (int*)&res));
  return res;
}

BSTR __fastcall TMEGASECCAPI::ReadReg(BSTR key/*[in]*/, BSTR path/*[in]*/, BSTR name/*[in]*/, 
                                      BSTR type/*[in]*/, int iFlags/*[in]*/)
{
  BSTR regValue = 0;
  OLECHECK(GetDefaultInterface()->ReadReg(key, path, name, type, iFlags, (BSTR*)&regValue));
  return regValue;
}

int __fastcall TMEGASECCAPI::CAPISetDisplayLogo(BSTR strCustomize/*[in]*/, int iFlags/*[in]*/)
{
  int Ret;
  OLECHECK(GetDefaultInterface()->CAPISetDisplayLogo(strCustomize, iFlags, (int*)&Ret));
  return Ret;
}

int __fastcall TMEGASECCAPI::CAPIPureSignEx(BSTR strData/*[in]*/, BSTR signcodepage/*[in]*/, 
                                            BSTR strSubject/*[in]*/, BSTR bstrIssuer, 
                                            BSTR strProvider/*[in]*/, BSTR strPin/*[in]*/, 
                                            int iFlags/*[in]*/, int iHashFlag/*[in,def,opt]*/, 
                                            int iKeyUsage/*[in,def,opt]*/)
{
  int Ret;
  OLECHECK(GetDefaultInterface()->CAPIPureSignEx(strData, signcodepage, strSubject, bstrIssuer, 
                                                 strProvider, strPin, iFlags, iHashFlag, iKeyUsage, (int*)&Ret));
  return Ret;
}

int __fastcall TMEGASECCAPI::CAPIDecrypt2File2(BSTR strInputFilePath/*[in]*/, 
                                               BSTR strOutFilePath/*[in]*/, int Flags/*[in]*/, 
                                               int iKeyUsage/*[in]*/)
{
  int iRtn;
  OLECHECK(GetDefaultInterface()->CAPIDecrypt2File2(strInputFilePath, strOutFilePath, Flags, 
                                                    iKeyUsage, (int*)&iRtn));
  return iRtn;
}


};     // namespace Megaseccapiatllib_tlb


// *********************************************************************//
// The Register function is invoked by the IDE when this module is 
// installed in a Package. It provides the list of Components (including
// OCXes) implemented by this module. The following implementation
// informs the IDE of the OCX proxy classes implemented here.
// *********************************************************************//
namespace Megaseccapiatllib_ocx
{

void __fastcall PACKAGE Register()
{
  // [1]
  System::Classes::TComponentClass cls_ocx[] = {
                              __classid(Megaseccapiatllib_tlb::TMEGASECCAPI)
                           };
  System::Classes::RegisterComponents("Speedy", cls_ocx,
                     sizeof(cls_ocx)/sizeof(cls_ocx[0])-1);
}

};     // namespace Megaseccapiatllib_ocx
