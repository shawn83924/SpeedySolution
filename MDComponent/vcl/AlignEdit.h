//---------------------------------------------------------------------------

#ifndef AlignEditH
#define AlignEditH
//---------------------------------------------------------------------------
#include <System.SysUtils.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Graphics.hpp>
#include <Messages.hpp>
//---------------------------------------------------------------------------
typedef enum { vaTop, vaCenter, vaBottom } TEditVertAlign;
//---------------------------------------------------------------------------
class PACKAGE TAlignEdit : public TCustomControl
{
private:
	TEdit *FEditor;
	TPicture *FImagePicture;
	TColor FEditorClientColor;
	UnicodeString FTextHint;
	TAlignment FAlignment;
	TEditVertAlign FVertAlign;
	int FEditorHeight;
	int FTextOffsetX;
	int FTextOffsetY;
	int FImageWidth;
	int FImageHeight;
	int FImagePosX;
	int FImagePosY;
	TNotifyEvent FOnChange;

	UnicodeString __fastcall GetText(void);
	void __fastcall SetText(const UnicodeString Value);
	UnicodeString __fastcall GetTextHint(void);
	void __fastcall SetTextHint(const UnicodeString Value);
	WideChar __fastcall GetPasswordChar(void);
	void __fastcall SetPasswordChar(const WideChar Value);
	bool __fastcall GetReadOnly(void);
	void __fastcall SetReadOnly(bool Value);
	int __fastcall GetMaxLength(void);
	void __fastcall SetMaxLength(int Value);
	TBorderStyle __fastcall GetBorderStyle(void);
	void __fastcall SetBorderStyle(TBorderStyle Value);
	void __fastcall SetEditorClientColor(TColor Value);
	void __fastcall SetEditorHeight(int Value);
	void __fastcall SetTextOffsetX(int Value);
	void __fastcall SetTextOffsetY(int Value);
	void __fastcall SetImagePicture(TPicture *Value);
	void __fastcall SetImageWidth(int Value);
	void __fastcall SetImageHeight(int Value);
	void __fastcall SetImagePosX(int Value);
	void __fastcall SetImagePosY(int Value);
	void __fastcall SetAlignment(TAlignment Value);
	void __fastcall SetVertAlign(TEditVertAlign Value);

	void __fastcall UpdateEditorBounds(void);
	void __fastcall ApplyTextOffsets(void);
	void __fastcall ImageChanged(TObject *Sender);
	void __fastcall EditorClick(TObject *Sender);
	void __fastcall EditorChange(TObject *Sender);
protected:
	virtual void __fastcall CreateWnd(void);
	DYNAMIC void __fastcall Resize(void);
	DYNAMIC void __fastcall MouseDown(TMouseButton Button, TShiftState Shift, int X, int Y);
	virtual void __fastcall Paint(void);
	virtual void __fastcall WndProc(TMessage &Msg);
public:
	__fastcall TAlignEdit(TComponent* Owner);
	__fastcall virtual ~TAlignEdit(void);
__published:
	__property UnicodeString Text = { read = GetText, write = SetText };
	__property UnicodeString TextHint = { read = GetTextHint, write = SetTextHint };
	__property WideChar PasswordChar = { read = GetPasswordChar, write = SetPasswordChar, default = 0 };
	__property bool ReadOnly = { read = GetReadOnly, write = SetReadOnly, default = false };
	__property int MaxLength = { read = GetMaxLength, write = SetMaxLength, default = 0 };
	__property TBorderStyle BorderStyle = { read = GetBorderStyle, write = SetBorderStyle, default = bsNone };
	__property TColor EditorClientColor = { read = FEditorClientColor, write = SetEditorClientColor, default = clWindow };
	__property int EditorHeight = { read = FEditorHeight, write = SetEditorHeight, default = 21 };
	__property int TextOffsetX = { read = FTextOffsetX, write = SetTextOffsetX, default = 0 };
	__property int TextOffsetY = { read = FTextOffsetY, write = SetTextOffsetY, default = 0 };
	__property TPicture* ImagePicture = { read = FImagePicture, write = SetImagePicture };
	__property int ImageWidth = { read = FImageWidth, write = SetImageWidth, default = 21 };
	__property int ImageHeight = { read = FImageHeight, write = SetImageHeight, default = 21 };
	__property int ImagePosX = { read = FImagePosX, write = SetImagePosX, default = 0 };
	__property int ImagePosY = { read = FImagePosY, write = SetImagePosY, default = 0 };
	__property TAlignment Alignment = { read = FAlignment, write = SetAlignment, default = taLeftJustify };
	__property TEditVertAlign VertAlign = { read = FVertAlign, write = SetVertAlign, default = vaTop };
	__property TNotifyEvent OnChange = { read = FOnChange, write = FOnChange };

	__property Align;
	__property Anchors;
	__property Enabled;
	__property Font;
	__property Color;
	__property ParentColor;
	__property ParentFont;
	__property StyleElements;
	__property Visible;
	__property TabOrder;
	__property TabStop;
	__property ShowHint;
	__property ParentShowHint;
	__property PopupMenu;
	__property OnClick;
	__property OnDblClick;
	__property OnEnter;
	__property OnExit;
};
//---------------------------------------------------------------------------
#endif
