//---------------------------------------------------------------------------

#ifndef AlarmFormH
#define AlarmFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cspin.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class AlarmInfo : public TObject
{
public:
	int FBeginTime;
	int FEndTime;
	int Fmarket;
	int FSec;
public:
	__fastcall AlarmInfo() {}
	__fastcall AlarmInfo( const AnsiString& Str );
	AnsiString __fastcall toString( );
	AnsiString __fastcall GetMarket();
	AnsiString __fastcall GetBeginTime();
	AnsiString __fastcall GetEndTime( );
	bool __fastcall IsOverlap( int Market, int Start, int End );
	bool __fastcall IsCheckingTime( int Time );
};
//---------------------------------------------------------------------------
class TAlarmSettingForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *StartLabel;
	TLabel *StopLabel;
	TLabel *Label3;
	TCSpinEdit *CStartHourSpinEdit;
	TCSpinEdit *CStartMinutesSpinEdit;
	TCSpinEdit *CStopHourSpinEdit;
	TCSpinEdit *CStopMinutesSpinEdit;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
	TComboBox *MarketComboBox;
	TCSpinEdit *CStartSecSpinEdit;
	TCSpinEdit *CStopSecSpinEdit;
	TComboBox *TimerComboBox;
	TLabel *Label1;
	TBevel *Bevel1;
	void __fastcall BitBtn1Click(TObject *Sender);
private:	// User declarations
	AlarmInfo* FMofify;
	TObjectList* FAlarms;
public:		// User declarations
	__fastcall TAlarmSettingForm(TComponent* Owner, TObjectList* );
	__fastcall TAlarmSettingForm(TComponent* Owner, TObjectList* , AlarmInfo* Mofify );
	AlarmInfo* __fastcall  GetAlarmInfo( void );
	void __fastcall  UpdateAlarmInfo( void );
};
//---------------------------------------------------------------------------
extern PACKAGE TAlarmSettingForm *AlarmSettingForm;
//---------------------------------------------------------------------------
#endif
