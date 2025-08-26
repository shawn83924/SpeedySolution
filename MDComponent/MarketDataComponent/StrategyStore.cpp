//---------------------------------------------------------------------------

#include <vcl.h>

#pragma hdrstop

#include "StrategyStore.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
StrategySymbolIlligalException::StrategySymbolIlligalException( const UFC::AnsiString& ErrMsg, const UFC::AnsiString& Leg1, const UFC::AnsiString& Leg2 )
:UFC::Exception( ErrMsg.c_str() )
,FLeg1Symbol( Leg1 )
,FLeg2Symbol( Leg2 )
{}
//---------------------------------------------------------------------------
//
//	Class Strategy Record
//
//---------------------------------------------------------------------------
StrategyRecord::StrategyRecord( BasicInformation* Leg1, BasicInformation* Leg2, StrategyEnum Strategy, SideEnum side )
:FSide( side )
,FStrategy( Strategy )
,FLeg1( Leg1 )
,FLeg2( Leg2 )
,FLeg1Symbol( Leg1->GetSymbol() )
,FLeg2Symbol( Leg2->GetSymbol() )
{
	CheckSymbol();
	CombineSymbol();
	Leg1Px = FLeg1->GetRefPrice();
	Leg1BidPx = FLeg1->GetRefPrice();
	Leg1AskPx = FLeg1->GetRefPrice();
	Leg2Px = FLeg2->GetRefPrice();
	Leg2BidPx = FLeg2->GetRefPrice();
	Leg2AskPx = FLeg2->GetRefPrice();
	switch( FLeg1->GetCallPut() )
	{
		case cpCall: FCallPut1 = nsOrderMessageDefine::evtCall;break;
		case cpPut:  FCallPut1 = nsOrderMessageDefine::evtPut;break;
		case cpNone: FCallPut1 = nsOrderMessageDefine::evtNone;break;
	}
	FStrikePx1 = FLeg1->GetStrikePrice( );
	switch( FLeg2->GetCallPut() )
	{
		case cpCall: FCallPut2 = nsOrderMessageDefine::evtCall;break;
		case cpPut:  FCallPut2 = nsOrderMessageDefine::evtPut;break;
		case cpNone: FCallPut2 = nsOrderMessageDefine::evtNone;break;
	}
	FStrikePx2 = FLeg2->GetStrikePrice( );
}
//---------------------------------------------------------------------------
void StrategyRecord::CheckSymbol( void )
{
	if( FLeg1Symbol.SubString( 0, 3 ) != FLeg2Symbol.SubString( 0, 3 ) )
	{
		StrategySymbolIlligalException ex( "Symbol Error.", FLeg1Symbol, FLeg2Symbol );
		throw ex;
	}
}
//---------------------------------------------------------------------------
void StrategyRecord::CombineSymbol( void )
{
	switch( FStrategy )
	{
		case sPriceSpread:
			if( FLeg1->GetCallPut() != FLeg2->GetCallPut() )
			{
				StrategySymbolIlligalException ex( "價差交易：組合商品買賣權須相同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}

			if( FLeg1->GetStrikePrice() == FLeg2->GetStrikePrice() )
			{
				StrategySymbolIlligalException ex( "價差交易：組合商品履約價須不同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}

			if( FLeg1->GetMaturityDate() != FLeg2->GetMaturityDate() )
			{
				StrategySymbolIlligalException ex( "價差交易：組合商品到期日必須相同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}

			if( FLeg1->GetCallPut() == cpCall )
			{
				if( FLeg1->GetStrikePrice() > FLeg2->GetStrikePrice() )
				{
					FSymbol.Printf( "%s/%s", FLeg1Symbol.SubString( 0, 8 ).c_str(), FLeg2Symbol.SubString( 3, 7 ) );
					if( FSide == sBuy )
					{
						FSide1 = sSell;
						FSide2 = sBuy; 
					}
					else
					{
						FSide1 = sBuy;
						FSide2 = sSell;
					}
				}				
				else
				{
					FSymbol.Printf( "%s/%s", FLeg2Symbol.SubString( 0, 8 ).c_str(), FLeg1Symbol.SubString( 3, 7 ) );
					if( FSide == sBuy )
					{
						FSide1 = sBuy; 
						FSide2 = sSell;
					}
					else
					{
						FSide1 = sSell;
						FSide2 = sBuy;
					}
				}
			}
			else
			{
				 if( FLeg1->GetStrikePrice() < FLeg2->GetStrikePrice() )
				{
					FSymbol.Printf( "%s/%s", FLeg1Symbol.SubString( 0, 8 ).c_str(), FLeg2Symbol.SubString( 3, 7 ) );
					if( FSide == sBuy )
					{
						FSide1 = sSell;
						FSide2 = sBuy;
					}
					else
					{
						FSide1 = sBuy;
						FSide2 = sSell;
					}
				}
				else
				{
					FSymbol.Printf( "%s/%s", FLeg2Symbol.SubString( 0, 8 ).c_str(), FLeg1Symbol.SubString( 3, 7 ) );
					if( FSide == sBuy  )
					{
						FSide1 = sBuy;
						FSide2 = sSell;
					}
					else 
					{
						FSide1 = sSell;
						FSide2 = sBuy;
					}
				}
			}
			break;
		case sStrangle:
			if( FLeg1->GetCallPut() == FLeg2->GetCallPut() )
			{
				StrategySymbolIlligalException ex( "勒式組合：組合商品買賣權須不同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}

			if( FLeg1->GetStrikePrice() == FLeg2->GetStrikePrice() )
			{
				StrategySymbolIlligalException ex( "勒式組合：組合商品履約價不可相同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}
			if( FLeg1->GetCallPut() == cpCall )
				FSymbol.Printf( "%s:%s", FLeg1Symbol.c_str(), FLeg2Symbol.SubString( 3, 7 ) );
			else
				FSymbol.Printf( "%s:%s", FLeg2Symbol.c_str(), FLeg1Symbol.SubString( 3, 7 ) );
			if( FSide == nsOrderMessageDefine::sBuy )
			{
				FSide1 = nsOrderMessageDefine::sBuy;
				FSide2 = nsOrderMessageDefine::sBuy;
			}
			else
			{
				FSide1 = nsOrderMessageDefine::sSell;
				FSide2 = nsOrderMessageDefine::sSell;
			}
			break;
		case sStraddle:
			if( FLeg1->GetCallPut() == FLeg2->GetCallPut() )
			{
				StrategySymbolIlligalException ex( "跨式組合：組合商品買賣權不可相同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}

			if( FLeg1->GetStrikePrice() != FLeg2->GetStrikePrice() )
			{
				StrategySymbolIlligalException ex( "跨式組合：組合商品履約價須相同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}
			if( FLeg1->GetCallPut() == cpCall )
				FSymbol.Printf( "%s:%s", FLeg1Symbol.c_str(), FLeg2Symbol.SubString( 8, 2 ) );
			else
				FSymbol.Printf( "%s:%s", FLeg2Symbol.c_str(), FLeg1Symbol.SubString( 8, 2 ) );
			if( FSide == nsOrderMessageDefine::sBuy )
			{
				FSide1 = nsOrderMessageDefine::sBuy;
				FSide2 = nsOrderMessageDefine::sBuy;
			}
			else
			{
				FSide1 = nsOrderMessageDefine::sSell;
				FSide2 = nsOrderMessageDefine::sSell;
			}
			break;
		case sTimeSpread:
			if( FLeg1->GetCallPut() != FLeg2->GetCallPut() )
			{
				StrategySymbolIlligalException ex( "時間價差：組合商品買賣權須相同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}

			if( FLeg1->GetStrikePrice() != FLeg2->GetStrikePrice() )
			{
				StrategySymbolIlligalException ex( "時間價差：組合商品履約價須相同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}
			if( FLeg1->GetMaturityDate() == FLeg2->GetMaturityDate() )
			{
				StrategySymbolIlligalException ex( "時間價差：組合商品到期日不可相同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}

			if( FLeg1->GetMaturityDate().ToInt() < FLeg2->GetMaturityDate().ToInt() )
			{
				FSymbol.Printf( "%s/%s", FLeg1Symbol.c_str(), FLeg2Symbol.SubString( 8, 2 ) );
				if( FSide == nsOrderMessageDefine::sBuy )
				{
					FSide1 = nsOrderMessageDefine::sSell;
					FSide2 = nsOrderMessageDefine::sBuy;
				}
				else
				{
					FSide1 = nsOrderMessageDefine::sBuy;
					FSide2 = nsOrderMessageDefine::sSell;
				}
			}
			else
			{
				FSymbol.Printf( "%s/%s", FLeg2Symbol.c_str(), FLeg1Symbol.SubString( 8, 2 ) );
				if( FSide == nsOrderMessageDefine::sBuy )
				{
					FSide1 = nsOrderMessageDefine::sBuy;
					FSide2 = nsOrderMessageDefine::sSell;
				}
				else
				{
					FSide1 = nsOrderMessageDefine::sSell;
					FSide2 = nsOrderMessageDefine::sBuy;
				}
			}
			break;
		case sConversion:
		case sReversals:
			if( FLeg1->GetCallPut() == FLeg2->GetCallPut() )
			{
				StrategySymbolIlligalException ex( "組合/逆轉：買賣權不可相同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}

			if( FLeg1->GetStrikePrice() != FLeg2->GetStrikePrice() )
			{
				StrategySymbolIlligalException ex( "組合/逆轉：履約價必須相同", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}

			if( FStrategy == sConversion && FSide == nsOrderMessageDefine::sSell )
			{
				StrategySymbolIlligalException ex( "組合/逆轉：買賣別錯誤", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}

			if( FStrategy == sReversals && FSide == nsOrderMessageDefine::sBuy )
			{
				StrategySymbolIlligalException ex( "組合/逆轉：買賣別錯誤", FLeg1Symbol, FLeg2Symbol );
				throw ex;
			}


			if( FLeg1->GetCallPut() == cpCall )
			{
				FSymbol.Printf( "%s-%s", FLeg1Symbol.c_str(), FLeg2Symbol.SubString( 8, 2 ) );
				if( FSide == nsOrderMessageDefine::sBuy )
				{
					FSide1 = nsOrderMessageDefine::sSell;
					FSide2 = nsOrderMessageDefine::sBuy;
				}
				else
				{
					FSide1 = nsOrderMessageDefine::sBuy;
					FSide2 = nsOrderMessageDefine::sSell;
				}
			}
			else
			{
				FSymbol.Printf( "%s-%s", FLeg2Symbol.c_str(), FLeg1Symbol.SubString( 8, 2 ) );
				if( FSide == nsOrderMessageDefine::sBuy )
				{
					FSide1 = nsOrderMessageDefine::sBuy;
					FSide2 = nsOrderMessageDefine::sSell;
				}
				else
				{
					FSide1 = nsOrderMessageDefine::sSell;
					FSide2 = nsOrderMessageDefine::sBuy;
				}
			}
			break;
	}
}
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
//---------------------------------------------------------------------------
static inline void ValidCtrCheck(TCStrategyStore *)
{
	new TCStrategyStore(NULL);
}
//---------------------------------------------------------------------------
__fastcall TCStrategyStore::TCStrategyStore(TComponent* Owner)
: TComponent(Owner)
,FStore( NULL )
{
}
//---------------------------------------------------------------------------
void __fastcall TCStrategyStore::SetStrategy( String OPTPrefix, String YearMonth, StrategyEnum Strategy, nsOrderMessageDefine::SideEnum Side  )
{
	String TickStr;
	Clear();
	FPrefix = OPTPrefix;
	FYearMonth = YearMonth;
	FStrategy = Strategy;
	UFC::List<BasicInformation*>* InfoList = (UFC::List<BasicInformation*>*)FStore->GetOPTMDSeriesWithYearMonth( "TAIFEX", OPTPrefix, YearMonth );
	int count = InfoList->ItemCount();
	StrategyRecord* NewRec;

	for( register int i = 0; i < count; i++ )
	{
		for( register int j = 0; j < count; j++ )
		{
			try
			{
				NewRec = new StrategyRecord( InfoList->GetItem(i), InfoList->GetItem(j), FStrategy, Side );
				if( FTable.IsExists( NewRec->GetSymbol() ) == FALSE )
					FTable.Add( NewRec->GetSymbol(), NewRec );
				else
					delete NewRec;
			}
			catch( StrategySymbolIlligalException& ex )
			{
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TCStrategyStore::SetMarketDataStore( TCMarketDataStore* Store )
{
	if( FStore != Store)
		FStore = Store;
}
//---------------------------------------------------------------------------
void __fastcall TCStrategyStore::Clear( void )
{
	StrategyRecord* DelRec;
	for( register int i = 0; i < FTable.ItemCount(); i++ )
	{
		DelRec = FTable.GetItem( i );
			delete DelRec;
	}
	FTable.Clear();
}
//---------------------------------------------------------------------------
StrategyRecord* __fastcall TCStrategyStore::GetRecord( int Index )
{
	if( Index < FTable.ItemCount() )
	{
		return FTable.GetItem( Index );
	}
	else
		return NULL;
}
//---------------------------------------------------------------------------
int __fastcall TCStrategyStore::ItemCount( void )
{
	return FTable.ItemCount();
}
//---------------------------------------------------------------------------
Classes::TStringList* __fastcall TCStrategyStore::GetSymbols( void ) { return NULL; }
void TCStrategyStore::OnMarketDataUpdate( MarketDataMessage* Msg ){}
void TCStrategyStore::OnMarketDataUpdate( BasicData* Msg ) {}
void TCStrategyStore::OnMarketDataUpdate( MatchInfo* Msg ){}
void TCStrategyStore::OnMarketDataUpdate( TotalMatch* Msg ){}
void TCStrategyStore::OnMarketDataUpdate( DayHighLowPrice* Msg ){}
void TCStrategyStore::OnMarketDataUpdate( OpeningInfo* Msg ) {}
void TCStrategyStore::OnMarketDataUpdate( OrderBookData* Msg ){}
void TCStrategyStore::OnMarketDataUpdate( ClosingMarketData* Msg ) {}
void TCStrategyStore::OnMarketDataUpdate( UnderlyingIndexInfo* Msg ) {}
void TCStrategyStore::OnMarketDataUpdate( SumOfOrderInfo* Msg ) {}
//---------------------------------------------------------------------------
namespace Strategystore
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TCStrategyStore)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
