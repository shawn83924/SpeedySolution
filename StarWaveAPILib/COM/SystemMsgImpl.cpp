// ---------------------------------------------------------------------------
// SYSTEMMSGIMPL : Implementation of TSystemMsgImpl (CoClass: SystemMsg, Interface: ISystemMsg)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SystemMsgImpl.h"


/////////////////////////////////////////////////////////////////////////////
// TSystemMsgImpl



STDMETHODIMP TSystemMsgImpl::get_Code(long* Value)
{
  try
  {
	*Value = FCode;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISystemMsg);
  }
  return S_OK;
}


STDMETHODIMP TSystemMsgImpl::get_Data(BSTR* Value)
{
  try
  {
  	WideString DataStr( FData.c_str() );
   	*Value = DataStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISystemMsg);
  }
  return S_OK;
}


STDMETHODIMP TSystemMsgImpl::get_Exchange(BSTR* Value)
{
  try
  {
   	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISystemMsg);
  }
  return S_OK;
}


STDMETHODIMP TSystemMsgImpl::get_Market(EnumMarket* Value)
{
  try
  {
  	*Value = FMarket;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISystemMsg);
  }
  return S_OK;
}

void TSystemMsgImpl::FromMarketDataMessage( SystemMessage* Msg )
{

}




