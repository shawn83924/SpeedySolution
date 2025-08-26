//------------------------------------------------------------------------------------------------------------------------
#ifndef __AccountStore_H
#define __AccountStore_H
//------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../UFC/Utility.h"
//------------------------------------------------------------------------------------------------------------------------
typedef enum
{
    lsNone = 0,
    lsBuy  = 1,
    lsSell = 2,
    lsBoth = 3,
    lsUnknown = 4

} LockState;
//------------------------------------------------------------------------------------------------------------------------
class AccountStore;
//------------------------------------------------------------------------------------------------------------------------
// class AccountRec
//------------------------------------------------------------------------------------------------------------------------
class AccountRec
{
friend class AccountStore;
private:
	static UFC::PCriticalSection FStateCS;
private:
	UFC::AnsiString FAccount;
	char            FFlag;
	BOOL            FCanBuy;
	BOOL            FCanSell;
	Int32           FPosition;
private:
	AccountRec( ){}
	AccountRec( UFC::PStream* Stream );
	AccountRec( const UFC::AnsiString& ID, char Flag, UFC::PStream* Stream );
public:
	void		SetState( LockState );
	LockState	GetState( void );
	void		SaveToStream( UFC::PStream* Stream );
	void		LoadFromStream( UFC::PStream* Stream );
	char		GetFlag() { return FFlag; }
	const UFC::AnsiString& GetAccount() { return FAccount; }
};
//------------------------------------------------------------------------------------------------------------------------
// class AccountStore
//------------------------------------------------------------------------------------------------------------------------
class AccountStore : public UFC::SocketServerListener
{
private:
	static UFC::AnsiString SHandShakeName;
	static UFC::AnsiString SConfigFilePath;
	static UFC::AnsiString SSyncFilePath;
private: /// Implement interface SocketServerListener
	void OnListen( UFC::MultiplexingServer* ServSocket );
	void OnClientConnect( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket );
	void OnClientDisconnect( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket );
	BOOL OnClientWrite( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket );	
        BOOL OnHeartbeat( UFC::PClientSocket* pSocket, BOOL IsRead  )  { return TRUE; }
	void OnServerIdle(UFC::MultiplexingServer* ServSocket ){ }
protected:
	UFC::AnsiString					FThreadName;
	UFC::PCriticalSection				FHashTableCS;
	UFC::PHashedList<UFC::AnsiString,AccountRec*>	FAccounts;
	UFC::FileStreamEx*				FDataFile;
	UFC::MultiplexingServer				FTCPServer;
public:
	///< Add an account.
	AccountRec*	AddAccount( const UFC::AnsiString& ID, char Flag );
	AccountRec*	GetAccount( const UFC::AnsiString& ID );
	char		GetAccountFlag( const UFC::AnsiString& ID );
	char		GetDayTradePolicy( const UFC::AnsiString& ID );
	void		UpdateAccount( AccountRec* UpdateAccount );
	void		Flush( void ) { FDataFile->Flush(); }
private:
	void		ExecuteRecover( void );
	void		Initialize( void );
public:
	LockState	SetState( const UFC::AnsiString& ID, LockState State );
	LockState	GetState( const UFC::AnsiString& ID );
public:
	AccountStore( UFC::AnsiString ThreadName, BOOL NeedRecover );
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
