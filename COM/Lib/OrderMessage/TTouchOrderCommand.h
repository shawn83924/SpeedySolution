//#pragma once
#ifndef _TTOUCHORDERCOMMAND_H_
#define _TTOUCHORDERCOMMAND_H_

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
        tocQuery,   // 查詢觸價單 code:'Q'
        // 以下行為非由使用者控制，僅作為記錄說明
        tocLogon,   // 登入通知   code:'I' 由 API 自動傳送
        tocLogout   // 登出通知   code:'O' 由 server 觸發，只有 server 端觸價模組會收到 
    };

    enum PriceDependOnEnum
    {
        pdoNone = 0, // 觸價時以指定價格為下單時的委託價
        pdoMatch     // 觸價時以成交價+/-檔位(Ticks)為下單時的委託價
    };

    enum TouchedActionEnum
    {
        taUnknown = 0,     // 未知的觸價行為
        taNewOrder,        // 觸價下新單
        taCxlOrder,        // 觸價下刪單
        taRpxOrder,        // 觸價下改單
        taCxlTouchedOrder, // 觸價刪觸價單(觸價買或觸價賣)
        taWarning          // 觸價送訊息       
    };

private:
    
    long long                        FNID;
    TouchedOrderCommandEnum          FCmdType;
    nsOrderMessageDefine::MarketEnum FMarket;
    nsOrderMessageDefine::SideEnum   FSide;
    PriceDependOnEnum                FPriceDependOn;
    int                              FTicks;
    TouchedActionEnum                FTouchedActionType;

    std::string FSymbol;
    std::string FPrice;
    std::string FUserData;
    std::string FTouchOrderID;
    std::map<std::string, std::string> FTriggeredAction;
    std::string FTriggeringCondition;
    std::string FTriggerExpression;
    
    std::string FLastErrMsg;

    double      FTouchedActionPrice;                             // 記錄觸價下單時的委託價
    std::string FTouchedActionSymbol;                            // 記錄觸價下單時的商品代碼
    nsOrderMessageDefine::MarketEnum FTouchedActionMarket;       // 記錄觸價下單時的市場別，方便判斷要下單的商品是期貨或選擇權
    nsOrderMessageDefine::OrderTypeEnum FTouchedActionOrderType; // 記錄觸價下新單時用的是市價還是限價(只對觸價下新單有意義)

private:
    void ClearTriggeredAction();
    BOOL ToTriggeredAction(TNewOrderMessage* order);      
    BOOL ToTriggeredAction(TCancelOrderMessage* order);   
    BOOL ToTriggeredAction(TReplaceOrderMessage* order);  
    BOOL ToTriggeredAction(TTouchOrderCommand* toc);      
    BOOL ToTriggeredAction(const char* warning);    

public:

    TTouchOrderCommand() : FNID(0),
        FCmdType(tocNew),
        FMarket(nsOrderMessageDefine::mUnknown),
        FSide(nsOrderMessageDefine::sNone),
        FPriceDependOn(pdoNone),
        FTicks(0),
        FTouchedActionType(taUnknown),
        FTouchedActionMarket(nsOrderMessageDefine::mUnknown),
        FTouchedActionOrderType(nsOrderMessageDefine::otNone)
    {

    }

    TTouchOrderCommand(TouchedOrderCommandEnum toc_type,
                       const char* touchorder_id,
                       nsOrderMessageDefine::SideEnum side = nsOrderMessageDefine::sNone) : FNID(0),
        FCmdType(toc_type),
        FMarket(nsOrderMessageDefine::mUnknown),
        FSide(side),
        FPriceDependOn(pdoNone),
        FTicks(0),
        FTouchedActionType(taUnknown),
        FTouchedActionMarket(nsOrderMessageDefine::mUnknown),
        FTouchedActionOrderType(nsOrderMessageDefine::otNone)
    {

    }
    // 設定觸價命令類別
    void SetCmdType(TouchedOrderCommandEnum toc_type) { FCmdType = toc_type; }           
    
    // 設定觸價條件跟隨的市場行情 
    void SetMarket(nsOrderMessageDefine::MarketEnum market) { FMarket = market; }        
    
    // 設定觸價條件跟隨的行情標的
    void SetSymbol(const char* symbol) { FSymbol = symbol; }
    
    // 只有在設定觸價刪觸價單時作為 setTiggeredAction(TTouchOrderCommand*) 時輸入的物件有用
    void SetSide(nsOrderMessageDefine::SideEnum side)
    {
        FSide = side;
    }

    // 設定暫停、啟動、移除跟查詢時指定的觸價單單號
    void SetTouchOrderID(const char* id);
    
    // ---- deprecated. "不要用" ---------  
    void SetPrice(const char* price) {}
    
    // 設定觸價下新單或觸價下改單時是以觸價時的成交價加減檔位為委託價                                                                         
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
    
    const char* GetLastErrorMsg() { return FLastErrMsg.c_str(); }

    // ---- 以下功能僅 Speedy API 內部使用，不需要對外開放 ------------
    void SetNID(long long nid)                                      { FNID = nid; }
    TouchedOrderCommandEnum GetCmdType()                            { return FCmdType; }
    nsOrderMessageDefine::MarketEnum GetMarket()                    { return FMarket; }
    std::string GetSymbol()                                         { return FSymbol;  }
    std::string GetUserData()                                       { return FUserData; }
    std::string GetTouchOrderID()                                   { return FTouchOrderID; }
    std::string GetTriggeringCondition()                            { return FTriggeringCondition; }
    std::string GetTriggerExpression()                              { return FTriggerExpression; }
    std::string GetTriggeredAction();                               
    std::string GetTouchedActionSymbol()                            { return FTouchedActionSymbol; }               
    nsOrderMessageDefine::MarketEnum GetTouchedActionMarket()       { return FTouchedActionMarket; }
    TouchedActionEnum                GetTouchedActionType()         { return FTouchedActionType; }
    // 給 TTaifexConnection 在 TouchOrderControl() 時設定錯誤訊息
    void SetLastErrorMsg(const char* msg)                           { FLastErrMsg = msg; }
    // 檢查設定是否正確 
    BOOL Verify();
};

#endif
