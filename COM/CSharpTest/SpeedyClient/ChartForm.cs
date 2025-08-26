using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Test1
{
    public partial class ChartForm : Form
    {
        //-----------------------------------------------------------------------------------------------------------------
        System.Collections.Hashtable FIndexHash = new System.Collections.Hashtable();
        string FExchange;
        string FSymbol;
        //-----------------------------------------------------------------------------------------------------------------
        public ChartForm( String Ex,String Sym, int Digi )
        {
            InitializeComponent();
            FExchange = Ex;
            FSymbol   = Sym;
            String FormatStr = "0";

            if (Digi > 0)
            {
                FormatStr += ".";
                for( int i=0;i< Digi; i++ )
                     FormatStr += "0";
            }
            chart1.BeginInit();
            chart1.ChartAreas["PriceArea"].AxisY.LabelStyle.Format = FormatStr;
            chart1.Series["Price"]["PointWidth"] = "0.6";            
            chart1.Series["Price"]["PriceDownColor"] = "Green";
            chart1.Series["Price"]["PriceUpColor"] = "Red";
            chart1.EndInit();
        }
        //-----------------------------------------------------------------------------------------------------------------
        public string Exchange
        {
            get
            {
                return FExchange;
            }
        }
        //-----------------------------------------------------------------------------------------------------------------
        public string Symbol
        {
            get
            {
                return FSymbol;
            }
        }
        //-----------------------------------------------------------------------------------------------------------------
        public void BeginUpdate()
        {
            chart1.Series["Price"].Points.Clear();
            FIndexHash.Clear();
        }
        //-----------------------------------------------------------------------------------------------------------------
        public void Update(StarWaveAPI.MinuteKInfo Info)
        {
            int TimeInt = Info.Hour*100 + Info.Minutes;
            int index;

            if( FIndexHash.Contains( TimeInt ) )
            {
                index = (int)FIndexHash[ TimeInt ];
                chart1.Series["Price"].Points[index].YValues[1] = Info.MinPx;
                chart1.Series["Price"].Points[index].YValues[2] = Info.OpenPx;
                chart1.Series["Price"].Points[index].YValues[3] = Info.ClosePx;

                chart1.Invalidate();
                chart1.Update();
            }
            else
                Add( Info );
        }
        //-----------------------------------------------------------------------------------------------------------------
        public void Add( StarWaveAPI.MinuteKInfo Info )
        {
            string TimeStr = string.Format("{0}:{1}", Info.Hour, Info.Minutes);            
            int TimeInt = Info.Hour*100 + Info.Minutes;

            int index = chart1.Series["Price"].Points.AddXY( DateTime.Parse(TimeStr), Info.MaxPx);
            chart1.Series["Price"].Points[index].YValues[1] = Info.MinPx;
            chart1.Series["Price"].Points[index].YValues[2] = Info.OpenPx;
            chart1.Series["Price"].Points[index].YValues[3] = Info.ClosePx;
            FIndexHash[ TimeInt ] = index;
        }
        //-----------------------------------------------------------------------------------------------------------------
        public void EndUpdate()
        {
            double Max = chart1.Series["Price"].Points.FindMaxByValue("Y1", 0).YValues[0];
            double Min = chart1.Series["Price"].Points.FindMinByValue("Y1", 0).YValues[0];
            double Extra = Math.Abs(Max - Min) * 0.1;

            chart1.ChartAreas["PriceArea"].AxisY.Maximum = Max+ Extra;
            chart1.ChartAreas["PriceArea"].AxisY.Minimum = Min -Extra;
        }
        //-----------------------------------------------------------------------------------------------------------------
    }
}
