#ifndef __FCM_ORDER_CONNECTION_OBJECT_H
#define __FCM_ORDER_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "../TAIFEXLib/ConnectionObject.h"
#include "../TAIFEXLib/MessageHeader.h"
#include "../UFC/PInt32.h"
#include "../UFC/AnsiString.h"
#include "../UFC/PQueue.h"
#include "SpeedyOrderClient.h"
class SpeedyOrderClient;
class SpeedyTMPOrderClient;
class ConnectionListener;
//----------------------------------------------------------------------------------------------------------------------
#define OPT_X25_QUEUE  0x73A10000
#define FUT_X25_QUEUE  0x72A10000
//-----------------------------------------------------------------------------------------------------------------------
//   FCM Side: Connection Object for AP code 4
//-----------------------------------------------------------------------------------------------------------------------
class SpeedyObjectsManager;
//-----------------------------------------------------------------------------------------------------------------------
class SpeedyOrderConnectionObject : public ConnectionObjectBase,public UFC::PThread
{
private:
        SpeedyObjectsManager* FObjectManager;
    	SpeedyOrderClient*    FOrderFCM;
        ConnectionListener*   FConnectionListener;
    	int                   FOrderNumber;
        BOOL                  FReconnect;
        BOOL                  FHandshake;
        BOOL                  FBrokenRequest;
        BOOL                  FEnabled;
        UFC::PInt32*          FLastNID;
        UFC::PQueue           FQueue;
        UFC::UQueueData       FOrder;
        UFC::AnsiString       FPVCID;
    	ConnectionParameter   FParams;
        char                  FDataStr[ 512 ];
        long                  FMType;
private:
        BOOL OpenMarket();
        BOOL ProcessOrder();
        BOOL IsExpired( MessageHeader*, Int32 NID, Int32 Tick ,const UFC::AnsiString& Key,const UFC::AnsiString& Host);
        void FetchData( UInt8* Data, Int32 Size, Int32& NID, Int32& Tick, char* Key, char* Host, char* Message, BOOL Dump = FALSE );
        Int32 RecordInfo( MessageHeader* OrderMessage, Int32 NID, const UFC::AnsiString& Key, UFC::AnsiString& OrderID, UFC::AnsiString& IB );
        void DisableLoop( void );
    	virtual void Execute( void );
        void  ReplyMessage( MessageHeader* Msg, Int32 NID, UFC::AnsiString Key, TMessageType& Type ,BOOL& NeedLog, UFC::AnsiString& ExecKey, const UFC::AnsiString& Host );
public:
       SpeedyOrderConnectionObject( SpeedyObjectsManager* ObjManager,
                                    ConnectionParameter& info,
                                    ConnectionListener*  CListener,
                                    UFC::ThreadListener* TListener,
                                    BOOL                 IsOpt );
        virtual ~SpeedyOrderConnectionObject();
public:
        virtual void Enable( BOOL IsEnabled );
        virtual void Active( void ) { UFC::PThread::Start(); }            
        virtual void Close() { Stop(); }
    	virtual void OnCommand( int , void*  );
    	virtual void UpdateConnectionParameter( ConnectionParameter& ){}
        virtual void Listen( BOOL IsListen );
        virtual BOOL IsReady( void ) { return FOrderFCM->IsReady(); }
};
//-----------------------------------------------------------------------------------------------------------------------
//   FCM Side: Connection Object for AP code 4
//-----------------------------------------------------------------------------------------------------------------------
class SpeedyTMPOrderConnectionObject : public ConnectionObjectBase,public UFC::PThread
{
private:
        SpeedyObjectsManager* FObjectManager;
    	SpeedyTMPOrderClient* FOrderFCM;
        ConnectionListener*  FConnectionListener;
    	int                  FOrderNumber;
        BOOL                 FReconnect;
        BOOL                 FHandshake;        
        BOOL                 FEnabled;
        BOOL                 FReadException;
        UFC::PInt32*         FLastNID;
        UFC::PQueue          FQueue;
        UFC::UQueueData      FOrder;
        UFC::AnsiString      FPVCID;
    	ConnectionParameter  FParams;
        UFC::AnsiString      FPBNO;
        char                 FDataStr[ 512 ];
        long                 FMType;
private:
        BOOL OpenMarket();
        BOOL ProcessOrder();
        BOOL IsExpired( MessageHeader*, Int32 NID, Int32 Tick ,const UFC::AnsiString& Key,const UFC::AnsiString& Host);
        void FetchData( UInt8* Data, Int32 Size, Int32& NID, Int32& Tick, char* Key, char* Host, char* Message, BOOL Dump = FALSE );
        Int32 RecordInfo( MessageHeader* OrderMessage, Int32 NID, const UFC::AnsiString& Key, UFC::AnsiString& OrderID, UFC::AnsiString& IB );
        void DisableLoop( void );
        void SetSystemTime( UInt32 Tick );
    	virtual void Execute( void );
public:
       SpeedyTMPOrderConnectionObject(  SpeedyObjectsManager*  ObjManager,
                                        ConnectionParameter& info,
                                        ConnectionListener* CListener,
                                        UFC::ThreadListener* TListener,
                                        BOOL IsOpt);
        virtual ~SpeedyTMPOrderConnectionObject();
public:
	virtual void Enable( BOOL IsEnabled );
        virtual void Active( void ) { UFC::PThread::Start(); }            
	virtual void Close() { Stop(); }
    	virtual void OnCommand( int , void*  );
	virtual void UpdateConnectionParameter( ConnectionParameter& ){}
        virtual void Listen( BOOL IsListen );
        virtual BOOL IsReady( void ) { return FOrderFCM->IsReady(); }
};
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
