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
#include "SimTFXMain.h"
//---------------------------------------------------------------------------
class TUsersList : public TForm
{
__published:	// IDE-managed Components
	TListView *UsersListView;
	TPanel *BtnPanel;
	TBitBtn *OKBtn;
	TBitBtn *CancelBtn;
private:	// User declarations
	void __fastcall ReadUsers(TSimTFXForm* SimTFXForm);
	void __fastcall SetUsersList( const AnsiString& users );
public:		// User declarations
	__fastcall TUsersList(TComponent* Owner, TSimTFXForm* SimTFXForm, const AnsiString& users);
	__fastcall ~TUsersList();
    AnsiString __fastcall GetUsers(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUsersList *UsersList;
//---------------------------------------------------------------------------
#endif
