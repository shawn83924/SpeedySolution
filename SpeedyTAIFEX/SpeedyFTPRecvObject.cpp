//---------------------------------------------------------------------------
// SpeedyFTPRecvObject.cpp
//---------------------------------------------------------------------------
#include "SpeedyFTPRecvObject.h"
#include "SpeedyObjectsManager.h"
#include "../UFC/iniFile.h"
//------------------------------------------------------------------------------
extern MessageObject*         MessageObj;
//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
TSpeedyFTPRecvObject::TSpeedyFTPRecvObject( SpeedyObjectsManager* ObjManager,
                                            const UFC::AnsiString& SourceID,
                                            const UFC::AnsiString& DestinationID,
                                            ConnectionParameter& info,
                                            UFC::ThreadListener* TListener,
                                            ConnectionListener* CListener,
                                            BOOL IsOpt )
:TSpeedyFTPConnectionObject( ObjManager, info, TListener, CListener, IsOpt )
{
    FSourceID = SourceID;
    FDestinationID = DestinationID;
    UFC::AnsiString LineBrokerID = info.GetBrokerID();

    FmType = ((LineBrokerID[1] -'0')*256 +(LineBrokerID[2] -'0')*16 + (LineBrokerID[3] -'0')) << 12;
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
    UFC::AnsiString Command,FileName;
    UFC::AnsiString Script,FilePath;
        
    if( FileCode == "P01" )
        new DecimalLocatorThread( FIsOptions );
    
    if( FIsOptions == TRUE )    
    {
        UFC::UiniFile   Config( "../cfg/OptionFT.cfg" );
        
        if( Config.SectionExists( FileCode ) == TRUE )
        {
            if( Config.GetValue( FileCode, "Script", Script ) && Config.GetValue( "Settings", "OPTFilePath",FilePath) )
            {            
                Command.Printf("%s %s %s%s &",Script.c_str(), FileCode.c_str(), FilePath.c_str(), FileCode.c_str() );            
                UFC::BufferedLog::Printf( " Execute command:%s",Command.c_str());
                system( Command.c_str());
            }
            else
                UFC::BufferedLog::Printf( " Can not get the Script or the OPTFilePath." );
        }        
        else
            UFC::BufferedLog::Printf( " Can not find a script for file code:%s.", FileCode.c_str() );
        MTree Data;

        FileName.Printf( "%s%s", "../ftOPT/", FileCode.c_str() );          
        Data.append( "Sender", GetConnectionParameter()->GetBrokerID());        
        Data.append( "FileCode", FileCode);
        Data.append( "Status", 0 );
        Data.append( "FileName", FileName );
        MessageObj->Send( "FT.RESPONSE.OPT", GetConnectionParameter()->GetBrokerID(), Data, FALSE );        
        
    }
    else
    {
        UFC::UiniFile   Config( "../cfg/FuturesFT.cfg" );
        
        if( Config.SectionExists( FileCode ) == TRUE )
        {
            if( Config.GetValue( FileCode, "Script", Script ) && Config.GetValue( "Settings", "FUTFilePath",FilePath) )
            {            
                Command.Printf("%s %s %s%s &",Script.c_str(), FileCode.c_str(), FilePath.c_str(), FileCode.c_str() );            
                UFC::BufferedLog::Printf( " Execute command:%s",Command.c_str());
                system( Command.c_str());
            }
            else
                UFC::BufferedLog::Printf( " Can not get the Script or the FUTFilePath." );
        }        
        else
            UFC::BufferedLog::Printf( " Can not find a script for file code:%s.", FileCode.c_str() );
        MTree Data;

        FileName.Printf( "%s%s", "../ftFUT/", FileCode.c_str() );          
        Data.append( "Sender", GetConnectionParameter()->GetBrokerID());        
        Data.append( "FileCode", FileCode);
        Data.append( "Status", 0 );
        Data.append( "FileName", FileName );
        MessageObj->Send( "FT.RESPONSE.FUT", GetConnectionParameter()->GetBrokerID(), Data, FALSE );        
    }
}
//----------------------------------------------------------------------------------------------------------------------
BOOL TSpeedyFTPRecvObject::Download( MessageF010* F010 )
{
    if( ! ReplyConfirmMessage( F010 ) )
        return FALSE;
    UFC::AnsiString   FileCode = F010->GetFileCode();
    UFC::FileStream*  FS = OpenFile( FileCode );
    MessageF030* F030 = NULL;
    char         IsEOF = '0';

    if( FS != NULL )
    {
        try
        {
            while( TRUE )
            {
                if( ( F030 = ReceiveF030( FileCode ) ) == NULL )
                    break;
                FS->Write( F030->GetData(), F030->GetMessageLength() - 4 );
                IsEOF = F030->GetEOF();
                delete F030;
                F030 = NULL;
                if( IsEOF == '1' )
                {
                    ReportStatusCode( FileCode, FTP_DOWNLOAD_FINISHED );
                    ToBackend( FileCode ); ///< download finished.
                    break;
                }
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
UFC::FileStream* TSpeedyFTPRecvObject::OpenFile( const UFC::AnsiString& FileCode )
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
