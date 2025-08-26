//---------------------------------------------------------------------------

#ifndef SelChannelFormH
#define SelChannelFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Buttons.hpp>
//---------------------------------------------------------------------------
class TChannelForm : public TForm
{
__published:	// IDE-managed Components
	TBitBtn *OkBtn;
	TBitBtn *CancelBtn;
	TLabel *UserNameLabel;
	TLabel *Label1;
	TLabel *Label2;
	TComboBox *ChComboBox;
	TLabel *ExLabel;
	TLabel *MktLabel;
private:	// User declarations
public:		// User declarations
	__fastcall TChannelForm(TComponent* Owner);
	void __fastcall Init( const String& Ex, const String& Mkt,  const String& ch,TStringList* chs);
};
//---------------------------------------------------------------------------
extern PACKAGE TChannelForm *ChannelForm;
//---------------------------------------------------------------------------
#endif
