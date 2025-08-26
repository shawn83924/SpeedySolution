//---------------------------------------------------------------------------

#ifndef CABasicObjectsH
#define CABasicObjectsH
#include "UFC.h"

//---------------------------------------------------------------------------
class ICAFunctions
{
public:
	virtual ~ICAFunctions() {};

	virtual UFC::AnsiString GetErrorMessage() = 0;
	virtual bool IsWork() = 0;
	virtual int GenerateSignatureBuffer(const char* DataBuffer, int DataLength, char** SignatureBuffer) = 0;
	virtual int GenerateSignature(const UFC::AnsiString& DataStr, UFC::AnsiString& SignatureStr) = 0;
	virtual int GetCertificateBufferFromSignature(char* SignatureBuffer, char** CertificateBuffer) = 0;
	virtual int GetCertificateFromSignature(const UFC::AnsiString& SignatureStr, UFC::AnsiString& Certificate) = 0;
	virtual int GenerateSignatureAndCAData(const UFC::AnsiString& DataStr, UFC::AnsiString& SignatureStr) = 0;
	virtual UFC::AnsiString GetCASubject() = 0;
	virtual UFC::AnsiString GetCANotBefore() = 0;
	virtual UFC::AnsiString GetCANotAfter() = 0;
	virtual UFC::AnsiString GetCASerialNumber() = 0;
};  //ICAFunctions
#endif
