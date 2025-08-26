/**@file Utility.h
*  @brief Header file for Utility.cpp
*  @author Simon Chang
*
*/
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef __ExchangeUtility_UTILITY_H
#define __ExchangeUtility_UTILITY_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef _WIN32
	#include <sys/time.h>
	#include <sys/timeb.h>
#endif
#include "../UFC/UFC.h"
#include "../Migo/MTree.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TFXException.h"        
#include "../RiskManager/RMInterface.h"
//------------------------------------------------------------------------------
const int TFX_BROKERID_OFFSET         = 14;
const int TFX_ORDERID_OFFSET          = 21;
const int TFX_ACCOUNT_OFFSET          = 26;
const int TFX_ACCOUNT_FLAG_OFFSET     = 33;
const int TFX_SYMBOL_OFFSET           = 34;
//---------------------------------------------------------------------------
const int TSE_ACCOUNT_FLAG_OFFSET     = 32;
const int TSE_ACCOUNT_OFFSET          = 25;
const int TSE_ORDERID_OFFSET          = 20;
const int TSE_BROKERID_OFFSET         = 14;
const int TSE_TSE_ORDERTYPE_OFFSET    = 56;
const int TSE_ORDERTYPE_OFFSET        = 57;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    mFutures        = 0,
    mOption         = 1,
    mTSE            = 2,
    mOTC            = 3,
    mForeignFutures = 4, ///< Foreign Futures (with Order Routing)
    mForeignOptions = 5, ///< Foreign Options (with Order Routing)
    mForeignStock   = 6, ///< Foreign Stock   (with Order Routing)
    mCNFutures      = 7,
    mCNOptions      = 8,
    mES             = 9
            
} MarketEnum;
//---------------------------------------------------------------------------
typedef enum
{
    mtUnknownMsg   = 0,
    mtNewOrder     = 1,
    mtCancelOrder  = 2,
    mtReduceOrder  = 3,
    mtNewQuote     = 4,
    mtCancelQuote  = 5,
    mtReduceQuote  = 6,
    mtQuoteRequest = 7,
    mtOrderStatus  = 8,
    mtQuoteStatus  = 9,
    mtReplacePrice = 10            

} MessageType;       
//--------------------------------------------------------------------------------------------------------------------
typedef enum
{
    tsNormal,
    tsOdd,
    tsPost,
    tsLend,
    tsTender,
    tsAuction,
    tsTenderEx,
    tsNegotiatePx,
    tsOddEx
    
} TradeSession;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
    tmtT010,///< Transection sub-system. OK
    tmtP010,///< Post Transection sub-system. OK                            
    tmtO010,///< Odd Transection sub-system. OK                    
    tmtV010,///< Lend Transection sub-system. OK            
    tmtA010,///< Auction Transection sub-system. OK
    tmtE010,///< Tender Transection sub-system. OK             
            
    tmtO110,///< OddEx Transection sub-system. OK                                
    tmtUnknown

}TradeMessageType;
//----------------------------------------------------------------------------------------------------------
#define LOG_BUFFER_SIZE  10240
//----------------------------------------------------------------------------------------------------------
class ThreadClock
{
private:
	static UFC::PCriticalSection FTimerCS;
	static unsigned int   FRefreshTime;
	static BOOL           FIsInit;
#ifdef _WIN32
	static SYSTEMTIME   FSysTime;
#else
	static struct tm      FTime;
    static struct timeval FTimeVal;        
#endif
private:
	static void  UpdateTime( void );
	static void* Execute( void* );
public:
	static void  InitThreadClock( unsigned int TimeMS );
	static BOOL  IsInit( void );
	static int   SecondOfToday( void );
	static void  GetTimeString( char* TimeStr, BOOL WithSeparate = FALSE, BOOL WithMS = FALSE );
	static void  GetTimeStringMS( char* TimeStr );
    static void  GetTimeStringMS_US( char* TimeStr, BOOL UseUS );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MemoryStreamWriter
{
private:
	char* FPrt;
	int   FPos;
public:
	MemoryStreamWriter( char* Memory ):FPrt(Memory),FPos(0){}
	void Write( const char* Data, int Len )
	{
		memcpy( FPrt + FPos,Data, Len);
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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MemoryStreamReader
{
private:
	const char* FPrt;
	int   FPos;
public:
    MemoryStreamReader( const char* Memory ):FPrt(Memory),FPos(0){}        
    void Read(  char* Data, int Len )
    {
        memcpy( Data, FPrt + FPos, Len );
        FPos += Len;
    }
    void ReadString( char* Data, int Len )
    {
        memcpy( Data, FPrt + FPos, Len );
        Data[ Len ] = 0;
        FPos += Len;
    }
    int GetPosition(){ return FPos; }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class LogFile
{
private:
	FILE* FFileHandle;
public:
	LogFile( const char* FileName, const char*Mode );
	~LogFile();
	void Begin( void );
	int WriteLine(char* Buffer );
	char* ReadLine(char* Buffer,int MaxLength );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class LogManager
{
private:
	UFC::PCriticalSection FCS;
	UFC::FileStreamEx*    FFileStream;
	char*                 FBuffer;
	int                   FPos;
	int                   FBufferLength;
public:
	LogManager( UFC::AnsiString FileName, int BufferLength = UFC::AnsiString::MAX_STR_BUFFER );
	~LogManager();
	void WriteString( UFC::AnsiString& LogString );
	void WriteString( const char* LogString );
    void Write( const UInt8* Data, Int32 DataLen );
	void Flush( void );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class StringTokenizer
{
private:
	UFC::PList*      FTokens;
public:
	StringTokenizer( char* String );
	~StringTokenizer();
	int GetCount( void ) { return FTokens->ItemCount(); }
	UFC::AnsiString GetToken( int Index ) { return *((UFC::AnsiString*)FTokens->GetItem( Index )); }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ConfigReader
{
private:
	UFC::PList*      FNames;
	UFC::PList*      FValues;
public:
	ConfigReader( char* FileName );
	~ConfigReader( void );
	int GetCount( void ) { return FNames->ItemCount(); }
	UFC::AnsiString GetName( int Index );
	UFC::AnsiString GetValue( int Index );
	UFC::AnsiString GetValueByName( UFC::AnsiString Name );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class TMPExtFields
{
public:
    UInt32 RptSeq;
    char   ExecType;
    Int16  LineBrokerID;
    UInt8  PartID;    
    Int16  LastQty;
    Int16  CumQty;
    Int16  LeavesQty;
    Int16  BeforeQty;
    Int64  PxSubTotal;
    Int32  OrgTransTime;
    Int32  TransTime;
    UInt32 UniqID;
    Int32  Price;
    UInt8  OrdType;        ///< 1:Market, 2:Limit, 3:MarketWithProtection
    UInt8  TimeInForce;    ///< 0:ROD 3:IOC 4:FOK
    char   PositionEffect; ///< 'O':Open 'C':Close 'D':DayTrade '9':MarketMaker
    Int16  Qty;
    Int16  StatusCode;
public:    
    Int64  RecvTick;
public:
    ///< Constructor
    TMPExtFields( void )
    :RptSeq( 0 )
    ,ExecType( '0' )
    ,LineBrokerID( 0 )
    ,PartID( 0 )    
    ,LastQty( 0 )
    ,CumQty( 0 )
    ,LeavesQty( 0 )
    ,BeforeQty( 0 )
    ,PxSubTotal( 0 )
    ,OrgTransTime( 0 )
    ,TransTime( 0 )
    ,UniqID( 0 )
    ,Price( 0 )
    ,OrdType( 2 )
    ,TimeInForce( 0 )
    ,PositionEffect( 'O' )
    ,Qty(0)
    ,StatusCode( 0 )
    ,RecvTick(0)
    {
    }    
    ///< Copy Constructor
    TMPExtFields( const TMPExtFields& Ref )
    :RptSeq( Ref.RptSeq )
    ,ExecType( Ref.ExecType )
    ,PartID( Ref.PartID )
    ,LastQty( Ref.LastQty )
    ,CumQty( Ref.CumQty )
    ,LeavesQty( Ref.LeavesQty )
    ,BeforeQty( Ref.BeforeQty )
    ,PxSubTotal( Ref.PxSubTotal )
    ,OrgTransTime( Ref.OrgTransTime )
    ,TransTime( Ref.TransTime )
    ,UniqID( Ref.UniqID )
    ,Price( Ref.Price )
    ,OrdType( Ref.OrdType )
    ,TimeInForce( Ref.TimeInForce )
    ,PositionEffect( Ref.PositionEffect )
    ,Qty( Ref.Qty )
    ,StatusCode( Ref.StatusCode )
    ,RecvTick( Ref.RecvTick )
    {
    }
    ///< Constructor from String
    TMPExtFields( const UFC::AnsiString& Str )
    {
        FromString( Str );
    }
    void ToString( UFC::AnsiString& Str ) const;
    void FromString( const UFC::AnsiString& Str );
    static BOOL IsValidTMPExtStringLength( Int32 );
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ExecutionParser
{
private:
    static UFC::AnsiString EMPTY_TMP_EXT;
    UFC::PStringList FStrings;
    TMPExtFields     FTMPFields;
    BOOL             FIsTMP;
    UInt8*           FData;
    Int32            FSize;
public:
    ExecutionParser( const UFC::AnsiString& Log );
    ~ExecutionParser( void );
    Int32                  GetTime( void );
    BOOL                   IsBinary( void );
    BOOL                   IsConfirm( void );
    BOOL                   IsFill( void );
    BOOL                   IsOffHour( void );
    BOOL                   IsTFXTMP( void ) { return FIsTMP; }
    Int32                  GetNID( void );
    Int32                  GetSequence( void );
    const UFC::AnsiString& GetKey( void );
    const UFC::AnsiString& GetData( void );
    BOOL                   GetData( UInt8*& Data, Int32& Size );
    const UFC::AnsiString& GetPBNO( void );
    const UFC::AnsiString& GetHostname( void );
    const UFC::AnsiString& GetPVCID( void );
    const UFC::AnsiString& GetTMPExtFields( void );
    const UFC::AnsiString& GetSymbol( void );
    const UFC::AnsiString& GetSysOrderID( void ) { return GetPVCID(); } ///< For CFFEX
    const UFC::AnsiString& GetSessionID( void )    { return GetPBNO( ); }///< For CFFEX
    Int32                  GetQty( void );
    Int32                  GetPrice( void );
    ///< Ext fields for TMP
    UInt32 GetTMPSequence( void )  { return FTMPFields.RptSeq; }
    char   GetExecType( void )     { return FTMPFields.ExecType; }
    Int16  GetStatusCode( void )   { return FTMPFields.StatusCode; }
    Int16  GetLastQty( void )      { return FTMPFields.LastQty; }
    Int16  GetCumQty( void )       { return FTMPFields.CumQty; }
    Int16  GetLeavesQty( void )    { return FTMPFields.LeavesQty; }
    Int16  GetBeforeQty( void )    { return FTMPFields.BeforeQty; }
    Int64  GetPxSubTotal( void )   { return FTMPFields.PxSubTotal; }
    Int32  GetOrgTransTime( void ) { return FTMPFields.OrgTransTime; }
    Int32  GetTransTime( void )    { return FTMPFields.TransTime; }
    UInt32 GetUniqID( void )       { return FTMPFields.UniqID; }
    ///< For TSEC format (CFFEX Format)
    static void Render( UFC::AnsiString&       LogStr,   ///[Out]
                        BOOL                   IsConfirm,///[In]
                        Int32                  NID,      ///[In]
                        Int32                  Seq,      ///[In]
                        const UFC::AnsiString& Key, ///[In]
                        const UFC::AnsiString& Data,///[In]
                        const UFC::AnsiString& PBNO,///[In]  (CFFEX UserID)
                        const UFC::AnsiString& Host,///[In]   
                        const UFC::AnsiString& PVC ); ///[In]  (CFFEX SysOrderID )
    ///< For X.25 text format
    static void Render( UFC::AnsiString&       LogStr,   ///[Out]
                        BOOL                   IsConfirm,///[In]
                        BOOL                   IsOffHour,///[In]
                        Int32                  NID,      ///[In]
                        Int32                  Seq,      ///[In]
                        const UFC::AnsiString& Key, ///[In]
                        const UFC::AnsiString& Data,///[In]
                        const UFC::AnsiString& PBNO,///[In]
                        const UFC::AnsiString& Host,///[In]
                        const UFC::AnsiString& PVC, ///[In]
                        const UFC::AnsiString& Symbol, ///[In]
                        Int32                  Qty,    ///[In]
                        Int32                  Price,  ///[In]
                        const TMPExtFields*    TMPFields = NULL );///[In][Default]
    static void Render( char*                  LogStr,   ///[Out]
                        BOOL                   IsConfirm,///[In]
                        BOOL                   IsOffHour,///[In]
                        Int32                  NID,      ///[In]
                        Int32                  Seq,      ///[In]
                        const UFC::AnsiString& Key, ///[In]
                        const UFC::AnsiString& Data,///[In]
                        const UFC::AnsiString& PBNO,///[In]
                        const UFC::AnsiString& Host,///[In]
                        const UFC::AnsiString& PVC, ///[In]
                        const UFC::AnsiString& Symbol, ///[In]
                        Int32                  Qty,    ///[In]
                        Int32                  Price,  ///[In]
                        const TMPExtFields*    TMPFields = NULL );///[In][Default]    
    static void Render( UFC::AnsiString&       LogStr,   ///[Out]
                        BOOL                   IsConfirm,///[In]
                        BOOL                   IsOffHour,///[In]
                        Int32                  NID,      ///[In]
                        Int32                  Seq,      ///[In]
                        const UFC::AnsiString& Key, ///[In]
                        const UFC::AnsiString& Data,///[In]
                        const UFC::AnsiString& PBNO,///[In]
                        const UFC::AnsiString& Host,///[In]
                        const UFC::AnsiString& PVC, ///[In]
                        const UFC::AnsiString& Symbol, ///[In]
                        Int32                  Qty,    ///[In]
                        Int32                  Price,  ///[In]
                        const UFC::AnsiString& TMPExtStr );///[In]
    ///< for TMP binary format
    static void Render( UFC::AnsiString&       LogStr,   ///[Out]
                        BOOL                   IsConfirm,///[In]
                        BOOL                   IsOffHour,///[In]
                        Int32                  NID,      ///[In]
                        Int32                  Seq,      ///[In]
                        const UFC::AnsiString& Key, ///[In]
                        UInt8*                 Data,///[In]
                        Int32                  Size,///[In]
                        const UFC::AnsiString& PBNO,///[In]
                        const UFC::AnsiString& Host,///[In]
                        const UFC::AnsiString& PVC,
                        const UFC::AnsiString& Symbol, ///[In]
                        Int32                  Qty,    ///[In]
                        Int32                  Price); ///[In]
    static void Render( char*                  LogStr,   ///[Out]
                        BOOL                   IsConfirm,///[In]
                        BOOL                   IsOffHour,///[In]
                        Int32                  NID,      ///[In]
                        Int32                  Seq,      ///[In]
                        const UFC::AnsiString& Key, ///[In]
                        UInt8*                 Data,///[In]
                        Int32                  Size,///[In]
                        const UFC::AnsiString& PBNO,///[In]
                        const UFC::AnsiString& Host,///[In]
                        const UFC::AnsiString& PVC,
                        const UFC::AnsiString& Symbol, ///[In]
                        Int32                  Qty,    ///[In]
                        Int32                  Price); ///[In]
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
///  Binary Layout: IsBIN    ( 4 bytes ) Is binary format (TMP) or text format (x.25)
///                 NID      ( 4 bytes )
///                 Tick     ( 4 bytes )
///                 Key size ( 4 Bytes )
///                     Key data
///                 Host size( 4 bytes )
///                     Host data
///                 Data size( 4 bytes )
///                     Order data
/// Total size: 24 + Key size + Host size + Data size
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class OrderRecord        
{
public: ///< Basic info
    Int32            Group;  ///< Order Group
    BOOL             IsDirty;///< It's already processed by another Speedy?
    Int32            IsBin;  ///< It's binary formats?
    Int32            NID;    ///< Client network ID. 
    Int32            CNID;   ///< Cancel by NID 
    Int64            Tick;   ///< Tick in speedy
    Int64            InQTick;   ///< Tick in speedy
    char             OrdSource;
    BOOL             IsOffHour;
public:  ///< For TWSE/OTC use.  
    Int32            MsgType;    
    Int32            FuncCode;
    Int32            ProcessingStockID;    
    BOOL             IsBroken;
public: ///< for Binary Order        
    Int32            FSize;    
    MessageType      FType;    
    UInt8            FData[ 512 ];    
public:        
    char             MDSource[4];
    char             Key[512];
    char             Hostname[256];
    char             FOID[32];
private:    
    void*            OrderObj;
private:
    void DumpData( const char* Tag );
    void AssignData( BOOL IsTAIFEX, UInt8* Data, Int32 Size );
    void AssignData( BOOL IsTAIFEX, const UFC::AnsiString& OrderStr );
public:
    OrderRecord( void );
    OrderRecord( BOOL IsTAIFEX, MTree* Tree );
    OrderRecord( BOOL IsTAIFEX, Int32 group,BOOL OffHour, UInt8* Data );
    OrderRecord( BOOL IsTAIFEX, Int32 group,Int32 nid, Int32 cnid, const UFC::AnsiString& key, const UFC::AnsiString& order, const UFC::AnsiString& host, Int64 tick, const UFC::AnsiString& Source,BOOL OffHour );
    OrderRecord( BOOL IsTAIFEX, Int32 group,Int32 nid, Int32 cnid, const UFC::AnsiString& key, UInt8* Data,  Int32 DataSize, const UFC::AnsiString& host, Int64 tick,BOOL OffHour );
    void Init( BOOL IsTAIFEX, Int32 group,Int32 nid, Int32 cnid, const UFC::AnsiString& key, const UFC::AnsiString& order, const UFC::AnsiString& host, Int64 tick, const UFC::AnsiString& Source,BOOL OffHour );    
    void Init( BOOL IsTAIFEX, CheckData& CheckObj );   
    void InitTWSE( Int32 nid, int Func, const UFC::AnsiString& key, const UFC::AnsiString& order );
public:    
    BOOL             FromMTree( BOOL IsTAIFEX, MTree* Tree, Int32 MaxLength = 2048 );    
    void             FromBinary( BOOL IsTAIFEX, UInt8* Data );
    void             ToMTree( MTree* Tree  );
    BOOL             ToBinary( UInt8* Data, Int32 MaxLength  );
public:    
    void*            GetData( void )             { return FData; }
    Int32            GetSize( void )             { return FSize; }
    char*            GetOrder( void )            { return (char*)FData; }
    MessageType      GetMessageType( void )      { return FType; }
    char*            GetOrderID( void )          { return FOID; } 
    char             GetOrderSource( void )      { return OrdSource; }
    char*            GetMarketDataSource( void ) { return MDSource; }
    void*            GetOrderObject( void )      { return OrderObj; }
    void             ClearOrderObject( void )    { OrderObj = NULL; }
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Utility functions
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  MillIntToHHMMSSMMString( char *str, int t );
//int   MillInt( void );
void  GetCurrentTimeString( char *TimeStr );
void  GetCurrentTimeStringNT( char *TimeStr );
void  strncpynt( char *s1, const char *s2, int n );
//long  GetTimeMS( void );
UFC::AnsiString GetDateString( void );
UFC::AnsiString GetDateStringYYYYMMDD( void );
//----------------------------------------------------------------------------------------
extern MessageType  OrderType( BOOL IsTAIFEX, Int32 HeaderInt );
extern MessageType  TMPOrderType( UInt8* Order, Int32 Size );
extern TradeSession GetTradeSessionBySubsystemID( int Subsys );
extern BOOL         IsNewOrder( const UFC::AnsiString & Order, BOOL IsTAIFEX );
extern BOOL         IsTMPNewOrder( UInt8* Order, Int32 Size );
extern MessageType  GetTMPMessageType( UInt8* Order, Int32 Size, UFC::AnsiString& Account,BOOL& IsOrder );
extern BOOL         GetTMPOrderID( UInt8* Order, Int32 Size, UFC::AnsiString& OID );
extern char         GetTMPExecType(  UInt8* Order, Int32 Size );
extern char         GetOrderType( const char* Order,  Int32 Size );
extern MessageType  GetOrderID( BOOL IsTAIFEX, BOOL IsBin,  UInt8* Order, Int32 Size, UFC::AnsiString& OrderID );
extern MessageType  GetOrderID( BOOL IsTAIFEX, BOOL IsBin,  UInt8* Order, Int32 Size, char* OrderID );
extern void         X010ToX020( const UFC::AnsiString& X010, int ErrorCode, UFC::AnsiString& X020, BOOL NewVer, BOOL NewExVer );
//----------------------------------------------------------------------------------------
// Remote debug API
//------------------------------------------------------------------------------
#define SocketPrintf printf
//----------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------

