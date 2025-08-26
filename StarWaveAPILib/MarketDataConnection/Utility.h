//---------------------------------------------------------------------------

#ifndef UtilityH
#define UtilityH
//---------------------------------------------------------------------------
#include "UFC.h"
#include "iniFile.h"
#include <vector>
#include "MarketDataMessage.h"
#include "NameValueMessage.h"
//---------------------------------------------------------------------------
#define MAX_PRE_CLOSE_DAYS  60
#define YEAR_TRADING_DAYS	252
#define STOCK_STATUS_BIT    32
extern double StdDev(double data[], int n);
//---------------------------------------------------------------------------
typedef enum
{
   etTAIFEX       = 0,
   etTWStock      = 1,
   etChinaFutures = 2,
   etCMELike      = 3,
   etCQGLike      = 4,
   etUnknownEx    = 5

}TExchangeType;
//---------------------------------------------------------------------------
typedef enum
{
	tdtTXFLike = 0,//"TXF","XIO","MXF","T5F"
	tdtFXFLike = 1,//"FXF","MSF","GDF","TGF"
	tdtEXFLike = 2,//"EXF","GTF"
	tdtGBFLike = 3,//"GBF","CPF"
	tdtTXOLike = 4,//"TXO"
	tdtTEOLike = 5,//"TEO","TFO","MSO","GTO"
	tdtTGOLike = 6, //"TGO"
	tdtRHOLike = 7, //"RHO","RTO"
	tdtUnknown = 8

}TTAIFEXDigiType;
//---------------------------------------------------------------------------
class Lookup
{
private:
	UFC::PHashMap<UFC::AnsiString,TExchangeType> FExMap;
	UFC::PHashMap<UFC::AnsiString,TTAIFEXDigiType> FDigiMap;
public:
	Lookup();
	TExchangeType ExchangeType( const UFC::AnsiString& Exchange );
	TTAIFEXDigiType TAIFEXDigiType( const UFC::AnsiString& Prod );
};
//---------------------------------------------------------------------------
class BasicInformation
{
private:
    Market                FMarket;
    UFC::NameValueMessage FMsg;
    UFC::AnsiString       FExchange;
    UFC::AnsiString       FSymbol;
    UFC::AnsiString       FProductID;
    UFC::AnsiString       FEndDate;
    UFC::AnsiString       FFirstNoticeDate;//add by vicky
    UFC::AnsiString       FMaturityDate;
    UFC::AnsiString       FMaturityDate1;
    UFC::AnsiString       FMaturityDate2;
    UFC::AnsiString       FAliasName;
    UFC::AnsiString       FChineseName;    
    double                FStrikePrice;
    double                FBullPrice;
    double                FBearPrice;
    double                FBullPrice2;
    double                FBearPrice2;
    double                FBullPrice3;
    double                FBearPrice3;
    double                FRefPrice;
    double                FTick;
    double                FContractMultiplier;
    double                FPreSettlementPx;
    double                FSettlementPx;
    int                   FDigit;
    CallPutCode           FCallPut;
    int                   FDecimalLocator;
    wchar_t*              FDisplayName;
    bool                  FTradeFlag;
    UFC::PStringList      FSpotList;
    UFC::AnsiString       FCurrency;
    UFC::AnsiString       FTradeSymbol;
    UFC::AnsiString       FUnderlying;
    bool                  FIsWarrant;
    int                   FExecuteQty;
    int                   FCancelQty;
    int                   FShares;
    double                FExecuteRate;
    int                   FTradeUnit;
    UFC::AnsiString       FLastTradeDate;
    int                   FTimeDiff; // add by joe
    int                   FExpiryDays;
    int                   FPreNCloseCount;
    int                   FMaxHistoryVolDays;
    int                   FNearMonthCode;   //add by vicky
    int                   FIsMostActive;    // true 1 false 0 //add by vicky

    double                FPreNClosePx[MAX_PRE_CLOSE_DAYS];
    double                FPreNChangeRange[MAX_PRE_CLOSE_DAYS];
    //--------------------TWSE/OTC Status------------------------------
    int                   FStockWarnCode;
    char                  FStockDayTradingIndicator;
    bool                  FStockIsMarginSale;
    bool                  FStockIsSecuritiesLendingSale;
    bool                  FStockIsNonTenTWDValue;
    bool                  FStockIsAbnormalRecommendation;
    bool                  FStockIsAbnormalSecurities;
	
private:
    UFC::AnsiString operator[] ( const UFC::AnsiString& FieldName );
public:
    BasicInformation( void );
    BasicInformation( const UFC::AnsiString& Msg );
    void SaveToStream( UFC::PStream* Stream );
    void LoadFromStream( UFC::PStream* Stream );
    void LoadFromString( const UFC::AnsiString& Msg );
public:
	///< Functions to set properties.
    void SetPreNChangeRange( void );
    void SetStrikePrice( double Price );
    void SetBullPrice( double Price );
    void SetBearPrice( double Price );
    void SetBullPrice2( double Price );
    void SetBearPrice2( double Price );
    void SetBullPrice3( double Price );
    void SetBearPrice3( double Price );
    void SetRefPrice( double Price );
    void SetDigit( int Digit );
    void SetCallPut( CallPutCode );
    void SetMaturityDate( const UFC::AnsiString& Date );
    void SetDisplayName( wchar_t* DisplayName );
    void SetAliasName( const UFC::AnsiString& AliasName );
    void SetDecimalLocator( int Decimal );
    void SetProductID( const UFC::AnsiString& ProductID ) { FProductID = ProductID; }
    void SetContractMultiplier( double CM );
    void SetUnderlying( const UFC::AnsiString& Underlying ) { FUnderlying = Underlying; }
    void SetPreNCloseCount( int iPreNCloseCount ) { FPreNCloseCount = iPreNCloseCount; } // add by joe
    void SetPreNClosePx( int n, double dPreNClosePx ) { FPreNClosePx[n] = dPreNClosePx; } // add by joe
    void SetPreSettlementPrice( double Price );	 //add by vicky
    void SetSettlementPrice( double Price );	 //add by vicky
	
	///< Functions to get properties.
	int              GetDecimalLocator( void );
	int              GetDigit( void );
	bool             IsMultileg( void );
	double           GetStrikePrice( void );
	double           GetBullPrice( void );
	double           GetBearPrice( void );
    double           GetBullPrice2( void );
	double           GetBearPrice2( void );
    double           GetBullPrice3( void );
	double           GetBearPrice3( void );
	double           GetRefPrice( void );
	double           GetContractMultiplier( void ) { return FContractMultiplier; }
	double           GetTick( void ) { return FTick; }
	Market           GetMarket( void );
	CallPutCode      GetCallPut( void );
	UFC::AnsiString& GetSymbol( void ) { return FSymbol; }
	UFC::AnsiString& GetExchange( void ) { return FExchange; }
	UFC::AnsiString& GetMaturityDate( void );
    UFC::AnsiString& GetMaturityDate1( void );
	UFC::AnsiString& GetMaturityDate2( void );
	UFC::AnsiString& GetAliasName( void ) { return FAliasName; }
	UFC::AnsiString& GetProductID( void ) { return FProductID; }
	UFC::AnsiString& GetEndDate( void );
	
	UFC::AnsiString  GetTickListSymbol( void );
	UFC::AnsiString& GetChineseName( void ) { return FChineseName; }
	wchar_t*         GetDisplayName( void ) { return FDisplayName; }
	bool			 GetTradeFlag( void ) { return FTradeFlag; }
	UFC::PStringList& GetSpotList( void ) { return FSpotList; }
	UFC::AnsiString& GetTradeSymbol( void ) { return FTradeSymbol; }
	UFC::AnsiString& GetCurrency( void ) { return FCurrency; }
	UFC::AnsiString& GetUnderlying( void ) { return FUnderlying; }
	bool			 IsWarrant( void ) { return FIsWarrant; }
	int 			 GetExecuteQty( void ) { return FExecuteQty; }
	int 			 GetCancelQty( void ) { return FCancelQty; }
	int				 GetShares( void ) { return FShares; }
	double		  	 GetExecuteRate( void ) { return FExecuteRate; }
	int				 GetTradeUnit( void ) { return FTradeUnit; }
	UFC::AnsiString& GetLastTradeDate( void ) { return FLastTradeDate; }    
	int              GetTimeDiff( void ) { return FTimeDiff; } // add by joe
	int              GetExpiryDays( void ) { return FExpiryDays; } // add by joe
	int              GetPreNCloseCount( void ) { return FPreNCloseCount; } // add by joe
	double 			 GetPreNClosePx( int n ) { return FPreNClosePx[n]; } // add by joe
	int 			 GetMaxHistoryVolDays( void ) { return FMaxHistoryVolDays; } // add by joe
    double 			 GetPreNChangeRange( int n ) { return FPreNChangeRange[n]; } // add by joe
	double 			 GetHistoryVol( int &iDays ); // add by joe
	int 			 GetNearMonthCode( void )    { return FNearMonthCode; }		//add by vicky
	int 			 GetIsMostActiveCode( void ) { return FIsMostActive; }	   //add by vicky
	double		  	 GetSettlementPrice( void ) { return FSettlementPx; }		   //add by vicky
	double		  	 GetPreSettlementPrice( void ) { return FPreSettlementPx; }    //add by vicky
	UFC::AnsiString  GetFirstNoticeDate( void ) {return FFirstNoticeDate;} //add by vicky
	//---------------TWSE/OTC Info-----------------   
	int 			 GetStockWarnCode ( void ) { return FStockWarnCode; }		   
	char 			 GetStockDayTradingIndicator( void ) { return FStockDayTradingIndicator; }
	bool 			 GetStockIsMarginSale( void ) { return FStockIsMarginSale; }
	bool 			 GetStockIsSecuritiesLendingSale( void ) { return FStockIsSecuritiesLendingSale; }
	bool 			 GetStockIsNonTenTWDValue( void ) { return FStockIsNonTenTWDValue; }
	bool 			 GetStockIsAbnormalRecommendation( void ) { return FStockIsAbnormalRecommendation; }
	bool 			 GetStockIsAbnormalSecurities( void ) { return FStockIsAbnormalSecurities; }
};
//---------------------------------------------------------------------------
class ExchangeInfo
{
private:
	UFC::AnsiString		FExchangeName;
	bool				FAmendQty;
	bool                FClAmendQty;
	bool				FReplacePx;
	bool                FClReplacePx;
	bool				FDayTrade;
	bool                FStopOrder; ///< Stop with market
	bool                FSWLOrder;  ///< Stop with limit
	bool				FTimeSpread;
	bool				FROD;
	bool				FIOC;
	bool				FFOK;
	bool				FMarketOrd;
	bool                FRODMarket;
	bool                FSupportOffhour; ///< Support TAIFEX offhour
	int					FMinutes;
	int					FMinutesEx;
	int					FSessionCount;
	int					FExtraSecondBeforeOpen;
	int					FExtraSecondAfterClose;
	int                 FTimeDiff;
	int                 FDays;                ///< KBar days.
	int                 FOffhourBeginMinutes; ///< Begin minutes
	UFC::UDateTime      FOffhourOpenTime;
	UFC::UDateTime      FOffhourCloseTime;
	UFC::PStringList	FStartTimeList;
	UFC::PStringList	FEndTimeList;
	UFC::PInt32List		FSessionMinutes;
	UFC::PInt32List		FSessionSecendsEx;
	UFC::List<UFC::UDateTime> FStartList;
	UFC::List<UFC::UDateTime> FEndList;
	UFC::List<UFC::UDateTime> FStartListEx;
	UFC::List<UFC::UDateTime> FEndListEx;
private:
	void  LoadTradingSupports( UFC::UiniFile& ExchangeConfig );
	void  LoadSessions( UFC::UiniFile& ExchangeConfig );
	void  GetHMFromString( UFC::AnsiString& Time, int& H, int& M );
public:
	ExchangeInfo( const UFC::AnsiString& ExchangeName, UFC::UiniFile& ExchangeConfig );
	void SetTAIFEXOffhour( bool SupportOffHour );
	void SetKBarDays( int Days );
public:
	UFC::AnsiString GetExchangeName( void ) { return FExchangeName; }
	bool            CanAmendQty( void )     { return FAmendQty; }    ///< Exchang support Amend qty?
	bool            ClientAmendQty( void )  { return FClAmendQty; }  ///< Client side use cancel/New as amend qty?
	bool            CanReplacePx( void )    { return FReplacePx; }   ///< Exchang support Replace Price ?
	bool            ClientReplacePx( void ) { return FClReplacePx; } ///< Client side use cancel/New as replace price?
	bool            CanDayTrade( void )     { return FDayTrade; }
	bool            CanPlaceTimeSpreadOrder( void ) { return FTimeSpread; }
	///< Support
	bool            CanPlaceStopOrder( void )     { return FStopOrder; }
	bool            CanPlaceStopWithLimit( void ) { return FSWLOrder; }
	bool            CanPlaceMarketOrder( void )   { return FMarketOrd; }
	///< Support Time in force.
	bool            CanPlaceRODOrder( void ) { return FROD; }
	bool            CanPlaceIOCOrder( void ) { return FIOC; }
	bool            CanPlaceFOKOrder( void ) { return FFOK; }
	bool            MarketRODOnly( void )    { return FRODMarket; }
public:
	int             TimeDiff( void ){ return FTimeDiff; }
	int             TotalMinutes( void ) { return FMinutes; }
	UFC::AnsiString GetSessionStartTime( int Session ) { return FStartTimeList[Session]; }
	UFC::AnsiString GetSessionEndTime( int Session ) { return FEndTimeList[Session]; }
	UFC::UDateTime  GetSessionStartDateTime( int Session ) { return FStartList[Session]; }
	UFC::UDateTime  GetSessionEndDateTime( int Session ) { return FEndList[Session]; }
	int				GetSessionCount( void ) { return FSessionCount; }
	void		    IndexToTime( int i, UFC::AnsiString& TimeStr );
	int				TimeToIndex( const UFC::UDateTime& DateTime, bool IsToday );
	int				TimeToIndex( int hour, int minute, bool IsToday );
	bool			IsTradingTime( int hour, int minute, bool IsToday );
public: ///< Functions for StockChartEx
	int             GetTotalTicksEx( void ) { return FMinutesEx; }
	int             TimeToIndexEx( const UFC::UDateTime& DateTime );
	int             TimeToIndexEx( int hour, int minute, int second );
	void			IndexToTimeEx( int i, UFC::AnsiString& TimeStr );
	int             GetExtraSecondBeforeOpen( void ) { return FExtraSecondBeforeOpen; }
	int             GetExtraSecondAfterClose( void ) { return FExtraSecondAfterClose; }
};
//---------------------------------------------------------------------------
#ifdef _WIN32
extern int __fastcall GenerateTicks( const UFC::AnsiString& Ex,    ///< [in]
									 const UFC::AnsiString& Symbol,///< [in]
									 double BullPx,                ///< [in]
									 double BearPx,                ///< [in]
									 std::vector<double>& TicksVector );///< [Out]
#endif
//---------------------------------------------------------------------------
#endif
