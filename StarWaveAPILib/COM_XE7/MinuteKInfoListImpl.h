// ---------------------------------------------------------------------------
// MinuteKInfoListImpl.h : Declaration of the TMinuteKInfoListImpl
// ---------------------------------------------------------------------------
#ifndef MinuteKInfoListImplH
#define MinuteKInfoListImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"
#include "ChartServerConnection.h"
// ---------------------------------------------------------------------------
// TMinuteKInfoListImpl     Implements IMinuteKInfoList, default interface of MinuteKInfoList
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : FALSE
// Description    : 
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TMinuteKInfoListImpl : public TCppAutoObject<IMinuteKInfoList>
{
  typedef _COM_CLASS inherited;
private:
	UFC::List<TMinuteKInfo>* FKBars;
public:
	void Ref( UFC::List<TMinuteKInfo>* KBars ) { FKBars = KBars; }
public:
  __fastcall TMinuteKInfoListImpl();
  __fastcall TMinuteKInfoListImpl(const System::_di_IInterface Controller);
  __fastcall TMinuteKInfoListImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  

  // IMinuteKInfoList
protected:
  STDMETHOD(get_Count(long* Value));
  STDMETHOD(get_Items(long Index, IMinuteKInfo** Value));
};




#endif //MinuteKInfoListImplH
