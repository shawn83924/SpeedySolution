// ---------------------------------------------------------------------------
// QuoteCancelMessageImpl.h : Declaration of the TQuoteCancelMessageImpl
// ---------------------------------------------------------------------------
#ifndef QuoteCancelMessageImplH
#define QuoteCancelMessageImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "SpeedyAPI_TLB.h"
#include <TQuoteCancelMessage.h>

// ---------------------------------------------------------------------------
// TQuoteCancelMessageImpl     Implements IQuoteCancelMessage, default interface of QuoteCancelMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : Message to cancel a quote
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TQuoteCancelMessageImpl : public TCppAutoObject<IQuoteCancelMessage>
{
  typedef _COM_CLASS inherited;
private:
    TQuoteCancelMessage FMessage;

public:
  __fastcall TQuoteCancelMessageImpl();
  __fastcall TQuoteCancelMessageImpl(const System::_di_IInterface Controller);
  __fastcall TQuoteCancelMessageImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // IQuoteCancelMessage
protected:
  STDMETHOD(get_Account(BSTR* Value));
  STDMETHOD(get_AccountFlag(BSTR* Value));
  STDMETHOD(get_AE(BSTR* Value));
  STDMETHOD(get_AskPrice(double* Value));
  STDMETHOD(get_BidPrice(double* Value));
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
  STDMETHOD(get_Value(BSTR Name, BSTR* Value));
  STDMETHOD(set_Account(BSTR Value));
  STDMETHOD(set_AccountFlag(BSTR Value));
  STDMETHOD(set_AE(BSTR Value));
  STDMETHOD(set_AskPrice(double Value));
  STDMETHOD(set_BidPrice(double Value));
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
  STDMETHOD(set_Value(BSTR Name, BSTR Value));
  STDMETHOD(get_TradingSessionID(TradingSessionIDEnum* Value));
  STDMETHOD(set_TradingSessionID(TradingSessionIDEnum Value));
};




#endif //QuoteCancelMessageImplH
