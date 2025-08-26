//------------------------------------------------------------------------------------------------------------------------
#include "CheckManager.h"
//------------------------------------------------------------------------------------------------------------------------
// class CheckManager
//------------------------------------------------------------------------------------------------------------------------
CheckManager::CheckManager( BOOL IsOption, AccountStore* Accounts )
:FIsOption( IsOption ),
 FAccounts( Accounts )
{
}
//------------------------------------------------------------------------------------------------------------------------
Int32 CheckManager::GetOrderType( const char* Order )
{
    if ( memcmp( Order, "3001", 4 ) == 0 ) /// Is an order message
        return 1;
    else if( memcmp( Order, "3002", 4 ) == 0 ) /// Is an reduce order message
        return 2;
    else if( memcmp( Order, "3003", 4 ) == 0 ) /// Is an delete order message
        return 3;
    else if( memcmp( Order, "3005", 4 ) == 0 ) /// Is an query order message
        return 4;
    return 0;
}
//------------------------------------------------------------------------------------------------------------------------
BOOL CheckManager::CheckBlackList( AccountRec* CheckAccount, char BS, UFC::AnsiString& Reason )
{
    if( CheckAccount != NULL ) 
    {
        LockState State = CheckAccount->GetState();
        if( BS == 'B' ) 
        {
            if( State == lsBuy || State == lsBoth ) 
            {
                Reason = "This Account has been locked by RM system (Buy order).";
                return FALSE;
            }
            else
                return TRUE;
        }
        else 
        {
            if( State == lsSell || State == lsBoth ) 
            {
                Reason = "This Account has been locked by RM system (Sell order).";
                return FALSE;
            }
            else
                return TRUE;
        }
    }
    else 
    {
        Reason.Printf( "Account not exists." );
        return FALSE;
    }
}
//------------------------------------------------------------------------------------------------------------------------
BOOL CheckManager::FlowOrderCheckPass( const UFC::AnsiString& Order, UFC::AnsiString& Reason )
{
    UFC::AnsiString Account( Order.c_str() + 26, 7 );
    AccountRec*     AccountData = FAccounts->GetAccount( Account );
    char            PositionEffect = Order[ 70 ]; ///< Original PositionEffect
    Int32           Type;
    
    if( AccountData == NULL ) 
    {
        ///< Account not exists.
        Reason.Printf( "Account not exists." );
        return FALSE;
    }
    ///< Check PositionEffect
    if( FIsOption ) 
    {
        ///< Option Order need set PositionEffect explicit when no BackendSystem
        if( PositionEffect != '0' && PositionEffect != '1' &&  PositionEffect != '2') 
        {
            //< Option Order need set PositionEffect explicit.
            Reason = "Option Order need set PositionEffect explicit.";
            return FALSE;
        }
    }
    ///< Check BlackList
    Type = GetOrderType( Order.c_str() );
    if ( Type == 1 ) ///< New Order
    {
        ///< Check Account Locker (BlackList)
        if( CheckBlackList( AccountData, Order[54], Reason ) == FALSE )
            return FALSE;
    }
    else if ( Type == 0 ) ///< Unknow type
    {
        ///< Order Header Error.
        Reason.Printf( "Order Header %s Error.", Order.SubString( 0, 6).c_str() );
        return FALSE;
    }
    return TRUE;
}
//------------------------------------------------------------------------------------------------------------------------
void CheckManager::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data )        
{
    UFC::AnsiString Order, RejectReason;
        
    if ( Data->get( "ORDER", Order )  )
    {
        LogRQ_Check( Order );
        if( FlowOrderCheckPass( Order, RejectReason ) == FALSE )
        {
            Data->append( "CODE", (Int32) 0 ); ///< Check failed.
            Data->append( "MSG", RejectReason );
            LogSQ_CheckReply( RejectReason, 0 );
        }
        else
        {
            Data->append( "CODE", (Int32) 1 ); ///< Check OK.
            LogSQ_CheckReply( "OK", 1 );
        }
        if( FIsOption ) 
            gMessageObj->Send( "RESULT.OPT", UFC::Hostname, *Data );
        else
            gMessageObj->Send( "RESULT.FUT", UFC::Hostname, *Data );
    }	
}
//------------------------------------------------------------------------------------------------------------------------
