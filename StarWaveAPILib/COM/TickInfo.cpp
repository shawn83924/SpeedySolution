 // 1.1
// UNIT1 : Implementation of TTickInfoImpl (CoClass: TickInfo, Interface: ITickInfo)

#include <vcl.h>
#pragma hdrstop

#include "TickInfo.h"

/////////////////////////////////////////////////////////////////////////////
// TTickInfoImpl
STDMETHODIMP TTickInfoImpl::get_BuyPx(double* Value)
{
	*Value = FInfo.BuyPx;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTickInfoImpl::get_FillPx(double* Value)
{
	*Value = FInfo.FillPx;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTickInfoImpl::get_FillQty(long* Value)
{
	*Value = FInfo.FillQty;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTickInfoImpl::get_ID(long* Value)
{
	*Value = FInfo.ID;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTickInfoImpl::get_SellPx(double* Value)
{
	*Value = FInfo.SellPx;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTickInfoImpl::get_Seq(long* Value)
{
	*Value = FInfo.Seq;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTickInfoImpl::get_Time(long* Value)
{
	*Value = FInfo.Time;
	return S_OK;
};
//------------------------------------------------------------------------------


