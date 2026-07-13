#pragma once
#ifndef TTRIGGERINGCONDITION_H
#define TTRIGGERINGCONDITION_H

#include <string>
#include <vector>
#include <set>
#include <map>

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

    struct PrimitiveSignal // 基礎訊號
    {
        enum PrimitiveSignalType // 基礎訊號類別
        {
			RealTimePriceComp = 0, //  0:'p'    比較成交價(有成交才算)
            SellSidePriceComp,     //  1:'ssp'  比較內盤成交價
            BuySidePriceComp,      //  2:'bsp'  比較外盤成交價
            LastPriceComp,         //  3:'lp'   比較最後成交價
            BidPriceComp,          //  4:'bp'   比較買進價
            AskPriceComp,          //  5:'ap'   比較賣出價
            PeriodAccumVolumeComp, //  6:'pav' 比較單位時間內的累積量
            VolumeComp,            //  7:'v'    比較單量
            BidVolumeComp,         //  8:'bv'   比較委買量
            AskVolumeComp,         //  9:'av'   比較委賣量
            AccumVolumeComp,       // 10:'acv'  比較總量
            AnyTop5BidVolumeComp,  // 11:'a5bv' 比較五檔其中一檔委買量
            AnyTop5AskVolumeComp   // 12:'a5av' 比較五檔其中一檔委賣量
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

        // 將比較運算子(LogicalComparisonOperator)列舉值轉成比較運算子識別碼
        std::string ToString(LogicalComparisonOperator op) const 
        {
            switch (op)
            {
            case lcoNE: return "ne";
            case lcoGT: return "gt";
            case lcoGE: return "ge";
            case lcoLE: return "le";
            case lcoLT: return "lt";
            default: return "eq";
            }
        }

        // 將基礎訊號類別(PrimitiveSignalType)轉成比較項目識別碼
        std::string ToString(PrimitiveSignalType ps_type) const 
        {
            switch (ps_type)
            {
            case RealTimePriceComp:     return "p";
            case SellSidePriceComp:     return "ssp";
            case BuySidePriceComp:      return "bsp";
            case LastPriceComp:         return "lp";
            case BidPriceComp:          return "bp";
            case AskPriceComp:          return "ap";
            case PeriodAccumVolumeComp: return "pacv";
            case VolumeComp:            return "v";
            case BidVolumeComp:         return "bv";
            case AskVolumeComp:         return "av";
            case AccumVolumeComp:       return "acv";
            case AnyTop5BidVolumeComp:  return "a5bv";
            default: return "?";                
            }
        }

        std::string ToString(const DecimalNumber& price) const
        {
            char Buf[64];
            snprintf(Buf, sizeof(Buf), "%d.%04d", price_threshold.integer_part, price_threshold.decimal_part);
            return std::string(Buf);
        }

        std::string ToString(unsigned int period, unsigned int volume) const
        {
            char Buf[64];
            snprintf(Buf, sizeof(Buf), "%d.%d", period, volume);
            return std::string(Buf);
        }

        std::string ToString() const
        {
            if (type <= PrimitiveSignalType::AskPriceComp)
                return ToString(type) + "#" + ToString(op) + "$" + ToString(price_threshold);
            else
                return ToString(type) + "#" + ToString(op) + "$" + ToString(period, volume_threshold);
        }

        std::string ToString(SignalID signal_id) const
        {
            // [訊號代碼]:[價格比較項目識別碼]#[比較運算子識別碼]$[指定價格整數部位].[指定價格小數部位]  => 比較價格
            // [訊號代碼]:[數量比較項目識別碼]#[比較運算子識別碼]$[0].[指定數量限制]                  => 比較數量
            // [訊號代碼]:[數量比較項目識別碼]#[比較運算子識別碼]$[指定累積時間].[指定數量限制]         => 比較指定時間內累積數量
            char Price[64];
            if (type <= PrimitiveSignalType::AskPriceComp)
                snprintf(Price, sizeof(Price), "%d.%04d", price_threshold.integer_part, price_threshold.decimal_part);
            else
                snprintf(Price, sizeof(Price), "%d.%d", period, volume_threshold);

			char IdBuf[32];
			snprintf(IdBuf, sizeof(IdBuf), "%u", signal_id);
			return std::string(IdBuf) + ":" + ToString();
            //return std::to_string(signal_id) + ":" + ToString(type) + "#" + ToString(op) + "$" + Price;
        }
    };

    struct ConjunctionGate // 邏輯運算輸出訊號(邏輯閘)
    {
        bool inverter_on; // turn Conjunction to Disjunction if inverter_on is true
        std::vector<SignalID> input_from;
        std::vector<SignalID> output_to;

		ConjunctionGate(bool inverted = false) :inverter_on( inverted ){}
		std::string ToString(SignalID signal_id)
		{
			// 訊號代碼:D  => 「或(OR)」 邏輯運算輸出
			// 訊號代碼:C  => 「且(AND)」邏輯運算輸出
			char IdBuf[32];
			snprintf(IdBuf, sizeof(IdBuf), "%u", signal_id);
			return std::string(IdBuf) + ":" + (inverter_on ? "D" : "C");
		}
    };

private:
    std::string lastErrMsg;
    std::string expression;
    bool editing_mode;
    bool syntax_error;

    std::vector<PrimitiveSignal>     primitive_signals;  // 所有的價、量比較項目(基礎訊號)
    std::vector<ConjunctionGate>     logic_gates;        // 所有的「或」、「且」邏輯閘
    std::map<std::string, SignalID>  primitive_set;      // 用 項目 + 運算子 + 指定值 當 key，濾掉重複的比對項目
    
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
    bool CheckLogicGatePreCondition(unsigned int input_num);
    
    std::string PrimitiveSignalToExpression(const PrimitiveSignal& primitive_signal);
    std::string SignalIDToExpression(SignalID signal_id, bool is_root);

    SignalID AddPrimitiveSignal(const PrimitiveSignal& primitive_signal);

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
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID MatchPriceComp(LogicalComparisonOperator op, const char* price); 
    // 比較內盤成交價
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID SellSideMatchPriceComp(LogicalComparisonOperator op, const char* price);
    // 比較外盤成交價
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID BuySideMatchPriceComp(LogicalComparisonOperator op, const char* price);
    // 比較最後成交價
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID LastMatchPriceComp(LogicalComparisonOperator op, const char* price);
    // 比較買進價
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID BidPriceComp(LogicalComparisonOperator op, const char* price);
    // 比較賣出價
    // return    0:失敗 可能原因：1.價格格式錯誤 2.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 3.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID AskPriceComp(LogicalComparisonOperator op, const char* price);
    // 比較單量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID VolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // 比較單位時間內的累積量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID PeriodAccumVolumeComp(LogicalComparisonOperator op, unsigned int period_in_tenth_sec, unsigned int volume);
    // 比較委買量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID BidVolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // 比較委賣量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID AskVolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // 比較總量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID AccumVolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // 比較五檔其中一檔委買量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
    SignalID AnyTop5BidVolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // 比較五檔其中一檔委賣量
    // return    0:失敗 可能原因：1.訊號數量已超過限制(MAX_PRIMITIVE_SIGNAL_NUM) 2.未呼叫 BeginEditing() 進入編輯模式 
    //         非0:成功 回傳的訊號代碼可做為 Conjuction 的 input 進行 AND 或 OR 運算
	SignalID AnyTop5AskVolumeComp(LogicalComparisonOperator op, unsigned int volume);
    // !inverter_on: 對所列出的訊號做 and 運算 (default)
    //  inverter_on: 對所列出的訊號做 or  運算 
    // return    0:失敗 可能原因：1.邏輯閘數量已超過限制(MAX_LOGIC_GATE_NUM) 2.輸入訊號數量已超過限制(MAX_SIGNAL_IN_NUM) 3.未呼叫 BeginEditing() 進入編輯模式 
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

#endif
