/* 
 * File:   TMPDefines.h
 * Author: simon
 *
 * Created on September 19, 2008, 9:57 PM
 */

#ifndef _TMPDEFINES_H
#define	_TMPDEFINES_H
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const UFCType::UInt8         TMP_HEARTBT_INT       = 30;
const UFCType::Int32         TMP_FIELD_STR_MAX     = 4096;
const UFCType::Int32         TMP_MAX_DATA_SIZE     = 2048; ///< TCP Buffer size 4096
const UFCType::Int32         TMP_HEADER_SIZE       = 15;   ///<            Seq 4 + Time 6 +Type 1 + FCMID 2 + SID 2 = 15
const UFCType::Int32         TMP_HEADER_FULL_SIZE  = 17;   ///< Length 2 + Seq 4 + Time 6 +Type 1 + FCMID 2 + SID 2 = 17
const UFCType::Int32         TMP_SIZE_DIFF         = 3;    ///< TMP Body size + Length(2) + CheckSum(1)
const UFCType::Int32         TMP_MAX_MSG_PER_L41   = 12;   ///< Upto 30 messages per L41
const UFCType::Int32         TMP_EXEC_THROUGHPUT_1 = 500;
const UFCType::Int32         TMP_EXEC_THROUGHPUT_2 = 1000;
const UFCType::Int32         TMP_REJECT_TIME_DIFF  = 2000;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const UFCType::Int32         TMP_128K_MSG_MAX      = 2;
const UFCType::Int32         TMP_128K_CMSG_MAX     = 3;
const UFCType::Int32         TMP_512K_MSG_MAX      = 3;  ///< 508/133   = 3
const UFCType::Int32         TMP_512K_CMSG_MAX     = 7;  ///< 508 /72   = 7
const UFCType::Int32         TMP_2M_MSG_MAX        = 9;  ///< 1258 /133 = 9
const UFCType::Int32         TMP_2M_CMSG_MAX       = 17; ///< 1258/72   = 17
const UFCType::UInt32        TMP_EXEC_COLLECT_TIME = 8000000; ///< 8000000 ns = 8 ms
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const UFCType::Int32         TMP_HEADER_MSGTYPE_OFFSET   = 12;
const UFCType::Int32         TMP_R02_ORDERID_OFFSET      = 23;
const UFCType::Int32         TMP_R22_ORDERID_OFFSET      = 21;
const UFCType::Int32         TMP_R03_ORDERID_OFFSET      = 21;

const UFCType::Int32         TMP_R01_ACCOUNT_OFFSET      = 66;
const UFCType::Int32         TMP_R01_ACCOUNT_FLAG_OFFSET = 70;
const UFCType::Int32         TMP_R01_ORDERID_OFFSET      = 22;
const UFCType::Int32         TMP_R01_FCMID_OFFSET        = 20;

const UFCType::Int32         TMP_R07_ORDERID_OFFSET      = 17;

const UFCType::Int32         TMP_R09_ACCOUNT_OFFSET      = 72;
const UFCType::Int32         TMP_R09_ACCOUNT_FLAG_OFFSET = 76;
const UFCType::Int32         TMP_R09_ORDERID_OFFSET      = 22;
const UFCType::Int32         TMP_R09_FCMID_OFFSET        = 20;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#endif	/* _TMPDEFINES_H */

