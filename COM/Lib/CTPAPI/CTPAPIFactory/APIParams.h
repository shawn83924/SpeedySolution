//---------------------------------------------------------------------------
#ifndef APIParamsH
#define APIParamsH
//-------------------------------------------------------------------------------------------------------------------------
#define WM_CTP_EVENT    (WM_USER + 100 )
#define WM_FEMAS_EVENT  (WM_USER + 200 )
#define WM_KSFT_EVENT   (WM_USER + 300 )
#define WM_HSUFX_EVENT  (WM_USER + 400 )
#define WM_KSTRD_EVENT  (WM_USER + 500 ) ///< 500~600  CThostFtdcTraderSpi:[+1~120]  CKSOptionSpi:[+121~160]  CKSVocSpi[+161+200]
//-------------------------------------------------------------------------------------------------------------------------
typedef union
{
	int       AsInt;
	bool      AsBool;
	char*     AsRowdata;

} UnionType;
//-------------------------------------------------------------------------------------------------------------------------
typedef enum
{
	cdtNotUse   =  0,
	cdtBool     =  1,
	cdtInt	    =  2,
	cdtRowData  =  3

} ParamDataType;
//-------------------------------------------------------------------------------------------------------------------------
class TCTPParam
{
private:
	UnionType      FData;
	ParamDataType  FType;
public:
	TCTPParam( );
	TCTPParam( int Val );
	TCTPParam( bool Val );
	TCTPParam( void* Ptr, int Size );
	~TCTPParam();
	int   AsInt( void ) { return FData.AsInt; }
	bool  AsBool( void ){ return FData.AsBool; }
	void* AsPtr( void ) { return FData.AsRowdata; }
};
//-------------------------------------------------------------------------------------------------------------------------
class TCTPParams
{
public:
	TCTPParam Param1;
	TCTPParam Param2;
	TCTPParam Param3;
	TCTPParam Param4;
public:
	TCTPParams( int Val );  ///< int
	TCTPParams( bool Val ); ///< bool
	TCTPParams( int Val, int Val1 );    ///< int, int
	TCTPParams( void* Ptr1, int Size1); ///<
	TCTPParams( void* Ptr1, int Size1, int Val1, bool Val2 );
	TCTPParams( void* Ptr1, int Size1, void* Ptr2, int Size2 );
	TCTPParams( void* Ptr1, int Size1, void* Ptr2, int Size2, int Val1, bool Val2 );
};
//-------------------------------------------------------------------------------------------------------------------------
#endif
