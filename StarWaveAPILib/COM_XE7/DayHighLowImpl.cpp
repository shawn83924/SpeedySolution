// ---------------------------------------------------------------------------
// DAYHIGHLOWIMPL : Implementation of TDayHighLowImpl (CoClass: DayHighLow, Interface: IDayHighLow)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "DayHighLowImpl.h"


// ---------------------------------------------------------------------------
// TDayHighLowImpl
// ---------------------------------------------------------------------------
__fastcall TDayHighLowImpl::TDayHighLowImpl()
{
}


// ---------------------------------------------------------------------------
// TDayHighLowImpl
// ---------------------------------------------------------------------------
__fastcall TDayHighLowImpl::TDayHighLowImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TDayHighLowImpl
// ---------------------------------------------------------------------------
__fastcall TDayHighLowImpl::TDayHighLowImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TDayHighLowImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TDayHighLowImpl>(Comserv::GetComServer(),
                           __classid(TDayHighLowImpl),
                           CLSID_DayHighLow,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
/////////////////////////////////////////////////////////////////////////////
// TDayHighLowImpl
STDMETHODIMP TDayHighLowImpl::get_DayHighPx(double* Value)
{
  try
  {
	*Value = FDayHighPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IDayHighLow);
  }
  return S_OK;
};


STDMETHODIMP TDayHighLowImpl::get_DayLowPx(double* Value)
{
  try
  {
	 *Value = FDayLowPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IDayHighLow);
  }
  return S_OK;
};


STDMETHODIMP TDayHighLowImpl::get_Exchange(BSTR* Value)
{
  try
  {
	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IDayHighLow);
  }
  return S_OK;
};


STDMETHODIMP TDayHighLowImpl::get_Market(EnumMarket* Value)
{
  try
  {
	*Value = FMarket;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IDayHighLow);
  }
  return S_OK;
};


STDMETHODIMP TDayHighLowImpl::get_Symbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IDayHighLow);
  }
  return S_OK;
};


void TDayHighLowImpl::FromDayHighLow( DayHighLowPrice* DayHighLow )
{
	FExchange = DayHighLow->GetExchange();
	FSymbol = DayHighLow->GetSymbol();
	FMarket = (EnumMarket)DayHighLow->GetMarket();
	FDayHighPx = DayHighLow->GetDayHighPrice();
	FDayLowPx = DayHighLow->GetDayLowPrice();
}


