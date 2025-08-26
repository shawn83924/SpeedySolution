//---------------------------------------------------------------------------

#ifndef VersionUnitH
#define VersionUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "cspin.h"
#include <Buttons.hpp>
//---------------------------------------------------------------------------
class TVersionForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TCSpinEdit *MajorSpin;
	TCSpinEdit *MinorSpin;
	TCSpinEdit *BuildSpin;
	TBitBtn *BitBtn1;
	TBitBtn *BitBtn2;
private:	// User declarations
public:		// User declarations
	__fastcall TVersionForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TVersionForm *VersionForm;
//---------------------------------------------------------------------------
#endif
