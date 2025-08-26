//---------------------------------------------------------------------------
#pragma hdrstop
#include <cmath>
#include <dateutils.hpp>
#include "IdHTTP.hpp"
#include "IdSSL.hpp"
#include "IdSSLOpenSSL.hpp"
#include "TradingObjects.h"
#include "TradingCommodities.h"
#include "MDComponentStrings.hpp"
#include "UFC.h"
#include "iniFile.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
String toDescribeTMPExecType( nsOrderMessageDefine::TMPExecTypeEnum TMPExecType )
{
	String text, text1, text2;
	switch( TMPExecType )
	{
		case nsOrderMessageDefine::tetNew:          text = Mdcomponentstrings_MD_EXECUTION_NEW;         break; //'0'
		case nsOrderMessageDefine::tetCanceled:     text = Mdcomponentstrings_MD_EXECUTION_CANCEL;      break; //'4'
		case nsOrderMessageDefine::tetReplaced:     text = Mdcomponentstrings_MD_EXECUTION_REPLACE_QTY; break; //'5'
		case nsOrderMessageDefine::tetFilled:       text = Mdcomponentstrings_MD_EXECUTION_FILL;        break; //'F'
		case nsOrderMessageDefine::tetPxReplaced:   text = Mdcomponentstrings_MD_EXECUTION_REPLACE_PX;  break; //'M'
		case nsOrderMessageDefine::tetPxReplaced2:  text = Mdcomponentstrings_MD_EXECUTION_REPLACE_PX;  break; //'m'
		case nsOrderMessageDefine::tetOrderStatus:  text = Mdcomponentstrings_MD_EXECUTION_STATUS;      break; //'I'
		case nsOrderMessageDefine::tetNewAndFilled:  //'6'
			text1 = Mdcomponentstrings_MD_EXECUTION_NEW;
			text2 = Mdcomponentstrings_MD_EXECUTION_FILL;
			text = text1 + L" and " + text2;
			break;
		default:                                    text = L"N/A";
	}
	return text;
}  //toDescribeTMPExecType()
//---------------------------------------------------------------------------
String toDescribeMarket( nsOrderMessageDefine::MarketEnum Market )
{
	String text;
	switch( Market )
	{
		case nsOrderMessageDefine::mTWFutures:   text = Mdcomponentstrings_MD_EXECUTION_FUTURES; break; // 0 TAIFEX Taiwan Futures
		case nsOrderMessageDefine::mTWOptions:   text = Mdcomponentstrings_MD_EXECUTION_OPTIONS; break; // 1 TAIFEX Taiwan Options
		case nsOrderMessageDefine::mTSE:         text = L"TSE";                                  break; // 2 TSEC Taiwan equity
		case nsOrderMessageDefine::mOTC:         text = L"OTC";                                  break; // 3 OTC Taiwan
		case nsOrderMessageDefine::mForeignFutures: text = L"PATS Futures";                         break; // 4 PATS Futures
		case nsOrderMessageDefine::mForeignOptions: text = L"PATS Options";                         break; // 5 PATS Options
		case nsOrderMessageDefine::mCNFutures:   text = L"China Futures";                        break; // 7 SGCN China Futures
		default:                                 text = L"N/A";
	}
	return text;
}  //toDescribeMarket()
//---------------------------------------------------------------------------
String toDescribeMessageType( nsOrderMessageDefine::MessageTypeEnum MessageType )
{
	String text;
	switch( MessageType )
	{
//		case nsOrderMessageDefine::mtNone:               text = L"None";               break; // 0
		case nsOrderMessageDefine::mtNew:                text = L"New";                break; // 1
		case nsOrderMessageDefine::mtCancel:             text = L"Cancel";             break; // 2
		case nsOrderMessageDefine::mtReplace:            text = L"Replace";            break; // 3
		case nsOrderMessageDefine::mtQuote:              text = L"Quote";              break; // 4
		case nsOrderMessageDefine::mtQuoteCancel:        text = L"QuoteCancel";        break; // 5
		case nsOrderMessageDefine::mtQuoteRequest:       text = L"QuoteRequest";       break; // 6
		case nsOrderMessageDefine::mtOrderStatusRequest: text = L"OrderStatusRequest"; break; // 7
		case nsOrderMessageDefine::mtExecutionReport:    text = L"ExecutionReport";    break; // 8
		case nsOrderMessageDefine::mtNews:               text = L"News";               break; // 9
		default:                                         text = L"N/A";
	}
	return text;
}  //toDescribeMessageType()
//---------------------------------------------------------------------------
String toDescribeOrderType( nsOrderMessageDefine::OrderTypeEnum OrderType )
{
	String text;
	switch( OrderType )
	{
		case nsOrderMessageDefine::otNone:      text = L"None";                                               break; // 0
		case nsOrderMessageDefine::otMarket:    text = Mdcomponentstrings_MD_EXECUTION_ORDER_TYPE_MARKET;     break; // 1
		case nsOrderMessageDefine::otLimit:     text = Mdcomponentstrings_MD_EXECUTION_ORDER_TYPE_LIMIT;      break; // 2
		case nsOrderMessageDefine::otStop:      text = Mdcomponentstrings_MD_EXECUTION_ORDER_TYPE_STOP;       break; // 3
		case nsOrderMessageDefine::otStopLimit: text = Mdcomponentstrings_MD_EXECUTION_ORDER_TYPE_STOP_LIMIT; break; // 4
		default:                                text = L"N/A";
	}
	return text;
}  //toDescribeOrderType()
//---------------------------------------------------------------------------
String toDescribeSide( nsOrderMessageDefine::SideEnum Side )
{
	String text;
	switch(Side)
	{
		case nsOrderMessageDefine::sNone: text = L"None";                              break; // 0
		case nsOrderMessageDefine::sBuy:  text = Mdcomponentstrings_MD_EXECUTION_BUY;  break; // 1
		case nsOrderMessageDefine::sSell: text = Mdcomponentstrings_MD_EXECUTION_SELL; break; // 2
		default:                          text = L"N/A";
	}
	return text;
}  //toDescribeSide()
//---------------------------------------------------------------------------
String toDescribeTimeInForce( nsOrderMessageDefine::TimeInForceEnum TimeInForce )
{
	String text;
	switch( TimeInForce )
	{
		case nsOrderMessageDefine::tifNone: text = L"None"; break; // 0
		case nsOrderMessageDefine::tifROD:  text = L"ROD";  break; // 1
		case nsOrderMessageDefine::tifIOC:  text = L"IOC";  break; // 2
		case nsOrderMessageDefine::tifFOK:  text = L"FOK";  break; // 3
		case nsOrderMessageDefine::tifTFXQ: text = L"TFXQ"; break; // 8
		default:                            text = L"N/A";
	}
	return text;
}  //toDescribeTimeInForce()
//---------------------------------------------------------------------------
String toDescribeEventType( nsOrderMessageDefine::EventTypeEnum EventType )
{
	String text;
	switch( EventType )
	{
		case nsOrderMessageDefine::evtNone: text = L"None"; break; // 0
		case nsOrderMessageDefine::evtCall: text = L"Call"; break; // 1
		case nsOrderMessageDefine::evtPut:  text = L"Put";  break; // 2
		default:                            text = L"N/A";
	}
	return text;
}  //toDescribeEventType()
//---------------------------------------------------------------------------
String toDescribeExecTransType( nsOrderMessageDefine::ExecTransTypeEnum ExecTransType )
{
	String text;
	switch( ExecTransType )
	{
		case nsOrderMessageDefine::ettNone:    text = L"None";    break; // 0
		case nsOrderMessageDefine::ettNew:     text = L"New";     break; // 1
		case nsOrderMessageDefine::ettCancel:  text = L"Cancel";  break; // 2
		case nsOrderMessageDefine::ettCorrect: text = L"Correct"; break; // 3
		case nsOrderMessageDefine::ettStatus:  text = L"Status";  break; // 4
		default:                               text = L"N/A";
	}
	return text;
}  //toDescribeExecTransType()
//---------------------------------------------------------------------------
String toDescribeExecType( nsOrderMessageDefine::ExecTypeEnum ExecType )
{
	String text;
	switch( ExecType )
	{
		case nsOrderMessageDefine::etNone:               text = L"None";               break; //  0
		case nsOrderMessageDefine::etPendingNew:         text = L"PendingNew";         break; //  1
		case nsOrderMessageDefine::etNew:                text = L"New";                break; //  2
		case nsOrderMessageDefine::etPendingReplace:     text = L"PendingReplace";     break; //  3
		case nsOrderMessageDefine::etReplaced:           text = L"Replaced";           break; //  4
		case nsOrderMessageDefine::etPendingCancel:      text = L"PendingCancel";      break; //  5
		case nsOrderMessageDefine::etCanceled:           text = L"Canceled";           break; //  6
		case nsOrderMessageDefine::etPartiallyFilled:    text = L"PartiallyFilled";    break; //  7
		case nsOrderMessageDefine::etFilled:             text = L"Filled";             break; //  8
		case nsOrderMessageDefine::etQuoteAccept:        text = L"QuoteAccept";        break; //  9
		case nsOrderMessageDefine::etExpired:            text = L"Expired";            break; // 10
		case nsOrderMessageDefine::etRejected:           text = L"Rejected";           break; // 11
		case nsOrderMessageDefine::etOrderStatus:        text = L"OrderStatus";        break; // 12
		case nsOrderMessageDefine::etQuoteRequestAccept: text = L"QuoteRequestAccept"; break; // 13
		default:                                         text = L"N/A";
	}
	return text;
}  //toDescribeExecType()
//---------------------------------------------------------------------------
String toDescribeOrderStatus( nsOrderMessageDefine::OrderStatusEnum OrderStatus,
							  nsOrderMessageDefine::TMPExecTypeEnum TMPExecType )
{
	String text;
	switch( OrderStatus )
	{
		case nsOrderMessageDefine::osNone:               text = L"None";                                 break; //  0
		case nsOrderMessageDefine::osPendingNew:         text = Mdcomponentstrings_MD_EXECUTION_PENDING; break; //  1
		case nsOrderMessageDefine::osNew:                text = Mdcomponentstrings_MD_EXECUTION_NEW;     break; //  2
		case nsOrderMessageDefine::osPendingReplace:     text = Mdcomponentstrings_MD_EXECUTION_PENDING; break; //  3
		case nsOrderMessageDefine::osReplaced:                                                                  //  4
			if ( TMPExecType == nsOrderMessageDefine::tetReplaced )
				text = Mdcomponentstrings_MD_EXECUTION_REPLACE_QTY;
			else if ( ( TMPExecType == nsOrderMessageDefine::tetPxReplaced ) ||
					  ( TMPExecType == nsOrderMessageDefine::tetPxReplaced2 ) )
				text = Mdcomponentstrings_MD_EXECUTION_REPLACE_PX;
			else
				text = L"Replaced";
			break;
		case nsOrderMessageDefine::osPendingCancel:      text = Mdcomponentstrings_MD_EXECUTION_PENDING; break; //  5
		case nsOrderMessageDefine::osCanceled:           text = Mdcomponentstrings_MD_EXECUTION_CANCEL;  break; //  6
		case nsOrderMessageDefine::osPartiallyFilled:                                                           //  7
		case nsOrderMessageDefine::osFilled:             text = Mdcomponentstrings_MD_EXECUTION_FILL;    break; //  8
		case nsOrderMessageDefine::osQuoteAccept:        text = L"QuoteAccept";                          break; //  9
		case nsOrderMessageDefine::osExpired:            text = L"Expired";                              break; // 10
		case nsOrderMessageDefine::osRejected:           text = Mdcomponentstrings_MD_EXECUTION_REJECT;  break; // 11
		case nsOrderMessageDefine::osQuoteRequestAccept: text = L"QuoteRequestAccept";                   break; // 12
		default:                                         text = L"N/A";
	}
	return text;
}  //toDescribeOrderStatus()
//---------------------------------------------------------------------------
extern String toDescribeCxlRejResponseTo( nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo )
{
	String text;
	switch( CxlRejResponseTo )
	{
		case nsOrderMessageDefine::crrNone:         text = L"None";         break; // 0
		case nsOrderMessageDefine::crrNew:          text = L"New";          break; // 1
		case nsOrderMessageDefine::crrReplace:      text = L"Replace";      break; // 2
		case nsOrderMessageDefine::crrCancel:       text = L"Cancel";       break; // 3
		case nsOrderMessageDefine::crrQuote:        text = L"Quote";        break; // 4
		case nsOrderMessageDefine::crrQuoteCancel:  text = L"QuoteCancel";  break; // 5
		case nsOrderMessageDefine::crrOrderStatus:  text = L"OrderStatus";  break; // 6
		case nsOrderMessageDefine::crrQuoteRequest: text = L"QuoteRequest"; break; // 7
		default:                                    text = L"N/A";
	}
	return text;
}  //toDescribeCxlRejResponseTo()
//---------------------------------------------------------------------------
String toDescribeOrdRejReason( nsOrderMessageDefine::OrdRejReasonEnum OrdRejReason )
{
	String text;
	switch( OrdRejReason )
	{
		case nsOrderMessageDefine::orrNone:                               text = L"None";                               break; // 0
		case nsOrderMessageDefine::orrBrokerExchangeOption:               text = L"BrokerExchangeOption";               break; // 1,
		case nsOrderMessageDefine::orrUnknownSymbol:                      text = L"UnknownSymbol";                      break; // 2,
		case nsOrderMessageDefine::orrExchangeClosed:                     text = L"ExchangeClosed";                     break; // 3,
		case nsOrderMessageDefine::orrOrderExceedsLimit:                  text = L"OrderExceedsLimit";                  break; // 4,
		case nsOrderMessageDefine::orrTooLateToEnter:                     text = L"TooLateToEnter";                     break; // 5,
		case nsOrderMessageDefine::orrUnknownOrder:                       text = L"UnknownOrder";                       break; // 6,
		case nsOrderMessageDefine::orrDuplicateOrder:                     text = L"DuplicateOrder";                     break; // 7,
		case nsOrderMessageDefine::orrDuplicateVerballyCommunicatedOrder: text = L"DuplicateVerballyCommunicatedOrder"; break; // 8,
		case nsOrderMessageDefine::orrStaleOrder:                         text = L"StaleOrder";                         break; // 9,
		case nsOrderMessageDefine::orrTradeAlongRequired:                 text = L"TradeAlongRequired";                 break; // 10,
		case nsOrderMessageDefine::orrInvalidInvestorID:                  text = L"InvalidInvestorID";                  break; // 11,
		case nsOrderMessageDefine::orrUnsupportedOrderCharacteristic:     text = L"UnsupportedOrderCharacteristic";     break; // 12,
		case nsOrderMessageDefine::orrSurveillenceOption:                 text = L"SurveillenceOption";                 break; // 13,
		case nsOrderMessageDefine::orrIncorrectQuantity:                  text = L"IncorrectQuantity";                  break; // 14,
		case nsOrderMessageDefine::orrIncorrectAllocatedQuantity:         text = L"IncorrectAllocatedQuantity";         break; // 15,
		case nsOrderMessageDefine::orrUnknownAccount:                     text = L"UnknownAccount";                     break; // 16,
		case nsOrderMessageDefine::orrOther:                              text = L"Other";                              break; // 17
		default:                                                          text = L"N/A";
	}
	return text;
}  //toDescribeOrdRejReason()
//---------------------------------------------------------------------------
String toDescribePositionEffect( nsOrderMessageDefine::PositionEffectEnum PositionEffect )
{
	String text;
	switch( PositionEffect )
	{
		case nsOrderMessageDefine::peOpen:           text = Mdcomponentstrings_MD_EXECUTION_POSITION_EFFECT_OPEN;         break; // 0
		case nsOrderMessageDefine::peClose:          text = Mdcomponentstrings_MD_EXECUTION_POSITION_EFFECT_CLOSE;        break; // 1
		case nsOrderMessageDefine::peDayTrade:       text = Mdcomponentstrings_MD_EXECUTION_POSITION_EFFECT_ROLLED;       break; // 2
		case nsOrderMessageDefine::peDayTradeOpen:   text = Mdcomponentstrings_MD_EXECUTION_POSITION_EFFECT_DAY_TRADE;    break; // 3
		case nsOrderMessageDefine::peAuto:           text = L"Auto";                                                      break; // 4
		case nsOrderMessageDefine::peAutoToday:      text = L"AutoToday";                                                 break; // 5
		case nsOrderMessageDefine::peTMPMarketMaker: text = Mdcomponentstrings_MD_EXECUTION_POSITION_EFFECT_MARKET_MAKER; break; // 9
		default:                                     text = L"N/A";
	}
	return text;
}  //toDescribePositionEffect()

//---------------------------------------------------------------------------
typedef std::map<String, String> toStringStringMap;
toStringStringMap toOldContractCodeIndex;
//---------------------------------------------------------------------------
String toFindTaifexContractCodeByOldCode(const String& OldTaifexContractCode)
{
	String ContractCode = L"";
	if (OldTaifexContractCode.Length() <= 0)
		return ContractCode;
	toStringStringMap::iterator it = toOldContractCodeIndex.find(OldTaifexContractCode);
	if (it != toOldContractCodeIndex.end()) ContractCode = it->second;
	return ContractCode;
}  //toFindTaifexContractCodeByOldCode()

//---------------------------------------------------------------------------
bool toInsertOldTaifexContractCodeIndex(const String& OldTaifexContractCode,
										const String& NewTaifexContractCode,
										bool DoCheck)
{
	bool IsSuccess = false;
	if ((OldTaifexContractCode.Length() <= 0) || (NewTaifexContractCode.Length() <= 0))
		return IsSuccess;

	if (DoCheck)
	{
		String ExistContractCode = toFindTaifexContractCodeByOldCode(OldTaifexContractCode);
		if (ExistContractCode.Length() > 0) return IsSuccess;
	}

	std::pair<toStringStringMap::iterator, bool> InsertPair;
	InsertPair = toOldContractCodeIndex.insert(toStringStringMap::value_type(OldTaifexContractCode, NewTaifexContractCode));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //toInsertOldTaifexContractCodeIndex()

//---------------------------------------------------------------------------
int otCheckDecimalPartLength(double RealNumber, int MaxDecimalPartLength)
{
	double TempRealNumber = std::fabs(RealNumber);
	double MultiPlier = std::pow(10.0, MaxDecimalPartLength);
	double RoundDiff = 5.0 / (MultiPlier * 10.0);
	int IntegerPart = static_cast<int>(TempRealNumber);
	double DecimalPart = TempRealNumber - static_cast<double>(IntegerPart);
	int DecimalPartLength = 0;
	int DecimalInteger = static_cast<int>((DecimalPart + RoundDiff) * MultiPlier);
	if (DecimalInteger > 0)
	{
		for (DecimalPartLength = MaxDecimalPartLength; DecimalPartLength > 0; DecimalPartLength--)
		{
			int Remainder = DecimalInteger % 10;
			if (Remainder != 0) break;
			DecimalInteger = DecimalInteger / 10;
		}  //for (DecimalPartLength = MaxDecimalPartLength; DecimalPartLength > 0; DecimalPartLength--)
		if (DecimalPartLength < 0) DecimalPartLength = 0;
	}
	return DecimalPartLength;
}  //otCheckDecimalPartLength()

//---------------------------------------------------------------------------
void otCheckRealNumberLength(double RealNumber, int& IntegerPartLength, int& DecimalPartLength, int MaxDecimalPartLength)
{
	IntegerPartLength = 0;
	DecimalPartLength = 0;
	double TempRealNumber = std::fabs(RealNumber);
	double DecimalPartMultiPlier = std::pow(10.0, MaxDecimalPartLength);
	double DecimalPartRoundDiff = 5.0 / (DecimalPartMultiPlier * 10.0);
	int IntegerPart = static_cast<int>(TempRealNumber);
	double DecimalPart = TempRealNumber - static_cast<double>(IntegerPart);
	int DecimalInteger = static_cast<int>((DecimalPart + DecimalPartRoundDiff) * DecimalPartMultiPlier);
	int Remainder;
	if (DecimalInteger > 0)
	{
		for (DecimalPartLength = MaxDecimalPartLength; DecimalPartLength > 0; DecimalPartLength--)
		{
			Remainder = DecimalInteger % 10;
			if (Remainder != 0) break;
			DecimalInteger = DecimalInteger / 10;
		}  //for (DecimalPartLength = MaxDecimalPartLength; DecimalPartLength > 0; DecimalPartLength--)
		if (DecimalPartLength < 0) DecimalPartLength = 0;
	}

	while (IntegerPart > 0)
	{
		IntegerPartLength++;
		IntegerPart = IntegerPart / 10;
	}  //if (IntegerPart > 0)
}  //otCheckRealNumberLength()

//----------------------------- TMixedFraction ------------------------------
//---------------------------------------------------------------------------
double TMixedFraction::CalculateRealNumber(int WholeNumber, double Numerator, int Denominator, bool IsPositive)
{
	double RealNumber = static_cast<double>(WholeNumber);
	if (Denominator > 0)
		RealNumber = RealNumber +
					 (Numerator / static_cast<double>(Denominator));
	if (!IsPositive) RealNumber = -RealNumber;
	return RealNumber;
}  //TMixedFraction::CalculateRealNumber()

//---------------------------------------------------------------------------
void TMixedFraction::ConvertRealNumber(double RealNumber, int Denominator,
									   int& WholeNumber, double& Numerator, bool& IsPositive)
{
	double TempRealNumber = RealNumber;
	if (TempRealNumber < 0.0)
	{
		IsPositive = false;
		TempRealNumber = -TempRealNumber;
	}
	else
		IsPositive = true;

	WholeNumber = static_cast<int>(TempRealNumber);
	Numerator = (TempRealNumber - static_cast<double>(WholeNumber)) * static_cast<double>(Denominator);
}  //TMixedFraction::ConvertRealNumber()

//---------------------------------------------------------------------------
int TMixedFraction::gcd(int a, int b)
{
	return b == 0 ? a : gcd(b, a % b);
}  //TMixedFraction::gcd()

//---------------------------------------------------------------------------
int TMixedFraction::lcm(int a, int b)
{
	if ((a == 0) || (b == 0)) return 0;
	return (a * b) / gcd(a, b);
}  //TMixedFraction::lcm()

//---------------------------------------------------------------------------
TMixedFraction::TMixedFraction(int WholeNumber, double Numerator, int Denominator, bool IsPositive)
:FIsPositive(IsPositive),
 FWholeNumber(FWholeNumber),
 FNumerator(Numerator),
 FDenominator(Denominator),
 FRealNumber(0.0)
{
	FRealNumber = CalculateRealNumber(FWholeNumber, FNumerator, FDenominator, IsPositive);
}  //TMixedFraction::TMixedFraction()

//---------------------------------------------------------------------------
TMixedFraction::TMixedFraction(double RealNumber, int Denominator)
:FIsPositive(true),
 FWholeNumber(0),
 FNumerator(0.0),
 FDenominator(Denominator),
 FRealNumber(RealNumber)
{
	ConvertRealNumber(FRealNumber, FDenominator, FWholeNumber, FNumerator, FIsPositive);
}  //TMixedFraction::TMixedFraction()

//---------------------------------------------------------------------------
double TMixedFraction::GetImproperNumerator()
{
	double ImproperNumerator = (FWholeNumber * static_cast<double>(FDenominator)) + FNumerator;
	if (FIsPositive)
		return ImproperNumerator;
	else
		return -ImproperNumerator;
}  //TMixedFraction::GetImproperNumerator()

//---------------------------------------------------------------------------
double TMixedFraction::ConvertImproperNumerator(double ImproperNumerator)
{
	if (ImproperNumerator >= 0.0)
		FIsPositive = true;
	else
		FIsPositive = false;
	int IntegerPart = static_cast<int>(ImproperNumerator);
	double DecimalPart = ImproperNumerator - static_cast<double>(IntegerPart);
	FNumerator = (IntegerPart % FDenominator) + DecimalPart;
	FWholeNumber = IntegerPart / FDenominator;
	FRealNumber = ImproperNumerator / static_cast<double>(FDenominator);
	return FRealNumber;
}  //TMixedFraction::ConvertImproperNumerator()

//---------------------------------------------------------------------------
double TMixedFraction::Add(int WholeNumber, double Numerator, int Denominator, bool IsPositive)
{
	int SummandWholeNumber = FWholeNumber;
	double SummandNumerator = FNumerator;
	if (!FIsPositive)
	{
		SummandWholeNumber = -SummandWholeNumber;
		SummandNumerator = -SummandNumerator;
	}

	int AddendWholeNumber = WholeNumber;
	double AddendNumerator = Numerator;
	if (!IsPositive)
	{
		AddendWholeNumber = -AddendWholeNumber;
		AddendNumerator = -AddendNumerator;
	}

	if (Denominator != FDenominator)
		FDenominator = lcm(FDenominator, Denominator);  //Least Common Multiple

	double SumNumerator = (SummandWholeNumber + AddendWholeNumber) * FDenominator +
						  (SummandNumerator + AddendNumerator);
	return ConvertImproperNumerator(SumNumerator);
}  //TMixedFraction::Add()

//---------------------------------------------------------------------------
double TMixedFraction::Add(TMixedFraction& Addend)
{
	return Add(Addend.GetWholeNumber(), Addend.GetNumerator(), Addend.GetDenominator(), Addend.IsPositive());
}  //TMixedFraction::Add()

//---------------------------------------------------------------------------
double TMixedFraction::Add(TMixedFraction *AddendPtr)
{
	if (AddendPtr == 0) return 0.0;
	return Add(AddendPtr->GetWholeNumber(), AddendPtr->GetNumerator(), AddendPtr->GetDenominator(), AddendPtr->IsPositive());
}  //TMixedFraction::Add()

//---------------------------------------------------------------------------
double TMixedFraction::Add(double RealNumber)
{
	FRealNumber += RealNumber;
	ConvertRealNumber(FRealNumber, FDenominator, FWholeNumber, FNumerator, FIsPositive);
	return FRealNumber;
}  //TMixedFraction::Add()

//---------------------------------------------------------------------------
double TMixedFraction::Multiply(double Multiplier)
{
	double ImproperNumerator = GetImproperNumerator();
	ImproperNumerator *= Multiplier;
	return ConvertImproperNumerator(ImproperNumerator);
}  //TMixedFraction::Multiply()

//---------------------------------------------------------------------------
double TMixedFraction::Divide(double Divisor)
{
	if (Divisor <= 0.000001) return FRealNumber;
	double ImproperNumerator = GetImproperNumerator();
	ImproperNumerator /= Divisor;
	return ConvertImproperNumerator(ImproperNumerator);
}  //TMixedFraction::Divide()

//------------------------------- TTransferBank --------------------------------
//---------------------------------------------------------------------------
TTransferBank::TTransferBank(const String& BankCode, const String& BankName)
:FBankCode(BankCode),
 FBankName(BankName)
{
}  //TTransferBank::TTransferBank()

//---------------------------------------------------------------------------
typedef std::map<String, TTransferBank*> toStringTransferBankMap;
toStringTransferBankMap toTransferBankCodeIndex;

//---------------------------------------------------------------------------
TTransferBank *toFindTransferBankByCode(const String& BankCode)
{
	TTransferBank *TransferBankPtr = 0;
	if (BankCode.Length() <= 0) return TransferBankPtr;
	toStringTransferBankMap::iterator it = toTransferBankCodeIndex.find(BankCode);
	if (it != toTransferBankCodeIndex.end()) TransferBankPtr = it->second;
	return TransferBankPtr;
}  //toFindTransferBankByCode()

//---------------------------------------------------------------------------
bool toInsertTransferBankCodeIndex(const String& BankCode,
								   TTransferBank *TransferBankPtr,
								   bool DoCheck)
{
	bool IsSuccess = false;
	if ((BankCode.Length() <= 0) || (TransferBankPtr == 0)) return IsSuccess;
	if (DoCheck)
	{
		TTransferBank *ExistTransferBankPtr = toFindTransferBankByCode(BankCode);
		if (ExistTransferBankPtr != 0) return IsSuccess;
	}  //if (DoCheck)

	std::pair<toStringTransferBankMap::iterator, bool> InsertPair;
	InsertPair = toTransferBankCodeIndex.insert(toStringTransferBankMap::value_type(BankCode, TransferBankPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //toInsertTransferBankCodeIndex()

//---------------------------------------------------------------------------
extern TTransferBank *toFindOrCreateTransferBank(const String& BankCode, const String& BankName, bool DoCreate)
{
	TTransferBank *transferBankPtr = toFindTransferBankByCode(BankCode);
	if ((transferBankPtr == 0) && DoCreate)
	{
		transferBankPtr = new TTransferBank(BankCode, BankName);
		if (!toInsertTransferBankCodeIndex(BankCode, transferBankPtr, false))  //Do not Check
		{
			delete transferBankPtr;
			transferBankPtr = 0;
		}
	}  //if ((transferBankPtr == 0) && DoCreate)
	return transferBankPtr;
}  //toFindOrCreateTransferBank()

//------------------------------ TBankAccount -------------------------------
//---------------------------------------------------------------------------
TBankAccount::TBankAccount(TTransferBank *TransferBankPtr, TTradingAccount *TradingAccountPtr,
						   const String& BankAccount, const String& Currency, const String& BankType)
:FTransferBankPtr(TransferBankPtr),
 FTradingAccountPtr(TradingAccountPtr),
 FBankAccount(BankAccount),
 FCurrency(Currency),
 FBankType(BankType)  //Z, T, Q
{
}  //TBankAccount::TBankAccount()

//--------------------------- TTransferMoneyCommand ----------------------------
//---------------------------------------------------------------------------
TTransferMoneyCommand::TTransferMoneyCommand(int UID, const String& TradingAccount, const String& BankCode,
											 const String& BankAccount, const String& TransferFlag,
											 int Amount)
:FUID(UID),
 FTradingAccount(TradingAccount),
 FBankCode(BankCode),
 FBankAccount(BankAccount),
 FTransferFlag(TransferFlag),
 FAmount(Amount),
 FCreateTime(L""),
 FTransferStatus(0),
 FSequenceNo(0),
 FErrorCode(L""),
 FErrorMessage(L"")
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	FCreateTime.printf(L"%04d/%02d/%02d %02d:%02d:%02d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
}  //TTransferMoneyCommand::TTransferMoneyCommand()

//----------------------------- TTransferRecord -----------------------------
//---------------------------------------------------------------------------
TTransferRecord::TTransferRecord(int SequenceNo, const String& TradingAccount,
								 const String& BankName, const String& BankAccount,
								 const String& Operator, const String& OperatingDate,
								 const String& OperatingTime, const String& OperatingType,
								 int Amount, const String& ProcessStatus,
								 const String& ProcessResult, const String& BankCode,
								 const String& BankType)
:FSequenceNo(SequenceNo),
 FTradingAccount(TradingAccount),
 FBankName(BankName),
 FBankAccount(BankAccount),
 FOperator(Operator),
 FOperatingDate(OperatingDate),
 FOperatingTime(OperatingTime),
 FOperatingType(OperatingType),
 FAmount(Amount),
 FProcessStatus(ProcessStatus),
 FProcessResult(ProcessResult),
 FBankCode(BankCode),
 FBankType(BankType)
{
}  //TransferRecord::TransferRecord()

//--------------------- TQueryBankAvailableCashCommand ----------------------
//---------------------------------------------------------------------------
TQueryBankAvailableCashCommand::TQueryBankAvailableCashCommand(int UID, const String& TradingAccount, const String& BankCode,
															   const String& Currency)
:FUID(UID),
 FTradingAccount(TradingAccount),
 FBankCode(BankCode),
 FCurrency(Currency),
 FCreateTime(L""),
 FAvailableCash(0),
 FErrorCode(L""),
 FErrorMessage(L"")
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	FCreateTime.printf(L"%04d/%02d/%02d %02d:%02d:%02d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
}  //TQueryBankAvailableCashCommand::TQueryBankAvailableCashCommand()

//------------------------ TAvailableBankCashRecord -------------------------
//---------------------------------------------------------------------------
TAvailableBankCashRecord::TAvailableBankCashRecord(const String& BankCode, const String& Currency, int AvailableCash)
:FBankCode(BankCode),
 FCurrency(Currency),
 FAvailableCash(AvailableCash),
 FUpdateTime(L"")
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	FUpdateTime.printf(L"%04d/%02d/%02d %02d:%02d:%02d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
}  //TAvailableBankCashRecord::TAvailableBankCashRecord()

//-------------------------- TAvailableFundRecord ---------------------------
//---------------------------------------------------------------------------
TAvailableFundRecord::TAvailableFundRecord(int AvailableFund, int AvailableCash, int AvailableTradingFund, int AvailableTradingCash, int CheckCredit)
:FAvailableFund(0),
 FAvailableCash(0),
 FAvailableTradingFund(0),
 FAvailableTradingCash(0),
 FCheckCredit(0),
 FErrorCode(L""),
 FErrorMessage(L""),
 FUpdateTime(L"")
{
	UpdateField(AvailableFund, AvailableCash, AvailableTradingFund, AvailableTradingCash, CheckCredit);
}  //TAvailableFundRecord::TAvailableFundRecord()
//---------------------------------------------------------------------------
void TAvailableFundRecord::UpdateField(int AvailableFund, int AvailableCash, int AvailableTradingFund, int AvailableTradingCash, int CheckCredit)
{
	FAvailableFund        = AvailableFund;
	FAvailableCash        = AvailableCash;
	FAvailableTradingFund = AvailableTradingFund;
	FAvailableTradingCash = AvailableTradingCash;
	FCheckCredit = CheckCredit;
	FErrorCode = L"";
	FErrorMessage = L"";
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	FUpdateTime.printf(L"%04d/%02d/%02d %02d:%02d:%02d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
}  //TAvailableFundRecord::UpdateField()
//---------------------------------------------------------------------------
void TAvailableFundRecord::UpdateField(const String& ErrorCode, const String& ErrorMessage)
{
	FErrorCode = ErrorCode;
	FErrorMessage = ErrorMessage;
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	FUpdateTime.printf(L"%04d/%02d/%02d %02d:%02d:%02d", SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
}  //TAvailableFundRecord::UpdateField()

//----------------------------- TTradingAccount -----------------------------
//---------------------------------------------------------------------------
TTradingAccount::TTradingAccount(const String& Account)
:FAccount(Account),
 FName(L""),
 FQryTaifexHoldOpenInterestURL(L""),
 FQryTaifexHoldOpenInterestErrMsg(L""),
 FQryOverseasHoldOpenInterestURL(L""),
 FQryOverseasHoldOpenInterestErrMsg(L""),
 FQryTransRecErrCode(L""),
 FQryTransRecErrMsg(L""),
 FRecvTransRecTime(L""),
 FAvailableFundRecordPtr(0)
{
}  //TTradingAccount::TTradingAccount()

//---------------------------------------------------------------------------
TTradingAccount::TTradingAccount(const String& Account, const String& Name)
:FAccount(Account),
 FName(Name),
 FQryTaifexHoldOpenInterestURL(L""),
 FQryTaifexHoldOpenInterestErrMsg(L""),
 FQryOverseasHoldOpenInterestURL(L""),
 FQryOverseasHoldOpenInterestErrMsg(L""),
 FAvailableFundRecordPtr(0)
{
}  //TTradingAccount::TTradingAccount()

//---------------------------------------------------------------------------
void TTradingAccount::ClearBankAccount()
{
	toStringBankAccountMap::iterator it;
	for (it = FBankAccountAccountIndex.begin(); it != FBankAccountAccountIndex.end(); it++)
	{
		TBankAccount *BankAccountPtr = it->second;
		delete BankAccountPtr;
	}
	FBankAccountAccountIndex.clear();
	FBankAccountArray.clear();
}  //TTradingAccount::ClearBankAccount()

//---------------------------------------------------------------------------
TBankAccount *TTradingAccount::FindBankAccountByIndexKey(const String& IndexKey)
{
	TBankAccount *BankAccountPtr = 0;
	if (IndexKey.Length() <= 0) return BankAccountPtr;
	toStringBankAccountMap::iterator it = FBankAccountAccountIndex.find(IndexKey);
	if (it != FBankAccountAccountIndex.end()) BankAccountPtr = it->second;
	return BankAccountPtr;
}  //TTradingAccount::FindBankAccountByIndexKey()
//---------------------------------------------------------------------------
TBankAccount *TTradingAccount::FindBankAccountByAccount(const String& BankCode, const String& BankAccount)
{
	TBankAccount *BankAccountPtr = 0;
	if ((BankCode.Length() <= 0) || (BankAccount.Length() <= 0)) return BankAccountPtr;
	String IndexKey = BankCode + BankAccount;
	BankAccountPtr = FindBankAccountByIndexKey(IndexKey);
	return BankAccountPtr;
}  //TTradingAccount::FindBankAccountByAccount()
//---------------------------------------------------------------------------
bool TTradingAccount::InsertBankAccountAccountIndex(const String& IndexKey,
													TBankAccount *BankAccountPtr,
													bool DoCheck)
{
	bool IsSuccess = false;
	if ((IndexKey.Length() <= 0) || (BankAccountPtr == 0)) return IsSuccess;
	if (DoCheck)
	{
		TBankAccount *ExistBankAccountPtr = FindBankAccountByIndexKey(IndexKey);
		if (ExistBankAccountPtr != 0) return IsSuccess;
	}  //if (DoCheck)

	std::pair<toStringBankAccountMap::iterator, bool> InsertPair;
	InsertPair = FBankAccountAccountIndex.insert(toStringBankAccountMap::value_type(IndexKey, BankAccountPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TTradingAccount::InsertBankAccountAccountIndex()
//---------------------------------------------------------------------------
bool TTradingAccount::InsertBankAccountAccountIndex(const String& BankCode, const String& BankAccount,
													TBankAccount *BankAccountPtr,
													bool DoCheck)
{
	bool IsSuccess = false;
	if ((BankCode.Length() <= 0) || (BankAccount.Length() <= 0) || (BankAccountPtr == 0)) return IsSuccess;
	String IndexKey = BankCode + BankAccount;
	IsSuccess = InsertBankAccountAccountIndex(IndexKey, BankAccountPtr, DoCheck);
	return IsSuccess;
}  //TTradingAccount::InsertBankAccountAccountIndex()
//---------------------------------------------------------------------------
void TTradingAccount::AddBankAccount(TBankAccount *BankAccountPtr)
{
	if (BankAccountPtr != 0)
		FBankAccountArray.push_back(BankAccountPtr);
}  //TTradingAccount::AddBankAccount()
//---------------------------------------------------------------------------
TBankAccount *TTradingAccount::GetBankAccountByIndex(int Index)
{
	TBankAccount *BankAccountPtr = 0;
	if ((Index >= 0) && (static_cast<unsigned int>(Index) < FBankAccountArray.size()))
		BankAccountPtr = FBankAccountArray[Index];
	return BankAccountPtr;
}  //TTradingAccount::GetBankAccountByIndex()
//---------------------------------------------------------------------------
void TTradingAccount::ClearTransferMoneyCmd()
{
	for (unsigned int i = 0; i < FTransferMoneyCmdArray.size(); i++)
	{
		TTransferMoneyCommand *TransferMoneyCmdPtr = FTransferMoneyCmdArray[i];
		delete TransferMoneyCmdPtr;
	}
	FTransferMoneyCmdArray.clear();
}  //TTradingAccount::ClearTransferMoneyCmd()
//---------------------------------------------------------------------------
void TTradingAccount::AddTransferMoneyCmd(TTransferMoneyCommand *TransferMoneyCmdPtr)
{
	if (TransferMoneyCmdPtr != 0)
		FTransferMoneyCmdArray.push_back(TransferMoneyCmdPtr);
}  //TTradingAccount::AddTransferMoneyCmd()
//---------------------------------------------------------------------------
TTransferMoneyCommand *TTradingAccount::GetTransferMoneyCmdByIndex(int Index)
{
	TTransferMoneyCommand *TransferMoneyCmdPtr = 0;
	if ((Index >= 0) && (static_cast<unsigned int>(Index) < FTransferMoneyCmdArray.size()))
		TransferMoneyCmdPtr = FTransferMoneyCmdArray[Index];
	return TransferMoneyCmdPtr;
}  //TTradingAccount::GetTransferMoneyCmdByIndex()
//---------------------------------------------------------------------------
TTransferMoneyCommand *TTradingAccount::GetTransferMoneyCmdByID(int ID)
{
	TTransferMoneyCommand *TransferMoneyCmdPtr = 0;
	for (unsigned int i = 0; i < FTransferMoneyCmdArray.size(); i++)
	{
		TTransferMoneyCommand *curTransferMoneyCmdPtr = FTransferMoneyCmdArray[i];
		if (ID == curTransferMoneyCmdPtr->GetUID())
		{
			TransferMoneyCmdPtr = curTransferMoneyCmdPtr;
			break;
		}
	}  //for (unsigned int i = 0; i < FTransferMoneyCmdArray.size(); i++)
	return TransferMoneyCmdPtr;
}  //TTradingAccount::GetTransferMoneyCmdByID()
//---------------------------------------------------------------------------
void TTradingAccount::ClearTransferRecord()
{
	for (unsigned int i = 0; i < FTransferRecordArray.size(); i++)
	{
		TTransferRecord *TransferRecordPtr = FTransferRecordArray[i];
		delete TransferRecordPtr;
	}
	FTransferRecordArray.clear();
}  //TTradingAccount::ClearTransferRecord()
//---------------------------------------------------------------------------
void TTradingAccount::AddTransferRecord(TTransferRecord *TransferRecordPtr)
{
	if (TransferRecordPtr != 0)
		FTransferRecordArray.push_back(TransferRecordPtr);
}  //TTradingAccount::AddTransferRecord()
//---------------------------------------------------------------------------
TTransferRecord *TTradingAccount::GetTransferRecordByIndex(int Index)
{
	TTransferRecord *TransferRecordPtr = 0;
	if ((Index >= 0) && (static_cast<unsigned int>(Index) < FTransferRecordArray.size()))
		TransferRecordPtr = FTransferRecordArray[Index];
	return TransferRecordPtr;
}  //TTradingAccount::GetTransferRecordByIndex()
//---------------------------------------------------------------------------
void TTradingAccount::ClearQryAvailableCashCmd()
{
	toIntQryAvailableCashCmdMap::iterator it;
	for (it = FQryAvailableCashCmdUIDIndex.begin(); it != FQryAvailableCashCmdUIDIndex.end(); it++)
	{
		TQueryBankAvailableCashCommand *qryAvailableCashCmdPtr = it->second;
		delete qryAvailableCashCmdPtr;
	}
	FQryAvailableCashCmdUIDIndex.clear();
}  //TTradingAccount::ClearQryAvailableCashCmd()
//---------------------------------------------------------------------------
TQueryBankAvailableCashCommand *TTradingAccount::FindQryAvailableCashCmdByUID(int UID)
{
	TQueryBankAvailableCashCommand *qryAvailableCashCmdPtr = 0;
	if (UID < 0) return qryAvailableCashCmdPtr;
	toIntQryAvailableCashCmdMap::iterator it = FQryAvailableCashCmdUIDIndex.find(UID);
	if (it != FQryAvailableCashCmdUIDIndex.end()) qryAvailableCashCmdPtr = it->second;
	return qryAvailableCashCmdPtr;
}  //TTradingAccount::FindQryAvailableCashCmdByUID()
//---------------------------------------------------------------------------
bool TTradingAccount::InsertQryAvailableCashCmdUIDIndex(int UID,
														TQueryBankAvailableCashCommand *QryAvailableCashCmdPtr,
														bool DoCheck)
{
	bool IsSuccess = false;
	if ((UID < 0) || (QryAvailableCashCmdPtr == 0)) return IsSuccess;
	if (DoCheck)
	{
		TQueryBankAvailableCashCommand *existcmdPtr = FindQryAvailableCashCmdByUID(UID);
		if (existcmdPtr != 0) return IsSuccess;
	}  //if (DoCheck)

	std::pair<toIntQryAvailableCashCmdMap::iterator, bool> InsertPair;
	InsertPair = FQryAvailableCashCmdUIDIndex.insert(toIntQryAvailableCashCmdMap::value_type(UID, QryAvailableCashCmdPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //TTradingAccount::InsertQryAvailableCashCmdUIDIndex()
//---------------------------------------------------------------------------
void TTradingAccount::ClearAvailableCashRec()
{
	for (unsigned int i = 0; i < FAvailableCashRecordArray.size(); i++)
	{
		TAvailableBankCashRecord *availableCashRecPtr = FAvailableCashRecordArray[i];
		delete availableCashRecPtr;
	}
	FAvailableCashRecordArray.clear();
}  //TTradingAccount::ClearAvailableCashRec()
//---------------------------------------------------------------------------
void TTradingAccount::AddAvailableCashRec(TAvailableBankCashRecord *AvailableCashRecPtr)
{
	if (AvailableCashRecPtr != 0)
		FAvailableCashRecordArray.push_back(AvailableCashRecPtr);
}  //TTradingAccount::AddAvailableCashRec()
//---------------------------------------------------------------------------
TAvailableBankCashRecord *TTradingAccount::GetAvailableCashRecByIndex(int Index)
{
	TAvailableBankCashRecord *availableCashRecPtr = 0;
	if ((Index >= 0) && (static_cast<unsigned int>(Index) < FAvailableCashRecordArray.size()))
		availableCashRecPtr = FAvailableCashRecordArray[Index];
	return availableCashRecPtr;
}  //TTradingAccount::GetAvailableCashRecByIndex()
//---------------------------------------------------------------------------
TAvailableBankCashRecord *TTradingAccount::GetAvailableCashRecByBankCode(const String& BankCode, const String& Currency)
{
	TAvailableBankCashRecord *availableCashRecPtr = 0;
	for (unsigned int i = 0; i < FAvailableCashRecordArray.size(); i++)
	{
		TAvailableBankCashRecord *curAvailableCashRecPtr = FAvailableCashRecordArray[i];
		if ((BankCode == curAvailableCashRecPtr->GetBankCode()) &&
			(Currency == curAvailableCashRecPtr->GetCurrency()))
		{
			availableCashRecPtr = curAvailableCashRecPtr;
			break;
		}
	}
	return availableCashRecPtr;
}  //TTradingAccount::GetAvailableCashRecByBankCode()
//---------------------------------------------------------------------------
void TTradingAccount::GetAvailableFund(int& AvailableFund, int& AvailableCash, int& AvailableTradingFund, int& AvailableTradingCash, int& CheckCredit, String& UpdateTime)
{
	if (FAvailableFundRecordPtr == 0)
	{
		AvailableFund = 0;
		AvailableCash = 0;
		AvailableTradingFund = 0;
		AvailableTradingCash = 0;
		UpdateTime = L"";
	}
	else
	{
		AvailableFund = FAvailableFundRecordPtr->GetAvailableFund();
		AvailableCash = FAvailableFundRecordPtr->GetAvailableCash();
		AvailableTradingFund = FAvailableFundRecordPtr->GetAvailableTradingFund();
		AvailableTradingCash = FAvailableFundRecordPtr->GetAvailableTradingCash();
		UpdateTime = FAvailableFundRecordPtr->GetUpdateTime();
	}
}  //TTradingAccount::GetAvailableFund()
//---------------------------------------------------------------------------
void TTradingAccount::GetAvailableFundError(String& ErrorCode, String& ErrorMessage, String& UpdateTime)
{
	if (FAvailableFundRecordPtr == 0)
	{
		ErrorCode = L"";
		ErrorMessage = L"";
		UpdateTime = L"";
	}
	else
	{

		ErrorCode = FAvailableFundRecordPtr->GetErrorCode();
		ErrorMessage = FAvailableFundRecordPtr->GetErrorMessage();
		UpdateTime = FAvailableFundRecordPtr->GetUpdateTime();
	}
}  //TTradingAccount::GetAvailableFundError()
//---------------------------------------------------------------------------
void TTradingAccount::UpdateAvailableFund(int AvailableFund, int AvailableCash, int AvailableTradingFund, int AvailableTradingCash, int CheckCredit)
{
	if (FAvailableFundRecordPtr == 0)
		FAvailableFundRecordPtr = new TAvailableFundRecord(AvailableFund, AvailableCash, AvailableTradingFund, AvailableTradingCash, CheckCredit);
	else
		FAvailableFundRecordPtr->UpdateField(AvailableFund, AvailableCash, AvailableTradingFund, AvailableTradingCash, CheckCredit);
}  //TTradingAccount::UpdateAvailableFund()
//---------------------------------------------------------------------------
void TTradingAccount::UpdateAvailableFundError(const String& ErrorCode, const String& ErrorMessage)
{
	if (FAvailableFundRecordPtr == 0) FAvailableFundRecordPtr = new TAvailableFundRecord(0, 0, 0, 0, 0);
	FAvailableFundRecordPtr->UpdateField(ErrorCode, ErrorMessage);
}  //TTradingAccount::UpdateAvailableFundError()

//---------------------------------------------------------------------------
typedef std::map<String, TTradingAccount*> toStringTradingAccountMap;
toStringTradingAccountMap toTradingAccountIndex;

//---------------------------------------------------------------------------
TTradingAccount *toFindTradingAccountByAccount(const String& Account)
{
	TTradingAccount *TradingAccountPtr = 0;
	if (Account.Length() <= 0) return TradingAccountPtr;
	toStringTradingAccountMap::iterator it = toTradingAccountIndex.find(Account);
	if (it != toTradingAccountIndex.end()) TradingAccountPtr = it->second;
	return TradingAccountPtr;
}  //toFindTradingAccountByAccount()

//---------------------------------------------------------------------------
bool toInsertTradingAccountIndex(const String& Account,
								 TTradingAccount *AccountPtr,
								 bool DoCheck)
{
	bool IsSuccess = false;
	if ((Account.Length() <= 0) || (AccountPtr == 0)) return IsSuccess;
	if (DoCheck)
	{
		TTradingAccount *ExistAccountPtr = toFindTradingAccountByAccount(Account);
		if (ExistAccountPtr != 0) return IsSuccess;
	}  //if (DoCheck)

	std::pair<toStringTradingAccountMap::iterator, bool> InsertPair;
	InsertPair = toTradingAccountIndex.insert(toStringTradingAccountMap::value_type(Account, AccountPtr));
	IsSuccess = InsertPair.second;
	return IsSuccess;
}  //toInsertTradingAccountIndex()

//---------------------------------------------------------------------------
extern TTradingAccount *toFindOrCreateTradingAccount(const String& Account, bool DoCreate)
{
	TTradingAccount *tradingAccountPtr = toFindTradingAccountByAccount(Account);
	if ((tradingAccountPtr == 0) && DoCreate)
	{
		tradingAccountPtr = new TTradingAccount(Account);
		if (!toInsertTradingAccountIndex(Account, tradingAccountPtr, false))  //Do not Check
		{
			delete tradingAccountPtr;
			tradingAccountPtr = 0;
		}
	}  //if ((tradingAccountPtr == 0) && DoCreate)
	return tradingAccountPtr;
}  //toFindOrCreateTradingAccount()
//---------------------------------------------------------------------------

