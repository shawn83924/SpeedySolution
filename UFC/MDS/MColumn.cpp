//------------------------------------------------------------------------------------------------------------------------
#include "MColumn.h"
//------------------------------------------------------------------------------------------------------------------------
namespace MDS
{
//------------------------------------------------------------------------------------------------------------------------
// class MColumn
//------------------------------------------------------------------------------------------------------------------------
MColumn::MColumn( MColumnDefine* pColumnDefine, UInt8* Data )
: FDefinition( pColumnDefine )
, FData( Data )
{
}
//------------------------------------------------------------------------------------------------------------------------
MColumn::~MColumn( void )
{
}
//------------------------------------------------------------------------------------------------------------------------
const UFC::AnsiString MColumn::ToString( void )
{
	UFC::AnsiString Result( "" );

	if ( (FData == NULL) || (FDefinition == NULL) )
		return Result;

	if ( FDefinition->FType == ctString ) ///< ctString
	{
		Result = (char*)(FData);
	}
	else if ( FDefinition->FType == ctChar ) ///< ctChar
	{
		Result.Printf( "%c", FData[ 0 ] );
	}
	else if ( FDefinition->FType == ctInt ) ///< ctInt
	{
		int intValue;
		memcpy( &intValue, FData, sizeof(int) );
		Result.Printf( "%d", intValue );
	}
	else if ( FDefinition->FType == ctInt64 ) ///< ctInt64
	{
		Int64 int64Value;
		memcpy( &int64Value, FData, sizeof(Int64) );
		Result.Printf( "%lld", int64Value );
	}
	else if ( FDefinition->FType == ctDouble ) ///< ctDouble
	{
		double	dblValue;
		char	strFormat[32];
		memcpy( &dblValue, FData, sizeof(double) );

		switch ( FDefinition->FShowPoint )
		{
			case 0 : Result.Printf( "%.0lf", dblValue ); break;
			case 1 : Result.Printf( "%.1lf", dblValue ); break;
			case 2 : Result.Printf( "%.2lf", dblValue ); break;
			case 3 : Result.Printf( "%.3lf", dblValue ); break;
			case 4 : Result.Printf( "%.4lf", dblValue ); break;
			default: sprintf( strFormat, "%%.%dlf", FDefinition->FShowPoint );
					 Result.Printf( strFormat, dblValue );
					 break;
		}
	}

	return Result;
}
//------------------------------------------------------------------------------------------------------------------------
const char MColumn::ToChar( void )
{
	char Result = ' ';

	if ( (FData == NULL) || (FDefinition == NULL) )
		return Result;

	if ( FDefinition->FType == ctChar ) ///< ctChar
	{
		Result = FData[ 0 ];
	}
	else if ( FDefinition->FType == ctString ) ///< ctString
	{
		Result = FData[ 0 ];
	}
	else if ( FDefinition->FType == ctInt ) ///< ctInt
	{
		int intValue;
		memcpy( &intValue, FData, sizeof(int) );
		Result = (char)intValue;
	}
	else if ( FDefinition->FType == ctInt64 ) ///< ctInt64
	{
		Int64 int64Value;
		memcpy( &int64Value, FData, sizeof(Int64) );
		Result = (char)int64Value;
	}
	else if ( FDefinition->FType == ctDouble ) ///< ctDouble
	{
		double dblValue;
		memcpy( &dblValue, FData, sizeof(double) );
		Result = (char)dblValue;
	}

	return Result;
}
//------------------------------------------------------------------------------------------------------------------------
const int MColumn::ToInt( void )
{
	int Result = 0;

	if ( (FData == NULL) || (FDefinition == NULL) )
		return Result;

	if ( FDefinition->FType == ctInt ) ///< ctInt
	{
		memcpy( &Result, FData, sizeof(int) );
	}
	else if ( FDefinition->FType == ctString ) ///< ctString
	{
		Result = UFC::AnsiString( (const char*)FData ).ToInt();
	}
	else if ( FDefinition->FType == ctChar ) ///< ctChar
	{
		Result = (int)FData[ 0 ];
	}
	else if ( FDefinition->FType == ctInt64 ) ///< ctInt64
	{
		Int64 int64Value;
		memcpy( &int64Value, FData, sizeof(Int64) );
		Result = (int)int64Value;
	}
	else if ( FDefinition->FType == ctDouble ) ///< ctDouble
	{
		double dblValue;
		memcpy( &dblValue, FData, sizeof(double) );
		Result = (int)dblValue;
	}

	return Result;
}
//------------------------------------------------------------------------------------------------------------------------
const Int64 MColumn::ToInt64( void )
{
	Int64 Result = 0LL;

	if ( (FData == NULL) || (FDefinition == NULL) )
		return Result;

	if ( FDefinition->FType == ctInt64 ) ///< ctInt64
	{
		memcpy( &Result, FData, sizeof(Int64) );
	}
	else if ( FDefinition->FType == ctString ) ///< ctString
	{
		Result = UFC::AnsiString( (const char*)FData ).ToInt64();
	}
	else if ( FDefinition->FType == ctChar ) ///< ctChar
	{
		Result = (Int64)FData[ 0 ];
	}
	else if ( FDefinition->FType == ctInt ) ///< ctInt
	{
		int intValue;
		memcpy( &intValue, FData, sizeof(int) );
		Result = (Int64)intValue;
	}
	else if ( FDefinition->FType == ctDouble ) ///< ctDouble
	{
		double dblValue;
		memcpy( &dblValue, FData, sizeof(double) );
		Result = (Int64)dblValue;
	}

	return Result;
}
//------------------------------------------------------------------------------------------------------------------------
const double MColumn::ToDouble( void )
{
	double Result = 0.0;

	if ( (FData == NULL) || (FDefinition == NULL) )
		return Result;

	if ( FDefinition->FType == ctDouble ) ///< ctDouble
	{
		memcpy( &Result, FData, sizeof(double) );
	}
	else if ( FDefinition->FType == ctString ) ///< ctString
	{
		Result = UFC::AnsiString( (const char*)FData ).ToDouble();
	}
	else if ( FDefinition->FType == ctChar ) ///< ctChar
	{
		Result = (double)FData[ 0 ];
	}
	else if ( FDefinition->FType == ctInt ) ///< ctInt
	{
		int intValue;
		memcpy( &intValue, FData, sizeof(int) );
		Result = (double)intValue;
	}
	else if ( FDefinition->FType == ctInt64 ) ///< ctInt64
	{
		Int64 int64Value;
		memcpy( &int64Value, FData, sizeof(Int64) );
		Result = (double)int64Value;
	}

	// avoid minus zero present confuse
	if ( mdsFloatEqual( Result, 0.0 ) )
		return 0.0;

	return Result;
}
//------------------------------------------------------------------------------------------------------------------------
void MColumn::FromString( const UFC::AnsiString& Data )
{
	if ( (FData == NULL) || (FDefinition == NULL) )
		return;

	if ( FDefinition->FType == ctString ) ///< ctString
	{
		int CopyLen = ( Data.Length() < FDefinition->FMaxLength ) ? Data.Length() : (FDefinition->FMaxLength-1);
		memcpy( FData, Data.c_str(), CopyLen );
		memset( FData+CopyLen, '\0', FDefinition->FMaxLength-CopyLen ); ///< Padding '\0'
	}
	else if ( FDefinition->FType == ctChar ) ///< ctChar
	{
		if ( Data.Length() > 0 )
			FData[ 0 ] = Data[ 0 ];
		else
			FData[ 0 ] = ' ';
	}
	else if ( FDefinition->FType == ctInt ) ///< ctInt
	{
		int intValue = Data.ToInt();
		memcpy( FData, &intValue, sizeof(int) );
	}
	else if ( FDefinition->FType == ctInt64 ) ///< ctInt64
	{
		Int64 int64Value = Data.ToInt64();
		memcpy( FData, &int64Value, sizeof(Int64) );
	}
	else if ( FDefinition->FType == ctDouble ) ///< ctDouble
	{
		double dblValue = Data.ToDouble();
		memcpy( FData, &dblValue, sizeof(double) );
	}
	else
		memset( FData, 0x00, FDefinition->FMaxLength );
}
//------------------------------------------------------------------------------------------------------------------------
void MColumn::FromChar( const char Data )
{
	if ( (FData == NULL) || (FDefinition == NULL) )
		return;

	if ( FDefinition->FType == ctChar ) ///< ctChar
	{
		FData[ 0 ] = Data;
	}
	else if ( FDefinition->FType == ctString ) ///< ctString
	{
		FData[ 0 ] = Data;
		memset( FData+1, '\0', FDefinition->FMaxLength-1 ); ///< Padding '\0'
	}
	else if ( FDefinition->FType == ctInt ) ///< ctInt
	{
		int intValue = (int)Data;
		memcpy( FData, &intValue, sizeof(int) );
	}
	else if ( FDefinition->FType == ctInt64 ) ///< ctInt64
	{
		Int64 int64Value = (Int64)Data;
		memcpy( FData, &int64Value, sizeof(Int64) );
	}
	else if ( FDefinition->FType == ctDouble ) ///< ctDouble
	{
		double dblValue = (double)Data;
		memcpy( FData, &dblValue, sizeof(double) );
	}
	else
		memset( FData, 0x00, FDefinition->FMaxLength );
}
//------------------------------------------------------------------------------------------------------------------------
void MColumn::FromInt( const int Data )
{
	if ( (FData == NULL) || (FDefinition == NULL) )
		return;

	if ( FDefinition->FType == ctInt ) ///< ctInt
	{
		memcpy( FData, &Data, sizeof(int) );
	}
	else if ( FDefinition->FType == ctString ) ///< ctString
	{
		char TmpBuf[16];
		int StrLen = sprintf( TmpBuf, "%d", Data );
		if ( StrLen < FDefinition->FMaxLength )
		{
			memcpy( FData, TmpBuf, StrLen );
			memset( FData+StrLen, '\0', FDefinition->FMaxLength-StrLen ); ///< Padding '\0'
		}
		else
		{
			int  CopyLen = FDefinition->FMaxLength - 1;
			char *CopyPos = TmpBuf + ( StrLen - CopyLen ); ///< Retrive right alignment
			memcpy( FData, CopyPos, CopyLen );
			FData[ CopyLen ] = 0;
		}
	}
	else if ( FDefinition->FType == ctChar ) ///< ctChar
	{
		FData[ 0 ] = (char)Data;
	}
	else if ( FDefinition->FType == ctInt64 ) ///< ctInt64
	{
		Int64 int64Value = (Int64)Data;
		memcpy( FData, &int64Value, sizeof(Int64) );
	}
	else if ( FDefinition->FType == ctDouble ) ///< ctDouble
	{
		double dblValue = (double)Data;
		memcpy( FData, &dblValue, sizeof(double) );
	}
	else
		memset( FData, 0x00, FDefinition->FMaxLength );
}
//------------------------------------------------------------------------------------------------------------------------
void MColumn::FromInt64( const Int64 Data )
{
	if ( (FData == NULL) || (FDefinition == NULL) )
		return;

	if ( FDefinition->FType == ctInt64 ) ///< ctInt64
	{
		memcpy( FData, &Data, sizeof(Int64) );
	}
	else if ( FDefinition->FType == ctString ) ///< ctString
	{
		char TmpBuf[32];
		int StrLen = sprintf( TmpBuf, "%lld", Data );
		if ( StrLen < FDefinition->FMaxLength )
		{
			memcpy( FData, TmpBuf, StrLen );
			memset( FData+StrLen, '\0', FDefinition->FMaxLength-StrLen ); ///< Padding '\0'
		}
		else
		{
			int  CopyLen = FDefinition->FMaxLength - 1;
			char *CopyPos = TmpBuf + (StrLen - CopyLen ); ///< Retrive right alignment
			memcpy( FData, CopyPos, CopyLen );
			FData[ CopyLen ] = 0;
		}
	}
	else if ( FDefinition->FType == ctChar ) ///< ctChar
	{
		FData[ 0 ] = (char)Data;
	}
	else if ( FDefinition->FType == ctInt ) ///< ctInt
	{
		int intValue = (int)Data;
		memcpy( FData, &intValue, sizeof(int) );
	}
	else if ( FDefinition->FType == ctDouble ) ///< ctDouble
	{
		double dblValue = (double)Data;
		memcpy( FData, &dblValue, sizeof(double) );
	}
	else
		memset( FData, 0x00, FDefinition->FMaxLength );
}
//------------------------------------------------------------------------------------------------------------------------
void MColumn::FromDouble( const double Data )
{
	if ( (FData == NULL) || (FDefinition == NULL) )
		return;

	// avoid minus zero present confuse
	double dblValue;
	if ( mdsFloatEqual( Data, 0.0 ) )
		dblValue = 0.0;
	else
		dblValue = Data;

	if ( FDefinition->FType == ctDouble ) ///< ctDouble
	{
		memcpy( FData, &dblValue, sizeof(double) );
	}
	else if ( FDefinition->FType == ctString ) ///< ctString
	{
		char TmpBuf[16];
		int StrLen = sprintf( TmpBuf, "%lf", dblValue );
		if ( StrLen < FDefinition->FMaxLength )
		{
			memcpy( FData, TmpBuf, StrLen );
			memset( FData+StrLen, '\0', FDefinition->FMaxLength-StrLen ); ///< Padding '\0'
		}
		else
		{
			int  CopyLen = FDefinition->FMaxLength - 1;
			char *CopyPos = TmpBuf + ( StrLen - CopyLen ); ///< Retrive right alignment
			memcpy( FData, CopyPos, CopyLen );
			FData[ CopyLen ] = 0;
		}
	}
	else if ( FDefinition->FType == ctChar ) ///< ctChar
	{
		FData[ 0 ] = (char)dblValue;
	}
	else if ( FDefinition->FType == ctInt ) ///< ctInt
	{
		int intValue = (int)dblValue;
		memcpy( FData, &intValue, sizeof(int) );
	}
	else if ( FDefinition->FType == ctInt64 ) ///< ctInt64
	{
		Int64 int64Value = (Int64)dblValue;
		memcpy( FData, &int64Value, sizeof(Int64) );
	}
	else
		memset( FData, 0x00, FDefinition->FMaxLength );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL MColumn::IsIndexMemberColumn( void )
{
	return FDefinition->FIsIndexMember;
}
//------------------------------------------------------------------------------------------------------------------------
} ///< namespace MDS
//------------------------------------------------------------------------------------------------------------------------
