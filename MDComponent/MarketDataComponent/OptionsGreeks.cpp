//---------------------------------------------------------------------------
#pragma hdrstop
#include "OptionsGreeks.h"
//---------------------------------------------------------------------------
#include <Math.h>
#include <SysUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
int MathErrorReturnValue = 1;
//---------------------------------------------------------------------------
int _matherr(struct _exception *a)
{
	a->retval = 0;
	return MathErrorReturnValue;
}
//---------------------------------------------------------------------------
// 解決除0 的問題
double OPTGreeks::cfnDivide(double ValueA, double ValueB)
{
   if( ValueB == 0 )
		return 0;
   else
		return (double)ValueA / ValueB;
}
//---------------------------------------------------------------------------
// 解決Log小於等於0 的問題(log(0),log(-1), 如 小於等於0 , 傳回0)
double OPTGreeks::cfnLog( double Value)
{
   if( Value <= 0.0000000000001 )
	   return 0;
   else
	   return (double)log(Value);
}
//---------------------------------------------------------------------------
// 20131001 研究部 陳昱宏 經理 vba 公式
// CND // The cumulative normal distribution function
double OPTGreeks::getNormalStdDistribute(double x)
{
	double CND;//=0.0;
	double L;//=0.0;
	double k;//=0.0;
	double a1=0.31938153;
	double a2=-0.356563782;
	double a3=1.781477937;
	double a4=-1.821255978;
	double a5=1.330274429;

	L = fabs(x);
	k = 1.0 / (1.0 + 0.2316419 * L);
	CND = 1.0 - 1.0 / sqrt(2.0 * M_PI) * exp(-Pow(L, 2) / 2.0) * (a1 * k + a2 * Pow(k, 2) + a3 * Pow(k, 3) + a4 * Pow(k, 4) + a5 * Pow(k, 5));

	if (x < 0)
		CND = 1.0 - CND;

	return CND;
}
//---------------------------------------------------------------------------
// 20131001 研究部 陳昱宏 經理 vba 公式
// ND // The normal distribution function
double OPTGreeks::getNormalStdDensity(double ZValue)
{
	return 1/sqrt(2 * M_PI) * exp(-ZValue * ZValue / 2.0);
}
//---------------------------------------------------------------------------
double OPTGreeks::GBlackScholes( int iCallPut, float sqrT, double S, double x, double T, double R, double b, double V)
{
	double d1, d2;
	double dRtn;

	d1 = cfnDivide(cfnLog(S / x) + (b + Pow(V, 2) / 2.0) * T, V * sqrT);
	d2 = d1 - V * sqrT;

	if (iCallPut == 0) // Call
	{
		double CNDd1 = getNormalStdDistribute(d1);
		double CNDd2 = getNormalStdDistribute(d2);
		dRtn = S * exp((b - R) * T) * CNDd1 - x * exp(-R * T) * CNDd2;
	}
	else if (iCallPut == 1) // Put
	{
		double _CNDd1 = getNormalStdDistribute(-d1);
		double _CNDd2 = getNormalStdDistribute(-d2);
		dRtn = x * exp(-R * T) * _CNDd2 - S * exp((b - R) * T) * _CNDd1;
	}

	return dRtn;
}
//---------------------------------------------------------------------------
double OPTGreeks::Pow( double a, double b )
{
	errno = 0;
	if( a == 0.0 && b < 0.0 )
		return 0.0;
	try
	{
		double powab = pow( a, b );
		if ( errno == 0 )
		{
			//  std::pow succeeded (without overflow)
			return powab;
		}
		else
		{
			//  some error (probably overflow) with std::pow.
			return 0.0;
		}
	}
	catch(...)
	{
		return 0.0;
    }
}
//---------------------------------------------------------------------------
double OPTGreeks::GVega(float sqrT, double S, double x, double T, double R, double b, double V)
{
	double NDd1;
	/////
	double d1;
	double dRtn;
	/////

	d1 = cfnDivide(cfnLog(S / x) + (b + Pow(V, 2.0 ) / 2.0) * T, V * sqrT);
	// 產出NDd1
	NDd1 = getNormalStdDensity(d1);

	dRtn = S * exp((b - R) * T) * NDd1 * sqrT;

	return dRtn;
}
//---------------------------------------------------------------------------
// 輸出: grkData 選擇權敏感度分析參數
// 輸入: iCallPut: 0=Call; 1=Put, dSpotPx: 現貨(期貨)成交價, dOptTradePx: 選擇權成交價, dStrikePx: 履約價,
//       dExpiryDayTimes: 到期天數時間, dYearTradingDays: 一年的交易天數252, dHistoryVol: 歷史波動率0.15, dNoRiskInterestRate: 無風險利率0.05
//---------------------------------------------------------------------------
void __fastcall OPTGreeks::GetOPTGreeks( TGreeks& grkData, int iCallPut, double dSpotPx, double dOptTradePx, double dStrikePx,
							double dExpiryDayTimes, double dYearTradingDays, double dHistoryVol,  double dNoRiskInterestRate
						)
{
	////double          dExpiryDays = dExpiryDayTimes;
	////double          dYearTradingDays = YEAR_TRADING_DAYS;
	////double          dNoRiskInterestRate = 0.05; // 無風險利率

	////if( FHistoryVol  <= 0 ) // 歷史波動率
	////	FHistoryVol = 0.15;
	////return;

	double S; // nowPrice(=dSpotPx) // 現貨價格
	double x; // excercisePrice // 履約價格
	double T; // spareDay // 年化 // Ｔ
	double R; // NoRiskInterestRateYear // 無風險利率(年化)
	double b = 0.0; // 股價殖利率
	double V; // HistoricalVolatility // 歷史波動率
	double cm; // 成交價
	/////
	double d1, d2;
	double CNDd1, CNDd2;
	double _CNDd1, _CNDd2;
	double NDd1;
	/////
	float spareDay = dExpiryDayTimes / dYearTradingDays; // 年化 // Ｔ
	float sqrT = sqrt(spareDay); // 取 根號(平方根) √Ｔ
	double NoRiskInterestRateYear = dNoRiskInterestRate; // 值已年化(不用再乘spareDay)
	double HistoricalVolatility = dHistoryVol; // 歷史波動率
	/////
	S = dSpotPx; // 現貨價格
	x = dStrikePx; // 履約價格
	T = spareDay; // 年化 // Ｔ
	R = NoRiskInterestRateYear; // 無風險利率(年化)
	V = HistoricalVolatility; // 歷史波動率
	cm = dOptTradePx; // 成交價
	/////

	d1 = cfnDivide(cfnLog(S / x) + (b + Pow(V, 2.0) / 2.0) * T, V * sqrT);
	d2 = d1 - V * sqrT;

	CNDd1 = getNormalStdDistribute(d1);
	CNDd2 = getNormalStdDistribute(d2);
	_CNDd1 = getNormalStdDistribute(-d1);
	_CNDd2 = getNormalStdDistribute(-d2);

	// 產出NDd1
	NDd1 = getNormalStdDensity(d1);

	if (iCallPut == 0) // Call
	{
		// 產出理論價
		grkData.dTheoryPx = S * exp((b - R) * T) * CNDd1 - x * exp(-R * T) * CNDd2;
		// 產出Delta
		grkData.dDelta = exp((b - R) * T) * CNDd1;
		// 產出Gamma (Call Put 一樣)
		grkData.dGamma = cfnDivide(exp((b - R) * T) * NDd1, (S * V * T));
		// 產出Vega (Call Put 一樣)
		grkData.dVega = S * exp((b - R) * T) * NDd1 * sqrT;
		// 產出Theta
		grkData.dTheta = cfnDivide(-S * exp((b - R) * T) * NDd1 * V, 2.0 * sqrT)
						- (b - R) * S * exp((b - R) * T) * CNDd1
						- R * x * exp(-R * T) * CNDd2;
		// 產生Rho
		grkData.dRho = T * x * exp(-R * T) * CNDd2;
		// 產出Lambda // 目前沒用到
		if (grkData.dTheoryPx <= 0)
			grkData.dLambda = 0;
		else
			grkData.dLambda = grkData.dDelta * S / grkData.dTheoryPx;
	}
	else if (iCallPut == 1) // Put
	{
		// 產出理論價
		grkData.dTheoryPx = x * exp(-R * T) * _CNDd2 - S * exp((b - R) * T) * _CNDd1;
		// 產出Delta
		grkData.dDelta = exp((b - R) * T) * (CNDd1 - 1.0);
		// 產出Gamma (Call Put 一樣)
		grkData.dGamma = cfnDivide(exp((b - R) * T) * NDd1, (S * V * T));
		// 產出Vega (Call Put 一樣)
		grkData.dVega = S * exp((b - R) * T) * NDd1 * sqrT;
		// 產出Theta
		grkData.dTheta  = cfnDivide(-S * exp((b - R) * T) * NDd1 * V, 2.0 * sqrT)
						+ (b - R) * S * exp((b - R) * T) * _CNDd1
						+ R * x * exp(-R * T) * _CNDd2;
		// 產生Rho
		grkData.dRho = -T * x * exp(-R * T) * _CNDd2;
		// 產出Lambda // 目前沒用到
		if (grkData.dTheoryPx <= 0)
			grkData.dLambda = 0;
		else
			grkData.dLambda = grkData.dDelta * S / grkData.dTheoryPx;
	}

	grkData.dVega   = grkData.dVega / 100.0;
	grkData.dTheta  = grkData.dTheta / 100.0;
	grkData.dRho    = grkData.dRho / 100.0;
	grkData.dLambda = fabs(grkData.dLambda);

	/////
	double vi;
	double ci;
	double vegai;
	double epsilon;
	/////

	vi = sqrt(cfnDivide(fabs(cfnLog(cfnDivide(S, x)) + R * T) * 2.0, T));
	if (vi >= 0)
	{
		ci = GBlackScholes(iCallPut, sqrT, S, x, T, R, 0, vi);
		vegai = GVega(sqrT, S, x, T, R, 0, vi);
	}
	epsilon = 0.001;

	int iCount = 0;
	int iCountMax = 20;
	while (fabs(cm - ci) > epsilon && iCount < iCountMax && vi >= 0)
	{
		vi = vi - cfnDivide((ci - cm), vegai);
		//
		if (vi >= 0)
		{
			ci = GBlackScholes(iCallPut, sqrT, S, x, T, R, 0, vi);
			//
			vegai = GVega(sqrT, S, x, T, R, 0, vi);
			//
		}
		iCount++;
		if (iCount >= iCountMax)
			break;
	} //end while

	if (vi > 10) // 1000%
	{
		if (iCount >= iCountMax)
			vi = epsilon * 3;
		else
			vi = epsilon * 4;
	}
	else if (vi < 0)
	{
		if (iCount >= iCountMax)
			vi = epsilon;
		else
			vi = epsilon * 2;
	}
	grkData.dImpv = vi;
}
//---------------------------------------------------------------------------

