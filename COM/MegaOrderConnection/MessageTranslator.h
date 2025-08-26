/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MessageTranslator.h
 * Author: leo
 *
 * Created on December 26, 2019, 5:33 PM
 */

#ifndef MESSAGETRANSLATOR_H
#define MESSAGETRANSLATOR_H

#include "UFC.h"
#include <vector>
#include <utility>


#define USER_DEFINE_FIELD 173    // 28 + 1 deciaml
#define T020_FIELD 23           // 22 + 1 decimal
#define R3_FIELD 21             // 20 + 1 decimal
#define TSE_ORD_FIELD 25        // 24 + 1 deciaml
#define T010_FIELD 59
#define FINAL_MSG_SIZE 256

struct t010_msg{
    char subsystemName[2];  // 0,1
    char functionCode[2];   // 2,3
    char messageType[2];    // 4.5
    char messageTime[6];    // 6~11
    char statusCode[2];     // 12,13
    char brokerID[4];       // 14~17
    char pvcID[2];          // 18,19
    char orderNo[5];        // 20~24
    char ivacno[7];         // 25~31
    char ivacnoFlag;        // 32
    char stockNo[6];        // 33~38
    char price[5];          // 39~43
    char priceDecimal[4];   // 44~47
    char quantity[6];       // 48~53
    char buySellCode;       // 54
    char exchangeCode;      // 55
    char orderType;         // 56
    char priceType;         // 57
    char timeInForce;       // 58
};

struct t020_msg{
    char subsystemName[2];
    char functionCode[2];
    char messageType[2];
    char messageTime[6];
    char statusCode[2];
    char brockerID[4];
    char pvcID[2];
    char orderNo[5];
    char ivacNo[7];
    char ivacNoFlag;
    char stockNo[6];
    char price_int[5];
    char price_dec[4];
    char quantity[6];
    char buySellCode;
    char exchangeCode;
    char oderType;
    char priceType;
    char timeInForce;
    char orderDate[8];
    char orderTime[9];
    char beforeQuantity[6];
    char afterQuantity[6];
};

struct r3_msg{
    char subsystemName[2];
    char functionCode[2];
    char messageType[2];
    char messageTime[2];
    char statusCode[2];
    char bodyLength[4];
    char bodyCnt[2];
    char stkNo[6];
    char mthQty[8];
    char mthPr_int[5];
    char mthPr_dec[4];
    char mthTime[9];
    char excd;
    char buySell;
    char orderNo[5];
    char ivacNo[7];
    char odrtpe;
    char seqNo[6];
    char brockerID[4];
    char recNo[8];
    char markS;
};

struct user_define{
    char line[2];
    char htsq[4];
    char insq[6];
    char mgsq[6];
    char kTime[9];
    char sCode;
    char oCode;
    char tlno[6];
    char sale[3];
    char idno[11];
    char grep[4];
    char njer[4];
    char fpQty[8];
    char cdArea;
    char cdQty[8];
    char cdPer[2];
    char sType[2];
    char oType_O;
    char bArea[4];
    char bTerm;
    char etm;
    char unit[6];
    char prMark;
    char cnOver;
    char pramt[7];
    char cnFlag;
    char prFee[5];
    char dbPer[4];
    char dbPrice_int[5];
    char dbPrice_dec[4];
    char osCode;
    char ooCode;
    char cmsovq[7];
    char markDt;
    char rkle;
    char sbrjForce;
    char otQty[8];
    char pawd[4];
    char filter[30];
};

struct tse_ord{
    char funcCode[2];
    char branchID[8];
    char custID[10];
    char custPW[20];
    char serverNo[2];
    char odSeqNum[6];
    char ordDate[8];
    char ordTime[9];
    char market;
    char apCode;
    char ordNo[8];
    char forign;
    char stockNo[6];
    char odPrice[5];
    char odPriceDecimal[4];
    char priceFlag;
    char odQty[10];
    char buySell;
    char trade;
    char act;
    char source[2];
    char ipCallID[24];
    char forceCode;
    char sFlag;
    char ordType;
};

struct tse_ack{
    char funcCode[2];
    char branchID[8];
    char custID[10];
    char custPW[20];
    char serverNo[2];
    char odSeqNum[6];
    char ordDate[8];
    char ordTime[9];
    char market;
    char apCode;
    char ordNo[8];
    char forign;
    char stockNo[6];
    char odPrice[5];
    char odPriceDecimal[4];
    char priceFlag;
    char odQty[10];
    char buySell;
    char trade;
    char act;
    char source[2];
    char orgQty[10];
    char beforeQty[10];
    char afterQty[10];
    char tseTime[9];
    char returnTime[9];
    char errCode[8];
    char controlCode;
    char endFlag;
    char errMsg[60];
    char sFlag;
    char ordType;
    char lf;
};

struct tse_mat{
    char funcCode[2];
    char branchID[8];
    char custID[10];
    char matchTime[9];
    char serverNo[2];
    char odSeqNum[6];
    char mktSeqNum[8];
    char market;
    char apCode;
    char ordNo[8];
    char forign;
    char stockNo[6];
    char price[5];
    char priceDecimal[4];
    char matchQty[10];
    char buySell;
    char trade;
    char source[2];
    char payPrice[10];
    char payPriceDecimal[2];
    char returnTime[9];
    char lf;
};

struct te_ord{
    char funcCode[2];
    char branchID[8];
    char custID[10];
    char custPW[20];
    char serverNo[2];
    char odSeqNum[6];
    char ordDate[8];
    char ordTime[9];
    char market;
    char apCode;
    char ordNo[8];
    char forign;
    char stockNo[6];
    char odPrice[5];
    char odPriceDecimal[4];
    char priceFlag;
    char odQty[10];
    char buySell;
    char trade;
    char act;
    char source[2];
    char ipCallID[24];
    char forceCode;
    char fineBhNo[4];
    char tranType;
    char ordType;
};

struct te_ack{
    char funcCode[2];
    char branchID[8];
    char custID[10];
    char custPW[20];
    char serverNo[2];
    char odSeqNum[6];
    char ordDate[8];
    char ordTime[9];
    char market;
    char apCode;
    char ordNo[8];
    char forign;
    char stockNo[6];
    char odPrice[5];
    char odPriceDecimal[4];
    char priceFlag;
    char odQty[10];
    char buySell;
    char trade;
    char act;
    char source[2];
    char orgQty[10];
    char beforeQty[10];
    char afterQty[10];
    char tseTime[9];
    char returnTime[9];
    char errCode[8];
    char controlCode;
    char fineBhNo[4];
    char tranType;
    char endFlag;
    char errMsg[60];
    char lf;
};

struct te_mat{
    char funcCode[2];
    char branchID[8];
    char custID[10];
    char matchTime[9];
    char serverNo[2];
    char odSeqNum[6];
    char mktSeqNum[8];
    char market;
    char apCode;
    char ordNo[8];
    char forign;
    char stockNo[6];
    char price[5];
    char priceDecimal[4];
    char matchQty[10];
    char buySell;
    char trade;
    char source[2];
    char payPrice[10];
    char payPriceDecimal[2];
    char returnTime[9];
    char lf;
};

class MessageTranslator {
public:
    
    char *checkMsgType(char *msg);
    
    MessageTranslator();
    
    char* tseOrdToT010(char *tseOrdMsg);
    char* t020ToTseAck(char *tseAckMsg);
    char* r3ToTseMat(char *tseMatMsg);
    char* teOrdToT010(char *teOrdMsg);
    char* t020ToTeAck(char *teAckMsg);
    char* r3ToTeMat(char *teMatMsg);
    void printMsg(char *msg,size_t msgSize);
    
    virtual ~MessageTranslator();
private:

    int msgPosition;
    char t010Msg[60];
    t010_msg t010MsgStruct;
    t020_msg t020MsgStruct;
    r3_msg r3MsgStruct;
    tse_ord *tseOrdStruct;
    tse_ack tseAckStruct;
    tse_mat tseMatStruct;
    te_ord *teOrdStruct;
    te_ack teAckStruct;
    te_mat teMatStruct;
    user_define usrDefStruct;
    char *finalMessage;
    void writeT010(char *msg,int len,char *position);
    void writeUsrDef(char *msg,int len,char *position);
    //void writeAppend(char *msg,int len);
    
};

#endif /* MESSAGETRANSLATOR_H */

