// ---------------------------------------------------------------------------
// OPENINFOIMPL : Implementation of TOpenInfoImpl (CoClass: OpenInfo, Interface: IOpenInfo)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "OpenInfoImpl.h"


// ---------------------------------------------------------------------------
// TOpenInfoImpl
// ---------------------------------------------------------------------------
__fastcall TOpenInfoImpl::TOpenInfoImpl()
{
}


// ---------------------------------------------------------------------------
// TOpenInfoImpl
// ---------------------------------------------------------------------------
__fastcall TOpenInfoImpl::TOpenInfoImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TOpenInfoImpl
// ---------------------------------------------------------------------------
__fastcall TOpenInfoImpl::TOpenInfoImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TOpenInfoImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TOpenInfoImpl>(Comserv::GetComServer(),
                           __classid(TOpenInfoImpl),
                           CLSID_OpenInfo,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
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

