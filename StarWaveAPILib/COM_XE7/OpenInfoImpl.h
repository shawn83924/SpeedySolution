// ---------------------------------------------------------------------------
// OpenInfoImpl.h : Declaration of the TOpenInfoImpl
// ---------------------------------------------------------------------------
#ifndef OpenInfoImplH
#define OpenInfoImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"

#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TOpenInfoImpl     Implements IOpenInfo, default interface of OpenInfo
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TOpenInfoImpl : public TCppAutoObject<IOpenInfo>
{
  typedef _COM_CLASS inherited;
private:
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	EnumMarket		FMarket;
	double			FOpenPx;
	int				FOpenQty;
	UFC::AnsiString FTime;

public:
	void FromOpeningInfo( OpeningInfo* Info );
public:
  __fastcall TOpenInfoImpl();
  __fastcall TOpenInfoImpl(const System::_di_IInterface Controller);
  __fastcall TOpenInfoImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // IOpenInfo
protected:
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_OpeningPx(double* Value));
  STDMETHOD(get_OpeningQty(long* Value));
  STDMETHOD(get_OpeningTime(BSTR* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
};




#endif //OpenInfoImplH
