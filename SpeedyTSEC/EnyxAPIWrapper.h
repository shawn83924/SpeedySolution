/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   EnyxAPIWrapper.h
 * Author: Kenny
 *
 * Created on November 16, 2020, 2:30 PM
 */

#ifndef ENYXAPIWRAPPER_H
#define ENYXAPIWRAPPER_H

#define APIWRAPPER_ERR_BASE          10000
#define APIWRAPPER_ERR_NEWCORES      10000
#define APIWRAPPER_ERR_GETSTACKCNT   20000
#define APIWRAPPER_ERR_GETNTHSTACK   30000
#define APIWRAPPER_ERR_GETPOLLCTXCNT 40000
#define APIWRAPPER_ERR_GETNTHPOLLCTX 50000
#define APIWRAPPER_ERR_VIFNOTREADY   55000

struct FPGATCPEventHandler
{
virtual void onConnected(unsigned int ethport_id, unsigned short session_id, void* user_data) = 0;
virtual void onConnectFail(void* user_data) = 0;
virtual void onDisconnected(unsigned int ethport_id, unsigned short session_id, void* user_data) = 0;
virtual void onDataReceived(unsigned int ethport_id, unsigned short session_id, const unsigned char* data, unsigned int len, void* user_data) = 0;
};

struct FPGATCPConnectionParam
{
    char  ip[32];
    int   port;
    int   local_port;
    void* user_data;        
};

struct FPGATCPContext
{
 virtual void tcp_open(const FPGATCPConnectionParam* cp, FPGATCPEventHandler* evh) = 0;
 virtual void tcp_close(unsigned int ethport_id, unsigned short session_id) = 0;
 virtual void tcp_send(unsigned int ethport_id, unsigned short session_id, const char* data, unsigned int len) = 0;
 virtual void addRefcnt() = 0;
 virtual void decRefCnt() = 0;  
};

extern "C"
{
    int  fpga_tcp_create_context(FPGATCPContext** context);    
}

#endif /* ENYXAPIWRAPPER_H */

