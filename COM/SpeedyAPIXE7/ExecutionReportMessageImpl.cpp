// ---------------------------------------------------------------------------
// EXECUTIONREPORTMESSAGEIMPL : Implementation of TExecutionReportMessageImpl (CoClass: ExecutionReportMessage, Interface: IExecutionReportMessage)
// ---------------------------------------------------------------------------
#include <vcl.h>
#include "WideStrUtils.hpp"
#pragma hdrstop
#include "ExecutionReportMessageImpl.h"
// ---------------------------------------------------------------------------
// TExecutionReportMessageImpl
// ---------------------------------------------------------------------------
__fastcall TExecutionReportMessageImpl::TExecutionReportMessageImpl()
{
}
// ---------------------------------------------------------------------------
__fastcall TExecutionReportMessageImpl::TExecutionReportMessageImpl( TExecutionReportMessage* Msg )
:FMessage( *Msg )
{
}
// ---------------------------------------------------------------------------
// TExecutionReportMessageImpl
// ---------------------------------------------------------------------------
__fastcall TExecutionReportMessageImpl::TExecutionReportMessageImpl(const System::_di_IInterface Controller)
							  : inherited(Controller)
{
}
// ---------------------------------------------------------------------------
// TExecutionReportMessageImpl
// ---------------------------------------------------------------------------
__fastcall TExecutionReportMessageImpl::TExecutionReportMessageImpl(Comobj::TComObjectFactory* Factory,
									  const System::_di_IInterface Controller)
							  : inherited(Factory, Controller)
{
}
// ---------------------------------------------------------------------------
// TExecutionReportMessageImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TExecutionReportMessageImpl>(Comserv::GetComServer(),
						   __classid(TExecutionReportMessageImpl),
						   CLSID_ExecutionReportMessage,
						   Comobj::ciMultiInstance,
#if defined(USING_MTA)
						   Comobj::tmFree );
#else
						   Comobj::tmApartment );
#endif
}
#pragma startup createFactory 32
//------------------------------------------------------------------------------
void TExecutionReportMessageImpl::DetachMessage( void )
{
   FMessage.Detach();
}
/////////////////////////////////////////////////////////////////////////////
// IBaseMessage
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_Instance(VARIANT* Value)
{
	VARIANT MessageInstance;
	MessageInstance.vt = VT_BYREF;
	MessageInstance.byref = &FMessage;
	*Value = MessageInstance;

	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_Account(BSTR* Value)
{
	WideString Account( FMessage.GetAccount() );
	*Value = Account.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_AE(BSTR* Value)
{
	WideString AE( FMessage.GetAE() );
	*Value = AE.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_ClOrdID(BSTR* Value)
{
	WideString ClOrdID( FMessage.GetClOrdID() );
	*Value = ClOrdID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_Market(MarketEnum* Value)
{
	*Value = (MarketEnum)( FMessage.GetMarket() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_MessageType(MessageTypeEnum* Value)
{
	*Value = (MessageTypeEnum)( FMessage.GetMessageType() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_NID(__int64* Value)
{
	*Value = FMessage.GetNID();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_Symbol(BSTR* Value)
{
    WideString Symbol( FMessage.GetSymbol() );
    *Value = Symbol.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_Account(BSTR Value)
{
    AnsiString Account( Value );
	FMessage.SetAccount( Account.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_AE(BSTR Value)
{
    AnsiString AE( Value );
    FMessage.SetAE( AE.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_ClOrdID(BSTR Value)
{
    AnsiString ClOrdID( Value );
    FMessage.SetClOrdID( ClOrdID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_Market(MarketEnum Value)
{
    FMessage.SetMarket( (nsOrderMessageDefine::MarketEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_MessageType(MessageTypeEnum Value)
{
    FMessage.SetMessageType( (nsOrderMessageDefine::MessageTypeEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_NID(__int64 Value)
{
    FMessage.SetNID( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_Symbol(BSTR Value)
{
    AnsiString Symbol( Value );
    FMessage.SetSymbol( Symbol.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_Value(BSTR Name, BSTR* Value)
{
	AnsiString ValName( Name );
	UFC::AnsiString ValValue( "" );
	FMessage.GetValue( ValName.c_str(), ValValue );
	WideString WideValue( ValValue.c_str() );

	*Value = WideValue.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_Value(BSTR Name, BSTR Value)
{
	AnsiString ValName( Name );
	AnsiString ValValue( Value );

	FMessage.SetValue( ValName.c_str(), ValValue.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_AccountFlag(BSTR* Value)
{
	WideString AccountFlag( FMessage.GetAccountFlag() );
	*Value = AccountFlag.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_AccountFlag(BSTR Value)
{
	AnsiString AccountFlag( Value );
	FMessage.SetAccountFlag( AccountFlag.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_GatewayData(BSTR* Value)
{
	WideString GDD( FMessage.GetGatewayData() );
	*Value = GDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_GatewayData(BSTR Value)
{
	AnsiString GDD( Value );

	FMessage.SetGatewayData( GDD.c_str() );
	return S_OK;
};
//---------------------------------------------------------------------------
const UFC::AnsiString PROXY_PREFIX( "<Proxy:" );
const int             PROXY_PREFIX_LEN = PROXY_PREFIX.Length(); ///< 7
const int             PROXY_NID_LEN      = 10;
const int             PROXY_MINUS_AE_LEN = PROXY_PREFIX_LEN + PROXY_NID_LEN;
//---------------------------------------------------------------------------
bool TExecutionReportMessageImpl::RemoveProxyUserData( const UFC::AnsiString& UserData,  ///< [In]
													UFC::AnsiString& Data ) ///< Out
{
	if( UserData.Length() >= 21 ) ///< Need 21 bytes "<Proxy:"(7) + AE(>3) + NID(10) + ">"(1)
	{
		int RightQuote = UserData.AnsiPos( ">" );

		if( UserData.AnsiNCompare( PROXY_PREFIX, PROXY_PREFIX_LEN ) == 0 && RightQuote > PROXY_PREFIX_LEN )
		{
			if( UserData.Length() > RightQuote + 1 )
				Data = UFC::AnsiString( UserData.c_str() + RightQuote + 1 );
			else
				Data = "";
			return true;
		}
	}
	Data = UserData;
	return false;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_Data(BSTR* Value)
{
	UFC::AnsiString Data;
	RemoveProxyUserData( FMessage.GetUserData(), Data ); ///< Out
	WideString UDD( Data.c_str() );
	*Value = UDD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_Data(BSTR Value)
{
	AnsiString UDD( Value );

	FMessage.SetUserData( UDD.c_str() );
	return S_OK;
};
/////////////////////////////////////////////////////////////////////////////
// TExecutionReportMessageImpl
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_CumQty(long* Value)
{
	*Value = FMessage.GetCumQty();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_CxlRejResponseTo(CxlRejResponseToEnum* Value)


{
	*Value = (CxlRejResponseToEnum)( FMessage.GetCxlRejResponseTo() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_EventType(EventTypeEnum* Value)
{
	*Value = (EventTypeEnum)( FMessage.GetEventType() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_ExecID(BSTR* Value)
{
	WideString ExecID( FMessage.GetExecID() );
	*Value = ExecID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_ExecRestatementReason(ExecRestatementReasonEnum* Value)


{
	*Value = (ExecRestatementReasonEnum)( FMessage.GetExecRestatementReason() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_ExecTransType(ExecTransTypeEnum* Value)


{
	*Value = (ExecTransTypeEnum)( FMessage.GetExecTransType() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_ExecType(ExecTypeEnum* Value)
{
	*Value = (ExecTypeEnum)( FMessage.GetExecType() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LastPx(double* Value)
{
	*Value = FMessage.GetLastPx();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LeavesQty(long* Value)
{
	*Value = FMessage.GetLeavesQty();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_MaturityMonthYear(BSTR* Value)
{
	WideString MaturityMonthYear( FMessage.GetMaturityMonthYear() );
	*Value = MaturityMonthYear.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_OrderID(BSTR* Value)
{
	WideString OrderID( FMessage.GetOrderID() );
	*Value = OrderID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_OrderQty(long* Value)
{
	*Value = FMessage.GetOrderQty();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_OrderStatus(OrderStatusEnum* Value)


{
	*Value = (OrderStatusEnum)( FMessage.GetOrderStatus() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_OrderType(OrderTypeEnum* Value)
{
	*Value = (OrderTypeEnum)( FMessage.GetOrderType() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_OrdRejReason(OrdRejReasonEnum* Value)


{
	*Value = (OrdRejReasonEnum)( FMessage.GetOrdRejReason() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_OrigClOrdID(BSTR* Value)
{
	WideString OrigClOrdID( FMessage.GetOrigClOrdID() );
	*Value = OrigClOrdID.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_Price(double* Value)
{
	*Value = FMessage.GetPrice();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_Side(SideEnum* Value)
{
	*Value = (SideEnum)( FMessage.GetSide() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_StopPx(double* Value)
{
	*Value = FMessage.GetStopPx();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_StrikePrice(double* Value)
{
	*Value = FMessage.GetStrikePrice();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_Text(BSTR* Value)
{
	if( IsUTF8String( FMessage.GetText() ) == true )
	{
		UTF8String UTF8Str( FMessage.GetText() );
		WideString Text( UTF8Str );
		*Value = Text.Copy();
	}
	else
	{
		String UnicodeStr( FMessage.GetText() );
		WideString Text( UnicodeStr );
		*Value = Text.Copy();
    }
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_TimeInForce(TimeInForceEnum* Value)


{
	*Value = (TimeInForceEnum)( FMessage.GetTimeInForce() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_CumQty(long Value)
{
	FMessage.SetCumQty( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_CxlRejResponseTo(CxlRejResponseToEnum Value)


{
	FMessage.SetCxlRejResponseTo( (nsOrderMessageDefine::CxlRejResponseToEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_EventType(EventTypeEnum Value)
{
	FMessage.SetEventType( (nsOrderMessageDefine::EventTypeEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_ExecID(BSTR Value)
{
	AnsiString ExecID( Value );
	FMessage.SetExecID( ExecID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_ExecRestatementReason(ExecRestatementReasonEnum Value)


{
	FMessage.SetExecRestatementReason( (nsOrderMessageDefine::ExecRestatementReasonEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_ExecTransType(ExecTransTypeEnum Value)


{
	FMessage.SetExecTransType( (nsOrderMessageDefine::ExecTransTypeEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_ExecType(ExecTypeEnum Value)
{
	FMessage.SetExecType( (nsOrderMessageDefine::ExecTypeEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_LastPx(double Value)
{
	FMessage.SetLastPx( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_LeavesQty(long Value)
{
	FMessage.SetLeavesQty( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_MaturityMonthYear(BSTR Value)
{
	AnsiString MaturityMonthYear( Value );
	FMessage.SetMaturityMonthYear( MaturityMonthYear.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_OrderID(BSTR Value)
{
	AnsiString OrderID( Value );
	FMessage.SetOrderID( OrderID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_OrderQty(long Value)
{
	FMessage.SetOrderQty( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_OrderStatus(OrderStatusEnum Value)
{
	FMessage.SetOrderStatus( (nsOrderMessageDefine::OrderStatusEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_OrderType(OrderTypeEnum Value)
{
	FMessage.SetOrderType( (nsOrderMessageDefine::OrderTypeEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_OrdRejReason(OrdRejReasonEnum Value)


{
	FMessage.SetOrdRejReason( (nsOrderMessageDefine::OrdRejReasonEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_OrigClOrdID(BSTR Value)
{
	AnsiString OrigClOrdID( Value );
	FMessage.SetOrigClOrdID( OrigClOrdID.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_Price(double Value)
{
	FMessage.SetPrice( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_Side(SideEnum Value)
{
	FMessage.SetSide( (nsOrderMessageDefine::SideEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_StopPx(double Value)
{
	FMessage.SetStopPx( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_StrikePrice(double Value)
{
	FMessage.SetStrikePrice( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_Text(BSTR Value)
{
	AnsiString Text( Value );
	FMessage.SetText( Text.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_TimeInForce(TimeInForceEnum Value)
{
	FMessage.SetTimeInForce( (nsOrderMessageDefine::TimeInForceEnum)Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_ConnectionID(long* Value)
{
	*Value = FMessage.GetConnectionID();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_Src(BSTR* Value)
{
	WideString TFXSrc( FMessage.GetSrc() );
	*Value = TFXSrc.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LegPrice1(double* Value)
{
	*Value = FMessage.GetLegPrice1();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LegPrice2(double* Value)
{
	*Value = FMessage.GetLegPrice2();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LegQty1(long* Value)
{
	*Value = FMessage.GetLegQty1();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LegQty2(long* Value)
{
	*Value = FMessage.GetLegQty2();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LegSide1(SideEnum* Value)
{
	*Value = (SideEnum)( FMessage.GetLegSide1() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LegSide2(SideEnum* Value)
{
	*Value = (SideEnum)( FMessage.GetLegSide2() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LegSymbol1(BSTR* Value)
{
	WideString LegSymbol1( FMessage.GetLegSymbol1() );
	*Value = LegSymbol1.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LegSymbol2(BSTR* Value)
{
	WideString LegSymbol2( FMessage.GetLegSymbol2() );
	*Value = LegSymbol2.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_TransactTime(BSTR* Value)
{
	WideString TransTime( FMessage.GetTransactTime() );

	*Value = TransTime.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LINBRN(BSTR* Value)
{
	WideString LineBRN( FMessage.GetLINBRN() );
	*Value = LineBRN.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_TradingSessionID(TradingSessionIDEnum* Value)


{
	*Value = (TradingSessionIDEnum)( FMessage.GetTradingSessionID() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_TradingSessionID(TradingSessionIDEnum Value)


{
  FMessage.SetTradingSessionID( (nsOrderMessageDefine::TradingSessionIDEnum)Value );
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_BrokerID(BSTR* Value)
{
	WideString Data( FMessage.GetBrokerID() );
	*Value = Data.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_PositionEffect(PositionEffectEnum* Value)


{
	*Value = (PositionEffectEnum)( FMessage.GetPositionEffect() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_PositionEffect(PositionEffectEnum Value)


{
	FMessage.SetPositionEffect( (nsOrderMessageDefine::PositionEffectEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_BeforeQty(long* Value)
{
	*Value = FMessage.GetBeforeQty();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_BeforeQty(long Value)
{
	FMessage.SetBeforeQty( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_LastQty(long* Value)
{
	*Value = FMessage.GetLastQty();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_LastQty(long Value)
{
	FMessage.SetLastQty( Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_PriceBase(PriceBaseEnum* Value)
{
	*Value = (PriceBaseEnum) FMessage.GetPxBase();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_PriceBase(PriceBaseEnum Value)
{
	FMessage.SetPxBase( (nsOrderMessageDefine::PriceBaseEnum) Value );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_ExchangeCode(BSTR* Value)
{
	WideString ExchangeCode;

	switch( FMessage.GetMarket() )
	{
		case nsOrderMessageDefine::mTWFutures:
		case nsOrderMessageDefine::mTWOptions: ExchangeCode = "TAIFEX";break;
		case nsOrderMessageDefine::mOTC:       ExchangeCode = "OTC";break;
		case nsOrderMessageDefine::mTSE:       ExchangeCode = "TWSE";break;
		case nsOrderMessageDefine::mES:        ExchangeCode = "ES";break;
		default: ExchangeCode = FMessage.GetExchangeCode();break;
	}
	*Value = ExchangeCode.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_ExchangeCode(BSTR Value)
{
	AnsiString ExchangeCode( Value );
	FMessage.SetExchangeCode( ExchangeCode.c_str() );
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_ReportSeq(long* Value)
{
	*Value = FMessage.GetReportSequence();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_TMPExecType(TMPExecTypeEnum* Value)


{
   switch( FMessage.GetTMPExecType() )
   {
		case nsOrderMessageDefine::tetNew:          *Value = TMPExecTypeEnum::tetNew;break;
		case nsOrderMessageDefine::tetCanceled:     *Value = TMPExecTypeEnum::tetCanceled;break;
		case nsOrderMessageDefine::tetReplaced:     *Value = TMPExecTypeEnum::tetReplaced;break;
		case nsOrderMessageDefine::tetFilled:       *Value = TMPExecTypeEnum::tetFilled;break;
		case nsOrderMessageDefine::tetPxReplaced:
		case nsOrderMessageDefine::tetPxReplaced2:  *Value = TMPExecTypeEnum::tetPxReplaced;break;
		case nsOrderMessageDefine::tetOrderStatus:  *Value = TMPExecTypeEnum::tetOrderStatus;break;
		case nsOrderMessageDefine::tetNewAndFilled: *Value = TMPExecTypeEnum::tetNewAndFilled;break;
   }
   return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_TMPUniqueID(long* Value)
{
	*Value = FMessage.GetTMPUniqueID();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_TMPSessionID(long* Value)
{
	UFC::AnsiString SID( FMessage.GetPVC() );

	*Value = SID.ToInt();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_TSEOrderType(BSTR* Value)
{
	WideString TSEOrderType( FMessage.GetTSEOrderType() );
	*Value = TSEOrderType.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_TSEExchangeCode(BSTR* Value)
{
	WideString TSEExchangeCode( FMessage.GetTSEExchangeCode() );
	*Value = TSEExchangeCode.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_TSEObjectBroker(BSTR* Value)
{
	WideString TSEObjectBroker( FMessage.GetTSEObjectBroker() );
	*Value = TSEObjectBroker.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_TSETradeKind(BSTR* Value)
{
	WideString TSETradeKind( FMessage.GetTSETradeKind() );
	*Value = TSETradeKind.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_EUREX2TAIFEX(BSTR* Value)
{
	WideString EUCD( FMessage.GetEUCD() );
	*Value = EUCD.Copy();
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_OrigTransactTime(BSTR* Value)
{
	WideString OrigTransactTime( FMessage.GetOrgTransactTime() );
	*Value = OrigTransactTime.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_us(long* Value)
{
	*Value = FMessage.GetUseus();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_ExecRefID(BSTR* Value)
{
	WideString ExecRefID( FMessage.GetExecRefID() );
	*Value = ExecRefID.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_ExecRefID(BSTR Value)
{
	AnsiString ExecRefID( Value );
	FMessage.SetExecRefID( ExecRefID.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_OrigExecID(BSTR* Value)
{
	WideString OrigExecID( FMessage.GetOrigExecID() );
	*Value = OrigExecID.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_OrigExecID(BSTR Value)
{
	AnsiString OrigExecID( Value );
	FMessage.SetOrigExecID( OrigExecID.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_OrderChannel(BSTR* Value)
{
	WideString OrderChannel( FMessage.GetOrderChannel() );
	*Value = OrderChannel.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_OrderChannel(BSTR Value)
{
	AnsiString OrderChannel( Value );
	FMessage.SetOrderChannel( OrderChannel.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_StatusCode(BSTR* Value)
{
	WideString ErrorCode( FMessage.GetStatusCode() );
	*Value = ErrorCode.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_ExpireDate(BSTR* Value)
{
	WideString ExpireDate( FMessage.GetExpireDate() );
	*Value = ExpireDate.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::set_ExpireDate(BSTR Value)
{
	AnsiString ExpireDate( Value );
	FMessage.SetExpireDate( ExpireDate.c_str() );
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_PxDecimalNum(long* Value)
{
	*Value = FMessage.GetPxDigit();
	return S_OK;
}
//------------------------------------------------------------------------------

STDMETHODIMP TExecutionReportMessageImpl::get_TMPStatusCode(long* Value)
{
	*Value = FMessage.GetTMPStstusCode();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_MessageDateTime(BSTR* Value)
{
	WideString MsgDateTime( FMessage.GetMessageTime() );
	*Value = MsgDateTime.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_CancelByExchange(VARIANT_BOOL* Value)


{
	if( FMessage.GetReportSequence() > 10000000 )
		*Value = VARIANT_TRUE;
	else
		*Value = VARIANT_FALSE;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_IsReplacePx(VARIANT_BOOL* Value)
{
	if( FMessage.IsReplacePx() == true )
		*Value = VARIANT_TRUE;
	else
		*Value = VARIANT_FALSE;
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::get_PartID(long* Value)
{
	*Value = FMessage.GetTMPPartID();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::GetTouchOrderResponse(BSTR* Value)
{
	WideString response( FMessage.GetTouchOrderResponse() );
	*Value = response.Copy();
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TExecutionReportMessageImpl::GetTouchOrderRespType(TouchOrderResponseTypeEnum* Value)
{
	*Value = (TouchOrderResponseTypeEnum)( FMessage.GetTouchOrderRespType() );
	return S_OK;

}
