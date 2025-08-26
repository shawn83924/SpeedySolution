#ifndef CommandListener_H
#define CommandListener_H
//---------------------------------------------------------------------------
#include "../UFC/PClientSocket.h"
#include "../UFC/UDateTime.h"
#include "MessageStore.h"
//---------------------------------------------------------------------------
class TCommandListener : public UFC::PThread, public UFC::SocketClientListener, public TMessageStoreEventHandler
{
private:
    UFC::PClientSocket FSocket;
    UFC::AnsiString    FRemoteIP;
    int                FRemotePort;
    TMessageStore*     FMessageStore;
    long               FReconnectTimes;
    long               FClientHeartbeatInterval;
    long               FTandemHeartbeatInterval;
    long               FClientIdleSec;
    long               FServerIdleSec;

public:
    TCommandListener( const UFC::AnsiString& RemoteIP, int RemotePort, int ClientHeartbeatInterval, int TandemHeartbeatInterval, TMessageStore* pMessageStore );
    virtual ~TCommandListener( void );

private:
    int             ReceiveIntegerFromSocketBuffer( UFC::PClientSocket* Socket );
    UFC::AnsiString ReceiveStringFromSocketBuffer( UFC::PClientSocket* Socket, int Length );
    void            Reconnect( void );
    void            EndSession( void );
    void            HTBT( void );
    void            SVON( int Seq );
    void            SVOF( void );
    void            RESD( int Seq );

public:
    virtual void Execute( void );

public:
    virtual void OnConnect( UFC::PClientSocket * Socket );
    virtual void OnDisconnect( UFC::PClientSocket * Socket, BOOL NeedReconnect = FALSE );
    virtual BOOL OnDataArrived( UFC::PClientSocket * Socket );
    virtual void OnIdle( UFC::PClientSocket * Socket );

public:
    virtual void OnNewReport( UFC::AnsiString* Message, int Seq );

public:
    void SetClientHeartbeatInterval( int Value ) { FClientHeartbeatInterval = Value; }
    void SetTandemHeartbeatInterval( int Value ) { FTandemHeartbeatInterval = Value; }
};
//---------------------------------------------------------------------------
#endif
