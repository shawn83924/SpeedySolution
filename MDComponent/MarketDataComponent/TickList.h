//---------------------------------------------------------------------------
#ifndef TickListH
#define TickListH
//---------------------------------------------------------------------------
#include "UFC.h"
#include "OrderMessageDefine.h"
#include "MarketDataConnection.h"
#include <Classes.hpp>
#include <map>
#include <string>
#include <vector>
//---------------------------------------------------------------------------
/// One step of a tick ladder: when Price > Threshold, the tick size is Tick.
/// Both values are scaled integers (x 10^TTickRules::Digit).
struct TTickStep
{
	int Threshold;
	int Tick;
};
typedef std::vector<TTickStep> TTickLadder; ///< Sorted by Threshold desc, last Threshold is 0.
//---------------------------------------------------------------------------
/// Tick size rules used by TTickList::GenerateTicks().
/// Group names are also the section names in FMTMD.ini:
///   [TickTAIFEXFutures]  Key = symbol prefix(2 chars) or Default (stock futures)
///   [TickTAIFEXOptions]  Key = symbol prefix(2 chars) or Default (stock options)
///   [TickExchange]       Key = exchange name (TWSE, OTC, KRX ...)
/// Ladder string format: "Threshold:Tick,Threshold:Tick,...,0:Tick" or a single fixed "Tick".
class TTickRules
{
private:
	std::map<std::string, TTickLadder> FLadders;
private:
	static std::string MakeKey( const char* Group, const char* Key );
public:
	static const int Digit = 3; ///< Scale digits of TTickStep values.
	static int         GroupCount( void );
	static const char* GroupName( int Index );
	static bool        ParseLadder( const char* LadderStr, TTickLadder& Ladder );
	static int         StepOf( const TTickLadder& Ladder, int ScaledPx );
public:
	TTickRules( void );
	void               LoadDefaults( void );
	bool               SetLadder( const char* Group, const char* Key, const char* LadderStr );
	const TTickLadder* Find( const char* Group, const char* Key, bool UseDefault ) const;
};
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
	const TTickRules*       FRules;            ///< Tick rules (owned by caller), NULL = built-in defaults.
private:
	const TTickRules& GetRules( void ) const;
	void GenerateTicks( double BullPx, double BearPx );
	void GenerateByLadder( const TTickLadder& Ladder, double BullPx, double BearPx );
	void InitTicks( void );
	int  FindNearTick( double Px );
public:
	/// Rules : tick rules used to generate ticks, NULL means using built-in defaults.
	///         The rules object must outlive this tick list.
	TTickList( BasicInformation* Info, const TTickRules* Rules = NULL );
	TTickList( const TTickRules* Rules = NULL );
	TTickList( BasicInformation* Info, double MaxPx, double MinPx, const TTickRules* Rules = NULL );
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
