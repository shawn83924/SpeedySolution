//---------------------------------------------------------------------------

#ifndef ThroughputLimitH
#define ThroughputLimitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "cspin.h"
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include "..\Migo\MTree.h"
//---------------------------------------------------------------------------
class TThroughputForm : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *CancelBtn;
	TBitBtn *OkBtn;
	TListView *AccountListView;
	TBitBtn *DelBtn;
	TBitBtn *AddBtn;
	TEdit *NewItemEdit;
	TLabel *Label8;
	TLabel *Label26;
	TCSpinEdit *DefThroughputSpinEdit;
	TCSpinEdit *ThroughputSpinEdit;
	TLabel *Label1;
	TLabel *Label2;
	void __fastcall AccountListViewClick(TObject *Sender);
	void __fastcall AddBtnClick(TObject *Sender);
	void __fastcall DelBtnClick(TObject *Sender);
	void __fastcall ThroughputSpinEditChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TThroughputForm(TComponent* Owner);
	void Load( MTree *Tree );
	void Save( MTree *Tree );
};
//---------------------------------------------------------------------------
extern PACKAGE TThroughputForm *ThroughputForm;
//---------------------------------------------------------------------------
#endif
