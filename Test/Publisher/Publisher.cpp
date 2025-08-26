#ifndef _WIN32
    #ident "@(#) $Id: SigoPublisher.cpp,v 1.12 2004/05/21 10:34:18 ap27 Exp $"
    #include <signal.h>
#endif
#include <stdio.h>
#include "Sigo.h"
#include "MApp.h"
#include "../UFC/UFC.h"
//--------------------------------------------------------------------------------------------------------------------
MessageObject *MBusObj;
//--------------------------------------------------------------------------------------------------------------------
void Wakeup( const UFC::AnsiString& APPName )
{
    MTree Data;

    Data.append( "DATA", APPName );
    MBusObj->Send( "WAKEUP", UFC::Hostname, Data );
}
//------------------------------------------------------------------------------
void ParseArgs( int argc, char** argv )
{
     for( int i = 1; i < argc; i++ )
     {
         UFC::AnsiString Param( argv[i] );
         if( Param == "-H" || Param == "-h" )
         {
             printf( "MBus Message publisher Copyright (c) 2003,2005 MDBS\n");
             printf( "Usage: Publisher\n" );
             printf( "       -H           Show this page.\n" );
             printf( "       -W [Process] Wakeup process.\n" );
             exit( 1 );
         }
         else if( Param == "-W" || Param == "-w" )
         {
             for( int j = 2; j < argc; j++ )
             {
                 printf( "Wake up process:%s\n", argv[ j ]  );
                 for( int i= 0; i<5; i++ )
                 {
                     Wakeup( argv[ j ] );
                     UFC::SleepMS( 100 );
                 }
             }
         }
     }
}
//------------------------------------------------------------------------------
void Test()
{
    UFC::PHashedList<UFC::AnsiString,UFC::AnsiString*> HashTable;

    for( int i=0;i<100;i ++ )
    {
        UFC::AnsiString Name,Value;
        Name.Printf( "Name%d", i );
        Value.Printf( "Value%d", i );
        HashTable.Add( Name, new UFC::AnsiString( Value.c_str() ) );
    }
    printf( "Count=%d\n", HashTable.ItemCount() );
    UFC::AnsiString* Obj;
    if( ( Obj = HashTable.GetObjectByKey("Name501")) != NULL )
        printf( "found! Value=%s\n", Obj->c_str() );
}
//------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    MBusObj = new MessageObject( "Publisher", "1.0", "Publisher test program." );
    MBusObj->Start();
    sleep( 2 );
    ParseArgs( argc, argv );
    sleep( 3 );
}
//--------------------------------------------------------------------------------------------------------------------
