/* 
 * File:   MarketSchedule.h
 * Author: yuan
 *
 * Created on 2015年2月11日, 下午 11:14
 */

#ifndef MARKETSCHEDULE_H
#define	MARKETSCHEDULE_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../UFC/iniFile.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//--------------------------------------------------------------------------------------------------------------------
typedef enum
{
    msWait     = 0,
    msTryOrder = 1,
    msOpened   = 2,
    msClosed   = 3,
    msStop     = 4
    
} TMarketStatus;
//--------------------------------------------------------------------------------------------------------------------
typedef enum
{
    mtNormal,
    mtOddTrade,
    mtPostTrade,
    mtLend,
    mtTender,
    mtAuction,
    mtTenderEx,
    mtNegotiatePx,
    mtOddTradeEx,
    
} TMarketType;
//--------------------------------------------------------------------------------------------------------------------
class MarketSchedule
{
public:
    TMarketType           FType;
    UFC::PCriticalSection FMarketStatusCS;
    UFC::PCriticalSection FPVCCountCS;    
    TMarketStatus         FStatus;            
public:        
    Int32                 FTryOrderTime;    
    Int32                 FStopTime;
    Int32                 FMaxQueue;
    Int32                 FOpenMarketTime;
    Int32                 FMaxQueueBeforeOpenMarket;
    Int32                 FCheckConnectionTime;
    BOOL                  FChecked;    
    UFC::PInt32           FLastStatus;
public:
    Int32                 FInitTryPVCCount;        
    Int32                 FTryPVCCount;    
    Int32                 FRetryInterval;
    BOOL                  FIsOpened;
    Int32                 FPVCCount;
    UFC::AnsiString       FName;
    Int32                 FOrdersPerSec;
    Int32                 FSwitchPVCThroughput;
    BOOL                  FNeedTry;
    void LoadSpecialSession( const UFC::AnsiString& Name, UFC::UiniFile& Cfg, BOOL Recover );
    void LoadNeedTrySession( const UFC::AnsiString& Name, UFC::UiniFile& Cfg, BOOL Recover );
public:
    MarketSchedule( BOOL IsTWSE, TMarketType Type, const UFC::AnsiString& Name, UFC::UiniFile& cfg, BOOL IsRecover );    
    void          Reload(  UFC::UiniFile& cfg );
    void          CloseMarket( void );
    TMarketType   GetMarketType( void ) { return FType; } 
    TMarketStatus GetStatus( void ){ return FStatus; }
    TMarketStatus SetStatus( TMarketStatus NewStatus );    
    BOOL          IsAutoSwitchMarket( void );
    BOOL          CanITry( void );
    BOOL          IsOpened( void ) { return FIsOpened; }
    void          OnTimer( Int32 TimeNow );
    char          GetAPCode( void ); ///< Speedy AP Code
    const char*   StateString( TMarketStatus Status );
    const UFC::AnsiString& GetName( void ) { return FName; }    
public:
    Int32         IncreasePVCCount( void );
    Int32         DecreasePVCCount( void ); 
    Int32         GetPVCCount( void ) { return FPVCCount; }
    BOOL          IsTimeToCheckPVC( Int32 Now );

};
//-----------------------------------------------------------------------------------------
}
//--------------------------------------------------------------------------------------------------------------------
#endif	/* MARKETSCHEDULE_H */

