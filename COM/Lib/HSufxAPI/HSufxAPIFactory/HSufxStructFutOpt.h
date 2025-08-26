//---------------------------------------------------------------------------
#ifndef HSufxStructFutOptH
#define HSufxStructFutOptH
//---------------------------------------------------------------------------
#include "OrderMessageDefine.h"
#include "t2sdk_interface.h"
//---------------------------------------------------------------------------
namespace HSufxFutOpt
{
//---------------------------------------------------------------------------
struct dataRefToOrderKey
{
	UFC::AnsiString ExchangeCode;
	int             entrust_no;
};
//---------------------------------------------------------------------------
struct dataReQueryCmd
{
	int    FunctionID;
	char   position_str[100+1];
};
//---------------------------------------------------------------------------
struct dataLoginResult
{
	bool   IsLoginOK;
	char   ErrMsg[500+1];
	int    branch_no;
	char   fund_account[18+1];
	char   user_token[512+1];
	char   client_id[18+1];
	int    sysnode_id;
	int    session_no;
	int    init_date;
};
//---------------------------------------------------------------------------
struct dataLogoutResult
{
	bool   IsLogoutOK;
};
//---------------------------------------------------------------------------
struct dataReplyHeartBeatMsg
{
	char* Buffer;
	int   BufferLen;
};
//---------------------------------------------------------------------------
struct dataChangePwdResult
{
	bool   IsChangePwdOK;
	char   ErrMsg[500+1];
};
//---------------------------------------------------------------------------
struct dataSubscribeResult
{
	bool   IsSubscribeOK;
	int    IssueType;
	char   ErrMsg[500+1];
};
//---------------------------------------------------------------------------
struct dataQryOrderResult
{
	int    batch_no;
	int    entrust_no;
	char   fund_account[18+1];
	char   futu_exch_type[4+1];
	char   futures_account[12+1];
	char   futu_code[30+1];
	char   money_type[3+1];
	char   futures_direction;
	double futu_entrust_price;
	char   entrust_bs;
	char   hedge_type;
	char   entrust_status;
	int    entrust_time;
	int    report_time;
	int    business_amount;
	int    entrust_amount;
	int    withdraw_amount;
	double curr_entrust_margin;
	char   entrust_type;
	char   confirm_id[20+1];
	double curr_entrust_fare;
	char   forceclose_reason;
	char   error_message[255+1];
	double business_balance;
	char   entrust_prop[3+1];
	char   arbit_code[30+1];
	char   second_code[30+1];
	char   weave_type;
	double spring_price;
	char   time_condition;
	int    valid_date;
	char   volume_condition;
	int    fex_min_volume;
	char   report_id[32+1];
	double curr_entrust_premium;
	int    session_no;
	char   entrust_reference[32+1];
	char   entrust_occasion[32+1];
	char   position_str[100+1];
};
//---------------------------------------------------------------------------
struct dataQryFillResult
{
	int    entrust_no;
	char   futu_exch_type[4+1];
	char   futures_account[12+1];
	char   futu_code[30+1];
	char   money_type[3+1];
	char   futures_direction;
	char   entrust_bs;
	char   hedge_type;
	double futu_business_price;
	char   entrust_type;
	int    business_time;
	int    business_amount;
	double business_fare;
	char   business_id[16+1];
	char   position_str[100+1];
	char   confirm_id[20+1];
	int    session_no;
	char   entrust_reference[32+1];
	char   entrust_occasion[32+1];
};
//---------------------------------------------------------------------------
struct dataQryAssetResult
{
	char   money_type[3+1];
	double current_balance;
	double enable_balance;
	double fetch_balance;
	double frozen_balance;
	double pre_entrust_balance;
	double entrust_balance;
	double hold_income;
	double hold_income_float;
	double begin_equity_balance;
	double equity_balance;
	double interest_balance;
	double drop_income;
	double drop_income_float;
	double business_fare;
	double hold_margin;
	double exch_hold_margin;
	double client_risk_rate;
	double exch_risk_rate;
	double out_premium;
	double in_premium;
	double market_value;
};
//---------------------------------------------------------------------------
struct dataQryPositionResult
{
	bool   IsEndNotify;
	char   futu_exch_type[4+1];
	char   futures_account[12+1];
	char   futu_code[30+1];
	char   money_type[3+1];
	char   entrust_bs;			///< 買賣方向    '1'-買入 '2'-賣出
	int    begin_amount;		///< 期初數量
	int    enable_amount;		///< 可用數量
	int    real_enable_amount;	///< 當日開倉可用數量
	double hold_income_float;	///< 持倉浮動盈虧
	double hold_income;			///< 期貨盯市盈虧
	double hold_margin;			///< 持倉保證金
	double average_price;		///< 平均價
	double futu_last_price;		///< 最新價格
	char   hedge_type;			///< 投機套保類型    '0'-投機 '1'-套保 '2'-套利 '3'-做市商
	int    real_amount;			///< 成交數量
	double real_open_balance;	///< 回報開倉金額
	double old_open_balance;	///< 老倉持倉金額
	int    real_current_amount;	///< 今總持倉 double??
	int    old_current_amount;	///< 老倉持倉數量
	char   futu_product_type;	///< 產品類別    '1'-期貨 '2'-期權 '3'-組合 '4'-即期 '5'-期轉現
	char   position_str[100+1];
};
//---------------------------------------------------------------------------
struct dataExchStatusResult
{
	char   futu_exch_type[4+1];
	char   exchange_status;		///< 與文件不同 交易所狀態 '0'-連接斷開 '1'-開盤前 '2'-集合報價 '3'-競價平衡 '4'-競價撮合 '5'-連續交易 '6'-暫停交易 '7'-閉市 'X'-未知
	char   exch_status;			///< 與文件不同 交易狀態   '0'-停止 '1'-開始
};
//---------------------------------------------------------------------------
struct dataExchTimeResult
{
	int    CZCE_curr_time;			///< 與文件不同, 文件 N8  實際資料 N9 HHMMSSmmm
	int    DCE_curr_time;			///< 與文件不同, 文件 N8  實際資料 N9 HHMMSSmmm
	int    SHFE_curr_time;			///< 與文件不同, 文件 N8  實際資料 N9 HHMMSSmmm
	int    CFFEX_curr_time;			///< 與文件不同, 文件 N8  實際資料 N9 HHMMSSmmm
};
//---------------------------------------------------------------------------
struct dataPendingExecution
{
	int    hSend;
	int    entrust_no;
	char   entrust_reference[32+1];
	//char   futu_exch_type[4+1]; ///< 有些環境(ex:魯証) 沒回這個欄位
};
//---------------------------------------------------------------------------
struct dataRejectExecution
{
	nsOrderMessageDefine::CxlRejResponseToEnum ResponseTo;
	int    hSend;
	char   ErrMsg[500+1];
};
//---------------------------------------------------------------------------
struct dataPushReport
{
	char   LY;
	char   QH[512];
};
//---------------------------------------------------------------------------
struct dataPushConfirm
{
	char*  entrust_no;
	char*  futures_account;
	char*  futu_exch_type;
	char*  futu_code;
	char*  entrust_bs;
	char*  futures_direction;
	char*  hedge_type;
	char*  fund_account;
	char*  report_id;
	char*  firm_id;
	char*  operator_no;
	char*  client_group;
	char*  entrust_amount;
	char*  total_business_amount;
	char*  withdraw_amount;
	char*  futu_entrust_price;
	char*  entrust_status;
	char*  branch_no;
	char*  batch_no;
	char*  entrust_type;
	char*  amount_per_hand;
	char*  forcedrop_reason;
	char*  init_date;
	char*  entrust_time;
	char*  confirm_id;
	char*  entrust_occasion;
	char*  futu_entrust_price2;
	char*  entrust_prop;
	char*  arbit_code;
	char*  entrust_reference;
	char*  error_message;
	char*  position_str;
	char*  session_no;
	char*  second_code;
	char*  weave_type;
	char*  delta_entrust_fare;
	char*  delta_entrust_margin;
	char*  delta_entrust_premium;
};
//---------------------------------------------------------------------------
struct dataPushFill
{
	char*  entrust_no;
	char*  futures_account;
	char*  futu_exch_type;
	char*  business_no;
	char*  futu_code;
	char*  entrust_bs;
	char*  futures_direction;
	char*  futu_business_price;
	char*  business_amount;
	char*  hedge_type;
	char*  fund_account;
	char*  report_id;
	char*  firm_id;
	char*  operator_no;
	char*  client_group;
	char*  entrust_amount;
	char*  total_business_amount;
	char*  withdraw_amount;
	char*  futu_entrust_price;
	char*  entrust_status;
	char*  branch_no;
	char*  batch_no;
	char*  entrust_type;
	char*  amount_per_hand;
	char*  forcedrop_reason;
	char*  init_date;
	char*  business_time;
	char*  confirm_id;
	char*  entrust_occasion;
	char*  futu_entrust_price2;
	char*  entrust_prop;
	char*  arbit_code;
	char*  entrust_reference;
	char*  position_str;
	char*  session_no;
};
//---------------------------------------------------------------------------
struct dataReportConfirm
{
	UFC::AnsiString entrust_no;
	UFC::AnsiString futu_exch_type;
	UFC::AnsiString futu_code;
	UFC::AnsiString fund_account;
	UFC::AnsiString entrust_prop;
	UFC::AnsiString error_message;
	char   entrust_bs;
	char   futures_direction;
	char   entrust_status;
	int    entrust_amount;
	double futu_entrust_price;
public:
	dataReportConfirm( const dataReportConfirm& Data )
	: entrust_no( Data.entrust_no )
	, futu_exch_type( Data.futu_exch_type )
	, futu_code( Data.futu_code )
	, fund_account( Data.fund_account )
	, entrust_prop( Data.entrust_prop )
	, error_message( Data.error_message )
	, entrust_bs( Data.entrust_bs )
	, futures_direction( Data.futures_direction )
	, entrust_status( Data.entrust_status )
	, entrust_amount( Data.entrust_amount )
	, futu_entrust_price( Data.futu_entrust_price )
	{ }
	dataReportConfirm( const dataPushConfirm& Data )
	: entrust_no( Data.entrust_no )
	, futu_exch_type( Data.futu_exch_type )
	, futu_code( Data.futu_code )
	, fund_account( Data.fund_account )
	, entrust_prop( Data.entrust_prop )
	, error_message( Data.error_message )
	{
		UFC::AnsiString StrPrice( Data.futu_entrust_price );
		futu_entrust_price = StrPrice.ToDouble();
		entrust_bs         = Data.entrust_bs[0];
		futures_direction  = Data.futures_direction[0];
		entrust_status     = Data.entrust_status[0];
		entrust_amount     = atoi( Data.entrust_amount );
	}
};
//---------------------------------------------------------------------------
struct dataReportFill
{
	UFC::AnsiString entrust_no;
	UFC::AnsiString futu_exch_type;
	UFC::AnsiString business_no;
	UFC::AnsiString fund_account;
	char   entrust_bs;
	char   entrust_status;
	double futu_business_price;
	int    business_amount;
	int    business_time;
public:
	dataReportFill( dataReportFill& Data )
	: entrust_no( Data.entrust_no )
	, futu_exch_type( Data.futu_exch_type )
	, business_no( Data.business_no )
	, fund_account( Data.fund_account )
	, entrust_status( Data.entrust_status )
	, entrust_bs( Data.entrust_bs )
	, futu_business_price( Data.futu_business_price )
	, business_amount( Data.business_amount )
	, business_time( Data.business_time )
	{ }
	dataReportFill( dataPushFill& Data )
	: entrust_no( Data.entrust_no )
	, futu_exch_type( Data.futu_exch_type )
	, business_no( Data.business_no )
	, fund_account( Data.fund_account )
	{
		UFC::AnsiString StrPrice( Data.futu_business_price );
		futu_business_price = StrPrice.ToDouble();
		entrust_bs          = Data.entrust_bs[0];
		entrust_status      = Data.entrust_status[0];
		business_amount     = atoi( Data.business_amount );
		business_time       = atoi( Data.business_time );
	}
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
