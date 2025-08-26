// ---------------------------------------------------------------------------
// SYSTEMMSGIMPL1 : Implementation of TSystemMsgImpl (CoClass: SystemMsg, Interface: ISystemMsg)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SystemMsgImpl.h"


// ---------------------------------------------------------------------------
// TSystemMsgImpl
// ---------------------------------------------------------------------------
__fastcall TSystemMsgImpl::TSystemMsgImpl()
{
}


// ---------------------------------------------------------------------------
// TSystemMsgImpl
// ---------------------------------------------------------------------------
__fastcall TSystemMsgImpl::TSystemMsgImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TSystemMsgImpl
// ---------------------------------------------------------------------------
__fastcall TSystemMsgImpl::TSystemMsgImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TSystemMsgImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TSystemMsgImpl>(Comserv::GetComServer(),
                           __classid(TSystemMsgImpl),
                           CLSID_SystemMsg,
                           Comobj::ciMultiInstance,
						   Comobj::tmApartment);
}
#pragma startup createFactory 32
/////////////////////////////////////////////////////////////////////////////
// TSystemMsgImpl
STDMETHODIMP TSystemMsgImpl::get_Code(long* Value)
{
	*Value = FCode;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TSystemMsgImpl::get_Data(BSTR* Value)
{
  try
  {
	WideString DataStr( FData.c_str() );
	*Value = DataStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISystemMsg);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TSystemMsgImpl::get_Exchange(BSTR* Value)
{
  try
  {
	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISystemMsg);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TSystemMsgImpl::get_Market(EnumMarket* Value)
{
	*Value = FMarket;
	return S_OK;
}
//------------------------------------------------------------------------------
void TSystemMsgImpl::FromMarketDataMessage( SystemMessage* Msg )
{
	FCode     = Msg->GetCode();
	FSymbol   = Msg->GetSymbol();
	FExchange = Msg->GetExchange();
	FData     = Msg->GetData();
	FMarket   = (EnumMarket)Msg->GetMarket();
}
//------------------------------------------------------------------------------
STDMETHODIMP TSystemMsgImpl::get_Symbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISystemMsg);
  }
  return S_OK;
}
//------------------------------------------------------------------------------



