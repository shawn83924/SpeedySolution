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
            this.Orderpanel = new System.Windows.Forms.Panel();
            this.label5 = new System.Windows.Forms.Label();
            this.PEcomboBox = new System.Windows.Forms.ComboBox();
            this.FTimeInForce = new System.Windows.Forms.ComboBox();
            this.label11 = new System.Windows.Forms.Label();
            this.FOrderType = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripProgressBar = new System.Windows.Forms.ToolStripProgressBar();
            this.toolStripStatusLabel2 = new System.Windows.Forms.ToolStripStatusLabel();
            this.CIDtoolStripStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel3 = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.Connectbutton = new System.Windows.Forms.ToolStripButton();
            this.Disconnectbutton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.FRecover = new System.Windows.Forms.ToolStripButton();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.dateTimePicker = new System.Windows.Forms.DateTimePicker();
            this.clockTimer = new System.Windows.Forms.Timer(this.components);
            this.panelFilter = new System.Windows.Forms.Panel();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.ShowFillcheckBox = new System.Windows.Forms.CheckBox();
            this.BSComboBox = new System.Windows.Forms.ComboBox();
            this.label13 = new System.Windows.Forms.Label();
            this.MarketComboBox = new System.Windows.Forms.ComboBox();
            this.label9 = new System.Windows.Forms.Label();
            this.AccountComboBox = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.timerShow = new System.Windows.Forms.Timer(this.components);
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
            this.FGrid.Location = new System.Drawing.Point(4, 156);
            this.FGrid.Name = "FGrid";
            this.FGrid.Rows.Count = 1;
            this.FGrid.Rows.DefaultSize = 28;
            this.FGrid.SelectionMode = C1.Win.C1FlexGrid.SelectionModeEnum.ListBox;
            this.FGrid.Size = new System.Drawing.Size(937, 304);
            this.FGrid.StyleInfo = resources.GetString("FGrid.StyleInfo");
            this.FGrid.TabIndex = 25;
            this.FGrid.SelChange += new System.EventHandler(this.FGrid_SelChange);
            this.FGrid.OwnerDrawCell += new C1.Win.C1FlexGrid.OwnerDrawCellEventHandler(this.FGrid_OwnerDrawCell);
            this.FGrid.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FGrid_KeyDown);
            this.FGrid.MouseClick += new System.Windows.Forms.MouseEventHandler(this.FGrid_MouseClick);
            // 
            // Orderpanel
            // 
            this.Orderpanel.Controls.Add(this.label5);
            this.Orderpanel.Controls.Add(this.PEcomboBox);
            this.Orderpanel.Controls.Add(this.FTimeInForce);
            this.Orderpanel.Controls.Add(this.label11);
            this.Orderpanel.Controls.Add(this.FOrderType);
            this.Orderpanel.Controls.Add(this.label7);
            this.Orderpanel.Enabled = false;
            this.Orderpanel.Location = new System.Drawing.Point(4, 112);
            this.Orderpanel.Name = "Orderpanel";
            this.Orderpanel.Size = new System.Drawing.Size(941, 43);
            this.Orderpanel.TabIndex = 30;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(445, 12);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(112, 19);
            this.label5.TabIndex = 72;
            this.label5.Text = "PositionEffect";
            // 
            // PEcomboBox
            // 
            this.PEcomboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.PEcomboBox.FormattingEnabled = true;
            this.PEcomboBox.Location = new System.Drawing.Point(559, 9);
            this.PEcomboBox.Name = "PEcomboBox";
            this.PEcomboBox.Size = new System.Drawing.Size(125, 26);
            this.PEcomboBox.TabIndex = 71;
            // 
            // FTimeInForce
            // 
            this.FTimeInForce.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.FTimeInForce.FormattingEnabled = true;
            this.FTimeInForce.Location = new System.Drawing.Point(117, 9);
            this.FTimeInForce.Name = "FTimeInForce";
            this.FTimeInForce.Size = new System.Drawing.Size(111, 26);
            this.FTimeInForce.TabIndex = 61;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(13, 12);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(103, 19);
            this.label11.TabIndex = 60;
            this.label11.Text = "TimeInForce";
            // 
            // FOrderType
            // 
            this.FOrderType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.FOrderType.FormattingEnabled = true;
            this.FOrderType.Location = new System.Drawing.Point(329, 9);
            this.FOrderType.Name = "FOrderType";
            this.FOrderType.Size = new System.Drawing.Size(105, 26);
            this.FOrderType.TabIndex = 53;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(239, 12);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(88, 19);
            this.label7.TabIndex = 52;
            this.label7.Text = "OrderType";
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1,
            this.toolStripProgressBar,
            this.toolStripStatusLabel2,
            this.CIDtoolStripStatusLabel,
            this.toolStripStatusLabel3});
            this.statusStrip1.Location = new System.Drawing.Point(0, 464);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Padding = new System.Windows.Forms.Padding(1, 0, 13, 0);
            this.statusStrip1.Size = new System.Drawing.Size(945, 24);
            this.statusStrip1.TabIndex = 31;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.AutoSize = false;
            this.toolStripStatusLabel1.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right;
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(300, 19);
            this.toolStripStatusLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // toolStripProgressBar
            // 
            this.toolStripProgressBar.Name = "toolStripProgressBar";
            this.toolStripProgressBar.Size = new System.Drawing.Size(150, 18);
            this.toolStripProgressBar.Step = 1;
            this.toolStripProgressBar.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.toolStripProgressBar.Visible = false;
            // 
            // toolStripStatusLabel2
            // 
            this.toolStripStatusLabel2.AutoSize = false;
            this.toolStripStatusLabel2.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right;
            this.toolStripStatusLabel2.Name = "toolStripStatusLabel2";
            this.toolStripStatusLabel2.Size = new System.Drawing.Size(200, 19);
            this.toolStripStatusLabel2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // CIDtoolStripStatusLabel
            // 
            this.CIDtoolStripStatusLabel.Name = "CIDtoolStripStatusLabel";
            this.CIDtoolStripStatusLabel.Size = new System.Drawing.Size(0, 19);
            this.CIDtoolStripStatusLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // toolStripStatusLabel3
            // 
            this.toolStripStatusLabel3.AutoSize = false;
            this.toolStripStatusLabel3.Name = "toolStripStatusLabel3";
            this.toolStripStatusLabel3.Size = new System.Drawing.Size(0, 19);
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
            this.toolStripLabel1,
            this.toolStripSeparator4,
            this.toolStripSeparator3});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(945, 50);
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
            this.FRecover.AutoSize = false;
            this.FRecover.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.FRecover.Enabled = false;
            this.FRecover.Image = ((System.Drawing.Image)(resources.GetObject("FRecover.Image")));
            this.FRecover.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.FRecover.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.FRecover.Name = "FRecover";
            this.FRecover.Size = new System.Drawing.Size(48, 47);
            this.FRecover.Text = "Recover";
            this.FRecover.Click += new System.EventHandler(this.FRecover_Click);
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
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 50);
            // 
            // dateTimePicker
            // 
            this.dateTimePicker.CustomFormat = "HH:mm:ss";
            this.dateTimePicker.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
            this.dateTimePicker.Location = new System.Drawing.Point(177, 11);
            this.dateTimePicker.Name = "dateTimePicker";
            this.dateTimePicker.ShowUpDown = true;
            this.dateTimePicker.Size = new System.Drawing.Size(94, 29);
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
            this.panelFilter.Size = new System.Drawing.Size(945, 60);
            this.panelFilter.TabIndex = 66;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.ShowFillcheckBox);
            this.groupBox1.Controls.Add(this.BSComboBox);
            this.groupBox1.Controls.Add(this.label13);
            this.groupBox1.Controls.Add(this.MarketComboBox);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.AccountComboBox);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Location = new System.Drawing.Point(12, 3);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(826, 54);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Filter";
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
            this.ShowFillcheckBox.CheckedChanged += new System.EventHandler(this.ShowFillcheckBox_CheckedChanged);
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
            this.BSComboBox.SelectedIndexChanged += new System.EventHandler(this.AccountComboBox_SelectedIndexChanged);
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
            this.MarketComboBox.SelectedIndexChanged += new System.EventHandler(this.AccountComboBox_SelectedIndexChanged);
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
            this.AccountComboBox.SelectedIndexChanged += new System.EventHandler(this.AccountComboBox_SelectedIndexChanged);
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
            // timerShow
            // 
            this.timerShow.Interval = 600;
            this.timerShow.Tick += new System.EventHandler(this.timerShow_Tick);
            // 
            // TOrderForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(945, 488);
            this.Controls.Add(this.panelFilter);
            this.Controls.Add(this.dateTimePicker);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.Orderpanel);
            this.Controls.Add(this.FGrid);
            this.Font = new System.Drawing.Font("PMingLiU", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
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
        private System.Windows.Forms.Panel Orderpanel;
        private System.Windows.Forms.ComboBox FTimeInForce;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ComboBox FOrderType;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton Connectbutton;
        private System.Windows.Forms.ToolStripButton Disconnectbutton;
        private System.Windows.Forms.ToolStripButton FRecover;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel2;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton toolStripButton1;
        private System.Windows.Forms.ToolStripStatusLabel CIDtoolStripStatusLabel;
        private System.Windows.Forms.DateTimePicker dateTimePicker;
        private System.Windows.Forms.Timer clockTimer;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.Panel panelFilter;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.ComboBox PEcomboBox;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox ShowFillcheckBox;
        private System.Windows.Forms.ComboBox BSComboBox;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.ComboBox MarketComboBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.ComboBox AccountComboBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ToolStripProgressBar toolStripProgressBar;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel3;
        private System.Windows.Forms.Timer timerShow;
    }
}