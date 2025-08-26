#include "MWinApp.h"
#include "MSubscriber.h"
#include "MigoHeader.h"
#include "../UFC/UFC.h"
#include <tchar.h>
//------------------------------------------------------------------------------
//
// Implement Public methods.
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------
MWinApp::MWinApp( HINSTANCE Instance,UFCType::Int32 Port, UFCType::Int32 QueueSize )
:MApp( Port, QueueSize )
,FInstance( Instance )
,FThreadID( GetCurrentThreadId() )
{
	if( InitMessageWindow() == TRUE )
	{
		if( CreateMessageWindow()== FALSE )
		{
			UFC::BufferedLog::DebugPrintf( " Can't create the message window." );
			throw UFC::Exception( " Can't create the message window." );
		}
		else
			UFC::BufferedLog::DebugPrintf( " Create message window:%p Thread:%d", (void*)FHwnd, FThreadID );
	}
	else
	{
		UFC::BufferedLog::DebugPrintf( " Register window class failed" );
		throw UFC::Exception( " Register window class failed." );
	}
}
//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
MWinApp::~MWinApp( void )
{
	DestroyWindow( FHwnd );
	UnregisterClassA( MessageWindowsClassName, FInstance );
}
//------------------------------------------------------------------------------
void MWinApp::TriggerOnMessage( DataMessage* MsgHeader )
{
	if( MsgHeader != NULL )
	{
		// ------------- Ways to send windows Message ----------------------------------
		//		DWORD Result;
		//
		///< With timeout.
		//	  SendMessageTimeout( FHwnd, WM_ON_MESSAGE, (UINT)this, (UINT) MsgHeader, SMTO_BLOCK|SMTO_NOTIMEOUTIFNOTHUNG, 5000, &Result );
		//
		///< noqueued message.
		//	 SendMessage( FHwnd, WM_ON_MESSAGE, (UINT)this, (UINT) MsgHeader );
		//
		///< queued message.
		//  PostMessage( FHwnd, WM_ON_MESSAGE, (WPARAM)this, (LPARAM) MsgHeader );

		UFCType::Int32 ErrorCount = 0;
		UFC::AnsiString ErrorMessage;

		if( MsgHeader->getSubject() != "NEWS.RESPONSE" )
		{
			while( !PostMessage( FHwnd, WM_ON_MESSAGE, (WPARAM)this, (LPARAM) MsgHeader ) )
			{
				DWORD retSize;
				LPTSTR pTemp = NULL;

				retSize = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
						NULL, GetLastError(), LANG_NEUTRAL, (LPTSTR)&pTemp, 0, NULL );
				if( retSize && pTemp != NULL)
				{
					pTemp[_tcslen(pTemp)-2] = '\0'; //remove cr and newline character
					ErrorMessage.Printf( " Error:[%d][%s]", GetLastError(), pTemp );
					LocalFree( (HLOCAL)pTemp );
				}
				ErrorCount++;

				if( ErrorCount % 100 == 0 )
				{
					UFC::BufferedLog::DebugPrintf( " PostMessage MessageID:[%d] Subject:[%s] Key:[%s] %s. Final result is failed, retry count:[%d]",
							MsgHeader->GetMessageID(), MsgHeader->getSubject().c_str(), MsgHeader->getKey().c_str(), ErrorMessage.c_str(), ErrorCount );
					return;
				}
				UFC::SleepMS( 10 );
			}
			if( ErrorCount > 0 )
				UFC::BufferedLog::DebugPrintf( " PostMessage MessageID:[%d] Subject:[%s] Key:[%s] %s. Final result is successful, retry count:[%d]",
					MsgHeader->GetMessageID(), MsgHeader->getSubject().c_str(), MsgHeader->getKey().c_str(), ErrorMessage.c_str(), ErrorCount );
		}
		else
			FireOnMessage( this, MsgHeader );
	}
}
//------------------------------------------------------------------------------
void MWinApp::TriggerOnConnected( void )
{
	::PostMessageA( FHwnd, WM_ON_CONNECT, (WPARAM)this,(LPARAM)0 );
}
//------------------------------------------------------------------------------
void MWinApp::TriggerOnError( PMAppError Error )
{
	::PostMessageA( FHwnd, WM_ON_ERROR, (WPARAM)this, (LPARAM)Error );
}
//------------------------------------------------------------------------------
void MWinApp::TriggerOnDisconnected( void )
{
	::PostMessageA( FHwnd, WM_ON_DISCONNECT, (WPARAM)this, (LPARAM)0 );
}
//------------------------------------------------------------------------------
void MWinApp::FireOnMessage( MWinApp* App,DataMessage* MsgHeader )
{
	MSubscriber*     Subscriber;
	PSubscriberList* Subscribers;
	MessageListener* EventListener;
	const MString&   Subject = MsgHeader->getSubject();
	const MString&   Key     = MsgHeader->getKey();
	MTree            TreeData;

	if( MsgHeader != NULL )
	{
		MsgHeader->getData( TreeData );
		if( MsgHeader->GetSenderID() != 0 )
			TreeData.append( "_IP", MsgHeader->GetSenderID() );

		UFC::PLockObject CSObj( App->FSubscriberListCS );

		if( (Subscribers = App->FindSubscriber( Subject, Key )) != NULL)
		{
			for( register int i = 0; i < Subscribers->ItemCount(); i++ )
			{
				Subscriber = Subscribers->GetItem( i );
				if( ( EventListener = Subscriber->getListener() ) != NULL )
					EventListener->OnMigoMessage( Subject, Key,  &TreeData );
			}
		}
		delete MsgHeader;
	}
}
//---------------------------------------------------------------------------
void MWinApp::FireOnConnected( MWinApp* App )
{
	if( App->FListener != NULL )
		App->FListener->OnMAppConnected();
}
//---------------------------------------------------------------------------
void MWinApp::FireOnError( MWinApp* App,PMAppError Error )
{
	if( App->FListener != NULL )
		App->FListener->OnMAppError( Error );
}
//---------------------------------------------------------------------------
void MWinApp::FireOnDisconnected( MWinApp* App )
{
	if( App->FListener != NULL )
		App->FListener->OnMAppDisconnected();
}
//---------------------------------------------------------------------------
LRESULT CALLBACK MWinApp::MessageWndProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	switch( Msg )
    {
		case WM_ON_MESSAGE:    FireOnMessage( static_cast<MWinApp*>((void*)wParam),static_cast<DataMessage*>((void*)lParam) );
							   return 0;
		case WM_ON_CONNECT:    FireOnConnected( static_cast<MWinApp*>((void*)wParam));
							   return 0;
		case WM_ON_DISCONNECT: FireOnDisconnected( static_cast<MWinApp*>((void*)wParam));
							   return 0;
		case WM_ON_ERROR:      FireOnError( static_cast<MWinApp*>((void*)wParam),(PMAppError)lParam);
							   return 0;
		case WM_DESTROY:	   return 0;
		default:               return DefWindowProcA( hwnd, Msg, wParam, lParam);
    }
}
//---------------------------------------------------------------------------
BOOL MWinApp::InitMessageWindow( void )
{
	GUID        Guid;
	WNDCLASSEXA wcx;

	if( CoCreateGuid( &Guid ) == S_OK )
		sprintf( MessageWindowsClassName, "%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X",
										   Guid.Data1, Guid.Data2, Guid.Data3,
										   Guid.Data4[0], Guid.Data4[1], Guid.Data4[2], Guid.Data4[3],
										   Guid.Data4[4], Guid.Data4[5], Guid.Data4[6], Guid.Data4[7]);
	else
		sprintf( MessageWindowsClassName, "%s_MsgWin_%10d", FAppName.c_str(), UFC::GetTickCountMS());
	// Fill in the window class structure with parameters that describe the main window.
	wcx.cbSize        = sizeof( wcx );          // size of structure
	wcx.style         = CS_HREDRAW |CS_VREDRAW; // redraw if size changes
	wcx.lpfnWndProc   = MWinApp::MessageWndProc;// points to window procedure
	wcx.cbClsExtra    = 0;                      // no extra class memory
	wcx.cbWndExtra    = 0;                      // no extra window memory
	wcx.hInstance     = FInstance;              // handle to instance
	wcx.hIcon         = NULL;                   // predefined app. icon
	wcx.hCursor       = NULL;                   // predefined arrow
	wcx.hbrBackground = NULL;                   // background brush
	wcx.lpszMenuName  = NULL;                   // name of menu resource
	wcx.lpszClassName = MessageWindowsClassName;// name of window class
	wcx.hIconSm = NULL;
	if( RegisterClassExA( &wcx ) == 0 )          // Register the window class.
		return FALSE;
	return TRUE;
}
//---------------------------------------------------------------------------
BOOL MWinApp::CreateMessageWindow( void ) // Create the message window.
{
//	printf( "Call CreateWindowA thread[%d]\n", ::GetCurrentThreadId() );
	FHwnd = CreateWindowA(  MessageWindowsClassName,// name of window class
							"MBusApp",              // title-bar string
							WS_OVERLAPPEDWINDOW ,// top-level window
							10,                  // horizontal position
							10,                  // vertical position
							100,                 // width
							100,                 // height
							HWND_MESSAGE,        // Message only window
							NULL,                // no class menu
							FInstance,           // handle to application instance
							NULL );              // no window-creation data
	if( FHwnd == NULL )
		return FALSE;
//	printf( "Create Window[%ld]\n", FHwnd );
//	ShowWindow( FHwnd, SW_SHOW );
//	UpdateWindow( FHwnd );
	return TRUE;
}
//---------------------------------------------------------------------------


