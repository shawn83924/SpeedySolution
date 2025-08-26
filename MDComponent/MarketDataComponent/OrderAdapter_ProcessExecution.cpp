//---------------------------------------------------------------------------
#pragma hdrstop
#include <vcl.h>
#include "OrderAdapter.h"
#include "MessageDataFormat.h"
#include "TNewOrderMessage.h"
#include "TCancelOrderMessage.h"
#include "TReplaceOrderMessage.h"
#include "TExecutionUnit.h"
#include "TradingObjects.h"
#include "TradingCommodities.h"
#include "TradingObjectPool.h"
#include "OrderStore.h"
#include <WideStrUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
void TOrderAdapter::ProcessPendingNewMsg( nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus,  TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr )
{
	UFCType::Int64 RptNID = NewReportPtr->GetNID();
	String exchangeCode = NewReportPtr->GetExchangeCode();
	String orderSymbol = NewReportPtr->GetSymbol()->GetOriginalSymbol();
	String rptExecId = NewReportPtr->GetExecID();
	String RptOrderId( NewReportPtr->GetOrderID());
	String logMessage,LocalMessageTimeStr,contractFullSymbol,TimeOrderIDKey;
	TExecution* OrderRootPtr = FOrderStore->FindOrderRootByNID( RptNID );

	GetLocalSystemTime( LocalMessageTimeStr, true );
	NewReportPtr->SetLocalMessageTime(LocalMessageTimeStr);
	TimeOrderIDKey.printf( L"%s%lld", LocalMessageTimeStr, RptNID );
	contractFullSymbol.printf( L"%s-%s", exchangeCode, orderSymbol );
	NewReportPtr->SetTimeOrderIDKey( TimeOrderIDKey );
	if( rptExecId.Length() <= 0 )
	{
		rptExecId = TimeOrderIDKey;
		NewReportPtr->SetExecID(rptExecId);
	}
	if (FOrderStore->FindOrderMessageByExecID(rptExecId) != 0)
	{
		logMessage.printf(L"Duplicate Exec ID[%s], Skip this Message.", rptExecId);
		UFC::BufferedLog::Printf(" TOrderAdapter::ProcessPendingNewMsg() %s", s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
		UFC::BufferedLog::FlushToFile();
		return;
	}
	//Get Order Root
	if (OrderRootPtr != NULL )
	{
		if (OrderRootPtr->GetOrderStatus() == nsOrderMessageDefine::osNone)
		{
			NewReportPtr->SetMarket(OrderRootPtr->GetMarket());
			NewReportPtr->SetExchangeCode(OrderRootPtr->GetExchangeCode());
			NewReportPtr->SetSymbol(OrderRootPtr->GetSymbol()->GetSymbol(0));
			NewReportPtr->SetSide(OrderRootPtr->GetSide());
			NewReportPtr->SetPrice(OrderRootPtr->GetPrice());
			NewReportPtr->SetOrderQty(OrderRootPtr->GetOrderQty());
			NewReportPtr->SetOrderType(OrderRootPtr->GetOrderType());
			NewReportPtr->SetTimeInForce(OrderRootPtr->GetTimeInForce());
			NewReportPtr->SetStopPrice(OrderRootPtr->GetStopPrice());
			if ((OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peAuto) ||
				(OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peAutoToday))
				OrderRootPtr->SetPositionEffect(NewReportPtr->GetPositionEffect());
			else
				NewReportPtr->SetPositionEffect(OrderRootPtr->GetPositionEffect());
			NewReportPtr->SetStrategyName(OrderRootPtr->GetStrategyName());
		}
	}
	else
	{
		UFC::BufferedLog::Printf(" TOrderAdapter::ProcessPendingNewMsg() OrderRoot not Found by NID[%lld]", RptNID );
		delete NewReportPtr;
		return;
	}

	OrderRootPtr->SetTMPExecType(RecvMsg->GetTMPExecType());
	OrderRootPtr->SetPxDigit(NewReportPtr->GetPxDigit());
	OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osPendingNew);
	OrderRootPtr->SetStatusCode(NewReportPtr->GetStatusCode());
	OrderRootPtr->SetText(NewReportPtr->GetText());
	if (RptOrderId.Length() > 0)
	{
		NewReportPtr->SetOrderID(RptOrderId);
		OrderRootPtr->SetOrderID(RptOrderId);
	}
	if( OrderRootPtr->GetLocalMessageTime().Length() <= 0)
	{
		OrderRootPtr->SetLocalMessageTime(LocalMessageTimeStr);
		OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		FOrderStore->InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
		FOrderStore->InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
	}
	FOrderStore->AddOrderMessage(NewReportPtr);
	FOrderStore->InsertOrderMessageExecIDIndex(NewReportPtr);
	NewReportPtr->SetRootNID(OrderRootPtr->GetNID());
	OrderRootPtr->InsertOrderMessage(NewReportPtr);
//	OrderRootPtr->InsertOrderMessageExecIDIndex(NewReportPtr);
	NewReportPtr->WriteToOrderLogStream(FOrderStore->GetOrderMsgFileStream());
	if (FOrderStore->OnOrderStatusChange != 0)
		FOrderStore->OnOrderStatusChange(FOrderStore);

//	FOrderStore->ExecuteOrderStatusListener( OrderRootPtr->GetMarket(), OrderRootPtr->GetOrderID() );
	if( !FIsRecovering )
		FOrderStore->ExecuteOrderMessageListener( OrderRootPtr );
	ProcessStrategyOrder( RecvMsg, OrderRootPtr );

	if (FOrderStore->IsAutoPositionEffect() &&
		((NewReportPtr->GetPositionEffect() == nsOrderMessageDefine::peClose) ||
		 (NewReportPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)))
	{
		String symbol = OrderRootPtr->GetSymbol()->GetSymbol(0);
		int processPositionCode = 0;
		String processPositionMsg = L"";
		s888::CContractPosition *positionPtr = s888::gTradingObjsPool.FindAccountContractPosition(FID, exchangeCode, orderSymbol, s888::constCreate, processPositionCode, processPositionMsg, s888::rwLockForWrite);
		if (positionPtr != 0)
		{
			if (NewReportPtr->GetPositionEffect() == nsOrderMessageDefine::peClose)
			{
				if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy) {
					positionPtr->AddBuyOrderCloseQuantity(OrderRootPtr->GetOrderQty(), s888::rwLockForWrite);
				} else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell) {
					positionPtr->AddSellOrderCloseQuantity(OrderRootPtr->GetOrderQty(), s888::rwLockForWrite);
				} else {
					UFC::BufferedLog::Printf(" TOrderAdapter::ProcessPendingNewMsg() Wrong Side %d, NID[%lld].", OrderRootPtr->GetSide(), NewReportPtr->GetNID());
				}
			}
			else if (NewReportPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)
			{
				if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy) {
					positionPtr->AddBuyOrderCloseTodayQuantity(OrderRootPtr->GetOrderQty(), s888::rwLockForWrite);
				} else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell) {
					positionPtr->AddSellOrderCloseTodayQuantity(OrderRootPtr->GetOrderQty(), s888::rwLockForWrite);
				} else {
					UFC::BufferedLog::Printf(" TOrderAdapter::ProcessPendingNewMsg() Wrong Side %d, NID[%lld].", OrderRootPtr->GetSide(), NewReportPtr->GetNID());
				}
			}
			else
				UFC::BufferedLog::Printf(" TOrderAdapter::ProcessPendingNewMsg() Wrong Position Efferc %d, NID[%lld].", OrderRootPtr->GetPositionEffect(), NewReportPtr->GetNID());
			logMessage.printf(L" PendingNew Order Report NID[%lld] [%s] side[%s] pe[%s] Qty[%d] Px[%15.6lf].",
							  RptNID, contractFullSymbol,
							  s888::DescribeBuySell(OrderRootPtr->GetSide()),
							  s888::DescribePositionEffect(OrderRootPtr->GetPositionEffect()),
							  OrderRootPtr->GetOrderQty(), OrderRootPtr->GetPrice());
			UFC::BufferedLog::Printf(" TOrderAdapter::ProcessPendingNewMsg() %s", s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
			positionPtr->DumpData(s888::constFlush, s888::rwLockForRead );
		}
		else
		{
			UFC::BufferedLog::Printf(" TOrderAdapter::ProcessPendingNewMsg() Find Account Contract Position Failed. %d[%s]",
									 processPositionCode, s888::ConvertUnicodeToUFCAnsiStr(processPositionMsg).c_str());
			UFC::BufferedLog::FlushToFile();
		}  //if (positionPtr != 0)
	}
}  //TOrderAdapter::ProcessPendingNewMsg()

//---------------------------------------------------------------------------
void TOrderAdapter::ProcessNewMsg(nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus, TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr)
{
	UFCType::Int64 RptNID = NewReportPtr->GetNID();
	String logMessage,contractFullSymbol,TimeOrderIDKey,LocalMessageTimeStr;
	String exchangeCode = NewReportPtr->GetExchangeCode();
	String orderSymbol = NewReportPtr->GetSymbol()->GetOriginalSymbol();
	String tradeSymbol = NewReportPtr->GetTradeSymbol();
	String TransactTimeStr = NewReportPtr->GetTransactTime();
	String rptExecId = NewReportPtr->GetExecID();
	String RptOrderId( NewReportPtr->GetOrderID());
	nsOrderMessageDefine::ExecTypeEnum RptExecType = NewReportPtr->GetExecType();

	UFC::BufferedLog::Printf(" NewOrder[%s] Use[%d]ms", RecvMsg->GetOrderID(), RecvMsg->GetUseus()/1000 );
	GetLocalSystemTime( LocalMessageTimeStr, true );
	contractFullSymbol.printf(L"%s-%s", exchangeCode, orderSymbol);
	///< OrderID not exists !!!
	if ((RptOrderId.Length() <= 0 ) || (RptOrderId == L"00000"))
	{
		delete NewReportPtr;
		return;
	}
	///< Response OrderStatus request
	if( RptExecType == nsOrderMessageDefine::etOrderStatus )
	{
		if( NewReportPtr->GetCumQty() > 0 ) ///< Update the filled info.
		{
			nsOrderMessageDefine::OrderStatusEnum NewMsgOrderStatus;

			if (NewReportPtr->GetLeavesQty() > 0)
				NewMsgOrderStatus = nsOrderMessageDefine::osPartiallyFilled;
			else
				NewMsgOrderStatus = nsOrderMessageDefine::osFilled;
			NewReportPtr->SetOrderStatus( NewMsgOrderStatus );
			NewReportPtr->SetLastPx(NewReportPtr->GetPrice());
			ProcessFilledMsg( NewMsgOrderStatus, RecvMsg, NewReportPtr );
			return;
		}
	}
	//Set Local Message Time
	if( FIsRecovering && TransactTimeStr.Length() > 0 )
		NewReportPtr->SetLocalMessageTime(TransactTimeStr);
	else
		NewReportPtr->SetLocalMessageTime(LocalMessageTimeStr);
	TimeOrderIDKey = NewReportPtr->GetLocalMessageTime() + RptOrderId;
	NewReportPtr->SetTimeOrderIDKey( TimeOrderIDKey );
	///< Process ExecID
	if( rptExecId.Length() <= 0 )
	{
		rptExecId = TimeOrderIDKey;
		NewReportPtr->SetExecID( rptExecId );
	}
	///< Check ExecID Duplicate
	if (FOrderStore->FindOrderMessageByExecID( rptExecId ) != 0)
	{
		AnsiString dupExecID(rptExecId);
		UFC::BufferedLog::Printf(" TOrderAdapter::ProcessNewMsg() Duplicate Exec ID[%s], Skip this Message.", dupExecID.c_str() );
		UFC::BufferedLog::FlushToFile();
		return;
	}

	//Find Order Root By Message NID First
	bool IsOrderStatusChanged = false;
	bool isOrderRootExist = true;
	nsOrderMessageDefine::OrderStatusEnum OrderRootStatus = nsOrderMessageDefine::osNone;
	TExecution* OrderRootPtr = FindOrderRootByOrderId_NID(RptOrderId, RptNID);

	if( OrderRootPtr == NULL )
	{   //No Order Root Exists, Create a New Order Root
		isOrderRootExist = false;
		IsOrderStatusChanged = true;
		OrderRootPtr = new TExecution(RecvMsg);
		OrderRootPtr->SetMarket(NewReportPtr->GetMarket());
		OrderRootPtr->SetRoot(true);
		OrderRootPtr->SetRootCreatedByReport(true);
		OrderRootPtr->SetLocalMessageTime(NewReportPtr->GetLocalMessageTime());
		OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		FOrderStore->AddOrderRoot(OrderRootPtr);
		FOrderStore->InsertOrderRootNIDIndex(OrderRootPtr);
		if ((orderSymbol.Length() > 0) && (orderSymbol != tradeSymbol))
			OrderRootPtr->SetSymbol(orderSymbol);
		if (((OrderRootPtr->GetMarket() == nsOrderMessageDefine::mTWFutures) ||
			 (OrderRootPtr->GetMarket() == nsOrderMessageDefine::mTWOptions)) &&
			(RecvMsg->GetTMPExecType() == nsOrderMessageDefine::tetNewAndFilled))
		{
			OrderRootPtr->SetCumQty(0);
			OrderRootPtr->SetLeavesQty(OrderRootPtr->GetOrderQty());
		}
		logMessage.printf(L" Order Root [%lld] not Found, Create One, [%s] side[%s] pe[%s] Qty[%d] Px[%15.6lf].",
						  RptNID, contractFullSymbol,
						  s888::DescribeBuySell(OrderRootPtr->GetSide()),
						  s888::DescribePositionEffect(OrderRootPtr->GetPositionEffect()),
						  OrderRootPtr->GetOrderQty(), OrderRootPtr->GetPrice());
		UFC::BufferedLog::Printf(" TOrderAdapter::ProcessNewMsg() %s", s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
	}
	else  //Order Root Exist
	{
		OrderRootStatus = OrderRootPtr->GetOrderStatus();
		if ((OrderRootStatus == nsOrderMessageDefine::osReplaced) ||
			(OrderRootStatus == nsOrderMessageDefine::osCanceled) ||
			(OrderRootStatus == nsOrderMessageDefine::osPartiallyFilled) ||
			(OrderRootStatus == nsOrderMessageDefine::osFilled) ||
			((RptExecType != nsOrderMessageDefine::etOrderStatus) &&
			 (OrderRootStatus == nsOrderMessageDefine::osRejected)))
		{
			if ((!FIsRecovering) && (FOrderStore->OnErrorMessage != 0))
			{
				SYSTEMTIME SysTime;

				GetLocalTime( &SysTime );
				FOrderStore->OnErrorMessage(FOrderStore, SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds,
											osErrNotCorrectMessage, MsgOrderStatus,
											RptOrderId, RptNID, orderSymbol, NewReportPtr->GetSide(),
											NewReportPtr->GetPrice(), NewReportPtr->GetOrderQty());
            }
			delete NewReportPtr;
			logMessage.printf(L"OrderRoot OrderID[%s] Status is not pendingNew. NID=%lld.", RptOrderId, RptNID);
			UFC::BufferedLog::Printf(" TOrderAdapter::ProcessNewMsg() %s", s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
			return;
		}

		if (OrderRootStatus != nsOrderMessageDefine::osNew)
		{
			IsOrderStatusChanged = true;
			OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osNew);
			OrderRootPtr->SetTransactTime(TransactTimeStr);
			OrderRootPtr->SetOrderID(RptOrderId);
			if (NewReportPtr->GetLeavesQty() > 0)
				OrderRootPtr->SetLeavesQty(NewReportPtr->GetLeavesQty());
			else
				OrderRootPtr->SetLeavesQty(OrderRootPtr->GetOrderQty());
			OrderRootPtr->SetPxDigit(NewReportPtr->GetPxDigit());
			if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)
			{
				if (OrderRootPtr->GetTimeOrderIDKey().Length() > 0)
				{
					FOrderStore->RemoveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
					FOrderStore->RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
				}
			}
			OrderRootPtr->SetLocalMessageTime(NewReportPtr->GetLocalMessageTime());
			OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		}  //if (OrderRootStatus != nsOrderMessageDefine::osNew)

		OrderRootPtr->SetStatusCode(NewReportPtr->GetStatusCode());
		OrderRootPtr->SetText(NewReportPtr->GetText());
		if (((OrderRootPtr->GetMarket() == nsOrderMessageDefine::mTWFutures) ||
			 (OrderRootPtr->GetMarket() == nsOrderMessageDefine::mTWOptions)) &&
			(RecvMsg->GetTMPExecType() == nsOrderMessageDefine::tetNewAndFilled))
		{
			OrderRootPtr->SetCumQty(0);
			OrderRootPtr->SetLeavesQty(OrderRootPtr->GetOrderQty());
		}
	}  //if (OrderRootPtr == 0)

	if ((OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peAuto) ||
		(OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peAutoToday))
		OrderRootPtr->SetPositionEffect(NewReportPtr->GetPositionEffect());
	if ((OrderRootPtr->GetDisplayName().Length() <= 0) &&
		(NewReportPtr->GetDisplayName().Length() > 0))
		OrderRootPtr->SetDisplayName(NewReportPtr->GetDisplayName());

	if (!isOrderRootExist && IsOrderStatusChanged && FOrderStore->IsAutoPositionEffect() && OrderRootPtr->IsRootCreatedByReport() &&
		((NewReportPtr->GetPositionEffect() == nsOrderMessageDefine::peClose) ||
		 (NewReportPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)))
	{
		String symbol = OrderRootPtr->GetSymbol()->GetSymbol(0);
		int processPositionCode = 0;
		String processPositionMsg = L"";
		s888::CContractPosition *positionPtr = s888::gTradingObjsPool.FindAccountContractPosition(FID, exchangeCode, tradeSymbol, s888::constCreate, processPositionCode, processPositionMsg, s888::rwLockForWrite);
		if (positionPtr != 0)
		{
			if (NewReportPtr->GetPositionEffect() == nsOrderMessageDefine::peClose)
			{
				if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy) {
					positionPtr->AddBuyOrderCloseQuantity(OrderRootPtr->GetOrderQty(), s888::rwLockForWrite);
				} else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell) {
					positionPtr->AddSellOrderCloseQuantity(OrderRootPtr->GetOrderQty(), s888::rwLockForWrite);
				} else {
					UFC::BufferedLog::Printf(" TOrderAdapter::ProcessNewMsg() Wrong Side %d, NID[%lld].", OrderRootPtr->GetSide(), NewReportPtr->GetNID());
				}
			}
			else if (NewReportPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)
			{
				if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy) {
					positionPtr->AddBuyOrderCloseTodayQuantity(OrderRootPtr->GetOrderQty(), s888::rwLockForWrite);
				} else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell) {
					positionPtr->AddSellOrderCloseTodayQuantity(OrderRootPtr->GetOrderQty(), s888::rwLockForWrite);
				} else {
					UFC::BufferedLog::Printf(" TOrderAdapter::ProcessNewMsg() Wrong Side %d, NID[%lld].", OrderRootPtr->GetSide(), NewReportPtr->GetNID());
				}
			}
			else
				UFC::BufferedLog::Printf(" TOrderAdapter::ProcessNewMsg() Wrong Position Efferc %d, NID[%lld].", OrderRootPtr->GetPositionEffect(), NewReportPtr->GetNID());
			logMessage.printf(L" PendingNew Order Report NID[%lld] [%s] side[%s] pe[%s] Qty[%d] Px[%15.6lf].",
							  RptNID, contractFullSymbol,
							  s888::DescribeBuySell(OrderRootPtr->GetSide()),
							  s888::DescribePositionEffect(OrderRootPtr->GetPositionEffect()),
							  OrderRootPtr->GetOrderQty(), OrderRootPtr->GetPrice());
			UFC::BufferedLog::Printf(" TOrderAdapter::ProcessNewMsg() %s",
									 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
			positionPtr->DumpData(s888::constFlush, s888::rwLockForRead);
		}
		else
		{
			UFC::BufferedLog::Printf(" TOrderAdapter::TOrderAdapter::ProcessNewMsg() Find Account Contract Position Failed. %d[%s]",
									 processPositionCode, s888::ConvertUnicodeToUFCAnsiStr(processPositionMsg).c_str());
			UFC::BufferedLog::FlushToFile();
		}  //if (positionPtr != 0)
	}

	FOrderStore->AddOrderMessage(NewReportPtr);
	FOrderStore->InsertOrderMessageExecIDIndex(NewReportPtr);
	NewReportPtr->SetRootNID(OrderRootPtr->GetNID());
	OrderRootPtr->InsertOrderMessage(NewReportPtr);
//	OrderRootPtr->InsertOrderMessageExecIDIndex(NewReportPtr);
	if (IsOrderStatusChanged)
	{
		FOrderStore->InsertOrderRootOrderIDIndex(OrderRootPtr);
		FOrderStore->InsertOrderRootTimeOrderIDIndex(OrderRootPtr);       ///< All Pagr
		FOrderStore->InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr); ///< Active Page
		FOrderStore->InsertOrderRootSymbolSidePriceIndex(OrderRootPtr);

		if (FOrderStore->OnNewOrderReply != 0)
			FOrderStore->OnNewOrderReply(FOrderStore,
										 RptOrderId, orderSymbol,
										 NewReportPtr->GetSide(), NewReportPtr->GetPrice(), NewReportPtr->GetOrderQty(),
										 NewReportPtr->GetOrderType(), NewReportPtr->GetTimeInForce(), NewReportPtr->GetPositionEffect());

		if (!OrderRootPtr->GetSymbol()->IsMultileg())
		{
			FOrderStore->ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), OrderRootPtr->GetExchangeCode(),
												 OrderRootPtr->GetSymbol()->GetOriginalSymbol(),
												 OrderRootPtr->GetSide(),
												 OrderRootPtr->GetPrice());
		}
		if (FOrderStore->OnOrderStatusChange != 0)
			FOrderStore->OnOrderStatusChange(FOrderStore);
	}  //if (IsOrderStatusChanged)

	NewReportPtr->SetStopPrice(OrderRootPtr->GetStopPrice());
	NewReportPtr->SetStrategyName(OrderRootPtr->GetStrategyName());
	NewReportPtr->WriteToOrderLogStream(FOrderStore->GetOrderMsgFileStream());
	if (!FIsRecovering) FOrderStore->ExecuteOrderMessageListener(OrderRootPtr);
//	FOrderStore->ExecuteOrderStatusListener(OrderRootPtr->GetMarket(), RptOrderId);
	ProcessStrategyOrder(RecvMsg, OrderRootPtr);
}  //TOrderAdapter::ProcessNewMsg()

//---------------------------------------------------------------------------
TExecution *TOrderAdapter::FindOrderRootByOrderId_NID(const UnicodeString& RptOrderId, UFCType::Int64 RptNID)
{
	TExecution *OrderRootPtr = 0;
	//Find Order Root By Order ID First
	if ((RptOrderId.Length() > 0) && (RptOrderId != L"00000"))
		OrderRootPtr = FOrderStore->FindOrderRootByOrderID(RptOrderId);

	//Find Order Root By Message NID
	if (OrderRootPtr == 0) OrderRootPtr = FOrderStore->FindOrderRootByNID(RptNID);

	if (OrderRootPtr == 0)  //Search Order Root in Query NID Map
	{
		UFCType::Int64 OrderRootNID = FindOrderRootNIDByQueryNID(RptNID);
		if (OrderRootNID > 0L)
		{
			OrderRootPtr = FOrderStore->FindOrderRootByNID(OrderRootNID);
			if (OrderRootPtr != 0) RemoveQueryNIDIndex(RptNID);
		}
	}  //if (OrderRootPtr == 0)

	return OrderRootPtr;
}  //TOrderAdapter::FindOrderRootByOrderId_NID()

//---------------------------------------------------------------------------
void TOrderAdapter::ProcessFilledMsg(nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus, TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr)
{
	UFCType::Int64 RptNID = NewReportPtr->GetNID();
	String exchangeCode = NewReportPtr->GetExchangeCode();
	String orderSymbol = NewReportPtr->GetSymbol()->GetOriginalSymbol();
	String RptOrderId( NewReportPtr->GetOrderID());
	String rptExecId( NewReportPtr->GetExecID() );
	String TransactTimeStr( NewReportPtr->GetTransactTime() );
	String contractFullSymbol,LocalMessageTimeStr,logMessage,TimeOrderIDKey;
	nsOrderMessageDefine::ExecTypeEnum RptExecType = NewReportPtr->GetExecType();

	GetLocalSystemTime( LocalMessageTimeStr, true );
	contractFullSymbol.printf(L"%s-%s", exchangeCode, orderSymbol);
	if ((RptOrderId.Length() <= 0) || (RptOrderId == L"00000"))
	{
		if (!FIsRecovering && (FOrderStore->OnErrorMessage != 0))
		{
			SYSTEMTIME SysTime;

			GetLocalTime( &SysTime );
			FOrderStore->OnErrorMessage(FOrderStore, SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds,
										osErrNotCorrectMessage, MsgOrderStatus,
										RptOrderId, RptNID, orderSymbol, NewReportPtr->GetSide(),
										NewReportPtr->GetPrice(), NewReportPtr->GetOrderQty());
		}
		delete NewReportPtr;
		return;
	}
	NewReportPtr->SetLastPx(NewReportPtr->GetPrice());
	if( NewReportPtr->GetLeavesQty() > 0 )
		NewReportPtr->SetOrderStatus(nsOrderMessageDefine::osPartiallyFilled);
	else
		NewReportPtr->SetOrderStatus(nsOrderMessageDefine::osFilled);
	//Set Local Message Time
	if( FIsRecovering && TransactTimeStr.Length() > 0 )
		NewReportPtr->SetLocalMessageTime(TransactTimeStr);
	else
		NewReportPtr->SetLocalMessageTime(LocalMessageTimeStr);
	TimeOrderIDKey = NewReportPtr->GetLocalMessageTime() + RptOrderId;
	NewReportPtr->SetTimeOrderIDKey(TimeOrderIDKey);
	if( rptExecId.Length() <= 0 )
	{
		rptExecId = TimeOrderIDKey;
		NewReportPtr->SetExecID(rptExecId);
	}
	if (FOrderStore->FindOrderMessageByExecID(rptExecId) != 0)
	{
		logMessage.printf(L"Duplicate Exec ID[%s], Skip this Message.", rptExecId);
		UFC::BufferedLog::Printf(" TOrderAdapter::ProcessFilledMsg() %s", s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
		UFC::BufferedLog::FlushToFile();
		return;
	}
	//Get Order Root
	TExecution *OrderRootPtr = FindOrderRootByOrderId_NID(RptOrderId, RptNID);
	nsOrderMessageDefine::OrderStatusEnum OrderRootStatus = nsOrderMessageDefine::osNone;
	bool IsOrderStatusChanged = false;
	if (OrderRootPtr == 0)
	{   //No Order Root Exists, Create a New Order Root
		logMessage.printf(L"OrderRoot not Found by OrderID[%s], NID[%lld]. Create One.", RptOrderId, RptNID);
		UFC::BufferedLog::Printf(" TOrderAdapter::ProcessFilledMsg() %s", s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
		IsOrderStatusChanged = true;
		OrderRootPtr = new TExecution(RecvMsg);
		OrderRootPtr->SetMarket(NewReportPtr->GetMarket());
		OrderRootPtr->SetRoot(true);
		OrderRootPtr->SetLocalMessageTime(NewReportPtr->GetLocalMessageTime());
		OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		OrderRootPtr->SetSymbol(orderSymbol);
		FOrderStore->AddOrderRoot(OrderRootPtr);
		FOrderStore->InsertOrderRootNIDIndex(OrderRootPtr);
		FOrderStore->InsertOrderRootOrderIDIndex(OrderRootPtr);
		FOrderStore->InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
	}
	else
	{   //Order Root Exist
		OrderRootStatus = OrderRootPtr->GetOrderStatus();
		if ((OrderRootStatus == nsOrderMessageDefine::osCanceled) ||
			((RptExecType != nsOrderMessageDefine::etOrderStatus) &&
			 (OrderRootStatus == nsOrderMessageDefine::osFilled)))
		{
			if ((!FIsRecovering) && (FOrderStore->OnErrorMessage != 0))
			{
				SYSTEMTIME SysTime;

				GetLocalTime( &SysTime );
				FOrderStore->OnErrorMessage(FOrderStore, SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds,
											osErrNotCorrectMessage, MsgOrderStatus,
											RptOrderId, RptNID, orderSymbol, NewReportPtr->GetSide(),
											NewReportPtr->GetPrice(), NewReportPtr->GetOrderQty());
            }
			delete NewReportPtr;
			return;
		}

		String OrderRootOrderId = OrderRootPtr->GetOrderID();
		if (((OrderRootOrderId.Length() <= 0) || (OrderRootOrderId == L"00000")) &&
			((RptOrderId.Length() > 0) && (RptOrderId != L"00000")))
		{
			OrderRootPtr->SetOrderID(RptOrderId);
			FOrderStore->InsertOrderRootOrderIDIndex(OrderRootPtr);
		}

		int rptLastQty = NewReportPtr->GetOrderQty();
		if ((OrderRootStatus != MsgOrderStatus) ||
			((MsgOrderStatus == nsOrderMessageDefine::osPartiallyFilled) &&
			 (rptLastQty <= OrderRootPtr->GetLeavesQty())))
		{
			IsOrderStatusChanged = true;
			OrderRootPtr->SetCumQty(OrderRootPtr->GetCumQty() + rptLastQty);
			OrderRootPtr->SetLeavesQty(OrderRootPtr->GetLeavesQty() - rptLastQty);
			OrderRootPtr->SetLastQty(rptLastQty);
			OrderRootPtr->SetLegQty1(NewReportPtr->GetLegQty1());
			OrderRootPtr->SetLegQty2(NewReportPtr->GetLegQty2());
			OrderRootPtr->SetLastPx(NewReportPtr->GetLastPx());
			OrderRootPtr->SetLegPx1(NewReportPtr->GetLegPx1());
			OrderRootPtr->SetLegPx2(NewReportPtr->GetLegPx2());
			OrderRootPtr->SetAvgPx(NewReportPtr->GetAvgPx());
			OrderRootPtr->SetHHMM( FOrderStore->MarketDataStore->GetClock( exchangeCode )->GetHHMM() );
			if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)
			{
				if (OrderRootPtr->GetTimeOrderIDKey().Length() > 0)
				{
					FOrderStore->RemoveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
					FOrderStore->RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
				}

				String EmptyStr = L"";
				OrderRootPtr->SetLocalMessageTime(EmptyStr);
				OrderRootPtr->SetTimeOrderIDKey(EmptyStr);
			}

			if (OrderRootPtr->GetLocalMessageTime().Length() <= 0)
				OrderRootPtr->SetLocalMessageTime(NewReportPtr->GetLocalMessageTime());

			if (OrderRootPtr->GetTimeOrderIDKey().Length() <= 0)
			{
				OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
				FOrderStore->InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
			}

			if (OrderRootPtr->GetLeavesQty() > 0)
			{
				NewReportPtr->SetOrderStatus(nsOrderMessageDefine::osPartiallyFilled);
				OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osPartiallyFilled);
			}
			else
			{
				NewReportPtr->SetOrderStatus(nsOrderMessageDefine::osFilled);
				OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osFilled);
			}
			NewReportPtr->SetLeavesQty(OrderRootPtr->GetLeavesQty());
			NewReportPtr->SetCumQty(OrderRootPtr->GetCumQty());

			OrderRootPtr->SetStatusCode(NewReportPtr->GetStatusCode());
			OrderRootPtr->SetText(NewReportPtr->GetText());
		}
	}  //if (OrderRootPtr == 0)

	if (OrderRootPtr->GetLeavesQty() > 0)
		FOrderStore->InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
	else
		FOrderStore->RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());

	FOrderStore->AddOrderMessage(NewReportPtr);
	FOrderStore->InsertOrderMessageExecIDIndex(NewReportPtr);
	NewReportPtr->SetStopPrice(OrderRootPtr->GetStopPrice());
	NewReportPtr->SetRootNID(OrderRootPtr->GetNID());
	OrderRootPtr->InsertOrderMessage(NewReportPtr);
//	OrderRootPtr->InsertOrderMessageExecIDIndex(NewReportPtr);
	if (IsOrderStatusChanged)
	{
		FOrderStore->InsertFilledOrderRootTimeOrderIDIndex(OrderRootPtr);
		FOrderStore->InsertFillMessageSymbolIndex(NewReportPtr);
		FOrderStore->AddPositionRecFillQty(NewReportPtr);

		if (FOrderStore->IsAutoPositionEffect())
		{
			int processPositionCode = 0;
			String processPositionMsg = L"";
			s888::CContractPosition *positionPtr = s888::gTradingObjsPool.FindAccountContractPosition(FOrderStore->ID, exchangeCode, orderSymbol, s888::constCreate, processPositionCode, processPositionMsg, s888::rwLockForWrite);
			if (positionPtr != 0)
			{
				positionPtr->LockForWrite();
				if (NewReportPtr->GetPositionEffect() == nsOrderMessageDefine::peOpen)
				{
					if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy)
						positionPtr->AddBuyFillOpenQuantity(OrderRootPtr->GetLastQty(), s888::rwNotLock);
					else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell)
						positionPtr->AddSellFillOpenQuantity(OrderRootPtr->GetLastQty(), s888::rwNotLock);
				}
				else if (NewReportPtr->GetPositionEffect() == nsOrderMessageDefine::peClose)
				{
					if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy)
					{
						positionPtr->AddBuyFillCloseQuantity(OrderRootPtr->GetLastQty(), s888::rwNotLock);
						positionPtr->SubstractBuyOrderCloseQuantity(OrderRootPtr->GetLastQty(), s888::rwNotLock);
					}
					else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell)
					{
						positionPtr->AddSellFillCloseQuantity(OrderRootPtr->GetLastQty(), s888::rwNotLock);
						positionPtr->SubstractSellOrderCloseQuantity(OrderRootPtr->GetLastQty(), s888::rwNotLock);
					}
				}
				else if (NewReportPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)
				{
					if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sBuy)
					{
						positionPtr->AddBuyFillCloseTodayQuantity(OrderRootPtr->GetLastQty(), s888::rwNotLock);
						positionPtr->SubstractBuyOrderCloseTodayQuantity(OrderRootPtr->GetLastQty(), s888::rwNotLock);
					}
					else if (OrderRootPtr->GetSide() == nsOrderMessageDefine::sSell)
					{
						positionPtr->AddSellFillCloseTodayQuantity(OrderRootPtr->GetLastQty(), s888::rwNotLock);
						positionPtr->SubstractSellOrderCloseTodayQuantity(OrderRootPtr->GetLastQty(), s888::rwNotLock);
					}
				}
				logMessage.printf(L" Filled Report[%s] NID[%lld] [%s] side[%s] pe[%s] LastQty[%d] LastPx[%15.6lf].",
								  RptOrderId, RptNID, contractFullSymbol,
								  s888::DescribeBuySell(OrderRootPtr->GetSide()),
								  s888::DescribePositionEffect(OrderRootPtr->GetPositionEffect()),
								  OrderRootPtr->GetLastQty(), OrderRootPtr->GetLastPx());
				UFC::BufferedLog::Printf(" TOrderAdapter::ProcessFilledMsg() %s",
										 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
				positionPtr->DumpData(s888::constFlush, s888::rwNotLock);
				positionPtr->UnlockForWrite();
			}
			else
			{
				UFC::BufferedLog::Printf(" TOrderAdapter::ProcessFilledMsg() Find Account Contract Position Failed. %d[%s]",
										 processPositionCode, s888::ConvertUnicodeToUFCAnsiStr(processPositionMsg).c_str());
				UFC::BufferedLog::FlushToFile();
			}  //if (positionPtr != 0)
		}  //if (OrderStore->IsAutoPositionEffect())
		FOrderStore->ExecuteOnFilled( exchangeCode,orderSymbol, RptOrderId, OrderRootPtr->GetLastPx(), OrderRootPtr->GetLastQty(), OrderRootPtr->GetSide(), MsgOrderStatus, OrderRootPtr->GetHHMM() );
		if (OrderRootPtr->GetSymbol()->IsMultileg())
		{
			String curSymbolCode = OrderRootPtr->GetSymbol()->GetSymbol(1);
			FOrderStore->SubscribeSymbol(exchangeCode, curSymbolCode, false, false);  //need not IncreaseCount, Subscribe if need
			FOrderStore->ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), exchangeCode, curSymbolCode,
												 OrderRootPtr->GetLegSide1(),
												 OrderRootPtr->GetPrice());
			FOrderStore->ExecutePositionChangeListener(OrderRootPtr->GetMarket(), exchangeCode, curSymbolCode,
													   OrderRootPtr->GetLegSide1(), OrderRootPtr->GetLegPx1(), OrderRootPtr->GetLegQty1(), OrderRootPtr->GetTickCount());

			curSymbolCode = OrderRootPtr->GetSymbol()->GetSymbol(2);
			FOrderStore->SubscribeSymbol(exchangeCode, curSymbolCode, false, false);  //need not IncreaseCount, Subscribe if need
			FOrderStore->ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), exchangeCode, curSymbolCode,
												 OrderRootPtr->GetLegSide2(),
												 OrderRootPtr->GetPrice());
			FOrderStore->ExecutePositionChangeListener(OrderRootPtr->GetMarket(), exchangeCode, curSymbolCode,
													   OrderRootPtr->GetLegSide2(), OrderRootPtr->GetLegPx2(), OrderRootPtr->GetLegQty2(), OrderRootPtr->GetTickCount());
			///< Handle Option Strategy Order
			UFC::AnsiString strategySymbol = s888::ConvertUnicodeToUFCAnsiStr(orderSymbol);
			FOrderStore->ExecuteStrategyOrderListener(strategySymbol.c_str(), OrderRootPtr->GetSide(), OrderRootPtr->GetLastPx(), OrderRootPtr->GetLastQty(), OrderRootPtr->GetLeavesQty());
		}
		else
		{
			FOrderStore->SubscribeSymbol(exchangeCode, orderSymbol, true, false);  //needIncreaseCount, Subscribe if need
			FOrderStore->ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), exchangeCode, orderSymbol,
												 OrderRootPtr->GetSide(), OrderRootPtr->GetPrice());
			FOrderStore->ExecutePositionChangeListener(OrderRootPtr->GetMarket(), exchangeCode, orderSymbol,
													   OrderRootPtr->GetSide(), OrderRootPtr->GetLastPx(), OrderRootPtr->GetLastQty(), OrderRootPtr->GetTickCount());
		}  //if (OrderRootPtr->GetSymbol()->IsMultileg())

		if (FOrderStore->OnOrderStatusChange != 0)
			FOrderStore->OnOrderStatusChange(FOrderStore);
	}  //if (IsOrderStatusChanged)

	NewReportPtr->SetStrategyName(OrderRootPtr->GetStrategyName());
	NewReportPtr->WriteToOrderLogStream(FOrderStore->GetOrderMsgFileStream());
//	FOrderStore->ExecuteOrderStatusListener(OrderRootPtr->GetMarket(), RptOrderId);
	if (!FIsRecovering) FOrderStore->ExecuteOrderMessageListener(OrderRootPtr);
	ProcessStrategyOrder(RecvMsg, OrderRootPtr);
}  //TOrderAdapter::ProcessFilledMsg()

//---------------------------------------------------------------------------
void TOrderAdapter::ProcessReplacedMsg(nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus, TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr)
{
	UFCType::Int64 RptNID =  NewReportPtr->GetNID();
	String logMessage,contractFullSymbol,LocalMessageTimeStr;
	String exchangeCode = NewReportPtr->GetExchangeCode();
	String orderSymbol = NewReportPtr->GetSymbol()->GetOriginalSymbol();
	String tradeSymbol = NewReportPtr->GetTradeSymbol();
	String RptOrderId( NewReportPtr->GetOrderID());
	nsOrderMessageDefine::TMPExecTypeEnum MsgTMPExecType = NewReportPtr->GetTMPExecType();
	bool IsTaifex = false;

	GetLocalSystemTime( LocalMessageTimeStr, true );
	if (orderSymbol != tradeSymbol)
		contractFullSymbol.printf(L"%s-%s(%s)", exchangeCode, orderSymbol, tradeSymbol);
	else
		contractFullSymbol.printf(L"%s-%s", exchangeCode, orderSymbol);

	if( RecvMsg->GetMarket() == nsOrderMessageDefine::mTWFutures || RecvMsg->GetMarket() == nsOrderMessageDefine::mTWOptions )
	{
		IsTaifex = true;
		if (((MsgTMPExecType != nsOrderMessageDefine::tetReplaced) &&
			 (MsgTMPExecType != nsOrderMessageDefine::tetPxReplaced) &&
			 (MsgTMPExecType != nsOrderMessageDefine::tetPxReplaced2)) ||
			((RptOrderId.Length() <= 0) || (RptOrderId == L"00000")))
		{
			if (!FIsRecovering && (FOrderStore->OnErrorMessage != 0))
			{
				SYSTEMTIME SysTime;

				GetLocalTime( &SysTime );
				FOrderStore->OnErrorMessage(FOrderStore, SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds,
											osErrNotCorrectMessage, MsgOrderStatus,
											RptOrderId, RptNID, orderSymbol, NewReportPtr->GetSide(),
											NewReportPtr->GetPrice(), NewReportPtr->GetOrderQty());
			}

			delete NewReportPtr;
			return;
		}
	}

	//Set Local Message Time
	String TransactTimeStr = NewReportPtr->GetTransactTime();
	String TimeOrderIDKey;
	if (FIsRecovering && (TransactTimeStr.Length() > 0))
	{
		NewReportPtr->SetLocalMessageTime(TransactTimeStr);
		TimeOrderIDKey = TransactTimeStr + RptOrderId;
	}
	else
	{
		NewReportPtr->SetLocalMessageTime(LocalMessageTimeStr);
		TimeOrderIDKey = LocalMessageTimeStr + RptOrderId;
	}

	NewReportPtr->SetTimeOrderIDKey(TimeOrderIDKey);
	String rptExecId = NewReportPtr->GetExecID();
	if (rptExecId.Length() <= 0)
	{
		rptExecId = TimeOrderIDKey;
		NewReportPtr->SetExecID(TimeOrderIDKey);
	}
	if( IsTaifex == true && FOrderStore->FindOrderMessageByExecID(rptExecId) != NULL )
	{
		String logMessage;
		logMessage.printf(L"Duplicate Exec ID[%s], Skip this Message.");
		UFC::BufferedLog::Printf(" TOrderAdapter::ProcessReplacedMsg() %s",
								 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
		UFC::BufferedLog::FlushToFile();
		return;
	}

	//Get Order Root
	TExecution* OrderRootPtr = FindOrderRootByOrderId_NID(RptOrderId, RptNID);
	nsOrderMessageDefine::OrderStatusEnum OrderRootStatus = nsOrderMessageDefine::osNone;
	nsOrderMessageDefine::ExecTypeEnum RptExecType = NewReportPtr->GetExecType();
	bool IsOrderStatusChanged = false;
	if (OrderRootPtr == 0)
	{   //No Order Root Exists, Create a New Order Root
		IsOrderStatusChanged = true;
		OrderRootPtr = new TExecution(RecvMsg);
		OrderRootPtr->SetMarket(NewReportPtr->GetMarket());
		OrderRootPtr->SetRoot(true);
		OrderRootPtr->SetLocalMessageTime(NewReportPtr->GetLocalMessageTime());
		OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		FOrderStore->AddOrderRoot(OrderRootPtr);
		FOrderStore->InsertOrderRootNIDIndex(OrderRootPtr);
		FOrderStore->InsertOrderRootOrderIDIndex(OrderRootPtr);
		FOrderStore->InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
		if (OrderRootPtr->GetLeavesQty() <= 0)
		{
			OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
			FOrderStore->InsertCanceledOrderRootTimeOrderIDIndex(OrderRootPtr);
		}
		else
		{
			FOrderStore->InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
			FOrderStore->InsertOrderRootSymbolSidePriceIndex(OrderRootPtr);
		}

		if ((orderSymbol.Length() > 0) && (orderSymbol != tradeSymbol))
			OrderRootPtr->SetSymbol(orderSymbol);
	}
	else
	{  //Order Root Exist
		OrderRootStatus = OrderRootPtr->GetOrderStatus();
		if ((OrderRootStatus == nsOrderMessageDefine::osCanceled) ||
			(OrderRootStatus == nsOrderMessageDefine::osFilled) ||
			((RptExecType != nsOrderMessageDefine::etOrderStatus) &&
			 (OrderRootStatus == nsOrderMessageDefine::osRejected)))
		{
			if ((!FIsRecovering) && (FOrderStore->OnErrorMessage != 0))
			{
				SYSTEMTIME SysTime;

				GetLocalTime( &SysTime );
				FOrderStore->OnErrorMessage(FOrderStore, SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds,
											osErrNotCorrectMessage, MsgOrderStatus,
											RptOrderId, RptNID, orderSymbol, NewReportPtr->GetSide(),
											NewReportPtr->GetPrice(), NewReportPtr->GetOrderQty());
            }
			delete NewReportPtr;
			return;
		}

		String OrderRootOrderId = OrderRootPtr->GetOrderID();
		if (((OrderRootOrderId.Length() <= 0) || (OrderRootOrderId == L"00000")) &&
			((RptOrderId.Length() > 0) && (RptOrderId != L"00000")))
		{
			OrderRootPtr->SetOrderID(RptOrderId);
			FOrderStore->InsertOrderRootOrderIDIndex(OrderRootPtr);
		}

		if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)
		{
			if (OrderRootPtr->GetTimeOrderIDKey().Length() > 0)
			{
				FOrderStore->RemoveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
				FOrderStore->RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
			}

			String EmptyStr = L"";
			OrderRootPtr->SetLocalMessageTime(EmptyStr);
			OrderRootPtr->SetTimeOrderIDKey(EmptyStr);
		}

		if (OrderRootPtr->GetLocalMessageTime().Length() <= 0)
			OrderRootPtr->SetLocalMessageTime(NewReportPtr->GetLocalMessageTime());

		if (OrderRootPtr->GetTimeOrderIDKey().Length() <= 0)
		{
			OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
			FOrderStore->InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
		}

		if( (IsTaifex == true && MsgTMPExecType == nsOrderMessageDefine::tetReplaced) ||
			(IsTaifex == false && OrderRootPtr->GetOrderQty() != NewReportPtr->GetOrderQty()) )
		{   //Replace Quantity
			if ((OrderRootStatus != nsOrderMessageDefine::osReplaced) ||
				(OrderRootPtr->GetLeavesQty() != NewReportPtr->GetLeavesQty()))
			{
				IsOrderStatusChanged = true;
				OrderRootPtr->SetLeavesQty(NewReportPtr->GetLeavesQty());
				if( OrderRootPtr->GetOrderQty() > NewReportPtr->GetOrderQty())
					OrderRootPtr->SetOrderQty(OrderRootPtr->GetOrderQty() - NewReportPtr->GetOrderQty());
				else
					OrderRootPtr->SetOrderQty(NewReportPtr->GetLeavesQty());

				if (OrderRootPtr->GetLeavesQty() <= 0)
				{
					FOrderStore->RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
					OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
					FOrderStore->InsertCanceledOrderRootTimeOrderIDIndex(OrderRootPtr);
				}
				else
				{
					FOrderStore->InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
					OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osReplaced);
				}

				NewReportPtr->SetPrice(OrderRootPtr->GetPrice());
				NewReportPtr->SetLastPx(OrderRootPtr->GetLastPx());
			}
		}
		else
		{   //Replace Price
			IsOrderStatusChanged = true;
			FOrderStore->RemoveOrderRootSymbolSidePriceIndex(OrderRootPtr);
			if (!OrderRootPtr->GetSymbol()->IsMultileg())
				FOrderStore->ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), OrderRootPtr->GetExchangeCode(),
													 orderSymbol, OrderRootPtr->GetSide(), OrderRootPtr->GetPrice());

			OrderRootPtr->SetPrice(NewReportPtr->GetPrice());
			OrderRootPtr->SetOrderType(NewReportPtr->GetOrderType());
			OrderRootPtr->SetTimeInForce(NewReportPtr->GetTimeInForce());
			OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osReplaced);
			FOrderStore->InsertOrderRootSymbolSidePriceIndex(OrderRootPtr);
		}  //if (MsgTMPExecType == nsOrderMessageDefine::tetReplaced)

		OrderRootPtr->SetStatusCode(NewReportPtr->GetStatusCode());
		OrderRootPtr->SetText(NewReportPtr->GetText());
	}  //if (OrderRootPtr == 0)

	OrderRootPtr->SetTMPExecType(MsgTMPExecType);
	if ((OrderRootPtr->GetDisplayName().Length() <= 0) &&
		(NewReportPtr->GetDisplayName().Length() > 0))
		OrderRootPtr->SetDisplayName(NewReportPtr->GetDisplayName());

	FOrderStore->AddOrderMessage(NewReportPtr);
	FOrderStore->InsertOrderMessageExecIDIndex(NewReportPtr);
	if (OrderRootPtr != 0)
	{
		NewReportPtr->SetRootNID(OrderRootPtr->GetNID());
		OrderRootPtr->InsertOrderMessage(NewReportPtr);
//		OrderRootPtr->InsertOrderMessageExecIDIndex(NewReportPtr);
	}

	if (IsOrderStatusChanged)
	{
		if (!OrderRootPtr->GetSymbol()->IsMultileg())
			FOrderStore->ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), OrderRootPtr->GetExchangeCode(),
												 orderSymbol, OrderRootPtr->GetSide(), OrderRootPtr->GetPrice());

		if (FOrderStore->OnOrderStatusChange != 0)
			FOrderStore->OnOrderStatusChange(FOrderStore);
	}

	NewReportPtr->SetStopPrice(OrderRootPtr->GetStopPrice());
	NewReportPtr->SetStrategyName(OrderRootPtr->GetStrategyName());
	NewReportPtr->WriteToOrderLogStream(FOrderStore->GetOrderMsgFileStream());
//	FOrderStore->ExecuteOrderStatusListener(OrderRootPtr->GetMarket(), RptOrderId);
	if (!FIsRecovering) FOrderStore->ExecuteOrderMessageListener(OrderRootPtr);
}  //TOrderAdapter::ProcessReplacedMsg()

//---------------------------------------------------------------------------
void TOrderAdapter::ProcessCanceledMsg(nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus,TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr)
{
	UFCType::Int64 RptNID =  NewReportPtr->GetNID();
	String logMessage,contractFullSymbol,LocalMessageTimeStr;
	String exchangeCode = NewReportPtr->GetExchangeCode();
	String orderSymbol = NewReportPtr->GetSymbol()->GetOriginalSymbol();
	String tradeSymbol = NewReportPtr->GetTradeSymbol();
	String RptOrderId( NewReportPtr->GetOrderID());

	GetLocalSystemTime( LocalMessageTimeStr, true );
	if (orderSymbol != tradeSymbol)
		contractFullSymbol.printf(L"%s-%s(%s)", exchangeCode, orderSymbol, tradeSymbol);
	else
		contractFullSymbol.printf(L"%s-%s", exchangeCode, orderSymbol);

	if ((RptOrderId.Length() <= 0) || (RptOrderId == L"00000"))
	{
		if ((!FIsRecovering) && (FOrderStore->OnErrorMessage != 0))
		{
			SYSTEMTIME SysTime;

			GetLocalTime( &SysTime );
			FOrderStore->OnErrorMessage(FOrderStore, SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds,
										osErrNotCorrectMessage, MsgOrderStatus,
										RptOrderId, RptNID, orderSymbol, NewReportPtr->GetSide(),
										NewReportPtr->GetPrice(), NewReportPtr->GetOrderQty());
		}
		delete NewReportPtr;
		return;
	}

	//Set Local Message Time
	String TransactTimeStr = NewReportPtr->GetTransactTime();
	String TimeOrderIDKey;
	if (FIsRecovering && (TransactTimeStr.Length() > 0))
	{
		NewReportPtr->SetLocalMessageTime(TransactTimeStr);
		TimeOrderIDKey = TransactTimeStr + RptOrderId;
	}
	else
	{
		NewReportPtr->SetLocalMessageTime(LocalMessageTimeStr);
		TimeOrderIDKey = LocalMessageTimeStr + RptOrderId;
	}

	NewReportPtr->SetTimeOrderIDKey(TimeOrderIDKey);
	String rptExecId = NewReportPtr->GetExecID();
	if (rptExecId.Length() <= 0)
	{
		rptExecId = TimeOrderIDKey;
		NewReportPtr->SetExecID(rptExecId);
	}

	if (FOrderStore->FindOrderMessageByExecID(rptExecId) != 0)
	{
		logMessage.printf(L"Duplicate Exec ID[%s], Skip this Message.", rptExecId);
		UFC::BufferedLog::Printf(" TOrderAdapter::ProcessCanceledMsg() %s",
								 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
		UFC::BufferedLog::FlushToFile();
		return;
	}

	//Get Order Root
	String OrderRootOrderId = L"";
	TExecution *OrderRootPtr = FindOrderRootByOrderId_NID(RptOrderId, RptNID);
	nsOrderMessageDefine::OrderStatusEnum OrderRootStatus = nsOrderMessageDefine::osNone;
//	nsOrderMessageDefine::ExecTypeEnum RptExecType = NewReportPtr->GetExecType();
	bool isOrderRootExist = false;
	bool IsOrderStatusChanged = false;
	int leavesQty = 0;
	if (OrderRootPtr == 0)
	{   //No Order Root Exists, Create a New Order Root
		IsOrderStatusChanged = true;
		OrderRootPtr = new TExecution(RecvMsg);
		OrderRootPtr->SetMarket(NewReportPtr->GetMarket());
		OrderRootPtr->SetRoot(true);
		OrderRootPtr->SetLocalMessageTime(NewReportPtr->GetLocalMessageTime());
		OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		if ((orderSymbol.Length() > 0) && (orderSymbol != tradeSymbol))
			OrderRootPtr->SetSymbol(orderSymbol);
		FOrderStore->AddOrderRoot(OrderRootPtr);
		FOrderStore->InsertOrderRootNIDIndex(OrderRootPtr);
		FOrderStore->InsertOrderRootOrderIDIndex(OrderRootPtr);
		FOrderStore->InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
		FOrderStore->InsertCanceledOrderRootTimeOrderIDIndex(OrderRootPtr);
	}
	else
	{   //Order Root Exist
		isOrderRootExist = true;
		leavesQty = OrderRootPtr->GetLeavesQty();
		OrderRootStatus = OrderRootPtr->GetOrderStatus();
		if (OrderRootStatus == nsOrderMessageDefine::osFilled)
		{
			if ((!FIsRecovering) && (FOrderStore->OnErrorMessage != 0))
			{
				SYSTEMTIME SysTime;

				GetLocalTime( &SysTime );
				FOrderStore->OnErrorMessage(FOrderStore, SysTime.wHour, SysTime.wMinute, SysTime.wSecond, SysTime.wMilliseconds,
											osErrNotCorrectMessage, MsgOrderStatus,
											RptOrderId, RptNID, orderSymbol, NewReportPtr->GetSide(),
											NewReportPtr->GetPrice(), NewReportPtr->GetOrderQty());
            }
			delete NewReportPtr;
			return;
		}

		OrderRootOrderId = OrderRootPtr->GetOrderID();
		if (((OrderRootOrderId.Length() <= 0) || (OrderRootOrderId == L"00000")) &&
			((RptOrderId.Length() > 0) && (RptOrderId != L"00000")))
		{
			OrderRootPtr->SetOrderID(RptOrderId);
			FOrderStore->InsertOrderRootOrderIDIndex(OrderRootPtr);
		}

		if (OrderRootStatus != nsOrderMessageDefine::osCanceled)
		{
			IsOrderStatusChanged = true;
			if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)
			{
				if (OrderRootPtr->GetTimeOrderIDKey().Length() > 0)
				{
					FOrderStore->RemoveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
					FOrderStore->RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
				}

				String EmptyStr = L"";
				OrderRootPtr->SetLocalMessageTime(EmptyStr);
				OrderRootPtr->SetTimeOrderIDKey(EmptyStr);
			}

			if (OrderRootPtr->GetLocalMessageTime().Length() <= 0)
				OrderRootPtr->SetLocalMessageTime(NewReportPtr->GetLocalMessageTime());

			if (OrderRootPtr->GetTimeOrderIDKey().Length() <= 0)
			{
				OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
				FOrderStore->InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
			}

			if ((OrderRootStatus == nsOrderMessageDefine::osNew) ||
				(OrderRootStatus == nsOrderMessageDefine::osRejected) ||
				(OrderRootStatus == nsOrderMessageDefine::osReplaced) ||
				(OrderRootStatus == nsOrderMessageDefine::osPartiallyFilled))
				FOrderStore->RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());

			FOrderStore->InsertCanceledOrderRootTimeOrderIDIndex(OrderRootPtr);
			OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osCanceled);
		}  //if (OrderRootStatus != nsOrderMessageDefine::osCanceled)

		OrderRootPtr->SetStatusCode(NewReportPtr->GetStatusCode());
		OrderRootPtr->SetText(NewReportPtr->GetText());
		if (OrderRootPtr->GetSymbol()->IsMultileg())
		{   ///< Handle Option Strategy Order
			UFC::AnsiString strategySymbol = s888::ConvertUnicodeToUFCAnsiStr(tradeSymbol);
			FOrderStore->ExecuteStrategyOrderListener(strategySymbol.c_str(), OrderRootPtr->GetSide(), OrderRootPtr->GetLastPx(), OrderRootPtr->GetLastQty(), OrderRootPtr->GetLeavesQty());
		}  //if (OrderRootPtr->GetSymbol()->IsMultileg())
	}  //if (OrderRootPtr == 0)
	if( /*RecvMsg->GetTMPStstusCode() == 47 ||*/ RecvMsg->GetTMPStstusCode() == 48 )
	{
		OrderRootPtr->SetText( "交易所價格穩定機制退單" );
		NewReportPtr->SetText( "交易所價格穩定機制退單" );
	}
	FOrderStore->AddOrderMessage(NewReportPtr);
	FOrderStore->InsertOrderMessageExecIDIndex(NewReportPtr);
	NewReportPtr->SetRootNID(OrderRootPtr->GetNID());
	OrderRootPtr->InsertOrderMessage(NewReportPtr);
//	OrderRootPtr->InsertOrderMessageExecIDIndex(NewReportPtr);
	if (IsOrderStatusChanged)
	{
		if (!OrderRootPtr->GetSymbol()->IsMultileg())
			FOrderStore->ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), OrderRootPtr->GetExchangeCode(),
												 orderSymbol, OrderRootPtr->GetSide(), OrderRootPtr->GetPrice());

		if (FOrderStore->OnOrderStatusChange != 0)
			FOrderStore->OnOrderStatusChange(FOrderStore);

		if (isOrderRootExist && FOrderStore->IsAutoPositionEffect() && (leavesQty > 0) &&
			((OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peClose) ||
			 (OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)))
		{
			String positionMsg = L"";
			s888::CTradingAccount *accountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex(FOrderStore->ID, s888::rwLockForRead);
			if (accountPtr != 0)
			{
				s888::CContractPosition *positionPtr = accountPtr->FindContractPositionFromSymbolIndex(exchangeCode, tradeSymbol, s888::rwLockForRead);
				if (positionPtr != 0)
				{
					positionPtr->LockForWrite();
					positionPtr->SubstractOrderQuantity(OrderRootPtr->GetPositionEffect(), OrderRootPtr->GetSide(), leavesQty, s888::rwNotLock);
					logMessage.printf(L" Cancel Report[%s] NID[%lld] [%s] side[%s] pe[%s] Qty[%d] leavesQty[%d].",
									  OrderRootOrderId, RptNID, contractFullSymbol,
									  s888::DescribeBuySell(OrderRootPtr->GetSide()),
									  s888::DescribePositionEffect(OrderRootPtr->GetPositionEffect()),
									  OrderRootPtr->GetOrderQty(), OrderRootPtr->GetLeavesQty());
					UFC::BufferedLog::Printf(" TOrderAdapter::ProcessCanceledMsg() %s",
											 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
					positionPtr->DumpData(s888::constFlush, s888::rwNotLock);
					positionPtr->UnlockForWrite();
				}
				else
					positionMsg.printf(L"Account[%s] Contract Position[%s] not found.", FOrderStore->ID, contractFullSymbol);
			}
			else
				positionMsg.printf(L"Account[%s] not Found.", FID);

			if (positionMsg.Length() > 0)
			{
				UFC::BufferedLog::Printf(" TOrderAdapter::ProcessCanceledMsg() %s", s888::ConvertUnicodeToUFCAnsiStr(positionMsg).c_str());
				UFC::BufferedLog::FlushToFile();
			}
		}  //if (FIsAutoPositionEffect && (leavesQty > 0) &&
		FOrderStore->increaseCancelReportCount();
		FOrderStore->AddCancelStatistic(exchangeCode, orderSymbol, leavesQty);
	}

	NewReportPtr->SetStopPrice(OrderRootPtr->GetStopPrice());
	NewReportPtr->SetStrategyName(OrderRootPtr->GetStrategyName());
	NewReportPtr->WriteToOrderLogStream(FOrderStore->GetOrderMsgFileStream());
//	FOrderStore->ExecuteOrderStatusListener(OrderRootPtr->GetMarket(), RptOrderId);
	if (!FIsRecovering) FOrderStore->ExecuteOrderMessageListener(OrderRootPtr);
	ProcessStrategyOrder(RecvMsg, OrderRootPtr);
	if (FOrderStore->OnCancelStatistic != 0)
		FOrderStore->OnCancelStatistic(FOrderStore, exchangeCode, orderSymbol, leavesQty);
}  //TOrderAdapter::ProcessCanceledMsg()

//---------------------------------------------------------------------------
void TOrderAdapter::ProcessRejectedMsg( nsOrderMessageDefine::OrderStatusEnum MsgOrderStatus, TExecutionReportMessage *RecvMsg, TExecution *NewReportPtr )
{
	UFCType::Int64 RptNID =  NewReportPtr->GetNID();
	String logMessage,contractFullSymbol,LocalMessageTimeStr;
	UnicodeString exchangeCode = NewReportPtr->GetExchangeCode();
	String orderSymbol = NewReportPtr->GetSymbol()->GetOriginalSymbol();
	String tradeSymbol = NewReportPtr->GetTradeSymbol();
	String RptOrderId( NewReportPtr->GetOrderID());

	GetLocalSystemTime( LocalMessageTimeStr, true );
	if (orderSymbol != tradeSymbol)
		contractFullSymbol.printf(L"%s-%s(%s)", exchangeCode, orderSymbol, tradeSymbol);
	else
		contractFullSymbol.printf(L"%s-%s", exchangeCode, orderSymbol);

	String IdStr;
	if ((RptOrderId.Length() <= 0) || (RptOrderId == L"00000"))
		IdStr.printf(L"%lld", RptNID);
	else
		IdStr = RptOrderId;

	//Set Local Message Time
	UnicodeString TransactTimeStr = NewReportPtr->GetTransactTime();
	UnicodeString TimeOrderIDKey;
	if (FIsRecovering && (TransactTimeStr.Length() > 0))
	{
		NewReportPtr->SetLocalMessageTime(TransactTimeStr);
		TimeOrderIDKey = TransactTimeStr + IdStr;
	}
	else
	{
		NewReportPtr->SetLocalMessageTime(LocalMessageTimeStr);
		TimeOrderIDKey = LocalMessageTimeStr + IdStr;
	}

	NewReportPtr->SetTimeOrderIDKey(TimeOrderIDKey);
	String rptExecId = NewReportPtr->GetExecID();
	if (rptExecId.Length() <= 0)
	{
		rptExecId = TimeOrderIDKey;
		NewReportPtr->SetExecID(rptExecId);
	}

	if (FOrderStore->FindOrderMessageByExecID(rptExecId) != 0)
	{
		logMessage.printf(L"Duplicate Exec ID[%s], Skip this Message.", rptExecId);
		UFC::BufferedLog::Printf(" TOrderAdapter::ProcessRejectedMsg() %s",
								 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
		UFC::BufferedLog::FlushToFile();
		return;
	}

	//Get Order Root
	TExecution* OrderRootPtr = FindOrderRootByOrderId_NID(RptOrderId, RptNID);
	nsOrderMessageDefine::OrderStatusEnum OrderRootStatus = nsOrderMessageDefine::osNone;
	bool isOrderRootExist = false;
	bool IsOrderStatusChanged = false;
	if (OrderRootPtr == 0)
	{   //No Order Root Exists, Create a New Order Root
		logMessage.printf(L"Find no Order Root By OrderID[%s], NID[%lld].", RptOrderId, RptNID);
		UFC::BufferedLog::Printf(" TOrderAdapter::ProcessRejectedMsg() %s",
						 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
		IsOrderStatusChanged = true;
		OrderRootPtr = new TExecution(RecvMsg);
		OrderRootPtr->SetMarket(NewReportPtr->GetMarket());
		OrderRootPtr->SetRoot(true);
		OrderRootPtr->SetLocalMessageTime(LocalMessageTimeStr);
		OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
		OrderRootPtr->SetExecID(NewReportPtr->GetExecID());
		if ((orderSymbol.Length() > 0) && (orderSymbol != tradeSymbol))
			OrderRootPtr->SetSymbol(orderSymbol);
		FOrderStore->AddOrderRoot(OrderRootPtr);
		FOrderStore->InsertOrderRootNIDIndex(OrderRootPtr);
		FOrderStore->InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
		FOrderStore->InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
		if ((RptOrderId.Length() > 0) && (RptOrderId != L"00000"))
			FOrderStore->InsertOrderRootOrderIDIndex(OrderRootPtr);
	}
	else
	{   //Order Root Exist
		String rootExchangeCode = OrderRootPtr->GetExchangeCode();
		if ((exchangeCode.Length() <= 0) || (exchangeCode != rootExchangeCode))
			exchangeCode = rootExchangeCode;
		String rootSymbol = OrderRootPtr->GetSymbol()->GetOriginalSymbol();
		if ((orderSymbol.Length() <= 0) || (orderSymbol != rootSymbol))
			orderSymbol = rootSymbol;
		String rootTradeSymbol = OrderRootPtr->GetTradeSymbol();
		if ((tradeSymbol.Length() <= 0) || (tradeSymbol != rootTradeSymbol))
			tradeSymbol = rootTradeSymbol;
		if (orderSymbol != tradeSymbol)
			contractFullSymbol.printf(L"%s-%s(%s)", exchangeCode, orderSymbol, tradeSymbol);
		else
			contractFullSymbol.printf(L"%s-%s", exchangeCode, orderSymbol);
		logMessage.printf(L"Find Order Root[%s] By OrderID[%s], NID[%lld] [%s] Qty[%d] leavesQty[%d] CxlRejResponseTo[%d] pe[%d].",
		OrderRootPtr->GetOrderID(), RptOrderId, RptNID, contractFullSymbol, OrderRootPtr->GetOrderQty(), OrderRootPtr->GetLeavesQty(), RecvMsg->GetCxlRejResponseTo(), OrderRootPtr->GetPositionEffect());
		UFC::BufferedLog::Printf(" TOrderAdapter::ProcessRejectedMsg() %s",
						 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
		isOrderRootExist = true;
		OrderRootStatus = OrderRootPtr->GetOrderStatus();
		String OrderRootOrderId = OrderRootPtr->GetOrderID();
		if (((OrderRootOrderId.Length() <= 0) || (OrderRootOrderId == L"00000")) &&
			((RptOrderId.Length() > 0) && (RptOrderId != L"00000")))
		{
			OrderRootPtr->SetOrderID(RptOrderId);
			FOrderStore->InsertOrderRootOrderIDIndex(OrderRootPtr);
		}

		NewReportPtr->SetExchangeCode(exchangeCode);
		NewReportPtr->SetMarket(OrderRootPtr->GetMarket());
		NewReportPtr->SetSymbol(orderSymbol);
		NewReportPtr->SetTradeSymbol(tradeSymbol);
		NewReportPtr->SetSide(OrderRootPtr->GetSide());
		NewReportPtr->SetPrice(OrderRootPtr->GetPrice());
		NewReportPtr->SetOrderQty(OrderRootPtr->GetOrderQty());
		NewReportPtr->SetOrderType(OrderRootPtr->GetOrderType());
		NewReportPtr->SetTimeInForce(OrderRootPtr->GetTimeInForce());
		if ((OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peAuto) ||
			(OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peAutoToday))
			OrderRootPtr->SetPositionEffect(NewReportPtr->GetPositionEffect());
		else
			NewReportPtr->SetPositionEffect(OrderRootPtr->GetPositionEffect());
		if (OrderRootStatus != nsOrderMessageDefine::osRejected)
		{
			IsOrderStatusChanged = true;
			if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)
			{
				if (OrderRootPtr->GetTimeOrderIDKey().Length() > 0)
				{
					FOrderStore->RemoveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
					FOrderStore->RemoveActiveOrderRootByTimeOrderID(OrderRootPtr->GetTimeOrderIDKey());
				}

				UnicodeString EmptyStr = L"";
				OrderRootPtr->SetLocalMessageTime(EmptyStr);
				OrderRootPtr->SetTimeOrderIDKey(EmptyStr);
			}  //if (OrderRootStatus == nsOrderMessageDefine::osPendingNew)

			if (OrderRootPtr->GetLocalMessageTime().Length() <= 0)
				OrderRootPtr->SetLocalMessageTime(NewReportPtr->GetLocalMessageTime());

			if (OrderRootPtr->GetTimeOrderIDKey().Length() <= 0)
			{
				OrderRootPtr->SetTimeOrderIDKey(TimeOrderIDKey);
				FOrderStore->InsertOrderRootTimeOrderIDIndex(OrderRootPtr);
				FOrderStore->InsertActiveOrderRootTimeOrderIDIndex(OrderRootPtr);
			}
		}  //if (OrderRootStatus != nsOrderMessageDefine::osRejected)

		int RptStatusCode = NewReportPtr->GetStatusCode();
		String ReportText = UTF8String( RecvMsg->GetText() );
		if ((OrderRootStatus != nsOrderMessageDefine::osCanceled) &&
			(OrderRootStatus != nsOrderMessageDefine::osFilled) &&
			(OrderRootStatus != nsOrderMessageDefine::osRejected) &&
			(RecvMsg->GetCxlRejResponseTo() == nsOrderMessageDefine::crrNew) )
			OrderRootPtr->SetOrderStatus(nsOrderMessageDefine::osRejected);

		OrderRootPtr->SetStatusCode(RptStatusCode);
		OrderRootPtr->SetText(ReportText);
	}  //if (OrderRootPtr == 0)

	nsOrderMessageDefine::PositionEffectEnum OrderRootPE = OrderRootPtr->GetPositionEffect();
	if ((OrderRootPE == nsOrderMessageDefine::peAuto) ||
		(OrderRootPE == nsOrderMessageDefine::peAutoToday))
	{
		nsOrderMessageDefine::PositionEffectEnum MsgPE = RecvMsg->GetPositionEffect();
		if ((MsgPE == nsOrderMessageDefine::peOpen) ||
			(MsgPE == nsOrderMessageDefine::peClose) ||
			(MsgPE == nsOrderMessageDefine::peDayTrade))
		{
			OrderRootPtr->SetPositionEffect(MsgPE);
		}
	}

	if ((OrderRootPtr->GetDisplayName().Length() <= 0) &&
		(NewReportPtr->GetDisplayName().Length() > 0))
		OrderRootPtr->SetDisplayName(NewReportPtr->GetDisplayName());

	FOrderStore->AddOrderMessage(NewReportPtr);
	FOrderStore->InsertOrderMessageExecIDIndex(NewReportPtr);
	NewReportPtr->SetRootNID(OrderRootPtr->GetNID());
	OrderRootPtr->InsertOrderMessage(NewReportPtr);
//	OrderRootPtr->InsertOrderMessageExecIDIndex(NewReportPtr);
	if (IsOrderStatusChanged)
	{
		if (((OrderRootStatus != nsOrderMessageDefine::osNew) ||
			 (OrderRootStatus != nsOrderMessageDefine::osPartiallyFilled)) &&
			(!OrderRootPtr->GetSymbol()->IsMultileg()))
		{
			FOrderStore->ExecuteOrderQtyListener(OrderRootPtr->GetMarket(), OrderRootPtr->GetExchangeCode(),
												 orderSymbol, OrderRootPtr->GetSide(), OrderRootPtr->GetPrice());
		}

		if (FOrderStore->OnOrderStatusChange != 0)
			FOrderStore->OnOrderStatusChange(FOrderStore);

		int leavesQty = OrderRootPtr->GetLeavesQty();
		if (isOrderRootExist && FOrderStore->IsAutoPositionEffect() &&
			(RecvMsg->GetCxlRejResponseTo() != nsOrderMessageDefine::crrCancel) &&
			(leavesQty > 0) &&
			((OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peClose) ||
			 (OrderRootPtr->GetPositionEffect() == nsOrderMessageDefine::peDayTrade)))
		{
			String positionMsg = L"";
			s888::CTradingAccount *accountPtr = s888::gTradingObjsPool.FindAccountFromLogonIdIndex(FOrderStore->ID, s888::rwLockForRead);
			if (accountPtr != 0)
			{
				s888::CContractPosition *positionPtr = accountPtr->FindContractPositionFromSymbolIndex(exchangeCode, tradeSymbol, s888::rwLockForRead);
				if (positionPtr != 0)
				{
					positionPtr->LockForWrite();
					positionPtr->SubstractOrderQuantity(OrderRootPtr->GetPositionEffect(), OrderRootPtr->GetSide(), leavesQty, s888::rwNotLock);
					logMessage.printf(L" Reject Report[%s] NID[%lld] [%s] side[%s] pe[%s] Qty[%d] leavesQty[%d].",
									  RptOrderId, RptNID, contractFullSymbol,
									  s888::DescribeBuySell(OrderRootPtr->GetSide()),
									  s888::DescribePositionEffect(OrderRootPtr->GetPositionEffect()),
									  OrderRootPtr->GetOrderQty(), OrderRootPtr->GetLeavesQty());
					UFC::BufferedLog::Printf(" TOrderAdapter::ProcessRejectedMsg() %s",
											 s888::ConvertUnicodeToUFCAnsiStr(logMessage).c_str());
					positionPtr->DumpData(s888::constFlush, s888::rwNotLock);
					positionPtr->UnlockForWrite();
				}
				else
					positionMsg.printf(L"Account[%s] Contract Position[%s] not found.", FOrderStore->ID, contractFullSymbol);
			}
			else
				positionMsg.printf(L"Account[%s] not Found.", FID);

			if (positionMsg.Length() > 0)
			{
				UFC::BufferedLog::Printf(" TOrderAdapter::ProcessRejectedMsg() %s", s888::ConvertUnicodeToUFCAnsiStr(positionMsg).c_str());
				UFC::BufferedLog::FlushToFile();
			}
		}  //if (FIsAutoPositionEffect && (leavesQty > 0) &&

		if (RecvMsg->GetCxlRejResponseTo() == nsOrderMessageDefine::crrCancel) FOrderStore->increaseCancelReportCount();
	}  //if (IsOrderStatusChanged)

	if (FOrderStore->OnRejectedReply != NULL)
		FOrderStore->OnRejectedReply( FOrderStore );

	NewReportPtr->SetStopPrice(OrderRootPtr->GetStopPrice());
	NewReportPtr->SetStrategyName(OrderRootPtr->GetStrategyName());
	NewReportPtr->WriteToOrderLogStream(FOrderStore->GetOrderMsgFileStream());
//	FOrderStore->ExecuteOrderStatusListener(OrderRootPtr->GetMarket(), orderSymbol);
	if (!FIsRecovering)
		FOrderStore->ExecuteOrderMessageListener(OrderRootPtr);

	ProcessStrategyOrder(RecvMsg, OrderRootPtr);
}  //TOrderAdapter::ProcessRejectedMsg()
//---------------------------------------------------------------------------
