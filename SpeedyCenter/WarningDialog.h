//---------------------------------------------------------------------------

#ifndef WarningDialogH
#define WarningDialogH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "RoundForm.h"
//---------------------------------------------------------------------------
class TWarningForm : public TForm
{
__published:	// IDE-managed Components
    TImage *Image;
    TTimer *HideTimer;
    TLabel *MsgLabel;
    TRoundForm *RoundForm;
	TLabel *HostLabel;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall HideTimerTimer(TObject *Sender);
	void __fastcall ImageClick(TObject *Sender);
private:	// User declarations
	int FFadeCountDown;
	AnsiString FLogFileName;
public:		// User declarations
	__fastcall TWarningForm(TComponent* Owner);
	void __fastcall ShowWarning( const String& Host,const String& Message, const String& File );
	void __fastcall LogError( const AnsiString& Host,const AnsiString& IP,const AnsiString& ECode,const AnsiString& Message );
	void __fastcall LogError( const AnsiString& Host,const AnsiString& IP,const AnsiString& ECode,const String& Message );
};
//---------------------------------------------------------------------------
extern PACKAGE TWarningForm *WarningForm;
//---------------------------------------------------------------------------
#endif
