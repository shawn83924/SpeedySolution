#ifndef __TWSE_SMESSAGES_H
#define __TWSE_SMESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "MessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
// Begin Negotiate Price Transection Sub-System messages.
//----------------------------------------------------------------------------------------------------------------------
//  Base class for all S Messages
//---------------------------------------------------------------------------
class MessageSXXX : public MessageHeader
{
public:    
    char* FMsgBody;
    Int32 FLength;
 public:
    MessageSXXX( const char* Func, const char* Type, char* Body, int Length );
    MessageSXXX( const char* Data, char* Body, int Length = 0 );
    void         SetBody( const char* Body, int Length );
    virtual void AsString( char* Buffer );
    virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
//  S010 Buy/Sell report Message
//---------------------------------------------------------------------------
class MessageS010 : public MessageSXXX
{
public:    
    char Body[ 31 ]; ///< Old version 26,after 2020/03/02 32
 public:
    MessageS010( ):MessageSXXX( "00", "01", Body, (MessageHeader::FNewExVersion == TRUE)? 31:26 ){}
    MessageS010( const char* Data ):MessageSXXX( Data, Body, (MessageHeader::FNewExVersion == TRUE)? 31:26 ){}
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S010 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S010 --------------------\n" );        
    }
};
//---------------------------------------------------------------------------
//  S020 Buy/Sell report confirm Message
//---------------------------------------------------------------------------
class MessageS020 : public MessageSXXX
{
public:    
    char Body[ 31 ]; ///< Old version 26,after 2020/03/02 become 31
public:
    MessageS020( ):MessageSXXX( "00", "02", Body, (MessageHeader::FNewExVersion == TRUE)? 31:26 ){}    
    MessageS020( const char *Data ):MessageSXXX( Data, Body, (MessageHeader::FNewExVersion == TRUE)? 31:26 ){}    
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S020 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S020 --------------------\n" );        
    }
};
//---------------------------------------------------------------------------
//  S030 Customer fill report message.
//---------------------------------------------------------------------------
class MessageS030 : public MessageSXXX
{
public: ///< Old:41 New:48 
    char Body[ 53 ]; ///< Old version 48,after 2020/03/02 53
public:
    MessageS030( ):MessageSXXX( "00","03", Body, (MessageHeader::FNewExVersion == TRUE)? 53:48 ){}
    MessageS030( const char* Data ):MessageSXXX( Data, Body,  (MessageHeader::FNewExVersion == TRUE)? 53:48 ){}
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S030 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S030 --------------------\n" );        
    }
};
//---------------------------------------------------------------------------
//  S040 Customer fill report confirm message.
//---------------------------------------------------------------------------
class MessageS040 : public MessageSXXX
{
public: ///< Old:53 New:60
    char Body[67];///< Old version 60,after 2020/03/02 67
public:
    MessageS040( ):MessageSXXX( "00","04", Body, (MessageHeader::FNewExVersion == TRUE)? 67:60 ){}
    MessageS040( const char* Data ):MessageSXXX( Data, Body, (MessageHeader::FNewExVersion == TRUE)? 67:60 ){}
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S040 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S040 --------------------\n" );        
    }
};
//---------------------------------------------------------------------------
//  S050 Sell side fill report message.
//---------------------------------------------------------------------------
class MessageS050 : public MessageSXXX
{
public: ///< Old:29 New:36
    char Body[41];///< Old version 36,after 2020/03/02 41
public:
    MessageS050( ):MessageSXXX( "00","05", Body, (MessageHeader::FNewExVersion == TRUE)? 41:36 ) {}
    MessageS050( const char* Data ):MessageSXXX( Data,Body,(MessageHeader::FNewExVersion == TRUE)? 41:36 ) {}
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S050 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S050 --------------------\n" );        
    }
};
//---------------------------------------------------------------------------
//  S060 Sell side fill report confirm message.
//---------------------------------------------------------------------------
class MessageS060 : public MessageSXXX
{
public: ///< Old:47 New:54
    char Body[63];///< Old version 54,after 2020/03/02 63
public:
    MessageS060( ):MessageSXXX("00","06",Body, (MessageHeader::FNewExVersion == TRUE)? 63:54){}
    MessageS060( const char* Data ):MessageSXXX( Data,Body, (MessageHeader::FNewExVersion == TRUE)? 63:54 ){}
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S060 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S060 --------------------\n" );        
    }
};
//---------------------------------------------------------------------------
//  S070 Buy side fill report message.
//---------------------------------------------------------------------------
class MessageS070 : public MessageSXXX
{
public:///< Old:18 New:25
    char Body[25];
public:
    MessageS070( ):MessageSXXX( "00","07", Body, 25 ){}
    MessageS070( const char* Data ):MessageSXXX( Data, Body, 25 ){}
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S070 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S070 --------------------\n" );        
    }
};
//---------------------------------------------------------------------------
//  S080 Buy side fill report confirm message.
//---------------------------------------------------------------------------
class MessageS080 : public MessageSXXX
{
public:///< Old:52 New:59
    char Body[68];///< Old version 59,after 2020/03/02 68
public:
    MessageS080( ):MessageSXXX( "00","08", Body, (MessageHeader::FNewExVersion == TRUE)? 68:59 ){}
    MessageS080( const char* Data ):MessageSXXX( Data, Body, (MessageHeader::FNewExVersion == TRUE)? 68:59 ){}
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S080 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S080 --------------------\n" );        
    }
};
//---------------------------------------------------------------------------
//  S090 fill report query message.
//---------------------------------------------------------------------------
class MessageS090 : public MessageSXXX
{
public:
    char Body[6];
public:
    MessageS090( ):MessageSXXX( "00","09", Body, 6 ){}
    MessageS090( const char* Data ):MessageSXXX( Data, Body, 6 ){}
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S090 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S090 --------------------\n" );        
    }
};
//---------------------------------------------------------------------------
//  S100 fill report query response message.
//---------------------------------------------------------------------------
class MessageS100 : public MessageSXXX
{
public:///< Old version 58,after 2020/03/02 73
    char Body[ 2 + 73*10 ]; ///< Up to 10 report per message.
    
public:
    MessageS100( ):MessageSXXX( "04","10", Body, (MessageHeader::FNewExVersion == TRUE)? 732:582 ){}
    MessageS100( const char* Data );
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S100 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S100 --------------------\n" );           
    }
};
//---------------------------------------------------------------------------
//  S110 Buy/Sell report query message.
//---------------------------------------------------------------------------
class MessageS110 : public MessageSXXX
{
public:
    char Body[7];
public:
    MessageS110( ):MessageSXXX( "04","11", Body, 7 ){}
    MessageS110( const char* Data ):MessageSXXX( Data, Body, 7  ){}
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S110 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S110 --------------------\n" );        
    }
};
//---------------------------------------------------------------------------
//  S120 Buy/Sell report query response message.
//---------------------------------------------------------------------------
class MessageS120 : public MessageSXXX
{
public:///< Old version 23,after 2020/03/02 28
    char Body[ 8 + 28*10 ];
public:
    MessageS120( ):MessageSXXX("04", "11", Body, (MessageHeader::FNewExVersion == TRUE)? 288:238){}
    MessageS120( const char* Data );
    virtual void print( void )
    {
        SocketPrintf("---------------------- Message S120 ----------------------\n" );
        MessageSXXX::print();
        SocketPrintf("-------------------- End message S120 --------------------\n" );                   
    }
};
//---------------------------------------------------------------------------
//
//  Header size message.
//
//---------------------------------------------------------------------------
//  S130 Heartbeat message (Request)
//---------------------------------------------------------------------------
class MessageS130 : public MessageHeader
{
public:
    MessageS130( ):MessageHeader("96","00","13"){}
    MessageS130( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("----------- Confirm connection message S130 ------------\n" );
    }
};
//---------------------------------------------------------------------------
//  S140 Heartbeat message (Reply)
//---------------------------------------------------------------------------
class MessageS140 : public MessageHeader
{
public:
    MessageS140( ):MessageHeader("96","00","14"){}
    MessageS140( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("--------- Confirm connection reply message S140 ---------\n" );
    }
};
//---------------------------------------------------------------------------
//  S150 Error message
//---------------------------------------------------------------------------
class MessageS150 : public MessageHeader
{
public:
    MessageS150( ):MessageHeader("96","00","15"){}
    MessageS150( const char* Data ):MessageHeader( Data ){}
    virtual void print( void )
    {
        SocketPrintf("--------- Error message S150:%d ---------\n", GetStatusCode() );
    }    
};
//----------------------------------------------------------------------------------------------------------------------
// End Negotiate Px Transection Sub-System messages.
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------

