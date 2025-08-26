// ---------------------------------------------------------------------------
// BrokerQueueImpl.h : Declaration of the TBrokerQueueImpl
// ---------------------------------------------------------------------------
#ifndef BrokerQueueImplH
#define BrokerQueueImplH

#include <System.Win.ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TBrokerQueueImpl     Implements IBrokerQueue, default interface of BrokerQueue
// ThreadingModel : tmApartment
// Dual Interface : FALSE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TBrokerQueueImpl : public TCppAutoObject<IBrokerQueue>
{
  typedef _COM_CLASS inherited;
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	int                 FCount;
	EnumBrokerQueueType FTypes[40];
	int                 FValues[40];
	bool                FIsBuy;
public:
	void FromBrokerQueue( HKBrokerQueue* Info );
public:
  __fastcall TBrokerQueueImpl();
  __fastcall TBrokerQueueImpl(const System::_di_IInterface Controller);
  __fastcall TBrokerQueueImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  // IBrokerQueue
protected:
  STDMETHOD(get_IsBuy(VARIANT_BOOL* Value));
  STDMETHOD(get_ItemType(long index, EnumBrokerQueueType* Value));
  STDMETHOD(get_ItemValue(long index, long* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
};




#endif //BrokerQueueImplH
