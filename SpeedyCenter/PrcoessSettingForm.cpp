//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PrcoessSettingForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "cspin"
#pragma resource "*.dfm"
TProcessForm *ProcessForm;
//---------------------------------------------------------------------------
__fastcall TProcessForm::TProcessForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
__fastcall TProcessForm::TProcessForm( AnsiString Name, AnsiString Start, AnsiString Stop, bool AutoChecked,
									   bool WatchDogChecked, int WatchSec, int RestartTimes )
	: TForm((TComponent*)NULL)
{
    AnsiString Value;

    ProcessLabel->Caption = Name;
    Value = Start.SubString(1,2);
    CStartHourSpinEdit->Value    = Value.ToInt();
    Value = Start.SubString(4,2);
	CStartMinutesSpinEdit->Value = Value.ToInt();
    Value = Stop.SubString(1,2);
    CStopHourSpinEdit->Value     = Value.ToInt();
    Value = Stop.SubString(4,2);
	CStopMinutesSpinEdit->Value  = Value.ToInt();
	AutoCheckBox->Checked = AutoChecked;
	if( CStartHourSpinEdit->Value*100 + CStartMinutesSpinEdit->Value >
		CStopHourSpinEdit->Value*100 + CStopMinutesSpinEdit->Value )
		NextDayLabel->Visible = true;
	else
		NextDayLabel->Visible = false;
	Height = 350;
	WatchDogCheckBox->Checked = WatchDogChecked;
	WatchSecondsSpinEdit->Value = WatchSec;
	RestartTimesSpinEdit->Value = RestartTimes;
	WatchDogGroupBox->Visible = true;
}
//---------------------------------------------------------------------------
__fastcall TProcessForm::TProcessForm( AnsiString Name, AnsiString Start, AnsiString Stop, bool AutoChecked )
	: TForm((TComponent*)NULL)
{
	AnsiString Value;

	ProcessLabel->Caption = Name;
	Value = Start.SubString(1,2);
	CStartHourSpinEdit->Value    = Value.ToInt();
	Value = Start.SubString(4,2);
	CStartMinutesSpinEdit->Value = Value.ToInt();
	Value = Stop.SubString(1,2);
	CStopHourSpinEdit->Value     = Value.ToInt();
	Value = Stop.SubString(4,2);
	CStopMinutesSpinEdit->Value  = Value.ToInt();
	AutoCheckBox->Checked = AutoChecked;
	if( CStartHourSpinEdit->Value*100 + CStartMinutesSpinEdit->Value >
		CStopHourSpinEdit->Value*100 + CStopMinutesSpinEdit->Value )
		NextDayLabel->Visible = true;
	else
		NextDayLabel->Visible = false;
	Height = 225;
	WatchDogCheckBox->Checked = false;
	WatchSecondsSpinEdit->Value = 10;
	RestartTimesSpinEdit->Value = 3;
	WatchDogGroupBox->Visible = false;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TProcessForm::GetStartTime( void )
{
	AnsiString Rtn;
	Rtn.printf("%02d:%02d",CStartHourSpinEdit->Value,CStartMinutesSpinEdit->Value);
	return Rtn;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TProcessForm::GetStopTime( void )
{
	AnsiString Rtn;
	Rtn.printf("%02d:%02d",CStopHourSpinEdit->Value,CStopMinutesSpinEdit->Value);
	return Rtn;
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::CStartHourSpinEditKeyPress(TObject *Sender,
	  char &Key)
{
	if( Key < '0' || Key > '9' )
		Key = 0;
	if( CStartHourSpinEdit->Value*100 + CStartMinutesSpinEdit->Value >
		CStopHourSpinEdit->Value*100 + CStopMinutesSpinEdit->Value )
		NextDayLabel->Visible = true;
	else
		NextDayLabel->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::EnableWatchDog( bool Enable )
{
	WatchSecondsSpinEdit->Enabled = Enable;
	RestartTimesSpinEdit->Enabled = Enable;
	CheckLabel->Enabled           = Enable;
	MinutesLabel->Enabled         = Enable;
	RestartLabel->Enabled         = Enable;
	TimesLabel->Enabled           = Enable;
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::EnableAutoStartStop( bool Enable )
{
	CStartHourSpinEdit->Enabled    = Enable;
	CStartMinutesSpinEdit->Enabled = Enable;
	CStopHourSpinEdit->Enabled     = Enable;
	CStopMinutesSpinEdit->Enabled  = Enable;
	StartLabel->Enabled            = Enable;
	StopLabel->Enabled             = Enable;
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::AutoCheckBoxClick(TObject *Sender)
{
	if( AutoCheckBox->Checked == true )
	{
		EnableAutoStartStop( true );
		EnableWatchDog( true );
		WatchDogCheckBox->Enabled = true;
	}
	else
	{
		EnableAutoStartStop( false );
		EnableWatchDog( false );
		WatchDogCheckBox->Checked = false;
		WatchDogCheckBox->Enabled = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TProcessForm::CStartHourSpinEditChange(TObject *Sender)
{
	if( CStartHourSpinEdit->Value*100 + CStartMinutesSpinEdit->Value >
		CStopHourSpinEdit->Value*100 + CStopMinutesSpinEdit->Value )
		NextDayLabel->Visible = true;
	else
		NextDayLabel->Visible = false;
}
//---------------------------------------------------------------------------

