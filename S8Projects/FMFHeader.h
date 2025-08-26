
//---------------------------------------------------------------------------
#ifndef FMFHeaderH
#define FMFHeaderH
//---------------------------------------------------------------------------
typedef enum
{
	mdfDepth,
	mdfList,
	mdfKBar,
	mdfExec

}TMDFormType;
//---------------------------------------------------------------------------
class TClientForm
{
public:
	virtual TMDFormType __fastcall Type( void ) = 0;
	virtual void        __fastcall SetActive( bool ) = 0;
	virtual bool        __fastcall IsActive( void ) = 0;
	virtual TForm*      __fastcall GetTForm( void ) = 0;
	virtual int         __fastcall GetGroup( void ) = 0;
	virtual void        __fastcall Release( void ) = 0;
	virtual void        __fastcall SetVisible( bool Visible ) = 0;
};
//---------------------------------------------------------------------------
class UnifyProductKey
{
public:
	String project_id;
	String schedule_id;
public:
	UnifyProductKey( const String& pid, const String& sid )
	:project_id(pid),schedule_id(sid){}
};
//---------------------------------------------------------------------------
class  UnifyProductInfo
{
public:
	String ID;
	String ExpiredDate;
	int    YYYYMMDD;
public:
	UnifyProductInfo( const String& id, const String& date )
	:ID( id )
	,ExpiredDate( date )
	{
		String YYYY( ExpiredDate.c_str(), 4 );
		String MM( ExpiredDate.c_str() + 5, 2 );
		String DD( ExpiredDate.c_str() + 8, 2 );
		YYYYMMDD = YYYY.ToInt()*10000+ MM.ToInt()*100 + DD.ToInt();
	}
};
//---------------------------------------------------------------------------
extern UnifyProductKey UnifyKey; ///< Unify AP閃電下單版
extern UnifyProductKey AOEPackKey; ///< 啟蒙包
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
