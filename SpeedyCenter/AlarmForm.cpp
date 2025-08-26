//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "AlarmForm.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cspin"
#pragma resource "*.dfm"
TAlarmSettingForm *AlarmSettingForm;
//---------------------------------------------------------------------------
__fastcall AlarmInfo::AlarmInfo( const AnsiString& Str )
{
   TStringList* Strs = new TStringList();
   Strs->Delimiter = ',';
   Strs->DelimitedText = Str;

   Fmarket    = Strs->Strings[ 0 ].ToInt();
   FBeginTime = Strs->Strings[ 1 ].ToInt();
   FEndTime   = Strs->Strings[ 2 ].ToInt();
   FSec       = Strs->Strings[ 3 ].ToInt();
}
//---------------------------------------------------------------------------
AnsiString __fastcall AlarmInfo::toString( )
{
	AnsiString Str;
	Str.printf( "%d,%d,%d,%d", Fmarket, FBeginTime,  FEndTime, FSec );
	return Str;
}
//---------------------------------------------------------------------------
AnsiString __fastcall AlarmInfo::GetMarket()
{
	switch( Fmarket )
	{
		case 0:  return Scstrings_MAIN_OPTIONS;//"選擇權";
		case 1:  return Scstrings_MAIN_FUTURES;//"期貨";
		case 2:  return Scstrings_MAIN_TSEC;//"集中";
		default: return Scstrings_MAIN_OTC;//"櫃檯";
	}
}
//---------------------------------------------------------------------------
AnsiString __fastcall AlarmInfo::GetBeginTime()
{
	AnsiString Str;
	Str.printf( "%02d:%02d:%02d", FBeginTime/10000, (FBeginTime%10000)/100,FBeginTime%100 );
	return Str;
}
//---------------------------------------------------------------------------
AnsiString __fastcall AlarmInfo::GetEndTime( )
{
	AnsiString Str;
	Str.printf( "%02d:%02d:%02d", FEndTime/10000, (FEndTime%10000)/100,FEndTime%100 );
	return Str;
}
//---------------------------------------------------------------------------
bool __fastcall AlarmInfo::IsCheckingTime( int Time )
{
	if( Time >= FBeginTime && Time <= FEndTime )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall AlarmInfo::IsOverlap( int Market, int Start, int End )
{
	if( Fmarket != Market )
		return false;
	if( Start >= FEndTime )
		return false;
	if( End <= FBeginTime )
		return false;
	return true;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
__fastcall TAlarmSettingForm::TAlarmSettingForm(TComponent* Owner, TObjectList* Alarms)
: TForm(Owner)
, FAlarms( Alarms )
, FMofify( NULL )
{
}
//---------------------------------------------------------------------------
__fastcall TAlarmSettingForm::TAlarmSettingForm(TComponent* Owner, TObjectList* Alarms, AlarmInfo* Mofify )
: TForm( Owner )
, FAlarms( Alarms )
, FMofify( Mofify )
{
	MarketComboBox->ItemIndex = FMofify->Fmarket;
	TimerComboBox->ItemIndex = TimerComboBox->Items->IndexOf( IntToStr( FMofify->FSec ) );
	CStartHourSpinEdit->Value = FMofify->FBeginTime/10000;
	CStartMinutesSpinEdit->Value = (FMofify->FBeginTime%10000)/100;
	CStartSecSpinEdit->Value =  FMofify->FBeginTime%100;
	CStopHourSpinEdit->Value =  FMofify->FEndTime/10000;
	CStopMinutesSpinEdit->Value = (FMofify->FEndTime%10000)/100;
	CStopSecSpinEdit->Value =  FMofify->FEndTime%100;
}
//---------------------------------------------------------------------------
AlarmInfo* __fastcall  TAlarmSettingForm::GetAlarmInfo( void )
{
	AlarmInfo* NewAlarm = new AlarmInfo();
	NewAlarm->FSec    = TimerComboBox->Text.ToInt();
	NewAlarm->Fmarket = MarketComboBox->ItemIndex;
	NewAlarm->FBeginTime = CStartHourSpinEdit->Value * 10000 +
						   CStartMinutesSpinEdit->Value *100 +
						   CStartSecSpinEdit->Value;
	NewAlarm->FEndTime   = CStopHourSpinEdit->Value * 10000 +
						   CStopMinutesSpinEdit->Value *100 +
						   CStopSecSpinEdit->Value;
	return NewAlarm;
}
//---------------------------------------------------------------------------
void __fastcall  TAlarmSettingForm::UpdateAlarmInfo( void )
{
	if( FMofify != NULL )
	{
		FMofify->FSec    = TimerComboBox->Text.ToInt();
		FMofify->Fmarket = MarketComboBox->ItemIndex;
		FMofify->FBeginTime = CStartHourSpinEdit->Value * 10000 +
							   CStartMinutesSpinEdit->Value *100 +
							   CStartSecSpinEdit->Value;
		FMofify->FEndTime   = CStopHourSpinEdit->Value * 10000 +
							   CStopMinutesSpinEdit->Value *100 +
							   CStopSecSpinEdit->Value;
	}
}
//---------------------------------------------------------------------------
void __fastcall TAlarmSettingForm::BitBtn1Click(TObject *Sender)
{
	ModalResult = mrNone;
	int Start = CStartHourSpinEdit->Value * 10000 +
				CStartMinutesSpinEdit->Value *100 +
				CStartSecSpinEdit->Value;
	int Stop  = CStopHourSpinEdit->Value * 10000 +
				CStopMinutesSpinEdit->Value *100 +
				CStopSecSpinEdit->Value;
	if( Start > Stop )
	{
		ShowMessage( Scstrings_MAIN_ALARM_BEGIN_SHOULD_BEFORE_STOP_TIME );
		return;
	}
	if( Start == Stop )
	{
		ShowMessage( Scstrings_MAIN_ALARM_BEGIN_STOP_TIME_CANT_EQUAL );
		return;
	}
	for( int i = 0; i < FAlarms->Count; i ++ )
	{
		AlarmInfo* CmpAlarm =  (AlarmInfo*)FAlarms->Items[i];
		if( CmpAlarm != FMofify )
		{
			if( CmpAlarm->IsOverlap( MarketComboBox->ItemIndex, Start, Stop ))
			{
				ShowMessage( Scstrings_MAIN_ALARM_OVERLAPED_TIME_SETTING );
				return;
			}
		}
	}
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
