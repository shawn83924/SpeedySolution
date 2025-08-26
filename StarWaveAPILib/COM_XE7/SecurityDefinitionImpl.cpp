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


STDMETHODIMP TSecurityDefinitionImpl::get_EndDate(BSTR* Value)
{
  try
  {
	WideString Date( FEndDate.c_str() );
	*Value = Date.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};


STDMETHODIMP TSecurityDefinitionImpl::get_Exchange(BSTR* Value)
{
  try
  {
	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};


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


STDMETHODIMP TSecurityDefinitionImpl::get_MaturityDate(BSTR* Value)
{
  try
  {
	WideString Date( FMaturityDate.c_str() );
	*Value = Date.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};


STDMETHODIMP TSecurityDefinitionImpl::get_MaturityDate2(BSTR* Value)
{
  try
  {
	WideString Date( FMaturityDate2.c_str() );
	*Value = Date.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};


STDMETHODIMP TSecurityDefinitionImpl::get_ProductID(BSTR* Value)
{
  try
  {
	WideString ProdStr( FProductID.c_str() );
	*Value = ProdStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};


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


STDMETHODIMP TSecurityDefinitionImpl::get_Symbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};

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

 void TSecurityDefinitionImpl::FromBasicInfo( BasicInformation* Info )
 {
	FExchange = Info->GetExchange();
	FSymbol = Info->GetSymbol();
	FProductID = Info->GetProductID();
	FEndDate = Info->GetEndDate();
	FMaturityDate = Info->GetMaturityDate();
	FMaturityDate2 = Info->GetMaturityDate2();
	FBullPx = Info->GetBullPrice();
	FRefPx = Info->GetRefPrice();
	FBearPx = Info->GetBearPrice();
	FStrikePx = Info->GetStrikePrice();
	FMarket =  Info->GetMarket();
	FCallPut = Info->GetCallPut();
	FContractMultiplier = Info->GetContractMultiplier();
	FIsMultileg = Info->IsMultileg();
	FStrikePx = Info->GetStrikePrice();
	FDigit = Info->GetDigit();
	FTick = Info->GetTick();
	FCurrency = Info->GetCurrency();
	FTradeSymbol = Info->GetTradeSymbol();
	FUnderlying = Info->GetUnderlying();
	FIsWarrant = Info->IsWarrant();
	FTradeUnit = Info->GetTradeUnit();
	FExecuteQty = Info->GetExecuteQty();
	FCancelQty = Info->GetCancelQty();
	FShares = Info->GetShares();
	FExecuteRate = Info->GetExecuteRate();
	FChineseName = Info->GetChineseName();
	FTradeFlag = Info->GetTradeFlag();
	FLastTradeDate = Info->GetLastTradeDate();
	FDecimalLocator = Info->GetDecimalLocator();
	FTimeDiff = Info->GetTimeDiff();
	FExpiryDays = Info->GetExpiryDays();
	FMaxHistoryVolDays = Info->GetMaxHistoryVolDays();
	for( int i=0; i<FMaxHistoryVolDays; i++ )
		FPreNChangeRange[i] = Info->GetPreNChangeRange(i);

	FBullPx2 = Info->GetBullPrice2();
	FBearPx2 = Info->GetBearPrice2();
	FBullPx3 = Info->GetBullPrice3();
	FBearPx3 = Info->GetBearPrice3();
 }





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



STDMETHODIMP TSecurityDefinitionImpl::get_Currency(BSTR* Value)
{
  try
  {
	WideString CurStr( FCurrency.c_str() );
	*Value = CurStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};


STDMETHODIMP TSecurityDefinitionImpl::get_TradeSymbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FTradeSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};



STDMETHODIMP TSecurityDefinitionImpl::get_Underlying(BSTR* Value)
{
  try
  {
	WideString UnderlyingStr( FUnderlying.c_str() );
	*Value = UnderlyingStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};



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



STDMETHODIMP TSecurityDefinitionImpl::get_ChineseName(BSTR* Value)
{
  try
  {
	WideString ChineseName = UTF8ToWideString( FChineseName.c_str() );
	*Value = ChineseName.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}



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



STDMETHODIMP TSecurityDefinitionImpl::get_LastTradeDate(BSTR* Value)
{
  try
  {
	WideString LastTradeDate = UTF8ToWideString( FLastTradeDate.c_str() );
	*Value = LastTradeDate.Copy();
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
}



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


STDMETHODIMP TSecurityDefinitionImpl::get_HistoryVol(long Days, double* Value)
{
  try
  {
	if( Days > FMaxHistoryVolDays )
		Days = FMaxHistoryVolDays;
	*Value = StdDev( FPreNChangeRange, Days ) * sqrt( YEAR_TRADING_DAYS );
  }
  catch(Exception &e)
  {
    return Error(e.Message.c_str(), IID_ISecurityDefinition);
  }
  return S_OK;
};



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



