//------------------------------------------------------------------------------------------------------------------------
#ifndef __RCRoutingBase_H
#define __RCRoutingBase_H
//------------------------------------------------------------------------------------------------------------------------
#include "../MDS/MDS.h"
#include "RCGProtocol.h"
#include "RCRuleDefine.h"
#include "../RCDataStore/RCUtility.h"
#include "../RCDataStore/mtdRC_ROUTING.h"
#include "../RCDataStore/mtdRC_LISTING.h"
#include "../RCDataStore/mtdRC_ROUTING_QUERY.h"
#include "../RCDataStore/mtdCUSTOMACCOUNT.h"
#include "../RCDataStore/mtdTHRESHOLD.h"
#include "../UFC/NameValueMessage.h"
#include "ThresholdManager.h"
//------------------------------------------------------------------------------------------------------------------------
using namespace MDS;
//------------------------------------------------------------------------------------------------------------------------
// Define
//------------------------------------------------------------------------------------------------------------------------
#define RCGatewayPortNumber		9801
//------------------------------------------------------------------------------------------------------------------------
#define RCMTypeFromLocal		66
#define RCMTypeFromRemote		88
#define DBMTypeFromSpeedy		0x78888866
//------------------------------------------------------------------------------------------------------------------------
#define RCRemainQueueMaxLimit	1000
//------------------------------------------------------------------------------------------------------------------------
extern ThresholdManager*               gThresholdManager;
extern UFC::PCriticalSection           gThresholdManagerCS;
//------------------------------------------------------------------------------------------------------------------------
// enumeration
//------------------------------------------------------------------------------------------------------------------------
typedef enum
{
    rtCheck  = 1,
    rtReport = 2
} enRoutingType;
//------------------------------------------------------------------------------------------------------------------------
typedef enum
{
	crErr_NotConnected     = -31, ///< [RCRoutingNodeRemote] - Connection not connected yet
	crErr_SendSocketError  = -32, ///< [RCRoutingNodeRemote] - Send socket error
	crErr_SendQueueError   = -21, ///< [RCRoutingNodeLocal] - Send queue error
	crErr_RejectOrder      = -8,  ///< ***[RCRoutingManager] - Break RCServer Check, Reject Order, for CheckLevel 6
	crErr_OrdNoRangeError  = -7,  ///< ***[RCRoutingManager] - OrderNo range error ( Only for SpeedyGateway )
	crErr_IPNotInRCListing = -6,  ///< [RCRoutingManager] - IP not in RC_LISTING talbe
	crErr_NoRoutingNode    = -5,  ///< [RCRoutingManager] - No Routing Node exist
	crErr_GarbageData      = -4,  ///< ***[RCRoutingManager] - Garbage data, source queue data length error
	crErr_RoutingTypeError = -3,  ///< ***[RCRoutingManager] - Design time error
	crErr_CheckLevelError  = -2,  ///< [RCRoutingManager] - CheckLevel not 1~9 , 0, 10
	crErr_RoutingNotFound  = -1,  ///< [RCRoutingManager] - Routing not found in RC_Routing table
	crL0_CheckByClient     = 0,   ///< [RCRoutingManager] - CheckLevel 0, no need check by RiskCenter
        crL10_NeedToDB         = 10,  ///< [RCRoutingManager] - CheckLevel 10, no need check by RiskCenter
        crL1_OK1               = 1,   ///< [RCRoutingManager] - CheckLevel 1, SendCheckRequest OK by 1st node
	crL2_OK1               = 2,   ///< [RCRoutingManager] - CheckLevel 2, SendCheckRequest OK by 1st node                
        crL6_OK1               = 6,   ///< [RCRoutingManager] - CheckLevel 6, SendCheckRequest OK by 1st node
	crL7_OK1               = 7,   ///< [RCRoutingManager] - CheckLevel 7, SendCheckRequest OK by 1st node
	crL8_OK1               = 8,   ///< [RCRoutingManager] - CheckLevel 8, SendCheckRequest OK by 1st node
	crL9_OK1               = 9,   ///< [RCRoutingManager] - CheckLevel 9, SendCheckRequest OK by 1st node
        crL1_OK2               = 21,  ///< [RCRoutingManager] - CheckLevel 1, SendCheckRequest OK by 2nd node
	crL2_OK2               = 22,  ///< [RCRoutingManager] - CheckLevel 2, SendCheckRequest OK by 2nd node                
        crL6_OK2               = 26,  ///< [RCRoutingManager] - CheckLevel 6, SendCheckRequest OK by 2nd node
	crL7_OK2               = 27,  ///< [RCRoutingManager] - CheckLevel 7, SendCheckRequest OK by 2nd node
	crL8_OK2               = 28,  ///< [RCRoutingManager] - CheckLevel 8, SendCheckRequest OK by 2nd node
	crL9_OK2               = 29,  ///< [RCRoutingManager] - CheckLevel 9, SendCheckRequest OK by 2nd node
	crL1_OKByFront         = 31,  ///< [RCRoutingManager] - CheckLevel 1, Threshold check OK by front
        crL2_OKByFront         = 32   ///< [RCRoutingManager] - CheckLevel 2, Threshold check OK by front                
} enRtnCheckRequest;
//------------------------------------------------------------------------------------------------------------------------
typedef enum
{
	orErr_NotConnected            = -31, ///< [RCRoutingNodeRemote] - Connection not connected yet
	orErr_SendSocketError         = -32, ///< [RCRoutingNodeRemote] - Send socket error
	orErr_SendQueueError          = -21, ///< [RCRoutingNodeLocal] - Send queue error
	orErr_IPNotInRCListing        = -6,  ///< [RCRoutingManager] - IP not in RC_LISTING talbe
	orErr_GarbageData             = -4,  ///< [RCRoutingManager] - Garbage data, source queue data length error
	orErr_RoutingTypeError        = -3,  ///< [RCRoutingManager] - Design time error
	orErr_RoutingNotFound         = -1,  ///< [RCRoutingManager] - Not found in RC_Routing table
	orSend_NoOneOK                = 0,   ///< [RCRoutingManager] - SendOrderReport no node OK     ( 0 )
	orSend_OK1                    = 1,   ///< [RCRoutingManager] - SendOrderReport OK by 1st node ( 1 ) 
	orSend_OK2                    = 2,   ///< [RCRoutingManager] - SendOrderReport OK by 2nd node ( 2 ) 
        orSend_OKAll                  = 3,   ///< [RCRoutingManager] - SendOrderReport OK by all node ( 1+2=3 )
        orSend_QueryOK1               = 4,   ///< [RCRoutingManager] - SendOrderReport OK by 1st Query ( 0100 )
        orSend_OK1AndQueryOK1         = 5,   ///< [RCRoutingManager] - SendOrderReport OK by 1st node and 1st Query ( 0101 )
        orSend_OK2AndQueryOK1         = 6,   ///< [RCRoutingManager] - SendOrderReport OK by 2nd node and 1st Query ( 0110 )
        orSend_OKAllAndQueryOK1       = 7,   ///< [RCRoutingManager] - SendOrderReport OK by all node and 1st Query ( 0111 )        
        orSend_QueryOK2               = 8,   ///< [RCRoutingManager] - SendOrderReport No node OK and OK by 2nd Query ( 1000 )
        orSend_OK1AndQueryOK2         = 9,   ///< [RCRoutingManager] - SendOrderReport OK by all node and 2nd Query ( 1001 )
        orSend_OK2AndQueryOK2         = 10,  ///< [RCRoutingManager] - SendOrderReport OK by 2nd node and 2nd Query ( 1010 )
        orSend_OKAllAndQueryOK2       = 11,  ///< [RCRoutingManager] - SendOrderReport OK by 1st node and 2nd Query ( 1011 )
        orSend_QueryAllOK             = 12,  ///< [RCRoutingManager] - SendOrderReport No node OK and OK by all Query ( 1100 )
        orSend_OK1andQueryAllOK       = 13,  ///< [RCRoutingManager] - SendOrderReport OK by 1st node and all Query ( 1101 )
        orSend_OK2AndQueryAllOK       = 14,  ///< [RCRoutingManager] - SendOrderReport OK by 2nd node and all Query ( 1110 )                
        orSend_OKAllAndQueryAllOK     = 15,  ///< [RCRoutingManager] - SendOrderReport OK by all node and all Query ( 1111 )
	orSend_InternalOK             = 99   ///< [RCRoutingNodeBase] - RCRoutingNode object internal use
} enRtnOrderReport;
//------------------------------------------------------------------------------------------------------------------------
// For Speedy use
// enum RCMarketEnum
//------------------------------------------------------------------------------------------------------------------------
typedef enum
{
	rcmTWFutures   = 0, ///< TAIFEX Taiwan Futures
	rcmTWOptions   = 1, ///< TAIFEX Taiwan Options
	rcmTSE         = 2, ///< TSEC   Taiwan equity
	rcmOTC         = 3, ///< OTC    Taiwan
	rcmPATSFutures = 4, ///< PATS          Futures
	rcmPATSOptions = 5, ///< PATS          Options
	rcmCNFutures   = 6, ///< CFFEX  China  Futures
	rcmCNOptions   = 7  ///< CFFEX  China  Options
 } RCMarketEnum;
//------------------------------------------------------------------------------------------------------------------------
// struct RCCheckExtendData
//------------------------------------------------------------------------------------------------------------------------
typedef struct s_RCCheckExtendData {
	char	AccountFlag;
	char	DayTradePolicy;
	int     CheckLevel;
} RCCheckExtendData;
//------------------------------------------------------------------------------------------------------------------------
// class RCSpeedyCheckParm
//------------------------------------------------------------------------------------------------------------------------
class RCSpeedyCheckParm
{
public:
	RCMarketEnum	MarketType;
	BOOL		IsOpt;
    BOOL		IsOffHour;
	Int64		Tick;
	Int32		SpeedyNID;
	UFC::AnsiString Order;
	Int32		OrderServerNID;
	UFC::AnsiString Key;
    UFC::AnsiString Source;
    UFC::AnsiString GDD;        // Parsed from Key
    UFC::AnsiString UDD;        // Parsed from Key
};
//------------------------------------------------------------------------------------------------------------------------
// class RCSpeedyReportParm
//------------------------------------------------------------------------------------------------------------------------
class RCSpeedyReportParm
{
public:
	RCMarketEnum	MarketType;	///< caller will just assign rcmCNFutures when CFFEX !!
	BOOL		IsOpt;		///< caller will no assign now!!
    BOOL		IsOffHour;
	Int64		Tick;
	Int32		SpeedyNID;
	UFC::AnsiString Report;
	UFC::AnsiString Key;
	Int32		posAE;
	Int32		posIB;
	Int32		posNID;
	Int32		posCNC;
	Int32		posIP;
};
//------------------------------------------------------------------------------------------------------------------------
// class RCSpeedyCheckResultParm
//------------------------------------------------------------------------------------------------------------------------
class RCSpeedyCheckResultParm
{
public:
	RCMarketEnum    MarketType;
	BOOL            IsOpt;
    BOOL            IsOffHour;
	Int64           Tick;
	Int32           SpeedyNID;
	UFC::AnsiString Order;
	UFC::AnsiString Key;
    UFC::AnsiString Source;
	UFC::AnsiString Code;
	UFC::AnsiString Msg;
};
//------------------------------------------------------------------------------------------------------------------------
// class RCSpeedyReportReplyParm
//------------------------------------------------------------------------------------------------------------------------
//class RCSpeedyReportReplyParm
//{
//public:
//	RCMarketEnum    MarketType;
//	BOOL            IsOpt;
//    BOOL            IsOffHour;
//	UInt32          Tick;
//	Int32           SpeedyNID;
//	UFC::AnsiString Order;
//	UFC::AnsiString Key;
//    UFC::AnsiString Source;
//	UFC::AnsiString Code;
//	UFC::AnsiString Msg;
//};
//------------------------------------------------------------------------------------------------------------------------
// class RCCheckResultListener
//------------------------------------------------------------------------------------------------------------------------
class RCCheckResultListener
{
public:
	// there are multiple thread will hook same OnCheckResultArrive()
	// so, the resouces in the OnCheckResultArrive() should be protected.
	virtual void OnCheckResultArrive( BOOL            IsOpt,
                                          UFC::UQueueData *Data, ///< A4 format
                                          int             Len ) = 0;
public:
	virtual ~RCCheckResultListener() {}
};
//------------------------------------------------------------------------------------------------------------------------
// class RCReportReplyListener
//------------------------------------------------------------------------------------------------------------------------
class RCReportReplyListener
{
public:
	// there are multiple thread will hook same OnReportReplyArrive()
	// so, the resouces in the OnReportReplyArrive() should be protected.
	virtual void OnReportReplyArrive( BOOL            IsOpt,
                                          UFC::UQueueData *Data,
                                          int             Len ) = 0;
public:
	virtual ~RCReportReplyListener() {}
};
//------------------------------------------------------------------------------------------------------------------------
// class RCSpeedyCheckResultListener
//------------------------------------------------------------------------------------------------------------------------
class RCSpeedyCheckResultListener
{
public:
	// there are multiple thread will hook same OnCheckResultArrive()
	// so, the resouces in the OnCheckResultArrive() should be protected.
	virtual void OnCheckResultArrive( RCSpeedyCheckResultParm& SpeedyParm ) = 0;
public:
	virtual ~RCSpeedyCheckResultListener() {}
};
//------------------------------------------------------------------------------------------------------------------------
// class RCRoutingNodeBase
//------------------------------------------------------------------------------------------------------------------------
class RCRoutingNodeBase
{
public: ///< For terminate internal thread if exist
	virtual void			StopInternalThread( void ) = 0;
public: ///< For enRoutingType = rtCheck
	virtual enRtnCheckRequest	SendCheckRequest( BOOL               IsOpt,
							  int                CheckLevel, ///< 1 ~ 9
                                                          UFC::UQueueStruct* QueueData,	 ///< A3 format
                                                          int                Len ) = 0;
public: ///< For enRoutingType = rtReport
	virtual enRtnOrderReport	SendOrderReport( BOOL               IsOpt,
                                                         int                CheckLevel,	///< 1 ~ 9
                                                         UFC::UQueueStruct* QueueData,	///< A5 format
                                                         int                Len ) = 0;
public: ///< For Speedy use
	virtual enRtnCheckRequest	SpeedySendCheckRequest( int                CheckLevel, ///< 1 ~ 9
                                                                RCSpeedyCheckParm& CheckParm ) = 0;
public: ///< For Threshold check use
        virtual BOOL			IsLogon( void ) = 0;
public:
	virtual ~RCRoutingNodeBase() {}
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
