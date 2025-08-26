#include "TThreadApp.h"
//---------------------------------------------------------------------------
__fastcall TThreadApp::TThreadApp( MApp* pApp )
:TThread( false )
,FApp( pApp )
{
	FStartEvent = new TEvent( true );
}
//---------------------------------------------------------------------------
void __fastcall TThreadApp::Wait( int ms )
{
	FStartEvent->WaitFor( ms );
}
//---------------------------------------------------------------------------
__fastcall TThreadApp::~TThreadApp(void)
{
	Terminate();  ///< Try to terminate this thread.
	FApp->stop(); ///< Stop the MApp Object.
	WaitFor();    ///< Wait for this thread terminated.
	delete FStartEvent;
}
//---------------------------------------------------------------------------
void __fastcall TThreadApp::Execute(void)
{
	FStartEvent->SetEvent();
	FApp->start( FALSE );
}
//---------------------------------------------------------------------------

