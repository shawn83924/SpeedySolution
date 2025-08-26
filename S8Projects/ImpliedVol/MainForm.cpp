//---------------------------------------------------------------------------

#include <vcl.h>
#include <Math.hpp>
#pragma hdrstop

#include "MainForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
const double PI = 3.14159265358979323846;
const int    Times = 1000000;
const double TestValue = 0.0012;//2.1415;//9265358979323846;

//---------------------------------------------------------------------------
// 輸入S,K,R,T, Target 逼近出年化的 Implied Volatility.
//
// S 標的現貨價格，我們拿來算台指選的話，就拿台指期市價當作S
// K 履約價格 X
// R 年利率
// T 距離到期日的時間，要年化
//
// Target 選擇權的市價
//---------------------------------------------------------------------------
Options::Options( bool IsCall, double SpotPx, double StrikePx, double RiskFreeRate, double Time )
:FSpotPx( SpotPx )
,FStrikePx( StrikePx )
,FRiskFreeRate( RiskFreeRate )
,FTime( Time )
,FIsCall( IsCall )
,FLastPx( 0 )
,FLastVol( 0 )
{
	if( FIsCall == true )
	{
		FPxF = &Options::PriceCall;
		FGreeks = &Options::GreeksCall;
	}
	else
	{
		FPxF = &Options::PricePut;
		FGreeks = &Options::GreeksPut;
	}
	FLogSdK  = Log( FSpotPx / FStrikePx );
	FSqrtT   = sqrt( FTime );
	FExpMRT  = Exp( -FRiskFreeRate * FTime );
	FKExpMRT = FStrikePx * FExpMRT;
}
//---------------------------------------------------------------------------
// Prevent divided by zero.
//---------------------------------------------------------------------------
double Options::Divide(double ValueA, double ValueB)
{
   if( ValueB == 0 )
		return 0;
   else
		return (double)ValueA / ValueB;
}
//---------------------------------------------------------------------------
// 解決Log小於等於0 的問題(log(0),log(-1), 如 小於等於0 , 傳回0)
//---------------------------------------------------------------------------
double Options::Log( double Value )
{
   if( Value <= 0.0000000000001 )
	   return 0;
   else
	   return (double)log(Value);
}
//---------------------------------------------------------------------------
double Options::NormalDensity(double ZValue)
{
	return 1/sqrt( 2*PI ) * exp( -ZValue * ZValue / 2.0);
}
//---------------------------------------------------------------------------
double Options::NormalDistributeLUT( double ZValue )
{
	int Index;

	if( ZValue >= 0)
	{
		if( ZValue >= 4.0 ) // 超出表格外
			return 1.0;
		Index = (int)(ZValue*10000);
		return GNormalTable[Index];
	}
	else
	{
		if( ZValue <= -3.9999 ) // 超出表格外
			return 0;
		Index = (int)(ZValue*-10000);
		return (1.0 - GNormalTable[Index]);
	}
}
//---------------------------------------------------------------------------
double Options::NormalDistribute( double ZValue )
{
	double CND, L, k, kPow2,kPow3,kPow4,kPow5;
	double a1 =  0.31938153;
	double a2 = -0.356563782;
	double a3 =  1.781477937;
	double a4 = -1.821255978;
	double a5 =  1.330274429;

	if( ZValue < 0.0 )
		L = -1*ZValue;
	else
		L = ZValue;
	k     = 1.0 / ( 1.0 + 0.2316419 * L );
	kPow2 = k*k;
	kPow3 = kPow2*k;
	kPow4 = kPow3*k;
	kPow5 = kPow4*k;
	CND = 1.0 - 1.0/sqrt( 2.0*PI )* exp( -1.0*L*L / 2.0)*(a1*k + a2*kPow2 + a3*kPow3 + a4*kPow4 + a5*kPow5);
	if( ZValue < 0 )
		CND = 1.0 - CND;
	return CND;
}
//---------------------------------------------------------------------------
void Options::GreeksCall( double S,  double V )
{
	SetUnderlyingPx( S );
	double Yield  = 0.0; // 股價殖利率
	double d1     = Divide( FLogSdK + (Yield + V*V / 2.0) * FTime,  V * FSqrtT );
	double d2     = d1 - V * FSqrtT;
	double CNDd1  = NormalDistribute( d1 );
	double CNDd2  = NormalDistribute( d2 );
	double NDd1   = NormalDensity( d1 );

	// Call
	FTheoryPx = S * FExpMRT * CNDd1 - FKExpMRT * CNDd2;  //< 產出理論價
	FDelta    = FExpMRT * CNDd1;                         //< 產出Delta
	FGamma    = Divide( FExpMRT * NDd1, S * V * FTime ); //< 產出Gamma(Call Put 一樣)
	FVega     = S * FExpMRT * NDd1 * FSqrtT;             //< 產出Vega (Call Put 一樣)
	FTheta    = Divide( -S * FExpMRT * NDd1 * V, 2.0 * FSqrtT )
					 - (Yield - FRiskFreeRate) * S * FExpMRT * CNDd1
					 - FRiskFreeRate * FKExpMRT * CNDd2; //< 產出Theta
	FRho      = FTime * FKExpMRT * CNDd2;                //< 產生Rho
	if( FTheoryPx <= 0) //< 產出Lambda // 目前沒用到
		FLambda = 0;
	else
		FLambda = FDelta * S / FTheoryPx;
}
//---------------------------------------------------------------------------
void Options::GreeksPut( double S,  double V )
{
	SetUnderlyingPx( S );
	double Yield  = 0.0; // 股價殖利率
	double d1     = Divide( FLogSdK + (Yield + V*V / 2.0) * FTime,  V * FSqrtT );
	double d2     = d1 - V * FSqrtT;
	double CNDd1  = NormalDistribute( d1 );
	double _CNDd1 = NormalDistribute( -d1 );
	double _CNDd2 = NormalDistribute( -d2 );
	double NDd1   = NormalDensity( d1 );

	// Put
	FTheoryPx = S * FExpMRT * _CNDd1 - FKExpMRT * _CNDd2;  //< 產出理論價
	FDelta    = FExpMRT * (CNDd1-1.0);                     //< 產出Delta
	FGamma    = Divide( FExpMRT * NDd1, S * V * FTime );   //< 產出Gamma(Call Put 一樣)
	FVega     = S * FExpMRT * NDd1 * FSqrtT;               //< 產出Vega (Call Put 一樣)
	FTheta    = Divide( -S * FExpMRT * NDd1 * V, 2.0 * FSqrtT )
					 - (Yield - FRiskFreeRate) * S * FExpMRT * _CNDd1
					 - FRiskFreeRate * FKExpMRT * _CNDd2;  //< 產出Theta
	FRho      = FTime * FKExpMRT * _CNDd2;                 //< 產生Rho
	if( FTheoryPx <= 0) //< 產出Lambda // 目前沒用到
		FLambda = 0;
	else
		FLambda = FDelta * S / FTheoryPx;
}
//---------------------------------------------------------------------------
void  Options::UpdateGreeks( double S, double Volatility )
{
	(this->*FGreeks)( S, Volatility );
}
//---------------------------------------------------------------------------
bool Options::SetTime( double Time )
{
	if( FTime != Time )
	{
		FTime    = Time;
		FSqrtT   = sqrt( FTime );
		FExpMRT  = Exp( - FRiskFreeRate * FTime );
		FKExpMRT = FStrikePx * FExpMRT;
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool Options::SetUnderlyingPx( double S )
{
	if( FSpotPx != S )
	{
		FSpotPx = S;
		FLogSdK = Log( FSpotPx / FStrikePx );
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
double Options::PriceCall( double S,  double V )
{
	double b_call = ( FRiskFreeRate + 0.5 * V * V) * FTime;
	double b_put  = ( FRiskFreeRate - 0.5 * V * V) * FTime;
	double c      =  V * FSqrtT;
	double d1     = ( FLogSdK + b_call) / c;
	double d2     = ( FLogSdK + b_put ) / c;
	return (S * NormalDistribute( d1 ) - FKExpMRT * NormalDistribute(d2));
}
//---------------------------------------------------------------------------
double Options::PricePut( double S,  double V )
{
	double b_call = ( FRiskFreeRate + 0.5 * V * V) * FTime;
	double b_put  = ( FRiskFreeRate - 0.5 * V * V) * FTime;
	double c      =  V * FSqrtT;
	double d1     = ( FLogSdK + b_call) / c;
	double d2     = ( FLogSdK + b_put) / c;
	return ( FKExpMRT * NormalDistribute( -d2 ) - S * NormalDistribute( -d1 ));
}
//---------------------------------------------------------------------------
double Options::ImpliedVol( double Target )
{
	return ImpliedVol( FSpotPx, Target );
}
//---------------------------------------------------------------------------
double Options::ImpliedVol( double S, double Target )
{
	///< Underlying price or Options price changed.
	if( SetUnderlyingPx( S ) == true || FLastPx != Target )
	{
		double high    = 1.0;
		double low     = 0.0;
		double GuessPx = (this->*FPxF)(S, 0.5 );///< 從0.5開始逼近

		while( (high - low) > 0.00001 )
		{
			if( GuessPx > Target)
				high = (high + low) / 2;
			else
				low  = (high + low) / 2;
			GuessPx = (this->*FPxF)( S, (high + low) / 2 );
		}
		FLastPx  = Target;
		FLastVol = (high + low) / 2;
	}
	return FLastVol;
}
//---------------------------------------------------------------------------











//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
double NormalDistribute(double x)
{
	double CND=0.0;
	double L=0.0;
	double k=0.0;
	double a1=0.31938153;
	double a2=-0.356563782;
	double a3=1.781477937;
	double a4=-1.821255978;
	double a5=1.330274429;

	L = fabs(x);
	k = 1.0 / (1.0 + 0.2316419 * L);
	CND = 1.0 - 1.0 / sqrt(2.0 * PI) * exp(-Power(L, 2) / 2.0) *
	(a1 * k + a2 * Power(k, 2) + a3 * Power(k, 3) + a4 * Power(k, 4) + a5 * Power(k, 5));

	if (x < 0)
		CND = 1.0 - CND;

	return CND;
}
//---------------------------------------------------------------------------
double NormalDistributeEx( double x )
{
	double CND, L, k, kPow2,kPow3,kPow4,kPow5;
	double a1 =  0.31938153;
	double a2 = -0.356563782;
	double a3 =  1.781477937;
	double a4 = -1.821255978;
	double a5 =  1.330274429;

	if( x < 0.0 )
		L = -1*x;
	else
		L = x;
	k     = 1.0 / ( 1.0 + 0.2316419 * L );
	kPow2 = k*k;
	kPow3 = kPow2*k;
	kPow4 = kPow3*k;
	kPow5 = kPow4*k;

	CND = 1.0 - 1.0/sqrt( 2.0*PI )* exp( -1.0*L*L / 2.0)*(a1*k + a2*kPow2 + a3*kPow3 + a4*kPow4 + a5*kPow5);
	if( x < 0 )
		CND = 1.0 - CND;
	return CND;
}
//---------------------------------------------------------------------------
double NormalStdDistributeLUT( double ZValue )
{
	int Index;

	if( ZValue >= 0)
	{
		if( ZValue >= 4.0 ) // 超出表格外
			return 1.0;
		Index = (int)(ZValue*10000);
		return GNormalTable[Index];
	}
	else
	{
		if( ZValue <= -3.9999 ) // 超出表格外
			return 0;
		Index = (int)(ZValue*-10000);
		return (1.0 - GNormalTable[Index]);
	}
}
//---------------------------------------------------------------------------
double PriceC(double S, double K, double R, double T, double V)
{
	double a = log(S / K);
	double b_call = (R + 0.5 * pow(V, 2)) * T;
	double b_put = (R - 0.5 * pow(V, 2)) * T;
	double c = V * sqrt(T);
	double d1 = (a + b_call) / c;
	double d2 = (a + b_put) / c;
	double CallPrice = S * NormalDistribute(d1) - K * exp(-R * T) * NormalDistribute(d2);
	return CallPrice;
}
//---------------------------------------------------------------------------
// 輸入S,K,R,T, Target 逼近出年化的 Implied Volatility.
//
// S 現貨價格，我們拿來算台指選的話，就拿台指期市價當作S
// K 履約價格 X
// R 年利率
// T 距離到期日的時間，要年化
// Target 標的選擇權的市價
//---------------------------------------------------------------------------
double CallImpliedVol(double S, double K, double R, double T, double Target)
{
	double high = 1;
	double low  = 0;
	double GuessVol;

	while ((high - low) > 0.00001)
	{
		GuessVol = (high + low) / 2;
		if( PriceC(S, K, R, T, GuessVol ) > Target)
			high = GuessVol;
		else
			low = GuessVol;
	}
	GuessVol = (high + low) / 2;
	return GuessVol;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	DWORD Begin = ::GetTickCount();
	String Message;
	double Result;

	for( register int i=0;i< Times; i++ )
		 Result = NormalDistribute( TestValue );
	Message.printf( L"%d times use %d ms. Result[%0.5f] ", Times,(GetTickCount()- Begin), Result );
	ShowMessage( Message );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
	DWORD Begin = ::GetTickCount();
	String Message;
	double Result;

	for( register int i=0;i< Times; i++ )
		 Result = NormalDistributeEx( TestValue );
	Message.printf( L"%d times use %d ms. Result[%0.5f] ", Times,(GetTickCount()- Begin), Result );
	ShowMessage( Message );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender)
{
	DWORD Begin = ::GetTickCount();
	String Message;
	double Result;

	for( register int i=0;i< Times; i++ )
		 Result = NormalStdDistributeLUT( TestValue );
	Message.printf( L"%d times use %d ms. Result[%0.5f] ", Times,(GetTickCount()- Begin), Result );
	ShowMessage( Message );
}
//---------------------------------------------------------------------------
double UnnderlyingPx[] = {9034,9036};
double StrikePx = 9600;
double Price[] = {0.5, 0.8 };
//---------------------------------------------------------------------------
void __fastcall TForm1::Button4Click(TObject *Sender)
{
	DWORD Begin = ::GetTickCount();
	String Message;
	double Result;

	for( register int i=0;i< Times; i++ )
		 Result = CallImpliedVol( UnnderlyingPx[i%2], StrikePx, 0.005, 9.0/253.0, Price[i%2] );
	Message.printf( L"%d times use %d ms. Result[%0.5f] ", Times,(GetTickCount()- Begin), Result );
	ShowMessage( Message );
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button5Click(TObject *Sender)
{
	DWORD Begin = ::GetTickCount();
	String Message;
	double Result;

	Options OptPin( true, UnnderlyingPx[0], StrikePx, 0.005, 9.0/253.0 );
	for( register int i=0;i< Times; i++ )
	{
		 Result = OptPin.ImpliedVol( UnnderlyingPx[i%2], Price[i%2] );
		 OptPin.UpdateGreeks( UnnderlyingPx[i%2], Result );
	}
	Message.printf( L"%d times use %d ms. Result[%0.5f] ", Times,(GetTickCount()- Begin), Result );
	ShowMessage( Message );
}
//---------------------------------------------------------------------------
