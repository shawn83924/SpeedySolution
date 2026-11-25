//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <algorithm>

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
	FInputFilter = TInputFilterFlags() << iffLetters << iffDigits << iffSpace << iffChinese << iffSpecial;
	FFilteringText = false;
	FMaskEnabled = false;
	FMaskVisibleLength = 3;
	FMaskChar = L'*';
	FRealText = L"";
	FDisplayText = L"";

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
	FEditor->OnKeyPress = EditorKeyPress;

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
	if (FMaskEnabled)
		return FRealText;
	if (FEditor == NULL)
		return L"";
	return FEditor->Text;
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetText(const UnicodeString Value)
{
	if (FEditor == NULL)
		return;

	if (FMaskEnabled)
	{
		if (FRealText == Value)
			return;
		FRealText = Value;
		RefreshMaskedDisplay(FRealText.Length());
		UpdateHeightAndNotify();
		return;
	}

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
	// 內部改寫編輯框文字時觸發的巢狀事件，由外層統一處理
	if (FFilteringText || FEditor == NULL)
		return;

	if (FMaskEnabled)
	{
		HandleMaskedChange();
	}
	else
	{
		UnicodeString src = FEditor->Text;
		UnicodeString cleaned = FilterText(src);

		if (cleaned != src)
		{
			FFilteringText = true;
			int caretPos = FEditor->SelStart;
			FEditor->Text = cleaned;
			int removedCount = src.Length() - cleaned.Length();
			FEditor->SelStart = std::max(0, caretPos - removedCount);
			FFilteringText = false;
		}
	}

	UpdateHeightAndNotify();
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::UpdateHeightAndNotify(void)
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
bool __fastcall TAlignEdit::IsCharAllowed(WideChar Ch)
{
	bool isLetter  = (Ch >= L'A' && Ch <= L'Z') || (Ch >= L'a' && Ch <= L'z');
	bool isDigit   = (Ch >= L'0' && Ch <= L'9');
	bool isSpace   = (Ch == L' ');
	bool isChinese = (Ch > 0x7F);
	bool isSpecial = (Ch >= 0x21 && Ch <= 0x7E) && !isLetter && !isDigit;

	return
		(FInputFilter.Contains(iffLetters) && isLetter)  ||
		(FInputFilter.Contains(iffDigits)  && isDigit)   ||
		(FInputFilter.Contains(iffSpace)   && isSpace)   ||
		(FInputFilter.Contains(iffChinese) && isChinese) ||
		(FInputFilter.Contains(iffSpecial) && isSpecial);
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::EditorKeyPress(TObject *, System::WideChar &Key)
{
	if (Key < 0x20) // 放行所有控制字元（退格、Ctrl+C/V/X/A 等）
		return;

	if (!IsCharAllowed(Key))
		Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetInputFilter(TInputFilterFlags Value)
{
	FInputFilter = Value;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TAlignEdit::FilterText(const UnicodeString Value)
{
	UnicodeString cleaned;
	for (int i = 1; i <= Value.Length(); i++)
	{
		if (IsCharAllowed(Value[i]))
			cleaned += Value[i];
	}
	return cleaned;
}
//---------------------------------------------------------------------------
// 隱碼功能
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetMaskEnabled(bool Value)
{
	if (FMaskEnabled == Value)
		return;

	if (FEditor == NULL)
	{
		FMaskEnabled = Value;
		return;
	}

	int caretPos = FEditor->SelStart;
	if (Value)
	{
		// 開啟：以目前編輯框內容作為實際文字，改為顯示隱碼
		FRealText = FEditor->Text;
		FMaskEnabled = true;
		RefreshMaskedDisplay(caretPos);
	}
	else
	{
		// 關閉：還原實際文字
		FMaskEnabled = false;
		FFilteringText = true;
		FEditor->Text = FRealText;
		FEditor->SelStart = std::min(caretPos, FRealText.Length());
		FFilteringText = false;
		FRealText = L"";
		FDisplayText = L"";
	}
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetMaskVisibleLength(int Value)
{
	if (Value < 0)
		Value = 0;

	if (FMaskVisibleLength == Value)
		return;

	FMaskVisibleLength = Value;
	if (FMaskEnabled && FEditor != NULL)
		RefreshMaskedDisplay(FEditor->SelStart);
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::SetMaskChar(WideChar Value)
{
	if (Value == 0)
		Value = L'*';

	if (FMaskChar == Value)
		return;

	FMaskChar = Value;
	if (FMaskEnabled && FEditor != NULL)
		RefreshMaskedDisplay(FEditor->SelStart);
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TAlignEdit::BuildMaskedText(const UnicodeString Value)
{
	int len = Value.Length();
	if (len <= FMaskVisibleLength)
		return Value;

	return Value.SubString(1, FMaskVisibleLength) + UnicodeString::StringOfChar(FMaskChar, len - FMaskVisibleLength);
}
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::RefreshMaskedDisplay(int CaretPos)
{
	if (FEditor == NULL)
		return;

	FDisplayText = BuildMaskedText(FRealText);

	FFilteringText = true;
	if (FEditor->Text != FDisplayText)
		FEditor->Text = FDisplayText;
	FEditor->SelStart = std::max(0, std::min(CaretPos, FDisplayText.Length()));
	FFilteringText = false;

	// 編輯框的 Undo 緩衝區存的是顯示文字，復原後會與實際文字不同步，故清除
	if (FEditor->HandleAllocated())
		SendMessage(FEditor->Handle, EM_EMPTYUNDOBUFFER, 0, 0);
}
//---------------------------------------------------------------------------
// 比對編輯前後的顯示文字，找出被刪除與新插入的區段，再套用到實際文字
//---------------------------------------------------------------------------
void __fastcall TAlignEdit::HandleMaskedChange(void)
{
	const UnicodeString oldDisp = FDisplayText;
	const UnicodeString newDisp = FEditor->Text;
	const int oldLen = oldDisp.Length();
	const int newLen = newDisp.Length();
	const int caretPos = FEditor->SelStart; // 編輯後游標位於插入區段的尾端

	// 共同後綴：游標之後的文字不會被本次編輯改變
	int commonSuffix = 0;
	const int maxCommon = std::min(oldLen, newLen);
	while (commonSuffix < maxCommon &&
		   oldDisp[oldLen - commonSuffix] == newDisp[newLen - commonSuffix])
		commonSuffix++;
	int suffix = std::min(commonSuffix, std::max(0, newLen - caretPos));

	// 共同前綴（不可與後綴重疊）
	int prefixLimit = std::min(oldLen, newLen) - suffix;
	int prefix = 0;
	while (prefix < prefixLimit && oldDisp[prefix + 1] == newDisp[prefix + 1])
		prefix++;

	UnicodeString inserted = FilterText(newDisp.SubString(prefix + 1, newLen - suffix - prefix));

	FRealText = FRealText.SubString(1, prefix) + inserted + FRealText.SubString(oldLen - suffix + 1, oldLen);

	RefreshMaskedDisplay(prefix + inserted.Length());
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
