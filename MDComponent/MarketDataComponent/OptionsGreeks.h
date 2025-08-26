//---------------------------------------------------------------------------

#ifndef OptionsGreeksH
#define OptionsGreeksH
//---------------------------------------------------------------------------

class TGreeks // 選擇權敏感度分析參數
{
public:
	double dTheoryPx;
	double dImpv;
	double dDelta;
	double dGamma;
	double dTheta;
	double dVega;
	double dRho;
	double dLambda;
	TGreeks():dTheoryPx(0.0),dImpv(0.0),dDelta(0.0),dGamma(0.0),dTheta(0.0),dVega(0.0),dRho(0.0),dLambda(0.0){}
} ;

//---------------------------------------------------------------------------

class OPTGreeks
{
private:
	static double cfnDivide(double ValueA, double ValueB);
	static double cfnLog( double Value);
	static double Pow( double a, double b );

private:
	static double getNormalStdDistribute(double x);
	static double getNormalStdDensity(double ZValue);
	static double GBlackScholes( int iCallPut, float sqrT, double S, double x, double T, double R, double b, double V);
	static double GVega(float sqrT, double S, double x, double T, double R, double b, double V);

public:
	static void __fastcall GetOPTGreeks(TGreeks& grkData, int iCallPut, double dSpotPx, double dOptTradePx, double dStrikePx,
										double dExpiryDayTimes, double dYearTradingDays, double dHistoryVol,  double dNoRiskInterestRate
										);
};
//---------------------------------------------------------------------------
#endif
