//---------------------------------------------------------------------------

#ifndef PositionChartH
#define PositionChartH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <Controls.hpp>
#include "MarketDataStore.h"
#include "TickList.h"
//---------------------------------------------------------------------------
class ChartRecord
{
private:
	UFC::AnsiString FSymbol;
	int FQty;
	double FPrice;  ///< Avg Px for this position
	double FBestBuyPx;
	double FBestSellPx;
	double FStrikePrice;
	double FBEPPx;
	double FSumPxQty;
	Market FMarket;
	nsOrderMessageDefine::SideEnum FSide;
	CallPutCode FCPCode;
	double FContractMultiplier;
public:
	ChartRecord( const UFC::AnsiString& Symbol, Market market, double StrikePx, CallPutCode CallPut, nsOrderMessageDefine::SideEnum Side, double ContracMultiplier );
	int __fastcall GetQty( void ) { return FQty; }
	double __fastcall GetPrice( void ) { return FPrice; }
	double __fastcall GetBestBuyPx( void ) { return FBestBuyPx; }
	double __fastcall GetBestSellPx( void ) { return FBestSellPx; }
	double __fastcall GetStrikePx( void ) { return FStrikePrice; }
	double __fastcall GetBEPPx( void ) { return  FBEPPx; }
	Market __fastcall GetMarket( void ) { return FMarket; }
	nsOrderMessageDefine::SideEnum __fastcall GetSide( void ) { return FSide; }
	CallPutCode __fastcall GetCallPut( void ) { return FCPCode; }
	double __fastcall GetContractMultiplier( void ) { return FContractMultiplier; }
	void __fastcall AddPosition( double Px, int Qty );
	void __fastcall SetBestBuyPx( double Px ) { FBestBuyPx = Px; }
	void __fastcall SetBestSellPx( double Px ) { FBestSellPx = Px; }
	void __fastcall SetPx( double Px );
	UFC::AnsiString GetSymbol( void ) { return FSymbol;}
};
//---------------------------------------------------------------------------
class PxPoint
{
private:
	double FPx;
	double FProfit;
public:
	PxPoint(){}
	__fastcall PxPoint( double Px, double Profit ):FPx( Px ), FProfit( Profit ) {}
	double __fastcall GetPx( void ) { return FPx; }
	double __fastcall GetProfit( void ) { return FProfit; }
	void __fastcall AddProfit( double Px ) { FProfit += Px; }
};
//---------------------------------------------------------------------------
class PxPointCompare : public UFC::CompareInterface
{
	virtual int Compare( void* elem1, void* elem2 );
};
//---------------------------------------------------------------------------
class PACKAGE TPositionChart : public TCustomControl, public IMarketDataListener
{
private:
	String FSymbol;
	Graphics::TBitmap* FBuffer;
	int	FPxWidth;
	TCMarketDataStore* FMarketDataStore;
	UFC::PHashedList<UFC::AnsiString, ChartRecord*> FTable;
	UFC::PHashedList<int, PxPoint*> FPosSummary;
	UFC::PList FPosList;
	double FMaxStrikePx;
	double FMinStrikePx;
	double FMaxPx;
	double FMinPx;
protected:
private:
	void __fastcall WndProc( TMessage &Msg );
	void __fastcall DrawBackground( TCanvas* canvas, TRect& BufferRect, bool IsFocus );
	void __fastcall DrawCoordinate( TCanvas* canvas );
	void __fastcall DrawPosition( TCanvas* canvas );
	void __fastcall CalPointProfit( nsOrderMessageDefine::SideEnum side, CallPutCode CallPut, double StkPx, double cost, double CM, PxPoint* point );
	void __fastcall CalculatePosition( void );
	void __fastcall PxToCoordinate( double Px, double Profit, int& X, int & Y );
private:
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
	virtual Classes::TStringList* __fastcall GetSymbols( void );
public:
	__fastcall TPositionChart(TComponent* Owner);
	__fastcall ~TPositionChart( void );
	void __fastcall Clear( void );
	void __fastcall Paint( void );
	void __fastcall AddSymbol( String Exchange, String Symbol, nsOrderMessageDefine::SideEnum Side, int Qty = 1 );
	void __fastcall SetMarketDataStore( TCMarketDataStore* Store );
protected:
	DYNAMIC void __fastcall Resize( void );
__published:
	__property Align;
	__property TCMarketDataStore* MarketDataStore = { read = FMarketDataStore, write = SetMarketDataStore };

};
//---------------------------------------------------------------------------
#endif
