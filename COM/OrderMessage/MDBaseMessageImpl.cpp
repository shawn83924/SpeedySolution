// UNIT1 : Implementation of TMDBaseMessageImpl (CoClass: MDBaseMessage, Interface: IMDBaseMessage)

#include <vcl.h>
#pragma hdrstop

#include "MDBaseMessageImpl.h"

/////////////////////////////////////////////////////////////////////////////
// TMDBaseMessageImpl
STDMETHODIMP TMDBaseMessageImpl::get_Instance(VARIANT* Value)
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


STDMETHODIMP TMDBaseMessageImpl::get_Market(BSTR* Value)
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


