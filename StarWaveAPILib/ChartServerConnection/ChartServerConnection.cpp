//---------------------------------------------------------------------------
#include "Utility.h"
#include "ChartServerConnection.h"
//------------------------------------------------------------------------------
///< Ask for prev. N days K Bars.
UFC::AnsiString CANDLE_HISTORY_REQUEST_SUBJECT( "CANDLE" );
UFC::AnsiString CANDLE_HISTORY_RESPONSE_SUBJECT( "CANDLE.RECOVER" );
///< Ask for today's K Bars.
UFC::AnsiString CANDLE_TODAY_REQUEST_SUBJECT( "TICK.PIC" );
UFC::AnsiString CANDLE_TODAY_RESPONSE_SUBJECT( "PIC.RECOVER" );
///< Ask for recover today's ticks.
UFC::AnsiString TICK_RECOVER_REQUEST( "TICK.LIST" );
UFC::AnsiString TICK_RECOVER_RESPONSE( "LIST.RECOVER" );
//---------------------------------------------------------------------------
#ifdef _WIN32
__fastcall TTickInfo::TTickInfo( int* RowData, int DecimalLocator )
#else
TTickInfo::TTickInfo( int* RowData, int DecimalLocator )
#endif
:ID( *(RowData+0) )
,Seq( *(RowData+1) )
,Time( *(RowData+2) )
,BuyIntPx( *(RowData+3) )
,SellIntPx( *(RowData+4) )
,FillIntPx( *(RowData+5) )
,FillQty( *(RowData+6) )
,BuyPx( UFC::IntToDouble( BuyIntPx, DecimalLocator ))
,SellPx( UFC::IntToDouble( SellIntPx, DecimalLocator))
,FillPx( UFC::IntToDouble( FillIntPx, DecimalLocator))
,BuyPxIndex( 0 )
,SellPxIndex( 0 )
,FillPxIndex( 0 )
,RefSec( 0 )
,TenMS( 0 )
{
}
//---------------------------------------------------------------------------
#ifdef _WIN32
__fastcall TTickInfo::TTickInfo( const TTickInfo& Ref )
#else
TTickInfo::TTickInfo( const TTickInfo& Ref )
#endif
:ID( Ref.ID )
,Seq( Ref.Seq )
,Time( Ref.Time )
,BuyIntPx( Ref.BuyIntPx )
,SellIntPx( Ref.SellIntPx )
,FillIntPx( Ref.FillIntPx )
,FillQty( Ref.FillQty )
,BuyPx( Ref.BuyPx )
,SellPx( Ref.SellPx )
,FillPx( Ref.FillPx )
,BuyPxIndex( Ref.BuyPxIndex )
,SellPxIndex( Ref.SellPxIndex )
,FillPxIndex( Ref.FillPxIndex )
,RefSec( Ref.RefSec )
,TenMS( Ref.TenMS )
{

}
//---------------------------------------------------------------------------
#ifdef _WIN32
__fastcall  TTickInfo::TTickInfo( void )
#else
TTickInfo::TTickInfo( void )
#endif
:ID( 0 )
,Seq( 0 )
,Time( 0 )
,BuyIntPx( 0 )
,SellIntPx( 0 )
,FillIntPx( 0 )
,FillQty( 0 )
,BuyPx( 0 )
,SellPx( 0 )
,FillPx( 0 )
,BuyPxIndex( 0 )
,SellPxIndex( 0 )
,FillPxIndex( 0 )
,RefSec( 0 )
,TenMS( 0 )
{

}
//------------------------------------------------------------------------------
#ifdef _WIN32
__fastcall  TTickInfo::TTickInfo( UFC::PStream* Stream )
#else
TTickInfo::TTickInfo( UFC::PStream* Stream )
#endif
{
   LoadFromStream( Stream );
}
//------------------------------------------------------------------------------
void TTickInfo::Accumulate( const TTickInfo& Ref )
{
   BuyPxIndex  += Ref.BuyPxIndex;
   SellPxIndex += Ref.SellPxIndex;
   FillPxIndex += Ref.FillPxIndex;
   FillQty     += Ref.FillQty;
}
//------------------------------------------------------------------------------
void TTickInfo::Average( int Count )
{
	BuyPxIndex  = BuyPxIndex  / Count;
	SellPxIndex = SellPxIndex / Count;
	FillPxIndex = FillPxIndex / Count;
}
//------------------------------------------------------------------------------
void TTickInfo::SaveToStream( UFC::PStream* Stream)
{
	Stream->Write( &ID, sizeof( int ));
	Stream->Write( &Seq, sizeof( int ));
	Stream->Write( &Time, sizeof( int ));
	Stream->Write( &BuyIntPx, sizeof( int ));
	Stream->Write( &SellIntPx, sizeof( int ));
	Stream->Write( &FillIntPx, sizeof( int ));
	Stream->Write( &FillQty, sizeof( int ));
}
//------------------------------------------------------------------------------
void TTickInfo::LoadFromStream( UFC::PStream* Stream)
{
	Stream->Read( &ID, sizeof( int ));
	Stream->Read( &Seq, sizeof( int ));
	Stream->Read( &Time, sizeof( int ));
	Stream->Read( &BuyIntPx, sizeof( int ));
	Stream->Read( &SellIntPx, sizeof( int ));
	Stream->Read( &FillIntPx, sizeof( int ));
	Stream->Read( &FillQty, sizeof( int ));
}
//---------------------------------------------------------------------------
#ifdef _WIN32
__fastcall TMinuteKInfo::TMinuteKInfo( int* Array, int DecimalLocator )
#else
TMinuteKInfo::TMinuteKInfo( int* Array, int DecimalLocator )
#endif
:Hour( Array[0]/1000000)
,Minutes( (Array[0]%1000000)/10000 )
,Qty( Array[1] )
{
	double divided = pow( 10.0, (double)DecimalLocator );

	MaxPx   = ((double)Array[2])/divided;
	MinPx   = ((double)Array[3])/divided;
	OpenPx  = ((double)Array[4])/divided;
	ClosePx = ((double)Array[5])/divided;
}
//---------------------------------------------------------------------------
#ifdef _WIN32
__fastcall TMinuteKInfo::TMinuteKInfo( const TMinuteKInfo& Ref )
#else
TMinuteKInfo::TMinuteKInfo( const TMinuteKInfo& Ref )
#endif
:Hour( Ref.Hour )
,Minutes( Ref.Minutes )
,Qty( Ref.Qty )
,MaxPx( Ref.MaxPx )
,MinPx( Ref.MinPx )
,OpenPx( Ref.OpenPx )
,ClosePx( Ref.ClosePx )
{

}
//---------------------------------------------------------------------------
#ifdef _WIN32
__fastcall TMinuteKInfo::TMinuteKInfo( void )
#else
TMinuteKInfo::TMinuteKInfo( void )
#endif
:Hour( 0 )
,Minutes( 0 )
,Qty( 0 )
,MaxPx( -1 )
,MinPx( -1 )
,OpenPx( -1 )
,ClosePx( 0.0 )
{
}
//---------------------------------------------------------------------------
SubscriberInfo::SubscriberInfo( const UFC::AnsiString& Ex,
								const UFC::AnsiString& Sym,
								int decimallocator,
								double BullPrice,
								double BearPrice )
:Exchange( Ex )
,Symbol( Sym )
,FRef( 1 )
,FIsReg( false )
,BullPx( BullPrice )
,BearPx( BearPrice )
,DecimalLocator( decimallocator )
{
}
//---------------------------------------------------------------------------
SubscriberInfo::SubscriberInfo()
:FRef( 1 )
,FIsReg( false )
,BullPx(0)
,BearPx(0)
,DecimalLocator(0)
{
}
//---------------------------------------------------------------------------
int SubscriberInfo::IncreaseRef( void )
{
	UFC::PLockObject Lock( FCS );

	FRef++;
	return FRef;
}
//---------------------------------------------------------------------------
int SubscriberInfo::DecreaseRef( void )
{
	UFC::PLockObject Lock( FCS );

	FRef--;
	return FRef;
}
//---------------------------------------------------------------------------
void SubscriberInfo::Regist( bool Value )
{
	UFC::PLockObject Lock( FCS );
	FIsReg = Value;
}
//---------------------------------------------------------------------------
bool SubscriberInfo::IsRegist( void )
{
	UFC::PLockObject Lock( FCS );
	return FIsReg;
}
//---------------------------------------------------------------------------
void SubscriberInfo::Init( TTickInfo& Info )
{
	MinuteKBarInfo.Hour    = Info.Time/1000000;
	MinuteKBarInfo.Minutes = (Info.Time%1000000)/10000;
	MinuteKBarInfo.Qty     = Info.FillQty;
	MinuteKBarInfo.MaxPx   = Info.FillPx;
	MinuteKBarInfo.MinPx   = Info.FillPx;
	MinuteKBarInfo.OpenPx  = Info.FillPx;
	MinuteKBarInfo.ClosePx = Info.FillPx;
}
//---------------------------------------------------------------------------
void SubscriberInfo::Update( TTickInfo& Info )
{
	if( MinuteKBarInfo.MaxPx == -1 || Info.FillPx > MinuteKBarInfo.MaxPx )
		MinuteKBarInfo.MaxPx   = Info.FillPx;
	if( MinuteKBarInfo.MinPx == -1 || Info.FillPx < MinuteKBarInfo.MinPx )
		MinuteKBarInfo.MinPx   = Info.FillPx;
	if( MinuteKBarInfo.OpenPx == -1)
		MinuteKBarInfo.OpenPx  = Info.FillPx;
	MinuteKBarInfo.Qty        += Info.FillQty;
	MinuteKBarInfo.ClosePx     = Info.FillPx;
}
//---------------------------------------------------------------------------
//
//   Class StarWaveCSConnection
//
//---------------------------------------------------------------------------
StarWaveCSConnection::StarWaveCSConnection( HINSTANCE Instance, const UFC::AnsiString& AppName, IChartServerConnectionEventListener* pListener, BOOL IsWin32GUIApp )
:FInstance( Instance )
,FAppName( AppName )
,FListener( pListener )
,FAcuRecvBytes( 0 )
,FAcuRecvMsgs( 0 )
,FOnTickMsg(&StarWaveCSConnection::TickData )
,FOnTodayKMsg(&StarWaveCSConnection::TodayCandle )
,FOnNDaysKMsg( &StarWaveCSConnection::PrevNDaysCandle )
{
	MSubscriber* NewSubscriber;
#ifndef _WIN32
	FApp = new MApp();
#else
	if( IsWin32GUIApp == TRUE )
		FApp = new MWinApp( Instance );
	else
		FApp =  new MApp();
#endif
	FApp->SetAppName( AppName );
	FApp->SetListener( this );
	FThread = new TMDThreadApp( FApp );
	///< Listen Recover subject
	FRecoverKey.Printf( "%s@%s.%d", FAppName.c_str(), UFC::Hostname, UFC::GetTickCountMS() );
	///< Listen prev. N days K Bars.
	NewSubscriber = new MSubscriber( "CandleHistorySubscriber", CANDLE_HISTORY_RESPONSE_SUBJECT, FRecoverKey );
	NewSubscriber->addListener( this );
	FApp->AddSubscriber( NewSubscriber );
	///< Listen today's K Bars.
	NewSubscriber = new MSubscriber( "CandleTodaySubscriber", CANDLE_TODAY_RESPONSE_SUBJECT, FRecoverKey );
	NewSubscriber->addListener( this );
	FApp->AddSubscriber( NewSubscriber );
	///< Listen today's ticks.
	NewSubscriber = new MSubscriber( "TicksTodaySubscriber", TICK_RECOVER_RESPONSE, FRecoverKey );
	NewSubscriber->addListener( this );
	FApp->AddSubscriber( NewSubscriber );
	///< event handler hash map
	FCallbackFuncs.Add( TICK_RECOVER_RESPONSE,          &FOnTickMsg   );
	FCallbackFuncs.Add( CANDLE_TODAY_RESPONSE_SUBJECT,  &FOnTodayKMsg );
	FCallbackFuncs.Add( CANDLE_HISTORY_RESPONSE_SUBJECT,&FOnNDaysKMsg );
}
//---------------------------------------------------------------------------
StarWaveCSConnection::~StarWaveCSConnection()
{
//	delete FThread;
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::Connect( const UFC::AnsiString& IPAddress, Int32 Port, int Timeout )
{
	Disconnect();
	FApp->SetHost( IPAddress );
	FApp->SetPort( Port );
	FApp->Connect( Timeout );
	FApp->WaitForConnected( Timeout );
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::Disconnect( void )
{
	if( FApp->IsConnected() == TRUE )
		FApp->Disconnect();
}
//---------------------------------------------------------------------------
bool StarWaveCSConnection::IsConnected( void )
{
	return FApp->IsConnected();
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::AddListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key )
{
	UFC::AnsiString SubscriberName;
	UFC::PLockObject Lock( FCS );

	SubscriberName.Printf( "%s.%s", Subject.c_str(), Key.c_str() );
	if( FSubscribeTable.IsExists( SubscriberName ) == FALSE )
	{
		MSubscriber* NewSubscriber = new MSubscriber( SubscriberName, Subject, Key );
		NewSubscriber->addListener( this );
		FApp->AddSubscriber( NewSubscriber );
		FSubscribeTable.Add( SubscriberName, NewSubscriber );
	}
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::DelListener( const UFC::AnsiString& Subject, const UFC::AnsiString& Key )
{
	UFC::AnsiString SubscriberName;
	UFC::PLockObject Lock( FCS );

	SubscriberName.Printf( "%s.%s", Subject.c_str(), Key.c_str() );
	if( FSubscribeTable.IsExists( SubscriberName ) )
	{
		MSubscriber* DelSubscriber = FSubscribeTable.GetObjectByKey( SubscriberName );
		FApp->DeleteSubscriber( DelSubscriber );
		FSubscribeTable.DeleteByKey( SubscriberName );
		delete DelSubscriber;
	}
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::RemoveAllListener( void )
{
	UFC::PLockObject Lock( FCS );

	for(register int i = 0; i < FSubscribeTable.ItemCount(); i++ )
	{
		MSubscriber* DelSubscriber = FSubscribeTable.GetItem(i);
		FApp->DeleteSubscriber( DelSubscriber );
		delete DelSubscriber;
	}
	FSubscribeTable.Clear();
}
//---------------------------------------------------------------------------
inline BOOL StarWaveCSConnection::Send( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* pTree )
{
	return FApp->send_by_key( Subject, Key, *pTree, false );
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::RequestTicks( BasicInformation* Contract, int Begin, int End )
{
	MTree ReqData;

	ReqData.append( "Exchange", Contract->GetExchange() );
	ReqData.append( "Symbol", Contract->GetSymbol() );
	ReqData.append( "BeginID", Begin );
	ReqData.append( "EndID", End );
	Send( "TICK.LIST", FRecoverKey, &ReqData );
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::RequestTicks( const UFC::AnsiString& Exchange,const UFC::AnsiString& Symbol, int Begin, int End )
{
	MTree ReqData;

	ReqData.append( "Exchange", Exchange );
	ReqData.append( "Symbol", Symbol );
	ReqData.append( "BeginID", Begin );
	ReqData.append( "EndID", End );
	Send( "TICK.LIST", FRecoverKey, &ReqData );
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::Subscribe( BasicInformation* Contract )
{
	Subscribe( Contract->GetExchange(),
		   Contract->GetSymbol(),
		   Contract->GetDecimalLocator(),
		   Contract->GetBullPrice(),
		   Contract->GetBearPrice() );
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::Subscribe( const UFC::AnsiString& Exchange,
									  const UFC::AnsiString& Symbol,
									  int DecimalLocator,
									  double BullPx,
									  double BearPx )
{
	SubscriberInfo* RegInfo;
	MTree           ReqData;

	ReqData.append( "Exchange", Exchange );
	ReqData.append( "Symbol", Symbol );
	if( (RegInfo = FSubscribeInfoTable.GetObjectByKey( Symbol )) == NULL )
	{
		UFC::PLockObject Lock( FCS );

		RegInfo = new SubscriberInfo( Exchange,Symbol,DecimalLocator,BullPx,BearPx );
		FSubscribeInfoTable.Add( Symbol , RegInfo );
	}
	else
		RegInfo->IncreaseRef();
	Send( CANDLE_HISTORY_REQUEST_SUBJECT, FRecoverKey, &ReqData ); ///< Ask for prev. N days K Bars.
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::SubscribeSymbol( SubscriberInfo* RegInfo, const UFC::AnsiString& CurrSymbol )
{
	RegInfo->Regist( true );
	AddListener( CANDLE_TODAY_RESPONSE_SUBJECT, CurrSymbol );
	AddListener( TICK_RECOVER_RESPONSE, CurrSymbol );
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::UnsubscribeSymbol( SubscriberInfo* RegInfo, const UFC::AnsiString& CurrSymbol )
{
	DelListener( CANDLE_TODAY_RESPONSE_SUBJECT, CurrSymbol );
	DelListener( TICK_RECOVER_RESPONSE, CurrSymbol );
	RegInfo->Regist( false );
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::Unsubscribe( BasicInformation* Contract )
{
	UFC::AnsiString Symbol( Contract->GetSymbol() );
	UFC::AnsiString Exchange( Contract->GetExchange() );

	Unsubscribe( Exchange, Symbol );
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::Unsubscribe( const UFC::AnsiString& Exchange,const UFC::AnsiString& Symbol )
{
	SubscriberInfo* RegInfo;

	if( (RegInfo = FSubscribeInfoTable.GetObjectByKey( Symbol )) != NULL )
	{
		if( RegInfo->DecreaseRef() <= 0 )
		{
			UnsubscribeSymbol( RegInfo, Symbol );
			UFC::PLockObject Lock( FCS );

			FSubscribeInfoTable.DeleteByKey( Symbol );
			delete RegInfo;
        }
	}
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::OnMAppConnected( void )
{
	if( FListener != NULL )
	{
		UFC::SleepMS( 330 );
		FListener->OnConnected();
	}
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::OnMAppDisconnected( void )
{
	if( FListener != NULL )
		FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::OnMAppError( PMAppError )
{
	if( FListener != NULL )
		FListener->OnDisconnected();
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data )
{
	if( FListener != NULL )
	{
		MDEventFunc* Callback;
		if( (Callback = FCallbackFuncs.GetObjectByKey( Subject )) != NULL )
			(this->**Callback)( Key, Data );
	}
	FAcuRecvBytes += Data->getSize();
	FAcuRecvMsgs++;
}
//---------------------------------------------------------------------------
void StarWaveCSConnection::TickData(  const UFC::AnsiString& Key, MTree *Data )
{
	if( Key == FRecoverKey )
		TodayTicks( Data );
	else
		LastTick( Data );
}
//---------------------------------------------------------------------------
// Candle sticks informations of previous N days.
//---------------------------------------------------------------------------
void StarWaveCSConnection::PrevNDaysCandle(  const UFC::AnsiString& Key, MTree *Tree )
{
	UFC::AnsiString CurrSymbol, CurrExchange;
	SubscriberInfo* RegInfo;
	int DayCount;

	if( Tree->get( "Day",      DayCount ) == true && DayCount > 0 &&
		Tree->get( "Exchange", CurrExchange ) == true &&
		Tree->get( "Symbol",   CurrSymbol ) == true &&
		(RegInfo = FSubscribeInfoTable.GetObjectByKey( CurrSymbol )) != NULL )
	{
		UFC::AnsiString DateName,CountName,DataName,Date,Value;
		char*           RowData;
		int*            Array;
		int             DataSize, KBarCount;
		double MaxPx = RegInfo->BullPx;
		double MinPx = RegInfo->BearPx;
		UFC::List<TMinuteKInfoList*> KBars;
		UFC::PStringList             Dates;

		if( Tree->get( "MaxPx", Value ) )
			MaxPx = UFC::IntToDouble( Value, RegInfo->DecimalLocator );
		if( Tree->get( "MinPx", Value ) )
			MinPx = UFC::IntToDouble( Value, RegInfo->DecimalLocator );
		for( int i = 0; i <= DayCount; i++ )
		{
			DateName.Printf(  "Date%d",  i+1 );
			CountName.Printf( "Count%d", i+1 );
			DataName.Printf(  "DATA%d",  i+1 );
			if( Tree->get( DateName,  Date ) == true )               ///< Gate date string.
			{
				TMinuteKInfoList* DayKBar = new TMinuteKInfoList();

				KBars.Add( DayKBar );
				Dates.Add( Date );
				if(	Tree->get( CountName, KBarCount ) == true &&  ///< Get candle count of this day.
					Tree->get( DataName,  RowData, DataSize ) == true )   ///< Get row data.
				{
					for( register int j = 0; j < KBarCount; j++ )
					{
						Array = ((int*)RowData)+6*j;
						TMinuteKInfo Candle( Array, RegInfo->DecimalLocator );
						DayKBar->Add( Candle );
					}
				}
			}
		}
		FListener->OnDayKBars( CurrExchange, CurrSymbol, Dates.ItemCount(), MaxPx, MinPx, Dates, KBars );
		for( int i = 0; i < KBars.ItemCount(); i++ )
			 delete KBars[i];
		if( Key == FRecoverKey )
		{
			MTree ReqData;
			ReqData.append( "Exchange", CurrExchange );
			ReqData.append( "Symbol", CurrSymbol );

			Send( CANDLE_TODAY_REQUEST_SUBJECT, FRecoverKey, &ReqData );   ///< Ask for today's K Bars.*/
			//Send( TICK_RECOVER_REQUEST, FRecoverKey, &ReqData );           ///< Request for ticks data
		}
	}
}
//---------------------------------------------------------------------------
// The candle sticks information of today.
//---------------------------------------------------------------------------
void StarWaveCSConnection::TodayCandle( const UFC::AnsiString& Key, MTree *Tree )
{
	UFC::AnsiString CurrSymbol;
	SubscriberInfo* RegInfo;
	char*           RowData;
	int*            Array;
	int             Count,DataSize;

	if(  Tree->get( "Symbol", CurrSymbol ) == true &&
		(RegInfo = FSubscribeInfoTable.GetObjectByKey( CurrSymbol )) != NULL )
	{
		if( Tree->get( "Count", Count ) == true )
		{
			if( Count > 0 && Tree->get( "DATA", RowData, DataSize ) == true )
			{
				if( Count > 1 ) ///< Prev. N K-Bars of today.
				{
					UFC::List<TMinuteKInfo> KBars;

					for( register int i = 0; i < Count; i++ )
					{
						Array = ((int*)RowData)+6*i;
						TMinuteKInfo Candle( Array, RegInfo->DecimalLocator );
						KBars.Add( Candle );
					}
					FListener->OnRecoverKBars( RegInfo->Exchange, RegInfo->Symbol, KBars );
				}
				else /// Count == 1  The last K bar.
				{
					TMinuteKInfo LastCandle( (int*)RowData, RegInfo->DecimalLocator );
					FListener->OnLastKBar( RegInfo->Exchange, RegInfo->Symbol, LastCandle );
				}
			}
		}
		if( RegInfo->IsRegist() == false )
			SubscribeSymbol( RegInfo, CurrSymbol );
	}
}
//---------------------------------------------------------------------------
//  Receive the fill ticks today.(for Tick List)
//---------------------------------------------------------------------------
void StarWaveCSConnection::TodayTicks( MTree *Tree)
{
	UFC::AnsiString CurrSymbol;
	SubscriberInfo* RegInfo;
	int             Count,DataSize;
	char*           RowData;

	///< Format: ID, Seq, Time, BuyPx, SellPx, LastPx, LastQty
	if( Tree->get( "Count",  Count ) == true && Count > 1 &&
		Tree->get( "DATA",   RowData, DataSize ) == true &&
		Tree->get( "Symbol", CurrSymbol ) == true &&
		(RegInfo = FSubscribeInfoTable.GetObjectByKey( CurrSymbol )) != NULL )
	{
		int*                 Array;
		UFC::List<TTickInfo> Ticks;

		for( register int i = 0;i < Count; i++ )
		{
			Array = ((int*)RowData)+7*i;
			TTickInfo Info( Array, RegInfo->DecimalLocator  );
			Ticks.Add( Info );
		}
		FListener->OnRecoverTicks( RegInfo->Exchange, RegInfo->Symbol, Ticks );
	}
}
//---------------------------------------------------------------------------
// Last tick changed.
//---------------------------------------------------------------------------
void StarWaveCSConnection::LastTick( MTree *Tree )
{
	UFC::AnsiString CurrSymbol;
	SubscriberInfo* RegInfo;
	int             Count,DataSize;
	char*           CArray;

	if( Tree->get( "Count",  Count ) == true && Count == 1  &&
		Tree->get( "DATA",   CArray, DataSize ) == true  &&
		Tree->get( "Symbol", CurrSymbol ) == true &&
		(RegInfo = FSubscribeInfoTable.GetObjectByKey( CurrSymbol )) != NULL )
	{
		TTickInfo Info( (int*)CArray, RegInfo->DecimalLocator );
		int       Hour    = Info.Time/1000000;
		int       Minutes = (Info.Time%1000000)/10000;

		FListener->OnLastTick( RegInfo->Exchange, RegInfo->Symbol, Info );///< Trigger last tick changed event.
		if( RegInfo->MinuteKBarInfo.Hour != Hour || RegInfo->MinuteKBarInfo.Minutes != Minutes ) ///< Not the same minutes.
			RegInfo->Init( Info );
		else ///< Update K-Bar info
			RegInfo->Update( Info );
		FListener->OnLastKBar( RegInfo->Exchange, RegInfo->Symbol, RegInfo->MinuteKBarInfo ); ///< Trigger last K-Bar changed event.
	}
}
//---------------------------------------------------------------------------


