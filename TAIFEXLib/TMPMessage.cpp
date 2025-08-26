
#include "TMPTypes.h"
#include "TMPMessage.h"

#include "../UFC/TFixedFormat.h"
#include "../UFC/TParseData.h"
#ifndef _FOR_FCM
    #include "sqlite3.h"
#endif
        
//---------------------------------------------------------------------------
#if !defined(CLOCK_MONOTONIC_COARSE)    
    #define CLOCK_MONOTONIC_COARSE CLOCK_MONOTONIC
#endif    
#if !defined(CLOCK_REALTIME_COARSE)    
    #define CLOCK_REALTIME_COARSE CLOCK_REALTIME
#endif            
//-----------------------------------------------------------------------------------------
namespace TMP
{
//------------------------------------------------------------------------------------------------------
// Class TMPMessage
//
//------------------------------------------------------------------------------------------------------
TMPDataFormat* TMPMessage::Formats[ 256 ];
BOOL           TMPMessage::EnableCheckSum = TRUE;
BOOL           TMPMessage::EnableFLEX = FALSE;
BOOL           TMPMessage::OptionMarket = TRUE;
BOOL           TMPMessage::IsFUTPA7 = FALSE;
BOOL           TMPMessage::IsOPTPA7 = FALSE;
//------------------------------------------------------------------------------------------------------
UFC::PHashedList<UFC::AnsiString, TMPDataFormat*> TMPMessage::FormatMap;
UFC::PHashMap<UFC::AnsiString, int>               TMPMessage::URLMap;
//------------------------------------------------------------------------------------------------------
UFC::AnsiString                                   TMPMessage::NoFCM( "F000999" );
UFC::PHashMap<UFC::AnsiString, int>               TMPMessage::FCMMap;
UFC::PHashedList<int, UFC::AnsiString*>           TMPMessage::FCMIndex;
//------------------------------------------------------------------------------------------------------
UFC::AnsiString                                   TMPMessage::NoFUTSymbol( "XXFXX" );
UFC::AnsiString                                   TMPMessage::NoOPTSymbol( "XXO00000XX" );
UFC::PHashedList<UFC::AnsiString, SymbolInfo*>    TMPMessage::SymbolMap;
UFC::PHashedList<int, SymbolInfo*>                TMPMessage::OPTSymbolIndex;
UFC::PHashedList<int, SymbolInfo*>                TMPMessage::FUTSymbolIndex;
UFC::PHashedList<int, SymbolInfo*>                TMPMessage::OPTOffHourSymbolIndex;
UFC::PHashedList<int, SymbolInfo*>                TMPMessage::FUTOffHourSymbolIndex;
UFC::UDateTime                                    TMPMessage::FUTP06Time;     
UFC::UDateTime                                    TMPMessage::FUTP07Time;     
UFC::UDateTime                                    TMPMessage::FUTP08Time;         
UFC::UDateTime                                    TMPMessage::FUTOffHourP08Time;         
UFC::UDateTime                                    TMPMessage::OPTP06Time;     
UFC::UDateTime                                    TMPMessage::OPTP07Time;     
UFC::UDateTime                                    TMPMessage::OPTP08Time;   
UFC::UDateTime                                    TMPMessage::OPTOffHourP08Time;   
FutP08Loader                                      TMPMessage::FUTLoader;
OptP08Loader                                      TMPMessage::OPTLoader;
UFC::PCriticalSection                             TMPMessage::SymbolLock;
//----------------------------------------------------------------------------------------------------------
PThreadObjectPool<TMPR01Message>*                  TMPMessage::R01PoolPtr = NULL;
PThreadObjectPool<TMPR07Message>*                  TMPMessage::R07PoolPtr = NULL;
PThreadObjectPool<TMPR09Message>*                  TMPMessage::R09PoolPtr = NULL;
PThreadObjectPool<TMPRX01Message>*                 TMPMessage::RX01PoolPtr = NULL;
PThreadObjectPool<TMPRX07Message>*                 TMPMessage::RX07PoolPtr = NULL;
PThreadObjectPool<TMPRX09Message>*                 TMPMessage::RX09PoolPtr = NULL;
//----------------------------------------------------------------------------------------------------------
void TMPMessage::InitializeThreadPool()
{
    TMPMessage::R01PoolPtr = new PThreadObjectPool<TMPR01Message>( 2048, 1024 );
    TMPMessage::R07PoolPtr = new PThreadObjectPool<TMPR07Message>( 512,  128 );
    TMPMessage::R09PoolPtr = new PThreadObjectPool<TMPR09Message>( 2048, 1024 );
    
    TMPMessage::RX01PoolPtr = new PThreadObjectPool<TMPRX01Message>( 512, 256 );
    TMPMessage::RX07PoolPtr = new PThreadObjectPool<TMPRX07Message>( 256, 128 );
    TMPMessage::RX09PoolPtr = new PThreadObjectPool<TMPRX09Message>( 512, 256 );
}  
//----------------------------------------------------------------------------------------------------------
void UpdateFileTime( UFC::UDateTime& FileTime, const UFC::AnsiString& FileName )
{
    struct stat FileStat;    

    stat( FileName.c_str(), &FileStat ); 
#ifdef __AIX
    FileTime = UFC::UDateTime( (time_t)FileStat.st_mtime, 0 );
#elif defined ( _WIN32 )
    FileTime = UFC::UDateTime( (time_t)FileStat.st_mtime, 0 );
#else    
    FileTime = UFC::UDateTime( (time_t)FileStat.st_mtim.tv_sec, FileStat.st_mtim.tv_nsec/1000 );
#endif        
}
//------------------------------------------------------------------------------------------------------
void FutP08Loader::OnFileChanged( const UFC::AnsiString& FileName, void* data )
{   
    UFC::BufferedLog::Printf( " Reload Futures Off-Hour P08[%s]",FileName.c_str() );    
    UpdateFileTime( TMPMessage::FUTOffHourP08Time, FileName );    
    TMPMessage::LoadP08File(  FALSE, TRUE, FileName, *((UFC::AnsiString*)data) );    
}
//------------------------------------------------------------------------------------------------------
void OptP08Loader::OnFileChanged( const UFC::AnsiString& FileName, void* data )
{        
    UFC::BufferedLog::Printf( " Reload Options Off-Hour P08[%s]",FileName.c_str() );    
    UpdateFileTime( TMPMessage::OPTOffHourP08Time, FileName );    	    
    TMPMessage::LoadP08File(  TRUE, TRUE, FileName, *((UFC::AnsiString*)data) );    
}
//------------------------------------------------------------------------------------------------------
//
// Constructor
//
// Construct a TMP message from Name = Value style string.
// or use a message name to create a TMP Message.( Like "L10","R01"... )     
//------------------------------------------------------------------------------------------------------
TMPMessage::TMPMessage( const UFC::AnsiString& TMPString )
{
    UFC::PStringList Strings;
    UFC::AnsiString  Name,Value;   
    Int32            Index;
    UInt8            CheckSum;
    
    Strings.SetStrings( TMPString, "|\n" );
    if( Strings.ItemCount() == 1 )
    {
        if((FFormat = FormatMap.GetObjectByKey( TMPString )) ==NULL )
        {
            UFC::AnsiString Msg;                
            Msg.Printf( "TMP Parser error, Parser of Message Name[%s] not exists.",TMPString.c_str());  
            throw( TMPStringParserException( Msg.c_str() ) );
        }
        else    
        {
            FMsgSeq  = TMPUInt32( (UInt32)0 );
            FMsgType = TMPUInt8( FFormat->GetMessageType());
            FFormat->AddTMPFields( FFields );
            return;
        }
    }
    else if( Strings.ItemCount() < 7 )
        throw( TMPStringParserException( "Invalid TMP string, fields less then 7.") );
    ///< parse msg_length field.
    SpiltNameValue( Strings[0], Name, Value );
    if( Name !=  "msg_length" )
        throw( TMPStringParserException( "TMP string missing msg_length field.") );
    FMsgLen = TMPUInt16( Value );
    ///< parse MsgSeqNum field.
    SpiltNameValue( Strings[1], Name, Value );
    if( Name !=  "MsgSeqNum" )
        throw( TMPStringParserException( "TMP string missing MsgSeqNum field.") );
    FMsgSeq = TMPUInt32( Value );
    ///< parse msg_time field.
    SpiltNameValue( Strings[2], Name, Value );
    if( Name !=  "msg_time" )
        throw( TMPStringParserException( "TMP string missing msg_time field.") );
    TMPTime MsgTime( Value );
    FMsgTime = MsgTime;
    ///< parse MessageType field.
    SpiltNameValue( Strings[3], Name, Value );
    if( Name !=  "MessageType" )
        throw( TMPStringParserException( "TMP string missing MessageType field.") );
    FMsgType = TMPUInt8( Value );    
    ///< parse fcm_id field.
    SpiltNameValue( Strings[4], Name, Value );
    if( Name !=  "fcm_id" )
        throw( TMPStringParserException( "TMP string missing fcm_id field.") );    
    FFCMID = TMPUInt16( Value );    
    ///< parse session_id field.
    SpiltNameValue( Strings[5], Name, Value );
    if( Name !=  "session_id" )
        throw( TMPStringParserException( "TMP string missing fcm_id field.") );    
    FSessionID = TMPUInt16( Value );    
    ///< Get the fornmat object.
	FFormat = Formats[ FMsgType.ToUInt8() ];
    if( FFormat == NULL )
    {
        UFC::AnsiString Msg;                
		Msg.Printf( "TMP Parser error, Parser of type[%u] not exists.",FMsgType.ToUInt8());
        throw( TMPStringParserException( Msg.c_str() ) );
    }    
    ///< Add all fields of this message.
    FFormat->AddTMPFields( FFields );        
    for( int i = 6; i < Strings.ItemCount() - 1; i++ )
    {
        SpiltNameValue( Strings[i], Name, Value );
        if( (Index = FFormat->GetFieldIndex( Name )) == -1 )        
        {
            UFC::AnsiString Msg;
	    Msg.Printf( "TMP string MessageType=%u don't contains field %s",FMsgType.ToUInt8(), Name.c_str());
            throw( TMPStringParserException( Msg ) );
        }
        else
            FFields.GetItem( Index )->FromString( Value );
    }
    ///< Parse CheckSum field.
    SpiltNameValue( Strings[ Strings.ItemCount() - 1], Name, Value );
    if( Name !=  "CheckSum" )
        throw( TMPStringParserException( "TMP string missing CheckSum field.") );
    FCheckSum = TMPUInt8( Value );
	CheckSum  = (UInt8)(HeaderCheckSum( ) + BodyCheckSum( ));
	if( EnableCheckSum == TRUE && CheckSum != FCheckSum.ToUInt8() )
		throw( TMPCheckSumException( CheckSum, FCheckSum.ToUInt8() ) );
}

//------------------------------------------------------------------------------------------------------
//
// Constructor
//
// Construct a TMP message from binary stream.
//------------------------------------------------------------------------------------------------------
TMPMessage::TMPMessage( UFC::PStream* Stream )
:FMsgLen( Stream )
,FMsgSeq( Stream )
,FMsgTime( Stream)
,FMsgType( Stream )
,FFCMID( Stream ) 
,FSessionID( Stream )
,FFormat( Formats[FMsgType.ToUInt8()] )///< Get the fornmat object.
{
    if( FFormat == NULL )
    {
        UFC::AnsiString Msg;                
		Msg.Printf( "TMP Parser error, Parser of type[%u] not exists.",FMsgType.ToUInt8());
        throw( TMPStringParserException( Msg.c_str() ) );
    }
    FFormat->AddTMPFieldsFromStream( FFields, Stream );
    FCheckSum = TMPUInt8( Stream );                
}

//------------------------------------------------------------------------------------------------------
//
// Constructor
// Construct a TMP message from binary stream.
// Add By Zhen Fan 2009.04.20
//------------------------------------------------------------------------------------------------------
TMPMessage::TMPMessage( UFC::PStream* Stream, UFC::AnsiString )
:FMsgType( Stream )
,FFormat( Formats[FMsgType.ToUInt8()] )///< Get the fornmat object.
{
    if( FFormat == NULL )
    {
        UFC::AnsiString Msg;
        Msg.Printf( "TMP Parser error, Parser of type[%u] not exists.",FMsgType.ToUInt8());
        throw( TMPStringParserException( Msg.c_str() ) );
    }
    FFormat->AddTMPFieldsFromStream( FFields, Stream );
}
//----------------------------------------------------------------------------------------------------------
//
// Constructor
//
// Use message type to create a TMP message.
//------------------------------------------------------------------------------------------------------
TMPMessage::TMPMessage( TMPMessageType Type )
:FMsgSeq( (UInt32)0 )
,FMsgType( (UInt8)Type )
,FFormat( Formats[ Type ] )///< Get the fornmat object.
{
    if( FFormat == NULL )
    {
        UFC::AnsiString Msg;                
	Msg.Printf( "TMP Parser error, Parser of type[%u] not exists.",FMsgType.ToUInt8());
        throw( TMPStringParserException( Msg.c_str() ) );
    }    
    FFormat->AddTMPFields( FFields );
}
//----------------------------------------------------------------------------------------------------------
//
// Copy Constructor
//
// Duplicate a TMP message.
//--------------------------------------------------------------------------------------------
TMPMessage::TMPMessage( TMPMessage& Ref )
:FMsgLen( Ref.FMsgLen )
,FMsgSeq( Ref.FMsgSeq )
,FMsgTime( Ref.FMsgTime )
,FMsgType( Ref.FMsgType )
,FFCMID( Ref.FFCMID )
,FSessionID( Ref.FSessionID )
,FCheckSum( Ref.FCheckSum )
,FFormat( Ref.FFormat )
{
    for( register int i = 0; i < Ref.FFields.ItemCount(); i++ )
        FFields.Add( Ref.FFields.GetItem(i)->Duplicate() );
}
//--------------------------------------------------------------------------------------------
TMPMessage::~TMPMessage( void )
{
    for( register int i = 0; i < FFields.ItemCount(); i++ )
        delete FFields.GetItem( i );    
}
//--------------------------------------------------------------------------------------------
void TMPMessage::SpiltNameValue( const UFC::AnsiString& Line, UFC::AnsiString& Name, UFC::AnsiString& Value )
{
     int  Pos = Line.AnsiPos( "=" );
     
     if( Pos != -1 )
     {
        Name  = UFC::AnsiString( Line.c_str(), Pos );
        Value = UFC::AnsiString( Line.c_str() + Pos + 1, Line.Length() - Pos -1 );
        Name.TrimLeft();
        Name.TrimRight();
        Value.TrimLeft();
        Value.TrimRight();     
     }
     else
     {
         UFC::AnsiString Msg;
                
         Msg.Printf( "TMP field missing '=', field string[%s]",Line.c_str());  
         throw( TMPStringParserException( Msg.c_str() ));
     }    
}     
//----------------------------------------------------------------------------------------------------------
UInt8 TMPMessage::HeaderCheckSum( void )
{
    UFC::MemoryStream HeaderStream( 16 );
    
    FMsgLen.ToStream( &HeaderStream ); // Size:2
    FMsgSeq.ToStream( &HeaderStream ); // Size:4
    FMsgTime.ToStream( &HeaderStream );// Size:6
    FMsgType.ToStream( &HeaderStream );// Size:1    
    FFCMID.ToStream( &HeaderStream );  // Size:2    
    FSessionID.ToStream( &HeaderStream );// Size:2    
    return CalculateCheckSum( (UInt8*)HeaderStream.GetBuffer(), HeaderStream.GetSize());
}
//----------------------------------------------------------------------------------------------------------
UInt8 TMPMessage::CalculateCheckSum( UInt8* Data, Int32 Length )
{
    if( EnableCheckSum == FALSE )
        return 0;
    register UInt8 Result = 0;
    for( register int i= 0; i < Length; i++ )
         Result += *(Data+i);
    return Result;
}
//----------------------------------------------------------------------------------------------------------
UInt8 TMPMessage::BodyCheckSum( void )
{
    if( EnableCheckSum == TRUE )
    {
        UFC::MemoryStream BodyStream( 256 );
    
        for( register int i = 0; i < FFields.ItemCount(); i++ )
             FFields.GetItem( i )->ToStream( &BodyStream );
        return CalculateCheckSum( (UInt8*)BodyStream.GetBuffer(),BodyStream.GetSize());
    }
    else
        return 0;
}
//----------------------------------------------------------------------------------------------------------
Int32 TMPMessage::ToStream( UFC::PStream* Stream )
{
    UFC::MemoryStream BodyStream( 256 );

    FMsgLen = (UInt16)GetMessageSize( );
    FMsgLen.ToStream( &BodyStream ); // Size:2
    FMsgSeq.ToStream( &BodyStream ); // Size:4
    FMsgTime.ToStream( &BodyStream );// Size:6
    FMsgType.ToStream( &BodyStream );// Size:1    
    FFCMID.ToStream( &BodyStream );// Size:2    
    FSessionID.ToStream( &BodyStream );// Size:2
    for( register int i = 0; i < FFields.ItemCount(); i++ )
        FFields.GetItem( i )->ToStream( &BodyStream );
    Stream->Write( BodyStream.GetBuffer(), BodyStream.GetSize());    
    FCheckSum = CalculateCheckSum( (UInt8*)BodyStream.GetBuffer(), BodyStream.GetSize());
    FCheckSum.ToStream( Stream ); // Size:1
    return BodyStream.GetSize();
}
//----------------------------------------------------------------------------------------------------------
Int32 TMPMessage::ToStreamString( UFC::PStream* Stream, UFC::AnsiString& TMPString )
{
    UFC::MemoryStream BodyStream( 256 );
    char              FieldStr[ TMP_FIELD_STR_MAX ];
    char              HeaderStr[ 128 ];
    char              TimeStr[ 32 ];
    TMPType*          Field;
    std::stringstream MessageStr;		

    ///< Header Stream
    FMsgLen = (UInt16)GetMessageSize( );
    FMsgLen.ToStream( &BodyStream );   // Size:2
    FMsgSeq.ToStream( &BodyStream );   // Size:4
    FMsgTime.ToStream( &BodyStream );  // Size:6
    FMsgType.ToStream( &BodyStream );  // Size:1    
    FFCMID.ToStream( &BodyStream );    // Size:2    
    FSessionID.ToStream( &BodyStream );// Size:2
    ///< Header String
    FMsgTime.ToString( TimeStr, 32 );    
    sprintf( HeaderStr, 
             "msg_length=%d|MsgSeqNum=%d|msg_time=%s|MessageType=%d|fcm_id=%d|session_id=%d|",
             FMsgLen.ToInteger(),
             FMsgSeq.ToInteger(),
             TimeStr,
             FMsgType.ToInteger(),
             FFCMID.ToInteger(), 
             FSessionID.ToInteger());    
    MessageStr << HeaderStr;    
    for( register int i = 0; i < FFields.ItemCount(); i++ )
    {
        Field = FFields.GetItem( i );                
        ///< Body Stream
        Field->ToStream( &BodyStream );
        ///< Bosy String
        Field->ToString( FieldStr, TMP_FIELD_STR_MAX );        
        MessageStr << FFormat->GetFieldName( i ).c_str() <<"="<< FieldStr <<"|";
    }        
    Stream->Write( BodyStream.GetBuffer(), BodyStream.GetSize());    
    ///< Calculate check sum
    FCheckSum = CalculateCheckSum( (UInt8*)BodyStream.GetBuffer(), BodyStream.GetSize());
    FCheckSum.ToStream( Stream ); // Size:1
    MessageStr << "CheckSum="<< FCheckSum.ToInteger() <<"|";
    TMPString = MessageStr.str().c_str();
    return BodyStream.GetSize();
}
//----------------------------------------------------------------------------------------------------------
Int32 TMPMessage::ToString( UFC::AnsiString& TMPString )
{
    char              FieldStr[ TMP_FIELD_STR_MAX ];
    char              HeaderStr[ 128 ];
    char              TimeStr[ 32 ];
    TMPType*          Field;
    std::stringstream MessageStr;		

    ///< Convert msg_length field to string.
	FMsgLen = (UInt16)GetMessageSize( );
    FMsgTime.ToString( TimeStr, 32 );
    sprintf( HeaderStr, "msg_length=%d|MsgSeqNum=%d|msg_time=%s|MessageType=%d|fcm_id=%d|session_id=%d|",
            FMsgLen.ToInteger(),FMsgSeq.ToInteger(),TimeStr,FMsgType.ToInteger(),FFCMID.ToInteger(), FSessionID.ToInteger());
    MessageStr << HeaderStr;
    for( register int i = 0; i < FFields.ItemCount(); i++ )
    {
        Field = FFields.GetItem( i );                
        Field->ToString( FieldStr, TMP_FIELD_STR_MAX );        
        MessageStr << FFormat->GetFieldName( i ).c_str() <<"="<< FieldStr <<"|";
    }        
    ///< Convert CheckSum field to string.    
    MessageStr << "CheckSum="<< ((HeaderCheckSum() + BodyCheckSum())%255) <<"|";
    TMPString = MessageStr.str().c_str();
    return TMPString.Length();
}
//----------------------------------------------------------------------------------------------------------
Int32 TMPMessage::GetMessageSize( void )
{
    if( FFormat->IsFixLength() == FALSE)
    {
        register Int32 MsgSize = TMP_HEADER_SIZE;        
        for( register int i = 0; i < FFields.ItemCount(); i++ )    
             MsgSize += FFields.GetItem( i )->GetSize();
        return MsgSize;
    }        
    else
        return FFormat->GetMessageLength( );
}
//----------------------------------------------------------------------------------------------------------
BOOL TMPMessage::FieldExists( const UFC::AnsiString& Name )
{
    if( FFormat->GetFieldIndex( Name ) != -1 )
        return TRUE;
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------
TMPType* TMPMessage::GetField( const UFC::AnsiString& Name )
{
    int Index = FFormat->GetFieldIndex( Name );
    if( Index != -1 )
        return FFields.GetItem( Index );
    else
        throw( TMPFieldNotFountException( Name.c_str()) );
}
//----------------------------------------------------------------------------------------------------------
TMPType* TMPMessage::GetField( Int32 Index )
{
    if( Index < 0 || Index >= FFields.ItemCount( ))
        throw( TMPIndexOutOfBoundException( Index, FFields.ItemCount( )) );
    return FFields.GetItem( Index );
}
//---------------------------------------------------------------------------
TMPType& TMPMessage::operator [](const int index)
{
    return *GetField( index );
}
//---------------------------------------------------------------------------
TMPType& TMPMessage::operator [](const AnsiString& Name )
{
    return *GetField( Name );
}
//----------------------------------------------------------------------------------------------------------
void TMPMessage::SetField(  const AnsiString& Name, const AnsiString& Value )
{
    int Index = FFormat->GetFieldIndex( Name );
    if( Index != -1 )
    {
        TMPType* Field = FFields.GetItem( Index );
        Field->FromString( Value );
    }
    else    
        throw( TMPFieldNotFountException(Name.c_str()) );
}
//----------------------------------------------------------------------------------------------------------
void TMPMessage::SetField( const AnsiString& Name, Int32 Value )
{
    int Index = FFormat->GetFieldIndex( Name );
    if( Index != -1 )
    {
        TMPType* Field = FFields.GetItem( Index );
        Field->FromInteger( Value );
    }
    else    
        throw( TMPFieldNotFountException(Name.c_str()) );    
}
//----------------------------------------------------------------------------------------------------------
void TMPMessage::SetField( const AnsiString& Name, TMPType& Val  )
{
    int Index = FFormat->GetFieldIndex( Name );
    if( Index != -1 )
    {
        TMPType* Field = FFields.GetItem( Index );
        Field->Assign( Val );
    }
    else
        throw( TMPFieldNotFountException(Name.c_str()) );
}
//----------------------------------------------------------------------------------------------------------
void TMPMessage::SetField( const AnsiString& Name, TMPType* Val  )
{
    int Index = FFormat->GetFieldIndex( Name );
    if( Index != -1 )
    {
        TMPType* Field = FFields.GetItem( Index );
        Field->Assign( *Val );
    }
    else
        throw( TMPFieldNotFountException(Name.c_str()) );
}
//----------------------------------------------------------------------------------------------------------
void TMPMessage::SetField( Int32 Index, const AnsiString& Value )
{
    TMPType* Field = FFields.GetItem( Index );
    if( Field != NULL )
        Field->FromString( Value );
    else
        throw( TMPFieldNotFountException( Index ) );
}
//----------------------------------------------------------------------------------------------------------
void TMPMessage::SetField( Int32 Index, Int32 Value )
{
    TMPType* Field = FFields.GetItem( Index );
    if( Field != NULL )    
        Field->FromInteger( Value );        
    else
        throw( TMPFieldNotFountException( Index ) );
}
//----------------------------------------------------------------------------------------------------------
void TMPMessage::SetField( Int32 Index, TMPType& Value )
{
    TMPType* Field = FFields.GetItem( Index );
    if( Field != NULL )
        Field->Assign( Value );
    else
        throw( TMPFieldNotFountException( Index ) );
}
//----------------------------------------------------------------------------------------------------------
void TMPMessage::SetField( Int32 Index, TMPType* Value )
{
    TMPType* Field = FFields.GetItem( Index );
    if( Field != NULL )
        Field->Assign( *Value );
    else
        throw( TMPFieldNotFountException( Index ) );
}
//----------------------------------------------------------------------------------------------------------
//
//  static functions.
//  Static functions for TMP message.
//----------------------------------------------------------------------------------------------------------
void TMPMessage::InitTMPMessageLibrary(  const TMPLibMarket LibraryMarket  )
{
    static BOOL AlreadyInit = FALSE;
    UFC::PStringList Files;
    Int32            Count = FileList( "../TMPFormats", ".ini", Files );

    if( AlreadyInit == FALSE )
    {
        UFC::BufferedLog::Printf( " %d TMP Message format file found.", Count );
        ///< Set all TMPDataFormat pointer to NULL.
        for( Int32 i = 0; i< 256; i++ )
             Formats[ i ] = NULL;
        ///< Load all format file in this directory.
        for( Int32 i = 0; i < Count; i++ )
        {
            try
            {
                UFC::AnsiString FileName;

                FileName.Printf( "%s/%s", "../TMPFormats", Files.GetItem( i ).c_str() );
				if( FileName.SubString( (UInt16)(FileName.Length() - 3) ,3) == "ini" )
                {
                    UFC::BufferedLog::Printf( " Loading TMP Message format file[%s].", FileName.c_str() );
                    TMPDataFormat* NewFormat = new TMPDataFormat( FileName );
                    if( Formats[ NewFormat->GetMessageType() ] != NULL )
                        UFC::BufferedLog::Printf( " Warning! Format for Type:%d already exists.", NewFormat->GetMessageType() );
                    else
                        Formats[ NewFormat->GetMessageType() ] = NewFormat;
                    FormatMap.Add( NewFormat->GetMessageName(), NewFormat );
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Load TMP Message [%s] OK.", NewFormat->GetMessageName().c_str() );
                }                
            }
            catch( TMPConfigException& e )
            {
                UFC::BufferedLog::Printf(" %s.", e.what() );
            }
        }
        UFC::BufferedLog::Printf( " [%d] TMP Message formats loaded.", Count );
        AlreadyInit = TRUE;
        FCMIndex.Clear();
        OPTSymbolIndex.Clear();
        FUTSymbolIndex.Clear();
        OPTOffHourSymbolIndex.Clear();
        FUTOffHourSymbolIndex.Clear();
        SymbolMap.Clear();
        if( LibraryMarket == tlmOptions )
        {
            UFC::BufferedLog::Printf( " Load BrokerID table P06." );
            if( LoadFCMFromFile( TRUE, "../ftOPT" ) == FALSE) ///< Load P06
                exit( 0 );
            LoadSymbolFromFile( TRUE, "../ftOPT", "../cfg" );///< Load P08
        }
        else if( LibraryMarket == tlmFutures )
        {
            UFC::BufferedLog::Printf( " Load BrokerID table P06." );
            if( LoadFCMFromFile( FALSE, "../ftFUT" ) == FALSE )///< Load P06
                exit( 0 );
            LoadSymbolFromFile( FALSE, "../ftFUT", "../cfg" );///< Load P08
        }
        else
        {
            UFC::BufferedLog::Printf( " Load BrokerID table P06." );
            if( LoadFCMFromFile( TRUE,  "../ftOPT" ) == FALSE || 
                LoadFCMFromFile( FALSE, "../ftFUT" ) == FALSE )///< Load P06
                exit( 0 );
            LoadSymbolFromFile( TRUE,  "../ftOPT", "../cfg" );
            LoadSymbolFromFile( FALSE, "../ftFUT", "../cfg" );///< Load P08
        }
    }
    else
        UFC::BufferedLog::Printf( " TMP Message library already initialize." );
}
//----------------------------------------------------------------------------------------------------------
UInt16 TMPMessage::GetBrokerID( const UFC::AnsiString& BrokerID )
{    
    int BrokerIndex = 0;

    if( BrokerID.Length() == 7 &&    
        FCMMap.GetObjectByKey( BrokerID, BrokerIndex ) == TRUE )
        return (UInt16)BrokerIndex;    
    return FCM_NOT_EXIST;
}
//----------------------------------------------------------------------------------------------------------
const UFC::AnsiString& TMPMessage::GetBrokerID( UInt16 FCMID )
{
    UFC::AnsiString* BrokerIDPtr = FCMIndex.GetObjectByKey( FCMID );
    if( BrokerIDPtr == NULL )
        return NoFCM; ///< Symbol not exists, return "XXXXXXXXXX         ".
    else
        return *BrokerIDPtr;
}
//----------------------------------------------------------------------------------------------------------
UInt16 TMPMessage::GetCMID( const UFC::AnsiString& CMID, const UFC::AnsiString& BrokerID )
{
    UFC::AnsiString CMIDStr( CMID );
    UFC::AnsiString FCMID;
    int             Index;        
    
    CMIDStr.PadThis( 4, '0' );
    if( BrokerID.AnsiPos( "999" ) == 4 ) ///< for PT 
    {
        ///< Check FXXX999.
        FCMID.Printf( "%s999", CMIDStr.c_str() );
        if( FCMMap.GetObjectByKey( FCMID, Index ) == TRUE )
            return (UInt16) Index;
        ///< Check FXXX000.
        FCMID.Printf( "%s000", CMIDStr.c_str() );
        if( FCMMap.GetObjectByKey( FCMID, Index ) == TRUE  )
            return (UInt16) Index;
    }
    else
    {
        ///< Check FXXX000 first.
        FCMID.Printf( "%s000", CMIDStr.c_str() );
        if( FCMMap.GetObjectByKey( FCMID, Index ) == TRUE  )
            return (UInt16) Index;
        ///< Others.
        FCMID.Printf( "%s999", CMIDStr.c_str() );
        if( FCMMap.GetObjectByKey( FCMID, Index ) == TRUE )
            return (UInt16) Index;        
    }
    ///< Others.
    for( register int i = 1; i < 999; i++ ) ///< Try from FXXX001 to FXXX998
    {
        FCMID.Printf( "%s03d", CMIDStr.c_str(), i );
        if( FCMMap.GetObjectByKey( FCMID, Index ) == TRUE )
            return (UInt16) Index;
    }
    UFC::BufferedLog::Printf( " CMID[%s] not found!", CMIDStr.c_str() );  
    return 0;
}
//----------------------------------------------------------------------------------------------------------
UInt16 TMPMessage::GetSymbol( const UFC::AnsiString& Symbol )
{   ///< 0 :means not exists.
	UFC::AnsiString Product( Symbol );

	Product.TrimRight();
    UFC::PLockObject Lock( TMPMessage::SymbolLock );
    
	SymbolInfo* InfoPtr = SymbolMap.GetObjectByKey( Product );
	if( InfoPtr == NULL )
		return 0;
	return (UInt16)(InfoPtr->Index);
}
//----------------------------------------------------------------------------------------------------------
UInt16 TMPMessage::GetSymbol( const UFC::AnsiString& Symbol, int& Decimal )
{   ///< 0 :means not exists.
	UFC::AnsiString Product( Symbol );

	Product.TrimRight();
    UFC::PLockObject Lock( TMPMessage::SymbolLock );
	SymbolInfo* InfoPtr = SymbolMap.GetObjectByKey( Product );
	if( InfoPtr == NULL )
	{
		Decimal = 2;
		return 0;
	}
	Decimal = InfoPtr->DecimalLocator;
	return (UInt16)(InfoPtr->Index);
}
//----------------------------------------------------------------------------------------------------------
const UFC::AnsiString& TMPMessage::GetSymbol( BOOL IsOptions, BOOL IsOffHour, UInt16 SymbolID )
{
    SymbolInfo* InfoPtr;
	if( IsOptions == TRUE )
	{
        UFC::PLockObject Lock( TMPMessage::SymbolLock );        
		
        if( IsOffHour == FALSE )   
            InfoPtr = OPTSymbolIndex.GetObjectByKey( SymbolID );
        else
            InfoPtr = OPTOffHourSymbolIndex.GetObjectByKey( SymbolID );
		if( InfoPtr == NULL )
			return NoOPTSymbol; ///< Symbol not exists, return "XXXXXXXXXX         ".
		else
			return InfoPtr->Symbol;
    }
    else
    {
        UFC::PLockObject Lock( TMPMessage::SymbolLock );
        
		if( IsOffHour == FALSE )   
            InfoPtr = FUTSymbolIndex.GetObjectByKey( SymbolID );
        else
            InfoPtr = FUTOffHourSymbolIndex.GetObjectByKey( SymbolID );
		if( InfoPtr == NULL )
            return NoFUTSymbol; ///< Symbol not exists, return "XXXXXXXXXX         ".
        else
			return InfoPtr->Symbol;
    }
}
//----------------------------------------------------------------------------------------------------------
BOOL TMPMessage::SymbolExists( const UFC::AnsiString& Symbol )
{
    UFC::PLockObject Lock( TMPMessage::SymbolLock );
    return SymbolMap.IsExists( Symbol );
}
//----------------------------------------------------------------------------------------------------------
BOOL TMPMessage::SymbolExists( BOOL IsOptions, BOOL IsOffHour, UInt16 SymbolID )
{
    UFC::PLockObject Lock( TMPMessage::SymbolLock );
    if( IsOptions == TRUE )
    {
        if( IsOffHour == FALSE )       
            return OPTSymbolIndex.IsExists( SymbolID );
        else
            return OPTOffHourSymbolIndex.IsExists( SymbolID );
    }        
    else
    {
        if( IsOffHour == FALSE )           
            return FUTSymbolIndex.IsExists( SymbolID );
        else
            return FUTOffHourSymbolIndex.IsExists( SymbolID );
    }
}        
//----------------------------------------------------------------------------------------------------------
void TMPMessage::ToHexString( UFC::AnsiString& Result, ///[Out]
                              UInt8*           Data,   ///[in]
                              Int32            Length )///[in]
{
    char* HexStr = new char[ Length*2+3 ];
      
    *HexStr     = '0';
    *(HexStr+1) = 'x';
    for( register int i = 0; i < Length; i ++ )
         sprintf( HexStr+( 2*i )+2, "%02X", *(Data +i) );
    Result = UFC::AnsiString( HexStr, Length*2+2 );    
    delete [] HexStr;
}
//-----------------------------------------------------------------------------------------------------------
BOOL TMPMessage::LoadFCMFromFile( BOOL IsOptions,  const UFC::AnsiString& FilePath  )
{  
    UFC::AnsiString FileName;

    if( IsOptions == TRUE )
        FileName.Printf("%s/P06.10", FilePath.c_str() );
    else
        FileName.Printf("%s/P06.20", FilePath.c_str() );
    if( UFC::FileExists(FileName) == TRUE )
    {   
        if( IsOptions == TRUE )
            UpdateFileTime( TMPMessage::OPTP06Time,FileName );
        else
            UpdateFileTime( TMPMessage::FUTP06Time,FileName );
       
        UFC::FileStream P06File( FileName, O_RDONLY );
        char Buffer[16];
    
        while( P06File.Read( Buffer ,12) == 12 )
        {
            UFC::AnsiString  BrokerID( Buffer, 7 );
            int              Index   = UFC::AnsiString( Buffer + 7, 5 ).ToInt();
        
            BrokerID.PadThis( 7, '0' );
            FCMIndex.Add( Index, new UFC::AnsiString(BrokerID) );
            FCMMap.Add( BrokerID, (int)Index );
        }
        return TRUE;
    }
    else
    {
        UFC::BufferedLog::Printf( " Missing BrokerID file[%s].", FileName.c_str() );  
        UFC::BufferedLog::FlushToFile();
        return FALSE;
    }
        
}
//--------------------------------------------------------------------------------------------------------------------
void TMPMessage::LoadSymbolFromFile(  BOOL IsOptions, const UFC::AnsiString& FilePath,const UFC::AnsiString& CfgPath  )
{  
    UFC::AnsiString FileName,OHFileName,FormatFile;

    if( IsOptions == TRUE )
    {
        FileName.Printf("%s/P08.10", FilePath.c_str());
        OHFileName.Printf("%s/P08.11", FilePath.c_str());
        FormatFile.Printf( "%s/Options_P08.ini", CfgPath.c_str() );
    }
    else
    {
        FileName.Printf("%s/P08.20", FilePath.c_str());
        OHFileName.Printf("%s/P08.21", FilePath.c_str());
        FormatFile.Printf( "%s/Futures_P08.ini", CfgPath.c_str());
    }
    if( UFC::FileExists( FileName ) == TRUE &&  UFC::FileExists( FormatFile ) == TRUE )
    {
        if( IsOptions == TRUE )
            UpdateFileTime( TMPMessage::OPTP08Time,  FileName );                        
        else
            UpdateFileTime( TMPMessage::FUTP08Time,  FileName );                   
        LoadP08File(  IsOptions, false, FileName, FormatFile );
        if( UFC::FileExists( OHFileName ) == TRUE  )
        {            
            if( IsOptions == TRUE )
                UpdateFileTime( OPTOffHourP08Time,  FileName );                                        
            else
                UpdateFileTime( FUTOffHourP08Time,  FileName );                                                        
            LoadP08File(  IsOptions, true, OHFileName, FormatFile );
        }
        if( IsOptions == TRUE )
            new FileChangedWatchDog( OHFileName, &OPTLoader, new AnsiString(FormatFile), 10 );
        else
            new FileChangedWatchDog( OHFileName, &FUTLoader, new AnsiString(FormatFile), 10 );
	}
	else
	{
        if( UFC::FileExists( FileName ) == FALSE )
            UFC::BufferedLog::Printf( " Missing P08 file[%s].", FileName.c_str() );
        if( UFC::FileExists( FormatFile ) == FALSE)
            UFC::BufferedLog::Printf( " Missing Format file[%s].", FormatFile.c_str() );
        UFC::BufferedLog::FlushToFile();
        exit( 0 );
    }
}
//-----------------------------------------------------------------------------------------------------------
void TMPMessage::LoadP08File(  BOOL IsOptions, BOOL IsOffHour, const UFC::AnsiString& File,const UFC::AnsiString& Format  )
{  
    UFC::PHashedList<int,SymbolInfo*>* SymbolIndex;
    UFC::TParseData   P08ParseData;
    UFC::FileStream   P08FileStream( File, O_RDONLY );
    UFC::TFixedFormat P08DataFormat( Format );
    UFC::TDataset*    pDataset;        
        
    if( IsOptions == TRUE )
    {
        if( IsOffHour == TRUE )
            SymbolIndex = &OPTOffHourSymbolIndex;
        else
            SymbolIndex = &OPTSymbolIndex;
    }
    else
    {
        if( IsOffHour == TRUE )
            SymbolIndex = &FUTOffHourSymbolIndex;
        else    
            SymbolIndex = &FUTSymbolIndex;
    }
    P08ParseData.SetDataFormat( &P08DataFormat );
    if( (pDataset = P08ParseData.Parse( &P08FileStream )) != NULL )
    {        
        int nCount = pDataset->GetRecordCount();
        int Index, Decimal;
        UFC::TRecord* pRecord;
        UFC::AnsiString  Symbol,DecimalLocator,PSeq;
        UFC::PLockObject Lock( TMPMessage::SymbolLock );
        
        for( register int i = 0; i < nCount; i++ )
        {
            pRecord = pDataset->GetRecord( i );        
            if( pRecord->GetField( "CommId", Symbol ) == true && 
                pRecord->GetField( "DecimalLocator", DecimalLocator ) == true &&
                pRecord->GetField( "PSeq", PSeq )  == true )
            {   
                Index = PSeq.ToInt();
                if( SymbolIndex->IsExists( Index ) == false )///< Contract index not exists
                {
                    SymbolInfo* Syminfo;
                    
                    Decimal = DecimalLocator.ToInt();
                    Symbol.TrimRight();
                    Syminfo = new SymbolInfo(  Symbol,  Index, Decimal );
                    SymbolIndex->Add( Index, Syminfo );
                    SymbolMap.Add( Symbol, Syminfo );
                    UFC::BufferedLog::DebugPrintf( UFC::dlInformation, " Add Symbol[%s] Decimal[%d] Index[%d] ", Symbol.c_str(), Decimal, Index );                
                }        
            }    
        }
    }        
}        
//-----------------------------------------------------------------------------------------------------------
#ifndef _FOR_FCM
//-----------------------------------------------------------------------------------------------------------
void TMPMessage::LoadFCMFromSQLite(  const UFC::AnsiString& DBFileName  )
{
	char*           SQLiteErrorStr;
	int             FCMCount = 0;
	sqlite3*        HandleSQLite;

	if( sqlite3_open( DBFileName.c_str(), &HandleSQLite ) == SQLITE_OK )
	{
		UFC::AnsiString QuerySQL( "BEGIN;" );

		if( sqlite3_exec( HandleSQLite, QuerySQL.c_str(), NULL, 0, &SQLiteErrorStr ) != SQLITE_OK )
		{
			UFC::BufferedLog::Printf( " SQL error: %s", SQLiteErrorStr);
			exit( 0 );
		}
		QuerySQL = "SELECT BRK_NO, BRK_SEQ, BRK_OPEN_CODE from BRK order by BRK_SEQ";
		if( sqlite3_exec( HandleSQLite, QuerySQL.c_str(), SQLiteFCMcallback, (void*)&FCMCount, &SQLiteErrorStr) != SQLITE_OK )
		{
			UFC::BufferedLog::Printf( " SQL error: %s", SQLiteErrorStr);
			exit( 0 );
		}
		QuerySQL = "COMMIT;";
		if( sqlite3_exec( HandleSQLite, QuerySQL.c_str(), NULL, 0, &SQLiteErrorStr)!=SQLITE_OK )
		{
			UFC::BufferedLog::Printf( " SQL error: %s", SQLiteErrorStr );
			exit( 0 );
		}
		UFC::BufferedLog::Printf( " Add %d BrokerID to BrokerID,BrokerIndex map.",  FCMCount );
		sqlite3_close( HandleSQLite );
	}
}
//--------------------------------------------------------------------------------------------------------------------
int TMPMessage::SQLiteFCMcallback( void* count, int , char **argv, char ** )
{
	int*             FCMCount = (int*)count;
	UFC::AnsiString  BrokerID( argv[ 0 ], 7 );
	int              Index   = UFC::AnsiString( argv[ 1 ] ).ToInt();
	UFC::AnsiString  OpenCode( argv[ 2 ], 1 );

	if( OpenCode[0] == 'Y')
	{
		BrokerID.PadThis( 7, '0' );
		FCMIndex.Add( Index, new UFC::AnsiString(BrokerID) );
		FCMMap.Add( BrokerID, (int*)((int)Index) );
		*(FCMCount) = *(FCMCount) + 1;
	}
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------
void TMPMessage::LoadSymbolFromSQLite(  const UFC::AnsiString& DBFileName  )
{
	char*           SQLiteErrorStr;
    int             SymbolCount = 0;    
    sqlite3*        HandleSQLite;
    
    if( sqlite3_open( DBFileName.c_str(), &HandleSQLite ) == SQLITE_OK )
    {
        UFC::AnsiString QuerySQL( "BEGIN;" );
    
        if( sqlite3_exec( HandleSQLite, QuerySQL.c_str(), NULL, 0, &SQLiteErrorStr ) != SQLITE_OK )
        {
            UFC::BufferedLog::Printf( " SQL error: %s", SQLiteErrorStr);
            UFC::BufferedLog::FlushToFile();
            exit( 0 );
        }
        QuerySQL = "SELECT PROD_ID,PROD_PGSEQ FROM PROD where PROD_EXPIRE_CODE != 'Y' order by PROD_PGSEQ";
        if( sqlite3_exec( HandleSQLite, QuerySQL.c_str(), SQLiteSymbolcallback, (void*)&SymbolCount, &SQLiteErrorStr) != SQLITE_OK )
        {
            UFC::BufferedLog::Printf( " SQL error: %s", SQLiteErrorStr);
            UFC::BufferedLog::FlushToFile();
            exit( 0 );
        }    
        QuerySQL = "COMMIT;";
        if( sqlite3_exec( HandleSQLite, QuerySQL.c_str(), NULL, 0, &SQLiteErrorStr)!=SQLITE_OK )
        {
            UFC::BufferedLog::Printf( " SQL error: %s", SQLiteErrorStr );
            UFC::BufferedLog::FlushToFile();
            exit( 0 );
        }    
        UFC::BufferedLog::Printf( " Add %d Symbols to Symbol,Symbol Index map.", SymbolCount );  
        sqlite3_close( HandleSQLite );    
    }
}
//--------------------------------------------------------------------------------------------------------------------
int TMPMessage::SQLiteSymbolcallback( void* count, int , char **argv, char ** )
{
	int*             SymbolCount = (int*)count;
	UFC::AnsiString  Symbol( argv[ 0 ], 10 );
	int              Index   = UFC::AnsiString( argv[ 1 ] ).ToInt();

	Symbol.TrimRight();
	SymbolIndex.Add( Index, new UFC::AnsiString( Symbol ));
	SymbolMap.Add( Symbol, (int*)((int)Index) );
	if( SymbolIndex.ItemCount() != Index )
	{
		UFC::BufferedLog::DebugPrintf( UFC::dlFatalError, " Symbol[%s] outof order, Should be[%d] but get[%d]", Symbol.c_str(),SymbolIndex.ItemCount(),Index );
		UFC::BufferedLog::FlushToFile();
		exit( 0 );
	}
	*(SymbolCount) = *(SymbolCount) + 1;
	return 0;
}
//----------------------------------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------------------------------
void TMPMessage::LoadURLTable( BOOL IsOptions, const UFC::AnsiString& BrokerID )
{
    UFC::AnsiString FCM( BrokerID );
    UFC::AnsiString FileName,OffHourFileName;
    FILE* FFileHandle;    
    char  Buffer[ 512 ];

    FCM.PadThis( 4,'0');
    if( IsOptions == TRUE )
    {
        FileName.Printf( "../ftOPT/PA7.10.%s", FCM.c_str());
        OffHourFileName.Printf( "../ftOPT/PA7.11.%s", FCM.c_str());
        IsOPTPA7 = TRUE;
        if( UFC::FileExists( FileName ) == FALSE )
        {
            FileName.Printf( "../ftOPT/P07.10.%s", FCM.c_str());
            IsOPTPA7 = FALSE;
        }
        else
            UFC::BufferedLog::Printf( " Load Options backup URL table file[%s].", FileName.c_str() );
        if( UFC::FileExists( OffHourFileName ) == FALSE )        
            OffHourFileName.Printf( "../ftOPT/P07.11.%s", FCM.c_str());
        else
            UFC::BufferedLog::Printf( " Load Options off-hour backup URL table file[%s].", OffHourFileName.c_str() );
    }
    else
    {
        FileName.Printf( "../ftFUT/PA7.20.%s", FCM.c_str());
        OffHourFileName.Printf( "../ftFUT/PA7.21.%s", FCM.c_str());
        IsFUTPA7 = TRUE;
        if( UFC::FileExists( FileName ) == FALSE )
        {
            FileName.Printf( "../ftFUT/P07.20.%s", FCM.c_str());
            IsFUTPA7 = FALSE;
        }
        else
            UFC::BufferedLog::Printf( " Load Futures backup URL table file[%s].", FileName.c_str() );
        if( UFC::FileExists( OffHourFileName ) == FALSE )        
            OffHourFileName.Printf( "../ftFUT/P07.21.%s", FCM.c_str());
        else
            UFC::BufferedLog::Printf( " Load Futures off-hour backup URL table file[%s].", OffHourFileName.c_str() );
    }
    ///< Load normal session P07/PA7
    if( (FFileHandle = fopen( FileName.c_str(), "r" )) != NULL ) 
    {
        if( IsOptions == TRUE )
            UpdateFileTime( OPTP07Time,  FileName );            
        else
            UpdateFileTime( FUTP07Time,  FileName );    
        while( fgets( Buffer, 512, FFileHandle ) != NULL ) 
        {
            UFC::PStringList Strings;

            Strings.SetStrings( Buffer , ",\n" );
            if( Strings.ItemCount() == 2 && Strings[1].ToInt() != 0 )        
                URLMap.Add( Strings[0], (int)Strings[1].ToInt() );   
        }
        fclose( FFileHandle );
    }
    else
        UFC::BufferedLog::Printf( " Can't open URL table file \"%s\".", FileName.c_str() );
    ///< Load off-hour session P07/PA7
    if( (FFileHandle = fopen( OffHourFileName.c_str(), "r" )) != NULL ) 
    {
        while( fgets( Buffer, 512, FFileHandle ) != NULL ) 
        {
            UFC::PStringList Strings;

            Strings.SetStrings( Buffer , ",\n" );
            if( Strings.ItemCount() == 2 && Strings[1].ToInt() != 0 )        
                URLMap.Add( Strings[0], (int)Strings[1].ToInt() );   
        }
        fclose( FFileHandle );        
    }
    else
        UFC::BufferedLog::Printf( " Can't open Off-hour URL table file \"%s\".", OffHourFileName.c_str() );        
    
}
//----------------------------------------------------------------------------------------------------------
BOOL TMPMessage::GetURLPort( BOOL IsOptions, const UFC::AnsiString& BrokerID, UInt16 SessionID, Int32 Backup, UFC::AnsiString& URL, UInt16& Port, BOOL IsTEST, BOOL IsCM , BOOL IsOffHour  )
{      
    UFC::AnsiString BrokerIDLower( BrokerID );
    UFC::AnsiString Protocol;
    UFC::AnsiString Market;
    UFC::AnsiString Exchange;
    int             PeerPort;

    BrokerIDLower.LowerThis();
    if( IsOptions == TRUE )
        Market = "opt";
    else
        Market = "fut";
    if( IsCM == TRUE )
    {
        if( Backup == 0 )
            Protocol = "tmpcm";
        else
            Protocol.Printf("tmpcmbak%d", Backup );
    }
    else
    {
        if( Backup == 0 )
            Protocol = "tmp";
        else
            Protocol.Printf("tmpbak%d", Backup );
    }
    if( IsOffHour == TRUE )
        Exchange = "taifex1";
    else
        Exchange = "taifex";
    if( IsTEST == TRUE )
        URL.Printf( "%s.test.session%d.%s.%s.%s", BrokerIDLower.c_str(), SessionID, Protocol.c_str(),Market.c_str(),Exchange.c_str()  );
    else
        URL.Printf( "%s.session%d.%s.%s.%s", BrokerIDLower.c_str(), SessionID, Protocol.c_str(),Market.c_str(),Exchange.c_str() );   
    if( URLMap.GetObjectByKey( URL, PeerPort ) == FALSE )
        return FALSE; ///< URL not exists
    Port = PeerPort;
    return TRUE;
}
//------------------------------------------------------------------------------------------------------
//
//  Fast version TMP Message
//
//----------------------------------------------------------------------------------------------------------
//  TMP Message R01
//----------------------------------------------------------------------------------------------------------
void TMPR01Message::Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq )
{
    int      Count = Length() -1;
    register UFC::UInt8 Sum = 0;
    struct timespec now;
    
    if( ExecType.Get() != 'I' ) 
    {
        seq = seq +1;
        MsgSeqNum.Set( seq );    
    }
    else
        MsgSeqNum.Set( 0 ); ///< Query    
    clock_gettime( CLOCK_REALTIME_COARSE ,&now ); ///< Get the current time.   
    
    epoch_s.Set( now.tv_sec );
    ms.Set( (int)now.tv_nsec/1000000 );    
    fcm_id_h.Set( fcmid );
    session_id.Set( sid );        
    for( register int i = 0; i < Count; i++ )
         Sum += *(FRowData+i);        
    CheckSum.Set( Sum );
}
//------------------------------------------------------------------------------------------------------
int TMPR01Message::ToString( char* Buf, int Len )
{
    char         Timestr[ 16 ];///< HH:MM:SS.mmm
    char         OrderID[ 6 ];
    char         Symbol[ 21 ];
    char         InfoSource[ 4 ];
    struct tm    time;
    time_t       Epoch = epoch_s.Get();
    TMPSymbol    SymbolObj( symbol_type.Get(), symbol.Data() );    
    
    localtime_r( &Epoch, &time );    
    snprintf( Timestr, 16, "%02d:%02d:%02d.%03d", time.tm_hour, time.tm_min, time.tm_sec, ms.Get() );    
    order_no.Get( OrderID );
    info_souce.Get( InfoSource );
    SymbolObj.ToString( Symbol, 20 );    
    return snprintf( Buf, Len,
              "msg_length=77|MsgSeqNum=%d|msg_time=%s|MessageType=101|fcm_id=%d|session_id=%d|ExecType=%c|cm_id=%d|fcm_id=%d|order_no=%s|ord_id=%d|user_define=%lld|Symbol=%s|Price=%d|qty=%d|investor_acno=%d|investor_flag=%c|Side=%d|OrdType=%d|TimeInForce=%d|PositionEffect=%c|order_source=%c|info_source=%s|CheckSum=%d|"
              ,MsgSeqNum.Get()
              ,Timestr
              ,fcm_id_h.Get()
              ,session_id.Get()
              ,ExecType.Get()
              ,cm_id.Get()
              ,fcm_id.Get()
              ,OrderID
              ,ord_id.Get()
              ,user_define.Get()
              ,Symbol
              ,Price.Get()
              ,qty.Get()
              ,investor_acno.Get()
              ,investor_flag.Get()
              ,Side.Get()
              ,Ordtype.Get()
              ,TimeInForce.Get()
              ,PositionEffect.Get()
              ,order_souce.Get()
              ,InfoSource
              ,CheckSum.Get() );
}
//------------------------------------------------------------------------------------------------------
void TMPR01Message::Recycle( void )
{
    if( TMP::TMPMessage::R01PoolPtr != NULL )
        TMP::TMPMessage::R01PoolPtr->Recycle( this );
}  
//----------------------------------------------------------------------------------------------------------
//  TMP Message R07
//----------------------------------------------------------------------------------------------------------
void TMPR07Message::Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq )
{
    int      Count = Length() -1;
    register UFC::UInt8 Sum = 0;
    struct timespec now;

    MsgSeqNum.Set( 0 );
    clock_gettime( CLOCK_REALTIME_COARSE ,&now ); ///< Get the current time.            
    epoch_s.Set( now.tv_sec );
    ms.Set( (int)now.tv_nsec/1000000 );
    fcm_id_h.Set( fcmid );
    session_id.Set( sid );    
    for( register int i = 0; i < Count; i++ )
         Sum += *(FRowData+i);        
    CheckSum.Set( Sum );
}
//------------------------------------------------------------------------------------------------------
int TMPR07Message::ToString( char* Buf, int Len )
{
    char         Timestr[ 16 ];///< HH:MM:SS.mmm
    char         OrderID[ 6 ];
    char         Symbol[ 21 ];
    char         InfoSource[ 4 ];
    struct tm    time;
    time_t       Epoch = epoch_s.Get();
    TMPSymbol    SymbolObj( symbol_type.Get(), symbol.Data() );
    
    localtime_r( &Epoch, &time );    
    snprintf( Timestr, 16, "%02d:%02d:%02d.%03d", time.tm_hour, time.tm_min, time.tm_sec, ms.Get() );    
    order_no.Get( OrderID );    
    info_souce.Get( InfoSource );    
    SymbolObj.ToString( Symbol, 20 );    
    return snprintf( Buf, Len,
              "msg_length=51|MsgSeqNum=%d|msg_time=%s|MessageType=107|fcm_id=%d|session_id=%d|order_no=%s|ord_id=%d|fcm_id=%d|Symbol=%s|order_source=%c|info_source=%s|CheckSum=%d|"
              ,MsgSeqNum.Get()
              ,Timestr
              ,fcm_id_h.Get()
              ,session_id.Get()            
              ,OrderID
              ,ord_id.Get()
              ,fcm_id.Get()
              ,Symbol
              ,order_souce.Get()
              ,InfoSource
              ,CheckSum.Get() );
            
}            
//------------------------------------------------------------------------------------------------------
void TMPR07Message::Recycle( void )
{
    if( TMP::TMPMessage::R07PoolPtr != NULL )
        TMP::TMPMessage::R07PoolPtr->Recycle( this );
}  
//----------------------------------------------------------------------------------------------------------
//  TMP Message R09
//----------------------------------------------------------------------------------------------------------
void TMPR09Message::Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq )
{
    int      Count = Length() -1;
    register UFC::UInt8 Sum = 0;
    struct timespec now;

    if( ExecType.Get() != 'I' ) 
    {
        seq = seq +1;
        MsgSeqNum.Set( seq );    
    }
    else
        MsgSeqNum.Set( 0 ); ///< Query    
    MsgSeqNum.Set( seq );
    clock_gettime( CLOCK_REALTIME_COARSE ,&now ); ///< Get the current time.            
    epoch_s.Set( now.tv_sec );
    ms.Set( (int)now.tv_nsec/1000000 );
    fcm_id_h.Set( fcmid );
    session_id.Set( sid );        
    for( register int i = 0; i < Count; i++ )
         Sum += *(FRowData+i);        
    CheckSum.Set( Sum );
}
//------------------------------------------------------------------------------------------------------
int TMPR09Message::ToString( char* Buf, int Len )
{
    char         Timestr[ 16 ];///< HH:MM:SS.mmm
    char         OrderID[ 6 ];
    char         Symbol[ 21 ];
    char         InfoSource[ 4 ];
    struct tm    time;
    time_t       Epoch = epoch_s.Get();
    TMPSymbol    SymbolObj( symbol_type.Get(), symbol.Data() );
    
    localtime_r( &Epoch, &time );    
    snprintf( Timestr, 16, "%02d:%02d:%02d.%03d", time.tm_hour, time.tm_min, time.tm_sec, ms.Get());    
    order_no.Get( OrderID );    
    info_souce.Get(InfoSource);    
    SymbolObj.ToString( Symbol, 20 );    
    return snprintf( Buf, Len,
              "msg_length=81|MsgSeqNum=%d|msg_time=%s|MessageType=109|fcm_id=%d|session_id=%d|ExecType=%c|cm_id=%d|fcm_id=%d|order_no=%s|ord_id=%d|user_define=%lld|Symbol=%s|BidPx=%d|OfferPx=%d|BidSize=%d|OfferSize=%d|investor_acno=%d|investor_flag=%c|TimeInForce=%d|PositionEffect=%c|order_source=%c|info_source=%s|CheckSum=%d|"
              ,MsgSeqNum.Get()
              ,Timestr
              ,fcm_id_h.Get()
              ,session_id.Get()
              ,ExecType.Get()
              ,cm_id.Get()
              ,fcm_id.Get()
              ,OrderID
              ,ord_id.Get()
              ,user_define.Get()
              ,Symbol
              ,BidPx.Get()
              ,OfferPx.Get()
              ,BidSize.Get()
              ,OfferSize.Get()
              ,investor_acno.Get()
              ,investor_flag.Get()
              ,TimeInForce.Get()
              ,PositionEffect.Get()
              ,order_souce.Get()
              ,InfoSource
              ,CheckSum.Get() );
}            
//------------------------------------------------------------------------------------------------------
void TMPR09Message::Recycle( void )
{
    if( TMP::TMPMessage::R09PoolPtr != NULL )
        TMP::TMPMessage::R09PoolPtr->Recycle( this );
}  
//----------------------------------------------------------------------------------------------------------
//  FLEX Message RX07
//----------------------------------------------------------------------------------------------------------
void TMPRX01Message::Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq )
{
    int      Count = Length() -1;
    register UFC::UInt8 Sum = 0;
    struct timespec now;
    
    MsgSeqNum.Set( 0 );    
    clock_gettime( CLOCK_REALTIME_COARSE ,&now ); ///< Get the current time.            
    epoch_s.Set( now.tv_sec );
    ms.Set( (int)now.tv_nsec/1000000 );    
    msg_time_ns.Set( (int)now.tv_nsec );    
    fcm_id_h.Set( fcmid );
    session_id.Set( sid );        
    for( register int i = 0; i < Count; i++ )
         Sum += *(FRowData+i);        
    CheckSum.Set( Sum );
}
//------------------------------------------------------------------------------------------------------
int TMPRX01Message::ToString( char* Buf, int Len )
{
    char         Timestr[ 16 ];///< HH:MM:SS.mmm
    char         OrderID[ 6 ];
    char         Symbol[ 21 ];
    char         Filler[ 16 ];
    struct tm    time;
    time_t       Epoch = epoch_s.Get();
    
    localtime_r( &Epoch, &time );    
    snprintf( Timestr, 16, "%02d:%02d:%02d.%03d", time.tm_hour, time.tm_min, time.tm_sec, ms.Get() );    
    order_no.Get( OrderID );
    symbol.Get( Symbol );    
    filler.Get( Filler );    
    return snprintf( Buf, Len,
              "msg_length=%d|MsgSeqNum=%d|msg_time=%s|MessageType=201|fcm_id=%d|session_id=%d|MsgTypeExt=%d|preserve=%d|msg_time_ns=%d|ExecType=%c|cm_id=%d|fcm_id=%d|order_no=%s|ord_id=%d|user_define=%lld|Symbol=%s|Price=%d|qty=%d|investor_acno=%d|investor_flag=%c|Side=%d|OrdType=%d|TimeInForce=%d|PositionEffect=%c|filler=%s|CheckSum=%d|"
              ,msg_length.Get()
              ,MsgSeqNum.Get()
              ,Timestr
              ,fcm_id_h.Get()
              ,session_id.Get()
              ,MsgTypeExt.Get()
              ,preserve.Get()
              ,msg_time_ns.Get()
              ,ExecType.Get()
              ,cm_id.Get()
              ,fcm_id.Get()
              ,OrderID
              ,ord_id.Get()
              ,user_define.Get()
              ,Symbol
              ,Price.Get()
              ,qty.Get()
              ,investor_acno.Get()
              ,investor_flag.Get()
              ,Side.Get()
              ,Ordtype.Get()
              ,TimeInForce.Get()
              ,PositionEffect.Get()              
              ,Filler  
              ,CheckSum.Get() );
}
//------------------------------------------------------------------------------------------------------
void TMPRX01Message::Recycle( void )
{
    if( TMP::TMPMessage::RX01PoolPtr != NULL )
        TMP::TMPMessage::RX01PoolPtr->Recycle( this );
}  
//----------------------------------------------------------------------------------------------------------
//  FLEX Message RX07
//----------------------------------------------------------------------------------------------------------
void TMPRX07Message::Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq )
{
    int      Count = Length() -1;
    register UFC::UInt8 Sum = 0;
    struct timespec now;

    MsgSeqNum.Set( 0 );
    clock_gettime( CLOCK_REALTIME_COARSE ,&now ); ///< Get the current time.            
    epoch_s.Set( now.tv_sec );
    ms.Set( (int)now.tv_nsec/1000000 );
    msg_time_ns.Set( (int)now.tv_nsec );    
    fcm_id_h.Set( fcmid );
    session_id.Set( sid );    
    for( register int i = 0; i < Count; i++ )
         Sum += *(FRowData+i);        
    CheckSum.Set( Sum );
}
//------------------------------------------------------------------------------------------------------
int TMPRX07Message::ToString( char* Buf, int Len )
{
    char         Timestr[ 16 ];///< HH:MM:SS.mmm
    char         OrderID[ 6 ];
    char         Symbol[ 21 ];
    char         Filler[ 16 ];
    struct tm    time;
    time_t       Epoch = epoch_s.Get();
    
    localtime_r( &Epoch, &time );    
    snprintf( Timestr, 16, "%02d:%02d:%02d.%03d", time.tm_hour, time.tm_min, time.tm_sec, ms.Get() );    
    order_no.Get( OrderID );    
    symbol.Get( Symbol );    
    filler.Get( Filler );   
    return snprintf( Buf, Len,
              "msg_length=%d|MsgSeqNum=%d|msg_time=%s|MessageType=207|fcm_id=%d|session_id=%d|MsgTypeExt=%d|preserve=%d|msg_time_ns=%d|order_no=%s|ord_id=%d|fcm_id=%d|Symbol=%s|filler=%s|CheckSum=%d|"
              ,msg_length.Get()
              ,MsgSeqNum.Get()
              ,Timestr
              ,fcm_id_h.Get()
              ,session_id.Get()            
              ,MsgTypeExt.Get()
              ,preserve.Get()
              ,msg_time_ns.Get()            
              ,OrderID
              ,ord_id.Get()
              ,fcm_id.Get()
              ,Symbol ///< [20]
              ,Filler ///< [8]
              ,CheckSum.Get() );
            
}            
//------------------------------------------------------------------------------------------------------
void TMPRX07Message::Recycle( void )
{
    if( TMP::TMPMessage::RX07PoolPtr != NULL )
        TMP::TMPMessage::RX07PoolPtr->Recycle( this );
}  
//----------------------------------------------------------------------------------------------------------
//  FLEX Message RX09
//----------------------------------------------------------------------------------------------------------
void TMPRX09Message::Update( UFC::UInt16 fcmid, UFC::UInt16 sid, UFC::Int32& seq )
{
    int      Count = Length() -1;
    register UFC::UInt8 Sum = 0;
    struct timespec now;

    MsgSeqNum.Set( 0 );
    clock_gettime( CLOCK_REALTIME_COARSE ,&now ); ///< Get the current time.            
    epoch_s.Set( now.tv_sec );
    ms.Set( (int)now.tv_nsec/1000000 );
    msg_time_ns.Set( (int)now.tv_nsec );    
    fcm_id_h.Set( fcmid );
    session_id.Set( sid );        
    for( register int i = 0; i < Count; i++ )
         Sum += *(FRowData+i);        
    CheckSum.Set( Sum );
}
//------------------------------------------------------------------------------------------------------
int TMPRX09Message::ToString( char* Buf, int Len )
{
    char         Timestr[ 16 ];///< HH:MM:SS.mmm
    char         OrderID[ 6 ];
    char         Symbol[ 21 ];
    char         Filler[ 16 ];
    struct tm    time;
    time_t       Epoch = epoch_s.Get();
    
    localtime_r( &Epoch, &time );    
    snprintf( Timestr, 16, "%02d:%02d:%02d.%03d", time.tm_hour, time.tm_min, time.tm_sec, ms.Get());    
    order_no.Get( OrderID );    
    symbol.Get( Symbol );    
    filler.Get( Filler );   
    return snprintf( Buf, Len,
              "msg_length=%d|MsgSeqNum=%d|msg_time=%s|MessageType=209|fcm_id=%d|session_id=%d|MsgTypeExt=%d|preserve=%d|msg_time_ns=%d|ExecType=%c|cm_id=%d|fcm_id=%d|order_no=%s|ord_id=%d|user_define=%lld|Symbol=%s|BidPx=%d|OfferPx=%d|BidSize=%d|OfferSize=%d|investor_acno=%d|investor_flag=%c|TimeInForce=%d|PositionEffect=%c|filler=%s|CheckSum=%d|"
              ,msg_length.Get()
              ,MsgSeqNum.Get()
              ,Timestr
              ,fcm_id_h.Get()
              ,session_id.Get()
              ,MsgTypeExt.Get()
              ,preserve.Get()
              ,msg_time_ns.Get()                        
              ,ExecType.Get()
              ,cm_id.Get()
              ,fcm_id.Get()
              ,OrderID
              ,ord_id.Get()
              ,user_define.Get()
              ,Symbol
              ,BidPx.Get()
              ,OfferPx.Get()
              ,BidSize.Get()
              ,OfferSize.Get()
              ,investor_acno.Get()
              ,investor_flag.Get()
              ,TimeInForce.Get()
              ,PositionEffect.Get()
              ,Filler ///< [12]
              ,CheckSum.Get() );
}            
//------------------------------------------------------------------------------------------------------
void TMPRX09Message::Recycle( void )
{
    if( TMP::TMPMessage::RX09PoolPtr != NULL )
        TMP::TMPMessage::RX09PoolPtr->Recycle( this );
}  
//----------------------------------------------------------------------------------------------------------
} ///< End of TMP name space.
//----------------------------------------------------------------------------------------------------------

