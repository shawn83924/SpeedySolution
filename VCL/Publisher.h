//---------------------------------------------------------------------------
#ifndef VCL_Component
#define VCL_Component
#endif

#ifndef PublisherH
#define PublisherH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include <MessageObject.h>
//---------------------------------------------------------------------------
class PACKAGE TPublisher : public TComponent
{
private:
	System::UnicodeString            FSubject;
	System::UnicodeString            FKey;
	UFC::AnsiString                  FAnsiSubject;
	UFC::AnsiString                  FAnsiKey;
	TMessageObject*                  FApp;
private:
	void __fastcall                  SetMessageObject( TMessageObject* MessageObject );
	void __fastcall                  SetSubject( const System::UnicodeString Subject );
	void __fastcall                  SetKey( const System::UnicodeString Key );
	const System::UnicodeString __fastcall GetSubject( void );
	const System::UnicodeString __fastcall GetKey( void );
public:
	__fastcall                        TPublisher( TComponent* Owner );
	__fastcall                        ~TPublisher( void );
	void __fastcall                   SendData( MTree* Data );
__published:
	__property TMessageObject*        MessageObject = {read = FApp, write = SetMessageObject};
	__property System::UnicodeString  Subject = {read = GetSubject, write = SetSubject };
	__property System::UnicodeString  Key     = {read = GetKey,     write = SetKey };
};
//---------------------------------------------------------------------------
#endif
 