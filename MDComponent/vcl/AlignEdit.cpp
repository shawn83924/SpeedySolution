//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "AlignEdit.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TAlignEdit *)
{
	new TAlignEdit(NULL);
}
//---------------------------------------------------------------------------
__fastcall TAlignEdit::TAlignEdit(TComponent* Owner)
	: TCustomControl(Owner)
{
	ControlStyle = ControlStyle << csAcceptsControls;
	Width = 121;
	Height = 21;
	TabStop = true;

	FAlignment = taLeftJustify;
	FVertAlign = vaTop;
	FEditorHeight = 21;
	FTextOffsetX = 0;
	FTextOffsetY = 0;
	FImageWidth = 21;
	FImageHeight = 21;
	FImagePosX = 0;
	FImagePosY = 0;
	FEditorClientColor = clWindow;
	FOnChange = NULL;

	StyleElements = TStyleElements();
	ParentColor = false;
	Color = FEditorClientColor;

	FEditor = new TEdit(this);
	FEditor->Parent = this;
	FEditor->BorderStyle = bsNone;
	FEditor->Align = alNone;
	FEditor->TabStop = false;
	FEditor->Text = L"";
	FEditor->Alignment = FAlignment;
	FEditor->AutoSize = true;
	FEditor->StyleElements = StyleElements;
	FEditor->ParentFont = true;
	FEditor->ParentColor = false;
	FEditor->Color = FEditorClientColor;
	FEditor->OnClick = EditorClick;
	FEditor->OnChange = EditorChange;

	FImagePicture = new TPicture();
	FImagePicture->OnChange = ImageChanged;
}
//---------------------------------------------------------------------------
__fastcall TAlignEdit::~TAlignEdit(void)
{
	if (FImagePicture != NULL)
	{
		delete FImagePicture;
		FImagePicture = NULL;
	}

	FEditor = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::CreateWnd(void)
{
	TCustomControl::CreateWnd();
	if (FEditor != NULL)
		FEditor->Color = FEditorClientColor;
	UpdateEditorBounds();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::Resize(void)
{
	TCustomControl::Resize();
	UpdateEditorBounds();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::MouseDown(TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button == mbLeft && FEditor != NULL && FEditor->CanFocus())
		FEditor->SetFocus();

	TCustomControl::MouseDown(Button, Shift, X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::Paint(void)
{
	Canvas->Brush->Color = FEditorClientColor;
	Canvas->FillRect(ClientRect);

	if (FImagePicture != NULL && FImagePicture->Graphic != NULL && !FImagePicture->Graphic->Empty)
	{
		TRect imageRect(FImagePosX, FImagePosY, FImagePosX + FImageWidth, FImagePosY + FImageHeight);
		Canvas->StretchDraw(imageRect, FImagePicture->Graphic);
	}
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::UpdateEditorBounds(void)
{
	if (FEditor == NULL)
		return;

	int editHeight = FEditorHeight;
	if (editHeight < 1)
		editHeight = 1;
	if (editHeight > ClientHeight)
		editHeight = ClientHeight;

	int topPos = 0;
	switch (FVertAlign)
	{
		case vaCenter:
			topPos = (ClientHeight - editHeight) / 2;
			break;
		case vaBottom:
			topPos = ClientHeight - editHeight;
			break;
		case vaTop:
		default:
			topPos = 0;
			break;
	}

	if (topPos < 0)
		topPos = 0;

	topPos += FTextOffsetY;
	if (topPos < 0)
		topPos = 0;

	int editorLeft = 0;
	int editorWidth = ClientWidth;

	if (FImagePicture != NULL && FImagePicture->Graphic != NULL && !FImagePicture->Graphic->Empty)
	{
		const int imageRight = FImagePosX + FImageWidth;
		editorLeft = imageRight + 4; // reserve left icon area + gap
		editorWidth = ClientWidth - editorLeft;
	}

	if (editorWidth < 1)
		editorWidth = 1;

	FEditor->SetBounds(editorLeft, topPos, editorWidth, editHeight);
	ApplyTextOffsets();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::ApplyTextOffsets(void)
{
	if (FEditor == NULL || !FEditor->HandleAllocated())
		return;

	int leftMargin = 1 + FTextOffsetX;
	if (leftMargin < 0)
		leftMargin = 0;

	SendMessage(FEditor->Handle, EM_SETMARGINS, EC_LEFTMARGIN, MAKELPARAM(leftMargin, 0));

	RECT textRect;
	::GetClientRect(FEditor->Handle, &textRect); // call WinAPI GetClientRect, not TWinControl::GetClientRect()
	textRect.left = leftMargin;
	SendMessage(FEditor->Handle, EM_SETRECTNP, 0, (LPARAM)&textRect);
	InvalidateRect(FEditor->Handle, NULL, TRUE);
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::ImageChanged(TObject *)
{
	UpdateEditorBounds();
	Invalidate();
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TAlignEdit::GetText(void)
{
	if (FEditor == NULL)
		return L"";
	return FEditor->Text;
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetText(const UnicodeString Value)
{
	if (FEditor != NULL)
		FEditor->Text = Value;
}
//---------------------------------------------------------------------------
WideChar __fastcall TAlignEdit::GetPasswordChar(void)
{
	if (FEditor == NULL)
		return 0;
	return FEditor->PasswordChar;
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetPasswordChar(const WideChar Value)
{
	if (FEditor != NULL)
		FEditor->PasswordChar = Value;
}
//---------------------------------------------------------------------------
bool __fastcall TAlignEdit::GetReadOnly(void)
{
	if (FEditor == NULL)
		return false;
	return FEditor->ReadOnly;
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetReadOnly(bool Value)
{
	if (FEditor != NULL)
		FEditor->ReadOnly = Value;
}
//---------------------------------------------------------------------------
int __fastcall TAlignEdit::GetMaxLength(void)
{
	if (FEditor == NULL)
		return 0;
	return FEditor->MaxLength;
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetMaxLength(int Value)
{
	if (FEditor != NULL)
		FEditor->MaxLength = Value;
}
//---------------------------------------------------------------------------
TBorderStyle __fastcall TAlignEdit::GetBorderStyle(void)
{
	if (FEditor == NULL)
		return bsSingle;
	return FEditor->BorderStyle;
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetBorderStyle(TBorderStyle Value)
{
	if (FEditor != NULL)
	{
		FEditor->BorderStyle = Value;
		UpdateEditorBounds();
	}
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetEditorClientColor(TColor Value)
{
	if (FEditorClientColor == Value)
		return;

	FEditorClientColor = Value;
	Color = Value;

	if (FEditor != NULL)
	{
		FEditor->ParentColor = false;
		FEditor->Color = Value;
	}

	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetEditorHeight(int Value)
{
	if (Value < 1)
		Value = 1;

	if (FEditorHeight == Value)
		return;

	FEditorHeight = Value;
	UpdateEditorBounds();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetTextOffsetX(int Value)
{
	if (FTextOffsetX == Value)
		return;

	FTextOffsetX = Value;
	ApplyTextOffsets();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetTextOffsetY(int Value)
{
	if (FTextOffsetY == Value)
		return;

	FTextOffsetY = Value;
	UpdateEditorBounds();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetImagePicture(TPicture *Value)
{
	if (FImagePicture == NULL)
		return;

	FImagePicture->Assign(Value);
	UpdateEditorBounds();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetImageWidth(int Value)
{
	if (Value < 1)
		Value = 1;

	if (FImageWidth == Value)
		return;

	FImageWidth = Value;
	UpdateEditorBounds();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetImageHeight(int Value)
{
	if (Value < 1)
		Value = 1;

	if (FImageHeight == Value)
		return;

	FImageHeight = Value;
	UpdateEditorBounds();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetImagePosX(int Value)
{
	if (FImagePosX == Value)
		return;

	FImagePosX = Value;
	UpdateEditorBounds();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetImagePosY(int Value)
{
	if (FImagePosY == Value)
		return;

	FImagePosY = Value;
	UpdateEditorBounds();
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetAlignment(TAlignment Value)
{
	if (FAlignment == Value)
		return;

	FAlignment = Value;
	if (FEditor != NULL)
		FEditor->Alignment = Value;
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetVertAlign(TEditVertAlign Value)
{
	if (FVertAlign == Value)
		return;

	FVertAlign = Value;
	UpdateEditorBounds();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::EditorClick(TObject *)
{
	Click();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::EditorChange(TObject *)
{
	if (FEditor != NULL && FEditor->HandleAllocated())
	{
		HDC dc = GetDC(FEditor->Handle);
		if (dc != NULL)
		{
			HFONT oldFont = (HFONT)SelectObject(dc, FEditor->Font->Handle);
			TEXTMETRIC tm;
			GetTextMetrics(dc, &tm);
			SelectObject(dc, oldFont);
			ReleaseDC(FEditor->Handle, dc);

			FEditorHeight = tm.tmHeight + 1;
			UpdateEditorBounds();
		}
	}

	if (FOnChange != NULL)
		FOnChange(this);
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::WndProc(TMessage &Msg)
{
	switch (Msg.Msg)
	{
		case CM_MOUSEENTER:
			Cursor = crIBeam;
			break;
		case CM_MOUSELEAVE:
			Cursor = crDefault;
			break;
		case WM_SETFOCUS:
			if (FEditor != NULL && FEditor->CanFocus())
				FEditor->SetFocus();
			Msg.Result = 0;
			return;
		case CM_FONTCHANGED:
			TCustomControl::WndProc(Msg);
			if (FEditor != NULL)
			{
				FEditor->Font->Assign(Font);
				if (FEditor->HandleAllocated())
				{
					HDC dc = GetDC(FEditor->Handle);
					if (dc != NULL)
					{
						HFONT oldFont = (HFONT)SelectObject(dc, FEditor->Font->Handle);
						TEXTMETRIC tm;
						GetTextMetrics(dc, &tm);
						SelectObject(dc, oldFont);
						ReleaseDC(FEditor->Handle, dc);
						FEditorHeight = tm.tmHeight + 1;
					}
				}
			}
			UpdateEditorBounds();
			return;
		case CM_COLORCHANGED:
			TCustomControl::WndProc(Msg);
			FEditorClientColor = Color;
			if (FEditor != NULL)
			{
				FEditor->ParentColor = false;
				FEditor->Color = FEditorClientColor;
			}
			Invalidate();
			return;
		case CM_ENABLEDCHANGED:
			TCustomControl::WndProc(Msg);
			if (FEditor != NULL)
				FEditor->Enabled = Enabled;
			return;
		case CM_STYLECHANGED:
			TCustomControl::WndProc(Msg);
			if (FEditor != NULL)
				FEditor->StyleElements = StyleElements;
			return;
		case WM_SIZE:
			TCustomControl::WndProc(Msg);
			UpdateEditorBounds();
			return;
	}

	TCustomControl::WndProc(Msg);
}
//---------------------------------------------------------------------------
namespace Alignedit
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TAlignEdit)};
		RegisterComponents(L"MD", classes, 0);
	}
}
//---------------------------------------------------------------------------
