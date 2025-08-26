#ifndef TAIFEX_FILE_OBJECT_MANAGER_H
#define TAIFEX_FILE_OBJECT_MANAGER_H
//------------------------------------------------------------------------------
#include "../SimTAIFEX/QueryFCM.h"
#include "FileConnectionObject.h"
//------------------------------------------------------------------------------
class TAIFEXFileObjectManager : public ThreadListener, public ConnectionListener, public PThread
{
private:
    Int32		FFCMCount;
    Int32		FCMCount;
    Int32		FOpenSwitch;
    PEvent		FEvent;
    PList		FConnectionObjects;
    AnsiString  FSourceID;
    AnsiString  FDestinationID;
private:
    // Implement ThreadListener interface.
    virtual void OnTerminate(PThread* TerminateThread);
    // Implement ConnectionListener interface.
    virtual void OnConnectionNotify(ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);
private:
    void PassToInitiaorConnection(void* Data);
    void SendConfirmMessage();
    ConnectionObjectBase* GetConnection( int Link, int PVC );
public:
    TAIFEXFileObjectManager( const AnsiString& SourceID, const AnsiString& DestinationID);
    ~TAIFEXFileObjectManager( void );
    virtual void Execute();
public:
    void CreateConnectionObjects( BOOL IsOptions );
    void ResetPVC( int Link, int PVC );
    int  GetPVCState( int Link, int PVC );
    void OnOpenSwitch( int Value );
};
//------------------------------------------------------------------------------
#endif
