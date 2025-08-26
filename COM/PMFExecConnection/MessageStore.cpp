//---------------------------------------------------------------------------
// MessageStore.cpp
//---------------------------------------------------------------------------
#include <string.h>
#include "MessageStore.h"
//---------------------------------------------------------------------------
TMessageStore::TMessageStore( const UFC::AnsiString& FileName, int MessageLength, BOOL NeedToRecover )
:UFC::PThread( NULL )
,FEventHandler( NULL )
,FMemStore( NULL )
,FFileStore( NULL )
,FPos( 0 )
,FMessageLength( MessageLength )
{
    InitStore( FileName, NeedToRecover );        
}
//---------------------------------------------------------------------------
TMessageStore::~TMessageStore( void )
{
    delete FMemStore;
    delete FFileStore;
}
//---------------------------------------------------------------------------
void TMessageStore::InitStore( const UFC::AnsiString& FileName, BOOL NeedToRecover )
{    
    FMemStore  = new UFC::PtrList<UFC::AnsiString>( 1000 );
    if( NeedToRecover )
    {
        FFileStore = new UFC::FileStreamEx( FileName, "a+" );
        RecoverFromFile();
    }
    else
        FFileStore = new UFC::FileStreamEx( FileName, "w+" );
}
//---------------------------------------------------------------------------
void TMessageStore::RecoverFromFile( void )
{    
    char* Buff = new char[ FMessageLength + 1 ];
    Buff[ FMessageLength ] = 0x00;
    try
    {
        while( TRUE )
        {
            int ReadSize = FFileStore->Read( Buff, FMessageLength );
            if( ReadSize != FMessageLength )
                break;
            FMemStore->Add( new UFC::AnsiString( Buff ) );
        }
    }
    catch(...)
    {
    }
    delete [] Buff;
//printf( "%d messages recovered\n", FMemStore->ItemCount() );
}
//------------------------------------------------------------------------------
UFC::AnsiString* TMessageStore::AddMessageIntoStore( const char* MessageString )
{    
    UFC::PLockObject Lock( FStoreCS );
    UFC::AnsiString* Message = new UFC::AnsiString( MessageString );
    try
    {        
        if( Message->Length() == FMessageLength )
        {            
            FFileStore->Write( MessageString, FMessageLength );
            FFileStore->Flush();
            FMemStore->Add( Message );
            UFC::BufferedLog::Printf(" AddMessageIntoStore() ItemCount = %d, Pos = %d", FMemStore->ItemCount(), FPos);
            return Message;
        }
        else
        {
            UFC::BufferedLog::Printf(" AddMessageIntoStore() Error Msg Len = %d, FMessageLength = %d", Message->Length(), FMessageLength);
        }
    }
    catch(...)
    {
        UFC::BufferedLog::DebugPrintf( " Exceptioin!" );
    }
    delete Message;
    return NULL;
}
//---------------------------------------------------------------------------
UFC::AnsiString* TMessageStore::GetMessageFromStore( int MessageIndex )
{
    UFC::PLockObject Lock( FStoreCS );
    return FMemStore->GetItem( MessageIndex );
}
//---------------------------------------------------------------------------
void TMessageStore::SetEventHandler( TMessageStoreEventHandler* pEventHandler )
{
    FEventHandler = pEventHandler;    
}
//---------------------------------------------------------------------------
void TMessageStore::Add( const char* MessageString )
{
    if( (MessageString != NULL) && (MessageString[0] != 0x00) )
    {
//UFC::BufferedLog::Printf(" Store::Add Msg Len = %d, FMessageLength = %d   Pos %d", strlen(MessageString), FMessageLength, FPos);
        AddMessageIntoStore( MessageString );
    }
}
//---------------------------------------------------------------------------
bool TMessageStore::CheckNewReport( void )
{    
    if( FEventHandler == NULL )
        return false;
    
    UFC::PLockObject Lock( FPosCS );
    if( FPos < FMemStore->ItemCount() )
    {        
        UFC::AnsiString* NewMessage = GetMessageFromStore( FPos );        
        FPos++;
        FEventHandler->OnNewReport( NewMessage, FPos );
        return true;
    }
    else
    {
//UFC::BufferedLog::Printf( " No New Msg In Store ! Pos %d  ItemCount %d", FPos,  FMemStore->ItemCount());
      return false;
    }
}
//---------------------------------------------------------------------------
void TMessageStore::SeekTo( int Seq )
{
    UFC::PLockObject Lock( FPosCS );

    if( Seq <= 0 )
        FPos = 0;
    else
        FPos = Seq - 1;
}
//---------------------------------------------------------------------------
void TMessageStore::Execute()
{
    bool IsNewReport = false;
    while( true )
    {
        IsNewReport = CheckNewReport();
        if (!IsNewReport)
            UFC::SleepMS(100);
    }
}

//---------------------------------------------------------------------------

Int32 TMessageStore::GetItemCount() {
    if (FMemStore == NULL)
        return 0;
    else
        return FMemStore->ItemCount();
}  //GetItemCount()
