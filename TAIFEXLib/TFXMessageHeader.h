//----------------------------------------------------------------------------------------------------------------------
#ifndef __MESSAGEHEADER_H
#define __MESSAGEHEADER_H
//----------------------------------------------------------------------------------------------------------------------
#define MESSAGE_HEADER_SIZE 14
//----------------------------------------------------------------------------------------------------------------------
#include "../UFC/AnsiString.h"
#include "../ExchangeUtility/Utility.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
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
	mtL080,   // Link sub-system messages( for both Options and Futures)
	mtR010,
	mtR020,
	mtR030,
	mtR040,
	mtR050,
	mtR060,
	mtR070,
	mtR080,
	mtR090,
	mtR100,   // Transection sub-system messages.(for Options)
	mtC010,
	mtC020,
	mtC030,
	mtC040,
	mtC050,
	mtC060,
	mtC011,
	mtC021,
	mtC031,
	mtC032,	  // Transection confirm sub-system messages.(for Options)
	mtF010,
	mtF020,
	mtF030,
	mtF040,
	mtF050,
	mtF060,
	mtF070,
	mtF080,
	mtF170,
	mtF180,   // Transection file sub-system messages.(for both Options and Futures)

} TMessageType;
//----------------------------------------------------------------------------------------------------------------------
class MessageHeader
{
    private: // Data member
    	char FSystemName[2];
        char FFunctionCode[2];
        char FMessageType[2];
        char FMessageTime[6];
        char FStatusCode[2];
	public:  // Virtual functions and constructor,distructor
        MessageHeader( const char* SubSys, const char* Func, const char* MsgType, const char* Status = "00" );
        MessageHeader( char* Data );
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
    public:  // Main functions
        TMessageType MessageType( void );
        BOOL  IsMessage( TMessageType Msg );
    private: // Private static functions
        static TMessageType GetLinkSubsystemMessage( int FuncCode, int MsgType );
        static TMessageType GetFileTransferSubsystemMessage( int FuncCode, int MsgType );
        static TMessageType GetTransactionSubsystemMessage( int FuncCode, int MsgType );
        static TMessageType GetConfirmSubsystemMessage( int FuncCode, int MsgType );
        static MessageHeader* CreateInstance( TMessageType Type, char* Data );
        static int GetMessageLength( TMessageType Type );
    public:
    	static TMessageType   GetMessageType( char* Data );
    	static MessageHeader* CreateMessage( char* Data );
    	static MessageHeader* CreateMessage( char* Data, int Length );
    	static BOOL           IsMessage(  char* Data, TMessageType Msg );
};
//----------------------------------------------------------------------------------------------------------------------
typedef  MessageHeader* MessageHeaderPtr;
//-----------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------
