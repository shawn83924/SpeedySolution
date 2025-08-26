//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "SpeedyServerForm.h"
#include "GroupServer.h"
#include "SCStrings.hpp"
#include <Registry.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TServerForm *ServerForm;
//---------------------------------------------------------------------------
extern bool   gAutoLogon;
extern String gIP;
extern AnsiString CurrentDir;
//---------------------------------------------------------------------------
__fastcall TServerForm::TServerForm(TComponent* Owner)
:TForm(Owner)
,FSortIndex( 0 )
{
	FSortDir[0] = 1;
	FSortDir[1] = 1;
	FSortDir[2] = 1;
	FHeaderStrings[0] = L"OS";
	FHeaderStrings[1] = Scstrings_MAIN_HOSTNAME;
	FHeaderStrings[2] = L"IP ";
	FServerNames = new TStringList();
	FServerIPs   = new TStringList();
}
//---------------------------------------------------------------------------
bool __fastcall TServerForm::IsValidIP( const String& IP )
{
	if( IP.Length() < 7 || IP.Length() > 15 )
		return false;
	TStringList* Strs = new TStringList();

	Strs->Delimiter = '.';
	Strs->DelimitedText = IP;
	if( Strs->Count == 4 )
	{
        try
        {
            for( int i = 0; i < 4; i++ ) ///< Check each number.
            {
                int number  = Strs->Strings[ i ].ToInt();
                if( number< 0 || number > 255 )
                {
                    delete Strs;
                    return false;
                }
            }
            delete Strs;
            return true;
        }
        catch( EConvertError &e )
        {
            delete Strs;
            return false;
        }
    }
    delete Strs;
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::AddServer( int OS, const AnsiString& NameString, const AnsiString& IP )
{
	 TStringList* Names = new TStringList();
	 String       ServerName;

	 Names->Delimiter = ':';
	 Names->DelimitedText = NameString;
	 if( Names->Count == 1 ) ///< Single server.
		 ServerName = NameString;
	 else
		 ServerName = Names->Strings[ 0 ];
	 delete	Names;
	 for( int i = 0; i < ServerListView->Items->Count; i++ )
	 {
		  if( ServerListView->Items->Item[i]->SubItems->Strings[0].Compare( ServerName )==0 )
		  {   ///< Update
			  ServerListView->Items->Item[i]->ImageIndex = OS;
			  ServerListView->Items->Item[i]->Caption = NameString.c_str();
			  ServerListView->Items->Item[i]->SubItems->Strings[1] = IP;
			  return;
		  }
	 }
	 TListItem* NewHost = ServerListView->Items->Add();
	 NewHost->ImageIndex = OS;
	 NewHost->Caption = NameString.c_str();
	 NewHost->SubItems->Add( ServerName );
	 NewHost->SubItems->Add( IP.c_str() );
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::SaveServers( void )
{
	AnsiString ValueName,Body,Host,IP;

	if( ServerListView->Items->Count > 0 )
	{
		TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");

		iniFile->WriteInteger( L"ServerList", L"Count", ServerListView->Items->Count );

		for( int i = 1; i <= ServerListView->Items->Count; i++ )
		{
			TListItem* Item = ServerListView->Items->Item[ i - 1 ];
			ValueName.printf( "Item%d", i );
			Host = Item->Caption;
			IP   = Item->SubItems->Strings[1];
			Body.printf("%d,%s,%s",Item->ImageIndex, Host.c_str(), IP.c_str() );
			iniFile->WriteString( L"ServerList", ValueName, Body );
		}
		for( int i = ServerListView->Items->Count+1; ;i++ )
		{
			ValueName.printf( "Item%d", i );

			if( iniFile->ValueExists( L"ServerList", ValueName ) == true )
				iniFile->DeleteKey(  L"ServerList", ValueName );
			else
				break;
		}
		if( IsValidIP( IPLabeledEdit->Text ) == true  )
			iniFile->WriteString( L"ServerList", L"LastIP", IPLabeledEdit->Text );
		delete iniFile;
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::LoadServers( void )
{
	TIniFile* iniFile = new TIniFile( CurrentDir +"\\SpeedyClientCfg.ini");

	if( iniFile->ValueExists( L"ServerList", L"LastIP" ) == true )
		IPLabeledEdit->Text = iniFile->ReadString( L"ServerList", L"LastIP", "127.0.0.1" );
	if( iniFile->ValueExists( L"ServerList", "Count") == true )
	{
		AnsiString ValueName;
		int ServerCount = iniFile->ReadInteger( L"ServerList", L"Count", 0 );

		ServerListView->Items->Clear();
		for( int i = 1; i <= ServerCount; i++ )
		{
			ValueName.printf("Item%d", i );
			if( iniFile->ValueExists( L"ServerList", ValueName ) == true )
			{
				 TStringList* Str = new TStringList();
				 Str->Delimiter = ',';
				 Str->DelimitedText = iniFile->ReadString( L"ServerList", ValueName, "" );
				 if( Str->Count == 3 )
					 AddServer( Str->Strings[0].ToInt(), Str->Strings[1], Str->Strings[2] );
				 delete	Str;
			}
		}
	}
	else
	{
		TRegistry *Reg = new TRegistry();

		Reg->RootKey = HKEY_LOCAL_MACHINE;
		if( Reg->OpenKey( "Software\\MDBS\\SpeedyCenter\\ServerList" ,true ) == true )
		{
			try
			{
				if( Reg->ValueExists("LastIP"))
					IPLabeledEdit->Text = Reg->ReadString( "LastIP" );
				if( Reg->ValueExists("Count"))
				{
					AnsiString ValueName;
					int ServerCount = Reg->ReadInteger("Count");
					ServerListView->Items->Clear();

					for( int i = 1; i <= ServerCount; i++ )
					{
						ValueName.printf("Item%d", i );
						if( Reg->ValueExists( ValueName ) )
						{
							 TStringList* Str = new TStringList();
							 Str->Delimiter = ',';
							 Str->DelimitedText = Reg->ReadString( ValueName );
							 if( Str->Count == 3 )
								 AddServer( Str->Strings[0].ToInt(), Str->Strings[1], Str->Strings[2] );
							 delete	Str;
						}
					}
				}
			}
			catch(...){}
			Reg->CloseKey();
		}
		delete Reg;
	}
	delete iniFile;
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::ServerListViewClick(TObject *Sender)
{
	TListItem* Item = ServerListView->Selected;
	if( Item != NULL )
	{
		if(  Item->ImageIndex == 5 ) ///< Group
			IPLabeledEdit->Text = Item->SubItems->Strings[0];
		else
			IPLabeledEdit->Text = Item->SubItems->Strings[1];
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::OpenGroupItem( TListItem* Item )
{
	FServerNames->Clear();
	FServerIPs->Clear();
	FServerIPs->Delimiter = ':';
	FServerIPs->DelimitedText = Item->SubItems->Strings[1];
	FServerNames->Delimiter = ':';
	FServerNames->DelimitedText = Item->Caption;
	FServerNames->Delete( 0 );
	IPLabeledEdit->Text = FServerIPs->Strings[1];
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::OpenItem( TListItem* Item )
{
	FServerNames->Clear();
	FServerIPs->Clear();
	FServerIPs->Add( Item->SubItems->Strings[1] );
	FServerNames->Add( Item->SubItems->Strings[0] );
	IPLabeledEdit->Text = Item->SubItems->Strings[1];
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::OpenItem( const String& IP )
{
	FServerNames->Clear();
	FServerIPs->Clear();
	FServerIPs->Add( IP);
	FServerNames->Add( L"UnknownHost" );
}
//---------------------------------------------------------------------------
TListItem* __fastcall TServerForm::FindGroupItemByName( const String& Name )
{
	TListItem* Item;

	for( int i = 0; i < ServerListView->Items->Count; i++ )
	{
		Item = ServerListView->Items->Item[i];
		String GroupName( Item->Caption.c_str(), Name.Length() );

		if( GroupName.Compare( Name ) == 0 && Item->ImageIndex == 5 )
			return Item;
	}
	return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::ServerListViewDblClick(TObject *Sender)
{
	TListItem* Item = NULL;

	if( ServerListView->Focused() == true )
		Item = ServerListView->Selected;

	if( Item != NULL ) ///< A server item selected.
	{
		if( Item->ImageIndex == 5 )
			OpenGroupItem( Item ); ///< Server Group
		else
			OpenItem( Item );      ///< Single server
		ModalResult = mrOk;
	}
	else
	{
		if( IsValidIP( IPLabeledEdit->Text ) == true ) ///< Open new server with IP
		{
			OpenItem( IPLabeledEdit->Text );
			ModalResult = mrOk;
		}
		else
		{
			if( (Item = FindGroupItemByName( IPLabeledEdit->Text )) != NULL ) ///< It's a group.
			{
				OpenGroupItem( Item );
				ModalResult = mrOk;
			}
			else ///< Not a valid IP and not a group name.
				ShowMessage( Scstrings_MAIN_PVC_SETTING_WRONG_IP_ADDRESS );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::FormShow(TObject *Sender)
{
	LoadServers(  );
	if( gAutoLogon == true )
	{
		IPLabeledEdit->Text = gIP;
		AutoConnectTimer->Enabled = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::ItemDeleteClick(TObject *Sender)
{
	TListItem* Item = ServerListView->Selected;
	if( Item != NULL )
	{
		ServerListView->Items->Delete( Item->Index );
		SaveServers();
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::ServerListViewColumnClick(TObject *Sender, TListColumn *Column)
{
	FSortIndex = Column->Index;
	FSortDir[ FSortIndex ] = -1 * FSortDir[ FSortIndex ];
	ServerListView->AlphaSort();

	for( int i = 0; i < ServerListView->Columns->Count; i ++ )
	{
		if( i == FSortIndex )
		{
			if( FSortDir[ i ] < 0 )
				Column->Caption = FHeaderStrings[i] + L"¡¶";
			else
				Column->Caption = FHeaderStrings[i] + L"¡¿";
		}
		else
			ServerListView->Columns->Items[i]->Caption = FHeaderStrings[i];
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::ServerListViewCompare(TObject *Sender, TListItem *Item1,
		  TListItem *Item2, int Data, int &Compare)
{
	if( FSortIndex == 0 )
	{
		if( Item1->ImageIndex >  Item2->ImageIndex )
			Compare = 1 * FSortDir[ 0 ];
		else if( Item1->ImageIndex <  Item2->ImageIndex )
			Compare = -1 * FSortDir[ 0 ];
		else
			Compare = 0;
	}
	else if( FSortIndex == 1 )
		Compare = Item1->SubItems->Strings[0].Compare( Item2->SubItems->Strings[0] )* FSortDir[ 1 ];
	else
		Compare = Item1->SubItems->Strings[1].Compare( Item2->SubItems->Strings[1] )* FSortDir[ 2 ];
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::ItemNewGroupClick(TObject *Sender)
{
	ServerGroupForm->LoadFromListView( ServerListView );
	if( ServerGroupForm->ShowModal() == mrOk )
	{
		AddServer( 5,
				   ServerGroupForm->GetServersNameString(),
				   ServerGroupForm->GetServersIPString()  );
		SaveServers();
	}
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::ItemModifyGroupClick(TObject *Sender)
{
	TListItem* Item = ServerListView->Selected;
	if( Item != NULL )
	{
		ServerGroupForm->LoadFromListView( Item->Caption, ServerListView );
		if( ServerGroupForm->ShowModal() == mrOk )
		{
			AddServer( 5,
					   ServerGroupForm->GetServersNameString(),
					   ServerGroupForm->GetServersIPString()  );
			SaveServers();
		}
    }
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::PopupMenuPopup(TObject *Sender)
{
	TListItem* Item = ServerListView->Selected;
	if( Item != NULL )
	{
		ItemDelete->Visible = true;
		if( Item->ImageIndex == 5 ) ///< group
			ItemModifyGroup->Visible = true;
		else
			ItemModifyGroup->Visible = false;
	}
	else
		ItemDelete->Visible = false;
}
//---------------------------------------------------------------------------
int __fastcall TServerForm::GetServerCount( void )
{
	return FServerNames->Count;
}
//---------------------------------------------------------------------------
bool __fastcall TServerForm::PopServerNameIP( String& Name, String& IP )
{
	if( FServerNames->Count > 0 )
	{
		Name = FServerNames->Strings[ 0 ];
		FServerNames->Delete( 0 );
		IP = FServerIPs->Strings[ 0 ];
		FServerIPs->Delete( 0 );
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TServerForm::AutoConnectTimerTimer(TObject *Sender)
{
	AutoConnectTimer->Enabled = false;
	ServerListViewDblClick( NULL );
}
//---------------------------------------------------------------------------

