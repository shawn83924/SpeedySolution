//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "IniEditBox.h"
#include "iniFile.h"
#include "IniFiles.hpp"
#include "NewValueForm.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TIniEditBox *)
{
    new TIniEditBox(NULL);
}
//---------------------------------------------------------------------------
__fastcall TIniEditBox::TIniEditBox(TComponent* Owner)
:TCustomListBox(Owner)
,FSelectIndex( -1 )
,FInitItemCount( 0 )
{
    Style = lbOwnerDrawFixed;
    FEdit = new TEdit( this );
    FEdit->Parent = this;
    FEdit->Visible = false;
    FEdit->Font = Font;
    FEdit->Tag = -1;
    FEdit->ImeMode = imClose;
	ItemHeight = 20;

	FMenu = new TPopupMenu( this );
	TMenuItem* AddSection = new TMenuItem( FMenu );
	AddSection->Caption = L"New Section";
	AddSection->OnClick = OnNewSection;

	TMenuItem* AddNameValue = new TMenuItem( FMenu );
	AddNameValue->Caption = L"New Name-Value";
	AddNameValue->OnClick = OnNewNameValue;

	TMenuItem* Delete = new TMenuItem( FMenu );
	Delete->Caption = L"Delete";
	Delete->OnClick = OnDelete;

	FMenu->Items->Add( AddSection );
	FMenu->Items->Add( AddNameValue );
	FMenu->Items->Add( Delete );
}
//---------------------------------------------------------------------------
int __fastcall TIniEditBox::SectionList( TStringList* List )
{
	TiniObject* Obj;
	int SecIndex = 0;

	for( register int i = 0; i < Items->Count; i++ )
	{
		 if((Obj = dynamic_cast<TiniObject*>( Items->Objects[i] ))!=NULL)
		 {
			if( Obj->IsSection() == true )
				List->Add( Obj->GetName() );
		 }
	}

	for( register int i = FSelectIndex; i >=0; i-- )
	{
		 if((Obj = dynamic_cast<TiniObject*>( Items->Objects[i] ))!=NULL)
		 {
			if( Obj->IsSection() == true )
			{
				SecIndex = List->IndexOf( Obj->GetName() );
				break;
			}
		 }
	}
	return SecIndex;
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::OnNewSection( TObject* Sender )
{
	int Index;

	Items->AddObject( "New Section", new TSectionObject( "New Section" ));
	Index = Items->Count -1;
	ItemIndex  = Index;
	TopIndex   = Index;
	EditSection( Index, ItemRect( Index ), "New Section" );
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::OnNewNameValue( TObject* Sender )
{
	if( FSelectIndex != -1 )
	{
		TAddValueForm* NewValueForm = new TAddValueForm( this );
		TStringList*   Sections = new TStringList();
		int            SecIndex = SectionList( Sections );

		NewValueForm->SectionComboBox->Items->AddStrings( Sections );
		NewValueForm->SectionComboBox->ItemIndex = SecIndex;
		if( NewValueForm->ShowModal() == mrOk )
		{
			AnsiString NameStr = NewValueForm->NameEdit->Text;
			AnsiString ValueStr = NewValueForm->ValueEdit->Text;
			int Index = FSelectIndex + 1;

			if( FEdit->Tag != -1 )
			{
				UpdateValue( FEdit->Tag, FEdit->Text );
				FEdit->Tag = -1;
			}
			Items->InsertObject( Index, NameStr, new TNameValueObject( NameStr, ValueStr ) );
			TRect Rect  = ItemRect( Index );
			int   HalfW = Rect.Width() / 2;

			Rect.Left  = Rect.Left + HalfW;
			ItemIndex  = Index;
			EditSection( Index, Rect,ValueStr);
		}
		delete Sections;
		delete NewValueForm;
	}
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::OnDelete( TObject* Sender )
{
	if( FSelectIndex != -1 )
	{
		String Msg;
		TSectionObject* Section = dynamic_cast<TSectionObject*>( Items->Objects[FSelectIndex] );
		if( Section != NULL ) ///< Delete Section
		{
			Msg.printf( L"Do you want to delete section[%s] and all values in section?", String( Section->GetName()) );
			if( MessageDlg( Msg, mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
			{
				TiniObject* DelItem;
				int EndIndex = FSelectIndex;

				for( int i = FSelectIndex + 1; i < Items->Count; i ++ )
				{
					if( (DelItem = dynamic_cast<TiniObject*>( Items->Objects[i])) != NULL )
					{
						if( DelItem->IsSection() == true )
						{
							EndIndex = i - 1;
							break;
						}
					}
					if( i == Items->Count -1 && EndIndex == FSelectIndex )
						EndIndex = i;
				}
				for( int i = EndIndex; i >= FSelectIndex; i -- )
				{
					delete  Items->Objects[i];
					Items->Delete( i );
				}
				FEdit->Tag = -1;
				FEdit->Visible = false;
			}
		}
		else ///< Delete Name-Value
		{
			TNameValueObject* Item = dynamic_cast< TNameValueObject*>( Items->Objects[FSelectIndex] );

			Msg.printf( L"Do you want to delete value[%s] ?", String( Item->GetName()) );
			if( MessageDlg( Msg, mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
			{
				delete  Items->Objects[FSelectIndex];
				Items->Delete( FSelectIndex );
				FEdit->Tag = -1;
				FEdit->Visible = false;
			}
		}
	}
}
//---------------------------------------------------------------------------
__fastcall TIniEditBox::~TIniEditBox( void )
{
//    delete FEdit;
//    ClearItems( );
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::UpdateValue( int Index, AnsiString Value )
{
	TSectionObject* Section = dynamic_cast<TSectionObject*>( Items->Objects[Index] );
	if( Section != NULL )
	{
		if( Value.AnsiCompare( Section->GetName()) != 0 )
			Section->SetName( Value );
	}
	else
	{
		TNameValueObject* Item = dynamic_cast< TNameValueObject*>( Items->Objects[Index] );
		if( Item != NULL &&  Value.AnsiCompare( Item->GetValue() ) != 0  )
			Item->SetValue( Value );
	}
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::EditSection( int Index, const Types::TRect &Rect, const AnsiString& Name )
{
	if( FEdit->Tag != -1 )
		UpdateValue( FEdit->Tag, FEdit->Text );
	FEdit->BoundsRect = Rect;
	FEdit->Visible = true;
	FEdit->Text = Name;
	FEdit->Focused();
	FEdit->Tag = Index;
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::DrawItem(int Index, const Types::TRect &Rect, TOwnerDrawState State)
{
	TSectionObject* Section = dynamic_cast<TSectionObject*>( Items->Objects[Index] );
	TTextFormat Formats;

	Formats <<tfLeft<<tfVerticalCenter<<tfEndEllipsis;
	Canvas->Pen->Color  = clGray;
	Canvas->Pen->Style  = psInsideFrame;
	if( State.Contains( odSelected ) )
		Canvas->Font->Color = clNavy;
	else
		Canvas->Font->Color = clBlack;
	if( Section != NULL ) ///< Section name item
	{
		if( State.Contains( odSelected ) && FEditable )
		{
			EditSection( Index, Rect, Section->GetName() );
		}
        else
		{
			String NameStr( Section->GetName() );
			TRect  DrawRect( Rect );

            Canvas->Brush->Color = clSilver;;
			Canvas->FillRect( DrawRect );
			Canvas->TextRect( DrawRect , NameStr, Formats  );
        }
	}
    else  ///< Name-Value pair item
    {
        TNameValueObject* Item = dynamic_cast<TNameValueObject*>( Items->Objects[Index] );
        if( Item!= NULL )
        {
			int    MidX = Rect.Left + Rect.Width() / 2;
			int    HalfW = Rect.Width() / 2;
			String NameStr( Item->GetName() );
			String ValueStr(Item->GetValue());
			TRect  DrawRect( Rect );
			TRect  EditRect( Rect );

			DrawRect.Right = DrawRect.Right - HalfW;
			EditRect.left  = EditRect.Left + HalfW;
			if( State.Contains( odSelected ) )
				Canvas->Brush->Color = clWhite;
			else
				Canvas->Brush->Color = clBtnFace;
			Canvas->FillRect( DrawRect );
			Canvas->TextRect( DrawRect, NameStr, Formats );
			Canvas->FillRect( EditRect );
			if( State.Contains( odSelected ) && FEditable )
			{
				EditSection( Index, EditRect, Item->GetValue() );
            }
            else
            {
                Canvas->Font->Color = clNavy;
				Canvas->TextRect( EditRect, ValueStr, Formats );
            }
            Canvas->MoveTo( MidX -1 , Rect.Top );
            Canvas->LineTo( MidX -1 , Rect.Bottom );
        }
    }
    Canvas->MoveTo( Rect.Left , Rect.Bottom -1 );
    Canvas->LineTo( Rect.Right , Rect.Bottom - 1 );
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::Load( const AnsiString& FileName )
{
    ClearItems( );
    FFileName = FileName.c_str();
    UFC::UiniFile IniFile( FFileName );
    UFC::Section* Section;

    for( int i=0; i< IniFile.SectionCount(); i ++ )
    {
		Section = IniFile.GetSection( i );
		Items->AddObject( Section->GetSectionName().c_str(), new TSectionObject( Section->GetSectionName().c_str() ));
		for( int j = 0; j < Section->ItemCount(); j++ )
        {
			UFC::AnsiString Name,Value;

            if( Section->GetNameValue( j, Name, Value ) == TRUE )
				Items->AddObject( Name.c_str(), new TNameValueObject( Name.c_str(),Value.c_str()));
		}
	}
	FInitItemCount = Items->Count;
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::Load( UFC::UiniFile& IniFile )
{
	ClearItems( );
	UFC::Section* Section;

	for( int i=0; i< IniFile.SectionCount(); i ++ )
	{
		Section = IniFile.GetSection( i );
		Items->AddObject( Section->GetSectionName().c_str(), new TSectionObject( Section->GetSectionName().c_str() ));
		for( int j = 0; j < Section->ItemCount(); j++ )
		{
			UFC::AnsiString Name,Value;

			if( Section->GetNameValue( j, Name, Value ) == TRUE )
				Items->AddObject( Name.c_str(), new TNameValueObject( Name.c_str(),Value.c_str()));
        }
	}
	FInitItemCount = Items->Count;
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::ClearItems( void )
{
    for( int i = 0;i < Items->Count; i++ )
         delete Items->Objects[i];
	TCustomListBox::Clear();
	FEdit->Visible = false;
	FEdit->Tag = -1;
}
//---------------------------------------------------------------------------
bool __fastcall TIniEditBox::Modified( void )
{
	if( FInitItemCount != Items->Count )
		return true;
	if( FEdit->Tag != -1 )
		UpdateValue( FEdit->Tag, FEdit->Text );
    for( int i = 0;i < Items->Count; i++ )
    {
         if( ((TiniObject*)Items->Objects[i])->IsDirty() == true )
             return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::Save(  const AnsiString& FileName )
{
    UFC::UiniFile     IniFile( FileName.c_str(), true );
    UFC::Section*     NewSection = NULL;
    TSectionObject*   Section;
    TNameValueObject* NameValPair;
    AnsiString        SectionName;

    IniFile.Clear();
    if( FEdit->Tag != -1 )
		UpdateValue( FEdit->Tag, FEdit->Text );
    for( int i = 0;i < Items->Count; i++ )
    {
        if( (Section = dynamic_cast<TSectionObject*>( Items->Objects[i] ))!=NULL )
        {
            NewSection  = new UFC::Section( Section->GetName().c_str() );
            IniFile.AddSection( NewSection );
        }
        else if( (NameValPair = dynamic_cast<TNameValueObject*>( Items->Objects[i] )) != NULL )
        {
            if( NewSection )
                NewSection->SetValue( NameValPair->GetName().c_str(), NameValPair->GetValue().c_str() );
        }
    }
	IniFile.Save();
	FInitItemCount = Items->Count;
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::Save( UFC::UiniFile& IniFile )
{
	TSectionObject*   Section;
	TNameValueObject* NameValPair;
	UFC::AnsiString   SectionName;

	IniFile.Clear();
	if( FEdit->Tag != -1 )
		UpdateValue( FEdit->Tag, FEdit->Text );
	for( int i = 0;i < Items->Count; i++ )
	{
		if( (Section = dynamic_cast<TSectionObject*>( Items->Objects[i] ))!=NULL )
		{
			SectionName = Section->GetName().c_str();
		}
		else if( (NameValPair = dynamic_cast<TNameValueObject*>( Items->Objects[i] )) != NULL )
		{
			IniFile.SetValue( SectionName, NameValPair->GetName().c_str(), NameValPair->GetValue().c_str() );
		}
	}
	FInitItemCount = Items->Count;
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::Save( void )
{
	Save( FFileName.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::SetEditable( bool Editable )
{
    FEditable = Editable;
    if( FEditable == false )
    {
        FEdit->Visible = false;
        if( FEdit->Tag != -1 )
        {
            UpdateValue( FEdit->Tag, FEdit->Text );
            FEdit->Tag = -1;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::Resize( void )
{
	Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TIniEditBox::MouseDown( Controls::TMouseButton Button, Classes::TShiftState Shift,int X,int Y)
{
	if( Button == mbRight && FEditable )
	{
		TPoint DownPoint = TPoint( X,Y );
		FSelectIndex = ItemAtPos( DownPoint, true );

		if( FSelectIndex != -1 )
		{
			TPoint PT = ClientToScreen( DownPoint );
			FMenu->Popup( PT.X,PT.Y );
		}
    }
}
//---------------------------------------------------------------------------
namespace Inieditbox
{
    void __fastcall PACKAGE Register()
    {
         TComponentClass classes[1] = {__classid(TIniEditBox)};
         RegisterComponents("MBus", classes, 0);
    }
}
//---------------------------------------------------------------------------
