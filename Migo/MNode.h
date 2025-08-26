#ifndef _MNODE_H_
#define _MNODE_H_
#include "MDefinitions.h"
#include "../UFC/UFC.h"
//------------------------------------------------------------------------------
typedef union
{
	short          AsShort;
	int            AsInt;
	long           AsLong;
	long long      AsInt64;
	unsigned short AsUShort;
	unsigned int   AsUInt;
	unsigned long  AsULong;
	float          AsFloat;
	double         AsDouble;
	char*          AsString;
	unsigned char* AsRowdata;

} UnionVariant;
//------------------------------------------------------------------------------
class MNode
{
private:
	MString         m_sName;
	MDataType       m_iType;
	UFCType::Int32  m_iSize;
	UnionVariant    m_Data;
public:
	MNode( const MString& rsNodeName,short ShortData );
	MNode( const MString& rsNodeName,int IntData );
	MNode( const MString& rsNodeName,long long Int64Data );
	MNode( const MString& rsNodeName,long LongData );
	MNode( const MString& rsNodeName,unsigned short UShortData );
	MNode( const MString& rsNodeName,unsigned int UIntData );
	MNode( const MString& rsNodeName,unsigned long ULongData );
	MNode( const MString& rsNodeName,float FloatData );
	MNode( const MString& rsNodeName,double DoubleData );
	MNode( const MString& rsNodeName,const MString& StringData );
	MNode( const MString& rsNodeName,const char* StringData );
	MNode( const MString& rsNodeName,const unsigned char* Data, int Size, BOOL Compress = FALSE );
public:
	MNode();
	MNode( UFC::PStream* s );
	MNode( const MString& rsNodeName,MDataType type, int size, const void* pData);
	MNode( const MNode& refMNode );
	virtual ~MNode();
public:
	void SaveToStream( UFC::PStream* s );
	void LoadFromStream( UFC::PStream* s );
public:
	const MString& getName() const  { return m_sName; }
	MDataType      getType() const  { return m_iType; }
	UFCType::Int32 getSize() const  { return m_iSize; }
	const void*    getData() const;
	MNode&         operator =( const MNode& rhs );
        void           Set( MDataType NewType, int NewSize, const void* pData );
        BOOL           SetInt( int NewValue );
        BOOL           SetString( const MString& StringData );
}; // class MNode
//------------------------------------------------------------------------------
#endif // _MNode_h_
//------------------------------------------------------------------------------

