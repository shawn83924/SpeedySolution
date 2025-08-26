//------------------------------------------------------------------------------------------------------------------------
#ifndef __mtdCUST_SUM_H
#define __mtdCUST_SUM_H
//------------------------------------------------------------------------------------------------------------------------
#include "../MDS/MDS.h"
//------------------------------------------------------------------------------------------------------------------------
// class mtdCUST_SUM
//------------------------------------------------------------------------------------------------------------------------
class mtdCUST_SUM
{
public: ///< Key size definition
	static int KeySize_PK;		///< IB + ACC_GRP_AE + CURRENCY
	static int KeySize_KEY1;	///< IB + ACC_GRP_AE
public: ///< Index Object pointer
	static MDS::MIndexObjectMultiple*	IndexObj_KEY1;
public: ///< Filed number definition
	static const int IB                = 0;		///< 分支別
	static const int ACC_GRP_AE        = 1;		///< 客戶帳號+群組+子帳
	static const int CURRENCY          = 2;		///< 幣別
	static const int CASH_FORWARD      = 3;		///< 昨日餘額
	static const int DEPOSIT           = 4;		///< 存款
	static const int WITHDRAWAL        = 5;		///< 提款
	static const int FLOAT_MARGIN      = 6;		///< 浮動損益
	static const int BALANCE           = 7;		///< 今日餘額
	static const int IM                = 8;		///< 原始保證金
	static const int MM                = 9;		///< 維持保證金
	static const int CANUSE_MARGIN     = 10;	///< 可動用保證金
	static const int PREMIUM           = 11;	///< 權利金
	static const int RISK_RATE         = 12;	///< 風險比率%
	static const int EQUITY            = 13;	///< 淨值
	static const int CALL_MARGIN       = 14;	///< 追繳保證金
	static const int OFFSET_GL         = 15;	///< 平倉損益
	static const int TAX_RATE          = 16;	///< 交易稅
	static const int SWAP_GL           = 17;	///< 履約損益
	static const int OPTION_IM         = 18;	///< 期權原始保證金
	static const int OPTION_MM         = 19;	///< 期權維持保證金
	static const int FLOAT_PREMIUM     = 20;	///< 變動權利金
	static const int ORDER_IM          = 21;	///< 委託原始保證金
	static const int OPEN_GL_YES       = 22;	///< 昨日未平倉損益
	static const int ORDER_PREMIUM     = 23;	///< 委託權利金
	static const int FULL_IM           = 24;	///< 全額原始保證金
	static const int FULL_MM           = 25;	///< 全額維持保證金
	static const int COMMISSION1       = 26;	///< 國內佣金
	static const int REDUCE_IM         = 27;	///< 期貨多空減收保證金
	static const int REDUCE_MM         = 28;	///< 多空減收維持保證金
	static const int COV_MATCH         = 29;	///< 留倉抵繳金額 ( 洗價用 )
	static const int COV_ORDER         = 30;	///< 委託抵繳金額 ( 洗價用 )
	static const int EQUITY_TOT        = 31;	///< 帳戶總市值   ( 洗價用 )
	static const int CAN_ORDER_MARGIN  = 32;	///< 可下單保證金 ( 洗價用 )
	static const int RISK_RATE_TOT     = 33;	///< 全帳戶風險比率 ( 洗價用 )
	static const int SPAN_LAST_CTIME   = 34;	///< 最後計算SPAN所使用的參數檔的 建檔時間 YYYYMMDDhhmmss
	static const int UPDATE_TIME       = 35;	///< 風險更新時間 ( 洗價用 HHMMSSmmm )
	static const int SUPPLEMENT_MARGIN = 36;	///< 追加保證金
	static const int RE_OPEN_GL_YES    = 37;        ///< 扣昨未平倉GL
        static const int RE_FLOATING       = 38;        ///< 扣浮動GL
	static const int PRE_SALE_GOLD_AMT = 39;        ///< 預扣黃金轉現貨價款
        static const int SALE_GOLD_AMT     = 40;        ///< 黃金轉現貨價款
        static const int DSP_FLOAT_MARGIN  = 41;        ///< T結浮動
        static const int DSP_FLOAT_PREMIUM = 42;        ///< T結市值
        static const int DSP_IM            = 43;        ///< T結保證金
        static const int DSP_RISK_RATE     = 44;        ///< T結風險指標
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
