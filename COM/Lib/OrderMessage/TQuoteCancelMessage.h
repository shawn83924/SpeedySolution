#ifndef _TQuoteCancelMessage_H_
#define _TQuoteCancelMessage_H_
//--------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TBaseMessage.h"
//--------------------------------------------------------------------------
class TQuoteCancelMessage :
	public TBaseMessage
{
public:
    TQuoteCancelMessage( void );
public:
    ~TQuoteCancelMessage(void);
public:
    void SetQuoteID( const char* Value );
    void SetBidPrice( double Value );
    void SetAskPrice( double Value );
	void SetOrderSource( nsOrderMessageDefine::OrderSourceEnum Value );
	void SetMarketDataSource( const char* Value );
	void SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value );

	nsOrderMessageDefine::OrderSourceEnum      GetOrderSource( void );
	nsOrderMessageDefine::TradingSessionIDEnum GetTradingSessionID( void );
	const char*                                GetMarketDataSource( void );
	const char*                                GetQuoteID( void );
	double                                     GetBidPrice( void );
	double                                     GetAskPrice( void );
};
//--------------------------------------------------------------------------
#endif

