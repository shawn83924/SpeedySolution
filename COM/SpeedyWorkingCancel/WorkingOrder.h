/* 
 * File:   WorkingOrder.h
 * Author: yuan
 *
 * Created on 2016年4月9日, 下午 10:34
 */

#ifndef WORKINGORDER_H
#define	WORKINGORDER_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
//--------------------------------------------------------------------------
class WorkingOrder
{
private:    
    nsOrderMessageDefine::MarketEnum Market;        
    nsOrderMessageDefine::SideEnum   Side;    
private:
    UFC::AnsiString OrderID;
    UFC::AnsiString BrokerID;    
    UFC::AnsiString Account;    
    UFC::AnsiString CMID;
    UFC::AnsiString Symbol;  
    int             Qty;
    bool            IsOrder; //Order or quote    
private:    
    UFC::AnsiString AE;        
    UFC::AnsiString Data;        
public:
    WorkingOrder( TExecutionReportMessage* Msg );
    bool Reduce( int Qty ); ///< return true for closed
    void print( void );
public:    
    bool MatchAccount( const UFC::AnsiString& ChkAcc );
    bool MatchAE( const UFC::AnsiString& ChkAE );
    bool MatchData( const UFC::AnsiString& ChkData );
    bool MatchProduct( const UFC::AnsiString& ChkPID );
    TCancelOrderMessage* GetCancelOrderMessage( void );
};
//--------------------------------------------------------------------------
#endif	/* WORKINGORDER_H */

