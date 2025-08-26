#include "TWSESpeedyOrderClient.h"
#include "TWSESpeedyObjectsManager.h"
#include "../TSECLib/LinkSubSystem.h"
#include "../TSECLib/Connection.h"
#include "../TSECLib/ConnectionObject.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SpeedyOrderClient::SpeedyOrderClient( SpeedyObjectsManager* ObjManager,
                                      ConnectionParameter&  Param,
                                      ConnectionListener*   Listener,
                                      Int32                 SubSystem,
                                      ConnectionObjectBase* Owner,
                                      BOOL                  IsTWSE)
:FIsConnected( FALSE ),
 FConnectionListener( Listener ),
 FOwner( Owner ),
 FSubSystem( SubSystem ),
 FObjectManager( ObjManager ),
 FIsTWSE( IsTWSE )
{
    FX25OrderLine = new LinkSubSystemFCM( FIsTWSE , Param );
    FX25OrderLine->SetLogManager( FObjectManager->OrderLog );
    UFC::StrLCpy( FPVCID, Param.GetPVCID().c_str(), 3 );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SpeedyOrderClient::~SpeedyOrderClient()
{
    Close();
    delete FX25OrderLine;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyOrderClient::Open()
{
    FX25OrderLine->Open( );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyOrderClient::Close()
{
    if( FIsConnected == TRUE )
    {
        FIsConnected = FALSE;
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_BROKEN );
    }
    FX25OrderLine->Close();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// OrderListener interface function
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Call this function when open market.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyOrderClient::OnOpenMarket()
{
    FOrderCount = 0;
    if( FIsConnected == TRUE )
    {
        FIsConnected = FALSE;
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_BROKEN );
    }
    FX25OrderLine->OnlineHandshake( );
    FIsConnected = TRUE;
    FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_READY, NULL );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyOrderClient::Handshake( BOOL ReceiveL010 )
{
    if( FIsConnected == TRUE )
    {
        FIsConnected = FALSE;
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_BROKEN );
    }
    FX25OrderLine->OnlineHandshake( ReceiveL010 );
    FIsConnected = TRUE;
    FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_READY, NULL );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyOrderClient::OnReconnect()
{
    if( FIsConnected == TRUE )
    {
        FIsConnected = FALSE;
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_BROKEN );
    }
    FX25OrderLine->OnlineHandshake( FALSE );
    FIsConnected = TRUE;
    FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_READY, NULL );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// When this connection idle over 60 sec, perform the idle handshake (R040/R050 exchange)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyOrderClient::OnIdle()
{
    MessageHeader* Msg;
          
    if( FSubSystem == 30 || FSubSystem == 93 ) ///< Normal
    {
        MessageT040    T040( FIsTWSE ); // R040 Confirm Connection message
        FX25OrderLine->SendMessage( &T040, TWSE_LINKSUBSYSTEM_TIMEOUT );// Send the confirm connection message
    }
    else if( FSubSystem == 32 || FSubSystem == 98 ) ///< Post trade
    {
        MessageP040    P040( FIsTWSE ); // P040 Confirm Connection message
        FX25OrderLine->SendMessage( &P040, TWSE_LINKSUBSYSTEM_TIMEOUT );// Send the confirm connection message
    }
    else if( FSubSystem == 40 || FSubSystem == 94 ) /// Odd order
    {
        MessageO070    O070( FIsTWSE ); // O070 Confirm Connection message
        FX25OrderLine->SendMessage( &O070, TWSE_LINKSUBSYSTEM_TIMEOUT );// Send the confirm connection message
    }
    else if( FSubSystem == 33 || FSubSystem == 83 ) ///< OddEx Order
    {
        MessageO140    O140( FIsTWSE ); // R040 Confirm Connection message
        FX25OrderLine->SendMessage( &O140, TWSE_LINKSUBSYSTEM_TIMEOUT );// Send the confirm connection message
    }
    else if( FSubSystem == 31 || FSubSystem == 90 ) /// Lend order
    {
        MessageV040    V040( FIsTWSE ); // V040 Confirm Connection message
        FX25OrderLine->SendMessage( &V040, TWSE_LINKSUBSYSTEM_TIMEOUT );// Send the confirm connection message
    }
    else if( FSubSystem == 41 || FSubSystem == 97 ) /// Tender offer order
    {
        MessageE040    E040( FIsTWSE ); // E040 Confirm Connection message
        FX25OrderLine->SendMessage( &E040, TWSE_LINKSUBSYSTEM_TIMEOUT );// Send the confirm connection message
    }
    else if( FSubSystem == 42  || FSubSystem == 89  ) /// TenderEx offer order
    {
        MessageEx040    Ex040( FIsTWSE ); // Ex040 Confirm Connection message
        FX25OrderLine->SendMessage( &Ex040, TWSE_LINKSUBSYSTEM_TIMEOUT );// Send the confirm connection message
    }
    else if( FSubSystem == 70 ) /// Auction order
    {
        MessageA040    A040; // A040 Confirm Connection message
        FX25OrderLine->SendMessage( &A040, TWSE_LINKSUBSYSTEM_TIMEOUT );// Send the confirm connection message
    }
    else if( FSubSystem == 96 ) /// Negotiate Price
    {
        MessageS130    S130; // S130 Confirm Connection message
        FX25OrderLine->SendMessage( &S130, TWSE_LINKSUBSYSTEM_TIMEOUT );// Send the confirm connection message
    }
    if( ( Msg = FX25OrderLine->ReceiveMessage( TWSE_LINKSUBSYSTEM_TIMEOUT ) ) != NULL )
    {
        TMessageType Type = Msg->MessageType( );        
        
        switch( Type )
        {
            case mtT050: ///< Normal
            case mtP050: ///< Post trade
            case mtO080: ///< Odd lots           
            case mtO150: ///< Odd Ex
            case mtV050: ///< Lend
            case mtE050: ///< Tender offer 
            case mtEx050:///< TenderEx offer 
            case mtA050: ///< Auction
            case mtS140: ///< Negotiate Price
                         delete Msg; ///< Is a comfirm reply message.                                 
                         break;
            case mtL070: ///< Close market                             
                         delete Msg;
                         throw( MarketClosedException("Market Closed") );
                         break;
            default:     delete Msg;
                         UFC::BufferedLog::Printf( " Confirm connection failed, not a Confirm reply message!" );
                         break;
        }
    }
    else
        UFC::BufferedLog::Printf( " Confirm connection failed, timeout!" );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderClient::IsReady( void )
{
    return (FX25OrderLine->GetStatus() == PVC_STATUS_ONLINE_READY );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Call this function to send order to TAIFEX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderClient::OnOrder( MessageHeader* OrderMessage, OrderRecord& OI  )
{
    TMessageType Type = OrderMessage->MessageType( );

    switch( Type )
    {
            case mtT010: // T010 order message. Copy the PVCID to the message
                         memcpy(((MessageT010*)OrderMessage)->PVCID,FPVCID,2);
                         break;
            case mtO010: // O010 order message. Copy the PVCID to the message
                         memcpy(((MessageO010*)OrderMessage)->PVCID,FPVCID,2);
                         break;                         
            case mtO110: // O110 order message. Copy the PVCID to the message
                         memcpy(((MessageO110*)OrderMessage)->PVCID,FPVCID,2);
                         break;                         
            case mtP010: // P010 order message. Copy the PVCID to the message
                         memcpy(((MessageP010*)OrderMessage)->PVCID,FPVCID,2);
                         break;                         
            case mtV010: // V010 Lend message. Copy the PVCID to the message
                         memcpy(((MessageV010*)OrderMessage)->PVCID,FPVCID,2);
                         break;                                                  
            case mtE010: // E010 Tender offer message. Copy the PVCID to the message
                         memcpy(((MessageE010*)OrderMessage)->PVCID,FPVCID,2);
                         break;                                                                           
            case mtEx010:// Ex010 TenderEx offer message. Copy the PVCID to the message
                         memcpy(((MessageEx010*)OrderMessage)->PVCID,FPVCID,2);
                         break;                                                                                                    
            case mtA010: // A010 Auction message. Copy the PVCID to the message
                         memcpy(((MessageA010*)OrderMessage)->PVCID,FPVCID,2);
                         break;                                                                                                    
            case mtS010:
            case mtS030:
            case mtS050:
            case mtS070:                
            case mtS090:    
            case mtS110:    
                        break; ///< Negotiate Px messages don't have PVCID field. 
            case mtP060:
            case mtO090:
            case mtT060: 
            case mtA060:
            case mtE060:
            case mtEx060:
            case mtV060: break; // O090,P060,T060,A060,E060,V060 Reconnect inquire message            
            default:     UFC::BufferedLog::Printf( " Un-support Message Type %d\n", Type );
                         return FALSE;
    }
    FOrderCount++;
    OrderMessage->UpdateTime();
    if( FX25OrderLine->SendMessage( OrderMessage, TWSE_ORDERSUBSYSTEM_TIMEOUT ) )
    	return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Call this function to retrieve the reply message from the TAIFEX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* SpeedyOrderClient::OnHandleReplyMessage( TMessageType& Type , BOOL& NeedLog, BOOL& IsCancelDel, UFC::AnsiString& ExecKey, UFC::AnsiString& OrigKey, UFC::AnsiString& BrokerID )
{
    MessageHeader*   Msg = FX25OrderLine->ReceiveMessage( TWSE_ORDERSUBSYSTEM_TIMEOUT );
    UFC::AnsiString  OrderID,Time;
    Int32            Func,OrigNID;    

    if( Msg != NULL )
    {
        Type = Msg->MessageType( );
        Func = Msg->GetFunctionCode();
        Time = UFC::AnsiString( Msg->GetTimeStr(), 6 );
        NeedLog = FALSE;
        IsCancelDel = FALSE;
        switch( Type )
        {
            case mtL010: return NULL;
            case mtL070: UFC::BufferedLog::Printf( " Receive L070 status code:%d", Msg->GetStatusCode() );    
                         return NULL;
            case mtT020: BrokerID = UFC::AnsiString( ((MessageT020*)Msg)->BrokerID, 4 );
                         OrderID  = UFC::AnsiString( ((MessageT020*)Msg)->OrderNo, 5 );                                     
                         ExecKey.Printf("T020_%d_%s_%s%s", Func, Time.c_str(),BrokerID.c_str(), OrderID.c_str() );
                         if( Func == 1 || Func == 2 || Func == 3 || Func == 4 )
                             NeedLog = TRUE;
                         if( Func == 3 || Func == 4 )
                         {
                             IsCancelDel = TRUE;
                             if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/OrderID, /*In*/BrokerID, OrigKey, OrigNID ) == FALSE )    
                             {    
                                 UFC::BufferedLog::Printf( " T020 Can't find Original Order BrokerID[%s] OrderID[%s]",BrokerID.c_str(), OrderID.c_str() );
                                 OrigKey = "";
                             }
                         }
                         return Msg;
            case mtO020: BrokerID = UFC::AnsiString( ((MessageO020*)Msg)->BrokerID, 4 );
                         OrderID  = UFC::AnsiString( ((MessageO020*)Msg)->OrderNo, 5 );            
                         ExecKey.Printf("O020_%d_%s_%s%s", Func,  Time.c_str(),BrokerID.c_str(), OrderID.c_str() );
                         if( Func == 1 || Func == 2 || Func == 3 || Func == 4 )
                             NeedLog = TRUE;
                         if( Func == 3 || Func == 4 )
                         {
                             IsCancelDel = TRUE;
                             if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/OrderID, /*In*/BrokerID, OrigKey, OrigNID ) == FALSE )    
                             {    
                                 UFC::BufferedLog::Printf( " O020 Can't find Original Order BrokerID[%s] OrderID[%s]",BrokerID.c_str(), OrderID.c_str() );
                                 OrigKey = "";
                             }
                         }
                         return Msg;
            case mtO120: BrokerID = UFC::AnsiString( ((MessageO120*)Msg)->BrokerID, 4 );
                         OrderID  = UFC::AnsiString( ((MessageO120*)Msg)->OrderNo, 5 );                                     
                         ExecKey.Printf("O120_%d_%s_%s%s", Func, Time.c_str(),BrokerID.c_str(), OrderID.c_str() );
                         if( Func == 1 || Func == 2 || Func == 3 || Func == 4 )
                             NeedLog = TRUE;
                         if( Func == 3 || Func == 4 )
                         {
                             IsCancelDel = TRUE;
                             if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/OrderID, /*In*/BrokerID, OrigKey, OrigNID ) == FALSE )    
                             {    
                                 UFC::BufferedLog::Printf( " O120 Can't find Original Order BrokerID[%s] OrderID[%s]",BrokerID.c_str(), OrderID.c_str() );
                                 OrigKey = "";
                             }
                         }
                         return Msg;                         
            case mtP020: BrokerID = UFC::AnsiString( ((MessageP020*)Msg)->BrokerID, 4 );
                         OrderID  = UFC::AnsiString( ((MessageP020*)Msg)->OrderNo, 5 );            
                         ExecKey.Printf("P020_%d_%s_%s%s", Func,  Time.c_str(), BrokerID.c_str(), OrderID.c_str()  );
                         if( Func == 1 || Func == 2 || Func == 3 || Func == 4 )
                             NeedLog = TRUE;
                         if( Func == 3 || Func == 4 )
                         {
                             IsCancelDel = TRUE;
                             if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/OrderID, /*In*/BrokerID, OrigKey, OrigNID ) == FALSE )    
                             {
                                 UFC::BufferedLog::Printf( " P020 Can't find Original Order BrokerID[%s] OrderID[%s]",BrokerID.c_str(), OrderID.c_str() );                              
                                 OrigKey = "";
                             }
                         }
                         return Msg; 
            case mtA020: BrokerID = UFC::AnsiString( ((MessageA020*)Msg)->BrokerID, 4 );
                         OrderID  = UFC::AnsiString( ((MessageA020*)Msg)->OrderNo, 5 );                                     
                         ExecKey.Printf("A020_%d_%s_%s%s", Func,  Time.c_str(), BrokerID.c_str(), OrderID.c_str()  );                                                  
                         if( Func == 1 || Func == 2 || Func == 3 )
                             NeedLog = TRUE;
                         if( Func == 2 || Func == 3 )
                         {
                             IsCancelDel = TRUE;
                             if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/OrderID, /*In*/BrokerID, OrigKey, OrigNID ) == FALSE )    
                             {
                                 UFC::BufferedLog::Printf( " A020 Can't find Original Order BrokerID[%s] OrderID[%s]",BrokerID.c_str(), OrderID.c_str() );
                                 OrigKey = "";
                             }
                         }
                         return Msg;
            case mtE020: BrokerID = UFC::AnsiString( ((MessageE020*)Msg)->BrokerID, 4 );
                         OrderID  = UFC::AnsiString( ((MessageE020*)Msg)->OrderNo, 5 );            
                         ExecKey.Printf("E020_%d_%s_%s%s", Func,  Time.c_str(), BrokerID.c_str(), OrderID.c_str()  );
                         if( Func == 1 || Func == 2 || Func == 3 )
                             NeedLog = TRUE;
                         if( Func == 2 || Func == 3 )
                         {
                             IsCancelDel = TRUE;
                             if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/OrderID, /*In*/BrokerID, OrigKey, OrigNID ) == FALSE )    
                             {    
                                 UFC::BufferedLog::Printf( " E020 Can't find Original Order BrokerID[%s] OrderID[%s]",BrokerID.c_str(), OrderID.c_str() );
                                 OrigKey = "";
                             }
                         }
                         return Msg;
            case mtEx020: BrokerID = UFC::AnsiString( ((MessageEx020*)Msg)->BrokerID, 4 );
                         OrderID  = UFC::AnsiString( ((MessageEx020*)Msg)->OrderNo, 5 );            
                         ExecKey.Printf("Ex020_%d_%s_%s%s", Func,  Time.c_str(), BrokerID.c_str(), OrderID.c_str()  );
                         if( Func == 1 || Func == 2 || Func == 3 )
                             NeedLog = TRUE;
                         if( Func == 2 || Func == 3 )
                         {
                             IsCancelDel = TRUE;
                             if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/OrderID, /*In*/BrokerID, OrigKey, OrigNID ) == FALSE )    
                             {
                                 UFC::BufferedLog::Printf( " Ex020 Can't find Original Order BrokerID[%s] OrderID[%s]",BrokerID.c_str(), OrderID.c_str() );
                                 OrigKey = "";
                             }
                         }
                         return Msg;                         
            case mtV020: BrokerID = UFC::AnsiString( ((MessageV020*)Msg)->BrokerID, 4 );
                         OrderID  = UFC::AnsiString( ((MessageV020*)Msg)->OrderNo, 5 );            
                         ExecKey.Printf("V020_%d_%s_%s%s", Func,  Time.c_str(), BrokerID.c_str(), OrderID.c_str()  );
                         if( Func == 1 || Func == 2 || Func == 3 )
                             NeedLog = TRUE;
                         if( Func == 2 || Func == 3 )
                         {
                             IsCancelDel = TRUE;
                             if( FObjectManager->OrderInfo.GetOrderInfo( /*In*/OrderID, /*In*/BrokerID,OrigKey, OrigNID ) == FALSE )    
                             {
                                 UFC::BufferedLog::Printf( " V020 Can't find Original Order BrokerID[%s] OrderID[%s]",BrokerID.c_str(), OrderID.c_str() );
                                 OrigKey = "";
                             }
                         }
                         return Msg;                        
            case mtS020:
            case mtS040:
            case mtS060:
            case mtS070:                
            case mtS100:    
            case mtS120: ///< Negotiate Px messages don't keep in OrderInfo.                          
            default:     ExecKey = "null";
                         return Msg;
         }
    }
    return NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  SpeedyOrderClient::SetTradeSession( char APCode, Int32 SubSystem )
{
    FSubSystem = SubSystem;
    UFC::BufferedLog::Printf( " SpeedyOrderClient::Change to AP code:%c SubSystem: %d", APCode, FSubSystem );    
    FX25OrderLine->SetAPCode( APCode );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MarketSchedule* SpeedyOrderClient::GetMarket( void )
{
    return FObjectManager->GetScheduleObjectBySubsystemID( FSubSystem );
}        
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Int32 SpeedyOrderClient::GetStatus()
{
    return FX25OrderLine->GetStatus();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyOrderClient::SetStatus( Int32 Status)
{
    FX25OrderLine->SetStatus( Status );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyOrderClient::StopHandshake()
{
    FX25OrderLine->StopHandshake();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyOrderClient::Listen( BOOL IsListen )
{
    if( FX25OrderLine != NULL )
    {
        if( IsListen == TRUE )
            FX25OrderLine->SetListener( FOwner );
        else
            FX25OrderLine->SetListener( NULL );
    }
}
//-----------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



