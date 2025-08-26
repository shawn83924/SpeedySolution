//---------------------------------------------------------------------------

#ifndef TMDThreadAppH
#define TMDThreadAppH
//------------------------------------------------------------------------------
#include "PThread.h"
#ifndef _WIN32
	#include "MApp.h"
#else
	#include "MWinApp.h"
#endif
//------------------------------------------------------------------------------
class TMDThreadApp : public UFC::PThread
{
private:
	MApp*           FApp;
public:
    TMDThreadApp( MApp* pApp );
    virtual ~TMDThreadApp( void );
protected:
    virtual void Execute( void );
};
//------------------------------------------------------------------------------
#endif
