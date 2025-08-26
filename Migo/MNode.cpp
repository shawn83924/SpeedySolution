#include "MNode.h"
#include "../UFC/zlibutility.h"
#include "AnsiString.h"
//------------------------------------------------------------------------------
using UFCType::UInt32;
using UFCType::Int32;
using UFCType::UInt8;
using UFCType::Int64;
//------------------------------------------------------------------------------
MNode::MNode()
:m_sName(""),
 m_iType( M_UNKNOW ),
 m_iSize( 0 )
{
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,short ShortData )
:m_sName( rsNodeName ),
 m_iType( M_SHORT ),
 m_iSize( sizeof(short) ) ///< 16 bits
{
    m_Data.AsShort = ShortData;
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,int IntData )
:m_sName( rsNodeName ),
 m_iType( M_INT ),
 m_iSize( sizeof(int) ) ///< 32 bits
{
	m_Data.AsInt = IntData;
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,long long Int64Data )
:m_sName( rsNodeName ),
 m_iType( M_INT64 ),
 m_iSize( sizeof(long long) ) ///< 64 bits
{
	m_Data.AsInt64 = Int64Data;
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,long LongData )
:m_sName( rsNodeName ),
 m_iType( M_LONG ),
 m_iSize( sizeof(long) ) ///< 32 bits
{
    m_Data.AsLong = LongData;
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,unsigned short UShortData )
:m_sName( rsNodeName ),
 m_iType( M_USHORT ),
 m_iSize( sizeof(unsigned short) )  ///< 16 bits
{
    m_Data.AsUShort = UShortData;
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,unsigned int UIntData )
:m_sName( rsNodeName ),
 m_iType( M_UINT ),
 m_iSize( sizeof(unsigned int) ) ///< 32 bits
{
    m_Data.AsUInt = UIntData;
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,unsigned long ULongData )
:m_sName( rsNodeName ),
 m_iType( M_ULONG ),
 m_iSize( sizeof(unsigned long) ) ///< 32 bits
{
    m_Data.AsULong = ULongData;
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,float FloatData )
:m_sName( rsNodeName ),
 m_iType( M_FLOAT ),
 m_iSize( sizeof(float) ) ///< 32 bits
{
    m_Data.AsFloat = FloatData;
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,double DoubleData )
:m_sName( rsNodeName ),
 m_iType( M_DOUBLE ),
 m_iSize( sizeof(double) ) ///< 64 bits
{
    m_Data.AsDouble = DoubleData;
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,const MString& StringData )
:m_sName( rsNodeName ),
 m_iType( M_STRING ),
 m_iSize( StringData.Length() + 1 ) ///< variable size
{
    m_Data.AsString = new char[ m_iSize ];
    strcpy( m_Data.AsString, StringData.c_str() );
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,const char* StringData )
:m_sName( rsNodeName ),
 m_iType( M_STRING ),
 m_iSize( (UFCType::Int32)strlen(StringData)+1 )  ///< variable size
{
    m_Data.AsString = new char[ m_iSize ];
    strcpy( m_Data.AsString, StringData );
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName,const unsigned char* Data, int Size, BOOL Compress )
:m_sName( rsNodeName ),
 m_iSize( Size )
{
    if( Compress == TRUE )
		m_iType = M_COMPRESSEDDATA; ///< Need compress when Save this Node to Stream.
	else
		m_iType = M_ROWDATA;
    m_Data.AsRowdata = new unsigned char[ m_iSize ];
    memcpy( m_Data.AsRowdata, Data, m_iSize );
}
//------------------------------------------------------------------------------
MNode::MNode( const MString& rsNodeName, MDataType type, int size, const void* pData)
:m_sName( rsNodeName ),
 m_iType( type ),
 m_iSize( size )
{
    switch( type )
    {
        case M_INT:	      m_Data.AsInt    = *((const int*)pData);break;
        case M_INT64:     m_Data.AsInt64  = *((const long long*)pData);break;
        case M_UINT:	  m_Data.AsUInt   = *((const unsigned int*)pData);break;
        case M_SHORT:	  m_Data.AsShort  = *((const short*)pData);break;
        case M_USHORT:    m_Data.AsUShort = *((const unsigned short*)pData);break;
        case M_LONG:	  m_Data.AsLong   = *((const long*)pData);break;
        case M_ULONG:	  m_Data.AsULong  = *((const unsigned long*)pData);break;
        case M_FLOAT:     m_Data.AsFloat  = *((const float*)pData);break;
        case M_DOUBLE:    m_Data.AsDouble = *((const double*)pData);break;
        case M_STRING:    m_Data.AsString = new char[ m_iSize ];
                          memcpy( m_Data.AsString, pData, m_iSize );
                          break;
        case M_COMPRESSEDDATA:
        case M_ROWDATA:   m_Data.AsRowdata = new unsigned char[ m_iSize ];
                          memcpy( m_Data.AsRowdata, pData, m_iSize );
                          break;
        default:          break;
    }
}
//------------------------------------------------------------------------------
MNode::MNode( const MNode& refMNode )
:m_sName( refMNode.m_sName ),
 m_iType( refMNode.m_iType ),
 m_iSize( refMNode.m_iSize )
{
    if( m_iType == M_STRING )
    {
        m_Data.AsString = new char[ m_iSize ];
        strcpy( m_Data.AsString, refMNode.m_Data.AsString );
    }
    else if( m_iType == M_ROWDATA || m_iType == M_COMPRESSEDDATA )
    {
        m_Data.AsRowdata = new unsigned char[ m_iSize ];
        memcpy( m_Data.AsRowdata, refMNode.m_Data.AsRowdata , m_iSize );
    }
    else
        m_Data = refMNode.m_Data;
        //memcpy( &m_Data, &refMNode.m_Data, m_iSize );
}
//------------------------------------------------------------------------------
MNode::MNode( UFC::PStream* Stream )
{
    LoadFromStream( Stream );
}
//------------------------------------------------------------------------------
MNode::~MNode()
{
    if( m_iType == M_STRING )
        delete [] m_Data.AsString;
    else if( m_iType == M_ROWDATA || m_iType == M_COMPRESSEDDATA )
        delete [] m_Data.AsRowdata;
}
//------------------------------------------------------------------------------
const void* MNode::getData() const
{
    if( m_iType == M_STRING )
        return (const void*)m_Data.AsString;
    else if( m_iType == M_ROWDATA || m_iType == M_COMPRESSEDDATA  )
        return (const void*)m_Data.AsRowdata;
    else
        return (const void*)&m_Data;
}
//------------------------------------------------------------------------------
void  MNode::LoadFromStream( UFC::PStream* Stream )
{
    UFC::NInt32  DataType;
    UFC::NInt32  DataSize;

    m_sName.LoadFromStream( Stream );
    DataSize.LoadFromStream( Stream );
    DataType.LoadFromStream( Stream );
    m_iType = (MDataType) DataType.ToInt32();
    m_iSize = DataSize.ToInt32();
    if( m_iType == M_STRING )
	{
        m_Data.AsString = new char[ m_iSize ];
        Stream->Read( m_Data.AsString, m_iSize );
	}
    else if( m_iType == M_ROWDATA )
    {
        m_Data.AsRowdata = new unsigned char[ m_iSize ];
        Stream->Read( m_Data.AsRowdata, m_iSize );
    }
	else if( m_iType == M_COMPRESSEDDATA ) ///< Uncompress it!!
    {
        UFC::CompressedData cData( Stream, m_iSize );
        UInt32              SrcSize = cData.GetSourceSize();

        m_Data.AsRowdata = new unsigned char[ SrcSize ];
        UFC::Uncompress( &cData, &SrcSize, m_Data.AsRowdata );
		m_iSize = (UFCType::Int32) SrcSize;
	}
	else if( m_iType == M_DOUBLE )
	{
		UFC::NDouble Double;
		Double.LoadFromStream( Stream );
		m_Data.AsDouble = Double.ToDouble();
	}
	else if( m_iType == M_INT64 )
	{
		UFC::NInt64 Integer64;
		Integer64.LoadFromStream( Stream );
		m_Data.AsInt64 = Integer64.ToInt64();
	}
    else ///< for M_INT,M_UINT,M_SHORT,M_USHORT,M_LONG,M_ULONG,M_FLOAT
    {
		UInt8 Data[ 4 ];
        Stream->Read( Data, 4 );
        m_Data.AsInt = UFC::NInt32::ToInt32( Data );
    }
}
//------------------------------------------------------------------------------
void MNode::SaveToStream( UFC::PStream* Stream )
{
    if( m_iType == M_COMPRESSEDDATA )
    {
        UFC::CompressedData* cData = UFC::Compress( m_Data.AsRowdata, m_iSize ); ///< Compress it!!
		
        if( cData != NULL )
        {
            if( cData->GetSize() > (UInt32)m_iSize  )
            {
                UFC::NInt32  DataType( (UFCType::Int32)M_ROWDATA );
                UFC::NInt32  DataSize( m_iSize );

                m_sName.SaveToStream( Stream ); ///< Name
                DataSize.SaveToStream( Stream );///< Size
                DataType.SaveToStream( Stream );///< Type
                Stream->Write( m_Data.AsRowdata, m_iSize );///< Data
            }
            else
            {
                UFC::NInt32  DataType( (UFCType::Int32)m_iType );
                UFC::NInt32  DataSize( cData->GetSize() );

                m_sName.SaveToStream( Stream ); ///< Name
                DataSize.SaveToStream( Stream ); ///< Size
                DataType.SaveToStream( Stream ); ///< Type
                Stream->Write( cData->GetData(), cData->GetSize() ); //Data
            }
            delete cData;
        }
    }
	else ///< None compressed data.
    {
	    UFC::NInt32  DataType( (UFCType::Int32)m_iType );
        UFC::NInt32  DataSize( m_iSize );

        m_sName.SaveToStream( Stream ); ///< Name
        DataSize.SaveToStream( Stream );///< Size
        DataType.SaveToStream( Stream );///< Type

        if( m_iType == M_STRING )
                Stream->Write( m_Data.AsString, m_iSize );///< String Data
        else if( m_iType == M_ROWDATA )
                Stream->Write( m_Data.AsRowdata, m_iSize );///< Row Data
        else if( m_iType == M_DOUBLE )
        {
                UFC::NDouble Data( m_Data.AsDouble );
                Data.SaveToStream( Stream );          ///< Double Data
        }
        else if( m_iType == M_INT64 )
        {
                UFC::NInt64 Data( m_Data.AsInt64 );
                Data.SaveToStream( Stream );          ///< Int64 Data
        }
        else ///< for M_INT,M_UINT,M_SHORT,M_USHORT,M_LONG,M_ULONG,M_FLOAT
        {
                UFC::NInt32 Data( m_Data.AsInt );
                Stream->Write( Data.GetBytes(), 4 ); ///< size < Int32 Data
        }
    }
}
//------------------------------------------------------------------------------
MNode&  MNode::operator =( const MNode& rhs )
{
	if( this != &rhs )
	{
        if( m_iType == M_STRING )
            delete [] m_Data.AsString;
        else if( m_iType == M_ROWDATA || m_iType == M_COMPRESSEDDATA )
            delete [] m_Data.AsRowdata;
        m_sName = rhs.m_sName;
        m_iType = rhs.m_iType;
        m_iSize = rhs.m_iSize;
        if( rhs.m_iType == M_STRING )
        {
            m_Data.AsString = new char[ m_iSize ];
            strcpy( m_Data.AsString, rhs.m_Data.AsString );
        }
        else if( rhs.m_iType == M_ROWDATA ||  rhs.m_iType == M_COMPRESSEDDATA )
        {
            m_Data.AsRowdata = new unsigned char[ m_iSize ];
            memcpy( m_Data.AsRowdata, rhs.m_Data.AsRowdata , m_iSize );
        }
	else ///< for M_INT,M_INT64,M_UINT,M_SHORT,M_USHORT,M_LONG,M_ULONG,M_FLOAT
	    memcpy( &m_Data, &rhs.m_Data, m_iSize );
	}
	return *this;
}
//------------------------------------------------------------------------------
void MNode::Set( MDataType NewType, int NewSize, const void* pData)
{
    if( m_iType == M_STRING )
        delete [] m_Data.AsString;
    else if( m_iType == M_ROWDATA || m_iType == M_COMPRESSEDDATA )
        delete [] m_Data.AsRowdata;

    m_iType = NewType;
    m_iSize = NewSize ;
    switch( m_iType )
    {
        case M_INT:	    m_Data.AsInt    = *((const int*)pData);break;
        case M_INT64:	m_Data.AsInt64  = *((const long long*)pData);break;
        case M_UINT:    m_Data.AsUInt   = *((const unsigned int*)pData);break;
        case M_SHORT:	m_Data.AsShort  = *((const short*)pData);break;
        case M_USHORT:	m_Data.AsUShort = *((const unsigned short*)pData);break;
        case M_LONG:	m_Data.AsLong   = *((const long*)pData);break;
        case M_ULONG:	m_Data.AsULong  = *((const unsigned long*)pData);break;
        case M_FLOAT:  	m_Data.AsFloat  = *((const float*)pData);break;
        case M_DOUBLE:	m_Data.AsDouble = *((const double*)pData);break;
        case M_STRING:	m_Data.AsString = new char[ m_iSize ];
                        memcpy( m_Data.AsString, pData, m_iSize );
                        break;
        case M_COMPRESSEDDATA:
        case M_ROWDATA:	m_Data.AsRowdata = new unsigned char[ m_iSize ];
                        memcpy( m_Data.AsRowdata, pData, m_iSize );
                        break;
        default:       	break;
     }
}
//------------------------------------------------------------------------------
BOOL MNode::SetInt( int NewValue )
{    
    if( m_iType == M_INT )
    {
        m_Data.AsInt = NewValue;
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL  MNode::SetString( const MString& refStr )
{
    if( m_iType == M_STRING )
    {
        UFCType::Int32 NewSize = refStr.Length() + 1;
        
        if( NewSize > m_iSize )///< Size grow, re-alloc memory.        
        {
            delete [] m_Data.AsString;             
            m_Data.AsString = new char[ m_iSize ];            
        }
        m_iSize = NewSize;
        memcpy( m_Data.AsString, refStr.c_str(), m_iSize );
        return TRUE;        
    }
    return FALSE;    
}
//------------------------------------------------------------------------------
