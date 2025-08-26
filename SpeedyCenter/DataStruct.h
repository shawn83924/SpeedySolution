//---------------------------------------------------------------------------
#ifndef DataStructH
#define DataStructH
//---------------------------------------------------------------------------
class ControlHeader
{
    private:
    	char SystemName[2];
	    char FunctionCode[2];
	    char MessageType[2];
	    char MessageTime[6];
	    char StatusCode[2];
    public:
        ControlHeader( char* SubSys,char* Func, char* MsgType );
        ControlHeader( char* Data );
        AnsiString AsString();
        int GetSubSysem();
        int GetFunctionCode();
        int GetMessageType();
};
//---------------------------------------------------------------------------
class R010 : public ControlHeader
{
    private:
        char FcmNo[7];           /* FCM ID */
        char OrderNo[5];         /* order # */
        char InvestorAcno[7];    /* investor account # */
        char InvestorFlag;       /* investor flag */
        char CommodityId[20];    /* Commodity ID */
        char BuySellCode;        /* B/S code */
        char OrderType;          /* order type */
        char OrderPrice[9];      /* order price */
        char OrderQty[4];        /* quantity */
        char OrderCondition;     /* order condition */
        char OpenOffsetFlag;     /* open offset flag */
        char ClearMemberId[4];   /* clear member ID */
    public:
        R010( char *FunctionCode );
        R010( AnsiString& Data );
        AnsiString AsString();
        void SetFCM( AnsiString );
        void SetOrderNo( AnsiString );
        void SetInvestorAccount( AnsiString );
        void SetInvestorFlag( char);
        void SetCommodityID( AnsiString );
        void SetBuySellCode( char );
        void SetOrderType( char );
        void SetOrderPrice( int );
        void SetOrderQuantity( int );
        void SetOrderCondition( char );
        void SetOpenOffsetFlag( char );
        void SetClearMemberID( AnsiString );
        AnsiString Print();
};
//---------------------------------------------------------------------------
class R010F : public ControlHeader
{
    private:
        char FcmNo[7];           /// FCM ID
        char OrderNo[5];         /// order
        char InvestorAcno[7];    /// investor account
        char InvestorFlag;       /// investor flag
        char CommodityId[7];     /// Commodity ID
        char SettlementMonth[6];
        char StrikePrice[8];
        char BuySellCode;        /// B/S code
        char OrderType;          /// order type
        char OrderPrice[8];      /// order price
        char OrderQty[4];        /// quantity
        char OpenOffsetFlag;     /// open offset flag
        char ClearMemberId[4];   /// clear member ID
    public:
        R010F( char *FunctionCode );
        R010F( AnsiString& Data );
        AnsiString AsString();
        void SetFCM( AnsiString );
        void SetOrderNo( AnsiString );
        void SetInvestorAccount( AnsiString );
        void SetInvestorFlag( char);
        void SetCommodityID( AnsiString );
        void SetSettlementMonth( AnsiString );
        void SetStrikePrice( int );
        void SetBuySellCode( char );
        void SetOrderType( char );
        void SetOrderPrice( int );
        void SetOrderQuantity( int );
        void SetOpenOffsetFlag( char );
        void SetClearMemberID( AnsiString );
        AnsiString Print();
};
//---------------------------------------------------------------------------
class R020 : public ControlHeader
{
    private:
        char FcmNo[7];
        char OrderNo[5];
        char InvestorAcno[7];
        char InvestorFlag;
        char CommodityId[20];
        char BuySellCode;
        char OrderType;
        char OrderPrice[9];
        char OrderQty[4];
        char OrderCondition;
        char OpenOffsetFlag;
        char ClearMemberId[4];
        char BeforeQty[4];
        char AfterQty[4];
        char ReceiveTime[8];
        char SequenceNo[8];
    public:
        R020( char *Data );
        AnsiString Print();
};
//---------------------------------------------------------------------------
class R070 : public ControlHeader
{
    private:
        char FFcmNo[7];     // broker ID
        char FCommodityId[20];   // production ID
    public:
        R070( AnsiString FCM, AnsiString CommodityId );
        AnsiString AsString();
};
//---------------------------------------------------------------------------
class R090 : public ControlHeader
{
    private:
        char FcmNo[7];     // broker ID
        char OrderNo[5];   // order No
        char InvestorAcno[7];    // account No
        char InvestorFlag;   // flag of account No
        char CommodityId[20];   // production ID
        char BuyPrice[9];  // buy order price
        char SellPrice[9]; // sell order price
        char BuyQty[4];    // buy order quantity
        char SellQty[4];   // sell order quantity
        char OrderCondition;    // order condition
        char OpenOffsetFlag;  // open offset code
        char ClearMemberId[4];    // member no
     public:
        R090( char *FunctionCode );
        AnsiString AsString();
        void SetFCM( AnsiString );
        void SetOrderNo( AnsiString );
        void SetInvestorAccount( AnsiString );
        void SetInvestorFlag( char);
        void SetCommodityID( AnsiString );
        void SetBuyPrice( int );
        void SetBuyQuantity( int );
        void SetSellPrice( int );
        void SetSellQuantity( int );
        void SetOrderCondition( char );
        void SetOpenOffsetFlag( char );
        void SetClearMemberID( AnsiString );
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
