/* 
 * File:   SpeedyFIXManager.h
 * Author: yuan
 *
 * Created on 2014年5月12日, 下午 11:25
 */

#ifndef SPEEDYFIXMANAGER_H
#define	SPEEDYFIXMANAGER_H
//------------------------------------------------------------------------------
#include "Application.h"
#include "MessageCracker.h"
#include "Values.h"
#include "Mutex.h"
#include "Session.h"

#include "fix44/Reject.h"
#include "fix44/NewOrderSingle.h"
#include "fix44/ExecutionReport.h"
#include "fix44/OrderCancelRequest.h"
#include "fix44/OrderCancelReplaceRequest.h"
#include "fix44/OrderCancelReject.h"
#include "fix44/OrderStatusRequest.h"
#include "SocketInitiator.h"
#include "ThreadedSocketInitiator.h"
#include "UFCFileLog.h"
#include <queue>        
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "../UFC/iniFile.h"
#include "SMPMessage.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------
class OrderListener;
class SpeedyObjectsManager;
//------------------------------------------------------------------------------
class SpeedyFIXManager : public FIX::Application, UFC::PThread, FIX::MessageCracker
{
private:
    UFC::AnsiString               FSettingFile;
    FIX::SessionSettings*         FSettings;      
    UFC::PCriticalSection         FFIXSessionLock;      
    FIX::MessageStoreFactory*     FStoreFactory;           
    FIX::UFCFileLogFactory*       FLogFactory;                  
    FIX::Initiator*               FInitiator;        
    UFC::PHashMap<UFC::AnsiString,OrderListener*>  FFIXOrderClients;
    UFC::PHashMap<UFC::AnsiString,OrderListener*>  FFIXOrderClientsBackup;
    UFC::List<OrderListener*>                      FFIXSession;
    int                                            FSessionindex;
    BOOL                          FIsMonitoring;
    SpeedyObjectsManager*         FObjectManager;
    int                           FSetingTimeTick;
private: ///< implement interface PThread
    virtual void Execute( void );    
private:        
    virtual void onMessage( const FIX44::ExecutionReport&, const FIX::SessionID& );        
    virtual void onMessage( const FIX44::OrderCancelReject&, const FIX::SessionID& );      
    virtual void onMessage( const FIX44::BusinessMessageReject&, const FIX::SessionID& );
private: ///< implement interface FIX::Application
    void onCreate( const FIX::SessionID& );
    void onLogon( const FIX::SessionID& sessionID );
    void onLogout( const FIX::SessionID& sessionID );
    void toAdmin( FIX::Message&, const FIX::SessionID& );
    void fromAdmin( const FIX::Message&, const FIX::SessionID& )throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon );
    void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );
    void toApp( FIX::Message&, const FIX::SessionID& )  throw( FIX::DoNotSend );
    void BindCore( void );
    FIX::Session* NextSession( void );
public:
    SpeedyFIXManager( SpeedyObjectsManager* ObjectManager );
    void AddFIXClient( const string& SessionID, OrderListener* OrderClient );
    OrderListener* FindFIXClient( const UFC::AnsiString& SessionID );
    FIX::Session*  FindFIXSession(  const UFC::AnsiString& SenderCompID, const UFC::AnsiString& TargetCompID );
    void Run( void );
    FIX::SessionSettings* GetSettings( void ) { return FSettings; }      
    void SetIsMonitoring( BOOL YesOrNo ) { FIsMonitoring = YesOrNo; }
    void UpdateFIXStatus( void );
    BOOL SendFIXOrder( SMPMessage& SMPMsg );    
};
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------

#endif	/* SPEEDYFIXMANAGER_H */

