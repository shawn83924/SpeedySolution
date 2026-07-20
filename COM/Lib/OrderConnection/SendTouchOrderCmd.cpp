#include "TTaifexConnection.h"

const UFC::AnsiString SUBJECT_TOUCH_REQUEST = "TOUCH.REQUEST";// added bu Kenny to support Touch Order. 2026/03/12

BOOL TTaifexConnection::TouchOrderControl(TTouchOrderCommand* toc)
{
    TTouchOrderCommand::TouchedOrderCommandEnum cmd = toc->GetCmdType();    

    Glog->fprintf("----- TouchOrder Control ------------");
    Glog->fprintf("  [CmdType]:%d", cmd);
    
    if (!toc->Verify())
    {
        Glog->fprintf("  [EORROR]:%s", toc->GetLastErrorMsg());
        return FALSE;
    }

    std::string cmd_node;    
    
    if (TTouchOrderCommand::tocNew != cmd)
    {
        switch (cmd)
        {
        case TTouchOrderCommand::tocPause:
            cmd_node = "act=P";
            break;
        case TTouchOrderCommand::tocActive:
            cmd_node = "act=A";
            break;
        case TTouchOrderCommand::tocKill:
            cmd_node = "act=K";
            break;
        case TTouchOrderCommand::tocQuery:
            cmd_node = "act=Q";
            break;
        default:
            toc->SetLastErrorMsg("Unknown or unsupported command type!");
            return FALSE;
        }

        std::string toid = toc->GetTouchOrderID();
        if (!toid.empty())
        {
            cmd_node += "^toid=";
            cmd_node += toid;
        }

        MTHandle        MHandle;
        MApp* MBusClient = FTransport->GetMApp();
        MBusClient->BeginSend(MHandle, SUBJECT_TOUCH_REQUEST, FID);
        MBusClient->WriteString(MHandle, "ID", FID);
        Glog->fprintf("  [ID]:%s", FID.c_str());
        
        MBusClient->WriteInt32(MHandle, "CID", FCurrentConnectionID);
        MBusClient->WriteString(MHandle, "COMMAND", cmd_node.c_str());
        Glog->fprintf("  [COMMAND]:%s", cmd_node.c_str());

        return MBusClient->EndSend(MHandle);
    }

    // verify the symbol of touch order
    nsOrderMessageDefine::MarketEnum market = toc->GetMarket();
    std::string symbol = toc->GetSymbol();
    if (nsOrderMessageDefine::mTWFutures == market ||
        nsOrderMessageDefine::mTWOptions == market)
    {
        int decimal_locator = GetTAIFEXPricePrecision(market, symbol.c_str());
        if (decimal_locator < 0)
        {
            toc->SetLastErrorMsg("The product-id of futures/options touch order is not exist!");
            Glog->fprintf("  [EORROR]:%s", toc->GetLastErrorMsg());
            return FALSE;
        }
    }

    // verify the symbol of order to execute if touched
    TTouchOrderCommand::TouchedActionEnum tact_type = toc->GetTouchedActionType();
    if (TTouchOrderCommand::taWarning != tact_type)
    {
        nsOrderMessageDefine::MarketEnum act_market = toc->GetTouchedActionMarket();
        std::string act_symbol = toc->GetTouchedActionSymbol();        
        if (nsOrderMessageDefine::mTWFutures == act_market ||
            nsOrderMessageDefine::mTWOptions == act_market)
        {
            int decimal_locator = GetTAIFEXPricePrecision(act_market, act_symbol.c_str());
            if (decimal_locator < 0)
            {
                toc->SetLastErrorMsg("The product-id of futures/options touched-to-execute order is not exist!");
                Glog->fprintf("  [EORROR]:%s", toc->GetLastErrorMsg());
                return FALSE;
            }
        }
    }    

    cmd_node = "act=N";
    std::string action = toc->GetTriggeredAction();
    std::string scene = toc->GetTriggeringCondition();
    std::string expression = toc->GetTriggerExpression();
    
    long long NID = GenerateNID(nsOrderMessageDefine::mtNew);
    toc->SetNID(NID);
    
    MTHandle        MHandle;
    MApp* MBusClient = FTransport->GetMApp();
    MBusClient->BeginSend(MHandle, SUBJECT_TOUCH_REQUEST, FID);
    if (nsOrderMessageDefine::mTSE == market)
    {
        MBusClient->WriteString(MHandle, "MARKET", "TSE");
        Glog->fprintf("  [MARKET]:TSE");
    }
    else if (nsOrderMessageDefine::mOTC == market)
    {
        MBusClient->WriteString(MHandle, "MARKET", "OTC");
        Glog->fprintf("  [MARKET]:OTC");
    }
    else if (nsOrderMessageDefine::mTWFutures == market)
    {
        MBusClient->WriteString(MHandle, "MARKET", "FUTURES");
        Glog->fprintf("  [MARKET]:FUTURES");
    }
    else if (nsOrderMessageDefine::mTWOptions == market)
    {
        MBusClient->WriteString(MHandle, "MARKET", "OPTIONS");
        Glog->fprintf("  [MARKET]:OPTIONS");
    }

    MBusClient->WriteString(MHandle, "ID", FID);
    Glog->fprintf("  [ID]:%s", FID.c_str());
    
    MBusClient->WriteInt64(MHandle, "NID", NID);
    Glog->fprintf("  [NID]:%lld", NID);
        
    MBusClient->WriteInt32(MHandle, "CID", FCurrentConnectionID);
    MBusClient->WriteString(MHandle, "COMMAND", cmd_node.c_str());
    Glog->fprintf("  [COMMAND]:%s", cmd_node.c_str());    
    
    MBusClient->WriteString(MHandle, "SYMBOL", symbol.c_str());
    Glog->fprintf("  [SYMBOL]:%s", symbol.c_str());

    //MBusClient->WriteInt32(MHandle, "DECIMAL_LOCATOR", decimal_locator);
    //Glog->fprintf("  [DECIMAL_LOCATOR]:%d", decimal_locator);

    std::string user_data = toc->GetUserData();
    if (!user_data.empty())
    {
        MBusClient->WriteString(MHandle, "USER_DATA", user_data.c_str());
        Glog->fprintf("  [USER_DATA]:%s", user_data.c_str());
    }    
    
    MBusClient->WriteString(MHandle, "ACTION", action.c_str());
    MBusClient->WriteString(MHandle, "SCENE", scene.c_str());
    MBusClient->WriteString(MHandle, "EXPRESSION", expression.c_str());

    Glog->fprintf("  [ACTION]:%s", action.c_str());
    Glog->fprintf("  [SCENE]:%s", scene.c_str());
    Glog->fprintf("  [EXPRESSION]:%s", expression.c_str());
    
    return MBusClient->EndSend(MHandle);
}