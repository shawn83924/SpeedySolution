// ---------------------------------------------------------------------------
// SECURITYDEFINITIONIMPL : Implementation of TSecurityDefinitionImpl (CoClass: SecurityDefinition, Interface: ISecurityDefinition)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "SecurityDefinitionImpl.h"


// ---------------------------------------------------------------------------
// TSecurityDefinitionImpl
// ---------------------------------------------------------------------------
__fastcall TSecurityDefinitionImpl::TSecurityDefinitionImpl()
{
}


// ---------------------------------------------------------------------------
// TSecurityDefinitionImpl
// ---------------------------------------------------------------------------
__fastcall TSecurityDefinitionImpl::TSecurityDefinitionImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TSecurityDefinitionImpl
// ---------------------------------------------------------------------------
__fastcall TSecurityDefinitionImpl::TSecurityDefinitionImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TSecurityDefinitionImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TSecurityDefinitionImpl>(Comserv::GetComServer(),
                           __classid(TSecurityDefinitionImpl),
                           CLSID_SecurityDefinition,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
/////////////////////////////////////////////////////////////////////////////
// TSecurityDefinitionImpl
STDMETHODIMP TSecurityDefinitionImpl::get_BearPx(double* Value)
{
  try
  {
 	*Value = FBearPx;
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_BullPx(double* Value)
{
  try
  {
	*Value = FBullPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_CallPut(EnumCallPut* Value)
{
  try
  {
	*Value = FCallPut;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_ContractMultiplier(double* Value)
{
  try
  {
	*Value = FContractMultiplier;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_EndDate(BSTR* Value)
{
  try
  {
//	WideString Date( FEndDate.c_str() );
	*Value = FEndDate.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_Exchange(BSTR* Value)
{
  try
  {
//	WideString ExStr( FExchange.c_str() );
	*Value = FExchange.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_Market(EnumMarket* Value)
{
  try
  {
	*Value = FMarket;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_MaturityDate(BSTR* Value)
{
  try
  {
//	WideString Date( FMaturityDate.c_str() );
	*Value = FMaturityDate.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_MaturityDate2(BSTR* Value)
{
  try
  {
//	WideString Date( FMaturityDate2.c_str() );
	*Value = FMaturityDate2.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_ProductID(BSTR* Value)
{
  try
  {
//	WideString ProdStr( FProductID.c_str() );
	*Value = FProductID.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_RefPx(double* Value)
{
  try
  {
	*Value = FRefPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_Symbol(BSTR* Value)
{
  try
  {
//	WideString SymStr( FSymbol.c_str() );
	*Value = FSymbol.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_IsMultileg(VARIANT_BOOL* Value)
{
  try
  {
	if( FIsMultileg )
		*Value = VARIANT_TRUE;
	else
		*Value = VARIANT_FALSE;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_StrikePx(double* Value)
{
  try
  {
	*Value = FStrikePx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_Digit(long* Value)
{
  try
  {
	*Value = FDigit;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_Tick(double* Value)
{
  try
  {
	if( FExchange == "TAIFEX" || FExchange == "TFX" || FExchange == "TWSE" || FExchange == "OTC" )
		*Value = 0;
	else
		*Value = FTick;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_Currency(BSTR* Value)
{
  try
  {
//	WideString CurStr( FCurrency.c_str() );
	*Value = FCurrency.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_TradeSymbol(BSTR* Value)
{
  try
  {
//	WideString SymStr( FTradeSymbol.c_str() );
	*Value = FTradeSymbol.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_Underlying(BSTR* Value)
{
  try
  {
//	WideString UnderlyingStr( FUnderlying.c_str() );
	*Value = FUnderlying.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_CancelQty(long* Value)
{
  try
  {
	*Value = FCancelQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_ExecuteQty(long* Value)
{
  try
  {
	*Value = FExecuteQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_ExecuteRate(double* Value)
{
  try
  {
	*Value = FExecuteRate;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_IsWarrant(VARIANT_BOOL* Value)
{
  try
  {
	if( FIsWarrant )
		*Value = VARIANT_TRUE;
	else
		*Value = VARIANT_FALSE;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_Shares(long* Value)
{
  try
  {
	 *Value = FShares;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_TradeUnit(long* Value)
{
  try
  {
	*Value = FTradeUnit;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_ChineseName(BSTR* Value)
{
  try
  {
//	WideString ChineseName = UTF8ToWideString( FChineseName.c_str() );
	*Value = FChineseName.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_TradeFlag(VARIANT_BOOL* Value)
{
  try
  {
		if( FTradeFlag )
			*Value = VARIANT_TRUE;
		else
			*Value = VARIANT_FALSE;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_LastTradeDate(BSTR* Value)
{
  try
  {
//	WideString LastTradeDate = UTF8ToWideString( FLastTradeDate.c_str() );
	*Value = FLastTradeDate.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_DecimalLocator(long* Value)
{
  try
  {
	*Value = FDecimalLocator;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_BearPx2(double* Value)
{
  try
  {
	*Value = FBearPx2;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_BearPx3(double* Value)
{
  try
  {
	*Value = FBearPx3;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_BullPx2(double* Value)
{
  try
  {
	*Value = FBullPx2;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_BullPx3(double* Value)
{
  try
  {
	*Value = FBullPx3;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_TimeDiff(long* Value)
{
  try
  {
	*Value = FTimeDiff;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_ExpiryDays(long* Value)
{
  try
  {
	*Value = FExpiryDays;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_HistoryVol(long Days, double* Value)
{
  try
  {
	if( Days > FMaxHistoryVolDays )
		Days = FMaxHistoryVolDays;
	*Value = StdDev( FPreNChangeRange, Days ) * sqrt( (float)YEAR_TRADING_DAYS );
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_MaxHistoryVolDays(long* Value)
{
  try
  {
	*Value = FMaxHistoryVolDays;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_PrevClosePx(long Day, double* Value)
{
  try
  {
	if( Day > FMaxHistoryVolDays - 1 )
		Day = FMaxHistoryVolDays - 1;
	if( Day < 0 )
		Day = 0;
	*Value = FPreNChangeRange[ Day ];
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_IsMostActive(long* Value)
{
	*Value = FIsMostActive;
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_NearMonth(long* Value)
{
	*Value = FNearMonth;
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_PreSettlementPx(double* Value)
{
	*Value = FPreSettlementPrice;
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::get_SettlementPx(double* Value)
{
	*Value = FSettlementPrice;
	return S_OK;
}
// ---------------------------------------------------------------------------
STDMETHODIMP TSecurityDefinitionImpl::Copy(ISecurityDefinition* Ref)
{
	///< String Type
	BSTR SValue;

	Ref->get_Exchange( &SValue );
	FExchange      = SValue;
	Ref->get_Symbol( &SValue );
	FSymbol        = SValue;
	Ref->get_ProductID( &SValue );
	FProductID     = SValue;
	Ref->get_EndDate( &SValue );
	FEndDate       = SValue;
	Ref->get_MaturityDate( &SValue );
	FMaturityDate  = SValue;
	Ref->get_MaturityDate2( &SValue );
	FMaturityDate2 = SValue;
	Ref->get_Currency( &SValue );
	FCurrency      = SValue;
	Ref->get_TradeSymbol( &SValue );
	FTradeSymbol   = SValue;
	Ref->get_Underlying( &SValue );
	FUnderlying    = SValue;
	Ref->get_LastTradeDate( &SValue );
	FLastTradeDate = SValue;
	Ref->get_ChineseName( &SValue );
	FChineseName   = SValue;

	///< Enum types
	Ref->get_Market( &FMarket );
	Ref->get_CallPut( &FCallPut );
	///< Boolean type
	VARIANT_BOOL BValue;

	Ref->get_IsMultileg( &BValue );
	FIsMultileg = (BValue == VARIANT_TRUE)? true:false;
	Ref->get_IsWarrant( &BValue );
	FIsWarrant  = (BValue == VARIANT_TRUE)? true:false;
	Ref->get_TradeFlag( &BValue );
	FTradeFlag  = (BValue == VARIANT_TRUE)? true:false;
	///< Integer type
	long Value;

	Ref->get_Digit( &Value );
	FDigit = Value;
	Ref->get_TradeUnit( &Value );
	FTradeUnit = Value;
	Ref->get_ExecuteQty( &Value );
	FExecuteQty = Value;
	Ref->get_CancelQty( &Value );
	FCancelQty = Value;
	Ref->get_Shares( &Value );
	FShares = Value;
	Ref->get_DecimalLocator( &Value );
	FDecimalLocator = Value;
	Ref->get_TimeDiff( &Value );
	FTimeDiff = Value;
	Ref->get_ExpiryDays( &Value );
	FExpiryDays = Value;
	Ref->get_MaxHistoryVolDays( &Value );
	FMaxHistoryVolDays = Value;
	///< double type
	double DValue;

	Ref->get_BullPx( &DValue );
	FBullPx = DValue;
	Ref->get_RefPx( &DValue );
	FRefPx = DValue;
	Ref->get_BearPx( &DValue );
	FBearPx = DValue;
	Ref->get_BullPx2( &DValue );
	FBullPx2 = DValue;
	Ref->get_BearPx2( &DValue );
	FBearPx2 = DValue;
	Ref->get_BullPx3( &DValue );
	FBullPx3 = DValue;
	Ref->get_BearPx3( &DValue );
	FBearPx3 = DValue;
	Ref->get_StrikePx( &DValue );
	FStrikePx = DValue;
	Ref->get_ContractMultiplier( &DValue );
	FContractMultiplier = DValue;
	Ref->get_Tick( &DValue );
	FTick = DValue;
	Ref->get_ExecuteRate( &DValue );
	FExecuteRate = DValue;
	for( int i=0; i<FMaxHistoryVolDays; i++ )
	{
		Ref->get_PrevClosePx( i, &DValue );
		FPreNChangeRange[i] = DValue;
	}
	Ref->get_PreSettlementPx( &DValue );
	FPreSettlementPrice = DValue;

	Ref->get_SettlementPx( &DValue );
	FSettlementPrice = DValue;

	Ref->get_IsMostActive( &Value );
	FIsMostActive = Value;
	Ref->get_NearMonth( &Value );
	FNearMonth = Value;
	return S_OK;
}
// ---------------------------------------------------------------------------
 void TSecurityDefinitionImpl::FromBasicInfo( BasicInformation* Info )
 {
	///< Enum type
	FMarket =  (EnumMarket)Info->GetMarket();
	FCallPut = (EnumCallPut)Info->GetCallPut();
	///< Boolean type
	FIsMultileg = Info->IsMultileg();
	FIsWarrant = Info->IsWarrant();
	FTradeFlag = Info->GetTradeFlag();
	///< String type
	FExchange = Info->GetExchange().c_str();
	FSymbol = Info->GetSymbol().c_str();
	FProductID = Info->GetProductID().c_str();
	FEndDate = Info->GetEndDate().c_str();
	FMaturityDate = Info->GetMaturityDate().c_str();
	FMaturityDate2 = Info->GetMaturityDate2().c_str();
	FCurrency = Info->GetCurrency().c_str();
	FTradeSymbol = Info->GetTradeSymbol().c_str();
	FUnderlying = Info->GetUnderlying().c_str();
	FChineseName = UTF8ToWideString( Info->GetChineseName().c_str() );
	FLastTradeDate = UTF8ToWideString( Info->GetLastTradeDate().c_str() );
	///< Integer type
	FDigit = Info->GetDigit();
	FDecimalLocator = Info->GetDecimalLocator();
	FExecuteQty = Info->GetExecuteQty();
	FCancelQty = Info->GetCancelQty();
	FShares = Info->GetShares();
	FTradeUnit = Info->GetTradeUnit();
	FTimeDiff = Info->GetTimeDiff();
	FExpiryDays = Info->GetExpiryDays();
	FMaxHistoryVolDays = Info->GetMaxHistoryVolDays();
	///< double type
	FBullPx = Info->GetBullPrice();
	FRefPx = Info->GetRefPrice();
	FBearPx = Info->GetBearPrice();
	FBullPx2 = Info->GetBullPrice2();
	FBearPx2 = Info->GetBearPrice2();
	FBullPx3 = Info->GetBullPrice3();
	FBearPx3 = Info->GetBearPrice3();
	FStrikePx = Info->GetStrikePrice();
	FContractMultiplier = Info->GetContractMultiplier();
	FTick = Info->GetTick();
	FExecuteRate = Info->GetExecuteRate();
	for( int i=0; i<FMaxHistoryVolDays; i++ )
		FPreNChangeRange[i] = Info->GetPreNChangeRange(i);
	FSettlementPrice = Info->GetSettlementPrice();
	FPreSettlementPrice = Info->GetPreSettlementPrice();
	FIsMostActive = Info->GetIsMostActiveCode();
	FNearMonth = Info->GetNearMonthCode();
 }
// ---------------------------------------------------------------------------



