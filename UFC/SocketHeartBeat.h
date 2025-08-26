#ifndef SocketHeartBeatH
#define SocketHeartBeatH
#include "PClientSocket.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SocketIoMonitor : public PThread
{
private:
    PtrList<PClientSocket>* FIdleList;
    PtrList<PSocket>        FGarbageList;
    PCriticalSection*       FClientListCS;
    PCriticalSection        FTrashListCS;
    BOOL				    FIsRunning;
    BOOL                    FEnableHeartbeat;
private:
    void DecreaseSocketTimeCount( void );
    void ClearTrashConnections( void );
    virtual void Execute( void );
public:
    SocketIoMonitor( PtrList<PClientSocket>* pList, PCriticalSection* pCS );
    virtual ~SocketIoMonitor( void );
    void ToTrashCan( PSocket* TrashSocket, int Sec = 3600 );
public:
    void Run( void );
    void Stop( void );
    void EnableHeartbeat( BOOL Enable ) { FEnableHeartbeat = Enable; } 

};//class SocketHeartbeat
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}//namespace UFC
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif
