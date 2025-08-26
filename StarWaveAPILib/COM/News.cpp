 // 1.1
// UNIT1 : Implementation of TNewsImpl (CoClass: News, Interface: INews)

#include <vcl.h>
#pragma hdrstop

#include "News.h"

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



