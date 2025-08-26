
#ifndef _OrderMessageDefine_H_
#define _OrderMessageDefine_H_
//--------------------------------------------------------------------------
namespace nsOrderMessageDefine
{
//--------------------------------------------------------------------------
typedef enum
{
	tetNew          = '0',
	tetCanceled     = '4',
	tetReplaced     = '5',
	tetFilled       = 'F',
	tetPxReplaced   = 'M',
	tetPxReplaced2  = 'm',
	tetOrderStatus  = 'I',
	tetNewAndFilled = '6'

} TMPExecTypeEnum;
//--------------------------------------------------------------------------
typedef enum
{
	mTWFutures      = 0, ///< TAIFEX Taiwan Futures
	mTWOptions      = 1, ///< TAIFEX Taiwan Options
	mTSE            = 2, ///< TSEC   Taiwan equity
	mOTC            = 3, ///< OTC    Taiwan
	mForeignFutures = 4, ///< Foreign Futures (with Order Routing)
	mForeignOptions = 5, ///< Foreign Options (with Order Routing)
	mForeignStock   = 6, ///< Foreign Stock   (with Order Routing)
	mCNFutures      = 7,
	mCNOptions      = 8,
	mES             = 9,
    mUnknown        = 99

} MarketEnum;
//--------------------------------------------------------------------------
typedef enum
{
    mtReplacePx          = 0,
    mtNew                = 1,
    mtCancel             = 2,
    mtReplace            = 3,
    mtQuote              = 4,
    mtQuoteCancel        = 5,
    mtQuoteRequest       = 6,
    mtOrderStatusRequest = 7,
    mtExecutionReport    = 8,
    mtNews               = 9

} MessageTypeEnum;
//--------------------------------------------------------------------------
typedef enum
{
	otNone                 = 0,
	otMarket               = 1,
	otLimit                = 2,
	otMarketWithProtection = 3,
	otStop                 = 4,
	otStopLimit            = 5,
    otMLtoLimit            = 6,
    otMarketOnClose        = 7,
    otLimitOnClose         = 8
} OrderTypeEnum;
//--------------------------------------------------------------------------
typedef enum
{
    sNone = 0,
    sBuy  = 1,
    sSell = 2

} SideEnum;
//--------------------------------------------------------------------------
typedef enum
{
    tifNone     = 0,
    tifROD      = 1,  //Day
    tifIOC      = 2,  //Immediate or Cancel
    tifFOK      = 3,  //Fill or Kill
    tifGTC      = 4,  //Good Till Cancel
    tifGTD      = 5,  //Good Till Date
    tifOPG      = 6,  //At the Opening
    tifCLS      = 7,  //At the Close
    tifGTX      = 8,  //Good Till Crossing
    tifTFXQ     = 9
} TimeInForceEnum;
//--------------------------------------------------------------------------
typedef enum
{
    evtNone	= 0,
    evtCall	= 1,
    evtPut	= 2

} EventTypeEnum;
//--------------------------------------------------------------------------
typedef enum
{
    ettNone    = 0,
    ettNew     = 1,
    ettCancel  = 2,
    ettCorrect = 3,
    ettStatus  = 4
    
} ExecTransTypeEnum;
//--------------------------------------------------------------------------
typedef enum
{
    etNone                =  0,
    etPendingNew          =  1,
    etNew                 =  2,
    etPendingReplace      =  3,
    etReplaced            =  4,
    etPendingCancel       =  5,
    etCanceled            =  6,
    etPartiallyFilled     =  7,
    etFilled              =  8,
    etQuoteAccept         =  9,
    etExpired             = 10,
    etRejected            = 11,
    etOrderStatus         = 12,
    etQuoteRequestAccept  = 13,
    etRestated            = 14,
    etTrade               = 15,
    etTradeCorrection     = 16,
    etTradeCancel         = 17
} ExecTypeEnum;
//--------------------------------------------------------------------------
typedef enum
{
    osNone               =  0,
    osPendingNew         =  1,
    osNew                =  2,
    osPendingReplace     =  3,
    osReplaced           =  4,
    osPendingCancel      =  5,
    osCanceled           =  6,
    osPartiallyFilled    =  7,
    osFilled             =  8,
    osQuoteAccept        =  9,
    osExpired            = 10,
    osRejected           = 11,
    osQuoteRequestAccept = 12
} OrderStatusEnum;
//--------------------------------------------------------------------------
typedef enum
{
    crrNone        = 0,
    crrNew         = 1,
    crrReplace     = 2,
    crrCancel      = 3,
    crrQuote       = 4,
    crrQuoteCancel = 5,
    crrOrderStatus = 6,
    crrQuoteRequest = 7

} CxlRejResponseToEnum;
//--------------------------------------------------------------------------
typedef enum
{
    errNone = 0

} ExecRestatementReasonEnum;
//--------------------------------------------------------------------------
typedef enum
{
    orrNone                               = 0,
    orrBrokerExchangeOption               = 1,
    orrUnknownSymbol                      = 2,
    orrExchangeClosed                     = 3,
    orrOrderExceedsLimit                  = 4,
    orrTooLateToEnter                     = 5,
    orrUnknownOrder                       = 6,
    orrDuplicateOrder                     = 7,
    orrDuplicateVerballyCommunicatedOrder = 8,
    orrStaleOrder                         = 9,
    orrTradeAlongRequired                 = 10,
    orrInvalidInvestorID                  = 11,
    orrUnsupportedOrderCharacteristic     = 12,
    orrSurveillenceOption                 = 13,
    orrIncorrectQuantity                  = 14,
    orrIncorrectAllocatedQuantity         = 15,
    orrUnknownAccount                     = 16,
    orrOther                              = 17

} OrdRejReasonEnum;
//--------------------------------------------------------------------------
typedef enum
{
	peOpen           = 0,
	peClose          = 1,
	peDayTrade       = 2,
	peDayTradeOpen   = 3,
	peAuto           = 4,
	peAutoToday      = 5,
	peForceClose     = 7,
	peTMPMarketMaker = 9,
	peTAIFEXAuto     = 10

} PositionEffectEnum;
//--------------------------------------------------------------------------
typedef enum
{
	pbNone     =   1,
	pbBase4    =   4,
	pbBase8    =   8,
	pbBase16   =  16,
	pbBase32   =  32,
	pbBase64   =  64,
	pbBase128  = 128,
	pbBase256  = 256
} PriceBaseEnum;
//--------------------------------------------------------------------------
typedef enum
{
	tsNormal      = 0,
	tsOddLot      = 1,
	tsOffHour     = 2,
	tsAuction     = 3,
	tsLend        = 4,
	tsTender      = 5,
	tsTenderEx    = 6,
	tsNegotiatePx = 7,
	tsAuto        = 8,
	tsIntradayOdd = 9

} TradingSessionIDEnum;
//--------------------------------------------------------------------------
typedef enum
{
	osDedicatedLine = 0,
	osAPI           = 1,
	osMobile        = 2,
	osWeb           = 3,
	osPCApplication = 4,
	osVoice         = 5,
	osGeneral       = 6

} OrderSourceEnum;
//--------------------------------------------------------------------------
typedef enum 
{
	hfSpeculation	= 1,
	hfArbitrage		= 2,
	hfHedge			= 3,
	hfCovered		= 4	
} CombHedgeFlagEnum;
//--------------------------------------------------------------------------
typedef enum 
{
	lcktLock		= 1,
	lcktUnlock		= 2,  
} LockTypeEnum;
//--------------------------------------------------------------------------
} //namespace nsOrderMessageDefine
//--------------------------------------------------------------------------
#endif
