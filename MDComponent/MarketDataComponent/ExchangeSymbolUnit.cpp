//---------------------------------------------------------------------------


#pragma hdrstop

#include <dateutils.hpp>
#include "ExchangeSymbolUnit.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
bool IsDigitalCharacter(const char Data)
{
	if ((Data < '0') || (Data > '9'))
		return false;
	else
		return true;
}  //IsDigitalCharacter()

//---------------------------------------------------------------------------
bool IsDigitalCharacter(const wchar_t Data)
{
	if ((Data < L'0') || (Data > L'9'))
		return false;
	else
		return true;
}  //IsDigitalCharacter()

//---------------------------------------------------------------------------
bool ExtractStrikePriceFromTaifexSingleSymbol(const UnicodeString& OriginalSymbol,
											  int MaxStrikePriceLength, int LastIndex,
											  int& StrikePrice, int& StrikePriceLength)
{
	StrikePrice = 0;
	int OriginalSymbolLength = OriginalSymbol.Length();
	if ((LastIndex < 1) || (LastIndex > OriginalSymbolLength)) return false;

	UnicodeString StrikePriceStr = L"";
	for (int i = LastIndex; i > 0; i--)
	{
		if ((StrikePriceStr.Length() < MaxStrikePriceLength) && IsDigitalCharacter(OriginalSymbol[i]))
			StrikePriceStr = OriginalSymbol.SubString(i, 1) + StrikePriceStr;
		else
			break;
	}  //for (int i = LastIndex; i > 0; i--)

	StrikePriceLength = StrikePriceStr.Length();
	if ((StrikePriceLength > 0) && (StrikePriceLength <= MaxStrikePriceLength))
	{
		StrikePrice = StrikePriceStr.ToInt();
		return true;
	}
	else
		return false;
}  //ExtractStrikePriceFromTaifexSingleSymbol()

//---------------------------------------------------------------------------
int GetMonthByTaifexMonthCode(char MonthCode)
{
	int Month;
	switch(MonthCode)
	{
		case 'A':  case 'M': Month =  1; break;
		case 'B':  case 'N': Month =  2; break;
		case 'C':  case 'O': Month =  3; break;
		case 'D':  case 'P': Month =  4; break;
		case 'E':  case 'Q': Month =  5; break;
		case 'F':  case 'R': Month =  6; break;
		case 'G':  case 'S': Month =  7; break;
		case 'H':  case 'T': Month =  8; break;
		case 'I':  case 'U': Month =  9; break;
		case 'J':  case 'V': Month = 10; break;
		case 'K':  case 'W': Month = 11; break;
		case 'L':  case 'X': Month = 12; break;
		default: Month = 0;
	}  //switch(MonthCode)

	return Month;
}  //GetMonthByTaifexMonthCode()

//---------------------------------------------------------------------------
int GetMonthByTaifexMonthCode(wchar_t MonthCode)
{
	int Month;
	switch(MonthCode)
	{
		case L'A':  case L'M': Month =  1; break;
		case L'B':  case L'N': Month =  2; break;
		case L'C':  case L'O': Month =  3; break;
		case L'D':  case L'P': Month =  4; break;
		case L'E':  case L'Q': Month =  5; break;
		case L'F':  case L'R': Month =  6; break;
		case L'G':  case L'S': Month =  7; break;
		case L'H':  case L'T': Month =  8; break;
		case L'I':  case L'U': Month =  9; break;
		case L'J':  case L'V': Month = 10; break;
		case L'K':  case L'W': Month = 11; break;
		case L'L':  case L'X': Month = 12; break;
		default: Month = 0;
	}  //switch(MonthCode)

	return Month;
}  //GetMonthByTaifexMonthCode()

//---------------------------------------------------------------------------
bool ExtractMonthCodeFromTaifexSingleSymbol(const UnicodeString& OriginalSymbol,
											int MonthCodeIndex, wchar_t& MonthCode, int& MonthValue)
{
	MonthCode = OriginalSymbol[MonthCodeIndex];
	MonthValue = GetMonthByTaifexMonthCode(MonthCode);
	if ((MonthValue >= 1) && (MonthValue <= 12))
		return true;
	else
		return false;
}  //ExtractMonthCodeFromTaifexSingleSymbol()

//---------------------------------------------------------------------------
int GetYearByTaifexYearCode(const UnicodeString& YearCode)
{
	int MaturityYear = 0;
	for (int i = 1; i <= YearCode.Length(); i++)
		if (!IsDigitalCharacter(YearCode[i])) return MaturityYear;

	int ThisYear = YearOf(Now());
	if (YearCode.Length() == 1)
	{
		int BeginYear, EndYear, CurrentYear;
		BeginYear = ThisYear;
		EndYear = ThisYear + 9;
		MaturityYear = 0;
		for (CurrentYear = BeginYear; CurrentYear <= EndYear; CurrentYear++)
		{
			 UnicodeString YearString;
			 YearString.printf(L"%04d", CurrentYear);
			 if (YearString[4] == YearCode[1])
			 {
				 MaturityYear = YearString.ToInt();
				 break;
			 }
		}  //for (CurrentYear = BeginYear; CurrentYear <= EndYear; CurrentYear++)
	}
	else if (YearCode.Length() == 2)
	{
		UnicodeString ThisYearString;
		ThisYearString.printf(L"%04d", ThisYear);
		UnicodeString MaturityYearString = ThisYearString.SubString(1, 2) + YearCode;
		MaturityYear = MaturityYearString.ToInt();
	}
	else
	{
		MaturityYear = 0;
	}  //if (YearCode.Length() == 1)

	return MaturityYear;
}  //GetYearByTaifexYearCode()

//---------------------------------------------------------------------------
bool ExtractYearCodeFromTaifexSingleSymbol(const UnicodeString& OriginalSymbol,
										   int& StartIndex, UnicodeString& YearCode, int& YearValue)
{
	bool ExtractSuccess = false;
	YearCode = L"";
	StartIndex = -1;
	YearValue = 0;
	int OriginalSymbolLength = OriginalSymbol.Length();
	if (OriginalSymbolLength < 1) return ExtractSuccess;

	for (int i = OriginalSymbolLength; i >= 1; i--)
	{
		if (!IsDigitalCharacter(OriginalSymbol[i])) break;

		StartIndex = i;
		YearCode = OriginalSymbol.SubString(StartIndex, 1) + YearCode;
	}  //for (int i = OriginalSymbolLength; i >= 1; i--)

	if (YearCode.Length() > 0)
	{
		YearValue = GetYearByTaifexYearCode(YearCode);
		if (YearValue > 0) ExtractSuccess = true;
	}

	return ExtractSuccess;
}  //ExtractYearCodeFromTaifexSingleSymbol()

//---------------------------------------------------------------------------
bool ParseTaifexSingleSymbol(nsOrderMessageDefine::MarketEnum Market,
							 const UnicodeString& OriginalSymbol,
							 UnicodeString& ContractId,
							 nsOrderMessageDefine::EventTypeEnum& CallOrPut,
							 int& StrikePrice,
							 wchar_t& MaturityMonthCode, int& MaturityMonth,
							 UnicodeString& MaturityYearCode, int& MaturityYear)
{
	bool IsParsingSuccess = false;
	int OriginalSymbolLength = OriginalSymbol.Length();
	if (OriginalSymbolLength <= 4) return IsParsingSuccess;

	UnicodeString TempContractId = L"";
	nsOrderMessageDefine::EventTypeEnum TempCallOrPut = nsOrderMessageDefine::evtNone;
	int TempStrikePrice = 0;
	wchar_t TempMaturityMonthCode = 0;
	int TempMaturityMonth = 0;
	UnicodeString TempMaturityYearCode = L"";
	int TempMaturityYear = 0;

	int YearCodeStartIndex;
	if (!ExtractYearCodeFromTaifexSingleSymbol(OriginalSymbol, YearCodeStartIndex, TempMaturityYearCode, TempMaturityYear))
		return IsParsingSuccess;

	int MonthCodeIndex = YearCodeStartIndex - 1;
	if (!ExtractMonthCodeFromTaifexSingleSymbol(OriginalSymbol, MonthCodeIndex,
												TempMaturityMonthCode, TempMaturityMonth))
		return IsParsingSuccess;

	int ContractIdLastIndex;// = 0;
	if (Market == nsOrderMessageDefine::mTWOptions)
	{
		int StrikePriceLastIndex = MonthCodeIndex - 1;
		int StrikePriceLength = 0;
		if (ExtractStrikePriceFromTaifexSingleSymbol(OriginalSymbol, 5, StrikePriceLastIndex,
													 TempStrikePrice, StrikePriceLength))
			ContractIdLastIndex = StrikePriceLastIndex - StrikePriceLength;
		else
			ContractIdLastIndex = StrikePriceLastIndex;
	}
	else
		ContractIdLastIndex = MonthCodeIndex - 1;

	if (ContractIdLastIndex > 0)
	{
		TempContractId = OriginalSymbol.SubString(1, ContractIdLastIndex);
		if (TempStrikePrice > 0)
		{
			if ((TempMaturityMonthCode >= L'A') && (TempMaturityMonthCode <= L'L'))
			{
				TempCallOrPut = nsOrderMessageDefine::evtCall;
				IsParsingSuccess = true;
			}
			else if ((TempMaturityMonthCode >= L'M') && (TempMaturityMonthCode <= L'X'))
			{
				TempCallOrPut = nsOrderMessageDefine::evtPut;
				IsParsingSuccess = true;
			}
			else
				TempCallOrPut = nsOrderMessageDefine::evtNone;
		}
		else
		{
			TempCallOrPut = nsOrderMessageDefine::evtNone;
			if ((TempMaturityMonthCode >= L'A') && (TempMaturityMonthCode <= L'L'))
				IsParsingSuccess = true;
		}
	}  //if (ContractIdLastIndex > 0)

	if (IsParsingSuccess)
	{
		ContractId = TempContractId;
		CallOrPut = TempCallOrPut;
		StrikePrice = TempStrikePrice;
		MaturityMonthCode = TempMaturityMonthCode;
		MaturityMonth = TempMaturityMonth;
		MaturityYearCode = TempMaturityYearCode;
		MaturityYear = TempMaturityYear;
	}  //if (IsParsingSuccess)

	return IsParsingSuccess;
}  //ParseTaifexSingleSymbol()

//---------------------------------------------------------------------------
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
								  nsOrderMessageDefine::SideEnum& Leg2Side)
{
	if (OriginalSide == nsOrderMessageDefine::sBuy)
	{
		Leg1Side = nsOrderMessageDefine::sSell;
		Leg2Side = nsOrderMessageDefine::sBuy;
	}
	else if (OriginalSide == nsOrderMessageDefine::sSell)
	{
		Leg1Side = nsOrderMessageDefine::sBuy;
		Leg2Side = nsOrderMessageDefine::sSell;
	}
	else
	{
		Leg1Side = nsOrderMessageDefine::sNone;
		Leg2Side = nsOrderMessageDefine::sNone;
	}

	ContractId = OriginalSymbol.SubString(1, 3);
	ComposedCode = OriginalSymbol[9];
	UnicodeString Leg1StrikePriceStr = OriginalSymbol.SubString(4, 5);
	UnicodeString Leg2StrikePriceStr = OriginalSymbol.SubString(10, 5);
	UnicodeString OriginalMonthCode = OriginalSymbol.SubString(15, 1);
	UnicodeString OriginalYearCode = OriginalSymbol.SubString(16, 1);
	UnicodeString TempContractId;
	Leg1Symbol = ContractId + Leg1StrikePriceStr + OriginalMonthCode + OriginalYearCode;
	bool IsLeg1ParseSuccess = ParseTaifexSingleSymbol(Market, Leg1Symbol, TempContractId,
													  Leg1CallOrPut, Leg1StrikePrice,
													  Leg1MaturityMonthCode, Leg1MaturityMonth,
													  Leg1MaturityYearCode, Leg1MaturityYear);
	Leg2Symbol = ContractId + Leg2StrikePriceStr + OriginalMonthCode + OriginalYearCode;
	bool IsLeg2ParseSuccess = ParseTaifexSingleSymbol(Market, Leg2Symbol, TempContractId,
													  Leg2CallOrPut, Leg2StrikePrice,
													  Leg2MaturityMonthCode, Leg2MaturityMonth,
													  Leg2MaturityYearCode, Leg2MaturityYear);
	bool IsCorrectSymbol = false;
	if (IsLeg1ParseSuccess && IsLeg2ParseSuccess)
	{
		if (((Leg1CallOrPut == nsOrderMessageDefine::evtCall) &&
			 (Leg1StrikePrice > Leg2StrikePrice)) ||
			((Leg1CallOrPut == nsOrderMessageDefine::evtPut) &&
			 (Leg1StrikePrice < Leg2StrikePrice)))
			IsCorrectSymbol = true;
	}

	return IsCorrectSymbol;
}  //ParseTaifexPriceSpreadSymbol()

//---------------------------------------------------------------------------
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
								 nsOrderMessageDefine::SideEnum& Leg2Side)
{
	if (OriginalSide == nsOrderMessageDefine::sBuy)
	{
		Leg1Side = nsOrderMessageDefine::sSell;
		Leg2Side = nsOrderMessageDefine::sBuy;
	}
	else if (OriginalSide == nsOrderMessageDefine::sSell)
	{
		Leg1Side = nsOrderMessageDefine::sBuy;
		Leg2Side = nsOrderMessageDefine::sSell;
	}
	else
	{
		Leg1Side = nsOrderMessageDefine::sNone;
		Leg2Side = nsOrderMessageDefine::sNone;
	}

	ContractId = OriginalSymbol.SubString(1, 3);
	ComposedCode = OriginalSymbol[11];
	UnicodeString StrikePriceStr = OriginalSymbol.SubString(4, 5);
	UnicodeString Leg1MonthCode = OriginalSymbol.SubString(9, 1);
	UnicodeString Leg1YearCode = OriginalSymbol.SubString(10, 1);
	UnicodeString Leg2MonthCode = OriginalSymbol.SubString(12, 1);
	UnicodeString Leg2YearCode = OriginalSymbol.SubString(13, 1);

	UnicodeString TempContractId;
	Leg1Symbol = ContractId + StrikePriceStr + Leg1MonthCode + Leg1YearCode;
	bool IsLeg1ParseSuccess = ParseTaifexSingleSymbol(Market, Leg1Symbol, TempContractId,
													  Leg1CallOrPut, Leg1StrikePrice,
													  Leg1MaturityMonthCode, Leg1MaturityMonth,
													  Leg1MaturityYearCode, Leg1MaturityYear);
	Leg2Symbol = ContractId + StrikePriceStr + Leg2MonthCode + Leg2YearCode;
	bool IsLeg2ParseSuccess = ParseTaifexSingleSymbol(Market, Leg2Symbol, TempContractId,
													  Leg2CallOrPut, Leg2StrikePrice,
													  Leg2MaturityMonthCode, Leg2MaturityMonth,
													  Leg2MaturityYearCode, Leg2MaturityYear);
	bool IsCorrectSymbol = false;
	if (IsLeg1ParseSuccess && IsLeg2ParseSuccess)
	{
		if (Leg1CallOrPut == Leg2CallOrPut)
		{
			int Leg1YearMonth = Leg1MaturityYear * 100 +  Leg1MaturityMonth;
			int Leg2YearMonth = Leg2MaturityYear * 100 +  Leg2MaturityMonth;
			if (Leg1YearMonth < Leg2YearMonth) IsCorrectSymbol = true;
		}
	}

	return IsCorrectSymbol;
}  //ParseTaifexTimeSpreadSymbol()

//---------------------------------------------------------------------------
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
							   nsOrderMessageDefine::SideEnum& Leg2Side)
{
	if (OriginalSide == nsOrderMessageDefine::sBuy)
	{
		Leg1Side = nsOrderMessageDefine::sBuy;
		Leg2Side = nsOrderMessageDefine::sBuy;
	}
	else if (OriginalSide == nsOrderMessageDefine::sSell)
	{
		Leg1Side = nsOrderMessageDefine::sSell;
		Leg2Side = nsOrderMessageDefine::sSell;
	}
	else
	{
		Leg1Side = nsOrderMessageDefine::sNone;
		Leg2Side = nsOrderMessageDefine::sNone;
	}

	ContractId = OriginalSymbol.SubString(1, 3);
	ComposedCode = OriginalSymbol[11];
	UnicodeString StrikePriceStr = OriginalSymbol.SubString(4, 5);
	UnicodeString Leg1MonthCode = OriginalSymbol.SubString(9, 1);
	UnicodeString Leg1YearCode = OriginalSymbol.SubString(10, 1);
	UnicodeString Leg2MonthCode = OriginalSymbol.SubString(12, 1);
	UnicodeString Leg2YearCode = OriginalSymbol.SubString(13, 1);

	UnicodeString TempContractId;
	Leg1Symbol = ContractId + StrikePriceStr + Leg1MonthCode + Leg1YearCode;
	bool IsLeg1ParseSuccess = ParseTaifexSingleSymbol(Market, Leg1Symbol, TempContractId,
													  Leg1CallOrPut, Leg1StrikePrice,
													  Leg1MaturityMonthCode, Leg1MaturityMonth,
													  Leg1MaturityYearCode, Leg1MaturityYear);
	Leg2Symbol = ContractId + StrikePriceStr + Leg2MonthCode + Leg2YearCode;
	bool IsLeg2ParseSuccess = ParseTaifexSingleSymbol(Market, Leg2Symbol, TempContractId,
													  Leg2CallOrPut, Leg2StrikePrice,
													  Leg2MaturityMonthCode, Leg2MaturityMonth,
													  Leg2MaturityYearCode, Leg2MaturityYear);
	bool IsCorrectSymbol = false;
	if (IsLeg1ParseSuccess && IsLeg2ParseSuccess)
	{
		if ((Leg1CallOrPut == nsOrderMessageDefine::evtCall) &&
			(Leg2CallOrPut == nsOrderMessageDefine::evtPut))
		{
			int Leg1YearMonth = Leg1MaturityYear * 100 +  Leg1MaturityMonth;
			int Leg2YearMonth = Leg2MaturityYear * 100 +  Leg2MaturityMonth;
			if (Leg1YearMonth == Leg2YearMonth) IsCorrectSymbol = true;
		}
	}

	return IsCorrectSymbol;
}  //ParseTaifexStraddleSymbol()

//---------------------------------------------------------------------------
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
							   nsOrderMessageDefine::SideEnum& Leg2Side)
{
	if (OriginalSide == nsOrderMessageDefine::sBuy)
	{
		Leg1Side = nsOrderMessageDefine::sBuy;
		Leg2Side = nsOrderMessageDefine::sBuy;
	}
	else if (OriginalSide == nsOrderMessageDefine::sSell)
	{
		Leg1Side = nsOrderMessageDefine::sSell;
		Leg2Side = nsOrderMessageDefine::sSell;
	}
	else
	{
		Leg1Side = nsOrderMessageDefine::sNone;
		Leg2Side = nsOrderMessageDefine::sNone;
	}

	ContractId = OriginalSymbol.SubString(1, 3);
	ComposedCode = OriginalSymbol[11];
	UnicodeString Leg1StrikePriceStr = OriginalSymbol.SubString(4, 5);
	UnicodeString Leg2StrikePriceStr = OriginalSymbol.SubString(12, 5);
	UnicodeString Leg1MonthCode = OriginalSymbol.SubString(9, 1);
	UnicodeString Leg1YearCode = OriginalSymbol.SubString(10, 1);
	UnicodeString Leg2MonthCode = OriginalSymbol.SubString(17, 1);
	UnicodeString Leg2YearCode = OriginalSymbol.SubString(18, 1);

	UnicodeString TempContractId;
	Leg1Symbol = ContractId + Leg1StrikePriceStr + Leg1MonthCode + Leg1YearCode;
	bool IsLeg1ParseSuccess = ParseTaifexSingleSymbol(Market, Leg1Symbol, TempContractId,
													  Leg1CallOrPut, Leg1StrikePrice,
													  Leg1MaturityMonthCode, Leg1MaturityMonth,
													  Leg1MaturityYearCode, Leg1MaturityYear);
	Leg2Symbol = ContractId + Leg2StrikePriceStr + Leg2MonthCode + Leg2YearCode;
	bool IsLeg2ParseSuccess = ParseTaifexSingleSymbol(Market, Leg2Symbol, TempContractId,
													  Leg2CallOrPut, Leg2StrikePrice,
													  Leg2MaturityMonthCode, Leg2MaturityMonth,
													  Leg2MaturityYearCode, Leg2MaturityYear);
	bool IsCorrectSymbol = false;
	if (IsLeg1ParseSuccess && IsLeg2ParseSuccess)
	{
		if ((Leg1StrikePrice != Leg2StrikePrice) &&
			(Leg1CallOrPut == nsOrderMessageDefine::evtCall) &&
			(Leg2CallOrPut == nsOrderMessageDefine::evtPut))
		{
			int Leg1YearMonth = Leg1MaturityYear * 100 +  Leg1MaturityMonth;
			int Leg2YearMonth = Leg2MaturityYear * 100 +  Leg2MaturityMonth;
			if (Leg1YearMonth == Leg2YearMonth) IsCorrectSymbol = true;
		}
	}

	return IsCorrectSymbol;
}  //ParseTaifexStrangleSymbol()

//---------------------------------------------------------------------------
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
								 nsOrderMessageDefine::SideEnum& Leg2Side)
{
	if (OriginalSide == nsOrderMessageDefine::sBuy)
	{
		Leg1Side = nsOrderMessageDefine::sSell;
		Leg2Side = nsOrderMessageDefine::sBuy;
	}
	else if (OriginalSide == nsOrderMessageDefine::sSell)
	{
		Leg1Side = nsOrderMessageDefine::sBuy;
		Leg2Side = nsOrderMessageDefine::sSell;
	}
	else
	{
		Leg1Side = nsOrderMessageDefine::sNone;
		Leg2Side = nsOrderMessageDefine::sNone;
	}

	ContractId = OriginalSymbol.SubString(1, 3);
	ComposedCode = OriginalSymbol[11];
	UnicodeString StrikePriceStr = OriginalSymbol.SubString(4, 5);
	UnicodeString Leg1MonthCode = OriginalSymbol.SubString(9, 1);
	UnicodeString Leg1YearCode = OriginalSymbol.SubString(10, 1);
	UnicodeString Leg2MonthCode = OriginalSymbol.SubString(12, 1);
	UnicodeString Leg2YearCode = OriginalSymbol.SubString(13, 1);

	UnicodeString TempContractId;
	Leg1Symbol = ContractId + StrikePriceStr + Leg1MonthCode + Leg1YearCode;
	bool IsLeg1ParseSuccess = ParseTaifexSingleSymbol(Market, Leg1Symbol, TempContractId,
													  Leg1CallOrPut, Leg1StrikePrice,
													  Leg1MaturityMonthCode, Leg1MaturityMonth,
													  Leg1MaturityYearCode, Leg1MaturityYear);
	Leg2Symbol = ContractId + StrikePriceStr + Leg2MonthCode + Leg2YearCode;
	bool IsLeg2ParseSuccess = ParseTaifexSingleSymbol(Market, Leg2Symbol, TempContractId,
													  Leg2CallOrPut, Leg2StrikePrice,
													  Leg2MaturityMonthCode, Leg2MaturityMonth,
													  Leg2MaturityYearCode, Leg2MaturityYear);
	bool IsCorrectSymbol = false;
	if (IsLeg1ParseSuccess && IsLeg2ParseSuccess)
	{
		if ((Leg1CallOrPut == nsOrderMessageDefine::evtCall) &&
			(Leg2CallOrPut == nsOrderMessageDefine::evtPut))
		{
			int Leg1YearMonth = Leg1MaturityYear * 100 +  Leg1MaturityMonth;
			int Leg2YearMonth = Leg2MaturityYear * 100 +  Leg2MaturityMonth;
			if (Leg1YearMonth == Leg2YearMonth) IsCorrectSymbol = true;
		}
	}

	return IsCorrectSymbol;
}  //ParseTaifexConversionSymbol()

//---------------------------------------------------------------------------
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
									  nsOrderMessageDefine::SideEnum& Leg2Side)
{
	if (OriginalSide == nsOrderMessageDefine::sBuy)
	{
		Leg1Side = nsOrderMessageDefine::sSell;
		Leg2Side = nsOrderMessageDefine::sBuy;
	}
	else if (OriginalSide == nsOrderMessageDefine::sSell)
	{
		Leg1Side = nsOrderMessageDefine::sBuy;
		Leg2Side = nsOrderMessageDefine::sSell;
	}
	else
	{
		Leg1Side = nsOrderMessageDefine::sNone;
		Leg2Side = nsOrderMessageDefine::sNone;
	}

	ContractId = OriginalSymbol.SubString(1, 3);
	ComposedCode = OriginalSymbol[6];
	UnicodeString Leg1MonthCode = OriginalSymbol.SubString(4, 1);
	UnicodeString Leg1YearCode = OriginalSymbol.SubString(5, 1);
	UnicodeString Leg2MonthCode = OriginalSymbol.SubString(7, 1);
	UnicodeString Leg2YearCode = OriginalSymbol.SubString(8, 1);

	UnicodeString TempContractId;
	Leg1Symbol = ContractId + Leg1MonthCode + Leg1YearCode;
	bool IsLeg1ParseSuccess = ParseTaifexSingleSymbol(Market, Leg1Symbol, TempContractId,
													  Leg1CallOrPut, Leg1StrikePrice,
													  Leg1MaturityMonthCode, Leg1MaturityMonth,
													  Leg1MaturityYearCode, Leg1MaturityYear);
	Leg2Symbol = ContractId + Leg2MonthCode + Leg2YearCode;
	bool IsLeg2ParseSuccess = ParseTaifexSingleSymbol(Market, Leg2Symbol, TempContractId,
													  Leg2CallOrPut, Leg2StrikePrice,
													  Leg2MaturityMonthCode, Leg2MaturityMonth,
													  Leg2MaturityYearCode, Leg2MaturityYear);
	bool IsCorrectSymbol = false;
	if (IsLeg1ParseSuccess && IsLeg2ParseSuccess)
	{
		int Leg1YearMonth = Leg1MaturityYear * 100 +  Leg1MaturityMonth;
		int Leg2YearMonth = Leg2MaturityYear * 100 +  Leg2MaturityMonth;
		if (Leg1YearMonth < Leg2YearMonth) IsCorrectSymbol = true;
	}

	return IsCorrectSymbol;
}  //ParseTaifexFuturesComposedSymbol()

//---------------------------------------------------------------------------
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
					   nsOrderMessageDefine::SideEnum& Leg2Side)
{
	bool IsParsingSuccess;// = false;
	int OriginalSymbolLength = OriginalSymbol.Length();
	if ((OriginalSymbolLength == 16) && (OriginalSymbol[9] == L'/'))  //Options Price Spread
	{
		IsParsingSuccess = ParseTaifexPriceSpreadSymbol(Market, OriginalSymbol, OriginalSide,
														ContractId, ComposedCode,
														Leg1Symbol, Leg2Symbol,
														Leg1CallOrPut, Leg2CallOrPut,
														Leg1StrikePrice, Leg2StrikePrice,
														Leg1MaturityMonthCode, Leg1MaturityMonth,
														Leg2MaturityMonthCode, Leg2MaturityMonth,
														Leg1MaturityYearCode, Leg1MaturityYear,
														Leg2MaturityYearCode, Leg2MaturityYear,
														Leg1Side, Leg2Side);
	}
	else if ((OriginalSymbolLength == 13) && (OriginalSymbol[11] == L'/'))  //Options Time Spread
	{
		IsParsingSuccess = ParseTaifexTimeSpreadSymbol(Market, OriginalSymbol, OriginalSide,
													   ContractId, ComposedCode,
													   Leg1Symbol, Leg2Symbol,
													   Leg1CallOrPut, Leg2CallOrPut,
													   Leg1StrikePrice, Leg2StrikePrice,
													   Leg1MaturityMonthCode, Leg1MaturityMonth,
													   Leg2MaturityMonthCode, Leg2MaturityMonth,
													   Leg1MaturityYearCode, Leg1MaturityYear,
													   Leg2MaturityYearCode, Leg2MaturityYear,
													   Leg1Side, Leg2Side);
	}
	else if ((OriginalSymbolLength == 13) && (OriginalSymbol[11] == L':'))  //Options Straddle
	{
		IsParsingSuccess = ParseTaifexStraddleSymbol(Market, OriginalSymbol, OriginalSide,
													 ContractId, ComposedCode,
													 Leg1Symbol, Leg2Symbol,
													 Leg1CallOrPut, Leg2CallOrPut,
													 Leg1StrikePrice, Leg2StrikePrice,
													 Leg1MaturityMonthCode, Leg1MaturityMonth,
													 Leg2MaturityMonthCode, Leg2MaturityMonth,
													 Leg1MaturityYearCode, Leg1MaturityYear,
													 Leg2MaturityYearCode, Leg2MaturityYear,
													 Leg1Side, Leg2Side);
	}
	else if ((OriginalSymbolLength == 18) && (OriginalSymbol[11] == L':'))  //Options Strangle
	{
		IsParsingSuccess = ParseTaifexStrangleSymbol(Market, OriginalSymbol, OriginalSide,
													 ContractId, ComposedCode,
													 Leg1Symbol, Leg2Symbol,
													 Leg1CallOrPut, Leg2CallOrPut,
													 Leg1StrikePrice, Leg2StrikePrice,
													 Leg1MaturityMonthCode, Leg1MaturityMonth,
													 Leg2MaturityMonthCode, Leg2MaturityMonth,
													 Leg1MaturityYearCode, Leg1MaturityYear,
													 Leg2MaturityYearCode, Leg2MaturityYear,
													 Leg1Side, Leg2Side);
	}
	else if ((OriginalSymbolLength == 13) && (OriginalSymbol[11] == L'-'))  //Options Conversion & Reversals
	{
		IsParsingSuccess = ParseTaifexConversionSymbol(Market, OriginalSymbol, OriginalSide,
													   ContractId, ComposedCode,
													   Leg1Symbol, Leg2Symbol,
													   Leg1CallOrPut, Leg2CallOrPut,
													   Leg1StrikePrice, Leg2StrikePrice,
													   Leg1MaturityMonthCode, Leg1MaturityMonth,
													   Leg2MaturityMonthCode, Leg2MaturityMonth,
													   Leg1MaturityYearCode, Leg1MaturityYear,
													   Leg2MaturityYearCode, Leg2MaturityYear,
													   Leg1Side, Leg2Side);
	}
	else if ((OriginalSymbolLength == 8) && (OriginalSymbol[6] == L'/'))  //Futures Composed
	{
		IsParsingSuccess = ParseTaifexFuturesComposedSymbol(Market, OriginalSymbol, OriginalSide,
															ContractId, ComposedCode,
															Leg1Symbol, Leg2Symbol,
															Leg1CallOrPut, Leg2CallOrPut,
															Leg1StrikePrice, Leg2StrikePrice,
															Leg1MaturityMonthCode, Leg1MaturityMonth,
															Leg2MaturityMonthCode, Leg2MaturityMonth,
															Leg1MaturityYearCode, Leg1MaturityYear,
															Leg2MaturityYearCode, Leg2MaturityYear,
															Leg1Side, Leg2Side);
	}
	else
	{
		IsParsingSuccess = ParseTaifexSingleSymbol(Market, OriginalSymbol, ContractId,
												   Leg1CallOrPut, Leg1StrikePrice,
												   Leg1MaturityMonthCode, Leg1MaturityMonth,
												   Leg1MaturityYearCode, Leg1MaturityYear);
		if (IsParsingSuccess)
		{
			ComposedCode = 0;
			Leg1Symbol = OriginalSymbol;
			Leg1Side = OriginalSide;
			Leg2Symbol = L"";
			Leg2CallOrPut = nsOrderMessageDefine::evtNone;
			Leg2StrikePrice = 0;
			Leg2MaturityMonthCode = 0;
			Leg2MaturityMonth = 0;
			Leg2MaturityYearCode = L"";
			Leg2MaturityYear = 0;
			Leg2Side = nsOrderMessageDefine::sNone;
		}  //if (IsParsingSuccess)
	}  //if ((OriginalSymbolLength == 16) && (OriginalSymbol[9] == L'/'))

	return IsParsingSuccess;
}  //ParseTaifexSymbol()

//---------------------------------------------------------------------------
TExchangeSymbol::TExchangeSymbol()
:FMarket(nsOrderMessageDefine::mTWFutures),
 FExchangeId(L""),
 FOriginalSymbol(L""),
 FSide(nsOrderMessageDefine::sNone),
 FContractId(L""),
 FComposedCode(0),
 FIsMultileg(false),
 FIsCorrectCommodity(false)
{
}  //TExchangeSymbol::TExchangeSymbol()

//---------------------------------------------------------------------------
TExchangeSymbol::TExchangeSymbol(const UnicodeString& OriginalSymbol)
:FMarket(nsOrderMessageDefine::mTWFutures),
 FExchangeId(L""),
 FOriginalSymbol(OriginalSymbol),
 FSide(nsOrderMessageDefine::sNone),
 FContractId(L""),
 FComposedCode(0),
 FIsMultileg(false),
 FIsCorrectCommodity(false)
{
	int OriginalSymbolLength = OriginalSymbol.Length();
	if ((OriginalSymbolLength == 18) || (OriginalSymbolLength == 16) ||
		(OriginalSymbolLength == 13) || (OriginalSymbolLength == 10))
		FMarket = nsOrderMessageDefine::mTWOptions;
	else if ((OriginalSymbolLength == 5) || (OriginalSymbolLength == 8))
		FMarket = nsOrderMessageDefine::mTWFutures;
	else
		FMarket = nsOrderMessageDefine::mTWFutures;

	ParseSymbol();
}  //TExchangeSymbol::TExchangeSymbol()

//---------------------------------------------------------------------------
TExchangeSymbol::TExchangeSymbol(nsOrderMessageDefine::MarketEnum Market,
								 const UnicodeString& ExchangeId,
								 const UnicodeString& OriginalSymbol,
								 nsOrderMessageDefine::SideEnum Side)
:FMarket(Market),
 FExchangeId(ExchangeId),
 FOriginalSymbol(OriginalSymbol),
 FSide(Side),
 FContractId(L""),
 FComposedCode(0),
 FIsMultileg(false),
 FIsCorrectCommodity(false)
{
	ParseSymbol();
}  //TExchangeSymbol::TExchangeSymbol()

//---------------------------------------------------------------------------
bool TExchangeSymbol::ParseSymbol()
{
	FIsCorrectCommodity = false;
	if ((FMarket == nsOrderMessageDefine::mTWFutures) ||
		(FMarket == nsOrderMessageDefine::mTWOptions))
	{
		FIsCorrectCommodity = ParseTaifexSymbol(FMarket, FOriginalSymbol, FSide, FContractId, FComposedCode,
												FLeg1Symbol, FLeg2Symbol,
												FLeg1CallPut, FLeg2CallPut,
												FLeg1StrikePrice, FLeg2StrikePrice,
												FLeg1MaturityMonthCode, FLeg1MaturityMonth,
												FLeg2MaturityMonthCode, FLeg2MaturityMonth,
												FLeg1MaturityYearCode, FLeg1MaturityYear,
												FLeg2MaturityYearCode, FLeg2MaturityYear,
												FLeg1Side, FLeg2Side);
		if (FIsCorrectCommodity)
		{
			if ((FComposedCode == L'/') || (FComposedCode == L':') || (FComposedCode == L'-'))
				FIsMultileg = true;
			else
				FIsMultileg = false;

			if (FLeg1StrikePrice > 0)
				FMarket == nsOrderMessageDefine::mTWOptions;
			else
				FMarket == nsOrderMessageDefine::mTWFutures;
		}  //if (FIsCorrectCommodity)
	}
	else if ((FMarket == nsOrderMessageDefine::mForeignFutures) ||
			 (FMarket == nsOrderMessageDefine::mForeignOptions))
	{
		FIsCorrectCommodity = true;
		FContractId = L"";
		FComposedCode = 0;
		FIsMultileg = false;

		FLeg1Symbol = FOriginalSymbol;
		FLeg1CallPut = nsOrderMessageDefine::evtNone;
		FLeg1StrikePrice = 0;
		FLeg1Side = FSide;

		int SymbolLength = FLeg1Symbol.Length();
		if (SymbolLength > 2)
		{
			FLeg1MaturityYearCode = FLeg1Symbol.SubString(SymbolLength, 1);
			FLeg1MaturityYear = GetYearByTaifexYearCode(FLeg1MaturityYearCode);
			FLeg1MaturityMonthCode = FLeg1Symbol[SymbolLength - 1];
			FLeg1MaturityMonth = GetMonthByTaifexMonthCode(FLeg1MaturityMonthCode);
		}
		else
		{
			FLeg1MaturityYearCode = L"";
			FLeg1MaturityYear = 0;
			FLeg1MaturityMonthCode = 0;
			FLeg1MaturityMonth = 0;
		}

		FLeg2Symbol = L"";
		FLeg2CallPut = nsOrderMessageDefine::evtNone;
		FLeg2StrikePrice = 0;
		FLeg2MaturityYearCode = L"";
		FLeg2MaturityYear = 0;
		FLeg2MaturityMonthCode = 0;
		FLeg2MaturityMonth = 0;
		FLeg2Side = nsOrderMessageDefine::sNone;
	}
	else
	{
		FIsCorrectCommodity = true;
		FContractId = L"";
		FComposedCode = 0;
		FIsMultileg = false;

		FLeg1Symbol = FOriginalSymbol;
		FLeg1CallPut = nsOrderMessageDefine::evtNone;
		FLeg1StrikePrice = 0;
		FLeg1MaturityYearCode = L"";
		FLeg1MaturityYear = 0;
		FLeg1MaturityMonthCode = 0;
		FLeg1MaturityMonth = 0;
		FLeg1Side = FSide;

		FLeg2Symbol = L"";
		FLeg2CallPut = nsOrderMessageDefine::evtNone;
		FLeg2StrikePrice = 0;
		FLeg2MaturityYearCode = L"";
		FLeg2MaturityYear = 0;
		FLeg2MaturityMonthCode = 0;
		FLeg2MaturityMonth = 0;
		FLeg2Side = nsOrderMessageDefine::sNone;
	}

	return FIsCorrectCommodity;
}  //TExchangeSymbol::ParseSymbol()

//---------------------------------------------------------------------------
UnicodeString TExchangeSymbol::GetSymbol(int LegNo)
{
	if ((LegNo == 1) || (!FIsMultileg && (LegNo == 2)))
		return FLeg1Symbol;
	else if ((LegNo == 2) && FIsMultileg)
		return FLeg2Symbol;
	else
		return FOriginalSymbol;
}  //TExchangeSymbol::GetSymbol()

//---------------------------------------------------------------------------
int TExchangeSymbol::GetMaturityYear(int LegNo)
{
	if ((LegNo == 2) && FIsMultileg)
		return FLeg2MaturityYear;
	else
		return FLeg1MaturityYear;
}  //TExchangeSymbol::GetMaturityYear()

//---------------------------------------------------------------------------
int TExchangeSymbol::GetMaturityMonth(int LegNo)
{
	if ((LegNo == 2) && FIsMultileg)
		return FLeg2MaturityMonth;
	else
		return FLeg1MaturityMonth;
}  //TExchangeSymbol::GetMaturityMonth()

//---------------------------------------------------------------------------
UnicodeString TExchangeSymbol::GetMaturityYearMonth(int LegNo)
{
	UnicodeString MaturityYearMonth;
	if ((LegNo == 2) && FIsMultileg)
		MaturityYearMonth.printf(L"%04d%02d", FLeg2MaturityYear, FLeg2MaturityMonth);
	else
		MaturityYearMonth.printf(L"%04d%02d", FLeg1MaturityYear, FLeg1MaturityMonth);

	return MaturityYearMonth;
}  //TExchangeSymbol::GetMaturityYearMonth()

//---------------------------------------------------------------------------
nsOrderMessageDefine::EventTypeEnum TExchangeSymbol::GetCallPut(int LegNo)
{
	if ((LegNo == 2) && FIsMultileg)
		return FLeg2CallPut;
	else
		return FLeg1CallPut;
}  //TExchangeSymbol::GetCallPut()

//---------------------------------------------------------------------------
char TExchangeSymbol::GetCallPutCode(int LegNo)
{
	nsOrderMessageDefine::EventTypeEnum CPValue = GetCallPut(LegNo);
	if (CPValue == nsOrderMessageDefine::evtCall)
		return 'C';
	else if (CPValue == nsOrderMessageDefine::evtPut)
		return 'P';
	else
		return 'N';
}  //TExchangeSymbol::GetCallPutCode()

//---------------------------------------------------------------------------
int TExchangeSymbol::GetStrikePx(int LegNo)
{
	if ((LegNo == 2) && FIsMultileg)
		return FLeg2StrikePrice;
	else
		return FLeg1StrikePrice;
}  //TExchangeSymbol::GetStrikePx()

//---------------------------------------------------------------------------
nsOrderMessageDefine::SideEnum TExchangeSymbol::GetSide(int LegNo)
{
	if ((LegNo == 1) || (!FIsMultileg && (LegNo == 2)))
		return FLeg1Side;
	else if ((LegNo == 2) && FIsMultileg)
		return FLeg2Side;
	else
		return FSide;
}  //TExchangeSymbol::GetSide()

//---------------------------------------------------------------------------
bool TExchangeSymbol::SetSymbol(nsOrderMessageDefine::MarketEnum Market,
								const UnicodeString& ExchangeId,
								const UnicodeString& OriginalSymbol,
								nsOrderMessageDefine::SideEnum Side)
{
	FMarket = Market;
	FExchangeId = ExchangeId;
	FOriginalSymbol = OriginalSymbol;
	FSide = Side;
	FContractId = L"";
	FComposedCode = 0;
	FIsMultileg = false;
	FIsCorrectCommodity = false;
	return ParseSymbol();
}  //TExchangeSymbol::SetSymbol()

