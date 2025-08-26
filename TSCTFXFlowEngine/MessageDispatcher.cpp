#include "MessageDispatcher.h"
//---------------------------------------------------------------------------
MessageDispatcher::MessageDispatcher( const UFC::AnsiString& FileName, BOOL FirstTime )
:FMessageObject( "TSCTFXFlowEngine", "2.0", "Flow control between AS/400 and FIX gateway." )
{
    UFC::UiniFile   Config( FileName );
    int             SectionCount = Config.SectionCount();
    SocketSend*     pConnection;

        for( int i = 0; i < SectionCount; i++ )
        {
            UFC::Section* pSection = Config.GetSection( i );
            Setting*      pSet = new Setting( pSection );

            if( FConnections.GetObjectByKey(pSet->WebID) == NULL )
            {
                UFC::PtrQueue<UFC::AnsiString>*     JobQueue = new UFC::PtrQueue<UFC::AnsiString>();
                pConnection = new SocketSend( pSet, FirstTime, &FMessageObject, JobQueue );
                FConnections.Add( pSet->WebID, JobQueue );
                pConnection->Run();
            }
            else
            {
                pConnection = new SocketSend( pSet, FirstTime, &FMessageObject, FConnections.GetObjectByKey(pSet->WebID) );
                pConnection->Run();
            }
        }

    FMessageObject.AddListener( "ORDER", UFC::Hostname, this );
}
//---------------------------------------------------------------------------
void MessageDispatcher::OnMigoMessage( UFC::AnsiString Subject,UFC::AnsiString Key, MTree* Data )
{
    MString     WebID;
    Int32       Market;
    
    UFC::PtrQueue<UFC::AnsiString>*   FQueue;

    if( Data->get( "EDS", WebID ) == TRUE && Data->get( "SYS", Market ) == TRUE && Market != 2 ) // 0:future, 1:Option, 2: Stock
    {
        ///< Dispatch the event according to the key string.
        if( ( FQueue = FConnections.GetObjectByKey( WebID ))!= NULL )
        {
            UFC::AnsiString* Order = new UFC::AnsiString();

            if( Data->get( "ORDER", *Order ) == TRUE )
                FQueue->Inqueue( Order );            
        }
    }
}
//------------------------------------------------------------------------------
void MessageDispatcher::Run( void )
{
    FMessageObject.Start();
    while( TRUE )
    {
        sleep( 10 );
    }
}
//------------------------------------------------------------------------------
