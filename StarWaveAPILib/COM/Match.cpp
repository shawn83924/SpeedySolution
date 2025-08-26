 // 1.1
// UNIT1 : Implementation of TMatchImpl (CoClass: Match, Interface: IMatch)

#include <vcl.h>
#pragma hdrstop

#include "Match.h"

/////////////////////////////////////////////////////////////////////////////
// TMatchImpl
STDMETHODIMP TMatchImpl::get_Exchange(BSTR* Value)
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


STDMETHODIMP TMatchImpl::get_Market(EnumMarket* Value)
{
  try
  {
	*Value = FMarket;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IMatch);
  }
  return S_OK;
};


STDMETHODIMP TMatchImpl::get_MatchPx(double* Value)
{
  try
  {
	*Value = FMatchPx;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IMatch);
  }
  return S_OK;
};


STDMETHODIMP TMatchImpl::get_MatchQty(long* Value)
{
  try
  {
	*Value = FMatchQty;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IMatch);
  }
  return S_OK;
};


STDMETHODIMP TMatchImpl::get_MatchTime(BSTR* Value)
{
  try
  {
	WideString TimeStr( FMatchTime.c_str() );
	*Value = TimeStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IMatch);
  }
  return S_OK;
};


STDMETHODIMP TMatchImpl::get_Symbol(BSTR* Value)
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


void TMatchImpl::FromMatchInfo( MatchInfo* Info )
{
	FExchange = Info->GetExchange();
	FSymbol = Info->GetSymbol();
	FMarket = (EnumMarket)Info->GetMarket();
	FTime = Info->GetMessageTime();
	FMatchPx = Info->GetMatchPrice();
	FMatchQty = Info->GetMatchQty();
	FMatchTime = Info->GetMatchTime();
}

STDMETHODIMP TMatchImpl::get_Time(BSTR* Value)
{
  try
  {
	WideString TimeStr( FTime.c_str() );
	*Value = TimeStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IMatch);
  }
  return S_OK;
};



