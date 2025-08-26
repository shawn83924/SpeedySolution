// ---------------------------------------------------------------------------
// QuoteMessageImpl.h : Declaration of the TQuoteMessageImpl
// ---------------------------------------------------------------------------
#ifndef QuoteMessageImplH
#define QuoteMessageImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "SpeedyAPI_TLB.h"
#include <TQuoteMessage.h>

// ---------------------------------------------------------------------------
// TQuoteMessageImpl     Implements IQuoteMessage, default interface of QuoteMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : Message to place a quote 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TQuoteMessageImpl : public TCppAutoObject<IQuoteMessage>
{
  typedef _COM_CLASS inherited;
private:
	TQuoteMessage FMessage;
public:
  __fastcall TQuoteMessageImpl();
  __fastcall TQuoteMessageImpl(const System::_di_IInterface Controller);
  __fastcall TQuoteMessageImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // IQuoteMessage
protected:
  STDMETHOD(get_Account(BSTR* Value));
  STDMETHOD(get_AccountFlag(BSTR* Value));
  STDMETHOD(get_AE(BSTR* Value));
  STDMETHOD(get_AskPrice(double* Value));
  STDMETHOD(get_AskQty(long* Value));
  STDMETHOD(get_BidPrice(double* Value));
  STDMETHOD(get_BidQty(long* Value));
  STDMETHOD(get_ClOrdID(BSTR* Value));
  STDMETHOD(get_Data(BSTR* Value));
  STDMETHOD(get_GatewayData(BSTR* Value));
  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_Market(MarketEnum* Value));
  STDMETHOD(get_MarketDataSource(BSTR* Value));
  STDMETHOD(get_MessageType(MessageTypeEnum* Value));
  STDMETHOD(get_NID(__int64* Value));
  STDMETHOD(get_OrderSource(OrderSourceEnum* Value));
  STDMETHOD(get_QuoteID(BSTR* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_TimeInForce(TimeInForceEnum* Value));
  STDMETHOD(get_Value(BSTR Name, BSTR* Value));
  STDMETHOD(set_Account(BSTR Value));
  STDMETHOD(set_AccountFlag(BSTR Value));
  STDMETHOD(set_AE(BSTR Value));
  STDMETHOD(set_AskPrice(double Value));
  STDMETHOD(set_AskQty(long Value));
  STDMETHOD(set_BidPrice(double Value));
  STDMETHOD(set_BidQty(long Value));
  STDMETHOD(set_ClOrdID(BSTR Value));
  STDMETHOD(set_Data(BSTR Value));
  STDMETHOD(set_GatewayData(BSTR Value));
  STDMETHOD(set_Market(MarketEnum Value));
  STDMETHOD(set_MarketDataSource(BSTR Value));
  STDMETHOD(set_MessageType(MessageTypeEnum Value));
  STDMETHOD(set_NID(__int64 Value));
  STDMETHOD(set_OrderSource(OrderSourceEnum Value));
  STDMETHOD(set_QuoteID(BSTR Value));
  STDMETHOD(set_Symbol(BSTR Value));
  STDMETHOD(set_TimeInForce(TimeInForceEnum Value));
  STDMETHOD(set_Value(BSTR Name, BSTR Value));
  STDMETHOD(get_TradingSessionID(TradingSessionIDEnum* Value));
  STDMETHOD(set_TradingSessionID(TradingSessionIDEnum Value));
};




#endif //QuoteMessageImplH
