// 
// File:   Main.cpp
// Author: yuan
//
// Created on September 4, 2007, 2:27 PM
//
//------------------------------------------------------------------------------
#include <stdlib.h>
#include "SpeedyOrderConnection.h"
#include "Sigo.h"
//------------------------------------------------------------------------------
MessageObject*         MBusObject;
SpeedyOrderConnection* OrderConnection = NULL;
const int              COMMAND_CXL_REPLY     = 10008;
//------------------------------------------------------------------------------
class CancelLintener : public MessageListener
{
private: 
    //------------------------------------------------------------------------------    
    void Reply( const UFC::AnsiString& Key, int Count )
    {
        MTree Data;
        
        Data.append( "CMD", COMMAND_CXL_REPLY );
        Data.append( "COUNT", Count );
        Data.append( "Key", Key );
        MBusObject->Send( "SPEEDY.COMMAND", UFC::Hostname, Data );
    }
    //------------------------------------------------------------------------------    
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data )
    {
        UFC::List<TCancelOrderMessage*> CxlList;
        CancelMarket Market = cmAll;
        UFC::AnsiString Account("");
        UFC::AnsiString AE("");
        UFC::AnsiString UserData(""); 
        UFC::AnsiString ProdID(""); 
        UFC::AnsiString ReplyKey;
        Int32 value;
        
        Data->get( "Key", ReplyKey );
        if( OrderConnection != NULL )
        {    
            int CxCount;
            if( Data->get( "Market", value ) == true )
                Market = (CancelMarket)value;
            Data->get( "Account",  Account );
            Data->get( "AE",       AE );
            Data->get( "UserData", UserData );
            Data->get( "ProdID",   ProdID );
            UFC::BufferedLog::Printf( " Cancel Working Market[%d] Account[%s] AE[%s] Data[%s] ProdID[%s]",                             
                                      Market, Account.c_str(), AE.c_str(), UserData.c_str(), ProdID.c_str() );
            OrderConnection->CancelList( CxlList, Market, Account, AE, UserData, ProdID );
            CxCount = CxlList.ItemCount();
            if( CxCount > 0 )
            {
                OrderConnection->Cancel( CxlList );
                Reply( ReplyKey, CxCount );
            }
            else
                Reply( ReplyKey, 0 );
        }
    }
    //------------------------------------------------------------------------------    
};
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "SpeedyWorkingCancel Copyright (c) 2007~%d MDBS\n", UFC::ThisYear() );
    printf( "    -H                   Show this page.\n" );
    exit( 1 );
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{    
    try
    {
        for( int i = 1; i < argc; i++ )
        {
            UFC::AnsiString Param( argv[i] );
            
            if( Param == "-H" || Param == "-h" )
                PrintHelp();
        }             
    }
    catch( UFC::Exception& ex )
    {
        printf( "Error:%s\n", ex.what() );
        PrintHelp();
    }
}
//------------------------------------------------------------------------------
//
// main
//
//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    CheckArgs( argc, argv );
    
    UFC::BufferedLog::SetLogObject( new UFC::BufferedLog( "SpeedyCxlWorking Log","log","SpeedyWorkingCancel","log", 10240, TRUE ) );            
    
    MBusObject = new MessageObject( "SpeedyWorkingCancel", "1.0.0", "Cancel working order utility." ); 
    OrderConnection = new SpeedyOrderConnection( 53030 );
    MBusObject->AddListener( "CANCEL.WORKING", UFC::Hostname, new CancelLintener() );        
    
    ///< Start running thread.
    MBusObject->Start();
    MBusObject->WaitForConnected();    
    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );        
        
        UFC::BufferedLog::FlushToFile();
        fflush( stdout );
    }
    delete OrderConnection;
    return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------
