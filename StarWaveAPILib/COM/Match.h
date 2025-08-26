// 1.2
// Unit1.h : Declaration of the TMatchImpl

#ifndef MatchH
#define MatchH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
/////////////////////////////////////////////////////////////////////////////
// TMatchImpl     Implements IMatch, default interface of Match
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.Match
// Description    : Match Information
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TMatchImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TMatchImpl, &CLSID_Match>,
  public IDispatchImpl<IMatch, &IID_IMatch, &LIBID_StarWaveAPI>
{
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	UFC::AnsiString	FTime; ///< HH:MM:ss.mmm
	double			FMatchPx;
	int				FMatchQty;
	UFC::AnsiString FMatchTime;
public:
  TMatchImpl()
  {
  }
  void FromMatchInfo( MatchInfo* Info );
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.Match");
  DECLARE_DESCRIPTION("Match Information");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TMatchImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TMatchImpl)
  COM_INTERFACE_ENTRY(IMatch)
  COM_INTERFACE_ENTRY2(IDispatch, IMatch)
END_COM_MAP()

// IMatch
public:

  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_MatchPx(double* Value));
  STDMETHOD(get_MatchQty(long* Value));
  STDMETHOD(get_MatchTime(BSTR* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_Time(BSTR* Value));
};

#endif //Unit1H
