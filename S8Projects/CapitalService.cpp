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
TCapitalService::TCapitalService( const String& BaseURL )
:FBaseURL( BaseURL )
,FLoginOK( false )
{
}
//---------------------------------------------------------------------------
bool TCapitalService::LoginBroker( const String& ID, const String& Password, String& Msg )
{
	FAccounts.ClearAccounts();
	String account = "1111111";
	String BrokerID = "F915000";
	String dayTrade = "Today";
	FAccounts.FFutAccounts.Add( new TAccountInfo(account, BrokerID, dayTrade ) );
	FAccounts.FStockAccounts.Add( new TAccountInfo(account, BrokerID, dayTrade ) );
	FAccounts.FName = "Shawn";
	FAccounts.FIDNO = "A12345678";
	return true;
}
//---------------------------------------------------------------------------
bool TCapitalService::GetPosition( bool IsTAIFEX, const String& Account, String& Msg )
{
	return true;
}
//---------------------------------------------------------------------------
void TCapitalService::ClearPosition( const String& ID )
{
}
//---------------------------------------------------------------------------
bool TCapitalService::SignAgreememt( const String& Account, TCAChecker* CAChecker, String& Msg )
{
	return true;
}
//---------------------------------------------------------------------------
TBrokerUser* TCapitalService::GetAccount( void )
{
	return &FAccounts;
}
