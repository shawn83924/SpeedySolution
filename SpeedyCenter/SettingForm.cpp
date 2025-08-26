//---------------------------------------------------------------------------
#include <vcl.h>
#include <winnt.h>
#pragma hdrstop
#include "SettingForm.h"
#include "AlarmForm.h"
#include "reinit.hpp"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
extern AnsiString CurrentDir;
extern LCID FCurrentLCID;
TFormSetting *FormSetting;
//---------------------------------------------------------------------------
__fastcall TFormSetting::TFormSetting(TComponent* Owner)
: TForm(Owner)
,FCenter( false )
{
	FAlarms = new TObjectList();
	Sources = new TStringList();
	FormShow( this );
	if( DirectoryExists( CurrentDir + "\\Sound\\" ) == false )
		ForceDirectories( CurrentDir + "\\Sound\\" );
	FSoundListBox = new TFileListBox( this );
	FSoundListBox->Parent = this;
	FSoundListBox->Visible = false;
	FSoundListBox->Directory = CurrentDir + "\\Sound\\";
	FSoundListBox->Mask = "*.wav";

	BrokenComboBox->Items->AddStrings( FSoundListBox->Items );
	CrashComboBox->Items->AddStrings( FSoundListBox->Items );
	WarningComboBox->Items->AddStrings( FSoundListBox->Items );
	NoTradeComboBox->Items->AddStrings( FSoundListBox->Items );
	CPUWarningComboBox->Items->AddStrings( FSoundListBox->Items );
	MemoryWarningComboBox->Items->AddStrings( FSoundListBox->Items );
	DiskWarningComboBox->Items->AddStrings( FSoundListBox->Items );
	TimeDiffWarningComboBox->Items->AddStrings( FSoundListBox->Items );
}
//---------------------------------------------------------------------------
int __fastcall TFormSetting::GetQueueLimit()
{
	return QLimitUpDown->Position;
}
//---------------------------------------------------------------------------
int __fastcall TFormSetting::GetCIDLimit()
{
	return CIDMinUpDown->Position;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TFormSetting::GetProcessErrorSound()
{
	return FSoundListBox->Directory + "//" + FProcessErrSound;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TFormSetting::GetLineErrorSound()
{
	return FSoundListBox->Directory + "//" +  FLineErrSound;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TFormSetting::GetWarningSound()
{
	return FSoundListBox->Directory + "//" +  FWarningSound;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TFormSetting::GetNoTradeSound()
{
	return FSoundListBox->Directory + "//" +  FNoTradeSound;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TFormSetting::GetCPUWarningSound()
{
	return FSoundListBox->Directory + "//" +  FCPUWarningSound;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TFormSetting::GetMemWarningSound()
{
	return FSoundListBox->Directory + "//" +  FMemWarningSound;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TFormSetting::GetDiskWarningSound()
{
	return FSoundListBox->Directory + "//" +  FDiskWarningSound;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TFormSetting::GetTimeDiffWarningSound()
{
	return FSoundListBox->Directory + "//" +  FTimeDiffWarningSound;
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::FormShow(TObject *Sender)
{
	TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");

	FFCM                  = iniFile->ReadString( "FCM Setting", "FCMID","F001000");
	FMBusIP               = iniFile->ReadString( "MBus Setting", "HOST","127.0.0.1");
	///< Alarm sound.
	FProcessErrSound      = iniFile->ReadString( "Warning Setting", "ProcessDown","ProcessErr.wav");
	FLineErrSound         = iniFile->ReadString( "Warning Setting", "LineBroken","LineErr.wav");
	FWarningSound         = iniFile->ReadString( "Warning Setting", "Warning","LineErr.wav");
	FNoTradeSound         = iniFile->ReadString( "Warning Setting", "NoTrade","BankAalarm.wav");
	FCPUWarningSound      = iniFile->ReadString( "Monitoring", "CPUSound","BankAalarm.wav");
	FMemWarningSound      = iniFile->ReadString( "Monitoring", "MemorySound","BankAalarm.wav");
	FDiskWarningSound     = iniFile->ReadString( "Monitoring", "DiskSound","BankAalarm.wav");
	FTimeDiffWarningSound = iniFile->ReadString( "Monitoring", "TimeDiffSound","BankAalarm.wav");
	///< Show warning message on screen center.
	FCenter               = iniFile->ReadBool( "Warning Setting", "Center", false );
	///< CPU,Memory,Timediff alarm setting
	FOPTAlarm		      = iniFile->ReadBool( "Alarm Setting", "OPT", false );
	FFUTAlarm		      = iniFile->ReadBool( "Alarm Setting", "FUT", false );
	FTSEAlarm		      = iniFile->ReadBool( "Alarm Setting", "TSE", false );
	FOTCAlarm		      = iniFile->ReadBool( "Alarm Setting", "OTC", false );
	FCPUAlarm             = iniFile->ReadBool( "Alarm Setting", "CPU", false );
	FMEMAlarm             = iniFile->ReadBool( "Alarm Setting", "Memory", false );
	FDiskAlarm            = iniFile->ReadBool( "Alarm Setting", "Disk", false );
	FTimeDiffAlarm        = iniFile->ReadBool( "Alarm Setting", "TimeDiff", false );

	QLimitUpDown->Position   = iniFile->ReadInteger("Alarm Setting","QueueLimit",64 );
	CIDMinUpDown->Position   = iniFile->ReadInteger("Alarm Setting","CIDLimit", 100 );
	WarningCheckBox->Checked = iniFile->ReadBool( "Warning Setting", "Confirm", true );
	CloseConfirmCheckBox->Checked = iniFile->ReadBool( "Warning Setting", "CloseConfirm", true );
	ForceRMCheckBox->Checked = iniFile->ReadBool( "Warning Setting", "ForceUseRM", false );
	LogCheckBox->Checked = iniFile->ReadBool( "Warning Setting", "EventLog", false );
	ErrOnlyCheckBox->Checked = iniFile->ReadBool( "Warning Setting", "ErrorOnly", false );
	int index;

	if( (index = CrashComboBox->Items->IndexOf( FProcessErrSound )) != -1 )
		CrashComboBox->ItemIndex = index;
	else
		CrashComboBox->ItemIndex = 0;
	if( (index = BrokenComboBox->Items->IndexOf( FLineErrSound )) != -1 )
		BrokenComboBox->ItemIndex = index;
	else
		BrokenComboBox->ItemIndex = 0;
	if( (index = WarningComboBox->Items->IndexOf( FWarningSound )) != -1 )
		WarningComboBox->ItemIndex = index;
	else
		WarningComboBox->ItemIndex = 0;
	if( (index = NoTradeComboBox->Items->IndexOf( FNoTradeSound )) != -1 )
		NoTradeComboBox->ItemIndex = index;
	else
		NoTradeComboBox->ItemIndex = 0;
	if( (index = CPUWarningComboBox->Items->IndexOf( FCPUWarningSound )) != -1 )
		CPUWarningComboBox->ItemIndex = index;
	else
		CPUWarningComboBox->ItemIndex = 0;
	if( (index = MemoryWarningComboBox->Items->IndexOf( FMemWarningSound )) != -1 )
		MemoryWarningComboBox->ItemIndex = index;
	else
		MemoryWarningComboBox->ItemIndex = 0;
	if( (index = DiskWarningComboBox->Items->IndexOf( FDiskWarningSound )) != -1 )
		DiskWarningComboBox->ItemIndex = index;
	else
		DiskWarningComboBox->ItemIndex = 0;
	if( (index = TimeDiffWarningComboBox->Items->IndexOf( FTimeDiffWarningSound )) != -1 )
		TimeDiffWarningComboBox->ItemIndex = index;
	else
		TimeDiffWarningComboBox->ItemIndex = 0;
	FAlarms->Clear();
	AlarmListView->Clear();
	AnsiString Item,Str;
	int AlarmCount = iniFile->ReadInteger( "Alarms", "Count", 0 );

	for( int i=0; i < AlarmCount ; i++ )
	{
		Item = IntToStr( i );
		Str = iniFile->ReadString( "Alarms", Item, "err" );
		if( Str.AnsiCompare( "err" ) != 0 )
		{
			AlarmInfo* NewAlarm = new AlarmInfo( Str );
			FAlarms->Add( NewAlarm );
			AddAlarmItem( NewAlarm );
		}
	}
	Sources->Clear();
	Str  = iniFile->ReadString( "Source", "0", "Speedy-API" );
	Sources->Add( Str );
	Str  = iniFile->ReadString( "Source", "1", "FIX-Orc" );
	Sources->Add( Str );
	Str  = iniFile->ReadString( "Source", "2", "FIX-GLTRADE" );
	Sources->Add( Str );
	Str  = iniFile->ReadString( "Source", "3", "FIX-RTS" );
	Sources->Add( Str );
	Str  = iniFile->ReadString( "Source", "4", "FIX-RTS-QFII" );
	Sources->Add( Str );
	Str  = iniFile->ReadString( "Source", "5", "FIX-5" );
	Sources->Add( Str );
	Str  = iniFile->ReadString( "Source", "6", "FIX-6" );
	Sources->Add( Str );
	Str  = iniFile->ReadString( "Source", "7", "FIX-7" );
	Sources->Add( Str );
	Str  = iniFile->ReadString( "Source", "8", "FIX-8" );
	Sources->Add( Str );
	Str  = iniFile->ReadString( "Source", "9", "FIX-9" );
	Sources->Add( Str );
	for( int i= 10;;i++)
	{
		AnsiString Name( i );
		if( iniFile->ValueExists( "Source", Name ) == true )
		{
			Item.printf( "FIX-%d", i );
			Str  = iniFile->ReadString( "Source", Name, Item );
			Sources->Add( Str );
		}
		else
			break;
    }
	delete iniFile;

	FChangeToLanguage =  FCurrentLCID;

	if( FChangeToLanguage == ChineseTraditional )
		LanComboBox->ItemIndex = 0;
	else if( FChangeToLanguage == ChineseSimplified )
		LanComboBox->ItemIndex = 1;
	else
		LanComboBox->ItemIndex = 2;

    PageControl->ActivePageIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::OkBtnClick(TObject *Sender)
{
	TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");

	WriteIniString( iniFile, "FCM Setting", "FCMID", FFCM );
	WriteIniString( iniFile, "MBus Setting", "HOST", FMBusIP );
	WriteIniString( iniFile, "Warning Setting", "ProcessDown",CrashComboBox->Text );
	WriteIniString( iniFile, "Warning Setting", "LineBroken",BrokenComboBox->Text );
	WriteIniString( iniFile, "Warning Setting", "Warning",WarningComboBox->Text );
	WriteIniString( iniFile, "Warning Setting", "NoTrade", NoTradeComboBox->Text );
	WriteIniString( iniFile, "Monitoring", "CPUSound",CPUWarningComboBox->Text);
	WriteIniString( iniFile, "Monitoring", "MemorySound",MemoryWarningComboBox->Text);
	WriteIniString( iniFile, "Monitoring", "DiskSound",DiskWarningComboBox->Text);
	WriteIniString( iniFile, "Monitoring", "TimeDiffSound",TimeDiffWarningComboBox->Text);
	WriteIniBool( iniFile, "Warning Setting", "Center", PosRadioGroup->ItemIndex );
	WriteIniBool( iniFile, "Warning Setting", "Confirm", WarningCheckBox->Checked );
	WriteIniBool( iniFile, "Warning Setting", "ForceUseRM", ForceRMCheckBox->Checked );
	WriteIniBool( iniFile, "Warning Setting", "EventLog", LogCheckBox->Checked );
	WriteIniBool( iniFile, "Warning Setting", "CloseConfirm", CloseConfirmCheckBox->Checked );
	WriteIniBool( iniFile, "Warning Setting", "ErrorOnly", ErrOnlyCheckBox->Checked );
	WriteIniInteger( iniFile, "Alarm Setting","QueueLimit",QLimitUpDown->Position );
	WriteIniInteger( iniFile, "Alarm Setting","CIDLimit",CIDMinUpDown->Position );
	FProcessErrSound      = CrashComboBox->Text;
	FLineErrSound         = BrokenComboBox->Text;
	FWarningSound         = WarningComboBox->Text;
	FNoTradeSound         = NoTradeComboBox->Text;
	FCPUWarningSound      = CPUWarningComboBox->Text;
	FMemWarningSound      = MemoryWarningComboBox->Text;
	FDiskWarningSound     = DiskWarningComboBox->Text;
	FTimeDiffWarningSound = TimeDiffWarningComboBox->Text;
	FCenter               = PosRadioGroup->ItemIndex;

	AnsiString Item,Str;

	WriteIniInteger( iniFile, "Alarms", "Count", FAlarms->Count );
	for( int i = 0; i < FAlarms->Count ; i++)
	{
		Item = IntToStr( i );
		Str  = ((AlarmInfo*)FAlarms->Items[i])->ToString();
		WriteIniString( iniFile, "Alarms", Item, Str );
	}
	delete iniFile;

	if( LanComboBox->ItemIndex == 0 )
		FChangeToLanguage = ChineseTraditional;
	else if( LanComboBox->ItemIndex == 1 )
		FChangeToLanguage = ChineseSimplified;
	else
		FChangeToLanguage = EnglishUS;
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::BrokenSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundListBox->Directory + "//" + BrokenComboBox->Text;
	PlaySoundA( Sound.c_str(), NULL, SND_ASYNC|SND_FILENAME );
}
//---------------------------------------------------------------------------

void __fastcall TFormSetting::CrashSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundListBox->Directory + "//" + CrashComboBox->Text;
	PlaySoundA( Sound.c_str(), NULL, SND_ASYNC|SND_FILENAME );
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::WarningSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundListBox->Directory + "//" + WarningComboBox->Text;
	PlaySoundA( Sound.c_str(), NULL, SND_ASYNC|SND_FILENAME );
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::NoTradeSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundListBox->Directory + "//" + NoTradeComboBox->Text;
	PlaySoundA( Sound.c_str(), NULL, SND_ASYNC|SND_FILENAME );
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::CPUSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundListBox->Directory + "//" + CPUWarningComboBox->Text;
	PlaySoundA( Sound.c_str(), NULL, SND_ASYNC|SND_FILENAME );
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::MemorySoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundListBox->Directory + "//" + MemoryWarningComboBox->Text;
	PlaySoundA( Sound.c_str(), NULL, SND_ASYNC|SND_FILENAME );
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::DiskSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundListBox->Directory + "//" + DiskWarningComboBox->Text;
	PlaySoundA( Sound.c_str(), NULL, SND_ASYNC|SND_FILENAME );
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::TimeDiffSoundButtonClick(TObject *Sender)
{
	AnsiString Sound = FSoundListBox->Directory + "//" + TimeDiffWarningComboBox->Text;
	PlaySoundA( Sound.c_str(), NULL, SND_ASYNC|SND_FILENAME );
}
//---------------------------------------------------------------------------
bool __fastcall TFormSetting::Confirm( const AnsiString& Msg )
{
	if( WarningCheckBox->Checked == true )
	{
		if( MessageDlg( Msg, mtWarning, TMsgDlgButtons() << mbOK << mbCancel ,0) == mrCancel )
			return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall TFormSetting::CloseConfirm( const AnsiString& Msg )
{
	if( CloseConfirmCheckBox->Checked == true )
	{
		if( MessageDlg( Msg, mtConfirmation, TMsgDlgButtons() << mbOK << mbCancel, 0) == mrCancel )
			return false;
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::WriteIniInteger( TIniFile* iniFile, const String& Section,const String& Name, int Value )
{
	try
	{
		if( iniFile != NULL )
			iniFile->WriteInteger( Section, Name, Value );
	}
	catch( ... ){}
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::WriteIniString( TIniFile* iniFile, const String& Section,const String& Name, const String& Value )
{
	try
	{
		if( iniFile != NULL )
			iniFile->WriteString( Section, Name, Value );
	}
	catch( ... ){}
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::WriteIniBool( TIniFile* iniFile, const String& Section,const String& Name, bool Value )
{
	try
	{
		if( iniFile != NULL )
			iniFile->WriteBool( Section, Name, Value );
	}
	catch( ... ){}
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::WriteIniBool( const String& Section,const String& Name, bool Value )
{
	TIniFile* iniFile = NULL;
	try
	{
		iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");
		WriteIniBool( iniFile, Section, Name, Value );
		delete  iniFile;
		iniFile = NULL;
	}
	catch( ... )
	{
		if( iniFile != NULL )
			iniFile = NULL;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::SetAlarmOPT( bool Enable )
{
	if( FOPTAlarm != Enable )
	{
		FOPTAlarm = Enable;
		WriteIniBool( "Alarm Setting", "OPT", FOPTAlarm );
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::SetAlarmFUT( bool Enable )
{
	if( FFUTAlarm != Enable )
	{
		FFUTAlarm = Enable;
		WriteIniBool( "Alarm Setting", "FUT", FFUTAlarm );
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::SetAlarmTSE( bool Enable )
{
	if( FTSEAlarm != Enable )
	{
		FTSEAlarm = Enable;
		WriteIniBool( "Alarm Setting", "TSE", FTSEAlarm );
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::SetAlarmOTC( bool Enable )
{
	if( FOTCAlarm != Enable )
	{
		FOTCAlarm = Enable;
		WriteIniBool( "Alarm Setting", "OTC", FOTCAlarm );
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::SetAlarmCPU( bool Enable )
{
	if( FCPUAlarm != Enable )
	{
		FCPUAlarm = Enable;
		WriteIniBool( "Alarm Setting", "CPU", FCPUAlarm );
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::SetAlarmMemory( bool Enable )
{
	if( FMEMAlarm != Enable )
	{
		FMEMAlarm = Enable;
		WriteIniBool( "Alarm Setting", "Memory", FMEMAlarm );
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::SetAlarmDisk( bool Enable )
{
	if( FDiskAlarm != Enable )
	{
		FDiskAlarm = Enable;
		WriteIniBool( "Alarm Setting", "Dsik", FDiskAlarm );
	}
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::SetAlarmTimeDiff( bool Enable )
{
	if( FTimeDiffAlarm != Enable )
	{
		FTimeDiffAlarm = Enable;
		WriteIniBool( "Alarm Setting", "TimeDiff", FTimeDiffAlarm );
	}
}
//---------------------------------------------------------------------------
bool __fastcall TFormSetting::IsCheckingTime( int Market, int& Interval )
{
	AlarmInfo* CompareItem;
	int        TimeNow;
	SYSTEMTIME Time;

	GetLocalTime( &Time );
	TimeNow = Time.wHour*10000 + Time.wMinute*100 +	Time.wSecond;
	for( int i = 0; i < FAlarms->Count ; i++)
	{
		CompareItem = ( AlarmInfo* ) FAlarms->Items[i];
		if( CompareItem->Fmarket == Market && CompareItem->IsCheckingTime( TimeNow ))
		{
			Interval = CompareItem->FSec;
			return true;
		}
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TFormSetting::IsOPTCheckingTime( int& Interval )
{
	return IsCheckingTime( 0, Interval );
}
//---------------------------------------------------------------------------
bool __fastcall TFormSetting::IsFUTCheckingTime( int& Interval )
{
	return IsCheckingTime( 1, Interval );
}
//---------------------------------------------------------------------------
bool __fastcall TFormSetting::IsTSECheckingTime( int& Interval )
{
	return IsCheckingTime( 2, Interval );
}
//---------------------------------------------------------------------------
bool __fastcall TFormSetting::IsOTCCheckingTime( int& Interval )
{
	return IsCheckingTime( 3, Interval );
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::AddAlarmItem( AlarmInfo* NewAlarm )
{
	TListItem* NewListItem;

	NewListItem = AlarmListView->Items->Add();
	NewListItem->Caption = NewAlarm->GetMarket();
	NewListItem->SubItems->Add( NewAlarm->GetBeginTime() );
	NewListItem->SubItems->Add( NewAlarm->GetEndTime() );
	NewListItem->SubItems->Add( IntToStr( NewAlarm->FSec ) );
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::NewAlarmButtonClick(TObject *Sender)
{
	AlarmSettingForm = new TAlarmSettingForm( this, FAlarms );
	if( AlarmSettingForm->ShowModal() == mrOk )
	{
		AlarmInfo* NewAlarm = AlarmSettingForm->GetAlarmInfo();
		FAlarms->Add( NewAlarm );
		AddAlarmItem( NewAlarm );
	}
	delete AlarmSettingForm;
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::ModifyAlarmButtonClick(TObject *Sender)
{
	TListItem* ModifyItem = AlarmListView->Selected;
	if( ModifyItem != NULL )
	{
		AlarmInfo* ModifyAlarm = (AlarmInfo*)FAlarms->Items[ ModifyItem->Index ];
		AlarmSettingForm = new TAlarmSettingForm( this, FAlarms, ModifyAlarm );
		if( AlarmSettingForm->ShowModal() == mrOk )
		{
			AlarmSettingForm->UpdateAlarmInfo();
			ModifyItem->Caption = ModifyAlarm->GetMarket();
			ModifyItem->SubItems->Strings[0] = ModifyAlarm->GetBeginTime();
			ModifyItem->SubItems->Strings[1] = ModifyAlarm->GetEndTime();
			ModifyItem->SubItems->Strings[2] = IntToStr( ModifyAlarm->FSec );
		}
		delete AlarmSettingForm;
	}
	else
		ShowMessage( Scstrings_MAIN_ALARM_SEL_MODIFY_SETTING );
}
//---------------------------------------------------------------------------
void __fastcall TFormSetting::DeleteAlarmButtonClick(TObject *Sender)
{
	TListItem* DeleteItem = AlarmListView->Selected;
	if( DeleteItem != NULL )
	{
		if( MessageDlg( Scstrings_MAIN_ALARM_SEL_DELETE_SETTING_ASK, mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
		{
			FAlarms->Delete( DeleteItem->Index );
			AlarmListView->Items->Delete( DeleteItem->Index );
		}
	}
	else
		ShowMessage( Scstrings_MAIN_ALARM_SEL_DELETE_SETTING );
}
//---------------------------------------------------------------------------

