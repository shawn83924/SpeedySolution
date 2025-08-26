//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "NewSession.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cspin"
#pragma resource "*.dfm"
TSessionForm *SessionForm;
//---------------------------------------------------------------------------
__fastcall TSessionForm::TSessionForm(TComponent* Owner )
    : TForm(Owner)
{
    SetWindowLong( PageControl->Handle,GWL_STYLE,0x54000100);
    TypeRadioGroup->ItemIndex = 0;
    PageControl->ActivePage = InitiatorSheet;
	APortEdit->Text = IntToStr( TAcceptor::GetAcceptorPort() );
}
//---------------------------------------------------------------------------
__fastcall TSessionForm::TSessionForm( TStringList* FIXDicts )
	: TForm((TComponent*)NULL)
{
	SetWindowLong( PageControl->Handle,GWL_STYLE,0x54000100);
	TypeRadioGroup->ItemIndex = 0;
	PageControl->ActivePage = InitiatorSheet;
	APortEdit->Text = IntToStr( TAcceptor::GetAcceptorPort() );

	FIXDictComboBox->Items->Add( L"[None]" );
	FIXDictComboBox->Items->AddStrings( FIXDicts );
	FIXDictComboBox->ItemIndex = FIXDictComboBox->Items->IndexOf( L"FIX44.xml" );
	AppVLabel->Visible = false;
	AppVComboBox->Visible = false;
	PersistCheckBox->Checked = true;
	Caption = Scstrings_MAIN_FIX_SESSION_ADD;//"新增連線";
}
//---------------------------------------------------------------------------
__fastcall TSessionForm::TSessionForm( TFIXSession* Session, TStringList* FIXDicts )
: TForm(Owner)
{
	FModifyInitiator = dynamic_cast<TInitiator*>(Session);
	FModifyAcceptor  = dynamic_cast<TAcceptor*>(Session);

	SetWindowLong( PageControl->Handle,GWL_STYLE,0x54000100);
	FIXDictComboBox->Items->Add( L"[None]" );
	FIXDictComboBox->Items->AddStrings( FIXDicts );
	if( FModifyInitiator != NULL  || FModifyAcceptor != NULL )
	{
		FIXVerComboBox->ItemIndex  = FIXVerComboBox->Items->IndexOf( Session->GetFIXVersion() );
		FIXDictComboBox->ItemIndex = FIXDictComboBox->Items->IndexOf( Session->GetDictonary() );
		if( Session->GetFIXVersion() == "FIXT.1.1" )
		{
			AppVLabel->Visible    = true;
			AppVComboBox->Visible = true;
			AppVComboBox->ItemIndex = AppVComboBox->Items->IndexOf( Session->GetFIXAppVersion() );
		}
		else
		{
			AppVLabel->Visible    = false;
			AppVComboBox->Visible = false;
			AppVComboBox->ItemIndex = AppVComboBox->Items->IndexOf( L"FIX.5.0SP2" );
        }
		SenderIDEdit->Text         = Session->GetSenderCompID();
		TargetIDEdit->Text         = Session->GetTargetCompID();
		if( FModifyInitiator != NULL )
		{
			TypeRadioGroup->ItemIndex  = 0;
			PageControl->ActivePage    = InitiatorSheet;
			HeartbeatEdit->Text        = FModifyInitiator->GetHeartbeatInterval();
			ReconnectEdit->Text        = FModifyInitiator->GetReconnectInterval();
			HostEdit->Text             = FModifyInitiator->GetHostIP( 0 );
			PortEdit->Text             = IntToStr(FModifyInitiator->GetHostPort( 0 ));
			if( FModifyInitiator->HostCount() >= 2 )
			{
				Host1Edit->Text        = FModifyInitiator->GetHostIP( 1 );
				Port1Edit->Text        = IntToStr(FModifyInitiator->GetHostPort( 1 ));
			}
			if( FModifyInitiator->HostCount() >= 3 )
			{
				Host2Edit->Text        = FModifyInitiator->GetHostIP( 2 );
				Port2Edit->Text        = IntToStr(FModifyInitiator->GetHostPort( 2 ));
			}
		}
		if( FModifyAcceptor != NULL )
		{
			TypeRadioGroup->ItemIndex  = 1;
			PageControl->ActivePage    = AcceptSheet;
			APortEdit->Text            = IntToStr( TAcceptor::GetAcceptorPort() );
		}
		PersistCheckBox->Checked      = Session->GetPersistMessages( );
		ResetOnLogonCheckBox->Checked = Session->GetResetOnLogon();
		VerifySeqCheckBox->Checked    = Session->GetVerifyMsgSeqNum();

		int StartInt = Session->GetStartTime();
		int StopInt  = Session->GetEndTime();
		StartHourSpinEdit->Value    = StartInt/10000;
		StartMinutesSpinEdit->Value = (StartInt%10000)/100;
		StartSecSpinEdit->Value       = StartInt%100;
		EndHourSpinEdit->Value      = StopInt/10000;
		EndMinutesSpinEdit->Value   = (StopInt%10000)/100;
		EndSecSpinEdit->Value       = StopInt%100;
		int sdindex = StartDayComboBox->Items->IndexOf( Session->GetStartDay() );
		if( sdindex == -1 )
			StartDayComboBox->ItemIndex = 0;
		else
			StartDayComboBox->ItemIndex = sdindex;
		int edindex = EndDayComboBox->Items->IndexOf( Session->GetEndDay() );
		if( edindex == -1 )
			EndDayComboBox->ItemIndex = 0;
		else
			EndDayComboBox->ItemIndex = edindex;
	}
	TypeRadioGroup->Enabled = false;
	Caption = Scstrings_MAIN_FIX_SESSION_MODIFY;//"修改連線設定";
}
//---------------------------------------------------------------------------
bool __fastcall TSessionForm::CheckInitiator( )
{
	FIXVersion   = FIXVerComboBox->Text;
	SenderCompID = SenderIDEdit->Text;
	TargetCompID = TargetIDEdit->Text;

	if( SenderCompID.Length() == 0 )
	{
		ShowMessage( Scstrings_MAIN_FIX_SESSION_INPUT_VALID_SENDER_ID );//"請輸入正確的 SenderCompID.");
		return false;
	}
	if( TargetCompID.Length() == 0 )
	{
		ShowMessage( Scstrings_MAIN_FIX_SESSION_INPUT_VALID_TARGET_ID );///"請輸入正確的 TargetCompID.");
		return false;
	}
	try
	{
		HeartbeatIntival = HeartbeatEdit->Text.ToInt();
		ReconnectIntival = ReconnectEdit->Text.ToInt();
	}
	catch(...)
	{
		ShowMessage( Scstrings_MAIN_FIX_SESSION_INTERVAL_MUST_NUM );
		return false;
	}
	if( HostEdit->Text.IsEmpty() || PortEdit->Text.IsEmpty() )
	{
		ShowMessage( Scstrings_MAIN_FIX_SESSION_INPUT_VALID_IP_PORT );
        return false;
    }
    try
    {
        PortEdit->Text.ToInt();
        if( !Port1Edit->Text.IsEmpty() )
			Port1Edit->Text.ToInt();
        if( !Port2Edit->Text.IsEmpty() )
            Port2Edit->Text.ToInt();
    }
    catch(...)
    {
		ShowMessage( Scstrings_MAIN_PVC_SETTING_PORT_MUST_NUM  );
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool __fastcall TSessionForm::CheckAcceptor( )
{
	FIXVersion   = FIXVerComboBox->Text;
	SenderCompID = SenderIDEdit->Text;
	TargetCompID = TargetIDEdit->Text;
	if( SenderCompID.Length() == 0 )
    {
		ShowMessage( Scstrings_MAIN_FIX_SESSION_INPUT_VALID_SENDER_ID );
        return false;
    }
    if( TargetCompID.Length() == 0 )
    {
		ShowMessage( Scstrings_MAIN_FIX_SESSION_INPUT_VALID_TARGET_ID );
        return false;
    }
    try
    {
		AcceptorPort = APortEdit->Text.ToInt();
    }
    catch(...)
    {
		ShowMessage( Scstrings_MAIN_PVC_SETTING_PORT_MUST_NUM );
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TSessionForm::OKBtnClick(TObject *Sender)
{
    bool Result;
	if( TypeRadioGroup->ItemIndex == 0 )
		Result = CheckInitiator();
	else
		Result = CheckAcceptor();
	if( Result )
		ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TSessionForm::TypeRadioGroupClick(TObject *Sender)
{
	if( TypeRadioGroup->ItemIndex == 0 )
		PageControl->ActivePage = InitiatorSheet;
	else
		PageControl->ActivePage = AcceptSheet;
}
//---------------------------------------------------------------------------
void __fastcall TSessionForm::FIXVerComboBoxChange(TObject *Sender)
{
	int SelItem = 0;

	if( FIXVerComboBox->Text == L"FIX.4.0" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX40.xml");
	else if( FIXVerComboBox->Text == L"FIX.4.1" )
		SelItem= FIXDictComboBox->Items->IndexOf(L"FIX41.xml");
	else if( FIXVerComboBox->Text == L"FIX.4.2" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX42.xml");
	else if( FIXVerComboBox->Text == L"FIX.4.3" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX43.xml");
	else if( FIXVerComboBox->Text == L"FIX.4.4" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX44.xml");
	else if( FIXVerComboBox->Text == L"FIXT.1.1" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX50SP2.xml");

	if( FIXVerComboBox->Text == L"FIXT.1.1" )
	{
		AppVLabel->Visible = true;
		AppVComboBox->Visible = true;
	}
	else
	{
		AppVLabel->Visible = false;
		AppVComboBox->Visible = false;
	}
	if( SelItem == -1 )
		SelItem = 0;
	FIXDictComboBox->ItemIndex = SelItem;
}
//---------------------------------------------------------------------------
void __fastcall TSessionForm::AppVComboBoxChange(TObject *Sender)
{
	int SelItem = 0;

	if( AppVComboBox->Text == L"FIX.4.0" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX40.xml");
	else if( AppVComboBox->Text == L"FIX.4.1" )
		SelItem= FIXDictComboBox->Items->IndexOf(L"FIX41.xml");
	else if( AppVComboBox->Text == L"FIX.4.2" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX42.xml");
	else if( AppVComboBox->Text == L"FIX.4.3" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX43.xml");
	else if( AppVComboBox->Text == L"FIX.4.4" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX44.xml");
	else if( AppVComboBox->Text == L"FIX.5.0" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX50.xml");
	else if( AppVComboBox->Text == L"FIX.5.0SP1" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX50SP1.xml");
	else if( AppVComboBox->Text == L"FIX.5.0SP2" )
		SelItem = FIXDictComboBox->Items->IndexOf(L"FIX50SP2.xml");
	if( SelItem == -1 )
		SelItem = 0;
	FIXDictComboBox->ItemIndex = SelItem;
}
//---------------------------------------------------------------------------
void __fastcall TSessionForm::UpdateSession()
{
	int StartInt = StartHourSpinEdit->Value * 10000 +  StartMinutesSpinEdit->Value * 100 + StartSecSpinEdit->Value;
	int StopInt  = EndHourSpinEdit->Value * 10000 +  EndMinutesSpinEdit->Value * 100 + EndSecSpinEdit->Value;

	if( FModifyInitiator != NULL )
	{
		FModifyInitiator->SetFIXVersion( FIXVerComboBox->Text );
		FModifyInitiator->SetFIXAppVersion( AppVComboBox->Text );
		FModifyInitiator->SetSenderCompID( SenderIDEdit->Text );
		FModifyInitiator->SetTargetCompID( TargetIDEdit->Text );
		FModifyInitiator->SetDictonary( FIXDictComboBox->Text );
		FModifyInitiator->SetHeartbeatInterval( HeartbeatEdit->Text.ToInt() );
		FModifyInitiator->SetReconnectInterval( ReconnectEdit->Text.ToInt() );
		FModifyInitiator->ClearHosts();
		FModifyInitiator->AddHost( HostEdit->Text,PortEdit->Text.ToInt() );
		if( !Host1Edit->Text.IsEmpty() && !Port1Edit->Text.IsEmpty() )
			FModifyInitiator->AddHost( Host1Edit->Text,Port1Edit->Text.ToInt() );
		if( !Host2Edit->Text.IsEmpty() && !Port2Edit->Text.IsEmpty() )
			FModifyInitiator->AddHost( Host2Edit->Text,Port2Edit->Text.ToInt() );
		FModifyInitiator->SetPersistMessages( PersistCheckBox->Checked );
		FModifyInitiator->SetStartTime( StartInt );
		FModifyInitiator->SetEndTime( StopInt );
		FModifyInitiator->SetStartDay( StartDayComboBox->Text );
		FModifyInitiator->SetEndDay( EndDayComboBox->Text );
		FModifyInitiator->SetVerifyMsgSeqNum( VerifySeqCheckBox->Checked );
	}
	if( FModifyAcceptor != NULL )
	{
		FModifyAcceptor->SetFIXVersion( FIXVerComboBox->Text );
		FModifyAcceptor->SetFIXAppVersion( AppVComboBox->Text );
		FModifyAcceptor->SetSenderCompID( SenderIDEdit->Text );
		FModifyAcceptor->SetTargetCompID( TargetIDEdit->Text );
		FModifyAcceptor->SetDictonary( FIXDictComboBox->Text );
		TAcceptor::SetAcceptorPort( APortEdit->Text.ToInt() );
		FModifyAcceptor->SetPersistMessages( PersistCheckBox->Checked );
		FModifyAcceptor->SetStartTime( StartInt );
		FModifyAcceptor->SetEndTime( StopInt );
		FModifyAcceptor->SetStartDay( StartDayComboBox->Text );
		FModifyAcceptor->SetEndDay( EndDayComboBox->Text );
		FModifyAcceptor->SetResetOnLogon( ResetOnLogonCheckBox->Checked );
		FModifyAcceptor->SetVerifyMsgSeqNum( VerifySeqCheckBox->Checked );
	}
}
//---------------------------------------------------------------------------
TFIXSession* __fastcall TSessionForm::CreateSession( bool& IsAcceptor )
{
	int StartInt = StartHourSpinEdit->Value * 10000 +  StartMinutesSpinEdit->Value * 100 + StartSecSpinEdit->Value;
	int StopInt  = EndHourSpinEdit->Value * 10000 +  EndMinutesSpinEdit->Value * 100 + EndSecSpinEdit->Value;

	if( TypeRadioGroup->ItemIndex == 0 )
	{
		TInitiator* Initiator = new TInitiator( FIXVersion.c_str(),SenderCompID.c_str(),TargetCompID.c_str());
		Initiator->SetFIXAppVersion( AppVComboBox->Text );
		Initiator->SetDictonary( FIXDictComboBox->Text );
		Initiator->SetHeartbeatInterval( HeartbeatIntival );
		Initiator->SetReconnectInterval( ReconnectIntival );
		Initiator->AddHost( HostEdit->Text, PortEdit->Text.ToInt() );
		if( !Host1Edit->Text.IsEmpty() && !Port1Edit->Text.IsEmpty() )
			Initiator->AddHost( Host1Edit->Text, Port1Edit->Text.ToInt() );
		if( !Host2Edit->Text.IsEmpty() && !Port2Edit->Text.IsEmpty() )
			Initiator->AddHost( Host2Edit->Text, Port2Edit->Text.ToInt() );
		Initiator->SetPersistMessages( PersistCheckBox->Checked );
		Initiator->SetStartTime( StartInt );
		Initiator->SetEndTime( StopInt );
		Initiator->SetStartDay( StartDayComboBox->Text );
		Initiator->SetEndDay( EndDayComboBox->Text );
		Initiator->SetVerifyMsgSeqNum( VerifySeqCheckBox->Checked );
		IsAcceptor = false;
		return  Initiator;
	}
	else
	{
		TAcceptor* Acceptor = new TAcceptor( FIXVersion.c_str(),SenderCompID.c_str(),TargetCompID.c_str());
		Acceptor->SetFIXAppVersion( AppVComboBox->Text );
		Acceptor->SetDictonary( FIXDictComboBox->Text );
		TAcceptor::SetAcceptorPort( AcceptorPort );
		Acceptor->SetPersistMessages( PersistCheckBox->Checked );
		Acceptor->SetStartTime( StartInt );
		Acceptor->SetEndTime( StopInt );
		Acceptor->SetStartDay( StartDayComboBox->Text );
		Acceptor->SetEndDay( EndDayComboBox->Text );
		Acceptor->SetResetOnLogon( ResetOnLogonCheckBox->Checked );
		Acceptor->SetVerifyMsgSeqNum( VerifySeqCheckBox->Checked );
		IsAcceptor = true;
		return Acceptor;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSessionForm::StartDayComboBoxChange(TObject *Sender)
{
	if( StartDayComboBox->ItemIndex == 0 )
	{
		if( EndDayComboBox->ItemIndex != 0 )
			EndDayComboBox->ItemIndex = 0;
	}
	else
	{
		if( EndDayComboBox->ItemIndex == 0 )
			EndDayComboBox->ItemIndex = 1;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSessionForm::EndDayComboBoxChange(TObject *Sender)
{
	if( EndDayComboBox->ItemIndex == 0 )
	{
		if( StartDayComboBox->ItemIndex != 0 )
			StartDayComboBox->ItemIndex = 0;
	}
	else
	{
		if( StartDayComboBox->ItemIndex == 0 )
			StartDayComboBox->ItemIndex = 1;
    }
}
//---------------------------------------------------------------------------

