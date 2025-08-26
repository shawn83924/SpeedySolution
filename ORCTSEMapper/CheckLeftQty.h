//---------------------------------------------------------------------------
// CheckLeftQty
//---------------------------------------------------------------------------
#if !defined(CheckLeftQty_H)
  #define CheckLeftQty_H
//---------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../UFC/Stream.h"
#include "../UFC/iniFile.h"
//---------------------------------------------------------------------------
class LeftQtyStore;
//---------------------------------------------------------------------------
class ICheckLeftQtyParam
{
public:
    virtual void ToString(UFC::AnsiString& outResult) = 0;
};
//---------------------------------------------------------------------------
class Address
{
public:
    UFC::AnsiString FAddressIP;
    Int32           FPort;
};
//---------------------------------------------------------------------------
class AccountAttribute
{
private:
    BOOL                    FMustPrealloc;
    UFC::AnsiString         FAccount;
    UFC::AnsiString         FExecutionBroker;
    UFC::AnsiString         FSalesID;
public:
    AccountAttribute( BOOL MustPrealloc, UFC::AnsiString Account, UFC::AnsiString ExecutionBroker, UFC::AnsiString SalesID)
                    :FMustPrealloc( MustPrealloc )
                    ,FAccount(Account)
                    ,FExecutionBroker(ExecutionBroker)
                    ,FSalesID(SalesID) {}
public:
    BOOL              MustPreAlloc()          { return FMustPrealloc; }
    const UFC::AnsiString&   GetAccount()            { return FAccount; }
    const UFC::AnsiString&   GetExecutionBroker()    { return FExecutionBroker; }
    const UFC::AnsiString&   GetSalesID()            { return FSalesID; }
};
//---------------------------------------------------------------------------
class LeftQtyRecord
{
friend class LeftQtyStore;
private:
    Int32                 FPosition;
    UFC::AnsiString       FKey; ///< EexeBrockerID(4) + Account(7) + Symbol(6) = 17 bytes
    Int32                 FPreAllocateQty;
    Int32                 FCumQty;
    Int32                 FEstimateQty;
    Int32                 FOrigQty;
private:
    LeftQtyRecord( Int32 Pos, UFC::AnsiString& Key, Int32 AllocQty, Int32 CumQty, Int32 EstimateQty,Int32 OrigQty );
    LeftQtyRecord( UFC::PStream* Stream );
public:
    void SaveToStream( UFC::PStream* Stream );
    BOOL ModifyLeftQty( BOOL IsAlloc, Int32 Qty );
};
//---------------------------------------------------------------------------
class LeftQtyStore
{
public:
    UFC::PHashedList<UFC::AnsiString, LeftQtyRecord*>     FAccountHashedList;
    UFC::PHashedList<UFC::AnsiString, AccountAttribute*>  FAccountAttributeList;
    UFC::List<Address*>                                   FAddressList;
    UFC::PCriticalSection                                 FLockCS;
    Int32                 FLockCount;
    Int32                 FCount;
    Int32                 FPointer;
    UFC::AnsiString       BrkID;
private:
    UFC::FileStream*      FDataFile;
    BOOL AllocFromTSCD( const UFC::AnsiString& Account,const UFC::AnsiString& Symbol,Int32 Qty, BOOL IsAlloc, UFC::AnsiString& Reason );
    void Recover( void );
    void ReadTSCDConfig(void);
    void ReadCheckerFile( BOOL NeedRecover );
    void ReadAccountList(void);
public:
    LeftQtyStore( BOOL Recover );
    void UpdateRecord( LeftQtyRecord* Record);
    void RemoveTimeOutConecction();
    void SwapTSCDConecction();
    BOOL CheckLeftQty( BOOL IsAlloc,UFC::AnsiString& Account,UFC::AnsiString& Symbol,Int32 Qty,UFC::AnsiString& SaleID,UFC::AnsiString &Reason );
    BOOL AccountExists( const UFC::AnsiString& Account, UFC::AnsiString& SalesID );
    void Lock( void );
    void Unlock( void );
    void ReloadAccountList( void );
    void ReloadCheckStatus( void );
};
//---------------------------------------------------------------------------
class _332CheckParam : public ICheckLeftQtyParam
{
public:
    UFC::AnsiString BrkCod;
    UFC::AnsiString Account;
    UFC::AnsiString StockNo;
    UFC::AnsiString InqStkShr;
    UFC::AnsiString InqType;
public:
    _332CheckParam();
    virtual ~_332CheckParam(){}
    virtual void ToString(UFC::AnsiString& outResult);
};
//---------------------------------------------------------------------------
class TCheckLeftQty : public UFC::SocketClientListener
{
private:
    UFC::PClientSocket* FSocket;
    UFC::AnsiString     FRequestString;
    Int8                FResult;
    BOOL                FCheckResult;
    UFC::AnsiString     FReason;
    Int32               FRecvMessage;
public:
    TCheckLeftQty(UFC::AnsiString& Address, Int32 Port);
    ~TCheckLeftQty();

public:
    virtual void OnConnect( UFC::PClientSocket* Socket ) { UFC::BufferedLog::DebugPrintf(" Connect to Soap Adapter.");}
    virtual void OnDisconnect( UFC::PClientSocket* Socket ) { UFC::BufferedLog::DebugPrintf(" Soap adaptor disconnect ");}
    virtual BOOL OnDataArrived( UFC::PClientSocket* Socket );
    virtual void OnIdle( UFC::PClientSocket* Socket ) {}

public:
    BOOL Check(ICheckLeftQtyParam& Param, LeftQtyStore* LStore );
    UFC::AnsiString GetReason() { return FReason; }
};
//---------------------------------------------------------------------------

#endif // CheckLeftQty_H
