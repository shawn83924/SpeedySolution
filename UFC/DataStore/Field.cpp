#include "Field.h"
//---------------------------------------------------------------------------
namespace nsDataStore
{
//---------------------------------------------------------------------------
TField::TField( TFieldProperties* pProperties, UInt8* Data )
:fProperties( pProperties )
,fData( Data )
{
}
//---------------------------------------------------------------------------
TField::~TField( void )
{
}
//---------------------------------------------------------------------------
const UFC::AnsiString TField::ToString( void )
{
	UFC::AnsiString Result( "" );

	if( (fData != NULL) && (fProperties != NULL) )
	{
		if( fProperties->FType == ftString )
		{
			Result = (char*)(fData);
		}
		else if( fProperties->FType == ftInt )
		{
			int IntVal = 0;
			memcpy( &IntVal, fData, sizeof(int) );
			Result.Printf( "%d", IntVal );
		}
		else if( fProperties->FType == ftDouble )
		{
			double DblVal = 0.0;
			memcpy( &DblVal, fData, sizeof(double) );
			Result.Printf( "%lf", DblVal );
		}
	}

	return Result;
}
//---------------------------------------------------------------------------
const int TField::ToInt( void )
{
	int Result = 0;

	if( (fData != NULL) && (fProperties != NULL) )
	{
		if( fProperties->FType == ftString )
		{
			Result = UFC::AnsiString( (const char*)(fData) ).ToInt();
		}
		else if( fProperties->FType == ftInt )
		{
			memcpy( &Result, fData, sizeof(int) );
		}
		else if( fProperties->FType == ftDouble )
		{
			double DblVal = 0.0;
			memcpy( &DblVal, fData, sizeof(double) );
			Result = (int)DblVal;
		}
	}

	return Result;
}
//---------------------------------------------------------------------------
const double TField::ToDouble( void )
{
    double Result = 0.0;

	if( (fData != NULL) && (fProperties != NULL) )
	{
		if( fProperties->FType == ftString )
		{
			Result = UFC::AnsiString( (char*)(fData) ).ToDouble();
		}
		else if( fProperties->FType == ftInt )
		{
			int IntVal = 0;
			memcpy( &IntVal, fData, sizeof(int) );
			Result = (double)IntVal;
		}
		else if( fProperties->FType == ftDouble )
		{
			memcpy( &Result, fData, sizeof(double) );
		}
	}

	return Result;
}
//---------------------------------------------------------------------------
void TField::FromString( const UFC::AnsiString& Data )
{
	if( (fData != NULL) && (fProperties != NULL) )
	{
		memset( fData, 0x00, fProperties->FMaxLength );

		if( fProperties->FType == ftString )
		{
			memcpy( fData, Data.c_str(), (Data.Length() < fProperties->FMaxLength) ? Data.Length() : (fProperties->FMaxLength - 1) );
		}
		else if( fProperties->FType == ftInt )
		{
			int IntVal = Data.ToInt();
			memcpy( fData, &IntVal, sizeof(int) );
		}
		else if( fProperties->FType == ftDouble )
		{
			double DblVal = Data.ToDouble();
			memcpy( fData, &DblVal, sizeof(double) );
		}
	}
}
//---------------------------------------------------------------------------
void TField::FromInt( const int Data )
{
	if( (fData != NULL) && (fProperties != NULL) )
	{
		memset( fData, 0x00, fProperties->FMaxLength );

		if( fProperties->FType == ftString )
		{
			char TmpBuf[16];
			sprintf( TmpBuf, "%d", Data );
			int StrLen = strlen( TmpBuf );
			
			if( StrLen < fProperties->FMaxLength )
			{
				memcpy( fData, TmpBuf, StrLen );
			}
			else
			{
				StrLen = (StrLen - fProperties->FMaxLength + 1);
				memcpy( fData, TmpBuf + StrLen, StrLen );
            }
		}
		else if( fProperties->FType == ftInt )
		{
			memcpy( fData, &Data, sizeof(int) );
		}
		else if( fProperties->FType == ftDouble )
		{
			double DblVal = (double)Data;
			memcpy( fData, &DblVal, sizeof(double) );
		}
	}
}
//---------------------------------------------------------------------------
void TField::FromDouble( const double Data )
{
	if( (fData != NULL) && (fProperties != NULL) )
	{
		memset( fData, 0x00, fProperties->FMaxLength );

		if( fProperties->FType == ftString )
		{
			char TmpBuf[16];
			sprintf( TmpBuf, "%lf", Data );
			int StrLen = strlen( TmpBuf );
			
			if( StrLen < fProperties->FMaxLength )
			{
				memcpy( fData, TmpBuf, StrLen );
			}
			else
			{
				StrLen = (StrLen - fProperties->FMaxLength + 1);
				memcpy( fData, TmpBuf + StrLen, StrLen );
            }
		}
		else if( fProperties->FType == ftInt )
		{
			int IntVal = (int)Data;
			memcpy( fData, &IntVal, sizeof(int) );
		}
		else if( fProperties->FType == ftDouble )
		{
			memcpy( fData, &Data, sizeof(double) );
		}
	}
}
//---------------------------------------------------------------------------
bool TField::IsUnique( void )
{
	return fProperties->FUnique;
}
//---------------------------------------------------------------------------
bool TField::IsIndex( void )
{
	return fProperties->FIndex;
}
//---------------------------------------------------------------------------
}

