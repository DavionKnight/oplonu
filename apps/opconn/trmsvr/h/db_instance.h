#ifndef __DB_INSTANCE_H__
#define __DB_INSTANCE_H__
#ifdef HAVE_DB_MANAGMENT
#include "db_api.h"

#define DB_CUR_VERSION   0x0

enum{
    DB_CTC_LOID_PASSWD_ID = 1,
    DB_CTC_TRM_SVR0_CFG_ID,
    DB_CTC_TRM_SVR1_CFG_ID,
    DB_CTC_TRM_SVR2_CFG_ID,
    DB_CTC_TRM_SVR3_CFG_ID,
    /* admin user information */
    DB_CTC_ADMIN_INFO_ID ,
    /* power user infomation */
    DB_CTC_USER_INFO_ID,
    /* port admin, auto, bc ctrl */
    DB_CTC_PORT_INFO_ID,   
};

#define DB_CTC_LOID_MAX_LEN  24
#define DB_CTC_PWD_MAX_LEN   12

typedef struct{
    cs_uint8 loid[DB_CTC_LOID_MAX_LEN + 1];
    cs_uint8 passwd[DB_CTC_PWD_MAX_LEN + 1];
    cs_uint16 rsvd;
}db_ctc_loid_pwd_t;

/*************** User infomation define *******************/
#define DB_CTC_MAX_USR_LEN  64
#define DB_CTC_MAX_PWD_LEN  64

#define DB_CTC_DEFAULT_ADMIN_NAME  "admin" 
#define DB_CTC_DEFAULT_ADMIN_PWD  "greenway"
#define DB_CTC_DEFAULT_USR_NAME  "user" 
#define DB_CTC_DEFAULT_USR_PWD  "user" 
typedef struct{
    cs_uint8    usr[DB_CTC_MAX_USR_LEN + 1];
    cs_uint8    pwd[DB_CTC_MAX_PWD_LEN + 1];
    cs_uint16    rsvd;
}db_ctc_user_t;

/******************** Port base infomation ********************/
typedef struct{
    cs_uint8             admin;
    cs_uint8             auto_neg;
    cs_uint8             work_mode;
    cs_uint8             pause;
} db_ctc_port_entry_t;

#define DB_CTC_MAX_PORT   4
typedef struct{
    db_ctc_port_entry_t port[DB_CTC_MAX_PORT];
} db_ctc_port_t;
#if 0
typedef struct{
    cs_uint8    init_flag; 
    cs_uint8    uart_id;        /* UART number 0,1,2,3,4*/
    cs_uint8    data_bits;      /* 7/8  */
    cs_uint8    stop_bits;      /* 1/2 */
    cs_uint8    flow_control;   /* None/XONXOFF/HardWare */
    cs_uint8    parity;         /* None/Odd/Even */
    cs_uint8    duplex;         /* full-duplex or half-duplex*/
    cs_uint8    mode;           /* Polling or Interrupt */    
    cs_uint32   baud_rate;          
    cs_uint32   proto_type;     /* protocol type TCP 6/UDP 1*/
    cs_uint32   proto_port;     /* L4 protocol port 1~65535 */
    cs_uint32   client_timeout; /* remote client timeout 0:never thoer:1~65535s*/
    cs_uint32   max_payload;    /* 512 ~1400 bytes */
    cs_uint32   min_payload;    /* 40 ~ max_payload */
    cs_uint32   max_res_time;   /* max response time 25~1000ms */
}db_ts_cfg_t;
#endif
void db_init();
void db_restore();

cs_status  db_ctc_set_default();


#endif
#endif



