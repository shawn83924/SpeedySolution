#ifndef _TWSE_FMESSAGES_H
#define _TWSE_FMESSAGES_H
//---------------------------------------------------------------------------
#include "MessageHeader.h"
//---------------------------------------------------------------------------
#define TWSE_FMESSAGE_HEADER_SIZE  26
#define TWSE_DATA_SIZE_F030       994
#define TWSE_DATA_SIZE_F210      3976
#define TWSE_DATA_SIZE_F050       995
#define TWSE_DATA_SIZE_F060       995    
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//---------------------------------------------------------------------------
class FMessageHeader : public MessageHeader
{
private :
    char FSourceID[4];
    char FDestinationID[4];
    char FMessageLength[4];

public :
    FMessageHeader( BOOL IsTWSE, const  char* SourceID, const  char* DestinationID, int MessageLength, const  char* Func, const char* MsgType, const char* Status = "00" );
    FMessageHeader( const char* Data );

public :
    virtual void AsString( char* Buffer );
    virtual void print( void );

public :
    void            SetSourceID( char* ID );
    void            SetDestinationID( char* ID );
    void            SetMessageLength( int Len );
    void            SwapID( void );
    UFC::AnsiString GetSourceID( void )        { return UFC::AnsiString( FSourceID, 4 ); }
    UFC::AnsiString GetDestinationID( void )   { return UFC::AnsiString( FDestinationID, 4 ); }
    UFC::AnsiString GetMessageLengthStr( void ){ return UFC::AnsiString( FMessageLength, 4 ); }
    int             GetMessageLength( void );
};
//---------------------------------------------------------------------------
class MessageF010 : public FMessageHeader
{
private :
    char FFileCode[3];
    char FFileSize[8];

public :
    MessageF010( BOOL IsTWSE, char* SourceID,  char* DestinationID,  char* FileCode, int FileSize );
    MessageF010( const char* Data );

public :
    virtual void AsString( char* Buffer );
    virtual void print( void )
    {
        SocketPrintf("----------------- Send Start Message  --------------------\n" );
    }

public :
    UFC::AnsiString GetFileCode( void )   { return UFC::AnsiString( FFileCode, 3 ); }
    UFC::AnsiString GetFileSizeStr( void ){ return UFC::AnsiString( FFileSize, 8 );  }
    int             GetFileSize( void );
};
//---------------------------------------------------------------------------
class MessageF020 : public FMessageHeader
{
private :
    char FFileCode[3];
    char FFileSize[8];

public :
    MessageF020( BOOL IsTWSE, char* SourceID,  char* DestinationID,  char* FileCode, int FileSize );
    MessageF020( const char* Data );

public :
    virtual void AsString( char* Buffer );
    virtual void print( void )
    {
        SocketPrintf("---------------- Receive Start Message -------------------\n" );
    }

public :
    UFC::AnsiString GetFileCode( void )   { return UFC::AnsiString( FFileCode, 3 ); }
    UFC::AnsiString GetFileSizeStr( void ){ return UFC::AnsiString( FFileSize, 8 );  }
    int             GetFileSize( void );
};
//---------------------------------------------------------------------------
class MessageF030 : public FMessageHeader
{
private :
    char FFileCode[3];
    char FEOF; ///< Last package 1 others 0
    char FData[ TWSE_DATA_SIZE_F030 ];

public :
    MessageF030( BOOL IsTWSE, char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode, char FileEOF, char* Data );
    MessageF030( const char* Data );

public :
    virtual void AsString( char* Buffer );
    virtual void print( void );

public :
    UFC::AnsiString GetFileCode( void ){ return UFC::AnsiString( FFileCode, 3 ); }
    void*           GetData( void )    { return FData; }
    char            GetEOF( void )     { return FEOF; }
};
//---------------------------------------------------------------------------
class MessageF210 : public FMessageHeader ///< TCP Ext ver F030
{
private :
    char FFileCode[3];
    char FEOF; ///< Last package 1 others 0
    char FData[ TWSE_DATA_SIZE_F210 ];

public :
    MessageF210( BOOL IsTWSE, char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode, char FileEOF, char* Data );
    MessageF210( const char* Data );
public :
    virtual void AsString( char* Buffer );
    virtual void print( void );
public :
    UFC::AnsiString GetFileCode( void ){ return UFC::AnsiString( FFileCode, 3 ); }
    void*           GetData( void )    { return FData; }
    char            GetEOF( void )     { return FEOF; }
};
//---------------------------------------------------------------------------
class MessageF040 : public FMessageHeader
{
private :
    char FFileCode[3];
    char FEOF;

public :
    MessageF040( BOOL IsTWSE, char* SourceID,  char* DestinationID,  char* FileCode, char DataEOF );
    MessageF040( const char* Data );

public :
    virtual void AsString( char* Buffer );
    virtual void print( void );

public :
    UFC::AnsiString GetFileCode( void ){ return UFC::AnsiString( FFileCode, 3 ); }
    char            GetEOF( void )     { return FEOF; }
};
//---------------------------------------------------------------------------
class MessageF220 : public FMessageHeader ///< TCP Ext ver F040
{
private :
    char FFileCode[3];
    char FEOF;
public :
    MessageF220( BOOL IsTWSE, char* SourceID,  char* DestinationID,  char* FileCode, char DataEOF );
    MessageF220( const char* Data );
public :
    virtual void AsString( char* Buffer );
    virtual void print( void );
public :
    UFC::AnsiString GetFileCode( void ){ return UFC::AnsiString( FFileCode, 3 ); }
    char            GetEOF( void )     { return FEOF; }
};
//---------------------------------------------------------------------------
class MessageF050 : public FMessageHeader
{
private :
    BOOL FHasS10;
    char FFileCode[3];
    char FRequest_Message[TWSE_DATA_SIZE_F050]; //221 bytes for Future
public :
    MessageF050( BOOL IsTWSE, char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode,  char* Request_Message );
    MessageF050( const char* Data );
public :
    virtual void AsString( char* Buffer );
    virtual void print( void );
public :
    UFC::AnsiString GetFileCode( void )       { return UFC::AnsiString( FFileCode, 3 ); }
    UFC::AnsiString GetRequest_Message( void ){ return UFC::AnsiString( FRequest_Message, GetMessageLength()); }
};
//---------------------------------------------------------------------------
class MessageF060 : public FMessageHeader
{
private :
    BOOL FHasS10;
    char FFileCode[3];
    char FRequest_Message[TWSE_DATA_SIZE_F060]; //221 bytes for Future

public :
    MessageF060( BOOL IsTWSE, char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode, char* Request_Message );
    MessageF060( const char* Data );
public :
    virtual void AsString( char* Buffer );
    virtual void print( void );
public :
    UFC::AnsiString GetFileCode( void )       { return UFC::AnsiString( FFileCode, 3 ); }
    char*           GetRequest_Message( void ){ return FRequest_Message; }
};
//---------------------------------------------------------------------------
class MessageF070 : public FMessageHeader
{
public :
    MessageF070( BOOL IsTWSE, char* SourceID,  char* DestinationID );
    MessageF070( const char* Data ):FMessageHeader( Data ){}

public :
    virtual void print( void )
    {
        SocketPrintf("----------------- Send End Message  --------------------\n" );
    }
};
//---------------------------------------------------------------------------
class MessageF080 : public FMessageHeader
{
public :
    MessageF080( BOOL IsTWSE, char* SourceID,  char* DestinationID );
    MessageF080( const char* Data ):FMessageHeader( Data ){}

public :
    virtual void print( void )
    {
        SocketPrintf("---------------- Receive End Message -------------------\n" );
    }
};
//---------------------------------------------------------------------------
// F170 Connection Confirm (Upload)
// F190 Connection Confirm (Download)
//---------------------------------------------------------------------------
class MessageF170 : public MessageHeader
{
public :
    MessageF170( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"20":"92"),"04","08"){}
    MessageF170( const char* Data ):MessageHeader( Data ){}

public :
    virtual void print( void )
    {
        SocketPrintf("---------------- Send Confirm Link Message -------------------\n" );
    }
};
//---------------------------------------------------------------------------
// F180 Reply connection confirm (Upload)
// F200 Reply Connection Confirm (Download)
//---------------------------------------------------------------------------
class MessageF180 : public MessageHeader
{
public :
    MessageF180( BOOL IsTWSE ):MessageHeader( (char*)((IsTWSE)?"20":"92"),"04","09"){}
    MessageF180( const char* Data ):MessageHeader( Data ){}

public :
    virtual void print( void )
    {
        SocketPrintf("---------------- Receive Confirm Link Message -------------------\n" );
    }
};
//-----------------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------


