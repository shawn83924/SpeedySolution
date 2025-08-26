#ifndef __FCM_ORDER_Objects_Manager_H
#define __FCM_ORDER_Objects_Manager_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "../TAIFEXLib/ConnectionObject.h"
//--------------------------------------------------------------------------------------------------------------------
#include "SpeedyConfirmConnectionObject.h"
#include "SpeedyOrderConnectionObject.h"
#include "SpeedyMarketDataConnectionObject.h"
#include "SpeedyFTPSendObject.h"
#include "SpeedyFTPRecvObject.h"
#include "OrderInfoStore.h"
#include "QueryPVC.h"
//----------------------------------------------------------------------------------------------------------------------
#define SPEEDY_ERROR_CONNECTION     88
#define SPEEDY_ERROR_TIMEOUT        87
#define SPEEDY_ERROR_ORDERID_MISS   86
#define SPEEDY_ERROR_QUEUE_FILL     85
#define SPEEDY_ERROR_STOCK_RUSH     84
#define SPEEDY_ERROR_NO_SPEEDY      83
#define SPEEDY_ERROR_NO_PVC         82
#define SPEEDY_ERROR_MAPPER_REJECT  81
//-----------------------------------------------------------------------------------------------------------------------
const Int32 MAX_DATA_SIZE = 384;
//------------------------------------------------------------------------------
#include <TFixedFormat.h>
#include <TParseData.h>
#include <iniFile.h>
//------------------------------------------------------------------------------
class TP01DecimalLocator
{
private:
    UFC::TFixedFormat FDataFormat;
    UFC::AnsiString   FP01FileName;

private:
    void CreateFileIfNotExist( const UFC::AnsiString& FileName );
    void SetValueFromRecord( UFC::TRecord* pRecord, UFC::UiniFile* pIniFile, const UFC::AnsiString& SectionName );

public:
    TP01DecimalLocator( const UFC::AnsiString& DataFormat, const UFC::AnsiString& P01File );

public:
    void Save( const UFC::AnsiString& iniFileName, const UFC::AnsiString& SectionName );
};
//---------------------------------------------------------------------------
class DecimalLocatorThread : public UFC::PThread
{
private:
    void Execute( void );
    BOOL FIsOption;
    BOOL FIsTMP;
public:
    DecimalLocatorThread( BOOL IsOpt, BOOL IsTMP = FALSE )
    :UFC::PThread( NULL, TRUE ),
     FIsOption( IsOpt ),
     FIsTMP( IsTMP )
    { Start(); }
};
//--------------------------------------------------------------------------------------------------------------------
class TransferOrderThread : public UFC::PThread
{
private:
    UFC::PQueue     FQueue;
    BOOL            FIsOptions;
private:    
    void SendToBackup( UInt8* Data );
public:    
    TransferOrderThread( BOOL IsOpt );
    void Execute( void );
};
//--------------------------------------------------------------------------------------------------------------------
class SpeedyObjectsManager : public UFC::ThreadListener, public ConnectionListener, public MessageListener
{
private:
        UFC::PHashedList<UFC::AnsiString, ConnectionObjectBase*>             FConnectionObjects;
        int                   FFCMOrderPVCCount;
        int                   FFCMConfirmPVCCount;
        int                   FCMConfirmPVCCount;
        int                   FCMOrderConfirmPVCCount;
        UFC::SInt             FTotalOrder;
        long	              FTotalTime;
        int                   FMaxTime;
        UFC::PCriticalSection FCriticalSection;
        UFC::PQueue           FQueue;
        TSpeedyFTPSendObject* FtpSendObj;
        TSpeedyFTPRecvObject* FtpRecvObj;
        UFC::PCriticalSection FPVCCountCS;
        UFC::PStringHashedSet FPVCSet;
        UFC::SInt             FPVCCount;
        BOOL                  FIsOptions;
        UFC::AnsiString       FFTPSubject;
        UFC::AnsiString       FSTXFTPSubject;
        UFC::AnsiString       FOrderSubject;
        UFC::AnsiString       FConfirmSubject;
        UFC::AnsiString       FFillSubject;
        UFC::AnsiString       FSyncSubject;        
        UInt16                FTSSReqID;
private:        
        LogManager*           FOrderLog;
        LogManager*           FFillLog;
        LogManager*           FFileTransferLog;
        LogManager*           FMarketDataLog;
private:
        ConnectionObjectBase* GetConnection( int Link,int PVC );
        void RemoveConnection( ConnectionObjectBase* Remove );
        void ReportPerformance( void );
        void CreateFtpSendObject( const UFC::AnsiString & FolderName, TFXSessions& Session );
        void CreateFtpRecvObject( const UFC::AnsiString & FolderName, TFXSessions& Session );
        void SendRWDataToBus( char* Data);
        void FtpStatusReport( TFtpStatusObject* pObj );
        BOOL PVCReady( ConnectionParameter* CObj );
        BOOL PVCBroken( ConnectionParameter* CObj );
        void CheckPVCBroken( ConnectionParameter * cp );
        void ReportPVCState( int Link, int PVC, int state  );
        void SpeedyReject( const UFC::AnsiString& Order,Int32 NID, char* Key,char* Host, Int32 ErrCode, BOOL AsConfirm = FALSE );
        void ModifyOID( char* Order, const UFC::AnsiString& OID );
        void RemoveOldLogFile();
public:
        SpeedyObjectsManager( BOOL IsOpt );
        virtual ~SpeedyObjectsManager();
        OrderInfoStore  OrderInfo;
        void CreateConnectionObjects( int MaxPVC = 0 );
        void IncreaceOrderCount(  ) { FTotalOrder++; }
        void ReplyLineState( int Link, int PVC );
        void ReplyLineState( void );
        void ListenConnection( int Link, int PVC );
        void EnableConnection( int Link, int PVC, BOOL Enabled );
        void Run();
        void Stop();
        void Flush();
        void Download( const UFC::AnsiString & FileCode, int StartSeq, int EndSeq );
        void TMPFTRquest( Int32 TradeKind, Int32 ReqID, const UFC::AnsiString& Data );
        void TMPTSSRquest( Int32 Group, Int32 SubType, Int32 ReqID = 0 );
        void Download( const UFC::AnsiString & FileCode, const UFC::AnsiString & Data = "" );
        // Implement ThreadListener interface.
        virtual void OnTerminate( UFC::PThread* TerminateThread  ){}
        // Implement ConnectionListener interface.
        virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data = NULL );
        // Interface MessageListener
        virtual void OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* );        
private: // For IOC
        class IOCOrder
        {
            public:
                UFC::AnsiString OrderID;
                UFC::AnsiString PBNO;
                UFC::AnsiString IB;
                int             LeftQty;
                int             RefCount;
                char*           C030;
                SpeedyConfirmConnectionObject* ConnectionObject;
            public:
                IOCOrder( const UFC::AnsiString& OID,const UFC::AnsiString& ib, int Qty);
                ~IOCOrder( void );
        };
        UFC::PCriticalSection FIOC_CS;
        UFC::PHashedList<UFC::AnsiString,IOCOrder*> FIOCOrders;
        UFC::PtrList<IOCOrder>						FIOCList;							
private: // For TMP Quote
        UFC::PCriticalSection             FQuoteSetCS;
        UFC::PHashedSet<UFC::AnsiString>  FQuoteSet;
public:
        void AddQuote( TMP::TMPMessage& Order );
        BOOL IsQuote( TMP::TMPMessage& Exec );
private:
        void MultilegOrderFill( SpeedyConfirmConnectionObject* pConnectionObject,C030ComboBody* C030Body, IOCOrder* pOrder,  const UFC::AnsiString& PBNO );
        void SingleOrderFill( SpeedyConfirmConnectionObject* pConnectionObject,C030SingleBody* C030Body, IOCOrder* pOrder,  const UFC::AnsiString& PBNO );
        void CheckIOCOrderQty( void );
        void SendCancelIOCOrderMessage( IOCOrder* pOrder );
        void DeleteIOCOrderInfo( IOCOrder* pOrder );
public:
        void NewIOC( const UFC::AnsiString& OID, const UFC::AnsiString& IB, int Qty );
        void FillIOC( SpeedyConfirmConnectionObject* pConnectionObject, C03XBody *Msg, const UFC::AnsiString& OID,  const UFC::AnsiString& IB, const UFC::AnsiString& PBNO  );
        const UFC::AnsiString& GetFTPSubject( void )    { return FFTPSubject; }
        const UFC::AnsiString& GetOrderSubject( void )  { return FOrderSubject; }
        const UFC::AnsiString& GetConfirmSubject( void ){ return FConfirmSubject; }
        const UFC::AnsiString& GetFillSubject( void )   { return FFillSubject; }
        const UFC::AnsiString& GetSyncPSubject( void )  { return FSyncSubject; }
        BOOL                   IsOptions( void )         { return FIsOptions; }
        LogManager*            GetOrderLog( void )       { return FOrderLog; }
        LogManager*            GetFillLog( void )        { return FFillLog; }
        LogManager*            GetFileTransferLog( void ){ return FFileTransferLog; }
        LogManager*            GetMarketDataLog( void )  { return FMarketDataLog; }        
};
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
