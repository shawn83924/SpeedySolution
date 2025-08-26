#include "../../TAIFEXLib/X25Connection.h"
#include "../../UFC/UFC.h"
//------------------------------------------------------------------------------
#ifdef _WIN32
//------------------------------------------------------------------------------
X25Connection* FConnection;
//------------------------------------------------------------------------------
BOOL WINAPI OnWin32Exit( DWORD dwCtrlType)
{	
	switch( dwCtrlType )
	{
		case CTRL_C_EVENT:       UFC::BufferedLog::Printf( " Ctrl-C event" );
					 break;
		case CTRL_CLOSE_EVENT: 	 UFC::BufferedLog::Printf( " Ctrl-Close event" );
					 break;
		case CTRL_BREAK_EVENT:   UFC::BufferedLog::Printf( " Ctrl-Break event" );
					 break;
		case CTRL_LOGOFF_EVENT:  UFC::BufferedLog::Printf( " Ctrl-Logoff event" );
					 break;
		case CTRL_SHUTDOWN_EVENT:UFC::BufferedLog::Printf( " Ctrl-Shutdown event" );
					 break;
		default:		 UFC::BufferedLog::Printf( " Unknown event:%d", dwCtrlType );
					 return FALSE;
	}
	UFC::BufferedLog::Printf( " Try to close PVC." );		
	FConnection->Close();
  UFC::BufferedLog::Printf( " PVC closed." );		
	X25Connection::CloseX25();	
	UFC::BufferedLog::Printf( " Exit." );	
	return FALSE;
}
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    if( argc == 3 )
    {
		#ifdef _WIN32
              SetConsoleCtrlHandler( OnWin32Exit, TRUE );
        #endif
        UFC::BufferedLog::Printf( "__________________________________________" );
        UFC::BufferedLog::Printf( "                                          " );
        UFC::BufferedLog::Printf( "   x25recv PVC testing tool               " );
        UFC::BufferedLog::Printf( "   Copyright (c) 2003,2005 MDBS .         " );
        UFC::BufferedLog::Printf( "                                          ");
        UFC::BufferedLog::Printf( "   Ver:1.0.0 Build Date:%s ",__DATE__ );
        UFC::BufferedLog::Printf( "__________________________________________" );

        int  Link = atoi(argv[1]);
        int  PVC  = atoi(argv[2]);
        char DataBuffer[256];
        int  DataSize;
        FConnection = new X25Connection();

        try
        {
            FConnection->Open( Link, PVC );
            while( TRUE )
            {
                memset( DataBuffer, 0, 256 );
                if((DataSize = FConnection->Receive( DataBuffer, 256, 30 )) >= PVC_OK ) ///< Receive a normal message
                    UFC::BufferedLog::Printf(" Recv:%s", DataBuffer );
                else if( DataSize <= PVC_COMMAND ) ///< Is a control command.
								UFC::BufferedLog::Printf(" Receive PVC Reset request" );
            }
            FConnection->Close();
            return 1;
        }
        catch( X25Exception& e )
        {
            FConnection->Close();
            UFC::BufferedLog::Printf( " %s", e.what());
            return 1;
        }
        catch( exception& )
        {
            FConnection->Close();
            return 1;
        }
    }
    else
        printf( "Usage: x25recv <Link> <PVC>\n" );
    return 1;
}
//------------------------------------------------------------------------------