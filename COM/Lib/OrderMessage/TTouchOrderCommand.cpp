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

// OrderSourceEnum -> 委託類別註記(char)
// copy from TTaifexconnection::OrderSourceCode()
// added on 2026/08/07
static char OrderSourceCode(nsOrderMessageDefine::OrderSourceEnum OrdSrc)
{
    switch (OrdSrc)
    {
    case nsOrderMessageDefine::osDedicatedLine:
        return 'D';
    case nsOrderMessageDefine::osAPI:
        return 'A';
    case nsOrderMessageDefine::osMobile:
        return 'M';
    case nsOrderMessageDefine::osWeb:
        return 'W';
    case nsOrderMessageDefine::osPCApplication:
        return 'P';
    case nsOrderMessageDefine::osVoice:
        return 'V';
    case nsOrderMessageDefine::osGeneral:
    default:
        return 'G';
    }
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

/*
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

    FPrice = price;

    if (pdoMatch == FPriceDependOn)
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
*/


// 設定觸價下新單或觸價下改單時是以觸價時的成交價加減檔位為委託價                                                                         
void TTouchOrderCommand::SetFloatingOrderPrice(PriceDependOnEnum depend_on, int ticks)             
{
    FPriceDependOn = depend_on;
    FTicks = ticks;

    // 以下程序留到 TTaifexConnection::TouchOrderControl() 時再執行
    /*
    if (pdoMatch != FPriceDependOn)
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
    */
}

void TTouchOrderCommand::ClearTriggeredAction()
{
    FTriggeredAction.clear();
    FTouchedActionType = taUnknown;
    FTouchedActionMarket = nsOrderMessageDefine::mUnknown;
    FTouchedActionPrice = 0;
    FTouchedActionOrderType = nsOrderMessageDefine::otNone;
    FTouchedActionSymbol.clear();
}

BOOL TTouchOrderCommand::ToTriggeredAction(TNewOrderMessage* order)
{
    ClearTriggeredAction();

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

        // added on 2026/08/07 
        // 期貨下單需要 order_source(char[1]) & info_source(char[3]) 兩個欄位
        // 下單時要合併一起放進 MTree[SOURCE] 裡
        std::ostringstream os;
        os << OrderSourceCode(order->GetOrderSource()) << order->GetMarketDataSource();
        std::string source = os.str();
        FTriggeredAction.insert(std::make_pair("source", source));
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

        // TBaseMeesgae 預設 Account 為 7 個空白字元，檢查一下是否是全空白
        bool all_blank = true;
        for (int i = 0; i < 7; ++i)
        {
            if (' ' != account[i])
            {
                all_blank = false;
                break;
            }
        }

        if (!all_blank)
        {
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
        // TBaseMeesgae 預設值會填 5 個空白字元，檢查是否如此
        bool all_blank = true;
        int len = strlen(symbol);
        for (int i = 0; i < len; ++i)
        {
            if (' ' != symbol[i])
            {
                all_blank = false;
                break;
            }
        }

        if (!all_blank)
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
    }
    else
    {
        FLastErrMsg = "Symbol not provided in Stock/Futures/Options order!";
        FTriggeredAction.clear();
        return FALSE;
    }

    // PRICE
    std::string price_field = "order_price";
    if (nsOrderMessageDefine::mTSE == market ||
        nsOrderMessageDefine::mOTC == market)
    {
        price_field = "price";
    }
   
    //if (pdoMatch == FPriceDependOn)
    //{
    //    if (!FTicks)
    //        FTriggeredAction[price_field] = "${match_price}";
    //    else
    //    {
    //        std::stringstream ss;
    //        if (FTicks < 0)
    //            ss << "${match_price}" << FTicks;
    //        else
    //            ss << "${match_price}+" << FTicks;
    //
    //        FTriggeredAction[price_field] = ss.str();            
    //    }
    //}
    //else if (!FPrice.empty())
    //{
    //    FTriggeredAction.insert(std::make_pair(price_field, FPrice));
    //}
    //else
    //{
        
    double price = order->GetPrice();
    std::string s_price = priceNomalize(price);
    FTriggeredAction.insert(std::make_pair(price_field, s_price));
    FTouchedActionPrice = price;
    
    //}

    // QUANTITY
    std::string quantity_field = "order_quantity";
    if (nsOrderMessageDefine::mTSE == market ||
        nsOrderMessageDefine::mOTC == market)
    {
        quantity_field = "quantity";
    }
    long order_qty = order->GetOrderQty();
    if (order_qty)
    {
        std::stringstream ss;
        ss << order_qty;
        
        FTriggeredAction.insert(std::make_pair(quantity_field, ss.str()));
    }
    else
    {
        FLastErrMsg = "OrderQty not provided in Stock/Futures/Options order!";
        FTriggeredAction.clear();
        return FALSE;
    }
    
    if (nsOrderMessageDefine::mTSE == market ||
        nsOrderMessageDefine::mOTC == market)
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
        if (nsOrderMessageDefine::otLimit == price_type)
        {
            FTriggeredAction.insert(std::make_pair("price_type", "2")); // 限價
            FTouchedActionOrderType = nsOrderMessageDefine::otLimit;
        }
        else
        {
            FTriggeredAction.insert(std::make_pair("price_type", "1")); // 市價
            FTouchedActionOrderType = nsOrderMessageDefine::otMarket;
        }

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
        {
            FTriggeredAction.insert(std::make_pair("taifex_order_type", "L")); // 限價
            FTouchedActionOrderType = nsOrderMessageDefine::otLimit;
        }
        else
        {
            FTriggeredAction.insert(std::make_pair("taifex_order_type", "M")); // 市價
            FTouchedActionOrderType = nsOrderMessageDefine::otMarket;
        }

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
    ClearTriggeredAction();

    nsOrderMessageDefine::MarketEnum market = order->GetMarket();
    if (nsOrderMessageDefine::mTSE == market ||
        nsOrderMessageDefine::mOTC == market)
    {
        FTriggeredAction.insert(std::make_pair("action", "twse_cancel"));
        nsOrderMessageDefine::TradingSessionIDEnum trading_session = order->GetTradingSessionID();
        if (nsOrderMessageDefine::tsNormal == trading_session) //一般
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

        // added on 2026/08/07 
        // 期貨下單需要 order_source(char[1]) & info_source(char[3]) 兩個欄位
        // 下單時要合併一起放進 MTree[SOURCE] 裡
        std::ostringstream os;
        os << OrderSourceCode(order->GetOrderSource()) << order->GetMarketDataSource();
        std::string source = os.str();
        FTriggeredAction.insert(std::make_pair("source", source));
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
    if (nsOrderMessageDefine::mTSE == market ||
        nsOrderMessageDefine::mOTC == market)
    {
        price_field = "price";
    }
    // 刪指定價位
    double price = order->GetPrice();
    if (price != 0.0)
    {        
        std::string s_price = priceNomalize(price);
        FTriggeredAction.insert(std::make_pair(price_field, s_price));
    }

    const char* symbol = order->GetSymbol();                                
    if (symbol)
    {
        // TBaseMeesgae 預設值會填 5 個空白字元，檢查是否如此
        bool all_blank = true;
        int len = strlen(symbol);
        for (int i = 0; i < len; ++i)
        {
            if (' ' != symbol[i])
            {
                all_blank = false;
                break;
            }
        }

        if (!all_blank)
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
    }
    else
    {
        FLastErrMsg = "Symbol not provided in Stock/Futures/Options order!";
        FTriggeredAction.clear();
        return FALSE;
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
        }
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

        // TBaseMeesgae 預設 Account 為 7 個空白字元，檢查一下是否是全空白
        bool all_blank = true;
        for (int i = 0; i < 7; ++i)
        {
            if (' ' != account[i])
            {
                all_blank = false;
                break;
            }
        }

        if (!all_blank)
        {
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
        
    FTouchedActionType = taCxlOrder;
    FTouchedActionMarket = market; 
    FTouchedActionSymbol = symbol;

    return TRUE;
}

BOOL TTouchOrderCommand::ToTriggeredAction(TReplaceOrderMessage* order)
{
    ClearTriggeredAction();

    nsOrderMessageDefine::MarketEnum market = order->GetMarket();
    if (nsOrderMessageDefine::/*MarketEnum::*/mTSE == market ||
        nsOrderMessageDefine::/*MarketEnum::*/mOTC == market)
    {
        FTriggeredAction.insert(std::make_pair("action", "twse_replace"));
        // 目前僅支援觸價單改價
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

        // added on 2026/08/07 
        // 期貨下單需要 order_source(char[1]) & info_source(char[3]) 兩個欄位
        // 下單時要合併一起放進 MTree[SOURCE] 裡
        std::ostringstream os;
        os << OrderSourceCode(order->GetOrderSource()) << order->GetMarketDataSource();
        std::string source = os.str();
        FTriggeredAction.insert(std::make_pair("source", source));
    }
    else
    {
        FLastErrMsg = "Unknown/Unsupported market!";
        FTriggeredAction.clear();
        return FALSE;
    }
    
    // 改買單或改賣單
    nsOrderMessageDefine::SideEnum side = order->GetSide();
    if (nsOrderMessageDefine::sBuy == side)
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "B"));
    else if (nsOrderMessageDefine::sSell == side)
        FTriggeredAction.insert(std::make_pair("buy_sell_code", "S"));

    // PRICE
    std::string price_field = "order_price";
    if (nsOrderMessageDefine::mTSE == market ||
        nsOrderMessageDefine::mOTC == market)
    {
        price_field = "price";
    }    

    double price = order->GetPrice();
    std::string s_price = priceNomalize(price);
    FTriggeredAction.insert(std::make_pair(price_field, s_price));
    FTouchedActionPrice = price;
    
    const char* symbol = order->GetSymbol();
    if (symbol)
    {
        // TBaseMeesgae 預設值會填 5 個空白字元，檢查是否如此
        bool all_blank = true;
        int len = strlen(symbol);
        for (int i = 0; i < len; ++i)
        {
            if (' ' != symbol[i])
            {
                all_blank = false;
                break;
            }
        }

        if (!all_blank)
        {
            if (nsOrderMessageDefine::mTSE == market ||
                nsOrderMessageDefine::mOTC == market)
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
    }
    else
    {
        FLastErrMsg = "Symbol not provided in Stock/Futures/Options order!";
        FTriggeredAction.clear();
        return FALSE;
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
        }
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

        // TBaseMeesgae 預設 Account 為 7 個空白字元，檢查一下是否是全空白
        bool all_blank = true;
        for (int i = 0; i < 7; ++i)
        {
            if (' ' != account[i])
            {
                all_blank = false;
                break;
            }
        }

        if (!all_blank)
        {
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

    FTouchedActionType = taRpxOrder;
    FTouchedActionMarket = market;
    FTouchedActionSymbol = symbol;

    return TRUE;
}

BOOL TTouchOrderCommand::ToTriggeredAction(TTouchOrderCommand* toc)
{
    ClearTriggeredAction();

    // 目前只支援刪觸價單
    if (toc && 
        TTouchOrderCommand::tocKill == toc->FCmdType)
    {
        nsOrderMessageDefine::MarketEnum market = toc->GetMarket();
        std::string                      symbol = toc->GetSymbol();

        if (symbol.empty())
        {
            FLastErrMsg = "Symbol not specified in touch order command!";
            return FALSE;
        }

        if (nsOrderMessageDefine::mTSE == market)
        {
            FTriggeredAction.insert(std::make_pair("market", "tse"));
            FTriggeredAction.insert(std::make_pair("stock_no", symbol));
        }
        else if (nsOrderMessageDefine::mOTC == market)
        {
            FTriggeredAction.insert(std::make_pair("market", "otc"));
            FTriggeredAction.insert(std::make_pair("stock_no", symbol));
        }
        else if (nsOrderMessageDefine::mTWFutures == market)
        {
            FTriggeredAction.insert(std::make_pair("market", "fut"));
            FTriggeredAction.insert(std::make_pair("prod_id", symbol));
        }
        else if (nsOrderMessageDefine::mTWOptions == market)
        {
            FTriggeredAction.insert(std::make_pair("market", "opt"));
            FTriggeredAction.insert(std::make_pair("prod_id", symbol));
        }
        else
        {
            FLastErrMsg = "Unknown/Unsupported market!";
            return FALSE;
        }        

        FTriggeredAction.insert(std::make_pair("action", "touchorder_cancel"));

        if (nsOrderMessageDefine::sBuy == toc->FSide)
            FTriggeredAction.insert(std::make_pair("buy_sell_code", "B"));
        else if(nsOrderMessageDefine::sSell == toc->FSide)
            FTriggeredAction.insert(std::make_pair("buy_sell_code", "S"));
        else
            FTriggeredAction.insert(std::make_pair("buy_sell_code", "A"));        
            

        FTouchedActionType = taCxlTouchedOrder;
        FTouchedActionMarket = market;
        FTouchedActionSymbol = symbol;        

        return TRUE;
    }

    FLastErrMsg = "Unsupported touch order command to execute when order is touched!";
    FTriggeredAction.clear();
    return FALSE;
}

BOOL TTouchOrderCommand::ToTriggeredAction(const char* warning)
{
    ClearTriggeredAction();

    if (!warning || !warning[0])
    {
        FLastErrMsg = "Warning message not specified!";
        return FALSE;
    }   

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

    bool is_price_blank = true;
    std::stringstream ss;
    std::map<std::string, std::string>::iterator iter = FTriggeredAction.begin();
    for (; iter != FTriggeredAction.end(); ++iter)
    {
        if (iter != FTriggeredAction.begin())
            ss << "^";

        if (taNewOrder != FTouchedActionType &&
            taRpxOrder != FTouchedActionType)
        {
            ss << iter->first << "=" << iter->second;
        }
        else
        {
            if (iter->first == "order_price" || iter->first == "price")
            {
                if (FPriceDependOn == pdoMatch)
                {
                    if (!FTicks)
                    {
                        ss << iter->first << "=${match_price}";
                    }
                    else
                    {
                        if (FTicks > 0)
                            ss << iter->first << "=${match_price}+" << FTicks;
                        else
                            ss << iter->first << "=${match_price}" << FTicks;
                    }
                }
                else
                {
                    ss << iter->first << "=" << iter->second;
                }
            }
            else
                ss << iter->first << "=" << iter->second;
        }
    }   

    return ss.str();   
}

BOOL TTouchOrderCommand::Verify()
{
    FLastErrMsg.clear();

    if (tocQuery == FCmdType)
        return TRUE;

    if (tocPause == FCmdType || tocActive == FCmdType || tocKill == FCmdType)
    {
        if (FTouchOrderID.empty())
        {
            FLastErrMsg = "Miss touch order id for Pause/Active/Remove to control touch order!";
            return FALSE;
        }

        return TRUE;
    }

    if (FTriggeringCondition.empty())
    {
        FLastErrMsg = "Triggering condition is incorrect!";
        return FALSE;
    }
    
    if (FTriggeredAction.empty())
    {
        FLastErrMsg = "Triggered action is not specified!";
        return FALSE;
    }

    if (FSymbol.empty())
    {
        FLastErrMsg = "The symbol of touch order is missing!";
        return FALSE;
    }

    if (FMarket != nsOrderMessageDefine::mTSE &&
        FMarket != nsOrderMessageDefine::mOTC &&
        FMarket != nsOrderMessageDefine::mTWFutures &&
        FMarket != nsOrderMessageDefine::mTWOptions)
    {
        FLastErrMsg = "The market of touch order is unsupported!";
        return FALSE;
    }

    if (taWarning == FTouchedActionType)
        return TRUE;

    if (FTouchedActionMarket != nsOrderMessageDefine::mTSE &&
        FTouchedActionMarket != nsOrderMessageDefine::mOTC &&
        FTouchedActionMarket != nsOrderMessageDefine::mTWFutures &&
        FTouchedActionMarket != nsOrderMessageDefine::mTWOptions)
    {
        FLastErrMsg = "The market of order to execute is unsupported!";
        return FALSE;
    }

    if (FTouchedActionSymbol.empty())
    {
        FLastErrMsg = "The stock_no/prod_id of order to execute is missing!";
        return FALSE;
    }

    if (taNewOrder == FTouchedActionType)
    {
        if (FMarket != FTouchedActionMarket || FSymbol != FTouchedActionSymbol) // 跨市場/商品觸價單
        {
            if (FTouchedActionOrderType != nsOrderMessageDefine::otMarket)
            {
                if (pdoMatch == FPriceDependOn) // 跨市場/商品觸價單的新單委託委託價不能用觸價時的成交價
                {
                    FLastErrMsg = "Floating order price is forbidden for cross market touched-new order!";
                    return FALSE;
                }
                else if (0.0 == FTouchedActionPrice) // 跨市場/商品觸價單的新單委託必須指定委託價
                {
                    FLastErrMsg = "the order price of cross market touched-new order is missing!";
                    return FALSE;
                }
            }
        }
        else
        {
            if (FTouchedActionOrderType != nsOrderMessageDefine::otMarket &&
                pdoMatch != FPriceDependOn &&
                0.0 == FTouchedActionPrice)
            {
                FLastErrMsg = "the order price of touched-new order is missing!";
                return FALSE;
            }
        }        
    }
    else if (taRpxOrder == FTouchedActionType)
    {
        if (FMarket != FTouchedActionMarket || FSymbol != FTouchedActionSymbol)
        {
            if (pdoMatch == FPriceDependOn) // 跨市場/商品觸價單的新單委託委託價不能用觸價時的成交價
            {
                FLastErrMsg = "Floating order price is forbidden for cross market touched-replace order!";
                return FALSE;
            }
            else if (0.0 == FTouchedActionPrice) // 跨市場/商品觸價單的新單委託必須指定委託價
            {
                FLastErrMsg = "the order price of cross market touched-replace order is missing!";
                return FALSE;
            }
        }
        else
        {
            if (pdoMatch != FPriceDependOn && 0.0 == FTouchedActionPrice)
            {
                FLastErrMsg = "the order price of touched-replace order is missing!";
                return FALSE;
            }
        }        
    }    

    return TRUE;
}
