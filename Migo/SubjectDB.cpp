
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
{
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
BOOL ConnectionInfo::Add( UFC::List<SubjectKeyPair>& SubjectKeyList, SubjectKeyPair NewPair )
{
    if( NewPair.getKey() == "all" )  ///< format "Subject.all"
    {
        for( register int i = SubjectKeyList.ItemCount()-1; i >= 0; i-- )
        {
            if( NewPair.getSubject() == SubjectKeyList[i].getSubject() ) ///< Has same subject.
            {
                if( SubjectKeyList[i].getKey() == "all" )
                    return FALSE;               ///< Already exists.
                else
                    SubjectKeyList.Delete( i ); ///< Remove the entry with the same subject
            }
        }
        SubjectKeyList.Add( NewPair );
    }
    else ///< format "Subject.key"
    {
        for( register int i = 0; i < SubjectKeyList.ItemCount(); i++ )
        {
            if( NewPair == SubjectKeyList[i] )
                return FALSE;
        }
        SubjectKeyList.Add( NewPair );
    }
    return TRUE;
}
//------------------------------------------------------------------------------
UFCType::Int32 ConnectionInfo::GetInterest( UFC::List<SubjectKeyPair>& SubjectKeyList )
{
    if( FRegistAll == TRUE)
    {
        Add( SubjectKeyList, SubjectKeyPair( "all", "all" ) );
    }
    else
    {
        for( register int i = FSubjects.ItemCount()-1; i >= 0; i-- )
        {
             UFC::PStringList KeyList;
             UFC::AnsiString  Subject;
             KeyInfo*         KeyInfoObj;
             if( FSubjects.GetItem( i, Subject, KeyInfoObj ) == TRUE )
             {
                 KeyInfoObj->GetInterest( KeyList );
                 for( register int j = 0;j < KeyList.ItemCount(); j++ )
                      Add( SubjectKeyList, SubjectKeyPair( Subject, KeyList[j] ) );
             }
        }
    }
    return SubjectKeyList.ItemCount();
}
//------------------------------------------------------------------------------
//
// Class SubjectsDB
//
// A memory DB to keep which connection interest in which subjects.
//
//------------------------------------------------------------------------------
SubjectsDB::SubjectsDB()
{
}
//------------------------------------------------------------------------------
SubjectsDB::~SubjectsDB()
{
    UFC::PLockObject    Lock( FCS );
    for( register int i = FConnectionsInfo.ItemCount()-1; i >= 0; i-- )
         delete FConnectionsInfo.GetItem( i );
    FConnectionsInfo.Clear();
}
//------------------------------------------------------------------------------
ConnectionInfo* SubjectsDB::FindConnection( UFC::PClientSocket* Connection )
{
    ConnectionInfo* Info;
    for( register int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
    {
        Info = FConnectionsInfo.GetItem( i );
        if( Info->Socket() == Connection )
            return Info;
    }
    return NULL;
}
//------------------------------------------------------------------------------
void  SubjectsDB::Regist(  RegistMessage* Header, UFC::PClientSocket* Connection )
{
    UFC::PLockObject    Lock( FCS );
    ConnectionInfo*     RegServer = FindConnection( Connection );

    if( RegServer == NULL ) ///< This connection info not exists, create a new one.
    {
        RegServer = new ConnectionInfo( Connection );
        FConnectionsInfo.Add( RegServer );
    }
    for( int i = 0; i < Header->ItemCount(); i++ )
    {
        const MString Subject = Header->getSubject( i );
        const MString Key = Header->getKey( i );
        RegServer->Regist( Subject, Key );
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
        FConnectionsInfo.Add( RegServer );
    }
    RegServer->Regist( Subject, Key );
}
//------------------------------------------------------------------------------
void  SubjectsDB::Unregist( const MString& Subject, const MString& Key, UFC::PClientSocket* Connection )
{
    UFC::PLockObject    Lock( FCS );
    ConnectionInfo*     RegServer = FindConnection( Connection );

    if( RegServer != NULL )
        RegServer->Unregist( Subject, Key );
}
//------------------------------------------------------------------------------
void  SubjectsDB::RemoveConnection( UFC::PClientSocket* Connection )
{
    UFC::PLockObject    Lock( FCS );
    ConnectionInfo*     Info;

    for( register int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
    {
        Info = FConnectionsInfo.GetItem( i );
        if( Info->Socket() == Connection )
        {
            FConnectionsInfo.Delete( i );
            delete Info;
            return;
        }
    }
}
//------------------------------------------------------------------------------
// Serach connections who interest in the given data message.
//------------------------------------------------------------------------------
UFCType::Int32 SubjectsDB::Search( UFC::PtrList<UFC::PClientSocket>* ConnectionList, DataMessage* Header )
{
    ConnectionInfo* Info;
    const MString Subject =  Header->getSubject();
    const MString Key     =  Header->getKey();
    UFC::PLockObject   Lock( FCS );

    for( register int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
    {
        Info = FConnectionsInfo.GetItem( i );
        if( Info->Interest( Subject, Key ) == TRUE )
            ConnectionList->Add( Info->Socket() );
    }
    return ConnectionList->ItemCount();
}
//------------------------------------------------------------------------------
UFCType::Int32 SubjectsDB::Search( UFC::PtrList<UFC::PClientSocket>* ConnectionList, const MString& Subject, const MString& Key )
{
    ConnectionInfo* Info;    
    UFC::PLockObject   Lock( FCS );

    for( register int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
    {
        Info = FConnectionsInfo.GetItem( i );
        if( Info->Interest( Subject, Key ) == TRUE )
            ConnectionList->Add( Info->Socket() );
    }
    return ConnectionList->ItemCount();
}
//------------------------------------------------------------------------------
UFCType::Int32 SubjectsDB::All( UFC::PtrList<UFC::PClientSocket>* ConnectionList )
{
    UFC::PLockObject   Lock( FCS );

    for( register int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
         ConnectionList->Add( FConnectionsInfo.GetItem( i )->Socket() );
    return ConnectionList->ItemCount();
}
//------------------------------------------------------------------------------
UFCType::Int32 SubjectsDB::GetInterest( UFC::List<SubjectKeyPair>& SubjectKeyList )
{
    ConnectionInfo* Info;

    UFC::PLockObject Lock( FCS );
    ///< If any connection register the "all","all", register all messages.
    for( register int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
    {
        if( FConnectionsInfo.GetItem( i )->InterestAll() == TRUE )
        {
            SubjectKeyList.Add( SubjectKeyPair("all","all"));
            return SubjectKeyList.ItemCount();
        }
    }
    for( register int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
    {
        Info = FConnectionsInfo.GetItem( i );
        Info->GetInterest( SubjectKeyList );
    }
    return SubjectKeyList.ItemCount();
}
//------------------------------------------------------------------------------
UFCType::Int32 SubjectsDB::GetInterest( RegistMessage& RegMsg )
{
    UFC::PLockObject Lock( FCS );
    ///< If any connection register the "all","all", register all messages.
    for( register int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
    {
        if( FConnectionsInfo.GetItem( i )->InterestAll() == TRUE )
        {
            RegMsg.Add( "all","all" );
            return 1;
        }
    }
    UFC::List<SubjectKeyPair> SubjectKeyList;
    for( register int i = 0; i < FConnectionsInfo.ItemCount(); i++ )
         FConnectionsInfo.GetItem( i )->GetInterest( SubjectKeyList );
    for( register int i = 0; i < SubjectKeyList.ItemCount(); i++ )
         RegMsg.Add( SubjectKeyList[i].getSubject(), SubjectKeyList[i].getKey());
    return SubjectKeyList.ItemCount();
}
//------------------------------------------------------------------------------

