// ---------------------------------------------------------------------------
// MatchImpl.h : Declaration of the TMatchImpl
// ---------------------------------------------------------------------------
#ifndef MatchImplH
#define MatchImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"

#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TMatchImpl     Implements IMatch, default interface of Match
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TMatchImpl : public TCppAutoObject<IMatch>
{
  typedef _COM_CLASS inherited;
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	UFC::AnsiString	FTime; ///< HH:MM:ss.mmm
	double			FMatchPx;
	int				FMatchQty;
	UFC::AnsiString FMatchTime;
public:
	void FromMatchInfo( MatchInfo* Info );
public:
  __fastcall TMatchImpl();
  __fastcall TMatchImpl(const System::_di_IInterface Controller);
  __fastcall TMatchImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // IMatch
protected:
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_MatchPx(double* Value));
  STDMETHOD(get_MatchQty(long* Value));
  STDMETHOD(get_MatchTime(BSTR* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_Time(BSTR* Value));
};




#endif //MatchImplH
