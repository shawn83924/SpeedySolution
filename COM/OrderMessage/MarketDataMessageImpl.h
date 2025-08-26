// Unit1.h : Declaration of the TMarketDataMessageImpl

#ifndef MarketDataMessageImplH
#define MarketDataMessageImplH

#define _ATL_APARTMENT_THREADED

#include "OrderMessage_TLB.h"
#include "TMarketDataMessage.h"

/////////////////////////////////////////////////////////////////////////////
// TMarketDataMessageImpl     Implements IMarketDataMessage, default interface of MarketDataMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : OrderMessage.MarketDataMessage
// Description    : 
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TMarketDataMessageImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TMarketDataMessageImpl, &CLSID_MarketDataMessage>,
  public IDispatchImpl<IMarketDataMessage, &IID_IMarketDataMessage, &LIBID_OrderMessage>
{
private:
	TMarketDataMessage	FMessage;
public:
  TMarketDataMessageImpl()
  {
  }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("OrderMessage.MarketDataMessage");
  DECLARE_DESCRIPTION("");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TMarketDataMessageImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


BEGIN_COM_MAP(TMarketDataMessageImpl)
  COM_INTERFACE_ENTRY(IMarketDataMessage)
  COM_INTERFACE_ENTRY2(IDispatch, IMarketDataMessage)
END_COM_MAP()

// IMDBaseMessage
public:
  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_Market(BSTR* Value));

// IMarketDataMessage
  STDMETHOD(get_BidPrice(long Depth, double* Value));
  STDMETHOD(get_BidSize(long Depth, long* Value));
  STDMETHOD(get_ClosingPrice(double* Value));
  STDMETHOD(get_OfferPrice(long Depth, double* Value));
  STDMETHOD(get_OfferSize(long Depth, long* Value));
  STDMETHOD(get_OpeningPrice(double* Value));
  STDMETHOD(get_OpenInterest(long* Value));
  STDMETHOD(get_SettlementPrice(double* Value));
  STDMETHOD(get_TradePrice(double* Value));
  STDMETHOD(get_TradeSize(long* Value));
  STDMETHOD(get_TradeVolume(long* Value));
  STDMETHOD(get_TradingSessionHighPrice(double* Value));
  STDMETHOD(get_TradingSessionLowPrice(double* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_Depth(long* Value));
  STDMETHOD(get_SecurityID(BSTR* Value));
  STDMETHOD(get_MaturityDate(long* Value));
  STDMETHOD(get_ReferencePrice(double* Value));
  STDMETHOD(get_FallLimitPrice(double* Value));
  STDMETHOD(get_RiseLimitPrice(double* Value));
  STDMETHOD(get_StrikePrice(double* Value));
public:

};

#endif //Unit1H
