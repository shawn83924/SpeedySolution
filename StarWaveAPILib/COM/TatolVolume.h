// 1.2
// Unit2.h : Declaration of the TTotalVolumnImpl

#ifndef TatolVolumeH
#define TatolVolumeH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
/////////////////////////////////////////////////////////////////////////////
// TTotalVolumnImpl     Implements ITotalVolumn, default interface of TotalVolumn
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.TotalVolumn
// Description    : Total Match Volumn
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TTotalVolumeImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TTotalVolumeImpl, &CLSID_TotalVolume>,
  public IDispatchImpl<ITotalVolume, &IID_ITotalVolume, &LIBID_StarWaveAPI>
{
public:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	int				FTotalVolume;
	int				FMatchBuyCount;
	int				FMatchSellCount;
  TTotalVolumeImpl()
  {
  }
  void FromTotalMatch( TotalMatch* Match );
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.TotalVolume");
  DECLARE_DESCRIPTION("Total Match Volume");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
	TTypedComServerRegistrarT<TTotalVolumeImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TTotalVolumeImpl)
  COM_INTERFACE_ENTRY(ITotalVolume)
  COM_INTERFACE_ENTRY2(IDispatch, ITotalVolume)
  DUALINTERFACE_ENTRY(ITotalVolume)
END_COM_MAP()

// ITotalVolumn
public:

  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_BuyMatchCount(long* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_SellMatchCount(long* Value));
  STDMETHOD(get_TotalMatchVolume(long* Value));
};

#endif //Unit2H
