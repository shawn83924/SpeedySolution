// ---------------------------------------------------------------------------
// RECOVERIMPL : Implementation of TRecoverImpl (CoClass: Recover, Interface: IRecover)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "RecoverImpl.h"


// ---------------------------------------------------------------------------
// TRecoverImpl
// ---------------------------------------------------------------------------
__fastcall TRecoverImpl::TRecoverImpl()
{
}


// ---------------------------------------------------------------------------
// TRecoverImpl
// ---------------------------------------------------------------------------
__fastcall TRecoverImpl::TRecoverImpl(const System::_di_IInterface Controller)
                              : inherited(Controller)
{
}


// ---------------------------------------------------------------------------
// TRecoverImpl
// ---------------------------------------------------------------------------
__fastcall TRecoverImpl::TRecoverImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller)
{
}

// ---------------------------------------------------------------------------
// TRecoverImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TRecoverImpl>(Comserv::GetComServer(),
                           __classid(TRecoverImpl),
                           CLSID_Recover,
                           Comobj::ciMultiInstance,
						   Comobj::tmApartment);
}
#pragma startup createFactory 32
/////////////////////////////////////////////////////////////////////////////
// TRecoverImpl
STDMETHODIMP TRecoverImpl::get_BuyOrderCount(long* Value)
{
  try
  {
	*Value = FBuyOrderCount;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_BuyOrderQty(long* Value)
{
  try
  {
	*Value = FBuyOrderQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_BuyPx(long Depth, double* Value)
{
  try
  {
	if( Depth > 0 && Depth <= MAX_DEPTH )
	{
		*Value = FBuyPx[Depth - 1];
	}
	else
		*Value = 0;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_BuyQty(long Depth, long* Value)
{
  try
  {
	if( Depth > 0 && Depth <= MAX_DEPTH )
	{
		*Value = FBuyQty[Depth - 1];
	}
	else
		*Value = 0;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_ClosingPx(double* Value)
{
  try
  {
	*Value = FClosePx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_DayHighPx(double* Value)
{
  try
  {
	*Value = FDayHighPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_DayLowPx(double* Value)
{
  try
  {
	*Value = FDayLowPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_Exchange(BSTR* Value)
{
  try
  {
	WideString ExStr( FExchange.c_str() );
	*Value = ExStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_Market(EnumMarket* Value)
{
  try
  {
	*Value = FMarket;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_MatchPx(double* Value)
{
  try
  {
	*Value = FMatchPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_MatchQty(long* Value)
{
  try
  {
	*Value = FMatchQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_MatchTime(BSTR* Value)
{
  try
  {
	WideString TimeStr( FMatchTime.c_str() );
	*Value = TimeStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_MatchDate(BSTR* Value)
{
  try
  {
	WideString DateStr( FMatchDate.c_str() );
	*Value = DateStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_OpeningPx(double* Value)
{
  try
  {
	*Value = FOpenPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_OpenInterest(long* Value)
{
  try
  {
	*Value = FOpenInterest;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_SellOrderCount(long* Value)
{
  try
  {
	*Value = FSellOrderCount;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_SellOrderQty(long* Value)
{
  try
  {
	*Value = FSellOrderQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_SellPx(long Depth, double* Value)
{
  try
  {
	if( Depth > 0 && Depth <= MAX_DEPTH )
	{
		*Value = FSellPx[Depth - 1];
	}
	else
		*Value = 0;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_SellQty(long Depth, long* Value)
{
  try
  {
	if( Depth > 0 && Depth <= MAX_DEPTH )
	{
		*Value = FSellQty[Depth - 1];
	}
	else
		*Value = 0;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_SettlementPx(double* Value)
{
  try
  {
	*Value = FSettlementPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_Symbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_TotalVolume(long* Value)
{
  try
  {
	*Value = FTotalVolume;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_DerivedBuyPx(long* Value)
{
  try
  {
	*Value = FDerivedBuyPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_DerivedBuyQty(long* Value)
{
  try
  {
	*Value = FDerivedBuyQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_DerivedFlag(VARIANT_BOOL* Value)
{
  try
  {
	if(FDerivedFlag)
		*Value = VARIANT_TRUE;
	else
		*Value = VARIANT_FALSE;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_DerivedSellPx(long* Value)
{
  try
  {
	*Value = FDerivedSellPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_DerivedSellQty(long* Value)
{
  try
  {
	*Value = FDerivedSellQty;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
void TRecoverImpl::FromMarketDataMessage( MarketDataMessage* Msg )
{
	FExchange = Msg->GetExchange();
	FSymbol =  Msg->GetSymbol();
	FActualSymbol = Msg->GetActualSymbol();
	FMarket = (EnumMarket)Msg->GetMarket();
	for( int i = 0 ; i < MAX_DEPTH; i++ )
	{
		FBuyPx[i]   = Msg->GetBuyPx(i+1);
		FBuyQty[i]  = Msg->GetBuyQty(i+1);
		FSellPx[i]  = Msg->GetSellPx(i+1);
		FSellQty[i] = Msg->GetSellQty(i+1);
	}
	FMatchPx = Msg->GetTradePrice();
	FMatchQty = Msg->GetTradeQty();
	FMatchTime = Msg->GetTradeTime();
	FMatchDate = Msg->GetTradeDate();
	FOpenPx = Msg->GetOpeningPrice();
	FDayHighPx = Msg->GetDayHighPrice();
	FDayLowPx = Msg->GetDayLowPrice();
	FClosePx = Msg->GetClosingPrice();
	FTotalVolume = Msg->GetTradeVolume();
	FBuyOrderCount = Msg->GetBuyTotalCount();
	FBuyOrderQty = Msg->GetBuyTotalQty();
	FSellOrderCount = Msg->GetSellTotalCount();
	FSellOrderQty = Msg->GetSellTotalQty();
	FPreClosePx = Msg->GetPreClosePx();
	FProductID = Msg->GetProductID();
	FBullPx = Msg->GetBullPx();
	FBearPx = Msg->GetBearPx();
	FRefPx = Msg->GetRefPx();
	FTradingSession = (EnumTradingSession)Msg->GetTradingSession();

	///< Futures/Options fields
	FSettlementPx = Msg->GetSettlementPrice();
	FOpenInterest = Msg->GetOpenInterest();
	FStrikePx = Msg->GetStrikePx();
	FCallPut = 	(EnumCallPut)Msg->GetCallPut();
	FSettleMonth = Msg->GetSettleMonth();
	FEndDate = Msg->GetEndDate();

	///< TAIFEX Derived price
	FDerivedFlag = Msg->GetDerivedFlag();
	FDerivedBuyPx = Msg->GetDerivedBuyPrice();
	FDerivedBuyQty = Msg->GetDerivedBuyQty();
	FDerivedSellPx = Msg->GetDerivedSellPrice();
	FDerivedSellQty = Msg->GetDerivedSellQty();

	///< HKEx Last 10 trades.
	FTrdCount = Msg->GetLastNTrade();
	for( int i = 0;i< FTrdCount; i++ )
	{
		FTrdTime[i] = Msg->GetLastTime(i);
		FTrdPx[i] = Msg->GetLastPrice(i);
		FTrdQty[i] = Msg->GetLastQty(i);
		FTrdType[i] = Msg->GetLastTradeType( i );
	}
	FSuspensionIndicator = Msg->GetSuspensionIndicator();
	memcpy( FBuyNumOfOrders, Msg->BuyNumOfOrders, sizeof( UFCType::UInt32)*MAX_DEPTH );
	memcpy( FSellNumOfOrders, Msg->SellNumOfOrders, sizeof( UFCType::UInt32)*MAX_DEPTH );
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_PreClosePx(double* Value)
{
  try
  {
	*Value = FPreClosePx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_BearPx(double* BearPx)
{
  try
  {
	*BearPx = FBearPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_BullPx(double* BullPx)
{
  try
  {
	*BullPx = FBullPx;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_CallPut(EnumCallPut* CallorPut)
{
  try
  {
	*CallorPut = FCallPut;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_ProductID(BSTR* ProductID)
{
	WideString IDStr( FProductID.c_str() );
	*ProductID = IDStr.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_RefPx(double* RefPx)
{
	*RefPx = FRefPx;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_StrikePx(double* StrikePx)
{
	*StrikePx = FStrikePx;
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_SettleDate(BSTR* Value)
{
	WideString DateStr( FSettleMonth.c_str() );
	*Value = DateStr.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_EndDate(BSTR* Value)
{
	WideString DateStr( FEndDate.c_str() );
	*Value = DateStr.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_TradingSession(EnumTradingSession* Value)
{
	*Value = FTradingSession;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_TradeCount(long* Value)
{
	if( FTrdCount < 10 )
		*Value = FTrdCount;
	else
		*Value = 10;
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_TradeTime(long index, long* Value)
{
  if( index >= 10 )
	  *Value = 0;
  else
	  *Value = FTrdTime[index];
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_TradePrice(long index, double* Value)
{
  if( index >= 10 )
	  *Value = 0.0;
  else
	  *Value = FTrdPx[index];
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_TradeQty(long index, long* Value)
{
  if( index >= 10 )
	  *Value = 0;
  else
	  *Value = FTrdQty[index];
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_TradeType(long index, EnumHKExTrdType* Value)
{
  if( index >= 10 )
	  *Value = (EnumHKExTrdType)0;
  else
	  *Value = (EnumHKExTrdType)FTrdType[index];
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_ActualSymbol(BSTR* Value)
{
  try
  {
	WideString SymStr( FActualSymbol.c_str() );
	*Value = SymStr.Copy();
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_SuspensionIndicator(long* Value)
{
  *Value = FSuspensionIndicator;
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_BuyNumberOfOrders(long Depth, long* Value)
{
  try
  {
	if( Depth <= 0 || Depth > MAX_DEPTH ) ///< 1~10
		*Value = 0;
	else
		*Value = FBuyNumOfOrders[Depth - 1];
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TRecoverImpl::get_SellNumberOfOrders(long Depth, long* Value)
{
  try
  {
	if( Depth <= 0 || Depth > MAX_DEPTH ) ///< 1~10
		*Value = 0;
	else
		*Value = FSellNumOfOrders[Depth - 1];
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IRecover);
  }
  return S_OK;
}
//------------------------------------------------------------------------------


