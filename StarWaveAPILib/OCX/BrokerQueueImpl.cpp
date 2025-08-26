// ---------------------------------------------------------------------------
// BROKERQUEUEIMPL : Implementation of TBrokerQueueImpl (CoClass: BrokerQueue, Interface: IBrokerQueue)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "BrokerQueueImpl.h"
// ---------------------------------------------------------------------------
// TBrokerQueueImpl
// ---------------------------------------------------------------------------
__fastcall TBrokerQueueImpl::TBrokerQueueImpl()
{
}


// ---------------------------------------------------------------------------
// TBrokerQueueImpl
// ---------------------------------------------------------------------------
__fastcall TBrokerQueueImpl::TBrokerQueueImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TBrokerQueueImpl
// ---------------------------------------------------------------------------
__fastcall TBrokerQueueImpl::TBrokerQueueImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TBrokerQueueImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TBrokerQueueImpl>(Comserv::GetComServer(),
						   __classid(TBrokerQueueImpl),
						   CLSID_BrokerQueue,
						   Comobj::ciMultiInstance,
						   Comobj::tmApartment);
}
#pragma startup createFactory 32
// ---------------------------------------------------------------------------
void TBrokerQueueImpl::FromBrokerQueue( HKBrokerQueue* Info )
{
	FExchange       = Info->GetExchange();
	FSymbol         = Info->GetSymbol();
	FMarket         = (EnumMarket)Info->GetMarket();
	FCount          = Info->Count();
	FIsBuy          = Info->IsBuy();
	for( int i = 0; i < 40; i++ )
	{
		FValues[i] = Info->GetValue( i );
		switch( Info->GetType( i ) )
		{
			case bqtBrokerID:       FTypes[i] = EnumBrokerQueueType::bqtBrokerID;break;
			case bqtNumberofSpread: FTypes[i] = EnumBrokerQueueType::bqtNumOfSpread;break;
			default:                FTypes[i] = EnumBrokerQueueType::bqtNull;break;
		}
	}
}
// ---------------------------------------------------------------------------
STDMETHODIMP TBrokerQueueImpl::get_IsBuy(VARIANT_BOOL* Value)
{
	if( FIsBuy == true )
	   *Value = VARIANT_TRUE;
	else
	   *Value = VARIANT_FALSE;
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TBrokerQueueImpl::get_ItemType(long index, EnumBrokerQueueType* Value)


















{
	if( index < 40 )
		*Value = (EnumBrokerQueueType)FTypes[index];
	else
		*Value = Starwaveapi_tlb::EnumBrokerQueueType::bqtNull;
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TBrokerQueueImpl::get_ItemValue(long index, long* Value)
{
	if( index < 40 )
		*Value = FValues[index];
	else
		*Value = 0;
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TBrokerQueueImpl::get_Exchange(BSTR* Value)
{
  try
  {
	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMatch);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TBrokerQueueImpl::get_Market(EnumMarket* Value)
{
	*Value = FMarket;
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TBrokerQueueImpl::get_Symbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IMatch);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------



