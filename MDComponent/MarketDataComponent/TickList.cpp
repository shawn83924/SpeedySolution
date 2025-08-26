//---------------------------------------------------------------------------
#pragma hdrstop
#include "TickList.h"
#include "math.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
//
//	class TTickList
//
//---------------------------------------------------------------------------
TTickList::TTickList( void  )
:FExchange( "" )
,FSymbol( "" )
,FContractMultiplier(0)
,FBullPx( 0 )
,FBearPx( 0 )
{
}
//---------------------------------------------------------------------------
TTickList::TTickList( BasicInformation* Info )
:FInfo( Info )
,FExchange( Info->GetExchange() )
,FSymbol( Info->GetSymbol() )
,FBullPx( Info->GetBullPrice() )
,FBearPx( Info->GetBearPrice() )
{
	InitTicks( );
	FContractMultiplier = Info->GetContractMultiplier();
}
//---------------------------------------------------------------------------
TTickList::TTickList( BasicInformation* Info, double MaxPx, double MinPx )
:FInfo( Info )
,FExchange( Info->GetExchange() )
,FSymbol( Info->GetSymbol() )
,FBullPx( Info->GetBullPrice() )
,FBearPx( Info->GetBearPrice() )
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
void TTickList::GenerateTicks( double BullPx, double BearPx )
{
	if( FExchange == "TAIFEX" || FExchange == "TFX" )
	{
		UFC::AnsiString Prefix( FSymbol.SubString( 0, 2 ) );
		//UFC::BufferedLog::Printf( " FSymbol = %s", FSymbol.c_str() );
		int Tick = -1;
		if( FSymbol.Length() == 5 || FSymbol.Length() == 8 ) /// Futures, Fix tick.
		{
			int TickPx = UFC::DoubleToInt( BullPx, 3 );
			int EndPx = UFC::DoubleToInt( BearPx , 3 );
			if( Prefix == "TX" || Prefix == "MX" || Prefix == "T5" || Prefix == "TM"|| Prefix == "XI" ||
				Prefix == "UN" || Prefix == "G2" || Prefix == "UD" )
				Tick = 1000;  ///< 1.0
			else if( Prefix == "EX" || Prefix == "GT" )
				Tick = 50;    ///< 0.05
			else if( Prefix == "SP" )
				Tick = 250;  ///< 0.25
			else if( Prefix == "FX" )
				Tick = 200;
			else if( Prefix == "GB" || Prefix == "CP" )
				Tick = 5;
			else if( Prefix == "MS" || Prefix == "GD" )
				Tick = 100;
			else if( Prefix == "TG" )
				Tick = 500;

			if( Tick > 0 )
			{
				while( TickPx >= EndPx )
				{
					FTickArray.push_back( UFC::IntToDouble( TickPx, 3 ) );
					TickPx -= Tick;
				}
			}
			else ///< Stock Futures
			{
				 ///<  ETF Futures
				if( Prefix == "NZ" || Prefix == "NY" || Prefix == "OA" )
				{
					//UFC::BufferedLog::Printf( " ETF Futures = %s", Prefix.c_str( ) );
					while( TickPx >= EndPx )
					{
						FTickArray.push_back( UFC::IntToDouble( TickPx, 3 ) );
						//UFC::BufferedLog::Printf( " ETF TickPx = %d", TickPx );
						if( TickPx > 50000 )
							TickPx -= 50;
						else
							TickPx -= 10;
					}
				}
				else
				{
					//UFC::BufferedLog::Printf( " Stock Futures = %s", Prefix.c_str( ) );
					while( TickPx >= EndPx )
					{
						FTickArray.push_back( UFC::IntToDouble( TickPx, 3 ) );
						if( TickPx > 1000000 )
							TickPx -= 5000;
						else if( TickPx > 500000 )
							TickPx -= 1000;
						else if( TickPx > 100000 )
							TickPx -= 500;
						else if( TickPx > 50000 )
							TickPx -= 100;
						else if( TickPx > 10000 )
							TickPx -= 50;
						else
							TickPx -= 10;
					}
				}
			}
		}
		else ///< Options
		{
			double TickPx = BullPx;

			if( Prefix == "TX")
			{
				if( TickPx < 300 )
				{
					TickPx = 300;
					BearPx = 0.1;
				}
				while( TickPx >= BearPx )
				{
					FTickArray.push_back( TickPx );
					if( TickPx > 1000 )
						TickPx -= 10;
					else if( TickPx > 500 )
						TickPx -= 5;
					else if( TickPx > 50 )
						TickPx -= 1;
					else if( TickPx > 10 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 50, 2 );
					}
					else
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 10, 2 );
					}
				}
			}
			else if( Prefix == "TE" || Prefix == "MS" || Prefix == "GT" )
			{
				while( TickPx >= BearPx )
				{
					FTickArray.push_back( TickPx );
					if( TickPx > 50 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 50, 2 );
					}
					else if( TickPx > 25 )
					{
						int TmpValue = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 25, 2 );
					}
					else if( TickPx > 2.5 )
					{
						int TmpValue = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 5, 2 );

					}
					else if( TickPx > 0.5 )
					{
						int TmpValue = UFC::DoubleToInt( TickPx, 3 );
						TickPx = UFC::IntToDouble( TmpValue - 25, 3 );
					}
					else
					{
						int TmpValue = UFC::DoubleToInt( TickPx, 3 );
						TickPx = UFC::IntToDouble( TmpValue - 5, 3 );
					}
				}
			}
			else if( Prefix == "TF" )
			{
				while( TickPx >= BearPx )
				{
					FTickArray.push_back( TickPx );
					if( TickPx > 200 )
						TickPx -= 2;
					else if( TickPx > 100 )
						TickPx -= 1;
					else if( TickPx > 10 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 20, 2 );
					}
					else if( TickPx > 2 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 10, 2 );
					}
					else
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 2, 2 );
					}
				}
			}
			else if( Prefix == "XI" )
			{
				while( TickPx >= BearPx )
				{
					FTickArray.push_back( TickPx );
					if( TickPx > 2000 )
						TickPx -= 20;
					else if( TickPx > 1000 )
						TickPx -= 10;
					else if( TickPx > 100 )
						TickPx -= 2;
					else if( TickPx > 20 )
						TickPx -= 1;
					else
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 20, 2 );
					}
				}
			}
			else if( Prefix == "TG" )
			{
				while( TickPx >= BearPx )
				{
					FTickArray.push_back( TickPx );
					TickPx -= 0.5;
				}
			}
			else ///< Stock Options
			{
				while( TickPx >= BearPx )
				{
					FTickArray.push_back( TickPx );
					if( TickPx > 1000 )
						TickPx -= 5;
					else if( TickPx > 150 )
						TickPx -= 1;
					else if( TickPx > 50 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 50, 2 );
					}
					else if( TickPx > 15 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 10, 2 );
					}
					else if( TickPx > 5 )
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 5, 2 );
					}
					else
					{
						int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
						TickPx = UFC::IntToDouble( TmpValue - 1, 2 );
					}
				}
			}
		}
	}
	else if( FExchange == "TWSE" ||  FExchange == "OTC")
	{
		double TickPx = BullPx;

		while( TickPx >= BearPx )
		{
			FTickArray.push_back( TickPx );
			if( TickPx > 1000 ) /// 1000 and above       Tick 5.00
				TickPx -= 5;
			else if( TickPx > 500 ) /// 500 <= Px < 1000 Tick 1.00
				TickPx -= 1;
			else if( TickPx > 100 ) /// 100 <= Px < 500  Tick 0.50
			{
				int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
				TickPx = UFC::IntToDouble( TmpValue - 50, 2 );
			}
			else if( TickPx > 50 ) /// 50 <= Px < 100    Tick 0.10
			{
				int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
				TickPx = UFC::IntToDouble( TmpValue - 10, 2 );
			}
			else if( TickPx > 10 ) /// 10 <= Px < 50     Tick 0.05
			{
				int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
				TickPx = UFC::IntToDouble( TmpValue - 5, 2 );
			}
			else                   /// 0.01 <= Px < 10   Tick 0.01
			{
				int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
				TickPx = UFC::IntToDouble( TmpValue - 1, 2 );
			}
		}
	}
	else if( FExchange == "KRX" )
	{
		double TickPx = BullPx;

		while( TickPx >= BearPx )
		{
			FTickArray.push_back( TickPx );

			if( TickPx > 3 ) /// 3 and above       Tick 0.05
			{
            	int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
				TickPx = UFC::IntToDouble( TmpValue - 5, 2 );
			}
			else
			{
				int TmpValue  = UFC::DoubleToInt( TickPx, 2 );
				TickPx = UFC::IntToDouble( TmpValue - 1, 2 );
			}
		}
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



