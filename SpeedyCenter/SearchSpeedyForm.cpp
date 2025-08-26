//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "SearchSpeedyForm.h"
#include "SimTFXMain.h"
#include "Speedy.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "GraphMarquee"
#pragma resource "*.dfm"
TSearchForm *SearchForm;
//---------------------------------------------------------------------------
__fastcall TSearchForm::TSearchForm(TComponent* Owner)
: TForm(Owner)
,FVisibleMode( true )
{
}
//---------------------------------------------------------------------------
void __fastcall TSearchForm::FormShow(TObject *Sender)
{
	OkBtn->Visible = false;
	CancelBtn->Visible = false;
	DetectLabel->Visible = true;
	Marquee->Visible = true;
	Marquee->Enabled = true;

	FSimTFXForm->Enabled = false;
	ServerListView->Clear();
	PublishTimer->Enabled = true;
	TimeoutTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSearchForm::WaitForServer(  TSimTFXForm* ParentForm, const AnsiString& SpeedyHost )
{
	FVisibleMode = false;
	FSimTFXForm  = ParentForm;
	FWaitHost    = SpeedyHost;
	if( Visible == true )
	{
		FSimTFXForm->Enabled = false;
		ServerListView->Clear();
		PublishTimer->Enabled = true;
		TimeoutTimer->Enabled = true;
	}
	else
	{
		Left = -10000;
		Top  = -10000;
		Visible = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSearchForm::PublishTimerTimer(TObject *Sender)
{
	MTree Data;

	Data.append( "COMMAND", SPEEDY_AGENT_ACK );
	FSimTFXForm->SpeedyAgentPublisher->SendData( &Data );
}
//---------------------------------------------------------------------------
void __fastcall TSearchForm::TimeoutTimerTimer(TObject *Sender)
{

	PublishTimer->Enabled = false;
	TimeoutTimer->Enabled = false;
	if( FVisibleMode == true )
	{
		DetectLabel->Visible  = false;
		Marquee->Visible      = false;
		Marquee->Enabled      = false;
		OkBtn->Visible        = true;
		CancelBtn->Visible    = true;
		if( ServerListView->Items->Count == 0 )
			OkBtn->Enabled = false;
		else
			OkBtn->Enabled = TRUE;
	}
	else
	{
		String Msg;

		CancelBtnClick( this );
		Msg.printf( Scstrings_MAIN_SPEEDYAGENT_NOT_EXIST.c_str(), String(FWaitHost) );
		ShowMessage( Msg );
	}
}
//---------------------------------------------------------------------------//---------------------------------------------------------------------------
void __fastcall TSearchForm::AddHost( AnsiString& SpeedyHost, int Type )
{
	 int count = ServerListView->Items->Count;

	 TStringList* Str = new TStringList();
	 Str->Delimiter = ',';
	 Str->DelimitedText = SpeedyHost;
	 for( int i = 0; i < count; i++ )
	 {
		  if( ServerListView->Items->Item[i]->Caption.Compare( Str->Strings[0] )==0)
			  return;
	 }
	 TListItem* NewHost = ServerListView->Items->Add();
	 NewHost->Caption = Str->Strings[0];
	 AnsiString VerStr;

	 VerStr.printf("%01d.%01d.%02d", Type/10000, (Type%10000)/1000, (Type%1000)/10);
	 NewHost->SubItems->Add( VerStr );
	 NewHost->Data = (void*)(Type/10);
	 NewHost->ImageIndex = 10 + Type%10;
	 if( Str->Count == 2 )
		 NewHost->SubItems->Add( Str->Strings[1] );
	 else
		 NewHost->SubItems->Add( L"---- -- --" );
	 delete Str;
	 if( FVisibleMode == false && NewHost->Caption.Compare(FWaitHost) == 0 )
	 {
		 NewHost->Selected = true;
		 ServerListViewDblClick( this );
	 }
}
//---------------------------------------------------------------------------
void __fastcall TSearchForm::WndProc( TMessage& Msg )
{
    if( Msg.Msg == WM_ADD_HOSTS )
    {
		AnsiString* NewHost = (AnsiString*) Msg.WParam;
		AddHost( *NewHost , Msg.LParam );
        delete NewHost;
    }
    TForm::WndProc( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TSearchForm::ServerListViewDblClick(TObject *Sender)
{
	if( ServerListView->Selected != NULL )
	{
		Visible = false;
		PublishTimer->Enabled = false;
		TimeoutTimer->Enabled = false;
		FSelHost = ServerListView->Selected->Caption;
		String LicenseStr = Scstrings_MAIN_LICENSE_EXPIRED_DATE + ServerListView->Selected->SubItems->Strings[1];

		FSimTFXForm->StatusBar->Panels->Items[1]->Text = LicenseStr;
		FSimTFXForm->FOSIndex  = ServerListView->Selected->ImageIndex - 10;
		FSimTFXForm->LogonOK( FSelHost, (int)ServerListView->Selected->Data );
		FSimTFXForm->Enabled = true;
	}
	else
	{
		ShowMessage( Scstrings_MAIN_SEL_MONITORING_HOST );
	}
}
//---------------------------------------------------------------------------
void __fastcall TSearchForm::CancelBtnClick(TObject *Sender)
{
	Visible = false;
	FSimTFXForm->LogoutButtonClick( NULL );
	FSimTFXForm->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSearchForm::Show( TSimTFXForm* ParentForm )
{
	FSimTFXForm = ParentForm;
	Visible = true;
}
//---------------------------------------------------------------------------

