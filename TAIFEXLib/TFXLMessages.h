#ifndef __LMESSAGES_H
#define __LMESSAGES_H
//---------------------------------------------------------------------------
#include "TFXMessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
//---------------------------------------------------------------------------
// Begin Link Sub-System messages.
// ***** for both Futrues and Options system *****
//---------------------------------------------------------------------------
class MessageL010 : public MessageHeader
{
   public:
        MessageL010( ):MessageHeader("10","10","00"){}
        MessageL010( char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("--------------- Wake up message L010 -------------------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageL020 : public MessageHeader
{
   public:
        MessageL020( ):MessageHeader("10","10","01"){}
        MessageL020( char* Data ):MessageHeader( Data ){}
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
        MessageL030( int AppNo );
        MessageL030( char* Data );
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
	    char BrokerID[7];
	    char APCode;
	    char KeyValue[2];
    public:
        MessageL040( char* AppendN, char* Broker, char AP, char* Password );
        MessageL040( char* Data );
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
        MessageL050( ):MessageHeader("10","20","04"){}
        MessageL050( char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("----------- Subsystem startup message L050 -------------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageL060 : public MessageHeader
{
   public:
        MessageL060( ):MessageHeader("10","20","05"){}
        MessageL060( char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("------- Confirm subsystem startup message L060 ---------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageL070 : public MessageHeader
{
   public:
        MessageL070( ):MessageHeader("10","30","06"){}
        MessageL070( char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
        	SocketPrintf("----------------- End message L070 ---------------------\n" );
        }
};
//---------------------------------------------------------------------------
class MessageL080 : public MessageHeader
{
   public:
        MessageL080( ):MessageHeader("10","30","07"){}
        MessageL080( char* Data ):MessageHeader( Data ){}
        virtual void print( void )
        {
			SocketPrintf("------------- Confirm end message L080 -----------------\n" );
        }
};
//---------------------------------------------------------------------------
// End Link Sub-System messages.
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
