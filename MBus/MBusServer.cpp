//-----------------------------------------------------------------------------------------------
#include "MBusServer.h"
#include "MApp.h"
#include "../Version.h"
//------------------------------------------------------------------------------------------------------------------
extern BOOL  BENCHMARK_MODE;
extern BOOL  DAEMON_MODE;
extern BOOL  SILENCE_MODE;
extern BOOL  PERMANENT;
extern Int32 MBUS_PORT;
extern Int32 MCAST_TTL;
extern Int32 MCAST_PORT;
extern Int32 RECV_THREAD;
extern Int32 SEND_THREAD;
extern Int32 MAX_CONNECTIONS;
extern Int32 THREAD_SWITCH_MS;
extern Int32 HEARTBEAT_INTER;
extern UFC::PStringHashedSet MonitorSubjects;
//------------------------------------------------------------------------------------------------------------------
const  Int32 IDLE_TIME        = 1000; ///< 1sec
const  Int32 IDLE_TWO_MINUTES = 120;  ///< App wil close after 2 minutes
Int32  TOTAL_IDLE_TIME        = 0;
//------------------------------------------------------------------------------------------------------------------
//  Constructor
//------------------------------------------------------------------------------------------------------------------
MBusServer::MBusServer( UFC::PStringHashedSet* IPs, UFC::PStringHashedSet* ClientIPs )
:FTCPServer( NULL )
,FUDPClient( NULL )
,FServerID( UFC::PSocket::GetLocalIPAddress())
,FLocalID( (Int32)inet_addr( "127.0.0.1" ) )
,FBroadcastCount( 0 )
,FOutCount( 0 )
,FAcceptIPs( IPs )
,FAcceptClientIPs( ClientIPs )
{
    ///< Create directory for MBus error log.
    UFC::CreateDir( "MBusErr" );
    ///< Create error log object.
    FNoSubscribeLog = new UFC::BufferedLog( "MBus No Subscribe Log",
                                            "bin/MBusErr",
                                            "MBusNoSubscribe",
                                            "txt",
                                            10240, TRUE );                    
}
//------------------------------------------------------------------------------------------------------------------
//  Destructor
//------------------------------------------------------------------------------------------------------------------
MBusServer::~MBusServer()
{
    /// Delete the UDP client.
    if( FUDPClient != NULL )
        delete FUDPClient;
    /// Delete the TCP multiplexing server.
    if( FTCPServer != NULL )
        delete FTCPServer;
    UFC::PLockObject    LockObj( FMBusWriteSocketListCS );
    /// Remove all server connections
    for( Int32 i = FMBusWriteConnectionList.ItemCount() - 1; i >= 0; i-- )
         delete FMBusWriteConnectionList.GetItem( i );
    if (FNoSubscribeLog != NULL)
        delete FNoSubscribeLog;        
}
//------------------------------------------------------------------------------------------------------------------
//  Send UDP data let other server to become aware of this server is running.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::SendServerStartupNotify( void )
{
    if( SILENCE_MODE == TRUE )///< Slicence mode on. don't send the muticast message.
    {
        UFC::SleepMS( 1000 );
        return;
    }
    MigoHeader        Header( ACK_NEW_SERVER_UP, 0, 0 );
    UFC::MemoryStream Stream( MIGO_HEADER_SIZE );

    Header.SaveToStream( &Stream );
    try
    {
        if( FBroadcastCount < UDP_BROADCAST_TIME ) ///< During the UDP_BROADCAST_TIME.
        {
            for( int i = 0; i < 10; i++ ) ///< Multicast IP address, 10 times/sec.
            {
                FUDPClient->SendTo( MULTICAST_ADDRESS, MCAST_PORT, (char*)Stream.GetBuffer(), MIGO_HEADER_SIZE );
                UFC::SleepMS( 100 );
            }
            FBroadcastCount++;
        }
        else
        {
            if( FBroadcastCount % 10 == 0 )
                FUDPClient->SendTo( MULTICAST_ADDRESS, MCAST_PORT, (char*)Stream.GetBuffer(), MIGO_HEADER_SIZE );
            FBroadcastCount++;
            UFC::SleepMS( 1000 );
        }
    }
    catch( UFC::SocketException&  )
    {
        UFC::SleepMS( 1000 );
    }
}
//------------------------------------------------------------------------------------------------------------------
// Send data to socket Queue
// and add this socket to sending thread pool
//------------------------------------------------------------------------------------------------------------------
inline void MBusServer::SendQueue( UFC::PClientSocket* Socket, const std::string& Data  )
{
    if( Socket->GetUserData() != -1 ) ///< -1 means this socket had been deleted.
    {
        Socket->SendQueue( Data ); ///< Add data to queue
        FSendingThreadPool->AddJob( Socket ); ///< Add to thread pool.
    }	
}
//------------------------------------------------------------------------------------------------------------------
// Is this client socket a MBus read socket?
//------------------------------------------------------------------------------------------------------------------
BOOL MBusServer::IsMBusReadConnections( UFC::PClientSocket* Socket )
{
    UFC::PLockObject Lock( FMBusReadSocketSetCS );
    
    return FMBusReadConnectionSet.Exists( Socket->GetHandle() );
}
//------------------------------------------------------------------------------------------------------------------
// Add a MBus read socket to set.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::AddMBusReadConnections( UFC::PClientSocket* Socket )
{
    UFC::PLockObject Lock( FMBusReadSocketSetCS );
    
    FMBusReadConnectionSet.Add( Socket->GetHandle() );
}
//------------------------------------------------------------------------------------------------------------------
// Remove a MBus read socket from set.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::RemoveMBusReadConnections( UFC::PClientSocket* Socket )
{
    UFC::PLockObject Lock( FMBusReadSocketSetCS );
    
    FMBusReadConnectionSet.Delete( Socket->GetHandle() );
}    
//------------------------------------------------------------------------------------------------------------------
// Send interest subjects to all other MBus daemons.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::RegistInterestSubjectsToAllServers()
{
    RegistMessage RegMsg( 0, IS_SERVER );

    FClientDB.GetInterest( RegMsg ); ///< Get all interest subjects.
    std::string DataString =  RegMsg.ToString();
    UFC::PLockObject    LockObj( FMBusWriteSocketListCS );
    ///< Send regist subjects message to all other MBus server.
    for( register int i = FMBusWriteConnectionList.ItemCount() - 1; i >= 0; --i )
         SendQueue( FMBusWriteConnectionList.GetItem( i ), DataString );
}
//------------------------------------------------------------------------------------------------------------------
// Send interest subjects to a MBus daemon.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::RegisterInterestSubjects( UFC::PClientSocket* Server, BOOL IsFirstTime )
{
     Int32 Flag = IS_SERVER;

     if( IsFirstTime == TRUE )
         Flag |= IS_FIRST_TIME;
     RegistMessage RegMsg( 0 , Flag );

     FClientDB.GetInterest( RegMsg ); ///< Get all interest subjects.
     SendQueue( Server, RegMsg.ToString() ); ///< Send to MBus
}
//------------------------------------------------------------------------------------------------------------------
BOOL MBusServer::IsMBusWriteConnections( UFC::PClientSocket* Socket )
{
    UFC::PLockObject Lock( FMBusWriteSocketListCS );
    
    return FMBusWriteConnectionSet.Exists( Socket->GetHandle() );
}
//------------------------------------------------------------------------------------------------------------------
//  Add a server socket to Server "Write" connection list.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::AddMBusWriteConnection( UFC::PClientSocket* Server )
{
   UFC::PLockObject    LockObj( FMBusWriteSocketListCS );

   FMBusWriteConnectionList.Add( Server );       ///< Add this socket to server list.
   FMBusWriteConnectionSet.Add( Server->GetHandle() );
}
//------------------------------------------------------------------------------------------------------------------
//  Delete a server socket to Server connection list.
//------------------------------------------------------------------------------------------------------------------
BOOL MBusServer::RemoveServerSocket( UFC::PClientSocket* Server )
{
    UFC::PClientSocket* Socket;
    UFC::PLockObject    LockObj( FMBusWriteSocketListCS );
    
    if( FMBusWriteConnectionSet.Exists( Server->GetHandle() ) )
    {
        FMBusWriteConnectionSet.Delete( Server->GetHandle() );
        try
        {
            for( register int i = FMBusWriteConnectionList.ItemCount() - 1; i >= 0; --i )
            {
                if( (Socket = FMBusWriteConnectionList.GetItem( i )) == Server )
                {
                    FServerDB.RemoveConnection( Socket );  ///< Remove all it's interest subjects.
                    FMBusWriteConnectionList.Delete( i );
                    FTCPServer->ToTrashcan( Socket, 5 ); ///< Delete the server socket after 5 min.
                    return TRUE;
                }
            }
        }
        catch( UFC::SocketException& e )
        {
            UFC::BufferedLog::Printf( " Removing a server connection failed. %s", e.what() );
            return FALSE;
        }
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
//  Try to establish a connection to a MBus daemon ( for Write)
//  return TRUE for succeeds FALSE if failed.
//------------------------------------------------------------------------------------------------------------------
BOOL MBusServer::ConnectToServer( UFC::AnsiString& Address )
{
    UFC::PClientSocket* Server = NULL;
    try
    {
        Server = new UFC::PClientSocket( Address, MBUS_PORT, FALSE );///< Create a client socket
        Server->SetCheckHeartbeatType( UFC::htCheckWrite );          ///< Write socket only need to check write I/O.
        Server->OpenSocket();                                        ///< Open socket.
        Server->Connect( 5 );                                        ///< Connect to another server.
        Server->SetTCPNoDelay( TRUE );                               ///< Turn off the Nagle algorithem
        Server->SetBlockMode( FALSE );                               ///< Set to Non-blocking mode.
        Server->SetBufferSize( SO_RCVBUF, 1048576 );                 ///< 1024K Solaris default
        Server->SetBufferSize( SO_SNDBUF, 1048576 );                 ///<   64k AIX default 
        AddMBusWriteConnection( Server );                            ///< Add this socket to server list.
        SendQueue( Server, MBusConnectMessage( 0, UFC::Hostname ).ToString() ); ///< Send "MBus server connected" ack.
        UFC::BufferedLog::Printf( " - Write Socket to MBus@%s established.", Address.c_str() );
        return TRUE;
    }
    catch( UFC::SocketException& e )
    {
        if( Server != NULL )
            delete Server;
        UFC::BufferedLog::Printf( " Try to establish write socket to MBus:%s failed, reason:%s", Address.c_str(), e.what() );
        return FALSE;
    }
}
//------------------------------------------------------------------------------------------------------------------
// Is there any IP address in FNewIPList?
//
// if exists, create connections to MBus servers.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::ConnectToNewServers( void )
{
    MString*            Address;
    MString*            Host;
    Int32               ListCount = FNewIPList.ItemCount();

    for( Int32 i = ListCount - 1; i >= 0; i-- )///< Is there any new MBus ?
    {
        if( (Address = FNewIPList.GetItem( i )) != NULL )
        {
            if( ConnectToServer( *Address ) == TRUE )
            {
                UFC::PLockObject Lock( FNewServerListCS );
                
                FNewIPList.Delete( i );  ///< Remove this server address from NewIPList                
                if( ( Host = FIPToHostname.GetObjectByKey( *Address ) )!= NULL )
                {
                    UFC::BufferedLog::Printf( " MBus@%s startup, Send notify to all clients.", Host->c_str() );
                    SendMBusAdminMsg( TRUE, *Host );
                }
                delete Address;          ///< free the IP string object.
            }
        }
    }
}
//------------------------------------------------------------------------------------------------------------------
// Add a server IP address string to FNewIPList
//
// Before add to list, make sure this IP address is not in the FNewIPList
// and the connection is not established.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::AddNewAddressIntoNewIPList( const MString& Address )
{
    register Int32 i;

    if( FAcceptIPs->ItemCount() > 0 && FAcceptIPs->Exists(Address) == FALSE )
        return; ///< Not accept this IP.
    {
        UFC::PLockObject    NewServerLock( FNewServerListCS );
        for( i = FNewIPList.ItemCount() - 1; i >= 0; --i ) ///< Is this address already in FNewIPlist ?
             if( *FNewIPList.GetItem(i) == Address )
                 return; ///< Yes! already exists.
    }
    {
        UFC::PLockObject    ServerLock( FMBusWriteSocketListCS );
        for( i = FMBusWriteConnectionList.ItemCount() - 1; i >= 0; --i )
             if( FMBusWriteConnectionList.GetItem( i )->GetPeerIPAddress() == Address )///< Is this server already connected ?
                 return; ///< Yes! connection already exists.
    }
    UFC::BufferedLog::Printf(" Find a new server IP:%s",Address.c_str());
    FNewIPList.Add( new MString( Address ) ); ///< Add this IP address to FNewIPList.
}
//------------------------------------------------------------------------------------------------------------------
// Generate the reply data
// according to the given Admin message.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::MakeAdminReplyData( int CMD, MTree& Data )
{
    if( CMD == ADMIN_ASK_PROCESS )///< Ask for all connected clients.
    {
        Data.append( "CMD", ADMIN_REPLY_PROCESS );
        Data.append( "HOST", UFC::Hostname );
        UFC::PLockObject Lock( FAppNameCS );

        Data.append( "COUNT", FSocketToAppName.ItemCount() );
        for( register int i = 0; i < FSocketToAppName.ItemCount(); i++ )
        {
             MString  Name( i );
             MString* AppName = FSocketToAppName.GetItem( i );
             if( AppName != NULL )
                 Data.append( Name, *AppName );
        }
    }
    else if( CMD == ADMIN_ASK_MBUS)///< Ask for all connected MBuss.
    {
        UFC::PtrList<UFC::PClientSocket>  ServerList;
        UFC::PClientSocket*               Server;
        Int32                             ServerCount = 1;
        MString*                          Host;

        Data.append( "CMD", ADMIN_REPLY_MBUS );
        Data.append( "HOST", UFC::Hostname );
        FServerDB.All( &ServerList );

        Data.append( "0", UFC::Hostname ); ///< This MBus.
        for( register int i = 0; i < ServerList.ItemCount(); i++ )
        {
            if( (Server = ServerList.GetItem( i )) != NULL )
            {
                if( ( Host = FIPToHostname.GetObjectByKey( Server->GetPeerIPAddress() ) )!= NULL )
                {
                    MString  Name( ServerCount );
                    Data.append( Name, *Host );
                    ServerCount++;
                }
            }
        }
        Data.append( "COUNT", ServerCount );
    }
}
//------------------------------------------------------------------------------------------------------------------
//  Reply the Admin message to the given MBus client.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::ReplyAdminMessage( DataMessage& DataMsg, UFC::PClientSocket* ClientSocket, BOOL IsLocalClient )
{
    if( IsLocalClient == FALSE  )
    {
        UFC::PClientSocket* Server;
        if( (Server = GetServerSocket( ClientSocket ))!= NULL )
             SendQueue( Server, DataMsg.ToString()  );
    }
    else
        SendQueue( ClientSocket, DataMsg.ToString()  );
}
//------------------------------------------------------------------------------------------------------------------
//  Send the Admin message to other MBus daemons.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::SendAdminMsgToServers( AdminMessage* Header )
{
    std::string         DataString = Header->ToString();
    UFC::PLockObject    LockObj( FMBusWriteSocketListCS );

    for( register Int32 i = FMBusWriteConnectionList.ItemCount() - 1; i >= 0; i-- )
         SendQueue( FMBusWriteConnectionList.GetItem( i ), DataString  );
}
//------------------------------------------------------------------------------------------------------------------
void MBusServer::SendKillAdminMsgToClient( DataMessage* Header, UFC::PClientSocket* ClientSocket, const UFC::AnsiString& KillName  )
{
    UFC::PtrList<UFC::PClientSocket>    ClientList;
    UFC::PClientSocket*                 Client;
    std::string                         RowData = Header->ToString();

    FClientDB.Search( &ClientList, Header );      ///< Serch the clients who interest this message.
    for( register Int32 i = ClientList.ItemCount() - 1; i >= 0; i-- )
    {
        if( (Client = ClientList.GetItem( i )) != NULL )
        {
            if( ClientSocket !=  Client ) ///< Don't send to the sender!
            {
                UFC::AnsiString* AppName;
                UFC::PLockObject Lock( FAppNameCS );

                if( (AppName = FSocketToAppName.GetObjectByKey( Client->GetHandle() )) != NULL &&
                     AppName->AnsiCompare( KillName ) == 0   )
                {
                    SendQueue( Client, RowData  );
                    UFC::BufferedLog::Printf(" Send MBus Kill Message to[%s] ", KillName.c_str() );
                    return;
                }
            }
        }
    }
}
//------------------------------------------------------------------------------------------------------------------
// When MBus server receive a Admin message
// call this function to handle this Admin message.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::HandleAdminMsg( AdminMessage* Admin, UFC::PClientSocket* ClientSocket )
{
     if( strcmp( Admin->getKey().c_str(), UFC::Hostname ) == 0 ) ///< Ask me!!
     {
         int   CMD;
         MTree Data,ReplyData;

         Admin->getData( Data ); ///< Extract the MTree data object from the Admin Message
         if( Data.get( "CMD", CMD ) )
         {
             if( CMD == ADMIN_KILL_PROCESS )
             {
                UFC::AnsiString AppName;
                if( Data.get( "APP", AppName ) )
                {
                    DataMessage DataMsg( FServerID, ADMIN_SUBJECT, UFC::Hostname, Data );

                    SendKillAdminMsgToClient( &DataMsg, ClientSocket, AppName );
                }
             }
             else
             {
                MakeAdminReplyData( CMD, ReplyData ); ///< Generate the reply message data.
                DataMessage DataMsg( 0, ADMIN_SUBJECT, "all", ReplyData );
                if( Admin->GetSenderID( ) == 0 ) ///< Request from the app connected to this MBus.
                     ReplyAdminMessage( DataMsg, ClientSocket, TRUE ); ///<Reply to the local connection
                else
                     ReplyAdminMessage( DataMsg, ClientSocket, FALSE );///< Reply to the other MBus daemon.
             }
         }
     }
     else if( Admin->GetSenderID( ) == 0 ) //< This Admin message generated from the local publisher.
     {                                      //< want ot ask other MBus deamon.
          Admin->SetSenderID( FServerID );
          SendAdminMsgToServers( Admin );        ///< Forward to other MBus.
     }
}
//------------------------------------------------------------------------------------------------------------------
// When a local MBus application connected.
// call this function to send 'Application online' Admin message to other MBus
//------------------------------------------------------------------------------------------------------------------
BOOL MBusServer::OnMAppConnected( ConnectMessage* ConnectedMsg, UFC::PClientSocket* ClientSocket )
{
    UFC::AnsiString AppName = ConnectedMsg->getAppName();
    MTree           Data,ExistsData;
    Int32           CMD = ADMIN_APP_FIRST_ONE;
    Int32           PeerID = ClientSocket->GetPeerID();    

    UFC::BufferedLog::Printf( " MBus Application \"%s\" startup, FD:%d", AppName.c_str(), ClientSocket->GetHandle() );
    if( PeerID != FServerID && PeerID != FLocalID ) ///< Not a local connection?        
    {
        UFC::AnsiString Address = ClientSocket->GetPeerIPAddress( ); 
        if( FAcceptClientIPs->ItemCount() > 0 && FAcceptClientIPs->Exists(Address) == FALSE )        
        {
            UFC::BufferedLog::Printf(" IP:%s not in ACL, disconnect it.", Address.c_str() );                
            return FALSE;
        }
    }   
    ///< Send a connect confirm acknowledgement.
    SendQueue( ClientSocket, MigoHeader( ACK_CONNECT_CONFIRM, HEARTBEAT_INTER, 0  ).ToString()  );
    ///< Send the Application online message to other MBus application.
    Data.append( "CMD",  ADMIN_APP_ONLINE );
    Data.append( "APP",  AppName );
    Data.append( "HOST", UFC::Hostname );
    DataMessage DataMsg( 0, "MBUS", "all", Data );
    Forward( &DataMsg, ClientSocket );
    ///< Add the socket handle to application name hash map.
    UFC::AnsiString* ExistName;
    UFC::PLockObject Lock( FAppNameCS );

    for( register int i = 0; i < FSocketToAppName.ItemCount(); i++ )
    {
        ExistName = FSocketToAppName.GetItem( i );
        if( ExistName != NULL && AppName.AnsiCompare( *ExistName ) == 0 )
        {
            CMD = ADMIN_APP_EXISTS;
            break;
        }
    }
    FSocketToAppName.Add( ClientSocket->GetHandle(), new UFC::AnsiString( AppName ) );
    ///< Send the Application online message back to this MBus application.
    ///< tell the app is there any same app connected.
    ExistsData.append( "CMD",  CMD );
    ExistsData.append( "APP",  AppName );
    ExistsData.append( "HOST", UFC::Hostname );
    DataMessage AdminMsg( 0, "MBUS", UFC::Hostname, ExistsData );
    SendQueue( ClientSocket, AdminMsg.ToString() );
     return TRUE;
}
//------------------------------------------------------------------------------------------------------------------
// When MBus server receive a message, call this function to forward
// to clients and other Mbus.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::Forward( DataMessage* Header, UFC::PClientSocket* ClientSocket )
{

    if( Header->GetSenderID( ) == 0 )         ///< This message is generated from the publisher dirrect connect to this server.
    {                                         ///< need forward to other servers and clients who interest in this message.
        Header->SetSenderID( FServerID );     ///< Set the SenderID to the ID of this MBus server.
        std::string Data = Header->ToString();

        ForwardToServers( Header, Data );               ///< Forward to other MBus.
        ForwardToClients( Header, Data, ClientSocket ); ///< foward to clients.
    }
    else
    {
        std::string Data = Header->ToString();
        ForwardToClients( Header, Data, ClientSocket ); ///< foward to clients.        
    }
}
//------------------------------------------------------------------------------------------------------------------
void MBusServer::LogNoSubscriberMsg( DataMessage* Header )
{
    UFC::AnsiString Str,Now,NodeString;
    MTree           Data;
    MTreeEnumerator* pTreeEnum;
    MNode* pNode;
    void*  pItem;

    UFC::GetTimeString( Now, TRUE );
    Str.Printf(" ---------------------%s-----------------------",Now.c_str());
    FNoSubscribeLog->WriteString( Str );
    Str.Printf(" Subject[%s]",Header->getSubject().c_str());
    FNoSubscribeLog->WriteString( Str );
    Str.Printf(" Key[%s]",Header->getKey().c_str());
    FNoSubscribeLog->WriteString( Str );
    Header->getData( Data );
    pTreeEnum = Data.newEnumerator();
    pTreeEnum->reset();
    while( (pNode = (MTree*)pTreeEnum->next()) != NULL )
    {
	MDataType mType = pNode->getType();
	MString   name  = pNode->getName();
	pItem = (void*)pNode->getData();
        switch (mType)
        {
            case M_STRING: NodeString.Printf("[String] %s:%s", name.c_str(), (char*) pItem);
                break;
            case M_BOOLEAN:NodeString.Printf("[Boolean] %s:%s", name.c_str(), (*((int*) pItem) == 0 ? "False" : "True"));
                break;
            case M_INT: NodeString.Printf("[Int] %s:%d", name.c_str(), *((int*) pItem));
                break;
            case M_UINT: NodeString.Printf("[Unsigned Int] %s:%d", name.c_str(), *((unsigned int*) pItem));
                break;
            case M_SHORT: NodeString.Printf("[Short]%s:%d", name.c_str(), *((short*) pItem));
                break;
            case M_USHORT: NodeString.Printf("[Unsigned Short]%s:%d", name.c_str(), *((unsigned short*) pItem));
                break;
            case M_LONG: NodeString.Printf("[Long]%s:%d", name.c_str(), *((long*) pItem));
                break;
            case M_ULONG: NodeString.Printf("[Unsigned Long]%s:%d", name.c_str(), *((unsigned long*) pItem));
                break;
            case M_FLOAT: NodeString.Printf("[Float]%s:%f", name.c_str(), *((float*) pItem));
                break;
            case M_DOUBLE: NodeString.Printf("[Double]%s:%f", name.c_str(), *((double*) pItem));
                break;
            case M_ROWDATA:UFC::BinaryToHexString((UInt8*) pItem, pNode->getSize(), Str);
                NodeString.Printf("[Row Data]%s:%s", name.c_str(), Str.c_str());
                break;
            default: NodeString.Printf("Unknown Data Type:%d.\n", mType);
                break;
        }
	FNoSubscribeLog->WriteString( NodeString );
    }
    Str.Printf(" --------------------------------------------------------");
    FNoSubscribeLog->WriteString( Str );
}
//------------------------------------------------------------------------------------------------------------------
// Forward message to clients which direct connect to this server.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::ForwardToClients( DataMessage* Header, const std::string& Data, UFC::PClientSocket* ClientSocket  )
{
    UFC::PtrList<UFC::PClientSocket>    ClientList;
    UFC::PClientSocket*                 Client;
    int                                 Sent = 0;

    FClientDB.Search( &ClientList, Header );      ///< Serch the clients who interest this message.
    for( register Int32 i = ClientList.ItemCount() - 1; i >= 0; i-- )
    {
        if( (Client = ClientList.GetItem( i )) != NULL )
        {
            if( ClientSocket !=  Client ) ///< Don't send to the sender!
            {
                SendQueue( Client, Data  );
                FOutCount++;
                Sent++;
            }
        }
    }
    if( Sent == 0 && MonitorSubjects.ItemCount() > 0) ///< No one subscribe this message.
    {
        if( MonitorSubjects.Exists( Header->getSubject() ) )
            LogNoSubscriberMsg( Header );
    }
}
//------------------------------------------------------------------------------------------------------------------
// Forward message to other MBus servers.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::ForwardToServers( DataMessage* Header, const std::string& Data )
{
    UFC::PtrList<UFC::PClientSocket>  ServerList;
    UFC::PClientSocket*               Server;

    FServerDB.Search( &ServerList, Header );      ///< Serch the MBus who interest this message.
    for( register Int32 i = ServerList.ItemCount() - 1; i >= 0; i-- )
    {
        if( (Server = ServerList.GetItem( i )) != NULL )
        {
            SendQueue( Server, Data );
            FOutCount++;
        }
    }
}
//------------------------------------------------------------------------------------------------------------------
// Call this function when received RegistMessage
//------------------------------------------------------------------------------------------------------------------
void MBusServer::OnRegistSubjects( RegistMessage* RegMsg, UFC::PClientSocket* Socket )
{
    if( RegMsg->IsFromServer() == TRUE ) ///< It's a  RegistMessage sent by other MBus ?
    {
        UFC::PClientSocket* ServerSocket;
        if( (ServerSocket = GetServerSocket( Socket ))!= NULL ) ///< find the write socket.
        {
            FServerDB.RemoveConnection( ServerSocket );   ///< Remove all it's interest subjects.
            if( RegMsg->ItemCount() > 0 )                 ///< Change the MBus subjects DB
            {
                FServerDB.Regist( RegMsg, ServerSocket ); ///< Add all interest subjects into DB.
                if( UFC::BufferedLog::DebugLevelExist(UFC::dlInformation ) == TRUE )
                {
                    UFC::BufferedLog::Printf(" -------------------MBus@%s--------------------", ServerSocket->GetPeerIPAddress().c_str() );
                    for( int i = 0; i < RegMsg->ItemCount(); i++ )
                         UFC::BufferedLog::Printf( " Subject:%s Key:%s",RegMsg->getSubject( i ).c_str(),RegMsg->getKey(i).c_str());
                    UFC::BufferedLog::Printf(" ------------------------------------------------------" );
                }
            }            
            if( RegMsg->IsFirstTime() == TRUE )           ///< If it's the first time received the reg info.
            {
                UFC::BufferedLog::Printf( " - First time received interested subjects from MBus@%s", ServerSocket->GetPeerIPAddress().c_str() );
                UFC::BufferedLog::Printf( "   Send the interested subjects back." );
                RegisterInterestSubjects( ServerSocket ); ///< Send the reg info back.
            }
        }
        else
            UFC::BufferedLog::Printf(" Server %s not found.", Socket->GetPeerIPAddress().c_str() );
    }
    else ///< It's a RegistMessage sent by client.
    {
         FClientDB.Regist( RegMsg, Socket );   ///< Regist a subject in the client DB.
         RegistInterestSubjectsToAllServers(); ///< Regist all interesting subjects to other MBus servers.
    }
}
//------------------------------------------------------------------------------------------------------------------
// Reply heartbeat message.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::ReplyHeartbeatMessage( UFC::PClientSocket* pSocket)
{
    if( IsMBusReadConnections( pSocket ) )///< It's a MBus connection ?
    {
         UFC::PClientSocket* Target = GetServerSocket( pSocket );
         if( Target != NULL ) ///< Find the MBus Write socket.
             SendQueue( Target, MigoHeader(  ACK_HEARTBEAT_CONFIRM  ).ToString()  );
         else ///< Can't find the write socket!
             throw UFC::SocketException( "The MBus write socket was broken" );
    }
    else  ///< Send heartbeat message to client.
    {
         UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Reply Heartbet to Client@%s FD[%d]", pSocket->GetPeerIPAddress().c_str(), pSocket->GetHandle() );
         SendQueue( pSocket, MigoHeader(  ACK_HEARTBEAT_CONFIRM  ).ToString()  );
    }
}
//------------------------------------------------------------------------------------------------------------------
//  Get the MBus write socket from the read socket
//
//  There are two socket connections between two MBus server
//  one for read (Subscriber) and the other for write (Publisher)
//  use this function to find the write connection by the given read socket.
//------------------------------------------------------------------------------------------------------------------
UFC::PClientSocket* MBusServer::GetServerSocket( UFC::PClientSocket* ReadSocket )
{
    int                          PeerID = ReadSocket->GetPeerID();
    register UFC::PClientSocket* Server;    

    if( IsMBusReadConnections( ReadSocket ) == TRUE ) ///< It's a MBus Read socket?
    {
        UFC::PLockObject  LockObj( FMBusWriteSocketListCS );
        
        for( register int i = FMBusWriteConnectionList.ItemCount() - 1; i >= 0; i-- )
        {
             Server = FMBusWriteConnectionList.GetItem(i);
             if( Server->GetPeerID() == PeerID )
                 return Server;
        }
    }
    return NULL;
}
//------------------------------------------------------------------------------------------------------------------
//
// Implement interface SocketServerListener
//
// Is the important interface to control the MultiplexingServer object.
//
//------------------------------------------------------------------------------------------------------------------
// This function will be called when a client socket is disconnected.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::OnClientDisconnect( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket )
{
    if( IsMBusReadConnections( ClientSocket )) ///< Is a MBus read connection ?
    {
        UFC::AnsiString* Host;
        
        UFC::PClientSocket* WriteSocket = GetServerSocket( ClientSocket ); ///< Get the write socket.        
        UFC::BufferedLog::Printf( " MBus@%s OnClientDisconnect, FD:%d", ClientSocket->GetPeerIPAddress().c_str(), ClientSocket->GetHandle() );
        RemoveMBusReadConnections( ClientSocket );                         ///< Delete if from "Read" socket set.
        ClientSocket->CloseSocket();
        if( WriteSocket != NULL )
        {            
            RemoveServerSocket( WriteSocket );            ///< Remove the MBus write socket
            WriteSocket->CloseSocket();                   ///< Close write socket.
            WriteSocket->ClearQueue();                    ///< Clear write socket buffer.                        
            FSendingThreadPool->RemoveJob( WriteSocket ); ///< Remove it from thread pool
        }
        if( ( Host = FIPToHostname.GetObjectByKey( ClientSocket->GetPeerIPAddress() ) )!= NULL )
        {
            UFC::BufferedLog::Printf( " MBus@%s Stopped, Send notify to all clients.", Host->c_str() );
            SendMBusAdminMsg( FALSE, *Host );
        }
    }
    else if( IsMBusWriteConnections( ClientSocket ) )
    {
        UFC::AnsiString* Host;
        
        UFC::BufferedLog::Printf( " MBus@%s OnClientDisconnect, FD:%d", ClientSocket->GetPeerIPAddress().c_str(), ClientSocket->GetHandle() );
        RemoveServerSocket( ClientSocket );            ///< Not a MBus write socket, It's a normal MApp read/write socket
        ClientSocket->CloseSocket();
        ClientSocket->ClearQueue();                    ///< Clear write socket buffer.
        FSendingThreadPool->RemoveJob( ClientSocket ); ///< Remove it from thread pool
        if( ( Host = FIPToHostname.GetObjectByKey( ClientSocket->GetPeerIPAddress() ) )!= NULL )
        {
            UFC::BufferedLog::Printf( " MBus@%s Stopped, Send notify to all clients.", Host->c_str() );
            SendMBusAdminMsg( FALSE, *Host );
        }
    }
    else
    {        
        UFC::BufferedLog::Printf( " MBus client@%s OnClientDisconnect, FD:%d", ClientSocket->GetPeerIPAddress().c_str(), ClientSocket->GetHandle() );
        FClientDB.RemoveConnection( ClientSocket );  ///< Yes! remove it from subject DB.
        SendAppStopAdminMsg( ClientSocket );                    
        RegistInterestSubjectsToAllServers();
        ClientSocket->ClearQueue(); ///< Clear write socket buffer.        
        FSendingThreadPool->RemoveJob( ClientSocket );        
    }
    UFC::BufferedLog::Printf(" Total [%d] Connections.", FTCPServer->GetConnectionCount() );       
}
//------------------------------------------------------------------------------------------------------------------
void MBusServer::SendAppStopAdminMsg( UFC::PClientSocket* ClientSocket )
{
    UFC::AnsiString* AppName;
    UFC::PLockObject Lock( FAppNameCS );
    
    if( (AppName = FSocketToAppName.GetObjectByKey( ClientSocket->GetHandle() )) != NULL )
    {
        MTree Data;

        UFC::BufferedLog::Printf( " MBus App \"%s\" disconnected, FD:%d", AppName->c_str(), ClientSocket->GetHandle() );
        Data.append( "CMD",  ADMIN_APP_OFFLINE );
        Data.append( "APP",  AppName->c_str() );
        Data.append( "HOST", UFC::Hostname );
        DataMessage DataMsg( 0, "MBUS", "all", Data );
        Forward( &DataMsg, NULL );
        delete AppName;
        FSocketToAppName.DeleteByKey( ClientSocket->GetHandle() );
    }    
}
//------------------------------------------------------------------------------------------------------------------
void MBusServer::SendMBusAdminMsg( BOOL IsOnline, const UFC::AnsiString& Hostname )
{ 
    MTree Data;

    if( IsOnline == TRUE )
        Data.append( "CMD",  ADMIN_APP_ONLINE );
    else
        Data.append( "CMD",  ADMIN_APP_OFFLINE );
    Data.append( "APP",  "MBus" );
    Data.append( "HOST", Hostname );
    DataMessage DataMsg( FServerID, "MBUS", "all", Data ); ///< To Local clients
    Forward( &DataMsg, NULL );
}
//------------------------------------------------------------------------------------------------------------------
// This function will be called when a client socket is connected.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::OnClientConnect( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket )
{
    if( FTCPServer->GetConnectionCount() + 1 > MAX_CONNECTIONS )
    {
        UFC::BufferedLog::Printf(" Client@%s FD:%d Connect failed, out of socket connection.(Max:%d)", ClientSocket->GetPeerIPAddress().c_str(), ClientSocket->GetHandle(), MAX_CONNECTIONS );
        UFC::SleepMS( 1000 );
        ClientSocket->Disconnect();        
    }
    else
    {
        Int32 PeerID = ClientSocket->GetPeerID();    
        if( PeerID == FServerID || PeerID == FLocalID ) ///< Is a local connection?
            UFC::BufferedLog::Printf(" Local Client FD:%d Connected.", ClientSocket->GetHandle() );
        else
            UFC::BufferedLog::Printf(" Client@%s FD:%d Connected.", ClientSocket->GetPeerIPAddress().c_str(), ClientSocket->GetHandle() );
        UFC::BufferedLog::Printf(" Total [%d] Connections.", FTCPServer->GetConnectionCount() );       
        ClientSocket->SetCheckHeartbeatType( UFC::htCheckBoth );
        ClientSocket->ResetHeartbeatTimer( TRUE );
        ClientSocket->ResetHeartbeatTimer( FALSE );
        ClientSocket->SetTCPNoDelay( TRUE ); ///< Turn on the TCP_NODELAY socket option.
        ClientSocket->SetBlockMode( FALSE );
        ClientSocket->SetWriteTimeout( 1000 );                             ///< Set write timeout to 1 sec
        ClientSocket->SetBufferSize( SO_RCVBUF, 1048576 );                 ///< 1024K Solaris default
        ClientSocket->SetBufferSize( SO_SNDBUF, 1048576 );                 ///<   64k AIX default     
    }
}
//------------------------------------------------------------------------------------------------------------------
void MBusServer::OnMBusConnected( MBusConnectMessage* MBusConnected,UFC::PClientSocket* MBusSocket )
{
    UFC::PClientSocket* WriteSocket;  
    
    MBusSocket->SetCheckHeartbeatType( UFC::htCheckRead );    
    AddMBusReadConnections( MBusSocket ); ///< New MBus server connected!! Add to Server exists set.
    if( FIPToHostname.GetObjectByKey( MBusSocket->GetPeerIPAddress() ) == NULL )
        FIPToHostname.Add( MBusSocket->GetPeerIPAddress(), new UFC::AnsiString( MBusConnected->getHostame()) );    
    if( (WriteSocket = GetServerSocket( MBusSocket ) )== NULL ) ///< The write socket not exists, means that the peer MBus find this server first.
    {
        AddNewAddressIntoNewIPList( MBusSocket->GetPeerIPAddress() );
        UFC::BufferedLog::Printf(" - MBus@%s [%s] Connected but the write socket not exist.", MBusSocket->GetPeerIPAddress().c_str(), MBusConnected->getHostame().c_str() );
        UFC::BufferedLog::Printf("   Try to establish a write back socket." );
    }
    else ///< No! write socket established.
    {
        RegisterInterestSubjects( WriteSocket, TRUE );
        UFC::BufferedLog::Printf(" - MBus:%s [%s]Connected and the write socket established.", MBusSocket->GetPeerIPAddress().c_str(), MBusConnected->getHostame().c_str() );
        UFC::BufferedLog::Printf("   First time sending the interest subjects info." );
        UFC::BufferedLog::Printf(" - MBus@%s Connected!", MBusConnected->getHostame().c_str() );
        SendMBusAdminMsg( TRUE, MBusConnected->getHostame() );        
    }
}
//------------------------------------------------------------------------------------------------------------------
// This function will be called when a client send data to this server.
//------------------------------------------------------------------------------------------------------------------
BOOL MBusServer::OnClientWrite( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket )
{
    MigoHeader*     Header = NULL;
    UFCType::UInt32 RcvSize = 0;
    BOOL            Rtn;
    try
    {
          if( (Header =  MigoHeader::CreateFromSocket( ClientSocket, RcvSize )) != NULL )
          {
              switch( Header->GetACK() )
              {
                  case ACK_SERVER_CONNECT:   OnMBusConnected((MBusConnectMessage*)Header, ClientSocket ); ///< A MBus connected.
                                             delete Header;
                                             return TRUE;
                  case ACK_CONNECT:          Rtn = OnMAppConnected((ConnectMessage*)Header,ClientSocket ); ///< A connected ACK. reply it a connect confirm ACK.
                                             delete Header;
                                             return Rtn;
                  case ACK_DISCONNECT:       throw( UFC::SocketException("Client request disconnect") );
                  case ACK_REGISTER:         OnRegistSubjects((RegistMessage*)Header, ClientSocket ); ///< Regist a subject to subject DB.
                                             delete Header;
                                             return TRUE;
                  case ACK_DELETE_LISTENER:  DeleteSubscriber( (UnregistMessage*)Header, ClientSocket );///< Unregist a subject.
                                             delete Header;
                                             return TRUE;
                  case ACK_MESSAGE:          Forward( (DataMessage*)Header, ClientSocket ); ///< Receiva a data message.
                                             delete Header;                                 ///< Forward to others.
                                             return TRUE;
                  case ACK_ADMIN_MESSAGE:    HandleAdminMsg((AdminMessage*)Header, ClientSocket ); ///< Receiva a admin message.
                                             delete Header;                                 ///< Forward to others.
                                             return TRUE;
                  case ACK_HEARTBEAT:        ReplyHeartbeatMessage( ClientSocket ); ///< Receive a heartbeat message,
                                             delete Header;                                 ///< Reply a heartbear.
                                             return TRUE;
                  case ACK_HEARTBEAT_CONFIRM:ClientSocket->ResetHeartbeatTimer( TRUE );
                                             ClientSocket->ResetHeartbeatTimer( FALSE );
                                             delete Header;
                                             return TRUE;
                  case ACK_TEST_EXISTS:      delete Header;
                                             return FALSE;
              }
          }
          else
              UFC::BufferedLog::Printf(" FD:%d Recv data failed, bad data or client pipe was broken.", ClientSocket->GetHandle() );
    }
    catch( UFC::SocketException &e )
    {
          if( Header != NULL )
              delete Header;
          UFC::BufferedLog::Printf(" MBus OnClientWrite exception:%s",e.what() );
          if( IsMBusReadConnections( ClientSocket )) ///< It's a MBus "Read" socket ?
              UFC::BufferedLog::Printf(" MBus@%s read socket disconnected.", ClientSocket->GetPeerIPAddress().c_str() );                           
          else
              UFC::BufferedLog::Printf(" MBus client@%s disconnected.", ClientSocket->GetPeerIPAddress().c_str() );                           
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" OnClientWrite throw unknown exception!");
        return FALSE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
// This function will be called when a connection need to perform heartbeat.
//------------------------------------------------------------------------------------------------------------------
BOOL MBusServer::OnHeartbeat( UFC::PClientSocket* pSocket, BOOL IsRead )
{
    if( IsMBusReadConnections( pSocket ))///< It's a MBus
    {
         UFC::PClientSocket* Target = GetServerSocket( pSocket );
         if( Target != NULL ) ///< Find the write socket.
         {
             UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Send %s heartbeat to MBus@%s FD:%d",IsRead?"read":"write", pSocket->GetPeerIPAddress().c_str(), pSocket->GetHandle() );
             SendQueue( Target, MigoHeader(  ACK_HEARTBEAT  ).ToString()  );
             return TRUE;
         }
         else ///< Can't find the write socket!
         {
             UFC::BufferedLog::Printf( " Send heartbeat fail. the write socket was broken" );
             return FALSE;
         }
    }
    else///< Send heartbeat message to client.
    {
         UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Send %s heartbeat to Client@%s FD:%d",IsRead?"read":"write", pSocket->GetPeerIPAddress().c_str(), pSocket->GetHandle() );
         SendQueue( pSocket, MigoHeader(  ACK_HEARTBEAT  ).ToString()  );
         return TRUE;
    }    
}
//------------------------------------------------------------------------------------------------------------------
void MBusServer::OnServerIdle(UFC::MultiplexingServer* ServSocket )
{
    static long LastIdle = 0;
    long TickNow = UFC::GetTickCountMS();

    if( LastIdle == 0 )
    {
        LastIdle = TickNow;
    }
    else
    {
        long diff = TickNow -  LastIdle;
        if(  diff < IDLE_TIME + 50 && diff > IDLE_TIME - 100)
            TOTAL_IDLE_TIME++;
        else
            TOTAL_IDLE_TIME = 0;
        LastIdle = TickNow;
    }
}
//------------------------------------------------------------------------------------------------------------------
//  Implement interface PUDPListener
//------------------------------------------------------------------------------------------------------------------
void MBusServer::OnDataArrived( UFC::PUDPClient* Socket, UFC::MemoryStream* Stream, const MString& Address )
{
    if( Stream->GetSize() == MIGO_HEADER_SIZE )
    {
        UInt8 ACK[ 4 ];

        Stream->Read( ACK, 4 );
        Int32 AckCode = UFC::NInt32::ToInt32( ACK );
        if( AckCode == ACK_NEW_SERVER_UP )
            AddNewAddressIntoNewIPList( Address );///< A new server startup, add the address to temp list.
    }
}
//------------------------------------------------------------------------------------------------------------------
//  Public methods.
//------------------------------------------------------------------------------------------------------------------
void MBusServer::DeleteSubscriber( UnregistMessage* RegMsg, UFC::PClientSocket* Socket )
{
    const UFC::AnsiString Subject = RegMsg->getSubject();
    const UFC::AnsiString Key     = RegMsg->getKey();

    FClientDB.Unregist( Subject, Key, Socket );///< a subscriber can regist only one subject.
    RegistInterestSubjectsToAllServers(); ///< Regist all interesting subjects
                                          ///< to other MBus servers.
}
//------------------------------------------------------------------------------------------------------------------
void MBusServer::OnError( UFC::PClientSocket* WriteSocket, UFC::Exception* e )
{       
    if( !IsMBusWriteConnections( WriteSocket ) )           
    {
        UFC::BufferedLog::Printf(" Send to Client@%s fd:%d failed.", WriteSocket->GetPeerIPAddress().c_str(), WriteSocket->GetHandle() );
        UFC::BufferedLog::Printf(" Exception:%s", e->what() );
        FClientDB.RemoveConnection( WriteSocket ); ///<  It's a client socket. remove it from subject DB.
        SendAppStopAdminMsg( WriteSocket );        ///< Send App stop message to other app.
        RegistInterestSubjectsToAllServers();
        FTCPServer->RemoveConnection( WriteSocket, 30 );
    }
    else
    {
        UFC::BufferedLog::Printf(" Send to MBus@%s failed.", WriteSocket->GetPeerIPAddress().c_str() );
        RemoveServerSocket( WriteSocket );
        UFC::BufferedLog::Printf(" Exception:%s", e->what() );
        WriteSocket->ClearQueue();            ///< Empty the write queue.
    }    
}
//------------------------------------------------------------------------------------------------------------------
void MBusServer::Start()
{
    if( ( FTCPServer = new UFC::MultiplexingServer( MBUS_PORT, MAX_CONNECTIONS, RECV_THREAD, THREAD_SWITCH_MS, "MBusSend"  ) ) != NULL )
    {
        FSendingThreadPool = new SendingThreadPool( this, SEND_THREAD /*Thread number*/, MAX_CONNECTIONS*2 /*Max Job*/, "MBusRecv" );
        /// Create the UDP client.
        if( SILENCE_MODE == FALSE )
        {
            try
            {
                if( (FUDPClient = new UFC::PUDPClient( "any", MCAST_PORT, TRUE )) != NULL )
                {
                    FUDPClient->SetListener( this );
                    FUDPClient->Open();
                    FUDPClient->SetMulticastAddress( MULTICAST_ADDRESS, 0 );
                    FUDPClient->SetMulticastTTL( MCAST_TTL );
                }
            }
            catch( UFC::Exception& e )
            {
                UFC::BufferedLog::Printf(" ********************************************");
                UFC::BufferedLog::Printf( " MBus daemon: %s", e.what() );
                UFC::BufferedLog::Printf( " Please turn on the Multi-cast function" );
                UFC::BufferedLog::Printf(" ********************************************");
                FUDPClient = NULL;
                SILENCE_MODE = TRUE;
            }
        }
        Int32 ICount = 0; ///< For BENCHMARK use.
        Int32 OCount = 0; ///< For BENCHMARK use.
        Int32 FlushCount = 0;
        FTCPServer->SetListener( this );
        FTCPServer->SetIdelInterval( IDLE_TIME );
        UFC::PSocket::SetHeartbeatTime( HEARTBEAT_INTER );
        UFC::BufferedLog::Printf(" ____________________________________________");
        UFC::BufferedLog::Printf("                                             ");
        UFC::BufferedLog::Printf("    MBus daemon %d bit mode                ", sizeof(void*)*8 );
        UFC::BufferedLog::Printf("    Copyright 2003~%d by MDBS Software Inc.", UFC::ThisYear());
        UFC::BufferedLog::Printf("    All right reserved.                      ");
        UFC::BufferedLog::Printf("                                             ");
        UFC::BufferedLog::Printf("    Ver:%s Build Date:%s",MBUS_VERSION,__DATE__ );
        UFC::BufferedLog::Printf("                                             ");
        UFC::BufferedLog::Printf("    Permanent Mode     :%s  ", PERMANENT?"ON":"OFF" );
        UFC::BufferedLog::Printf("    Silence Mode       :%s  ", SILENCE_MODE?"ON":"OFF" );
        UFC::BufferedLog::Printf("    Max Connection     :%d  ", MAX_CONNECTIONS );
        UFC::BufferedLog::Printf("    Sending thread     :%d  ", SEND_THREAD );
        UFC::BufferedLog::Printf("    Receiving thread   :%d  ", RECV_THREAD );
        UFC::BufferedLog::Printf("    Thread switch time :%dms", THREAD_SWITCH_MS ); 
        UFC::BufferedLog::Printf("    Service Port       :%d  ", MBUS_PORT );
        UFC::BufferedLog::Printf("    Heartbeat Interval :%d  ", HEARTBEAT_INTER );
        UFC::BufferedLog::Printf("    Multicast IP       :%s  ", MULTICAST_ADDRESS );
        UFC::BufferedLog::Printf("    Multicast Port     :%d  ", MCAST_PORT );
        UFC::BufferedLog::Printf("    Multicast TTL      :%d  ", MCAST_TTL );
        UFC::BufferedLog::Printf(" ____________________________________________");
        FTCPServer->Run();  ///< Start the TCP server.
        FSendingThreadPool->Run();

        while( PERMANENT || TOTAL_IDLE_TIME < IDLE_TWO_MINUTES )
        {
            if( UFC::BufferedLog::DebugLevelExist(UFC::dlPerformance ) == TRUE )
            {
                ICount =  FTCPServer->GetMessageCount();
                OCount =  FOutCount;
            }
            SendServerStartupNotify();  ///< Broadcast that this server is startuped. use 1 sec
            FlushCount++;

            if( UFC::BufferedLog::DebugLevelExist(UFC::dlPerformance ) == TRUE )
            {
                UFC::BufferedLog::DebugPrintf(UFC::dlPerformance," Incoming message rate: %05d msg/sec.", FTCPServer->GetMessageCount() - ICount );
                UFC::BufferedLog::DebugPrintf(UFC::dlPerformance," Outgoing message rate: %05d msg/sec.", FOutCount - OCount );
                UFC::BufferedLog::DebugPrintf(UFC::dlPerformance," Available thread count[%d] Connection[%d]", FTCPServer->AvailableThread(), FTCPServer->GetConnectionCount()  );
            }
            ConnectToNewServers();  ///< Is there any new server start up?
        }
        if( SILENCE_MODE == FALSE && FUDPClient!=NULL )
        {
            FUDPClient->Close();
        }
        FTCPServer->Stop();
    }
}
//------------------------------------------------------------------------------------------------------------------

