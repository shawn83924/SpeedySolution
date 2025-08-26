//---------------------------------------------------------------------------

#ifndef TParseDataH
#define TParseDataH
//---------------------------------------------------------------------------
#include "TDataFormat.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class TParseData
{
private:
	PStream*		FRefStream;
	TDataFormat*	FRefDataFormat;
	TDataset*		FDataset;

public:
	TParseData();
	TParseData(PStream* SourceStream, TDataFormat* DataFormat);
	virtual ~TParseData();

public:
	bool			SetDataFormat(TDataFormat*	Value);
	TRecord*		Parse( const char* Source, int Size );
	TDataset*		Parse( PStream* pStream );
	TDataFormat*	GetDataFormat() { return FRefDataFormat; }
};
//---------------------------------------------------------------------------
}
//--------------------------------------------------------------------------------------------
#endif
