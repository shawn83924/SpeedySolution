//---------------------------------------------------------------------------

#ifndef TDataFormatH
#define TDataFormatH
//---------------------------------------------------------------------------
#include "TDataset.h"
#include "Stream.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
typedef enum
{
	DFT_FIXED,
	DFT_DELIMITER

} TDataFormatType;
//---------------------------------------------------------------------------
class TFieldOffset
{
public:
	Int32 FStartAt;
	Int32 FSize;

public:
	TFieldOffset(Int32 StartAt, Int32 Size) : FStartAt(StartAt), FSize(Size) {}
};
//---------------------------------------------------------------------------
class TDataFormat
{
protected:
	TDataFormatType			    FFormatType;
	PtrList<TFieldOffset>	    FFieldOffset;
	TFieldSchema			    FFieldSchema;

public:
	TDataFormat(TDataFormatType FormatType);
	virtual ~TDataFormat() {}

public:
	virtual bool Parse( const char* inBUFF, int Size, TRecord* outREC ) = 0;
	virtual void LoadStreamToStringList( PStream* pStream, PtrList<AnsiString>* pStringList ) = 0;
    virtual AnsiString Render( TRecord* inREC ) = 0;

public:
	void		ClearField();
	void		AddField( const AnsiString& FieldName, Int32 StartAt, Int32 Size, TFieldDataType DataType = FDT_STRING, const AnsiString& DefaultValue = "" );
	TDataset*	NewDataset();
	Int32		GetFieldSize( Int32 FieldIndex );
	Int32		GetFieldCount() { return FFieldOffset.ItemCount(); }
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
