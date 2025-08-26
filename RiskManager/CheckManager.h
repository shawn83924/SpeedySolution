//------------------------------------------------------------------------------------------------------------------------
#ifndef __CheckManager_H
#define __CheckManager_H
//------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "AccountStore.h"
#include "RiskManager.h"
//------------------------------------------------------------------------------------------------------------------------
class CheckManager : public MessageListener
{
private:
	BOOL				FIsOption;
	AccountStore*			FAccounts;
private:
	Int32 GetOrderType( const char* Order );
	BOOL CheckBlackList( AccountRec* CheckAccount, char BS, UFC::AnsiString& Reason );
	BOOL FlowOrderCheckPass( const UFC::AnsiString& Order, UFC::AnsiString& Reason  );
public:
	CheckManager( BOOL IsOption, AccountStore* Accounts );
	virtual ~CheckManager() {};
public: /// Implement interface MessageListener
	virtual void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data );        
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
