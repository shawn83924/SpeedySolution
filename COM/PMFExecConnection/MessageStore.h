#ifndef MessageStore_H
#define MessageStore_H
//---------------------------------------------------------------------------
#include "../UFC/UFC.h"
//---------------------------------------------------------------------------
class TMessageStoreEventHandler
{
public:
    virtual ~TMessageStoreEventHandler() {}
    virtual void OnNewReport( UFC::AnsiString* Message, int Seq ) = 0;
};
//---------------------------------------------------------------------------
class TMessageStore : public UFC::PThread
{
public:
    TMessageStore( const UFC::AnsiString& FileName, int MessageLength, BOOL NeedToRecover );
    ~TMessageStore( void );

private:
    TMessageStoreEventHandler*      FEventHandler;
    UFC::PtrList<UFC::AnsiString>*  FMemStore;
    UFC::FileStreamEx*              FFileStore;
    UFC::PCriticalSection           FStoreCS;
    int                             FPos;
    UFC::PCriticalSection           FPosCS;
    int                             FMessageLength;

private:
    void                            InitStore( const UFC::AnsiString& FileName, BOOL NeedToRecover );
    void                            RecoverFromFile( void );
    UFC::AnsiString*                AddMessageIntoStore( const char* MessageString );
    UFC::AnsiString*                GetMessageFromStore( int MessageIndex );
    virtual void                    Execute();
public:
    bool                            CheckNewReport( void );
    void                            SetEventHandler( TMessageStoreEventHandler* pEventHandler );
    void                            Add( const char* MessageString );
    void                            SeekTo( int Seq );
    Int32                           GetItemCount();
};
//---------------------------------------------------------------------------
#endif
