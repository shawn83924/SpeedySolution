#include "TMPDataFormat.h"
//-----------------------------------------------------------------------------------------
namespace TMP
{
//-----------------------------------------------------------------------------------------
// Class TMPFieldInfo
// class to keep TMP field information.
//-----------------------------------------------------------------------------------------
TMPFieldInfo::TMPFieldInfo( const UFC::AnsiString& Name, TMPType* DefVal )
:FName( Name )
,FDefaultVal( DefVal )
{
}
//-----------------------------------------------------------------------------------------
TMPFieldInfo::~TMPFieldInfo( void )
{
    if( FDefaultVal != NULL )
        delete FDefaultVal;
}
//-----------------------------------------------------------------------------------------
TMPType*  TMPFieldInfo::CreateFieldFromStream( UFC::PStream* Stream )
{
    return FDefaultVal->FromStream( Stream );
}
//-----------------------------------------------------------------------------------------
TMPType*  TMPFieldInfo::CreateField( void )
{
    return FDefaultVal->Duplicate( );
}
//-----------------------------------------------------------------------------------------
// Class TMPDataFormat
// class to keep TMP message format.
// 1. Load config file.
// 2. Create all TMP fields with default value.
// 3, You can add all fields in this format with default values,
//    or you can add all fields from stream.
//------------------------------------------------------------------------------------------------------
TMPDataFormat::TMPDataFormat( const UFC::AnsiString& FormatFile )
:FMsgLen( 0 )
,FIsFIXLength( TRUE )
{
    LoadFormat( FormatFile );
}
//------------------------------------------------------------------------------------------------------
TMPDataFormat::~TMPDataFormat( void )
{
    for( Int32 i = 0; i < FFields.ItemCount( ); i++ )
         delete FFields.GetItem( i );   
}
//------------------------------------------------------------------------------------------------------
void TMPDataFormat::LoadFormat( const UFC::AnsiString& FormatFile )
{
    UFC::UiniFile iniFile( FormatFile );
    UFC::AnsiString Type,Name;

    if( iniFile.GetValue( "Header", "MessageType", Type ) &&
        iniFile.GetValue( "Header", "MessageName", FMsgName) )
    {
        FMsgType = (UInt8)Type.ToInt();
        LoadFieldSetting( &iniFile );
    }
    else
        throw( TMPConfigException( FormatFile ) );          
}
//------------------------------------------------------------------------------------------------------
TMPFieldInfo* TMPDataFormat::AddCharArrayField( const UFC::AnsiString& SectionName, UFC::Section* pSection )
{
    UFC::AnsiString Length,Default, Binary;
    Int32           ArraySize;
    BOOL            IsBinary = FALSE;
    
    if( pSection->GetValue( "Length", Length )  == FALSE )///< CharArray type must have Length property.
        throw( UFC::Exception( "CharArray field missing Length property.") );                                           
    if( pSection->GetValue( "Binary", Binary) == TRUE && Binary.ToInt() > 0  )
        IsBinary = TRUE;
    if( (ArraySize = Length.ToInt()) < 0 ) ///< Dynamic size field.
    {
        FIsFIXLength = FALSE;
        return new TMPFieldInfo( SectionName, TMPType::CreateTMPTypeObject( ttCharArray, ArraySize, IsBinary ) );
    }
    else ///< Fix length CharArray
    {
        if( pSection->GetValue( "Default", Default )  == TRUE ) ///< Default value exists? 
            return new TMPFieldInfo( SectionName, TMPType::CreateTMPTypeObject( ttCharArray, Default, IsBinary ) );        
        else 
            return new TMPFieldInfo( SectionName, TMPType::CreateTMPTypeObject( ttCharArray, ArraySize, IsBinary ) );                
    }
}
//------------------------------------------------------------------------------------------------------
TMPFieldInfo* TMPDataFormat::AddOthersField( TMPDataType  Datatype, const UFC::AnsiString& SectionName, UFC::Section* pSection )
{
    UFC::AnsiString Default;    
    ///< All others field types are fix length, NO Length property needed.    
    if( pSection->GetValue( "Default", Default )  == TRUE ) ///< Default value exists? 
        return new TMPFieldInfo( SectionName, TMPType::CreateTMPTypeObject( Datatype, Default, TRUE ) );
    else
        return new TMPFieldInfo( SectionName, TMPType::CreateTMPTypeObject( Datatype, 0, TRUE ) );    
}    
//------------------------------------------------------------------------------------------------------
Int16 TMPDataFormat::AddFieldInfo( const UFC::AnsiString& SectionName, UFC::Section* pSection )
{
    UFC::AnsiString Type;        
    
    if( pSection->GetValue( "Type", Type ) == TRUE ) ///< Type must exists.
    {
        TMPDataType   Datatype = ToTMPDataType( Type ); 
        TMPFieldInfo* NewFieldInfo;
        
        if( Datatype == ttCharArray ) 
            NewFieldInfo = AddCharArrayField( SectionName, pSection );
        else
            NewFieldInfo = AddOthersField( Datatype, SectionName, pSection );
        FFieldIndex.Add( SectionName, FFields.ItemCount() );
        FFields.Add( NewFieldInfo );
        return (Int16)NewFieldInfo->GetSize();
    }
    else
        throw( UFC::Exception( "Define field missing Type property.") );
}
//------------------------------------------------------------------------------------------------------
void TMPDataFormat::LoadFieldSetting( UFC::UiniFile* pIniFile )
{
    UFC::AnsiString SectionName;    
    UFC::Section*   pSection;
    
    
    for( register Int32 i = 0; i < pIniFile->SectionCount(); i++ )
    {
      	if( (pSection = pIniFile->GetSection( i )) != NULL )
        {
            SectionName = pSection->GetSectionName();
            if( SectionName != "Header" )  ///< Skip header section.
                FMsgLen += AddFieldInfo( SectionName, pSection);            
        }
    }
    FMsgLen += (Int16)TMP_HEADER_SIZE; ///< Add TMP Header size.(Not include msg_length and CheckSum fields)
}
//------------------------------------------------------------------------------------------------------
Int32 TMPDataFormat::GetFieldIndex( const UFC::AnsiString& Name ) 
{
    int Index;

    if( FFieldIndex.GetObjectByKey(Name, Index ) == FALSE  )
        return -1;
    return (Int32)Index;
}
//------------------------------------------------------------------------------------------------------
void TMPDataFormat::AddTMPFields( UFC::PtrList<TMPType>& Fields  )
{
    register TMPFieldInfo* FieldInfo = NULL;    
    
    for( register int i = 0; i < FFields.ItemCount(); i++ )
    {        
        try
        {
            FieldInfo = FFields.GetItem( i );    
            Fields.Add( FieldInfo->CreateField( ) );
        }
        catch( UFC::Exception& e )
        {
            UFC::BufferedLog::Printf( " Create field[%s] from TMPDataFormat failed.", FieldInfo->GetFieldName().c_str());
            throw e;
        }
    }
}
//------------------------------------------------------------------------------------------------------
void TMPDataFormat::AddTMPFieldsFromStream( UFC::PtrList<TMPType>& Fields , UFC::PStream* Stream )
{
    register TMPFieldInfo* FieldInfo = NULL;
    
    for( register int i = 0; i < FFields.ItemCount(); i++ )
    {
        try
        {
            FieldInfo = FFields.GetItem( i );
            Fields.Add( FieldInfo->CreateFieldFromStream( Stream ));
        }
        catch( UFC::Exception& e )
        {
            UFC::BufferedLog::Printf( " Create field[%s] from stream failed.", FieldInfo->GetFieldName().c_str());
            throw e;
        }
    }    
}
//------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------
