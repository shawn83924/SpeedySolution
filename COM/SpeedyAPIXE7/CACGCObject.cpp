//---------------------------------------------------------------------------

#pragma hdrstop
#include <cstdio>
#include <cstring>

#include "System.hpp"

#include "CACGCObject.h"

//------------------------------ CCACGCObject -------------------------------
//---------------------------------------------------------------------------
CCACGCObject::CCACGCObject(const UFC::AnsiString& DLLFileName)
:FDLLFileName(DLLFileName)
,FDllInstance(NULL)
,FfnCGCAPI_PFXP7Sign(NULL)
,FfnCGCAPI_PKCS7GetUserCertificate(NULL)
,FfnCGCAPI_CertGetSubject(NULL)
,FfnCGCAPI_CertGetNotBefore(NULL)
,FfnCGCAPI_CertGetNotAfter(NULL)
,FfnCGCAPI_CertGetSerialNumber(NULL)
,FfnCGXCAPI_LoadStore(NULL)
,FfnCGXCAPI_GetCertificates(NULL)
,FfnCGCAPI_SignEx(NULL)
,FfnCGCAPIMemFree(NULL)
,FfnCGXCAPIFreeHandle(NULL)
,FIsWork(false)
,FErrorMessage("")
,FCASubject("")
,FCANotBefore("")
,FCANotAfter("")
,FCASerialNumber("")
{
	FIsWork = LoadCGCDLLLibrary();
	if (!FIsWork)
	{
		UFC::BufferedLog::Printf(" CCACGCObject() %s.", FErrorMessage.c_str());
		UFC::BufferedLog::FlushToFile();
	}
}  //CCACGCObject::CCACGCObject()
//---------------------------------------------------------------------------
CCACGCObject::~CCACGCObject()
{
	if (FDllInstance == NULL) FreeLibrary(FDllInstance);
}  //CCACGCObject::~CCACGCObject()
//---------------------------------------------------------------------------
bool CCACGCObject::LoadCGCDLLLibrary()
{
	if (FDLLFileName.Length() <= 0)
	{
		FErrorMessage.Printf("Missing dynamic library Name.");
		return false;
	}

	String wDLLFileName = String(FDLLFileName.c_str());
	FDllInstance = ::LoadLibraryW(wDLLFileName.w_str());
	if (FDllInstance == NULL)
	{
		FErrorMessage.Printf("Load dynamic library \"%s\" Failed.", FDLLFileName.c_str());
		return false;
	}

	FfnCGCAPI_PFXP7Sign = (CGCAPI_PFXP7SignPtr)::GetProcAddress(FDllInstance, "CGCAPI_PFXP7Sign");
	if (FfnCGCAPI_PFXP7Sign == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGCAPI_PFXP7Sign\" Failed.");
		return false;
	}

	FfnCGCAPI_PKCS7GetUserCertificate = (CGCAPI_PKCS7GetUserCertificatePtr)::GetProcAddress(FDllInstance, "CGCAPI_PKCS7GetUserCertificate");
	if (FfnCGCAPI_PKCS7GetUserCertificate == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGCAPI_PKCS7GetUserCertificate\" Failed.");
		return false;
	}

	FfnCGCAPI_PFXP7Sign = (CGCAPI_PFXP7SignPtr)::GetProcAddress(FDllInstance, "CGCAPI_PFXP7Sign");
	if (FfnCGCAPI_PFXP7Sign == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGCAPI_PFXP7Sign\" Failed.");
		return false;
	}

	FfnCGCAPI_PKCS7GetUserCertificate = (CGCAPI_PKCS7GetUserCertificatePtr)::GetProcAddress(FDllInstance, "CGCAPI_PKCS7GetUserCertificate");
	if (FfnCGCAPI_PKCS7GetUserCertificate == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGCAPI_PKCS7GetUserCertificate\" Failed.");
		return false;
	}

	FfnCGCAPI_CertGetSubject = (CGCAPI_CertGetSubjectPtr)::GetProcAddress(FDllInstance, "CGCAPI_CertGetSubject");
	if (FfnCGCAPI_CertGetSubject == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGCAPI_CertGetSubject\" Failed.");
		return false;
	}

	FfnCGCAPI_CertGetNotBefore = (CGCAPI_CertGetNotBeforePtr)::GetProcAddress(FDllInstance, "CGCAPI_CertGetNotBefore");
	if (FfnCGCAPI_CertGetNotBefore == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGCAPI_CertGetNotBefore\" Failed.");
		return false;
	}

	FfnCGCAPI_CertGetNotAfter = (CGCAPI_CertGetNotAfterPtr)::GetProcAddress(FDllInstance, "CGCAPI_CertGetNotAfter");
	if (FfnCGCAPI_CertGetNotAfter == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGCAPI_CertGetNotAfter\" Failed.");
		return false;
	}

	FfnCGCAPI_CertGetSerialNumber = (CGCAPI_CertGetSerialNumberPtr)::GetProcAddress(FDllInstance, "CGCAPI_CertGetSerialNumber");
	if (FfnCGCAPI_CertGetSerialNumber == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGCAPI_CertGetSerialNumber\" Failed.");
		return false;
	}

	FfnCGXCAPI_LoadStore = (CGXCAPI_LoadStorePtr)::GetProcAddress(FDllInstance, "CGXCAPI_LoadStore");
	if (FfnCGXCAPI_LoadStore == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGXCAPI_LoadStore\" Failed.");
		return false;
	}

	FfnCGXCAPI_GetCertificates = (CGXCAPI_GetCertificatesPtr)::GetProcAddress(FDllInstance, "CGXCAPI_GetCertificates");
	if (FfnCGXCAPI_GetCertificates == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGXCAPI_GetCertificates\" Failed.");
		return false;
	}

	FfnCGCAPI_SignEx = (CGCAPI_SignExPtr)::GetProcAddress(FDllInstance, "CGCAPI_SignEx");
	if (FfnCGCAPI_SignEx == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGCAPI_SignEx\" Failed.");
		return false;
	}

	FfnCGCAPIMemFree = (CGCAPIMemFreePtr)::GetProcAddress(FDllInstance, "CGCAPIMemFree");
	if (FfnCGCAPIMemFree == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGCAPIMemFree\" Failed.");
		return false;
	}

	FfnCGXCAPIFreeHandle = (CGXCAPIFreeHandlePtr)::GetProcAddress(FDllInstance, "CGXCAPIFreeHandle");
	if (FfnCGXCAPIFreeHandle == NULL)
	{
		FErrorMessage.Printf("Load dynamic library Function \"CGXCAPIFreeHandle\" Failed.");
		return false;
	}

	return true;
}  //CCACGCObject::LoadCGCDLLLibrary()
//---------------------------------------------------------------------------
UFC::AnsiString CCACGCObject::DescribeReturnCode(int ReturnCode)
{
	UFC::AnsiString returnMsg = "";
	switch (ReturnCode)
	{
		case CG_RTN_SUCCESS:                    returnMsg = "success";                                                 break;  //    0
		case CG_RTN_ERROR:                      returnMsg = "general error";                                           break;  // 5001
		case CG_RTN_MEMALLOC_ERROR:             returnMsg = "Memory Allocation Error";                                 break;  // 5002
		case CG_RTN_BUFFER_TOO_SMALL:           returnMsg = "Buffer too small";                                        break;  // 5003
		case CG_RTN_FUNCTION_UNSUPPORT:         returnMsg = "function not support";                                    break;  // 5004
		case CG_RTN_INVALID_PARAM:              returnMsg = "Invalid parameter";                                       break;  // 5005
		case CG_RTN_INVALID_HANDLE:             returnMsg = "Invalid handle";                                          break;  // 5006
		case CG_RTN_LIB_EXPIRE:                 returnMsg = "TrialVersion Library is expired";                         break;  // 5007
		case CG_RTN_BASE64_ERROR:               returnMsg = "Base64 Encoding/Decoding Error";                          break;  // 5008
		case CG_RTN_CERT_NOT_FOUND:             returnMsg = "Certificate not found in MS CryptoAPI Database";          break;  // 5010
		case CG_RTN_CERT_EXPIRED:               returnMsg = "Certificate Expire";                                        break;  // 5011
		case CG_RTN_CERT_NOT_YET_VALID:         returnMsg = "Certificate can not be used now";                         break;  // 5012
		case CG_RTN_CERT_EXPIRE_OR_NOT_YET_USE: returnMsg = "Some certificates are expired, some can not be used now"; break;  // 5013
		case CG_RTN_CERT_DENIED:                returnMsg = "Certificate subject not match";                           break;  // 5014
		case CG_RTN_CERT_NOISSUER:              returnMsg = "Unable to find certificate issuer";                       break;  // 5015
		case CG_RTN_CERT_BAD_SIGNATURE:         returnMsg = "Certificate signature is invalid";                        break;  // 5016
		case CG_RTN_CERT_INVALID_KEYUSAGE:      returnMsg = "Invalid certificate keyusage";                            break;  // 5017
		case CG_RTN_CERT_REVOKED:               returnMsg = "Certificate is revoked";                                  break;  // 5020
//		case CG_RTN_CERT_UNSPECIFIED:           returnMsg = "Certificate is revoked"; break;  // 5020
		case CG_RTN_CERT_KEY_COMPROMISED:       returnMsg = "Certificate is revoked(key compromised)";                 break;  // 5021
		case CG_RTN_CERT_CA_COMPROMISED:        returnMsg = "Certificate is revoked(CA compromised)";                  break;  // 5022
		case CG_RTN_CERT_AFFILIATION_CHANGED:   returnMsg = "Certificate is revoked(affiliation changed)";             break;  // 5023
		case CG_RTN_CERT_SUPERSEDED:            returnMsg = "Certificate is revoked(superseded)";                      break;  // 5024
		case CG_RTN_CERT_CESSATION:             returnMsg = "Certificate is revoked(cessation)";                       break;  // 5025
		case CG_RTN_CERT_HOLD:                  returnMsg = "Certificate is revoked(hold)";                            break;  // 5026
		case CG_RTN_CERT_REMOVEFROMCRL:         returnMsg = "Certificate is revoked(hold)";                            break;  // 5028
		case CG_RTN_CRL_EXPIRED:                returnMsg = "CRL expired";                                             break;  // 5030
		case CG_RTN_CRL_NOT_YET_VALID:          returnMsg = "CRL not yet valid";                                       break;  // 5031
		case CG_RTN_CRL_NOT_FOUND:              returnMsg = "CRL not found";                                           break;  // 5032
		case CG_RTN_CRL_BAD_SIGNATURE:          returnMsg = "CRL signature invalid";                                   break;  // 5034
		case CG_RTN_GET_DIGEST_ERROR:           returnMsg = "Get Digest error";                                        break;  // 5035
		case CG_RTN_BAD_SIGNATURE:              returnMsg = "Invalid data signature";                                  break;  // 5036
		case CG_RTN_BAD_CONTENT:                returnMsg = "Content not match";                                       break;  // 5037
		case CG_RTN_BAD_CAPTCHA:                returnMsg = "Bad CAPTCHA";                                             break;  // 5038
		case CG_RTN_INVALID_CERT:               returnMsg = "Incorrect Certificate format";                            break;  // 5040
		case CG_RTN_INVALID_CRL:                returnMsg = "Incorrect CRL format";                                    break;  // 5041
		case CG_RTN_INVALID_PKCS7:              returnMsg = "Incorrect PKCS7 format";                                  break;  // 5042
		case CG_RTN_INVALID_KEY:                returnMsg = "Incorrect KEY format";                                    break;  // 5043
		case CG_RTN_INVALID_CERTREQ:            returnMsg = "Incorrect PKCS10 format";                                 break;  // 5044
		case CG_RTN_INVALID_FORMAT:             returnMsg = "Incorrect format";                                        break;  // 5045
		case CG_RTN_INVALID_PKCS12:             returnMsg = "Invalid PKCS12";                                          break;  // 5046
		case CG_RTN_OBJ_NOT_FOUND:              returnMsg = "Object not found";                                        break;  // 5050
		case CG_RTN_PKCS7_NO_CONTENT:           returnMsg = "PKCS7 no Content";                                        break;  // 5051
		case CG_RTN_PKCS7_NO_CERTIFICATE:       returnMsg = "PKCS7 no Cerftificate";                                   break;  // 5052
		case CG_RTN_PKCS7_NO_SIGNERINFO:        returnMsg = "PKCS7 no SIGNERINFO";                                     break;  // 5053
		case CG_RTN_UNMATCH_CERT_KEY:           returnMsg = "Certificate/PrivateKey not match";                        break;  // 5060
		case CG_RTN_SIGN_ERROR:                 returnMsg = "Sign error";                                              break;  // 5061
		case CG_RTN_VERIFY_ERROR:               returnMsg = "Verify error";                                            break;  // 5062
		case CG_RTN_ENCRYPT_ERROR:              returnMsg = "Encrypt error";                                           break;  // 5063
		case CG_RTN_DECRYPT_ERROR:              returnMsg = "Decrypt error";                                           break;  // 5064
		case CG_RTN_GENKEY_ERROR:               returnMsg = "Generate key error";                                      break;  // 5065
		case CG_RTN_DELETE_USR_CERT_ERROR:      returnMsg = "Delete user Certificate Error";                           break;  // 5066
		case CG_RTN_BAD_ALGID:                  returnMsg = "Bad ALGID";                                               break;  // 5067
		case CG_RTN_OPERATION_CANCELED:         returnMsg = "Operation Cancel";                                        break;  // 5070
		case CG_RTN_PASSWD_INVALID:             returnMsg = "Password Invalid";                                        break;  // 5071
		case CG_RTN_SCARD_BLOCKED:              returnMsg = "SCARD Blocked";                                           break;  // 5072
		case CG_RTN_REPLUG_TIMEOUT:             returnMsg = "REPLUG Timeout";                                          break;  // 5073
		case CG_RTN_DEVICE_DISABLED:            returnMsg = "Device Disabled";                                         break;  // 5074
		case CG_RTN_XMLPARSE_ERROR:             returnMsg = "XML Parse Error";                                         break;  // 5080
		case CG_RTN_XMLTAG_NOTFOUND:            returnMsg = "XML Tag not found";                                       break;  // 5081
		default:                                returnMsg = "Unknown Return Code";
	}
	return returnMsg;
}  //CCACGCObject::DescribeReturnCode()
//---------------------------------------------------------------------------
int CCACGCObject::GenerateSignature(const UFC::AnsiString& DataStr, UFC::AnsiString& SignatureStr)
{
	if (!FIsWork) return -1;

	char* signatureBuffer = 0;
	int result = GenerateSignatureBuffer(DataStr.c_str(), DataStr.Length(), &signatureBuffer);
	if (result == CG_RTN_SUCCESS)
	{
		SignatureStr = UFC::AnsiString(signatureBuffer);
		FfnCGCAPIMemFree(signatureBuffer);
	}
	else
		SignatureStr = "";
	return result;
}  //CCACGCObject::GenerateSignature()
//---------------------------------------------------------------------------
int CCACGCObject::GetCertificateBufferFromSignature(char* SignatureBuffer, char** CertificateBuffer)
{
	if (!FIsWork) return -1;
	*CertificateBuffer = 0;
	int result = FfnCGCAPI_PKCS7GetUserCertificate(SignatureBuffer, CertificateBuffer);
	if (result != CG_RTN_SUCCESS)
		FErrorMessage.Printf("Get Certificate Buffer From Signature failed. %d[%s]", result, DescribeReturnCode(result).c_str());
	return result;
}  //CCACGCObject::GetCertificateBufferFromSignature()
//---------------------------------------------------------------------------
int CCACGCObject::GetCertificateFromSignature(const UFC::AnsiString& SignatureStr, UFC::AnsiString& Certificate)
{
	char *certificateBuffer = 0;
	int result = FfnCGCAPI_PKCS7GetUserCertificate((char*)SignatureStr.c_str(), &certificateBuffer);
	if (result == CG_RTN_SUCCESS)
	{
		Certificate = UFC::AnsiString(certificateBuffer);
		FfnCGCAPIMemFree(certificateBuffer);
	}
	return result;
}  //CCACGCObject::GetCertificateFromSignature()
//---------------------------------------------------------------------------
int CCACGCObject::GenerateSignatureAndCAData(const UFC::AnsiString& DataStr, UFC::AnsiString& SignatureStr)
{
	SignatureStr = "";
	if (!FIsWork) return -1;

	FErrorMessage = "";
	FCASubject = "";
	FCANotBefore = "";
	FCANotAfter = "";
	FCASerialNumber = "";
	char *signatureBuffer = 0;
	int result = GenerateSignatureBuffer(DataStr.c_str(), DataStr.Length(), &signatureBuffer);
	if (result == CG_RTN_SUCCESS)
	{
		SignatureStr = UFC::AnsiString(signatureBuffer);
		char *certificateBuffer = 0;
		int certificateResult = FfnCGCAPI_PKCS7GetUserCertificate(signatureBuffer, &certificateBuffer);
		if (result == CG_RTN_SUCCESS)
		{
			UFC::PStringBuffer messageBuffer;
			char *subjectBuffer = 0;
			int subjectResult = FfnCGCAPI_CertGetSubject(certificateBuffer, 0, &subjectBuffer);
			if (subjectResult == CG_RTN_SUCCESS)
			{
				FCASubject = UFC::AnsiString(subjectBuffer);
				FfnCGCAPIMemFree(subjectBuffer);
			}
			else
				messageBuffer.AppendPrintf(" Subject(%d)", subjectResult);

			char* notBeforeBuffer = new char[32];
			std::memset(notBeforeBuffer, 0, 32);
			int notBeforeResult = FfnCGCAPI_CertGetNotBefore(certificateBuffer, 0, notBeforeBuffer);
			if (notBeforeResult == CG_RTN_SUCCESS)
				FCANotBefore = UFC::AnsiString(notBeforeBuffer);
			else
				messageBuffer.AppendPrintf(" NotBefore(%d)", notBeforeResult);
			delete [] notBeforeBuffer;

			char* notAfterBuffer = new char[32];
			std::memset(notAfterBuffer, 0, 32);
			int notAfterResult = FfnCGCAPI_CertGetNotAfter(certificateBuffer, 0, notAfterBuffer);
			if (notAfterResult == CG_RTN_SUCCESS)
				FCANotAfter = UFC::AnsiString(notAfterBuffer);
			else
				messageBuffer.AppendPrintf(" NotAfter(%d)", notAfterResult);
			delete [] notAfterBuffer;

			char *serialNumberBuffer = 0;
			int serialNumberResult = FfnCGCAPI_CertGetSerialNumber(certificateBuffer, 0, &serialNumberBuffer);
			if (serialNumberResult == CG_RTN_SUCCESS)
			{
				FCASerialNumber = UFC::AnsiString(serialNumberBuffer);
				FfnCGCAPIMemFree(serialNumberBuffer);
			}
			else
				messageBuffer.AppendPrintf(" SerialNumber(%d)", serialNumberResult);
			FfnCGCAPIMemFree(certificateBuffer);
			if (messageBuffer.Length() > 0) FErrorMessage.Printf("Get%s failed", messageBuffer.c_str());
		}
		else
			FErrorMessage.Printf("Get User Certificate failed. %d[%s]", certificateResult, DescribeReturnCode(certificateResult).c_str());

		FfnCGCAPIMemFree(signatureBuffer);
	}  //if (result == CG_RTN_SUCCESS)
	return result;
}  //CCACGCObject::GenerateSignatureAndCAData()

//---------------------------- CCACGCFileObject -----------------------------
//---------------------------------------------------------------------------
CCACGCFileObject::CCACGCFileObject(const UFC::AnsiString& DLLFileName, const UFC::AnsiString& PFXFilePathName, const UFC::AnsiString& PFXPassword)
:CCACGCObject(DLLFileName)
,FPFXFilePathName(PFXFilePathName)
,FPFXPassword(PFXPassword)
,FPFXBuffer(0)
,FPFXDataSize(0)
{
	if (FIsWork) FIsWork = LoadPFXFile();
	if (!FIsWork)
	{
		UFC::BufferedLog::Printf(" CCACGCFileObject() %s.", FErrorMessage.c_str());
		UFC::BufferedLog::FlushToFile();
	}
}  //CCACGCFileObject::CCACGCFileObject()
//---------------------------------------------------------------------------
CCACGCFileObject::~CCACGCFileObject()
{
	if (FPFXBuffer != 0) delete [] FPFXBuffer;
}  //CCACGCFileObject::~CCACGCFileObject()
//---------------------------------------------------------------------------
bool CCACGCFileObject::LoadPFXFile()
{
	if (FPFXFilePathName.Length() <= 0)
	{
		FErrorMessage.Printf("Missing PFX File Name.");
		return false;
	}

	std::FILE* filePtr = std::fopen(FPFXFilePathName.c_str(), "rb");
	if (filePtr == NULL)
	{
		FErrorMessage.Printf("Open PFX File \"%s\" Failed.", FPFXFilePathName.c_str());
		return false;
	}

	std::fseek(filePtr, 0, SEEK_END);
	FPFXDataSize = std::ftell(filePtr);
	std::rewind(filePtr);
	int bufferSize = FPFXDataSize + 1;
	FPFXBuffer = new unsigned char[bufferSize];
	std::memset(FPFXBuffer, 0, bufferSize);
	std::fread(FPFXBuffer, 1, FPFXDataSize, filePtr);
	std::fclose(filePtr);
	return true;
}  //CCACGCFileObject::LoadPFXFile()
//---------------------------------------------------------------------------
int CCACGCFileObject::GenerateSignatureBuffer(const char* DataBuffer, int DataLength, char** SignatureBuffer)
{
	if (!FIsWork) return -1;
	*SignatureBuffer = 0;
	int result = FfnCGCAPI_PFXP7Sign(FPFXBuffer, FPFXDataSize, FPFXPassword.c_str(), DataBuffer, DataLength, 0, CG_ALGOR_SHA1, 0, SignatureBuffer);
	if (result != CG_RTN_SUCCESS)
		FErrorMessage.Printf("Generate Signature Buffer failed. %d[%s]", result, DescribeReturnCode(result).c_str());
	return result;
}  //CCACGCFileObject::GenerateSignatureBuffer()

//--------------------------- CCACGCBrowserObject ---------------------------
//---------------------------------------------------------------------------
CCACGCBrowserObject::CCACGCBrowserObject(const UFC::AnsiString& DLLFileName, const UFC::AnsiString& OrganizationalUnit, const UFC::AnsiString& CommonName)
:CCACGCObject(DLLFileName)
,FOrganizationalUnit(OrganizationalUnit)
,FCommonName(CommonName)
,FCAStoreHandle(0)
,FCACertsHandlePtr(0)
,FCACertsNumber(0)
,FSubjectFilterBuffer(0)
{
	if (FIsWork)
	{
		FIsWork = false;
		if (FOrganizationalUnit.Length() > 0)
		{
			if (FCommonName.Length() > 0)
			{
				int result = FfnCGXCAPI_LoadStore(&FCAStoreHandle, CGCAPI_STORE_CU_MY | CGCAPI_STORE_CU_OTHER, 0);
				if (result == CG_RTN_SUCCESS)
				{
					FSubjectFilterBuffer = new char[128];
					std::memset(FSubjectFilterBuffer, 0, 128);
					std::sprintf(FSubjectFilterBuffer, "OU=%s\nCN=TW%s1", FOrganizationalUnit.c_str(), FCommonName.c_str());
					result = FfnCGXCAPI_GetCertificates(FCAStoreHandle, FSubjectFilterBuffer, NULL, CGCAPI_FLAG_SELCERT_AUTO, CG_KU_KEY_ENCIPHERMENT, &FCACertsHandlePtr, &FCACertsNumber);
					if (result == CG_RTN_SUCCESS)
						FIsWork = true;
					else
						FErrorMessage.Printf("Get Certificates[%s] failed. %d[%s]", FSubjectFilterBuffer, result, DescribeReturnCode(result).c_str());
				}
				else
					FErrorMessage.Printf("Load Store failed. %d[%s]", result, DescribeReturnCode(result).c_str());
			}
			else
				FErrorMessage.Printf("Missing Common Name");
		}
		else
			FErrorMessage.Printf("Missing Organizational Unit");
	}  //if (FIsWork)

	if (!FIsWork)
	{
		UFC::BufferedLog::Printf(" CCACGCBrowserObject() %s.", FErrorMessage.c_str());
		UFC::BufferedLog::FlushToFile();
	}
}  //CCACGCBrowserObject::CCACGCBrowserObject()
//---------------------------------------------------------------------------
CCACGCBrowserObject::~CCACGCBrowserObject()
{
	if (FSubjectFilterBuffer != 0) delete [] FSubjectFilterBuffer;
	if (FCAStoreHandle != 0) FfnCGXCAPIFreeHandle(FCAStoreHandle);
	if (FCACertsHandlePtr != 0) FfnCGXCAPIFreeHandle(*FCACertsHandlePtr);
}  //CCACGCBrowserObject::~CCACGCBrowserObject()
//---------------------------------------------------------------------------
int CCACGCBrowserObject::GenerateSignatureBuffer(const char* DataBuffer, int DataLength, char** SignatureBuffer)
{
	if (!FIsWork) return -1;
	*SignatureBuffer = 0;
	int result = FfnCGCAPI_SignEx(DataBuffer, DataLength, FSubjectFilterBuffer, "", "", "", CGCAPI_FLAG_SELCERT_AUTO, CG_ALGOR_SHA1, 0, SignatureBuffer);
	if (result != CG_RTN_SUCCESS)
		FErrorMessage.Printf("Generate Signature Buffer failed. %d[%s]", result, DescribeReturnCode(result).c_str());
	return result;
}  //CCACGCBrowserObject::GenerateSignatureBuffer()
#pragma package(smart_init)
