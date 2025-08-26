//---------------------------------------------------------------------------

#ifndef AgentFormH
#define AgentFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "RoundFormEx.h"
#include "RoundFormExRes.h"
#include "GraphButton.h"
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TAgentMain : public TForm
{
__published:	// IDE-managed Components
	TButton *OKButton;
	TMemo *Memo;
	TTimer *CheckProcessTimer;
	TTimer *UpdateTimer;
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall CheckProcessTimerTimer(TObject *Sender);
	void __fastcall UpdateTimerTimer(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TAgentMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAgentMain *AgentMain;
//---------------------------------------------------------------------------
#endif
