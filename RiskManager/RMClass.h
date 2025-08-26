/* 
 * File:   RMClass.h
 * Author: yuan
 *
 * Created on 2016年1月18日, 下午 10:19
 */

#ifndef RMCLASS_H
#define	RMCLASS_H
#include "RMInterface.h"
#include "RMConnection.h"
#include "../UFC/PThread.h"
//------------------------------------------------------------------------------
class RMClass : public RMObj , UFC::PThread
{
private:    
    GCRMConnection* FRM;
public:
    RMClass( BOOL RMode );    
    virtual ~RMClass();
public:    
    virtual void Check( CheckData& Obj );
    virtual void SetResultListener( CheckResultListener* Listener );
    virtual void Execute( void ); 
private:
    CheckResultListener* FListener;

};
//------------------------------------------------------------------------------
#endif	/* RMCLASS_H */

