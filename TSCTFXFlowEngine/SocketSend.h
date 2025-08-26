//---------------------------------------------------------------------------
#ifndef SocketSendH
#define SocketSendH
//---------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "../UFC/PInt32.h"
#include "../UFC/iniFile.h"
//---------------------------------------------------------------------------
#define FUTOffset 95
#define FUTDealOffset 78
#define NetworkSNoOffset 23
//---------------------------------------------------------------------------
enum CONNECTIONSTAT
{
  st_DISCONNECT,
  st_RECOVER,
  st_CONNECT
};
//---------------------------------------------------------------------------
class Setting
{
public:
    UFC::AnsiString                 WebID;
    UFC::AnsiString                 IPAddress;
    Int32                           Port;
    Int32                           ListenPort;
    Int32                           HBInterval;
public:
    Setting( UFC::Section* pSec );
};
//---------------------------------------------------------------------------
class SocketSend : public UFC::PThread,
                   public UFC::SocketClientListener,
                   public UFC::SocketServerListener
{
private:
    CONNECTIONSTAT                  FClientStat;
    CONNECTIONSTAT                  FServerStat;
    UFC::PtrQueue<UFC::AnsiString>* FJobQueue;
    void                            Execute(void);
    Setting                         *pSet;
public:
    UFC::PClientSocket              FSocket;
    UFC::MultiplexingServer         *FServer;
    UFC::PInt32                     MessageSeq;
    MessageObject*                  FMessageObject;
    char                            FOkMessage[260];
    char                            FAliveMessage[260];
public:
    SocketSend(Setting* TSet, bool FirstTimeReunning, MessageObject* pSigo, UFC::PtrQueue<UFC::AnsiString>* FJobQueue );
    ~SocketSend(){};
public:
    void Run() { Start(); }
    bool CreateSocketObject(void);
    void Work(UFC::AnsiString* SendData);
    void CreateServerObject(void);
    bool CheckConnction(void);
    void SendRejectOrderMessage( const UFC::AnsiString& Order );
    void HeartBeat(void);
    void DequeueFunc(void);
private:
    void OnQuoteExecution( const UFC::AnsiString& Confirm );
    void OnFilledExecution( const UFC::AnsiString& Filled );
    void OnConfirmExecution( const UFC::AnsiString& Confirm );
public:///<Function inherit from MessageListener
    //virtual void        OnMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
public:///<Function inherit from SocketClientListener
    void                OnConnect( UFC::PClientSocket * Socket )    { UFC::BufferedLog::Printf(" Order connection connected. Waiting for AS/400's ack..."); FClientStat = st_RECOVER; }
    void                OnDisconnect( UFC::PClientSocket * Socket ) { UFC::BufferedLog::Printf(" Order connection break."); FClientStat = st_DISCONNECT;}
    BOOL                OnDataArrived( UFC::PClientSocket * Socket ) {}
    void                OnIdle( UFC::PClientSocket * Socket ){}
public:///<Function inherit from SocketServerListener
    void                OnListen( UFC::MultiplexingServer * ServSocket ) ;
    void                OnClientConnect( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket );
    void                OnClientDisconnect( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket ) ;
    BOOL                OnClientWrite( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket ) ;
    void                OnServerIdle( UFC::MultiplexingServer * ServSocket ) {};
    BOOL                OnHeartbeat( UFC::PClientSocket* pSocket, BOOL  ) { return TRUE; }
};

#endif


