// ---------------------------------------------------------------------------
// TOTALVOLUMEIMPL : Implementation of TTotalVolumeImpl (CoClass: TotalVolume, Interface: ITotalVolume)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TotalVolumeImpl.h"


// ---------------------------------------------------------------------------
// TTotalVolumeImpl
// ---------------------------------------------------------------------------
__fastcall TTotalVolumeImpl::TTotalVolumeImpl()
{
}


// ---------------------------------------------------------------------------
// TTotalVolumeImpl
// ---------------------------------------------------------------------------
__fastcall TTotalVolumeImpl::TTotalVolumeImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TTotalVolumeImpl
// ---------------------------------------------------------------------------
__fastcall TTotalVolumeImpl::TTotalVolumeImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TTotalVolumeImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TTotalVolumeImpl>(Comserv::GetComServer(),
                           __classid(TTotalVolumeImpl),
                           CLSID_TotalVolume,
						   Comobj::ciMultiInstance,
						   Comobj::tmApartment);
}
#pragma startup createFactory 32


/////////////////////////////////////////////////////////////////////////////
// TTotalVolumnImpl
//------------------------------------------------------------------------------
void TTotalVolumeImpl::FromTotalMatch( TotalMatch* Match )
{
	FExchange       = Match->GetExchange();
	FSymbol         = Match->GetSymbol();
	FMarket         = (EnumMarket)Match->GetMarket();
	FTotalVolume    = Match->GetTotalMatchQty();
	FMatchBuyCount  = Match->GetBuyMatchCount();
	FMatchSellCount = Match->GetSellMatchCount();
	FIsTestMatch    = Match->IsTestMatch();
	FTradingSession = (EnumTradingSession)Match->GetTradingSession();
}
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
STDMETHODIMP TTotalVolumeImpl::get_BuyMatchCount(long* Value)
{
	*Value = FMatchBuyCount;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTotalVolumeImpl::get_Market(EnumMarket* Value)
{
	*Value = FMarket;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTotalVolumeImpl::get_SellMatchCount(long* Value)
{
	*Value = FMatchSellCount;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTotalVolumeImpl::get_TotalMatchVolume(long* Value)
{
	*Value = FTotalVolume;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTotalVolumeImpl::get_IsTestMatch(VARIANT_BOOL* Value)
{
	if( FIsTestMatch == true )
	   *Value = VARIANT_TRUE;
	else
	   *Value = VARIANT_FALSE;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TTotalVolumeImpl::get_TradingSession(EnumTradingSession* Value)
{
	*Value = FTradingSession;
	return S_OK;
}
//------------------------------------------------------------------------------



