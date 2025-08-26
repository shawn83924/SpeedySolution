/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MLMessage.h
 * Author: yuan
 *
 * Created on February 1, 2021, 12:43 AM
 */

#ifndef MLMESSAGE_H
#define MLMESSAGE_H
//------------------------------------------------------------------------------
#include "../UFC/UFCType.h"
//------------------------------------------------------------------------------
using namespace UFCType;
//------------------------------------------------------------------------------
typedef struct okdata
{
    Int8 DATE[8]; //9(8) 0
    Int8 MSG_SEQNUM[10]; //9(10) 8
    Int8 MSG_DATE[8]; //9(8) 18
    Int8 MSG_TIME[6]; //9(6) 26
    Int8 MSG_MIL[4]; //9(4) 32
    Int8 MSG_TYPE[3]; //X(3) 36
    Int8 LNK_BROKER_NO[5]; //X(5) 39
    Int8 SESSION_ID[5]; //9(5) 44
    
    //Int8 FLEX_MSG_TYPEEXT[3];
    //Int8 FLEX_PRESERVE[3];
    //Int8 FLEX_MSG_TIMES[10];
    Int8 STATUS_CODE[3]; //X(3) 49 FLEX x(5)
    
    Int8 EXEC_TYPE[1]; //X(1) 52
    Int8 CLEAR_MEMBER_ID_NO[5]; //X(5) 53
    Int8 BROKER_ID_NO[5]; //X(5) 58
    Int8 ORDER_NO[5]; //X(5) 63
    Int8 ORD_ID[7]; //9(7) 68
    Int8 USER_DEFINE[8]; //X(8) 75
    Int8 SYBMOL_TYPE[1]; //X(1) 83
    Int8 TR_ID[20]; //X(20) 84
    Int8 ORDER_PRICE[9]; //S9(8) 104
    Int8 ORDER_QTY[4]; //9(4) 113
    Int8 INVESTOR_ACNO[7]; //X(7) 117
    Int8 INVESTOR_FLAG[1]; //X(1) 124
    Int8 BUY_SELL_KIND[1]; //X(1) 125
    Int8 ORDER_TYPE[1]; //X(1) 126
    Int8 ORDER_COND[1]; //X(1) 127
    Int8 OPEN_OFFSET_KIND[1]; //X(1) 128
    Int8 LAST_MATCH_PRICE[10]; //S9(9) 129
    Int8 LAST_MATCH_QTY[4]; //9(4) 139
    Int8 TOT_MATCH_PRICE[17]; //S9(16) 143
    Int8 TOT_MATCH_QTY[4]; //9(4) 160
    Int8 LEAVES_QTY[4]; //9(4) 164
    Int8 BEFORE_QTY[4]; //9(4) 168
    Int8 BUY_SELL_KIND1[1]; //X(1) 172
    Int8 BUY_SELL_KIND2[1]; //X(1) 173
    Int8 MATCH_PRICE1[9]; //9(9) 174
    Int8 MATCH_PRICE2[9]; //9(9) 183
    Int8 MATCH_QTY1[4]; //9(4) 192
    Int8 MATCH_QTY2[4]; //9(4) 196
    Int8 ORDER_DATE[8]; //9(8) 200
    Int8 ORDER_TIME[6]; //9(6) 208
    Int8 ORDER_MIL[4]; //9(4) 214
    Int8 LAST_DATE[8]; //9(8) 218
    Int8 LAST_TIME[6]; //9(6) 226
    Int8 LAST_MIL[4]; //9(4) 232
    Int8 TARGET_ID[1]; //X(1) 236
    Int8 UNIQUE_ID[10]; //9(10) 237
    Int8 SEQNO[10]; //9(10) 247
    Int8 PROTOCOL_TYPE[1]; //X(1) 257
    
    //Int8 FLEX_PART_ID[3];//               PIC X(3)
    //Int8 FLEX_ORDER_MIL_R[9];//           PIC 9(9)
    //Int8 FLEX_LAST_MIL_R[9];//            PIC 9(9)    
    
    Int8 BROKER_ID[7]; //X(7) 258
    Int8 COMMODITY_ID1_1[7]; //X(7) 265
    Int8 SETTLEMENT_MONTH1_1[6]; //9(6) 272
    Int8 STRIKE_PRICE1_1[9]; //9(6)9(3) 278
    Int8 PRICE1_1[12]; //9(6)V9(6) 287
    Int8 COMMODITY_ID2_2[7]; //X(7) 299
    Int8 SETTLEMENT_MONTH2_2[6]; //9(6) 306
    Int8 STRIKE_PRICE2_2[9]; //9(6)9(3) 312
    Int8 PRICE2_2[12]; //9(6)V9(6) 321
    Int8 PRICE_D[13]; //S9(6)V9(6) 333
    Int8 AE_ID[4]; //9(4) 346
    Int8 DAY_TRADE_ID[1]; //X(1) 350
    Int8 IB_NO[3]; //X(03) 353
    Int8 DOS_IN_KIND[1]; //X(01) 354
    Int8 NET_NO[8]; //X(8) 355
    Int8 SUB_INVESTOR_ACNO[7]; //X(7) 363
    Int8 ODR_PRICE[9]; //S9(05)V9(3) 370
    Int8 CLEAR_MEMBER_ID[4]; //X(04) 379
    Int8 ORDER_KIND[1]; //< X(1) 383  ' ': Normal '1': off-hour
    Int8 FILLER[16]; //X(17) 384
    Int8 END; ///< 400
}OKD;
//------------------------------------------------------------------------------
typedef struct ocdata
{
    Int8 RO_T020[ 55 ];   ///< X(55) offset(0) (T020 length 69 - Header 14 )
    Int8 FUNCTION_CODE[2];///< 9(2)  offset(55)
    Int8 T030_STATUS[2];  ///< 9(2)  offset(57)
    Int8 TIME_OUT[1];     ///< X(1)  offset(59)
    Int8 SELLER[4];       ///< 9(4)  offset(60) 
    Int8 NETNO[7];        ///< X(7)  offset(64) 
    Int8 NET_SOURCE[1];   ///< X(1)  offset(71)
    Int8 NET_KIND[1];     ///< X(1)  offset(72)
    Int8 INNER_ACNO[7];   ///< 9(7)  offset(73)
    Int8 FILLER[20];      ///< X(20) offset(100)
    Int8 UNI_DATA[20];    ///< X(20) offset(80)    
                          ///< Total Length 120    
    Int8 EOS;
    
}RO_TSE;
//------------------------------------------------------------------------------
typedef struct ofdata
{
    Int8 RE_R030[62];     ///< X(62)  offset(0)   
    Int8 RECEIVE_TIME[8]; ///< 9(8)   offset(62)
    Int8 INNER_ACNO[7];   ///< 9(7)   offset(70)
    Int8 FILLER[23];      ///< X(23)  offset(97)   
    Int8 UNI_DATA[20];    ///< X(20)  offset(77)              
                          ///< Total Length 120       
    Int8 EOS;
    
}RE_TSE;
//------------------------------------------------------------------------------
typedef struct okfdata
{
    Int8 HEAD[2];           //< 0 
    Int8 BROKER_ID[7];      //< 2 
    Int8 ORDER_NO[5];       //< 9
    Int8 INVESTOR_ACNO[7];  //< 14
    Int8 INVESTOR_FLAG[1];  //< 21
    Int8 TR_ID[20];         //< 22                  
    Int8 BUY_SELL_KIND[1];  //< 42
    Int8 ORDER_TYPE[1];     //< 43
    Int8 ORDER_PRICE_SIGN[1];//< 44
    Int8 ORDER_PRICE[13];    //< 45
    Int8 ORDER_PRICE_M[7];   //< 58 
    Int8 ORDER_PRICE_D[4];   //< 65
    Int8 TOUCH_PRICE_SIGN[1];//< 69 
    Int8 TOUCH_PRICE[13];    //< 70  
    Int8 TOUCH_PRICE_M[7];   //< 83 
    Int8 TOUCH_PRICE_D[4];   //< 90  
    Int8 ORDER_QTY[10];      //< 94 
    Int8 ORDER_COND[1];      //< 104 
    Int8 OPEN_OFFSET_KIND[1];//< 105 
    Int8 REC_TIME[8];        //< 106 
    Int8 SEQNO[8];           //< 114  
    Int8 FUNCTION_KIND[2];   //< 122
    Int8 AE_ID[4];           //< 124  
    Int8 IB_NO[3];           //< 128 
    Int8 DOS_IN_KIND[1];     //< 131
    Int8 ORDER_SEQNO[20];    //< 132 
    Int8 BEF_ORDER_SEQNO[20];//< 152 
    Int8 ORDER_ID[20];       //< 172   
    Int8 EXECTION_TYPE[1];   //< 192
    Int8 REJECT_REASON[1];   //< 193
    Int8 DAY_TRADE[1];       //< 194 
    Int8 UPP_S_ID[4];        //< 195
    Int8 REJECT_TEXT[50];    //< 199
    Int8 NET_NO[8];          //< 249 
    Int8 SUB_INVESTOR_ACNO[7];//< 257
    Int8 COMMODITY_ID1[7];    //< 264
    Int8 SETTLEMENT_MONTH1[6];//< 271 
    Int8 STRIKE_PRICE1[9];    //< 277
    Int8 BUY_SELL_KIND1[1];   //< 286
    Int8 COMMODITY_ID2[7];    //< 287 
    Int8 SETTLEMENT_MONTH2[6];//< 294 
    Int8 STRIKE_PRICE2[9];    //< 300 
    Int8 BUY_SELL_KIND2[1];   //< 309 
    Int8 EXCH_ID[7];          //< 310
    Int8 Filler[195];         //< 317 
                              //< 512 
}OKF;
//------------------------------------------------------------------------------
typedef struct offdata
{
    Int8 HEAD[2];                //<0
    Int8 BROKER_ID[7];           //<2
    Int8 ORDER_NO[5];            //<9
    Int8 INVESTOR_ACNO[7];       //<14
    Int8 INVESTOR_FLAG[1];       //<21
    Int8 TR_ID[20];              //<22                          
    Int8 BUY_SELL_KIND[1];       //<42
    Int8 ORDER_TYPE[1];          //<43  
    Int8 OPEN_OFFSET_KIND[1];    //<44
    Int8 MATCH_PRICE[13];        //<45
    Int8 MATCH_PRICE_M[7];       //<58
    Int8 MATCH_PRICE_D[4];       //<65
    Int8 MATCH_QTY[10];          //<69
    Int8 UNMATCH_QTY[10];        //<79
    Int8 TOT_MATCH_QTY[10];      //<89
    Int8 MATCH_TIME[8];          //<99
    Int8 SEQNO[8];               //<107
    Int8 AE_ID[4];               //<115
    Int8 IB_NO[3];               //<119
    Int8 DOS_IN_KIND[1];         //<122
    Int8 ORDER_SEQNO[20];        //<123
    Int8 ORDER_ID[20];           //<143
    Int8 ORDER_STATUS[1];        //<163
    Int8 SIGN[1];                //<164
    Int8 AVG_PRICE[18];          //<165
    Int8 UPP_S_ID[4];            //<183
    Int8 NET_NO[8];              //<187
    Int8 SUB_INVESTOR_ACNO[7];   //<195
    Int8 COMMODITY_ID1[7];       //<202
    Int8 SETTLEMENT_MONTH1[6];   //<209
    Int8 STRIKE_PRICE1[9];       //<215
    Int8 BUY_SELL_KIND1[1];      //<224
    Int8 COMMODITY_ID2[7];       //<225
    Int8 SETTLEMENT_MONTH2[6];   //<232
    Int8 STRIKE_PRICE2[9];       //<238
    Int8 BUY_SELL_KIND2[1];      //<247
    Int8 MATCH_PRICE_SIGN[1];    //<248
    Int8 EXCH_ID[7];             //<249
    Int8 FILLER[256];            //<256    
                                 //<512    
}FOF;
//------------------------------------------------------------------------------
#endif /* MLMESSAGE_H */

