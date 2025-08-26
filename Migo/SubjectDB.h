//------------------------------------------------------------------------------
#ifndef SubjectDBH
#define SubjectDBH
//------------------------------------------------------------------------------
#include "MigoHeader.h"
//------------------------------------------------------------------------------
class SubjectKeyPair
{
private:
    MString FSubject;
    MString FKey;
public:
    SubjectKeyPair( const MString& Subject, const MString& Key ):FSubject( Subject ),FKey( Key ){}
    const MString& getSubject() { return FSubject; }
    const MString& getKey()     { return FKey; }
    BOOL  operator ==( const SubjectKeyPair &rhs ) const
    {
        if( FSubject == "all" || rhs.FSubject == "all")
            return TRUE;
        if( (FSubject == rhs.FSubject) && (FKey == rhs.FKey|| FKey =="all" || rhs.FKey =="all") )
            return TRUE;
        return FALSE;
    }
};
//------------------------------------------------------------------------------
class KeyInfo
{
private:
    BOOL                  FRegistAll;
    UFC::PStringHashedSet FKeys;
public:
    KeyInfo( void );
    BOOL  Interest( const MString& Key );
    void  Regist( const MString& Key );
    void  Unregist( const MString& Key );
    UFCType::Int32 GetInterest( UFC::PStringList& KeyList );
    BOOL  IsEmpty();
};
//------------------------------------------------------------------------------
class ConnectionInfo
{
private:
    BOOL                                        FRegistAll;
    UFC::PHashedList<UFC::AnsiString,KeyInfo*>  FSubjects;
    UFC::PClientSocket*                         FSocket;
private:
    BOOL Add( UFC::List<SubjectKeyPair>& SubjectKeyList, SubjectKeyPair NewPair );
public:
    ConnectionInfo( UFC::PClientSocket* Socket );
    ~ConnectionInfo();
    UFC::PClientSocket* Socket( void ) { return FSocket; }
    BOOL InterestAll( void ){ return FRegistAll; }
    BOOL Interest( const MString& Subject, const MString& Key );
    void Regist( const MString& Subject, const MString& Key );
    void Unregist( const MString& Subject, const MString& Key );
    UFCType::Int32 GetInterest( UFC::List<SubjectKeyPair>& SubjectKeyList );
};
//------------------------------------------------------------------------------
class SubjectsDB
{
private:
    UFC::PtrList<ConnectionInfo> FConnectionsInfo;
    UFC::PCriticalSection        FCS;
private:
    ConnectionInfo* FindConnection( UFC::PClientSocket* Connection );
public:
    SubjectsDB();
    ~SubjectsDB();
    void  Regist( RegistMessage* Header, UFC::PClientSocket* Connection );
    void  Regist(  const UFC::AnsiString& Subject, const UFC::AnsiString& Key, UFC::PClientSocket* Connection );
    void  Unregist( const MString& Subject, const MString& Key, UFC::PClientSocket* Connection );
    void  RemoveConnection( UFC::PClientSocket* Connection );
    UFCType::Int32 All( UFC::PtrList<UFC::PClientSocket>* ConnectionList );
    UFCType::Int32 Search( UFC::PtrList<UFC::PClientSocket>* ConnectionList, DataMessage* Header );
    UFCType::Int32 Search( UFC::PtrList<UFC::PClientSocket>* ConnectionList,  const UFC::AnsiString& Subject, const UFC::AnsiString& Key );
    UFCType::Int32 GetInterest( UFC::List<SubjectKeyPair>& SubjectKeyList );
    UFCType::Int32 GetInterest( RegistMessage& RegMsg );
};
//------------------------------------------------------------------------------
template <class T>
class CSubjectRec
{
public:
    MString FSubject;
    MString FKey;
    T*      FObject;
public:
    CSubjectRec( const MString& Subject, const MString& Key, T* Object = NULL );
	BOOL Compare( CSubjectRec< T >* Ref );
	BOOL Compare( const MString& Subject, const MString& Key, T* Object );
	BOOL ExactCompare( const MString& Subject, const MString& Key );
};
//------------------------------------------------------------------------------
template <class T>
CSubjectRec<T>::CSubjectRec( const MString& Subject, const MString& Key, T* Object )
:FSubject( Subject )
,FKey( Key )
,FObject( Object )
{
}
//------------------------------------------------------------------------------
template <class T>
BOOL CSubjectRec<T>::Compare( CSubjectRec< T >* Ref )
{
	if( Ref != NULL )
	{
		if( FSubject == "all" || Ref->FSubject == "all" || FSubject == Ref->FSubject ) ///< Compare subject
		{
			if( FKey == "all" || Ref->FKey == "all" || FKey == Ref->FKey )              ///< Compare key
			{
				if( FObject == NULL || Ref->FObject == NULL || FObject == Ref->FObject )///< Compare object
					return TRUE;
			}
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------------
template <class T>
BOOL CSubjectRec<T>::Compare( const MString& Subject, const MString& Key, T* Object )
{
	if( FSubject == "all" || Subject == "all" || FSubject == Subject ) ///< Compare subject
	{
		if( FKey == "all" || Key == "all" || FKey == Key )              ///< Compare key
		{
			if( FObject == NULL || Object == NULL || FObject == Object )///< Compare object
				return TRUE;
		}
	}
	return FALSE;
}
//------------------------------------------------------------------------------
template <class T>
BOOL CSubjectRec<T>::ExactCompare( const MString& Subject, const MString& Key )
{
	if( FSubject == Subject &&  FKey == Key )
		return TRUE;
	return FALSE;
}
//------------------------------------------------------------------------------
//
//
//------------------------------------------------------------------------------
template <class T>
class CSubjectDB
{
private:
	UFC::PtrList< CSubjectRec<T> > FRecords;
    UFC::PCriticalSection          FCS;
private:
    BOOL  Add( CSubjectRec<T>* Record );
public:
    BOOL  Add( RegistMessage* Header, T* Object );
    BOOL  Add( const MString& Subjectname, const MString& SubjectKey, T* Object );
    void  Delete( T* Object );
    void  Delete( UFCType::Int32 Index );
	UFCType::Int32  Search( UFC::PtrList<T>* List, DataMessage* Header, T* Object = NULL );
    UFCType::Int32  IndexOf( CSubjectRec<T>* rec );
	CSubjectRec<T>* Search( const MString& Subject, const MString& Key, T* Object );
	CSubjectRec<T>* ExactSearch( const MString& Subject, const MString& Key );
	CSubjectRec<T>* GetRecord( UFCType::Int32 Index );
};
//------------------------------------------------------------------------------    
template <class T>
BOOL  CSubjectDB<T>::Add( CSubjectRec<T>* Record )
{
    if( Record->FObject != NULL )
    {
        UFC::PLockObject FCSLock( FCS );

        if( IndexOf( Record ) == -1 )
        {
            FRecords.Add( Record );
            return TRUE;
        }
    }
	return FALSE;
}
//------------------------------------------------------------------------------
template <class T>
BOOL CSubjectDB<T>::Add( RegistMessage* Header, T* Object )
{
    CSubjectRec<T>* NewRecord = new CSubjectRec<T>( Header->getSubject(), Header->getKey(), Object );

    if( Add( NewRecord ) == FALSE )
    {
        delete NewRecord;
        return FALSE;
    }
    return TRUE;
}
//------------------------------------------------------------------------------
template <class T>
BOOL CSubjectDB<T>::Add( const MString& Subjectname, const MString& SubjectKey, T* Object )
{
    CSubjectRec<T>* NewRecord = new CSubjectRec<T>( Subjectname, SubjectKey, Object );

    if( Add( NewRecord ) == FALSE )
    {
        delete NewRecord;
        return FALSE;
    }
    return TRUE;
}
//------------------------------------------------------------------------------
template <class T>
void  CSubjectDB<T>::Delete( T* Object )
{
    CSubjectRec<T>*  DelRecord;
    UFC::PLockObject FCSLock( FCS );

    for( register UFCType::Int32 i =  FRecords.ItemCount() - 1; i >= 0; i-- )
    {
        DelRecord = FRecords.GetItem( i );
		if( DelRecord->FObject == Object )
        {
            FRecords.Delete( i );
            delete DelRecord;
        }
    }
}
//------------------------------------------------------------------------------
template <class T>
void  CSubjectDB<T>::Delete( UFCType::Int32 Index )
{
    CSubjectRec<T>*  rec;
    UFC::PLockObject FCSLock( FCS );

    if( (rec = FRecords.GetItem( Index ) ) != NULL )
    {
        FRecords.Delete( Index );
        delete rec;
	}
}
//------------------------------------------------------------------------------
template <class T>
UFCType::Int32 CSubjectDB<T>::Search( UFC::PtrList<T>* List, DataMessage* Header, T* Object )
{
    List->Clear();
    CSubjectRec<T>*  Record;
    CSubjectRec<T>   SearchRec( Header->getSubject(), Header->getKey(), Object );
    UFC::PLockObject FCSLock( FCS );

    for( register UFCType::Int32 i = FRecords.ItemCount() - 1; i >= 0; i-- )
    {
        Record = FRecords.GetItem( i );
        if( Record->Compare( &SearchRec ) == TRUE )
        {
            if( List->IndexOf( Record->FObject ) == -1 ) ///< This object already in match list.
                List->Add( Record->FObject );            ///< Add this object to match list.
        }
    }
	return List->ItemCount(); ///< Return the matched item count.
}
//------------------------------------------------------------------------------
template <class T>
UFCType::Int32 CSubjectDB<T>::IndexOf( CSubjectRec<T>* rec )
{
    for( register UFCType::Int32 i = FRecords.ItemCount() - 1; i >= 0; i-- )
    {
         if( rec->Compare( FRecords.GetItem( i ) ) == TRUE )
             return i;
    }
    return -1;
}
//------------------------------------------------------------------------------
template <class T>
CSubjectRec<T>* CSubjectDB<T>::Search( const MString& Subject, const MString& Key, T* Object )
{
	register CSubjectRec<T>*  Record;

	for( register UFCType::Int32 i = FRecords.ItemCount() - 1; i >= 0; i-- )
	{
		Record = FRecords.GetItem( i );
		if( Record->Compare( Subject, Key, Object ) == TRUE )
			return Record;
	}
	return NULL;
}
//------------------------------------------------------------------------------
template <class T>
CSubjectRec<T>* CSubjectDB<T>::ExactSearch( const MString& Subject, const MString& Key )
{
	register CSubjectRec<T>*  Record;

	for( register UFCType::Int32 i = FRecords.ItemCount() - 1; i >= 0; i-- )
	{
		Record = FRecords.GetItem( i );
		if( Record->ExactCompare( Subject, Key ) == TRUE )
			return Record;
	}
	return NULL;
}
//------------------------------------------------------------------------------
template <class T>
CSubjectRec<T>* CSubjectDB<T>::GetRecord( UFCType::Int32 Index )
{
	return FRecords.GetItem( Index );
}
//------------------------------------------------------------------------------
#endif

