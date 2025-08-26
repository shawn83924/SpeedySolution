#include "Record.h"
namespace nsDataStore
{
//---------------------------------------------------------------------------
TRecordInstance::TRecordInstance( int RecordID, TSchema* pSchema, UInt8* pData, int FileOffset )
:FID( RecordID )
,FRef( 0 )
,FFileOffset( FileOffset )
,fSchema( pSchema )
,FData( pData )
{
	try
	{
		Initial();
	}
	catch( UFC::Exception& ex )
	{
            InitialFailException ifex;
            ifex.Throw( "TRecord::TRecord\n %s\n", ex.what() );
	}
	catch( ... )
	{
            InitialFailException ifex;
            ifex.Throw( "TRecord::TRecord\n unknown error\n" );
	}
}
//---------------------------------------------------------------------------
TRecordInstance::~TRecordInstance( void )
{
	for( int i = FFieldHashList.ItemCount() - 1; i >= 0; i-- )
	{
		delete FFieldHashList.GetItem(i);
	}

	if( FData != NULL )
		delete [] FData;
}
//---------------------------------------------------------------------------
void TRecordInstance::Initial( void )
{
	if( fSchema == NULL )
        {
            InitialFailException ifex;
            ifex.Throw( "TRecord::Initial\n fSchema = null\n" );
        }

	int FieldCount = fSchema->FFields.ItemCount();
	int RecSize    = fSchema->FRecordSize;

	if( FData == NULL )
	{
		FData = new UInt8[ RecSize ];
		memset( FData, 0x00, RecSize );
		(*FData) |= REC_STATE_NEW;
	}

	int               Offset      = 1;
	TField*           pField      = NULL;
	TFieldProperties* pProperties = NULL;

	for( int i = 0; i < FieldCount; i++ )
	{
		pProperties = fSchema->FFields.GetItem(i);
		pField      = new TField( pProperties, FData + Offset );
		FFieldHashList.Add( pProperties->FName, pField );
		
		Offset += pProperties->FMaxLength;
	}
}
//---------------------------------------------------------------------------
TField* TRecordInstance::GetField( const UFC::AnsiString& FieldName )
{
    TField* pField = FFieldHashList.GetObjectByKey( FieldName );

    if( pField == NULL )
    {
        FieldNotFoundException fnfex;
        fnfex.Throw( "TRecordInstance::GetField[%s] not found\n", FieldName.c_str() );
    }

    return pField;
}
//---------------------------------------------------------------------------
UInt8 TRecordInstance::GetState( void )
{
	return (*FData);
}
//---------------------------------------------------------------------------
void TRecordInstance::SetState( UInt8 Value )
{
	(*FData) = Value;
}
//---------------------------------------------------------------------------
int TRecordInstance::IncreaseRef( void )
{
    UFC::PLockObject Lock( FCS );
    return ++FRef;
}
//---------------------------------------------------------------------------
int TRecordInstance::DecreaseRef( void )
{
    UFC::PLockObject Lock( FCS );
    
    int Ref = --FRef;
    return Ref;
}
//---------------------------------------------------------------------------
void TRecordInstance::SetFileOffset( Int32 Offset )
{
    UFC::PLockObject Lock( FCS );
    
    FFileOffset = Offset;
}
//---------------------------------------------------------------------------
/*

	IsUnique----REC_STATE_NEW----return true-----END1
	|           |
   (N)         (N)
	|           |
	|           return false---------------------END2
	|
	|
	|
	IsIndex-----REC_STATE_NEW----return true-----END3
	|           |
   (N)         (N)
	|           |
	|           IsNullString-----return true-----END4
	|           |
	|          (N)
	|           |
	|           return false---------------------END5
	|
	|
	return true----------------------------------END6

*/
//---------------------------------------------------------------------------
bool TRecordInstance::CanModifyField( TField* pField )
{
	if( pField->IsUnique() )
	{
		if( ( GetState() & REC_STATE_NEW ) != 0 )
			return true;//END1
		else
        	return false;//END2
	}
	else if( pField->IsIndex() )
	{
		if( ( GetState() & REC_STATE_NEW ) != 0 )
		{
			return true;//END3
		}
		else
		{
			if( pField->ToString().Length() == 0 )
				return true;//END4
			else
				return false;//END5
		}
	}

	return true;//END6
}
//---------------------------------------------------------------------------
//
// TRecord
//
//---------------------------------------------------------------------------
TRecord::TRecord( void )
:FInstance( NULL )
{
}
//---------------------------------------------------------------------------
TRecord::TRecord( TRecord* pRecord )
:FInstance( NULL )
{
	if( pRecord != NULL )
		FInstance = pRecord->FInstance;
	IncreaseReferenceCount();
}
//---------------------------------------------------------------------------
TRecord::TRecord( TRecordInstance* pInstance )
:FInstance( pInstance )
{
	IncreaseReferenceCount();
}
//---------------------------------------------------------------------------
TRecord::TRecord( const TRecord& Record )
:FInstance( Record.FInstance )
{
	IncreaseReferenceCount();
}
//---------------------------------------------------------------------------
TRecord::~TRecord( void )
{
	DecreaseReferenceCount();
}
//---------------------------------------------------------------------------
TRecord& TRecord::operator = ( const TRecord& Record )
{
	if( this != &Record )
	{
		DecreaseReferenceCount();
		FInstance = Record.FInstance;
		IncreaseReferenceCount();
	}
	return *this;
}
//---------------------------------------------------------------------------
void TRecord::IncreaseReferenceCount( void )
{
	if( FInstance != NULL )
	{
		FInstance->IncreaseRef();
	}
}
//---------------------------------------------------------------------------
void TRecord::DecreaseReferenceCount( void )
{
	if( FInstance != NULL )
	{
		if( FInstance->DecreaseRef() <= 0 )
        {
            delete FInstance;
            FInstance = NULL;
        }
	}
}
//---------------------------------------------------------------------------
UFC::AnsiString TRecord::GetFieldAsString( const UFC::AnsiString& FieldName ) const
{
	if( FInstance != NULL )
	{
		TField* pField = FInstance->GetField( FieldName );
		return pField->ToString();
	}
	return "";
}
//---------------------------------------------------------------------------
int TRecord::GetFieldAsInt( const UFC::AnsiString& FieldName ) const
{
	if( FInstance != NULL )
	{
		TField* pField = FInstance->GetField( FieldName );
		return pField->ToInt();
	}
	return 0;
}
//---------------------------------------------------------------------------
double TRecord::GetFieldAsDouble( const UFC::AnsiString& FieldName ) const
{
	if( FInstance != NULL )
	{
		TField* pField = FInstance->GetField( FieldName );
		return pField->ToDouble();
	}
	return 0.0;
}
//---------------------------------------------------------------------------
void TRecord::SetField( const UFC::AnsiString& FieldName, const UFC::AnsiString& FieldValue )
{
	if( FInstance != NULL )
	{
		TField* pField = FInstance->GetField( FieldName );
		if( FInstance->CanModifyField( pField ) )
		{
			pField->FromString( FieldValue );
			FInstance->SetState( FInstance->GetState() | REC_STATE_MODIFY );
		}
	}
}
//---------------------------------------------------------------------------
void TRecord::SetField( const UFC::AnsiString& FieldName, const int FieldValue )
{
	if( FInstance != NULL )
	{
		TField* pField = FInstance->GetField( FieldName );

		if( FInstance->CanModifyField( pField ) )
		{
			pField->FromInt( FieldValue );
			FInstance->SetState( FInstance->GetState() | REC_STATE_MODIFY );
		}
	}
}
//---------------------------------------------------------------------------
void TRecord::SetField( const UFC::AnsiString& FieldName, const double FieldValue )
{
	if( FInstance != NULL )
	{
		TField* pField = FInstance->GetField( FieldName );

		if( FInstance->CanModifyField( pField ) )
		{
			pField->FromDouble( FieldValue );
			FInstance->SetState( FInstance->GetState() | REC_STATE_MODIFY );
		}
	}
}
//---------------------------------------------------------------------------
void TRecord::Delete( void )
{
//	UInt8 State = FInstance->GetState();
	//FInstance->SetState( State | REC_STATE_DELETE | REC_STATE_MODIFY );
        FInstance->SetState( REC_STATE_DELETE | REC_STATE_MODIFY );
}
//---------------------------------------------------------------------------
bool TRecord::WriteFile( UFC::FileStreamEx* fs ) const
{
    if( FInstance != NULL )
    {
        if( FInstance->FFileOffset < 0 )
            FInstance->FFileOffset = fs->Seek( 0, SEEK_END );
        else
            fs->Seek( FInstance->FFileOffset, SEEK_SET );

        FInstance->SetState( FInstance->GetState() & REC_STATE_DELETE );

        if( fs->Write( FInstance->FData, FInstance->fSchema->FRecordSize ) == FInstance->fSchema->FRecordSize )
        {
            fs->Flush();
            return true;
        }
        else
            FInstance->FFileOffset = -1;
    }
    else
        UFC::BufferedLog::Printf(" TRecord::WriteFile() Record Instance is NULL.");

    return false;
}
//---------------------------------------------------------------------------
int TRecord::WriteFile( UFC::FileStreamEx* fs, UFC::AnsiString& ErrorMsg ) const
{
    int errorCode = 0;
    if( FInstance != NULL )
    {
        if( FInstance->FFileOffset < 0 )
            FInstance->FFileOffset = fs->Seek( 0, SEEK_END );
        else
            fs->Seek( FInstance->FFileOffset, SEEK_SET );

        FInstance->SetState( FInstance->GetState() & REC_STATE_DELETE );

        int recordSize = FInstance->fSchema->FRecordSize;
        int writeSize = fs->Write( FInstance->FData, recordSize );
        if( writeSize == recordSize )
            fs->Flush();
        else
        {
            FInstance->FFileOffset = -1;
            errorCode = -2;
            ErrorMsg.Printf("TRecord::WriteFile() WriteSize[%d] != ReocrdSize[%d].", writeSize, recordSize);
        }
    }
    else
    {
        errorCode = -1;
        ErrorMsg = "TRecord::WriteFile() Record Instance is Null.";
    }

    return errorCode;
}
//---------------------------------------------------------------------------
bool TRecord::CheckState( UInt8 Mask ) const
{
	if( FInstance != NULL )
	{
		return ( (FInstance->GetState() & Mask) != 0 );
	}
	return false;
}
//---------------------------------------------------------------------------
int TRecord::GetRecordID( void ) const
{
	if( FInstance != NULL )
	{
		return FInstance->FID;
	}
	return 0;
}
//---------------------------------------------------------------------------
}

