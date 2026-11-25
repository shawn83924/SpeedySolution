//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnifyDlg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma resource "*.dfm"
TUnifyDlgs *UnifyDlgs;
//---------------------------------------------------------------------------
__fastcall TUnifyDlgs::TUnifyDlgs(TComponent* Owner)
	: TForm(Owner)
	, FColorTextBox( NULL )
{
	RoundFormEx->Lockbox = false;
}
//---------------------------------------------------------------------------
bool __fastcall TUnifyDlgs::AskStringDialog( const String& text, String& Name )
{
	TUnifyDlgs* MsgDlg = new TUnifyDlgs( NULL );
	MsgDlg->Caption = text;
	MsgDlg->MsgLabel->Visible = false;
	MsgDlg->NameEdit->Visible = true;
	MsgDlg->NameEdit->Text = Name;
	MsgDlg->OKButton->Visible = true;
	MsgDlg->OKButton->Left = (MsgDlg->ClientWidth - MsgDlg->OKButton->Width - MsgDlg->OKButton->Width - 15 )/2;
	MsgDlg->CancelButton->Visible = true;
	MsgDlg->CancelButton->Left = MsgDlg->OKButton->Left + MsgDlg->OKButton->Width +  15;
	if( MsgDlg->ShowModal() == mrOk )
	{
	   Name = MsgDlg->NameEdit->Text;
	   delete MsgDlg;
	   return true;
	}
	delete MsgDlg;
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TUnifyDlgs::MessageDialog( const String& caption, const String& text )
{
	TUnifyDlgs* MsgDlg = new TUnifyDlgs( NULL );
	int MinW = MsgDlg->Canvas->TextWidth( text ) + 30;

	if( MsgDlg->Width < MinW )
		MsgDlg->Width = MinW;
	MsgDlg->Height = 110;
	MsgDlg->Caption = caption;
	MsgDlg->MsgLabel->Caption = text;
	MsgDlg->MsgLabel->Visible = true;
	MsgDlg->NameEdit->Visible = false;
	MsgDlg->OKButton->Visible = true;
	MsgDlg->OKButton->Left = (MsgDlg->ClientWidth - MsgDlg->OKButton->Width)/2;
	MsgDlg->CancelButton->Visible = false;
	MsgDlg->ShowModal();
	delete MsgDlg;
}
//---------------------------------------------------------------------------
bool __fastcall TUnifyDlgs::AskYesNoDialog( const String& caption, const String& text )
{
	TUnifyDlgs* MsgDlg = new TUnifyDlgs( NULL );
	int MinW = MsgDlg->Canvas->TextWidth( text ) + 30;

	if( MsgDlg->Width < MinW )
		MsgDlg->Width = MinW;
	MsgDlg->Caption = caption;
	MsgDlg->MsgLabel->Caption = text;
	MsgDlg->MsgLabel->Visible = true;
	MsgDlg->NameEdit->Visible = false;
	MsgDlg->OKButton->Visible = true;
	MsgDlg->OKButton->Left = (MsgDlg->ClientWidth - MsgDlg->OKButton->Width - MsgDlg->OKButton->Width - 15 )/2;
	MsgDlg->CancelButton->Visible = true;
	MsgDlg->CancelButton->Left = MsgDlg->OKButton->Left + MsgDlg->OKButton->Width + 15;
	if( MsgDlg->ShowModal() == mrOk )
	{
		delete MsgDlg;
		return true;
	}
	delete MsgDlg;
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUnifyDlgs::AskYesNoDialog(
		const String& caption,
		const String& text,
		const String& YesStr,
		const String& CancelStr)
{
	TUnifyDlgs* MsgDlg = new TUnifyDlgs( NULL );
	int MinW = MsgDlg->Canvas->TextWidth( text ) + 30;

	if( MsgDlg->Width < MinW )
		MsgDlg->Width = MinW;
	MsgDlg->Caption = caption;
	MsgDlg->MsgLabel->Caption = text;
	MsgDlg->MsgLabel->Visible = true;
	MsgDlg->NameEdit->Visible = false;
	MsgDlg->OKButton->Visible = true;
	MsgDlg->OKButton->Left = (MsgDlg->ClientWidth - MsgDlg->OKButton->Width - MsgDlg->OKButton->Width - 15 )/2;
	MsgDlg->OKButton->Caption = YesStr;
	MsgDlg->CancelButton->Visible = true;
	MsgDlg->CancelButton->Left = MsgDlg->OKButton->Left + MsgDlg->OKButton->Width + 15;
	MsgDlg->CancelButton->Caption = CancelStr;
	if( MsgDlg->ShowModal() == mrOk )
	{
		delete MsgDlg;
		return true;
	}
	delete MsgDlg;
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUnifyDlgs::TryParseColor( const String& Name, TColor& Color )
{
	String N = Name.Trim();

	if( N.IsEmpty() )
		return false;
	if( N[1] == L'$' ) ///< $BBGGRR
	{
		int Val;
		if( TryStrToInt( N, Val ) == false )
			return false;
		Color = static_cast<TColor>( Val );
		return true;
	}
	if( N.Length() < 2 || N.SubString( 1, 2 ).LowerCase() != L"cl" )
		N = L"cl" + N;

	int Val;
	if( IdentToColor( N, Val ) == false )
		return false;
	Color = static_cast<TColor>( Val );
	return true;
}
//---------------------------------------------------------------------------
///< Split text into color segments. Tags: <c=name> ... </c>. Unknown tags are kept as plain text.
void __fastcall TUnifyDlgs::ParseColorText( const String& text, TColor DefColor )
{
	FSegments.clear();

	TColor Cur = DefColor;
	String Buf;
	int    Len = text.Length();
	int    i   = 1;

	while( i <= Len )
	{
		if( text[i] == L'<' )
		{
			int Close = 0;
			for( int j = i + 1; j <= Len; j++ )
			{
				if( text[j] == L'>' )
				{
					Close = j;
					break;
				}
			}
			if( Close > 0 )
			{
				String Tag = text.SubString( i + 1, Close - i - 1 );
				bool   IsTag = false;
				TColor NewColor = Cur;

				if( Tag == L"/c" )
				{
					NewColor = DefColor;
					IsTag = true;
				}
				else if( Tag.Length() > 2 && Tag.SubString( 1, 2 ) == L"c=" )
					IsTag = TryParseColor( Tag.SubString( 3, Tag.Length() - 2 ), NewColor );

				if( IsTag == true )
				{
					if( Buf.IsEmpty() == false )
					{
						TColorTextSegment Seg;
						Seg.Text  = Buf;
						Seg.Color = Cur;
						FSegments.push_back( Seg );
						Buf = L"";
					}
					Cur = NewColor;
					i = Close + 1;
					continue;
				}
			}
		}
		Buf += text[i];
		i++;
	}
	if( Buf.IsEmpty() == false )
	{
		TColorTextSegment Seg;
		Seg.Text  = Buf;
		Seg.Color = Cur;
		FSegments.push_back( Seg );
	}
}
//---------------------------------------------------------------------------
String __fastcall TUnifyDlgs::GetPlainText( void )
{
	String Text;

	for( unsigned int i = 0; i < FSegments.size(); i++ )
		Text += FSegments[i].Text;
	return Text;
}
//---------------------------------------------------------------------------
///< Draw segments on one line, horizontally centered, top aligned ( same as MsgLabel ).
void __fastcall TUnifyDlgs::ColorTextBoxPaint( TObject* Sender )
{
	TPaintBox* Box = dynamic_cast<TPaintBox*>( Sender );

	if( Box == NULL )
		return;

	TCanvas* C = Box->Canvas;
	C->Font->Assign( MsgLabel->Font );
	C->Brush->Style = bsClear;

	int TotalW = C->TextWidth( GetPlainText() );
	int X = ( Box->Width - TotalW ) / 2;

	if( X < 0 )
		X = 0;
	for( unsigned int i = 0; i < FSegments.size(); i++ )
	{
		C->Font->Color = FSegments[i].Color;
		C->TextOut( X, 0, FSegments[i].Text );
		X += C->TextWidth( FSegments[i].Text );
	}
}
//---------------------------------------------------------------------------
bool __fastcall TUnifyDlgs::AskYesNoColorDialog( const String& caption, const String& text )
{
	TUnifyDlgs* MsgDlg = new TUnifyDlgs( NULL );

	MsgDlg->ParseColorText( text, MsgDlg->MsgLabel->Font->Color );

	int MinW = MsgDlg->Canvas->TextWidth( MsgDlg->GetPlainText() ) + 30;

	if( MsgDlg->Width < MinW )
		MsgDlg->Width = MinW;
	MsgDlg->Caption = caption;
	MsgDlg->MsgLabel->Visible = false;
	MsgDlg->NameEdit->Visible = false;

	///< Paint box takes MsgLabel's place to draw multi-color text.
	MsgDlg->FColorTextBox = new TPaintBox( MsgDlg );
	MsgDlg->FColorTextBox->Parent  = MsgDlg;
	MsgDlg->FColorTextBox->SetBounds( MsgDlg->MsgLabel->Left, MsgDlg->MsgLabel->Top,
									  MsgDlg->MsgLabel->Width, MsgDlg->MsgLabel->Height );
	MsgDlg->FColorTextBox->Anchors = MsgDlg->MsgLabel->Anchors;
	MsgDlg->FColorTextBox->OnPaint = MsgDlg->ColorTextBoxPaint;

	MsgDlg->OKButton->Visible = true;
	MsgDlg->OKButton->Left = (MsgDlg->ClientWidth - MsgDlg->OKButton->Width - MsgDlg->OKButton->Width - 15 )/2;
	MsgDlg->CancelButton->Visible = true;
	MsgDlg->CancelButton->Left = MsgDlg->OKButton->Left + MsgDlg->OKButton->Width + 15;

	bool Result = ( MsgDlg->ShowModal() == mrOk );
	delete MsgDlg;
	return Result;
}
//---------------------------------------------------------------------------
void __fastcall TUnifyDlgs::NameEditKeyPress(TObject *Sender, System::WideChar &Key)
{
	if( Key == VK_RETURN )
		this->ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TUnifyDlgs::OpenBrowser( const String& URL )
{
	SHELLEXECUTEINFOW si;
	/// Fill SHELLEXECUTEINFO structure for ShellExecute Win API.
	ZeroMemory( &si, sizeof(si) );
	si.cbSize   = sizeof(si);
	si.lpVerb   = L"open";
	si.lpFile   = URL.c_str();
	si.hInstApp = HInstance;
	si.nShow    = SW_SHOWDEFAULT;
	si.fMask    = SEE_MASK_NOCLOSEPROCESS;
	ShellExecuteExW( &si );
}
//---------------------------------------------------------------------------
void __fastcall TUnifyDlgs::FormCreate(TObject *Sender)
{
   Application->NormalizeTopMosts();
   SetWindowPos( Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
}
//---------------------------------------------------------------------------

