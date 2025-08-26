//---------------------------------------------------------------------------
#include "SocketHeartBeat.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
//
//SocketIoMonitor
//
//---------------------------------------------------------------------------
SocketIoMonitor::SocketIoMonitor(PtrList<PClientSocket>* pList, PCriticalSection* pCS)
:PThread(NULL, FALSE)
,FIdleList(pList)
,FClientListCS(pCS)
,FIsRunning(FALSE)
,FEnableHeartbeat(TRUE)
{
    PThread::Start();
}
//---------------------------------------------------------------------------
SocketIoMonitor::~SocketIoMonitor(void)
{
    if( IsTerminated() == FALSE ) ///< If it's a Non-blocking socket.
    {
        Terminate(); ///< Terminate the socket polling thread.
        WaitFor();   ///< Wait for thread terminated.
    }
}
//---------------------------------------------------------------------------
void SocketIoMonitor::Execute(void)
{
    while( IsTerminated() != TRUE )
    {
        if( FIsRunning == TRUE )
        {
            if( FEnableHeartbeat == TRUE )
                DecreaseSocketTimeCount();
            ClearTrashConnections();
        }
        UFC::SleepMS( 1000 );
    }
}
//---------------------------------------------------------------------------
void SocketIoMonitor::DecreaseSocketTimeCount(void)
{
    register PClientSocket* pSocket;

    PLockObject Lock( *FClientListCS );

    for( register Int32 i = FIdleList->ItemCount() - 1; i >= 0; i-- )
    {
         if( (pSocket = FIdleList->GetItem(i)) != NULL )
              pSocket->HeartbeatTimerCountDown( );
    }
}
//---------------------------------------------------------------------------
void SocketIoMonitor::ClearTrashConnections( void )
{
    register PSocket* TrashSocket;

    for( register Int32 i = FGarbageList.ItemCount() - 1; i >= 0; i-- )
    {
        if( ( TrashSocket = FGarbageList.GetItem( i ) ) != NULL )
        {
            if( TrashSocket->DeleteCountDown( ) <= 0 )
            {
                delete TrashSocket;
                PLockObject Lock( FTrashListCS );
                FGarbageList.Delete( i );
            }
        }
    }
}
//---------------------------------------------------------------------------
void SocketIoMonitor::Run(void)
{
    FIsRunning = TRUE;
}
//---------------------------------------------------------------------------
void SocketIoMonitor::Stop(void)
{
    FIsRunning = FALSE;
}
//---------------------------------------------------------------------------
void SocketIoMonitor::ToTrashCan( PSocket* TrashSocket, int Sec )
{
    PLockObject Lock( FTrashListCS );

    if( FGarbageList.IndexOf( TrashSocket ) == -1 )
    {
        FGarbageList.Add( TrashSocket );
        TrashSocket->SetDeleteCounter( Sec );
    }
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

