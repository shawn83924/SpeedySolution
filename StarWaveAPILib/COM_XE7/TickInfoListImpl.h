// ---------------------------------------------------------------------------
// TickInfoListImpl.h : Declaration of the TTickInfoListImpl
// ---------------------------------------------------------------------------
#ifndef TickInfoListImplH
#define TickInfoListImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"

#include "ChartServerConnection.h"
// ---------------------------------------------------------------------------
// TTickInfoListImpl     Implements ITickInfoList, default interface of TickInfoList
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    :
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TTickInfoListImpl : public TCppAutoObject<ITickInfoList>
{
  typedef _COM_CLASS inherited;
private:
	UFC::List<TTickInfo>* FTicks;
public:
	void Ref( UFC::List<TTickInfo>* Ticks ) { FTicks = Ticks; }
public:
  __fastcall TTickInfoListImpl();
  __fastcall TTickInfoListImpl(const System::_di_IInterface Controller);
  __fastcall TTickInfoListImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // ITickInfoList
protected:
  STDMETHOD(get_Count(long* Value));
  STDMETHOD(get_Items(long Index, ITickInfo** Value));
};




#endif //TickInfoListImplH
