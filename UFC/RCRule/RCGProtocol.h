//------------------------------------------------------------------------------------------------------------------------
#ifndef __RCGProtocol_H
#define __RCGProtocol_H
//------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFCType.h"
#include "../UFC/NetUtility.h"
#include "../UFC/PClientSocket.h"
//------------------------------------------------------------------------------------------------------------------------
using namespace UFC;
//------------------------------------------------------------------------------------------------------------------------
// Define configure constant
//------------------------------------------------------------------------------------------------------------------------
#define RCGP_HEARTBEAT_PERIOD_CLIENT	30		// seconds
#define RCGP_HEARTBEAT_PERIOD_SERVER	40		// seconds
#define RCGP_HEARTBEAT_TIMEOUT_CLIENT	10		// seconds
#define RCGP_HEARTBEAT_TIMEOUT_SERVER	5		// seconds
//------------------------------------------------------------------------------------------------------------------------
#define RCGP_HEADER_SIZE			8
#define RCGP_BODY_BUFFER_SIZE		1024
//------------------------------------------------------------------------------------------------------------------------
#define RCGP_ACT_LOGON				100		// S <-- C
#define RCGP_ACT_LOGON_CONFIRM		101		// S --> C
/////////////////////////////////////////////////////////////////
#define RCGP_ACT_DATA_CHECK			300		// S <-- C
#define RCGP_ACT_DATA_CHECK_RESULT	301		// S --> C
#define RCGP_ACT_DATA_REPORT		309		// S <-- C
#define RCGP_ACT_DATA_REPORT_REPLY	310		// S --> C
/////////////////////////////////////////////////////////////////
#define RCGP_ACT_HEARTBEAT			800		// S --> C  ,  S <-- C
#define RCGP_ACT_HEARTBEAT_CONFIRM	801		// S <-- C  ,  S --> C
//------------------------------------------------------------------------------------------------------------------------
#define RCRB_ACT_RECOVER_REQ		400		// S <-- C
#define RCRB_ACT_RECOVER_RESULT		401		// S --> C
#define RCRB_ACT_RECOVER_END		402		// S --> C
#define RCRB_ACT_RECOVER_ERR		403		// S --> C
//------------------------------------------------------------------------------------------------------------------------
// class RCGPacket
//------------------------------------------------------------------------------------------------------------------------
class RCGPacket
{
private:
	Int32				FAction;
	UFC::AnsiString		FBody;
public:
	Int32	GetAction()					{ return FAction; }
	void	SetAction( Int32 Action )	{ FAction  = Action; }
	UFC::AnsiString* GetBody()			{ return &FBody; }
public:
	void	LoadFromSocket( UFC::PClientSocket* Socket );
	void	SerializeToSocket( UFC::PClientSocket* Socket );
public:
	static	RCGPacket* CreateFromStream( UFC::PClientSocket* Socket ); /* Caller need delete it self */
public:
	RCGPacket( );
	RCGPacket( Int32 Action );
	RCGPacket( Int32 Action, const UFC::AnsiString& Body );
	RCGPacket( Int32 Action, const char* Body );
	virtual ~RCGPacket();
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
