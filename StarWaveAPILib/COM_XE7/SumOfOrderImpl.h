// ---------------------------------------------------------------------------
// SumOfOrderImpl.h : Declaration of the TSumOfOrderImpl
// ---------------------------------------------------------------------------
#ifndef SumOfOrderImplH
#define SumOfOrderImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TSumOfOrderImpl     Implements ISumOfOrder, default interface of SumOfOrder
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TSumOfOrderImpl : public TCppAutoObject<ISumOfOrder>
{
  typedef _COM_CLASS inherited;
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	int				FBuyOrderCount;
	int				FSellOrderCount;
	int				FBuyOrderQty;
	int				FSellOrderQty;
public:
	void FromSumOfOrderInfo( SumOfOrderInfo* Info );
public:
  __fastcall TSumOfOrderImpl();
  __fastcall TSumOfOrderImpl(const System::_di_IInterface Controller);
  __fastcall TSumOfOrderImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // ISumOfOrder
protected:
  STDMETHOD(get_BuyOrderCount(long* Value));
  STDMETHOD(get_BuyOrderQty(long* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_SellOrderCount(long* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_SellOrderQty(long* Value));
};




#endif //SumOfOrderImplH
