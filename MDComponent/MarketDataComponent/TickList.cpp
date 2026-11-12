//---------------------------------------------------------------------------
#pragma hdrstop
#include "TickList.h"
#include "math.h"
#include <algorithm>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
//	class TTickRules
//
//---------------------------------------------------------------------------
static const char* const TICK_RULE_GROUPS[] =
{
	"TickTAIFEXFutures",
	"TickTAIFEXOptions",
	"TickExchange"
};
//---------------------------------------------------------------------------
TTickRules::TTickRules( void )
{
	LoadDefaults( );
}
//---------------------------------------------------------------------------
int TTickRules::GroupCount( void )
{
	return sizeof( TICK_RULE_GROUPS ) / sizeof( TICK_RULE_GROUPS[0] );
}
//---------------------------------------------------------------------------
const char* TTickRules::GroupName( int Index )
{
	if( Index < 0 || Index >= GroupCount() )
		return "";
	return TICK_RULE_GROUPS[ Index ];
}
//---------------------------------------------------------------------------
std::string TTickRules::MakeKey( const char* Group, const char* Key )
{
	return std::string( Group ) + "." + Key;
}
//---------------------------------------------------------------------------
static bool TickStepGreater( const TTickStep& A, const TTickStep& B )
{
	return A.Threshold > B.Threshold;
}
//---------------------------------------------------------------------------
/// Parse "Threshold:Tick,Threshold:Tick,...,0:Tick" or a single fixed "Tick".
bool TTickRules::ParseLadder( const char* LadderStr, TTickLadder& Ladder )
{
	std::string Src( LadderStr == NULL ? "" : LadderStr );
	size_t      Pos = 0;

	Ladder.clear();
	while( Pos < Src.size() )
	{
		size_t Comma = Src.find( ',', Pos );
		if( Comma == std::string::npos )
			Comma = Src.size();
		std::string Item = Src.substr( Pos, Comma - Pos );
		Pos = Comma + 1;
		if( Item.find_first_not_of( " \t" ) == std::string::npos )
			continue;

		double    Threshold = 0, Tick = 0;
		TTickStep Step;

		if( Item.find( ':' ) == std::string::npos ) ///< Fixed tick.
		{
			if( sscanf( Item.c_str(), "%lf", &Tick ) != 1 )
				return false;
		}
		else if( sscanf( Item.c_str(), "%lf : %lf", &Threshold, &Tick ) != 2 )
			return false;
		Step.Threshold = UFC::DoubleToInt( Threshold, Digit );
		Step.Tick      = UFC::DoubleToInt( Tick, Digit );
		if( Step.Tick <= 0 || Step.Threshold < 0 )
			return false;
		Ladder.push_back( Step );
	}
	if( Ladder.empty() )
		return false;
	std::sort( Ladder.begin(), Ladder.end(), TickStepGreater );
	return Ladder.back().Threshold == 0; ///< Must have the default (0:Tick) step.
}
//---------------------------------------------------------------------------
int TTickRules::StepOf( const TTickLadder& Ladder, int ScaledPx )
{
	for( size_t i = 0; i < Ladder.size(); i++ )
	{
		if( ScaledPx > Ladder[i].Threshold )
			return Ladder[i].Tick;
	}
	return Ladder.empty() ? 0 : Ladder.back().Tick;
}
//---------------------------------------------------------------------------
/// Built-in rules, used when FMTMD.ini has no tick setting.
void TTickRules::LoadDefaults( void )
{
	FLadders.clear();
	///< TAIFEX futures, fixed tick.
	SetLadder( "TickTAIFEXFutures", "TX", "1" );
	SetLadder( "TickTAIFEXFutures", "MX", "1" );
	SetLadder( "TickTAIFEXFutures", "T5", "1" );
	SetLadder( "TickTAIFEXFutures", "TM", "1" );
	SetLadder( "TickTAIFEXFutures", "XI", "1" );
	SetLadder( "TickTAIFEXFutures", "UN", "1" );
	SetLadder( "TickTAIFEXFutures", "G2", "1" );
	SetLadder( "TickTAIFEXFutures", "UD", "1" );
	SetLadder( "TickTAIFEXFutures", "EX", "0.05" );
	SetLadder( "TickTAIFEXFutures", "GT", "0.05" );
	SetLadder( "TickTAIFEXFutures", "SP", "0.25" );
	SetLadder( "TickTAIFEXFutures", "FX", "0.2" );
	SetLadder( "TickTAIFEXFutures", "GB", "0.005" );
	SetLadder( "TickTAIFEXFutures", "CP", "0.005" );
	SetLadder( "TickTAIFEXFutures", "MS", "0.1" );
	SetLadder( "TickTAIFEXFutures", "GD", "0.1" );
	SetLadder( "TickTAIFEXFutures", "TG", "0.5" );
	///< TAIFEX ETF futures.
	SetLadder( "TickTAIFEXFutures", "NZ", "50:0.05,0:0.01" );
	SetLadder( "TickTAIFEXFutures", "NY", "50:0.05,0:0.01" );
	SetLadder( "TickTAIFEXFutures", "OA", "50:0.05,0:0.01" );
	///< TAIFEX stock futures.
	SetLadder( "TickTAIFEXFutures", "Default", "2500:5,500:1,100:0.5,50:0.1,10:0.05,0:0.01" );
	///< TAIFEX options.
	SetLadder( "TickTAIFEXOptions", "TX", "1000:10,500:5,50:1,10:0.5,0:0.1" );
	SetLadder( "TickTAIFEXOptions", "TE", "50:0.5,25:0.25,2.5:0.05,0.5:0.025,0:0.005" );
	SetLadder( "TickTAIFEXOptions", "MS", "50:0.5,25:0.25,2.5:0.05,0.5:0.025,0:0.005" );
	SetLadder( "TickTAIFEXOptions", "GT", "50:0.5,25:0.25,2.5:0.05,0.5:0.025,0:0.005" );
	SetLadder( "TickTAIFEXOptions", "TF", "200:2,100:1,10:0.2,2:0.1,0:0.02" );
	SetLadder( "TickTAIFEXOptions", "XI", "2000:20,1000:10,100:2,20:1,0:0.2" );
	SetLadder( "TickTAIFEXOptions", "TG", "0.5" );
	///< TAIFEX stock options.
	SetLadder( "TickTAIFEXOptions", "Default", "1000:5,150:1,50:0.5,15:0.1,5:0.05,0:0.01" );
	///< Stock exchanges.
	SetLadder( "TickExchange", "TWSE", "1000:5,500:1,100:0.5,50:0.1,10:0.05,0:0.01" );
	SetLadder( "TickExchange", "OTC",  "1000:5,500:1,100:0.5,50:0.1,10:0.05,0:0.01" );
	SetLadder( "TickExchange", "KRX",  "3:0.05,0:0.01" );
}
//---------------------------------------------------------------------------
bool TTickRules::SetLadder( const char* Group, const char* Key, const char* LadderStr )
{
	TTickLadder Ladder;

	if( ParseLadder( LadderStr, Ladder ) == false )
		return false;
	FLadders[ MakeKey( Group, Key ) ] = Ladder;
	return true;
}
//---------------------------------------------------------------------------
const TTickLadder* TTickRules::Find( const char* Group, const char* Key, bool UseDefault ) const
{
	std::map<std::string, TTickLadder>::const_iterator It = FLadders.find( MakeKey( Group, Key ) );

	if( It == FLadders.end() && UseDefault == true )
		It = FLadders.find( MakeKey( Group, "Default" ) );
	if( It == FLadders.end() )
		return NULL;
	return &It->second;
}
//---------------------------------------------------------------------------
//
//	class TTickList
//
//---------------------------------------------------------------------------
TTickList::TTickList( const TTickRules* Rules )
:FExchange( "" )
,FSymbol( "" )
,FContractMultiplier(0)
,FBullPx( 0 )
,FBearPx( 0 )
,FRules( Rules )
{
}
//---------------------------------------------------------------------------
TTickList::TTickList( BasicInformation* Info, const TTickRules* Rules )
:FInfo( Info )
,FExchange( Info->GetExchange() )
,FSymbol( Info->GetSymbol() )
,FBullPx( Info->GetBullPrice() )
,FBearPx( Info->GetBearPrice() )
,FRules( Rules )
{
	InitTicks( );
	FContractMultiplier = Info->GetContractMultiplier();
}
//---------------------------------------------------------------------------
TTickList::TTickList( BasicInformation* Info, double MaxPx, double MinPx, const TTickRules* Rules )
:FInfo( Info )
,FExchange( Info->GetExchange() )
,FSymbol( Info->GetSymbol() )
,FBullPx( Info->GetBullPrice() )
,FBearPx( Info->GetBearPrice() )
,FRules( Rules )
{
	if( MaxPx > FBullPx )
		FBullPx = MaxPx;

	if( MinPx < FBearPx )
		FBearPx = MinPx;

	InitTicks( );
	FContractMultiplier = Info->GetContractMultiplier();
}
//---------------------------------------------------------------------------
void TTickList::InitTicks( )
{
	if( FExchange == "CBOT" )
		FTNFormats = tnCBOT;
	else if( FExchange == "TWSE" ||  FExchange == "OTC")
		FTNFormats = tnTWSE_OTC;
	else
		FTNFormats = tnGeneric;
	if( FInfo->GetDecimalLocator() == 0 )
		FPxDigit = 3;
	else
		FPxDigit = FInfo->GetDecimalLocator();
	GenerateTicks( FBullPx, FBearPx );
	for( register int i = 0; i < (int)FTickArray.size(); i++ )
	{
		 int IntTickPx = UFC::DoubleToInt( FTickArray[ i ], FPxDigit );
		 FPxToTickIndexMap[ IntTickPx ] = i;
	}
}
//---------------------------------------------------------------------------
const TTickRules& TTickList::GetRules( void ) const
{
	static TTickRules DefaultRules;

	return ( FRules != NULL ) ? *FRules : DefaultRules;
}
//---------------------------------------------------------------------------
void TTickList::GenerateByLadder( const TTickLadder& Ladder, double BullPx, double BearPx )
{
	int TickPx = UFC::DoubleToInt( BullPx, TTickRules::Digit );
	int EndPx  = UFC::DoubleToInt( BearPx, TTickRules::Digit );

	while( TickPx >= EndPx )
	{
		FTickArray.push_back( UFC::IntToDouble( TickPx, TTickRules::Digit ) );
		int Step = TTickRules::StepOf( Ladder, TickPx );
		if( Step <= 0 )
			break;
		TickPx -= Step;
	}
}
//---------------------------------------------------------------------------
void TTickList::GenerateTicks( double BullPx, double BearPx )
{
	const TTickRules&  Rules  = GetRules();
	const TTickLadder* Ladder = NULL;

	if( FExchange == "TAIFEX" || FExchange == "TFX" )
	{
		UFC::AnsiString Prefix( FSymbol.SubString( 0, 2 ) );

		if( FSymbol.Length() == 5 || FSymbol.Length() == 8 ) /// Futures
			Ladder = Rules.Find( "TickTAIFEXFutures", Prefix.c_str(), true );
		else ///< Options
		{
			if( Prefix == "TX" && BullPx < 300 )
			{
				BullPx = 300;
				BearPx = 0.1;
			}
			Ladder = Rules.Find( "TickTAIFEXOptions", Prefix.c_str(), true );
		}
		if( Ladder != NULL )
			GenerateByLadder( *Ladder, BullPx, BearPx );
	}
	else if( ( Ladder = Rules.Find( "TickExchange", FExchange.c_str(), false ) ) != NULL )
	{
		GenerateByLadder( *Ladder, BullPx, BearPx );
	}
	else if( FExchange == "CFFEX" ||  FExchange == "DCE" ||  FExchange == "CZCE" ||  FExchange == "SHFE"  ||  FExchange == "SSE" )
	{
		int Tick   = UFC::DoubleToInt( FInfo->GetTick(), FInfo->GetDecimalLocator() );
		int TickPx = UFC::DoubleToInt( BullPx, FInfo->GetDecimalLocator() );
		int EndPx  = UFC::DoubleToInt( BearPx , FInfo->GetDecimalLocator() );

		if( Tick > 0 )
		{
			while( TickPx >= EndPx )
			{
				FTickArray.push_back( UFC::IntToDouble( TickPx, FInfo->GetDecimalLocator() ) );
				TickPx -= Tick;
			}
		}
	}
	else
	{
		int RefPx  = UFC::DoubleToInt( FInfo->GetRefPrice(), FInfo->GetDecimalLocator() );
		int Tick   = UFC::DoubleToInt( FInfo->GetTick(), FInfo->GetDecimalLocator() );
		int TickPx = UFC::DoubleToInt( BullPx, FInfo->GetDecimalLocator() );
		int EndPx  = UFC::DoubleToInt( BearPx , FInfo->GetDecimalLocator() );

		if( Tick > 0 )
		{
			if( TickPx > RefPx*1.5 || EndPx < RefPx*0.5 )
			{
				TickPx  = RefPx*1.5;
				EndPx   = RefPx*0.5;
				TickPx  = (TickPx/Tick)*Tick;
				EndPx   = (EndPx/Tick)*Tick;
			}
			while( TickPx >= EndPx )
			{
				FTickArray.push_back( UFC::IntToDouble( TickPx, FInfo->GetDecimalLocator() ) );
				TickPx -= Tick;
			}
		}
	}
}
//---------------------------------------------------------------------------
int TTickList::PxToTickIndex( double Price, bool WithException )
{
	int                              IntTickPx =  UFC::DoubleToInt( Price, FPxDigit );
	std::hash_map<int,int>::iterator Iterator  = FPxToTickIndexMap.find( IntTickPx );

	if( Iterator == FPxToTickIndexMap.end() )///< Not found
	{
		if( WithException == true )
		{
			String Msg;
			Msg.printf( L"Price %5.4f not in tick price list", Price );
			throw( Exception( Msg.c_str() ) );
		}
		else
			return FindNearTick( Price );
	}
	else
		return Iterator->second;
}
//--------------------------------------------------------------------------
int TTickList::FindNearTick( double Px )
{
	register int First = 0;
	register int Last  = (int)FTickArray.size()-1;
	register int Half  = (Last - First)>>1;

	if( Px  >= FTickArray[ First ] )///< Larger than the bull Px.
		return First;               ///< Use bull price
	if( Px  <= FTickArray[ Last ] ) ///< Smaller than the Bear Px.
		return Last;                ///< Use bear price
	for(;;) ///< Binary search.
	{
		if( Px  < FTickArray[Half] )
			First = Half;
		else if( Px  > FTickArray[Half] )
			Last  = Half;
		else                 ///< Bingo! Same as the middle object.
			return Half;
		Half  = First + ((Last - First)>>1);
		if( First == Half || Last == Half )
			return Last;
	}
}
//--------------------------------------------------------------------------
int TTickList::FindTick( double Px )
{
	int IntTickPx =  UFC::DoubleToInt( Px, FPxDigit );

	std::hash_map<int,int>::iterator Iterator = FPxToTickIndexMap.find( IntTickPx );
	if( Iterator == FPxToTickIndexMap.end() )///< Not found
		return FindNearTick( Px );
	else
		return Iterator->second;
}
//--------------------------------------------------------------------------
double TTickList::GetNetPositionProfit( double FillPx, DynamicArray<TTradePosition>& BuyPos,
							DynamicArray<TTradePosition>& SellPos )
{
	try
	{
		//int CumQty = 0;
		double Profit = 0;

		for( register int i = 0; i < BuyPos.Length; i++ )
		{
			Profit += ( FillPx - BuyPos[i].GetPx() ) * BuyPos[i].GetQty();
			//CumQty += BuyPos[i].GetQty();
		}
		for( register int i = 0; i < SellPos.Length; i++ )
		{
			Profit += ( SellPos[i].GetPx() - FillPx ) * SellPos[i].GetQty();
			//CumQty += SellPos[i].GetQty();
		}
		Profit *= FContractMultiplier;

		return Profit;
	}
	catch( Exception& )
	{
		return 0;
	}
}
//---------------------------------------------------------------------------
double TTickList::GetPxFromIndex( int Index, bool WithException )
{
	if( WithException == false )
	{
		int IndexMax = (int)FTickArray.size()-1;
		if( Index >= IndexMax )
			return FTickArray[ IndexMax ];
		else if( Index < 0 )
			return FTickArray[ 0 ];
		else
			return FTickArray[ Index ];
	}
	else
	{
		if( Index < (int)FTickArray.size() && Index >= 0 )
			return FTickArray[Index];
		else
		{
			String Msg;
			Msg.printf( L"Index %d not in tick price list", Index );
			throw( Exception( Msg.c_str() ) );
		}
	}
}
//---------------------------------------------------------------------------
double TTickList::GetTickStepFromIndex( int Index )
{
	int IndexMax = (int)FTickArray.size()-1;

	if( Index >= IndexMax )
		return FTickArray[ IndexMax - 1 ] - FTickArray[ IndexMax ] ;
	else if( Index <= 0 )
		return FTickArray[ 0 ] - FTickArray[ 1 ];
	else
		return FTickArray[ Index ] - FTickArray[ Index + 1 ];
}
//---------------------------------------------------------------------------
double TTickList::GetPxFromIndex( double DoubleIndex )
{
	int     IndexMax = (int)FTickArray.size()-1;
	int     IntPart   = (int)DoubleIndex;
	double  FloatPart = 1.0f - (DoubleIndex -((double)IntPart));

	if( IntPart >= IndexMax )
		return FTickArray[ IndexMax ];
	else if( IntPart < 0 )
		return FTickArray[ 0 ];
	else
	{
		double  BasePx    = GetPxFromIndex( IntPart, false );
		double  UpperPx   = GetPxFromIndex( IntPart - 1, false );
		return  BasePx + ((UpperPx - BasePx)*FloatPart);
	}
}
//---------------------------------------------------------------------------
void TTickList::SaveToStream( UFC::PStream* Stream )
{
	FExchange.SaveToStream( Stream );
	FSymbol.SaveToStream( Stream );
	Stream->Write( &FBullPx, sizeof( double ));
	Stream->Write( &FBearPx, sizeof( double ));
	Stream->Write( &FContractMultiplier, sizeof( double ));
}
//---------------------------------------------------------------------------
void TTickList::LoadFromStream( UFC::PStream* Stream )
{
	FTickArray.clear();
	FPxToTickIndexMap.clear();
	FExchange.LoadFromStream( Stream );
	FSymbol.LoadFromStream( Stream );
	Stream->Read( &FBullPx, sizeof( double ));
	Stream->Read( &FBearPx, sizeof( double ));
	Stream->Read( &FContractMultiplier, sizeof( double ) );
	InitTicks( );
}
//---------------------------------------------------------------------------
void TTickList::ToTNFormat( double Value, String& TextValue ) const
{
	if( FTNFormats == tnCBOT )
	{
		double Fraction, IntPart;

		Fraction = modf( Value, &IntPart );
		int TmpInt = UFC::DoubleToInt( Fraction, FInfo->GetDigit() );
		Fraction = UFC::IntToDouble( TmpInt, FInfo->GetDigit() );
		if( FInfo->GetProductID() == "ZB" )  // 1/32
			TextValue.printf( L"%.0f'%02.0f", IntPart, abs( Fraction * 32 ) );
		else if( FInfo->GetProductID() == "ZN" )
			TextValue.printf( L"%.0f'%02.1f", IntPart, abs( Fraction * 32 ) );
		else if( FInfo->GetProductID() == "ZF" )
			TextValue.printf( L"%.0f'%02.2f", IntPart, abs( Fraction * 32 ) );
		else if( FInfo->GetProductID() == "ZT" )
			TextValue.printf( L"%.0f'%02.2f", IntPart, abs( Fraction * 32 ) );
		else
			TextValue.printf( L"%.*f", FInfo->GetDigit(), Value );
	}
	else if( FTNFormats == tnTWSE_OTC  )
	{
		int Digi;

		if( Value >= 500 )      ///< 1~5
			Digi = 0;
		else if( Value >= 50 ) ///< 0.1~0.5
			Digi = 1;
		else
			Digi = 2;           ///< 0.01 ~0.05
		TextValue.printf( L"%.*f", Digi, Value );
	}
	else
		TextValue.printf( L"%.*f", FInfo->GetDigit(), Value );
}
//---------------------------------------------------------------------------



