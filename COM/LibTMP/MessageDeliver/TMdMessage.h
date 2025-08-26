#ifndef _TMdMessage_H_
#define _TMdMessage_H_
//------------------------------------------------------------------------------
#include "../Migo/MTree.h"
#include "../UFC/NameValueMessage.h"
//------------------------------------------------------------------------------
class TMdMessage
{
private:
	MTree			FTree;
	MTree*			FTreePtr;
	MTreeEnumerator*	FTreeEnum;
private:
	void SetValue( MNode* ExistNode, MNode* NewNode );
public:
	TMdMessage( void );
	~TMdMessage( void );
public:
	void		Attach( MTree* TreePtr);
	void		Detach( void );
	void		SeekToFirst( void );
	MNode*		Next( void );
	MTree*		GetMTree( void );

	void		SetStringValue( const char* FieldName, const char* Value );
	void		SetIntegerValue( const char* FieldName, int Value );
	void		SetInteger64Value( const char* FieldName, Int64 Value );
	void		SetDoubleValue( const char* FieldName, double Value );

	const char*	GetStringValue( const char* FieldName );
	const char*	GetStringValue( const char* FieldName, const char* DefaultValue );
	bool 	    GetStringValue( const char* FieldName, UFC::AnsiString& Value );
	int	      	GetIntegerValue( const char* FieldName, int DefaultValue );
	Int64	   	GetInteger64Value( const char* FieldName, Int64 DefaultValue );
	double		GetDoubleValue( const char* FieldName, double DefaultValue );

};
//------------------------------------------------------------------------------
#endif

