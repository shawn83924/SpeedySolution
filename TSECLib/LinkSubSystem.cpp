/*#ident "@(#)$Id: LinkSubSystem.cpp,v 1.97 2004/08/14 08:03:34 ap25 Exp $"
*  @file OnlineSunSystem.cpp
*  @brief Class implement Online subsystem.
*  @author Simon Chaang
*
*/
#include "LinkSubSystem.h"
#include "ConnectionObject.h"
#include "SocketConnection.h"
#include "ExchangeConst.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LinkSubSystemBase::LinkSubSystemBase( BOOL IsTWSE,
                                      int Line,
                                      int PVCNumber,
                                      const char* Password,
                                      const char* BrokerID,
                                      char  APCode,
                                      const char* IP,
                                      int   LocalPort,
                                      const char* PVCID  )
:FLine( Line ),
 FLocalPort( LocalPort ),
 FPVCNumber( PVCNumber ),
 FAPCode( APCode ),
 FBrokerID( BrokerID ),
 FPassword( Password ),
 FAddress( IP ),
 FPVCID( PVCID ),
 FSocketFD( -1 ), 
 FStatus( PVC_STATUS_UNKNOWN ),
 FLockReason( PVC_STATUS_SUBSYS_ERR ),
 FLinkSubSysStatus( FALSE ),
 FStopHandShake( FALSE ),
 FIsTWSE( IsTWSE ),
 FLintener( NULL ),
 FLogManager( NULL )
{
    FPVCID.PadThis( 2,' ' );
    FConnection = new SocketConnection();
    FIsTCP = TRUE;
    snprintf( FLinkPVCSendStr, sizeof(FLinkPVCSendStr), "Port%05d PVC:%s Send:",FLocalPort, FPVCID.c_str() );
    snprintf( FLinkPVCRecvStr, sizeof(FLinkPVCRecvStr), "Port%05d PVC:%s Recv:",FLocalPort, FPVCID.c_str() );
    FBuffer     = new char[ TWSE_MAX_MESSAGE_SIZE ];
    FLogBuffer  = new char[ TWSE_MAX_MESSAGE_SIZE * 2 ];    
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LinkSubSystemBase::LinkSubSystemBase( BOOL IsTWSE, ConnectionParameter& Params , int FD )
:FLine( Params.GetLink() ),
 FLocalPort( Params.GetLocalPort() ),
 FPVCNumber( Params.GetPVC() ),
 FAPCode( Params.GetAPCode() ),
 FBrokerID( Params.GetBrokerID() ),
 FPassword( Params.GetPassword() ),
 FAddress( Params.GetAddress() ),
 FPVCID( Params.GetPVCID() ),
 FSocketFD( FD ),
 FStatus( PVC_STATUS_UNKNOWN ),
 FLockReason( PVC_STATUS_SUBSYS_ERR ),
 FLinkSubSysStatus( FALSE ),
 FStopHandShake( FALSE ),
 FIsTWSE( IsTWSE ),
 FLintener( NULL ),
 FLogManager( NULL )
{
    FPVCID.PadThis( 2,' ' );
    
    FConnection = new SocketConnection();
    FIsTCP = TRUE;
    snprintf( FLinkPVCSendStr, sizeof(FLinkPVCSendStr), "Port%05d PVC:%s Send:",FLocalPort, FPVCID.c_str() );
    snprintf( FLinkPVCRecvStr, sizeof(FLinkPVCRecvStr), "Port%05d PVC:%s Recv:",FLocalPort, FPVCID.c_str() );
    FBuffer     = new char[ TWSE_MAX_MESSAGE_SIZE ];
    FLogBuffer  = new char[ LOG_BUFFER_SIZE ];   
}
// -------------------------------------------------------------------
//  Speedy Code                                      TWSE APCode    OTC APCode
//
//  '0':  Normal order.                                '0'
//  '1':  File Transfer.                               '1' 
//  '2':  File Transfer.                               '1'  
//  '3':  Execution report                             '3'      
//  '4':  Odd order                                    '2' 
//  '5':  Lend order                                   '4' 
//  '6':  TSE:Auction Order OTC: Negotiated price      '5'         '6'    
//  '7':  TenderEx order                               'B'
//  '8':  Tender order                                 '6'         '8'      
//  '9':  Post trade Order.                            '7'
//  'C':  Odd order.                                   'C'  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
char LinkSubSystemBase::ToExchangeAPCode( char SpeedyAPCode )
{
    switch( SpeedyAPCode )
    {
        case '0':  return '0'; ///< normal order
        case '1':
        case '2':  if( FIsTCP == TRUE && MessageHeader::FUseExtFT == TRUE )
                       return '9'; ///< Use File Transfer Ex protocol.
                   else
                       return '1'; ///< Use File Transfer protocol.
        case '3':  return '3'; ///< Execution report
        case '4':  return '2'; ///< Odd order
        case '5':  return '4'; ///< Lend order
        case '6':  if( FIsTWSE == TRUE ) ///< Auction Order
                       return '5';
                   else            ///< Negotiated price
                       return '6';
        case '7':  return 'B'; ///< TenderEx order
        case '8':  if( FIsTWSE == TRUE ) ///<Tender order
                       return '6';
                   else
                       return '8';
        case '9':  return '7';///< Post trade Order.
        case 'C':  return 'C';///< Odd Order.
        default:   return SpeedyAPCode;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::SetConnectionState( BOOL IsConnected ,int PVCState )
{
    FLinkSubSysStatus = IsConnected;
    SetStatus( PVCState );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::SetConnectionState( int PVCState )
{
    if( PVCState == PVC_STATUS_IDLE )
    {
        if( FLinkSubSysStatus == TRUE )
            SetStatus( PVC_STATUS_ONLINE_READY );
        else
            SetStatus( PVC_STATUS_OFFLINE_READY );
    }
    else if( PVCState == PVC_STATUS_BUSY )
    {
	if( FLinkSubSysStatus == TRUE )
	    SetStatus( PVC_STATUS_ONLINE_BUSY );
	else
	    SetStatus( PVC_STATUS_OFFLINE_BUSY );
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::Open( ConnectionParameter& Params )
{
    FLine      = Params.GetLink();
    FLocalPort = Params.GetLocalPort();
    FPVCNumber = Params.GetPVC();
    FBrokerID  = Params.GetBrokerID();
    FPassword  = Params.GetPassword();
    FAPCode    = Params.GetAPCode();
    FPVCID     = Params.GetPVCID();
    FPVCID.PadThis( 2, ' ' );
    FStatus           = PVC_STATUS_BROKEN;
    FLinkSubSysStatus = FALSE;
    FStopHandShake    = FALSE;
    snprintf( FLinkPVCSendStr, sizeof(FLinkPVCSendStr), "Port%05d PVC:%s Send:", FLocalPort, FPVCID.c_str() );
    snprintf( FLinkPVCRecvStr, sizeof(FLinkPVCRecvStr), "Port%05d PVC:%s Recv:", FLocalPort, FPVCID.c_str() );
    if( FSocketFD == -1 )
        FConnection->Open( FLine, FPVCNumber, (char*)FAddress.c_str(), FLocalPort );
    else
        FConnection->Open( FSocketFD );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::Open( void  )
{
    FStatus           = PVC_STATUS_BROKEN;
    FLinkSubSysStatus = FALSE;
    FStopHandShake    = FALSE;
    if( FSocketFD == -1 )
        FConnection->Open( FLine, FPVCNumber, (char*)FAddress.c_str(), FLocalPort );
    else
        FConnection->Open( FSocketFD );
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::Close( void )
{
    FStatus           = PVC_STATUS_BROKEN;
    FLinkSubSysStatus = FALSE;
    FStopHandShake    = FALSE;
    FConnection->Close();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LinkSubSystemBase::~LinkSubSystemBase()
{
    FConnection->Close();
    SetStatus( PVC_STATUS_UNKNOWN );
    delete [] FLogBuffer;
    delete [] FBuffer;
    delete FConnection;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemBase::WriteLog( BOOL IsSend, char *Msg, int Length )
{
    if( FLogManager != NULL )
    {        
        UFC::GetTimeString_ms_us( FLogBuffer, UFC::BufferedLog::IsEnableLog_us(), TRUE );            
        if( IsSend == TRUE )
            strcat( FLogBuffer, FLinkPVCSendStr );
        else
            strcat( FLogBuffer, FLinkPVCRecvStr );
        strncat( FLogBuffer, Msg, Length );
        FLogManager->WriteString( FLogBuffer );
        if( FLintener != NULL )
        {
            if( IsSend == TRUE )
                FLintener->OnCommand( EVENT_PVC_WRITE, (void*)FLogBuffer);
            else
                FLintener->OnCommand( EVENT_PVC_READ, (void*)FLogBuffer);
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemBase::SendMessage( MessageHeader* Message, long Timeout, BOOL ThrowReconnectException )
{
    int MessageLen;

    SetConnectionState( PVC_STATUS_BUSY );
    Message->AsString( FBuffer );
    MessageLen = (int)strlen( FBuffer );
    int Result = FConnection->Send( FBuffer, MessageLen, Timeout );
    if( Result >= PVC_OK )	 ///< Send X.25 message succeed.
    {
        SetConnectionState( PVC_STATUS_IDLE );
        WriteLog( TRUE, FBuffer, MessageLen );
        return TRUE;
    }
    else		     ///< X.25 timeout or disconnect
    {
        if( ThrowReconnectException == TRUE )
        {
            if( Result == PVC_RESET )
                throw( X25ReconnectException( "Receive PVC Reset request" ) );
            else
                throw( X25ReconnectException( "Send data timeout" ) );
        }
        return FALSE;
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* LinkSubSystemBase::ReceiveMessage( long TimeOut, BOOL ThrowReconnectException )
{
    MessageHeader* Msg;
    int ReceiveTime = ThreadClock::SecondOfToday();
    int SelectTime  = TimeOut;
    int Result;

    SetConnectionState( PVC_STATUS_BUSY );
    while( TRUE )
    {
        memset( FBuffer, 0, TWSE_MAX_MESSAGE_SIZE );
        Result = FConnection->Receive( FBuffer, TWSE_MAX_MESSAGE_SIZE, SelectTime );
        if( Result >= PVC_OK ) ///< Receive a normal message
        {
            WriteLog( FALSE, FBuffer, Result );
            if( (Msg = MessageHeader::CreateMessage( FBuffer, Result )) != NULL )
            {                    
                if( Msg->IsMessage( mtL010 ) == TRUE )
                {
                    if( Msg->GetStatusCode() != 0 ) ///< L010 with error message.
                        throw( X25RehandshakeException( "Received a L010 with error code", Msg->GetStatusCode() ) );                        
                    else if( ThrowReconnectException == TRUE )
                        throw( X25RehandshakeException( "Received a L010" ) );
                }
                SetConnectionState( PVC_STATUS_IDLE );
                return Msg;
            }
            else
            {
                UFC::BufferedLog::Printf( " ***************** Received Unknown Message *************");
                UFC::BufferedLog::Printf( " Data: %s\n", FBuffer);
                UFC::BufferedLog::Printf( " Receive length: %d\n", Result);
                UFC::BufferedLog::Printf( " Length: %d\n",strlen(FBuffer) );
                UFC::BufferedLog::Printf( " ********************************************************");
                throw( X25ReconnectException("Received Unknown Message.") );
            }
        }
        else if( Result <= PVC_COMMAND ) ///< Is a control command.
        {
            if( ThrowReconnectException == FALSE )
            {
                SelectTime = (int)TimeOut - (ThreadClock::SecondOfToday()-ReceiveTime);
                if( SelectTime <= 0 ) ///< Reach the timeout ?
                    return NULL; ///< Time out
            }
            else
                throw( X25ReconnectException( "Receive PVC Reset request" ) );
        }
        else
        {
            if( ThrowReconnectException == TRUE )
                throw( X25ReconnectException( "Receive data timeout" ) );
            return NULL; ///< Time out without exception.
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Add this function on 2004/08/14 by Simon
//
//  Ignore other message during the given timeout period
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
MessageHeader* LinkSubSystemBase::ReceiveMessage( TMessageType RequireType, int Timeout )
{
    int BeginTime = ThreadClock::SecondOfToday();
    int WaitTime  = Timeout;
    TMessageType   Type;
    MessageHeader* Msg;

    while( WaitTime > 0 )
    {
        if( ( Msg = ReceiveMessage( WaitTime, FALSE )) != NULL )///< Receive the wake up message replay from the FCM server.
        {
            Type = Msg->MessageType( );
            if( RequireType == Type )
                return Msg;
            else
                delete Msg;
        }
        WaitTime -= ( ThreadClock::SecondOfToday() - BeginTime );
    }
    return NULL;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Implement the link sub-system TWSE side handshake protocol.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LinkSubSystemFCM::LinkSubSystemFCM( BOOL IsTWSE, int Line, int PVCNumber, UFC::AnsiString Password, UFC::AnsiString BrokerID,char APCode )
:LinkSubSystemBase( IsTWSE, Line, PVCNumber, (char*)Password.c_str(), (char*)BrokerID.c_str(), APCode )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::OnlineHandshake(  BOOL ReceivedL010 )
{
    BOOL  SendL010 = ReceivedL010;

    while( TRUE )
    {
        SetConnectionState( FALSE, PVC_STATUS_BROKEN );
        if( FStopHandShake == TRUE )   ///< Force stop handshake.
        {
            FStopHandShake = FALSE;
            return FALSE;
        }
        try
        {
            if( SendL010 == TRUE )
            {
                MessageL010 L010( FIsTWSE );
                SendL010 = FALSE;
                SendMessage( &L010, TWSE_INTERNAL_TIMEOUT, FALSE );
            }
            if( WakeUpHandshake() == TRUE ) 	  ///< FCM Server wakeup handshake.(Send L010 and recv L010)
            {
                if( LoginHandshake() == TRUE )	  ///< FCM Server login handshake.(Send L020 and recv L030)
                {
                    if( SubsystemStartupHandshake() == TRUE )    ///< Subsystem startup handshake.(Send L050 and recv L050 and Send L060)
                    {
                        SetConnectionState( TRUE, PVC_STATUS_ONLINE_READY ); ///< Link SubSystem online.
                        return TRUE;				     ///< Subsystem startup succeed.
                    }
                }
            }
        }
        catch( X25RehandshakeException&  )
        {
            SendL010 = TRUE; ///< Received X25RehandshakeException message, Try to rehandshake.
        }
        catch( X25ReconnectException& )
        {
            SendL010 = TRUE; ///< Received X25RehandshakeException message, Try to rehandshake.
        }
    };

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::WakeUpHandshake( )
{
    MessageL010    L010( FIsTWSE );
    MessageHeader* Msg;

    if( SendMessage( &L010, TWSE_INTERNAL_TIMEOUT, FALSE ) == FALSE ) ///< Send wake up message L010
        return FALSE;
    if( FStopHandShake == TRUE )
        return FALSE;	
    if( (Msg = ReceiveMessage( mtL010, TWSE_INTERNAL_TIMEOUT ))!=NULL) ///< Receive the wake up message replay from the TAIFEX server.
    {
        delete Msg;
        return TRUE;
    }
    return FALSE; ///< timeout!
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::LoginHandshake( )
{
    MessageL020 L020( FIsTWSE );		      ///< on line subsystem,power on process,power on confirm message.

    if( SendMessage( &L020, TWSE_INTERNAL_TIMEOUT, FALSE ) == FALSE )	   ///< Send confirm wakeup message
        return FALSE;
    if( FStopHandShake == TRUE )
        return FALSE;
    MessageHeader* Msg = ReceiveMessage( TWSE_LINKSUBSYSTEM_TIMEOUT, FALSE );
    if( Msg != NULL && Msg->IsMessage( mtL030 ) == TRUE )
    {
        FAppendNo = (static_cast<MessageL030*>(Msg))->GetAppendNo(); ///< get the Append No
        return TRUE;
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemFCM::SubsystemStartupHandshake( )
{
    char APCode = ToExchangeAPCode( FAPCode ); ///< Convert Speedy AP code to Exchange AP code (TSC,OTC)
    MessageL040 L040( FIsTWSE, (char*)FAppendNo.c_str(), (char*)FBrokerID.c_str(), APCode, (char*)FPassword.c_str() );

    UFC::BufferedLog::Printf( " Speedy APCode:%c Exchange APCode:%c",FAPCode,APCode );        
    if( SendMessage( &L040, TWSE_LINKSUBSYSTEM_TIMEOUT ) == FALSE )	    ///< Replay login message L040
        return FALSE;
    if( FStopHandShake == TRUE )
        return FALSE;
    MessageHeader* Msg = ReceiveMessage( TWSE_LINKSUBSYSTEM_TIMEOUT, FALSE );
    if( Msg != NULL && Msg->IsMessage( mtL050 ) == TRUE )		///< Receive subsystem startup message
    {
        MessageL060 L060( FIsTWSE );									///< On line subsystem,login process,AP confirm message.
        return SendMessage( &L060, TWSE_INTERNAL_TIMEOUT, FALSE );	///< Replay subsystem startup confirm message
    }
    return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//
//  Implement the link sub-system exchange side handshake protocol.
//
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LinkSubSystemTAIFEX::LinkSubSystemTAIFEX( BOOL IsTWSE, int Line, int PVCNumber, char* Password, char* BrokerID,char APCode )
:LinkSubSystemBase( IsTWSE, Line, PVCNumber, Password, BrokerID, APCode )
{
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemTAIFEX::OnlineHandshake(  BOOL ReceivedL010 )
{
	int   ErrorCode;
	int   LoginFailedCount = 0;
	BOOL  SendL010 = ReceivedL010;

	while( TRUE )
	{
		SetConnectionState( FALSE, PVC_STATUS_BROKEN ); ///< Link SubSystem offline.
		if( FStopHandShake == TRUE ) ///< Force stop handshake.
                    return FALSE;
        	try
		{
			if( SendL010 == TRUE )
			{
                            MessageL010 L010( FIsTWSE );
                            SendL010 = FALSE;
                            SendMessage( &L010, TWSE_PULL_INTERVAL, FALSE );
			}
			if( WakeUpHandshake( ) == TRUE )	  ///< FCM Server wakeup handshake.(S:L010,R:L020)
			{
                            if( LoginHandshake() == TRUE )				  ///< FCM Server login handshake.(S:L030)
                            {
				while( LoginFailedCount < PVC_LOCK_TIMES )
				{
                                    if( (ErrorCode = SubsystemStartupHandshake()) == 0 )///< Subsystem startup handshake.(R:L040,S:L050)
                                    {
					ConfirmSubSysStartup( );
					LoginFailedCount = 0;
					return TRUE;
                                    }
                                    else
                                    {
					LoginFailedCount++;
					if( ErrorCode != -1 )
                                            LoginHandshake( ErrorCode );  ///< Resent L030 with errorcode
					else
                                            throw( X25RehandshakeException( "Not a L040 message"));
                                    }
				}
				SetConnectionState( FALSE, FLockReason ); ///< Link SubSystem offline.
				UFC::BufferedLog::Printf(" Authentication fail! PVC %s locked. \n",FPVCID.c_str() );
				return FALSE;
                            }
			}
		}
		catch( X25RehandshakeException&  )
		{
			SendL010 = TRUE; ///< Received X25RehandshakeException message, Try to rehandshake.
		}
		catch( X25ReconnectException& )
		{
			SendL010 = TRUE; ///< Received X25RehandshakeException message, Try to rehandshake.
		}
	};
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Function WakeUpHandshake:
//  Handle the WakeUp Handshake,Try to receive the message L020..if not reply the message L010.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemTAIFEX::WakeUpHandshake( void ) ///< Handle L010 and reply L020
{
	MessageL010    L010( FIsTWSE );
	MessageHeader* Msg;

	if( FStopHandShake == TRUE ) ///< User force stop thehandshake.
            return FALSE;
	// Modify on 2004/08/14 by Simon.
	if( (Msg = ReceiveMessage( mtL020, 10 )) != NULL )
	{
            delete Msg;
            return TRUE;  ///< OK! Is a L020, goto the next handshake.
	}
	else
	{
            SendMessage( &L010, TWSE_PULL_INTERVAL, FALSE );
            return FALSE;///< Time out, Send a L010 back!!
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Function LoginHandshake:
//  Handle the Login Handshake.
//  AppendNo: Random generated. between 1~999.
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
BOOL LinkSubSystemTAIFEX::LoginHandshake( int ErrorCode ) ///< Send L030
{
	UInt32 AppendNo = (rand()+1)%1000;
	UInt32 KeyValue = ((AppendNo * FPassword.ToInt())/100)%100; ///< Need thousand & hundred digi.

	snprintf( FKeyValueStr, 3,  "%02d", KeyValue );
	snprintf( FAppendNo, 4, "%03d", AppendNo );
	MessageL030 l030( FIsTWSE, AppendNo );

	l030.SetStatusCode( ErrorCode );
	if( SendMessage( &l030, TWSE_PULL_INTERVAL, FALSE ) == TRUE )///< Send Register notice message
	{
            if( FStopHandShake == TRUE )
                return FALSE;
            return TRUE;
	}
	UFC::BufferedLog::Printf( " Link Sub-System: Login handshake failed! ");
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Function SubsystemStartupHandshake :
//  Subsystem Startup Handshake, receive a register message (L040)
//  and reply a subsystem startup message.(L050)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int LinkSubSystemTAIFEX::SubsystemStartupHandshake( ) ///< Receive L040 and Send L050
{
    MessageHeader* Msg;
    MessageL050    L050( FIsTWSE );
    int		   ErrCode;

    ///< Modify on 2004/09/14 by Simon. Ignore other message durning 10 sec.
    if( (Msg = ReceiveMessage( mtL040, 10 )) != NULL )
    {
        if( (ErrCode = Authentication( static_cast< MessageL040* >( Msg ))) != 0 )
        {
            SetConnectionState( FALSE, FLockReason ); ///< Link SubSystem offline.
            return ErrCode; 						  ///< Authentication failed.
        }
        else if( SendMessage( &L050, TWSE_PULL_INTERVAL, FALSE ) == TRUE )
        {
            return 0;								  ///< Startup handshake succeed!
        }
    }
    return -1;  ///< Time out, No L040 incoming during this 10 sec.
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Function ConfirmSubSysStartup :
//  Try to receive the Confirm subsystem online message.(L060)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void LinkSubSystemTAIFEX::ConfirmSubSysStartup( void )
{
    MessageHeader* L060Msg = ReceiveMessage( TWSE_LINKSUBSYSTEM_TIMEOUT, TRUE );///< (R:L060)
    ///< if L060Msg ==NULL, the ReceiveMessage function will throw exception.

    TMessageType Type = L060Msg->MessageType( );
    delete L060Msg;
    if( Type == mtL060 )///< Is a L060! Subsystem startup succeed.
    {
	SetConnectionState( TRUE, PVC_STATUS_ONLINE_READY ); ///< Link SubSystem online!!!(Only entry to Online the subsys.)
    }
    else
    {
	throw( X25RehandshakeException( "Not a L060 message"));
    }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Function Authentication :
//  Check the AppenNo,Broker ID, APCode and the random generate KeyValue.( Between 1~999 )
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int LinkSubSystemTAIFEX::Authentication( MessageL040* L040 )
{
    if( strcmp( L040->GetAppendNo().c_str(),FAppendNo ) != 0  ) ///< Compare Append No.
    {
        UFC::BufferedLog::Printf(" Link Sub-System: L040 Append number error.");
        FLockReason = PVC_STATUS_PW_ERR;
        return 1; ///< Append No error.
    }
    if( L040->GetBrokerID() != FBrokerID ) ///< Compare BrokerID
    {
        UFC::BufferedLog::Printf(" Link Sub-System: L040 Broker ID error Broker:%s.",FBrokerID.c_str());
        FLockReason = PVC_STATUS_FCM_ERR;
        return 2; ///< Broker ID error.
    }
    char APCode = L040->GetAPCode();
    
    if( APCode != '4' && APCode != '5' && APCode != '6' && APCode != '7' && APCode != '8' && APCode != 'B' &&///< Not Auction,Lend,Tender subsystem
        APCode != ToExchangeAPCode( FAPCode ) ) ///< and AP code not equal.
    {    
        UFC::BufferedLog::Printf(" Link Sub-System: L040 AP Code error.");
        FLockReason = PVC_STATUS_AP_ERR;
        return 3; ///< AP Code error.
    }
    if( strcmp( L040->GetKeyValue().c_str(), FKeyValueStr ) != 0 ) ///< Compare Key value
    {
        UFC::BufferedLog::Printf(" Link Sub-System: L040 Key value error PW = %s.",FPassword.c_str());
        FLockReason = PVC_STATUS_PW_ERR;
        return 4; ///< Key value error.(Password error)
    }
    return 0;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
