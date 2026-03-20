#include "TTaifexConnection.h"

const UFC::AnsiString SUBJECT_TOUCH_REQUEST = "TOUCH.REQUEST";// added bu Kenny to support Touch Order. 2026/03/12

BOOL TTaifexConnection::TouchOrderControl(TTouchOrderCommand* toc)
{
    TTouchOrderCommand::TouchedOrderCommandEnum cmd = toc->GetCmdType();
    
    std::string cmd_node;
    std::string action;
    std::string scene;

    if (TTouchOrderCommand::TouchedOrderCommandEnum::tocNew == cmd)
    {
        cmd_node = "act=N";
        action = toc->GetTriggeredAction();
        scene = toc->GetTriggeringCondition();

        if (action.empty() || scene.empty())
            return FALSE;        
    }
    else
    {
        switch (cmd)
        {
        case TTouchOrderCommand::TouchedOrderCommandEnum::tocPause:
            cmd_node = "act=P";
            break;
        case TTouchOrderCommand::TouchedOrderCommandEnum::tocActive:
            cmd_node = "act=A";
            break;
        case TTouchOrderCommand::TouchedOrderCommandEnum::tocKill:
            cmd_node = "act=K";
            break;
        case TTouchOrderCommand::TouchedOrderCommandEnum::tocQuery:
            cmd_node = "act=Q";
            break;
        default:
            return FALSE;
        }

        std::string toid = toc->GetTouchOrderID();
        if (!toid.empty())
        {
            cmd_node += "toid=";
            cmd_node += toid;
        }
    }

    nsOrderMessageDefine::MarketEnum market = toc->GetMarket();
    if (nsOrderMessageDefine::mTSE != market && nsOrderMessageDefine::mOTC != market)
    {
        return FALSE;
    }

    long long NID = GenerateNID(nsOrderMessageDefine::MessageTypeEnum::mtNew);
    toc->SetNID(NID); 

    MTHandle        MHandle;
    MApp* MBusClient = FTransport->GetMApp();
    MBusClient->BeginSend(MHandle, SUBJECT_TOUCH_REQUEST, FID);
    if (nsOrderMessageDefine::mTSE == market)
        MBusClient->WriteString(MHandle, "MARKET", "TSE");
    else
        MBusClient->WriteString(MHandle, "MARKET", "OTC");

    MBusClient->WriteString(MHandle, "ID", FID);
    MBusClient->WriteInt64(MHandle, "NID", NID);
    MBusClient->WriteInt32(MHandle, "CID", FCurrentConnectionID);
    MBusClient->WriteString(MHandle, "COMMAND", cmd_node.c_str());
    
    if (TTouchOrderCommand::TouchedOrderCommandEnum::tocNew == cmd)
    {
        std::string user_data = toc->GetUserData();
        if (!user_data.empty())
            MBusClient->WriteString(MHandle, "USER_DATA", user_data.c_str());
    }

    if (!action.empty() && !scene.empty())
    {
        MBusClient->WriteString(MHandle, "ACTION", action.c_str());
        MBusClient->WriteString(MHandle, "SCENE", scene.c_str());
    }
    return MBusClient->EndSend(MHandle);
}