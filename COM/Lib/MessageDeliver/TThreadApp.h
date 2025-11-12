#ifndef _TThreadApp_H_
#define _TThreadApp_H_
//------------------------------------------------------------------------------
#include "../UFC/PThread.h"
#ifndef _WIN32
    #include "../Migo/MApp.h"
#else
    #include "../Migo/MWinApp.h"
#endif
//------------------------------------------------------------------------------
class TThreadApp : public UFC::PThread
{
private:
    MApp*           FApp;
public:
    TThreadApp( MApp* pApp );
    virtual ~TThreadApp( void );
protected:
    virtual void Execute( void );
};
//------------------------------------------------------------------------------
#endif

