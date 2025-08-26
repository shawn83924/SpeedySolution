#include "TMdMessage.h"
//------------------------------------------------------------------------------
TMdMessage::TMdMessage(void)
{
	Attach( &FTree );
}
//------------------------------------------------------------------------------
TMdMessage::~TMdMessage(void)
{
	FTree.Clear();
}
//------------------------------------------------------------------------------
void TMdMessage::Attach(MTree* TreePtr)
{
    FTreePtr = TreePtr;
    SeekToFirst();
}
//------------------------------------------------------------------------------
void TMdMessage::Detach(void)
{
    Attach( &FTree );
}
//------------------------------------------------------------------------------
void TMdMessage::SeekToFirst(void)
{
    FTreeEnum = FTreePtr->newEnumerator();
}
//------------------------------------------------------------------------------
MNode* TMdMessage::Next( void )
{
    return FTreeEnum->next();
}
//------------------------------------------------------------------------------
MTree* TMdMessage::GetMTree(void)
{
    return FTreePtr;
}
//------------------------------------------------------------------------------
void TMdMessage::SetValue(MNode* ExistNode, MNode* NewNode)
{
    if( ExistNode == NULL )
    {
		FTreePtr->append( NewNode );
	}
	else
	{
		(*ExistNode) = (*NewNode);
		delete NewNode;
	}
}
//------------------------------------------------------------------------------
void TMdMessage::SetStringValue( const char* FieldName, const char* Value )
{
    if( Value != NULL )
    {
		MNode* ExistNode	= FTreePtr->get( FieldName );
		MNode* NewNode		= new MNode( FieldName, Value );
		SetValue( ExistNode, NewNode );
	}
}
//------------------------------------------------------------------------------
void TMdMessage::SetIntegerValue( const char* FieldName, int Value )
{
	MNode* ExistNode	= FTreePtr->get( FieldName );
	MNode* NewNode		= new MNode( FieldName, Value );
	SetValue( ExistNode, NewNode );
}
//------------------------------------------------------------------------------
void TMdMessage::SetInteger64Value( const char* FieldName, Int64 Value )
{
	MNode* ExistNode	= FTreePtr->get( FieldName );
	MNode* NewNode		= new MNode( FieldName, Value );
	SetValue( ExistNode, NewNode );
}
//------------------------------------------------------------------------------
void TMdMessage::SetDoubleValue( const char* FieldName, double Value )
{
    MNode* ExistNode	= FTreePtr->get( FieldName );
    MNode* NewNode		= new MNode( FieldName, Value );
    SetValue( ExistNode, NewNode );
}
//------------------------------------------------------------------------------
const char* TMdMessage::GetStringValue( const char* FieldName )
{
    MNode* pNode = FTreePtr->get( FieldName );

    if( pNode != NULL )
    {
        if( pNode->getType() == M_STRING )
        {
            const void* data = pNode->getData();
            return (const char*)data;
        }
    }
    return NULL;
}
//------------------------------------------------------------------------------
const char* TMdMessage::GetStringValue( const char* FieldName, const char* DefaultValue )
{
	MNode* pNode = FTreePtr->get( FieldName );

	if( pNode != NULL )
	{
		if( pNode->getType() == M_STRING )
	{
			const void* data = pNode->getData();
			return (const char*)data;
		}
	}
	return DefaultValue;
}
//------------------------------------------------------------------------------
bool TMdMessage::GetStringValue( const char* FieldName, UFC::AnsiString& Value )
{
	MNode* pNode = FTreePtr->get( FieldName );

	if( pNode != NULL )
	{
		if( pNode->getType() == M_STRING )
		{
			const void* data = pNode->getData();
			int Size = pNode->getSize();
			Value = UFC::AnsiString( (const char*)data, Size );
			return true;
		}
	}
	return false;
}
//------------------------------------------------------------------------------
int TMdMessage::GetIntegerValue( const char* FieldName, int DefaultValue )
{
    MNode* pNode = FTreePtr->get( FieldName );
	
    if( pNode != NULL )
    {
		MDataType DataType = pNode->getType();
		if( DataType == M_INT )
		{
			const void* data = pNode->getData();
			return *((int*)data);
		}
	}
	return DefaultValue;
}
//------------------------------------------------------------------------------
Int64 TMdMessage::GetInteger64Value( const char* FieldName, Int64 DefaultValue )
{
	MNode* pNode = FTreePtr->get( FieldName );

	if( pNode != NULL )
	{
		MDataType DataType = pNode->getType();
		if( DataType == M_INT64 )
		{
			const void* data = pNode->getData();
			Int64 *int64Ptr = (Int64*)data;
			return *int64Ptr;
		}
	}
	return DefaultValue;
}
//------------------------------------------------------------------------------
double TMdMessage::GetDoubleValue( const char* FieldName, double DefaultValue )
{
	MNode* pNode = FTreePtr->get( FieldName );

	if( pNode != NULL )
	{
		MDataType DataType = pNode->getType();
		if( DataType == M_DOUBLE )
		{
			const void* data = pNode->getData();
			return *((double*)data);
		}
	}
    return DefaultValue;
}
//------------------------------------------------------------------------------

