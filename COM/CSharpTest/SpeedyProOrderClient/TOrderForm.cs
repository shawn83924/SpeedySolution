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
        TMPOrderConnection.TaifexTMPConnection FOrderConnection = null;
        System.Collections.Hashtable     FNIDHash = null;
        System.Collections.Hashtable     FOrderIDHash = null;
        System.Collections.Hashtable     FAccountSet = null;
        System.Collections.Hashtable     FNIDSet = null;        
        LogonForm                                       FLogon = null;
        Object                                                 LockObject = null;
        int                                                        FRecoverCount;
        long                                                    FFreq;
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public TOrderForm( )
        {
            InitializeComponent();
            InitializeGrid();
            InitializeComboBox();
            
            FNIDHash               = new System.Collections.Hashtable();
            FOrderIDHash        = new System.Collections.Hashtable();
            FAccountSet            = new System.Collections.Hashtable();
            FNIDSet                   = new System.Collections.Hashtable();
            FOrderConnection = new TMPOrderConnection.TaifexTMPConnection();
            FLogon                     = new LogonForm( FOrderConnection ) ;
            LockObject               = new Object();
            ReadSetting();
            // Event handler for MBus
            FOrderConnection.OnConnected += new TMPOrderConnection.ITaifexTMPConnectionEvents_OnConnectedEventHandler(FOrderConnection_OnConnected);
            FOrderConnection.OnDisconnected += new TMPOrderConnection.ITaifexTMPConnectionEvents_OnDisconnectedEventHandler(FOrderConnection_OnDisconnected);   
            // Event Handler for SpeedyGateway
            FOrderConnection.OnLogonReply += new TMPOrderConnection.ITaifexTMPConnectionEvents_OnLogonReplyEventHandler(FOrderConnection_OnLogon);
            FOrderConnection.OnExecutionReport += new TMPOrderConnection.ITaifexTMPConnectionEvents_OnExecutionReportEventHandler(FOrderConnection_OnExecutionReport);
            FOrderConnection.OnRecoverFinished += new TMPOrderConnection.ITaifexTMPConnectionEvents_OnRecoverFinishedEventHandler(FOrderConnection_OnRecoverFinished);
            FOrderConnection.Create("OrderClient@" + System.Net.Dns.GetHostName());                        
            FOrderConnection.SetLanguage(TMPOrderConnection.MessageLanguageEnum.mlChinese);
            QueryPerformanceFrequency( out FFreq ); 
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void ReadSetting()
        {
            Microsoft.Win32.RegistryKey SoftwareReg = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("SOFTWARE",true);           
            Microsoft.Win32.RegistryKey MDBSReg = SoftwareReg.CreateSubKey("MDBS");
            Microsoft.Win32.RegistryKey AppReg = MDBSReg.CreateSubKey("SpeedyOrderClient");

            FIp.Text = AppReg.GetValue("IP", "192.168.168.120").ToString();
            FPort.Text = AppReg.GetValue("Port", "23456").ToString();
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

            AppReg.SetValue("IP", FIp.Text );
            AppReg.SetValue("Port", FPort.Text );
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
        void  FOrderConnection_OnLogon(string ReplyMessage, bool IsAccept, int ConnectionID )
        {
            toolStripStatusLabel1.Text = ReplyMessage;
            toolStripStatusLabel3.Text = "ConnectionID:" + Convert.ToString(ConnectionID);
            Program.FIsAccept = IsAccept;
            if (IsAccept == true)
            {
                Program.FConnectionID = ConnectionID;
                FOrderConnection.BrokerID = Program.FBrokerID;
                FOrderConnection.ClearMemberID = Program.FCMID;

                this.FutButton.Enabled = true;
                this.OptButton.Enabled = true;                
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
                EventArgs e = new EventArgs();
                FMarket_SelectedIndexChanged( FutButton, e );                
            }
            else
            {
                System.Windows.Forms.MessageBox.Show( ReplyMessage );               
            }            
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void  FOrderConnection_OnDisconnected()
        {
            this.FutButton.Enabled = false;
            this.OptButton.Enabled = false;
            this.Orderpanel.Enabled = false;
            this.panelFilter.Enabled = false;
            this.Disconnectbutton.Enabled = false;
            this.FRecover.Enabled = false;
            this.Connectbutton.Enabled = true;
            Clear();
            toolStripStatusLabel1.Text = "Connection failed!";
            FOrderConnection.Disconnect();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void  FOrderConnection_OnConnected()
        {            
           if( FLogon.ShowDialog() == DialogResult.OK )           
                WriteSetting();
           else
                FOrderConnection.Disconnect();
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
            
            FSide.Items.Clear();
            FSide.Items.Add(OrderMessage.SideEnum.sBuy);
            FSide.Items.Add(OrderMessage.SideEnum.sSell);           
            

            SessionComboBox.Items.Clear();
            SessionComboBox.Items.Add(OrderMessage.TradingSessionIDEnum.tsNormal);
            SessionComboBox.Items.Add(OrderMessage.TradingSessionIDEnum.tsOddLot);
            SessionComboBox.Items.Add(OrderMessage.TradingSessionIDEnum.tsOffHour);
            SessionComboBox.Items.Add(OrderMessage.TradingSessionIDEnum.tsAuction);
            SessionComboBox.Items.Add(OrderMessage.TradingSessionIDEnum.tsLend);
            SessionComboBox.Items.Add(OrderMessage.TradingSessionIDEnum.tsTender);
            SessionComboBox.Items.Add(OrderMessage.TradingSessionIDEnum.tsTenderEx);

            BSComboBox.SelectedIndex = 0;
            AccountComboBox.SelectedIndex = 0;
            MarketComboBox.SelectedIndex = 0;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void FOrderConnection_OnExecutionReport( OrderMessage.ExecutionReportMessage Msg, bool PossDup)
        {
            try
            {
                if( PossDup == true )
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
                    case OrderMessage.OrderStatusEnum.osPendingNew:
                    case OrderMessage.OrderStatusEnum.osPendingCancel:
                    case OrderMessage.OrderStatusEnum.osPendingReplace: break;
                    case OrderMessage.OrderStatusEnum.osNew: OnNewOrder(Msg); break;///< Order confirm from Exchange.                             
                    case OrderMessage.OrderStatusEnum.osQuoteAccept: OnQuoteAccepted(Msg); break;///< Quote confirm from Exchange.                             
                    case OrderMessage.OrderStatusEnum.osFilled:
                    case OrderMessage.OrderStatusEnum.osPartiallyFilled: OnPartiallyFilled(Msg.OrderID, Msg.NID, Msg.LastQty, Msg.LeavesQty,Msg.Side, Msg.LastPx, Msg.Data); break;
                    case OrderMessage.OrderStatusEnum.osReplaced: OnReplaced(Msg); break;
                    case OrderMessage.OrderStatusEnum.osCanceled: OnCanceled(Msg); break;
                    case OrderMessage.OrderStatusEnum.osRejected: OnRejected(Msg); break;
                    case OrderMessage.OrderStatusEnum.osQuoteRequestAccept: OnQuoteRequestAccepted(Msg); break;
                    ///< case OrderMessage.OrderStatusEnum.osExpired:
                }
            }
            catch( System.Exception e )
            {
                Console.WriteLine( e.ToString() );
            }          
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FMarket_SelectedIndexChanged(object sender, EventArgs e)
        {
            if( sender == FutButton )
            {
                FutButton.Checked = true;
                OptButton.Checked = false;                             
                SessionComboBox.Enabled = false;                
                FOrderType.Enabled = true;                
                if (Program.FNewVer == "true")
                {
                    FSymbol.Text = Program.FUTSymbol;
                    FTimeInForce.Enabled = true;
                }
                else
                {
                    FSymbol.Text = Program.FUTSymbol;
                    FTimeInForce.Enabled = false;
                }
                FPrice.Text = Program.FUTPx;
            }
            else if( sender == OptButton )
            {
                FutButton.Checked = false;
                OptButton.Checked = true;                
                SessionComboBox.Enabled = false;
                FTimeInForce.Enabled = true;
                FOrderType.Enabled = true;                
                FSymbol.Text = Program.OPTSymbol;
                FPrice.Text = Program.OPTPx;
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
            int Count    = TotalQty/SliceQty;
            int LastQty  = TotalQty % SliceQty + SliceQty;
                
            for (int i = 0; i < Count; i++)
            {
                OrderMessage.NewOrderMessage Msg = new OrderMessage.NewOrderMessage();

                Msg.AE = Program.FLogonID;
                Msg.Account = Program.FLogonAccount;
                Msg.Symbol = FSymbol.Text;
                Msg.OrderType = (OrderMessage.OrderTypeEnum)(FOrderType.SelectedItem);
                Msg.Price = Program.ConvertStringToDouble(FPrice.Text);                
                Msg.BrokerID = Program.FBrokerID;
                
                if( i == Count - 1 )
                    Msg.OrderQty = LastQty;
                else
                    Msg.OrderQty = SliceQty;
                Msg.Side = (OrderMessage.SideEnum)(FSide.SelectedItem);
                Msg.TradingSessionID = (OrderMessage.TradingSessionIDEnum)(SessionComboBox.SelectedItem);
                if ( FutButton.Checked == true )
                {
                    Msg.Market = OrderMessage.MarketEnum.mFutures;                    
                    Msg.TimeInForce = (OrderMessage.TimeInForceEnum)(FTimeInForce.SelectedItem);
                    Msg.StrikePrice = 0;
                    Msg.Data = Program.UserData;
                    Program.FUTSymbol = FSymbol.Text;
                    Program.FUTPx = FPrice.Text;
                }
                else if ( OptButton.Checked == true )
                {
                    Msg.Market = OrderMessage.MarketEnum.mOptions;
                    Msg.TimeInForce = (OrderMessage.TimeInForceEnum)(FTimeInForce.SelectedItem);
                    Msg.Data = Program.UserData; 
                    Program.OPTSymbol = FSymbol.Text;
                    Program.OPTPx = FPrice.Text;
                }
                NewOrder( Msg );
                Msg = null;
            }
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
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnQuoteRequestAccepted(OrderMessage.ExecutionReportMessage Msg)
        {
            Console.WriteLine( "Quote Request Accept NID:" + Convert.ToString(Msg.NID) + " Symbol:" + Msg.Symbol ) ;
            
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnQuoteAccepted(OrderMessage.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByNID(Msg.NID);

            if ( FoundRow != null )  ///< Quote from this order client
             {
                  OrderMessage.OrderStatusEnum OldOrderStatus = (OrderMessage.OrderStatusEnum)FoundRow["OrderStatus"];
                  long UseMS = 0;

                  if (FoundRow.UserData != null)
                      UseMS = ((GetTick() - (long)FoundRow.UserData) * 1000000) / FFreq;
                  FoundRow["UseTime"] = UseMS;
                  FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                  FoundRow["OrderID"] = Msg.OrderID;
                  FoundRow["IsOrder"] = false;
                  if (OldOrderStatus == OrderMessage.OrderStatusEnum.osPendingNew)
                  {
                       FOrderIDHash[Msg.OrderID] = FoundRow;
                       FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osQuoteAccept;
                       if( Msg.Data != null)
                           FoundRow["Data"] = Msg.Data;
                   }
              }
              else ///< Quote from other people or FIX
              {
                    if (FNIDSet.ContainsKey(Msg.NID) == false )
                         AddNewRowByNewOrderExecutionReport(Msg, false);
              }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnPartiallyFilled(string OrderID, int NID, int MatchQty, int LeavesQty, OrderMessage.SideEnum Side, double Price, string Data)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(OrderID);

            if (FoundRow == null) ///< Search by OrderID
                FoundRow = SearchByNID(NID); ///< Search by NID
            if ( FoundRow != null )
            {
                if (Data != null)
                    FoundRow["Data"] = Data;
                if (MatchQty > 0) ///< Filled or Partially filled
                {
                    FoundRow["OrderID"] = OrderID;
                    FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                    if ( (bool)FoundRow["IsOrder"] == false  ) ///< quote filled.
                    {
                         int BidQty = Convert.ToInt32(FoundRow["BidQty"]);
                         int AskQty = Convert.ToInt32(FoundRow["AskQty"]);
                         int BidCumQty = Convert.ToInt32(FoundRow["BidCumQty"]);
                         int AskCumQty = Convert.ToInt32(FoundRow["AskCumQty"]);
                          switch (Side)
                          {
                                case OrderMessage.SideEnum.sBuy:
                                            BidCumQty += MatchQty;
                                            FoundRow["BidCumQty"] = BidCumQty;
                                            break;
                                case OrderMessage.SideEnum.sSell:
                                            AskCumQty += MatchQty;
                                            FoundRow["AskCumQty"] = AskCumQty;
                                            break;
                            }
                            if ((BidCumQty == BidQty) && (AskCumQty == AskQty)) ///< Filled
                            {
                                FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osFilled;
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
                                FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osFilled;
                                if (ShowFillcheckBox.Checked == false)
                                     FoundRow.Visible = false;
                            }
                            else
                            {
                                FoundRow["Price"] = Convert.ToString(Price);
                                FoundRow["CumQty"] = CumQty;
                                if (LeavesQty == 0)
                                {
                                    FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osFilled;
                                    if (ShowFillcheckBox.Checked == false)
                                        FoundRow.Visible = false;
                                }
                                else
                                    FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osPartiallyFilled;
                            }
                    }
                }
                else                
                    RemoveFromGrid( FoundRow );   ///<Canceled.
            }
            else            
                Console.WriteLine("Order:" + OrderID + " NID:" + Convert.ToString(NID) + " not found!");            
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnReplaced( OrderMessage.ExecutionReportMessage Msg )
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(Msg.OrderID);

            if ( FoundRow != null )
            {
                int OrderQty = Convert.ToInt32( FoundRow["Qty"] );
                int CumQty  = Convert.ToInt32( FoundRow["CumQty"] );
                int AfterQty   =  Msg.LeavesQty;

                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
                FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                FoundRow["Qty"] = AfterQty;
                if (AfterQty > 0)
                {                    
                    if (CumQty > 0)
                        FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osPartiallyFilled;
                    else
                        FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osNew;
                }
                else ///< Filled or canceled.
                {
                    if (CumQty > 0) ///< Filled
                    {
                        FoundRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osFilled;
                        if (ShowFillcheckBox.Checked == false)
                            FoundRow.Visible = false;
                    }
                    else ///< Canceled
                        RemoveFromGrid(FoundRow);
                }
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OnCanceled(OrderMessage.ExecutionReportMessage Msg)
        {
            C1.Win.C1FlexGrid.Row FoundRow = SearchByOrderID(Msg.OrderID);

            if (FoundRow != null)
            {               
                FoundRow["Time"] = System.DateTime.Now.ToString("HH:mm:ss.fff");
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
                if ((bool)FoundRow["IsOrder"] == true)
                {
                    int CumQty = Convert.ToInt32(FoundRow["CumQty"]);

                    if (CumQty == 0) ///<Canceled 
                        RemoveFromGrid(FoundRow);
                    else
                        HideCanceledPartiallyFillOrder(FoundRow);
                }
                else ///< Quote
                {
                    int BidCumQty = Convert.ToInt32(FoundRow["BidCumQty"]);
                    int BidQty = Convert.ToInt32(FoundRow["BidQty"]);
                    int AskCumQty = Convert.ToInt32(FoundRow["AskCumQty"]);
                    int AskQty = Convert.ToInt32(FoundRow["AskQty"]);

                    if (Msg.Side == OrderMessage.SideEnum.sBuy)
                    {
                        if (BidCumQty != BidQty)
                        {
                            FoundRow["BidQty"] = BidCumQty;
                            BidQty = BidCumQty;
                        }
                    }
                    else
                    {
                        if (AskCumQty != AskQty)
                        {
                            FoundRow["AskQty"] = AskCumQty;
                            AskQty = AskCumQty;
                        }
                    }
                    if (BidQty == BidCumQty && AskQty == AskCumQty)
                    {
                        if (BidCumQty == 0 && AskCumQty == 0) ///<Canceled 
                            RemoveFromGrid(FoundRow);
                        else
                            HideCanceledPartiallyFillOrder(FoundRow);
                    }
                }
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
                    OrderMessage.OrderStatusEnum OldOrderStatus = (OrderMessage.OrderStatusEnum)FoundRow["OrderStatus"];
                    toolStripStatusLabel1.Text = "Cancelled at status: " + OldOrderStatus.ToString() + " OID:" + Msg.OrderID;
                }
                toolStripLabel1.Invalidate();
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
                        FoundRow["Account"] = Msg.Account;
                        FoundRow["Side"] = Msg.Side;
                        break;
                    default: break;
                }
                if (Msg.Data != null)
                    FoundRow["Data"] = Msg.Data;
            }
            else
            {
                switch (Msg.CxlRejResponseTo)
                {
                    case OrderMessage.CxlRejResponseToEnum.crrNew:
                        if (FNIDSet.ContainsKey(Msg.NID) == false)
                            AddNewRowByRejectOrderExecutionReport(Msg, true);
                        break;
                    case OrderMessage.CxlRejResponseToEnum.crrQuote:
                        if (FNIDSet.ContainsKey(Msg.NID) == false)
                            AddNewRowByRejectOrderExecutionReport(Msg, false);
                        break;
                    default: break;
                }
                
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
        C1.Win.C1FlexGrid.Row AddNewRowByRejectOrderExecutionReport(OrderMessage.ExecutionReportMessage Msg, bool IsOrder)
        {
            C1.Win.C1FlexGrid.Row NewRow = FGrid.Rows.Add();
            FGrid.ShowCell(FGrid.Rows.Count - 1, 0);
            toolStripStatusLabel2.Text = Convert.ToSingle(FGrid.Rows.Count - 1) + " Order records";
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
            NewRow["Text"] = Msg.Text;
            NewRow["IsOrder"] = IsOrder;            
            if (NID != 0)
                FNIDHash[NID] = NewRow;
            FNIDSet.Add(NID, null);
            if (Msg.OrderID.Length > 0)
                FOrderIDHash[Msg.OrderID] = NewRow;            
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
                        OrderMessage.OrderStatusEnum OrderStatus = (OrderMessage.OrderStatusEnum)(CancelRow["OrderStatus"]);
                        switch (OrderStatus)
                        {
                            case OrderMessage.OrderStatusEnum.osPendingNew:
                            case OrderMessage.OrderStatusEnum.osPendingReplace:
                            case OrderMessage.OrderStatusEnum.osRejected:     RemoveFromGrid(CancelRow); break;
                            case OrderMessage.OrderStatusEnum.osQuoteAccept:  QuoteCancel(CancelRow); break;
                            case OrderMessage.OrderStatusEnum.osNew:
                            case OrderMessage.OrderStatusEnum.osPartiallyFilled:
                            case OrderMessage.OrderStatusEnum.osReplaced:     OrderCancel(CancelRow); break;
                            case OrderMessage.OrderStatusEnum.osCanceled:
                            case OrderMessage.OrderStatusEnum.osExpired:
                            case OrderMessage.OrderStatusEnum.osFilled:                            
                            default:break;
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
                    if( QueryRow != null )
                        OrderStatusRequest(QueryRow);
                    break;
                case Keys.Z:
                    this.SendQuoteRequest();
                    break;
                case Keys.A:
                    if (e.Control)
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
        private void SendQuoteRequest(  )
        {
            OrderMessage.QuoteRequestMessage Msg = new OrderMessage.QuoteRequestMessage();
            if (FutButton.Checked == true)
                Msg.Market = OrderMessage.MarketEnum.mFutures;
            else
                Msg.Market = OrderMessage.MarketEnum.mOptions;
            Msg.Account = Program.FMMAccount;
            Msg.Symbol = FSymbol.Text;            
            FOrderConnection.QuoteRequest(Msg);
            Msg = null;
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
            NewRow["IsOrder"] = true;
            Msg.PositionEffect = OrderMessage.PositionEffectEnum.peOpen;
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
            Msg.NID = FOrderConnection.GenerateUniqueID( OrderMessage.MessageTypeEnum.mtNew);
            NewRow["NID"] = Msg.NID;
            if (Msg.NID != 0)
                FNIDHash[Msg.NID] = NewRow;
            FNIDSet.Add( Msg.NID, null);
            CheckVisable(NewRow);
            CheckAccount(Msg.Account);
            try
            {
                NewRow.UserData = GetTick();
                FOrderConnection.NewOrder(Msg);
            }
            catch ( COMException e )
            {
                System.Windows.Forms.MessageBox.Show( e.Message ) ;
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void OrderReplace(C1.Win.C1FlexGrid.Row ReplaceRow)
        {
            OrderMessage.OrderStatusEnum OrderStatus = (OrderMessage.OrderStatusEnum)(ReplaceRow["OrderStatus"]);
            if ((OrderStatus == OrderMessage.OrderStatusEnum.osNew) || (OrderStatus == OrderMessage.OrderStatusEnum.osPartiallyFilled))
            {
                int QtyNow = Convert.ToInt32(ReplaceRow["Qty"]);
                TReplaceForm ReplaceForm = new TReplaceForm( QtyNow );
                if (ReplaceForm.ShowDialog() == DialogResult.OK)
                {
                    OrderMessage.ReplaceOrderMessage Msg = new OrderMessage.ReplaceOrderMessage();
                    
                    Msg.Account = ReplaceRow["Account"].ToString();
                    Msg.OrderID = ReplaceRow["OrderID"].ToString();
                    Msg.Symbol = ReplaceRow["Symbol"].ToString();
                    Msg.BrokerID = ReplaceRow["BrokerID"].ToString();
                    Msg.OrderQty = ReplaceForm.GetReplaceQty();                    
                    Msg.Side = (OrderMessage.SideEnum)(ReplaceRow["Side"]);
                    Msg.OrderType = (OrderMessage.OrderTypeEnum)(ReplaceRow["OrderType"]);
                    Msg.PositionEffect = OrderMessage.PositionEffectEnum.peOpen;
                    Msg.Price = Program.ConvertStringToDouble(ReplaceRow["Price"].ToString());
                    
                    OrderMessage.MarketEnum Market = (OrderMessage.MarketEnum)(ReplaceRow["Market"]);

                    Msg.Market = Market;

                    switch (Market)
                    {
                        case OrderMessage.MarketEnum.mFutures:
                             Msg.TimeInForce = (OrderMessage.TimeInForceEnum)(ReplaceRow["TimeInForce"]);
                            Msg.Data = Program.UserData;
                            break;
                        case OrderMessage.MarketEnum.mOptions:
                            Msg.TimeInForce = (OrderMessage.TimeInForceEnum)(ReplaceRow["TimeInForce"]);
                            Msg.Data = Program.UserData;
                            break;
                        case OrderMessage.MarketEnum.mTSE:
                            Msg.Data = Program.UserData;
                            break;
                        case OrderMessage.MarketEnum.mOTC:
                            Msg.Data = Program.UserData;
                            break;
                    }
                    Msg.PositionEffect = OrderMessage.PositionEffectEnum.peOpen;
                    
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

            Msg.Market = (OrderMessage.MarketEnum)(CancelRow["Market"]);
            Msg.Account = CancelRow["Account"].ToString();            
            Msg.OrderID = CancelRow["OrderID"].ToString(); ///< Cancel Order by OrderID.  
            Msg.Symbol  = CancelRow["Symbol"].ToString();
            Msg.Price   = Program.ConvertStringToDouble(CancelRow["Price"].ToString());
            Msg.Side    = (OrderMessage.SideEnum)(CancelRow["Side"]);
            Msg.OrderType        = (OrderMessage.OrderTypeEnum)(CancelRow["OrderType"]);
            Msg.TradingSessionID = (OrderMessage.TradingSessionIDEnum)(CancelRow["TradingSession"]);
            Msg.BrokerID = Program.FBrokerID;

            switch (Msg.Market)
            {
                case OrderMessage.MarketEnum.mFutures:                    
                    if( CancelRow["MaturityMonthYear"] != null )
                        Msg.MaturityMonthYear = CancelRow["MaturityMonthYear"].ToString();
                    Msg.StrikePrice = Convert.ToDouble(CancelRow["StrikePrice"]);
                    Msg.Data = Program.UserData + "[Fut Cancel]";
                    break;
                case OrderMessage.MarketEnum.mOptions:
                    Msg.TimeInForce = (OrderMessage.TimeInForceEnum)(CancelRow["TimeInForce"]);
                    Msg.Data = Program.UserData + "[Opt Cancel]";
                    break;
                case OrderMessage.MarketEnum.mOTC:
                    Msg.Data = Program.UserData + "[OTC Cancel]";
                    break;
                case OrderMessage.MarketEnum.mTSE:
                    Msg.Data = Program.UserData + "[TSE Cancel]";
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
            Msg.BrokerID = Program.FBrokerID;
            Msg.Data = "OrderStatusRequest";
            FOrderConnection.OrderStatusRequest(Msg);
            int NID = Msg.NID;
            if (NID != 0)
                FNIDHash[NID] = QueryRow;
            Console.WriteLine(" ------ Order Sataus Request  NID:" + Convert.ToString(NID) );
            Msg = null;
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void Quote()
        {
            TQuoteForm QuoteForm = new TQuoteForm();
            QuoteForm.SetSymbol( FSymbol.Text );
            QuoteForm.SetBidPrice( Program.ConvertStringToDouble( FPrice.Text ) );
            if (QuoteForm.ShowDialog() == DialogResult.OK)
            {
                OrderMessage.QuoteMessage Msg = new OrderMessage.QuoteMessage();

                if (FutButton.Checked == true)
                    Msg.Market = OrderMessage.MarketEnum.mFutures;
                else if (OptButton.Checked == true)
                    Msg.Market = OrderMessage.MarketEnum.mOptions;

                Msg.Account = Program.FMMAccount;
                Msg.AE = Program.FLogonID;
                Msg.Symbol   = QuoteForm.GetSymbol();
                Msg.BidPrice = QuoteForm.GetBidPrice();
                Msg.BidQty   = QuoteForm.GetBidQty();
                Msg.AskPrice = QuoteForm.GetAskPrice();
                Msg.AskQty   = QuoteForm.GetAskQty();
                Msg.TimeInForce = QuoteForm.GetTimeInForce();                
                Msg.Data = "New Quote";
                
                int BidQty = Msg.BidQty;
                int AskQty = Msg.AskQty;
                C1.Win.C1FlexGrid.Row NewRow = FGrid.Rows.Add();
                FGrid.ShowCell(FGrid.Rows.Count-1, 0);
                toolStripStatusLabel2.Text = Convert.ToSingle(FGrid.Rows.Count - 1) + " Order records";                
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
                NewRow["OrderStatus"] = OrderMessage.OrderStatusEnum.osPendingNew;
                Msg.NID = FOrderConnection.GenerateUniqueID( OrderMessage.MessageTypeEnum.mtQuote);
                NewRow["NID"] = Msg.NID;
                if (Msg.NID != 0)
                    FNIDHash[Msg.NID] = NewRow;
                FOrderConnection.Quote(Msg);                
                Msg = null;
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void QuoteCancel(C1.Win.C1FlexGrid.Row CancelRow)
        {
            OrderMessage.QuoteCancelMessage Msg = new OrderMessage.QuoteCancelMessage();

            Msg.AE = Program.FLogonID;
            Msg.Market   = (OrderMessage.MarketEnum)CancelRow["Market"];
            Msg.Account = Program.FMMAccount;
            Msg.QuoteID  = CancelRow["OrderID"].ToString();
            Msg.Symbol   = CancelRow["Symbol"].ToString();
            Msg.AskPrice = Program.ConvertStringToDouble(CancelRow["AskPrice"].ToString());
            Msg.BidPrice = Program.ConvertStringToDouble(CancelRow["BidPrice"].ToString());
            Msg.Data = Program.UserData +"Cancel";
            FOrderConnection.QuoteCancel( Msg );
            int NID = Msg.NID;
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
            FOrderConnection.Connect(FIp.Text, Port);            
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void FRecover_Click(object sender, EventArgs e)
        {
            string RecoverTime = dateTimePicker.Value.ToString("HHmmss");

            FRecover.Enabled = false;
            Clear();            
            FGrid.Visible = false;
            FRecoverCount = 0;
            FOrderConnection.Recover(RecoverTime, TMPOrderConnection.RecoverTypeEnum.rtAll, TMPOrderConnection.RecoverMarketEnum.rmAll);
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
                     if( Convert.ToInt32( FGrid[e.Row, FGrid.Cols["BidCumQty"].Index ].ToString() ) > 0 )
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
            if( Total <= 0 )
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
                Countlabel.Text = "Count:" + Convert.ToString(Total/Slice);
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
            FNewOrder_Click(this, e ); 
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            SettingForm setting = new SettingForm();
            if (setting.ShowDialog() == DialogResult.OK)
            {
                FOrderConnection.BrokerID = Program.FBrokerID;
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
            if (Program.FNewOrder == "true")
            {
                Orderpanel.Visible = true;                
                FGrid.Top = (Orderpanel.Top + Orderpanel.Height);
                FGrid.Height = this.ClientSize.Height - Orderpanel.Height - panelFilter.Height - toolStrip1.Height -30;
                this.FutButton.Visible = true;
                this.OptButton.Visible = true;
            }
            else
            {
                Orderpanel.Visible = false;
                FGrid.Top = (panelFilter.Top + panelFilter.Height);
                FGrid.Height = this.ClientSize.Height - panelFilter.Height - toolStrip1.Height -30;
                this.FutButton.Visible = false;
                this.OptButton.Visible = false;
            }
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
    }
}