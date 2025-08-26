using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Test1
{
    public partial class ContractsForm : Form
    {        
        StarWaveAPI.StarWaveConnection FMDConnection = null;
        StarWaveAPI.ChartServerConnection FCSConnection = null;        
        System.Collections.Hashtable FIndexTable = new System.Collections.Hashtable();
        System.Collections.Hashtable FChartTable = new System.Collections.Hashtable();
        TOrderForm FAppMainForm = null;        
        int TotalSymbols;
        int CurrentSymbol;
        //-----------------------------------------------------------------------------------------
        public ContractsForm(TOrderForm AppForm)
        {
            FAppMainForm = AppForm;
            
            InitializeComponent();
            FMDConnection = new StarWaveAPI.StarWaveConnection();
            FMDConnection.OnConnected += new StarWaveAPI.IStarWaveConnectionEvents_OnConnectedEventHandler(FMDConnection_OnConnected);
            FMDConnection.OnDisconnected += new StarWaveAPI.IStarWaveConnectionEvents_OnDisconnectedEventHandler(FMDConnection_OnDisconnected);
            FMDConnection.OnContractDownloadCompleted += new StarWaveAPI.IStarWaveConnectionEvents_OnContractDownloadCompletedEventHandler(FMDConnection_OnContractDownloadCompleted);
            FMDConnection.OnRecover += new StarWaveAPI.IStarWaveConnectionEvents_OnRecoverEventHandler(FMDConnection_OnRecover);
            FMDConnection.OnOrderBookData += new StarWaveAPI.IStarWaveConnectionEvents_OnOrderBookDataEventHandler(FMDConnection_OnOrderBookData);
            FMDConnection.OnMatchInfo += new StarWaveAPI.IStarWaveConnectionEvents_OnMatchInfoEventHandler(FMDConnection_OnMatchInfo);

            FCSConnection = new StarWaveAPI.ChartServerConnection();
            FCSConnection.OnConnected += new StarWaveAPI.IChartServerConnectionEvents_OnConnectedEventHandler(FCSConnection_OnConnected);
            FCSConnection.OnDisconnected += new StarWaveAPI.IChartServerConnectionEvents_OnDisconnectedEventHandler(FCSConnection_OnDisconnected);
            FCSConnection.OnLastKBar += new StarWaveAPI.IChartServerConnectionEvents_OnLastKBarEventHandler(FCSConnection_OnLastKBar);
            FCSConnection.OnLastTick += new StarWaveAPI.IChartServerConnectionEvents_OnLastTickEventHandler(FCSConnection_OnLastTick);
            FCSConnection.OnRecoverKBars += new StarWaveAPI.IChartServerConnectionEvents_OnRecoverKBarsEventHandler(FCSConnection_OnRecoverKBars);
            FCSConnection.OnRecoverTicks += new StarWaveAPI.IChartServerConnectionEvents_OnRecoverTicksEventHandler(FCSConnection_OnRecoverTicks);
            FCSConnection.OnDayKBars += new StarWaveAPI.IChartServerConnectionEvents_OnDayKBarsEventHandler(FCSConnection_OnDayKBars);
            
            DownloadButton.Enabled = false;
        }
        //-----------------------------------------------------------------------------------------
        public void Connect( )
        {
            DownloadButton.Enabled = false;
            System.Console.WriteLine("Connect to:{0}:{1}", Program.FStarWaveIP, Program.FStarWavePort );
            FMDConnection.Init();
            FCSConnection.Create();
            FMDConnection.Connect( Program.FStarWaveIP, Program.FStarWavePort, 10 );
        }
        //-----------------------------------------------------------------------------------------
        public void Disconnect()
        {
            CommodTreeView.Nodes.Clear();
            FMDConnection.Disconnect();
            FMDConnection.Destroy();
            FCSConnection.Disconnect();
            FCSConnection.Destroy();
            DownloadButton.Enabled = false;
        }
        //-----------------------------------------------------------------------------------------
        void FMDConnection_OnDisconnected()
        {
            CommodTreeView.Nodes.Clear();
            System.Console.WriteLine("FMDConnection_OnDisconnected");
            FAppMainForm.StarWave_OnDisconnected();
        }
        //-----------------------------------------------------------------------------------------
        void AddTWSETree(string Exchange)
        {
            TreeNode ExchangeNode, IndexNode, WarrantNode, ProdNode;
            string ExName;
            if (Exchange == "TWSE")
                ExName = "台灣證交所";
            else
                ExName = "櫃買中心";

            ExchangeNode = CommodTreeView.Nodes.Add(Exchange, ExName);
            IndexNode = ExchangeNode.Nodes.Add("Index", "類股指數");
            WarrantNode = ExchangeNode.Nodes.Add("Warrant", "權證");

            for (int i = 0; i < FMDConnection.SymbolCount(Exchange); i++)
            {
                string Symbol = FMDConnection.GetSymbol(Exchange, i);
                StarWaveAPI.SecurityDefinition Def = new StarWaveAPI.SecurityDefinition();
                FMDConnection.GetSecurityDefinition(Exchange, Symbol, Def);

                if (Def != null)
                {
                    if (!Def.TradeFlag)
                    {
                        IndexNode.Nodes.Add(Def.Symbol, Def.Symbol);
                    }
                    else if (Def.IsWarrant)
                    {
                        WarrantNode.Nodes.Add(Def.Symbol, Def.ChineseName);
                    }
                    else
                    {
                        if (!ExchangeNode.Nodes.ContainsKey(Def.ProductID))
                        {
                            ProdNode = ExchangeNode.Nodes.Add(Def.ProductID, Def.ProductID);
                        }
                        else
                        {
                            TreeNode[] node = ExchangeNode.Nodes.Find(Def.ProductID, false);
                            ProdNode = node[0];
                        }
                        ProdNode.Nodes.Add(Def.Symbol, Def.ChineseName);
                    }
//                    Flash10.AddCommoditySymbolIndex(Def);
                }
                CurrentSymbol++;
                if (CurrentSymbol % 10 == 0)
                {
                    FAppMainForm.StarWave_Progress(TotalSymbols, CurrentSymbol);
                    Application.DoEvents();
                }
            }
        }
        //-----------------------------------------------------------------------------------------
        void AddFuturesTree(string Exchange)
        {
            TreeNode ExNode = CommodTreeView.Nodes.Add(Exchange, Exchange);             
               
            for (int j = 0; j < FMDConnection.SymbolCount(Exchange); j++)
            {
                string Symbol = FMDConnection.GetSymbol(Exchange, j);
                StarWaveAPI.SecurityDefinition Def = new StarWaveAPI.SecurityDefinition();

                FMDConnection.GetSecurityDefinition(Exchange, Symbol, Def);
                if (Def != null)
                {
                    TreeNode MarketNode, ProdNode, MaturityDateNode ,SymbolNode;

                    if ( ExNode.Nodes.ContainsKey(Def.Market.ToString()))
                    {
                        TreeNode[] node = ExNode.Nodes.Find(Def.Market.ToString(), false);
                        MarketNode = node[0];
                    }
                    else
                        MarketNode = ExNode.Nodes.Add(Def.Market.ToString(), Def.Market.ToString());
                    if( MarketNode.Nodes.ContainsKey(Def.ProductID))
                    {
                        TreeNode[] node = MarketNode.Nodes.Find(Def.ProductID, false);
                        ProdNode = node[0];
                    }
                    else
                        ProdNode = MarketNode.Nodes.Add(Def.ProductID, Def.ProductID);
                    if (Def.MaturityDate != null)
                    {
                        if( ProdNode.Nodes.ContainsKey(Def.MaturityDate))
                        {
                            TreeNode[] node = ProdNode.Nodes.Find(Def.MaturityDate, false);
                            MaturityDateNode = node[0];
                        }
                        else
                            MaturityDateNode = ProdNode.Nodes.Add(Def.MaturityDate, Def.MaturityDate);                            
                    }
                    else
                        MaturityDateNode = null;
                    if (MaturityDateNode != null)
                        SymbolNode = MaturityDateNode.Nodes.Add(Def.Symbol, Def.Symbol);
                    else
                        SymbolNode = ProdNode.Nodes.Add(Def.Symbol, Def.Symbol);
                    Flash10.AddCommoditySymbolIndex(Def);
                }
                CurrentSymbol++;
                if( CurrentSymbol%10 == 0 )
                {
                    FAppMainForm.StarWave_Progress( TotalSymbols, CurrentSymbol);
                    Application.DoEvents();
                }
            }
        }  //AddFuturesTree()
        //-----------------------------------------------------------------------------------------
        void FMDConnection_OnContractDownloadCompleted(int ExchangeCount, int UseMS )
        {
            System.Console.WriteLine("FMDConnection_OnContractDownloadCompleted");
            TotalSymbols = 0;
            CurrentSymbol = 0;

            for (int i = 0; i < ExchangeCount; i++)
                TotalSymbols += FMDConnection.SymbolCount(FMDConnection.GetExchange(i));
            for (int i = 0; i < ExchangeCount; i++)
            {
                string Exchange = FMDConnection.GetExchange(i);

                if (Exchange == "TWSE" || Exchange == "OTC" )
                    AddTWSETree(Exchange );
                else
                    AddFuturesTree(Exchange);
            }
            CommodTreeView.Sort();
            CommodTreeView.EndUpdate();
            FAppMainForm.StarWave_OnDownloadOK();
            FCSConnection.Connect(Program.FChartServerIP, Program.FChartServerPort, 10);
            CommodTreeView.Enabled = true;
            DownloadButton.Enabled = true;
        }
        //-----------------------------------------------------------------------------------------
        void FMDConnection_OnConnected()
        {
            DownloadButton.Enabled = true;
            System.Console.WriteLine("FMDConnection_OnConnected");
            DownloadButton.PerformClick();
            FAppMainForm.StarWave_OnConnected();
        }
        //-----------------------------------------------------------------------------------------
        private void DownloadButton_Click(object sender, EventArgs e)
        {
            DownloadButton.Enabled = false;
            CommodTreeView.Enabled = false;
            CommodTreeView.BeginUpdate();
            CommodTreeView.Nodes.Clear();
            FMDConnection.DownloadContract();
        }
        //-----------------------------------------------------------------------------------------
        private TreeNode GetRootNode( TreeNode ChildNode )
        {
            TreeNode ParentNode = null;
            TreeNode TmpNode = ChildNode.Parent;

            while (TmpNode != null)
            {
                ParentNode = TmpNode;
                TmpNode = ParentNode.Parent;
            }
            return ParentNode;
        }
        //-----------------------------------------------------------------------------------------
        public void AddFlashFormIndex(string flashFormKey, Flash10 flashForm) 
        {
            System.Collections.ArrayList flashFormList = null;
            if (FIndexTable.ContainsKey(flashFormKey) == false) {
                flashFormList = new System.Collections.ArrayList();
                FIndexTable[flashFormKey] = flashFormList;
            }
            else
                flashFormList = (System.Collections.ArrayList)FIndexTable[flashFormKey];
            if (flashFormList != null) flashFormList.Add(flashForm);
        }  //AddFlashFormIndex()
        //-----------------------------------------------------------------------------------------
        public void AddFlashFormIndex(string exchange, string symbol, Flash10 flashForm) {
            string flashFormKey = string.Format("{0}.{1}", exchange, symbol);
            AddFlashFormIndex(flashFormKey, flashForm);
        }  //AddFlashFormIndex()
        //-----------------------------------------------------------------------------------------
        public void RemoveFlashFormIndex(string flashFormKey, Flash10 flashForm) {
            System.Collections.ArrayList flashFormList = (System.Collections.ArrayList)FIndexTable[flashFormKey];
            if (flashFormList != null) {
                flashFormList.Remove(flashForm);
                if (flashFormList.Count == 0) {
                    FIndexTable.Remove(flashFormKey);
                    flashFormList = null;
                }
            }  //if (flashFormList != null)
        }  //RemoveFlashFormIndex()
        //-----------------------------------------------------------------------------------------
        public void RemoveFlashFormIndex(string exchange, string symbol, Flash10 flashForm) {
            string flashFormKey = string.Format("{0}.{1}", exchange, symbol);
            RemoveFlashFormIndex(flashFormKey, flashForm);
        }  //RemoveFlashFormIndex()
        //-----------------------------------------------------------------------------------------
        public void SubscribeMarketData(string exchange, string symbol) {
            FMDConnection.Subscribe(exchange, symbol);
        }  //SubscribeMarketData()
        //-----------------------------------------------------------------------------------------
        public void UnsubscribeMarketData(string exchange, string symbol) {
            FMDConnection.Unsubscribe(exchange, symbol);
        }  //UnsubscribeMarketData()
        //-----------------------------------------------------------------------------------------
        private void CommodTreeView_DoubleClick(object sender, EventArgs e)
        {
            TreeNode SelectNode = CommodTreeView.SelectedNode;
            
            if (SelectNode != null && SelectNode.Nodes.Count == 0)
            {
                TreeNode ParentNode = GetRootNode(SelectNode);
                StarWaveAPI.SecurityDefinition Def = new StarWaveAPI.SecurityDefinition();

                if (FMDConnection.GetSecurityDefinition(ParentNode.Name, SelectNode.Name, Def) == true)
                {
                    string TableKey = string.Format("{0}.{1}", Def.Exchange, Def.Symbol);
                    Flash10 NewForm = new Flash10(Def);
                    NewForm.MdMainForm = this;
                    AddFlashFormIndex(TableKey, NewForm);
                    NewForm.FormClosed += new FormClosedEventHandler(MarketDataForm_FormClosed);
                    NewForm.Show();
                    SubscribeMarketData(Def.Exchange, Def.Symbol);
                }
                Def = null;
            }
        }
        //-----------------------------------------------------------------------------------------
        System.Collections.ArrayList GetUpdateList(String Exchange, String Symbol)
        {
            string TableKey = string.Format("{0}.{1}", Exchange, Symbol);
            return (System.Collections.ArrayList)FIndexTable[TableKey];
        }
        //-----------------------------------------------------------------------------------------
        void MarketDataForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            Flash10 DelForm = (Flash10)sender;
            if (DelForm != null)
            {
                System.Collections.ArrayList list = GetUpdateList(DelForm.Exchange, DelForm.Symbol);
                if (list != null)
                {
                    list.Remove(DelForm);
                    if (list.Count == 0)
                    {
                        string TableKey = string.Format("{0}.{1}", DelForm.Exchange, DelForm.Symbol);

                        FIndexTable.Remove(TableKey);
                        FMDConnection.Unsubscribe(DelForm.Exchange, DelForm.Symbol);
                        list = null;
                    }
                }
                DelForm = null;
            }
        }        
        //-----------------------------------------------------------------------------------------
        void FMDConnection_OnRecover(StarWaveAPI.Recover Recover)
        {
            System.Collections.ArrayList list = GetUpdateList(Recover.Exchange, Recover.Symbol);
            if (list != null)
            {
                System.Collections.ArrayList BuyPxs = new System.Collections.ArrayList();
                System.Collections.ArrayList BuyQtys = new System.Collections.ArrayList();
                System.Collections.ArrayList SellPxs = new System.Collections.ArrayList();
                System.Collections.ArrayList SellQtys = new System.Collections.ArrayList();
                System.Collections.ArrayList BuyFilledPxs = new System.Collections.ArrayList();
                System.Collections.ArrayList BuyFilledQtys = new System.Collections.ArrayList();
                System.Collections.ArrayList SellFilledPxs = new System.Collections.ArrayList();
                System.Collections.ArrayList SellFilledQtys = new System.Collections.ArrayList();

                FAppMainForm.FStore.GetWorking(Recover.Exchange, Recover.Symbol, true, BuyPxs, BuyQtys);
                FAppMainForm.FStore.GetWorking(Recover.Exchange, Recover.Symbol, false, SellPxs, SellQtys);
                FAppMainForm.FStore.GetFilled(Recover.Exchange, Recover.Symbol, true, BuyFilledPxs, BuyFilledQtys);
                FAppMainForm.FStore.GetFilled(Recover.Exchange, Recover.Symbol, false, SellFilledPxs, SellFilledQtys);                        
                for (int i = 0; i < list.Count; i++)
                {
                    Flash10 UpdateForm = (Flash10)list[i];
                    if (UpdateForm != null)
                    {
                        UpdateForm.OnRecover(Recover);
                        UpdateForm.UpdateWorking( true, BuyPxs, BuyQtys);
                        UpdateForm.UpdateWorking( false, SellPxs, SellQtys);
                        UpdateForm.UpdateFilled( true, BuyFilledPxs, BuyFilledQtys);
                        UpdateForm.UpdateFilled( false, SellFilledPxs, SellFilledQtys);
                    }                    
                }
            }
        }
        //-----------------------------------------------------------------------------------------
        void FMDConnection_OnOrderBookData(StarWaveAPI.OrderBook OrderBookData)
        {
            System.Collections.ArrayList list = GetUpdateList(OrderBookData.Exchange, OrderBookData.Symbol);
            if (list != null)
            {
                for (int i = 0; i < list.Count; i++)
                {
                    Flash10 UpdateForm = (Flash10)list[i];
                    if (UpdateForm != null)                    
                        UpdateForm.OnOrderBookData(OrderBookData);
                }
            }
        }
        //-----------------------------------------------------------------------------------------
        void FMDConnection_OnMatchInfo(StarWaveAPI.Match MatchInfo)
        {
            System.Collections.ArrayList list = GetUpdateList(MatchInfo.Exchange, MatchInfo.Symbol);
            if (list != null)
            {
                for (int i = 0; i < list.Count; i++)
                {
                    Flash10 UpdateForm = (Flash10)list[i];
                    if (UpdateForm != null)
                        UpdateForm.OnMatchInfo(MatchInfo);
                }
            }
        }
        //-----------------------------------------------------------------------------------------
        public void FMDConnection_OnOrderStoreWorkingUpdate( object Sender, String Ex, String Sym, bool IsBuy)
        {
            System.Collections.ArrayList list = GetUpdateList( Ex, Sym);
            if (list != null && list.Count > 0 )
            {
                System.Collections.ArrayList Pxs = new System.Collections.ArrayList();
                System.Collections.ArrayList Qtys = new System.Collections.ArrayList();

                for (int i = 0; i < list.Count; i++)
                {
                    Flash10 UpdateForm = (Flash10)list[i];
                    if (UpdateForm != null)
                    {
                        ((OrderStore)Sender).GetWorking(Ex, Sym, IsBuy, Pxs, Qtys);
                        UpdateForm.UpdateWorking( IsBuy, Pxs, Qtys);
                    }
                }
            }
        }  //FMDConnection_OnOrderStoreWorkingUpdate()
        //-----------------------------------------------------------------------------------------
        public void FMDConnection_OnOrderStoreFilledUpdate(object Sender, String Ex, String Sym, bool IsBuy)
        {
            System.Collections.ArrayList list = GetUpdateList(Ex, Sym);
            if (list != null && list.Count > 0)
            {
                System.Collections.ArrayList Pxs = new System.Collections.ArrayList();
                System.Collections.ArrayList Qtys = new System.Collections.ArrayList();

                for (int i = 0; i < list.Count; i++)
                {
                    Flash10 UpdateForm = (Flash10)list[i];
                    if (UpdateForm != null)
                    {
                        ((OrderStore)Sender).GetFilled(Ex, Sym, IsBuy, Pxs, Qtys);
                        UpdateForm.UpdateFilled(IsBuy, Pxs, Qtys);
                        ((OrderStore)Sender).GetWorking(Ex, Sym, IsBuy, Pxs, Qtys);
                        UpdateForm.UpdateWorking(IsBuy, Pxs, Qtys);
                    }
                }  //for (int i = 0; i < list.Count; i++)
            }
        }  //FMDConnection_OnOrderStoreFilledUpdate()
        //-----------------------------------------------------------------------------------------
        void FCSConnection_OnDayKBars(string Exchange, string Symbol, string Date, StarWaveAPI.MinuteKInfoList KBars)
        {
            System.Console.WriteLine("KBars Ex:{0} Sym:{1} Date:{2}", Exchange, Symbol, Date);
        }
        //-----------------------------------------------------------------------------------------
        void FCSConnection_OnLastTick(String Ex, String Sym, StarWaveAPI.TickInfo Tick)
        {
            System.Console.WriteLine("Tick Ex:{0} Sym:{1} Px:{2}", Ex, Sym, Tick.FillPx);
        }
        //-----------------------------------------------------------------------------------------
        void FCSConnection_OnRecoverTicks(String Ex, String Sym, StarWaveAPI.TickInfoList Ticks)
        {
            StarWaveAPI.TickInfo Info;
            for (int i = 0; i < Ticks.Count; i++)
            {
                Info = Ticks.Items[i];
                System.Console.WriteLine("Tick {0} Px[{1}]", Info.Time, Info.FillPx);
            }
        }
        //-----------------------------------------------------------------------------------------
        void FCSConnection_OnRecoverKBars(String Ex, String Sym, StarWaveAPI.MinuteKInfoList KBars)
        {
            ChartForm UpdateChart = GetChart(Ex, Sym);
            if (UpdateChart != null)
            {
                StarWaveAPI.MinuteKInfo Info;

                UpdateChart.BeginUpdate();
                for (int i = 0; i < KBars.Count; i++)
                {
                    Info = KBars.Items[i];                    
                    UpdateChart.Add(Info);
                }
                UpdateChart.EndUpdate();
            }
        }
        //-----------------------------------------------------------------------------------------
        void FCSConnection_OnLastKBar(String Ex, String Sym, StarWaveAPI.MinuteKInfo KBar)
        {
            ChartForm UpdateChart = GetChart(Ex, Sym);
            if( UpdateChart != null )
                UpdateChart.Update(KBar);
        }
        //-----------------------------------------------------------------------------------------
        void FCSConnection_OnDisconnected()
        {
            System.Console.WriteLine("ChartServerConnection OnDisconnected");
        }
        //-----------------------------------------------------------------------------------------
        void FCSConnection_OnConnected()
        {
            System.Console.WriteLine("ChartServerConnection OnConnected");
        }
        //-----------------------------------------------------------------------------------------
        ChartForm GetChart(String Exchange, String Symbol)
        {
            string TableKey = string.Format("{0}.{1}", Exchange, Symbol);
            return (ChartForm)FChartTable[ TableKey ];
        }
        //-----------------------------------------------------------------------------------------
        ChartForm AddChart(String Exchange, String Symbol)
        {
            string TableKey = string.Format("{0}.{1}", Exchange, Symbol);
            if (FChartTable.ContainsKey(TableKey) == false)
            {
                ChartForm NewChart;
                StarWaveAPI.SecurityDefinition Def = new StarWaveAPI.SecurityDefinition();

                if (FMDConnection.GetSecurityDefinition(Exchange, Symbol, Def))
                {
                    NewChart = new ChartForm(Exchange, Symbol, Def.Digit );
                    NewChart.Text = string.Format("{0},{1}", Exchange, Symbol);
                    FCSConnection.Subscribe( Def );                    
                    FChartTable[TableKey] = NewChart;
                    Def = null;
                    return NewChart;
                }
                else
                    return null;
                
            }
            else
                return (ChartForm)FChartTable[TableKey];
        }                    
        //-----------------------------------------------------------------------------------------
        private void candlestickToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TreeNode SelectNode = CommodTreeView.SelectedNode;

            if (SelectNode != null && SelectNode.Nodes.Count == 0)
            {
                TreeNode ParentNode = GetRootNode(SelectNode);
                
                ChartForm NewChart = AddChart( ParentNode.Name, SelectNode.Name );
                NewChart.FormClosed += new FormClosedEventHandler(NewChart_FormClosed);
                NewChart.Show();
            }
        }
        //-----------------------------------------------------------------------------------------
        void NewChart_FormClosed(object sender, FormClosedEventArgs e)
        {
            ChartForm CloseChart = (ChartForm)sender;
            if (CloseChart != null)
            {
                string TableKey = string.Format("{0}.{1}", CloseChart.Exchange, CloseChart.Symbol);
                FChartTable.Remove(TableKey);
                StarWaveAPI.SecurityDefinition Def = new StarWaveAPI.SecurityDefinition();

                if (FMDConnection.GetSecurityDefinition(CloseChart.Exchange, CloseChart.Symbol, Def))
                {
                    FCSConnection.Unsubscribe(Def);
                    Def = null;
                }
            }            
        }
        //-----------------------------------------------------------------------------------------
    }
}
