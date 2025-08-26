using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace WindowsApplication1
{
    public partial class MainForm : Form
    {
        MBusAPI.MdTransport FTransport;
        MBusAPI.MdListener FListener;
        bool FSending = false;
        bool FReceiving = false;
        int FInCount;
        int FOutCount;
        int FListCount;
        //----------------------------------------------------------------------------------------------
        public MainForm()
        {
            InitializeComponent();
            ///< Create MBus message transport object.
            FTransport = new MBusAPI.MdTransport();
            ///< Add event handler
            FTransport.OnConnected += new MBusAPI.IMdTransportEvents_OnConnectedEventHandler(FTransport_OnAppConnected);
            FTransport.OnDisconnected += new MBusAPI.IMdTransportEvents_OnDisconnectedEventHandler(FTransport_OnAppDisconnected);
            FTransport.OnError += new MBusAPI.IMdTransportEvents_OnErrorEventHandler(FTransport_OnAppError);
            FTransport.OnProcessConnected += new MBusAPI.IMdTransportEvents_OnProcessConnectedEventHandler(FTransport_OnProcessConnected);
            FTransport.OnProcessStartup += new MBusAPI.IMdTransportEvents_OnProcessStartupEventHandler(FTransport_OnProcessStartup);
            FTransport.OnProcessStopped += new MBusAPI.IMdTransportEvents_OnProcessStoppedEventHandler(FTransport_OnProcessStopped);
            ///< Create message listener.
            FListener = new MBusAPI.MdListener();
            FListener.OnMessageReceived += new MBusAPI.IMdListenerEvents_OnMessageReceivedEventHandler(FListener_OnMessageReceived);
        }
        //----------------------------------------------------------------------------------------------
        void FTransport_OnProcessStopped(MBusAPI.MdTransport Transport, string Host, string AppName)
        {
            Console.WriteLine( AppName + " Stopped! Thread:" + System.Threading.Thread.CurrentThread.ManagedThreadId.ToString());
            toolStripStatusLabel3.Text  =string.Format("{0}@{1} Stopped.",AppName, Host );
        }
        //----------------------------------------------------------------------------------------------
        void FTransport_OnProcessStartup(MBusAPI.MdTransport Transport, string Host, string AppName)
        {
            Console.WriteLine(AppName + " Startup! Thread:" + System.Threading.Thread.CurrentThread.ManagedThreadId.ToString());
            toolStripStatusLabel3.Text = string.Format("{0}@{1} Startup.", AppName, Host);
        }
        //----------------------------------------------------------------------------------------------
        void FTransport_OnProcessConnected(MBusAPI.MdTransport Transport, bool IsTheFirstOne)
        {
            if (IsTheFirstOne == false)
            {
                Console.WriteLine(" First CSharpPublisher, Thread:" + System.Threading.Thread.CurrentThread.ManagedThreadId.ToString());
                toolStripStatusLabel3.Text = "A CSharpPublisher already connected to this server.";
            }
            else
            {
                Console.WriteLine(" First CSharpPublisher, Thread:" + System.Threading.Thread.CurrentThread.ManagedThreadId.ToString());
                toolStripStatusLabel3.Text = " CSharpPublisher connected.";
            }
        }
        //----------------------------------------------------------------------------------------------
        void FListener_OnMessageReceived(string Subject, string Key, MBusAPI.MdMessage Message)
        {
            if ( checkBox1.Checked == true)
            {
                int NodeCount = Message.GetFieldCount();

                FMsgTreeView.BeginUpdate();

                TreeNode RootNode = FMsgTreeView.Nodes.Add(string.Format("{0}:{1}:{2}.{3} {4},{5}", System.DateTime.Now.Hour,System.DateTime.Now.Minute,System.DateTime.Now.Second,System.DateTime.Now.Millisecond, Subject, Key));
                for( int i = 0; i < NodeCount; i++)
                {
                     switch( Message.GetType( i ) )
                     {
                         case MBusAPI.DataTypeEnum.dtString: RootNode.Nodes.Add(string.Format("[String] {0}:{1}", Message.GetName(i), Convert.ToString(Message.GetValue(i)))); break;
                         case MBusAPI.DataTypeEnum.dtInteger: RootNode.Nodes.Add(string.Format("[Int] {0}:{1}", Message.GetName(i), Convert.ToString(Message.GetValue(i)))); break;
                         case MBusAPI.DataTypeEnum.dtDouble: RootNode.Nodes.Add(string.Format("[Double] {0}:{1}", Message.GetName(i), Convert.ToString(Message.GetValue(i)))); break;
                         case MBusAPI.DataTypeEnum.dtRowData: RootNode.Nodes.Add(string.Format("[RowData] {0}:{1}", Message.GetName(i), Convert.ToString(Message.GetValue(i)))); break;
                         default: break;
                     }                     
                }
                FMsgTreeView.EndUpdate();
            }
            FInCount++;
        }        
        //----------------------------------------------------------------------------------------------
        void FTransport_OnAppError(MBusAPI.MdTransport Transport, string ErrorMessage)
        {            
            Console.WriteLine("Connection failed!" + ErrorMessage );
            toolStripStatusLabel3.Text = "Connection failed!";
            SendBtn.Enabled = false;
            DisconnectBtn.Enabled = false;
            ConnectBtn.Enabled = true;
            this.IPtextBox.Enabled = true;
            this.SubjecttextBox.Enabled = true;
            this.KeytextBox.Enabled = true;
            this.timer.Enabled = false;
        }
        //----------------------------------------------------------------------------------------------
        void FTransport_OnAppDisconnected(MBusAPI.MdTransport Transport)
        {
            Console.WriteLine("Disconnected");
            toolStripStatusLabel3.Text = "Disconnected";
            SendBtn.Enabled = false;
            DisconnectBtn.Enabled = false;
            ConnectBtn.Enabled = true;
            this.IPtextBox.Enabled = true;
            this.SubjecttextBox.Enabled = true;
            this.KeytextBox.Enabled = true;
            this.timer.Enabled = false;            
        }
        //----------------------------------------------------------------------------------------------
        void FTransport_OnAppConnected(MBusAPI.MdTransport Transport)
        {
            Console.WriteLine( "Connected! Thread:" + System.Threading.Thread.CurrentThread.ManagedThreadId.ToString() );
            toolStripStatusLabel3.Text = "Connected!";
            SendBtn.Enabled = true;
            DisconnectBtn.Enabled = true;
            ConnectBtn.Enabled = false;
            this.IPtextBox.Enabled = false;
            this.SubjecttextBox.Enabled = false;
            this.KeytextBox.Enabled = false;
            this.timer.Enabled = true;
            //FTransport.AddMonitoringProcess("MD1500"/*Hostname*/, "SpeedyOPT"/*Process MBus Name*/);
        }             
        //----------------------------------------------------------------------------------------------
        private void ConnectBtn_Click(object sender, EventArgs e)
        {
            FTransport.Destroy();
            FTransport.Create("MDBS","CSharpPublisher", IPtextBox.Text, 12345, true);            
            FListener.Create(FTransport, SubjecttextBox.Text, KeytextBox.Text);            
            
            Console.WriteLine( "Connection...");
            toolStripStatusLabel3.Text = "Connection...";
            FTransport.Connect( 10 );               
        }
        //----------------------------------------------------------------------------------------------
        private void DisconnectBtn_Click(object sender, EventArgs e)
        {
            FListener.Destroy();
            Console.WriteLine("Disconnect...");
            toolStripStatusLabel3.Text = "Disconnect...";
            FTransport.Disconnect();            
        }
        //----------------------------------------------------------------------------------------------
        private void SendBtn_Click(object sender, EventArgs e)
        {
            if (FSending == false)
            {
                FSending = true;
                SendBtn.Text = "Stop";
                MBusAPI.MdMessage Message = new MBusAPI.MdMessage();
                Message.Create();
                Message.SetField( "string", "Testing String"¡@);
                Message.SetField( "int", 12345);                
                while (FSending == true)
                {
                    FTransport.Send("MessageDeliver", "test", Message);
                    FOutCount++;
                    Application.DoEvents();
                }
                Message.Destroy();
                Message = null;
            }
            else
            {
                FSending = false;
                SendBtn.Text = "Sending";
            }
        }
        //----------------------------------------------------------------------------------------------
        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true)
                FMsgTreeView.Enabled = true;
            else
                FMsgTreeView.Enabled = false;
        }
        //----------------------------------------------------------------------------------------------
        private void timer_Tick(object sender, EventArgs e)
        {
            toolStripStatusLabel1.Text = string.Format("Sent: {0} msg/sec", FOutCount);
            FOutCount = 0;
            toolStripStatusLabel2.Text = string.Format("Recv: {0} msg/sec", FInCount);
            FInCount = 0;
        }
        //----------------------------------------------------------------------------------------------
        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            Properties.Settings.Default.MBusSubject = this.SubjecttextBox.Text;
            Properties.Settings.Default.MBusKey = this.KeytextBox.Text;
            Properties.Settings.Default.MBusIP = this.IPtextBox.Text;
            Properties.Settings.Default.Save();
            FListener.Destroy();
            if( FTransport.IsConnected() )
                FTransport.Disconnect();            
            System.Threading.Thread.Sleep(200);
            FTransport.Destroy();
        }
        //----------------------------------------------------------------------------------------------
        private void ClearButton_Click(object sender, EventArgs e)
        {
            FMsgTreeView.Nodes.Clear();
        }
        //----------------------------------------------------------------------------------------------
    }
}