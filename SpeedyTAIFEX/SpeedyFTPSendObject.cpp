//---------------------------------------------------------------------------
// SpeedyFTPSendObject.cpp
//---------------------------------------------------------------------------
#include "SpeedyFTPSendObject.h"
#include "SpeedyObjectsManager.h"
//---------------------------------------------------------------------------
TSpeedyFTPSendObject::TSpeedyFTPSendObject( SpeedyObjectsManager* ObjManager,
                                            const UFC::AnsiString& SourceID,
                                            const UFC::AnsiString& DestinationID,
                                            ConnectionParameter&   info,
                                            UFC::ThreadListener*   TListener,
                                            ConnectionListener*    CListener,
                                            BOOL IsOpt)
:TSpeedyFTPConnectionObject( ObjManager, info, TListener, CListener, IsOpt )
{
    FSourceID = SourceID;
    FDestinationID = DestinationID;
}
//---------------------------------------------------------------------------
void TSpeedyFTPSendObject::CustomMessageLoopFunction( void )
{
    if( TSpeedyFTPConnectionObject::ReceiveMessage( 5, mtL010 ) )
        throw( X25RehandshakeException( "receive unknown message" ) );

    ViewJobList();
    if( FIdleTime >= IDLEHANDSHAKE_TIME_XBIO3 )
        OnIdle();
}
//---------------------------------------------------------------------------
void TSpeedyFTPSendObject::OnIdle( void )
{
    FIdleTime = 0;
}
//---------------------------------------------------------------------------
void TSpeedyFTPSendObject::AddNewJob( TFtpJobObject* pJob )
{
    TFtpJobObject* pExistsJob = NULL;
    BOOL CreateNewJob = TRUE;
    int StatusCode = -1;

    FCS.Enter();
    for( int i = FJobs.ItemCount() - 1; i >= 0; i-- )
    {
        pExistsJob = ( TFtpJobObject* )( FJobs.GetItem( i ) );
        if( pExistsJob->GetFileCode() == pJob->GetFileCode() )
        {
            CreateNewJob = FALSE;
            break;
        }
    }
    if( CreateNewJob )
        FJobs.Add( (void*)pJob );
    else
        StatusCode = FTP_JOB_EXISTS;
    FCS.Leave();
    ReportStatusCode( pJob->GetFileCode(), StatusCode );
}
//------------------------------------------------------------------------------
void TSpeedyFTPSendObject::ViewJobList( void )
{
    TFtpJobObject* pJob = NULL;
    FIdleTime++;

    FCS.Enter();
    if ( FJobs.ItemCount() > 0 )
    {
        pJob = ( TFtpJobObject* )( FJobs.GetItem( 0 ) );
        FJobs.Delete( 0 );
    }
    FCS.Leave();

    if( pJob )
    {
        FIdleTime = 0;
        Work( pJob );
        delete pJob;
    }
}
//------------------------------------------------------------------------------
BOOL TSpeedyFTPSendObject::Work( TFtpJobObject* pJob )
{
    return SendF050( pJob );
}
//------------------------------------------------------------------------------
//  Send FT Initialize message
//------------------------------------------------------------------------------
BOOL TSpeedyFTPSendObject::SendF010( TFtpJobObject* pJob )
{
    MessageF010 F010( (char*)(FSourceID.c_str()), (char*)(FDestinationID.c_str()), (char*)(pJob->GetFileCode().c_str()), 0 );
    if( ! TSpeedyFTPConnectionObject::SendMessage( &F010 ) )
        throw( X25RehandshakeException( "exception: send F010 message." ) );

    MessageHeader* Msg = NULL;

    if( (Msg = TSpeedyFTPConnectionObject::ReceiveMessage( 60, mtF020 )) == NULL )
        throw( X25RehandshakeException( "exception: receive F020 message." ) );

    MessageF020* F020 = static_cast < MessageF020* > ( Msg );
    int StatusCode = F020->GetStatusCode();
    UFC::AnsiString FileCode = F020->GetFileCode();
    delete F020;

    ReportStatusCode( pJob->GetFileCode(), StatusCode );
    if( StatusCode == 0 )
    {
        if( FileCode == pJob->GetFileCode() )
            return TRUE;
        else
            ReportStatusCode( pJob->GetFileCode(), FTP_ILLEGAL_FILE_CODE );
    }
    return FALSE;
}
//------------------------------------------------------------------------------
// Send FT request message
//------------------------------------------------------------------------------
BOOL TSpeedyFTPSendObject::SendF050( TFtpJobObject* pJob )
{
    int            MessageLength = pJob->GetData().Length();    
    MessageHeader* Msg = NULL;
    MessageF050    F050( (char*)(FSourceID.c_str()), (char*)(FDestinationID.c_str()), pJob->GetData() );
    

    ///< Send F050
    if( ! TSpeedyFTPConnectionObject::SendMessage( &F050 ) )
        throw( X25RehandshakeException( "exception: send F050 message." ) );
    ///<Receive F060
    
    if( (Msg = TSpeedyFTPConnectionObject::ReceiveMessage( 60, mtF060 )) == NULL )
        throw( X25RehandshakeException( "exception: receive F060 message." ) );
    MessageF060* F060 = static_cast < MessageF060 * > ( Msg );
    int StatusCode = F060->GetStatusCode();
    int F060MessageLength = F060->GetMessageLength();
    UFC::AnsiString FileCode = F060->GetFileCode();
    delete F060;

    ReportStatusCode( pJob->GetFileCode(), StatusCode );
    if( StatusCode == 0 )
    {
        if( F060MessageLength == MessageLength )
        {
            if( FileCode == pJob->GetFileCode() )
                return TRUE;
            else
                ReportStatusCode( pJob->GetFileCode(), FTP_ILLEGAL_FILE_CODE );
        }
        else
        {
            ReportStatusCode( pJob->GetFileCode(), FTP_ILLEGAL_BODY_LENGTH );
        }
    }
    return FALSE;
}
//------------------------------------------------------------------------------
// Send End message
//------------------------------------------------------------------------------
void TSpeedyFTPSendObject::SendF070( void )
{
    MessageF070    F070( (char*)(FSourceID.c_str()), (char*)(FDestinationID.c_str()) );
    MessageHeader* Msg = NULL;

    if( ! TSpeedyFTPConnectionObject::SendMessage( &F070 ) )
        throw( X25RehandshakeException( "exception: send F070 message." ) );
    if( (Msg = TSpeedyFTPConnectionObject::ReceiveMessage( 60, mtF080 )) == NULL )
        throw( X25RehandshakeException( "exception: receive F080 message." ) );
    delete Msg;
}
//------------------------------------------------------------------------------
