/*******************************************************************************
 *               Copyright (C) 2016, Huawei Tech. Co., Ltd.
 *                      ALL RIGHTS RESERVED
 *******************************************************************************/

/** @defgroup addcrc.c

 *  @author  Claud
 *  @version 1.0
 *  @date    2016/9/10 \n

 *  history:\n
 *          1. 2016/9/10, Claud, create this file\n\n
 *
 *
 * @{
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include "os_def.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */

#define MSG_HDR_MAGIC_OFF						0
#define MSG_HDR_MAGIC_SIZE						1
#define MSG_HDR_LEN_OFF							(MSG_HDR_MAGIC_OFF+MSG_HDR_MAGIC_SIZE)
#define MSG_HDR_LEN_SIZE						2
#define MSG_HDR_DATACRC_OFF						(MSG_HDR_LEN_OFF+MSG_HDR_LEN_SIZE)
#define MSG_HDR_DATACRC_SIZE					2
#define MSG_HDR_TOTAL_SIZE						(MSG_HDR_MAGIC_SIZE+MSG_HDR_LEN_SIZE+MSG_HDR_DATACRC_SIZE)

#define MODE_UNKNOWN	0
#define MODE_TOK		1
#define MODE_NONTOK		2


s32 htoi(s8 *s)
{  
    s32 i;  
    s32 n = 0;  
    
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X')) {  
        i = 2;
    }  
    else {  
        i = 0;  
    }
    for (; (s[i] >= '0' && s[i] <= '9') 
        || (s[i] >= 'a' && s[i] <= 'z') 
        || (s[i] >='A' && s[i] <= 'Z');++i) {  
        if (tolower(s[i]) > '9') 
            n = 16 * n + (10 + tolower(s[i]) - 'a');  
        else 
            n = 16 * n + (tolower(s[i]) - '0');
    }
    return n;  
}

void dump_cmd(s8 *cmd, u32 len)
{
    u32 i = 0;

	printf("Output:\n");
    for (i = 0; i < len; i++) {
        printf("%02X", (u8)cmd[i]);
    }
    printf("\n");
}

s32 main(s32 argc, s8 **argv)
{
    u32 cmdlen = 0;
  	s8 *cmdbuf = NULL;
	u8 opr1size = 0;
    u32 type = MODE_UNKNOWN;
    s8 *start = NULL;
    s8 bytestr[3];
    u32 i = 0;
    
	opr1size = strlen(argv[1]);
    if (opr1size > 2) {
        if (!(opr1size%2)) {
            cmdlen = sizeof(u8) + sizeof(u16) + sizeof(u16) + (opr1size/2);
            type = MODE_TOK;            
        }
        else {
            printf("Err Mode\n");
        	exit(0);
        }
    }
    else if (2 == opr1size) {
        cmdlen = sizeof(u8) + sizeof(u16) + sizeof(u16) + (argc -1);
        type = MODE_NONTOK;
    }
    else {
        printf("Err Mode\n");
    	exit(0);
	}

    cmdbuf = (u8 *)malloc(cmdlen);
    if (!cmdbuf) {
        printf("Memory error\n");
        return -1;
    }
    
    cmdbuf[MSG_HDR_MAGIC_OFF] = 0x7E;
    *(u16 *)&cmdbuf[MSG_HDR_LEN_OFF] = cmdlen - (sizeof(u8) + sizeof(u16) + sizeof(u16));
    if (MODE_TOK == type) {        
        start = argv[1];
        for (i = 0; i < (opr1size/2); i++) {
            strncpy((s8 *)bytestr, start, 2);
            cmdbuf[i+MSG_HDR_TOTAL_SIZE] = htoi(bytestr);
            start += 2;
        }
        *(u16 *)&(cmdbuf[MSG_HDR_DATACRC_OFF]) = com_crc((u8*)&cmdbuf[MSG_HDR_TOTAL_SIZE], (opr1size/2));
    }
    else if (MODE_NONTOK == type){
        for (i = 0; i < (argc-1); i++) {
            if (strlen(argv[i+1]) > 2) {
                printf("Err input\n");
        		return -1;
            }
            cmdbuf[i+MSG_HDR_TOTAL_SIZE] = htoi(argv[i+1]);
        }
        *(u16 *)&(cmdbuf[MSG_HDR_DATACRC_OFF]) = com_crc((u8*)&cmdbuf[MSG_HDR_TOTAL_SIZE], (argc-1));
    }
    dump_cmd(cmdbuf, cmdlen);
    return 0;    
}



#ifdef __cplusplus
#if __cplusplus
}
#endif /*  __cpluscplus */
#endif /*  __cpluscplus */




/** @}*/
