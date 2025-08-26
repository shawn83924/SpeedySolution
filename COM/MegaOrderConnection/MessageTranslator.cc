/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MessageTranslator.cpp
 * Author: leo
 * 
 * Created on December 26, 2019, 5:33 PM
 */

#include "MessageTranslator.h"
#include <cstring>
#include <iostream>

MessageTranslator::MessageTranslator() {
    msgPosition=0;
    finalMessage = new char[FINAL_MSG_SIZE];
}

MessageTranslator::~MessageTranslator() {
    delete[] finalMessage;
}

void MessageTranslator::printMsg(char *msg,size_t msgSize){
    for(int i = 0;i<msgSize;i++){
        printf("%c",msg[i]);
    }
}

char* MessageTranslator::tseOrdToT010(char *tseOrdMsg){
    tseOrdStruct = (tse_ord*)tseOrdMsg;     // dereference to tse_ord.
    
    UFC::BufferedLog::Printf(" tse ord source : [%s]",tseOrdMsg);
    //for(int i=0;i<10;i++){
    //    UFC::BufferedLog::Printf(" %c",tseOrdStruct->custID[i]);
    //}
    memset(&usrDefStruct,' ',sizeof(usrDefStruct));
    memset(&t010MsgStruct,' ',sizeof(t010MsgStruct));
    
    for(int i=0;i<TSE_ORD_FIELD;i++){  // 24 field + decimal
        switch(i){
            case 0: // subsystem name
                memcpy((char *)&t010MsgStruct.subsystemName,"30",2);
                break;
            case 1: // function code
                memcpy((char *)&t010MsgStruct.functionCode,"0",1);
                memcpy((char *)&t010MsgStruct.functionCode[1],&tseOrdStruct->funcCode[1],1);
                break;
            case 2: // message type
                memcpy((char *)&t010MsgStruct.messageType,"00",2);
                break;
            case 3: // message time
                memcpy((char *)&t010MsgStruct.messageTime,tseOrdStruct->ordTime,6);
                break;
            case 4: // status code
                memcpy((char *)&t010MsgStruct.statusCode,"00",2);
                break;
            case 5: // broker id
                memcpy((char *)&t010MsgStruct.brokerID,tseOrdStruct->branchID,4);
                break;
            case 6:     // pvc id
                break;
            case 7:     // order no
                memcpy((char *)&t010MsgStruct.orderNo,tseOrdStruct->ordNo,5);
                break;
            case 8: // ivacno  
                memcpy((char *)&t010MsgStruct.ivacno,tseOrdStruct->custID,7);
                break;
            case 9: // ivacno flag
                break;
            case 10:    // stock no
                memcpy((char *)&t010MsgStruct.stockNo,tseOrdStruct->stockNo,6);
                break;
            case 11:    // price int
                memcpy((char *)&t010MsgStruct.price,tseOrdStruct->odPrice,5);
                break;
            case 12:    // price dec
                memcpy((char *)&t010MsgStruct.priceDecimal,tseOrdStruct->odPriceDecimal,4);
                break;
            case 13:    // quantity
                memcpy((char *)&t010MsgStruct.quantity,tseOrdStruct->odQty,6);
                break;
            case 14:    // buy sell code
                memcpy((char *)&t010MsgStruct.buySellCode,&tseOrdStruct->buySell,1);
                break;
            case 15:    // exchange code
                memcpy((char *)&t010MsgStruct.exchangeCode,"0",1);
                break;
            case 16:    // order type
                memcpy((char *)&t010MsgStruct.orderType,&tseOrdStruct->trade,1);
                // FIXME: trancade zero at left side.
                break;
            case 17:    // price type
                memcpy((char *)&t010MsgStruct.priceType,&tseOrdStruct->priceFlag,1);
                break;
            case 18:    // time in force
                char type;
                if(tseOrdStruct->ordType == 'F'){
                    type = '4';
                }else if(tseOrdStruct->ordType == 'I'){
                    type = '3';
                }else if(tseOrdStruct->ordType == 'R'){
                    type = '0';
                }else if(tseOrdStruct->ordType == ' '){
                    type = ' ';
                }
                memcpy((char *)&t010MsgStruct.timeInForce,&type,1);
                break;
            case 19:    // user define part // line
                break;
            case 20:    // htsq
                memcpy((char *)&usrDefStruct.htsq,tseOrdStruct->serverNo,2);
                break;
            case 21:    // insq
                memcpy((char *)&usrDefStruct.insq,tseOrdStruct->odSeqNum,6);
                break;
            case 22:    // mgsq
                break;
            case 23:    // ktime
                break;
            case 24:    //scode
                memcpy((char *)&usrDefStruct.sCode,tseOrdStruct->source,2);
                break;
            case 25:    // ocode
                break;
            case 26:    // tlno
                break;
            case 27:    // sale
                memcpy((char *)&usrDefStruct.sale,&tseOrdStruct->custID[7],3);
                break;
            default:
                UFC::BufferedLog::Printf(" warning: enter default. this shoud not happen. , %d",i);
        }
    }
    int t010StrSize = sizeof(t010MsgStruct);
    memcpy(finalMessage,(char *)&t010MsgStruct,sizeof(t010MsgStruct));
    memcpy(&finalMessage[t010StrSize],(char *)&usrDefStruct,sizeof(usrDefStruct));
    finalMessage[sizeof(t010MsgStruct)+sizeof(usrDefStruct)] = '\0';
    
    printMsg(finalMessage,sizeof(t010MsgStruct)+sizeof(usrDefStruct));
    return finalMessage;
}

char* MessageTranslator::t020ToTseAck(char *tseAckMsg){
    /*
     * message case 0~19 are same. maybe...
     */
    char buf[8] = {'\0'};
    
    memcpy((char *)&t020MsgStruct,tseAckMsg,sizeof(t020MsgStruct));
    memcpy((char *)&usrDefStruct,&tseAckMsg[sizeof(t020MsgStruct)],sizeof(usrDefStruct));
    
    UFC::BufferedLog::Printf(" tseack message:[%s]",tseAckMsg);
    //for(int i=0;i<10;i++){
    //    UFC::BufferedLog::Printf("### %c",t020MsgStruct.functionCode[i]);
    //}
    memset((char *)&tseAckStruct,' ',sizeof(tseAckStruct));
    
    //int vectorSize = tseAckCfg->size();
    for(int i=0;i<USER_DEFINE_FIELD+T020_FIELD;i++){
        if(buf[0]!='\0'){
            memset(buf,'\0',sizeof(buf));
        }
        switch(i){
            case 0: // subsystem name
                memcpy(buf,"32",2);
                memcpy((char *)&tseAckStruct.funcCode,buf,2);
                break;
            case 1: // function code
                //memcpy((char *)&tseAckStruct.branchID,t020MsgStruct.brockerID,4);
                break;
            case 2: // message type
                //memcpy((char *)&tseAckStruct.);
                break;
            case 3: // message time
                break;
            case 4: // status code
                //memcpy()
                break;
            case 5: // broker id
                memcpy((char *)&tseAckStruct.branchID,t020MsgStruct.brockerID,4);
                break;
            case 6: // pvc id
                break;
            case 7: // order no
                memcpy((char *)&tseAckStruct.ordNo,t020MsgStruct.orderNo,5);
                break;
            case 8: // ivac no
                memcpy((char *)&tseAckStruct.custID,t020MsgStruct.ivacNo,7);
                break;
            case 9: // ivac no flag
                break;
            case 10:    // stock no
                memcpy((char *)&tseAckStruct.stockNo,t020MsgStruct.stockNo,6);
                break;
            case 11:    // price int
                memcpy((char *)&tseAckStruct.odPrice,t020MsgStruct.price_int,5);
                break;
            case 12:    // price dec
                memcpy((char *)&tseAckStruct.odPriceDecimal,t020MsgStruct.price_dec,4);
                break;
            case 13:    // quantity     // right aliment, fill zero at left 4 byte.
                memcpy(buf,"0000",4);
                memcpy((char *)&tseAckStruct.odQty,buf,4);
                memcpy((char *)&tseAckStruct.odQty[4],t020MsgStruct.quantity,6);
                break;
            case 14:    // buy sell code
                memcpy((char *)&tseAckStruct.buySell,&t020MsgStruct.buySellCode,1);
                break;
            case 15:    // exchange code
                break;           
            case 16:    // order type
                memcpy((char *)&tseAckStruct.trade,&t020MsgStruct.oderType,1);
                break;
            case 17:    // price type
                memcpy((char *)&tseAckStruct.priceFlag,&t020MsgStruct.priceType,1);
                break;
            case 18:    // time in force
                memcpy((char *)&tseAckStruct.ordType,&t020MsgStruct.timeInForce,1);
                break;
            case 19:    // order date
                memcpy((char *)&tseAckStruct.ordDate,t020MsgStruct.orderDate,8);
                break;
            case 20:    // order time
                memcpy((char *)&tseAckStruct.ordTime,t020MsgStruct.orderTime,9);
                break;
            case 21:    // before quantity
                memcpy((char *)&tseAckStruct.beforeQty,t020MsgStruct.beforeQuantity,6);
                break;
            case 22:    // after quantity
                memcpy((char *)&tseAckStruct.afterQty,t020MsgStruct.afterQuantity,6);
                break;
            case 23:    // line     (user define part.)
                break;
            case 24:    // htsq
                memcpy((char *)&tseAckStruct.serverNo,usrDefStruct.htsq,2);
                break;
            case 25:    // insq
                memcpy((char *)&tseAckStruct.odSeqNum,usrDefStruct.insq,6);
                break;
            case 26:    // mgsq
                break;
            case 27:    // ktime
                break;
            case 28:    // scode
                memcpy((char *)&tseAckStruct.source,&usrDefStruct.sCode,1);
                break;
            case 29:    // ocode
                break;
            case 30:    // tlno
                break;
            case 31:    // sale
                memcpy((char *)&tseAckStruct.custID[7],usrDefStruct.sale,3);
                break;
            case 32:    // idno
                break;
            case 33:    // grep
                break;
            case 34:    // njer
                break;
            case 35:    // fpqty
                break;
            case 36:    // cdarea
                break;
            case 37:    // cdqty
                break;
            case 38:    // crper
                break;
            case 39:    // stype
                break;
            case 40:    // otype o
                break;
            case 41:    // barea
                break;
            case 42:    // bterm
                break;
            case 43:    // etm
                break;
            case 44:    // unit
                break;
            case 45:    // prmark
                break;
            case 46:    // cnover
                break;
            case 47:    // prfee
                break;
            case 48:    // dbper
                break;
            case 49:    // dboruce
                break;
            case 50:    // iscide
                break;
            case 51:    // oocode
                break;
            case 52:    // cmsovq
                break;
            case 53:    // markdt
                break;
            case 54:    // rkle
                break;
            case 55:    // sbrjforce
                break;
            case 56:    // otqty
                break;
            case 57:    // pawd
                memcpy((char *)&tseAckStruct.custPW,usrDefStruct.pawd,4);
                break;
            case 58:    // filler
                break;
            default:
                UFC::BufferedLog::Printf(" warning: enter default. this shoud not happen. , %d",i);
        }
    }
    int tseAckSize = sizeof(tseAckStruct);
    memcpy(finalMessage,(char *)&t020MsgStruct,tseAckSize);
    finalMessage[tseAckSize] = '\0';
    
    UFC::BufferedLog::Printf(" tseAck Final Message: [%s]",finalMessage);
    return finalMessage;
}

char* MessageTranslator::r3ToTseMat(char *tseMatMsg){
    
    UFC::BufferedLog::Printf(" TseMat Message[%s]",tseMatMsg);
    //for(int i=0;i<10;i++){
    //    UFC::BufferedLog::Printf(" %c",tseOrdStruct->custID[i]);
    //}
    
    char buf[8] = {'\0'};
    
    memcpy((char *)&r3MsgStruct,tseMatMsg,sizeof(r3MsgStruct));
    memcpy((char *)&usrDefStruct,&tseMatMsg[sizeof(r3MsgStruct)],sizeof(usrDefStruct));
    
    memset(&tseMatStruct,' ',sizeof(tseMatStruct));
    
    int lfNum = 10;
    for(int i=0;i<R3_FIELD+USER_DEFINE_FIELD;i++){
        if(buf[0]!='\0'){
            memset(buf,'\0',sizeof(buf));
        }
        
        switch(i){
            case 0: // func code
                memcpy((char *)&tseMatStruct.funcCode,r3MsgStruct.functionCode,2);
                break;
            case 1: // branch id
                memcpy((char *)&tseMatStruct.branchID,r3MsgStruct.brockerID,8);
                break;
            case 2: // cust id  // right aliment fill 0
                memset((char *)&tseMatStruct.custID,'0',3);
                memcpy((char *)&tseMatStruct.custID[3],r3MsgStruct.ivacNo,7);
                break;
            case 3: // match time
                memcpy((char *)&tseMatStruct.matchTime,r3MsgStruct.mthTime,9);
                break;
            case 4: // server no
                //memcpy();
                break;
            case 5: // od seq num
                memcpy((char *)&tseMatStruct.odSeqNum,r3MsgStruct.seqNo,6);
                break;
            case 6: // mkt seq num
                memcpy((char *)&tseMatStruct.mktSeqNum,r3MsgStruct.recNo,8);
                break;
            case 7: // market
                
                break;
            case 8: // ap code
                if(tseMatStruct.apCode=='2'){
                    memcpy(buf,"O",1);
                }else if(tseMatStruct.apCode=='0'){
                    memcpy(buf,"N",1);
                }
                memcpy(buf,&r3MsgStruct.excd,1);
                break;
            case 9: // ord no
                memcpy((char *)&tseMatStruct.ordNo,r3MsgStruct.orderNo,5);
                break;
            case 10:    // forign
                
                break;
            case 11:    // stock no
                memcpy((char *)&tseMatStruct.stockNo,r3MsgStruct.stkNo,6);
                break;
            case 12:    // price
                memcpy((char *)&tseMatStruct.price,r3MsgStruct.mthPr_int,5);
                memcpy((char *)&tseMatStruct.priceDecimal,r3MsgStruct.mthPr_dec,4);
                break;
            case 13:    // match qty // right aliment fill 0
                memset((char *)&tseMatStruct.matchQty,'0',2);
                memcpy((char *)&tseMatStruct.matchQty[2],r3MsgStruct.mthQty,8);
                break;
            case 14:    // buysell
                memcpy((char *)&tseMatStruct.buySell,&r3MsgStruct.buySell,1);
                break;
            case 15:    // trade
                memcpy((char *)&tseMatStruct.trade,&r3MsgStruct.odrtpe,1);
                break;
            case 16:    // source
                break;
            case 17:    // pay price
                break;
            case 18:    // return time
                memcpy((char *)&tseMatStruct.returnTime,r3MsgStruct.messageTime,6);
                break;
            case 19:    // lf
                memcpy(buf,&lfNum,1);
                memcpy((char *)&tseMatStruct.lf,buf,1);
                break;
            default:
                UFC::BufferedLog::Printf(" warning: enter default. this shoud not happen. , %d",i);
        }
    }
    int tseMatSize = sizeof(tseMatStruct);
    memcpy(finalMessage,(char *)&tseMatStruct,tseMatSize);
    finalMessage[tseMatSize] = '\0';
    
    UFC::BufferedLog::Printf(" tseMat Final Message:[%s]",finalMessage);
    return finalMessage;
}

char* MessageTranslator::teOrdToT010(char *teOrdMsg){
    teOrdStruct = (te_ord*)teOrdMsg;     // dereference to tse_ord.
    
    //UFC::BufferedLog::Printf(" %s",teOrdMsg);
    //for(int i=0;i<10;i++){
    //    UFC::BufferedLog::Printf(" %c",teOrdStruct->custID[i]);
    //}
    memset(&usrDefStruct,' ',sizeof(usrDefStruct));
    memset(&t010MsgStruct,' ',sizeof(t010MsgStruct));
    
    for(int i=0;i<TSE_ORD_FIELD;i++){
        UFC::BufferedLog::Printf("AAA");
        switch(i){
            case 0:
                writeT010(teOrdStruct->funcCode,2,(char *)&t010MsgStruct.functionCode);    // from 2~3 total 2 byte.
                break;
            case 1:
                writeT010(teOrdStruct->branchID,4,(char *)t010MsgStruct.brokerID);
                break;
            case 2: // custID, last 3 byte is usrDefStruct->sale;
                writeT010(teOrdStruct->custID,7,(char *)&t010MsgStruct.ivacno);
                writeUsrDef(&teOrdStruct->custID[8],3,(char *)&usrDefStruct.sale);
                break;
            case 3: // custPW
                // NOTE: take 4 byte from 20 byte array. i don't know why...
                writeUsrDef(teOrdStruct->custPW,4,(char *)&usrDefStruct.pawd);
                break;
            case 4: // SERVER_NO
                // NOTE: write 2 byte to 4 byte array.
                writeUsrDef(teOrdStruct->serverNo,2,(char *)&usrDefStruct.htsq);
                break;
            case 5:
                writeUsrDef(teOrdStruct->odSeqNum,6,(char *)&usrDefStruct.insq);
                break;
            case 6:     // ordDate
                //writeUsrDef(tseOrdStruct->odSeqNum,6,(char *)&usrDefStruct.insq);
                break;
            case 7:     // ord time
                writeUsrDef(teOrdStruct->ordTime,6,(char *)&t010MsgStruct.messageTime);
                break;
            case 8: // market  
                //writeT010(&tseOrdStruct->market,1,(char*)t010MsgStruct.);
                break;
            case 9: // apCode
                //writeFromX(tseOrdStruct,2,2);
                break;
            case 10:    // ordNo
                //writeFromX(tseOrdStruct,2,2);
                break;
            case 11:    // forign
                //writeFromX(tseOrdStruct,2,2);
                break;
            case 12:    // stockNo
                writeUsrDef(teOrdStruct->stockNo,6,(char *)&t010MsgStruct.stockNo);
                break;
            case 13:    // odprice
                writeT010(teOrdStruct->odPrice,5,(char *)&t010MsgStruct.price);
                break;
            case 14:
                writeT010(teOrdStruct->odPriceDecimal,4,(char *)&t010MsgStruct.priceDecimal);
                break;
            case 15:
                writeT010(&teOrdStruct->priceFlag,1,(char *)&t010MsgStruct.priceType);
                break;
            case 16:
                // FIXME: trancade zero at left side.
                writeT010(teOrdStruct->odQty,6,(char *)&t010MsgStruct.quantity);
                break;
            case 17:
                writeT010(&teOrdStruct->buySell,1,(char *)&t010MsgStruct.buySellCode);
                break;
            case 18:
                writeT010(&teOrdStruct->trade,1,(char *)&t010MsgStruct.orderType);
                break;
            case 19:
                // FIXME: too many type.
                writeT010(&teOrdStruct->buySell,1,(char *)&t010MsgStruct.buySellCode);
                break;
            case 20:    // source
                // FIXME: 2 -> 1;
                writeT010(teOrdStruct->source,1,(char *)&usrDefStruct.sCode);
                break;
            case 21:
                //writeT010(tseOrdStruct->buySell,1,(char *)&t010MsgStruct.buySellCode);
                break;
            case 22:
                //writeT010(tseOrdStruct->buySell,1,(char *)&t010MsgStruct.buySellCode);
                break;
            case 23:
                //writeT010(tseOrdStruct->buySell,1,(char *)&t010MsgStruct.buySellCode);
                break;
            case 24:
                //writeT010(tseOrdStruct->buySell,1,(char *)&t010MsgStruct.buySellCode);
                break;
            case 25:
                //writeT010(tseOrdStruct->buySell,1,(char *)&t010MsgStruct.buySellCode);
                break;

            default:
                UFC::BufferedLog::Printf(" warning: enter default. this shoud not happen. , %d",i);
        }
    }
    int t010StrSize = sizeof(t010MsgStruct);
    memcpy(finalMessage,(char *)&t010MsgStruct,t010StrSize);
    memcpy(&finalMessage[t010StrSize],(char *)&usrDefStruct,sizeof(usrDefStruct));
    finalMessage[sizeof(t010MsgStruct)+sizeof(usrDefStruct)] = '\0';
    
    UFC::BufferedLog::Printf(" teOrdMsg:[%s]",finalMessage);
    return finalMessage;
}

char* MessageTranslator::t020ToTeAck(char *teAckMsg){
    
    UFC::BufferedLog::Printf(" %s",teAckMsg);
    //for(int i=0;i<10;i++){
    //    UFC::BufferedLog::Printf(" %c",teAckStruct->custID[i]);
    //}
    memset(&usrDefStruct,' ',sizeof(usrDefStruct));
    memset(&t010MsgStruct,' ',sizeof(t010MsgStruct));
    
    for(int i=0;i<T020_FIELD+USER_DEFINE_FIELD;i++){
        UFC::BufferedLog::Printf("AAA");
        switch(i){
            
            default:
                UFC::BufferedLog::Printf(" warning: enter default. this shoud not happen. , %d",i);
        }
    }
    int t010StrSize = sizeof(t010MsgStruct);
    memcpy(finalMessage,(char *)&t010MsgStruct,sizeof(t010MsgStruct));
    memcpy(&finalMessage[t010StrSize],(char *)&usrDefStruct,sizeof(usrDefStruct));
    //finalMessage[finalMessageSize-1] = '\0';
    return finalMessage;
}
char* MessageTranslator::r3ToTeMat(char *teMatMsg){
    
    UFC::BufferedLog::Printf(" %s",teMatMsg);
    for(int i=0;i<10;i++){
        UFC::BufferedLog::Printf(" %c",tseOrdStruct->custID[i]);
    }
    memset(&usrDefStruct,' ',sizeof(usrDefStruct));
    memset(&t010MsgStruct,' ',sizeof(t010MsgStruct));
    
    for(int i=0;i<R3_FIELD+USER_DEFINE_FIELD;i++){
        UFC::BufferedLog::Printf("AAA");
        switch(i){
           

            default:
                UFC::BufferedLog::Printf(" warning: enter default. this shoud not happen. , %d",i);
        }
    }
    int t010StrSize = sizeof(t010MsgStruct);
    memcpy(finalMessage,(char *)&t010MsgStruct,sizeof(t010MsgStruct));
    memcpy(&finalMessage[t010StrSize],(char *)&usrDefStruct,sizeof(usrDefStruct));
    //finalMessage[finalMessageSize-1] = '\0';
    return finalMessage;
}

void MessageTranslator::writeT010(char *msg,int len,char *position){
    for(int i=0;i<len;i++){
        position[i] = msg[i];
    }
    
}
void MessageTranslator::writeUsrDef(char *msg,int len,char *position){
    for(int i=0;i<len;i++){
        position[i] = msg[i];
    }
}
/*
void MessageTranslator::writeAppend(char *msg,int len){
    int i=0;
    for(;i<len;i++){
        t010Msg[msgPosition+i] = msg[i];
    }
    msgPosition+=1;
}*/

char *MessageTranslator::checkMsgType(char *msg){
    
    char *retMsg;
    if(msg[1]=='1'&&msg[65]=='T'){
        retMsg = tseOrdToT010(msg);
    }
    else if(msg[1]=='2'&&msg[65]=='T'){
        retMsg = t020ToTseAck(msg);
    }
    else if(msg[0]=='5'&&msg[1]=='0'){
        retMsg = r3ToTseMat(msg);
    }/*
    else if(msg[1]=='1'&&msg[65]=='O'){
        teOrdToT010(msg);
    }else if(msg[1]=='2'&&msg[65]=='O'){
        t020ToTeAck(msg);
    }else if(msg[1]=='3'&&msg[45]=='O'){
        r3ToTeMat(msg);
    }
    else{
        UFC::BufferedLog::Printf(" error. %c,%c",msg[1],msg[65]);
    }*/
    
    return retMsg;
}