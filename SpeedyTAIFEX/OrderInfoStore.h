//---------------------------------------------------------------------------

#ifndef OIDToClientIDH
#define OIDToClientIDH
#include "../UFC/UFC.h"
#include "../UFC/PInt32.h"
//---------------------------------------------------------------------------
class OIDToClientID;
class SyncExecThread;
//---------------------------------------------------------------------------
// Modify by Simon 2008/02/23
//
// Use IB + OID as a unique ID for an Order.
//---------------------------------------------------------------------------
// Class OrderInfo
// Keep Order information in memory.
// FNewOrderNID  : NID for this new order.
// FOrderID      : OrderID for this new order.
// FIB           : IB for thisorder. ( Use IB + OID as a unique ID for an Order )
// FKeyString    : User data for this order.
//---------------------------------------------------------------------------
class OrderInfo
{
friend class OrderInfoStore;
private:
    Int32            FNewOrderNID;
    UFC::AnsiString  FOrderID;
    UFC::AnsiString  FIB;
    UFC::AnsiString  FKeyString;
    BOOL             FIsSpeedy;
private:
    UFC::PInt32List  FCancleNIDs;
    UFC::PStringList FCancleKeys;
    UFC::PInt32List  FReplaceNIDs;
    UFC::PStringList FReplaceKeys;
private:
    OrderInfo( const UFC::AnsiString& OID,  const UFC::AnsiString& IB,const UFC::AnsiString& Key, Int32 NID, BOOL IsSpeedy );
    OrderInfo( const UFC::AnsiString& DataString );
    OrderInfo( ){}
    void FromString( const UFC::AnsiString& DataString );
    void ToString( UFC::AnsiString& DataString );
public:
    BOOL                   IsSpeedyOrder( void ){ return FIsSpeedy; }
    Int32                  GetNID( void )       { return FNewOrderNID; }
    const UFC::AnsiString& GetOrderID( void )   { return FOrderID; }
    const UFC::AnsiString& GetIB( void )        { return FIB; }
    const UFC::AnsiString& GetKeyString( void ) { return FKeyString; }
};
//---------------------------------------------------------------------------
// Class ExecInfo
//---------------------------------------------------------------------------
class ExecInfo
{
private:	
	UFC::AnsiString FLog;
public:
	ExecInfo( const UFC::AnsiString& Log ):FLog( Log ){}
        virtual ~ExecInfo( void ) {}
	const UFC::AnsiString& GetLog( void )	  { return FLog; }        
	virtual BOOL           IsNewOrder( void ) { return FALSE; }
};
//---------------------------------------------------------------------------
class NewExec : public ExecInfo
{
private:
	Int32           FNID;
	UFC::AnsiString FOID;
        UFC::AnsiString FIB;
	UFC::AnsiString FKey;	
public:	
	NewExec( Int32 NID, const UFC::AnsiString& OID, const UFC::AnsiString& IB, const UFC::AnsiString& Key,const UFC::AnsiString& Log )
	:ExecInfo( Log ),  
     FNID( NID ),
	 FOID( OID ),
     FIB( IB ),
	 FKey( Key ){}	
	Int32                  GetNID( void )	  { return FNID; }
	const UFC::AnsiString& GetOID( void )	  { return FOID; }
    const UFC::AnsiString& GetIB( void )	  { return FIB;  }
	const UFC::AnsiString& GetKey( void )	  { return FKey; }
    virtual BOOL           IsNewOrder( void ) { return TRUE; }
};
//---------------------------------------------------------------------------
class OrderInfoStore
{
friend class SyncExecThread;
private:
    static UFC::PCriticalSection                      FUpdateCS;
    static UFC::PCriticalSection                      FFileCS;
    static UFC::PCriticalSection                      FExecKeyCS;
    static UFC::PCriticalSection                      FExecCS;

    UFC::FileStreamEx*                                  FOrderInfoFile;
    UFC::FileStreamEx*                                  FCancelNIDFile;
    UFC::FileStreamEx*                                  FReplaceNIDFile;
    UFC::FileStreamEx*                                  FExecutionIDFile;
    UFC::FileStreamEx*                                  FExecutionLog;
    UFC::PHashedList< UFC::AnsiString, OrderInfo* >     FOIDMap;
    UFC::PHashedList< Int32, OrderInfo* >               FNIDMap;
    UFC::PHashedList< UFC::AnsiString, SyncExecThread*> FHostToSyncObj;
    UFC::PStringHashedSet                               FExecKeySet;
    UFC::PtrList<ExecInfo>				                FExecutions;
    BOOL                                                FCluster;
private:
    UFC::AnsiString FNewOrderRecoverFile;
    UFC::AnsiString FCancelNIDRecoverFile;
    UFC::AnsiString FReplaceNIDRecoverFile;
    UFC::AnsiString FExecutionIDRecoverFile;
    UFC::AnsiString FExecutionLogFile;    
    void Recover();
    void RecoverCancelNID();
    void RecoverReplaceNID();
    void RecoverExecutionID();
    void RecoverExecutions();
    void DeleteItemInList( UFC::PInt32List& List,  Int32 NID );
    void DeleteItemInList( UFC::PStringList& List, const UFC::AnsiString& CKey );
    void LogModify( BOOL IsAdd, const UFC::AnsiString& OID, const UFC::AnsiString& IB,Int32 NID, const UFC::AnsiString& CKey, UFC::FileStreamEx* File );
public:
    OrderInfo* AddOrder( const UFC::AnsiString& OID, const UFC::AnsiString& IB, const UFC::AnsiString& Key, Int32 NewOrderNID, BOOL IsSpeedy );
    BOOL GetOrderInfo( const UFC::AnsiString& OID, const UFC::AnsiString& IB, UFC::AnsiString& Key, Int32& NewOrderNID );
    BOOL GetOrderInfo( const Int32 NID, Int32& OrigNID, UFC::AnsiString& OID, UFC::AnsiString& IB, UFC::AnsiString& Key  );
    BOOL GetOID( Int32 NewOrderNID, UFC::AnsiString& OID, UFC::AnsiString& IB );
    
    BOOL IsExecExist( const UFC::AnsiString& Key );    

    void AddNID( const UFC::AnsiString& OID, const UFC::AnsiString& IB,  Int32 CancelNID, const UFC::AnsiString& CKey ); ///< For TMP Order Status request only.
    void PushCancelNID( const UFC::AnsiString& OID, const UFC::AnsiString& IB,  Int32 CancelNID, const UFC::AnsiString& CKey, BOOL IsTMP = FALSE );
    void PushReplaceNID( const UFC::AnsiString& OID, const UFC::AnsiString& IB, Int32 ReplaceNID, const UFC::AnsiString& CKey, BOOL IsTMP = FALSE );
    BOOL PopCancelNID(  const UFC::AnsiString& OID,  const UFC::AnsiString& IB, Int32& CancelNID, UFC::AnsiString& Key, UFC::AnsiString& CKey );
    BOOL PopCancelNID(  const Int32& CancelNID, const UFC::AnsiString& OID,  const UFC::AnsiString& IB, UFC::AnsiString& Key, UFC::AnsiString& CKey );
    BOOL PopReplaceNID( const UFC::AnsiString& OID,  const UFC::AnsiString& IB, Int32& ReplaceNID, UFC::AnsiString& Key, UFC::AnsiString& CKey);
    BOOL PopReplaceNID(  const Int32& ReplaceNID, const UFC::AnsiString& OID,  const UFC::AnsiString& IB, UFC::AnsiString& Key, UFC::AnsiString& CKey );
    
    void WriteExecutionLog( Int32 Seq, const UFC::AnsiString& Host, const UFC::AnsiString& Log );
    void WriteExecutionLog( BOOL IsConfirm, Int32 NID,const UFC::AnsiString& Key, const UFC::AnsiString& Data, const UFC::AnsiString& PBNO, const UFC::AnsiString& Host, const UFC::AnsiString& PVC, Int32 TMPRep_seq  );
public:
    OrderInfoStore( Int32 HashTableSize, BOOL  IsOpt, BOOL RecoverMode );
    BOOL ClusterEnable( void ) {return FCluster; };
    void OnSpeedyStartup( const UFC::AnsiString& Host);
    void OnSpeedyStop( const UFC::AnsiString& Host);
    void StopAllSyncThread( void );
    void SendSequenceToAllSpeedy( void );
    void SetExecSequence( const UFC::AnsiString& Host, Int32 Seq );
};
//---------------------------------------------------------------------------
class SyncExecThread : public UFC::PThread
{
private:
	UFC::PInt32*          FOutSeq;
	UFC::AnsiString       FHostname;
	BOOL                  FIsActive;
    BOOL                  FRecovering;
	OrderInfoStore*       FStore;
	UFC::PInt32HashedSet  FSeqSet;
	UFC::PCriticalSection FSeqCS;
    Int32                 FBeginTime;
    UFC::AnsiString       FSyncSubject;
public:
	SyncExecThread( BOOL IsOpt, BOOL RecoverMode, const UFC::AnsiString& Host, OrderInfoStore* Store );
    virtual void Execute( void );
	void  SetActive( BOOL Active )     { FIsActive = Active; }
	void  SetRecoverFrom( Int32 Seq );
	Int32 GetSequence( void )          { return FOutSeq->ToInt32(); }
	BOOL  MessageSeqExists( Int32 Seq );
	void  SetMessageSeq( Int32 Seq );
	void  SendSequence( void );
};
//---------------------------------------------------------------------------
#endif
