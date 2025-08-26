// 1.2
// Unit1.h : Declaration of the TSumOfOrderImpl

#ifndef SumOfOrderH
#define SumOfOrderH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
/////////////////////////////////////////////////////////////////////////////
// TSumOfOrderImpl     Implements ISumOfOrder, default interface of SumOfOrder
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.SumOfOrder
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TSumOfOrderImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TSumOfOrderImpl, &CLSID_SumOfOrder>,
  public IDispatchImpl<ISumOfOrder, &IID_ISumOfOrder, &LIBID_StarWaveAPI>
{
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	int				FBuyOrderCount;
	int				FSellOrderCount;
	int				FBuyOrderQty;
	int				FSellOrderQty;
public:
  TSumOfOrderImpl()
  {
  }
  void FromSumOfOrderInfo( SumOfOrderInfo* Info );
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.SumOfOrder");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TSumOfOrderImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TSumOfOrderImpl)
  COM_INTERFACE_ENTRY(ISumOfOrder)
  COM_INTERFACE_ENTRY2(IDispatch, ISumOfOrder)
END_COM_MAP()

// ISumOfOrder
public:

  STDMETHOD(get_BuyOrderCount(long* Value));
  STDMETHOD(get_BuyOrderQty(long* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_SellOrderCount(long* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_SellOrderQty(long* Value));
};

#endif //Unit1H
