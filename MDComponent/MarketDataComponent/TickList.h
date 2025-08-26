//---------------------------------------------------------------------------
#ifndef TickListH
#define TickListH
//---------------------------------------------------------------------------
#include "UFC.h"
#include "OrderMessageDefine.h"
#include "MarketDataConnection.h"
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TTradePosition
{
private:
	double FPx;
	int FQty;
public:
	TTradePosition() {}
	TTradePosition( double Px, int Qty ):FPx( 0 ), FQty( 0 ){}
	void SetPx( double Px ) { FPx = Px; }
	void SetQty( int Qty ) { FQty = Qty; }
	double GetPx( void ) { return FPx; }
	int GetQty( void ){ return FQty; }
};
//---------------------------------------------------------------------------
typedef enum
{
	tnCBOT     = 0,
	tnTWSE_OTC = 1,
	tnGeneric  = 2

} TTNFormats;
//---------------------------------------------------------------------------
class TTickList
{
private:
	BasicInformation*		FInfo;
	UFC::AnsiString 		FExchange;
	UFC::AnsiString 		FSymbol;
	double					FContractMultiplier;
	double 					FBullPx;
	double 					FBearPx;
	int						FPxDigit;
	std::vector<double>    	FTickArray;        ///< Ticks array.(keep all tick price)
	std::hash_map<int,int> 	FPxToTickIndexMap; ///< price to tick index map.(map price to tick index)
	TTNFormats              FTNFormats;
private:
	void GenerateTicks( double BullPx, double BearPx );
	void InitTicks( void );
	int  FindNearTick( double Px );
public:
	TTickList( BasicInformation* Info );
	TTickList( void );
	TTickList( BasicInformation* Info, double MaxPx, double MinPx );
public:
	const UFC::AnsiString& GetSymbol( void )   { return FSymbol; }
	const UFC::AnsiString& GetExchange( void ) { return FExchange; }
	double GetBullPx( void ) { return FBullPx; }
	double GetBearPx( void ) { return FBearPx; }
	double GetPxFromIndex( int Index, bool WithException = true );
	double GetPxFromIndex( double Index );
	double GetTickStepFromIndex( int Index );
	double GetNetPositionProfit( double FillPx, DynamicArray<TTradePosition>& BuyPos, DynamicArray<TTradePosition>& SellPos );
	double GetContractMultiplier( void ) { return FContractMultiplier; }
	int    ItemCount( void )             { return FTickArray.size(); }
	int    PxToTickIndex( double Px, bool WithException = true );
	int    FindTick( double Px );
	void   SaveToStream( UFC::PStream* );
	void   LoadFromStream( UFC::PStream* );
	void   ToTNFormat( double Value, String& TextValue ) const;
};
//---------------------------------------------------------------------------
#endif
