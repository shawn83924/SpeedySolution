//---------------------------------------------------------------------------

#ifndef RoomiH
#define RoomiH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "RoundFormEx.h"
//---------------------------------------------------------------------------
class TRoomiForm : public TForm
{
__published:	// IDE-managed Components
	TTimer *RedrawTimer;
	TRoundFormEx *RoundFormEx;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormPaint(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall RedrawTimerTimer(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	void __fastcall WndProc(Messages::TMessage &Message);
public:		// User declarations
	__fastcall TRoomiForm( TComponent* Owner);
    void __fastcall LoadFromClipboard( void );
	void __fastcall LoadProperties( void );
	void __fastcall SaveProperties( void );
};
//---------------------------------------------------------------------------
extern PACKAGE TRoomiForm *RoomiForm;
//---------------------------------------------------------------------------
#endif
