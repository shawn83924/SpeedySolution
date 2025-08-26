// ---------------------------------------------------------------------------
// MATCHIMPL : Implementation of TMatchImpl (CoClass: Match, Interface: IMatch)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MatchImpl.h"


// ---------------------------------------------------------------------------
// TMatchImpl
// ---------------------------------------------------------------------------
__fastcall TMatchImpl::TMatchImpl()
{
}


// ---------------------------------------------------------------------------
// TMatchImpl
// ---------------------------------------------------------------------------
__fastcall TMatchImpl::TMatchImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TMatchImpl
// ---------------------------------------------------------------------------
__fastcall TMatchImpl::TMatchImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TMatchImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TMatchImpl>(Comserv::GetComServer(),
                           __classid(TMatchImpl),
						   CLSID_Match,
						   Comobj::ciMultiInstance,
						   Comobj::tmApartment);
}
#pragma startup createFactory 32
/////////////////////////////////////////////////////////////////////////////
// TMatchImpl
//------------------------------------------------------------------------------
void TMatchImpl::FromMatchInfo( MatchInfo* Info )
{
	FExchange       = Info->GetExchange();
	FSymbol         = Info->GetSymbol();
	FMarket         = (EnumMarket)Info->GetMarket();
	FTime           = Info->GetMessageTime();
	FMatchPx        = Info->GetMatchPrice();
	FRefPx          = Info->GetRefPx();
	FMatchQty       = Info->GetMatchQty();
	FMatchTime      = Info->GetMatchTime();
	FIsTestMatch    = Info->IsTestMatch();
	FTradingSession = (EnumTradingSession)Info->GetTradingSession();
	FHKExTrdType    = (EnumHKExTrdType)Info->GetHKExTrdType();
}
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
STDMETHODIMP TMatchImpl::get_Market(EnumMarket* Value)
{
	*Value = FMarket;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMatchImpl::get_MatchPx(double* Value)
{
	*Value = FMatchPx;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMatchImpl::get_MatchQty(long* Value)
{
	*Value = FMatchQty;
	return S_OK;
};
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
STDMETHODIMP TMatchImpl::get_IsTestMatch(VARIANT_BOOL* Value)
{
	if( FIsTestMatch == true )
	   *Value = VARIANT_TRUE;
	else
	   *Value = VARIANT_FALSE;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TMatchImpl::get_TradingSession(EnumTradingSession* Value)
{
	*Value = FTradingSession;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TMatchImpl::get_HKExTrdType(EnumHKExTrdType* Value)
{
	*Value = FHKExTrdType;
    return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TMatchImpl::get_RefPrice(double* Value)
{
  *Value = FRefPx;
  return S_OK;
}
//------------------------------------------------------------------------------



