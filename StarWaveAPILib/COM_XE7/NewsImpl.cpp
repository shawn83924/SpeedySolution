// ---------------------------------------------------------------------------
// NEWSIMPL : Implementation of TNewsImpl (CoClass: News, Interface: INews)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "NewsImpl.h"


// ---------------------------------------------------------------------------
// TNewsImpl
// ---------------------------------------------------------------------------
__fastcall TNewsImpl::TNewsImpl()
:FKey(0)
{
}


// ---------------------------------------------------------------------------
// TNewsImpl
// ---------------------------------------------------------------------------
__fastcall TNewsImpl::TNewsImpl(const System::_di_IInterface Controller)
							  : inherited(Controller),FKey(0)
{
}


// ---------------------------------------------------------------------------
// TNewsImpl
// ---------------------------------------------------------------------------
__fastcall TNewsImpl::TNewsImpl(Comobj::TComObjectFactory* Factory,
									  const System::_di_IInterface Controller)
                              : inherited(Factory, Controller),FKey(0)
{
}

// ---------------------------------------------------------------------------
// TNewsImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TNewsImpl>(Comserv::GetComServer(),
                           __classid(TNewsImpl),
                           CLSID_News,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
/////////////////////////////////////////////////////////////////////////////
// TNewsImpl
 void TNewsImpl::FromNews( const UFC::AnsiString& Group,const UFC::AnsiString& Msg )
 {
	FGroup = Group;
	FData  = Msg;
 }

STDMETHODIMP TNewsImpl::get_Data(BSTR* Value)
{
  try
  {
	WideString Data( FData.c_str() );
	*Value = Data.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_INews);
  }
  return S_OK;
};


STDMETHODIMP TNewsImpl::get_Key(long* Value)
{
  try
  {
	*Value = FKey;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_INews);
  }
  return S_OK;
};


STDMETHODIMP TNewsImpl::get_Group(BSTR* Value)
{
  try
  {
	WideString Data( FGroup.c_str() );
	*Value = Data.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_INews);
  }
  return S_OK;
};

