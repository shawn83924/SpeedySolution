// 1.2
// Unit1.h : Declaration of the TChartServerConnectionImpl

#ifndef CSConnectionH
#define CSConnectionH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "ChartServerConnection.h"
/////////////////////////////////////////////////////////////////////////////
// TChartServerConnectionImpl     Implements IChartServerConnection, default interface of ChartServerConnection
//                  and IChartServerConnectionEvents, the default source interface
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : TRUE
// Default ProgID : StarWaveAPI.ChartServerConnection
// Description    : Connect to Chart Server
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TChartServerConnectionImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TChartServerConnectionImpl, &CLSID_ChartServerConnection>,
  public IConnectionPointContainerImpl<TChartServerConnectionImpl>,
  public TEvents_ChartServerConnection<TChartServerConnectionImpl>,
  public IDispatchImpl<IChartServerConnection, &IID_IChartServerConnection, &LIBID_StarWaveAPI>,
  public IChartServerConnectionEventListener
{
private:
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
  TChartServerConnectionImpl();

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.ChartServerConnection");
  DECLARE_DESCRIPTION("Connect to Chart Server");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
    TTypedComServerRegistrarT<TChartServerConnectionImpl>
    regObj(GetObjectCLSID(), GetProgID(), GetDescription());
    return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TChartServerConnectionImpl)
  COM_INTERFACE_ENTRY(IChartServerConnection)
  COM_INTERFACE_ENTRY2(IDispatch, IChartServerConnection)
  COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(TChartServerConnectionImpl)
  CONNECTION_POINT_ENTRY(DIID_IChartServerConnectionEvents)
END_CONNECTION_POINT_MAP()

// IChartServerConnection
public:

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

#endif //Unit1H
