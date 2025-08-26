#ifndef _TExecutionReportMessage_H_
#define _TExecutionReportMessage_H_
//--------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "TBaseMessage.h"
//--------------------------------------------------------------------------
class TExecutionReportMessage : public TBaseMessage
{
private:
    UFC::AnsiString FMaturityMonthYear;
    UFC::AnsiString FOrigClOrdID;
    UFC::AnsiString FExecID;
    UFC::AnsiString FExecRefID;
    UFC::AnsiString FOrigExecID;
    UFC::AnsiString FText;
    UFC::AnsiString FSrc;
    UFC::AnsiString FLINBRN;
    UFC::AnsiString FOrgTransactTime;
    UFC::AnsiString FTransactTime;
    UFC::AnsiString FMsgTime;
    UFC::AnsiString FTMPExt;
    UFC::AnsiString FLegSymbol1;
    UFC::AnsiString FLegSymbol2;
    UFC::AnsiString FCMID;
    UFC::AnsiString FRecNo;
    UFC::AnsiString FTSEOrderType;
    UFC::AnsiString FTSEExchangeCode;
    UFC::AnsiString FTSETradeKind;
    UFC::AnsiString FTSEObjectBroker;
    UFC::AnsiString FExchangeCode;
    UFC::AnsiString FPVC;
    UFC::AnsiString FStatusCode;
    UFC::AnsiString FProcessHost;
    UFC::AnsiString FEUCD;
    UFC::AnsiString FSellSideAccount;
    UFC::AnsiString FExchangeOrderID;
    long            FStockSeqNo;
    long            FOrderQty;
    long            FCumQty;
    long            FLastQty;
    long            FLeavesQty;
    long            FNoLegs;
    long            FLegQty1;
    long            FLegQty2;
    long            FBeforeQty;
    long            FAfterQty;
    long            FReportSequence;
    long            FTMPQty;
    long            FTMPStstusCode;
    long            FTMPBrokerID;
    long            FTMPUniqueID;
    long            FPxDigit;
    long            Fus;
    long            FPartID;
    double          FTMPPxSubTotal;
    double          FPrice;
    double          FAvgPx;
    double          FLastPx;
    double          FStopPx;
    double          FStrikePrice;
    double          FLegPrice1;
    double          FLegPrice2;
    bool            FReplacePx;
    Int64           FOrigNID;
    nsOrderMessageDefine::OrderTypeEnum             FOrderType;
    nsOrderMessageDefine::SideEnum                  FSide;
    nsOrderMessageDefine::TimeInForceEnum           FTimeInForce;
    nsOrderMessageDefine::PositionEffectEnum        FPositionEffect;
    nsOrderMessageDefine::EventTypeEnum             FEventType;
    nsOrderMessageDefine::ExecTransTypeEnum         FExecTransType;
    nsOrderMessageDefine::ExecTypeEnum              FExecType;
    nsOrderMessageDefine::TMPExecTypeEnum           FTMPExecType;
    nsOrderMessageDefine::OrderStatusEnum           FOrderStatus;
    nsOrderMessageDefine::ExecRestatementReasonEnum FExecRestatementReason;
    nsOrderMessageDefine::OrdRejReasonEnum          FOrdRejReason;
    nsOrderMessageDefine::CxlRejResponseToEnum      FCxlRejResponseTo;
    nsOrderMessageDefine::TradingSessionIDEnum      FTradingSessionID;
    nsOrderMessageDefine::SideEnum                  FLegSide1;
    nsOrderMessageDefine::SideEnum                  FLegSide2;
    UFC::AnsiString FExpireDate;  //Tag 432 for Tag 59 TimeInForce:Good till Date
    
    long FFixCxlRejResponseTo;
    long FFixCxlRejReason;
    long FFixOrdRejReason;
    
    UFC::PHashedList<UFC::AnsiString, UFC::AnsiString*> FExecutionAttributes;

public:
    TExecutionReportMessage( void );
    TExecutionReportMessage( TExecutionReportMessage& Ref );
public:
    ~TExecutionReportMessage(void);
public:
    void SetMaturityMonthYear( const char* Value )                                            { FMaturityMonthYear = Value; }
    void SetOrigClOrdID( const char* Value )                                                  { FOrigClOrdID = Value; }
    void SetExecID( const char* Value )                                                       { FExecID = Value; }
    void SetExecRefID( const char* Value )                                                    { FExecRefID = Value; }
    void SetOrigExecID( const char* Value )                                                   { FOrigExecID = Value; }
    void SetText( const char* Value )                                                         { FText = Value; }
    void SetOrderType( nsOrderMessageDefine::OrderTypeEnum Value )                            { FOrderType = Value; }
    void SetSide( nsOrderMessageDefine::SideEnum Value )                                      { FSide = Value; }
    void SetTimeInForce( nsOrderMessageDefine::TimeInForceEnum Value )                        { FTimeInForce = Value; }
    void SetPositionEffect( nsOrderMessageDefine::PositionEffectEnum Value )                  { FPositionEffect = Value; }
    void SetEventType( nsOrderMessageDefine::EventTypeEnum Value )                            { FEventType = Value; }
    void SetExecTransType( nsOrderMessageDefine::ExecTransTypeEnum Value )                    { FExecTransType = Value; }
    void SetExecType( nsOrderMessageDefine::ExecTypeEnum Value )                              { FExecType = Value; }
    void SetTMPExecType( nsOrderMessageDefine::TMPExecTypeEnum Value )                        { FTMPExecType = Value; }
    void SetOrderStatus( nsOrderMessageDefine::OrderStatusEnum Value )                        { FOrderStatus = Value; }
    void SetExecRestatementReason( nsOrderMessageDefine::ExecRestatementReasonEnum Value )    { FExecRestatementReason = Value; }
    void SetOrdRejReason( nsOrderMessageDefine::OrdRejReasonEnum Value )                      { FOrdRejReason = Value; }
    void SetCxlRejResponseTo( nsOrderMessageDefine::CxlRejResponseToEnum Value )              { FCxlRejResponseTo = Value; }
    void SetTradingSessionID( nsOrderMessageDefine::TradingSessionIDEnum Value )              { FTradingSessionID = Value; }
    void SetOrderQty( long Value )                                                            { FOrderQty = Value; }
    void SetCumQty( long Value )                                                              { FCumQty = Value; }
    void SetLastQty( long Value )                                                             { FLastQty = Value; }
    void SetLeavesQty( long Value )                                                           { FLeavesQty = Value; }
    void SetPrice( double Value )                                                             { FPrice = Value; }
    void SetAvgPx( double Value )                                                             { FAvgPx = Value; }
    void SetLastPx( double Value )                                                            { FLastPx = Value; }
    void SetStopPx( double Value )                                                            { FStopPx = Value; }
    void SetStrikePrice( double Value )                                                       { FStrikePrice = Value; }
    void SetSrc( const char* Value )                                                          { FSrc = Value; }
    void SetLINBRN( const char* Value )                                                       { FLINBRN = Value; }
    void SetOrgTransactTime( const char* hhMMssmmm )                                          { FOrgTransactTime = hhMMssmmm; }
    void SetTransactTime( const char* hhMMssmmm )                                             { FTransactTime = hhMMssmmm; }
    void SetTMPExt( const char* Value )                                                       { FTMPExt = Value; }
    void SetNoLegs( long Value )                                                              { FNoLegs = Value; }
    void SetLegSymbol1( const char* Value )                                                   { FLegSymbol1 = Value; }
    void SetLegSide1( nsOrderMessageDefine::SideEnum Value )                                  { FLegSide1 = Value; }
    void SetLegQty1( long Value )                                                             { FLegQty1 = Value; }
    void SetLegPrice1( double Value )                                                         { FLegPrice1 = Value; }
    void SetLegSymbol2( const char* Value )                                                   { FLegSymbol2 = Value; }
    void SetLegSide2( nsOrderMessageDefine::SideEnum Value )                                  { FLegSide2 = Value; }
    void SetLegQty2( long Value )                                                             { FLegQty2 = Value; }
    void SetLegPrice2( double Value )                                                         { FLegPrice2 = Value; }
    void SetBeforeQty( long Value )                                                           { FBeforeQty = Value; }
    void SetAfterQty( long Value )                                                            { FAfterQty = Value; }
    void SetCMID( const char* Value )                                                         { FCMID = Value; }
    void SetRecNo( const char* Value )                                                        { FRecNo = Value; }
    void SetTSEOrderType( const char* Value )                                                 { FTSEOrderType = Value; }
    void SetTSEExchangeCode( const char* Value )                                              { FTSEExchangeCode = Value; }
    void SetTSETradeKind( const char* Value )                                                 { FTSETradeKind = Value; }
    void SetTSEObjectBroker( const char* Value )                                              { FTSEObjectBroker = Value; }
    void SetExchangeCode( const char* Value )                                                 { FExchangeCode = Value; }
    void SetPVC( const char* Value )                                                          { FPVC = Value; }
    void SetStatusCode( const char* Value )                                                   { FStatusCode = Value; }
    void SetStockSeqNo( long Value )                                                          { FStockSeqNo = Value; }
    void SetProcessHost( const char* Value )                                                  { FProcessHost = Value; }
    void SetReportSequence( long Value )                                                      { FReportSequence = Value; }
    void SetTMPQty( long Value )                                                              { FTMPQty = Value; }
    void SetTMPStstusCode( long Value )                                                       { FTMPStstusCode = Value; }
    void SetTMPBrokerID( long Value )                                                         { FTMPBrokerID = Value; }
    void SetTMPUniqueID( long Value )                                                         { FTMPUniqueID = Value; }
    void SetTMPPxSubTotal( double Value )                                                     { FTMPPxSubTotal = Value; }
    void SetTMPPartID( long Value )                                                           { FPartID = Value; }
    void SetPxDigit( long Value )                                                             { FPxDigit = Value; }
    void SetUseus( long Value )                                                               { Fus = Value; }
    void SetEUCD( const char* Value )                                                         { FEUCD = Value; }
    void SetExpireDate( const char* Value )                                                   { FExpireDate = Value; }
    void SetMessageTime( const char* Value )                                                  { FMsgTime = Value; }
    void SetReplacePx( bool Rpx  )                                                            { FReplacePx = Rpx; }
    void SetSellSideAccount( const char* Value )                                              { FSellSideAccount = Value; }
    void SetExchangeOrderID( const char* Value )                                              { FExchangeOrderID = Value; }
    void SetOrigNID( Int64 ONID )                                                             { FOrigNID = ONID; }
    void SetFixCxlRejResponseTo( long Value )                                                 { FFixCxlRejResponseTo = Value; }
    void SetFixCxlRejReason( long Value )                                                     { FFixCxlRejReason = Value; }
    void SetFixOrdRejReason( long Value )                                                     { FFixOrdRejReason = Value; }
    void SetAttribute(const char* Key, const char* Value);
        
    const char*                                     GetSrc( void )                      {return FSrc.c_str(); }
    const char*                                     GetLINBRN( void )                   {return FLINBRN.c_str(); }
    const char*                                     GetMaturityMonthYear( void )        {return FMaturityMonthYear.c_str(); }
    const char*                                     GetOrigClOrdID( void )              {return FOrigClOrdID.c_str(); }
    const char*                                     GetExecID( void )                   {return FExecID.c_str(); }
    const char*                                     GetExecRefID( void )                {return FExecRefID.c_str(); }
    const char*                                     GetOrigExecID( void )               {return FOrigExecID.c_str(); }
    const char*                                     GetText( void )                     {return FText.c_str(); }
    const char*                                     GetOrgTransactTime( void )          {return FOrgTransactTime.c_str(); }
    const char*                                     GetTransactTime( void )             {return FTransactTime.c_str(); }
    long                                            GetConnectionID( void );
    nsOrderMessageDefine::OrderTypeEnum             GetOrderType( void )                {return FOrderType; }
    nsOrderMessageDefine::SideEnum                  GetSide( void )                     {return FSide; }
    nsOrderMessageDefine::TimeInForceEnum           GetTimeInForce( void )              {return FTimeInForce; }
    nsOrderMessageDefine::PositionEffectEnum        GetPositionEffect( void )           {return FPositionEffect; }
    nsOrderMessageDefine::EventTypeEnum             GetEventType( void )                {return FEventType; }
    nsOrderMessageDefine::ExecTransTypeEnum         GetExecTransType( void )            {return FExecTransType; }
    nsOrderMessageDefine::ExecTypeEnum              GetExecType( void )                 {return FExecType; }
    nsOrderMessageDefine::OrderStatusEnum           GetOrderStatus( void )              {return FOrderStatus; }
    nsOrderMessageDefine::ExecRestatementReasonEnum GetExecRestatementReason( void )    {return FExecRestatementReason; }
    nsOrderMessageDefine::OrdRejReasonEnum          GetOrdRejReason( void )             {return FOrdRejReason; }
    nsOrderMessageDefine::CxlRejResponseToEnum      GetCxlRejResponseTo( void )         {return FCxlRejResponseTo; }
    nsOrderMessageDefine::TradingSessionIDEnum      GetTradingSessionID( void )         {return FTradingSessionID; }
    nsOrderMessageDefine::TMPExecTypeEnum           GetTMPExecType( void )              {return FTMPExecType; }
    long                                            GetOrderQty( void )                 {return FOrderQty; }
    long                                            GetCumQty( void )                   {return FCumQty; }
    long                                            GetLastQty( void )                  {return FLastQty; }
    long                                            GetLeavesQty( void )                {return FLeavesQty; }
    double                                          GetPrice( void )                    {return FPrice; }
    double                                          GetLastPx( void )                   {return FLastPx; }
    double                                          GetAvgPx( void )                    {return FAvgPx; }
    double                                          GetStopPx( void )                   {return FStopPx; }
    double                                          GetStrikePrice( void )              {return FStrikePrice; }
    const char*                                     GetTMPExt( void )                   {return FTMPExt.c_str(); }
    long                                            GetNoLegs( void )                   {return FNoLegs; }
    const char*                                     GetLegSymbol1( void )               {return FLegSymbol1.c_str(); }
    nsOrderMessageDefine::SideEnum                  GetLegSide1( void )                 {return FLegSide1; }
    long                                            GetLegQty1( void )                  {return FLegQty1; }
    double                                          GetLegPrice1( void )                {return FLegPrice1; }
    const char*                                     GetLegSymbol2( void )               {return FLegSymbol2.c_str(); }
    nsOrderMessageDefine::SideEnum                  GetLegSide2( void )                 {return FLegSide2; }
    long                                            GetLegQty2( void )                  {return FLegQty2; }
    double                                          GetLegPrice2( void )                {return FLegPrice2; }
    long                                            GetBeforeQty( void )                {return FBeforeQty; }
    long                                            GetAfterQty( void )                 {return FAfterQty; }
    const char*                                     GetCMID( void )                     {return FCMID.c_str(); }
    const char*                                     GetRecNo( void )                    {return FRecNo.c_str(); }
    const char*                                     GetTSEOrderType( void )             {return FTSEOrderType.c_str(); }
    const char*                                     GetTSEExchangeCode( void )          {return FTSEExchangeCode.c_str(); }
    const char*                                     GetTSETradeKind( void )             {return FTSETradeKind.c_str(); }
    const char*                                     GetTSEObjectBroker( void )          {return FTSEObjectBroker.c_str(); }
    const char*                                     GetExchangeCode( void );
    const char*                                     GetPVC( void )                      {return FPVC.c_str(); }
    const char*                                     GetStatusCode( void )               {return FStatusCode.c_str(); }
    long                                            GetStockSeqNo( void )               {return FStockSeqNo; }
    const char*                                     GetProcessHost( void )              {return FProcessHost.c_str(); }
    long                                            GetReportSequence( void )           {return FReportSequence; }
    long                                            GetTMPQty( void )                   {return FTMPQty; }
    long                                            GetTMPStstusCode( void )            {return FTMPStstusCode; }
    long                                            GetTMPBrokerID( void )              {return FTMPBrokerID; }
    long                                            GetTMPUniqueID( void )              {return FTMPUniqueID; }
    double                                          GetTMPPxSubTotal( void )            {return FTMPPxSubTotal; }
    long                                            GetTMPPartID( void )                {return FPartID; }
    long                                            GetPxDigit( void )                  {return FPxDigit; }
    long                                            GetUseus( void )                    {return Fus; }
    const char*                                     GetEUCD( void )                     {return FEUCD.c_str(); }
    const char*                                     GetExpireDate( void )               {return FExpireDate.c_str(); }
    const char*                                     GetMessageTime( void )              {return FMsgTime.c_str(); }
    const char*                                     GetSellSideAccount( void )          {return FSellSideAccount.c_str(); }
    const char*                                     GetExchangeOrderID( void )          {return FExchangeOrderID.c_str(); }
    bool                                            IsReplacePx( void )                 {return FReplacePx; }
    Int64                                           GetOrigNID( void )                  {return FOrigNID; }
    long                                            GetFixCxlRejResponseTo()            {return FFixCxlRejResponseTo; }
    long                                            GetFixCxlRejReason()                {return FFixCxlRejReason; }
    long                                            GetFixOrdRejReason()                {return FFixOrdRejReason; }
    UFCType::Int32                                  GetAttributeCount()                 { return FExecutionAttributes.ItemCount(); }
    BOOL                                            GetAttribute( UFCType::Int32 index, UFC::AnsiString& key, UFC::AnsiString& value );
    BOOL                                            GetAttribute( const UFC::AnsiString& key, UFC::AnsiString& value );
};
//--------------------------------------------------------------------------
#endif

