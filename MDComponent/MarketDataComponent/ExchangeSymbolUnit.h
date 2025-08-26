//---------------------------------------------------------------------------

#ifndef ExchangeSymbolUnitH
#define ExchangeSymbolUnitH
#include <vcl.h>
#include "OrderMessageDefine.h"

//------------------------------------------------------------------------------
bool IsDigitalCharacter(const char Data);
bool IsDigitalCharacter(const wchar_t Data);
bool ExtractStrikePriceFromTaifexSingleSymbol(const UnicodeString& OriginalSymbol,
											  int MaxStrikePriceLength, int LastIndex,
											  int& StrikePrice, int& StrikePriceLength);
int GetMonthByTaifexMonthCode(char MonthCode);
int GetMonthByTaifexMonthCode(wchar_t MonthCode);
bool ExtractMonthCodeFromTaifexSingleSymbol(const UnicodeString& OriginalSymbol,
											int MonthCodeIndex, wchar_t& MonthCode, int& MonthValue);
int GetYearByTaifexYearCode(const UnicodeString& YearCode);
bool ExtractYearCodeFromTaifexSingleSymbol(const UnicodeString& OriginalSymbol,
										   int& StartIndex, UnicodeString& YearCode, int& YearValue);
bool ParseTaifexSingleSymbol(nsOrderMessageDefine::MarketEnum Market,
							 const UnicodeString& OriginalSymbol,
							 UnicodeString& ContractId,
							 nsOrderMessageDefine::EventTypeEnum& CallOrPut,
							 int& StrikePrice,
							 wchar_t& MaturityMonthCode, int& MaturityMonth,
							 UnicodeString& MaturityYearCode, int& MaturityYear);
bool ParseTaifexPriceSpreadSymbol(nsOrderMessageDefine::MarketEnum Market,
								  const UnicodeString& OriginalSymbol,
								  const nsOrderMessageDefine::SideEnum OriginalSide,
								  UnicodeString& ContractId,
								  wchar_t& ComposedCode,
								  UnicodeString& Leg1Symbol, UnicodeString& Leg2Symbol,
								  nsOrderMessageDefine::EventTypeEnum& Leg1CallOrPut,
								  nsOrderMessageDefine::EventTypeEnum& Leg2CallOrPut,
								  int& Leg1StrikePrice, int& Leg2StrikePrice,
								  wchar_t& Leg1MaturityMonthCode, int& Leg1MaturityMonth,
								  wchar_t& Leg2MaturityMonthCode, int& Leg2MaturityMonth,
								  UnicodeString& Leg1MaturityYearCode, int& Leg1MaturityYear,
								  UnicodeString& Leg2MaturityYearCode, int& Leg2MaturityYear,
								  nsOrderMessageDefine::SideEnum& Leg1Side,
								  nsOrderMessageDefine::SideEnum& Leg2Side);
bool ParseTaifexTimeSpreadSymbol(nsOrderMessageDefine::MarketEnum Market,
								 const UnicodeString& OriginalSymbol,
								 const nsOrderMessageDefine::SideEnum OriginalSide,
								 UnicodeString& ContractId,
								 wchar_t& ComposedCode,
								 UnicodeString& Leg1Symbol, UnicodeString& Leg2Symbol,
								 nsOrderMessageDefine::EventTypeEnum& Leg1CallOrPut,
								 nsOrderMessageDefine::EventTypeEnum& Leg2CallOrPut,
								 int& Leg1StrikePrice, int& Leg2StrikePrice,
								 wchar_t& Leg1MaturityMonthCode, int& Leg1MaturityMonth,
								 wchar_t& Leg2MaturityMonthCode, int& Leg2MaturityMonth,
								 UnicodeString& Leg1MaturityYearCode, int& Leg1MaturityYear,
								 UnicodeString& Leg2MaturityYearCode, int& Leg2MaturityYear,
								 nsOrderMessageDefine::SideEnum& Leg1Side,
								 nsOrderMessageDefine::SideEnum& Leg2Side);
bool ParseTaifexStraddleSymbol(nsOrderMessageDefine::MarketEnum Market,
							   const UnicodeString& OriginalSymbol,
							   const nsOrderMessageDefine::SideEnum OriginalSide,
							   UnicodeString& ContractId,
							   wchar_t& ComposedCode,
							   UnicodeString& Leg1Symbol, UnicodeString& Leg2Symbol,
							   nsOrderMessageDefine::EventTypeEnum& Leg1CallOrPut,
							   nsOrderMessageDefine::EventTypeEnum& Leg2CallOrPut,
							   int& Leg1StrikePrice, int& Leg2StrikePrice,
							   wchar_t& Leg1MaturityMonthCode, int& Leg1MaturityMonth,
							   wchar_t& Leg2MaturityMonthCode, int& Leg2MaturityMonth,
							   UnicodeString& Leg1MaturityYearCode, int& Leg1MaturityYear,
							   UnicodeString& Leg2MaturityYearCode, int& Leg2MaturityYear,
							   nsOrderMessageDefine::SideEnum& Leg1Side,
							   nsOrderMessageDefine::SideEnum& Leg2Side);
bool ParseTaifexStrangleSymbol(nsOrderMessageDefine::MarketEnum Market,
							   const UnicodeString& OriginalSymbol,
							   const nsOrderMessageDefine::SideEnum OriginalSide,
							   UnicodeString& ContractId,
							   wchar_t& ComposedCode,
							   UnicodeString& Leg1Symbol, UnicodeString& Leg2Symbol,
							   nsOrderMessageDefine::EventTypeEnum& Leg1CallOrPut,
							   nsOrderMessageDefine::EventTypeEnum& Leg2CallOrPut,
							   int& Leg1StrikePrice, int& Leg2StrikePrice,
							   wchar_t& Leg1MaturityMonthCode, int& Leg1MaturityMonth,
							   wchar_t& Leg2MaturityMonthCode, int& Leg2MaturityMonth,
							   UnicodeString& Leg1MaturityYearCode, int& Leg1MaturityYear,
							   UnicodeString& Leg2MaturityYearCode, int& Leg2MaturityYear,
							   nsOrderMessageDefine::SideEnum& Leg1Side,
							   nsOrderMessageDefine::SideEnum& Leg2Side);
bool ParseTaifexConversionSymbol(nsOrderMessageDefine::MarketEnum Market,
								 const UnicodeString& OriginalSymbol,
								 const nsOrderMessageDefine::SideEnum OriginalSide,
								 UnicodeString& ContractId,
								 wchar_t& ComposedCode,
								 UnicodeString& Leg1Symbol, UnicodeString& Leg2Symbol,
								 nsOrderMessageDefine::EventTypeEnum& Leg1CallOrPut,
								 nsOrderMessageDefine::EventTypeEnum& Leg2CallOrPut,
								 int& Leg1StrikePrice, int& Leg2StrikePrice,
								 wchar_t& Leg1MaturityMonthCode, int& Leg1MaturityMonth,
								 wchar_t& Leg2MaturityMonthCode, int& Leg2MaturityMonth,
								 UnicodeString& Leg1MaturityYearCode, int& Leg1MaturityYear,
								 UnicodeString& Leg2MaturityYearCode, int& Leg2MaturityYear,
								 nsOrderMessageDefine::SideEnum& Leg1Side,
								 nsOrderMessageDefine::SideEnum& Leg2Side);
bool ParseTaifexFuturesComposedSymbol(nsOrderMessageDefine::MarketEnum Market,
									  const UnicodeString& OriginalSymbol,
									  const nsOrderMessageDefine::SideEnum OriginalSide,
									  UnicodeString& ContractId,
									  wchar_t& ComposedCode,
									  UnicodeString& Leg1Symbol, UnicodeString& Leg2Symbol,
									  nsOrderMessageDefine::EventTypeEnum& Leg1CallOrPut,
									  nsOrderMessageDefine::EventTypeEnum& Leg2CallOrPut,
									  int& Leg1StrikePrice, int& Leg2StrikePrice,
									  wchar_t& Leg1MaturityMonthCode, int& Leg1MaturityMonth,
									  wchar_t& Leg2MaturityMonthCode, int& Leg2MaturityMonth,
									  UnicodeString& Leg1MaturityYearCode, int& Leg1MaturityYear,
									  UnicodeString& Leg2MaturityYearCode, int& Leg2MaturityYear,
									  nsOrderMessageDefine::SideEnum& Leg1Side,
									  nsOrderMessageDefine::SideEnum& Leg2Side);
bool ParseTaifexSymbol(nsOrderMessageDefine::MarketEnum Market,
					   const UnicodeString& OriginalSymbol,
					   const nsOrderMessageDefine::SideEnum OriginalSide,
					   UnicodeString& ContractId,
					   wchar_t& ComposedCode,
					   UnicodeString& Leg1Symbol, UnicodeString& Leg2Symbol,
					   nsOrderMessageDefine::EventTypeEnum& Leg1CallOrPut,
					   nsOrderMessageDefine::EventTypeEnum& Leg2CallOrPut,
					   int& Leg1StrikePrice, int& Leg2StrikePrice,
					   wchar_t& Leg1MaturityMonthCode, int& Leg1MaturityMonth,
					   wchar_t& Leg2MaturityMonthCode, int& Leg2MaturityMonth,
					   UnicodeString& Leg1MaturityYearCode, int& Leg1MaturityYear,
					   UnicodeString& Leg2MaturityYearCode, int& Leg2MaturityYear,
					   nsOrderMessageDefine::SideEnum& Leg1Side,
					   nsOrderMessageDefine::SideEnum& Leg2Side);
//------------------------------------------------------------------------------
class TExchangeSymbol : public TObject
{
private:
	nsOrderMessageDefine::MarketEnum FMarket;
	UnicodeString                    FExchangeId;
	UnicodeString                    FOriginalSymbol;
	nsOrderMessageDefine::SideEnum   FSide;

	UnicodeString                    FContractId;
	wchar_t                          FComposedCode;
	bool                             FIsMultileg;
	bool                             FIsCorrectCommodity;

	//Leg1 Data Field
	UnicodeString                       FLeg1Symbol;
	nsOrderMessageDefine::EventTypeEnum FLeg1CallPut;
	int                                 FLeg1StrikePrice;
	UnicodeString                       FLeg1MaturityYearCode;
	int                                 FLeg1MaturityYear;
	wchar_t                             FLeg1MaturityMonthCode;
	int                                 FLeg1MaturityMonth;
	nsOrderMessageDefine::SideEnum      FLeg1Side;

	//Leg2 Data Field
	UnicodeString                       FLeg2Symbol;
	nsOrderMessageDefine::EventTypeEnum FLeg2CallPut;
	int                                 FLeg2StrikePrice;
	UnicodeString                       FLeg2MaturityYearCode;
	int                                 FLeg2MaturityYear;
	wchar_t                             FLeg2MaturityMonthCode;
	int                                 FLeg2MaturityMonth;
	nsOrderMessageDefine::SideEnum      FLeg2Side;

	bool ParseSymbol();

public:
	TExchangeSymbol();
	TExchangeSymbol(const UnicodeString& OriginalSymbol);
	TExchangeSymbol(nsOrderMessageDefine::MarketEnum Market,
					const UnicodeString& ExchangeId,
					const UnicodeString& OriginalSymbol,
					nsOrderMessageDefine::SideEnum Side = nsOrderMessageDefine::sNone);

	nsOrderMessageDefine::MarketEnum GetMarket() {return FMarket;};
	void SetMarket(nsOrderMessageDefine::MarketEnum Market) {FMarket = Market;};
	UnicodeString GetExchangeId() {return FExchangeId;};
	void SetExchangeId(const UnicodeString& ExchangeId) {FExchangeId = ExchangeId;};
	UnicodeString GetOriginalSymbol() {return FOriginalSymbol;};
	void SetOriginalSymbol(const UnicodeString& OriginalSymbol) {FOriginalSymbol = OriginalSymbol;};
	bool IsCorrectCommodity() {return FIsCorrectCommodity;};
	bool IsMultileg() {return FIsMultileg;};

	UnicodeString GetContractId() {return FContractId;};
	wchar_t GetComposedCode() {return FComposedCode;};
	UnicodeString GetSymbol(int LegNo = 0);
	int GetMaturityYear(int LegNo = 0);
	int GetMaturityMonth(int LegNo = 0);
	UnicodeString GetMaturityYearMonth(int LegNo = 0);
	nsOrderMessageDefine::EventTypeEnum GetCallPut(int LegNo = 0);
	char GetCallPutCode(int LegNo = 0);
	int GetStrikePx(int LegNo = 0);
	nsOrderMessageDefine::SideEnum GetSide(int LegNo = 0);

	bool SetSymbol(nsOrderMessageDefine::MarketEnum Market,
				   const UnicodeString& ExchangeId,
				   const UnicodeString& OriginalSymbol,
				   nsOrderMessageDefine::SideEnum Side = nsOrderMessageDefine::sNone);
};  //TExchangeSymbol
//---------------------------------------------------------------------------
#endif
