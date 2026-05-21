#pragma once
#ifndef TTOUCHORDERCOMMAND_H
#define TTOUCHORDERCOMMAND_H

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
    std::string FTriggerExpression;

private:
    BOOL ToTriggeredAction(TNewOrderMessage* order);      
    BOOL ToTriggeredAction(TCancelOrderMessage* order);   
    BOOL ToTriggeredAction(TReplaceOrderMessage* order);  
    BOOL ToTriggeredAction(TTouchOrderCommand* toc);      
    BOOL ToTriggeredAction(const char* warning);

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
    // 設定觸價命令類別
    void SetCmdType(TouchedOrderCommandEnum toc_type) { FCmdType = toc_type; }           
    // 設定觸價條件跟隨的市場行情 
    void SetMarket(nsOrderMessageDefine::MarketEnum market) { FMarket = market; }        
    // 設定觸價條件跟隨的行情標的
    void SetSymbol(const char* symbol) { FSymbol = symbol; }
    // 原意為設定觸價後刪委託單/改價/刪觸價單時所針對的委託/觸價單買賣別，現在保留介面供其他用途，請透過下列方式指定買賣別
    //   .觸價後刪委託單請透過 TCancelOrderMessage 物件的 SetSide() 來指定
    //   .觸價後改價請透過 TReplaceOrderMessage 物件的 SetSide() 來指定
    //   .觸價後刪觸價單請透過 TTouchOrderCommand 物件的 SetSide() 來指定
    void SetSide(nsOrderMessageDefine::SideEnum side);
    // 設定暫停、啟動、移除跟查詢時指定的觸價單單號
    // 若是用在觸價後刪觸價單則是用來設定要刪除的觸價單的單號，在這種情況下
    //   SetTouchOrderID 所設定的單號會優先於 SetTriggeredAction 裡 TTouchOrderCommand 物件所設定的單號 
    void SetTouchOrderID(const char* id);
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
    void SetPrice(const char* price);                                                    
    // 設定觸價下新單或觸價下改單時是以觸價時的成交價加減檔位為委託價                                                                         
    // 設定價格方式有3種，優先順序為
    //   1. 透過 SetFloatingOrderPrice() 設定委託價是成交價的加減檔位數
    //   2. 透過 SetPrice() 設定,且用字串指定的數值是正常的數值格式 NNNNN.NNNN or NNNNN N 為 0~9 的數值
    //      。整數大於5位時左方數字會被切除，小數部位大於4位時右方數字會被切除。
    //   3. 透過 TReplaceOrderMessage::SetPrice() 或 TNewOrderMessage::SetPrice() 設定的數值
    void SetFloatingOrderPrice(PriceDependOnEnum depend_on, int ticks);
    // 設定觸價單的自訂定義資料，查詢或觸價回報時會回傳
    void SetUserData(const char* data)                   { FUserData = data; }
    // 設定觸價後下新單
    BOOL SetTriggeredAction(TNewOrderMessage* order)     { return ToTriggeredAction(order); }   
    // 設定觸價後下刪單
    BOOL SetTriggeredAction(TCancelOrderMessage* order)  { return ToTriggeredAction(order); }   
    // 設定觸價後下改單
    BOOL SetTriggeredAction(TReplaceOrderMessage* order) { return ToTriggeredAction(order); }   
    // 設定觸價後刪觸價單
    BOOL SetTriggeredAction(TTouchOrderCommand* toc)     { return ToTriggeredAction(toc); }     
    // 設定觸價後發警示用
    BOOL SetTriggeredAction(const char* msg)             { return ToTriggeredAction(msg); }     
    // 設定設定觸價條件
    void SetTriggeringCondition(TTriggeringCondition* ttc);                              

    long long GetNID() { return FNID; }

    // ---- 以下功能僅 Speedy API 內部使用，不需要對外開放 ------------
    void SetNID(long long nid)                  { FNID = nid; }
    TouchedOrderCommandEnum GetCmdType()        { return FCmdType; }
    nsOrderMessageDefine::MarketEnum GetMarket(){ return FMarket; }
    std::string GetSymbol()                     { return FSymbol;  }
    std::string GetUserData()                   { return FUserData; }
    std::string GetTouchOrderID()               { return FTouchOrderID; }
    std::string GetTriggeringCondition()        { return FTriggeringCondition; }
    std::string GetTriggerExpression()          { return FTriggerExpression; }
    std::string GetTriggeredAction();
};

#endif
