// ---------------------------------------------------------------------------
// TOUCHORDERCOMMANDIMPL : Implementation of TTouchOrderCommandImpl (CoClass: TouchOrderCommand, Interface: ITouchOrderCommand)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TouchOrderCommandImpl.h"

// ---------------------------------------------------------------------------
// TTouchOrderCommandImpl
// ---------------------------------------------------------------------------
__fastcall TTouchOrderCommandImpl::TTouchOrderCommandImpl()
{
	FCommand = new TTouchOrderCommand();
}
// ---------------------------------------------------------------------------
__fastcall TTouchOrderCommandImpl::TTouchOrderCommandImpl(
	Speedyapi_tlb::TouchedOrderCommandEnum toc_type,
	BSTR touchorder_id,
	Speedyapi_tlb::SideEnum side)
{
	TTouchOrderCommand::TouchedOrderCommandEnum Type = static_cast<TTouchOrderCommand::TouchedOrderCommandEnum>(static_cast<int>(toc_type));
	AnsiString id(touchorder_id);
	nsOrderMessageDefine::SideEnum Side =  static_cast<nsOrderMessageDefine::SideEnum>(static_cast<int>(side));
	FCommand = new TTouchOrderCommand(Type, id.c_str(), Side);
}
// ---------------------------------------------------------------------------
// TTouchOrderCommandImpl
// ---------------------------------------------------------------------------
__fastcall TTouchOrderCommandImpl::TTouchOrderCommandImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
	FCommand = new TTouchOrderCommand();
}
// ---------------------------------------------------------------------------
// TTouchOrderCommandImpl
// ---------------------------------------------------------------------------
__fastcall TTouchOrderCommandImpl::TTouchOrderCommandImpl(Comobj::TComObjectFactory* Factory,
									  const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
	FCommand = new TTouchOrderCommand();
}
__fastcall TTouchOrderCommandImpl::~TTouchOrderCommandImpl()
{
	delete FCommand;
}
// ---------------------------------------------------------------------------
// TTouchOrderCommandImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TTouchOrderCommandImpl>(Comserv::GetComServer(),
                           __classid(TTouchOrderCommandImpl),
                           CLSID_TouchOrderCommand,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetCmdType(TouchedOrderCommandEnum toc_type)
{
	TTouchOrderCommand::TouchedOrderCommandEnum Type = static_cast<TTouchOrderCommand::TouchedOrderCommandEnum>(static_cast<int>(toc_type));
	FCommand->SetCmdType(Type);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetMarket(MarketEnum market)
{
	nsOrderMessageDefine::MarketEnum Market = static_cast<nsOrderMessageDefine::MarketEnum>(static_cast<int>(market));
	FCommand->SetMarket(Market);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetSymbol(BSTR symbol)
{
	AnsiString Symbol(symbol);
	FCommand->SetSymbol(Symbol.c_str());
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetSide(SideEnum side)
{
	nsOrderMessageDefine::SideEnum Side = static_cast<nsOrderMessageDefine::SideEnum>(static_cast<int>(side));
	FCommand->SetSide(Side);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetTouchOrderID(BSTR id)
{
	AnsiString Id(id);
	FCommand->SetTouchOrderID(Id.c_str());
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetPrice(BSTR price)
{
	AnsiString Price(price);
	FCommand->SetPrice(Price.c_str());
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetFloatingOrderPrice(PriceDependOnEnum depend_on,
          int ticks)
{
	TTouchOrderCommand::PriceDependOnEnum Depend_on = static_cast<TTouchOrderCommand::PriceDependOnEnum>(static_cast<int>(depend_on));
	FCommand->SetFloatingOrderPrice(Depend_on, ticks);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetUserData(BSTR data)
{
	AnsiString Data(data);
	FCommand->SetUserData(Data.c_str());
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetTriggeredActionNewOrder(INewOrderMessage* order,
          VARIANT_BOOL* value)
{
	VARIANT MessageInstance;
	order->get_Instance( &MessageInstance );
	if( MessageInstance.byref != NULL )
		*value = FCommand->SetTriggeredAction( static_cast<TNewOrderMessage*>(MessageInstance.byref) );
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetTriggeredActionCancelOrder(ICancelOrderMessage* order,
          VARIANT_BOOL* value)
{
	VARIANT MessageInstance;
	order->get_Instance( &MessageInstance );
	if( MessageInstance.byref != NULL )
		*value = FCommand->SetTriggeredAction( static_cast<TCancelOrderMessage*>(MessageInstance.byref) );
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetTriggeredActionReplaceOrder(IReplaceOrderMessage* order,
          VARIANT_BOOL* value)

{
	VARIANT MessageInstance;
	order->get_Instance( &MessageInstance );
	if( MessageInstance.byref != NULL )
		*value = FCommand->SetTriggeredAction( static_cast<TReplaceOrderMessage*>(MessageInstance.byref));
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetTriggeredActionTouchOrderCmd(ITouchOrderCommand* toc,
          VARIANT_BOOL* value)

{
	VARIANT MessageInstance;
	toc->get_Instance( &MessageInstance );
	if( MessageInstance.byref != NULL )
		*value = FCommand->SetTriggeredAction( static_cast<TTouchOrderCommand*>(MessageInstance.byref));
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::get_Instance(VARIANT* Value)
{
	VARIANT MessageInstance;
	MessageInstance.vt = VT_BYREF;
	MessageInstance.byref = FCommand;
	*Value = MessageInstance;
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetTriggeredActionMsg(BSTR msg, VARIANT_BOOL* value)

{
	AnsiString Msg(msg);
	*value = FCommand->SetTriggeredAction(Msg.c_str());
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::SetTriggeringCondition(ITriggeringCondition* ttc)
{
	VARIANT ConditionInstance;
	ttc->get_Instance(&ConditionInstance);
	if(ConditionInstance.byref != NULL)
		FCommand->SetTriggeringCondition( static_cast<TTriggeringCondition*>(ConditionInstance.byref) );
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTouchOrderCommandImpl::GetNID(__int64* NID)
{
	*NID = FCommand->GetNID();
	return S_OK;
}
