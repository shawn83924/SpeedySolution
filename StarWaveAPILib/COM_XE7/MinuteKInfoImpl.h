// ---------------------------------------------------------------------------
// MinuteKInfoImpl.h : Declaration of the TMinuteKInfoImpl
// ---------------------------------------------------------------------------
#ifndef MinuteKInfoImplH
#define MinuteKInfoImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"
#include "ChartServerConnection.h"

// ---------------------------------------------------------------------------
// TMinuteKInfoImpl     Implements IMinuteKInfo, default interface of MinuteKInfo
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TMinuteKInfoImpl : public TCppAutoObject<IMinuteKInfo>
{
  typedef _COM_CLASS inherited;
private:
	TMinuteKInfo FInfo;
public:
	void Copy( TMinuteKInfo& NewVal ) {  FInfo = NewVal; }
public:
  __fastcall TMinuteKInfoImpl();
  __fastcall TMinuteKInfoImpl(const System::_di_IInterface Controller);
  __fastcall TMinuteKInfoImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // IMinuteKInfo
protected:
  STDMETHOD(get_ClosePx(double* Value));
  STDMETHOD(get_Hour(long* Value));
  STDMETHOD(get_MaxPx(double* Value));
  STDMETHOD(get_MinPx(double* Value));
  STDMETHOD(get_Minutes(long* Value));
  STDMETHOD(get_OpenPx(double* Value));
  STDMETHOD(get_Qty(long* Value));
};




#endif //MinuteKInfoImplH
