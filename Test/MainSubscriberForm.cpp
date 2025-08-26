//---------------------------------------------------------------------------
#include <vcl.h>
#include <Registry.hpp>
#pragma hdrstop
#include "MainSubscriberForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MessageObject"
#pragma link "Subscriber"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TSubscriberForm *SubscriberForm;
//---------------------------------------------------------------------------
//
//  Class TSubscriberForm
//
//---------------------------------------------------------------------------
__fastcall TSubscriberForm::TSubscriberForm(TComponent* Owner)
:TForm( Owner )
,FMsgCount( 0 )
,FConnectHost( "" )
{
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::ConnectButtonClick(TObject *Sender)
{
	try
	{
		int Port = PortEdit->Text.ToInt();
		if( Port < 1000  )
		{
			ShowMessage("無效的Port number.");
			return;
		}
	}
	catch( ... )
	{
		ShowMessage("無效的Port number.");
		return;
	}
	MessageObject->Host = IPEdit->Text;
	Subscriber->Subject = SubjectEdit->Text;
	Subscriber->Key     = KeyEdit->Text;
	MessageObject->Port = PortEdit->Text.ToInt();
	MessageObject->Connect();
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::DisconnectButtonClick(TObject *Sender)
{
	MessageObject->Disconnect();
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MessageObjectConnected(TObject *Sender)
{
	ConnectButton->Enabled = false;
	IPEdit->Enabled = false;
	PortEdit->Enabled = false;
	SubjectEdit->Enabled = false;
	KeyEdit->Enabled = false;
	DisconnectButton->Enabled = true;
	Timer->Enabled = true;
	StatusBar->Panels->Items[1]->Text = "Connected";
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MessageObjectDisconnected(TObject *Sender)
{
	ConnectButton->Enabled = true;
	IPEdit->Enabled = true;
	PortEdit->Enabled = true;
	SubjectEdit->Enabled = true;
	KeyEdit->Enabled = true;
	DisconnectButton->Enabled = false;
	Timer->Enabled = false;
	StatusBar->Panels->Items[1]->Text = "Disconnected";
	MBusTreeView->Items->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MessageObjectError(TObject *Sender)
{
	ConnectButton->Enabled = true;
	IPEdit->Enabled = true;
	PortEdit->Enabled = true;
	SubjectEdit->Enabled = true;
	KeyEdit->Enabled = true;
	DisconnectButton->Enabled = false;
	Timer->Enabled = false;
	StatusBar->Panels->Items[1]->Text = "Socket error";
	MBusTreeView->Items->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::FormShow(TObject *Sender)
{
	TRegistry *Reg = new TRegistry();

	Reg->RootKey = HKEY_LOCAL_MACHINE;
	Reg->OpenKey( "Software\\MDBS\\WinSubscriber" ,true );
	if( Reg->ValueExists("IP"))
		IPEdit->Text = Reg->ReadString( "IP" );
	else
		IPEdit->Text = "127.0.0.1";
	if( Reg->ValueExists("Port"))
		PortEdit->Text = Reg->ReadString( "Port" );
	else
		PortEdit->Text = "12345";
	if( Reg->ValueExists("Subject"))
		SubjectEdit->Text = Reg->ReadString( "Subject" );
	else
		SubjectEdit->Text = "all";
	if( Reg->ValueExists("Key"))
		KeyEdit->Text = Reg->ReadString( "Key" );
	else
		KeyEdit->Text = "all";
	Reg->CloseKey();
	delete Reg;
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::FormClose(TObject *Sender,
	  TCloseAction &Action)
{
	if( MessageObject->IsConnected() )
	{
		ShowMessage("Disconnect it before close.");
		Action = caNone;
	}
	TRegistry *Reg = new TRegistry();

	Reg->RootKey = HKEY_LOCAL_MACHINE;
	Reg->OpenKey( "Software\\MDBS\\WinSubscriber" ,true );
	Reg->WriteString( "IP",      IPEdit->Text );
	Reg->WriteString( "PORT",    PortEdit->Text );
	Reg->WriteString( "Subject", SubjectEdit->Text );
	Reg->WriteString( "Key",     KeyEdit->Text  );
	Reg->CloseKey();
	delete Reg;
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::TimerTimer(TObject *Sender)
{
	static int PrevCount = 0;
	AnsiString Msg;
	Msg.printf( "Receive %d msg/sec", FMsgCount - PrevCount );
	PrevCount = FMsgCount;
	StatusBar->Panels->Items[0]->Text = Msg;
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::SubscriberMessage(const MString &Subject,
	  const MString &Key, MTree *Tree)
{
	FMsgCount++;
	if( DetailCheckBox->Checked == true )
	{
		SYSTEMTIME       SystemTime;
		AnsiString       NodeName,NodeString;
		UFC::AnsiString  BinString;
		MTreeEnumerator* pTreeEnum = Tree->newEnumerator();
		TTreeNode*       NewNode;
		MNode*           pNode;
		void*            pItem;

		GetLocalTime( &SystemTime );
		NodeName.printf( "%02d:%02d:%02d.%03d %s,%s", SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds,Subject.c_str(),Key.c_str() );
		NewNode = TreeView->Items->Add( TreeView->Items->GetFirstNode(), NodeName );
		pTreeEnum->reset();
		while( (pNode = (MTree*)pTreeEnum->next()) != NULL )
		{
			MDataType mType = pNode->getType();
			MString   name  = pNode->getName();
			pItem           = (void*)pNode->getData();
			switch( mType )
			{
						case M_STRING: NodeString.printf( "[String] %s:%s", name.c_str(),(char*)pItem );
									   break;
						case M_BOOLEAN:NodeString.printf( "[Boolean] %s:%s", name.c_str(),(*((int*)pItem)==0?"False":"True") );
									   break;
						case M_INT:    NodeString.printf( "[Int32] %s:%d", name.c_str(),*((int*)pItem) );
									   break;
						case M_UINT:   NodeString.printf( "[UInt32] %s:%d", name.c_str(),*((unsigned int*)pItem) );
									   break;
						case M_SHORT:  NodeString.printf( "[Int16]%s:%d", name.c_str(),*((short*)pItem) );
									   break;
						case M_USHORT: NodeString.printf( "[UInt16]%s:%d", name.c_str(),*((unsigned short*)pItem) );
									   break;
						case M_LONG:   NodeString.printf( "[Long]%s:%d", name.c_str(),*((long*)pItem) );
									   break;
						case M_ULONG:  NodeString.printf( "[ULong]%s:%d", name.c_str(),*((unsigned long*)pItem) );
									   break;
						case M_FLOAT:  NodeString.printf( "[Float]%s:%f", name.c_str(),*((float*)pItem) );
									   break;
						case M_DOUBLE: NodeString.printf( "[Double]%s:%f", name.c_str(),*((double*)pItem) );
									   break;
						case M_ROWDATA:UFC::BinaryToHexString( (const UFCType::UInt8*)pItem,pNode->getSize(),BinString );
									   NodeString.printf( "[Row Data]%s:0x%s", name.c_str(), BinString.c_str() );
									   break;
						default:       printf( "Unknown Data Type:%d.\n", mType ) ;
									   break;
			}
			TreeView->Items->AddChild( NewNode, NodeString.c_str());
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::ClrButtonClick(TObject *Sender)
{
	TreeView->Items->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MessageObjectMBusConnected(const MString &Hostname,
		  bool IsTheTirstOne)
{
	UFC::AnsiString Msg;

	FConnectHost = Hostname;
	if( IsTheTirstOne == true )
		Msg.Printf( "Is the 1th WinSubscriber connected to %s", Hostname.c_str());
	else
		Msg.Printf( "Other WinSubscriber already connected to %s", Hostname.c_str());
	StatusBar->Panels->Items[1]->Text = Msg.c_str();
	MBusTimer->Enabled = true;
//	this->MonitorTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MessageObjectProcessList( const MString &Hostname, TStringList* Processs)
{
	TTreeNode* MBusNode = FindMBusNode( Hostname );
	TTreeNode* ProcessNode;
	if( MBusNode != NULL )
	{
		for( int i=0; i < Processs->Count; i ++ )
		{
			System::UnicodeString ProcessName( Processs->Strings[i] );
			if( ( ProcessNode = FindProcessNode( MBusNode, ProcessName.t_str()) ) == NULL )
				ProcessNode = MBusTreeView->Items->AddChild( MBusNode, Processs->Strings[i] );
			ProcessNode->ImageIndex    = 1;
			ProcessNode->SelectedIndex = 1;
		}
	}
}
//---------------------------------------------------------------------------
TTreeNode* __fastcall TSubscriberForm::FindMBusNode( const UFC::AnsiString& MBusName )
{
	TTreeNode* MBusNode = MBusTreeView->Items->GetFirstNode();
	System::UnicodeString MBusHostname( MBusName.c_str() );

	while( MBusNode != NULL )
	{
		if( MBusNode->Text.Compare( MBusHostname ) == 0 )
			return MBusNode;
		MBusNode = MBusNode->getNextSibling( );
	};
	return NULL;
}
//---------------------------------------------------------------------------
TTreeNode* __fastcall TSubscriberForm::FindProcessNode( TTreeNode* MBusNode, const UFC::AnsiString& ProcessName )
{
	if( MBusNode != NULL )
	{
		System::UnicodeString SearchProcessName( ProcessName.c_str() );
		TTreeNode*            ProcessNode = MBusNode->getFirstChild();

		while( ProcessNode != NULL )
		{
			if( ProcessNode->Text.Compare( SearchProcessName ) == 0 )
				return ProcessNode;
			ProcessNode = ProcessNode->getNextSibling( );
		};
	}
	return NULL;
}
//---------------------------------------------------------------------------
TTreeNode* __fastcall TSubscriberForm::FindProcessNode( const UFC::AnsiString& MBusName, const UFC::AnsiString& ProcessName )
{
	TTreeNode* MBusNode = FindMBusNode( MBusName );

	if( MBusNode != NULL )
		return FindProcessNode( MBusNode, ProcessName );
	return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::ProcessListTimerTimer(TObject *Sender)
{
	ProcessListTimer->Enabled = false;
	if( MessageObject->IsConnected() == TRUE )
	{
		TTreeNode* MBusNode = MBusTreeView->Items->GetFirstNode();
		while( MBusNode != NULL )
		{
			System::UnicodeString MBusName = MBusNode->Text;
			MessageObject->RequestProcessList( MBusName.t_str() );
			MBusNode = MBusNode->getNextSibling( );
		};
	}
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MBusTimerTimer(TObject *Sender)
{
	MBusTimer->Enabled = false;
	if( MessageObject->IsConnected() == TRUE )
		MessageObject->RequestMBusList( );
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MessageObjectProcessStartup(const MString &Hostname,
		  const MString &ProcessName)
{
	TTreeNode* ProcessNode = FindProcessNode( Hostname, ProcessName);
	if( ProcessNode != NULL )
	{
		ProcessNode->ImageIndex    = 1;
		ProcessNode->SelectedIndex = 1;
		MBusTreeView->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MessageObjectProcessStopped(const MString &Hostname,
		  const MString &ProcessName)
{
	TTreeNode* ProcessNode = FindProcessNode( Hostname, ProcessName);
	if( ProcessNode != NULL )
	{
		ProcessNode->ImageIndex    = 0;
		ProcessNode->SelectedIndex = 0;
		MBusTreeView->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MessageObjectMBusList(const MString &Hostname, TStringList *MBuss)

{
	TTreeNode*       MBusNode;

	for( int i=0; i < MBuss->Count; i ++ )
	{
		System::UnicodeString PName( MBuss->Strings[i] );

		if( (MBusNode = FindMBusNode( PName.t_str() )) == NULL )
			MBusNode = MBusTreeView->Items->Add( MBusTreeView->Items->GetFirstNode(), MBuss->Strings[i] );
		MBusNode->ImageIndex    = 3; ///< Alive.
		MBusNode->SelectedIndex = 3;
	}
	ProcessListTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MBusTreeViewExpanded(TObject *Sender, TTreeNode *Node)

{
	System::UnicodeString MBusName( Node->Text );
	UFC::AnsiString       MBusAnsiName( MBusName.t_str());
	TTreeNode*            MBusNode;

	if( (MBusNode = FindMBusNode( MBusAnsiName )) == Node )
	{
		TTreeNode*  ProcessNode = MBusNode->getFirstChild();

		while( ProcessNode != NULL )
		{
			System::UnicodeString PName( ProcessNode->Text );

			MessageObject->MonitoringProcess( MBusAnsiName, PName.t_str());
			ProcessNode = ProcessNode->getNextSibling( );
		};
	}
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MessageObjectMBusStartup(const MString &Hostname)

{
	TTreeNode* MBusNode = FindMBusNode( Hostname );
	if( MBusNode != NULL )
	{
		MBusNode->ImageIndex    = 3; ///< MBus activate.
		MBusNode->SelectedIndex = 3;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSubscriberForm::MessageObjectMBusStopped(const MString &Hostname)

{
	TTreeNode* MBusNode = FindMBusNode( Hostname );
	if( MBusNode != NULL )
	{
		MBusNode->ImageIndex    = 2; ///< MBus down.
		MBusNode->SelectedIndex = 2;
	}
}
//---------------------------------------------------------------------------





