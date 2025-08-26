//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "EventViewer.h"
#include "SimTFXMain.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEventForm *EventForm;
extern AnsiString CurrentDir;
//---------------------------------------------------------------------------
__fastcall TEventForm::TEventForm(TComponent* Owner)
    : TForm(Owner)
{
    Load();
}
//---------------------------------------------------------------------------
__fastcall TEventForm::~TEventForm( void )
{
    Save( CurrentDir +"\\Events");
}
//---------------------------------------------------------------------------
void __fastcall TEventForm::AddEvent( const AnsiString& Host, const AnsiString& Type, const AnsiString& Msg )
{
	TListItem* NewLog = EventListView->Items->Insert( 0 );
	TDateTime Now = TDateTime::CurrentDateTime();

	if( Type == "inf" )
		NewLog->ImageIndex = 0; ///< information message
	else if( Type == "war" )
		NewLog->ImageIndex = 1; ///< warning message
	else if( Type == "err" )
		NewLog->ImageIndex = 2; ///< Error message
	NewLog->SubItems->Add( Host );            ///< Hostname
	NewLog->SubItems->Add( Now.DateString() );///< Date
	NewLog->SubItems->Add( Now.TimeString() );///< Time
	NewLog->SubItems->Add( Msg );             ///< Message
	StatusBar->Panels->Items[1]->Text = IntToStr( EventListView->Items->Count )+ Scstrings_MAIN_EVENT_VIEWER_EVENTS;//" 個事件";
}
//---------------------------------------------------------------------------
void __fastcall TEventForm::FormShow(TObject *Sender)
{
	SetBounds( Screen->WorkAreaWidth - Width - 10,Screen->WorkAreaHeight - Height -10, Width, Height );
}
//---------------------------------------------------------------------------
void __fastcall TEventForm::ToolButton2Click(TObject *Sender)
{
	EventListView->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TEventForm::Save( const AnsiString& FileName )
{
	TStringList* Log = new TStringList();
	for( int i = 0; i < EventListView->Items->Count; i++ )
	{
		TListItem* Item = EventListView->Items->Item[ i ];

		AnsiString LogStr;
		if( Item->ImageIndex == 0 )      ///< information message
			LogStr = "inf,\"" + Item->SubItems->Strings[0]+"\",\""+Item->SubItems->Strings[1]+"\",\""+Item->SubItems->Strings[2]+"\",\""+Item->SubItems->Strings[3]+"\"";
		else if( Item->ImageIndex == 1 ) ///< warning message
			LogStr = "war,\"" + Item->SubItems->Strings[0]+"\",\""+Item->SubItems->Strings[1]+"\",\""+Item->SubItems->Strings[2]+"\",\""+Item->SubItems->Strings[3]+"\"";
		else if( Item->ImageIndex == 2 ) ///< Error message
			LogStr = "err,\"" + Item->SubItems->Strings[0]+"\",\""+Item->SubItems->Strings[1]+"\",\""+Item->SubItems->Strings[2]+"\",\""+Item->SubItems->Strings[3]+"\"";
		Log->Add( LogStr );
	}
	Log->SaveToFile( FileName );
	delete Log;
}
//---------------------------------------------------------------------------
void __fastcall TEventForm::Load( void )
{
	if( FileExists( CurrentDir +"\\Events" ))
	{
		int Begin;

		EventListView->Clear();
		TStringList* Log = new TStringList();
		TStringList* Parser = new TStringList();
		Log->LoadFromFile( CurrentDir +"\\Events" );
		Begin = Log->Count - 200;
		if( Begin < 0 )
			Begin = 0;
		for( int i = Begin; i < Log->Count; i++)
		{
			Parser->Delimiter = ',';
			Parser->DelimitedText = Log->Strings[i];
			if( Parser->Count >= 5 )
			{
				TListItem* NewLog = EventListView->Items->Insert( 0 );
				if( Parser->Strings[0].Compare("inf") == 0 )
					NewLog->ImageIndex = 0; ///< information message
				else if( Parser->Strings[0].Compare("war") == 0 )
					NewLog->ImageIndex = 1; ///< warning message
				else if( Parser->Strings[0].Compare("err") == 0 )
					NewLog->ImageIndex = 2; ///< Error message
				else
					continue; ///< Unknown
				NewLog->SubItems->Add( Parser->Strings[1] );///< Host
				NewLog->SubItems->Add( Parser->Strings[2] );///< Date
				NewLog->SubItems->Add( Parser->Strings[3] );///< Time
				NewLog->SubItems->Add( Parser->Strings[4] ); ///< Message
			}
		}
		delete Parser;
		delete Log;
	}
	StatusBar->Panels->Items[1]->Text = IntToStr( EventListView->Items->Count )+ Scstrings_MAIN_EVENT_VIEWER_EVENTS;//" 個事件";

}
//---------------------------------------------------------------------------
void __fastcall TEventForm::ToolButton1Click(TObject *Sender)
{
    if( SaveDialog->Execute() )
        Save( SaveDialog->FileName );
}
//---------------------------------------------------------------------------

