// ---------------------------------------------------------------------------
// SystemMsgImpl1.h : Declaration of the TSystemMsgImpl
// ---------------------------------------------------------------------------
#ifndef SystemMsgImplH
#define SystemMsgImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"

#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TSystemMsgImpl     Implements ISystemMsg, default interface of SystemMsg
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TSystemMsgImpl : public TCppAutoObject<ISystemMsg>
{
  typedef _COM_CLASS inherited;
private:
	UFC::AnsiString FExchange;
	int 			FCode;
	UFC::AnsiString FData;
	EnumMarket		FMarket;
public:
	void FromMarketDataMessage( SystemMessage* Msg );
public:
  __fastcall TSystemMsgImpl();
  __fastcall TSystemMsgImpl(const System::_di_IInterface Controller);
  __fastcall TSystemMsgImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // ISystemMsg
protected:
  STDMETHOD(get_Code(long* Value));
  STDMETHOD(get_Data(BSTR* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
};




#endif //SystemMsgImpl1H
