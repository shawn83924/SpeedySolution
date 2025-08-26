#ifndef _TOrderStatusRequest_H_
#define _TOrderStatusRequest_H_
//--------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TBaseMessage.h"
//--------------------------------------------------------------------------
class TOrderStatusRequest : public TBaseMessage
{
public:
    TOrderStatusRequest( void );
public:
    ~TOrderStatusRequest(void);
public:
    UFC::AnsiString OrderType;

    void SetTSEOrderType( const char* Type );
    void SetPrice( double Value );
    void SetSide( nsOrderMessageDefine::SideEnum Value );
    void SetKeep( const char* Value );
    void SetStockSeqNo( long Value );
    void SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value );
	void SetTSETradeKind( const char* Type );
	void SetTSEObjectBroker( const char* Value );
	void SetTSEExchangeCode( const char* Type );
	void SetExchangeCode( const char* Type );
	void SetSrc(  const char* Value );
	void SetOrigNID( long Value );

	const char*                                GetSrc( void );
	const char*                                GetTSEOrderType( void );
	double                                     GetPrice( void );
	nsOrderMessageDefine::SideEnum             GetSide( void );
	const char*                                GetKeep( void );
	long                                       GetStockSeqNo( void );
	long                                       GetOrigNID( void );
	nsOrderMessageDefine::TradingSessionIDEnum GetTradingSessionID( void );
	const char*                                GetTSETradeKind( void );
	const char*                                GetTSEObjectBroker( void );
	const char*                                GetTSEExchangeCode( void );
	const char*                                GetExchangeCode( void );

};
//--------------------------------------------------------------------------
#endif

