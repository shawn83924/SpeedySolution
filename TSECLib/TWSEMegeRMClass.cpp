/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "TWSEMegeRMClass.h"
#include "TWSEMegaRMDataPool.h"

namespace twserm
{
//--------------------------- CMegaRiskManagerClass ----------------------------
//------------------------------------------------------------------------------
CMegaRiskManagerClass::CMegaRiskManagerClass(bool IsRecoverMode)
:CTWSERiskManagerClass(IsRecoverMode)
{
    UFC::BufferedLog::Printf(" CMegaRiskManagerClass::%s()", __func__);
}  //CMegaRiskManagerClass::CMegaRiskManagerClass()
//------------------------------------------------------------------------------
CMegaRiskManagerClass::~CMegaRiskManagerClass()
{
}  //CMegaRiskManagerClass::~CMegaRiskManagerClass()
//------------------------------------------------------------------------------
void CMegaRiskManagerClass::Check(CheckData& Obj)
{
    
}  //CMegaRiskManagerClass::Check()
//------------------------------------------------------------------------------
void CMegaRiskManagerClass::Execute(void)
{
    int loopCount = 0;
    int checkFileInterval = gMegaRMDataPoolPtr->GetCheckFileInterval();
    while (!IsTerminated())
    {
        loopCount++;
        FLogPtr->fprintf(" CMegaRiskManagerClass::%s() Loop %d", __func__, loopCount);
        
        
        FLogPtr->Flush();
        UFC::SleepMS(checkFileInterval);
    }  //while (!IsTerminated())
}  //CMegaRiskManagerClass::Execute()
}  //namespace twserm
