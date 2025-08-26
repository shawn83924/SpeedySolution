//---------------------------------------------------------------------------


#pragma hdrstop

#include "TradingUtility.h"
namespace s888
{
//---------------------------------------------------------------------------
int ConvertUnicodeToAnsi(const String& UnicodeStr, char *AnsiBuffer, int MaxAnsiLength)
{
	int ansiStrLength = 0;
	if ((AnsiBuffer == 0) || (MaxAnsiLength <= 0)) return ansiStrLength;

	int unicodeStrLength = UnicodeStr.Length();
	if (unicodeStrLength > 0)
	{
		ansiStrLength = UnicodeToUtf8(AnsiBuffer, MaxAnsiLength, UnicodeStr.c_str(), unicodeStrLength);
		AnsiBuffer[ansiStrLength] = 0;
	}  //if (unicodeStrLength > 0)
	return ansiStrLength;
}  //ConvertUnicodeToAnsi()
//---------------------------------------------------------------------------
AnsiString ConvertUnicodeToAnsiStr(const String& UnicodeStr)
{
	AnsiString ansiStr = "";
	int unicodeStrLength = UnicodeStr.Length();
	if (unicodeStrLength > 0)
	{
		int ansiBufferSize = unicodeStrLength * 4;
		char *ansiBuffer = new char[ansiBufferSize + 1];
		int ansiStrLength = ConvertUnicodeToAnsi(UnicodeStr, ansiBuffer, ansiBufferSize);
		if (ansiStrLength > 0) ansiStr = ansiBuffer;
		delete [] ansiBuffer;
	}  //if (unicodeStrLength > 0)
	return ansiStr;
}  //ConvertUnicodeToAnsiStr()
//---------------------------------------------------------------------------
UFC::AnsiString ConvertUnicodeToUFCAnsiStr(const String& UnicodeStr)
{
	UFC::AnsiString ansiStr = "";
	int unicodeStrLength = UnicodeStr.Length();
	if (unicodeStrLength > 0)
	{
		int ansiBufferSize = unicodeStrLength * 4;
		char *ansiBuffer = new char[ansiBufferSize + 1];
		int ansiStrLength = ConvertUnicodeToAnsi(UnicodeStr, ansiBuffer, ansiBufferSize);
		if (ansiStrLength > 0) ansiStr = ansiBuffer;
		delete [] ansiBuffer;
	}  //if ( unicodeStrLength > 0 )
	return ansiStr;
}  //ConvertUnicodeToUFCAnsiStr()
//---------------------------------------------------------------------------
void WriteUFCLog(const String& LogStr, bool DoFlush)
{
	UFC::AnsiString ansiLogStr = ConvertUnicodeToUFCAnsiStr(LogStr);
	UFC::BufferedLog::Printf(" %s", ansiLogStr.c_str());
	if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //WriteUFCLog()
//---------------------------------------------------------------------------
void WriteUFCLog(const String& HeadStr, const String& LogStr, bool DoFlush)
{
	UFC::AnsiString ansiHeadStr = ConvertUnicodeToUFCAnsiStr(HeadStr);
	UFC::AnsiString ansiLogStr = ConvertUnicodeToUFCAnsiStr(LogStr);
	UFC::BufferedLog::Printf(" %s %s", ansiHeadStr.c_str(), ansiLogStr.c_str());
	if (DoFlush) UFC::BufferedLog::FlushToFile();
}  //WriteUFCLog()
//---------------------------------------------------------------------------
String DescribeMarket(nsOrderMessageDefine::MarketEnum Market)
{
	String marketStr;
	switch(Market)
	{
		case nsOrderMessageDefine::mTWFutures:   marketStr = L"TWFutures";   break;  // 0 TAIFEX Taiwan Futures
		case nsOrderMessageDefine::mTWOptions:   marketStr = L"TWOptions";   break;  // 1 TAIFEX Taiwan Options
		case nsOrderMessageDefine::mTSE:         marketStr = L"TSE";         break;  // 2 TSEC   Taiwan equity
		case nsOrderMessageDefine::mOTC:         marketStr = L"OTC";         break;  // 3 OTC    Taiwan
		case nsOrderMessageDefine::mForeignFutures: marketStr = L"PATSFutures"; break;  // 4 PATS          Futures
		case nsOrderMessageDefine::mForeignOptions: marketStr = L"PATSOptions"; break;  // 5 PATS          Options
		case nsOrderMessageDefine::mCNFutures:   marketStr = L"CNFutures";   break;  // 6 China  Futures
		case nsOrderMessageDefine::mCNOptions:   marketStr = L"CNOptions";   break;  // 7 China  Options
		default:                                 marketStr = L"N/A";         break;
	}  //switch(Market)
	return marketStr;
}  //DescribeMarket()
//---------------------------------------------------------------------------
String DescribeCallPut(nsOrderMessageDefine::EventTypeEnum CallPut)
{
	String callPutStr;
	switch(CallPut)
	{
		case nsOrderMessageDefine::evtNone: callPutStr = L"None"; break;  // 0
		case nsOrderMessageDefine::evtCall: callPutStr = L"Call"; break;  // 1
		case nsOrderMessageDefine::evtPut:  callPutStr = L"Put";  break;  // 2
		default:                            callPutStr = L"N/A";  break;
	}  //switch(CallPut)
	return callPutStr;
}  //DescribeCallPut()
//---------------------------------------------------------------------------
String DescribeBuySell(nsOrderMessageDefine::SideEnum BuySell)
{
	String buySellStr;
	switch(BuySell)
	{
		case nsOrderMessageDefine::sNone: buySellStr = L"None"; break;  // 0
		case nsOrderMessageDefine::sBuy:  buySellStr = L"Buy";  break;  // 1
		case nsOrderMessageDefine::sSell: buySellStr = L"Sell"; break;  // 2
		default:                          buySellStr = L"N/A";  break;
	}  //switch(BuySell)
	return buySellStr;
}  //DescribeBuySell()
//---------------------------------------------------------------------------
String DescribeOrderType(nsOrderMessageDefine::OrderTypeEnum OrderType)
{
	String orderTypeStr;
	switch(OrderType)
	{
		case nsOrderMessageDefine::otNone:                 orderTypeStr = L"None";                   break;  // 0
		case nsOrderMessageDefine::otMarket:               orderTypeStr = L"Market";                 break;  // 1
		case nsOrderMessageDefine::otLimit:                orderTypeStr = L"Limit";                  break;  // 2
		case nsOrderMessageDefine::otMarketWithProtection: orderTypeStr = L"Market With Protection"; break;  // 3
		case nsOrderMessageDefine::otStop:                 orderTypeStr = L"Stop";                   break;  // 4
		case nsOrderMessageDefine::otStopLimit:            orderTypeStr = L"Stop Limit";             break;  // 5
		default:                                           orderTypeStr = L"N/A";                    break;
	}  //switch(OrderType)
	return orderTypeStr;
}  //DescribeOrderType()
//---------------------------------------------------------------------------
String DescribeTimeInForce(nsOrderMessageDefine::TimeInForceEnum TimeInForce)
{
	String timeInForceStr;
	switch(TimeInForce)
	{
		case nsOrderMessageDefine::tifNone: timeInForceStr = L"None"; break;  // 0
		case nsOrderMessageDefine::tifROD:  timeInForceStr = L"ROD";  break;  // 1
		case nsOrderMessageDefine::tifIOC:  timeInForceStr = L"IOC";  break;  // 2
		case nsOrderMessageDefine::tifFOK:  timeInForceStr = L"FOK";  break;  // 3
		case nsOrderMessageDefine::tifTFXQ: timeInForceStr = L"TFXQ"; break;  // 8
		default:                            timeInForceStr = L"N/A";  break;
	}  //switch(TimeInForce)
	return timeInForceStr;
}  //DescribeTimeInForce()
//---------------------------------------------------------------------------
String DescribePositionEffect(nsOrderMessageDefine::PositionEffectEnum PositionEffect)
{
	String positionEffectStr;
	switch(PositionEffect)
	{
		case nsOrderMessageDefine::peOpen:           positionEffectStr = L"Open";           break; // 0
		case nsOrderMessageDefine::peClose:          positionEffectStr = L"Close";          break; // 1
		case nsOrderMessageDefine::peDayTrade:       positionEffectStr = L"DayTrade";       break; // 2
		case nsOrderMessageDefine::peDayTradeOpen:   positionEffectStr = L"DayTradeOpen";   break; // 3
		case nsOrderMessageDefine::peAuto:           positionEffectStr = L"Auto";           break; // 4
		case nsOrderMessageDefine::peAutoToday:      positionEffectStr = L"AutoToday";      break; // 5
		case nsOrderMessageDefine::peTMPMarketMaker: positionEffectStr = L"TMPMarketMaker"; break; // 9
		default:                                     positionEffectStr = L"N/A";
	}
	return positionEffectStr;
}  //DescribePositionEffect()
//---------------------------------------------------------------------------
String DescribeExecType(nsOrderMessageDefine::ExecTypeEnum ExecType)
{
	String execTypeStr;
	switch(ExecType)
	{
		case nsOrderMessageDefine::etNone:               execTypeStr = L"None";               break; // 0
		case nsOrderMessageDefine::etPendingNew:         execTypeStr = L"PendingNew";         break; // 1
		case nsOrderMessageDefine::etNew:                execTypeStr = L"New";                break; // 2
		case nsOrderMessageDefine::etPendingReplace:     execTypeStr = L"PendingReplace";     break; // 3
		case nsOrderMessageDefine::etReplaced:           execTypeStr = L"Replaced";           break; // 4
		case nsOrderMessageDefine::etPendingCancel:      execTypeStr = L"PendingCancel";      break; // 5
		case nsOrderMessageDefine::etCanceled:           execTypeStr = L"Canceled";           break; // 6
		case nsOrderMessageDefine::etPartiallyFilled:    execTypeStr = L"PartiallyFilled";    break; // 7
		case nsOrderMessageDefine::etFilled:             execTypeStr = L"Filled";             break; // 8
		case nsOrderMessageDefine::etQuoteAccept:        execTypeStr = L"QuoteAccept";        break; // 9
		case nsOrderMessageDefine::etExpired:            execTypeStr = L"Expired";            break; //10
		case nsOrderMessageDefine::etRejected:           execTypeStr = L"Rejected";           break; //11
		case nsOrderMessageDefine::etOrderStatus:        execTypeStr = L"OrderStatus";        break; //12
		case nsOrderMessageDefine::etQuoteRequestAccept: execTypeStr = L"QuoteRequestAccept"; break; //13
		default:                                         execTypeStr = L"N/A";
	}
	return execTypeStr;
}  //DescribeExecType()
//---------------------------------------------------------------------------
String DescribeTMPExecType(nsOrderMessageDefine::TMPExecTypeEnum TMPExecType)
{
	String tmpExecTypeStr;
	switch(TMPExecType)
	{
		case nsOrderMessageDefine::tetNew:          tmpExecTypeStr = L"None";            break; // '0'
		case nsOrderMessageDefine::tetCanceled:     tmpExecTypeStr = L"PendingNew";      break; // '4'
		case nsOrderMessageDefine::tetReplaced:     tmpExecTypeStr = L"New";             break; // '5'
		case nsOrderMessageDefine::tetFilled:       tmpExecTypeStr = L"PendingReplace";  break; // 'F'
		case nsOrderMessageDefine::tetPxReplaced:   tmpExecTypeStr = L"Replaced";        break; // 'M'
		case nsOrderMessageDefine::tetPxReplaced2:  tmpExecTypeStr = L"PendingCancel";   break; // 'm'
		case nsOrderMessageDefine::tetOrderStatus:  tmpExecTypeStr = L"Canceled";        break; // 'I'
		case nsOrderMessageDefine::tetNewAndFilled: tmpExecTypeStr = L"PartiallyFilled"; break; // '6'
		default:                                    tmpExecTypeStr = L"N/A";
	}
	return tmpExecTypeStr;
}  //DescribeTMPExecType()
//---------------------------------------------------------------------------
String DescribeOrderStatus(nsOrderMessageDefine::OrderStatusEnum OrderStatus)
{
	String orderStatusStr;
	switch(OrderStatus)
	{
		case nsOrderMessageDefine::osNone:               orderStatusStr = L"None";               break; // 0
		case nsOrderMessageDefine::osPendingNew:         orderStatusStr = L"PendingNew";         break; // 1
		case nsOrderMessageDefine::osNew:                orderStatusStr = L"New";                break; // 2
		case nsOrderMessageDefine::osPendingReplace:     orderStatusStr = L"PendingReplace";     break; // 3
		case nsOrderMessageDefine::osReplaced:           orderStatusStr = L"Replaced";           break; // 4
		case nsOrderMessageDefine::osPendingCancel:      orderStatusStr = L"PendingCancel";      break; // 5
		case nsOrderMessageDefine::osCanceled:           orderStatusStr = L"Canceled";           break; // 6
		case nsOrderMessageDefine::osPartiallyFilled:    orderStatusStr = L"PartiallyFilled";    break; // 7
		case nsOrderMessageDefine::osFilled:             orderStatusStr = L"Filled";             break; // 8
		case nsOrderMessageDefine::osQuoteAccept:        orderStatusStr = L"sQuoteAccept";       break; // 9
		case nsOrderMessageDefine::osExpired:            orderStatusStr = L"Expired";            break; //10
		case nsOrderMessageDefine::osRejected:           orderStatusStr = L"Rejected";           break; //11
		case nsOrderMessageDefine::osQuoteRequestAccept: orderStatusStr = L"QuoteRequestAccept"; break; //12
		default:                                         orderStatusStr = L"N/A";
	}
	return orderStatusStr;
}  //DescribeOrderStatus()
//---------------------------------------------------------------------------
String DescribeCxlRejResponseTo(nsOrderMessageDefine::CxlRejResponseToEnum CxlRejResponseTo)
{
	String cxlRejResponseStr;
	switch(CxlRejResponseTo)
	{
		case nsOrderMessageDefine::crrNone:         cxlRejResponseStr = L"None";         break; // 0
		case nsOrderMessageDefine::crrNew:          cxlRejResponseStr = L"New";          break; // 1
		case nsOrderMessageDefine::crrReplace:      cxlRejResponseStr = L"Replace";      break; // 2
		case nsOrderMessageDefine::crrCancel:       cxlRejResponseStr = L"Cancel";       break; // 3
		case nsOrderMessageDefine::crrQuote:        cxlRejResponseStr = L"Quote";        break; // 4
		case nsOrderMessageDefine::crrQuoteCancel:  cxlRejResponseStr = L"QuoteCancel";  break; // 5
		case nsOrderMessageDefine::crrOrderStatus:  cxlRejResponseStr = L"OrderStatus";  break; // 6
		case nsOrderMessageDefine::crrQuoteRequest: cxlRejResponseStr = L"QuoteRequest"; break; // 7
		default:                                    cxlRejResponseStr = L"N/A";
	}
	return cxlRejResponseStr;
}  //DescribeCxlRejResponseTo()
//---------------------------------------------------------------------------
String DescribeOrdRejReason( nsOrderMessageDefine::OrdRejReasonEnum OrdRejReason )
{
	String ordRejReasonStr;
	switch (OrdRejReason)
	{
		case nsOrderMessageDefine::orrNone:                               ordRejReasonStr = L"None";                               break; // 0
		case nsOrderMessageDefine::orrBrokerExchangeOption:               ordRejReasonStr = L"BrokerExchangeOption";               break; // 1
		case nsOrderMessageDefine::orrUnknownSymbol:                      ordRejReasonStr = L"UnknownSymbol";                      break; // 2
		case nsOrderMessageDefine::orrExchangeClosed:                     ordRejReasonStr = L"ExchangeClosed";                     break; // 3
		case nsOrderMessageDefine::orrOrderExceedsLimit:                  ordRejReasonStr = L"OrderExceedsLimit";                  break; // 4
		case nsOrderMessageDefine::orrTooLateToEnter:                     ordRejReasonStr = L"TooLateToEnter";                     break; // 5
		case nsOrderMessageDefine::orrUnknownOrder:                       ordRejReasonStr = L"UnknownOrder";                       break; // 6
		case nsOrderMessageDefine::orrDuplicateOrder:                     ordRejReasonStr = L"DuplicateOrder";                     break; // 7
		case nsOrderMessageDefine::orrDuplicateVerballyCommunicatedOrder: ordRejReasonStr = L"DuplicateVerballyCommunicatedOrder"; break; // 8
		case nsOrderMessageDefine::orrStaleOrder:                         ordRejReasonStr = L"StaleOrder";                         break; // 9
		case nsOrderMessageDefine::orrTradeAlongRequired:                 ordRejReasonStr = L"TradeAlongRequired";                 break; //10
		case nsOrderMessageDefine::orrInvalidInvestorID:                  ordRejReasonStr = L"InvalidInvestorID";                  break; //11
		case nsOrderMessageDefine::orrUnsupportedOrderCharacteristic:     ordRejReasonStr = L"UnsupportedOrderCharacteristic";     break; //12
		case nsOrderMessageDefine::orrSurveillenceOption:                 ordRejReasonStr = L"SurveillenceOption";                 break; //13
		case nsOrderMessageDefine::orrIncorrectQuantity:                  ordRejReasonStr = L"IncorrectQuantity";                  break; //14
		case nsOrderMessageDefine::orrIncorrectAllocatedQuantity:         ordRejReasonStr = L"IncorrectAllocatedQuantity";         break; //15
		case nsOrderMessageDefine::orrUnknownAccount:                     ordRejReasonStr = L"UnknownAccount";                     break; //16
		case nsOrderMessageDefine::orrOther:                              ordRejReasonStr = L"Other";                              break; //17
		default:                                                          ordRejReasonStr = L"N/A";
	}
	return ordRejReasonStr;
}  //DescribeOrdRejReason()
//---------------------------------------------------------------------------
String DescribeQueryStatus(QueryStatusEnum QueryStatus)
{
	String queryStatusStr;
	switch(QueryStatus)
	{
		case qsNone:            queryStatusStr = L"None";              break;  // 0
		case qsPrepareRequest:  queryStatusStr = L"Prepare Request";   break;  // 1
		case qsRequestSended:   queryStatusStr = L"Request Sended";    break;  // 2
		case qsResponseArrived: queryStatusStr = L"Response Arrived";  break;  // 3
		case qsParsingResponse: queryStatusStr = L"Parsing Response";  break;  // 4
		case qsFinished:        queryStatusStr = L"Finished";          break;  // 5
		default:                queryStatusStr = L"N/A";               break;
	}  //switch(QueryStatus)
	return queryStatusStr;
}  //DescribeQueryStatus()
//---------------------------------------------------------------------------
bool IsDigitalCharacter(const wchar_t Data)
{
	if ((Data < L'0') || (Data > L'9'))
		return false;
	else
		return true;
}  //IsDigitalCharacter()
//---------------------------------------------------------------------------
bool IsDigitalString(const String& TargetStr)
{
	for (int i = 1; i <= TargetStr.Length(); i++)
		if (!IsDigitalCharacter(TargetStr[i])) return false;
	return true;
}  //IsDigitalString()
//---------------------------------------------------------------------------
void DoLockObject(UFC::PCriticalSection *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if ((LockObjPtr != 0) &&
		((RWLockType == rwLockForRead) || (RWLockType == rwLockForWrite)))
		LockObjPtr->Acquire();
}  //DoLockObject()
//---------------------------------------------------------------------------
void DoUnlockObject(UFC::PCriticalSection *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if ((LockObjPtr != 0) &&
		((RWLockType == rwLockForRead) || (RWLockType == rwLockForWrite)))
		LockObjPtr->Release();
}  //DoUnlockObject()
//---------------------------------------------------------------------------
void DoLockObject(UFC::PReadWriteLock *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (LockObjPtr == 0) return;
	if (RWLockType == rwLockForRead)
		LockObjPtr->LockForRead();
	else if (RWLockType == rwLockForWrite)
		LockObjPtr->LockForWrite();
}  //DoLockObject()
//---------------------------------------------------------------------------
void DoUnlockObject(UFC::PReadWriteLock *LockObjPtr, ReadWrietLockTypeEnum RWLockType)
{
	if (LockObjPtr == 0) return;
	if (RWLockType == rwLockForRead)
		LockObjPtr->UnlockForRead();
	else if (RWLockType == rwLockForWrite)
		LockObjPtr->UnlockForWrite();
}  //DoUnlockObject()

};  //namespace s888
#pragma package(smart_init)
