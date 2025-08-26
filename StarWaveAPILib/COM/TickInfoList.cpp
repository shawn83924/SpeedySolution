 // 1.1
// UNIT1 : Implementation of TTickInfoListImpl (CoClass: TickInfoList, Interface: ITickInfoList)
#include <vcl.h>
#pragma hdrstop
#include "TickInfoList.h"
#include "TickInfo.h"
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
			TTickInfo Info = FTicks->GetItem( Index );
			TCOMITickInfo  TickInfo = CoTickInfo::Create();
			TTickInfoImpl* CppTickInfo = dynamic_cast<TTickInfoImpl*>((ITickInfo*)TickInfo);

			CppTickInfo->Copy( Info );
			CppTickInfo->QueryInterface( IID_ITickInfo, (void**)Value );
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


