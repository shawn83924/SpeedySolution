//---------------------------------------------------------------------------
#ifndef VCL_Component
#define VCL_Component
#endif
#ifndef SubscriberH
#define SubscriberH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <MSubscriber.h>
#include <MessageObject.h>
//------------------------------------------------------------------------------
typedef void __fastcall (__closure *TMessageListener)(const MString& Subject, const MString& Key, MTree* Tree);
//------------------------------------------------------------------------------
class DoEventListener : public MessageListener
{
public:
	::TMessageListener& FListener;
public:
	DoEventListener( ::TMessageListener& Listener):FListener( Listener ){}
	void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* refMEvent )
	{
		if( FListener != NULL )
			FListener( Subject, Key, refMEvent );
	}
};
//------------------------------------------------------------------------------
class PACKAGE TSubscriber : public TComponent
{
private:
	System::UnicodeString            FSubject;
	System::UnicodeString            FKey;
	UFC::AnsiString                  FAnsiSubject;
	UFC::AnsiString                  FAnsiKey;
	TMessageObject*                  FApp;
private:
	MSubscriber*                     FSubscriber;
	::TMessageListener                 FListener;
	DoEventListener                  FDoEventListener;
private:
	void __fastcall                  SetMessageObject( TMessageObject* MessageObject );
	void __fastcall                  SetSubject( System::UnicodeString Subject );
	void __fastcall                  SetKey( System::UnicodeString Key );
	void __fastcall                  Update( void );
	virtual void __fastcall          Loaded( void );
public:
	__fastcall                       TSubscriber( TComponent* Owner );
	__fastcall                       ~TSubscriber( void );
__published:
	__property TMessageObject*       MessageObject = {read = FApp, write = SetMessageObject};
	__property System::UnicodeString Subject       = {read = FSubject, write = SetSubject };
	__property System::UnicodeString Key           = {read = FKey, write = SetKey };
	__property ::TMessageListener      OnMessage     = {read = FListener, write = FListener};
};
//------------------------------------------------------------------------------
#endif
