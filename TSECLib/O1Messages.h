#ifndef __TWSE_O1MESSAGES_H
#define __TWSE_O1MESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "MessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
// Begin Transection Sub-System messages.
//----------------------------------------------------------------------------------------------------------------------
//  O110 Order Message
//---------------------------------------------------------------------------
class MessageO110 : public MessageHeader, public RMOrderInterface
{
public:
    char BrokerID[5];        ///< Broker ID: 4
    char PVCID[3];           ///< PVCID :2  
    char OrderNo[6];         ///< Order Number:5
    char InvestorAcno[8];    ///< Investor account: 7
    char InvestorFlag;       ///< investor flag 
    char StockID[7];         ///< Stock ID: 6
    char OrderPrice[10];     ///< Order price: 6  *New: 9
    char OrderQty[7];        ///< quantity: 3 *New: 6 
    char BuySellCode;        ///< B/S code 
    char ExchangeCode;       ///< Exchange code   
    char TWSEOrderType;      ///< TWSE order type
    char OrderType;          ///< *New OrderType: 1:Market 2:Limit  
    char TimeInForce;        ///< *NewTimeInForce 0:ROD 3:IOC 4:FOK    
private:
    int    FQty;
    double FPx;
    char   Symbol[7];         ///< Stock ID: 6
public:
    MessageO110( BOOL IsTWSE, const char* FunctionCode );
    MessageO110( BOOL IsTWSE, char* FunctionCode, char* Body );
    MessageO110( const char* Data );
public:    
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
//  O120 Confirm Order Message
//---------------------------------------------------------------------------
class MessageO120 : public MessageHeader
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
    MessageO120( BOOL IsTWSE ):MessageHeader(  (char*)((IsTWSE)?"33":"83"), "00", "01" ){}
    MessageO120( const char *Data );
    MessageO120( BOOL IsTWSE, char *Func, char *Body );
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
//  O130 Error message
//---------------------------------------------------------------------------
class MessageO130 : public MessageHeader
{
public:
    MessageO130( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"33":"83"),"00","03"){}
    MessageO130( const char* Data ):MessageHeader( Data ){}
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  O140 Heartbeat message (Request)
//---------------------------------------------------------------------------
class MessageO140 : public MessageHeader
{
public:
    MessageO140( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"33":"83"),"00","02"){}
    MessageO140( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("----------- Confirm connection message O140 ------------\n" );
    }
};
//---------------------------------------------------------------------------
//  O150 Heartbeat message (Reply)
//---------------------------------------------------------------------------
class MessageO150 : public MessageHeader
{
public:
    MessageO150( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"33":"83"),"00","05"){}
    MessageO150( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("--------- Confirm connection reply message O150 ---------\n" );
    }
};
//---------------------------------------------------------------------------
//  O160 Reconnect query message
//---------------------------------------------------------------------------
class MessageO160 : public MessageHeader
{
   public:
        MessageO160( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"33":"83"),"00","04"){}
        MessageO160( const char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("--------- Reconnect query message O160 ---------\n" );
        }
};
//----------------------------------------------------------------------------------------------------------------------
// End Transection Sub-System messages.
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------

