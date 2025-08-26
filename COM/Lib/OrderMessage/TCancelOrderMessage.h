#ifndef _TCancelOrderMessage_H_
#define _TCancelOrderMessage_H_
//--------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TBaseMessage.h"
//--------------------------------------------------------------------------
class TCancelOrderMessage :	public TBaseMessage
{
private:
	UFC::AnsiString FMaturityMonthYear;
	UFC::AnsiString FMarketDataSource;
	UFC::AnsiString FExchangeCode;
	UFC::AnsiString FTSEOrderType;
	UFC::AnsiString FTSEExchangeCode;
	UFC::AnsiString FTSETradeKind;
	UFC::AnsiString FTSEObjectBroker;
	UFC::AnsiString FOrigClOrdID;
	UFC::AnsiString FKeep;
	UFC::AnsiString FSrc;
	UFC::AnsiString FIBNO;
	UFC::AnsiString FEUCD;
	double          FPrice;
	long            FOrderQty;
	long            FStockSeqNo;
	UFCType::Int64  FCNID;
	nsOrderMessageDefine::OrderTypeEnum        FOrderType;
	nsOrderMessageDefine::SideEnum             FSide;
	nsOrderMessageDefine::TimeInForceEnum      FTimeInForce;
	nsOrderMessageDefine::OrderSourceEnum      FOrderSource;
	nsOrderMessageDefine::TradingSessionIDEnum FTradingSessionID;
public:
	TCancelOrderMessage( void );
public:
	~TCancelOrderMessage(void);
public:
	void SetMaturityMonthYear( const char* Value )                              { FMaturityMonthYear = Value; }
	void SetOrigClOrdID( const char* Value )                                    { FOrigClOrdID = Value; }
	void SetOrderType( nsOrderMessageDefine::OrderTypeEnum Value )              { FOrderType = Value; }
	void SetSide( nsOrderMessageDefine::SideEnum Value )						{ FSide = Value; }
	void SetTimeInForce( nsOrderMessageDefine::TimeInForceEnum Value )          { FTimeInForce = Value; }
	void SetOrderSource( nsOrderMessageDefine::OrderSourceEnum Value )          { FOrderSource = Value; }
	void SetMarketDataSource( const char* Value );
	void SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value ){ FTradingSessionID = Value; }
	void SetPrice( double Value )                                               { FPrice = Value; }
	void SetOrderQty( long Value )                                              { FOrderQty = Value; }
	void SetCNID( UFCType::Int64 Value )                                        { FCNID = Value; }
	void SetTSEOrderType( const char* Type )                                    { FTSEOrderType = Type; }
	void SetTSEExchangeCode( const char* Type )                                 { FTSEExchangeCode = Type; }
	void SetExchangeCode( const char* Type )                                    { FExchangeCode = Type; }
	void SetTSETradeKind( const char* Type )                                    { FTSETradeKind = Type; }
	void SetTSEObjectBroker(  const char* Value )                               { FTSEObjectBroker = Value; }
	void SetStockSeqNo( long Value )                                            { FStockSeqNo = Value; }
	void SetKeep( const char* Value )                                           { FKeep = Value; }
	void SetSrc( const char* Value )                                            { FSrc = Value; }
	void SetIBNO( const char* Value )                                           { FIBNO = Value; }
	void SetEUCD( const char* Value )                                           { FEUCD = Value; }

	const char*                                GetMaturityMonthYear( void )     { return FMaturityMonthYear.c_str(); }
	const char*                                GetOrigClOrdID( void )           { return FOrigClOrdID.c_str(); }
	nsOrderMessageDefine::OrderTypeEnum        GetOrderType( void )             { return FOrderType; }
	nsOrderMessageDefine::SideEnum             GetSide( void )                  { return FSide; }
	nsOrderMessageDefine::TimeInForceEnum      GetTimeInForce( void )           { return FTimeInForce; }
	nsOrderMessageDefine::TradingSessionIDEnum GetTradingSessionID( void );
	nsOrderMessageDefine::OrderSourceEnum      GetOrderSource( void )           { return FOrderSource; }
	const char*                                GetMarketDataSource( void )      { return FMarketDataSource.c_str(); }
	UFCType::Int64                             GetCNID( void )                  { return FCNID; }
	double                                     GetPrice( void )                 { return FPrice; }
	long                                       GetOrderQty( void )              { return FOrderQty; }
	const char*                                GetTSEOrderType( void )          { return FTSEOrderType.c_str(); }
	const char*                                GetTSEExchangeCode( void )       { return FTSEExchangeCode.c_str(); }
	const char*                                GetExchangeCode( void )          { return FExchangeCode.c_str(); }
	const char*                                GetTSETradeKind( void )          { return FTSETradeKind.c_str(); }
	const char*                                GetTSEObjectBroker( void )       { return FTSEObjectBroker.c_str(); }
	long                                       GetStockSeqNo( void )            { return FStockSeqNo; }
	const char*                                GetKeep( void )                  { return FKeep.c_str(); }
	const char*                                GetSrc( void )                   { return FSrc.c_str(); }
	const char*                                GetIBNO( void )                  { return FIBNO.c_str(); }
	const char*                                GetEUCD( void )                  { return FEUCD.c_str(); }
};
//--------------------------------------------------------------------------
#endif

