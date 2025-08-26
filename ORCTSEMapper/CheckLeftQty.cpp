//---------------------------------------------------------------------------
#include "OrcTSEMapper.h"
//---------------------------------------------------------------------------
const UFC::AnsiString AccountFile( "../cfg/ORCTSEAccount.txt");
const UFC::AnsiString LeftQtyFile( "../cfg/ORCTSEPrelock.csv" );
//---------------------------------------------------------------------------
// Class _332CheckParam
//---------------------------------------------------------------------------
_332CheckParam::_332CheckParam()
:BrkCod("")
,Account("")
,StockNo("")
,InqStkShr("")
,InqType("")
{
}
//---------------------------------------------------------------------------
void _332CheckParam::ToString(UFC::AnsiString& outResult)
{
    outResult.Printf( "332|%s|%s|%s|%s|%s|#", BrkCod.c_str(), Account.c_str(), StockNo.c_str(), InqStkShr.c_str(), InqType.c_str() );
    UFC::BufferedLog::DebugPrintf(" outResult = [%s]", outResult.c_str());
}
//---------------------------------------------------------------------------
// Class TCheckLeftQty
//---------------------------------------------------------------------------
TCheckLeftQty::TCheckLeftQty(UFC::AnsiString& Address, Int32 Port)
:FRequestString("")
,FResult( 0x00 )
,FCheckResult(FALSE)
,FRecvMessage(0)
{
    FSocket = new UFC::PClientSocket( Address, Port );
    FSocket->SetListener( this );
}
//---------------------------------------------------------------------------
TCheckLeftQty::~TCheckLeftQty()
{
    try
    {
        delete FSocket;
    }
    catch( UFC::SocketException & e )
    {
        UFC::BufferedLog::DebugPrintf(" %s", e.what() );
    }
}
//---------------------------------------------------------------------------
BOOL TCheckLeftQty::Check( ICheckLeftQtyParam& Param,  LeftQtyStore* LStore )
{
    Param.ToString( FRequestString );

    if( FRequestString.Length() > 0 )
    {
        try
        {
            FSocket->Connect();
            FSocket->SetBlockMode( false );
            FResult = 0x00;
            Int32 WaitTime = 50; ///< Wait for 5 sec

            UFC::BufferedLog::DebugPrintf(" Send request to TSCD...");
            while( WaitTime > 0 )
            {
                if( FResult != 0x00 )
                    break;
                WaitTime--;
                UFC::SleepMS( 100 );
            }
            if( WaitTime == 0 )
                UFC::BufferedLog::DebugPrintf(" TSCD Time out!");
            FSocket->Disconnect();
        }
        catch(...)
        {
            UFC::BufferedLog::Printf(" Connect failed.");
            LStore->RemoveTimeOutConecction();
            LStore->SwapTSCDConecction();
            return FALSE;
        }
    }
    LStore->SwapTSCDConecction();
    UFC::BufferedLog::DebugPrintf(" TSCD CheckResult = %d", FCheckResult );
    return FCheckResult;
}

//---------------------------------------------------------------------------

BOOL TCheckLeftQty::OnDataArrived(UFC::PClientSocket* Socket)
{
    if( Socket == NULL )
        return FALSE;
    Int8 Buffer[256];
    memset( Buffer, 0x00, 256 );
    try
    {
        Int32 RecvSize = FSocket->RecvBuffer( (UInt8*)Buffer, 256 );
        if( RecvSize > 0 )
        {
            UFC::BufferedLog::DebugPrintf( " Recv from TSCD:[%s]", Buffer );
            if( strcmp( Buffer, "_SOAP_ADAPTER_" ) == 0 )
            {
                FSocket->SendBuffer( FRequestString.c_str(), FRequestString.Length() );
                UFC::BufferedLog::Printf ( " Send:[%s]", FRequestString.c_str() );
            }
            else
            {
                FResult = Buffer[0];
                UFC::AnsiString CheckCode( Buffer + 2, 4 );

                if( FResult == '1' )
                {
                    FCheckResult = FALSE;
                }
                else
                {
                    if( CheckCode == "W103" || CheckCode == "W117" || CheckCode == "W119")
                        FCheckResult = TRUE;
                    else
                        FCheckResult = FALSE;
                }
            }
        }
        return TRUE;
    }
    catch( UFC::SocketException & e )
    {
        UFC::BufferedLog::DebugPrintf(" %s", e.what() );
    }
    return FALSE;
}

//---------------------------------------------------------------------------
// Class  LeftQtyRecord
//---------------------------------------------------------------------------
LeftQtyRecord::LeftQtyRecord( Int32 Pos, UFC::AnsiString& Key, Int32 AllocQty, Int32 CumQty, Int32 EstimateQty, Int32 OrigQty )
:FPosition( Pos ),
 FKey( Key ),
 FPreAllocateQty( AllocQty ),
 FCumQty( CumQty ),
 FEstimateQty( EstimateQty ),
 FOrigQty( OrigQty )
{
}
//---------------------------------------------------------------------------
LeftQtyRecord::LeftQtyRecord( UFC::PStream* Stream )
:FPosition( Stream->GetPosition() )
{
    char Buffer[ 66 ];

    Stream->Read( Buffer, 66 );
    UFC::AnsiString Key(    Buffer,  17 );
    UFC::AnsiString Qty(    Buffer + 17,  12 );
    UFC::AnsiString CumQty( Buffer + 29,  12 );
    UFC::AnsiString NewQty( Buffer + 41,  12 );
    UFC::AnsiString OrigQty( Buffer + 53,  12 );

    Key.TrimRight();
    FKey            = Key;
    FPreAllocateQty = Qty.ToInt();
    FCumQty         = CumQty.ToInt();
    FEstimateQty    = NewQty.ToInt();
    FOrigQty        = OrigQty.ToInt();
    UFC::BufferedLog::DebugPrintf(" Key[%s] Qty[%d] CumQty[%d] NewQty[%d] OrigQty[%d]", FKey.c_str(), FPreAllocateQty, FCumQty,FEstimateQty, FOrigQty );
}
//------------------------------------------------------------------------------
void LeftQtyRecord::SaveToStream( UFC::PStream* Stream )
{
    char Buffer[ 66 ];

    UFC::AnsiString PadKey( FKey );
    PadKey.PadThis( 17, ' ' );
    sprintf( Buffer, "%s%012d%012d%012d%012d\n", PadKey.c_str(), FPreAllocateQty, FCumQty, FEstimateQty, FOrigQty );
    Stream->Seek( FPosition, UFC::soFromBeginning );
    Stream->Write( Buffer, 66 );
}
//------------------------------------------------------------------------------
BOOL LeftQtyRecord::ModifyLeftQty( BOOL IsAlloc, Int32 Qty )
{
    if( IsAlloc == TRUE ) ///< Reduce qty
    {
        Int32 LeftQty = FPreAllocateQty - FCumQty;
        if( LeftQty < Qty ) ///< Left qty is not enough
            return FALSE; ///< Left qty is not enough.
        FCumQty += Qty;
        return TRUE;
    }
    else ///< give back the stocks.
    {
        if( FCumQty < Qty )
            return FALSE;
        FCumQty -= Qty;
        return TRUE;
    }
}
//---------------------------------------------------------------------------
// Class  LeftQtyStore
//------------------------------------------------------------------------------
LeftQtyStore::LeftQtyStore( BOOL NeedRecover )
:FLockCount( 0 )
,FCount( 0 )
,FPointer( 0 )

{
    if( NeedRecover == TRUE )
    {
        FDataFile = new UFC::FileStream( LeftQtyFile, O_CREAT|O_RDWR );
        Recover();
    }
    else
        FDataFile = new UFC::FileStream( LeftQtyFile, O_CREAT|O_TRUNC|O_RDWR );
    ReadAccountList();
    ReadTSCDConfig();
}
//---------------------------------------------------------------------------
void LeftQtyStore::ReadTSCDConfig( void )
{
    ///< Read TSCD server info.
    UFC::AnsiString   Value;
    UFC::UiniFile     Config( "../cfg/ORCTSEMapper.cfg" );

    if( Config.GetValue( "TSCD", "BrokerID", BrkID ) == TRUE )
        UFC::BufferedLog::DebugPrintf( " TSCD BrokerID = [%s]", BrkID.c_str() );
    else
        UFC::BufferedLog::Printf( " Can't read BrokerID from GLMapper.cfg.");
    UFC::AnsiString   AddressStr("Address1");
    UFC::AnsiString   Port("Port1");

    while( Config.ValueExists( "TSCD", AddressStr ) == TRUE )
    {
        Address* AddressPtr = new Address();
        Config.GetValue( "TSCD", AddressStr, (AddressPtr->FAddressIP) );
        Config.GetValue( "TSCD", Port, Value);
        AddressPtr->FPort = Value.ToInt();
        FAddressList.Add(AddressPtr);
        FCount++;
        UFC::BufferedLog::DebugPrintf(" TSCD Server Address%d = %s:%d", FCount, AddressPtr->FAddressIP.c_str(), AddressPtr->FPort);
        AddressStr.Printf( "Address%d", FCount + 1 );
        Port.Printf("Port%d", FCount + 1 );
    }
}
//------------------------------------------------------------------------------
void LeftQtyStore::ReloadAccountList(void)
{
    Lock( );
    for( int i = 0; i < FAccountAttributeList.ItemCount(); i++ )
        delete FAccountAttributeList.GetItem( i );
    FAccountAttributeList.Clear();
    ReadAccountList();
    Unlock( );
}
//------------------------------------------------------------------------------
void LeftQtyStore::ReadAccountList(void)
{
    UFC::AnsiString    Type,Account,ExecutionBroker,SalesID;
    char               Buffer[ 512 ] = "";
    char               Seps[] = ",";
    FILE*              FileHandle;
    AccountAttribute*  Attribute;

    if( (FileHandle = fopen( AccountFile.c_str() , "r" )) == NULL )
    {
        UFC::BufferedLog::Printf( " Can not open file %s", AccountFile.c_str());
        exit( 0 );
    }
    while( fgets( Buffer, 512, FileHandle ) != NULL )
    {
        Type = strtok( Buffer, Seps );
        Account = strtok( NULL, Seps );
        ExecutionBroker  = strtok( NULL, Seps);
        SalesID = strtok( NULL, Seps);
        UFC::BufferedLog::DebugPrintf(" Type[%s] Account[%s] Broker[%s] SalesID[%s]", Type.c_str(), Account.c_str(), ExecutionBroker.c_str(), SalesID.c_str());
        Attribute = new AccountAttribute( Type.ToInt(), Account, ExecutionBroker, SalesID );
        FAccountAttributeList.Add( Account, Attribute );
    }
    fclose( FileHandle );
}
//------------------------------------------------------------------------------
void LeftQtyStore::ReloadCheckStatus( void )
{
    Lock( );
    ///< Delete all LeftQtyRecord objects in memory.
    for( int i = 0; i < FAccountHashedList.ItemCount(); i++ )
         delete FAccountHashedList.GetItem( i );
    FAccountHashedList.Clear();
    ///< Close and reopen the LeftQty File.
    delete FDataFile;
    FDataFile = new UFC::FileStream( LeftQtyFile, O_CREAT|O_RDWR );
    ///< Rebuild LeftQtyRecord objects in memory.
    Recover();
    Unlock( );
}
//------------------------------------------------------------------------------
void LeftQtyStore::Recover(void)
{
    Int32           FileSize = FDataFile->GetSize();
    LeftQtyRecord*  NewRecord;

    FDataFile->Seek( 0, UFC::soFromBeginning );
    UFC::BufferedLog::DebugPrintf(" File size = %d", FileSize );
    while( FDataFile->GetPosition() < FileSize )
    {
        NewRecord = new LeftQtyRecord( FDataFile );
        FAccountHashedList.Add( NewRecord->FKey, NewRecord );
    }
    UFC::BufferedLog::DebugPrintf(" Read %d prelock stock.", FAccountHashedList.ItemCount());
}
//------------------------------------------------------------------------------
void LeftQtyStore::UpdateRecord( LeftQtyRecord* Record )
{
    Record->SaveToStream( FDataFile );
    FDataFile->Flush();
}
//------------------------------------------------------------------------------
void LeftQtyStore::SwapTSCDConecction()
{
    FPointer++;
    if( FPointer >= FCount )
        FPointer = 0;
}
//------------------------------------------------------------------------------
void LeftQtyStore::RemoveTimeOutConecction()
{
    if( FCount > 0 )
    {
        FAddressList.Delete( FPointer );
        FCount--;
    }
    else
    {
        UFC::BufferedLog::Printf(" All LeftQty checking service are not available.");
    }
}
//------------------------------------------------------------------------------
void LeftQtyStore::Lock( void )
{
    UFC::PLockObject Locker( FLockCS );

    FLockCount++;
}
//------------------------------------------------------------------------------
void LeftQtyStore::Unlock( void )
{
    UFC::PLockObject Locker( FLockCS );

    FLockCount--;
}
//------------------------------------------------------------------------------
BOOL LeftQtyStore::AccountExists( const UFC::AnsiString& Account, UFC::AnsiString& SalesID )
{
    AccountAttribute*  AccountAttrib = FAccountAttributeList.GetObjectByKey( Account );

    if( AccountAttrib != NULL ) ///< Account exists.
    {
        SalesID = AccountAttrib->GetSalesID();
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL LeftQtyStore::CheckLeftQty( BOOL IsAlloc, UFC::AnsiString& Account, UFC::AnsiString& Symbol, Int32 SellQty,  UFC::AnsiString& SaleID, UFC::AnsiString &Reason )
{
    if( FLockCount > 0 ) ///< The mapper was locked.
    {
        Reason = "Gateway: Error.Administrator lock the gateway.";
        return FALSE;
    }
    AccountAttribute*  AccountAttrib = FAccountAttributeList.GetObjectByKey( Account );
    LeftQtyRecord*     SaleStockRecord;

    if( AccountAttrib != NULL ) ///< Account exists.
    {
        UFC::BufferedLog::DebugPrintf(" Account:%s exists.",Account.c_str());
        UFC::AnsiString Key = AccountAttrib->GetExecutionBroker() + Account + Symbol;

        SaleID = AccountAttrib->GetSalesID();
        if( (SaleStockRecord = FAccountHashedList.GetObjectByKey( Key ) ) != NULL )///< Pre-alloc this Stock?
        {
            UFC::BufferedLog::DebugPrintf(" Prelock Stock:%s",Symbol.c_str());
            if(  SaleStockRecord->ModifyLeftQty( IsAlloc,SellQty ) == TRUE ) ///< Use pre-alloc shares.
            {
                 UFC::BufferedLog::DebugPrintf(" OK.");
                 UpdateRecord( SaleStockRecord );
                 return TRUE;
            }
            if( AccountAttrib->MustPreAlloc() == TRUE ) ///< Must pre-alloc shares ?
            {
                UFC::BufferedLog::DebugPrintf(" Prelock qty not enough, Reject it.");
                Reason = "31017";
                return FALSE;
            }
            else ///< Alloc from TSCD
                return AllocFromTSCD( AccountAttrib->GetExecutionBroker() + Account, Symbol,SellQty,IsAlloc,Reason );
        }
        else ///< No pre-alloc stock.
        {
             UFC::BufferedLog::DebugPrintf( " No Prelock Stock:%s", Symbol.c_str() );
            if( AccountAttrib->MustPreAlloc() == TRUE ) ///< Must pre-alloc shares ?
            {
                UFC::BufferedLog::DebugPrintf(" Account must prelock shares, Reject it.");
                Reason = "31018";
                return FALSE;
            }
            else ///< Alloc from TSCD
                 return AllocFromTSCD( AccountAttrib->GetExecutionBroker() + Account, Symbol,SellQty,IsAlloc,Reason );
        }
    }
    else  ///< Not a valid account, reject it.
    {
        UFC::BufferedLog::DebugPrintf(" Account:%s not exists.",Account.c_str());
        Reason = "31014";
        return FALSE;
    }
}
//------------------------------------------------------------------------------
BOOL LeftQtyStore::AllocFromTSCD( const UFC::AnsiString& Account,const UFC::AnsiString& Symbol,Int32 Qty, BOOL IsAlloc, UFC::AnsiString& Reason )
{
    if( IsStandby == TRUE ) ///< No action... if mapper is in standby mode.
        return TRUE;
    if( FCount > 0 )
    {
        Address*         Addr = FAddressList.GetItem( FPointer );
        TCheckLeftQty    Checker( Addr->FAddressIP, Addr->FPort );
        UFC::BufferedLog::DebugPrintf( " Soap adaptor: %s:%d",Addr->FAddressIP.c_str(), Addr->FPort  );
        _332CheckParam   Param;

        Param.BrkCod    = BrkID;
        Param.Account   = Account;
        Param.StockNo   = Symbol;
        Param.InqStkShr = UFC::AnsiString( Qty );
        if( IsAlloc == TRUE )
            Param.InqType   = "0";
        else
            Param.InqType   = "1";
        if( Checker.Check( Param, this ) == FALSE )
        {
            Reason = "31017";
            return FALSE;
        }
        return TRUE;
    }
    else
    {
        UFC::BufferedLog::Printf(" No TSCD connection to use.");
        Reason = "Gateway: Error. No TSCD connection available.";
        return FALSE;
    }
}
//------------------------------------------------------------------------------

