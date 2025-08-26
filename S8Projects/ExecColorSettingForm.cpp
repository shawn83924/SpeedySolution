//---------------------------------------------------------------------------
#include <vcl.h>
#include "MainForm.h"
#include "config.h"
#pragma hdrstop
#include "ExecColorSettingForm.h"
#include "PMFNextStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TExecSettingForm *ExecSettingForm = NULL;
//---------------------------------------------------------------------------
__fastcall TExecSettingForm::TExecSettingForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TExecSettingForm::DrawGradientBar( TCanvas* canvas, TRect& dRect,TColor DownCol )
{
	int R = GetRValue( (DWORD)DownCol );
	int G = GetGValue( (DWORD)DownCol );
	int B = GetBValue( (DWORD)DownCol );
	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;

	vert [1] .x      = dRect.Right;
	vert [1] .y      = dRect.Bottom;
	vert [1] .Red    = R<<8;
	vert [1] .Green  = G<<8;
	vert [1] .Blue   = B<<8;
	vert [1] .Alpha  = 0x0000;

	R+=64;
	G+=64;
	B+=64;
	if( R >255 ) R = 255;
	if( G >255 ) G = 255;
	if( B >255 ) B = 255;
	vert [0] .x      = dRect.Left;
	vert [0] .y      = dRect.Top;
	vert [0] .Red    = R<<8;
	vert [0] .Green  = G<<8;
	vert [0] .Blue   = B<<8;
	vert [0] .Alpha  = 0x0000;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill( canvas->Handle, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V );
}
//---------------------------------------------------------------------------
void __fastcall TExecSettingForm::LoadColor( void )
{
	FNewBuyColor           = (TColor)g_Config.GetIntegerProperty("Setting","NewBuyColor", clBlack );
	FNewBuyBKColor 		   = (TColor)g_Config.GetIntegerProperty("Setting","NewBuyBkColor", 0x00D7D7FF );
	FNewSellColor          = (TColor)g_Config.GetIntegerProperty("Setting","NewSellColor", clBlack );
	FNewSellBKColor        = (TColor)g_Config.GetIntegerProperty("Setting","NewSellBkColor", 0x00F0FFF0 );
	FPartiallyBuyColor     = (TColor)g_Config.GetIntegerProperty("Setting","PartialBuyColor", clMaroon );
	FPartiallyBuyBKColor   = (TColor)g_Config.GetIntegerProperty("Setting","PartialBuyBkColor", 0x00FFA6FF );
	FPartiallySellColor    = (TColor)g_Config.GetIntegerProperty("Setting","PartialSellColor", 0x00004000 );
	FPartiallySellBKColor  = (TColor)g_Config.GetIntegerProperty("Setting","PartialSellBKColor", clMoneyGreen );
	FFillBuyColor          = (TColor)g_Config.GetIntegerProperty("Setting","FillBuyColor", clMaroon );
	FFillBuyBKColor        = (TColor)g_Config.GetIntegerProperty("Setting","FillBuyBkColor", 0x00FFA6FF );
	FFillSellColor         = (TColor)g_Config.GetIntegerProperty("Setting","FillSellColor", 0x00004000 );
	FFillSellBKColor       = (TColor)g_Config.GetIntegerProperty("Setting","FillSellBKColor", clMoneyGreen );
	FPendingColor          = (TColor)g_Config.GetIntegerProperty("Setting","PendingColor", clNavy );
	FPendingBKColor        = (TColor)g_Config.GetIntegerProperty("Setting","PendingBKColor", 0x0080FFFF );
	FRejectColor           = (TColor)g_Config.GetIntegerProperty("Setting","RejectColor", clWhite );
	FRejectBKColor         = (TColor)g_Config.GetIntegerProperty("Setting","RejectBKColor", 0x00211883 );
	FCancelColor           = (TColor)g_Config.GetIntegerProperty("Setting","CancelColor", clGray );
	FCancelBKColor         = (TColor)g_Config.GetIntegerProperty("Setting","CancelBkColor", 0x00CECECE );
}
//---------------------------------------------------------------------------
void __fastcall TExecSettingForm::SaveColor( void )
{
	g_Config.SetIntegerProperty("Setting","NewBuyColor", FNewBuyColor );
	g_Config.SetIntegerProperty("Setting","NewBuyBkColor", FNewBuyBKColor );
	g_Config.SetIntegerProperty("Setting","NewSellColor", FNewSellColor );
	g_Config.SetIntegerProperty("Setting","NewSellBkColor", FNewSellBKColor );
	g_Config.SetIntegerProperty("Setting","PartialBuyColor", FPartiallyBuyColor );
	g_Config.SetIntegerProperty("Setting","PartialBuyBkColor", FPartiallyBuyBKColor );
	g_Config.SetIntegerProperty("Setting","PartialSellColor", FPartiallySellColor );
	g_Config.SetIntegerProperty("Setting","PartialSellBKColor", FPartiallySellBKColor );
	g_Config.SetIntegerProperty("Setting","FillBuyColor", FFillBuyColor );
	g_Config.SetIntegerProperty("Setting","FillBuyBkColor", FFillBuyBKColor );
	g_Config.SetIntegerProperty("Setting","FillSellColor", FFillSellColor );
	g_Config.SetIntegerProperty("Setting","FillSellBKColor", FFillSellBKColor );
	g_Config.SetIntegerProperty("Setting","PendingColor", FPendingColor );
	g_Config.SetIntegerProperty("Setting","PendingBKColor", FPendingBKColor );
	g_Config.SetIntegerProperty("Setting","RejectColor", FRejectColor );
	g_Config.SetIntegerProperty("Setting","RejectBKColor", FRejectBKColor );
	g_Config.SetIntegerProperty("Setting","CancelColor", FCancelColor );
	g_Config.SetIntegerProperty("Setting","CancelBkColor", FCancelBKColor );
}
//---------------------------------------------------------------------------
void __fastcall TExecSettingForm::DefaultColor( void )
{
	FNewBuyColor           = clBlack;
	FNewBuyBKColor 		   = (TColor) 0x00D7D7FF;
	FNewSellColor          = clBlack;
	FNewSellBKColor        = (TColor)0x00F0FFF0;
	FPartiallyBuyColor     = clMaroon;
	FPartiallyBuyBKColor   = (TColor)0x00FFA6FF;
	FPartiallySellColor    = (TColor)0x00004000;
	FPartiallySellBKColor  = clMoneyGreen;
	FFillBuyColor          = clMaroon;
	FFillBuyBKColor        = (TColor)0x00FFA6FF;
	FFillSellColor         = (TColor)0x00004000;
	FFillSellBKColor       = clMoneyGreen;
	FPendingColor          = clNavy;
	FPendingBKColor        = (TColor)0x0080FFFF;
	FRejectColor           = clWhite;
	FRejectBKColor         = (TColor)0x00211883;
	FCancelColor           = clGray;
	FCancelBKColor         = (TColor)0x00CECECE;
}
//---------------------------------------------------------------------------
void __fastcall TExecSettingForm::DefaultColorButtonClick(TObject *Sender)
{
	DefaultColor();
	DrawGridClick( this );
	DrawGrid->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TExecSettingForm::DrawGridDrawCell(TObject *Sender, int ACol, int ARow,
		  TRect &Rect, TGridDrawState State)
{
	TColor BKColor;
	TColor FontColor;
	Vcl::Graphics::TTextFormat Formats;

	switch( ARow )
	{
		case 0:	FontColor = FNewBuyColor;
				BKColor = FNewBuyBKColor;break;
		case 1:	FontColor = FFillBuyColor;
				BKColor = FFillBuyBKColor;break;
		case 2:	FontColor = FPartiallyBuyColor;
				BKColor = FPartiallyBuyBKColor;break;
		case 3:	FontColor = FNewSellColor;
				BKColor = FNewSellBKColor;break;
		case 4:	FontColor = FFillSellColor;
				BKColor = FFillSellBKColor;break;
		case 5:	FontColor = FPartiallySellColor;
				BKColor = FPartiallySellBKColor;break;
		case 6:	FontColor = FPendingColor;
				BKColor = FPendingBKColor;break;
		case 7:	FontColor = FRejectColor;
				BKColor = FRejectBKColor;break;
		case 8:	FontColor = FCancelColor;
				BKColor = FCancelBKColor;break;
	}
	DrawGradientBar( DrawGrid->Canvas, Rect , BKColor );
	DrawGrid->Canvas->Brush->Style = bsClear;
	DrawGrid->Canvas->Font->Color  = FontColor;
	Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
	String OutString( FStatusStrings[ARow] );
	DrawGrid->Canvas->TextRect( Rect, OutString, Formats );
}
//---------------------------------------------------------------------------
void __fastcall TExecSettingForm::FormShow(TObject *Sender)
{
	FStatusStrings[0] = Pmfnextstrings_NEXT_EXECSET_STATE_NEW;
	FStatusStrings[1] = Pmfnextstrings_NEXT_EXECSET_STATE_FILL;
	FStatusStrings[2] = Pmfnextstrings_NEXT_EXECSET_STATE_FILL;
	FStatusStrings[3] = Pmfnextstrings_NEXT_EXECSET_STATE_NEW;
	FStatusStrings[4] = Pmfnextstrings_NEXT_EXECSET_STATE_FILL;
	FStatusStrings[5] = Pmfnextstrings_NEXT_EXECSET_STATE_FILL;
	FStatusStrings[6] = Pmfnextstrings_NEXT_EXECSET_STATE_SENDING;
	FStatusStrings[7] = Pmfnextstrings_NEXT_EXECSET_STATE_REJECTED;
	FStatusStrings[8] = Pmfnextstrings_NEXT_EXECSET_STATE_CANCELED;
	LoadColor( );
	DrawGridClick( this );
}
//---------------------------------------------------------------------------
void __fastcall TExecSettingForm::DrawGridClick(TObject *Sender)
{
	switch( DrawGrid->Selection.Top )
	{
		case 0:	FontColorBox->Selected = FNewBuyColor;
				BKColorBox->Selected = FNewBuyBKColor;break;
		case 1:	FontColorBox->Selected = FFillBuyColor;
				BKColorBox->Selected = FFillBuyBKColor;break;
		case 2:	FontColorBox->Selected = FPartiallyBuyColor;
				BKColorBox->Selected = FPartiallyBuyBKColor;break;
		case 3:	FontColorBox->Selected = FNewSellColor;
				BKColorBox->Selected = FNewSellBKColor;break;
		case 4:	FontColorBox->Selected = FFillSellColor;
				BKColorBox->Selected = FFillSellBKColor;break;
		case 5:	FontColorBox->Selected = FPartiallySellColor;
				BKColorBox->Selected = FPartiallySellBKColor;break;
		case 6:	FontColorBox->Selected = FPendingColor;
				BKColorBox->Selected = FPendingBKColor;break;
		case 7:	FontColorBox->Selected = FRejectColor;
				BKColorBox->Selected = FRejectBKColor;break;
		case 8:	FontColorBox->Selected = FCancelColor;
				BKColorBox->Selected = FCancelBKColor;break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TExecSettingForm::BKColorBoxChange(TObject *Sender)
{
	switch( DrawGrid->Selection.Top )
	{
		case 0:	FNewBuyColor = FontColorBox->Selected;
				FNewBuyBKColor = BKColorBox->Selected;break;
		case 1:	FFillBuyColor = FontColorBox->Selected;
				FFillBuyBKColor = BKColorBox->Selected;break;
		case 2:	FPartiallyBuyColor = FontColorBox->Selected;
				FPartiallyBuyBKColor = BKColorBox->Selected;break;
		case 3:	FNewSellColor = FontColorBox->Selected;
				FNewSellBKColor = BKColorBox->Selected;break;
		case 4:	FFillSellColor = FontColorBox->Selected;
				FFillSellBKColor = BKColorBox->Selected;break;
		case 5:	FPartiallySellColor = FontColorBox->Selected;
				FPartiallySellBKColor = BKColorBox->Selected;break;
		case 6:	FPendingColor = FontColorBox->Selected;
				FPendingBKColor = BKColorBox->Selected;break;
		case 7:	FRejectColor = FontColorBox->Selected;
				FRejectBKColor = BKColorBox->Selected;break;
		case 8:	FCancelColor = FontColorBox->Selected;
				FCancelBKColor = BKColorBox->Selected;break;
	}
	DrawGrid->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TExecSettingForm::OptNewOrderButtonClick(TObject *Sender)
{
	SaveColor( );
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------


