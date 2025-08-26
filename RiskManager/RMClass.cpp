/* 
 * File:   RMClass.cpp
 * Author: yuan
 * 
 * Created on 2016年1月18日, 下午 10:19
 */
#include "RMClass.h"
#include "../UFC/BufferedLog.h"
//------------------------------------------------------------------------------
UFC::PInt32     FRecoverBeginTime;
BOOL            Newfprintf  = TRUE;
BOOL            KeepLog     = TRUE;
BOOL            RecoverMode = FALSE;
BOOL            AS400Check  = FALSE;
UFC::UTime      FTime;
UFC::AnsiString AppName = "GCRiskManager";
UFC::AnsiString FRecoverTimeFileName = "DynamicRM";
UFC::BufferedLog* GLog = NULL;
//------------------------------------------------------------------------------
RMObj* CreateRM( BOOL RMode )
{
    if( GLog == NULL )
        GLog = new UFC::BufferedLog( "TAIFEX RM Log","log","TAIFEXRiskManager","log", 1048576, TRUE );                
    return new RMClass( RecoverMode );
}
//------------------------------------------------------------------------------
void DestroyRM( RMObj* RM )
{    
    if( RM != NULL )
        delete RM;
 }    
//------------------------------------------------------------------------------
RMClass::RMClass( BOOL RMode ) 
:RMObj()
,PThread( NULL )
,FListener( NULL )
{
    RecoverMode = RMode;
    FRM = new GCRMConnection( 80000 );   
    GLog->fprintf( " ******** RMClass created OK!" );
    GLog->fprintf( " ******** File[%s]", __BASE_FILE__ );
    Start( );
}
//------------------------------------------------------------------------------
RMClass::~RMClass() 
{
    delete FRM;
}
//------------------------------------------------------------------------------
void RMClass::SetResultListener( CheckResultListener* Listener ) 
{ 
    FListener = Listener; 
    FRM->SetResultListener( Listener );
}   
//------------------------------------------------------------------------------
void RMClass::Check( CheckData& Obj )
{
    TCheckResult Result = FRM->CheckOrder( Obj );
    
    switch( Result )
    {
        case crReject: if( FListener != NULL )
                           FListener->OnResult( Obj, 0 );
                       break;    
        case crPass:   if( FListener != NULL )
                           FListener->OnResult( Obj, 1 );
                       break;    
        case crToOderRM:break;
    }    
}
//------------------------------------------------------------------------------
void RMClass::Execute( void )
{   
    int Counter = 6;    

    UFC::SleepMS( 20000 );  
    GLog->fprintf( " ******** RMClass Set MBus object.");        
    FRM->InitMBus( FListener->OnGetMBusObject() );        
    FRM->SetAccountInfo( "9903758", "yuan", 100000000 );
    
    while( TRUE )
    {
        UFC::SleepMS( 1000 );        
                
        if( Counter % 5 == 0 ) ///< Check connection every 5 sec         
        {            
            FRM->CheckConnection();
            GLog->FlushToFile();
        }        
        Counter++;
    }
}
//------------------------------------------------------------------------------
