// ---------------------------------------------------------------------------
// TickInfoImpl.h : Declaration of the TTickInfoImpl
// ---------------------------------------------------------------------------
#ifndef TickInfoImplH
#define TickInfoImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"
#include "ChartServerConnection.h"
// ---------------------------------------------------------------------------
// TTickInfoImpl     Implements ITickInfo, default interface of TickInfo
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TTickInfoImpl : public TCppAutoObject<ITickInfo>
{
  typedef _COM_CLASS inherited;
private:
	TTickInfo FInfo;
public:
	void Copy( TTickInfo& NewInfo ) { FInfo = NewInfo; }
public:
  __fastcall TTickInfoImpl();
  __fastcall TTickInfoImpl(const System::_di_IInterface Controller);
  __fastcall TTickInfoImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // ITickInfo
protected:
  STDMETHOD(get_BuyPx(double* Value));
  STDMETHOD(get_FillPx(double* Value));
  STDMETHOD(get_FillQty(long* Value));
  STDMETHOD(get_ID(long* Value));
  STDMETHOD(get_SellPx(double* Value));
  STDMETHOD(get_Seq(long* Value));
  STDMETHOD(get_Time(long* Value));
};




#endif //TickInfoImplH
