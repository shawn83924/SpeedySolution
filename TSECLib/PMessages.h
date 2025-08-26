#ifndef __TWSE_PMESSAGES_H
#define __TWSE_PMESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "MessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
// Begin Post Transection Sub-System messages.
//----------------------------------------------------------------------------------------------------------------------
//  P010 Order Message
//---------------------------------------------------------------------------
class MessageP010 : public MessageHeader, public RMOrderInterface
{
public:
    char BrokerID[5];        ///< Broker ID: 4
    char PVCID[3];           ///< PVCID :2      
    char OrderNo[6];         ///< Order Number:5
    char InvestorAcno[8];    ///< Investor account: 7    
    char InvestorFlag;       ///< investor flag         
    char StockID[7];         ///< Stock ID: 6    
    char BuySellCode;        ///< B/S code 
    char ExchangeCode;       ///< Exchange code   
    char TWSEOrderType;      ///< TWSE order type
public:///< Modify for 2020/03/20
    char OrderPrice[10];     ///< Order price: 6  *New: 9
    char OrderQty[9];        ///< quantity: 3 *New: 6 
public:///< Add for 2020/03/20
    char OrderType;          ///< *New OrderType: 1:Market 2:Limit  
    char TimeInForce;        ///< *NewTimeInForce 0:ROD 3:IOC 4:FOK            
private:
    int FQty;
    double FPx;    
    char Symbol[7];         ///< Stock ID: 6
public:
    MessageP010( BOOL IsTWSE, const char* FunctionCode );
    MessageP010( BOOL IsTWSE, char* FunctionCode, char* Body );
    MessageP010( const char* Data );
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
    virtual TSEOrderTypeEnum GetTWSEOrderType( void ){ return (TSEOrderTypeEnum)TWSEOrderType; }            
    virtual double      GetOrderPrice( void );
    virtual int         GetOrderQty( void );      
public:        
    virtual void AsString( char* Buffer );
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  P020 Confirm Order Message
//---------------------------------------------------------------------------
class MessageP020 : public MessageHeader
{
public:
    char BrokerID[ 4 ];
    char PVCID[ 2 ];
    char OrderNo[ 5 ];
    char InvestorAcno[ 7 ];
    char InvestorFlag;
    char StockID[ 6 ];  ///< Commodity ID ( Oprions 20 Futures 7)
    char OrderPrice[ 10 ]; ///< *6 -> 9
    char OrderQty[ 7 ];   ///< *3 -> 6
    char BuySellCode;
    char ExchangeCode;
    char TWSEOrderType;   ///< TWSE order type
    char OrderType;       ///< *New OrderType: 1:Market 2:Limit  
    char TimeInForce;     ///< *New TimeInForce 0:ROD 3:IOC 4:FOK    
    char OrderDate[ 9 ];  ///< * YYMMDD -> YYYYMMDD 6->8
    char OrderTime[ 10 ];  ///< * HHMMSS -> HHMMSSmmm  6->9
    char BeforeQty[ 7 ];  ///< * 3 -> 6
    char AfterQty[ 7 ];   ///< * 3 -> 6
public:
    MessageP020( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"32":"98"), "00", "01" ){}
    MessageP020( const char *Data );
    MessageP020( BOOL IsTWSE, char *Func, char *Body );
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
//  P030 Error message
//---------------------------------------------------------------------------
class MessageP030 : public MessageHeader
{
public:
    MessageP030( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"32":"98"),"00","03"){}
    MessageP030( const char* Data ):MessageHeader( Data ){}
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  P040 Heartbeat message (Request)
//---------------------------------------------------------------------------
class MessageP040 : public MessageHeader
{
public:
    MessageP040( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"32":"98"),"00","02"){}
    MessageP040( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("----------- Confirm connection message P040 ------------\n" );
    }
};
//---------------------------------------------------------------------------
//  P050 Heartbeat message (Reply)
//---------------------------------------------------------------------------
class MessageP050 : public MessageHeader
{
public:
    MessageP050( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"32":"98"),"00","05"){}
    MessageP050( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("--------- Confirm connection reply message P050 ---------\n" );
    }
};
//---------------------------------------------------------------------------
//  P060 Reconnect query message
//---------------------------------------------------------------------------
class MessageP060 : public MessageHeader
{
public:
    MessageP060( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"32":"98"),"00","04"){}
    MessageP060( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
     	SocketPrintf("--------- Reconnect query message P060 ---------\n" );
    }
};
//---------------------------------------------------------------------------
//  P070 Query message
//---------------------------------------------------------------------------
class MessageP070 : public MessageHeader
{
public:
    char StockID[ 6 ];    
public:
    MessageP070( const char *Data );
    void LoadBody( const char* Data );
    virtual void AsString( char* Buffer );
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  P080 Query Response message
//---------------------------------------------------------------------------
class MessageP080 : public MessageHeader
{
public:
    char StockID[ 6 ];    
    char StaticTime[4]; //HHMM
    char BuyCNT[10];
    char BuySHR[10];
    char SellCNT[10];
    char SellSHR[10];
public:
    MessageP080( const char *Data );
    void LoadBody( const char* Data );
    virtual void AsString( char* Buffer );
    virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
// End Transection Sub-System messages.
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------

