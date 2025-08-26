#include "FIFO.h"
#include "BufferedLog.h"
namespace UFC
{
//------------------------------------------------------------------------------
FIFO::FIFO( const UFC::AnsiString& PipeName, FIFOOpenMode OpenMode )
:FName( PipeName )
,FOpenMode( OpenMode )
{
    int Mode = 0;
    switch( OpenMode )
    {
        case FIFO_ReadOnly: Mode = O_RDONLY;  break;
        case FIFO_NonBlockReadOnly: Mode = O_RDONLY|O_NONBLOCK; break;
        case FIFO_WriteOnly: Mode = O_WRONLY; break;
        case FIFO_NonBlockWriteOnly: Mode = O_WRONLY | O_NONBLOCK; break;
    }
    BufferedLog::Printf( " Check the FIFO file." );    
    
    if( access( PipeName.c_str(), F_OK ) == -1 )
    {
        BufferedLog::Printf( " FIFO doesn't exit." );
        
        if( mkfifo( PipeName, 0777 ) != 0 )
        {
            BufferedLog::Printf( " mkfifo fail" );
            AnsiString ErrStr;
            perror( "FIFO Create" );
            ErrStr.Printf( "Could not create FIFO:[%s]", FName.c_str() );
            UFC::Exception ex( ErrStr.c_str() );
            throw ex;
        }
        else
            BufferedLog::Printf( " mkfifo success." );
            
    }    
    
    PFD = open( FName, Mode );
    
    BufferedLog::Printf( " Open FIFO:[%s]  FD:[%d]", FName.c_str(), PFD );
    if( PFD < 0 )
    {
        AnsiString ErrStr;
        perror( "FIFO open" );
        ErrStr.Printf( "Could not open FIFO:[%s]", FName.c_str() );
        Exception e( ErrStr.c_str() );
        throw e;
    }
    
}
//------------------------------------------------------------------------------
FIFO::~FIFO()
{
    if( PFD > 0 )
        close( PFD );
}
//------------------------------------------------------------------------------
int FIFO::Read( void* Buffer, int Size )
{
    return read( PFD, Buffer, Size );
    
}
//------------------------------------------------------------------------------
int FIFO::Write( const void* Buffer, int Size )
{
    return write( PFD, Buffer, Size );
}
//------------------------------------------------------------------------------
}



