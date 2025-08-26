#ifndef _TBaseMessage_H_
#define _TBaseMessage_H_
//--------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------
#include "../MessageDeliver/TMdMessage.h"
#include "../UFC/NameValueMessage.h"
#include "OrderMessageDefine.h"
//--------------------------------------------------------------------------
class TBaseMessage : public TMdMessage
{
protected:
    UFC::AnsiString                       FAccount;
    UFC::AnsiString                       FAccountFlag;
    UFC::AnsiString                       FAE;
    UFC::AnsiString                       FClOrdID;
    UFC::AnsiString                       FSymbol;
    UFC::AnsiString                       FOrderID;
    UFC::AnsiString                       FBrokerID;
    UFCType::Int64                        FNID;
    UFCType::Int32                        FGroup;
    nsOrderMessageDefine::MarketEnum      FMarket;
    nsOrderMessageDefine::MessageTypeEnum FMessageType;
    UFC::AnsiString                       FLocalIP;
    UFC::AnsiString                       FPeerIP;
    nsOrderMessageDefine::PriceBaseEnum   FPriceBase;
    UFC::AnsiString                       FAENO;

    double                                FStrikePrice;
    nsOrderMessageDefine::EventTypeEnum   FEventType;
    
    UFC::AnsiString FOrderChannel;    
    
    UFC::AnsiString FCASessionID;
    UFC::AnsiString FCASignature;
    UFC::AnsiString FCAPlainText;
    
    bool FIsRequestSend;

public:
    TBaseMessage( void );
    TBaseMessage( TBaseMessage& Ref );
    virtual ~TBaseMessage(void);
private:
    UFC::AnsiString FDATA; ///< All Data
    UFC::AnsiString FUDD;  ///< User Define data
    UFC::AnsiString FGDD;  ///< Gateway Data
    UFC::AnsiString FNVS;  ///< Name values
    void UpdateAllUserData( void );
public:
    void        SetAllUserData( const UFC::AnsiString& DataStr );
    const char* GetAllUserData( void );
    ///< Functions to access GatewayDefineData.(GDD) for Yuanta
    void        SetGatewayData( const char* Value );
    const char* GetGatewayData( void );
    ///< Functions to access UserDefineData.(UDD)
    void        SetUserData( const char* Value );
    const char* GetUserData( void );
    void        SetData( const char* Value ) { SetUserData( Value );}
    const char* GetData( void )              { return GetUserData( ); }
    ///< Functions to access user define Name/Value pairs
    void        SetValue( const char* Name, const char* Value );
    bool        GetValue( const UFC::AnsiString& Name, UFC::AnsiString& Value );
    bool        ValueExists( const char* Name );
public:
	///< Set functions
    void SetAccount( const char* Value )                               { FAccount = Value; }
    void SetAE( const char* Value )                                    { FAE = Value; }
    void SetClOrdID( const char* Value )                               { FClOrdID = Value; }
    void SetSymbol( const char* Value )                                { FSymbol = Value; }
    void SetMarket( nsOrderMessageDefine::MarketEnum Value )           { FMarket = Value; }
    void SetMessageType( nsOrderMessageDefine::MessageTypeEnum Value ) { FMessageType = Value; }
    void SetBrokerID( const char* Value )                              { FBrokerID = Value; }
    void SetAccountFlag( const char* Value )                           { FAccountFlag = Value; }
    void SetNID( UFCType::Int64 Value )                                { FNID = Value; }
    void SetGroup( int Value )                                         { FGroup = Value; }
    void SetOrderID( const char* Value )                               { FOrderID = Value; }
    void SetLocalIP( const char* Value )                               { FLocalIP = Value; }
    void SetPeerIP( const char* Value )                                { FPeerIP = Value; }
    void SetPxBase( nsOrderMessageDefine::PriceBaseEnum Value )        { FPriceBase = Value; }
    void SetAENO( const char* Value )                                  { FAENO = Value; }
    void SetStrikePrice( double Value )                                { FStrikePrice = Value; }
    void SetEventType( nsOrderMessageDefine::EventTypeEnum Value )     { FEventType = Value; }
    void SetOrderChannel( const char* Value )                          { FOrderChannel = Value; }
    
    void SetCASessionID( const char* Value )                           { FCASessionID = Value; }
    void SetCASignature( const char* Value )                           { FCASignature = Value; }
    void SetCAPlainText( const char* Value )                           { FCAPlainText = Value; }
    void SetIsRequestSend( bool Value )                                { FIsRequestSend = Value; }
    
    ///< Get functions
    const char*                           GetAccount( void )       { return FAccount.c_str();}
    const char*                           GetAE( void )            { return FAE.c_str();}
    const char*                           GetClOrdID( void )       { return FClOrdID.c_str();}
    const char*                           GetSymbol( void )        { return FSymbol.c_str();}
    nsOrderMessageDefine::MarketEnum      GetMarket( void )        { return FMarket; }
    nsOrderMessageDefine::MessageTypeEnum GetMessageType( void )   { return FMessageType; }
    const char*                           GetBrokerID( void )      { return FBrokerID.c_str();}
    const char*                           GetAccountFlag( void )   { return FAccountFlag.c_str();}
    const char*                           GetLocalIP( void )       { return FLocalIP.c_str();}
    const char*                           GetPeerIP( void )        { return FPeerIP.c_str();}
    nsOrderMessageDefine::PriceBaseEnum   GetPxBase( void )        { return FPriceBase; }
    const char*                           GetAENO( void )          { return FAENO.c_str();}
    double                                GetStrikePrice( void )   { return FStrikePrice; }
    nsOrderMessageDefine::EventTypeEnum   GetEventType( void )     { return FEventType; }
    const char*                           GetOrderChannel( void )  { return FOrderChannel.c_str(); }
    const char*                           GetCASessionID( void )   { return FCASessionID.c_str(); }
    const char*                           GetCASignature( void )   { return FCASignature.c_str(); }
    const char*                           GetCAPlainText( void )   { return FCAPlainText.c_str(); }
    bool                                  GetIsRequestSend( void ) { return FIsRequestSend; }
    UFCType::Int64                        GetNID( void )           { return FNID; }
    int                                   GetGroup( void )         { return FGroup; }
    
    ///< For Order interface.
    virtual nsOrderMessageDefine::OrderTypeEnum      GetOrderType( void )       { return nsOrderMessageDefine::otLimit; }
    virtual nsOrderMessageDefine::SideEnum           GetSide( void )            { return nsOrderMessageDefine::sBuy;}
    virtual nsOrderMessageDefine::TimeInForceEnum    GetTimeInForce( void )     { return nsOrderMessageDefine::tifROD;}
    virtual nsOrderMessageDefine::PositionEffectEnum GetPositionEffect( void )  { return nsOrderMessageDefine::peOpen;}
    virtual const char*                              GetOrderID( void)          { return FOrderID.c_str();}
    virtual const char*                              GetMaturityMonthYear(void) { return "000000";}
    virtual long                                     GetOrderQty( void )        { return 0; }
    virtual double                                   GetPrice( void )           { return 0.0; }
};
//--------------------------------------------------------------------------
#endif

