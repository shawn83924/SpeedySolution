/* 
 * File:   TWSERMClass.cpp
 * Author: Zhen Fan
 *
 * Created on 2021/01/07 08:40
 */

#include "../RiskManager/RMInterface.h" //RiskManager

#include "TWSERMDataPool.h"
#include "TWSEMegaRMDataPool.h"
#include "TWSEEntrustRMDataPool.h"
#include "TWSERMClass.h"
#include "TWSEMegaRMClass.h"
#include "TWSEEntrustRMClass.h"
//------------------------------------------------------------------------------
RMObj* CreateRM(BOOL RecoverMode)
{
    bool isRecoverMode = false;
    if (RecoverMode == TRUE) isRecoverMode = true;
    
    RMObj* rmObjPtr = 0;
    UFC::AnsiString riskManagerID = twserm::CTWSERiskManagerClass::GetRiskManagerID();
    twserm::CRiskManagerDataPool::SetRecoverMode(isRecoverMode);
    twserm::CRiskManagerDataPool::SetRiskManagerID(riskManagerID);
    twserm::CRiskManagerDataPool::SetConfigureFilePathName(twserm::CTWSERiskManagerClass::GetConfigureFilePathName());
    if (twserm::CMegaRiskManagerClass::IsTWSE())
    {
        twserm::CRiskManagerDataPool::SetMarket(nsOrderMessageDefine::mTSE);
        twserm::CRiskManagerDataPool::SetMarketName("TSE");
    }
    else
    {
        twserm::CRiskManagerDataPool::SetMarket(nsOrderMessageDefine::mOTC);
        twserm::CRiskManagerDataPool::SetMarketName("OTC");
    }
    
    UFC::AnsiString upperRiskManagerID = riskManagerID.UpperCase();
    if (upperRiskManagerID == "MEGA")
    {
        twserm::gMegaRMDataPoolPtr = new twserm::CMegaRiskManagerDataPool();
        twserm::gRMDataPoolPtr = twserm::gMegaRMDataPoolPtr;
        twserm::CRiskManagerDataPool::CreateLogObject();
        twserm::gRMDataPoolPtr->LoadSetting();
        twserm::gRMDataPoolPtr->PrintSetting();
//        twserm::CRiskManagerDataPool::LoadStockDataFile();
//        twserm::CRiskManagerDataPool::DumpStockData();
        twserm::CMegaRiskManagerClass::SetLogObject(twserm::CRiskManagerDataPool::GetLogObject());
        twserm::CMegaRiskManagerClass* megaRMObjPtr = new twserm::CMegaRiskManagerClass(isRecoverMode);
        megaRMObjPtr->Start();
        rmObjPtr = megaRMObjPtr;
    }
    else if (upperRiskManagerID == "ENTRUST")
    {
        twserm::gEntrustRMDataPoolPtr = new twserm::CEntrustRiskManagerDataPool();
        twserm::gRMDataPoolPtr = twserm::gEntrustRMDataPoolPtr;
        twserm::CRiskManagerDataPool::CreateLogObject();
        twserm::gRMDataPoolPtr->LoadSetting();
        twserm::gRMDataPoolPtr->PrintSetting();
        
        twserm::CEntrustRiskManagerClass::SetLogObject(twserm::CRiskManagerDataPool::GetLogObject());
        twserm::CEntrustRiskManagerClass* entrustRMObjPtr = new twserm::CEntrustRiskManagerClass(isRecoverMode);
        entrustRMObjPtr->Start();
        rmObjPtr = entrustRMObjPtr;
    }
    
    return rmObjPtr;
}  //CreateRM()
//------------------------------------------------------------------------------
void DestroyRM(RMObj* RM)
{
    if (RM != 0) delete RM;
    twserm::gRMDataPoolPtr->DeleteLogObject();
}  //DestroyRM()
