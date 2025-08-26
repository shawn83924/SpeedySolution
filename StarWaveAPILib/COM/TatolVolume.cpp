 // 1.1
// UNIT2 : Implementation of TTotalVolumnImpl (CoClass: TotalVolumn, Interface: ITotalVolumn)

#include <vcl.h>
#pragma hdrstop

#include "TatolVolume.h"

/////////////////////////////////////////////////////////////////////////////
// TTotalVolumnImpl
STDMETHODIMP TTotalVolumeImpl::get_Exchange(BSTR* Value)
{
  try
  {
	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ITotalVolume);
  }
  return S_OK;
};


STDMETHODIMP TTotalVolumeImpl::get_Symbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ITotalVolume);
  }
  return S_OK;
};


STDMETHODIMP TTotalVolumeImpl::get_BuyMatchCount(long* Value)
{
  try
  {
	*Value = FMatchBuyCount;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ITotalVolume);
  }
  return S_OK;
};


STDMETHODIMP TTotalVolumeImpl::get_Market(EnumMarket* Value)
{
  try
  {
	*Value = FMarket;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ITotalVolume);
  }
  return S_OK;
};


STDMETHODIMP TTotalVolumeImpl::get_SellMatchCount(long* Value)
{
  try
  {
	*Value = FMatchSellCount;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ITotalVolume);
  }
  return S_OK;
};


STDMETHODIMP TTotalVolumeImpl::get_TotalMatchVolume(long* Value)
{
  try
  {
	*Value = FTotalVolume;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ITotalVolume);
  }
  return S_OK;
};


void TTotalVolumeImpl::FromTotalMatch( TotalMatch* Match )
{
	FExchange = Match->GetExchange();
	FSymbol = Match->GetSymbol();
	FMarket = (EnumMarket)Match->GetMarket();
	FTotalVolume = Match->GetTotalMatchQty();
	FMatchBuyCount = Match->GetBuyMatchCount();
	FMatchSellCount = Match->GetSellMatchCount();
}
