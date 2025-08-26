//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "ChartsStore.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//

static inline void ValidCtrCheck(TChartsStore *)
{
	new TChartsStore(NULL);
}
//---------------------------------------------------------------------------
__fastcall TChartsStore::TChartsStore(TComponent* Owner)
:TCustomControl(Owner)
,FIP( L"10.6.76.104" )
,FPort( 34569 )
,FCSConnection( NULL )
{
	if( !ComponentState.Contains( csDesigning ) )
		FCSConnection = new StarWaveCSConnection( (HINSTANCE)Application->Handle, "TChartsStore", this, TRUE );
}
//---------------------------------------------------------------------------
__fastcall TChartsStore::~TChartsStore()
{
	if( FCSConnection != NULL )
		delete FCSConnection;
}
//---------------------------------------------------------------------------
void __fastcall TChartsStore::Loaded(void)
{
	Width  = Canvas->TextWidth( Name ) + 6;
	Height = Canvas->TextHeight( Name ) + 4;
	Constraints->MaxWidth = Width;
	Constraints->MinWidth = Width;
	Constraints->MaxHeight = Height;
	Constraints->MinHeight = Height;
}
//---------------------------------------------------------------------------
void __fastcall TChartsStore::Paint(void)
{
	TTextFormat  Formats;
	String OutText( Name );

	TRect PRect(0,0,Width,Height);
	Canvas->Rectangle(0,0,Width,Height);
	Frame3D( Canvas, PRect, clSilver, clGray, 2 );
	Formats <<tfSingleLine<<tfCenter<<tfVerticalCenter;
	Canvas->TextRect( PRect, OutText, Formats );

}
//---------------------------------------------------------------------------
void __fastcall TChartsStore::Connect( int TimeOutSec  )
{
	if( FCSConnection != NULL )
	{
		UFC::AnsiString IPStr( FIP.c_str());
		FCSConnection->Connect( IPStr, FPort, TimeOutSec );
	}
}
//---------------------------------------------------------------------------
void __fastcall TChartsStore::Disconnect( void )
{
	if( FCSConnection != NULL )
		FCSConnection->Disconnect();
}
//---------------------------------------------------------------------------
void __fastcall TChartsStore::GetContractKey( BasicInformation* Contract, UFC::AnsiString& Key )
{
	Key.Printf("%s-%s", Contract->GetExchange().c_str(),Contract->GetSymbol().c_str());
}
//---------------------------------------------------------------------------
UFC::List<IChartListener*>* __fastcall TChartsStore::GetListener( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol )
{
	UFC::AnsiString              ContractKey;
	UFC::List<IChartListener*>*  Listeners;

	ContractKey.Printf("%s-%s", Exchange.c_str(), Symbol.c_str());
	if( FChartListeners.GetObjectByKey( ContractKey, Listeners ) == true )
		return Listeners;
	return NULL;
}
//---------------------------------------------------------------------------
void __fastcall TChartsStore::Subscribe( BasicInformation* Contract, IChartListener* Listener )
{
	if( FCSConnection != NULL )
	{
		UFC::AnsiString              ContractKey;
		UFC::List<IChartListener*>*  Listeners;

		GetContractKey( Contract , ContractKey );
		if( FChartListeners.GetObjectByKey( ContractKey, Listeners ) == false ) ///< Not subscriber
		{
			Listeners = new  UFC::List<IChartListener*>();
			FChartListeners.Add( ContractKey, Listeners );
		}
		if( Listeners->IndexOf( Listener ) == -1 )///< Listener not exists
		{
			Listeners->Add( Listener );
			FCSConnection->Subscribe( Contract );
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TChartsStore::Unsubscribe( BasicInformation* Contract, IChartListener* Listenet )
{
	if( FCSConnection != NULL )
	{
		UFC::AnsiString              ContractKey;
		UFC::List<IChartListener*>*  Listeners;
		int                          DelIndex = -1;

		GetContractKey( Contract , ContractKey );
		if( FChartListeners.GetObjectByKey( ContractKey, Listeners ) == true )
		{
			if( (DelIndex = Listeners->IndexOf( Listenet )) != -1 )///< Listener exists.
			{
				Listeners->Delete( DelIndex );
				if( Listeners->ItemCount() <= 0 )  ///< No more subscriber
				{   ///< delete Listener list.
					FChartListeners.DeleteByKey( ContractKey );
					delete Listeners;
				}
				///< Unsubscribe Contract
				FCSConnection->Unsubscribe( Contract );
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TChartsStore::RequestTicks( BasicInformation* Contract, int Begin, int End )
{
	if( FCSConnection != NULL )
		FCSConnection->RequestTicks( Contract, Begin, End );
}
//---------------------------------------------------------------------------
void TChartsStore::OnConnected( void )
{
	if( FOnConnected != NULL )
		FOnConnected(this);
}
//---------------------------------------------------------------------------
void TChartsStore::OnDisconnected( void )
{
	if( FOnDisconnected != NULL )
		FOnDisconnected(this);
}
//---------------------------------------------------------------------------
void TChartsStore::OnLastTick( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TTickInfo& Tick )
{
	UFC::List<IChartListener*>*  Listeners;

	if( (Listeners = GetListener( Exchange, Symbol )) != NULL )
	{
		for( register int i = 0; i < Listeners->ItemCount(); i++ )
			Listeners->GetItem(i)->OnLastTick( Tick );
	}
}
//---------------------------------------------------------------------------
void TChartsStore::OnLastKBar( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,TMinuteKInfo& KBar )
{
	UFC::List<IChartListener*>*  Listeners;

	if( (Listeners = GetListener( Exchange, Symbol )) != NULL )
	{
		for( register int i = 0; i < Listeners->ItemCount(); i++ )
			Listeners->GetItem(i)->OnLastKBar( KBar );
	}
}
//---------------------------------------------------------------------------
void TChartsStore::OnRecoverTicks( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,UFC::List<TTickInfo>& Ticks )
{
	UFC::List<IChartListener*>*  Listeners;

	if( (Listeners = GetListener( Exchange, Symbol )) != NULL )
	{
		for( register int i = 0; i < Listeners->ItemCount(); i++ )
			Listeners->GetItem(i)->OnRecoverTicks( Ticks );
	}
}
//---------------------------------------------------------------------------
void TChartsStore::OnRecoverKBars( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol,UFC::List<TMinuteKInfo>& KBars )
{
	UFC::List<IChartListener*>*  Listeners;

	if( (Listeners = GetListener( Exchange, Symbol )) != NULL )
	{
		for( register int i = 0; i < Listeners->ItemCount(); i++ )
			Listeners->GetItem(i)->OnRecoverKBars( KBars );
	}
}
//---------------------------------------------------------------------------
void TChartsStore::OnDayKBars( const UFC::AnsiString& Exchange, const UFC::AnsiString& Symbol, int Count, double MaxPx, double MinPx, UFC::PStringList& Date, UFC::List<TMinuteKInfoList*>& KBars )
{
	UFC::List<IChartListener*>*  Listeners;

	if( (Listeners = GetListener( Exchange, Symbol )) != NULL )
	{
		for( register int i = 0; i < Listeners->ItemCount(); i++ )
			Listeners->GetItem(i)->OnDayKBars( Count, MaxPx, MinPx, Date, KBars );
	}
}
//---------------------------------------------------------------------------
unsigned long long __fastcall TChartsStore::GetAcuRecvBytes( void )
{
	if( FCSConnection != NULL )
		return FCSConnection->GetAcuRecvBytes();
	return 0;
}
//---------------------------------------------------------------------------
unsigned long long __fastcall TChartsStore::GetAcuRecvMsgs( void )
{
	if( FCSConnection != NULL )
		return FCSConnection->GetAcuRecvMsgs();
	return 0;
}
//---------------------------------------------------------------------------
bool __fastcall TChartsStore::IsConnected( void )
{
	if( FCSConnection != NULL )
		return FCSConnection->IsConnected();
	return false;
}
//---------------------------------------------------------------------------
namespace Chartsstore
{
	void __fastcall PACKAGE Register()
	{
		TComponentClass classes[1] = {__classid(TChartsStore)};
		RegisterComponents(L"Speedy", classes, 0);
	}
}
//---------------------------------------------------------------------------
