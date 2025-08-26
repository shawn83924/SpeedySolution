//---------------------------------------------------------------------------
// SpeedyFTPConnectionObject
//---------------------------------------------------------------------------
#ifndef SpeedyFTPConnectionObject_H
#define SpeedyFTPConnectionObject_H
//---------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../TSECLib/ConnectionObject.h"
#include "../TSECLib/LinkSubSystem.h"
//---------------------------------------------------------------------------
#define stX25Normal        0
#define stX25Handshake     1
#define stX25Lock          2
//---------------------------------------------------------------------------
#ifndef DEF_FTP_STATUS_CODE
#define DEF_FTP_STATUS_CODE
//DocDef
 #define FTP_NORMAL_RESPONSE          0
 #define FTP_ILLEGAL_FILE_CODE        10
 #define FTP_ILLEGAL_EOF_VALUE        11
 #define FTP_ILLEGAL_FILE_SIZE        12
 #define FTP_TIMING_ERROR             13
 #define FTP_FILE_IS_NOT_READY        14
 #define FTP_FILE_IS_READY_BUT_EMPTY   17
 #define FTP_ABORT_BY_INITIATOR        19
 #define FTP_DUPLICATE_ACCESS_REQUEST  79
 #define FTP_ILLEGAL_SUBSYSTEM_NAME    81
 #define FTP_ILLEGAL_FUNCTION_CODE     82
 #define FTP_ILLEGAL_MESSAGE_TYPE      83
 #define FTP_ILLEGAL_MESSAGE_TIME      84
 #define FTP_ILLEGAL_STATUS_CODE       85
 #define FTP_ILLEGAL_SOURCE_ID         86
 #define FTP_ILLEGAL_OBJECT_ID         87
 #define FTP_ILLEGAL_BODY_LENGTH       88
 #define FTP_INTERNAL_ERROR            89
 #define FTP_CALL_COMPUTER_CENTER      99
 #define FTP_WAITING                   -1
 #define FTP_JOB_EXISTS                198
 #define FTP_UNKNOWN_ERROR             199
 #define FTP_DOWNLOAD_FINISHED         200
#endif
//---------------------------------------------------------------------------
class TFtpStatusObject
{
private:
    UFC::AnsiString FFileCode;
    int				FStatusCode;
public:
    TFtpStatusObject( const UFC::AnsiString& FileCode, int StatusCode ) : FFileCode( FileCode ), FStatusCode( StatusCode ) {}
    UFC::AnsiString& GetFileCode( void )   { return FFileCode; }
    int              GetStatusCode( void ) { return FStatusCode; }
};
//---------------------------------------------------------------------------
class TSpeedyFTPConnectionObject : public ConnectionObjectBase, public UFC::PThread
{
private: // member
    LinkSubSystemFCM* X25Connection;
    char			  FX25Stat;
protected:
    ConnectionListener* X25ConnectionListener;
    UFC::AnsiString  FSourceID;
    UFC::AnsiString  FDestinationID;
    long        FIdleTime;
    UFC::AnsiString  FFolderName;
public: // constructor
    TSpeedyFTPConnectionObject( ConnectionParameter& info, UFC::ThreadListener* TListener, ConnectionListener* CListener );
protected: // virtual functions can be overrided
    ///< PThread
    virtual void Execute( void );    
    ///< TSpeedyFTPConnectionObject
    virtual void CustomMessageLoopFunction( void ) {}
    virtual void OnIdle( void ) {}
    virtual void OnReceiveL010( void ) {}
private: // called by this only
    void X25ConnectionHandshake( BOOL ReceivedL010 );
    void MessageLoop( void );
protected: // can only called by child
    BOOL SendMessage( FMessageHeader* FMsg, long TimeoutSec = ORDERSUBSYSTEM_TIMEOUT ); // throw X25RehandshakeException
    MessageHeader* ReceiveMessage( long TimeoutSec = 60/*ORDERSUBSYSTEM_TIMEOUT*/, TMessageType RequireType = mtUnknown ); // throw X25RehandshakeException
    void ReportStatusCode( const UFC::AnsiString& FileCode, int StatusCode );
public:
    void SetFolderName( const UFC::AnsiString& FolderName );
    ///< ConnectionObjectBase
    virtual void Listen( BOOL );
    virtual void Enable( BOOL ){}    
    virtual void Active( void ) { UFC::PThread::Start(); }
    virtual void Close( void ){}
    virtual void OnCommand( int Command, void* Data );
    virtual void UpdateConnectionParameter( ConnectionParameter& NewParam ){};    
};
//---------------------------------------------------------------------------
#endif // SpeedyFTPConnectionObject_H
