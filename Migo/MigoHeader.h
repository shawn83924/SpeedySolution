#ifndef _MigoHeader_h_
#define _MigoHeader_h_
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "MTree.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef __AIX
    #define ACK_ADMIN_MESSAGE     8
    #define ACK_TEST_EXISTS       16
    #define ACK_PROXY_PROTOCOL    17
    #define ACK_REGISTER          32
    #define ACK_DELETE_LISTENER   33
    #define ACK_MESSAGE           64
    #define ACK_SERVER_CONNECT    69
    #define ACK_CONNECT_CONFIRM   70
    #define ACK_CONNECT           71
    #define ACK_DISCONNECT        72
    #define ACK_NEW_SERVER_UP     73
    #define ACK_SERVER_LIST       74
    #define ACK_HEARTBEAT         81
    #define ACK_HEARTBEAT_CONFIRM 91
    #define ADMIN_APP_ONLINE      100 ///< A MBus application is online.
    #define ADMIN_APP_OFFLINE     101 ///< A MBus application is offline.
    #define ADMIN_APP_QUERY       102 ///< Ask for a MBus application existence.
    #define ADMIN_APP_RESPONSE    103 ///< Reply the existence of a MBus application .
    #define ADMIN_ASK_PROCESS     104
    #define ADMIN_REPLY_PROCESS   105
    #define ADMIN_APP_WAKEUP      106
    #define ADMIN_APP_EXISTS      107
    #define ADMIN_APP_FIRST_ONE   108 
    #define ADMIN_ASK_MBUS        109
    #define ADMIN_REPLY_MBUS      110
    #define ADMIN_KILL_PROCESS    111
    #define MIGO_HEADER_SIZE       20
    #define MIGO_SIZE_FIELD_OFFSET 12
    #define IS_SERVER        0x00000001
    #define IS_FIRST_TIME    0x00000002
#else
    const int ACK_ADMIN_MESSAGE     = 8;
    const int ACK_TEST_EXISTS       = 16;
    const int ACK_PROXY_PROTOCOL    = 17;
    const int ACK_REGISTER          = 32;
    const int ACK_DELETE_LISTENER   = 33;
    const int ACK_MESSAGE           = 64;
    const int ACK_SERVER_CONNECT    = 69;
    const int ACK_CONNECT_CONFIRM   = 70;
    const int ACK_CONNECT           = 71;
    const int ACK_DISCONNECT        = 72;
    const int ACK_NEW_SERVER_UP     = 73;
    const int ACK_SERVER_LIST       = 74;
    const int ACK_HEARTBEAT         = 81;
    const int ACK_HEARTBEAT_CONFIRM = 91;
    const int ADMIN_APP_ONLINE      = 100; ///< A MBus application is online.
    const int ADMIN_APP_OFFLINE     = 101; ///< A MBus application is offline.
    const int ADMIN_APP_QUERY       = 102; ///< Ask for a MBus application existence.
    const int ADMIN_APP_RESPONSE    = 103; ///< Reply the existence of a MBus application .
    const int ADMIN_ASK_PROCESS     = 104;
    const int ADMIN_REPLY_PROCESS   = 105;
    const int ADMIN_APP_WAKEUP      = 106;
    const int ADMIN_APP_EXISTS      = 107;
    const int ADMIN_APP_FIRST_ONE   = 108;
    const int ADMIN_ASK_MBUS        = 109;
    const int ADMIN_REPLY_MBUS      = 110;
    const int ADMIN_KILL_PROCESS    = 111;
    const int MIGO_HEADER_SIZE         = 20;
    const int MIGO_SIZE_FIELD_OFFSET   = 12;
    const UFCType::Int32 IS_SERVER     = 0x00000001;
    const UFCType::Int32 IS_FIRST_TIME = 0x00000002;
#endif
const int MBUS_MSG_MAX = 1048576;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern UFC::AnsiString ADMIN_SUBJECT;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
using UFCType::UInt8;
class DataMessagePool;   ///< Optional pool for the MBus order hot path (see below).
class MigoHeader
{
protected:
    UFCType::Int32	     FACK;
    UFCType::Int32	     FSenderID;
    UFCType::Int32	     FMessageID;
protected:
    UFCType::Int32       HeaderChecksum( UFCType::Int32 Size ) { return (FACK + Size + FMessageID); }
    UFCType::Int32       DataChecksum( UFCType::UInt8* Data, UFCType::Int32 Size );
public:
    MigoHeader( UFCType::Int32 Action, UFCType::Int32 MessageID = 0, UFCType::Int32 SenderID = 0 );
    MigoHeader( MigoHeader& Src ); ///< Copy constrator.
    virtual ~MigoHeader() {}
public:
    UFCType::Int32	    GetACK()       { return FACK; }
    UFCType::Int32	    GetSenderID()  { return FSenderID; }
    UFCType::Int32	    GetMessageID() { return FMessageID; }
    void	    SetACK( UFCType::Int32 ACK )             { FACK  = ACK; }
    void	    SetSenderID( UFCType::Int32 SenderID )   { FSenderID  = SenderID; }
    void	    SetMessageID( UFCType::Int32 MessageID ) { FMessageID = MessageID; }
private:
    static MigoHeader* ProcessProxyProtocol( UFC::UInt8* Data, UFC::PClientSocket* Socket );
    static MigoHeader* ProcessHeaderSizeMessage( UFCType::Int32 Act, UFCType::Int32 MsgID, UFCType::Int32 SenderID );
    static MigoHeader* ProcessDataMessage( UFCType::Int32 Act, UFCType::Int32 MsgID, UFCType::Int32 SenderID, UFCType::Int32 Size , UFC::PClientSocket* Socket, DataMessagePool* Pool = NULL );
    static MigoHeader* ProcessDataMessage( UFCType::Int32 Act, UFCType::Int32 MsgID, UFCType::Int32 SenderID, UFCType::Int32 Size , UFC::PStream* Stream );
    static MigoHeader* CreateMessageFromStream( UFCType::Int32 Act, UFCType::Int32 MsgID, UFCType::Int32 SenderID, UFCType::Int32 Size , UFC::MemoryStream* MemStream );
public:
    ///< Legacy 2-arg overload kept as a distinct symbol so binaries built against
    ///< the old header keep resolving against libUFC (no ABI break); forwards Pool=NULL.
    static MigoHeader* CreateFromSocket( UFC::PClientSocket* Socket, UFCType::UInt32& RcvSize );
    ///< Pool is opt-in: when non-NULL, ACK_MESSAGE payloads reuse a pooled DataMessage.
    static MigoHeader* CreateFromSocket( UFC::PClientSocket* Socket, UFCType::UInt32& RcvSize, DataMessagePool* Pool );
    static MigoHeader* CreateFromStream( UFC::PStream* Stream, UFCType::UInt32& RcvSize );
    static UFCType::Int32       HeaderChecksum( UFCType::Int32 Ack, UFCType::Int32 Size, UFCType::Int32 MessageID ) { return (Ack + Size + MessageID); }
public:
    virtual    void    SerializeToSocket( UFC::PClientSocket* Socket );
    virtual    void    SaveToStream( UFC::PStream* Stream );
    std::string ToString( void );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ProxyPotocolMessage : public MigoHeader
{
private:
    MString FLocalIP;
    MString FPeerIP;
public:
    ProxyPotocolMessage( const MString &LocalIP , const MString &PeerIP );
    virtual ~ProxyPotocolMessage() {}
public:
    const MString& getLocalIP( void ) { return FLocalIP; }    
    const MString& getPeerIP( void ) { return FPeerIP; }    
};    
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ConnectMessage : public MigoHeader
{
private:
    MString FAppName;
public:
    ConnectMessage( UFCType::Int32 ID, UFCType::Int32 SenderID, UFC::MemoryStream* DataStream );
    ConnectMessage( UFCType::Int32 ID, const MString &AppName );
    ConnectMessage( ConnectMessage& Src );///< Copy constrator.
    virtual ~ConnectMessage() {}
    virtual void SaveToStream( UFC::PStream* Stream );
public:
    const MString& getAppName( void ) { return FAppName; }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MBusConnectMessage : public MigoHeader
{
private:
    MString FHostname;
public:
    MBusConnectMessage( UFCType::Int32 ID, UFCType::Int32 SenderID, UFC::MemoryStream* DataStream );
    MBusConnectMessage( UFCType::Int32 ID, const MString &Hostame );
    MBusConnectMessage( MBusConnectMessage& Src );///< Copy constrator.
    virtual ~MBusConnectMessage() {}
    virtual void SaveToStream( UFC::PStream* Stream );
public:
    const MString& getHostame( void ) { return FHostname; }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class RegistMessage : public MigoHeader
{
private:
    typedef std::pair< UFC::AnsiString, UFC::AnsiString > SubjectKeyPair;
private:
    UFC::List< SubjectKeyPair > FPairList;
    UFCType::Int32              FFlag;
    bool                        FRegAllAll;
public:
    RegistMessage( UFCType::Int32 ID, UFCType::Int32 SenderID, UFC::MemoryStream* DataStream );
    RegistMessage( UFCType::Int32 ID, const MString &Subject, const MString &Key, UFCType::Int32 Flag = 0 );
    RegistMessage( UFCType::Int32 ID, UFCType::Int32 FFlag = 0 );
    RegistMessage( RegistMessage& Src );///< Copy constrator.
    virtual ~RegistMessage() {}
    virtual void SaveToStream( UFC::PStream* Stream );
public:
    const MString& getSubject( int Index = 0 ) { return FPairList.GetItem(Index).first; }
    const MString& getKey( int Index = 0 ) { return FPairList.GetItem(Index).second; }
    void  Add( const MString& Subject, const MString& Key );
    UFCType::Int32 Remove( const MString& Subject, const MString& Key );
    UFCType::Int32 ItemCount() { return FPairList.ItemCount(); }
    BOOL  IsFromServer() { return (FFlag & IS_SERVER) == IS_SERVER; }
    BOOL  IsFirstTime()  { return (FFlag & IS_FIRST_TIME) == IS_FIRST_TIME; }
	void  Clear();
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class UnregistMessage : public MigoHeader
{
private:
    MString FSubject;
    MString FKey;
public:
    UnregistMessage( UFCType::Int32 ID, UFCType::Int32 SenderID, UFC::MemoryStream* DataStream  );
    UnregistMessage( UFCType::Int32 ID, const MString &Subject, const MString &Key );
    UnregistMessage( UnregistMessage& Src );///< Copy constrator.
    virtual ~UnregistMessage() {}
    virtual void SaveToStream( UFC::PStream* Stream );
public:
    const MString& getSubject( void ) { return FSubject; }
    const MString& getKey( void ) { return FKey; }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
///< Zero-alloc extraction of the fixed order schema from a DataMessage's serialized
///< MTree, bypassing getData()'s per-field MNode/hashmap/string allocation. ORDER/ID
///< point into the message buffer (valid while the DataMessage lives). Stock reads
///< ORDER/NID/ID/GROUP/PeerIP; futures also OFFHOUR. Absent fields keep has*=false (matches
///< MTree::get returning false). See DataMessage::ScanOrderFields.
struct FastOrderFields
{
    const char* Order;   int OrderLen;   bool HasOrder;
    const char* ID;      int IDLen;      bool HasID;
    int         NID;                     bool HasNID;
    int         Group;                   bool HasGroup;
    bool        OffHour;                 bool HasOffHour;
    bool        HasStrategy;  ///< touch/stop order (STRATEGY field present) -> caller must use the getData path
    const char* PeerIP;  int PeerIPLen;  bool HasPeerIP;   ///< client IP written by the sender (order log / ODS)
    FastOrderFields() : Order(0),OrderLen(0),HasOrder(false), ID(0),IDLen(0),HasID(false),
                        NID(0),HasNID(false), Group(0),HasGroup(false), OffHour(false),HasOffHour(false),
                        HasStrategy(false), PeerIP(0),PeerIPLen(0),HasPeerIP(false) {}
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class DataMessage : public MigoHeader
{
private:
    MString            FSubject;
    MString            FKey;
    MString            FIPAddress;
    UFC::MemoryStream  FStream;
    UFCType::Int32     FDataPos;
    UFCType::Int32     FFD;
public:
    DataMessage( UFCType::Int32 ID, UFCType::Int32 SenderID, UFC::MemoryStream* DataStream );
    ///< Order hot path: read the payload straight from the socket into FStream,
    ///< skipping the intermediate stack MemoryStream + copy-construct.
    DataMessage( UFCType::Int32 ID, UFCType::Int32 SenderID, UFCType::Int32 Size, UFC::PClientSocket* Socket );
    DataMessage( UFCType::Int32 ID, const MString &Subject, const MString &Key, MTree &Data );
    DataMessage( DataMessage& Src );
    DataMessage( void );   ///< Empty object for pool preallocation; populate with InitFromSocket().
    ///< Repopulate a recycled object from the socket, reusing FStream's buffer when it fits.
    void InitFromSocket( UFCType::Int32 ID, UFCType::Int32 SenderID, UFCType::Int32 Size, UFC::PClientSocket* Socket );
    virtual void SaveToStream( UFC::PStream* Stream );
public:
    const MString& getSubject( void ) { return FSubject; }
    const MString& getKey( void )     { return FKey; }
    void  getData( MTree& Data );
    ///< Fast, zero-alloc scan of the serialized MTree for the order fields (see
    ///< FastOrderFields). Returns true if the payload parsed cleanly. Byte-equivalent
    ///< to reading the same fields via getData; used on the MBus order hot path.
    bool  ScanOrderFields( FastOrderFields& Out );
    static void MessageToStream( UFCType::Int32 ID, const MString &Subject, const MString &Key, MTree &Data, UFC::MemoryStream* );
    static void MessageToSocket( UFCType::Int32 ID, const MString &Subject, const MString &Key, MTree &Data, UFC::PClientSocket* Socket );	
public:
    void SetIPAddress( const MString& IP ) { FIPAddress = IP; }
    void SetFD( UFCType::Int32 FD )	   { FFD = FD; }
    const MString& GetIPAddress( void )	   { return FIPAddress; }
    UFCType::Int32 GetFD( void )	   { return FFD; }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Thread-safe pool of live DataMessage objects for the MBus order hot path.
//  Only usable where a message's create->use->delete is confined to one thread
//  within one call (e.g. MBusServer::OnClientWrite). Recycled objects keep their
//  FStream buffer, so a warm pool serves orders with zero allocation.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class DataMessagePool
{
private:
    UFC::PtrQueue<DataMessage> FFree;   ///< internal critical section makes borrow/return thread-safe
public:
    DataMessagePool( UFCType::Int32 Prealloc = 0 );
    ~DataMessagePool();
    ///< Borrow an object and populate it from the socket. Never returns NULL (falls back to new).
    DataMessage* Acquire( UFCType::Int32 ID, UFCType::Int32 SenderID, UFCType::Int32 Size, UFC::PClientSocket* Socket );
    void         Release( DataMessage* Msg );   ///< Return to the pool (delete if the pool is full).
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class AdminMessage : public MigoHeader
{
private:
    MString           FKey;
    UFC::MemoryStream FStream;
    UFCType::Int32             FDataPos;
public:
    AdminMessage( UFCType::Int32 ID, UFCType::Int32 SenderID, UFC::MemoryStream* DataStream );
    AdminMessage( UFCType::Int32 ID, const MString &Key, MTree &Data  );
    AdminMessage( AdminMessage& Src );
    virtual void SaveToStream( UFC::PStream* Stream );
public:
    const MString& getSubject( void ) { return ADMIN_SUBJECT; }
    const MString& getKey( void ) { return FKey; }
    void  getData( MTree& Data );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif // MigoHeader_h

