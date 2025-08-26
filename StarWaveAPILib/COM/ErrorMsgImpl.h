// ---------------------------------------------------------------------------
// ErrorMsgImpl.h : Declaration of the TErrorMsgImpl
// ---------------------------------------------------------------------------

#ifndef ErrorMsgImplH
#define ErrorMsgImplH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataMessage.h"
/////////////////////////////////////////////////////////////////////////////
// TErrorMsgImpl     Implements IErrorMsg, default interface of ErrorMsg
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    :
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TErrorMsgImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TErrorMsgImpl, &CLSID_ErrorMsg>,
  public IDispatchImpl<IErrorMsg, &IID_IErrorMsg, &LIBID_StarWaveAPI>
{
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
  TErrorMsgImpl()
  {
  }
  void FromMarketDataMessage( ErrorMessage* Msg );
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.ErrorMsg");
  DECLARE_DESCRIPTION("Error Message");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
	TTypedComServerRegistrarT<TErrorMsgImpl>
	regObj(GetObjectCLSID(), GetProgID(), GetDescription());
	return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TErrorMsgImpl)
  COM_INTERFACE_ENTRY(IErrorMsg)
  COM_INTERFACE_ENTRY2(IDispatch, IErrorMsg)
END_COM_MAP()

// IErrorMsg
public:
  STDMETHOD(get_ErrorCode(long* Value));
  STDMETHOD(get_ErrorMsg(BSTR* Value));

  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_CallPut(EnumCallPut* Value));
  STDMETHOD(get_ProductID(BSTR* Value));
  STDMETHOD(get_StrikePx(double* Value));
  STDMETHOD(get_SettleDate(BSTR* Value));
};

//---------------------------------------------------------------------------
#endif //ErrorMsgImplH
