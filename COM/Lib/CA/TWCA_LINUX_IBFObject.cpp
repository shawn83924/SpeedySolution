/* 
 * File:   TWCA_LINUX_IBFObject.cpp
 * Author: Zhen Fan
 *
 * Created on 2026/10/13 15:49
 */
#include <dlfcn.h>

#include "TWCA_LINUX_IBFObject.h"

//------------------------------- CIBFTWCAObject -------------------------------
//------------------------------------------------------------------------------
CIBFTWCAObject::CIBFTWCAObject(const UFC::AnsiString& PFXFilePathName, const UFC::AnsiString& PFXPassword, UFC::BufferedLog* LogPtr)
:CApiCADllObject(caTWCA, LogPtr)
,FfnTWCA_New(0)
,FfnTWCA_Free(0)
,FfnTWCA_LoadProperty(0)
,FfnTWCA_SelectCert(0)
,FfnTWCA_SignPkcs7(0)
,FfnTWCA_GetSignedPkcs7(0)
,FfnTWCA_GetSignerCertB64(0)
,FfnTWCA_GetSignerCertSubject(0)
,FfnTWCA_GetSignerCertSubjectCN(0)
,FfnTWCA_GetSignerCertIssuer(0)
,FfnTWCA_GetSignerCertNotBefore(0)
,FfnTWCA_GetSignerCertNotAfter(0)
,FfnTWCA_GetSignerCertSerial(0)
,FfnTWCA_GetSignerCertFinger(0)
,FfnTWCA_GetErrorCode(0)
,FfnTWCA_GetErrorMsg(0)
,FfnTWCA_Version(0)
,FPFXFilePathName(PFXFilePathName)
,FPFXPassword(PFXPassword)
,FTWCAHandle(0)
,FSubjectCN("")
,FSubject("")
,FIssuer("")
,FNotBefore("")
,FNotAfter("")
,FSerial("")
,FFinger("")
,FEncoding("UTF-16LE")
,FDigest("SHA-256")
{
    FIsWorking = false;
/*
    FTWCAHandle = TWCA_New();
    if (FTWCAHandle != 0)
    {
        FLogPtr->fprintf(" %s() New TWCA Object Success.", __func__);
        if (TWCA_SelectCert(FTWCAHandle, FPFXFilePathName.c_str(), FPFXPassword.c_str()) == TWCA_OK)
        {
            FIsWorking = true;
            FLogPtr->fprintf(" %s() Call TWCA_SelectCert(\"%s\", \"%s\") Success.", __func__, FPFXFilePathName.c_str(), FPFXPassword.c_str());
            
            FSubjectCN = UFC::AnsiString(TWCA_GetSignerCertSubjectCN(FTWCAHandle));
            FLogPtr->fprintf(" %s() SubjectCN[%s].", __func__, FSubjectCN.c_str());
            
            FSubject = UFC::AnsiString(TWCA_GetSignerCertSubject(FTWCAHandle));
            FLogPtr->fprintf(" %s() Subject[%s].", __func__, FSubject.c_str());
            
            FIssuer = UFC::AnsiString(TWCA_GetSignerCertIssuer(FTWCAHandle));
            FLogPtr->fprintf(" %s() Issuer[%s].", __func__, FIssuer.c_str());
            
            FNotBefore = UFC::AnsiString(TWCA_GetSignerCertNotBefore(FTWCAHandle));
            FLogPtr->fprintf(" %s() NotBefore[%s].", __func__, FNotBefore.c_str());
            
            FNotAfter = UFC::AnsiString(TWCA_GetSignerCertNotAfter(FTWCAHandle));
            FLogPtr->fprintf(" %s() NotAfter[%s].", __func__, FNotAfter.c_str());
            
            FSerial = UFC::AnsiString(TWCA_GetSignerCertSerial(FTWCAHandle));
            FLogPtr->fprintf(" %s() Serial[%s].", __func__, FSerial.c_str());
            
            FFinger = UFC::AnsiString(TWCA_GetSignerCertFinger(FTWCAHandle));
            FLogPtr->fprintf(" %s() Finger[%s].", __func__, FFinger.c_str());
            
            UFC::AnsiString plainText = "This is a book.";
            FLogPtr->fprintf(" %s() PlainText[%s].", __func__, plainText.c_str());
            FLogPtr->fprintf(" %s() Encoding[%s].", __func__, FEncoding.c_str());
            FLogPtr->fprintf(" %s() Digest[%s].", __func__, FDigest.c_str());
            
            if (TWCA_SignPkcs7(FTWCAHandle, (const unsigned char*)plainText.c_str(), plainText.Length(), FEncoding.c_str(), FDigest.c_str()) == TWCA_OK) 
            {
                FLogPtr->fprintf(" %s() Call TWCA_SignPkcs7 success.", __func__);
                
                UFC::AnsiString signedPkcs7(TWCA_GetSignedPkcs7(FTWCAHandle));
                DumpSignature(signedPkcs7);
//                for (int i = 0; i < signedPkcs7.Length(); i++)
//                {
//                    FLogPtr->fprintf("%5d:'%c'.", i + 1, signedPkcs7[i]);
//                }
                FLogPtr->fprintf(" %s() PKCS#7(b64) Length=%d.", __func__, signedPkcs7.Length());
            }
            else
            {
                FLogPtr->fprintf(" %s() Call TWCA_SignPkcs7 failed:%d[%s]", __func__, TWCA_GetErrorCode(FTWCAHandle), TWCA_GetErrorMsg(FTWCAHandle));
            }
        }
        else
        {
            FLogPtr->fprintf(" %s() Call TWCA_SelectCert(\"%s\", \"%s\") failed:%d[%s]", __func__, FPFXFilePathName.c_str(), FPFXPassword.c_str(), TWCA_GetErrorCode(FTWCAHandle), TWCA_GetErrorMsg(FTWCAHandle));
            TWCA_Free(FTWCAHandle);
        }
    }
    else
        FLogPtr->fprintf(" %s() New TWCA Object failed.", __func__);
*/
    FLogPtr->FlushToFile();
}  //CIBFTWCAObject::CIBFTWCAObject()
//------------------------------------------------------------------------------
CIBFTWCAObject::CIBFTWCAObject(const UFC::AnsiString& DynamicLibFilePathName, const UFC::AnsiString& PFXFilePathName, const UFC::AnsiString& PFXPassword, UFC::BufferedLog* LogPtr)
:CApiCADllObject(caTWCA, LogPtr)
,FfnTWCA_New(0)
,FfnTWCA_Free(0)
,FfnTWCA_LoadProperty(0)
,FfnTWCA_SelectCert(0)
,FfnTWCA_SignPkcs7(0)
,FfnTWCA_GetSignedPkcs7(0)
,FfnTWCA_GetSignerCertB64(0)
,FfnTWCA_GetSignerCertSubject(0)
,FfnTWCA_GetSignerCertSubjectCN(0)
,FfnTWCA_GetSignerCertIssuer(0)
,FfnTWCA_GetSignerCertNotBefore(0)
,FfnTWCA_GetSignerCertNotAfter(0)
,FfnTWCA_GetSignerCertSerial(0)
,FfnTWCA_GetSignerCertFinger(0)
,FfnTWCA_GetErrorCode(0)
,FfnTWCA_GetErrorMsg(0)
,FfnTWCA_Version(0)
,FDynamicLibPtr(0)
,FPFXFilePathName(PFXFilePathName)
,FPFXPassword(PFXPassword)
,FTWCAHandle(0)
,FSubjectCN("")
,FSubject("")
,FIssuer("")
,FNotBefore("")
,FNotAfter("")
,FSerial("")
,FFinger("")
,FEncoding("UTF-16LE")
,FDigest("SHA-256")
{
    FDLLFileName = DynamicLibFilePathName;
    FIsWorking = LoadCADynamicLibrary();
    if (FIsWorking)
    {
        FTWCAHandle = FfnTWCA_New();
        if (FTWCAHandle != 0)
        {
            UFC::AnsiString caDllVersion(FfnTWCA_Version());
            FLogPtr->fprintf(" %s() New TWCA Object Success. Version[%s]", __func__, caDllVersion.c_str());

            if (FfnTWCA_SelectCert(FTWCAHandle, FPFXFilePathName.c_str(), FPFXPassword.c_str()) == TWCA_OK)
            {
                FLogPtr->fprintf(" %s() Call TWCA_SelectCert(\"%s\", \"%s\") Success.", __func__, FPFXFilePathName.c_str(), FPFXPassword.c_str());
                
                FSubjectCN = UFC::AnsiString(FfnTWCA_GetSignerCertSubjectCN(FTWCAHandle));
                FLogPtr->fprintf(" %s() SubjectCN[%s].", __func__, FSubjectCN.c_str());
            
                FSubject = UFC::AnsiString(FfnTWCA_GetSignerCertSubject(FTWCAHandle));
                FLogPtr->fprintf(" %s() Subject[%s].", __func__, FSubject.c_str());
            
                FIssuer = UFC::AnsiString(FfnTWCA_GetSignerCertIssuer(FTWCAHandle));
                FLogPtr->fprintf(" %s() Issuer[%s].", __func__, FIssuer.c_str());
            
                FNotBefore = UFC::AnsiString(FfnTWCA_GetSignerCertNotBefore(FTWCAHandle));
                FLogPtr->fprintf(" %s() NotBefore[%s].", __func__, FNotBefore.c_str());
            
                FNotAfter = UFC::AnsiString(FfnTWCA_GetSignerCertNotAfter(FTWCAHandle));
                FLogPtr->fprintf(" %s() NotAfter[%s].", __func__, FNotAfter.c_str());
            
                FSerial = UFC::AnsiString(FfnTWCA_GetSignerCertSerial(FTWCAHandle));
                FLogPtr->fprintf(" %s() Serial[%s].", __func__, FSerial.c_str());
            
                FFinger = UFC::AnsiString(FfnTWCA_GetSignerCertFinger(FTWCAHandle));
                FLogPtr->fprintf(" %s() Finger[%s].", __func__, FFinger.c_str());
                
                UFC::AnsiString plainText = "This is a book.";
                FLogPtr->fprintf(" %s() PlainText[%s].", __func__, plainText.c_str());
                FLogPtr->fprintf(" %s() Encoding[%s].", __func__, FEncoding.c_str());
                FLogPtr->fprintf(" %s() Digest[%s].", __func__, FDigest.c_str());
            
//                CAResultData caResultObj;
//                caResultObj.SetPlainText(plainText);
//                int caResultCode = GenerateSignatureAndCAData(plainText, caResultObj);
//                if (caResultCode == TWCA_OK)
//                {
//                    FLogPtr->fprintf(" %s() Call TWCA_SignPkcs7 success.", __func__);
//                    DumpSignature(caResultObj.GetSignature());
//                    FLogPtr->fprintf(" %s() Subject[%s]", __func__, caResultObj.GetSubject().c_str());
//                    FLogPtr->fprintf(" %s() SerialNumber[%s]", __func__, caResultObj.GetSerialNumber().c_str());
//                    FLogPtr->fprintf(" %s() NotBefore[%s]", __func__, caResultObj.GetNotBefore().c_str());
//                    FLogPtr->fprintf(" %s() NotAfter[%s]", __func__, caResultObj.GetNotAfter().c_str());
//                }
//                else
//                    FLogPtr->fprintf(" %s() Call TWCA_SignPkcs7 failed: %d[%s].", __func__, caResultObj.GetErrorCode(), caResultObj.GetResultMsg().c_str());
                        
//                if (FfnTWCA_SignPkcs7(FTWCAHandle, (const unsigned char*)plainText.c_str(), plainText.Length(), FEncoding.c_str(), FDigest.c_str()) == TWCA_OK) 
//                {
//                    FLogPtr->fprintf(" %s() Call TWCA_SignPkcs7 success.", __func__);
//                    UFC::AnsiString signedPkcs7(FfnTWCA_GetSignedPkcs7(FTWCAHandle));
//                    DumpSignature(signedPkcs7);
//                    FLogPtr->fprintf(" %s() PKCS#7(b64) Length=%d.", __func__, signedPkcs7.Length());
//                }
//                else
//                {
//                    FLogPtr->fprintf(" %s() Call TWCA_SignPkcs7 failed:%d[%s]", __func__, FfnTWCA_GetErrorCode(FTWCAHandle), FfnTWCA_GetErrorMsg(FTWCAHandle));
//                }
            }
            else
                FLogPtr->fprintf(" %s() Call TWCA_SelectCert(\"%s\", \"%s\") failed:%d[%s]", __func__, FPFXFilePathName.c_str(), FPFXPassword.c_str(), FfnTWCA_GetErrorCode(FTWCAHandle), FfnTWCA_GetErrorMsg(FTWCAHandle));
        }
        else
            FLogPtr->fprintf(" %s() New TWCA Object failed.", __func__);
    }
    FLogPtr->FlushToFile();
}  //CIBFTWCAObject::CIBFTWCAObject()
//------------------------------------------------------------------------------
CIBFTWCAObject::~CIBFTWCAObject()
{
    if (FIsWorking && (FTWCAHandle != 0)) FfnTWCA_Free(FTWCAHandle);
}  //CIBFTWCAObject::~CIBFTWCAObject()
//---------------------------------------------------------------------------
bool CIBFTWCAObject::LoadCADynamicLibrary()
{
    UFC::AnsiString logPrefix = "";
    logPrefix.Printf("CIBFTWCAObject::%s()", __func__);
    
    bool isLoadSuccess = false;
    if (FDLLFileName.Length() <= 0) return isLoadSuccess;
    if (UFC::FileExists(FDLLFileName) == FALSE)
    {
        FLogPtr->fprintf(" %s DynamicLibrary[%s] not exists.", logPrefix.c_str());
        return isLoadSuccess;
    }
    
    FDynamicLibPtr = dlopen(FDLLFileName.c_str(), RTLD_LAZY);
    if (FDynamicLibPtr != 0)
    {
        FLogPtr->fprintf(" %s Load DynamicLibrary[%s].", logPrefix.c_str(), FDLLFileName.c_str());
        isLoadSuccess = true;
    }
    else
    {
        FLogPtr->fprintf(" %s Load DynamicLibrary[%s] failed!", logPrefix.c_str(), FDLLFileName.c_str());    
        FLogPtr->fprintf(" %s Reason[%s]", logPrefix.c_str(), dlerror());    
        return isLoadSuccess;
    }
    
    UFC::AnsiString funcName = "TWCA_New";
    FfnTWCA_New = (TWCA_New_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_New == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_Free";
    FfnTWCA_Free = (TWCA_Free_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_Free == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());

    funcName = "TWCA_LoadProperty";
    FfnTWCA_LoadProperty = (TWCA_LoadProperty_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_LoadProperty == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());

    funcName = "TWCA_SelectCert";
    FfnTWCA_SelectCert = (TWCA_SelectCert_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_SelectCert == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());

    funcName = "TWCA_SignPkcs7";
    FfnTWCA_SignPkcs7 = (TWCA_SignPkcs7_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_SignPkcs7 == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_GetSignedPkcs7";
    FfnTWCA_GetSignedPkcs7 = (TWCA_GetSignedPkcs7_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_GetSignedPkcs7 == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_GetSignerCertB64";
    FfnTWCA_GetSignerCertB64 = (TWCA_GetSignerCertB64_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_GetSignerCertB64 == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_GetSignerCertSubject";
    FfnTWCA_GetSignerCertSubject = (TWCA_GetSignerCertSubject_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_GetSignerCertSubject == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_GetSignerCertSubjectCN";
    FfnTWCA_GetSignerCertSubjectCN = (TWCA_GetSignerCertSubjectCN_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_GetSignerCertSubjectCN == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_GetSignerCertIssuer";
    FfnTWCA_GetSignerCertIssuer = (TWCA_GetSignerCertIssuer_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_GetSignerCertIssuer == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_GetSignerCertNotBefore";
    FfnTWCA_GetSignerCertNotBefore = (TWCA_GetSignerCertNotBefore_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_GetSignerCertNotBefore == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_GetSignerCertNotAfter";
    FfnTWCA_GetSignerCertNotAfter = (TWCA_GetSignerCertNotAfter_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_GetSignerCertNotAfter == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_GetSignerCertSerial";
    FfnTWCA_GetSignerCertSerial = (TWCA_GetSignerCertSerial_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_GetSignerCertSerial == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_GetSignerCertFinger";
    FfnTWCA_GetSignerCertFinger = (TWCA_GetSignerCertFinger_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_GetSignerCertFinger == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_GetErrorCode";
    FfnTWCA_GetErrorCode = (TWCA_GetErrorCode_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_GetErrorCode == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_GetErrorMsg";
    FfnTWCA_GetErrorMsg = (TWCA_GetErrorMsg_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_GetErrorMsg == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    funcName = "TWCA_Version";
    FfnTWCA_Version = (TWCA_Version_Ptr)dlsym(FDynamicLibPtr, funcName.c_str());
    if (FfnTWCA_Version == 0)
        FLogPtr->fprintf(" %s Load Function[%s] from DynamicLibrary[%s] failed!", logPrefix.c_str(), funcName.c_str(), FDLLFileName.c_str());
    
    FLogPtr->FlushToFile();
    return isLoadSuccess;
}  //CIBFTWCAObject::LoadCADynamicLibrary()
//---------------------------------------------------------------------------
int CIBFTWCAObject::GenerateSignatureAndCAData(const UFC::AnsiString& PlainText, CAResultData& ResultData)
{
    int caResult = -1;
    int plainTextLen = PlainText.Length();
    int twcaErrorCode = caResult;
    UFC::AnsiString twcaErrorMsg = "";
    if (plainTextLen > 0)
    {
        caResult = FfnTWCA_SignPkcs7(FTWCAHandle, (const unsigned char*)PlainText.c_str(), PlainText.Length(), FEncoding.c_str(), FDigest.c_str());
        twcaErrorCode = caResult;
        if (caResult == TWCA_OK) 
        {
            UFC::AnsiString signedPkcs7(FfnTWCA_GetSignedPkcs7(FTWCAHandle));
            ResultData.SetSubject(FSubject);
            ResultData.SetSerialNumber(FSerial);
            ResultData.SetNotBefore(FNotBefore);
            ResultData.SetNotAfter(FNotAfter);
            ResultData.SetSignature(signedPkcs7);
        }
        else
        {
            twcaErrorCode = FfnTWCA_GetErrorCode(FTWCAHandle);
            twcaErrorMsg = UFC::AnsiString(FfnTWCA_GetErrorMsg(FTWCAHandle));
            FLogPtr->fprintf(" %s() Call TWCA_SignPkcs7 failed:%d[%s]", __func__, twcaErrorCode, twcaErrorMsg.c_str());
        }
    }
    else
    {
        twcaErrorMsg.Printf("Plain Text is NULL.");
    }
    ResultData.SetErrorCode(twcaErrorCode);
    ResultData.SetResultMsg(twcaErrorMsg);
    
    return caResult;
}  //CIBFTWCAObject::GenerateSignatureAndCAData()
//------------------------------------------------------------------------------
int CIBFTWCAObject::GenerateLogonSignatureAndCAData(const UFC::AnsiString& LogonID, CAResultData& ResultData)
{
    ResultData.SetSubject(FSubject);
    ResultData.SetSerialNumber(FSerial);
    ResultData.SetNotBefore(FNotBefore);
    ResultData.SetNotBefore(FNotAfter);
    return 0;
}  //CIBFTWCAObject::GenerateLogonSignatureAndCAData()
//------------------------------------------------------------------------------
int CIBFTWCAObject::GenerateReqMsgSignatureAndCAData(TBaseMessage* ReqPtr, const UFC::AnsiString& ReqTelegram, CAResultData& ResultData)
{
    UFC::AnsiString plainText = ReqTelegram;
    ResultData.SetPlainText(plainText);
    return GenerateSignatureAndCAData(plainText, ResultData);
}  //CIBFTWCAObject::GenerateReqMsgSignatureAndCAData()
//------------------------------------------------------------------------------
void CIBFTWCAObject::DumpSignature(const UFC::AnsiString& Signature)
{
    int signatureLen = Signature.Length();
    int characterOfRow = 64;
    int rowCount = signatureLen / characterOfRow;
    int characterOfLastRow = signatureLen % characterOfRow;
    for (int curRowIndex = 0; curRowIndex < rowCount; curRowIndex++)
    {
        int beginPos = curRowIndex * characterOfRow;
        FLogPtr->fprintf(" %4d %s", curRowIndex + 1, Signature.SubString(beginPos, characterOfRow).c_str());
    }
    
    if (characterOfLastRow > 0)
    {
        int beginPos = rowCount * characterOfRow;
        FLogPtr->fprintf(" %4d %s", rowCount + 1, Signature.SubString(beginPos, characterOfLastRow).c_str());
    }
    
    FLogPtr->fprintf(" Signature Length %d", signatureLen);
    FLogPtr->FlushToFile();
}  //CIBFTWCAObject::DumpSignature()
