#include "TTouchOrderCommand.h"
#include <sstream>

// 原意為設定觸價後刪委託單/改價/刪觸價單時所針對的委託/觸價單買賣別，現在保留介面供其他用途，請透過下列方式指定買賣別
//   .觸價後刪委託單請透過 TCancelOrderMessage 物件的 SetSide() 來指定
//   .觸價後改價請透過 TReplaceOrderMessage 物件的 SetSide() 來指定
//   .觸價後刪觸價單請透過 TTouchOrderCommand 物件的 SetSide() 來指定
void TTouchOrderCommand::SetSide(nsOrderMessageDefine::SideEnum side)
{
    FSide = side;
}

// 設定暫停、啟動、移除跟查詢時指定的觸價單單號
// 若是用在觸價後刪觸價單則是用來設定要刪除的觸價單的單號，在這種情況下
//   SetTouchOrderID 所設定的單號會優先於 SetTriggeredAction 裡 TTouchOrderCommand 物件所設定的單號 
void TTouchOrderCommand::SetTouchOrderID(const char* id)
{
    if (!id || !id[0])
    {
        FTouchOrderID.clear();
        return;
    }

    FTouchOrderID = id;
    std::map<std::string, std::string>::iterator iter = FTriggeredAction.find("action");
    if (iter == FTriggeredAction.end() || iter->second != "touchorder_cancel")
        return;

    FTriggeredAction["order_no"] = id;
}

// 若是觸價後刪單，委託單的委託價的價格等於 SetPrice() 的指定價格才會被刪除
// 若是觸價後改單，改價委託的委託價會是 SetPrice() 指定的價格
// 若是觸價後下新單，新單委託的委託價會是 SetPrice() 指定的價格
// 使用下列方法設定指定價格也可以，但浮點數轉字串時可能出現精確度的問題導致價格與預期的不同
//   TCancelOrderMessage::SetPrice()
//   TReplaceOrderMessage::SetPrice()
//   TNewOrderMessage::SetPrice()
// 若是觸價後下新單跟觸價後下改單的設定價格方式有3種，優先順序為
//   1. 透過 SetFloatingOrderPrice() 設定委託價是成交價的加減檔位數
//      設定 PriceDependOnEnum::pdoNone 表示決定優先順序時跳過 1.這一項 (預設值)
//   2. 透過 SetPrice() 設定,且用字串指定的數值是正常的數值格式 NNNNN.NNNN or NNNNN N 為 0~9 的數值
//      。整數大於5位時左方數字會被切除，小數部位大於4位時右方數字會被切除。
//      輸入空字串 ""、"0" 或錯誤格式的數值表示決定優先順序時跳過 2.這一項。(預設值)
//   3. 透過 TReplaceOrderMessage::SetPrice() 或 TNewOrderMessage::SetPrice() 設定的數值
void TTouchOrderCommand::SetPrice(const char* price)
{
    if (!price || !price[0])
    {
        FPrice.clear();
        return;
    }

    // 確認一下 price 是否是正確的數值格式
    try
    {
        double dbl_price = std::stod(price);
        if (dbl_price == 0.0)
        {
            FPrice.clear();
            return;
        }
    }
    catch (...)
    {
        FPrice.clear();
        return;
    }

    FPrice = price;

    if (PriceDependOnEnum::pdoMatch == FPriceDependOn)
        return;

    std::map<std::string, std::string>::iterator iter = FTriggeredAction.find("action");
    if (iter == FTriggeredAction.end() || 
        (iter->second != "twse_cancel" && 
         iter->second != "twse_replace" && 
         iter->second != "twse_new"))
        return;

    FTriggeredAction["price"] = price;
}

// 設定觸價下新單或觸價下改單時是以觸價時的成交價加減檔位為委託價                                                                         
// 設定價格方式有3種，優先順序為
//   1. 透過 SetFloatingOrderPrice() 設定委託價是成交價的加減檔位數
//      設定 PriceDependOnEnum::pdoNone 表示決定優先順序時跳過 1.這一項 (預設值)
//   2. 透過 SetPrice() 設定,且用字串指定的數值是正常的數值格式 NNNNN.NNNN or NNNNN N 為 0~9 的數值
//      。整數大於5位時左方數字會被切除，小數部位大於4位時右方數字會被切除。
//      輸入空字串 ""、"0" 或錯誤格式的數值表示決定優先順序時跳過 2.這一項。(預設值)
//   3. 透過 TReplaceOrderMessage::SetPrice() 或 TNewOrderMessage::SetPrice() 設定的數值
void TTouchOrderCommand::SetFloatingOrderPrice(PriceDependOnEnum depend_on, int ticks)             
{
    FPriceDependOn = depend_on;
    FTicks = ticks;

    if (PriceDependOnEnum::pdoMatch != FPriceDependOn)
        return;

    std::map<std::string, std::string>::iterator iter = FTriggeredAction.find("action");
    if (iter == FTriggeredAction.end() || 
        (iter->second != "twse_new" && 
         iter->second != "twse_replace"))
        return;

    if (!FTicks)
        FTriggeredAction["price"] = "${match_price}";
    else
    {
        if (FTicks < 0)
            FTriggeredAction["price"] = std::string{ "${match_price}" } + std::to_string(FTicks);
        else
            FTriggeredAction["price"] = std::string{ "${match_price}+" } + std::to_string(FTicks);
    }
}

BOOL TTouchOrderCommand::ToTriggeredAction(TNewOrderMessage* order)
{
    FTriggeredAction.clear();
    
    FTriggeredAction.insert(std::make_pair("action", "twse_new"));

    // 目前僅支援證劵下單
    nsOrderMessageDefine::MarketEnum market = order->GetMarket();
    if (market != nsOrderMessageDefine::MarketEnum::mTSE && 
        market != nsOrderMessageDefine::MarketEnum::mOTC)
    {
        FTriggeredAction.clear();
        return FALSE;
    }

    // Note: 欄位命名根據證交所的「一般交易電腦作業手冊」裡格式 T010 的欄位名稱為基礎，並做下列變化
    //       1. 大寫英文改成小寫英文
    //       2. 符號 '-' 改成 '_'
    nsOrderMessageDefine::SideEnum side = order->GetSide();                 
    if (nsOrderMessageDefine::SideEnum::sBuy == side)
    {
        FTriggeredAction.insert(std::make_pair("function_code", "01"));
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "B"));
    }
    else
    {
        FTriggeredAction.insert(std::make_pair("function_code", "02"));
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "S"));
    }    
    
    // Note: Market 跟 Trading Session 會決定電文檔頭 sub-system 欄位的內容
    nsOrderMessageDefine::TradingSessionIDEnum trading_session = order->GetTradingSessionID();
    if (nsOrderMessageDefine::TradingSessionIDEnum::tsNormal == trading_session) //一般
    {
        if (nsOrderMessageDefine::MarketEnum::mTSE == market)
            FTriggeredAction.insert(std::make_pair("subsystem_name", "30"));
        else
            FTriggeredAction.insert(std::make_pair("subsystem_name", "93"));        
    }
    else if (nsOrderMessageDefine::TradingSessionIDEnum::tsIntradayOdd == trading_session) //盤中零股
    {
        if (nsOrderMessageDefine::MarketEnum::mTSE == market)
            FTriggeredAction.insert(std::make_pair("subsystem_name", "33"));
        else
            FTriggeredAction.insert(std::make_pair("subsystem_name", "83"));
    }
    else if (nsOrderMessageDefine::TradingSessionIDEnum::tsOffHour == trading_session) //盤後定價
    {
        if (nsOrderMessageDefine::MarketEnum::mTSE == market)
            FTriggeredAction.insert(std::make_pair("subsystem_name", "32"));
        else
            FTriggeredAction.insert(std::make_pair("subsystem_name", "98"));
    }
    else if (nsOrderMessageDefine::TradingSessionIDEnum::tsOddLot == trading_session) //盤後零股
    {
        if (nsOrderMessageDefine::MarketEnum::mTSE == market)
            FTriggeredAction.insert(std::make_pair("subsystem_name", "40"));
        else
            FTriggeredAction.insert(std::make_pair("subsystem_name", "94"));
    }
    else
    {
        FTriggeredAction.clear();
        return FALSE;
    }

    // BORKER-ID
    const char* broker_id = order->GetBrokerID();                           
    if (broker_id)
        FTriggeredAction.insert(std::make_pair("broker_id", broker_id));
    else
    {
        FTriggeredAction.clear();
        return FALSE;
    }

    // IVACNO
    const char* account = order->GetAccount();                              
    if (account)
        FTriggeredAction.insert(std::make_pair("ivacno", account));
    else
    {
        FTriggeredAction.clear();
        return FALSE;
    }

    // IVACNO-FLAG
    const char* account_flag = order->GetAccountFlag();                     
    if (account_flag)
        FTriggeredAction.insert(std::make_pair("ivacno_flag", account_flag));

    // STOCK-NO
    const char* symbol = order->GetSymbol();                                
    if (symbol)
        FTriggeredAction.insert(std::make_pair("stock_no", symbol));    

    // PRICE
    if (PriceDependOnEnum::pdoMatch == FPriceDependOn)
    {
        if (!FTicks)
            FTriggeredAction["price"] = "${match_price}";
        else
        {
            if (FTicks < 0)
                FTriggeredAction["price"] = std::string{ "${match_price}" } + std::to_string(FTicks);
            else
                FTriggeredAction["price"] = std::string{ "${match_price}+" } + std::to_string(FTicks);
        }
    }
    else if (!FPrice.empty())
    {
        FTriggeredAction.insert(std::make_pair("price", FPrice));
    }
    else
    {
        std::string s_price = std::to_string(order->GetPrice());                
        FTriggeredAction.insert(std::make_pair("price", s_price));
    }

    // QUANTITY
    long order_qty = order->GetOrderQty();
    if (order_qty)
    {
        std::string s_qty = std::to_string(order_qty);
        FTriggeredAction.insert(std::make_pair("quantity", s_qty));
    }
    else
    {
        FTriggeredAction.clear();
        return FALSE;
    }
    
    // EXCHANGE-CODE
    const char* twse_exchange_code = order->GetTSEExchangeCode();           
    if (twse_exchange_code)
        FTriggeredAction.insert(std::make_pair("exchange_code", twse_exchange_code));

    // ORDER-TYPE
    const char* twse_order_type = order->GetTSEOrderType();                 
    if (twse_order_type)
        FTriggeredAction.insert(std::make_pair("order_type", twse_order_type));

    // PRICE-TYPE
    nsOrderMessageDefine::OrderTypeEnum price_type = order->GetOrderType(); 
    if (nsOrderMessageDefine::OrderTypeEnum::otLimit == price_type)
        FTriggeredAction.insert(std::make_pair("price_type", "2")); // 限價
    else
        FTriggeredAction.insert(std::make_pair("price_type", "1")); // 市價

    // TIME-IN-FORCE
    nsOrderMessageDefine::TimeInForceEnum tif = order->GetTimeInForce();    
    if (nsOrderMessageDefine::TimeInForceEnum::tifIOC == tif)
        FTriggeredAction.insert(std::make_pair("time_in_force", "3"));
    else if (nsOrderMessageDefine::TimeInForceEnum::tifFOK == tif)
        FTriggeredAction.insert(std::make_pair("time_in_forcef", "4"));
    else
        FTriggeredAction.insert(std::make_pair("time_in_force", "0")); // ROD

    return TRUE;
}

BOOL TTouchOrderCommand::ToTriggeredAction(TCancelOrderMessage* order)
{
    FTriggeredAction.clear();

    FTriggeredAction.insert(std::make_pair("action", "twse_cancel"));
    FTriggeredAction.insert(std::make_pair("function_code", "04"));
    
    // 刪買或刪賣，不指定就全刪
    nsOrderMessageDefine::SideEnum side = order->GetSide();
    if (nsOrderMessageDefine::SideEnum::sBuy == side)
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "B"));
    else if (nsOrderMessageDefine::SideEnum::sSell == side)
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "S"));
    else
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "A"));
    
    // 刪指定價位
    if (!FPrice.empty())
    {
        FTriggeredAction["price"] = FPrice;
    }
    else
    {
        double price = order->GetPrice();
        if (price != 0)
        {
            std::string s_price = std::to_string(price);
            FTriggeredAction.insert(std::make_pair("price", s_price));
        }
    }

    // STOCK-NO 非必要，預防之後有觸價要刪別的商品的狀況
    const char* symbol = order->GetSymbol();                                
    if (symbol)
        FTriggeredAction.insert(std::make_pair("stock_no", symbol));

    // TODO ...其他欄位目前不需要，未來擴增可用

    return TRUE;
}

BOOL TTouchOrderCommand::ToTriggeredAction(TReplaceOrderMessage* order)
{
    FTriggeredAction.clear();

    FTriggeredAction.insert(std::make_pair("action", "twse_replace"));
    // 目前僅支援觸價單改價
    FTriggeredAction.insert(std::make_pair("function-code", "06"));
    
    // 改買單或改賣單
    nsOrderMessageDefine::SideEnum side = order->GetSide();
    if (nsOrderMessageDefine::SideEnum::sBuy == side)
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "B"));
    else if (nsOrderMessageDefine::SideEnum::sSell == side)
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "S"));

    // PRICE
    if (PriceDependOnEnum::pdoMatch == FPriceDependOn)
    {
        if (!FTicks)
            FTriggeredAction["price"] = "${match_price}";
        else
        {
            if (FTicks < 0)
                FTriggeredAction["price"] = std::string{ "${match_price}" } + std::to_string(FTicks);
            else
                FTriggeredAction["price"] = std::string{ "${match_price}+" } + std::to_string(FTicks);
        }
    }
    else if (!FPrice.empty())
    {
        FTriggeredAction.insert(std::make_pair("price", FPrice));
    }
    else
    {
        double price = order->GetPrice();
        if (price == 0.0)
        {
            FTriggeredAction.clear();
            return FALSE;
        }

        std::string s_price = std::to_string(price);
        FTriggeredAction.insert(std::make_pair("price", s_price));
    }    

    // STOCK-NO 非必要，預防之後有觸價要刪別的商品的狀況
    const char* symbol = order->GetSymbol();
    if (symbol)
        FTriggeredAction.insert(std::make_pair("stock_no", symbol));

    // TODO ...其他欄位目前不需要，未來擴增可用

    return TRUE;
}

BOOL TTouchOrderCommand::ToTriggeredAction(TTouchOrderCommand* toc)
{
    FTriggeredAction.clear();
    
    // 目前只支援刪觸價單
    if (toc && 
        TTouchOrderCommand::TouchedOrderCommandEnum::tocKill == toc->FCmdType)
    {
        FTriggeredAction.insert(std::make_pair("action", "touchorder_cancel"));

        if (nsOrderMessageDefine::SideEnum::sBuy == toc->FSide)
            FTriggeredAction.insert(std::make_pair("buy_sell_code", "B"));
        else if(nsOrderMessageDefine::SideEnum::sSell == toc->FSide)
            FTriggeredAction.insert(std::make_pair("buy_sell_code", "S"));
        else
            FTriggeredAction.insert(std::make_pair("buy_sell_code", "A"));

        // 觸價刪觸價單不需要指定觸價單單號. removed on 2026/04/07
        /*
        std::string toid = GetTouchOrderID();
        if (toid.empty())
            toid = toc->GetTouchOrderID();

        if (toid.empty())
        {
            FTriggeredAction.clear();
            return FALSE;
        }

        FTriggeredAction["order_no"] = toid;
        */

        return TRUE;
    }

    return FALSE;
}

BOOL TTouchOrderCommand::ToTriggeredAction(const char* warning)
{
    if (!warning || !warning[0])
        return FALSE;

    FTriggeredAction.clear();
    FTriggeredAction.insert(std::make_pair("action", "warning"));
    FTriggeredAction.insert(std::make_pair("message", warning));    

    return TRUE;
}

void TTouchOrderCommand::SetTriggeringCondition(TTriggeringCondition* ttc)
{
    FTriggeringCondition = ttc->ToString();
    FTriggerExpression = ttc->ToExpression();
}

std::string TTouchOrderCommand::GetTriggeredAction()
{
    if (FCmdType != TouchedOrderCommandEnum::tocNew)
        return "";

    std::stringstream ss;
    std::map<std::string, std::string>::iterator iter = FTriggeredAction.begin();
    for (; iter != FTriggeredAction.end(); ++iter)
    {
        if (iter != FTriggeredAction.begin())
            ss << "^";
        ss << iter->first << "=" << iter->second;
    }

    return ss.str();

    /*
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
    */
}

