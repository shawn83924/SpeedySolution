//------------------------------------------------------------------------------------------------------------------------
#ifndef __MDSCommObj_H
#define __MDSCommObj_H
//------------------------------------------------------------------------------------------------------------------------
#include "MDSType.h"
//------------------------------------------------------------------------------------------------------------------------
inline size_t HashBinary( const char* Buffer, const int Size )
{
	char *P = (char *)Buffer;
	size_t HashVal = 5381;
	char *E = P + Size;

	while( P != E )
		HashVal = ( (HashVal << 5) + HashVal ) + ( *P++ );
	return HashVal;
};
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
// class MDSBinaryObj
//------------------------------------------------------------------------------------------------------------------------
class MDSBinaryObj
{
private:
	static const Int32 MAX_STR_BUFFER = 12288;
	char*	FBuffer;
	/**
	 * the binary size, when STRING column
	 * 1. will no append '\0'
	 * 2. need padding '\0' if data is not rech Column length
	 * ex : STRING(5) --> this column will occupied 5 bytes
	 * ex : STRING(5) Column = "Allen" Index = "Allen"
	 * ex : STRING(7) Column = "Allen" Index = "Allen\0\0"
	 */
	Int32	FSize;
	Int32	FPos;
public:
	char*	GetBuffer() const { return FBuffer; }
	Int32	GetSize() const { return FSize; }
public:
	Int32	Compare( const MDSBinaryObj& rhs ) const throw();
public:
	void	PaddingZero( int Size );
	void	AppendBuffer( const char* Buf, Int32 Size );
	void	AppendAnsiString( const UFC::AnsiString& Str, int DataLen );
	void	AppendCString( const char* Str, int DataLen );
	void	AppendChar( const char Value );
	void	AppendInt( int Value );
	void	AppendInt64( Int64 Value );
	void	AppendDouble( double Value );
public:
	/**
	 * Printf(), will reset buffer size. Don't mixed use with AppendXXX... etc.
	 */
	void	Printf( const char* FormatStr, ... );
	Int32	Seek( Int32 Offset, Int32 Origin = UFC::soFromBeginning );
	/**
	 * ReAllocate buffer, when Size <= 0 then delete buffer and set FSize = 0
	 */
	void	ReAllocBuffer( Int32 Size );
public:
	BOOL			operator == (const MDSBinaryObj& rhs ) const { return Compare(rhs) == 0; }
	MDSBinaryObj&	operator = ( const MDSBinaryObj& rhs );
	operator size_t() const { return HashBinary( GetBuffer(), GetSize() ); }
public:
	MDSBinaryObj();
	MDSBinaryObj( Int32 Size );
	MDSBinaryObj( const char* Src, Int32 Size );
	MDSBinaryObj( const MDSBinaryObj& src );
	~MDSBinaryObj();
};
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
//
// STL Setting
//
//------------------------------------------------------------------------------------------------------------------------
#ifdef __GNUC__							     ///< For GCC compiler
	#if __GNUC__ < 3                         ///< GCC 2.XX
		#include <hash_map.h>
		#include <hash_set.h>
		namespace Sgi { using ::hash_map; }; ///< inherit globals
		namespace std
		{
			template <> struct hash< MDS::MDSBinaryObj >
			{
				size_t operator()( const MDS::MDSBinaryObj& BinaryObj ) const
				{
					return HashBinary( BinaryObj.GetBuffer(), BinaryObj.GetSize() );
				}
			};
		}
	#elif __GNUC__ > 3                       ///< GCC 4.XX
		
        #ifndef __GXX_EXPERIMENTAL_CXX0X__    
        	#include <ext/hash_map>
        	#include <ext/hash_set>
        	namespace Sgi = ::__gnu_cxx;         ///< GCC 3.1 and later
        	namespace __gnu_cxx
        	{
        		template <> struct hash< MDS::MDSBinaryObj >
        		{
					size_t operator()( const MDS::MDSBinaryObj& BinaryObj ) const
        			{
        				return HashBinary( BinaryObj.GetBuffer(), BinaryObj.GetSize() );
        			}
        		};
        	}
        #else                        ///< GCC 4.3X above, for  C++ 0x Standards.
            #include <unordered_set>
            #include <unordered_map>
            #define hash_map unordered_map
            #define hash_multimap unordered_multimap
            #define hash_set unordered_set
            namespace std
            {
                template <> struct hash< MDS::MDSBinaryObj >
                {
					size_t operator()(const MDS::MDSBinaryObj& BinaryObj ) const
                    {
        				return HashBinary( BinaryObj.GetBuffer(), BinaryObj.GetSize() );
                    }
                };
            }
		#endif
	#else                                    ///< GCC 3.XX
    	#include <ext/hash_map>
		#include <ext/hash_set>
		#if __GNUC_MINOR__ == 0
		namespace Sgi = std;            ///< GCC 3.0
		namespace std
		{
			template <> struct hash< MDS::MDSBinaryObj >
			{
				size_t operator()( const MDS::MDSBinaryObj& BinaryObj ) const
				{
					return HashBinary( BinaryObj.GetBuffer(), BinaryObj.GetSize() );
				}
			};
		}
		#else
		namespace Sgi = ::__gnu_cxx;    ///< GCC 3.1 and later
		namespace __gnu_cxx
		{
			template <> struct hash< MDS::MDSBinaryObj >
			{
				size_t operator()( const MDS::MDSBinaryObj& BinaryObj ) const
				{
					return HashBinary( BinaryObj.GetBuffer(), BinaryObj.GetSize() );
				}
			};
		}
		#endif
	#endif
#else      // ...there are other compilers, right?
	#include <hash_map>
	#include <hash_set>
	namespace Sgi = std;
	namespace std
	{
		#if (__BORLANDC__ >= 0x0580) ///< For BDS 2006 Dinkum C++ Library
			template<> struct less< MDS::MDSBinaryObj >
			{
				bool operator()( const MDS::MDSBinaryObj& left, const MDS::MDSBinaryObj& right ) const
				{
					return ( left.Compare( right ) < 0 );
				}
			};
		#else ///< now for STLPort
			template <> struct hash< MDS::MDSBinaryObj >
			{
				size_t operator()( const MDS::MDSBinaryObj& obj ) const
				{
					return HashBinary( obj.GetBuffer(), obj.GetSize() );
				}
			};
		#endif
	}
#endif
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
