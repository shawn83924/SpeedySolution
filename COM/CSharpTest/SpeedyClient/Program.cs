using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Test1
{
    static class Program
    {
        public static bool   FIsAccept = false;
        public static string FNewOrder = "true"; 
        public static string FNewVer = "true";
        public static int    FConnectionID = -1;
        public static string FLogonID = "";
        public static string FLogonAccount = "";
        public static string FMMAccount = "8888888";
        public static string FBrokerID = "F006000";
        public static string FTSEBrokerID = "7000";
        public static string FCMID = "F006";
        public static string FUTSymbol = "XIFI8";
        public static string FUTPx = "255";
        public static string OPTSymbol = "TXO06500I9";
        public static string OPTPx = "6400.0";
        public static string TSESymbol = "2357";
        public static string TSEPx = "89.50";
        public static string OTCSymbol = "6205";
        public static string OTCPx = "61.00";
        public static string FReadOnly = "false";
        public static string UserData = "000011";

        public static string FSpeedyIP = "192.168.0.22";
        public static int    FSpeedyPort = 23456;
        public static string FStarWaveIP = "10.6.84.200";
        public static int    FStarWavePort = 34567;
        public static string FChartServerIP = "10.6.84.200";
        public static int FChartServerPort = 34567;

        public static TOrderForm FAppMainForm = null;
        //---------------------------------------------------------------------------
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            FAppMainForm = new TOrderForm();
            Application.Run( FAppMainForm );            
        }
        //---------------------------------------------------------------------------
        public static int ConvertStringToInt32(string StrVal)
        {
            try
            {
                return Convert.ToInt32(StrVal);
            }
            catch
            {
            }
            return 0;
        }
        //---------------------------------------------------------------------------
        public static double ConvertStringToDouble(string StrVal)
        {
            try
            {
                return Convert.ToDouble(StrVal);
            }
            catch
            {
            }
            return 0.0;
        }
        //---------------------------------------------------------------------------
        public static void TextBoxNumberOnly(KeyEventArgs e, bool Dot)
        {
            switch (e.KeyCode)
            {
                case Keys.D0: break;
                case Keys.D1: break;
                case Keys.D2: break;
                case Keys.D3: break;
                case Keys.D4: break;
                case Keys.D5: break;
                case Keys.D6: break;
                case Keys.D7: break;
                case Keys.D8: break;
                case Keys.D9: break;

                case Keys.NumPad0: break;
                case Keys.NumPad1: break;
                case Keys.NumPad2: break;
                case Keys.NumPad3: break;
                case Keys.NumPad4: break;
                case Keys.NumPad5: break;
                case Keys.NumPad6: break;
                case Keys.NumPad7: break;
                case Keys.NumPad8: break;
                case Keys.NumPad9: break;

                case Keys.OemPeriod:
                    if (Dot)
                    {
                        break;
                    }
                    else
                    {
                        e.SuppressKeyPress = true;
                        return;
                    }

                case Keys.Delete: break;
                case Keys.Back: break;
                case Keys.Left: break;
                case Keys.Right: break;
                case Keys.Home: break;
                case Keys.End: break;

                default: e.SuppressKeyPress = true; return;
            }

            e.SuppressKeyPress = false;
        }
        //---------------------------------------------------------------------------
    }
}