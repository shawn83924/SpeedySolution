#include "MessageDispatcher.h"
#include "iniFile.h"
//------------------------------------------------------------------------------
const UFC::AnsiString CONFIG_FILE( "../cfg/SpeedyQueueRiskManager.cfg" );
const UFC::AnsiString RM_NAME( "SpeedyQueueRiskManager" );
const UFC::AnsiString FUT_SUBJECT  = RM_NAME + ".FUT";
const UFC::AnsiString OPT_SUBJECT  = RM_NAME + ".OPT";
const UFC::AnsiString TSE_SUBJECT  = RM_NAME + ".TSE";
const UFC::AnsiString OTC_SUBJECT  = RM_NAME + ".OTC";
const UFC::AnsiString PATS_SUBJECT = RM_NAME + ".PATS";
const UFC::AnsiString ES_SUBJECT   = RM_NAME + ".ES";
//------------------------------------------------------------------------------
MessageDispatcher::MessageDispatcher( void )
{      
    FMessageObject = new MessageObject( RM_NAME , "1.0.0", "Speedy Queue Connection for RM module" );
    ///< For Futures/Options
    FMessageObject->AddListener( FUT_SUBJECT, UFC::Hostname, this );
    FMessageObject->AddListener( OPT_SUBJECT, UFC::Hostname, this );    
    ///< For TSEC/OTC
    FMessageObject->AddListener( TSE_SUBJECT, UFC::Hostname, this );
    FMessageObject->AddListener( OTC_SUBJECT, UFC::Hostname, this );
    ///< For PATS
    FMessageObject->AddListener( PATS_SUBJECT, UFC::Hostname, this );
    ///< For ES
    FMessageObject->AddListener( ES_SUBJECT, UFC::Hostname, this );
    
    LoadConfig();
    
    FMessageObject->Start();
    FMessageObject->WaitForConnected();
}
//------------------------------------------------------------------------------
void MessageDispatcher::LoadConfig( void )
{
    ///< Load Default Queue setting.
    UFC::UiniFile   Config( CONFIG_FILE );
    UFC::AnsiString SendQueueID, RecvQueueID;

    ///< for TAIFEX 
    if( Config.GetValue( "Setting", "SendQueueID", SendQueueID ) == FALSE )
    {
        UFC::BufferedLog::Printf( " SendQueueID not set in [Setting]." );
        exit( 0 );
    }
    else
        UFC::BufferedLog::Printf( " TAIFEX Send Queue:[%s]", SendQueueID.c_str() );    
    if( Config.GetValue( "Setting", "RecvQueueID", RecvQueueID ) == FALSE )
    {
        UFC::BufferedLog::Printf( " RecvQueueID not set in [Setting]." );
        exit( 0 );
    }
    else
        UFC::BufferedLog::Printf( " TAIFEX Recv Queue:[%s]", RecvQueueID.c_str() );
    FTAIFEXConnection = new SpeedyQueueConnection( SendQueueID, RecvQueueID, FMessageObject, eTAIFEX  );
    ///< for TWSE/OTC 
    if( Config.GetValue( "Setting", "TSESendQueueID", SendQueueID ) == FALSE )
    {
        UFC::BufferedLog::Printf( " TSESendQueueID not set in [Setting]." );
        exit( 0 );
    }
    else
        UFC::BufferedLog::Printf( " TSE Send Queue:[%s]", SendQueueID.c_str() );
    if( Config.GetValue( "Setting", "TSERecvQueueID", RecvQueueID ) == FALSE )
    {
        UFC::BufferedLog::Printf( " TSERecvQueueID not set in [Setting]." );
        exit( 0 );
    }
    else
        UFC::BufferedLog::Printf( " TSE Recv Queue:[%s]", RecvQueueID.c_str() );
    FTSEOTCConnection = new SpeedyQueueConnection( SendQueueID, RecvQueueID, FMessageObject, eTWSEOTC  );
    ///< for PATS
    if( Config.GetValue( "Setting", "PATSSendQueueID", SendQueueID ) == FALSE )
    {
        UFC::BufferedLog::Printf( " PATSSendQueueID not set in [Setting]." );
        exit( 0 );
    }
    else
        UFC::BufferedLog::Printf( " PATS Send Queue:[%s]", SendQueueID.c_str() );
    if( Config.GetValue( "Setting", "PATSRecvQueueID", RecvQueueID ) == FALSE )
    {
        UFC::BufferedLog::Printf( " PATSRecvQueueID not set in [Setting]." );
        exit( 0 );
    }
    else
        UFC::BufferedLog::Printf( " PATS Recv Queue:[%s]", RecvQueueID.c_str() );
    FPATSConnection = new SpeedyQueueConnection( SendQueueID, RecvQueueID, FMessageObject, ePATS  );
}
//------------------------------------------------------------------------------
//
// RiskModule <- Queue <- SpeedyQueueRiskManager <- SpeedyGateway
//
// Receive Order from MBus
// Convert to Queue RM Fix-length format.
//------------------------------------------------------------------------------
// 0-74    TAIFEX/TWSE Order.
// 75      Market. TAIFEX: F/O TWSE: T/O
// 76-85   NID
// 85-355  User define data.
// 356-364 Order time
// 365     End of message.
//------------------------------------------------------------------------------
void MessageDispatcher::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
{
    UFC::AnsiString OrderTime, UserData, Order, SendMsg;
    Int32 NID;
        
    UFC::GetTimeString( OrderTime, FALSE );        
    OrderTime.PadThis( 9, '0' ); ///< HHMMSSmmm    
    Data->get( "ORDER", Order ); ///< R010 Length 75 
    Data->get( "NID",   NID );     ///< Integer 10
    Data->get( "KEY",   UserData );
    
    UFC::BufferedLog::DebugPrintf( " Receive order from SpeedyGateway " );
    UFC::BufferedLog::DebugPrintf( " Order [%s]", Order.c_str() );
    UFC::BufferedLog::DebugPrintf( " NID   [%d]", NID );    
    UFC::BufferedLog::DebugPrintf( " Key   [%s]", UserData.c_str() );
    UserData.PadThis( 270, ' ' );       
    
    if( Subject == FUT_SUBJECT )
    {
        SendMsg.Printf( "%s%c%010d%s%s%c", Order.c_str(), 'F', NID, UserData.c_str(), OrderTime.c_str(), 0x0a );
        UFC::BufferedLog::Printf( " Send Futures Queue:[%s]", SendMsg.c_str() );
        FTAIFEXConnection->OnMessage( SendMsg, mFutures, Data );
    }
    else if( Subject == OPT_SUBJECT )
    {
        SendMsg.Printf( "%s%c%010d%s%s%c", Order.c_str(), 'O', NID, UserData.c_str(), OrderTime.c_str(), 0x0a );
        UFC::BufferedLog::Printf( " Send Options Queue:[%s]", SendMsg.c_str() );
        FTAIFEXConnection->OnMessage( SendMsg, mOptions, Data );
    }
    else if( Subject == TSE_SUBJECT )
    {
        Order.PadThis( 75, ' ' );
        SendMsg.Printf( "%s%c%010d%s%s%c", Order.c_str(), 'T', NID, UserData.c_str(), OrderTime.c_str(), 0x0a );
        UFC::BufferedLog::Printf( " Send TSE Queue:[%s]", SendMsg.c_str() );
        FTSEOTCConnection->OnMessage( SendMsg, mTSE, Data );
    }
    else if( Subject == OTC_SUBJECT )
    {
        Order.PadThis( 75, ' ' );
        SendMsg.Printf( "%s%c%010d%s%s%c", Order.c_str(), 'O', NID, UserData.c_str(), OrderTime.c_str(), 0x0a );
        UFC::BufferedLog::Printf( " Send OTC Queue:[%s]", SendMsg.c_str() );
        FTSEOTCConnection->OnMessage( SendMsg, mOTC, Data );
    }
    else if( Subject == PATS_SUBJECT )
    {
        Order.PadThis( 256, ' ' );
        SendMsg.Printf( "%s%c%010d%s%s%c", Order.c_str(), 'P', NID, UserData.c_str(), OrderTime.c_str(), 0x0a );
        UFC::BufferedLog::Printf( " Send PATS Queue:[%s]", SendMsg.c_str() );
        FPATSConnection->OnMessage( SendMsg, mPATS, Data );
    }
    else if( Subject == ES_SUBJECT )
    {
        Order.PadThis( 75, ' ' );
        SendMsg.Printf( "%s%c%010d%s%s%c", Order.c_str(), 'E', NID, UserData.c_str(), OrderTime.c_str(), 0x0a );
        UFC::BufferedLog::Printf( " Send TSE Queue:[%s]", SendMsg.c_str() );
        FTSEOTCConnection->OnMessage( SendMsg, mTSE, Data );        
    }
    else
    {
        UFC::BufferedLog::Printf( " Receive data from unknown subject:[%s]", Subject.c_str() );
        return;
    }
}
//------------------------------------------------------------------------------





