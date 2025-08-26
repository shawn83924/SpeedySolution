//---------------------------------------------------------------------------

#ifndef NewUserH
#define NewUserH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ComCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <Mask.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TNewUserForm : public TForm
{
__published:	// IDE-managed Components
    TLabel *Label1;
    TMaskEdit *PwdEdit;
    TLabel *Label2;
    TLabel *Label3;
    TMaskEdit *PwdConfirmEdit;
    TEdit *IDEdit;
    TLabel *Label4;
    TComboBox *GroupComboBox;
	TBitBtn *OKBtn;
	TBitBtn *CancelBtn;
    TLabel *Label5;
    TComboBox *StateComboBox;
    TPanel *IDPanel;
    TPanel *PwdPanel;
    TPanel *AttrPanel;
    TPanel *BtnPanel;
    void __fastcall OKBtnClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	bool CheckPassword;
	bool __fastcall EasyPassword( AnsiString pwd );
    bool __fastcall CharDup( AnsiString pwd );
    bool __fastcall NumAndEn( AnsiString pwd );
public:		// User declarations
	__fastcall TNewUserForm();
	__fastcall TNewUserForm( const AnsiString& ID );
    __fastcall TNewUserForm( TListItem* EditItem, bool ModifyAttr = true );
};
//---------------------------------------------------------------------------
extern PACKAGE TNewUserForm *NewUserForm;
//---------------------------------------------------------------------------
#endif
