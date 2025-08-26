//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "GroupServer.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TServerGroupForm *ServerGroupForm;
//---------------------------------------------------------------------------
__fastcall TServerGroupForm::TServerGroupForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TServerGroupForm::LoadFromListView( const String& Names, TListView *Servers  )
{
	TListItem*   NewItem;
	TListItem*   OrigItem;
	TStringList* NameStrings = new TStringList();

	NameStrings->Delimiter = ':';
	NameStrings->DelimitedText = Names;
	GroupNameEdit->Text = NameStrings->Strings[0];
	GroupNameEdit->Enabled = false;
	NameStrings->Delete( 0 );

	GroupServerListView->Items->BeginUpdate();
	ServerListView->Items->BeginUpdate();
	GroupServerListView->Items->Clear();
	ServerListView->Items->Clear();
	for( int i = 0; i < NameStrings->Count; i ++ )
	{
		 NewItem = GroupServerListView->Items->Add( );
		 NewItem->Caption = NameStrings->Strings[i];
		 NewItem->ImageIndex = -1;
	}
	for( int i = 0; i < Servers->Items->Count; i ++ )
	{
		OrigItem = Servers->Items->Item[ i ];
		if( OrigItem->ImageIndex <= 4 )///< Not Group
		{
			NewItem = GroupServerListView->FindCaption(0,OrigItem->SubItems->Strings[ 0 ],false,true,true);
			if( NewItem == NULL )
				NewItem  = ServerListView->Items->Add( );
			NewItem->ImageIndex = OrigItem->ImageIndex;
			NewItem->Caption = OrigItem->Caption;
			NewItem->SubItems->Add( OrigItem->SubItems->Strings[ 0 ] );
			NewItem->SubItems->Add( OrigItem->SubItems->Strings[ 1 ] );
		}
	}
	for( int i = GroupServerListView->Items->Count-1; i >= 0; i-- )
	{
		OrigItem = GroupServerListView->Items->Item[ i ];
		if( OrigItem->ImageIndex == -1 )
			GroupServerListView->Items->Delete( i );
    }
	ServerListView->Items->EndUpdate();
	GroupServerListView->Items->EndUpdate();
	delete	NameStrings;
}
//---------------------------------------------------------------------------
void __fastcall TServerGroupForm::LoadFromListView( TListView *Servers )
{
	TListItem* NewItem;
	TListItem* OrigItem;

	GroupServerListView->Items->Clear();
	ServerListView->Items->BeginUpdate();
	ServerListView->Items->Clear();
	for( int i = 0; i < Servers->Items->Count; i ++ )
	{
		OrigItem = Servers->Items->Item[i];
		if( OrigItem->ImageIndex <= 4 )///< Not Group
		{
			NewItem  = ServerListView->Items->Add( );
			NewItem->ImageIndex = OrigItem->ImageIndex;
			NewItem->Caption = OrigItem->Caption;
			NewItem->SubItems->Add( OrigItem->SubItems->Strings[ 0 ] );
			NewItem->SubItems->Add( OrigItem->SubItems->Strings[ 1 ] );
		}
	}
	ServerListView->Items->EndUpdate();
	GroupNameEdit->Enabled = true;
	for( int i = 1;;i++)
	{
		String GroupName;

		GroupName.printf( L"Group%d", i );
		if( ServerListView->FindCaption(0,GroupName,false,true,true ) == NULL )
		{
			GroupNameEdit->Text = GroupName;
			break;
        }
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerGroupForm::AddBtnClick(TObject *Sender)
{
	TListItem* MoveItem = ServerListView->Selected;

	if( MoveItem != NULL )
	{
		TListItem* NewItem;
		NewItem  = GroupServerListView->Items->Add( );
		NewItem->ImageIndex = MoveItem->ImageIndex;
		NewItem->Caption = MoveItem->Caption;
		NewItem->SubItems->Add( MoveItem->SubItems->Strings[ 0 ] );
		NewItem->SubItems->Add( MoveItem->SubItems->Strings[ 1 ] );
		ServerListView->Items->Delete( MoveItem->Index );
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerGroupForm::DelBtnClick(TObject *Sender)
{
	TListItem* MoveItem = GroupServerListView->Selected;

	if( MoveItem != NULL )
	{
		TListItem* NewItem;
		NewItem  = ServerListView->Items->Add( );
		NewItem->ImageIndex = MoveItem->ImageIndex;
		NewItem->Caption = MoveItem->Caption;
		NewItem->SubItems->Add( MoveItem->SubItems->Strings[ 0 ] );
		NewItem->SubItems->Add( MoveItem->SubItems->Strings[ 1 ] );
		GroupServerListView->Items->Delete( MoveItem->Index );
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerGroupForm::BitBtn1Click(TObject *Sender)
{
	if( GroupNameEdit->Text.Length() < 1 )
	{
		ShowMessage( Scstrings_MAIN_MISS_GROUP_NAME );
		ModalResult = mrNone;
		return;
	}
	if( GroupServerListView->Items->Count <= 1 )
	{
		ShowMessage( Scstrings_MAIN_NEED_2_SERVER_IN_GROUP );
		ModalResult = mrNone;
		return;
	}
}
//---------------------------------------------------------------------------
String __fastcall TServerGroupForm::GetServersNameString( void )
{
	String     Names;
	TListItem* Server;

	Names.printf( L"%s:", GroupNameEdit->Text );
	for( int i = 0; i < GroupServerListView->Items->Count; i ++ )
	{
		Server = GroupServerListView->Items->Item[i];
		Names += Server->SubItems->Strings[0];
		if( i < GroupServerListView->Items->Count - 1  )
			Names += L":";
	}
	return Names;
}
//---------------------------------------------------------------------------
String __fastcall TServerGroupForm::GetServersIPString( void )
{
	String     IPs;
	TListItem* Server;

	for( int i = 0; i < GroupServerListView->Items->Count; i ++ )
	{
		Server = GroupServerListView->Items->Item[i];
		IPs += Server->SubItems->Strings[1];
		if( i < GroupServerListView->Items->Count - 1  )
			IPs += L":";
	}
	return IPs;
}
//---------------------------------------------------------------------------
void __fastcall TServerGroupForm::UpBitBtnClick(TObject *Sender)
{
	TListItem* MoveItem;
	TListItem* UpperItem;
	TListView* ListView;

	if( GroupServerListView->Selected != NULL )
	{
		MoveItem = GroupServerListView->Selected;
		ListView = GroupServerListView;
	}
	else if( ServerListView->Selected != NULL )
	{
		MoveItem = ServerListView->Selected;
		ListView = ServerListView;
	}
	else
		return;
	if( MoveItem->Index > 0 )
	{
		int    ImgIndexTmp;
		String CaptionTmp,ValueTmp;

		UpperItem = ListView->Items->Item[ MoveItem->Index -1 ];
		ListView->Items->BeginUpdate();
		CaptionTmp = MoveItem->Caption;
		MoveItem->Caption = UpperItem->Caption;
		UpperItem->Caption = CaptionTmp;
		ImgIndexTmp = MoveItem->ImageIndex;
		MoveItem->ImageIndex = UpperItem->ImageIndex;
		UpperItem->ImageIndex = ImgIndexTmp;
		ValueTmp = MoveItem->SubItems->Strings[0];
		MoveItem->SubItems->Strings[0] = UpperItem->SubItems->Strings[0];
		UpperItem->SubItems->Strings[0] = ValueTmp;
		ValueTmp = MoveItem->SubItems->Strings[1];
		MoveItem->SubItems->Strings[1] = UpperItem->SubItems->Strings[1];
		UpperItem->SubItems->Strings[1] = ValueTmp;
		ListView->Selected = UpperItem;
		ListView->Items->EndUpdate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerGroupForm::DownBitBtnClick(TObject *Sender)
{
	TListItem* MoveItem;
	TListItem* LowerItem;
	TListView* ListView;

	if( GroupServerListView->Selected != NULL )
	{
		MoveItem = GroupServerListView->Selected;
		ListView = GroupServerListView;
	}
	else if( ServerListView->Selected != NULL )
	{
		MoveItem = ServerListView->Selected;
		ListView = ServerListView;
	}
	else
		return;
	if( MoveItem->Index < ListView->Items->Count - 1 )
	{
		int    ImgIndexTmp;
		String CaptionTmp,ValueTmp;

		LowerItem = ListView->Items->Item[ MoveItem->Index + 1 ];
		ListView->Items->BeginUpdate();
		///< Swap Caption.
		CaptionTmp = MoveItem->Caption;
		MoveItem->Caption = LowerItem->Caption;
		LowerItem->Caption = CaptionTmp;
		///< Swap ImageIndex.
		ImgIndexTmp = MoveItem->ImageIndex;
		MoveItem->ImageIndex = LowerItem->ImageIndex;
		LowerItem->ImageIndex = ImgIndexTmp;
		///< Swap Server Name.
		ValueTmp = MoveItem->SubItems->Strings[0];
		MoveItem->SubItems->Strings[0] = LowerItem->SubItems->Strings[0];
		LowerItem->SubItems->Strings[0] = ValueTmp;
		///< Swap Server IP.
		ValueTmp = MoveItem->SubItems->Strings[1];
		MoveItem->SubItems->Strings[1] = LowerItem->SubItems->Strings[1];
		LowerItem->SubItems->Strings[1] = ValueTmp;
		///< Set sleclted item.
		ListView->Selected = LowerItem;
		ListView->Items->EndUpdate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerGroupForm::GroupServerListViewSelectItem(TObject *Sender, TListItem *Item,
		  bool Selected)
{
	if( Selected == true )
		ServerListView->Selected = NULL;
}
//---------------------------------------------------------------------------

void __fastcall TServerGroupForm::ServerListViewSelectItem(TObject *Sender, TListItem *Item,
		  bool Selected)
{
	if( Selected == true )
		GroupServerListView->Selected = NULL;
}
//---------------------------------------------------------------------------

