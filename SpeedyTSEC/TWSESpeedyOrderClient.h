//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __FCMCLIENT_H
#define __FCMCLIENT_H
#include "../UFC/UFC.h"
#include "../UFC/iniFile.h"
#include "../TSECLib/Messages.h"
#include "fix44/ExecutionReport.h"
#include "fix44/BusinessMessageReject.h"
#include "fix44/OrderCancelReject.h"
#include "fix44/NewOrderSingle.h"
#include "fix44/OrderCancelRequest.h"
#include "fix44/OrderCancelReplaceRequest.h"
#include "fix44/OrderStatusRequest.h"
#include "Session.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define PVC_BUFFER_SIZE 256
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MessageHeader;
class LinkSubSystemFCM;
class LogFile;
class ConnectionParameter;
class ConnectionListener;
class ConnectionObjectBase;
class MarketSchedule;
class OrderInfo;
class SpeedyObjectsManager;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class OrderListener // Abstract class
{
public:
    // stage control
    virtual void  OnReconnect() = 0;
    virtual void  OnOpenMarket() = 0;
    virtual void  OnCloseMarket() = 0;
    virtual void  OnIdle() = 0;
    virtual void  Handshake( BOOL ReceiveL010 = FALSE ) = 0;
    
    virtual BOOL  IsReady( void ) = 0;
    virtual BOOL  IsFIX( void ) { return FALSE; }
    virtual void  SetTradeSession( char APCode, Int32 SubSystem ) = 0;
    virtual ConnectionObjectBase* GetOwner( void ) = 0;
    // Message handler    
    virtual MessageHeader* OnHandleReplyMessage( TMessageType& Type , BOOL& NeedLog, BOOL& IsCancelDel , UFC::AnsiString& ExecKey, UFC::AnsiString& OrigKey, UFC::AnsiString& BrokerID ) = 0;
    virtual BOOL  OnOrder( MessageHeader* CtlHeader, OrderRecord& OI  ) = 0;    
    virtual Int32 GetStatus() = 0;
    virtual void  SetStatus( Int32 ) = 0;
    virtual void  StopHandshake() = 0;
    virtual void  Listen( BOOL ) = 0;
    virtual void  Open() = 0;
    virtual void  Close() = 0; 
    //< For FIX
    virtual int   GetSequence( bool IsIn ) { return 0; }
    virtual void  SetSequence( int InSeq, int OutSeq ){}
    virtual void  OnLogon( void ){}
    virtual void  OnLogoff( void ){}
    virtual void  onMessage( const FIX44::ExecutionReport& ) {}
    virtual void  onMessage( const FIX44::OrderCancelReject& ) {}
    virtual void  onMessage( const FIX44::BusinessMessageReject& ){}
    virtual void  onMessage( BOOL IsSend, const FIX::Message& ){}
    virtual FIX::Session* GetFIXSession() { return NULL; } 
    ///< Destructor
    virtual ~OrderListener( void ){}    
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SpeedyOrderClient :public OrderListener
{
private:
        LinkSubSystemFCM*     FX25OrderLine;
        LogFile* 	      FLogFile;                
        BOOL                  FIsConnected;
        ConnectionListener*   FConnectionListener;
        ConnectionObjectBase* FOwner;
        char                  FPVCID[ 3 ];
        int                   FSubSystem;
        int 		      FOrderCount;        
        SpeedyObjectsManager* FObjectManager; 
        BOOL                  FIsTWSE;
public:
        SpeedyOrderClient( SpeedyObjectsManager* ObjManager,
                           ConnectionParameter&  Param,
                           ConnectionListener*   Listener,
                           Int32                 SubSystem,
                           ConnectionObjectBase* Owner,
                           BOOL                  IsTWSE);
        virtual ~SpeedyOrderClient();
        MarketSchedule* GetMarket( void );
public:
        // OrderListener interface
        void  OnReconnect();    ///< Reconnect to TAIFEX.
        void  OnOpenMarket();   ///< Call this function when open market.
        void  OnCloseMarket(){}	///< Call this function when close market.
        void  OnIdle();	        ///< Call this function to perform idle handshake.
        void  SetTradeSession(  char APCode, Int32 SubSystem );
        ConnectionObjectBase* GetOwner( void ) { return FOwner; }
        void  Handshake( BOOL ReceiveL010 = FALSE );
        BOOL  IsReady();        
public:
        BOOL  OnOrder( MessageHeader* CtlHeader, OrderRecord& OI  );
        MessageHeader* OnHandleReplyMessage( TMessageType& Type , BOOL& NeedLog,BOOL& IsCancelDel, UFC::AnsiString& ExecKey, UFC::AnsiString& OrigKey, UFC::AnsiString& BrokerID );
        Int32 GetStatus();
        void SetStatus( Int32 );
        void StopHandshake();
        void Listen( BOOL );
        void Open();
        void Close();
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TWSEOrder
{
public:
   char       OrderNO[ 6 ]; 
   char       Account [ 8 ]; 
   char       Symbol[ 7 ]; 
   char       BrokerID[ 5 ];
   char       InvestorFlag[2];
   char       OrderType[2];
   char       ExchangeCode[2];   
   char       TradingSession[9];
   double     Price;    
   Int64      OrderQty;
   char       Side;
   char       TimeInforce;
   char       PriceType;
   char       TargetSubID[ 2 ];
public:   
   char       RefOrderID[3];   
public:
   TWSEOrder(){}
   void FromTMP( MessageT010*  NormalOrder, const UFC::AnsiString& PVCID );
   void FromTMP( MessageO010*  OddOrder, const UFC::AnsiString& PVCID );
   void FromTMP( MessageO110*  OddOrder, const UFC::AnsiString& PVCID );   
   void FromTMP( MessageP010*  PostOrder, const UFC::AnsiString& PVCID );
   void FromTMP( MessageV010*  LendOrder, const UFC::AnsiString& PVCID );
   void FromTMP( MessageA010*  AuctionOrder, const UFC::AnsiString& PVCID );
   void FromTMP( MessageE010*  TenderOrder, const UFC::AnsiString& PVCID );
   void FromTMP( MessageEx010* TenderExOrder, const UFC::AnsiString& PVCID );
   static char AccountFlagTMP2FIX( char );   
   static char AccountFlagFIX2TMP( char );   
public:   
   static BOOL IsInit;
public:   
   static char TMP2FIX[ 256 ];
   static char FIX2TMP[ 256 ];
   static void Init( void );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SpeedyOrderConnectionObject;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SpeedyFIXOrderClient :public OrderListener
{
private:  
        FIX::Session*         FSession; 
        ConnectionListener*   FConnectionListener;
        SpeedyOrderConnectionObject* FOwner;        
        ConnectionObjectBase* FListener;
        int                   FSubSystem;
        int 		      FOrderCount;        
        int                   FStatus;
        UFC::AnsiString       FSenderCompID;
        UFC::AnsiString       FTargetCompID;        
        UFC::AnsiString       FTargetSubID;
        UFC::AnsiString       FPVCID;
        UFC::AnsiString       FLineBrokerID;        
        UFC::AnsiString       FTag49;        
        UFC::AnsiString       FTag56;        
        UFC::AnsiString       FTag10004;
        char                  FLinkPVCSendStr[ 32 ];
        char                  FLinkPVCRecvStr[ 32 ];
        char*                 FLogBuffer; 
        BOOL                  FEnabled;
        int                   FPort;
        SpeedyObjectsManager* FObjectManager; 
        BOOL                  FIsTWSE;
        int                   FProfileID;
        char                  FFIXRejStaleOrd;  
        const char* GetTargetSubID( void );
        BOOL ProcessFIXRejectMsg( const FIX44::ExecutionReport& Report, MessageHeader* Reply, UFC::AnsiString& Msg  );
        BOOL ProcessFIXErrorMsg( const FIX44::ExecutionReport& Report, MessageHeader* Reply, UFC::AnsiString& Msg );
        void SendFillMessage( char* Buffer, const char* TSECTime, const char* LineTime, Int32 OrigNID, const char* Key, BOOL IsSpeedy );
        void FIXFill( const FIX44::ExecutionReport& Report, OrderInfo* OI );
        void FIXConfirmNormal( const FIX44::ExecutionReport& Report, OrderInfo* OI );
        void FIXConfirmOdd( const FIX44::ExecutionReport& Report, OrderInfo* OI );
        void FIXConfirmOddEx( const FIX44::ExecutionReport& Report, OrderInfo* OI );        
        void FIXConfirmPost( const FIX44::ExecutionReport& Report, OrderInfo* OI );
        void FIXConfirmLend( const FIX44::ExecutionReport& Report, OrderInfo* OI );
        void FIXConfirmAuction( const FIX44::ExecutionReport& Report, OrderInfo* OI );
        void FIXConfirmTender( const FIX44::ExecutionReport& Report, OrderInfo* OI );
        void FIXConfirmTenderEx( const FIX44::ExecutionReport& Report, OrderInfo* OI );        
        OrderRecord* GetSMPOrderRecord( OrderInfo* OI, MessageHeader* Msg  ); 
        ///< for fill, add filled position
        void CheckPositionFilled( FIX::ClOrdID& ClOrdID, FIX::Symbol& Symbol, FIX::OrderID& OID, int Qty, bool IsOdd );                
        ///< for New sell order.
        void CheckPartiallyConfirmPositionRelease( FIX::ClOrdID& ClOrdID, FIX::OrderID& OrderID, FIX::Symbol& Symbol, FIX::OrderQty& Qty, char TwseOrdType, bool IsNorman );
        void CheckPositionRelease( FIX::ClOrdID& ClOrdID, FIX::Symbol& Symbol, FIX::OrderID& OrderID, FIX::OrderQty& Qty, char TwseOrdType, bool IsOdd, bool IsPost );
        ///< for New buy order.
        void CheckPartiallyConfirmFundRelease( FIX::Account& Account, FIX::OrderID& OrderID, FIX::Symbol& Symbol, FIX::OrderQty& Qty, FIX::Price& Price );
        void CheckFundRelease( FIX::Account& Account, FIX::Symbol& Symbol, FIX::OrderID& OrderID, FIX::Price& Px, FIX::OrderQty& Qty, bool IsOdd );        
public:    
        typedef bool (SpeedyFIXOrderClient::*FastNewOrderFunc)( MessageT010* NormalOrder, OrderRecord& OI  );                
        FastNewOrderFunc  FFastNewOrder;
private:          
        FIX44::NewOrderSingle*            FNewOrder;              
        FIX44::OrderCancelReplaceRequest* FOrderReplace;
        FIX44::OrderCancelRequest*        FOrderCancel;
        FIX44::OrderStatusRequest*        FOrderStatus;
        void InitFIXMessages( void );
        OrderInfo* AddNewOrderInfo( const UFC::AnsiString& OrderNO, const UFC::AnsiString& BrokerID, int TSessoin, const FIX44::ExecutionReport& Report );
public:
        SpeedyFIXOrderClient( SpeedyObjectsManager* ObjManager,
                              ConnectionParameter&  Param,
                              ConnectionListener*   Listener,
                              Int32                 SubSystem,
                              SpeedyOrderConnectionObject* Owner,
                              BOOL                  IsTWSE );
        virtual ~SpeedyFIXOrderClient();
        MarketSchedule* GetMarket( void );
        bool          FastNewOrder( MessageT010* NormalOrder, OrderRecord& OI  );
        bool          FastNewOrderBenchmark( MessageT010* NormalOrder, OrderRecord& OI  );        
        bool          SendNewOrderSingle( MessageT010* OrderMessage, OrderRecord& OI, Int64& );
        bool          SendNewOrderSingle( MessageT010* OrderMessage, OrderRecord& OI );        
        FIX::Message* CreateFIXMessage( int Func, TWSEOrder& Order, OrderRecord& OI );   
        FIX::Message* CreateNewOrderSingle( TWSEOrder& Order, OrderRecord& OI );
        FIX::Message* CreateOrderCancelReplace( TWSEOrder& ReduceMessage, OrderRecord& OI );
        FIX::Message* CreateOrderCancel( TWSEOrder& CancelMessage, OrderRecord& OI );
        FIX::Message* CreateOrderStatus( TWSEOrder& StatusMessage, OrderRecord& OI );
        FIX::Session* GetFIXSession() { return FSession; } 
public:
        // OrderListener interface
        void  OnReconnect();    ///< Reconnect to TAIFEX.
        void  OnOpenMarket();   ///< Call this function when open market.
        void  OnCloseMarket(){}	///< Call this function when close market.        
        void  OnIdle();	        ///< Call this function to perform idle handshake.
        void  SetTradeSession(  char APCode, Int32 SubSystem );
        void  Handshake( BOOL ReceiveL010 = FALSE );
        ConnectionObjectBase* GetOwner( void );
        BOOL  IsReady();
        BOOL  IsFIX( void ) { return TRUE; }
        void  OnLogon( void );
        void  OnLogoff( void );
public:        
        BOOL  OnOrder( MessageHeader* CtlHeader, OrderRecord& OI );
        void  onMessage( const FIX44::ExecutionReport& );
        void  onMessage( const FIX44::OrderCancelReject& );
        void  onMessage( const FIX44::BusinessMessageReject& );
        void  onMessage( BOOL IsSend, const FIX::Message& );
        void  SetSequence( int InSeq, int OutSeq );
        int   GetSequence( bool IsIn );
        MessageHeader* OnHandleReplyMessage( TMessageType& Type , BOOL& NeedLog,BOOL& IsCancelDel, UFC::AnsiString& ExecKey, UFC::AnsiString& OrigKey, UFC::AnsiString& BrokerID );
        Int32 GetStatus();
        void SetStatus( Int32 );
        void StopHandshake(){}
        void Listen( BOOL );
        void Open();
        void Close();
};
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

