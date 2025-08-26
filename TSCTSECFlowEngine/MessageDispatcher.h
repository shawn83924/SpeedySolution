//---------------------------------------------------------------------------
#ifndef MessageDispatcherH
#define MessageDispatcherH
//---------------------------------------------------------------------------
#include "SocketSend.h"
#include "Sigo.h"
#include "MApp.h"
//---------------------------------------------------------------------------
class MessageDispatcher : public MessageListener
{
public:
     MessageObject                                  FMessageObject;
     UFC::PHashedList<UFC::AnsiString, UFC::PtrQueue<UFC::AnsiString>*>  FConnections;
     Int32        BeginMessageNo;
     Int32        EndMessageNo ;
     BOOL         FirstTimeRun;
     BOOL         FBenchMark;
public:
     MessageDispatcher( const UFC::AnsiString& FileName, BOOL FirstTime, BOOL BenchMark, BOOL Test );
     void OnMigoMessage( UFC::AnsiString Subject,UFC::AnsiString Key, MTree* Data );
     void Run( void );
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
