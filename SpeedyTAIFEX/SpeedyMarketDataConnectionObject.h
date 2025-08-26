#ifndef __SPEEDY_MARKET_DATA_CONNECTION_OBJECT_H
#define __SPEEDY_MARKET_DATA_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "../TAIFEXLib/ConnectionObject.h"
#include "../UFC/PThread.h"
#include "../UFC/PQueue.h"
//-----------------------------------------------------------------------------------------------------------------------
class SpeedyObjectsManager;
//-----------------------------------------------------------------------------------------------------------------------
//   FCM Side: Connection Object for AP code 3
//-----------------------------------------------------------------------------------------------------------------------
class SpeedyMarketDataConnectionObject : public ConnectionObjectBase, public UFC::PThread
{
private:
        SpeedyObjectsManager* FObjectManager;
        ConnectionListener*  FConnectionListener;
        Connection*          FConnection;
        BOOL                 FListen;
        BOOL                 FStart;
        Int32                FState;
        Int32                FMsgLenTable[ 64 ];
        char                 FLogBuffer[ 256 ];
        UFC::AnsiString      FLinkPVCStr;
        UFC::PQueue          FCommandQueue;
        UFC::PQueue          FDataQueue;
        BOOL                 FIsOptions;
private:
        void CheckCommand();
        // Implement interface PThread
        virtual void Execute( void );
public:
        SpeedyMarketDataConnectionObject( SpeedyObjectsManager* ObjManager,
                                          ConnectionParameter& info,
                                          ConnectionListener*  Listener,
                                          UFC::ThreadListener* TListener,
                                          BOOL                 IsOpt );
        virtual ~SpeedyMarketDataConnectionObject();
        void WriteLog( char* Data, int Size );
        void InitMessageLengthTable( void );
public:
        virtual void Listen( BOOL );
        virtual void Active( void ) { UFC::PThread::Start(); }
        virtual void Close( void ){}
        virtual void OnCommand( int , void* );
        virtual BOOL IsReady( void ) { return FConnection->IsOpened(); }
        virtual void UpdateConnectionParameter( ConnectionParameter& ){}
};
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------

