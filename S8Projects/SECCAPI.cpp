//---------------------------------------------------------------------------
#pragma hdrstop
#include "SECCAPI.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
TSECCAPI::TSECCAPI(BrokerType BType, TComponent* Owner)
{
	FBrokerType = BType;
	if(FBrokerType == MEGA)// Mega Futures
	{
		FMEGACAObject = new TMEGASECCAPI( Owner );
	}
	if(FBrokerType == Capital)// Capital Futures
	{
		// TODO: create capital object in future
	}
}
String TSECCAPI::GetCASerial()
{
	if(FBrokerType == MEGA)
		return FMEGACAObject->CGCAPIStockGetSN();

	if(FBrokerType == Capital)// TODO: return capital CA serial
		return "1234";

	return "";
}

UTF8String TSECCAPI::GetSigned(	wchar_t* bFilepath,
								wchar_t* bPassword,
								wchar_t* bData,
								wchar_t* bSubject,
								int iFlags,
								int iHashFlag,
								int iKeyUsage)
{
	if(FBrokerType == MEGA)
		return FMEGACAObject->CGCAPIStockPFXPureSign( 	bFilepath,
														bPassword,
														bData,
														bSubject,
														iFlags,
														iHashFlag,
														iKeyUsage);

	if(FBrokerType == Capital)// TODO: return capital sign
		return L"Pass";

    return L"";
}

long TSECCAPI::GetErrorCode()
{
	if(FBrokerType == MEGA)
		return FMEGACAObject->GetErrorCode();

	if(FBrokerType == Capital)// TODO: return capital error code
		return 0;

	return 0;
}
