//---------------------------------------------------------------------------

#ifndef SystemInfoFormH
#define SystemInfoFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ImgList.hpp>
#include <System.ImageList.hpp>
//---------------------------------------------------------------------------
class TSystemForm : public TForm
{
__published:	// IDE-managed Components
	TListBox *InfoListBox;
	TImageList *StatImageList;
	void __fastcall InfoListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect,
          TOwnerDrawState State);
private:	// User declarations
public:		// User declarations
	__fastcall TSystemForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSystemForm *SystemForm;
//---------------------------------------------------------------------------
#endif
