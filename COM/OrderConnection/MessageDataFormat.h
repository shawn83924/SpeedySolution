#ifndef _MESSAGE_DATA_FORMAT_
#define _MESSAGE_DATA_FORMAT_

///<  Option R010,R020,C030,C030 Multilegs,C030 Cancel/Reduce
///<         R070,R080,R090,R100,C030 Quote Cancel/Reduce( Futures new version)
#define IDR_OPT_R010	 22001
#define IDR_OPT_R020	 22002
#define IDR_OPT_C030	 22003
#define IDR_OPT_C030M	 22004
#define IDR_OPT_C030CR	 22005
#define IDR_OPT_R070	 22006
#define IDR_OPT_R080	 22007
#define IDR_OPT_R090	 22008
#define IDR_OPT_R100	 22009
#define IDR_OPT_C030QCR	 22010
///< TSEC Normal,Odd lot, Off hour trade session.
#define IDR_TSE_T010     23001
#define IDR_TSE_O010     23002
#define IDR_TSE_P010     23003
#define IDR_TSE_T020     23004
#define IDR_TSE_T020Ex   23104
#define IDR_TSE_O020     23005
#define IDR_TSE_P020     23006
#define IDR_TSE_R030     23007
#define IDR_TSE_R030Ex   23107
///< TSEC Auction,Lend,Tender trade session.
#define IDR_TSE_A010     23008
#define IDR_TSE_A020     23009
#define IDR_TSE_V010     23010
#define IDR_TSE_V020     23011
#define IDR_TSE_E010     23012
#define IDR_TSE_E020     23013
#define IDR_TSE_Ex010    23014
#define IDR_TSE_Ex020    23015
///< OTC Normal,Odd lot, Off hour trade session.
#define IDR_OTC_T010     24001
#define IDR_OTC_O010     24002
#define IDR_OTC_P010     24003
#define IDR_OTC_T020     24004
#define IDR_OTC_T020Ex   24104
#define IDR_OTC_O020     24005
#define IDR_OTC_P020     24006
#define IDR_OTC_R030     24007
#define IDR_OTC_R030Ex   24107
///< OTC Lend,Tender trade session.
#define IDR_OTC_V010     24008
#define IDR_OTC_V020     24009
#define IDR_OTC_E010     24010
#define IDR_OTC_E020     24011
#define IDR_OTC_Ex010    24012
#define IDR_OTC_Ex020    24013

///< Error message table
#define IDR_ERROR_ENG    25001
#define IDR_ERROR_CHT    25002
#define IDR_FUTURES_PROD 25003
#define IDR_OPTIONS_PROD 25004

#define IDCHART_ROW_DATA 30001

#endif
