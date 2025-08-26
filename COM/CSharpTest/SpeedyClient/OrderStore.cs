using System;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;

namespace Test1
{
    //-----------------------------------------------------------------------------------------
    class PxQtyPair
    {
        //-----------------------------------------------------------------------------------------
        public int    Qty;
        public double Px;
        //-----------------------------------------------------------------------------------------
        public PxQtyPair( double Price  )
        {
            Px = Price;
            Qty = 0;
        }
        //-----------------------------------------------------------------------------------------
    }
    //-----------------------------------------------------------------------------------------
    class PxQtyContainer
    {
        //-----------------------------------------------------------------------------------------
        System.Collections.Hashtable FPxHash = null;
        //-----------------------------------------------------------------------------------------
        public PxQtyContainer()
        {
            FPxHash   = new System.Collections.Hashtable();
        }
        //-----------------------------------------------------------------------------------------
        public void GetPxQty( System.Collections.ArrayList Pxs, System.Collections.ArrayList Qtys )
        {
            foreach ( DictionaryEntry de in FPxHash )
            {
                PxQtyPair Pair = (PxQtyPair)de.Value;

                Pxs.Add(Pair.Px);
                Qtys.Add(Pair.Qty);
            }
        }
        //-----------------------------------------------------------------------------------------
        public void ModifyQty(double Px, int Qty)
        {
            int IntKey = (int)(Px*10000);
            PxQtyPair Pair;

            if (FPxHash.ContainsKey(IntKey) == false )
            {
                Pair = new PxQtyPair( Px );
                FPxHash[IntKey] = Pair;
            }
            else
                Pair = (PxQtyPair)FPxHash[IntKey];
            Pair.Qty += Qty;
            if (Pair.Qty <= 0)
            {
                FPxHash.Remove( IntKey );
                Pair = null;
            }
        }
        //-----------------------------------------------------------------------------------------
    }
    //-----------------------------------------------------------------------------------------
    public class OrderStore
    {
        //-----------------------------------------------------------------------------------------
        System.Collections.Hashtable FBuyOrdersHash = null;
        System.Collections.Hashtable FSellOrdersHash = null;
        System.Collections.Hashtable FBuyFilledHash = null;
        System.Collections.Hashtable FSellFilledHash = null;
        //-----------------------------------------------------------------------------------------
        public delegate void Update(object Sender, string Exchange, string Symbol, bool IsBuySide);
        public Update OnWorkingChange = null;
        public Update OnFilledChange  = null;
        //-----------------------------------------------------------------------------------------
        public OrderStore()
        {
            FBuyOrdersHash  = new System.Collections.Hashtable();
            FSellOrdersHash = new System.Collections.Hashtable();
            FBuyFilledHash  = new System.Collections.Hashtable();
            FSellFilledHash = new System.Collections.Hashtable();
        }
        //-----------------------------------------------------------------------------------------
        void Modify( System.Collections.Hashtable Table, String Key, double Px, int Qty)
        {
            PxQtyContainer Container;

            if( Table.ContainsKey(Key) == false)
            {
                Container = new PxQtyContainer();
                Table[ Key ] = Container;
            }
            else
                Container = (PxQtyContainer)Table[Key];
            Container.ModifyQty( Px, Qty );
        }
        //-----------------------------------------------------------------------------------------
        void GetPxQty( System.Collections.Hashtable Table, String Key, System.Collections.ArrayList Pxs, System.Collections.ArrayList Qtys )
        {
            PxQtyContainer Container;

            Pxs.Clear();
            Qtys.Clear();
            if (Table.ContainsKey(Key) == true)
            {
                Container = (PxQtyContainer)Table[Key];
                Container.GetPxQty( Pxs, Qtys );
            }
        }
        //-----------------------------------------------------------------------------------------
        public void Clear()
        {
            FBuyOrdersHash.Clear();
            FSellOrdersHash.Clear();
            FBuyFilledHash.Clear();
            FSellFilledHash.Clear();
        }
        //-----------------------------------------------------------------------------------------
        public void Working( String Exchange, String Symbol, bool BuySide, double Px, int Qty)
        {
            string Key = string.Format("{0}.{1}", Exchange, Symbol );
            
            if (BuySide == true)
                Modify( FBuyOrdersHash, Key, Px, Qty );            
            else
                Modify( FSellOrdersHash, Key, Px, Qty);
            OnWorkingChange( this, Exchange, Symbol, BuySide); 
        }
        //-----------------------------------------------------------------------------------------
        public void Filled(String Exchange, String Symbol, bool BuySide, double OrderPx, double Px, int Qty)
        {
            string Key = string.Format("{0}.{1}", Exchange, Symbol);
            if (Qty > 0)
            {
                if (BuySide == true)
                {
                    Modify(FBuyOrdersHash, Key, OrderPx, -Qty);
                    Modify(FBuyFilledHash, Key, Px, Qty);
                }
                else
                {
                    Modify(FSellOrdersHash, Key, OrderPx, -Qty);
                    Modify(FSellFilledHash, Key, Px, Qty);
                }
            }
            OnFilledChange( this, Exchange, Symbol, BuySide); 
        }        
        //-----------------------------------------------------------------------------------------
        public void GetWorking(String Exchange, String Symbol, bool BuySide, System.Collections.ArrayList Pxs, System.Collections.ArrayList Qtys)
        {
            string Key = string.Format("{0}.{1}", Exchange, Symbol);

            if( BuySide == true)              
                GetPxQty( FBuyOrdersHash, Key, Pxs, Qtys);
            else
                GetPxQty(FSellOrdersHash, Key, Pxs, Qtys);  
        }
        //-----------------------------------------------------------------------------------------
        public void GetFilled(String Exchange, String Symbol, bool BuySide, System.Collections.ArrayList Pxs, System.Collections.ArrayList Qtys)
        {
            string Key = string.Format("{0}.{1}", Exchange, Symbol);

            if (BuySide == true)
                GetPxQty(FBuyFilledHash, Key, Pxs, Qtys);
            else
                GetPxQty(FSellFilledHash, Key, Pxs, Qtys);
        }
        //-----------------------------------------------------------------------------------------
    }
}
