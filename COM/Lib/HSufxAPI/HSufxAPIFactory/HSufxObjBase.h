//---------------------------------------------------------------------------
#ifndef HSufxObjBaseH
#define HSufxObjBaseH
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
#include "t2sdk_interface.h"
#include "HSufxBCBInit.h"
//---------------------------------------------------------------------------
#include "../../OrderMessage/TNewOrderMessage.h"
#include "../../OrderMessage/TCancelOrderMessage.h"
#include "../OrderMessage/TExecutionReportMessage.h"
#include "../../MarketDataMessage/TNewsMessage.h"
//---------------------------------------------------------------------------
extern HSufxApiInit gHSufxAPI;
//---------------------------------------------------------------------------
class TTaifexConnection;
//---------------------------------------------------------------------------
// class HSufxObjBase
//---------------------------------------------------------------------------
class HSufxObjBase
{
public:
	virtual void HSufxInitial( void ) = 0;
	virtual void HSufxRelease( void ) = 0;
	virtual BOOL HSufxConnect( const char* strT2Servers, int TimeoutSec ) = 0;
	virtual void HSufxSendLogin( void ) = 0;
	virtual void HSufxSendLogout( void ) = 0;
	virtual void HSufxNewOrder( TNewOrderMessage* Msg, const UFC::AnsiString& UserDefine ) = 0;
	virtual void HSufxCancelOrder( TCancelOrderMessage* Msg, const UFC::AnsiString& UserDefine ) = 0;
	virtual int  HandleHSufxMessage( TNewsMessage* Msg ) = 0;
public:
	virtual ~HSufxObjBase() {}
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

