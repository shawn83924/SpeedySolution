#ifndef _TReplaceOrderMessage_H_
#define _TReplaceOrderMessage_H_
//--------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TBaseMessage.h"
//--------------------------------------------------------------------------
class TReplaceOrderMessage : public TBaseMessage
{
public:
    TReplaceOrderMessage( void );
public:
    ~TReplaceOrderMessage(void);
public:
    void SetMaturityMonthYear( const char* Value );
    void SetOrigClOrdID( const char* Value );
    void SetOrderType( nsOrderMessageDefine::OrderTypeEnum Value );
    void SetSide( nsOrderMessageDefine::SideEnum Value );
    void SetTimeInForce( nsOrderMessageDefine::TimeInForceEnum Value );
//    void SetEventType( nsOrderMessageDefine::EventTypeEnum Value );
    void SetPositionEffect( nsOrderMessageDefine::PositionEffectEnum Value );
    void SetOrderSource( nsOrderMessageDefine::OrderSourceEnum Value );
    void SetMarketDataSource( const char* Value );
    void SetOrderQty( long Value );
    void SetPrice( double Value );
    void SetStopPx( double Value );
//    void SetStrikePrice( double Value );
    void SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value );
    void SetTSEOrderType( const char* Type );
    void SetTSEExchangeCode( const char* Type );
    void SetExchangeCode( const char* Type );
    void SetTSETradeKind( const char* Type );
    void SetTSEObjectBroker(  const char* Value );
    void SetStockSeqNo( long Value );
    void SetKeep( const char* Value );
    void SetSrc( const char* Value );
    void SetIBNO( const char* Value );
    void SetEUCD( const char* Value );
    void SetExpireDate( const char* Value );

    const char*                                GetMaturityMonthYear( void );
    const char*                                GetOrigClOrdID( void );
    nsOrderMessageDefine::OrderTypeEnum        GetOrderType( void );
    nsOrderMessageDefine::SideEnum             GetSide( void );
    nsOrderMessageDefine::TimeInForceEnum      GetTimeInForce( void );
    nsOrderMessageDefine::PositionEffectEnum   GetPositionEffect( void );
    nsOrderMessageDefine::TradingSessionIDEnum GetTradingSessionID( void );
    nsOrderMessageDefine::OrderSourceEnum      GetOrderSource( void );
    const char*                                GetMarketDataSource( void );
    long                                       GetOrderQty( void );
    double                                     GetPrice( void );
    double                                     GetStopPx( void );
    const char*                                GetTSEOrderType( void );
    const char*                                GetTSEExchangeCode( void );
    const char*                                GetExchangeCode( void );
    const char*                                GetTSETradeKind( void );
    const char*                                GetTSEObjectBroker( void );
    long                                       GetStockSeqNo( void );
    const char*                                GetKeep( void );
    const char*                                GetSrc( void );
    const char*                                GetIBNO( void );
    const char*                                GetEUCD( void );
    const char*                                GetExpireDate( void );
};
//--------------------------------------------------------------------------
#endif
