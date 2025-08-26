#ifndef _WIN32
    #ident "@(#) $Id: SigoPublisher.cpp,v 1.12 2004/05/21 10:34:18 ap27 Exp $"
    #include <signal.h>
#endif
#include <stdio.h>
#include "../Sigo/Sigo.h"
#include "MApp.h"
//--------------------------------------------------------------------------------------------------------------------
MessageObject *Sigo;
//--------------------------------------------------------------------------------------------------------------------
void SendDBInfo()
{
    MTree Data;
    MString ID("ServerId");
    MString USR("User");
    MString PWD("Password");
    MString DBN("DBName");
    int CurrentOpenSwitch = 10;
    Data.append( ID, "OPTIONSDB1" );
    Data.append( USR, "apusr1" );
    Data.append( PWD, "apusr1" );
    Data.append( DBN, "options" );
    Data.append("Schctrl", CurrentOpenSwitch );
    Sigo->Send( "TFX.OPT.PROD.ADS.DBS.LGI", "all", Data );
//    printf("\nSend DB information to TFX.OPT.PROD.ADS.DBS.LGI\n");
}
//--------------------------------------------------------------------------------------------------------------------
void SendOpenSwitch()
{
	int Value;
	MTree Data;

	printf("Open switch Value:");
	scanf( "%d", &Value );

	Data.append("Schctrl", Value );
    Sigo->Send( "TFX.UNI.PROD.SCH", "OPT", Data );

	printf("\nSend OpenSwitch %d to TFX.UNI.PROD.SCH.\n",Value);
}
//--------------------------------------------------------------------------------------------------------------------
void SendPVCReset()
{
	int Link,PVC;
	MTree Data;

	printf("Link Number:");
	scanf( "%d", &Link );

	printf("PVC Number:");
	scanf( "%d", &PVC );

	AnsiString LinkStr( Link );
	AnsiString PVCStr( PVC );

	Data.append("PORT_NO", LinkStr.c_str() );
	Data.append("PVC_NO", PVCStr.c_str() );
    Sigo->Send( "TFX.UNI.PROD.XTC.RST", LinkStr, Data );

	printf("\nSend Reset PVC:%d to TFX.UNI.PROD.XTC.RST.\n",PVC);
}
//--------------------------------------------------------------------------------------------------------------------
void SendClosePVC()
{
	int Link,PVC;
	MTree Data;

	printf("Link Number:");
	scanf( "%d", &Link );

	printf("PVC Number:");
	scanf( "%d", &PVC );

	AnsiString LinkStr( Link );
	AnsiString PVCStr( PVC );
	AnsiString BkType( "P" );
	AnsiString FuncType( "C" );

	Data.append("BACKUP_TYPE", BkType.c_str() );
	Data.append("FUNC_TYPE", FuncType.c_str() );
	Data.append("PVC_NO", PVCStr.c_str() );
	Sigo->Send( "TFX.UNI.PROD.XTC.BAK", LinkStr, Data );

	printf("\nSend Close PVC:%d to TFX.UNI.PROD.XTC.BAK.\n",PVC);
}
//--------------------------------------------------------------------------------------------------------------------
void SendCreatePVC()
{
	int Link,PVC;
	char TmpStr[20];
        MTree Data;

	printf("Link Number:");
	scanf( "%d", &Link );

	printf("PVC Number:");
	scanf( "%d", &PVC );

	printf("Broker ID:");
	scanf( "%s", TmpStr );
	AnsiString BrokerID( TmpStr );


	printf("Password:");
	scanf( "%s", TmpStr );
	AnsiString Password( TmpStr );

	AnsiString LinkStr( Link );
	AnsiString PVCStr( PVC );
	AnsiString BkType( "P" );
	AnsiString FuncType( "O" );

	Data.append("BACKUP_TYPE", BkType.c_str() );
	Data.append("FUNC_TYPE", FuncType.c_str() );
	Data.append("PVC_NO", PVCStr.c_str() );
	Data.append("BROKER_ID", BrokerID.c_str() );
	Data.append("PASSWORD", Password.c_str() );
	Sigo->Send( "TFX.UNI.PROD.XTC.BAK", LinkStr, Data );

	printf("\nSend Create PVC:%d to TFX.UNI.PROD.XTC.BAK.\n",PVC);
}
//------------------------------------------------------------------------------
int main( int argc, char** argv )
{
    MApp::SetMAppPort( 12345 );
    Sigo = new MessageObject( "SigoPublisher", "1.0", "Sigo Publisher test program." );
    Sigo->Start();
    int LoopCount;
    char Key[8];
    while( TRUE )
    {
        if( argc == 1 )
        {
            printf( "1. Send DB information.\n" );
            printf( "2. Send OpenSwitch.\n" );
            printf( "3. Reset PVC.\n" );
            printf( "4. Close PVC.\n" );
            printf( "5. Create PVC.\n" );
            scanf( "%s", Key );
            switch( Key[0] )
            {
                case '1': SendDBInfo();     break;
                case '2': SendOpenSwitch(); break;
                case '3': SendPVCReset();   break;
                case '4': SendClosePVC();   break;
                case '5': SendCreatePVC();   break;
            }
        }
        else if( argc == 4 )
        {
            MTree Data;
            Data.append("Data",argv[3]);
            printf("Send %s to %s %s\n",argv[3],argv[1],argv[2]);
            Sigo->Send( argv[1],argv[2], Data );
            sleep( 5 );
        }
        else
        {
            LoopCount = 0;
            while( LoopCount < 200 )
            {
                SendDBInfo();
                LoopCount++;
            }
            UFC::SleepMS( 1 );
        }

    }
//  delete Sigo;
}
//--------------------------------------------------------------------------------------------------------------------
