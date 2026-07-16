//---------------------------------------------------------------------------

#ifndef LoginSettingH
#define LoginSettingH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ValEdit.hpp>
#include "RoundFormEx.h"
#include "TTaifexConnection.h"
//---------------------------------------------------------------------------
class TLoginSettingForm : public TForm
{
__published:	// IDE-managed Components
	TValueListEditor *ValueListEditor;
	TRoundFormEx *RoundFormEx;
	TButton *OKButton;
	TButton *CancelButton;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
private:	// User declarations
	void __fastcall LoadConfigSetting(const char* FileName);
	void __fastcall SaveConfigSetting(const char* FileName);
public:		// User declarations
	__fastcall TLoginSettingForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoginSettingForm *LoginSettingForm;
//---------------------------------------------------------------------------
#endif
