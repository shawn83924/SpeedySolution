//---------------------------------------------------------------------------
// SpeedyFTPRecvObject.cpp
//---------------------------------------------------------------------------
#include "SpeedyFTPRecvObject.h"
#include "../UFC/iniFile.h"
//---------------------------------------------------------------------------
using namespace UFC;
//---------------------------------------------------------------------------
void ResendExecutionThread::Execute( void )
{
    PQueue     FillQueue;
    AnsiString FilePath;
    AnsiString FTPathName  = "TSEFilePath";
    long       Key = 0x74E10000;
    UiniFile   Config( "../cfg/FTRecordLength.cfg" );

    if( Config.GetValue( "Settings", FTPathName,FilePath))
    {
        if( FilePath.LastChar() == '/' )
            FilePath += "R30";
        else
            FilePath += "/R30";
        if ( !FillQueue.Open( Key, true ) )
        {
            printf( "Open fill report queue failed.\n" );
            return;
        }
        try
        {
            char       Buffer[ 256 ];
            FileStream File( FilePath, O_RDONLY );
            int        Count = 0;

            File.Seek( 0, soFromBeginning );
            while( File.Read( Buffer, 127 ) == 127 )
            {
                 UFC::AnsiString Data( Buffer, 127 );
                 Data.TrimRight();
                 UQueueData OrderData( FMType, (char*) Data.c_str() );
                 FillQueue.Send( OrderData );
                 UFC::SleepMS( 30 );
                 Count++;
            }
            UFC::BufferedLog::Printf( " Resend %d executions from TSEC.",Count );
        }
        catch( exception &e )
        {
             UFC::BufferedLog::Printf( " %s", e.what());
             return;
        }
    }
}
//---------------------------------------------------------------------------
TSpeedyFTPRecvObject::TSpeedyFTPRecvObject( const UFC::AnsiString& SourceID, const UFC::AnsiString& DestinationID, ConnectionParameter& info, UFC::ThreadListener* TListener, ConnectionListener* CListener )
:TSpeedyFTPConnectionObject( info, TListener, CListener )
{
    FSourceID = SourceID;
    FDestinationID = DestinationID;
    FmType = ((SourceID[1] -'0')*256 +(SourceID[2] -'0')*16 + (SourceID[3] -'0')) << 12;
}
//---------------------------------------------------------------------------
void TSpeedyFTPRecvObject::CustomMessageLoopFunction( void )
{
    MessageHeader* InMsg = NULL;

    if( (InMsg = TSpeedyFTPConnectionObject::ReceiveMessage( 65 )) != NULL )
    {
        BOOL Result = FALSE;

        switch( InMsg->MessageType() )
        {
            case mtF010:  Result = Download( static_cast < MessageF010* > ( InMsg ) );
                          break;
            case mtF170:  Result = ReplyConfirmMessage( static_cast < FMessageHeader* > ( InMsg ) );
                          break;
            case mtL010:  delete InMsg;
                          throw( X25RehandshakeException( "Receive L010 Re-handshake." ) );
                          break;
            default:      UFC::BufferedLog::Printf( " msgtype:%d", InMsg->MessageType() );
                          break;
        }
        delete InMsg;
        if( ! Result )
            throw( X25RehandshakeException( "exception: check message." ) );
    }
    else
         throw( X25RehandshakeException( "Receive message timeout." ) );
}
//---------------------------------------------------------------------------
void TSpeedyFTPRecvObject::OnIdle( void )
{
}
//---------------------------------------------------------------------------
BOOL TSpeedyFTPRecvObject::ReplyConfirmMessage( FMessageHeader* FMsg )
{
    FMsg->SwapID();
    FMsg->SetMessageType( ( FMsg->GetMessageType() ) + 1 );

    return TSpeedyFTPConnectionObject::SendMessage( FMsg );// Send the confirm connection message
}
//----------------------------------------------------------------------------------------------------------------------
void TSpeedyFTPRecvObject::ToBackend( const UFC::AnsiString& FileCode )
{
    UFC::AnsiString Command;
    UFC::AnsiString Script,ScriptPath,FilePath,RecordLength;
    UFC::AnsiString SectionName = "TSE";
    UFC::AnsiString FTPathName  = "TSEFilePath";
    UFC::UiniFile   Config( "../cfg/FTRecordLength.cfg" );

    if( FileCode == "R30" )
    {
        UFC::BufferedLog::Printf( " Resend %s execution reports.",SectionName.c_str() );
        new ResendExecutionThread( FmType );
        return;
    }
    if( Config.GetValue( SectionName, FileCode, RecordLength ))
    {
        if( Config.GetValue( "Settings", "ScriptName", Script ) &&
            Config.GetValue( "Settings", "ScriptPath", ScriptPath )&&
            Config.GetValue( "Settings", FTPathName,FilePath))
        {
            Command.Printf("%s%s S %s %s %d &",ScriptPath.c_str(),Script.c_str(),FileCode.c_str(),FilePath.c_str(), RecordLength.ToInt() );
            UFC::BufferedLog::Printf( " Execute command:%s",Command.c_str());
            system( Command.c_str());
        }
        else
            UFC::BufferedLog::Printf( " Can not get the ScriptName,ScriptPath or the FilePath." );
    }
    else
        UFC::BufferedLog::Printf( " Not a validate File Code." );
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TSpeedyFTPRecvObject::Download( MessageF010* F010 )
{
    if( ! ReplyConfirmMessage( F010 ) )
        return FALSE;
    UFC::AnsiString   FileCode = F010->GetFileCode();
    UFC::FileStream*  FS = OpenFile( FileCode );
    MessageF030*      F030 = NULL;
    char              IsEOF = '0';

    if( FS != NULL )
    {
        try
        {
            while( TRUE )
            {
                if( IsEOF == '1' )
                {
                    ReportStatusCode( FileCode, FTP_DOWNLOAD_FINISHED );
                    ToBackend( FileCode ); ///< download finished.
                    break;
                }
                if( (F030 = ReceiveF030(FileCode)) == NULL )
                    break;
                FS->Write( F030->GetData(), F030->GetMessageLength() - 4 );
                IsEOF = F030->GetEOF();
                delete F030;
                F030 = NULL;
                if( !SendF040( FileCode, IsEOF ) )
                     break;
            }
            delete FS;
            FS = NULL;
            return TRUE;
        }
        catch(...)
        {
            if( FS != NULL )
                delete FS;
            if( F030 != NULL )
               delete F030;
        }
    }
    return FALSE;
}
//----------------------------------------------------------------------------------------------------------------------
FileStream* TSpeedyFTPRecvObject::OpenFile( const UFC::AnsiString& FileCode )
{
    UFC::FileStream* FS = NULL;

    try
    {
        FS = new UFC::FileStream( FFolderName + FileCode, O_RDWR|O_CREAT|O_TRUNC );
    }
    catch(...)
    {
        if( FS )
            delete FS;
        return NULL;
    }
    return FS;
}
//----------------------------------------------------------------------------------------------------------------------
MessageF030* TSpeedyFTPRecvObject::ReceiveF030( const UFC::AnsiString& FileCode )
{
    MessageHeader* Msg = NULL;

    if( (Msg = TSpeedyFTPConnectionObject::ReceiveMessage( LINKSUBSYSTEM_TIMEOUT, mtF030 )) == NULL )
        throw( X25RehandshakeException( "exception: receive F030 message." ) );

    MessageF030* F030 = static_cast < MessageF030* > ( Msg );
    if( F030->GetStatusCode() == 0 && F030->GetFileCode() == FileCode.c_str() )
    {
        return F030;
    }
    ReportStatusCode( FileCode, F030->GetStatusCode() );
    delete Msg;
    return NULL;
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TSpeedyFTPRecvObject::SendF040( const UFC::AnsiString& FileCode, char IsEOF )
{
    MessageF040 F040( (char*)(FSourceID.c_str()), (char*)(FDestinationID.c_str()), (char*)(FileCode.c_str()), IsEOF );

    return TSpeedyFTPConnectionObject::SendMessage( &F040 );
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TSpeedyFTPRecvObject::Finished( void )
{
    MessageHeader* Msg = NULL;
    BOOL Result = FALSE;

    if( (Msg = TSpeedyFTPConnectionObject::ReceiveMessage( LINKSUBSYSTEM_TIMEOUT,mtF070 )) != NULL )
    {
        MessageF070* F070 = static_cast < MessageF070* > ( Msg );

        if( F070->GetStatusCode() == 0 )
        {
            Result = ReplyConfirmMessage( F070 );
        }
        delete F070;
    }
    return Result;
}
//----------------------------------------------------------------------------------------------------------------------
