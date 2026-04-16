//---------------------------------------------------------------------------

#ifndef SECCAPIH
#define SECCAPIH
//---------------------------------------------------------------------------
#include "MEGASECCAPIATLLib_OCX.h"
#include "FMTConfig.h"
#include "BrokerType.h"
//---------------------------------------------------------------------------
class TSECCAPI
{
private:
	BrokerType FBrokerType;
	String FBrokerID;
	TMEGASECCAPI* FMEGACAObject;
public:
	String GetCASerial(void);
	UTF8String GetSigned(	wchar_t* bFilepath,
							wchar_t* bPassword,
							wchar_t* bData,
							wchar_t* bSubject,
							int iFlags,
							int iHashFlag,
							int iKeyUsage);
	long GetErrorCode(void);
    TSECCAPI(BrokerType BType, TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

