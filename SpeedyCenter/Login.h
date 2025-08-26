//---------------------------------------------------------------------------

#ifndef LoginH
#define LoginH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Mask.hpp>
#include <ExtCtrls.hpp>
#include "Subscriber.h"
#include "IniEditBox.h"
//---------------------------------------------------------------------------
class TLoginForm : public TForm
{
__published:	// IDE-managed Components
    TEdit *IDEdit;
	TLabel *PasswordLabel;
	TLabel *UserNameLabel;
    TMaskEdit *PwdEdit;
	TBitBtn *OkBtn;
	TBitBtn *CancelBtn;
	TTimer *AutoLogonTimer;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OkBtnClick(TObject *Sender);
	void __fastcall AutoLogonTimerTimer(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TLoginForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoginForm *LoginForm;
//---------------------------------------------------------------------------
#endif
