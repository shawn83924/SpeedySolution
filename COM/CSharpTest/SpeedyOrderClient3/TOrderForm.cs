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
using System.Globalization;

namespace Test1
{
    public partial class TOrderForm : Form
    {
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        [DllImport("Kernel32.dll")]
        private static extern bool QueryPerformanceCounter(out long lpPerformanceCount);

        [DllImport("Kernel32.dll")]
        private static extern bool QueryPerformanceFrequency(out long lpFrequency);
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        SpeedyAPI.OrderConnection FOrderConnection = null;
        SpeedyAPI.NewOrderMessage FNewOrderMsg = null;
        System.Collections.Hashtable FNIDHash = null;
        System.Collections.Hashtable FOrderIDHash = null;
        System.Collections.Hashtable FAccountSet = null;
        System.Collections.Hashtable FNIDSet = null;
        LogonForm FLogon = null;
        Object LockObject = null;
        int FRecoverCount;
        long FFreq;
        SpeedyAPI.APIEnum FWhichAPI = SpeedyAPI.APIEnum.aeSpeedy;
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public TOrderForm()
        {
            InitializeComponent();
            InitializeGrid();
            InitializeComboBox();

            FNIDHash = new System.Collections.Hashtable();
            FOrderIDHash = new System.Collections.Hashtable();
            FAccountSet = new System.Collections.Hashtable();
            FNIDSet = new System.Collections.Hashtable();
            FOrderConnection = new SpeedyAPI.OrderConnection();
            FNewOrderMsg = new SpeedyAPI.NewOrderMessage();
            FLogon = new LogonForm(FOrderConnection);
            LockObject = new Object();
            ReadSetting();
            // Enable auto detect API. ( Supports Speedy and China CTP/Femas/SunGard API )
            FOrderConnection.DetectAPI = true;
            // Event handler for connection.
            FOrderConnection.OnConnected += new SpeedyAPI.IOrderConnectionEvents_OnConnectedEventHandler(FOrderConnection_OnConnected);
            FOrderConnection.OnDisconnected += new SpeedyAPI.IOrderConnectionEvents_OnDisconnectedEventHandler(FOrderConnection_OnDisconnected);
            // Event Handler for SpeedyGateway
            FOrderConnection.OnLogonReply += new SpeedyAPI.IOrderConnectionEvents_OnLogonReplyEventHandler(FOrderConnection_OnLogonReply);
            FOrderConnection.OnExecutionReport += new SpeedyAPI.IOrderConnectionEvents_OnExecutionReportEventHandler(FOrderConnection_OnExecutionReport);
            FOrderConnection.OnRecoverFinished += new SpeedyAPI.IOrderConnectionEvents_OnRecoverFinishedEventHandler(FOrderConnection_OnRecoverFinished);
            FOrderConnection.OnNews += new SpeedyAPI.IOrderConnectionEvents_OnNewsEventHandler(FOrderConnection_OnNews);
            FOrderConnection.OnCancelWorking += new SpeedyAPI.IOrderConnectionEvents_OnCancelWorkingEventHandler(FOrderConnection_OnCancelWorking);
            String LogFileName = "SpeedyOrderClient_" + System.DateTime.Now.Date.Year.ToString() + "_" + System.DateTime.Now.Date.Month.ToString() + "_" + System.DateTime.Now.Date.Day.ToString() + ".log";
            ///< Create object instance. the AppName must unique in this PC. 
            FOrderConnection.Create2("OrderClient@" + System.Net.Dns.GetHostName());
            ///< turn on the debug log.
            FOrderConnection.SetDebugLog(LogFileName);
            FOrderConnection.SetBrokerID(SpeedyAPI.MarketEnum.mFutures, Program.FBrokerID);
            FOrderConnection.SetBrokerID(SpeedyAPI.MarketEnum.mTSE, Program.FTSEBrokerID);
            FOrderConnection.SetBrokerID(SpeedyAPI.MarketEnum.mCNFutures, Program.FCNBrokerID);
            FOrderConnection.ClearMemberID = Program.FCMID;
            FOrderConnection.SetLanguage(SpeedyAPI.MessageLanguageEnum.mlChinese);
            FOrderConnection.AddSellSide(SpeedyAPI.SellSideTypeEnum.ssPATS);
            FOrderConnection.UseNewFuturesSymbol = true;
            FOrderConnection.EnablePandingNewAck = false;

            SetMarketButton();

            QueryPerformanceFrequency(out FFreq);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void TOrderForm_Shown(object sender, EventArgs e)
        {
            if (Program.FNewOrder == "true")
            {
                Orderpanel.Visible = true;
                FGrid.Top = (Orderpanel.Top + Orderpanel.Height);
                FGrid.Height = this.ClientSize.Height - Orderpanel.Height - panelFilter.Height - toolStrip1.Height - 30;
                EnableMarketBtns(true);
            }
            else
            {
                Orderpanel.Visible = false;
                FGrid.Top = (panelFilter.Top + panelFilter.Height);
                FGrid.Height = this.ClientSize.Height - panelFilter.Height - toolStrip1.Height - 30;
                EnableMarketBtns(false);
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void SetMarketButton()
        {
            bool SupportTWSEOTCPATS = false;

            if (FWhichAPI == SpeedyAPI.APIEnum.aeSpeedy)
                SupportTWSEOTCPATS = true;
            this.FutButton.Visible = true;
            this.OptButton.Visible = true;
            this.TseButton.Visible = SupportTWSEOTCPATS;
            this.OtcButton.Visible = SupportTWSEOTCPATS;
            this.PATSButton.Visible = SupportTWSEOTCPATS;
            this.PATSOButton.Visible = SupportTWSEOTCPATS;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void FOrderConnection_OnNews(SpeedyAPI.NewsMessage News)
        {
            toolStripStatusLabel1.Text = News.Headline;
            toolStripStatusLabel3.Text = News.Text;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void ReadSetting()
        {
            Microsoft.Win32.RegistryKey SoftwareReg = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("SOFTWARE", true);
            Microsoft.Win32.RegistryKey MDBSReg = SoftwareReg.CreateSubKey("MDBS");
            Microsoft.Win32.RegistryKey AppReg = MDBSReg.CreateSubKey("SpeedyOrderClient");

            FIp.Text = AppReg.GetValue("IP", "192.168.168.120").ToString();
            FPort.Text = AppReg.GetValue("Port", "23456").ToString();
            FLogon.FId.Text = AppReg.GetValue("ID", "frank").ToString();
            FLogon.FPass.Text = AppReg.GetValue("Password", "frank").ToString();
            FLogon.FAccount.Text = AppReg.GetValue("Account", "0000001").ToString();
            Program.FBrokerID = AppReg.GetValue("BrokerID", "F006000").ToString();
            Program.FTSEBrokerID = AppReg.GetValue("TSEBrokerID", "7000").ToString();
            Program.FCNBrokerID = AppReg.GetValue("CNBrokerID", "1111").ToString();
            Program.FCMID = AppReg.GetValue("CMID", "F006").ToString();
            Program.FMMAccount = AppReg.GetValue("MMAccount", "8888888").ToString();
            Program.FPATSAccount = AppReg.GetValue("PATSAccount", "APITEST18B").ToString();
            Program.FNewVer = AppReg.GetValue("NewVer", "false").ToString();
            Program.FNewOrder = AppReg.GetValue("NewOrder", "false").ToString();

            Program.FUTSymbol = AppReg.GetValue("FutSym", "XIFI8").ToString();
            Program.OPTSymbol = AppReg.GetValue("OptSym", "TXO06500I9").ToString();
            Program.CNFUTSymbol = AppReg.GetValue("CNFutSym", "IF1312").ToString();
            Program.CNOPTSymbol = AppReg.GetValue("CNOptSym", "IO06500I9").ToString();
            Program.TSESymbol = AppReg.GetValue("TseSym", "2357").ToString();
            Program.OTCSymbol = AppReg.GetValue("OtcSym", "6205").ToString();
            Program.PATSSymbol = AppReg.GetValue("PatsSym", "TW").ToString();
            Program.FUTPx = AppReg.GetValue("FutPx", "255").ToString();
            Program.OPTPx = AppReg.GetValue("OptPx", "6400.0").ToString();
            Program.CNFUTPx = AppReg.GetValue("CNFutPx", "255").ToString();
            Program.CNOPTPx = AppReg.GetValue("CNOptPx", "6400.0").ToString();
            Program.TSEPx = AppReg.GetValue("TsePx", "89.50").ToString();
            Program.OTCPx = AppReg.GetValue("OtcPx", "61.00").ToString();
            Program.PATSPx = AppReg.GetValue("PatsPx", "345.00").ToString();
            Program.PATSEx = AppReg.GetValue("PatsEx", "SIM").ToString();
            Program.PATSMMY = AppReg.GetValue("PatsMMY", "DEC11").ToString();
            Program.FReadOnly = AppReg.GetValue("ReadOnly", "false").ToString();
            Program.UserData = AppReg.GetValue("UserData", "000011").ToString();
            if (Program.FReadOnly == "true")
                Program.FNewOrder = "false";
            int ColCount = FGrid.Cols.Count;
            try
            {
                for (int i = 0; i < ColCount; i++)
                {
                    string IndexName = "IndexCol" + Convert.ToString(i);
                    Object ValueObj = AppReg.GetValue(IndexName);
                    if (ValueObj != null)
                        FGrid.Cols[ValueObj.ToString()].Move(i);
                }
                for (int i = 0; i < ColCount; i++)
                {
                    string VisableName = "VisibleCol" + Convert.ToString(i);
                    Object ValueObj = AppReg.GetValue(VisableName);
                    if (ValueObj != null)
                        FGrid.Cols[i].Visible = Convert.ToBoolean(ValueObj);
                }
            }
            catch (Exception)
            {
            }
            AppReg.Close();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void WriteSetting()
        {
            Microsoft.Win32.RegistryKey SoftwareReg = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("SOFTWARE", true);
            Microsoft.Win32.RegistryKey MDBSReg = SoftwareReg.CreateSubKey("MDBS");
            Microsoft.Win32.RegistryKey AppReg = MDBSReg.CreateSubKey("SpeedyOrderClient");

            AppReg.SetValue("IP", FIp.Text);
            AppReg.SetValue("Port", FPort.Text);
            AppReg.SetValue("ID", FLogon.FId.Text);
            AppReg.SetValue("Password", FLogon.FPass.Text);
            AppReg.SetValue("Account", FLogon.FAccount.Text);
            AppReg.SetValue("BrokerID", Program.FBrokerID);
            AppReg.SetValue("CNBrokerID", Program.FCNBrokerID);
            AppReg.SetValue("TSEBrokerID", Program.FTSEBrokerID);
            AppReg.SetValue("CMID", Program.FCMID);
            AppReg.SetValue("MMAccount", Program.FMMAccount);
            AppReg.SetValue("PATSAccount", Program.FPATSAccount);
            AppReg.SetValue("NewVer", Program.FNewVer);
            AppReg.SetValue("NewOrder", Program.FNewOrder);

            AppReg.SetValue("FutSym", Program.FUTSymbol);
            AppReg.SetValue("OptSym", Program.OPTSymbol);
            AppReg.SetValue("CNFutSym", Program.CNFUTSymbol);
            AppReg.SetValue("CNOptSym", Program.CNOPTSymbol);
            AppReg.SetValue("TseSym", Program.TSESymbol);
            AppReg.SetValue("OtcSym", Program.OTCSymbol);
            AppReg.SetValue("PatsSym", Program.PATSSymbol);
            AppReg.SetValue("FutPx", Program.FUTPx);
            AppReg.SetValue("OptPx", Program.OPTPx);
            AppReg.SetValue("CNFutPx", Program.CNFUTPx);
            AppReg.SetValue("CNOptPx", Program.CNOPTPx);
            AppReg.SetValue("TsePx", Program.TSEPx);
            AppReg.SetValue("OtcPx", Program.OTCPx);
            AppReg.SetValue("PatsPx", Program.PATSPx);
            AppReg.SetValue("PatsEx", Program.PATSEx);
            AppReg.SetValue("PatsMMY", Program.PATSMMY);
            AppReg.SetValue("ReadOnly", Program.FReadOnly);
            AppReg.SetValue("UserData", Program.UserData);
            int ColCount = FGrid.Cols.Count;
            for (int i = 0; i < ColCount; i++)
            {
                string VisableName = "VisibleCol" + Convert.ToString(i);
                string IndexName = "IndexCol" + Convert.ToString(FGrid.Cols[i].Index);
                AppReg.SetValue(VisableName, FGrid.Cols[i].Visible);
                AppReg.SetValue(IndexName, FGrid.Cols[i].Name);
            }
            AppReg.Close();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void EnableMarketBtns(bool Enable)
        {
            this.FutButton.Enabled = Enable;
            this.OptButton.Enabled = Enable;
            this.TseButton.Enabled = Enable;
            this.OtcButton.Enabled = Enable;
            this.PATSButton.Enabled = Enable;
            this.PATSOButton.Enabled = Enable;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void SelectedMarket(object sender)
        {
            if (sender == FutButton)
                FutButton.Checked = true;
            else
                FutButton.Checked = false;
            if (sender == OptButton)
                OptButton.Checked = true;
            else
                OptButton.Checked = false;
            if (sender == TseButton)
                TseButton.Checked = true;
            else
                TseButton.Checked = false;
            if (sender == OtcButton)
                OtcButton.Checked = true;
            else
                OtcButton.Checked = false;
            if (sender == PATSButton)
                PATSButton.Checked = true;
            else
                PATSButton.Checked = false;
            if (sender == PATSOButton)
                PATSOButton.Checked = true;
            else
                PATSOButton.Checked = false;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void FOrderConnection_OnCancelWorking(SpeedyAPI.CancelWorkingResultEnum Result, int Count)
        {
            toolStripStatusLabel1.Text = Result.ToString();
            toolStripStatusLabel3.Text = "Cancel [" + Convert.ToString(Count) + "] working orders.";
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void FOrderConnection_OnLogonReply(string ReplyMessage, SpeedyAPI.LogonResultEnum Result, int ConnectionID)
        {
            toolStripStatusLabel1.Text = ReplyMessage;
            toolStripStatusLabel3.Text = "Version[" + FOrderConnection.Version + "] Date[" + FOrderConnection.BuildDate + "] ConnectionID:" + Convert.ToString(ConnectionID);

            if (Result == SpeedyAPI.LogonResultEnum.lrOK)
            {
                Program.FIsAccept = true;
                Program.FConnectionID = ConnectionID;
                EnableMarketBtns(true);
                this.Orderpanel.Enabled = true;
                this.panelFilter.Enabled = true;
                this.Disconnectbutton.Enabled = true;
                this.FRecover.Enabled = true;
                this.CancelWorkingButton.Enabled = true;
                this.Connectbutton.Enabled = false;
                if (this.FOrderConnection.IsAdministrator == true)
                    this.Text = "Admin:" + Program.FLogonID;
                else
                    this.Text = "User:" + Program.FLogonID;
                AccounttextBox.Text = Program.FLogonAccount;
                Clear();
                FRecoverCount = 0;
                EventArgs e = new EventArgs();
                FMarket_SelectedIndexChanged(FutButton, e);
            }
            else
            {
                Program.FIsAccept = false;
                System.Windows.Forms.MessageBox.Show(ReplyMessage);
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void FOrderConnection_OnDisconnected()
        {
            EnableMarketBtns(false);
            this.Orderpanel.Enabled = false;
            this.panelFilter.Enabled = false;
            this.Disconnectbutton.Enabled = false;
            this.FRecover.Enabled = false;
            this.CancelWorkingButton.Enabled = false;
            this.Connectbutton.Enabled = true;

            Clear();
            toolStripStatusLabel1.Text = "Connection failed!";
            FOrderConnection.Disconnect();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void FOrderConnection_OnConnected()
        {
            FLogon.ShowDialog();
            WriteSetting();
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
            FGrid.Cols[ColNo].Caption = "Exchange";
            FGrid.Cols[ColNo].Name = "Exchange";
            FGrid.Cols[ColNo].Width = 80;
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
            FOrderType.Items.Add(SpeedyAPI.OrderTypeEnum.otMarket);
            FOrderType.Items.Add(SpeedyAPI.OrderTypeEnum.otLimit);
            FOrderType.Items.Add(SpeedyAPI.OrderTypeEnum.otMarketWithProtection);

            FTimeInForce.Items.Clear();
            FTimeInForce.Items.Add(SpeedyAPI.TimeInForceEnum.tifNone);
            FTimeInForce.Items.Add(SpeedyAPI.TimeInForceEnum.tifROD);
            FTimeInForce.Items.Add(SpeedyAPI.TimeInForceEnum.tifIOC);
            FTimeInForce.Items.Add(SpeedyAPI.TimeInForceEnum.tifFOK);

            FSide.Items.Clear();
            FSide.Items.Add(SpeedyAPI.SideEnum.sBuy);
            FSide.Items.Add(SpeedyAPI.SideEnum.sSell);

            PEcomboBox.Items.Clear();
            PEcomboBox.Items.Add(SpeedyAPI.PositionEffectEnum.peOpen);
            PEcomboBox.Items.Add(SpeedyAPI.PositionEffectEnum.peClose);
            PEcomboBox.Items.Add(SpeedyAPI.PositionEffectEnum.peRolled);
            PEcomboBox.Items.Add(SpeedyAPI.PositionEffectEnum.peDayTradeOpen);
            PEcomboBox.Items.Add(SpeedyAPI.PositionEffectEnum.peAuto);

            SessionComboBox.Items.Clear();
            SessionComboBox.Items.Add(SpeedyAPI.TradingSessionIDEnum.tsNormal);
            SessionComboBox.Items.Add(SpeedyAPI.TradingSessionIDEnum.tsOddLot);
            SessionComboBox.Items.Add(SpeedyAPI.TradingSessionIDEnum.tsOffHour);
            SessionComboBox.Items.Add(SpeedyAPI.TradingSessionIDEnum.tsAuction);
            SessionComboBox.Items.Add(SpeedyAPI.TradingSessionIDEnum.tsLend);
            SessionComboBox.Items.Add(SpeedyAPI.TradingSessionIDEnum.tsTender);
            SessionComboBox.Items.Add(SpeedyAPI.TradingSessionIDEnum.tsTenderEx);

            PEcomboBox.SelectedIndex = 0;
            BSComboBox.SelectedIndex = 0;
            AccountComboBox.SelectedIndex = 0;
            MarketComboBox.SelectedIndex = 0;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void FOrderConnection_OnExecutionReport(SpeedyAPI.ExecutionReportMessage Msg, SpeedyAPI.ExecDupEnum PossDup)
        {
            try
            {
                if (PossDup == SpeedyAPI.ExecDupEnum.edPossibleDuplicate || PossDup == SpeedyAPI.ExecDupEnum.edSpeedyGenerate)
                {
                    FRecoverCount++;
                    if (FRecoverCount % 50 == 0)
                        toolStripStatusLabel1.Text = "Recover " + Convert.ToString(FRecoverCount) + " Executions";
                }
                else
                {
                    if (Msg.ConnectionID != Program.FConnectionID)
                    {
                        Console.WriteLine("ConnectionID = " + Msg.ConnectionID);
                        Console.WriteLine("Message:" + Msg.Src);
                    }
                }
                if (Msg.ExecType == SpeedyAPI.ExecTypeEnum.etOrderStatus)
                {
                    OnOrderStatus(Msg);
                    return;
                }

                switch (Msg.OrderStatus)
                {
                    case SpeedyAPI.OrderStatusEnum.osPendingNew: OnPendingNew(Msg); break;
                    case SpeedyAPI.OrderStatusEnum.osPendingCancel: OnPendingCancel(Msg); break;
                    case SpeedyAPI.OrderStatusEnum.osPendingReplace: OnPendingReplace(Msg); break;
                    case SpeedyAPI.OrderStatusEnum.osNew: OnNewOrder(Msg); break;///< Order confirm from Exchange.                             
                    case SpeedyAPI.OrderStatusEnum.osQuoteAccept: OnQuoteAccepted(Msg); break;///< Quote confirm from Exchange.                             
                    case SpeedyAPI.OrderStatusEnum.osFilled:
                    case SpeedyAPI.OrderStatusEnum.osPartiallyFilled: OnPartiallyFilled(Msg.OrderID, Msg.NID, Msg.OrderQty, Msg.Side, Msg.Price, Msg.Data); break;
                    case SpeedyAPI.OrderStatusEnum.osReplaced: OnReplaced(Msg); break;
                    case SpeedyAPI.OrderStatusEnum.osCanceled: OnCanceled(Msg); break;
                    case SpeedyAPI.OrderStatusEnum.osRejected: OnRejected(Msg); break;
                }
            }
            catch (System.Exception e)
            {
                Console.WriteLine(e.ToString());
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FMarket_SelectedIndexChanged(object sender, EventArgs e)
        {
            SelectedMarket(sender);
            if (sender == FutButton)
            {
                SessionComboBox.Enabled = false;
                StockSeqTextBox.Enabled = false;
                FMaturityMonthYear.Text = "";
                FMaturityMonthYear.Enabled = false;
                FOrderType.Enabled = true;
                if (FWhichAPI == SpeedyAPI.APIEnum.aeSpeedy)
                {
                    FSymbol.Text = Program.FUTSymbol;
                    FPrice.Text = Program.FUTPx;
                    FTimeInForce.Enabled = true;
                    FExchange.Text = "TAIFEX";
                    FExchange.Enabled = false;
                }
                else
                {
                    FSymbol.Text = Program.CNFUTSymbol;
                    FPrice.Text = Program.CNFUTPx;
                    FTimeInForce.Enabled = false;
                    FExchange.Text = "CFFEX";
                    FExchange.Enabled = false;
                }
            }
            else if (sender == OptButton)
            {
                SessionComboBox.Enabled = false;
                FTimeInForce.Enabled = true;
                FOrderType.Enabled = true;
                StockSeqTextBox.Enabled = false;
                FMaturityMonthYear.Text = "";
                FMaturityMonthYear.Enabled = false;
                if (FWhichAPI == SpeedyAPI.APIEnum.aeSpeedy)
                {
                    FSymbol.Text = Program.OPTSymbol;
                    FPrice.Text = Program.OPTPx;
                    FExchange.Text = "TAIFEX";
                    FExchange.Enabled = false;
                }
                else
                {
                    FSymbol.Text = Program.CNOPTSymbol;
                    FPrice.Text = Program.CNOPTPx;
                    FExchange.Text = "CFFEX";
                    FExchange.Enabled = true;
                }
            }
            else if (sender == TseButton)
            {
                SessionComboBox.Enabled = true;
                FTimeInForce.Enabled = false;
                FOrderType.Enabled = false;
                StockSeqTextBox.Enabled = true;
                FExchange.Text = "TWSE";
                FExchange.Enabled = false;
                FMaturityMonthYear.Text = "";
                FMaturityMonthYear.Enabled = false;
                FSymbol.Text = Program.TSESymbol;
                FPrice.Text = Program.TSEPx;
            }
            else if (sender == PATSButton)
            {
                SessionComboBox.Enabled = true;
                FTimeInForce.Enabled = false;
                FOrderType.Enabled = false;
                StockSeqTextBox.Enabled = true;
                FExchange.Enabled = true;
                FMaturityMonthYear.Enabled = true;
                FMaturityMonthYear.Text = Program.PATSMMY;
                FExchange.Text = Program.PATSEx;
                FSymbol.Text = Program.PATSSymbol;
                FPrice.Text = Program.PATSPx;
            }
            else if (sender == PATSOButton)
            {
                SessionComboBox.Enabled = true;
                FTimeInForce.Enabled = false;
                FOrderType.Enabled = false;
                StockSeqTextBox.Enabled = true;
                FExchange.Enabled = true;
                FMaturityMonthYear.Enabled = true;
                FMaturityMonthYear.Text = Program.PATSMMY;
                FExchange.Text = Program.PATSEx;
                FSymbol.Text = Program.PATSSymbol;
                FPrice.Text = Program.PATSPx;
            }
            else
            {
                SessionComboBox.Enabled = true;
                FTimeInForce.Enabled = false;
                FOrderType.Enabled = false;
                StockSeqTextBox.Enabled = true;
                FExchange.Text = "OTC";
                FExchange.Enabled = false;
                FMaturityMonthYear.Text = "";
                FMaturityMonthYear.Enabled = false;
                FSymbol.Text = Program.OTCSymbol;
                FPrice.Text = Program.OTCPx;
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void TOrderForm_Load(object sender, EventArgs e)
        {
            FOrderType.SelectedIndex = 1;
            FSide.SelectedIndex = 0;
            FTimeInForce.SelectedIndex = 1;
            SessionComboBox.SelectedIndex = 0;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FNewOrder_Click(object sender, EventArgs e)
        {
            int TotalQty = Program.ConvertStringToInt32(FQty.Text);
            int SliceQty = Program.ConvertStringToInt32(FSliceQty.Text);
            int Count = TotalQty / SliceQty;
            int LastQty = TotalQty % SliceQty + SliceQty;

            for (int i = 0; i < Count; i++)
            {
                long BeginTick = GetTick();
                long UIUseTick, UseUS;

                FNewOrderMsg.AE = Program.FLogonID;
                FNewOrderMsg.Account = this.AccounttextBox.Text;
                FNewOrderMsg.Symbol = FSymbol.Text;
                FNewOrderMsg.OrderType = (SpeedyAPI.OrderTypeEnum)(FOrderType.SelectedItem);
                FNewOrderMsg.PositionEffect = (SpeedyAPI.PositionEffectEnum)(PEcomboBox.SelectedItem);
                FNewOrderMsg.Price = Program.ConvertStringToDouble(FPrice.Text);
                FNewOrderMsg.Side = (SpeedyAPI.SideEnum)(FSide.SelectedItem);
                FNewOrderMsg.TradingSessionID = (SpeedyAPI.TradingSessionIDEnum)(SessionComboBox.SelectedItem);
                FNewOrderMsg.Data = Program.UserData;
                if (i == Count - 1)
                    FNewOrderMsg.OrderQty = LastQty;
                else
                    FNewOrderMsg.OrderQty = SliceQty;
                if (FutButton.Checked == true)
                {
                    if (FWhichAPI == SpeedyAPI.APIEnum.aeSpeedy)
                    {
                        FNewOrderMsg.Market = SpeedyAPI.MarketEnum.mFutures;
                        Program.FUTSymbol = FSymbol.Text;
                        Program.FUTPx = FPrice.Text;
                    }
                    else
                    {
                        FNewOrderMsg.Market = SpeedyAPI.MarketEnum.mCNFutures;
                        Program.CNFUTSymbol = FSymbol.Text;
                        Program.CNFUTPx = FPrice.Text;
                    }
                    FNewOrderMsg.TimeInForce = (SpeedyAPI.TimeInForceEnum)(FTimeInForce.SelectedItem);
                    FNewOrderMsg.StrikePrice = 0;
                    FNewOrderMsg.OrderSource = SpeedyAPI.OrderSourceEnum.osMobile;
                    FNewOrderMsg.MarketDataSource = "SWM";
                }
                else if (OptButton.Checked == true)
                {
                    if (FWhichAPI == SpeedyAPI.APIEnum.aeSpeedy)
                    {
                        FNewOrderMsg.Market = SpeedyAPI.MarketEnum.mOptions;
                        Program.OPTSymbol = FSymbol.Text;
                        Program.OPTPx = FPrice.Text;
                    }
                    else
                    {
                        FNewOrderMsg.Market = SpeedyAPI.MarketEnum.mCNOptions;
                        Program.CNOPTSymbol = FSymbol.Text;
                        Program.CNOPTPx = FPrice.Text;
                    }
                    FNewOrderMsg.TimeInForce = (SpeedyAPI.TimeInForceEnum)(FTimeInForce.SelectedItem);
                    FNewOrderMsg.OrderSource = SpeedyAPI.OrderSourceEnum.osMobile;
                    FNewOrderMsg.MarketDataSource = "SWM";
                }
                else if (TseButton.Checked == true)
                {
                    FNewOrderMsg.Market = SpeedyAPI.MarketEnum.mTSE;
                    FNewOrderMsg.TSEStockSeqNo = Program.ConvertStringToInt32(StockSeqTextBox.Text);
                    FNewOrderMsg.TSEOrderType = "0";
                    Program.TSESymbol = FSymbol.Text;
                    Program.TSEPx = FPrice.Text;
                }
                else if (PATSButton.Checked == true)
                {
                    FNewOrderMsg.Market = SpeedyAPI.MarketEnum.mPATSFutures;
                    FNewOrderMsg.ExchangeCode = FExchange.Text;
                    FNewOrderMsg.MaturityMonthYear = FMaturityMonthYear.Text;
                    FNewOrderMsg.Account = Program.FPATSAccount;
                    FNewOrderMsg.TimeInForce = SpeedyAPI.TimeInForceEnum.tifROD;
                    Program.PATSEx = FExchange.Text;
                    Program.PATSMMY = FMaturityMonthYear.Text;
                    Program.PATSSymbol = FSymbol.Text;
                    Program.PATSPx = FPrice.Text;
                }
                else if (PATSOButton.Checked == true)
                {
                    FNewOrderMsg.Market = SpeedyAPI.MarketEnum.mPATSOptions;
                    FNewOrderMsg.ExchangeCode = FExchange.Text;
                    FNewOrderMsg.MaturityMonthYear = FMaturityMonthYear.Text;
                    FNewOrderMsg.Account = Program.FPATSAccount;
                    FNewOrderMsg.TimeInForce = SpeedyAPI.TimeInForceEnum.tifROD;
                    FNewOrderMsg.StrikePrice = 123.45;
                    Program.PATSEx = FExchange.Text;
                    Program.PATSMMY = FMaturityMonthYear.Text;
                    Program.PATSSymbol = FSymbol.Text;
                    Program.PATSPx = FPrice.Text;
                }
                else
                {
                    FNewOrderMsg.Market = SpeedyAPI.MarketEnum.mOTC;
                    FNewOrderMsg.TSEStockSeqNo = Program.ConvertStringToInt32(StockSeqTextBox.Text); ;

                    Program.OTCSymbol = FSymbol.Text;
                    Program.OTCPx = FPrice.Text;
                }
                FNewOrderMsg.NID = FOrderConnection.GenerateUniqueID(FNewOrderMsg.Market, SpeedyAPI.MessageTypeEnum.mtNew);
                UseUS = ((GetTick() - BeginTick) * 1000000) / (long)FFreq;
                Console.WriteLine("-----------------------------------------------------------------");
                Console.WriteLine(" Fill Fields use:" + Convert.ToString(UseUS) + " us");
                FNIDSet.Add(FNewOrderMsg.NID, null);

                try
                {
                    UIUseTick = AddNewOrderRow(FNewOrderMsg, FNewOrderMsg.NID);
                    Console.WriteLine(" Update  UI  use:" + Convert.ToString((UIUseTick * 1000000) / (long)FFreq) + " us");

                    BeginTick = GetTick();
                    FOrderConnection.NewOrder(FNewOrderMsg);
                    UseUS = ((GetTick() - BeginTick) * 1000000) / (long)FFreq;
                    Console.WriteLine(" Send Order  use:" + Convert.ToString(UseUS) + " us");

                }
                catch (Exception ex)
                {
                    System.Windows.Forms.MessageBox.Show(ex.Message);
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void Clear()
        {
            FNIDHash.Clear();
            FOrderIDHash.Clear();
            FAccountSet.Clear();
            FNIDSet.Clear();
            FGrid.Rows.RemoveRange(1, FGrid.Rows.Count - 1);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void TOrderForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (SendOrderTimer.Enabled == true)
            {
                SendOrderTimer.Enabled = false;
                FOrderConnection.Disconnect();
            }
            this.WriteSetting();
            FLogon.Dispose();
            FOrderConnection.Destroy();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        long GetTick()
        {
            long CurrentTick;
            QueryPerformanceCounter(out CurrentTick);
            return CurrentTick;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnOrderStatus(SpeedyAPI.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByNID(Msg.NID);

            if (FoundRow != null)
            {
                long UseMS = ((GetTick() - (long)FoundRow.UserData) * 1000) / FFreq;

                FoundRow["Symbol"] = Msg.Symbol;
                FoundRow["OrderID"] = Msg.OrderID;
                FoundRow["Data"] = Msg.Data;
                FoundRow["BrokerID"] = Msg.BrokerID;
                FoundRow["OrderStatus"] = Msg.OrderStatus;
                FoundRow["Text"] = "OrderStatus";
                FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                FoundRow["UseTime"] = UseMS;
                Console.WriteLine("Order Status Request NID:" + Convert.ToString(Msg.NID));
            }
            else
                Console.WriteLine("Order Status Request NID:" + Convert.ToString(Msg.NID) + " not found!");
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnPendingNew(SpeedyAPI.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByNID(Msg.NID);

            if (FoundRow != null)
            {
                FoundRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osPendingNew;
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
            }
            else
                Console.WriteLine("Pending new NID:" + Convert.ToString(Msg.NID) + " not found!");
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnPendingCancel(SpeedyAPI.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(Msg.OrderID);

            if (FoundRow != null)
            {
                FoundRow.UserData = GetTick();
                SpeedyAPI.OrderStatusEnum OrderStatus = (SpeedyAPI.OrderStatusEnum)(FoundRow["OrderStatus"]);
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
                if (OrderStatus != SpeedyAPI.OrderStatusEnum.osQuoteAccept)
                    FoundRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osPendingCancel;
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnPendingReplace(SpeedyAPI.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(Msg.OrderID);
            if (FoundRow != null)
            {
                FoundRow.UserData = GetTick();
                FoundRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osPendingReplace;
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnNewOrder(SpeedyAPI.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByNID(Msg.NID);

            if (FoundRow != null)///< Order from this order client
            {
                SpeedyAPI.OrderStatusEnum OldOrderStatus = (SpeedyAPI.OrderStatusEnum)FoundRow["OrderStatus"];
                NumberFormatInfo provider = new NumberFormatInfo();

                long UseMS = ((GetTick() - (long)FoundRow.UserData) * 1000) / FFreq;
                FoundRow["UseTime"] = UseMS;
                FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                FoundRow["OrderID"] = Msg.OrderID;
                FoundRow["IsOrder"] = true;
                FoundRow["BrokerID"] = Msg.BrokerID;
                if (OldOrderStatus == SpeedyAPI.OrderStatusEnum.osPendingNew)
                {
                    FOrderIDHash[Msg.OrderID] = FoundRow; ///< Add to OrderID map
                    FoundRow["Symbol"] = Msg.Symbol;
                    FoundRow["Data"] = Msg.Data;
                    //FoundRow["BrokerID"] = Msg.BrokerID;
                    FoundRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osNew;
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
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnQuoteAccepted(SpeedyAPI.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByNID(Msg.NID);

            if (FoundRow != null)  ///< Quote from this order client
            {
                SpeedyAPI.OrderStatusEnum OldOrderStatus = (SpeedyAPI.OrderStatusEnum)FoundRow["OrderStatus"];
                long UseMS = 0;

                if (FoundRow.UserData != null)
                    UseMS = ((GetTick() - (long)FoundRow.UserData) * 1000000) / FFreq;
                FoundRow["UseTime"] = UseMS;
                FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                FoundRow["OrderID"] = Msg.OrderID;
                FoundRow["IsOrder"] = false;
                if (OldOrderStatus == SpeedyAPI.OrderStatusEnum.osPendingNew)
                {
                    FOrderIDHash[Msg.OrderID] = FoundRow;
                    FoundRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osQuoteAccept;
                    if (Msg.Data != null)
                        FoundRow["Data"] = Msg.Data;
                }
            }
            else ///< Quote from other people or FIX
            {
                if (FNIDSet.ContainsKey(Msg.NID) == false)
                    AddNewRowByNewOrderExecutionReport(Msg, false);
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnPartiallyFilled(string OrderID, long NID, int MatchQty, SpeedyAPI.SideEnum Side, double Price, string Data)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(OrderID);

            if (FoundRow == null) ///< Search by OrderID
            {
                Console.WriteLine("Order:" + OrderID + " not found! try to use NID:" + Convert.ToString(NID));
                FoundRow = SearchByNID(NID); ///< Search by NID
            }
            if (FoundRow != null)
            {
                if (Data != null)
                    FoundRow["Data"] = Data;
                if (MatchQty > 0) ///< Filled or Partially filled
                {
                    FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                    if ((bool)FoundRow["IsOrder"] == false) ///< quote filled.
                    {
                        int BidQty = Convert.ToInt32(FoundRow["BidQty"]);
                        int AskQty = Convert.ToInt32(FoundRow["AskQty"]);
                        int BidCumQty = Convert.ToInt32(FoundRow["BidCumQty"]);
                        int AskCumQty = Convert.ToInt32(FoundRow["AskCumQty"]);
                        switch (Side)
                        {
                            case SpeedyAPI.SideEnum.sBuy:
                                BidCumQty += MatchQty;
                                FoundRow["BidCumQty"] = BidCumQty;
                                break;
                            case SpeedyAPI.SideEnum.sSell:
                                AskCumQty += MatchQty;
                                FoundRow["AskCumQty"] = AskCumQty;
                                break;
                        }
                        if ((BidCumQty == BidQty) && (AskCumQty == AskQty)) ///< Filled
                        {
                            FoundRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osFilled;
                            if (ShowFillcheckBox.Checked == false)
                                FoundRow.Visible = false;
                        }
                    }
                    else
                    {
                        int Qty = Convert.ToInt32(FoundRow["Qty"]);
                        int CumQty = Convert.ToInt32(FoundRow["CumQty"]) + MatchQty;

                        if (Qty == CumQty) ///< Filled
                        {
                            FoundRow["Price"] = Convert.ToString(Price);
                            FoundRow["CumQty"] = CumQty;
                            FoundRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osFilled;
                            if (ShowFillcheckBox.Checked == false)
                                FoundRow.Visible = false;
                        }
                        else
                        {
                            FoundRow["Price"] = Convert.ToString(Price);
                            FoundRow["CumQty"] = CumQty;
                            FoundRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osPartiallyFilled;
                        }
                    }
                }
                else
                    RemoveFromGrid(FoundRow);   ///<Canceled.
            }
            else
                Console.WriteLine("Order:" + OrderID + " NID:" + Convert.ToString(NID) + " not found!");
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnReplaced(SpeedyAPI.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(Msg.OrderID);

            if (FoundRow != null)
            {
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
                FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");

                if (Msg.TMPExecType == SpeedyAPI.TMPExecTypeEnum.tetPxReplaced)
                {
                    FoundRow["Price"] = Convert.ToString(Msg.Price);
                    FoundRow["OrderType"] = Msg.OrderType;
                    FoundRow["TimeInForce"] = Msg.TimeInForce;
                }
                else ///< Reduce Qty
                {
                    int OrderQty = Convert.ToInt32(FoundRow["Qty"]);
                    int CumQty = Convert.ToInt32(FoundRow["CumQty"]);
                    int AfterQty = 0;
                    if ((Msg.Market == SpeedyAPI.MarketEnum.mPATSFutures) || (Msg.Market == SpeedyAPI.MarketEnum.mPATSOptions))
                        AfterQty = Msg.OrderQty;
                    else
                        AfterQty = OrderQty - Msg.OrderQty;

                    FoundRow["Qty"] = AfterQty;
                    if (AfterQty - CumQty > 0)
                    {
                        if (CumQty > 0)
                            FoundRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osPartiallyFilled;
                    }
                    else ///< Filled or canceled.
                    {
                        if (CumQty > 0) ///< Filled
                        {
                            FoundRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osFilled;
                            if (ShowFillcheckBox.Checked == false)
                                FoundRow.Visible = false;
                        }
                        else ///< Canceled
                            RemoveFromGrid(FoundRow);
                    }
            }
        }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnCanceled(SpeedyAPI.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(Msg.OrderID);

            if (FoundRow != null)
            {
                int CumQty = Convert.ToInt32(FoundRow["CumQty"]);

                FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
                if (CumQty == 0) ///<Canceled 
                    RemoveFromGrid(FoundRow);
                else
                    HideCanceledPartiallyFillOrder(FoundRow);
            }
            else
            {
                FoundRow = SearchByNID(Msg.NID);
                if (FoundRow != null)
                {
                    int CumQty = Convert.ToInt32(FoundRow["CumQty"]);

                    FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                    if (Msg.Data != null)
                        FoundRow["Data"] = Msg.Data;
                    if (CumQty == 0)
                        RemoveFromGrid(FoundRow);
                    else
                        HideCanceledPartiallyFillOrder(FoundRow);
                    SpeedyAPI.OrderStatusEnum OldOrderStatus = (SpeedyAPI.OrderStatusEnum)FoundRow["OrderStatus"];
                    toolStripStatusLabel1.Text = "Cancelled at status: " + OldOrderStatus.ToString() + " OID:" + Msg.OrderID;
                }
                toolStripLabel1.Invalidate();
            }
            toolStripStatusLabel1.Text = "Cancelled qty: " + Msg.OrderQty;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnRejected(SpeedyAPI.ExecutionReportMessage Msg)
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
                    case SpeedyAPI.CxlRejResponseToEnum.crrNew:
                    case SpeedyAPI.CxlRejResponseToEnum.crrQuote:
                    case SpeedyAPI.CxlRejResponseToEnum.crrOrderStatus:
                        FoundRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osRejected;
                        FoundRow["OrderID"] = Msg.OrderID;
                        break;
                    default: FoundRow["OrderID"] = Msg.OrderID;
                        break;
                }
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void HideCanceledPartiallyFillOrder(C1.Win.C1FlexGrid.Row HideRow)
        {
            HideRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osCanceled;

            if (ShowFillcheckBox.Checked == false)
                HideRow.Visible = false;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void RemoveFromGrid(C1.Win.C1FlexGrid.Row RemoveRow)
        {

            if (RemoveRow["NID"] != null)
            {
                long NID = Convert.ToInt64(RemoveRow["NID"]);
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
        private long AddNewOrderRow(SpeedyAPI.NewOrderMessage Msg, long NID)
        {
            long BeginTick = GetTick();
            long EndTick;

            FGrid.BeginUpdate();
            C1.Win.C1FlexGrid.Row NewRow = FGrid.Rows.Add();
            FGrid.ShowCell(FGrid.Rows.Count - 1, 0);
            foreach (C1.Win.C1FlexGrid.Row SelRow in FGrid.Rows.Selected)
                SelRow.Selected = false;
            NewRow.Selected = true;

            NewRow["AE"] = Msg.AE;
            NewRow["Account"] = Msg.Account;
            NewRow["Exchange"] = Msg.ExchangeCode;
            NewRow["Symbol"] = Msg.Symbol;
            NewRow["OrderID"] = "";
            NewRow["Side"] = Msg.Side;
            NewRow["Price"] = Msg.Price;
            NewRow["Qty"] = Msg.OrderQty;
            NewRow["CumQty"] = 0;
            NewRow["OrderType"] = Msg.OrderType;
            NewRow["Market"] = Msg.Market;
            NewRow["Text"] = "";
            NewRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osPendingNew;
            NewRow["TradingSession"] = Msg.TradingSessionID;
            NewRow["MaturityMonthYear"] = Msg.MaturityMonthYear;
            switch (Msg.Market)
            {
                case SpeedyAPI.MarketEnum.mFutures:
                    NewRow["StrikePrice"] = 0.0;
                    NewRow["TimeInForce"] = Msg.TimeInForce;
                    break;
                case SpeedyAPI.MarketEnum.mOptions:
                    NewRow["StrikePrice"] = Msg.StrikePrice;
                    NewRow["TimeInForce"] = Msg.TimeInForce;
                    break;
                case SpeedyAPI.MarketEnum.mPATSFutures:
                    NewRow["StrikePrice"] = 0.0;
                    NewRow["TimeInForce"] = Msg.TimeInForce;
                    break;
                case SpeedyAPI.MarketEnum.mPATSOptions:
                    NewRow["StrikePrice"] = Msg.StrikePrice;
                    NewRow["TimeInForce"] = Msg.TimeInForce;
                    break;
            }
            NewRow["NID"] = NID;
            FNIDHash[Msg.NID] = NewRow;
            CheckVisable(NewRow);
            CheckAccount(Msg.Account);
            toolStripStatusLabel2.Text = Convert.ToSingle(FGrid.Rows.Count - 1) + " Order records";
            FGrid.EndUpdate();
            EndTick = GetTick();
            NewRow.UserData = EndTick;
            return EndTick - BeginTick;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        C1.Win.C1FlexGrid.Row AddNewRowByNewOrderExecutionReport(SpeedyAPI.ExecutionReportMessage Msg, bool IsOrder)
        {
            C1.Win.C1FlexGrid.Row NewRow = FGrid.Rows.Add();
            FGrid.ShowCell(FGrid.Rows.Count - 1, 0);
            toolStripStatusLabel2.Text = Convert.ToSingle(FGrid.Rows.Count - 1) + " Order records";
            long NID = Msg.NID;

            NewRow.UserData = GetTick();
            NewRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
            NewRow["AE"] = Msg.AE;
            NewRow["Account"] = Msg.Account;
            NewRow["Exchange"] = Msg.ExchangeCode;
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
            if (Msg.Data != null)
                NewRow["Data"] = Msg.Data;
            if (NID != 0)
                FNIDHash[¡@NID¡@] = NewRow;
            FNIDSet.Add(NID, null);
            if (Msg.OrderID.Length > 0)
                FOrderIDHash[Msg.OrderID] = NewRow;
            CheckVisable(NewRow);
            CheckAccount(Msg.Account);
            return NewRow;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        C1.Win.C1FlexGrid.Row SearchByNID(long NID)
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
        private void FGrid_KeyDown(object sender, KeyEventArgs e)
        {
            if (Program.FReadOnly == "true")
                return;
            switch (e.KeyCode)
            {
                case Keys.Delete:
                case Keys.C:
                    for (int i = FGrid.Rows.Selected.Count - 1; i >= 0; i--)
                    {
                        C1.Win.C1FlexGrid.Row CancelRow = FGrid.Rows.Selected[i];
                        SpeedyAPI.OrderStatusEnum OrderStatus = (SpeedyAPI.OrderStatusEnum)(CancelRow["OrderStatus"]);
                        switch (OrderStatus)
                        {
                            case SpeedyAPI.OrderStatusEnum.osPendingNew:
                            case SpeedyAPI.OrderStatusEnum.osPendingReplace:
                            case SpeedyAPI.OrderStatusEnum.osRejected: RemoveFromGrid(CancelRow); break;
                            case SpeedyAPI.OrderStatusEnum.osQuoteAccept: QuoteCancel(CancelRow); break;
                            case SpeedyAPI.OrderStatusEnum.osNew:
                            case SpeedyAPI.OrderStatusEnum.osFilled:
                            case SpeedyAPI.OrderStatusEnum.osPartiallyFilled:
                            case SpeedyAPI.OrderStatusEnum.osReplaced: OrderCancel(CancelRow); break;
                            case SpeedyAPI.OrderStatusEnum.osCanceled:
                            case SpeedyAPI.OrderStatusEnum.osExpired:
                            default: break;
                        }
                    }
                    break;

                case Keys.R:
                    if (FGrid.Rows.Selected.Count > 0)
                        OrderReplace(FGrid.Rows.Selected[0]);
                    break;
                case Keys.Q:
                    Quote();
                    break;
                case Keys.S:
                    C1.Win.C1FlexGrid.Row QueryRow = FGrid.Rows.Selected[0];
                    if (QueryRow != null)
                        OrderStatusRequest(QueryRow);
                    break;
                case Keys.A:
                    if (e.Control)
                        FGrid.Select(1, 0, FGrid.Rows.Count - 1, FGrid.Cols.Count - 1);
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
        private void FQuoteRequestTimer_Tick(object sender, EventArgs e)
        {
            SpeedyAPI.QuoteRequestMessage Msg = new SpeedyAPI.QuoteRequestMessage();

            Msg.Market = SpeedyAPI.MarketEnum.mOptions;
            Msg.Account = Program.FMMAccount;
            Msg.Symbol = FSymbol.Text;
            FOrderConnection.QuoteRequest(Msg);
            Msg = null;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OrderReplace(C1.Win.C1FlexGrid.Row ReplaceRow)
        {
            SpeedyAPI.OrderStatusEnum OrderStatus = (SpeedyAPI.OrderStatusEnum)(ReplaceRow["OrderStatus"]);
            if ((OrderStatus == SpeedyAPI.OrderStatusEnum.osNew) || (OrderStatus == SpeedyAPI.OrderStatusEnum.osPartiallyFilled))
            {

                SpeedyAPI.MarketEnum Market = (SpeedyAPI.MarketEnum)(ReplaceRow["Market"]);
                double CurPrice = Program.ConvertStringToDouble(ReplaceRow["Price"].ToString());
                int QtyNow = Convert.ToInt32(ReplaceRow["Qty"]);
                bool IsLimitROD = false;
                bool IsPartiallFill = true;

                if (Market != SpeedyAPI.MarketEnum.mOTC && Market != SpeedyAPI.MarketEnum.mTSE)
                {
                    if (SpeedyAPI.TimeInForceEnum.tifROD == (SpeedyAPI.TimeInForceEnum)(ReplaceRow["TimeInForce"]) &&
                        SpeedyAPI.OrderTypeEnum.otLimit == (SpeedyAPI.OrderTypeEnum)(ReplaceRow["OrderType"]))
                        IsLimitROD = true;
                    if (Convert.ToInt32(ReplaceRow["CumQty"]) == 0)
                        IsPartiallFill = false;
                }
                TReplaceForm ReplaceForm = new TReplaceForm(Market, QtyNow, CurPrice, IsLimitROD, IsPartiallFill);

                if (ReplaceForm.ShowDialog() == DialogResult.OK)
                {
                    SpeedyAPI.ReplaceOrderMessage Msg = new SpeedyAPI.ReplaceOrderMessage();

                    Msg.Market = Market;
                    Msg.Account = ReplaceRow["Account"].ToString();
                    Msg.OrderID = ReplaceRow["OrderID"].ToString();
                    Msg.ExchangeCode = ReplaceRow["Exchange"].ToString();
                    Msg.Symbol = ReplaceRow["Symbol"].ToString();
                    if( ReplaceRow["MaturityMonthYear"] != null )
                        Msg.MaturityMonthYear = ReplaceRow["MaturityMonthYear"].ToString();
                    Msg.BrokerID = ReplaceRow["BrokerID"].ToString();
                    Msg.Side = (SpeedyAPI.SideEnum)(ReplaceRow["Side"]);
                    Msg.PositionEffect = SpeedyAPI.PositionEffectEnum.peOpen;
                    Msg.Data = Program.UserData;
                    if (ReplaceForm.IsReplaceQty() == true)
                    {
                        Msg.OrderType = (SpeedyAPI.OrderTypeEnum)(ReplaceRow["OrderType"]);
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
                    long BeginTick = GetTick();

                    FOrderConnection.ReplaceOrder(Msg);
                    long UseMS = ((GetTick() - BeginTick) * 1000000) / (long)FFreq;
                    Console.WriteLine("Replace Order use:" + Convert.ToString(UseMS) + " us");
                    long NID = Msg.NID;
                    if (NID != 0)
                        FNIDHash[NID] = ReplaceRow;
                    Msg = null;
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OrderCancel(C1.Win.C1FlexGrid.Row CancelRow)
        {

            SpeedyAPI.CancelOrderMessage Msg = new SpeedyAPI.CancelOrderMessage();
            String OrderID = CancelRow["OrderID"].ToString(); ///< Cancel Order by OrderID.   
            long BeginTick = GetTick();

            Msg.Market = (SpeedyAPI.MarketEnum)(CancelRow["Market"]);
            Msg.Account = CancelRow["Account"].ToString();
            Msg.OrderID = OrderID;// "z" + OrderID.Substring(1, 4);
            Msg.ExchangeCode = CancelRow["Exchange"].ToString();
            Msg.Symbol = CancelRow["Symbol"].ToString();            
            Msg.Price = Program.ConvertStringToDouble(CancelRow["Price"].ToString());
            Msg.Side = (SpeedyAPI.SideEnum)(CancelRow["Side"]);
            Msg.OrderType = (SpeedyAPI.OrderTypeEnum)(CancelRow["OrderType"]);
            Msg.TradingSessionID = (SpeedyAPI.TradingSessionIDEnum)(CancelRow["TradingSession"]);
            if (CancelRow["MaturityMonthYear"] != null)
                Msg.MaturityMonthYear = CancelRow["MaturityMonthYear"].ToString();

            switch (Msg.Market)
            {
                case SpeedyAPI.MarketEnum.mFutures:
                    if (CancelRow["MaturityMonthYear"] != null)
                        Msg.MaturityMonthYear = CancelRow["MaturityMonthYear"].ToString();
                    Msg.StrikePrice = Convert.ToDouble(CancelRow["StrikePrice"]);
                    Msg.Data = Program.UserData;
                    break;
                case SpeedyAPI.MarketEnum.mOptions:
                    Msg.TimeInForce = (SpeedyAPI.TimeInForceEnum)(CancelRow["TimeInForce"]);
                    Msg.Data = Program.UserData;
                    break;
                case SpeedyAPI.MarketEnum.mOTC:
                    Msg.Data = Program.UserData;
                    break;
                case SpeedyAPI.MarketEnum.mTSE:
                    Msg.Data = Program.UserData;
                    break;
            }

            FOrderConnection.CancelOrder(Msg);
            long UseMS = ((GetTick() - BeginTick) * 1000000) / (long)FFreq;
            Console.WriteLine("Cancel Order use:" + Convert.ToString(UseMS) + " us");
            FNIDHash[Msg.NID] = CancelRow;
            Msg = null;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OrderStatusRequest(C1.Win.C1FlexGrid.Row QueryRow)
        {
            SpeedyAPI.OrderStatusRequestMessage Msg = new SpeedyAPI.OrderStatusRequestMessage();

            Msg.Market = (SpeedyAPI.MarketEnum)(QueryRow["Market"]);
            Msg.Account = QueryRow["Account"].ToString();
            Msg.OrderID = QueryRow["OrderID"].ToString(); ///< Cancel Order by OrderID.  
            Msg.Symbol = QueryRow["Symbol"].ToString();
            Msg.Price = Program.ConvertStringToDouble(QueryRow["Price"].ToString());
            Msg.Side = (SpeedyAPI.SideEnum)(QueryRow["Side"]);
            Msg.TradingSessionID = (SpeedyAPI.TradingSessionIDEnum)(QueryRow["TradingSession"]);
            Msg.Data = "OrderStatusRequest";
            FOrderConnection.OrderStatusRequest(Msg);
            long NID = Msg.NID;
            if (NID != 0)
                FNIDHash[NID] = QueryRow;
            Console.WriteLine(" ------ Order Sataus Request  NID:" + Convert.ToString(NID));
            Msg = null;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void Quote()
        {
            TQuoteForm QuoteForm = new TQuoteForm();
            QuoteForm.SetSymbol(FSymbol.Text);
            QuoteForm.SetBidPrice(Program.ConvertStringToDouble(FPrice.Text));
            if (QuoteForm.ShowDialog() == DialogResult.OK)
            {
                SpeedyAPI.QuoteMessage Msg = new SpeedyAPI.QuoteMessage();

                if (FutButton.Checked == true)
                    Msg.Market = SpeedyAPI.MarketEnum.mFutures;
                else if (OptButton.Checked == true)
                    Msg.Market = SpeedyAPI.MarketEnum.mOptions;

                Msg.Account = Program.FMMAccount;
                Msg.AE = Program.FLogonID;
                Msg.Symbol = QuoteForm.GetSymbol();
                Msg.BidPrice = QuoteForm.GetBidPrice();
                Msg.BidQty = QuoteForm.GetBidQty();
                Msg.AskPrice = QuoteForm.GetAskPrice();
                Msg.AskQty = QuoteForm.GetAskQty();
                Msg.TimeInForce = QuoteForm.GetTimeInForce();

                int BidQty = Msg.BidQty;
                int AskQty = Msg.AskQty;
                C1.Win.C1FlexGrid.Row NewRow = FGrid.Rows.Add();
                FGrid.ShowCell(FGrid.Rows.Count - 1, 0);
                toolStripStatusLabel2.Text = Convert.ToSingle(FGrid.Rows.Count - 1) + " Order records";
                //foreach (C1.Win.C1FlexGrid.Row SelRow in FGrid.Rows.Selected)
                //    SelRow.Selected = false;                
                NewRow.Selected = true;
                NewRow["AE"] = Msg.AE;
                NewRow["Market"] = Msg.Market;
                NewRow["Symbol"] = Msg.Symbol;
                NewRow["BidPrice"] = Msg.BidPrice;
                NewRow["BidQty"] = BidQty;
                NewRow["BidCumQty"] = 0;
                NewRow["AskPrice"] = Msg.AskPrice;
                NewRow["AskQty"] = AskQty;
                NewRow["AskCumQty"] = 0;
                NewRow["TimeInForce"] = Msg.TimeInForce;
                NewRow["OrderStatus"] = SpeedyAPI.OrderStatusEnum.osPendingNew;
                Msg.NID = FOrderConnection.GenerateUniqueID(Msg.Market, SpeedyAPI.MessageTypeEnum.mtQuote);
                NewRow["NID"] = Msg.NID;

                Msg.OrderSource = SpeedyAPI.OrderSourceEnum.osMobile;
                Msg.MarketDataSource = "QT9";
                if (Msg.NID != 0)
                    FNIDHash[Msg.NID] = NewRow;
                FOrderConnection.Quote(Msg);
                Msg = null;
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void QuoteCancel(C1.Win.C1FlexGrid.Row CancelRow)
        {
            SpeedyAPI.QuoteCancelMessage Msg = new SpeedyAPI.QuoteCancelMessage();

            Msg.AE = Program.FLogonID;
            Msg.Market = (SpeedyAPI.MarketEnum)CancelRow["Market"];
            Msg.Account = Program.FMMAccount;
            Msg.QuoteID = CancelRow["OrderID"].ToString();
            Msg.Symbol = CancelRow["Symbol"].ToString();
            Msg.AskPrice = Program.ConvertStringToDouble(CancelRow["AskPrice"].ToString());
            Msg.BidPrice = Program.ConvertStringToDouble(CancelRow["BidPrice"].ToString());
            FOrderConnection.QuoteCancel(Msg);
            long NID = Msg.NID;
            if (NID != 0)
                FNIDHash[NID] = CancelRow;
            Msg = null;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void Connectbutton_Click(object sender, EventArgs e)
        {
            if (FIp.Text.Length <= 0)
            {
                MessageBox.Show("Ip.Text.Length <= 0");
                return;
            }
            int Port = Convert.ToInt32(FPort.Text);
            if ((Port <= 0x0000) || (Port > 0xffff))
            {
                MessageBox.Show("(Por <= 0) || (Port > 65535)");
                return;
            }
            FOrderConnection.Connect2(FIp.Text, Port, 2);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FRecover_Click(object sender, EventArgs e)
        {
            string RecoverTime = dateTimePicker.Value.ToString("HHmmss");

            FRecover.Enabled = false;
            Clear();
            FGrid.Visible = false;
            FRecoverCount = 0;
            FOrderConnection.Recover(RecoverTime, SpeedyAPI.RecoverTypeEnum.rtAll, SpeedyAPI.RecoverMarketEnum.rmAll);
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
            if (e.Row < FGrid.Rows.Fixed)
                return;
            object SideCell = FGrid[e.Row, FGrid.Cols["Side"].Index];

            SpeedyAPI.SideEnum Side;
            if (SideCell == null)
                Side = SpeedyAPI.SideEnum.sNone;
            else
                Side = (SpeedyAPI.SideEnum)SideCell;
            SpeedyAPI.OrderStatusEnum Status = (SpeedyAPI.OrderStatusEnum)(FGrid[e.Row, FGrid.Cols["OrderStatus"].Index]);

            switch (Status)
            {
                case SpeedyAPI.OrderStatusEnum.osPendingNew:
                case SpeedyAPI.OrderStatusEnum.osPendingCancel:
                case SpeedyAPI.OrderStatusEnum.osPendingReplace: SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["Pending"]); break;
                case SpeedyAPI.OrderStatusEnum.osRejected: SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["Rejected"]); break;
                case SpeedyAPI.OrderStatusEnum.osNew:
                    if (Side == SpeedyAPI.SideEnum.sBuy)
                        SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["BuyNewOrder"]);
                    else
                        SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["SellNewOrder"]);
                    break;
                case SpeedyAPI.OrderStatusEnum.osPartiallyFilled:
                    if (Side == SpeedyAPI.SideEnum.sBuy)
                        SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["BuyPartialFilled"]);
                    else
                        SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["SellPartialFilled"]);
                    break;
                case SpeedyAPI.OrderStatusEnum.osCanceled:
                case SpeedyAPI.OrderStatusEnum.osFilled:
                    SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["Filled"]);
                    break;
                case SpeedyAPI.OrderStatusEnum.osQuoteAccept:
                    if (Convert.ToInt32(FGrid[e.Row, FGrid.Cols["BidCumQty"].Index].ToString()) > 0)
                        SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["QuotePartialFilled"]);
                    else
                        SetRowStyle(FGrid.Rows[e.Row], FGrid.Styles["NewQuote"]);
                    break;

            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void FQty_TextChanged(object sender, EventArgs e)
        {
            int Total = Program.ConvertStringToInt32(FQty.Text);
            int Slice = Program.ConvertStringToInt32(FSliceQty.Text);
            if (Total <= 0)
            {
                FQty.Text = "1";
                FSliceQty.Text = "1";
                Countlabel.Text = "Count:1";
            }
            else if (Total > 0 && Slice <= 0)
            {
                FSliceQty.Text = "1";
                Countlabel.Text = "Count:" + Convert.ToString(Total);
            }
            else if (Total > 0 && Slice > 0 && Slice > Total)
            {
                FSliceQty.Text = FQty.Text;
                Countlabel.Text = "Count:1";
            }
            else
            {
                Countlabel.Text = "Count:" + Convert.ToString(Total / Slice);
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void button1_Click(object sender, EventArgs e)
        {
            SendOrderTimer.Enabled = !SendOrderTimer.Enabled;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void SendOrderTimer_Tick(object sender, EventArgs e)
        {
            FNewOrder_Click(this, e);
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            SettingForm setting = new SettingForm();
            if (setting.ShowDialog() == DialogResult.OK)
            {
                FOrderConnection.SetBrokerID(SpeedyAPI.MarketEnum.mFutures, Program.FBrokerID);
                FOrderConnection.SetBrokerID(SpeedyAPI.MarketEnum.mTSE, Program.FTSEBrokerID);
                FOrderConnection.SetBrokerID(SpeedyAPI.MarketEnum.mCNFutures, Program.FCNBrokerID);
                FOrderConnection.ClearMemberID = Program.FCMID;
                if (Program.FNewOrder == "true")
                {
                    Orderpanel.Visible = true;
                    FGrid.Top = (Orderpanel.Top + Orderpanel.Height);
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
        private void clockTimer_Tick(object sender, EventArgs e)
        {
            dateTimePicker.Value = DateTime.Now;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void CheckAccount(String Account)
        {
            if (FAccountSet.Contains(Account) == false)
            {
                FAccountSet.Add(Account, null);
                AccountComboBox.Items.Add(Account);
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void CheckVisable(C1.Win.C1FlexGrid.Row Row)
        {
            switch (BSComboBox.SelectedIndex)
            {
                case 0: break;
                case 1: if ((SpeedyAPI.SideEnum)Row["Side"] != SpeedyAPI.SideEnum.sBuy)
                    {
                        Row.Visible = false;
                        return;
                    }
                    break;
                case 2: if ((SpeedyAPI.SideEnum)Row["Side"] != SpeedyAPI.SideEnum.sSell)
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
                case 1: if ((SpeedyAPI.MarketEnum)Row["Market"] != SpeedyAPI.MarketEnum.mFutures)
                    {
                        Row.Visible = false;
                        return;
                    }
                    break;
                case 2: if ((SpeedyAPI.MarketEnum)Row["Market"] != SpeedyAPI.MarketEnum.mOptions)
                    {
                        Row.Visible = false;
                        return;
                    }
                    break;
                case 3: if ((SpeedyAPI.MarketEnum)Row["Market"] != SpeedyAPI.MarketEnum.mTSE)
                    {
                        Row.Visible = false;
                        return;
                    }
                    break;
                case 4: if ((SpeedyAPI.MarketEnum)Row["Market"] != SpeedyAPI.MarketEnum.mOTC)
                    {
                        Row.Visible = false;
                        return;
                    }
                    break;
                default: break;
            }

            if (AccountComboBox.SelectedIndex != 0)
            {
                if (Row["Account"].ToString() != AccountComboBox.Text)
                {
                    Row.Visible = false;
                    return;
                }
            }

            SpeedyAPI.OrderStatusEnum OrderStatus = (SpeedyAPI.OrderStatusEnum)Row["OrderStatus"];
            if (OrderStatus == SpeedyAPI.OrderStatusEnum.osFilled || OrderStatus == SpeedyAPI.OrderStatusEnum.osCanceled)
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
        private void button2_Click(object sender, EventArgs e)
        {
            SpeedyAPI.QuoteRequestMessage Msg = new SpeedyAPI.QuoteRequestMessage();

            if (FutButton.Checked == true)
                Msg.Market = SpeedyAPI.MarketEnum.mFutures;
            else if (OptButton.Checked == true)
                Msg.Market = SpeedyAPI.MarketEnum.mOptions;
            Msg.Account = Program.FMMAccount;
            Msg.Symbol = FSymbol.Text;
            FOrderConnection.QuoteRequest(Msg);
            Msg = null;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void button2_Click_1(object sender, EventArgs e)
        {
            SpeedyAPI.QuoteRequestMessage Msg = new SpeedyAPI.QuoteRequestMessage();

            Msg.Market = SpeedyAPI.MarketEnum.mFutures;
            Msg.Account = Program.FMMAccount;
            Msg.Symbol = FSymbol.Text;
            FOrderConnection.QuoteRequest(Msg);
            Msg = null;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
        private void CancelWorkingButton_Click(object sender, EventArgs e)
        {
            SpeedyClient.CancelWorkingForm CxlWorkingForm = new SpeedyClient.CancelWorkingForm();

            if (CxlWorkingForm.ShowDialog() == DialogResult.OK)
            {
                FOrderConnection.CancelWorking(CxlWorkingForm.GetMarket(),
                                                CxlWorkingForm.AETextBox.Text,
                                                CxlWorkingForm.AccountTextBox.Text,
                                                CxlWorkingForm.DataTextBox.Text,
                                                CxlWorkingForm.ProdIDTextBox.Text);
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------      
    }
}