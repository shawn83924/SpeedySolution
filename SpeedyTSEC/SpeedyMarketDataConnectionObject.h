#ifndef __SPEEDY_MARKET_DATA_CONNECTION_OBJECT_H
#define __SPEEDY_MARKET_DATA_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "../TSECLib/ConnectionObject.h"
#include "../UFC/PQueue.h"
//-----------------------------------------------------------------------------------------------------------------------
//   FCM Side: Connection Object for AP code 3
//-----------------------------------------------------------------------------------------------------------------------
class SpeedyMarketDataConnectionObject : public ConnectionObjectBase, public PThread
{
private:
        ConnectionListener*  FConnectionListener;
        Connection*          FConnection;
        BOOL                 FListen;
        BOOL                 FStart;
        Int32                FState;
        Int32                FMsgLenTable[ 64 ];
        char                 FLogBuffer[ 256 ];
        AnsiString           FLinkPVCStr;
        PQueue               FCommandQueue;
        PQueue               FDataQueue;
private:
        void CheckCommand();
        // Implement interface PThread
        virtual void Execute( void );
public:
        SpeedyMarketDataConnectionObject( ConnectionParameter& info,
                                          ConnectionListener*  Listener,
                                          ThreadListener*      TListener );
        virtual ~SpeedyMarketDataConnectionObject();
        void WriteLog( char* Data, int Size );
        void InitMessageLengthTable( void );
public:
        virtual void Listen( BOOL );
        virtual void Close( void ){}
        virtual void OnCommand( int , void* );
        virtual void UpdateConnectionParameter( ConnectionParameter& ){}
};
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------

