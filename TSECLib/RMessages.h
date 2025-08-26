//----------------------------------------------------------------------------------------------------------------------
#ifndef __TWSE_CMESSAGES_H
#define __TWSE_CMESSAGES_H
//----------------------------------------------------------------------------------------------------------------------
#include "MessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
class MessageR1 : public MessageHeader
{
    private:
        char BrokerID[ 4 ];
        char StartSeq[ 6 ];
    public:
    	MessageR1( const char* Data );
        MessageR1( BOOL IsTWSE, char* BrokerID, int SeqNo );
 	int   GetSeq();
        char* GetBrokerID(){ return BrokerID; }
        virtual void AsString( char* Buffer );
        virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class MessageR2 : public MessageHeader
{
    private:
        char BrokerID[ 4 ];
        char StartSeq[ 6 ];
    public:
    	MessageR2( const char* Data );
        MessageR2( BOOL IsTWSE, char* BrokerID, int SeqNo );
        int   GetSeq();
        virtual void AsString( char* Buffer );
        virtual void print( void );
};
//----------------------------------------------------------------------------------------------------------------------
class R3Body;
//----------------------------------------------------------------------------------------------------------------------
const int R3_MSG_COUNT = 48;
//----------------------------------------------------------------------------------------------------------------------
class MessageR3 : public MessageHeader
{
    private:
        char      FBodyLength[ 4 ];
        int       FMsgCount;
        R3Body*   FMessages[ R3_MSG_COUNT ]; ///< 4 -> 48
     public:
        MessageR3( BOOL IsTWSE );
        MessageR3( const char* Buffer );
        virtual ~MessageR3( );
        virtual void AsString( char* Buffer );
        virtual void print( void );
        int     GetMessageCount( void ) { return FMsgCount; }
        R3Body* GetMessage( int Index ) { return FMessages[Index]; }
        BOOL    AddMessage( R3Body* NewMessage );
};
//----------------------------------------------------------------------------------------------------------------------
class R3Body
{
    public:
        char FStockID[ 6 ];
        char FQuntity[ 9 ];
        char FPrice[10];     ///< New 6->9
        char FMatchTime[10]; ///< New 8->9
        char FEXCD;
        char FBuySellCode;
        char FOrderNo[ 5 ];
        char FInvestorACNo[ 7 ];
        char FOrderType;
        char FSeq[ 6 ];
        char FBroker[ 4 ];
        char FRecNo[ 8 ];
        char Fmarks;
    public:        
        int  FQty;
        double FLastPx;
    public:
        R3Body( ) {}
        R3Body( const char* Data, int& Offset );
        virtual ~R3Body( void ){}
        virtual int AsString( char* Buffer );
        virtual void print( void );
    public:
        Int32  GetSeq( void ) { return UFC::AnsiString( FSeq, 6 ).ToInt(); }
        Int32  GetRecNo( void ) { return UFC::AnsiString( FRecNo, 8 ).ToInt(); }
        int    ProcessingTime( void );
        double GetPrice( void );
        int    GetQty( void );
    public: 
        void  SetPrice( double Px );
        void  SetQty( int Qty );
        void  SetTime( int H, int M, int S, int mmm );        
    public:
        static R3Body* CreateFromString( const char* Str, int &offset );  ///< Class factory.
};
//----------------------------------------------------------------------------------------------------------------------
class MessageR4 : public MessageHeader
{
   public:
        MessageR4( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"50":"95"),"00","04"){}
        MessageR4( const char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
            SocketPrintf("----------- Confirm Connection message R4 ------------\n" );
        }
};
//----------------------------------------------------------------------------------------------------------------------
class MessageR5 : public MessageHeader
{
   public:
        MessageR5( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"50":"95"),"00","05"){}
        MessageR5( const char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
            SocketPrintf("-------- Confirm Connection reply message R5----------\n" );
        }
};
//----------------------------------------------------------------------------------------------------------------------
class MessageR6 : public MessageHeader
{
   private:
        char TotalRecord[6];
   public:
        MessageR6( BOOL IsTWSE, int Total )
        :MessageHeader( (char*)((IsTWSE)?"50":"95"),"00","10" )
        {
            UFC::AnsiString CountStr;
            CountStr.Printf( "%06d",Total );
            memcpy( TotalRecord, CountStr.c_str(), 6 );
        }
        MessageR6( const char* Data ):MessageHeader( Data )
        {
            memcpy( TotalRecord, Data + MESSAGE_HEADER_SIZE, 6 );
        }
        virtual void AsString( char* Buffer )
        {
            MessageHeader::AsString( Buffer );
            MemoryStreamWriter MemWriter( Buffer + MESSAGE_HEADER_SIZE );
            MemWriter.Write( TotalRecord, 6 );
            MemWriter.AppendZero();
        }
        virtual void print( void )
        {
            SocketPrintf("------------- End Confirm Message C060 -----------------\n" );
        }
};
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
