#ifndef __TWSE_TMESSAGES_H
#define __TWSE_TMESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "MessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{

// 4   char BrokerID[4];        ///< Broker ID: 4
// 4   int  InvestorAcno;       ///< Investor account: 4
// 6   char StockID[6];         ///< Stock ID: 6
// 4   int  OrderPrice;         ///< Order price: 4
// 4   int  OrderQty;           ///< quantity: 4
// 1  1bite    char BuySellCode;        ///< B/S code 
//    1bite    char ExchangeCode;       ///< Exchange code: 0: Regular, FixedPrice  2: OddLots   
//    3bite    char TWSEOrderType;      ///< TWSE order type: 0,1,2,3,4,5,6
//    1bite    char OrderType;          ///< *New OrderType: 1:Market 2:Limit  
//    2bite    char TimeInForce;        ///< *NewTimeInForce 0:ROD 3:IOC 4:FOK    
// 23 Bytes.
//----------------------------------------------------------------------------------------------------------------------
// Begin Transection Sub-System messages.
//----------------------------------------------------------------------------------------------------------------------
//  R010 Order Message
//---------------------------------------------------------------------------
class MessageT010 : public MessageHeader, public RMOrderInterface
{
public:
    char BrokerID[5];        ///< Broker ID: 4
    char PVCID[3];           ///< PVCID :2  
    char OrderNo[6];         ///< Order Number:5
    char InvestorAcno[8];    ///< Investor account: 7
    char InvestorFlag;       ///< investor flag: 1: Normal(FIX) 2:ATM(FIX) 3:DMA Order(FIX) 4:Internet(FIX) 5:Voice(FIX) 6:API(FIX)
    char StockID[7];         ///< Stock ID: 6
    char OrderPrice[10];     ///< Order price: 6  *New: 9
    char OrderQty[7];        ///< quantity: 3 *New: 6 
    char BuySellCode;        ///< B/S code 
    char ExchangeCode;       ///< Exchange code: 0: Regular, FixedPrice  2: OddLots   
    char TWSEOrderType;      ///< TWSE order type: 0:Normal
                             ///< Via Securities Finance 1: Purchase on Margin 2:Short Sell
                             ///< Via Securities Firms conduct Margin Lending 3:Purchase on Margin 4:Short Sell
                             ///< 5:  SBL Short Sell type 6: SBL Short Sell type 6
    char OrderType;          ///< *New OrderType: 1:Market 2:Limit  
    char TimeInForce;        ///< *NewTimeInForce 0:ROD 3:IOC 4:FOK    
private:
    int    FQty;
    double FPx;
    char   Symbol[7];         ///< Stock ID: 6
public:
    MessageT010( BOOL IsTWSE, const char* FunctionCode );
    MessageT010( BOOL IsTWSE, char* FunctionCode, char* Body );
    MessageT010( const char* Data );
    MessageT010( void );
public:    
    void Load( const char *Data );
    void LoadBody( const char *Body );
    void SetPrice( double Px );
    void SetQty( int Qty );    
public:        
    const char*      GetStockID( void )  { return StockID; }  ///< Not trimed      
    
    virtual const char* GetSymbol( void )        { return Symbol; }
    virtual const char* GetOrderID( void )       { return OrderNo; }
    virtual const char* GetBrokerID( void )      { return BrokerID; }  
    virtual const char* GetAccount( void )       { return InvestorAcno; }
    virtual TSESideEnum GetSide( void )          { return (TSESideEnum)BuySellCode; } 
    virtual TSEOrderTypeEnum   GetTWSEOrderType( void ) { return (TSEOrderTypeEnum)TWSEOrderType; }            
    virtual TSEPriceTypeEnum   GetOrderType( void )     { return (TSEPriceTypeEnum)OrderType; }            
    virtual TSETimeInForceEnum GetTimeInForce( void )   { return (TSETimeInForceEnum)TimeInForce; }                
    virtual double             GetOrderPrice( void );
    virtual int                GetOrderQty( void );        
public:    
    virtual void AsString( char* Buffer );
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  R020 Confirm Order Message
//---------------------------------------------------------------------------
class MessageT020 : public MessageHeader
{
public:
    char BrokerID[ 4 ];
    char PVCID[ 2 ];
    char OrderNo[ 5 ];
    char InvestorAcno[ 7 ];
    char InvestorFlag;
    char StockID[ 6 ];  
    char OrderPrice[ 10 ]; ///< *New 6 -> 9
    char OrderQty[ 7 ];   ///< *New 3 -> 6
    char BuySellCode;
    char ExchangeCode;
    char TWSEOrderType;   ///< TWSE order type
    char OrderType;       ///< *New OrderType: 1:Market 2:Limit  
    char TimeInForce;     ///< *New TimeInForce 0:ROD 3:IOC 4:FOK    
    char OrderDate[ 9 ];  ///< *New YYMMDD -> YYYYMMDD
    char OrderTime[ 10 ];  ///< *New HHMMSS -> HHMMSSmmm
    char BeforeQty[ 7 ];  ///< *New 3 -> 6
    char AfterQty[ 7 ];   ///< *New 3 -> 6
public:
    MessageT020( BOOL IsTWSE ):MessageHeader(  (char*)((IsTWSE)?"30":"93"), "00", "01" ){}
    MessageT020( const char *Data );
    MessageT020( BOOL IsTWSE, char *Func, char *Body );
    void LoadBody( const char* Data );
    virtual void AsString( char* Buffer );
    virtual void print( void );
public:
    void SetPrice( double Px );
    void SetQty( int Qty );
    void SetBeforeQty( int Qty );
    void SetAfterQty( int Qty );
    void SetDate( int YYYY, int MM, int DD );
    void SetTime( int H, int M, int S, int mmm );
};
//---------------------------------------------------------------------------
//  R030 Error message
//---------------------------------------------------------------------------
class MessageT030 : public MessageHeader
{
public:
    MessageT030( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"30":"93"),"00","03"){}
    MessageT030( const char* Data ):MessageHeader( Data ){}
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  R040 Heartbeat message (Request)
//---------------------------------------------------------------------------
class MessageT040 : public MessageHeader
{
public:
    MessageT040( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"30":"93"),"00","02"){}
    MessageT040( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("----------- Confirm connection message R040 ------------\n" );
    }
};
//---------------------------------------------------------------------------
//  R050 Heartbeat message (Reply)
//---------------------------------------------------------------------------
class MessageT050 : public MessageHeader
{
public:
    MessageT050( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"30":"93"),"00","05"){}
    MessageT050( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("--------- Confirm connection reply message R050 ---------\n" );
    }
};
//---------------------------------------------------------------------------
//  R060 Reconnect query message
//---------------------------------------------------------------------------
class MessageT060 : public MessageHeader
{
   public:
        MessageT060( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"30":"93"),"00","04"){}
        MessageT060( const char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("--------- Reconnect query message R060 ---------\n" );
        }
};
//----------------------------------------------------------------------------------------------------------------------
// End Transection Sub-System messages.
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------

