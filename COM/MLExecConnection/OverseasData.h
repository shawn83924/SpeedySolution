/* 
 * File:   OverseasData.h
 * Author: Zhen Fan
 *
 * Created on 2013/03/26 10:55
 */

#ifndef OVERSEASDATA_H
#define	OVERSEASDATA_H
#include "UFC.h"
//------------------------------------------------------------------------------
typedef struct os_confirm_struct 
{
    ///< Begin TEMPF-BODY
    Int8 FUNCTION_CODE[3];      // 1.X(3)        0 201:Confirm  202:Reject
    Int8 SPEEDY_TIME[8];        // 2.9(8)        3
    Int8 PATS_TIME[8];          // 3.9(8)       11 
    Int8 SPEEDY_SEQNO[8];       // 4.9(8)       19
    Int8 FCM_ID[10];            // 5.X(10)      27  SIM:test   PTH
    Int8 FCM_ACCOUNT[20];       // 6.X(20)      37
    Int8 ORDER_SEQNO[20];       // 7.X(20)      57  YYYYMMDD + SPEEDY_SEQNO
    Int8 BEF_ORDER_SEQNO[20];   // 8.X(20)      77
    Int8 ORDER_ID[20];          // 9.X(20)      97
    Int8 SEC_ORDER_ID[20];      //10.X(20)     117 
    Int8 EXEC_TYPE;             //11.X(1)      137
    Int8 ORDER_STATUS;          //12.X(1)      138
    Int8 EXCHANGE[20];          //13.X(20)     139
    Int8 COMMODITY[20];         //14.X(20)     159
    Int8 SECU_TYPE[5];          //15.X(5)      179
    Int8 SETTLEMENT_MONTH[6];   //16.9(6)      184
    Int8 STRIKE_PRICE[13];      //17.9(7)V9(6) 190
    Int8 CALL_PUT_FLAG;         //18.X(1)      203  0:PUT  1:CALL
    Int8 BUY_SELL_KIND;         //19.X(1)      204  1:Buy  2:SELL
    Int8 ORDER_QTY[10];         //20.9(10)     205
    Int8 ORDER_PRICE[13];       //21.9(7)V9(6) 215
    Int8 STOP_PRICE[13];        //22.9(7)V9(6) 228
    Int8 ORDER_TYPE;            //23.X(1)      241
    Int8 ORDER_COND;            //24.X(1)      242
    Int8 EXPIRE_DATE[8];        //25.9(8)      243
    Int8 OPEN_OFFSET_KIND;      //26.X(1)      251
    Int8 REJECT_TO;             //27.X(1)      252
    Int8 REJECT_REASON;         //28.X(1)      253
    Int8 TEXT[100];             //29.X(100)    254
    ///< End TEMPF-BODY
    Int8 ACNO[20];              //30.X(20)     354
    Int8 TOT_MATCH_QTY[10];     //31.9(10)     374
    Int8 PARTYID[7];            //32.X(7)      384 
    Int8 EUREXACCTID[7];        //33.X(7)      391     
    Int8 COMMODITY_ID2[20];     //34.X(20)     398
    Int8 SETTLEMENT_MONTH2[10]; //35.9(10)     418    
    Int8 SECU_TYPE2[5];         //36.X(5)      428    
    Int8 STRIKE_PRICE2[13];     //37.9(7)V9(6) 433    
    Int8 CALL_PUT_FLAG2;        //38.X(1)      446
    Int8 BUY_SELL_KIND2;        //39.X(1)      447        
    Int8 PRICE_FLAG;            //40.X(1)      448  + ' ' /-
    Int8 SUB_ACNO[7];           //41.X(7)      449 
    Int8 DAY_TRADE_ID;          //42.X(1)      456    
    Int8 ORDER_NO[5];           //43.X(5)      457
    Int8 FILLER[19];            //44.x(19)     462 
    Int8 END;                   //45.X(1)      481
} OS_CONFIRM_STRUCT;            //< Total      481               

//------------------------------------------------------------------------------
typedef struct os_match_struct 
{
    ///< Begin TEMPF-BODY
    Int8 FUNCTION_CODE[3];      // 1.X(3)        0  203: Filled
    Int8 MATCH_TIME[8];         // 2.9(8)        3
    Int8 SPEEDY_SEQNO[8];       // 3.9(8)       11
    Int8 FCM_ID[10];            // 4.X(10)      19  SIM:test   PTH
    Int8 FCM_ACCOUNT[20];       // 5.X(20)      29
    Int8 MATCH_SEQNO[20];       // 6.X(20)      49  YYYYMMDD + SPEEDY_SEQNO
    Int8 BEF_MATCH_SEQNO[20];   // 7.X(20)      69
    Int8 ORDER_ID[20];          // 8.X(20)      89
    Int8 PATS_SEQNO[70];        // 9.X(70)     109    
    Int8 TRANS_TYPE;            //10.X(1)      179
    Int8 REFID[20];             //11.X(20)     180
    Int8 EXEC_TYPE;             //12.X(1)      200
    Int8 ORDER_STATUS;          //13.X(1)      201
    Int8 REST_REASON[8];        //14.9(8)      202    
    Int8 EXCHANGE[20];          //15.X(20)     210
    Int8 COMMODITY[20];         //16.X(20)     230
    Int8 PRODUCT_TYPE[5];       //17.X(5)      250
    Int8 SETTLEMENT_MONTH[6];   //18.9(6)      255
    Int8 STRIKE_PRICE[13];      //19.9(7)V9(6) 261
    Int8 CALL_PUT_FLAG;         //20.X(1)      274  0:PUT  1:CALL
    Int8 BUY_SELL_KIND;         //21.X(1)      275  1:Buy  2:SELL    
    Int8 ORDER_QTY[10];         //22.9(10)     276
    Int8 ORDER_PRICE[13];       //23.9(7)V9(6) 286
    Int8 STOP_PRICE[13];        //24.9(7)V9(6) 299
    Int8 ORDER_TYPE;            //25.X(1)      312
    Int8 ORDER_COND;            //26.X(1)      313
    Int8 EXPIRE_DATE[8];        //27.9(8)      314    
    Int8 OPEN_OFFSET_KIND;      //28.X(1)      322
    Int8 MATCH_QTY[10];         //29.9(10)     323
    Int8 MATCH_PRICE[13];       //30.9(7)V9(6) 333
    Int8 UNMATCH_QTY[10];       //31.9(10)     346
    Int8 TOT_MATCH_QTY[10];     //32.9(10)     356
    Int8 AVG_MATCH_PRICE[13];   //33.9(7)V9(6) 366
    ///< End TEMPF-BODY
    Int8 ACNO[20];              //34.X(20)     379
    Int8 PARTYID[7];            //35.X(7)      399
    Int8 EUREXACCTID[7];        //36.X(7)      406         
    Int8 COMMODITY_ID2[20];     //37.X(20)     413
    Int8 SETTLEMENT_MONTH2[10]; //38.9(10)     433 
    Int8 SECU_TYPE2[5];         //39.X(5)      443        
    Int8 STRIKE_PRICE2[13];     //40.9(7)V9(6) 448    
    Int8 CALL_PUT_FLAG2;        //41.X(1)      461
    Int8 BUY_SELL_KIND2;        //42.X(1)      462    
    Int8 PRICE_FLAG;            //43.X(1)      463  + ' ' /-
    Int8 SUB_ACNO[7];           //44.X(7)      464 
    Int8 DAY_TRADE_ID;          //45.X(1)      471        
    Int8 ORDER_NO[5];           //46.X(5)      472
    Int8 FILLER[14];            //47.x(14)     477 
    Int8 END;                   //48.X(1)      491    
} OS_MATCH_STRUCT;
#endif	/* OVERSEASDATA_H */

