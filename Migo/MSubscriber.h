// MSubscriber.h
//----------------------------------------------------------------------------------------------------------------------
#ifndef _MSubscriber_h_
#define _MSubscriber_h_
//----------------------------------------------------------------------------------------------------------------------
#include "MTree.h"
#include "MApp.h"
//----------------------------------------------------------------------------------------------------------------------
//   Interface MessageListener
//----------------------------------------------------------------------------------------------------------------------
class MessageListener
{
public:
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* ) = 0;
#ifndef _WIN32    
    virtual ~MessageListener( void ) {}
#endif 
};
//----------------------------------------------------------------------------------------------------------------------
class RequestMessageListener
{
public:
    virtual bool OnRequest( const UFC::AnsiString& Subject,  MTree* Req, MTree* Response ) = 0;
#ifndef _WIN32    
    virtual ~RequestMessageListener( void ) {}
#endif 
};
//----------------------------------------------------------------------------------------------------------------------
class MSubscriber : public MComponent
{
private:
        MessageListener* m_EventListener;
public:
	MSubscriber( const MString& rsObjectName,
		     const MString& rsSubjectName,
		     const MString& rsSubjectKeys );
	MessageListener* getListener( void ){ return m_EventListener; }
	virtual ~MSubscriber();
	Mboolean addListener( MessageListener* pListener );
	static MSubscriber* downCast(MComponent* pMComponent);
}; // class MSubscriber
//----------------------------------------------------------------------------------------------------------------------
#endif // _MSubscriber_h_


