#ifndef __SPEEDY_CONFIRM_CONNECTION_OBJECT_H
#define __SPEEDY_CONFIRM_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "../TSECLib/ConnectionObject.h"
#include "../TSECLib/Messages.h"
#include "../UFC/PInt32.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//-----------------------------------------------------------------------------------------------------------------------
class LinkSubSystemFCM;
class SpeedyObjectsManager;
//-----------------------------------------------------------------------------------------------------------------------
//   FCM Side: Connection Object for AP code 3
//-----------------------------------------------------------------------------------------------------------------------
class SpeedyConfirmConnectionObject : public ConnectionObjectBase, public UFC::PThread
{
private:
    BOOL                 FNeedLock;
    BOOL                 FHandshakeReady;
    BOOL                 FEnabled;
    UFC::PInt32*         FSeqNumber;
    int                  F030Count;
    char                 FAPCode;
    BOOL                 FBroken;
    LinkSubSystemFCM*    FConnection;
    UFC::AnsiString      FLineBrokerID;
    ConnectionListener*  FConnectionListener;
    SpeedyObjectsManager* FObjectManager; 
    BOOL                 FIsTWSE;
private:
	void SendFillMessage(  char* Buffer, const char* TSECTime, const char* LineTime, Int32 OrigNID, const char* Key, BOOL IsSpeedyFill   );
private:
	// Implement interface PThread
	virtual void Execute( void );
private:
        void CheckTime( char * );
	void MessageLoop( void );
	void DisableLoop( void );
	void FCMHandshake( void );
	void ReceiveR3( MessageR3* R3 );
	void SetSystemTime( Int32 hhmmss );
	void ConfirmsubSystemHandshake( BOOL ReceivedL010 );
        BOOL FetchOrderInfo( R3Body* R3, Int32& OrigNID, UFC::AnsiString& Key, UFC::AnsiString& OID );
public:
	SpeedyConfirmConnectionObject( SpeedyObjectsManager* ObjManager,
                                       ConnectionParameter&  info,
                                       ConnectionListener*   Listener,
                                       UFC::ThreadListener*  TListener,
                                       char		     APCode,
                                       BOOL                  IsTWSE );
	virtual ~SpeedyConfirmConnectionObject();
public:
    virtual void Enable( BOOL );
    virtual BOOL IsEnable( void );
    virtual void Listen( BOOL );
    virtual void Active( void ) { UFC::PThread::Start(); }
	virtual void Close( void ){}
	virtual void OnCommand( int , void* );
	virtual void UpdateConnectionParameter( ConnectionParameter& ){}
};
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------

