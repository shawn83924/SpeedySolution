// ---------------------------------------------------------------------------
// MINUTEKINFOLISTIMPL : Implementation of TMinuteKInfoListImpl (CoClass: MinuteKInfoList, Interface: IMinuteKInfoList)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MinuteKInfoListImpl.h"
#include "MinuteKInfoImpl.h"

// ---------------------------------------------------------------------------
// TMinuteKInfoListImpl
// ---------------------------------------------------------------------------
__fastcall TMinuteKInfoListImpl::TMinuteKInfoListImpl()
:FKBars( NULL )
{
}


// ---------------------------------------------------------------------------
// TMinuteKInfoListImpl
// ---------------------------------------------------------------------------
__fastcall TMinuteKInfoListImpl::TMinuteKInfoListImpl(const System::_di_IInterface Controller)
							  : inherited(Controller),FKBars( NULL )
{
}


// ---------------------------------------------------------------------------
// TMinuteKInfoListImpl
// ---------------------------------------------------------------------------
__fastcall TMinuteKInfoListImpl::TMinuteKInfoListImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller),FKBars( NULL )
{
}

// ---------------------------------------------------------------------------
// TMinuteKInfoListImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TMinuteKInfoListImpl>(Comserv::GetComServer(),
                           __classid(TMinuteKInfoListImpl),
                           CLSID_MinuteKInfoList,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
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
			//TCOMIMinuteKInfo  MinuteKInfo = CoMinuteKInfo::Create();
			TMinuteKInfoImpl* CppMinuteKInfo = new TMinuteKInfoImpl();//dynamic_cast<TMinuteKInfoImpl*>((IMinuteKInfo*)MinuteKInfo);

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


