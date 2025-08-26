//---------------------------------------------------------------------------
#ifndef UserDB_H
#define UserDB_H
//---------------------------------------------------------------------------
#include "../UFC/UFC.h"
//---------------------------------------------------------------------------
typedef enum
{
    agAdmin    = 0,
    agPowerOP  = 1,
    agOP       = 2,    
    agManager  = 3

} AccountGroup;
//---------------------------------------------------------------------------
typedef enum
{
    asOK      = 0,
    asLocked  = 1,
    asExpired = 2
            
} AccountState;
//---------------------------------------------------------------------------
class User
{
public:
    UFC::AnsiString FID;
    UFC::AnsiString FPWD;
    AccountGroup    FGroup;   ///< 0:Admin 1:PowerOP 2:OP 3:Manager
    AccountState    FState;   ///< 0:OK    1:Locked  2:Expired
    int             FErrCount;
    UFC::UDate      FExpDate;
    UFC::UDate      FPwdChangeDate;    
    int             FPwdVailidateDays; 
public:
    User( int PwdVailidateDays, const UFC::AnsiString& ID/* = "Admin"*/ );
    UFC::AnsiString& GetID(){ return FID; }
    UFC::AnsiString& GetPwd(){ return FPWD; }
    void SetPwd( UFC::AnsiString& NewPwd );

    AccountGroup  GetGroup() { return FGroup; }
    BOOL SetGroup( AccountGroup NewGroup, AccountGroup& OldGroup );

    AccountState  GetState() { return FState; }
    BOOL SetState( AccountState NewState );
    BOOL SetState( AccountState NewState, UFC::AnsiString& Action );

    int  GetErrCount() { return FErrCount; }
    void SetErrCount( int ErrCount ) { FErrCount = ErrCount; }
    void LoadFromStream( UFC::PStream* Stream );
    void SaveToStream( UFC::PStream* Stream );
    void ToString( UFC::AnsiString& Str );
    int  ExpLeavesDay( void );
    bool ValidUser( void );
};
//---------------------------------------------------------------------------
class UserDB
{
private:
    int                FMaxCount;
    UFC::AnsiString    FFileName;
    UFC::PtrList<User> FUserList;
    int                FPwdVailidateDays;
private:
    BOOL          UserExists( UFC::AnsiString& Id );
    int           AdminCount();
public:
    UserDB( int PwdVailidateDays, int MaxUser = 20 );
    ~UserDB( );
    int   GetCount()            { return FUserList.ItemCount(); };
    User* GetUser( int index )  { return FUserList.GetItem(index); };
    User* FindUser( const UFC::AnsiString& UserID );
    ///< Return 0: No more user.
    ///< Return -1: User already exists.
    ///< Return -2: Error.
    ///< Return -3: Can't delete the last user.
    ///< Return 1 ~ MaxUser: User count now.
    int   Add( User* NewUser );
    ///< Return 0: User not exists.
    ///< Return -1: Can't delete the last Admin.
    ///< Return -2: Can't delete the last user.
    ///< Return -3: Can't delete the last user.
    ///< Return 1 ~ MaxUser: User count now.
    int   Delete( UFC::AnsiString& UserID );
    void  Update();
};
//---------------------------------------------------------------------------
#endif // Unit1_H
//---------------------------------------------------------------------------
