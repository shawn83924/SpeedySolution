#include "TMPTypes.h"
//-----------------------------------------------------------------------------------------
namespace TMP
{
//------------------------------------------------------------------------------------------------------    
// Class TMPCharArray
// Implement CharArray class for TMP protocol.
//
// ( note: Len < 0 means it's a variable length field)
//------------------------------------------------------------------------------------------------------    
TMPCharArray::TMPCharArray( Int32 Len, BOOL IsBin )
:TMPType(ttCharArray)
,FIsBinary( IsBin )
{ 
    if( Len > 0 ) ///< Fix length field.
    {
        FDynamicLen    = FALSE;
		FDataLen       = (UInt16)Len;
        FData          = new UInt8[ FDataLen+1 ]; 
        FOtherFieldLen = 0;
        if( IsBin == TRUE ) ///< Binary field.
            memset( FData, 0, FDataLen );
        else ///< Text field.
            memset( FData, ' ', FDataLen );
        *( FData+ FDataLen ) = 0;
    }
    else ///< Length < 0 means it's a variable length field.
    {
        FDynamicLen    = TRUE;    
        FDataLen       = 0;
        FData          = NULL;        
        FOtherFieldLen = Len;
    }
}
//------------------------------------------------------------------------------------------------------    
TMPCharArray::TMPCharArray( const UFC::AnsiString& Str, BOOL IsBin )
:TMPType( ttCharArray )
,FData( NULL )
,FDynamicLen( FALSE )
,FOtherFieldLen( 0 )
,FIsBinary( IsBin )
,FDataLen( 0 )
{ 
    FromString( Str );
}
//------------------------------------------------------------------------------------------------------    
TMPCharArray::TMPCharArray( UFC::PStream* Stream, Int32 Len )
:TMPType(ttCharArray)
,FDynamicLen( FALSE )
,FOtherFieldLen( 0 )
,FIsBinary( FALSE )
{
	FDataLen = (UInt16)Len;
    FData    = new UInt8[ FDataLen + 1 ]; 
    if( Stream->Read( (void*)FData, FDataLen ) != FDataLen )
        throw( TMPBufferException( FDataLen ) );        
    *( FData+ FDataLen ) = 0;
}
//------------------------------------------------------------------------------------------------------    
TMPCharArray::TMPCharArray( const TMPCharArray& ref )
:TMPType( ttCharArray )
,FDynamicLen( ref.FDynamicLen )
,FOtherFieldLen( ref.FOtherFieldLen )
,FIsBinary( ref.FIsBinary )
,FDataLen( ref.FDataLen )
{
    if( ref.FDataLen  == 0 && ref.FData == NULL)
    {
        FDataLen  = 0;
        FData     = NULL;        
    }
    else
    {
        FData = new UInt8[ FDataLen + 1 ];
        memcpy( FData, ref.FData, FDataLen + 1 );
    }
}
//------------------------------------------------------------------------------------------------------    
TMPCharArray::~TMPCharArray( void )
{
    if( FDataLen > 0 && FData != NULL )
        delete [] FData;
}
//------------------------------------------------------------------------------------------------------    
UInt8 TMPCharArray::HexChToUInt8( const char Ch )
{
    UFC::AnsiString Err;
    switch( Ch )
    {
        case '0':return 0;
        case '1':return 1;
        case '2':return 2;
        case '3':return 3;
        case '4':return 4;
        case '5':return 5;
        case '6':return 6;
        case '7':return 7;
        case '8':return 8;
        case '9':return 9;
        case 'A':
        case 'a':return 10;
        case 'B':
        case 'b':return 11;
        case 'C':
        case 'c':return 12;
        case 'D':
        case 'd':return 13;
        case 'E':
        case 'e':return 14;
        case 'F':
        case 'f':return 15;
        default: Err.Printf("Invalid char[%c] in hex string", Ch );   
                 throw( TMPFieldException( Err.c_str() ) );	                       
    }
}    
//------------------------------------------------------------------------------------------------------    
void TMPCharArray::FromHexString( const char* StrBuf, UInt8* Data, Int32 Len )
{
	for( register int i = 0; i < Len; i++ )
		 *(Data+i) = (UInt8)(16*HexChToUInt8(*(StrBuf+(i*2)))+ HexChToUInt8(*(StrBuf+(i*2)+1)));
}
//------------------------------------------------------------------------------------------------------
Int32 TMPCharArray::ToBinary( UInt8* Buffer )
{
    memcpy( Buffer, FData, FDataLen );
    return FDataLen; 
}        
//------------------------------------------------------------------------------------------------------    
Int32 TMPCharArray::ToStream( UFC::PStream* Stream )
{   
    return Stream->Write( (void*)FData, FDataLen ); 
}    
//------------------------------------------------------------------------------------------------------    
Int32 TMPCharArray::ToString( char* Str, UInt32 DataLength )
{
    if( FIsBinary == FALSE )
    {
        if( DataLength < (UInt32)FDataLen )
            throw( TMPBufferException( FDataLen ) );
        strcpy( Str, (char*)FData );
        return FDataLen;
    }
    else
    {
        if( DataLength < (UInt32)(FDataLen*2+2) )
            throw( TMPBufferException( FDataLen*2 ) );
        *Str     = '0';
        *(Str+1) = 'x';
        for( register int i = 0; i < FDataLen; i ++ )
             sprintf( Str+( 2*i )+2, "%02X", *(FData +i) );
        return FDataLen*2 + 2;
    }
}
//------------------------------------------------------------------------------------------------------    
Int32 TMPCharArray::ToInteger( void )
{
    throw( TMPFieldTypeException( "TMPCharArray", "Int32" ) );	   
}
//------------------------------------------------------------------------------------------------------        
void TMPCharArray::FromInteger( Int32  )
{    
    throw( TMPFieldTypeException( "Int32", "TMPCharArray" ) );	   
}
//------------------------------------------------------------------------------------------------------        
void TMPCharArray::SetTextData( const char* Str, int StrLen )
{
	UInt16 OrgLen = FDataLen;
    
    if( StrLen <= 0 || Str == NULL )
        throw( TMPFieldException( " Assign NULL text string to CharArray" ) );	                           
    if( FDynamicLen == FALSE )
    {
		FDataLen = OrgLen;
        if( FDataLen == 0 )
			FDataLen = (UInt16)StrLen;
        FData    = new UInt8[ FDataLen + 1 ]; 
        if( StrLen >= FDataLen )
            memcpy( FData, Str, FDataLen );         
        else
        {
            memset( FData, ' ', FDataLen );
            memcpy( FData, Str, StrLen ); 
        }        
    }
    else
    {
		FDataLen = (UInt16)StrLen;
        FData    = new UInt8[ FDataLen + 1 ];
        memcpy( FData, Str, StrLen );
    }
    *( FData+ FDataLen ) = 0;                
}
//------------------------------------------------------------------------------------------------------        
void TMPCharArray::SetBinaryData( const UInt8* Data, int DataLen )
{
	UInt16 OrgLen   = FDataLen;
    BOOL  IsHexStr = IsHexString( Data, DataLen );
    
    if( FDynamicLen == FALSE )
    {   ///< Fix length Char Array.         
		FDataLen = OrgLen;
        if( IsHexStr == TRUE )                
        {            
            int HexLen = (DataLen/2) - 1;
            if( FDataLen == 0 )
				FDataLen = (UInt16)HexLen;
            FData    = new UInt8[ FDataLen + 1 ];
            memset( FData, 0, FDataLen + 1 );        
            if( HexLen < FDataLen )
                FromHexString( (const char*)Data+2, FData, HexLen );
            else
                FromHexString( (const char*)Data+2, FData, FDataLen );
        }
        else
        {
            if( FDataLen == 0 )
				FDataLen = (UInt16)DataLen;
            FData    = new UInt8[ FDataLen + 1 ];
            memset( FData, 0, FDataLen + 1 );        
            if( DataLen < FDataLen )
                memcpy( FData, Data, DataLen );
            else
                memcpy( FData, Data, FDataLen );
        }
    }
    else
    {
        if( IsHexStr == TRUE )                
        {
			FDataLen = (UInt16)((DataLen/2) - 1);
            FData    = new UInt8[ FDataLen + 1 ];
            FromHexString( (const char*)Data+2, FData, FDataLen );
        }
        else
        {
			FDataLen = (UInt16)DataLen;
            FData    = new UInt8[ FDataLen + 1 ];                        
            memcpy( FData, Data, FDataLen );            
                    }
        *(FData + FDataLen) = 0;        
    }            
}    
//------------------------------------------------------------------------------------------------------        
BOOL TMPCharArray::IsHexString( const UInt8* Data, int DataLen )
{
    if( DataLen <= 0 || Data == NULL )
        throw( TMPFieldException( "Assign NULL string to CharArray" ) );	                       
    if( *Data == '0' && (*(Data+1) == 'x' || *(Data+1) == 'X') )
    {
        if( DataLen % 2 != 0 )
            throw( TMPFieldException( "Invalidate Hex string" ) );        
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------        
void TMPCharArray::FromString( const UFC::AnsiString& Str )
{       
    if( FDataLen > 0 && FData != NULL )
        delete FData;
    if( FIsBinary == FALSE )
        SetTextData( Str.c_str(), Str.Length());        
    else
        SetBinaryData(  (const UInt8*)Str.c_str(), Str.Length() );        
}    
//------------------------------------------------------------------------------------------------------    
TMPType* TMPCharArray::FromStream( UFC::PStream* Stream  ) 
{ 
    TMPCharArray* NewOne; 
    
    if( FDynamicLen == TRUE )    
        NewOne = new TMPCharArray( Stream, Stream->GetSize() + FOtherFieldLen ); 
    else
        NewOne = new TMPCharArray( Stream, FDataLen ); 
    NewOne->FDynamicLen = FDynamicLen;
    NewOne->FIsBinary   = FIsBinary;
    return NewOne;
}
//------------------------------------------------------------------------------------------------------    
TMPType* TMPCharArray::Duplicate( void  ) 
{ 
    return new TMPCharArray( *this );
}    
//------------------------------------------------------------------------------------------------------
void TMPCharArray::Assign( TMPType& ref )
{
    TMPCharArray* RefPtr = static_cast<TMPCharArray*>( &ref );
    if( FType != ref.GetType() )
        throw( TMPFieldTypeException( ToTMPTypeString( ref.GetType() ), ToTMPTypeString(FType) ) );

    if( FDataLen != 0 && FData != NULL)
        delete [] FData;
    FDynamicLen    = RefPtr->FDynamicLen;
    FOtherFieldLen = RefPtr->FOtherFieldLen;
    FIsBinary      = RefPtr->FIsBinary;
    FDataLen       = RefPtr->FDataLen;
    if( RefPtr->FDataLen == 0 && RefPtr->FData == NULL)
    {
        FDataLen  = 0;
        FData     = NULL;
    }
    else
    {
        FData     = new UInt8[ FDataLen + 1 ];
        memcpy( FData, RefPtr->FData, FDataLen + 1 );
    }
}
//------------------------------------------------------------------------------------------------------    
TMPCharArray& TMPCharArray::operator = ( const char* Val ) 
{
    FromString( Val );
    return *this;
}
//------------------------------------------------------------------------------------------------------    
TMPCharArray& TMPCharArray::operator = ( const TMPCharArray& ref ) 
{
    if( FDataLen > 0 && FData != NULL )
        delete FData;       
    FDynamicLen    = ref.FDynamicLen;
    FIsBinary      = ref.FIsBinary;
    FOtherFieldLen = ref.FOtherFieldLen;
    FDataLen       = ref.FDataLen;
    FData          = new UInt8[ FDataLen + 1 ]; 
    memcpy( FData, ref.FData, FDataLen + 1 );
    return *this;
}
//------------------------------------------------------------------------------------------------------    
TMPCharArray::operator const char* ( void ) 
{
    return (const char*) FData;
}
//------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------



