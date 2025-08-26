#ifndef MCOMPONENT_H
#define MCOMPONENT_H
#include "MDefinitions.h"
class MApp;
//------------------------------------------------------------------------------
typedef enum
{
  ctUnknown    = 0,
  ctSubscriber = 1,
  ctPublisher  = 2

} ComponentType;
//------------------------------------------------------------------------------
class MComponent
{
protected:
	MString		m_ObjectName;
	MString	    	m_SubjectName;
	MString	    	m_SubjectKey;
	MApp*		FMApp;
	ComponentType   FComType;
private:
        MComponent();
public:
	MComponent( const MString& ObjName, const MString& SubjectName, const MString& Key, ComponentType Type );	
	~MComponent( void ){}
	void           setMApp( MApp* mapp )                         { FMApp = mapp; }
        const MString& getObjectName( void )                         { return m_ObjectName; }
	void           setObjectName( const MString& rsObjectName)   { m_ObjectName  = rsObjectName; }
	const MString& getSubjectName( void )                        { return m_SubjectName; }
	void           setSubjectName( const MString& rsSubjectName) { m_SubjectName = rsSubjectName; }
        const MString& getSubjectKey( void )                         { return m_SubjectKey; }
	void           setSubjectKey( const MString& rsSubjectKey)   { m_SubjectKey  = rsSubjectKey; }
	ComponentType  getComType( void )                            { return FComType; }
	void           setComType( ComponentType Type )              { FComType = Type; }

};
//------------------------------------------------------------------------------
#endif
