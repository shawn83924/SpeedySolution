namespace Test1
{
    partial class TOrderForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TOrderForm));
            this.FGrid = new C1.Win.C1FlexGrid.C1FlexGrid();
            this.FQuoteRequestTimer = new System.Windows.Forms.Timer(this.components);
            this.Orderpanel = new System.Windows.Forms.Panel();
            this.FMaturityMonthYear = new System.Windows.Forms.TextBox();
            this.label15 = new System.Windows.Forms.Label();
            this.FExchange = new System.Windows.Forms.TextBox();
            this.label14 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.PEcomboBox = new System.Windows.Forms.ComboBox();
            this.StockSeqTextBox = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.SessionComboBox = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.Countlabel = new System.Windows.Forms.Label();
            this.FSliceQty = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.FNewOrder = new System.Windows.Forms.Button();
            this.FQty = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.FTimeInForce = new System.Windows.Forms.ComboBox();
            this.label11 = new System.Windows.Forms.Label();
            this.FSide = new System.Windows.Forms.ComboBox();
            this.label10 = new System.Windows.Forms.Label();
            this.FPrice = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.FOrderType = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this.FSymbol = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel2 = new System.Windows.Forms.ToolStripStatusLabel();
            this.CIDtoolStripStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel3 = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.Connectbutton = new System.Windows.Forms.ToolStripButton();
            this.Disconnectbutton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.FRecover = new System.Windows.Forms.ToolStripButton();
            this.CancelWorkingButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.FutButton = new System.Windows.Forms.ToolStripButton();
            this.OptButton = new System.Windows.Forms.ToolStripButton();
            this.TseButton = new System.Windows.Forms.ToolStripButton();
            this.OtcButton = new System.Windows.Forms.ToolStripButton();
            this.PATSOButton = new System.Windows.Forms.ToolStripButton();
            this.PATSButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.IPLabel = new System.Windows.Forms.ToolStripLabel();
            this.FIp = new System.Windows.Forms.ToolStripTextBox();
            this.HostLabel = new System.Windows.Forms.ToolStripLabel();
            this.FPort = new System.Windows.Forms.ToolStripTextBox();
            this.SendOrderTimer = new System.Windows.Forms.Timer(this.components);
            this.dateTimePicker = new System.Windows.Forms.DateTimePicker();
            this.clockTimer = new System.Windows.Forms.Timer(this.components);
            this.panelFilter = new System.Windows.Forms.Panel();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.AccounttextBox = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.button2 = new System.Windows.Forms.Button();
            this.ShowFillcheckBox = new System.Windows.Forms.CheckBox();
            this.BSComboBox = new System.Windows.Forms.ComboBox();
            this.label13 = new System.Windows.Forms.Label();
            this.MarketComboBox = new System.Windows.Forms.ComboBox();
            this.label9 = new System.Windows.Forms.Label();
            this.AccountComboBox = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.FGrid)).BeginInit();
            this.Orderpanel.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.panelFilter.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // FGrid
            // 
            this.FGrid.AllowEditing = false;
            this.FGrid.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.FGrid.ColumnInfo = "1,0,0,0,0,140,Columns:";
            this.FGrid.DrawMode = C1.Win.C1FlexGrid.DrawModeEnum.OwnerDraw;
            this.FGrid.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.FGrid.Location = new System.Drawing.Point(4, 198);
            this.FGrid.Name = "FGrid";
            this.FGrid.Rows.Count = 1;
            this.FGrid.Rows.DefaultSize = 28;
            this.FGrid.SelectionMode = C1.Win.C1FlexGrid.SelectionModeEnum.ListBox;
            this.FGrid.Size = new System.Drawing.Size(1103, 290);
            this.FGrid.StyleInfo = resources.GetString("FGrid.StyleInfo");
            this.FGrid.TabIndex = 25;
            this.FGrid.SelChange += new System.EventHandler(this.FGrid_SelChange);
            this.FGrid.OwnerDrawCell += new C1.Win.C1FlexGrid.OwnerDrawCellEventHandler(this.FGrid_OwnerDrawCell);
            this.FGrid.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FGrid_KeyDown);
            this.FGrid.MouseClick += new System.Windows.Forms.MouseEventHandler(this.FGrid_MouseClick);
            // 
            // FQuoteRequestTimer
            // 
            this.FQuoteRequestTimer.Interval = 1000;
            this.FQuoteRequestTimer.Tick += new System.EventHandler(this.FQuoteRequestTimer_Tick);
            // 
            // Orderpanel
            // 
            this.Orderpanel.Controls.Add(this.FMaturityMonthYear);
            this.Orderpanel.Controls.Add(this.label15);
            this.Orderpanel.Controls.Add(this.FExchange);
            this.Orderpanel.Controls.Add(this.label14);
            this.Orderpanel.Controls.Add(this.label5);
            this.Orderpanel.Controls.Add(this.PEcomboBox);
            this.Orderpanel.Controls.Add(this.StockSeqTextBox);
            this.Orderpanel.Controls.Add(this.label4);
            this.Orderpanel.Controls.Add(this.SessionComboBox);
            this.Orderpanel.Controls.Add(this.label2);
            this.Orderpanel.Controls.Add(this.button1);
            this.Orderpanel.Controls.Add(this.Countlabel);
            this.Orderpanel.Controls.Add(this.FSliceQty);
            this.Orderpanel.Controls.Add(this.label1);
            this.Orderpanel.Controls.Add(this.FNewOrder);
            this.Orderpanel.Controls.Add(this.FQty);
            this.Orderpanel.Controls.Add(this.label12);
            this.Orderpanel.Controls.Add(this.FTimeInForce);
            this.Orderpanel.Controls.Add(this.label11);
            this.Orderpanel.Controls.Add(this.FSide);
            this.Orderpanel.Controls.Add(this.label10);
            this.Orderpanel.Controls.Add(this.FPrice);
            this.Orderpanel.Controls.Add(this.label8);
            this.Orderpanel.Controls.Add(this.FOrderType);
            this.Orderpanel.Controls.Add(this.label7);
            this.Orderpanel.Controls.Add(this.FSymbol);
            this.Orderpanel.Controls.Add(this.label3);
            this.Orderpanel.Enabled = false;
            this.Orderpanel.Location = new System.Drawing.Point(4, 116);
            this.Orderpanel.Name = "Orderpanel";
            this.Orderpanel.Size = new System.Drawing.Size(1106, 76);
            this.Orderpanel.TabIndex = 30;
            // 
            // FMaturityMonthYear
            // 
            this.FMaturityMonthYear.Enabled = false;
            this.FMaturityMonthYear.ForeColor = System.Drawing.SystemColors.WindowText;
            this.FMaturityMonthYear.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.FMaturityMonthYear.Location = new System.Drawing.Point(83, 40);
            this.FMaturityMonthYear.MaxLength = 20;
            this.FMaturityMonthYear.Name = "FMaturityMonthYear";
            this.FMaturityMonthYear.Size = new System.Drawing.Size(81, 29);
            this.FMaturityMonthYear.TabIndex = 76;
            this.FMaturityMonthYear.Text = "DEC11";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(3, 43);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(72, 19);
            this.label15.TabIndex = 75;
            this.label15.Text = "Maturity";
            // 
            // FExchange
            // 
            this.FExchange.Enabled = false;
            this.FExchange.ForeColor = System.Drawing.SystemColors.WindowText;
            this.FExchange.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.FExchange.Location = new System.Drawing.Point(83, 7);
            this.FExchange.MaxLength = 20;
            this.FExchange.Name = "FExchange";
            this.FExchange.Size = new System.Drawing.Size(81, 29);
            this.FExchange.TabIndex = 74;
            this.FExchange.Text = "TAIFEX";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(4, 10);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(80, 19);
            this.label14.TabIndex = 73;
            this.label14.Text = "Exchange";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(875, 11);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(112, 19);
            this.label5.TabIndex = 72;
            this.label5.Text = "PositionEffect";
            // 
            // PEcomboBox
            // 
            this.PEcomboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.PEcomboBox.FormattingEnabled = true;
            this.PEcomboBox.Location = new System.Drawing.Point(987, 8);
            this.PEcomboBox.Name = "PEcomboBox";
            this.PEcomboBox.Size = new System.Drawing.Size(107, 26);
            this.PEcomboBox.TabIndex = 71;
            // 
            // StockSeqTextBox
            // 
            this.StockSeqTextBox.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.StockSeqTextBox.Location = new System.Drawing.Point(859, 40);
            this.StockSeqTextBox.MaxLength = 6;
            this.StockSeqTextBox.Name = "StockSeqTextBox";
            this.StockSeqTextBox.Size = new System.Drawing.Size(28, 29);
            this.StockSeqTextBox.TabIndex = 70;
            this.StockSeqTextBox.Text = "0";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(762, 45);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(99, 19);
            this.label4.TabIndex = 69;
            this.label4.Text = "StockSeqNo";
            // 
            // SessionComboBox
            // 
            this.SessionComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.SessionComboBox.Enabled = false;
            this.SessionComboBox.FormattingEnabled = true;
            this.SessionComboBox.Items.AddRange(new object[] {
            "Normal",
            "Odd Lot",
            "Off Hour",
            "Auction",
            "Lend",
            "Tender"});
            this.SessionComboBox.Location = new System.Drawing.Point(641, 42);
            this.SessionComboBox.Name = "SessionComboBox";
            this.SessionComboBox.Size = new System.Drawing.Size(121, 26);
            this.SessionComboBox.TabIndex = 68;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(577, 45);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(64, 19);
            this.label2.TabIndex = 67;
            this.label2.Text = "Session";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(1031, 38);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(63, 31);
            this.button1.TabIndex = 66;
            this.button1.Text = "Auto";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // Countlabel
            // 
            this.Countlabel.AutoSize = true;
            this.Countlabel.Location = new System.Drawing.Point(491, 45);
            this.Countlabel.Name = "Countlabel";
            this.Countlabel.Size = new System.Drawing.Size(67, 19);
            this.Countlabel.TabIndex = 65;
            this.Countlabel.Text = "Count:1";
            // 
            // FSliceQty
            // 
            this.FSliceQty.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.FSliceQty.Location = new System.Drawing.Point(442, 40);
            this.FSliceQty.MaxLength = 4;
            this.FSliceQty.Name = "FSliceQty";
            this.FSliceQty.Size = new System.Drawing.Size(46, 29);
            this.FSliceQty.TabIndex = 64;
            this.FSliceQty.Text = "10";
            this.FSliceQty.TextChanged += new System.EventHandler(this.FQty_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(369, 45);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 19);
            this.label1.TabIndex = 63;
            this.label1.Text = "Slice Qty";
            // 
            // FNewOrder
            // 
            this.FNewOrder.Location = new System.Drawing.Point(892, 38);
            this.FNewOrder.Name = "FNewOrder";
            this.FNewOrder.Size = new System.Drawing.Size(133, 32);
            this.FNewOrder.TabIndex = 62;
            this.FNewOrder.Text = "New";
            this.FNewOrder.UseVisualStyleBackColor = true;
            this.FNewOrder.Click += new System.EventHandler(this.FNewOrder_Click);
            // 
            // FQty
            // 
            this.FQty.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.FQty.Location = new System.Drawing.Point(318, 40);
            this.FQty.MaxLength = 4;
            this.FQty.Name = "FQty";
            this.FQty.Size = new System.Drawing.Size(48, 29);
            this.FQty.TabIndex = 57;
            this.FQty.Text = "10";
            this.FQty.TextChanged += new System.EventHandler(this.FQty_TextChanged);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(283, 45);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(36, 19);
            this.label12.TabIndex = 56;
            this.label12.Text = "Qty";
            // 
            // FTimeInForce
            // 
            this.FTimeInForce.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.FTimeInForce.Enabled = false;
            this.FTimeInForce.FormattingEnabled = true;
            this.FTimeInForce.Location = new System.Drawing.Point(559, 7);
            this.FTimeInForce.Name = "FTimeInForce";
            this.FTimeInForce.Size = new System.Drawing.Size(66, 26);
            this.FTimeInForce.TabIndex = 61;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(456, 10);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(103, 19);
            this.label11.TabIndex = 60;
            this.label11.Text = "TimeInForce";
            // 
            // FSide
            // 
            this.FSide.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.FSide.FormattingEnabled = true;
            this.FSide.Location = new System.Drawing.Point(388, 8);
            this.FSide.Name = "FSide";
            this.FSide.Size = new System.Drawing.Size(65, 26);
            this.FSide.TabIndex = 59;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(346, 11);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(41, 19);
            this.label10.TabIndex = 58;
            this.label10.Text = "Side";
            // 
            // FPrice
            // 
            this.FPrice.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.FPrice.Location = new System.Drawing.Point(209, 40);
            this.FPrice.MaxLength = 10;
            this.FPrice.Name = "FPrice";
            this.FPrice.Size = new System.Drawing.Size(72, 29);
            this.FPrice.TabIndex = 55;
            this.FPrice.Text = "6000";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(164, 45);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(46, 19);
            this.label8.TabIndex = 54;
            this.label8.Text = "Price";
            // 
            // FOrderType
            // 
            this.FOrderType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.FOrderType.FormattingEnabled = true;
            this.FOrderType.Location = new System.Drawing.Point(717, 7);
            this.FOrderType.Name = "FOrderType";
            this.FOrderType.Size = new System.Drawing.Size(152, 26);
            this.FOrderType.TabIndex = 53;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(628, 10);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(88, 19);
            this.label7.TabIndex = 52;
            this.label7.Text = "OrderType";
            // 
            // FSymbol
            // 
            this.FSymbol.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.FSymbol.Location = new System.Drawing.Point(230, 7);
            this.FSymbol.MaxLength = 20;
            this.FSymbol.Name = "FSymbol";
            this.FSymbol.Size = new System.Drawing.Size(113, 29);
            this.FSymbol.TabIndex = 47;
            this.FSymbol.Text = "FITX";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(166, 10);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(65, 19);
            this.label3.TabIndex = 45;
            this.label3.Text = "Symbol";
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1,
            this.toolStripStatusLabel2,
            this.CIDtoolStripStatusLabel,
            this.toolStripStatusLabel3});
            this.statusStrip1.Location = new System.Drawing.Point(0, 489);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Padding = new System.Windows.Forms.Padding(1, 0, 13, 0);
            this.statusStrip1.Size = new System.Drawing.Size(1111, 22);
            this.statusStrip1.TabIndex = 31;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.AutoSize = false;
            this.toolStripStatusLabel1.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right;
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(300, 17);
            this.toolStripStatusLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // toolStripStatusLabel2
            // 
            this.toolStripStatusLabel2.AutoSize = false;
            this.toolStripStatusLabel2.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right;
            this.toolStripStatusLabel2.Name = "toolStripStatusLabel2";
            this.toolStripStatusLabel2.Size = new System.Drawing.Size(200, 17);
            this.toolStripStatusLabel2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // CIDtoolStripStatusLabel
            // 
            this.CIDtoolStripStatusLabel.Name = "CIDtoolStripStatusLabel";
            this.CIDtoolStripStatusLabel.Size = new System.Drawing.Size(0, 17);
            this.CIDtoolStripStatusLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // toolStripStatusLabel3
            // 
            this.toolStripStatusLabel3.Name = "toolStripStatusLabel3";
            this.toolStripStatusLabel3.Size = new System.Drawing.Size(0, 17);
            // 
            // toolStrip1
            // 
            this.toolStrip1.AutoSize = false;
            this.toolStrip1.BackColor = System.Drawing.SystemColors.Control;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton1,
            this.Connectbutton,
            this.Disconnectbutton,
            this.toolStripSeparator2,
            this.FRecover,
            this.CancelWorkingButton,
            this.toolStripLabel1,
            this.toolStripSeparator4,
            this.FutButton,
            this.OptButton,
            this.TseButton,
            this.OtcButton,
            this.PATSOButton,
            this.PATSButton,
            this.toolStripSeparator3,
            this.IPLabel,
            this.FIp,
            this.HostLabel,
            this.FPort});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(1111, 50);
            this.toolStrip1.TabIndex = 64;
            this.toolStrip1.Text = "toolStrip";
            // 
            // toolStripButton1
            // 
            this.toolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton1.Image")));
            this.toolStripButton1.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton1.Name = "toolStripButton1";
            this.toolStripButton1.Size = new System.Drawing.Size(36, 47);
            this.toolStripButton1.Text = "Setting";
            this.toolStripButton1.Click += new System.EventHandler(this.toolStripButton1_Click);
            // 
            // Connectbutton
            // 
            this.Connectbutton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.Connectbutton.Image = ((System.Drawing.Image)(resources.GetObject("Connectbutton.Image")));
            this.Connectbutton.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.Connectbutton.ImageTransparentColor = System.Drawing.Color.Orchid;
            this.Connectbutton.Name = "Connectbutton";
            this.Connectbutton.Size = new System.Drawing.Size(36, 47);
            this.Connectbutton.Text = "Connect";
            this.Connectbutton.Click += new System.EventHandler(this.Connectbutton_Click);
            // 
            // Disconnectbutton
            // 
            this.Disconnectbutton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.Disconnectbutton.Enabled = false;
            this.Disconnectbutton.Image = ((System.Drawing.Image)(resources.GetObject("Disconnectbutton.Image")));
            this.Disconnectbutton.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.Disconnectbutton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.Disconnectbutton.Name = "Disconnectbutton";
            this.Disconnectbutton.Size = new System.Drawing.Size(36, 47);
            this.Disconnectbutton.Text = "Disconnect";
            this.Disconnectbutton.Click += new System.EventHandler(this.Disconnectbutton_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 50);
            // 
            // FRecover
            // 
            this.FRecover.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.FRecover.Enabled = false;
            this.FRecover.Image = ((System.Drawing.Image)(resources.GetObject("FRecover.Image")));
            this.FRecover.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.FRecover.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.FRecover.Name = "FRecover";
            this.FRecover.Size = new System.Drawing.Size(36, 47);
            this.FRecover.Text = "Recover";
            this.FRecover.Click += new System.EventHandler(this.FRecover_Click);
            // 
            // CancelWorkingButton
            // 
            this.CancelWorkingButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.CancelWorkingButton.Enabled = false;
            this.CancelWorkingButton.Image = ((System.Drawing.Image)(resources.GetObject("CancelWorkingButton.Image")));
            this.CancelWorkingButton.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.CancelWorkingButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.CancelWorkingButton.Name = "CancelWorkingButton";
            this.CancelWorkingButton.Size = new System.Drawing.Size(28, 47);
            this.CancelWorkingButton.Text = "CancelWorkingButton";
            this.CancelWorkingButton.ToolTipText = "CancelWorkingButton";
            this.CancelWorkingButton.Click += new System.EventHandler(this.CancelWorkingButton_Click);
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.AutoSize = false;
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(110, 36);
            this.toolStripLabel1.Text = "                        ";
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(6, 50);
            // 
            // FutButton
            // 
            this.FutButton.AutoSize = false;
            this.FutButton.Checked = true;
            this.FutButton.CheckState = System.Windows.Forms.CheckState.Checked;
            this.FutButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.FutButton.Enabled = false;
            this.FutButton.Font = new System.Drawing.Font("Arial", 10.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FutButton.Image = ((System.Drawing.Image)(resources.GetObject("FutButton.Image")));
            this.FutButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.FutButton.Name = "FutButton";
            this.FutButton.Size = new System.Drawing.Size(50, 36);
            this.FutButton.Text = "Fut";
            this.FutButton.ToolTipText = "Futures";
            this.FutButton.Click += new System.EventHandler(this.FMarket_SelectedIndexChanged);
            // 
            // OptButton
            // 
            this.OptButton.AutoSize = false;
            this.OptButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.OptButton.Enabled = false;
            this.OptButton.Font = new System.Drawing.Font("Arial", 10.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.OptButton.Image = ((System.Drawing.Image)(resources.GetObject("OptButton.Image")));
            this.OptButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.OptButton.Name = "OptButton";
            this.OptButton.Size = new System.Drawing.Size(50, 36);
            this.OptButton.Text = "Opt";
            this.OptButton.ToolTipText = "Option";
            this.OptButton.Click += new System.EventHandler(this.FMarket_SelectedIndexChanged);
            // 
            // TseButton
            // 
            this.TseButton.AutoSize = false;
            this.TseButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.TseButton.Enabled = false;
            this.TseButton.Font = new System.Drawing.Font("Arial", 10.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.TseButton.Image = ((System.Drawing.Image)(resources.GetObject("TseButton.Image")));
            this.TseButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.TseButton.Name = "TseButton";
            this.TseButton.Size = new System.Drawing.Size(50, 36);
            this.TseButton.Text = "TSE";
            this.TseButton.ToolTipText = "Option";
            this.TseButton.Click += new System.EventHandler(this.FMarket_SelectedIndexChanged);
            // 
            // OtcButton
            // 
            this.OtcButton.AutoSize = false;
            this.OtcButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.OtcButton.Enabled = false;
            this.OtcButton.Font = new System.Drawing.Font("Arial", 10.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.OtcButton.Image = ((System.Drawing.Image)(resources.GetObject("OtcButton.Image")));
            this.OtcButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.OtcButton.Name = "OtcButton";
            this.OtcButton.Size = new System.Drawing.Size(50, 36);
            this.OtcButton.Text = "OTC";
            this.OtcButton.ToolTipText = "Option";
            this.OtcButton.Click += new System.EventHandler(this.FMarket_SelectedIndexChanged);
            // 
            // PATSOButton
            // 
            this.PATSOButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.PATSOButton.Enabled = false;
            this.PATSOButton.Font = new System.Drawing.Font("Arial", 10.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PATSOButton.Image = ((System.Drawing.Image)(resources.GetObject("PATSOButton.Image")));
            this.PATSOButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.PATSOButton.Name = "PATSOButton";
            this.PATSOButton.Size = new System.Drawing.Size(79, 47);
            this.PATSOButton.Text = "PATSO";
            this.PATSOButton.ToolTipText = "PATS";
            this.PATSOButton.Click += new System.EventHandler(this.FMarket_SelectedIndexChanged);
            // 
            // PATSButton
            // 
            this.PATSButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.PATSButton.Enabled = false;
            this.PATSButton.Font = new System.Drawing.Font("Arial", 10.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.PATSButton.Image = ((System.Drawing.Image)(resources.GetObject("PATSButton.Image")));
            this.PATSButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.PATSButton.Name = "PATSButton";
            this.PATSButton.Size = new System.Drawing.Size(64, 47);
            this.PATSButton.Text = "PATS";
            this.PATSButton.ToolTipText = "PATS";
            this.PATSButton.Click += new System.EventHandler(this.FMarket_SelectedIndexChanged);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 50);
            // 
            // IPLabel
            // 
            this.IPLabel.Name = "IPLabel";
            this.IPLabel.Size = new System.Drawing.Size(41, 47);
            this.IPLabel.Text = "Host";
            // 
            // FIp
            // 
            this.FIp.Name = "FIp";
            this.FIp.Size = new System.Drawing.Size(126, 50);
            this.FIp.Text = "192.168.168.120";
            // 
            // HostLabel
            // 
            this.HostLabel.Name = "HostLabel";
            this.HostLabel.Size = new System.Drawing.Size(38, 47);
            this.HostLabel.Text = "Port";
            // 
            // FPort
            // 
            this.FPort.Name = "FPort";
            this.FPort.Size = new System.Drawing.Size(60, 50);
            this.FPort.Text = "23456";
            // 
            // SendOrderTimer
            // 
            this.SendOrderTimer.Interval = 1000;
            this.SendOrderTimer.Tick += new System.EventHandler(this.SendOrderTimer_Tick);
            // 
            // dateTimePicker
            // 
            this.dateTimePicker.CustomFormat = "HH:mm:ss";
            this.dateTimePicker.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
            this.dateTimePicker.Location = new System.Drawing.Point(190, 11);
            this.dateTimePicker.Name = "dateTimePicker";
            this.dateTimePicker.ShowUpDown = true;
            this.dateTimePicker.Size = new System.Drawing.Size(95, 29);
            this.dateTimePicker.TabIndex = 65;
            // 
            // clockTimer
            // 
            this.clockTimer.Enabled = true;
            this.clockTimer.Interval = 60000;
            this.clockTimer.Tick += new System.EventHandler(this.clockTimer_Tick);
            // 
            // panelFilter
            // 
            this.panelFilter.Controls.Add(this.groupBox1);
            this.panelFilter.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelFilter.Enabled = false;
            this.panelFilter.Location = new System.Drawing.Point(0, 50);
            this.panelFilter.Name = "panelFilter";
            this.panelFilter.Size = new System.Drawing.Size(1111, 60);
            this.panelFilter.TabIndex = 66;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.AccounttextBox);
            this.groupBox1.Controls.Add(this.label16);
            this.groupBox1.Controls.Add(this.button2);
            this.groupBox1.Controls.Add(this.ShowFillcheckBox);
            this.groupBox1.Controls.Add(this.BSComboBox);
            this.groupBox1.Controls.Add(this.label13);
            this.groupBox1.Controls.Add(this.MarketComboBox);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.AccountComboBox);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Location = new System.Drawing.Point(12, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(1045, 54);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Filter";
            // 
            // AccounttextBox
            // 
            this.AccounttextBox.ForeColor = System.Drawing.SystemColors.WindowText;
            this.AccounttextBox.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.AccounttextBox.Location = new System.Drawing.Point(840, 16);
            this.AccounttextBox.MaxLength = 20;
            this.AccounttextBox.Name = "AccounttextBox";
            this.AccounttextBox.Size = new System.Drawing.Size(81, 29);
            this.AccounttextBox.TabIndex = 77;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(764, 22);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(70, 19);
            this.label16.TabIndex = 76;
            this.label16.Text = "Account";
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(632, 17);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(126, 27);
            this.button2.TabIndex = 71;
            this.button2.Text = "QuoteRequest";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click_1);
            // 
            // ShowFillcheckBox
            // 
            this.ShowFillcheckBox.AutoSize = true;
            this.ShowFillcheckBox.Location = new System.Drawing.Point(464, 20);
            this.ShowFillcheckBox.Name = "ShowFillcheckBox";
            this.ShowFillcheckBox.Size = new System.Drawing.Size(165, 23);
            this.ShowFillcheckBox.TabIndex = 75;
            this.ShowFillcheckBox.Text = "Show filled orders";
            this.ShowFillcheckBox.UseVisualStyleBackColor = true;
            // 
            // BSComboBox
            // 
            this.BSComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.BSComboBox.FormattingEnabled = true;
            this.BSComboBox.Items.AddRange(new object[] {
            "All",
            "Buy",
            "Sell"});
            this.BSComboBox.Location = new System.Drawing.Point(362, 18);
            this.BSComboBox.Name = "BSComboBox";
            this.BSComboBox.Size = new System.Drawing.Size(87, 26);
            this.BSComboBox.TabIndex = 74;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(322, 21);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(41, 19);
            this.label13.TabIndex = 73;
            this.label13.Text = "Side";
            // 
            // MarketComboBox
            // 
            this.MarketComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.MarketComboBox.FormattingEnabled = true;
            this.MarketComboBox.Items.AddRange(new object[] {
            "All",
            "Futures",
            "Option",
            "TSE",
            "OTC"});
            this.MarketComboBox.Location = new System.Drawing.Point(229, 18);
            this.MarketComboBox.Name = "MarketComboBox";
            this.MarketComboBox.Size = new System.Drawing.Size(87, 26);
            this.MarketComboBox.TabIndex = 72;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(164, 21);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(61, 19);
            this.label9.TabIndex = 71;
            this.label9.Text = "Market";
            // 
            // AccountComboBox
            // 
            this.AccountComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.AccountComboBox.FormattingEnabled = true;
            this.AccountComboBox.Items.AddRange(new object[] {
            "All"});
            this.AccountComboBox.Location = new System.Drawing.Point(76, 19);
            this.AccountComboBox.Name = "AccountComboBox";
            this.AccountComboBox.Size = new System.Drawing.Size(87, 26);
            this.AccountComboBox.TabIndex = 70;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(5, 21);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(70, 19);
            this.label6.TabIndex = 69;
            this.label6.Text = "Account";
            // 
            // imageList1
            // 
            this.imageList1.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
            this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            // 
            // TOrderForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1111, 511);
            this.Controls.Add(this.panelFilter);
            this.Controls.Add(this.dateTimePicker);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.Orderpanel);
            this.Controls.Add(this.FGrid);
            this.Font = new System.Drawing.Font("PMingLiU", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(542, 367);
            this.Name = "TOrderForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Speedy Order Client";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.TOrderForm_FormClosing);
            this.Load += new System.EventHandler(this.TOrderForm_Load);
            this.Shown += new System.EventHandler(this.TOrderForm_Shown);
            ((System.ComponentModel.ISupportInitialize)(this.FGrid)).EndInit();
            this.Orderpanel.ResumeLayout(false);
            this.Orderpanel.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.panelFilter.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private C1.Win.C1FlexGrid.C1FlexGrid FGrid;
        private System.Windows.Forms.Timer FQuoteRequestTimer;
        private System.Windows.Forms.Panel Orderpanel;
        private System.Windows.Forms.Button FNewOrder;
        private System.Windows.Forms.TextBox FQty;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.ComboBox FTimeInForce;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ComboBox FSide;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox FPrice;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox FOrderType;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox FSymbol;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton Connectbutton;
        private System.Windows.Forms.ToolStripButton Disconnectbutton;
        private System.Windows.Forms.ToolStripButton FRecover;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        public System.Windows.Forms.ToolStripLabel IPLabel;
        private System.Windows.Forms.ToolStripTextBox FIp;
        private System.Windows.Forms.ToolStripLabel HostLabel;
        private System.Windows.Forms.ToolStripTextBox FPort;
        private System.Windows.Forms.Label Countlabel;
        private System.Windows.Forms.TextBox FSliceQty;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Timer SendOrderTimer;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel2;
        private System.Windows.Forms.ToolStripButton FutButton;
        private System.Windows.Forms.ToolStripButton OptButton;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton toolStripButton1;
        private System.Windows.Forms.ToolStripButton TseButton;
        private System.Windows.Forms.ToolStripButton OtcButton;
        private System.Windows.Forms.ComboBox SessionComboBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox StockSeqTextBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ToolStripStatusLabel CIDtoolStripStatusLabel;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel3;
        private System.Windows.Forms.DateTimePicker dateTimePicker;
        private System.Windows.Forms.Timer clockTimer;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.Panel panelFilter;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.ComboBox PEcomboBox;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.CheckBox ShowFillcheckBox;
        private System.Windows.Forms.ComboBox BSComboBox;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.ComboBox MarketComboBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.ComboBox AccountComboBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ToolStripButton PATSButton;
        private System.Windows.Forms.TextBox FExchange;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TextBox FMaturityMonthYear;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox AccounttextBox;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.ToolStripButton PATSOButton;
        private System.Windows.Forms.ToolStripButton CancelWorkingButton;
    }
}