//---------------------------------------------------------------------------

#ifndef TRenderDataH
#define TRenderDataH
//---------------------------------------------------------------------------
#include "TDataFormat.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class TRenderData
{
private:
	TDataFormat*	FRefDataFormat;
	TDataset*		FDataset;
	TRecord*		FRecord;

public:
	TRenderData();
	virtual ~TRenderData();

public:
public:
	bool            SetDataFormat(TDataFormat* RefDataFormat);
	AnsiString	Render();
	void            Reset();

	void		    SetField( Int32 FieldIndex, const AnsiString& Value );
	void		    SetField( const AnsiString& FieldName, const AnsiString& Value );
	void		    SetField( const AnsiString& FieldName, Int32 Value );
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
