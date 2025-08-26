// ---------------------------------------------------------------------------
// SYSTEMMSGIMPL : Implementation of TErrorMsgImpl (CoClass: ErrorMsg, Interface: IErrorMsg)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "ErrorMsgImpl.h"

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// TErrorMsgImpl
STDMETHODIMP TErrorMsgImpl::get_ErrorCode(long* Value)
{
  try
  {
	*Value = FErrorCode;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IErrorMsg);
  }
  return S_OK;
}


STDMETHODIMP TErrorMsgImpl::get_ErrorMsg(BSTR* Value)
{
  try
  {
	WideString DataStr( FErrorMsg.c_str() );
	*Value = DataStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IErrorMsg);
  }
  return S_OK;
}

STDMETHODIMP TErrorMsgImpl::get_Exchange(BSTR* Value)
{
  try
  {
   	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IErrorMsg);
  }
  return S_OK;
}


STDMETHODIMP TErrorMsgImpl::get_Market(EnumMarket* Value)
{
  try
  {
  	*Value = FMarket;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IErrorMsg);
  }
  return S_OK;
}

void TErrorMsgImpl::FromMarketDataMessage( ErrorMessage* Msg )
{
	FExchange = Msg->GetExchange();
	FMarket = (EnumMarket)Msg->GetMarket();
	FErrorCode = Msg->GetErrorCode();
	FErrorMsg = Msg->GetErrorMsg();
	FProductID = Msg->GetProductID();
	FStrikePx = Msg->GetStrikePx();
	FCallPut = 	(EnumCallPut)Msg->GetCallPut();
	FSettleMonth = Msg->GetSettleMonth();
}

STDMETHODIMP TErrorMsgImpl::get_CallPut(EnumCallPut* Value)
{
  try
  {
	*Value = FCallPut;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IErrorMsg);
  }
  return S_OK;
};

STDMETHODIMP TErrorMsgImpl::get_ProductID(BSTR* Value)
{
  try
  {
	WideString IDStr( FProductID.c_str() );
	*Value = IDStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IErrorMsg);
  }
  return S_OK;
};

STDMETHODIMP TErrorMsgImpl::get_StrikePx(double* Value)
{
  try
  {
	*Value = FStrikePx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IErrorMsg);
  }
  return S_OK;
};

STDMETHODIMP TErrorMsgImpl::get_SettleDate(BSTR* Value)
{
  try
  {
	WideString DateStr( FSettleMonth.c_str() );
	*Value = DateStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IErrorMsg);
  }
  return S_OK;
};
