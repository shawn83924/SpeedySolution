#include "TTriggeringCondition.h"

#include <sstream>
#include <queue>

const size_t       MAX_PRIMITIVE_SIGNAL_NUM = 16;
const size_t       MAX_LOGIC_GATE_NUM       = 16;
const unsigned int MAX_SIGNAL_IN_NUM        = 16;


bool TTriggeringCondition::StringToDecimal(const char* price, unsigned int& integer_part, unsigned int& decimal_part)
{
    std::string s_price = price;
    std::string s_integer_part;
    std::string s_decimal_part{ "0000" };
    auto npos = s_price.find('.');
    if (npos == std::string::npos)
        s_integer_part = s_price;
    else
    {
        s_integer_part = s_price.substr(0, npos);
        s_decimal_part = s_price.substr(npos + 1);
        if (s_decimal_part.size() > 4)
            s_decimal_part = s_decimal_part.substr(0, 4);
        while (s_decimal_part.size() < 4)
            s_decimal_part.push_back('0');
    }

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

    return true;
}

bool TTriggeringCondition::CheckPrimitivePreCondition()
{
    if (!CheckEditingMode())
        return false;

    if (primitive_signals.size() >= MAX_PRIMITIVE_SIGNAL_NUM)
    {
        lastErrMsg = std::string{ "Too many Primitive Signals! Limitation is " } + std::to_string(MAX_PRIMITIVE_SIGNAL_NUM);
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
        lastErrMsg = std::string{ "Too many Logic Gates! Limitation is " } + std::to_string(MAX_LOGIC_GATE_NUM);
        return false;
    }

    if (input_num > MAX_SIGNAL_IN_NUM)
    {
        lastErrMsg = std::string{ "Too many signal input! Limitation is " } + std::to_string(MAX_SIGNAL_IN_NUM);
        return false;
    }

    return true;
}

std::string TTriggeringCondition::PrimitiveSignalToExpression(const TTriggeringCondition::PrimitiveSignal& primitive_signal)
{
    std::stringstream ss;
    switch (primitive_signal.type)
    {
    case PrimitiveSignal::PrimitiveSignalType::RealTimePriceComp:
        ss << "${match_price}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::SellSidePriceComp:
        ss << "${sellside_price}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::BuySidePriceComp:
        ss << "${buyside_price}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::LastPriceComp:
        ss << "${last_price}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::BidPriceComp:
        ss << "${bid_price}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::AskPriceComp:
        ss << "${ask_price}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::PeriodAccumVolumeComp:
        ss << "${accum_volume_in_" << primitive_signal.period * 100 << "ms}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::VolumeComp:
        ss << "${volume}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::BidVolumeComp:
        ss << "${bid_volume}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::AskVolumeComp:
        ss << "${ask_volume}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::AccumVolumeComp:
        ss << "${accum_volume}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::AnyTop5BidVolumeComp:
        ss << "${any_bid_volume}";
        break;
    case PrimitiveSignal::PrimitiveSignalType::AnyTop5AskVolumeComp:
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

    if (primitive_signal.type <= PrimitiveSignal::PrimitiveSignalType::AskPriceComp)
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

        for (const auto& signal_id : logic_gates[item_index].input_from)
        {   
            if (first_item)
                first_item = false;
            else
                ss << op;

            ss << SignalIDToExpression(signal_id, false);
        }

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
}

bool TTriggeringCondition::EndEditing()
{
    if (primitive_signals.size() == 1)
    {
        editing_mode = false;
        syntax_error = false;
        return true;
    }

    if (primitive_signals.empty())
    {
        editing_mode = false;
        syntax_error = true;
        lastErrMsg = "Triggering condition is blank!";
        return false;
    }

    for (size_t i = 0; i < primitive_signals.size(); ++i)
    {
        unsigned int signal_id = static_cast<unsigned int>(i) | 0x0100;
        if (primitive_signals[i].output_to.empty())
        {
            editing_mode = false;
            syntax_error = true;
            lastErrMsg = std::string{ "Primitive signal " } + std::to_string(signal_id) + " is unused.";
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

    if (!StringToDecimal(price, decimal_number.first, decimal_number.second))
        return 0;
    
    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::RealTimePriceComp, op, decimal_number.first, decimal_number.second);
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

    if (!StringToDecimal(price, decimal_number.first, decimal_number.second))
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::SellSidePriceComp, op, decimal_number.first, decimal_number.second);
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

    if (!StringToDecimal(price, decimal_number.first, decimal_number.second))
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::BuySidePriceComp, op, decimal_number.first, decimal_number.second);
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

    if (!StringToDecimal(price, decimal_number.first, decimal_number.second))
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::LastPriceComp, op, decimal_number.first, decimal_number.second);
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

    if (!StringToDecimal(price, decimal_number.first, decimal_number.second))
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::BidPriceComp, op, decimal_number.first, decimal_number.second);
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

    if (!StringToDecimal(price, decimal_number.first, decimal_number.second))
        return 0;

    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::AskPriceComp, op, decimal_number.first, decimal_number.second);
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
        
    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::VolumeComp, op, 0, volume);
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

    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::PeriodAccumVolumeComp, op, period_in_tenth_sec, volume);
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

    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::BidVolumeComp, op, 0, volume);
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

    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::AskVolumeComp, op, 0, volume);
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

    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::AccumVolumeComp, op, 0, volume);
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

    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::AnyTop5BidVolumeComp, op, 0, volume);
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

    PrimitiveSignal primitive_signal(PrimitiveSignal::PrimitiveSignalType::AnyTop5AskVolumeComp, op, 0, volume);
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

    std::vector<SignalID> signals_in;
    std::set<unsigned int> logic_gates_in;
    std::set<unsigned int> primitives_in;

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
    logic_gates.push_back(std::move(logic_gate));
    unsigned int signal_id = static_cast<unsigned int>(logic_gate_index) | 0x0200;

    for (auto& primitive_index : primitives_in)
         primitive_signals[primitive_index].output_to.push_back(signal_id);
    
    for (auto& logic_gate_index : logic_gates_in)
        logic_gates[logic_gate_index].output_to.push_back(signal_id);
    
    return signal_id;
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

    return "";
}

std::string TTriggeringCondition::ToString() 
{
    if (editing_mode)
    {
        lastErrMsg = "Instruction is not available when editing mode is on.";
        return std::string{};
    }

    if (syntax_error)
    {
        lastErrMsg = "Instruction is not available when syntax error exist.";
        return std::string{};
    }

    // the format of output string
    // triggering condition (scene)  ---> 訊號組成列表;訊號關聯性列表;
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
    for (size_t i = 0; i < primitive_signals.size(); ++i)
    {
        SignalID signal_id = static_cast<SignalID>(i) | 0x0100;
        if (i)
            ss << ",";
        ss << primitive_signals[i].ToString(signal_id);
    }

    if (primitive_signals.size() == 1)
    {
        ss << ";;";
        return ss.str();
    }

    std::queue<SignalID> gate_queue;
    for (size_t i = 0; i < logic_gates.size(); ++i)
    {
        SignalID signal_id = static_cast<SignalID>(i) | 0x0200;
        if (logic_gates[i].output_to.empty() && gate_queue.empty())
            gate_queue.push(signal_id);
        ss << "," << logic_gates[i].ToString(signal_id);
    }

    ss << ";";
    
    bool first_item{ true };
    while (!gate_queue.empty())
    {
        SignalID root_signal_id = gate_queue.front();
        gate_queue.pop();

        unsigned int root_gate_index = root_signal_id & 0xFF;
        for (const auto& signal_id : logic_gates[root_gate_index].input_from)
        {
            unsigned int category = (signal_id >> 8) & 0x03;
            if (2 == category) // logic gate
                gate_queue.push(signal_id);

            if (first_item)
                first_item = false;
            else
                ss << ",";

            ss << std::to_string(root_signal_id) << ":" << signal_id;
        }
    }

    ss << ";";

    return ss.str();
}