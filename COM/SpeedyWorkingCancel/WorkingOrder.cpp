#include "WorkingOrder.h"
//--------------------------------------------------------------------------
WorkingOrder::WorkingOrder( TExecutionReportMessage* Msg )
:Market( Msg->GetMarket() )
,Side( Msg->GetSide() )    
,OrderID( Msg->GetOrderID())
,BrokerID( Msg->GetBrokerID() )    
,Account( Msg->GetAccount() )    
,CMID( Msg->GetCMID() )
,Symbol( Msg->GetSymbol() )
,Qty( Msg->GetOrderQty() )
,IsOrder( TRUE ) //Order or quote    
,AE( Msg->GetAE() )
,Data( Msg->GetData() )  
{
    if( Msg->GetPositionEffect() == nsOrderMessageDefine::peTMPMarketMaker )
        IsOrder = false; ///< Quote    
}
//--------------------------------------------------------------------------
bool WorkingOrder::Reduce( int RQty ) ///< return true for closed
{
    Qty -= RQty;
    if( Qty <= 0 )
        return true;
    return false;    
}
//--------------------------------------------------------------------------
void WorkingOrder::print( void )
{
    UFC::AnsiString MkStr;
    
    if( Market == nsOrderMessageDefine::mTWFutures ) 
        MkStr = "FUT";
    else if( Market == nsOrderMessageDefine::mTWOptions ) 
        MkStr = "OPT";
    else 
        MkStr = UFC::AnsiString( (int)Market );    
    UFC::BufferedLog::Printf( " %s OID[%s] FCM[%s] Sym[%s] Acc[%s] AE[%s] Qty[%d] Data[%s]",                             
                             MkStr.c_str(),
                             OrderID.c_str(),
                             BrokerID.c_str(),
                             Symbol.c_str(),
                             Account.c_str(),
                             AE.c_str(), 
                             Qty,
                             Data.c_str()); 
}
//--------------------------------------------------------------------------
bool WorkingOrder::MatchAccount( const UFC::AnsiString& ChkAcc ) 
{ 
    if( ChkAcc.Length() == 0 )
        return true;
    return( Account.AnsiCompare(ChkAcc ) == 0); 
}
//--------------------------------------------------------------------------
bool WorkingOrder::MatchAE( const UFC::AnsiString& ChkAE )       
{ 
    if( ChkAE.Length() == 0 )
        return true;    
    return( AE.AnsiCompare( ChkAE ) == 0); 
}
//--------------------------------------------------------------------------
bool WorkingOrder::MatchData( const UFC::AnsiString& ChkData )   
{ 
    if( ChkData.Length() == 0 )
        return true;        
    return( Data.AnsiPos( ChkData ) != -1); 
} 
//--------------------------------------------------------------------------
bool WorkingOrder::MatchProduct( const UFC::AnsiString& ChkPID ) 
{ 
    if( ChkPID.Length() == 0 )
        return true;            
    return( Symbol.AnsiNCompare( ChkPID,ChkPID.Length() ) == 0);
}   
//--------------------------------------------------------------------------
TCancelOrderMessage* WorkingOrder::GetCancelOrderMessage( void )
{
    TCancelOrderMessage* Cxl = new TCancelOrderMessage();  
    Cxl->SetMarket( Market );
    Cxl->SetAccount( Account.c_str() );
    Cxl->SetOrderID( OrderID.c_str() );
    Cxl->SetSymbol( Symbol.c_str() );
    Cxl->SetSide( Side );
    Cxl->SetBrokerID( BrokerID.c_str() );    
    return Cxl;
}    
//--------------------------------------------------------------------------
