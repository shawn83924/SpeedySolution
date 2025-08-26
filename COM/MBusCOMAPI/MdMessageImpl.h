// ---------------------------------------------------------------------------
// MdMessageImpl.h : Declaration of the TMdMessageImpl
// ---------------------------------------------------------------------------
#ifndef MdMessageImplH
#define MdMessageImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "MBusAPI_TLB.h"
#include <MTree.h>
//------------------------------------------------------------------------------
// Template function
// cast a object pointer to a variant
//------------------------------------------------------------------------------
template <class T> VARIANT CastToVariant( T obj )
{
	VARIANT Instance;
	Instance.vt    = VT_BYREF|VT_VOID;
	Instance.byref = obj;
	return Instance;
}
//------------------------------------------------------------------------------
// Template function
// cast a pointer variant to a object pointer.
//------------------------------------------------------------------------------
template <class T> T CastToObject( VARIANT* Instance )
{
	return static_cast<T>(Instance->byref);
}
// ---------------------------------------------------------------------------
// TMdMessageImpl     Implements IMdMessage, default interface of MdMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : MBus Message object
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TMdMessageImpl : public TCppAutoObject<IMdMessage>
{
  typedef _COM_CLASS inherited;

public:
  __fastcall TMdMessageImpl();
  __fastcall TMdMessageImpl(const System::_di_IInterface Controller);
  __fastcall TMdMessageImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
private:
	MTree*              FTree;
	MTreeEnumerator*	FTreeEnum;
	bool                FOwnData;
	VARIANT             FInstance;
private:
	MNode*	CreateStringNode(const char* FieldName, BSTR FieldValue);
	MNode*	CreateIntegerNode(const char* FieldName, int FieldValue);
	MNode*	CreateDoubleNode(const char* FieldName, double FieldValue);
	MNode*  CreateRowDataNode(const char* FieldName, SAFEARRAY* RowData , VARIANT_BOOL Compress = false );
	MNode*	NewField(const AnsiString& FieldName, VARIANT* Value );
private:
	DataTypeEnum GetNodeType( MNode* pNode );
	void         GetNodeValue( MNode* pNode, VARIANT* Value );
	BSTR         GetStringFromNode( MNode* pNode );
	SAFEARRAY*   GetRowDataFromNode( MNode* pNode );
	void         UpdateStringField( MNode* pNode, VARIANT* Value );
	void         UpdateRowDataField( MNode* pNode, VARIANT* Value, VARIANT_BOOL Compress = false );
	void         UpdateField( MNode* pNode, VARIANT* Value );
public:// IMdMessage
  STDMETHOD(Attach(VARIANT* Instance));
  STDMETHOD(Create());
  STDMETHOD(Destroy());
  STDMETHOD(GetBytesValue(BSTR FieldName, VARIANT* FieldValue));
  STDMETHOD(GetDoubleValue(BSTR FieldName, double DefaultValue, double* Value));
  STDMETHOD(GetField(BSTR FieldName, VARIANT* Value));
  STDMETHOD(GetFieldCount(long* Value));
  STDMETHOD(GetIntegerValue(BSTR FieldName, long DefaultValue, long* Value));
  STDMETHOD(GetMessageInstance(VARIANT* Value));
  STDMETHOD(GetName(long Index, BSTR* Name));
  STDMETHOD(GetStringValue(BSTR FieldName, BSTR* Value));
  STDMETHOD(GetType(long Index, DataTypeEnum* Type));
  STDMETHOD(GetValue(long Index, VARIANT* Value));
  STDMETHOD(IsValid(VARIANT_BOOL* Value));
  STDMETHOD(SetBytesValue(BSTR FieldName, VARIANT Value, VARIANT_BOOL Compress));
  STDMETHOD(SetDoubleValue(BSTR FieldName, double FieldValue));
  STDMETHOD(SetField(BSTR FieldName, VARIANT FieldValue));
  STDMETHOD(SetIntegerValue(BSTR FieldName, long FieldValue));
  STDMETHOD(SetStringValue(BSTR FieldName, BSTR FieldValue));
};




#endif //MdMessageImplH
