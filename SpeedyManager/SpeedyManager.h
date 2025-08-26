//----------------------------------------------------------------------------------------------------------------------
#ifndef __SPEEDY_MANAGER_H
#define __SPEEDY_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "../Migo/Sigo.h"
#include "../UFC/AnsiString.h"
#include "../UFC/iniFile.h"
#include "UserDB.h"
//----------------------------------------------------------------------------------------------------------------------
#define CMD_LOGIN             1
#define CMD_LOGIN_RESULT      2
#define CMD_LOGOUT            3
#define CMD_GET_CONFIG        4
#define CMD_UPDATE_CONFIG     5
#define CMD_GET_LIST          6
#define CMD_USERS_LIST        7
#define CMD_ADD_USERS         8
#define CMD_ADD_RESULT        9
#define CMD_DELETE_USER       10
#define CMD_DELETE_RESULT     11
#define CMD_MODIFY_USER       12
#define CMD_MODIFY_RESULT     13
#define CMD_MODIFY_PWD        14
#define CMD_MODIFY_PWD_RESULT 15
#define CMD_QUERY_LOG         18
#define CMD_REPLY_LOG         19
#define CMD_WRITE_LOG         20
//----------------------------------------------------------------------------------------------------------------------
#define ACTION_LOGON         0
#define ACTION_LOGON_FAILED  1
#define ACTION_LOGOUT        2
#define ACTION_NEW_USER      3
#define ACTION_DEETE_USER    4
#define ACTION_MODIFY_USER   5
#define ACTION_LOCKED        6
//----------------------------------------------------------------------------------------------------------------------
#define ERROR_VERSION_TOO_OLD           -7
#define ERROR_NOT_SUPPORT_ADMIN         -6
#define ERROR_ALREADY_LOGON             -5
#define ERROR_ACCOUNT_LOCKED            -4
#define ERROR_SPEEDY_ACCOUNT_EXPIRED    -3
#define ERROR_SPEEDY_WRONG_PASSWORD     -2
#define ERROR_ACCOUNT_NOT_EXISTS        -1
//----------------------------------------------------------------------------------------------------------------------
#define USR_ERROR_NOT_SUPPORT_ADMIN  -4
#define USR_ERROR_NOT_ADMINISTRATOR  -3
#define USR_ERROR_USR_ALREADY_EXISTS -2
#define USR_ERROR_USR_NOT_EXISTS     -1
#define RESULT_OK                     1
#define RESULT_FAILED                 0
//----------------------------------------------------------------------------------------------------------------------
class SpeedyManager : public MessageListener
{
private:
    UInt32          FLocalIP;
    int             PwdVailidateDays;
    int             PwdChangeHintDays;
    int             ErrorLockCount;
    int             LogkeepDays;
    int             MaxUserCount;
    UserDB*         Users;
    UFC::UiniFile*  GroupSetting;
    UFC::LogObject* FUserlog;
    BOOL            SupportAdmin;
    BOOL            IsAdminLogon;    
    UFC::AnsiString AdminAccount;
public:    
    UFC::UDateTime  StartupTime;
    UFC::AnsiString VersionBuidDate;
private:    
    void GetGroupName( int Index, UFC::AnsiString& GroupName  );
    void RemoveOldLogFile( void );    
    void AddGroupAttribute( Int32 Group , MTree* ReplyData );
    void LoadSetting( void );
private:    
    int  Logout( MTree* Data );
    int  Login( MTree* Data , MTree* ReplyData );
    void AddUsersListToMTree(  MTree*  ReplyList );
    void QueryLog( MTree* Data, MTree* ReplyData );
    void WriteUserlog( MTree *Data );    
    int  AddUser( MTree* Data );
    int  DeleteUser( MTree* Data );
    int  ModifyUser( MTree* Data );
    int  ModifyPwd( MTree* Data );
    void WriteUserlog( const UFC::AnsiString& Log );
public:
    int  PasswordChangeDays( )     { return PwdVailidateDays; }
    int  PasswordChangeHintDays( ) { return PwdChangeHintDays; }
    int  PasswordErrorLockTimes( ) { return ErrorLockCount; }
    SpeedyManager( );
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );        
};
//----------------------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------------------


