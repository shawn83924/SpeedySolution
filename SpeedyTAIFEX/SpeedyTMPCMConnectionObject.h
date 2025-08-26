#ifndef __SPEEDY_CONFIRM_CONNECTION_OBJECT_H
#define __SPEEDY_CONFIRM_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "../TAIFEXLib/ConnectionObject.h"
#include "../TAIFEXLib/CMessages.h"
#include "../UFC/PInt32.h"
//-----------------------------------------------------------------------------------------------------------------------
class LinkSubSystemFCM;
class SpeedyObjectsManager;
//-----------------------------------------------------------------------------------------------------------------------
//   FCM Side: Connection Object for AP code 3
//-----------------------------------------------------------------------------------------------------------------------
class SpeedyConfirmConnectionObject : public ConnectionObjectBase, public UFC::PThread
{
private:
        SpeedyObjectsManager* FObjectManager;
	BOOL                 FNeedLock;
	BOOL                 FHandshakeReady;
        BOOL                 FEnabled;
        UFC::PInt32*         FSeqNumber;
	int                  F030Count;
	int                  F031Count;
	int                  F032Count;
        UInt32               FmType;
	char                 FAPCode;
	BOOL		     FBroken;
	LinkSubSystemFCM*    FConnection;
        UFC::AnsiString      FLineBrokerID;
        UFC::AnsiString      FPVCID;
	ConnectionListener*  FConnectionListener;
        char                 FMsgBuffer[ 1024 ];
private:
        void  SetSystemTime( Int32 hhmmss );
public:
        char  FetchOrderInfo( C03XBody* C030, Int32& OrigNID, UFC::AnsiString& Key, UFC::AnsiString& OID, UFC::AnsiString& IB, UFC::AnsiString& CKey );
        void  SendFillMessage( char* Buffer, const char* TAIFEXTime, const char* X25Time, Int32 OrigNID = -1, const char* Key = NULL, const char* CKey = NULL );
private:
	// Implement interface PThread
	virtual void Execute( void );
private:
	void MessageLoop( void );
        void DisableLoop( void );
	void FCMHandshake( void );
	void CMHandshake( void );
	void ReceiveC030( MessageC030* C030 );
	void ReceiveC031( MessageC031* C031 );
	void ReceiveC032( MessageC032* C032 );
	void ConfirmsubSystemHandshake( BOOL ReceivedL010 );
public:
	SpeedyConfirmConnectionObject( SpeedyObjectsManager* ObjManager,
                                       ConnectionParameter& info,
		                       ConnectionListener*  Listener,
		                       UFC::ThreadListener* TListener,
		                       char		    APCode,
                                       BOOL                 IsOpt );
	virtual ~SpeedyConfirmConnectionObject();
public:
        virtual void Listen( BOOL );
        virtual void Enable( BOOL );
        virtual void Active( void )  { UFC::PThread::Start(); }
        virtual BOOL IsReady( void );
	virtual void Close()         { Stop(); }
	virtual void OnCommand( int , void* );
	virtual void UpdateConnectionParameter( ConnectionParameter& ){}
};
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------

