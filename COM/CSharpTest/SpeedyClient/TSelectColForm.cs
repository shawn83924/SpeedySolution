using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Test1
{
    public partial class TSelectColForm : Form
    {
        C1.Win.C1FlexGrid.C1FlexGrid FGrid = null;
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        public TSelectColForm(C1.Win.C1FlexGrid.C1FlexGrid RefGrid)
        {
            InitializeComponent();

            FGrid = RefGrid;
            AddCheckBoxes();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        void AddCheckBoxes()
        {
            int ColCount = FGrid.Cols.Count;
            for (int i = 0; i < ColCount; i++)
            {
                C1.Win.C1FlexGrid.Column Col = FGrid.Cols[i];
                FCheckedListBox.Items.Add(Col.Caption, Col.Visible);
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void TSelectColForm_MouseLeave(object sender, EventArgs e)
        {
            Close();
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
        private void TSelectColForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            int ColCount = FGrid.Cols.Count;
            for (int i = 0; i < ColCount; i++)
            {
                FGrid.Cols[i].Visible = FCheckedListBox.GetItemChecked(i);
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------
    }
}