//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "GraphToggle.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TGraphToggle *)
{
	new TGraphToggle(NULL);
}
//---------------------------------------------------------------------------
__fastcall TGraphToggle::TGraphToggle(TComponent* Owner)
	: TToggleSwitch(Owner)
{
	FOffPicture = new TPicture();
	FOnPicture = new TPicture();
	FOffPicture->OnChange = PictureChanged;
	FOnPicture->OnChange = PictureChanged;
	AutoSize = false;
}
//---------------------------------------------------------------------------
__fastcall TGraphToggle::~TGraphToggle(void)
{
	delete FOffPicture;
	delete FOnPicture;
}
//---------------------------------------------------------------------------
void __fastcall TGraphToggle::Paint(void)
{
    TPicture* picture;

	if(State == tssOff)
		picture = FOffPicture;
	else
		picture = FOnPicture;

	if( picture == NULL || picture->Graphic == NULL || picture->Graphic->Empty )
	{
		TToggleSwitch::Paint();
		return;
	}

	Canvas->StretchDraw( ClientRect, picture->Graphic );
}
//---------------------------------------------------------------------------
void __fastcall TGraphToggle::PictureChanged(TObject* )
{
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TGraphToggle::SetOffPicture(TPicture* picture)
{
	if(picture == NULL)
		FOffPicture->Assign(NULL);
	else
		FOffPicture->Assign(picture);
}
//---------------------------------------------------------------------------
void __fastcall TGraphToggle::SetOnPicture(TPicture* picture)
{
    if(picture == NULL)
		FOnPicture->Assign(NULL);
	else
		FOnPicture->Assign(picture);
}
//---------------------------------------------------------------------------
namespace Graphtoggle
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TGraphToggle)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
