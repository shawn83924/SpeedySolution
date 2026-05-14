// ---------------------------------------------------------------------------
// ExecutionReportMessageImpl.h : Declaration of the TExecutionReportMessageImpl
// ---------------------------------------------------------------------------
#ifndef ExecutionReportMessageImplH
#define ExecutionReportMessageImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "SpeedyAPI_TLB.h"
#include <TExecutionReportMessage.h>

// ---------------------------------------------------------------------------
// TExecutionReportMessageImpl     Implements IExecutionReportMessage, default interface of ExecutionReportMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TExecutionReportMessageImpl : public TCppAutoObject<IExecutionReportMessage>
{
  typedef _COM_CLASS inherited;
public:
	TExecutionReportMessage FMessage;
public:
  __fastcall TExecutionReportMessageImpl();
  __fastcall TExecutionReportMessageImpl( TExecutionReportMessage* );
  __fastcall TExecutionReportMessageImpl(const System::_di_IInterface Controller);
  __fastcall TExecutionReportMessageImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
public:
  void DetachMessage( void );
  // IExecutionReportMessage
private:
  bool RemoveProxyUserData( const UFC::AnsiString& UserData, UFC::AnsiString& Data );
protected:
  STDMETHOD(get_Account(BSTR* Value));
  STDMETHOD(get_AccountFlag(BSTR* Value));
  STDMETHOD(get_AE(BSTR* Value));
  STDMETHOD(get_BeforeQty(long* Value));
  STDMETHOD(get_BrokerID(BSTR* Value));
  STDMETHOD(get_ClOrdID(BSTR* Value));
  STDMETHOD(get_ConnectionID(long* Value));
  STDMETHOD(get_CumQty(long* Value));
  STDMETHOD(get_CxlRejResponseTo(CxlRejResponseToEnum* Value));
  STDMETHOD(get_Data(BSTR* Value));
  STDMETHOD(get_EUREX2TAIFEX(BSTR* Value));
  STDMETHOD(get_EventType(EventTypeEnum* Value));
  STDMETHOD(get_ExchangeCode(BSTR* Value));
  STDMETHOD(get_ExecID(BSTR* Value));
  STDMETHOD(get_ExecRestatementReason(ExecRestatementReasonEnum* Value));
  STDMETHOD(get_ExecTransType(ExecTransTypeEnum* Value));
  STDMETHOD(get_ExecType(ExecTypeEnum* Value));
  STDMETHOD(get_GatewayData(BSTR* Value));
  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_LastPx(double* Value));
  STDMETHOD(get_LastQty(long* Value));
  STDMETHOD(get_LeavesQty(long* Value));
  STDMETHOD(get_LegPrice1(double* Value));
  STDMETHOD(get_LegPrice2(double* Value));
  STDMETHOD(get_LegQty1(long* Value));
  STDMETHOD(get_LegQty2(long* Value));
  STDMETHOD(get_LegSide1(SideEnum* Value));
  STDMETHOD(get_LegSide2(SideEnum* Value));
  STDMETHOD(get_LegSymbol1(BSTR* Value));
  STDMETHOD(get_LegSymbol2(BSTR* Value));
  STDMETHOD(get_LINBRN(BSTR* Value));
  STDMETHOD(get_Market(MarketEnum* Value));
  STDMETHOD(get_MaturityMonthYear(BSTR* Value));
  STDMETHOD(get_MessageType(MessageTypeEnum* Value));
  STDMETHOD(get_NID(__int64* Value));
  STDMETHOD(get_OrderID(BSTR* Value));
  STDMETHOD(get_OrderQty(long* Value));
  STDMETHOD(get_OrderStatus(OrderStatusEnum* Value));
  STDMETHOD(get_OrderType(OrderTypeEnum* Value));
  STDMETHOD(get_OrdRejReason(OrdRejReasonEnum* Value));
  STDMETHOD(get_OrigClOrdID(BSTR* Value));
  STDMETHOD(get_PositionEffect(PositionEffectEnum* Value));
  STDMETHOD(get_Price(double* Value));
  STDMETHOD(get_PriceBase(PriceBaseEnum* Value));
  STDMETHOD(get_ReportSeq(long* Value));
  STDMETHOD(get_Side(SideEnum* Value));
  STDMETHOD(get_Src(BSTR* Value));
  STDMETHOD(get_StopPx(double* Value));
  STDMETHOD(get_StrikePrice(double* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_Text(BSTR* Value));
  STDMETHOD(get_TimeInForce(TimeInForceEnum* Value));
  STDMETHOD(get_TMPExecType(TMPExecTypeEnum* Value));
  STDMETHOD(get_TMPSessionID(long* Value));
  STDMETHOD(get_TMPUniqueID(long* Value));
  STDMETHOD(get_TradingSessionID(TradingSessionIDEnum* Value));
  STDMETHOD(get_TransactTime(BSTR* Value));
  STDMETHOD(get_TSEExchangeCode(BSTR* Value));
  STDMETHOD(get_TSEObjectBroker(BSTR* Value));
  STDMETHOD(get_TSEOrderType(BSTR* Value));
  STDMETHOD(get_TSETradeKind(BSTR* Value));
  STDMETHOD(get_Value(BSTR Name, BSTR* Value));
  STDMETHOD(set_Account(BSTR Value));
  STDMETHOD(set_AccountFlag(BSTR Value));
  STDMETHOD(set_AE(BSTR Value));
  STDMETHOD(set_BeforeQty(long Value));
  STDMETHOD(set_ClOrdID(BSTR Value));
  STDMETHOD(set_CumQty(long Value));
  STDMETHOD(set_CxlRejResponseTo(CxlRejResponseToEnum Value));
  STDMETHOD(set_Data(BSTR Value));
  STDMETHOD(set_EventType(EventTypeEnum Value));
  STDMETHOD(set_ExchangeCode(BSTR Value));
  STDMETHOD(set_ExecID(BSTR Value));
  STDMETHOD(set_ExecRestatementReason(ExecRestatementReasonEnum Value));
  STDMETHOD(set_ExecTransType(ExecTransTypeEnum Value));
  STDMETHOD(set_ExecType(ExecTypeEnum Value));
  STDMETHOD(set_GatewayData(BSTR Value));
  STDMETHOD(set_LastPx(double Value));
  STDMETHOD(set_LastQty(long Value));
  STDMETHOD(set_LeavesQty(long Value));
  STDMETHOD(set_Market(MarketEnum Value));
  STDMETHOD(set_MaturityMonthYear(BSTR Value));
  STDMETHOD(set_MessageType(MessageTypeEnum Value));
  STDMETHOD(set_NID(__int64 Value));
  STDMETHOD(set_OrderID(BSTR Value));
  STDMETHOD(set_OrderQty(long Value));
  STDMETHOD(set_OrderStatus(OrderStatusEnum Value));
  STDMETHOD(set_OrderType(OrderTypeEnum Value));
  STDMETHOD(set_OrdRejReason(OrdRejReasonEnum Value));
  STDMETHOD(set_OrigClOrdID(BSTR Value));
  STDMETHOD(set_PositionEffect(PositionEffectEnum Value));
  STDMETHOD(set_Price(double Value));
  STDMETHOD(set_PriceBase(PriceBaseEnum Value));
  STDMETHOD(set_Side(SideEnum Value));
  STDMETHOD(set_StopPx(double Value));
  STDMETHOD(set_StrikePrice(double Value));
  STDMETHOD(set_Symbol(BSTR Value));
  STDMETHOD(set_Text(BSTR Value));
  STDMETHOD(set_TimeInForce(TimeInForceEnum Value));
  STDMETHOD(set_TradingSessionID(TradingSessionIDEnum Value));
  STDMETHOD(set_Value(BSTR Name, BSTR Value));
  STDMETHOD(get_OrigTransactTime(BSTR* Value));
  STDMETHOD(get_us(long* Value));
  STDMETHOD(get_ExecRefID(BSTR* Value));
  STDMETHOD(set_ExecRefID(BSTR Value));
  STDMETHOD(get_OrigExecID(BSTR* Value));
  STDMETHOD(set_OrigExecID(BSTR Value));
  STDMETHOD(get_OrderChannel(BSTR* Value));
  STDMETHOD(set_OrderChannel(BSTR Value));
  STDMETHOD(get_StatusCode(BSTR* Value));
  STDMETHOD(get_ExpireDate(BSTR* Value));
  STDMETHOD(set_ExpireDate(BSTR Value));
  STDMETHOD(get_PxDecimalNum(long* Value));
  STDMETHOD(get_TMPStatusCode(long* Value));
  STDMETHOD(get_MessageDateTime(BSTR* Value));
  STDMETHOD(get_CancelByExchange(VARIANT_BOOL* Value));
  STDMETHOD(get_IsReplacePx(VARIANT_BOOL* Value));
  STDMETHOD(get_PartID(long* Value));
  STDMETHOD(GetTouchOrderResponse(BSTR* Value));
  STDMETHOD(GetTouchOrderRespType(TouchOrderResponseTypeEnum* Value));
};




#endif //ExecutionReportMessageImplH
