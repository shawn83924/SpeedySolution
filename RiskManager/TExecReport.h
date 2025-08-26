/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TExecReport.h
 * Author: yuan
 *
 * Created on October 25, 2019, 8:24 AM
 */

#ifndef TEXECREPORT_H
#define TEXECREPORT_H

#include "../UFC/AnsiString.h"
#include "../COM/Lib/OrderMessage/OrderMessageDefine.h"

//--------------------------------------------------------------------------
class TExecReport
{
private:    
    UFC::AnsiString FOrderID;
    UFC::AnsiString FBrokerID;
    UFC::AnsiString FAccount;
    UFC::AnsiString FAE;
    UFC::AnsiString FClOrdID;
    UFC::AnsiString FUDD;  ///< User Define data
    UFC::AnsiString FSymbol;                    
    UFC::AnsiString FExecID;    
    UFC::AnsiString FLegSymbol1;
    UFC::AnsiString FLegSymbol2;    
    UFC::AnsiString FStatusCode;
    
    long            FOrderQty;  
    long            FLastQty;        
    long            FNoLegs;
    long            FLegQty1;
    long            FLegQty2;
    long            FBeforeQty;
    long            FAfterQty;
    
    double          FPrice;    
    double          FLastPx;
    double          FLegPrice1;
    double          FLegPrice2; 
    
    nsOrderMessageDefine::MarketEnum                FMarket;
    nsOrderMessageDefine::SideEnum                  FSide;   
    nsOrderMessageDefine::PositionEffectEnum        FPositionEffect;
    nsOrderMessageDefine::ExecTypeEnum              FExecType;
    nsOrderMessageDefine::TMPExecTypeEnum           FTMPExecType;
    nsOrderMessageDefine::OrderStatusEnum           FOrderStatus;
    nsOrderMessageDefine::CxlRejResponseToEnum      FCxlRejResponseTo;        
    nsOrderMessageDefine::SideEnum                  FLegSide1;
    nsOrderMessageDefine::SideEnum                  FLegSide2;

    
public:
    TExecReport( void )
    :FOrderID( "00000" )
    ,FAccount( "       " )
    ,FAE( "     " )
    ,FClOrdID( "   " )
    ,FSymbol("     ")       
    ,FExecID( "" )    
    ,FStatusCode( "00" )
    ,FOrderQty( 0 )
    ,FLastQty( 0 )    
    ,FNoLegs( 0 )
    ,FLegQty1( 0 )
    ,FLegQty2( 0 )
    ,FBeforeQty( 0 )
    ,FAfterQty( 0 )
    ,FPrice( 0.0 )
    ,FLastPx( 0.0 )
    ,FLegPrice1( 0.0 )
    ,FLegPrice2( 0.0 )
    ,FMarket( nsOrderMessageDefine::mTWFutures )
    ,FSide( nsOrderMessageDefine::sNone )
    ,FPositionEffect( nsOrderMessageDefine::peOpen )
    ,FOrderStatus( nsOrderMessageDefine::osNone )    
    ,FCxlRejResponseTo( nsOrderMessageDefine::crrNone )    
    ,FLegSide1( nsOrderMessageDefine::sNone )
    ,FLegSide2( nsOrderMessageDefine::sNone )
    { };
    
public:
    ~TExecReport(void){};
public:        
    void SetOrderID( const char* Value )                                                      { FOrderID = Value; }    
    void SetBrokerID( const char* Value )                                                     { FBrokerID = Value; }
    void SetAccount( const char* Value )                                                      { FAccount = Value; }    
    void SetAE( const char* Value )                                                           { FAE = Value; }
    void SetClOrdID( const char* Value )                                                      { FClOrdID = Value; }
    void SetUserData( const char* Value )                                                     { FUDD = Value; }
    void SetSymbol( const char* Value )                                                       { FSymbol = Value; }
    void SetMarket( nsOrderMessageDefine::MarketEnum Value )                                  { FMarket = Value; }
    
    void SetExecID( const char* Value )                                                       { FExecID = Value; }
    void SetSide( nsOrderMessageDefine::SideEnum Value )                                      { FSide = Value; }
    void SetPositionEffect( nsOrderMessageDefine::PositionEffectEnum Value )                  { FPositionEffect = Value; }
    void SetExecType( nsOrderMessageDefine::ExecTypeEnum Value )                              { FExecType = Value; }
    void SetTMPExecType( nsOrderMessageDefine::TMPExecTypeEnum Value )                        { FTMPExecType = Value; }
    void SetOrderStatus( nsOrderMessageDefine::OrderStatusEnum Value )                        { FOrderStatus = Value; }
    void SetCxlRejResponseTo( nsOrderMessageDefine::CxlRejResponseToEnum Value )              { FCxlRejResponseTo = Value; }    
    
    void SetOrderQty( long Value )                                                            { FOrderQty = Value; }
    void SetLastQty( long Value )                                                             { FLastQty = Value; }    
    void SetPrice( double Value )                                                             { FPrice = Value; }
    void SetLastPx( double Value )                                                            { FLastPx = Value; }
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
    void SetStatusCode( const char* Value )                                                   { FStatusCode = Value; }
    
    const char*                                     GetOrderID( void)                   {return FOrderID.c_str(); }
    const char*                                     GetBrokerID( void )                 {return FBrokerID.c_str();}
    const char*                                     GetAccount( void )                  {return FAccount.c_str();}
    const char*                                     GetAE( void )                       {return FAE.c_str();}
    const char*                                     GetClOrdID( void )                  {return FClOrdID.c_str();}
    const char*                                     GetUserData( void )                 {return FUDD.c_str();}
    const char*                                     GetSymbol( void )                   {return FSymbol.c_str();}
    nsOrderMessageDefine::MarketEnum                GetMarket( void )                   {return FMarket; }
    
    const char*                                     GetExecID( void )                   {return FExecID.c_str(); }
    nsOrderMessageDefine::SideEnum                  GetSide( void )                     {return FSide; }
    nsOrderMessageDefine::PositionEffectEnum        GetPositionEffect( void )           {return FPositionEffect; }
    nsOrderMessageDefine::ExecTypeEnum              GetExecType( void )                 {return FExecType; }
    nsOrderMessageDefine::OrderStatusEnum           GetOrderStatus( void )              {return FOrderStatus; }
    nsOrderMessageDefine::CxlRejResponseToEnum      GetCxlRejResponseTo( void )         {return FCxlRejResponseTo; }
    nsOrderMessageDefine::TMPExecTypeEnum           GetTMPExecType( void )              {return FTMPExecType; }
    
    
    
    long                                            GetOrderQty( void )                 {return FOrderQty; }
    long                                            GetLastQty( void )                  {return FLastQty; }
    double                                          GetPrice( void )                    {return FPrice; }
    double                                          GetLastPx( void )                   {return FLastPx; }    
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
    const char*                                     GetStatusCode( void )               {return FStatusCode.c_str(); }
};
//--------------------------------------------------------------------------


#endif /* TEXECREPORT_H */

