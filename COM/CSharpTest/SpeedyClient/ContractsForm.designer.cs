namespace Test1
{
    partial class ContractsForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ContractsForm));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.DownloadButton = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.CommodTreeView = new System.Windows.Forms.TreeView();
            this.ContractMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.depthToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.candlestickToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStrip1.SuspendLayout();
            this.ContractMenuStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.AutoSize = false;
            this.toolStrip1.BackColor = System.Drawing.SystemColors.Control;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.DownloadButton,
            this.toolStripSeparator2});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(300, 49);
            this.toolStrip1.TabIndex = 65;
            this.toolStrip1.Text = "toolStrip";
            // 
            // DownloadButton
            // 
            this.DownloadButton.AutoSize = false;
            this.DownloadButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.DownloadButton.Image = ((System.Drawing.Image)(resources.GetObject("DownloadButton.Image")));
            this.DownloadButton.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.DownloadButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.DownloadButton.Name = "DownloadButton";
            this.DownloadButton.Size = new System.Drawing.Size(40, 46);
            this.DownloadButton.Text = "Download Contracts";
            this.DownloadButton.Click += new System.EventHandler(this.DownloadButton_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 49);
            // 
            // CommodTreeView
            // 
            this.CommodTreeView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.CommodTreeView.ContextMenuStrip = this.ContractMenuStrip;
            this.CommodTreeView.Font = new System.Drawing.Font("PMingLiU", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.CommodTreeView.Location = new System.Drawing.Point(0, 52);
            this.CommodTreeView.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.CommodTreeView.Name = "CommodTreeView";
            this.CommodTreeView.Size = new System.Drawing.Size(300, 502);
            this.CommodTreeView.TabIndex = 66;
            this.CommodTreeView.DoubleClick += new System.EventHandler(this.CommodTreeView_DoubleClick);
            // 
            // ContractMenuStrip
            // 
            this.ContractMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.depthToolStripMenuItem,
            this.candlestickToolStripMenuItem});
            this.ContractMenuStrip.Name = "contextMenuStrip1";
            this.ContractMenuStrip.Size = new System.Drawing.Size(159, 74);
            // 
            // depthToolStripMenuItem
            // 
            this.depthToolStripMenuItem.Name = "depthToolStripMenuItem";
            this.depthToolStripMenuItem.Size = new System.Drawing.Size(158, 24);
            this.depthToolStripMenuItem.Text = "Depth";
            this.depthToolStripMenuItem.Click += new System.EventHandler(this.CommodTreeView_DoubleClick);
            // 
            // candlestickToolStripMenuItem
            // 
            this.candlestickToolStripMenuItem.Name = "candlestickToolStripMenuItem";
            this.candlestickToolStripMenuItem.Size = new System.Drawing.Size(158, 24);
            this.candlestickToolStripMenuItem.Text = "Candlestick";
            this.candlestickToolStripMenuItem.Click += new System.EventHandler(this.candlestickToolStripMenuItem_Click);
            // 
            // ContractsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(300, 552);
            this.Controls.Add(this.CommodTreeView);
            this.Controls.Add(this.toolStrip1);
            this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.Name = "ContractsForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Contracts";
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.ContractMenuStrip.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripButton DownloadButton;
        private System.Windows.Forms.TreeView CommodTreeView;
        private System.Windows.Forms.ContextMenuStrip ContractMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem depthToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem candlestickToolStripMenuItem;
    }
}

