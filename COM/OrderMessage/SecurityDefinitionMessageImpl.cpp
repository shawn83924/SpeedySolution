// UNIT1 : Implementation of TSecurityDefinitionMessageImpl (CoClass: SecurityDefinitionMessage, Interface: ISecurityDefinitionMessage)

#include <vcl.h>
#pragma hdrstop

#include "SecurityDefinitionMessageImpl.h"

/////////////////////////////////////////////////////////////////////////////
// TSecurityDefinitionMessageImpl
STDMETHODIMP TSecurityDefinitionMessageImpl::get_SecurityID(long Index, BSTR* Value)






























{
  try
  {
	WideString	SecurityID( FMessage.GetSecurityID( Index ) );
	*Value = SecurityID.c_bstr();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinitionMessage);
  }
  return S_OK;
};
//-----------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionMessageImpl::get_Instance(VARIANT* Value)
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

STDMETHODIMP TSecurityDefinitionMessageImpl::get_Market(BSTR* Value)
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

STDMETHODIMP TSecurityDefinitionMessageImpl::get_SecurityIDCount(long* Value)
{
  try
  {
	*Value = FMessage.GetSymbolCount();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinitionMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionMessageImpl::get_MaturityDate(long Index, long* Value)






























{
  try
  {
	*Value = FMessage.GetMaturityDate( Index );
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinitionMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionMessageImpl::get_Symbol(long Index, BSTR* Value)
{
  try
  {
	WideString Symbol( FMessage.GetSymbol( Index ) );
	*Value = Symbol.c_bstr();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinitionMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionMessageImpl::get_ReferencePrice(long Index, double* Value)






























{
  try
  {
	*Value = FMessage.GetRefPrice( Index );
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinitionMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionMessageImpl::get_FallLimitPrice(long Index, double* Value)






























{
  try
  {
	*Value = FMessage.GetFallLimitPrice(Index);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinitionMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionMessageImpl::get_RiseLimitPrice(long Index, double* Value)






























{
  try
  {
	*Value = FMessage.GetRiseLimitPrice(Index);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinitionMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionMessageImpl::get_StrikePrice(long Index, double* Value)






























{
  try
  {
	*Value = FMessage.GetStrikePrice(Index);
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinitionMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------


