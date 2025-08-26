//---------------------------------------------------------------------------

#ifndef UniFSCCAObjectH
#define UniFSCCAObjectH
//---------------------------------------------------------------------------
#ifdef WIN32
#include "IFSCATLAPI.h"
#endif  //#ifdef WIN32

#include "ApiCADllObject.h"

//--------------------------------------------------------------------------
class CUniFSCCAObject : public CApiCADllObject
{
protected:
#ifdef WIN32
	IFSCATLAPI* FFSCObjPtr;
#endif  //#ifdef WIN32

public:
	CUniFSCCAObject(const UFC::AnsiString& CommonName, UFC::BufferedLog* LogPtr);

	virtual bool CreateActiveXCAObject();

	int GenerateSignatureAndCAData(const UFC::AnsiString& PlainText, CAResultData& ResultData);
	int GenerateLogonSignatureAndCAData(const UFC::AnsiString& LogonID, CAResultData& ResultData);
	int GenerateReqMsgSignatureAndCAData(TBaseMessage* ReqPtr, const UFC::AnsiString& ReqTelegram, CAResultData& ResultData);
};  //CUniFSCCAObject
#endif
