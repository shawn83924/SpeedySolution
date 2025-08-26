// ---------------------------------------------------------------------------
// CancelOrderMessageImpl.h : Declaration of the TCancelOrderMessageImpl
// ---------------------------------------------------------------------------
#ifndef CancelOrderMessageImplH
#define CancelOrderMessageImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "SpeedyAPI_TLB.h"
#include <TCancelOrderMessage.h>

// ---------------------------------------------------------------------------
// TCancelOrderMessageImpl     Implements ICancelOrderMessage, default interface of CancelOrderMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : Message to cancel an order
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TCancelOrderMessageImpl : public TCppAutoObject<ICancelOrderMessage>
{
  typedef _COM_CLASS inherited;
private:
	TCancelOrderMessage FMessage;
public:
  __fastcall TCancelOrderMessageImpl();
  __fastcall TCancelOrderMessageImpl(const System::_di_IInterface Controller);
  __fastcall TCancelOrderMessageImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // ICancelOrderMessage
protected:
  STDMETHOD(get_Account(BSTR* Value));
  STDMETHOD(get_AccountFlag(BSTR* Value));
  STDMETHOD(get_AE(BSTR* Value));
  STDMETHOD(get_AENO(BSTR* Value));
  STDMETHOD(get_BrokerID(BSTR* Value));
  STDMETHOD(get_CancelNID(__int64* Value));
  STDMETHOD(get_ClOrdID(BSTR* Value));
  STDMETHOD(get_Data(BSTR* Value));
  STDMETHOD(get_EUREX2TAIFEX(BSTR* Value));
  STDMETHOD(get_EventType(EventTypeEnum* Value));
  STDMETHOD(get_ExchangeCode(BSTR* Value));
  STDMETHOD(get_GatewayData(BSTR* Value));
  STDMETHOD(get_Group(long* Value));
  STDMETHOD(get_IBNO(BSTR* Value));
  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_Market(MarketEnum* Value));
  STDMETHOD(get_MarketDataSource(BSTR* Value));
  STDMETHOD(get_MaturityMonthYear(BSTR* Value));
  STDMETHOD(get_MessageType(MessageTypeEnum* Value));
  STDMETHOD(get_NID(__int64* Value));
  STDMETHOD(get_OrderID(BSTR* Value));
  STDMETHOD(get_OrderSource(OrderSourceEnum* Value));
  STDMETHOD(get_OrderType(OrderTypeEnum* Value));
  STDMETHOD(get_OrigClOrdID(BSTR* Value));
  STDMETHOD(get_Price(double* Value));
  STDMETHOD(get_Side(SideEnum* Value));
  STDMETHOD(get_StrikePrice(double* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_TimeInForce(TimeInForceEnum* Value));
  STDMETHOD(get_TradingSessionID(TradingSessionIDEnum* Value));
  STDMETHOD(get_TSEExchangeCode(BSTR* Value));
  STDMETHOD(get_TSEObjectBroker(BSTR* Value));
  STDMETHOD(get_TSEOrderType(BSTR* Value));
  STDMETHOD(get_TSEStockSeqNo(long* Value));
  STDMETHOD(get_TSETradeKind(BSTR* Value));
  STDMETHOD(get_Value(BSTR Name, BSTR* Value));
  STDMETHOD(set_Account(BSTR Value));
  STDMETHOD(set_AccountFlag(BSTR Value));
  STDMETHOD(set_AE(BSTR Value));
  STDMETHOD(set_AENO(BSTR Value));
  STDMETHOD(set_BrokerID(BSTR Value));
  STDMETHOD(set_CancelNID(__int64 Value));
  STDMETHOD(set_ClOrdID(BSTR Value));
  STDMETHOD(set_Data(BSTR Value));
  STDMETHOD(set_EUREX2TAIFEX(BSTR Value));
  STDMETHOD(set_EventType(EventTypeEnum Value));
  STDMETHOD(set_ExchangeCode(BSTR Value));
  STDMETHOD(set_GatewayData(BSTR Value));
  STDMETHOD(set_Group(long Value));
  STDMETHOD(set_IBNO(BSTR Value));
  STDMETHOD(set_Market(MarketEnum Value));
  STDMETHOD(set_MarketDataSource(BSTR Value));
  STDMETHOD(set_MaturityMonthYear(BSTR Value));
  STDMETHOD(set_MessageType(MessageTypeEnum Value));
  STDMETHOD(set_NID(__int64 Value));
  STDMETHOD(set_OrderID(BSTR Value));
  STDMETHOD(set_OrderSource(OrderSourceEnum Value));
  STDMETHOD(set_OrderType(OrderTypeEnum Value));
  STDMETHOD(set_OrigClOrdID(BSTR Value));
  STDMETHOD(set_Price(double Value));
  STDMETHOD(set_Side(SideEnum Value));
  STDMETHOD(set_StrikePrice(double Value));
  STDMETHOD(set_Symbol(BSTR Value));
  STDMETHOD(set_TimeInForce(TimeInForceEnum Value));
  STDMETHOD(set_TradingSessionID(TradingSessionIDEnum Value));
  STDMETHOD(set_TSEExchangeCode(BSTR Value));
  STDMETHOD(set_TSEObjectBroker(BSTR Value));
  STDMETHOD(set_TSEOrderType(BSTR Value));
  STDMETHOD(set_TSEStockSeqNo(long Value));
  STDMETHOD(set_TSETradeKind(BSTR Value));
  STDMETHOD(set_Value(BSTR Name, BSTR Value));
  STDMETHOD(get_Src(BSTR* Value));
  STDMETHOD(set_Src(BSTR Value));
  STDMETHOD(get_LocalIP(BSTR* Value));
  STDMETHOD(set_LocalIP(BSTR Value));
  STDMETHOD(get_CASignature(BSTR* Value));
  STDMETHOD(set_CASignature(BSTR Value));
  STDMETHOD(get_CAPlainText(BSTR* Value));
  STDMETHOD(set_CAPlainText(BSTR Value));
  STDMETHOD(get_CASessionID(BSTR* Value));
  STDMETHOD(set_CASessionID(BSTR Value));
};




#endif //CancelOrderMessageImplH
