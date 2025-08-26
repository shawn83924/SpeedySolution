/*
 * File:   BufferedLog.h
 * Author: yuan
 *
 * BufferSize : 100KB is best performance : 
 *              min/avg/max=0.184/0.267/68.123 us
 * 
 * Created on July 13, 2009, 11:53 PM
 */

#ifndef _BUFFEREDLOG_H
#define	_BUFFEREDLOG_H
//------------------------------------------------------------------------------
#include "PThread.h"
#include "PQueue.h"
#include "FileStream.h"
#include "PInt32.h"
#include "PPtrQueue.h"
//------------------------------------------------------------------------------
namespace UFC
{
//------------------------------------------------------------------------------
const Int32 dlFatalError  = 1;
const Int32 dlWarning     = 2;
const Int32 dlInformation = 4;
const Int32 dlPerformance = 8;
const Int32 dlOther       = 16;
const Int32 dlUser1       = 32;
const Int32 dlUser2       = 64;
const Int32 dlUser3       = 128;
const Int32 dlUser4       = 256;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void AddToLogCleanerCfg( const AnsiString& SectionName, 
                                const AnsiString& RefDir, 
                                const AnsiString& Prefix, 
                                const AnsiString& Ext,                           
                                BOOL  TW_Year );
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void GenerateLogFileName( AnsiString& FileName, 
                                 const AnsiString& RefDir, 
                                 const AnsiString& Prefix, 
                                 const AnsiString& Ext,                           
                                 BOOL  TW_Year );



// ---------------------------------------------------------------------------------
class Buffer
{
private:
    char*        FPtr;   
    Int32        FSize;
    Int32        FPos;
public:
    Buffer(Int32 Size);
    ~Buffer();
    
public:    
    char*  GetBuffer()  { return FPtr ; }
    Int32  Size()       { return FSize; }
    Int32  GetPos()     { return FPos;  }
    BOOL   IsOverflow(Int32 NewDataLen);    
    Int32  GetAvailableSize() ;
    void   Write( const char* Data, Int32 Len, BOOL AppendLinefeed = FALSE );
    void   Clear();    
};


// CleanStore => Buffer => DirtyStore 
// DirtyStore => File   => CleanStore
// ---------------------------------------------------------------------------------
class CachedBuffer : public PThread
{
private:    
    FileStream64*          FFileStream;
    Int32                  FBufferSize;
    Int32                  FLevel;           ///< how many blocks of Buffer
    
private:    
    PCriticalSection       FCS;
    PCriticalSection       FIOCS;
    PEvent                 FDirtyEvent;
    PtrQueue<Buffer>       FClean;           ///< CleanStore
    PtrQueue<Buffer>       FDirty;           ///< DirtyStore 
    Buffer*                FBuffer;
    Int32                  FIntervalMS;
    Int32                  FWriteBlockSize;  ///< size for earch writing file 
    BOOL                   FSlowdown;        ///< avoid to writing burst.
    
public:    
    CachedBuffer(FileStream64* FileStream, Int32 BufferSize, BOOL Cache);
    ~CachedBuffer() ;
private:    
    void        Initial();    
    void        FlushToCache( );
    BOOL        CacheToFile( BOOL IsBLock );            
    void        WriteToFile( const char* Data, Int32 Size, BOOL AppendLinefeed , BOOL FlushAfterWrite );
    
    void        GetCleanCache(     Buffer*& Clean  );
    void        MoveToCleanCache(  Buffer*  Clean  );
    void        MoveToDirtyCache(  Buffer*  Dirty  );
    Int32       GetCacheLevel() { return FClean.ItemCount() + FDirty.ItemCount() + ((FBuffer==NULL) ? 0 : 1) ; }   ///< quantity of Cache
    
private:    
    void        Execute( void );       
    
public:    
    void        Write( const char* Data, Int32 Len, BOOL AppendLinefeed = FALSE , BOOL FlushAfterWrite = FALSE ); 
    void        Flush( BOOL IsBlock = FALSE );    
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MemoryWriter
{
private:
	char* FPrt;
	int   FPos;
public:
	MemoryWriter( char* Memory ):FPrt(Memory),FPos(0){}
	void Write( void* Data, int Len )
	{
            memcpy( FPrt + FPos, Data, Len);
            FPos += Len;
	}
	void AppendZero( void )
	{
            AppendChar( 0 );
	}
	void AppendChar( char ch )
	{
            *( FPrt + FPos ) = ch;
            FPos ++;
	}
	int GetPosition(){ return FPos; }
};
//------------------------------------------------------------------------------
class ObjToLog
{
public:
    virtual ~ObjToLog( void ) {}
    virtual int ToString( char* Buffer, int MaxSize ) = 0;    
};
//------------------------------------------------------------------------------
class StringToLog: public ObjToLog
{
private:
    int   FLen;
    char* FBuffer;
public:
    StringToLog( const char* Str, int len )
    :FLen( len )
    {
        FBuffer = new char[ (size_t)FLen + 1 ];
        memcpy( FBuffer, Str, FLen );
        FBuffer[ FLen ] = 0;
    }
    virtual ~StringToLog( void ) { delete [] FBuffer; }
    virtual int ToString( char* Buffer, int MaxSize ) { return UFC::StrLCpy( Buffer, FBuffer, MaxSize ); }
};
//------------------------------------------------------------------------------
class LogObject
{
public:
    virtual ~LogObject( void ) {}
    virtual void   Flush( BOOL IsBlock = FALSE ) = 0;
    virtual void   WriteString( const AnsiString& LogString ) = 0;
    virtual void   WriteString( const char* LogString ) = 0;
    virtual void   WriteString( const char* LogString, Int32 StrLength ) = 0;
    virtual void   Write( const AnsiString& LogString ) = 0;
    virtual void   Write( const char* Log, Int32 Len )  = 0;    
    virtual void   Write( ObjToLog* obj )  = 0;        
};
//------------------------------------------------------------------------------
const int MAX_SIZE_PER_WRITE = 248;
//------------------------------------------------------------------------------
class BufferedLogData
{
public:
	static char*            FSTRBuffer;
	static Int32            FBufferSize;
	static Int32            FTimeStrLen;
	static Int32            FDebugLevel;
	static LogObject*       FLogObject;
	static BOOL             FIsLog_us;
	static BOOL             FToStdout;
	static BOOL             FFlushAfterWrite;
	static BOOL             FIsCreated;
	static PCriticalSection FBufferCS;
	static PStringList      FPreFixList;
public:
	BufferedLogData();
	~BufferedLogData();
};
//------------------------------------------------------------------------------
class BufferedLog : public LogObject, public UFC::PThread
{
private:
    PCriticalSection   FCS;
    FileStream64*      FFileStream;
    CachedBuffer*      FBuffer;          
    BOOL               FStdOut;
    BOOL               FUseThread;
    UFC::LFQueue<ObjToLog> FLogObjQ;
    AnsiString             FFileName;
public: 
    BufferedLog( AnsiString FileName, int BufferLength = 10240, BOOL Append = FALSE, BOOL Cache = TRUE ); 
    BufferedLog( const AnsiString& SectionName,
                 const AnsiString& Dir,
                 const AnsiString& Prefix, 
                 const AnsiString& Ext, 
                 int   BufferLength = 10240, 
                 BOOL  Append = FALSE,
                 BOOL  TW_Year = FALSE,
                 BOOL  Cache = TRUE,
                 BOOL  Thread = FALSE );
    BufferedLog( BOOL IsStdOut = TRUE ); 
    const AnsiString& GetFileName( void ) { return FFileName; }
    virtual ~BufferedLog();
public:
    virtual void Execute( void );
public:
    virtual void Flush( BOOL IsBlock = FALSE );
    virtual void WriteString( const AnsiString& LogString );
    virtual void WriteString( const char* LogString );
    virtual void WriteString( const char* LogString, Int32 Len );
    virtual void Write( const AnsiString& LogString );
    virtual void Write( const char* Log, Int32 Length ) ;    
    virtual void Write( ObjToLog* obj );            
public:    
    void   SetToStdout( BOOL OnOff ) { FStdOut = OnOff; }
    UInt16 fprintf( const char* FormatStr, ... );
    UInt16 fprint( const char* str );
    UInt16 fprint( const char* str, const char* str1 );
    UInt16 fprint( const char* str, const char* str1, const char* str2 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14,  const char* str15 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14,  const char* str15,  const char* str16 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14,  const char* str15,  const char* str16,  const char* str17 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14,  const char* str15,  const char* str16,  const char* str17,  const char* str18 );
    UInt16 fprint( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9,  const char* str10,  const char* str11,  const char* str12,  const char* str13,  const char* str14,  const char* str15,  const char* str16,  const char* str17,  const char* str18,  const char* str19 );
private:
    static Int32 PrepareBuffer( Int32 NeedSize, Int32 PrefixIndex = -1 );
    static inline void  GetTimestamp( char* Buffer );
public:
    /*
     *  Print log to screen?
     *  @Param OnOff: TRUE for print log to screen and save to log, FALSE for save to file only.
     *
     */
    static void         SetPrintToStdout( BOOL OnOff );
    /*
     *  Set the debug log level.
     *  @Param Level: Level to log.( Default Level = 0, turn off debug log)
     *
     */
    static void         SetDebugMode( Int32 Level );
    /*
     *  Add the prefix string for a log.
     *  @Param Prefix: Prefix string.
     *  @return Int32: Index of this prefix.
     *
     */
    static Int32        AddPrefix( const UFC::AnsiString& Perfix );
    /*
     *  Set the global LogObject to save log.
     *  @Param logObj: Object inherited interface LogObject.
     *
     */
    static LogObject*   SetLogObject( LogObject* logObj );
    /*
     * Flush logs in buffer to file. ( call the global LogObject Flush function )
     */
    static void         FlushToFile( );
    /*
     * Print formats string.
     * To stdio, global log object or both, ignore debug level.
     *
     */
    static UInt16       Printf( const char* FormatStr, ... );
    /*
     * Print formats string.
     * To stdio, global log object or both, debug level should > 0.
     *
     */
    static UInt16       DebugPrintf( const char* FormatStr, ... );
    /*
     * Print formats string.
     * To stdio, global log object or both, debug level should match.
     * @Param Level: debug level of the output string.
     *
     */
    static UInt16       DebugPrintf( Int32 Level, const char* FormatStr, ... );
    /*
     * Print formats string.
     * To stdio, global log object or both, debug level should match.
     * @Param Level: debug level of the output string.
     * @Param PerfixIndex: The index of the prefix string.
     *
     */
    static UInt16       DebugPrintf( Int32 Level, Int32 PerfixIndex, const char* FormatStr, ... );
    /*
     * Check the debug level existence.
     * @Param Level: The debug level which want to check.
     * @return BOOL: TRUE for exist.
     *
     */
    static BOOL  DebugLevelExist( Int32 Level );
    static void  EnableLog_us( BOOL Enable );
    static BOOL  IsEnableLog_us( void );
    static void	 FlushAfterWrite( BOOL flush );
    static UInt16 print( const char* str );
    static UInt16 print( const char* str, const char* str1 );
    static UInt16 print( const char* str, const char* str1, const char* str2 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13, const char* str14 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13, const char* str14, const char* str15 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13, const char* str14, const char* str15, const char* str16 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13, const char* str14, const char* str15, const char* str16, const char* str17 );
    static UInt16 print( const char* str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8, const char* str9, const char* str10, const char* str11, const char* str12, const char* str13, const char* str14, const char* str15, const char* str16, const char* str17, const char* str18 );
    static void GenerateCoreDump();
};
//------------------------------------------------------------------------------
extern BufferedLogData GLogData;
//------------------------------------------------------------------------------
void PrintObjectField( const AnsiString& FieldName, const AnsiString& FieldValue );
void PrintObjectField( const AnsiString& FieldName, Int32 FieldValue );
void PrintObjectField( const AnsiString& FieldName, Int64 FieldValue );
void PrintObjectField( const AnsiString& FieldName, bool FieldValue );
void PrintObjectField( const AnsiString& FieldName, char FieldValue );
void PrintObjectField( const AnsiString& FieldName, double FieldValue, int IntWidth = 23, int Precision = 9,  bool PadZero = false );
} ///< End of namespace UFC
//------------------------------------------------------------------------------
#endif	/* _BUFFEREDLOG_H */

