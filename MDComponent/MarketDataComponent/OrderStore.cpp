//------------------------------------------------------------------------------
#include <vcl.h>
#include <string.h>
#include <math.h>
#pragma hdrstop
#include "XMLDoc.hpp"
#include "IdHTTP.hpp"
#include "IdSSL.hpp"
#include "IdSSLOpenSSL.hpp"
#include "TExecutionReportMessage.h"
#include "OrderStore.h"
#include "TradingObjects.h"
#include "TradingObjectPool.h"
#include <System.JSON.hpp>
#include <WideStrUtils.hpp>
#pragma package(smart_init)
//------------------------------------------------------------------------------
// To Do:
/// TExecution* CurStopOrderPtr not record StrikePrice and EventType.
//------------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TOrderStore *)
{
	new TOrderStore(NULL);
}
//-------------------------------- TOrderStore ---------------------------------
bool TOrderStore::FIsTestMode = false;
bool TOrderStore::FIsTestMatchPrice = false;
bool TOrderStore::FIsAutoPositionEffect = false;
bool TOrderStore::FCanSeparateCloseTodayOrder = true;  //是否自動拆平今、平倉
bool TOrderStore::FCanSeparateOpenOrder = true;        //是否自動拆新倉、平倉(若為true，設定自動拆平今、平倉)
int TOrderStore::FTestOrderIdSeq = 0;
int TOrderStore::FTestExecIdSeq = 0;
//------------------------------------------------------------------------------
bool TOrderStore::IsTestMode() {return FIsTestMode;};
void TOrderStore::SetTestMode(bool IsTestMode) {FIsTestMode = IsTestMode;};
bool TOrderStore::IsTestMatchPrice() {return FIsTestMatchPrice;};
void TOrderStore::SetTestMatchPrice(bool IsTestMatchPrice) {FIsTestMatchPrice = IsTestMatchPrice;};
bool TOrderStore::IsAutoPositionEffect() {return FIsAutoPositionEffect;};
void TOrderStore::SetIsAutoPositionEffect(bool IsAutoPositionEffect) {FIsAutoPositionEffect = IsAutoPositionEffect;};
bool TOrderStore::CanSeparateCloseTodayOrder() {return FCanSeparateCloseTodayOrder;};
bool TOrderStore::CanSeparateOpenOrder() {return FCanSeparateOpenOrder;};
bool TOrderStore::IsContractDownloadCompleted() {return FCanSeparateOpenOrder;};
//------------------------------------------------------------------------------
void TOrderStore::SetCanSeparateCloseTodayOrder(bool CanSeparateCloseTodayOrder)
{   //若已設定自動拆新倉、平倉，則一定自動拆平今、平倉
	if ((FCanSeparateOpenOrder == true) && (CanSeparateCloseTodayOrder == false)) return;
	FCanSeparateCloseTodayOrder = CanSeparateCloseTodayOrder;
}
//------------------------------------------------------------------------------
void TOrderStore::SetCanSeparateOpenOrder(bool CanSeparateOpenOrder)
{   //若自動拆新倉、平倉，則一定自動拆平今、平倉
	FCanSeparateOpenOrder = CanSeparateOpenOrder;
	if (FCanSeparateOpenOrder == true) FCanSeparateCloseTodayOrder = true;
}  //TOrderStore::SetCanSeparateOpenOrder()
//------------------------------------------------------------------------------
int IntABS( int value )
{
	if( value < 0 )
		return -value;
	return value;
}
//------------------------------------------------------------------------------
__fastcall TOrderStore::TOrderStore(TComponent* Owner)
:TComponent(Owner)
,FBrokerID( L"F001000" )
,FClearMemberID( L"F001" )
,FExchange(L"TAIFEX")
,FIBID( L"000")
,FChannel(false)
,FAdapter(NULL)
,FOnConnect(NULL)
,FOnDisconnect(NULL)
,FOnLogonOK(NULL)
,FOnLogonFailed(NULL)
,FOnNewOrder(NULL)
,FOnCancelByOrderID(NULL)
,FOnCancelBySymbolPrice(NULL)
,FOnNewOrderReply(NULL)
,FOnFilledReply(NULL)
,FOnRejectedReply(NULL)
,FOnOrderStatusChange(NULL)
,FOnBeginRecover(NULL)
,FOnRecoverFinished(NULL)
,FOnStopOrderChange(NULL)
,FOnStopOrderError(NULL)
,FOnErrorMessage(NULL)
,FOnMarginsQueryReply(NULL)
,FOnServerAck(NULL)
,FOnFloatingProfit(NULL)
,FIsTry( true )
,FCanUse( 1000000 )
,FMaxLots( 200 )
{
	FOrderMsgFileStream = 0;
	FStopOrderFileStream = 0;
	FIsFirstTime = false;
	FRecoverStartTime = L"000000000";
	FVersion = L"0.0.0.0";
	FRecoverOverlapSecond = 60;

	FMarketDataTimerInterval = 250;
	FMarketDataTimer = new TTimer(this);
	FMarketDataTimer->Enabled = false;
	FMarketDataTimer->Interval = FMarketDataTimerInterval;
	FMarketDataTimer->OnTimer  = OnMarketDataTimer;

	FCancelCommandCount = 0;
	FCancelReportCount = 0;
}  //TOrderStore::TOrderStore()
//------------------------------------------------------------------------------
void __fastcall TOrderStore::Loaded(void)
{
	if( ComponentState.Contains( csDesigning ) )
		FAdapter = NULL;
}
//------------------------------------------------------------------------------
__fastcall TOrderStore::~TOrderStore()
{
	delete FMarketDataTimer;
	if( FAdapter != NULL )
		delete FAdapter;
}
//---------------------------------------------------------------------------
// Set properties
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetAppName(UnicodeString NewAppName)
{
	FAppName = NewAppName;
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetIP(UnicodeString NewIP)
{
	FIP = NewIP;
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetPort(int NewPort)
{
	FPort = NewPort;
	if( FAdapter != NULL )
		FAdapter->SetPort(NewPort);
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetID(UnicodeString NewID)
{
	FID = NewID;
	s888::gTradingObjsPool.LockAccount(s888::rwLockForWrite);
	s888::CTradingAccount *AccountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex( FID, s888::rwNotLock );
	if( AccountPtr == NULL )
	{
		AccountPtr = new s888::CTradingAccount( FID, 0 );
		s888::gTradingObjsPool.InsertAccountIntoLogonIdIndex( FID, AccountPtr, s888::constNotCheckExist, s888::rwNotLock );
	}
	s888::gTradingObjsPool.UnlockAccount(s888::rwLockForWrite);
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetPassword(UnicodeString NewPassword)
{
	FPassword = NewPassword;
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetAccount(UnicodeString NewAccount)
{
	s888::CTradingAccount *AccountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex( FID, s888::rwLockForRead);
	FAccount = NewAccount;
	if( AccountPtr !=  NULL )
	{
		AccountPtr->SetMarketAccount( nsOrderMessageDefine::mTWFutures, FAccount, s888::rwLockForWrite);
		AccountPtr->SetMarketAccount( nsOrderMessageDefine::mTWOptions, FAccount, s888::rwLockForWrite);
	}

}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetTWSEAccount(UnicodeString NewAccount)
{
	s888::CTradingAccount *AccountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex( FID, s888::rwLockForRead);
	FTWSEAccount = NewAccount;
	if( AccountPtr !=  NULL )
	{
		AccountPtr->SetMarketAccount( nsOrderMessageDefine::mTSE, FTWSEAccount, s888::rwLockForWrite);
		AccountPtr->SetMarketAccount( nsOrderMessageDefine::mOTC, FTWSEAccount, s888::rwLockForWrite);
	}
}
//---------------------------------------------------------------------------
String  __fastcall TOrderStore::GetUserName( void )
{
	if( FAdapter != NULL)
		return FAdapter->GetUserName( );
	return L"";
}
//---------------------------------------------------------------------------
void  __fastcall TOrderStore::SetUserName( String NewName )
{
	if( FAdapter != NULL)
		FAdapter->SetUserName( NewName );
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetBrokerID(UnicodeString NewBrokerID)
{
	FBrokerID = NewBrokerID;
	if( FAdapter != NULL )
		FAdapter->SetBrokerID(NewBrokerID);
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetTWSEBrokerID(UnicodeString NewBrokerID)
{
	FTWSEBrokerID = NewBrokerID;
	if( FAdapter != NULL )
		FAdapter->SetTWSEBrokerID(NewBrokerID);
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetClearMemberID(UnicodeString NewClearMemberID)
{
	FClearMemberID = NewClearMemberID;
	if( FAdapter != NULL )
		FAdapter->SetClearMemberID(NewClearMemberID);
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetOrderLogPath(UnicodeString NewOrderLogPath)
{
	FOrderLogPath = NewOrderLogPath;
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetOrderLogFileNamePrefix(UnicodeString NewOrderLogFileNamePrefix)
{
	FOrderLogFileNamePrefix = NewOrderLogFileNamePrefix;
}
//---------------------------------------------------------------------------
void  __fastcall TOrderStore::SetActiveExecutive( String AE )
{
	FActiveExecutive = AE;
	if( FAdapter != NULL )
		FAdapter->ActiveExecutive = AE;
}
//---------------------------------------------------------------------------
void  __fastcall TOrderStore::SetIBID( String IB )
{
	FIBID = IB;
	if( FAdapter != NULL )
		FAdapter->IB = IB;
}
//---------------------------------------------------------------------------
bool  __fastcall TOrderStore::GetIsEDS( void )
{
	if( FAdapter != NULL )
		return FAdapter->IsEDS;
	return FIsEDS;
}
//---------------------------------------------------------------------------
void  __fastcall TOrderStore::SetIsEDS( bool YorN )
{
	FIsEDS = YorN;
	if( FAdapter != NULL )
		FAdapter->IsEDS = YorN;
}
//------------------------------------------------------------------------------
String __fastcall TOrderStore::GetTradingDate( void )
{
	String TradingDate;
	UFC::AnsiString Today;

	UFC::GetTradeYYYYMMDD( Today );
	TradingDate = Today.c_str();
	if( FAdapter != NULL )
		TradingDate = FAdapter->GetTradingDate();
	return TradingDate;
}
//------------------------------------------------------------------------------
void __fastcall TOrderStore::SetChannel( bool Channel )
{
	FChannel = Channel;
//	if (FAdapter != NULL)
//		FAdapter->SetChannel( Channel );
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::SetIsTry( bool IsTry )
{
	FIsTry = IsTry;
	if (FAdapter != NULL)
		FAdapter->SetIsTry( IsTry );
}
//---------------------------------------------------------------------------
// End Set properties
//---------------------------------------------------------------------------
void __fastcall TOrderStore::InitialOrderAdapter( void )
{
	if( FAdapter == NULL )
	{
		String TempOrderLogPath;

		if (FOrderLogPath.Length() <= 0)
			TempOrderLogPath = GetEnvironmentVariable(L"TEMP");
		else
			TempOrderLogPath = FOrderLogPath;
		if (TempOrderLogPath[TempOrderLogPath.Length() - 1] != '\\')
			TempOrderLogPath = TempOrderLogPath + L"\\";

		if( DirectoryExists( TempOrderLogPath ) == false )
			ForceDirectories( TempOrderLogPath );

		UFC::AnsiString tradeDate;
		UFC::GetTradeYYYYMMDD( tradeDate );
		String logTime = UTF8ToUnicodeString(tradeDate.c_str());
		FConnectionLogFileName = TempOrderLogPath + L"SpeedyUnifyTrading" + logTime + L".log";
		FAdapter = new TOrderAdapter( this,
									  FAppName,
									  FBrokerID,
									  FClearMemberID,
									  FConnectionLogFileName,
									  FUseAPI );
		FAdapter->IsEDS           = FIsEDS;
		FAdapter->ActiveExecutive = FActiveExecutive;
		FAdapter->IB              = FIBID;
	}
	else
	{
		FAdapter->SetBrokerID( FBrokerID );
		FAdapter->SetClearMemberID( FClearMemberID );
	}
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((OSVERSIONINFO *)&osvi);

	UFC::BufferedLog::DebugPrintf("================================================================================");
	if (FIsTestMode)
		UFC::BufferedLog::DebugPrintf(" Order Store Initial Test Mode");
	else
		UFC::BufferedLog::DebugPrintf(" Order Store Initial ");
	UFC::BufferedLog::DebugPrintf(" Operating System Version %d.%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
}  //TOrderStore::InitialOrderAdapter()

//---------------------------------------------------------------------------
String __fastcall TOrderStore::ReadOrderLogStr(TFileStream *OrderLogStmPtr)
{
	char readBuffer[2048];
	char inputCharacter;
	int readCount = 0;
	int bufferPosition = 0;
	do {
		readBuffer[bufferPosition] = 0;
		readCount = OrderLogStmPtr->Read(&inputCharacter, 1);
		if ((readCount > 0) && (inputCharacter != 10))
		{
			readBuffer[bufferPosition] = inputCharacter;
			bufferPosition++;
		}
	} while ((readCount > 0) && (inputCharacter != 10));
	String orderLogStr = L"";
	if (bufferPosition > 0) orderLogStr = UTF8ToString(readBuffer);
	return orderLogStr;
}  //TOrderStore::ReadOrderLogStr()

//---------------------------------------------------------------------------
void __fastcall TOrderStore::RecoverFromNet()
{
	if ((FAdapter != 0) && FAdapter->IsConnected() && FAdapter->IsLogon())
		FAdapter->DoNetRecover();
}  //TOrderStore::RecoverFromNet()
//---------------------------------------------------------------------------
void __fastcall TOrderStore::GetTempLogPath( String& TempOrderLogPath )
{
	if( FOrderLogPath.Length() <= 0)
		TempOrderLogPath = GetEnvironmentVariable( L"TEMP" );
	else
		TempOrderLogPath = FOrderLogPath;
	if( TempOrderLogPath[ TempOrderLogPath.Length() - 1] != '\\')
		TempOrderLogPath = TempOrderLogPath + L"\\";
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::UpdateOrderLogFileNamePrefix( void )
{
	if( FOrderLogFileNamePrefix.Length() <= 0 )
	{
		if( FIsTestMode)
			FOrderLogFileNamePrefix = L"testOrderStore";
		else
			FOrderLogFileNamePrefix = L"OrderStore";
	}
}
//---------------------------------------------------------------------------
bool __fastcall TOrderStore::RecoverFileExist( void )
{
	String CheckFile,TempOrderLogPath;

	GetTempLogPath( TempOrderLogPath );
	UpdateOrderLogFileNamePrefix();
	CheckFile = TempOrderLogPath + FOrderLogFileNamePrefix + L"StartTime.log";
	return FileExists( CheckFile );
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::RemoveRecoverFile( void )
{
	String DeleteFileName,Prefix,TempOrderLogPath;

	GetTempLogPath( TempOrderLogPath );
	UpdateOrderLogFileNamePrefix();
	DeleteFileName = TempOrderLogPath + FOrderLogFileNamePrefix + L"Message.log";
	DeleteFile( DeleteFileName );
	DeleteFileName = TempOrderLogPath + FOrderLogFileNamePrefix + L"StartTime.log";
	DeleteFile( DeleteFileName );
	DeleteFileName = TempOrderLogPath + FOrderLogFileNamePrefix + L"StopOrder.log";
	DeleteFile( DeleteFileName );
}
//---------------------------------------------------------------------------
bool __fastcall TOrderStore::NeedClearOrderFiles( const AnsiString& StartDateString )
{
	bool FirstTime = true;
	char DateBuffer[10];
	char TimeBuffer[10];
	char VersionBuffer[40];
	int  VersionBufferLength = 0;

	memset( DateBuffer, 0, 10);
	memset( TimeBuffer, 0, 10);
	memset( VersionBuffer, 0, 40);
	//--------------------------------------------------------------------------
	///<  Start Time file data format
	///<  Date 8    Time 9   Int 4
	///< YYYYMMDD|HHMMSSmmm|Length|Version String
	if( FileExists( FStartTimeFileName ) )
	{
		TFileStream* TimeFileStream = new TFileStream(FStartTimeFileName, fmOpenReadWrite);
		if( TimeFileStream->Size >= 17 )
		{
			TimeFileStream->Read( DateBuffer, 8 ); ///< YYYYMMDD
			String FileDateString( DateBuffer );
			if( FileDateString == StartDateString ) ///< Same date ?
			{
				TimeFileStream->Read( TimeBuffer, 9 ); ///< HHMMSSmmm
				String FileTimeString( TimeBuffer );
				if( TimeFileStream->Size >= 19 )
				{
					TimeFileStream->Read( (void*)(&VersionBufferLength), sizeof(VersionBufferLength));
					if (VersionBufferLength > 0)
					{
						TimeFileStream->Read((void*)VersionBuffer, VersionBufferLength);
						String FileVersion( VersionBuffer );
						if( FileVersion == FVersion ) ///< Same version ?
							FirstTime = false;
					}
				}
			}
		}
		delete TimeFileStream;
	}
	if( FirstTime == true ) ///< update to the new Date/Time/Version
	{
		AnsiString   StartTimeString;
		AnsiString   WriteVersion = FVersion;
		SYSTEMTIME   SysTime;
		TFileStream* TimeFileStream = new TFileStream( FStartTimeFileName, fmCreate);

		GetLocalTime(&SysTime);
		StartTimeString.printf( "%02d%02d%02d%03d", SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds );
		VersionBufferLength = WriteVersion.Length();
		TimeFileStream->Write( StartDateString.c_str(), 8 );
		TimeFileStream->Write( StartTimeString.c_str(), 9 );
		TimeFileStream->Write( (void*) (&VersionBufferLength), sizeof(VersionBufferLength));
		if( VersionBufferLength > 0 )
			TimeFileStream->Write((void*)(WriteVersion.c_str()), VersionBufferLength);
		delete TimeFileStream;
	}
	return FirstTime;
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::RecoverOrderLogFileData()
{
	UFC::AnsiString TradeDate;
	AnsiString      StartDateString;
	String          TempOrderLogPath;

	CloseOrderLogFile();
	if( FAdapter != NULL && FAdapter->IsLogon() )
		TradeDate = FAdapter->GetTradingDate();
	else
		UFC::GetTradeYYYYMMDD( TradeDate );
	StartDateString = TradeDate.c_str();
	GetTempLogPath( TempOrderLogPath );
	UpdateOrderLogFileNamePrefix();
	FOrderMsgLogFileName = TempOrderLogPath + FOrderLogFileNamePrefix + L"Message.log";
	FStartTimeFileName   = TempOrderLogPath + FOrderLogFileNamePrefix + L"StartTime.log";
	FStopOrderFileName   = TempOrderLogPath + FOrderLogFileNamePrefix + L"StopOrder.log";
	UFC::BufferedLog::Printf(" Recover Order Log Data Begin: OrderMessaggLog[%s] StartTimeFile[%s]", s888::ConvertUnicodeToUFCAnsiStr(FOrderMsgLogFileName).c_str(), s888::ConvertUnicodeToUFCAnsiStr(FStartTimeFileName).c_str());
	UFC::BufferedLog::FlushToFile();
	FIsFirstTime = NeedClearOrderFiles( StartDateString );
	FRecoverStartTime = L"000000000";
	if( FIsFirstTime == true ) ///< First time starup, clear OrderMessage and StopOrder file
	{
		FOrderMsgFileStream = new TFileStream(FOrderMsgLogFileName, fmCreate);
		FStopOrderFileStream = new TFileStream(FStopOrderFileName, fmCreate);
	}
	else
	{   //Read Message From File
		if( FileExists( FOrderMsgLogFileName ) )
		{
			FOrderMsgFileStream = new TFileStream(FOrderMsgLogFileName, fmOpenRead);
			TExecution* OrderMessagePtr;
			while (FOrderMsgFileStream->Position < FOrderMsgFileStream->Size)
			{
				String orderLogStr = ReadOrderLogStr(FOrderMsgFileStream);
				if (orderLogStr <= 0) continue;
				OrderMessagePtr = new TExecution(orderLogStr);

				if (FRecoverStartTime < OrderMessagePtr->GetTransactTime())
					FRecoverStartTime = OrderMessagePtr->GetTransactTime();

				if( !InsertOrderMessage(OrderMessagePtr) )
				{
					UFC::BufferedLog::Printf(" TOrderStore::RecoverOrderLogFileData() Insert Order[%s] Failed.",
											 s888::ConvertUnicodeToUFCAnsiStr(orderLogStr).c_str());
					UFC::BufferedLog::FlushToFile();
					delete OrderMessagePtr;
				}
			}
			if( FIsTestMode == true )
			{
				CStringTExecutionMap::iterator it;
				for (it = FActiveOrderRootTimeOrderIDIndex.begin(); it != FActiveOrderRootTimeOrderIDIndex.end(); it++)
				{
					TExecution *curOrderRootPtr = it->second;
					nsOrderMessageDefine::OrderStatusEnum curOrderStatus = curOrderRootPtr->GetOrderStatus();
					if ((curOrderStatus == nsOrderMessageDefine::osNew) ||
						(curOrderStatus == nsOrderMessageDefine::osPartiallyFilled))
					{
						TExecutionReportMessage *newRptPtr = new TExecutionReportMessage();
						newRptPtr->SetOrderID(s888::ConvertUnicodeToUFCAnsiStr(curOrderRootPtr->GetOrderID()).c_str());
						newRptPtr->SetMarket(curOrderRootPtr->GetMarket());
						newRptPtr->SetExchangeCode(s888::ConvertUnicodeToUFCAnsiStr(curOrderRootPtr->GetExchangeCode()).c_str());
						newRptPtr->SetSymbol(s888::ConvertUnicodeToUFCAnsiStr(curOrderRootPtr->GetSymbol()->GetSymbol(0)).c_str());
						newRptPtr->SetAccount(s888::ConvertUnicodeToUFCAnsiStr(curOrderRootPtr->GetAccount()).c_str());
						newRptPtr->SetSide(curOrderRootPtr->GetSide());
						newRptPtr->SetPrice(curOrderRootPtr->GetPrice());
						newRptPtr->SetOrderQty(curOrderRootPtr->GetOrderQty());
						newRptPtr->SetOrderType(curOrderRootPtr->GetOrderType());
						newRptPtr->SetTimeInForce(curOrderRootPtr->GetTimeInForce());
						newRptPtr->SetPositionEffect(curOrderRootPtr->GetPositionEffect());
						newRptPtr->SetNID(curOrderRootPtr->GetNID());
						newRptPtr->SetStopPx(curOrderRootPtr->GetStopPrice());
						newRptPtr->SetCumQty(curOrderRootPtr->GetCumQty());
						newRptPtr->SetLeavesQty(curOrderRootPtr->GetLeavesQty());
						newRptPtr->SetLastQty(curOrderRootPtr->GetLastQty());
						newRptPtr->SetLastPx(curOrderRootPtr->GetLastPx());
						newRptPtr->SetLegPrice1(curOrderRootPtr->GetLegPx1());
						newRptPtr->SetLegQty1(curOrderRootPtr->GetLegQty1());
						newRptPtr->SetOrderStatus(curOrderRootPtr->GetOrderStatus());
						newRptPtr->SetExecType(curOrderRootPtr->GetExecType());
						newRptPtr->SetTransactTime(s888::ConvertUnicodeToUFCAnsiStr(curOrderRootPtr->GetTransactTime()).c_str());
						AppendTestExecRptToListDirectly(newRptPtr);
					}
				}
			}
			delete FOrderMsgFileStream;
			FOrderMsgFileStream = new TFileStream(FOrderMsgLogFileName, fmOpenReadWrite);  //reOpen File for Write
			FOrderMsgFileStream->Seek(0, soFromEnd);  //Move to End of File
		}
		else
			FOrderMsgFileStream = new TFileStream(FOrderMsgLogFileName, fmCreate);
		//Read Stop Order From File
		if (FileExists(FStopOrderFileName))
		{
			FStopOrderFileStream = new TFileStream(FStopOrderFileName, fmOpenReadWrite);  //reOpen File for Write
			FStopOrderFileStream->Seek(0, soFromEnd);  ////Move to End of File
		}
		else
			FStopOrderFileStream = new TFileStream(FStopOrderFileName, fmCreate);
	}
	UFC::BufferedLog::Printf(" Recover Order Log Data End: OrderMessaggLog[%s] StartTimeFile[%s]", s888::ConvertUnicodeToUFCAnsiStr(FOrderMsgLogFileName).c_str(), s888::ConvertUnicodeToUFCAnsiStr(FStartTimeFileName).c_str());
	UFC::BufferedLog::FlushToFile();
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::CloseOrderLogFile()
{
	if (FOrderMsgFileStream != 0)
	{
		delete FOrderMsgFileStream;
		FOrderMsgFileStream = 0;
	}  //if (FOrderMsgFileStream != 0)

	if (FStopOrderFileStream != 0)
	{
		delete FStopOrderFileStream;
		FStopOrderFileStream = 0;
	}  //if (FStopOrderFileStream != 0)
}  //TOrderStore::CloseOrderLogFile()

//---------------------------------------------------------------------------
void __fastcall TOrderStore::Connect()
{
	if( FAdapter != NULL )
		FAdapter->Connect( FIP, FPort );
	UFC::AnsiString tempIP( FIP.c_str());
	UFC::BufferedLog::DebugPrintf(" Order Store [%s] Connect to [%s:%d]", UFC::Hostname, tempIP.c_str(), FPort);
}  //TOrderStore::Connect()
//---------------------------------------------------------------------------
void TOrderStore::ClearBusinessData()
{
	ClearOrderRootData();
	ClearStopOrderData();
	ClearOrderMessageData();
	ExecuteRefreshData();
	FOrderQtyListenerMap.clear();
	FPositionChangeListenerMap.clear();
}  //TOrderStore::ClearBusinessData()
//---------------------------------------------------------------------------
void __fastcall TOrderStore::Disconnect(bool DoClearBussiness)
{
	if( FStopOrderFileStream != 0 )
	{
		delete FStopOrderFileStream;
		FStopOrderFileStream = 0;
	}
	if (FOrderMsgFileStream != 0)
	{
		delete FOrderMsgFileStream;
		FOrderMsgFileStream = 0;
	}
	if (DoClearBussiness)
		ClearBusinessData();
	if ((FAdapter != NULL ) && FAdapter->IsConnected())
		FAdapter->Disconnect();
}  //TOrderStore::Disconnect()
//---------------------------------------------------------------------------
bool __fastcall TOrderStore::IsConnected()
{
	if (FAdapter != NULL )
		return FAdapter->IsConnected();
	else
		return false;
}  //TOrderStore::IsConnected()
//---------------------------------------------------------------------------
void __fastcall TOrderStore::LogonProxy( void )
{
	if ((FAdapter != NULL ) && !FAdapter->IsLogon())
	{
		String Accounts;
		if( FAccount.Length() > 1 )
		{
			if( FTWSEAccount.Length() > 1 )
				Accounts = FAccount + L"," + FTWSEAccount;
			else
				Accounts = FAccount;
		}
		else
			Accounts = FTWSEAccount;
		FAdapter->LogonProxy( FID, Accounts );
		UFC::AnsiString tempID( FID.c_str());
		UFC::AnsiString tempAccount( Accounts.c_str());
		UFC::AnsiString tempBrokerID( FBrokerID.c_str());
		UFC::BufferedLog::DebugPrintf(" Order Store Logon ID[%s]  Account[%s]  BrokerNo[%s]", tempID.c_str(), tempAccount.c_str(), tempBrokerID.c_str());
	}
}  //TOrderStore::Logon()

//---------------------------------------------------------------------------
void __fastcall TOrderStore::LogonPropTrade( void )
{
	if ((FAdapter != NULL ) && !FAdapter->IsLogon())
	{
		FAdapter->LogonPropTrade( FID, FPassword, FAccount );
		UFC::AnsiString tempID( FID.c_str());
		UFC::AnsiString tempAccount( FAccount.c_str());
		UFC::AnsiString tempBrokerID( FBrokerID.c_str());
		UFC::BufferedLog::DebugPrintf(" Order Store Logon ID[%s]  Account[%s]  BrokerNo[%s]", tempID.c_str(), tempAccount.c_str(), tempBrokerID.c_str());
	}
}  //TOrderStore::Logon()
//---------------------------------------------------------------------------
void __fastcall TOrderStore::Logoff(bool DoClearBussiness)
{
	if (FStopOrderFileStream != 0)
	{
		delete FStopOrderFileStream;
		FStopOrderFileStream = 0;
	}

	if (FOrderMsgFileStream != 0)
	{
		delete FOrderMsgFileStream;
		FOrderMsgFileStream = 0;
	}

	if (DoClearBussiness)
		ClearBusinessData();

	if ((FAdapter != NULL ) && FAdapter->IsLogon())
		FAdapter->Logoff();
}  //TOrderStore::Logon()

//---------------------------------------------------------------------------
bool __fastcall TOrderStore::IsLogon()
{
	if (FAdapter != NULL )
		return FAdapter->IsLogon();
	else
		return false;
}  //TOrderStore::IsLogon()

//---------------------------------------------------------------------------
bool __fastcall TOrderStore::IsReady()
{
	if (FAdapter != NULL )
		return (FAdapter->IsLogon() && FAdapter->IsConnected());
	else
		return false;
}  //TOrderStore::IsReady()

//---------------------------------------------------------------------------
bool __fastcall TOrderStore::IsRecovering()
{
	if (FAdapter != NULL )
		return FAdapter->IsRecovering();
	else
		return false;
}  //TOrderStore::IsRecovering()
//---------------------------------------------------------------------------
UnicodeString TOrderStore::GetConnectionLogFileName()
{
	return FConnectionLogFileName;
}
//---------------------------------------------------------------------------
void TOrderStore::BuildExchangeSymbolIndexKey(const UnicodeString& ExchangeCode,
											  const UnicodeString& SymbolCode,
											  UnicodeString& IndexKey)
{
	UnicodeString DelimiterChar = L",";
	IndexKey = ExchangeCode + DelimiterChar + SymbolCode;
}  //TOrderStore::BuildExchangeSymbolIndexKey()

//---------------------------------------------------------------------------
void TOrderStore::ParseExchangeSymbolIndexKey(const UnicodeString& IndexKey,
											  UnicodeString& ExchangeCode,
											  UnicodeString& SymbolCode)
{
	UnicodeString DelimiterChar = L",";
	int DelimiterPosition = IndexKey.Pos(DelimiterChar);
	int IndexKeyLength = IndexKey.Length();
	if (DelimiterPosition == 0)
	{
		ExchangeCode = L"";
		SymbolCode = IndexKey;
	}
	else if (DelimiterPosition == 1)
	{
		ExchangeCode = L"";
		SymbolCode = IndexKey.SubString(DelimiterPosition + 1, IndexKeyLength - DelimiterPosition);
	}
	else if (DelimiterPosition == IndexKeyLength)
	{
		ExchangeCode = IndexKey.SubString(1, IndexKeyLength - DelimiterPosition);
		SymbolCode = L"";
	}
	else
	{
		ExchangeCode = IndexKey.SubString(1, DelimiterPosition - 1);
		SymbolCode = IndexKey.SubString(DelimiterPosition + 1, IndexKeyLength - DelimiterPosition);
	}
}  //TOrderStore::ParseExchangeSymbolIndexKey()

//---------------------------------------------------------------------------
BasicInformation *TOrderStore::GetCommodityBasicInformation(const String& ExchangeCode, const String& Symbol)
{
	BasicInformation *InformationPtr = 0;
	if (FMarketDataStore == 0) return InformationPtr;
	InformationPtr = FMarketDataStore->GetBasicInformation(ExchangeCode, Symbol, false);  //Do not throw Exception
	return InformationPtr;
}  //TOrderStore::GetCommodityBasicInformation()

//---------------------------------------------------------------------------
String __fastcall TOrderStore::GetTradeSymbol(const String& ExchangeCode, const String& OrderSymbol, bool CheckSymbolIndexFirst)
{
	String tradeSymbol( OrderSymbol );
	return tradeSymbol;
}
//---------------------------------------------------------------------------
String __fastcall TOrderStore::GetOrderSymbol(const String& ExchangeCode, const String& OrigSymbol, bool CheckTradeSymbolIndexFirst)
{
	String orderSymbol = L"";
	s888::CTradingContract *contractPtr = 0;
	if (CheckTradeSymbolIndexFirst)
	{
		contractPtr = s888::gTradingObjsPool.FindContractFromTradeSymbolIndex(ExchangeCode, OrigSymbol, s888::rwLockForRead);
		if (contractPtr == 0) contractPtr = s888::gTradingObjsPool.FindContractFromSymbolIndex(ExchangeCode, OrigSymbol, s888::rwLockForRead);
	}
	else
	{
		contractPtr = s888::gTradingObjsPool.FindContractFromSymbolIndex(ExchangeCode, OrigSymbol, s888::rwLockForRead);
		if (contractPtr == 0) contractPtr = s888::gTradingObjsPool.FindContractFromTradeSymbolIndex(ExchangeCode, OrigSymbol, s888::rwLockForRead);
	}

	if (contractPtr != 0)
		orderSymbol = contractPtr->GetSymbol(s888::rwLockForRead);
	else
	{
		BasicInformation *infoPtr = GetCommodityBasicInformation(ExchangeCode, OrigSymbol);
		if (infoPtr != 0) orderSymbol = UTF8ToUnicodeString(infoPtr->GetSymbol().c_str());
	}
	if (orderSymbol.Length() <= 0) orderSymbol = OrigSymbol;
	return orderSymbol;
}  //TOrderStore::GetOrderSymbol()
//---------------------------------------------------------------------------
String __fastcall TOrderStore::GetMarketAccountForOrder(nsOrderMessageDefine::MarketEnum Market)
{
/*	String logonID = FID;
	String marketAccount( L"" );
	if( FID.Length() > 0 )
	{
		s888::CTradingAccount *tradingAccountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex( FID, s888::rwLockForRead);
		if( tradingAccountPtr != 0)
			marketAccount = tradingAccountPtr->FindMarketAccount( Market, s888::rwLockForRead);
	}
	if( marketAccount.Length() <= 0 )
		marketAccount = FAccount;
	return marketAccount;*/
	switch(  Market )
	{
		case nsOrderMessageDefine::mTSE:
		case nsOrderMessageDefine::mOTC: return FTWSEAccount;
		case nsOrderMessageDefine::mTWFutures:
		case nsOrderMessageDefine::mTWOptions: return FAccount;
	}
	return FAccount;
}  //TOrderStore::GetMarketAccountForOrder()
//---------------------------------------------------------------------------
bool TOrderStore::NewOrder( nsOrderMessageDefine::MarketEnum Market,
							const String& Exchange,
							const String& Symbol,
							nsOrderMessageDefine::SideEnum Side,
							double Px, int Qty,
							nsOrderMessageDefine::OrderTypeEnum      OrderType,
							nsOrderMessageDefine::TimeInForceEnum    TimeInForce,
							nsOrderMessageDefine::PositionEffectEnum PositionEffect,
							nsOrderMessageDefine::EventTypeEnum      EventType,
							double StrikePx,
							int TickCount,
							double StopPx,
							char TWSEOrdType )
{
	UFCType::Int64 Begin = UFC::GetTickCountUS();
	if (FAdapter == NULL )        throw OSConnectionNotExistException();
	if (!FAdapter->IsConnected()) throw OSConnectionNotConnectedException();
	if (!FAdapter->IsLogon())     throw OSConnectionNotLogonException();

	try
	{
		bool   IsAccepted = true;
		String marketAccount;

		if( FOnNewOrder != NULL )
			FOnNewOrder(this, Exchange, Symbol, Side, Px, Qty, OrderType, TimeInForce, PositionEffect, IsAccepted);
		if( IsAccepted )
		{
			switch(  Market )
			{
				case nsOrderMessageDefine::mTSE:
				case nsOrderMessageDefine::mOTC: marketAccount = FTWSEAccount;
												 break;
				default:                         marketAccount = FAccount;
												 break;
			}
			FAdapter->NewSingleOrder(   Market,
										Exchange,
										Symbol,
										Side,
										Px,
										Qty,
										OrderType,
										TimeInForce,
										PositionEffect,
										EventType,
										StrikePx,
										marketAccount,
										FActiveExecutive,
										FIBID,
										TickCount,
										StopPx,
										"     ", /// No StrategyName
										TWSEOrdType );

		}
		UFC::BufferedLog::Printf( " [TOrderStore::NewOrder] use[%d]us", UFC::GetTickCountUS() - Begin );
		return IsAccepted;
	}
	catch( UFC::Exception& x)
	{
		throw x;
	}
	catch (...)
	{
		 throw OSNewOrderException();
	}
}  //TOrderStore::NewOrder
//---------------------------------------------------------------------------
bool TOrderStore::NewOrder( nsOrderMessageDefine::MarketEnum Market,
							const String& Exchange,
							const String& Symbol,
							nsOrderMessageDefine::SideEnum Side,
							double Px,
							int Qty,
							nsOrderMessageDefine::OrderTypeEnum      OrderType,
							nsOrderMessageDefine::TimeInForceEnum    TimeInForce,
							nsOrderMessageDefine::PositionEffectEnum PositionEffect,
							nsOrderMessageDefine::EventTypeEnum      EventType,
							double StrikePx,
							int TickCount,
							double StopPx,
							const UFC::AnsiString& StrategyName,
							char TWSEOrdType,
							UFCType::Int64& NID )
{
	UFCType::Int64 Begin = UFC::GetTickCountUS();
	if (FAdapter == NULL )        throw OSConnectionNotExistException();
	if (!FAdapter->IsConnected()) throw OSConnectionNotConnectedException();
	if (!FAdapter->IsLogon())     throw OSConnectionNotLogonException();

	try
	{
		bool   IsAccepted = true;
		String marketAccount;

		if( FOnNewOrder != NULL )
			FOnNewOrder(this, Exchange, Symbol, Side, Px, Qty, OrderType, TimeInForce, PositionEffect, IsAccepted);
		if( IsAccepted )
		{
			switch(  Market )
			{
				case nsOrderMessageDefine::mTSE:
				case nsOrderMessageDefine::mOTC: marketAccount = FTWSEAccount;
												 break;
				default:                         marketAccount = FAccount;
												 break;
			}
			NID = FAdapter->NewSingleOrder(
				Market,
				Exchange,
				Symbol,
				Side,
				Px,
				Qty,
				OrderType,
				TimeInForce,
				PositionEffect,
				EventType,
				StrikePx,
				marketAccount,
				FActiveExecutive,
				FIBID,
				TickCount,
				StopPx,
				StrategyName,
				TWSEOrdType );

		}
		UFC::BufferedLog::Printf( " [TOrderStore::NewOrder] use[%d]us", UFC::GetTickCountUS() - Begin );
		return IsAccepted;
	}
	catch( UFC::Exception& x)
	{
		throw x;
	}
	catch (...)
	{
		 throw OSNewOrderException();
	}
}  //TOrderStore::NewOrder
//---------------------------------------------------------------------------
UFCType::Int64 TOrderStore::NewOrder( const UFC::AnsiString& FieldValueStr,
									  const UFC::AnsiString& StrategyName,
									  int TickCount )
{
	UFC::AnsiString       AnsiExchangeCode, AnsiSymbolCode;
	UFC::NameValueMessage FieldData("^\n");
	BasicInformation*     Info;

	FieldData.FromString( FieldValueStr );
	if ((FieldData.Get("exh", AnsiExchangeCode) == FALSE) || (FieldData.Get("sym", AnsiSymbolCode) == FALSE))
		return 0;
	UFCType::Int64 NewNID = FAdapter->GenerateNID( nsOrderMessageDefine::mtNew );
	if( (Info = MarketDataStore->GetBasicInformation(AnsiExchangeCode, AnsiSymbolCode, false )) == NULL)  //do not Throw Exception
	{
		UFC::AnsiString RejectMsg;
		RejectMsg.Printf("[%s][%s] Not Exist.", AnsiExchangeCode.c_str(), AnsiSymbolCode.c_str());
		FAdapter->RejectStrategyRequestDirectly( 0, 99, RejectMsg, NewNID, StrategyName);
	}
	else
	{
		nsOrderMessageDefine::MarketEnum Market = (nsOrderMessageDefine::MarketEnum)Info->GetMarket();
        ///< Add call/put and strike price if this is an Options order.
		if( Market == nsOrderMessageDefine::mForeignOptions || Market == nsOrderMessageDefine::mTWOptions || Market == nsOrderMessageDefine::mCNOptions )
		{
			switch( Info->GetCallPut() )
			{
				case cpCall: FieldData.Append( "cp", "C" );break;
				case cpPut:  FieldData.Append( "cp", "P" );break;
				case cpNone: break;
			}
			FieldData.Append( "stkpx", Info->GetStrikePrice( ));
        }
		UFC::AnsiString ansiAccount = s888::ConvertUnicodeToUFCAnsiStr(GetMarketAccountForOrder(Market));
		FAdapter->NewOrder( Market,
							NewNID,
							FieldData,
							StrategyName,
							ansiAccount,
							FActiveExecutive,
							FIBID,
							TickCount );
	}
	return NewNID;
}  //TOrderStore::NewOrder()
//---------------------------------------------------------------------------
UFCType::Int64 TOrderStore::CancelOrder(nsOrderMessageDefine::MarketEnum Market,
							   const String& Exchange,
							   const String& OrderID,
							   const UFC::AnsiString& StrategyName )
{
	if (FAdapter == 0) throw OSConnectionNotExistException();
	if (!FAdapter->IsConnected()) throw OSConnectionNotConnectedException();
	if (!FAdapter->IsLogon()) throw OSConnectionNotLogonException();
	UFCType::Int64 CancelNID;
	bool IsAccepted = true;

	try
	{
		if( FOnCancelByOrderID != 0)
			FOnCancelByOrderID(this, OrderID, IsAccepted);
		if( IsAccepted)
			CancelNID = FAdapter->CancelOrder( Market,
											   Exchange,
											   OrderID,
											   FActiveExecutive,
											   FIBID,
											   StrategyName);
		else
			CancelNID = FAdapter->GenerateNID();
	}
	catch( UFC::Exception& x )
	{
		throw x;
	}
	catch (...)
	{
		 throw OSCancelOrderException();
	}
	return CancelNID;
}
//---------------------------------------------------------------------------
int TOrderStore::CancelOrder(nsOrderMessageDefine::MarketEnum Market,
							 const String& Exchange,
							 const String& SymbolCode,
							 nsOrderMessageDefine::SideEnum Side,
							 double Price,
							 bool SkipAsk )
{
	if( FAdapter == 0 )  	throw OSConnectionNotExistException();
	if( !FAdapter->IsConnected() )		throw OSConnectionNotConnectedException();
	if( !FAdapter->IsLogon() )    		throw OSConnectionNotLogonException();
	if( SymbolCode.Length() <= 0 )		return 0;

	bool IsAccepted = true;
	int CxCount = 0;

	if( ( SkipAsk == false ) && ( FOnCancelBySymbolPrice != NULL ) )
		FOnCancelBySymbolPrice( this, SymbolCode, Side, Price, IsAccepted );
	if( IsAccepted == true )
	{
		typedef CStringTExecutionMultiMap::iterator MI;
		std::pair<MI, MI> g;
		String            IndexKey;
		IndexKey.printf( L"%s%1d%20.5lf", SymbolCode, Side, Price);
		g = FOrderSymbolSidePriceIndex.equal_range( IndexKey );
		for( MI p = g.first; p != g.second; p++)
		{
			TExecution *curOrderRootPtr = p->second;
			nsOrderMessageDefine::OrderStatusEnum OrderStatus = curOrderRootPtr->GetOrderStatus();
			if ((OrderStatus == nsOrderMessageDefine::osNew) ||
				(OrderStatus == nsOrderMessageDefine::osReplaced) ||
				(OrderStatus == nsOrderMessageDefine::osPartiallyFilled))
			{
				try
				{
					AnsiString Strategy = curOrderRootPtr->GetStrategyName();
					FAdapter->CancelOrder( Market,
										   Exchange,
										   curOrderRootPtr->GetOrderID(),
										   FActiveExecutive,
										   FIBID,
										   Strategy.c_str() );
					CxCount ++;
				}
				catch( UFC::Exception& x )
				{
					throw x;
				}
				catch (...)
				{
					throw OSCancelOrderException();
				}  //try
			}
		}  //for (MI p = g.first; p != g.second; p++)
	}
	return CxCount;
}  //TOrderStore::CancelOrder()

//---------------------------------------------------------------------------
UFCType::Int64 TOrderStore::CancelOrder(const UFC::AnsiString& FieldValueStr,
							   const UFC::AnsiString& StrategyName)
{
	return FAdapter->CancelOrder( FieldValueStr,
								  StrategyName,
								  FActiveExecutive,
								  FIBID );
}  //TOrderStore::CancelOrder()
//---------------------------------------------------------------------------
void TOrderStore::CancelAllOrder()
{
	if (FAdapter == 0)            throw OSConnectionNotExistException();
	if (!FAdapter->IsConnected()) throw OSConnectionNotConnectedException();
	if (!FAdapter->IsLogon())     throw OSConnectionNotLogonException();

	typedef CStringTExecutionMap::iterator MI;
	TExecution*                            CurOrderRootPtr;
	nsOrderMessageDefine::OrderStatusEnum  OrderRootStatus;

	std::vector<nsOrderMessageDefine::MarketEnum> cancelMarketArray;
	std::vector<String> cancelOrderIDArray;
	std::vector<String> cancelExchangeArray;
	int cancelCount = 0;
	for (MI p = FActiveOrderRootTimeOrderIDIndex.begin(); p != FActiveOrderRootTimeOrderIDIndex.end(); p++)
	{
		CurOrderRootPtr = p->second;
		OrderRootStatus = CurOrderRootPtr->GetOrderStatus();
		if ((OrderRootStatus == nsOrderMessageDefine::osNew) ||
			(OrderRootStatus == nsOrderMessageDefine::osReplaced) ||
			(OrderRootStatus == nsOrderMessageDefine::osPartiallyFilled))
		{
			try
			{
				if (!FIsTestMode)
				{
					FAdapter->CancelOrder( CurOrderRootPtr->GetMarket(),
										   CurOrderRootPtr->GetExchangeCode(),
										   CurOrderRootPtr->GetOrderID(),
										   FActiveExecutive,
										   FIBID );
				}
				else
				{
					cancelMarketArray.push_back(CurOrderRootPtr->GetMarket());
					cancelOrderIDArray.push_back(CurOrderRootPtr->GetOrderID());
					cancelExchangeArray.push_back(CurOrderRootPtr->GetExchangeCode());
					cancelCount++;
				}
			}
			catch( UFC::Exception& x )
			{
				throw x;
			}
			catch (...)
			{
				throw OSCancelOrderException();
			}  //try
		}
	}  //for (MI p = FActiveOrderRootTimeOrderIDIndex.begin(); p != FActiveOrderRootTimeOrderIDIndex.end(); p++)

	if (FIsTestMode && (cancelCount > 0))
	{
		for (int i = 0; i < cancelCount; i++)
		{
			nsOrderMessageDefine::MarketEnum curMarket = cancelMarketArray[i];
			String curOrderID = cancelOrderIDArray[i];
			String curExchangeCode = cancelExchangeArray[i];
			FAdapter->CancelOrder( curMarket, curExchangeCode, curOrderID, FActiveExecutive, FIBID );
	    }
	}
}  //TOrderStore::CancelAllOrder()
//---------------------------------------------------------------------------
bool __fastcall TOrderStore::IsActiveeOrder( nsOrderMessageDefine::OrderStatusEnum  OrderRootStatus )
{
	if ((OrderRootStatus == nsOrderMessageDefine::osNew) ||
		(OrderRootStatus == nsOrderMessageDefine::osReplaced) ||
		(OrderRootStatus == nsOrderMessageDefine::osPartiallyFilled))
		return true;
	return false;
}
//---------------------------------------------------------------------------
int TOrderStore::CancelAllOrder( const String& Exchange, const String& Symbol )
{
	if (FAdapter == 0)            throw OSConnectionNotExistException();
	if (!FAdapter->IsConnected()) throw OSConnectionNotConnectedException();
	if (!FAdapter->IsLogon())     throw OSConnectionNotLogonException();

	typedef CStringTExecutionMap::iterator        MI;
	TExecution*                                   CurOrderRootPtr;
	nsOrderMessageDefine::OrderStatusEnum         OrderRootStatus;
	std::vector<nsOrderMessageDefine::MarketEnum> cancelMarketArray;
	std::vector<String>                           cancelOrderIDArray;
	std::vector<String>                           cancelExchangeArray;
	int cancelCount = 0;

	for (MI p = FActiveOrderRootTimeOrderIDIndex.begin(); p != FActiveOrderRootTimeOrderIDIndex.end(); p++)
	{
		CurOrderRootPtr = p->second;
		OrderRootStatus = CurOrderRootPtr->GetOrderStatus();
		if( IsActiveeOrder(OrderRootStatus) == true &&
			CurOrderRootPtr->GetExchangeCode() == Exchange &&
			CurOrderRootPtr->GetSymbol()->GetSymbol(0) == Symbol )
		{
			try
			{
				if (!FIsTestMode)
				{
					FAdapter->CancelOrder( CurOrderRootPtr->GetMarket(),
										   CurOrderRootPtr->GetExchangeCode(),
										   CurOrderRootPtr->GetOrderID(),
										   FActiveExecutive,
										   FIBID );
					cancelCount++;
				}
				else
				{
					cancelMarketArray.push_back(CurOrderRootPtr->GetMarket());
					cancelExchangeArray.push_back(CurOrderRootPtr->GetExchangeCode());
					cancelOrderIDArray.push_back(CurOrderRootPtr->GetOrderID());
					cancelCount++;
				}
			}
			catch( UFC::Exception& x )
			{
				throw x;
			}
			catch (...)
			{
				throw OSCancelOrderException();
			}  //try
		}
	}  //for (MI p = FActiveOrderRootTimeOrderIDIndex.begin(); p != FActiveOrderRootTimeOrderIDIndex.end(); p++)
	if( FIsTestMode && (cancelCount > 0) )
	{
		for (int i = 0; i < cancelCount; i++)
		{
			nsOrderMessageDefine::MarketEnum curMarket = cancelMarketArray[i];
			String                           curOrderID = cancelOrderIDArray[i];
			String                           curExchangeCode = cancelExchangeArray[i];

			FAdapter->CancelOrder( curMarket, curExchangeCode, curOrderID, FActiveExecutive, FIBID );
		}
	}
	return cancelCount;
}  //TOrderStore::CancelAllOrder()
//---------------------------------------------------------------------------
void TOrderStore::AmendOrder(nsOrderMessageDefine::MarketEnum Market,
							 const String& Exchange,
							 const String& OrderID, int Qty)
{
	if (FAdapter == 0)            throw OSConnectionNotExistException();
	if (!FAdapter->IsConnected()) throw OSConnectionNotConnectedException();
	if (!FAdapter->IsLogon())     throw OSConnectionNotLogonException();

	try
	{
		FAdapter->AmendOrder(Market, Exchange, OrderID, Qty);
	}
	catch( UFC::Exception& x )
	{
		throw x;
	}
	catch (...)
	{
		 throw OSAmendOrderException();
	}  //try
}  //TOrderStore::AmendOrder()

//---------------------------------------------------------------------------
void TOrderStore::ReplaceOrderPrice( nsOrderMessageDefine::MarketEnum Market,
									 const String& Exchange,
									 const String& OrderID,
									 nsOrderMessageDefine::OrderTypeEnum OrderType,
									 nsOrderMessageDefine::TimeInForceEnum TimeInForce,
									 double NewPrice, bool SupportRPX  )
{
	if (FAdapter == 0)            throw OSConnectionNotExistException();
	if (!FAdapter->IsConnected()) throw OSConnectionNotConnectedException();
	if (!FAdapter->IsLogon())     throw OSConnectionNotLogonException();

	try
	{
		if( SupportRPX == false )
		{
			TExecution* CurOrderRootPtr =  FindOrderRootByOrderID( OrderID );
			if( CurOrderRootPtr != NULL )
				CancelAndNewAsReplace( Market, Exchange, OrderType, TimeInForce, NewPrice, CurOrderRootPtr );
		}
		else
			FAdapter->ReplaceOrderPrice( Market, Exchange, OrderID, OrderType, TimeInForce, NewPrice);
	}
	catch( UFC::Exception& x )
	{
		throw x;
	}
	catch (...)
	{
		 throw OSReplaceOrderPriceException();
	}  //try
}  //TOrderStore::ReplaceOrderPrice()
//---------------------------------------------------------------------------
void __fastcall TOrderStore::CancelAndNewAsReplace(  nsOrderMessageDefine::MarketEnum      Market,
													 const String&                         Exchange,
													 nsOrderMessageDefine::OrderTypeEnum   OrderType,
													 nsOrderMessageDefine::TimeInForceEnum TimeInForce,
													 double                                NewPrice,
													 TExecution*                           CurOrderRootPtr )
{
	nsOrderMessageDefine::EventTypeEnum EventType;
	int StopCancelCount = FCancelReportCount + 1;

	CancelOrder( Market, Exchange, CurOrderRootPtr->GetOrderID() );
	for( int i = 0; i < 500; i++ )
	{
		UFC::SleepMS( 10 );
		Application->ProcessMessages();
		if( FCancelReportCount >= StopCancelCount )
		{
			if( CurOrderRootPtr->GetSymbol()->GetCallPut() == 'C' )
				EventType = nsOrderMessageDefine::evtCall;
			else if( CurOrderRootPtr->GetSymbol()->GetCallPut() == 'P' )
				EventType = nsOrderMessageDefine::evtPut;
			else
				EventType = nsOrderMessageDefine::evtNone;
			NewOrder( Market, Exchange,
					  CurOrderRootPtr->GetSymbol()->GetOriginalSymbol(),
					  CurOrderRootPtr->GetSide(),
					  NewPrice,
					  CurOrderRootPtr->GetLeavesQty(),
					  OrderType,
					  TimeInForce,
					  CurOrderRootPtr->GetPositionEffect(),
					  EventType,
					  CurOrderRootPtr->GetSymbol()->GetStrikePx(), 0, 0.0, '0' );
			return;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOrderStore::CancelAndNewAsReplace(  nsOrderMessageDefine::MarketEnum Market,
													 const String&                    Exchange,
													 double                           NewPrice,
													 TExecution*                      CurOrderRootPtr )
{
	CancelAndNewAsReplace( Market,
						   Exchange,
						   CurOrderRootPtr->GetOrderType(),
						   CurOrderRootPtr->GetTimeInForce(),
						   NewPrice,
						   CurOrderRootPtr );
}
//---------------------------------------------------------------------------
void TOrderStore::ReplaceOrderPrice(nsOrderMessageDefine::MarketEnum Market,
									const String& Exchange,
									const String& SymbolCode,
									nsOrderMessageDefine::SideEnum Side,
									double Price, double NewPrice, bool SupportRPX )
{
	if (FAdapter == 0)            throw OSConnectionNotExistException();
	if (!FAdapter->IsConnected()) throw OSConnectionNotConnectedException();
	if (!FAdapter->IsLogon())     throw OSConnectionNotLogonException();
	if ((SymbolCode.Length() <= 0) || (Price <= 0)) return;

	typedef CStringTExecutionMultiMap::iterator MI;
	std::pair<MI, MI>      g;
	TExecution*            CurOrderRootPtr;
	UFC::List<TExecution*> Orders;
	String                 IndexKey;

	IndexKey.printf( L"%s%1d%20.5lf", SymbolCode.c_str(), Side, Price);
	g = FOrderSymbolSidePriceIndex.equal_range( IndexKey );
	for( MI p = g.first; p != g.second; p++)
	{
		CurOrderRootPtr = p->second;
		nsOrderMessageDefine::OrderStatusEnum OrderStatus = CurOrderRootPtr->GetOrderStatus();
		if ((OrderStatus == nsOrderMessageDefine::osNew) ||
			(OrderStatus == nsOrderMessageDefine::osReplaced) ||
			(OrderStatus == nsOrderMessageDefine::osPartiallyFilled))
			Orders.Add( CurOrderRootPtr );
	}
	for( register int i = Orders.ItemCount()-1; i >= 0; i-- )
	{
		try
		{
			CurOrderRootPtr = Orders[ i ];
			if( SupportRPX == false )
				CancelAndNewAsReplace( Market, Exchange, NewPrice, CurOrderRootPtr );
			else
				FAdapter->ReplaceOrderPrice( Market, Exchange,
						CurOrderRootPtr->GetOrderID(),
						CurOrderRootPtr->GetOrderType(),
						CurOrderRootPtr->GetTimeInForce(),
						NewPrice );
		}
		catch( UFC::Exception& x )
		{
			throw x;
		}
		catch (...)
		{
			throw OSCancelOrderException();
		}
	}
}  //TOrderStore::ReplaceOrderPrice()

//---------------------------------------------------------------------------
void TOrderStore::RequestOrderStatus(int OrderNID,
									 nsOrderMessageDefine::MarketEnum Market,
									 const String& Exchange,
									 const String& OrderID, const String& Symbol,
									 nsOrderMessageDefine::SideEnum Side, double Px)
{
	if (FAdapter == 0)            throw OSConnectionNotExistException();
	if (!FAdapter->IsConnected()) throw OSConnectionNotConnectedException();
	if (!FAdapter->IsLogon())     throw OSConnectionNotLogonException();
	try
	{
		FAdapter->RequestOrderStatus(OrderNID, Market, Exchange, OrderID, Symbol, Side, Px);
	}
	catch( UFC::Exception& x )
	{
		throw x;
	}
	catch (...)
	{
		 throw OSRequestOrderStatusException();
	}  //try
}  //TOrderStore::RequestOrderStatus()
//---------------------------------------------------------------------------
//
// The margin/position response may arrive malformed for JSON parsing:
//   1. The whole payload is wrapped in an extra pair of double quotes.
//   2. Object members are separated by newlines instead of commas.
// Normalize it so TJSONObject::ParseJSONValue can succeed.
// e.g.  "{\n "Margin":"48806906.0000"\n "Position":\n [\n ]\n}"\n
//
String TOrderStore::NormalizeBalanceJSON( const String& raw )
{
	String s = raw.Trim();

	// Strip one surrounding layer of double quotes, if present.
	if( s.Length() >= 2 && s[1] == L'"' && s[s.Length()] == L'"' )
		s = s.SubString( 2, s.Length() - 2 ).Trim();

	// Rebuild, dropping newlines and inserting commas where members were
	// only separated by a line break (bracket-depth aware, string aware).
	String out;
	int  depth    = 0;
	bool inString = false;

	for( int i = 1; i <= s.Length(); ++i )
	{
		wchar_t c = s[i];

		if( inString )
		{
			out += c;
			if( c == L'"' && s[i - 1] != L'\\' )
				inString = false;
			continue;
		}

		switch( c )
		{
			case L'"':
				inString = true;
				out += c;
				break;
			case L'{':
			case L'[':
				++depth;
				out += c;
				break;
			case L'}':
			case L']':
				--depth;
				out += c;
				break;
			case L'\n':
			case L'\r':
			{
				// A line break inside an object may be a member separator.
				int j = i + 1;
				while( j <= s.Length() &&
					   ( s[j] == L' ' || s[j] == L'\t' || s[j] == L'\n' || s[j] == L'\r' ) )
					++j;

				wchar_t prev = out.IsEmpty() ? 0 : out[out.Length()];
				wchar_t next = ( j <= s.Length() ) ? s[j] : 0;

				bool prevEndsToken = ( prev == L'"' || prev == L'}' || prev == L']' ||
									   ( prev >= L'0' && prev <= L'9' ) ||
									   prev == L'e' || prev == L'l' );          // number / true|false / null
				bool nextStartsToken = ( next == L'"' || next == L'{' || next == L'[' ||
										 next == L'-' || ( next >= L'0' && next <= L'9' ) ||
										 next == L't' || next == L'f' || next == L'n' );

				if( depth > 0 && prevEndsToken && nextStartsToken )
					out += L',';
				// otherwise: treat the line break as whitespace and drop it
				break;
			}
			default:
				out += c;
		}
	}

	return out;
}
//---------------------------------------------------------------------------
UFC::AnsiString TOrderStore::GetBalance( void )
{
	UFC::AnsiString result;
	FAdapter->MarginPositionRequest( "F030000", FAccount.c_str(), result);
	
	TJSONValue* jsonValue = TJSONObject::ParseJSONValue(NormalizeBalanceJSON(String(result.c_str())));
	if (jsonValue == NULL)
		return result;

	try
	{
		TJSONObject* jsonObject = dynamic_cast<TJSONObject*>(jsonValue);
		if (jsonObject == NULL)
			return result;

		TJSONValue* marginValue = jsonObject->GetValue(L"Margin");
		if (marginValue == NULL)
			return result;

		// Margin 原始值可能帶多位小數 (e.g. "48806906.0000")，以金額呈現：
		// 每 3 位數加千分位逗號，最多顯示小數點後 2 位
		double margin = StrToFloatDef( marginValue->Value(), 0.0 );
		return UFC::AnsiString(AnsiString(FormatFloat("#,##0.00", margin)).c_str());
	}
	__finally
	{
		delete jsonValue;
	}

	return result;
}
//---------------------------------------------------------------------------
double TOrderStore::GetNewestPrice( const String& ExchangeCode, const String& Symbol )
{
	double newestPrice = 0.0;

	if( (ExchangeCode.Length() <= 0) || (Symbol.Length() <= 0.0))
		return newestPrice;
	s888::CPositionStatisticRecord *positionStatisticRecPtr = FindOrCreatePosStatisticRec( ExchangeCode, Symbol );
	if( (positionStatisticRecPtr == NULL ) || (positionStatisticRecPtr->GetMarketTradeQuantitySum(s888::rwLockForRead) <= 0))
	{
		BasicInformation *infoPtr = GetCommodityBasicInformation(ExchangeCode, Symbol);
		if( infoPtr != NULL )
			newestPrice = infoPtr->GetRefPrice();
	}
	else
		newestPrice = positionStatisticRecPtr->GetLastPrice( s888::rwLockForRead );
	return newestPrice;
}
//---------------------------------------------------------------------------
void TOrderStore::CancelStopOrderQuantity(const String& StopExchangeCode, const String& StopSymbol, double StopPrice,
										  const String& ExchangeCode, const String& Symbol, double Price,
										  nsOrderMessageDefine::SideEnum Side, int CancelQuantity)
{
	if ((FMarketDataStore == 0) ||
		(StopExchangeCode.Length() <= 0) || (StopSymbol.Length() <= 0) ||
		(ExchangeCode.Length() <= 0) || (Symbol.Length() <= 0) || (CancelQuantity <= 0)) return;

	BasicInformation *stopInfoPtr = GetCommodityBasicInformation(StopExchangeCode, StopSymbol);
	if (stopInfoPtr == 0) return;
	nsOrderMessageDefine::MarketEnum stopMarket = static_cast<nsOrderMessageDefine::MarketEnum>(stopInfoPtr->GetMarket());

	BasicInformation *targetInfoPtr = GetCommodityBasicInformation(ExchangeCode, Symbol);
	if (targetInfoPtr == 0) return;
	nsOrderMessageDefine::MarketEnum targetMarket = static_cast<nsOrderMessageDefine::MarketEnum>(targetInfoPtr->GetMarket());
	String targetTradeSymbol = GetTradeSymbol(ExchangeCode, Symbol, true);  //Check Symbol Index First

	TExecution* curStopOrderPtr = FindStopOrderConditionIndex(StopExchangeCode, StopSymbol, StopPrice, ExchangeCode, Symbol, Price, Side);
	if (curStopOrderPtr == 0) return;
	int curStopOrderQuantity = curStopOrderPtr->GetOrderQty();
	int restOrderQuantity = curStopOrderQuantity - CancelQuantity;
	if (restOrderQuantity <= 0)
	{
		if (FOnStopOrderChange != 0)
		{
			bool IsAccepted = false;
			FOnStopOrderChange(this, StopExchangeCode, StopSymbol, StopPrice,
							   curStopOrderPtr->GetSymbol()->GetSymbol(0),
							   curStopOrderPtr->GetPrice(), curStopOrderPtr->GetOrderQty(),
							   curStopOrderPtr->GetTimeInForce(), curStopOrderPtr->GetSide(),
							   soRemove, IsAccepted);
			if (!IsAccepted) return;
		}  //if (FOnStopOrderChange != 0)

		//UnSubscribe Symbol Price Data
		UnsubscribeSymbol(StopExchangeCode, StopSymbol, false);  //Unsubscribe if need
	}  //if (restOrderQuantity <= 0)

	RemoveStopOrderConditionIndex(StopExchangeCode, StopSymbol, StopPrice, ExchangeCode, Symbol, Price, Side);
	RemoveStopOrderTimeNIDIndex(curStopOrderPtr);
	curStopOrderPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
	curStopOrderPtr->WriteToOrderLogStream(FStopOrderFileStream);
	ExecuteStopOrderChanged(curStopOrderPtr, soRemove);
	ExecuteOrderQtyListenerForStopOrder(curStopOrderPtr->GetMarket(), ExchangeCode, StopSymbol, StopPrice, Side, Price, 0, curStopOrderPtr->GetOrderType());
	AnsiString ansiStopExchangeCode = s888::ConvertUnicodeToAnsiStr(StopExchangeCode);
	AnsiString ansiStopSymbol = s888::ConvertUnicodeToAnsiStr(StopSymbol);
	AnsiString ansiSymbol = s888::ConvertUnicodeToAnsiStr(Symbol);
	UFC::BufferedLog::Printf(" Cancel Stop Order Quantity Condition[%s-%s][%.3lf] Product[%s] Px[%.3lf] Qty[%d]",
							 ansiStopExchangeCode.c_str(), ansiStopSymbol.c_str(), StopPrice, ansiSymbol.c_str(), Price, curStopOrderQuantity);

	if (restOrderQuantity > 0)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		String localMessageTime;
		localMessageTime.printf(L"%02d%02d%02d%03d", sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
		UFCType::Int64 newNID = FAdapter->GenerateNID();
		TExecution *newStopOrderPtr = new TExecution();
		newStopOrderPtr->SetMarket(curStopOrderPtr->GetMarket());
		newStopOrderPtr->SetExchangeCode(ExchangeCode);
		newStopOrderPtr->SetSymbol(Symbol);
		newStopOrderPtr->SetSide(curStopOrderPtr->GetSide());
		newStopOrderPtr->SetPrice(curStopOrderPtr->GetPrice());
		newStopOrderPtr->SetOrderQty(restOrderQuantity);
		newStopOrderPtr->SetOrderType(curStopOrderPtr->GetOrderType());
		newStopOrderPtr->SetTimeInForce(curStopOrderPtr->GetTimeInForce());
		newStopOrderPtr->SetPositionEffect(curStopOrderPtr->GetPositionEffect());
		newStopOrderPtr->SetNID(newNID);
		newStopOrderPtr->SetLocalMessageTime(localMessageTime);
		newStopOrderPtr->SetStopExchangeCode(StopExchangeCode);
		newStopOrderPtr->SetStopSymbol(StopSymbol);
		newStopOrderPtr->SetStopPrice(curStopOrderPtr->GetStopPrice());
		newStopOrderPtr->SetStopMarketPrice(GetNewestPrice(StopExchangeCode, StopSymbol));
		newStopOrderPtr->SetOrderStatus(nsOrderMessageDefine::osNew);
		newStopOrderPtr->SetRoot(true);
		newStopOrderPtr->SetExpanded(false);
		newStopOrderPtr->SetCumQty(0);
		newStopOrderPtr->SetTradeSymbol(targetTradeSymbol);
		String stopPriceString;
		stopPriceString.printf(L"%.5lf", curStopOrderPtr->GetStopPrice());
		String tempText = Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_NEW_TEXT1 + StopSymbol +
						  Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_NEW_TEXT2 + stopPriceString +
						  Mdcomponentstrings_MD_ORDERSTORE_STOP_ORDER_NEW_TEXT3;
		newStopOrderPtr->SetText(tempText);
		NewStopOrder(newStopOrderPtr, false);
	}
	delete curStopOrderPtr;
}  //TOrderStore::CancelStopOrderQuantity()
//---------------------------------------------------------------------------
void TOrderStore::AddOrderRoot(TExecution* OrderRootPtr)
{
	if (OrderRootPtr == 0) return;
	OrderRootPtr->SetRoot(true);
	FOrderRootList.Add(OrderRootPtr);
}  //TOrderStore::AddOrderRoot()
//---------------------------------------------------------------------------
void TOrderStore::UpdateDisplayName( void )
{
	TExecution* OrderRootPtr;
	for( int i = 0; i < FOrderRootList.ItemCount(); i++ )
	{
		OrderRootPtr = FOrderRootList.GetItem( i );
		OrderRootPtr->UpdateDisplayName();
    }
}
//---------------------------------------------------------------------------
int TOrderStore::QueryPendingNewOrderRoot()
{
	int QueryCount = 0;
	if (FOrderRootTimeOrderIDIndex.size() <= 0) return QueryCount;

	//Get All PendingNew Order Root
	typedef CStringTExecutionMap::iterator MI;
	TExecution* CurOrderRootPtr;
	for (MI it = FOrderRootTimeOrderIDIndex.begin(); it != FOrderRootTimeOrderIDIndex.end(); it++)
	{
		CurOrderRootPtr = it->second;
		if (CurOrderRootPtr->GetOrderStatus() == nsOrderMessageDefine::osPendingNew)
		{
			RequestOrderStatus(CurOrderRootPtr->GetNID(), CurOrderRootPtr->GetMarket(),
							   CurOrderRootPtr->GetExchangeCode(),
							   CurOrderRootPtr->GetOrderID(), CurOrderRootPtr->GetSymbol()->GetSymbol(0),
							   CurOrderRootPtr->GetSide(), CurOrderRootPtr->GetPrice());
			QueryCount++;
		}
	}

	return QueryCount;
}  //TOrderStore::QueryPendingNewOrderRoot()
//---------------------------------------------------------------------------
int TOrderStore::DeleteRejectedOrderRoot(int PreserveCount)
{
	int DeleteCount = 0;
	if (PreserveCount < 0) return DeleteCount;
	if (FOrderRootTimeOrderIDIndex.size() <= 0) return DeleteCount;

	//Get All Rejected Order Root
	typedef CStringTExecutionMap::iterator MI;
	UFC::PtrList<TExecution> RejectedOrderRootList;
	TExecution* CurOrderRootPtr;
	for (MI it = FOrderRootTimeOrderIDIndex.begin(); it != FOrderRootTimeOrderIDIndex.end(); it++)
	{
		CurOrderRootPtr = it->second;
		if (CurOrderRootPtr->GetOrderStatus() == nsOrderMessageDefine::osRejected)
			RejectedOrderRootList.Add(CurOrderRootPtr);
	}

	if (RejectedOrderRootList.ItemCount() <= PreserveCount)
		return DeleteCount;

	UnicodeString TimeOrderIDKey;
	for (int i = 0; i < RejectedOrderRootList.ItemCount(); i++)
	{
		if ((RejectedOrderRootList.ItemCount() - DeleteCount) <= PreserveCount)
			break;

		CurOrderRootPtr = RejectedOrderRootList.GetItem(i);
		TimeOrderIDKey = CurOrderRootPtr->GetTimeOrderIDKey();
		RemoveOrderRootByTimeOrderID(TimeOrderIDKey);
		RemoveActiveOrderRootByTimeOrderID(TimeOrderIDKey);

		if (FOrderMessageListenerList.ItemCount() > 0)
		{
			TOrderMessageListener* CurListener;
			for (int i = 0; i < FOrderMessageListenerList.ItemCount(); i++)
			{
				CurListener = FOrderMessageListenerList.GetItem(i);
				CurListener->DeleteRejectedOrderRoot(CurOrderRootPtr);
			}  //for (int i = 0; i < FOrderQtyListenerList.ItemCount(); i++)
		}  //if (FOrderMessageListenerList.ItemCount() > 0)

		DeleteCount++;
	}  //for (int i = 0; i < RejectedOrderRootList.ItemCount(); i++)

	return DeleteCount;
}  //TOrderStore::DeleteRejectedOrderRoot()

//---------------------------------------------------------------------------
TExecution* TOrderStore::FindOrderRootByNID(UFCType::Int64 NIDValue)
{
	TExecution* ExecutionPtr = 0;
	if (FOrderRootNIDIndex.size() <= 0) return ExecutionPtr;
	CInt64TExecutionMap::iterator it = FOrderRootNIDIndex.find(NIDValue);
	if (it != FOrderRootNIDIndex.end()) ExecutionPtr = it->second;
	return ExecutionPtr;
}  //TOrderStore::FindOrderRootByNID()

//---------------------------------------------------------------------------
bool TOrderStore::InsertOrderRootNIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist)
{
	bool IsSuccess = false;
	if( OrderRootPtr == NULL ) return false;

	UFCType::Int64 RootNID = OrderRootPtr->GetNID();
	if ((NeedCheckExist) && (FindOrderRootByNID(RootNID) != 0)) return false;

	std::pair<CInt64TExecutionMap::iterator, bool> InsertPair;
	InsertPair = FOrderRootNIDIndex.insert( CInt64TExecutionMap::value_type( RootNID, OrderRootPtr ));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TOrderStore::InsertOrderRootNIDIndex()

//---------------------------------------------------------------------------
TExecution* TOrderStore::FindOrderRootByOrderID(UnicodeString OrderID)
{
	TExecution* ExecutionPtr = 0;
	if ((FOrderRootOrderIDIndex.size() <= 0) || (OrderID.Length() <= 0)) return ExecutionPtr;

	CStringTExecutionMap::iterator it = FOrderRootOrderIDIndex.find(OrderID);
	if (it != FOrderRootOrderIDIndex.end()) ExecutionPtr = it->second;

	return ExecutionPtr;
}  //TOrderStore::FindOrderRootByOrderID()

//---------------------------------------------------------------------------
bool TOrderStore::InsertOrderRootOrderIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist)
{
	bool IsSuccess = false;
	if (OrderRootPtr == 0) return IsSuccess;
	UnicodeString IndexKey = OrderRootPtr->GetOrderID();
	if (IndexKey.Length() <= 0) return IsSuccess;
	if ((NeedCheckExist) && (FindOrderRootByOrderID(IndexKey) != 0)) return IsSuccess;

	std::pair<CStringTExecutionMap::iterator, bool> InsertPair;
	InsertPair = FOrderRootOrderIDIndex.insert(CStringTExecutionMap::value_type(IndexKey, OrderRootPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TOrderStore::InsertOrderRootOrderIDIndex()

//---------------------------------------------------------------------------
TExecution* TOrderStore::FindOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey)
{
	TExecution* ExecutionPtr = 0;
	if ((FOrderRootTimeOrderIDIndex.size() <= 0) || (TimeOrderIDKey.Length() <= 0)) return ExecutionPtr;

	CStringTExecutionMap::iterator it = FOrderRootTimeOrderIDIndex.find(TimeOrderIDKey);
	if (it != FOrderRootTimeOrderIDIndex.end()) ExecutionPtr = it->second;

	return ExecutionPtr;
}  //TOrderStore::FindOrderRootByTimeOrderID()

//---------------------------------------------------------------------------
bool TOrderStore::InsertOrderRootTimeOrderIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist)
{
	bool IsSuccess = false;
	if (OrderRootPtr == 0) return IsSuccess;
	UnicodeString IndexKey = OrderRootPtr->GetTimeOrderIDKey();
	if (IndexKey.Length() <= 0) return IsSuccess;
	if ((NeedCheckExist) && (FindOrderRootByTimeOrderID(IndexKey) != 0)) return IsSuccess;

	std::pair<CStringTExecutionMap::iterator, bool> InsertPair;
	InsertPair = FOrderRootTimeOrderIDIndex.insert(CStringTExecutionMap::value_type(IndexKey, OrderRootPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TOrderStore::InsertOrderRootTimeOrderIDIndex()

//---------------------------------------------------------------------------
bool TOrderStore::RemoveOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey)
{
	bool IsSuccess = false;
	if (TimeOrderIDKey.Length() <= 0) return IsSuccess;

	CStringTExecutionMap::iterator it = FOrderRootTimeOrderIDIndex.find(TimeOrderIDKey);
	if (it != FOrderRootTimeOrderIDIndex.end())
	{
		FOrderRootTimeOrderIDIndex.erase(it);
		IsSuccess = true;
	}

	return IsSuccess;
}  //TOrderStore::RemoveOrderRootByTimeOrderID()

//---------------------------------------------------------------------------
TExecution* TOrderStore::FindFilledOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey)
{
	TExecution* ExecutionPtr = 0;
	if ((FFilledOrderRootTimeOrderIDIndex.size() <= 0) || (TimeOrderIDKey.Length() <= 0)) return ExecutionPtr;

	CStringTExecutionMap::iterator it = FFilledOrderRootTimeOrderIDIndex.find(TimeOrderIDKey);
	if (it != FFilledOrderRootTimeOrderIDIndex.end()) ExecutionPtr = it->second;

	return ExecutionPtr;
}  //TOrderStore::FindFilledOrderRootByTimeOrderID()

//---------------------------------------------------------------------------
bool TOrderStore::InsertFilledOrderRootTimeOrderIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist)
{
	bool IsSuccess = false;
	if (OrderRootPtr == 0) return IsSuccess;
	UnicodeString IndexKey = OrderRootPtr->GetTimeOrderIDKey();
	if (IndexKey.Length() <= 0) return IsSuccess;
	if ((NeedCheckExist) && (FindFilledOrderRootByTimeOrderID(IndexKey) != 0)) return IsSuccess;
	std::pair<CStringTExecutionMap::iterator, bool> InsertPair;
	InsertPair = FFilledOrderRootTimeOrderIDIndex.insert(CStringTExecutionMap::value_type(IndexKey, OrderRootPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TOrderStore::InsertFilledOrderRootTimeOrderIDIndex()

//---------------------------------------------------------------------------
bool TOrderStore::RemoveFilledOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey)
{
	bool IsSuccess = false;
	if (TimeOrderIDKey.Length() <= 0) return IsSuccess;

	CStringTExecutionMap::iterator it = FFilledOrderRootTimeOrderIDIndex.find(TimeOrderIDKey);
	if (it != FFilledOrderRootTimeOrderIDIndex.end())
	{
		FFilledOrderRootTimeOrderIDIndex.erase(it);
		IsSuccess = true;
	}

	return IsSuccess;
}  //TOrderStore::RemoveFilledOrderRootByTimeOrderID()

//---------------------------------------------------------------------------
TExecution* TOrderStore::FindCanceledOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey)
{
	TExecution* ExecutionPtr = 0;
	if ((FCanceledOrderRootTimeOrderIDIndex.size() <= 0) || (TimeOrderIDKey.Length() <= 0)) return ExecutionPtr;

	CStringTExecutionMap::iterator it = FCanceledOrderRootTimeOrderIDIndex.find(TimeOrderIDKey);
	if (it != FCanceledOrderRootTimeOrderIDIndex.end()) ExecutionPtr = it->second;

	return ExecutionPtr;
}  //TOrderStore::FindCanceledOrderRootByTimeOrderID()

//---------------------------------------------------------------------------
bool TOrderStore::InsertCanceledOrderRootTimeOrderIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist)
{
	bool IsSuccess = false;
	if (OrderRootPtr == 0) return IsSuccess;
	UnicodeString IndexKey = OrderRootPtr->GetTimeOrderIDKey();
	if (IndexKey.Length() <= 0) return IsSuccess;
	if ((NeedCheckExist) && (FindCanceledOrderRootByTimeOrderID(IndexKey) != 0)) return IsSuccess;

	std::pair<CStringTExecutionMap::iterator, bool> InsertPair;
	InsertPair = FCanceledOrderRootTimeOrderIDIndex.insert(CStringTExecutionMap::value_type(IndexKey, OrderRootPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TOrderStore::InsertCanceledOrderRootTimeOrderIDIndex()

//---------------------------------------------------------------------------
bool TOrderStore::RemoveCanceledOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey)
{
	bool IsSuccess = false;
	if (TimeOrderIDKey.Length() <= 0) return IsSuccess;

	CStringTExecutionMap::iterator it = FCanceledOrderRootTimeOrderIDIndex.find(TimeOrderIDKey);
	if (it != FCanceledOrderRootTimeOrderIDIndex.end())
	{
		FCanceledOrderRootTimeOrderIDIndex.erase(it);
		IsSuccess = true;
	}

	return IsSuccess;
}  //TOrderStore::RemoveCanceledOrderRootByTimeOrderID()

//---------------------------------------------------------------------------
TExecution* TOrderStore::FindActiveOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey)
{
	TExecution* ExecutionPtr = 0;
	if ((FActiveOrderRootTimeOrderIDIndex.size() <= 0) || (TimeOrderIDKey.Length() <= 0)) return ExecutionPtr;

	CStringTExecutionMap::iterator it = FActiveOrderRootTimeOrderIDIndex.find(TimeOrderIDKey);
	if (it != FActiveOrderRootTimeOrderIDIndex.end()) ExecutionPtr = it->second;

	return ExecutionPtr;
}  //TOrderStore::FindActiveOrderRootByTimeOrderID()

//---------------------------------------------------------------------------
bool TOrderStore::InsertActiveOrderRootTimeOrderIDIndex(TExecution* OrderRootPtr, bool NeedCheckExist)
{
	bool IsSuccess = false;
	if (OrderRootPtr == 0) return IsSuccess;
	UnicodeString IndexKey = OrderRootPtr->GetTimeOrderIDKey();
	if (IndexKey.Length() <= 0) return IsSuccess;
	if ((NeedCheckExist) && (FindActiveOrderRootByTimeOrderID(IndexKey) != 0)) return IsSuccess;

	std::pair<CStringTExecutionMap::iterator, bool> InsertPair;
	InsertPair = FActiveOrderRootTimeOrderIDIndex.insert(CStringTExecutionMap::value_type(IndexKey, OrderRootPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TOrderStore::InsertActiveOrderRootTimeOrderIDIndex()

//---------------------------------------------------------------------------
bool TOrderStore::RemoveActiveOrderRootByTimeOrderID(UnicodeString TimeOrderIDKey)
{
	bool IsSuccess = false;
	if (TimeOrderIDKey.Length() <= 0)
		return IsSuccess;
	else
		IsSuccess = true;

	CStringTExecutionMap::iterator it = FActiveOrderRootTimeOrderIDIndex.find(TimeOrderIDKey);
	if (it != FActiveOrderRootTimeOrderIDIndex.end())
		FActiveOrderRootTimeOrderIDIndex.erase(it);

	return IsSuccess;
}  //TOrderStore::RemoveActiveOrderRootByTimeOrderID()

//---------------------------------------------------------------------------
bool TOrderStore::InsertOrderRootSymbolSidePriceIndex(TExecution* OrderRootPtr)
{
	bool IsSuccess = false;
	if (OrderRootPtr == 0) return IsSuccess;

	TExchangeSymbol *SymbolPtr = OrderRootPtr->GetSymbol();
	if (SymbolPtr == 0) return IsSuccess;

	String exchangeCode = OrderRootPtr->GetExchangeCode();
	String OrderPriceString;
	OrderPriceString.printf(L"%20.5lf", OrderRootPtr->GetPrice());
	String symbol;
	String orderSymbol;
	String SideString;
	String IndexKey;
	if (SymbolPtr->IsMultileg())
	{
		symbol = SymbolPtr->GetSymbol(1);
		orderSymbol = GetOrderSymbol(exchangeCode, symbol, false);  //Check Symbol Index First
		SideString.printf(L"%1d", OrderRootPtr->GetLegSide1());
		IndexKey = orderSymbol + SideString + OrderPriceString;
		FOrderSymbolSidePriceIndex.insert(CStringTExecutionMultiMap::value_type(IndexKey, OrderRootPtr));

		symbol = SymbolPtr->GetSymbol(2);
		orderSymbol = GetOrderSymbol(exchangeCode, symbol, false);  //Check Symbol Index First
		SideString.printf(L"%1d", OrderRootPtr->GetLegSide2());
		IndexKey = orderSymbol + SideString + OrderPriceString;
		FOrderSymbolSidePriceIndex.insert(CStringTExecutionMultiMap::value_type(IndexKey, OrderRootPtr));
	}
	else
	{
		symbol = SymbolPtr->GetSymbol(0);
		orderSymbol = GetOrderSymbol(exchangeCode, symbol, false);  //Check Symbol Index First
		SideString.printf(L"%1d", OrderRootPtr->GetSide());
		IndexKey = orderSymbol + SideString + OrderPriceString;
		FOrderSymbolSidePriceIndex.insert(CStringTExecutionMultiMap::value_type(IndexKey, OrderRootPtr));
	}
	IsSuccess = true;
	return IsSuccess;
}  //TOrderStore::InsertOrderRootSymbolPriceIndex()

//---------------------------------------------------------------------------
bool TOrderStore::RemoveOrderRootSymbolSidePriceIndex(TExecution* OrderRootPtr)
{
	bool IsSuccess = false;
	if (OrderRootPtr == 0) return IsSuccess;

	TExchangeSymbol* SymbolPtr = OrderRootPtr->GetSymbol();
	if (SymbolPtr == 0) return IsSuccess;

	String OrderPriceString;
	OrderPriceString.printf(L"%20.5lf", OrderRootPtr->GetPrice());
	String SideString;
	String IndexKey;
	String OrderId = OrderRootPtr->GetOrderID();
	typedef CStringTExecutionMultiMap::iterator MI;
	TExecution* CurOrderRootPtr;
	UnicodeString CurOrderId;
	if (SymbolPtr->IsMultileg())
	{
		SideString.printf(L"%1d", OrderRootPtr->GetLegSide1());
		IndexKey = SymbolPtr->GetSymbol(1) + SideString + OrderPriceString;
		std::pair<MI, MI> g = FOrderSymbolSidePriceIndex.equal_range(IndexKey);
		for (MI p = g.first; p != g.second; p++)
		{
			CurOrderRootPtr = p->second;
			CurOrderId = CurOrderRootPtr->GetOrderID();
			if (CurOrderId.Length() <= 0) continue;
			IsSuccess = true;
			if (CurOrderId == OrderId)
			{
				FOrderSymbolSidePriceIndex.erase(p);
				break;
			}
		}  //for (MI p = g.first; p != g.second; p++)
		SideString.printf(L"%1d", OrderRootPtr->GetLegSide2());
		IndexKey = SymbolPtr->GetSymbol(2) + SideString + OrderPriceString;
		std::pair<MI, MI> h = FOrderSymbolSidePriceIndex.equal_range(IndexKey);
		for (MI p = h.first; p != h.second; p++)
		{
			CurOrderRootPtr = p->second;
			CurOrderId = CurOrderRootPtr->GetOrderID();
			if (CurOrderId.Length() <= 0) continue;
			if (CurOrderId == OrderId)
			{
				FOrderSymbolSidePriceIndex.erase(p);
				IsSuccess = true;
				break;
			}
		}  //for (MI p = h.first; p != h.second; p++)
	}
	else
	{
		SideString.printf(L"%1d", OrderRootPtr->GetSide());
		IndexKey = SymbolPtr->GetSymbol(0) + SideString + OrderPriceString;
		std::pair<MI, MI> g = FOrderSymbolSidePriceIndex.equal_range(IndexKey);
		for (MI p = g.first; p != g.second; p++)
		{
			CurOrderRootPtr = p->second;
			CurOrderId = CurOrderRootPtr->GetOrderID();
			if (CurOrderId.Length() <= 0) continue;
			if (CurOrderId == OrderId)
			{
				FOrderSymbolSidePriceIndex.erase(p);
				IsSuccess = true;
				break;
			}
		}  //for (MI p = g.first; p != g.second; p++)
	}

	return IsSuccess;
}  //TOrderStore::RemoveOrderRootSymbolSidePriceIndex()

//---------------------------------------------------------------------------
int TOrderStore::SumSymbolSidePriceLeavesQuantity(String OrderSymbol, nsOrderMessageDefine::SideEnum Side, double Price)
{
	int sumQuantity = 0;
	if (OrderSymbol.Length() <= 0) return sumQuantity;
	String SideString;
	SideString.printf(L"%1d", Side);
	String OrderPriceString;
	OrderPriceString.printf(L"%20.5lf", Price);
	String indexKey = OrderSymbol + SideString + OrderPriceString;
	typedef CStringTExecutionMultiMap::iterator MI;
	std::pair<MI, MI> g = FOrderSymbolSidePriceIndex.equal_range(indexKey);
	for (MI p = g.first; p != g.second; p++)
	{
		TExecution *curOrderRootPtr = p->second;
		nsOrderMessageDefine::OrderStatusEnum orderStatus = curOrderRootPtr->GetOrderStatus();
		int leavesQty = curOrderRootPtr->GetLeavesQty();
		if ((orderStatus == nsOrderMessageDefine::osNew) ||
			(orderStatus == nsOrderMessageDefine::osReplaced) ||
			(orderStatus == nsOrderMessageDefine::osPartiallyFilled))
		{
			if (leavesQty <= 0) continue;
			sumQuantity += leavesQty;
		}
	}  //for (MI p = g.first; p != g.second; p++)

	return sumQuantity;
}  //TOrderStore::SumSymbolPriceLeavesQuantity()

//---------------------------------------------------------------------------
void TOrderStore::ClearOrderRootData()
{
	TExecution* OrderRootPtr;
	for (int i = 0; i < FOrderRootList.ItemCount(); i++)
	{
		OrderRootPtr = FOrderRootList.GetItem(i);
		delete OrderRootPtr;
	}  //for (int i = 0; i < FOrderRootList.ItemCount(); i++)

	FOrderRootList.Clear();
	FOrderRootNIDIndex.clear();
	FOrderRootOrderIDIndex.clear();
	FOrderRootTimeOrderIDIndex.clear();
	FFilledOrderRootTimeOrderIDIndex.clear();
	FCanceledOrderRootTimeOrderIDIndex.clear();
	FActiveOrderRootTimeOrderIDIndex.clear();
	FOrderSymbolSidePriceIndex.clear();
}  //TOrderStore::ClearOrderRootData()
//---------------------------------------------------------------------------
Classes::TStringList* __fastcall TOrderStore::GetSymbols(void)
{
	Classes::TStringList* strList = new Classes::TStringList();
	CStringIntMap::iterator it;
	for (int i = 0; i < GetPosStatisticRecArrayCount(); i++)
	{
		s888::CPositionStatisticRecord *posStatisticRecPtr = GetPosStatisticRecFromArray(i);
		String curSymbol = posStatisticRecPtr->GetSymbol(s888::rwLockForRead);
		strList->Add(curSymbol);
	}
	return strList;
}  //TOrderStore::GetSymbols()
//---------------------------------------------------------------------------
void TOrderStore::OnMarketDataUpdate( MarketDataMessage* Msg )  //For Recover
{
	//s888::CTradingContract*          contractPtr = s888::gTradingObjsPool.FindContractFromSymbolIndex( exchangeCode, symbol, s888::rwLockForRead );
	nsOrderMessageDefine::MarketEnum Market = static_cast<nsOrderMessageDefine::MarketEnum>(Msg->GetMarket());
	String                           exchangeCode(  Msg->GetExchange().c_str() );
	String                           symbol( Msg->GetSymbol().c_str() );
	s888::CPositionStatisticRecord *posStatisticRecPtr = FindOrCreatePosStatisticRec( exchangeCode, symbol);

	if( posStatisticRecPtr != NULL )
	{
		int    msgTradeQty = Msg->GetTradeQty();
		double newestPrice = (msgTradeQty > 0)? Msg->GetTradePrice() : Msg->GetClosingPrice();

		posStatisticRecPtr->LockForWrite();
		if( !posStatisticRecPtr->IsMarketDataReceived(s888::rwNotLock) )
		{
			UFC::BufferedLog::Printf(" TOrderStore::OnMarketDataUpdate() for Recover, [%s-%s] First Market Data arrived.", Msg->GetExchange().c_str(), Msg->GetSymbol().c_str() );
			posStatisticRecPtr->SetMarketDataReceived(true, s888::rwNotLock);
		}
		posStatisticRecPtr->SetLastPrice(newestPrice, s888::rwNotLock);
		posStatisticRecPtr->IncreaseMarketTradeQuantitySum(msgTradeQty, s888::rwNotLock);
		posStatisticRecPtr->UnlockForWrite();
		ExecutePositionChangeListener(static_cast<nsOrderMessageDefine::MarketEnum>(Msg->GetMarket()), exchangeCode, symbol);
	}
	else
	{
		UFC::BufferedLog::Printf(" TOrderStore::OnMarketDataUpdate() for Recover, Insert [%s-%s] Position Statistic Record Failed", Msg->GetExchange().c_str(), Msg->GetSymbol().c_str() );
		UFC::BufferedLog::FlushToFile();
		return;
	}
}  //TOrderStore::OnMarketDataUpdate()
//---------------------------------------------------------------------------
void TOrderStore::OnMarketDataUpdate(BasicData* Msg) {}
void TOrderStore::OnMarketDataUpdate(TotalMatch* Msg){}
void TOrderStore::OnMarketDataUpdate(DayHighLowPrice* Msg) {}
void TOrderStore::OnMarketDataUpdate(OpeningInfo* Msg) {}
void TOrderStore::OnMarketDataUpdate(ClosingMarketData* Msg) {}
void TOrderStore::OnMarketDataUpdate(UnderlyingIndexInfo* Msg) {}
void TOrderStore::OnMarketDataUpdate(SumOfOrderInfo* Msg) {}
//---------------------------------------------------------------------------
int TOrderStore::ToNewHHMM( int oldHHMM, const String& OrigTime )
{
	AnsiString      timestr( OrigTime );
	UFC::UDateTime  StopOrderTime( timestr.c_str() );

	timestr.printf( "%04d", oldHHMM );
	UFC::UDateTime  KBarHHMM( timestr.c_str() );
	UFC::UDateTime  Now;

	KBarHHMM += ( Now - StopOrderTime );
	int NewHHMM = KBarHHMM.getHour()*100 + KBarHHMM.getMinute();

	return NewHHMM;
}
//---------------------------------------------------------------------------
void TOrderStore::ProcessTestExecRptForMatch( MatchInfo *MatchMsgPtr )
{
	nsOrderMessageDefine::MarketEnum Market = (nsOrderMessageDefine::MarketEnum)MatchMsgPtr->GetMarket();
	UFC::AnsiString ansiExchangeCode = MatchMsgPtr->GetExchange();
	String          exchangeCode(ansiExchangeCode.c_str());
	UFC::AnsiString ansiSymbol = MatchMsgPtr->GetSymbol();
	String          symbol( ansiSymbol.c_str() );

//	s888::CTradingContract *contractPtr = s888::gTradingObjsPool.FindContractFromSymbolIndex(exchangeCode, symbol, s888::rwLockForRead);
//	if (contractPtr !=  NULL )
//	{
		double matchPrice = MatchMsgPtr->GetMatchPrice();
		int matchQty = MatchMsgPtr->GetMatchQty();
		std::list<TExecutionReportMessage*>::iterator it;
		std::list<TExecutionReportMessage*>::iterator curIt;

		for (it = FTestExecRptList.begin(); it != FTestExecRptList.end();)
		{
			curIt = it++;
			TExecutionReportMessage *curRptPtr = *curIt;
			nsOrderMessageDefine::OrderStatusEnum curOrderStatus = curRptPtr->GetOrderStatus();
			if ((curOrderStatus != nsOrderMessageDefine::osNew) && (curOrderStatus != nsOrderMessageDefine::osPartiallyFilled))
				continue;
			UFC::AnsiString curAnsiExchangeCode = curRptPtr->GetExchangeCode();
			UFC::AnsiString curAnsiSymbol = curRptPtr->GetSymbol();
			if ((curAnsiExchangeCode == ansiExchangeCode) && (curAnsiSymbol == ansiSymbol))
			{
				int origLeavesQty = curRptPtr->GetLeavesQty();
				MatchOnFillChange( curRptPtr, matchQty, matchPrice );
				if ((curRptPtr->GetOrderStatus() == nsOrderMessageDefine::osFilled) ||
					((curRptPtr->GetOrderStatus() == nsOrderMessageDefine::osPartiallyFilled) &&
					 (origLeavesQty > curRptPtr->GetLeavesQty())))
				{
					double origOrderPrice = curRptPtr->GetPrice();
					UFC::AnsiString UDD( curRptPtr->GetAllUserData() );


					curRptPtr->SetPrice(curRptPtr->GetLastPx());
					FAdapter->OnExecutionReport(curRptPtr, edNewExecution);
					curRptPtr->SetPrice(origOrderPrice);
				}
				if (curRptPtr->GetOrderStatus() == nsOrderMessageDefine::osFilled)
				{
					FTestExecRptList.erase(curIt);
					delete curRptPtr;
				}
			}
		}
//	}
}
//---------------------------------------------------------------------------
void TOrderStore::OnMarketDataUpdate(MatchInfo* Msg)  //For Fill
{
	nsOrderMessageDefine::MarketEnum msgMarket = static_cast<nsOrderMessageDefine::MarketEnum>(Msg->GetMarket());
	String      exchangeCode( Msg->GetExchange().c_str() );
	String      symbol( Msg->GetSymbol().c_str() );
	String 		logHead( L"TOrderStore::OnMarketDataUpdate(For Fill)" );
	String		logMsg;
	double      msgMatchPrice = Msg->GetMatchPrice();
	int         msgTradeQty   = Msg->GetMatchQty();
	s888::CPositionStatisticRecord *posStatisticRecPtr;

	if( FIsTestMode && FIsTestMatchPrice )
		ProcessTestExecRptForMatch( Msg );

	if( (posStatisticRecPtr = FindOrCreatePosStatisticRec(exchangeCode, symbol)) != NULL )  ///< Position exists
	{
		posStatisticRecPtr->LockForWrite();
		if( posStatisticRecPtr->IsMarketDataReceived(s888::rwNotLock) == false )
		{
			posStatisticRecPtr->SetMarketDataReceived( true, s888::rwNotLock );
			logMsg.printf(L"First Market Data[%s-%s] arrived.", exchangeCode, symbol );
			s888::WriteUFCLog(logHead, logMsg, s888::constFlush);
		}
		double origLastPrice = posStatisticRecPtr->GetLastPrice(s888::rwNotLock);
		DWORD  lastTickCount = posStatisticRecPtr->GetLastTickCount(s888::rwNotLock);
		DWORD  msgTickCount  = GetTickCount();  //Retrieves the number of milliseconds that have elapsed since the system was started.
		posStatisticRecPtr->UnlockForWrite();

		///< Process stop order.
		if( FAdapter != NULL  && FAdapter->IsLogon() && msgMatchPrice != origLastPrice && msgTradeQty > 0 )
			ProcessStopOrder( exchangeCode, symbol, msgMatchPrice );  //For Stop Order
		///< Need realtime process
		int TimeDiff = (int)(msgTickCount - lastTickCount);

		if( TimeDiff >= FMarketDataTimerInterval )
		{  	///< Update position record
			posStatisticRecPtr->LockForWrite();
			posStatisticRecPtr->SetLastPrice( msgMatchPrice, s888::rwNotLock);
			posStatisticRecPtr->IncreaseMarketTradeQuantitySum( msgTradeQty, s888::rwNotLock);
			posStatisticRecPtr->SetLastTickCount( msgTickCount, s888::rwNotLock);
			posStatisticRecPtr->UnlockForWrite();
			ExecutePositionChangeListener( msgMarket, exchangeCode, symbol );
		}
		else ///< Update later.
		{
			posStatisticRecPtr->LockForWrite();
			posStatisticRecPtr->SetLastPrice( msgMatchPrice, s888::rwNotLock);
			posStatisticRecPtr->IncreaseMarketTradeQuantitySum( msgTradeQty, s888::rwNotLock);
			posStatisticRecPtr->UnlockForWrite();
			PushPosStatisticRecToWaitingQueue( posStatisticRecPtr );
			if(!FMarketDataTimer->Enabled )
				FMarketDataTimer->Enabled = true;
		}
	}
	else  ///< Position Not exists
	{
		logMsg.printf(L"Find or Create Position Statistic Record [%s-%s] Failed.", exchangeCode, symbol );
		s888::WriteUFCLog( logHead, logMsg, s888::constFlush );
		return;
	}
}  //TOrderStore::OnMarketDataUpdate()
//---------------------------------------------------------------------------
void __fastcall TOrderStore::OnMarketDataTimer(TObject *Sender)
{
	int   waitingCount = GetPosStatisticRecWaitingQueueCount();
	DWORD curTickCount = GetTickCount();

	FMarketDataTimer->Enabled = false;
	for( int i = 0; i < waitingCount; i++)
	{
		s888::CPositionStatisticRecord *posStatisticRecPtr = GetFirstPosStatisticRecOfWaitingQueue();
		PopPosStatisticRecFromWaitingQueue();
		if( posStatisticRecPtr != NULL )
		{
			posStatisticRecPtr->LockForRead();
			String exchangeCode = posStatisticRecPtr->GetExchangeCode(s888::rwNotLock);
			String symbol       = posStatisticRecPtr->GetSymbol(s888::rwNotLock);
			nsOrderMessageDefine::MarketEnum market = posStatisticRecPtr->GetMarket(s888::rwNotLock);
			posStatisticRecPtr->SetLastTickCount( curTickCount, s888::rwNotLock);
			posStatisticRecPtr->UnlockForRead();

			ExecutePositionChangeListener( market, exchangeCode, symbol);
		}
	}
}
//---------------------------------------------------------------------------
int TOrderStore::SubscribeSymbol(const String& ExchangeCode, const String& Symbol, bool SubscribeAnyway)
{
	if ((FMarketDataStore == 0) ||
		((ExchangeCode.Length() <= 0) || (Symbol.Length() <= 0))) return -1;

	UFC::AnsiString ansiExchangeCode = s888::ConvertUnicodeToUFCAnsiStr(ExchangeCode);
	s888::CTradingContract *contractPtr = s888::gTradingObjsPool.FindContractFromSymbolIndex(ExchangeCode, Symbol, s888::rwLockForRead);
	if (contractPtr == 0)
		contractPtr = s888::gTradingObjsPool.FindContractFromTradeSymbolIndex(ExchangeCode, Symbol, s888::rwLockForRead);
	String subscribeSymbol = Symbol;
	if (contractPtr != 0) subscribeSymbol = contractPtr->GetSymbol(s888::rwLockForRead);
	UFC::AnsiString ansiSubscribeSymbol = s888::ConvertUnicodeToUFCAnsiStr(subscribeSymbol);

	int subscribeCount = 0;
	s888::CPositionStatisticRecord *posStatisticRecPtr = FindOrCreatePosStatisticRec(ExchangeCode, subscribeSymbol);
	if (posStatisticRecPtr != 0)
	{
		posStatisticRecPtr->LockForWrite();
		subscribeCount = posStatisticRecPtr->IncreaseSubscribeCount(1, s888::rwNotLock);
		if (subscribeCount <= 1) posStatisticRecPtr->SetMarketDataReceived(false, s888::rwNotLock);
		posStatisticRecPtr->UnlockForWrite();
	}

	if (SubscribeAnyway || (subscribeCount <= 1))
	{
		FMarketDataStore->Unsubscribe(ansiExchangeCode, ansiSubscribeSymbol, this);
		FMarketDataStore->Subscribe(ansiExchangeCode, ansiSubscribeSymbol, this, stSnapshotWithUpdate );//67 );
	}
	return subscribeCount;
}  //TOrderStore::SubscribeSymbol()
//---------------------------------------------------------------------------
int TOrderStore::SubscribeSymbol(const String& ExchangeCode, const String& Symbol, bool needIncreaseCount, bool SubscribeAnyway)
{
	if ((FMarketDataStore == 0) ||
		((ExchangeCode.Length() <= 0) || (Symbol.Length() <= 0))) return -1;
	int subscribeCount = 0;
	s888::CPositionStatisticRecord *posStatisticRecPtr = FindOrCreatePosStatisticRec(ExchangeCode, Symbol);
	if (posStatisticRecPtr == 0) return subscribeCount;
	posStatisticRecPtr->LockForWrite();
	bool isMarketDataReceived = posStatisticRecPtr->IsMarketDataReceived(s888::rwNotLock);
	subscribeCount = posStatisticRecPtr->GetSubscribeCount(s888::rwNotLock);
	if ((subscribeCount == 0) || needIncreaseCount)
		subscribeCount = posStatisticRecPtr->IncreaseSubscribeCount(1, s888::rwNotLock);
	String subscribeSymbol = posStatisticRecPtr->GetSymbol(s888::rwNotLock);
	posStatisticRecPtr->UnlockForWrite();
	UFC::AnsiString ansiExchangeCode = s888::ConvertUnicodeToUFCAnsiStr(ExchangeCode);
	UFC::AnsiString ansiSubscribeSymbol = s888::ConvertUnicodeToUFCAnsiStr(subscribeSymbol);
	if( /*FIsContractDownloadCompleted &&*/ (SubscribeAnyway || !isMarketDataReceived || (subscribeCount <= 1)))
	{
		FMarketDataStore->Unsubscribe(ansiExchangeCode, ansiSubscribeSymbol, this);
		FMarketDataStore->Subscribe(ansiExchangeCode, ansiSubscribeSymbol, this, stSnapshotWithUpdate );
		UFC::BufferedLog::Printf(" TOrderStore::SubscribeSymbol() Subscribe[%s-%s], Count=%04d.", ansiExchangeCode.c_str(), ansiSubscribeSymbol.c_str(), subscribeCount);
	}
	else
		UFC::BufferedLog::Printf(" TOrderStore::SubscribeSymbol() [%s-%s], Count=%04d.", ansiExchangeCode.c_str(), ansiSubscribeSymbol.c_str(), subscribeCount);
	UFC::BufferedLog::FlushToFile();
	return subscribeCount;
}  //TOrderStore::SubscribeSymbol()
//---------------------------------------------------------------------------
int TOrderStore::SubscribeAllPosStatisticRecSymbol(bool needIncreaseCount, bool SubscribeAnyway)
{
	if (FMarketDataStore == 0) return -1;
	int subscribeCount = 0;
	for (int i = 0; i < GetPosStatisticRecArrayCount(); i++)
	{
		s888::CPositionStatisticRecord *posStatisticRecPtr = GetPosStatisticRecFromArray(i);
		int curSubscribeCount = 0;
		bool needSubscribe = false;
		posStatisticRecPtr->LockForWrite();
		String curExchangeCode = posStatisticRecPtr->GetExchangeCode(s888::rwNotLock);
		String curSymbol = posStatisticRecPtr->GetSymbol(s888::rwNotLock);
		String curContractCode = curSymbol.SubString(1,3);
		s888::CTaxRateObject *taxObjPtr = s888::gTradingObjsPool.FindTaxRateFromCodeIndex(curExchangeCode, curContractCode, s888::rwLockForRead);
		s888::CFeeObject *feeObjPtr = s888::gTradingObjsPool.FindFeeObjFromLogonIdContractCodeIndex(FID, curExchangeCode, curContractCode, s888::rwLockForRead);

		posStatisticRecPtr->SetContractMultiplier( FMarketDataStore->ProductContractMultiplier( curExchangeCode, curContractCode ), s888::rwNotLock);
		if (taxObjPtr != NULL ) posStatisticRecPtr->SetTaxRateObject(taxObjPtr, s888::rwNotLock);
		if (feeObjPtr != NULL ) posStatisticRecPtr->SetFeeObject(feeObjPtr, s888::rwNotLock);

		posStatisticRecPtr->ReOffsetPosition(s888::rwNotLock);
		posStatisticRecPtr->DumpData(s888::constFlush, s888::rwNotLock);
		bool curIsMarketDataReceived = posStatisticRecPtr->IsMarketDataReceived(s888::rwNotLock);
		curSubscribeCount = posStatisticRecPtr->GetSubscribeCount(s888::rwNotLock);
		if (SubscribeAnyway || (curSubscribeCount == 0) ||
			((curSubscribeCount > 0) && !curIsMarketDataReceived))
		{
			needSubscribe = true;
			if (needIncreaseCount || (curSubscribeCount == 0))
				curSubscribeCount = posStatisticRecPtr->IncreaseSubscribeCount(1, s888::rwNotLock);
		}
		posStatisticRecPtr->UnlockForWrite();
		UFC::AnsiString ansiExchangeCode = s888::ConvertUnicodeToUFCAnsiStr(curExchangeCode);
		UFC::AnsiString ansiSubscribeSymbol = s888::ConvertUnicodeToUFCAnsiStr(curSymbol);
		if (needSubscribe)
		{
			FMarketDataStore->Unsubscribe(ansiExchangeCode, ansiSubscribeSymbol, this);
			FMarketDataStore->Subscribe(ansiExchangeCode, ansiSubscribeSymbol, this, stSnapshotWithUpdate );
			subscribeCount++;
			UFC::BufferedLog::Printf(" TOrderStore::SubscribeAllPosStatisticRecSymbol() Subscribe[%s-%s], Count=%04d.", ansiExchangeCode.c_str(), ansiSubscribeSymbol.c_str(), curSubscribeCount);
		}
		else
			UFC::BufferedLog::Printf(" TOrderStore::SubscribeAllPosStatisticRecSymbol() [%s-%s], Count=%04d.", ansiExchangeCode.c_str(), ansiSubscribeSymbol.c_str(), curSubscribeCount);
	}  //for (int i = 0; i < GetPosStatisticRecArrayCount(); i++)
	UFC::BufferedLog::Printf(" TOrderStore::SubscribeAllPosStatisticRecSymbol() Subscribe %04d Symbol.", subscribeCount);
	UFC::BufferedLog::FlushToFile();
	return subscribeCount;
}  //TOrderStore::SubscribeAllPosStatisticRecSymbol()
//---------------------------------------------------------------------------
int TOrderStore::UnsubscribeSymbol(const String& ExchangeCode, const String& Symbol, bool UnsubscribeAnyway)
{
	if ((FMarketDataStore == 0) ||
		((ExchangeCode.Length() <= 0) || (Symbol.Length() <= 0))) return -1;
	int subscribeCount = 0;
	s888::CPositionStatisticRecord *posStatisticRecPtr = FindOrCreatePosStatisticRec(ExchangeCode, Symbol);
	if (posStatisticRecPtr == 0) return subscribeCount;
	posStatisticRecPtr->LockForWrite();
	if (UnsubscribeAnyway)
		posStatisticRecPtr->SetSubscribeCount(subscribeCount, s888::rwNotLock);
	else
		subscribeCount = posStatisticRecPtr->IncreaseSubscribeCount(-1, s888::rwNotLock);
	String subscribeSymbol = posStatisticRecPtr->GetSymbol(s888::rwNotLock);
	posStatisticRecPtr->UnlockForWrite();
	UFC::AnsiString ansiExchangeCode = s888::ConvertUnicodeToUFCAnsiStr(ExchangeCode);
	UFC::AnsiString ansiSubscribeSymbol = s888::ConvertUnicodeToUFCAnsiStr(subscribeSymbol);
	if (UnsubscribeAnyway || (subscribeCount <= 0))
	{
		FMarketDataStore->Unsubscribe(ansiExchangeCode, ansiSubscribeSymbol, this);
		UFC::BufferedLog::Printf(" TOrderStore::UnsubscribeSymbol() Unsubscribe[%s-%s], Count=%04d.", ansiExchangeCode.c_str(), ansiSubscribeSymbol.c_str(), subscribeCount);
	}
	else
		UFC::BufferedLog::Printf(" TOrderStore::UnsubscribeSymbol() [%s-%s], Count=%04d.", ansiExchangeCode.c_str(), ansiSubscribeSymbol.c_str(), subscribeCount);
	return subscribeCount;
}  //TOrderStore::UnsubscribeSymbol()

/*//---------------------------------------------------------------------------
bool TOrderStore::InsertStopOrderTimeNIDIndex( TExecution* StopOrderPtr )
{
	bool IsSuccess = false;
	String TimeNIDKey;

	if( StopOrderPtr == 0)
		return IsSuccess;
	TimeNIDKey.printf( L"%s%lld", StopOrderPtr->GetLocalMessageTime(), StopOrderPtr->GetNID());
	if( FindStopOrderByTimeNID( TimeNIDKey ) == 0 )
	{
		FStopOrderTimeNIDIndex.insert( CStringTExecutionMap::value_type( TimeNIDKey, StopOrderPtr ));
		IsSuccess = true;
	}
	else
		IsSuccess = false;
	return IsSuccess;
}*/
//---------------------------------------------------------------------------
/*bool TOrderStore::RemoveStopOrderByTimeNID(UnicodeString TimeNIDKey)
{
	bool IsSuccess = false;
	if (TimeNIDKey.Length() <= 0) return IsSuccess;

	CStringTExecutionMap::iterator it = FStopOrderTimeNIDIndex.find(TimeNIDKey);
	if (it != FStopOrderTimeNIDIndex.end())
	{
		FStopOrderTimeNIDIndex.erase(it);
		IsSuccess = true;
	}

	return IsSuccess;
} */
//---------------------------------------------------------------------------
void TOrderStore::AddOrderMessage(TExecution* OrderMessagePtr)
{
   FOrderMessageList.Add(OrderMessagePtr);
}  //TOrderStore::AddOrderRoot()

//---------------------------------------------------------------------------
TExecution* TOrderStore::FindOrderMessageByExecID(UnicodeString ExecID)
{
	TExecution* ExecutionPtr = 0;
	if ((FOrderMessageExecIDIndex.size() <= 0) || (ExecID.Length() <= 0)) return ExecutionPtr;
	CStringTExecutionMap::iterator it = FOrderMessageExecIDIndex.find(ExecID);
	if (it != FOrderMessageExecIDIndex.end()) ExecutionPtr = it->second;

	return ExecutionPtr;
}  //TOrderStore::FindOrderMessageByExecID()

//---------------------------------------------------------------------------
bool TOrderStore::InsertOrderMessageExecIDIndex(TExecution* OrderMessagePtr, bool NeedCheckExist)
{
	bool IsSuccess = false;
	if (OrderMessagePtr == 0) return IsSuccess;
	UnicodeString IndexKey = OrderMessagePtr->GetExecID();
	if (IndexKey.Length() <= 0) return IsSuccess;
	if ((NeedCheckExist) && (FindOrderMessageByExecID(IndexKey) != 0)) return IsSuccess;

	std::pair<CStringTExecutionMap::iterator, bool> InsertPair;
	InsertPair = FOrderMessageExecIDIndex.insert(CStringTExecutionMap::value_type(IndexKey, OrderMessagePtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TOrderStore::InsertOrderMessageExecIDIndex()

//---------------------------------------------------------------------------
bool TOrderStore::InsertFillMessageSymbolIndex(TExecution* FillMessagePtr)
{
	bool IsSuccess = false;
	if (FillMessagePtr == 0) return IsSuccess;
	if ((FillMessagePtr->GetOrderStatus() != nsOrderMessageDefine::osPartiallyFilled) &&
		(FillMessagePtr->GetOrderStatus() != nsOrderMessageDefine::osFilled))
		return IsSuccess;

	UnicodeString IndexKey;
	if (FillMessagePtr->GetSymbol()->IsMultileg())
	{
		IndexKey = FillMessagePtr->GetSymbol()->GetSymbol(1);
		FFillMessageSymbolIndex.insert(CStringTExecutionMultiMap::value_type(IndexKey, FillMessagePtr));

		IndexKey = FillMessagePtr->GetSymbol()->GetSymbol(2);
		FFillMessageSymbolIndex.insert(CStringTExecutionMultiMap::value_type(IndexKey, FillMessagePtr));
	}
	else
	{
		IndexKey = FillMessagePtr->GetSymbol()->GetSymbol(0);
		FFillMessageSymbolIndex.insert(CStringTExecutionMultiMap::value_type(IndexKey, FillMessagePtr));
	}

	IsSuccess = true;
	return IsSuccess;
}  //TOrderStore::InsertFillMessageSymbolIndex()
//------------------------------------------------------------------------------
CStringTExecutionMap* TOrderStore::GetOrderRootTimeOrderIDIndex()         {return &FOrderRootTimeOrderIDIndex;};
CStringTExecutionMap* TOrderStore::GetFilledOrderRootTimeOrderIDIndex()   {return &FFilledOrderRootTimeOrderIDIndex;};
CStringTExecutionMap* TOrderStore::GetCanceledOrderRootTimeOrderIDIndex() {return &FCanceledOrderRootTimeOrderIDIndex;};
CStringTExecutionMap* TOrderStore::GetActiveOrderRootTimeOrderIDIndex()   {return &FActiveOrderRootTimeOrderIDIndex;};
CStringTExecutionMap* TOrderStore::GetStopOrderTimeNIDIndex()             {return &FStopOrderTimeNIDIndex;};
//------------------------------------------------------------------------------
void TOrderStore::GetTaxAndFeeRate(const String& ExchangeCode,
								   const String& Symbol,
								   const String& Account,
								   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
								   double& TaxRate, double& FeeRateByVolume, double& FeeRateByPrice)
{
	TaxRate = 0.0;
	FeeRateByVolume = 0.0;
	FeeRateByPrice = 0.0;
	String targetExchangeCode = s888::gTradingObjsPool.ConvertIfBorkerExchangeCode( ExchangeCode, s888::rwLockForRead);
	s888::CTradingContract *contractPtr = s888::gTradingObjsPool.FindContractFromSymbolIndex(targetExchangeCode, Symbol, s888::rwLockForRead);
	if (contractPtr == 0)
		contractPtr = s888::gTradingObjsPool.FindContractFromTradeSymbolIndex(targetExchangeCode, Symbol, s888::rwLockForRead);

	s888::CTradingCommodity *commodityPtr = 0;
	String contractCode = L"";
	if (contractPtr != 0) commodityPtr = contractPtr->GetCommodity(s888::rwLockForRead);
	if (commodityPtr == 0)
	{
		BasicInformation *infoPtr = GetCommodityBasicInformation(ExchangeCode, Symbol);
		if (infoPtr != 0)
		{
			UFC::AnsiString ansiContractCode = infoPtr->GetProductID();
			contractCode = UTF8ToUnicodeString(ansiContractCode.c_str());
			commodityPtr = s888::gTradingObjsPool.FindCommodityFromCodeIndex(targetExchangeCode, contractCode, s888::rwLockForRead);
		}
	}  //if (commodityPtr == 0)

	if (commodityPtr != 0)
	{
		commodityPtr->LockForRead();
		TaxRate = commodityPtr->GetTaxRate(s888::rwNotLock);
		if (contractCode.Length() <= 0) contractCode = commodityPtr->GetContractCode(s888::rwNotLock);
		commodityPtr->UnlockForRead();
	}
	s888::CTaxRateObject *taxObjPtr = s888::gTradingObjsPool.FindTaxRateFromCodeIndex(targetExchangeCode, contractCode, s888::rwLockForRead);
	if( taxObjPtr != NULL )
		TaxRate = taxObjPtr->GetTaxRate( s888::rwLockForRead );
	s888::CFeeObject *feeObjPtr = 0;
	if (feeObjPtr == 0)
		s888::CFeeObject *feeObjPtr = s888::gTradingObjsPool.FindFeeObjFromLogonIdContractCodeIndex(Account, targetExchangeCode, contractCode, s888::rwLockForRead);
	if (feeObjPtr != 0)
	{
		feeObjPtr->LockForRead();
		switch (PositionEffect)
		{
			case nsOrderMessageDefine::peOpen:
				FeeRateByVolume = feeObjPtr->GetOpenRateByVolume(s888::rwNotLock);
				FeeRateByPrice = feeObjPtr->GetOpenRateByMoney(s888::rwNotLock);
				break;
			case nsOrderMessageDefine::peClose:
				FeeRateByVolume = feeObjPtr->GetCloseRateByVolume(s888::rwNotLock);
				FeeRateByPrice = feeObjPtr->GetCloseRateByMoney(s888::rwNotLock);
				break;
			case nsOrderMessageDefine::peDayTrade:
				FeeRateByVolume = feeObjPtr->GetCloseTodayRateByVolume(s888::rwNotLock);
				FeeRateByPrice = feeObjPtr->GetCloseTodayRateByMoney(s888::rwNotLock);
				break;
			default:
				FeeRateByVolume = 0.0;
				FeeRateByPrice = 0.0;
		}  //switch (PositionEffect)
		feeObjPtr->UnlockForRead();
	}  //if (feeObjPtr != 0)
}  //TOrderStore::GetTaxAndFeeRate()
//------------------------------------------------------------------------------
int TOrderStore::GetPosStatisticRecTradeSymbolIndexCount(s888::ReadWrietLockTypeEnum RWLockType )
{
	return s888::GetObjectContainerCount(FPosStatisticRecTradeSymbolIndex, &FPosStatisticRecCSLock, RWLockType);
};
//------------------------------------------------------------------------------
s888::CPositionStatisticRecord *TOrderStore::FindPosStatisticRecByTradeSymbol(const String& ExchangeCode, const String& TradeSymbol, s888::ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = s888::CPositionStatisticRecord::BuildKey(ExchangeCode, TradeSymbol);
	return static_cast<s888::CPositionStatisticRecord*>(s888::FindTradingObjFromMap(FPosStatisticRecTradeSymbolIndex, keyValue, &FPosStatisticRecCSLock, RWLockType));
}  //TOrderStore::FindPosStatisticRecByTradeSymbol()
//------------------------------------------------------------------------------
bool TOrderStore::InsertPosStatisticRecToTradeSymbolIndex(const String& ExchangeCode, const String& TradeSymbol,
														  s888::CPositionStatisticRecord *PosStatisticRecPtr,
														  bool DoCheckExist, s888::ReadWrietLockTypeEnum RWLockType)
{
	String keyValue = s888::CPositionStatisticRecord::BuildKey(ExchangeCode, TradeSymbol);
	return s888::InsertTradingObjIntoMap(FPosStatisticRecTradeSymbolIndex, keyValue, PosStatisticRecPtr, DoCheckExist, &FPosStatisticRecCSLock, RWLockType);
}  //TOrderStore::InsertPosStatisticRecToTradeSymbolIndex()
//------------------------------------------------------------------------------
int TOrderStore::ClearPosStatisticRecTradeSymbolIndex(bool DoDelete, s888::ReadWrietLockTypeEnum RWLockType)
{
	return s888::ClearTradingObjMap(FPosStatisticRecTradeSymbolIndex, DoDelete, &FPosStatisticRecCSLock, RWLockType);
}  //TOrderStore::ClearPosStatisticRecTradeSymbolIndex()
//------------------------------------------------------------------------------
int TOrderStore::GetPosStatisticRecArrayCount(s888::ReadWrietLockTypeEnum RWLockType )
{
	return s888::GetObjectContainerCount(FPosStatisticRecArray, &FPosStatisticRecCSLock, RWLockType);
}
//------------------------------------------------------------------------------
s888::CPositionStatisticRecord *TOrderStore::GetPosStatisticRecFromArray(int ArrayIndex, s888::ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<s888::CPositionStatisticRecord*>(s888::GetTradingObjFromVector(FPosStatisticRecArray, ArrayIndex, &FPosStatisticRecCSLock, RWLockType));
}  //TOrderStore::GetPosStatisticRecFromArray()
//------------------------------------------------------------------------------
bool TOrderStore::AppendPosStatisticRecToArray(s888::CPositionStatisticRecord *PosStatisticRecPtr, s888::ReadWrietLockTypeEnum RWLockType)
{
	return s888::AppendTradingObjToVector(FPosStatisticRecArray, PosStatisticRecPtr, &FPosStatisticRecCSLock, RWLockType);
}  //TOrderStore::AppendPosStatisticRecToArray()
//------------------------------------------------------------------------------
int TOrderStore::ClearPosStatisticRecArray(bool DoDelete, s888::ReadWrietLockTypeEnum RWLockType)
{
	return s888::ClearTradingObjVector(FPosStatisticRecArray, DoDelete, &FPosStatisticRecCSLock, RWLockType);
}  //TOrderStore::ClearPosStatisticRecArray()
//------------------------------------------------------------------------------
int TOrderStore::GetPosStatisticRecWaitingQueueCount(s888::ReadWrietLockTypeEnum RWLockType )
{
	return s888::GetObjectContainerCount(FPosStatisticRecWaitingQueue, &FPosStatisticRecCSLock, RWLockType);
}
//------------------------------------------------------------------------------
s888::CPositionStatisticRecord *TOrderStore::GetPosStatisticRecFromWaitingQueue(int QueueIndex, s888::ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<s888::CPositionStatisticRecord*>(s888::GetTradingObjFromQueue(FPosStatisticRecWaitingQueue, QueueIndex, &FPosStatisticRecCSLock, RWLockType));
}  //TOrderStore::GetPosStatisticRecFromWaitingQueue()
//------------------------------------------------------------------------------
void TOrderStore::PushPosStatisticRecToWaitingQueue(s888::CPositionStatisticRecord *PosStatisticRecPtr, s888::ReadWrietLockTypeEnum RWLockType)
{
	PushTradingObjToQueueBack(FPosStatisticRecWaitingQueue, PosStatisticRecPtr, &FPosStatisticRecCSLock, RWLockType);
}  //TOrderStore::PushPosStatisticRecToWaitingQueue()
//------------------------------------------------------------------------------
s888::CPositionStatisticRecord *TOrderStore::GetFirstPosStatisticRecOfWaitingQueue(s888::ReadWrietLockTypeEnum RWLockType)
{
	return static_cast<s888::CPositionStatisticRecord*>(s888::GetFirstTradingObjOfQueue(FPosStatisticRecWaitingQueue, &FPosStatisticRecCSLock, RWLockType));
}  //TOrderStore::GetPosStatisticRecOfWaitingQueue()
//------------------------------------------------------------------------------
void TOrderStore::PopPosStatisticRecFromWaitingQueue(s888::ReadWrietLockTypeEnum RWLockType)
{
	s888::PopFirstTradingObjFromQueue(FPosStatisticRecWaitingQueue, &FPosStatisticRecCSLock, RWLockType);
}  //TOrderStore::PopPosStatisticRecFromWaitingQueue()
//------------------------------------------------------------------------------
int TOrderStore::ClearPosStatisticRecWaitingQueue(s888::ReadWrietLockTypeEnum RWLockType)
{
	return s888::ClearTradingObjQueue(FPosStatisticRecWaitingQueue, s888::constNotDelete, &FPosStatisticRecCSLock, RWLockType);
}  //TOrderStore::ClearPosStatisticRecWaitingQueue()
//---------------------------------------------------------------------------
int TOrderStore::SetPosStatisticRecFeeObj(s888::CFeeObject *FeeObjPtr)
{
	int setCount = 0;
	FeeObjPtr->LockForRead();
	String feeExchangeCode = FeeObjPtr->GetExchangeCode(s888::rwNotLock);
	String feeContractCode = FeeObjPtr->GetContractCode(s888::rwNotLock);
	FeeObjPtr->UnlockForRead();
	FPosStatisticRecCSLock.Acquire();
	int posStatisticRecCount = GetPosStatisticRecArrayCount(s888::rwNotLock);
	for (int recIndex = 0; recIndex < posStatisticRecCount; recIndex++)
	{
		s888::CPositionStatisticRecord *posStatisticRecPtr = GetPosStatisticRecFromArray(recIndex, s888::rwNotLock);
		posStatisticRecPtr->LockForRead();
		String curExchangeCode = posStatisticRecPtr->GetExchangeCode(s888::rwNotLock);
		String curSymbol = posStatisticRecPtr->GetSymbol(s888::rwNotLock);
		String curTradeSymbol = posStatisticRecPtr->GetTradeSymbol(s888::rwNotLock);
		s888::CTradingCommodity *curCommodityPtr = posStatisticRecPtr->GetCommodityObject(s888::rwNotLock);
		s888::CTradingContract *curContractPtr = posStatisticRecPtr->GetContractObject(s888::rwNotLock);
		s888::CTaxRateObject *curTaxRateObjPtr = posStatisticRecPtr->GetTaxRateObject(s888::rwNotLock);
		posStatisticRecPtr->UnlockForRead();
		if (feeExchangeCode != curExchangeCode) continue;

		double curContractMultiplier = 1.0;
		if (curContractPtr == 0)
		{
			curContractPtr = s888::gTradingObjsPool.FindContractFromTradeSymbolIndex(curExchangeCode, curTradeSymbol, s888::rwLockForRead);
			if (curContractPtr == 0) curContractPtr = s888::gTradingObjsPool.FindContractFromSymbolIndex(curExchangeCode, curSymbol, s888::rwLockForRead);
		}

		if (curContractPtr != 0)
		{
			curContractPtr->LockForRead();
			curContractMultiplier = curContractPtr->GetContractMultiplier(s888::rwNotLock);
			if (curCommodityPtr == 0) curCommodityPtr = curContractPtr->GetCommodity(s888::rwNotLock);
			curContractPtr->UnlockForRead();
		}

		String curContractCode = L"";
		if (curCommodityPtr != 0)
			curContractCode = curCommodityPtr->GetContractCode(s888::rwLockForRead);
		if ((curContractCode.Length() <= 0) || (curContractCode != feeContractCode)) continue;

		if (curTaxRateObjPtr == 0)
			curTaxRateObjPtr = s888::gTradingObjsPool.FindTaxRateFromCodeIndex(curExchangeCode, curContractCode, s888::rwLockForRead);

		posStatisticRecPtr->LockForWrite();
		posStatisticRecPtr->SetContractMultiplier(curContractMultiplier, s888::rwNotLock);
		posStatisticRecPtr->SetCommodityObject(curCommodityPtr, s888::rwNotLock);
		posStatisticRecPtr->SetContractObject(curContractPtr, s888::rwNotLock);
		posStatisticRecPtr->SetFeeObject(FeeObjPtr, s888::rwNotLock);
		posStatisticRecPtr->SetTaxRateObject(curTaxRateObjPtr, s888::rwNotLock);
		posStatisticRecPtr->ReOffsetPosition(s888::rwNotLock);
		posStatisticRecPtr->DumpData(s888::constFlush, s888::rwNotLock);
		posStatisticRecPtr->UnlockForWrite();
		SubscribeSymbol(curExchangeCode, curSymbol, false, true);  //need not Increase Count, SubscribeAnyway
		setCount++;
	}  //for (int recIndex = 0; recIndex < posStatisticRecCount; recIndex++)
	FPosStatisticRecCSLock.Release();
	s888::FlushUFCLog();
	return setCount;
}  //TOrderStore::SetPosStatisticRecFeeObj()
//---------------------------------------------------------------------------
s888::CPositionStatisticRecord *TOrderStore::FindOrCreatePosStatisticRec(const String& ExchangeCode, const String& Symbol)
{
	String logHead = L"TOrderStore::FindOrCreatePosStatisticRec()";
	String logMsg = L"";
	String orderSymbol = Symbol;
	String tradeSymbol = Symbol;
	s888::CPositionStatisticRecord* positionStatisticRecPtr = FindPosStatisticRecByTradeSymbol(ExchangeCode, tradeSymbol, s888::rwLockForRead);
	s888::CTaxRateObject*    taxRateObjPtr = NULL;
	s888::CFeeObject*        feeObjPtr = NULL;
	String contractCode = Symbol.SubString(1, 3);
	double contractMultiplier = FMarketDataStore->ProductContractMultiplier( ExchangeCode, contractCode );

	if( positionStatisticRecPtr != NULL)
	{
		positionStatisticRecPtr->LockForRead();
		taxRateObjPtr = positionStatisticRecPtr->GetTaxRateObject(s888::rwNotLock);
		feeObjPtr = positionStatisticRecPtr->GetFeeObject(s888::rwNotLock);
		positionStatisticRecPtr->UnlockForRead();
	}
	else
	{
		FPosStatisticRecCSLock.Acquire();
		positionStatisticRecPtr = new s888::CPositionStatisticRecord( ExchangeCode, orderSymbol, tradeSymbol );
		if( InsertPosStatisticRecToTradeSymbolIndex(ExchangeCode, tradeSymbol, positionStatisticRecPtr, s888::constNotCheckExist, s888::rwNotLock) )
			AppendPosStatisticRecToArray(positionStatisticRecPtr, s888::rwNotLock);
		else
		{
			logMsg.printf(L"Insert Position Statistic Record[%s-%s] into Trade Symbol[%s] Index Failed.", ExchangeCode, orderSymbol, tradeSymbol);
			s888::WriteUFCLog(logHead, logMsg, s888::constFlush);
			delete positionStatisticRecPtr;
			positionStatisticRecPtr = 0;
		}
		FPosStatisticRecCSLock.Release();
	}
	if( taxRateObjPtr == NULL )
		taxRateObjPtr = s888::gTradingObjsPool.FindTaxRateFromCodeIndex(ExchangeCode, contractCode, s888::rwLockForRead);
	if( feeObjPtr == NULL )
		feeObjPtr = s888::gTradingObjsPool.FindFeeObjFromLogonIdContractCodeIndex(FID, ExchangeCode, contractCode, s888::rwLockForRead);

	if( positionStatisticRecPtr != NULL )
	{
		positionStatisticRecPtr->LockForWrite();
		if ((positionStatisticRecPtr->GetTaxRateObject(s888::rwNotLock) == 0) && (taxRateObjPtr != 0))
			positionStatisticRecPtr->SetTaxRateObject(taxRateObjPtr, s888::rwNotLock);
		if ((positionStatisticRecPtr->GetFeeObject(s888::rwNotLock) == 0) && (feeObjPtr != 0))
			positionStatisticRecPtr->SetFeeObject(feeObjPtr, s888::rwNotLock);
		positionStatisticRecPtr->SetContractMultiplier( contractMultiplier, s888::rwNotLock);
		positionStatisticRecPtr->UnlockForWrite();
	}
	return positionStatisticRecPtr;
}  //TOrderStore::FindOrCreatePosStatisticRec()
//---------------------------------------------------------------------------
s888::CPositionStatisticRecord *TOrderStore::AddPosStatisticRecFillQty(const String& OrderId, const String& ExecId,
																	   const String& ExchangeCode, const String& Symbol,
																	   nsOrderMessageDefine::MarketEnum Market,
																	   nsOrderMessageDefine::SideEnum Side,
																	   nsOrderMessageDefine::PositionEffectEnum PositionEffect,
																	   double MatchPrice, int MatchQuantity)
{
	s888::CPositionStatisticRecord *positionStatisticRecPtr = FindOrCreatePosStatisticRec(ExchangeCode, Symbol);
	if (positionStatisticRecPtr == 0) return positionStatisticRecPtr;
	positionStatisticRecPtr->LockForWrite();
	positionStatisticRecPtr->SetMarket(Market, s888::rwNotLock);
	positionStatisticRecPtr->AddFillReportData(OrderId, ExecId, Market, Side, PositionEffect, MatchPrice, MatchQuantity, s888::rwNotLock);
	positionStatisticRecPtr->DumpData(s888::constFlush, s888::rwNotLock);
	positionStatisticRecPtr->UnlockForWrite();
	return positionStatisticRecPtr;
}  //TOrderStore::AddPosStatisticRecFillQty()
//---------------------------------------------------------------------------
void TOrderStore::AddPositionRecFillQty(TExecution* OrderMessagePtr)
{
	if (OrderMessagePtr == 0) return;
	TExchangeSymbol *symbolObjPtr = OrderMessagePtr->GetSymbol();
	if (symbolObjPtr == 0) return;

	String orderID = OrderMessagePtr->GetOrderID();
	String execID = OrderMessagePtr->GetExecID();
	nsOrderMessageDefine::MarketEnum market = OrderMessagePtr->GetMarket();
	nsOrderMessageDefine::SideEnum side = OrderMessagePtr->GetSide();
	nsOrderMessageDefine::PositionEffectEnum positionEffect = OrderMessagePtr->GetPositionEffect();
	String exchangeCode = OrderMessagePtr->GetExchangeCode();
	String symbolCode = L"";
	if (symbolObjPtr->IsMultileg())
	{
		symbolCode = symbolObjPtr->GetSymbol(1);
		AddPosStatisticRecFillQty(orderID, execID, exchangeCode, symbolCode, market, side, positionEffect,
								  OrderMessagePtr->GetLegPx1(), OrderMessagePtr->GetOrderQty());

		symbolCode = symbolObjPtr->GetSymbol(2);
		AddPosStatisticRecFillQty(orderID, execID, exchangeCode, symbolCode, market, side, positionEffect,
								  OrderMessagePtr->GetLegPx2(), OrderMessagePtr->GetOrderQty());
	}
	else
	{
		symbolCode = symbolObjPtr->GetSymbol(0);
		AddPosStatisticRecFillQty(orderID, execID, exchangeCode, symbolCode, market, side, positionEffect,
								  OrderMessagePtr->GetLastPx(), OrderMessagePtr->GetOrderQty());
	}  //if (SymbolObjPtr->IsMultileg())
}  //TOrderStore::AddPositionRecFillQty()
//------------------------------------------------------------------------------
int TOrderStore::ClearPosStatisticRecHoldPosition()
{
	int clearCount = 0;
	FPosStatisticRecCSLock.Acquire();
	for (int i = 0; i < GetPosStatisticRecArrayCount(s888::rwNotLock); i++)
	{
		s888::CPositionStatisticRecord *curPosStatisticRecPtr = GetPosStatisticRecFromArray(i, s888::rwNotLock);
		String Ex = curPosStatisticRecPtr->GetExchangeCode( s888::rwNotLock);
		String Sym = curPosStatisticRecPtr->GetSymbol(s888::rwNotLock );

		UnsubscribeSymbol( Ex , Sym, true );
		curPosStatisticRecPtr->ClearHoldMatchSummaryArray(s888::constDelete, s888::rwLockForWrite);
		clearCount++;
	}
	FPosStatisticRecCSLock.Release();
	return clearCount;
}  //TOrderStore::ClearPosStatisticRecHoldPosition()
//------------------------------------------------------------------------------
void TOrderStore::ClearOrderMessageData()
{
	TExecution* OrderMessagePtr;
	for (int i = 0; i < FOrderMessageList.ItemCount(); i++)
	{
		OrderMessagePtr = FOrderMessageList.GetItem(i);
		delete OrderMessagePtr;
	}  //for (int i = 0; i < FOrderMessageList.ItemCount(); i++)

	FOrderMessageList.Clear();
	FOrderMessageExecIDIndex.clear();
	FFillMessageSymbolIndex.clear();
//	ClearPositionRec();
}  //TOrderStore::ClearOrderMessageData()


//------------------------------------------------------------------------------
void TOrderStore::WorkingQty( const String& Exchange, const String& Symbol, int& BQty, int& SQty )
{
	CStringTExecutionMultiMap::iterator lowIt  = FOrderSymbolSidePriceIndex.lower_bound( Symbol + L"00000000000000.00000" );
	CStringTExecutionMultiMap::iterator highIt = FOrderSymbolSidePriceIndex.upper_bound( Symbol + L"99999999999999.99999" );
	CStringTExecutionMultiMap::iterator curIt;
	String preKey = L"";

	BQty = 0;
	SQty = 0;
	for(curIt = lowIt; curIt != highIt; curIt++)
	{
		String curKey = (*curIt).first;
		TExecution *curOrderRootPtr = (*curIt).second;
		if( curKey != preKey )
		{
			preKey = curKey;
			if( curOrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy )
				BQty += SumSymbolSidePriceLeavesQuantity( Symbol, curOrderRootPtr->GetSide(), curOrderRootPtr->GetPrice());
			else
				SQty += SumSymbolSidePriceLeavesQuantity( Symbol, curOrderRootPtr->GetSide(), curOrderRootPtr->GetPrice());
		}
	}
}
//------------------------------------------------------------------------------
void TOrderStore::AddOrderQtyListener(nsOrderMessageDefine::MarketEnum Market,
									  String ExchangeCode, String Symbol,
									  TOrderQtyListener* QtyListener)
{
	if (QtyListener == 0) return;

	String orderSymbol = GetOrderSymbol(ExchangeCode, Symbol, false);  //Check Symbol Index First
	String indexKey = ExchangeCode + orderSymbol;
	FOrderQtyListenerMap.insert(CStringTOrderQtyListenerMultiMap::value_type(indexKey, QtyListener));

	String lowerKey = orderSymbol + L"00000000000000.00000";
	String upperKey = orderSymbol + L"99999999999999.99999";
	CStringTExecutionMultiMap::iterator lowIt = FOrderSymbolSidePriceIndex.lower_bound(lowerKey);
	CStringTExecutionMultiMap::iterator highIt = FOrderSymbolSidePriceIndex.upper_bound(upperKey);
	CStringTExecutionMultiMap::iterator curIt;
	String preKey = L"";
	for (curIt = lowIt; curIt != highIt; curIt++)
	{
		String curKey = (*curIt).first;
		TExecution *curOrderRootPtr = (*curIt).second;
		if (curKey != preKey)
		{
			preKey = curKey;
			int quantitySum = SumSymbolSidePriceLeavesQuantity(orderSymbol, curOrderRootPtr->GetSide(), curOrderRootPtr->GetPrice());
			QtyListener->OrderQtyChanged(curOrderRootPtr->GetMarket(), orderSymbol, curOrderRootPtr->GetSide(), curOrderRootPtr->GetPrice(), quantitySum);
		}  //if (CurKey != PreKey)
	}  //for (curIt = lowIt; curIt != highIt; curIt++)

	//Process Stop Order
	indexKey = ExchangeCode + orderSymbol;
	lowerKey = indexKey + L"00000000000000.00000";
	upperKey = indexKey + L"99999999999999.99999";
	lowIt = FStopOrderConditionIndex.lower_bound(lowerKey);
	highIt = FStopOrderConditionIndex.upper_bound(upperKey);
	for (curIt = lowIt; curIt != highIt; curIt++)
	{
		TExecution *curOrderRootPtr = (*curIt).second;
		QtyListener->StopOrderQtyChanged(curOrderRootPtr->GetMarket(), orderSymbol,
										 curOrderRootPtr->GetStopPrice(),
										 curOrderRootPtr->GetSide(),
										 curOrderRootPtr->GetPrice(),
										 curOrderRootPtr->GetOrderQty(),
										 curOrderRootPtr->GetOrderType());
	}  //for (curIt = lowIt; curIt != highIt; curIt++)
	return;
}  //TOrderStore::AddOrderQtyListener()

//------------------------------------------------------------------------------
void TOrderStore::ExecuteOrderQtyListener(nsOrderMessageDefine::MarketEnum Market, String ExchangeCode, String Symbol,
										  nsOrderMessageDefine::SideEnum Side, double Px)
{
	if (FOrderQtyListenerMap.size() <= 0) return;

	String orderSymbol = GetOrderSymbol(ExchangeCode, Symbol, false);  //Check Symbol Index First
	int QuantitySum = SumSymbolSidePriceLeavesQuantity(orderSymbol, Side, Px);
	String indexKey = ExchangeCode + orderSymbol;
	typedef CStringTOrderQtyListenerMultiMap::iterator MI;
	std::pair<MI, MI> g = FOrderQtyListenerMap.equal_range(indexKey);
	TOrderQtyListener* CurListener;
	for (MI p = g.first; p != g.second; p++)
	{
		CurListener = p->second;
		CurListener->OrderQtyChanged(Market, orderSymbol, Side, Px, QuantitySum);
	}  //for (MI p = g.first; p != g.second; p++)
}  //TOrderStore::ExecuteOrderQtyListener()

//------------------------------------------------------------------------------
void TOrderStore::ExecuteOrderQtyListenerForStopOrder(nsOrderMessageDefine::MarketEnum Market, String ExchangeCode, String Symbol,
													  double StopPx, nsOrderMessageDefine::SideEnum Side,
													  double Px, int Qty,
													  nsOrderMessageDefine::OrderTypeEnum OrderType)
{
	if (FOrderQtyListenerMap.size() <= 0) return;

	String orderSymbol = GetOrderSymbol(ExchangeCode, Symbol, false);  //Check Symbol Index First
	String indexKey = ExchangeCode + orderSymbol;
	typedef CStringTOrderQtyListenerMultiMap::iterator MI;
	std::pair<MI, MI> g = FOrderQtyListenerMap.equal_range(indexKey);
	for (MI p = g.first; p != g.second; p++)
	{
		TOrderQtyListener *curListener = p->second;
		curListener->StopOrderQtyChanged(Market, orderSymbol, StopPx, Side, Px, Qty, OrderType);
	}  //for (MI p = g.first; p != g.second; p++)
}  //TOrderStore::ExecuteOrderQtyListenerForStopOrder()

//------------------------------------------------------------------------------
void TOrderStore::RemoveOrderQtyListener(TOrderQtyListener* QtyListener)
{
	if (QtyListener == 0) return;

	CStringTOrderQtyListenerMultiMap::iterator MI;
	TOrderQtyListener* CurListener;
	for (MI = FOrderQtyListenerMap.begin(); MI != FOrderQtyListenerMap.end(); MI++)
	{
		CurListener = (*MI).second;
		if (CurListener == QtyListener)
		{
			FOrderQtyListenerMap.erase(MI);
			break;
		}
	}
//	FOrderQtyListenerList.Remove(QtyListener);
}
//------------------------------------------------------------------------------
void TOrderStore::AddPositionChangeListener(nsOrderMessageDefine::MarketEnum Market, String ExchangeCode, String Symbol,
											TPositionChangeListener* PositionChangeListener)
{
	if (PositionChangeListener == 0) return;
	String orderSymbol = GetOrderSymbol(ExchangeCode, Symbol, true);  //Check TradeSymbol Index First
	String indexKey;
	BuildExchangeSymbolIndexKey(ExchangeCode, orderSymbol, indexKey);
	FPositionChangeListenerMap.insert(CStringTPositionChangeListenerMultiMap::value_type(indexKey, PositionChangeListener));

	//Subscribe Symbol Price Data
	SubscribeSymbol(ExchangeCode, orderSymbol, true, true);  //need Increase Count, Subscribe Anyway);
	ExecutePositionChangeListener(Market, ExchangeCode, orderSymbol);
}
//------------------------------------------------------------------------------
void TOrderStore::ExecutePositionChangeListener(nsOrderMessageDefine::MarketEnum Market, String ExchangeCode, String OrderSymbol,
												nsOrderMessageDefine::SideEnum ExeRptSide,
												double ExeRptPx, int ExeRptQty, int ExeRptTickCount)
{
	TTickList *tickListPtr = NULL;
	if (FMarketDataStore != NULL)
		tickListPtr = (TTickList*)FMarketDataStore->GetTickList(ExchangeCode, OrderSymbol);

	double tickValue = 0.0;
	if( tickListPtr != NULL )
		tickValue = tickListPtr->GetContractMultiplier();

	s888::CPositionStatisticRecord *posStatisticRecPtr = FindOrCreatePosStatisticRec(ExchangeCode, OrderSymbol);
	DynamicArray<void*> matchPxStatisticArray;   //For OnProfit Event
	int holdPosition = 0;
	int buyQuantity = 0;
	double buyAmount = 0.0;
	int buyOpenInterestQuantity = 0;
	double buyOpenInterestAmount = 0.0;
	int sellQuantity = 0;
	double sellAmount = 0.0;
	int sellOpenInterestQuantity = 0;
	double sellOpenInterestAmount = 0.0;
	double floatingProfit = 0.0;
	double contractMultiplier = 1.0;
	double openInterestTax;
	double openInterestFee;
	matchPxStatisticArray.Length = 0;
	int curMatchPxStatisticIndex = 0;
	if (posStatisticRecPtr != 0)
	{
		posStatisticRecPtr->LockForWrite();
		holdPosition = posStatisticRecPtr->GetBuyHoldOpenQuantity(s888::rwNotLock) - posStatisticRecPtr->GetSellHoldOpenQuantity(s888::rwNotLock);
		buyQuantity = posStatisticRecPtr->GetBuyQuantity(s888::rwNotLock);
		buyAmount = posStatisticRecPtr->GetBuyAmount(s888::rwNotLock);
		buyOpenInterestQuantity = posStatisticRecPtr->GetBuyOpenInterestQuantity(s888::rwNotLock);
		buyOpenInterestAmount = posStatisticRecPtr->GetBuyOpenInterestAmount(s888::rwNotLock);
		sellQuantity = posStatisticRecPtr->GetSellQuantity(s888::rwNotLock);
		sellAmount = posStatisticRecPtr->GetSellAmount(s888::rwNotLock);
		sellOpenInterestQuantity = posStatisticRecPtr->GetSellOpenInterestQuantity(s888::rwNotLock);
		sellOpenInterestAmount = posStatisticRecPtr->GetSellOpenInterestAmount(s888::rwNotLock);
		floatingProfit = posStatisticRecPtr->GetFloatingProfit(s888::rwNotLock);
		openInterestTax = posStatisticRecPtr->GetOpenInterestTax(s888::rwNotLock);
		openInterestFee = posStatisticRecPtr->GetOpenInterestFee(s888::rwNotLock);
		contractMultiplier = posStatisticRecPtr->GetContractMultiplier(s888::rwNotLock);
		int buyPxQtyStatisticArrayCount = posStatisticRecPtr->GetBuyPxQtyStatisticArrayCount(s888::rwNotLock);
		int sellPxQtyStatisticArrayCount = posStatisticRecPtr->GetSellPxQtyStatisticArrayCount(s888::rwNotLock);
		matchPxStatisticArray.Length = buyPxQtyStatisticArrayCount + sellPxQtyStatisticArrayCount;
		for (int buyIndex = 0; buyIndex < buyPxQtyStatisticArrayCount; buyIndex++)
		{
			s888::CMatchPriceStatisticRecord *buyPxStatisticRecPtr = posStatisticRecPtr->GetBuyPxQtyStatisticFromArray(buyIndex, s888::rwNotLock);
			matchPxStatisticArray[curMatchPxStatisticIndex++] = buyPxStatisticRecPtr;
		}  //for (int buyIndex = 0; buyIndex < buyPxQtyStatisticArrayCount; buyIndex++)

		for (int sellIndex = 0; sellIndex < sellPxQtyStatisticArrayCount; sellIndex++)
		{
			s888::CMatchPriceStatisticRecord *sellPxStatisticRecPtr = posStatisticRecPtr->GetSellPxQtyStatisticFromArray(sellIndex, s888::rwNotLock);
			matchPxStatisticArray[curMatchPxStatisticIndex++] = sellPxStatisticRecPtr;
		}  //for (int sellIndex = 0; sellIndex < sellPxQtyStatisticArrayCount; sellIndex++)

		posStatisticRecPtr->UnlockForWrite();
		if (FOnFloatingProfit != 0)
			FOnFloatingProfit(this, ExchangeCode, OrderSymbol, posStatisticRecPtr);
	}  //if (posStatisticRecPtr != 0)

	int netPosition = buyQuantity - sellQuantity;
	double buyAveragegPrice = 0.0;
	if (buyQuantity > 0) buyAveragegPrice = buyAmount / static_cast<double>(buyQuantity) / contractMultiplier;
	double sellAveragegPrice = 0.0;
	if (sellQuantity > 0) sellAveragegPrice = sellAmount / static_cast<double>(sellQuantity) / contractMultiplier;
	double openInterestAveragegPrice = 0.0;
	int netOpenInterestQuantity = abs(buyOpenInterestQuantity - sellOpenInterestQuantity);
	if (netOpenInterestQuantity > 0)
		openInterestAveragegPrice = fabs(buyOpenInterestAmount - sellOpenInterestAmount) / static_cast<double>(netOpenInterestQuantity) / contractMultiplier;
	else
		openInterestAveragegPrice = 0.0;

	String indexKey;
	BuildExchangeSymbolIndexKey(ExchangeCode, OrderSymbol, indexKey);
	typedef CStringTPositionChangeListenerMultiMap::iterator PMI;
	std::pair<PMI, PMI> f = FPositionChangeListenerMap.equal_range(indexKey);
	for (PMI pp = f.first; pp != f.second; pp++)
	{
		TPositionChangeListener *curListener = pp->second;
		curListener->OnFill(holdPosition, buyAveragegPrice, sellAveragegPrice, buyQuantity, sellQuantity,
							buyOpenInterestQuantity, sellOpenInterestQuantity,
							matchPxStatisticArray, netPosition, floatingProfit);
		curListener->OnProfit(openInterestAveragegPrice, floatingProfit - (openInterestTax + openInterestFee));  //For OnProfit Event
		if ((ExeRptQty > 0) && (ExeRptTickCount != 0))
			curListener->OnFill(ExeRptSide, ExeRptPx, ExeRptQty, ExeRptTickCount);  //For Ping Pong
	}  //for (PMI pp = f.first; pp != f.second; pp++)
	matchPxStatisticArray.Length = 0;
}  //TOrderStore::ExecutePositionChangeListener()

//------------------------------------------------------------------------------
void TOrderStore::RemovePositionChangeListener(TPositionChangeListener* PositionChangeListener)
{
	if (PositionChangeListener == 0) return;

	String indexKey = L"";
	CStringTPositionChangeListenerMultiMap::iterator MI;
	TPositionChangeListener* CurListener;
	for (MI = FPositionChangeListenerMap.begin(); MI != FPositionChangeListenerMap.end(); MI++)
	{
		CurListener = (*MI).second;
		if (CurListener == PositionChangeListener)
		{
			indexKey = (*MI).first;
			FPositionChangeListenerMap.erase(MI);
			break;
		}
	}  //for (MI = FPositionChangeListenerMap.begin(); MI != FPositionChangeListenerMap.end(); MI++)
	if (indexKey.Length() <= 0) return;

	//UnSubscribe Symbol Price Data
	String exchangeCode = L"";
	String symbol = L"";
	ParseExchangeSymbolIndexKey(indexKey, exchangeCode, symbol);
	UnsubscribeSymbol(exchangeCode, symbol, false);  //Unsubscribe if need
}  //TOrderStore::RemovePositionChangeListener()

//------------------------------------------------------------------------------
void TOrderStore::AddOrderMessageListener(TOrderMessageListener* MessageListener)
{
	if( FOrderMessageListenerList.IndexOf( MessageListener ) == -1 )
		FOrderMessageListenerList.Add(MessageListener);
}
//------------------------------------------------------------------------------
void TOrderStore::ExecuteOrderMessageListener(TExecution* OrderRootPtr)
{
	if (FOrderMessageListenerList.ItemCount() <= 0) return;
	TOrderMessageListener* CurListener;
	for (int i = 0; i < FOrderMessageListenerList.ItemCount(); i++)
	{
		CurListener = FOrderMessageListenerList.GetItem(i);
		CurListener->OrderMessageArrived(OrderRootPtr);
	}
}
//------------------------------------------------------------------------------
void TOrderStore::ExecuteRefreshData()
{
	if (FOrderMessageListenerList.ItemCount() <= 0) return;
	TOrderMessageListener* CurListener;
	for (int i = 0; i < FOrderMessageListenerList.ItemCount(); i++)
	{
		CurListener = FOrderMessageListenerList.GetItem(i);
		CurListener->RefreshDataNotice();
	}
}
//------------------------------------------------------------------------------
void TOrderStore::ExecuteStopOrderChanged(TExecution* OrderRootPtr, StopOrderStatusEnum StopOrderStatus)
{
	if (FOrderMessageListenerList.ItemCount() <= 0) return;
	TOrderMessageListener* CurListener;
	for (int i = 0; i < FOrderMessageListenerList.ItemCount(); i++)
	{
		CurListener = FOrderMessageListenerList.GetItem(i);
		CurListener->StopOrderChanged(OrderRootPtr, StopOrderStatus);
	}
}
//------------------------------------------------------------------------------
TFileStream* TOrderStore::GetOrderMsgFileStream() {return FOrderMsgFileStream;}
//------------------------------------------------------------------------------
void TOrderStore::RemoveOrderMessageListener(TOrderMessageListener* MessageListener)
{
	if( FOrderMessageListenerList.IndexOf( MessageListener ) != -1 )
		FOrderMessageListenerList.Remove(MessageListener);
}
//------------------------------------------------------------------------------
bool TOrderStore::CheckReportExecIDExist(TExecution *NewRptPtr)
{
	bool IsExist = false;
	if (NewRptPtr != 0)
	{
		UnicodeString RptExecID = NewRptPtr->GetExecID();
		if (RptExecID.Length() <= 0) RptExecID = NewRptPtr->GetTimeOrderIDKey();

		if (RptExecID.Length() > 0)
		{
			NewRptPtr->SetExecID(RptExecID);
			if (FindOrderMessageByExecID(RptExecID) != 0) IsExist = true;
		}
	}
	return IsExist;
}
//------------------------------------------------------------------------------
bool TOrderStore::ProcessPendingNewMsg(TExecution *NewRptPtr,
									   nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
									   UFCType::Int64 RptNID, const UnicodeString& RptOrderId,
									   const UnicodeString& LocalMessageTimeStr)
{
	bool IsSuccess = false;
	//Get Order Root
	//Just Find Order Root By Message NID and Order Root Must Not Exists
	TExecution *OrderRootPtr = FindOrderRootByNID(RptNID);
	if (OrderRootPtr != 0) return IsSuccess; //Order Root Exist

	String RptNIDStr;
	RptNIDStr.printf(L"%lld", RptNID);
	String TimeOrderIDKey = NewRptPtr->GetLocalMessageTime() + RptNIDStr;
	NewRptPtr->SetTimeOrderIDKey(TimeOrderIDKey);
	if (CheckReportExecIDExist(NewRptPtr))  //Report Message Already Exist
		return IsSuccess;
	else
		IsSuccess = true;

	//Create a New Order Root
	OrderRootPtr = new TExecution(NewRptPtr);
	OrderRootPtr->SetLeavesQty(NewRptPtr->GetOrderQty());
	OrderRootPtr->SetCumQty(0);
	OrderRootPtr->SetRoot(true);
	OrderRootPtr->SetRootCreatedByReport(true);
	OrderRootPtr->SetLocalMessageTime(LocalMessageTimeStr);
	OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
	AddOrderRoot(OrderRootPtr);
	InsertOrderRootNIDIndex(OrderRootPtr);
	InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
	InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
	AddOrderMessage(NewRptPtr);
	InsertOrderMessageExecIDIndex(NewRptPtr);
	OrderRootPtr->InsertOrderMessage(NewRptPtr);
	ExecuteOrderMessageListener(OrderRootPtr);
	if (FIsAutoPositionEffect &&
		((NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peClose) ||
		 (NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)))
	{
		String exchangeCode = OrderRootPtr->GetExchangeCode();
		String symbol = OrderRootPtr->GetSymbol()->GetSymbol(0);
		String tradeSymbol = OrderRootPtr->GetTradeSymbol();
		int processPositionCode = 0;
		String processPositionMsg = L"";
		s888::CContractPosition *positionPtr = s888::gTradingObjsPool.FindAccountContractPosition(FID, exchangeCode, symbol, s888::constCreate, processPositionCode, processPositionMsg, s888::rwLockForWrite);
		if (positionPtr != 0)
		{
			positionPtr->LockForWrite();
			if (NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peClose)
			{
				if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy)
					positionPtr->AddBuyOrderCloseQuantity(OrderRootPtr->GetOrderQty(), s888::rwNotLock);
				else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell)
					positionPtr->AddSellOrderCloseQuantity(OrderRootPtr->GetOrderQty(), s888::rwNotLock);
			}
			else if (NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)
			{
				if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy)
					positionPtr->AddBuyOrderCloseTodayQuantity(OrderRootPtr->GetOrderQty(), s888::rwNotLock);
				else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell)
					positionPtr->AddSellOrderCloseTodayQuantity(OrderRootPtr->GetOrderQty(), s888::rwNotLock);
			}
			String logMessage;
			logMessage.printf(L" PendingNew Order Report NID[%lld] [%s-%s(%s)] side[%s] pe[%s] Qty[%d] Px[%15.6lf].",
							  RptNID, exchangeCode, symbol, tradeSymbol,
							  s888::DescribeBuySell(OrderRootPtr->GetSide()),
							  s888::DescribePositionEffect(OrderRootPtr->GetPositionEffect()),
							  OrderRootPtr->GetOrderQty(), OrderRootPtr->GetPrice());
			UFC::BufferedLog::Printf(" TOrderStore::ProcessPendingNewMsg() %s",
									 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
			positionPtr->DumpData(s888::constFlush, s888::rwNotLock);
			positionPtr->UnlockForWrite();
		}
		else
		{
			UFC::BufferedLog::Printf(" TOrderStore::ProcessNewMsg() Find Account Contract Position Failed. %d[%s]",
									 processPositionCode, s888::ConvertUnicodeToUFCAnsiStr(processPositionMsg).c_str());
			UFC::BufferedLog::FlushToFile();
		}  //if (positionPtr != 0)
	}
	return IsSuccess;
}
//------------------------------------------------------------------------------
bool TOrderStore::ProcessNewMsg(TExecution *NewRptPtr,
								nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
								UFCType::Int64 RptNID, const UnicodeString& RptOrderId,
								const UnicodeString& LocalMessageTimeStr)
{
	bool IsSuccess = false;
	UnicodeString TimeOrderIDKey = LocalMessageTimeStr + RptOrderId;
	NewRptPtr->SetTimeOrderIDKey(TimeOrderIDKey);
	if (CheckReportExecIDExist(NewRptPtr))  //Report Message Already Exist
		return IsSuccess;

	//Get Order Root
	//Find Order Root By Message NID First
	TExecution *OrderRootPtr = FindOrderRootByNID(RptNID);
	if (OrderRootPtr == 0)  //Find Order Root By Order ID
		OrderRootPtr = FindOrderRootByOrderID(RptOrderId);

	if (OrderRootPtr == 0)
	{   //Search Order Root By Message RootNID Map
		int OrderRootNID = NewRptPtr->GetRootNID();
		if (OrderRootNID > 0)
			OrderRootPtr = FindOrderRootByNID(OrderRootNID);
	}  //if (OrderRootPtr == 0)

	bool IsOrderStatusChanged = true;
	bool isOrderRootExist = true;
	if (OrderRootPtr == 0)
	{   //Create a New Order Root
		IsOrderStatusChanged = true;
		OrderRootPtr = new TExecution(NewRptPtr);
		OrderRootPtr->SetRoot(true);
		OrderRootPtr->SetRootCreatedByReport(true);
		OrderRootPtr->SetLocalMessageTime(NewRptPtr->GetLocalMessageTime());
		OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		if (OrderRootPtr->GetLeavesQty() <= 0) OrderRootPtr->SetLeavesQty(OrderRootPtr->GetOrderQty());
		AddOrderRoot(OrderRootPtr);
		InsertOrderRootNIDIndex(OrderRootPtr);
		IsSuccess = true;
		isOrderRootExist = false;
	}
	else
	{
		nsOrderMessageDefine::OrderStatusEnum OrderRootStatus = OrderRootPtr->GetOrderStatus();
		nsOrderMessageDefine::ExecTypeEnum RptExecType = NewRptPtr->GetExecType();
		if ((OrderRootStatus == nsOrderMessageDefine::osReplaced) ||
			(OrderRootStatus == nsOrderMessageDefine::osCanceled) ||
			(OrderRootStatus == nsOrderMessageDefine::osPartiallyFilled) ||
			(OrderRootStatus == nsOrderMessageDefine::osFilled) ||
			((RptExecType != nsOrderMessageDefine::etOrderStatus) &&
			 (OrderRootStatus == nsOrderMessageDefine::osRejected)))
			return IsSuccess;
		else
			IsSuccess = true;

		if (OrderRootStatus != nsOrderMessageDefine::osNew)
		{
			IsOrderStatusChanged = true;
			OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osNew);
			OrderRootPtr->SetTransactTime(NewRptPtr->GetTransactTime());
			OrderRootPtr->SetOrderID(NewRptPtr->GetOrderID());
			if (NewRptPtr->GetLeavesQty() > 0)
				OrderRootPtr->SetLeavesQty(NewRptPtr->GetLeavesQty());
			else
				OrderRootPtr->SetLeavesQty(OrderRootPtr->GetOrderQty());
			OrderRootPtr->SetPxDigit(NewRptPtr->GetPxDigit());
			if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)
			{
				if (OrderRootPtr->GetTimeOrderIDKey().Length() > 0)
				{
					RemoveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
					RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
				}

				OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
			}
		}
		String RptMsg = UTF8String( NewRptPtr->GetText());
		OrderRootPtr->SetStatusCode(NewRptPtr->GetStatusCode());
		OrderRootPtr->SetText( RptMsg );
	}  //if (OrderRootPtr == 0)

	if (IsOrderStatusChanged)
	{
		InsertOrderRootOrderIDIndex(OrderRootPtr);
		InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
		InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
		InsertOrderRootSymbolSidePriceIndex(OrderRootPtr);
		if (!OrderRootPtr->GetSymbol()->IsMultileg())
		{
			ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), OrderRootPtr->GetExchangeCode(),
									OrderRootPtr->GetSymbol()->GetSymbol(0),
									OrderRootPtr->GetSide(),
									OrderRootPtr->GetPrice());
		}
	}  //if (IsOrderStatusChanged)

	if (!isOrderRootExist && IsOrderStatusChanged && FIsAutoPositionEffect && OrderRootPtr->IsRootCreatedByReport() &&
		((NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peClose) ||
		 (NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)))
	{
		String exchangeCode = OrderRootPtr->GetExchangeCode();
		String symbol = OrderRootPtr->GetSymbol()->GetSymbol(0);
		String tradeSymbol = OrderRootPtr->GetTradeSymbol();
		int processPositionCode = 0;
		String processPositionMsg = L"";
		s888::CContractPosition *positionPtr = s888::gTradingObjsPool.FindAccountContractPosition(FID, exchangeCode, symbol, s888::constCreate, processPositionCode, processPositionMsg, s888::rwLockForWrite);
		if (positionPtr != 0)
		{
			positionPtr->LockForWrite();
			if (NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peClose)
			{
				if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy)
					positionPtr->AddBuyOrderCloseQuantity(OrderRootPtr->GetOrderQty(), s888::rwNotLock);
				else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell)
					positionPtr->AddSellOrderCloseQuantity(OrderRootPtr->GetOrderQty(), s888::rwNotLock);
			}
			else if (NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)
			{
				if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy)
					positionPtr->AddBuyOrderCloseTodayQuantity(OrderRootPtr->GetOrderQty(), s888::rwNotLock);
				else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell)
					positionPtr->AddSellOrderCloseTodayQuantity(OrderRootPtr->GetOrderQty(), s888::rwNotLock);
			}
			String logMessage;
			logMessage.printf(L" New Order Report NID[%lld] [%s-%s(%s)] side[%s] pe[%s] Qty[%d] Px[%15.6lf].",
							  RptNID, exchangeCode, symbol, tradeSymbol,
							  s888::DescribeBuySell(OrderRootPtr->GetSide()),
							  s888::DescribePositionEffect(OrderRootPtr->GetPositionEffect()),
							  OrderRootPtr->GetOrderQty(), OrderRootPtr->GetPrice());
			UFC::BufferedLog::Printf(" TOrderStore::ProcessNewMsg() %s",
									 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
			positionPtr->DumpData(s888::constFlush, s888::rwNotLock);
			positionPtr->UnlockForWrite();
		}
		else
		{
			UFC::BufferedLog::Printf(" TOrderStore::ProcessNewMsg() Find Account Contract Position Failed. %d[%s]",
									 processPositionCode, s888::ConvertUnicodeToUFCAnsiStr(processPositionMsg).c_str());
			UFC::BufferedLog::FlushToFile();
		}  //if (positionPtr != 0)
	}

	AddOrderMessage(NewRptPtr);
	InsertOrderMessageExecIDIndex(NewRptPtr);
	NewRptPtr->SetRootNID(OrderRootPtr->GetNID());
	OrderRootPtr->InsertOrderMessage(NewRptPtr);
	ExecuteOrderMessageListener(OrderRootPtr);
	return IsSuccess;
}  //TOrderStore::ProcessNewMsg()

//---------------------------------------------------------------------------
TExecution *TOrderStore::FindOrderRootByOrderId_NID(const UnicodeString& RptOrderId, UFCType::Int64 RptNID, int OrderRootNID)
{
	TExecution *OrderRootPtr = 0;

	//Find Order Root By Order ID First
	if ((RptOrderId.Length() > 0) && (RptOrderId != L"00000"))
		OrderRootPtr = FindOrderRootByOrderID(RptOrderId);

	//Find Order Root By Message NID
	if (OrderRootPtr == 0) OrderRootPtr = FindOrderRootByNID(RptNID);

	if (OrderRootPtr == 0)  //Search Order Root By Message RootNID Map
	{
		if (OrderRootNID > 0)
			OrderRootPtr = FindOrderRootByNID(OrderRootNID);
	}  //if (OrderRootPtr == 0)

	return OrderRootPtr;
}  //TOrderStore::FindOrderRootByOrderId_NID()

//------------------------------------------------------------------------------
bool TOrderStore::ProcessReplacedMsg(TExecution *NewRptPtr,
									 nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
									 UFCType::Int64 RptNID,
									 const UnicodeString& RptOrderId,
									 const UnicodeString& LocalMessageTimeStr)
{
	bool IsSuccess = false;
	UnicodeString TimeOrderIDKey = LocalMessageTimeStr + RptOrderId;
	NewRptPtr->SetTimeOrderIDKey(TimeOrderIDKey);
	if (CheckReportExecIDExist(NewRptPtr))  //Report Message Already Exist
		return IsSuccess;

	nsOrderMessageDefine::TMPExecTypeEnum RptTMPExecType = NewRptPtr->GetTMPExecType();
	if ((RptTMPExecType != nsOrderMessageDefine::tetReplaced) &&
		(RptTMPExecType != nsOrderMessageDefine::tetPxReplaced) &&
		(RptTMPExecType != nsOrderMessageDefine::tetPxReplaced2))
		return IsSuccess;

	//Get Order Root
	TExecution *OrderRootPtr = FindOrderRootByOrderId_NID(RptOrderId, RptNID, NewRptPtr->GetRootNID());
	bool IsOrderStatusChanged = false;
	if (OrderRootPtr == 0)
	{   //Create a New Order Root
		IsOrderStatusChanged = true;
		OrderRootPtr = new TExecution(NewRptPtr);
		OrderRootPtr->SetRoot(true);
		OrderRootPtr->SetLocalMessageTime(NewRptPtr->GetLocalMessageTime());
		OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		AddOrderRoot(OrderRootPtr);
		InsertOrderRootNIDIndex(OrderRootPtr);
		InsertOrderRootOrderIDIndex(OrderRootPtr);
		InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
		if (OrderRootPtr->GetLeavesQty() <= 0)
		{
			OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
			InsertCanceledOrderRootTimeOrderIDIndex(OrderRootPtr);
		}
		else
		{
			InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
			InsertOrderRootSymbolSidePriceIndex(OrderRootPtr);
        }
	}
	else
	{   //Order Root Exist
		nsOrderMessageDefine::OrderStatusEnum OrderRootStatus = OrderRootPtr->GetOrderStatus();
		nsOrderMessageDefine::ExecTypeEnum RptExecType = NewRptPtr->GetExecType();
		if ((OrderRootStatus == nsOrderMessageDefine::osCanceled) ||
			(OrderRootStatus == nsOrderMessageDefine::osFilled) ||
			((RptExecType != nsOrderMessageDefine::etOrderStatus) &&
			 (OrderRootStatus == nsOrderMessageDefine::osRejected)))
			return IsSuccess;

		UnicodeString OrderRootOrderId = OrderRootPtr->GetOrderID();
		if (((OrderRootOrderId.Length() <= 0) || (OrderRootOrderId == L"00000")) &&
			((RptOrderId.Length() > 0) && (RptOrderId != L"00000")))
		{
			OrderRootPtr->SetOrderID(RptOrderId);
			InsertOrderRootOrderIDIndex(OrderRootPtr);
		}

		if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)
		{
			if (OrderRootPtr->GetTimeOrderIDKey().Length() > 0)
			{
				RemoveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
				RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
			}

			UnicodeString EmptyStr = L"";
			OrderRootPtr->SetLocalMessageTime(EmptyStr);
			OrderRootPtr->SetTimeOrderIDKey(EmptyStr);
		}

		if (OrderRootPtr->GetLocalMessageTime().Length() <= 0)
			OrderRootPtr->SetLocalMessageTime(NewRptPtr->GetLocalMessageTime());

		if (OrderRootPtr->GetTimeOrderIDKey().Length() <= 0)
		{
			OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
			InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
		}

		if (RptTMPExecType == nsOrderMessageDefine::tetReplaced)
		{   //Replace Quantity
			if ((OrderRootStatus != nsOrderMessageDefine::osReplaced) ||
				(OrderRootPtr->GetLeavesQty() != NewRptPtr->GetLeavesQty()))
			{
				IsOrderStatusChanged = true;
				OrderRootPtr->SetLeavesQty(NewRptPtr->GetLeavesQty());
				if (OrderRootPtr->GetOrderQty() > NewRptPtr->GetOrderQty())
					OrderRootPtr->SetOrderQty(OrderRootPtr->GetOrderQty() - NewRptPtr->GetOrderQty());
				else
					OrderRootPtr->SetOrderQty(NewRptPtr->GetLeavesQty());

				if (OrderRootPtr->GetLeavesQty() <= 0)
				{
					RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
					OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
					InsertCanceledOrderRootTimeOrderIDIndex(OrderRootPtr);
				}
				else
				{
					InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
					OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osReplaced);
				}

				NewRptPtr->SetPrice(OrderRootPtr->GetPrice());
				NewRptPtr->SetLastPx(OrderRootPtr->GetLastPx());
			}
		}
		else
		{  //Replace Price
			IsOrderStatusChanged = true;
			RemoveOrderRootSymbolSidePriceIndex(OrderRootPtr);
			if (!OrderRootPtr->GetSymbol()->IsMultileg())
				ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), OrderRootPtr->GetExchangeCode(),
										OrderRootPtr->GetSymbol()->GetOriginalSymbol(),
										OrderRootPtr->GetSide(),
										OrderRootPtr->GetPrice());

			OrderRootPtr->SetPrice(NewRptPtr->GetPrice());
			OrderRootPtr->SetOrderType(NewRptPtr->GetOrderType());
			OrderRootPtr->SetTimeInForce(NewRptPtr->GetTimeInForce());
			OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osReplaced);
			InsertOrderRootSymbolSidePriceIndex(OrderRootPtr);
		}  //if (RptTMPExecType == nsOrderMessageDefine::tetReplaced)
		String RptMsg = UTF8String( NewRptPtr->GetText());

		OrderRootPtr->SetStatusCode(NewRptPtr->GetStatusCode());
		OrderRootPtr->SetText( RptMsg );
	}  //if (OrderRootPtr == 0)

	IsSuccess = true;
	OrderRootPtr->SetTMPExecType(RptTMPExecType);
	if ((OrderRootPtr->GetDisplayName().Length() <= 0) &&
		(NewRptPtr->GetDisplayName().Length() > 0))
		OrderRootPtr->SetDisplayName(NewRptPtr->GetDisplayName());

	AddOrderMessage(NewRptPtr);
	InsertOrderMessageExecIDIndex(NewRptPtr);
	NewRptPtr->SetRootNID(OrderRootPtr->GetNID());
	OrderRootPtr->InsertOrderMessage(NewRptPtr);
	if (IsOrderStatusChanged)
	{
		if (!OrderRootPtr->GetSymbol()->IsMultileg())
		{
			ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), OrderRootPtr->GetExchangeCode(),
									OrderRootPtr->GetSymbol()->GetOriginalSymbol(),
									OrderRootPtr->GetSide(),
									OrderRootPtr->GetPrice());
		}
	}

	ExecuteOrderMessageListener(OrderRootPtr);
	return IsSuccess;
}  //TOrderStore::ProcessReplacedMsg()

//------------------------------------------------------------------------------
bool TOrderStore::ProcessCanceledMsg(TExecution *NewRptPtr,
									 nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
									 UFCType::Int64 RptNID, const UnicodeString& RptOrderId,
									 const UnicodeString& LocalMessageTimeStr)
{
	bool IsSuccess = false;
	String TimeOrderIDKey = LocalMessageTimeStr + RptOrderId;
	NewRptPtr->SetTimeOrderIDKey(TimeOrderIDKey);
	if (CheckReportExecIDExist(NewRptPtr))  //Report Message Already Exist
		return IsSuccess;

	//Get Order Root
	TExecution *OrderRootPtr = FindOrderRootByOrderId_NID(RptOrderId, RptNID, NewRptPtr->GetRootNID());
	if (OrderRootPtr == 0)
	{
		UFC::BufferedLog::Printf(" TOrderStore::ProcessCanceledMsg() Order Id[%s] Not Found.", s888::ConvertUnicodeToUFCAnsiStr(RptOrderId).c_str());
		UFC::BufferedLog::FlushToFile();
		return false;
	}

	bool IsOrderStatusChanged = false;
	int leavesQty = OrderRootPtr->GetLeavesQty();
	if (OrderRootPtr == 0)
	{   //Create a New Order Root
		IsOrderStatusChanged = true;
		OrderRootPtr = new TExecution(NewRptPtr);
		OrderRootPtr->SetRoot(true);
		OrderRootPtr->SetLocalMessageTime(NewRptPtr->GetLocalMessageTime());
		OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		AddOrderRoot(OrderRootPtr);
		InsertOrderRootNIDIndex(OrderRootPtr);
		InsertOrderRootOrderIDIndex(OrderRootPtr);
		InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
		InsertCanceledOrderRootTimeOrderIDIndex(OrderRootPtr);
	}
	else
	{   //Order Root Exist
		nsOrderMessageDefine::OrderStatusEnum OrderRootStatus = OrderRootPtr->GetOrderStatus();
		if (OrderRootStatus == nsOrderMessageDefine::osFilled)
			return IsSuccess;

		UnicodeString OrderRootOrderId = OrderRootPtr->GetOrderID();
		if (((OrderRootOrderId.Length() <= 0) || (OrderRootOrderId == L"00000")) &&
			((RptOrderId.Length() > 0) && (RptOrderId != L"00000")))
		{
			OrderRootPtr->SetOrderID(RptOrderId);
			InsertOrderRootOrderIDIndex(OrderRootPtr);
		}

		if (OrderRootStatus != nsOrderMessageDefine::osCanceled)
		{
			IsOrderStatusChanged = true;
			if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)
			{
				if (OrderRootPtr->GetTimeOrderIDKey().Length() > 0)
				{
					RemoveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
					RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
				}

				UnicodeString EmptyStr = L"";
				OrderRootPtr->SetLocalMessageTime(EmptyStr);
				OrderRootPtr->SetTimeOrderIDKey(EmptyStr);
			}

			if (OrderRootPtr->GetLocalMessageTime().Length() <= 0)
				OrderRootPtr->SetLocalMessageTime(NewRptPtr->GetLocalMessageTime());

			if (OrderRootPtr->GetTimeOrderIDKey().Length() <= 0)
			{
				OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
				InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
			}

			if ((OrderRootStatus == nsOrderMessageDefine::osNew) ||
				(OrderRootStatus == nsOrderMessageDefine::osRejected) ||
				(OrderRootStatus == nsOrderMessageDefine::osReplaced) ||
				(OrderRootStatus == nsOrderMessageDefine::osPartiallyFilled))
				RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());

			InsertCanceledOrderRootTimeOrderIDIndex(OrderRootPtr);
			OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
		}
		String RptMsg = UTF8String( NewRptPtr->GetText());

		OrderRootPtr->SetStatusCode(NewRptPtr->GetStatusCode());
		OrderRootPtr->SetText( RptMsg );
	}  //if (OrderRootPtr == 0)

	IsSuccess = true;
	AddOrderMessage(NewRptPtr);
	InsertOrderMessageExecIDIndex(NewRptPtr);
	NewRptPtr->SetRootNID(OrderRootPtr->GetNID());
	OrderRootPtr->InsertOrderMessage(NewRptPtr);
	if (IsOrderStatusChanged)
	{
		if (!OrderRootPtr->GetSymbol()->IsMultileg())
			ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), OrderRootPtr->GetExchangeCode(),
									OrderRootPtr->GetSymbol()->GetOriginalSymbol(),
									OrderRootPtr->GetSide(),
									OrderRootPtr->GetPrice());

		if (FIsAutoPositionEffect && (leavesQty > 0) &&
			((NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peClose) ||
			 (NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)))
		{
			String positionMsg = L"";
			s888::CTradingAccount *accountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex(FID, s888::rwLockForRead);
			if (accountPtr != 0)
			{
				String exchangeCode = OrderRootPtr->GetExchangeCode();
				String symbol = OrderRootPtr->GetSymbol()->GetSymbol(0);
				String tradeSymbol = OrderRootPtr->GetTradeSymbol();
				s888::CContractPosition *positionPtr = accountPtr->FindContractPositionFromSymbolIndex(exchangeCode, symbol, s888::rwLockForRead);
				if (positionPtr != 0)
				{
					positionPtr->LockForWrite();
					positionPtr->SubstractOrderQuantity(OrderRootPtr->GetPositionEffect(), OrderRootPtr->GetSide(), leavesQty, s888::rwNotLock);
					String logMessage;
					logMessage.printf(L" Cancel Report[%s] NID[%lld] [%s-%s(%s)] side[%s] pe[%s] Qty[%d] leavesQty[%d].",
									  RptOrderId, RptNID, exchangeCode, symbol, tradeSymbol,
									  s888::DescribeBuySell(OrderRootPtr->GetSide()),
									  s888::DescribePositionEffect(OrderRootPtr->GetPositionEffect()),
									  OrderRootPtr->GetOrderQty(), OrderRootPtr->GetLeavesQty());
					UFC::BufferedLog::Printf(" TOrderStore::ProcessCanceledMsg() %s",
											 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
					positionPtr->DumpData(s888::constFlush, s888::rwNotLock);
					positionPtr->UnlockForWrite();
				}
				else
					positionMsg.printf(L"Account[%s] Contract Position[%s-%s(%s)] not found.", FID, exchangeCode, symbol, tradeSymbol);
			}
			else
				positionMsg.printf(L"Account[%s] not Found.", FID);

			if (positionMsg.Length() > 0)
			{
				UFC::BufferedLog::Printf(" TOrderStore::ProcessCanceledMsg() %s", s888::ConvertUnicodeToUFCAnsiStr(positionMsg).c_str());
				UFC::BufferedLog::FlushToFile();
			}
		}  //if (FIsAutoPositionEffect && (leavesQty > 0) &&
		//AddCancelStatistic(OrderRootPtr->GetExchangeCode(), OrderRootPtr->GetSymbol()->GetOriginalSymbol(), leavesQty);
	}

	ExecuteOrderMessageListener(OrderRootPtr);
	return IsSuccess;
}  //TOrderStore::ProcessCanceledMsg()

//------------------------------------------------------------------------------
bool TOrderStore::ProcessFilledMsg(TExecution *NewRptPtr,
								   nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
								   UFCType::Int64 RptNID, const UnicodeString& RptOrderId,
								   const UnicodeString& LocalMessageTimeStr)
{
	bool IsSuccess = false;
	String TimeOrderIDKey = LocalMessageTimeStr + RptOrderId;
	NewRptPtr->SetTimeOrderIDKey(TimeOrderIDKey);
	String logMessage = L"";
	String exchangeCode = NewRptPtr->GetExchangeCode();
	String orderSymbol  = NewRptPtr->GetSymbol()->GetSymbol(0);
	String tradeSymbol  = NewRptPtr->GetTradeSymbol();
	String orderId      = NewRptPtr->GetOrderID();
	String execId       = NewRptPtr->GetExecID();
	if (CheckReportExecIDExist(NewRptPtr))  //Report Message Already Exist
	{
		logMessage.printf(L"OrderId[%s] [%s-%s(%s)]: ExecId[%s] Exist Error.", orderId, exchangeCode, orderSymbol, tradeSymbol, execId);
		UFC::BufferedLog::Printf(" TOrderStore::ProcessFilledMsg() %s",
								 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
		UFC::BufferedLog::FlushToFile();
		return IsSuccess;
	}

	//Get Order Root
	TExecution *OrderRootPtr = FindOrderRootByOrderId_NID(RptOrderId, RptNID, NewRptPtr->GetRootNID());
	bool IsOrderStatusChanged = false;
	if (OrderRootPtr == 0)
	{   //Create a New Order Root
		IsOrderStatusChanged = true;
		OrderRootPtr = new TExecution(NewRptPtr);
		OrderRootPtr->SetRoot(true);
		OrderRootPtr->SetLocalMessageTime(LocalMessageTimeStr);
		OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		AddOrderRoot(OrderRootPtr);
		InsertOrderRootNIDIndex(OrderRootPtr);
		InsertOrderRootOrderIDIndex(OrderRootPtr);
		InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
	}
	else
	{   //Order Root Exist
		nsOrderMessageDefine::OrderStatusEnum OrderRootStatus = OrderRootPtr->GetOrderStatus();
		nsOrderMessageDefine::ExecTypeEnum RptExecType = NewRptPtr->GetExecType();
		if ((OrderRootStatus == nsOrderMessageDefine::osCanceled) ||
			((RptExecType != nsOrderMessageDefine::etOrderStatus) &&
			 (OrderRootStatus == nsOrderMessageDefine::osFilled)))
			return IsSuccess;

		UnicodeString OrderRootOrderId = OrderRootPtr->GetOrderID();
		if (((OrderRootOrderId.Length() <= 0) || (OrderRootOrderId == L"00000")) &&
			((RptOrderId.Length() > 0) && (RptOrderId != L"00000")))
		{
			OrderRootPtr->SetOrderID(RptOrderId);
			InsertOrderRootOrderIDIndex(OrderRootPtr);
		}

		if ((OrderRootStatus != NewRptPtr->GetOrderStatus()) ||
			((RptOrderStatus == nsOrderMessageDefine::osPartiallyFilled) &&
			 (NewRptPtr->GetLeavesQty() != OrderRootPtr->GetLeavesQty())))
		{
			IsOrderStatusChanged = true;
			OrderRootPtr->SetCumQty(NewRptPtr->GetCumQty());
			OrderRootPtr->SetLeavesQty(NewRptPtr->GetLeavesQty());
			OrderRootPtr->SetLastQty(NewRptPtr->GetLastQty());
			OrderRootPtr->SetLegQty1(NewRptPtr->GetLegQty1());
			OrderRootPtr->SetLegQty2(NewRptPtr->GetLegQty2());
			OrderRootPtr->SetLastPx(NewRptPtr->GetLastPx());
			OrderRootPtr->SetLegPx1(NewRptPtr->GetLegPx1());
			OrderRootPtr->SetLegPx2(NewRptPtr->GetLegPx2());
			OrderRootPtr->SetAvgPx(NewRptPtr->GetAvgPx());
			if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)
			{
				if (OrderRootPtr->GetTimeOrderIDKey().Length() > 0)
				{
					RemoveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
					RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
				}

				UnicodeString EmptyStr = L"";
				OrderRootPtr->SetLocalMessageTime(EmptyStr);
				OrderRootPtr->SetTimeOrderIDKey(EmptyStr);
			}

			if (OrderRootPtr->GetLocalMessageTime().Length() <= 0)
				OrderRootPtr->SetLocalMessageTime(NewRptPtr->GetLocalMessageTime());

			if (OrderRootPtr->GetTimeOrderIDKey().Length() <= 0)
			{
				OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
				InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
			}
		}
		String RptMsg = UTF8String( NewRptPtr->GetText());

		OrderRootPtr->SetOrderStatus(RptOrderStatus);
		OrderRootPtr->SetStatusCode(NewRptPtr->GetStatusCode());
		OrderRootPtr->SetText( RptMsg );
	}  //if (OrderRootPtr == 0)

	if (NewRptPtr->GetLeavesQty() <= 0)
		RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
	else
		InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);

	IsSuccess = true;
	AddOrderMessage(NewRptPtr);
	InsertOrderMessageExecIDIndex(NewRptPtr);
	NewRptPtr->SetRootNID(OrderRootPtr->GetNID());
	OrderRootPtr->InsertOrderMessage(NewRptPtr);
	if (IsOrderStatusChanged)
	{
		InsertFilledOrderRootTimeOrderIDIndex(OrderRootPtr);
		AddPositionRecFillQty(NewRptPtr);
		if (OnFilledReply != 0)
			OnFilledReply(this, exchangeCode, orderSymbol, RptOrderId, OrderRootPtr->GetSide(),  OrderRootPtr->GetLastPx(), OrderRootPtr->GetLastQty(),RptOrderStatus);

		InsertFillMessageSymbolIndex(NewRptPtr);
		if (NewRptPtr->GetSymbol()->IsMultileg())
		{
			String curSymbolCode = OrderRootPtr->GetSymbol()->GetSymbol(1);
			SubscribeSymbol(exchangeCode, curSymbolCode, false, true);  //need not IncreaseCount, SubscribeAnyway

			ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), exchangeCode, curSymbolCode,
									OrderRootPtr->GetLegSide1(), OrderRootPtr->GetPrice());
			ExecutePositionChangeListener(OrderRootPtr->GetMarket(), exchangeCode, curSymbolCode,
										  OrderRootPtr->GetLegSide1(), OrderRootPtr->GetLegPx1(), OrderRootPtr->GetLegQty1());

			curSymbolCode = OrderRootPtr->GetSymbol()->GetSymbol(2);
			SubscribeSymbol(exchangeCode, curSymbolCode, false, true);  //need not IncreaseCount, SubscribeAnyway
			ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), exchangeCode, curSymbolCode,
									OrderRootPtr->GetLegSide2(), OrderRootPtr->GetPrice());
			ExecutePositionChangeListener(OrderRootPtr->GetMarket(), exchangeCode, curSymbolCode,
										  OrderRootPtr->GetLegSide2(), OrderRootPtr->GetLegPx2(), OrderRootPtr->GetLegQty2());
		}
		else
		{
			SubscribeSymbol(exchangeCode, orderSymbol, false, true);  //need not IncreaseCount, SubscribeAnyway
			ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), exchangeCode, orderSymbol,
									OrderRootPtr->GetSide(), OrderRootPtr->GetPrice());
			ExecutePositionChangeListener(OrderRootPtr->GetMarket(), exchangeCode, orderSymbol,
										  OrderRootPtr->GetSide(), OrderRootPtr->GetLastPx(), OrderRootPtr->GetLastQty());
		}  //if (OrderRootPtr->GetSymbol()->IsMultileg())

		if (FIsAutoPositionEffect)
		{
			int processPositionCode = 0;
			String processPositionMsg = L"";
			s888::CContractPosition *positionPtr = s888::gTradingObjsPool.FindAccountContractPosition(FID, exchangeCode, orderSymbol, s888::constCreate, processPositionCode, processPositionMsg, s888::rwLockForWrite);
			if (positionPtr != 0)
			{
				positionPtr->LockForWrite();
				if (NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peOpen)
				{
					if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy)
						positionPtr->AddBuyFillOpenQuantity(NewRptPtr->GetLastQty(), s888::rwNotLock);
					else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell)
						positionPtr->AddSellFillOpenQuantity(NewRptPtr->GetLastQty(), s888::rwNotLock);
				}
				else if (NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peClose)
				{
					if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy)
					{
						positionPtr->AddBuyFillCloseQuantity(NewRptPtr->GetLastQty(), s888::rwNotLock);
						positionPtr->SubstractBuyOrderCloseQuantity(NewRptPtr->GetLastQty(), s888::rwNotLock);
					}
					else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell)
					{
						positionPtr->AddSellFillCloseQuantity(NewRptPtr->GetLastQty(), s888::rwNotLock);
						positionPtr->SubstractSellOrderCloseQuantity(NewRptPtr->GetLastQty(), s888::rwNotLock);
					}
				}
				else if (NewRptPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)
				{
					if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy)
					{
						positionPtr->AddBuyFillCloseTodayQuantity(NewRptPtr->GetLastQty(), s888::rwNotLock);
						positionPtr->SubstractBuyOrderCloseTodayQuantity(NewRptPtr->GetLastQty(), s888::rwNotLock);
					}
					else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell)
					{
						positionPtr->AddSellFillCloseTodayQuantity(NewRptPtr->GetLastQty(), s888::rwNotLock);
						positionPtr->SubstractSellOrderCloseTodayQuantity(NewRptPtr->GetLastQty(), s888::rwNotLock);
					}
				}
				logMessage.printf(L" Filled Report[%s] NID[%lld] [%s-%s(%s)] side[%s] pe[%s] LastQty[%d] LastPx[%15.6lf].",
								  RptOrderId, RptNID, exchangeCode, orderSymbol, tradeSymbol,
								  s888::DescribeBuySell(OrderRootPtr->GetSide()),
								  s888::DescribePositionEffect(OrderRootPtr->GetPositionEffect()),
								  OrderRootPtr->GetLastQty(), OrderRootPtr->GetLastPx());
				UFC::BufferedLog::Printf(" TOrderStore::ProcessFilledMsg() %s",
										 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
				positionPtr->DumpData(s888::constFlush, s888::rwNotLock);
				positionPtr->UnlockForWrite();
			}
			else
			{
				UFC::BufferedLog::Printf(" TOrderStore::ProcessFilledMsg() Find Account Contract Position Failed. %d[%s]",
										 processPositionCode, s888::ConvertUnicodeToUFCAnsiStr(processPositionMsg).c_str());
				UFC::BufferedLog::FlushToFile();
			}  //if (positionPtr != 0)
		}  //if (FIsAutoPositionEffect)
	}  //if (IsOrderStatusChanged)
	ExecuteOrderMessageListener(OrderRootPtr);
	return IsSuccess;
}  //TOrderStore::ProcessFilledMsg()
//---------------------------------------------------------------------------
String __fastcall TOrderStore::UTF8String( const String& CStr )
{
	AnsiString cstr( CStr );

	if( IsUTF8String( cstr.c_str() ) == true )
		return UTF8ToString( cstr.c_str() );
	return String( CStr );
}
//---------------------------------------------------------------------------
String __fastcall TOrderStore::UTF8String( const char* CStr )
{
	if( IsUTF8String( CStr ) == true )
		return UTF8ToString( CStr );
	return String( CStr );
}
//------------------------------------------------------------------------------
bool TOrderStore::ProcessRejectedMsg(TExecution *NewRptPtr,
									 nsOrderMessageDefine::OrderStatusEnum RptOrderStatus,
									 UFCType::Int64 RptNID, const UnicodeString& RptOrderId,
									 const UnicodeString& LocalMessageTimeStr)
{
	bool IsSuccess = false;
	String IdStr;
	if ((RptOrderId.Length() <= 0) || (RptOrderId == L"00000"))
		IdStr.printf(L"%lld", RptNID);
	else
		IdStr = RptOrderId;

	String TimeOrderIDKey = LocalMessageTimeStr + IdStr;
	NewRptPtr->SetTimeOrderIDKey(TimeOrderIDKey);
	//Get Order Root
	TExecution *OrderRootPtr = FindOrderRootByOrderId_NID(RptOrderId, RptNID, NewRptPtr->GetRootNID());
	bool IsOrderStatusChanged = false;
	bool isOrderRootExist = false;
	int leavesQty = 0;
	nsOrderMessageDefine::OrderStatusEnum OrderRootStatus = nsOrderMessageDefine::osNone;
	if (OrderRootPtr == 0)
	{   //Create a New Order Root
		IsOrderStatusChanged = true;
		OrderRootPtr = new TExecution(NewRptPtr);
		OrderRootPtr->SetRoot(true);
		OrderRootPtr->SetLocalMessageTime(LocalMessageTimeStr);
		OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		OrderRootPtr->SetExecID(NewRptPtr->GetExecID());
		AddOrderRoot(OrderRootPtr);
		InsertOrderRootNIDIndex(OrderRootPtr);
		InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
		InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
		if ((RptOrderId.Length() > 0) && (RptOrderId != L"00000"))
			InsertOrderRootOrderIDIndex(OrderRootPtr);
	}
	else
	{   //Order Root Exist
		isOrderRootExist = true;
		leavesQty = OrderRootPtr->GetLeavesQty();
		NewRptPtr->SetMarket(OrderRootPtr->GetMarket());
		NewRptPtr->SetSymbol(OrderRootPtr->GetSymbol()->GetOriginalSymbol());
		NewRptPtr->SetSide(OrderRootPtr->GetSide());
		NewRptPtr->SetPrice(OrderRootPtr->GetPrice());
		NewRptPtr->SetOrderQty(OrderRootPtr->GetOrderQty());
		NewRptPtr->SetOrderType(OrderRootPtr->GetOrderType());
		NewRptPtr->SetTimeInForce(OrderRootPtr->GetTimeInForce());
		NewRptPtr->SetPositionEffect(OrderRootPtr->GetPositionEffect());
		OrderRootStatus = OrderRootPtr->GetOrderStatus();
		if (OrderRootStatus != nsOrderMessageDefine::osRejected)
		{
			IsOrderStatusChanged = true;
			if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)
			{
				if (OrderRootPtr->GetTimeOrderIDKey().Length() > 0)
				{
					RemoveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
					RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
				}

				String EmptyStr = L"";
				OrderRootPtr->SetLocalMessageTime(EmptyStr);
				OrderRootPtr->SetTimeOrderIDKey(EmptyStr);
			}  //if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)

			if (OrderRootPtr->GetLocalMessageTime().Length() <= 0)
				OrderRootPtr->SetLocalMessageTime(NewRptPtr->GetLocalMessageTime());

			if (OrderRootPtr->GetTimeOrderIDKey().Length() <= 0)
			{
				OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
				InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
				InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
			}
		}  //if (OrderRootStatus != nsOrderMessageDefine::osRejected)

		int MsgStatusCode = NewRptPtr->GetStatusCode();
		String ReportText = UTF8String( NewRptPtr->GetText() );
		if ((OrderRootStatus != nsOrderMessageDefine::osCanceled) &&
			(OrderRootStatus != nsOrderMessageDefine::osFilled) &&
			(OrderRootStatus != nsOrderMessageDefine::osRejected) &&
			(NewRptPtr->GetCxlRejResponseTo() == nsOrderMessageDefine::crrNew) &&
			(ReportText.Pos(L"process not exists") <= 0))
			OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osRejected);

		OrderRootPtr->SetStatusCode(MsgStatusCode);
		OrderRootPtr->SetText(ReportText);
	}  //if (OrderRootPtr == 0)

	IsSuccess = true;
	AddOrderMessage(NewRptPtr);
	InsertOrderMessageExecIDIndex(NewRptPtr);
	NewRptPtr->SetRootNID(OrderRootPtr->GetNID());
	OrderRootPtr->InsertOrderMessage(NewRptPtr);
	String exchangeCode = OrderRootPtr->GetExchangeCode();
	String symbol = OrderRootPtr->GetSymbol()->GetSymbol(0);
	String tradeSymbol = OrderRootPtr->GetTradeSymbol();
	if (IsOrderStatusChanged) {
		if ((OrderRootStatus != nsOrderMessageDefine::osNew) ||
			(OrderRootStatus != nsOrderMessageDefine::osPartiallyFilled))
		{
			if (!OrderRootPtr->GetSymbol()->IsMultileg())
			{
				ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), exchangeCode, symbol,
										OrderRootPtr->GetSide(), OrderRootPtr->GetPrice());
			}  //if (OrderRootPtr->GetSymbol()->IsMultileg())
		}

		if (FIsAutoPositionEffect && (NewRptPtr->GetCxlRejResponseTo() != nsOrderMessageDefine::crrCancel) &&
			isOrderRootExist && (leavesQty > 0) &&
			((OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peClose) ||
			 (OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)))
		{
			String positionMsg = L"";
			s888::CTradingAccount *accountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex(FID, s888::rwLockForRead);
			if (accountPtr != 0)
			{
				s888::CContractPosition *positionPtr = accountPtr->FindContractPositionFromSymbolIndex(exchangeCode, symbol, s888::rwLockForRead);
				if (positionPtr != 0)
				{
					positionPtr->LockForWrite();
					positionPtr->SubstractOrderQuantity(OrderRootPtr->GetPositionEffect(), OrderRootPtr->GetSide(), leavesQty, s888::rwNotLock);
					String logMessage;
					logMessage.printf(L" Reject Report[%s] NID[%lld] [%s-%s(%s)] side[%s] pe[%s] Qty[%d] leavesQty[%d].",
									  RptOrderId, RptNID, exchangeCode, symbol, tradeSymbol,
									  s888::DescribeBuySell(OrderRootPtr->GetSide()),
									  s888::DescribePositionEffect(OrderRootPtr->GetPositionEffect()),
									  OrderRootPtr->GetOrderQty(), leavesQty);
					UFC::BufferedLog::Printf(" TOrderStore::ProcessRejectedMsg(() %s",
											 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
					positionPtr->DumpData(s888::constFlush, s888::rwNotLock);
					positionPtr->UnlockForWrite();
				}
				else
					positionMsg.printf(L"Account[%s] Contract Position[%s-%s(%s)] not found.", FID, exchangeCode, symbol, tradeSymbol);
			}
			else
				positionMsg.printf(L"Account[%s] not Found.", FID);

			if (positionMsg.Length() > 0)
			{
				UFC::BufferedLog::Printf(" TOrderStore::ProcessRejectedMsg() %s", s888::ConvertUnicodeToUFCAnsiStr(positionMsg).c_str());
				UFC::BufferedLog::FlushToFile();
			}
		}  //if (FIsAutoPositionEffect && (leavesQty > 0) &&

	}

	ExecuteOrderMessageListener(OrderRootPtr);
	return IsSuccess;
}  //TOrderStore::ProcessRejectedMsg()

//------------------------------------------------------------------------------
bool TOrderStore::InsertOrderMessage(TExecution* OrderMessagePtr)
{
	bool IsSuccess = false;
	if (OrderMessagePtr == 0) return IsSuccess;

	nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus = OrderMessagePtr->GetOrderStatus();
	UFCType::Int64 MsgNID = OrderMessagePtr->GetNID();
	UnicodeString LocalMessageTime = OrderMessagePtr->GetLocalMessageTime();
	UnicodeString MsgOrderId = OrderMessagePtr->GetOrderID();

	switch(MsgOrderStatus)
	{
		case nsOrderMessageDefine::osPendingNew:
			IsSuccess = ProcessPendingNewMsg(OrderMessagePtr, MsgOrderStatus,
											 MsgNID, MsgOrderId, LocalMessageTime);
			break;
		case nsOrderMessageDefine::osNew:
			IsSuccess = ProcessNewMsg(OrderMessagePtr, MsgOrderStatus,
									  MsgNID, MsgOrderId, LocalMessageTime);
			break;
		case nsOrderMessageDefine::osReplaced:
			IsSuccess = ProcessReplacedMsg(OrderMessagePtr, MsgOrderStatus,
										   MsgNID, MsgOrderId, LocalMessageTime);
			break;
		case nsOrderMessageDefine::osCanceled:
			IsSuccess = ProcessCanceledMsg(OrderMessagePtr, MsgOrderStatus,
										   MsgNID, MsgOrderId, LocalMessageTime);
			break;
		case nsOrderMessageDefine::osPartiallyFilled:
		case nsOrderMessageDefine::osFilled:
			IsSuccess = ProcessFilledMsg(OrderMessagePtr, MsgOrderStatus,
										 MsgNID, MsgOrderId, LocalMessageTime);
			break;
		case nsOrderMessageDefine::osRejected:
			IsSuccess = ProcessRejectedMsg(OrderMessagePtr, MsgOrderStatus,
										   MsgNID, MsgOrderId, LocalMessageTime);
			break;
		case nsOrderMessageDefine::osPendingReplace:
		case nsOrderMessageDefine::osPendingCancel:
		default:
			IsSuccess = false;
	}  //switch(MsgOrderStatus)

	return IsSuccess;
}  //TOrderStore::InsertOrderMessage()
//------------------------------------------------------------------------------
bool TOrderStore::IsFirstTime() {return FIsFirstTime;}
UnicodeString TOrderStore::GetRecoverStartTime() {return FRecoverStartTime;}
//------------------------------------------------------------------------------
void TOrderStore::SetCAListener( ICACheckListener* Listener )
{
	FAdapter->SetCAListener( Listener );
}
//------------------------------------------------------------------------------
ICACheckListener* TOrderStore::GetCAListener( void )
{
	return FAdapter->GetCAListener( );
}
//------------------------------------------------------------------------------
void TOrderStore::OnMarginsReply( String Text )
{
	if( FOnMarginsQueryReply != NULL )
		FOnMarginsQueryReply( this, Text );
}
//------------------------------------------------------------------------------
void TOrderStore::OnServerReply( int RTT )
{
	if( FOnServerAck != NULL )
		FOnServerAck( this, RTT );
}
//------------------------------------------------------------------------------
void TOrderStore::QueryMargins( const String& Query )
{
	FAdapter->QueryMargins( Query );
}
//------------------------------------------------------------------------------
void TOrderStore::SendMessageToServer( const String& Msg )
{
	FAdapter->SendMessageToServer( Msg );
}  //TOrderStore::SendMessageToServer()
//------------------------------------------------------------------------------
int TOrderStore::GetConnectionID()
{
	if (FAdapter != 0)
		return FAdapter->GetConnectionID();
	else
		return 0;
}  //TOrderStore::GetConnectionID()
//------------------------------------------------------------------------------
void TOrderStore::ExecuteOnDisconnect()
{
	UFC::BufferedLog::DebugPrintf(" Order Store Disconnect");
	UFC::BufferedLog::FlushToFile();
	if (OnDisconnect != 0) OnDisconnect(this);
}  //TOrderStore::ExecuteOnDisconnect()
//------------------------------------------------------------------------------
int TOrderStore::GetBuyHoldQuantity(String ExchangeCode, String SymbolCode)
{
	int resultData = 0;
	if (SymbolCode.Length() <= 0) return resultData;
	s888::CPositionStatisticRecord *positionStatisticRecPtr = FindOrCreatePosStatisticRec(ExchangeCode, SymbolCode);
	if (positionStatisticRecPtr != 0)
		resultData = positionStatisticRecPtr->GetBuyHoldOpenQuantity();
	return resultData;
}  //TOrderStore::GetBuyHoldQuantity()
//------------------------------------------------------------------------------
int TOrderStore::GetSellHoldQuantity(String ExchangeCode, String SymbolCode)
{
	int resultData = 0;
	if (SymbolCode.Length() <= 0) return resultData;
	s888::CPositionStatisticRecord *positionStatisticRecPtr = FindOrCreatePosStatisticRec(ExchangeCode, SymbolCode);
	if (positionStatisticRecPtr != 0)
		resultData = positionStatisticRecPtr->GetSellHoldOpenQuantity();
	return resultData;
}  //TOrderStore::GetSellHoldQuantity()
//------------------------------------------------------------------------------
bool TOrderStore::IsStrategyExists( const UFC::AnsiString& Symbol, SideEnum Side )
{
	UFC::AnsiString Key;
	Key.Printf( "%s_%c", Symbol, Side == sBuy?'B':'S' );

	if( FStrategyListenerTable.IsExists( Key ) )
		return true;
	return false;
}  //TOrderStore::IsStrategyExists()
//------------------------------------------------------------------------------
void TOrderStore::AddStrategyOrderListener( const UFC::AnsiString& Symbol, SideEnum Side, IStrategyOrderListener* Listener )
{
	UFC::AnsiString Key;
	Key.Printf( "%s_%c", Symbol, Side == sBuy?'B':'S' );

	if( !FStrategyListenerTable.IsExists( Key ) )
	{
		FStrategyListenerTable.Add( Key, Listener );
	}
}  //TOrderStore::AddStrategyOrderListener()
//------------------------------------------------------------------------------
void TOrderStore::ExecuteStrategyOrderListener( const UFC::AnsiString& Symbol, SideEnum Side, double LastPx, int LastQty, int LeavesQty )
{
	UFC::AnsiString Key;
	Key.Printf( "%s_%c", Symbol, Side == sBuy ? 'B' : 'S' );

	if( FStrategyListenerTable.IsExists( Key ) )
	{
		IStrategyOrderListener* Listener = FStrategyListenerTable.GetObjectByKey( Key );
		if( Listener != NULL )
		{
			Listener->OnFill( Symbol, LastPx, LastQty, LeavesQty );
		}
	}
}  //TOrderStore::ExecuteStrategyOrderListener()
//------------------------------------------------------------------------------
void TOrderStore::RemoveStrategyOrderListener( const UFC::AnsiString& Symbol, SideEnum Side )
{
	UFC::AnsiString Key;
	Key.Printf( "%s_%c", Symbol, Side == sBuy?'B':'S' );

	if( FStrategyListenerTable.IsExists( Key ) )
	{
		FStrategyListenerTable.DeleteByKey( Key );
	}
}  //TOrderStore::RemoveStrategyOrderListener()
//------------------------------------------------------------------------------
int TOrderStore::RecoverActiveStrategyReport(const UFC::AnsiString& StrategyName)
{
	int StrategyReportCount = 0;
	if (OnStrategyReport == 0) return StrategyReportCount;

	UnicodeString TargetStrategyName = UTF8ToUnicodeString(StrategyName);
	FAdapter->LockStrategyReport();
	CStringTExecutionMap::iterator it;
	for (it = FOrderRootOrderIDIndex.begin(); it != FOrderRootOrderIDIndex.end(); it++)
	{
		TExecution *CurOrderRootPtr = it->second;
		nsOrderMessageDefine::OrderStatusEnum OrderStatus = CurOrderRootPtr->GetOrderStatus();
		if ((OrderStatus != nsOrderMessageDefine::osNew) &&
			(OrderStatus != nsOrderMessageDefine::osReplaced) &&
			(OrderStatus != nsOrderMessageDefine::osPartiallyFilled))
			continue;

		UnicodeString CurStrategyName = CurOrderRootPtr->GetStrategyName();
		if( StrategyName != "Admin" )
		{
			if ((CurStrategyName.Length() <= 0) || (CurStrategyName != TargetStrategyName))
			continue;
		}
		StrategyReportCount++;
		UnicodeString NameValueStr = CurOrderRootPtr->EncodeNameValueString(0);
		char *NameValueBuffer = new char[1000];
		int BufferLength = UnicodeToUtf8(NameValueBuffer, 1000, NameValueStr.c_str(), NameValueStr.Length());
		NameValueBuffer[BufferLength] = 0;
		UFC::AnsiString AnsiNameValueStr = NameValueBuffer;
		delete [] NameValueBuffer;
		TStrategyReport *CurStrategyReportPtr = new TStrategyReport(StrategyName, AnsiNameValueStr, CurOrderRootPtr->GetNID());
		FAdapter->PushStrategyReport(CurStrategyReportPtr);
//		OnStrategyReport(AnsiNameValueStr, StrategyName, CurOrderRootPtr->GetNID());
	}  //for (it = FOrderRootOrderIDIndex.begin(); it != FOrderRootOrderIDIndex.end(); it++)
	FAdapter->EnableStrategyReportTimer();
	FAdapter->UnlockStrategyReport();
	return StrategyReportCount;
}  //TOrderStore::RecoverStrategyReport()
//------------------------------------------------------------------------------
wchar_t TOrderStore::ConvertNumberToChar(int Value)
{
	wchar_t numberChar;
	switch (Value)
	{
	case  0: numberChar = L'A'; break;
	case  1: numberChar = L'B'; break;
	case  2: numberChar = L'C'; break;
	case  3: numberChar = L'D'; break;
	case  4: numberChar = L'E'; break;
	case  5: numberChar = L'F'; break;
	case  6: numberChar = L'G'; break;
	case  7: numberChar = L'H'; break;
	case  8: numberChar = L'I'; break;
	case  9: numberChar = L'J'; break;
	case 10: numberChar = L'K'; break;
	case 11: numberChar = L'L'; break;
	case 12: numberChar = L'M'; break;
	case 13: numberChar = L'N'; break;
	case 14: numberChar = L'O'; break;
	case 15: numberChar = L'P'; break;
	case 16: numberChar = L'Q'; break;
	case 17: numberChar = L'R'; break;
	case 18: numberChar = L'S'; break;
	case 19: numberChar = L'T'; break;
	case 20: numberChar = L'U'; break;
	case 21: numberChar = L'V'; break;
	case 22: numberChar = L'W'; break;
	case 23: numberChar = L'X'; break;
	case 24: numberChar = L'Y'; break;
	case 25: numberChar = L'Z'; break;
	case 26: numberChar = L'a'; break;
	case 27: numberChar = L'b'; break;
	case 28: numberChar = L'c'; break;
	case 29: numberChar = L'd'; break;
	case 30: numberChar = L'e'; break;
	case 31: numberChar = L'f'; break;
	case 32: numberChar = L'g'; break;
	case 33: numberChar = L'h'; break;
	case 34: numberChar = L'i'; break;
	case 35: numberChar = L'j'; break;
	case 36: numberChar = L'k'; break;
	case 37: numberChar = L'l'; break;
	case 38: numberChar = L'm'; break;
	case 39: numberChar = L'n'; break;
	case 40: numberChar = L'o'; break;
	case 41: numberChar = L'p'; break;
	case 42: numberChar = L'q'; break;
	case 43: numberChar = L'r'; break;
	case 44: numberChar = L's'; break;
	case 45: numberChar = L't'; break;
	case 46: numberChar = L'u'; break;
	case 47: numberChar = L'v'; break;
	case 48: numberChar = L'w'; break;
	case 49: numberChar = L'x'; break;
	case 50: numberChar = L'y'; break;
	case 51: numberChar = L'z'; break;
	case 52: numberChar = L'0'; break;
	case 53: numberChar = L'1'; break;
	case 54: numberChar = L'2'; break;
	case 55: numberChar = L'3'; break;
	case 56: numberChar = L'4'; break;
	case 57: numberChar = L'5'; break;
	case 58: numberChar = L'6'; break;
	case 59: numberChar = L'7'; break;
	default: numberChar = L'8';
	}
	return numberChar;
}  //TOrderStore::ConvertNumberToChar()
//------------------------------------------------------------------------------
String TOrderStore::GenerateNewIDPrefix()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	wchar_t HourChar = ConvertNumberToChar(SysTime.wHour);
	wchar_t MinuteChar = ConvertNumberToChar(SysTime.wMinute);
	wchar_t SecondChar = ConvertNumberToChar(SysTime.wSecond);
	String newIDPrefix;
	newIDPrefix.printf(L"%c%c%c", HourChar, MinuteChar, SecondChar);
	return newIDPrefix;
}  //TOrderStore::GenerateNewIDPrefix()
//------------------------------------------------------------------------------
String TOrderStore::GenerateNewOrderID()
{
	int newIdSequence = ++FTestOrderIdSeq;
	if (newIdSequence > 99)
	{
		FTestOrderIdSeq = 0;
		newIdSequence = FTestOrderIdSeq;
	}
	String newOrderID;
	newOrderID.printf(L"%s%02d", GenerateNewIDPrefix(), newIdSequence);
	return newOrderID;
}  //TOrderStore::GenerateNewOrderID()
//------------------------------------------------------------------------------
String TOrderStore::GenerateNewExecID()
{
	int newIdSequence = ++FTestExecIdSeq;
	if (newIdSequence > 9999)
	{
		FTestOrderIdSeq = 0;
		newIdSequence = FTestExecIdSeq;
	}
	String newExecID;
	newExecID.printf(L"%s%04d", GenerateNewIDPrefix(), newIdSequence);
	return newExecID;
}  //TOrderStore::GenerateNewExecID()
//------------------------------------------------------------------------------
String TOrderStore::GenerateTestOrderTime()
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	String orderTiime;
	orderTiime.printf(L"%02d:%02d:%02d.%03d", SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds);
	return orderTiime;
}  //TOrderStore::GenerateTestOrderTime()
//------------------------------------------------------------------------------
int TOrderStore::ProdIDMargin( const UFC::AnsiString& ProdID )
{
	if( ProdID == "TXF" )
		return 200000;
	else if( ProdID == "TMF" )
		return 10000;
	else if( ProdID == "MXF" || ProdID == "MX1" || ProdID == "MX2" ||ProdID == "MX4" ||ProdID == "MX5")
		return 50000;
	else if( ProdID == "EXF" )///< 電子期貨
		return  80000;
	else if( ProdID == "FXF" )///< 金融期貨
		return  45000;
	else if( ProdID == "T5F" )///< 台灣50期貨
		return  32000;
	else if( ProdID == "XIF" )///< 非金電期貨
		return  50000;
	else
		return  20000;
}
//------------------------------------------------------------------------------
bool TOrderStore::LotsControl( const UFC::AnsiString& ProdID, double Profit, int BFilled, int SFilled, int WBuy, int WSell  )
{
	int FillPos = BFilled - SFilled;
	int MarginPerLot = ProdIDMargin( ProdID );
//	int LoseLots = ( Profit > 0 )? 0 : (Profit/MarginPerLot)+1;
//	int MaxLots = FMaxLots -LoseLots;
	int RiskLots;

	if( FillPos < 0 ) ///< Short position
	{
		if( WBuy > IntABS( FillPos ) + WSell )
			RiskLots = WBuy;
		else
			RiskLots = IntABS( FillPos ) + WSell;
	}
	else ///< Long position
	{
		if( WBuy + FillPos > WSell )
			RiskLots = WBuy + FillPos;
		else
			RiskLots = WSell;
	}
	if( RiskLots > FMaxLots )
		return false;
	return true;
}
//------------------------------------------------------------------------------
void TOrderStore::AutoPositionEffect( TExecutionReportMessage *NewRptPtr, s888::CPositionStatisticRecord *posStatisticRecPtr )
{
	nsOrderMessageDefine::PositionEffectEnum positionEffect = NewRptPtr->GetPositionEffect();

	if( (positionEffect == nsOrderMessageDefine::peAuto) || (positionEffect == nsOrderMessageDefine::peAutoToday))
	{
		if (posStatisticRecPtr != NULL )
		{
			nsOrderMessageDefine::SideEnum newRptSide = NewRptPtr->GetSide();
			int openInterestQty = 0;

			if( newRptSide == nsOrderMessageDefine::sBuy )
				openInterestQty = posStatisticRecPtr->GetSellOpenInterestQuantity(s888::rwNotLock);
			else if (newRptSide == nsOrderMessageDefine::sSell)
				openInterestQty = posStatisticRecPtr->GetBuyOpenInterestQuantity(s888::rwNotLock );
			if( openInterestQty > 0 )
			{
				if (openInterestQty < NewRptPtr->GetOrderQty())
					NewRptPtr->SetPositionEffect( nsOrderMessageDefine::peOpen );
				else
				{
					if( positionEffect == nsOrderMessageDefine::peAuto )
						NewRptPtr->SetPositionEffect(nsOrderMessageDefine::peClose);
					else
						NewRptPtr->SetPositionEffect(nsOrderMessageDefine::peDayTrade);
				}
			}
			else
				NewRptPtr->SetPositionEffect(nsOrderMessageDefine::peOpen);
		}
		else ///< No position, set to Open.
			NewRptPtr->SetPositionEffect(nsOrderMessageDefine::peOpen);
	}
}
//------------------------------------------------------------------------------
void TOrderStore::AppendTestExecRptToList(TExecutionReportMessage *NewRptPtr)
{
	UFC::AnsiString ansiExchangeCode( NewRptPtr->GetExchangeCode() );
	UFC::AnsiString ansiTradeSymbol( NewRptPtr->GetSymbol() );
	UFC::AnsiString ProdID( NewRptPtr->GetSymbol(), 3 );
	String          exchangeCode( ansiExchangeCode.c_str() );
	String          symbol( ansiTradeSymbol.c_str() );

	SubscribeSymbol( exchangeCode, symbol, true, false);  //need increase Count, Subscribe if need
	nsOrderMessageDefine::SideEnum newRptSide = NewRptPtr->GetSide();
	nsOrderMessageDefine::PositionEffectEnum positionEffect = NewRptPtr->GetPositionEffect();
	s888::CPositionStatisticRecord *posStatisticRecPtr = FindOrCreatePosStatisticRec(exchangeCode, symbol);
	int processPositionCode = 0;
	String processPositionMsg = L"";
	s888::CContractPosition *positionPtr = s888::gTradingObjsPool.FindAccountContractPosition( FID, exchangeCode, symbol, s888::constCreate, processPositionCode, processPositionMsg, s888::rwLockForWrite);
	int buyOpenInterestQty = 0;
	int sellOpenInterestQty = 0;
	int WorkingBuy = 0;
	int WorkingSell = 0;
	double bestBuyPrice1 = 0.0;
	int bestBuyQty1 = 0;
	double bestSellPrice1 = 0.0;
	double Profit = 0.0;
	int bestSellQty1 = 0;

	WorkingQty( exchangeCode, symbol, WorkingBuy, WorkingSell );
	if( posStatisticRecPtr != NULL )
	{
		posStatisticRecPtr->LockForRead();
		buyOpenInterestQty = posStatisticRecPtr->GetBuyOpenInterestQuantity(s888::rwNotLock);
		sellOpenInterestQty = posStatisticRecPtr->GetSellOpenInterestQuantity(s888::rwNotLock);
		bestBuyPrice1 = posStatisticRecPtr->GetBestBuyPrice1(s888::rwNotLock);
		bestBuyQty1 = posStatisticRecPtr->GetBestBuyQuantity1(s888::rwNotLock);
		bestSellPrice1 = posStatisticRecPtr->GetBestSellPrice1(s888::rwNotLock);
		bestSellQty1 = posStatisticRecPtr->GetBestSellQuantity1(s888::rwNotLock);
		Profit = posStatisticRecPtr->GetFloatingProfit(s888::rwNotLock);
		posStatisticRecPtr->UnlockForRead();
	}
	///< Auto position
	AutoPositionEffect( NewRptPtr, posStatisticRecPtr );

	UFC::AnsiString ansiOrigOrderID = NewRptPtr->GetOrderID();
	UFC::AnsiString ansiOrderID = s888::ConvertUnicodeToUFCAnsiStr(GenerateNewOrderID());
	UFC::AnsiString ansiExecID = s888::ConvertUnicodeToUFCAnsiStr(GenerateNewExecID());
	UFC::AnsiString ansiOrderTime = s888::ConvertUnicodeToUFCAnsiStr(GenerateTestOrderTime());
	///< Pending new execution
	NewRptPtr->SetOrderID(ansiOrderID.c_str());
	NewRptPtr->SetExecID(ansiExecID.c_str());
	NewRptPtr->SetOrderStatus(nsOrderMessageDefine::osPendingNew);
	NewRptPtr->SetExecType(nsOrderMessageDefine::etPendingNew);
	NewRptPtr->SetTransactTime(ansiOrderTime.c_str());
	NewRptPtr->SetTMPExecType(nsOrderMessageDefine::tetNew);
	NewRptPtr->SetLastPx(0.0);
	NewRptPtr->SetLastQty(0);
	NewRptPtr->SetCumQty(0);
	NewRptPtr->SetLeavesQty(NewRptPtr->GetOrderQty());
	FAdapter->OnExecutionReport(NewRptPtr, edNewExecution);
	///< New execution
	ansiOrderTime = s888::ConvertUnicodeToUFCAnsiStr(GenerateTestOrderTime());
	ansiExecID = s888::ConvertUnicodeToUFCAnsiStr(GenerateNewExecID());
	NewRptPtr->SetTransactTime(ansiOrderTime.c_str());
	NewRptPtr->SetExecID( ansiExecID.c_str() );
	///< risk control
	if( newRptSide == nsOrderMessageDefine::sBuy )
		WorkingBuy = WorkingBuy + NewRptPtr->GetOrderQty();
	else
		WorkingSell = WorkingSell + NewRptPtr->GetOrderQty();
	if( LotsControl(  ProdID, Profit, buyOpenInterestQty, sellOpenInterestQty, WorkingBuy, WorkingSell ) == false )
	{
		NewRptPtr->SetExecType(nsOrderMessageDefine::etRejected);
		NewRptPtr->SetOrderStatus(nsOrderMessageDefine::osRejected);
		NewRptPtr->SetCxlRejResponseTo( nsOrderMessageDefine::crrNew );
		NewRptPtr->SetStatusCode( "98" );
		NewRptPtr->SetText( "保證金不足" );
	}
	else
	{
		NewRptPtr->SetExecType(nsOrderMessageDefine::etNew);
		NewRptPtr->SetOrderStatus(nsOrderMessageDefine::osNew);
	}
	FAdapter->OnExecutionReport(NewRptPtr, edNewExecution);

	if (posStatisticRecPtr != 0)
	{
		if (((newRptSide == nsOrderMessageDefine::sBuy) && (bestSellQty1 > 0)) ||
			((newRptSide == nsOrderMessageDefine::sSell) && (bestBuyQty1 > 0)))
			MatchOnDepthChange( NewRptPtr, bestBuyQty1, bestBuyPrice1, bestSellQty1, bestSellPrice1);
	}

	if ((NewRptPtr->GetOrderStatus() == nsOrderMessageDefine::osFilled) ||(NewRptPtr->GetOrderStatus() == nsOrderMessageDefine::osPartiallyFilled))
	{
		double origOrderPrice = NewRptPtr->GetPrice();
		NewRptPtr->SetPrice(NewRptPtr->GetLastPx());
		FAdapter->OnExecutionReport(NewRptPtr, edNewExecution);
		NewRptPtr->SetPrice(origOrderPrice);
	}
	if (NewRptPtr->GetOrderStatus() != nsOrderMessageDefine::osFilled)
		FTestExecRptList.push_back( NewRptPtr );
	else
		delete NewRptPtr;
}  //TOrderStore::AppendTestExecRptToList()
//---------------------------------------------------------------------------
void TOrderStore::AppendTestExecRptToList(TCancelOrderMessage& CancelMsg)
{
	UFC::AnsiString ansiOrderID = CancelMsg.GetOrderID();
	std::list<TExecutionReportMessage*>::iterator it;
	std::list<TExecutionReportMessage*>::iterator curIt;
	for (it = FTestExecRptList.begin(); it != FTestExecRptList.end();)
	{
		curIt = it++;
		TExecutionReportMessage *curRptPtr = *curIt;
		UFC::AnsiString curAnsiOrderID = curRptPtr->GetOrderID();
		if (curAnsiOrderID == ansiOrderID)
		{
			UFC::AnsiString ansiExchangeCode = CancelMsg.GetExchangeCode();
			UFC::AnsiString ansiSymbol = CancelMsg.GetSymbol();
			curRptPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
			curRptPtr->SetExecType(nsOrderMessageDefine::etCanceled);
			curRptPtr->SetLeavesQty(0);
			curRptPtr->SetLastQty(0);
			curRptPtr->SetLastPx(0.0);
			curRptPtr->SetLegQty1(0);
			curRptPtr->SetLegPrice1(0.0);
			curRptPtr->SetTransactTime(s888::ConvertUnicodeToUFCAnsiStr(GenerateTestOrderTime()).c_str());
			curRptPtr->SetExecID(s888::ConvertUnicodeToUFCAnsiStr(GenerateNewExecID()).c_str());
			FAdapter->OnExecutionReport(curRptPtr, edNewExecution);
			FTestExecRptList.erase(curIt);
			delete curRptPtr;
		}
	}  //for (it = FTestExecRptList.begin(); it != FTestExecRptList.end();)
}  //TOrderStore::AppendTestExecRptToList()
//---------------------------------------------------------------------------
void TOrderStore::AppendTestExecRptToList(TReplaceOrderMessage& ReplaceMsg )
{
	UFC::AnsiString ansiOrderID = ReplaceMsg.GetOrderID();
	std::list<TExecutionReportMessage*>::iterator it;
	std::list<TExecutionReportMessage*>::iterator curIt;
	for (it = FTestExecRptList.begin(); it != FTestExecRptList.end();)
	{
		curIt = it++;
		TExecutionReportMessage *curRptPtr = *curIt;
		UFC::AnsiString curAnsiOrderID = curRptPtr->GetOrderID();
		if (curAnsiOrderID == ansiOrderID)
		{
			UFC::AnsiString ansiExchangeCode = ReplaceMsg.GetExchangeCode();
			UFC::AnsiString ansiSymbol = ReplaceMsg.GetSymbol();

			curRptPtr->SetOrderStatus(nsOrderMessageDefine::osReplaced);
			curRptPtr->SetExecType(nsOrderMessageDefine::etReplaced);
			if( ReplaceMsg.GetOrderQty() == 0 )
			{
				curRptPtr->SetPrice( ReplaceMsg.GetPrice());
				if( ReplaceMsg.GetMarket() == nsOrderMessageDefine::mTWFutures || ReplaceMsg.GetMarket() == nsOrderMessageDefine::mTWOptions )
					curRptPtr->SetTMPExecType( nsOrderMessageDefine::tetPxReplaced );
			}
			else
			{
				curRptPtr->SetOrderQty( ReplaceMsg.GetOrderQty());
				if( ReplaceMsg.GetMarket() == nsOrderMessageDefine::mTWFutures || ReplaceMsg.GetMarket() == nsOrderMessageDefine::mTWOptions )
					curRptPtr->SetTMPExecType( nsOrderMessageDefine::tetReplaced );
			}
			curRptPtr->SetTransactTime(s888::ConvertUnicodeToUFCAnsiStr(GenerateTestOrderTime()).c_str());
			curRptPtr->SetExecID(s888::ConvertUnicodeToUFCAnsiStr(GenerateNewExecID()).c_str());

			FAdapter->OnExecutionReport(curRptPtr, edNewExecution);
//			FTestExecRptList.erase(curIt);
//			delete curRptPtr;
		}
	}  //for (it = FTestExecRptList.begin(); it != FTestExecRptList.end();)
}  //TOrderStore::AppendTestExecRptToList()
//---------------------------------------------------------------------------
int  TOrderStore::GetCancelCommandCount() {return FCancelCommandCount;};
int  TOrderStore::GetCancelReportCount() {return FCancelReportCount;};
void TOrderStore::SetCancelCommandCount(int CancelCommandCount) {FCancelCommandCount = CancelCommandCount;};
void TOrderStore::SetCancelReportCount(int CancelReportCount) {FCancelReportCount = CancelReportCount;};
void TOrderStore::increaseCancelCommandCount() {FCancelCommandCount++;};
void TOrderStore::increaseCancelReportCount() {FCancelReportCount++;};
//---------------------------------------------------------------------------
void TOrderStore::AppendTestExecRptToListDirectly(TExecutionReportMessage *RptPtr)
{
	FTestExecRptList.push_back(RptPtr);
}  //TOrderStore::AppendTestExecRptToListDirectly()
//---------------------------------------------------------------------------
bool TOrderStore::MatchOnFillChange( TExecutionReportMessage *CurRptPtr, int MatchQuantity, double MatchPrice )
{
	nsOrderMessageDefine::OrderStatusEnum curOrderStatus = CurRptPtr->GetOrderStatus();

	if( curOrderStatus == nsOrderMessageDefine::osNew || curOrderStatus == nsOrderMessageDefine::osPartiallyFilled )
	{
		nsOrderMessageDefine::OrderStatusEnum newOrderStatus;
		nsOrderMessageDefine::ExecTypeEnum    newExecType;
		nsOrderMessageDefine::SideEnum        curSide = CurRptPtr->GetSide();
		nsOrderMessageDefine::OrderTypeEnum   curOrderType = CurRptPtr->GetOrderType();
		double                                curOrderPx = CurRptPtr->GetPrice();
		int                                   curLeavesQty = CurRptPtr->GetLeavesQty();
		double                                lastPrice = 0.0;
		int                                   lastQty = 0;

		if ((curSide == nsOrderMessageDefine::sBuy) &&
			(((curOrderType == nsOrderMessageDefine::otLimit) && (curOrderPx > MatchPrice)) ||
			 (curOrderType == nsOrderMessageDefine::otMarket)))
		{
			lastPrice = MatchPrice;
			if( curLeavesQty > MatchQuantity )///< Match same qty as real market
			{
				lastQty        = MatchQuantity;
				newOrderStatus = nsOrderMessageDefine::osPartiallyFilled;
				newExecType    = nsOrderMessageDefine::etPartiallyFilled;
			}
			else  ///< all filled.
			{
				lastQty        = curLeavesQty;
				newOrderStatus = nsOrderMessageDefine::osFilled;
				newExecType    = nsOrderMessageDefine::etFilled;
			}
		}
		else if ((curSide == nsOrderMessageDefine::sSell) &&
				 (((curOrderType == nsOrderMessageDefine::otLimit) && (curOrderPx < MatchPrice)) ||
				  (curOrderType == nsOrderMessageDefine::otMarket)))
		{
			lastPrice = MatchPrice;
			if (curLeavesQty > MatchQuantity) ///< Match same qty as real market
			{
				lastQty        = MatchQuantity;
				newOrderStatus = nsOrderMessageDefine::osPartiallyFilled;
				newExecType    = nsOrderMessageDefine::etPartiallyFilled;
			}
			else  ///< all filled.
			{
				lastQty        = curLeavesQty;
				newOrderStatus = nsOrderMessageDefine::osFilled;
				newExecType    = nsOrderMessageDefine::etFilled;
			}
		}
		if( lastQty > 0 ) ///< Matched.
		{
			CurRptPtr->SetCumQty(CurRptPtr->GetCumQty() + lastQty);
			CurRptPtr->SetOrderQty(lastQty);
			CurRptPtr->SetLeavesQty(curLeavesQty - lastQty);
			CurRptPtr->SetLastQty(lastQty);
			CurRptPtr->SetLastPx(lastPrice);
			CurRptPtr->SetLegQty1(lastQty);
			CurRptPtr->SetLegPrice1(lastPrice);
			CurRptPtr->SetOrderStatus(newOrderStatus);
			CurRptPtr->SetExecType(newExecType);
			CurRptPtr->SetTransactTime(s888::ConvertUnicodeToUFCAnsiStr(GenerateTestOrderTime()).c_str());
			CurRptPtr->SetExecID(s888::ConvertUnicodeToUFCAnsiStr(GenerateNewExecID()).c_str());
			return true;
		}
		else  ///< Not match
			return false;
	}
	return false;
}
//---------------------------------------------------------------------------
bool TOrderStore::MatchOnDepthChange( TExecutionReportMessage *CurRptPtr, int BuyQuantity1, double BuyPrice1,int SellQuantity1, double SellPrice1)
{
	nsOrderMessageDefine::OrderStatusEnum curOrderStatus = CurRptPtr->GetOrderStatus();

	if( curOrderStatus == nsOrderMessageDefine::osNew || curOrderStatus == nsOrderMessageDefine::osPartiallyFilled )
	{
		nsOrderMessageDefine::OrderStatusEnum newOrderStatus;
		nsOrderMessageDefine::ExecTypeEnum    newExecType;
		nsOrderMessageDefine::SideEnum        curSide = CurRptPtr->GetSide();
		nsOrderMessageDefine::OrderTypeEnum   curOrderType = CurRptPtr->GetOrderType();
		double                                curOrderPx = CurRptPtr->GetPrice();
		int                                   curLeavesQty = CurRptPtr->GetLeavesQty();
		double                                lastPrice = 0.0;
		int                                   lastQty = 0;

		if ((curSide == nsOrderMessageDefine::sBuy) &&
			(((curOrderType == nsOrderMessageDefine::otLimit) && (curOrderPx >= SellPrice1)) ||
			 (curOrderType == nsOrderMessageDefine::otMarket)))
		{
			lastPrice = SellPrice1;
			if( curLeavesQty > SellQuantity1) ///< Match same qty as ask1 qty
			{
				lastQty        = SellQuantity1;
				newOrderStatus = nsOrderMessageDefine::osPartiallyFilled;
				newExecType    = nsOrderMessageDefine::etPartiallyFilled;
			}
			else   ///< all filled.
			{
				lastQty        = curLeavesQty;
				newOrderStatus = nsOrderMessageDefine::osFilled;
				newExecType    = nsOrderMessageDefine::etFilled;
			}
		}
		else if ((curSide == nsOrderMessageDefine::sSell) &&
				 (((curOrderType == nsOrderMessageDefine::otLimit) && (curOrderPx <= BuyPrice1)) ||
				  (curOrderType == nsOrderMessageDefine::otMarket)))
		{
			lastPrice = BuyPrice1;
			if (curLeavesQty > BuyQuantity1) ///< Match same qty as bid1 qty
			{
				lastQty        = BuyQuantity1;
				newOrderStatus = nsOrderMessageDefine::osPartiallyFilled;
				newExecType    = nsOrderMessageDefine::etPartiallyFilled;
			}
			else   ///< all filled.
			{
				lastQty        = curLeavesQty;
				newOrderStatus = nsOrderMessageDefine::osFilled;
				newExecType    = nsOrderMessageDefine::etFilled;
			}
		}
		if( lastQty > 0 ) ///< Matched.
		{
			CurRptPtr->SetCumQty(CurRptPtr->GetCumQty() + lastQty);
			CurRptPtr->SetOrderQty(lastQty);
			CurRptPtr->SetLeavesQty(curLeavesQty - lastQty);
			CurRptPtr->SetLastQty(lastQty);
			CurRptPtr->SetLastPx(lastPrice);
			CurRptPtr->SetLegQty1(lastQty);
			CurRptPtr->SetLegPrice1(lastPrice);
			CurRptPtr->SetOrderStatus(newOrderStatus);
			CurRptPtr->SetExecType(newExecType);
			CurRptPtr->SetTransactTime(s888::ConvertUnicodeToUFCAnsiStr(GenerateTestOrderTime()).c_str());
			CurRptPtr->SetExecID(s888::ConvertUnicodeToUFCAnsiStr(GenerateNewExecID()).c_str());
			return true;
		}
		else ///< Not match
			return false;
	}
	return false;
}
//---------------------------------------------------------------------------
void TOrderStore::OnMarketDataUpdate( OrderBookData* Msg )
{
	nsOrderMessageDefine::MarketEnum Market = static_cast<nsOrderMessageDefine::MarketEnum>(Msg->GetMarket());
	s888::CPositionStatisticRecord* posStatisticRecPtr;
	UFC::AnsiString ansiExchangeCode( Msg->GetExchange());
	UFC::AnsiString ansiSymbol( Msg->GetSymbol());
	String exchangeCode( ansiExchangeCode.c_str());
	String symbol( ansiSymbol.c_str() );
	double bid1Px  = Msg->GetBuyPrice1();
	double ask1Px  = Msg->GetSellPrice1();
	int    bid1Qty = Msg->GetBuyQty1();
	int    ask1Qty = Msg->GetSellQty1();

	if( (posStatisticRecPtr = FindOrCreatePosStatisticRec( exchangeCode, symbol )) !=  NULL )
	{
		posStatisticRecPtr->LockForRead();
		posStatisticRecPtr->SetBestBuyPrice1( bid1Px, s888::rwNotLock);
		posStatisticRecPtr->SetBestSellPrice1( ask1Px, s888::rwNotLock);
		posStatisticRecPtr->SetBestBuyQuantity1( bid1Qty, s888::rwNotLock);
		posStatisticRecPtr->SetBestSellQuantity1( ask1Qty, s888::rwNotLock);
		posStatisticRecPtr->UnlockForRead();
		if( FIsTestMode == true )
			OnDepthSimMatch( ansiExchangeCode, ansiSymbol, bid1Px, bid1Qty, ask1Px, ask1Qty );
	}
	else
	{
		UFC::BufferedLog::Printf(" TOrderStore::OnMarketDataUpdate() Insert [%s-%s] Position Statistic Record Failed.", ansiExchangeCode.c_str(), ansiSymbol.c_str() );
		UFC::BufferedLog::FlushToFile();
	}
}
//------------------------------------------------------------------------------
void TOrderStore::OnDepthSimMatch( const UFC::AnsiString& Ex, const UFC::AnsiString& Sym, double buyPrice1, int buyQty1, double sellPrice1, int sellQty1 )
{
	///< Test match
	std::list<TExecutionReportMessage*>::iterator it;
	std::list<TExecutionReportMessage*>::iterator curIt;

	for (it = FTestExecRptList.begin(); it != FTestExecRptList.end();)
	{
		curIt = it++;
		TExecutionReportMessage *curRptPtr = *curIt;
		nsOrderMessageDefine::OrderStatusEnum curOrderStatus = curRptPtr->GetOrderStatus();

		///< Process New or PartiallyFilled order only
		if ((curOrderStatus == nsOrderMessageDefine::osNew) || (curOrderStatus == nsOrderMessageDefine::osPartiallyFilled))
		{
			UFC::AnsiString curAnsiExchangeCode( curRptPtr->GetExchangeCode());
			UFC::AnsiString curAnsiSymbolCode( curRptPtr->GetSymbol());
			///< Symbol matched.
			if( curAnsiExchangeCode == Ex && curAnsiSymbolCode == Sym  )
			{
				int origLeavesQty = curRptPtr->GetLeavesQty();

				///< matching....
				MatchOnDepthChange( curRptPtr, buyQty1, buyPrice1, sellQty1, sellPrice1);
				///< Matched.
				if((curRptPtr->GetOrderStatus() == nsOrderMessageDefine::osFilled) ||
					((curRptPtr->GetOrderStatus() == nsOrderMessageDefine::osPartiallyFilled) &&
					 (origLeavesQty > curRptPtr->GetLeavesQty())))
				{
					double origOrderPrice = curRptPtr->GetPrice();

					///< add a filled execution.
					curRptPtr->SetPrice( curRptPtr->GetLastPx() );
					FAdapter->OnExecutionReport(curRptPtr, edNewExecution);
					curRptPtr->SetPrice( origOrderPrice );
				}
                ///< Filled, Remove it from order book.
				if (curRptPtr->GetOrderStatus() == nsOrderMessageDefine::osFilled)
				{
					FTestExecRptList.erase(curIt);
					delete curRptPtr;
				}
			}
		}
	}
}
//------------------------------------------------------------------------------
bool TOrderStore::AddCancelStatistic(const String& ExchangeCode, const String& Symbol, int CancelQty)
{
	bool isSuccess = false;
	String logMsg = L"";
	String logonID = FID;
	s888::CTradingAccount *tradingAccountPtr = 0;
	if (logonID.Length() > 0) tradingAccountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex(logonID, s888::rwLockForRead);
	if (tradingAccountPtr != 0)
	{
		tradingAccountPtr->LockForWrite();
		if (tradingAccountPtr->AddContractCancelStatistic(ExchangeCode, Symbol, CancelQty, 1, s888::rwNotLock))
		{
			s888::CTradingContract *contractPtr = s888::gTradingObjsPool.FindContractFromSymbolIndex(ExchangeCode, Symbol, s888::rwLockForRead);
			if (contractPtr != 0)
			{
				s888::CTradingCommodity *commodityPtr = contractPtr->GetCommodity(s888::rwLockForRead);
				if (commodityPtr != 0)
				{
					String contractCode = commodityPtr->GetContractCode(s888::rwLockForRead);
					if (tradingAccountPtr->AddCommodityCancelStatistic(ExchangeCode, contractCode, CancelQty, 1, s888::rwNotLock))
						isSuccess = true;
					else
						logMsg.printf(L"Add Trading Account[%s] Commodity[%s-%s] Cancel Statistic falied.", logonID, ExchangeCode, contractCode);
				}
				else
					logMsg.printf(L"Add Trading Account[%s] Contract[%s-%s] has no Commodity.", logonID, ExchangeCode, Symbol);
			}
			else
				logMsg.printf(L"Trading Account[%s]'s Contract[%s-%s] not Found.", logonID, ExchangeCode, Symbol);
		}
		else
			logMsg.printf(L"Add Trading Account[%s] Contract[%s-%s] Cancel Statistic falied.", logonID, ExchangeCode, Symbol);
tradingAccountPtr->DumpContractCancelStatistic(s888::rwNotLock);
tradingAccountPtr->DumpCommodityCancelStatistic(s888::rwNotLock);
		tradingAccountPtr->UnlockForWrite();
	}
	else
		logMsg.printf(L"Trading Account[%s] not Found.", logonID);
	if (logMsg.Length() > 0)
	{
		UFC::BufferedLog::DebugPrintf(" TOrderStore::AddCancelStatistic() [%s]",
									  s888::ConvertUnicodeToUFCAnsiStr(logMsg).c_str());
	}
	return isSuccess;
}  //TOrderStore::AddCancelStatistic()
//------------------------------------------------------------------------------
void TOrderStore::RecalculateCommodityCancelStatistic()
{
	String logonID = FID;
	String logMsg = L"";
	s888::CTradingAccount *tradingAccountPtr = 0;
	if (logonID.Length() > 0) tradingAccountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex(logonID, s888::rwLockForRead);
	if (tradingAccountPtr != 0)
	{
		tradingAccountPtr->LockForWrite();
		tradingAccountPtr->ClearCommodityCancelStatisticArray(s888::constNotDelete, s888::rwNotLock);
		tradingAccountPtr->ClearCommodityCancelStatisticCodeIndex(s888::constDelete, s888::rwNotLock);
		for (int i = 0; i < tradingAccountPtr->GetContractCancelStatisticArrayCount(s888::rwNotLock); i++)
		{
			logMsg = L"";
			s888::CContractCancelStatistic *contractStatisticPtr = tradingAccountPtr->GetContractCancelStatisticFromArray(i, s888::rwNotLock);
			contractStatisticPtr->LockForRead();
			String exchangeCode = contractStatisticPtr->GetExchangeCode(s888::rwNotLock);
			String symbol = contractStatisticPtr->GetSymbol(s888::rwNotLock);
			int cancelQty = contractStatisticPtr->GetCancelQuantity(s888::rwNotLock);
			int cancelCount = contractStatisticPtr->GetCancelReportCount(s888::rwNotLock);
			contractStatisticPtr->UnlockForRead();
			s888::CTradingContract *contractPtr = s888::gTradingObjsPool.FindContractFromSymbolIndex(exchangeCode, symbol, s888::rwLockForRead);
			if (contractPtr != 0)
			{
				s888::CTradingCommodity *commodityPtr = contractPtr->GetCommodity(s888::rwLockForRead);
				if (commodityPtr != 0)
				{
					String contractCode = commodityPtr->GetContractCode(s888::rwLockForRead);
					if (!tradingAccountPtr->AddCommodityCancelStatistic(exchangeCode, contractCode, cancelQty, cancelCount, s888::rwNotLock))
					logMsg.printf(L"Account[%s] Contract[%s-%s]'s Commodity[%s] not Found.", logonID, exchangeCode, symbol, contractCode);
				}
				else
					logMsg.printf(L"Account[%s] Contract[%s-%s]'s Commodity not Found.", logonID, exchangeCode, symbol);
			}
			else
				logMsg.printf(L"Account[%s] Contract[%s-%s] not Found.", logonID, exchangeCode, symbol);

			if (logMsg.Length() > 0)
				UFC::BufferedLog::DebugPrintf(" TOrderStore::RecalculateCommodityCancelStatistic() [%s]",
											  s888::ConvertUnicodeToUFCAnsiStr(logMsg).c_str());
		}  //for (int i = 0; i < tradingAccountPtr->GetContractCancelStatisticArrayCount(false); i++)
tradingAccountPtr->DumpContractCancelStatistic(s888::rwNotLock);
tradingAccountPtr->DumpCommodityCancelStatistic(s888::rwNotLock);
		tradingAccountPtr->UnlockForWrite();
	}
	else
		logMsg.printf(L"Trading Account[%s] not Found.", logonID);

	if (logMsg.Length() > 0)
		UFC::BufferedLog::DebugPrintf(" TOrderStore::RecalculateCommodityCancelStatistic() [%s]",
									  s888::ConvertUnicodeToUFCAnsiStr(logMsg).c_str());
}  //TOrderStore::RecalculateCommodityCancelStatistic()
//------------------------------------------------------------------------------
void TOrderStore::GetContractCancelStatistic(const String& ExchangeCode, const String& Symbol, int& CancelQty, int& CancelRptCount)
{
	CancelQty = 0;
	CancelRptCount = 0;
	String logonID = FID;
	String logMsg = L"";
	s888::CTradingAccount *tradingAccountPtr = 0;
	if (logonID.Length() > 0) tradingAccountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex(logonID, s888::rwLockForRead);
	if (tradingAccountPtr != 0)
	{
		s888::CContractCancelStatistic *contractStatisticPtr = tradingAccountPtr->FindContractCancelStatisticFromSymbolIndex(ExchangeCode, Symbol, s888::rwLockForRead);
		if (contractStatisticPtr != 0)
		{
			contractStatisticPtr->LockForRead();
			CancelQty = contractStatisticPtr->GetCancelQuantity(s888::rwNotLock);
			CancelRptCount = contractStatisticPtr->GetCancelReportCount(s888::rwNotLock);
			contractStatisticPtr->UnlockForRead();
		}
	}
}  //TOrderStore::GetContractCancelStatistic()
//------------------------------------------------------------------------------
void TOrderStore::GetCommodityCancelStatistic(const String& ExchangeCode, const String& ContractCode, int& CancelQty, int& CancelRptCount)
{
	CancelQty = 0;
	CancelRptCount = 0;
	String logonID = FID;
	String logMsg = L"";
	s888::CTradingAccount *tradingAccountPtr = 0;
	if (logonID.Length() > 0) tradingAccountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex(logonID, s888::rwLockForRead);
	if (tradingAccountPtr != 0)
	{
		s888::CCommodityCancelStatistic *commodityStatisticPtr = tradingAccountPtr->FindCommodityCancelStatisticFromCodeIndex(ExchangeCode, ContractCode, s888::rwLockForRead);
		if (commodityStatisticPtr != 0)
		{
			commodityStatisticPtr->LockForRead();
			CancelQty = commodityStatisticPtr->GetCancelQuantity(s888::rwNotLock);
			CancelRptCount = commodityStatisticPtr->GetCancelReportCount(s888::rwNotLock);
			commodityStatisticPtr->UnlockForRead();
		}
	}
}  //TOrderStore::GetCommodityCancelStatistic()
//---------------------------------------------------------------------------
const String& __fastcall TOrderStore::GetLocalIP( void )
{
	return  FAdapter->GetLocalIP();
}
//------------------------------------------------------------------------------
void TOrderStore::GetFilledList(  const String& Exchange, const String& Symbol, UFC::List<TFilledData>& Filleds )
{
	CStringTExecutionMap* OrderMapPtr    = &FFilledOrderRootTimeOrderIDIndex;
	int                   TotalItemCount = OrderMapPtr->size();
	int                   CurItemNo      = 0;
	TExecution*           OrderRootPtr;

	CStringTExecutionMap::reverse_iterator it;
	for( it = OrderMapPtr->rbegin(); it != OrderMapPtr->rend(); it++)
	{
		String TestSymbol;

		OrderRootPtr = it->second;
		if( OrderRootPtr->GetSymbol() != NULL )
			TestSymbol = OrderRootPtr->GetSymbol()->GetSymbol( 0 );
		if( OrderRootPtr->GetExchangeCode() == Exchange &&  TestSymbol == Symbol )
		{
			TFilledData FillD( OrderRootPtr->GetSide(), OrderRootPtr->GetLastPx(),OrderRootPtr->GetCumQty(), OrderRootPtr->GetHHMM() );

			Filleds.Add( FillD );
		}
		CurItemNo++;
	}
}
//------------------------------------------------------------------------------
void TOrderStore::AddFilledListener( const String& Exchange, const String& Symbol, TFilledListener* Listener )
{
	if( Listener != NULL )
	{
		UFC::PtrList<TFilledListener>* LintenerList;
		UFC::List<TFilledData> Filleds;
		AnsiString Key( Exchange + L"_" + Symbol );

		if( FFilledListenerMap.GetObjectByKey( Key.c_str(), LintenerList ) == false )
		{
			LintenerList = new UFC::PtrList<TFilledListener>();
			FFilledListenerMap.Add( Key.c_str(), LintenerList );
		}
		GetFilledList( Exchange, Symbol, Filleds );
		Listener->OnRecoverFilled( Exchange, Symbol, Filleds );
		if( LintenerList->IndexOf( Listener ) == -1 )
			LintenerList->Add( Listener );
	}
}
//------------------------------------------------------------------------------
void TOrderStore::ExecuteOnFilled(  const String& Exchange, const String& Symbol, const String& OrderID, double Price, int Qty, nsOrderMessageDefine::SideEnum Side, nsOrderMessageDefine::OrderStatusEnum OrderStatus, int HHMM )
{
	UFC::PtrList<TFilledListener>* LintenerList;
	AnsiString Key( Exchange + L"_" + Symbol );

	if( FFilledListenerMap.GetObjectByKey( Key.c_str(), LintenerList ) == true )
	{
		TFilledData FillD( Side, Price, Qty, HHMM );
		TFilledListener* FilledListener;

		for( int i =0; i< LintenerList->ItemCount(); i++ )
		{
			FilledListener = LintenerList->GetItem( i );

			FilledListener->OnFilled( Exchange, Symbol, FillD );
		}
	}
	if( FOnFilledReply !=  NULL )
		FOnFilledReply( this,  Exchange, Symbol,OrderID, Side, Price, Qty, OrderStatus );
}
//------------------------------------------------------------------------------
void TOrderStore::RemoveFilledListener( const String& Exchange, const String& Symbol, TFilledListener* Listener)
{
	if( Listener != NULL )
	{
		UFC::PtrList<TFilledListener>* LintenerList;
		AnsiString Key( Exchange + L"_" + Symbol );

		if( FFilledListenerMap.GetObjectByKey( Key.c_str(), LintenerList ) == true )
		{
			int delIndex = LintenerList->IndexOf( Listener );
			if( delIndex != -1 )
				LintenerList->Delete( delIndex );
		}
	}
}
//------------------------------------------------------------------------------
namespace Orderstore
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TOrderStore)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//------------------------------------------------------------------------------
