//---------------------------------------------------------------------------

#ifndef TMarketDataMessageH
#define TMarketDataMessageH
//---------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TMDBaseMessage.h"
//--------------------------------------------------------------------------
class TMarketDataMessage :
	public TMDBaseMessage
{
public:
	TMarketDataMessage(void);
public:
	~TMarketDataMessage( void );
public:
	const char* GetSymbol( void );
	void 		SetSymbol( const char* Symbol );
	const char* GetSecurityID( void );
	void 		SetSecurityID( const char* SecurityID );
public:

	double	GetBidPrice( int Depth );
	int     GetBidSize( int Depth );
	double	GetOfferPrice( int Depth );
	int     GetOfferSize( int Depth );
	double	GetTradePrice( void );
	double	GetTradeSize( void );
	double	GetOpeningPrice( void );
	double	GetClosingPrice( void );
	double	GetSettlementPrice( void );
	double	GetTradingSessionHighPrice( void );
	double	GetTradingSessionLowPrice( void );
	int		GetTradeVolume( void );
	int		GetOpenInterest( void );
	int		GetMarketDepth( void );
	int		GetMaturityDate( void );
	double	GetRefPrice( void );
	double	GetRiseLimitPrice( void );
	double	GetFallLimitPrice( void );
	int		GetStrikePrice( void );

	void	SetMarketDepth( int Depth );
	void	SetBidPrice( int Depth, double Price );
	void    SetBidSize( int Depth, int Size );
	void	SetOfferPrice( int Depth, double Price );
	void	SetOfferSize( int Depth, int Size );
	void	SetTradePrice( double Price );
	void	SetTradeSize( int Size );
	void	SetOpeningPrice( double Price );
	void	SetClosingPrice( double Price );
	void	SetSettlementPrice( double Price );
	void	SetTradingSessionHighPrice( double Price );
	void	SetTradingSessionLowPrice( double Price );
	void	SetTradeVolume( int Size );
	void	SetOpenInterest( int Size );
	void	SetMaturityDate( int Date );
	void	SetRefPrice( double Price );
	void	SetRiseLimitPrice( double Price );
	void	SetFallLimitPrice( double Price );
	void	SetStrikePrice( int Price );
};
//--------------------------------------------------------------------------
#endif
