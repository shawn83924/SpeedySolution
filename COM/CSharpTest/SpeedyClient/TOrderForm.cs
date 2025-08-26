using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Threading;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.Text;
using System.Xml;
using System.Xml.Xsl;
using System.Windows.Forms;

namespace Test1
{
    public partial class TOrderForm : Form    
    {
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        [DllImport("Kernel32.dll")]
        private static extern bool QueryPerformanceCounter(  out long lpPerformanceCount );

        [DllImport("Kernel32.dll")]
        private static extern bool QueryPerformanceFrequency(  out long lpFrequency );
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        OrderConnection.TaifexConnection FOrderConnection = null;
        System.Collections.Hashtable     FNIDHash = null;
        System.Collections.Hashtable     FOrderIDHash = null;
        System.Collections.Hashtable     FAccountSet = null;
        System.Collections.Hashtable     FNIDSet = null;
        ContractsForm                    FMDMainForm = null;
        LogonForm                        FLogon = null;
        Object                           LockObject = null;
        int                              FRecoverCount;
        long                             FFreq;
        public OrderStore FStore = null; 
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public TOrderForm( )
        {
            InitializeComponent();
            InitializeGrid();
            InitializeComboBox();
            
            FNIDHash         = new System.Collections.Hashtable();
            FOrderIDHash     = new System.Collections.Hashtable();
            FAccountSet      = new System.Collections.Hashtable();
            FNIDSet          = new System.Collections.Hashtable();
            FOrderConnection = new OrderConnection.TaifexConnection();
            FLogon           = new LogonForm( FOrderConnection ) ;
            FMDMainForm      = new ContractsForm(this); 
            LockObject       = new Object();
            FStore           = new OrderStore();
            ReadSetting();
            // Event handler for MBus
            FOrderConnection.OnConnected    += new OrderConnection.ITaifexConnectionEvents_OnConnectedEventHandler(FOrderConnection_OnConnected);
            FOrderConnection.OnDisconnected += new OrderConnection.ITaifexConnectionEvents_OnDisconnectedEventHandler(FOrderConnection_OnDisconnected);   
            // Event Handler for SpeedyGateway
            FOrderConnection.OnLogonReply      += new OrderConnection.ITaifexConnectionEvents_OnLogonReplyEventHandler(FOrderConnection_OnLogon);
            FOrderConnection.OnExecutionReport += new OrderConnection.ITaifexConnectionEvents_OnExecutionReportEventHandler(FOrderConnection_OnExecutionReport);
            FOrderConnection.OnRecoverFinished += new OrderConnection.ITaifexConnectionEvents_OnRecoverFinishedEventHandler(FOrderConnection_OnRecoverFinished);
            FOrderConnection.OnNews += new OrderConnection.ITaifexConnectionEvents_OnNewsEventHandler(FOrderConnection_OnNews);
            FOrderConnection.Create2( "OrderClient@" + System.Net.Dns.GetHostName());
            FOrderConnection.SetBrokerID( OrderMessage.MarketEnum.mFutures, Program.FBrokerID );
            FOrderConnection.SetBrokerID( OrderMessage.MarketEnum.mTSE, Program.FTSEBrokerID );
            FOrderConnection.ClearMemberID = Program.FCMID;
            FOrderConnection.SetLanguage( OrderConnection.MessageLanguageEnum.mlChinese );
            FOrderConnection.UseNewFuturesSymbol = true;
            FStore.OnWorkingChange += new OrderStore.Update(FMDMainForm.FMDConnection_OnOrderStoreWorkingUpdate);
            FStore.OnFilledChange += new OrderStore.Update(FMDMainForm.FMDConnection_OnOrderStoreFilledUpdate);  
            QueryPerformanceFrequency( out FFreq ); 
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void FOrderConnection_OnNews(OrderMessage.NewsMessage News)
        {
            toolStripStatusLabel1.Text = News.Headline;
            toolStripStatusLabel3.Text = News.Text; 
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void ReadSetting()
        {
            Microsoft.Win32.RegistryKey SoftwareReg = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("SOFTWARE",true);           
            Microsoft.Win32.RegistryKey MDBSReg = SoftwareReg.CreateSubKey("MDBS");
            Microsoft.Win32.RegistryKey AppReg = MDBSReg.CreateSubKey("SpeedyOrderClient");

            Program.FSpeedyIP   = AppReg.GetValue("IP", "192.168.0.22").ToString();
            Program.FSpeedyPort = Convert.ToInt32( AppReg.GetValue("Port", "23456").ToString() );
            Program.FStarWaveIP = AppReg.GetValue("MDIP", "10.6.84.200").ToString();
            Program.FStarWavePort = Convert.ToInt32(AppReg.GetValue("MDPort", "34567").ToString());
            Program.FChartServerIP = AppReg.GetValue("CSIP", "10.6.84.200").ToString();
            Program.FChartServerPort = Convert.ToInt32(AppReg.GetValue("CSPort", "34569").ToString());
            FLogon.FId.Text = AppReg.GetValue("ID", "frank").ToString();
            FLogon.FPass.Text = AppReg.GetValue("Password", "frank").ToString();
            FLogon.FAccount.Text = AppReg.GetValue("Account", "0000001").ToString();
            Program.FBrokerID = AppReg.GetValue("BrokerID", "F006000").ToString();
            Program.FTSEBrokerID = AppReg.GetValue("TSEBrokerID", "7000").ToString();
            Program.FCMID = AppReg.GetValue( "CMID", "F006").ToString();
            Program.FMMAccount = AppReg.GetValue("MMAccount", "8888888").ToString();
            Program.FNewVer = AppReg.GetValue( "NewVer", "false").ToString();
            Program.FNewOrder = AppReg.GetValue("NewOrder", "false").ToString();

            Program.FUTSymbol = AppReg.GetValue("FutSym", "XIFI8" ).ToString();
            Program.OPTSymbol = AppReg.GetValue("OptSym", "TXO06500I9").ToString();
            Program.TSESymbol = AppReg.GetValue("TseSym", "2357").ToString();
            Program.OTCSymbol = AppReg.GetValue("OtcSym", "6205").ToString();
            Program.FUTPx = AppReg.GetValue("FutPx", "255").ToString();
            Program.OPTPx = AppReg.GetValue("OptPx", "6400.0").ToString();
            Program.TSEPx = AppReg.GetValue("TsePx", "89.50").ToString();
            Program.OTCPx = AppReg.GetValue("OtcPx", "61.00").ToString();
            Program.FReadOnly = AppReg.GetValue("ReadOnly", "false").ToString();
            Program.UserData = AppReg.GetValue("UserData", "000011").ToString();
            if (Program.FReadOnly == "true")
                Program.FNewOrder = "false";
            int ColCount = FGrid.Cols.Count;
            
            for (int i = 0; i < ColCount; i++)
            {
                  string  IndexName = "IndexCol" + Convert.ToString( i);
                  Object ValueObj   = AppReg.GetValue( IndexName );
                  if ( ValueObj != null )
                      FGrid.Cols[ ValueObj.ToString() ].Move(i);
            }
            for (int i = 0; i < ColCount; i++)
            {
                string VisableName = "VisibleCol" + Convert.ToString(i);
                Object ValueObj = AppReg.GetValue(VisableName);
                if (ValueObj != null)
                    FGrid.Cols[i].Visible = Convert.ToBoolean(ValueObj);
            }
            AppReg.Close();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void WriteSetting()
        {
            Microsoft.Win32.RegistryKey SoftwareReg = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("SOFTWARE",true);
            Microsoft.Win32.RegistryKey MDBSReg = SoftwareReg.CreateSubKey("MDBS");
            Microsoft.Win32.RegistryKey AppReg = MDBSReg.CreateSubKey("SpeedyOrderClient");

            AppReg.SetValue("IP", Program.FSpeedyIP );
            AppReg.SetValue("Port", Convert.ToString( Program.FSpeedyPort ) );
            AppReg.SetValue("MDIP", Program.FStarWaveIP);
            AppReg.SetValue("MDPort", Convert.ToString(Program.FStarWavePort));
            AppReg.SetValue("CSIP", Program.FChartServerIP);
            AppReg.SetValue("CSPort", Convert.ToString(Program.FChartServerPort));
            AppReg.SetValue("ID", FLogon.FId.Text);
            AppReg.SetValue("Password", FLogon.FPass.Text );
            AppReg.SetValue("Account", FLogon.FAccount.Text );
            AppReg.SetValue("BrokerID", Program.FBrokerID );
            AppReg.SetValue("TSEBrokerID", Program.FTSEBrokerID);
            AppReg.SetValue("CMID", Program.FCMID );
            AppReg.SetValue("MMAccount", Program.FMMAccount );
            AppReg.SetValue("NewVer", Program.FNewVer );
            AppReg.SetValue("NewOrder", Program.FNewOrder);

            AppReg.SetValue("FutSym", Program.FUTSymbol );
            AppReg.SetValue("OptSym", Program.OPTSymbol );
            AppReg.SetValue("TseSym", Program.TSESymbol );
            AppReg.SetValue("OtcSym", Program.OTCSymbol );
            AppReg.SetValue("FutPx", Program.FUTPx );
            AppReg.SetValue("OptPx", Program.OPTPx );
            AppReg.SetValue("TsePx", Program.TSEPx );
            AppReg.SetValue("OtcPx", Program.OTCPx );
            AppReg.SetValue("ReadOnly", Program.FReadOnly);
            AppReg.SetValue("UserData", Program.UserData);
            int ColCount = FGrid.Cols.Count;
            for (int i = 0; i < ColCount; i++)
            {
                string VisableName = "VisibleCol" + Convert.ToString( i );
                string IndexName = "IndexCol" + Convert.ToString( FGrid.Cols[i].Index );
                AppReg.SetValue( VisableName, FGrid.Cols[i].Visible );
                AppReg.SetValue(IndexName, FGrid.Cols[i].Name );
            }
            AppReg.Close();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void  FOrderConnection_OnLogon(string ReplyMessage, OrderConnection.LogonResultEnum Accept, int ConnectionID )
        {
            toolStripStatusLabel1.Text = ReplyMessage;
            toolStripStatusLabel3.Text = "ConnectionID:" + Convert.ToString(ConnectionID);
            
            if (Accept == OrderConnection.LogonResultEnum.lrOK )
            {
                Program.FIsAccept = true;
                Program.FConnectionID = ConnectionID;
                this.Orderpanel.Enabled = true;
                this.panelFilter.Enabled = true;
                this.Disconnectbutton.Enabled = true;
                this.FRecover.Enabled = true;
                this.Connectbutton.Enabled = false;
                if (this.FOrderConnection.IsAdministrator == true)
                    this.Text = "Admin:" + Program.FLogonID;
                else
                    this.Text = "User:" + Program.FLogonID;                
                Clear();                
                FRecoverCount = 0;
                FMDMainForm.Connect();
            }
            else
            {
                Program.FIsAccept = false;
                System.Windows.Forms.MessageBox.Show( ReplyMessage );               
            }            
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void  FOrderConnection_OnDisconnected()
        {
            this.Orderpanel.Enabled = false;
            this.panelFilter.Enabled = false;
            this.Disconnectbutton.Enabled = false;
            this.FRecover.Enabled = false;
            this.Connectbutton.Enabled = true;
            Clear();
            toolStripStatusLabel1.Text = "Connection failed!";
            FOrderConnection.Disconnect();
            FMDMainForm.Disconnect();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void  FOrderConnection_OnConnected()
        {            
            FLogon.ShowDialog();
            WriteSetting();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void FOrderConnection_OnRecoverFinished(int Count)
        {
            Console.WriteLine("Recover " + Convert.ToSingle(Count) + " execution finished.");
            toolStripStatusLabel1.Text = "Recover " + Convert.ToString(FRecoverCount) + " Executions";
            FGrid.Visible = true;
            FRecover.Enabled = true;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public void StarWave_OnConnected()
        {
            toolStripProgressBar.Visible = true;
            toolStripStatusLabel1.Text = "Download contracts...";
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public void StarWave_OnDisconnected()
        {
            toolStripStatusLabel1.Text = "Connect to StarWave failed.";
            FOrderConnection_OnDisconnected();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public void StarWave_OnDownloadOK()
        {
            toolStripStatusLabel1.Text = "StarWave Connected.";
            toolStripProgressBar.Value = 0;
            toolStripProgressBar.Visible = false;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public void StarWave_Progress( int Total, int Pos )
        {
            if (toolStripProgressBar.Visible == false)
                toolStripProgressBar.Visible = true;
            toolStripProgressBar.Maximum = Total;
            toolStripProgressBar.Value = Pos;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void InitializeGrid()
        {
            int ColNo = 0;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "AE";
            FGrid.Cols[ColNo].Name = "AE";
            FGrid.Cols[ColNo].Width = 70;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "OrderStatus";
            FGrid.Cols[ColNo].Name = "OrderStatus";
            FGrid.Cols[ColNo].Width = 130;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "Symbol";
            FGrid.Cols[ColNo].Name = "Symbol";
            FGrid.Cols[ColNo].Width = 100;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "Data";
            FGrid.Cols[ColNo].Name = "Data";
            FGrid.Cols[ColNo].Width = 100;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = true;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "OrderID";
            FGrid.Cols[ColNo].Name = "OrderID";
            FGrid.Cols[ColNo].Width = 70;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "NID";
            FGrid.Cols[ColNo].Name = "NID";
            FGrid.Cols[ColNo].Width = 80;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;


            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "Side";
            FGrid.Cols[ColNo].Name = "Side";
            FGrid.Cols[ColNo].Width = 60;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "Price";
            FGrid.Cols[ColNo].Name = "Price";
            FGrid.Cols[ColNo].Width = 100;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "LastPrice";
            FGrid.Cols[ColNo].Name = "LastPrice";
            FGrid.Cols[ColNo].Width = 100;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;


            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "Qty";
            FGrid.Cols[ColNo].Name = "Qty";
            FGrid.Cols[ColNo].Width = 40;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "CumQty";
            FGrid.Cols[ColNo].Name = "CumQty";
            FGrid.Cols[ColNo].Width = 70;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "Time";
            FGrid.Cols[ColNo].Name = "Time";
            FGrid.Cols[ColNo].Width = 100;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "UseTime";
            FGrid.Cols[ColNo].Name = "UseTime";
            FGrid.Cols[ColNo].Width = 70;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "Text";
            FGrid.Cols[ColNo].Name = "Text";
            FGrid.Cols[ColNo].Width = 150;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "BidPrice";
            FGrid.Cols[ColNo].Name = "BidPrice";
            FGrid.Cols[ColNo].Width = 100;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "BidQty";
            FGrid.Cols[ColNo].Name = "BidQty";
            FGrid.Cols[ColNo].Width = 60;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;            
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "BidCumQty";
            FGrid.Cols[ColNo].Name = "BidCumQty";
            FGrid.Cols[ColNo].Width = 90;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "AskPrice";
            FGrid.Cols[ColNo].Name = "AskPrice";
            FGrid.Cols[ColNo].Width = 100;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "AskQty";
            FGrid.Cols[ColNo].Name = "AskQty";
            FGrid.Cols[ColNo].Width = 60;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "AskCumQty";
            FGrid.Cols[ColNo].Name = "AskCumQty";
            FGrid.Cols[ColNo].Width = 90;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "Account";
            FGrid.Cols[ColNo].Name = "Account";
            FGrid.Cols[ColNo].Width = 70;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "Market";
            FGrid.Cols[ColNo].Name = "Market";
            FGrid.Cols[ColNo].Width = 80;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "OrderType";
            FGrid.Cols[ColNo].Name = "OrderType";
            FGrid.Cols[ColNo].Width = 90;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "MaturityMonthYear";
            FGrid.Cols[ColNo].Name = "MaturityMonthYear";
            FGrid.Cols[ColNo].Width = 170;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "StrikePrice";
            FGrid.Cols[ColNo].Name = "StrikePrice";
            FGrid.Cols[ColNo].Width = 110;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.RightCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "TimeInForce";
            FGrid.Cols[ColNo].Name = "TimeInForce";
            FGrid.Cols[ColNo].Width = 110;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "TradingSession";
            FGrid.Cols[ColNo].Name = "TradingSession";
            FGrid.Cols[ColNo].Width = 110;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "BrokerID";
            FGrid.Cols[ColNo].Name = "BrokerID";
            FGrid.Cols[ColNo].Width = 60;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Add();
            FGrid.Cols[ColNo].Caption = "IsOrder";
            FGrid.Cols[ColNo].Name = "IsOrder";
            FGrid.Cols[ColNo].Width = 30;
            FGrid.Cols[ColNo].TextAlign = C1.Win.C1FlexGrid.TextAlignEnum.LeftCenter;
            FGrid.Cols[ColNo].TextAlignFixed = C1.Win.C1FlexGrid.TextAlignEnum.CenterCenter;
            FGrid.Cols[ColNo].Visible = false;
            FGrid.Cols[ColNo].AllowEditing = false;
            ColNo++;

            FGrid.Cols.Count = ColNo;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void InitializeComboBox()
        {
            FOrderType.Items.Clear();
            FOrderType.Items.Add(OrderMessage.OrderTypeEnum.otMarket);
            FOrderType.Items.Add(OrderMessage.OrderTypeEnum.otLimit);

            FTimeInForce.Items.Clear();
            FTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifNone);
            FTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifROD);
            FTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifIOC);
            FTimeInForce.Items.Add(OrderMessage.TimeInForceEnum.tifFOK);
            
            PEcomboBox.Items.Clear();
            PEcomboBox.Items.Add(OrderMessage.PositionEffectEnum.peOpen);
            PEcomboBox.Items.Add(OrderMessage.PositionEffectEnum.peClose);
            PEcomboBox.Items.Add(OrderMessage.PositionEffectEnum.peRolled);
            PEcomboBox.Items.Add(OrderMessage.PositionEffectEnum.peDayTradeOpen);
            PEcomboBox.Items.Add(OrderMessage.PositionEffectEnum.peAuto);

            PEcomboBox.SelectedIndex = 0;
            BSComboBox.SelectedIndex = 0;
            AccountComboBox.SelectedIndex = 0;
            MarketComboBox.SelectedIndex = 0;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void FOrderConnection_OnExecutionReport( OrderMessage.ExecutionReportMessage Msg, OrderConnection.ExecDupEnum PossDup )
        {
            try
            {
                if (PossDup == OrderConnection.ExecDupEnum.edPossibleDuplicate )
                {
                    FRecoverCount++;
                    if (FRecoverCount % 50 == 0)
                       toolStripStatusLabel1.Text = "Recover "+ Convert.ToString( FRecoverCount ) + " Executions";                   
                }
                else 
                {
                    if (Msg.ConnectionID != Program.FConnectionID)
                    {
                        Console.WriteLine("ConnectionID = " + Msg.ConnectionID);
                        Console.WriteLine("Message:" + Msg.src);
                    }
                }
                if (Msg.ExecType == OrderMessage.ExecTypeEnum.etOrderStatus)
                {
                    OnOrderStatus(Msg);
                    return;
                }

                switch (Msg.OrderStatus)
                {
                    case OrderMessage.OrderStatusEnum.osPendingNew: OnPendingNew(Msg); break;
                    case OrderMessage.OrderStatusEnum.osPendingCancel: OnPendingCancel(Msg); break;
                    case OrderMessage.OrderStatusEnum.osPendingReplace: OnPendingReplace(Msg); break;
                    case OrderMessage.OrderStatusEnum.osNew: OnNewOrder(Msg); break;///< Order confirm from Exchange.                             
                    case OrderMessage.OrderStatusEnum.osQuoteAccept: break;///< Quote confirm from Exchange.                             
                    case OrderMessage.OrderStatusEnum.osFilled:
                    case OrderMessage.OrderStatusEnum.osPartiallyFilled: OnPartiallyFilled(Msg); break;
                    case OrderMessage.OrderStatusEnum.osReplaced: OnReplaced(Msg); break;
                    case OrderMessage.OrderStatusEnum.osCanceled: OnCanceled(Msg); break;
                    case OrderMessage.OrderStatusEnum.osRejected: OnRejected(Msg); break;
                    ///< case OrderMessage.OrderStatusEnum.osExpired:
                }
            }
            catch( System.Exception e )
            {
                Console.WriteLine( e.ToString() );
            }          
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void TOrderForm_Load(object sender, EventArgs e)
        {
            FOrderType.SelectedIndex = 1;
            FTimeInForce.SelectedIndex = 1;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public void NewOrder(OrderMessage.MarketEnum Market, String Symbol, OrderMessage.SideEnum Side, double Px,
                             int OrderQty, OrderMessage.TimeInForceEnum TimeInForce)
        {
            OrderMessage.NewOrderMessage Msg = new OrderMessage.NewOrderMessage();
            Msg.Market = Market;
            Msg.Price = Px;
            Msg.OrderQty = OrderQty;
            Msg.Side = Side;
            Msg.Data = Program.UserData;
            Msg.AE = Program.FLogonID;
            Msg.Account = Program.FLogonAccount;
            Msg.Symbol = Symbol;
            Msg.OrderType = (OrderMessage.OrderTypeEnum)(FOrderType.SelectedItem);
            Msg.PositionEffect = (OrderMessage.PositionEffectEnum)(PEcomboBox.SelectedItem);
            Msg.TimeInForce = TimeInForce;                
            NewOrder( Msg );
            Msg = null;            
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void Clear()
        {
            FNIDHash.Clear();
            FOrderIDHash.Clear();
            FAccountSet.Clear();
            FNIDSet.Clear();
            FGrid.Rows.RemoveRange( 1, FGrid.Rows.Count - 1 );            
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void TOrderForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.WriteSetting();
            FLogon.Dispose();
            FOrderConnection.Destroy();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        long GetTick(  )
        {
            long CurrentTick;
            QueryPerformanceCounter(out CurrentTick );
            return CurrentTick;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnOrderStatus(OrderMessage.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByNID( Msg.NID );

            if ( FoundRow  != null )
            {
                long UseMS = (( GetTick() - (long)FoundRow.UserData) * 1000000) / FFreq;
                FoundRow["Symbol"]         = Msg.Symbol;
                FoundRow["OrderID"]        = Msg.OrderID;
                FoundRow["Data"]              = Msg.Data;
                FoundRow["BrokerID"]       = Msg.BrokerID;
                FoundRow["OrderStatus"] = Msg.OrderStatus;
                FoundRow["Text"]                = "OrderStatus";                
                FoundRow["Time"]              = System.DateTime.Now.ToString("HH:mm:ss.fff");
                FoundRow["UseTime"]      = UseMS;
                Console.WriteLine( "Order Status Request NID:" + Convert.ToString(Msg.NID));
            }
            else
                Console.WriteLine("Order Status Request NID:" + Convert.ToString(Msg.NID) + " not found!");
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnPendingNew(OrderMessage.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByNID( Msg.NID );

            if ( FoundRow  != null )            
            {
                FoundRow.UserData = GetTick();
                FoundRow[ "OrderStatus" ] = OrderMessage.OrderStatusEnum.osPendingNew;
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
            }
            else
                Console.WriteLine("Pending new NID:" + Convert.ToString(Msg.NID) + " not found!");
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnPendingCancel(OrderMessage.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID( Msg.OrderID ) ;

            if ( FoundRow != null)            
            {
                FoundRow.UserData = GetTick();
                OrderMessage.OrderStatusEnum OrderStatus = (OrderMessage.OrderStatusEnum)(FoundRow["OrderStatus"]);
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
                FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osPendingCancel;
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnPendingReplace(OrderMessage.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(Msg.OrderID);
            if ( FoundRow != null)
            {
                FoundRow.UserData = GetTick();
                FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osPendingReplace;
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnNewOrder(OrderMessage.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByNID(Msg.NID);

            if (FoundRow != null)///< Order from this order client
            {
                OrderMessage.OrderStatusEnum OldOrderStatus = (OrderMessage.OrderStatusEnum)FoundRow["OrderStatus"];
                long UseMS = 0;

                if( FoundRow.UserData != null )
                     UseMS = ((GetTick() - (long)FoundRow.UserData) * 1000000) / FFreq;
                FoundRow["UseTime"] = UseMS;
                FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                FoundRow["OrderID"] = Msg.OrderID;
                FoundRow["IsOrder"] = true;
                if ( OldOrderStatus == OrderMessage.OrderStatusEnum.osPendingNew )
                {                    
                    FOrderIDHash[ Msg.OrderID ] = FoundRow; ///< Add to OrderID map
                    FoundRow["Symbol"] = Msg.Symbol;
                    FoundRow["Data"] = Msg.Data;
                    FoundRow["BrokerID"] = Msg.BrokerID;
                    FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osNew;
                }
                else
                {                   
                    toolStripStatusLabel1.Text = "Confirm at status:" + OldOrderStatus.ToString() + " OID:" + Msg.OrderID;
                    toolStripLabel1.Invalidate();
                }
            }
            else ///< Order from other people or FIX
            {
                if (FNIDSet.ContainsKey(Msg.NID) == false) ///< NID not exists
                    AddNewRowByNewOrderExecutionReport(Msg, true);
            }
            FStore.Working(Msg.ExchangeCode, Msg.Symbol, (Msg.Side == OrderMessage.SideEnum.sBuy), Msg.Price, Msg.OrderQty);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnPartiallyFilled( OrderMessage.ExecutionReportMessage Msg )
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(Msg.OrderID);

            if (FoundRow == null) ///< Search by OrderID
            {
                Console.WriteLine("Order:" + Msg.OrderID + " not found! try to use NID:" + Convert.ToString(Msg.NID));
                FoundRow = SearchByNID(Msg.NID); ///< Search by NID
            }
            if (FoundRow != null)///< Search by NID
            {
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
                if (Msg.OrderQty > 0) ///< Filled or Partially filled
                {                                      
                    int Qty = Convert.ToInt32(FoundRow["Qty"]);
                    int CumQty = Convert.ToInt32(FoundRow["CumQty"]) + Msg.OrderQty;

                    FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                    double OrdPx = Convert.ToDouble(FoundRow["Price"]);
                    if (Qty == CumQty) ///< Filled
                    {
                        FoundRow["LastPrice"] = Convert.ToString(Msg.Price);
                        FoundRow["CumQty"] = CumQty;
                        FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osFilled;
                        if (ShowFillcheckBox.Checked == false)
                                FoundRow.Visible = false;
//                        FStore.Working(Msg.ExchangeCode, Msg.Symbol, (Msg.Side == OrderMessage.SideEnum.sBuy), OrdPx, -Msg.OrderQty);
                    }
                    else ///< Partially filled
                    {
                        FoundRow["LastPrice"] = Convert.ToString(Msg.Price);
                        FoundRow["CumQty"] = CumQty;
                        FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osPartiallyFilled;
                    }
                    FStore.Filled(Msg.ExchangeCode, Msg.Symbol, (Msg.Side == OrderMessage.SideEnum.sBuy), OrdPx, Msg.Price, Msg.OrderQty);
                }
                else                
                    RemoveFromGrid( FoundRow );   ///<Canceled.
            }
            else            
                Console.WriteLine("Order:" + Msg.OrderID + " NID:" + Convert.ToString(Msg.NID) + " not found!");            
        }  //OnPartiallyFilled()
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnReplaced( OrderMessage.ExecutionReportMessage Msg )
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(Msg.OrderID);
            if ( FoundRow != null )
            {
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
                FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");

                if (Msg.TMPExecType == OrderMessage.TMPExecTypeEnum.tetPxReplaced)
                {
                    double origPrice = Program.ConvertStringToDouble(FoundRow["Price"].ToString());
                    FoundRow["Price"] = Convert.ToString(Msg.Price);
                    FoundRow["OrderType"] = Msg.OrderType;
                    FoundRow["TimeInForce"] = Msg.TimeInForce;
                    FStore.Working(Msg.ExchangeCode, Msg.Symbol, (Msg.Side == OrderMessage.SideEnum.sBuy), origPrice, -Msg.OrderQty);
                    FStore.Working(Msg.ExchangeCode, Msg.Symbol, (Msg.Side == OrderMessage.SideEnum.sBuy), Msg.Price, Msg.OrderQty);
                }
                else ///< Reduce Qty
                {
                    int OrderQty = Convert.ToInt32(FoundRow["Qty"]);
                    int CumQty = Convert.ToInt32(FoundRow["CumQty"]);
                    int AfterQty = OrderQty - Msg.OrderQty;
                    
                    if (AfterQty - CumQty > 0)
                    {
                        FoundRow["Qty"] = AfterQty;
                        if (CumQty > 0)
                            FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osPartiallyFilled;
                        else
                            FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osNew;
                    }
                    else ///< Filled or canceled.
                    {
                        FoundRow["Qty"] = AfterQty;
                        if (CumQty > 0) ///< Filled
                        {
                            FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osFilled;
                            if (ShowFillcheckBox.Checked == false)
                                FoundRow.Visible = false;
                        }
                        else ///< Canceled
                            RemoveFromGrid(FoundRow);
                    }  //if (AfterQty - CumQty > 0)
                }  //if (Msg.TMPExecType == OrderMessage.TMPExecTypeEnum.tetPxReplaced)
            }  //if ( FoundRow != null )
        }  //OnReplaced()
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnCanceled(OrderMessage.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(Msg.OrderID);///< Search by OrderID

            if (FoundRow != null) ///< Find the row!
            {
                int CumQty = Convert.ToInt32(FoundRow["CumQty"]);

                FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
                if (CumQty == 0) ///<Canceled 
                    RemoveFromGrid(FoundRow);
                else
                    HideCanceledPartiallyFillOrder(FoundRow);
                FStore.Working( Msg.ExchangeCode, Msg.Symbol, (Msg.Side == OrderMessage.SideEnum.sBuy), Msg.Price, -Msg.OrderQty);
            }
            else ///<  Not found, try to search by NID.
            {
                FoundRow = SearchByNID(Msg.NID);///< Search by OrderID
                if (FoundRow != null) ///< Find the row!
                {
                    int CumQty = Convert.ToInt32(FoundRow["CumQty"]);

                    FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                    if (Msg.Data != null)
                        FoundRow["Data"] = Msg.Data;
                    if (CumQty == 0)
                        RemoveFromGrid(FoundRow);
                    else
                        HideCanceledPartiallyFillOrder(FoundRow);
                    OrderMessage.OrderStatusEnum OldOrderStatus = (OrderMessage.OrderStatusEnum)FoundRow["OrderStatus"];
                    toolStripStatusLabel1.Text = "Cancelled at status: " + OldOrderStatus.ToString() + " OID:" + Msg.OrderID;
                }
                toolStripLabel1.Invalidate();
                FStore.Working(Msg.ExchangeCode, Msg.Symbol, (Msg.Side == OrderMessage.SideEnum.sBuy), Msg.Price, -Msg.OrderQty);
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnRejected(OrderMessage.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByNID(Msg.NID);

            if (FoundRow == null)
                FoundRow = SearchByOrderID(Msg.OrderID);
            if (FoundRow != null)
            {
                FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                FoundRow["Text"] = string.Copy(Msg.Text);
                switch (Msg.CxlRejResponseTo)
                {
                    case OrderMessage.CxlRejResponseToEnum.crrNew:
                    case OrderMessage.CxlRejResponseToEnum.crrQuote:
                              FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osRejected;
                              FoundRow["OrderID"] = Msg.OrderID;
                              break;
                    default: break;
                }                
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
            }
        }       
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void HideCanceledPartiallyFillOrder(C1.Win.C1FlexGrid.Row HideRow)
        {
            HideRow[ "OrderStatus" ]  = OrderMessage.OrderStatusEnum.osCanceled;
            
            if ( ShowFillcheckBox.Checked == false )
                HideRow.Visible = false;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void RemoveFromGrid(C1.Win.C1FlexGrid.Row RemoveRow)
        {

            if (RemoveRow["NID"] != null)
            {
                int NID = Convert.ToInt32(RemoveRow["NID"]);
                if (FNIDHash.ContainsKey(NID))
                {
                    FNIDHash.Remove(NID);
                    FGrid.Rows.Remove(RemoveRow);
                    FGrid.ShowCell(FGrid.Rows.Count - 1, 0);
                    toolStripStatusLabel2.Text = Convert.ToSingle(FGrid.Rows.Count - 1) + " Order records";
                    return;
                }
            }
            if (RemoveRow["OrderID"] != null)
            {
                string OrderID = RemoveRow["OrderID"].ToString();
                if (FOrderIDHash.ContainsKey(OrderID))
                {
                    FOrderIDHash.Remove(OrderID);
                    FGrid.Rows.Remove(RemoveRow);
                    FGrid.ShowCell(FGrid.Rows.Count - 1, 0);
                    toolStripStatusLabel2.Text = Convert.ToSingle(FGrid.Rows.Count - 1) + " Order records";
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        C1.Win.C1FlexGrid.Row AddNewRowByNewOrderExecutionReport( OrderMessage.ExecutionReportMessage Msg, bool IsOrder )
        {           
            C1.Win.C1FlexGrid.Row NewRow = FGrid.Rows.Add();
            FGrid.ShowCell(FGrid.Rows.Count-1, 0);
            toolStripStatusLabel2.Text = Convert.ToSingle(FGrid.Rows.Count-1) + " Order records";
            int NID = Msg.NID;

            NewRow.UserData = GetTick();
            NewRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");                        
            NewRow["AE"] = Msg.AE;
            NewRow["Account"] = Msg.Account;
            NewRow["Symbol"] = Msg.Symbol;
            NewRow["OrderID"] = Msg.OrderID;
            NewRow["NID"] = Msg.NID;
            NewRow["Side"] = Msg.Side;
            NewRow["Price"] = Msg.Price;
            NewRow["Qty"] = Msg.OrderQty;
            NewRow["CumQty"] = 0;
            NewRow["OrderType"] = Msg.OrderType;
            NewRow["Market"] = Msg.Market;
            NewRow["OrderStatus"] = Msg.OrderStatus;
            NewRow["MaturityMonthYear"] = Msg.MaturityMonthYear;
            NewRow["StrikePrice"] = Msg.StrikePrice;
            NewRow["TimeInForce"] = Msg.TimeInForce;
            NewRow["TradingSession"] = Msg.TradingSessionID;
            NewRow["BrokerID"] = Msg.BrokerID;
            NewRow["BidCumQty"] = 0;
            NewRow["AskCumQty"] = 0;
            NewRow["AskPrice"] = 0;
            NewRow["BidPrice"] = 0;
            NewRow["IsOrder"] = IsOrder;
            if(Msg.Data != null)
                NewRow["Data"] = Msg.Data;
            if( NID != 0 )
                FNIDHash[¡@NID¡@] = NewRow;
            FNIDSet.Add( NID, null );
            if( Msg.OrderID.Length > 0 )
                FOrderIDHash[ Msg.OrderID ] = NewRow;
            CheckVisable( NewRow );
            CheckAccount(Msg.Account);
            return NewRow;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        C1.Win.C1FlexGrid.Row SearchByNID(int NID)
        {
            if (FNIDHash.ContainsKey(NID))
                return (C1.Win.C1FlexGrid.Row)(FNIDHash[NID]);
            return null;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        C1.Win.C1FlexGrid.Row SearchByOrderID(string OrderID)
        {
            if (FOrderIDHash.ContainsKey(OrderID))
                return (C1.Win.C1FlexGrid.Row)(FOrderIDHash[OrderID]);
            return null;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FPrice_KeyDown(object sender, KeyEventArgs e)
        {
            Program.TextBoxNumberOnly(e, true);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FQty_KeyDown(object sender, KeyEventArgs e)
        {
            Program.TextBoxNumberOnly(e, false);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void CancelSelected( )
        {
            for( int i = FGrid.Rows.Selected.Count - 1; i >= 0; i--)
            {
                C1.Win.C1FlexGrid.Row CancelRow = FGrid.Rows.Selected[i];
                OrderMessage.OrderStatusEnum OrderStatus = (OrderMessage.OrderStatusEnum)(CancelRow["OrderStatus"]);
                switch (OrderStatus)
                {
                    case OrderMessage.OrderStatusEnum.osPendingNew:
                    case OrderMessage.OrderStatusEnum.osPendingReplace:
                    case OrderMessage.OrderStatusEnum.osRejected: RemoveFromGrid(CancelRow); break;
                    case OrderMessage.OrderStatusEnum.osQuoteAccept: break;
                    case OrderMessage.OrderStatusEnum.osNew:
                    case OrderMessage.OrderStatusEnum.osPartiallyFilled:
                    case OrderMessage.OrderStatusEnum.osReplaced: OrderCancel(CancelRow); break;
                    case OrderMessage.OrderStatusEnum.osCanceled:
                    case OrderMessage.OrderStatusEnum.osExpired:
                    case OrderMessage.OrderStatusEnum.osFilled:
                    default: break;
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FGrid_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Delete:
                case Keys.C: CancelSelected();  break;
                case Keys.R: if (FGrid.Rows.Selected.Count > 0)
                                OrderReplace(FGrid.Rows.Selected[0]);
                             break;
                case Keys.S: C1.Win.C1FlexGrid.Row QueryRow = FGrid.Rows.Selected[0];
                             if( QueryRow != null )
                                 OrderStatusRequest(QueryRow);
                             break;
                case Keys.A: if (e.Control)
                                FGrid.Select( 1, 0, FGrid.Rows.Count-1, FGrid.Cols.Count-1);
                             break;
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FGrid_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                TSelectColForm SelectColForm = new TSelectColForm(FGrid);
                SelectColForm.Left = e.X + this.Left + FGrid.Left;
                SelectColForm.Top = e.Y + this.Top + FGrid.Top;
                SelectColForm.Show();
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void NewOrder(OrderMessage.NewOrderMessage Msg )
        {
            C1.Win.C1FlexGrid.Row NewRow = FGrid.Rows.Add();
            FGrid.ShowCell( FGrid.Rows.Count -1, 0 );
            toolStripStatusLabel2.Text = Convert.ToSingle(FGrid.Rows.Count-1) +" Order records";
            foreach (C1.Win.C1FlexGrid.Row SelRow in FGrid.Rows.Selected)
                SelRow.Selected = false;
            NewRow.Selected = true;

            NewRow["AE"] = Msg.AE;
            NewRow["Account"] = Msg.Account;
            NewRow["Symbol"] = Msg.Symbol;            
            NewRow["OrderID"] = "";
            NewRow["Side"] = Msg.Side;
            NewRow["Price"] = Msg.Price;
            NewRow["Qty"] = Msg.OrderQty;
            NewRow["CumQty"] = 0;
            NewRow["OrderType"] = Msg.OrderType;
            NewRow["Market"] = Msg.Market;
            NewRow["Text"] = "";
            NewRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osPendingNew;
            NewRow["TradingSession"] = Msg.TradingSessionID;
            NewRow["MaturityMonthYear"] = Msg.MaturityMonthYear;
            switch (Msg.Market)
            {
                case OrderMessage.MarketEnum.mFutures:                    
                    NewRow["StrikePrice"] = Msg.StrikePrice;
                    NewRow["TimeInForce"] = Msg.TimeInForce;
                    break;
                case OrderMessage.MarketEnum.mOptions:
                    NewRow["StrikePrice"] = 0.0;
                    NewRow["TimeInForce"] = Msg.TimeInForce;
                    break;
            }
            Msg.NID = FOrderConnection.GenerateUniqueID(Msg.Market, OrderMessage.MessageTypeEnum.mtNew);
            NewRow["NID"] = Msg.NID;
            if (Msg.NID != 0)
                FNIDHash[Msg.NID] = NewRow;
            FNIDSet.Add( Msg.NID, null);
            CheckVisable(NewRow);
            CheckAccount(Msg.Account);
            FOrderConnection.NewOrder(Msg);         
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OrderReplace(C1.Win.C1FlexGrid.Row ReplaceRow)
        {
            OrderMessage.OrderStatusEnum OrderStatus = (OrderMessage.OrderStatusEnum)(ReplaceRow["OrderStatus"]);
            if ((OrderStatus == OrderMessage.OrderStatusEnum.osNew) || (OrderStatus == OrderMessage.OrderStatusEnum.osPartiallyFilled))
            {
                OrderMessage.MarketEnum Market = (OrderMessage.MarketEnum)(ReplaceRow["Market"]);
                double CurPrice = Program.ConvertStringToDouble(ReplaceRow["Price"].ToString());
                int    QtyNow = Convert.ToInt32(ReplaceRow["Qty"]);
                bool   IsLimitROD = false;
                bool   IsPartiallFill = true;
                
                if (Market != OrderMessage.MarketEnum.mOTC && Market != OrderMessage.MarketEnum.mTSE)                
                {
                    if (OrderMessage.TimeInForceEnum.tifROD == (OrderMessage.TimeInForceEnum)(ReplaceRow["TimeInForce"]) &&
                        OrderMessage.OrderTypeEnum.otLimit == (OrderMessage.OrderTypeEnum)(ReplaceRow["OrderType"]))
                        IsLimitROD = true;
                    if (Convert.ToInt32(ReplaceRow["CumQty"]) == 0)
                        IsPartiallFill = false;
                }
                TReplaceForm ReplaceForm = new TReplaceForm( Market, QtyNow, CurPrice, IsLimitROD, IsPartiallFill);
                
                if (ReplaceForm.ShowDialog() == DialogResult.OK)
                {
                    OrderMessage.ReplaceOrderMessage Msg = new OrderMessage.ReplaceOrderMessage();

                    Msg.Market         = Market;
                    Msg.Account        = ReplaceRow["Account"].ToString();
                    Msg.OrderID        = ReplaceRow["OrderID"].ToString();
                    Msg.Symbol         = ReplaceRow["Symbol"].ToString();
                    Msg.BrokerID       = ReplaceRow["BrokerID"].ToString();                    
                    Msg.Side           = (OrderMessage.SideEnum)(ReplaceRow["Side"]);
                    Msg.PositionEffect = OrderMessage.PositionEffectEnum.peOpen;
                    Msg.Data           = Program.UserData;
                    if (ReplaceForm.IsReplaceQty() == true)
                    {
                        Msg.OrderType = (OrderMessage.OrderTypeEnum)(ReplaceRow["OrderType"]);
                        Msg.OrderQty = ReplaceForm.GetReplaceQty();
                        Msg.Price = Program.ConvertStringToDouble(ReplaceRow["Price"].ToString());
                    }
                    else
                    {
                        Msg.OrderQty = 0;
                        Msg.OrderType = ReplaceForm.GetOrderType();
                        Msg.Price = ReplaceForm.GetReplacePrice();
                        Msg.TimeInForce = ReplaceForm.GetTIF();
                    }                    
                    FOrderConnection.ReplaceOrder(Msg);
                    int NID = Msg.NID;
                    if (NID != 0)
                        FNIDHash[NID] = ReplaceRow;
                    Msg = null;
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OrderCancel(C1.Win.C1FlexGrid.Row CancelRow)
        {
            OrderMessage.CancelOrderMessage Msg = new OrderMessage.CancelOrderMessage();
            String OrderID = CancelRow["OrderID"].ToString(); ///< Cancel Order by OrderID.   
                        
            Msg.Market = (OrderMessage.MarketEnum)(CancelRow["Market"]);
            Msg.Account = CancelRow["Account"].ToString();
            Msg.OrderID = OrderID;// "z" + OrderID.Substring(1, 4);
            Msg.Symbol  = CancelRow["Symbol"].ToString();
            Msg.Price   = Program.ConvertStringToDouble(CancelRow["Price"].ToString());
            Msg.Side    = (OrderMessage.SideEnum)(CancelRow["Side"]);
            Msg.OrderType        = (OrderMessage.OrderTypeEnum)(CancelRow["OrderType"]);
            Msg.TradingSessionID = (OrderMessage.TradingSessionIDEnum)(CancelRow["TradingSession"]);
            //Msg.TimeInForce = (OrderMessage.TimeInForceEnum)(CancelRow["TimeInForce"]);
            
            //Msg.BrokerID = CancelRow["BrokerID"].ToString();
            switch (Msg.Market)
            {
                case OrderMessage.MarketEnum.mFutures:                    
                    if( CancelRow["MaturityMonthYear"] != null )
                        Msg.MaturityMonthYear = CancelRow["MaturityMonthYear"].ToString();
                    Msg.StrikePrice = Convert.ToDouble(CancelRow["StrikePrice"]);
                    Msg.Data = Program.UserData;
                    break;
                case OrderMessage.MarketEnum.mOptions:
                    Msg.TimeInForce = (OrderMessage.TimeInForceEnum)(CancelRow["TimeInForce"]);
                    Msg.Data = Program.UserData;
                    break;
                case OrderMessage.MarketEnum.mOTC:
                    Msg.Data = Program.UserData;
                    break;
                case OrderMessage.MarketEnum.mTSE:
                    Msg.Data = Program.UserData;
                    break;
            }
            FOrderConnection.CancelOrder(Msg);
            int NID = Msg.NID;
            if ( NID != 0)
                 FNIDHash[NID] = CancelRow;
            Msg = null;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OrderStatusRequest(C1.Win.C1FlexGrid.Row  QueryRow )
        {
            OrderMessage.OrderStatusRequestMessage Msg = new OrderMessage.OrderStatusRequestMessage();

            Msg.Market = (OrderMessage.MarketEnum)(QueryRow["Market"]);
            Msg.Account = QueryRow["Account"].ToString();
            Msg.OrderID = QueryRow["OrderID"].ToString(); ///< Cancel Order by OrderID.  
            Msg.Symbol = QueryRow["Symbol"].ToString();
            Msg.Price = Program.ConvertStringToDouble(QueryRow["Price"].ToString());
            Msg.Side = (OrderMessage.SideEnum)(QueryRow["Side"]);
            Msg.TradingSessionID = (OrderMessage.TradingSessionIDEnum)(QueryRow["TradingSession"]);
            Msg.Data = "OrderStatusRequest";
            FOrderConnection.OrderStatusRequest(Msg);
            int NID = Msg.NID;
            if (NID != 0)
                FNIDHash[NID] = QueryRow;
            Console.WriteLine(" ------ Order Sataus Request  NID:" + Convert.ToString(NID) );
            Msg = null;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void Connectbutton_Click(object sender, EventArgs e)
        {
            if ( Program.FSpeedyIP.Length <= 0)
            {
                MessageBox.Show("Ip.Text.Length <= 0");
                return;
            }
            int Port = Program.FSpeedyPort;
            if ((Port <= 0x0000) || (Port > 0xffff))
            {
                MessageBox.Show("(Port <= 0) || (Port > 65535)");
                return;
            }
            FOrderConnection.Connect(Program.FSpeedyIP, Port);            
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FRecover_Click(object sender, EventArgs e)
        {
            string RecoverTime = dateTimePicker.Value.ToString("HHmmss");

            FRecover.Enabled = false;
            Clear();
            FStore.Clear();
            FGrid.Visible = false;
            FRecoverCount = 0;
            FOrderConnection.Recover( RecoverTime, OrderConnection.RecoverTypeEnum.rtAll, OrderConnection.RecoverMarketEnum.rmAll);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FIp_KeyDown(object sender, KeyEventArgs e)
        {
            Program.TextBoxNumberOnly(e, true);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FPort_KeyDown(object sender, KeyEventArgs e)
        {
            Program.TextBoxNumberOnly(e, false);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void Disconnectbutton_Click(object sender, EventArgs e)
        {
            FOrderConnection.Disconnect();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void SetRowStyle(C1.Win.C1FlexGrid.Row Row, C1.Win.C1FlexGrid.CellStyle Style)
        {
            if (Row.Style != Style)
                Row.Style = Style;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FGrid_OwnerDrawCell(object sender, C1.Win.C1FlexGrid.OwnerDrawCellEventArgs e)
        {
            if( e.Row < FGrid.Rows.Fixed)
                return;
            object SideCell = FGrid[e.Row, FGrid.Cols["Side"].Index];

            OrderMessage.SideEnum Side;
            if (SideCell == null)
                Side = OrderMessage.SideEnum.sNone;
            else
                Side = (OrderMessage.SideEnum)SideCell;
            OrderMessage.OrderStatusEnum Status = (OrderMessage.OrderStatusEnum)(FGrid[e.Row, FGrid.Cols["OrderStatus"].Index ]);
            
            switch (Status)
            {
                case OrderMessage.OrderStatusEnum.osPendingNew:
                case OrderMessage.OrderStatusEnum.osPendingCancel:
                case OrderMessage.OrderStatusEnum.osPendingReplace: SetRowStyle( FGrid.Rows[e.Row], FGrid.Styles["Pending"]); break;
                case OrderMessage.OrderStatusEnum.osRejected: SetRowStyle( FGrid.Rows[e.Row], FGrid.Styles["Rejected"]); break;
                case OrderMessage.OrderStatusEnum.osNew: 
                    if( Side == OrderMessage.SideEnum.sBuy )
                        SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["BuyNewOrder"]);
                    else               
                        SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["SellNewOrder"]); 
                    break;
                case OrderMessage.OrderStatusEnum.osPartiallyFilled:
                    if (Side == OrderMessage.SideEnum.sBuy)
                        SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["BuyPartialFilled"]);
                    else
                        SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["SellPartialFilled"]);
                    break;
                case OrderMessage.OrderStatusEnum.osCanceled:
                case OrderMessage.OrderStatusEnum.osFilled:
                    SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["Filled"]);
                    break;
                case OrderMessage.OrderStatusEnum.osQuoteAccept:                     
                    break;

            }            
        }        
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            SettingForm setting = new SettingForm();
            if (setting.ShowDialog() == DialogResult.OK)
            {
                FOrderConnection.SetBrokerID( OrderMessage.MarketEnum.mFutures, Program.FBrokerID );
                FOrderConnection.SetBrokerID(OrderMessage.MarketEnum.mTSE, Program.FTSEBrokerID);
                FOrderConnection.ClearMemberID = Program.FCMID;
                if (Program.FNewOrder == "true")
                {
                    Orderpanel.Visible = true;
                    FGrid.Top = (Orderpanel.Top + Orderpanel.Height  );
                    FGrid.Height = this.ClientSize.Height - Orderpanel.Height - panelFilter.Height - toolStrip1.Height;
                }
                else
                {
                    Orderpanel.Visible = false;
                    FGrid.Top = (panelFilter.Top + panelFilter.Height);
                    FGrid.Height = this.ClientSize.Height - panelFilter.Height - toolStrip1.Height;
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void TOrderForm_Shown(object sender, EventArgs e)
        {
            Orderpanel.Visible = true;                
            FGrid.Top = (Orderpanel.Top + Orderpanel.Height);
            FGrid.Height = this.ClientSize.Height - Orderpanel.Height - panelFilter.Height - toolStrip1.Height -30;
            timerShow.Enabled = true;            
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void clockTimer_Tick(object sender, EventArgs e)
        {
            dateTimePicker.Value  = DateTime.Now;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void CheckAccount(  String Account )
        {
            if (FAccountSet.Contains(Account) == false)
            {
                FAccountSet.Add(Account, null);
                AccountComboBox.Items.Add(Account);
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void CheckVisable( C1.Win.C1FlexGrid.Row Row )
        {
            switch (BSComboBox.SelectedIndex )
            {
                case 0: break;
                case 1: if ((OrderMessage.SideEnum)Row["Side"] != OrderMessage.SideEnum.sBuy)
                                {
                                    Row.Visible = false;
                                    return;
                                }
                                break;
                case 2:   if ((OrderMessage.SideEnum)Row["Side"] != OrderMessage.SideEnum.sSell)
                                {
                                    Row.Visible = false;
                                    return;
                                }
                                break;
                default: break;
            }            
            
            switch (MarketComboBox.SelectedIndex)
            {
                case 0: break;
                case 1: if ((OrderMessage.MarketEnum)Row["Market"] != OrderMessage.MarketEnum.mFutures)
                                {
                                        Row.Visible = false;
                                        return;
                                }
                                break;
                case 2: if ((OrderMessage.MarketEnum)Row["Market"] != OrderMessage.MarketEnum.mOptions)
                                {
                                        Row.Visible = false;
                                        return;
                                }
                                break;
                case 3: if ((OrderMessage.MarketEnum)Row["Market"] != OrderMessage.MarketEnum.mTSE)
                                {
                                        Row.Visible = false;
                                        return;
                                }
                                break;
                case 4: if ((OrderMessage.MarketEnum)Row["Market"] != OrderMessage.MarketEnum.mOTC)
                                {
                                        Row.Visible = false;
                                        return;
                                }
                                break;
                default: break;
            }
            
            if (AccountComboBox.SelectedIndex != 0)
            {
                if(  Row["Account"].ToString()  !=  AccountComboBox.Text )
                {
                    Row.Visible = false;
                    return;
                }
            }

            OrderMessage.OrderStatusEnum OrderStatus = (OrderMessage.OrderStatusEnum)Row["OrderStatus"];
            if (OrderStatus == OrderMessage.OrderStatusEnum.osFilled || OrderStatus == OrderMessage.OrderStatusEnum.osCanceled )
            {
                if (ShowFillcheckBox.Checked == false)
                {
                    Row.Visible = false;
                    return;
                }
            }
            Row.Visible = true;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void AccountComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            for (int i = 1; i < FGrid.Rows.Count; i++)
                CheckVisable(FGrid.Rows[i]);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void ShowFillcheckBox_CheckedChanged(object sender, EventArgs e)
        {
            for (int i = 1; i < FGrid.Rows.Count; i++)
                CheckVisable(FGrid.Rows[i]);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void FGrid_SelChange(object sender, EventArgs e)
        {
            toolStripStatusLabel2.Text = Convert.ToString(FGrid.Rows.Selected.Count) + " Orders selected";                              
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void timerShow_Tick(object sender, EventArgs e)
        {
            timerShow.Enabled = false;
            FMDMainForm.Left = Left - FMDMainForm.Width;
            FMDMainForm.Top = Top;
            FMDMainForm.Height = Height;
            FMDMainForm.Show();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public void CancelOrder( OrderMessage.MarketEnum Market, String Symbol, OrderMessage.SideEnum Side, double Px)
        {
            for (int i = FGrid.Rows.Count - 1; i >= FGrid.Rows.Fixed; i--)
            {
                C1.Win.C1FlexGrid.Row CancelRow = FGrid.Rows[i];
                if (CancelRow != null)
                {                    
                    OrderMessage.OrderStatusEnum Status = (OrderMessage.OrderStatusEnum)CancelRow["OrderStatus"];
                    switch (Status)
                    {
                        case  OrderMessage.OrderStatusEnum.osNew:
                        case  OrderMessage.OrderStatusEnum.osPartiallyFilled:
                        case  OrderMessage.OrderStatusEnum.osReplaced:
                                            if (Market == (OrderMessage.MarketEnum)(CancelRow["Market"]) &&
                                                Symbol == CancelRow["Symbol"].ToString() &&
                                                Side == (OrderMessage.SideEnum)(CancelRow["Side"]) &&
                                                Px == Program.ConvertStringToDouble(CancelRow["Price"].ToString()))
                                                OrderCancel(CancelRow);
                                            break;
                        default: break;
                    }          
                }
            }                
        }  // CancelOrder()
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        public void CancelOrder(OrderMessage.MarketEnum Market, String Symbol, OrderMessage.SideEnum Side )
        {
            for (int i = FGrid.Rows.Count - 1; i >= FGrid.Rows.Fixed; i--)
            {
                C1.Win.C1FlexGrid.Row CancelRow = FGrid.Rows[i];
                if (CancelRow != null)
                {
                    OrderMessage.OrderStatusEnum Status = (OrderMessage.OrderStatusEnum)CancelRow["OrderStatus"];
                    switch (Status)
                    {
                        case OrderMessage.OrderStatusEnum.osNew:
                        case OrderMessage.OrderStatusEnum.osPartiallyFilled:
                        case OrderMessage.OrderStatusEnum.osReplaced:
                            if (Market == (OrderMessage.MarketEnum)(CancelRow["Market"]) &&
                                Symbol == CancelRow["Symbol"].ToString() &&
                                Side == (OrderMessage.SideEnum)(CancelRow["Side"]))
                                OrderCancel(CancelRow);
                            break;
                        default: break;
                    }
                }
            }
        }  //CancelOrder()
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        void OrderReplace(C1.Win.C1FlexGrid.Row replaceRow, double newPrice) {
            OrderMessage.OrderStatusEnum orderStatus = (OrderMessage.OrderStatusEnum)(replaceRow["OrderStatus"]);
            if ((orderStatus == OrderMessage.OrderStatusEnum.osNew) || 
                (orderStatus == OrderMessage.OrderStatusEnum.osPartiallyFilled) ||
                (orderStatus == OrderMessage.OrderStatusEnum.osReplaced)) {
                OrderMessage.MarketEnum market = (OrderMessage.MarketEnum)(replaceRow["Market"]);
                double CurPrice = Program.ConvertStringToDouble(replaceRow["Price"].ToString());
                int QtyNow = Convert.ToInt32(replaceRow["Qty"]);
                OrderMessage.ReplaceOrderMessage replaceMsg = new OrderMessage.ReplaceOrderMessage();
                replaceMsg.Market = market;
                replaceMsg.Account = replaceRow["Account"].ToString();
                replaceMsg.OrderID = replaceRow["OrderID"].ToString();
                replaceMsg.Symbol = replaceRow["Symbol"].ToString();
                replaceMsg.BrokerID = replaceRow["BrokerID"].ToString();
                replaceMsg.Side = (OrderMessage.SideEnum)(replaceRow["Side"]);
                replaceMsg.PositionEffect = OrderMessage.PositionEffectEnum.peOpen;
                replaceMsg.Data = Program.UserData;
                replaceMsg.OrderType = (OrderMessage.OrderTypeEnum)(replaceRow["OrderType"]);
                replaceMsg.OrderQty = 0;
                replaceMsg.Price = newPrice;
                replaceMsg.TimeInForce = (OrderMessage.TimeInForceEnum)(replaceRow["TimeInForce"]); ;
                FOrderConnection.ReplaceOrder(replaceMsg);
                int NID = replaceMsg.NID;
                if (NID != 0) FNIDHash[NID] = replaceRow;
                replaceMsg = null;
            }
        }  //OrderReplace()
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public void ReplaceOrderPrice(OrderMessage.MarketEnum Market, String Symbol, OrderMessage.SideEnum Side, double origPrice, double newPrice) {
            for (int i = FGrid.Rows.Count - 1; i >= FGrid.Rows.Fixed; i--) {
                C1.Win.C1FlexGrid.Row currentRow = FGrid.Rows[i];

                if (currentRow == null) continue;
                OrderMessage.OrderStatusEnum orderStatus = (OrderMessage.OrderStatusEnum)currentRow["OrderStatus"];
                if ((orderStatus == OrderMessage.OrderStatusEnum.osNew) ||
                    (orderStatus == OrderMessage.OrderStatusEnum.osPartiallyFilled) ||
                    (orderStatus == OrderMessage.OrderStatusEnum.osReplaced)) {
                    if ((Market == (OrderMessage.MarketEnum)(currentRow["Market"])) &&
                        (Symbol == currentRow["Symbol"].ToString()) &&
                        (Side == (OrderMessage.SideEnum)(currentRow["Side"])) &&
                        (origPrice == Program.ConvertStringToDouble(currentRow["Price"].ToString())))
                        OrderReplace(currentRow, newPrice);
                }
            }  //for (int i = FGrid.Rows.Count - 1; i >= FGrid.Rows.Fixed; i--)
        }  // CancelOrder()
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
    }
}