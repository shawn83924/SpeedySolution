#ifndef _TMdListener_H_
#define _TMdListener_H_
//------------------------------------------------------------------------------
#include "TMdTransport.h"
#include "../Migo/MSubscriber.h"
//------------------------------------------------------------------------------
class TMdListener
{
private:
	TTransport*   FTransport;
	MSubscriber*	FSubscriber;
	UFC::AnsiString FName;
	UFC::AnsiString FSubject;
	UFC::AnsiString FKey;
public:
	TMdListener( TTransport* pTransport, MessageListener* pEventListener, const char* Subject, const char* Key );
	~TMdListener(void);
};
//------------------------------------------------------------------------------
#endif
