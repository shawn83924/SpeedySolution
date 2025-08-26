//---------------------------------------------------------------------------

#ifndef MLTWCAObjectH
#define MLTWCAObjectH
#ifdef WIN32
#include "ITWCAAPI.h"
#endif  //#ifdef WIN32

#include "ApiCADllObject.h"

//--------------------------------------------------------------------------
class CMLTWCAObject : public CApiCADllObject
{
protected:
#ifdef WIN32
	ITWCAAPI* FTWCAObjPtr;
#endif  //#ifdef WIN32

public:
	CMLTWCAObject(const UFC::AnsiString& CommonName, UFC::BufferedLog* LogPtr);

	virtual bool CreateActiveXCAObject();

	int GenerateSignatureAndCAData(const UFC::AnsiString& PlainText, CAResultData& ResultData);
	int GenerateLogonSignatureAndCAData(const UFC::AnsiString& LogonID, CAResultData& ResultData);
	int GenerateReqMsgSignatureAndCAData(TBaseMessage* ReqPtr, const UFC::AnsiString& ReqTelegram, CAResultData& ResultData);
};  //CMLTWCAObject
#endif#endif
