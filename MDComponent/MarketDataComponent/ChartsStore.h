//---------------------------------------------------------------------------

#ifndef ChartsStoreH
#define ChartsStoreH
//---------------------------------------------------------------------------
#include <SysUtils.hpp>
#include <Classes.hpp>
#include "ChartServerConnection.h"
#include "TickList.h"
//---------------------------------------------------------------------------
class IChartListener
{
public:
	virtual void OnLastKBar(  TMinuteKInfo& KBar ) = 0;
	virtual void OnRecoverKBars( UFC::List<TMinuteKInfo>& KBars ) = 0;
	virtual void OnDayKBars(  int Index, double MaxPx, double MinPx, UFC::PStringList& Date, UFC::List<TMinuteKInfoList*>& KBars ) = 0;
public:
	virtual void OnLastTick( TTickInfo& Tick ) = 0;
	virtual void OnRecoverTicks( UFC::List<TTickInfo>& Ticks ) = 0;
};
//---------------------------------------------------------------------------
class PACKAGE TChartsStore : public TCustomControl, public IChartServerConnectionEventListener
{
private:
	String                                         FIP;
	int                                            FPort;
	TNotifyEvent                                   FOnConnected;
	TNotifyEvent                                   FOnDisconnected;
private:
	StarWaveCSConnection*                                      FCSConnection;
	UFC::PHashMap<UFC::AnsiString,UFC::List<IChartListener*>*> FChartListeners;
private:
	unsigned long long          __fastcall GetAcuRecvBytes( void );
	unsigned long long          __fastcall GetAcuRecvMsgs( void );
	void                        __fastcall GetContractKey( BasicInformation* Contract, UFC::AnsiString& Key );
	UFC::List<IChartListener*>* __fastcall GetListener( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol );
protected:
	virtual void __fastcall Loaded(void);
	virtual void __fastcall Paint(void);
private:
	virtual void OnConnected( void );
	virtual void OnDisconnected( void );
	virtual void OnLastTick( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TTickInfo& Tick );
	virtual void OnLastKBar( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TMinuteKInfo& KBar );
	virtual void OnRecoverTicks( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,UFC::List<TTickInfo>& Ticks );
	virtual void OnRecoverKBars( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,UFC::List<TMinuteKInfo>& KBars );
	virtual void OnDayKBars( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, int Count, double MaxPx, double MinPx, UFC::PStringList& Date, UFC::List<TMinuteKInfoList*>& KBars );
public:
	__fastcall TChartsStore(TComponent* Owner);
	__fastcall ~TChartsStore();
	void __fastcall Connect( int TimeOutSec );
	void __fastcall Disconnect( void );
	bool __fastcall IsConnected( void );
	void __fastcall Subscribe( BasicInformation* Contract, IChartListener* Listenet );
	void __fastcall Unsubscribe( BasicInformation* Contract, IChartListener* Listenet );
	void __fastcall RequestTicks( BasicInformation* Contract, int Begin, int End );
__published: ///< Properties
	__property String       IP                 = { read = FIP, write = FIP };
	__property int          Port               = { read = FPort, write = FPort, default = 34569 };
	__property unsigned long long AcuRecvBytes = { read = GetAcuRecvBytes };
	__property unsigned long long AcuRecvMsgs  = { read = GetAcuRecvMsgs  };
__published: ///< Events
	__property TNotifyEvent OnAppConnected     = { read = FOnConnected, write = FOnConnected };
	__property TNotifyEvent OnAppDisconnected  = { read = FOnDisconnected, write = FOnDisconnected };

};
//---------------------------------------------------------------------------
#endif
