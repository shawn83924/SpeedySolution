#ifndef _TQuoteMessage_H_
#define _TQuoteMessage_H_
//--------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TBaseMessage.h"
//--------------------------------------------------------------------------
class TQuoteMessage : public TBaseMessage
{
public:
    TQuoteMessage( void );
public:
    ~TQuoteMessage( void );
public:
    void SetTimeInForce( nsOrderMessageDefine::TimeInForceEnum Value );
    void SetQuoteID( const char* Value );    
    void SetBidQty( long Value );
    void SetAskQty( long Value );
    void SetBidPrice( double Value );
	void SetAskPrice( double Value );
	void SetOrderSource( nsOrderMessageDefine::OrderSourceEnum Value );
	void SetMarketDataSource( const char* Value );
	void SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value );

	nsOrderMessageDefine::TimeInForceEnum      GetTimeInForce( void );
	nsOrderMessageDefine::OrderSourceEnum      GetOrderSource( void );
	nsOrderMessageDefine::TradingSessionIDEnum GetTradingSessionID( void );
	const char*                                GetMarketDataSource( void );
	const char*                                GetQuoteID( void );
	long                                       GetBidQty( void );
	long                                       GetAskQty( void );
	double                                     GetBidPrice( void );
	double                                     GetAskPrice( void );
};
//--------------------------------------------------------------------------
#endif

