//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "APIForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cspin"
#pragma resource "*.dfm"
TAPISettingForm *APISettingForm;
//---------------------------------------------------------------------------
__fastcall TAPISettingForm::TAPISettingForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
