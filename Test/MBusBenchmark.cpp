#ifndef _WIN32
    #ident "@(#) $Id: SigoPublisher.cpp,v 1.12 2004/05/21 10:34:18 ap27 Exp $"
    #include <signal.h>
#endif
#include <stdio.h>
#include "../Migo/Sigo.h"
#include "../Migo/MApp.h"
#include "../UFC/UFC.h"
//--------------------------------------------------------------------------------------------------------------------
MessageObject *Sigo;
BOOL StaticMode = FALSE;
BOOL StressMode = FALSE;
//--------------------------------------------------------------------------------------------------------------------
void Benchmark()
{
    MTree*               Tree;
    int                  Begin,Diff;
    UFC::PtrQueue<MTree> MsgQueue( 10000 );

    Begin = UFC::GetTickCountMS();
    for( int i=0; i < 10000; i++ )
    {
        Tree = new MTree();
        Tree->append( "ORDER", "30010012121200*1234567890*1234567890*1234567890*1234567890*1234567890*12324567890*1234567890*1234567890" );
        Tree->append( "NID", 12345678 );
        Tree->append( "EDS", "123451234512345" );
        Tree->append( "SYS", 1 ); ///< 1 for Options, 0 for Futures.
        MsgQueue.Inqueue( Tree );
    }
    Diff = UFC::GetTickCountMS() - Begin;
    printf( "Create 10000 MTree Order messages use:%dms %f msg/sec \n", Diff, (float)10000000/(float)Diff );
    Begin = UFC::GetTickCountMS();
    while(  ( Tree = MsgQueue.DequeueNoWait( ) ) != NULL )
    {
        delete Tree;
    }
    Diff = UFC::GetTickCountMS() - Begin;
    printf( "Destroy 10000 MTree Order messages use:%dms %f msg/sec \n", Diff, (float)10000000/(float)Diff );

    UFC::AnsiString Subject = "ORDER";
    UFC::AnsiString Key = "all";
    MTree           Data;

    Data.append( "ORDER", "30010012121200*1234567890*1234567890*1234567890*1234567890*1234567890*12324567890*1234567890*1234567890" );
    Data.append( "NID", 12345678 );
    Data.append( "EDS", "123451234512345" );
    Data.append( "SYS", 1 ); ///< 1 for Options, 0 for Futures.
    Begin = UFC::GetTickCountMS();
    for( int i=0; i < 10000; i++ )
    {
        UFC::MemoryStream WriteStream( 96 );
        DataMessage::MessageToStream( 0, Subject, Key, Data, &WriteStream );
    }
    Diff = UFC::GetTickCountMS() - Begin;
    printf( "Serializing 10000 Order messages to stream use:%dms %f msg/sec \n", Diff, (float)10000000/(float)Diff );


    UFC::MemoryStream TestStream( 96 );
    DataMessage::MessageToStream( 0, Subject, Key, Data, &TestStream );

    Begin = UFC::GetTickCountMS();
    for( int i=0; i < 10000; i++ )
    {
        TestStream.Seek( 0, UFC::soFromBeginning  );
        MigoHeader* Msg;
        Msg = MigoHeader::CreateFromStream( &TestStream );
        delete Msg;
    }
    Diff = UFC::GetTickCountMS() - Begin;
    printf( "Serializing 10000 Order messages from stream use:%dms %f msg/sec \n", Diff, (float)10000000/(float)Diff );

}
//--------------------------------------------------------------------------------------------------------------------
void SendOrder()
{
    MTree Data;

    Data.append( "ORDER", "30010012121200*1234567890*1234567890*1234567890*1234567890*1234567890*12324567890*1234567890*1234567890" );
    Data.append( "NID", 12345678 );
    Data.append( "EDS", "123451234512345" );
    Data.append( "SYS", 1 );
    Sigo->Send( "ORDER", "all", Data );
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char * argv[] )
{
    for( int i = 1; i < argc; i++ )
    {
        UFC::AnsiString Param( argv[i] );
        if( Param == "-H" || Param == "-h" || Param == "-?" )
        {
            printf( "MBusBenchmark Copyright (c) 2003,2005 MDBS\n");
            printf( "Usage: MBusBenchmark [options]\n" );
            printf( "   -H  Show this page.\n" );
            printf( "   -B  Static benchmark.\n" );
            printf( "   -S  Stress benchmarket.\n" );
            exit( 1 );
        }
        else if( Param == "-B" || Param == "-b" )
            StaticMode = TRUE;
        else if( Param == "-S" || Param == "-s")
            StressMode = TRUE;
    }
}
//------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    CheckArgs( argc, argv );
    if( StaticMode == TRUE )
        Benchmark();
    if( StressMode == TRUE )
    {
        Sigo = new MessageObject( "MBusBenchmark", "1.0", "MBusBenchmark test program." );
        Sigo->Start();

        sleep( 5 );
        while( TRUE )
        {
           SendOrder();
        }
    }
}
//--------------------------------------------------------------------------------------------------------------------
