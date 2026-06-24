//---------------------------------------------------------------------------

#ifndef CASettingH
#define CASettingH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "RoundFormEx.h"
#include <Vcl.Dialogs.hpp>
#include "GraphButton.h"
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.FileCtrl.hpp>
#include "UnifyDlg.h"
//---------------------------------------------------------------------------
class TCASettingForm : public TForm
{
__published:	// IDE-managed Components
	TEdit *CAPasswordEdit;
	TEdit *CAFileEdit;
	TLabel *CAFileLabel;
	TLabel *CAPasswordLabel;
	TRoundFormEx *RoundFormEx;
	TButton *CheckCAButton;
	TLabel *IDLabel;
	TLabel *SNLabel;
	TLabel *ResultLabel;
	TGraphButton *QuiteButton;
	TSpeedButton *BrowseButton;
	TCheckBox *SavePasswordCheckBox;
	TPageControl *PageControl;
	TTabSheet *CATabSheet;
	TTabSheet *FileTabSheet;
	TDirectoryListBox *DirectoryListBox;
	TFileListBox *FileListBox;
	TEdit *SelectFileEdit;
	TLabel *Label1;
	TButton *OKButton;
	TButton *CancelButton;
	void __fastcall BrowseButtonClick(TObject *Sender);
	void __fastcall CheckCAButtonClick(TObject *Sender);
	void __fastcall QuiteButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
	bool FCloseMainForm;
public:		// User declarations
	__fastcall TCASettingForm(TComponent* Owner);
	__property bool CloseMainForm = { read = FCloseMainForm };
};
//---------------------------------------------------------------------------
extern PACKAGE TCASettingForm *CASettingForm;
//---------------------------------------------------------------------------
#endif
