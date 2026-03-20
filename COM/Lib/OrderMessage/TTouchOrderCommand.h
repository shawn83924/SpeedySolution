#pragma once

#include "TNewOrderMessage.h"
#include "TCancelOrderMessage.h"
#include "TReplaceOrderMessage.h"
#include "TTriggeringCondition.h"

#include <string>
#include <map>

class TTouchOrderCommand
{
public:

   enum TouchedOrderCommandEnum
   {
        tocNew = 0, // 新增觸價單 code:'N'
        tocPause,   // 暫停觸價單 code:'P'
        tocActive,  // 啟動觸價單 code:'A'
        tocKill,    // 移除觸價單 code:'K'
        tocQuery    // 查詢觸價單 code:'Q'
    };

    enum PriceDependOnEnum
    {
        pdoNone = 0, // 觸價時以指定價格為下單時的委託價
        pdoMatch     // 觸價時以成交價+/-檔位(Ticks)為下單時的委託價
    };

private:
    
    long long                        FNID;
    TouchedOrderCommandEnum          FCmdType;
    nsOrderMessageDefine::MarketEnum FMarket;
    nsOrderMessageDefine::SideEnum   FSide;
    PriceDependOnEnum                FPriceDependOn;
    int                              FTicks;

    std::string FSymbol;
    std::string FPrice;
    std::string FUserData;
    std::string FTouchOrderID;
    std::map<std::string, std::string> FTriggeredAction;
    std::string FTriggeringCondition;  

private:
    void ToTriggeredAction(TNewOrderMessage* order);      
    void ToTriggeredAction(TCancelOrderMessage* order);   
    void ToTriggeredAction(TReplaceOrderMessage* order);  
    void ToTriggeredAction(TTouchOrderCommand* toc);      
    void ToTriggeredAction(const char* warning);

public:

    TTouchOrderCommand() : FNID(0),
        FCmdType(TouchedOrderCommandEnum::tocNew),
        FMarket(nsOrderMessageDefine::MarketEnum::mTSE),
        FSide(nsOrderMessageDefine::SideEnum::sNone),
        FPriceDependOn(PriceDependOnEnum::pdoNone),
        FTicks(0)
    {

    }

    TTouchOrderCommand(TouchedOrderCommandEnum toc_type,
                       const char* touchorder_id,
                       nsOrderMessageDefine::SideEnum side = nsOrderMessageDefine::SideEnum::sNone) : FNID(0),
        FCmdType(toc_type),
        FMarket(nsOrderMessageDefine::MarketEnum::mTSE),
        FSide(side),
        FPriceDependOn(PriceDependOnEnum::pdoNone),
        FTicks(0)
    {

    }

    void SetCmdType(TouchedOrderCommandEnum toc_type) { FCmdType = toc_type; }     // 設定觸價命令類別
    void SetMarket(nsOrderMessageDefine::MarketEnum market) { FMarket = market; }  // 設定觸價條件跟隨的市場行情 
    void SetSymbol(const char* symbol) { FSymbol = symbol; }                       // 設定觸價條件跟隨的行情標的
    void SetSide(nsOrderMessageDefine::SideEnum side) { FSide = side; }            // 設定觸發行為(TriggeredAction)時所針對的委託/觸價單買賣別
    void SetTouchOrderID(const char* id) { FTouchOrderID = id; }                   // 設定暫停、啟動、移除跟查詢時指定的觸價單單號
    void SetPrice(const char* price) { FPrice = price; }                           // 設定觸發行為(TriggeredAction)時所針對的以特定價格委託的委託單
    void SetFloatingOrderPrice(PriceDependOnEnum depend_on, int ticks)             // 若觸價下新單時的委託價是根據成交價用這個來設定
    {
        FPriceDependOn = depend_on;
        FTicks = ticks;
    }
    void SetUserData(const char* data)                   { FUserData = data; }           // 設定觸價單的自訂定義資料，查詢或觸價回報時會回傳
    void SetTriggeredAction(TNewOrderMessage* order)     { ToTriggeredAction(order); }   // 觸價時下新單用這個來設定
    void SetTriggeredAction(TCancelOrderMessage* order)  { ToTriggeredAction(order); }   // 觸價時下刪單用這個來設定
    void SetTriggeredAction(TReplaceOrderMessage* order) { ToTriggeredAction(order); }   // 觸價時下改單用這個來設定
    void SetTriggeredAction(TTouchOrderCommand* toc)     { ToTriggeredAction(toc); }     // 觸價時刪觸價單用這個來設定
    void SetTriggeredAction(const char* msg)             { ToTriggeredAction(msg); }     // 觸價時發警示用這個來設定
    void SetTriggeringCondition(TTriggeringCondition* ttc);                              // 設定觸價條件

    long long GetNID() { return FNID; }

    // ---- 以下功能僅 Speedy API 內部使用，不需要對外開放 ------------
    void SetNID(long long nid)                  { FNID = nid; }
    TouchedOrderCommandEnum GetCmdType()        { return FCmdType; }
    nsOrderMessageDefine::MarketEnum GetMarket(){ return FMarket; }
    std::string GetUserData()                   { return FUserData; }
    std::string GetTouchOrderID()               { return FTouchOrderID; }
    std::string GetTriggeringCondition()        { return FTriggeringCondition; }
    std::string GetTriggeredAction();
};