//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "RoundFormExRes.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TRoundFormExRes *)
{
	new TRoundFormExRes(NULL);
}
//---------------------------------------------------------------------------
__fastcall TRoundFormExRes::TRoundFormExRes(TComponent* Owner)
:TComponent(Owner)
,FTransparent( false )
{
	///< Bitmaps for frame
	FTopBmp        = new Graphics::TBitmap();
	FLeftBmp       = new Graphics::TBitmap();
	FRightBmp      = new Graphics::TBitmap();
	FBottomBmp     = new Graphics::TBitmap();
	///< Bitmaps for system box
	FIconBmp       = new Graphics::TBitmap();
	FMinBtnBmp     = new Graphics::TBitmap();
	FMaxBtnBmp     = new Graphics::TBitmap();
	FRestoreBtnBmp = new Graphics::TBitmap();
	FCloseBtnBmp   = new Graphics::TBitmap();
	FLockBtnBmp    = new Graphics::TBitmap();
	FLockedBtnBmp  = new Graphics::TBitmap();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::Loaded( void )
{
	FCaptionHeight   = FTopBmp->Height / 2;
	FBottomHight     = FBottomBmp->Height / 2;
	FLeftFrameWidth  = FLeftBmp->Width / 2;
	FRightFrameWidth = FRightBmp->Width / 2;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetTopBmp(Graphics::TBitmap *Value)
{
	FTopBmp->Assign( Value );
	FTopBmp->Dormant();
	FCaptionHeight = FTopBmp->Height / 2;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetLeftBmp(Graphics::TBitmap *Value)
{
	FLeftBmp->Assign( Value );
	FLeftBmp->Dormant();
	FLeftFrameWidth = FLeftBmp->Width / 2;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetRightBmp(Graphics::TBitmap *Value)
{
	FRightBmp->Assign( Value );
	FRightBmp->Dormant();
	FRightFrameWidth = FRightBmp->Width / 2;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetBottomBmp(Graphics::TBitmap *Value)
{
	FBottomBmp->Assign( Value );
	FBottomBmp->Dormant();
	FBottomHight = FBottomBmp->Height / 2;
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetCloseBoxBmp(Graphics::TBitmap *Value)
{
    FCloseBtnBmp->Assign( Value );
	FCloseBtnBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetMinimizeBmp(Graphics::TBitmap *Value)
{
    FMinBtnBmp->Assign( Value );
    FMinBtnBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetMaximizeBmp(Graphics::TBitmap *Value)
{
    FMaxBtnBmp->Assign( Value );
    FMaxBtnBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetRestoreBmp(Graphics::TBitmap *Value)
{
    RestoreGraph->Assign( Value );
    RestoreGraph->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetLockBmp(Graphics::TBitmap *Value)
{
    FLockBtnBmp->Assign( Value );
    FLockBtnBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetLockedBmp(Graphics::TBitmap *Value)
{
    FLockedBtnBmp->Assign( Value );
    FLockedBtnBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetIconBmp(Graphics::TBitmap *Value)
{
    FIconBmp->Assign( Value );
    FIconBmp->Dormant();
}
//---------------------------------------------------------------------------
void __fastcall TRoundFormExRes::SetTransparent( bool Transparent )
{
	FTransparent = Transparent;
}
//---------------------------------------------------------------------------
namespace Roundformexres
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TRoundFormExRes)};
		RegisterComponents(L"MD", classes, 0);
	}
}
//---------------------------------------------------------------------------
