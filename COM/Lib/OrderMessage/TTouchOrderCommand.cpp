#include "TTouchOrderCommand.h"
#include <sstream>

void TTouchOrderCommand::ToTriggeredAction(TNewOrderMessage* order)
{
    FTriggeredAction.clear();
    FTriggeredAction.insert(std::make_pair("fn", "N"));
    // 目前僅支援證劵下單
    nsOrderMessageDefine::MarketEnum market = order->GetMarket();
    nsOrderMessageDefine::TradingSessionIDEnum trading_session = order->GetTradingSessionID();
    const char* broker_id = order->GetBrokerID();                           // BORKER-ID
    if(broker_id)
        FTriggeredAction.insert(std::make_pair("bk", broker_id));
    const char* account = order->GetAccount();                              // IVACNO
    if (account)
        FTriggeredAction.insert(std::make_pair("acc", account));
    const char* account_flag = order->GetAccountFlag();                     // IVACNO-FLAG
    if (account_flag)
        FTriggeredAction.insert(std::make_pair("af", account_flag));
    const char* symbol = order->GetSymbol();                                // STOCK-NO
    if (symbol)
        FTriggeredAction.insert(std::make_pair("sym", symbol));
    std::string s_price = std::to_string(order->GetPrice());                // PRICE
    FTriggeredAction.insert(std::make_pair("px", s_price));
    std::string s_qty = std::to_string(order->GetOrderQty());               // QUANTITY
    FTriggeredAction.insert(std::make_pair("qty", s_qty));
    nsOrderMessageDefine::SideEnum side = order->GetSide();                 // BUY-SELL-CODE
    if (nsOrderMessageDefine::SideEnum::sBuy == side)
        FTriggeredAction.insert(std::make_pair("bs", "B"));
    else
        FTriggeredAction.insert(std::make_pair("bs", "S"));
    const char* twse_exchange_code = order->GetTSEExchangeCode();           // EXCHANGE-CODE
    if (twse_exchange_code)
        FTriggeredAction.insert(std::make_pair("exc", twse_exchange_code));
    const char* twse_order_type = order->GetTSEOrderType();                 // ORDER-TYPE
    if (twse_order_type)
        FTriggeredAction.insert(std::make_pair("tot", twse_order_type));
    nsOrderMessageDefine::OrderTypeEnum price_type = order->GetOrderType(); // PRICE-TYPE
    if (nsOrderMessageDefine::OrderTypeEnum::otLimit == price_type)
        FTriggeredAction.insert(std::make_pair("ot", "L"));
    else
        FTriggeredAction.insert(std::make_pair("ot", "M"));
    nsOrderMessageDefine::TimeInForceEnum tif = order->GetTimeInForce();    // TIME-IN-FORCE
    if (nsOrderMessageDefine::TimeInForceEnum::tifIOC == tif)
        FTriggeredAction.insert(std::make_pair("tif", "I"));
    else if (nsOrderMessageDefine::TimeInForceEnum::tifFOK == tif)
        FTriggeredAction.insert(std::make_pair("tif", "F"));
    else
        FTriggeredAction.insert(std::make_pair("tif", "R"));
}

void TTouchOrderCommand::ToTriggeredAction(TCancelOrderMessage* order)
{
    FTriggeredAction.clear();
    FTriggeredAction.insert(std::make_pair("fn", "C"));
    
    // TODO ...目前不需要，未來擴增可用
}

void TTouchOrderCommand::ToTriggeredAction(TReplaceOrderMessage* order)
{
    FTriggeredAction.clear();
    FTriggeredAction.insert(std::make_pair("fn", "R"));
    
    // TODO ...目前不需要，未來擴增可用
}

void TTouchOrderCommand::ToTriggeredAction(TTouchOrderCommand* toc)
{
    FTriggeredAction.clear();
    
    // 目前只支援刪觸價單
    if (toc && 
        TTouchOrderCommand::TouchedOrderCommandEnum::tocKill == toc->FCmdType)
    {
        FTriggeredAction.insert(std::make_pair("fn", "T"));
        FTriggeredAction.insert(std::make_pair("act", "K"));
        if (nsOrderMessageDefine::SideEnum::sBuy == toc->FSide)
            FTriggeredAction.insert(std::make_pair("bs", "B"));
        else if(nsOrderMessageDefine::SideEnum::sSell == toc->FSide)
            FTriggeredAction.insert(std::make_pair("bs", "S"));
        else
            FTriggeredAction.insert(std::make_pair("bs", "A"));
    }
}

void TTouchOrderCommand::ToTriggeredAction(const char* warning)
{
    FTriggeredAction.clear();
    FTriggeredAction.insert(std::make_pair("fn", "W"));
    FTriggeredAction.insert(std::make_pair("msg", warning));    
}

void TTouchOrderCommand::SetTriggeringCondition(TTriggeringCondition* ttc)
{
    FTriggeringCondition = ttc->ToString();
}

std::string TTouchOrderCommand::GetTriggeredAction()
{
    if (FCmdType != TouchedOrderCommandEnum::tocNew)
        return "";

    std::map<std::string, std::string>::iterator iter = FTriggeredAction.find("fn");
    if ( iter == FTriggeredAction.end())
        return "";
    std::string fn = iter->second;

    std::stringstream ss;
    iter = FTriggeredAction.begin();
    bool first = true;
    for (; iter != FTriggeredAction.end(); ++iter)
    {
        if (first)
            first = false;
        else
            ss << "^";

        ss << iter->first << "=" << iter->second;
    }
    if (fn == "C")
    {
        if (nsOrderMessageDefine::SideEnum::sSell == FSide)
            ss << "^bs=S";
        else if (nsOrderMessageDefine::SideEnum::sBuy == FSide)
            ss << "^bs=B";
        else
            ss << "^bs=A";
        
        if (!FPrice.empty())
            ss << "^px=" << FPrice;
    }
    else if (fn == "R")
    {
        if (nsOrderMessageDefine::SideEnum::sSell == FSide)
            ss << "^bs=S";
        else
            ss << "^bs=B";
    }

    return ss.str();
}

