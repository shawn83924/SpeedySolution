// UNIT1 : Implementation of TMarketDataMessageImpl (CoClass: MarketDataMessage, Interface: IMarketDataMessage)

#include <vcl.h>
#pragma hdrstop

#include "MarketDataMessageImpl.h"

/////////////////////////////////////////////////////////////////////////////
// TMarketDataMessageImpl
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_Instance(VARIANT* Value)
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

STDMETHODIMP TMarketDataMessageImpl::get_Market(BSTR* Value)
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
STDMETHODIMP TMarketDataMessageImpl::get_Symbol(BSTR* Value)
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
STDMETHODIMP TMarketDataMessageImpl::get_BidPrice(long Depth, double* Value)
{
  try
  {
	*Value = FMessage.GetBidPrice( Depth );
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_BidSize(long Depth, long* Value)
{
  try
  {
  	*Value = FMessage.GetBidSize( Depth );
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_ClosingPrice(double* Value)
{
  try
  {
  	*Value = FMessage.GetClosingPrice();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_OfferPrice(long Depth, double* Value)
{
  try
  {
	*Value = FMessage.GetOfferPrice( Depth );
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_OfferSize(long Depth, long* Value)
{
  try
  {
	*Value = FMessage.GetOfferSize( Depth );
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_OpeningPrice(double* Value)
{
  try
  {
	*Value = FMessage.GetOpeningPrice();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_OpenInterest(long* Value)
{
  try
  {
	*Value = FMessage.GetOpenInterest();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_SettlementPrice(double* Value)
{
  try
  {
	*Value = FMessage.GetSettlementPrice();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_TradePrice(double* Value)
{
  try
  {
	*Value = FMessage.GetTradePrice();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_TradeSize(long* Value)
{
  try
  {
	*Value = FMessage.GetTradeSize();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_TradeVolume(long* Value)
{
  try
  {
	*Value = FMessage.GetTradeVolume();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_TradingSessionHighPrice(double* Value)
{
  try
  {
	*Value = FMessage.GetTradingSessionHighPrice();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_TradingSessionLowPrice(double* Value)
{
  try
  {
	*Value = FMessage.GetTradingSessionLowPrice();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_Depth(long* Value)
{
  try
  {
	*Value = FMessage.GetMarketDepth();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_SecurityID(BSTR* Value)
{
  try
  {
	WideString	SecurityID( FMessage.GetSecurityID() );
	*Value = SecurityID.c_bstr();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_MaturityDate(long* Value)
{
  try
  {
	*Value = FMessage.GetMaturityDate();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------


STDMETHODIMP TMarketDataMessageImpl::get_ReferencePrice(double* Value)
{
  try
  {
	*Value = FMessage.GetRefPrice();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------


STDMETHODIMP TMarketDataMessageImpl::get_FallLimitPrice(double* Value)
{
  try
  {
	*Value = FMessage.GetFallLimitPrice();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};

//------------------------------------------------------------------------------
STDMETHODIMP TMarketDataMessageImpl::get_RiseLimitPrice(double* Value)
{
  try
  {
	*Value = FMessage.GetRiseLimitPrice();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------

STDMETHODIMP TMarketDataMessageImpl::get_StrikePrice(double* Value)
{
  try
  {
  	*Value = FMessage.GetStrikePrice(); 
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMarketDataMessage);
  }
  return S_OK;
};
//------------------------------------------------------------------------------


