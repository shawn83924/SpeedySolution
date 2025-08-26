//---------------------------------------------------------------------------

#ifndef SocketSendH
#define SocketSendH

#include "PClientSocket.h"
#include "PThread.h"
#include "List.h"
#include "Sigo.h"
#include "MTree.h"
#include "MultiplexingServer.h"
#include "PInt32.h"
#include "UDateTime.h"
#include "iniFile.h"
#include "TParseData.h"
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
    UFC::AnsiString                 HostAddress;
    UFC::AnsiString                 LocalIP;
    Int32                           Port;
    Int32                           ListenPort;
    Int32                           HBInterval;
    Int32                           ConnectInterval;
public:
    Setting( UFC::Section *iniSection );
};
//---------------------------------------------------------------------------
class SocketSend : public UFC::PThread,
                   public UFC::SocketClientListener,
                   public UFC::SocketServerListener
{
private:
    CONNECTIONSTAT                      FStat;
    CONNECTIONSTAT                      SStat;
    UFC::PtrQueue<UFC::AnsiString>*     JobQueue;
    void                                Execute(void);
    Setting                             *pSet;
    BOOL                                TestingMode;
public:
    UFC::PClientSocket                  FSocket;
    UFC::MultiplexingServer             *FServer;
    UFC::PInt32                         PInt;
    MessageObject*                      FMessageObject;
    char                                FOkMessage[260];
    char                                FAliveMessage[260];
public:
    SocketSend(Setting* pSet, bool FirstTimeReunning, MessageObject* pSigo, UFC::PtrQueue<UFC::AnsiString>* FJobQueue, BOOL Test);
    ~SocketSend(){};
public:
    void                Run() { Start(); }
    bool                CreateSocketObject(void);
    void                Work(UFC::AnsiString* SendData);
    void                CreateServerObject(void);
    bool                CheckConnction(void);
    void                Recover( UFC::PClientSocket * Socket);
    void                ReceiveData( UFC::PClientSocket * Socket);
    void                SendRejectOrderMessage( const UFC::AnsiString& );
    void                HeartBeat(void);
    void                DequeueFunc(void);
    Int32               GetConfirmNID( char* Buffer);
    bool                CheckConfirmWID( char* Buffer);
    bool                CheckFillWID( char* Buffer);
    int                 RejectNID( const UFC::AnsiString& Buffer );
    void                DequeueSend(void);
    int                 GetOrderType( const UFC::AnsiString& Buffer );
    int                 GetClOrderID( const UFC::AnsiString& Buffer );
private:
    void                OnStockExecution( char* Buffer );
public:
    void                OnMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
public:///<Function inherit from SocketClientListener
    void                OnConnect( UFC::PClientSocket * Socket )    { UFC::BufferedLog::Printf(" Connected to server");FStat = st_CONNECT; }
    void                OnDisconnect( UFC::PClientSocket * Socket ) { UFC::BufferedLog::Printf(" Disonnected"); FStat = st_DISCONNECT;}
    BOOL                OnDataArrived( UFC::PClientSocket * Socket ) {};
    void                OnIdle( UFC::PClientSocket * Socket ){}
public:///<Function inherit from SocketServerListener
    void                OnListen( UFC::MultiplexingServer * ServSocket ) ;
    void                OnClientConnect( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket );
    void                OnClientDisconnect( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket ) ;
    BOOL                OnClientWrite( UFC::MultiplexingServer * ServSocket, UFC::PClientSocket * ClientSocket ) ;
    void                OnServerIdle( UFC::MultiplexingServer * ServSocket ) {};
    BOOL                OnHeartbeat( UFC::PClientSocket* pSocket, BOOL IsRead  ) { return TRUE; }
};

#endif


