#include "TTouchOrderCommand.h"
#include <sstream>

// 將價格轉成小數位數最多4位的字串
static std::string priceNomalize(double price)
{
    std::ostringstream ss;
    if (price == 0.0)
        return "0";

    if (price > 0)
        ss << price + 0.00001;
    else
        ss << price - 0.00001;

    std::string s_price = ss.str();
    std::string::size_type pos = s_price.find('.');
    s_price = s_price.substr(0, pos) + "." + s_price.substr(pos + 1, 4);
    return s_price;
}

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
    std::string s_integer_part;
    std::string s_decimal_part;
    std::string s_price = price;
    std::string::size_type pos = s_price.find('.');
    if (pos == std::string::npos)
        s_integer_part = s_price;
    else
    {
        s_integer_part = s_price.substr(0, pos);
        s_decimal_part = s_price.substr(pos + 1);
        if (s_decimal_part.empty())
        {
            FPrice.clear();
            return;
        }
    }

    if (s_integer_part.empty())
    {
        FPrice.clear();
        return;
    }

    for (size_t i = 0; i < s_integer_part.size(); ++i)
    {
        if (!isdigit(s_integer_part[i]))
        {
            FPrice.clear();
            return;
        }
    }

    for (size_t i = 0; i < s_decimal_part.size(); ++i)
    {
        if (!isdigit(s_decimal_part[i]))
        {
            FPrice.clear();
            return;
        }
    }

    /*
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
    */

    FPrice = price;

    if (/*PriceDependOnEnum::*/pdoMatch == FPriceDependOn)
        return;

    std::map<std::string, std::string>::iterator iter = FTriggeredAction.find("action");
    if (iter == FTriggeredAction.end())
        return;

    if (iter->second == "twse_cancel" ||
        iter->second == "twse_replace" ||
        iter->second == "twse_new")
    {
        FTriggeredAction["price"] = price;
    }
    else if (iter->second == "taifex_cancel" ||
             iter->second == "taifex_replace" ||
             iter->second == "taifex_new")
    {
        FTriggeredAction["order_price"] = price;
    }
    else
        return;

    
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

    if (/*PriceDependOnEnum::*/pdoMatch != FPriceDependOn)
        return;

    std::map<std::string, std::string>::iterator iter = FTriggeredAction.find("action");
    if (iter == FTriggeredAction.end() || 
        (iter->second != "twse_new" &&
         iter->second != "twse_replace") ||
        (iter->second != "taifex_new" &&
         iter->second != "taifex_replace"))
        return;

    std::string price_field = "order_price";
    if (iter->second == "twse_new" ||
        iter->second == "twse_replace")
    {
        price_field = "price";
    }

    if (!FTicks)
        FTriggeredAction[price_field] = "${match_price}";
    else
    {
        std::stringstream ss;
        if (FTicks < 0)
            ss << "${match_price}" << FTicks;
        else
            ss << "${match_price}+" << FTicks;

        FTriggeredAction[price_field] = ss.str();        
    }
}

BOOL TTouchOrderCommand::ToTriggeredAction(TNewOrderMessage* order)
{
    FTriggeredAction.clear();
    FTouchedActionType = taUnknown;
    FTouchedActionMarket = nsOrderMessageDefine::mUnknown;
    FTouchedActionSymbol.clear();

    // 目前僅支援國內證劵跟國內期貨下單
    nsOrderMessageDefine::MarketEnum market = order->GetMarket();
    if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
        nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
    {
        FTriggeredAction.insert(std::make_pair("action", "twse_new"));        
    }
    else if (nsOrderMessageDefine::/*MarketEnum::*/mTWFutures == market || 
             nsOrderMessageDefine::/*MarketEnum::*/mTWOptions == market)
    {
        FTriggeredAction.insert(std::make_pair("action", "taifex_new"));
        if (nsOrderMessageDefine::mTWFutures == market)
            FTriggeredAction.insert(std::make_pair("market", "fut"));
        else
            FTriggeredAction.insert(std::make_pair("market", "opt"));
    }
    else
    {
        FLastErrMsg = "Unknown/Unsupported market!";
        FTriggeredAction.clear();
        return FALSE;
    }    

    // Note: 欄位命名:
    //       [證劵]
    //       根據證交所的「一般交易電腦作業手冊」裡格式 T010 的欄位名稱為基礎，並做下列變化
    //       1. 大寫英文改成小寫英文
    //       2. 符號 '-' 改成 '_'
    //       [期權]
    //       根據「X.25期貨與選擇權交易系統主機連線作業規範(V2.4).pdf」裡格式 R010 的欄位名稱為基礎，並做下列變化
    //       1. 大寫英文改成小寫英文
    //       2. 符號 '-' 改成 '_'
    nsOrderMessageDefine::SideEnum side = order->GetSide();
    if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
        nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
    {
        if (nsOrderMessageDefine::/*SideEnum::*/sBuy == side)
        {
            FTriggeredAction.insert(std::make_pair("function_code", "01"));
            FTriggeredAction.insert(std::make_pair("buy_sell_code", "B"));
        }
        else
        {
            FTriggeredAction.insert(std::make_pair("function_code", "02"));
            FTriggeredAction.insert(std::make_pair("buy_sell_code", "S"));
        }
    }
    else
    {
        FTriggeredAction.insert(std::make_pair("function_code", "01"));
        if (nsOrderMessageDefine::/*SideEnum::*/sBuy == side)
            FTriggeredAction.insert(std::make_pair("buy_sell_code", "B"));
        else
            FTriggeredAction.insert(std::make_pair("buy_sell_code", "S"));
        
    }

    // Note: Market 跟 Trading Session 會決定電文檔頭 sub-system 欄位的內容
    if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
        nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
    {
        nsOrderMessageDefine::TradingSessionIDEnum trading_session = order->GetTradingSessionID();
        if (nsOrderMessageDefine::/*TradingSessionIDEnum::*/tsNormal == trading_session) //一般
        {
            if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market)
                FTriggeredAction.insert(std::make_pair("subsystem_name", "30"));
            else
                FTriggeredAction.insert(std::make_pair("subsystem_name", "93"));
        }
        else if (nsOrderMessageDefine::/*TradingSessionIDEnum::*/tsIntradayOdd == trading_session) //盤中零股
        {
            if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market)
                FTriggeredAction.insert(std::make_pair("subsystem_name", "33"));
            else
                FTriggeredAction.insert(std::make_pair("subsystem_name", "83"));
        }
        else if (nsOrderMessageDefine::/*TradingSessionIDEnum::*/tsOffHour == trading_session) //盤後定價
        {
            if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market)
                FTriggeredAction.insert(std::make_pair("subsystem_name", "32"));
            else
                FTriggeredAction.insert(std::make_pair("subsystem_name", "98"));
        }
        else if (nsOrderMessageDefine::/*TradingSessionIDEnum::*/tsOddLot == trading_session) //盤後零股
        {
            if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market)
                FTriggeredAction.insert(std::make_pair("subsystem_name", "40"));
            else
                FTriggeredAction.insert(std::make_pair("subsystem_name", "94"));
        }
        else
        {
            FLastErrMsg = "Unknow/Unsupported Trading Session in Stock order!";
            FTriggeredAction.clear();
            return FALSE;
        }
    }
    else
    {
        FTriggeredAction.insert(std::make_pair("subsystem_name", "30"));
    }

    // BORKER-ID
    const char* broker_id = order->GetBrokerID();                           
    if (broker_id)
    {
        if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
            nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
        {
            if (strlen(broker_id) != 4)
            {
                FLastErrMsg = "Invalid BrokerID format in Stock order!";
                FTriggeredAction.clear();
                return FALSE;
            }
            else
                FTriggeredAction.insert(std::make_pair("broker_id", broker_id));
        }
        else
        {
            std::string fcm_no = broker_id;
            if (fcm_no.size() != 7)
            {
                FLastErrMsg = "Invalid BrokerID format in Futures/Options order!";
                FTriggeredAction.clear();
                return FALSE;
            }

            FTriggeredAction.insert(std::make_pair("fcm_no", fcm_no));
            FTriggeredAction.insert(std::make_pair("clear_member_id", fcm_no.substr(0,4)));
        }
    }
    else
    {
        FLastErrMsg = "BrokerID not provided in Stock/Futures/Options order!";
        FTriggeredAction.clear();
        return FALSE;
    }

    // IVACNO
    const char* account = order->GetAccount();                              
    if (account)
    {
        if (strlen(account) != 7)
        {
            FLastErrMsg = "Invalid Account format in Stock/Futures/Options order!";
            FTriggeredAction.clear();
            return FALSE;
        }

        if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
            nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
        {
            FTriggeredAction.insert(std::make_pair("ivacno", account));
        }
        else
        {
            FTriggeredAction.insert(std::make_pair("investor_acno", account));
        }
    }
    else
    {
        FLastErrMsg = "Account not provided in Stock/Futures/Options order!";
        FTriggeredAction.clear();
        return FALSE;
    }

    // IVACNO-FLAG
    const char* account_flag = order->GetAccountFlag();                     
    if (account_flag)
    {
        if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
            nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
        {
            FTriggeredAction.insert(std::make_pair("ivacno_flag", account_flag));
        }
        else
        {
            FTriggeredAction.insert(std::make_pair("investor_flag", account_flag));
        }
    }

    // STOCK-NO
    const char* symbol = order->GetSymbol();                                
    if (symbol)
    {
        if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
            nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
        {
            FTriggeredAction.insert(std::make_pair("stock_no", symbol));
        }
        else
        {
            FTriggeredAction.insert(std::make_pair("prod_id", symbol));
        }
    }
    else
    {
        FLastErrMsg = "Symbol not provided in Stock/Futures/Options order!";
        FTriggeredAction.clear();
        return FALSE;
    }

    // PRICE
    std::string price_field = "order_price";
    if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
        nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
    {
        price_field = "price";
    }
    if (/*PriceDependOnEnum::*/pdoMatch == FPriceDependOn)
    {
        if (!FTicks)
            FTriggeredAction[price_field] = "${match_price}";
        else
        {
            std::stringstream ss;
            if (FTicks < 0)
                ss << "${match_price}" << FTicks;
            else
                ss << "${match_price}+" << FTicks;

            FTriggeredAction[price_field] = ss.str();            
        }
    }
    else if (!FPrice.empty())
    {
        FTriggeredAction.insert(std::make_pair(price_field, FPrice));
    }
    else
    {
        //std::stringstream ss;
        //ss << order->GetPrice();
        //std::string s_price = ss.str();                
        std::string s_price = priceNomalize(order->GetPrice());
        FTriggeredAction.insert(std::make_pair(price_field, s_price));
    }

    // QUANTITY
    std::string quantity_field = "order_quantity";
    if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
        nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
    {
        quantity_field = "quantity";
    }
    long order_qty = order->GetOrderQty();
    if (order_qty)
    {
        std::stringstream ss;
        ss << order_qty;
        //std::string s_qty = std::to_string(order_qty);
        FTriggeredAction.insert(std::make_pair(quantity_field, ss.str()));
    }
    else
    {
        FLastErrMsg = "OrderQty not provided in Stock/Futures/Options order!";
        FTriggeredAction.clear();
        return FALSE;
    }
    
    if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
        nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
    {
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
        if (nsOrderMessageDefine::/*OrderTypeEnum::*/otLimit == price_type)
            FTriggeredAction.insert(std::make_pair("price_type", "2")); // 限價
        else
            FTriggeredAction.insert(std::make_pair("price_type", "1")); // 市價

        // TIME-IN-FORCE
        nsOrderMessageDefine::TimeInForceEnum tif = order->GetTimeInForce();
        if (nsOrderMessageDefine::/*TimeInForceEnum::*/tifIOC == tif)
            FTriggeredAction.insert(std::make_pair("time_in_force", "3"));
        else if (nsOrderMessageDefine::/*TimeInForceEnum::*/tifFOK == tif)
            FTriggeredAction.insert(std::make_pair("time_in_forcef", "4"));
        else
            FTriggeredAction.insert(std::make_pair("time_in_force", "0")); // ROD
    }
    else
    {
        // ORDER_TYPE
        nsOrderMessageDefine::OrderTypeEnum price_type = order->GetOrderType();
        if (nsOrderMessageDefine::/*OrderTypeEnum::*/otLimit == price_type)
            FTriggeredAction.insert(std::make_pair("order_type", "L")); // 限價
        else
            FTriggeredAction.insert(std::make_pair("order_type", "M")); // 市價

        // ORDER-CONDITION
        nsOrderMessageDefine::TimeInForceEnum tif = order->GetTimeInForce();
        if (nsOrderMessageDefine::/*TimeInForceEnum::*/tifIOC == tif)
            FTriggeredAction.insert(std::make_pair("order_condition", "I"));
        else if (nsOrderMessageDefine::/*TimeInForceEnum::*/tifFOK == tif)
            FTriggeredAction.insert(std::make_pair("order_condition", "F"));
        else
            FTriggeredAction.insert(std::make_pair("order_condition", "R")); // ROD

        // OPEN-OFFSET-FLAG
        std::string open_offset_flag;
        switch (order->GetPositionEffect()) 				 //< Open offset flag
        {
        case nsOrderMessageDefine::peOpen:          open_offset_flag = "0"; break;
        case nsOrderMessageDefine::peClose:         open_offset_flag = "1"; break;
        case nsOrderMessageDefine::peDayTrade:      open_offset_flag = "2"; break;
        case nsOrderMessageDefine::peAuto:          open_offset_flag = " "; break;
        case nsOrderMessageDefine::peDayTradeOpen:  open_offset_flag = "3"; break;
        case nsOrderMessageDefine::peAutoToday:     open_offset_flag = "5"; break;
        case nsOrderMessageDefine::peForceClose:    open_offset_flag = "7"; break;
        case nsOrderMessageDefine::peTMPMarketMaker:open_offset_flag = "9"; break;
        case nsOrderMessageDefine::peTAIFEXAuto:    open_offset_flag = "A"; break;
        default:                                    open_offset_flag = "0"; break;
        }
        FTriggeredAction.insert(std::make_pair("open_offset_flag", open_offset_flag)); 
    }

    // USER_DATA
    if (order->GetAllUserData())
    {
        std::string user_data = order->GetAllUserData();
        std::string encoded_userdata;
        for (size_t i = 0; i < user_data.size(); ++i)
        {
            if (' ' == user_data.at(i))
                encoded_userdata.append("%20");
            else if ('^' == user_data.at(i))
                encoded_userdata.append("%5E");
            else
                encoded_userdata.append(1, user_data.at(i));
        }

        FTriggeredAction.insert(std::make_pair("user_data", encoded_userdata));
    }

    FTouchedActionType = taNewOrder;
    FTouchedActionMarket = market;
    FTouchedActionSymbol = symbol;

    return TRUE;
}

BOOL TTouchOrderCommand::ToTriggeredAction(TCancelOrderMessage* order)
{
    FTriggeredAction.clear();
    FTouchedActionType = taUnknown;
    FTouchedActionSymbol.clear();

    nsOrderMessageDefine::MarketEnum market = order->GetMarket();
    if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
        nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
    {
        FTriggeredAction.insert(std::make_pair("action", "twse_cancel"));
        FTriggeredAction.insert(std::make_pair("function_code", "04"));
    }
    else if (nsOrderMessageDefine::/*MarketEnum::*/mTWFutures == market || 
             nsOrderMessageDefine::/*MarketEnum::*/mTWOptions == market)
    {
        FTriggeredAction.insert(std::make_pair("action", "taifex_cancel"));
        FTriggeredAction.insert(std::make_pair("function_code", "03"));
        if (nsOrderMessageDefine::mTWFutures == market)
            FTriggeredAction.insert(std::make_pair("market", "fut"));
        else
            FTriggeredAction.insert(std::make_pair("market", "opt"));
    }
    else
    {
        FLastErrMsg = "Unknown/Unsupported market!";
        FTriggeredAction.clear();
        return FALSE;
    }

    
    // 刪買或刪賣，不指定就全刪
    nsOrderMessageDefine::SideEnum side = order->GetSide();
    if (nsOrderMessageDefine::/*SideEnum::*/sBuy == side)
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "B"));
    else if (nsOrderMessageDefine::/*SideEnum::*/sSell == side)
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "S"));
    else
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "A"));
    
    std::string price_field = "order_price";
    if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
        nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
    {
        price_field = "price";
    }
    // 刪指定價位
    if (!FPrice.empty())
    {
        FTriggeredAction[price_field] = FPrice;
    }
    else
    {
        double price = order->GetPrice();
        if (price != 0.0)
        {
            //std::stringstream ss;
            //ss << price;
            //std::string s_price = std::to_string(price);
            std::string s_price = priceNomalize(price);
            FTriggeredAction.insert(std::make_pair(price_field, s_price));
        }
    }


    const char* symbol = order->GetSymbol();                                
    if (symbol)
    {
        if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
            nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
        {
            FTriggeredAction.insert(std::make_pair("stock_no", symbol));
        }
        else
        {
            FTriggeredAction.insert(std::make_pair("prod_id", symbol));
        }
    }
    else
    {
        FLastErrMsg = "Symbol not provided in Stock/Futures/Options order!";
        FTriggeredAction.clear();
        return FALSE;
    }

    if (order->GetAllUserData())
    {
        std::string user_data = order->GetAllUserData();
        std::string encoded_userdata;
        for (size_t i = 0; i < user_data.size(); ++i)
        {
            if (' ' == user_data.at(i))
                encoded_userdata.append("%20");
            else if ('^' == user_data.at(i))
                encoded_userdata.append("%5E");
            else
                encoded_userdata.append(1, user_data.at(i));
        }

        FTriggeredAction.insert(std::make_pair("user_data", encoded_userdata));
    }

    // TODO ...其他欄位目前不需要，未來擴增可用
    FTouchedActionType = taCxlOrder;
    FTouchedActionMarket = market;
    FTouchedActionSymbol = symbol;

    return TRUE;
}

BOOL TTouchOrderCommand::ToTriggeredAction(TReplaceOrderMessage* order)
{
    FTriggeredAction.clear();
    FTouchedActionType = taUnknown;
    FTouchedActionSymbol.clear();

    nsOrderMessageDefine::MarketEnum market = order->GetMarket();
    if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
        nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
    {
        FTriggeredAction.insert(std::make_pair("action", "twse_replace"));
        // 目前僅支援觸價單改價
        FTriggeredAction.insert(std::make_pair("function-code", "06"));
    }
    else if (nsOrderMessageDefine::/*MarketEnum::*/mTWFutures == market ||
             nsOrderMessageDefine::/*MarketEnum::*/mTWOptions == market)
    {
        FTriggeredAction.insert(std::make_pair("action", "taifex_replace"));
        // 目前僅支援觸價單改價
        FTriggeredAction.insert(std::make_pair("function-code", "06"));
        if (nsOrderMessageDefine::mTWFutures == market)
            FTriggeredAction.insert(std::make_pair("market", "fut"));
        else
            FTriggeredAction.insert(std::make_pair("market", "opt"));
    }
    else
    {
        FLastErrMsg = "Unknown/Unsupported market!";
        FTriggeredAction.clear();
        return FALSE;
    }
    
    // 改買單或改賣單
    nsOrderMessageDefine::SideEnum side = order->GetSide();
    if (nsOrderMessageDefine::/*SideEnum::*/sBuy == side)
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "B"));
    else if (nsOrderMessageDefine::/*SideEnum::*/sSell == side)
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "S"));

    // PRICE
    std::string price_field = "order_price";
    if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
        nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
    {
        price_field = "price";
    }

    if (/*PriceDependOnEnum::*/pdoMatch == FPriceDependOn)
    {
        if (!FTicks)
            FTriggeredAction[price_field] = "${match_price}";
        else
        {
            std::stringstream ss;
            if (FTicks < 0)
                ss << "${match_price}" << FTicks;
            else
                ss << "${match_price}+" << FTicks;
            
            FTriggeredAction[price_field] = ss.str();

            /*
            if (FTicks < 0)
                FTriggeredAction["price"] = std::string{ "${match_price}" } + std::to_string(FTicks);
            else
                FTriggeredAction["price"] = std::string{ "${match_price}+" } + std::to_string(FTicks);
            */
        }
    }
    else if (!FPrice.empty())
    {
        FTriggeredAction.insert(std::make_pair(price_field, FPrice));
    }
    else
    {
        double price = order->GetPrice();
        if (price == 0.0)
        {
            FLastErrMsg = "Price not provided in Stock/Futures/Options order!";
            FTriggeredAction.clear();
            return FALSE;
        }

        //std::stringstream ss;
        //ss << price;
        //std::string s_price = std::to_string(price);
        std::string s_price = priceNomalize(price);
        FTriggeredAction.insert(std::make_pair(price_field, s_price));
    }    

    const char* symbol = order->GetSymbol();
    if (symbol)
    {
        if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
            nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
        {
            FTriggeredAction.insert(std::make_pair("stock_no", symbol));
        }
        else
        {
            FTriggeredAction.insert(std::make_pair("prod_id", symbol));
        }
    }
    else
    {
        FLastErrMsg = "Symbol not provided in Stock/Futures/Options order!";
        FTriggeredAction.clear();
        return FALSE;
    }

    if (order->GetAllUserData())
    {
        std::string user_data = order->GetAllUserData();
        std::string encoded_userdata;
        for (size_t i = 0; i < user_data.size(); ++i)
        {
            if (' ' == user_data.at(i))
                encoded_userdata.append("%20");
            else if ('^' == user_data.at(i))
                encoded_userdata.append("%5E");
            else
                encoded_userdata.append(1, user_data.at(i));
        }

        FTriggeredAction.insert(std::make_pair("user_data", encoded_userdata));
    }

    // TODO ...其他欄位目前不需要，未來擴增可用
    FTouchedActionType = taRpxOrder;
    FTouchedActionMarket = market;
    FTouchedActionSymbol = symbol;

    return TRUE;
}

BOOL TTouchOrderCommand::ToTriggeredAction(TTouchOrderCommand* toc)
{
    FTriggeredAction.clear();
    FTouchedActionType = taUnknown;
    FTouchedActionMarket = nsOrderMessageDefine::mUnknown;

    // 目前只支援刪觸價單
    if (toc && 
        TTouchOrderCommand::/*TouchedOrderCommandEnum::*/tocKill == toc->FCmdType)
    {
        FTriggeredAction.insert(std::make_pair("action", "touchorder_cancel"));

        if (nsOrderMessageDefine::/*SideEnum::*/sBuy == toc->FSide)
            FTriggeredAction.insert(std::make_pair("buy_sell_code", "B"));
        else if(nsOrderMessageDefine::/*SideEnum::*/sSell == toc->FSide)
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
        FTouchedActionType = taCxlTouchedOrder;

        return TRUE;
    }

    FLastErrMsg = "Unsupported touch order command to execute when order is touched!";
    FTriggeredAction.clear();
    return FALSE;
}

BOOL TTouchOrderCommand::ToTriggeredAction(const char* warning)
{
    FTriggeredAction.clear();
    FTouchedActionType = taUnknown;
    FTouchedActionMarket = nsOrderMessageDefine::mUnknown;

    if (!warning || !warning[0])
        return FALSE;

    std::string encoded_warning;
    size_t length = strlen(warning);
    for (size_t i = 0; i < length; ++i)
    {
        if (warning[i] == ' ')
            encoded_warning.append("%20");
        else if (warning[i] == '^')
            encoded_warning.append("%5E");
        else
            encoded_warning.push_back(warning[i]);
    }

    
    FTriggeredAction.insert(std::make_pair("action", "warning"));
    FTriggeredAction.insert(std::make_pair("message", warning));    
    FTouchedActionType = taWarning;

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

/*
void TTouchOrderCommand::SetOrderMessagePricePrecision(int price_precision)
{
    std::stringstream ss;
    ss << price_precision;
    FTriggeredAction.insert(std::make_pair("price_precision", ss.str()));
}
*/
