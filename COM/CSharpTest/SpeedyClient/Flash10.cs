using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using C1.Win.C1FlexGrid;

namespace Test1
{
    public partial class Flash10 : Form
    {
        public static Dictionary<string, StarWaveAPI.SecurityDefinition> commoditySymbolIndex = new Dictionary<string, StarWaveAPI.SecurityDefinition>();
        public static SortedList<string, StarWaveAPI.SecurityDefinition> sortCommodityIndex = new SortedList<string, StarWaveAPI.SecurityDefinition>();
        public static bool AddCommoditySymbolIndex(StarWaveAPI.SecurityDefinition commodityDefinition) 
        {
            bool IsSuccess = false;
            if (commodityDefinition == null) return IsSuccess;
            
            string indexKey = commodityDefinition.Exchange + " " + commodityDefinition.Symbol;
            try 
            {
                commoditySymbolIndex.Add(indexKey, commodityDefinition);
                sortCommodityIndex.Add(indexKey, commodityDefinition);
                IsSuccess = true;
            } 
            catch (ArgumentNullException ) 
            {
                return IsSuccess;
            } 
            catch (ArgumentException ) 
            {
                return IsSuccess;
            }
            return IsSuccess;
        }  //AddCommoditySymbolIndex()

        private ContractsForm mdMainForm = null;
        public ContractsForm MdMainForm 
        {
            get { return mdMainForm; }
            set { this.mdMainForm = value; }
        }  //MdMainForm
        const int DEPTH = 5;
        private string FExchange;
        private string FSymbol;
        static float FMinimalFontSize = 14.0F;
        Font _GridFont = new Font("Arial", FMinimalFontSize, FontStyle.Regular);
        Dictionary<decimal, C1.Win.C1FlexGrid.Row> PxToRow = new Dictionary<decimal, C1.Win.C1FlexGrid.Row>();
        Dictionary<C1.Win.C1FlexGrid.Row, decimal> RowToPx = new Dictionary<C1.Win.C1FlexGrid.Row, decimal>();
        decimal MatchPx;   //成交價格
        decimal[] BuyPx;   //Bid五檔價       
        decimal[] SellPx;  //Ask五檔價
        decimal FTick = 0; //檔差 
        decimal BullPx;    //漲停
        decimal BearPx;    //跌停
        decimal FRefPx;    //理論價
        string Digit = "1";  
        StarWaveAPI.SecurityDefinition _Def = null;
        string TableKey = "";
        System.Collections.ArrayList WorkingBuyPxs = new System.Collections.ArrayList();
        System.Collections.ArrayList WorkingSellPxs = new System.Collections.ArrayList();
        System.Collections.ArrayList FilledBuyPxs = new System.Collections.ArrayList();
        System.Collections.ArrayList FilledSellPxs = new System.Collections.ArrayList();
        int FMouseDownRow;
        int FMouseDownColumn;
        decimal FMouseDownPrice;
        int FMouseDownQty;
        OrderMessage.SideEnum FMouseDownSide;
        //************************************************************************************************************************************************************************************************************ 
        public Flash10(StarWaveAPI.SecurityDefinition Def)
        {
            InitializeComponent();
            _Def = Def;            
            FExchange = _Def.Exchange;
            FSymbol = _Def.Symbol;           
            TableKey = string.Format("{0} {1}", FExchange, FSymbol);
            this.Text = TableKey;

            cbBuyTimeInForce.Items.Clear();
            cbBuyTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifNone);
            cbBuyTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifROD);
            cbBuyTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifIOC);
            cbBuyTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifFOK);
            cbBuyTimeInForce.SelectedIndex = 1;

            cbSellTimeInForce.Items.Clear();
            cbSellTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifNone);
            cbSellTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifROD);
            cbSellTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifIOC);
            cbSellTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifFOK);
            cbSellTimeInForce.SelectedIndex = 1;

            //Grid視覺化處理
            InitializeGrid();
            DefineGridFont(_GridFont);
            InitializeGridStyle();
            AutoSizeColumns(this.c1FlexGrid1);
            
            //顯示上方第二列統計值
            c1FlexGrid1[1, 0] = "0.000";
            c1FlexGrid1[1, 1] = 0;
            c1FlexGrid1[1, 2] = 0;
            c1FlexGrid1[1, 3] = 0;
            c1FlexGrid1[1, 4] = "0:0";
            c1FlexGrid1[1, 5] = 0;
            c1FlexGrid1[1, 6] = 0;
            c1FlexGrid1[1, 7] = 0;
            c1FlexGrid1[1, 8] = "0.000";

            //初始化給值
            BuyPx = new decimal[5];
            SellPx = new decimal[5];
            for (int i = 0; i < 5; i++)
            {
                BuyPx[i] = 0;
                SellPx[i] = 0;
            }

            foreach (KeyValuePair<string, StarWaveAPI.SecurityDefinition> kvp in sortCommodityIndex) {
                string curKey = kvp.Key;
                int CurIndex = cbCommodity.Items.Add(kvp.Key);
                if (curKey.Equals(TableKey) == true) cbCommodity.SelectedIndex = CurIndex;
            }
            FMouseDownRow = 0;
            FMouseDownColumn = 0;
        }
        //************************************************************************************************************************************************************************************************************
        void InitializeGrid()//初始化視窗元件
        {
            c1FlexGrid1.DoubleBuffer = true;
            int ColNo = 0;
            int TWidth = 0;
            c1FlexGrid1.Clear();
            c1FlexGrid1.Cols.Add();
            c1FlexGrid1.Cols[ColNo].Caption = "買成(0)";
            c1FlexGrid1.Cols[ColNo].Name = "BidE4";           
            c1FlexGrid1.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].AllowEditing = false;
            TWidth = TWidth + c1FlexGrid1.Cols[ColNo].Width;
            ColNo++;

            c1FlexGrid1.Cols.Add();
            c1FlexGrid1.Cols[ColNo].Caption = "觸價";
            c1FlexGrid1.Cols[ColNo].Name = "BidSTP";           
            c1FlexGrid1.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].AllowEditing = false;
            TWidth = TWidth + c1FlexGrid1.Cols[ColNo].Width;
            ColNo++;

            c1FlexGrid1.Cols.Add();
            c1FlexGrid1.Cols[ColNo].Caption = "委託";
            c1FlexGrid1.Cols[ColNo].Name = "BidE5";           
            c1FlexGrid1.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].AllowEditing = false;
            TWidth = TWidth + c1FlexGrid1.Cols[ColNo].Width;
            ColNo++;

            c1FlexGrid1.Cols.Add();
            c1FlexGrid1.Cols[ColNo].Caption = "市場";
            c1FlexGrid1.Cols[ColNo].Name = "Bid";          
            c1FlexGrid1.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            c1FlexGrid1.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].AllowEditing = false;
            TWidth = TWidth + c1FlexGrid1.Cols[ColNo].Width;
            ColNo++;

            c1FlexGrid1.Cols.Add();
            c1FlexGrid1.Cols[ColNo].Caption = "Price";
            c1FlexGrid1.Cols[ColNo].Name = "Price";            
            c1FlexGrid1.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].AllowEditing = false;
            TWidth = TWidth + c1FlexGrid1.Cols[ColNo].Width;
            ColNo++;

            c1FlexGrid1.Cols.Add();
            c1FlexGrid1.Cols[ColNo].Caption = "市場";
            c1FlexGrid1.Cols[ColNo].Name = "Ask";           
            c1FlexGrid1.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            c1FlexGrid1.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].AllowEditing = false;
            TWidth = TWidth + c1FlexGrid1.Cols[ColNo].Width;
            ColNo++;

            c1FlexGrid1.Cols.Add();
            c1FlexGrid1.Cols[ColNo].Caption = "委託";
            c1FlexGrid1.Cols[ColNo].Name = "AskE5";           
            c1FlexGrid1.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].AllowEditing = false;
            TWidth = TWidth + c1FlexGrid1.Cols[ColNo].Width;
            ColNo++;

            c1FlexGrid1.Cols.Add();
            c1FlexGrid1.Cols[ColNo].Caption = "觸價";
            c1FlexGrid1.Cols[ColNo].Name = "AskSTP";           
            c1FlexGrid1.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].AllowEditing = false;
            TWidth = TWidth + c1FlexGrid1.Cols[ColNo].Width;
            ColNo++;

            c1FlexGrid1.Cols.Add();
            c1FlexGrid1.Cols[ColNo].Caption = "賣成(0)";
            c1FlexGrid1.Cols[ColNo].Name = "AskE4";            
            c1FlexGrid1.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            c1FlexGrid1.Cols[ColNo].AllowEditing = false;
            TWidth = TWidth + c1FlexGrid1.Cols[ColNo].Width;
            ColNo++;

            c1FlexGrid1.Cols.Count = ColNo;
            c1FlexGrid1.Cols["AskSTP"].Visible = false;
            c1FlexGrid1.Cols["BidSTP"].Visible = false;
        }  //InitializeGrid()
        //************************************************************************************************************************************************************************************************************ 
        void DefineGridFont(Font _Font)
        {
            c1FlexGrid1.Styles["Bid"].Font = _Font;
            c1FlexGrid1.Styles["Ask"].Font = _Font;
            c1FlexGrid1.Styles["BidE5"].Font = _Font;
            c1FlexGrid1.Styles["AskE5"].Font = _Font;
            c1FlexGrid1.Styles["Price"].Font = _Font;
            c1FlexGrid1.Styles["BidSTP"].Font = _Font;
            c1FlexGrid1.Styles["AskSTP"].Font = _Font;
            c1FlexGrid1.Styles["BidE4"].Font = _Font;
            c1FlexGrid1.Styles["AskE4"].Font = _Font;
            c1FlexGrid1.Styles["MatchPx"].Font = _Font;
            c1FlexGrid1.Styles["UpMatchPx"].Font = _Font;
            c1FlexGrid1.Styles["DownMatchPx"].Font = _Font;
            c1FlexGrid1.Styles["BearPx"].Font = _Font;
            c1FlexGrid1.Styles["BullPx"].Font = _Font;
            c1FlexGrid1.Styles["MatchPx_UpRef"].Font = _Font;
            c1FlexGrid1.Styles["MatchPx_DownRef"].Font = _Font;
            c1FlexGrid1.Styles["Fixed"].Font = _Font;
        }  //DefineGridFont()
        //************************************************************************************************************************************************************************************************************
        void InitializeGridStyle()
        {
            c1FlexGrid1.Cols["Bid"].Style = c1FlexGrid1.Styles["Bid"];
            c1FlexGrid1.Cols["Ask"].Style = c1FlexGrid1.Styles["Ask"];
            c1FlexGrid1.Cols["BidE5"].Style = c1FlexGrid1.Styles["BidE5"];
            c1FlexGrid1.Cols["AskE5"].Style = c1FlexGrid1.Styles["AskE5"];
            c1FlexGrid1.Cols["Price"].Style = c1FlexGrid1.Styles["Price"];
            c1FlexGrid1.Cols["BidSTP"].Style = c1FlexGrid1.Styles["BidSTP"];
            c1FlexGrid1.Cols["AskSTP"].Style = c1FlexGrid1.Styles["AskSTP"];
            c1FlexGrid1.Cols["BidE4"].Style = c1FlexGrid1.Styles["BidE4"];
            c1FlexGrid1.Cols["AskE4"].Style = c1FlexGrid1.Styles["AskE4"];
        }  //InitializeGridStyle()
        //************************************************************************************************************************************************************************************************************ 
        private void AutoSizeColumns(C1FlexGrid _Grid)
        {
            using (Graphics g = c1FlexGrid1.CreateGraphics())
            {
                SizeF Rsz = g.MeasureString(c1FlexGrid1[0, 0].ToString(), _GridFont);
                _Grid.Rows.DefaultSize = (int)Rsz.Height + 3;

                int GridWith = 0;
                for (int i = 0; i < _Grid.Cols.Count; i++)
                {
                    SizeF sz;
                    if (i == 4)
                        sz = g.MeasureString("0000.00(00)", _GridFont);
                    else
                        sz = g.MeasureString(c1FlexGrid1[0, i].ToString(), _GridFont);

                    _Grid.Cols[i].Width = (int)sz.Width;
                    switch (i)
                    {
                        case 0:  //BidE4 買成(0)
                            if (c1FlexGrid1.Cols["AskE4"].Visible == true)
                                _Grid.Cols[i].Width += 16;
                            else
                                _Grid.Cols[i].Width = 0;
                            break;
                        case 1:  //BidSTP  觸價
                            if (c1FlexGrid1.Cols["AskSTP"].Visible == true)
                                _Grid.Cols[i].Width += 4;
                            else
                                _Grid.Cols[i].Width = 0;
                            break;
                        case 2:  //BidE5  委託
                            _Grid.Cols[i].Width += 6;
                            break;
                        case 3:  //Bid  市場
                            _Grid.Cols[i].Width += 20;
                            break;
                        case 4:  //Price  Price
                            //_Grid.Cols[i].Width += 40;
                            break;
                        case 5:  //Ask  市場
                            _Grid.Cols[i].Width += 20;
                            break;
                        case 6:  //AskE5  委託
                            _Grid.Cols[i].Width += 6;
                            break;
                        case 7:  //AskSTP  觸價
                            if (c1FlexGrid1.Cols["BidSTP"].Visible == true)
                                _Grid.Cols[i].Width += 6;
                            else
                                _Grid.Cols[i].Width = 0;
                            break;
                        case 8:  //AskE4  賣成(0)
                            if (c1FlexGrid1.Cols["BidE4"].Visible == true)
                                _Grid.Cols[i].Width += 16;
                            else
                                _Grid.Cols[i].Width = 0;
                            break;
                    }
                    GridWith += _Grid.Cols[i].Width;
                }
                GridWith += System.Windows.Forms.SystemInformation.VerticalScrollBarWidth;
                _Grid.Width = GridWith;
                pnSetting.Width = GridWith;
                this.MaximumSize = new Size(0, 0);
                this.Width = _Grid.Width + 12;
                this.MaximumSize = new Size(this.Width, 1500);
            }
        }  //AutoSizeColumns()
        //************************************************************************************************************************************************************************************************************ 
        public void OnRecover(StarWaveAPI.Recover Recover)
        {          
            //都沒有資料開始畫底圖
            if (PxToRow.Count == 0)
            {
                //先計算出商品檔差
                FTick = Convert.ToDecimal(Recover.BuyPx[1]) - Convert.ToDecimal(Recover.BuyPx[2]);

                if (_Def.Market == StarWaveAPI.EnumMarket.TWOptions)
                    FTick = this.calOPtick((double)_Def.BullPx, TableKey);
                else
                    FTick = this.calFUTtick(TableKey, (double)FTick);

                WriteLogFile(string.Format("FTick-->{0}", FTick));
                if (FTick == 0)
                    return;

                bool DoTick = true;
                decimal Px = Convert.ToDecimal(_Def.BullPx) - FTick + FTick;//利用decimal+-調整漲停第一次格式
                while (DoTick)
                {
                    C1.Win.C1FlexGrid.Row NewRow = c1FlexGrid1.Rows.Add();
                    NewRow["BidE4"] = "";
                    NewRow["BidSTP"] = "";
                    NewRow["BidE5"] = "";
                    NewRow["Bid"] = "";
                    NewRow["Price"] = Px;
                    NewRow["Ask"] = "";
                    NewRow["AskE5"] = "";
                    NewRow["AskSTP"] = "";
                    NewRow["AskE4"] = "";
                    PxToRow.Add(Px, NewRow);
                    RowToPx.Add(NewRow, Px);

                    if (Px == Convert.ToDecimal(_Def.BearPx) || Px <= 0)
                        DoTick = false;

                    if (_Def.Market == StarWaveAPI.EnumMarket.TWOptions)
                        Px = Px - this.calOPtick((double)Px, TableKey);
                    else
                        Px = Px - FTick;
                    NewRow = null;
                }
                BullPx = Convert.ToDecimal(_Def.BullPx);
                BearPx = Convert.ToDecimal(_Def.BearPx);
                FRefPx = Convert.ToDecimal(_Def.RefPx);
                Digit = GetDigit(_Def.Digit);
                
                if (Recover.MatchPx != 0)
                {
                    UpdateMatchPx(Convert.ToDecimal(Recover.MatchPx.ToString(Digit)), Recover.MatchQty);
                }
                else//用理論價格-->昨收
                {
                    UpdateMatchPx(Convert.ToDecimal(_Def.RefPx.ToString(Digit)), Recover.MatchQty);
                }

                //################################
                double[] _BuyPx = new double[5];
                int[] _BuyQty = new int[5];
                double[] _SellPx = new double[5];
                int[] _SellQty = new int[5];
                //################################
                for (int i = 0; i < 5; i++)
                {
                    if (Recover.BuyPx[i + 1] != 0)
                        _BuyPx[i] = Recover.BuyPx[i + 1];
                    if (Recover.BuyQty[i + 1] != 0)
                        _BuyQty[i] = Recover.BuyQty[i + 1];
                    if (Recover.SellPx[i + 1] != 0)
                        _SellPx[i] = Recover.SellPx[i + 1];
                    if (Recover.SellQty[i + 1] != 0)
                        _SellQty[i] = Recover.SellQty[i + 1];
                }
                UpdateOrderBook(_BuyPx, _BuyQty, _SellPx, _SellQty);     
            }
        }  //OnRecover()
        //************************************************************************************************************************************************************************************************************
        public void OnMatchInfo(StarWaveAPI.Match MatchInfo)
        {
            //成交資訊更新
            UpdateMatchPx(Convert.ToDecimal(MatchInfo.MatchPx.ToString(Digit)), MatchInfo.MatchQty);
        }
        //************************************************************************************************************************************************************************************************************
        void UpdateMatchPx(decimal _MatchPx, int _MatchQty)
        {
            //先清除舊資料
            string DisplayStr = string.Format("{0}({1})", _MatchPx, _MatchQty);
            if (MatchPx != 0)
            {
                if (PxToRow.ContainsKey(MatchPx) == true)
                {
                    C1.Win.C1FlexGrid.Row C1Row = PxToRow[MatchPx];

                    if (DisplayStr == C1Row["Price"].ToString())//內容都一樣,不要更新UI了
                        return;

                    string[] _MPx = C1Row["Price"].ToString().Split('(');
                    C1Row["Price"] = _MPx[0];
                    _MPx = null;
                }
            }
            if (_MatchPx != 0)
            {
                if (PxToRow.ContainsKey(_MatchPx) == true)
                {
                    C1.Win.C1FlexGrid.Row C1Row = PxToRow[_MatchPx];
                    C1Row["Price"] = DisplayStr;
                    DisplayStr = null;
                    //成交價置中
                    if (cbCenter5.Checked) CenterGrid(C1Row, false);
                }
            }
            MatchPx = _MatchPx;
        }
        //************************************************************************************************************************************************************************************************************
        void CenterGrid(C1.Win.C1FlexGrid.Row C1Row, bool IsMouse)
        {          
            int TopRow = c1FlexGrid1.TopRow;
            int BottomRow = c1FlexGrid1.BottomRow;
            int visibleRows = BottomRow - TopRow + 1;
            int centerRow_offset = visibleRows / 2;
            int TopRow_index = 0;
            TopRow_index = C1Row.Index - centerRow_offset;
            if (TopRow_index < 0) TopRow_index = 0;
            c1FlexGrid1.TopRow = TopRow_index;
        }
        //************************************************************************************************************************************************************************************************************
        public void OnOrderBookData(StarWaveAPI.OrderBook OrderBookData)
        {            
            //################################
            double[] _BuyPx = new double[5];
            int[] _BuyQty = new int[5];
            double[] _SellPx = new double[5];
            int[] _SellQty = new int[5];
            //################################
            for (int i = 0; i < 5; i++)
            {
                if (OrderBookData.BuyPx[i + 1] != 0)
                    _BuyPx[i] = OrderBookData.BuyPx[i + 1];
                if (OrderBookData.BuyQty[i + 1] != 0)
                    _BuyQty[i] = OrderBookData.BuyQty[i + 1];
                if (OrderBookData.SellPx[i + 1] != 0)
                    _SellPx[i] = OrderBookData.SellPx[i + 1];
                if (OrderBookData.SellQty[i + 1] != 0)
                    _SellQty[i] = OrderBookData.SellQty[i + 1];
            }
            UpdateOrderBook(_BuyPx, _BuyQty, _SellPx, _SellQty);            
        }
        //************************************************************************************************************************************************************************************************************
        void UpdateOrderBook(double[] _BuyPx, int[] _BuyQty, double[] _SellPx, int[] _SellQty)
        {
            try
            {
                //顯示資料
                int TBidQty = 0;
                int TAskQty = 0;

                for (int i = 0; i < 5; i++)
                {
                    if (PxToRow.ContainsKey(BuyPx[i]) == true)
                    {
                        C1.Win.C1FlexGrid.Row C1Row = PxToRow[BuyPx[i]];
                        C1Row["Bid"] = "";
                    }
                    if (PxToRow.ContainsKey(SellPx[i]) == true)
                    {
                        C1.Win.C1FlexGrid.Row C1Row = PxToRow[SellPx[i]];
                        C1Row["Ask"] = "";
                    }
                }
                for (int i = 0; i < 5; i++)
                {
                    decimal TBidPx = Convert.ToDecimal(_BuyPx[i]);
                    decimal TAskPx = Convert.ToDecimal(_SellPx[i]);

                    if (PxToRow.ContainsKey(TBidPx) == true)
                    {
                        C1.Win.C1FlexGrid.Row C1Row = PxToRow[TBidPx];
                        C1Row["Bid"] = _BuyQty[i];
                    }

                    if (PxToRow.ContainsKey(TAskPx) == true)
                    {
                        C1.Win.C1FlexGrid.Row C1Row = PxToRow[TAskPx];
                        C1Row["Ask"] = _SellQty[i];
                    }

                    TBidQty = TBidQty + _BuyQty[i];
                    BuyPx[i] = TBidPx;

                    TAskQty = TAskQty + _SellQty[i];
                    SellPx[i] = TAskPx;
                }
                c1FlexGrid1[1, "Bid"] = TBidQty;
                c1FlexGrid1[1, "Ask"] = TAskQty;               
            }
            catch (Exception ex)
            {
                this.WriteLogFile(ex.ToString());
            }
        }
        //************************************************************************************************************************************************************************************************************
        private string GetDigit(int _iDigit)
        {
            int iDigit = _iDigit; // 小數位數
            string sDigit = "0";
            if (iDigit > 0)
            {
                sDigit = "0." + sDigit.PadRight(iDigit, '0');
            }
            else if (iDigit < 0)
            {
                sDigit = "";
            }
            return sDigit;
        }
        //************************************************************************************************************************************************************************************************************ 
        public string Exchange
        {
            get
            {
                return FExchange;
            }
        }
        //************************************************************************************************************************************************************************************************************ 
        public string Symbol
        {
            get
            {
                return FSymbol;
            }
        }      
        //************************************************************************************************************************************************************************************************************
        public decimal calFUTtick(string TableKey, double _tick)
        {
            string[] Key = TableKey.Split(' ');
            double tick = 1;
            switch (Key[1].Substring(0, 3))
            {
                case "TXF":
                    tick = 1;
                    break;
                case "MXF":
                    tick = 1;
                    break;
                case "FXF":
                    tick = 0.2;
                    break;
                case "EXF":
                    tick = 0.05;
                    break;
                case "GTF":
                    tick = 0.05;
                    break;
                case "XIF":
                    tick = 1;
                    break;
                case "T5F":
                    tick = 1;
                    break;
                case "MSF":
                    tick = 0.1;
                    break;
                case "GDF":
                    tick = 0.1;
                    break;
                case "GBF":
                    tick = 0.005;
                    break;
                case "CPF":
                    tick = 0.005;
                    break;
                case "TGF":
                    tick = 0.5;
                    break;
                default:
                    tick = _tick;
                    break;
            }
            return (decimal)tick;
        }
        //************************************************************************************************************************************************************************************************************
        public decimal calOPtick(double opprice, string TableKey)
        {
            string[] Key = TableKey.Split(' ');
            double tick = 1;
            switch (Key[1].Substring(0, 3))
            {
                case "TXO":
                    if (Convert.ToDouble(opprice) <= 10)
                    {
                        tick = 0.1;
                    }
                    else if ((Convert.ToDouble(opprice) > 10) && (Convert.ToDouble(opprice) <= 50))
                    {
                        tick = 0.5;
                    }
                    else if ((Convert.ToDouble(opprice) > 50) && (Convert.ToDouble(opprice) <= 500))
                    {
                        tick = 1;
                    }
                    else if ((Convert.ToDouble(opprice) > 500) && (Convert.ToDouble(opprice) <= 1000))
                    {
                        tick = 5;
                    }
                    else if (Convert.ToDouble(opprice) > 1000)
                    {
                        tick = 10;
                    }
                    break;
                case "TFO":
                    if (Convert.ToDouble(opprice) <= 2)
                    {
                        tick = 0.02;
                    }
                    else if ((Convert.ToDouble(opprice) > 2) && (Convert.ToDouble(opprice) <= 10))
                    {
                        tick = 0.1;
                    }
                    else if ((Convert.ToDouble(opprice) > 10) && (Convert.ToDouble(opprice) <= 100))
                    {
                        tick = 0.2;
                    }
                    else if ((Convert.ToDouble(opprice) > 100) && (Convert.ToDouble(opprice) <= 200))
                    {
                        tick = 1;
                    }
                    else if (Convert.ToDouble(opprice) > 200)
                    {
                        tick = 2;
                    }
                    break;
                case "TEO":
                    if (Convert.ToDouble(opprice) <= 0.5)
                    {
                        tick = 0.005;
                    }
                    else if ((Convert.ToDouble(opprice) > 0.5) && (Convert.ToDouble(opprice) <= 2.5))
                    {
                        tick = 0.025;
                    }
                    else if ((Convert.ToDouble(opprice) > 2.5) && (Convert.ToDouble(opprice) <= 25))
                    {
                        tick = 0.05;
                    }
                    else if ((Convert.ToDouble(opprice) > 25) && (Convert.ToDouble(opprice) <= 50))
                    {
                        tick = 0.25;
                    }
                    else if (Convert.ToDouble(opprice) > 50)
                    {
                        tick = 0.50;
                    }
                    break;
                case "GTO":
                    if (Convert.ToDouble(opprice) <= 0.5)
                    {
                        tick = 0.005;
                    }
                    else if ((Convert.ToDouble(opprice) > 0.5) && (Convert.ToDouble(opprice) <= 2.5))
                    {
                        tick = 0.025;
                    }
                    else if ((Convert.ToDouble(opprice) > 2.5) && (Convert.ToDouble(opprice) <= 25))
                    {
                        tick = 0.05;
                    }
                    else if ((Convert.ToDouble(opprice) > 25) && (Convert.ToDouble(opprice) <= 50))
                    {
                        tick = 0.25;
                    }
                    else if (Convert.ToDouble(opprice) > 50)
                    {
                        tick = 0.50;
                    }
                    break;
                case "XIO":
                    if (Convert.ToDouble(opprice) <= 20)
                    {
                        tick = 0.2;
                    }
                    else if ((Convert.ToDouble(opprice) > 20) && (Convert.ToDouble(opprice) <= 100))
                    {
                        tick = 1;
                    }
                    else if ((Convert.ToDouble(opprice) > 100) && (Convert.ToDouble(opprice) <= 1000))
                    {
                        tick = 2;
                    }
                    else if ((Convert.ToDouble(opprice) > 1000) && (Convert.ToDouble(opprice) <= 2000))
                    {
                        tick = 10;
                    }
                    else if (Convert.ToDouble(opprice) > 2000)
                    {
                        tick = 2;
                    }
                    break;
                case "TGO":
                    tick = 0.5;
                    break;
                case "MSO":
                    if (Convert.ToDouble(opprice) <= 0.5)
                    {
                        tick = 0.005;
                    }
                    else if ((Convert.ToDouble(opprice) > 0.5) && (Convert.ToDouble(opprice) <= 2.5))
                    {
                        tick = 0.025;
                    }
                    else if ((Convert.ToDouble(opprice) > 2.5) && (Convert.ToDouble(opprice) <= 25))
                    {
                        tick = 0.05;
                    }
                    else if ((Convert.ToDouble(25) > 1000) && (Convert.ToDouble(opprice) <= 50))
                    {
                        tick = 0.25;
                    }
                    else if (Convert.ToDouble(opprice) > 50)
                    {
                        tick = 0.50;
                    }
                    break;
            }
            return (decimal)tick;
        }
        //************************************************************************************************************************************************************************************************************ 
        private void WriteLogFile(String input)
        {
            try
            {
                string TempMillisecond = DateTime.Now.Millisecond.ToString().PadLeft(3, '0');
                string WriteLogFileStr = DateTime.Now.ToString("HH:mm:ss.fff") + " " + input;
                System.Console.WriteLine(WriteLogFileStr);
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }
        //************************************************************************************************************************************************************************************************************ 
        private void c1FlexGrid1_OwnerDrawCell(object sender, C1.Win.C1FlexGrid.OwnerDrawCellEventArgs e)
        {
            if (e.Row == 0 || e.Row == 1)
                return;

            switch (c1FlexGrid1.Cols[e.Col].Name)
            {
                case "Price":
                    string[] _MPx = c1FlexGrid1[e.Row, "Price"].ToString().Split('(');
                    decimal mPx = Convert.ToDecimal(_MPx[0]);

                    if (mPx == BullPx && mPx != MatchPx)
                    {
                        e.Style = c1FlexGrid1.Styles["BullPx"];
                    }
                    else if (mPx == BearPx && mPx != MatchPx)
                    {
                        e.Style = c1FlexGrid1.Styles["BearPx"];
                    }
                    else if (mPx > MatchPx)
                    {
                        e.Style = c1FlexGrid1.Styles["UpMatchPx"];
                    }
                    else if (mPx < MatchPx)
                    {
                        e.Style = c1FlexGrid1.Styles["DownMatchPx"];
                    }
                    else if (mPx == MatchPx)
                    {
                        if (mPx >= SellPx[0])
                            e.Style = c1FlexGrid1.Styles["MatchPx_UpRef"];
                        else if (mPx <= BuyPx[0])
                            e.Style = c1FlexGrid1.Styles["MatchPx_DownRef"];
                        else
                            e.Style = c1FlexGrid1.Styles["MatchPx"];
                    }
                    _MPx = null;
                    break;
            }
        }
        //************************************************************************************************************************************************************************************************************ 
        private void c1FlexGrid1_MouseDown(object sender, MouseEventArgs e)
        {
            FMouseDownRow = c1FlexGrid1.Row;
            FMouseDownColumn = c1FlexGrid1.Col;
            if (e.Button == System.Windows.Forms.MouseButtons.Right)
            {
                if (MatchPx != 0)
                {
                    if (PxToRow.ContainsKey(MatchPx) == true)
                    {
                        C1.Win.C1FlexGrid.Row C1Row = PxToRow[MatchPx];
                        CenterGrid(C1Row, false);
                    }
                }
            }
            else
            {
                HitTestInfo Hi = c1FlexGrid1.HitTest(e.X, e.Y);
                if (Hi.Type == HitTestTypeEnum.Cell)
                {
                    C1.Win.C1FlexGrid.Row targetRow = c1FlexGrid1.Rows[Hi.Row];
                    FMouseDownPrice = RowToPx[targetRow];
                    string DisplayQtyStr;
                    if (Hi.Column == 2) {  //委買
                        DisplayQtyStr = c1FlexGrid1.GetDataDisplay(Hi.Row, 2);
                        FMouseDownSide = OrderMessage.SideEnum.sBuy;
                    } else if (Hi.Column == 6) {  //委賣
                        DisplayQtyStr = c1FlexGrid1.GetDataDisplay(Hi.Row, 6);
                        FMouseDownSide = OrderMessage.SideEnum.sSell;
                    } else {
                        DisplayQtyStr = "";
                        FMouseDownSide = OrderMessage.SideEnum.sNone;
                    }
                    if (DisplayQtyStr.Length > 0) 
                        FMouseDownQty = Convert.ToInt32(DisplayQtyStr);
                    else
                        FMouseDownQty = 0;
//string mouseMessage = string.Format("MouseDown X{0} Y{1} R{2} C{3} Q{4}", e.X, e.Y, FMouseDownRow, FMouseDownColumn, FMouseDownQty);
//lbStatus.Items.Add(mouseMessage);
                }
                else if (Hi.Type == HitTestTypeEnum.ColumnHeader)
                {
                    if( Hi.Row == 1 )
                    {
                        if (Hi.Column == 2 ) //委買全刪
                            Program.FAppMainForm.CancelOrder((OrderMessage.MarketEnum)_Def.Market, _Def.Symbol, OrderMessage.SideEnum.sBuy );
                        else if (Hi.Column == 6) //委賣全刪
                            Program.FAppMainForm.CancelOrder((OrderMessage.MarketEnum)_Def.Market, _Def.Symbol, OrderMessage.SideEnum.sSell );
                    }
                }  //if (Hi.Type == HitTestTypeEnum.Cell)
            }  //if (e.Button == System.Windows.Forms.MouseButtons.Right)
        }  //c1FlexGrid1_MouseDown()
        //-----------------------------------------------------------------------------------------------------
        private void c1FlexGrid1_MouseClick(object sender, MouseEventArgs e) {
            if (e.Button == System.Windows.Forms.MouseButtons.Left) {  //滑鼠左鍵 Click
                HitTestInfo hitInformation = c1FlexGrid1.HitTest(e.X, e.Y);
                if ((hitInformation.Type == HitTestTypeEnum.Cell) &&  //Click Cell
                    ((hitInformation.Row == FMouseDownRow) && (hitInformation.Column == FMouseDownColumn))) { //MouseDown 與 Click 在同一個Cell
                    decimal price = RowToPx[c1FlexGrid1.Rows[hitInformation.Row]];
                    string WriteLogFileStr = "";
                    switch (c1FlexGrid1.Col) {
                        case 2:  //委買
                            Program.FAppMainForm.CancelOrder((OrderMessage.MarketEnum)_Def.Market, _Def.Symbol, OrderMessage.SideEnum.sBuy, (double)price);
                            break;
                        case 3:  //買進
                            WriteLogFileStr = "Buy@" + Convert.ToString(price);
                            Program.FAppMainForm.NewOrder((OrderMessage.MarketEnum)_Def.Market, _Def.Symbol, OrderMessage.SideEnum.sBuy, (double)price,
                                                          (int)nudOrderQty.Value, (OrderMessage.TimeInForceEnum)(cbBuyTimeInForce.SelectedItem));
                            break;
                        case 5:  //賣出
                            WriteLogFileStr = "Sell@" + Convert.ToString(price);
                            Program.FAppMainForm.NewOrder((OrderMessage.MarketEnum)_Def.Market, _Def.Symbol, OrderMessage.SideEnum.sSell, (double)price,
                                                          (int)nudOrderQty.Value, (OrderMessage.TimeInForceEnum)(cbSellTimeInForce.SelectedItem));
                            break;
                        case 6:  //委賣
                            Program.FAppMainForm.CancelOrder((OrderMessage.MarketEnum)_Def.Market, _Def.Symbol, OrderMessage.SideEnum.sSell, (double)price);
                            break;
                    }
                    if (WriteLogFileStr.Length > 0) System.Console.WriteLine(WriteLogFileStr);
                }
            }  //if (e.Button == System.Windows.Forms.MouseButtons.Left)
        }  //c1FlexGrid1_MouseClick()
        //************************************************************************************************************************************************************************************************************
        void Update(String FileldName, System.Collections.ArrayList WorkingPxs, System.Collections.ArrayList Pxs, System.Collections.ArrayList Qtys)
        {
            int TotalQty = 0;

            for (int i = 0; i < WorkingPxs.Count; i++)
            {
                decimal Px = (decimal)WorkingPxs[i];
                if (PxToRow.ContainsKey(Px) == true)
                {
                    C1.Win.C1FlexGrid.Row C1Row = PxToRow[Px];
                    C1Row[FileldName] = "";
                }
            }
            WorkingPxs.Clear();
            for (int i = 0; i < Pxs.Count; i++)
            {
                decimal Px = Convert.ToDecimal(Pxs[i]);

                if (PxToRow.ContainsKey( Px) == true)
                {
                    C1.Win.C1FlexGrid.Row C1Row = PxToRow[ Px ];
                    C1Row[FileldName] = Qtys[i];
                }
                TotalQty += (int)Qtys[i];
                WorkingPxs.Add( Px );
            }
            c1FlexGrid1[1, FileldName] = TotalQty;
        }
        //************************************************************************************************************************************************************************************************************
        public void UpdateWorking( bool IsBuySide, System.Collections.ArrayList Pxs, System.Collections.ArrayList Qtys )
        {
            if (IsBuySide == true)
                Update("BidE5", WorkingBuyPxs, Pxs, Qtys);
            else
                Update("AskE5", WorkingSellPxs, Pxs, Qtys);
        }
        //************************************************************************************************************************************************************************************************************
        public void UpdateFilled(bool IsBuySide, System.Collections.ArrayList Pxs, System.Collections.ArrayList Qtys)
        {
            if (IsBuySide == true)
                Update("BidE4", FilledBuyPxs, Pxs, Qtys);
            else
                Update("AskE4", FilledSellPxs, Pxs, Qtys);
        }
        //-------------------------------------------------------------------------------------------------------------------------
        private void cbCommodity_SelectedIndexChanged(object sender, EventArgs e) {
            if (mdMainForm == null) return;
            mdMainForm.RemoveFlashFormIndex(this.FExchange, this.FSymbol, this);
            mdMainForm.UnsubscribeMarketData(this.FExchange, this.FSymbol);
            PxToRow.Clear();
            RowToPx.Clear();
            WorkingBuyPxs.Clear();
            WorkingSellPxs.Clear();
            FilledBuyPxs.Clear();
            FilledSellPxs.Clear();

            //初始化給值
            for (int i = 0; i < 5; i++) {
                BuyPx[i] = 0;
                SellPx[i] = 0;
            }

            for (int i = c1FlexGrid1.Rows.Count - 1; i > 1; i--) 
                c1FlexGrid1.Rows.Remove(i);
            //顯示上方第二列統計值
            c1FlexGrid1[1, 0] = "0.000";
            c1FlexGrid1[1, 1] = 0;
            c1FlexGrid1[1, 2] = 0;
            c1FlexGrid1[1, 3] = 0;
            c1FlexGrid1[1, 4] = "0:0";
            c1FlexGrid1[1, 5] = 0;
            c1FlexGrid1[1, 6] = 0;
            c1FlexGrid1[1, 7] = 0;
            c1FlexGrid1[1, 8] = "0.000";

            string IndexKey = cbCommodity.SelectedItem.ToString();
            StarWaveAPI.SecurityDefinition CommodityDef = commoditySymbolIndex[IndexKey] as StarWaveAPI.SecurityDefinition;
            _Def = CommodityDef;
            this.FExchange = CommodityDef.Exchange;
            this.FSymbol = CommodityDef.Symbol;
            this.Text = this.FExchange + " " + this.FSymbol;
            mdMainForm.AddFlashFormIndex(this.FExchange, this.FSymbol, this);
            mdMainForm.SubscribeMarketData(CommodityDef.Exchange, CommodityDef.Symbol);
        }  //cbCommodity_SelectedIndexChanged()
        //-------------------------------------------------------------------------------------------------------------------------
        private void btnBuyQty01_Click(object sender, EventArgs e) {
            nudOrderQty.Value = 1;
        }
        //-------------------------------------------------------------------------------------------------------------------------
        private void btnBuyQty02_Click(object sender, EventArgs e) {
            nudOrderQty.Value = 2;
        }
        //-------------------------------------------------------------------------------------------------------------------------
        private void btnBuyQty05_Click(object sender, EventArgs e) {
            nudOrderQty.Value = 5;
        }
        //-------------------------------------------------------------------------------------------------------------------------
        private void btnBuyQty10_Click(object sender, EventArgs e) {
            nudOrderQty.Value = 10;
        }
        //------------------------------------------------------------------------------------------------
        private int CalculateColumnWidth(int columnIndex, Font columnFont, string testStr) {
            int columnWidth = 0;
            if (c1FlexGrid1.Cols[columnIndex].Visible == true) {
                using (Graphics g = c1FlexGrid1.CreateGraphics()) {
                    string columnStr; 
                    if (testStr.Length <= 0) 
                        columnStr = c1FlexGrid1.Cols[columnIndex].Caption;
                    else
                        columnStr = testStr;
                    SizeF columnSize = g.MeasureString(columnStr, columnFont);
                    columnWidth = (int)columnSize.Width;
                }
            }  //if (c1FlexGrid1.Cols[ColumnIndex].Visible == true)
            return 0;
        }  //CalculateColumnWidth()
        //-------------------------------------------------------------------------------------------------------------------------
        private int CalculateTotalColumnWidth(Font columnFont) {
            int totalColumnWidth = 0;
            for (int i = 0; i < c1FlexGrid1.Cols.Count; i++) {
                string testStr = "";
                if (i == 4) testStr = "0000.00(00)";
                totalColumnWidth += CalculateColumnWidth(i, columnFont, testStr);
            }
            return totalColumnWidth;
        }  //CalculateTotalColumnWidth()
        //-------------------------------------------------------------------------------------------------------------------------
        private void Flash10_Resize(object sender, EventArgs e) {
        }
        //-------------------------------------------------------------------------------------------------------------------------
        private void btnFontIncrease_Click(object sender, EventArgs e) {
            float origFontSize = _GridFont.Size;
            _GridFont = new Font("Arial", origFontSize + 1, FontStyle.Regular);
            DefineGridFont(_GridFont);
            InitializeGridStyle();
            AutoSizeColumns(this.c1FlexGrid1);
            if (cbCenter5.Checked && (MatchPx != 0)) {
                if (PxToRow.ContainsKey(MatchPx) == true) {
                    C1.Win.C1FlexGrid.Row C1Row = PxToRow[MatchPx];
                    CenterGrid(C1Row, false);
                }
            }
            if (_GridFont.Size > FMinimalFontSize) btnFontDecrease.Enabled = true;
        }
        //-------------------------------------------------------------------------------------------------------------------------
        private void btnFontDecrease_Click(object sender, EventArgs e) {
            float newFontSize = _GridFont.Size - 1.0F;
            if (newFontSize < FMinimalFontSize) return;
            _GridFont = new Font("Arial", newFontSize, FontStyle.Regular);
            DefineGridFont(_GridFont);
            InitializeGridStyle();
            AutoSizeColumns(this.c1FlexGrid1);
            if (cbCenter5.Checked && (MatchPx != 0)) {
                if (PxToRow.ContainsKey(MatchPx) == true) {
                    C1.Win.C1FlexGrid.Row C1Row = PxToRow[MatchPx];
                    CenterGrid(C1Row, false);
                }
            }
            if (_GridFont.Size <= FMinimalFontSize) btnFontDecrease.Enabled = false;
        }
        //-------------------------------------------------------------------------------------------------------------------------
        private void c1FlexGrid1_DragOver(object sender, DragEventArgs e) {
//            this.Cursor = Cursors.Help;
//string mouseMessage = string.Format("DragOver X{0} Y{1} R{2} C{3} SR{4} SC{5}", e.X, e.Y, c1FlexGrid1.Row, c1FlexGrid1.Col, c1FlexGrid1.RowSel, c1FlexGrid1.ColSel);
//lbStatus.Items.Add(mouseMessage);
        }
        //-------------------------------------------------------------------------------------------------------------------------
        private void c1FlexGrid1_DragDrop(object sender, DragEventArgs e) {
            this.Cursor = Cursors.Default;
            Point dropPt = c1FlexGrid1.PointToClient(new Point(e.X, e.Y));
            HitTestInfo hitInformation = c1FlexGrid1.HitTest(dropPt);
            if ((hitInformation.Type == HitTestTypeEnum.Cell) &&  //Click Cell
                ((FMouseDownColumn == 2) || (FMouseDownColumn == 6)) &&  //委買 or 委賣
                (hitInformation.Column == FMouseDownColumn) &&  //同為委買 or 同為委賣
                (hitInformation.Row != FMouseDownRow)) {                 //不同價格
                decimal newPrice = RowToPx[c1FlexGrid1.Rows[hitInformation.Row]];
//string mouseMessage = string.Format("DragDrop R{0} C{1} OP{2} NP{3}", hitInformation.Row, hitInformation.Column, FMouseDownPrice, newPrice);
//lbStatus.Items.Add(mouseMessage);
                if (FMouseDownSide == OrderMessage.SideEnum.sBuy) {
                    c1FlexGrid1.SetCellStyle(FMouseDownRow, FMouseDownColumn, "BidE5");
                    Program.FAppMainForm.ReplaceOrderPrice((OrderMessage.MarketEnum)_Def.Market, _Def.Symbol, FMouseDownSide, (double)FMouseDownPrice, (double)newPrice);
                }
                else if (FMouseDownSide == OrderMessage.SideEnum.sSell) {
                    c1FlexGrid1.SetCellStyle(FMouseDownRow, FMouseDownColumn, "AskE5");
                    Program.FAppMainForm.ReplaceOrderPrice((OrderMessage.MarketEnum)_Def.Market, _Def.Symbol, FMouseDownSide, (double)FMouseDownPrice, (double)newPrice);
                }
            }
        }  //c1FlexGrid1_DragDrop()
        //-------------------------------------------------------------------------------------------------------------------------
        private void c1FlexGrid1_MouseMove(object sender, MouseEventArgs e) {
            if (e.Button == System.Windows.Forms.MouseButtons.Left) {
                if ((FMouseDownQty <= 0) || ((FMouseDownColumn != 2) && (FMouseDownColumn != 6)))   //非委買、委賣
                    return;
                HitTestInfo hitInformation = c1FlexGrid1.HitTest(e.X, e.Y);
                if ((hitInformation.Type == HitTestTypeEnum.Cell) &&  //Click Cell
                    ((hitInformation.Row != FMouseDownRow) || (hitInformation.Column != FMouseDownColumn))) { //Mouse 移出 MouseDown Cell
//                    this.Cursor = Cursors.WaitCursor;
//                    c1FlexGrid1.SetCellStyle(FMouseDownRow, FMouseDownColumn, "Highlight");
                    string priceStr = string.Format("{0}", FMouseDownPrice);
                    c1FlexGrid1.DoDragDrop(priceStr, DragDropEffects.All);
//string mouseMessage = string.Format("MouseMove {0},{1} {2}", e.X, e.Y, sender.ToString());
//lbStatus.Items.Add(mouseMessage);
                }
            }  //if (e.Button == System.Windows.Forms.MouseButtons.Left)
        }
        //-------------------------------------------------------------------------------------------------------------------------
        private void c1FlexGrid1_MouseLeaveCell(object sender, RowColEventArgs e) {
        }
        //-------------------------------------------------------------------------------------------------------------------------
        private void lbStatus_DragEnter(object sender, DragEventArgs e) {
            if (e.Data.GetDataPresent(DataFormats.UnicodeText, false))
                e.Effect = DragDropEffects.All;
        }
        //-------------------------------------------------------------------------------------------------------------------------
        private void c1FlexGrid1_MouseUp(object sender, MouseEventArgs e) {
            this.Cursor = Cursors.Default;
//string mouseMessage = string.Format("MouseUp X{0} Y{1}", e.X, e.Y);
//lbStatus.Items.Add(mouseMessage);
        }
        //************************************************************************************************************************************************************************************************************ 
    }
}
