
#include "BufferedLog.h"
#include "AnsiString.h"
#include "NetUtility.h"
#include "PInt32.h"
#include "iniFile.h"
#include <iostream>
//------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
#define FileStream_DefaultBufferSize    12*4096
#define MIN_BUFFERSIZE                  24*4096
#define MAX_CacheLevel                  1024
//------------------------------------------------------------------------------
const int DEF_BUFFER_SIZE = 256;
//------------------------------------------------------------------------------
BufferedLogData GLogData;
//------------------------------------------------------------------------------
//  Class BufferedLogData
//------------------------------------------------------------------------------
BOOL             BufferedLogData::FIsCreated  = false;
char*            BufferedLogData::FSTRBuffer  = NULL;
Int32            BufferedLogData::FBufferSize = 0;
PStringList      BufferedLogData::FPreFixList;
PCriticalSection BufferedLogData::FBufferCS;
LogObject*       BufferedLogData::FLogObject  = NULL;
BOOL             BufferedLogData::FToStdout   = TRUE;
BOOL             BufferedLogData::FIsLog_us   = FALSE;
Int32            BufferedLogData::FTimeStrLen = 12; ///< "HH:MM:SS.sss" length =12 "HH:MM:SS.ssssss" length =15
Int32            BufferedLogData::FDebugLevel = 0;
BOOL	         BufferedLogData::FFlushAfterWrite = FALSE;
//------------------------------------------------------------------------------
BufferedLogData::BufferedLogData( void )
{
    FSTRBuffer  = new char[ DEF_BUFFER_SIZE ];
    FBufferSize = DEF_BUFFER_SIZE;	
    ///< FIsCreated  = true means
    ///< All static objects for BufferLog are ready to use.
    FIsCreated  = true;
}
//------------------------------------------------------------------------------
BufferedLogData::~BufferedLogData( void )
{
    FIsCreated = FALSE;
    if( FSTRBuffer != NULL )
	delete [] FSTRBuffer;
    FBufferSize = 0;
}
//------------------------------------------------------------------------------
//   Log name utility
//------------------------------------------------------------------------------
void AddToLogCleanerCfg( const AnsiString& SectionName, 
                         const AnsiString& RefDir, 
                         const AnsiString& Prefix, 
                         const AnsiString& Ext,                           
                         BOOL  TW_Year )
{
    UFC::SleepMS( rand()%100 + 50 );
    UFC::AnsiString Script;    
    UFC::UiniFile Config( "../cfg/LogCleaner.ini", TRUE );        
    
    if( Config.SectionExists( SectionName ) == FALSE )
    {       
        if( TW_Year == TRUE )
            Script = "string.format(\"" +Prefix+"%d%02d%02d."+Ext+"\",Y-1911,M,D)";
        else
            Script = "string.format(\"" +Prefix+"%d%02d%02d."+Ext+"\",Y,M,D)";
        Config.SetValue( SectionName, "RefDir", RefDir );
        Config.SetValue( SectionName, "KeepDays", "3" );
        Config.SetValue( SectionName, "DelDays", "360" );        
        Config.SetValue( SectionName, "FileName", Script );        
        Config.Save();
    }               
}
//------------------------------------------------------------------------------
void GenerateLogFileName( AnsiString& FileName, 
                          const AnsiString& RefDir, 
                          const AnsiString& Prefix, 
                          const AnsiString& Ext,                           
                          BOOL  TW_Year )
{
    UFC::AnsiString Today;
    
    if( TW_Year == TRUE )        
        UFC::GetTradeYYYMMDD( Today );  ///< Taiwan Year           
    else
        UFC::GetTradeYYYYMMDD( Today );
    if( RefDir.Length() == 0 ) //< Current Dir
        FileName.Printf( "%s%s.%s", Prefix.c_str(),Today.c_str(),Ext.c_str() );             
    else
        FileName.Printf( "../%s/%s%s.%s", RefDir.c_str(),Prefix.c_str(),Today.c_str(),Ext.c_str() );             
}
//------------------------------------------------------------------------------
//
//  Class BufferedLog
//  Constructor
//
//------------------------------------------------------------------------------
BufferedLog::BufferedLog( const AnsiString& SectionName, 
                          const AnsiString& RefDir, 
                          const AnsiString& Prefix, 
                          const AnsiString& Ext, 
                          int   BufferLength, 
                          BOOL  Append,
                          BOOL  TW_Year,
                          BOOL  Cache,
                          BOOL  Thread )
:PThread( NULL, TRUE )
,FFileStream( NULL )
,FBuffer( NULL ) 
,FStdOut( FALSE )
,FUseThread( Thread )
{
    GenerateLogFileName( FFileName, RefDir, Prefix, Ext, TW_Year );    
    AddToLogCleanerCfg( SectionName, RefDir, Prefix, Ext, TW_Year );
    if( Append == TRUE )
        FFileStream = new FileStream64( FFileName, "a" , FALSE, FileStream_DefaultBufferSize );
    else
        FFileStream = new FileStream64( FFileName, "w" , FALSE, FileStream_DefaultBufferSize );
    FBuffer = new CachedBuffer(FFileStream, BufferLength, Cache);
    if( FUseThread == TRUE )
        Start();
}
//------------------------------------------------------------------------------
BufferedLog::BufferedLog( AnsiString FileName, int BufferLength, BOOL Append, BOOL Cache)
:PThread( NULL, TRUE )
,FFileStream( NULL )
,FBuffer( NULL ) 
,FStdOut( FALSE )
,FUseThread( FALSE )
,FFileName( FileName )
{
    if( Append == TRUE )
        FFileStream = new FileStream64( FFileName, "a" ,  FileStream_DefaultBufferSize );
    else
        FFileStream = new FileStream64( FFileName, "w" ,  FileStream_DefaultBufferSize );
    FBuffer = new CachedBuffer(FFileStream, BufferLength, Cache);    
}
//------------------------------------------------------------------------------
BufferedLog::BufferedLog( BOOL IsStdOut )
:PThread( NULL, TRUE )
,FFileStream( NULL )
,FBuffer( NULL ) 
,FStdOut( IsStdOut )
,FUseThread( FALSE )
{
    
}
//------------------------------------------------------------------------------
BufferedLog::~BufferedLog()
{    
    if( FBuffer != NULL ) 
    {
        delete FBuffer;
        FBuffer = NULL;
    }
    if( FFileStream != NULL ) 
        delete FFileStream;
}
//------------------------------------------------------------------------------
void BufferedLog::Execute( void )
{
    char Buffer[ 4096 ];
    ObjToLog* LogObj;
    int Len;
    
    while( TRUE )
    {
        if( FLogObjQ.pop( &LogObj , 1 ) == TRUE )
        {
            if( LogObj != NULL )
            {
                Len = LogObj->ToString( Buffer, 4096 );
                if( Len > 0 && Len < 4096 ) 
                    FBuffer->Write( Buffer, Len, TRUE, BufferedLogData::FFlushAfterWrite) ;                        
                delete LogObj;
            }
        }
    }
}
//------------------------------------------------------------------------------
//<
//< Implement LogObject interface
//<
//------------------------------------------------------------------------------
void BufferedLog::Flush( BOOL IsBlock )
{
    if( FBuffer != NULL ) 
        FBuffer->Flush( IsBlock );
}
//------------------------------------------------------------------------------
void BufferedLog::WriteString( const char* LogString, Int32 StrLength )
{    
    if( FBuffer != NULL ) 
    {    
        if( FUseThread == TRUE )
            Write( new StringToLog( LogString, StrLength ));
        else
            FBuffer->Write(LogString, StrLength, TRUE, BufferedLogData::FFlushAfterWrite) ;
    }
}
//------------------------------------------------------------------------------
void BufferedLog::WriteString( const AnsiString& LogString )       
{ 
    WriteString( LogString.c_str(), LogString.Length()); 
}
//------------------------------------------------------------------------------
void BufferedLog::WriteString( const char* LogString )             
{ 
    WriteString( LogString, (Int32)strlen( LogString ) ); 
}
//------------------------------------------------------------------------------
void BufferedLog::Write( const char* Log, Int32 Length )
{    
    if( FBuffer != NULL ) 
    {
        if( FUseThread == TRUE )
            Write( new StringToLog( Log, Length ));
        else
            FBuffer->Write(Log, Length, FALSE, BufferedLogData::FFlushAfterWrite) ;
    }
}
//------------------------------------------------------------------------------
void BufferedLog::Write( const AnsiString& LogString )             
{ 
    Write( LogString.c_str(), LogString.Length()); 
}
//------------------------------------------------------------------------------
void BufferedLog::Write( ObjToLog* obj )
{
    if( FBuffer != NULL ) 
    {    
        if( FUseThread == FALSE ) ///< Thread mode off, write direct to file
        {
            char Buffer[ 4096 ];
            int Len = obj->ToString( Buffer, 4096 );

            if( Len > 0 && Len < 4096 &&  FBuffer != NULL ) 
                FBuffer->Write( Buffer, Len, TRUE, BufferedLogData::FFlushAfterWrite) ;    
            delete obj;
        }
        else ///< Thread mode on, add log object to queue
            FLogObjQ.push( obj );
    }
}
//------------------------------------------------------------------------------
//<
//< Implement static private functions
//<
//------------------------------------------------------------------------------
const int BUF_SIZE = 4096;
//------------------------------------------------------------------------------
inline void BufferedLog::GetTimestamp( char* Buffer )
{
    if( BufferedLogData::FIsLog_us == TRUE )
        GetTimeString_us( Buffer );
    else
        GetTimeString( Buffer );    
}
//------------------------------------------------------------------------------
Int32 BufferedLog::PrepareBuffer( Int32 NeedSize, Int32 PrefixIndex )
{
    Int32 PrefixSize,BufferSize;

    if( BufferedLogData::FIsCreated == TRUE ) ///< log buffer createded ?
    {
        if( PrefixIndex < 0 || PrefixIndex >= BufferedLogData::FPreFixList.ItemCount() )
            PrefixSize = 0;///< No prefix.
        else
            PrefixSize = BufferedLogData::FPreFixList.GetItem( PrefixIndex ).Length(); ///< Prefix size.
        BufferSize = NeedSize + PrefixSize + BufferedLogData::FTimeStrLen + 1; ///< (HH:MM:SS.sss) Length =12 \0 Length = 1
        if( BufferSize > BufferedLogData::FBufferSize || BufferedLogData::FSTRBuffer == NULL)
        {
            if( BufferSize > AnsiString::MAX_STR_BUFFER)
                BufferedLogData::FBufferSize = AnsiString::MAX_STR_BUFFER;
            else
                BufferedLogData::FBufferSize = BufferSize;
            if( BufferedLogData::FSTRBuffer != NULL )
                delete [] BufferedLogData::FSTRBuffer;
            BufferedLogData::FSTRBuffer = new char[ BufferedLogData::FBufferSize ];
        }
        if( BufferedLogData::FIsLog_us == TRUE )
            GetTimeString_us( BufferedLogData::FSTRBuffer );
        else
            GetTimeString( BufferedLogData::FSTRBuffer );
        if( PrefixSize > 0 )
            strcpy( BufferedLogData::FSTRBuffer + BufferedLogData::FTimeStrLen, BufferedLogData::FPreFixList.GetItem( PrefixIndex ).c_str());
        return PrefixSize + BufferedLogData::FTimeStrLen;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprintf( const char* FormatStr, ... )
{
    char        Buffer[ BUF_SIZE + 1 ];
    UInt16      CharCount;
    va_list     va;

    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    GetTimestamp( Buffer );
    va_start( va , FormatStr );        
    vsnprintf( Buffer + BufferedLogData::FTimeStrLen, BUF_SIZE - BufferedLogData::FTimeStrLen, FormatStr, va );
    va_end( va );

    CharCount = (UFCType::UInt16)strlen( Buffer );
    WriteString( (const char*)Buffer, CharCount );    
    if( FStdOut == TRUE )
        puts( Buffer );
    return CharCount;	
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL )
    {
        UInt16 CharCount;
        char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );        
        while ((*TextBuf++ = *str++ ) != 0);                        
        CharCount = TextBuf - Buffer - 1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1 )
{
    char Buffer[ BUF_SIZE + 1 ];        
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );        
        while ((*TextBuf++ = *str++ ) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);                        
        CharCount = TextBuf - Buffer - 1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2 )
{
    char Buffer[ BUF_SIZE + 1 ];        
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL)
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);                        
        CharCount = TextBuf - Buffer - 1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);                                
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);                                
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);                                
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);                                
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);                                        
        CharCount = TextBuf - Buffer -1 ;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);                                        
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);                                        
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL)
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);                                        
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);                                        
        TextBuf--;
        while ((*TextBuf++ = *str9++) != 0);                                        
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);                        
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);                                
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);                                        
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);                                        
        TextBuf--;
        while ((*TextBuf++ = *str9++) != 0);                                        
        TextBuf--;
        while ((*TextBuf++ = *str10++) != 0);                                                
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL )
    {        
        UInt16 CharCount;
        register char* TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str9++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str10++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str11++) != 0); 
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;    
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL )
    {        
        UInt16 CharCount;
        register char* TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str9++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str10++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str11++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str12++) != 0); 
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;    
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL )
    {        
        UInt16 CharCount;
        register char* TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str9++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str10++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str11++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str12++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str13++) != 0); 
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;    
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL && str14 != NULL )
    {        
        UInt16 CharCount;
        register char* TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str9++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str10++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str11++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str12++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str13++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str14++) != 0); 
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;    
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14,  const char* str15 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL && str14 != NULL && str15 != NULL )
    {        
        UInt16 CharCount;
        register char* TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str9++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str10++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str11++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str12++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str13++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str14++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str15++) != 0); 
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;    
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14,  const char* str15,  const char* str16 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL && str14 != NULL && str15 != NULL && str16 != NULL )
    {        
        UInt16 CharCount;
        register char* TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str9++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str10++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str11++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str12++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str13++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str14++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str15++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str16++) != 0); 
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;    
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14,  const char* str15,  const char* str16,  const char* str17 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL && str14 != NULL && str15 != NULL && str16 != NULL && str17 != NULL )
    {        
        UInt16 CharCount;
        register char* TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str9++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str10++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str11++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str12++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str13++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str14++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str15++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str16++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str17++) != 0); 
        CharCount = TextBuf - Buffer -1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;    
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14,  const char* str15,  const char* str16,  const char* str17,  const char* str18 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL && str14 != NULL && str15 != NULL && str16 != NULL && str17 != NULL && str18 != NULL )
    {        
        UInt16 CharCount;
        register char* TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str9++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str10++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str11++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str12++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str13++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str14++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str15++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str16++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str17++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str18++) != 0); 
        CharCount = TextBuf - Buffer - 1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;    
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14,  const char* str15,  const char* str16,  const char* str17,  const char* str18,  const char* str19 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if (FBuffer == NULL && FStdOut == FALSE) return 0;
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL && str14 != NULL && str15 != NULL && str16 != NULL && str17 != NULL && str18 != NULL && str19 != NULL )
    {        
        UInt16 CharCount;
        register char* TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ((*TextBuf++ = *str++ ) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str1++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str2++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str3++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str4++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str5++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str6++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str7++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str8++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str9++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str10++) != 0);
        TextBuf--;
        while ((*TextBuf++ = *str11++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str12++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str13++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str14++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str15++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str16++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str17++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str18++) != 0); 
        TextBuf--;
        while ((*TextBuf++ = *str19++) != 0); 
        CharCount = TextBuf - Buffer - 1;
        WriteString( (const char*)Buffer, CharCount );
        if( FStdOut == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;    
}
//------------------------------------------------------------------------------
Int32 BufferedLog::AddPrefix( const UFC::AnsiString& Perfix )
{
    if( BufferedLogData::FIsCreated == TRUE )
    {
        PLockObject Lock( BufferedLogData::FBufferCS );

        ///< Try to find an existing prefix string.
        for( register int i = 0; i < BufferedLogData::FPreFixList.ItemCount(); i ++ )
        {
                if( Perfix == BufferedLogData::FPreFixList.GetItem( i ) )
                        return i;
        }
        ///< Not found, add a new one.
        BufferedLogData::FPreFixList.Add( Perfix );
        return BufferedLogData::FPreFixList.ItemCount() - 1;
    }
    return 0;
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::Printf( const char* FormatStr, ... )
{
    if( BufferedLogData::FIsCreated == TRUE )
    {
        PLockObject Lock( BufferedLogData::FBufferCS );
        Int32       CharCount,Offset;
        va_list     va;

        va_start( va , FormatStr );
    #ifndef _WIN32 ///< For Linux,UNIX.
        va_list     vacpy;
        va_copy( vacpy, va );
        CharCount = vsnprintf( NULL, 0, FormatStr, vacpy );
        va_end( vacpy );
    #else
        CharCount = vsnprintf( NULL, 0, FormatStr, va );
    #endif
        Offset = PrepareBuffer( CharCount );
        CharCount = vsnprintf( BufferedLogData::FSTRBuffer + Offset, BufferedLogData::FBufferSize - Offset, FormatStr, va );
        va_end( va );

        if( CharCount <= 0 )
            throw( UFC::Exception( " vsnprintf failed" ));
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)BufferedLogData::FSTRBuffer,(UFCType::Int32)strlen( BufferedLogData::FSTRBuffer ) );
    #ifndef _WIN32
        if( BufferedLogData::FToStdout == TRUE )
            printf( "%s\n", BufferedLogData::FSTRBuffer );
    #endif
        return CharCount;
    }
    else
    {
    #ifndef _WIN32
        va_list     va;

        va_start( va , FormatStr );
        vprintf( FormatStr, va );
        va_end( va );
    #endif
        return 0;
    }
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::DebugPrintf( const char* FormatStr, ... )
{
	if( BufferedLogData::FDebugLevel == 0 )
		return 0;
	if( BufferedLogData::FIsCreated == TRUE )
	{
		PLockObject Lock( BufferedLogData::FBufferCS );
		Int32       CharCount,Offset;
		va_list     va;

		va_start( va , FormatStr );
	#ifndef _WIN32 ///< For Linux,UNIX.
		va_list     vacpy;
		va_copy( vacpy, va );
		CharCount = vsnprintf( NULL, 0, FormatStr, vacpy );
		va_end( vacpy );
	#else
		CharCount = vsnprintf( NULL, 0, FormatStr, va );
	#endif
		Offset = PrepareBuffer( CharCount );
		CharCount = vsnprintf( BufferedLogData::FSTRBuffer + Offset, BufferedLogData::FBufferSize - Offset , FormatStr, va );
		va_end( va );
		if( CharCount <= 0 )
			throw( UFC::Exception( " vsnprintf failed" ));
		if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
			BufferedLogData::FLogObject->WriteString( (const char*)BufferedLogData::FSTRBuffer, (UFCType::Int32)strlen( BufferedLogData::FSTRBuffer ) );
	#ifndef _WIN32
		if( BufferedLogData::FToStdout == TRUE )
			printf( "%s\n", BufferedLogData::FSTRBuffer );
	#endif
		return CharCount;
	}
	else
	{
	#ifndef _WIN32
		va_list     va;

		va_start( va , FormatStr );
		vprintf( FormatStr, va );
		va_end( va );
	#endif
		return 0;
	}
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::DebugPrintf( Int32 Level, const char* FormatStr, ... )
{
	if( (BufferedLogData::FDebugLevel & Level ) == 0 )///< Not contains this level ?
		return 0;
	if( BufferedLogData::FIsCreated == TRUE )
	{
		PLockObject Lock( BufferedLogData::FBufferCS );
		Int32       CharCount,Offset;
		va_list     va;

		va_start( va , FormatStr );
	#ifndef _WIN32 ///< For Linux,UNIX.
		va_list vacpy;
		va_copy( vacpy, va );
		CharCount = vsnprintf( NULL, 0, FormatStr, vacpy );
		va_end( vacpy );
	#else
		CharCount = vsnprintf( NULL, 0, FormatStr, va );
	#endif
		Offset = PrepareBuffer( CharCount );
		CharCount = vsnprintf( BufferedLogData::FSTRBuffer + Offset, BufferedLogData::FBufferSize - Offset, FormatStr, va );
		va_end( va );
		if( CharCount <= 0 )
			throw( UFC::Exception( " vsnprintf failed" ));
		if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
			BufferedLogData::FLogObject->WriteString( (const char*)BufferedLogData::FSTRBuffer, (UFCType::Int32)strlen( BufferedLogData::FSTRBuffer ) );
		if( BufferedLogData::FToStdout == TRUE )
			printf( "%s\n", BufferedLogData::FSTRBuffer );
		return CharCount;
	}
	else
	{
	#ifndef _WIN32
		va_list     va;

		va_start( va , FormatStr );
		vprintf( FormatStr, va );
		va_end( va );
	#endif
		return 0;
	}
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::DebugPrintf( Int32 Level, Int32 PrefixIndex, const char* FormatStr, ... )
{
	if( (BufferedLogData::FDebugLevel & Level ) == 0 )///< Not contains this level ?
		return 0;
	if( BufferedLogData::FIsCreated == TRUE )
	{
		PLockObject Lock( BufferedLogData::FBufferCS );
		Int32       CharCount,Offset;
		va_list     va;

		va_start( va , FormatStr );
	#ifndef _WIN32 //__LINUX
		va_list vacpy;
		va_copy( vacpy, va );
		CharCount = vsnprintf( NULL, 0, FormatStr, vacpy );
		va_end( vacpy );
	#else
		CharCount = vsnprintf( NULL, 0, FormatStr, va );
	#endif
		Offset = PrepareBuffer( CharCount, PrefixIndex );
		CharCount = vsnprintf( BufferedLogData::FSTRBuffer + Offset, BufferedLogData::FBufferSize - Offset, FormatStr, va );
		va_end( va );
		if( CharCount <= 0 )
			throw( UFC::Exception( " vsnprintf failed" ));
		if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
			BufferedLogData::FLogObject->WriteString( (const char*)BufferedLogData::FSTRBuffer, (UFCType::Int32)strlen( BufferedLogData::FSTRBuffer ) );
		if( BufferedLogData::FToStdout == TRUE )
			printf( "%s\n", BufferedLogData::FSTRBuffer );
		return CharCount;
	}
	else
	{
	#ifndef _WIN32
		va_list     va;

		va_start( va , FormatStr );
		vprintf( FormatStr, va );
		va_end( va );
	#endif
		return 0;
	}
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL )
    {
        UInt16 CharCount;
        char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );        
        while ( ( *TextBuf++ = *str++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1 )
{
    char Buffer[ BUF_SIZE + 1 ];        
    
    if( str != NULL && str1 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );        
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2 )
{
    char Buffer[ BUF_SIZE + 1 ];        
    
    if( str != NULL && str1 != NULL && str2 != NULL)
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//---------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str8++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL)
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str8++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str9++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str8++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str9++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str10++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str8++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str9++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str10++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str11++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str8++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str9++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str10++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str11++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str12++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str8++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str9++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str10++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str11++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str12++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str13++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13, const char* str14 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL && str14 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str8++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str9++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str10++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str11++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str12++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str13++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str14++ ) != 0) ;
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13, const char* str14, const char* str15 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL && str14 != NULL && str15 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( (* TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str8++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str9++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str10++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str11++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str12++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str13++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str14++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str15++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13, const char* str14, const char* str15, const char* str16 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL && str14 != NULL && str15 != NULL && str16 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str8++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str9++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str10++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str11++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str12++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str13++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str14++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str15++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str16++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13, const char* str14, const char* str15, const char* str16, const char* str17 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL && str14 != NULL && str15 != NULL && str16 != NULL && str17 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str8++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str9++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str10++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str11++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str12++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str13++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str14++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str15++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str16++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str17++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//------------------------------------------------------------------------------
UInt16 BufferedLog::print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13, const char* str14, const char* str15, const char* str16, const char* str17, const char* str18 )
{
    char Buffer[ BUF_SIZE + 1 ];            
    
    if( str != NULL && str1 != NULL && str2 != NULL && str3 != NULL && str4 != NULL && str5 != NULL && str6 != NULL && str7 != NULL && str8 != NULL && str9 != NULL && str10 != NULL && str11 != NULL && str12 != NULL && str13 != NULL && str14 != NULL && str15 != NULL && str16 != NULL && str17 != NULL && str18 != NULL )
    {        
        UInt16 CharCount;
        register char*  TextBuf = Buffer + BufferedLogData::FTimeStrLen;
        
        GetTimestamp( Buffer );
        while ( ( *TextBuf++ = *str++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str1++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str2++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str3++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str4++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str5++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str6++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str7++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str8++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str9++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str10++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str11++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str12++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str13++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str14++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str15++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str16++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str17++ ) != 0 );
        TextBuf--;
        while ( ( *TextBuf++ = *str18++ ) != 0 );
        CharCount = TextBuf - Buffer - 1;
        if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
            BufferedLogData::FLogObject->WriteString( (const char*)Buffer, CharCount );
        if( BufferedLogData::FToStdout == TRUE )
            puts( Buffer );
        return CharCount;
    }
    return 0;
}
//---------------------------------------------------------------------------
void BufferedLog::GenerateCoreDump()
{
    // * for generate coredump file
    int a1 = 3;
    int b1 = 0;
    double c1 = a1 / b1;
    Printf(" %d / %d = %10.3lf", a1, b1, c1);

    int ii = 1000;
    AnsiString tmpStr = "1234";
    Printf(" %s %s", ii, tmpStr.c_str());

    char* p = NULL;
    strcpy(p, "copy string to null pointer and cause crash");
    
}
//---------------------------------------------------------------------------
BOOL BufferedLog::DebugLevelExist( Int32 Level )
{
    if( (BufferedLogData::FDebugLevel & Level ) == 0 )///< Not contains this level ?
        return FALSE;
    return TRUE;
}
//---------------------------------------------------------------------------
void BufferedLog::SetPrintToStdout( BOOL OnOff )
{
    BufferedLogData::FToStdout = OnOff;
}
//---------------------------------------------------------------------------
void BufferedLog::SetDebugMode( Int32 Level )
{
    BufferedLogData::FDebugLevel = Level;
}
//---------------------------------------------------------------------------
LogObject* BufferedLog::SetLogObject( LogObject* logObj )
{
    LogObject* OldLogObj = BufferedLogData::FLogObject;
    BufferedLogData::FLogObject = logObj;
    return OldLogObj;
}
//---------------------------------------------------------------------------
void BufferedLog::FlushToFile( )
{
    if( BufferedLogData::FLogObject != NULL ) ///< Log object exists.
        BufferedLogData::FLogObject->Flush( TRUE );
}
//---------------------------------------------------------------------------
void BufferedLog::EnableLog_us( BOOL Enable )
{
    if( Enable == TRUE)
    {
        BufferedLogData::FIsLog_us   = TRUE;
        BufferedLogData::FTimeStrLen = 15; ///< "HH:MM:SS.ssssss" length =15
    }
    else
    {
        BufferedLogData::FIsLog_us   = FALSE;
        BufferedLogData::FTimeStrLen = 12; ///< "HH:MM:SS.sss" length =12
    }
}
//---------------------------------------------------------------------------
void BufferedLog::FlushAfterWrite( BOOL flush )
{
    BufferedLogData::FFlushAfterWrite = flush;
}
//---------------------------------------------------------------------------
BOOL BufferedLog::IsEnableLog_us( void )
{
    return BufferedLogData::FIsLog_us;
}
//---------------------------------------------------------------------------
//
// Class Buffer
// 
//---------------------------------------------------------------------------
Buffer::Buffer(Int32 PSize)
:FPtr(NULL)
,FSize(PSize)
,FPos(0)
{
    if( FSize  < 64 )
        FSize = 64;    
    FPtr = new char[FSize];    
    memset( FPtr, 0, FSize) ;
}
//---------------------------------------------------------------------------
Buffer::~Buffer()
{
    if (FPtr) delete FPtr;
}
//---------------------------------------------------------------------------
BOOL Buffer::IsOverflow( Int32 Len )
{
    return (FPos + Len > FSize) ;
}
//---------------------------------------------------------------------------
Int32  Buffer::GetAvailableSize() 
{ 
    return (FSize - FPos);  
}
//---------------------------------------------------------------------------
void Buffer::Write( const char* Data, Int32 Len, BOOL AppendLinefeed )
{
    memcpy( FPtr + FPos, Data, Len);
    FPos += Len;
    if (AppendLinefeed)
    {
        FPtr[FPos] = '\n';
        FPos ++;
    }
}
//---------------------------------------------------------------------------
void Buffer::Clear( )
{
    FPos = 0;
}
//---------------------------------------------------------------------------
//
// Class CachedBuffer
// 
//---------------------------------------------------------------------------
CachedBuffer::CachedBuffer( FileStream64* FileStream, Int32 BufferSize, BOOL Cache )
: PThread( NULL , FALSE)
, FFileStream( FileStream )
, FBufferSize( (Cache && BufferSize < MIN_BUFFERSIZE) ? MIN_BUFFERSIZE : BufferSize )
, FLevel(      (Cache) ? 3 : 0 )
, FClean(      (Cache) ? MAX_CacheLevel : 0 )
, FDirty(      (Cache) ? MAX_CacheLevel : 0 )
, FBuffer(NULL)
, FIntervalMS( 1000 )
, FWriteBlockSize( FileStream_DefaultBufferSize )
, FSlowdown(TRUE)
{   
    Initial();
    if (FLevel > 0 )
        Start();
}
//---------------------------------------------------------------------------
CachedBuffer::~CachedBuffer( )
{
    FSlowdown = FALSE;
    if (! IsTerminated())
    {
        Terminate();    
        FDirtyEvent.SetEvent();
        WaitFor(10);     
    }
    Flush( TRUE );  
    FClean.Clear(TRUE);
    FDirty.Clear(TRUE);
    if (FBuffer) 
    {
        delete FBuffer;        
        FBuffer = NULL;
    }
}
//---------------------------------------------------------------------------
void CachedBuffer::Initial()
{
    Buffer*  Clean = NULL;
    if (FLevel > 0)
    {
        for (int i=0 ;i < FLevel ; i++)
        {
            Clean = new Buffer(FBufferSize);
            MoveToCleanCache(Clean);
        }
        GetCleanCache(FBuffer);
    }
    else
    {
        FBuffer = new Buffer(FBufferSize);
    }
}
//---------------------------------------------------------------------------
void CachedBuffer::Execute( void )
{
    while ( ! IsTerminated() )
    {
        try
        {         
            if (FDirtyEvent.WaitFor( FIntervalMS/1000, FIntervalMS%1000)) ///< busy
            {
                if ( IsTerminated()==FALSE && FSlowdown)
                    FDirtyEvent.WaitFor( 0, 500 ) ;
            }
            else ///< is idle ?
            {
                FlushToCache();
            }
            CacheToFile( IsTerminated() );
        }
        catch(Exception& ex)
        {
            printf(" CachedBuffer::Execute() Exception : %s", ex.what());
        }
    }
    CacheToFile( TRUE );
}
//---------------------------------------------------------------------------
void CachedBuffer::Write( const char* Data, Int32 Len, BOOL AppendLinefeed, BOOL FlushAfterWrite)
{
    PLockObject Lock( FCS );          
    if (Len <= 0) return ;
    if (FBuffer == NULL)
    {
        throw Exception( " CachedBuffer::WriteToBuffer() CachedBuffer no Buffer" );
    }     
    Int32 NeedLen = Len + ((AppendLinefeed) ? 1 : 0) ;
    if (FLevel <=0)
    {        
        if (FBuffer->IsOverflow( NeedLen ))  ///< Buffer not enough, Directly write file and clear buffer
        {
            WriteToFile( FBuffer->GetBuffer(), FBuffer->GetPos(), FALSE, TRUE );
            FBuffer->Clear() ;
            if (FBuffer->IsOverflow( NeedLen ))  ///< Buffer not enough, Directly write file
            {
                WriteToFile( Data, Len, AppendLinefeed, FlushAfterWrite );
                return ;
            }
        }
        FBuffer->Write(Data , Len, AppendLinefeed);    
        if (FBuffer->IsOverflow(1) || FlushAfterWrite)  ///< Full ?
        {
            WriteToFile( FBuffer->GetBuffer(), FBuffer->GetPos(), FALSE, FlushAfterWrite );
            FBuffer->Clear() ;
        }
    }
    else
    {
        if (FBuffer->IsOverflow( NeedLen ))       ///< Buffer is not enough
        {
            FlushToCache();
            if (FBuffer->IsOverflow( NeedLen ))   ///< Buffer size < Len
            {
                Buffer* LastOne = FBuffer ;
                FBuffer = new Buffer( NeedLen + 4096 );
                delete LastOne;
            }
        }
        FBuffer->Write(Data , Len, AppendLinefeed);    
        if (FBuffer->IsOverflow(1))  ///< Full ?
        {
            FlushToCache();
        }        
        if (FlushAfterWrite)
            Flush( );            
    }
}
//---------------------------------------------------------------------------
void CachedBuffer::WriteToFile( const char* Data, Int32 Len, BOOL AppendLinefeed, BOOL FlushAfterWrite )
{
    PLockObject Lock( FIOCS );
    if ( Data==NULL || Len <= 0 ) return ;
    FFileStream->Write( Data, Len );
    if (AppendLinefeed)
    {
        FFileStream->Write( "\n", 1 ); 
    }
    if (FlushAfterWrite)
    {
        FFileStream->Flush();
    }
}
//---------------------------------------------------------------------------
void CachedBuffer::FlushToCache()
{
    PLockObject Lock( FCS );
    if (FBuffer==NULL || FBuffer->GetPos() <= 0) return;   
    MoveToDirtyCache(FBuffer);
    FBuffer = NULL; 
    GetCleanCache(FBuffer);
    return ;
}
//---------------------------------------------------------------------------
// return TRUE : means writing to disk
//---------------------------------------------------------------------------
BOOL CachedBuffer::CacheToFile( BOOL IsBLock )
{   
    static   BOOL IsFlushing = FALSE;
    Int32    WriteQueueCount = 0;
    Buffer*  Dirty           = NULL;             
    
    if (FLevel <= 0) return FALSE;
    if (IsBLock==FALSE && IsFlushing)
    {
        return IsFlushing;
    }
    
    //------------------------------------------------
    PLockObject Lock( FIOCS );
    if (FDirty.ItemCount() <= 0) return FALSE;    
    
    IsFlushing = TRUE;
    while ( (Dirty=FDirty.DequeueNoWait()) != NULL )
    {
        try
        {
            WriteToFile( Dirty->GetBuffer(), Dirty->GetPos(), FALSE, IsBLock ) ;
            MoveToCleanCache( Dirty ) ;
            WriteQueueCount++;
        }
        catch(Exception &ex)
        {
            printf(" CachedBuffer::CacheToFile() Exception : %s", ex.what());
        }        
        if (FDirty.ItemCount() > 0 && IsBLock==FALSE && FSlowdown) ///< avoid io burst
        {
            SleepMS(1);                  ///< avoid io burst
        }
    } //end while      
    FFileStream->Flush();
    FSlowdown  = TRUE;
    IsFlushing = FALSE;    
    return IsFlushing;
}
//---------------------------------------------------------------------------
void CachedBuffer::Flush( BOOL IsBlock )
{
     FSlowdown = FALSE;    
     if (FLevel <= 0)
     {         
         PLockObject Lock( FCS );
         WriteToFile( FBuffer->GetBuffer(), FBuffer->GetPos(), FALSE, TRUE );
         FBuffer->Clear() ;
     }
     else if (IsBlock) 
     {
         FlushToCache(); 
         CacheToFile( TRUE ) ;
     }
}
//---------------------------------------------------------------------------
void CachedBuffer::GetCleanCache( Buffer*& Clean )
{
    Clean = FClean.DequeueNoWait() ;
    if (Clean != NULL) return ;    
    if (GetCacheLevel() < FClean.Size())   ///< Quantity of created Cache < MAX_CacheLevel ?
    {
        Clean = new Buffer(FBufferSize);
    }
    else ///< cannot create a new cache, waiting ...
    {
        Int32  TryCount=0;        
        FSlowdown = FALSE;
        printf(" WARNING: CachedBuffer::GetCleanCache() cache is not enough and waitting for clean (Clean:Dirty:Buffer=%d:%d:%d Size=%d)\n", FClean.ItemCount(), FDirty.ItemCount(), (FBuffer==NULL) ? 1 : 0 , FClean.Size() );
        while ((Clean = FClean.Dequeue(1))==NULL)
        {
           FSlowdown = FALSE;
           TryCount++; 
           printf(" WARNING: CachedBuffer::GetCleanCache() cache is not enough and waitting timeout (Clean:Dirty:Buffer=%d:%d:%d Size=%d) TryCount=%d\n", FClean.ItemCount(), FDirty.ItemCount(), (FBuffer==NULL) ? 1 : 0 , FClean.Size(), TryCount);
        }
    }
    return ;    
}
//---------------------------------------------------------------------------
void CachedBuffer::MoveToCleanCache( Buffer* Clean )
{
    Clean->Clear();
    if (FClean.Inqueue(Clean, 0) == FALSE)
    {
        AnsiString Msg;
        Msg.Printf(" CachedBuffer::MoveToCleanCache() Clean=%d/%d Inqueue fail", FClean.ItemCount(), FClean.Size());
        printf("%s\n", Msg.c_str()); fflush(stdout); 
        throw Exception( Msg );
    }    
}
//---------------------------------------------------------------------------
void CachedBuffer::MoveToDirtyCache( Buffer* Dirty )
{
    if (FDirty.Inqueue(Dirty, 0) == FALSE)    
    {
        AnsiString Msg;
        
        Msg.Printf(" CachedBuffer::MoveToDirtyCache() Dirty=%d/%d Inqueue fail", FDirty.ItemCount(), FDirty.Size());
        printf("%s\n", Msg.c_str()); fflush(stdout); 
        FDirtyEvent.SetEvent();        
        throw Exception( Msg );
    }
    FDirtyEvent.SetEvent();    
}
//---------------------------------------------------------------------------
void PrintObjectField( const AnsiString& FieldName, const AnsiString& FieldValue )
{
    if( FieldValue.Length() > 0 ) 
        BufferedLog::print( " ", FieldName.c_str(), "=\"", FieldValue.c_str(), "\"" );
    else
        BufferedLog::print( " ", FieldName.c_str(), "=\"\"" );
}
//---------------------------------------------------------------------------
void PrintObjectField( const AnsiString& FieldName, Int32 FieldValue )
{
    BufferedLog::print( " ", FieldName.c_str(), "=", UFC::IntToStr(FieldValue) );
}
//---------------------------------------------------------------------------
void PrintObjectField( const AnsiString& FieldName, Int64 FieldValue )
{
    BufferedLog::Printf( " %s=%lld", FieldName.c_str(), FieldValue );
}
//---------------------------------------------------------------------------
void PrintObjectField( const AnsiString& FieldName, bool FieldValue )
{
    if (FieldValue)
        BufferedLog::print( " ", FieldName.c_str(), "=true" );
    else
        BufferedLog::print( " ", FieldName.c_str(), "=false" );
}
//---------------------------------------------------------------------------
void PrintObjectField( const AnsiString& FieldName, char FieldValue )
{
    if( ( FieldValue >= 32 ) && ( FieldValue <= 126 ) )
        BufferedLog::Printf( " %s='%c'", FieldName.c_str(), FieldValue );
    else
        BufferedLog::Printf( " %s=%d", FieldName.c_str(), FieldValue );
}
//---------------------------------------------------------------------------
void PrintObjectField( const AnsiString& FieldName, double FieldValue, int IntWidth, int Precision,  bool PadZero )
{
    BufferedLog::print( " ", FieldName.c_str(), "=", UFC::DoubleToStr(FieldValue, IntWidth, Precision, PadZero) );
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------

