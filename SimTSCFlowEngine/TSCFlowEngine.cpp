#include "../UFC/UFC.h"
#include "../UFC/PQueue.h"
#include "../UFC/iniFile.h"
#include "../Migo/Sigo.h"
#include "TSCFlowEngine.h"
//------------------------------------------------------------------------------
//  EDS Check Result queue format.
//
//  00 + 0 + 0 + 00 + "    " + Error String
//  --   -   -   --
//  |    |   |   |_ EDS Error code.
//  |    |   |_____ Open/close offset.
//  |    |_________ Account Flag.
//  |______________ TAIFEX Status ??
//
//------------------------------------------------------------------------------
MessageObject * MessageObj = NULL;
BOOL RecoverMode = TRUE;
BOOL IsStandby   = FALSE;
//------------------------------------------------------------------------------
UFC::PCriticalSection FOPTOrderCS;
UFC::PCriticalSection FFUTOrderCS;
UFC::PHashedList < Int32, OrderObj * > FFUTOrders;
UFC::PHashedList < Int32, OrderObj * > FOPTOrders;
//----------------------------------------------------------------------------------------------------------------------
BOOL CheckConfirmErr( UFC::AnsiString & Confirm ) ///< Add error code to R030
{
    char Err[3]="00";

    memcpy( Err, Confirm.c_str() + 12, 2 );
    if( atoi( Err ) != 0 ) ///< Is a err from TAIFEX.
    {
        Confirm[ 0 ] = '9';
        Confirm[ 1 ] = '9';
        return FALSE;
    }
    return TRUE;
}
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{
    for( int i = 1; i < argc; i++ )
    {
       UFC::AnsiString Param( argv[i] );
       if( Param == "-H" || Param == "-h" || Param == "-?" )
       {
           printf( "Usage: SpeedyFlowEngine [options]\n" );
           printf( "   -H  Show this page.\n" );
           printf( "   -F  First time startup.( Clear all queue).\n" );
           printf( "   -S  Standby mode.\n" );
           exit( 1 );
       }
       else if( Param == "-F" || Param == "-f" )
            RecoverMode = FALSE;
       else if( Param == "-S" || Param == "-s" )
            IsStandby = TRUE;
    }
}
//------------------------------------------------------------------------------
int main( int argc, char * argv[] )
{
    CheckArgs( argc, argv );
    UFC::BufferedLog::Printf( "__________________________________________" );
    UFC::BufferedLog::Printf( "                                          " );
    UFC::BufferedLog::Printf( "   Flow control module,SpeedyFlowEngine   " );
    UFC::BufferedLog::Printf( "   startup at %s.            ", UFC::Hostname );
    UFC::BufferedLog::Printf( "                                          ");
    UFC::BufferedLog::Printf( "   TSC Version." );
    if( IsStandby == TRUE )
        UFC::BufferedLog::Printf("    Standby Mode:ON" );
    else
        UFC::BufferedLog::Printf("    Standby Mode:OFF" );
    UFC::BufferedLog::Printf( "   Ver:1.2.21 Build Date:%s ",__DATE__ );
    UFC::BufferedLog::Printf( "__________________________________________" );
    MessageObj = new MessageObject( "TSCFlowEngine", "1.0", "FlowEngine" ); ///< Create Migo Message object.
    MessageObj->AddListener( "ORDER", "all", new OrderListener() );
    MessageObj->AddListener( "ORDER.CONFIRM.FUT", "all", new FUTConfirmListener() );
    MessageObj->AddListener( "ORDER.CONFIRM.OPT", "all", new OPTConfirmListener() );
    MessageObj->Start();        ///< Start Message bump.
    sleep( 2 );
    while ( TRUE )
    {
        sleep( 5 );
    }
    return 0;
}
//------------------------------------------------------------------------------
//
//  For incoming Order
//
// Class to handle the incoming order message
//------------------------------------------------------------------------------
void OrderListener::GenerateOrderID( UFC::AnsiString & OrderID,BOOL Quote )
{
    static char TNo = 'A';
    static char TQNo = 'Q';
    static int  OrdNo = 1;
    static int  QuoteNo = 1;

    if( Quote == TRUE )
    {
        OrderID.Printf("%c%04d",TQNo,QuoteNo );
        QuoteNo++;
        if( QuoteNo == 999 )
        {
           QuoteNo = 1;
           TQNo++;
        }
    }
    else
    {
        OrderID.Printf("%c%04d",TNo,OrdNo );
        OrdNo++;
        if( OrdNo == 999 )
        {
           OrdNo = 1;
           TNo++;
        }
    }
}
//------------------------------------------------------------------------------
void OrderListener::ConvertOrder( UFC::AnsiString& Order,BOOL IsOpt )
{
    UFC::AnsiString OrderID;

    if( Order[0] == '4' )///Quote
    {
        if( Order[1] == '1') ///< New quote
        {
            memcpy( (void*)Order.c_str(),"30010812121200", 14 );
            GenerateOrderID( OrderID, TRUE );
            memcpy( (void*)(Order.c_str() + 21), OrderID.c_str(), 5 );
        }
        else ///< Quote cancel
            memcpy( (void*)Order.c_str(),"30030812121200", 14 );
        Order[33] = '8'; ///< Replace the Account flag.
    }
    else
    {
        switch( Order[1] )
        {
            case '1': memcpy( (void*)Order.c_str(),"30010012121200", 14 );
                      GenerateOrderID( OrderID );
                      memcpy( (void*)(Order.c_str() + 21), OrderID.c_str(), 5 );
                      break;
            case '2': memcpy( (void*)Order.c_str(),"30020012121200", 14 );break;
            case '3': memcpy( (void*)Order.c_str(),"30030012121200", 14 );break;
        }
        if( IsOpt == TRUE )
        {
            Order[70] = '0'; ///< Replace the open/close offset.
            Order[33] = '8'; ///< Replace the Account flag.
        }
        else
        {
            Order[69] = '0'; ///< Replace the open/close offset.
            Order[33] = '2'; ///< Replace the Account flag.
        }
    }
    //Order.SetLength( Order.Length()-12 );
}
//------------------------------------------------------------------------------
void OrderListener::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree * Data )
{
    UFC::AnsiString Order, EDS;
    Int32 IsOpt, NID;
    MTree ReplyData;

    if ( Data->get( "ORDER", Order ) && Data->get( "EDS", EDS ) && Data->get( "SYS", IsOpt ) && Data->get( "NID", NID ) )
    {
        ConvertOrder( Order, IsOpt );
        if( IsOpt )
        {
            UFC::PLockObject Lock( FOPTOrderCS );

            FOPTOrders.Add( NID, new OrderObj( NID, Order, EDS ) );
            ReplyData.append( "ORDER", Order );
            ReplyData.append( "EDS", EDS );
            ReplyData.append( "NID", NID );
            MessageObj->Send( "ORDER.OPT", UFC::Hostname, ReplyData, FALSE );
        }
        else
        {
            UFC::PLockObject Lock( FFUTOrderCS );

            FFUTOrders.Add( NID, new OrderObj( NID, Order, EDS ) );
            ReplyData.append( "ORDER", Order );
            ReplyData.append( "EDS", EDS );
            ReplyData.append( "NID", NID );
            MessageObj->Send( "ORDER.FUT", UFC::Hostname, ReplyData, FALSE );
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------
//
// For Order Confirm
//
// Class to handle the order confirm message from Speedy
//----------------------------------------------------------------------------------------------------------------------
void OPTConfirmListener::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree * Data )
{
    UFC::AnsiString Confirm;
    Int32 NID;
    MTree Tree;

    if ( Data->get( "CONFIRM_ORDER", Confirm ) && Data->get( "NID", NID ) )
    {
        OrderObj * OrderObject;

        FOPTOrderCS.Enter();
        OrderObject = FOPTOrders.GetObjectByKey( NID );
        FOPTOrders.DeleteByKey( NID );
        FOPTOrderCS.Leave();

        if( OrderObject )
        {
            UFC::AnsiString ConfirmString;

            if( CheckConfirmErr( Confirm ) == TRUE )
            {
                if( Confirm[5] == '9')///<Quote
                    ConfirmString.Printf( "21%s%s000F20061008",Confirm.c_str()+14, OrderObject->EDS.c_str() );
                else
                    ConfirmString.Printf( "21%s%s000F20061008",Confirm.c_str(), OrderObject->EDS.c_str() );
            }
            else
                ConfirmString.Printf( "23%s%s%100d000F20061008",Confirm.c_str(), OrderObject->EDS.c_str(), NID );
            Tree.append( "REPLY", ConfirmString );
            Tree.append( "EDS",   OrderObject->EDS.c_str() );
            Tree.append( "NID",   NID );
            MessageObj->Send( "REPLY.OPT", UFC::Hostname, Tree );
            delete OrderObject;
        }
        else
            UFC::BufferedLog::Printf( " Option order %d not found", NID );
    }
}
//----------------------------------------------------------------------------------------------------------------------
void FUTConfirmListener::OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key, MTree * Data )
{
    UFC::AnsiString Confirm;
    Int32 NID;
    MTree Tree;

    if ( Data->get( "CONFIRM_ORDER", Confirm ) && Data->get( "NID", NID ) )
    {
        OrderObj * OrderObject;

        FFUTOrderCS.Enter();
        OrderObject = FFUTOrders.GetObjectByKey( NID );
        FFUTOrders.DeleteByKey( NID );
        FFUTOrderCS.Leave();

        if ( OrderObject )
        {
            UFC::AnsiString ConfirmString;

            if( CheckConfirmErr( Confirm ) == TRUE )
                ConfirmString.Printf( "11%s%s000F20061008",Confirm.c_str(), OrderObject->EDS.c_str() );
            else
                ConfirmString.Printf( "13%s%s%100d000F20061008",Confirm.c_str(), OrderObject->EDS.c_str(), NID );
            Tree.append( "REPLY", ConfirmString );
            Tree.append( "EDS", OrderObject->EDS.c_str() );
            Tree.append( "NID", NID );
            MessageObj->Send( "REPLY.FUT", UFC::Hostname, Tree );
            delete OrderObject;
        }
        else
            UFC::BufferedLog::Printf( " Futures order %d not found", NID );
    }
}
//------------------------------------------------------------------------------
