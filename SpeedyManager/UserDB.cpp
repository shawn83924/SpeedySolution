//---------------------------------------------------------------------------
#include "UserDB.h"
//---------------------------------------------------------------------------
User::User( int PwdVailidateDays, const UFC::AnsiString& ID )
:FID( ID ),
 FPWD( "e3afed0047b08059d0fada10f400c1e5" ), ///< md5 string for "Admin"
 FGroup( agAdmin ),   ///< 0 Admin, 1 OP
 FState( asOK ),      ///< 0. OK
 FPwdVailidateDays( PwdVailidateDays )
{
    FExpDate += FPwdVailidateDays;
}
//---------------------------------------------------------------------------
bool User::ValidUser( void )
{
    if( FID.Length() == 0 )
        return false;            
    if( FPWD.Length() != 32 )
        return false;    
    if( FState != asOK && FState != asLocked && FState != asExpired )
        return false;    
    return true;    
}
//---------------------------------------------------------------------------
void User::SetPwd( UFC::AnsiString& NewPwd )
{
    FPWD = NewPwd;
    FPwdChangeDate.setCurrent( );
    FExpDate.setCurrent( );
    FExpDate += FPwdVailidateDays;
    if( FState == asExpired )
        FState = asOK;
}
//---------------------------------------------------------------------------
BOOL User::SetGroup( AccountGroup NewGroup, AccountGroup& OldGroup )
{
    OldGroup = FGroup;
    if( FGroup != NewGroup )
    {
        FGroup = NewGroup;
        return TRUE;
    }
    return FALSE;
}
//---------------------------------------------------------------------------
BOOL User::SetState( AccountState NewState, UFC::AnsiString& Action  )
{
  if( FState != NewState )
  {
      Action = "Change the user state";
      if( FState == asOK )
      {
          if( NewState == asLocked )
              Action = "Change this user to Locked state";
          else if( NewState == asExpired )
              Action = "Change this user to Expired state";
      }
      else if( FState == asLocked )
      {
          if( NewState == asOK )
              Action = "Unlock this user";
          else if( NewState == asExpired )
              Action = "Change this user to Expired state";
      }
      else if( FState == asExpired )
      {
          if( NewState == asOK )
          {
              Action = "Unlock this expired user";
              FPwdChangeDate.setCurrent( );
              FExpDate.setCurrent( );
              FExpDate += FPwdVailidateDays;
          }
          else if( NewState == asLocked )
              Action = "Change this user to Locked state";
      }
      FState = NewState;
      return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------
BOOL User::SetState( AccountState NewState  )
{
  if( FState != NewState )
  {
      if( FState == asExpired )
      {
          if( FState == asOK )
          {
              FPwdChangeDate.setCurrent( );
              FExpDate.setCurrent( );
              FExpDate += FPwdVailidateDays;
          }
      }
      FState = NewState;
      return TRUE;
  }
  return FALSE;
}
//---------------------------------------------------------------------------
int  User::ExpLeavesDay( void )
{
    UFC::UDate Today;

    Today.setCurrent( );
    return FExpDate - Today;
}
//---------------------------------------------------------------------------
void User::LoadFromStream( UFC::PStream* Stream )
{
    FID.LoadFromStream( Stream );
    FPWD.LoadFromStream( Stream );
    Stream->Read( &FGroup, sizeof(int));
    Stream->Read( &FState, sizeof(int));
    Stream->Read( &FErrCount, sizeof(int));
    FPwdChangeDate.LoadFromStream( Stream, FALSE );
    FExpDate.LoadFromStream( Stream, FALSE );
    UFC::UDate Today;
    if( Today > FExpDate )
        FState = asExpired; ///< Password expired.
}
//---------------------------------------------------------------------------
void User::SaveToStream( UFC::PStream* Stream )
{
    FID.SaveToStream( Stream );
    FPWD.SaveToStream( Stream );
    Stream->Write( &FGroup, sizeof(int));
    Stream->Write( &FState, sizeof(int));
    Stream->Write( &FErrCount, sizeof(int));
    FPwdChangeDate.SaveToStream( Stream, FALSE );
    FExpDate.SaveToStream( Stream, FALSE );
}
//---------------------------------------------------------------------------
void User::ToString( UFC::AnsiString& Str )
{
    Str.Printf("%s,%s,%d,%d,%d/%02d/%02d", FID.c_str(),FPWD.c_str(), (Int32)FGroup,FState,
               FExpDate.getYear(),FExpDate.getMonth(),FExpDate.getDate() );
}
//---------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
UserDB::UserDB( int PwdVailidateDays,int MaxUser )
:FMaxCount( MaxUser )
,FFileName( "../cfg/Users" )
,FPwdVailidateDays( PwdVailidateDays )
{
    if( UFC::FileExists( FFileName ) == TRUE )
    {
        int Count;

        UFC::FileStream File( FFileName, O_RDONLY );
        UFC::MemoryStream Mem( File.GetSize(), File.GetSize() );

        File.Read( (void*)Mem.GetBuffer(), File.GetSize() );
        Mem.Read( &Count, sizeof(int));
        for( int i=0;i<Count;i++)
        {
            User* usr = new User( FPwdVailidateDays, "Guest" );
            usr->LoadFromStream( &Mem );
            if( usr->ValidUser() == true  )
                FUserList.Add( usr );
            else
                delete usr;
        }        
    }
    if( FindUser( "Admin") == NULL )
    {
        FUserList.Add( new User( FPwdVailidateDays, "Admin" ) );            
        Update();
    }
}
//---------------------------------------------------------------------------
UserDB::~UserDB()
{
    for( int i = 0; i < FUserList.ItemCount(); i++ )
         delete FUserList.GetItem(i);
    FUserList.Clear();
}
//---------------------------------------------------------------------------
void UserDB::Update()
{
    int Count = FUserList.ItemCount();

    UFC::MemoryStream Mem;
    Mem.Write( &Count, sizeof(int));
    for( int i = 0; i < Count; i++ )
         FUserList.GetItem( i )->SaveToStream( &Mem );

    UFC::FileStream File( FFileName, O_CREAT|O_RDWR|O_TRUNC );
    File.Write( Mem.GetBuffer(), Mem.GetSize() );
}
//---------------------------------------------------------------------------
BOOL UserDB::UserExists( UFC::AnsiString& ID )
{
    for( int i = 0; i < FUserList.ItemCount(); i++ )
       if( ID == FUserList.GetItem( i )->GetID() )
           return TRUE;
    return FALSE;
}
//---------------------------------------------------------------------------
int UserDB::AdminCount()
{
    int AdminC = 0;
    for( int i = 0; i < FUserList.ItemCount(); i++ )
       if( FUserList.GetItem( i )->GetGroup() == agAdmin )
           AdminC++;
    return AdminC;
}
//---------------------------------------------------------------------------
int UserDB::Add( User* NewUser )
{
    if( UserExists( NewUser->GetID() ) == TRUE )
        return -1;
    if( FUserList.ItemCount() == FMaxCount )
        return 0;
    FUserList.Add( NewUser );
    Update();
    return FUserList.ItemCount();
}
//---------------------------------------------------------------------------
int UserDB::Delete( UFC::AnsiString& UserID )
{
    User* usr = FindUser( UserID );
    if( usr == NULL )
        return 0;   ///< Return 0: User not exists.
    if( usr->GetGroup() == 0 && AdminCount() == 1 )
        return -1;  ///< Return -1: Can't delete the last Admin.
    if( FUserList.ItemCount() == 1 )
        return -2;  ///< Return -2: Can't delete the last user.
    for( int i = 0; i < FUserList.ItemCount(); i++ )
    {
         if( UserID == FUserList.GetItem( i )->GetID() )
         {
             delete FUserList.GetItem( i );
             FUserList.Delete( i );
             break;
         }
    }
    Update();
    return FUserList.ItemCount();
}
//---------------------------------------------------------------------------
User* UserDB::FindUser( const UFC::AnsiString& UserID )
{
    for( int i = 0; i < FUserList.ItemCount(); i++ )
       if( UserID == FUserList.GetItem( i )->GetID() )
           return  FUserList.GetItem( i );
    return NULL;
}
//---------------------------------------------------------------------------
