// ---------------------------------------------------------------------------
// MdListenerImpl.h : Declaration of the TMdListenerImpl
// ---------------------------------------------------------------------------
#ifndef MdListenerImplH
#define MdListenerImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "MBusAPI_TLB.h"
#include "MdMessageImpl.H"
#include <MSubscriber.h>
// ---------------------------------------------------------------------------
class MsgHandler;
// ---------------------------------------------------------------------------
// TMdListenerImpl     Implements IMdListener, default interface of MdListener
// Events         : Implements IMdListenerEvents, the default source interface
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : TRUE
// Description    : MBus Message listener object
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TMdListenerImpl : public TCppAutoObjectEvent<IMdListener>
{
  typedef _COM_CLASS inherited;
  std::vector<IMdListenerEventsDisp*> FEventList;
public:
  __fastcall TMdListenerImpl();
  __fastcall TMdListenerImpl(const System::_di_IInterface Controller);
  __fastcall TMdListenerImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  void __fastcall EventSinkChanged(const System::_di_IInterface EventSink);
private:
	MSubscriber*	FSubscriber;
	MApp*           FApp;
	MString			FSubject;
	MString			FKey;
	MString			FName;
	MsgHandler*     FHandler;
private:
	void CreateSubscriber( MApp* pApp, const MString& Subject, const MString& Key  );
	void UpdateUniqueName( void );
	void Update( void );
public: ///< MessageListener
	void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* pTree);
protected:  // IMdListener
	STDMETHOD(Create(IMdTransport* Transport, BSTR Subject, BSTR Key));
	STDMETHOD(Destroy());
	STDMETHOD(get_Key(BSTR* Value));
	STDMETHOD(get_Subject(BSTR* Value));
	STDMETHOD(IsValid(VARIANT_BOOL* Value));
	STDMETHOD(set_Key(BSTR Value));
	STDMETHOD(set_Subject(BSTR Value));
};
// ---------------------------------------------------------------------------
class MsgHandler :public MessageListener
{
private:
	TMdListenerImpl* FOwner;
private: ///< MessageListener
	virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* pTree)
	{
		FOwner->OnMigoMessage( Subject, Key, pTree );
	}
public:
	MsgHandler( TMdListenerImpl* Owner )
	:FOwner( Owner )
	{

	}
};
// ---------------------------------------------------------------------------
#endif //MdListenerImplH
