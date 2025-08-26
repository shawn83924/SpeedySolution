//---------------------------------------------------------------------------

#ifndef StrategyStoreH
#define StrategyStoreH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include "OrderMessageDefine.h"
#include "MarketDataStore.h"
//---------------------------------------------------------------------------
using namespace nsOrderMessageDefine;
//---------------------------------------------------------------------------
typedef enum strategyEnum
{
	sPriceSpread = 0,
	sStrangle = 1,
	sStraddle = 2,
	sConversion = 3,
	sReversals = 4,
	sTimeSpread = 5
}StrategyEnum;
//---------------------------------------------------------------------------
class StrategySymbolIlligalException: public UFC::Exception
{
private:
	UFC::AnsiString FLeg1Symbol;
	UFC::AnsiString FLeg2Symbol;
public:
	StrategySymbolIlligalException( const UFC::AnsiString& ErrMsg, const UFC::AnsiString& Leg1, const UFC::AnsiString& Leg2 );
};
//---------------------------------------------------------------------------
class StrategyRecord
{
private:
	UFC::AnsiString   FSymbol;
	UFC::AnsiString   FLeg1Symbol;
	UFC::AnsiString   FLeg2Symbol;
	StrategyEnum      FStrategy;
	BasicInformation* FLeg1;
	BasicInformation* FLeg2;
	SideEnum          FSide;
	double         Leg1Px;
	double         Leg1BidPx;
	double         Leg1AskPx;
	SideEnum       FSide1;
	EventTypeEnum  FCallPut1;
	double         FStrikePx1;
	double         Leg2Px;
	double         Leg2BidPx;
	double         Leg2AskPx;
	SideEnum       FSide2;
	EventTypeEnum  FCallPut2;
	double         FStrikePx2;
public:
	StrategyRecord( BasicInformation* Leg1, BasicInformation* Leg2, StrategyEnum Strategy, SideEnum side );
	void CheckSymbol( void );
	void CombineSymbol( void );
	void CalculateBEP( void );
	UFC::AnsiString& GetSymbol( void ) { return FSymbol; }
	UFC::AnsiString& GetLeg1Symbol( void ) { return FLeg1Symbol; }
	UFC::AnsiString& GetLeg2Symbol( void ) { return FLeg2Symbol; }
	SideEnum GetSide( void ) { return FSide; }
	SideEnum GetLeg1Side() { return FSide1; }
	SideEnum GetLeg2Side() { return FSide2; }
	EventTypeEnum GetLeg1CP() { return FCallPut1; }
	EventTypeEnum GetLeg2CP() { return FCallPut2; }
	double GetLeg1StrikePx() { return FStrikePx1; }
	double GetLeg2StrikePx() { return FStrikePx2; }
	StrategyEnum GetStrategy( void ) { return FStrategy; }
};
//---------------------------------------------------------------------------
class PACKAGE TCStrategyStore : public TComponent, public IMarketDataListener
{
private:
	String FPrefix;
	String FYearMonth;
	StrategyEnum FStrategy;
	TCMarketDataStore* FStore;
	UFC::PHashedList<UFC::AnsiString, StrategyRecord*> FTable;
protected:
public:
	__fastcall TCStrategyStore(TComponent* Owner);
private:
	virtual Classes::TStringList* __fastcall GetSymbols( void );
	virtual void OnMarketDataUpdate( MarketDataMessage* Msg );
	virtual void OnMarketDataUpdate( BasicData* Msg );
	virtual void OnMarketDataUpdate( MatchInfo* Msg );
	virtual void OnMarketDataUpdate( TotalMatch* Msg );
	virtual void OnMarketDataUpdate( DayHighLowPrice* Msg );
	virtual void OnMarketDataUpdate( OpeningInfo* Msg );
	virtual void OnMarketDataUpdate( OrderBookData* Msg );
	virtual void OnMarketDataUpdate( ClosingMarketData* Msg );
	virtual void OnMarketDataUpdate( UnderlyingIndexInfo* Msg );
	virtual void OnMarketDataUpdate( SumOfOrderInfo* Msg );
public:
	void __fastcall SetMarketDataStore( TCMarketDataStore* Store );
	void __fastcall SetStrategy( String OPTPrefix, String YearMonth, StrategyEnum Strategy, SideEnum Side );
	void __fastcall Clear( void );
	StrategyRecord* __fastcall GetRecord( int Index );
	int __fastcall ItemCount( void );
__published:
	__property TCMarketDataStore* Store = { read = FStore, write = SetMarketDataStore };

};
//---------------------------------------------------------------------------
#endif
