//---------------------------------------------------------------------------

#ifndef AccountInputH
#define AccountInputH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include "Publisher.h"
//---------------------------------------------------------------------------
class TAccountInputForm : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TEdit *AccountEdit;
    TEdit *AccountFlagEdit;
    TLabel *Label2;
    TBitBtn *BitBtn1;
    TBitBtn *BitBtn2;
    void __fastcall BitBtn1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TAccountInputForm(TComponent* Owner, bool InputFlag = true);
};
//---------------------------------------------------------------------------
extern PACKAGE TAccountInputForm *AccountInputForm;
//---------------------------------------------------------------------------
#endif
