#include "SpeedyManager.h"
#include "../Version.h"
//----------------------------------------------------------------------------------------------------------------------
extern MessageObject* MessageObj;
//----------------------------------------------------------------------------------------------------------------------
SpeedyManager::SpeedyManager()
:FLocalIP( UFC::PSocket::GetLocalIPAddress())
,PwdVailidateDays( 90 ) ///< You should change password every "PwdVailidateDays" days.
,PwdChangeHintDays( 15 )///< SpeedyCenter will give you a tip to change password "PwdChangeHintDays" days before password expired.
,ErrorLockCount( 3 )    ///< Max password error count.
,LogkeepDays( 60 )      ///< Keep user log for "LogkeepDays" days.
,MaxUserCount( 20 )     
,SupportAdmin( TRUE )
,IsAdminLogon( FALSE )
{    
    FUserlog = new UFC::BufferedLog( "../Userlog/"+UFC::GetDateString()+".log", 102400, TRUE );   
    LoadSetting();    
    RemoveOldLogFile();
    Users = new UserDB( PwdVailidateDays, MaxUserCount );
    VersionBuidDate.Printf( "Ver:%s Build Date:%s", SPEEDY_MANAGER_VER, __DATE__ );
    UFC::BufferedLog::Printf( "____________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   SpeedyManager,User Management Module   " );
    UFC::BufferedLog::Printf( "   Copyright 2003~%d by MDBS Software Inc.",UFC::ThisYear() );
    UFC::BufferedLog::Printf( "   All right reserved.                      ");
    UFC::BufferedLog::Printf( "   Startup at: %s.", UFC::Hostname );
    UFC::BufferedLog::Printf( "   %s ", VersionBuidDate.c_str() );		
    UFC::BufferedLog::Printf( "   %d bit mode                          ", sizeof(void*)*8 );            
    UFC::BufferedLog::Printf( "    " );		
    UFC::BufferedLog::Printf( "   Password expire days:%d", PwdVailidateDays );
    UFC::BufferedLog::Printf( "   Hint password expire days:%d ", PwdChangeHintDays );
    UFC::BufferedLog::Printf( "   Password error count:%d ", ErrorLockCount );
    UFC::BufferedLog::Printf( "   Keep user log days:%d ", LogkeepDays );
    UFC::BufferedLog::Printf( "   Local IP:%d.%d.%d.%d", FLocalIP>>24,(FLocalIP<<8)>>24,(FLocalIP<<16)>>24,(FLocalIP<<24)>>24 );    
    UFC::BufferedLog::Printf( "____________________________________________" );
    StartupTime.setCurrent( );         
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyManager::GetGroupName( int Index, UFC::AnsiString& GroupName  )
{
    UFC::AnsiString Id( Index );

    if( GroupSetting->GetValue("GroupNames",Id, GroupName ) == FALSE )
    {
        if( Index == 0 )
            GroupName = "Admin";
        else if( Index == 1 )
            GroupName = "Power User";
        else if( Index == 2 )
            GroupName = "Operator";
        else
            GroupName.Printf( "Group%d",Index );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyManager::WriteUserlog( MTree *Data )
{
    UFC::AnsiString Account,Type,Msg,logStr;

    if( Data->get( "ID", Account ) && Data->get( "TYPE", Type ) && Data->get( "MSG", Msg ))
    {        
        logStr.Printf( "%s,%s,%s", Account.c_str(),Type.c_str(),Msg.c_str());                
        WriteUserlog( logStr );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyManager::WriteUserlog( const UFC::AnsiString& Log )
{
    UFC::AnsiString logStr,Time;    
    
    UFC::GetTimeString( Time );
    logStr.Printf("%s,%s", Time.c_str(), Log.c_str() );        
    FUserlog->WriteString( logStr );    
    FUserlog->Flush( );
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyManager::RemoveOldLogFile( void )
{
     UFC::UDate Day60Ago;
     UFC::AnsiString DelFile;

     Day60Ago.setCurrent();
     Day60Ago -= LogkeepDays;
     DelFile.Printf( "../Userlog/%02d%02d%02d.log",Day60Ago.getYear()-1911,Day60Ago.getMonth(),Day60Ago.getDate());
     unlink( DelFile.c_str() );
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyManager::QueryLog( MTree* Data, MTree* ReplyData )
{
   UFC::AnsiString LogFileName,Account,DateStr;
   Int32 BY,BM,BD,EY,EM,ED;

   if( Data->get( "ID", Account ) &&
       Data->get( "BY", BY) && Data->get( "BM", BM ) && Data->get( "BD", BD) &&
       Data->get( "EY", EY) && Data->get( "EM", EM ) && Data->get( "ED", ED))
   {
       UFC::UDate SearchDate( BD, BM, BY );
       UFC::UDate EndDate( ED, EM, EY );
       char  Buffer[ 512 ];
       FILE* FFileHandle;
       UFC::MemoryStream LogStream;

       while( SearchDate <= EndDate )
       {
          UFC::AnsiString  DateStr,ResultStr;          

          LogFileName.Printf( "../Userlog/%02d%02d%02d.log",SearchDate.getYear()-1911,SearchDate.getMonth(),SearchDate.getDate());
          DateStr.Printf( "%04d/%02d/%02d",SearchDate.getYear(),SearchDate.getMonth(),SearchDate.getDate());
          if( (FFileHandle = fopen( LogFileName.c_str(), "r" )) != NULL )
          {
               while( fgets( Buffer, 512, FFileHandle ) != NULL )
               {
                    if( memcmp( Buffer + 13 , Account.c_str(), Account.Length() ) == 0 )/// Compare the account.
                    {
                        UFC::AnsiString TimeStr( Buffer, 8 ); ///< Fetch the time string hh:MM:ss
                        UFC::AnsiString DataStr( Buffer + 13 );                        
                        ResultStr.Printf( "%s,%s,%s", DateStr.c_str(), TimeStr.c_str(), DataStr.c_str());
                        LogStream.Write( ResultStr.c_str(), (Int32)ResultStr.Length() );
                    } ///< String format: Date,Time,Account,message type,message
               }
               fclose( FFileHandle );
          }
          SearchDate+=1; /// Add 1 day.
       };
       if( LogStream.GetPosition() == 0 )///< No Data
       {
           UFC::UDate Today;
           UFC::AnsiString DateStr,TimeNow,ResultStr;
           
           Today.setCurrent();
           DateStr.Printf( "%04d/%02d/%02d",Today.getYear(),Today.getMonth(),Today.getDate());
           UFC::GetTimeString( TimeNow, TRUE );
           ResultStr.Printf( "%s,%s,%s,inf,No message.", DateStr.c_str(), TimeNow.c_str(), Account.c_str());
           LogStream.Write( ResultStr.c_str(), (Int32)ResultStr.Length() );           
       }    
       ReplyData->append( "LOG", (unsigned char*)LogStream.GetBuffer(), LogStream.GetSize());
    }
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyManager::AddGroupAttribute( Int32 Group , MTree* ReplyData )
{
    if( Group >= 0 )
    {
        UFC::AnsiString GroupName,Name,Value;
        UFC::Section*        GroupSession;

        if( Group > 0 ) ///< administrator don't have attributes
        {
            GroupName.Printf( "Group%d", Group );
            if( (GroupSession = GroupSetting->GetSection( GroupName )) != NULL )
            {
                for( int i = 0; i < GroupSession->ItemCount(); i++ )
                {
                    if( GroupSession->GetNameValue( i, Name, Value ) )
                        ReplyData->append( Name, Value.ToInt() );
                }
            }
        }
        if( (GroupSession = GroupSetting->GetSection( "GroupNames" )) != NULL )
        {
            for( int i = 0; i < GroupSession->ItemCount(); i++ )
            {
                if( GroupSession->GetNameValue( i, Name, Value ) )
                    ReplyData->append( Name, Value );
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
int SpeedyManager::Login( MTree* Data , MTree* ReplyData)
{
    UFC::AnsiString Id, Pwd, log;
    User* user;
    

    if( Data->get( "ID", Id ) && Data->get( "PWD", Pwd ) )
    {
        if( (user = Users->FindUser( Id ) ) != NULL )
        {
            /* 
            Int32 Version = 0;
             
            if( Data->get( "VER", Version ) == TRUE)
            {
                if( Version < 30038 ) ///< 3.0.38
                {
                    log.Printf( "%s, err,SpeedyCenter version[%d] too old.", Version, Id.c_str() );
                    WriteUserlog( log );
                    return ERROR_VERSION_TOO_OLD;
                }
            }
            else
            {
                log.Printf( "%s,err,Missing VER node.", Id.c_str() );
                WriteUserlog( log );
                return ERROR_VERSION_TOO_OLD;
            }*/
          
            if( user->GetState( ) == asLocked )
            {
                log.Printf( "%s,err,Account locked.", Id.c_str() );
                WriteUserlog( log );
                return ERROR_ACCOUNT_LOCKED; ///< Account Locked.
            }
            
            if( Pwd == user->GetPwd() ) ///< Compare password.
            {
                AccountGroup Group = user->GetGroup();
                if( Group == agAdmin )
                {                  
                    if( IsAdminLogon == TRUE )
                    {
                        log.Printf( "%s,err,An administrator already login.", Id.c_str() );
                        WriteUserlog( log );
                        return ERROR_ALREADY_LOGON;   ///< An Administrator already logon.
                    }
                    else if( SupportAdmin == FALSE )
                    {
                        log.Printf( "%s,err,Not support administration functions.", Id.c_str());
                        WriteUserlog( log );
                        return ERROR_NOT_SUPPORT_ADMIN;  ///< This SpeedyAgent not support administration functions. 
                    }
                    else
                    {
                        IsAdminLogon = TRUE;
                        AdminAccount = Id;
                    }
                }  //if( Group == agAdmin )
                
                AddGroupAttribute( Group, ReplyData );
                if( ( Id.AnsiCompare( "Admin" ) != 0 ) && ( user->GetState( ) == asExpired ) ) ///< Valid user, but password expired.("Admin" never expired)
                {
                    ReplyData->append( "GP", Group ); 
                    log.Printf( "%s,err,Account expired" , Id.c_str() );
                    WriteUserlog( log );
                    return ERROR_SPEEDY_ACCOUNT_EXPIRED; ///< Password Expired.
                }
                log.Printf( "%s,inf,Login OK!", Id.c_str() );
                WriteUserlog( log );
                user->SetErrCount( 0 );
                Int32 LeavesDays = user->ExpLeavesDay();
                if( PwdChangeHintDays >= LeavesDays )
                    ReplyData->append( "WARNING", LeavesDays );
                return Group; ///< Logon OK!!!
            }
            else
            {
                int ErrCount = user->GetErrCount();
                ErrCount++;
                if( ErrCount >= ErrorLockCount )
                {
                    ErrCount = ErrorLockCount;
                    user->SetState( asLocked );///< Locked
                }
                user->SetErrCount( ErrCount );
                log.Printf( "%s,err,Invalid password.", Id.c_str());
                WriteUserlog( log );
                return ERROR_SPEEDY_WRONG_PASSWORD; ///< Invalid PWD.
            }
        }
        else
        {
           log.Printf(",%s,err,Account not exists.",  Id.c_str() );
           WriteUserlog( log );
           return ERROR_ACCOUNT_NOT_EXISTS;///< User not exists.
        }
    }  //if( Data->get( "ID", Id ) && Data->get( "PWD", Pwd ) )
    return ERROR_ACCOUNT_NOT_EXISTS;///< User not exists.
}
//----------------------------------------------------------------------------------------------------------------------
int SpeedyManager::Logout( MTree* Data )
{
   UFC::AnsiString Id,log;
   
   if( Data->get( "ID", Id ))
   {
        if( Id.Length() >= 5 && Users->FindUser( Id )!= NULL )
        {
            if( AdminAccount == Id )
                IsAdminLogon = FALSE;
            log.Printf( "%s,inf,Logout.", Id.c_str());
            WriteUserlog( log );
            return RESULT_OK;
        }
   }
   return RESULT_FAILED;
}
//----------------------------------------------------------------------------------------------------------------------
void SpeedyManager::AddUsersListToMTree(  MTree*  ReplyList )
{
    Int32  count = Users->GetCount();

    ReplyList->append( "COUNT",  count );
    for( int i= 1; i <= count; i++ )
    {
        UFC::AnsiString FieldName = "USER" + UFC::AnsiString( i );
        UFC::AnsiString UserData;

        Users->GetUser( i-1 )->ToString( UserData );
        ReplyList->append( FieldName, UserData );
    }
}
//----------------------------------------------------------------------------------------------------------------------
int SpeedyManager::AddUser( MTree* Data )
{
     UFC::AnsiString Id, Pwd, log;
     AccountGroup OldGroup;
     int          Group;

     if( SupportAdmin == FALSE )
         return USR_ERROR_NOT_SUPPORT_ADMIN;
     if( IsAdminLogon == FALSE )
         return USR_ERROR_NOT_ADMINISTRATOR;
     if(  Data->get( "ID", Id ) && Data->get( "PWD", Pwd ) && Data->get( "GROUP", Group  ) )
     {
         User*      NewUser = new User( PwdVailidateDays, Id );
         int        Result;

         NewUser->SetPwd( Pwd );
         NewUser->SetGroup( (AccountGroup)Group, OldGroup );
         Result = Users->Add( NewUser );
         if( Result <= 0 )
             delete NewUser;
         else
         {
             log.Printf( "%s,inf,Add new user:%s.",AdminAccount.c_str(),Id.c_str());
             WriteUserlog( log );
         }
         return Result;
     }
     else
        return USR_ERROR_USR_ALREADY_EXISTS;
}
//----------------------------------------------------------------------------------------------------------------------
int SpeedyManager::DeleteUser( MTree* Data )
{
    UFC::AnsiString Id, log;

    if( SupportAdmin == FALSE )
        return USR_ERROR_NOT_SUPPORT_ADMIN;
    if( IsAdminLogon == FALSE )
        return USR_ERROR_NOT_ADMINISTRATOR;
    if(  Data->get( "ID", Id ) )
    {
        int  Result = Users->Delete( Id );

        if( Result > 0 )
        {
            log.Printf( "%s,inf,Delete user:%s.",AdminAccount.c_str(),Id.c_str() );
            WriteUserlog( log );
        }
        return Result;
    }
    else
       return RESULT_FAILED;
}
//----------------------------------------------------------------------------------------------------------------------
int SpeedyManager::ModifyUser( MTree* Data )
{
    UFC::AnsiString Id,Action,log;
    User*           ModifyUser;
    int             Group,State;
    AccountGroup    OldGroup;
    BOOL            Modify = FALSE;

    if( SupportAdmin == FALSE )
        return USR_ERROR_NOT_SUPPORT_ADMIN;
    if( IsAdminLogon == FALSE )
        return USR_ERROR_NOT_ADMINISTRATOR;
    if( Data->get( "ID", Id ) && Data->get( "GROUP", Group ) && Data->get( "STATE", State ))
    {
        if( (ModifyUser= Users->FindUser( Id )) == NULL )
            return USR_ERROR_USR_NOT_EXISTS;
        if( ModifyUser->SetGroup( (AccountGroup)Group, OldGroup ) == TRUE )
        {
            UFC::AnsiString NewGP,OldGP;

            GetGroupName( Group, NewGP );
            GetGroupName( OldGroup, OldGP );
            log.Printf( "%s,inf,Modify user group from %s to %s.User:%s.",AdminAccount.c_str(),OldGP.c_str(),NewGP.c_str(),Id.c_str() );
            WriteUserlog( log );            
            Modify = TRUE;
        }
        if( ModifyUser->SetState( (AccountState)State, Action ) == TRUE )
        {
            log.Printf( "%s,inf,%s.User:%s.",AdminAccount.c_str(),Action.c_str(), Id.c_str() );
            WriteUserlog( log );
            Modify = TRUE;
        }
        if( Modify == TRUE)
            Users->Update();
        return RESULT_OK;
    }
    else
        return RESULT_FAILED;
}
//----------------------------------------------------------------------------------------------------------------------
int SpeedyManager::ModifyPwd( MTree* Data )
{
    User* ModifyUser;
    UFC::AnsiString Id,Pwd,UserName,log;
    

    if( Data->get( "ID", Id ) && Data->get( "PWD", Pwd ))
    {
        if( SupportAdmin == FALSE )
            return USR_ERROR_NOT_SUPPORT_ADMIN;        
        if( (ModifyUser = Users->FindUser( Id )) == NULL )
            return USR_ERROR_USR_NOT_EXISTS;
        ModifyUser->SetPwd( Pwd );
        Users->Update();
        if( Data->get( "USER", UserName ) == TRUE )
            log.Printf( "%s,inf,Modify password.User:%s.",UserName.c_str(),Id.c_str() );
        else
            log.Printf( "%s,inf,Modify password.User:%s.",AdminAccount.c_str(),Id.c_str() );
        WriteUserlog( log );        
        return RESULT_OK;
    }
    else
        return RESULT_FAILED;
}
//----------------------------------------------------------------------------------------------------------------------
// Key: host name of the login PC.
//----------------------------------------------------------------------------------------------------------------------
void SpeedyManager::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
     MTree      ReplyData;
     Int32      CMD;
     Int32      Result;
     Int32      IP;
     UInt32     UIP;
     
     if( Data->get( "_IP", IP ) == TRUE && IP != (Int32)FLocalIP ) ///< Message from other MBus
     {
         UIP = (UInt32)IP;
         UFC::BufferedLog::Printf( " Ignore message to:%d.%d.%d.%d", UIP>>24,(UIP<<8)>>24,(UIP<<16)>>24,(UIP<<24)>>24 );
         return;
     }
     else
         UIP = (UInt32)IP;
     if( Data->get( "COMMAND", CMD ) )
     {        
         switch( CMD )
         {
            case CMD_LOGIN:       Result = Login( Data, &ReplyData );
                                  ReplyData.append( "COMMAND", CMD_LOGIN_RESULT );
                                  ReplyData.append( "RESULT", Result );
                                  ReplyData.append( "HOST", UFC::Hostname );                                  
                                  MessageObj->Send( "ADMIN", Key, ReplyData, FALSE );
                                  UFC::BufferedLog::Printf( " Handle Login message from:%s to:%d.%d.%d.%d", Key.c_str(), UIP>>24,(UIP<<8)>>24,(UIP<<16)>>24,(UIP<<24)>>24 );
                                  break;
            case CMD_LOGOUT:      Logout( Data );
                                  UFC::BufferedLog::Printf( " Handle Logout message from:%s to:%d.%d.%d.%d", Key.c_str(),UIP>>24,(UIP<<8)>>24,(UIP<<16)>>24,(UIP<<24)>>24 );
                                  break;
            case CMD_GET_LIST:    AddUsersListToMTree( &ReplyData );
                                  ReplyData.append( "COMMAND", CMD_USERS_LIST );
                                  ReplyData.append( "HOST", UFC::Hostname );
                                  MessageObj->Send( "ADMIN", Key, ReplyData, FALSE );
                                  UFC::BufferedLog::Printf( " Handle get user list message from:%s to:%d.%d.%d.%d", Key.c_str(),UIP>>24,(UIP<<8)>>24,(UIP<<16)>>24,(UIP<<24)>>24 );
                                  break;
            case CMD_QUERY_LOG:   ReplyData.append( "COMMAND", CMD_REPLY_LOG );
                                  QueryLog( Data, &ReplyData );
                                  MessageObj->Send( "ADMIN", Key, ReplyData, FALSE );
                                  UFC::BufferedLog::Printf( " Handle query log message from:%s to:%d.%d.%d.%d", Key.c_str(),UIP>>24,(UIP<<8)>>24,(UIP<<16)>>24,(UIP<<24)>>24 );
                                  break;
            case CMD_WRITE_LOG:   WriteUserlog( Data );
                                  UFC::BufferedLog::Printf( " Handle write log message from:%s to:%d.%d.%d.%d", Key.c_str(),UIP>>24,(UIP<<8)>>24,(UIP<<16)>>24,(UIP<<24)>>24 );
                                  break;                                  
                                  
            case CMD_ADD_USERS:   Result = AddUser( Data );
                                  ReplyData.append( "COMMAND", CMD_ADD_RESULT );
                                  ReplyData.append( "RESULT", Result );
                                  if( Result > 0 )
                                      AddUsersListToMTree( &ReplyData );
                                  MessageObj->Send( "ADMIN", Key, ReplyData, FALSE );
                                  UFC::BufferedLog::Printf( " Handle add user message from:%s to:%d.%d.%d.%d", Key.c_str(),UIP>>24,(UIP<<8)>>24,(UIP<<16)>>24,(UIP<<24)>>24 );
                                  break;
            case CMD_DELETE_USER: Result = DeleteUser( Data );
                                  ReplyData.append( "COMMAND", CMD_DELETE_RESULT );
                                  ReplyData.append( "RESULT", Result );
                                  if( Result > 0 )
                                      AddUsersListToMTree( &ReplyData );
                                  MessageObj->Send( "ADMIN", Key, ReplyData, FALSE );
                                  UFC::BufferedLog::Printf( " Handle delete user message from:%s to:%d.%d.%d.%d", Key.c_str(), UIP>>24,(UIP<<8)>>24,(UIP<<16)>>24,(UIP<<24)>>24 );
                                  break;
            case CMD_MODIFY_USER: Result = ModifyUser( Data );
                                  ReplyData.append( "COMMAND", CMD_MODIFY_RESULT );
                                  ReplyData.append( "RESULT", Result );
                                  if( Result > 0 )
                                      AddUsersListToMTree( &ReplyData );
                                  MessageObj->Send( "ADMIN", Key, ReplyData, FALSE );
                                  UFC::BufferedLog::Printf( " Handle modify user message from:%s to:%d.%d.%d.%d", Key.c_str(), UIP>>24,(UIP<<8)>>24,(UIP<<16)>>24,(UIP<<24)>>24 );
                                  break;
            case CMD_MODIFY_PWD:  Result = ModifyPwd( Data );
                                  ReplyData.append( "COMMAND", CMD_MODIFY_PWD_RESULT );
                                  ReplyData.append( "RESULT", Result );
                                  if( Result > 0 )
                                      AddUsersListToMTree( &ReplyData );
                                  MessageObj->Send( "ADMIN", Key, ReplyData, FALSE );
                                  UFC::BufferedLog::Printf( " Handle modify password message from:%s to:%d.%d.%d.%d", Key.c_str(), UIP>>24,(UIP<<8)>>24,(UIP<<16)>>24,(UIP<<24)>>24 );
                                  break;
         }
     }
}
//------------------------------------------------------------------------------
void SpeedyManager::LoadSetting( void )
{
    try
    {
        UFC::AnsiString Value;
        UFC::UiniFile   Config( "../cfg/SpeedyManager.cfg" );       
        
        if( Config.GetValue( "Setting", "PasswordExpireDays",Value ) == TRUE )
            PwdVailidateDays = Value.ToInt();
        if( Config.GetValue( "Setting", "ChangePasswordHintDays",Value ) == TRUE )
            PwdChangeHintDays = Value.ToInt();
        if( Config.GetValue( "Setting", "PasswordErrorLockCount",Value ) == TRUE )
            ErrorLockCount = Value.ToInt();  
        if( Config.GetValue( "Setting", "UserLogKeepDays",Value ) == TRUE )
            LogkeepDays = Value.ToInt();
        if( Config.GetValue( "Setting", "Administration", Value ) == TRUE )        
            SupportAdmin = Value.ToInt();        
        if( Config.GetValue( "Setting", "UserCount", Value ) == TRUE )        
            MaxUserCount = Value.ToInt();
    }
    catch(...)
    {
        UFC::BufferedLog::Printf(" SpeedyManager.cfg not found, use default settings.");        
    }
    GroupSetting = new UFC::UiniFile( "../cfg/SpeedyUserGroup.cfg", TRUE );
}
//--------------------------------------------------------------------------------------------------------------------

