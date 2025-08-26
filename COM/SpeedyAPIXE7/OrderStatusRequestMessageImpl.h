// ---------------------------------------------------------------------------
// OrderStatusRequestImpl.h : Declaration of the TOrderStatusRequestImpl
// ---------------------------------------------------------------------------
#ifndef OrderStatusRequestMessageImplH
#define OrderStatusRequestMessageImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "SpeedyAPI_TLB.h"
#include <TOrderStatusRequest.h>

// ---------------------------------------------------------------------------
// TOrderStatusRequestImpl     Implements IOrderStatusRequest, default interface of OrderStatusRequest
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TOrderStatusRequestMessageImpl : public TCppAutoObject<IOrderStatusRequestMessage>
{
  typedef _COM_CLASS inherited;
private:
	TOrderStatusRequest FMessage;
public:
  __fastcall TOrderStatusRequestMessageImpl();
  __fastcall TOrderStatusRequestMessageImpl(const System::_di_IInterface Controller);
  __fastcall TOrderStatusRequestMessageImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // IOrderStatusRequest
protected:
  STDMETHOD(get_Account(BSTR* Value));
  STDMETHOD(get_AccountFlag(BSTR* Value));
  STDMETHOD(get_AE(BSTR* Value));
  STDMETHOD(get_ClOrdID(BSTR* Value));
  STDMETHOD(get_Data(BSTR* Value));
  STDMETHOD(get_GatewayData(BSTR* Value));
  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_Market(MarketEnum* Value));
  STDMETHOD(get_MessageType(MessageTypeEnum* Value));
  STDMETHOD(get_NID(__int64* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_Value(BSTR Name, BSTR* Value));
  STDMETHOD(set_Account(BSTR Value));
  STDMETHOD(set_AccountFlag(BSTR Value));
  STDMETHOD(set_AE(BSTR Value));
  STDMETHOD(set_ClOrdID(BSTR Value));
  STDMETHOD(set_Data(BSTR Value));
  STDMETHOD(set_GatewayData(BSTR Value));
  STDMETHOD(set_Market(MarketEnum Value));
  STDMETHOD(set_MessageType(MessageTypeEnum Value));
  STDMETHOD(set_NID(__int64 Value));
  STDMETHOD(set_Symbol(BSTR Value));
  STDMETHOD(set_Value(BSTR Name, BSTR Value));
  STDMETHOD(get_BrokerID(BSTR* Value));
  STDMETHOD(get_ExchangeCode(BSTR* Value));
  STDMETHOD(get_OrderID(BSTR* Value));
  STDMETHOD(get_Price(double* Value));
  STDMETHOD(get_Side(SideEnum* Value));
  STDMETHOD(get_TradingSessionID(TradingSessionIDEnum* Value));
  STDMETHOD(get_TSEExchangeCode(BSTR* Value));
  STDMETHOD(get_TSEKeep(BSTR* Value));
  STDMETHOD(get_TSEObjectBroker(BSTR* Value));
  STDMETHOD(get_TSEOrderType(BSTR* Value));
  STDMETHOD(get_TSEStockSeqNo(long* Value));
  STDMETHOD(get_TSETradeKind(BSTR* Value));
  STDMETHOD(set_BrokerID(BSTR Value));
  STDMETHOD(set_ExchangeCode(BSTR Value));
  STDMETHOD(set_OrderID(BSTR Value));
  STDMETHOD(set_Price(double Value));
  STDMETHOD(set_Side(SideEnum Value));
  STDMETHOD(set_TradingSessionID(TradingSessionIDEnum Value));
  STDMETHOD(set_TSEExchangeCode(BSTR Value));
  STDMETHOD(set_TSEKeep(BSTR Value));
  STDMETHOD(set_TSEObjectBroker(BSTR Value));
  STDMETHOD(set_TSEOrderType(BSTR Value));
  STDMETHOD(set_TSEStockSeqNo(long Value));
  STDMETHOD(set_TSETradeKind(BSTR Value));
};




#endif //OrderStatusRequestImplH
