#ifndef _FMESSAGES_H
#define _FMESSAGES_H
//---------------------------------------------------------------------------
#include "TFXMessageHeader.h"
//-----------------------------------------------------------------------------------------
namespace TFX
{
//---------------------------------------------------------------------------
#define FMESSAGE_HEADER_SIZE 32
//---------------------------------------------------------------------------
class FMessageHeader : public MessageHeader
{
	private :
		char FSourceID[7];
		char FDestinationID[7];
		char FMessageLength[4];
	public :
		FMessageHeader( const char* SourceID, const char* DestinationID, int MessageLength, const char* SubSys, const char* Func, const char* MsgType, const char* Status = "00" );
		FMessageHeader( char* Data );

		void  SetSourceID( char* ID );
		void  SetDestinationID( char* ID );
		void  SetMessageLength( int Len );
		void  SwapID( void );

		UFC::AnsiString GetSourceID()        { return UFC::AnsiString( FSourceID, 7 ); }
		UFC::AnsiString GetDestinationID()   { return UFC::AnsiString( FDestinationID, 7 ); }
		UFC::AnsiString GetMessageLengthStr(){ return UFC::AnsiString( FMessageLength, 4 ); }
		int   GetMessageLength();

		virtual void AsString( char* Buffer );
        virtual void print( void );
    public :
        static int GetDefMsgLen( char* FileCode );

};
//---------------------------------------------------------------------------
class MessageF010 : public FMessageHeader
{
	private :
		char FFileCode[3];
		char FFileSize[8];
	public :
		MessageF010(  char* SourceID,  char* DestinationID,  char* FileCode, int FileSize );
		MessageF010( char* Data );

		UFC::AnsiString GetFileCode()   { return UFC::AnsiString( FFileCode, 3 ); }
		UFC::AnsiString GetFileSizeStr(){ return UFC::AnsiString( FFileSize, 8 );  }
		int   GetFileSize();

		virtual void AsString( char* Buffer );
		virtual void print( void )
		{
			SocketPrintf("----------------- Send Start Message  --------------------\n" );
		}
};
//---------------------------------------------------------------------------
class MessageF020 : public FMessageHeader
{
	private :
		char FFileCode[3];
		char FFileSize[8];
	public :
		MessageF020(  char* SourceID,  char* DestinationID,  char* FileCode, int FileSize );
		MessageF020( char* Data );

		UFC::AnsiString GetFileCode()   { return UFC::AnsiString( FFileCode, 3 ); }
		UFC::AnsiString GetFileSizeStr(){ return UFC::AnsiString( FFileSize, 8 );  }
		int   GetFileSize();

		virtual void AsString( char* Buffer );
		virtual void print( void )
		{
			SocketPrintf("---------------- Receive Start Message -------------------\n" );
		}
};
//---------------------------------------------------------------------------
class MessageF030 : public FMessageHeader
{
	private :
		char FFileCode[3];
		char FEOF; ///< Last package 1 others 0
		char FData[ 988 ];
	public :
		MessageF030(  char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode, char FileEOF, char* Data );
		MessageF030( char* Data );

		UFC::AnsiString GetFileCode(){ return UFC::AnsiString( FFileCode, 3 ); }
		//char  GetEOFStr()  { return FEOF; }
		char  GetEOF()     { return FEOF; }
		char* GetData()    { return FData; }

		virtual void AsString( char* Buffer );
        virtual void print( void );
};
//---------------------------------------------------------------------------
class MessageF040 : public FMessageHeader
{
	private :
		char FFileCode[3];
		char FEOF;
	public :
		MessageF040(  char* SourceID,  char* DestinationID,  char* FileCode, char DataEOF );
		MessageF040( char* Data );

		UFC::AnsiString GetFileCode(){ return UFC::AnsiString( FFileCode, 3 ); }
		//char  GetEOFStr()  { return FEOF; }
		char  GetEOF()     { return FEOF; }
		virtual void AsString( char* Buffer );
        virtual void print( void );
};
//---------------------------------------------------------------------------
class MessageF050 : public FMessageHeader
{
	private :
		char FFileCode[3];
		char FRequest_Message[989]; //221 bytes for Future
		char FReplyType;
		int  FStartSequence;
		int  FEndSequence;
		char FRequestID[7];
	private :
		void ReadRequestMessage(void);
	public :
                MessageF050(  char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode,  char* Request_Message );
                MessageF050(  char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode,  int StartSeq, int EndSeq );
                MessageF050(  char* SourceID,  char* DestinationID, const UFC::AnsiString& Data  );
                MessageF050(  char* Data );

		UFC::AnsiString GetFileCode()	{ return UFC::AnsiString( FFileCode, 3 ); }
		UFC::AnsiString GetRequest_Message()	{ return UFC::AnsiString( FRequest_Message, GetMessageLength()); }
		char            GetReplyType()		{ return FReplyType; }
		int             GetStartSequence()	{ return FStartSequence; }
		int             GetEndSequence()	{ return FEndSequence; }
		UFC::AnsiString GetRequestID()	        { return UFC::AnsiString( FRequestID, 7 ); }

		virtual void AsString( char* Buffer );
        virtual void print( void );
};
//---------------------------------------------------------------------------
class MessageF060 : public FMessageHeader
{
	private :
		char FFileCode[3];
		char FRequest_Message[989]; //221 bytes for Future
	public :
		MessageF060(  char* SourceID,  char* DestinationID, int MessageLength,  char* FileCode, char* Request_Message );
		MessageF060( char* Data );

		UFC::AnsiString GetFileCode()       { return UFC::AnsiString( FFileCode, 3 ); }
		char* GetRequest_Message(){ return FRequest_Message; }

		virtual void AsString( char* Buffer );
        virtual void print( void );
};
//---------------------------------------------------------------------------
class MessageF070 : public FMessageHeader
{
	public :
		MessageF070(  char* SourceID,  char* DestinationID );
		MessageF070( char* Data ):FMessageHeader( Data ){}

		virtual void print( void )
		{
			SocketPrintf("----------------- Send End Message  --------------------\n" );
		}
};
//---------------------------------------------------------------------------
class MessageF080 : public FMessageHeader
{
	public :
		MessageF080(  char* SourceID,  char* DestinationID );
		MessageF080( char* Data ):FMessageHeader( Data ){}

		virtual void print( void )
		{
			SocketPrintf("---------------- Receive End Message -------------------\n" );
		}
};
//---------------------------------------------------------------------------
class MessageF170 : public FMessageHeader
{
	public :
		MessageF170(  char* SourceID,  char* DestinationID );
		MessageF170( char* Data ):FMessageHeader( Data ){}

		virtual void print( void )
		{
			SocketPrintf("---------------- Send Confirm Link Message -------------------\n" );
		}
};
//---------------------------------------------------------------------------
class MessageF180 : public FMessageHeader
{
	public :
		MessageF180(  char* SourceID,  char* DestinationID );
		MessageF180( char* Data ):FMessageHeader( Data ){}

		virtual void print( void )
		{
			SocketPrintf("---------------- Receive Confirm Link Message -------------------\n" );
		}
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif

