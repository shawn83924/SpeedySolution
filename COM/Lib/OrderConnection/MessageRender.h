//---------------------------------------------------------------------------
#ifndef TWMessageRenderH
#define TWMessageRenderH
//---------------------------------------------------------------------------
class TAIFEXR01
{
public:
	char Header[14];
	char FcmNo[7];           /* FCM ID */
	char OrderNo[5];         /* order # */
	char InvestorAcno[7];    /* investor account # */
	char InvestorFlag;       /* investor flag */
	char CommodityId[20];    /* Commodity ID ( Oprions 20 Futures 7) */
	char BuySellCode;        /* B/S code */
	char OrderType;          /* order type */
	char OrderPrice[9];      /* order price (OPTIONS 9, FUTURES 8)*/
	char OrderQty[4];        /* quantity */
	char OrderCondition;     /* order condition (Options only) */
	char OpenOffsetFlag;     /* open offset flag */
	char ClearMemberId[4];   /* clear member ID */
	char End;
public:
	TAIFEXR01( void );
	void SetFunction( int Func );
	char* ToString();
};
//---------------------------------------------------------------------------
class TWSET01Ex
{
public:
	char Header[14];
	char BrokerID[4];        ///< Broker ID
	char PVCID[2];           ///< PVCID
	char OrderNo[5];         ///< order #
	char InvestorAcno[7];    ///< investor account #
	char InvestorFlag;       ///< investor flag. ' ': Normal 'A': auto
				 ///<                'D': DMA    'I': internet
				 ///<                'V': Voice  'P': API
	char StockID[6];         ///< Stock ID
	char OrderPrice[9];      ///< *New* order price 9(4)v2 to 9(5)v4
	char OrderQty[6];        ///< *New* quantity  9(4) to 9(6)
	char BuySellCode;        ///< B/S code
	char ExchangeCode;
	char OrderType;          ///< order type
	char PriceType;          ///< *New* '1': Market '2':Limit
	char TimeInForce;        ///< *New* '0': ROD '3':IOC '4':FOK
	char End;
public:
	TWSET01Ex( void );
	void SetFunction( BOOL IsTSE, int Func );
	void SetIntradayOddFunc( BOOL IsTSE, int Func );
	char* ToString();
};
//---------------------------------------------------------------------------
#endif
