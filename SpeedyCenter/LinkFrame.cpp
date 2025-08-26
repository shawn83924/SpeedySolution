//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "LinkFrame.h"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
__fastcall TLineInfo::TLineInfo( AnsiString FCM,
								 AnsiString Pwd,
								 int        Link,
								 int        PVCNo,
								 int        APCode,
								 TMarket    Market,
								 int        Group,
								 AnsiString CMorIP,   ///<CMID or IP
								 int        LocalPort,
								 AnsiString PVCID,    ///< PVCID or URL
								 bool       Enable,
								 int        Throughput )
:FLink( Link ),
 FPVC( PVCNo ),
 FAPCode( APCode ),
 FPassword( Pwd ),
 FState( PVC_STATUS_INIT ),
 FLocalPort( LocalPort ),
 FMarket( Market ),
 FPVCID( PVCID ),
 FColor( clBlack ),
 FFCMNo( FCM ),
 FCMNo( "" ),
 FGroup( Group ),
 FIsEnabled( Enable ),
 FThroughput( Throughput ),
 FInSeq( 1 ),
 FOutSeq( 1 ),
 FBackupIndex( 0 )
{
	FIPList = new TStringList();
	if( Market == mtStock || Market == mtOTC ) ///< It's a Stock session.
	{
		if( APCode == 'C' )
			FAPCode = 10;
		if( IsValidIP( CMorIP ))
		{
			//FIP = CMorIP;
			FIPList->Add( CMorIP );
			FIsTCP = true;
		}
		else
		{
			FIPList->Add( "127.0.0.1" );
			FIsTCP = false;
		}
	}
	else  ///< It's a TAIFEX session.
	{
		FCMNo = CMorIP;
		if( PVCID.AnsiCompare("00") != 0)
		{
			//FIP    = PVCID;
			FIPList->Delimiter = '^';
			FIPList->DelimitedText = PVCID;
			FIsTCP = true;
		}
		else ///< TAIFEX x.25 line.
		{
			FIPList->Add( "127.0.0.1" );
			FIsTCP = false;
		}
	}
}
//---------------------------------------------------------------------------
bool __fastcall TLineInfo::SetState( int State )
{
    bool rtn = true;

	if( FState != State )
	{
		int OrigModState = FState%100;
		int NewModState  = State%100;
		FState = State;

		if( OrigModState == PVC_STATUS_ONLINE_BUSY || OrigModState == PVC_STATUS_ONLINE_READY ||
			OrigModState == PVC_STATUS_TRY_OPEN    || OrigModState == PVC_STATUS_WAIT_TRY ||
			OrigModState == PVC_STATUS_WAIT_OPEN ) ///< PVC Connected!!
		{
			if( NewModState != PVC_STATUS_ONLINE_BUSY && NewModState != PVC_STATUS_ONLINE_READY &&
				NewModState != PVC_STATUS_TRY_OPEN    && NewModState != PVC_STATUS_WAIT_TRY &&
				NewModState != PVC_STATUS_WAIT_OPEN )
			rtn = false; ///< PVC Broken!!
		}
		switch( NewModState )
		{
			case PVC_STATUS_TRY_OPEN     :
			case PVC_STATUS_WAIT_TRY     :
			case PVC_STATUS_WAIT_OPEN    :  FColor = clNavy;  break;
			case PVC_STATUS_ONLINE_BUSY  :
			case PVC_STATUS_ONLINE_READY :  FColor = clGreen; break;
			case PVC_STATUS_DISABLE      :
			case PVC_STATUS_UNKNOWN      :
			case PVC_STATUS_INIT         :  FColor = clGray;  break;
			case PVC_STATUS_UNKNOWNPVC   :
			case PVC_STATUS_FINISHED     :
			case PVC_STATUS_SUBSYS_ERR   :
			case PVC_STATUS_BACKUP       :
			case PVC_STATUS_PW_ERR       :
			case PVC_STATUS_FCM_ERR      :
			case PVC_STATUS_AP_ERR       :
			case PVC_STATUS_BROKEN       :
			case PVC_STATUS_OFFLINE_BUSY :
			case PVC_STATUS_SLEEPING     :
			case PVC_STATUS_OFFLINE_READY:
			default						 :  FColor = clRed;   break;
		}
    }
    return rtn;
}
//---------------------------------------------------------------------------
int __fastcall TLineInfo::GetIPCount( void )
{
	return FIPList->Count;
}
//---------------------------------------------------------------------------
AnsiString& __fastcall TLineInfo::GetIP( int Index )
{
	if( Index >= 0  )
	{
		if( FIPList->Count > Index )
			FCurrIP = FIPList->Strings[ Index ];
		else
			FCurrIP = FIPList->Strings[ 0 ];
	}
	else
		FCurrIP = FIPList->Strings[ FBackupIndex ];
	return FCurrIP;
}
//---------------------------------------------------------------------------
void __fastcall TLineInfo::SetIP( const AnsiString& IP )
{
	if( FIPList->Count > FBackupIndex )
		FIPList->Strings[ FBackupIndex ] = IP;
	else
		FIPList->Add( IP );
	FIsTCP = true;
}
//---------------------------------------------------------------------------
String __fastcall TLineInfo::GetState( void )
{
	switch( FState%100 )
	{
		case PVC_STATUS_INIT         :  FColor = clGray;
										return L"---";
		case PVC_STATUS_ENABLE       :  FColor = clRed;
										return Scstrings_MAIN_PROCESS_STAT_ACTIVEING;///"啟用中";
		case PVC_STATUS_DISABLE      :  FColor = clGray;
										return Scstrings_MAIN_PROCESS_STAT_DISABLED;///"停用";
		case PVC_STATUS_UNKNOWNPVC   :  FColor = clRed;
										return Scstrings_MAIN_PROCESS_STAT_NOT_ACTIVE;///"無啟動";
		case PVC_STATUS_DISABLEING   :  FColor = clRed;
										return Scstrings_MAIN_PROCESS_STAT_DISABLEING;///"停用中";
		case PVC_STATUS_SLEEPING     :  FColor = clRed;
										return Scstrings_MAIN_PROCESS_STAT_SLEEPING;///"休眠";
		case PVC_STATUS_FINISHED     :
		case PVC_STATUS_SUBSYS_ERR   :
		case PVC_STATUS_BACKUP       :
		case PVC_STATUS_PW_ERR       :
		case PVC_STATUS_FCM_ERR      :
		case PVC_STATUS_AP_ERR       :
		case PVC_STATUS_BROKEN       :
		case PVC_STATUS_OFFLINE_BUSY :
		case PVC_STATUS_OFFLINE_READY:  FColor = clRed;
										return Scstrings_MAIN_SESSION_STAT_BROKEN;///"斷線";
		case PVC_STATUS_TRY_OPEN     :
		case PVC_STATUS_WAIT_TRY     :
		case PVC_STATUS_WAIT_OPEN    :  FColor = clNavy;
										return Scstrings_MAIN_SESSION_STAT_CONNECTED;///"已連線";
		case PVC_STATUS_ONLINE_BUSY  :
		case PVC_STATUS_ONLINE_READY :  FColor = clGreen;
										return Scstrings_MAIN_SESSION_STAT_CONNECTED;///"已連線";
		case PVC_STATUS_UNKNOWN      :  FColor = clBlack;
		default:                        return Scstrings_MAIN_PROCESS_STAT_UNKNOWN;///"狀態不明";
	}
}
//---------------------------------------------------------------------------
bool __fastcall TLineInfo::IsConnected( void )
{
	switch( FState%100 )
	{
		case PVC_STATUS_TRY_OPEN     :
		case PVC_STATUS_WAIT_TRY     :
		case PVC_STATUS_WAIT_OPEN    :
		case PVC_STATUS_ONLINE_BUSY  :
		case PVC_STATUS_ONLINE_READY :	return true;
		default:                        return false;
	}
}
//---------------------------------------------------------------------------
bool __fastcall TLineInfo::IsFTLine( void )
{
	if( FAPCode == 1 || FAPCode == 2 )
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TLineInfo::IsOrderLine( void )
{
	if( FMarket == mtStock || FMarket == mtOTC )
	{
		switch( FAPCode )
		{
			case 0:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10: return true;
			default: return false;
		}
	}
	else
	{
		if( FAPCode == 4 )
			return true;
		return false;
	}
}
//---------------------------------------------------------------------------
bool __fastcall TLineInfo::IsConfirmLine( void )
{
	if( FMarket == mtStock || FMarket == mtOTC )
	{
		if( FAPCode == 3)
			return true;
		return false;
	}
	else
	{
		switch( FAPCode )
		{
			case 3:
			case 8:
			case 9:  return true;
			default: return false;
		}
	}
}
//---------------------------------------------------------------------------
String __fastcall TLineInfo::GetApplication( void )
{
	if( FMarket == mtStock || FMarket == mtOTC ) ///< TSEC
	{
		if( FState > 100 ) ///< Special Session
		{
			switch( FState / 100 )
			{
				case 3: return Scstrings_MAIN_LEND;//"標借";
				case 4: return Scstrings_MAIN_TENDER;//"標購";
				case 5: if( FMarket == mtStock )
							return Scstrings_MAIN_AUCTION;//"拍賣";
						else
							return Scstrings_MAIN_NEGOTIATE_PX;//"議價";
				case 6: return Scstrings_MAIN_TENDER_EX;//"證金標購";
			}
		}
		switch( FAPCode ) ///< Normal trading session
		{
			case 0:  return Scstrings_MAIN_NORMAL;//"普通";
			case 1:  return Scstrings_MAIN_FTP_SEND;//"檔案(傳送)";
			case 2:  return Scstrings_MAIN_FTP_RECV;//"檔案(接收)";
			case 3:  return Scstrings_MAIN_EXECUTION;///"成交回報";
			case 4:  return Scstrings_MAIN_ODD_NORMAL;///"零股/普通";
			case 9:  return Scstrings_MAIN_POST_NORMAL;///"定價/普通";
			case 10: return L"盤中零股";
			default: return Scstrings_MAIN_UNKNOWN_AP_CODE + String( FAPCode );
		}
	}
	else ///< TAIFEX Futures,Option
	{
		if( this->FIsTFXOffhour == false )
		{
			switch( FAPCode )
			{
				case 3:  return Scstrings_MAIN_PVC_SETTING_TMP_EXECUTION;///3:成交回報";
				case 4:  return Scstrings_MAIN_PVC_SETTING_TMP_ORDER;///4:委託";
				case 8:  return Scstrings_MAIN_PVC_SETTING_X25_CM_ORDER;///8:結算委回";
				case 9:  return Scstrings_MAIN_PVC_SETTING_X25_CM_EXECUTION;///9:結算成回";
				default: return Scstrings_MAIN_UNKNOWN_AP_CODE + String( FAPCode );
			}
		}
		else
		{
			switch( FAPCode )
			{
				case 3:  return Scstrings_MAIN_PVC_SETTING_TMP_EXECUTION+"(PM)";///3:成交回報";
				case 4:  return Scstrings_MAIN_PVC_SETTING_TMP_ORDER+"(PM)";///4:委託";
				case 8:  return Scstrings_MAIN_PVC_SETTING_X25_CM_ORDER+"(PM)";///8:結算委回";
				case 9:  return Scstrings_MAIN_PVC_SETTING_X25_CM_EXECUTION+"(PM)";///9:結算成回";
				default: return Scstrings_MAIN_UNKNOWN_AP_CODE + String( FAPCode )+"(PM)";
			}
		}
	}
}
//---------------------------------------------------------------------------
bool __fastcall TLineInfo::IsFIX( void )
{
	if( (FMarket == mtStock || FMarket == mtOTC ) && ///< TWSE , OTC suport FIX now
		 FIsTCP == true ) ///< FIX must a TCP connection
	{
	   if( FPVC == 99 )
		   return true;
	}
	return false;
}
//---------------------------------------------------------------------------
String __fastcall TLineInfo::GetMessage( void )
{
	String Prefix;

	if( FState == PVC_STATUS_UNKNOWNPVC )
		return Scstrings_MAIN_SESSION_STAT_THREAD_STOPED;///"線路Thread已停止";
	if( FState == PVC_STATUS_UNKNOWN )
		return Scstrings_MAIN_SESSION_STAT_INITIALIZE;///"起始中...";
	switch( FState / 100 )
	{
		case 1: Prefix = Scstrings_MAIN_ODD; break;
		case 2: Prefix = Scstrings_MAIN_POST; break;
		case 3: Prefix = Scstrings_MAIN_LEND; break;
		case 4: Prefix = Scstrings_MAIN_TENDER; break;
		case 5: if( FMarket == mtStock )
					Prefix = Scstrings_MAIN_AUCTION;
				else
					Prefix = Scstrings_MAIN_NEGOTIATE_PX;
				break;
		case 6: Prefix = Scstrings_MAIN_TENDER_EX; break;
		case 7: Prefix = L"盤中零股";break;
		default:Prefix = L""; break;
	}
	switch( FState % 100 )
	{
		case PVC_STATUS_INIT         	 :return L"---";
		case PVC_STATUS_FINISHED     	 :return Scstrings_MAIN_SESSION_STATUS_FINISHED;//"成回已送完";
		case PVC_STATUS_SUBSYS_ERR   	 :return Scstrings_MAIN_SESSION_STATUS_SUBSYS_ERR;//"連線子系統錯誤";
		case PVC_STATUS_PW_ERR       	 :return Scstrings_MAIN_SESSION_STATUS_PW_ERR;//"密碼錯誤";
		case PVC_STATUS_FCM_ERR      	 :return Scstrings_MAIN_SESSION_STATUS_FCM_ERR;//"券商代號錯誤";
		case PVC_STATUS_AP_ERR       	 :return Scstrings_MAIN_SESSION_STATUS_AP_ERR;//"AP Code 錯誤";
		case PVC_STATUS_BROKEN       	 :return Scstrings_MAIN_SESSION_STATUS_BROKEN;//"重新連線中";
		case PVC_STATUS_OFFLINE_BUSY     :return Prefix + Scstrings_MAIN_SESSION_STATUS_OFFLINE_BUSY;//"等待連線中";
		case PVC_STATUS_SLEEPING         :return Prefix + Scstrings_MAIN_SESSION_STATUS_SLEEPING;//"盤別變換中";
		case PVC_STATUS_TRY_OPEN         :return Prefix + Scstrings_MAIN_SESSION_STATUS_TRY_OPEN;//"試單中";
		case PVC_STATUS_WAIT_OPEN        :return Prefix + Scstrings_MAIN_SESSION_STATUS_WAIT_OPEN;//"等待開盤中";
		case PVC_STATUS_WAIT_TRY         :return Prefix + Scstrings_MAIN_SESSION_STATUS_WAIT_TRY;//"等待試單中";
		case PVC_STATUS_OFFLINE_READY	 :return Prefix + Scstrings_MAIN_SESSION_STATUS_OFFLINE_READY;//"線路閒置中";
		case PVC_STATUS_ONLINE_BUSY  	 :return Prefix + Scstrings_MAIN_SESSION_STATUS_ONLINE_BUSY;//"傳送資料中";
		case PVC_STATUS_ONLINE_READY 	 :return Prefix + Scstrings_MAIN_SESSION_STATUS_ONLINE_READY;//"線路閒置";
		case PVC_STATUS_DISABLE      	 :return Prefix + Scstrings_MAIN_SESSION_STATUS_DISABLE;//"線路停用";
		case PVC_STATUS_DISABLEING   	 :return Prefix + Scstrings_MAIN_SESSION_STATUS_DISABLEING;//"線路停用中";
		case PVC_STATUS_ENABLE       	 :return Prefix + Scstrings_MAIN_SESSION_STATUS_ENABLE;//"線路啟用中";
		default:                          return Scstrings_MAIN_SESSION_STATUS_UNKNOWN + IntToStr( FState );
	}
}
//---------------------------------------------------------------------------
TColor  __fastcall TLineInfo::GetColor( bool IsSelected )
{
	if( IsSelected )
		return clWhite;
	return FColor;
}
//---------------------------------------------------------------------------
//
//  TFIXSession
//  Base class for a FIX session
//
//---------------------------------------------------------------------------
const AnsiString FDictPathPrefix = "../FIXSPEC/";
//---------------------------------------------------------------------------
__fastcall TFIXSession::TFIXSession( const char* Ver, const char* Sender, const char* Target )
:FVersion( Ver )
,FSenderID( Sender )
,FTargetID( Target )
,FDictonary( "[None]" )
,FInSeq( 0 )
,FOutSeq( 0 )
,FState( SESSION_STATUS_UNKNOWN )
,FStartTime( 0 )
,FEndTime( 0 )
,FEnabled( true )
,FPersistMessages( true  )
{
	if( FVersion == "FIX.4.0" )
		FDictonary = "FIX40.xml";
	else if( FVersion == "FIX.4.1" )
		FDictonary = "FIX41.xml";
	else if( FVersion == "FIX.4.2" )
		FDictonary = "FIX42.xml";
	else if( FVersion == "FIX.4.3" )
		FDictonary = "FIX43.xml";
	else
		FDictonary = "FIX44.xml";
	UpdateSessionKey();
}
//---------------------------------------------------------------------------
__fastcall TFIXSession::TFIXSession( UFC::Section* iniSection )
:FVersion( "FIX.4.4" )
,FSenderID( "DefaultSender" )
,FTargetID( "DefaultTarget" )
,FDictonary( "[None]" )
,FInSeq( 0 )
,FOutSeq( 0 )
,FState( SESSION_STATUS_UNKNOWN )
,FStartTime( 0 )
,FEndTime( 0 )
,FStartDay( "everyday" )
,FEndDay( "everyday" )
,FEnabled( true )
,FPersistMessages( true )
,FMillisecondsInTimeStamp( true )
,FResetOnLogon( true )
,FVerifyMsgSeqNum( true )
{
	UFC::AnsiString Value;
	UFC::AnsiString StartStr, EndStr;

	if( iniSection->GetValue("BeginString",Value))
		FVersion = Value.c_str();
	if( iniSection->GetValue("SenderCompID",Value))
		FSenderID = Value.c_str();
	if( iniSection->GetValue("TargetCompID",Value))
		FTargetID = Value.c_str();
	if( iniSection->GetValue("PersistMessages",Value ))
	{
		if( Value == "N" )
			FPersistMessages = false;
	}
	if( iniSection->GetValue("ResetOnLogon",Value ))
	{
		if( Value == "N" )
			FResetOnLogon = false;
	}
	if( iniSection->GetValue("VerifyMsgSeqNum",Value ))
	{
		if( Value == "N" )
			FVerifyMsgSeqNum = false;
	}
	if( iniSection->GetValue("MillisecondsInTimeStamp",Value ))
	{
		if( Value == "N" )
			FMillisecondsInTimeStamp = false;
	}
	if( iniSection->GetValue("StartTime",StartStr) && iniSection->GetValue("EndTime",EndStr) )
	{
		FStartTime = StartStr.SubString( 0, 2 ).ToInt() * 10000 + StartStr.SubString( 3, 2 ).ToInt() *100 + StartStr.SubString( 6, 2 ).ToInt();
		FEndTime   = EndStr.SubString( 0, 2 ).ToInt() * 10000 + EndStr.SubString( 3, 2 ).ToInt() *100 + EndStr.SubString( 6, 2 ).ToInt();
	}
	if( iniSection->GetValue("StartDay",Value ))
		SetStartDay( Value.c_str() );
	if( iniSection->GetValue("EndDay",Value ))
		SetEndDay( Value.c_str() );
	UpdateSessionKey();
}
//---------------------------------------------------------------------------
void __fastcall TFIXSession::UpdateSessionKey()
{
	FDictonaryFullPath =  FDictPathPrefix + FDictonary;
    FSessionKey = FVersion+":"+FSenderID+"->"+FTargetID;
}
//---------------------------------------------------------------------------
void  __fastcall TFIXSession::SetDictonaryFullPath( const AnsiString& Dict )
{
    FDictonary = AnsiString( Dict.c_str() + FDictPathPrefix.Length() );
    UpdateSessionKey();
}
//---------------------------------------------------------------------------
void  __fastcall TFIXSession::SetDictonary( const AnsiString& Ver )
{
    FDictonary = Ver;
    UpdateSessionKey();
}
//---------------------------------------------------------------------------
void  __fastcall TFIXSession::SetFIXVersion( const AnsiString& Ver )
{
    FVersion = Ver;
    UpdateSessionKey();
}
//---------------------------------------------------------------------------
void  __fastcall TFIXSession::SetStartDay( const AnsiString& Day )
{
	AnsiString PreFix( Day.c_str(), 2 );

	if( PreFix == "su" )
		FStartDay = "sunday";
	else if( PreFix == "mo" )
		FStartDay = "monday";
	else if( PreFix == "tu" )
		FStartDay = "tuesday";
	else if( PreFix == "we" )
		FStartDay = "wednesday";
	else if( PreFix == "th" )
		FStartDay = "thursday";
	else if( PreFix == "fr" )
		FStartDay = "friday";
	else if( PreFix == "sa" )
		FStartDay = "saturday";
	else
		FStartDay = "everyday";
}
//---------------------------------------------------------------------------
void  __fastcall TFIXSession::SetEndDay( const AnsiString& Day )
{
	AnsiString PreFix( Day.c_str(), 2 );

	if( PreFix == "su" )
		FEndDay = "sunday";
	else if( PreFix == "mo" )
		FEndDay = "monday";
	else if( PreFix == "tu" )
		FEndDay = "tuesday";
	else if( PreFix == "we" )
		FEndDay = "wednesday";
	else if( PreFix == "th" )
		FEndDay = "thursday";
	else if( PreFix == "fr" )
		FEndDay = "friday";
	else if( PreFix == "sa" )
		FEndDay = "saturday";
	else
		FEndDay = "everyday";
}
//---------------------------------------------------------------------------
void  __fastcall TFIXSession::SetFIXAppVersion( const AnsiString& Ver )
{
	if( Ver == "2" )
		FAppVersion = "FIX.4.0";
	else if( Ver == "3" )
		FAppVersion = "FIX.4.1";
	else if( Ver == "4" )
		FAppVersion = "FIX.4.2";
	else if( Ver == "5" )
		FAppVersion = "FIX.4.3";
	else if( Ver == "6" )
		FAppVersion = "FIX.4.4";
	else if( Ver == "7" )
		FAppVersion = "FIX.5.0";
	else if( Ver == "8" )
		FAppVersion = "FIX.5.0SP1";
	else if( Ver == "9" )
		FAppVersion = "FIX.5.0SP2";
	else
		FAppVersion = Ver;
	UpdateSessionKey();
}
//---------------------------------------------------------------------------
void  __fastcall TFIXSession::SetSenderCompID( const AnsiString& Sender )
{
	FSenderID = Sender;
	UpdateSessionKey();
}
//---------------------------------------------------------------------------
void  __fastcall TFIXSession::SetTargetCompID( const AnsiString& Target )
{
    FTargetID = Target;
    UpdateSessionKey();
}
//---------------------------------------------------------------------------
//
//   TAcceptor
//   Class to keep FIX acceptor information.
//
//---------------------------------------------------------------------------
int TAcceptor::FAcceptorPort = 5000;
//---------------------------------------------------------------------------
__fastcall TAcceptor::TAcceptor( const char* Ver, const char* Sender, const char* Target )
:TFIXSession( Ver, Sender, Target )
{
}
//---------------------------------------------------------------------------
__fastcall TAcceptor::TAcceptor( UFC::Section* iniSection )
:TFIXSession( iniSection )
{
	UFC::AnsiString Value;

	if( FVersion == "FIXT.1.1" ) ///< FIX 5.0
	{
		if( iniSection->GetValue( "DefaultApplVerID", Value ) )
			SetFIXAppVersion( Value.c_str() );
		if( iniSection->GetValue("AppDataDictionary",Value ) )
			SetDictonaryFullPath( Value.c_str() );
		else
			SetDictonary( "[None]" );
	}
	else
	{
		if( iniSection->GetValue("DataDictionary",Value ) )
			SetDictonaryFullPath( Value.c_str() );
		else
			SetDictonary( "[None]" );
	}

}
//---------------------------------------------------------------------------
UFC::Section* __fastcall TAcceptor::CreateConfigSession()
{
	AnsiString Time;
	int StartTime = GetStartTime();
	int EndTime   = GetEndTime();
	UFC::Section* NewSection = new UFC::Section("SESSION");

	NewSection->SetValue( "ConnectionType", "acceptor" );
	NewSection->SetValue( "BeginString",  FVersion.c_str());
	NewSection->SetValue( "SenderCompID", FSenderID.c_str());
	NewSection->SetValue( "TargetCompID", FTargetID.c_str());
	if( FVersion == "FIXT.1.1" ) ///< FIX 5.0
	{
		NewSection->SetValue( "DefaultApplVerID", FAppVersion.c_str());
		if( FDictonary.AnsiCompare( "[None]" ) != 0 )
		{
			NewSection->SetValue( "UseDataDictionary","Y");
			NewSection->SetValue( "TransportDataDictionary", "FIXT1.1.xml" );
			NewSection->SetValue( "AppDataDictionary", FDictonaryFullPath.c_str());
		}
		else
			NewSection->SetValue( "UseDataDictionary","N");
	}
	else
	{
		if( FDictonary.AnsiCompare( "[None]" ) != 0 )
		{
			NewSection->SetValue( "UseDataDictionary","Y");
			NewSection->SetValue( "DataDictionary", FDictonaryFullPath.c_str());
		}
		else
			NewSection->SetValue( "UseDataDictionary","N");
	}
	if( GetPersistMessages() == true )
		NewSection->SetValue( "PersistMessages", "Y");
	else
		NewSection->SetValue( "PersistMessages", "N");
	if( GetMillisecondsInTimeStamp() == true )
		NewSection->SetValue( "MillisecondsInTimeStamp", "Y");
	else
		NewSection->SetValue( "MillisecondsInTimeStamp", "N");

	if( GetResetOnLogon() == true )
		NewSection->SetValue( "ResetOnLogon", "Y");
	else
		NewSection->SetValue( "ResetOnLogon", "N");
	if( GetVerifyMsgSeqNum() == true )
		NewSection->SetValue( "VerifyMsgSeqNum", "Y");
	else
		NewSection->SetValue( "VerifyMsgSeqNum", "N");
	Time.printf( "%02d:%02d:%02d", StartTime/10000, (StartTime%10000)/100,StartTime%100 );
	NewSection->SetValue( "StartTime", Time.c_str());
	Time.printf( "%02d:%02d:%02d", EndTime/10000, (EndTime%10000)/100,EndTime%100 );
	NewSection->SetValue( "EndTime", Time.c_str());
	if( FStartDay != "everyday" )
		NewSection->SetValue( "StartDay", FStartDay.c_str() );
	if( FEndDay != "everyday" )
		NewSection->SetValue( "EndDay", FEndDay.c_str() );
	return NewSection;
}
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
__fastcall TInitiator::TInitiator( const char* Ver, const char* Sender, const char* Target )
:TFIXSession( Ver, Sender, Target )
,FHeartbeatInterval( 30 )
,FReconnectInterval( 30 )
,FLogonTimeout( 10 )
{
	FIPList = new TStringList();
	FPorts  = new TList();
}
//---------------------------------------------------------------------------
__fastcall TInitiator::TInitiator( UFC::Section* iniSection )
:TFIXSession( iniSection )
{
	UFC::AnsiString Value, Value1;

	FIPList = new TStringList();
	FPorts  = new TList();
	if( FVersion == "FIXT.1.1" ) ///< FIX 5.0
	{
		if( iniSection->GetValue( "DefaultApplVerID", Value ) )
			SetFIXAppVersion( Value.c_str() );
		if( iniSection->GetValue("AppDataDictionary",Value ) )
			SetDictonaryFullPath( Value.c_str() );
		else
			SetDictonary( "[None]" );
	}
	else
	{
		if( iniSection->GetValue("DataDictionary",Value ) )
			SetDictonaryFullPath( Value.c_str() );
		else
			SetDictonary( "[None]" );
	}
	if( iniSection->GetValue("ReconnectInterval",Value) == TRUE )
		SetReconnectInterval( Value.ToInt() );
	if( iniSection->GetValue("HeartBtInt",Value) == TRUE )
		SetHeartbeatInterval( Value.ToInt() );
	if( iniSection->GetValue("LogonTimeout",Value) == TRUE )
		SetLogonTimeout( Value.ToInt() );
	if( iniSection->GetValue("SocketConnectHost",Value) && iniSection->GetValue("SocketConnectPort",Value1))
	{
		AddHost( Value.c_str(),Value1.ToInt() );
		if( iniSection->GetValue("SocketConnectHost1",Value) && iniSection->GetValue("SocketConnectPort1",Value1))
		{
			AddHost( Value.c_str(),Value1.ToInt() );
			if( iniSection->GetValue("SocketConnectHost2",Value) && iniSection->GetValue("SocketConnectPort2",Value1))
				AddHost( Value.c_str(),Value1.ToInt() );
		}
	}
	else
		AddHost( "127.0.0.1", 5000 );
}
//---------------------------------------------------------------------------
__fastcall TInitiator::~TInitiator( )
{
	delete FIPList;
	delete FPorts;
}
//---------------------------------------------------------------------------
void __fastcall TInitiator::ClearHosts()
{
	FIPList->Clear();
	FPorts->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TInitiator::DeleteHosts( int index )
{
	FIPList->Delete( index );
	FPorts->Delete( index );
}
//---------------------------------------------------------------------------
void __fastcall TInitiator::AddHost( AnsiString NewIP, int Port )
{
	FIPList->Add( NewIP );
	FPorts->Add( (void*)Port );
}
//---------------------------------------------------------------------------
int __fastcall TInitiator::GetHostPort( int index )
{
	return (int)FPorts->Items[ index ];
}
//---------------------------------------------------------------------------
void __fastcall TInitiator::SetHostPort( int index, int Port )
{
	FPorts->Items[ index ] = (void*)Port;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TInitiator::GetHostIP( int index )
{
	return FIPList->Strings[ index ];
}
//---------------------------------------------------------------------------
void __fastcall TInitiator::SetHostIP( int index, AnsiString& NewIP )
{
	FIPList->Strings[ index ] = NewIP;
}
//---------------------------------------------------------------------------
UFC::Section* __fastcall TInitiator::CreateConfigSession()
{
	AnsiString Time;
	int StartTime = GetStartTime();
	int EndTime   = GetEndTime();
	UFC::Section* NewSection = new UFC::Section("SESSION");
	UFC::AnsiString HeartbeatInterval(FHeartbeatInterval);
	UFC::AnsiString ReconnectInterval(FReconnectInterval);
	UFC::AnsiString LogonTimeout( FLogonTimeout );

	NewSection->SetValue( "ConnectionType", "initiator" );
	NewSection->SetValue( "BeginString", FVersion.c_str());
	NewSection->SetValue( "SenderCompID", FSenderID.c_str());
	NewSection->SetValue( "TargetCompID", FTargetID.c_str());
	NewSection->SetValue( "HeartBtInt", HeartbeatInterval );
	NewSection->SetValue( "ReconnectInterval", ReconnectInterval );
	NewSection->SetValue( "LogonTimeout", LogonTimeout );
	NewSection->SetValue( "SocketConnectHost", GetHostIP(0).c_str() );
	NewSection->SetValue( "SocketConnectPort", UFC::AnsiString( GetHostPort(0 )) );
	if( FVersion == "FIXT.1.1" ) ///< FIX 5.0
	{
		NewSection->SetValue( "DefaultApplVerID", FAppVersion.c_str());
		if( FDictonary.AnsiCompare( "[None]" ) != 0 )
		{
			NewSection->SetValue( "UseDataDictionary","Y");
			NewSection->SetValue( "TransportDataDictionary", "FIXT1.1.xml" );
			NewSection->SetValue( "AppDataDictionary", FDictonaryFullPath.c_str());
		}
		else
			NewSection->SetValue( "UseDataDictionary","N");
	}
	else
	{
		if( FDictonary.AnsiCompare( "[None]" ) != 0 )
		{
			NewSection->SetValue( "UseDataDictionary","Y");
			NewSection->SetValue( "DataDictionary", FDictonaryFullPath.c_str());
		}
		else
			NewSection->SetValue( "UseDataDictionary","N");
	}
	if( GetPersistMessages() == true )
		NewSection->SetValue( "PersistMessages", "Y");
	else
		NewSection->SetValue( "PersistMessages", "N");
	if( GetMillisecondsInTimeStamp() == true )
		NewSection->SetValue( "MillisecondsInTimeStamp", "Y");
	else
		NewSection->SetValue( "MillisecondsInTimeStamp", "N");
	if( HostCount() >= 2 )
	{
		NewSection->SetValue( "SocketConnectHost1", GetHostIP(1).c_str() );
		NewSection->SetValue( "SocketConnectPort1", UFC::AnsiString(GetHostPort(1)) );
		if( HostCount() >= 3 )
		{
			NewSection->SetValue( "SocketConnectHost2", GetHostIP(2).c_str() );
			NewSection->SetValue( "SocketConnectPort2", UFC::AnsiString(GetHostPort(2)));
		}
	}
	if( GetVerifyMsgSeqNum() == true )
		NewSection->SetValue( "VerifyMsgSeqNum", "Y");
	else
		NewSection->SetValue( "VerifyMsgSeqNum", "N");
	Time.printf( "%02d:%02d:%02d", StartTime/10000, (StartTime%10000)/100,StartTime%100 );
	NewSection->SetValue( "StartTime", Time.c_str());
	Time.printf( "%02d:%02d:%02d", EndTime/10000, (EndTime%10000)/100,EndTime%100 );
	NewSection->SetValue( "EndTime", Time.c_str());
	if( FStartDay != "everyday" )
		NewSection->SetValue( "StartDay", FStartDay.c_str() );
	if( FEndDay != "everyday" )
		NewSection->SetValue( "EndDay", FEndDay.c_str() );
	return NewSection;
}
//---------------------------------------------------------------------------
bool IsValidIP( const AnsiString& IP )
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
