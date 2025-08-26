//---------------------------------------------------------------------------
// SpeedyFTPConnectionObject
//---------------------------------------------------------------------------
#ifndef SpeedyFTPConnectionObject_H
#define SpeedyFTPConnectionObject_H

#include "../UFC/UFC.h"
#include "../TAIFEXLib/ConnectionObject.h"
#include "../TAIFEXLib/LinkSubSystem.h"
//---------------------------------------------------------------------------
typedef char TX25ConnectionStat;
const TX25ConnectionStat stX25Normal        = 0;
const TX25ConnectionStat stX25Handshake     = 1;
const TX25ConnectionStat stX25Lock          = 2;
//---------------------------------------------------------------------------
#ifndef DEF_FTP_STATUS_CODE
#define DEF_FTP_STATUS_CODE
//DocDef
const int FTP_NORMAL_RESPONSE           = 0;
const int FTP_ILLEGAL_FILE_CODE         = 10;
const int FTP_ILLEGAL_EOF_VALUE         = 11;
const int FTP_ILLEGAL_FILE_SIZE         = 12;
const int FTP_TIMING_ERROR              = 13;
const int FTP_FILE_IS_NOT_READY         = 14;
const int FTP_FILE_IS_READY_BUT_EMPTY   = 17;
const int FTP_ABORT_BY_INITIATOR        = 19;
const int FTP_DUPLICATE_ACCESS_REQUEST  = 79;
const int FTP_ILLEGAL_SUBSYSTEM_NAME    = 81;
const int FTP_ILLEGAL_FUNCTION_CODE     = 82;
const int FTP_ILLEGAL_MESSAGE_TYPE      = 83;
const int FTP_ILLEGAL_MESSAGE_TIME      = 84;
const int FTP_ILLEGAL_STATUS_CODE       = 85;
const int FTP_ILLEGAL_SOURCE_ID         = 86;
const int FTP_ILLEGAL_OBJECT_ID         = 87;
const int FTP_ILLEGAL_BODY_LENGTH       = 88;
const int FTP_INTERNAL_ERROR            = 89;
const int FTP_CALL_COMPUTER_CENTER      = 99;

//CustDef
const int FTP_WAITING                   = -1;
const int FTP_JOB_EXISTS                = 198;
const int FTP_UNKNOWN_ERROR             = 199;
const int FTP_DOWNLOAD_FINISHED         = 200;
#endif
//---------------------------------------------------------------------------
class SpeedyObjectsManager;
//---------------------------------------------------------------------------
class TFtpStatusObject
{
private:
    UFC::AnsiString FFileCode;
    int FStatusCode;

public:
    TFtpStatusObject( const UFC::AnsiString& FileCode, int StatusCode )
    :FFileCode( FileCode ), FStatusCode( StatusCode ) {}

    UFC::AnsiString GetFileCode( void ) { return FFileCode; }
    int GetStatusCode( void ) { return FStatusCode; }
};
//---------------------------------------------------------------------------
class TSpeedyFTPConnectionObject : public ConnectionObjectBase, public UFC::PThread
{
private: // member
    LinkSubSystemFCM*     X25Connection;
    TX25ConnectionStat    FX25Stat;
protected:
    ConnectionListener*   X25ConnectionListener;
    SpeedyObjectsManager* FObjectManager;    
    UFC::AnsiString       FSourceID;
    UFC::AnsiString       FDestinationID;
    UFC::AnsiString       FFolderName;
    long                  FIdleTime;
    BOOL                  FIsOptions;   
public: // constructor
    TSpeedyFTPConnectionObject( SpeedyObjectsManager* ObjManager,
                                ConnectionParameter& info, 
                                UFC::ThreadListener* TListener, 
                                ConnectionListener* CListener, 
                                BOOL IsOpt );
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
    const UFC::AnsiString& GetSourceID( void )       { return FSourceID; }
    const UFC::AnsiString& GetFDestinationID( void ) { return FDestinationID; }
    void SetFolderName( const UFC::AnsiString& FolderName );
    ///< ConnectionObjectBase
    virtual void Active( void ) { UFC::PThread::Start(); }
    virtual void Close() { Stop(); }
    virtual void Listen( BOOL );
    virtual void OnCommand( int Command, void* Data );
    virtual BOOL IsReady( void ) { return X25Connection->IsReady(); }
    virtual void UpdateConnectionParameter( ConnectionParameter& NewParam ){};
};
//---------------------------------------------------------------------------
#endif // SpeedyFTPConnectionObject_H
