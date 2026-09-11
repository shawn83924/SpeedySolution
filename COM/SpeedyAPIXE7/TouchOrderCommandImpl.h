// ---------------------------------------------------------------------------
// TouchOrderCommandImpl.h : Declaration of the TTouchOrderCommandImpl
// ---------------------------------------------------------------------------
#ifndef TouchOrderCommandImplH
#define TouchOrderCommandImplH

#include <System.Win.ComServ.hpp>
#include <axbase.h>
#include "SpeedyAPI_TLB.h"
#include "TTouchOrderCommand.h"

// ---------------------------------------------------------------------------
// TTouchOrderCommandImpl     Implements ITouchOrderCommand, default interface of TouchOrderCommand
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TTouchOrderCommandImpl : public TCppAutoObject<ITouchOrderCommand>
{
  typedef _COM_CLASS inherited;
private:
	TTouchOrderCommand* FCommand;
public:
  STDMETHOD(SetCmdType(TouchedOrderCommandEnum toc_type));
  STDMETHOD(SetMarket(MarketEnum market));
  STDMETHOD(SetSymbol(BSTR symbol));
  STDMETHOD(SetSide(SideEnum side));
  STDMETHOD(SetTouchOrderID(BSTR id));
  STDMETHOD(SetPrice(BSTR price));
  STDMETHOD(SetFloatingOrderPrice(PriceDependOnEnum depend_on, int ticks));
  STDMETHOD(SetUserData(BSTR data));
  STDMETHOD(SetTriggeredActionNewOrder(INewOrderMessage* order, VARIANT_BOOL* value));
  STDMETHOD(SetTriggeredActionCancelOrder(ICancelOrderMessage* order, VARIANT_BOOL* value));
  STDMETHOD(SetTriggeredActionReplaceOrder(IReplaceOrderMessage* order, VARIANT_BOOL* value));
  STDMETHOD(SetTriggeredActionTouchOrderCmd(ITouchOrderCommand* toc, VARIANT_BOOL* value));
  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(SetTriggeredActionMsg(BSTR msg, VARIANT_BOOL* value));
  STDMETHOD(SetTriggeringCondition(ITriggeringCondition* ttc));
  STDMETHOD(GetNID(__int64* NID));
  STDMETHOD(GetLastErrorMsg(BSTR* Value));
  __fastcall TTouchOrderCommandImpl();
  __fastcall TTouchOrderCommandImpl(
	Speedyapi_tlb::TouchedOrderCommandEnum toc_type,
	BSTR touchorder_id,
	Speedyapi_tlb::SideEnum side = Speedyapi_tlb::SideEnum::sNone);
  __fastcall TTouchOrderCommandImpl(const System::_di_IInterface Controller);
  __fastcall TTouchOrderCommandImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  __fastcall ~TTouchOrderCommandImpl();

  // ITouchOrderCommand
protected:
};
#endif //TouchOrderCommandImplH
