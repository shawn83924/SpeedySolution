#ifndef TAIFEX_FILE_CONNECTION_OBJECT_H
#define TAIFEX_FILE_CONNECTION_OBJECT_H
#include "ConnectionObject.h"
#include "../UFC/UFC.h"
#include "Messages.h"
#include "../../inc/cch.h"
#include "../../inc/cch_reader.h"
//------------------------------------------------------------------------------
using namespace UFC;
class LinkSubSystemTAIFEX;
//------------------------------------------------------------------------------
class TAIFEXFileConnectionObject : public ConnectionObjectBase, public PThread
{
private:
	LinkSubSystemTAIFEX*  FConnection;
	ConnectionListener*   FConnectionListener;
	PCriticalSection	  FCS;
    BOOL            FNeedLock;
    BOOL            FNeedReset;
	PList			FJobs;
	BOOL			FSendMsg;
	FileStream*		FFile;
	char			FApCode;
	BOOL			FHandshakeReady;
	AnsiString		FSourceID;
	AnsiString		FDestinationID;
	AnsiString		FFileCode;
	char			FReplyType;
	Int32			FStartSequence;
	Int32			FEndSequence;
	AnsiString		FRequestID;
	BROKER_PVC_DEF  FPVCInfo;
    AnsiString		FFolderName;
    long            FIdleTime;

private:
	virtual void UpdateConnectionParameter(ConnectionParameter& NewParam);
	virtual void OnCommand( Int32 Command, void* Data );
	virtual void Execute(void);
private:
	void FileSubSystemHandshake(BOOL ReceivedL010);
	void MessageLoop(void);
	void Responser(void);
	void Initiator(void);
	void ChekcReceiveMessage(void);
        void LockLoop( void );

private:
	void ReplyConfirmMessage(FMessageHeader* InFMsg);
	void ReplyMessageF030(FMessageHeader * InFMsg);
	void ReplyMessageF040(FMessageHeader* InFMsg);
	void ReplyMessageF060(FMessageHeader* InFMsg);
	BOOL ReceiveF040(void);
	BOOL SendFMessage(FMessageHeader* FMsg);
	void SendF170CheckConnection(FMessageHeader* FMsg);

private:
	BOOL		CheckStatusCodeOfReceivedMessage(FMessageHeader* FMsg);
	Int32		CheckFileStatus(Int32* stCode);
	Int32		ReadR30Data(char* Buffer, Int32 Size);
	Int32		ReadR40Data(char* Buffer, Int32 Size);
        Int32           ReadR50Data(char* Buffer, Int32 Size);
        Int32           ReadRData(char* Buffer, Int32 Size, char ApCode);
	void		ReadFileData(char* Buffer, Int32 Size);
	void		Work(FMessageHeader* FMsg);
	void		CloseWork(void);
	void		OffLine(const AnsiString& SourceID, const AnsiString& DestinationID);
	FMessageHeader*	GetNewJob(void);

public:
	TAIFEXFileConnectionObject( ConnectionParameter& info,
								ConnectionListener* CListener,
								ThreadListener* TListener );
	virtual ~TAIFEXFileConnectionObject(void);

public:
	void AddNewJob(FMessageHeader* FMsg);
	void SetFolderName(const AnsiString& FolderName) { FFolderName = FolderName + "/"; }
        virtual void Close( void ) {};
        virtual void Active( void ) { PThread::Start(); } 
};
//------------------------------------------------------------------------------
#endif
