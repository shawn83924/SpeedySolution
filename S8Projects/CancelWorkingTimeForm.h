//---------------------------------------------------------------------------

#ifndef CancelWorkingTimeFormH
#define CancelWorkingTimeFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "RoundFormEx.h"
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TCancelWorkingForm : public TForm
{
__published:	// IDE-managed Components
	TButton *OKButton;
	TButton *CancelButton;
	TRoundFormEx *RoundFormEx;
	TEdit *HourEdit;
	TUpDown *HUpDown;
	TEdit *MinutesEdit;
	TUpDown *MinutesUpDown;
	TEdit *SecEdit;
	TUpDown *SecUpDown;
	TLabel *Label15;
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
	bool   FEnableTimer;
	int    FCancelTime;
	String FTimeString;
public:		// User declarations
	__fastcall TCancelWorkingForm(TComponent* Owner);
	void __fastcall SetEnable( bool OnOff );
	bool __fastcall GetEnable( void );
	void __fastcall SetTime( int hhmmss );
	int __fastcall GetTime( void );
	const String& __fastcall GetTimeString( void );
};
//---------------------------------------------------------------------------
extern PACKAGE TCancelWorkingForm *CancelWorkingForm;
//---------------------------------------------------------------------------
#endif
