//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "main.h"
#include "ContractForm.h"
#include "ChartForm.h"
#include "FMTConfig.h"
#include "ContractViewer.h"
#include "OrderStore.h"
#include "UnifyDlg.h"
#include "SearchUnit.h"
#include "Roomi.h"
#include "UnifyUtility.h"
#include "OrderConfirm.h"
#include "StopSetting.h"
#include "SetFontForm.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RoundFormEx"
#pragma link "GraphButton"
#pragma link "GraphPanel"
#pragma link "GraphMarquee"
#pragma link "CandleStickChart"
#pragma resource "*.dfm"
//------------------------------------------------------------------------------
TLineChartForm *LineChartForm;
//---------------------------------------------------------------------------
extern TOrderStore*       gOrderStore;
extern TCMarketDataStore* gMarketDataStore;
extern TUnifyUser         gUser;
//------------------------------------------------------------------------------
TColor ChartColor[2]      = {clWhite,RGB(53,53,53)};
TColor ChartTextColor[2]  =	{RGB(5,22,45),RGB(153,153,153)};
TColor ChartGridColor[2]  = {RGB(229,229,229),RGB(66,68,70)};
TColor CandleLineColor[2] = {Graphics::clNone,Graphics::clNone};
TColor BullColor[2]       = {RGB(230,0,0),RGB(230,0,0)};
TColor BearColor[2]       = {RGB(33,140,4),RGB(33,140,4)};
TColor MAColor1[2]        = {RGB(143,255,254),RGB(143,255,254)};
TColor MAColor2[2]        = {RGB(255,241,92),RGB(255,241,92)};
TColor MAColor3[2]        = {RGB(255,93,220),RGB(255,93,220)};
//---------------------------------------------------------------------------
__fastcall TGraphButtonGroup::TGraphButtonGroup()
:FSelected( NULL )
{

}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonGroup::Add( TGraphButton* Btn )
{
   FGroup.Add( Btn );
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonGroup::Select( TGraphButton* Btn )
{
	TGraphButton* CurBtn;
	for( int i = 0; i < FGroup.ItemCount(); i++ )
	{
		CurBtn = FGroup.GetItem(i);
		if(  CurBtn == Btn )
		{
			CurBtn->Selected = true;
			FSelected = CurBtn;
		}
		else
			CurBtn->Selected = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonGroup::Select( int index )
{
	TGraphButton* CurBtn;
	for( int i = 0; i < FGroup.ItemCount(); i++ )
	{
		if( i == index )
		{
			CurBtn->Selected = true;
			FSelected = CurBtn;
		}
		else
			CurBtn->Selected = false;
	}
}
//---------------------------------------------------------------------------
TGraphButton* __fastcall TGraphButtonGroup::Selected( void )
{
	 return FSelected;
}
//---------------------------------------------------------------------------
void __fastcall TGraphButtonGroup::SetEnabled( bool enable )
{
	TGraphButton* CurBtn;

	for( int i = 0; i < FGroup.ItemCount(); i++ )
	{
		CurBtn = FGroup.GetItem(i);
		CurBtn->Enabled = enable;
		CurBtn->Invalidate();
	}
}
//------------------------------------------------------------------------------
//
// Class TLineChartForm
//
//------------------------------------------------------------------------------
__fastcall TLineChartForm::TLineChartForm(TComponent* Owner)
:TForm(Owner)
,FExchange( L"TAIFEX" )
,FSymbol( L"TXFFront1" )
,FAliasSymbol( "TXFFront1" )
,FFontSize( Canvas->Font->Size )
,FWhichStore( 0 )
,FDefaultDay( false )
,FFOKMarket( false )
,FShowBSFields( false )
,FSymbolChanging( false )
,FRecoverFinished( true )
,FFirstResponse( false )
,FLastKeyDownTick( 0 )
,FLastUpdate( 0 )
,FPixelRate( 6 )
,FOnChartFormClosed( NULL )
,FLeft(0)
,FTop(0)
,FIsTXFMXF( false )
,FSkyLandMustOff( false )
{
	ToolSV->UseAnimation = false;
	ToolSV->Opened = false;
	ToolSV->UseAnimation = true;
	this->DefaultMonitor = dmDesktop;
	CandleStickChart->MaxFPS = 10;
	//FEditTools

	FEditTools.Add(HLineButton);
	FEditTools.Add(VLineButton);
	FEditTools.Add(LineButton);
	FEditTools.Add(DelButton);
	FEditTools.Add(TextButton);
	FEditTools.Add(SelectButton);
	FEditTools.Select( 0 );

	FZoomTools.Add(AutoZoomButton);
	FZoomTools.Add(ZoomXYButton);
	FZoomTools.Add(ZoomXButton);
	FZoomTools.Add(ZoomYButton);
	FZoomTools.Select( 0 );

	FKMinkeys.Add( KBar1Button );
	FKMinkeys.Add( KBar5Button );
	FKMinkeys.Add( KBar10Button );
	FKMinkeys.Add( KBar15Button );
	FKMinkeys.Add( KBar20Button );
//	FKMinkeys.Add( KBar30Button );

	FKMinComboBoxs.Add( K1ComboBox );
	FKMinComboBoxs.Add( K2ComboBox );
	FKMinComboBoxs.Add( K3ComboBox );
	FKMinComboBoxs.Add( K4ComboBox );
	FKMinComboBoxs.Add( K5ComboBox );

	FLights.Length     = 5;
	for( int i = 0; i < 5; i++ )
		 FLights[i] = 0;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::WndProc( TMessage &Msg )
{
	switch( Msg.Msg )
	{
		case WM_ERASEBKGND:
			 Msg.Result = 1;
			 return;
		case WM_MOUSEACTIVATE:
			 this->BringToFront();
			 ContractViewerForm->ActiveClient( this );
			 CandleStickChart->SetFocus();
			 Msg.Result = MA_ACTIVATE;
			 return;
	}
	inherited::WndProc( Msg );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::GetSymbolProperties( BasicInformation* Info )
{
	FSymbol         = Info->GetSymbol().c_str();
	FExchange       = Info->GetExchange().c_str();
	FAliasSymbol    = Info->GetAliasName().c_str();
	FStrikePxDigi   = Info->GetDigit();
	FDecimalLocator = Info->GetDecimalLocator();
	FRefPrice       = Info->GetRefPrice();
	FMarket         = (MarketEnum)Info->GetMarket();
	FRefIntPx       = UFC::DoubleToInt( Info->GetRefPrice(), Info->GetDecimalLocator() );
	if( FMarket == nsOrderMessageDefine::mTSE || FMarket == nsOrderMessageDefine::mOTC )
		FDisplayName.printf( L"%s(%s)",Info->GetDisplayName(), FSymbol );
	else
		FDisplayName    = Info->GetDisplayName();
	switch( Info->GetCallPut() )
	{
		case cpCall: FCallPut = nsOrderMessageDefine::evtCall;break;
		case cpPut:  FCallPut = nsOrderMessageDefine::evtPut;break;
		case ::cpNone: FCallPut = nsOrderMessageDefine::evtNone;break;
	}
	FStrikePX = Info->GetStrikePrice( );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::AOEFunction( const String& Exchange, const String& Symbol )
{
	String PROD = Symbol.SubString(1,3);

	if( Exchange == L"TAIFEX" &&( PROD == L"TXF" || PROD == L"MXF" || PROD == L"TMF" ) )
		FIsTXFMXF = true;
	else
		FIsTXFMXF = false;
	BOXLabel->Visible = FIsTXFMXF;
	ShowBoxSwitch->Visible = FIsTXFMXF;
	SkyLandLabel->Visible = FIsTXFMXF;
	SkyLandToggleSwitch->Visible = FIsTXFMXF;
	int sel = PxGridRadioGroup->ItemIndex;
	if( FIsTXFMXF == true )
	{
		PxGridRadioGroup->Items->Clear();
		PxGridRadioGroup->Items->Add(L"固定格距");
		PxGridRadioGroup->Items->Add(L"自動格距");
		PxGridRadioGroup->Items->Add(L"AOE自動格距");
		PxGridRadioGroup->ItemIndex = sel;
	}
	else
	{
		PxGridRadioGroup->Items->Clear();
		PxGridRadioGroup->Items->Add(L"固定格距");
		PxGridRadioGroup->Items->Add(L"自動格距");
		if( sel == 2 )
			PxGridRadioGroup->ItemIndex = 1;
		else
			PxGridRadioGroup->ItemIndex = sel;
    }
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::RequestGraph( String Exchange, String Symbol )
{
	BasicInformation* Info;
	BasicInformation* OrigInfo;

	if( (Info = gMarketDataStore->GetBasicInformation( Exchange, Symbol, false ) ) != NULL )
	{

		if( Info->GetBullPrice() == 0 && Info->GetBearPrice() == 0 )
		{
		   TUnifyDlgs::MessageDialog( "Speedy Unify", L"此商品的漲跌停價為0,不能顯示" );
		   return;
		}
		FSymbolChanging = true;
		CandleStickChart->Enabled  = false;

		AOEFunction( Exchange, Symbol );
		///< Unsubscribe old symbol
		if( (OrigInfo = gMarketDataStore->GetBasicInformation( FExchange, FSymbol, false ) ) != NULL )
		{
			 MainForm->ChartsStore->Unsubscribe( OrigInfo, this );
			 gOrderStore->RemoveOrderQtyListener( CandleStickChart );
			 gOrderStore->RemovePositionChangeListener( CandleStickChart );
		}
		///< Update new symbol info
		GetSymbolProperties( Info );
		Caption = FDisplayName;
		CandleStickChart->SetSymbol( Info );
		FFirstResponse  = true;
		FSymbolChanging = false;
		FSignalInit = false;
		RemoveFilledListener( FExchange, FSymbol);
		gOrderStore->AddOrderQtyListener(FMarket, FExchange, FSymbol, CandleStickChart );
		gOrderStore->AddPositionChangeListener( FMarket, FExchange, FSymbol, CandleStickChart );
		MainForm->ChartsStore->Subscribe( Info, this );
		CandleStickChart->Enabled  = true;
	}
	else
	{
		String Msg;

		Msg.printf( L"錯誤的商品代號:%s", Symbol.c_str());
		TUnifyDlgs::MessageDialog( "Speedy Unify", Msg );
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SymbolButtonClick(TObject *Sender)
{
	if( TContractInfoForm::SelectSymbol( FExchange, FSymbol, FExchange, FSymbol ) )
		RequestGraph( FExchange, FSymbol );
}
//---------------------------------------------------------------------------
//  Receive the old fill ticks.(Today's)
//---------------------------------------------------------------------------
void TLineChartForm::OnRecoverTicks( UFC::List<TTickInfo>& Ticks )
{
}
//---------------------------------------------------------------------------
//  Received the real-time fill tick.
//  1. Add to Tick List
//  2. Add to bubble chart.
//---------------------------------------------------------------------------
void TLineChartForm::OnLastTick( TTickInfo& Tick )
{
}
//---------------------------------------------------------------------------
// The KBar information of previous N days.
//---------------------------------------------------------------------------
void TLineChartForm::OnDayKBars(  int DayCount, double MaxPx, double MinPx, UFC::PStringList& Dates, UFC::List<TMinuteKInfoList*>& KBars )
{
	try
	{
		if( FSymbolChanging == false && FFirstResponse == true )
		{
			double H[8],L[8],c[8];
			int HasDataDay = 0;

			CandleStickChart->KBarDays = DayCount;
			CandleStickChart->SetTickEx( MaxPx, MinPx );
			for( int i = 0; i < DayCount; i++ )
			{
				UFC::AnsiString DateStr( Dates.GetItem( i ));
				CandleStickChart->SetDate( i+1, DateStr );
			}
			///< DayCount - 1 means the last trading day.
			for( int i = 0; i < DayCount; i++ )
			{
				TMinuteKInfoList& KBar = *KBars[ i ];
				float DayH = 0;
				float DayL = 999999;
				float DayC;

				for( int j =0; j< KBar.ItemCount(); j++ )
				{
					TMinutesTickInfo CurKBar( KBar[j] );

					if( CurKBar.MaxPx > DayH )
						DayH = CurKBar.MaxPx;
					if( CurKBar.MinPx < DayL )
						DayL = CurKBar.MinPx;
					DayC = CurKBar.ClosePx;
					CandleStickChart->SetData( i + 1/* Day */, CurKBar, false/*NeedPaint*/ );
					if( i % 50 == 0 )
						Application->ProcessMessages();
				}
				if( i > 0 )
				{
					H[ DayCount - 2 - i] = DayH;
					L[ DayCount - 2 - i] = DayL;
					c[ DayCount - 2 - i] = DayC;
				}
				if( KBar.ItemCount() > 5 )
					HasDataDay++;
			}
			///<  Sky/Land limit
			FindSkyLand( HasDataDay, H, L, c );
			CandleStickChart->MinutesPerStick = FKMinutes;
			KBar1ButtonClick( NULL );
		}
	}
	catch( Exception &exception )
	{
		 TUnifyDlgs::MessageDialog( L"Speedy Unify", exception.Message );
	}
}
//---------------------------------------------------------------------------
// The KBar information of today.
//---------------------------------------------------------------------------
void TLineChartForm::OnRecoverKBars( UFC::List<TMinuteKInfo>& KBars )
{
	try
	{
		int Index;

		if( FSymbolChanging == false && FFirstResponse == true  )
		{
			int KCount = KBars.ItemCount();
			FFirstResponse = false;
			for( register int i = 0; i < KCount; i++ )
			{
				TMinutesTickInfo KBar( KBars[i] );

				CandleStickChart->SetData( 0/* Day 0:Today*/, KBar, false/*NeedPaint*/ );
			}
		}
		CandleStickChart->MinutesPerStick = FKMinutes;
		CandleStickChart->CenterLastTick();
	}
	catch( Exception &exception )
	{
		 TUnifyDlgs::MessageDialog( L"Speedy Unify", exception.Message );
	}
}
//---------------------------------------------------------------------------
void  __fastcall TLineChartForm::FindSkyLand( int HasDataDay, double *H, double *L, double *c )
{
	if( FIsTXFMXF == true && HasDataDay > 5 )
	{
		int    Offset = g_Config.GetParamInt( "SkyLandOffset" );
		double M = L[0]+((H[0]-L[1])+(H[1]-L[2])+(H[2]-L[3]))/3.0;       ///< Excel M (D + L)
		double O = H[0]+((H[0]-H[1])+(H[1]-H[2])+(H[2]-H[3]))/3.0;       ///< Excel O (C + N)
		double P = (H[0]+L[0]+c[0])/3.0*2 -L[0];                         ///< Excel P
		double AG = H[0]- ((H[1]-L[0])+(H[2]-L[1])+ (H[3]-L[2]))/3.0;    ///< Excel AG  ( C-AF)
		double AI = L[0]- ((L[1]-L[0])+(L[2]-L[1])+ (L[3]-L[2]))/3.0;    ///< Excel AI  ( D-AH)
		double AJ = (H[0]+L[0]+c[0])/3.0*2 -H[0];                        ///< Excel AJ
		double SkyLimit  = (H[0] + M+O+P)/4.0 * 1.001  - Offset;
		double LandLimit = (L[0] + AG+AI+AJ)/4.0 *0.999  - Offset;

		CandleStickChart->SkyPx = SkyLimit;
		CandleStickChart->LandPx = LandLimit;
		SkyLandLabel->Visible = true;
		SkyLandToggleSwitch->Visible = true;
		FSkyLandMustOff = false;
	}
	else
	{
		CandleStickChart->SkyPx = 0.0;
		CandleStickChart->LandPx = 0.0;
		SkyLandToggleSwitch->State = tssOff;
		FSkyLandMustOff = true;
		CandleStickChart->ExableSkyLand = false;
		SkyLandLabel->Visible = false;
		SkyLandToggleSwitch->Visible = false;
	}
}
//---------------------------------------------------------------------------
// The Candle stick information of the last minute.
//---------------------------------------------------------------------------
void TLineChartForm::OnLastKBar(  TMinuteKInfo& KBar )
{
	TMinutesTickInfo LastKBar( KBar );

	if( CandleStickChart->SetData( 0, LastKBar, true) == true )
	{
		if( AutoCenterSwitch->State == tssOn )
			CandleStickChart->CenterLastTick();
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::LoadSetting( void )
{
	CandleStickChart->GDIPlus   = true;
	CandleStickChart->GradientCandle  =  g_Config.GetBoolProperty("Setting","Chart3DKLine", false );
	LoadColor();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SaveProperties( const String& Profile )
{
	String ProfileDir(Profile);

	g_Config.SetDesktopInteger( ProfileDir, "ScalType",(int)CandleStickChart->Scale );
	g_Config.SetDesktopInteger( ProfileDir, "EditType",(int)CandleStickChart->Edit );
	g_Config.SetDesktopBool( ProfileDir, "EditMode",  CandleStickChart->EditMode );

	g_Config.SetDesktopInteger( ProfileDir, "KMinutes", CandleStickChart->MinutesPerStick );
	g_Config.SetDesktopInteger( ProfileDir, "ShowQty",	CandleStickChart->ShowQtyBar );
	g_Config.SetDesktopBool( ProfileDir, "ShowMA1",	MA1Button->Selected );
	g_Config.SetDesktopBool( ProfileDir, "ShowMA2",	MA2Button->Selected );
	g_Config.SetDesktopBool( ProfileDir, "ShowMA3",	MA3Button->Selected );
	g_Config.SetDesktopBool( ProfileDir, "DayHLSection", CandleStickChart->DayHLSection );

	g_Config.SetDesktopInteger(ProfileDir,"MA1NumOfK", CandleStickChart->MACount1 );
	g_Config.SetDesktopInteger(ProfileDir,"MA2NumOfK", CandleStickChart->MACount2 );
	g_Config.SetDesktopInteger(ProfileDir,"MA3NumOfK", CandleStickChart->MACount3 );
	g_Config.SetDesktopInteger(ProfileDir,"MA1Px", Px1ComboBox->ItemIndex );
	g_Config.SetDesktopInteger(ProfileDir,"MA2Px", Px2ComboBox->ItemIndex );
	g_Config.SetDesktopInteger(ProfileDir,"MA3Px", Px3ComboBox->ItemIndex );
	g_Config.SetDesktopInteger(ProfileDir,"HLSectionType", SectionTypeComboBox->ItemIndex  );
	g_Config.SetDesktopInteger(ProfileDir,"PixelsTick", CandleStickChart->PixelsPerTick );
	if( Profile != L"Default" )
	{
		g_Config.SetDesktopInteger(ProfileDir,"XScale", CandleStickChart->XScale*10000 );
		g_Config.SetDesktopInteger(ProfileDir,"YScale", CandleStickChart->YScale*10000 );
	}
	g_Config.SetDesktopInteger(ProfileDir,"AutoCenter", (AutoCenterSwitch->State == tssOn)? true:false );
	g_Config.SetDesktopInteger(ProfileDir,"SpaceNK",   SpaceComboBox->ItemIndex );
	g_Config.SetDesktopInteger(ProfileDir,"ShowHL",     (DayHLSwitch->State == tssOn)? true:false );
	g_Config.SetDesktopInteger(ProfileDir,"ShowAvgPx",  (AvgPxSwitch->State == tssOn)? true:false );
	g_Config.SetDesktopInteger(ProfileDir,"EnableTrade", (TradingSwitch->State == tssOn)? true:false );
	g_Config.SetDesktopInteger(ProfileDir,"ShowBox",    (ShowBoxSwitch->State == tssOn)? true:false );
	g_Config.SetDesktopInteger(ProfileDir,"ShowSkyLand",(SkyLandToggleSwitch->State == tssOn)? true:false );
	g_Config.SetDesktopInteger(ProfileDir,"ShowPivot",  (PivotSwitch->State == tssOn)? true:false );
	g_Config.SetDesktopInteger(ProfileDir,"PivotDeviation", DeviationTrackBar->Position);
	g_Config.SetDesktopInteger(ProfileDir,"PxGridType",  PxGridRadioGroup->ItemIndex );
	g_Config.SetDesktopInteger(ProfileDir,"TimeGridType",  TimeGridComboBox->ItemIndex );
	g_Config.SetDesktopInteger(ProfileDir,"PxGridTicks", CandleStickChart->TicksPerGrid );
	g_Config.SetIntegerProperty(ProfileDir,"DrawLineColor", LineColorBox->Selected );
	g_Config.SetIntegerProperty(ProfileDir ,"Page", PageControl->ActivePageIndex );


	SaveKBarSetting();
	SaveColor();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SaveKBarSetting( void )
{
	String Name;

	for( int i =0; i < FKMinkeys.ItemCount(); i++ )
	{
		TGraphButton* KBarButton = FKMinkeys[i];
		Name.printf( L"KMin%d", i +1 );
		g_Config.SetDesktopInteger( "KBar", Name, KBarButton->Tag );
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::LoadKBarSetting( void )
{
	String Name,MinStr, HintStr;
	int Min;

	for( int i =0; i < FKMinkeys.ItemCount(); i++ )
	{
		TGraphButton* KBarButton = FKMinkeys[i];
		int DefMin = KBarButton->Tag;
		int ComboIndex;

		Name.printf( L"KMin%d", i +1 );
		Min = g_Config.GetDesktopInteger( "KBar", Name, DefMin );
		MinStr.printf( L"%d",Min );
		ComboIndex = FKMinComboBoxs[i]->Items->IndexOf( MinStr );
		if( ComboIndex == -1 ) ///< Not found...uses first item
		{
			FKMinComboBoxs[i]->ItemIndex = 0;
			KBarButton->Tag = FKMinComboBoxs[i]->Text.ToInt();
			KBarButton->ButtonText = FKMinComboBoxs[i]->Text;
		}
		else
		{
			FKMinComboBoxs[i]->ItemIndex = ComboIndex;
			KBarButton->Tag = Min;
			KBarButton->ButtonText = MinStr;
		}
		HintStr.printf( L"%d分鐘K", KBarButton->Tag );
		KBarButton->Hint = HintStr;
		if( KBarButton->Selected == true )
			CandleStickChart->MinutesPerStick = KBarButton->Tag;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::LoadProperties( const String& Profile )
{
	String ProfileDir(Profile);

	LoadKBarSetting();
	switch(	(ScaleType)g_Config.GetDesktopInteger( ProfileDir, "ScalType", 0 ))
	{
		case stAuto:  ZoomXYButtonClick( AutoZoomButton );break;
		case stXOnly: ZoomXYButtonClick( ZoomXButton );break;
		case stYOnly: ZoomXYButtonClick( ZoomYButton );break;
		case stXYBoth:ZoomXYButtonClick( ZoomXYButton );break;
	}
	switch(	(EditType)g_Config.GetDesktopInteger( ProfileDir, "EditType", 1 ))
	{
		case etHLine:  EditButtonClick( HLineButton );break;
		case etVLine:  EditButtonClick( VLineButton );break;
		case et2PLine: EditButtonClick( LineButton );break;
		case etText:   EditButtonClick( TextButton );break;
		case etDefault:EditButtonClick( SelectButton );break;
		case etDelete: EditButtonClick( DelButton );break;
	}
	CandleStickChart->EditMode = g_Config.GetDesktopBool( ProfileDir, "EditMode", false );
	SetEditMode( CandleStickChart->EditMode );
	FKMinutes = g_Config.GetDesktopInteger( ProfileDir, "KMinutes", 1 );
	CandleStickChart->MinutesPerStick = FKMinutes;
	CandleStickChart->ShowQtyBar = g_Config.GetDesktopInteger( ProfileDir, "ShowQty", 30 );
	CandleStickChart->ShowMA1 = g_Config.GetDesktopBool( ProfileDir, "ShowMA1",true );
	CandleStickChart->ShowMA2 = g_Config.GetDesktopBool( ProfileDir, "ShowMA2",true );
	CandleStickChart->ShowMA3 = g_Config.GetDesktopBool( ProfileDir, "ShowMA3",true );
	CandleStickChart->DayHLSection = g_Config.GetDesktopBool( ProfileDir, "DayHLSection", true );
	CandleStickChart->MACount1 = g_Config.GetDesktopInteger(ProfileDir,"MA1NumOfK", 5 );
	CandleStickChart->MACount2 = g_Config.GetDesktopInteger(ProfileDir,"MA2NumOfK", 10 );
	CandleStickChart->MACount3 = g_Config.GetDesktopInteger(ProfileDir,"MA3NumOfK", 20 );
	CandleStickChart->MAPrice1 = (MAPrice)g_Config.GetDesktopInteger(ProfileDir,"MA1Px", 1 );
	CandleStickChart->MAPrice2 = (MAPrice)g_Config.GetDesktopInteger(ProfileDir,"MA2Px", 1 );
	CandleStickChart->MAPrice3 = (MAPrice)g_Config.GetDesktopInteger(ProfileDir,"MA3Px", 1 );
	CandleStickChart->HLSection = (SectionType)g_Config.GetDesktopInteger(ProfileDir,"HLSectionType", 0 );
	CandleStickChart->PixelsPerTick = g_Config.GetDesktopInteger(ProfileDir,"PixelsTick", 10 );
	SpaceComboBox->ItemIndex = g_Config.GetDesktopInteger(ProfileDir,"SpaceNK", 1 );
	AutoCenterSwitch->State = (g_Config.GetDesktopInteger(ProfileDir,"AutoCenter", false )== true)?tssOn:tssOff;
	DayHLSwitch->State      = (g_Config.GetDesktopInteger(ProfileDir,"ShowHL",true )== true)?tssOn:tssOff;
	AvgPxSwitch->State      = (g_Config.GetDesktopInteger(ProfileDir,"ShowAvgPx",true )== true)?tssOn:tssOff;
	TradingSwitch->State    = (g_Config.GetDesktopInteger(ProfileDir,"EnableTrade",true )== true)?tssOn:tssOff;
	ShowBoxSwitch->State    = (g_Config.GetDesktopInteger(ProfileDir,"ShowBox", false )== true)?tssOn:tssOff;
	SkyLandToggleSwitch->State = (g_Config.GetDesktopInteger(ProfileDir,"ShowSkyLand", false )== true)?tssOn:tssOff;
	PivotSwitch->State         = (g_Config.GetDesktopInteger(ProfileDir,"ShowPivot", false )== true)?tssOn:tssOff;
	DeviationTrackBar->Position  = g_Config.GetDesktopInteger(ProfileDir,"PivotDeviation", 2 );
	PxGridRadioGroup->ItemIndex  = g_Config.GetDesktopInteger(ProfileDir,"PxGridType", 0 );
	TimeGridComboBox->ItemIndex  = g_Config.GetDesktopInteger(ProfileDir,"TimeGridType", 3 );
	PxGridUpDown->Position       = g_Config.GetDesktopInteger(ProfileDir,"PxGridTicks", 10 );
	LineColorBox->Selected       = (TColor)g_Config.GetIntegerProperty(ProfileDir,"DrawLineColor", clCream );
	CandleStickChart->LineColor = LineColorBox->Selected;
	CandleStickChart->TicksPerGrid = PxGridUpDown->Position;
	CandleStickChart->PxGrid = (PxGridType) PxGridRadioGroup->ItemIndex;

	OrdConfirmCheckBox->Checked = g_Config.GetBoolProperty( "Setting","ConfirmBeforeNew", true );
	MA1KUpDown->Position = CandleStickChart->MACount1;
	MA2KUpDown->Position = CandleStickChart->MACount2;
	MA3KUpDown->Position = CandleStickChart->MACount3;
	MA1Button->Selected = CandleStickChart->ShowMA1;
	MA2Button->Selected = CandleStickChart->ShowMA2;
	MA3Button->Selected = CandleStickChart->ShowMA3;
	HLSectionBox->Checked = CandleStickChart->DayHLSection;
	Px1ComboBox->ItemIndex = (int)CandleStickChart->MAPrice1;
	Px2ComboBox->ItemIndex = (int)CandleStickChart->MAPrice2;
	Px3ComboBox->ItemIndex = (int)CandleStickChart->MAPrice3;
	SectionTypeComboBox->ItemIndex = (int)CandleStickChart->HLSection;

	PageControl->ActivePageIndex   = g_Config.GetIntegerProperty( ProfileDir ,"Page", 0 );

	if( DayHLSwitch->State == tssOn )
		CandleStickChart->ShowDayHL = true;
	else
		CandleStickChart->ShowDayHL = false;
	if( AvgPxSwitch->State == tssOn )
		CandleStickChart->ShowAvgPx = true;
	else
		CandleStickChart->ShowAvgPx = false;
	if( TradingSwitch->State == tssOn )
		CandleStickChart->EnableTrade = true;
	else
		CandleStickChart->EnableTrade = false;
	if(	ShowBoxSwitch->State == tssOn )
		CandleStickChart->ShowBox = true;
	else
		CandleStickChart->ShowBox = false;
	if(	PivotSwitch->State == tssOn )
	{
		DeviationTrackBarChange( NULL );
		CandleStickChart->ExablePivot = true;
    }
	else
		CandleStickChart->ExablePivot = false;
	if(	FSkyLandMustOff == false )
	{
		if(	SkyLandToggleSwitch->State == tssOn )
			CandleStickChart->ExableSkyLand = true;
		else
			CandleStickChart->ExableSkyLand = false;
	}
	else
		CandleStickChart->ExableSkyLand = false;
	if( Profile != L"Default" )
	{
		CandleStickChart->XScale   = (double)g_Config.GetDesktopInteger(ProfileDir,"XScale", 9000 )/10000;
		CandleStickChart->YScale   = (double)g_Config.GetDesktopInteger(ProfileDir,"YScale", 9000 )/10000;
	}
	LoadStopSetting();
	UpdatePixels5Tick();
	PxGridRadioGroupClick( NULL );
	TimeGridComboBoxChange( NULL );
	KBar1ButtonClick( NULL );
	SpaceComboBoxChange( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::UpdatePixels5Tick( void )
{
	String PixelsStr( CandleStickChart->PixelsPerTick*5 );
	int    SelIndex = PixelsComboBox->Items->IndexOf( PixelsStr );
	if( SelIndex != -1 )
		PixelsComboBox->ItemIndex = SelIndex;
	else
		PixelsComboBox->ItemIndex = 3;
	PixelsComboBoxChange( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::Release( void )
{
	BasicInformation* Info;
	AnsiString Ex( FExchange.c_str() );
	AnsiString Sym( FSymbol.c_str() );

	RemoveFilledListener( FExchange, FSymbol);
	if( (Info = gMarketDataStore->GetBasicInformation( Ex, Sym, false ) ) != NULL )
	{
		MainForm->ChartsStore->Unsubscribe( Info, this );
		gOrderStore->RemoveOrderQtyListener( CandleStickChart );
		gOrderStore->RemovePositionChangeListener( CandleStickChart );
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	BasicInformation* Info;

	if( FOnChartFormClosed != NULL )
		FOnChartFormClosed( this );
	RemoveFilledListener( FExchange, FSymbol );
	if( (Info = gMarketDataStore->GetBasicInformation( FExchange, FSymbol, false ) ) != NULL )
	{
		MainForm->ChartsStore->Unsubscribe( Info, this );
	    gOrderStore->RemoveOrderQtyListener( CandleStickChart );
		gOrderStore->RemovePositionChangeListener( CandleStickChart );
	}
	Action = caFree;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::KBar1ButtonClick(TObject *Sender)
{
	bool IsSelected = false;

	if( Sender != NULL )
	{
		FKMinutes = ((TGraphButton*)Sender)->Tag;
		CandleStickChart->MinutesPerStick = FKMinutes;
	}
	///< Select which button?
	for( int i =0; i < FKMinkeys.ItemCount(); i++ )
	{
		TGraphButton* KBar1Button = FKMinkeys[i];
		if( KBar1Button->Tag == CandleStickChart->MinutesPerStick )
		{
			KBar1Button->Selected = true;
			IsSelected = true;
		}
		else
			KBar1Button->Selected = false;
	}
	///< No! Select the first.
	if( IsSelected == false )
	{
		TGraphButton* KBar1Button = FKMinkeys[ 0 ];
		CandleStickChart->MinutesPerStick = KBar1Button->Tag;
		KBar1Button->Selected = true;
	}
	if( CandleStickChart->MinutesPerStick <= 3 )
	{
//		ExchangeClock* Clock = gMarketDataStore->GetClock( L"TAIFEX" );
//		if( Clock != NULL )
//			CandleStickChartKBar( NULL, Clock->Hour , Clock->Minutes );
	}
	SettingPanel->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::ShowQtyButtonClick(TObject *Sender)
{
	if( CandleStickChart->ShowQtyBar == 0)
	{
		CandleStickChart->ShowQtyBar = 30;
		ShowQtyButton->Hint = L"顯示量(30\%視窗高度)";
	}
	else if( CandleStickChart->ShowQtyBar == 30)
	{
		CandleStickChart->ShowQtyBar = 20;
		ShowQtyButton->Hint = L"顯示量(20\%視窗高度)";
	}
	else if( CandleStickChart->ShowQtyBar == 20)
	{
		CandleStickChart->ShowQtyBar = 10;
		ShowQtyButton->Hint = L"顯示量(10\%視窗高度)";
	}
	else
	{
		CandleStickChart->ShowQtyBar = 0;
		ShowQtyButton->Hint = L"隱藏成交量";
	}
	CandleStickChart->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::MA1ButtonClick(TObject *Sender)
{
	if( CandleStickChart->ShowMA1 == true )
	{
		CandleStickChart->ShowMA1 = false;
		MA1Button->Selected = false;
	}
	else
	{
		CandleStickChart->ShowMA1 = true;
		MA1Button->Selected = true;
	}
	CandleStickChart->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::MA2ButtonClick(TObject *Sender)
{
	if( CandleStickChart->ShowMA2 == true )
	{
		CandleStickChart->ShowMA2 = false;
		MA2Button->Selected = false;
	}
	else
	{
		CandleStickChart->ShowMA2 = true;
		MA2Button->Selected = true;
	}
	CandleStickChart->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::MA3ButtonClick(TObject *Sender)
{
	if(  CandleStickChart->ShowMA3 == true )
	{
		CandleStickChart->ShowMA3 = false;
		MA3Button->Selected = false;
	}
	else
	{
		CandleStickChart->ShowMA3 = true;
		MA3Button->Selected = true;
	}
	CandleStickChart->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::ZoomXYButtonClick(TObject *Sender)
{
	if( Sender == AutoZoomButton )
		CandleStickChart->Scale = stAuto;
	else if( Sender == ZoomXYButton )
		CandleStickChart->Scale = stXYBoth;
	else if( Sender == ZoomXButton )
		CandleStickChart->Scale = stXOnly;
	else if( Sender == ZoomYButton )
		CandleStickChart->Scale = stYOnly;
	FZoomTools.Select( (TGraphButton*)Sender );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::RoundFormExIconClick(TObject *Sender)
{
	TPoint ScrLT = ClientToScreen( TPoint( 0, 0 ));

	SearchSymbolForm->Top = ScrLT.Y - RoundFormEx->CaptionHeight;
	SearchSymbolForm->Left = ScrLT.X - RoundFormEx->LeftFrameWidth;
	SearchSymbolForm->Width = Width;
	ContractViewerForm->OnSelectSymbol = SelectSymbol;
	if( RoundFormEx->Lock == true )
	{
		Application->NormalizeTopMosts();
		SetWindowPos( SearchSymbolForm->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
	}
	else
		SetWindowPos( SearchSymbolForm->Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
	SearchSymbolForm->Show();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SelectSymbol( System::TObject* Sender, TSelectActionType Type, const String& Ex, const String& Sym )
{
	switch( Type )
	{
		case satSymbol: if( FExchange != Ex || FSymbol != Sym )
							RequestGraph( Ex, Sym );
						break;
		case satDepth:  ContractViewerForm->OpenOrderBookForm( Ex, Sym, true );
						break;
		case satKChart: ContractViewerForm->OpenKBarForm( Ex, Sym );
						break;
	}
	ContractViewerForm->OnSelectSymbol = NULL;
	ContractViewerForm->Hide();
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::imgMenuClick(TObject *Sender)
{
	if( ToolSV->Opened )
		ToolSV->Close();
	else
	{
		if( RoundFormEx->Lock == true )
			EnableColorSetting( false );
		else
			EnableColorSetting( true );
		ToolSV->Open();
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::MA1KUpDownChangingEx(TObject *Sender, bool &AllowChange,
		  int NewValue, TUpDownDirection Direction)
{
	CandleStickChart->MACount1 = NewValue;
	AllowChange = true;
	RepaintTimer->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::MA2KUpDownChangingEx(TObject *Sender, bool &AllowChange,
		  int NewValue, TUpDownDirection Direction)
{
	CandleStickChart->MACount2 = NewValue;
	AllowChange = true;
	RepaintTimer->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::MA3KUpDownChangingEx(TObject *Sender, bool &AllowChange,
		  int NewValue, TUpDownDirection Direction)
{
	CandleStickChart->MACount3 = NewValue;
	AllowChange = true;
	RepaintTimer->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::RepaintTimerTimer(TObject *Sender)
{
	RepaintTimer->Enabled = false;
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::KBarPaintBoxPaint(TObject *Sender)
{
	Graphics::TBitmap* Buffer = new Graphics::TBitmap();
	Buffer->Width  = KBarPaintBox->Width;
	Buffer->Height = KBarPaintBox->Height;
	Buffer->Canvas->Pen->Color = ToolSV->Color;
	Buffer->Canvas->Brush->Color = ToolSV->Color;
	Buffer->Canvas->Rectangle(0,0,Buffer->Width,Buffer->Height );
	Buffer->Canvas->Pen->Color = CandleStickChart->GridColor;
	Buffer->Canvas->MoveTo( 2, 0 );
	Buffer->Canvas->LineTo( 2, 7*FPixelRate );
	Buffer->Canvas->Brush->Color = CandleStickChart->BullColor;
	Buffer->Canvas->Rectangle(0,FPixelRate,5,6*FPixelRate );
	KBarPaintBox->Canvas->Draw(0,0, Buffer);
	delete Buffer;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::PixelsComboBoxChange(TObject *Sender)
{
	FPixelRate = PixelsComboBox->Text.ToInt() / 5;
	KBarPaintBox->Repaint();
	if( Sender != NULL )
		CandleStickChart->PixelsPerTick = FPixelRate;
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::MA1KEditMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	TEdit* Edit = dynamic_cast<TEdit*>( Sender );
	if( Edit != NULL )
		Edit->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::MA1KEditChange(TObject *Sender)
{
	try
	{
		int NMA = MA1KEdit->Text.ToInt();

		MA1KUpDown->Position = NMA;
		CandleStickChart->MACount1 = MA1KUpDown->Position;
		RepaintTimer->Enabled = true;
	}
	catch(...)
	{
		MA1KEdit->Text = String( MA1KUpDown->Position );
	}
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::MA2KEditChange(TObject *Sender)
{
	try
	{
		int NMA = MA2KEdit->Text.ToInt();

		MA2KUpDown->Position = NMA;
		CandleStickChart->MACount2 = MA2KUpDown->Position;
		RepaintTimer->Enabled = true;
	}
	catch(...)
	{
		MA2KEdit->Text = String( MA2KUpDown->Position );
	}
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::MA3KEditChange(TObject *Sender)
{
	try
	{
		int NMA = MA3KEdit->Text.ToInt();

		MA3KUpDown->Position = NMA;
		CandleStickChart->MACount3 = MA3KUpDown->Position;
		RepaintTimer->Enabled = true;
	}
	catch(...)
	{
		MA3KEdit->Text = String( MA3KUpDown->Position );
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::Px1ComboBoxChange(TObject *Sender)
{
	CandleStickChart->MAPrice1 = (MAPrice)Px1ComboBox->ItemIndex;
	RepaintTimer->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::Px2ComboBoxChange(TObject *Sender)
{
	CandleStickChart->MAPrice2 = (MAPrice)Px2ComboBox->ItemIndex;
	RepaintTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::Px3ComboBoxChange(TObject *Sender)
{
	CandleStickChart->MAPrice3 = (MAPrice)Px3ComboBox->ItemIndex;
	RepaintTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::DefColorButtonClick(TObject *Sender)
{
	int DorL = !(bool)g_Config.GetBoolProperty( "Setting","Skin", true );

	CandleStickChart->Color     = ChartColor[DorL];
	CandleStickChart->TextColor	= ChartTextColor[DorL];
	CandleStickChart->GridColor	= ChartGridColor[DorL];
	CandleStickChart->CandleLineColor = CandleLineColor[DorL];
	CandleStickChart->BullColor = BullColor[DorL];
	CandleStickChart->BearColor = BearColor[DorL];
	CandleStickChart->MAColor1  = MAColor1[DorL];
	CandleStickChart->MAColor2  = MAColor2[DorL];
	CandleStickChart->MAColor3  = MAColor3[DorL];
	SaveColor( );
	ContractViewerForm->LoadChartColor( NULL );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::ChartBKColorBoxChange(TObject *Sender)
{
	CandleStickChart->Color = ChartBKColorBox->Selected;
	g_Config.SetIntegerProperty("Setting","ChartBKColor",   CandleStickChart->Color);
	ContractViewerForm->LoadChartColor( this );
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::ChartFGColorBoxChange(TObject *Sender)
{
	CandleStickChart->TextColor = ChartFGColorBox->Selected;
	g_Config.SetIntegerProperty("Setting","ChartFontColor", CandleStickChart->TextColor );
	ContractViewerForm->LoadChartColor( this );
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::GridColorBoxChange(TObject *Sender)
{
	CandleStickChart->GridColor = GridColorBox->Selected;
	g_Config.SetIntegerProperty("Setting","ChartGridColor", CandleStickChart->GridColor );
	ContractViewerForm->LoadChartColor( this );
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::CandleLineColorBoxChange(TObject *Sender)
{
	CandleStickChart->CandleLineColor = CandleLineColorBox->Selected;
	g_Config.SetIntegerProperty("Setting","ChartKLineColor",CandleStickChart->CandleLineColor );
	ContractViewerForm->LoadChartColor( this );
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::BullColorBoxChange(TObject *Sender)
{
	CandleStickChart->BullColor = BullColorBox->Selected;
	g_Config.SetIntegerProperty("Setting","ChartBullColor", CandleStickChart->BullColor );
	ContractViewerForm->LoadChartColor( this );
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::BearColorBoxChange(TObject *Sender)
{
	CandleStickChart->BearColor = BearColorBox->Selected;
	g_Config.SetIntegerProperty("Setting","ChartBearColor", CandleStickChart->BearColor );
	ContractViewerForm->LoadChartColor( this );
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::MA1ColorBoxChange(TObject *Sender)
{
	CandleStickChart->MAColor1 = MA1ColorBox->Selected;
	g_Config.SetIntegerProperty("Setting","MA1Color", MA1ColorBox->Selected );
	ContractViewerForm->LoadChartColor( this );
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::MA2ColorBoxChange(TObject *Sender)
{
	CandleStickChart->MAColor2 = MA2ColorBox->Selected;
	g_Config.SetIntegerProperty("Setting","MA2Color", MA2ColorBox->Selected );
	ContractViewerForm->LoadChartColor( this );
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::MA3ColorBoxChange(TObject *Sender)
{
	CandleStickChart->MAColor3 = MA3ColorBox->Selected;
	g_Config.SetIntegerProperty("Setting","MA3Color", MA3ColorBox->Selected );
	ContractViewerForm->LoadChartColor( this );
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SectionLineColorBoxChange(TObject *Sender)
{
	CandleStickChart->SectionLineColor = SectionLineColorBox->Selected;
	g_Config.SetIntegerProperty( "Setting","SectionLineColor", SectionLineColorBox->Selected );
	ContractViewerForm->LoadChartColor( this );
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::LoadColor( void )
{
	int DorL = !(bool)g_Config.GetBoolProperty( "Setting","Skin", true );

	CandleStickChart->Color           = (TColor)g_Config.GetIntegerProperty("Setting","ChartBKColor" , ChartColor[DorL] );
	CandleStickChart->TextColor	      = (TColor)g_Config.GetIntegerProperty("Setting","ChartFontColor",ChartTextColor[DorL] );
	CandleStickChart->GridColor	      = (TColor)g_Config.GetIntegerProperty("Setting","ChartGridColor",ChartGridColor[DorL] );
	CandleStickChart->CandleLineColor = (TColor)g_Config.GetIntegerProperty("Setting","ChartKLineColor",CandleLineColor[DorL] );
	CandleStickChart->BullColor       = (TColor)g_Config.GetIntegerProperty("Setting","ChartBullColor" ,BullColor[DorL] );
	CandleStickChart->BearColor       = (TColor)g_Config.GetIntegerProperty("Setting","ChartBearColor" ,BearColor[DorL] );
	CandleStickChart->MAColor1        = (TColor)g_Config.GetIntegerProperty("Setting","MA1Color", MAColor1[DorL] );
	CandleStickChart->MAColor2        = (TColor)g_Config.GetIntegerProperty("Setting","MA2Color", MAColor2[DorL] );
	CandleStickChart->MAColor3        = (TColor)g_Config.GetIntegerProperty("Setting","MA3Color", MAColor3[DorL] );
	CandleStickChart->SectionLineColor= (TColor)g_Config.GetIntegerProperty("Setting","SectionLineColor", clOlive );
	ChartBKColorBox->Selected    = CandleStickChart->Color;
	ChartFGColorBox->Selected    = CandleStickChart->TextColor;
	GridColorBox->Selected       = CandleStickChart->GridColor;
	CandleLineColorBox->Selected = CandleStickChart->CandleLineColor;
	BullColorBox->Selected       = CandleStickChart->BullColor;
	BearColorBox->Selected       = CandleStickChart->BearColor;
	MA1ColorBox->Selected        = CandleStickChart->MAColor1;
	MA2ColorBox->Selected        = CandleStickChart->MAColor2;
	MA3ColorBox->Selected        = CandleStickChart->MAColor3;
	SectionLineColorBox->Selected= CandleStickChart->SectionLineColor;
   	OrdConfirmCheckBox->Checked  = g_Config.GetBoolProperty( "Setting","ConfirmBeforeNew", true );
	CandleStickChart->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SaveColor( void )
{
	g_Config.SetIntegerProperty("Setting","ChartBKColor",   CandleStickChart->Color);
	g_Config.SetIntegerProperty("Setting","ChartFontColor", CandleStickChart->TextColor );
	g_Config.SetIntegerProperty("Setting","ChartBullColor", CandleStickChart->BullColor );
	g_Config.SetIntegerProperty("Setting","ChartBearColor", CandleStickChart->BearColor );
	g_Config.SetIntegerProperty("Setting","ChartGridColor", CandleStickChart->GridColor );
	g_Config.SetIntegerProperty("Setting","ChartKLineColor",CandleStickChart->CandleLineColor );
	g_Config.SetIntegerProperty("Setting","MA1Color", CandleStickChart->MAColor1 );
	g_Config.SetIntegerProperty("Setting","MA2Color", CandleStickChart->MAColor2 );
	g_Config.SetIntegerProperty("Setting","MA3Color", CandleStickChart->MAColor3 );
	g_Config.SetIntegerProperty("Setting","SectionLineColor", CandleStickChart->SectionLineColor );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SetVisible( bool IsVisible )
{
	if( IsVisible == false )
		Visible = false;
	else
		Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::ShowBoxSwitchClick(TObject *Sender)
{
	if( ShowBoxSwitch->State == tssOff )
		CandleStickChart->ShowBox = false;
	else
		CandleStickChart->ShowBox = true;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::DayHLSwitchClick(TObject *Sender)
{
	if( DayHLSwitch->State == tssOff )
		CandleStickChart->ShowDayHL = false;
	else
		CandleStickChart->ShowDayHL = true;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::AvgPxSwitchClick(TObject *Sender)
{
	if( AvgPxSwitch->State == tssOff )
		CandleStickChart->ShowAvgPx = false;
	else
		CandleStickChart->ShowAvgPx = true;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::TradingSwitchClick(TObject *Sender)
{
	if( TradingSwitch->State == tssOff )
		CandleStickChart->EnableTrade = false;
	else
		CandleStickChart->EnableTrade = true;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SectionTypeComboBoxChange(TObject *Sender)
{
	CandleStickChart->HLSection = (SectionType)SectionTypeComboBox->ItemIndex;
	RepaintTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::PxGridEditChange(TObject *Sender)
{
	try
	{
		int NMA = PxGridEdit->Text.ToInt();
		PxGridUpDown->Position = NMA;
		CandleStickChart->TicksPerGrid = PxGridUpDown->Position;
	}
	catch(...)
	{
		PxGridEdit->Text = String( PxGridUpDown->Position );
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::PxGridUpDownChangingEx(TObject *Sender, bool &AllowChange,
          int NewValue, TUpDownDirection Direction)
{
	CandleStickChart->TicksPerGrid = NewValue;
	AllowChange = true;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::PxGridRadioGroupClick(TObject *Sender)
{
	CandleStickChart->PxGrid = (PxGridType)PxGridRadioGroup->ItemIndex;
	if( PxGridRadioGroup->ItemIndex == 0 )
	{
		PxGridEdit->Enabled = true;
		PxGridUpDown->Enabled = true;
	}
	else
	{
		PxGridEdit->Enabled = false;
		PxGridUpDown->Enabled = false;
	}

}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::RemoveFilledListener( const String& Ex,const String& Sym )
{
	gOrderStore->RemoveFilledListener(  Ex, Sym, this );
	CandleStickChart->ClearFilled();
	CandleStickChart->Invalidate();
	TradePointsButton->Selected = false;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::TradePointsButtonClick(TObject *Sender)
{
	if( gOrderStore->IsReady() == true )
	{
		if( TradePointsButton->Selected == false )
		{
			TradePointsButton->Selected = true;
			gOrderStore->AddFilledListener( FExchange, FSymbol, this );
		}
		else
		   RemoveFilledListener(  FExchange, FSymbol);
	}
	else
		TUnifyDlgs::MessageDialog( "Speedy Unify", L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------
void TLineChartForm::OnRecoverFilled( const String& Ex,const String& Sym, UFC::List<TFilledData>& Filleds )
{
	CandleStickChart->ClearFilled();
	for( int i =0;i< Filleds.ItemCount();i++ )
		CandleStickChart->AddFilled( Filleds[i] );
	CandleStickChart->Invalidate();
}
//---------------------------------------------------------------------------
void TLineChartForm::OnFilled( const String& Ex,const String& Sym, TFilledData& Filled )
{
	String Msg;

	if( Filled.Side == sBuy )
		Msg.printf( L"Buy Px[%0.2f] Qty[%d] Time[%04d]", Filled.FilledPx, Filled.Qty, Filled.hhmm );
	else
		Msg.printf( L"Sell Px[%0.2f] Qty[%d] Time[%04d]", Filled.FilledPx, Filled.Qty, Filled.hhmm );
	CandleStickChart->AddFilled( Filled );
	CandleStickChart->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SetEditMode( bool EditMode )
{
	if( EditMode  == true )
	{
		CrossGraphButton->Selected  = false;
		///< Disable all Zoom buttion
		FZoomTools.SetEnabled( false );
		///< Enable all edit buttion
		FEditTools.SetEnabled( true );
		LineColorBox->Visible = true;
	}
	else
	{
		CrossGraphButton->Selected  = true;
		///< Enable all Zoom buttion
		FZoomTools.SetEnabled( true );
		///< Disable all edit buttion
		FEditTools.SetEnabled( false );
		LineColorBox->Visible = false;
	}
	CandleStickChart->EditMode = EditMode;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::CrossGraphButtonClick(TObject *Sender)
{
	if( CrossGraphButton->Selected  == false )
	{
		CrossGraphButton->Selected  = true;
		CrossGraphButton->Hint = L"取價下單模式";
		SetEditMode( false );
	}
	else
	{
		CrossGraphButton->Selected  = false;
		CrossGraphButton->Hint = L"畫線模式";
		SetEditMode( true );
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::EditButtonClick(TObject *Sender)
{
	if( Sender == HLineButton )
		CandleStickChart->Edit = etHLine;
	else if( Sender == VLineButton )
		CandleStickChart->Edit = etVLine;
	else if( Sender == LineButton )
		CandleStickChart->Edit = et2PLine;
	else if( Sender == DelButton )
		CandleStickChart->Edit = etDelete;
	else if( Sender == TextButton )
		CandleStickChart->Edit = etText;
	else if( Sender == SelectButton )
		CandleStickChart->Edit = etDefault;
	FEditTools.Select((TGraphButton*)Sender );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::LineColorBoxChange(TObject *Sender)
{
	CandleStickChart->LineColor = LineColorBox->Selected;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::EnableColorSetting( bool IsEnable )
{
	ChartBKColorBox->Enabled = IsEnable;
	ChartFGColorBox->Enabled = IsEnable;
	GridColorBox->Enabled = IsEnable;
	BullColorBox->Enabled = IsEnable;
	BearColorBox->Enabled = IsEnable;
	CandleLineColorBox->Enabled = IsEnable;
	MA1ColorBox->Enabled = IsEnable;
	MA2ColorBox->Enabled = IsEnable;
	MA3ColorBox->Enabled = IsEnable;
	SectionLineColorBox->Enabled = IsEnable;
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::RoundFormExLockIconClick(TObject *Sender)
{
	if( RoundFormEx->Lock == true )
		EnableColorSetting( false );
	else
		EnableColorSetting( true );
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::SpaceComboBoxChange(TObject *Sender)
{
	int NK = SpaceComboBox->Text.ToInt();

	CandleStickChart->RightSpace = NK;
	if( AutoCenterSwitch->State == tssOn )
		CandleStickChart->CenterLastTick();
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::K1ComboBoxChange(TObject *Sender)
{
	int Index = FKMinComboBoxs.IndexOf( (TComboBox*)Sender );
	if( Index != -1)
	{
		TGraphButton* KBarButton = FKMinkeys[ Index ];
		String MinStr = FKMinComboBoxs[ Index ]->Text;
		String HintStr;
		int    Min = MinStr.ToInt();

		KBarButton->Tag = Min;
		KBarButton->ButtonText = MinStr;
		HintStr.printf( L"%d分鐘K", Min );
		KBarButton->Hint = HintStr;
		if(	KBarButton->Selected == true )
			CandleStickChart->MinutesPerStick = Min;
		SaveKBarSetting();
		ContractViewerForm->LoadChartKSetting( this );
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SkyLandToggleSwitchClick(TObject *Sender)
{
	if( SkyLandToggleSwitch->State == tssOff )
		CandleStickChart->ExableSkyLand = false;
	else
		CandleStickChart->ExableSkyLand = true;
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::PivotSwitchClick(TObject *Sender)
{
	if( PivotSwitch->State == tssOff )
		CandleStickChart->ExablePivot = false;
	else
	{
		CandleStickChart->PivotDeviation = GetDeviation() /100.0;
		CandleStickChart->ExablePivot = true;
	}
}
//---------------------------------------------------------------------------
double __fastcall TLineChartForm::GetDeviation( void )
{
	if( DeviationTrackBar->Position < 10 ) ///< 0~9 maps to 0.1~0.9
		return  ((double)DeviationTrackBar->Position)*0.1 + 0.1;
	else ///< 10~15 maps to 1~4
		return  ((double)DeviationTrackBar->Position) - 9.0;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::DeviationTrackBarChange(TObject *Sender)
{
	String txt;
	double dev = GetDeviation();

	txt.printf( L"轉折敏感度 %0.1f", dev );
	PivotDevLabel->Caption = txt;
	CandleStickChart->PivotDeviation = dev /100.0;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SentRoomiButtonClick(TObject *Sender)
{
	if( RoomiForm == NULL )
	{
		MainForm->IMButtonClick( NULL );
		if( RoomiForm != NULL )
			WinSleep( 150 );
	}
	CandleStickChart->SaveToClipboard();
	if( RoomiForm != NULL )
	{
		if( RoomiForm->Visible == false )
			RoomiForm->Show();
		RoomiForm->LoadFromClipboard();
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::TimeGridComboBoxChange(TObject *Sender)
{
	CandleStickChart->TimeGrid =(TimeGridType)TimeGridComboBox->ItemIndex;
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::CopyDepthButtonClick(TObject *Sender)
{
	ContractViewerForm->OpenOrderBookForm(FExchange,FSymbol,true);
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::CandleStickChartDelete(TObject *Sender, SideEnum side,
          double Price)
{
	try
	{
		if( gOrderStore->IsReady() == true )
			gOrderStore->CancelOrder( FMarket, FExchange, FSymbol, side, Price);
		else
			TUnifyDlgs::MessageDialog( "Speedy Unify", L"請先登入下單服務器" );
	}
	catch( UFC::Exception& execp )
	{
		String Msg;
		Msg.printf( L"刪單失敗[%s]", execp.what() );
		TUnifyDlgs::MessageDialog( "Speedy Unify", Msg ); ///< 刪單失敗
	}
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::CandleStickChartReplacePx(TObject *Sender, SideEnum side,
          double Price, double NewPrice)
{
	String Msg;
	ExchangeInfo*  ExInfo;

	try
	{
		if( (ExInfo = gMarketDataStore->GetExchangeInfo( FExchange ))!= NULL )
		{
			if( ExInfo->CanReplacePx() == true )
			{
				gOrderStore->ReplaceOrderPrice( FMarket, FExchange, FSymbol, side, Price, NewPrice, true );
				return;
			}
			else
				Msg.printf( L"改價失敗,交易所[%s]不支援改價", FExchange );
		}
		else
			Msg.printf( L"改價失敗,交易所[%s]不存在", FExchange );
	}
	catch( UFC::Exception& execp )
	{
		Msg.printf( L"改價失敗,原因[%s]", execp.what() );
	}
	TUnifyDlgs::MessageDialog( "Speedy Unify", Msg ); ///< 改價失敗
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::CandleStickChartStopOrderReplacePx(TObject *Sender,
		  SideEnum side, nsOrderMessageDefine::OrderTypeEnum Type, int Qty, double Price, double NewPrice,double ExecPx)
{
	if( gOrderStore->IsReady() == true )
	{
		PositionEffectEnum  PE = OrdConfirmForm->GetPositionEffect();

		gOrderStore->CancelStopOrder( FExchange, FSymbol, Price, side );
		Application->ProcessMessages( );
		UFC::BufferedLog::Printf( " -----[CandleStickChart::OnStopOrder]" );
		gOrderStore->NewStopOrderRequest( FExchange,                ///< Stop Exchange
										  FSymbol,                  ///< Stop symbol
										  NewPrice,                 ///< Stop price
										  FMarket,
										  FExchange,                ///< Execute Exchange
										  FSymbol,                  ///< Execute symbol
										  side,                     ///< Buy/Sell
										  ExecPx,                   ///< Execute price
										  Qty,                      ///< Qty
										  Type,
										  (Type == nsOrderMessageDefine::otMarket)?nsOrderMessageDefine::tifIOC:nsOrderMessageDefine::tifROD,
										  PE,                             ///< Position effect. ///<nsOrderMessageDefine::peAuto, ///< Position effect.
										  nsOrderMessageDefine::evtNone,
										  0,
										  true );                         ///< client side stop order.

	}
	else
		TUnifyDlgs::MessageDialog( "Speedy Unify", L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------
bool __fastcall TLineChartForm::PlaceOrder( SideEnum side, double Price, int Qty, OrderTypeEnum OrderType )
{
	try
	{
		AnsiString          Sym( FSymbol );
		PositionEffectEnum  PE = OrdConfirmForm->GetPositionEffect();
		char                TWSEOrdType = OrdConfirmForm->GetTWSEOrderType( side );
        bool                Rtn;

		if( side == sBuy )
			UFC::BufferedLog::Printf( " [TLineChartForm::PlaceOrder]Buy Symbol[%s] %d@%0.2f", Sym.c_str(),Qty, Price );
		else
			UFC::BufferedLog::Printf( " [TLineChartForm::PlaceOrder]Sell Symbol[%s] %d@%0.2f", Sym.c_str(),Qty, Price );
		Rtn = gOrderStore->NewOrder(  FMarket, ///< Market
									  FExchange,
									  FSymbol,  ///< symbol
									  side,     ///< side
									  Price,    ///< Price
									  Qty,      ///< Qty
									  OrderType,
									  nsOrderMessageDefine::tifROD,   ///< time in force.
									  PE,       ///< Position effect.
									  evtNone,  ///< Call or put
									  0.0,      ///< Options Strike price
									  0,        ///< Tick Count
									  0.0,      ///< Stop Price
									  TWSEOrdType );
		return Rtn;
	}
	catch( UFC::Exception& execp )
	{
		String Msg;
		Msg.printf( L"委託失敗[%s]", execp.what() );
		TUnifyDlgs::MessageDialog( "Speedy Unify", Msg ); ///< 委託失敗
		return false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::CandleStickChartNewOrder(TObject *Sender, SideEnum side,  double Price)
{
	if( gOrderStore->IsReady() == true )
	{
		AnsiString Sym( FSymbol );
		bool NeedsConfirm = g_Config.GetBoolProperty( "Setting","ConfirmBeforeNew", true );
		int      OrdQty  = 1;
		double   OrdPx   = Price;
		SideEnum OrdSide = side;

		if( NeedsConfirm == true )
		{
			OrdConfirmForm->Parent = this;
			OrdConfirmForm->Caption = Caption;
			OrdConfirmForm->Init( FExchange, FSymbol, side,  Price );

			if( OrdConfirmForm->ShowModal() == mrOk )
			{
				OrdQty  = OrdConfirmForm->GetQty();
				OrdSide = OrdConfirmForm->GetSide();
				OrdPx   = OrdConfirmForm->GetPrice();
				UFC::BufferedLog::Printf( " -----[TLineChartForm::OnNewOrder] Symbol[%s] Qty[%d]", Sym.c_str(), OrdQty );
				PlaceOrder( OrdSide, OrdPx, OrdQty , nsOrderMessageDefine::otLimit );
				OrdConfirmCheckBox->Checked = g_Config.GetBoolProperty( "Setting","ConfirmBeforeNew", true );
			}
		}
		else
		{
			UFC::BufferedLog::Printf( " -----[TLineChartForm::OnNewOrder] Symbol[%s] Qty[%d]", Sym.c_str(), OrdQty );
			PlaceOrder( OrdSide, OrdPx, OrdQty, nsOrderMessageDefine::otLimit );
		}
	}
	else
		TUnifyDlgs::MessageDialog( "Speedy Unify", L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::CandleStickChartDeleteStopOrder(TObject *Sender, SideEnum side,
		  double Price)
{
	if( gOrderStore->IsReady() == true )
		gOrderStore->CancelStopOrder( FExchange, FSymbol, Price, side );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::OrdConfirmCheckBoxClick(TObject *Sender)
{
	if( SaveAskOrderConfirm( OrdConfirmCheckBox->Checked ) == true )
		ContractViewerForm->LoadChartColor( this );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::AtAOELabelClick(TObject *Sender)
{
	OrdConfirmCheckBox->Checked =! OrdConfirmCheckBox->Checked;
	if( SaveAskOrderConfirm( OrdConfirmCheckBox->Checked ) == true )
		ContractViewerForm->LoadChartColor( this );
}
//---------------------------------------------------------------------------
bool __fastcall TLineChartForm::SaveAskOrderConfirm(  bool NewVal )
{
	bool OrigVal = g_Config.GetBoolProperty( "Setting","ConfirmBeforeNew", true );

	if( NewVal != OrigVal )
	{
		if( NewVal == false )
		{
			if( TUnifyDlgs::AskYesNoDialog( "線圖下單", "確定要取消顯示下單前的確認視窗? 取消後會用最後設定的張數,是否當沖,下單到交易所.") == true )
			{
				g_Config.SetBoolProperty( "Setting","ConfirmBeforeNew", false );
				return true; ///< Changed
			}
			else
			{
				OrdConfirmCheckBox->Checked = true;
				return false;
            }
		}
		else
		{
			g_Config.SetBoolProperty( "Setting","ConfirmBeforeNew", true );
			return true; ///< Changed
        }
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::LoadStopSetting( void )
{
	CandleStickChart->BuyStopOrderType  = g_Config.GetIntegerProperty("StopSetting","BuyOrdType", nsOrderMessageDefine::otLimit );
	CandleStickChart->SellStopOrderType = g_Config.GetIntegerProperty("StopSetting","SellOrdType", nsOrderMessageDefine::otLimit );
	CandleStickChart->BuyStopTick  = g_Config.GetIntegerProperty("StopSetting","BuyTick", 0 );
	CandleStickChart->SellStopTick = g_Config.GetIntegerProperty("StopSetting","SellTick", 0 );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::SaveStopSetting( void )
{
	g_Config.SetIntegerProperty("StopSetting","BuyOrdType", (int)CandleStickChart->BuyStopOrderType );
	g_Config.SetIntegerProperty("StopSetting","SellOrdType", (int)CandleStickChart->SellStopOrderType );
	g_Config.SetIntegerProperty("StopSetting","BuyTick", CandleStickChart->BuyStopTick );
	g_Config.SetIntegerProperty("StopSetting","SellTick", CandleStickChart->SellStopTick );
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::StopSettingBtnClick(TObject *Sender)
{
	TStopSettingForm* StopSettingForm = new TStopSettingForm( this );

	StopSettingForm->ApplySetting( CandleStickChart );
	if( RoundFormEx->Lock == true )
	{
	   Application->NormalizeTopMosts();
	   SetWindowPos( StopSettingForm->Handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
	}
	else
		SetWindowPos( Handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);
	if( StopSettingForm->ShowModal() == mrOk )
	{
		CandleStickChart->BuyStopOrderType = StopSettingForm->GetBuyOrderType();
		if( StopSettingForm->GetBuyOrderType() == nsOrderMessageDefine::otLimit )
			CandleStickChart->BuyStopTick = StopSettingForm->GetBuyTick();

		CandleStickChart->SellStopOrderType = StopSettingForm->GetSellOrderType();
		if( StopSettingForm->GetSellOrderType() == nsOrderMessageDefine::otLimit )
			CandleStickChart->SellStopTick = StopSettingForm->GetSellTick();
		SaveStopSetting();
		ContractViewerForm->LoadStopSetting( this );
	}
	delete StopSettingForm;
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::CandleStickChartNewStopOrder(TObject *Sender, SideEnum side,
		  double Px, OrderTypeEnum Type, double ExecPrice)
{
	if( gOrderStore->IsReady() == true )
	{
		PositionEffectEnum  PE = OrdConfirmForm->GetPositionEffect();

		UFC::BufferedLog::Printf( " -----[CandleStickChart::OnNewStopOrder]" );
		gOrderStore->NewStopOrderRequest( FExchange,                ///< Stop Exchange
										  FSymbol,                  ///< Stop symbol
										  Px,                 ///< Stop price
										  FMarket,
										  FExchange,                ///< Execute Exchange
										  FSymbol,                  ///< Execute symbol
										  side,                     ///< Buy/Sell
										  ExecPrice,                   ///< Execute price
										  1,                      ///< Qty
										  Type,
										  (Type == nsOrderMessageDefine::otMarket)?nsOrderMessageDefine::tifIOC:nsOrderMessageDefine::tifROD,
										  PE,                             ///< Position effect. ///<nsOrderMessageDefine::peAuto, ///< Position effect.
										  nsOrderMessageDefine::evtNone,
										  0,
										  true );                         ///< client side stop order.

	}
	else
		TUnifyDlgs::MessageDialog( "Speedy Unify", L"請先登入下單服務器" );
}
//---------------------------------------------------------------------------

void __fastcall TLineChartForm::CandleStickChartClosePosition(TObject *Sender, SideEnum side,
		  int Pos)
{
	double ClosePx;
	int    dir;
	nsOrderMessageDefine::SideEnum OrdSide;

	if( side ==  nsOrderMessageDefine::sSell )
	{
		dir   = -1;
		OrdSide  = nsOrderMessageDefine::sBuy;
	}
	else
	{
		dir   = 1;
		OrdSide  = nsOrderMessageDefine::sSell;
	}
	ClosePx = CandleStickChart->GetTickPrice( dir* MainForm->CloseBetterTick() ); ///< Get price.
	///< Place new order to close positions.
	if( Pos != 0 )
		PlaceOrder( OrdSide, ClosePx, Pos, nsOrderMessageDefine::otLimit );

}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::CandleStickChartNewText(TObject *Sender, TText *NewText,
		  bool &Accept)
{
	FontForm->TextEdit->Color = CandleStickChart->Color;
	FontForm->SetTextObject( NewText );
	if( FontForm->ShowModal() != mrOk )
		Accept = false;
}
//---------------------------------------------------------------------------
void __fastcall TLineChartForm::HLSectionBoxClick(TObject *Sender)
{
	CandleStickChart->DayHLSection = HLSectionBox->Checked;
}
//---------------------------------------------------------------------------

