//------------------------------------------------------------------------------------------------------------------------
#include "AccountStore.h"
#include "RiskManager.h"
//------------------------------------------------------------------------------------------------------------------------
// class AccountRec
//------------------------------------------------------------------------------------------------------------------------
UFC::PCriticalSection AccountRec::FStateCS;
//------------------------------------------------------------------------------------------------------------------------
AccountRec::AccountRec( UFC::PStream* Stream )
: FPosition( Stream->GetPosition() )
{
    LoadFromStream( Stream );
}
//------------------------------------------------------------------------------------------------------------------------
AccountRec::AccountRec( const UFC::AnsiString& ID, char Flag, UFC::PStream* Stream )
: FAccount( ID )
, FFlag( Flag )
, FCanBuy( TRUE )
, FCanSell( TRUE )
, FPosition( Stream->GetPosition() )
{
    SaveToStream( Stream );
}
//------------------------------------------------------------------------------------------------------------------------
void AccountRec::SetState( LockState State )
{
    UFC::PLockObject Lock( FStateCS );
    
    switch( State ) 
    {
        case lsNone:FCanBuy  = TRUE;
                    FCanSell = TRUE;
                    break;
        case lsBuy: FCanBuy  = FALSE;
                    FCanSell = TRUE;
                    break;
        case lsSell:FCanBuy  = TRUE;
                    FCanSell = FALSE;
                    break;
        case lsBoth:FCanBuy  = FALSE;
                    FCanSell = FALSE;
                    break;
        case lsUnknown: break;
    }
}
//------------------------------------------------------------------------------------------------------------------------
LockState AccountRec::GetState( void )
{
    if( FCanBuy == TRUE && FCanSell == TRUE)
        return lsNone;
    else if( FCanBuy == FALSE && FCanSell == FALSE )
        return lsBoth;
    else if( FCanBuy == FALSE  )
        return lsBuy;
    else
        return lsSell;
}
//------------------------------------------------------------------------------------------------------------------------
void AccountRec::SaveToStream( UFC::PStream* Stream )
{
    char Buffer[ 16 ];
    
    strcpy( Buffer, FAccount.c_str() );
    Buffer[ 8 ]  = FFlag;
    Buffer[ 9 ]  = (FCanBuy  == TRUE)?1:0;
    Buffer[ 10 ] = (FCanSell == TRUE)?1:0;;
    Stream->Seek( FPosition, UFC::soFromBeginning );
    Stream->Write( Buffer , 16 );
}
//------------------------------------------------------------------------------------------------------------------------
void AccountRec::LoadFromStream( UFC::PStream* Stream )
{
    char Buffer[ 16 ];
    
    Stream->Read( Buffer , 16 );
    FAccount = UFC::AnsiString( Buffer, 7 );
    FFlag           = Buffer[ 8 ];
    FCanBuy         = Buffer[ 9 ];
    FCanSell        = Buffer[ 10 ];
}
//------------------------------------------------------------------------------------------------------------------------
// class AccountStore
//------------------------------------------------------------------------------------------------------------------------
UFC::AnsiString AccountStore::SHandShakeName  = "ACCOUNT_MANAGER";
UFC::AnsiString AccountStore::SSyncFilePath   = FP_AccountStore;
//------------------------------------------------------------------------------------------------------------------------
AccountStore::AccountStore( UFC::AnsiString ThreadName, BOOL NeedRecover )
: FThreadName( ThreadName )
, FTCPServer( 9900, 10, 5 )
{
    if( NeedRecover == TRUE )
    {
	FDataFile = new UFC::FileStreamEx( SSyncFilePath, "r+" );	// "r+" = O_RDWR
	ExecuteRecover();
    }
    else
    {
	FDataFile = new UFC::FileStreamEx( SSyncFilePath, "w+" );	// "w+" = O_CREAT|O_TRUNC|O_RDWR
	Initialize();
    }
    FTCPServer.SetListener( this );
    FTCPServer.SetIdelInterval( 600 );
    FTCPServer.Run();

    // Print start up message
    UFC::BufferedLog::Printf( " [%s] thread is start up.", FThreadName.c_str() );
}
//------------------------------------------------------------------------------------------------------------------------
void AccountStore::ExecuteRecover( void )
{
    Int32		FileSize = FDataFile->GetSize();
    AccountRec*	NewRecord;

    FDataFile->Seek( 0, UFC::soFromBeginning );
    while( FDataFile->GetPosition() < FileSize )
    {
	NewRecord = new AccountRec( FDataFile );
	FAccounts.Add( NewRecord->GetAccount(), NewRecord );
    }
    UFC::BufferedLog::DebugPrintf(" Recover %d accounts information", FAccounts.ItemCount());
}
//------------------------------------------------------------------------------------------------------------------------
void AccountStore::Initialize( void )
{
    try
    {
        UFC::UiniFile                    Users( "../cfg/SpeedyGatewayUsers.cfg" );        
        UFC::Section*                    CheckSection;
        UFC::PHashedSet<UFC::AnsiString> Accounts;
        UFC::AnsiString                  Name,Value;

        for( int i = 0; i < Users.SectionCount(); i++ )
        {
            CheckSection = Users.GetSection( i );
            for( int j = 0; j < CheckSection->ItemCount();j++)
            {
                CheckSection->GetNameValue( j, Name,Value );
                if( Name.Length() == 7 && Value.Length() == 1  )///< Account length =7,and flag length = 1
                {
                    if( Name.ToInt() != 0 || Name == "0000000" )
                    {
                        if( Accounts.Exists( Name ) == FALSE )///New Account
                        {   // Add Account to Store                        
                            AddAccount( Name, Value[ 0 ] );
                            Accounts.Add( Name );
                        }
                    }
                }
            }                   
        }
        UFC::BufferedLog::DebugPrintf(" Build %d accounts information", FAccounts.ItemCount());
    }
    catch( ... )
    {
        UFC::BufferedLog::Printf(" ../cfg/SpeedyGatewayUsers.cfg not found.");
        exit(1);
    }
}
//------------------------------------------------------------------------------------------------------------------------
AccountRec* AccountStore::AddAccount( const UFC::AnsiString& ID, char Flag )
{
    AccountRec* NewAccount = new AccountRec( ID, Flag, FDataFile );
    FAccounts.Add( ID, NewAccount );
    FDataFile->Flush();
    return NewAccount;
}
//------------------------------------------------------------------------------------------------------------------------
AccountRec* AccountStore::GetAccount( const UFC::AnsiString& ID )
{
    return FAccounts.GetObjectByKey( ID );
}
//------------------------------------------------------------------------------------------------------------------------
char AccountStore::GetAccountFlag( const UFC::AnsiString& ID )
{
    AccountRec* ChkAccount = FAccounts.GetObjectByKey( ID );
    if( ChkAccount != NULL )
	return ChkAccount->GetFlag();
    return '1';
}
//------------------------------------------------------------------------------------------------------------------------
LockState AccountStore::SetState( const UFC::AnsiString& ID, LockState State )
{
    AccountRec* CheckAccount = FAccounts.GetObjectByKey( ID );
    if( CheckAccount != NULL ) 
    {
        CheckAccount->SetState( State );
        UpdateAccount( CheckAccount );
        FDataFile->Flush();
        return CheckAccount->GetState();
    }
    return lsUnknown;
}
//------------------------------------------------------------------------------------------------------------------------
LockState AccountStore::GetState( const UFC::AnsiString& ID )
{
    AccountRec* CheckAccount = FAccounts.GetObjectByKey( ID );
    if( CheckAccount != NULL )
        return CheckAccount->GetState();
    return lsUnknown;
}
//------------------------------------------------------------------------------------------------------------------------
void AccountStore::UpdateAccount( AccountRec* CurrentAccount )
{
    ///< Get the file position of this record.
    UInt32 Pos = CurrentAccount->FPosition;
    
    ///< Seek to the Position of this record.
    FDataFile->Seek( Pos, UFC::soFromBeginning );
    ///< Write to file
    CurrentAccount->SaveToStream( FDataFile );
    ///< Seek to the EOF
    FDataFile->Seek( 0, UFC::soFromEnd );
}
//------------------------------------------------------------------------------------------------------------------------
void AccountStore::OnListen( UFC::MultiplexingServer* ServSocket )
{
    UFC::BufferedLog::DebugPrintf(" Account managerment service listen on port:%d", ServSocket->GetPort());
}
//------------------------------------------------------------------------------------------------------------------------
void AccountStore::OnClientConnect( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket )
{
    UFC::BufferedLog::Printf(" [%s] A client Connected, FD:%d", FThreadName.c_str(), ClientSocket->GetHandle() );

    ClientSocket->SendBuffer( SHandShakeName, SHandShakeName.Length() + 1 );
}
//------------------------------------------------------------------------------------------------------------------------
void AccountStore::OnClientDisconnect( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket )
{
    UFC::BufferedLog::Printf(" [%s] A client Disonnected, FD:%d", FThreadName.c_str(), ClientSocket->GetHandle() );
}
//------------------------------------------------------------------------------------------------------------------------
BOOL AccountStore::OnClientWrite( UFC::MultiplexingServer* ServSocket, UFC::PClientSocket* ClientSocket )
{
    ///< Fromat:  Func|Account|State|#
    ///< Example: G|0389590|0|#    <--Query state
    ///< Example: S|0389590|1|#    <--Lock Sell side
    ///< Return:  NewState|#
    try 
    {
        UInt8 Buffer[ 64 ];
        
        memset( Buffer, 0, 64 );
        ClientSocket->BlockRecv( Buffer, 14 );
        UFC::AnsiString Result;
        UFC::AnsiString Account( (char*)Buffer + 2 , 7);
        char Rtn   = (char)lsUnknown;
        char State = Buffer[ 10 ];
        
        UFC::BufferedLog::Printf( " [%s] Receive:%s Account:%s State:%c", FThreadName.c_str(), Buffer, Account.c_str(), State );
        
        if( Buffer[0]=='S' ) ///< Set State
            Rtn = (int)SetState( Account, (LockState)(State-'0')) + '0';
        else if( Buffer[0]=='G' ) ///< Get State
            Rtn = (int)GetState( Account ) + '0';
        Result.Printf("%c|#", Rtn );
        UFC::BufferedLog::Printf( " [%s] Reply:%s", FThreadName.c_str(), Result.c_str() );
        ClientSocket->SendBuffer( Result.c_str(), Result.Length()+1 );
        return TRUE;
    }
    catch( UFC::SocketException &e ) 
    {
        UFC::BufferedLog::Printf( " [%s] FD:%d Recv data failed, bad data or client pipe was broken.", FThreadName.c_str(), ClientSocket->GetHandle() );
    }
    catch(...) 
    {
        UFC::BufferedLog::Printf( " [%s] OnClientWrite throw unknown exception!", FThreadName.c_str() );
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------
