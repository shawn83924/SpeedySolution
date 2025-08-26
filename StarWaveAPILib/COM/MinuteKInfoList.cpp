 // 1.1
// UNIT1 : Implementation of TMinuteKInfoListImpl (CoClass: MinuteKInfoList, Interface: IMinuteKInfoList)
#include <vcl.h>
#pragma hdrstop
#include "MinuteKInfoList.h"
#include "MinuteKInfo.h"
/////////////////////////////////////////////////////////////////////////////
// TMinuteKInfoListImpl
//------------------------------------------------------------------------------
STDMETHODIMP TMinuteKInfoListImpl::get_Count(long* Value)
{
	try
	{
		if( FKBars != NULL )
			*Value = FKBars->ItemCount();
		else
			*Value = 0;
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IMinuteKInfoList);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TMinuteKInfoListImpl::get_Items(long Index, IMinuteKInfo** Value)
{
	try
	{
		if( FKBars != NULL && Index < FKBars->ItemCount() )
		{
			TMinuteKInfo Info = FKBars->GetItem( Index );
			TCOMIMinuteKInfo  MinuteKInfo = CoMinuteKInfo::Create();
			TMinuteKInfoImpl* CppMinuteKInfo = dynamic_cast<TMinuteKInfoImpl*>((IMinuteKInfo*)MinuteKInfo);

			CppMinuteKInfo->Copy( Info );
			CppMinuteKInfo->QueryInterface( IID_IMinuteKInfo, (void**)Value );
		}
		else
			return Error(  "Index out of bound", IID_IMinuteKInfoList);
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IMinuteKInfoList);
	}
	return S_OK;
};
//------------------------------------------------------------------------------


