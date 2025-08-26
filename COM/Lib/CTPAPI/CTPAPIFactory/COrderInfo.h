//---------------------------------------------------------------------------
#ifndef COrderInfoH
#define COrderInfoH
//---------------------------------------------------------------------------
#include "UFC.h"
#include "NameValueMessage.h"
//---------------------------------------------------------------------------
#include "ThostFtdcUserApiStruct.h"
#include "../../FemasAPI/FemasAPIFactory/UstpFtdcUserApiStruct.h"
#ifdef _KSFT
#include "../../KSftAPI/KSftAPIFactory/KSUserApiStructEx.h"
#else
#include "../../KStrdAPI/KStrdAPIFactory/IncEx/KSUserApiStructEx.h"
#endif
#include "../../HSufxAPI/HSufxAPIFactory/HSufxStructFutOpt.h"
#include "../../HSufxAPI/HSufxAPIFactory/HSufxStructStockOpt.h"
#include "../../OrderMessage/TNewOrderMessage.h"
//---------------------------------------------------------------------------
#ifndef Namespace__KingstarAPI_
#define Namespace__KingstarAPI_
#define KS_ KingstarAPI
#endif
//---------------------------------------------------------------------------
#include "OrderMessageDefine.h"
//---------------------------------------------------------------------------
typedef enum
{
	cosPending         = 0,
	cosConfirm         = 1,
	cosExchangeConfirm = 2,
	cosClosed          = 3

} COrderStatus;
//---------------------------------------------------------------------------
typedef enum
{
	csNone           = 0,
	csByOrderRef     = 1,
	csByOrderLocalID = 2,
	csByOrderSysID   = 3,

} CxlStyle;
//---------------------------------------------------------------------------
class COrderInfo
{
private: ///< User set.( knowns before send)
   COrderStatus FStatus;
   int FFrontID;
   int FSessionID;
   int FOrderRef;
   int FCancelRef;
private:  ///< Order Data
   UFC::AnsiString                          FBrokerID;
   UFC::AnsiString                          FAccount;
   UFC::AnsiString                          FSymbol;
   nsOrderMessageDefine::SideEnum           FSide;
   nsOrderMessageDefine::TimeInForceEnum    FTimeInForce;
   nsOrderMessageDefine::OrderTypeEnum      FOrderType;
   nsOrderMessageDefine::PositionEffectEnum FPositionEffect;
   double                                   FPrice;
private:
   UFC::AnsiString FData;
   UFC::AnsiString FCxlData;
private: ///< Update when CTP Server ack.
   UFC::AnsiString FExchangeID;
   UFC::AnsiString FOrderLocalID;
private: ///< Update when Exchange accepted.
   UFC::AnsiString FOrderSysID;
private:
	int FOrderQty;
	int FCumQty;
	int FLeavesQty;
public:
   int            GetFrontID( void ) { return FFrontID; }
   int            GetSessionID( void ) { return FSessionID; }
   int            GetOrderRef( void ) { return FOrderRef; }
   int            GetCancelRef( void ) { return FCancelRef; }
   Int64          GetOrderNID( void );
   Int64          GetCancelNID( void );
   int            GetOrderLocalIDInt( void ) { return FOrderLocalID.ToInt(); }
   Int64          GetOrderSysIDInt64( void ) { return FOrderSysID.ToInt64(); }
public: ///< Price,Qry properties
   int            GetOrderQty( void ) { return  FOrderQty; }
   int            GetCumQty( void ) { return  FCumQty; }
   int            GetLeavesQty( void ) { return  FLeavesQty; }
   double         GetPrice( void ) { return FPrice; }
public:
   const char*                              GetBrokerID( void ) { return FBrokerID.c_str(); }
   const char*                              GetUserID( void ) { return FAccount.c_str(); }
   const char*                              GetExchangeID( void ) { return FExchangeID.c_str(); }
   const char*                              GetSymbol( void ) { return FSymbol.c_str(); }
   const char*                              GetOrderLocalID( void ) { return FOrderLocalID.c_str(); }
   const char*                              GetOrderSysID( void ) { return FOrderSysID.c_str(); }
   const char*                              GetUserData( void ) { return FData.c_str(); }
   const char*                              GetCxlUserData( void ) { return FCxlData.c_str(); }
   COrderStatus                             GetStatus( void ) { return FStatus; }
   nsOrderMessageDefine::SideEnum           GetSide( void ) { return FSide; }
   nsOrderMessageDefine::TimeInForceEnum    GetTimeInForce( void ) { return FTimeInForce; }
   nsOrderMessageDefine::OrderTypeEnum      GetOrderType( void ) { return FOrderType; }
   nsOrderMessageDefine::PositionEffectEnum GetPositionEffect( void ) { return FPositionEffect; }
public:
   COrderInfo( int FID, int SID, CThostFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine );
   COrderInfo( int FID, int SID, CUstpFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine );
   COrderInfo( int FID, int SID, KS_::CThostFtdcInputOrderField& Ord, const UFC::AnsiString& UserDefine );
   COrderInfo( int FID, int SID, TNewOrderMessage* Ord, const UFC::AnsiString& UserDefine );
   COrderInfo( CThostFtdcOrderField *pOrder );
   COrderInfo( KS_::CThostFtdcOrderField *pOrder );
   COrderInfo( int BranchNo, int SID, const UFC::AnsiString& ExchangeID, HSufxFutOpt::dataReportConfirm& Data );
   COrderInfo( int BranchNo, int SID, const UFC::AnsiString& ExchangeID, HSufxFutOpt::dataQryOrderResult *pOrder );
   COrderInfo( int BranchNo, int SID, HSufxStockOpt::dataQryOrderResult *pOrder );
   COrderInfo( int FID, int SID, CUstpFtdcOrderField *Ord );
   COrderInfo( UFC::NameValueMessage& TradeLog );
   void         CTPAccepted( const UFC::AnsiString& ExchangeID, const UFC::AnsiString& OrderLocalID );
   void         FemasAccepted( const UFC::AnsiString& ExchangeID, const UFC::AnsiString& OrderSysID, const UFC::AnsiString& OrderLocalID = "" );
   void         HSufxAccepted( const UFC::AnsiString& OrderLocalID );
   void         SetOrderLocalID( const UFC::AnsiString& OrderLocalID ) { FOrderLocalID = OrderLocalID; }
   void         EachangeAccepted( const UFC::AnsiString& OrderSysID, const UFC::AnsiString& OrderLocalID );
   void         EachangeAccepted( const UFC::AnsiString& OrderSysID );
   void         EachangeAccepted( const UFC::AnsiString& ExchangeID, const UFC::AnsiString& OrderSysID, const UFC::AnsiString& OrderLocalID );
   CxlStyle     Cancel( const UFC::AnsiString& UserDefine, int COrdRef );
   void         Fill( int qty );
   void         CloseOrder( void );
   static Int64 ToNID( int FrontID, int SessionID, int OrderRef );
   static int   ToOrderRef( Int64 NID );
};
//---------------------------------------------------------------------------
#endif
