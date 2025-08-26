using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Test1
{
    static class Program
    {
        //---------------------------------------------------------------------------
        public static bool   FIsAccept = false;
        public static string FNewOrder = "false"; 
        public static string FNewVer = "true";
        public static int    FConnectionID = -1;
        public static string FLogonID = "";
        public static string FLogonAccount = "";
        public static string FReadOnly = "false";
        public static string UserData = "000011";
        public static string FMMAccount = "8888888";     ///< Market maker account.
        public static string FPATSAccount = "APITEST18B";///< PATs account 
        public static string FBrokerID = "F006000";      ///< Futures broker ID.   
        public static string FTSEBrokerID = "7000";      ///< Equity broker ID.     
        public static string FCNBrokerID   = "1111";      ///< China broker ID.     
        public static string FCMID = "F006";             ///< Futures clear member ID.   
        ///< For TAIFEX Futures/Options
        public static string FUTSymbol = "XIFI8";
        public static string FUTPx = "255";
        public static string OPTSymbol = "TXO06500I9";
        public static string OPTPx = "6400.0";
        public static string CNFUTSymbol = "IF1312";
        public static string CNFUTPx = "255";
        public static string CNOPTSymbol = "IO";
        public static string CNOPTPx = "6400.0";
        ///< For TWSE/OTC
        public static string TSESymbol = "2357";
        public static string TSEPx = "89.50";
        public static string OTCSymbol = "6205";
        public static string OTCPx = "61.00";        
        /// < For PATS        
        public static string PATSSymbol = "TW";
        public static string PATSPx = "357";
        public static string PATSEx = "SIM";
        public static string PATSMMY = "DEC11";
        //---------------------------------------------------------------------------
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);            
            Application.Run( new TOrderForm( ));            
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