// ---------------------------------------------------------------------------
// TriggeringConditionImpl.h : Declaration of the TTriggeringConditionImpl
// ---------------------------------------------------------------------------
#ifndef TriggeringConditionImplH
#define TriggeringConditionImplH

#include <System.Win.ComServ.hpp>
#include <axbase.h>
#include "SpeedyAPI_TLB.h"
#include "TTriggeringCondition.h"
#include "TDataFormat.h"

// ---------------------------------------------------------------------------
// TTriggeringConditionImpl     Implements ITriggeringCondition, default interface of TriggeringCondition
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TTriggeringConditionImpl : public TCppAutoObject<ITriggeringCondition>
{
  typedef _COM_CLASS inherited;

private:
  TTriggeringCondition FCondition;
public:
  __fastcall TTriggeringConditionImpl();
  __fastcall TTriggeringConditionImpl(const System::_di_IInterface Controller);
  __fastcall TTriggeringConditionImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);

  // ITriggeringCondition
protected:
  STDMETHOD(BeginEditing());
  STDMETHOD(EndEditing(VARIANT_BOOL* Value));
  STDMETHOD(MatchPriceComp(LogicalComparisonOperatorEnum op, BSTR price, unsigned* id));
  STDMETHOD(SellSideMatchPriceComp(LogicalComparisonOperatorEnum op, BSTR price, unsigned* id));
  STDMETHOD(BuySideMatchPriceComp(LogicalComparisonOperatorEnum op, BSTR price, unsigned* id));
  STDMETHOD(BidPriceComp(LogicalComparisonOperatorEnum op, BSTR price, unsigned* id));
  STDMETHOD(LastMatchPriceComp(LogicalComparisonOperatorEnum op, BSTR price, unsigned* id));
  STDMETHOD(AskPriceComp(LogicalComparisonOperatorEnum op, BSTR price, unsigned* id));
  STDMETHOD(VolumeComp(LogicalComparisonOperatorEnum op, unsigned volume, unsigned* id));
  STDMETHOD(PeriodAccumVolumeComp(LogicalComparisonOperatorEnum op, unsigned period_in_tenth_sec,
          unsigned volume, unsigned* id));
  STDMETHOD(BidVolumeComp(LogicalComparisonOperatorEnum op, unsigned volume, unsigned* id));
  STDMETHOD(AskVolumeComp(LogicalComparisonOperatorEnum op, unsigned volume, unsigned* id));
  STDMETHOD(AccumVolumeComp(LogicalComparisonOperatorEnum op, unsigned volume, unsigned* id));
  STDMETHOD(AnyTop5BidVolumeComp(LogicalComparisonOperatorEnum op, unsigned volume,
          unsigned* id));
  STDMETHOD(AnyTop5AskVolumeComp(LogicalComparisonOperatorEnum op, unsigned volume,
          unsigned* id));
  STDMETHOD(Conjunction(LPSAFEARRAY signal_list, unsigned count, VARIANT_BOOL inverter_on,
          unsigned* id));
  STDMETHOD(GetLastErrMsg(BSTR* lastErrMsg));
  STDMETHOD(ToExpression(BSTR* triggeringCondition));
  STDMETHOD(get_Instance(VARIANT* Value));
};
#endif //TriggeringConditionImplH
