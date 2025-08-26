namespace Test1
{
    partial class TSelectColForm
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
            this.FCheckedListBox = new System.Windows.Forms.CheckedListBox();
            this.SuspendLayout();
            // 
            // FCheckedListBox
            // 
            this.FCheckedListBox.BackColor = System.Drawing.SystemColors.Control;
            this.FCheckedListBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.FCheckedListBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.FCheckedListBox.FormattingEnabled = true;
            this.FCheckedListBox.Location = new System.Drawing.Point(0, 0);
            this.FCheckedListBox.Margin = new System.Windows.Forms.Padding(4);
            this.FCheckedListBox.Name = "FCheckedListBox";
            this.FCheckedListBox.Size = new System.Drawing.Size(251, 336);
            this.FCheckedListBox.TabIndex = 0;
            this.FCheckedListBox.MouseLeave += new System.EventHandler(this.TSelectColForm_MouseLeave);
            // 
            // TSelectColForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(251, 336);
            this.Controls.Add(this.FCheckedListBox);
            this.Font = new System.Drawing.Font("PMingLiU", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "TSelectColForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "TSelectColForm";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.TSelectColForm_FormClosing);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.CheckedListBox FCheckedListBox;
    }
}