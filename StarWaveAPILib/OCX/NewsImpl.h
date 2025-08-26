// ---------------------------------------------------------------------------
// NewsImpl.h : Declaration of the TNewsImpl
// ---------------------------------------------------------------------------
#ifndef NewsImplH
#define NewsImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"

#include "MarketDataMessage.h"
// ---------------------------------------------------------------------------
// TNewsImpl     Implements INews, default interface of News
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TNewsImpl : public TCppAutoObject<INews>
{
  typedef _COM_CLASS inherited;
private:
	int FKey;
	UFC::AnsiString FGroup;
	UFC::AnsiString FData;
public:
	void FromNews( const UFC::AnsiString& Group,const UFC::AnsiString& Msg );
public:
  __fastcall TNewsImpl();
  __fastcall TNewsImpl(const System::_di_IInterface Controller);
  __fastcall TNewsImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // INews
protected:
  STDMETHOD(get_Data(BSTR* Value));
  STDMETHOD(get_Key(long* Value));
  STDMETHOD(get_Group(BSTR* Value));
};




#endif //NewsImplH
