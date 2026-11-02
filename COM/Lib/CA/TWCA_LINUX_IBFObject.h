/* 
 * File:   TWCA_LINUX_IBFObject.h
 * Author: Zhen Fan
 *
 * Created on 2026/10/13 15:49
 */

#ifndef TWCA_LINUX_IBFOBJECT_H
#define TWCA_LINUX_IBFOBJECT_H
#include "TWCA_LinuxSign.h"

#include "UFC.h"
#include "ApiCADllObject.h"

typedef TWCAHandle (*TWCA_New_Ptr) (void);
typedef void (*TWCA_Free_Ptr) (TWCAHandle);
typedef int (*TWCA_LoadProperty_Ptr) (TWCAHandle, const char*);
typedef int (*TWCA_SelectCert_Ptr) (TWCAHandle, const char*, const char*);
typedef int (*TWCA_SignPkcs7_Ptr) (TWCAHandle, const unsigned char*, int, const char*, const char*);
typedef const char* (*TWCA_GetSignedPkcs7_Ptr) (TWCAHandle);
typedef const char* (*TWCA_GetSignerCertB64_Ptr) (TWCAHandle);
typedef const char* (*TWCA_GetSignerCertSubject_Ptr) (TWCAHandle);
typedef const char* (*TWCA_GetSignerCertSubjectCN_Ptr) (TWCAHandle);
typedef const char* (*TWCA_GetSignerCertIssuer_Ptr) (TWCAHandle);
typedef const char* (*TWCA_GetSignerCertNotBefore_Ptr) (TWCAHandle);
typedef const char* (*TWCA_GetSignerCertNotAfter_Ptr) (TWCAHandle);
typedef const char* (*TWCA_GetSignerCertSerial_Ptr) (TWCAHandle);
typedef const char* (*TWCA_GetSignerCertFinger_Ptr) (TWCAHandle);
typedef int         (*TWCA_GetErrorCode_Ptr) (TWCAHandle);
typedef const char* (*TWCA_GetErrorMsg_Ptr) (TWCAHandle);
typedef const char* (*TWCA_Version_Ptr) (void);

//---------------------------------------------------------------------------
class CIBFTWCAObject : public CApiCADllObject
{
protected:
    TWCA_New_Ptr                    FfnTWCA_New;
    TWCA_Free_Ptr                   FfnTWCA_Free;
    TWCA_LoadProperty_Ptr           FfnTWCA_LoadProperty;
    TWCA_SelectCert_Ptr             FfnTWCA_SelectCert;
    TWCA_SignPkcs7_Ptr              FfnTWCA_SignPkcs7;
    TWCA_GetSignedPkcs7_Ptr         FfnTWCA_GetSignedPkcs7;
    TWCA_GetSignerCertB64_Ptr       FfnTWCA_GetSignerCertB64;
    TWCA_GetSignerCertSubject_Ptr   FfnTWCA_GetSignerCertSubject;
    TWCA_GetSignerCertSubjectCN_Ptr FfnTWCA_GetSignerCertSubjectCN;
    TWCA_GetSignerCertIssuer_Ptr    FfnTWCA_GetSignerCertIssuer;
    TWCA_GetSignerCertNotBefore_Ptr FfnTWCA_GetSignerCertNotBefore;
    TWCA_GetSignerCertNotAfter_Ptr  FfnTWCA_GetSignerCertNotAfter;
    TWCA_GetSignerCertSerial_Ptr    FfnTWCA_GetSignerCertSerial;
    TWCA_GetSignerCertFinger_Ptr    FfnTWCA_GetSignerCertFinger;
    TWCA_GetErrorCode_Ptr           FfnTWCA_GetErrorCode;
    TWCA_GetErrorMsg_Ptr            FfnTWCA_GetErrorMsg;
    TWCA_Version_Ptr                FfnTWCA_Version;
    
    void* FDynamicLibPtr;
    UFC::AnsiString FPFXFilePathName;
    UFC::AnsiString FPFXPassword;
    
    TWCAHandle FTWCAHandle;

    UFC::AnsiString FSubjectCN;  //主旨 CN
    UFC::AnsiString FSubject;    //主旨 FC2253
    UFC::AnsiString FIssuer;     //發行者 RFC2253
    UFC::AnsiString FNotBefore;  //生效時間 YYYY/MM/DD HH:MM:SS UTC
    UFC::AnsiString FNotAfter;   //到期時間 YYYY/MM/DD HH:MM:SS UTC
    UFC::AnsiString FSerial;     //序號 大寫十六進位
    UFC::AnsiString FFinger;     //SHA-1 指紋 十六進位
    
    UFC::AnsiString FEncoding;   //目標編碼 UTF-8, UTF-16LE(Default)
    UFC::AnsiString FDigest;     //摘要演算法，僅支援 SHA-1 與 SHA-256.  SHA1(Default),SHA256,SHA2
    
public:
    CIBFTWCAObject(const UFC::AnsiString& PFXFilePathName, const UFC::AnsiString& PFXPassword, UFC::BufferedLog* LogPtr);
    CIBFTWCAObject(const UFC::AnsiString& DynamicLibFilePathName, const UFC::AnsiString& PFXFilePathName, const UFC::AnsiString& PFXPassword, UFC::BufferedLog* LogPtr);
    ~CIBFTWCAObject();
    
    bool LoadCADynamicLibrary();
    
    int GenerateSignatureAndCAData(const UFC::AnsiString& PlainText, CAResultData& ResultData);
    int GenerateLogonSignatureAndCAData(const UFC::AnsiString& LogonID, CAResultData& ResultData);
    int GenerateReqMsgSignatureAndCAData(TBaseMessage* ReqPtr, const UFC::AnsiString& ReqTelegram, CAResultData& ResultData);
    
    void DumpSignature(const UFC::AnsiString& Signature);
};  //CIBFTWCAObject
#endif /* TWCA_LINUX_IBFOBJECT_H */

