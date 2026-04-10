/* 
 * File:   RMInterface.h
 * Author: yuan
 *
 * Created on 2016 10:21
 */

#ifndef RMINTERFACE_H
#define	RMINTERFACE_H
//------------------------------------------------------------------------------
#include "TExecReport.h"
#include "../UFC/AnsiString.h"
#include "../Migo/Sigo.h"
  
//------------------------------------------------------------------------------
enum rmCheckResult
{
	rmRejectGeneral = -99,
	rmRejectStock,
	rmRejectBullBear,
	rmRejectTick,
	rmRejectQty,
	rmRejectMark,
	rmRejectAccount,
	rmRejectPosition,
	rmRejectBuy,
	rmRejectSell,
	rmRejectMarginStockFlat,
	rmRejectMarginStockQual,
	rmRejectMarginAccountQual,
	rmRejectSingleLimit,
	rmRejectFinBuy,
	rmRejectFinSell,
	rmRejectShortBuy,
	rmRejectShortSell,
	rmResultPass = 0,
	rmWarningWaterLevel
};
//------------------------------------------------------------------------------
class CheckData
{
public: ///< In 
    int             Market;   ///< Market (mFutures:0 mOption:1 mTSE:2  mOTC:3)
    int             NID;      ///< Unique network ID
    Int64           Tick;     ///< Begin check tick.(us) 
    BOOL            IsOffHour;///< Is off-hour order.( TAIFEX only)
    UFC::AnsiString Order;    ///< Order message string.( TIAFEX:R010  TWSE:T010,P010,O010 )
    UFC::AnsiString Key;      ///< Format: AE,User Data     
    UFC::AnsiString Source;   ///< Order source.( TAIFEX only)
public:  ///< Out    
    UFC::AnsiString Msg;      ///< Reject reason.
    void*           OrderPtr; ///< Pointor to TWSE: MessageT010,MessageP010,MessageO010 object.    
public:    
    CheckData( int M, int nid, const UFC::AnsiString& Odr, const UFC::AnsiString& k, const UFC::AnsiString& src, BOOL OffHour, Int64 tk )    
    :Market( M ),
    NID( nid ),
    Tick( tk ),
    IsOffHour( OffHour ),
    Order( Odr ),
    Key( k ),
    Source( src ),
    OrderPtr( NULL )
    {}
    void  SetOrderObject( void* Obj ) { OrderPtr = Obj; }
    void* GetOrderObject( void )      { return OrderPtr; }
};
//------------------------------------------------------------------------------
class CheckResultListener
{
public:
    virtual ~CheckResultListener() {}
    virtual void OnResult( CheckData& Obj, int Code ) = 0;
    virtual MessageObject* OnGetMBusObject( void ) = 0;
};
//------------------------------------------------------------------------------
class RMObj
{
public:    
    virtual ~RMObj(){}
    virtual void Check( CheckData& Obj ) = 0;
    virtual void Report( TExecReport* Msg, int PossDup ) = 0; //	PossDup: 0-NewExecution, 1-PossibleDuplicate, 2-SpeedyGenerate
    virtual BOOL QueryBalancePosition( const UFC::AnsiString& BrokerID, const UFC::AnsiString& AccountNo, UFC::AnsiString& Result ) = 0;
    virtual void SetResultListener( CheckResultListener* Listener ) = 0;    
};
//------------------------------------------------------------------------------
class RMObjExtra : public RMObj
{
public:
	virtual void FixExecutionReport(const char *fixstr) = 0;

#if defined(__DEBUG__) || defined(__RECORDING_TIME__)
	virtual void InsertFakeDataToStockPool(const char *stock_str) = 0;
	virtual void InsertFakeDataToAccountsPool(const char *acc_str) = 0;
	virtual void InsertFakeDataToPositionPool(const char *pos_str) = 0;
	virtual void InsertFakeDataToTicketPool(const char *ticket_str) = 0;
	virtual void InsertFakeDataToLendingPool(const char *lending_str) = 0;
	virtual void InsertMarginToStockPool(const char *stock_str) = 0;
	virtual void InsertMarginToAccountsPool(const char *acc_str) = 0;
	virtual void InsertT33ToStockPool(const char *t33) = 0;
#endif
};
//------------------------------------------------------------------------------
extern "C"
{
    RMObj* CreateRM( BOOL RecoverMode );
    void   DestroyRM( RMObj* RM );
}
//------------------------------------------------------------------------------
typedef RMObj* (*FuncCreate)  ( BOOL );
typedef void   (*FuncDestroy) ( RMObj*);
//------------------------------------------------------------------------------
#endif	/* RMINTERFACE_H */

