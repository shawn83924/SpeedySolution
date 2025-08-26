//----------------------------------------------------------------------------------------------------------------------
#ifndef __TransactionObject_h
#define __TransactionObject_h
//----------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "Messages.h"
//----------------------------------------------------------------------------------------------------------------------
extern UFC::PStringList  Executions;
extern UFC::PEvent       ExecutionsEvent;
//----------------------------------------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
class ConnectionObjectBase;   
class TSECOrderManager;
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
    int MaxOrderResponseTime;      ///< in ms
    int OrderAverageResponseTime;  ///< in ms
    int CurrentOrderCount;	
	
} OrderStates;
//----------------------------------------------------------------------------------------------------------------------
class OrderInfo
{
public:
    static UFC::PHashMap< UFC::AnsiString, int* >   FStockLastPxs;    
public:
    TradeSession    FSession;
    UFC::AnsiString FOID;
    UFC::AnsiString FSymbol;
    UFC::AnsiString FBrokerID;
    UFC::AnsiString FAccount; 
    UFC::AnsiString FObjectBroker;///< For Odd trade
    char            FInvestorFlag;
    char            FSide;
    int             FPrice;
    int             FQty; 
    int             FFillQty;
    int             FReduceQty;    
    char            FExchangeCode;
    char            FOrderType;       
    char            FTradeKind;///< For Odd trade       
    int             FStkSeqNo; ///< For Tender trade
    BOOL            FKeep;     ///< For Lend trade
public:             ///< for New Version
    char            FTimeInforce; ///< 4:FOK 3:IOC 0:ROD
    char            FOrdType;     ///< 1:Market 2:Limit 
public:
    UFC::AnsiString FPVCID;
    UFC::AnsiString FLineBrokerID;
    BOOL            FIsFIX;
public:
    int     FCountDown;
    int     FFilledQty;
    double  FFilledPx;
    int     CountDown( void );
    BOOL    IsSetMatchRule( void );
    void    SetMatchRule( int Sec, int Qty, double Px );
private:
    void FillT020( MessageT020* T020, int Qty, int Before,int After, const UFC::AnsiString& PVCID );
    void FillP020( MessageP020* P020, int Qty, int Before,int After, const UFC::AnsiString& PVCID );
    void FillO020( MessageO020* O020, int Qty, int Before,int After, const UFC::AnsiString& PVCID );
    void FillO120( MessageO120* O120, int Qty, int Before,int After, const UFC::AnsiString& PVCID );
    void FillA020( MessageA020* A020, int Qty, int Before,int After, const UFC::AnsiString& PVCID );
    void FillE020( MessageE020* E020, int Qty, int Before,int After, const UFC::AnsiString& PVCID );
    void FillEx020( MessageEx020* Ex020, int Qty, int Before,int After, const UFC::AnsiString& PVCID );
    void FillV020( MessageV020* V020, int Qty, int Before,int After, const UFC::AnsiString& PVCID );    
    void FillR3Body( R3Body* R3, int cQty, int Seq, int TotalSeq );
public:
    OrderInfo( const UFC::AnsiString& OID, MessageT010*  Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX );
    OrderInfo( const UFC::AnsiString& OID, MessageP010*  Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX );
    OrderInfo( const UFC::AnsiString& OID, MessageO010*  Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX );    
    OrderInfo( const UFC::AnsiString& OID, MessageO110*  Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX );    
    OrderInfo( const UFC::AnsiString& OID, MessageA010*  Order );
    OrderInfo( const UFC::AnsiString& OID, MessageE010*  Order );
    OrderInfo( const UFC::AnsiString& OID, MessageEx010* Order );
    OrderInfo( const UFC::AnsiString& OID, MessageV010*  Order );    
    
    
    MessageT020*  ConfirmT020(  const UFC::AnsiString& PVCID );
    MessageP020*  ConfirmP020(  const UFC::AnsiString& PVCID );
    MessageO020*  ConfirmO020(  const UFC::AnsiString& PVCID );
    MessageO120*  ConfirmO120(  const UFC::AnsiString& PVCID );
    MessageA020*  ConfirmA020(  const UFC::AnsiString& PVCID );
    MessageE020*  ConfirmE020(  const UFC::AnsiString& PVCID );
    MessageEx020* ConfirmEx020(  const UFC::AnsiString& PVCID );
    MessageV020*  ConfirmV020(  const UFC::AnsiString& PVCID );    
    
    MessageT020*  CancelT020(  const UFC::AnsiString& PVCID );
    MessageP020*  CancelP020(  const UFC::AnsiString& PVCID );
    MessageO020*  CancelO020(  const UFC::AnsiString& PVCID );
    MessageO120*  CancelO120(  const UFC::AnsiString& PVCID );
    MessageA020*  CancelA020(  const UFC::AnsiString& PVCID );
    MessageE020*  CancelE020(  const UFC::AnsiString& PVCID );
    MessageEx020* CancelEx020(  const UFC::AnsiString& PVCID );
    MessageV020*  CancelV020(  const UFC::AnsiString& PVCID );    
    
    MessageT020*  ReplaceT020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled );
    MessageT020*  ReplaceT020(  const UFC::AnsiString& PVCID, int Px );
    MessageP020*  ReplaceP020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled );
    MessageO020*  ReplaceO020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled );
    MessageO120*  ReplaceO120(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled );    
    MessageA020*  ReplaceA020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled );
    MessageE020*  ReplaceE020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled );
    MessageEx020* ReplaceEx020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled );
    MessageV020*  ReplaceV020(  const UFC::AnsiString& PVCID, int Qty, BOOL& Canceled );    
    
    MessageT020*  QueryT020(  const UFC::AnsiString& PVCID );
    MessageP020*  QueryP020(  const UFC::AnsiString& PVCID );
    MessageO020*  QueryO020(  const UFC::AnsiString& PVCID );
    MessageO120*  QueryO120(  const UFC::AnsiString& PVCID );
    MessageA020*  QueryA020(  const UFC::AnsiString& PVCID );
    MessageE020*  QueryE020(  const UFC::AnsiString& PVCID );
    MessageEx020* QueryEx020(  const UFC::AnsiString& PVCID );
    MessageV020*  QueryV020(  const UFC::AnsiString& PVCID );        
    
    void Fill( char* FillStr, int Seq, int TotalSeq );
    void FillByRule( char* FillStr, int Seq, int TotalSeq, BOOL& Canceled );
    void PartialFill( char* FillStr, int Seq, int TotalSeq, BOOL& Canceled );    
    void PartialFill( char* FillStr, int Seq, int TotalSeq,int Qty, BOOL& Canceled );
    const char* GetFIXSenderSubID( void );
    
};
//----------------------------------------------------------------------------------------------------------------------
class StockInfo
{
public:
    int TPrice;
    int BPrice;
    int CPrice;
    StockInfo( int TPx, int BPx, int CPx )
    :TPrice(TPx),
     BPrice(BPx),
     CPrice(CPx){}
};
//----------------------------------------------------------------------------------------------------------------------
class  TransactionObject
{
private:    
    UFC::PCriticalSection                             FCS;
    UFC::PCriticalSection                             FLastConfirmCS;
    UFC::PtrList< OrderInfo >                         FOrderBook;    
    UFC::PHashedSet< UFC::AnsiString >                FOIDs;
    UFC::PHashedList< UFC::AnsiString, StockInfo* >   FStocks;
    UFC::PHashedList< UFC::AnsiString, OrderInfo* >   FNormalOrders;
    UFC::PHashedList< UFC::AnsiString, OrderInfo* >   FOddOrders;
    UFC::PHashedList< UFC::AnsiString, OrderInfo* >   FIntradayOddOrders;
    UFC::PHashedList< UFC::AnsiString, OrderInfo* >   FPostOrders;
    UFC::PHashedList< UFC::AnsiString, OrderInfo* >   FLendOrders;    
    UFC::PHashedList< UFC::AnsiString, OrderInfo* >   FTenderOrders;
    UFC::PHashedList< UFC::AnsiString, OrderInfo* >   FTenderExOrders;    
    UFC::PHashedList< UFC::AnsiString, OrderInfo* >   FAuctionOrders;
    UFC::PHashedList< UFC::AnsiString, char* >        FLastConfirm;
    int                                               FOpenSwitch;
    int                                               FMatchedOrder;    
    int                                               FFillSeq;
    int                                               FOrderCount;
    int                                               FTotalResponseTime;
    int                                               FMaxResponseTime;
    BOOL                                              FCheckPrice;
    TSECOrderManager*                                 FManager;
private:    
    MessageHeader* NewOrder( MessageT010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX  );
    MessageHeader* NewOrder( MessageP010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX  );
    MessageHeader* NewOrder( MessageO010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX  );
    MessageHeader* NewOrder( MessageO110* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX  );    
    MessageHeader* NewOrder( MessageA010* Order, const UFC::AnsiString& PVCID );
    MessageHeader* NewOrder( MessageE010* Order, const UFC::AnsiString& PVCID );
    MessageHeader* NewOrder( MessageEx010* Order, const UFC::AnsiString& PVCID );    
    MessageHeader* NewOrder( MessageV010* Order, const UFC::AnsiString& PVCID );    
    
    MessageHeader* CancelOrder( MessageT010* Order, const UFC::AnsiString& PVCID, BOOL IsFIX );
    MessageHeader* CancelOrder( MessageP010* Order, const UFC::AnsiString& PVCID, BOOL IsFIX );
    MessageHeader* CancelOrder( MessageO010* Order, const UFC::AnsiString& PVCID, BOOL IsFIX );
    MessageHeader* CancelOrder( MessageO110* Order, const UFC::AnsiString& PVCID, BOOL IsFIX );    
    MessageHeader* CancelOrder( MessageA010* Order, const UFC::AnsiString& PVCID );
    MessageHeader* CancelOrder( MessageE010* Order, const UFC::AnsiString& PVCID );
    MessageHeader* CancelOrder( MessageEx010* Order, const UFC::AnsiString& PVCID );
    MessageHeader* CancelOrder( MessageV010* Order, const UFC::AnsiString& PVCID );    
    
    MessageHeader* ReplaceOrder( MessageT010* Order, const UFC::AnsiString& PVCID, BOOL IsFIX );
    MessageHeader* ReplaceOrder( MessageP010* Order, const UFC::AnsiString& PVCID, BOOL IsFIX );
    MessageHeader* ReplaceOrder( MessageO010* Order, const UFC::AnsiString& PVCID, BOOL IsFIX );
    MessageHeader* ReplaceOrder( MessageO110* Order, const UFC::AnsiString& PVCID, BOOL IsFIX );    
    MessageHeader* ReplaceOrder( MessageA010* Order, const UFC::AnsiString& PVCID );
    MessageHeader* ReplaceOrder( MessageE010* Order, const UFC::AnsiString& PVCID );
    MessageHeader* ReplaceOrder( MessageEx010* Order, const UFC::AnsiString& PVCID );    
    MessageHeader* ReplaceOrder( MessageV010* Order, const UFC::AnsiString& PVCID );    
    
    MessageHeader* QueryOrder( MessageT010* Order, const UFC::AnsiString& PVCID ); ///< In T010, out T020,T030
    MessageHeader* QueryOrder( MessageP010* Order, const UFC::AnsiString& PVCID ); ///< In T010, out T020,T030
    MessageHeader* QueryOrder( MessageO010* Order, const UFC::AnsiString& PVCID ); ///< In T010, out T020,T030
    MessageHeader* QueryOrder( MessageO110* Order, const UFC::AnsiString& PVCID ); ///< In T010, out T020,T030
    MessageHeader* QueryOrder( MessageA010* Order, const UFC::AnsiString& PVCID ); ///< In T010, out T020,T030
    MessageHeader* QueryOrder( MessageE010* Order, const UFC::AnsiString& PVCID ); ///< In T010, out T020,T030
    MessageHeader* QueryOrder( MessageEx010* Order, const UFC::AnsiString& PVCID ); ///< In T010, out T020,T030    
    MessageHeader* QueryOrder( MessageV010* Order, const UFC::AnsiString& PVCID ); ///< In T010, out T020,T030    
    
    MessageHeader* CheckMarketState( TradeSession Session, int Func );
    MessageHeader* ReconnectQuery( TradeSession, const UFC::AnsiString& PVCID );
    MessageHeader* GetLastConfirm( const UFC::AnsiString& PVCID );
    
    MessageHeader* HandleNormalOrder( MessageT010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX );
    MessageHeader* HandleOddOrder( MessageO010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX );
    MessageHeader* HandlePostOrder( MessageP010* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX );
    MessageHeader* HandleIntradayOddOrder( MessageO110* Order, const UFC::AnsiString& PVCID, const UFC::AnsiString& BrkID, BOOL IsFIX );
    MessageHeader* HandleAuctionOrder( MessageA010* Order, const UFC::AnsiString& PVCID );
    MessageHeader* HandleTenderOrder( MessageE010* Order, const UFC::AnsiString& PVCID );
    MessageHeader* HandleTenderExOrder( MessageEx010* Order, const UFC::AnsiString& PVCID );
    MessageHeader* HandleLendOrder( MessageV010* Order, const UFC::AnsiString& PVCID );    
    
    
    MessageHeader* NormalReject( OrderInfo* OrderRecord, const UFC::AnsiString& PVCID, int Func, int Error, BOOL IsFIX );
    MessageHeader* OddReject( OrderInfo* OrderRecord, const UFC::AnsiString& PVCID, int Func, int Error, BOOL IsFIX );
    MessageHeader* PostReject( OrderInfo* OrderRecord, const UFC::AnsiString& PVCID, int Func, int Error, BOOL IsFIX );
    MessageHeader* IntradayOddReject( OrderInfo* OrderRecord, const UFC::AnsiString& PVCID, int Func, int Error, BOOL IsFIX );
    void           DeleteOrder( OrderInfo* Order, BOOL DelOrderBook = TRUE );
    void           AddToMatchList( OrderInfo* Order );
    void           DeleteFromMatchList( OrderInfo* Order);
    void           UpdateStates( UInt32 Tick );
    int            CheckPx( const UFC::AnsiString& Symbol, int Px );
    MessageHeader* Reject( MessageHeader* RejectMsg, int Func, int Error );
    void           SendFIXFill( OrderInfo* MatchOrder, char* Msg, BOOL Filled );
    int            MatchAuto();
    int            MatchByRule();
public:
    TransactionObject( TSECOrderManager* Manager );
    MessageHeader* OnMessage( ConnectionObjectBase* Connection, MessageHeader* Order, BOOL IsFIX );
    void SetOpenSwitch( int OpenSwitch );        
    void GetStates( OrderStates* States );
    int  Match();
    void SetLastConfirm( const UFC::AnsiString& PVCID , MessageHeader* );    
};
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
