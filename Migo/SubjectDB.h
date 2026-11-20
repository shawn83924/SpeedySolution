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
    const MString& getSubject() const { return FSubject; }
    const MString& getKey()     const { return FKey; }
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
    ///< NOTE: this object is walked one by one on the forwarding hot path; do not add fields here for cold-path needs --
    ///< a larger cache footprint shows up directly in the per-message cost (measured: one extra MString made
    ///< Search 10~27% slower). If you need the subject name, use PHashedList's CurrentKey().
    KeyInfo( void );
    BOOL  Interest( const MString& Key );
    void  Regist( const MString& Key );
    void  Unregist( const MString& Key );
    UFCType::Int32 GetInterest( UFC::PStringList& KeyList );
    BOOL  IsEmpty();
};
//------------------------------------------------------------------------------
//  Accumulator for GetInterest: the list itself + the indexes used for dedup.
//
//  The original dedup scanned the whole list linearly on every insert, and that list is shared across all
//  connections, so the total cost was O(pairs^2). Measured with 200 connections x 40 subjects (8000 pairs): one
//  GetInterest took ~900ms while holding SubjectsDB's FCS the whole time -- i.e. every client
//  register/unregister/disconnect blocked all message forwarding for almost a second.
//
//  This expands the original operator== matching semantics into three O(1) hash lookups; behavior is identical.
//------------------------------------------------------------------------------
class SubjectKeyAccumulator
{
private:
    UFC::List<SubjectKeyPair>& FList;
    UFC::PStringHashedSet      FExactPairs;     ///< "subject\x01key"; only entries whose key != "all"
    UFC::PStringHashedSet      FAllKeySubjects; ///< subjects that already have (subject,"all")
    BOOL                       FHasAllSubject;  ///< the list already has an entry with subject == "all"
private:
    static MString PairKey( const MString& Subject, const MString& Key );
public:
    SubjectKeyAccumulator( UFC::List<SubjectKeyPair>& List );
    BOOL Add( const SubjectKeyPair& NewPair );
};
//------------------------------------------------------------------------------
class ConnectionInfo
{
private:
    BOOL                                        FRegistAll;
    UFC::PHashedList<UFC::AnsiString,KeyInfo*>  FSubjects;
    UFC::PClientSocket*                         FSocket;
    ///< Sequence number assigned when added to SubjectsDB. SubjectsDB's connection list is append-at-tail,
    ///< delete-in-middle, so sequence order always equals list order. The reverse index uses it to restore
    ///< candidate connections to exactly the same output order as the original linear scan.
    UFCType::Int32                              FSeq;
public:
    ConnectionInfo( UFC::PClientSocket* Socket );
    ~ConnectionInfo();
    UFC::PClientSocket* Socket( void ) { return FSocket; }
    BOOL InterestAll( void ){ return FRegistAll; }
    UFCType::Int32  GetSeq( void ) const      { return FSeq; }
    void            SetSeq( UFCType::Int32 S ){ FSeq = S; }
    ///< For reverse-index maintenance: list the subjects this connection currently has registered
    UFCType::Int32  GetSubjectNames( UFC::PStringList& Names );
    BOOL            HasSubject( const MString& Subject );
    BOOL Interest( const MString& Subject, const MString& Key );
    void Regist( const MString& Subject, const MString& Key );
    void Unregist( const MString& Subject, const MString& Key );
    UFCType::Int32 GetInterest( UFC::List<SubjectKeyPair>& SubjectKeyList );
    ///< Use this overload when accumulating across several connections so they share one dedup index
    void GetInterest( SubjectKeyAccumulator& Acc );
};
//------------------------------------------------------------------------------
class SubjectsDB
{
private:
    UFC::PtrList<ConnectionInfo> FConnectionsInfo;
    UFC::PCriticalSection        FCS;
    ///< ---- subject reverse index ----
    ///< Search used to scan every connection for each message (measured: 128 idle connections that receive
    ///< nothing cut throughput by 45%). The index turns it into one hash lookup and only checks the few
    ///< connections that actually registered that subject.
    ///< Both candidate lists are kept in ascending FSeq order; Search does a merge walk,
    ///< so the output order is exactly the same as the original linear scan.
    UFC::PHashedList<UFC::AnsiString,UFC::PtrList<ConnectionInfo>*> FSubjectIndex;
    UFC::PtrList<ConnectionInfo> FAllSubjectConns; ///< connections that registered subject == "all"
    UFCType::Int32               FNextSeq;
    ///< socket pointer -> ConnectionInfo. FindConnection used to scan the connection list linearly,
    ///< once per register/unregister; measured with 500 connections, the last connection in the list
    ///< took 11.7us to Regist versus 0.32us for the first.
    UFC::PHashedList<size_t,ConnectionInfo*>         FConnBySocket; ///< key = (size_t)socket: pointer-sized on Win64 too; hash<size_t> exists in old ext/hash_map
private:
    ConnectionInfo* FindConnection( UFC::PClientSocket* Connection );
    void  IndexAdd( const MString& Subject, ConnectionInfo* Info );
    void  IndexRemove( const MString& Subject, ConnectionInfo* Info );
    void  IndexRemoveConnection( ConnectionInfo* Info );
    void  IndexClear( void );
    void  SearchIndexed( UFC::PtrList<UFC::PClientSocket>* ConnectionList,
                         const MString& Subject, const MString& Key );
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

    for( UFCType::Int32 i =  FRecords.ItemCount() - 1; i >= 0; i-- )
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

    for( UFCType::Int32 i = FRecords.ItemCount() - 1; i >= 0; i-- )
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
    for( UFCType::Int32 i = FRecords.ItemCount() - 1; i >= 0; i-- )
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
	CSubjectRec<T>*  Record;

	for( UFCType::Int32 i = FRecords.ItemCount() - 1; i >= 0; i-- )
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
	CSubjectRec<T>*  Record;

	for( UFCType::Int32 i = FRecords.ItemCount() - 1; i >= 0; i-- )
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

