#include "TRenderData.h"
#include "NetUtility.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
TRenderData::TRenderData()
:FRefDataFormat(NULL)
,FDataset(NULL)
,FRecord(NULL)
{
}
//---------------------------------------------------------------------------
TRenderData::~TRenderData()
{
	if( FDataset != NULL )
		delete FDataset;
}
//---------------------------------------------------------------------------
bool TRenderData::SetDataFormat( TDataFormat* RefDataFormat )
{
	if( RefDataFormat != NULL )
	{
		if( FDataset != NULL )
			delete FDataset;

		FRefDataFormat = RefDataFormat;
		FDataset = FRefDataFormat->NewDataset();
		FRecord = FDataset->NewRecord();
	}

	if( FDataset != NULL )
		return true;

	return false;
}
//---------------------------------------------------------------------------
AnsiString TRenderData::Render()
{
	if( FRefDataFormat != NULL && FRecord != NULL )
		return FRefDataFormat->Render( FRecord );

	return "";
}
//---------------------------------------------------------------------------
void TRenderData::Reset()
{    
	if( FRecord != NULL )
		FRecord->Reset();
}
//---------------------------------------------------------------------------
void TRenderData::SetField(Int32 FieldIndex, const AnsiString& Value)
{
	if( FRecord != NULL )
		FRecord->SetField( FieldIndex, Value );
}
//---------------------------------------------------------------------------
void TRenderData::SetField(const AnsiString& FieldName, const AnsiString& Value)
{
	if( FRecord != NULL )
		FRecord->SetField( FieldName, Value );
}
//---------------------------------------------------------------------------
void TRenderData::SetField(const AnsiString& FieldName, Int32 Value)
{
    SetField( FieldName, AnsiString(Value) );
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

