 // 1.1
// UNIT1 : Implementation of TDayHighLowImpl (CoClass: DayHighLow, Interface: IDayHighLow)

#include <vcl.h>
#pragma hdrstop

#include "DayHighLow.h"

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

