// ---------------------------------------------------------------------------
// MDMESSAGEIMPL : Implementation of TMdMessageImpl (CoClass: MdMessage, Interface: IMdMessage)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MdMessageImpl.h"
// ---------------------------------------------------------------------------
// TMdMessageImpl
// ---------------------------------------------------------------------------
__fastcall TMdMessageImpl::TMdMessageImpl()
:FTree( NULL )
,FTreeEnum( NULL )
,FOwnData( false )
,FInstance( CastToVariant<MTree*>( NULL ) )
{
}
// ---------------------------------------------------------------------------
// TMdMessageImpl
// ---------------------------------------------------------------------------
__fastcall TMdMessageImpl::TMdMessageImpl(const System::_di_IInterface Controller)
:inherited(Controller)
,FTree( NULL )
,FTreeEnum( NULL )
,FOwnData( false )
,FInstance( CastToVariant<MTree*>( NULL ) )
{
}
// ---------------------------------------------------------------------------
// TMdMessageImpl
// ---------------------------------------------------------------------------
__fastcall TMdMessageImpl::TMdMessageImpl(Comobj::TComObjectFactory* Factory,
									  const System::_di_IInterface Controller)
:inherited(Factory, Controller)
,FTree( NULL )
,FTreeEnum( NULL )
,FOwnData( false )
,FInstance( CastToVariant<MTree*>( NULL ) )
{
}
// ---------------------------------------------------------------------------
// TMdMessageImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TMdMessageImpl>(Comserv::GetComServer(),
                           __classid(TMdMessageImpl),
                           CLSID_MdMessage,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
//---------------------------------------------------------------------------
MNode* TMdMessageImpl::CreateStringNode(const char* FieldName, BSTR FieldValue)
{
	UTF8String UTF8FieldValue( FieldValue );

	return new MNode( FieldName, UTF8FieldValue.c_str() );
}
//---------------------------------------------------------------------------
MNode* TMdMessageImpl::CreateIntegerNode(const char* FieldName, int FieldValue)
{
    return new MNode( FieldName, FieldValue );
}
//---------------------------------------------------------------------------
MNode* TMdMessageImpl::CreateDoubleNode(const char* FieldName, double FieldValue)
{
	return new MNode( FieldName, FieldValue );
}
//---------------------------------------------------------------------------
MNode* TMdMessageImpl::CreateRowDataNode(const char* FieldName, SAFEARRAY* RowData, VARIANT_BOOL Compress )
{
	UFCType::UInt8  HUGEP *pBytes;
	long   Size;
	MNode* NewNode;

	SafeArrayGetUBound( RowData, 1, &Size );
	SafeArrayAccessData( RowData, (void HUGEP**)&pBytes);
	if( Compress == VARIANT_TRUE )
		NewNode = new MNode( FieldName, (const unsigned char*)pBytes,Size, TRUE );
	else
		NewNode = new MNode( FieldName, (const unsigned char*)pBytes,Size, FALSE );
	SafeArrayUnaccessData( RowData );
	return NewNode;
}
//---------------------------------------------------------------------------
MNode* TMdMessageImpl::NewField(const AnsiString& FieldName, VARIANT* Value)
{
    switch( Value->vt )
    {
		case VT_BSTR:	return CreateStringNode( FieldName.c_str(), Value->bstrVal );
        case VT_I2:		return CreateIntegerNode( FieldName.c_str(), (UFCType::Int32)Value->iVal );
        case VT_UI2:	return CreateIntegerNode( FieldName.c_str(), (UFCType::Int32)Value->uiVal );
        case VT_I4:		return CreateIntegerNode( FieldName.c_str(), (UFCType::Int32)Value->intVal );
        case VT_UI4:	return CreateIntegerNode( FieldName.c_str(), (UFCType::Int32)Value->uintVal );
		case VT_R4:		return CreateDoubleNode( FieldName.c_str(), (double)Value->fltVal );
		case VT_R8:		return CreateDoubleNode( FieldName.c_str(), Value->dblVal );
		case VT_ARRAY|VT_UI1: return CreateRowDataNode( FieldName.c_str(), Value->parray );
		default:        return new MNode( FieldName.c_str(), "NULL" );
    }
}
//---------------------------------------------------------------------------
void TMdMessageImpl::UpdateField( MNode* pNode, VARIANT* Value )
{
	switch( Value->vt )
	{
		case VT_BSTR:	UpdateStringField( pNode, Value );
						break;
		case VT_I2:		pNode->Set( M_SHORT, sizeof(short), &Value->iVal );
						break;
		case VT_UI2:	pNode->Set( M_USHORT, sizeof(unsigned short), &Value->uiVal );
						break;
		case VT_I4:		pNode->Set( M_INT, sizeof(int), &Value->intVal );
						break;
		case VT_UI4:	pNode->Set( M_UINT, sizeof(unsigned int), &Value->uintVal );
						break;
		case VT_R4:		pNode->Set( M_FLOAT, sizeof(float), &Value->fltVal );
						break;
		case VT_R8:     pNode->Set( M_DOUBLE, sizeof(double), &Value->dblVal );
						break;
		case VT_ARRAY|VT_UI1: UpdateRowDataField( pNode, Value );
						break;
	}
}
//---------------------------------------------------------------------------
void TMdMessageImpl::UpdateStringField( MNode* pNode, VARIANT* Value )
{
	UTF8String cStr( Value->bstrVal );

	pNode->Set( M_STRING, cStr.Length() + 1 , cStr.c_str() );
}
//---------------------------------------------------------------------------
void TMdMessageImpl::UpdateRowDataField( MNode* pNode, VARIANT* Value, VARIANT_BOOL Compress )
{
	UFCType::UInt8  HUGEP *pBytes;
	long   Size;

	SafeArrayGetUBound( Value->parray, 1, &Size );
	SafeArrayAccessData( Value->parray, (void HUGEP**)&pBytes);
	if( Compress == VARIANT_TRUE )
		pNode->Set( M_COMPRESSEDDATA, Size, pBytes );
	else
		pNode->Set( M_ROWDATA, Size, pBytes );
	SafeArrayUnaccessData( Value->parray );
}
//---------------------------------------------------------------------------
BSTR TMdMessageImpl::GetStringFromNode( MNode* pNode )
{
	UTF8String UTF8Str( (char*)pNode->getData() );
	int   len = pNode->getSize();

	if( len > 0 && UTF8Str.Length() > 0  )
	{
		WideString ResultBSTR( UTF8Str );
		return SysAllocString( ResultBSTR.c_bstr() );
	}
	WideString NullBSTR( "NULL" );
	return SysAllocString( NullBSTR.c_bstr() );
}
//---------------------------------------------------------------------------
SAFEARRAY* TMdMessageImpl::GetRowDataFromNode( MNode* pNode )
{
	char* Data = (char*)pNode->getData();
	int   Size = pNode->getSize();

	if( Size > 0  )
	{
	   UFCType::UInt8 HUGEP *pBytes;
	   SAFEARRAY * psa;
	   SAFEARRAYBOUND rgsabound[1]; ///< 1 dimension array

	   rgsabound[0].lLbound = 0;
	   rgsabound[0].cElements = Size;
	   psa = SafeArrayCreate( VT_UI1, 1, rgsabound);
	   SafeArrayAccessData(psa, (void HUGEP**)&pBytes);// Get a pointer to the elements of the array.
	   memcpy( pBytes, Data, Size );
	   SafeArrayUnaccessData( psa );
	   return psa;
	}
	return NULL;
}
//---------------------------------------------------------------------------
void TMdMessageImpl::GetNodeValue( MNode* pNode, VARIANT* Value )
{
    if( pNode != NULL && Value != NULL )
    {
        const void* data = pNode->getData();
        if( data != NULL )
        {
			switch( pNode->getType() )
  			{
				case M_STRING: Value->vt = VT_BSTR;
							   Value->bstrVal = GetStringFromNode( pNode );
							   return;
				case M_SHORT:  Value->vt = VT_I4;
							   Value->iVal = (int)*((short*)data);
							   return;
				case M_USHORT: Value->vt = VT_I4;
							   Value->uiVal = (int)*((unsigned_short*)data);
							   return;
				case M_LONG:   Value->vt = VT_I4;
							   Value->lVal = (int)*((long*)data);
							   return;
				case M_INT:    Value->vt = VT_I4;
							   Value->intVal = *((int*)data);
							   return;
				case M_ULONG:  Value->vt = VT_I4;
							   Value->ulVal = (int)*((long*)data);
							   return;
				case M_UINT:   Value->vt = VT_I4;
							   Value->uintVal = (int)*((unsigned*)data);
							   return;
				case M_FLOAT:  Value->vt = VT_R8;
							   Value->dblVal = (double)*((float*)data);
							   return;
				case M_DOUBLE: Value->vt = VT_R8;
							   Value->dblVal = *((double*)data);
							   return;
				case M_COMPRESSEDDATA:
				case M_ROWDATA:V_VT(Value)    = VT_ARRAY|VT_UI1;
							   V_ARRAY(Value) = GetRowDataFromNode( pNode );
							   return;
				default:       Value->vt = VT_I4;
							   Value->intVal = 0;
							   return;
			}
		}
    }
}
//---------------------------------------------------------------------------
DataTypeEnum TMdMessageImpl::GetNodeType( MNode* pNode )
{
	switch( pNode->getType() )
	{
		case M_SHORT:
		case M_USHORT:
		case M_LONG:
		case M_INT:
		case M_ULONG:
		case M_UINT:          return DataTypeEnum::dtInteger;
		case M_STRING:        return DataTypeEnum::dtString;
		case M_FLOAT:
		case M_DOUBLE:        return DataTypeEnum::dtDouble;
		case M_COMPRESSEDDATA:
		case M_ROWDATA:       return DataTypeEnum::dtRowData;
		default:              return DataTypeEnum::dtUnknown;
	}
}
//---------------------------------------------------------------------------
//
//  Implement Interface ITMdMessage
//
//---------------------------------------------------------------------------
// Method Create
// Create the TMdMessage instance
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::Create()
{
	if( FOwnData == true && FTree != NULL )
		delete FTree;
	FTree     = new MTree();
	FOwnData  = true;
	FInstance = CastToVariant<MTree*>( FTree );
	return S_OK;
}
//---------------------------------------------------------------------------
// Destroy the TMdMessage instance
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::Destroy()
{
    if( FOwnData == true )
    {
        FOwnData = false;
        if( FTree != NULL )
            delete FTree;
    }
	FInstance = CastToVariant<MTree*>( NULL );
    FTree     = NULL;
    return S_OK;
}
//---------------------------------------------------------------------------
// Attach a MNode instance to TMdMessage (Internal use only)
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::Attach(VARIANT* Instance)
{
	if( Instance->vt == VT_BYREF|VT_VOID && Instance->byref != NULL )
	{
		if( FOwnData == true )
		{
			if( FTree != NULL )
				delete FTree;
			FOwnData = false;
		}
		FOwnData  = false;
		FInstance = *Instance;
		FTree     = CastToObject<MTree*>( Instance );
		return S_OK;
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
// Get the MNode instance of this TMdMessage (Internal use only)
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::GetMessageInstance(VARIANT* Value)
{
	*Value = FInstance;
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::IsValid(VARIANT_BOOL* Value)
{

	if( FTree != NULL )
		*Value = VARIANT_TRUE;
	else
		*Value = VARIANT_FALSE;
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::GetField(BSTR FieldName, VARIANT* Value)
{
    AnsiString AnsiName( FieldName );
    MNode* pNode = FTree->get( AnsiName.c_str() );

	if( pNode != NULL )
    {
	    GetNodeValue( pNode, Value );
		return S_OK;
	}
	Value->vt = VT_I4;
	Value->intVal = 0;
	return S_FALSE;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::SetField(BSTR FieldName, VARIANT FieldValue)
{
	AnsiString AnsiFieldName( FieldName );
	MNode* pNode = FTree->get( AnsiFieldName.c_str() );

	if( pNode != NULL )
	{
		UpdateField( pNode, &FieldValue );
		return S_OK;
	}
	pNode = NewField( AnsiFieldName, &FieldValue );
	FTree->append( pNode );
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::GetFieldCount(long* Value)
{
	*Value = FTree->getNodeCount();
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::GetStringValue(BSTR FieldName, BSTR* Value)
{
	AnsiString AnsiName( FieldName );
	MNode* pNode = FTree->get( AnsiName.c_str() );

	if( pNode != NULL  &&  pNode->getType() == M_STRING )
	{
		*Value = GetStringFromNode( pNode );
		return S_OK;
	}
	*Value = SysAllocString( L"NULL" );;
	return S_FALSE;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::SetStringValue(BSTR FieldName, BSTR FieldValue)
{
   VARIANT vFieldValue;

   vFieldValue.vt      = VT_BSTR;
   vFieldValue.bstrVal = FieldValue;
   SetField( FieldName, vFieldValue );
   return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::GetIntegerValue(BSTR FieldName, long DefaultValue, long* Value)

{
   VARIANT FieldValue;

   if( GetField( FieldName, &FieldValue ) ==  S_OK )
   {
	   if( FieldValue.vt == VT_I4 )
       {
           *Value = FieldValue.intVal;
           return S_OK;
       }
   }
   *Value = DefaultValue;
   return S_FALSE;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::SetIntegerValue(BSTR FieldName, long FieldValue)
{
	VARIANT vFieldValue;

	vFieldValue.vt      = VT_I4;
	vFieldValue.intVal  = FieldValue;
	SetField( FieldName, vFieldValue );
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::GetDoubleValue(BSTR FieldName, double DefaultValue, double* Value)

{
	VARIANT FieldValue;

	if( GetField( FieldName, &FieldValue ) ==  S_OK )
	{
		if( FieldValue.vt == VT_R8 )
		{
			*Value = FieldValue.dblVal;
			return S_OK;
		}
	}
	*Value = DefaultValue;
	return S_FALSE;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::SetDoubleValue(BSTR FieldName, double FieldValue)
{
	VARIANT vFieldValue;

	vFieldValue.vt      = VT_R8;
	vFieldValue.dblVal  = FieldValue;
	SetField( FieldName, vFieldValue );
	return S_OK;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::GetBytesValue(BSTR FieldName, VARIANT* FieldValue)
{
	AnsiString AnsiName( FieldName );
	MNode* pNode = FTree->get( AnsiName.c_str() );


	if( pNode != NULL  &&
		(pNode->getType() == M_ROWDATA || pNode->getType() == M_COMPRESSEDDATA) )
	{
		V_VT(FieldValue)    = VT_ARRAY|VT_UI1;
		V_ARRAY(FieldValue) = GetRowDataFromNode( pNode );
		return S_OK;
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::SetBytesValue(BSTR FieldName, VARIANT Value, VARIANT_BOOL Compress)

{
	if( V_VT( &Value ) == VT_ARRAY|VT_UI1 )
	{
		AnsiString AnsiFieldName( FieldName );
		MNode*     pNode = FTree->get( AnsiFieldName.c_str() );

		if( pNode != NULL ) ///< Node exists, update it.
		{
			UpdateRowDataField( pNode, &Value, Compress );
			return S_OK;
		}
		else ///< Node Not exists, Create a new one.
		{
			pNode = CreateRowDataNode( AnsiFieldName.c_str(), Value.parray , Compress );
			FTree->append( pNode );
			return S_OK;
		}
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
// Get Name of the node
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::GetName(long Index, BSTR* Name)
{
	MNode* pNode = FTree->get( Index );

	if( pNode != NULL )
	{
		WideString NameStr( pNode->getName().c_str() );
		*Name = NameStr.Copy();
		return S_OK;
	}
	WideString NullStr( "NULL" );
	*Name = NullStr.Copy();
	return S_FALSE;
}
//---------------------------------------------------------------------------
// Get node value
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::GetValue(long Index, VARIANT* Value)
{
	MNode* pNode = FTree->get( Index );

	if( pNode != NULL )
	{
		GetNodeValue( pNode, Value );
		return S_OK;
	}
	return S_FALSE;
}
//---------------------------------------------------------------------------
// Get Node data type.
//---------------------------------------------------------------------------
STDMETHODIMP TMdMessageImpl::GetType(long Index, DataTypeEnum* Type)
{
	MNode* pNode = FTree->get( Index );

	if( pNode != NULL )
	{
		*Type = GetNodeType( pNode );
		return S_OK;
	}
	*Type = DataTypeEnum::dtUnknown;
	return S_FALSE;
}
//---------------------------------------------------------------------------
