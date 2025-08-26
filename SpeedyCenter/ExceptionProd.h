//---------------------------------------------------------------------------

#ifndef ExceptionProdH
#define ExceptionProdH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TExceptionProdForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *UserNameLabel;
	TLabel *Label1;
	TLabel *Label2;
	TBitBtn *OkBtn;
	TBitBtn *CancelBtn;
	TComboBox *ChComboBox;
	TComboBox *ExComboBox;
	TComboBox *MarketComboBox;
	TLabel *Label3;
	TEdit *ProdIDEdit;
	TCheckBox *ApplyAllChCheckBox;
private:	// User declarations
public:		// User declarations
	__fastcall TExceptionProdForm(TComponent* Owner);
	void Init( TStringList* Exchanges, TStringList* Channels );
	void Init( TStringList* Exchanges, TStringList* Channels, TStrings* Settings );
};
//---------------------------------------------------------------------------
extern PACKAGE TExceptionProdForm *ExceptionProdForm;
//---------------------------------------------------------------------------
#endif
