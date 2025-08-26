//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "SetFontForm.h"
#include "FMTConfig.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TFontForm *FontForm;
//---------------------------------------------------------------------------
__fastcall TFontForm::TFontForm(TComponent* Owner)
	: TForm(Owner)
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
void __fastcall TFontForm::SetTextObject( TText* txt )
{
	int Size,index;
	String SizeStr;
	FText = txt;

	TextColorBox->Selected = (TColor)g_Config.GetIntegerProperty( "TextSetting","Color", (int)clCream );
	TextEdit->Text         = g_Config.GetStringProperty( "TextSetting","Text", L"¤å¦r" );
	Size                   = g_Config.GetIntegerProperty( "TextSetting","Size", SizeComboBox->Text.ToInt() );
	BoldButton->Down       = g_Config.GetBoolProperty( "TextSetting","BoldDown", false );
	UnderlineButton->Down  = g_Config.GetBoolProperty( "TextSetting","UnderlineDown", false );
	ItalicButton->Down     = g_Config.GetBoolProperty( "TextSetting","ItalicDown", false );

	SizeStr.printf( L"%d", Size );
	if( (index = SizeComboBox->Items->IndexOf(SizeStr)) == -1 )
	{
		SizeComboBox->ItemIndex = 0;
		Size = SizeComboBox->Text.ToInt();
	}
	else
		SizeComboBox->ItemIndex = index;
	FText->SetText( TextEdit->Text );
	FText->SetColor( TextColorBox->Selected );
	FText->SetSize( Size );
	TextEdit->Font->Color = TextColorBox->Selected;
	TextEdit->Font->Size  = Size;
	BoldButtonClick( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TFontForm::SizeComboBoxChange(TObject *Sender)
{
	int Size = SizeComboBox->Text.ToInt();

	FText->SetSize( Size );
	TextEdit->Font->Size = Size;
}
//---------------------------------------------------------------------------
void __fastcall TFontForm::TextColorBoxChange(TObject *Sender)
{
	FText->SetColor( TextColorBox->Selected );
	TextEdit->Font->Color = TextColorBox->Selected;
	FText->RePaint();
}
//---------------------------------------------------------------------------
void __fastcall TFontForm::BoldButtonClick(TObject *Sender)
{
	System::Uitypes::TFontStyles FontStyle;

	FontStyle.Clear();
	if( BoldButton->Down == true )
		FontStyle << fsBold;
	if( UnderlineButton->Down == true )
		FontStyle << fsUnderline;
	if( ItalicButton->Down == true )
		FontStyle << fsItalic;
	TextEdit->Font->Style = FontStyle;
	FText->SetStyle( FontStyle );
}
//---------------------------------------------------------------------------
void __fastcall TFontForm::FormCreate(TObject *Sender)
{
   Application->NormalizeTopMosts();
   SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
}
//---------------------------------------------------------------------------
void __fastcall TFontForm::OKButtonClick(TObject *Sender)
{
	System::Uitypes::TFontStyles FontStyle;

	FontStyle.Clear();
	if( BoldButton->Down == true )
		FontStyle << fsBold;
	if( UnderlineButton->Down == true )
		FontStyle << fsUnderline;
	if( ItalicButton->Down == true )
		FontStyle << fsItalic;
	FText->SetStyle( FontStyle );
	FText->SetColor( TextColorBox->Selected );
	FText->SetSize(  TextEdit->Font->Size );
	FText->SetText(  TextEdit->Text );
	FText->RePaint();

	g_Config.SetIntegerProperty( "TextSetting","Color", (int)TextColorBox->Selected );
	g_Config.SetStringProperty( "TextSetting","Text", TextEdit->Text );
	g_Config.SetIntegerProperty( "TextSetting","Size", TextEdit->Font->Size );
	g_Config.SetBoolProperty( "TextSetting","BoldDown", BoldButton->Down );
	g_Config.SetBoolProperty( "TextSetting","UnderlineDown", UnderlineButton->Down );
	g_Config.SetBoolProperty( "TextSetting","ItalicDown", ItalicButton->Down );
}
//---------------------------------------------------------------------------
void __fastcall TFontForm::TextEditChange(TObject *Sender)
{
	FText->SetText(  TextEdit->Text );
}
//---------------------------------------------------------------------------
void __fastcall TFontForm::SymbolButtonClick(TObject *Sender)
{
	TSpeedButton *SpdButton = dynamic_cast<TSpeedButton*>(Sender);
	if( SpdButton != NULL )
	{
		String NewText = TextEdit->Text + SpdButton->Caption;
		TextEdit->Text = NewText;
	}
}
//---------------------------------------------------------------------------

