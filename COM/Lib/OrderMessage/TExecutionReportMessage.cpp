#include "TExecutionReportMessage.h"
#include "TNetworkID.h"
//---------------------------------------------------------------------------
TExecutionReportMessage::TExecutionReportMessage( void )
:TBaseMessage( )
,FMaturityMonthYear( "" )
,FOrigClOrdID( "" )
,FExecID( "" )
,FExecRefID( "" )
,FOrigExecID( "" )
,FText( "" )
,FSrc( "" )
,FLINBRN( "0000000" )
,FOrgTransactTime( "" )
,FTransactTime( "" )
,FMsgTime( "" )
,FTSEOrderType( "0" )
,FTSEExchangeCode( "0" )
,FTSETradeKind( "2" )
,FTSEObjectBroker( "0000" )
,FExchangeCode( "TAIFEX" )
,FPVC( "00" )
,FStatusCode( "00" )
,FEUCD( "" )
,FSellSideAccount( "" )
,FExchangeOrderID( "" )
,FStockSeqNo( 0 )
,FOrderQty( 0 )
,FCumQty( 0 )
,FLastQty( 0 )
,FLeavesQty( 0 )
,FNoLegs( 0 )
,FLegQty1( 0 )
,FLegQty2( 0 )
,FBeforeQty( 0 )
,FAfterQty( 0 )
,FReportSequence( 0 )
,FTMPQty( 0 )
,FTMPStstusCode( 0 )
,FTMPBrokerID( 0 )
,FTMPUniqueID( 0 )
,FPxDigit( 0 )
,Fus( 0 )
,FPartID( 0 )
,FTMPPxSubTotal( 0.0 )
,FPrice( 0.0 )
,FAvgPx( 0.0 )
,FLastPx( 0.0 )
,FStopPx( 0.0 )
,FStrikePrice( 0.0 )
,FLegPrice1( 0.0 )
,FLegPrice2( 0.0 )
,FReplacePx( false )
,FOrigNID( 0 )
,FOrderType( nsOrderMessageDefine::otLimit )
,FSide( nsOrderMessageDefine::sNone )
,FTimeInForce( nsOrderMessageDefine::tifNone )
,FPositionEffect( nsOrderMessageDefine::peOpen )
,FEventType( nsOrderMessageDefine::evtNone )
,FExecTransType( nsOrderMessageDefine::ettNone )
,FOrderStatus( nsOrderMessageDefine::osNone )
,FExecRestatementReason( nsOrderMessageDefine::errNone )
,FOrdRejReason( nsOrderMessageDefine::orrNone )
,FCxlRejResponseTo( nsOrderMessageDefine::crrNone )
,FTradingSessionID( nsOrderMessageDefine::tsNormal )
,FLegSide1( nsOrderMessageDefine::sNone )
,FLegSide2( nsOrderMessageDefine::sNone )
,FExpireDate( "" )
,FFixCxlRejResponseTo( -1 )
,FFixCxlRejReason( -1 )
,FFixOrdRejReason( -1 )
{
    SetMessageType( nsOrderMessageDefine::mtExecutionReport );
}
//---------------------------------------------------------------------------
TExecutionReportMessage::TExecutionReportMessage( TExecutionReportMessage& Ref )
:TBaseMessage( Ref )
,FMaturityMonthYear( Ref.FMaturityMonthYear)
,FOrigClOrdID( Ref.FOrigClOrdID)
,FExecID( Ref.FExecID)
,FExecRefID( Ref.FExecRefID )
,FOrigExecID( Ref.FOrigExecID )
,FText( Ref.FText)
,FSrc( Ref.FSrc)
,FLINBRN( Ref.FLINBRN)
,FOrgTransactTime( Ref.FOrgTransactTime)
,FTransactTime( Ref.FTransactTime)
,FMsgTime( Ref.FMsgTime )
,FTMPExt( Ref.FTMPExt)
,FLegSymbol1( Ref.FLegSymbol1)
,FLegSymbol2( Ref.FLegSymbol2)
,FCMID( Ref.FCMID)
,FRecNo( Ref.FRecNo)
,FTSEOrderType( Ref.FTSEOrderType)
,FTSEExchangeCode( Ref.FTSEExchangeCode)
,FTSETradeKind( Ref.FTSETradeKind)
,FTSEObjectBroker( Ref.FTSEObjectBroker)
,FExchangeCode( Ref.FExchangeCode)
,FPVC( Ref.FPVC)
,FStatusCode( Ref.FStatusCode)
,FProcessHost( Ref.FProcessHost)
,FEUCD( Ref.FEUCD)
,FSellSideAccount( Ref.FSellSideAccount )
,FExchangeOrderID( Ref.FExchangeOrderID )
,FStockSeqNo( Ref.FStockSeqNo)
,FOrderQty( Ref.FOrderQty)
,FCumQty( Ref.FCumQty)
,FLastQty( Ref.FLastQty)
,FLeavesQty( Ref.FLeavesQty)
,FNoLegs( Ref.FNoLegs)
,FLegQty1( Ref.FLegQty1)
,FLegQty2( Ref.FLegQty2)
,FBeforeQty( Ref.FBeforeQty)
,FAfterQty( Ref.FAfterQty)
,FReportSequence( Ref.FReportSequence)
,FTMPQty( Ref.FTMPQty)
,FTMPStstusCode( Ref.FTMPStstusCode)
,FTMPBrokerID( Ref.FTMPBrokerID)
,FTMPUniqueID( Ref.FTMPUniqueID)
,FPxDigit( Ref.FPxDigit)
,Fus( Ref.Fus )
,FPartID( Ref.FPartID )
,FTMPPxSubTotal( Ref.FTMPPxSubTotal)
,FPrice( Ref.FPrice)
,FAvgPx( Ref.FAvgPx)
,FLastPx( Ref.FLastPx)
,FStopPx( Ref.FStopPx)
,FStrikePrice( Ref.FStrikePrice)
,FLegPrice1( Ref.FLegPrice1)
,FLegPrice2( Ref.FLegPrice2)
,FReplacePx( Ref.FReplacePx )
,FOrigNID( Ref.FOrigNID )
,FOrderType( Ref.FOrderType)
,FSide( Ref.FSide)
,FTimeInForce( Ref.FTimeInForce)
,FPositionEffect( Ref.FPositionEffect)
,FEventType( Ref.FEventType)
,FExecTransType( Ref.FExecTransType)
,FExecType( Ref.FExecType)
,FTMPExecType( Ref.FTMPExecType)
,FOrderStatus( Ref.FOrderStatus )
,FExecRestatementReason( Ref.FExecRestatementReason )
,FOrdRejReason( Ref.FOrdRejReason )
,FCxlRejResponseTo( Ref.FCxlRejResponseTo )
,FTradingSessionID( Ref.FTradingSessionID )
,FLegSide1( Ref.FLegSide1 )
,FLegSide2( Ref.FLegSide2 )
,FExpireDate( Ref.FExpireDate )
,FFixCxlRejResponseTo( -1 )
,FFixCxlRejReason( -1 )
,FFixOrdRejReason( -1 )
{
    SetMessageType( nsOrderMessageDefine::mtExecutionReport );
}
//---------------------------------------------------------------------------
TExecutionReportMessage::~TExecutionReportMessage(void)
{
    UFCType::Int32 count = FExecutionAttributes.ItemCount();
    for (UFCType::Int32 i = 0; i < count; ++i)
    {
        UFC::AnsiString key;
        UFC::AnsiString* p_value = NULL;
        if (FExecutionAttributes.GetItem(i, key, p_value))
            delete p_value;
    }
}
//---------------------------------------------------------------------------
long TExecutionReportMessage::GetConnectionID( void )
{
    return TNetworkID::GetConnectionID( (UFCType::UInt32)GetNID() );
}
//---------------------------------------------------------------------------
const char*  TExecutionReportMessage::GetExchangeCode( void )
{
    switch( FMarket )
    {
        case nsOrderMessageDefine::mTWFutures:
        case nsOrderMessageDefine::mTWOptions: if( FExchangeCode != "TAIFEX" )
                                                   FExchangeCode = "TAIFEX";
                                               break;
        case nsOrderMessageDefine::mTSE:       if( FExchangeCode != "TWSE" )
                                                   FExchangeCode = "TWSE";
                                               break;
        case nsOrderMessageDefine::mOTC:       if( FExchangeCode != "OTC" )
                                                   FExchangeCode = "OTC";
                                               break;
        case nsOrderMessageDefine::mES:        if( FExchangeCode != "ES" )
                                                  FExchangeCode = "ES";
                                               break;
        default:break;
    }
    return FExchangeCode.c_str();
}
//---------------------------------------------------------------------------
void TExecutionReportMessage::SetAttribute(const char* Key, const char* Value)
{
    UFC::AnsiString attr_name = Key;

    if (FExecutionAttributes.IsExists(attr_name))
    {
        delete FExecutionAttributes.GetObjectByKey(attr_name);
        FExecutionAttributes.DeleteByKey(attr_name);
    }
    FExecutionAttributes.Add(attr_name, new UFC::AnsiString(Value));
}
//---------------------------------------------------------------------------
BOOL TExecutionReportMessage::GetAttribute( UFCType::Int32 index, UFC::AnsiString& key, UFC::AnsiString& value )
{
    UFC::AnsiString* p_value = NULL;
    if (!FExecutionAttributes.GetItem(index, key, p_value))
        return FALSE;

    value = *p_value;
    return TRUE;    
}
//---------------------------------------------------------------------------
BOOL TExecutionReportMessage::GetAttribute(const UFC::AnsiString& key, UFC::AnsiString& value)
{
    if (!FExecutionAttributes.IsExists(key))
        return FALSE;

    UFC::AnsiString* p_value = FExecutionAttributes.GetObjectByKey(key);
    value = *p_value;

    return TRUE;
}