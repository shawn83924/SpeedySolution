#include "TFixedFormat.h"
#include "Exception.h"
//---------------------------------------------------------------------------
//const char* DEFAULT_CONFIG_SECTION_NAME = "FixedFormatConfig";
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
const UFC::AnsiString TFixedFormat::DEFAULT_CONFIG_SECTION_NAME( "FixedFormatConfig" );
//---------------------------------------------------------------------------
TFixedFormat::TFixedFormat()
:TDataFormat(DFT_FIXED)
,FLineLength(0)
,FErrorMsg( "" )
{
}
//---------------------------------------------------------------------------
TFixedFormat::TFixedFormat(const AnsiString& IniFilePath)
:TDataFormat(DFT_FIXED)
,FLineLength(0)
,FErrorMsg( "" )
{
    SetIniFile( IniFilePath );
}   
//---------------------------------------------------------------------------
TFixedFormat::~TFixedFormat()
{
}
//---------------------------------------------------------------------------
bool TFixedFormat::SetIniFile(const AnsiString& IniFilePath)
{
    try
    {
        UiniFile iniFile( IniFilePath, FALSE );  //not Create File
        InitialFromIniFile( &iniFile );
        return true;
    }
    catch(UFC::FileException& ex)
    {
        FErrorMsg = UFC::AnsiString( ex.what() );
    }        
    catch(...)
    {
        FErrorMsg = "Exception";
    }
    return false;
}
//---------------------------------------------------------------------------
void TFixedFormat::InitialFromIniFile(UiniFile* pIniFile)
{
    try
    {
        ClearField();

        if( pIniFile != NULL )
        {
            LoadConfigSetting( pIniFile );
            LoadFieldSetting( pIniFile );
        }
    }
    catch(...)
    {
    }
}
//---------------------------------------------------------------------------
const AnsiString& TFixedFormat::GetDefaultConfigSectionName()
{
    return DEFAULT_CONFIG_SECTION_NAME;
}
//---------------------------------------------------------------------------
void TFixedFormat::LoadConfigSetting(UiniFile* pIniFile)
{
    AnsiString Value( "" );

    if( pIniFile->GetValue( DEFAULT_CONFIG_SECTION_NAME, "LineLength", Value ) )
        SetLineLength( Value.ToInt() );

    LoadConfigSettingEx( pIniFile );
}
//---------------------------------------------------------------------------
TFieldDataType TFixedFormat::ReadDataType( Section* pSection )
{
    AnsiString Value;

    if( pSection->GetValue( "DataType", Value ) )
    {
        if( Value == "INT" )
            return FDT_INT;
    }

    return FDT_STRING;
}
//---------------------------------------------------------------------------
void TFixedFormat::LoadFieldSetting( UiniFile* pIniFile )
{
    for( register Int32 i = 0; i < pIniFile->SectionCount(); i++ )
    {
      	Section* pSection = pIniFile->GetSection(i);
        if( pSection != NULL )
        {
            AnsiString SectionName = pSection->GetSectionName();
            if( (DEFAULT_CONFIG_SECTION_NAME != SectionName) && !IsConfigSection( SectionName ) )
            {
                AnsiString StartAt("");
                AnsiString Size("");
                if( pSection->GetValue( "StartAt", StartAt ) && pSection->GetValue( "Size", Size ) )
                {
                    AnsiString DefaultValue("");
                    pSection->GetValue( "Default", DefaultValue );
                    AddField( SectionName, StartAt.ToInt(), Size.ToInt(), ReadDataType(pSection), DefaultValue );
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
bool TFixedFormat::Parse( const char* inBUFF, int Size, TRecord* outREC )
{
    if( (inBUFF != NULL) && (outREC != NULL) )
    {
        Int32 FieldCount = FFieldOffset.ItemCount();
        if( FieldCount == outREC->GetFieldCount() )
        {
            TFieldOffset* pFieldOffset;
    	    for( register Int32 i = 0; i < FieldCount; i++ )
            {
                pFieldOffset = FFieldOffset.GetItem(i);
                if( ( pFieldOffset->FStartAt + pFieldOffset->FSize) <= Size )
          	      outREC->SetField( i, AnsiString( (inBUFF + pFieldOffset->FStartAt), pFieldOffset->FSize ) );
            }
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void TFixedFormat::LoadStreamToStringList( PStream* pStream, PtrList<AnsiString>* pStringList )
{
    char* Buff = new char[ FLineLength ];

    int ReadSize;

    while( (ReadSize = pStream->Read( Buff, FLineLength )) > 0 )
    {
        if( ReadSize == FLineLength)
            pStringList->Add( new AnsiString( Buff, ReadSize ) );
    }

    delete [] Buff;
}
//---------------------------------------------------------------------------
AnsiString TFixedFormat::Render( TRecord* inREC )
{
    if( inREC != NULL )
    {
        Int32 FieldCount = FFieldOffset.ItemCount();
        if( FieldCount == inREC->GetFieldCount() )
        {
            char* Buff = new char[ FLineLength ];
            memset( Buff, 0x20, FLineLength );
            TFieldOffset* pFieldOffset;
            AnsiString FieldValue;

            for( register Int32 i = 0; i < FieldCount; i++ )
            {
                pFieldOffset = FFieldOffset.GetItem(i);
                if( (pFieldOffset->FStartAt + pFieldOffset->FSize) <= FLineLength )
                {
                    inREC->GetField( i, FieldValue );

                    if( FFieldSchema.GetDataType(i) == FDT_INT )
                        FillZeroChar( FieldValue, pFieldOffset->FSize );

                    CopyFieldValueToBuffer( pFieldOffset, FieldValue, Buff );
                }
            }

            AnsiString Result( Buff, FLineLength );
            delete [] Buff;
            return Result;
        }
    }

    return "";
}
//---------------------------------------------------------------------------
void TFixedFormat::FillZeroChar( AnsiString& Value, Int32 Size )
{
    if( Size > 0 )
    {
        if( Value.Length() == 0 )
            Value = "0";
        Int32 IntageValue = Value.ToInt();
        Value.Printf( "%0*d", Size, IntageValue );
    }
    else
    {
        Value = "";
    }
}
//---------------------------------------------------------------------------
void TFixedFormat::CopyFieldValueToBuffer( TFieldOffset* pFieldOffset, const AnsiString& FieldValue, char* Buff )
{
    Int32 CpSize = FieldValue.Length();

    if( CpSize > pFieldOffset->FSize )
        CpSize = pFieldOffset->FSize;

    if( CpSize > 0 )
        memcpy( Buff + pFieldOffset->FStartAt, FieldValue.c_str(), CpSize );
}
//---------------------------------------------------------------------------
BOOL  TFixedFormat::IsValidStream( UFC::PStream* Stm )
{
    Int64 sSize = Stm->GetSize();
             
    if( sSize == 0 || FLineLength == 0 )
        return FALSE;
    if( (double)(sSize/(Int64)FLineLength) == ((double)sSize/(double)FLineLength) )    
        return TRUE;
    return FALSE;
}    
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

