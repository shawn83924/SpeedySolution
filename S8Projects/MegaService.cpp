//---------------------------------------------------------------------------
#include "FMTConfig.h"
#include "UnifyUtility.h"
#include "UnifyDlg.h"
#include "MD5.h"
#include <System.NetEncoding.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include "IdSSL.hpp"
#include "IdSSLOpenSSL.hpp"
#include "IdExplicitTLSClientServerBase.hpp"
#include <IdHTTP.hpp>
#include "MarketDataStore.h"
#include "OrderStore.h"
#include "AggrementUnit.h"
#include "CAChecker.h"
//---------------------------------------------------------------------------
#include "TradingCommodities.h"
#include "TradingObjectPool.h"
#include "TradingPosition.h"
//---------------------------------------------------------------------------
const String& CheckSumKey = "futuresTreasure888";
const String& ExTAIFEX = L"TAIFEX";
extern TCMarketDataStore* gMarketDataStore;
extern TOrderStore*       gOrderStore;
extern TUnifyUser         gUser;
using namespace s888;
//---------------------------------------------------------------------------
TMegaService::TMegaService( const String& BaseURL )
:FBaseURL( BaseURL )
,FLoginOK( false )
{
	FStockTaxRate = new CTaxRateObject( L"TWSE", L"2330", 0.0015 );
}
//---------------------------------------------------------------------------
bool TMegaService::ToErrorMessage( const String& Code, String& Msg )
{
	switch( Code.ToInt() )
	{
		case   0:	Msg = L"執行成功";break;
		case   1:	Msg = L"查無此帳號";break;
		case 901:	Msg = L"輸入參數錯誤";break;
		case 902:	Msg = L"參數解析錯誤(checksum)";break;
		case 911:	Msg = L"目前系統異常、請稍後再試";break;
		case 998:	Msg = L"查無資料";break;
		case 999:	Msg = L"帳密錯誤";break;
		default:    return false;
	}
	return true;
}
//---------------------------------------------------------------------------
TBrokerUser* TMegaService::GetAccount( void )
{
	return &FAccounts;
}
//---------------------------------------------------------------------------
bool TMegaService::LoginBroker( const String& ID, const String& Password, String& Msg )
{
	FID       = ID;
	FPassword = Password;
	TMemoryStream* ResultStream = new TMemoryStream();
	String ResponseJSON;
	bool   Result = false;

	FAccounts.ClearAccounts();
	if( RequestLogon( ResultStream, Msg ) == true )
	{
		if( GetResponseJSON( ResultStream, ResponseJSON ) == true )
		{
			UFC::BufferedLog::Printf("-------------Logon Success-----------------");
			UFC::BufferedLog::Printf("Response:");
			AnsiString str = ResponseJSON;
			UFC::BufferedLog::Printf( "%s", str.c_str() );
			TJSONValue *lpJson = TJSONObject::ParseJSONValue( ResponseJSON );
			TJSONObject *lpRoot = dynamic_cast<TJSONObject *>(lpJson);
			if( lpRoot != NULL )
			{
				String ResultStr  = lpRoot->Values[L"result"]->Value();
				String MessageStr = lpRoot->Values[L"message"]->Value();
				if( ResultStr == L"0" )
				{
					TJSONValue *lpDataList = lpRoot->GetValue( L"dataList" );
					TJSONArray *AccountArray;

					if( lpDataList != NULL &&
						(AccountArray = dynamic_cast<TJSONArray*>(lpDataList))!= NULL &&
						AccountArray->Count > 0 )
					{
						for( int i=0;i< AccountArray->Count; i++ )
						{
							 TJSONObject *AccItem = dynamic_cast<TJSONObject *>(AccountArray->Items[i]);
							 String  account       = AccItem->Values[L"account"]->Value();
							 String  branch_id     = AccItem->Values[L"branch_id"]->Value();
							 String  account_type  = AccItem->Values[L"account_type"]->Value();
							 String  dayTrade      = AccItem->Values[L"day_trade"]->Value();
							 int id;
							 String  BrokerID;
							 if (TryStrToInt(branch_id, id))
							 	BrokerID = BrenchToBrokrtID( id );

							 if( account_type == L"1" ) ///< Stock Account
								 FAccounts.FStockAccounts.Add( new TAccountInfo(account, BrokerID, branch_id, dayTrade ) );
							 else if( account_type == L"2" ) ///< Futures Account
								 FAccounts.FFutAccounts.Add( new TAccountInfo( account, "F030000", branch_id, dayTrade ) );
						}
						FAccounts.FName  = lpRoot->Values[L"name"]->Value();
						FAccounts.FIDNO  = lpRoot->Values[L"idno"]->Value();
						Msg    = MessageStr;
						Result = true;
					}
					else
						Msg = L"沒有可用的下單帳號";
				}
				else
				{
				   if( ToErrorMessage( ResultStr,  Msg ) == false )
					   Msg = MessageStr;
				}
			}
		}
	}
	delete ResultStream;
	return Result;
}
//---------------------------------------------------------------------------
bool TMegaService::GetResponseJSON( TMemoryStream* Stream, String& ResponseJSON )
{
	String  JsonTxt( (char*)Stream->Memory, Stream->Size );
	TJSONValue *lpJson = TJSONObject::ParseJSONValue( JsonTxt );
	TJSONObject *lpRoot = dynamic_cast<TJSONObject *>(lpJson);
	if( lpRoot != NULL )
	{
		String DataJSON = lpRoot->Values[ "response_data" ]->Value();
		ResponseJSON = Base64Decode( DataJSON ); ///< Base64 Decode
		delete lpJson;
		return true;
	}
	delete lpJson;
	return false;
}
//---------------------------------------------------------------------------
void TMegaService::GenData( const String& ID, const String& Password, String& Out )
{
	AnsiString ChkSumStr = CheckSumKey + ID + Password;
	UFC::MD5   ChkSum( (const UFC::UInt8*)ChkSumStr.c_str(), ChkSumStr.Length() );
	String     CheckSum( ChkSum.ToString().c_str() );
	String     Param;

	Param.printf( L"{\"checksum\":\"%s\",\"cust_id\":\"%s\",\"pwd\":\"%s\"}",CheckSum, ID,Password );
	Out = Base64Encode( Param );  ///< Base64 Encode
}
//---------------------------------------------------------------------------
void TMegaService::GenData( const String& ID, const String& Password, const String& IB, String& Out )
{
	AnsiString ChkSumStr = CheckSumKey + IB +ID + Password;
	UFC::MD5   ChkSum( (const UFC::UInt8*)ChkSumStr.c_str(), ChkSumStr.Length() );
	String     CheckSum( ChkSum.ToString().c_str() );
	String     Param;

	Param.printf( L"{\"checksum\":\"%s\",\"branch_id\":\"%s\",\"cust_id\":\"%s\",\"pwd\":\"%s\"}",CheckSum,IB,ID,Password );
	Out = Base64Encode( Param );  ///< Base64 Encode
}
//---------------------------------------------------------------------------
void TMegaService::GenData2( const String& ID, const String& Password, const String& IB, String& Out )
{
	AnsiString ChkSumStr = CheckSumKey + IB +ID + L"1";
	UFC::MD5   ChkSum( (const UFC::UInt8*)ChkSumStr.c_str(), ChkSumStr.Length() );
	String     CheckSum( ChkSum.ToString().c_str() );
	String     Param;

	Param.printf( L"{\"checksum\":\"%s\",\"branch_id\":\"%s\",\"cust_id\":\"%s\",\"tip\":\"1\"}",CheckSum,IB,ID );
	Out = Base64Encode( Param );  ///< Base64 Encode
}
//---------------------------------------------------------------------------
bool TMegaService::RequestLogon( TMemoryStream* OutStream, String& Msg )
{
	TIdSSLIOHandlerSocketOpenSSL* SSLIOHandler = new TIdSSLIOHandlerSocketOpenSSL( NULL );
	TIdHTTP*                      pHTTP        = new TIdHTTP( NULL );
	TMemoryStream*                SourceStream = new TMemoryStream();
	String                        URL,Data;
	bool                          Result = true;

	try
	{
		GenData( FID, FPassword, Data );
		URL.printf( L"%sdataTrans.do?data=%s&txid=IFT_O_01", FBaseURL, Data );
		if( URL.Pos( L"https" ) != 0 ) ///< Use https
		{
			SSLIOHandler->SSLOptions->Method = sslvSSLv23;
			pHTTP->IOHandler = SSLIOHandler;
		}
		pHTTP->ConnectTimeout = 90000; ///< 90 sec IIS default(Paul said)
		pHTTP->Post( pHTTP->URL->URLEncode( URL ), SourceStream, OutStream );
	}
	catch( Exception& ex )
	{
		Result = false;
		Msg = ex.ToString();
	}
	delete SSLIOHandler;
	delete pHTTP;
	delete SourceStream;;
	return Result;
}
//---------------------------------------------------------------------------
bool TMegaService::RequestPosition(  bool IsTAIFEX, const String& Account, TMemoryStream* OutStream, String& Msg )
{
	TIdSSLIOHandlerSocketOpenSSL* SSLIOHandler = new TIdSSLIOHandlerSocketOpenSSL( NULL );
	TIdHTTP*                      pHTTP        = new TIdHTTP( NULL );
	TMemoryStream*                SourceStream = new TMemoryStream();
	String                        URL,Data;
	bool                          Result = true;

	try
	{
		if( IsTAIFEX == true )
		{
			GenData( Account, FPassword, L"098",  Data ); ///< Mega only one branch F030000 map to 098
			URL.printf( L"%sdataTrans.do?data=%s&txid=IFT_O_03", FBaseURL, Data );
		}
		else
		{
			GenData2( Account, FPassword, L"000",  Data );
			URL.printf( L"%sdataTrans.do?data=%s&txid=IFT_O_06", FBaseURL, Data );
        }
		if( URL.Pos( L"https" ) != 0 ) ///< Use https
		{
			SSLIOHandler->SSLOptions->Method = sslvSSLv23;
			pHTTP->IOHandler = SSLIOHandler;
		}
		pHTTP->ConnectTimeout = 90000; ///< 90 sec IIS default(Paul said)
		pHTTP->Post( pHTTP->URL->URLEncode( URL ), SourceStream, OutStream );
	}
	catch( Exception& ex )
	{
		Result = false;
		Msg = ex.ToString();
	}
	delete SSLIOHandler;
	delete pHTTP;
	delete SourceStream;;
	return Result;
}
//---------------------------------------------------------------------------
bool TMegaService::GetPosition( bool IsTAIFEX, const String& Account, String& Msg )
{
	TMemoryStream* ResultStream = new TMemoryStream();
	String ResponseJSON;
	bool   Result = false;

	if( RequestPosition( IsTAIFEX, Account, ResultStream, Msg ) == true )
	{
		if( GetResponseJSON( ResultStream, ResponseJSON ) == true )
		{
			TJSONValue *lpJson = TJSONObject::ParseJSONValue( ResponseJSON );
			TJSONObject *lpRoot = dynamic_cast<TJSONObject *>(lpJson);

			String ResultStr  = lpRoot->Values[L"result"]->Value();
			String MessageStr = lpRoot->Values[L"message"]->Value();
			if( ResultStr == L"0" )
			{
				TJSONValue *lpDataList;
				TJSONArray *PosArray;
				TFileStream* SaveFile;

				if( IsTAIFEX == true )
				{
					UFC::BufferedLog::Printf( " ------------ Load TAIFEX position ----------" );
					lpDataList = lpRoot->GetValue( L"dataList" );
					SaveFile = new TFileStream( "TAIFEXPosition.txt", fmCreate );
				}
				else
				{
					UFC::BufferedLog::Printf( " ------------- Load TWSE position -----------" );
					lpDataList = lpRoot->GetValue( L"stksumList" );
					SaveFile = new TFileStream( "TWSEPosition.txt", fmCreate );
				}
				if( lpDataList != NULL &&(PosArray = dynamic_cast<TJSONArray*>(lpDataList))!= NULL )
				{
					if( IsTAIFEX == true )
						AddPosition( Account, PosArray );
					else
						AddStockPosition( Account, PosArray );
					Msg = MessageStr;
				}
				else
					Msg = L"沒有留倉部位";
				UFC::BufferedLog::Printf( " --------------------------------------------" );

				AnsiString Json( ResponseJSON );
				SaveFile->Write( Json.c_str(), Json.Length() + 1 );
				delete SaveFile;
				Result = true;
			}
			else
			{
			   if( ToErrorMessage( ResultStr,  Msg ) == false )
				   Msg = MessageStr;
			}
		}
	}
	delete ResultStream;
	return Result;
}
//---------------------------------------------------------------------------
void TMegaService::AddStockPosition( const String& Account, TJSONArray *PosArray )
{
	s888::CTradingAccount* AccountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex( gUser.LoginUserID, s888::rwLockForRead );
	if( AccountPtr != NULL )
	{
		AccountPtr->LockForWrite();
		AccountPtr->SetTWSEHoldPositionQueryStatus( qsNone, rwNotLock );
		AccountPtr->SetTWSEHoldPositionQueryMessage( L"", rwNotLock );
		AccountPtr->ClearTWSEHoldPositionArray( rwNotLock );
		AccountPtr->UnlockForWrite();

		for( int i = 0; i < PosArray->Count; i++ )
		{
			 TJSONObject *PosItem = dynamic_cast<TJSONObject*>(PosArray->Items[i]);
			 String  symbol            = PosItem->Values[L"stkno"]->Value();
			 String  type              = PosItem->Values[L"stype"]->Value();
			 TJSONValue *lpDataList	   = PosItem->GetValue( L"stkdatList" );
			 TJSONArray *StockArray;
			 nsOrderMessageDefine::MarketEnum    market;   // market
			 nsOrderMessageDefine::SideEnum      Side = nsOrderMessageDefine::sBuy;
			 String  Ex = L"TWSE";
			 CHoldPositionData* holdPositionPtr;


			 if( type == L"H" )
				 market = nsOrderMessageDefine::mTSE;
			 else if( type == L"O" )
			 {
				 Ex = L"OTC";
				 market = nsOrderMessageDefine::mOTC;
			 }
			 else if( type == L"R" )
			 {
				 Ex = L"ES";
				 market = nsOrderMessageDefine::mES;
			 }
			 else
				 market = nsOrderMessageDefine::mTSE;
			if( lpDataList != NULL &&(StockArray = dynamic_cast<TJSONArray*>(lpDataList))!= NULL )
			{
				for( int i = 0; i < StockArray->Count; i++ )
				{
					 TJSONObject *Pos   = dynamic_cast<TJSONObject*>(StockArray->Items[i]);
					 String  Time       = Pos->Values[L"ttime"]->Value();
					 String  TradeDate  = Pos->Values[L"tdate"]->Value();
					 int     quantity   = Pos->Values[L"qty"]->Value().ToInt() / 1000 ; ///< uqty
					 double  matchPrice = Pos->Values[L"price"]->Value().ToDouble();
					 String  OID        = Pos->Values[L"ordno"]->Value();
					 String  BS         = Pos->Values[L"bs"]->Value();

					 if( Time == L"" )
					 {
						 OID.SetLength( 5 );
						 if( BS == L"B" )
							 Side = nsOrderMessageDefine::sBuy;
						 else
							 Side = nsOrderMessageDefine::sSell;
						 holdPositionPtr = new CHoldPositionData( Ex, symbol, market, Side, quantity, matchPrice );
						 holdPositionPtr->SetTradeSymbol( symbol, rwNotLock );
						 holdPositionPtr->SetOrderID( OID, rwNotLock);
						 holdPositionPtr->SetTradeDate( TradeDate, rwNotLock);
						 AccountPtr->AppendTWSEHoldPositionToArray(holdPositionPtr, rwLockForWrite);
						 s888::CPositionStatisticRecord *positionStatisticRecPtr = gOrderStore->FindOrCreatePosStatisticRec( Ex, symbol );
						 if( positionStatisticRecPtr != 0 )
						 {
							 nsOrderMessageDefine::PositionEffectEnum positionEffect = nsOrderMessageDefine::peOpen;
							 positionStatisticRecPtr->LockForWrite();
							 positionStatisticRecPtr->AddHoldPosition( OID, OID, market, Side, positionEffect, matchPrice, quantity, s888::rwNotLock);
							 positionStatisticRecPtr->ReOffsetPosition( s888::rwNotLock );
							 positionStatisticRecPtr->DumpData( s888::constFlush, s888::rwNotLock );
							 positionStatisticRecPtr->UnlockForWrite();
							 gOrderStore->SubscribeSymbol( Ex, symbol, true, true);
						 }
					 }
				}
			}
		}
	}

}
//---------------------------------------------------------------------------
void TMegaService::ClearPosition( const String& ID )
{
	s888::CTradingAccount* AccountPtr   = s888::gTradingObjsPool.FindAccountFromLogonIdIndex( gUser.LoginUserID, s888::rwLockForRead );
	BasicInformation* Info;

	if( AccountPtr != NULL )
	{
		AccountPtr->LockForWrite();
		AccountPtr->ClearContractPositionArray();
		AccountPtr->SetTaifexHoldPositionQueryStatus( qsNone, rwNotLock );
		AccountPtr->SetTaifexHoldPositionQueryMessage( L"", rwNotLock );
		AccountPtr->ClearTaifexHoldPositionArray( rwNotLock );
		AccountPtr->SetTaifexHoldPositionQueryStatus( qsNone, rwNotLock );
		AccountPtr->SetTaifexHoldPositionQueryMessage( L"", rwNotLock );
		AccountPtr->ClearTaifexHoldPositionArray( rwNotLock );
		AccountPtr->UnlockForWrite();
	}
}
//---------------------------------------------------------------------------
void TMegaService::AddPosition( const String& Account, TJSONArray *PosArray )
{
	s888::CTradingAccount* AccountPtr   = s888::gTradingObjsPool.FindAccountFromLogonIdIndex( gUser.LoginUserID, s888::rwLockForRead );
	BasicInformation* Info;

	if( AccountPtr != NULL )
	{
		for( int i=0;i< PosArray->Count; i++ )
		{
			 TJSONObject *PosItem = dynamic_cast<TJSONObject*>(PosArray->Items[i]);
			 String  OrderID           = PosItem->Values[L"v5"]->Value();
			 String  TradeDate         = PosItem->Values[L"v2"]->Value();
			 String  ProdID            = PosItem->Values[L"v9"]->Value();
			 String  maturityYearMonth = PosItem->Values[L"v10"]->Value();
			 String  type              = PosItem->Values[L"v11"]->Value();
			 double  strikePrice       = PosItem->Values[L"v12"]->Value().ToDouble();
			 String  side              = PosItem->Values[L"v13"]->Value();
			 int     quantity          = PosItem->Values[L"v15"]->Value().ToInt(); ///< uqty
			 double  matchPrice        = PosItem->Values[L"v16"]->Value().ToDouble();
			 nsOrderMessageDefine::MarketEnum    market;   // market
			 nsOrderMessageDefine::SideEnum      Side;     // buy sell
			 String symbol,logMsg,Msg;

			 if( side == L"S" )
				 Side = nsOrderMessageDefine::sSell;
			 else
				 Side = nsOrderMessageDefine::sBuy;
			 if( type == L"C" ) ///< Options call
			 {
				 market = nsOrderMessageDefine::mTWOptions;
				 Info = gMarketDataStore->GetOPTInfo( ExTAIFEX, ProdID, maturityYearMonth, strikePrice, cpCall );
			 }
			 else if( type == L"P" ) ///< Options Put
			 {
				 market = nsOrderMessageDefine::mTWOptions;
				 Info = gMarketDataStore->GetOPTInfo( ExTAIFEX, ProdID, maturityYearMonth, strikePrice, cpPut );
			 }
			 else ///< Futures
			 {
				 market = nsOrderMessageDefine::mTWFutures;
				 Info = gMarketDataStore->GetFUTInfo( ExTAIFEX, ProdID, maturityYearMonth );
			 }
			 if( quantity > 0 && Info != NULL )
			{
				symbol = Info->GetSymbol().c_str();

				CHoldPositionData *holdPositionPtr = new CHoldPositionData( ExTAIFEX, symbol, market, Side, quantity, matchPrice );
				holdPositionPtr->SetTradeSymbol( symbol, rwNotLock );
				holdPositionPtr->SetOrderID( OrderID, rwNotLock);
				holdPositionPtr->SetTradeDate( TradeDate, rwNotLock);
				AccountPtr->AppendTaifexHoldPositionToArray(holdPositionPtr, rwLockForWrite);

				Msg.printf( L" %s %s %d@%0.3f", symbol, side, quantity, matchPrice );
			}
			else
				Msg.printf( L" Prod[%s][%s] Side[%s] %d@%0.3f not found!", ProdID, maturityYearMonth, side, quantity, matchPrice );
			AnsiString AnsiMsg( Msg );
			UFC::BufferedLog::Printf( " %s", AnsiMsg.c_str());
		}
		ProcessHoldPositionData( AccountPtr );
	}
}
//---------------------------------------------------------------------------
void TMegaService::ProcessHoldPositionData( s888::CTradingAccount* AccountPtr )
{
	int PositionCount = AccountPtr->GetTaifexHoldPositionCount( s888::rwNotLock );
	s888::CHoldPositionData *holdPositionPtr;

	for( int i  = 0; i < PositionCount; i++)
	{
		holdPositionPtr = AccountPtr->GetTaifexHoldPositionFromArray( i, s888::rwNotLock);
		holdPositionPtr->LockForRead();
		String orderId      = holdPositionPtr->GetOrderID(s888::rwNotLock);
		String symbol       = holdPositionPtr->GetSymbol(s888::rwNotLock);
		int    holdQuantity = holdPositionPtr->GetHoldQuantity(s888::rwNotLock);
		double matchPrice   = holdPositionPtr->GetMatchPrice(s888::rwNotLock);
		nsOrderMessageDefine::SideEnum side = holdPositionPtr->GetSide(s888::rwNotLock);
		nsOrderMessageDefine::MarketEnum market = holdPositionPtr->GetMarket(s888::rwNotLock);
		holdPositionPtr->UnlockForRead();

		s888::CPositionStatisticRecord *positionStatisticRecPtr = gOrderStore->FindOrCreatePosStatisticRec( ExTAIFEX, symbol );
		if (positionStatisticRecPtr != 0)
		{
			nsOrderMessageDefine::PositionEffectEnum positionEffect = nsOrderMessageDefine::peOpen;
			positionStatisticRecPtr->LockForWrite();
			positionStatisticRecPtr->AddHoldPosition(orderId, orderId, market, side, positionEffect, matchPrice, holdQuantity, s888::rwNotLock);
			positionStatisticRecPtr->ReOffsetPosition(s888::rwNotLock);
			positionStatisticRecPtr->DumpData(s888::constFlush, s888::rwNotLock);
			positionStatisticRecPtr->UnlockForWrite();
			gOrderStore->SubscribeSymbol( ExTAIFEX, symbol, true, true);
		}
	}
}
//---------------------------------------------------------------------------
bool TMegaService::SignAgreememt( const String& Account, TCAChecker* CAChecker, String& Msg )
{
	bool Signed = true;
	bool Rtn = false;

	if( Signed == false )
	{
		 AggrementForm = new TAggrementForm( NULL );
		 if( AggrementForm->ShowModal() == mrOk )
		 {
			 String SignTxt;

			 SignTxt.printf( L"SpeedyUnify_Mega_%s_Signed_Agreement", Account );
			 if( CAChecker->SignAgrement( SignTxt, Msg ) == true )
			 {
				 g_Config.SetIntegerProperty( Account, "Agreement", 1 );
				 Rtn = true;
			 }
		 }
		 else
		 {
			 Msg = L"用戶不同意簽署風險預告暨使用聲明書.";
		 }
		 delete AggrementForm;
	}
	else
		Rtn = true;
	return Rtn;
}
//---------------------------------------------------------------------------
void TMegaService::AddTestPosition( bool IsTAIFEX )
{
	if( IsTAIFEX == false )
	{
		String symbol = L"1416";
		nsOrderMessageDefine::MarketEnum    market = nsOrderMessageDefine::mTSE;
		nsOrderMessageDefine::SideEnum      Side = nsOrderMessageDefine::sBuy;
		int quantity = 140;
		double  matchPrice = 11.02;
		s888::CPositionStatisticRecord *positionStatisticRecPtr = gOrderStore->FindOrCreatePosStatisticRec( "TWSE", symbol );
		if (positionStatisticRecPtr != 0)
		{
			nsOrderMessageDefine::PositionEffectEnum positionEffect = nsOrderMessageDefine::peOpen;
			positionStatisticRecPtr->LockForWrite();
			positionStatisticRecPtr->AddHoldPosition( "z0002", "z0002", market, Side, positionEffect, matchPrice, quantity, s888::rwNotLock);
			positionStatisticRecPtr->ReOffsetPosition( s888::rwNotLock );
			positionStatisticRecPtr->DumpData( s888::constFlush, s888::rwNotLock );
			positionStatisticRecPtr->SetTaxRateObject( FStockTaxRate, s888::rwNotLock );
			positionStatisticRecPtr->SetContractMultiplier( 1000, s888::rwNotLock );
			positionStatisticRecPtr->UnlockForWrite();
			gOrderStore->SubscribeSymbol( "TWSE", symbol, true, true);
		}
	}
	else
	{
		s888::CTradingAccount* AccountPtr   = s888::gTradingObjsPool.FindAccountFromLogonIdIndex( gUser.LoginUserID, s888::rwLockForRead );
		if( AccountPtr != NULL )
		{
				String symbol = L"TXFD1";
				nsOrderMessageDefine::MarketEnum    market = nsOrderMessageDefine::mTWFutures;
				nsOrderMessageDefine::SideEnum      Side = nsOrderMessageDefine::sBuy;
				int quantity = 5;
				double  matchPrice = 16540;

				CHoldPositionData *holdPositionPtr = new CHoldPositionData( "TAIFEX", symbol, market, Side, quantity, matchPrice );
				holdPositionPtr->SetTradeSymbol( symbol, rwNotLock );
				holdPositionPtr->SetOrderID( "z0001", rwNotLock);
				holdPositionPtr->SetTradeDate( L"20210405", rwNotLock);
				AccountPtr->AppendTaifexHoldPositionToArray(holdPositionPtr, rwLockForWrite);


				symbol = L"MXFD1";
				quantity = 6;
				matchPrice = 16530;
				holdPositionPtr = new CHoldPositionData( "TAIFEX", symbol, market, Side, quantity, matchPrice );
				holdPositionPtr->SetTradeSymbol( symbol, rwNotLock );
				holdPositionPtr->SetOrderID( L"z0002", rwNotLock);
				holdPositionPtr->SetTradeDate( L"20210401", rwNotLock);
				AccountPtr->AppendTaifexHoldPositionToArray(holdPositionPtr, rwLockForWrite);

				symbol = L"MXFD1";
				quantity = 2;
				matchPrice = 16630;
				holdPositionPtr = new CHoldPositionData( "TAIFEX", symbol, market, Side, quantity, matchPrice );
				holdPositionPtr->SetTradeSymbol( symbol, rwNotLock );
				holdPositionPtr->SetOrderID( L"z0003", rwNotLock);
				holdPositionPtr->SetTradeDate( L"20210407", rwNotLock);
				AccountPtr->AppendTaifexHoldPositionToArray(holdPositionPtr, rwLockForWrite);

			ProcessHoldPositionData( AccountPtr );
		}
	}
}
//---------------------------------------------------------------------------
bool TMegaService::RequestRiskDisclosureStatement( const String& Branch_ID, const String& cust_id, TMemoryStream* OutStream, String& Msg )
{
	TIdSSLIOHandlerSocketOpenSSL* SSLIOHandler = new TIdSSLIOHandlerSocketOpenSSL( NULL );
	TIdHTTP*                      pHTTP        = new TIdHTTP( NULL );
	TMemoryStream*                SourceStream = new TMemoryStream();
	String                        URL,Data;
	bool                          Result = true;

	try
	{
		AnsiString ChkSumStr = CheckSumKey + Branch_ID + cust_id;
		UFC::MD5   ChkSum( (const UFC::UInt8*)ChkSumStr.c_str(), ChkSumStr.Length() );
		String     CheckSum( ChkSum.ToString().c_str() );
		String     Param;

		Param.printf( L"{\"checksum\":\"%s\",\"branch_id\":\"%s\",\"cust_id\":\"%s\"}", CheckSum, Branch_ID, cust_id );
		Data = Base64Encode( Param );
		URL.printf( L"%sdataTrans.do?data=%s&txid=IFT_O_07", FBaseURL, Data );
		if( URL.Pos( L"https" ) != 0 )
		{
			SSLIOHandler->SSLOptions->Method = sslvSSLv23;
			pHTTP->IOHandler = SSLIOHandler;
		}
		pHTTP->ConnectTimeout = 90000;
		pHTTP->Post( pHTTP->URL->URLEncode( URL ), SourceStream, OutStream );
	}
	catch( Exception& ex )
	{
		Result = false;
		Msg = ex.ToString();
	}
	delete SSLIOHandler;
	delete pHTTP;
	delete SourceStream;
	return Result;
}
//---------------------------------------------------------------------------
bool TMegaService::SignRiskDisclosureStatement(String& Msg)
{
	TMemoryStream* ResultStream = new TMemoryStream();
	String ResponseJSON;
	bool   Result = false;
	if(FAccounts.FFutAccounts.ItemCount()<=0)
	{
        delete ResultStream;
		return Result;
	}
	TAccountInfo* accountInfo = FAccounts.FFutAccounts.GetItem(0);
	if( RequestRiskDisclosureStatement(accountInfo->BranchID, accountInfo->Account, ResultStream, Msg) != true )
	{
		delete ResultStream;
		return Result;
	}

	if( GetResponseJSON(ResultStream, ResponseJSON) != true )
	{
		delete ResultStream;
		return Result;
	}

	TJSONValue  *lpJson = TJSONObject::ParseJSONValue( ResponseJSON );
	TJSONObject *lpRoot = dynamic_cast<TJSONObject *>(lpJson);
	if( lpRoot == NULL )
	{
		delete ResultStream;
		return Result;
	}

	String ResultStr  = lpRoot->Values[L"result"]->Value();
	String MessageStr = lpRoot->Values[L"message"]->Value();
	if( ResultStr != L"0" )
	{
		if( ToErrorMessage( ResultStr, Msg ) == false )
			Msg = MessageStr;
		delete lpJson;
		delete ResultStream;
		return Result;
	}

	TJSONValue *lpDataList = lpRoot->GetValue( L"qrydataList" );
	TJSONArray *DataArray  = dynamic_cast<TJSONArray*>(lpDataList);
	if( DataArray == NULL || DataArray->Count <= 0 )
	{
		Msg = L"查無風險預告書資料";
		delete lpJson;
		delete ResultStream;
		return Result;
	}

	TJSONObject *Item = dynamic_cast<TJSONObject *>(DataArray->Items[0]);
	String Status = Item->Values[L"status"]->Value();
	if (Status != L"S")
	{
		String status_result;
		if     ( Status == L"N" ) status_result = L"未上傳";
		else if( Status == L"Y" ) status_result = L"已上傳";
		else if( Status == L"E" ) status_result = L"上傳失敗";
		else if( Status == L"C" ) status_result = L"解約";
		else if( Status == L"D" ) status_result = L"不可簽署";
		else                      status_result = Status;

		Msg.printf( L"風險預告書未完成簽署(%s)", status_result );

		delete lpJson;
		delete ResultStream;
		return Result;
	}

	Msg = MessageStr;
	Result = true;

	delete lpJson;
	delete ResultStream;
	return Result;
}
//---------------------------------------------------------------------------
const String& TMegaService::BrenchToBrokrtID( int MegaBrenchID )
{
	switch( MegaBrenchID )
	{
		case 0: FBrokerID = L"7000";break;
		case 1: FBrokerID =  L"7001";break;
		case 2: FBrokerID =  L"7002";break;
		case 3: FBrokerID =  L"7003";break;
		case 5: FBrokerID =  L"7005";break;
		case 6: FBrokerID =  L"7006";break;
		case 7: FBrokerID =  L"7007";break;
		case 8: FBrokerID =  L"7008";break;
		case 9: FBrokerID =  L"7009";break;
		case 52: FBrokerID =  L"700B";break;
		case 53: FBrokerID =  L"700C";break;
		case 54: FBrokerID =  L"700D";break;
		case 55: FBrokerID =  L"700E";break;
		case 56: FBrokerID =  L"700F";break;
		case 57: FBrokerID =  L"700G";break;
		case 58: FBrokerID =  L"700H";break;
		case 59: FBrokerID =  L"700I";break;
		case 60: FBrokerID =  L"700J";break;
		case 61: FBrokerID =  L"700K";break;
		case 62: FBrokerID =  L"700L";break;
		case 63: FBrokerID =  L"700M";break;
		case 64: FBrokerID =  L"700N";break;
		case 65: FBrokerID =  L"700P";break;
		case 66: FBrokerID =  L"700Q";break;
		case 67: FBrokerID =  L"700R";break;
		case 68: FBrokerID =  L"700S";break;
		case 69: FBrokerID =  L"700T";break;
		case 70: FBrokerID =  L"700U";break;
		case 71: FBrokerID =  L"700V";break;
		case 73: FBrokerID =  L"700W";break;
		case 88: FBrokerID =  L"700X";break;
		case 75: FBrokerID =  L"700Z";break;
		case 77: FBrokerID =  L"700a";break;
		case 22: FBrokerID =  L"700b";break;
		case 23: FBrokerID =  L"700c";break;
		case 24: FBrokerID =  L"700d";break;
		case 25: FBrokerID =  L"700e";break;
		case 26: FBrokerID =  L"700f";break;
		case 27: FBrokerID =  L"700g";break;
		case 28: FBrokerID =  L"700h";break;
		case 29: FBrokerID =  L"700i";break;
		case 86: FBrokerID =  L"700j";break;
		case 31: FBrokerID =  L"700k";break;
		case 36: FBrokerID =  L"700p";break;
		case 37: FBrokerID =  L"700q";break;
		case 38: FBrokerID =  L"700r";break;
		case 39: FBrokerID =  L"700s";break;
		case 43: FBrokerID =  L"700w";break;
		case 98: FBrokerID =  L"F030000";break;
		default: FBrokerID =  L"7000";break;
	}
	return FBrokerID;
}
//---------------------------------------------------------------------------
