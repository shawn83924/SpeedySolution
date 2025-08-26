namespace WindowsApplication1
{
    partial class MainForm
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
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel2 = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel3 = new System.Windows.Forms.ToolStripStatusLabel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.ClearButton = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.KeytextBox = new System.Windows.Forms.TextBox();
            this.SubjecttextBox = new System.Windows.Forms.TextBox();
            this.IPtextBox = new System.Windows.Forms.TextBox();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.DisconnectBtn = new System.Windows.Forms.Button();
            this.ConnectBtn = new System.Windows.Forms.Button();
            this.SendBtn = new System.Windows.Forms.Button();
            this.FMsgTreeView = new System.Windows.Forms.TreeView();
            this.statusStrip1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // timer
            // 
            this.timer.Interval = 1000;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel1,
            this.toolStripStatusLabel2,
            this.toolStripStatusLabel3});
            this.statusStrip1.Location = new System.Drawing.Point(0, 491);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Padding = new System.Windows.Forms.Padding(1, 0, 19, 0);
            this.statusStrip1.Size = new System.Drawing.Size(761, 24);
            this.statusStrip1.TabIndex = 9;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.AutoSize = false;
            this.toolStripStatusLabel1.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right;
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(104, 19);
            this.toolStripStatusLabel1.Text = "Sent:";
            this.toolStripStatusLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // toolStripStatusLabel2
            // 
            this.toolStripStatusLabel2.AutoSize = false;
            this.toolStripStatusLabel2.BorderSides = System.Windows.Forms.ToolStripStatusLabelBorderSides.Right;
            this.toolStripStatusLabel2.Name = "toolStripStatusLabel2";
            this.toolStripStatusLabel2.Size = new System.Drawing.Size(104, 19);
            this.toolStripStatusLabel2.Text = "Recv:";
            this.toolStripStatusLabel2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // toolStripStatusLabel3
            // 
            this.toolStripStatusLabel3.Name = "toolStripStatusLabel3";
            this.toolStripStatusLabel3.Size = new System.Drawing.Size(147, 19);
            this.toolStripStatusLabel3.Text = "MBus CSharp Client";
            this.toolStripStatusLabel3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.ClearButton);
            this.panel1.Controls.Add(this.label3);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Controls.Add(this.KeytextBox);
            this.panel1.Controls.Add(this.SubjecttextBox);
            this.panel1.Controls.Add(this.IPtextBox);
            this.panel1.Controls.Add(this.checkBox1);
            this.panel1.Controls.Add(this.DisconnectBtn);
            this.panel1.Controls.Add(this.ConnectBtn);
            this.panel1.Controls.Add(this.SendBtn);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(761, 128);
            this.panel1.TabIndex = 16;
            // 
            // ClearButton
            // 
            this.ClearButton.Location = new System.Drawing.Point(293, 92);
            this.ClearButton.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.ClearButton.Name = "ClearButton";
            this.ClearButton.Size = new System.Drawing.Size(163, 29);
            this.ClearButton.TabIndex = 27;
            this.ClearButton.Text = "Clear Messages";
            this.ClearButton.UseVisualStyleBackColor = true;
            this.ClearButton.Click += new System.EventHandler(this.ClearButton_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(64, 75);
            this.label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(30, 15);
            this.label3.TabIndex = 26;
            this.label3.Text = "Key";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(44, 44);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(49, 15);
            this.label2.TabIndex = 25;
            this.label2.Text = "Subject";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(8, 15);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(82, 15);
            this.label1.TabIndex = 24;
            this.label1.Text = "MBus Server";
            // 
            // KeytextBox
            // 
            this.KeytextBox.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::WindowsApplication1.Properties.Settings.Default, "MBusKey", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.KeytextBox.Location = new System.Drawing.Point(104, 71);
            this.KeytextBox.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.KeytextBox.Name = "KeytextBox";
            this.KeytextBox.Size = new System.Drawing.Size(169, 25);
            this.KeytextBox.TabIndex = 23;
            this.KeytextBox.Text = global::WindowsApplication1.Properties.Settings.Default.MBusKey;
            // 
            // SubjecttextBox
            // 
            this.SubjecttextBox.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::WindowsApplication1.Properties.Settings.Default, "MBusSubject", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.SubjecttextBox.Location = new System.Drawing.Point(104, 40);
            this.SubjecttextBox.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.SubjecttextBox.Name = "SubjecttextBox";
            this.SubjecttextBox.Size = new System.Drawing.Size(169, 25);
            this.SubjecttextBox.TabIndex = 22;
            this.SubjecttextBox.Text = global::WindowsApplication1.Properties.Settings.Default.MBusSubject;
            // 
            // IPtextBox
            // 
            this.IPtextBox.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::WindowsApplication1.Properties.Settings.Default, "MBusIP", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
            this.IPtextBox.Location = new System.Drawing.Point(104, 9);
            this.IPtextBox.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.IPtextBox.Name = "IPtextBox";
            this.IPtextBox.Size = new System.Drawing.Size(169, 25);
            this.IPtextBox.TabIndex = 21;
            this.IPtextBox.Text = global::WindowsApplication1.Properties.Settings.Default.MBusIP;
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(104, 105);
            this.checkBox1.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(151, 19);
            this.checkBox1.TabIndex = 20;
            this.checkBox1.Text = "Show Message Detail";
            this.checkBox1.UseVisualStyleBackColor = true;
            // 
            // DisconnectBtn
            // 
            this.DisconnectBtn.Enabled = false;
            this.DisconnectBtn.Location = new System.Drawing.Point(293, 32);
            this.DisconnectBtn.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.DisconnectBtn.Name = "DisconnectBtn";
            this.DisconnectBtn.Size = new System.Drawing.Size(163, 29);
            this.DisconnectBtn.TabIndex = 18;
            this.DisconnectBtn.Text = "Disconnect";
            this.DisconnectBtn.UseVisualStyleBackColor = true;
            this.DisconnectBtn.Click += new System.EventHandler(this.DisconnectBtn_Click);
            // 
            // ConnectBtn
            // 
            this.ConnectBtn.Location = new System.Drawing.Point(293, 4);
            this.ConnectBtn.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.ConnectBtn.Name = "ConnectBtn";
            this.ConnectBtn.Size = new System.Drawing.Size(163, 29);
            this.ConnectBtn.TabIndex = 17;
            this.ConnectBtn.Text = "Connect";
            this.ConnectBtn.UseVisualStyleBackColor = true;
            this.ConnectBtn.Click += new System.EventHandler(this.ConnectBtn_Click);
            // 
            // SendBtn
            // 
            this.SendBtn.Enabled = false;
            this.SendBtn.Location = new System.Drawing.Point(293, 62);
            this.SendBtn.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.SendBtn.Name = "SendBtn";
            this.SendBtn.Size = new System.Drawing.Size(163, 29);
            this.SendBtn.TabIndex = 16;
            this.SendBtn.Text = "Sending";
            this.SendBtn.UseVisualStyleBackColor = true;
            this.SendBtn.Click += new System.EventHandler(this.SendBtn_Click);
            // 
            // FMsgTreeView
            // 
            this.FMsgTreeView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.FMsgTreeView.Location = new System.Drawing.Point(3, 129);
            this.FMsgTreeView.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.FMsgTreeView.Name = "FMsgTreeView";
            this.FMsgTreeView.Size = new System.Drawing.Size(755, 355);
            this.FMsgTreeView.TabIndex = 17;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(761, 515);
            this.Controls.Add(this.FMsgTreeView);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.statusStrip1);
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "MainForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Show;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "MBus Benchmark";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel2;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel3;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox KeytextBox;
        private System.Windows.Forms.TextBox SubjecttextBox;
        private System.Windows.Forms.TextBox IPtextBox;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.Button DisconnectBtn;
        private System.Windows.Forms.Button ConnectBtn;
        private System.Windows.Forms.Button SendBtn;
        private System.Windows.Forms.TreeView FMsgTreeView;
        private System.Windows.Forms.Button ClearButton;
    }
}

