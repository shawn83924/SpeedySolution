// ---------------------------------------------------------------------------
// SYSTEMMSGIMPL1 : Implementation of TSystemMsgImpl (CoClass: SystemMsg, Interface: ISystemMsg)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SystemMsgImpl.h"


// ---------------------------------------------------------------------------
// TSystemMsgImpl
// ---------------------------------------------------------------------------
__fastcall TSystemMsgImpl::TSystemMsgImpl()
{
}


// ---------------------------------------------------------------------------
// TSystemMsgImpl
// ---------------------------------------------------------------------------
__fastcall TSystemMsgImpl::TSystemMsgImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TSystemMsgImpl
// ---------------------------------------------------------------------------
__fastcall TSystemMsgImpl::TSystemMsgImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TSystemMsgImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TSystemMsgImpl>(Comserv::GetComServer(),
                           __classid(TSystemMsgImpl),
                           CLSID_SystemMsg,
                           Comobj::ciMultiInstance,
						   Comobj::tmApartment);
}
#pragma startup createFactory 32
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



