// ---------------------------------------------------------------------------
// BaseMessageImpl.h : Declaration of the TBaseMessageImpl
// ---------------------------------------------------------------------------
#ifndef BaseMessageImplH
#define BaseMessageImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "SpeedyAPI_TLB.h"
#include <TBaseMessage.h>

// ---------------------------------------------------------------------------
// TBaseMessageImpl     Implements IBaseMessage, default interface of BaseMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : Basic class for all trading messages.
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TBaseMessageImpl : public TCppAutoObject<IBaseMessage>
{
  typedef _COM_CLASS inherited;

private:
	TBaseMessage FMessage;
public:
  __fastcall TBaseMessageImpl();
  __fastcall TBaseMessageImpl(const System::_di_IInterface Controller);
  __fastcall TBaseMessageImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // IBaseMessage
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
};




#endif //BaseMessageImplH
