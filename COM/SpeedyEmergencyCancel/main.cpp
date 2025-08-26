/* 
 * File:   main.cpp
 * Author: yuan
 *
 * Created on May 26, 2009, 2:47 PM
 */

#include <stdlib.h>
#include <stdio.h>

#include "UFC.h"
//#include "AnsiString.h"
//#include "UDateTime.h"
//#include "FileStream.h"
//#include "PThread.h"
//#include "List.h"
//#include "NetUtility.h"
//#include "iniFile.h"

//#include "../Lib/OrderConnection/TTaifexConnection.h"
//#include "../Lib/EmbadedResource.h"
#include "../Lib/OrderMessage/OrderMessageDefine.h"
#include "../Lib/OrderMessage/TCancelOrderMessage.h"
#include "../Lib/OrderMessage/TQuoteCancelMessage.h"
#include "CancelOrderConnection.h"

UFC::AnsiString ConfigFileName;
UFC::UiniFile*  pConfig;
UFC::AnsiString ApplicationName;
UFC::AnsiString ServerIP;
UFC::AnsiString ServerUserID;
UFC::AnsiString ServerPassword;
UFC::AnsiString ServerAccount;
UFC::AnsiString ServerBrokerID;
UFC::AnsiString CancelDataFilePath;

UFC::AnsiString ServerPortString;
long            ServerPort;

UFC::AnsiString CancelIntervalString;
long            CancelInterval;

nsOrderMessageDefine::MarketEnum MarketType;
//------------------------------------------------------------------------------
void LoadSetting()
{
    pConfig = new UFC::UiniFile(ConfigFileName);

    if (pConfig->GetValue("Setting", "AppName", ApplicationName) == FALSE)
        ApplicationName.Printf("SpeedyEmergencyCancel");

    if (pConfig->GetValue( "Setting", "IP", ServerIP ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Speedy Server IP not set.");
        exit(1);
    }

    if (pConfig->GetValue( "Setting", "Port", ServerPortString ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Speedy Server Port not set.");
        exit(1);
    }
    ServerPort = ServerPortString.ToInt();

    if (pConfig->GetValue( "Setting", "ID", ServerUserID ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Logon ID not set.");
        exit(1);
    }

    if (pConfig->GetValue( "Setting", "Password", ServerPassword ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Logon Password not set.");
        exit(1);
    }

    if (pConfig->GetValue( "Setting", "Account", ServerAccount ) == FALSE )
    {
        UFC::BufferedLog::Printf(" Logon Account not set.");
        exit(1);
    }

    if (pConfig->GetValue( "Setting", "BrokerID", ServerBrokerID ) == FALSE )
    {
        UFC::BufferedLog::Printf(" BrokerID not set.");
        exit(1);
    }

    CancelInterval = 0;
    if (pConfig->GetValue( "Setting", "CancelInterval", CancelIntervalString ) == FALSE )
    {
        UFC::BufferedLog::Printf(" CancelInterval not set.");
        exit(1);
    }
    CancelInterval = CancelIntervalString.ToInt();

    CancelDataFilePath.Printf("/Speedy/bin/");
    if (pConfig->GetValue( "Setting", "CancelDataFilePath", CancelDataFilePath ) == FALSE )
    {
        UFC::BufferedLog::Printf(" CancelDataFilePath not set.");
        exit(1);
    }
    if (CancelDataFilePath[CancelDataFilePath.Length()] != '/')
        CancelDataFilePath = CancelDataFilePath + "/";
}  //LoadSetting
//------------------------------------------------------------------------------
void PrintHelp( void )
{
    printf( "SpeedyEmergencyCancel Copyright (c) September 2008~%d MDBS\n", UFC::ThisYear() );
    printf( "SpeedyEmergencyCancel -M[O/F] \n" );
    printf( "    -M[O/F]            Market Type. O: Options, F: Futures\n" );
    exit( 1 );
}  //PrintHelp
//------------------------------------------------------------------------------
void CheckArgs( int argc, char *argv[] )
{
    if (argc != 2)
    {
        PrintHelp();
    }
    MarketType = nsOrderMessageDefine::mOTC;
    for (int i = 1; i < argc; i++)
    {
        UFC::AnsiString Param(argv[i]);
        if( Param.AnsiPos("-M") != -1 )
        {
            if (Param.Length() != 3)
            {
                printf( "Parameter [%s] is wrong, must be -MO or -MF.\n", Param.c_str());
                PrintHelp();
            }
            else
            {
                if( Param[2] == 'O')
                    MarketType = nsOrderMessageDefine::mTWOptions;
                else if( Param[2] == 'F')
                    MarketType = nsOrderMessageDefine::mTWFutures;
                else
                {
                    printf( "Parameter [%s] is wrong, must be -MO or -MF.\n", Param.c_str());
                    PrintHelp();
                }
            }
        }
    }  //for (int i = 1; i < argc; i++)

    if ((MarketType != nsOrderMessageDefine::mTWOptions) && (MarketType != nsOrderMessageDefine::mTWFutures))
    {
        printf( "No Parameter -M[O/F], must be -MO or -MF.\n");
        PrintHelp();
    }
}  //CheckArgs()

//------------------------------------------------------------------------------
/*
BOOL ParseDataToCancelMessage(UFC::AnsiString& aCancelRecord, nsOrderMessageDefine::MarketEnum MsgMarketType,
                              TCancelOrderMessage& CancelMsg)
{
    if (aCancelRecord[aCancelRecord.Length() - 1] == '\n')
        aCancelRecord[aCancelRecord.Length() - 1] = 0;
    else
        return FALSE;

    UFC::PStringList DataList;
    DataList.SetStrings(aCancelRecord, ",");
    if (DataList.ItemCount() >= 3)
    {
for (int i = 0; i < DataList.ItemCount(); i++)
{
    printf("[%s]", DataList.GetItem(i).c_str());
}
printf("\n");

        CancelMsg.SetMarket(MsgMarketType);
        CancelMsg.SetTradingSessionID(nsOrderMessageDefine::tsNormal);
        CancelMsg.SetSymbol(DataList.GetItem(2).c_str());
        CancelMsg.SetOrderID(DataList.GetItem(1).c_str());
        CancelMsg.SetBrokerID(DataList.GetItem(0).c_str());

        if (DataList.ItemCount() >= 4)
        {
            char BorS = (DataList.GetItem(3))[0];
            if (BorS == 'B')
                CancelMsg.SetSide(nsOrderMessageDefine::sBuy);
            else
                CancelMsg.SetSide(nsOrderMessageDefine::sSell);
        }  //if (DataList.ItemCount() >= 4)

        return TRUE;
    }  //if (DataList.ItemCount() >= 3)

    return FALSE;
}  //ParseDataToCancelMessage()
*/

/*
 * 
 */
int main(int argc, char** argv)
{
    ConfigFileName.Printf("../cfg/SpeedyEmergencyCancel.cfg");
    UFC::BufferedLog::SetLogObject(new UFC::BufferedLog("../log/SpeedyEmergencyCancel.log", 1024, TRUE));
    LoadSetting();
    //CheckArgs(argc, argv);
    MarketType = nsOrderMessageDefine::mTWFutures;

    UFC::UDateTime ProgramBeginTime;
    ProgramBeginTime.setCurrent();

/*
    UFC::AnsiString CancelDataFileName;
    if (MarketType == nsOrderMessageDefine::mOptions)
        CancelDataFileName.Printf("/Speedy/bin/SpeedyEmergencyCancel.OPT.%04d%02d%02d.txt",
                                  ProgramBeginTime.getYear(), ProgramBeginTime.getMonth(), ProgramBeginTime.getDate());
    else
        CancelDataFileName.Printf("/Speedy/bin/SpeedyEmergencyCancel.FUT.%04d%02d%02d.txt",
                                  ProgramBeginTime.getYear(), ProgramBeginTime.getMonth(), ProgramBeginTime.getDate());
 */
    UFC::AnsiString OptCancelDataFileName;
    OptCancelDataFileName.Printf("%sSpeedyEmergencyCancel.OPT.%04d%02d%02d.txt",
                                 CancelDataFilePath.c_str(), ProgramBeginTime.getYear(), ProgramBeginTime.getMonth(), ProgramBeginTime.getDate());

    UFC::AnsiString FutCancelDataFileName;
    FutCancelDataFileName.Printf("%sSpeedyEmergencyCancel.FUT.%04d%02d%02d.txt",
                                 CancelDataFilePath.c_str(), ProgramBeginTime.getYear(), ProgramBeginTime.getMonth(), ProgramBeginTime.getDate());

    UFC::BufferedLog::Printf("----------------------------------------------------------------------------");
    UFC::BufferedLog::Printf("SpeedyEmergencyCancel Begin at %04d/%02d/%02d %02d:%02d:%02d.....",
                             ProgramBeginTime.getYear(), ProgramBeginTime.getMonth(), ProgramBeginTime.getDate(),
                             ProgramBeginTime.getHour(), ProgramBeginTime.getMinute(), ProgramBeginTime.getSecond());
    UFC::BufferedLog::Printf("    Options Data File Name [%s]", OptCancelDataFileName.c_str());
    UFC::BufferedLog::Printf("    Futures Data File Name [%s]", FutCancelDataFileName.c_str());
    UFC::BufferedLog::Printf("----------------------------------------------------------------------------");

    UFC::FileStreamEx* OptCancelDataFile;
    UFC::FileStreamEx* FutCancelDataFile;
    try
    {
        OptCancelDataFile = new UFC::FileStreamEx(OptCancelDataFileName, "a+");
        FutCancelDataFile = new UFC::FileStreamEx(FutCancelDataFileName, "a+");
    }
    catch (UFC::FileException& e)
    {
        UFC::BufferedLog::Printf("    File Exception [%s]", e.what());
        return -1;
    }  //try

    OptCancelDataFile->Seek(0, UFC::soFromBeginning);
    FutCancelDataFile->Seek(0, UFC::soFromBeginning);

    CancelOrderConnection* OrderConnection = new CancelOrderConnection( 0 );
//   OrderConnection->Cancel();

    while(TRUE)
    {
        if (OrderConnection->IsLogon() == TRUE)
        {
            char LastChar;

            UFC::AnsiString aOptCancelRecord;
            BOOL OptNotEOF;
            int OptPosition = OptCancelDataFile->GetPosition();
            OptNotEOF = OptCancelDataFile->ReadLine(aOptCancelRecord);
            if (aOptCancelRecord.Length() > 0)
            {
                LastChar = aOptCancelRecord[aOptCancelRecord.Length() - 1];
                if (LastChar != '\n')  //incomplete Record
                {
                    OptCancelDataFile->Seek(OptPosition, UFC::soFromBeginning);
                    OptNotEOF = TRUE;
                }
                else  //complete Record
                {
/*
    if (aCancelRecord[aCancelRecord.Length() - 1] == '\n')
        aCancelRecord[aCancelRecord.Length() - 1] = 0;
    else
        return FALSE;

    UFC::PStringList DataList;
    DataList.SetStrings(aCancelRecord, ",");
    if (DataList.ItemCount() >= 3)
    {
for (int i = 0; i < DataList.ItemCount(); i++)
{
    printf("[%s]", DataList.GetItem(i).c_str());
}
printf("\n");

        CancelMsg.SetMarket(MsgMarketType);
        CancelMsg.SetTradingSessionID(nsOrderMessageDefine::tsNormal);
        CancelMsg.SetSymbol(DataList.GetItem(2).c_str());
        CancelMsg.SetOrderID(DataList.GetItem(1).c_str());
        CancelMsg.SetBrokerID(DataList.GetItem(0).c_str());

        if (DataList.ItemCount() >= 4)
        {
            char BorS = (DataList.GetItem(3))[0];
            if (BorS == 'B')
                CancelMsg.SetSide(nsOrderMessageDefine::sBuy);
            else
                CancelMsg.SetSide(nsOrderMessageDefine::sSell);
        }  //if (DataList.ItemCount() >= 4)

        return TRUE;
    }  //if (DataList.ItemCount() >= 3)

 */
                    OptCancelDataFile->Seek(OptPosition + aOptCancelRecord.Length(), UFC::soFromBeginning);
                    aOptCancelRecord[aOptCancelRecord.Length() - 1] = 0;
//printf("opt Record[%s]\n", aOptCancelRecord.c_str());
                    UFC::PStringList optDataList;
                    optDataList.SetStrings(aOptCancelRecord, ",");
                    if (optDataList.ItemCount() >= 3) {
//for (int i = 0; i < optDataList.ItemCount(); i++)
//{
//    printf("[%s]", optDataList.GetItem(i).c_str());
//}
//printf("\n");
                        if (optDataList.ItemCount() == 3) {  //quote
                            TQuoteCancelMessage optQuoteCancelMsg;
                            optQuoteCancelMsg.SetMarket(nsOrderMessageDefine::mTWOptions);
                            optQuoteCancelMsg.SetSymbol(optDataList.GetItem(2).c_str());
                            optQuoteCancelMsg.SetQuoteID(optDataList.GetItem(1).c_str());
                            optQuoteCancelMsg.SetBrokerID(optDataList.GetItem(0).c_str());
                            OrderConnection->QuoteCancel(optQuoteCancelMsg);
                        }
                        else if (optDataList.ItemCount() == 4)
                        {
                            TCancelOrderMessage optCancelMsg;
                            optCancelMsg.SetMarket(nsOrderMessageDefine::mTWOptions);
                            optCancelMsg.SetTradingSessionID(nsOrderMessageDefine::tsNormal);
                            optCancelMsg.SetSymbol(optDataList.GetItem(2).c_str());
                            optCancelMsg.SetOrderID(optDataList.GetItem(1).c_str());
                            optCancelMsg.SetBrokerID(optDataList.GetItem(0).c_str());
                            char BorS = (optDataList.GetItem(3))[0];
                            if (BorS == 'B')
                                optCancelMsg.SetSide(nsOrderMessageDefine::sBuy);
                            else
                                optCancelMsg.SetSide(nsOrderMessageDefine::sSell);

                            OrderConnection->Cancel(optCancelMsg);
                        }  //if (optDataList.ItemCount() == 3)

                        if (CancelInterval > 0)
                            UFC::SleepMS(CancelInterval);
                    }  //if (optDataList.ItemCount() >= 3)



/*
                    if (ParseDataToCancelMessage(aOptCancelRecord, nsOrderMessageDefine::mOptions, CancelMsg) == TRUE)
                    {
                        OrderConnection->Cancel(CancelMsg);

                        if (CancelInterval > 0)
                            UFC::SleepMS(CancelInterval);
                    }
 */
                }  //if (LastChar != '\n')
            }  //if (aOptCancelRecord.Length() > 0)

            UFC::AnsiString aFutCancelRecord;
            BOOL FutNotEOF;
            int FutPosition = FutCancelDataFile->GetPosition();
            FutNotEOF = FutCancelDataFile->ReadLine(aFutCancelRecord);
            if (aFutCancelRecord.Length() > 0)
            {
                LastChar = aFutCancelRecord[aFutCancelRecord.Length() - 1];
                if (LastChar != '\n')  //incomplete Record
                {
                    FutCancelDataFile->Seek(FutPosition, UFC::soFromBeginning);
                    FutNotEOF = TRUE;
                }
                else  //complete Record
                {
                    FutCancelDataFile->Seek(FutPosition + aFutCancelRecord.Length(), UFC::soFromBeginning);
                    aFutCancelRecord[aFutCancelRecord.Length() - 1] = 0;
                    UFC::PStringList futDataList;
                    futDataList.SetStrings(aFutCancelRecord, ",");
                    if (futDataList.ItemCount() >= 3) {
//printf("fut Record[%s]\n", aFutCancelRecord.c_str());
//for (int i = 0; i < futDataList.ItemCount(); i++)
//{
//    printf("[%s]", futDataList.GetItem(i).c_str());
//}
//printf("\n");
                        if (futDataList.ItemCount() == 3) {  //quote
                            TQuoteCancelMessage futQuoteCancelMsg;
                            futQuoteCancelMsg.SetMarket(nsOrderMessageDefine::mTWFutures);
                            futQuoteCancelMsg.SetSymbol(futDataList.GetItem(2).c_str());
                            futQuoteCancelMsg.SetQuoteID(futDataList.GetItem(1).c_str());
                            futQuoteCancelMsg.SetBrokerID(futDataList.GetItem(0).c_str());
                            OrderConnection->QuoteCancel(futQuoteCancelMsg);
                        }
                        else if (futDataList.ItemCount() == 4)
                        {
                            TCancelOrderMessage futCancelMsg;
                            futCancelMsg.SetMarket(nsOrderMessageDefine::mTWFutures);
                            futCancelMsg.SetTradingSessionID(nsOrderMessageDefine::tsNormal);
                            futCancelMsg.SetSymbol(futDataList.GetItem(2).c_str());
                            futCancelMsg.SetOrderID(futDataList.GetItem(1).c_str());
                            futCancelMsg.SetBrokerID(futDataList.GetItem(0).c_str());
                            char BorS = (futDataList.GetItem(3))[0];
                            if (BorS == 'B')
                                futCancelMsg.SetSide(nsOrderMessageDefine::sBuy);
                            else
                                futCancelMsg.SetSide(nsOrderMessageDefine::sSell);

                            OrderConnection->Cancel(futCancelMsg);
                        }  //if (futDataList.ItemCount() == 3)

                        if (CancelInterval > 0)
                            UFC::SleepMS(CancelInterval);
                    }  //if (futDataList.ItemCount() >= 3)



/*
                    TCancelOrderMessage CancelMsg;
                    if (ParseDataToCancelMessage(aFutCancelRecord, nsOrderMessageDefine::mFutures, CancelMsg) == TRUE)
                    {
                        OrderConnection->Cancel(CancelMsg);

                        if (CancelInterval > 0)
                            UFC::SleepMS(CancelInterval);
                    }
 */
                }  //if (LastChar != '\n')
            }  //if (aFutCancelRecord.Length() > 0)

            if ((OptNotEOF == FALSE) && (FutNotEOF == FALSE))
            {
printf("Waiting For New Entry.... \n");
                UFC::SleepMS(5000);
            }  //if ((OptNotEOF == FALSE) && (FutNotEOF == FALSE))
        }
        else
        {
printf("Waiting For Logon.... \n");
            UFC::SleepMS(10000);
        }  //if (OrderConnection->IsLogon() == TRUE)
    }  //while(TRUE)

    return (EXIT_SUCCESS);
}  //main()

