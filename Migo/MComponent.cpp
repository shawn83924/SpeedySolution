#include "MComponent.h"
//------------------------------------------------------------------------------
MComponent::MComponent( const MString& ObjName, const MString& SubjectName, const MString& Key, ComponentType Type )
:m_ObjectName( ObjName ),
 m_SubjectName( SubjectName ),
 m_SubjectKey( Key ),
 FMApp( NULL ),
 FComType( Type )
{
}
//------------------------------------------------------------------------------
MComponent::MComponent()
:m_ObjectName( "" ),
 m_SubjectName( "" ),
 m_SubjectKey( ""),
 FMApp( NULL ),
 FComType( ctUnknown )
{
}
//------------------------------------------------------------------------------
