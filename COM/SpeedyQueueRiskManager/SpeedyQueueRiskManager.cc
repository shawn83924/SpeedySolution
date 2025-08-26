#include "SpeedyQueueRiskManager.h"
#include "../UFC/NameValueMessage.h"
//------------------------------------------------------------------------------
extern BOOL DumpOnly;
const Int32 MSG_LENGTH = 219;
extern UFC::UiniFile* ErrorCodeTable;
//------------------------------------------------------------------------------
SpeedyQueueConnection::SpeedyQueueConnection( const UFC::AnsiString& SendQueueID, 
                                              const UFC::AnsiString& RecvQueueID, 
                                              MessageObject* MessageObj, 
                                              EchangeEnum Exchange  )
:UFC::PThread( NULL, FALSE )
,FMessageObject( MessageObj )
,FSendQueue( FALSE )
,FRecvQueue( FALSE )
,FExchange( Exchange )
{
    long QID;
    UFC::AnsiString Ex;

    switch( FExchange )
    {
        case eTAIFEX:    Ex = "TAIFEX"; break;
        case eTWSEOTC:   Ex = "TSEC"; break;
        case ePATS:      Ex = "PATS"; break;
    }
    QID = strtol( SendQueueID.c_str(),0,0);
    UFC::BufferedLog::Printf( " Open %s Send to risk manager queue[%x]", Ex.c_str(), QID );
    if( FSendQueue.Open( QID, TRUE ) == FALSE )
    {
        UFC::BufferedLog::Printf( " Open %s Send queue:%x failed", Ex.c_str(),QID );
        exit( 0 );
    }

    QID = strtol( RecvQueueID.c_str(),0,0);
    UFC::BufferedLog::Printf( " Open %s Recv checked order queue[%x]", Ex.c_str(),QID );
    if( FRecvQueue.Open( QID, TRUE ) == FALSE )
    {
        UFC::BufferedLog::Printf( " Open %s Recv queue:%x failed", Ex.c_str(),QID );
        exit( 0 );
    }
    Start();
}
//------------------------------------------------------------------------------
void SpeedyQueueConnection::Execute( void )
{
    UFC::UQueueData  OrderData( 0, 1024 );

    UFC::SleepMS( 3000 );
    while( TRUE )
    {
        try ///< Dequeue an execution from Speedy.
        {   ///< Get order data from queue.
            OrderData.SetMType( 0 );

            FRecvQueue.Recv( OrderData );
            if(  OrderData.GetSize() >= MSG_LENGTH )
            {
                UFC::AnsiString Result( (char*)OrderData.GetData() );

                OnRecvMessage( Result );
            }
        }
        catch( UFC::Exception& e )
        {
            UFC::BufferedLog::Printf( " FRecvQueue.Recv Exception:%s", e.what() );
            UFC::BufferedLog::Printf( " MType:%d Len:%d", OrderData.GetMType(), OrderData.GetSize() );
        }
    }
}
//------------------------------------------------------------------------------
UFC::AnsiString SpeedyQueueConnection::GetPATSOrderID( const UFC::AnsiString& Order )
{
    UFC::NameValueMessage PATSMessage("^\n");
    UFC::AnsiString OID( "00000" );
    PATSMessage.FromString( Order );  
    PATSMessage.Get( "oid", OID );
    return OID;
}    
//------------------------------------------------------------------------------
// Receive Order from MBus
// Send to Queue.
//------------------------------------------------------------------------------
void SpeedyQueueConnection::OnMessage( const UFC::AnsiString& SendMsg, Market mkt, MTree* Data )
{
    try
    {
        UFC::UQueueData QueueData( (long)1, 1024 );

        memcpy( QueueData.GetData(), SendMsg.c_str(), SendMsg.Length() +1 );
        FSendQueue.Send( QueueData );
    }
    catch( UFC::Exception& e )
    {
        Data->append( "CODE", 0 );                
        Data->append( "MSG", "Can not send this order to backend" );
        if( mkt == mFutures )
            FMessageObject->Send( "RESULT.FUT", UFC::Hostname, *Data );
        else if( mkt == mOptions )
            FMessageObject->Send( "RESULT.OPT", UFC::Hostname, *Data );
        else if( mkt == mTSE  )
            FMessageObject->Send( "RESULT.TSE", UFC::Hostname, *Data );
        else if( mkt == mOTC )
            FMessageObject->Send( "RESULT.OTC", UFC::Hostname, *Data );
        else if( mkt == mPATS )
            FMessageObject->Send( "RESULT.PATS", UFC::Hostname, *Data );        
    }
}
//------------------------------------------------------------------------------
//
// RiskModule -> Queue -> SpeedyQueueRiskManager -> Speedy
//
// Receive checked order result from Risk module
// Send check result to SpeedyGateway
//------------------------------------------------------------------------------
// 0-74    TAIFEX/TWSE Order format   ( PATS 256)
// 75-78   Risk manager Error code
// 79      Market. TAIFEX: F/O TWSE: T/O
// 80-89   NID.
// 90-359  User Define data.
// 360-368 Order time.
// 369     End char.     
//------------------------------------------------------------------------------
void SpeedyQueueConnection::OnRecvMessage( const UFC::AnsiString& Message )
{
    int             OrderLen = 75;    
    UFC::AnsiString OrderID,OrderStr;
    MTree           Data;    
    
    switch( FExchange )
    {
        case eTAIFEX:   OrderStr = Message.SubString( 0, OrderLen );
                        OrderID  = OrderStr.SubString( 21, 5 );                                     
                        break;
        case eTWSEOTC:  OrderStr = Message.SubString( 0, OrderLen );
                        OrderID  = OrderStr.SubString( 20, 5 );
                        OrderLen = 75;
                        break;
        case ePATS:     OrderLen = 256;
                        OrderStr = Message.SubString( 0, OrderLen );
                        OrderID  = GetPATSOrderID( OrderStr  );                                   
                        break;
    }
    UFC::AnsiString ErrorCode( Message.SubString( OrderLen, 4 ) );    ///< Order Length: 75
    char            Market = Message[ OrderLen + 4 ];                 ///< Error Code: 4
    UFC::AnsiString NIDStr(Message.SubString( OrderLen + 4 + 1, 10 ));///< NID Length: 10 
    UFC::AnsiString Key(Message.SubString( OrderLen +4 +1 +10, 270 ));///< User Date : 270        
    Int32           NID = NIDStr.ToInt();
    
    Key.TrimRight();
    OrderStr.TrimRight();
    UFC::BufferedLog::Printf( " Recv Queue:[%s]", Message.c_str() );
    UFC::BufferedLog::DebugPrintf( " ErrorCode:[%s]", ErrorCode.c_str() );
    UFC::BufferedLog::DebugPrintf( " Market:[%c]", Market );   
    UFC::BufferedLog::DebugPrintf( " NID:[%010d]", NID );
    UFC::BufferedLog::DebugPrintf( " Key:[%s]", Key.c_str() );
    UFC::BufferedLog::DebugPrintf( " OrderID:[%s]", OrderID.c_str() );

    Data.append( "NID",   NID );
    Data.append( "KEY",   Key );
    Data.append( "ORDER", OrderStr );
    Data.append( "OID",   OrderID );
    
    if( ErrorCode.AnsiCompare( "0000" ) != 0 ) ///< backend Rejected
    {
        UFC::AnsiString MSG;

        if( ErrorCodeTable != NULL && ErrorCodeTable->GetValue( "Error", ErrorCode, MSG ) )
            UFC::BufferedLog::DebugPrintf( " ErrorCode:[%s] %s",ErrorCode.c_str(), MSG.c_str() );
        else
            MSG.Printf( "ErrorCode:[%s]", ErrorCode.c_str() );
        Data.append( "MSG", MSG );
        Data.append( "CODE", 0 );
    }
    else
        Data.append( "CODE", 1 );
    switch( FExchange )
    {
        case eTAIFEX:   if( Market == 'F' )
                            FMessageObject->Send( "RESULT.FUT", UFC::Hostname, Data );
                        else
                            FMessageObject->Send( "RESULT.OPT", UFC::Hostname, Data );
                        break;        
        case eTWSEOTC:  if( Market == 'T') 
                            FMessageObject->Send( "RESULT.TSE", UFC::Hostname, Data );
                        else if( Market == 'O' )
                            FMessageObject->Send( "RESULT.OTC", UFC::Hostname, Data );
                        else    
                            FMessageObject->Send( "RESULT.ES", UFC::Hostname, Data );
                        break;        
        case ePATS:     FMessageObject->Send( "RESULT.PATS", UFC::Hostname, Data );
                        break;
    }    
}
//------------------------------------------------------------------------------






