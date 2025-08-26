// ---------------------------------------------------------------------------
// ORDERBOOKIMPL : Implementation of TOrderBookImpl (CoClass: OrderBook, Interface: IOrderBook)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "OrderBookImpl.h"


// ---------------------------------------------------------------------------
// TOrderBookImpl
// ---------------------------------------------------------------------------
__fastcall TOrderBookImpl::TOrderBookImpl()
{
}


// ---------------------------------------------------------------------------
// TOrderBookImpl
// ---------------------------------------------------------------------------
__fastcall TOrderBookImpl::TOrderBookImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TOrderBookImpl
// ---------------------------------------------------------------------------
__fastcall TOrderBookImpl::TOrderBookImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TOrderBookImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TOrderBookImpl>(Comserv::GetComServer(),
                           __classid(TOrderBookImpl),
                           CLSID_OrderBook,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
/////////////////////////////////////////////////////////////////////////////
// TOrderBookImpl
STDMETHODIMP TOrderBookImpl::get_DerivedBuyPx(double* Value)
{
  try
  {
	*Value = FDerivedBuyPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_DerivedBuyQty(long* Value)
{
  try
  {
	*Value = FDerivedBuyQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_DerivedFlag(VARIANT_BOOL* Value)
{
  try
  {
	if( FDerivedFlag )
		*Value = VARIANT_TRUE;
	else
		*Value = VARIANT_FALSE;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_DerivedSellPx(double* Value)
{
  try
  {
	*Value = FDerivedSellPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_DerivedSellQty(long* Value)
{
  try
  {
	*Value = FDerivedSellQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_Exchange(BSTR* Value)
{
  try
  {
	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_Market(EnumMarket* Value)
{
  try
  {
	*Value = FMarket;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_Symbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_BuyPx(long Depth, double* Value)
{
  try
  {
	if( Depth <= 0 || Depth > MAX_DEPTH )
		*Value = 0;
	else
		*Value = FBuyPx[Depth - 1];
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_BuyQty(long Depth, long* Value)
{
  try
  {
	if( Depth <= 0 || Depth > MAX_DEPTH )
		*Value = 0;
	else
		*Value = FBuyQty[Depth - 1];
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_SellPx(long Depth, double* Value)
{
  try
  {
	if( Depth <= 0 || Depth > MAX_DEPTH )
		*Value = 0;
	else
		*Value = FSellPx[Depth - 1];
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_SellQty(long Depth, long* Value)
{
  try
  {
	if( Depth <= 0 || Depth > MAX_DEPTH )
		*Value = 0;
	else
		*Value = FSellQty[Depth - 1];
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
void TOrderBookImpl::FromOrderBookData( OrderBookData* Data )
{
	FExchange = Data->GetExchange();
	FSymbol =  Data->GetSymbol();
	FMarket = (EnumMarket)Data->GetMarket();
	FTime = Data->GetMessageTime();
	for( int i = 0 ; i < MAX_DEPTH; i++ )
	{
		FBuyPx[i]   = Data->GetBuyPx(i+1);
		FBuyQty[i]  = Data->GetBuyQty(i+1);
		FSellPx[i]  = Data->GetSellPx(i+1);
		FSellQty[i] = Data->GetSellQty(i+1);
	}
	if( Data->HasNumOfOrders == true )
	{
		memcpy( FBuyNumOfOrders, Data->BuyNumOfOrders, sizeof( UFCType::UInt32)*MAX_DEPTH );
		memcpy( FSellNumOfOrders, Data->SellNumOfOrders, sizeof( UFCType::UInt32)*MAX_DEPTH );
	}
	else
	{
		memset( FBuyNumOfOrders, sizeof( UFCType::UInt32)*MAX_DEPTH, 0 );
		memset( FSellNumOfOrders, sizeof( UFCType::UInt32)*MAX_DEPTH, 0 );
	}
	FDerivedFlag    = Data->GetDerivedFlag();
	FDerivedBuyPx   = Data->GetDerivedBuyPrice();
	FDerivedBuyQty  = Data->GetDerivedBuyQty();
	FDerivedSellPx  = Data->GetDerivedSellPrice();
	FDerivedSellQty = Data->GetDerivedSellQty();
	FIsTestMatch    = Data->IsTestMatch();
	FTradingSession = (EnumTradingSession)Data->GetTradingSession();
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_Time(BSTR* Value)
{
  try
  {
	WideString TimeStr( FTime.c_str() );
	*Value = TimeStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_IsTestMatch(VARIANT_BOOL* Value)
{
	if( FIsTestMatch == true )
	   *Value = VARIANT_TRUE;
	else
	   *Value = VARIANT_FALSE;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_TradingSession(EnumTradingSession* Value)
{
  *Value = FTradingSession;
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_BuyNumberOfOrders(long Depth, long* Value)
{
  try
  {
	if( Depth <= 0 || Depth > MAX_DEPTH ) ///< 1~10
		*Value = 0;
	else
		*Value = FBuyNumOfOrders[Depth - 1];
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TOrderBookImpl::get_SellNumberOfOrders(long Depth, long* Value)
{
  try
  {
	if( Depth <= 0 || Depth > MAX_DEPTH ) ///< 1~10
		*Value = 0;
	else
		*Value = FSellNumOfOrders[Depth - 1];
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOrderBook);
  }
  return S_OK;
}
//------------------------------------------------------------------------------



