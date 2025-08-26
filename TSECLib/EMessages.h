#ifndef __TWSE_EMESSAGES_H
#define __TWSE_EMESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "MessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
// Begin Tender Transection Sub-System messages.
//----------------------------------------------------------------------------------------------------------------------
//  E010 Order Message
//---------------------------------------------------------------------------
class MessageE010 : public MessageHeader
{
public:
    char BrokerID[4];        /* Broker ID */
    char PVCID[2];
    char OrderNo[5];         /* order # */
    char InvestorAcno[7];    /* investor account # */    
    char StockID[6];         /* Stock ID */
    char StkSeqNo[2];
    char OrderPrice[9];      /* order price*/ ///< Old 6 New 9
    char OrderQty[12];       /* quantity */    
 public:
    MessageE010( BOOL IsTWSE, char* FunctionCode, char* Body );
    MessageE010( const char* Data );
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
//  E020 Confirm Order Message
//---------------------------------------------------------------------------
class MessageE020 : public MessageHeader
{
public:
    char BrokerID[ 4 ];
    char PVCID[ 2 ];
    char OrderNo[ 5 ];
    char InvestorAcno[ 7 ];
    char StockID[ 6 ]; 
    char StkSeqNo[2];    
    char OrderPrice[ 9 ]; ///< Old 6 New 9 
    char OrderQty[ 12 ];    
    char OrderDate[ 8 ];  ///< YYMMDD New YYYYMMDD
    char OrderTime[ 8 ];  ///< HHMMSS New HHMMSSmm
    char BeforeQty[ 12 ];
    char AfterQty[ 12 ];    
    char BeforePrice[ 9 ]; ///< Old 6 New 9
    char AfterPrice[ 9 ];  ///< Old 6 New 9      
public:
    MessageE020( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"41":"97"), "00", "01" ){}
    MessageE020( const char *Data );
    MessageE020( BOOL IsTWSE, char *Func, char *Body );
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
//  E030 Error message
//---------------------------------------------------------------------------
class MessageE030 : public MessageHeader
{
public:
    MessageE030( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"41":"97"),"00","03"){}
    MessageE030( const char* Data ):MessageHeader( Data ){}
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  E040 Heartbeat message (Request)
//---------------------------------------------------------------------------
class MessageE040 : public MessageHeader
{
public:
    MessageE040( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"41":"97"),"00","02"){}
    MessageE040( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("----------- Confirm connection message E040 ------------\n" );
    }
};
//---------------------------------------------------------------------------
//  E050 Heartbeat message (Reply)
//---------------------------------------------------------------------------
class MessageE050 : public MessageHeader
{
public:
    MessageE050( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"41":"97"),"00","05"){}
    MessageE050( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("--------- Confirm connection reply message E050 ---------\n" );
    }
};
//---------------------------------------------------------------------------
//  E060 Reconnect query message
//---------------------------------------------------------------------------
class MessageE060 : public MessageHeader
{
public:
    MessageE060( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"41":"97"),"00","04"){}
    MessageE060( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
     	SocketPrintf("--------- Reconnect query message E060 ---------\n" );
    }
};
//----------------------------------------------------------------------------------------------------------------------
// End Tender Transection Sub-System messages.
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------

