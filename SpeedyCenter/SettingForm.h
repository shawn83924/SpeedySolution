//---------------------------------------------------------------------------

#ifndef SettingFormH
#define SettingFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <FileCtrl.hpp>
#include <IniFiles.hpp>
#include <Mmsystem.h>
class AlarmInfo;
//---------------------------------------------------------------------------
const unsigned int ChineseTraditional = ( SUBLANG_CHINESE_TRADITIONAL << 10 ) | LANG_CHINESE;/// TAIWAN
const unsigned int ChineseSimplified  = ( SUBLANG_CHINESE_SIMPLIFIED << 10  ) | LANG_CHINESE; /// China
const unsigned int EnglishUS          = ( SUBLANG_ENGLISH_US << 10  )         | LANG_ENGLISH;
//---------------------------------------------------------------------------
class TFormSetting : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *OkBtn;
	TBitBtn *CancelBtn;
	TPageControl *PageControl;
	TTabSheet *BusTabSheet;
	TTabSheet *WarningTabSheet;
	TRadioGroup *PosRadioGroup;
	TLabel *BrokenLabel;
	TLabel *ProcessCrashLabel;
	TLabel *WarningLabel;
	TComboBox *BrokenComboBox;
	TComboBox *CrashComboBox;
	TComboBox *WarningComboBox;
	TSpeedButton *BrokenSoundButton;
	TSpeedButton *CrashSoundButton;
	TSpeedButton *WarningSoundButton;
	TTabSheet *TradeMonTabSheet;
	TLabel *Label1;
	TComboBox *NoTradeComboBox;
	TSpeedButton *NoTradeSoundButton;
	TListView *AlarmListView;
	TSpeedButton *NewAlarmButton;
	TSpeedButton *ModifyAlarmButton;
	TSpeedButton *DeleteAlarmButton;
	TLabel *Label2;
	TComboBox *CPUWarningComboBox;
	TSpeedButton *CPUSoundButton;
	TLabel *Label3;
	TComboBox *MemoryWarningComboBox;
	TSpeedButton *MemorySoundButton;
	TLabel *Label4;
	TComboBox *TimeDiffWarningComboBox;
	TSpeedButton *TimeDiffSoundButton;
	TLabel *Label5;
	TUpDown *QLimitUpDown;
	TEdit *QLimitEdit;
	TCheckBox *WarningCheckBox;
	TComboBox *LanComboBox;
	TLabel *Label6;
	TLabel *Label7;
	TComboBox *DiskWarningComboBox;
	TSpeedButton *DiskSoundButton;
	TCheckBox *ErrOnlyCheckBox;
	TLabel *Label8;
	TEdit *CIDMinEdit;
	TUpDown *CIDMinUpDown;
	TCheckBox *CloseConfirmCheckBox;
	TCheckBox *ForceRMCheckBox;
	TCheckBox *LogCheckBox;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall OkBtnClick(TObject *Sender);
	void __fastcall BrokenSoundButtonClick(TObject *Sender);
	void __fastcall CrashSoundButtonClick(TObject *Sender);
	void __fastcall WarningSoundButtonClick(TObject *Sender);
	void __fastcall NoTradeSoundButtonClick(TObject *Sender);
	void __fastcall NewAlarmButtonClick(TObject *Sender);
	void __fastcall ModifyAlarmButtonClick(TObject *Sender);
	void __fastcall DeleteAlarmButtonClick(TObject *Sender);
	void __fastcall CPUSoundButtonClick(TObject *Sender);
	void __fastcall MemorySoundButtonClick(TObject *Sender);
	void __fastcall TimeDiffSoundButtonClick(TObject *Sender);
	void __fastcall DiskSoundButtonClick(TObject *Sender);
private:	// User declarations
		TFileListBox *FSoundListBox;
		AnsiString FFCM;
		AnsiString FMBusIP;
		AnsiString FProcessErrSound;
		AnsiString FLineErrSound;
		AnsiString FWarningSound;
		AnsiString FNoTradeSound;
		AnsiString FCPUWarningSound;
		AnsiString FMemWarningSound;
		AnsiString FDiskWarningSound;
		AnsiString FTimeDiffWarningSound;
		bool       FCenter;
		bool       FOPTAlarm;
		bool       FFUTAlarm;
		bool       FTSEAlarm;
		bool       FOTCAlarm;
		bool       FCPUAlarm;
		bool       FMEMAlarm;
		bool       FDiskAlarm;
		bool       FTimeDiffAlarm;
		TObjectList* FAlarms;
		LCID       FChangeToLanguage;
		void __fastcall AddAlarmItem( AlarmInfo* NewAlarm );
		bool __fastcall IsCheckingTime( int Market, int& Interval );
public:		// User declarations
		__fastcall TFormSetting(TComponent* Owner);
		TStringList* Sources;
		LCID       __fastcall GetLanguage(){ return FChangeToLanguage; }
		AnsiString __fastcall GetMBusIP(){ return FMBusIP; }
		AnsiString __fastcall GetFCMID(){ return FFCM; }
		AnsiString __fastcall GetProcessErrorSound();
		AnsiString __fastcall GetLineErrorSound();
		AnsiString __fastcall GetWarningSound();
		AnsiString __fastcall GetNoTradeSound();
		AnsiString __fastcall GetCPUWarningSound();
		AnsiString __fastcall GetMemWarningSound();
		AnsiString __fastcall GetDiskWarningSound();
		AnsiString __fastcall GetTimeDiffWarningSound();
		void __fastcall WriteIniBool( const String& Section,const String& Name, bool Value );
		void __fastcall WriteIniBool( TIniFile* iniFile, const String& Section,const String& Name, bool Value );
		void __fastcall WriteIniString( TIniFile* iniFile, const String& Section,const String& Name, const String& Value );
		void __fastcall WriteIniInteger( TIniFile* iniFile, const String& Section,const String& Name, int Value );
		int        __fastcall GetQueueLimit();
		int        __fastcall GetCIDLimit();
		bool       __fastcall IsCenter(){ return FCenter; }
		bool       __fastcall ForceRM(){ return ForceRMCheckBox->Checked; }
        bool       __fastcall EventLog(){ return LogCheckBox->Checked; }
		bool       __fastcall Confirm( const AnsiString& Msg );
		bool       __fastcall CloseConfirm( const AnsiString& Msg );
		bool       __fastcall AlarmOPT() { return FOPTAlarm; }
		bool       __fastcall AlarmFUT() { return FFUTAlarm; }
		bool       __fastcall AlarmTSE() { return FTSEAlarm; }
		bool       __fastcall AlarmOTC() { return FOTCAlarm; }
		bool       __fastcall AlarmCPU() { return FCPUAlarm; }
		bool       __fastcall AlarmDisk() { return FDiskAlarm; }
		bool       __fastcall AlarmMemory() { return FMEMAlarm; }
		bool       __fastcall AlarmTimeDiff() { return FTimeDiffAlarm; }
		bool       __fastcall OnlyErrorAlarmTimeDiff() { return ErrOnlyCheckBox->Checked; }

		void       __fastcall SetAlarmOPT( bool Enable );
		void       __fastcall SetAlarmFUT( bool Enable );
		void       __fastcall SetAlarmTSE( bool Enable );
		void       __fastcall SetAlarmOTC( bool Enable );
		void       __fastcall SetAlarmCPU( bool Enable );
		void       __fastcall SetAlarmMemory( bool Enable );
		void       __fastcall SetAlarmTimeDiff( bool Enable );
		void       __fastcall SetAlarmDisk( bool Enable );

		bool       __fastcall IsOPTCheckingTime( int& Interval );
		bool       __fastcall IsFUTCheckingTime( int& Interval );
		bool       __fastcall IsTSECheckingTime( int& Interval );
		bool       __fastcall IsOTCCheckingTime( int& Interval );
};
//---------------------------------------------------------------------------
extern PACKAGE TFormSetting *FormSetting;
//---------------------------------------------------------------------------
#endif
