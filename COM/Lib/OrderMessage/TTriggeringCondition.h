#pragma once

#include <string>
#include <vector>
#include <set>

enum LogicalComparisonOperator
{
    lcoEQ = 0,  // equal to 
    lcoNE,      // not equal to
    lcoGT,      // greater than
    lcoGE,      // greater than or equal
    lcoLE,      // lesser than or equal
    lcoLT       // lesser than
};

typedef unsigned int SignalID;

class TTriggeringCondition
{
public :
    //using SignalID = unsigned int;

    struct PrimitiveSignal
    {
        enum PrimitiveSignalType
        {
            RealTimePriceComp,
            SellSidePriceComp,
            BuySidePriceComp,
            LastPriceComp,
            BidPriceComp,
            AskPriceComp,
            PeriodAccumVolumeComp,
            VolumeComp,
            BidVolumeComp,
            AskVolumeComp,
            AccumVolumeComp,
            AnyTop5BidVolumeComp,
            AnyTop5AskVolumeComp
        } type;

        LogicalComparisonOperator op;

        struct DecimalNumber
        {
            unsigned int integer_part; // 取 5 位整數
            unsigned int decimal_part; // 取 4 位小數, so 0.1 => 1000
        } price_threshold;

        unsigned int period; // unit: 1/10 secs or 100ms
        unsigned int volume_threshold;
        
        //std::vector<LogicalGateIndex> signal_receivers;
        std::vector<SignalID> output_to; 

        PrimitiveSignal(PrimitiveSignalType pst, LogicalComparisonOperator lco, unsigned int period_or_integer_part, unsigned int volume_or_decimal_part)
        {
            type = pst;
            op = lco;

            if (pst <= PrimitiveSignalType::AskPriceComp)
            {
                period = 0;
                volume_threshold = 0;
                price_threshold.integer_part = period_or_integer_part;
                price_threshold.decimal_part = volume_or_decimal_part;
            }
            else
            {
                price_threshold.integer_part = 0;
                price_threshold.decimal_part = 0;

                if (pst == PrimitiveSignalType::PeriodAccumVolumeComp)
                    period = period_or_integer_part;
                else
                    period = 0;

                volume_threshold = volume_or_decimal_part;
            }
        } 

        std::string ToString(SignalID signal_id)
        {
            char Price[64];
            if (type <= PrimitiveSignalType::AskPriceComp)
                snprintf(Price, sizeof(Price), "%d.%04d", price_threshold.integer_part, price_threshold.decimal_part);
            else
                snprintf(Price, sizeof(Price), "%d.%d", period, volume_threshold);

            return std::to_string(signal_id) + ":" + std::to_string(static_cast<unsigned short>(type)) + "#" + std::to_string(op) + "$" + Price;
        }
    };

    struct ConjunctionGate
    {
        bool inverter_on; // turn Conjunction to Disjunction if inverter_on is true
        std::vector<SignalID> input_from;
        std::vector<SignalID> output_to;

        ConjunctionGate(bool inverted = false) :inverter_on{ inverted } {}
        std::string ToString(SignalID signal_id)
        {
            return std::to_string(signal_id) + ":" + (inverter_on ? "D" : "C");
        }
    };

private:
    std::string lastErrMsg;
    std::string expression;
    bool editing_mode;
    bool syntax_error;

    std::vector<PrimitiveSignal>     primitive_signals;
    std::vector<ConjunctionGate>     logic_gates;
    
private:
    bool CheckEditingMode()
    {
        if (!editing_mode)
        {
            lastErrMsg = "Toggling on Editing mode before setup triggering condition.";
            return false;
        }

        return true;
    }

    bool StringToDecimal(const char* price, unsigned int& integer_part, unsigned int& decimal_part);
    bool CheckPrimitivePreCondition();
    bool CheckLogicGatePreCondition();
    
    std::string PrimitiveSignalToExpression(const PrimitiveSignal& primitive_signal);
    std::string SignalIDToExpression(SignalID signal_id, bool is_root);

public:
    TTriggeringCondition() : editing_mode(false), syntax_error(true){}

    // 開始編輯觸價條件
    void BeginEditing() 
    { 
        syntax_error = true; 
        editing_mode = true; 
    }
    // 停止編輯觸價條件並檢查觸價條件設定是否有誤
    // return false: triggering condition 設定有誤
    //        true:  triggering condition 設定成功
    bool EndEditing();
    // 比較成交價(有成交才算)
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(64) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID MatchPriceComp(LogicalComparisonOperator op, const char* price); 
    // 比較內盤成交價
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(64) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID SellSideMatchPriceComp(LogicalComparisonOperator op, const char* price);
    // 比較外盤成交價
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(64) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID BuySideMatchPriceComp(LogicalComparisonOperator op, const char* price);
    // 比較最後成交價
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(64) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID LastMatchPriceComp(LogicalComparisonOperator op, const char* price);
    // 比較買進價
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(64) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID BidPriceComp(LogicalComparisonOperator op, const char* price);
    // 比較賣出價
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(64) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID AskPriceComp(LogicalComparisonOperator op, const char* price);
    // 比較單量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(64) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID VolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // 比較單位時間內的累積量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(64) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID PeriodAccumVolumeComp(LogicalComparisonOperator op, unsigned int period_in_tenth_sec, unsigned int volume);
    // 比較委買量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(64) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID BidVolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // 比較委賣量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(64) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID AskVolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // 比較總量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(64) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID AccumVolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // 比較五檔其中一檔委買量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(64) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID AnyTop5BidVolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // 比較五檔其中一檔委賣量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(64) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID AnyTop5AskVolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // !inverter_on: 對所列出的訊號做 and 運算 (default)
    //  inverter_on: 對所列出的訊號做 or  運算 
    // return    0:失敗 可能原因：1.訊號數量已超過限制(64) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID Conjunction(SignalID* signal_list, unsigned int count, bool inverter_on = false);
    // 當 xxxxComp() 跟 Conjunction() 回傳 0 或 EndEditing() 回傳 false 時取得錯誤訊息
    const char* GetLastErrMsg() { return lastErrMsg.c_str(); }
    // 將 triggering condition 轉換成可閱讀的表示式。triggering condition 設定有錯 或處於 editing mode 時回傳空字串。
    const char* ToExpression();
    
    // ---- 以下功能僅 Speedy API 內部使用，不需要對外開放 ------------
    // 將 triggering condition 轉換為供程式處理用的字串型態內容。triggering condition 設定有錯 或處於 editing mode 時回傳空字串。
    std::string  ToString();     
};

