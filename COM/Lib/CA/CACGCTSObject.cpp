//---------------------------------------------------------------------------

#pragma hdrstop
#include <cstdio>
#include <cstring>

//#include "System.hpp"

#include "CACGCTSObject.h"

//----------------------------- CTSCGCCAObject ------------------------------
UFC::AnsiString CTSCGCCAObject::GetErrorMsgA(int ErrorID)
{
	UFC::AnsiString errMsg = "";
	switch (ErrorID)
	{
		case CG_RTN_SUCCESS:                    errMsg = "success";                                                    break;  //   0
		case CG_RTN_ERROR:                      errMsg = "general error";                                              break;  //5001
		case CG_RTN_MEMALLOC_ERROR:             errMsg = "Memory Allocation Error";                                    break;  //5002
		case CG_RTN_BUFFER_TOO_SMALL:           errMsg = "Buffer too small";                                           break;  //5003
		case CG_RTN_FUNCTION_UNSUPPORT:         errMsg = "function not support";                                       break;  //5004
		case CG_RTN_INVALID_PARAM:              errMsg = "Invalid parameter";                                          break;  //5005
		case CG_RTN_INVALID_HANDLE:             errMsg = "Invalid handle";                                             break;  //5006
		case CG_RTN_LIB_EXPIRE:                 errMsg = "TrialVersion Library is expired!";                           break;  //5007
		case CG_RTN_BASE64_ERROR:               errMsg = "Base64 Encoding/Decoding Error";                             break;  //5008
		case CG_RTN_HEX_ERROR:                  errMsg = "Hex Encoding/Decoding Error";                                break;  //5009
		case CG_RTN_CERT_NOT_FOUND:             errMsg = "certificate not found in MS CryptoAPI Database";             break;  //5010
		case CG_RTN_CERT_EXPIRED:               errMsg = "Certicate Expire";                                           break;  //5011
		case CG_RTN_CERT_NOT_YET_VALID:         errMsg = "Certificate can not be used now";                            break;  //5012
		case CG_RTN_CERT_EXPIRE_OR_NOT_YET_USE: errMsg = "Some certificates are expired, some can not be used now!!!"; break;  //5013
		case CG_RTN_CERT_DENIED:                errMsg = "Certificate subject not match";                              break;  //5014
		case CG_RTN_CERT_NOISSUER:              errMsg = "Unable to find certificate issuer";                          break;  //5015
		case CG_RTN_CERT_BAD_SIGNATURE:         errMsg = "Certificate signature is invalid";                           break;  //5016
		case CG_RTN_CERT_INVALID_KEYUSAGE:      errMsg = "Invalid certificate keyusage";                               break;  //5017
		case CG_RTN_CERT_REVOKED:               errMsg = "Certificate is revoked";                                     break;  //5020
//		case CG_RTN_CERT_UNSPECIFIED:           errMsg = "Certificate is revoked";                                     break;  //5020
		case CG_RTN_CERT_KEY_COMPROMISED:       errMsg = "Certificate is revoked(key compromised)";                    break;  //5021
		case CG_RTN_CERT_CA_COMPROMISED:        errMsg = "Certificate is revoked(CA compromised)";                     break;  //5022
		case CG_RTN_CERT_AFFILIATION_CHANGED:   errMsg = "Certificate is revoked(affiliation changed)";                break;  //5023
		case CG_RTN_CERT_SUPERSEDED:            errMsg = "Certificate is revoked(superseded)";                         break;  //5024
		case CG_RTN_CERT_CESSATION:             errMsg = "Certificate is revoked(cessation)";                          break;  //5025
		case CG_RTN_CERT_HOLD:                  errMsg = "Certificate is revoked(hold)";                               break;  //5026
		case CG_RTN_CERT_REMOVEFROMCRL:         errMsg = "Certificate is revoked(hold)";                               break;  //5028
		case CG_RTN_CRL_EXPIRED:                errMsg = "CRL expired";                                                break;  //5030
		case CG_RTN_CRL_NOT_YET_VALID:          errMsg = "CRL not yet valid";                                          break;  //5031
		case CG_RTN_CRL_NOT_FOUND:              errMsg = "CRL not found";                                              break;  //5032
		case CG_RTN_CRL_BAD_SIGNATURE:          errMsg = "CRL signature invalid";                                      break;  //5034
		case CG_RTN_GET_DIGEST_ERROR:           errMsg = "Get Digest error";                                           break;  //5035
		case CG_RTN_BAD_SIGNATURE:              errMsg = "Invalid data signature";                                     break;  //5036
		case CG_RTN_BAD_CONTENT:                errMsg = "Content not match";                                          break;  //5037
		case CG_RTN_BAD_CAPTCHA:                errMsg = "Bad Captcha";                                                break;  //5038
		case CG_RTN_INVALID_CERT:               errMsg = "Incorrect Certificate format";                               break;  //5040
		case CG_RTN_INVALID_CRL:                errMsg = "Incorrect CRL format";                                       break;  //5041
		case CG_RTN_INVALID_PKCS7:              errMsg = "Incorrect PKCS7 format";                                     break;  //5042
		case CG_RTN_INVALID_KEY:                errMsg = "Incorrect KEY format";                                       break;  //5043
		case CG_RTN_INVALID_CERTREQ:            errMsg = "Incorrect PKCS10 format";                                    break;  //5044
		case CG_RTN_INVALID_FORMAT:             errMsg = "Incorrect format";                                           break;  //5045
		case CG_RTN_INVALID_PKCS12:             errMsg = "Invalid PKCS12";                                             break;  //5046
		case CG_RTN_OBJ_NOT_FOUND:              errMsg = "Object not Found";                                           break;  //5050
		case CG_RTN_PKCS7_NO_CONTENT:           errMsg = "PKCS7 no Content";                                           break;  //5051
		case CG_RTN_PKCS7_NO_CERTIFICATE:       errMsg = "PKCS7 no Certificate";                                       break;  //5052
		case CG_RTN_PKCS7_NO_SIGNERINFO:        errMsg = "PKCS7 no SignerInfo";                                        break;  //5053
		case CG_RTN_UNMATCH_CERT_KEY:           errMsg = "Certificate/PrivateKey not match";                           break;  //5060
		case CG_RTN_SIGN_ERROR:                 errMsg = "Sign error";                                                 break;  //5061
		case CG_RTN_VERIFY_ERROR:               errMsg = "Verify error";                                               break;  //5062
		case CG_RTN_ENCRYPT_ERROR:              errMsg = "Encrypt error";                                              break;  //5063
		case CG_RTN_DECRYPT_ERROR:              errMsg = "Decrypt error";                                              break;  //5064
		case CG_RTN_GENKEY_ERROR:               errMsg = "Generate key error";                                         break;  //5065
		case CG_RTN_DELETE_USR_CERT_ERROR:      errMsg = "Delete usr Cert Error";                                      break;  //5066
		case CG_RTN_BAD_ALGID:                  errMsg = "Bad Algid";                                                  break;  //5067
		case CG_RTN_OPERATION_CANCELED:         errMsg = "Operation Canceled";                                         break;  //5070
		case CG_RTN_PASSWD_INVALID:             errMsg = "Password Invalid";                                           break;  //5071
		case CG_RTN_SCARD_BLOCKED:              errMsg = "Scard Blocked";                                              break;  //5072
		case CG_RTN_REPLUG_TIMEOUT:             errMsg = "Replug Timeout";                                             break;  //5073
		case CG_RTN_DEVICE_DISABLED:            errMsg = "Device Disabled";                                            break;  //5074
		case CG_RTN_XMLPARSE_ERROR:             errMsg = "XML Parse Error";                                            break;  //5080
		case CG_RTN_XMLTAG_NOTFOUND:            errMsg = "XML Tag not Found";                                          break;  //5081
		case CGCAPI_RTN_OPENSTORE_ERROR:        errMsg = "Open Store Error";                                           break;  //5201
		case CGCAPI_RTN_CREATECHAIN_ERROR:      errMsg = "Create Chain Error";                                         break;  //5202
		case CGCAPI_RTN_CRYPTCONTEXT_ERROR:     errMsg = "Crypt Context Error";                                        break;  //5203
		case CGCAPI_RTN_NO_PRIVATE_KEY:         errMsg = "No Private Key";                                             break;  //5204
		case CGCAPI_RTN_UNEXPORTABLE:           errMsg = "UnExportable";                                               break;  //5205
		case CGCAPI_RTN_STORE_ACCESSDENY:       errMsg = "Store Accessdeny";                                           break;  //5206
		case CG_RTN_UNICODE_ERROR:              errMsg = "Unicode Error";                                              break;  //5901
		case CG_RTN_FILE_NOT_FOUND:             errMsg = "File Not Found";                                             break;  //5902
		case CG_RTN_PATH_NOT_FOUND:             errMsg = "Path Not Found";                                             break;  //5903
		case CG_RTN_BAD_NETPATH:                errMsg = "Network path was not found";                                 break;  //5904
		case CG_RTN_LOGON_FAILURE:              errMsg = "Unknown logon user name or bad password";                    break;  //5905
		case CG_RTN_ACCESS_DENIED:              errMsg = "Access Denied, no right";                                    break;  //5906
		case CG_RTN_BAD_INITCODE:               errMsg = "Bad Initcode";                                               break;  //5907
		case CG_RTN_INITCODE_NOT_VERIFY:        errMsg = "Initcode not verify";                                        break;  //5908
		case CG_RTN_BAD_PASSWD:                 errMsg = "Bad Password";                                               break;  //5909
		default:                                errMsg = "Unknown Error";
	}
	return errMsg;
}  //CTSCGCCAObject::GetErrorMsgA()

//---------------------------------------------------------------------------
CTSCGCCAObject::CTSCGCCAObject(const UFC::AnsiString& CommonName, const UFC::AnsiString& DllFileName, UFC::BufferedLog* LogPtr)
:CApiCADllObject(caCGC, CommonName, DllFileName, LogPtr)
,FfnCGCAPI_CertEncrypt(NULL)
,FfnCGCAPI_Sign(NULL)
,FfnCGCAPI_SignEx(NULL)
,FfnCGCAPI_SignAndEncrypt(NULL)
,FfnCGCAPI_Verify(NULL)
,FfnCGCAPI_ComposeSignature(NULL)
,FfnCGCAPI_VerifyDigest(NULL)
,FfnCGCAPI_Encrypt(NULL)
,FfnCGCAPI_Decrypt(NULL)
,FfnCGCAPI_DecryptAndVerify(NULL)
,FfnCGCAPI_Hash(NULL)
,FfnCGCAPI_ExportPKCS12(NULL)
,FfnCGCAPI_CertGetDigest(NULL)
,FfnCGCAPI_CertGetIssuer(NULL)
,FfnCGCAPI_CertGetNotAfter(NULL)
,FfnCGCAPI_CertGetNotBefore(NULL)
,FfnCGCAPI_CertGetFXMLExtension(NULL)
,FfnCGCAPI_CertGetSubject(NULL)
,FfnCGCAPI_RegisterCertificate(NULL)
,FfnCGCAPI_ComposePKCS7(NULL)
,FfnCGCAPI_PKCS7GetUserCertificate(NULL)
,FfnCGCAPI_ShowCertificate(NULL)
,FfnCGCAPI_PureSign(NULL)
,FfnCGCAPI_PFXGetCertCount(NULL)
,FfnCGCAPIMemAlloc(NULL)
,FfnCGCAPIMemRealloc(NULL)
,FfnCGCAPIMemFree(NULL)
,FfnCGXCAPI_SignAndEncrypt(NULL)
,FfnCGXCAPI_ComposeSignature(NULL)
,FfnCGXCAPI_DecryptAndVerify(NULL)
,FfnCGXCAPI_GetUserCertificate(NULL)
,FfnCGXCAPI_GetCertificate(NULL)
,FfnCGXCAPI_GenerateKey(NULL)
,FfnCGXCAPI_GetPrivateKey(NULL)
,FfnCGXCAPI_CertGetPublicKey(NULL)
,FfnCGXCAPI_Encrypt(NULL)
,FfnCGXCAPI_EncryptInit(NULL)
,FfnCGXCAPI_EncryptUpdate(NULL)
,FfnCGXCAPI_EncryptFinal(NULL)
,FfnCGXCAPI_Decrypt(NULL)
,FfnCGXCAPI_DecryptInit(NULL)
,FfnCGXCAPI_DecryptUpdate(NULL)
,FfnCGXCAPI_DecryptFinal(NULL)
,FfnCGXCAPI_HashInit(NULL)
,FfnCGXCAPI_HashUpdate(NULL)
,FfnCGXCAPI_HashFinal(NULL)
,FfnCGXCAPI_CertGetSerialNumber(NULL)
,FfnCGXCAPI_CertGetSubject(NULL)
,FfnCGXCAPI_CertGetIssuer(NULL)
,FfnCGXCAPI_CertGetNotBefore(NULL)
,FfnCGXCAPI_CertGetNotAfter(NULL)
,FfnCGXCAPI_CertGetDigest(NULL)
,FfnCGXCAPI_ExportPKCS12(NULL)
,FfnCGXCAPI_Sign(NULL)
,FfnCGXCAPI_Verify(NULL)
,FfnCGXCAPI_PKCS7GetSignerInfoCount(NULL)
,FfnCGXCAPI_PKCS7GetSignerInfo(NULL)
,FfnCGXCAPI_PKCS7GetSignerCertificate(NULL)
,FfnCGXCAPI_PKCS7GetContent(NULL)
,FfnCGXCAPI_PKCS7GetUserCertificate(NULL)
,FfnCGXCAPI_PKCS7AddSignerCertificate(NULL)
,FfnCGXCAPI_PureSign(NULL)
,FfnCGXCAPI_PureVerify(NULL)
,FfnCGXCAPI_Encode(NULL)
,FfnCGXCAPI_EncodeObject(NULL)
,FfnCGXCAPI_Decode(NULL)
,FfnCGXCAPI_DecodeObject(NULL)
,FfnCGXCAPI_LoadStore(NULL)
,FfnCGXCAPI_LoadStoreByName(NULL)
,FfnCGXCAPI_VerifyCertChain(NULL)
,FfnCGXCAPI_AcceptPKCS7(NULL)
,FfnCGXCAPI_DeleteUsrCert(NULL)
,FfnCGXCAPI_ChangeP12Pwd(NULL)
,FfnCGXCAPIFreeHandle(NULL)
,FfnCGXCAPIFreeBlob(NULL)
,FfnCGCAPI_PFXP7Sign(NULL)
,FfnCGCAPI_CertGetSerialNumber(NULL)
,FfnCGCAPI_CertGetSubjectW(NULL)
,FfnCGXCAPI_GetCertificates(NULL)
,FfnCGCAPI_GetUserCertificate(NULL)
,FfnCGCAPI_GetUserCertificates(NULL)
,FfnGetDLLVersion(NULL)
{
	FIsWorking = LoadCADynamicLibrary();
	if (FIsWorking)
	{
		char* versionPtr = 0;
		if (FfnGetDLLVersion(0, &versionPtr) == CG_RTN_SUCCESS)
		{
			FLogPtr->fprintf(" %s() Dll Version[%s] !", __func__, versionPtr);
			FfnCGCAPIMemFree(versionPtr);
		}

//int CGCAPI_Sign(const void* pbData, unsigned long lData,
//                const char* strSubject, int iFlags, int iKeyUsage, char** pstrSignature);
		 UFC::AnsiString plainText = "This is a book.";
		 UFC::AnsiString base64Text = UFC::Base64EncodeStr(plainText);
		 FLogPtr->fprintf(" %s() PlainText[%s] => Base64[%s]!", __func__, plainText.c_str(), base64Text.c_str());

//		 UFC::AnsiString subject = "OU=RA-mlfututest\nCN=A111111111";
//		 UFC::AnsiString subject = "OU=mlfututest\nCN=TWA111111111";
//		 UFC::AnsiString subject = "OU=mlfututest\nCN=A111111111";
//		 UFC::AnsiString subject = "CN=TWA111111111";
		 UFC::AnsiString subject = "OU=TSSTEST\nCN=A121708511";
		 UFC::AnsiString issuerSubject = "";
		 UFC::AnsiString provider = "";
		 UFC::AnsiString pin = "";
		 FLogPtr->fprintf(" %s() subject[%s] !", __func__, subject.c_str());
		 int iFlags = 0;
		 iFlags &= ~CGCAPI_FLAG_SELCERT_MANUAL;
//		 iFlags |= CG_FLAG_SUBJECT_PARTIALMATCH | CGCAPI_FLAG_SELCERT_AUTO | CGCAPI_FLAG_SELCERT_CHECKVALID;
		 iFlags |= CG_FLAG_SUBJECT_PARTIALMATCH | CGCAPI_FLAG_SELCERT_AUTO | CGCAPI_FLAG_SELCERT_CHECKVALID | CG_FLAG_BASE64_ENCODE;

		 int iKeyUsage = 0;
		 char* sig = 0;
		 int isigLen = 0;
		 char* cert = 0;

		 int rtn = 0;
//		 rtn = FfnCGCAPI_PureSign(plainText.c_str(), plainText.Length(), subject.c_str(),
		 rtn = FfnCGCAPI_PureSign(base64Text.c_str(), base64Text.Length(), subject.c_str(),
								  issuerSubject.c_str(), provider.c_str(), pin.c_str(),
								  iFlags, CG_ALGOR_SHA256 | CG_FLAG_DOHASH,
								  iKeyUsage, &sig, &isigLen, &cert);
		 FLogPtr->fprintf(" %s() CGCAPI_PureSign rtn=%d !", __func__, rtn);
//int CGCAPI_PureSign(const void* pbData, unsigned long lData, const char* strSubject, const char* strIssuerSubject, const char* strProvider, const char* strPin, int iFlags, int iHashalg, int iKeyUsage, char** pstrSignature, int* lSignature, char** pstrCert);

//		 rtn = FfnCGCAPI_Sign(plainText.c_str(), plainText.Length(), subject.c_str(), iFlags, 0, &cert);
//		 FLogPtr->fprintf(" %s() CGCAPI_Sign rtn=%d !", __func__, rtn);
		 if (rtn == CG_RTN_SUCCESS)
		 {
			 FLogPtr->fprintf(" %s() sig", __func__);
			 FLogPtr->fprintf(" %s() [%s]%d !", __func__, sig, isigLen);
			 UFC::AnsiString sig64 = UFC::Base64EncodeBuf(sig, isigLen);
			 FLogPtr->fprintf(" %s() sig64[%s] !", __func__, sig64.c_str());

			 FLogPtr->fprintf(" %s() ", __func__);
			 FLogPtr->fprintf(" %s() cert", __func__);
			 FLogPtr->fprintf(" %s() [%s]", __func__, cert);

			 rtn = FfnCGCAPI_GetUserCertificate(0,
												subject.c_str(),
												0,
												iFlags,
												0,
												&cert);
			 FLogPtr->fprintf(" %s() CGCAPI_GetUserCertificate rtn=%d !", __func__, rtn);
			 if (rtn == CG_RTN_SUCCESS)
			 {
				 FLogPtr->fprintf(" %s() CGCAPI_GetUserCertificate[%s] !", __func__, cert);

				 char* subjectBuff = 0;
				 rtn = FfnCGCAPI_CertGetSubject(cert, 0, &subjectBuff);
				 FLogPtr->fprintf(" %s() FfnCGCAPI_CertGetSubject rtn=%d !", __func__, rtn);
				 if (rtn == CG_RTN_SUCCESS)
					 FLogPtr->fprintf(" %s() FfnCGCAPI_CertGetSubject[%s] !", __func__, subjectBuff);

				 char* serialNumber = 0;
				 rtn = FfnCGCAPI_CertGetSerialNumber(cert, 0, &serialNumber);
				 FLogPtr->fprintf(" %s() SerialNumber rtn=%d!", __func__, rtn);
				 if (rtn == CG_RTN_SUCCESS)
					 FLogPtr->fprintf(" %s() SerialNumber[%s] !", __func__, serialNumber);

				 char notBefore[20];
				 rtn = FfnCGCAPI_CertGetNotBefore(cert, 0, notBefore);
				 FLogPtr->fprintf(" %s() NotBefore rtn=%d!", __func__, rtn);
				 if (rtn == CG_RTN_SUCCESS)
					 FLogPtr->fprintf(" %s() NotBefore[%s] !", __func__, notBefore);

				 char notAfter[20];
				 rtn = FfnCGCAPI_CertGetNotAfter(cert, 0, notAfter);
				 FLogPtr->fprintf(" %s() NotAfter rtn=%d!", __func__, rtn);
				 if (rtn == CG_RTN_SUCCESS)
					 FLogPtr->fprintf(" %s() NotAfter[%s] !", __func__, notAfter);
			 }


		 }

	}
	else
		FLogPtr->fprintf(" %s() Load Dll[%s] failed !", __func__, FDLLFileName.c_str());
}  //CTSCGCCAObject::CTSCGCCAObject()
//---------------------------------------------------------------------------
bool CTSCGCCAObject::LoadCADynamicLibrary()
{
	bool isSuccess = false;
#ifdef WIN32
//	String dllFileNameW = String(FDLLFileName.c_str());

//	FDllHandle = ::LoadLibrary(dllFileNameW.c_str());
	FDllHandle = ::LoadLibraryA(FDLLFileName.c_str());

	if (FDllHandle != NULL)
	{
		FLogPtr->fprintf(" %s() Load Dll[%s] success !", __func__, FDLLFileName.c_str());
		isSuccess = true;
		int loadFunctionCount = 0;

		FfnCGCAPI_CertEncrypt = (CGCAPI_CertEncryptPtr)::GetProcAddress(FDllHandle, "CGCAPI_CertEncrypt");
		if (FfnCGCAPI_CertEncrypt == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_CertEncrypt\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_Sign = (CGCAPI_SignPtr)::GetProcAddress(FDllHandle, "CGCAPI_Sign");
		if (FfnCGCAPI_Sign == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_Sign\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_SignEx = (CGCAPI_SignExPtr)::GetProcAddress(FDllHandle, "CGCAPI_SignEx");
		if (FfnCGCAPI_SignEx == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_SignEx\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_SignAndEncrypt = (CGCAPI_SignAndEncryptPtr)::GetProcAddress(FDllHandle, "CGCAPI_SignAndEncrypt");
		if (FfnCGCAPI_SignAndEncrypt == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_SignAndEncrypt\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_Verify = (CGCAPI_VerifyPtr)::GetProcAddress(FDllHandle, "CGCAPI_Verify");
		if (FfnCGCAPI_Verify == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_Verify\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_ComposeSignature = (CGCAPI_ComposeSignaturePtr)::GetProcAddress(FDllHandle, "CGCAPI_ComposeSignature");
		if (FfnCGCAPI_ComposeSignature == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_ComposeSignature\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_VerifyDigest = (CGCAPI_VerifyDigestPtr)::GetProcAddress(FDllHandle, "CGCAPI_VerifyDigest");
		if (FfnCGCAPI_VerifyDigest == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_VerifyDigest\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_Encrypt = (CGCAPI_EncryptPtr)::GetProcAddress(FDllHandle, "CGCAPI_Encrypt");
		if (FfnCGCAPI_Encrypt == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_Encrypt\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_Decrypt = (CGCAPI_DecryptPtr)::GetProcAddress(FDllHandle, "CGCAPI_Decrypt");
		if (FfnCGCAPI_Decrypt == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_Decrypt\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_DecryptAndVerify = (CGCAPI_DecryptAndVerifyPtr)::GetProcAddress(FDllHandle, "CGCAPI_DecryptAndVerify");
		if (FfnCGCAPI_DecryptAndVerify == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_DecryptAndVerify\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_Hash = (CGCAPI_HashPtr)::GetProcAddress(FDllHandle, "CGCAPI_Hash");
		if (FfnCGCAPI_Hash == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_Hash\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_ExportPKCS12 = (CGCAPI_ExportPKCS12Ptr)::GetProcAddress(FDllHandle, "CGCAPI_ExportPKCS12");
		if (FfnCGCAPI_ExportPKCS12 == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_ExportPKCS12\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_CertGetDigest = (CGCAPI_CertGetDigestPtr)::GetProcAddress(FDllHandle, "CGCAPI_CertGetDigest");
		if (FfnCGCAPI_CertGetDigest == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_CertGetDigest\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_CertGetIssuer = (CGCAPI_CertGetIssuerPtr)::GetProcAddress(FDllHandle, "CGCAPI_CertGetIssuer");
		if (FfnCGCAPI_CertGetIssuer == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_CertGetIssuer\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_CertGetNotAfter = (CGCAPI_CertGetNotAfterPtr)::GetProcAddress(FDllHandle, "CGCAPI_CertGetNotAfter");
		if (FfnCGCAPI_CertGetNotAfter == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_CertGetNotAfter\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_CertGetNotBefore = (CGCAPI_CertGetNotBeforePtr)::GetProcAddress(FDllHandle, "CGCAPI_CertGetNotBefore");
		if (FfnCGCAPI_CertGetNotBefore == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_CertGetNotBefore\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_CertGetFXMLExtension = (CGCAPI_CertGetFXMLExtensionPtr)::GetProcAddress(FDllHandle, "CGCAPI_CertGetFXMLExtension");
		if (FfnCGCAPI_CertGetFXMLExtension == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_CertGetFXMLExtension\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_CertGetSubject = (CGCAPI_CertGetSubjectPtr)::GetProcAddress(FDllHandle, "CGCAPI_CertGetSubject");
		if (FfnCGCAPI_CertGetSubject == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_CertGetSubject\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_RegisterCertificate = (CGCAPI_RegisterCertificatePtr)::GetProcAddress(FDllHandle, "CGCAPI_RegisterCertificate");
		if (FfnCGCAPI_RegisterCertificate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_RegisterCertificate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_ComposePKCS7 = (CGCAPI_ComposePKCS7Ptr)::GetProcAddress(FDllHandle, "CGCAPI_ComposePKCS7");
		if (FfnCGCAPI_ComposePKCS7 == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_ComposePKCS7\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_PKCS7GetUserCertificate = (CGCAPI_PKCS7GetUserCertificatePtr)::GetProcAddress(FDllHandle, "CGCAPI_PKCS7GetUserCertificate");
		if (FfnCGCAPI_PKCS7GetUserCertificate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_PKCS7GetUserCertificate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_ShowCertificate = (CGCAPI_ShowCertificatePtr)::GetProcAddress(FDllHandle, "CGCAPI_ShowCertificate");
		if (FfnCGCAPI_ShowCertificate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_ShowCertificate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_PureSign = (CGCAPI_PureSignPtr)::GetProcAddress(FDllHandle, "CGCAPI_PureSign");
		if (FfnCGCAPI_PureSign == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_PureSign\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_PFXGetCertCount = (CGCAPI_PFXGetCertCountPtr)::GetProcAddress(FDllHandle, "CGCAPI_PFXGetCertCount");
		if (FfnCGCAPI_PFXGetCertCount == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_PFXGetCertCount\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPIMemAlloc = (CGCAPIMemAllocPtr)::GetProcAddress(FDllHandle, "CGCAPIMemAlloc");
		if (FfnCGCAPIMemAlloc == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPIMemAlloc\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPIMemRealloc = (CGCAPIMemReallocPtr)::GetProcAddress(FDllHandle, "CGCAPIMemRealloc");
		if (FfnCGCAPIMemRealloc == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPIMemRealloc\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPIMemFree = (CGCAPIMemFreePtr)::GetProcAddress(FDllHandle, "CGCAPIMemFree");
		if (FfnCGCAPIMemFree == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPIMemFree\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_SignAndEncrypt = (CGXCAPI_SignAndEncryptPtr)::GetProcAddress(FDllHandle, "CGXCAPI_SignAndEncrypt");
		if (FfnCGXCAPI_SignAndEncrypt == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_SignAndEncrypt\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_ComposeSignature = (CGXCAPI_ComposeSignaturePtr)::GetProcAddress(FDllHandle, "CGXCAPI_ComposeSignature");
		if (FfnCGXCAPI_ComposeSignature == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_ComposeSignature\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_DecryptAndVerify = (CGXCAPI_DecryptAndVerifyPtr)::GetProcAddress(FDllHandle, "CGXCAPI_DecryptAndVerify");
		if (FfnCGXCAPI_DecryptAndVerify == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_DecryptAndVerify\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_GetUserCertificate = (CGXCAPI_GetUserCertificatePtr)::GetProcAddress(FDllHandle, "CGXCAPI_GetUserCertificate");
		if (FfnCGXCAPI_GetUserCertificate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_GetUserCertificate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_GetCertificate = (CGXCAPI_GetCertificatePtr)::GetProcAddress(FDllHandle, "CGXCAPI_GetCertificate");
		if (FfnCGXCAPI_GetCertificate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_GetCertificate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_GenerateKey = (CGXCAPI_GenerateKeyPtr)::GetProcAddress(FDllHandle, "CGXCAPI_GenerateKey");
		if (FfnCGXCAPI_GenerateKey == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_GenerateKey\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_GetPrivateKey = (CGXCAPI_GetPrivateKeyPtr)::GetProcAddress(FDllHandle, "CGXCAPI_GetPrivateKey");
		if (FfnCGXCAPI_GetPrivateKey == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_GetPrivateKey\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_CertGetPublicKey = (CGXCAPI_CertGetPublicKeyPtr)::GetProcAddress(FDllHandle, "CGXCAPI_CertGetPublicKey");
		if (FfnCGXCAPI_CertGetPublicKey == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_CertGetPublicKey\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_Encrypt = (CGXCAPI_EncryptPtr)::GetProcAddress(FDllHandle, "CGXCAPI_Encrypt");
		if (FfnCGXCAPI_Encrypt == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_Encrypt\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_EncryptInit = (CGXCAPI_EncryptInitPtr)::GetProcAddress(FDllHandle, "CGXCAPI_EncryptInit");
		if (FfnCGXCAPI_EncryptInit == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_EncryptInit\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_EncryptUpdate = (CGXCAPI_EncryptUpdatePtr)::GetProcAddress(FDllHandle, "CGXCAPI_EncryptUpdate");
		if (FfnCGXCAPI_EncryptUpdate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_EncryptUpdate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_EncryptFinal = (CGXCAPI_EncryptFinalPtr)::GetProcAddress(FDllHandle, "CGXCAPI_EncryptFinal");
		if (FfnCGXCAPI_EncryptFinal == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_EncryptFinal\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_Decrypt = (CGXCAPI_DecryptPtr)::GetProcAddress(FDllHandle, "CGXCAPI_Decrypt");
		if (FfnCGXCAPI_Decrypt == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_Decrypt\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_DecryptInit = (CGXCAPI_DecryptInitPtr)::GetProcAddress(FDllHandle, "CGXCAPI_DecryptInit");
		if (FfnCGXCAPI_DecryptInit == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_DecryptInit\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_DecryptUpdate = (CGXCAPI_DecryptUpdatePtr)::GetProcAddress(FDllHandle, "CGXCAPI_DecryptUpdate");
		if (FfnCGXCAPI_DecryptUpdate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_DecryptUpdate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_DecryptFinal = (CGXCAPI_DecryptFinalPtr)::GetProcAddress(FDllHandle, "CGXCAPI_DecryptFinal");
		if (FfnCGXCAPI_DecryptFinal == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_DecryptFinal\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_HashInit = (CGXCAPI_HashInitPtr)::GetProcAddress(FDllHandle, "CGXCAPI_HashInit");
		if (FfnCGXCAPI_HashInit == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_HashInit\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_HashUpdate = (CGXCAPI_HashUpdatePtr)::GetProcAddress(FDllHandle, "CGXCAPI_HashUpdate");
		if (FfnCGXCAPI_HashUpdate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_HashUpdate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_HashFinal = (CGXCAPI_HashFinalPtr)::GetProcAddress(FDllHandle, "CGXCAPI_HashFinal");
		if (FfnCGXCAPI_HashFinal == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_HashFinal\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_CertGetSerialNumber = (CGXCAPI_CertGetSerialNumberPtr)::GetProcAddress(FDllHandle, "CGXCAPI_CertGetSerialNumber");
		if (FfnCGXCAPI_CertGetSerialNumber == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_CertGetSerialNumber\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_CertGetSubject = (CGXCAPI_CertGetSubjectPtr)::GetProcAddress(FDllHandle, "CGXCAPI_CertGetSubject");
		if (FfnCGXCAPI_CertGetSubject == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_CertGetSubject\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_CertGetIssuer = (CGXCAPI_CertGetIssuerPtr)::GetProcAddress(FDllHandle, "CGXCAPI_CertGetIssuer");
		if (FfnCGXCAPI_CertGetIssuer == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_CertGetIssuer\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_CertGetNotBefore = (CGXCAPI_CertGetNotBeforePtr)::GetProcAddress(FDllHandle, "CGXCAPI_CertGetNotBefore");
		if (FfnCGXCAPI_CertGetNotBefore == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_CertGetNotBefore\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_CertGetNotAfter = (CGXCAPI_CertGetNotAfterPtr)::GetProcAddress(FDllHandle, "CGXCAPI_CertGetNotAfter");
		if (FfnCGXCAPI_CertGetNotAfter == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_CertGetNotAfter\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_CertGetDigest = (CGXCAPI_CertGetDigestPtr)::GetProcAddress(FDllHandle, "CGXCAPI_CertGetDigest");
		if (FfnCGXCAPI_CertGetDigest == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_CertGetDigest\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_ExportPKCS12 = (CGXCAPI_ExportPKCS12Ptr)::GetProcAddress(FDllHandle, "CGXCAPI_ExportPKCS12");
		if (FfnCGXCAPI_ExportPKCS12 == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_ExportPKCS12\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_Sign = (CGXCAPI_SignPtr)::GetProcAddress(FDllHandle, "CGXCAPI_Sign");
		if (FfnCGXCAPI_Sign == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_Sign\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_Verify = (CGXCAPI_VerifyPtr)::GetProcAddress(FDllHandle, "CGXCAPI_Verify");
		if (FfnCGXCAPI_Verify == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_Verify\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_PKCS7GetSignerInfoCount = (CGXCAPI_PKCS7GetSignerInfoCountPtr)::GetProcAddress(FDllHandle, "CGXCAPI_PKCS7GetSignerInfoCount");
		if (FfnCGXCAPI_PKCS7GetSignerInfoCount == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_PKCS7GetSignerInfoCount\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_PKCS7GetSignerInfo = (CGXCAPI_PKCS7GetSignerInfoPtr)::GetProcAddress(FDllHandle, "CGXCAPI_PKCS7GetSignerInfo");
		if (FfnCGXCAPI_PKCS7GetSignerInfo == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_PKCS7GetSignerInfo\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_PKCS7GetSignerCertificate = (CGXCAPI_PKCS7GetSignerCertificatePtr)::GetProcAddress(FDllHandle, "CGXCAPI_PKCS7GetSignerCertificate");
		if (FfnCGXCAPI_PKCS7GetSignerCertificate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_PKCS7GetSignerCertificate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_PKCS7GetContent = (CGXCAPI_PKCS7GetContentPtr)::GetProcAddress(FDllHandle, "CGXCAPI_PKCS7GetContent");
		if (FfnCGXCAPI_PKCS7GetContent == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_PKCS7GetContent\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_PKCS7GetUserCertificate = (CGXCAPI_PKCS7GetUserCertificatePtr)::GetProcAddress(FDllHandle, "CGXCAPI_PKCS7GetUserCertificate");
		if (FfnCGXCAPI_PKCS7GetUserCertificate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_PKCS7GetUserCertificate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_PKCS7AddSignerCertificate = (CGXCAPI_PKCS7AddSignerCertificatePtr)::GetProcAddress(FDllHandle, "CGXCAPI_PKCS7AddSignerCertificate");
		if (FfnCGXCAPI_PKCS7AddSignerCertificate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_PKCS7AddSignerCertificate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_PureSign = (CGXCAPI_PureSignPtr)::GetProcAddress(FDllHandle, "CGXCAPI_PureSign");
		if (FfnCGXCAPI_PureSign == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_PureSign\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_PureVerify = (CGXCAPI_PureVerifyPtr)::GetProcAddress(FDllHandle, "CGXCAPI_PureVerify");
		if (FfnCGXCAPI_PureVerify == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_PureVerify\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_Encode = (CGXCAPI_EncodePtr)::GetProcAddress(FDllHandle, "CGXCAPI_Encode");
		if (FfnCGXCAPI_Encode == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_Encode\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_EncodeObject = (CGXCAPI_EncodeObjectPtr)::GetProcAddress(FDllHandle, "CGXCAPI_EncodeObject");
		if (FfnCGXCAPI_EncodeObject == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_EncodeObject\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_Decode = (CGXCAPI_DecodePtr)::GetProcAddress(FDllHandle, "CGXCAPI_Decode");
		if (FfnCGXCAPI_Decode == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_Decode\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_DecodeObject = (CGXCAPI_DecodeObjectPtr)::GetProcAddress(FDllHandle, "CGXCAPI_DecodeObject");
		if (FfnCGXCAPI_DecodeObject == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_DecodeObject\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_LoadStore = (CGXCAPI_LoadStorePtr)::GetProcAddress(FDllHandle, "CGXCAPI_LoadStore");
		if (FfnCGXCAPI_LoadStore == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_LoadStore\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_LoadStoreByName = (CGXCAPI_LoadStoreByNamePtr)::GetProcAddress(FDllHandle, "CGXCAPI_LoadStoreByName");
		if (FfnCGXCAPI_LoadStoreByName == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_LoadStoreByName\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_VerifyCertChain = (CGXCAPI_VerifyCertChainPtr)::GetProcAddress(FDllHandle, "CGXCAPI_VerifyCertChain");
		if (FfnCGXCAPI_VerifyCertChain == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_VerifyCertChain\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_AcceptPKCS7 = (CGXCAPI_AcceptPKCS7Ptr)::GetProcAddress(FDllHandle, "CGXCAPI_AcceptPKCS7");
		if (FfnCGXCAPI_AcceptPKCS7 == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_AcceptPKCS7\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_DeleteUsrCert = (CGXCAPI_DeleteUsrCertPtr)::GetProcAddress(FDllHandle, "CGXCAPI_DeleteUsrCert");
		if (FfnCGXCAPI_DeleteUsrCert == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_DeleteUsrCert\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_ChangeP12Pwd = (CGXCAPI_ChangeP12PwdPtr)::GetProcAddress(FDllHandle, "CGXCAPI_ChangeP12Pwd");
		if (FfnCGXCAPI_ChangeP12Pwd == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_ChangeP12Pwd\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPIFreeHandle = (CGXCAPIFreeHandlePtr)::GetProcAddress(FDllHandle, "CGXCAPIFreeHandle");
		if (FfnCGXCAPIFreeHandle == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPIFreeHandle\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPIFreeBlob = (CGXCAPIFreeBlobPtr)::GetProcAddress(FDllHandle, "CGXCAPIFreeBlob");
		if (FfnCGXCAPIFreeBlob == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPIFreeBlob\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_PFXP7Sign = (CGCAPI_PFXP7SignPtr)::GetProcAddress(FDllHandle, "CGCAPI_PFXP7Sign");
		if (FfnCGCAPI_PFXP7Sign == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_PFXP7Sign\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_CertGetSerialNumber = (CGCAPI_CertGetSerialNumberPtr)::GetProcAddress(FDllHandle, "CGCAPI_CertGetSerialNumber");
		if (FfnCGCAPI_CertGetSerialNumber == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_CertGetSerialNumber\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_CertGetSubjectW = (CGCAPI_CertGetSubjectWPtr)::GetProcAddress(FDllHandle, "CGCAPI_CertGetSubjectW");
		if (FfnCGCAPI_CertGetSubjectW == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_CertGetSubjectW\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGXCAPI_GetCertificates = (CGXCAPI_GetCertificatesPtr)::GetProcAddress(FDllHandle, "CGXCAPI_GetCertificates");
		if (FfnCGXCAPI_GetCertificates == NULL)
			FLogPtr->fprintf(" %s() Load \"CGXCAPI_GetCertificates\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_GetUserCertificate = (CGCAPI_GetUserCertificatePtr)::GetProcAddress(FDllHandle, "CGCAPI_GetUserCertificate");
		if (FfnCGCAPI_GetUserCertificate == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_GetUserCertificate\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnCGCAPI_GetUserCertificates = (CGCAPI_GetUserCertificatesPtr)::GetProcAddress(FDllHandle, "CGCAPI_GetUserCertificates");
		if (FfnCGCAPI_GetUserCertificates == NULL)
			FLogPtr->fprintf(" %s() Load \"CGCAPI_GetUserCertificates\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FfnGetDLLVersion = (GetDLLVersionPtr)::GetProcAddress(FDllHandle, "GetDLLVersion");
		if (FfnGetDLLVersion == NULL)
			FLogPtr->fprintf(" %s() Load \"GetDLLVersion\" from Library[%s] falied.", __func__, FDLLFileName.c_str());
		else
			loadFunctionCount++;

		FLogPtr->fprintf(" %s() Load %d functions from Library[%s].", __func__, loadFunctionCount, FDLLFileName.c_str());
	}
	else
		FLogPtr->fprintf(" %s() Load Dll[%s] falied.", __func__, FDLLFileName.c_str());
    FLogPtr->FlushToFile();
#endif  //#ifdef WIN32

	return isSuccess;
}  //CApiCADllObject::LoadCADynamicLibrary()

//#pragma package(smart_init)
