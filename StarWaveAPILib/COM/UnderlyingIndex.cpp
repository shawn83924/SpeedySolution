 // 1.1
// UNIT1 : Implementation of TUnderlyingIndexImpl (CoClass: UnderlyingIndex, Interface: IUnderlyingIndex)

#include <vcl.h>
#pragma hdrstop

#include "UnderlyingIndex.h"

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


void TUnderlyingIndexImpl::FromIndexMessage( UnderlyingIndexInfo* Info )
{
    FExchange = Info->GetExchange();
	FIndexKind = Info->GetSymbol();
	FIndexValue = Info->GetIndexValue();
	FIndexTime = Info->GetShowTime();
	FMarket = (EnumMarket)Info->GetMarket();
}


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



