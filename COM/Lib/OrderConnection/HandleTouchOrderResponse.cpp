#include "TTaifexConnection.h"

void  TTaifexConnection::ReceiveTouchOrderResponse(MTree* pTree)
{
    Glog->fprintf("----- TouchOrder Command Response ------------");

    int type = nsOrderMessageDefine::TouchOrderResponseTypeEnum::tortNone;
    UFC::AnsiString Response;

    if (!pTree->get("TYPE", type) || !pTree->get("RESPONSE", Response))
        return;
    
    Glog->fprintf("  [TYPE]:%d", type);
    Glog->fprintf("  [RESPONSE]:%s", Response.c_str());

    nsOrderMessageDefine::TouchOrderResponseTypeEnum resp_type = 
        static_cast<nsOrderMessageDefine::TouchOrderResponseTypeEnum>(type);

    TExecutionReportMessage ExecutionReport;
    ExecutionReport.SetExecType(nsOrderMessageDefine::ExecTypeEnum::etNone);
    ExecutionReport.SetTouchOrderRespType(resp_type);
    if (Response.Length())
        ExecutionReport.SetTouchOrderResponse(Response.c_str());

    TrigerOnExecutionReport(&ExecutionReport, edSpeedyGenerate);
}
