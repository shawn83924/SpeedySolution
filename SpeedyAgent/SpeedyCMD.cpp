#include "SpeedyAgent.h"
//--------------------------------------------------------------------------------------------------------------------
StdioPublisher::StdioPublisher( )
:UFC::PThread( NULL )
,FIsOpen( FALSE )
,FKey( "" )
{
    Start();
}
//--------------------------------------------------------------------------------------------------------------------
void StdioPublisher::Execute( void )
{
    UFC::FileStreamEx StdioFile( "Stdio.txt", "r" );

    while( TRUE )
    {
        UFC::AnsiString StdoutLine = StdioFile.ReadLine();

        if( FIsOpen == TRUE  && StdoutLine.Length() > 0 )
        {
            MTree Data;
            Data.append( "LINE", StdoutLine );
            MessageObj->Send( UFC::Hostname, FKey, Data, FALSE );
        }
        else
            UFC::SleepMS( 50 );
    }
}
//--------------------------------------------------------------------------------------------------------------------
CMDListener::CMDListener()
:UFC::PThread( NULL )
{
    FILE *stream;

    if( (stream = freopen("Stdio.txt", "w", stdout)) == NULL )
         exit( -1 );
    FStdioPublisher = new StdioPublisher();
    Start();
}
//--------------------------------------------------------------------------------------------------------------------
void CMDListener::Execute( void )
{
    while( TRUE )
    {
        if( FCMDs.ItemCount() > 0 )
        {
            FCMDCS.Enter();
            UFC::AnsiString CMD = FCMDs[0];
            FCMDs.Delete(0);
            FCMDCS.Leave();
            if( CMD.AnsiPos("cd") == 0 )
            {
                chdir( CMD.c_str() + 3 );
                system( "pwd" );
            }
            else
                system( CMD );
            fflush( stdout );
        }
        else
            UFC::SleepMS( 100 );
    }
}
//--------------------------------------------------------------------------------------------------------------------
void CMDListener::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree* Data )
{
    int Action;
    UFC::AnsiString CMD;

    if( Data->get( "CMD", CMD ) && Data->get( "ACT", Action ) )
    {
        if( Action == 0 )
            Begin( Key );
        else if( Action == 1 )
        {
            UFC::PLockObject Lock( FCMDCS );
            FCMDs.Add( CMD );
        }
        else if( Action == 2 )
            End();
    }
}
//--------------------------------------------------------------------------------------------------------------------
