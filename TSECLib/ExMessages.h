#ifndef __TWSE_EXMESSAGES_H
#define __TWSE_EXMESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "MessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
// Begin Tender Transection Sub-System messages.
//----------------------------------------------------------------------------------------------------------------------
//  Ex010 Order Message
//---------------------------------------------------------------------------
class MessageEx010 : public MessageHeader
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
    MessageEx010( BOOL IsTWSE, char* FunctionCode, char* Body );
    MessageEx010( const char* Data );
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
//  Ex020 Confirm Order Message
//---------------------------------------------------------------------------
class MessageEx020 : public MessageHeader
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
    char OrderDate[ 8 ];  ///< YYYYMMDD 
    char OrderTime[ 8 ];  ///< HHMMSSmm 
    char BeforeQty[ 12 ];
    char AfterQty[ 12 ];    
    char BeforePrice[ 9 ];///< Old 6 New 9
    char AfterPrice[ 9 ]; ///< Old 6 New 9       
public:
    MessageEx020( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"42":"89"), "00", "01" ){}
    MessageEx020( const char *Data );
    MessageEx020( BOOL IsTWSE, char *Func, char *Body );
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
//  Ex030 Error message
//---------------------------------------------------------------------------
class MessageEx030 : public MessageHeader
{
public:
    MessageEx030( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"42":"89"),"00","03"){}
    MessageEx030( const char* Data ):MessageHeader( Data ){}
    virtual void print( void );
};
//---------------------------------------------------------------------------
//  E040 Heartbeat message (Request)
//---------------------------------------------------------------------------
class MessageEx040 : public MessageHeader
{
public:
    MessageEx040( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"42":"89"),"00","02"){}
    MessageEx040( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("----------- Confirm connection message E140 ------------\n" );
    }
};
//---------------------------------------------------------------------------
//  E050 Heartbeat message (Reply)
//---------------------------------------------------------------------------
class MessageEx050 : public MessageHeader
{
public:
    MessageEx050( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"42":"89"),"00","05"){}
    MessageEx050( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("--------- Confirm connection reply message E150 ---------\n" );
    }
};
//---------------------------------------------------------------------------
//  E060 Reconnect query message
//---------------------------------------------------------------------------
class MessageEx060 : public MessageHeader
{
public:
    MessageEx060( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"42":"89"),"00","04"){}
    MessageEx060( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
     	SocketPrintf("--------- Reconnect query message E160 ---------\n" );
    }
};
//----------------------------------------------------------------------------------------------------------------------
// End Tender Transection Sub-System messages.
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------

