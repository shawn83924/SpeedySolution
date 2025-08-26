// UNIT1 : Implementation of TSecurityDefinitionRequestMessageImpl (CoClass: SecurityDefinitionRequestMessage, Interface: ISecurityDefinitionRequestMessage)

#include <vcl.h>
#pragma hdrstop

#include "SecurityDefinitionRequestMessageImpl.h"

/////////////////////////////////////////////////////////////////////////////
// TSecurityDefinitionRequestMessageImpl
STDMETHODIMP TSecurityDefinitionRequestMessageImpl::set_RequestMarket(BSTR Param1)





























{
  try
  {
	AnsiString	ReqMarket( Param1 );
	FMessage.SetRequestMarket( ReqMarket.c_str() );
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinitionRequestMessage);
  }
  return S_OK;
};
//-----------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionRequestMessageImpl::get_Instance(VARIANT* Value)
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
	return Error(e.Message.c_str(), IID_IMDBaseMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionRequestMessageImpl::get_Market(BSTR* Value)
{
  try
  {
	WideString	Market( FMessage.GetMarket() );
	*Value = Market.c_bstr();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMDBaseMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------




