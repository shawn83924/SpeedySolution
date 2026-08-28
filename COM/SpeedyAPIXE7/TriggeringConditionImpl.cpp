// ---------------------------------------------------------------------------
// TRIGGERINGCONDITIONIMPL : Implementation of TTriggeringConditionImpl (CoClass: TriggeringCondition, Interface: ITriggeringCondition)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "TriggeringConditionImpl.h"
// ---------------------------------------------------------------------------
// TTriggeringConditionImpl
// ---------------------------------------------------------------------------
__fastcall TTriggeringConditionImpl::TTriggeringConditionImpl()
{
}
// ---------------------------------------------------------------------------
// TTriggeringConditionImpl
// ---------------------------------------------------------------------------
__fastcall TTriggeringConditionImpl::TTriggeringConditionImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}
// ---------------------------------------------------------------------------
// TTriggeringConditionImpl
// ---------------------------------------------------------------------------
__fastcall TTriggeringConditionImpl::TTriggeringConditionImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}
// ---------------------------------------------------------------------------
// TTriggeringConditionImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TTriggeringConditionImpl>(Comserv::GetComServer(),
                           __classid(TTriggeringConditionImpl),
                           CLSID_TriggeringCondition,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::BeginEditing()
{
	FCondition.BeginEditing();
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::EndEditing(VARIANT_BOOL* Value)

{
	*Value = FCondition.EndEditing();
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::MatchPriceComp(LogicalComparisonOperatorEnum op,
          BSTR price, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	AnsiString Price(price);
	*id = FCondition.MatchPriceComp(Operator, Price.c_str());
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::SellSideMatchPriceComp(LogicalComparisonOperatorEnum op,
          BSTR price, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	AnsiString Price(price);
	*id = FCondition.SellSideMatchPriceComp(Operator, Price.c_str());
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::BuySideMatchPriceComp(LogicalComparisonOperatorEnum op,
          BSTR price, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	AnsiString Price(price);
	*id = FCondition.BuySideMatchPriceComp(Operator, Price.c_str());
	return S_OK;

}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::BidPriceComp(LogicalComparisonOperatorEnum op,
          BSTR price, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	AnsiString Price(price);
	*id = FCondition.BidPriceComp(Operator, Price.c_str());
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::LastMatchPriceComp(LogicalComparisonOperatorEnum op,
          BSTR price, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	AnsiString Price(price);
	*id = FCondition.LastMatchPriceComp(Operator, Price.c_str());
	return S_OK;

}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::AskPriceComp(LogicalComparisonOperatorEnum op,
          BSTR price, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	AnsiString Price(price);
	*id = FCondition.AskPriceComp(Operator, Price.c_str());
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::VolumeComp(LogicalComparisonOperatorEnum op,
          unsigned volume, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	*id = FCondition.VolumeComp(Operator, volume);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::PeriodAccumVolumeComp(LogicalComparisonOperatorEnum op,
          unsigned period_in_tenth_sec, unsigned volume, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	*id = FCondition.PeriodAccumVolumeComp(Operator, period_in_tenth_sec, volume);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::BidVolumeComp(LogicalComparisonOperatorEnum op,
          unsigned volume, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	*id = FCondition.BidVolumeComp(Operator, volume);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::AskVolumeComp(LogicalComparisonOperatorEnum op,
          unsigned volume, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	*id = FCondition.AskVolumeComp(Operator, volume);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::AccumVolumeComp(LogicalComparisonOperatorEnum op,
          unsigned volume, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	*id = FCondition.AccumVolumeComp(Operator, volume);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::AnyTop5BidVolumeComp(LogicalComparisonOperatorEnum op,
          unsigned volume, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	*id = FCondition.AnyTop5BidVolumeComp(Operator, volume);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::AnyTop5AskVolumeComp(LogicalComparisonOperatorEnum op,
          unsigned volume, unsigned* id)
{
	LogicalComparisonOperator Operator = static_cast<LogicalComparisonOperator>(static_cast<int>(op));
	*id = FCondition.AnyTop5AskVolumeComp(Operator, volume);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::Conjunction(LPSAFEARRAY signal_list, unsigned count,
          VARIANT_BOOL inverter_on, unsigned* id)
{
	SignalID* pData = NULL;
	HRESULT hr = SafeArrayAccessData(signal_list, (void**)&pData);
	if (FAILED(hr))
		return hr;

	*id = FCondition.Conjunction(pData, count, inverter_on);

	SafeArrayUnaccessData(signal_list);
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::GetLastErrMsg(BSTR* lastErrMsg)
{
	const char* errMsg = FCondition.GetLastErrMsg();
	UFC::AnsiString AnsiErrMsg(errMsg);
	WideString wideErrMsg(AnsiErrMsg.c_str());
	*lastErrMsg = wideErrMsg.Copy();
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::ToExpression(BSTR* triggeringCondition)
{
	const char* condition = FCondition.ToExpression();
	UFC::AnsiString AnsiCondition(condition);
	WideString wideCondition(AnsiCondition.c_str());
	*triggeringCondition = wideCondition.Copy();
    return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TTriggeringConditionImpl::get_Instance(VARIANT* Value)
{
	VARIANT ConditionInstance;
	ConditionInstance.vt = VT_BYREF;
	ConditionInstance.byref = &FCondition;
	*Value = ConditionInstance;

	return S_OK;
}
