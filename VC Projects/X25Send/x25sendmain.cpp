#include "TAIFEXLib/Connection.h"
#include "UFC/UFC.h"


int main(int argc, char *argv[])
{
    if( argc == 3 )
    {
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
        X25Connection FConnection;

        try
        {
            FConnection.Open( Link, PVC );
            while( TRUE )
            {
                memset( DataBuffer, 0, 256 );
                UFC::BufferedLog::Printf( " Input data:" );
                scanf( "%s", DataBuffer );
                if((DataSize = FConnection.Send( DataBuffer, strlen(DataBuffer), 30 )) >= PVC_OK )///< Send X.25 message succeed.
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
            FConnection.Close();
            return 1;
        }
        catch( X25Exception& e )
        {
            FConnection.Close();
            UFC::BufferedLog::Printf( " %s", e.what());
            return 1;
        }
        catch( exception &e )
        {
            FConnection.Close();
            return 1;
        }
    }
    else
        printf( "Usage: x25send <Link> <PVC>\n" );
    return 1;
}
