#include "MSubscriber.h"
//------------------------------------------------------------------------------
MSubscriber::MSubscriber( const MString& rsObjectName,
			  const MString& rsSubjectName,
                          const MString& rsSubjectKeys )
:MComponent( rsObjectName, rsSubjectName, rsSubjectKeys, ctSubscriber )
,m_EventListener( NULL )
{
}
//------------------------------------------------------------------------------
MSubscriber::~MSubscriber()
{
}
//------------------------------------------------------------------------------
Mboolean MSubscriber::addListener( MessageListener* pListener )
{
    if( pListener != NULL )
    {
        m_EventListener = pListener;
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
MSubscriber* MSubscriber::downCast(MComponent* pMComponent)
{
    if( pMComponent != NULL && pMComponent->getComType() == ctSubscriber )
	return (MSubscriber*)pMComponent;
    return NULL;
}
//------------------------------------------------------------------------------
