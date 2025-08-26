//------------------------------------------------------------------------------
#ifndef STL_CONFIG_H
#define STL_CONFIG_H

//------------------------------------------------------------------------------
#include "Exception.h"
#include "AnsiString.h"
#include <utility>
#include <map>
#include <vector>
//------------------------------------------------------------------------------
// G++ 4.8.1 is the first version implementation C++11 standard
//------------------------------------------------------------------------------
#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

#if GCC_VERSION > 40800
    #ifdef __LINUX        
        #ifndef __IBMZ
            #define GCC_CPP11_SUPPORT
        #endif  
    #endif
#endif

//------------------------------------------------------------------------------
// Add a hash structure for UFC::AnsiString
// Let STL can generate hash code for UFC::AnsiString
//------------------------------------------------------------------------------
#ifdef __GNUC__				         ///< For GCC compiler
	#if __GNUC__ < 3                         ///< GCC 2.XX
            #include <hash_map.h>
            #include <hash_set.h>
            namespace Sgi { using ::hash_map; }; ///< inherit globals
            namespace std
            {
                 template <> struct hash< UFC::AnsiString >
                 {
                     size_t operator()(const UFC::AnsiString& s) const
                     {
                        return hash<char const*>()(s.c_str());
                     }
                 };
                 template <> struct hash< double >
                 {
                     size_t operator()(const double& d) const
                     {
                        return hash<double>()(d);
                     }
                 };
            }
	#else    		
            #if __GNUC__ > 3                       ///< GCC 4.XX
                #ifndef __GXX_EXPERIMENTAL_CXX0X__    
                    #include <ext/hash_map>
                    #include <ext/hash_set>

                    namespace Sgi = ::__gnu_cxx;    ///< GCC 3.1 and later
                    namespace __gnu_cxx
                    {
                        template <> struct hash< UFC::AnsiString >
                        {
                            size_t operator()(const UFC::AnsiString& s) const
                            {
                                return hash<char const*>()(s.c_str());
                            }
                        };
                        template <> struct hash< double >
                        {
                            size_t operator()(const double& d) const
                            {
                                return hash<double>()(d);
                            }
                        };
                    }
                #else                               ///< GCC 4.3X above, for C++ 0x Standards.
                    #include <unordered_set>
                    #include <unordered_map>
                    #define hash_map unordered_map
                    #define hash_set unordered_set

                    namespace Sgi = std;            
                    namespace std
                    {
                        template <> struct hash< UFC::AnsiString >
                        {
                            size_t operator()(const UFC::AnsiString& s) const
                            {
                                char *p = (char *)s.c_str();
                                size_t hashval = 5381;
                                char *e = p + (s.Length());

                                while(p != e)
                                     hashval = ((hashval << 5) + hashval) + (*p++);
                                return hashval;
                            }
                        };
                    }
                #endif
            #else                               ///< GCC 3.XX
                #include <ext/hash_map>
                #include <ext/hash_set>
                #if __GNUC_MINOR__ == 0
                    namespace Sgi = std;            ///< GCC 3.0
                    namespace std
                    {
                       template <> struct hash< UFC::AnsiString >
                       {
                           size_t operator()(const UFC::AnsiString& s) const
                           {
                               return hash<char const*>()(s.c_str());
                           }
                       };
                       template <> struct hash< double >
                       {
                           size_t operator()(const double& d) const
                           {
                               return hash<double>()(d);
                           }
                       };
                    }
                #else
                    namespace Sgi = ::__gnu_cxx;    ///< GCC 3.1 and later
                    namespace __gnu_cxx
                    {
                        template <> struct hash< UFC::AnsiString >
                        {
                            size_t operator()(const UFC::AnsiString& s) const
                            {
                                return hash<char const*>()(s.c_str());
                            }
                        };
                        template <> struct hash< double >
                        {
                            size_t operator()(const double& d) const
                            {
                                return hash<double>()(d);
                            }
                        };
                    }
                #endif
            #endif ///< End gcc version > 3.XX
        #endif

#else      ///< End of GCC ...there are other compilers, right? (Visual C++, Borland C++)

	namespace Sgi = std;
	#if (__BORLANDC__ >= 0x0580) ///< For BDS 2006 later, Use Dinkum C++ Library
		#include <hash_map>
		#include <hash_set>
		namespace std
		{
			template<> struct less< UFC::AnsiString >
			{
				bool operator()( const UFC::AnsiString& left, const UFC::AnsiString& right) const
				{
					return ( left.AnsiCompare( right ) < 0 );
				}
			};
			template<> struct less< double >
			{
				bool operator()( const double& left, const double& right) const
				{
					return ( left < right  );
				}
			};
		}//end namespace*/
	#elif (_MSC_VER >= 1600 )  ///< For >= Visual C++ 2010, for C++ 0x Standards.
		//---------------------------------------------------------
		#include <unordered_set>
		#include <unordered_map>
		#define hash_map unordered_map
		#define hash_set unordered_set
		namespace std
		{
			template <> struct hash< UFC::AnsiString >
			{
				inline size_t operator()(const UFC::AnsiString& s) const
				{
					return (stdext::hash_value(s.c_str())) ;
				}
			};
		} //end std*/
	#else ///< For STLPort
		//--------------------------------------------------------------
		#include <hash_map>
		#include <hash_set>
		namespace std
		{
			template <> struct hash< UFC::AnsiString >
			{
				inline size_t operator()( const UFC::AnsiString& s) const
				{
					char *p = (char *)s.c_str();
					size_t hashval = 5381;
					char *e = p + (s.Length());

					while(p != e)
							hashval = ((hashval << 5) + hashval) + (*p++);
					return hashval;
				}
			};
			template <> struct hash< double >
			{
				size_t operator()( const double& d) const
				{
					return std::hash<double>()(d);
				}
			};
		 }//end namespace std
	#endif
#endif
//------------------------------------------------------------------------------

#endif
//------------------------------------------------------------------------------
