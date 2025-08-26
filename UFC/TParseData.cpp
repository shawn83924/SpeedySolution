#include "TParseData.h"
#include "BufferedLog.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
TParseData::TParseData()
:FRefStream(NULL)
,FRefDataFormat(NULL)
,FDataset(NULL)
{
}
//---------------------------------------------------------------------------
TParseData::TParseData( PStream* SourceStream, TDataFormat* DataFormat)
:FRefStream(SourceStream)
,FRefDataFormat(NULL)
,FDataset(NULL)
{
    SetDataFormat( DataFormat );
}
//---------------------------------------------------------------------------
TParseData::~TParseData()
{
    if( FDataset != NULL )
    	delete FDataset;
}
//---------------------------------------------------------------------------
bool TParseData::SetDataFormat(TDataFormat*	Value)
{
    if( Value != NULL )
    {
	if( FDataset != NULL )
            delete FDataset;
	FRefDataFormat = Value;
	FDataset = FRefDataFormat->NewDataset();
    }
    if( FDataset != NULL )
        return true;
    return false;
}
//---------------------------------------------------------------------------
TRecord* TParseData::Parse( const char* Source, int Size )
{
    TRecord* pRecord = FDataset->NewRecord( FALSE );

    if( pRecord != NULL )
        FRefDataFormat->Parse( Source, Size, pRecord );

    return pRecord;
}
//---------------------------------------------------------------------------
TDataset* TParseData::Parse( PStream* pStream )
{
    TRecord* pRecord;
    
    if( (FRefDataFormat != NULL) && (FDataset != NULL) )
    {
        FDataset->DeleteAllRecords();
        if( pStream != NULL )
        {
            PtrList<AnsiString> StringList;
    	    StringList.SetOwnObjects( TRUE );
            FRefDataFormat->LoadStreamToStringList( pStream, &StringList );

            int ListCount = StringList.ItemCount();
            AnsiString* str;

            for( register int i = 0; i < ListCount; i++ )
            {
                str = StringList.GetItem(i);
                
                if( (pRecord= FDataset->NewRecord( TRUE )) != NULL )
                    FRefDataFormat->Parse( str->c_str(), str->Length(), pRecord );
            }
            StringList.Clear();
        }
        return FDataset;
    }
    return NULL;
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

