// ---------------------------------------------------------------------------
// TotalVolumeImpl.h : Declaration of the TTotalVolumeImpl
// ---------------------------------------------------------------------------
#ifndef TotalVolumeImplH
#define TotalVolumeImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"

#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TTotalVolumeImpl     Implements ITotalVolume, default interface of TotalVolume
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TTotalVolumeImpl : public TCppAutoObject<ITotalVolume>
{
  typedef _COM_CLASS inherited;
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	int				FTotalVolume;
	int				FMatchBuyCount;
	int				FMatchSellCount;
public:
	void FromTotalMatch( TotalMatch* Match );
public:
  __fastcall TTotalVolumeImpl();
  __fastcall TTotalVolumeImpl(const System::_di_IInterface Controller);
  __fastcall TTotalVolumeImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // ITotalVolume
protected:
  STDMETHOD(get_BuyMatchCount(long* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_SellMatchCount(long* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_TotalMatchVolume(long* Value));
};




#endif //TotalVolumeImplH
