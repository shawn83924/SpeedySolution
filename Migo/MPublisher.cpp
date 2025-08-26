#include "MPublisher.h"
//------------------------------------------------------------------------------
MPublisher::MPublisher( const MString& rsObjectName, const MString& rsSubjectName, const MString& rsSubjectKeys )
:MComponent( rsObjectName,  rsSubjectName, rsSubjectKeys, ctPublisher ),
 m_pHandle( NULL ),
 m_bIsBind( FALSE )
{

}
//------------------------------------------------------------------------------
MPublisher::~MPublisher()
{
}
//------------------------------------------------------------------------------
void MPublisher::send( const MTree& refMTree )
{
}
//------------------------------------------------------------------------------
void MPublisher::send_by_key( MTree& refMTree, const MString& sKey)
{
	if( FMApp != NULL )
    	FMApp->send_by_key( getSubjectName(), sKey, refMTree );
}
//------------------------------------------------------------------------------
void MPublisher::sendWithReply(	const MTree& rMTree , MEventListener* pEventListener,
					MSubscriber* &pSubscriber)
{
}
//------------------------------------------------------------------------------
void MPublisher::sendWithReply(	const MTree& rMTree , MEventListener* pEventListener,
					unsigned long iTimeout, MSubscriber*& pSubscriber)
{
}
//------------------------------------------------------------------------------
void MPublisher::binding()
{
}
//------------------------------------------------------------------------------
void MPublisher::rebind()
{
}
//------------------------------------------------------------------------------
void MPublisher::setReplySubject(const MString& rsReplySubject)
{
	m_sReplySubject = rsReplySubject;
}
//------------------------------------------------------------------------------
void MPublisher::setReplyKeys(const MString& rsReplyKeys )
{
	m_sReplyKeys = rsReplyKeys;
}
//------------------------------------------------------------------------------
MPublisher* MPublisher::downCast(MComponent* pMComponent)
{
	if( pMComponent != NULL )
	{
		if( pMComponent->getComType() == ctPublisher )
		{
			return (MPublisher*)pMComponent;
		}
	}
	return NULL;
}
//------------------------------------------------------------------------------
