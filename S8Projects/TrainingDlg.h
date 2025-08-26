//---------------------------------------------------------------------------

#ifndef TrainingDlgH
#define TrainingDlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "GraphButton.h"
#include "RoundFormEx.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.jpeg.hpp>
#include <Vcl.Imaging.pngimage.hpp>
//---------------------------------------------------------------------------
class TTrainingDlgForm : public TForm
{
__published:	// IDE-managed Components
	TImage *Image1;
	TGraphButton *CAButton;
	TLabel *Label1;
	TLabel *Label2;
	TRoundFormEx *RoundFormEx;
	void __fastcall CAButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TTrainingDlgForm(TComponent* Owner);
	static void __fastcall MessageDlg( const String& caption, const String& caption2 );
};
//---------------------------------------------------------------------------
extern PACKAGE TTrainingDlgForm *TrainingDlgForm;
//---------------------------------------------------------------------------
#endif
