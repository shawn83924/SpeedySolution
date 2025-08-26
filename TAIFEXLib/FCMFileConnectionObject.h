#ident "@(#) $Id: FCMFileConnectionObject.h,v 1.4 2004/05/26 04:30:30 ap27 Exp $"
#ifndef __FCM_FILE_CONNECTION_OBJECT_H
#define __FCM_FILE_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "ConnectionObject.h"
#include "../UFC/UFC.h"
//-----------------------------------------------------------------------------------------------------------------------
//   FCM Side: Connection Object for AP code 1,2
//-----------------------------------------------------------------------------------------------------------------------
class FCMFileConnectionObject : public ConnectionObjectBase, public SocketClientListener, public PThread
{
private:
	BOOL                 FHandshakeReady;
	BOOL                 FPVCLock;
	bool                 FSendMsg;
	BOOL                 FRequest;
	BOOL 	             FBroken;
	char                 FAPCode;
	AnsiString           FIPAddress;
	Int32                FPort;
	Int32                FSeqNumber;
	FileData*            FFileData;
	LinkSubSystemFCM*    FConnection;
	ConnectionListener*  FConnectionListener;
	PClientSocket*       FSocketClient;
	FileStream*          FActionFile;
	FileStream*          FDataFile;
	virtual void Execute( void );
private:
	void ConnectToServer( );
	void SendSocketMessage( BOOL IsSend, MessageHeader* Msg );
	BOOL SendPackage( unsigned char CMD, int Size = 0, void *Data = NULL );
	// Implement interface SocketClientListener
        virtual void OnConnect( PClientSocket * Socket );
        virtual void OnDisconnect( PClientSocket * Socket ) {}
        virtual BOOL OnDataArrived( PClientSocket * Socket );
private:
	void MessageLoop( void );
	void ReplyConfirmMessage( FMessageHeader* InFMsg );
	void ReplyMessageF040( FMessageHeader* InFMsg );
	void SendMessageF030( FMessageHeader* InFMsg );
	void SendMessageF070( FMessageHeader* InFMsg );
	void SendFile( char* FileCode, char* FileName );
	void SendRequest( );
private:
	void ChekcReceiveMessage();
	void ShowMessage( bool IsSend, FMessageHeader* InFMsg );
	void FileSubSystemHandshake( BOOL ReceivedL010 );
public:
	FCMFileConnectionObject( ConnectionParameter& info,
		                     ConnectionListener*  Listener,
		                     ThreadListener*      TListener,
		                     const AnsiString& IPAddress = "",
		                     Int32	          Port = 0 );
	virtual ~FCMFileConnectionObject();
public:
//	void SendF050( char* FileCode, char* RequestData );
public:
	virtual void OnCommand( int , void* ){}
	virtual void Close( void ){}
	virtual void UpdateConnectionParameter( ConnectionParameter& ){}
};
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------

