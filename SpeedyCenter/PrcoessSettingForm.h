//---------------------------------------------------------------------------

#ifndef PrcoessSettingFormH
#define PrcoessSettingFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CSPIN.h"
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "cspin.h"
//---------------------------------------------------------------------------
class TProcessForm : public TForm
{
__published:	// IDE-managed Components
    TCSpinEdit *CStartHourSpinEdit;
    TCSpinEdit *CStartMinutesSpinEdit;
    TLabel *StartLabel;
    TLabel *StopLabel;
    TCSpinEdit *CStopHourSpinEdit;
    TCSpinEdit *CStopMinutesSpinEdit;
    TLabel *Label3;
    TLabel *ProcessLabel;
    TBitBtn *BitBtn1;
    TBitBtn *BitBtn2;
    TCheckBox *AutoCheckBox;
	TGroupBox *AutoStartGroupBox;
	TLabel *NextDayLabel;
	TGroupBox *WatchDogGroupBox;
	TLabel *CheckLabel;
	TLabel *RestartLabel;
	TCSpinEdit *WatchSecondsSpinEdit;
	TCSpinEdit *RestartTimesSpinEdit;
	TCheckBox *WatchDogCheckBox;
	TLabel *MinutesLabel;
	TLabel *TimesLabel;
        void __fastcall CStartHourSpinEditKeyPress(TObject *Sender,
          char &Key);
	void __fastcall AutoCheckBoxClick(TObject *Sender);
	void __fastcall CStartHourSpinEditChange(TObject *Sender);
private:	// User declarations
	void __fastcall EnableWatchDog( bool Enable );
	void __fastcall EnableAutoStartStop( bool Enable );
public:		// User declarations
	__fastcall TProcessForm(TComponent* Owner);
    __fastcall TProcessForm( AnsiString,AnsiString,AnsiString,bool );
    __fastcall TProcessForm( AnsiString,AnsiString,AnsiString,bool, bool WatchDogChecked, int WatchMin, int RestartTimes );
    AnsiString __fastcall GetStartTime( void );
    AnsiString __fastcall GetStopTime( void );
    bool       __fastcall GetAutoStart( void ) { return AutoCheckBox->Checked; }
};
//---------------------------------------------------------------------------
extern PACKAGE TProcessForm *ProcessForm;
//---------------------------------------------------------------------------
#endif
