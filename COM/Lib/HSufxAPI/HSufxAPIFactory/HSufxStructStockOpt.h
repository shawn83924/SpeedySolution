//---------------------------------------------------------------------------
#ifndef HSufxStructStockOptH
#define HSufxStructStockOptH
//---------------------------------------------------------------------------
#include "OrderMessageDefine.h"
#include "t2sdk_interface.h"
//---------------------------------------------------------------------------
namespace HSufxStockOpt
{
//---------------------------------------------------------------------------
struct dataLoginResult
{
	bool   IsLoginOK;
	char   ErrMsg[500+1];
	int    branch_no;
	char   fund_account[18+1];
	char   user_token[40+1];
	char   client_id[18+1];
	int    sysnode_id;
	int    login_times;
	int    init_date;
};
//---------------------------------------------------------------------------
struct dataLogoutResult
{
	bool   IsLogoutOK;
};
//---------------------------------------------------------------------------
struct dataReplyHeartBeat
{
	REQ_DATA ReqData;
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
	bool   IsEndNotify;
	int    init_date;
	int    batch_no;
	int    entrust_no;
	char   exchange_type[4+1];
	char   fund_account[18+1];
	char   option_account[13+1];
	char   option_code[8+1];
	char   optcontract_id[32+1];
	char   stock_code[6+1];
	char   entrust_bs;
	char   entrust_oc;
	char   covered_flag;
	double opt_entrust_price;
	double entrust_amount;
	double business_amount;
	double opt_business_price;
	int    report_no;
	int    report_time;
	char   entrust_type;
	char   entrust_status;
	int    entrust_time;
	int    entrust_date;
	char   entrust_prop[3+1];
	char   entrust_src;
	char   trade_name[64+1];
	char   option_name[32+1];
	char   cancel_info[256+1];
	double withdraw_amount;
	char   withdraw_flag;
	char   position_str[32+1];
};
//---------------------------------------------------------------------------
struct dataQryFillResult
{
	bool   IsEndNotify;
	int    init_date;
	int    serial_no;
	char   exchange_type[4+1];
	//char   fund_account[18+1];
	char   option_account[13+1];
	char   option_code[8+1];
	char   optcontract_id[32+1];
	char   stock_code[6+1];
	char   entrust_bs;
	char   entrust_oc;
	char   covered_flag;
	double opt_business_price;
	double business_amount;
	int    business_time;
	char   real_type;
	char   real_status;
	int    business_times;
	int    entrust_no;
	double business_balance;
	char   option_name[32+1];
	char   trade_name[64+1];
	int    report_no;
	char   entrust_prop[3+1];
	char   business_id[32+1];
	char   position_str[32+1];
};
//---------------------------------------------------------------------------
struct dataQryAssetResult
{
	double total_asset;
	double fund_asset;
	double current_balance;
	double enable_balance;
	double enable_bail_balance;
	double used_bail_balance;
	double used_pur_balance;
	double enable_pur_balance;
	double pur_quota;
	double income_balance;
	//double frozen_balance; ///<ゅン⊿Τ
	double risk_degree;
	double real_risk_degree;
	double dyna_market_value;
	double real_used_bail;
	char   optrisk_type;
};
//---------------------------------------------------------------------------
struct dataQryPositionResult
{
	bool   IsEndNotify;
	char   fund_account[18+1];
	char   exchange_type[4+1];
	char   option_account[13+1];
	char   opthold_type;			///< '0'-舦よ(Buy) '1'-竡叭よ(Sell) '2'-称よ
	char   option_code[8+1];
	char   stock_code;
	char   optcontract_id[32+1];
	char   option_name[32+1];
	char   option_type;				///< 'C'-Call 'P'-Put
	double current_amount;			///< 讽玡计秖
	double hold_amount;				///< Τ计秖(珹〆癠ゼΘユ场だ)
	double enable_amount;			///< ノ计秖
	double real_open_amount;		///< さ秨计秖
	double real_drop_amount;		///< さキ计秖
	double entrust_drop_amount;		///< さキ〆癠秖
	double last_price;				///< 夹靡ㄩ程穝基
	double opt_last_price;			///< 程穝基
	double opt_cost_price;			///< 秨А基
	double exercise_price;
	double market_value;
	double av_buy_optprice;
	double av_income_balance;
	double cost_balance;
	double income_balance;
	double exercise_income;
	double duty_used_bail;
	int    exercise_date;
	int    amount_per_hand;
	char   position_str[32+1];
};
//---------------------------------------------------------------------------
struct dataPendingExecution
{
	int    hSend;
	int    entrust_no;
};
//---------------------------------------------------------------------------
struct dataConfirmExecution
{
	int    branch_no;
	char   fund_account[18+1];
	int    entrust_no;
	char   entrust_status;
	char   entrust_type;
};
//---------------------------------------------------------------------------
struct dataRejectExecution
{
	nsOrderMessageDefine::CxlRejResponseToEnum ResponseTo;
	int    hSend;
	char   ErrMsg[500+1];
};
//---------------------------------------------------------------------------
struct dataRejectByExh
{
	nsOrderMessageDefine::CxlRejResponseToEnum ResponseTo;
	int    branch_no;
	char   fund_account[18+1];
	int    entrust_no;
	char   entrust_status;
	char   extern_code[8+1];
	int    business_time;
};
//---------------------------------------------------------------------------
struct dataCanceledExecution
{
	int    branch_no;
	char   fund_account[18+1];
	int    entrust_no;
	char   entrust_status;
	int    business_time;
};
//---------------------------------------------------------------------------
struct dataFillExecution
{
	int    branch_no;
	char   fund_account[18+1];
	int    entrust_no;
	char   entrust_status;
	char   business_id[32+1];
	double business_amount;
	double opt_business_price;
	int    business_time;
	char   entrust_bs;
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
