#ifndef __TWSE_VMESSAGES_H
#define __TWSE_VMESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "MessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
// Begin Lend Transection Sub-System messages.
//----------------------------------------------------------------------------------------------------------------------
//  V010 Order Message
//---------------------------------------------------------------------------
class MessageV010 : public MessageHeader
{
public:
    char BrokerID[4];        /* Broker ID */
    char PVCID[2];
    char OrderNo[5];         /* order # */
    char InvestorAcno[7];    /* investor account # */    
    char StockID[6];         /* Stock ID */
    char OrderPrice[8];      /* order price*/ ///< Old 7 New 8
    char OrderQty[6];       /* quantity */    
    char Keep;       /* quantity */    
 public:
    MessageV010( BOOL IsTWSE, char* FunctionCode, char* Body );
    MessageV010( const char* Data );
    void LoadBody( const char *Body );
    const UFC::AnsiString GetBrokerID() { return UFC::AnsiString( BrokerID,4); }
    const UFC::AnsiString GetOrderNo()  { return UFC::AnsiString( OrderNo,5); }
    const UFC::AnsiString GetStockID()  { return UFC::AnsiString( StockID, 6 );}
    double  GetOrderPrice( void );
    int     GetOrderQty( void );        
    virtual void AsString( char* Buffer );
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  V020 Confirm Order Message
//---------------------------------------------------------------------------
class MessageV020 : public MessageHeader
{
public:
    char BrokerID[ 4 ];
    char PVCID[ 2 ];
    char OrderNo[ 5 ];
    char InvestorAcno[ 7 ];
    char StockID[ 6 ]; 
    char OrderPrice[ 8 ]; ///< Old 7 new 8
    char OrderQty[ 6 ];      
    char Keep;
    char OrderDate[ 8 ];  ///< YYMMDD  New YYYYMMDD
    char OrderTime[ 8 ];  ///< HHMMSS  New HHMMSSmm
    char BeforeQty[ 6 ];
    char AfterQty[ 6 ];    
public:
    MessageV020( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"31":"90"), "00", "01" ){}
    MessageV020( const char *Data );
    MessageV020( BOOL IsTWSE, char *Func, char *Body );
    void LoadBody( const char* Data );
    virtual void AsString( char* Buffer );
    virtual void print( void );
    
    void SetPrice( double Px );
    void SetQty( int Qty );                                
    void SetBeforeQty( int Qty );                                
    void SetAfterQty( int Qty );                                
    void SetDate( int Y, int M, int D );
    void SetTime( int h, int m, int S, int ms );  
};
//---------------------------------------------------------------------------
//  V030 Error message
//---------------------------------------------------------------------------
class MessageV030 : public MessageHeader
{
public:
    MessageV030( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"31":"90"),"00","03"){}
    MessageV030( const char* Data ):MessageHeader( Data ){}
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  A040 Heartbeat message (Request)
//---------------------------------------------------------------------------
class MessageV040 : public MessageHeader
{
public:
    MessageV040( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"31":"90"),"00","02"){}
    MessageV040( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("----------- Confirm connection message V040 ------------\n" );
    }
};
//---------------------------------------------------------------------------
//  V050 Heartbeat message (Reply)
//---------------------------------------------------------------------------
class MessageV050 : public MessageHeader
{
public:
    MessageV050( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"31":"90"),"00","05"){}
    MessageV050( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("--------- Confirm connection reply message V050 ---------\n" );
    }
};
//---------------------------------------------------------------------------
//  V060 Reconnect query message
//---------------------------------------------------------------------------
class MessageV060 : public MessageHeader
{
public:
    MessageV060( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"31":"90"),"00","04"){}
    MessageV060( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
     	SocketPrintf("--------- Reconnect query message V060 ---------\n" );
    }
};
//----------------------------------------------------------------------------------------------------------------------
// End Auction Transection Sub-System messages.
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------

