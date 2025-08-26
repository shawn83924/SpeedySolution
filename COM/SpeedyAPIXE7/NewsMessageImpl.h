// ---------------------------------------------------------------------------
// NewsMessageImpl.h : Declaration of the TNewsMessageImpl
// ---------------------------------------------------------------------------
#ifndef NewsMessageImplH
#define NewsMessageImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "SpeedyAPI_TLB.h"
#include "TNewsMessage.h"

// ---------------------------------------------------------------------------
// TNewsMessageImpl     Implements INewsMessage, default interface of NewsMessage
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TNewsMessageImpl : public TCppAutoObject<INewsMessage>
{
  typedef _COM_CLASS inherited;
private:
	TNewsMessage	FMessage;
public:
  __fastcall TNewsMessageImpl();
  __fastcall TNewsMessageImpl(const System::_di_IInterface Controller);
  __fastcall TNewsMessageImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
public:
	void AttachMessage( TNewsMessage* Msg );
  // INewsMessage
protected:
  STDMETHOD(get_Headline(BSTR* Value));
  STDMETHOD(get_ID(long* Value));
  STDMETHOD(get_Instance(VARIANT* Value));
  STDMETHOD(get_Market(BSTR* Value));
  STDMETHOD(get_MsgCount(long* Value));
  STDMETHOD(get_MsgNum(long* Value));
  STDMETHOD(get_Text(BSTR* Value));
  STDMETHOD(set_Headline(BSTR Value));
  STDMETHOD(set_ID(long Value));
  STDMETHOD(set_Text(BSTR Value));
};




#endif //NewsMessageImplH
