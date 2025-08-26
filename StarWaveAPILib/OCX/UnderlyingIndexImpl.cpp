// ---------------------------------------------------------------------------
// UNDERLYINGINDEXIMPL : Implementation of TUnderlyingIndexImpl (CoClass: UnderlyingIndex, Interface: IUnderlyingIndex)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnderlyingIndexImpl.h"


// ---------------------------------------------------------------------------
// TUnderlyingIndexImpl
// ---------------------------------------------------------------------------
__fastcall TUnderlyingIndexImpl::TUnderlyingIndexImpl()
{
}


// ---------------------------------------------------------------------------
// TUnderlyingIndexImpl
// ---------------------------------------------------------------------------
__fastcall TUnderlyingIndexImpl::TUnderlyingIndexImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TUnderlyingIndexImpl
// ---------------------------------------------------------------------------
__fastcall TUnderlyingIndexImpl::TUnderlyingIndexImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TUnderlyingIndexImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TUnderlyingIndexImpl>(Comserv::GetComServer(),
						   __classid(TUnderlyingIndexImpl),
						   CLSID_UnderlyingIndex,
						   Comobj::ciMultiInstance,
						   Comobj::tmApartment);
}
#pragma startup createFactory 32

/////////////////////////////////////////////////////////////////////////////
// TUnderlyingIndexImpl
STDMETHODIMP TUnderlyingIndexImpl::get_IndexKind(BSTR* Value)
{
  try
  {
	WideString SymStr( FIndexKind.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TUnderlyingIndexImpl::get_IndexTime(BSTR* Value)
{
  try
  {
	WideString TimeStr( FIndexTime.c_str() );
	*Value = TimeStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TUnderlyingIndexImpl::get_IndexValue(double* Value)
{
  try
  {
	*Value = FIndexValue;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
void TUnderlyingIndexImpl::FromIndexMessage( UnderlyingIndexInfo* Info )
{
	FExchange    = Info->GetExchange();
	FIndexKind   = Info->GetSymbol();
	FIndexValue  = Info->GetIndexValue();
	FTotalQty    = Info->GetTotalQty();
	FTotalCount  = Info->GetTotalCount();
	FTotalAmount = Info->GetTotalAmount();

	FBuyValue  = Info->GetBuyValue();
	FSellValue = Info->GetSellValue();
	FFixValue  = Info->GetFixValue();
	FIndexTime = Info->GetShowTime();
	FExchangeDate = Info->GetExchangeDate();
	FMarket = (EnumMarket)Info->GetMarket();
}
//------------------------------------------------------------------------------
STDMETHODIMP TUnderlyingIndexImpl::get_Exchange(BSTR* Value)
{
  try
  {
	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TUnderlyingIndexImpl::get_Market(EnumMarket* Value)
{
  try
  {
	*Value = FMarket;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TUnderlyingIndexImpl::get_BuyValue(double* Value)
{
  try
  {
	*Value = FBuyValue;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TUnderlyingIndexImpl::get_ExchangeDate(BSTR* Value)
{
  try
  {
	WideString DateStr( FExchangeDate.c_str() );
	*Value = DateStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TUnderlyingIndexImpl::get_FixValue(double* Value)
{
  try
  {
	*Value = FFixValue;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TUnderlyingIndexImpl::get_SellValue(double* Value)
{
  try
  {
	*Value = FSellValue;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TUnderlyingIndexImpl::get_TotalAmount(unsigned_int64* Value)
{
  try
  {
	*Value = FTotalAmount;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TUnderlyingIndexImpl::get_TotalCount(long* Value)
{
  try
  {
	*Value = FTotalCount;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TUnderlyingIndexImpl::get_TotalQty(long* Value)
{
  try
  {
	*Value = FTotalQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IUnderlyingIndex);
  }
  return S_OK;
}
//------------------------------------------------------------------------------



