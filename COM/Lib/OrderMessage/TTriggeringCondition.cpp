#include "TTriggeringCondition.h"

#include <sstream>
#include <queue>
#include <stdlib.h>

const size_t       MAX_PRIMITIVE_SIGNAL_NUM = 16;
const size_t       MAX_LOGIC_GATE_NUM       = 16;
const unsigned int MAX_SIGNAL_IN_NUM        = 16;

// 將字串型態的浮點數解析出整數部位跟小數部位，小數部位數值會跟根據指定的小數位數(decimal locator)做修正
// ex. 1. 輸入 "12.3" decimal_locator:4 ，這時輸入會被視為是 "12.3000" 所以 integer_part 會是 12 decimal_part 會是 3000 
//     2. 輸入 "0.0123" decimal_locator:4 ，回傳的 integer_part 為 0  decimal_part 為 123
bool TTriggeringCondition::StringToDecimal(const char* price, unsigned int decimal_locator, unsigned int& integer_part, unsigned int& decimal_part)
{
    std::string s_price = price;
    std::string s_integer_part;
    std::string s_decimal_part;
    std::string::size_type npos = s_price.find('.');
    if (npos == std::string::npos)
        s_integer_part = s_price;
    else
    {
        s_integer_part = s_price.substr(0, npos);
        s_decimal_part = s_price.substr(npos + 1);
        if (s_decimal_part.size() > decimal_locator)
            s_decimal_part = s_decimal_part.substr(0, decimal_locator);
        while (s_decimal_part.size() < decimal_locator)
            s_decimal_part.push_back('0');
    }

    if (s_integer_part.empty())
        s_integer_part = "0";
    if (s_decimal_part.empty())
        s_decimal_part = "0";

    for (size_t i = 0; i < s_integer_part.size(); ++i)
    {
        if (!isdigit(s_integer_part[i]))
        {
            lastErrMsg = "Invalid price value format!";
            return false;
        }
    }
    for (size_t i = 0; i < s_decimal_part.size(); ++i)
    {
        if (!isdigit(s_decimal_part[i]))
        {
            lastErrMsg = "Invalid price value format!";
            return false;
        }
    }

    integer_part = atoi(s_integer_part.c_str());
    decimal_part = atoi(s_decimal_part.c_str());

    /*
    try
    {
        integer_part = std::stoi(s_integer_part);
        decimal_part = std::stoi(s_decimal_part);        
    }
    catch (...)
    {
        lastErrMsg = "Invalid price value format!";
        return false;
    }
    */

    return true;
}

bool TTriggeringCondition::CheckPrimitivePreCondition()
{
    if (!CheckEditingMode())
        return false;

    if (primitive_signals.size() >= MAX_PRIMITIVE_SIGNAL_NUM)
    {
        //lastErrMsg = std::string("Too many Primitive Signals! Limitation is ") + std::to_string(MAX_PRIMITIVE_SIGNAL_NUM);
        std::stringstream ss;
        ss << "Too many Primitive Signals! Limitation is " << MAX_PRIMITIVE_SIGNAL_NUM;
        lastErrMsg = ss.str();

        return false;
    }

    return true;
}

bool TTriggeringCondition::CheckLogicGatePreCondition(unsigned int input_num)
{
    if (!CheckEditingMode())
        return false;

    if (logic_gates.size() >= MAX_LOGIC_GATE_NUM)
    {
        //lastErrMsg = std::string{ "Too many Logic Gates! Limitation is " } + std::to_string(MAX_LOGIC_GATE_NUM);
        std::stringstream ss;
        ss << "Too many Logic Gates! Limitation is " << MAX_LOGIC_GATE_NUM;
        lastErrMsg = ss.str();
        return false;
    }

    if (input_num > MAX_SIGNAL_IN_NUM)
    {
        //lastErrMsg = std::string{ "Too many signal input! Limitation is " } + std::to_string(MAX_SIGNAL_IN_NUM);
        std::stringstream ss;
        ss << "Too many signal input! Limitation is " << MAX_SIGNAL_IN_NUM;
        lastErrMsg = ss.str();
        return false;
    }

    return true;
}

std::string TTriggeringCondition::PrimitiveSignalToExpression(const TTriggeringCondition::PrimitiveSignal& primitive_signal)
{
    std::stringstream ss;
    switch (primitive_signal.type)
    {
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstRealTimePriceComp:
        ss << "${match_price}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstSellSidePriceComp:
        ss << "${sellside_price}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstBuySidePriceComp:
        ss << "${buyside_price}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstLastPriceComp:
        ss << "${last_price}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstBidPriceComp:
        ss << "${bid_price}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstAskPriceComp:
        ss << "${ask_price}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstPeriodAccumVolumeComp:
        ss << "${accum_volume_in_" << primitive_signal.period * 100 << "ms}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstVolumeComp:
        ss << "${volume}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstBidVolumeComp:
        ss << "${bid_volume}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstAskVolumeComp:
        ss << "${ask_volume}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstAccumVolumeComp:
        ss << "${accum_volume}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstAnyTop5BidVolumeComp:
        ss << "${any_bid_volume}";
        break;
    case PrimitiveSignal::/*PrimitiveSignalType::*/pstAnyTop5AskVolumeComp:
        ss << "${any_ask_volume}";
        break;
    default:
        ss << "${unknown_item}";
        break;
    }

    switch (primitive_signal.op)
    {
    case lcoNE:
        ss << "!=";
        break;
    case lcoGT:
        ss << ">";
        break;
    case lcoGE:
        ss << ">=";
        break;
    case lcoLE:
        ss << "<=";
        break;
    case lcoLT:
        ss << "<";
        break;
    default: // locEQ
        ss << "==";
        break;
    }

    if (primitive_signal.type <= PrimitiveSignal::/*PrimitiveSignalType::*/pstAskPriceComp)
    {
        ss << primitive_signal.price_threshold.integer_part;
        
        if (primitive_signal.price_threshold.decimal_part)
        {
            char buf[64];
            snprintf(buf, sizeof(buf), ".%04d", primitive_signal.price_threshold.decimal_part);
            ss << buf;
        }       
    }
    else
    {
        ss << primitive_signal.volume_threshold;
    }
    
    return ss.str();
}

std::string TTriggeringCondition::SignalIDToExpression(SignalID signal_id, bool is_root)
{
    unsigned int category = (signal_id >> 8) & 0x03;
    unsigned int item_index = signal_id & 0xFF;

    if (2 == category) // logic gate
    {
        std::stringstream ss;
        if (!is_root)
            ss << "(";

        bool first_item = true;
        std::string op = " && ";
        if (logic_gates[item_index].inverter_on)
            op = " || ";

        //for (const auto& signal_id : logic_gates[item_index].input_from)
        for(size_t i=0; i< logic_gates[item_index].input_from.size(); ++i)
        {   
            SignalID signal_id = logic_gates[item_index].input_from.at(i);
            if (first_item)
                first_item = false;
            else
                ss << op;

            ss << SignalIDToExpression(signal_id, false);
        }

        if (!is_root)
            ss << ")";

        return ss.str();
    }
    else // primitive signal
    {
        return PrimitiveSignalToExpression(primitive_signals[item_index]);
    }
}

SignalID TTriggeringCondition::AddPrimitiveSignal(const TTriggeringCondition::PrimitiveSignal& primitive_signal)
{
    std::string key = primitive_signal.ToString();
    std::map<std::string, SignalID>::iterator iter = primitive_set.find(key);
    if (iter == primitive_set.end())
    {
        size_t primitive_signal_index = primitive_signals.size();
        primitive_signals.push_back(primitive_signal);

        unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
        primitive_set.insert(std::make_pair(key, signal_id));
        return signal_id;
    }
    else
        return iter->second;
}

void TTriggeringCondition::BeginEditing()
{
    lastErrMsg.clear();
    expression.clear();
    
    primitive_signals.clear();
    logic_gates.clear();      
    primitive_set.clear();    

    exec_time.enabled = false;

    syntax_error = true;
    editing_mode = true;
}

bool TTriggeringCondition::EndEditing()
{
    if (primitive_signals.size() == 1)
    {
        editing_mode = false;
        syntax_error = false;
        return true;
    }

    if (primitive_signals.empty() && !exec_time.enabled)
    {
        editing_mode = false;
        syntax_error = true;
        lastErrMsg = "Triggering condition is blank!";
        return false;
    }

    if (!primitive_signals.empty())
    {
        for (size_t i = 0; i < primitive_signals.size(); ++i)
        {
            unsigned int signal_id = static_cast<unsigned int>(i) | 0x0100;
            if (primitive_signals[i].output_to.empty())
            {
                editing_mode = false;
                syntax_error = true;
                //lastErrMsg = std::string{ "Primitive signal " } + std::to_string(signal_id) + " is unused.";
                std::stringstream ss;
                ss << "Primitive signal " << signal_id << " is unused.";
                lastErrMsg = ss.str();
                return false;
            }
        }

        std::vector<SignalID> root_nodes;
        for (size_t i = 0; i < logic_gates.size(); ++i)
        {
            unsigned int signal_id = static_cast<unsigned int>(i) | 0x0200;
            if (logic_gates[i].output_to.empty())
                root_nodes.push_back(signal_id);
        }

        if (root_nodes.size() > 1)
        {
            editing_mode = false;
            syntax_error = true;
            lastErrMsg = "There are too many signals output!";
            return false;
        }
    }

    editing_mode = false;
    syntax_error = false;
    return true;
}

// 比較成交價(有成交才算)
SignalID TTriggeringCondition::MatchPriceComp(LogicalComparisonOperator op, const char* price)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    std::pair<unsigned int, unsigned int> decimal_number; // (first:integer_part, second:decimal_part)

    if (!StringToDecimal(price, 4, decimal_number.first, decimal_number.second))
        return 0;
    
    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstRealTimePriceComp, op, decimal_number.first, decimal_number.second);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto key = primitive_signal.ToString();
    auto iter = primitive_set.find(key);
    if (iter == primitive_set.end())
    {
        auto primitive_signal_index = primitive_signals.size();
        primitive_signals.push_back(primitive_signal);

        unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
        primitive_set.insert(std::make_pair(key, signal_id));
        return signal_id;
    }
    else
        return iter->second;
    */
}

// 比較內盤成交價
SignalID TTriggeringCondition::SellSideMatchPriceComp(LogicalComparisonOperator op, const char* price)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    std::pair<unsigned int, unsigned int> decimal_number; // (first:integer_part, second:decimal_part)

    if (!StringToDecimal(price, 4, decimal_number.first, decimal_number.second))
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstSellSidePriceComp, op, decimal_number.first, decimal_number.second);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);    

    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

// 比較外盤成交價
SignalID TTriggeringCondition::BuySideMatchPriceComp(LogicalComparisonOperator op, const char* price)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    std::pair<unsigned int, unsigned int> decimal_number; // (first:integer_part, second:decimal_part)

    if (!StringToDecimal(price, 4, decimal_number.first, decimal_number.second))
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstBuySidePriceComp, op, decimal_number.first, decimal_number.second);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);    

    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

// 比較最後成交價
SignalID TTriggeringCondition::LastMatchPriceComp(LogicalComparisonOperator op, const char* price)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    std::pair<unsigned int, unsigned int> decimal_number; // (first:integer_part, second:decimal_part)

    if (!StringToDecimal(price, 4, decimal_number.first, decimal_number.second))
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstLastPriceComp, op, decimal_number.first, decimal_number.second);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);    

    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

// 比較買進價
SignalID TTriggeringCondition::BidPriceComp(LogicalComparisonOperator op, const char* price)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    std::pair<unsigned int, unsigned int> decimal_number; // (first:integer_part, second:decimal_part)

    if (!StringToDecimal(price, 4, decimal_number.first, decimal_number.second))
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstBidPriceComp, op, decimal_number.first, decimal_number.second);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);    

    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

// 比較賣出價
SignalID TTriggeringCondition::AskPriceComp(LogicalComparisonOperator op, const char* price)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    std::pair<unsigned int, unsigned int> decimal_number; // (first:integer_part, second:decimal_part)

    if (!StringToDecimal(price, 4, decimal_number.first, decimal_number.second))
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstAskPriceComp, op, decimal_number.first, decimal_number.second);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);
    
    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

// 比較單量
SignalID TTriggeringCondition::VolumeComp(LogicalComparisonOperator op, unsigned int volume)
{
    if (!CheckPrimitivePreCondition())
        return 0;
        
    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstVolumeComp, op, 0, volume);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);
    
    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

// 比較單位時間內的累積量
SignalID TTriggeringCondition::PeriodAccumVolumeComp(LogicalComparisonOperator op, unsigned int period_in_tenth_sec, unsigned int volume)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstPeriodAccumVolumeComp, op, period_in_tenth_sec, volume);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);    

    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

// 比較委買量
SignalID TTriggeringCondition::BidVolumeComp(LogicalComparisonOperator op, unsigned int volume)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstBidVolumeComp, op, 0, volume);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);    

    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

// 比較委賣量
SignalID TTriggeringCondition::AskVolumeComp(LogicalComparisonOperator op, unsigned int volume)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstAskVolumeComp, op, 0, volume);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);    

    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

// 比較總量
SignalID TTriggeringCondition::AccumVolumeComp(LogicalComparisonOperator op, unsigned int volume)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstAccumVolumeComp, op, 0, volume);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);    

    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

// 比較五檔其中一檔委買量
SignalID TTriggeringCondition::AnyTop5BidVolumeComp(LogicalComparisonOperator op, unsigned int volume)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstAnyTop5BidVolumeComp, op, 0, volume);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);    

    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

// 比較五檔其中一檔委賣量
SignalID TTriggeringCondition::AnyTop5AskVolumeComp(LogicalComparisonOperator op, unsigned int volume)
{
    if (!CheckPrimitivePreCondition())
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::/*PrimitiveSignalType::*/pstAnyTop5AskVolumeComp, op, 0, volume);
    return AddPrimitiveSignal(primitive_signal);
    /*
    auto primitive_signal_index = primitive_signals.size();
    primitive_signals.push_back(primitive_signal);
    
    unsigned int signal_id = static_cast<unsigned int>(primitive_signal_index) | 0x0100;
    return signal_id;
    */
}

SignalID TTriggeringCondition::Conjunction(SignalID* signal_id_list, unsigned int count, bool inverter_on)
{
    if (!CheckLogicGatePreCondition(count))
        return 0;

    std::vector<SignalID> signals_in; // 所有訊號輸入列表(排除掉重複的訊號輸入)
    std::set<unsigned int> logic_gates_in; // 邏輯閘訊號輸入(用來排除重複輸入)
    std::set<unsigned int> primitives_in;  // 基礎訊號輸入(用來排除重複輸入)

    for (unsigned int i = 0; i < count; ++i)
    {
        unsigned int signal_in = signal_id_list[i];             

        unsigned int category = (signal_in >> 8) & 0x03;
        unsigned int index = signal_in & 0x00FF;

        if (category == 1) // primitive signal
        {
            if (primitives_in.find(index) == primitives_in.end())
            {
                primitives_in.insert(index);
                signals_in.push_back(signal_in);                
            }
        }
        else if (category == 2) // logic gate
        {
            if (logic_gates_in.find(index) == logic_gates_in.end())
            {
                logic_gates_in.insert(index);
                signals_in.push_back(signal_in);               
            }
        }
    } 

    ConjunctionGate logic_gate(inverter_on);
    logic_gate.input_from.swap(signals_in);

    unsigned int logic_gate_index = static_cast<unsigned int>(logic_gates.size());
    //logic_gates.push_back(std::move(logic_gate));
    logic_gates.push_back(logic_gate);
    unsigned int signal_id = static_cast<unsigned int>(logic_gate_index) | 0x0200;

    //for (auto& primitive_index : primitives_in)
    for (std::set<unsigned int>::iterator iter = primitives_in.begin(); iter != primitives_in.end(); ++iter)
    {
        unsigned int primitive_index = *iter;
        primitive_signals[primitive_index].output_to.push_back(signal_id);
    }
    
    //for (auto& logic_gate_index : logic_gates_in)
    for (std::set<unsigned int>::iterator iter = logic_gates_in.begin(); iter != logic_gates_in.end(); ++iter)
    {
        unsigned int logic_gate_index = *iter;
        logic_gates[logic_gate_index].output_to.push_back(signal_id);
    }
    
    return signal_id;
}

bool TTriggeringCondition::SetExecutionTime(unsigned int hour, unsigned int minute)
{
    if (!CheckEditingMode())
        return false;

    if (hour > 23 || minute > 59)
    {
        lastErrMsg = "Invalid time value.";
        syntax_error = true;
        return false;
    }

    exec_time.enabled = true;
    exec_time.hour = hour;
    exec_time.minute = minute;

    return true;
}

// Note: 
//        TTriggeringCondition::ToExpression() 是輸出「使用者可讀」的觸價條件設定
//        TTriggeringCondition::ToString() 是輸出供「程式解析」用的觸價條件設定
const char* TTriggeringCondition::ToExpression()
{
    if (editing_mode)
    {
        lastErrMsg = "Expression is not available when editing mode is on.";
        return "";
    }

    if (syntax_error)
    {
        lastErrMsg = "Expression is not available when syntax error exist.";
        return "";
    }

    if (!primitive_signals.empty())
    {
        if (primitive_signals.size() == 1)
        {
            expression = PrimitiveSignalToExpression(primitive_signals.front());
            return expression.c_str();
        }

        for (size_t i = 0; i < logic_gates.size(); ++i)
        {
            SignalID signal_id = static_cast<SignalID>(i) | 0x0200;
            if (logic_gates[i].output_to.empty())
            {
                expression = SignalIDToExpression(signal_id, true);
                return expression.c_str();
            }
        }
    }
    else if (exec_time.enabled)
    {
        char hhmm[64];
        snprintf(hhmm, sizeof(hhmm), "%02d:%02d", exec_time.hour, exec_time.minute);
        expression = std::string("${current_time}>=") + hhmm;
        return expression.c_str();
    }

    return "";
}

const std::string GROUP_SEPARATOR       = ";";
const std::string SIGNAL_UNIT_SEPARATOR = ",";
const std::string SIGNAL_ID_SEPARATOR   = ":";

std::string TTriggeringCondition::ToString() 
{
    if (editing_mode)
    {
        lastErrMsg = "Instruction is not available when editing mode is on.";
        return "";
    }

    if (syntax_error)
    {
        lastErrMsg = "Instruction is not available when syntax error exist.";
        return "";
    }

    // the format of output string
    // triggering condition (scene)  ---> 訊號組成列表;訊號關聯性列表;指定執行時間;
    // 訊號組成列表                    ---> 訊號單元1,訊號單元2,.....訊號單元N
    //
    // 訊號單元                       ---> [訊號代碼]:[價格比較項目識別碼]#[比較運算子識別碼]$[指定價格整數部位].[指定價格小數部位]  => 比較價格
    //                               ---> [訊號代碼]:[數量比較項目識別碼]#[比較運算子識別碼]$[0].[指定數量限制]                  => 比較數量
    //                               ---> [訊號代碼]:[數量比較項目識別碼]#[比較運算子識別碼]$[指定累積時間].[指定數量限制]         => 比較指定時間內累積數量
    //                               ---> [訊號代碼]:D => disjunction, 「OR」邏輯閘
    //                               ---> [訊號代碼]:C => conjunction, 「AND」邏輯閘
    // 
    // 比較項目識別碼 PrimitiveSignal::ToString(PrimitiveSignalType)
    //     'p'    比較成交價(有成交才算)
    //     'ssp'  比較內盤成交價
    //     'bsp'  比較外盤成交價
    //     'lp'   比較最後成交價
    //     'bp'   比較買進價
    //     'ap'   比較賣出價
    //     'pacv'  比較單位時間內的累積量
    //     'v'    比較單量
    //     'bv'   比較委買量
    //     'av'   比較委賣量
    //     'acv'  比較總量
    //     'a5bv' 比較五檔其中一檔委買量
    //     'a5av' 比較五檔其中一檔委賣量
    // 
    // 比較運算子識別碼 PrimitiveSignal::ToString(LogicalComparisonOperator)
    //     'eq'   等於
    //     'ne'   不等於
    //     'gt'   大於
    //     'ge'   大於或等於
    //     'le'   小於或等於
    //     'lt'   小於
    // 
    // 訊號關聯性列表                  ---> [標的訊號代碼:來源訊號代碼],[標的訊號代碼:來源訊號代碼],.....,[標的訊號代碼:來源訊號代碼] 
    // 
    // 訊號代碼編碼原則
    //                  0b00000001xxxxxxxx   -> 訊號是價、量比較結果輸出
    //                  0b00000010xxxxxxxx   -> 訊號是 OR, AND 運算結果輸出
    std::stringstream ss;
    if (!primitive_signals.empty())
    {
        // 1. 輸出訊號組成列表
        //   1.1. 輸出基礎訊號單元
        for (size_t i = 0; i < primitive_signals.size(); ++i)
        {
            SignalID signal_id = static_cast<SignalID>(i) | 0x0100;
            if (i)
                ss << SIGNAL_UNIT_SEPARATOR; //",";
            ss << primitive_signals[i].ToString(signal_id);
        }

        if (primitive_signals.size() == 1)
        {
            ss << GROUP_SEPARATOR << GROUP_SEPARATOR << GROUP_SEPARATOR; //";;;";
            return ss.str();
        }

        // 1.2. 輸出邏輯閘訊號單元
        std::queue<SignalID> gate_queue;
        for (size_t i = 0; i < logic_gates.size(); ++i)
        {
            SignalID signal_id = static_cast<SignalID>(i) | 0x0200;
            if (logic_gates[i].output_to.empty() && gate_queue.empty())
                gate_queue.push(signal_id);
            ss << SIGNAL_UNIT_SEPARATOR /*","*/ << logic_gates[i].ToString(signal_id);
        }

        ss << GROUP_SEPARATOR;//";";

        // 2. 輸出訊號關聯性列表
        bool first_item(true);
        while (!gate_queue.empty())
        {
            SignalID root_signal_id = gate_queue.front();
            gate_queue.pop();

            unsigned int root_gate_index = root_signal_id & 0xFF;
            //for (const auto& signal_id : logic_gates[root_gate_index].input_from)
            for (size_t i = 0; i < logic_gates[root_gate_index].input_from.size(); ++i)
            {
                SignalID signal_id = logic_gates[root_gate_index].input_from.at(i);
                unsigned int category = (signal_id >> 8) & 0x03;
                if (2 == category) // logic gate
                    gate_queue.push(signal_id);

                if (first_item)
                    first_item = false;
                else
                    ss << SIGNAL_UNIT_SEPARATOR;//",";

                //ss << std::to_string(root_signal_id) << ":" << signal_id;
                // 輸出「標的訊號代碼:來源訊號代碼」
                ss << root_signal_id << SIGNAL_ID_SEPARATOR /*":"*/ << signal_id;
            }
        }

        ss << GROUP_SEPARATOR << GROUP_SEPARATOR;//";;";
    }
    else
    {
        char hhmm[64];
        snprintf(hhmm, sizeof(hhmm), "%02d:%02d", exec_time.hour, exec_time.minute);
        ss << GROUP_SEPARATOR << GROUP_SEPARATOR /*";;"*/ << hhmm << GROUP_SEPARATOR;//";";
    }

    return ss.str();
}