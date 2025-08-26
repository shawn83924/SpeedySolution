// ---------------------------------------------------------------------------
// MINUTEKINFOIMPL : Implementation of TMinuteKInfoImpl (CoClass: MinuteKInfo, Interface: IMinuteKInfo)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MinuteKInfoImpl.h"


// ---------------------------------------------------------------------------
// TMinuteKInfoImpl
// ---------------------------------------------------------------------------
__fastcall TMinuteKInfoImpl::TMinuteKInfoImpl()
{
}


// ---------------------------------------------------------------------------
// TMinuteKInfoImpl
// ---------------------------------------------------------------------------
__fastcall TMinuteKInfoImpl::TMinuteKInfoImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TMinuteKInfoImpl
// ---------------------------------------------------------------------------
__fastcall TMinuteKInfoImpl::TMinuteKInfoImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TMinuteKInfoImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TMinuteKInfoImpl>(Comserv::GetComServer(),
                           __classid(TMinuteKInfoImpl),
                           CLSID_MinuteKInfo,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
/////////////////////////////////////////////////////////////////////////////
// TMinuteKInfoImpl
STDMETHODIMP TMinuteKInfoImpl::get_ClosePx(double* Value)
{
	*Value = FInfo.ClosePx;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMinuteKInfoImpl::get_Hour(long* Value)
{
	*Value = FInfo.Hour;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMinuteKInfoImpl::get_MaxPx(double* Value)
{
	*Value = FInfo.MaxPx;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMinuteKInfoImpl::get_MinPx(double* Value)
{
	*Value = FInfo.MinPx;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMinuteKInfoImpl::get_Minutes(long* Value)
{
	*Value = FInfo.Minutes;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMinuteKInfoImpl::get_OpenPx(double* Value)
{
	*Value = FInfo.OpenPx;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMinuteKInfoImpl::get_Qty(long* Value)
{
	*Value = FInfo.Qty;
	return S_OK;
};
//------------------------------------------------------------------------------


