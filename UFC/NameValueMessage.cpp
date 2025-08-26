#include "NameValueMessage.h"
//---------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
NameValueMessage::NameValueMessage(  const UFC::AnsiString& Delimeter, const UFC::AnsiString& Equals )
:FDelimeter( Delimeter )
,FEquals( Equals )
{
}
//------------------------------------------------------------------------------
NameValueMessage::~NameValueMessage( void )
{
    Clear();
}
//------------------------------------------------------------------------------
BOOL NameValueMessage::Delete( const UFC::AnsiString& Name )
{
    if( Name.Length() > 0)
    {
        UFC::AnsiString* OldVal;

        if( (OldVal = FNameValues.GetObjectByKey( Name )) != NULL )
        {
            int OldIndex;
            UFC::AnsiString OldStr;

            ///< Remove from List
            if( OldVal->Length() > 0 )
                OldStr.Printf( "%s%s%s", Name.c_str(), FEquals.c_str(), OldVal->c_str() );
            else
                OldStr.Printf( "%s%s", Name.c_str(), FEquals.c_str() );
            if( (OldIndex = FList.IndexOf( OldStr )) != -1 )
                FList.Delete( OldIndex );
            ///< Remove from Hashtable
            FNameValues.DeleteByKey( Name );
            delete OldVal;
            return true;
        }
    }
    return false;
}
//------------------------------------------------------------------------------
void NameValueMessage::Append( const UFC::AnsiString& Name, const UFC::AnsiString& Value )
{
    if( Name.Length() > 0 )
    {
        UFC::AnsiString Str;
        UFC::AnsiString* OldVal;

        if( Value.Length() > 0)
            Str.Printf( "%s%s%s", Name.c_str(), FEquals.c_str(), Value.c_str() );
        else
            Str.Printf( "%s%s", Name.c_str(), FEquals.c_str() );
        if( (OldVal = FNameValues.GetObjectByKey( Name )) != NULL )
        {
            int OldIndex;
            UFC::AnsiString OldStr;

            ///< Update List
            if( OldVal->Length() > 0 )
                OldStr.Printf( "%s%s%s", Name.c_str(), FEquals.c_str(), OldVal->c_str() );
            else
                OldStr.Printf( "%s%s", Name.c_str(), FEquals.c_str() );
            if( (OldIndex = FList.IndexOf( OldStr )) != -1 )
                FList[ OldIndex ] = Str;
            else
                FList.Add( Str );
            ///< Update Hashtable
            FNameValues.SetObjectByKey( Name, new UFC::AnsiString( Value ) );
            delete OldVal;
        }
        else
        {
            ///< Add to Hashtable
            FNameValues.Add( Name, new UFC::AnsiString(Value) );
            ///< Add to List
            FList.Add( Str );
        }
    }
}
//------------------------------------------------------------------------------
void NameValueMessage::Append( const UFC::AnsiString& Name, long Value )
{
    UFC::AnsiString StringValue;
    StringValue.Printf( "%d", Value );

    Append( Name, StringValue );
}
//------------------------------------------------------------------------------
void NameValueMessage::Append( const UFC::AnsiString& Name, double Value, bool TrimZero )
{
    UFC::AnsiString StringValue;
    StringValue.Printf( "%0.9lf", Value );

    if( TrimZero == true )
    {
        StringValue.TrimRight( '0' );
        StringValue.TrimRight( '.' );
    }
    Append( Name, StringValue );
}
//------------------------------------------------------------------------------
void NameValueMessage::AddFront( const UFC::AnsiString& Name, const UFC::AnsiString& Value )
{
    if( (Name.Length() > 0) && (Value.Length() > 0) )
    {
        UFC::AnsiString  NameValuePair;
        UFC::AnsiString* OldVal;

        NameValuePair.Printf( "%s%s%s", Name.c_str(), FEquals.c_str(), Value.c_str() );
        if( (OldVal = FNameValues.GetObjectByKey( Name )) != NULL )
        {
                int OldIndex;
                UFC::AnsiString OldNameValuePair;

                OldNameValuePair.Printf( "%s%s%s", Name.c_str(), FEquals.c_str(), OldVal->c_str() );
                if( ( OldIndex = FList.IndexOf( OldNameValuePair )) != -1 )
                         FList.Delete( OldIndex );
                FList.AddFront( NameValuePair );
                FNameValues.SetObjectByKey( Name, new UFC::AnsiString( Value ) );
                delete OldVal;
        }
        else
        {
                FNameValues.Add( Name, new UFC::AnsiString( Value ) );
                FList.AddFront( NameValuePair );
        }
    }
}
//------------------------------------------------------------------------------
void NameValueMessage::AddFront( const UFC::AnsiString& Name, long Value )
{
    UFC::AnsiString StringValue;

    StringValue.Printf( "%d", Value );
    AddFront( Name, StringValue );
}
//------------------------------------------------------------------------------
void NameValueMessage::AddFront( const UFC::AnsiString& Name, double Value, bool TrimZero )
{
    UFC::AnsiString StringValue;

    StringValue.Printf( "%0.8lf", Value );
    if( TrimZero == true )
    {
            StringValue.TrimRight( '0' );
            StringValue.TrimRight( '.' );
    }
    AddFront( Name, StringValue );
}
//------------------------------------------------------------------------------
UFC::AnsiString NameValueMessage::ToString( bool WithRtn )
{
    UFC::MemoryStream ms( FList.ItemCount()*10 );
    UFC::AnsiString Str;

    for( register int i = 0; i < FList.ItemCount(); i++ )
    {
        Str = FList[i];
        ms.Write( Str.c_str(), Str.Length() );
        ms.Write( FDelimeter.c_str(), 1 );
    }
    if( WithRtn == true )
        ms.Write( "\n\0", 2 );
    else
        ms.Write( "\0", 1 );
    Str = (const char*)(ms.GetBuffer());
    return Str;
}
//------------------------------------------------------------------------------
void NameValueMessage::FromString( const UFC::AnsiString& Str, bool TrimQuote )
{
    Clear();
    if( TrimQuote == true )
        FList.SetQuoteStrings( Str, FDelimeter );
    else
        FList.SetStrings( Str, FDelimeter );

    for( int i = 0; i < FList.ItemCount(); i++ )
    {
        UFC::AnsiString& Item = FList[i];
        int Pos      = Item.AnsiPos( FEquals );
        int ValueLen = Item.Length() - Pos - 1;

        if( Pos > 0 && ValueLen > 0 )
        {
            UFC::AnsiString  Name  = Item.SubString( 0, Pos );
            UFC::AnsiString* Value = new UFC::AnsiString( Item.SubString( Pos + 1 , ValueLen ) );

            if( TrimQuote == true )
            {
                Name.TrimLeft('"');
                Name.TrimRight('"');
                Value->TrimLeft('"');
                Value->TrimRight('"');
            }
            FNameValues.Add( Name , Value );
        }
    }
}
//------------------------------------------------------------------------------
BOOL NameValueMessage::Get( const UFC::AnsiString& Name, UFC::AnsiString& Value, BOOL ThrowException )
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
    if( ThrowException == TRUE )
    {
        UFC::AnsiString Msg;
        Msg.Printf( "Value [%s] not exists.", Name.c_str());
        throw UFC::Exception( Msg );
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL NameValueMessage::IsExists( const UFC::AnsiString& Name )
{
    if( Name.Length() > 0 )
    {
        UFC::AnsiString* pValue = FNameValues.GetObjectByKey( Name );
        if( pValue != NULL )
            return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
void NameValueMessage::Clear( void )
{
    UFC::AnsiString* Value = FNameValues.First();

    while( Value != NULL )
    {
        delete Value;
        Value = FNameValues.Next();
    }
    FNameValues.Clear();
    FList.Clear();
}
//------------------------------------------------------------------------------
int  NameValueMessage::ItemCount( void )
{
    return FNameValues.ItemCount();
}
//------------------------------------------------------------------------------
BOOL NameValueMessage::Get( int Index, UFC::AnsiString& Name, UFC::AnsiString& Value )
{
    if( Index >= 0 && Index < ItemCount() )
    {
        UFC::PStringList Split;
        
        Split.SetStrings( FList[Index], FEquals );
        if( Split.ItemCount() == 2 )
        {
            Name = Split[0];
            Value = Split[1];
            return TRUE;
        }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
