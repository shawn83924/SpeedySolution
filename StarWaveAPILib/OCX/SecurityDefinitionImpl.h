// ---------------------------------------------------------------------------
// SecurityDefinitionImpl.h : Declaration of the TSecurityDefinitionImpl
// ---------------------------------------------------------------------------
#ifndef SecurityDefinitionImplH
#define SecurityDefinitionImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"

#include "Utility.h"
// ---------------------------------------------------------------------------
// TSecurityDefinitionImpl     Implements ISecurityDefinition, default interface of SecurityDefinition
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TSecurityDefinitionImpl : public TCppAutoObject<ISecurityDefinition>
{
  typedef _COM_CLASS inherited;
private:
	///< Double type
	double			FBullPx;
	double			FRefPx;
	double			FBearPx;
	double			FBullPx2;
	double			FBearPx2;
	double			FBullPx3;
	double			FBearPx3;
	double			FStrikePx;
	double			FContractMultiplier;
	double			FTick;
	double			FExecuteRate;
	double          FPreNChangeRange[MAX_PRE_CLOSE_DAYS];
	double          FSettlementPrice;
	double          FPreSettlementPrice;
	EnumMarket		FMarket;
	EnumCallPut		FCallPut;
	///< Boolean
	bool			FIsMultileg;
	bool			FIsWarrant;
	bool			FTradeFlag;
	///< String type
	WideString      FExchange;
	WideString      FSymbol;
	WideString      FProductID;
	WideString      FEndDate;
	WideString      FMaturityDate;
	WideString      FMaturityDate2;
	WideString      FCurrency;
	WideString      FTradeSymbol;
	WideString      FUnderlying;
	WideString      FChineseName;
	WideString      FLastTradeDate;
	int 			FDigit;
	int             FDecimalLocator;
	int				FExecuteQty;
	int				FCancelQty;
	int				FShares;
	int				FTradeUnit;
	int 			FTimeDiff;
	int				FExpiryDays;
	int             FMaxHistoryVolDays;
	int             FIsMostActive;
	int             FNearMonth;
public:
	void FromBasicInfo( BasicInformation* Info );
public:
  __fastcall TSecurityDefinitionImpl();
  __fastcall TSecurityDefinitionImpl(const System::_di_IInterface Controller);
  __fastcall TSecurityDefinitionImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);


  // ISecurityDefinition
protected:
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
  STDMETHOD(Copy(ISecurityDefinition* Ref));
  STDMETHOD(get_PrevClosePx(long Day, double* Value));
  STDMETHOD(get_IsMostActive(long* Value));
  STDMETHOD(get_NearMonth(long* Value));
  STDMETHOD(get_PreSettlementPx(double* Value));
  STDMETHOD(get_SettlementPx(double* Value));
};




#endif //SecurityDefinitionImplH
