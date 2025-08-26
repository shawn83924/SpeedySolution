// ---------------------------------------------------------------------------
// CLOSEMARKETDATAIMPL : Implementation of TCloseMarketDataImpl (CoClass: CloseMarketData, Interface: ICloseMarketData)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "CloseMarketDataImpl.h"


// ---------------------------------------------------------------------------
// TCloseMarketDataImpl
// ---------------------------------------------------------------------------
__fastcall TCloseMarketDataImpl::TCloseMarketDataImpl()
{
}


// ---------------------------------------------------------------------------
// TCloseMarketDataImpl
// ---------------------------------------------------------------------------
__fastcall TCloseMarketDataImpl::TCloseMarketDataImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TCloseMarketDataImpl
// ---------------------------------------------------------------------------
__fastcall TCloseMarketDataImpl::TCloseMarketDataImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TCloseMarketDataImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TCloseMarketDataImpl>(Comserv::GetComServer(),
                           __classid(TCloseMarketDataImpl),
                           CLSID_CloseMarketData,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32

/////////////////////////////////////////////////////////////////////////////
// TCloseMarketDataImpl
STDMETHODIMP TCloseMarketDataImpl::get_BuyCountTotal(long* Value)
{
  try
  {
	*Value = FBuyCountTotal;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_BuyPx(double* Value)
{
  try
  {
	*Value = FBuyPx;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_BuyQtyTotal(long* Value)
{
  try
  {
   	*Value = FBuyQtyTotal;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_ClosePx(double* Value)
{
  try
  {
	*Value = FClosePx;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_DayHighPx(double* Value)
{
  try
  {
	*Value = FDayHighPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_DayLowPx(double* Value)
{
  try
  {
	*Value = FDayLowPx;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_Exchange(BSTR* Value)
{
  try
  {
	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_Market(EnumMarket* Value)
{
  try
  {
	*Value = FMarket;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_OpenInterest(long* Value)
{
  try
  {
	*Value = FOpenInterest;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_OpenPx(double* Value)
{
  try
  {
	*Value = FOpenPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_SellCountTotal(long* Value)
{
  try
  {
	*Value = FSellCountTotal;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_SellPx(double* Value)
{
  try
  {
	*Value = FSellPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_SellQtyTotal(long* Value)
{
  try
  {
	*Value = FSellQtyTotal;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_SettlementPx(double* Value)
{
  try
  {
	*Value = FSettlementPx;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_Symbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_TermHighPx(double* Value)
{
  try
  {
	*Value = FTermHighPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_TermLowPx(double* Value)
{
  try
  {
	*Value = FTermLowPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_TotalCount(long* Value)
{
  try
  {
	*Value = FTotalCount;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


STDMETHODIMP TCloseMarketDataImpl::get_TotalQty(long* Value)
{
  try
  {
  	*Value = FTotalQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ICloseMarketData);
  }
  return S_OK;
};


void TCloseMarketDataImpl::FromI070( ClosingMarketData* Data )
{
	FillClosingData( Data );
	FSettlementPx = 0;
	FOpenInterest = 0;
}

void TCloseMarketDataImpl::FromI071( ClosingMarketDataWithSettlementPrice* Data )
{
	FillClosingData( Data );
	FSettlementPx = Data->GetSettlementPrice();
	FOpenInterest = 0;
}

void TCloseMarketDataImpl::FromI072( ClosingMarketDataWithSettlementPriceAndOpenInterest* Data )
{
	FillClosingData( Data );
	FSettlementPx = Data->GetSettlementPrice();
	FOpenInterest = Data->GetOpenInterest();
}

void TCloseMarketDataImpl::FillClosingData( ClosingMarketData* Data )
{
	FExchange = Data->GetExchange();
	FSymbol = Data->GetSymbol();
	FMarket = (EnumMarket)Data->GetMarket();
	FTermHighPx = Data->GetTermHighPrice();
	FTermLowPx = Data->GetTermLowPrice();
	FDayHighPx = Data->GetDayHighPrice();
	FDayLowPx = Data->GetDayLowPrice();
	FOpenPx = Data->GetOpenPrice();
	FBuyPx = Data->GetBuyPrice();
	FSellPx = Data->GetSellPrice();
	FClosePx = Data->GetClosePrice();
	FBuyCountTotal = Data->GetBuyTotalCount();
	FBuyQtyTotal = Data->GetBuyTotalQty();
	FSellCountTotal = Data->GetSellTotalCount();
	FSellQtyTotal = Data->GetSellTotalQty();
	FTotalCount = Data->GetTotalTradeCount();
	FTotalQty = Data->GetTotalTradeQty();
}

