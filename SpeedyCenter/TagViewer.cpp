//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "TagViewer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTagForm *TagForm;
//---------------------------------------------------------------------------
extern AnsiString CurrentDir;
//---------------------------------------------------------------------------
__fastcall TTagForm::TTagForm(TComponent* Owner)
    : TForm(Owner),FColumnToSort(0),FSortDir(1)
{
	AnsiString TagNames  = CurrentDir + "\\Data\\TagNames.ini";
	AnsiString TagValues = CurrentDir + "\\Data\\TagValues.ini";
	if( DirectoryExists( CurrentDir + "\\Data\\" ) == false )
		ForceDirectories( CurrentDir + "\\Data\\" );
	FTagToName = new UFC::UiniFile( TagNames.c_str(), TRUE );
    FTagValues = new UFC::UiniFile( TagValues.c_str(), TRUE );
}
//---------------------------------------------------------------------------
void __fastcall TTagForm::ReplaceChar( const AnsiString& Msg, char From, char To )
{
   char *CharPtr = Msg.c_str();
   for( register int i = 0;i < Msg.Length(); i++ )
   {
       if( CharPtr[i] == From )
           CharPtr[i] = To;
   }
}
//---------------------------------------------------------------------------
void __fastcall TTagForm::SetFIXMessage( const AnsiString& Msg )
{
    TStringList* Strs = new TStringList();

    Strs->Delimiter = ' ';
    Strs->DelimitedText = Msg;
	TagListView->Items->BeginUpdate();
    TagListView->Clear();
    for( int i = 0 ;i < Strs->Count; i ++ )
    {
        int Pos = Strs->Strings[ i ].Pos( "=" );
        if( Pos != 0 )
        {
            AnsiString Tag   =  Strs->Strings[ i ].SubString( 1, Pos-1 );
            AnsiString Value =  Strs->Strings[ i ].SubString( Pos+1, Strs->Strings[ i ].Length()-Pos );
            UFC::AnsiString Val;
            TListItem* NewTag = TagListView->Items->Add();

            NewTag->Caption = Tag;
			if( FTagToName->GetValue( "Tags", Tag.c_str(), Val ) )
				NewTag->SubItems->Add( Val.c_str() );
			else
				NewTag->SubItems->Add( "" );
			if( Value.AnsiPos("_")!=0 )
				ReplaceChar( Value, '_',' ' );
			NewTag->SubItems->Add( Value );
			if( FTagValues->GetValue( Tag.c_str(), Value.c_str(), Val ) )
			{
				NewTag->SubItems->Add( Val.c_str() );
				if( Tag.ToInt() == 35 )
					MsgTypeLabel->Caption = Val.c_str();
			}
			else
			{
				NewTag->SubItems->Add( "" );
				if( Tag.ToInt() == 35 )
					MsgTypeLabel->Caption = Value;
			}
		}
	}
    TagListView->Items->EndUpdate();
    delete Strs;
}
//---------------------------------------------------------------------------
void __fastcall TTagForm::SetTWSEFIXMessage( const AnsiString& Msg )
{
	SetFIXMessage( Msg.c_str( ) + 18 ); ///< Skip "hh:mm:ss.mmm Recv:"
	if( Msg[14] == 'R' )
		TagListView->Font->Color  = clLime;  ///< Receive
	else
		TagListView->Font->Color  = clWhite; ///< Send
}
//---------------------------------------------------------------------------
void __fastcall TTagForm::SetTMPMessage( const AnsiString& Msg )
{
	TStringList* Strs = new TStringList();

	Strs->Delimiter = ' ';
	Strs->DelimitedText = Msg.c_str( ) + 18; ///< Skip "hh:mm:ss.mmm Recv:"
	if( Msg[ 14 ] == 'R' )
		TagListView->Font->Color  = clLime;  ///< Receive
	else
		TagListView->Font->Color  = clWhite; ///< Send
	TagListView->Items->BeginUpdate();
	TagListView->Clear();
	for( int i = 0 ;i < Strs->Count; i ++ )
	{
		int Pos = Strs->Strings[ i ].Pos( "=" );
		if( Pos != 0 )
		{
			AnsiString      Tag   = Strs->Strings[ i ].SubString( 1, Pos-1 );
			AnsiString      Value = Strs->Strings[ i ].SubString( Pos+1, Strs->Strings[ i ].Length()-Pos );
			TListItem*      NewTag = TagListView->Items->Add();
			UFC::AnsiString Val;

			NewTag->SubItems->Add( Tag );
			NewTag->SubItems->Add( Value );
			if( Tag == "MessageType" )
			{
				if( FTagValues->GetValue( Tag.c_str(), Value.c_str(), Val ) )
					MsgTypeLabel->Caption = Val.c_str();
			}

		}
	}
    TagListView->Items->EndUpdate();
    delete Strs;
}
//---------------------------------------------------------------------------
void __fastcall TTagForm::TagListViewColumnClick(TObject *Sender,
      TListColumn *Column)
{
    FColumnToSort = Column->Index;
    FSortDir *= -1;
    TagListView->AlphaSort();
}
//---------------------------------------------------------------------------
void __fastcall TTagForm::TagListViewCompare(TObject *Sender,
      TListItem *Item1, TListItem *Item2, int Data, int &Compare)
{
    if( FColumnToSort == 0 )
    {
        int val1 = Item1->Caption.ToInt();
        int val2 = Item2->Caption.ToInt();
        if( val1 == val2 )
            Compare = 0;
        else if( val1 > val2 )
            Compare = -FSortDir;
        else
            Compare = FSortDir;
    }
    else
    {
        int ix = FColumnToSort - 1;
        Compare = FSortDir*CompareText(Item1->SubItems->Strings[ix], Item2->SubItems->Strings[ix]);
    }
}
//---------------------------------------------------------------------------

