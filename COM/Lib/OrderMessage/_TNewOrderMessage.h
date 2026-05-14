#ifndef _TNewOrderMessage_H_
#define _TNewOrderMessage_H_
//--------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TBaseMessage.h"
//--------------------------------------------------------------------------
class TNewOrderMessage : public TBaseMessage
{
private:
    UFC::AnsiString FMaturityMonthYear;
    UFC::AnsiString FMarketDataSource;
    UFC::AnsiString FExchangeCode;
    UFC::AnsiString FTSEOrderType;
    UFC::AnsiString FTSEExchangeCode;
    UFC::AnsiString FTSETradeKind;
    UFC::AnsiString FTSEObjectBroker;
    UFC::AnsiString FKeep;
    UFC::AnsiString FSrc;
    UFC::AnsiString FIBNO;
    UFC::AnsiString FEUCD;
    double          FPrice;
    double          FStopPx;
    long            FOrderQty;
    long            FStockSeqNo;
    long            FDayTrade;
    nsOrderMessageDefine::OrderTypeEnum        FOrderType;
    nsOrderMessageDefine::SideEnum             FSide;
    nsOrderMessageDefine::TimeInForceEnum      FTimeInForce;
    nsOrderMessageDefine::PositionEffectEnum   FPositionEffect;
    nsOrderMessageDefine::CombHedgeFlagEnum    FCombHedgeFlag;
    nsOrderMessageDefine::OrderSourceEnum      FOrderSource;
    nsOrderMessageDefine::TradingSessionIDEnum FTradingSessionID;
    UFC::AnsiString FExpireDate;  //Tag 432 for Tag 59 TimeInForce:Good till Date
    UFC::PHashedList<UFC::AnsiString, UFC::AnsiString*> FOrderAttributes;

public:
    TNewOrderMessage( void );
    ~TNewOrderMessage(void);
public:
    void SetMaturityMonthYear( const char* Value )                              { FMaturityMonthYear = Value; }
    void SetOrderType( nsOrderMessageDefine::OrderTypeEnum Value )              { FOrderType = Value; }
    void SetSide( nsOrderMessageDefine::SideEnum Value )                        { FSide = Value; }
    void SetTimeInForce( nsOrderMessageDefine::TimeInForceEnum Value )          { FTimeInForce = Value; }
    void SetPositionEffect( nsOrderMessageDefine::PositionEffectEnum Value );
    void SetCombHedgeFlag( nsOrderMessageDefine::CombHedgeFlagEnum Value )      { FCombHedgeFlag = Value; }
    void SetOrderSource( nsOrderMessageDefine::OrderSourceEnum Value )          { FOrderSource = Value; }
    void SetMarketDataSource( const char* Value );
    void SetPrice( double Value )                                               { FPrice = Value; }
    void SetStopPx( double Value )                                              { FStopPx = Value; }
    void SetOrderQty( long Value )                                              { FOrderQty = Value; }
    void SetExchangeCode( const char* Type )                                    { FExchangeCode = Type; }
    void SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value ){ FTradingSessionID = Value; }
    void SetTSEOrderType( const char* Type )                                    { FTSEOrderType = Type; }
    void SetTSEExchangeCode( const char* Type )                                 { FTSEExchangeCode = Type; }
    void SetTSETradeKind( const char* Type )                                    { FTSETradeKind = Type; }
    void SetTSEObjectBroker( const char* Value )                                { FTSEObjectBroker = Value; }
    void SetStockSeqNo( long Value )                                            { FStockSeqNo = Value; }
    void SetKeep( const char* Value )                                           { FKeep = Value; }
    void SetSrc( const char* Value )                                            { FSrc = Value; }
    void SetIBNO( const char* Value )                                           { FIBNO = Value; }
    void SetDayTrade( long Value )                                              { FDayTrade = Value; }
    void SetEUCD( const char* Value )                                           { FEUCD = Value; }
    void SetExpireDate( const char* Value )                                     { FExpireDate = Value; }
    void SetAttribute(const char* Key, const char* Value);
    
    const char*                                GetMaturityMonthYear( void )     { return FMaturityMonthYear.c_str(); }
    nsOrderMessageDefine::OrderTypeEnum        GetOrderType( void )	            { return FOrderType; }
    nsOrderMessageDefine::SideEnum             GetSide( void )                  { return FSide; }
    nsOrderMessageDefine::TimeInForceEnum      GetTimeInForce( void )           { return FTimeInForce; }
    nsOrderMessageDefine::PositionEffectEnum   GetPositionEffect( void )        { return FPositionEffect; }
    nsOrderMessageDefine::CombHedgeFlagEnum    GetCombHedgeFlag( void )         { return FCombHedgeFlag; }
    nsOrderMessageDefine::TradingSessionIDEnum GetTradingSessionID( void );
    nsOrderMessageDefine::OrderSourceEnum      GetOrderSource( void )           { return FOrderSource; }
    const char*                                GetMarketDataSource( void )      { return FMarketDataSource.c_str();}
    double                                     GetPrice( void )                 { return FPrice; }
    double                                     GetStopPx( void )                { return FStopPx; }
    long                                       GetOrderQty( void )              { return FOrderQty; }
    const char*                                GetTSEOrderType( void )          { return FTSEOrderType.c_str();}
    const char*                                GetTSEExchangeCode( void )       { return FTSEExchangeCode.c_str();}
    const char*                                GetExchangeCode( void );
    const char*                                GetTSETradeKind( void )          { return FTSETradeKind.c_str();}
    const char*                                GetTSEObjectBroker( void )       { return FTSEObjectBroker.c_str();}
    long                                       GetStockSeqNo( void )            { return FStockSeqNo; }
    const char*                                GetKeep( void )                  { return FKeep.c_str();}
    const char*                                GetSrc( void )                   { return FSrc.c_str();}
    const char*                                GetIBNO( void )                  { return FIBNO.c_str();}
    long                                       GetDayTrade( void )              { return FDayTrade; }
    const char*                                GetEUCD( void )                  { return FEUCD.c_str();}
    const char*                                GetExpireDate( void )            { return FExpireDate.c_str(); }
    BOOL                                       GetAttribute( UFCType::Int32 index, UFC::AnsiString& key, UFC::AnsiString& value );
    UFCType::Int32                             GetAttributeCount() { return FOrderAttributes.ItemCount(); }
};
//--------------------------------------------------------------------------
#endif



