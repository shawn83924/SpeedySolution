#include "Schema.h"
namespace nsDataStore
{
//---------------------------------------------------------------------------
TSchema::TSchema( const UFC::AnsiString& Name )
:FRecordSize( 1 ) //dirty bits
{
	try
	{
		FFields.SetOwnObjects( TRUE );

		UFC::AnsiString FileName;
		FileName.Printf( "%s.schema", Name.c_str() );
        if( UFC::FileExists( FileName ) == false  )
            FileName.Printf( "../cfg/%s.schema", Name.c_str() );            
		Initial( FileName );
	}
	catch( DataStoreException& ex )
	{
                InitialFailException ifex;
                ifex.Throw( ex.what() );
	}
	catch( UFC::Exception& ex )
	{
                InitialFailException ifex;
                ifex.Throw( "TSchema::TSchema\n %s\n", ex.what() );
	}
	catch( ... )
	{
                InitialFailException ifex;
                ifex.Throw( "TSchema::TSchema\n unknown error\n" );
	}
}
//---------------------------------------------------------------------------
TSchema::~TSchema( void )
{
	FFields.Clear();
}
//---------------------------------------------------------------------------
void TSchema::AddFieldProperties( const UFC::AnsiString& Name, enumFieldType Type, int MaxLength, bool Unique, bool Index )
{
	if( Unique && Index )
	{
		InitialFailException ifex;
                ifex.Throw( "TSchema::AddFieldProperties\n unique and index are incompatible\n" );
	}

	for( int i = 0; i < FFields.ItemCount(); i++ )
	{
		if( Name == FFields.GetItem(i)->FName )
		{
        		InitialFailException ifex;
                        ifex.Throw( "TSchema::AddFieldProperties\n duplicate field name\n" );
		}
	}

	switch( Type )
	{
	case ftString:
		MaxLength++;
		break;
	case ftInt:
		MaxLength = sizeof(int);
		break;
	case ftDouble:
		MaxLength = sizeof(double);
		break;
	}

	TFieldProperties* NewProp = new TFieldProperties();
	NewProp->FName      = Name;
	NewProp->FType      = Type;
	NewProp->FMaxLength = MaxLength;
	NewProp->FUnique    = Unique;
	NewProp->FIndex     = Index;

	FFields.Add( NewProp );
	FRecordSize += MaxLength;
}
//---------------------------------------------------------------------------
void TSchema::Initial( const UFC::AnsiString& FileName )
{
	UFC::UiniFile iniFile( FileName );
	UFC::AnsiString FieldName;
	UFC::Section* pSection;

	FFields.Clear();

	for( int i = 0; i < iniFile.SectionCount(); i++ )
	{
		pSection  = iniFile.GetSection(i);
		FieldName = pSection->GetSectionName();

		if( FieldName.Length() == 0 )
                {
                    InitialFailException ifex;
                    ifex.Throw( "TSchema::Initial\n field name = null string\n" );
                }

		AddFieldProperties( pSection->GetSectionName(), ReadType( pSection ), ReadMaxLength( pSection ), ReadUnique( pSection ), ReadIndex( pSection ) );
	}
}
//---------------------------------------------------------------------------
enumFieldType TSchema::ReadType( UFC::Section* pSection )
{
	UFC::AnsiString Msg, Type;

	if( !pSection->GetValue( "Type", Type ) )
        {
            InitialFailException ifex;
            ifex.Throw( "TSchema::ReadType\n there is no type in field %s\n", pSection->GetSectionName().c_str() );
        }

	Type.UpperThis();

	enumFieldType Result = ftString;

	if( Type == "INT" )
		Result = ftInt;
	else if( Type == "DOUBLE" )
		Result = ftDouble;
	else if( Type == "STRING" )
		Result = ftString;
	else
        {
            InitialFailException ifex;
            ifex.Throw( "TSchema::ReadType\n nknown type in field %s\n", pSection->GetSectionName().c_str() );
        }

	return Result;		
}
//---------------------------------------------------------------------------
bool TSchema::ReadUnique( UFC::Section* pSection )
{
	UFC::AnsiString Unique;

	if( !pSection->GetValue( "Unique", Unique ) )
		return false;

	Unique.UpperThis();
	if( Unique == "TRUE" )
		return true;
	if( Unique == "YES" )
		return true;
	if( Unique == "1" )
		return true;

	return false;
}
//---------------------------------------------------------------------------
bool TSchema::ReadIndex( UFC::Section* pSection )
{
	UFC::AnsiString Index;

	if( !pSection->GetValue( "Index", Index ) )
		return false;

	Index.UpperThis();
	if( Index == "TRUE" )
		return true;
	if( Index == "YES" )
		return true;
	if( Index == "1" )
		return true;

	return false;
}
//---------------------------------------------------------------------------
int TSchema::ReadMaxLength( UFC::Section* pSection )
{
	UFC::AnsiString MaxLength;

	if( !pSection->GetValue( "MaxLength", MaxLength ) )
		return 0;

	return MaxLength.ToInt();
}
//---------------------------------------------------------------------------
}

