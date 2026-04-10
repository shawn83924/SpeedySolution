//---------------------------------------------------------------------------

#ifndef CACheckerH
#define CACheckerH
//---------------------------------------------------------------------------
#include "UFC.h"
#include "MEGASECCAPIATLLib_OCX.h"
#include <XMLDoc.hpp>
#include <OleCtrls.hpp>
#include "OrderStore.h"
#include "FMTConfig.h"
#include "BrokerType.h"
#include "SECCAPI.h"
//---------------------------------------------------------------------------
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <IdSSL.hpp>
#include <IdSSLOpenSSL.hpp>
//---------------------------------------------------------------------------
enum ComType
{
	ctFutures = 0,
	ctOptions = 1,
	ctInternational = 2,
	ctTWSE = 3,
	ctOTC = 4
};
//---------------------------------------------------------------------------
class CheckData
{
private:
	ComType FType;
	String FContent;
public:
	CheckData( ComType Type, const String& Content ): FType( Type ), FContent( Content ){}
public:
	String GetContent( void ) { return FContent; }
	ComType GetType( void ) { return FType; }
};
//---------------------------------------------------------------------------
class TCAChecker;
//---------------------------------------------------------------------------
typedef void __fastcall (__closure *TOnCACheckFail)(TCAChecker* Sender, UnicodeString& Reason );
//---------------------------------------------------------------------------
using namespace Megaseccapiatllib_tlb;
//---------------------------------------------------------------------------
class TCAChecker: public UFC::PThread, public ICACheckListener
{
private:
	UFC::PtrQueue<CheckData> FQueue;
	UFC::PCriticalSection FSignCS;
	TSECCAPI* FCAObject;
	BrokerType FBrokerType;
	TIdHTTP*      FHTTP;
	TIdSSLIOHandlerSocketOpenSSL* FSSLIOHandler;
	TComponent* FOwner;
	bool FInitOK;
	bool FSync;
	bool FCheckResult;
	bool FIsRunning;
	String FLastError;
	String FURL;
    String FErrMsg;
	TOnCACheckFail FOnCACheckFail;
public:
	String FID;
	String FPFXFile;
	String FPFXFilePassword;
	String FSignSubject;
	String FCASerial;
	String FLocalIP;
	AnsiString FAnsiLocalIP;
public:
	TCAChecker(TComponent* Owner);
	~TCAChecker( void );
public:
	bool InitialCheckerAndTest( String& Reason /* out */ );

	void Work( CheckData* Data );
	bool Check( ComType Type, const String& Content, String& Reason );
    bool SignAgrement( const String& Content, String& Reason );
	virtual void OnCACheck( nsOrderMessageDefine::MarketEnum Market,
							   SendMessageType SType,
							   TBaseMessage* Msg,
							   bool& CanSend,
							   UFC::AnsiString& RejectMsg );
	void SetBrokerType(TBrokerConfig* BrokerConfig);
private:
	void EscapeDataString( UTF8String& EscStr );
	TSECCAPI* CreateNewFCAObject( BrokerType BType, TComponent* Owner);
	virtual void Execute( void );
private:
	void EmptyQueueAndTriggerError( String& Reason );
	bool HandleResultDoc( TMemoryStream* Result, bool IsTest, String& Reason );
	int  Sign( const String& Content, String& Sign );
	bool PostMEGA( const String& Content, const String& sign, const String& BizCode, String& Reason, bool IsTest = false );
    bool PostCapital();
	void ClearAndLog( void );
    const String& ToErrorMessage( int code );
public:
	__property bool CheckResult   = { read = FCheckResult };
	__property String CASerial    = { read = FCASerial };
	__property String ID = { read = FID, write = FID };
	__property String URL = { read = FURL, write = FURL };
	__property String PFXFileName = { read = FPFXFile, write = FPFXFile };
	__property String PFXPassword = { read = FPFXFilePassword, write = FPFXFilePassword };
	__property TOnCACheckFail OnCACheckFail = { read = FOnCACheckFail, write = FOnCACheckFail };

};
//---------------------------------------------------------------------------
#endif
