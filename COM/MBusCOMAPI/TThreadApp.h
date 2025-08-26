//---------------------------------------------------------------------------

#ifndef TThreadApp_H
#define TThreadApp_H
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <SyncObjs.hpp>
#include <MWinApp.h>
//---------------------------------------------------------------------------
class TThreadApp : public TThread
{
private:
	MApp*   FApp;
	TEvent* FStartEvent;
private:
	virtual void __fastcall Execute(void);
    
public:
	__fastcall TThreadApp( MApp* pApp );
	virtual __fastcall ~TThreadApp( void );
	void __fastcall Wait( int ms );
};
//---------------------------------------------------------------------------
#endif
 