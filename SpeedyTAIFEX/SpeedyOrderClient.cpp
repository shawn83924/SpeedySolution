#include "SpeedyOrderClient.h"
#include "../TAIFEXLib/LinkSubSystem.h"
#include "../TAIFEXLib/Connection.h"
#include "../TAIFEXLib/ConnectionObject.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constractor
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SpeedyOrderClient::SpeedyOrderClient( ConnectionParameter &Param,
                                      ConnectionListener* Listener,
                                      ConnectionObjectBase* Owner,
                                      LogManager* OrderLogManager )
:FBrokerID(Param.GetBrokerID()),
 FLinePassword(Param.GetPassword()),
 FLine( Param.GetLink() ),
 FPVCNumber( Param.GetPVC()),
 FIsConnected( FALSE ),
 FConnectionListener( Listener ),
 FOwner( Owner )
{
    FX25OrderLine = new LinkSubSystemFCM( FLine, Param.IsOptions(), FPVCNumber, FLinePassword, FBrokerID,'4' );
    FX25OrderLine->SetLogManager( OrderLogManager );
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
    FX25OrderLine->Open();
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
    if( FX25OrderLine->OnlineHandshake( ) == TRUE )
    {
        FIsConnected = TRUE;
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_READY, NULL );
    }
    else
        throw X25Exception(-9,"Handshake exception");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyOrderClient::Handshake( BOOL ReceiveL010 )
{
    if( FIsConnected == TRUE )
    {
        FIsConnected = FALSE;
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_BROKEN );
    }
    if( FX25OrderLine->OnlineHandshake( ReceiveL010 ) == TRUE )
    {
        FIsConnected = TRUE;
        FConnectionListener->OnConnectionNotify( FOwner, CONNECTION_OBJECT_PVC_READY, NULL );
    }
    else
        throw X25Exception(-9,"Handshake exception");
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// When this connectioon idle over 60 sec, perform the idle handshake (R040/R050 exchange)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SpeedyOrderClient::OnIdle()
{
    MessageR040    R040; // R040 Confirm Connection message
    MessageHeader* Msg;

    FX25OrderLine->SendMessage( &R040, LINKSUBSYSTEM_TIMEOUT );// Send the confirm connection message
    if( ( Msg = FX25OrderLine->ReceiveMessage( LINKSUBSYSTEM_TIMEOUT ) ) != NULL )
    {
        if( Msg->IsMessage( mtR050 ) == TRUE  ) ///< Is a R050
        {
            delete Msg;
        }
        else if( Msg->IsMessage( mtL070 ) == TRUE ) ///< Close market
        {
            delete Msg;
            throw MarketClosedException("Market Closed");
        }
        else
        {
            delete Msg;
            UFC::BufferedLog::Printf( " Confirm connection failed, not a R050 message!\n" );
        }
    }
    else
        UFC::BufferedLog::Printf( " Confirm connection failed, timeout!\n" );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderClient::IsReady( void )
{
    return (FX25OrderLine->GetStatus() == PVC_STATUS_ONLINE_READY );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
UInt8 SpeedyOrderClient::GetHeartBeatInterval( void )
{ 
    return IDLEHANDSHAKE_TIME_XBIO4; 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Call this function to send order to TAIFEX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL SpeedyOrderClient::OnOrder( MessageHeader* OrderMessage, Int64 )
{
    TMessageType Type = OrderMessage->MessageType( );

    switch( Type )
    {
            case mtR010:                             // R010 order message.
            case mtR090:                             // R090 Quote message.
            case mtR060:                             // R060 Reconnect inquire message
            case mtR070: break;                      // R070 Request message
            case mtR040: return FALSE;               // R040 Skip the confirm connection message
            default:     UFC::BufferedLog::Printf( " Unsupport Message Type %d\n", Type );
                         return FALSE;
    }
    FOrderCount++;
    if( FX25OrderLine->SendMessage( OrderMessage, ORDERSUBSYSTEM_TIMEOUT ) )
    	return TRUE;
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Call this function to retrive the reply message from the TAIFEX
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* SpeedyOrderClient::OnHandleReplyMessage( TMessageType& Type , BOOL& NeedLog, UFC::AnsiString& ExecKey )
{
    MessageHeader*   Msg = FX25OrderLine->ReceiveMessage( ORDERSUBSYSTEM_TIMEOUT );
    UFC::AnsiString  SeqStr;
    Int32            Func;

    if( Msg != NULL )
    {
        Type = Msg->MessageType( );
        NeedLog = FALSE;
        switch( Type )
        {
            case mtL010: return NULL;
            case mtL070: throw MarketClosedException("Market Closed");
            case mtR020: SeqStr = UFC::AnsiString( ((MessageR020*)Msg)->SequenceNo, 8 );
                         Func = Msg->GetFunctionCode();
                         ExecKey.Printf("R020_%d_%s", Func, SeqStr.c_str() );
                         if( Func == 1 )
                             NeedLog = TRUE;
                         return Msg;
            case mtR080: SeqStr = UFC::AnsiString( ((MessageR080*)Msg)->SequenceNo, 8 );
                         Func = Msg->GetFunctionCode();
                         ExecKey.Printf("R080_%d_%s", Func, SeqStr.c_str() );
                         return Msg;
            case mtR100: SeqStr = UFC::AnsiString(((MessageR100*)Msg)->SequenceNo, 8 );
                         Func = Msg->GetFunctionCode();
                         ExecKey.Printf("R100_%d_%s", Func, SeqStr.c_str() );
                         if( Func == 1 )
                             NeedLog = TRUE;
                         return Msg;
            default:     ExecKey = "null";
                         return Msg;
         }
    }
    return NULL;
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
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



