//----------------------------------------------------------------------------------------------------------------------
#ifndef __TWSE_MESSAGEHEADER_H
#define __TWSE_MESSAGEHEADER_H
//----------------------------------------------------------------------------------------------------------------------
#define MESSAGE_HEADER_SIZE 14
//----------------------------------------------------------------------------------------------------------------------
#include "../UFC/AnsiString.h"
#include "../UFC/PThread.h"
#include "../ExchangeUtility/Utility.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    lsNormal        = 0,        
    lsPost          = 1,
    lsOdd           = 2,
    lsAuction       = 3,
    lsNegotiatePx   = 4,
    lsLend          = 5,        
    lsTender        = 6,    
    lsTenderEx      = 7,
    lsOddEx         = 8,
    lsUnknown       = 9
        
} TSubSys;  
//------------------------------------------------------------------------------
typedef enum
{
    sBuy             = 'B',
    sSell            = 'S',

} TSESideEnum;
//----------------------------------------------------------------------------
typedef enum
{
    otNormal          = '0',
    otSellShort       = '5',
    otSellShortExempt = '6'
            
} TSEOrderTypeEnum;
//----------------------------------------------------------------------------
typedef enum
{    
    ptMarket   = '1',
    ptLimit    = '2'
            
} TSEPriceTypeEnum;
//----------------------------------------------------------------------------
typedef enum
{    
    tifROD   = '0',
    tifIOC   = '3',
    tifFOK   = '4'
            
} TSETimeInForceEnum;
//----------------------------------------------------------------------------------------------------------------------
typedef enum
{
    mtUnknown,   // Unknown message
    mtL010,
    mtL020,
    mtL030,
    mtL040,
    mtL050,
    mtL060,
    mtL070,
    mtL080,   // Link sub-system messages.
    mtT010,
    mtT020,
    mtT030,
    mtT040,
    mtT050,
    mtT060,  // Transection sub-system messages.
    mtO010,
    mtO020,
    mtO030,
    mtO040,
    mtO060,
    mtO070,
    mtO080,
    mtO090, // Odd Transection sub-system messages.
    mtP010,
    mtP020,
    mtP030,
    mtP040,
    mtP050,
    mtP060,
    mtP070,
    mtP080, // Post Transection sub-system messages.
    mtA010,
    mtA020,
    mtA030,
    mtA040,
    mtA050, // 拍賣 TargetSubID = “5” Auction
    mtA060, // Auction  Transection sub-system messages.
    mtE010,
    mtE020,
    mtE030,
    mtE040,
    mtE050, // 標購 TargetSubID = “6” Reverse Auction
    mtE060, // Tender Transection sub-system messages.
    mtEx010,
    mtEx020,
    mtEx030,
    mtEx040,
    mtEx050, // 證金標購 TargetSubID = “B” Reverse Auction by Securities Finance 
    mtEx060, // Tender Ex Transection sub-system messages.    
    mtV010,
    mtV020,
    mtV030,
    mtV040,
    mtV050, // 標借 TargetSubID = “4” Lending Auction
    mtV060, // Lend Transection sub-system messages.
    mtR1,
    mtR2,
    mtR3,
    mtR4,
    mtR5,
    mtR6, // Transection confirm sub-system messages.
    mtF010,
    mtF020,
    mtF030,
    mtF040,
    mtF050,
    mtF060,
    mtF070,
    mtF080,
    mtF170,
    mtF180,
    mtF210,
    mtF220,// Transection file sub-system messages.(for both TSE and OTC)
    mtS010,
    mtS020,
    mtS030,
    mtS040,
    mtS050,
    mtS060,
    mtS070,
    mtS080,
    mtS090,
    mtS100,
    mtS110,
    mtS120,
    mtS130,
    mtS140,
    mtS150,
    mtS160,// Negotiate Px sub-system messages.(OTC only)   
    mtO110,
    mtO120,
    mtO130,
    mtO140,
    mtO150,
    mtO160  // Odd Transection sub-system messages.            

} TMessageType;
//----------------------------------------------------------------------------------------------------------------------
class RMOrderInterface
{
public:    
    virtual ~RMOrderInterface( void ){}
    virtual const char*        GetSymbol( void ) = 0;
    virtual const char*        GetOrderID( void ) = 0;
    virtual const char*        GetBrokerID( void ) = 0;
    virtual const char*        GetAccount( void ) = 0;
    virtual TSESideEnum        GetSide( void ) = 0;
    virtual TSEOrderTypeEnum   GetTWSEOrderType( void ) = 0;
    virtual TSEPriceTypeEnum   GetOrderType( void ) { return ptLimit; }
    virtual TSETimeInForceEnum GetTimeInForce( void ) { return tifROD; }
    virtual double             GetOrderPrice( void ) = 0;
    virtual int                GetOrderQty( void ) = 0;
};    
//--------------------------------------------------------------------------
class MessageT010;
//--------------------------------------------------------------------------
class TWSEOrderObjectPool  : public UFC::PThread
{
private:
    queue<MessageT010*>   FQ;     
    UFC::PCriticalSection FCS;
    size_t                FPoolSize;
    int                   FNewPer10ms;    
    int                   FPoolSizeLow;
private:   
    void Execute( void );     
public:
    TWSEOrderObjectPool( size_t PoolSize = 32768, int NewObjectPerSec = 1024 );
    MessageT010* GetInstance( void );
};	
//----------------------------------------------------------------------------------------------------------------------
class MessageHeader
{
private: // Data member
    char FSystemName[3]; /// 2
    char FFunctionCode[3];/// 2
    char FMessageType[3];/// 2
    char FMessageTime[7];/// 6
    char FStatusCode[3]; /// 2
public:  // Virtual functions and constructor, distructor
    MessageHeader( const char* SubSys, const char* Func, const char* MsgType, const char* Status = "00" );
    MessageHeader( const char* Data );
    MessageHeader( void );
    virtual ~MessageHeader( void ){}
virtual void AsString( char* Buffer );
    virtual void print( void );
public:  // Access member functions.
    int   GetSubSystem();
    int   GetFunctionCode();
    int   GetMessageType();
    int   GetStatusCode();
    char* GetSubSystemStr()   { return FSystemName;  }
    char* GetFunctionCodeStr(){ return FFunctionCode;}
    char* GetMessageTypeStr() { return FMessageType; }
    char* GetStatusCodeStr()  { return FStatusCode;  }
    char* GetTimeStr()        { return FMessageTime;}
    void  SetStatusCode( int Code );
    void  SetStatusCodeStr( char* Code );
    void  SetFunctionCode( int Code );
    void  SetMessageType( int Type );
    void  UpdateTime();
    void  LoadHeader( const char* Data );
public:  // Main functions
    TMessageType MessageType( void );
    BOOL  IsMessage( TMessageType Msg );
private: // Private static functions
    static TMessageType GetLinkSubsystemMessage( int FuncCode, int MsgType );
    static TMessageType GetFileTransferSubsystemMessage( int FuncCode, int MsgType );
    static TMessageType GetTransactionSubsystemMessage( int FuncCode, int MsgType );
    static TMessageType GetOddTransactionSubsystemMessage( int FuncCode, int MsgType );
    static TMessageType GetOddExTransactionSubsystemMessage( int FuncCode, int MsgType );
    static TMessageType GetPostTransactionSubsystemMessage( int FuncCode, int MsgType );
    static TMessageType GetAuctionTransactionSubsystemMessage( int FuncCode, int MsgType );
    static TMessageType GetLendTransactionSubsystemMessage( int FuncCode, int MsgType );
    static TMessageType GetTenderTransactionSubsystemMessage( int FuncCode, int MsgType );
    static TMessageType GetTenderExTransactionSubsystemMessage( int FuncCode, int MsgType );
    static TMessageType GetConfirmSubsystemMessage( int FuncCode, int MsgType );
    static TMessageType GetNegotiatePxTransactionSubsystemMessage( int FuncCode, int MsgType );
    static MessageHeader* CreateInstance( TMessageType Type, const char* Data );
    static int GetMessageLength( TMessageType Type );
    static TWSEOrderObjectPool* T010PoolPtr;    
public:
    static TSubSys        GetTradingSession( int SubSystem );
    static TMessageType   GetMessageType( const char* Data );
    static MessageHeader* CreateMessage( const char* Data );
    static MessageHeader* CreateMessage( const char* Data, int Length );
    static BOOL           IsMessage(  char* Data, TMessageType Msg );
    static void           SetNewVersion( BOOL IsNew, BOOL IsNewEx );
    static BOOL           IsNewVersion( void );
    static BOOL           IsNewExVersion( void );
    static void           Initialize( void );
    static MessageT010* FastT010( const char* Data );
public:
    static BOOL           FUseExtFT;
    static BOOL           FNewVersion;
    static BOOL           FNewExVersion;
    static void           SetUseExtFT( BOOL UseExtFT ){ FUseExtFT = FUseExtFT; }
};
//----------------------------------------------------------------------------------------------------------------------
typedef  MessageHeader* MessageHeaderPtr;
typedef enum
{
    smUnknown,  ///< Unknown message.
    smSLM010,   ///< Confirm Connected message.
    smSLM020,   ///< TMP Socket message.
    smSLM030,   ///< Heartbeat message.
    smSLM040    ///< Error message

} TSocketMessageType;
//----------------------------------------------------------------------------------------------------------------------
class SocketMessage
{
    char  HeaderCode[2];
    char  ConrolCode[2];
    Int16 MessageLength;

};
//----------------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
