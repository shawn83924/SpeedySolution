// X25Send.cpp : Defines the entry point for the console application.
#include "../../UFC/UFC.h"
#include "../../TAIFEXLib/X25Connection.h"
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
        UFC::BufferedLog::Printf( "   x25send PVC testing tool               " );
        UFC::BufferedLog::Printf( "   Copyright (c) 2003,2005 MDBS .         " );
        UFC::BufferedLog::Printf( "                                          ");
        UFC::BufferedLog::Printf( "   Ver:1.0.0 Build Date:%s ",__DATE__ );
        UFC::BufferedLog::Printf( "__________________________________________" );

        int  Link = atoi(argv[1]);
        int  PVC  = atoi(argv[2]);
        char DataBuffer[256];
        int  DataSize;
     
				
        try
        {
					  FConnection = new X25Connection();
            FConnection->Open( Link, PVC );
            while( TRUE )
            {
                memset( DataBuffer, 0, 256 );
                UFC::BufferedLog::Printf( " Input data:" );
                scanf( "%s", DataBuffer );
                if((DataSize = FConnection->Send( DataBuffer, (int)strlen(DataBuffer), 30 )) >= PVC_OK )///< Send X.25 message succeed.
                {
                    UFC::BufferedLog::Printf( " Sent:%s", DataBuffer );
                }
                else		     ///< X.25 timeout or disconnect
                {
                    if( DataSize == PVC_RESET )
                        throw( X25ReconnectException( "Receive PVC Reset request" ) );
                    else
                        throw( X25ReconnectException( "Send data timeout" ) );
                }
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
        catch( exception &e )
        {
            FConnection->Close();
            return 1;
        }
    }
    else
        printf( "Usage: x25send <Link> <PVC>\n" );
    return 1;
}
