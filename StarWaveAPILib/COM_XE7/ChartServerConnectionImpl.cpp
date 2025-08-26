// ---------------------------------------------------------------------------
// CHARTSERVERCONNECTIONIMPL : Implementation of TChartServerConnectionImpl (CoClass: ChartServerConnection, Interface: IChartServerConnection)
// ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ChartServerConnectionImpl.h"
#include "TickInfoImpl.h"
#include "MinuteKInfoImpl.h"
#include "TickInfoListImpl.h"
#include "MinuteKInfoListImpl.h"

// ---------------------------------------------------------------------------
// TChartServerConnectionImpl
// ---------------------------------------------------------------------------
__fastcall TChartServerConnectionImpl::TChartServerConnectionImpl()
:FConnection( NULL )
{
}


// ---------------------------------------------------------------------------
// TChartServerConnectionImpl
// ---------------------------------------------------------------------------
__fastcall TChartServerConnectionImpl::TChartServerConnectionImpl(const System::_di_IInterface Controller)
							  : inherited(Controller),FConnection( NULL )
{
}


// ---------------------------------------------------------------------------
// TChartServerConnectionImpl
// ---------------------------------------------------------------------------
__fastcall TChartServerConnectionImpl::TChartServerConnectionImpl(Comobj::TComObjectFactory* Factory,
                                      const System::_di_IInterface Controller)
                              : inherited(Factory, Controller),FConnection( NULL )
{
}

void __fastcall TChartServerConnectionImpl::EventSinkChanged(const System::_di_IInterface EventSink)
{
	if( EventSink != NULL )
	{
		IChartServerConnectionEventsDisp* NewEvents = new IChartServerConnectionEventsDisp();

		FEventList.push_back( NewEvents );
		NewEvents->Bind( EventSink );
	}
}

// ---------------------------------------------------------------------------
// TChartServerConnectionImpl - Class Factory
// ---------------------------------------------------------------------------
static void createFactory()
{
  new TCppAutoObjectFactory<TChartServerConnectionImpl>(Comserv::GetComServer(),
                           __classid(TChartServerConnectionImpl),
                           CLSID_ChartServerConnection,
                           Comobj::ciMultiInstance,
                           Comobj::tmApartment);
}
#pragma startup createFactory 32
//------------------------------------------------------------------------------
void TChartServerConnectionImpl::OnConnected( void )
{
	for( register size_t i=0;i< FEventList.size(); i ++ )
		FEventList[i]->OnConnected();
}
//------------------------------------------------------------------------------
void TChartServerConnectionImpl::OnDisconnected( void )
{
	for( register size_t i=0;i< FEventList.size(); i ++ )
		FEventList[i]->OnDisconnected();
}
//------------------------------------------------------------------------------
void TChartServerConnectionImpl::OnLastTick( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TTickInfo& Tick )
{
	//TCOMITickInfo  TickInfo = CoTickInfo::Create();
	//TTickInfoImpl* CppTickInfo = dynamic_cast<TTickInfoImpl*>((ITickInfo*)TickInfo);
	TTickInfoImpl* CppTickInfo = new TTickInfoImpl();
	WideString     WideEx( Exchange.c_str() );
	WideString     WideSym( Symbol.c_str() );

	CppTickInfo->Copy( Tick );
	for( register size_t i=0;i< FEventList.size(); i ++ )
		FEventList[i]->OnLastTick( WideEx.c_bstr(), WideSym.c_bstr(),interface_cast<ITickInfo>( CppTickInfo ));

//		 (ITickInfo*)TickInfo );
}
//------------------------------------------------------------------------------
void TChartServerConnectionImpl::OnLastKBar( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TMinuteKInfo& KBar )
{
	//TCOMIMinuteKInfo  MinuteKInfo = CoMinuteKInfo::Create();
	TMinuteKInfoImpl* CppMinuteKInfo = new TMinuteKInfoImpl();//dynamic_cast<TMinuteKInfoImpl*>((IMinuteKInfo*)MinuteKInfo);
	WideString        WideEx( Exchange.c_str() );
	WideString        WideSym( Symbol.c_str() );

	CppMinuteKInfo->Copy( KBar );
	for( register size_t i=0;i< FEventList.size(); i ++ )
		FEventList[i]->OnLastKBar( WideEx.c_bstr(), WideSym.c_bstr(),interface_cast<IMinuteKInfo>(CppMinuteKInfo) );
}
//------------------------------------------------------------------------------
void TChartServerConnectionImpl::OnRecoverTicks( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,UFC::List<TTickInfo>& Ticks )
{
	//TCOMITickInfoList  TickInfoList = CoTickInfoList::Create();
	TTickInfoListImpl* CppTickInfoList = new TTickInfoListImpl();//dynamic_cast<TTickInfoListImpl*>((ITickInfoList*)TickInfoList);
	WideString         WideEx( Exchange.c_str() );
	WideString         WideSym( Symbol.c_str() );

	CppTickInfoList->Ref( &Ticks );
	for( register size_t i=0;i< FEventList.size(); i ++ )
		FEventList[i]->OnRecoverTicks( WideEx.c_bstr(), WideSym.c_bstr(),interface_cast<ITickInfoList>(CppTickInfoList) );
}
//------------------------------------------------------------------------------
void TChartServerConnectionImpl::OnRecoverKBars( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,UFC::List<TMinuteKInfo>& KBars )
{
	//TCOMIMinuteKInfoList  MinuteKInfoList = CoMinuteKInfoList::Create();
	TMinuteKInfoListImpl* CppMinuteKInfoList = new TMinuteKInfoListImpl();//dynamic_cast<TMinuteKInfoListImpl*>((IMinuteKInfoList*)MinuteKInfoList);
	WideString            WideEx( Exchange.c_str() );
	WideString            WideSym( Symbol.c_str() );

	CppMinuteKInfoList->Ref( &KBars );
	for( register size_t i=0;i< FEventList.size(); i ++ )
		FEventList[i]->OnRecoverKBars( WideEx.c_bstr(), WideSym.c_bstr(),interface_cast<IMinuteKInfoList>(CppMinuteKInfoList) );
}
//------------------------------------------------------------------------------
void TChartServerConnectionImpl::OnDayKBars( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,
							 int Count, double MaxPx, double MinPx, UFC::PStringList& Date, UFC::List<TMinuteKInfoList*>& KBars )
{
	WideString            WideEx( Exchange.c_str() );
	WideString            WideSym( Symbol.c_str() );

	for( int i =0; i < Count; i++ )
	{
		WideString            WideDate( Date[i].c_str() );
//		TCOMIMinuteKInfoList  MinuteKInfoList = CoMinuteKInfoList::Create();
		TMinuteKInfoListImpl* CppMinuteKInfoList =  new TMinuteKInfoListImpl();//dynamic_cast<TMinuteKInfoListImpl*>((IMinuteKInfoList*)MinuteKInfoList);

		CppMinuteKInfoList->Ref( KBars[i] );
		for( register size_t i=0;i< FEventList.size(); i ++ )
			FEventList[i]->OnDayKBars( WideEx.c_bstr(), WideSym.c_bstr(), WideDate.c_bstr(), interface_cast<IMinuteKInfoList>(CppMinuteKInfoList) );
	}
}
//------------------------------------------------------------------------------
//
//   Implement COM methods for IChartServerConnection.
//
//------------------------------------------------------------------------------
STDMETHODIMP TChartServerConnectionImpl::Create()
{
	try
	{
		UFC::AnsiString AppName;

		AppName.Printf( "CSClient@%s.%d", UFC::Hostname, GetTickCount() );
		FConnection = new StarWaveCSConnection( HInstance, AppName, this, TRUE );
	}
	catch( Exception &e )
	{
		return Error(e.Message.c_str(), IID_IChartServerConnection);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TChartServerConnectionImpl::Destroy()
{
  try
  {
	if( FConnection != NULL )
	{
		FConnection->Disconnect();
		delete FConnection;
	}
	FConnection = NULL;
  }
  catch(Exception &e)
  {
	return Error(e.Message.c_str(), IID_IChartServerConnection);
  }
  return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TChartServerConnectionImpl::Connect(BSTR IP, long Port, long Timeout)
{
	if( FConnection != NULL )
	{
		AnsiString IPStr( IP );

		FConnection->Connect( IPStr.c_str(), Port, Timeout );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TChartServerConnectionImpl::Disconnect()
{
	try
	{
		if( FConnection != NULL )
			FConnection->Disconnect();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IChartServerConnection);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TChartServerConnectionImpl::get_IP(BSTR* Value)
{
	try
	{
		if( FConnection != NULL )
		{
			WideString IP( FConnection->GetIP().c_str() );
			*Value = IP.Copy();
		}
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IChartServerConnection);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TChartServerConnectionImpl::get_Port(long* Value)
{
	try
	{
		if( FConnection != NULL )
			*Value = FConnection->GetPort();
	}
	catch(Exception &e)
	{
		return Error(e.Message.c_str(), IID_IChartServerConnection);
	}
	return S_OK;
};
//------------------------------------------------------------------------------
STDMETHODIMP TChartServerConnectionImpl::RequestTicks(ISecurityDefinition* Contract,
          long Begin, long End)
{
	WideString WEx,WSym;

	Contract->get_Exchange( &WEx );
	Contract->get_Symbol( &WSym );
	if( FConnection != NULL )
	{
		AnsiString Exchange( WEx );
		AnsiString Symbol( WSym );
		FConnection->RequestTicks( Exchange.c_str(), Symbol.c_str(), Begin, End );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TChartServerConnectionImpl::Subscribe(ISecurityDefinition* Security)
{
	WideString WEx,WSym;
	double BullPx,BearPx;
	long DecimalLocator;

	Security->get_Exchange( &WEx );
	Security->get_Symbol( &WSym );
	Security->get_DecimalLocator( &DecimalLocator );
	Security->get_BullPx( &BullPx );
	Security->get_BearPx( &BearPx );
	if( FConnection != NULL )
	{
		AnsiString Exchange( WEx );
		AnsiString Symbol( WSym );
		FConnection->Subscribe( Exchange.c_str(),Symbol.c_str(), DecimalLocator,BullPx, BearPx );
	}
	return S_OK;
}
//------------------------------------------------------------------------------
STDMETHODIMP TChartServerConnectionImpl::Unsubscribe(ISecurityDefinition* Contract)
{
	WideString WEx,WSym;

	Contract->get_Exchange( &WEx );
	Contract->get_Symbol( &WSym );
	if( FConnection != NULL )
	{
		AnsiString Exchange( WEx );
		AnsiString Symbol( WSym );
		FConnection->Unsubscribe( Exchange.c_str(), Symbol.c_str() );
	}
	return S_OK;
}
//------------------------------------------------------------------------------

