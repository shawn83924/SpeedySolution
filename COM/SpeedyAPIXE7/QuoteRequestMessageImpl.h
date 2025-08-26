// ---------------------------------------------------------------------------
// QuoteRequestMessageImpl.h : Declaration of the TQuoteRequestMessageImpl
// ---------------------------------------------------------------------------
#ifndef QuoteRequestMessageImplH
#define QuoteRequestMessageImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "SpeedyAPI_TLB.h"
#include <TQuoteRequestMessage.h>

// ---------------------------------------------------------------------------
// TQuoteRequestMessageImpl     Implements IQuoteRequestMessage, default interface of QuoteRequestMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TQuoteRequestMessageImpl : public TCppAutoObject<IQuoteRequestMessage>
{
  typedef _COM_CLASS inherited;
private:
	TQuoteRequestMessage FMessage;
public:
  __fastcall TQuoteRequestMessageImpl();
  __fastcall TQuoteRequestMessageImpl(const System::_di_IInterface Controller);
  __fastcall TQuoteRequestMessageImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // IQuoteRequestMessage
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
  STDMETHOD(get_TradingSessionID(TradingSessionIDEnum* Value));
  STDMETHOD(set_TradingSessionID(TradingSessionIDEnum Value));
  STDMETHOD(get_OrderID(BSTR* Value));
  STDMETHOD(set_OrderID(BSTR Value));
};




#endif //QuoteRequestMessageImplH
