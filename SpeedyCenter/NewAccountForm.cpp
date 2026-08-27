//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "NewAccountForm.h"
#include "AccountInput.h"
#include "SimTFXMain.h"
#include "TerminalForm.h"
#include "VersionUnit.h"
#include "..\UFC\MD5.h"
#include "SCStrings.hpp"
#include "SettingForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cspin"
#pragma resource "*.dfm"
TAccountForm *AccountForm = NULL;
//------------------------------------------------------------------------------
extern AnsiString CurrentDir;
//------------------------------------------------------------------------------
//  0~9, Upper case A-Z, lower caase a-z.
//------------------------------------------------------------------------------
const char ChLUT[] = { "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" };
//------------------------------------------------------------------------------
UFCType::Int32 CharToInt( char ch )
{
	if( ch >= '0' && ch <= '9' ) ///< 10 from 0~9
		return (UFCType::Int32)(ch - '0');
	else if( ch >= 'A' && ch <= 'Z' ) ///< 26 from 10 ~ 35
		return (UFCType::Int32)(ch - 'A' + 10 );
	else if( ch >= 'a' && ch <= 'z' )
		return (UFCType::Int32)(ch - 'a' + 36 );///< 26 from 36 ~ 61
	else
		return 0;
}
//------------------------------------------------------------------------------
bool IsNumber( char ch )
{
	if( ch >= '0' && ch <= '9')
		return true;
	return false;
}
//------------------------------------------------------------------------------
bool isAlnum( char ch )
{
	if( (ch >= '0' && ch <= '9')||(ch >= 'A' && ch <= 'Z')||(ch >= 'a' && ch <= 'z'))
		return true;
	return false;
}
//------------------------------------------------------------------------------
char IntToChar( UFCType::Int32 index )
{
	if( index >=0 && index <=61 )
		return ChLUT[ index ];
	return '0';
}
//------------------------------------------------------------------------------
UFCType::Int32 TFXOIDToInt( const UFC::AnsiString& OID, int Terms )
{
	switch( Terms)
	{
		case 1:  return (CharToInt(OID[0])*62*62 + CharToInt(OID[1])*62 + CharToInt(OID[2]) )*100 + OID.SubString(3,2).ToInt();
		case 2:  return (CharToInt(OID[0])*62*62*62 + CharToInt(OID[1])*62*62 + CharToInt(OID[2])*62  + CharToInt(OID[3]) )*10 + OID.SubString(4,1).ToInt();
		case 3:  return (CharToInt(OID[0])*62*62*62*62 + CharToInt(OID[1])*62*62*62 + CharToInt(OID[2])*62*62  + CharToInt(OID[3])*62 + CharToInt(OID[4]));
		default: return (CharToInt(OID[0])*62 + CharToInt( OID[1] ))*1000 + OID.SubString( 2,3).ToInt();
	}
}
//------------------------------------------------------------------------------
UFCType::Int32 TSEOIDToInt( const UFC::AnsiString& OID, int Terms )
{
	switch( Terms)
	{
		case 0:  return (CharToInt(OID[0])*10000 + OID.SubString( 1,4 ).ToInt());
		case 1:  return (CharToInt(OID[0])*62 + CharToInt( OID[1] ))*1000 + OID.SubString( 2,3).ToInt();
		case 2:  return (CharToInt(OID[0])*62*62 + CharToInt(OID[1])*62 + CharToInt(OID[2]) )*100 + OID.SubString(3,2).ToInt();
		case 3:  return (CharToInt(OID[0])*62*62*62 + CharToInt(OID[1])*62*62 + CharToInt(OID[2])*62  + CharToInt(OID[3]) )*10 + OID.SubString(4,1).ToInt();
		case 4:  return (CharToInt(OID[0])*62*62*62*62 + CharToInt(OID[1])*62*62*62 + CharToInt(OID[2])*62*62  + CharToInt(OID[3])*62 + CharToInt(OID[4]));
		default: return (CharToInt(OID[0])*10000 + OID.SubString( 1,4 ).ToInt());
	}
}
//------------------------------------------------------------------------------
UFCType::Int32 PATSOIDToInt( const UFC::AnsiString& OID )
{
	return  CharToInt( OID[0])*10000 + OID.SubString( 1,4 ).ToInt();
}
//------------------------------------------------------------------------------
bool CheckTFXOID( const char* OID, int Terms  )
{
	switch( Terms )
	{
		case 1: return (isAlnum( OID[0]) && isAlnum( OID[1]) && isAlnum( OID[2]) &&  IsNumber( OID[3]) && IsNumber( OID[4]));
		case 2: return (isAlnum( OID[0]) && isAlnum( OID[1]) && isAlnum( OID[2]) &&  isAlnum( OID[3])  && IsNumber( OID[4]));
		case 3: return (isAlnum( OID[0]) && isAlnum( OID[1]) && isAlnum( OID[2]) &&  isAlnum( OID[3])  && isAlnum( OID[4]));
		default:return (isAlnum( OID[0]) && isAlnum( OID[1]) && IsNumber( OID[2]) &&  IsNumber( OID[3]) && IsNumber( OID[4]));
	}
}
//------------------------------------------------------------------------------
bool CheckPATSOID( const char* OID )
{
	return (isAlnum( OID[0]) && IsNumber( OID[1]) && IsNumber( OID[2]) &&  IsNumber( OID[3]) && IsNumber( OID[4]));
}
//------------------------------------------------------------------------------
bool CheckTSEOID( const char* OID, int Terms   )
{
	switch( Terms )
	{
		case 0: return (isAlnum( OID[0]) && IsNumber( OID[1]) && IsNumber( OID[2]) &&  IsNumber( OID[3]) && IsNumber( OID[4]));
		case 1: return (isAlnum( OID[0]) && isAlnum( OID[1]) && IsNumber( OID[2]) &&  IsNumber( OID[3]) && IsNumber( OID[4]));
		case 2: return (isAlnum( OID[0]) && isAlnum( OID[1]) && isAlnum( OID[2]) &&  IsNumber( OID[3]) && IsNumber( OID[4]));
		case 3: return (isAlnum( OID[0]) && isAlnum( OID[1]) && isAlnum( OID[2]) &&  isAlnum( OID[3])  && IsNumber( OID[4]));
		case 4: return (isAlnum( OID[0]) && isAlnum( OID[1]) && isAlnum( OID[2]) &&  isAlnum( OID[3])  && isAlnum( OID[4]));
		default:return (isAlnum( OID[0]) && IsNumber( OID[1]) && IsNumber( OID[2]) &&  IsNumber( OID[3]) && IsNumber( OID[4]));
	}
}
//------------------------------------------------------------------------------
bool CheckFUTBrokerID( const AnsiString& BrokerID )
{
	if( BrokerID.Length() != 7 )
		return false;
	if( BrokerID[ 1 ] != 'F' && BrokerID[ 1 ] != 'S' )
		return false;
	try
	{
		BrokerID.SubString( 2, 6 ).ToInt();
	}
	catch( ... )
	{
		return false;
	}
	return true;
}
//------------------------------------------------------------------------------
bool CheckTSEBrokerID( const AnsiString& BrokerID )
{
	try
	{
		if( BrokerID.Length() != 4 )
			return false;
		BrokerID.SubString( 0, 3 ).ToInt();
	}
	catch( ... )
	{
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
//
//  Class TAccountForm
//
//---------------------------------------------------------------------------
//   For New Account setting
//---------------------------------------------------------------------------
__fastcall TAccountForm::TAccountForm(TComponent* Owner)
:TForm(Owner)
,FParentForm( dynamic_cast< TSimTFXForm*> (Owner) )
,FUse62CarrySeq( false )
{
	FParentForm = dynamic_cast< TSimTFXForm*> (Owner);
	Caption = Scstrings_MAIN_AE_ADD_AE;///"新增營業員";
	IPCheckBoxClick( NULL );
	SetOIDGenType( 3, true );
	SetOIDGenType( 3, false );
	DefaultCheckerComboBox->ItemIndex = 1;
	TSEDefaultCheckerComboBox->ItemIndex = 1;
	PATSTabSheet->TabVisible = false;
}
//---------------------------------------------------------------------------
//   For Modify Account setting
//---------------------------------------------------------------------------
__fastcall TAccountForm::TAccountForm( TComponent* Owner, UFC::Section* iniSection, TStrings* Checkers,TStrings* Sources, bool ModifyAttr )
:TForm( Owner )
,FParentForm( dynamic_cast< TSimTFXForm*> (Owner) )
,FUse62CarrySeq( false )
{
	if( ModifyAttr )///< Modify existing ActiveExecutive
	{
		UFC::AnsiString Type,Group = "0",Source = "0", Users;

		Caption = Scstrings_MAIN_AE_MODIFY_AE;///"修改營業員資料";
		SourceComboBox->Items->Assign( Sources );
		CheckerComboBox->Items->Assign(	Checkers );
		TSECheckerComboBox->Items->Assign( Checkers );
		PATSCheckerComboBox->Items->Assign( Checkers );
		IDEdit->Text = iniSection->GetSectionName().c_str();
		IDEdit->Enabled = false;
		PwdEdit->Text = "1234rewq";
		PwdConfirmEdit->Text = "1234rewq";
		PwdPanel->Visible = false;
		iniSection->GetValue( "Type",Type );
		iniSection->GetValue( "Source",Source );
		iniSection->GetValue( "Group",Group );
		iniSection->GetValue( "Users",Users );
		GroupComboBox->ItemIndex = TypeToIndex( Type );
		SourceComboBox->ItemIndex = Source.ToInt();
		ComboBoxGroup->ItemIndex = Group.ToInt();
		GroupComboBoxChange( NULL );
		UsersEdit->Text = Users.c_str() != NULL ? Users.c_str() : "";
		InitOrderIDProperties( iniSection );
		InitBrokerIDProperties( iniSection );
		InitCheckerProperties( iniSection );
		InitOtherProperties( iniSection );
	}
	else ///< Modify password
	{
		UFC::AnsiString ForceChangePwd = "0";

		Caption = Scstrings_MAIN_AE_MODIFY_PASSWORD;///"修改密碼";
		IDEdit->Text = iniSection->GetSectionName().c_str();
		IDEdit->Enabled = false;
		PwdEdit->Text = "";
		PwdConfirmEdit->Text = "";
		AttrPanel->Visible = false;
		SrcPanel->Visible = false;
		AccountPanel->Visible = false;
		DefaultCheckerComboBox->ItemIndex = 0;
		iniSection->GetValue( "ForceChangePassword",ForceChangePwd );
		if( ForceChangePwd.ToInt() == 1 )
			ModifyPasswordCheckBox->Checked = true;
		else
			ModifyPasswordCheckBox->Checked = false;
	}
	TFXOffHourTabSheet->TabVisible = false;
	PATSTabSheet->TabVisible = false;
}
//---------------------------------------------------------------------------
//   For shared term setting
//---------------------------------------------------------------------------
__fastcall TAccountForm::TAccountForm( TComponent* Owner, UFC::Section* iniSection, UFC::Section* OHiniSection, TStrings* Checkers,TStrings* Sources )
:TForm( Owner )
,FParentForm( dynamic_cast< TSimTFXForm*> (Owner) )
,FUse62CarrySeq( false )
{
	UFC::AnsiString Type,Group = "0",Source = "0",Users;

	Caption = Scstrings_MAIN_AE_TERM_POOL_API_VER_SETTING;///"共用櫃號及API版本設定";
	SourceComboBox->Items->Assign( Sources );
	CheckerComboBox->Items->Assign(	Checkers );
	TSECheckerComboBox->Items->Assign( Checkers );
	PATSCheckerComboBox->Items->Assign( Checkers );
	IDEdit->Text = iniSection->GetSectionName().c_str();
	IDEdit->Enabled = false;
	PwdEdit->Text = "1234rewq";
	PwdConfirmEdit->Text = "1234rewq";
	IDPanel->Visible = false;
	PwdPanel->Visible = false;
	iniSection->GetValue( "Type",Type );
	iniSection->GetValue( "Source",Source );
	iniSection->GetValue( "Group",Group );
	iniSection->GetValue( "Users",Users);
	GroupComboBox->ItemIndex = TypeToIndex( Type );
	SourceComboBox->ItemIndex = Source.ToInt();
	ComboBoxGroup->ItemIndex = Group.ToInt();
	UsersEdit->Text = Users.c_str() != NULL ? Users.c_str() : "";
	GroupComboBoxChange( NULL );
	InitOrderIDProperties( iniSection );
	InitOHOrderIDProperties( OHiniSection );
	InitBrokerIDProperties( iniSection );
	InitCheckerProperties( iniSection );
	InitOtherProperties( iniSection );
	SrcPanel->Visible = false;
	AccountPanel->Visible = false;
	VersionSheet->TabVisible = true;
	WarningSheet->TabVisible = true;
	PATSTabSheet->TabVisible = true;
	TFXOffHourTabSheet->TabVisible = true;
	FUTBrokerIDCheckBox->Enabled = false;
	FUTBrokerIDEdit->Enabled = false;
	Label11->Enabled = false;
	ComboBoxGroup->Enabled = false;
	Label6->Enabled = false;
	Label9->Enabled = false;
	CheckerComboBox->Enabled = false;
	DefaultCheckerComboBox->Enabled = false;
	TSEBrokerIDCheckBox->Enabled = false;
	TSEBrokerIDEdit->Enabled = false;
	Label7->Enabled = false;
	Label10->Enabled = false;
	TSECheckerComboBox->Enabled = false;
	TSEDefaultCheckerComboBox->Enabled = false;
	OIDGenGroup->Buttons[3]->Enabled = false;
	OHOIDGenGroup->Buttons[3]->Enabled = false;
	TSEOIDGenGroup->Buttons[3]->Enabled = false;

	UFC::AnsiString VCount,Value,ID;
	if( iniSection->GetValue( "VerCount", VCount ) )
	{
		for( int i = 0; i < VCount.ToInt();i++ )
		{
			if( iniSection->GetValue( UFC::AnsiString(i), Value ) )
				VerListBox->Items->Add( Value.c_str() );
		}
	}
	if( iniSection->GetValue( "Level1", Value ) )
		Level1Spin->Value = Value.ToInt( );
	if( iniSection->GetValue( "Level2", Value ) )
		Level2Spin->Value = Value.ToInt( );
	Level2Spin->MaxValue = Level1Spin->Value;
	if( Level2Spin->Value > Level2Spin->MaxValue )
		Level2Spin->Value = Level2Spin->MaxValue -1;
	if( iniSection->GetValue( "Version", Value ) )
		VersionLabel->Caption = Value.c_str();
	if( iniSection->GetValue( "Build", Value ) )
		BuildLabel->Caption = Value.c_str();
	if( iniSection->GetValue( "OffHourOtherRule", Value ) )
	{
		OffHourCheckBox->Checked = Value.ToInt();
		OffHourCheckBoxClick( NULL );
	}
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::InitOrderIDProperties( UFC::Section* iniSection )
{
	UFC::AnsiString Begin,End,Terminals,Value;
	UFC::AnsiString TAIFEXTerm( "0" );
	UFC::AnsiString TSE2Term( "0" );
	UFC::AnsiString Use62Carry( "0" );

	iniSection->GetValue( "Use62CarrySeq",Use62Carry );
	FUse62CarrySeq = Use62Carry.ToInt();
	///< For TAIFEX
	if( iniSection->GetValue( "Begin",Begin ) && iniSection->GetValue( "End",End ))
	{
		BeginOIDEdit->Text = Begin.c_str();
		EndOIDEdit->Text   = End.c_str();
		SetOIDGenType( 1, true );
	}
	else if( iniSection->GetValue( "Terminals", Terminals ) )
	{
		TerminalsEdit->Text = Terminals.c_str();
		SetOIDGenType( 2, true );
	}
	else if( iniSection->GetValue( "UseGlobalTerminals", Value ) )
		SetOIDGenType( 3, true );
	else
		SetOIDGenType( 0, true );
	iniSection->GetValue( "TAIFEX3TermOrderID",TAIFEXTerm );
	TermsComboBox->ItemIndex = TAIFEXTerm.ToInt();
	TermsComboBoxChange( NULL );
	///< For TWSE/OTC
	if( iniSection->GetValue( "TSEBegin",Begin ) && iniSection->GetValue( "TSEEnd",End ))
	{
		TSEBeginOIDEdit->Text = Begin.c_str();
		TSEEndOIDEdit->Text   = End.c_str();
		SetOIDGenType( 1, false );
	}
	else if( iniSection->GetValue( "TSETerminals", Terminals ) )
	{
		TSETerminalsEdit->Text = Terminals.c_str();
		SetOIDGenType( 2, false );
	}
	else if( iniSection->GetValue( "TSEUseGlobalTerminals", Value))
		SetOIDGenType( 3, false );
	else
		SetOIDGenType( 0, false );
	iniSection->GetValue( "TSE2TermOrderID",TSE2Term );
	TSETermsComboBox->ItemIndex = TSE2Term.ToInt();
	TSETermsComboBoxChange( NULL );
	///< for Pats
	if( iniSection->GetValue( "PATSBegin",Begin ) && iniSection->GetValue( "PATSEnd",End ))
	{
		PATSBeginOIDEdit->Text = Begin.c_str();
		PATSEndOIDEdit->Text   = End.c_str();
		PATSOIDPrefixEdit->Text = L"u";
		SetPATSOIDGenType( 1 );
	}
	else if( iniSection->GetValue( "PATSOrderIDPrefix", Value ) )
	{
		PATSBeginOIDEdit->Text = "A0001";
		PATSEndOIDEdit->Text   = "B9999";
		PATSOIDPrefixEdit->Text = Value.c_str();
		SetPATSOIDGenType( 0 );
	}
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::InitOHOrderIDProperties( UFC::Section* iniSection )
{
	UFC::AnsiString Begin,End,Terminals,TSE2Term,Value;
	UFC::AnsiString TAIFEXTerm( "0" );

	///< For TAIFEX
	if( iniSection->GetValue( "Begin",Begin ) && iniSection->GetValue( "End",End ))
	{
		OHBeginOIDEdit->Text = Begin.c_str();
		OHEndOIDEdit->Text   = End.c_str();
		OHOIDGenGroup->ItemIndex = 1;
		OHOIDGenGroupClick( NULL );
	}
	else if( iniSection->GetValue( "Terminals", Terminals ) )
	{
		OHTerminalsEdit->Text = Terminals.c_str();
		OHOIDGenGroup->ItemIndex = 2;
		OHOIDGenGroupClick( NULL );
	}
	else
	{
		OHOIDGenGroup->ItemIndex = 0;
		OHOIDGenGroupClick( NULL );
	}
	iniSection->GetValue( "TAIFEX3TermOrderID",TAIFEXTerm );
	OHTermsComboBox->ItemIndex = TAIFEXTerm.ToInt();
	OHTermsComboBoxChange( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::InitBrokerIDProperties( UFC::Section* iniSection )
{
	UFC::AnsiString TSEBrokerID,FUTBrokerID;

	if( iniSection->GetValue( "TSEBrokerID", TSEBrokerID ))
	{
		TSEBrokerIDEdit->Text = TSEBrokerID.c_str();
		TSEBrokerIDCheckBox->Checked = true;
	}
	else
		TSEBrokerIDCheckBox->Checked = false;
	TSEBrokerIDCheckBoxClick( NULL );
	if( iniSection->GetValue( "FUTBrokerID", FUTBrokerID ))
	{
		FUTBrokerIDEdit->Text = FUTBrokerID.c_str();
		FUTBrokerIDCheckBox->Checked = true;
	}
	else
		FUTBrokerIDCheckBox->Checked = false;
	FUTBrokerIDCheckBoxClick( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::InitCheckerProperties( UFC::Section* iniSection )
{
	UFC::AnsiString Action,Checker;

	DefaultCheckerComboBox->Visible = !FormSetting->ForceRM();
	TSEDefaultCheckerComboBox->Visible = !FormSetting->ForceRM();
	Label9->Visible = !FormSetting->ForceRM();
	Label10->Visible = !FormSetting->ForceRM();
	///< for TAIFEX
	if( iniSection->GetValue( "DefCheckAction", Action ))
		DefaultCheckerComboBox->ItemIndex = Action.ToInt();
	else
		DefaultCheckerComboBox->ItemIndex = 1;
	if( iniSection->GetValue( "Checker", Checker ))
	{
		int index = CheckerComboBox->Items->IndexOf( Checker.c_str() );
		if( index == -1 )
			CheckerComboBox->ItemIndex = 0;
		else
			CheckerComboBox->ItemIndex = index;
	}
	else
		CheckerComboBox->ItemIndex = 0;
	///< For TSE/OTC
	if( iniSection->GetValue( "TSEDefCheckAction", Action ))
		TSEDefaultCheckerComboBox->ItemIndex = Action.ToInt();
	else
		TSEDefaultCheckerComboBox->ItemIndex = 1;
	if( iniSection->GetValue( "TSEChecker", Checker ))
	{
		int index = TSECheckerComboBox->Items->IndexOf( Checker.c_str() );
		if( index == -1 )
			TSECheckerComboBox->ItemIndex = 0;
		else
			TSECheckerComboBox->ItemIndex = index;
	}
	else
		TSECheckerComboBox->ItemIndex = 0;
	///< for PATS
	if( iniSection->GetValue( "PATSDefCheckAction", Action ))
		PATSDefaultCheckerComboBox->ItemIndex = Action.ToInt();
	else
		PATSDefaultCheckerComboBox->ItemIndex = 1;
	if( iniSection->GetValue( "PATSChecker", Checker ))
	{
		int index = PATSCheckerComboBox->Items->IndexOf( Checker.c_str() );
		if( index == -1 )
			PATSCheckerComboBox->Items->Add( Checker.c_str() );
		PATSCheckerComboBox->ItemIndex = PATSCheckerComboBox->Items->IndexOf( Checker.c_str() );
	}
	else
		PATSCheckerComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::InitOtherProperties( UFC::Section* iniSection )
{
	UFC::AnsiString ForceIP,Value,Throughput("300");

	iniSection->GetValue( "ThroughputMax", Throughput );
	ThroughputSpinEdit->Value = Throughput.ToInt();

	iniSection->GetValue( "IP", ForceIP );
	if( ForceIP == "All" )
	{
		IPCheckBox->Checked = false;
		IPEdit->Text = "";
	}
	else
	{
		IPCheckBox->Checked = true;
		IPEdit->Text = ForceIP.c_str();
	}
	IPCheckBoxClick( NULL );
	///< TAIFEX OrderSource and InfoSource
	OrdSrcComboBox->ItemIndex = 0;
	if( iniSection->GetValue( "OrderSource", Value ) )
	{
		for( int i = 0;i < OrdSrcComboBox->Items->Count; i++ )
		{
		   AnsiString Str = OrdSrcComboBox->Items->Strings[i];
		   if( Str[1] == Value[0] )
		   {
			   OrdSrcComboBox->ItemIndex = i;
			   break;
		   }
		}
	}
	InfoSrcEdit->Text = L"";
	if( iniSection->GetValue( "MarketDataSource", Value ) )
	{
		if( Value.Length( ) == 3 )
			InfoSrcEdit->Text = Value.c_str();
	}
	///< Get accounts.
	for( int j = 0; j < iniSection->ItemCount(); j++ )
	{
		UFC::AnsiString Name,Value;
		TListItem*      NewListItem;
		iniSection->GetNameValue( j, Name,Value );
		///< It's an account setting.
		if( Value.Length() == 1 )
		{
			if( Name.Length() == 10 && Name.AnsiNCompare( "TSE", 3) == 0 )
			{
				NewListItem = TWSEAccountListView->Items->Add();
				NewListItem->Caption = Name.c_str() + 3;
				NewListItem->SubItems->Add( Value.c_str() );
			}
			else if( Name.Length() == 10 && Name.AnsiNCompare( "TFX", 3) == 0 )
			{
				NewListItem = AccountListView->Items->Add();
				NewListItem->Caption = Name.c_str() + 3;
				NewListItem->SubItems->Add( Value.c_str() );
			}
			else if( Value == "P" && Name != "OrderSource" )
			{
				///< It's an PATS account setting.
				AccListBox->Items->Add( Name.c_str() );
			}
			else if( Name.Length() == 7 ) ///< Old version, add to both
			{
				NewListItem = AccountListView->Items->Add();
				NewListItem->Caption = Name.c_str();
				NewListItem->SubItems->Add( Value.c_str() );
				NewListItem = TWSEAccountListView->Items->Add();
				NewListItem->Caption = Name.c_str();
				NewListItem->SubItems->Add( Value.c_str() );
			}
		}
	}
}
//---------------------------------------------------------------------------
int __fastcall TAccountForm::TypeToIndex( const UFC::AnsiString& Type )
{
	if( Type == "Admin" )
		return 0;
	else if( Type == "User" )
		return 1;
	else if( Type == "Channel" )
		return 2;
	else
		return 1;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::GetType( UFC::AnsiString& Type )
{
	switch( GroupComboBox->ItemIndex )
	{
		case 0: Type  = "Admin";break;
		case 1: Type  = "User";break;
		case 2: Type  = "Channel";break;
		default: Type = "User";break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::FormShow(TObject *Sender)
{
	ClientHeight = BtnPanel->Top + BtnPanel->Height;
	PageControl->ActivePage = TFXTabSheet;
	PageControlChange( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::IPCheckBoxClick(TObject *Sender)
{
	if( IPCheckBox->Checked == true )
		IPEdit->Enabled  = true;
	else
		IPEdit->Enabled  = false;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::UpdateChangePasswd( UFC::UiniFile* ini )
{
	AnsiString AE = IDEdit->Text;
	AnsiString Pwd = PwdEdit->Text;
	UFC::MD5   PWDmd5( (const UFCType::UInt8*) Pwd.c_str(),Pwd.Length());

	ini->SetValue( AE.c_str(), "Password", PWDmd5.ToString() );
	ini->SetValue( AE.c_str(), "ForceChangePassword", ModifyPasswordCheckBox->Checked ? "1" : "0" );
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::UpdateNewAccount( UFC::UiniFile* ini )
{
	AnsiString      IP;
	AnsiString      TFXChecker = CheckerComboBox->Text;
	AnsiString      TSEChecker = TSECheckerComboBox->Text;
	AnsiString      AE  = IDEdit->Text;
	AnsiString      Pwd = PwdEdit->Text;
	UFC::AnsiString Group( ComboBoxGroup->ItemIndex );
	UFC::AnsiString Type,Source( SourceComboBox->ItemIndex );
	UFC::AnsiString TAIFEXTerms( TermsComboBox->ItemIndex );
	UFC::AnsiString TSETerms( TSETermsComboBox->ItemIndex );
	UFC::AnsiString Throughput( (int)ThroughputSpinEdit->Value );
	UFC::MD5        PWDmd5( (const UFCType::UInt8*)Pwd.c_str(),Pwd.Length());

	if( IPCheckBox->Checked == true )
		IP = IPEdit->Text;
	else
		IP = "All";
	if( OIDGenGroup->ItemIndex == 1)
	{
		AnsiString  Begin = BeginOIDEdit->Text;
		AnsiString  End   = EndOIDEdit->Text;

		ini->SetValue( AE.c_str(), "Begin", Begin.c_str() );
		ini->SetValue( AE.c_str(), "End", End.c_str() );
	}
	else if( OIDGenGroup->ItemIndex == 2 )
	{
		AnsiString Terminals = TerminalsEdit->Text;

		ini->SetValue( AE.c_str(), "Terminals", Terminals.c_str() );
	}
	else if( OIDGenGroup->ItemIndex == 3 )
		ini->SetValue( AE.c_str(), "UseGlobalTerminals", "1" );
	ini->SetValue( AE.c_str(), "TAIFEX3TermOrderID",TAIFEXTerms );
	if( TSEOIDGenGroup->ItemIndex == 1)
	{
		AnsiString  Begin = TSEBeginOIDEdit->Text;
		AnsiString  End   = TSEEndOIDEdit->Text;

		ini->SetValue( AE.c_str(), "TSEBegin", Begin.c_str() );
		ini->SetValue( AE.c_str(), "TSEEnd", End.c_str() );
	}
	else if( TSEOIDGenGroup->ItemIndex == 2 )
	{
		AnsiString Terminals = TSETerminalsEdit->Text;

		ini->SetValue( AE.c_str(), "TSETerminals", Terminals.c_str() );
	}
	else if( TSEOIDGenGroup->ItemIndex == 3 )
		ini->SetValue( AE.c_str(), "TSEUseGlobalTerminals", "1" );
	ini->SetValue( AE.c_str(), "TSE2TermOrderID",TSETerms );
	if( TSEBrokerIDCheckBox->Checked == true )
	{
		AnsiString  BrokerID = TSEBrokerIDEdit->Text;

		ini->SetValue( AE.c_str(), "TSEBrokerID", BrokerID.c_str() );
	}
	if( FUTBrokerIDCheckBox->Checked == true )
	{
		AnsiString  BrokerID = FUTBrokerIDEdit->Text;

		ini->SetValue( AE.c_str(), "FUTBrokerID", BrokerID.c_str() );
	}
	if( FormSetting->ForceRM() == false )
	{
		if( DefaultCheckerComboBox->ItemIndex == 0 )
			ini->SetValue( AE.c_str(), "DefCheckAction", "0" ); ///< Reject
		else
			ini->SetValue( AE.c_str(), "DefCheckAction", "1" ); ///< Pass
	}
	else
		ini->SetValue( AE.c_str(), "DefCheckAction", "0" ); ///< Reject
	if( FormSetting->ForceRM() == false )
	{
		if( TSEDefaultCheckerComboBox->ItemIndex == 0 )
			ini->SetValue( AE.c_str(), "TSEDefCheckAction", "0" ); ///< Reject
		else
			ini->SetValue( AE.c_str(), "TSEDefCheckAction", "1" ); ///< Pass
	}
	else
		ini->SetValue( AE.c_str(), "TSEDefCheckAction", "0" ); ///< Reject
	TFXChecker.TrimRight();
	if( FormSetting->ForceRM() == false && CheckerComboBox->ItemIndex == 0  )
		ini->SetValue( AE.c_str(), "Checker", "null" );
	else
		ini->SetValue( AE.c_str(), "Checker", TFXChecker.c_str() );
	TSEChecker.TrimRight();
	if( FormSetting->ForceRM() == false && TSECheckerComboBox->ItemIndex == 0  )
		ini->SetValue( AE.c_str(), "TSEChecker", "null" );
	else
		ini->SetValue( AE.c_str(), "TSEChecker", TSEChecker.c_str() );
	GetType( Type );
	ini->SetValue( AE.c_str(), "Password", PWDmd5.ToString() );
	ini->SetValue( AE.c_str(), "ForceChangePassword", ModifyPasswordCheckBox->Checked ? "1" : "0" );
	ini->SetValue( AE.c_str(), "Type", Type );
	ini->SetValue( AE.c_str(), "IP", IP.c_str() );
	ini->SetValue( AE.c_str(), "Source", Source );
	ini->SetValue( AE.c_str(), "Group", Group );
	ini->SetValue( AE.c_str(), "ThroughputMax", Throughput.c_str() );
	if( FUse62CarrySeq == true )
		ini->SetValue( AE.c_str(), "Use62CarrySeq", "1" );
	else
		ini->SetValue( AE.c_str(), "Use62CarrySeq", "0" );
	///< TAIFEX OrderSource and InfoSource
	if( OrdSrcComboBox->ItemIndex > 0 )
	{
		AnsiString Str = OrdSrcComboBox->Items->Strings[ OrdSrcComboBox->ItemIndex ];
		Str.SetLength( 1 );
		ini->SetValue( AE.c_str(), "OrderSource", Str.c_str() );
	}
	if( InfoSrcEdit->Text.Length() == 3 )
	{
		AnsiString Str = InfoSrcEdit->Text;
		ini->SetValue( AE.c_str(), "MarketDataSource", Str.c_str() );
	}
	TListItem*      UpdateListItem;
	for( int i = 0; i < AccountListView->Items->Count;i++ )
	{
		if( (UpdateListItem = AccountListView->Items->Item[i]) != NULL )
		{
			AnsiString Name  = "TFX" + UpdateListItem->Caption;
			AnsiString Value = UpdateListItem->SubItems->Strings[0];

			ini->SetValue( AE.c_str(), Name.c_str(), Value.c_str() );
		}
	}
	for( int i = 0; i < TWSEAccountListView->Items->Count;i++ )
	{
		if( (UpdateListItem = TWSEAccountListView->Items->Item[i]) != NULL )
		{
			AnsiString Name  = "TSE" + UpdateListItem->Caption;
			AnsiString Value = UpdateListItem->SubItems->Strings[0];

			ini->SetValue( AE.c_str(), Name.c_str(), Value.c_str() );
		}
	}
	for( int i = 0; i < AccListBox->Items->Count;i++ )
	{
		AnsiString Name  = AccListBox->Items->Strings[i];
		AnsiString Value( "P" );;

		ini->SetValue( AE.c_str(), Name.c_str(), Value.c_str() );
	}
	if( VersionSheet->TabVisible == true )
	{
		ini->SetValue( AE.c_str(), "VerCount", UFC::AnsiString( VerListBox->Items->Count ) );
		for( int i = 0; i < VerListBox->Items->Count;i++ )
		{
			AnsiString Value = VerListBox->Items->Strings[i];

			ini->SetValue( AE.c_str(), UFC::AnsiString(i), Value.c_str());
		}
		ini->SetValue( AE.c_str(), "Level1", UFC::AnsiString( (int)Level1Spin->Value));
		ini->SetValue( AE.c_str(), "Level2", UFC::AnsiString( (int)Level2Spin->Value));
	}
	if( AE == "Speedy" )
	{
		if( OHOIDGenGroup->ItemIndex == 1)
		{
			AnsiString  Begin = OHBeginOIDEdit->Text;
			AnsiString  End   = OHEndOIDEdit->Text;

			ini->SetValue( "SpeedyOffHour", "Begin", Begin.c_str() );
			ini->SetValue( "SpeedyOffHour", "End", End.c_str() );
		}
		else if( OHOIDGenGroup->ItemIndex == 2 )
		{
			AnsiString Terminals = OHTerminalsEdit->Text;
			ini->SetValue( "SpeedyOffHour", "Terminals", Terminals.c_str() );
		}
		else if( OHOIDGenGroup->ItemIndex == 3 )
			ini->SetValue( "SpeedyOffHour", "UseGlobalTerminals", "1" );
		UFC::AnsiString  OHTAIFEXTerms( OHTermsComboBox->ItemIndex );
		ini->SetValue( "SpeedyOffHour", "TAIFEX3TermOrderID",OHTAIFEXTerms );
		if( OffHourCheckBox->Checked == true )
			ini->SetValue( "Speedy", "OffHourOtherRule", "1" );
		else
			ini->SetValue( "Speedy", "OffHourOtherRule", "0" );
	}
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::UpdateModifyAccount( UFC::UiniFile* ini )
{
	UFC::AnsiString  Group( ComboBoxGroup->ItemIndex );
	UFC::AnsiString  Type,Source( SourceComboBox->ItemIndex );
	UFC::AnsiString  TAIFEXTerms( TermsComboBox->ItemIndex );
	UFC::AnsiString  TSETerms( TSETermsComboBox->ItemIndex );
	UFC::AnsiString  Throughput( (int)ThroughputSpinEdit->Value );
	AnsiString       IP;
	AnsiString       AE = IDEdit->Text;
	AnsiString       TFXChecker = CheckerComboBox->Text;
	AnsiString       TSEChecker = TSECheckerComboBox->Text;
	TListItem*       UpdateListItem;
	UFC::Section*    iniSection = ini->GetSection( AE.c_str() );

	if( IPCheckBox->Checked == true )
		IP = IPEdit->Text;
	else
		IP = "All";
	iniSection->SetValue( "ThroughputMax", Throughput.c_str() );
	if( FUse62CarrySeq == true )
		iniSection->SetValue( "Use62CarrySeq", "1" );
	else
		iniSection->SetValue( "Use62CarrySeq", "0" );
	///< OrderID generate rule
	if( OIDGenGroup->ItemIndex == 1 )
	{
		AnsiString  Begin = BeginOIDEdit->Text;
		AnsiString  End   = EndOIDEdit->Text;

		iniSection->DeleteValue( "Terminals" );
		iniSection->SetValue( "Begin", Begin.c_str() );
		iniSection->SetValue( "End", End.c_str() );
	}
	else if( OIDGenGroup->ItemIndex == 2 )
	{
		AnsiString Terminals = TerminalsEdit->Text;

		iniSection->SetValue( "Terminals", Terminals.c_str() );
		iniSection->DeleteValue( "Begin" );
		iniSection->DeleteValue( "End" );
	}
	else if( OIDGenGroup->ItemIndex == 3 )
	{
		iniSection->SetValue( "UseGlobalTerminals", "1" );
		iniSection->DeleteValue( "Terminals" );
		iniSection->DeleteValue( "Begin" );
		iniSection->DeleteValue( "End" );
	}
	else
	{
		iniSection->DeleteValue( "UseGlobalTerminals" );
		iniSection->DeleteValue( "Terminals" );
		iniSection->DeleteValue( "Begin" );
		iniSection->DeleteValue( "End" );
	}
	iniSection->SetValue( "TAIFEX3TermOrderID",TAIFEXTerms );
	if( TSEOIDGenGroup->ItemIndex == 1 )
	{
		AnsiString  Begin = TSEBeginOIDEdit->Text;
		AnsiString  End   = TSEEndOIDEdit->Text;

		iniSection->DeleteValue( "TSETerminals" );
		iniSection->SetValue( "TSEBegin", Begin.c_str() );
		iniSection->SetValue( "TSEEnd", End.c_str() );
	}
	else if( TSEOIDGenGroup->ItemIndex == 2 )
	{
		AnsiString Terminals = TSETerminalsEdit->Text;

		iniSection->SetValue( "TSETerminals", Terminals.c_str() );
		iniSection->DeleteValue( "TSEBegin" );
		iniSection->DeleteValue( "TSEEnd" );
	}
	else if( TSEOIDGenGroup->ItemIndex == 3 )
	{
		iniSection->SetValue( "TSEUseGlobalTerminals", "1" );
		iniSection->DeleteValue( "TSETerminals" );
		iniSection->DeleteValue( "TSEBegin" );
		iniSection->DeleteValue( "TSEEnd" );
	}
	else
	{
		iniSection->DeleteValue( "TSEUseGlobalTerminals" );
		iniSection->DeleteValue( "TSETerminals" );
		iniSection->DeleteValue( "TSEBegin" );
		iniSection->DeleteValue( "TSEEnd" );
	}
	iniSection->SetValue( "TSE2TermOrderID",TSETerms );
	///< Update BrokerID
	if( TSEBrokerIDCheckBox->Checked == true )
	{
		AnsiString  BrokerID = TSEBrokerIDEdit->Text;
		iniSection->SetValue( "TSEBrokerID", BrokerID.c_str() );
	}
	else
		iniSection->DeleteValue( "TSEBrokerID" );
	if( FUTBrokerIDCheckBox->Checked == true )
	{
		AnsiString  BrokerID = FUTBrokerIDEdit->Text;
		iniSection->SetValue( "FUTBrokerID", BrokerID.c_str() );
	}
	else
		iniSection->DeleteValue( "FUTBrokerID" );
	///< Update Order Checker options.
	if( FormSetting->ForceRM() == false )
	{
		if( DefaultCheckerComboBox->ItemIndex == 0 )
			iniSection->SetValue( "DefCheckAction", "0" ); ///< Reject
		else
			iniSection->SetValue( "DefCheckAction", "1" ); ///< Pass
	}
	else
		iniSection->SetValue( "DefCheckAction", "0" ); ///< Reject
	if( FormSetting->ForceRM() == false )
	{
		if( TSEDefaultCheckerComboBox->ItemIndex == 0 )
			iniSection->SetValue( "TSEDefCheckAction", "0" ); ///< Reject
		else
			iniSection->SetValue( "TSEDefCheckAction", "1" ); ///< Pass
	}
	else
		iniSection->SetValue( "TSEDefCheckAction", "0" ); ///< Reject
	TFXChecker.TrimRight();
	if( FormSetting->ForceRM() == false && CheckerComboBox->ItemIndex == 0  )
		ini->SetValue( AE.c_str(), "Checker", "null" );
	else
		ini->SetValue( AE.c_str(), "Checker", TFXChecker.c_str() );
	TSEChecker.TrimRight();
	if( FormSetting->ForceRM() == false && TSECheckerComboBox->ItemIndex == 0  )
		ini->SetValue( AE.c_str(), "TSEChecker", "null" );
	else
		ini->SetValue( AE.c_str(), "TSEChecker", TSEChecker.c_str() );
	///< Update AE information.
	GetType( Type );
	iniSection->SetValue( "Type", Type );
	iniSection->SetValue( "IP", IP.c_str() );
	iniSection->SetValue( "Source", Source );
	iniSection->SetValue( "Group", Group );
	///< TAIFEX OrderSource and InfoSource
	if( OrdSrcComboBox->ItemIndex > 0 )
	{
		AnsiString Str = OrdSrcComboBox->Items->Strings[ OrdSrcComboBox->ItemIndex ];
		Str.SetLength( 1 );
		iniSection->SetValue( "OrderSource", Str.c_str() );
	}
	else
		iniSection->DeleteValue( "OrderSource" );
	if( InfoSrcEdit->Text.Length() == 3 )
	{
		AnsiString Str = InfoSrcEdit->Text;
		ini->SetValue( AE.c_str(), "MarketDataSource", Str.c_str() );
	}
	else
		iniSection->DeleteValue( "MarketDataSource" );
	///< Delete all accounts
	for( int i = iniSection->ItemCount()-1; i >= 0; i-- )
	{
		 UFC::AnsiString Name,Value;

		 iniSection->GetNameValue( i, Name,Value );
		 if( Value.Length() == 1 )
		 {
			 if( Value == "P" && Name != "OrderSource" )
				 iniSection->DeleteValue( Name );
			 if( Name.Length() == 7 || Name.Length() == 10 )
				 iniSection->DeleteValue( Name );
		 }
	}
	///< Add TAIFEX accounts
	for( int i = 0; i < AccountListView->Items->Count;i++ )
	{
		if( (UpdateListItem = AccountListView->Items->Item[i]) != NULL )
		{
			AnsiString Name  = "TFX" + UpdateListItem->Caption;
			AnsiString Value = UpdateListItem->SubItems->Strings[0];

			iniSection->SetValue( Name.c_str(), Value.c_str() );
		}
	}
	///< Add TWSE accounts
	for( int i = 0; i < TWSEAccountListView->Items->Count;i++ )
	{
		if( (UpdateListItem = TWSEAccountListView->Items->Item[i]) != NULL )
		{
			AnsiString Name  = "TSE" + UpdateListItem->Caption;
			AnsiString Value = UpdateListItem->SubItems->Strings[0];

			ini->SetValue( AE.c_str(), Name.c_str(), Value.c_str() );
		}
	}
	///< Add global futures accounts
	for( int i = 0; i < AccListBox->Items->Count;i++ )
	{
		AnsiString Name  = AccListBox->Items->Strings[i];
		AnsiString Value( "P" );;

		iniSection->SetValue( Name.c_str(), Value.c_str() );
	}

	if( VersionSheet->TabVisible == true )
	{
		ini->SetValue( AE.c_str(), "VerCount", UFC::AnsiString( VerListBox->Items->Count ) );
		for( int i = 0; i < VerListBox->Items->Count;i++ )
		{
			AnsiString VerStr = VerListBox->Items->Strings[i];

			ini->SetValue( AE.c_str(), UFC::AnsiString(i), VerStr.c_str());
		}
		ini->SetValue( AE.c_str(), "Level1", UFC::AnsiString( (int)Level1Spin->Value));
		ini->SetValue( AE.c_str(), "Level2", UFC::AnsiString( (int)Level2Spin->Value));
	}
	if( PATSTabSheet->TabVisible == true )
	{
		AnsiString PATSChecker = PATSCheckerComboBox->Text;

		PATSChecker.TrimRight();
		if( PATSOIDGroup->ItemIndex == 0 )
		{
			AnsiString  Prefix = PATSOIDPrefixEdit->Text;

			iniSection->SetValue( "PATSOrderIDPrefix", Prefix.c_str() );
			iniSection->DeleteValue( "PATSBegin" );
			iniSection->DeleteValue( "PATSEnd" );
		}
		else
		{
			AnsiString  Begin = PATSBeginOIDEdit->Text;
			AnsiString  End   = PATSEndOIDEdit->Text;

			iniSection->DeleteValue( "PATSOrderIDPrefix" );
			iniSection->SetValue( "PATSBegin", Begin.c_str() );
			iniSection->SetValue( "PATSEnd", End.c_str() );
		}
		if( PATSDefaultCheckerComboBox->ItemIndex == 0 )
			iniSection->SetValue( "PATSDefCheckAction", "0" ); ///< Reject
		else
			iniSection->SetValue( "PATSDefCheckAction", "1" ); ///< Pass
		if( PATSCheckerComboBox->ItemIndex == 0  )
			iniSection->SetValue( "PATSChecker", "null" );
		else
			iniSection->SetValue( "PATSChecker", PATSChecker.c_str() );
	}
	if( AE == "Speedy" )
	{
		UFC::Section*    OHiniSection = ini->GetSection( "SpeedyOffHour" );
		if( OHOIDGenGroup->ItemIndex == 1 )
		{
			AnsiString  Begin = OHBeginOIDEdit->Text;
			AnsiString  End   = OHEndOIDEdit->Text;

			OHiniSection->DeleteValue( "Terminals" );
			OHiniSection->SetValue( "Begin", Begin.c_str() );
			OHiniSection->SetValue( "End", End.c_str() );
		}
		else if( OIDGenGroup->ItemIndex == 2 )
		{
			AnsiString Terminals = OHTerminalsEdit->Text;

			OHiniSection->SetValue( "Terminals", Terminals.c_str() );
			OHiniSection->DeleteValue( "Begin" );
			OHiniSection->DeleteValue( "End" );
		}
		else if( OIDGenGroup->ItemIndex == 3 )
		{
			OHiniSection->SetValue( "UseGlobalTerminals", "1" );
			OHiniSection->DeleteValue( "Terminals" );
			OHiniSection->DeleteValue( "Begin" );
			OHiniSection->DeleteValue( "End" );
		}
		else
		{
			OHiniSection->DeleteValue( "UseGlobalTerminals" );
			OHiniSection->DeleteValue( "Terminals" );
			OHiniSection->DeleteValue( "Begin" );
			OHiniSection->DeleteValue( "End" );
		}
		UFC::AnsiString  OHTAIFEXTerms( OHTermsComboBox->ItemIndex );
		OHiniSection->SetValue( "TAIFEX3TermOrderID",OHTAIFEXTerms );
		if( OffHourCheckBox->Checked == true )
			iniSection->SetValue( "OffHourOtherRule", "1" );
		else
			iniSection->SetValue( "OffHourOtherRule", "0" );
	}

	if(UsersEdit->Text == "")
	{
		iniSection->DeleteValue("Users");
	}
	else
	{
		iniSection->SetValue( "Users", UsersEdit->Text.c_str() );
	}
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::AddAccButtonClick(TObject *Sender)
{
	AccountInputForm = new TAccountInputForm( this, false );
	if( AccountInputForm->ShowModal() == mrOk )
		AccListBox->Items->Add( AccountInputForm->AccountEdit->Text );
	delete AccountInputForm;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::DelAccButtonClick(TObject *Sender)
{
	int Del = AccListBox->ItemIndex;

	if( Del != -1 )
	{
		if( MessageDlg( Scstrings_MAIN_AE_DEL_AE_ASK + AccListBox->Items->Strings[ Del ] + L"?",
			mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
			AccListBox->Items->Delete( Del );
	}
	else
		ShowMessage( Scstrings_MAIN_AE_SEL_DEL_AE );///"請先選取欲刪除的帳號.");
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::AddAccountButtonClick(TObject *Sender)
{
	AccountInputForm = new TAccountInputForm( this );
	if( AccountInputForm->ShowModal() == mrOk )
	{
		TListItem*      NewListItem;

		NewListItem = AccountListView->Items->Add();
		NewListItem->Caption = AccountInputForm->AccountEdit->Text;
		NewListItem->SubItems->Add( AccountInputForm->AccountFlagEdit->Text );
	}
	delete AccountInputForm;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::DeleteAccountButtonClick(TObject *Sender)
{
	TListItem* Del = AccountListView->Selected;

	if( Del != NULL )
	{
		if( MessageDlg( Scstrings_MAIN_AE_DEL_AE_ASK + Del->Caption + L"?",
			mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
			AccountListView->DeleteSelected();
	}
	else
		ShowMessage( Scstrings_MAIN_AE_SEL_DEL_AE );///"請先選取欲刪除的帳號.");
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::GroupComboBoxChange(TObject *Sender)
{
	if( GroupComboBox->ItemIndex == 0 ) ///< For Admin
	{
		SourceComboBox->Enabled = true;
		AccountPanel->Visible   = true;
		UsersBitBtn->Enabled    = false;
	}
	else if( GroupComboBox->ItemIndex == 1 ) ///< For AE
	{
		SourceComboBox->Enabled = true;
		AccountPanel->Visible   = true;
		UsersBitBtn->Enabled    = true;
	}
	else ///For Channel
	{
		SourceComboBox->ItemIndex = 0; ///< Channel must use Speedy-API
		SourceComboBox->Enabled   = false;
		AccountPanel->Visible     = false;
		UsersBitBtn->Enabled      = true;
	}
	ClientHeight = BtnPanel->Top + BtnPanel->Height;
}

//---------------------------------------------------------------------------
void __fastcall TAccountForm::FUTBrokerIDCheckBoxClick(TObject *Sender)
{
	if( FUTBrokerIDCheckBox->Checked == true )
		FUTBrokerIDEdit->Enabled  = true;
	else
		FUTBrokerIDEdit->Enabled  = false;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::TSEBrokerIDCheckBoxClick(TObject *Sender)
{
	if( TSEBrokerIDCheckBox->Checked == true )
		TSEBrokerIDEdit->Enabled  = true;
	else
		TSEBrokerIDEdit->Enabled  = false;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::SetOIDGenType( int Type, bool IsTFX )
{
	if( IsTFX == true )
	{
		OIDGenGroup->ItemIndex = Type;
		OIDGenGroupClick( NULL );
	}
	else
	{
		TSEOIDGenGroup->ItemIndex = Type;
		TSEOIDGenGroupClick( NULL );
	}
}

//---------------------------------------------------------------------------
void __fastcall TAccountForm::SetPATSOIDGenType( int Type )
{
	PATSOIDGroup->ItemIndex = Type;
	PATSOIDGroupClick( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::OIDGenGroupClick(TObject *Sender)
{
	switch( OIDGenGroup->ItemIndex )
	{
		case 3:
		case 0: BeginOIDEdit->Enabled  = false;
				EndOIDEdit->Enabled    = false;
				TerminalsEdit->Enabled = false;
				EditBtn->Enabled       = false;
				break;
		case 1: BeginOIDEdit->Enabled  = true;
				EndOIDEdit->Enabled    = true;
				TerminalsEdit->Enabled = false;
				EditBtn->Enabled       = false;
				break;
		case 2: BeginOIDEdit->Enabled  = false;
				EndOIDEdit->Enabled    = false;
				TerminalsEdit->Enabled = true;
				EditBtn->Enabled       = true;
				break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::TSEOIDGenGroupClick(TObject *Sender)
{
	switch( TSEOIDGenGroup->ItemIndex )
	{
		case 3:
		case 0: TSEBeginOIDEdit->Enabled  = false;
				TSEEndOIDEdit->Enabled    = false;
				TSETerminalsEdit->Enabled = false;
				TSEEditBtn->Enabled       = false;
				break;
		case 1: TSEBeginOIDEdit->Enabled  = true;
				TSEEndOIDEdit->Enabled    = true;
				TSETerminalsEdit->Enabled = false;
				TSEEditBtn->Enabled       = false;
				break;
		case 2: TSEBeginOIDEdit->Enabled  = false;
				TSEEndOIDEdit->Enabled    = false;
				TSETerminalsEdit->Enabled = true;
				TSEEditBtn->Enabled       = true;
				break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::TFXEditBtnClick(TObject *Sender)
{
	TermForm = new TTermForm( this, true, TermsComboBox->ItemIndex, FUse62CarrySeq );
	TermForm->TermMemo->Lines->Delimiter = ';';
	TermForm->TermMemo->Lines->DelimitedText = TerminalsEdit->Text;
	TermForm->Num62CheckBox->Visible = true;
	if( TermForm->ShowModal() == mrOk)
	{
		TerminalsEdit->Text = TermForm->TermMemo->Lines->DelimitedText;
		FUse62CarrySeq = TermForm->Is62Carry();
	}
	delete TermForm;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::TSEEditBtnClick(TObject *Sender)
{
	TermForm = new TTermForm( this, false, TSETermsComboBox->ItemIndex, false );
	TermForm->TermMemo->Lines->Delimiter = ';';
	TermForm->TermMemo->Lines->DelimitedText = TSETerminalsEdit->Text;
	TermForm->Num62CheckBox->Visible = false;
	if( TermForm->ShowModal() == mrOk)
		TSETerminalsEdit->Text = TermForm->TermMemo->Lines->DelimitedText;
	delete TermForm;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::OHTermEditBtnClick(TObject *Sender)
{
	TermForm = new TTermForm( this, true, TermsComboBox->ItemIndex, FUse62CarrySeq );
	TermForm->TermMemo->Lines->Delimiter = ';';
	TermForm->TermMemo->Lines->DelimitedText = TerminalsEdit->Text;
	TermForm->Num62CheckBox->Visible = true;
	if( TermForm->ShowModal() == mrOk)
	{
		OHTerminalsEdit->Text = TermForm->TermMemo->Lines->DelimitedText;
		FUse62CarrySeq = TermForm->Is62Carry();
	}
	delete TermForm;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::BitBtn3Click(TObject *Sender)
{
	if( OIDGenGroup->ItemIndex == 1 )
	{
		AnsiString       BOID = BeginOIDEdit->Text;
		AnsiString       EOID = EndOIDEdit->Text;
		UFC::AnsiString  BeginOID( BOID.c_str());
		UFC::AnsiString  EndOID( EOID.c_str());
		String           Msg;
		BeginOID.PadThis( 5, '0');
		EndOID.PadThis( 5, '0');
		int ChkBeginInt = TFXOIDToInt( BeginOID, TermsComboBox->ItemIndex );
		int ChkEndInt = TFXOIDToInt( EndOID, TermsComboBox->ItemIndex );

		Msg.printf( Scstrings_MAIN_AE_TOTAL_ORDERID_COUNT.c_str(), ChkEndInt - ChkBeginInt );
		ShowMessage( Msg );///"共可產生[%d]個委託單號";
	}
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::AddVerButtonClick(TObject *Sender)
{
	VersionForm = new TVersionForm( this );
	if( VersionForm->ShowModal( ) == mrOk )
	{
		AnsiString Ver;

		Ver.printf("%d.%02d.%02d", VersionForm->MajorSpin->Value, VersionForm->MinorSpin->Value, VersionForm->BuildSpin->Value );
		if( VerListBox->Items->IndexOf( Ver ) == -1 )
			VerListBox->Items->Add( Ver );
		else
			ShowMessage( Scstrings_MAIN_AE_API_VER_ALREADY_EXISTS );///"此版本已存在.");
	}
	delete VersionForm;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::DelVerButtonClick(TObject *Sender)
{
	int Del = VerListBox->ItemIndex;

	if( Del != -1 )
		VerListBox->Items->Delete( Del );
	else
		ShowMessage( Scstrings_MAIN_AE_SEL_DEL_API_VER );///"請先選取欲刪除的版本.");
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::Level1SpinChange(TObject *Sender)
{
	if( Level1Spin->Value < Level2Spin->MaxValue )
		Level2Spin->MaxValue = Level1Spin->Value;
	if( Level2Spin->Value > Level2Spin->MaxValue )
		Level2Spin->Value = Level2Spin->MaxValue -1;
}
//---------------------------------------------------------------------------
bool __fastcall TAccountForm::CheckOIDOverlap( const UFC::AnsiString& CheckAE, int TFXBeginInt, int TFXEndInt, int TermDigi )
{
	AnsiString       ConfigFileName = FParentForm->GetUserConfigFileName( );
	UFC::UiniFile    ini( ConfigFileName.c_str() );
	UFC::AnsiString  BOID,EOID,AE;

	for( int i=0; i < ini.SectionCount(); i++ )
	{
		UFC::Section* iniSection = ini.GetSection( i );

		if( iniSection->GetSectionName() != CheckAE &&
			iniSection->GetValue( "Begin", BOID ) &&
			iniSection->GetValue( "End", EOID ) )
		{
			int ChkBeginInt = TFXOIDToInt( BOID, TermDigi );
			int ChkEndInt = TFXOIDToInt( EOID, TermDigi );

			if( (ChkBeginInt >= TFXBeginInt && ChkBeginInt <= TFXEndInt ) ||
				(ChkEndInt >= TFXBeginInt && ChkEndInt <= TFXEndInt ) ||
				(ChkBeginInt <= TFXBeginInt && ChkEndInt >= TFXEndInt ) )
			{
				String Msg,ErrAE;
				UFC::AnsiString OverlapAE(iniSection->GetSectionName());

				if( OverlapAE == "Speedy" )
					ErrAE = L"共用櫃號";
				else if(  OverlapAE == "SpeedyOffHour" )
					ErrAE = L"下午盤共用櫃號";
				else
					ErrAE = String( OverlapAE.c_str() );
				Msg.printf( Scstrings_MAIN_AE_ORDERID_RANGE_OVERLAP.c_str(),ErrAE );
				ShowMessage( Msg );
				return false;
			}
		}
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::OKBtnClick(TObject *Sender)
{
	AnsiString       CheckAE = IDEdit->Text;
	UFC::AnsiString  CurrentAE( CheckAE.c_str() );

	if( IDEdit->Text.Length() < 3 )
	{
		ShowMessage( Scstrings_MAIN_AE_ID_AT_LEAST_LEN_3 );///"帳號長度至少要三碼.");
		return;
	}
	if( PwdEdit->Text.Length() < 5 )
	{
		ShowMessage( Scstrings_MAIN_AE_PASSWORD_AT_LEAST_LEN_5 );///"密碼長度至少要五碼.");
		return;
	}
	if( PwdEdit->Text.Compare( PwdConfirmEdit->Text ) !=0 )
	{
		ShowMessage( Scstrings_MAIN_USER_PASSWORD_CONFIRM_NOT_EQUAL );///"密碼欄位與確認密碼欄位不同.");
		return;
	}
	if( FUTBrokerIDCheckBox->Checked == true && CheckFUTBrokerID( FUTBrokerIDEdit->Text ) == false )
	{
		ShowMessage( Scstrings_MAIN_AE_INPUT_VALID_TAIFEX_BROKERID );///"請填寫正確指定的期貨商代號.");
		return;
	}
	if( TSEBrokerIDCheckBox->Checked == true && CheckTSEBrokerID( TSEBrokerIDEdit->Text ) == false )
	{
		ShowMessage( Scstrings_MAIN_AE_INPUT_VALID_TSEC_BROKERID );///"請填寫正確指定的証券商代號.");
		return;
	}
	//if( PATSTabSheet->TabVisible == false && FormSetting->ForceRM() == true && CheckerComboBox->ItemIndex == 0 )
	//{
	//	ShowMessage( "必須指定一個風控程式" );
	//	return;
	//}
	if( OIDGenGroup->ItemIndex == 1 )
	{
		AnsiString BeginOID = BeginOIDEdit->Text;
		AnsiString EndOID = EndOIDEdit->Text;

		if( BeginOID.Length() != 5 || EndOID.Length() != 5 )
		{
			ShowMessage( Scstrings_MAIN_AE_TAIFEX_ORDERID_LEN_5 );///"期交所單號長度必須是五碼.");
			return;
		}
		if( CheckTFXOID( BeginOID.c_str(), TermsComboBox->ItemIndex )== false || CheckTFXOID( EndOID.c_str(), TermsComboBox->ItemIndex )== false )
		{
			ShowMessage( Scstrings_MAIN_AE_INVALID_TAIFEX_ORDERID );///"錯誤的期交所單號格式.");
			return;
		}
		int TFXBeginInt = TFXOIDToInt( BeginOID.c_str(), TermsComboBox->ItemIndex );
		int TFXEndInt   = TFXOIDToInt( EndOID.c_str(), TermsComboBox->ItemIndex );

		if( TFXBeginInt >= TFXEndInt )
		{
			ShowMessage( Scstrings_MAIN_AE_TAIFEX_ORDERID_END_LESS_BEGIN );
			return;
		}
		if( CheckOIDOverlap( CurrentAE, TFXBeginInt, TFXEndInt, TermsComboBox->ItemIndex ) == true )
		{
			if( CurrentAE == "Speedy" && OffHourCheckBox->Checked == true && OHOIDGenGroup->ItemIndex == 1 )
			{
				AnsiString OHBeginOID = OHBeginOIDEdit->Text;
				AnsiString OHEndOID = OHEndOIDEdit->Text;

				TFXBeginInt = TFXOIDToInt( OHBeginOID.c_str(), OHTermsComboBox->ItemIndex );
				TFXEndInt   = TFXOIDToInt( OHEndOID.c_str(), OHTermsComboBox->ItemIndex );
				if( CheckOIDOverlap( "SpeedyOffHour", TFXBeginInt, TFXEndInt, OHTermsComboBox->ItemIndex ) == false )
					return;
            }
		}
		else
			return;

	}
	else if( OIDGenGroup->ItemIndex == 2 )
	{
		TStringList* Terms = new TStringList();
		Terms->Delimiter = ';';
		Terms->DelimitedText = TerminalsEdit->Text;
		if( Terms->Count == 0 )
		{
			delete Terms;
			ShowMessage( Scstrings_MAIN_AE_INPUT_TAIFEX_TERM_LIST );///"請輸入櫃號清單" );
			return;
		}
		for( int i = 0; i < Terms->Count; i ++ )
		{
			if( TermsComboBox->ItemIndex == 1 )
			{
				if( Terms->Strings[i].Length() !=3 )
				{
					delete Terms;
					ShowMessage( Scstrings_MAIN_AE_TAIFEX_TERM_LEM_MUST + L"3.");///"櫃號長度必須是三碼" );
					return;
				}
			}
			else
			{
				if( Terms->Strings[i].Length() !=2 )
				{
					delete Terms;
					ShowMessage( Scstrings_MAIN_AE_TAIFEX_TERM_LEM_MUST + L"2.");///"櫃號長度必須是兩碼" );
					return;
				}
			}
		}
	}
	if( TSEOIDGenGroup->ItemIndex == 1 )
	{
		AnsiString TSEBeginOID = TSEBeginOIDEdit->Text;
		AnsiString TSEEndOID = TSEEndOIDEdit->Text;

		if( TSEBeginOID.Length() != 5 || TSEEndOID.Length() != 5 )
		{
			ShowMessage( Scstrings_MAIN_AE_TSEC_ORDERID_LEN_5 );///"證交所單號長度必須是五碼.");
			return;
		}
		if( CheckTSEOID( TSEBeginOID.c_str(),TSETermsComboBox->ItemIndex )== false || CheckTSEOID( TSEEndOID.c_str(),TSETermsComboBox->ItemIndex )== false )
		{
			ShowMessage( Scstrings_MAIN_AE_INVALID_TSEC_ORDERID );///"錯誤的證交所單號格式.");
			return;
		}
		int TSEBeginInt = TSEOIDToInt( TSEBeginOID.c_str(),TSETermsComboBox->ItemIndex );
		int TSEEndInt   = TSEOIDToInt( TSEEndOID.c_str(),TSETermsComboBox->ItemIndex );

		if( TSEBeginInt >= TSEEndInt )
		{
			ShowMessage( Scstrings_MAIN_AE_TSEC_ORDERID_END_LESS_BEGIN );
			return;
		}
		AnsiString       ConfigFileName = FParentForm->GetUserConfigFileName( );
		UFC::UiniFile    ini( ConfigFileName.c_str() );
		UFC::AnsiString  BOID,EOID,AE;

		for( int i=0; i < ini.SectionCount(); i++ )
		{
			UFC::Section* iniSection = ini.GetSection( i );
			if( iniSection->GetSectionName() != CurrentAE &&
				iniSection->GetValue( "TSEBegin", BOID ) &&
				iniSection->GetValue( "TSEEnd", EOID ) )
			{
				int ChkBeginInt = TSEOIDToInt( BOID,TSETermsComboBox->ItemIndex );
				int ChkEndInt   = TSEOIDToInt( EOID,TSETermsComboBox->ItemIndex );

				if( (ChkBeginInt >= TSEBeginInt && ChkBeginInt <= TSEEndInt ) ||
					(ChkEndInt >= TSEBeginInt && ChkEndInt <= TSEEndInt ) ||
					(ChkBeginInt <= TSEBeginInt && ChkEndInt >= TSEEndInt ) )
				{
					String Msg;

					Msg.printf( Scstrings_MAIN_AE_TSEC_ORDERID_RANGE_OVERLAP.c_str(),String(iniSection->GetSectionName().c_str()) );
					ShowMessage( Msg );
					return;
				}
			}
		}
	}
	else if( TSEOIDGenGroup->ItemIndex == 2 )
	{
		TStringList* Terms = new TStringList();
		Terms->Delimiter = ';';
		Terms->DelimitedText = TSETerminalsEdit->Text;
		if( Terms->Count == 0 )
		{
			delete Terms;
			ShowMessage( Scstrings_MAIN_AE_INPUT_TSEC_TERM_LIST );///"請輸入證交所櫃號清單" );
			return;
		}
		for( int i = 0; i < Terms->Count; i ++ )
		{
			if( TSETermsComboBox->ItemIndex == 1 )
			{
				if( Terms->Strings[i].Length() != 2 )
				{
					delete Terms;
					ShowMessage( Scstrings_MAIN_AE_TSEC_TERM_LEM_MUST + L"2.");///"證交所櫃號長度必須是2碼" );
					return;
				}
			}
			else
			{
				if( Terms->Strings[i].Length() != 1 )
				{
					delete Terms;
					ShowMessage( Scstrings_MAIN_AE_TSEC_TERM_LEM_MUST + L"1.");///"證交所櫃號長度必須是1碼" );
					return;
				}
			}
		}
	}
	if( PATSTabSheet->TabVisible == true )
	{
		if( PATSOIDGroup->ItemIndex == 1 )
		{
			AnsiString PATSBeginOID = PATSBeginOIDEdit->Text;
			AnsiString PATSEndOID = PATSEndOIDEdit->Text;

			if( PATSBeginOID.Length() != 5 || PATSEndOID.Length() != 5 )
			{
				ShowMessage( Scstrings_MAIN_AE_ORDERID_LEN_5 );///"單號長度必須是五碼.");
				return;
			}
			if( CheckPATSOID( PATSBeginOID.c_str() )== false ||
				CheckPATSOID( PATSEndOID.c_str())== false )
			{
				ShowMessage( Scstrings_MAIN_AE_INVALID_ORDERID );///"錯誤的單號格式.");
				return;
			}
			int PATSBeginInt = PATSOIDToInt( PATSBeginOID.c_str() );
			int PATSEndInt   = PATSOIDToInt( PATSEndOID.c_str() );

			if( PATSBeginInt >= PATSEndInt )
			{
				ShowMessage( Scstrings_MAIN_AE_ORDERID_END_LESS_BEGIN );
				return;
			}
		}
	}
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::TermsComboBoxChange(TObject *Sender)
{
	if( TermsComboBox->ItemIndex > 1 ) ///< 4,5 Terms
	{
		OIDGenGroup->Buttons[2]->Enabled = false;
		if( OIDGenGroup->ItemIndex == 2 )
			OIDGenGroup->ItemIndex = 1;
	}
	else ///< 2,3 Terms
	{
		OIDGenGroup->Buttons[2]->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TAccountForm::PATSOIDGroupClick(TObject *Sender)
{
	switch( PATSOIDGroup->ItemIndex )
	{
		case 0: PATSBeginOIDEdit->Enabled  = false;
				PATSEndOIDEdit->Enabled    = false;
				PATSOIDPrefixEdit->Enabled = true;
				break;
		case 1: PATSBeginOIDEdit->Enabled  = true;
				PATSEndOIDEdit->Enabled    = true;
				PATSOIDPrefixEdit->Enabled = false;
				break;
	}
}
//---------------------------------------------------------------------------

void __fastcall TAccountForm::NewTWSEAccountButtonClick(TObject *Sender)
{
	AccountInputForm = new TAccountInputForm( this );
	if( AccountInputForm->ShowModal() == mrOk )
	{
		TListItem*      NewListItem;

		NewListItem = TWSEAccountListView->Items->Add();
		NewListItem->Caption = AccountInputForm->AccountEdit->Text;
		NewListItem->SubItems->Add( AccountInputForm->AccountFlagEdit->Text );
	}
	delete AccountInputForm;
}
//---------------------------------------------------------------------------

void __fastcall TAccountForm::DelTWSEAccountButtonClick(TObject *Sender)
{
	TListItem* Del = TWSEAccountListView->Selected;

	if( Del != NULL )
	{
		if( MessageDlg( Scstrings_MAIN_AE_DEL_AE_ASK + Del->Caption + L"?",
			mtWarning, TMsgDlgButtons() << mbOK << mbCancel, 0 ) == mrOk )
			TWSEAccountListView->DeleteSelected();
	}
	else
		ShowMessage( Scstrings_MAIN_AE_SEL_DEL_AE );///"請先選取欲刪除的帳號.");
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::PageControlChange(TObject *Sender)
{
	if( PageControl->ActivePage == TFXTabSheet )
		AccPageControl->ActivePageIndex = 0;
	else if( PageControl->ActivePage == TSETabSheet )
		AccPageControl->ActivePageIndex = 1;
	else
		AccPageControl->ActivePageIndex = 0;
}
//---------------------------------------------------------------------------

void __fastcall TAccountForm::BitBtn1Click(TObject *Sender)
{
	if( OHOIDGenGroup->ItemIndex == 1 )
	{
		AnsiString       BOID = OHBeginOIDEdit->Text;
		AnsiString       EOID = OHEndOIDEdit->Text;
		UFC::AnsiString  BeginOID( BOID.c_str());
		UFC::AnsiString  EndOID( EOID.c_str());
		String           Msg;
		BeginOID.PadThis( 5, '0');
		EndOID.PadThis( 5, '0');
		int ChkBeginInt = TFXOIDToInt( BeginOID, OHTermsComboBox->ItemIndex );
		int ChkEndInt = TFXOIDToInt( EndOID, OHTermsComboBox->ItemIndex );

		Msg.printf( Scstrings_MAIN_AE_TOTAL_ORDERID_COUNT.c_str(), ChkEndInt - ChkBeginInt );
		ShowMessage( Msg );///"共可產生[%d]個委託單號";
	}
}
//---------------------------------------------------------------------------

void __fastcall TAccountForm::OHTermsComboBoxChange(TObject *Sender)
{
	if( OHTermsComboBox->ItemIndex > 1 ) ///< 4,5 Terms
	{
		OHOIDGenGroup->Buttons[2]->Enabled = false;
		if( OHOIDGenGroup->ItemIndex == 2 )
			OHOIDGenGroup->ItemIndex = 1;
	}
	else ///< 2,3 Terms
	{
		OHOIDGenGroup->Buttons[2]->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TAccountForm::OHOIDGenGroupClick(TObject *Sender)
{
	switch( OHOIDGenGroup->ItemIndex )
	{
		case 3:
		case 0: OHBeginOIDEdit->Enabled  = false;
				OHEndOIDEdit->Enabled    = false;
				OHTerminalsEdit->Enabled = false;
				OHTermEditBtn->Enabled   = false;
				break;
		case 1: OHBeginOIDEdit->Enabled  = true;
				OHEndOIDEdit->Enabled    = true;
				OHTerminalsEdit->Enabled = false;
				OHTermEditBtn->Enabled   = false;
				break;
		case 2: OHBeginOIDEdit->Enabled  = false;
				OHEndOIDEdit->Enabled    = false;
				OHTerminalsEdit->Enabled = true;
				OHTermEditBtn->Enabled   = true;
				break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::OffHourCheckBoxClick(TObject *Sender)
{
	OHOIDGenGroup->Enabled   = OffHourCheckBox->Checked;
	OHTermsComboBox->Enabled = OffHourCheckBox->Checked;
	BitBtn1->Enabled         = OffHourCheckBox->Checked;
	OHTermEditBtn->Enabled   = OffHourCheckBox->Checked;
	OHBeginOIDEdit->Enabled  = OffHourCheckBox->Checked;
	OHEndOIDEdit->Enabled    = OffHourCheckBox->Checked;
	OHTerminalsEdit->Enabled = OffHourCheckBox->Checked;
	if( OffHourCheckBox->Checked == true )
		OHOIDGenGroupClick( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::TSETermsComboBoxChange(TObject *Sender)
{
	if( TSETermsComboBox->ItemIndex > 1 ) ///< 3,4,5 Terms
	{
		TSEOIDGenGroup->Buttons[2]->Enabled = false;
		if( TSEOIDGenGroup->ItemIndex == 2 )
			TSEOIDGenGroup->ItemIndex = 1;
	}
	else
		TSEOIDGenGroup->Buttons[2]->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TAccountForm::UsersBitBtnClick(TObject *Sender)
{
	UsersList = new TUsersList(this, FParentForm, UsersEdit->Text);
	if( UsersList->ShowModal() == mrOk )
		UsersEdit->Text = UsersList->GetUsers().c_str();
	delete UsersList;
	UsersList = NULL;
}
//---------------------------------------------------------------------------

