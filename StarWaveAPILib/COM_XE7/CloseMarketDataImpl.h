// ---------------------------------------------------------------------------
// CloseMarketDataImpl.h : Declaration of the TCloseMarketDataImpl
// ---------------------------------------------------------------------------
#ifndef CloseMarketDataImplH
#define CloseMarketDataImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"

#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TCloseMarketDataImpl     Implements ICloseMarketData, default interface of CloseMarketData
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TCloseMarketDataImpl : public TCppAutoObject<ICloseMarketData>
{
  typedef _COM_CLASS inherited;
private:
	UFC::AnsiString			FExchange;
	UFC::AnsiString			FSymbol;
	EnumMarket				FMarket;
	double					FTermHighPx;
	double					FTermLowPx;
	double					FDayHighPx;
	double					FDayLowPx;
	double					FOpenPx;
	double					FBuyPx;
	double					FSellPx;
	double					FClosePx;
	int						FBuyCountTotal;
	int						FBuyQtyTotal;
	int						FSellCountTotal;
	int						FSellQtyTotal;
	int						FTotalCount;
	int						FTotalQty;
	double					FSettlementPx;
	int						FOpenInterest;

	void FillClosingData( ClosingMarketData* Data );
public:
	void FromI070( ClosingMarketData* Data );
	void FromI071( ClosingMarketDataWithSettlementPrice* Data );
	void FromI072( ClosingMarketDataWithSettlementPriceAndOpenInterest* Data );
	void FromI073( ClosingMarketData* Data ){}
public:
  __fastcall TCloseMarketDataImpl();
  __fastcall TCloseMarketDataImpl(const System::_di_IInterface Controller);
  __fastcall TCloseMarketDataImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // ICloseMarketData
protected:
  STDMETHOD(get_BuyCountTotal(long* Value));
  STDMETHOD(get_BuyPx(double* Value));
  STDMETHOD(get_BuyQtyTotal(long* Value));
  STDMETHOD(get_ClosePx(double* Value));
  STDMETHOD(get_DayHighPx(double* Value));
  STDMETHOD(get_DayLowPx(double* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_OpenInterest(long* Value));
  STDMETHOD(get_OpenPx(double* Value));
  STDMETHOD(get_SellCountTotal(long* Value));
  STDMETHOD(get_SellPx(double* Value));
  STDMETHOD(get_SellQtyTotal(long* Value));
  STDMETHOD(get_SettlementPx(double* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_TermHighPx(double* Value));
  STDMETHOD(get_TermLowPx(double* Value));
  STDMETHOD(get_TotalCount(long* Value));
  STDMETHOD(get_TotalQty(long* Value));
};




#endif //CloseMarketDataImplH
