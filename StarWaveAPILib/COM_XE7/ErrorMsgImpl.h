// ---------------------------------------------------------------------------
// ErrorMsgImpl1.h : Declaration of the TErrorMsgImpl
// ---------------------------------------------------------------------------
#ifndef ErrorMsgImplH
#define ErrorMsgImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TErrorMsgImpl     Implements IErrorMsg, default interface of ErrorMsg
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TErrorMsgImpl : public TCppAutoObject<IErrorMsg>
{
  typedef _COM_CLASS inherited;
private:
	UFC::AnsiString FExchange;
	int 			FErrorCode;
	UFC::AnsiString FErrorMsg;
	EnumMarket		FMarket;
	UFC::AnsiString FProductID;
	double			FStrikePx;
	EnumCallPut		FCallPut;
	UFC::AnsiString	FSettleMonth;
public:
	void FromMarketDataMessage( ErrorMessage* Msg );
public:
  __fastcall TErrorMsgImpl();
  __fastcall TErrorMsgImpl(const System::_di_IInterface Controller);
  __fastcall TErrorMsgImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  // IErrorMsg
protected:
  STDMETHOD(get_ErrorCode(long* Value));
  STDMETHOD(get_ErrorMsg(BSTR* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_CallPut(EnumCallPut* Value));
  STDMETHOD(get_ProductID(BSTR* Value));
  STDMETHOD(get_StrikePx(double* Value));
  STDMETHOD(get_SettleDate(BSTR* Value));
};




#endif //ErrorMsgImpl1H
