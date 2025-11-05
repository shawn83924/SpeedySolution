//---------------------------------------------------------------------------

#ifndef APIFormH
#define APIFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "cspin.h"
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TAPISettingForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TLabel *Label2;
	TCSpinEdit *SecSpinEdit;
	TLabel *Label3;
	TCSpinEdit *OrdLimitEdit;
	TLabel *Label4;
	TCSpinEdit *DayLimitEdit;
	TLabel *Label5;
	TBitBtn *OkBtn;
	TBitBtn *CancelBtn;
private:	// User declarations
public:		// User declarations
	__fastcall TAPISettingForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAPISettingForm *APISettingForm;
//---------------------------------------------------------------------------
#endif
