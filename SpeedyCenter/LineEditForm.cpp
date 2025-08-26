//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "LineEditForm.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPVCSettingForm *PVCSettingForm;
//---------------------------------------------------------------------------
//  Create a new session
//---------------------------------------------------------------------------
__fastcall TPVCSettingForm::TPVCSettingForm(TComponent* Owner, MarketSet& Market)
	: TForm(Owner)
{
	SetMarketComboBox( Market );
	SysComboBox->ItemIndex = 0;
	if( SysComboBox->Items->Strings[ SysComboBox->ItemIndex ].Compare( Scstrings_MAIN_TSEC ) != 0 &&
		SysComboBox->Items->Strings[ SysComboBox->ItemIndex ].Compare( Scstrings_MAIN_OTC ) != 0 )
		SetExchange( false );
	else
		SetExchange( true );
	PVCEdit->Text     = L"1";
	EnabledCheckBox->Checked = true; ///< Default: Session enabled
	OffHourCheckBox->Checked = false;///< Default: Normal trading session
}
//---------------------------------------------------------------------------
//  Modify or copy an existing session
//---------------------------------------------------------------------------
__fastcall TPVCSettingForm::TPVCSettingForm(TComponent* Owner, TLineInfo* info, MarketSet& Market, bool IsCopy )
	: TForm(Owner),
	FIsCopy( IsCopy )
{
	SetMarketComboBox( Market );
	if( IsCopy )
		Caption = Scstrings_MAIN_PVC_SETTING_ADD_NEW_SESSION;///<"新增連線";
	else
		Caption = Scstrings_MAIN_PVC_SETTING_MODIFY_SESSION;///<"修改連線屬性";
	switch( info->GetSystem() )
	{
	  case mtFutures:  SysComboBox->ItemIndex = SysComboBox->Items->IndexOf( Scstrings_MAIN_FUTURES );
					   SetExchange( false );
					   break;
	  case mtOption:   SysComboBox->ItemIndex = SysComboBox->Items->IndexOf( Scstrings_MAIN_OPTIONS );
					   SetExchange( false );
					   break;
	  case mtStock:    SysComboBox->ItemIndex = SysComboBox->Items->IndexOf( Scstrings_MAIN_TSEC);
					   SetExchange( true );
					   break;
	  case mtOTC:      SysComboBox->ItemIndex = SysComboBox->Items->IndexOf( Scstrings_MAIN_OTC);
					   SetExchange( true );
					   break;
	}
	SetAPCode( info->GetAPCode(), info->GetSystem() );
	SysComboBox->Enabled = false;     ///< Can not change market.
	LinkEdit->Text    = IntToStr( info->GetLink());
	PVCEdit->Text     = IntToStr( info->GetPVC());
	FCMEdit->Text     = info->GetBrokerID();
	LinePWDEdit->Text = info->GetPassword();
	PVCIDEdit->Text   = info->GetPVCID();
	EnabledCheckBox->Checked = info->IsEnabled();
	OffHourCheckBox->Checked = info->GetOffHour();

	if( info->GetGroup() == 99 )
		GroupEdit->Text = L"all";
	else if( info->GetGroup() == -1 )
		GroupEdit->Text = L"0";
	else
		GroupEdit->Text = IntToStr( info->GetGroup() );
	if( info->GetSystem() == mtStock || info->GetSystem() == mtOTC )
	{
		CMEdit->Text = info->GetIP();
		LocalPortURLEdit->Text = IntToStr( info->GetLocalPort() );
		FOrgPVCID = info->GetPVCID();
		int index = (info->GetThroughput()/20)-1;

		if( index < 0 ) index = 0;
		if( index > 9 ) index = 9;
		if( info->GetPVC() == 99 )
		{
			ProtocolComboBox->ItemIndex = 1;
			TWSETPComboBox->ItemIndex = index;
			TWSETPComboBox->Visible = true;
			TWSETPLabel->Visible    = true;
		}
		else
		{
			ProtocolComboBox->ItemIndex = 0;
			TWSETPComboBox->ItemIndex = 0;
			TWSETPComboBox->Visible = false;
			TWSETPLabel->Visible    = false;
		}
	}
	else
	{
		LocalPortURLEdit->Text = info->GetIP();
		FOrgPVCID = info->GetIP();
	}
	APCodeComboBox->Enabled = false;  ///< Can not change AP Code.
	APCodeComboBoxChange( NULL);
	FOrgLink = info->GetLink();
	FOrgPVC  = info->GetPVC();
	FOrgFCM  = FCMEdit->Text;
	FOrgCM   = CMEdit->Text;
	FOrgPWD  = LinePWDEdit->Text;
	FOrgLocalPort = info->GetLocalPort();
	FOrgGroup = info->GetGroup();
	FOrgIsEnabled  = info->IsEnabled();
	FOrgIsOffHour  = info->GetOffHour();
	FOrgTPIndex    = TWSETPComboBox->ItemIndex;
}
//---------------------------------------------------------------------------
void __fastcall TPVCSettingForm::SetMarketComboBox( MarketSet& Market )
{
    if( Market.Contains( mtFutures ) )
		SysComboBox->Items->Add( Scstrings_MAIN_FUTURES );
    if( Market.Contains( mtOption ) )
		SysComboBox->Items->Add( Scstrings_MAIN_OPTIONS );
	if( Market.Contains( mtStock ) )
		SysComboBox->Items->Add( Scstrings_MAIN_TSEC );
	if( Market.Contains( mtOTC ) )
		SysComboBox->Items->Add( Scstrings_MAIN_OTC );
}
//---------------------------------------------------------------------------
TMarket __fastcall TPVCSettingForm::GetMarket()
{
	AnsiString MarketName = SysComboBox->Items->Strings[ SysComboBox->ItemIndex ];

	if( MarketName.AnsiCompare( Scstrings_MAIN_OPTIONS ) == 0 )
		return mtOption;
	else if( MarketName.AnsiCompare( Scstrings_MAIN_FUTURES ) == 0 )
		return mtFutures;
	else if( MarketName.AnsiCompare( Scstrings_MAIN_TSEC ) == 0 )
		return mtStock;
	else
		return mtOTC;
}
//---------------------------------------------------------------------------
void __fastcall TPVCSettingForm::SetAPCodeCombobox( bool IsTSEC )
{
	APCodeComboBox->Items->Clear();
	if( IsTSEC ) ///< for TSE OTC
	{
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TSE_NORMAL );//"0:普通(交易所0)");  0
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TSE_FTP_RECV );//"1:檔案傳輸(接收,交易所1)"); 1
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TSE_FTP_SEND );//"2:檔案傳輸(傳送,交易所1)"); 2
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TSE_EXECUTION );//"3:成交回報(交易所3)"); 3
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TSE_NORMAL_ODD );//"4:零股/普通(交易所2)"); 4
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TSE_LEND );//"5:標借(交易所4)"); 5
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TSE_AUCTION );//"6:拍賣(交易所5)"); 6
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TSE_TENDER );//"8:標購(交易所6)"); 7
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TSE_NORMAL_POST );//"9:定價/普通(交易所7)"); 8
		APCodeComboBox->Items->Add( L"C:盤中零股" );//"C:盤中零股" 9
	}
	else
	{
		///< TAIFEX TCP/IP TMP protocol
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TMP_ORDER );//"4:委託");
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TMP_CLEAR_MEMBER );//"8:結算會員");
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TMP_EXECUTION );//"3:成交回報");
		APCodeComboBox->Items->Add( Scstrings_MAIN_PVC_SETTING_TMP_CM_FILLED );//"9:結算成回");
	}
}
//---------------------------------------------------------------------------
void __fastcall TPVCSettingForm::SetExchange( bool IsTSEC )
{
	if( IsTSEC )
	{
		LocalPortURLLabel->Caption  = L"Local Port";
		LocalPortURLEdit->Width     = 65;
		LocalPortURLEdit->MaxLength = 6;
		LocalPortURLEdit->Visible   = true;
		LocalPortURLLabel->Visible  = true;

		PVCIDLabel->Visible     = true;
		PVCIDEdit->Visible      = true;
		CMEdit->Visible         = true;
		CMorIPLabel->Visible    = true;

		CMorIPLabel->Caption    = Scstrings_MAIN_IP_ADDRESS;
		CMEdit->MaxLength = 15;
		CMEdit->Width = 150;
		BrokerLabel->Caption = Scstrings_MAIN_TSEC_BROKER_ID;
		FCMEdit->MaxLength = 4;
		LinkEdit->MaxLength = 6;
		SetAPCodeCombobox( true);

		OffHourCheckBox->Visible   = false;
		LocalPortURLEdit->Enabled  = true;
		LocalPortURLLabel->Enabled = true;
		PVCLabel->Caption  = L"PVC";
		PortLabel->Caption = L"TCP Port";
		PVCLabel->Visible = false;
		PVCEdit->Visible = false;
		TWSETPComboBox->Visible = true;
		TWSETPLabel->Visible    = true;
		if( APCodeComboBox->ItemIndex == 0  )
		{
			ProtocolComboBox->Visible = true;
			ProtocolLabel->Visible = true;
		}
		else
		{
			ProtocolComboBox->Visible = false;
			ProtocolLabel->Visible = false;
		}
		//EnabledCheckBox->Visible = true;
	}
	else
	{
		LocalPortURLLabel->Caption = L"URL";
		LocalPortURLEdit->Width    = 180;
		LocalPortURLEdit->MaxLength = 32;
		LocalPortURLEdit->Visible  = false;
		LocalPortURLLabel->Visible = false;

		PVCIDLabel->Visible     = false;
		PVCIDEdit->Visible      = false;
		CMEdit->Visible         = false;
		CMorIPLabel->Visible    = false;

		BrokerLabel->Caption = Scstrings_MAIN_TAIFEX_BROKER_ID;
		FCMEdit->MaxLength = 7;
		LinkEdit->MaxLength = 6;
		SetAPCodeCombobox( false);

		OffHourCheckBox->Visible   = true;
		ProtocolComboBox->Visible = false;
		ProtocolLabel->Visible = false;
		//EnabledCheckBox->Visible = false;
		PVCLabel->Visible = true;
		PVCEdit->Visible = true;
		PVCLabel->Caption  = L"Session ID";
		PortLabel->Caption = L"Local Port";

		TWSETPComboBox->Visible = false;
		TWSETPLabel->Visible    = false;
	}
}
//---------------------------------------------------------------------------
int __fastcall TPVCSettingForm::GetLocalPort()
{
	if( GetMarket() == mtStock || GetMarket() == mtOTC )
		return FLocalPort;
	else
		return 0;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPVCSettingForm::GetPVCID()
{
	if( GetMarket() == mtStock || GetMarket() == mtOTC )
		return FPVCID;
	AnsiString url,ProtocolName;

	if( GetAPCode( ) == 4 )
		ProtocolName = "tmp";
	else
		ProtocolName = "tmpcm";
	if(GetMarket() == mtFutures )
	   url.printf("%s.session%d.%s.fut.taifex",FFCM.c_str(),FPVC,ProtocolName.c_str());
	else
	   url.printf("%s.session%d.%s.opt.taifex",FFCM.c_str(),FPVC,ProtocolName.c_str());
	return url.LowerCase();
}
//---------------------------------------------------------------------------
bool  __fastcall TPVCSettingForm::IsModified()
{
	if( FOrgFCM.AnsiCompare( FFCM ) == 0 &&
		FOrgCM.AnsiCompare( FCM ) == 0 &&
		FOrgPWD.AnsiCompare( FPWD ) == 0 &&
		FOrgPVCID.AnsiCompare( FPVCID ) == 0 && ///<  as URL
		FOrgLink == FLink &&
		FOrgPVC  == FPVC &&
		FOrgLocalPort == FLocalPort &&
		FOrgGroup == FGroup &&
		FOrgIsEnabled  == FIsEnabled &&
		FOrgIsOffHour == FIsOffHour &&
		FOrgTPIndex   == TWSETPComboBox->ItemIndex )
		return false;
	return true;
}
//---------------------------------------------------------------------------
bool  __fastcall TPVCSettingForm::IsConnectionModified()
{
    if( FOrgLink == FLink && FOrgPVC  == FPVC )
        return false;
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TPVCSettingForm::SysComboBoxChange(TObject *Sender)
{
	if( SysComboBox->Items->Strings[ SysComboBox->ItemIndex ].Compare( Scstrings_MAIN_TSEC ) != 0 &&
		SysComboBox->Items->Strings[ SysComboBox->ItemIndex ].Compare( Scstrings_MAIN_OTC ) != 0 )
		SetExchange( false );
	else
		SetExchange( true );
}
//---------------------------------------------------------------------------
void __fastcall TPVCSettingForm::APCodeComboBoxChange(TObject *Sender)
{
	int ApCode = GetAPCode();

	if( SysComboBox->Items->Strings[ SysComboBox->ItemIndex ].Compare( Scstrings_MAIN_TSEC ) == 0 ||
		SysComboBox->Items->Strings[ SysComboBox->ItemIndex ].Compare( Scstrings_MAIN_OTC  ) == 0 )
	{   ///< TWSE,OTC
		if( ApCode == 0 )  ///< Only Normal trading session
		{                  ///< Support FIX protocol
			ProtocolComboBox->Visible = true;
			ProtocolLabel->Visible = true;
		}
		else
		{
			ProtocolComboBox->Visible = false;
			ProtocolLabel->Visible = false;
		}
		///< Only trading session support group
		if( ApCode == 0 || ApCode == 4 || ApCode == 5 || ApCode == 6 ||
			ApCode == 7 || ApCode == 8 || ApCode == 9 || ApCode == 10  )
		{
			GroupEdit->Visible = true;
			GroupLabel->Visible = true;
		}
		else
		{
			GroupEdit->Visible = false;
			GroupLabel->Visible = false;
		}
	}
	else
	{
		///< Only trading session support group
		if( ApCode == 4 )
		{
			GroupEdit->Visible = true;
			GroupLabel->Visible = true;
		}
		else
		{
			GroupEdit->Visible = false;
			GroupLabel->Visible = false;
		}
	}

}
//---------------------------------------------------------------------------
int __fastcall TPVCSettingForm::GetAPCode()
{
	if( GetMarket() == mtStock || GetMarket() == mtOTC )
	{
		switch( APCodeComboBox->ItemIndex )
		{
			case 0: return 0;
			case 1: return 1;
			case 2: return 2;
			case 3: return 3;
			case 4: return 4;
			case 5: return 5;
			case 6: return 6;
			case 7: return 8;
			case 8: return 9;
			case 9: return 10;
			case 'C': return 10;
		}
	}
	else
	{
		switch( APCodeComboBox->ItemIndex )
		{
			case 0: return 4;
			case 1: return 8;
			case 2: return 3;
			case 3: return 9;
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
void __fastcall TPVCSettingForm::SetAPCode( int APCode, TMarket sys )
{
	if( sys == mtStock || sys == mtOTC )
	{
		switch( APCode )
		{
			case 0: APCodeComboBox->ItemIndex = 0;break;
			case 1: APCodeComboBox->ItemIndex = 1;break;
            case 2: APCodeComboBox->ItemIndex = 2;break;
            case 3: APCodeComboBox->ItemIndex = 3;break;
            case 4: APCodeComboBox->ItemIndex = 4;break;
			case 5: APCodeComboBox->ItemIndex = 5;break;
			case 6: APCodeComboBox->ItemIndex = 6;break;
			case 8: APCodeComboBox->ItemIndex = 7;break;
			case 9: APCodeComboBox->ItemIndex = 8;break;
			case 10: APCodeComboBox->ItemIndex = 9;break;
			case 'C': APCodeComboBox->ItemIndex = 9;break;
		}
	}
	else
	{
		switch( APCode )
		{
			case 4: APCodeComboBox->ItemIndex = 0;break;
			case 8: APCodeComboBox->ItemIndex = 1;break;
			case 3: APCodeComboBox->ItemIndex = 2;break;
			case 9: APCodeComboBox->ItemIndex = 3;break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TPVCSettingForm::OKBitBtnClick(TObject *Sender)
{
	TMarket market = GetMarket();

	FPWD = LinePWDEdit->Text;
	if( market == mtStock || market == mtOTC )
	{
		FFCM = FCMEdit->Text;
		FCM  = CMEdit->Text;
		if( CheckTSEC() == false )
			return;
	}
	else
	{
		FFCM = FCMEdit->Text.UpperCase();
		FCM  = CMEdit->Text.UpperCase();
		if( CheckTAIFEX() == false )
			return;
	}
	try
	{
		if( FPWD.Length() != 4)
		{
			ShowMessage( Scstrings_MAIN_PVC_SETTING_PASSWORD_LEN_SHOULD_4 );
            return;
        }
        FPWD.ToInt();
    }
    catch(...)
    {
		ShowMessage( Scstrings_MAIN_PVC_SETTING_PASSWORD_LEN_SHOULD_4 );
		return;
    }
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
bool __fastcall TPVCSettingForm::CheckTAIFEX( )
{
    if( FFCM.Length() != 7)
    {
		ShowMessage( Scstrings_MAIN_PVC_SETTING_BROKERID_LEN_SHOULD_7 );
        return false;
    }
    if( FFCM[1] < 'A' || FFCM[1] > 'Z' )
    {
		ShowMessage( Scstrings_MAIN_PVC_SETTING_BROKERID_FIRST_A_2_Z );
        return false;
    }
    try
	{
		AnsiString Num = FFCM.SubString( 2, 6 );
		Num.ToInt();
	}
    catch(...)
    {
		ShowMessage( Scstrings_MAIN_PVC_SETTING_BROKERID_LAST_6_NUM );
        return false;
    }
    try
	{
	   FPVC = PVCEdit->Text.ToInt();
	   if( FPVC < 1 || FPVC > 999 )
	   {
		   ShowMessage( Scstrings_MAIN_PVC_SETTING_SESSIONID_3DIGI );
		   return false;
	   }
	}
	catch(...)
	{
		ShowMessage( Scstrings_MAIN_PVC_SETTING_SESSIONID_SHOULD_NUM );
		return false;
	}
	try
	{
	   FLink = LinkEdit->Text.ToInt();
	}
	catch(...)
	{
	   ShowMessage( Scstrings_MAIN_PVC_SETTING_PORT_MUST_NUM );
	   return false;
	}
	if( APCodeComboBox->ItemIndex != 0 && FCM.Length() != 0 )
	{
		if( FCM.Length() != 4 )
		{
			ShowMessage( Scstrings_MAIN_PVC_SETTING_CM_MUST_LEN4 );
			return false;
		}
		if( FCM[1] < 'A' || FCM[1] > 'Z' )
		{
			ShowMessage( Scstrings_MAIN_PVC_SETTING_CM_FIRST_MUST_A_2_Z );
			return false;
		}
		try
		{
			AnsiString Num = FCM.SubString( 2,3);
			Num.ToInt();
		}
		catch(...)
		{
			ShowMessage( Scstrings_MAIN_PVC_SETTING_CM_LAST_3_MUST_NUM);
			return false;
		}
	}
	FPVCID = LocalPortURLEdit->Text;
	try
	{
		if( GroupEdit->Text == L"all" )
			FGroup = 99;
		else
			FGroup = GroupEdit->Text.ToInt();
	}
	catch(...)
	{
		FGroup = FOrgGroup;
	}
	if( FGroup == -1 )
		FGroup = 0;
	FIsEnabled  = EnabledCheckBox->Checked;
	FIsOffHour  = OffHourCheckBox->Checked;
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall TPVCSettingForm::CheckTSEC( )
{
    if( FFCM.Length() != 4)
	{
		ShowMessage( Scstrings_MAIN_PVC_SETTING_TSEC_BROKERID_SHOULD_4 );
        return false;
    }
    try
    {
       FPVC = PVCEdit->Text.ToInt();
       if( FPVC < 1 || FPVC > 99 )
       {
		   ShowMessage( Scstrings_MAIN_PVC_SETTING_TSEC_PVC_BETWEEN_1_99 );
		   return false;
       }
    }
    catch(...)
	{
	   ShowMessage( Scstrings_MAIN_PVC_SETTING_PVC_SHOULD_NUM );
       return false;
    }
	FPVCID = PVCIDEdit->Text;
	if( FPVCID.Length() != 2 )
    {
	   ShowMessage( Scstrings_MAIN_PVC_SETTING_PVC_ID_2DIGI );
       return false;
	}
    try
    {
       FLink = LinkEdit->Text.ToInt();
	   if( FLink < 0 || FLink > 65536 )
	   {
		   ShowMessage( Scstrings_MAIN_PVC_SETTING_PORT_BETWEEN_0_65535 );
           return false;
       }
    }
    catch(...)
    {
	   ShowMessage( Scstrings_MAIN_PVC_SETTING_PORT_MUST_NUM );
	   return false;
	}
	if( ProtocolComboBox->ItemIndex == 0 && FPVC == 99 )
		FPVC = 1;
	if( (APCodeComboBox->ItemIndex == 0 ||
		 APCodeComboBox->ItemIndex == 4 ||
		 APCodeComboBox->ItemIndex == 9 ) &&
		ProtocolComboBox->ItemIndex == 1 )
		FPVC = 99; ///< FIX Session
	if( IsValidIP( FCM ) == false )
	{
	   ShowMessage( Scstrings_MAIN_PVC_SETTING_WRONG_IP_ADDRESS );
	   return false;
	}
	try
	{
	   FLocalPort = LocalPortURLEdit->Text.ToInt();
	   if( FLocalPort < 1 || FLocalPort > 65536 )
	   {
		   ShowMessage( Scstrings_MAIN_PVC_SETTING_TCP_PORT_BETWEEN_1_65535 );
		   return false;
	   }
	}
	catch(...)
	{
	   ShowMessage( Scstrings_MAIN_PVC_SETTING_PORT_MUST_NUM );
	   return false;
	}
	try
	{
		if( GroupEdit->Text == L"all" )
			FGroup = 99;
		else
			FGroup = GroupEdit->Text.ToInt();
	}
	catch(...)
	{
		FGroup = FOrgGroup;
	}
	if( FGroup == -1 )
		FGroup = 0;
	FIsEnabled  = EnabledCheckBox->Checked;
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TPVCSettingForm::FormShow(TObject *Sender)
{
	if( FIsCopy == true )
	{
		if( PVCEdit->Visible == true )
			PVCEdit->SetFocus();
		else if( PVCIDEdit->Visible == true )
			PVCIDEdit->SetFocus();
	}
}
//---------------------------------------------------------------------------
int __fastcall TPVCSettingForm::GetThreougput()
{
	TMarket Mkt = GetMarket();
	if( Mkt == mtStock || Mkt == mtOTC )
	{
		if( ProtocolComboBox->ItemIndex == 1 )
			return (TWSETPComboBox->ItemIndex+1)*20;
		else
			return 20;
	}
	else
		return 16;
}
//---------------------------------------------------------------------------

