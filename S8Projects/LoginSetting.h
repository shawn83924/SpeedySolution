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
	TValueListEditor *SettingListEditor;
	TRoundFormEx *RoundFormEx;
	TButton *OKButton;
	TButton *CancelButton;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall SettingListEditorKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall SettingListEditorEditButtonClick(TObject *Sender);

private:	// User declarations
	void __fastcall InitSettingListEditor( void );
	void __fastcall LoadConfigSetting(const char* FileName);
	void __fastcall SaveConfigSetting(const char* FileName);
	String FMDPass; ///< Real MD_Pass value, the grid only shows a mask
public:		// User declarations
	__fastcall TLoginSettingForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoginSettingForm *LoginSettingForm;
//---------------------------------------------------------------------------
#endif
