// 1.2
// Unit1.h : Declaration of the TSecurityDefinitionImpl

#ifndef SecurityDefinitionH
#define SecurityDefinitionH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "Utility.h"

/////////////////////////////////////////////////////////////////////////////
// TSecurityDefinitionImpl     Implements ISecurityDefinition, default interface of SecurityDefinition
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Default ProgID : StarWaveAPI.SecurityDefinition
// Description    : Security Definition
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TSecurityDefinitionImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TSecurityDefinitionImpl, &CLSID_SecurityDefinition>,
  public IDispatchImpl<ISecurityDefinition, &IID_ISecurityDefinition, &LIBID_StarWaveAPI>
{
private:
	double			FBullPx;
	double			FRefPx;
	double			FBearPx;
	double			FBullPx2;
	double			FBearPx2;
	double			FBullPx3;
	double			FBearPx3;
	double			FStrikePx;
	EnumMarket		FMarket;
	EnumCallPut		FCallPut;
	double			FContractMultiplier;
	bool			FIsMultileg;
	UFC::AnsiString FExchange;
	UFC::AnsiString FSymbol;
	UFC::AnsiString FProductID;
	UFC::AnsiString FEndDate;
	UFC::AnsiString FMaturityDate;
	UFC::AnsiString FMaturityDate2;
	int 			FDigit;
	int             FDecimalLocator;
	double			FTick;
	UFC::AnsiString FCurrency;
	UFC::AnsiString FTradeSymbol;
	UFC::AnsiString FUnderlying;
	bool			FIsWarrant;
	int				FExecuteQty;
	int				FCancelQty;
	int				FShares;
	double			FExecuteRate;
	int				FTradeUnit;
	UFC::AnsiString FChineseName;
	bool			FTradeFlag;
	UFC::AnsiString FLastTradeDate;
	int 			FTimeDiff;
	int				FExpiryDays;
	int             FMaxHistoryVolDays;
	double          FPreNChangeRange[MAX_PRE_CLOSE_DAYS];
public:
  TSecurityDefinitionImpl()
  {
  }
  void FromBasicInfo( BasicInformation* Info );
  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.SecurityDefinition");
  DECLARE_DESCRIPTION("Security Definition");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
	TTypedComServerRegistrarT<TSecurityDefinitionImpl>
	regObj(GetObjectCLSID(), GetProgID(), GetDescription());
	return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TSecurityDefinitionImpl)
  COM_INTERFACE_ENTRY(ISecurityDefinition)
  COM_INTERFACE_ENTRY2(IDispatch, ISecurityDefinition)
END_COM_MAP()

// ISecurityDefinition
public:

  STDMETHOD(get_BearPx(double* Value));
  STDMETHOD(get_BullPx(double* Value));
  STDMETHOD(get_CallPut(EnumCallPut* Value));
  STDMETHOD(get_ContractMultiplier(double* Value));
  STDMETHOD(get_EndDate(BSTR* Value));
  STDMETHOD(get_Exchange(BSTR* Value));
  STDMETHOD(get_Market(EnumMarket* Value));
  STDMETHOD(get_MaturityDate(BSTR* Value));
  STDMETHOD(get_MaturityDate2(BSTR* Value));
  STDMETHOD(get_ProductID(BSTR* Value));
  STDMETHOD(get_RefPx(double* Value));
  STDMETHOD(get_Symbol(BSTR* Value));
  STDMETHOD(get_IsMultileg(VARIANT_BOOL* Value));
  STDMETHOD(get_StrikePx(double* Value));
  STDMETHOD(get_Digit(long* Value));
  STDMETHOD(get_Tick(double* Value));
  STDMETHOD(get_Currency(BSTR* Value));
  STDMETHOD(get_TradeSymbol(BSTR* Value));
  STDMETHOD(get_Underlying(BSTR* Value));
  STDMETHOD(get_CancelQty(long* Value));
  STDMETHOD(get_ExecuteQty(long* Value));
  STDMETHOD(get_ExecuteRate(double* Value));
  STDMETHOD(get_IsWarrant(VARIANT_BOOL* Value));
  STDMETHOD(get_Shares(long* Value));
  STDMETHOD(get_TradeUnit(long* Value));
  STDMETHOD(get_ChineseName(BSTR* Value));
  STDMETHOD(get_TradeFlag(VARIANT_BOOL* Value));
  STDMETHOD(get_LastTradeDate(BSTR* Value));
  STDMETHOD(get_DecimalLocator(long* Value));
  STDMETHOD(get_BearPx2(double* Value));
  STDMETHOD(get_BearPx3(double* Value));
  STDMETHOD(get_BullPx2(double* Value));
  STDMETHOD(get_BullPx3(double* Value));
  STDMETHOD(get_TimeDiff(long* Value));
  STDMETHOD(get_ExpiryDays(long* Value));
  STDMETHOD(get_HistoryVol(long Days, double* Value));
  STDMETHOD(get_MaxHistoryVolDays(long* Value));
};

#endif //Unit1H
