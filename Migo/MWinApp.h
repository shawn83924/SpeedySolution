#ifndef _MWinApp_h
#define _MWinApp_h
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "MApp.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define WM_ON_CONNECT    (WM_USER + 300)
#define WM_ON_DISCONNECT (WM_USER + 301)
#define WM_ON_ERROR      (WM_USER + 302)
#define WM_ON_MESSAGE    (WM_USER + 303)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MWinApp : public MApp
{
private:
    HWND            FHwnd;
	HINSTANCE       FInstance;
	DWORD           FThreadID;
private:
	BOOL CreateMessageWindow( void );
    BOOL InitMessageWindow( void );
	char MessageWindowsClassName[ 128 ];
private:
    static LRESULT CALLBACK MessageWndProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam );
    static void FireOnMessage( MWinApp* App,DataMessage* MsgHeader );
    static void FireOnConnected( MWinApp* App );
    static void FireOnError( MWinApp* App,PMAppError Error );
    static void FireOnDisconnected( MWinApp* App );
private:
    virtual void TriggerOnMessage( DataMessage* MsgHeader );
    virtual void TriggerOnConnected( void );
    virtual void TriggerOnError( PMAppError Error );
	virtual void TriggerOnDisconnected( void );
public:
    MWinApp( HINSTANCE Instance ,UFCType::Int32 Port = MAPP_DEF_PORT, UFCType::Int32 QueueSize = MSG_QUEUE_SIZE );
	virtual  ~MWinApp( void );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

