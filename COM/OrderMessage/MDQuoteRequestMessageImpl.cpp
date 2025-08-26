// UNIT1 : Implementation of TMDQuoteRequestMessageImpl (CoClass: MDQuoteRequestMessage, Interface: IMDQuoteRequestMessage)

#include <vcl.h>
#pragma hdrstop

#include "MDQuoteRequestMessageImpl.h"

/////////////////////////////////////////////////////////////////////////////
// TMDQuoteRequestMessageImpl
STDMETHODIMP TMDQuoteRequestMessageImpl::get_Instance(VARIANT* Value)
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

STDMETHODIMP TMDQuoteRequestMessageImpl::get_Market(BSTR* Value)
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

STDMETHODIMP TMDQuoteRequestMessageImpl::get_Symbol(BSTR* Value)
{
  try
  {
	WideString	Symbol( FMessage.GetSymbol() );
	*Value = Symbol.c_bstr();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMDBaseMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMDQuoteRequestMessageImpl::get_SecurityID(BSTR* Value)
{
  try
  {
	WideString	SecurityID( FMessage.GetSecurityID() );
	*Value = SecurityID.c_bstr();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_IMDQuoteRequestMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------


