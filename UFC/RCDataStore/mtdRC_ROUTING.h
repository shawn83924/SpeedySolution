//------------------------------------------------------------------------------------------------------------------------
#ifndef __mtdRC_ROUTING_H
#define __mtdRC_ROUTING_H
//------------------------------------------------------------------------------------------------------------------------
// class mtdRC_ROUTING
//------------------------------------------------------------------------------------------------------------------------
class mtdRC_ROUTING
{
public: ///< Key size definition
	static int KeySize_PK;		///< RC_GROUP + IB + ACCOUNT
public: ///< Filed number definition
	static const int RC_GROUP        = 0;	///< 群組
	static const int IB              = 1;	///< 分支別
	static const int ACCOUNT         = 2;	///< 帳號
	static const int CHECK_LEVEL     = 3;	///< 風控等級
	static const int IP1             = 4;	///< 主控中台
	static const int IP2             = 5;	///< 備控中台
	static const int CUSTOM_KIND     = 6;	///< 客戶類別
	static const int DAYTRADE_POLICY = 7;	///< 當沖類別
	static const int CAN_ORDER_LEVEL = 8;	///< 可下單保證金等級
	static const int RISK_RATE_LEVEL = 9;	///< 全帳戶風險比率%等級
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
