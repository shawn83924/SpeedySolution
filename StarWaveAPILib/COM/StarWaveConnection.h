// 1.2
// Unit1.h : Declaration of the TStarWaveConnectionImpl

#ifndef StarWaveConnectionH
#define StarWaveConnectionH

#define _ATL_APARTMENT_THREADED

#include "StarWaveAPI_TLB.h"
#include "MarketDataConnection.h"
#include "SecurityDefinition.h"
#include "Match.h"
#include "TatolVolume.h"
#include "DayHighLow.h"
#include "OpeningInfo.h"
#include "SumOfOrder.h"
#include "News.h"
#include "UnderlyingIndex.h"
#include "CloseMarketData.h"
#include "OrderBookData.h"
#include "Recover.h"
#include "SystemMsgImpl.h"
#include "ErrorMsgImpl.h"
#include "MDConnectionObject.h"

/////////////////////////////////////////////////////////////////////////////
// TStarWaveConnectionImpl     Implements IStarWaveConnection, default interface of StarWaveConnection
//                  and IStarWaveConnectionEvents, the default source interface
// ThreadingModel : tmApartment
// Dual Interface : TRUE
// Event Support  : TRUE
// Default ProgID : StarWaveAPI.StarWaveConnection
// Description    : StarWave Connection
/////////////////////////////////////////////////////////////////////////////
class ATL_NO_VTABLE TStarWaveConnectionImpl :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<TStarWaveConnectionImpl, &CLSID_StarWaveConnection>,
  public IConnectionPointContainerImpl<TStarWaveConnectionImpl>,
  public TEvents_StarWaveConnection<TStarWaveConnectionImpl>,
  public IDispatchImpl<IStarWaveConnection, &IID_IStarWaveConnection, &LIBID_StarWaveAPI>
{
private:
	MDConnection* FConnection;
	UFC::AnsiString FIP;
	int FPort;
	UFC::AnsiString FAppName;
public:
  TStarWaveConnectionImpl():FConnection( NULL ), FIP( "" ), FPort( 34567 ), FAppName( "" )
  {
  }

  // Data used when registering Object
  //
  DECLARE_THREADING_MODEL(otApartment);
  DECLARE_PROGID("StarWaveAPI.StarWaveConnection");
  DECLARE_DESCRIPTION("StarWave Connection");

  // Function invoked to (un)register object
  //
  static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
  {
	TTypedComServerRegistrarT<TStarWaveConnectionImpl>
	regObj(GetObjectCLSID(), GetProgID(), GetDescription());
	return regObj.UpdateRegistry(bRegister);
  }


DECLARE_GET_CONTROLLING_UNKNOWN()

BEGIN_COM_MAP(TStarWaveConnectionImpl)
  COM_INTERFACE_ENTRY(IStarWaveConnection)
  COM_INTERFACE_ENTRY2(IDispatch, IStarWaveConnection)
  COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_CONNECTION_POINT_MAP(TStarWaveConnectionImpl)
  CONNECTION_POINT_ENTRY(DIID_IStarWaveConnectionEvents)
END_CONNECTION_POINT_MAP()

// IStarWaveConnection
public:
  STDMETHOD(Connect(BSTR IP, long Port, long Timeout));
  STDMETHOD(Disconnect());
  STDMETHOD(DownloadContract());
  STDMETHOD(get_AppName(BSTR* Value));
  STDMETHOD(get_IP(BSTR* Value));
  STDMETHOD(get_Port(long* Value));
  STDMETHOD(GetExchange(long Index, BSTR* Exchange));
  STDMETHOD(Init());
  STDMETHOD(Destroy());
  STDMETHOD(Subscribe(BSTR Exchange, BSTR Symbol, EnumSubscribeType SubscribeType));









  STDMETHOD(Unsubscribe(BSTR Exchange, BSTR Symbol));
  STDMETHOD(UnsubscribeAll());
  STDMETHOD(SymbolCount(BSTR Exchange, long* Count));
  STDMETHOD(GetSymbol(BSTR Exchange, long Index, BSTR* Symbol));
  STDMETHOD(GetSecurityDefinition(BSTR Exchange, BSTR Symbol, ISecurityDefinition* SecurityDefinition,
          VARIANT_BOOL* IsExist));
  STDMETHOD(GetSecurityDefinitionByIndex(BSTR Exchange, long Index, ISecurityDefinition* SecurityDefinition,
          VARIANT_BOOL* IsExist));
  STDMETHOD(SubscribeSystemMessage(BSTR Exchange));
  STDMETHOD(UnsubscribeSystemMessage(BSTR Exchange));
  STDMETHOD(SendNews(BSTR Group, BSTR Message));
  STDMETHOD(SubscribeNews(BSTR Group));
  STDMETHOD(UnsubscribeNews(BSTR Group));
  //STDMETHOD(Subscribe2(BSTR Exchange));
  STDMETHOD(Recover(BSTR Exchange));
  STDMETHOD(SubscribeX(BSTR Exchange, BSTR ProductID, BSTR SettlementMonth, EnumCallPut CallPutCode,
          double StrikePx, EnumSubscribeType SubscribeType));
};
#endif //Unit1H
