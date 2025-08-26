// ---------------------------------------------------------------------------
// TICKINFOLISTIMPL : Implementation of TTickInfoListImpl (CoClass: TickInfoList, Interface: ITickInfoList)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TickInfoListImpl.h"
#include "TickInfoImpl.h"

// ---------------------------------------------------------------------------
// TTickInfoListImpl
// ---------------------------------------------------------------------------
__fastcall TTickInfoListImpl::TTickInfoListImpl()
:FTicks( NULL )
{
}


// ---------------------------------------------------------------------------
// TTickInfoListImpl
// ---------------------------------------------------------------------------
__fastcall TTickInfoListImpl::TTickInfoListImpl(const System::_di_IInterface Controller)
							  : inherited(Controller),FTicks( NULL )
{
}


// ---------------------------------------------------------------------------
// TTickInfoListImpl
// ---------------------------------------------------------------------------
__fastcall TTickInfoListImpl::TTickInfoListImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
							  : inherited(Factory, Controller),FTicks( NULL )
{
}

// ---------------------------------------------------------------------------
// TTickInfoListImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TTickInfoListImpl>(Comserv::GetComServer(),
                           __classid(TTickInfoListImpl),
                           CLSID_TickInfoList,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
/////////////////////////////////////////////////////////////////////////////
// TTickInfoListImpl
//------------------------------------------------------------------------------
STDMETHODIMP TTickInfoListImpl::get_Count(long* Value)
{
	try
	{
		if( FTicks != NULL )
			*Value = FTicks->ItemCount();
		else
			*Value = 0;
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_ITickInfoList);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TTickInfoListImpl::get_Items(long Index, ITickInfo** Value)
{
	try
	{
		if( FTicks != NULL && Index < FTicks->ItemCount() )
		{
			TTickInfoImpl* TickInfoImpl = new TTickInfoImpl();
			TTickInfo Info = FTicks->GetItem( Index );

			TickInfoImpl->Copy( Info );
			TickInfoImpl->QueryInterface( IID_ITickInfo, (void**)Value );
			TickInfoImpl->Release();
		}
		else
			return Error(  "Index out of bound", IID_ITickInfoList);
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_ITickInfoList);
	}
	return S_OK;
};
//------------------------------------------------------------------------------


