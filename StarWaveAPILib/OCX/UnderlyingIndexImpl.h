// ---------------------------------------------------------------------------
// UnderlyingIndexImpl.h : Declaration of the TUnderlyingIndexImpl
// ---------------------------------------------------------------------------
#ifndef UnderlyingIndexImplH
#define UnderlyingIndexImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"

#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TUnderlyingIndexImpl     Implements IUnderlyingIndex, default interface of UnderlyingIndex
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TUnderlyingIndexImpl : public TCppAutoObject<IUnderlyingIndex>
{
  typedef _COM_CLASS inherited;

private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FIndexKind;
	double			FIndexValue;
	double			FBuyValue;
	double			FSellValue;
	double			FFixValue;
	UFC::AnsiString FIndexTime;
	UFC::AnsiString FExchangeDate;
	EnumMarket		FMarket;
	int             FTotalQty;
	int             FTotalCount;
	UFC::Int64      FTotalAmount;
public:
	void FromIndexMessage( UnderlyingIndexInfo* Info );
public:
  __fastcall TUnderlyingIndexImpl();
  __fastcall TUnderlyingIndexImpl(const System::_di_IInterface Controller);
  __fastcall TUnderlyingIndexImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  // IUnderlyingIndex
protected:
  STDMETHOD(get_IndexKind(BSTR* Value));
  STDMETHOD(get_IndexTime(BSTR* Value));
  STDMETHOD(get_IndexValue(double* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_BuyValue(double* Value));
  STDMETHOD(get_ExchangeDate(BSTR* Value));
  STDMETHOD(get_FixValue(double* Value));
  STDMETHOD(get_SellValue(double* Value));
  STDMETHOD(get_TotalAmount(unsigned_int64* Value));
  STDMETHOD(get_TotalCount(long* Value));
  STDMETHOD(get_TotalQty(long* Value));
};




#endif //UnderlyingIndexImplH
