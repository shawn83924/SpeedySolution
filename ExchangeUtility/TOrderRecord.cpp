//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include  "Utility.h"
#include  "TMPDefines.h"
#include  "../UFC/NetUtility.h"
    
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// class OrderRecord
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
///  Binary Layout: IsBIN    ( 4 bytes integer 32 ) Is binary format (TMP) or text format (x.25)
///                 NID      ( 4 bytes integer 32 ) Network ID
///                 CNID     ( 4 bytes integer 32 ) Cancel by Network ID
///                 Tick     ( 8 bytes integer 64 ) Order in queue tick 
///                 OrdSrc   ( 1 bytes char       ) Order source (TAIFEX only)
///                 MDSrc    ( 3 bytes char [3]   ) Market data source (TAIFEX only)
///                 Key size ( 4 Bytes integer 32 ) 
///                     Key data
///                 Host size( 4 bytes integer 32 )
///                     Host data
///                 Data size( 4 bytes integer 32 )
///                     Order data
///
/// Total size: 36 + Key size + Host size + Data size
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Int32  HEADER_FIX_LENGTH = 36;  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Default constructor.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
OrderRecord::OrderRecord( void )
:Group(0)
,IsDirty( FALSE )
,IsBin( FALSE )
,NID(0)
,Tick( UFC::GetTickCountUS() )
,OrdSource('G')          
,IsOffHour( FALSE )
,FSize(0)
,OrderObj( NULL )
{
    Key[0]=0;
    Hostname[0]=0;    
    strncpynt( MDSource, "999", 3 );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// For X.25 Text mode
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
OrderRecord::OrderRecord( BOOL IsTAIFEX,
                          Int32 group,
                          Int32 nid,
                          Int32 cnid,
                          const UFC::AnsiString& key,
                          const UFC::AnsiString& order,
                          const UFC::AnsiString& host,
                          Int64 tick,
                          const UFC::AnsiString& Source,
                          BOOL OffHour  )
:Group( group )
,IsDirty( FALSE )
,IsBin( FALSE )
,NID( nid )
,CNID( cnid )
,Tick( tick )
,IsOffHour( OffHour )
,OrderObj( NULL )
{
    strncpynt( Key, key, 511 );
    strncpynt( Hostname, host, 255 );    
    if( Source.Length() == 4 )
    {    
        OrdSource = Source[0];
        strncpynt( MDSource, Source.c_str() + 1, 3 );        
    }
    else
    {
        OrdSource = 'G';
        strncpynt( MDSource, "999", 3 );
    }
    AssignData( IsTAIFEX, order );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OrderRecord::Init( BOOL IsTAIFEX, 
                        Int32 group,
                        Int32 nid, 
                        Int32 cnid, 
                        const UFC::AnsiString& key, 
                        const UFC::AnsiString& order, 
                        const UFC::AnsiString& host, 
                        Int64 tick, 
                        const UFC::AnsiString& Source,
                        BOOL OffHour )
{
    Group = group;
    IsDirty = FALSE;
    IsBin =  FALSE;
    NID = nid;      ///< 
    CNID = cnid;
    Tick = tick;
    IsOffHour = OffHour;
    strncpynt( Key, key, 511 );  ///< 
    strncpynt( Hostname, host, 255 );    
    if( Source.Length() == 4 )
    {    
        OrdSource = Source[0];
        strncpynt( MDSource, Source.c_str() + 1, 3 );        
    }
    else
    {
        OrdSource = 'G';
        strncpynt( MDSource, "999", 3 );
    }
    AssignData( IsTAIFEX, order );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OrderRecord::InitTWSE( Int32 nid, int Func, const UFC::AnsiString& key, const UFC::AnsiString& order )
{
    CNID      = 0;
    NID       = nid;             
    InQTick   = Tick;    
    FuncCode  = Func;  
    strncpynt( Key, key, 511 );  
    strncpynt( Hostname, UFC::Hostname, 255 );        
    AssignData( FALSE, order );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OrderRecord::Init( BOOL IsTAIFEX, CheckData& Obj )
{
    Group     = 0;
    CNID      = 0;
    NID       = Obj.NID;
    InQTick   = Obj.Tick; //UFC::GetTickCountUS();
    strncpynt( Key, Obj.Key.c_str(), 511 );
    strncpynt( Hostname, UFC::Hostname, 255 );    
    if( IsTAIFEX == TRUE )
    {
        IsDirty   = FALSE;
        IsBin     = FALSE;
        Tick      = Obj.Tick;
        IsOffHour = Obj.IsOffHour;
        OrderObj  = Obj.GetOrderObject();
        if(  Obj.Source.Length() == 4 )
        {    
            OrdSource = Obj.Source[0];
            strncpynt( MDSource, Obj.Source.c_str() + 1, 3 );        
        }
        else
        {
            OrdSource = 'G';
            strncpynt( MDSource, "999", 3 );
        }
    }    
    AssignData( IsTAIFEX, Obj.Order );        
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// For TCP/IP binary mode.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
OrderRecord::OrderRecord( BOOL   IsTAIFEX,
                          Int32  group,
                          Int32  nid,
                          Int32  cnid,
                          const  UFC::AnsiString& key,
                          UInt8* data,
                          Int32  dataLength,
                          const  UFC::AnsiString& host,
                          Int64  tick,
                          BOOL   OffHour)
:Group( group )
,IsDirty( FALSE )
,IsBin( TRUE )
,NID( nid )
,CNID( cnid )
,Tick( tick )
,OrdSource('G')          
,IsOffHour( OffHour )
,OrderObj( NULL )
{
    strncpynt( Key, key, 511 );
    strncpynt( Hostname, host, 255 );
    strncpynt( MDSource, "999", 3 );    
    AssignData( IsTAIFEX, data, dataLength );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Construct from Queue.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
OrderRecord::OrderRecord( BOOL IsTAIFEX, Int32 group, BOOL OffHour, UInt8* Data )
:Group( group )
,IsOffHour( OffHour )
,OrderObj( NULL )
{
    FromBinary( IsTAIFEX, Data );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
// Construct from MTree.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
OrderRecord::OrderRecord( BOOL IsTAIFEX, MTree* Tree )
:OrderObj( NULL )
{
    if( FromMTree( IsTAIFEX, Tree ) == FALSE )
        throw( UFC::Exception(" Create Order from MTree failed"));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OrderRecord::AssignData( BOOL IsTAIFEX, UInt8* Data, Int32 Size )
{
    FSize = Size;
    memcpy( FData, Data, FSize);
    FType = ::GetOrderID( IsTAIFEX, TRUE, FData, FSize, FOID );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OrderRecord::AssignData( BOOL IsTAIFEX, const UFC::AnsiString& OrderStr )
{
    FSize = OrderStr.Length() + 1;
    memcpy( FData, OrderStr.c_str(), FSize );
    FType = ::GetOrderID( IsTAIFEX, FALSE, FData, FSize, FOID );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL OrderRecord::FromMTree( BOOL IsTAIFEX, MTree* Data, Int32 MaxDataSize )
{
    UFC::AnsiString Order,Source,AnsiKey,AnsiHostname;
    Int32           Dirty,TotalSize,DataSize;
    char*           BinData;

    ///< Required fields.
    if( Data->get( "NID", NID ) == FALSE )
        return FALSE;
    if( Data->get( "BIN", IsBin ) == FALSE )  ///< Get Is binary format.
        IsBin = FALSE;
    if( IsBin == TRUE ) ///< It's binary format.
    {
        if( Data->get( "ORDER", BinData, DataSize ) == TRUE )
            AssignData( IsTAIFEX, (UInt8*) BinData, DataSize );        
        else
            return FALSE;
    }
    else ///< It's string format.
    {
        if( Data->get( "ORDER", Order ) == TRUE )
            AssignData( IsTAIFEX, Order );
        else
            return FALSE;
    }
    if( Data->get( "SOURCE", Source ) == TRUE && Source.Length() == 4 )
    {    
        OrdSource = Source[0];
        strncpynt( MDSource, Source.c_str() + 1, 3 );        
    }
    else
    {
        OrdSource = 'G';
        strncpynt( MDSource, "999", 3 );        
    }
    if( Data->get( "CNID", CNID ) == FALSE )
        CNID = 0;
    ///< Optional fields.
    if( Data->get( "KEY",   AnsiKey ) == FALSE )      ///< Get the Key.
        Key[0] = 0;
    else
    {
        if( AnsiKey.Length() < 511 )
            strncpynt( Key, AnsiKey.c_str(), AnsiKey.Length() );    
        else
            strncpynt( Key, AnsiKey.c_str(), 511 );    
    }
    if( Data->get( "HOST",  AnsiHostname ) == FALSE ) ///< get the hostname.
        Hostname[0]=0;
    else
    {
        if( AnsiHostname.Length() < 255 )
            strncpynt( Hostname, AnsiHostname.c_str(), AnsiHostname.Length() );    
        else
            strncpynt( Hostname, AnsiHostname.c_str(), 255 );    
    }
    if( Data->get( "GROUP", Group ) == FALSE )    ///< Get Session group.
        Group = 0;
    TotalSize = HEADER_FIX_LENGTH + AnsiKey.Length() + AnsiHostname.Length() + FSize;
    ///< Others fields.
    if( Data->get( "DIRTY", Dirty ) == FALSE ) ///< No dirty byte exists
        IsDirty = FALSE;
    else
        IsDirty = TRUE;    
    Tick = UFC::GetTickCountUS(); ///< Always use local Tick
    if( Data->get( "OFFHOUR", IsOffHour ) == FALSE ) ///< No dirty byte exists        
        IsOffHour = FALSE;
    ///< Check fields.
    if( (UInt32)NID == 0 ) ///< Check NID
    {
        UFC::BufferedLog::Printf( " Invalid NID[%d] Order[%s]", NID, Order.c_str() );
        return FALSE;
    }
    if( TotalSize >= MaxDataSize ) ///< Data size larger than MAX_DATA_SIZE ?
    {
        UFC::BufferedLog::Printf( " Invalid Order NID[%d] Order[%s] size larger then %d", NID, Order.c_str(),MaxDataSize );
        return FALSE;
    }
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OrderRecord::FromBinary( BOOL IsTAIFEX, UInt8* Data )
{
    Int32 KeySize,HostSize;
    MemoryStreamReader MemReader( (char*)Data );

    MemReader.Read( (char*)&IsBin,   sizeof(Int32) );  /// Fetch the IsBin
    MemReader.Read( (char*)&NID,     sizeof(Int32) );  /// Fetch the NID
    MemReader.Read( (char*)&CNID,    sizeof(Int32) );  /// Fetch the CNID
    MemReader.Read( (char*)&Tick,    sizeof(Int64) );  /// Fetch the In queue Tick.
    MemReader.Read( &OrdSource,      1             );  /// Fetch the Order Source
    MemReader.Read( MDSource,        3             );  /// Fetch the Market data Source    
    MemReader.Read( (char*)&KeySize, sizeof(Int32) );  /// Fetch the Key String size.
    MDSource[ 3 ] = 0;
    if( KeySize > 0 )    
        MemReader.ReadString( Key, KeySize );
    else
        Key[0]=0;    
    MemReader.Read( (char*)&HostSize, sizeof(Int32) );/// Fetch the Host String size.
    if( HostSize > 0 )           
        MemReader.ReadString( Hostname, HostSize );
    else
        Hostname[0]=0;
    MemReader.Read( (char*)&FSize, sizeof(Int32) ); /// Fetch the Message String size.
    if( FSize > 0 )    
        MemReader.ReadString( (char*)FData, FSize );
    FType = ::GetOrderID( IsTAIFEX, IsBin, FData, FSize, FOID );    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  OrderRecord::ToMTree( MTree* Data )
{
    UFC::AnsiString Source;
    
    ///< Required fields.
    Data->append( "BIN", IsBin );      ///< Get Is binary format.
    if( IsBin == TRUE )
        Data->append( "ORDER", FData, FSize );
    else
        Data->append( "ORDER", (const char*)FData );
    Data->append( "NID", NID );
    Data->append( "GROUP", Group );    ///< Get Session group.
    ///< Optional fields.
    if( strlen( Hostname) > 0 )
        Data->append( "KEY",  Key );      ///< Get the Key.
    if( strlen( Hostname ) > 0 )
        Data->append( "HOST",  Hostname ); ///< get the hostname.
    ///< Others fields.
    if( IsDirty == TRUE )
        Data->append( "DIRTY", (Int32)IsDirty );
    if( Tick != 0)
        Data->append( "TICK", Tick );
    if( CNID != 0 )
        Data->append( "CNID", CNID );
    Source.Printf("%c%s", OrdSource, MDSource );
    Data->append( "SOURCE", Source );         
    Data->append( "OFFHOUR", IsOffHour );         
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL  OrderRecord::ToBinary( UInt8* Data, Int32 MaxLength )
{
    MemoryStreamWriter MemWriter( (char*)Data );
    Int32 KeySize  = strlen(Key);
    Int32 HostSize = strlen(Hostname);
    Int32 RecSize = HEADER_FIX_LENGTH + KeySize + HostSize + FSize;
            
    if( RecSize > MaxLength )
    {
        UFC::BufferedLog::Printf( " ########################################## ", MaxLength );
        UFC::BufferedLog::Printf( " Order Record size[%d > %d]", RecSize, MaxLength );
        UFC::BufferedLog::Printf( " Order[%s] Size[%d] ", FData, FSize );
        UFC::BufferedLog::Printf( " Key[%s] Size[%d] ", Key, KeySize );
        UFC::BufferedLog::Printf( " Hostname[%s] Size[%d] ", Hostname, HostSize );
        UFC::BufferedLog::Printf( " ########################################## ", MaxLength );    
        int diff = RecSize - MaxLength;
        if( KeySize > diff )
        {
            KeySize -= diff;
            Key[ KeySize ] = 0; 
        }
        else
            return FALSE;
    }
    MemWriter.Write( (char*)&IsBin,   sizeof(Int32) );  /// Write the IsBin
    MemWriter.Write( (char*)&NID,     sizeof(Int32) );  /// Write the NID
    MemWriter.Write( (char*)&CNID,    sizeof(Int32) );  /// Write the CNID
    MemWriter.Write( (char*)&Tick,    sizeof(Int64) );  /// Write the In-queue Tick.
    MemWriter.Write( &OrdSource,      1             );  /// Write the Order Source
    MemWriter.Write( MDSource,        3             );  /// Write the Market data Source    
    MemWriter.Write( (char*)&KeySize, sizeof(Int32) );  /// Write the Key size.
    if( KeySize > 0 )
        MemWriter.Write( Key, KeySize );
    MemWriter.Write( (char*)&HostSize, sizeof(Int32) );/// Write the Host name size.
    if( HostSize > 0 )
        MemWriter.Write( Hostname, HostSize );
    MemWriter.Write( (char*)&FSize, sizeof(Int32) );/// Write the Order data size.
    MemWriter.Write( (char*)FData, FSize );    
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void OrderRecord::DumpData( const char* Tag )
{
    UFC::BufferedLog::Printf( " ----------------------------------------- %s ----------------------------------------------", Tag );
    UFC::BufferedLog::Printf( " IsBin:%d NID:%010d CNID:%010d", IsBin,NID,CNID );    
    UFC::BufferedLog::Printf( " Group:%d Tick:%lld Host:%s", Group,Tick, Hostname );    
    UFC::BufferedLog::Printf( " Key:[%s]", Key );    
    UFC::BufferedLog::Printf( " Order:[%s]", FData );    
    UFC::BufferedLog::Printf( " -------------------------------------------------------------------------------------------" );        
}    
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// End OrderRecord
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


