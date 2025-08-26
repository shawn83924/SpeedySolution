namespace Test1
{
    partial class Flash10
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Flash10));
            this.c1FlexGrid1 = new C1.Win.C1FlexGrid.C1FlexGrid();
            this.pnSetting = new System.Windows.Forms.Panel();
            this.btnFontDecrease = new System.Windows.Forms.Button();
            this.btnFontIncrease = new System.Windows.Forms.Button();
            this.btnBuyQty10 = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.btnBuyQty05 = new System.Windows.Forms.Button();
            this.cbCenter5 = new System.Windows.Forms.CheckBox();
            this.btnBuyQty02 = new System.Windows.Forms.Button();
            this.gbSell = new System.Windows.Forms.GroupBox();
            this.cbSellTimeInForce = new System.Windows.Forms.ComboBox();
            this.btnBuyQty01 = new System.Windows.Forms.Button();
            this.gbBuy = new System.Windows.Forms.GroupBox();
            this.cbBuyTimeInForce = new System.Windows.Forms.ComboBox();
            this.nudOrderQty = new System.Windows.Forms.NumericUpDown();
            this.cbCommodity = new System.Windows.Forms.ComboBox();
            ((System.ComponentModel.ISupportInitialize)(this.c1FlexGrid1)).BeginInit();
            this.pnSetting.SuspendLayout();
            this.gbSell.SuspendLayout();
            this.gbBuy.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudOrderQty)).BeginInit();
            this.SuspendLayout();
            // 
            // c1FlexGrid1
            // 
            this.c1FlexGrid1.AllowDragging = C1.Win.C1FlexGrid.AllowDraggingEnum.None;
            this.c1FlexGrid1.AllowEditing = false;
            this.c1FlexGrid1.AllowResizing = C1.Win.C1FlexGrid.AllowResizingEnum.None;
            this.c1FlexGrid1.AllowSorting = C1.Win.C1FlexGrid.AllowSortingEnum.None;
            this.c1FlexGrid1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.c1FlexGrid1.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.c1FlexGrid1.BorderStyle = C1.Win.C1FlexGrid.Util.BaseControls.BorderStyleEnum.FixedSingle;
            this.c1FlexGrid1.ColumnInfo = "0,0,0,0,0,110,Columns:";
            this.c1FlexGrid1.DragMode = C1.Win.C1FlexGrid.DragModeEnum.Automatic;
            this.c1FlexGrid1.DrawMode = C1.Win.C1FlexGrid.DrawModeEnum.OwnerDraw;
            this.c1FlexGrid1.DropMode = C1.Win.C1FlexGrid.DropModeEnum.Automatic;
            this.c1FlexGrid1.Font = new System.Drawing.Font("Verdana", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.c1FlexGrid1.HighLight = C1.Win.C1FlexGrid.HighLightEnum.Never;
            this.c1FlexGrid1.Location = new System.Drawing.Point(2, 78);
            this.c1FlexGrid1.Name = "c1FlexGrid1";
            this.c1FlexGrid1.Rows.Count = 2;
            this.c1FlexGrid1.Rows.DefaultSize = 22;
            this.c1FlexGrid1.Rows.Fixed = 2;
            this.c1FlexGrid1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.c1FlexGrid1.SelectionMode = C1.Win.C1FlexGrid.SelectionModeEnum.Cell;
            this.c1FlexGrid1.Size = new System.Drawing.Size(567, 604);
            this.c1FlexGrid1.StyleInfo = resources.GetString("c1FlexGrid1.StyleInfo");
            this.c1FlexGrid1.TabIndex = 2;
            this.c1FlexGrid1.MouseLeaveCell += new C1.Win.C1FlexGrid.RowColEventHandler(this.c1FlexGrid1_MouseLeaveCell);
            this.c1FlexGrid1.OwnerDrawCell += new C1.Win.C1FlexGrid.OwnerDrawCellEventHandler(this.c1FlexGrid1_OwnerDrawCell);
            this.c1FlexGrid1.DragDrop += new System.Windows.Forms.DragEventHandler(this.c1FlexGrid1_DragDrop);
            this.c1FlexGrid1.DragOver += new System.Windows.Forms.DragEventHandler(this.c1FlexGrid1_DragOver);
            this.c1FlexGrid1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.c1FlexGrid1_MouseClick);
            this.c1FlexGrid1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.c1FlexGrid1_MouseDown);
            this.c1FlexGrid1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.c1FlexGrid1_MouseMove);
            this.c1FlexGrid1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.c1FlexGrid1_MouseUp);
            // 
            // pnSetting
            // 
            this.pnSetting.Controls.Add(this.btnFontDecrease);
            this.pnSetting.Controls.Add(this.btnFontIncrease);
            this.pnSetting.Controls.Add(this.btnBuyQty10);
            this.pnSetting.Controls.Add(this.label1);
            this.pnSetting.Controls.Add(this.btnBuyQty05);
            this.pnSetting.Controls.Add(this.cbCenter5);
            this.pnSetting.Controls.Add(this.btnBuyQty02);
            this.pnSetting.Controls.Add(this.gbSell);
            this.pnSetting.Controls.Add(this.btnBuyQty01);
            this.pnSetting.Controls.Add(this.gbBuy);
            this.pnSetting.Controls.Add(this.nudOrderQty);
            this.pnSetting.Controls.Add(this.cbCommodity);
            this.pnSetting.Location = new System.Drawing.Point(2, 2);
            this.pnSetting.Name = "pnSetting";
            this.pnSetting.Size = new System.Drawing.Size(501, 77);
            this.pnSetting.TabIndex = 3;
            // 
            // btnFontDecrease
            // 
            this.btnFontDecrease.Enabled = false;
            this.btnFontDecrease.Location = new System.Drawing.Point(334, 44);
            this.btnFontDecrease.Name = "btnFontDecrease";
            this.btnFontDecrease.Size = new System.Drawing.Size(27, 26);
            this.btnFontDecrease.TabIndex = 11;
            this.btnFontDecrease.Text = "A-";
            this.btnFontDecrease.UseVisualStyleBackColor = true;
            this.btnFontDecrease.Click += new System.EventHandler(this.btnFontDecrease_Click);
            // 
            // btnFontIncrease
            // 
            this.btnFontIncrease.Location = new System.Drawing.Point(307, 44);
            this.btnFontIncrease.Name = "btnFontIncrease";
            this.btnFontIncrease.Size = new System.Drawing.Size(27, 26);
            this.btnFontIncrease.TabIndex = 10;
            this.btnFontIncrease.Text = "A+";
            this.btnFontIncrease.UseVisualStyleBackColor = true;
            this.btnFontIncrease.Click += new System.EventHandler(this.btnFontIncrease_Click);
            // 
            // btnBuyQty10
            // 
            this.btnBuyQty10.Font = new System.Drawing.Font("新細明體", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.btnBuyQty10.Location = new System.Drawing.Point(270, 44);
            this.btnBuyQty10.Name = "btnBuyQty10";
            this.btnBuyQty10.Padding = new System.Windows.Forms.Padding(3);
            this.btnBuyQty10.Size = new System.Drawing.Size(31, 26);
            this.btnBuyQty10.TabIndex = 8;
            this.btnBuyQty10.Text = "10";
            this.btnBuyQty10.UseVisualStyleBackColor = true;
            this.btnBuyQty10.Click += new System.EventHandler(this.btnBuyQty10_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(87, 51);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 8;
            this.label1.Text = "下單數量";
            // 
            // btnBuyQty05
            // 
            this.btnBuyQty05.Font = new System.Drawing.Font("新細明體", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.btnBuyQty05.Location = new System.Drawing.Point(240, 44);
            this.btnBuyQty05.Name = "btnBuyQty05";
            this.btnBuyQty05.Padding = new System.Windows.Forms.Padding(3);
            this.btnBuyQty05.Size = new System.Drawing.Size(31, 26);
            this.btnBuyQty05.TabIndex = 7;
            this.btnBuyQty05.Text = "5";
            this.btnBuyQty05.UseVisualStyleBackColor = true;
            this.btnBuyQty05.Click += new System.EventHandler(this.btnBuyQty05_Click);
            // 
            // cbCenter5
            // 
            this.cbCenter5.AutoSize = true;
            this.cbCenter5.Checked = true;
            this.cbCenter5.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbCenter5.Location = new System.Drawing.Point(204, 8);
            this.cbCenter5.Name = "cbCenter5";
            this.cbCenter5.Size = new System.Drawing.Size(72, 16);
            this.cbCenter5.TabIndex = 7;
            this.cbCenter5.Text = "成交置中";
            this.cbCenter5.UseVisualStyleBackColor = true;
            // 
            // btnBuyQty02
            // 
            this.btnBuyQty02.Font = new System.Drawing.Font("新細明體", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.btnBuyQty02.Location = new System.Drawing.Point(210, 44);
            this.btnBuyQty02.Name = "btnBuyQty02";
            this.btnBuyQty02.Size = new System.Drawing.Size(31, 26);
            this.btnBuyQty02.TabIndex = 6;
            this.btnBuyQty02.Text = "2";
            this.btnBuyQty02.UseVisualStyleBackColor = true;
            this.btnBuyQty02.Click += new System.EventHandler(this.btnBuyQty02_Click);
            // 
            // gbSell
            // 
            this.gbSell.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.gbSell.Controls.Add(this.cbSellTimeInForce);
            this.gbSell.Location = new System.Drawing.Point(429, 28);
            this.gbSell.Name = "gbSell";
            this.gbSell.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
            this.gbSell.Size = new System.Drawing.Size(69, 40);
            this.gbSell.TabIndex = 6;
            this.gbSell.TabStop = false;
            this.gbSell.Text = "Sell";
            // 
            // cbSellTimeInForce
            // 
            this.cbSellTimeInForce.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbSellTimeInForce.FormattingEnabled = true;
            this.cbSellTimeInForce.Location = new System.Drawing.Point(2, 17);
            this.cbSellTimeInForce.Name = "cbSellTimeInForce";
            this.cbSellTimeInForce.Size = new System.Drawing.Size(62, 20);
            this.cbSellTimeInForce.TabIndex = 4;
            // 
            // btnBuyQty01
            // 
            this.btnBuyQty01.Font = new System.Drawing.Font("新細明體", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.btnBuyQty01.Location = new System.Drawing.Point(180, 44);
            this.btnBuyQty01.Name = "btnBuyQty01";
            this.btnBuyQty01.Size = new System.Drawing.Size(31, 26);
            this.btnBuyQty01.TabIndex = 5;
            this.btnBuyQty01.Text = "1";
            this.btnBuyQty01.UseVisualStyleBackColor = true;
            this.btnBuyQty01.Click += new System.EventHandler(this.btnBuyQty01_Click);
            // 
            // gbBuy
            // 
            this.gbBuy.Controls.Add(this.cbBuyTimeInForce);
            this.gbBuy.Location = new System.Drawing.Point(3, 29);
            this.gbBuy.Name = "gbBuy";
            this.gbBuy.Padding = new System.Windows.Forms.Padding(0);
            this.gbBuy.Size = new System.Drawing.Size(69, 40);
            this.gbBuy.TabIndex = 5;
            this.gbBuy.TabStop = false;
            this.gbBuy.Text = "Buy";
            // 
            // cbBuyTimeInForce
            // 
            this.cbBuyTimeInForce.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbBuyTimeInForce.FormattingEnabled = true;
            this.cbBuyTimeInForce.Location = new System.Drawing.Point(3, 17);
            this.cbBuyTimeInForce.Name = "cbBuyTimeInForce";
            this.cbBuyTimeInForce.Size = new System.Drawing.Size(62, 20);
            this.cbBuyTimeInForce.TabIndex = 3;
            // 
            // nudOrderQty
            // 
            this.nudOrderQty.Location = new System.Drawing.Point(140, 46);
            this.nudOrderQty.Name = "nudOrderQty";
            this.nudOrderQty.Size = new System.Drawing.Size(40, 22);
            this.nudOrderQty.TabIndex = 4;
            this.nudOrderQty.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // cbCommodity
            // 
            this.cbCommodity.FormattingEnabled = true;
            this.cbCommodity.Location = new System.Drawing.Point(3, 6);
            this.cbCommodity.Name = "cbCommodity";
            this.cbCommodity.Size = new System.Drawing.Size(189, 20);
            this.cbCommodity.TabIndex = 2;
            this.cbCommodity.SelectedIndexChanged += new System.EventHandler(this.cbCommodity_SelectedIndexChanged);
            // 
            // Flash10
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(216)))), ((int)(((byte)(228)))), ((int)(((byte)(248)))));
            this.ClientSize = new System.Drawing.Size(570, 685);
            this.Controls.Add(this.pnSetting);
            this.Controls.Add(this.c1FlexGrid1);
            this.Name = "Flash10";
            this.Text = "Flash10";
            this.Resize += new System.EventHandler(this.Flash10_Resize);
            ((System.ComponentModel.ISupportInitialize)(this.c1FlexGrid1)).EndInit();
            this.pnSetting.ResumeLayout(false);
            this.pnSetting.PerformLayout();
            this.gbSell.ResumeLayout(false);
            this.gbBuy.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.nudOrderQty)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private C1.Win.C1FlexGrid.C1FlexGrid c1FlexGrid1;
        private System.Windows.Forms.Panel pnSetting;
        private System.Windows.Forms.ComboBox cbCommodity;
        private System.Windows.Forms.ComboBox cbBuyTimeInForce;
        private System.Windows.Forms.ComboBox cbSellTimeInForce;
        private System.Windows.Forms.GroupBox gbSell;
        private System.Windows.Forms.GroupBox gbBuy;
        private System.Windows.Forms.NumericUpDown nudOrderQty;
        private System.Windows.Forms.Button btnBuyQty10;
        private System.Windows.Forms.Button btnBuyQty05;
        private System.Windows.Forms.Button btnBuyQty02;
        private System.Windows.Forms.Button btnBuyQty01;
        private System.Windows.Forms.CheckBox cbCenter5;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnFontIncrease;
        private System.Windows.Forms.Button btnFontDecrease;
    }
}