/**
 *  @file TransactionObject.cpp
 *  @brief Core Class to process all kinds of orders.
 *  @author Simon Chaang
 */
#include "TMPTransactionObject.h"
#include "ConnectionObject.h"
#include "TMPOrderObjectsManager.h"
#include "../TMP34/OdrUtility.h"
#include "../TAIFEXLib/RMessages.h"
#include "../../inc/TMP_msg.h"
#include "../../inc/msglib.h"
//----------------------------------------------------------------------------------------------------------------------    
extern MexObject*      MexObj;
extern int	       PortNo;
extern BOOL            IsOption;
extern UFC::AnsiString TFX_QUOTE_REQUEST_SUBJECT;
extern UFC::AnsiString TFX_FT_REQUEST_SUBJECT;
extern Int32           RejectOrderSec;
//----------------------------------------------------------------------------------------------------------------------    
TMPTransactionObject::TMPTransactionObject( TMPOrderManager* OrderManager, const UFC::AnsiString& DBFileName  )
:FTMPOrderManager( OrderManager )
,FNewOrderChecker()
{
    FNewOrderChecker.setOptionFlag(  IsOption );
    UFC::BufferedLog::Printf( " ------------- OrderChecker load data ------------- " );
    if( FNewOrderChecker.loadDatabaseData( DBFileName.c_str() ) != 0 )
    {
        UFC::BufferedLog::Printf( " OrderChecker load data from DB error." );
        COUT.MSG( TMP34_INITIAL_ERROR, "OrderChecker load data from DB error." );
        exit( 1 );
    }
    UFC::BufferedLog::Printf( " -------------------- Finished --------------------- " );
}        
//----------------------------------------------------------------------------------------------------------------------
void TMPTransactionObject::OnTradingStatusChange( UInt8 Group, UInt8 Openswitch )
{
    FNewOrderChecker.setOpenSwitch( Group, (nsOrder::OpenSwitchValue)Openswitch );
}
//----------------------------------------------------------------------------------------------------------------------
void TMPTransactionObject::OnEvent(int /*Type*/, void* )
{
   
}
//----------------------------------------------------------------------------------------------------------------------
TMP::TMPMessage* TMPTransactionObject::OnMessage( ConnectionObjectBase* Sender ,TMP::TMPMessage* InMsg, UInt8 Status ) 
{ 
    UInt16 FCMIndex = Sender->GetConnectionParameter()->GetBrokerIndex();
    UInt16 SessionID =  Sender->GetConnectionParameter()->GetSessionID();       
         
    switch( InMsg->GetMessageType() )
    {
       case TMP::tmtR04: return new TMP::TMPMessage(TMP::tmtR05 ); // R050 Confirm Connection message
       case TMP::tmtR05: //UFC::BufferedLog::Printf( " Receive heartbeat message.");
                         break;
       case TMP::tmtR01: ///< Order (New,Cancel,Replace,OrderStatusRequest)
                         return HandleR01( FCMIndex , SessionID ,*InMsg, Status ) ;
       case TMP::tmtR07: ///< Quote request.
                         return HandleR07( FCMIndex , SessionID ,*InMsg ) ;                         
       case TMP::tmtR09: ///< Quote.
                         return HandleR09( FCMIndex , SessionID ,*InMsg, Status ) ;
       case TMP::tmtR11: ///< Trading status
                         return HandleR11( FCMIndex , SessionID ,*InMsg ) ;
       case TMP::tmtR13: return HandleR13( FCMIndex , SessionID ,*InMsg ) ;
       default:          UFC::BufferedLog::Printf( " Unhandle TMP message Type:%d.", InMsg->GetMessageType() );
                         break;
    }
    return NULL;
}
//----------------------------------------------------------------------------------------------------------------------
TMP::TMPMessage* TMPTransactionObject::HandleR01( UInt16 FCMIndex, UInt16 SessionID, TMP::TMPMessage& R01,UInt8 Status )
{
    odm_In          Odr;
    BOOL            CheckPxLimit = TRUE;
    TMP::TMPTime    RecvTime;
    TMP::TMPSymbol* symbol = (TMP::TMPSymbol*)R01.GetField( R01_Symbol );

    memset( &Odr, 0, sizeof( odm_In ));
    R01[ R01_ExecType ].ToBinary( (UInt8*)&Odr.header.ExecType );
    if( R01.GetMessageSeq() == 0 && Odr.header.ExecType != 'I' )
        throw( TMPRehandshakeException("Receive R01 without seq",207));
    if( FTMPOrderManager->IsOdrServerConnected() == FALSE )
        return OdrUtility::RejectR01( R01, 99 ); ///< Odr_server disconnected.
    ///< fill header.    
    Odr.header.msg_tag       = (uint8_t) mtIn;    
    Odr.header.source_fcm_id = FCMIndex;
    Odr.header.session_id    = SessionID;
    Odr.header.protocol      = 1; ///TMP
    Odr.header.tfx_grp_id    = 0;
    Odr.header.network_id    = (UInt64)UFC::GetTickCountMS();
    Odr.header.client_id     = FTMPOrderManager->GetProcessID();        
    ///< fill ord_data_t    
    R01[ R01_Price ].ToBinary( (UInt8*)&Odr.BidPx );
    R01[ R01_qty ].ToBinary( (UInt8*)&Odr.BidSize );
    R01[ R01_cm_id ].ToBinary( (UInt8*)&Odr.ord_data.cm_id );
    R01[ R01_fcm_id ].ToBinary( (UInt8*)&Odr.ord_data.fcm_id );
    R01[ R01_order_no ].ToBinary( (UInt8*)Odr.ord_data.order_no );
    R01[ R01_investor_flag ].ToBinary( (UInt8*)&Odr.ord_data.investor_flag );
    R01[ R01_ord_id ].ToBinary( (UInt8*)&Odr.ord_data.order_id );
    R01[ R01_user_define ].ToBinary( (UInt8*) Odr.ord_data.user_define );
    R01[ R01_investor_acno ].ToBinary( (UInt8*)&Odr.ord_data.investor_acno );
    R01[ R01_Side ].ToBinary( (UInt8*)&Odr.ord_data.Side );
    R01[ R01_OrdType ].ToBinary( (UInt8*)&Odr.ord_data.OrdType );
    R01[ R01_TimeInForce ].ToBinary( (UInt8*)&Odr.ord_data.TimeInForce );
    R01[ R01_PositionEffect ].ToBinary( (UInt8*)&Odr.ord_data.PositionEffect );
     ///< Fetch binary Symbol
    if(  symbol->GetBinSymbol( Odr.ord_data.symbol.symbol_type,
                               Odr.ord_data.Side,
                               Odr.ord_data.symbol.pseq1, Odr.ord_data.symbol.leg_side[0],
                               Odr.ord_data.symbol.pseq2, Odr.ord_data.symbol.leg_side[1],
                               Odr.ord_data.symbol.comb_op ) == TRUE )
    {
        ///< Check order.
        Int32 TimeDiff = RecvTime.DiffMS( R01.GetMessageTime());
        if( TimeDiff > RejectOrderSec || TimeDiff < - RejectOrderSec )
        {
            Odr.ord_data.status_code = 242; ///< Reject order
        }
        else///< Check Order
        {
            char ErrMsg[256];
            ///< STF Multileg order not support ROD. add by Simon 2009/02/26
            if( TMP::TMPMessage::GetSystemType() >= 20 &&  ///< Futures
                symbol->IsMultileg( ) == TRUE )            ///< Multileg
            {
                if( FNewOrderChecker.isPdkStl( Odr.ord_data.symbol.pseq1 ) == TRUE )
                {
                    if( Odr.ord_data.TimeInForce == 0 ) ///< ROD, Reject it
                    {
                        UFC::BufferedLog::Printf( " Reject Order, Order[%s] is a multileg ROD STF symbol", R01[ R01_order_no ].ToString().c_str() );
                        Odr.ord_data.status_code = 27; ///< Invalid symbol.
                        FTMPOrderManager->SendToOdrServer( &Odr );
                        return NULL;
                    }
                    else ///< Ok, but don't check the price limit
                        CheckPxLimit = FALSE;
                }
            }
            ///< End modify
            if( (Odr.ord_data.status_code = FNewOrderChecker.doCheck( Odr, ErrMsg, CheckPxLimit )) != 0 )
            {
                UFC::BufferedLog::DebugPrintf( " Failed Code[%d][%s]", Odr.ord_data.status_code, ErrMsg );
                if( Odr.ord_data.status_code == 1 ) ///<Market closed.
                    return OdrUtility::RejectR01( R01, 1 );
            }
            if( Status != 0 && Odr.ord_data.status_code == 0 ) ///< Overwrite the throughput warning error code.
                Odr.ord_data.status_code = Status;
            UInt32 Now = UFC::GetTickCountMS();
            UFC::BufferedLog::DebugPrintf( UFC::dlPerformance," Tick:%u Check OrderID[%s] Use[%d ms]",Now, R01[R01_order_no].ToString().c_str(), Now - (UInt32)Odr.header.network_id );
        }
    }
    else
        Odr.ord_data.status_code = 20; ///< Invalid symbol.
    FTMPOrderManager->SendToOdrServer( &Odr );
    return NULL;
}
//----------------------------------------------------------------------------------------------------------------------
TMP::TMPMessage* TMPTransactionObject::HandleR09( UInt16 FCMIndex, UInt16 SessionID, TMP::TMPMessage& R09,UInt8 Status )
{
    odm_In Odr;
    TMP::TMPTime    RecvTime;
    TMP::TMPSymbol* symbol = (TMP::TMPSymbol*)R09.GetField("Symbol");

    memset( &Odr, 0, sizeof( odm_In ));
    R09[ "ExecType" ].ToBinary( (UInt8*)&Odr.header.ExecType );
    if( R09.GetMessageSeq() == 0 && Odr.header.ExecType != 'I' )
        throw( TMPRehandshakeException("Receive R09 without seq", 207));        
    if( FTMPOrderManager->IsOdrServerConnected() == FALSE )
        return OdrUtility::RejectR01( R09, 99 ); ///< Odr_server disconnected.
    ///< fill header.    
    Odr.header.msg_tag       = (uint8_t) mtIn;    
    Odr.header.source_fcm_id = FCMIndex;
    Odr.header.session_id    = SessionID;
    Odr.header.protocol      = 1; ///TMP
    Odr.header.tfx_grp_id    = 0;
    Odr.header.network_id    = 0;
    Odr.header.client_id     = FTMPOrderManager->GetProcessID();
    ///< fill ord_data_t    
    R09[ "cm_id" ].ToBinary( (UInt8*)&Odr.ord_data.cm_id );
    R09[ "fcm_id" ].ToBinary( (UInt8*)&Odr.ord_data.fcm_id );
    R09[ "order_no" ].ToBinary( (UInt8*)Odr.ord_data.order_no );    
    R09[ "investor_flag" ].ToBinary( (UInt8*)&Odr.ord_data.investor_flag );
    R09[ "ord_id" ].ToBinary( (UInt8*)&Odr.ord_data.order_id );
    R09[ "user_define" ].ToBinary( (UInt8*) Odr.ord_data.user_define );
    Odr.ord_data.Side = 1;
    Odr.ord_data.OrdType = 2;    
    R09[ "investor_acno" ].ToBinary( (UInt8*)&Odr.ord_data.investor_acno );    
    R09[ "TimeInForce" ].ToBinary( (UInt8*)&Odr.ord_data.TimeInForce );
    R09[ "PositionEffect" ].ToBinary( (UInt8*)&Odr.ord_data.PositionEffect );
    ///< fill quote fields.
    R09[ "BidPx" ].ToBinary( (UInt8*)&Odr.BidPx );
    R09[ "OfferPx" ].ToBinary( (UInt8*)&Odr.OfferPx );    
    R09[ "BidSize" ].ToBinary( (UInt8*)&Odr.BidSize );
    R09[ "OfferSize" ].ToBinary( (UInt8*)&Odr.OfferSize );
    ///< Fetch binary Symbol
    if( symbol->GetBinSymbol( Odr.ord_data.symbol.symbol_type,
                              Odr.ord_data.Side,
                              Odr.ord_data.symbol.pseq1, Odr.ord_data.symbol.leg_side[0],
                              Odr.ord_data.symbol.pseq2, Odr.ord_data.symbol.leg_side[1],
                              Odr.ord_data.symbol.comb_op ) == TRUE )
    {
        ///< Check order.
        if( RecvTime.DiffMS( R09.GetMessageTime()) > RejectOrderSec )
        {
            Odr.ord_data.status_code = 242; ///< Reject order
        }
        else///< Check Order
        {
            char ErrMsg[256];
            
            if( symbol->IsMultileg( ) == TRUE ) ///< Multileg order not support quote request.
    				{
        				UFC::BufferedLog::Printf( " Reject Quote, Order[%s] is a multileg symbol", R09[ "order_no" ].ToString().c_str() );
        				Odr.ord_data.status_code = 20; ///< Invalid symbol.
    				}
            else if( (Odr.ord_data.status_code = FNewOrderChecker.doCheck( Odr, ErrMsg )) != 0 )
            {
                UFC::BufferedLog::DebugPrintf( " Failed Code[%d][%s]", Odr.ord_data.status_code, ErrMsg );
                if( Odr.ord_data.status_code == 1 ) ///<Market closed.
                {
                    OdrUtility::RejectR01( R09, 1 );
                    return OdrUtility::RejectR01( R09, 1 );
                }
            }
            else
                Odr.ord_data.PositionEffect = '9';
            if( Status != 0 && Odr.ord_data.status_code == 0 ) ///< Overwrite the throughput warning error code.
                Odr.ord_data.status_code = Status;
        }
    }
    else
        Odr.ord_data.status_code = 20; ///< Invalid symbol.
    FTMPOrderManager->SendToOdrServer( &Odr );
    return NULL;
}
//----------------------------------------------------------------------------------------------------------------------
TMP::TMPMessage* TMPTransactionObject::RejectR07( TMP::TMPMessage& R07, UInt8 status )
{
    TMP::TMPMessage* TMPReply = new TMP::TMPMessage( TMP::tmtR08 );
    TMP::TMPMessage& R08 = *TMPReply;
        
    R08["order_no"]    = R07["order_no"].ToString();
    R08["order_id"]    = R07["order_id"].ToInteger();
    R08["fcm_id"]      = R07["fcm_id"].ToInteger();    
    R08["Symbol"]      = R07["Symbol"].ToString();
    R08["status_code"] = status; 
    return TMPReply;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL IsNum( char a )
{
    if( a >= '0' && a <='9' )
        return TRUE;
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL IsAlphaNum( char a )
{
    if( a >= '0' && a <= '9' )
        return TRUE;
    if( a >= 'a' && a <= 'z' )
         return TRUE;
    if( a >= 'A' && a <= 'Z' )
         return TRUE;
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL  TMPTransactionObject::ValidOrderID( const UFC::AnsiString& OID )
{
    if( IsAlphaNum( OID[0] ) == TRUE &&
        IsAlphaNum( OID[1] ) == TRUE &&
        IsAlphaNum( OID[2] ) == TRUE &&
        IsNum( OID[ 3 ] ) == TRUE &&
        IsNum( OID[ 4 ] ) == TRUE  )
        return TRUE;
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
TMP::TMPMessage* TMPTransactionObject::HandleR07( UInt16 /*FCMIndex*/, UInt16 /*SessionID*/, TMP::TMPMessage& R07 )
{
    if( R07.GetMessageSeq() != 0 )
        throw( TMPRehandshakeException("Receive R07 with seq",207));
    TMP::TMPSymbol* Symbol = (TMP::TMPSymbol*)R07.GetField( "Symbol" );
    UFC::AnsiString LineBrokerID = TMP::TMPMessage::GetBrokerID( R07.GetFCMID() );
    UFC::AnsiString BrokerID     = TMP::TMPMessage::GetBrokerID( R07["fcm_id"].ToInteger() );
    UFC::AnsiString SymbolStr;
    UInt8           Type,Leg1Side,Leg2Side,CombOP;
    UInt16          Leg1Seq,Leg2Seq;
    MTree           Data;    

    if(  BrokerID.AnsiNCompare( LineBrokerID.c_str() , 4 ) !=0 )
    {
        UFC::BufferedLog::Printf( " Reject Quote Request, BrokerID [%d] not exists",SymbolStr.c_str() );
        return RejectR07( R07, 12 ); ///<Wrong FCM number.
    }
    if( Symbol->IsBinSymbol() == TRUE )
    {
        Symbol->GetBinSymbol( Type, 1, Leg1Seq, Leg1Side,Leg2Seq,Leg2Side,CombOP );    
        if( TMP::TMPMessage::SymbolExists( Leg1Seq ) == FALSE)
            return RejectR07( R07, 20 ); ///< Symbol not exists.
        Symbol->GetStringSymbol( Type, SymbolStr );        
    }
    else
    {
       Symbol->GetStringSymbol( Type, SymbolStr );
       Leg1Seq = TMP::TMPMessage::GetSymbol( SymbolStr );
       if( Leg1Seq == 0 )
       {
           UFC::BufferedLog::Printf( " Reject Quote Request, Symbol [%s] not exists",SymbolStr.c_str() );
           return RejectR07( R07, 20 ); ///< Symbol not exists.
       }
    }
    if( Symbol->IsMultileg( ) == TRUE ) ///< Multileg order not support quote request.
    {
        UFC::BufferedLog::Printf( " Reject Quote Request because [%s] is a multileg symbol",SymbolStr.c_str() );
        return RejectR07( R07, 20 );
    }
    if( FNewOrderChecker.isProductCloseTime( Leg1Seq ) == TRUE )
        return RejectR07( R07, 1 ); ///< Market closed.
    if( FNewOrderChecker.isProductOpenTime( Leg1Seq ) == FALSE )
        return RejectR07( R07, 23 ); ///< Market not opened.
    if( ValidOrderID( R07["order_no"].ToString() ) == FALSE )
        return RejectR07( R07, 18 );  ///< Invalidate OrderID
    Data.append( "Commodity_ID", SymbolStr.c_str() );
    Data.append( "order_no", R07["order_no"].ToString().c_str() );
    Data.append( "ord_id", (UInt32)R07["order_id"].ToInteger() );
    Data.append( "symbol_type", (UInt32)Type );
    Data.append( "source_fcm_id",(UInt32) R07.GetFCMID() );
    Data.append( "session_id",(UInt32) R07.GetSessionID() );
    Data.append( "client_id",(UInt32)PortNo%1000 );
    Data.append( "fcm_id",(UInt32)R07["fcm_id"].ToInteger() );
    Data.append( "pseq",(UInt32) Leg1Seq );    
    if( MexObj != NULL )
        MexObj->Send( TFX_QUOTE_REQUEST_SUBJECT, Data );
    return NULL;
}
//----------------------------------------------------------------------------------------------------------------------        
TMP::TMPMessage* TMPTransactionObject::HandleR11( UInt16 FCMIndex, UInt16 SessionID, TMP::TMPMessage& R11 )
{
    if( R11.GetMessageSeq() != 0 )
        throw( TMPRehandshakeException("Receive R11 with seq",207));
    TMP::TMPMessage* TMPReply = new TMP::TMPMessage( TMP::tmtR12 );
    TMP::TMPMessage& R12      = *TMPReply;    
    UInt8            Type     =  R11["SubscriptionRequestType"].ToInteger();
    UInt8            Group    =  R11["flow_group_no"].ToInteger();    
    UInt16           ReqID    =  R11["TradeReqID"].ToInteger();
    Int16            TradeStatus;

    if( Type == 1 ) ///< Subscribe
        TradeStatus = FTMPOrderManager->SubscribeTradeSessionStatus( FCMIndex, SessionID, Group, ReqID );    
    else if( Type == 2 ) ///< Unsubscription.
        TradeStatus = FTMPOrderManager->UnsubscribeTradeSessionStatus( FCMIndex, SessionID, Group, ReqID );        
    else if( Type == 0 )///< Ask status.
        TradeStatus = FTMPOrderManager->AskTradeSessionStatus( FCMIndex, SessionID, Group );
    else ///< Error
        TradeStatus = -2;

    if( TradeStatus == -1 )
    {
        R12["TradeStatus"] = 0;
        R12["status_code"] = 209; ///< Wrong group
    } 
    else if( TradeStatus == -2 )
    {
        R12["TradeStatus"] = 0;            
        R12["status_code"] = 210; ///< unknown SubscriptionRequestType.
    } 
    else
    {
        R12["TradeStatus"] = (UInt8)TradeStatus;
        R12["status_code"] = 0; ///< OK        
    }
    R12[ "TradeReqID"    ] = ReqID;
    R12[ "flow_group_no" ] = Group;
    return TMPReply;
}
//----------------------------------------------------------------------------------------------------------------------
TMP::TMPMessage* TMPTransactionObject::RejectR13( TMP::TMPMessage& R13, UInt8 status )
{
    TMP::TMPMessage* TMPReply = new TMP::TMPMessage( TMP::tmtR14 );
    TMP::TMPMessage& R14 = *TMPReply;

    R14[ "status_code"  ] = status;
    R14[ "fcm_req_id"   ] = R13["fcm_req_id"].ToInteger();
    R14[ "bulletin_time"] = TMP::TMPTime( );
    R14[ "system_type"  ] = R13["system_type"].ToInteger();
    R14[ "data"         ] = R13["data"].ToString();
    return TMPReply;
}
//----------------------------------------------------------------------------------------------------------------------
TMP::TMPMessage* TMPTransactionObject::HandleR13( UInt16 FCMIndex, UInt16 SessionID, TMP::TMPMessage& R13 )
{
    TMPOrderConnectionObject* Session;
    if( R13.GetMessageSeq() != 0 )
        throw( TMPRehandshakeException("Receive R13 with seq",207));

    if( (Session = static_cast<TMPOrderConnectionObject*>(FTMPOrderManager->GetSession(FCMIndex, SessionID)))!= NULL )
    {
        if( Session->CanProcessR12( ) == TRUE )
        {
            MTree Data;
    
            Data.append( "SOURCE_ID",     TMP::TMPMessage::GetBrokerID( R13.GetFCMID() ).c_str() );
            Data.append( "SESSION_ID",    (Int32)R13.GetSessionID() );
            Data.append( "TRADE_KIND",    (Int32)TMP::TMPMessage::GetTradeSession() );
            Data.append( "PROTOCOL_TYPE", (Int32)1 );
            Data.append( "FCM_REQ_ID",    (UInt32) R13["fcm_req_id"].ToInteger() );
            Data.append( "BODY",           R13["data"].ToString().c_str() );
            if( MexObj != NULL )
                MexObj->Send( TFX_FT_REQUEST_SUBJECT.c_str(), UFC::Hostname, Data );
            else
                RejectR13( R13, 0 ); ///< Reply OK if Mex object not exists.( for Testing )
        }
        else
            return RejectR13( R13, 121 );///< illegal time to get file.
    }
    return NULL;    
}
//----------------------------------------------------------------------------------------------------------------------
