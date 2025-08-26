#ifndef __TWSE_OMESSAGES_H
#define __TWSE_OMESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "MessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
// Begin Odd Transection Sub-System messages.
//----------------------------------------------------------------------------------------------------------------------
//  O010 Order Message
//---------------------------------------------------------------------------
class MessageO010 : public MessageHeader, public RMOrderInterface
{
public:
    char BrokerID[5];        ///< Broker ID: 4
    char PVCID[3];           ///< PVCID :2      
    char OrderNo[6];         ///< Order Number:5
    char InvestorAcno[8];    ///< Investor account: 7    
    char StockID[7];         ///< Stock ID: 6    
    char BuySellCode;        ///< B/S code 
public:///< Remove for 2020/03/20
    char TradeKind;          ///< TradeKind
    char ObjectBroker[5];    ///< ObjectBroker: 4
public:///< Modify for 2020/03/20
    char OrderPrice[10];     ///< Order price: 6  *New: 9
    char OrderQty[9];        ///< quantity: 8 *New: 6 
public:///< Add for 2020/03/20
    char InvestorFlag;       ///< investor flag     
    char ExchangeCode;       ///< Exchange code   
    char TWSEOrderType;      ///< TWSE order type
    char OrderType;          ///< *New OrderType: 1:Market 2:Limit  
    char TimeInForce;        ///< *NewTimeInForce 0:ROD 3:IOC 4:FOK        
private:
    int FQty;
    double FPx;      
    char Symbol[7];         ///< Stock ID: 6
public:
    MessageO010( BOOL IsTWSE, const char* FunctionCode );
    MessageO010( BOOL IsTWSE, char* FunctionCode, char* Body );
    MessageO010( const char* Data );
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
//  O020 Confirm Order Message
//---------------------------------------------------------------------------
class MessageO020 : public MessageHeader
{
public:
    char BrokerID[ 5 ];
    char PVCID[ 3 ];
    char OrderNo[ 6 ];
    char InvestorAcno[ 8 ];
    char StockID[ 7 ];  ///< Commodity ID ( Oprions 20 Futures 7)    
public:///< Remove for 2020/03/20    
    char BuySellCode;
    char TradeKind;
    char ObjectBroker[ 4 ];    
public:///< Modify for 2020/03/20
    char OrderPrice[ 10 ]; ///< *New 6 -> 9
    char OrderQty[ 9 ];   ///< *New 8 -> 6
    char OrderDate[ 9 ];  ///< *New YYMMDD -> YYYYMMDD
    char OrderTime[ 10 ];  ///< *New HHMMSS -> HHMMSSmmm
    char BeforeQty[ 9 ];  ///< *New 8 -> 6
    char AfterQty[ 9 ];   ///< *New 8 -> 6        
public:///< Add for 2020/03/20    
    char InvestorFlag;
    char ExchangeCode;
    char TWSEOrderType;   ///< TWSE order type
    char OrderType;       ///< *New OrderType: 1:Market 2:Limit  
    char TimeInForce;     ///< *New TimeInForce 0:ROD 3:IOC 4:FOK    
public:
    MessageO020( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"40":"94"), "00", "01" ){}
    MessageO020( const char *Data );
    MessageO020( BOOL IsTWSE, char *Func, char *Body );
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
//  O030 Error message
//---------------------------------------------------------------------------
class MessageO030 : public MessageHeader
{
public:
    MessageO030( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"40":"94"),"00","03"){}
    MessageO030( const char* Data ):MessageHeader( Data ){}
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  O040 Query message
//---------------------------------------------------------------------------
class MessageO040 : public MessageHeader
{
public:
    char StockID[ 6 ];
    char StickSHR[ 3 ];
public:
    MessageO040( const char *Data );
    void LoadBody( const char* Data );
    virtual void AsString( char* Buffer );
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  O060 Query Response message
//---------------------------------------------------------------------------
class MessageO060 : public MessageHeader
{
public:
    char StockID[ 6 ];
    char StickSHR[ 3 ];
    char BuyCNT[9];
    char Filler1[3];
    char SellCNT[9];
    char BuySHR[9];
    char Filler2[3];
    char SellSHR[9];
public:
    MessageO060( const char *Data );
    void LoadBody( const char* Data );
    virtual void AsString( char* Buffer );
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  O070 Heartbeat message (Request)
//---------------------------------------------------------------------------
class MessageO070 : public MessageHeader
{
public:
    MessageO070( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"40":"94"),"00","04"){}
    MessageO070( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("----------- Confirm connection message O070 ------------\n" );
    }
};
//---------------------------------------------------------------------------
//  O080 Heartbeat message (Reply)
//---------------------------------------------------------------------------
class MessageO080 : public MessageHeader
{
public:
    MessageO080( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"40":"94"),"00","09"){}
    MessageO080( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("--------- Confirm connection reply message O080 ---------\n" );
    }
};
//---------------------------------------------------------------------------
//  O090 Reconnect query message
//---------------------------------------------------------------------------
class MessageO090 : public MessageHeader
{
public:
    MessageO090( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"40":"94"),"00","06"){}
    MessageO090( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
      	SocketPrintf("--------- Reconnect query message O060 ---------\n" );
    }
};
//----------------------------------------------------------------------------------------------------------------------
// End Transection Sub-System messages.
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------

