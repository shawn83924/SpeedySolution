// ---------------------------------------------------------------------------
// SUMOFORDERIMPL : Implementation of TSumOfOrderImpl (CoClass: SumOfOrder, Interface: ISumOfOrder)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SumOfOrderImpl.h"


// ---------------------------------------------------------------------------
// TSumOfOrderImpl
// ---------------------------------------------------------------------------
__fastcall TSumOfOrderImpl::TSumOfOrderImpl()
{
}


// ---------------------------------------------------------------------------
// TSumOfOrderImpl
// ---------------------------------------------------------------------------
__fastcall TSumOfOrderImpl::TSumOfOrderImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TSumOfOrderImpl
// ---------------------------------------------------------------------------
__fastcall TSumOfOrderImpl::TSumOfOrderImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TSumOfOrderImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TSumOfOrderImpl>(Comserv::GetComServer(),
                           __classid(TSumOfOrderImpl),
                           CLSID_SumOfOrder,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32

/////////////////////////////////////////////////////////////////////////////
// TSumOfOrderImpl
STDMETHODIMP TSumOfOrderImpl::get_BuyOrderCount(long* Value)
{
  try
  {
	*Value = FBuyOrderCount;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISumOfOrder);
  }
  return S_OK;
};


STDMETHODIMP TSumOfOrderImpl::get_BuyOrderQty(long* Value)
{
  try
  {
	*Value = FBuyOrderQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISumOfOrder);
  }
  return S_OK;
};


STDMETHODIMP TSumOfOrderImpl::get_Exchange(BSTR* Value)
{
  try
  {
	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISumOfOrder);
  }
  return S_OK;
};


STDMETHODIMP TSumOfOrderImpl::get_Market(EnumMarket* Value)
{
  try
  {
  	*Value = FMarket;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISumOfOrder);
  }
  return S_OK;
};


STDMETHODIMP TSumOfOrderImpl::get_SellOrderCount(long* Value)
{
  try
  {
	*Value = FSellOrderCount;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISumOfOrder);
  }
  return S_OK;
};


STDMETHODIMP TSumOfOrderImpl::get_Symbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISumOfOrder);
  }
  return S_OK;
};


void TSumOfOrderImpl::FromSumOfOrderInfo( SumOfOrderInfo* Info )
{
	FExchange = Info->GetExchange();
	FSymbol = Info->GetSymbol();
	FMarket = (EnumMarket)Info->GetMarket();
	FBuyOrderCount = Info->GetBuyOrderCount();
	FSellOrderCount = Info->GetSellOrderCount();
	FBuyOrderQty = Info->GetBuyQty();
	FSellOrderQty = Info->GetSellQty();
}
STDMETHODIMP TSumOfOrderImpl::get_SellOrderQty(long* Value)
{
  try
  {
	*Value = FSellOrderQty;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISumOfOrder);
  }
  return S_OK;
};
