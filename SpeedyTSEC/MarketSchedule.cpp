#include "MarketSchedule.h"
//--------------------------------------------------------------------------------------------------------------------
//
// Class MarketSchedule
//
// Control the market status and contains market information
//
// Try
// OpenMarketTime
// Stop
//
//--------------------------------------------------------------------------------------------------------------------
extern BOOL            SkipTryOrder;
//--------------------------------------------------------------------------------------------------------------------
namespace TWSE
{
//--------------------------------------------------------------------------------------------------------------------
MarketSchedule::MarketSchedule( BOOL IsTWSE, TMarketType Type, const UFC::AnsiString& Name, UFC::UiniFile& Cfg, BOOL Recover )
:FType( Type )
,FStatus( msWait )
,FChecked( FALSE )
,FIsOpened( FALSE )
,FPVCCount( 0 )
,FName( Name ) 
,FOrdersPerSec( 0 )
,FSwitchPVCThroughput( 10 )
{
   UFC::AnsiString  UniqueName;   
   
   try
   {
       UniqueName.Printf( "%s_Name:%s", ((IsTWSE)?"TSEC":"OTC"), Name.c_str());
       FLastStatus.Attach( UniqueName );        
       FNeedTry = !IsAutoSwitchMarket( );
       if( FNeedTry )
           LoadNeedTrySession( Name, Cfg, Recover );
       else
           LoadSpecialSession( Name, Cfg, Recover );	
       if( SkipTryOrder == TRUE )
           FNeedTry = FALSE;
   }
   catch( UFC::Exception& e )
   {
       UFC::BufferedLog::Printf( " Open share memory ID[0x%08X] for [%s] trade session failed.", Hash( UniqueName ), Name.c_str());     
       UFC::BufferedLog::Printf( " Reason:%s",e.what() );
       exit( 0 );
   }   
}
//--------------------------------------------------------------------------------------------------------------------
void MarketSchedule::Reload(  UFC::UiniFile& cfg )
{
    if( FNeedTry )
        LoadNeedTrySession( FName, cfg, FALSE );
    else
        LoadSpecialSession( FName, cfg, FALSE );	    
}    
//--------------------------------------------------------------------------------------------------------------------
void  MarketSchedule::LoadNeedTrySession( const UFC::AnsiString& Name, UFC::UiniFile& Cfg, BOOL Recover )
{
    UFC::AnsiString StopTime,TryTime,TryCount,TryInterval,QueueSize,OpenMarketTime,QueueSizeBeforeOpenMarket,CheckTime;
    
    if( Cfg.GetValue( Name, "Stop", StopTime ) == TRUE && 
        Cfg.GetValue( Name, "Try", TryTime ) == TRUE && 
        Cfg.GetValue( Name, "MaxTryPVC", TryCount ) == TRUE && 
        Cfg.GetValue( Name, "RetryInterval", TryInterval ) == TRUE && 
        Cfg.GetValue( Name, "QueueSize",QueueSize ) == TRUE && 
        Cfg.GetValue( Name, "OpenMarketTime",OpenMarketTime ) == TRUE && 
        Cfg.GetValue( Name, "QueueSizeBeforeOpenMarket", QueueSizeBeforeOpenMarket ) == TRUE  )
    {
        FTryOrderTime   = TryTime.ToInt();    
        FStopTime       = StopTime.ToInt();        
        FTryPVCCount    = TryCount.ToInt();   
        FRetryInterval  = TryInterval.ToInt();   
        FMaxQueue       = QueueSize.ToInt();   
        FOpenMarketTime = OpenMarketTime.ToInt();
        FMaxQueueBeforeOpenMarket = QueueSizeBeforeOpenMarket.ToInt();        
        FInitTryPVCCount = FTryPVCCount;
        UFC::BufferedLog::Printf( " -------- Setting for %s trade session --------",Name.c_str() );             
        UFC::BufferedLog::Printf( " [%s] Try Order at %02d:%02d:%02d",Name.c_str(), FTryOrderTime/10000, (FTryOrderTime%10000)/100, FTryOrderTime%100 );     
        UFC::BufferedLog::Printf( " [%s] Sleeping at %02d:%02d:%02d",Name.c_str(), FStopTime/10000, (FStopTime%10000)/100, FStopTime%100 );     
        UFC::BufferedLog::Printf( " [%s] Use %d PVC to try order.",Name.c_str(), FTryPVCCount );     
        if( Cfg.GetValue( Name, "CheckPVCTime", CheckTime ) == TRUE )
            FCheckConnectionTime = CheckTime.ToInt();
        else
            FCheckConnectionTime = 0;
	if( Recover == FALSE )  
	{
	    FStatus = msWait;              
	    FLastStatus = (Int32)msWait;
	}
	else
	    FStatus = (TMarketStatus)((Int32)FLastStatus);
	UFC::BufferedLog::Printf( " [%s] Status = %s.",Name.c_str(), StateString( FStatus ));	
    }
    else
    {
        UFC::BufferedLog::Printf( " Incorrect schedule setting for [%s] trade session.", Name.c_str());     
        exit( 0 );
    }
}    
//--------------------------------------------------------------------------------------------------------------------
void MarketSchedule::LoadSpecialSession( const UFC::AnsiString& Name, UFC::UiniFile& Cfg, BOOL Recover )
{
    UFC::AnsiString StopTime,QueueSize,OpenMarketTime,CheckTime,Throughput;    
    
    if( Cfg.GetValue( Name, "Stop", StopTime ) == TRUE && 
        Cfg.GetValue( Name, "QueueSize",QueueSize ) == TRUE && 
        Cfg.GetValue( Name, "OpenMarketTime",OpenMarketTime ) == TRUE )
    {
        FOpenMarketTime = OpenMarketTime.ToInt();
        FStopTime       = StopTime.ToInt();        
        FMaxQueue       = QueueSize.ToInt();                       
        FTryPVCCount    = 0;   
        FRetryInterval  = 0;           
        FMaxQueueBeforeOpenMarket = FMaxQueue;                
        FTryOrderTime   = FOpenMarketTime;               
        FInitTryPVCCount = 0;
        if( Cfg.GetValue( Name, "SwitchPVCThroughput", Throughput ) == TRUE )
            FSwitchPVCThroughput = Throughput.ToInt();
        UFC::BufferedLog::Printf( " -------- Setting for %s trade session --------",Name.c_str() );             
        UFC::BufferedLog::Printf( " [%s] Open at %02d:%02d:%02d",Name.c_str(), FOpenMarketTime/10000, (FOpenMarketTime%10000)/100, FOpenMarketTime%100 );     
        UFC::BufferedLog::Printf( " [%s] Sleeping at %02d:%02d:%02d",Name.c_str(), FStopTime/10000, (FStopTime%10000)/100, FStopTime%100 );     
        UFC::BufferedLog::Printf( " [%s] Switch New PVC Throughput:%d orders/Sec.",Name.c_str(), FSwitchPVCThroughput );     
        if( Cfg.GetValue( Name, "CheckPVCTime", CheckTime ) == TRUE )
            FCheckConnectionTime = CheckTime.ToInt();
        else
            FCheckConnectionTime = 0;
	if( Recover == FALSE )  
	{
	    FStatus = msWait;              
	    FLastStatus = (Int32)msWait;
	}
	else
	    FStatus = (TMarketStatus)((Int32)FLastStatus);
	UFC::BufferedLog::Printf( " [%s] Status = %s.",Name.c_str(), StateString( FStatus ));		
    }
    else
    {
        UFC::BufferedLog::Printf( " Incorrect schedule setting for [%s] trade session.", Name.c_str());     
        exit( 0 );
    }    
}    
//--------------------------------------------------------------------------------------------------------------------
const char*  MarketSchedule::StateString( TMarketStatus Status )
{
    switch( Status )    
   {
       case msWait:     return "Waiting";
       case msTryOrder: return "Trying";
       case msOpened:   return "Market Opened";
       case msClosed:   return "Market Closed";
       case msStop:     return "Stopped";
       default:         return "Unknown";
   }           
}    
//--------------------------------------------------------------------------------------------------------------------
BOOL MarketSchedule::IsTimeToCheckPVC( Int32 Now )
{
    if( FChecked == FALSE &&  Now >= FCheckConnectionTime )
    {
        FChecked = TRUE;
        return TRUE;
    }
    return FALSE;
}
//--------------------------------------------------------------------------------------------------------------------
TMarketStatus MarketSchedule::SetStatus( TMarketStatus NewStatus )
{
    TMarketStatus OrigStatus = FStatus;
    
    if( NewStatus != FStatus )
    {
        UFC::BufferedLog::Printf( " Market[%s] Change state from [%s] to [%s]", FName.c_str(), StateString(OrigStatus), StateString(NewStatus) );
        UFC::PLockObject Lock( FMarketStatusCS );            
        
        if( NewStatus < FStatus && NewStatus <= msTryOrder )
            FTryPVCCount = FInitTryPVCCount;                
        FStatus     = NewStatus; 
        FLastStatus = (Int32)NewStatus;
    }
    return OrigStatus;
}
//--------------------------------------------------------------------------------------------------------------------
void MarketSchedule::CloseMarket( void )
{   
     if( FStatus < msStop )
    {
        UFC::PLockObject Lock( FMarketStatusCS );    
        
        FStatus     = msClosed; 
        FLastStatus = (Int32)msClosed;
    }
}        
//-----------------------------------------------------------------------------------------------------------------------
//   Values for TMarketStatus
//    msWait     = 0,
//    msTryOrder = 1,
//    msOpened   = 2,
//    msClosed   = 3,
//    msStop     = 4
//-----------------------------------------------------------------------------------------------------------------------
void MarketSchedule::OnTimer( Int32 TimeNow )
{
    if( FNeedTry )
    {
        if( TimeNow < FTryOrderTime ) ///< Time to start threads.
        {
            SetStatus(  msWait );
        }
        else if( TimeNow >= FTryOrderTime &&  TimeNow < FStopTime) ///< Time to begin try order.
        {
            if( FStatus == msWait )
                SetStatus( msTryOrder ); ///< msOpened, msClosed
            ///< Begin to try Order, When any PVC send order to exchange successfully, It will change market state to Opened state.
            ///< When PVC 3 received close market message, It will change market state to Closed state.
        }
        else ///< Time to stop threads.
            SetStatus( msStop );
    }
    else
    {
        if( TimeNow < FOpenMarketTime ) ///< Time to start threads.
        {
            SetStatus(  msWait );
        }
        else if( TimeNow >= FOpenMarketTime &&  TimeNow < FStopTime  ) ///< During trade session.
        {
            if( FStatus == msWait )
                SetStatus( msOpened ); ///< msOpened, msClosed	
            //else if( FStatus != msOpened )
            //    UFC::BufferedLog::Printf( " During trade session[%06d] but state[%s]", TimeNow, StateString(FStatus) );
        }
        else ///< Time to stop threads.
            SetStatus( msStop );
    }
    if( TimeNow >= FOpenMarketTime )
        FIsOpened = TRUE;
    else
        FIsOpened = FALSE;    
    if( TimeNow >= FStopTime )
        FIsOpened = FALSE;    
        
}
//-----------------------------------------------------------------------------------------------------------------------
BOOL MarketSchedule::CanITry( void )
{
    if( FStatus != msTryOrder && FStatus != msWait )
        return FALSE;
    UFC::PLockObject Lock( FMarketStatusCS );    
    if( FTryPVCCount > 0 )
    {
        FTryPVCCount--;
        return TRUE;
    }
    return FALSE;
}
//-----------------------------------------------------------------------------------------------------------------------
Int32 MarketSchedule::IncreasePVCCount( void )
{
    UFC::PLockObject Lock( FPVCCountCS );    
    FPVCCount++;
    return FPVCCount;
}
//-----------------------------------------------------------------------------------------------------------------------
Int32 MarketSchedule::DecreasePVCCount( void )
{
    UFC::PLockObject Lock( FPVCCountCS );    
    FPVCCount--;
    if( FPVCCount < 0 )
        FPVCCount = 0;
    return FPVCCount;
}
//-----------------------------------------------------------------------------------------------------------------------
BOOL MarketSchedule::IsAutoSwitchMarket( void )
{
    if( FType == mtLend || FType == mtTender || FType == mtAuction || FType == mtTenderEx || FType == mtNegotiatePx /*|| FType == mtOddTradeEx*/ )
        return TRUE;
    return FALSE;
}
//-----------------------------------------------------------------------------------------------------------------------
char MarketSchedule::GetAPCode( void )
{
    switch( FType )
    {       
       case mtOddTrade:    return '4';///<
       case mtPostTrade:   return '9';///<       
       case mtLend:        return '5';   
       case mtTender:      return '8';
       case mtTenderEx:    return '7';
       case mtAuction:  
       case mtNegotiatePx: return '6';
       case mtOddTradeEx:  return 'C'; 
       case mtNormal:    
       default:            return '0';///<
    }
}
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------------
