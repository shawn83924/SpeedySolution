// ---------------------------------------------------------------------------
// ChartServerConnectionImpl.h : Declaration of the TChartServerConnectionImpl
// ---------------------------------------------------------------------------
#ifndef ChartServerConnectionImplH
#define ChartServerConnectionImplH

#include <ComServ.hpp>
#include <axbase.h>
#include "StarWaveAPI_TLB.h"
#include "ChartServerConnection.h"
// ---------------------------------------------------------------------------
// TChartServerConnectionImpl     Implements IChartServerConnection, default interface of ChartServerConnection
// Events         : Implements IChartServerConnectionEvents, the default source interface
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : TRUE
// Description    :
// ---------------------------------------------------------------------------
class DAX_COM_CLASS TChartServerConnectionImpl : public TCppAutoObjectEvent<IChartServerConnection>,  public IChartServerConnectionEventListener
{
	typedef _COM_CLASS inherited;
	typedef std::vector<IChartServerConnectionEventsDisp*> TCSEventList;
private:
	TCSEventList          FEventList;
	StarWaveCSConnection* FConnection;
private: ///< Implement class IChartServerConnectionEventListener
	virtual void OnConnected( void );
	virtual void OnDisconnected( void );
	virtual void OnLastTick( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TTickInfo& Tick );
	virtual void OnLastKBar( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TMinuteKInfo& KBar );
	virtual void OnRecoverTicks( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,UFC::List<TTickInfo>& Ticks );
	virtual void OnRecoverKBars( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,UFC::List<TMinuteKInfo>& KBars );
	virtual void OnDayKBars( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,
							 int Count, double MaxPx, double MinPx, UFC::PStringList& Date, UFC::List<TMinuteKInfoList*>& KBars );
public:
  __fastcall TChartServerConnectionImpl();
  __fastcall TChartServerConnectionImpl(const System::_di_IInterface Controller);
  __fastcall TChartServerConnectionImpl(Comobj::TComObjectFactory* Factory, const System::_di_IInterface Controller);
  void __fastcall EventSinkChanged(const System::_di_IInterface EventSink);


  // IChartServerConnection
protected:
  STDMETHOD(Connect(BSTR IP, long Port, long Timeout));
  STDMETHOD(Create());
  STDMETHOD(Destroy());
  STDMETHOD(Disconnect());
  STDMETHOD(get_IP(BSTR* Value));
  STDMETHOD(get_Port(long* Value));
  STDMETHOD(RequestTicks(ISecurityDefinition* Contract, long Begin, long End));
  STDMETHOD(Subscribe(ISecurityDefinition* Security));
  STDMETHOD(Unsubscribe(ISecurityDefinition* Contract));
};




#endif //ChartServerConnectionImplH
