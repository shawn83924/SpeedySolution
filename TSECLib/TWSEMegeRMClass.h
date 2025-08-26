/* 
 * File:   TWSEMegeRMClass.h
 * Author: Zhen Fan
 *
 * Created on 2021/01/11 10:10
 */

#ifndef TWSEMEGERMCLASS_H
#define TWSEMEGERMCLASS_H
#include "TWSERMClass.h"

namespace twserm
{
//------------------------------------------------------------------------------
class CMegaRiskManagerClass : public CTWSERiskManagerClass
{
public:
    CMegaRiskManagerClass(bool IsRecoverMode);
    ~CMegaRiskManagerClass();
    
    //Implement RMObj
    virtual void Check(CheckData& Obj);
    virtual void Report(TExecReport* Msg, int PossDup) {}; //	PossDup: 0-NewExecution, 1-PossibleDuplicate, 2-SpeedyGenerate
    virtual void SetResultListener(CheckResultListener* Listener) {};
    
    //Implement PThread
    virtual void Execute(void);
};  //CMegaRiskManagerClass
}  //namespace twserm
#endif /* TWSEMEGERMCLASS_H */
