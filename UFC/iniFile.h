//---------------------------------------------------------------------------
#ifndef IniFileH
#define IniFileH
//---------------------------------------------------------------------------
#include "List.h"
#include "PHashedList.h"
#include "Stream.h"
#include "FileStream.h"
//---------------------------------------------------------------------------
namespace UFC
{
//---------------------------------------------------------------------------
class Section
{
private:
    typedef PHashMap<UFC::AnsiString, UFC::AnsiString*> NameValues;
    AnsiString FName;
    NameValues FNameValues;
    UFC::PStringList FNames;
public:
    Section( const UFC::AnsiString SectionName );
    ~Section( );
    const UFC::AnsiString& GetSectionName( void ) { return FName; }
    BOOL ValueExists( const UFC::AnsiString Name );
    void DeleteValue(  const UFC::AnsiString Name );
    BOOL GetValue( const UFC::AnsiString Name, UFC::AnsiString& Value );
    void SetValue( const UFC::AnsiString Name,  const UFC::AnsiString Value );
    void SaveToStream( PStream* Stream );
    int  ItemCount( void );
    BOOL GetNameValue( int Index, UFC::AnsiString& Name, UFC::AnsiString& Value );
};
//---------------------------------------------------------------------------
class UiniFile
{
private:
    UFC::PtrList<Section> FSections;
    UFC::AnsiString       FFileName;
private:
    BOOL IsRemark( UFC::AnsiString& Line );
    BOOL IsSection( UFC::AnsiString& Line );
    BOOL IsNameValue( UFC::AnsiString& Line );
    Section* AddSection( UFC::AnsiString& Line );
    Section* FindSection( const UFC::AnsiString& SectionName );
    Int32    FindIndex( const UFC::AnsiString& SectionName );
    void ParseIniFile(  const UFC::AnsiString iniFileName, BOOL CreateFile );
    void AddNameValue( Section* NameValue, UFC::AnsiString& Line );
public:
    UiniFile( const UFC::AnsiString iniFileName, BOOL CreateFile = FALSE );
    UiniFile( void );
    ~UiniFile( );
    BOOL       ValueExists( const UFC::AnsiString Section, const UFC::AnsiString Ident );
    BOOL       GetValue( const UFC::AnsiString Section, const UFC::AnsiString Name, UFC::AnsiString& Value );
    void       SetValue( const UFC::AnsiString Section, const UFC::AnsiString Name, const UFC::AnsiString Value );
    void       SaveToStream( PStream* Stream );
    void       Clear( void );
    bool       Save( void );
    void       Load(  const UFC::AnsiString iniFileName, BOOL CreateFile = FALSE  );
    BOOL       SectionExists( const UFC::AnsiString Section );
    void       AddSection( Section* NewSection ) { FSections.Add( NewSection ); }
    BOOL       DeleteSection( const UFC::AnsiString& SectionName );
    void       DeleteSection( int Index );
    Section*   GetSection( int Index ){ return FSections.GetItem( Index );}
    Section*   GetSection( const UFC::AnsiString& SectionName ){ return FindSection( SectionName );}
    Int32      GetSectionIndex( const UFC::AnsiString& SectionName ){ return FindIndex( SectionName );}
    Int32      SectionCount() { return FSections.ItemCount(); }
};
}
//---------------------------------------------------------------------------
#endif
