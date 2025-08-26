#include "SQMessage.h"
//------------------------------------------------------------------------------
TSQMessage::TSQMessage( void )
{
}
//------------------------------------------------------------------------------
TSQMessage::~TSQMessage( void )
{
    Clear();
}
//------------------------------------------------------------------------------
void TSQMessage::Append( const UFC::AnsiString& Name, const UFC::AnsiString& Value )
{
    if( (Name.Length() > 0) && (Value.Length() > 0) )
    {
        UFC::AnsiString Str;
        Str.Printf( "%s=%s", Name.c_str(), Value.c_str() );
        FNameValues.Add( Name, new UFC::AnsiString(Value) );
        FList.Add( Str );
    }
}
//------------------------------------------------------------------------------
void TSQMessage::Append( const UFC::AnsiString& Name, long Value )
{
    UFC::AnsiString StringValue;
    StringValue.Printf( "%d", Value );

    Append( Name, StringValue );
}
//------------------------------------------------------------------------------
void TSQMessage::Append( const UFC::AnsiString& Name, double Value )
{
    UFC::AnsiString StringValue;
    StringValue.Printf( "%lf", Value );

    Append( Name, StringValue );
}
//------------------------------------------------------------------------------
UFC::AnsiString TSQMessage::ToString( void )
{
    UFC::MemoryStream ms;
    UFC::AnsiString Str;
    
    for( int i = 0; i < FList.ItemCount(); i++ )
    {
        Str = FList[i];
        ms.Write( Str.c_str(), Str.Length() );
        ms.Write( "|", 1 );
    }
    ms.Write( "01=", 4 );
    
    Str = (const char*)(ms.GetBuffer());
    return Str;
}
//------------------------------------------------------------------------------
void TSQMessage::FromString( const UFC::AnsiString& Str )
{
    Clear();
    FList.SetStrings( Str, "|" );
    UFC::PStringList Split;
    
    for( int i = 0; i < FList.ItemCount(); i++ )
    {
        Split.SetStrings( FList[i], "=" );
        if( Split.ItemCount() == 2 )
            FNameValues.Add( Split[0], new UFC::AnsiString(Split[1]) );
    }
}
//------------------------------------------------------------------------------
BOOL TSQMessage::Get( const UFC::AnsiString& Name, UFC::AnsiString& Value )
{
    if( Name.Length() > 0 )
    {
        UFC::AnsiString* pValue = FNameValues.GetObjectByKey( Name );
        if( pValue != NULL )
        {
            Value = pValue->c_str();
            return TRUE;
        }
    }
    
    UFC::BufferedLog::DebugPrintf( " TSQMessage::Get( %s ) return FALSE", Name.c_str() );
    return FALSE;
}
//------------------------------------------------------------------------------
void TSQMessage::Clear( void )
{
    for( int i = 0; i < FNameValues.ItemCount(); i++ )
         delete FNameValues.GetItem( i );

    FNameValues.Clear();
    FList.Clear();
}
//------------------------------------------------------------------------------
