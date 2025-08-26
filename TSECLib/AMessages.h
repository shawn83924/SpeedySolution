#ifndef __TWSE_AMESSAGES_H
#define __TWSE_AMESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "MessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
// Begin Auction Transection Sub-System messages.
//----------------------------------------------------------------------------------------------------------------------
//  A010 Order Message
//---------------------------------------------------------------------------
class MessageA010 : public MessageHeader
{
public:
    char BrokerID[4];        /* Broker ID */
    char PVCID[2];
    char OrderNo[5];         /* order # */
    char InvestorAcno[7];    /* investor account # */    
    char StockID[6];         /* Stock ID */
    char OrderPrice[9];      /* order price*/ ///< Old 6 New 9
    char OrderQty[12];       /* quantity */    
 public:
    MessageA010( char* FunctionCode, char* Body );
    MessageA010( const char* Data );
    void LoadBody( const char *Body );
    const UFC::AnsiString GetBrokerID() { return UFC::AnsiString( BrokerID,4); }
    const UFC::AnsiString GetOrderNo()  { return UFC::AnsiString( OrderNo,5); }
    const UFC::AnsiString GetStockID()  { return UFC::AnsiString( StockID, 6 );}
    double  GetOrderPrice( void );
    Int64   GetOrderQty( void );        
    virtual void AsString( char* Buffer );
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  A020 Confirm Order Message
//---------------------------------------------------------------------------
class MessageA020 : public MessageHeader
{
public:
    char BrokerID[ 4 ];
    char PVCID[ 2 ];
    char OrderNo[ 5 ];
    char InvestorAcno[ 7 ];
    char StockID[ 6 ]; 
    char OrderPrice[ 9 ]; ///< Old 6 New 9
    char OrderQty[ 12 ];    
    char OrderDate[ 8 ];  ///< YYMMDD New YYYYMMDD  6 --> 8
    char OrderTime[ 8 ];  ///< HHMMSS New HHMMSSmm  6 --> 8
    char BeforeQty[ 12 ];
    char AfterQty[ 12 ];
    char BeforePrice[ 9 ];///< Old 6 New 9
    char AfterPrice[ 9 ]; ///< Old 6 New 9   
public:
    MessageA020( BOOL IsTWSE = TRUE ):MessageHeader(  "70", "00", "01" ){}
    MessageA020( const char *Data );
    MessageA020( char *Func, char *Body );
    void LoadBody( const char* Data );
    virtual void AsString( char* Buffer );
    virtual void print( void );
    
    void SetPrice( double Px );
    void SetQty( Int64 Qty );                                
    void SetBeforeQty( Int64 Qty );                                
    void SetAfterQty( Int64 Qty );                                
    void SetDate( int Y, int M, int D );
    void SetTime( int h, int m, int S, int ms );      
};
//---------------------------------------------------------------------------
//  A030 Error message
//---------------------------------------------------------------------------
class MessageA030 : public MessageHeader
{
public:
    MessageA030( ):MessageHeader("70","00","03"){}
    MessageA030( const char* Data ):MessageHeader( Data ){}
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  A040 Heartbeat message (Request)
//---------------------------------------------------------------------------
class MessageA040 : public MessageHeader
{
public:
    MessageA040( ):MessageHeader("70","00","02"){}
    MessageA040( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("----------- Confirm connection message A040 ------------\n" );
    }
};
//---------------------------------------------------------------------------
//  A050 Heartbeat message (Reply)
//---------------------------------------------------------------------------
class MessageA050 : public MessageHeader
{
public:
    MessageA050( ):MessageHeader("70","00","05"){}
    MessageA050( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("--------- Confirm connection reply message A050 ---------\n" );
    }
};
//---------------------------------------------------------------------------
//  A060 Reconnect query message
//---------------------------------------------------------------------------
class MessageA060 : public MessageHeader
{
public:
    MessageA060( ):MessageHeader("70","00","04"){}
    MessageA060( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
     	SocketPrintf("--------- Reconnect query message A060 ---------\n" );
    }
};
//----------------------------------------------------------------------------------------------------------------------
// End Auction Transection Sub-System messages.
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------

