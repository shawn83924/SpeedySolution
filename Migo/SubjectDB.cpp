
#ifdef WIN32
#pragma hdrstop
#endif

#include "SubjectDB.h"
//------------------------------------------------------------------------------
//
//   SubjectsDB --- ConnectionInfo --- KeyInfo
//               |                  |- KeyInfo
//               |- ConnectionInfo
//               |- ConnectionInfo --- KeyInfo
//                                  |- KeyInfo
//                                  |- KeyInfo
//------------------------------------------------------------------------------
// Class KeyInfo
//------------------------------------------------------------------------------
KeyInfo::KeyInfo( void )
:FRegistAll( FALSE )
{
}
//------------------------------------------------------------------------------
BOOL KeyInfo::Interest( const MString& Key )
{
    if( FRegistAll == TRUE )
        return TRUE;
    return FKeys.Exists( Key );
}
//------------------------------------------------------------------------------
void KeyInfo::Regist( const MString& Key )
{
    if( Key.AnsiCompare("all") == 0 )
        FRegistAll = TRUE;
    else
        FKeys.Add( Key );
}
//------------------------------------------------------------------------------
void KeyInfo::Unregist( const MString& Key )
{
    if( Key.AnsiCompare("all") == 0 )
        FRegistAll = FALSE;
    else
        FKeys.Delete( Key );
}
//------------------------------------------------------------------------------
BOOL KeyInfo::IsEmpty()
{
    if( FRegistAll == FALSE && FKeys.ItemCount() == 0 )
        return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------
UFCType::Int32 KeyInfo::GetInterest( UFC::PStringList& KeyList )
{
    if( FRegistAll == TRUE )
    {
        KeyList.Add( "all" );
    }
    else
	{
		UFC::AnsiString Item;
		if( FKeys.First( Item ) == TRUE )
		{
			KeyList.Add( Item );
			while( FKeys.Next( Item ) == TRUE )
			{
			   KeyList.Add( Item );
			}
		}
/*		for( int i = FKeys.ItemCount()-1; i >= 0; i-- )
			 KeyList.Add( FKeys[ i ] );*/
    }
    return FKeys.ItemCount();
}
//------------------------------------------------------------------------------
//
//  Class ConnectionInfo
//
//------------------------------------------------------------------------------
ConnectionInfo::ConnectionInfo( UFC::PClientSocket* InSocket )
:FRegistAll( FALSE )
,FSubjects( 64 )
,FSocket( InSocket )
,FSeq( 0 )
{
}
//------------------------------------------------------------------------------
//  List the subjects currently registered (used by SubjectsDB to maintain the reverse index)
//------------------------------------------------------------------------------
UFCType::Int32 ConnectionInfo::GetSubjectNames( UFC::PStringList& Names )
{
    KeyInfo* Info = FSubjects.First();

    while( Info != NULL )
    {
        Names.Add( FSubjects.CurrentKey() );
        Info = FSubjects.Next();
    }
    return Names.ItemCount();
}
//------------------------------------------------------------------------------
BOOL ConnectionInfo::HasSubject( const MString& Subject )
{
    return ( FSubjects.GetObjectByKey( Subject ) != NULL ) ? TRUE : FALSE;
}
//------------------------------------------------------------------------------
ConnectionInfo::~ConnectionInfo()
{
	KeyInfo* Info = FSubjects.First();

	while( Info != NULL )
	{
		delete Info;
		Info = FSubjects.Next();
	}
    FSubjects.Clear();
}
//------------------------------------------------------------------------------
BOOL ConnectionInfo::Interest( const MString& Subject, const MString& Key )
{
    if( FRegistAll == TRUE ) ///< Interesting in all subjects.
        return TRUE;
    KeyInfo* KeyInfo = FSubjects.GetObjectByKey( Subject );

    if( KeyInfo == NULL )    ///< Not interesting in this subject.
        return FALSE;
    return KeyInfo->Interest( Key );
}
//------------------------------------------------------------------------------
void ConnectionInfo::Regist( const MString& Subject, const MString& Key )
{
    if( Subject.AnsiCompare( "all" ) == 0 ) ///< This connection interest in
    {                                       ///  all messages.
        FRegistAll = TRUE;
    }
    else
    {
        KeyInfo* KeyInfoObj = FSubjects.GetObjectByKey( Subject ); ///< Find this subject.
        if( KeyInfoObj == NULL )                                   ///< Not exists.
        {
            KeyInfoObj = new KeyInfo();                            ///< Create a new KeyInfo object.
            FSubjects.Add( Subject, KeyInfoObj );
        }
        KeyInfoObj->Regist( Key );
    }
}
//------------------------------------------------------------------------------
void ConnectionInfo::Unregist( const MString& Subject, const MString& Key )
{
    if( Subject.AnsiCompare( "all" ) == 0 ) ///< This connection not interest in
    {                                       ///  all messages.
        FRegistAll = FALSE;
    }
    else
    {
        KeyInfo* KeyInfoObj = FSubjects.GetObjectByKey( Subject ); ///< Find this subject.
        if( KeyInfoObj != NULL )                                   ///< Exists.
        {
            KeyInfoObj->Unregist( Key );  ///< Unregist it
            if( KeyInfoObj->IsEmpty())    ///< No one interest in this subject.
            {
                FSubjects.DeleteByKey( Subject ); ///< Remove this subject
                delete KeyInfoObj;                ///< delete keyInfo object.
            }
        }
    }
}
//------------------------------------------------------------------------------
//  Class SubjectKeyAccumulator
//------------------------------------------------------------------------------
SubjectKeyAccumulator::SubjectKeyAccumulator( UFC::List<SubjectKeyPair>& List )
:FList( List )
,FHasAllSubject( FALSE )
{
}
//------------------------------------------------------------------------------
MString SubjectKeyAccumulator::PairKey( const MString& Subject, const MString& Key )
{
    MString Result( Subject );

    Result += '\x01';  ///< subject / key never contain control characters, so this is a safe separator
    Result += Key;
    return Result;
}
//------------------------------------------------------------------------------
//  Equivalent to the original linear dedup, but with hash lookups instead of scans.
//
//  For key != "all" the original compared entries one by one with SubjectKeyPair::operator==; expanded,
//  that operator's semantics are exactly the three cases below (NewPair's key is known not to be "all"):
//    1. NewPair's subject is "all"      -> equal to any entry in the list
//    2. the list has subject == "all"   -> same as above
//    3. same subject and (same key, or the existing entry's key is "all")
//------------------------------------------------------------------------------
BOOL SubjectKeyAccumulator::Add( const SubjectKeyPair& NewPair )
{
    const MString& Subject = NewPair.getSubject();
    const MString& Key     = NewPair.getKey();

    if( Key == "all" )  ///< format "Subject.all"
    {
        if( FAllKeySubjects.Exists( Subject ) )
            return FALSE;                       ///< already have (Subject,"all")
        ///< Remove other entries with the same subject. This branch is rare, so it stays a linear scan.
        for( int i = FList.ItemCount() - 1; i >= 0; i-- )
        {
            if( Subject == FList[ i ].getSubject() )
            {
                FExactPairs.Delete( PairKey( Subject, FList[ i ].getKey() ) );
                FList.Delete( i );
            }
        }
        FList.Add( NewPair );
        FAllKeySubjects.Add( Subject );
        if( Subject == "all" )
            FHasAllSubject = TRUE;
        return TRUE;
    }
    ///< format "Subject.key"
    if( Subject == "all" )
    {
        if( FList.ItemCount() > 0 )             ///< case 1
            return FALSE;
    }
    else
    {
        if( FHasAllSubject )                    ///< case 2
            return FALSE;
        if( FAllKeySubjects.Exists( Subject ) ) ///< case 3 (existing entry's key is "all")
            return FALSE;
        if( FExactPairs.Exists( PairKey( Subject, Key ) ) ) ///< case 3 (same subject, same key)
            return FALSE;
    }
    FList.Add( NewPair );
    FExactPairs.Add( PairKey( Subject, Key ) );
    if( Subject == "all" )
        FHasAllSubject = TRUE;
    return TRUE;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
UFCType::Int32 ConnectionInfo::GetInterest( UFC::List<SubjectKeyPair>& SubjectKeyList )
{
    SubjectKeyAccumulator Acc( SubjectKeyList );

    GetInterest( Acc );
    return SubjectKeyList.ItemCount();
}
//------------------------------------------------------------------------------
void ConnectionInfo::GetInterest( SubjectKeyAccumulator& Acc )
{
    if( FRegistAll == TRUE)
    {
        Acc.Add( SubjectKeyPair( "all", "all" ) );
    }
    else
    {
        ///< The original walked with GetItem( Index ) from ItemCount()-1 down, but PHashedList is a hash map
        ///< underneath and indexed access walks from begin() to Index, so the whole loop was
        ///< O(subjects^2).
        ///
        ///< Now First()/CurrentKey()/Next() collect everything in one linear pass, then process in reverse --
        ///< reversed to reproduce the original processing order exactly. Order affects the dedup result (e.g.
        ///< "Subject.all" removes existing entries of the same subject), so same content in another order is not enough.
        UFC::PtrList<KeyInfo> Infos;
        UFC::PStringList      Subjects;
        UFC::PStringList      KeyList;
        KeyInfo*              KeyInfoObj = FSubjects.First();

        while( KeyInfoObj != NULL )
        {
            Infos.Add( KeyInfoObj );
            Subjects.Add( FSubjects.CurrentKey() );
            KeyInfoObj = FSubjects.Next();
        }
        for( int i = Infos.ItemCount() - 1; i >= 0; i-- )
        {
            KeyList.Clear();
            Infos.GetItem( i )->GetInterest( KeyList );
            for( int j = 0; j < KeyList.ItemCount(); j++ )
                 Acc.Add( SubjectKeyPair( Subjects[ i ], KeyList[ j ] ) );
        }
    }
}
//------------------------------------------------------------------------------
//
// Class SubjectsDB
//
// A memory DB to keep which connection interest in which subjects.
//
//------------------------------------------------------------------------------
SubjectsDB::SubjectsDB()
:FNextSeq( 0 )
{
}
//------------------------------------------------------------------------------
SubjectsDB::~SubjectsDB()
{
    UFC::PLockObject    Lock( FCS );
    IndexClear();
    FConnBySocket.Clear();
    for( int i = FConnectionsInfo.ItemCount()-1; i >= 0; i-- )
         delete FConnectionsInfo.GetItem( i );
    FConnectionsInfo.Clear();
}
//------------------------------------------------------------------------------
//
//  Reverse index maintenance
//
//  Design points:
//    1. Candidate lists are sorted by ascending FSeq. A new connection always has the largest FSeq, so
//       appending to the tail keeps the order; no extra sorting is needed.
//    2. Search merge-walks the per-subject list and the subscribed-to-"all" list,
//       so the output order matches the original scan of FConnectionsInfo exactly.
//    3. Callers always already hold FCS.
//------------------------------------------------------------------------------
void SubjectsDB::IndexAdd( const MString& Subject, ConnectionInfo* Info )
{
    UFC::PtrList<ConnectionInfo>* List;

    if( Subject.AnsiCompare( "all" ) == 0 )
        List = &FAllSubjectConns;
    else
    {
        if( ( List = FSubjectIndex.GetObjectByKey( Subject ) ) == NULL )
        {
            List = new UFC::PtrList<ConnectionInfo>();
            FSubjectIndex.Add( Subject, List );
        }
    }
    ///< Insert in ascending FSeq order. Almost always this is an append (a new connection has the largest FSeq),
    ///< but an existing connection that registers a new subject later may need to go in the middle.
    Int32 i = List->ItemCount() - 1;

    while( i >= 0 && List->GetItem( i )->GetSeq() > Info->GetSeq() )
        i--;
    if( i >= 0 && List->GetItem( i ) == Info )
        return;                                  ///< already in the list
    List->Insert( i + 1, Info );
}
//------------------------------------------------------------------------------
void SubjectsDB::IndexRemove( const MString& Subject, ConnectionInfo* Info )
{
    UFC::PtrList<ConnectionInfo>* List;

    if( Subject.AnsiCompare( "all" ) == 0 )
        List = &FAllSubjectConns;
    else if( ( List = FSubjectIndex.GetObjectByKey( Subject ) ) == NULL )
        return;

    Int32 Index = List->IndexOf( Info );
    if( Index != -1 )
        List->Delete( Index );
}
//------------------------------------------------------------------------------
void SubjectsDB::IndexRemoveConnection( ConnectionInfo* Info )
{
    UFC::PStringList Names;

    Info->GetSubjectNames( Names );
    for( int i = 0; i < Names.ItemCount(); i++ )
         IndexRemove( Names[ i ], Info );
    IndexRemove( MString( "all" ), Info );  ///< also remove it if it registered subject=="all"
}
//------------------------------------------------------------------------------
void SubjectsDB::IndexClear( void )
{
    UFC::PtrList<ConnectionInfo>* List = FSubjectIndex.First();

    while( List != NULL )
    {
        delete List;
        List = FSubjectIndex.Next();
    }
    FSubjectIndex.Clear();
    FAllSubjectConns.Clear();
}
//------------------------------------------------------------------------------
ConnectionInfo* SubjectsDB::FindConnection( UFC::PClientSocket* Connection )
{
    ///< Hash lookup instead of a linear scan. Callers always already hold FCS,
    ///< and FConnBySocket is maintained (add/remove) under FCS too, so the two stay consistent.
    return FConnBySocket.GetObjectByKey( (size_t)Connection );
}
//------------------------------------------------------------------------------
void  SubjectsDB::Regist(  RegistMessage* Header, UFC::PClientSocket* Connection )
{
    UFC::PLockObject    Lock( FCS );
    ConnectionInfo*     RegServer = FindConnection( Connection );

    if( RegServer == NULL ) ///< This connection info not exists, create a new one.
    {
        RegServer = new ConnectionInfo( Connection );
        RegServer->SetSeq( FNextSeq++ );
        FConnectionsInfo.Add( RegServer );
        FConnBySocket.Add( (size_t)Connection, RegServer );
    }
    for( int i = 0; i < Header->ItemCount(); i++ )
    {
        ///< getSubject/getKey already return const references; taking them by value copied two extra strings
        const MString& Subject = Header->getSubject( i );
        const MString& Key     = Header->getKey( i );
        RegServer->Regist( Subject, Key );
        IndexAdd( Subject, RegServer );
    }
}
//------------------------------------------------------------------------------
void  SubjectsDB::Regist(  const UFC::AnsiString& Subject, const UFC::AnsiString& Key, UFC::PClientSocket* Connection )
{
    UFC::PLockObject    Lock( FCS );
    ConnectionInfo*     RegServer = FindConnection( Connection );

    if( RegServer == NULL ) ///< This connection info not exists, create a new one.
    {
        RegServer = new ConnectionInfo( Connection );
        RegServer->SetSeq( FNextSeq++ );
        FConnectionsInfo.Add( RegServer );
        FConnBySocket.Add( (size_t)Connection, RegServer );
    }
    RegServer->Regist( Subject, Key );
    IndexAdd( Subject, RegServer );
}
//------------------------------------------------------------------------------
void  SubjectsDB::Unregist( const MString& Subject, const MString& Key, UFC::PClientSocket* Connection )
{
    UFC::PLockObject    Lock( FCS );
    ConnectionInfo*     RegServer = FindConnection( Connection );

    if( RegServer != NULL )
    {
        RegServer->Unregist( Subject, Key );
        ///< ConnectionInfo::Unregist only removes the subject when no key is left under it,
        ///< so look it up again to know whether the index entry should be withdrawn.
        ///< For subject == "all" it just turns FRegistAll off; it never stays in FSubjects.
        if( Subject.AnsiCompare( "all" ) == 0 )
        {
            if( RegServer->InterestAll() == FALSE )
                IndexRemove( Subject, RegServer );
        }
        else if( RegServer->HasSubject( Subject ) == FALSE )
            IndexRemove( Subject, RegServer );
    }
}
//------------------------------------------------------------------------------
void  SubjectsDB::RemoveConnection( UFC::PClientSocket* Connection )
{
    UFC::PLockObject    Lock( FCS );
    ConnectionInfo*     Info;

    for( int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
    {
        Info = FConnectionsInfo.GetItem( i );
        if( Info->Socket() == Connection )
        {
            IndexRemoveConnection( Info );   ///< withdraw the index first -- Info must still be alive to look up its subjects
            FConnBySocket.DeleteByKey( (size_t)Connection );
            FConnectionsInfo.Delete( i );
            delete Info;
            return;
        }
    }
}
//------------------------------------------------------------------------------
// Serach connections who interest in the given data message.
//------------------------------------------------------------------------------
//  Use the reverse index instead of scanning every connection.
//
//  There are only two kinds of candidates: connections that registered this subject, and those subscribed to "all".
//  Both lists are in ascending FSeq order, so a merge walk yields exactly the same output order as the
//  original scan of FConnectionsInfo (a connection present in both lists is emitted once).
//------------------------------------------------------------------------------
void SubjectsDB::SearchIndexed( UFC::PtrList<UFC::PClientSocket>* ConnectionList,
                                const MString& Subject, const MString& Key )
{
    UFC::PtrList<ConnectionInfo>* Exact = FSubjectIndex.GetObjectByKey( Subject );
    Int32 a = 0, b = 0;
    Int32 na = ( Exact != NULL ) ? Exact->ItemCount() : 0;
    Int32 nb = FAllSubjectConns.ItemCount();

    while( a < na || b < nb )
    {
        ConnectionInfo* Info;
        ConnectionInfo* A = ( a < na ) ? Exact->GetItem( a ) : NULL;
        ConnectionInfo* B = ( b < nb ) ? FAllSubjectConns.GetItem( b ) : NULL;

        if( A != NULL && B != NULL && A == B )   ///< same connection on both sides, take it once
        {
            Info = A; a++; b++;
        }
        else if( B == NULL || ( A != NULL && A->GetSeq() < B->GetSeq() ) )
        {
            Info = A; a++;
        }
        else
        {
            Info = B; b++;
        }
        if( Info->Interest( Subject, Key ) == TRUE )
            ConnectionList->Add( Info->Socket() );
    }
}
//------------------------------------------------------------------------------
UFCType::Int32 SubjectsDB::Search( UFC::PtrList<UFC::PClientSocket>* ConnectionList, DataMessage* Header )
{
    ///< Every message comes through here, once each for ClientDB and ServerDB.
    ///< Taking these by value used to copy 4 extra AnsiStrings per message.
    const MString&     Subject = Header->getSubject();
    const MString&     Key     = Header->getKey();
    UFC::PLockObject   Lock( FCS );

    SearchIndexed( ConnectionList, Subject, Key );
    return ConnectionList->ItemCount();
}
//------------------------------------------------------------------------------
UFCType::Int32 SubjectsDB::Search( UFC::PtrList<UFC::PClientSocket>* ConnectionList, const MString& Subject, const MString& Key )
{
    UFC::PLockObject   Lock( FCS );

    SearchIndexed( ConnectionList, Subject, Key );
    return ConnectionList->ItemCount();
}
//------------------------------------------------------------------------------
UFCType::Int32 SubjectsDB::All( UFC::PtrList<UFC::PClientSocket>* ConnectionList )
{
    UFC::PLockObject   Lock( FCS );

    for( int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
         ConnectionList->Add( FConnectionsInfo.GetItem( i )->Socket() );
    return ConnectionList->ItemCount();
}
//------------------------------------------------------------------------------
UFCType::Int32 SubjectsDB::GetInterest( UFC::List<SubjectKeyPair>& SubjectKeyList )
{
    ConnectionInfo* Info;

    UFC::PLockObject Lock( FCS );
    ///< If any connection register the "all","all", register all messages.
    for( int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
    {
        if( FConnectionsInfo.GetItem( i )->InterestAll() == TRUE )
        {
            SubjectKeyList.Add( SubjectKeyPair("all","all"));
            return SubjectKeyList.ItemCount();
        }
    }
    ///< The dedup index must be shared across all connections to replace the original O(pairs^2) linear scan
    SubjectKeyAccumulator Acc( SubjectKeyList );

    for( int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
    {
        Info = FConnectionsInfo.GetItem( i );
        Info->GetInterest( Acc );
    }
    return SubjectKeyList.ItemCount();
}
//------------------------------------------------------------------------------
UFCType::Int32 SubjectsDB::GetInterest( RegistMessage& RegMsg )
{
    UFC::PLockObject Lock( FCS );
    ///< If any connection register the "all","all", register all messages.
    for( int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
    {
        if( FConnectionsInfo.GetItem( i )->InterestAll() == TRUE )
        {
            RegMsg.Add( "all","all" );
            return 1;
        }
    }
    UFC::List<SubjectKeyPair> SubjectKeyList;
    SubjectKeyAccumulator     Acc( SubjectKeyList );

    for( int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
         FConnectionsInfo.GetItem( i )->GetInterest( Acc );
    for( int i = 0; i < SubjectKeyList.ItemCount(); i++ )
         RegMsg.Add( SubjectKeyList[i].getSubject(), SubjectKeyList[i].getKey());
    return SubjectKeyList.ItemCount();
}
//------------------------------------------------------------------------------

