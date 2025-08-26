//---------------------------------------------------------------------------
#ifndef MessageDispatcherH
#define MessageDispatcherH
//---------------------------------------------------------------------------
#include "SocketSend.h"
#include "../Migo/Sigo.h"
//---------------------------------------------------------------------------
class MessageDispatcher : public MessageListener
{
public:
     MessageObject                        FMessageObject;
     UFC::PHashedList<UFC::AnsiString, UFC::PtrQueue<UFC::AnsiString>*>  FConnections;
public:
     MessageDispatcher( const UFC::AnsiString& FileName, BOOL FirstTime );
     void OnMigoMessage( UFC::AnsiString Subject,UFC::AnsiString Key, MTree* Data );
     void Run( void );
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
