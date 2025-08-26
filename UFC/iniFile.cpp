//---------------------------------------------------------------------------
#include "iniFile.h"
#include "BufferedLog.h"
namespace UFC
{
//--------------------------------------------------------------------------------------------
Section::Section( const UFC::AnsiString SectionName )
:FName( SectionName )
{
}
//--------------------------------------------------------------------------------------------
Section::~Section( )
{
    UFC::AnsiString* Value = FNameValues.First();

    while( Value != NULL )
    {
        delete Value;
        Value = FNameValues.Next();
    }
    FNames.Clear();
}
//--------------------------------------------------------------------------------------------
BOOL Section::ValueExists( const UFC::AnsiString Name )
{
    return FNameValues.IsExists( Name );
}
//--------------------------------------------------------------------------------------------
void Section::DeleteValue( const UFC::AnsiString Name )
{
    FNameValues.DeleteByKey( Name );
    for( register Int32 i = 0; i < FNames.ItemCount();i++ )
    {
         if( FNames[i] == Name )
         {
             FNames.Delete( i );
             return;
         }
    }
}
//--------------------------------------------------------------------------------------------
BOOL Section::GetValue( const UFC::AnsiString Name, UFC::AnsiString& Value )
{
    UFC::AnsiString* ValuePtr;

    if( FNameValues.GetObjectByKey( Name, ValuePtr ) == TRUE )
    {
        Value = *ValuePtr;
        return TRUE;
    }
    return FALSE;
}
//--------------------------------------------------------------------------------------------
void Section::SetValue( const UFC::AnsiString Name,  const UFC::AnsiString Value )
{
    UFC::AnsiString* NewValue = new UFC::AnsiString( Value );
    UFC::AnsiString* OldValue =	FNameValues.SetObjectByKey( Name, NewValue );

    if( NewValue != OldValue )
        delete OldValue;
    else
        FNames.Add( Name );
}
//--------------------------------------------------------------------------------------------
BOOL Section::GetNameValue( int Index, UFC::AnsiString& Name, UFC::AnsiString& Value )
{
    if( Index < 0 || Index >= FNames.ItemCount() )
        return FALSE;
    UFC::AnsiString* pValue = NULL;
    Name  = FNames[ Index ];
    FNameValues.GetObjectByKey( Name, pValue );
    Value = *pValue;
    return TRUE;
}
//--------------------------------------------------------------------------------------------
void Section::SaveToStream( PStream* Stream )
{
    char buffer[ 8192 ];

#ifdef _MSC_VER
    _snprintf( buffer, 8192, "[%s]\n",FName.c_str());
#else
	snprintf( buffer, 8192, "[%s]\n",FName.c_str());
#endif
    Stream->Write(buffer,(int)strlen(buffer));
    for( int i = 0; i < FNameValues.ItemCount();i++ )
    {
        UFC::AnsiString  Name  = FNames[ i ];
        UFC::AnsiString* Value = NULL;
    
        FNameValues.GetObjectByKey( Name, Value );
        if (Name.AnsiPos('=') >= 0)
        {
            sprintf( buffer, "\"%s\"=%s\n", Name.c_str(), Value->c_str());
        }
        else
        {
            sprintf( buffer, "%s=%s\n", Name.c_str(), Value->c_str());
        }        		
        Stream->Write(buffer,(int)strlen(buffer));
    }
    strcpy( buffer, "\n" );
    Stream->Write(buffer,(int)strlen(buffer));
}
//--------------------------------------------------------------------------------------------
int  Section::ItemCount( void ) 
{ 
    return FNames.ItemCount(); 
}
//--------------------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------------------
UiniFile::UiniFile( const UFC::AnsiString iniFileName, BOOL CreateFile )
:FFileName( iniFileName )
{
	ParseIniFile( FFileName, CreateFile );
}
//--------------------------------------------------------------------------------------------
UiniFile::UiniFile( void )
:FFileName( "Setting.ini" )
{
}
//--------------------------------------------------------------------------------------------
UiniFile::~UiniFile( )
{
	 Clear();
}
//--------------------------------------------------------------------------------------------
void UiniFile::Load(  const UFC::AnsiString iniFileName, BOOL CreateFile )
{
	Clear();
	FFileName = iniFileName;
	ParseIniFile( FFileName, CreateFile );
}
//--------------------------------------------------------------------------------------------
void UiniFile::Clear( void )
{
     for( int i = 0; i < FSections.ItemCount(); i++ )
          delete FSections.GetItem( i );
     FSections.Clear();
}
//--------------------------------------------------------------------------------------------
BOOL UiniFile::IsRemark( UFC::AnsiString& Line )
{
    if( Line[0] == '#' )
        return TRUE;
    return FALSE;
}
//--------------------------------------------------------------------------------------------
BOOL UiniFile::IsSection( UFC::AnsiString& Line )
{
    if( Line[0] == '[' &&  Line[ Line.Length()-1 ] == ']')
        return TRUE;
    return FALSE;
}
//--------------------------------------------------------------------------------------------
BOOL UiniFile::IsNameValue( UFC::AnsiString& Line )
{
    Int32 EqualPos = Line.AnsiPos( "=" );
    
    if( EqualPos == -1 ||             ///< "=" not exists.
        EqualPos ==  0 ||             ///< Name not exists
        EqualPos == Line.Length()-1 ) ///< Value not exists
        return FALSE;
    return TRUE;
}
//--------------------------------------------------------------------------------------------
Section* UiniFile::AddSection( UFC::AnsiString& Line )
{
    AnsiString    SectionName( Line.c_str() + 1, Line.Length() - 2 );

    SectionName.TrimLeft();
    SectionName.TrimRight();
    if( SectionName.Length() > 0 )
    {
        Section* NameValue = new Section( SectionName );
        FSections.Add( NameValue );
        //UFC::BufferedLog::Printf(" *Add Section[%s].", SectionName.c_str() );
        return NameValue;
    }
    return NULL;
}
//--------------------------------------------------------------------------------------------
void UiniFile::AddNameValue( Section* NameValue, UFC::AnsiString& Line )
{
     int         Pos = Line.AnsiPos( "=" );
     int         QMPos = Line.AnsiPos( '"' );
     AnsiString  Name, Value;
     
     if( QMPos == 0 ) // handle equal sign in name
     {         
         char* buf = (char*)malloc( Line.Length() + 1 );
         
         memset( buf, 0x0, Line.Length() + 1 );
         memcpy( buf, Line.c_str(), Line.Length() + 1 );
         Name = strtok( buf, "\"" );
         
         if( Name.Length() > 0 )
             Value = strtok( NULL, "\"=" ); // find second quotation mark
         
         free( buf );
     }
     else
     {
        Name = Line.SubString( 0, Pos );
        Value = Line.SubString( Pos + 1, Line.Length() - Pos -1 );
     }

     Name.TrimLeft();
     Name.TrimRight();
     if( Name.Length() > 0 )
     {
        Value.TrimLeft();
        Value.TrimRight();
        if( Value.Length() > 0 )
        {
            NameValue->SetValue( Name, Value );
            //UFC::BufferedLog::Printf("  --- Add Name[%s][%s].", Name.c_str(), Value.c_str() );
        }
     }
}
//--------------------------------------------------------------------------------------------
void UiniFile::ParseIniFile(  const UFC::AnsiString iniFileName, BOOL CreateFile )
{
    char          Buffer[ 8192 ] = "";
    FILE*         FileHandle;
    Section*      Section = NULL;

    if( (FileHandle = fopen( iniFileName.c_str(), "r" )) == NULL )
    {
        if( CreateFile == FALSE )
        {
            UFC::AnsiString Msg;
            Msg.Printf("Can not open ini file %s ",iniFileName.c_str());
            throw( FileException( (char*)Msg.c_str() ));
        }
        else
            return;
    }
    while( fgets( Buffer, 8192, FileHandle ) != NULL )
    {
        AnsiString Line( Buffer );

        Line.TrimLeft();
        Line.TrimRight();
        if( Line.Length() == 0 )
            continue;
        if( IsRemark( Line ))         ///< Is this line mark off?
            continue;
        else if( IsSection( Line ))   ///< Is a session header?
            Section = AddSection( Line );
        else if( IsNameValue( Line ) && Section != NULL )///< Is a name vaue pair?
            AddNameValue( Section, Line );
	Buffer[0] = 0;
    }
    fclose( FileHandle );
}
//--------------------------------------------------------------------------------------------
Section* UiniFile::FindSection( const UFC::AnsiString& SectionName )
{
    for( register int i = 0; i < FSections.ItemCount(); i++ )
    {
        Section* SearchSection = FSections.GetItem( i );
        if( SearchSection->GetSectionName() == SectionName )
            return SearchSection;
    }
    return NULL;
}
//--------------------------------------------------------------------------------------------
Int32 UiniFile::FindIndex( const UFC::AnsiString& SectionName )
{
    for( register int i = 0; i < FSections.ItemCount(); i++ )
    {
        Section* SearchSection = FSections.GetItem( i );
        if( SearchSection->GetSectionName() == SectionName )
            return i;
    }
    return -1;
}
//--------------------------------------------------------------------------------------------
void UiniFile::DeleteSection( int Index )
{
    Section* DelSection = FSections.GetItem( Index );
    if( DelSection != NULL )
    {
        FSections.Delete( Index );
        delete DelSection;
    }
}
//--------------------------------------------------------------------------------------------
BOOL UiniFile::DeleteSection( const UFC::AnsiString& SectionName )
{
    Section* SearchSection;

    for( register int i = 0; i < FSections.ItemCount(); i++ )
    {
        SearchSection = FSections.GetItem( i );
        if( SearchSection->GetSectionName() == SectionName )
        {
            FSections.Delete( i );
            delete SearchSection;
            return TRUE;
        }
    }
    return FALSE;
}
//--------------------------------------------------------------------------------------------
BOOL UiniFile::SectionExists( const UFC::AnsiString Section )
{
    if( Section.Length() > 0 && FindSection( Section ) != NULL )
        return TRUE;
    return FALSE;
}
//--------------------------------------------------------------------------------------------
BOOL UiniFile::ValueExists( const UFC::AnsiString SectionName, const UFC::AnsiString Ident )
{
    if( SectionName.Length() > 0 )
    {        
        Section* NameValue = FindSection( SectionName );
        if( NameValue != NULL && Ident.Length() > 0 && NameValue->ValueExists( Ident ) == TRUE )
            return TRUE;
    }
    return FALSE;
}
//--------------------------------------------------------------------------------------------
BOOL UiniFile::GetValue( const UFC::AnsiString SectionName, const UFC::AnsiString Name, UFC::AnsiString& Value )
{
    if( SectionName.Length() > 0 )
    {        
        Section* NameValue = FindSection( SectionName );

        if( NameValue != NULL && Name.Length() > 0 )
        {
            if( NameValue->GetValue( Name, Value ) == TRUE )
                return TRUE;
        }
    }
    return FALSE;
}
//--------------------------------------------------------------------------------------------
void UiniFile::SetValue( const UFC::AnsiString SectionName, const UFC::AnsiString Name, const UFC::AnsiString Value )
{
    Section*  SessionSet = FindSection( SectionName );
    if( SessionSet == NULL )
    {
        SessionSet = new Section( SectionName );
        FSections.Add( SessionSet );
    }
    SessionSet->SetValue( Name, Value );
}
//--------------------------------------------------------------------------------------------
void UiniFile::SaveToStream( PStream* Stream )
{
	for( register int i = 0; i < FSections.ItemCount(); i++ )
		 FSections.GetItem( i )->SaveToStream( Stream  );
	UFC::BufferedLog::Printf( " [UiniFile::SaveToStream] %d Sections saved.", FSections.ItemCount() );
}
//--------------------------------------------------------------------------------------------
bool UiniFile::Save( void )
{
	try
	{
		UFC::FileStream saveini( FFileName, O_CREAT|O_TRUNC|O_RDWR  );
		SaveToStream( &saveini );
		return true;
	}
	catch( UFC::FileException& ex )
	{
		UFC::BufferedLog::Printf( " [UiniFile::Save] Error[%s]", ex.what() );
		return false;
	}

}
//--------------------------------------------------------------------------------------------
}
//--------------------------------------------------------------------------------------------
