//---------------------------------------------------------------------------

#ifndef AskForUpdateFormH
#define AskForUpdateFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "GraphButton.h"
#include "GraphPanel.h"
#include "RoundFormEx.h"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TAskUpdateForm : public TForm
{
__published:	// IDE-managed Components
	TGraphPanel *GraphPanel;
	TLabel *MsgLabel;
	TMemo *NoteMemo;
	TLabel *VersionLabel;
	TLabel *UpdateLabel;
	TRoundFormEx *RoundFormEx;
	TButton *CancelButton;
	TButton *OKButton;
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall CancelButtonClick(TObject *Sender);
private:	// User declarations
	bool FForce;
    bool FReInstall;
public:		// User declarations
	__fastcall TAskUpdateForm(TComponent* Owner);
	static TModalResult __fastcall AskForUpdate();
};
//---------------------------------------------------------------------------
extern PACKAGE TAskUpdateForm *AskUpdateForm;
//---------------------------------------------------------------------------
#endif
