//----------------------------------------------------------------------------------------------------------------------
#ifndef __TransactionObject_h
#define __TransactionObject_h
//----------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../TMP34/MexObject.h"
#include "Messages.h"
#include "TMPMessage.h"
//----------------------------------------------------------------------------------------------------------------------
class ConnectionObjectBase;
//----------------------------------------------------------------------------------------------------------------------
const int OPEN_SWITCH_CHANGED  = 1000;
//----------------------------------------------------------------------------------------------------------------------
class TransactionObject
{
public:
    virtual MessageHeader* OnMessage( ConnectionObjectBase* Sender ,MessageHeader* InMsg)=0;
    virtual TMP::TMPMessage* OnMessage( ConnectionObjectBase* ,TMP::TMPMessage* , UInt8 ) { return NULL; }
    virtual void OnTradingStatusChange( UInt8 Group, UInt8 Openswitch ) = 0;
    virtual void OnEvent(int Type, void* Data)=0;
};
//----------------------------------------------------------------------------------------------------------------------
class TMPOrderManager;
//----------------------------------------------------------------------------------------------------------------------
#include "../../inc/OrderChecker.h"
//----------------------------------------------------------------------------------------------------------------------
const int R01_ExecType       = 0;
const int R01_cm_id          = 1;
const int R01_fcm_id         = 2;
const int R01_order_no       = 3;
const int R01_ord_id         = 4;
const int R01_user_define    = 5;
const int R01_Symbol         = 6;
const int R01_Price          = 7;
const int R01_qty            = 8;
const int R01_investor_acno  = 9;
const int R01_investor_flag  = 10;
const int R01_Side           = 11;
const int R01_OrdType        = 12;
const int R01_TimeInForce    = 13;
const int R01_PositionEffect = 14;

/*const int R01_ExecType]
const int R01_cm_id]
const int R01_fcm_id]
const int R01_order_no]
const int R01_ord_id]
const int R01_user_define]
const int R01_Symbol]
const int R01_Price]
const int R01_qty]
const int R01_investor_acno]
const int R01_investor_flag]
const int R01_Side]
const int R01_OrdType]
const int R01_TimeInForce]
const int R01_PositionEffect]
*/
//----------------------------------------------------------------------------------------------------------------------
class TMPTransactionObject : public TransactionObject 
{
private:
    TMPOrderManager*        FTMPOrderManager;
    nsOrder::COrderChecker  FNewOrderChecker;
private:
    BOOL             ValidOrderID( const UFC::AnsiString& OID );
    TMP::TMPMessage* HandleR01( UInt16 FCMIndex, UInt16 SessionID, TMP::TMPMessage& R01, UInt8 Status  );
    TMP::TMPMessage* HandleR09( UInt16 FCMIndex, UInt16 SessionID, TMP::TMPMessage& R09, UInt8 Status  );
    TMP::TMPMessage* RejectR07( TMP::TMPMessage& R07, UInt8 status );
    TMP::TMPMessage* RejectR13( TMP::TMPMessage& R13, UInt8 status );
    TMP::TMPMessage* HandleR07( UInt16 FCMIndex, UInt16 SessionID, TMP::TMPMessage& R07 );
    TMP::TMPMessage* HandleR11( UInt16 FCMIndex, UInt16 SessionID, TMP::TMPMessage& R11 );
    TMP::TMPMessage* HandleR13( UInt16 FCMIndex, UInt16 SessionID, TMP::TMPMessage& R13 );
public:
    TMPTransactionObject( TMPOrderManager* , const UFC::AnsiString& DBFileName );
    virtual ~TMPTransactionObject( ) {}
    virtual MessageHeader* OnMessage( ConnectionObjectBase*  ,MessageHeader* ){ return NULL; }
    virtual TMP::TMPMessage* OnMessage( ConnectionObjectBase* Sender ,TMP::TMPMessage* InMsg, UInt8 Status );
    virtual void OnTradingStatusChange( UInt8 Group, UInt8 Openswitch );
    virtual void OnEvent(int Type, void* Data);
public:       
    void Report( ) {} 
};    
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
