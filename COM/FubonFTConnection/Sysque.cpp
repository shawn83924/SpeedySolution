#include "Sysque.h"
//------------------------------------------------------------------------------
TSysque::TSysque( const UFC::AnsiString& IP, int Port, const UFC::AnsiString& Name )
:FHandle( -1 )
,FQueIP( IP )
,FQuePort( Port )
,FQueName( Name )
{
}
//------------------------------------------------------------------------------
TSysque::~TSysque( void )
{
    Close();
}
//------------------------------------------------------------------------------
BOOL TSysque::Create( unsigned int max_msg_number, unsigned int max_msg_size )
{
    if( FHandle < 0 )
    {
        FHandle = SQCreate( (unsigned char *)(FQueIP.c_str()), FQuePort, (char*)(FQueName.c_str()), max_msg_number, max_msg_size );
        
        if( FHandle < 0 )
            OnError( FHandle );
    }
    return ( FHandle >= 0 );
}
//------------------------------------------------------------------------------
BOOL TSysque::Open( void )
{
    if( FHandle < 0 )
    {
        FHandle = SQOpen( (unsigned char *)(FQueIP.c_str()), FQuePort, (char*)(FQueName.c_str()) );
        
        QueueAttr Attr;
        SQGetQueAttr( FHandle, &Attr );
        
        UFC::BufferedLog::DebugPrintf( " " );
        UFC::BufferedLog::DebugPrintf( " quename    = [%s]", FQueName.c_str() );
        UFC::BufferedLog::DebugPrintf( " extmsg     = [%u]", Attr.extmsg );
        UFC::BufferedLog::DebugPrintf( " initmsg    = [%u]", Attr.initmsg );
        UFC::BufferedLog::DebugPrintf( " maxmsg     = [%u]", Attr.maxmsg );
        UFC::BufferedLog::DebugPrintf( " maxmsgsize = [%u]", Attr.maxmsgsize );
        UFC::BufferedLog::DebugPrintf( " maxqbytes  = [%u]", Attr.maxqbytes );
        UFC::BufferedLog::DebugPrintf( " msgqnum    = [%u]", Attr.msgqnum );        
    
        if( FHandle < 0 )
            OnError( FHandle );
    }
    
    return ( FHandle >= 0 );
}
//------------------------------------------------------------------------------
void TSysque::Close( void )
{
    if( FHandle >= 0 )
    {
        SQClose( FHandle );
    }
}
//------------------------------------------------------------------------------
int TSysque::Read( char* Buffer, int Size )
{
    int ReadSize = 0;
    
    if( SQMsgLeft( FHandle ) > 0 )
    {
        ReadSize = SQRead( FHandle, Buffer, Size - 1 );

        if( ReadSize > 0 )
        {
            Buffer[ReadSize] = 0x00;
            UFC::BufferedLog::DebugPrintf( " SQRead[%s]", Buffer );
        }	
        else
            OnError( ReadSize );
    }
    
    return ReadSize;
}
//------------------------------------------------------------------------------
int TSysque::Write( char* Data, int Size )
{
    static const int SYSQUE_WRITE_KEY_SIZE = 16;
    static char SYSQUE_WRITE_KEY[ SYSQUE_WRITE_KEY_SIZE ];
    
    memset( SYSQUE_WRITE_KEY, 0x20, SYSQUE_WRITE_KEY_SIZE );
    int WriteSize = SQWrite( FHandle, Data, Size, SYSQUE_WRITE_KEY );
    
    if( WriteSize != Size )
        OnError( WriteSize );
    else
    	UFC::BufferedLog::DebugPrintf( " SQWrite[%s]", Data );
    
    return WriteSize;
}
//------------------------------------------------------------------------------
void TSysque::OnError( int ErrorCode )
{
    
    switch( ErrorCode )
    {
        case SQE_SEM_CREATE:
            UFC::BufferedLog::DebugPrintf( " create semaphore fail" );
            break;
        case SQE_GET_SEMKEY:
            UFC::BufferedLog::DebugPrintf( " get semaphore key fail" );
            break;
        case SQE_FILE_CREATE:
            UFC::BufferedLog::DebugPrintf( " create file fail" );
            break;
        case SQE_FILE_OPEN:
            UFC::BufferedLog::DebugPrintf( " file open fail" );
            break;
        case SQE_MMAP_CREATE:
            UFC::BufferedLog::DebugPrintf( " file map memory fail" );
            break;
        case SQE_NO_HANDLE:
            UFC::BufferedLog::DebugPrintf( " no free queue handle" );
            break;
        case SQE_MSGSIZE_NOTMATCH:
            UFC::BufferedLog::DebugPrintf( " create an existed queue and size not match" );
            break;
        case SQE_QUE_NOTFIND:
            UFC::BufferedLog::DebugPrintf( " queue not find" );
            break;
        case SQE_QUE_LOCK:
            UFC::BufferedLog::DebugPrintf( " queue user by other process" );
            break;
        case SQE_INVALID_HANDLE:
            UFC::BufferedLog::DebugPrintf( " invalid queue handle" );
            break;
        case SQE_NO_PERMISSION:
            UFC::BufferedLog::DebugPrintf( " no permission" );
            break;

        case SQE_MSG_TOOBIG:
            UFC::BufferedLog::DebugPrintf( " msg size > buffer size" );
            break;
        case SQE_INVALID_PARA:
            UFC::BufferedLog::DebugPrintf( " invalid parameter" );
            break;
        case SQE_DATA_TOOBIG:
            UFC::BufferedLog::DebugPrintf( " data size > queue max msg size" );
            break;
            
        // remote queue only
        case SQE_TRANSQUE_OPEN:
            UFC::BufferedLog::DebugPrintf( " open transaction queue fail" );
            break;
        case SQE_HOST_NOTFIND:
            UFC::BufferedLog::DebugPrintf( " remote host name not find" );
            break;
        case SQE_CONN_FAIL:
            UFC::BufferedLog::DebugPrintf( " connect fail" );
            break;
        case SQE_CONN_TIMEOUT:
            UFC::BufferedLog::DebugPrintf( " remote connect timeout" );
            break;
        case SQE_QUE_FULL:
            UFC::BufferedLog::DebugPrintf( " queue full" );
            break;
        case SQE_RUN_CHILD:
            UFC::BufferedLog::DebugPrintf( " run child fail" );
            break;
        case SQE_PROG_DOWN:
            UFC::BufferedLog::DebugPrintf( " program down" );
            break;
    }
}
//------------------------------------------------------------------------------
