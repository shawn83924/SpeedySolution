//---------------------------------------------------------------------------
#ifndef MBUS_SERVER_H
#define MBUS_SERVER_H
//---------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../UFC/iniFile.h"
#include "MTree.h"
#include "MigoHeader.h"
#include "SubjectDB.h"
#include "PUDPClient.h"
#include <queue>
#include "SendingThreadPool.h"
//---------------------------------------------------------------------------
#define UDP_BROADCAST_TIME  10
#define MULTICAST_ADDRESS  "224.5.6.7"
#define VERSION_STR        "2.03"
//---------------------------------------------------------------------------
class AppInfo
{
private:
    UFC::AnsiString FAppName;
    UFC::AnsiString FAppVer;
    UFC::AnsiString FAppDescription;
    BOOL       FIsActive; ///< Active or stand by
public:
    AppInfo( const UFC::AnsiString& Name,
             const UFC::AnsiString& Ver,
             const UFC::AnsiString& Desc,
             BOOL  IsActive = TRUE )
   :FAppName( Name ),
    FAppVer( Ver ),
    FAppDescription( Desc ),
    FIsActive( IsActive ){}
};
//---------------------------------------------------------------------------
class MBusServer : public UFC::SocketServerListener, public UFC::PUDPListener, public SendingThreadPoolListener
{
private:
    UFC::PtrList<UFC::PClientSocket>  FMBusWriteConnectionList; ///< *** MBus "Write" connections list. ***
    UFC::PInt32HashedSet              FMBusReadConnectionSet;   ///< *** MBus "Read connection" FD set. ***
    UFC::PInt32HashedSet              FMBusWriteConnectionSet;  ///< *** MBus "Read connection" FD set. ***
    UFC::PtrList<MString>             FNewIPList;
    UFC::MultiplexingServer*          FTCPServer;            ///< TCP/IP MultiplexingServer
    UFC::PCriticalSection             FMBusReadSocketSetCS;
    UFC::PCriticalSection             FMBusWriteSocketListCS;///< Critical section for server connection list.
    UFC::PCriticalSection             FNewServerListCS;      ///< Critical section for new server list.
    UFC::PUDPClient*                  FUDPClient;            ///< A UDP client
    SubjectsDB                        FClientDB;    ///< Subject DB for local clients.
    SubjectsDB                        FServerDB;    ///< Subject DB for Mbus daemons in other mechines.
    Int32                             FServerID;    ///< The IP address this server used.
    Int32                             FLocalID;     ///< Localhost IP address.(means 127.0.0.1)
    Int32                             FBroadcastCount;
    Int32                             FOutCount;    ///< outgoing message count.
    UFC::PStringHashedSet*            FAcceptIPs;
    UFC::PStringHashedSet*            FAcceptClientIPs;    
    SendingThreadPool*                FSendingThreadPool;
    UFC::BufferedLog*                 FNoSubscribeLog;
private:    
    BOOL IsMBusReadConnections( UFC::PClientSocket* Socket );
    void AddMBusReadConnections( UFC::PClientSocket* Socket );
    void RemoveMBusReadConnections( UFC::PClientSocket* Socket );
    BOOL IsMBusWriteConnections( UFC::PClientSocket* Socket );
private:  ///< for Admin messages.
    UFC::PCriticalSection                              FAppNameCS;
    UFC::PHashedList<Int32,UFC::AnsiString*>           FSocketToAppName;///< Map socket FD to application name.
    UFC::PHashedList<UFC::AnsiString,UFC::AnsiString*> FIPToHostname;   ///< Map IP to host name.
public:
    virtual void OnError( UFC::PClientSocket* SendSocket, UFC::Exception* e );
public:
    /**
     *  Constructor for Migo server.
     */
    MBusServer( UFC::PStringHashedSet* IPs,  UFC::PStringHashedSet* ClientIPs );
    /**
     *  Destructor for Migo server.
     */
    ~MBusServer();
    /**
     *  Start running the Migo server.
     */
    void Start();
    void DeleteSubscriber( UnregistMessage* RegMsg, UFC::PClientSocket* Socket );
    void AddNewAddressIntoNewIPList( const MString& Address );
    void SendQueue( UFC::PClientSocket* Server, const std::string& Data  );
private: ///< Client side function.(Between MBus and local MBus applications)
    void OnRegistSubjects( RegistMessage* RegMsg, UFC::PClientSocket* Socket );
private: ///< Server side function.(Between MBus and MBus)
    /**
     *  Function to send UDP Multicast message to other MBus servers.
     */
    void SendServerStartupNotify( void );
    /**
     *  Function to send register subjects message to other MBus servers.
     */
    void RegistInterestSubjectsToAllServers();
    void RegisterInterestSubjects( UFC::PClientSocket* Socket, BOOL IsFirstTime = FALSE );
    /**
     *  Functions to connect to other MBus.
     */
    BOOL ConnectToServer( UFC::AnsiString& Address );
    void ConnectToNewServers( void );
    void OnMBusConnected( MBusConnectMessage* MBusConnected, UFC::PClientSocket* ClientSocket );
    /**
     *  Functions to maintain server list.
     */
    BOOL RemoveServerSocket( UFC::PClientSocket* Server );
    void AddMBusWriteConnection( UFC::PClientSocket* Server );
    UFC::PClientSocket* GetServerSocket( UFC::PClientSocket* ReadSocket );
    /**
	 *  Functions to foward messages between MBus servers and local MBus applications.
     */
    void Forward( DataMessage * Header, UFC::PClientSocket* ClientSocket );
    void ForwardToClients( DataMessage* Header, const std::string& Data, UFC::PClientSocket* ClientSocket );
    void ForwardToServers( DataMessage* Header, const std::string& Data );
    /**
     *  Functions to handle MBus Admin messages.
     */
    void HandleAdminMsg( AdminMessage* Admin, UFC::PClientSocket* ClientSocket );
    void MakeAdminReplyData( int CMD, MTree& Data );
    void ReplyAdminMessage( DataMessage& DataMsg, UFC::PClientSocket* ClientSocket, BOOL IsLocalClient );
    void SendKillAdminMsgToClient( DataMessage* Header, UFC::PClientSocket* ClientSocket, const UFC::AnsiString& KillName  );
    void SendAdminMsgToServers( AdminMessage* Header );
    void SendMBusAdminMsg( BOOL IsOnline, const UFC::AnsiString& Hostname );
    BOOL OnMAppConnected( ConnectMessage* ConnectedMsg, UFC::PClientSocket* ClientSocket );

    void  ReplyHeartbeatMessage( UFC::PClientSocket* pSocket);
    Int32 FindMaxFDandBuildSet( UFC::PtrList<UFC::PClientSocket>& ClientList, fd_set& set );
    void  RemoveBadSocket( UFC::PtrList<UFC::PClientSocket>& ClientList );
    void  CheckSockets( UFC::PtrList<UFC::PClientSocket>& ClientList );
    void  SendAppStopAdminMsg( UFC::PClientSocket* ClientSocket );
private: /// Implement interface SocketServerListener
    void OnListen( UFC::MultiplexingServer* ServSocket ) { UFC::BufferedLog::Printf(" MBus Server: OnListen"); }
    void OnClientConnect( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket );
    void OnClientDisconnect( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket );
    BOOL OnClientWrite( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket );
    BOOL OnHeartbeat( UFC::PClientSocket* pSocket, BOOL IsRead );
    void OnServerIdle(UFC::MultiplexingServer* ServSocket );
    void LogNoSubscriberMsg( DataMessage* Header );
private:  /// Implement interface PUDPListener
    virtual void OnDataArrived( UFC::PUDPClient* Socket, UFC::MemoryStream* Stream, const MString& Address );
};
//---------------------------------------------------------------------------
#endif // Unit1_H
