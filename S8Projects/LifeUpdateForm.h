//---------------------------------------------------------------------------

#ifndef LifeUpdateFormH
#define LifeUpdateFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RoundFormEx.h"
#include "GraphButton.h"
#include "GraphPanel.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TUpdateForm : public TForm
{
__published:	// IDE-managed Components
	TTimer *UpdateTimer;
	TTimer *ProgressTimer;
	TGraphPanel *GraphPanel;
	TLabel *CurrentFileLabel;
	TLabel *CurrentProgressLabel;
	TLabel *CurrentLabel;
	TLabel *TotalLabel;
	TProgressBar *CurrentProgressBar;
	TProgressBar *TotalProgressBar;
	TRoundFormEx *RoundFormEx;
	TButton *CancelButton;
	void __fastcall CancelButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall UpdateTimerTimer(TObject *Sender);
	void __fastcall ProgressTimerTimer(TObject *Sender);
private:	// User declarations
	int    FUpdateItemCount;
	String FUpdateOKMsg;
	String FUpdateFailMsg;
	String FUpdateFileMsg;
public:		// User declarations
	__fastcall TUpdateForm(TComponent* Owner);
	int __fastcall GetUpdateCount( void );
	bool  __fastcall  UpdateMainProgram( void );
};
//---------------------------------------------------------------------------
extern PACKAGE TUpdateForm *UpdateForm;
//---------------------------------------------------------------------------
#endif
