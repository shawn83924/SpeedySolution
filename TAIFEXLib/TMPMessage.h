//
// File:   TMPMessage.h
// Author: Simon Chang
//   
// Created on June 15, 2008, 10:27 PM
//
//------------------------------------------------------------------------------------------------------
#ifndef _TMPMESSAGE_H
#define	_TMPMESSAGE_H
//------------------------------------------------------------------------------------------------------
#include "TMPTypes.h"
#include "TMPDataFormat.h"
#include "TMPMessage.h"
#include "../UFC/ThreadObjectPool.h"
//-----------------------------------------------------------------------------------------
namespace TMP
{
using  namespace UFC;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    tlmOptions = 10,
    tlmFutures = 20,
    tlmBoth    = 30

}TMPLibMarket;

//----------------------------------------------------------------------------------------------------------------------
typedef enum 
{
    // Link sub-system messages( for both Options and Futures)
    tmtL10 = 10, ///< startup notify   ( TAIFEX <-> FCM )
    tmtL20 = 20, ///< startup confirm  ( TAIFEX <-- FCM )
    tmtL30 = 30, ///< Logon notify     ( TAIFEX --> FCM )
    tmtL40 = 40, ///< Logon request    ( TAIFEX <-- FCM )
    tmtL41 = 41, ///< Recover          ( TAIFEX --> FCM )
    tmtL42 = 42, ///< Recover confirm  ( TAIFEX <-- FCM )
    tmtL50 = 50, ///< Logon Response   ( TAIFEX --> FCM )
    tmtL60 = 60, ///< Logon confirm    ( TAIFEX <-- FCM )
    tmtL70 = 70, ///< End session      ( TAIFEX <-- FCM )
    tmtL80 = 80, ///< End confirm      ( TAIFEX --> FCM )
            
    tmtLX30 = 230, ///< FLEX Logon notify     ( TAIFEX --> FCM )            
    // Order sub-system message.
    tmtR01 = 101, ///< New Order
    tmtR02 = 102, ///< Execution Report
    tmtR22 = 122, ///< Execution Report
    tmtR03 = 103, ///< Reject execution
    tmtR04 = 104, ///< Heartbeat 
    tmtR05 = 105, ///< Heartbeat response
    tmtR07 = 107, ///< Quote Request
    tmtR08 = 108, ///< Quote Request response
    tmtR09 = 109, ///< Quote 
    tmtR11 = 111, ///< Trading Session Status request
    tmtR12 = 112, ///< Trading Session Status    
    tmtR13 = 113, ///< News request message
    tmtR14 = 114, ///< News response message
    tmtR15 = 115, ///< CM Exceution request message 
    tmtR16 = 116, ///< CM Exceution response message
    tmtR17 = 117, ///< TMP system setting message
    tmtR18 = 118, ///< TMP system notify messsage
    //< FLEX message.        
    tmtRX01 = 201, ///< New FLEX Order
    tmtRX02 = 202, ///< Execution Report       
    tmtRX03 = 203, ///< Reject execution       
    tmtRX07 = 207, ///< Quote Request                    
    tmtRX08 = 208, ///< Quote Request response
    tmtRX09 = 209, ///< Quote 
    tmtRX13 = 213, ///< News request message       
    tmtRX14 = 214, ///< News response message       
    tmtRX15 = 215, ///< CM Exceution request message         
    tmtRX16 = 216, ///< CM Exceution response message       
    tmtRX19 = 219, ///< New FLEX product request       
    tmtRX20 = 220, ///< New FLEX product response message       
    tmtRX40 = 240, ///< Recover executions request. (Like L40)
    tmtRX41 = 241, ///< Recover executions. (Like L41)
    tmtRX42 = 242, ///< Stop recover.
    // New Message, with Long Symbol support.
    tmtR31 = 131, ///< New Order (with Long Symbol support)
    tmtR32 = 132, ///< Execution Report (with Long Symbol support) 
    tmtR37 = 137, ///< Quote Request (with Long Symbol support)
    tmtR38 = 138, ///< Quote Request response (with Long Symbol support)
    tmtR39 = 139  ///< Quote (with Long Symbol support) 
     

} TMPMessageType;
//----------------------------------------------------------------------------------------------------------------------
const int R01_ExecType       = 0;
const int R01_cm_id          = 1;
const int R01_fcm_id         = 2;
const int R01_order_no       = 3;
const int R01_ord_id         = 4;
const int R01_user_define    = 5;
const int R01_Symbol         = 6;
const int R01_Price          = 7;
const int R01_qty            = 8;
const int R01_investor_acno  = 9;
const int R01_investor_flag  = 10;
const int R01_Side           = 11;
const int R01_OrdType        = 12;
const int R01_TimeInForce    = 13;
const int R01_PositionEffect = 14;
const int R01_order_source   = 15;
const int R01_info_source    = 16;
//------------------------------------------------------------------------------
const int R02_status_code    =  0;
const int R02_ExecType       =  1;
const int R02_cm_id          =  2;
const int R02_fcm_id         =  3;
const int R02_order_no       =  4;
const int R02_ord_id         =  5;
const int R02_user_define    =  6;
const int R02_Symbol         =  7;
const int R02_Price          =  8;
const int R02_qty            =  9;
const int R02_investor_acno  = 10;
const int R02_investor_flag  = 11;
const int R02_Side           = 12;
const int R02_OrdType        = 13;
const int R02_TimeInForce    = 14;
const int R02_PositionEffect = 15;
const int R02_LastPx         = 16;
const int R02_LastQty        = 17;
const int R02_px_subtotal    = 18;
const int R02_CumQty         = 19;
const int R02_LeavesQty      = 20;
const int R02_before_qty     = 21;
const int R02_leg_side1      = 22;
const int R02_leg_side2      = 23;
const int R02_leg_px1        = 24;
const int R02_leg_px2        = 25;
const int R02_leg_qty1       = 26;
const int R02_leg_qty2       = 27;
const int R02_org_trans_time = 28;
const int R02_TransactTime   = 29;
const int R02_target_id      = 30;
const int R02_uniq_id        = 31;
const int R02_rpt_seq        = 32;
const int R02_protocol_type  = 33;
//------------------------------------------------------------------------------
const int R22_status_code    =  0;
const int R22_ExecType       =  1;
const int R22_fcm_id         =  2;
const int R22_order_no       =  3;
const int R22_ord_id         =  4;
const int R22_user_define    =  5;
const int R22_Side           =  6;
const int R22_PositionEffect =  7;
const int R22_LeavesQty      =  8;
const int R22_before_qty     =  9;
const int R22_leg_px1        = 10;
const int R22_leg_px2        = 11;
const int R22_leg_qty1       = 12;
const int R22_leg_qty2       = 13;
const int R22_TransactTime   = 14;
const int R22_uniq_id        = 15;
const int R22_rpt_seq        = 16;
const int R22_protocol_type  = 17;
const int R22_Price          = 18;
//------------------------------------------------------------------------------------------------------
const UInt16 FCM_NOT_EXIST = 9999;
//------------------------------------------------------------------------------------------------------
class SymbolInfo
{
public:
	UFC::AnsiString Symbol;
	int             Index;
	int             DecimalLocator;
public:
	SymbolInfo( const UFC::AnsiString Sym, int pseq, int decimal )
	:Symbol( Sym )
	,Index( pseq )
	,DecimalLocator( decimal ){}
};
//------------------------------------------------------------------------------------------------------
class FutP08Loader :public UFC::FileChangedListrner
{
public: ///< Implement interface PStream.
    virtual ~FutP08Loader() {}
	virtual void OnFileChanged( const UFC::AnsiString& FileName, void* data );
};
//------------------------------------------------------------------------------------------------------
class OptP08Loader :public UFC::FileChangedListrner
{
public: ///< Implement interface PStream.
    virtual ~OptP08Loader() {}
	virtual void OnFileChanged( const UFC::AnsiString& FileName, void* data );
};
//------------------------------------------------------------------------------------------------------
class TMPR01Message;
class TMPR07Message;
class TMPR09Message;
class TMPRX01Message;
class TMPRX07Message;
class TMPRX09Message;
//------------------------------------------------------------------------------------------------------
class TMPMessage
{
private:  
    static UFC::PCriticalSection  SymbolLock;
    static FutP08Loader   FUTLoader; 
    static OptP08Loader   OPTLoader; 
    static TMPDataFormat* Formats[ 256 ];
    static BOOL           EnableCheckSum;
    static BOOL           EnableFLEX;
    static BOOL           OptionMarket;    
    static UFC::PHashedList<UFC::AnsiString, TMPDataFormat*> FormatMap;
    ///< For FCM
    static UFC::AnsiString                                   NoFCM;
    static UFC::PHashMap<UFC::AnsiString, int>               FCMMap;
    static UFC::PHashedList<int, UFC::AnsiString*>           FCMIndex;
    ///< For Symbols
    static UFC::AnsiString                                   NoOPTSymbol;
    static UFC::AnsiString                                   NoFUTSymbol;
    static UFC::PHashedList<UFC::AnsiString, SymbolInfo*>    SymbolMap;
    static UFC::PHashedList<int, SymbolInfo*>                FUTSymbolIndex;
    static UFC::PHashedList<int, SymbolInfo*>                OPTSymbolIndex;
    static UFC::PHashedList<int, SymbolInfo*>                FUTOffHourSymbolIndex;
    static UFC::PHashedList<int, SymbolInfo*>                OPTOffHourSymbolIndex;
    static UFC::PHashMap<UFC::AnsiString, int>               URLMap;
    static UFC::UDateTime                                    FUTP06Time;     
    static UFC::UDateTime                                    FUTP07Time;         
    static UFC::UDateTime                                    FUTP08Time;                 
    static UFC::UDateTime                                    OPTP06Time;     
    static UFC::UDateTime                                    OPTP07Time;         
    static UFC::UDateTime                                    OPTP08Time;             
    static BOOL                                              IsFUTPA7;
    static BOOL                                              IsOPTPA7;
public:    
    static UFC::UDateTime                                    FUTOffHourP08Time;
    static UFC::UDateTime                                    OPTOffHourP08Time;                 
private: ///< TMP Header members.
    TMPUInt16 FMsgLen;       ///< 2
    TMPUInt32 FMsgSeq;
    TMPTime   FMsgTime;
    TMPUInt8  FMsgType;        
    TMPUInt16 FFCMID;
    TMPUInt16 FSessionID;
    TMPUInt8  FCheckSum; 
private:        
    /**
     * Fields container
     */
    UFC::PtrList<TMPType> FFields;
    /**
     * Message format object.
     */
    TMPDataFormat*        FFormat;
private:      
    UInt8 HeaderCheckSum( void );
    UInt8 BodyCheckSum( void );
    UInt8 CalculateCheckSum( UInt8* Data, Int32 Length );
    void  SpiltNameValue( const UFC::AnsiString& Line, UFC::AnsiString& Name, UFC::AnsiString& Value );
    TMPMessage( void ){}
public:    
    static void LoadP08File(  BOOL IsOptions, BOOL IsOffHour, const UFC::AnsiString& File,const UFC::AnsiString& Format );        
private:                
    static BOOL LoadFCMFromFile( BOOL IsOptions, const UFC::AnsiString& FileOrDirName  );
    static void LoadSymbolFromFile( BOOL IsOptions, const UFC::AnsiString& FileOrDirName, const UFC::AnsiString& CfgDir );    
#ifndef _FOR_FCM
    static void LoadFCMFromSQLite( const UFC::AnsiString& FileOrDirName  );
    static int  SQLiteFCMcallback( void* count, int argc, char **argv, char ** azColName );
    static void LoadSymbolFromSQLite( const UFC::AnsiString& FileOrDirName  );
    static int  SQLiteSymbolcallback( void* count, int argc, char **argv, char ** azColName );
#endif
public:
    /**     
     * Constructor
     * Construct a TMP message from Name=Value sytle string.
     * or use a message name to create a TMP Message.( Like "L10", "R01"... )     
     */
    TMPMessage( const UFC::AnsiString& TMPstring );    
    /**     
     * Constructor
     * Construct a TMP message from binary stream.
     */    
    TMPMessage( UFC::PStream* Stream );        

    /**     
     * Constructor
     * Construct a TMP message from binary stream.
     * Add By Zhen Fan 2009.04.20
     */
    TMPMessage( UFC::PStream* Stream, UFC::AnsiString BrokerOrClearMemberType );

    /**     
     * Constructor
     * Use message type to create a TMP message.
     */    
    TMPMessage( TMPMessageType Type );
    /**
     * Copy Constructor
     * Use existing message to create a TMP message.
     */
    TMPMessage( TMPMessage& Src );
    /**     
     * Destructor     
     */        
    ~TMPMessage( void );
    /**
     * Get the TMP message create time.
     */
    TMPTime&  GetMessageTime( void ) { return FMsgTime; }
    /**
     * Get the unique sequence number of this TMP message.
     */
	UInt32  GetMessageSeq( void ) { return FMsgSeq.ToInteger(); }
    /**
     * Set this TMP message unique sequence number.
     */
    void    SetMessageSeq( UInt32 Seq ) { FMsgSeq = Seq; }
    /**
     * Get this TMP message type.
     */
    TMPMessageType GetMessageType( void ) { return (TMPMessageType)FMsgType.ToInteger(); }
    /**
     * Get the Session FCM Index of this message.
     */
	UInt16  GetFCMID( void )          { return FFCMID.ToInteger(); }
    /**
     * Set the Session FCM Index of this message.
     */
    void    SetFCMID( UInt16 FCMID)   { FFCMID  = TMPUInt16( FCMID ); }
    /**
     * Get the SessionID of this message.
     */
	UInt16  GetSessionID( void )      { return FSessionID.ToInteger(); }
    /**
     * Set the SessionID of this message.
     */
    void    SetSessionID( UInt16 SID) { FSessionID  = TMPUInt16( SID ); }
    /**
     * Serialize this TMP message to stream.
     */
    Int32    ToStream( UFC::PStream* Stream );
    /**
     * Convert this TMP message to string.
     */
    Int32    ToString(  UFC::AnsiString& TMPString  );
    Int32    ToStreamString( UFC::PStream* Stream, UFC::AnsiString& TMPString );
    /**
     * Check TMP message field existence.
     */
    BOOL     FieldExists( const UFC::AnsiString& Name );
    /**
     * Get TMP message field by name.
     */    
    TMPType* GetField( const UFC::AnsiString& Name );
    /**
     * Get TMP message field by index.
     */
    TMPType* GetField( Int32 Index );
    /**
     * Get TMP message size. (Binary size)
     */    
    Int32    GetMessageSize( void );
    Int32    GetMessageFullSize( void ) { return GetMessageSize( ) + TMP_SIZE_DIFF; }
    /**
     * Set TMP message field value by field name.
     *
     * @Param Name, Name of this field.
     * @Param Value, Value want to set.
     *        Char: Single character string
     *        CharArray: String.
     *        Int8,UInt8,Int16,UInt16,Int32,UInt32,Int64: Number string
     *        Time: String format HH:MM:SS.mmm.
     *        Symbol string: TAIFEX symbol string.
     *        Symbol Binary: String format Leg1ID,Leg2ID,Leg1Side,Leg2Side,CombOP 
     *    
     *        (* You can use this function to set value for all TMP types.)        
     */
    void     SetField( const AnsiString& Name, const AnsiString& Value );    
    /**
     * Set TMP message field value by index.
     */
    void     SetField( Int32 Index, const AnsiString& Value );
    void     SetField( Int32 Index, TMPType& Value );
    void     SetField( Int32 Index, TMPType* Value );
    /**
     * Set TMP message field value by field name.
     *
     * @Param Name, Name of this field.
     * @Param Value, Value want to set.
     *        Support types:
     *        Char,Int8,UInt8,Int16,UInt16,Int32,UInt32,Int64
     *         
     *        Not Support types: 
     *        Time,Symbol,CharArray
     */
    void     SetField( const AnsiString& Name, Int32 Value );
    void     SetField( const AnsiString& Name, TMPType& Value );
    void     SetField( const AnsiString& Name, TMPType* Value );
    /**
     * Set TMP message field value by index.
     */
    void     SetField( Int32 Index, Int32 Value );
    void     UpdateTime( void ) { FMsgTime.CurrentTime(); }
public:    
    /**
     * Operator [ index ]
     * Get TMP message field by index.
     */
    TMPType&   operator [] (const int index);
    /**
     * Operator [ Name ]
     * Get TMP message field by name string.
     */
    TMPType&   operator [] (const UFC::AnsiString& Name );    
public:    
    /**
     * Initialize TMP message library.
     *
     * @Param [in]LibraryMarket, Initialize library for Futures, Options or both system.
     *
     */
    static void        InitTMPMessageLibrary( const TMPLibMarket LibraryMarket );
    /**
     * Initialize the TMP BrokerID+SessionID map to URL lookup table. Load from file P07.BrokerID
     *
     * @Param [in]IsOptions, For Futures or Options system.
     * @Param [in]BrokerID, For this broker.
     */
    static void        LoadURLTable( BOOL IsOptions, const UFC::AnsiString& BrokerID );
    /**
     * lookup the TAIFEX TMP server URL and port.
     *
     * @Param [in]IsOptions, For Futures or Options system.
     * @Param [in]BrokerID,  BrokerID of this session.
     * @Param [in]SessionID, SessionID of this session.
     * @Param [in]Backup,    Backup URL/Port for this session.
     * @Param [Out]URL,      URL of this session.
     * @Param [Out]Port,     Server port of this session.
     * @Param [in]IsTEST,    Connection to testing environment.(default FALSE)
     * @Param [in]IsCM,      Is a ClearMameber session.(default FALSE)
     * @Param [in]IsOffHour, Is a off hour trading session
     */
    static BOOL        GetURLPort( BOOL IsOptions, const UFC::AnsiString& BrokerID, UInt16 SessionID, Int32 Backup, UFC::AnsiString& URL, UInt16& Port, BOOL IsTEST, BOOL IsCM , BOOL IsOffHour );

public:
    /**
     * Get BorkerID index from  BorkerID string .
     */
    static UInt16                 GetBrokerID( const UFC::AnsiString& BrokerID );
    /**
     * Get BorkerID String by index.
     */    
    static const UFC::AnsiString& GetBrokerID( UInt16 Index );
    /**
     * Get CMID in index mode.
     */
    static UInt16                 GetCMID( const UFC::AnsiString& CMID, const UFC::AnsiString& BrokerID );
    /**
     * Get Symbol index from Symbol string .
     */
    static UInt16                 GetSymbol( const UFC::AnsiString& Symbol );
    static UInt16                 GetSymbol( const UFC::AnsiString& Symbol, int& Decimal );
    static BOOL                   SymbolExists( const UFC::AnsiString& Symbol );
    /**
     * Get BorkerID String by index.
     */    
    static const UFC::AnsiString& GetSymbol( BOOL IsOptions, BOOL IsOffHour, UInt16 Index );
    static BOOL                   SymbolExists( BOOL IsOptions, BOOL IsOffHour, UInt16 Index );
    /**
     * Enable/Disable checksum.
     */
    static void                   SetEnableCheckSum( BOOL Enabled )  { EnableCheckSum = Enabled; }
    static BOOL                   IsCheckSumEnabled( void )          { return EnableCheckSum; }    
    /**
     * Enable/Disable FLEX.
     */
    static void                   SetEnableFLEX( BOOL Enabled )  { EnableFLEX = Enabled; }
    static BOOL                   IsFLEXEnabled( void )          { return EnableFLEX; }    
    /**
     * Binary data to Hex string.( example:0xAF345 )
     */
    static void                   ToHexString( UFC::AnsiString& Result, UInt8* Data, Int32 Length );
    static UFC::UDateTime&        GetP06FileTime( BOOL IsOpt )       { return IsOpt? OPTP06Time:FUTP06Time; }     
    static UFC::UDateTime&        GetP07FileTime( BOOL IsOpt )       { return IsOpt? OPTP07Time:FUTP07Time; }         
    static UFC::UDateTime&        GetP08FileTime( BOOL IsOpt )       { return IsOpt? OPTP08Time:FUTP08Time; }
    static UFC::UDateTime&        GetOffHourP08FileTime( BOOL IsOpt ){ return IsOpt? OPTOffHourP08Time:FUTOffHourP08Time; }    
    static BOOL                   IsLoadPA7( BOOL IsOpt )            { return IsOpt? IsOPTPA7:IsFUTPA7; }

    static PThreadObjectPool<TMPR01Message>* R01PoolPtr;  //Add by Zhen Fan 2021/05/20
    static PThreadObjectPool<TMPR07Message>* R07PoolPtr;  //Add by Zhen Fan 2021/05/20
    static PThreadObjectPool<TMPR09Message>* R09PoolPtr;  //Add by Zhen Fan 2021/05/20
    static PThreadObjectPool<TMPRX01Message>* RX01PoolPtr; 
    static PThreadObjectPool<TMPRX07Message>* RX07PoolPtr; 
    static PThreadObjectPool<TMPRX09Message>* RX09PoolPtr; 
    static void InitializeThreadPool();  //Add by Zhen Fan 2021/05/20
};
//------------------------------------------------------------------------------------------------------
class FastTMPMessage
{
public:     
    virtual ~FastTMPMessage() {}
    virtual const UInt8* GetBytes( void ) = 0;
    virtual int    Length( void )   = 0;
    virtual void   Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq ) = 0;
    virtual int    ToString( char* Buf, int Len ) = 0;
    virtual TMPMessageType GetMessageType( void ) = 0;
    virtual char   GetExecType( void ) = 0;
    virtual void   SetExecType( char ) = 0;
    virtual int    GetFCMID( void ) = 0;
    virtual int    GetOrdID( void ) = 0;
    virtual void   GetOrderID( char* Out ) = 0;
    virtual void   Recycle( void )  = 0;
};
//------------------------------------------------------------------------------------------------------
class TMPR01Message : public FastTMPMessage
{
private:  ///< TMP Message header  
    UFC::RNUInt16    msg_length; ///< length 77(fixed)
    UFC::RNUInt32    MsgSeqNum;
    UFC::RNUInt32    epoch_s;
    UFC::RNUInt16    ms;
    UFC::RNUInt8     MessageType; ///< R01: 101(fixed)
    UFC::RNUInt16    fcm_id_h;    ///< fixed by session
    UFC::RNUInt16    session_id;  ///< fixed by session
private: ///< TMP Message body   
    UFC::RNChar      ExecType;
    UFC::RNUInt16    cm_id;
    UFC::RNUInt16    fcm_id;
    UFC::RNCharArray order_no;//[5];
    UFC::RNUInt32    ord_id;
    UFC::RNUInt64    user_define;
    UFC::RNUInt8     symbol_type;
    UFC::RNCharArray symbol;//[ 20 ];
    UFC::RNUInt32    Price;
    UFC::RNUInt16    qty;
    UFC::RNUInt32    investor_acno;
    UFC::RNChar      investor_flag;
    UFC::RNUInt8     Side;
    UFC::RNUInt8     Ordtype;
    UFC::RNUInt8     TimeInForce;
    UFC::RNChar      PositionEffect;
    UFC::RNChar      order_souce;
    UFC::RNCharArray info_souce;//[3];
private:        
    UFC::RNUInt8     CheckSum;
private:        
    UFC::UInt8       FRowData[128];
public:    
    TMPR01Message()
    :msg_length( FRowData , 0 ) ///< length 77(fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< R01: 101(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,ExecType( FRowData , 17 )
    ,cm_id( FRowData , 18 )
    ,fcm_id( FRowData , 20 )
    ,order_no( FRowData , 22, 5 )//[5];
    ,ord_id( FRowData , 27 )
    ,user_define( FRowData , 31 )
    ,symbol_type( FRowData , 39 )
    ,symbol( FRowData , 40, 20 )//[ 20 ];
    ,Price( FRowData , 60 )
    ,qty( FRowData , 64 )
    ,investor_acno( FRowData , 66 )
    ,investor_flag( FRowData , 70 )
    ,Side( FRowData , 71 )
    ,Ordtype( FRowData , 72 )
    ,TimeInForce( FRowData , 73 )
    ,PositionEffect( FRowData , 74 )
    ,order_souce( FRowData , 75 )
    ,info_souce( FRowData , 76, 3 )//[3];
    ,CheckSum( FRowData , 79 )    
    {
        msg_length.Set( 77 );
        MessageType.Set( 101 );
    }
    TMPR01Message( TMPR01Message& Ref )
    :msg_length( FRowData , 0 ) ///< length 77(fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< R01: 101(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,ExecType( FRowData , 17 )
    ,cm_id( FRowData , 18 )
    ,fcm_id( FRowData , 20 )
    ,order_no( FRowData , 22, 5 )//[5];
    ,ord_id( FRowData , 27 )
    ,user_define( FRowData , 31 )
    ,symbol_type( FRowData , 39 )
    ,symbol( FRowData , 40, 20 )//[ 20 ];
    ,Price( FRowData , 60 )
    ,qty( FRowData , 64 )
    ,investor_acno( FRowData , 66 )
    ,investor_flag( FRowData , 70 )
    ,Side( FRowData , 71 )
    ,Ordtype( FRowData , 72 )
    ,TimeInForce( FRowData , 73 )
    ,PositionEffect( FRowData , 74 )
    ,order_souce( FRowData , 75 )
    ,info_souce( FRowData , 76, 3 )//[3];
    ,CheckSum( FRowData , 79 )    
    {
        memcpy( FRowData, Ref.FRowData, 128 );
    }
    const UInt8*   GetBytes( void )        { return FRowData; }
    int            Length( void )          { return 80; } ///< 77 + msg_length(2) + CheckSum(1)
    void           Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq );    
    int            ToString( char* Buf, int Len );
    TMPMessageType GetMessageType( void )  { return tmtR01; }
    char           GetExecType( void )     { return ExecType.Get(); }
    void           SetExecType( char tp )  { ExecType.Set( tp); }
    int            GetFCMID( void )        { return fcm_id.Get(); }
    int            GetOrdID( void )        { return ord_id.Get(); }
    void           GetOrderID( char* Out ) { order_no.Get(Out); }
    void           Recycle( void );  //Add by Zhen Fan 2021/05/20
public:
    void Set_ExecType( char exectype )        { ExecType.Set(exectype); }
    void Set_cm_id( UFC::UInt16 cmid )        { cm_id.Set(cmid); }
    void Set_fcm_id( UFC::UInt16 fcmid )      { fcm_id.Set(fcmid); }
    void Set_order_no( const char* oid )      { order_no.Set( oid); }
    void Set_ord_id( UFC::UInt32 oid )        { ord_id.Set(oid); }
    void Set_user_define( UFC::UInt64 udd )   { user_define.Set(udd); }
    void Set_symbol_type( UFC::UInt8 st )     { symbol_type.Set(st); }  
    void Set_symbol( const char* sym)         { symbol.Set(sym); }
    void Set_Price( UFC::UInt32 px )          { Price.Set(px); }
    void Set_qty( UFC::UInt16 Qty  )          { qty.Set(Qty); }
    void Set_investor_acno( UFC::UInt32 acno ){ investor_acno.Set(acno); }
    void Set_investor_flag( char flag )       { investor_flag.Set(flag); }
    void Set_Side( UFC::UInt8 side )          { Side.Set(side); }  
    void Set_Ordtype( UFC::UInt8 ot)          { Ordtype.Set(ot); }
    void Set_TimeInForce( UFC::UInt8 tif)     { TimeInForce.Set(tif); }
    void Set_PositionEffect( char pe )        { PositionEffect.Set(pe); }
    void Set_order_souce( char oc )           { order_souce.Set(oc); }
    void Set_info_souce( const char* is)      { info_souce.Set(is); }
    
};        
//------------------------------------------------------------------------------------------------------
class TMPR07Message : public FastTMPMessage
{
private: ///< TMP Message header   
    UFC::RNUInt16     msg_length; ///< length 51(fixed)
    UFC::RNUInt32     MsgSeqNum;
    UFC::RNUInt32     epoch_s;
    UFC::RNUInt16     ms;
    UFC::RNUInt8      MessageType; ///< R07: 107(fixed)
    UFC::RNUInt16     fcm_id_h;    ///< fixed by session
    UFC::RNUInt16     session_id;  ///< fixed by session
private: ///< TMP Message body    
    UFC::RNCharArray  order_no;//[5];
    UFC::RNUInt32     ord_id;
    UFC::RNUInt16     fcm_id;
    UFC::RNUInt8      symbol_type;
    UFC::RNCharArray  symbol;//[ 20 ];
    UFC::RNChar       order_souce;
    UFC::RNCharArray  info_souce;//[3];
private:        
    UFC::RNUInt8      CheckSum;
private:        
    UFC::UInt8       FRowData[64];    
public:    
    TMPR07Message()
    :msg_length( FRowData , 0 ) ///< length 51(fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< R07: 107(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,order_no( FRowData , 17 , 5)
    ,ord_id( FRowData , 22 )
    ,fcm_id( FRowData , 26 )
    ,symbol_type( FRowData , 28 )
    ,symbol( FRowData , 29, 20 )
    ,order_souce( FRowData , 49 )
    ,info_souce( FRowData , 50, 3 )
    ,CheckSum( FRowData , 53 )    
    {
        msg_length.Set( 51 );
        MessageType.Set( 107 );
    }    
    TMPR07Message( TMPR07Message& Ref )
    :msg_length( FRowData , 0 ) ///< length 51(fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< R07: 107(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,order_no( FRowData , 17 , 5)
    ,ord_id( FRowData , 22 )
    ,fcm_id( FRowData , 26 )
    ,symbol_type( FRowData , 28 )
    ,symbol( FRowData , 29, 20 )
    ,order_souce( FRowData , 49 )
    ,info_souce( FRowData , 50, 3 )
    ,CheckSum( FRowData , 53 )    
    {
        memcpy( FRowData, Ref.FRowData, 64 );
    }  
    const UInt8* GetBytes( void ) { return FRowData; }
    int          Length( void )   { return 54; } ///< 51 + msg_length(2) + CheckSum(1)
    void         Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq );    
    int         ToString( char* Buf, int Len );
    TMPMessageType GetMessageType( void ) { return tmtR07; }    
    char           GetExecType( void )     { return '0'; }
    void           SetExecType( char tp )  { }
    int            GetFCMID( void )        { return fcm_id.Get(); }
    int            GetOrdID( void )        { return ord_id.Get(); }
    void           GetOrderID( char* Out ) { order_no.Get(Out); }
    void           Recycle( void );  //Add by Zhen Fan 2021/05/20
public:        
    void Set_fcm_id( UFC::UInt16 fcmid )      { fcm_id.Set(fcmid); }
    void Set_order_no( const char* oid )      { order_no.Set(oid); }
    void Set_ord_id( UFC::UInt32 oid )        { ord_id.Set(oid); }
    void Set_symbol_type( UFC::UInt8 st )     { symbol_type.Set(st); }  
    void Set_symbol( const char* sym)         { symbol.Set(sym); }
    void Set_order_souce( char oc )           { order_souce.Set(oc); }
    void Set_info_souce( const char* is)      { info_souce.Set(is); }
    
};    
//------------------------------------------------------------------------------------------------------
class TMPR09Message : public FastTMPMessage
{
private: ///< TMP Message header   
    UFC::RNUInt16    msg_length; ///< length 81 (fixed)
    UFC::RNUInt32    MsgSeqNum;
    UFC::RNUInt32    epoch_s;
    UFC::RNUInt16    ms;
    UFC::RNUInt8     MessageType; ///< R01: 109(fixed)
    UFC::RNUInt16    fcm_id_h;    ///< fixed by session
    UFC::RNUInt16    session_id;  ///< fixed by session
private: ///< TMP Message body   
    UFC::RNChar      ExecType;
    UFC::RNUInt16    cm_id;
    UFC::RNUInt16    fcm_id;
    UFC::RNCharArray order_no;//[5];
    UFC::RNUInt32    ord_id;
    UFC::RNUInt64    user_define;
    UFC::RNUInt8     symbol_type;
    UFC::RNCharArray symbol;//[ 20 ];
    UFC::RNUInt32    BidPx;
    UFC::RNUInt32    OfferPx;
    UFC::RNUInt16    BidSize;
    UFC::RNUInt16    OfferSize;
    UFC::RNUInt32    investor_acno;
    UFC::RNChar      investor_flag;
    UFC::RNUInt8     TimeInForce;
    UFC::RNChar      PositionEffect;
    UFC::RNChar      order_souce;
    UFC::RNCharArray info_souce;//[3];
private:        
    UFC::RNUInt8     CheckSum;
private:        
    UFC::UInt8       FRowData[128];
public:    
    TMPR09Message()
    :msg_length( FRowData , 0 ) ///< length 81 (fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< R09: 109(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,ExecType( FRowData , 17 )
    ,cm_id( FRowData , 18 )
    ,fcm_id( FRowData , 20 )
    ,order_no( FRowData , 22 ,5 )//[5];
    ,ord_id( FRowData , 27 )
    ,user_define( FRowData , 31 )
    ,symbol_type( FRowData , 39 )
    ,symbol( FRowData , 40, 20 )//[ 20 ];
    ,BidPx( FRowData , 60 )
    ,OfferPx( FRowData , 64 )
    ,BidSize( FRowData , 68 )
    ,OfferSize( FRowData , 70 )
    ,investor_acno( FRowData , 72 )
    ,investor_flag( FRowData , 76 )
    ,TimeInForce( FRowData , 77 )
    ,PositionEffect( FRowData , 78 )
    ,order_souce( FRowData , 79 )
    ,info_souce( FRowData , 80,3 )//[3];
    ,CheckSum( FRowData , 83 )
    {
        msg_length.Set( 81 );
        MessageType.Set( 109 );
    }
    TMPR09Message( TMPR09Message& Ref )
    :msg_length( FRowData , 0 ) ///< length 81 (fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< R09: 109(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,ExecType( FRowData , 17 )
    ,cm_id( FRowData , 18 )
    ,fcm_id( FRowData , 20 )
    ,order_no( FRowData , 22 ,5 )//[5];
    ,ord_id( FRowData , 27 )
    ,user_define( FRowData , 31 )
    ,symbol_type( FRowData , 39 )
    ,symbol( FRowData , 40, 20 )//[ 20 ];
    ,BidPx( FRowData , 60 )
    ,OfferPx( FRowData , 64 )
    ,BidSize( FRowData , 68 )
    ,OfferSize( FRowData , 70 )
    ,investor_acno( FRowData , 72 )
    ,investor_flag( FRowData , 76 )
    ,TimeInForce( FRowData , 77 )
    ,PositionEffect( FRowData , 78 )
    ,order_souce( FRowData , 79 )
    ,info_souce( FRowData , 80,3 )//[3];
    ,CheckSum( FRowData , 83 )
    {
        memcpy( FRowData, Ref.FRowData, 128 );
    }
    const UInt8* GetBytes( void ) { return FRowData; }
    int          Length( void )   { return 84; } ///< 81 + msg_length(2) + CheckSum(1)
    void         Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq );
    int         ToString( char* Buf, int Len );
    TMPMessageType GetMessageType( void ) { return tmtR09; }
    char           GetExecType( void )     { return ExecType.Get(); }
    void           SetExecType( char tp )  { ExecType.Set( tp); }
    int            GetFCMID( void )        { return fcm_id.Get(); }
    int            GetOrdID( void )        { return ord_id.Get(); }
    void           GetOrderID( char* Out ) { order_no.Get(Out); }
    void           Recycle( void );  //Add by Zhen Fan 2021/05/20
public:
    void Set_ExecType( char exectype )        { ExecType.Set(exectype); }
    void Set_cm_id( UFC::UInt16 cmid )        { cm_id.Set(cmid); }
    void Set_fcm_id( UFC::UInt16 fcmid )      { fcm_id.Set(fcmid); }
    void Set_order_no( const char* oid )      { order_no.Set(oid); }
    void Set_ord_id( UFC::UInt32 oid )        { ord_id.Set(oid); }
    void Set_user_define( UFC::UInt64 udd )   { user_define.Set(udd); }
    void Set_symbol_type( UFC::UInt8 st )     { symbol_type.Set(st); }  
    void Set_symbol( const char* sym)         { symbol.Set(sym); }
    void Set_BidPx( UFC::UInt32 px )          { BidPx.Set(px); }
    void Set_OfferPx( UFC::UInt32 px )        { OfferPx.Set(px); }
    void Set_BidSize( UFC::UInt16 Qty  )      { BidSize.Set(Qty); }
    void Set_OfferSize( UFC::UInt16 Qty  )    { OfferSize.Set(Qty); }
    void Set_investor_acno( UFC::UInt32 acno ){ investor_acno.Set(acno); }
    void Set_investor_flag( char flag )       { investor_flag.Set(flag); }    
    void Set_TimeInForce( UFC::UInt8 tif)     { TimeInForce.Set(tif); }
    void Set_PositionEffect( char pe )        { PositionEffect.Set(pe); }
    void Set_order_souce( char oc )           { order_souce.Set(oc); }
    void Set_info_souce( const char* is)      { info_souce.Set(is); }    
};        
//------------------------------------------------------------------------------------------------------
class TMPRX01Message : public FastTMPMessage
{
private:  ///< TMP Message header  
    UFC::RNUInt16    msg_length; ///< length 77(fixed)
    UFC::RNUInt32    MsgSeqNum;
    UFC::RNUInt32    epoch_s;
    UFC::RNUInt16    ms;
    UFC::RNUInt8     MessageType; ///< R01: 101(fixed)
    UFC::RNUInt16    fcm_id_h;    ///< fixed by session
    UFC::RNUInt16    session_id;  ///< fixed by session
private: ///< TMP Message header Ex     
    UFC::RNUInt8     MsgTypeExt; 
    UFC::RNUInt8     preserve; 
    UFC::RNUInt32    msg_time_ns;
private: ///< TMP Message body   
    UFC::RNChar      ExecType;
    UFC::RNUInt16    cm_id;
    UFC::RNUInt16    fcm_id;
    UFC::RNCharArray order_no;//[5];
    UFC::RNUInt32    ord_id;
    UFC::RNUInt64    user_define;
    UFC::RNCharArray symbol;//[ 20 ];
    UFC::RNUInt32    Price;
    UFC::RNUInt16    qty;
    UFC::RNUInt32    investor_acno;
    UFC::RNChar      investor_flag;
    UFC::RNUInt8     Side;
    UFC::RNUInt8     Ordtype;
    UFC::RNUInt8     TimeInForce;
    UFC::RNChar      PositionEffect;    
    UFC::RNCharArray filler;//[12];
private:        
    UFC::RNUInt8     CheckSum;
private:        
    UFC::UInt8       FRowData[128];
public:    
    TMPRX01Message()
    :msg_length( FRowData , 0 ) ///< length 89(fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< RX01: 201(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,MsgTypeExt( FRowData , 17 )
    ,preserve( FRowData , 18 )
    ,msg_time_ns( FRowData , 19 )
    ,ExecType( FRowData , 23 )
    ,cm_id( FRowData , 24 )
    ,fcm_id( FRowData , 26 )
    ,order_no( FRowData , 28, 5 )//[5];
    ,ord_id( FRowData , 33 )
    ,user_define( FRowData , 37 )
    ,symbol( FRowData , 45, 20 )//[ 20 ];
    ,Price( FRowData , 65 )
    ,qty( FRowData , 69 )
    ,investor_acno( FRowData , 71 )
    ,investor_flag( FRowData , 75 )
    ,Side( FRowData , 76 )
    ,Ordtype( FRowData , 77 )
    ,TimeInForce( FRowData , 78 )
    ,PositionEffect( FRowData , 79 )    
    ,filler( FRowData , 80, 12 )//[3];
    ,CheckSum( FRowData , 92 )    
    {
        msg_length.Set( 90 );
        MessageType.Set( 201 );
        preserve.Set( 0 );
        MsgTypeExt.Set( 2 ); ///< Text Mode        
    }
    TMPRX01Message( TMPRX01Message& Ref )
    :msg_length( FRowData , 0 ) ///< length 89(fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< RX01: 201(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,MsgTypeExt( FRowData , 17 )
    ,preserve( FRowData , 18 )
    ,msg_time_ns( FRowData , 19 )
    ,ExecType( FRowData , 23 )
    ,cm_id( FRowData , 24 )
    ,fcm_id( FRowData , 26 )
    ,order_no( FRowData , 28, 5 )//[5];
    ,ord_id( FRowData , 33 )
    ,user_define( FRowData , 37 )
    ,symbol( FRowData , 45, 20 )//[ 20 ];
    ,Price( FRowData , 65 )
    ,qty( FRowData , 69 )
    ,investor_acno( FRowData , 71 )
    ,investor_flag( FRowData , 75 )
    ,Side( FRowData , 76 )
    ,Ordtype( FRowData , 77 )
    ,TimeInForce( FRowData , 78 )
    ,PositionEffect( FRowData , 79 )    
    ,filler( FRowData , 80, 12 )//[3];
    ,CheckSum( FRowData , 92 )    
    {
        memcpy( FRowData, Ref.FRowData, 128 );
    }
    const UInt8*   GetBytes( void )        { return FRowData; }
    int            Length( void )          { return 93; } ///< 90 + msg_length(2) + CheckSum(1)
    void           Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq );    
    int            ToString( char* Buf, int Len );
    TMPMessageType GetMessageType( void )  { return tmtRX01; }
    char           GetExecType( void )     { return ExecType.Get(); }
    void           SetExecType( char tp )  { ExecType.Set( tp); }
    int            GetFCMID( void )        { return fcm_id.Get(); }
    int            GetOrdID( void )        { return ord_id.Get(); }
    void           GetOrderID( char* Out ) { order_no.Get(Out); }
    void           Recycle( void ); 
public:
    void Set_ExecType( char exectype )        { ExecType.Set(exectype); }
    void Set_cm_id( UFC::UInt16 cmid )        { cm_id.Set(cmid); }
    void Set_fcm_id( UFC::UInt16 fcmid )      { fcm_id.Set(fcmid); }
    void Set_order_no( const char* oid )      { order_no.Set( oid); }
    void Set_ord_id( UFC::UInt32 oid )        { ord_id.Set(oid); }
    void Set_user_define( UFC::UInt64 udd )   { user_define.Set(udd); }
    void Set_symbol( const char* sym)         { symbol.Set(sym); }
    void Set_Price( UFC::UInt32 px )          { Price.Set(px); }
    void Set_qty( UFC::UInt16 Qty  )          { qty.Set(Qty); }
    void Set_investor_acno( UFC::UInt32 acno ){ investor_acno.Set(acno); }
    void Set_investor_flag( char flag )       { investor_flag.Set(flag); }
    void Set_Side( UFC::UInt8 side )          { Side.Set(side); }  
    void Set_Ordtype( UFC::UInt8 ot)          { Ordtype.Set(ot); }
    void Set_TimeInForce( UFC::UInt8 tif)     { TimeInForce.Set(tif); }
    void Set_PositionEffect( char pe )        { PositionEffect.Set(pe); }   
};        
//------------------------------------------------------------------------------------------------------
class TMPRX07Message : public FastTMPMessage
{
private: ///< TMP Message header   
    UFC::RNUInt16     msg_length; ///< length 51(fixed)
    UFC::RNUInt32     MsgSeqNum;
    UFC::RNUInt32     epoch_s;
    UFC::RNUInt16     ms;
    UFC::RNUInt8      MessageType; ///< R07: 107(fixed)
    UFC::RNUInt16     fcm_id_h;    ///< fixed by session
    UFC::RNUInt16     session_id;  ///< fixed by session
private: ///< TMP Message header Ex     
    UFC::RNUInt8     MsgTypeExt; 
    UFC::RNUInt8     preserve; 
    UFC::RNUInt32    msg_time_ns;    
private: ///< TMP Message body    
    UFC::RNCharArray  order_no;//[5];
    UFC::RNUInt32     ord_id;
    UFC::RNUInt16     fcm_id;
    UFC::RNCharArray  symbol;//[ 20 ];
    UFC::RNCharArray  filler;//[ 8 ];
private:        
    UFC::RNUInt8      CheckSum;
private:        
    UFC::UInt8       FRowData[64];    
public:    
    TMPRX07Message()
    :msg_length( FRowData , 0 ) ///< length 60(fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< RX07: 207(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,MsgTypeExt( FRowData , 17 )
    ,preserve( FRowData , 18 )
    ,msg_time_ns( FRowData , 19 )    
    ,order_no( FRowData , 23 , 5)
    ,ord_id( FRowData , 28 )
    ,fcm_id( FRowData , 32 )
    ,symbol( FRowData , 34, 20 )
    ,filler( FRowData , 54, 8 )
    ,CheckSum( FRowData , 62 )    
    {
        msg_length.Set( 60 );
        MessageType.Set( 207 );
        preserve.Set( 0 );
        MsgTypeExt.Set( 2 ); ///< Text Mode
    }    
    TMPRX07Message( TMPRX07Message& Ref )
    :msg_length( FRowData , 0 ) ///< length 60(fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< RX07: 207(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,MsgTypeExt( FRowData , 17 )
    ,preserve( FRowData , 18 )
    ,msg_time_ns( FRowData , 19 )    
    ,order_no( FRowData , 23 , 5)
    ,ord_id( FRowData , 28 )
    ,fcm_id( FRowData , 32 )
    ,symbol( FRowData , 34, 20 )
    ,filler( FRowData , 54, 8 )
    ,CheckSum( FRowData , 62 )    
    {
        memcpy( FRowData, Ref.FRowData, 64 );
    }  
    const UInt8* GetBytes( void ) { return FRowData; }
    int          Length( void )   { return 63; } ///< 60 + msg_length(2) + CheckSum(1)
    void         Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq );    
    int         ToString( char* Buf, int Len );
    TMPMessageType GetMessageType( void ) { return tmtRX07; }    
    char           GetExecType( void )     { return '0'; }
    void           SetExecType( char tp )  { }
    int            GetFCMID( void )        { return fcm_id.Get(); }
    int            GetOrdID( void )        { return ord_id.Get(); }
    void           GetOrderID( char* Out ) { order_no.Get(Out); }
    void           Recycle( void ); 
public:        
    void Set_fcm_id( UFC::UInt16 fcmid )      { fcm_id.Set(fcmid); }
    void Set_order_no( const char* oid )      { order_no.Set(oid); }
    void Set_ord_id( UFC::UInt32 oid )        { ord_id.Set(oid); }
    void Set_symbol( const char* sym)         { symbol.Set(sym); }
};    
//------------------------------------------------------------------------------------------------------
class TMPRX09Message : public FastTMPMessage
{
private: ///< TMP Message header   
    UFC::RNUInt16    msg_length; ///< length 81 (fixed)
    UFC::RNUInt32    MsgSeqNum;
    UFC::RNUInt32    epoch_s;
    UFC::RNUInt16    ms;
    UFC::RNUInt8     MessageType; ///< RX09: 209(fixed)
    UFC::RNUInt16    fcm_id_h;    ///< fixed by session
    UFC::RNUInt16    session_id;  ///< fixed by session
private: ///< TMP Message header Ex     
    UFC::RNUInt8     MsgTypeExt; 
    UFC::RNUInt8     preserve; 
    UFC::RNUInt32    msg_time_ns;        
private: ///< TMP Message body   
    UFC::RNChar      ExecType;
    UFC::RNUInt16    cm_id;
    UFC::RNUInt16    fcm_id;
    UFC::RNCharArray order_no;//[5];
    UFC::RNUInt32    ord_id;
    UFC::RNUInt64    user_define;
    UFC::RNCharArray symbol;//[ 20 ];
    UFC::RNUInt32    BidPx;
    UFC::RNUInt32    OfferPx;
    UFC::RNUInt16    BidSize;
    UFC::RNUInt16    OfferSize;
    UFC::RNUInt32    investor_acno;
    UFC::RNChar      investor_flag;
    UFC::RNUInt8     TimeInForce;
    UFC::RNChar      PositionEffect;    
    UFC::RNCharArray filler;//[12];
private:        
    UFC::RNUInt8     CheckSum;
private:        
    UFC::UInt8       FRowData[128];
public:    
    TMPRX09Message()
    :msg_length( FRowData , 0 ) ///< length 94 (fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< RX09: 209(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,MsgTypeExt( FRowData , 17 )
    ,preserve( FRowData , 18 )
    ,msg_time_ns( FRowData , 19 )    
    ,ExecType( FRowData , 23 )
    ,cm_id( FRowData , 24 )
    ,fcm_id( FRowData , 26 )
    ,order_no( FRowData , 28 ,5 )//[5];
    ,ord_id( FRowData , 33 )
    ,user_define( FRowData , 37 )
    ,symbol( FRowData , 45, 20 )//[ 20 ];
    ,BidPx( FRowData , 65 )
    ,OfferPx( FRowData , 69 )
    ,BidSize( FRowData , 73 )
    ,OfferSize( FRowData , 75 )
    ,investor_acno( FRowData , 77 )
    ,investor_flag( FRowData , 81 )
    ,TimeInForce( FRowData , 82 )
    ,PositionEffect( FRowData , 83 )    
    ,filler( FRowData , 84, 12 )//[12];
    ,CheckSum( FRowData , 96 )
    {
        msg_length.Set( 94 );
        MessageType.Set( 209 );
        preserve.Set( 0 );
        MsgTypeExt.Set( 2 ); ///< Text Mode
    }
    TMPRX09Message( TMPRX09Message& Ref )
    :msg_length( FRowData , 0 ) ///< length 94 (fixed)
    ,MsgSeqNum( FRowData , 2 )
    ,epoch_s( FRowData , 6 )
    ,ms( FRowData , 10 )
    ,MessageType( FRowData , 12 ) ///< RX09: 209(fixed)
    ,fcm_id_h( FRowData , 13 )    ///< fixed by session
    ,session_id( FRowData , 15 )  ///< fixed by session
    ,MsgTypeExt( FRowData , 17 )
    ,preserve( FRowData , 18 )
    ,msg_time_ns( FRowData , 19 )    
    ,ExecType( FRowData , 23 )
    ,cm_id( FRowData , 24 )
    ,fcm_id( FRowData , 26 )
    ,order_no( FRowData , 28 ,5 )//[5];
    ,ord_id( FRowData , 33 )
    ,user_define( FRowData , 37 )
    ,symbol( FRowData , 45, 20 )//[ 20 ];
    ,BidPx( FRowData , 65 )
    ,OfferPx( FRowData , 69 )
    ,BidSize( FRowData , 73 )
    ,OfferSize( FRowData , 75 )
    ,investor_acno( FRowData , 77 )
    ,investor_flag( FRowData , 81 )
    ,TimeInForce( FRowData , 82 )
    ,PositionEffect( FRowData , 83 )    
    ,filler( FRowData , 84, 12 )//[12];
    ,CheckSum( FRowData , 96 )
    {
        memcpy( FRowData, Ref.FRowData, 128 );
    }
    const UInt8* GetBytes( void ) { return FRowData; }
    int          Length( void )   { return 97; } ///< 94 + msg_length(2) + CheckSum(1)
    void         Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq );
    int         ToString( char* Buf, int Len );
    TMPMessageType GetMessageType( void ) { return tmtR09; }
    char           GetExecType( void )     { return ExecType.Get(); }
    void           SetExecType( char tp )  { ExecType.Set( tp); }
    int            GetFCMID( void )        { return fcm_id.Get(); }
    int            GetOrdID( void )        { return ord_id.Get(); }
    void           GetOrderID( char* Out ) { order_no.Get(Out); }
    void           Recycle( void );  //Add by Zhen Fan 2021/05/20
public:
    void Set_ExecType( char exectype )        { ExecType.Set(exectype); }
    void Set_cm_id( UFC::UInt16 cmid )        { cm_id.Set(cmid); }
    void Set_fcm_id( UFC::UInt16 fcmid )      { fcm_id.Set(fcmid); }
    void Set_order_no( const char* oid )      { order_no.Set(oid); }
    void Set_ord_id( UFC::UInt32 oid )        { ord_id.Set(oid); }
    void Set_user_define( UFC::UInt64 udd )   { user_define.Set(udd); }
    void Set_symbol( const char* sym)         { symbol.Set(sym); }
    void Set_BidPx( UFC::UInt32 px )          { BidPx.Set(px); }
    void Set_OfferPx( UFC::UInt32 px )        { OfferPx.Set(px); }
    void Set_BidSize( UFC::UInt16 Qty  )      { BidSize.Set(Qty); }
    void Set_OfferSize( UFC::UInt16 Qty  )    { OfferSize.Set(Qty); }
    void Set_investor_acno( UFC::UInt32 acno ){ investor_acno.Set(acno); }
    void Set_investor_flag( char flag )       { investor_flag.Set(flag); }    
    void Set_TimeInForce( UFC::UInt8 tif)     { TimeInForce.Set(tif); }
    void Set_PositionEffect( char pe )        { PositionEffect.Set(pe); }
};        
//------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------
#endif	/* _TMPMESSAGE_H */

