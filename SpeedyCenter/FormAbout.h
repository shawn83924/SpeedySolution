//---------------------------------------------------------------------------

#ifndef FormAboutH
#define FormAboutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
#define PROCESS_NAME "SpeedyCenter.exe"
//---------------------------------------------------------------------------
class TAboutForm : public TForm
{
__published:	// IDE-managed Components
	TImage *SpeedyLogoImage;
	TLabel *CopyRightLabel;
	TButton *OKButton;
	TBevel *Bevel;
    TLabel *VerLabel;
	TLabel *VersionLabel;
	TLabel *BuildDateLabel;
    TLabel *DateLabel;
private:	// User declarations
public:		// User declarations
    __fastcall TAboutForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAboutForm *AboutForm;
//---------------------------------------------------------------------------
#endif
