//---------------------------------------------------------------------------
#ifndef UsersFormH
#define UsersFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TUsersList : public TForm
{
__published:	// IDE-managed Components
	TListView *UsersListView;
	TPanel *BtnPanel;
	TBitBtn *OKBtn;
	TBitBtn *CancelBtn;
private:	// User declarations
	///< Purely mechanical: Users->Strings[i] is the caption to show,
	///< Users->Objects[i] (non-NULL == checked) is whether it starts checked.
	///< Caller is responsible for deciding which users to list and which
	///< are pre-selected; this form has no knowledge of ini files/accounts.
	void __fastcall PopulateUsersList( TStringList* Users );
public:		// User declarations
	__fastcall TUsersList(TComponent* Owner, TStringList* Users);
	__fastcall ~TUsersList();
    AnsiString __fastcall GetUsers(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUsersList *UsersList;
//---------------------------------------------------------------------------
#endif
