#ifndef __TWSE_LMESSAGES_H
#define __TWSE_LMESSAGES_H
//---------------------------------------------------------------------------
#include "MessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//---------------------------------------------------------------------------
// Begin Link Sub-System messages.
// ***** for Stock system *****
//---------------------------------------------------------------------------
class MessageL010 : public MessageHeader
{
   public:
        MessageL010( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"10":"91"),"10","00"){}
        MessageL010( const char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("--------------- Wake up message L010 -------------------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageL020 : public MessageHeader
{
   public:
        MessageL020( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"10":"91"),"10","01"){}
        MessageL020( const char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("----------- Confirm wake up message L020 ---------------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageL030 : public MessageHeader
{
   private:
   		char AppendNo[3];
   public:
        MessageL030( BOOL IsTWSE, int AppNo );
        MessageL030( const char* Data );
        UFC::AnsiString GetAppendNo();
        virtual void AsString( char* Buffer );
	virtual void print( void )
        {
        	SocketPrintf("----------- Register notice message L030 ---------------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageL040 : public MessageHeader
{
    private:
    	char AppendNo[3];
	    char BrokerID[4];
	    char APCode;
	    char KeyValue[2];
    public:
        MessageL040( BOOL IsTWSE, char* AppendN, char* Broker, char AP, char* Password );
        MessageL040( const char* Data );
        UFC::AnsiString GetAppendNo( void );
        UFC::AnsiString GetBrokerID( void );
        UFC::AnsiString GetKeyValue( void );
        char	   GetAPCode( void ){ return APCode; }
        virtual void print( void );
        virtual void AsString( char* Buffer );
};
//---------------------------------------------------------------------------
class MessageL050 : public MessageHeader
{
   public:
        MessageL050( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"10":"91"),"20","04"){}
        MessageL050( const char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("----------- Subsystem startup message L050 -------------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageL060 : public MessageHeader
{
   public:
        MessageL060( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"10":"91"),"20","05"){}
        MessageL060( const char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("------- Confirm subsystem startup message L060 ---------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageL070 : public MessageHeader
{
   public:
        MessageL070( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"10":"91"),"30","06"){}
        MessageL070( const char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("----------------- End message L070 ---------------------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageL080 : public MessageHeader
{
   public:
        MessageL080( BOOL IsTWSE ):MessageHeader((char*)((IsTWSE)?"10":"91"),"30","07"){}
        MessageL080( const char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
			SocketPrintf("------------- Confirm end message L080 -----------------\n" );
        }
};
//---------------------------------------------------------------------------
// End Link Sub-System messages.
//-----------------------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------

