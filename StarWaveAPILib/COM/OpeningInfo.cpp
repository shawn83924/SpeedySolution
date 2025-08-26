 // 1.1
// UNIT1 : Implementation of TOpenInfoImpl (CoClass: OpenInfo, Interface: IOpenInfo)

#include <vcl.h>
#pragma hdrstop

#include "OpeningInfo.h"

/////////////////////////////////////////////////////////////////////////////
// TOpenInfoImpl
STDMETHODIMP TOpenInfoImpl::get_Exchange(BSTR* Value)
{
  try
  {
	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IMatch);
  }
  return S_OK;
};


STDMETHODIMP TOpenInfoImpl::get_Market(EnumMarket* Value)
{
  try
  {
	*Value = FMarket;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOpenInfo);
  }
  return S_OK;
};


STDMETHODIMP TOpenInfoImpl::get_OpeningPx(double* Value)
{
  try
  {
	*Value = FOpenPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IOpenInfo);
  }
  return S_OK;
};


STDMETHODIMP TOpenInfoImpl::get_OpeningQty(long* Value)
{
  try
  {
	*Value = FOpenQty;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IOpenInfo);
  }
  return S_OK;
};


STDMETHODIMP TOpenInfoImpl::get_OpeningTime(BSTR* Value)
{
  try
  {
	WideString TimeStr( FTime.c_str() );
	*Value = TimeStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IOpenInfo);
  }
  return S_OK;
};


STDMETHODIMP TOpenInfoImpl::get_Symbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IMatch);
  }
  return S_OK;
};

void TOpenInfoImpl::FromOpeningInfo( OpeningInfo* Info )
{
	FExchange = Info->GetExchange();
	FSymbol = Info->GetSymbol();
	FMarket = (EnumMarket)Info->GetMarket();
	FOpenPx = Info->GetOpeningPrice();
	FOpenQty = Info->GetOpeningQty();
	FTime = Info->GetMatchTime();
}
