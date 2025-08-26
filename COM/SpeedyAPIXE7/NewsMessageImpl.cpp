// ---------------------------------------------------------------------------
// NEWSMESSAGEIMPL : Implementation of TNewsMessageImpl (CoClass: NewsMessage, Interface: INewsMessage)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "NewsMessageImpl.h"
// ---------------------------------------------------------------------------
// TNewsMessageImpl
// ---------------------------------------------------------------------------
__fastcall TNewsMessageImpl::TNewsMessageImpl()
{
}
// ---------------------------------------------------------------------------
// TNewsMessageImpl
// ---------------------------------------------------------------------------
__fastcall TNewsMessageImpl::TNewsMessageImpl(const System::_di_IInterface Controller)
							  : inherited(Controller)
{
}
// ---------------------------------------------------------------------------
// TNewsMessageImpl
// ---------------------------------------------------------------------------
__fastcall TNewsMessageImpl::TNewsMessageImpl(Comobj::TComObjectFactory* Factory,
									  const System::_di_IInterface Controller)
							  : inherited(Factory, Controller)
{
}
// ---------------------------------------------------------------------------
// TNewsMessageImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TNewsMessageImpl>(Comserv::GetComServer(),
                           __classid(TNewsMessageImpl),
                           CLSID_NewsMessage,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
//------------------------------------------------------------------------------
void TNewsMessageImpl::AttachMessage( TNewsMessage* Msg )
{
	FMessage.Attach( Msg->GetMTree() );
}
/////////////////////////////////////////////////////////////////////////////
// TNewsMessageImpl
//------------------------------------------------------------------------------
STDMETHODIMP TNewsMessageImpl::get_Headline(BSTR* Value)
{
  try
  {
	UTF8String UTF8Str( FMessage.GetHeadline()  );
	WideString Headline( UTF8Str );
	*Value = Headline.c_bstr();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_INewsMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewsMessageImpl::set_Headline(BSTR Value)
{
  try
  {
	AnsiString Headline( Value );
	FMessage.SetHeadline( Headline.c_str() );
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_INewsMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewsMessageImpl::get_Text(BSTR* Value)
{
  try
  {
	UTF8String UTF8Str( FMessage.GetText()  );
	WideString Text( UTF8Str );
	*Value = Text.c_bstr();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_INewsMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewsMessageImpl::set_Text(BSTR Value)
{
  try
  {
	AnsiString Text( Value );
	FMessage.SetText( Text.c_str() );
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_INewsMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewsMessageImpl::get_Instance(VARIANT* Value)
{
  try
  {
	VARIANT MessageInstance;
	MessageInstance.vt = VT_BYREF;
	MessageInstance.byref = &FMessage;
	*Value = MessageInstance;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_INewsMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------

STDMETHODIMP TNewsMessageImpl::get_Market(BSTR* Value)
{
  try
  {
	WideString	Market( FMessage.GetMarket() );
	*Value = Market.c_bstr();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_INewsMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewsMessageImpl::get_ID(long* Value)
{
  try
  {
	 *Value = FMessage.GetID();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_INewsMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewsMessageImpl::set_ID(long Value)
{
  try
  {
	 FMessage.SetID( Value );
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_INewsMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewsMessageImpl::get_MsgCount(long* Value)
{
  try
  {
	 *Value = FMessage.GetMsgCount();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_INewsMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TNewsMessageImpl::get_MsgNum(long* Value)
{
  try
  {
	 *Value = FMessage.GetMsgNum();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_INewsMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
