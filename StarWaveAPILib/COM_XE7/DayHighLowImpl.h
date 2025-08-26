// ---------------------------------------------------------------------------
// DayHighLowImpl.h : Declaration of the TDayHighLowImpl
// ---------------------------------------------------------------------------
#ifndef DayHighLowImplH
#define DayHighLowImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TDayHighLowImpl     Implements IDayHighLow, default interface of DayHighLow
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TDayHighLowImpl : public TCppAutoObject<IDayHighLow>
{
  typedef _COM_CLASS inherited;
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	double			FDayHighPx;
	double			FDayLowPx;
public:
	void FromDayHighLow( DayHighLowPrice* DayHighLow );
public:
  __fastcall TDayHighLowImpl();
  __fastcall TDayHighLowImpl(const System::_di_IInterface Controller);
  __fastcall TDayHighLowImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // IDayHighLow
protected:
  STDMETHOD(get_DayHighPx(double* Value));
  STDMETHOD(get_DayLowPx(double* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
};




#endif //DayHighLowImplH
