#ifndef _TLV_PARSER_H_
#define _TLV_PARSER_H_

#define TLV_PARSER_OK        (0)
#define TLV_PARSER_ERROR    (-1)

#define TLV_HEADER_MAGIC    (0x84739458)

#define MAX_TLV_INSTANCE    2

#define startup_cfg_printf  cs_printf
#define TLV_PARSER_OFFSET(structure, member) ((int) &(((structure *) 0) -> member))

#define CFG_MAGIC_OFFSET    TLV_PARSER_OFFSET(tlv_header_t, magic)
#define CFG_CRC_OFFSET      TLV_PARSER_OFFSET(tlv_header_t, cfg_crc)
#define CFG_ACTIVE_OFFSET   TLV_PARSER_OFFSET(tlv_header_t, active)
#define CFG_FLAG_OFFSET     TLV_PARSER_OFFSET(tlv_header_t, reserved)
#define CFG_LEN_OFFSET      TLV_PARSER_OFFSET(tlv_header_t, cfg_len)
#define CFG_VERSION_OFFSET  TLV_PARSER_OFFSET(tlv_header_t, version)

#define TLV_TYPE_OFFSET     TLV_PARSER_OFFSET(tlv_t, type)
#define TLV_LEN_OFFSET      TLV_PARSER_OFFSET(tlv_t, len)
#define TLV_VALUE_OFFSET    TLV_PARSER_OFFSET(tlv_t, data)

#define TLV_INVALID_INST_ID        0

typedef struct
{
    unsigned int    magic;
    unsigned short  cfg_crc;
    unsigned char   active;
    unsigned char   reserved;
    unsigned int    cfg_len;
    unsigned int    version;
} tlv_header_t;

#define TLV_CFG_ENC_FLAG     0x01

typedef struct
{
    unsigned short  type;
    unsigned short  len;    /*The max length of this field, not the real length*/
    unsigned char   data[1];
} tlv_t;

typedef struct
{
    void            *base_address;
    unsigned int    *tlv_ptr_list;
    unsigned int    tlv_num;
    unsigned int    max_tlv_len;
    int             (*store)(void *base_addr , int len);
} tlv_instance_t;

unsigned int tlv_inst_init
    (
    void            *base_address,
    unsigned int    max_cfg_len,
    int             (*store)(void *base_addr , int len)
    );
unsigned int tlv_inst_duplicate
    (
    tlv_instance_t *src
    );

int tlv_load
    (
    unsigned int instance_id
    );
int tlv_read_char
    (
    unsigned int instance_id,
    unsigned short type, 
    unsigned char *value
    );
int tlv_read_short
    (
    unsigned int instance_id,
    unsigned short type, 
    unsigned short *value
    );
int tlv_read_int
    (
    unsigned int instance_id,
    unsigned short type , 
    unsigned int *value
    );
int tlv_read_buffer
    (
    unsigned int instance_id,
    unsigned short type, 
    unsigned int len, 
    unsigned char *value
    );
int tlv_write_char
    (
    unsigned int instance_id,
    unsigned short type, 
    unsigned char value
    );
int tlv_write_short
    (
    unsigned int instance_id,
    unsigned short type, 
    unsigned short value
    );
int tlv_write_int
    (
    unsigned int instance_id,
    unsigned short type, 
    unsigned int value
    );
int tlv_write_buffer
    (
    unsigned int instance_id,
    unsigned short type , 
    int max_len , 
    int len , 
    unsigned char *value
    );
void tlv_dump
    (
    unsigned int instance_id
    );
int tlv_write_flag
    (
    unsigned int instance_id,
    unsigned char value
    );
int tlv_read_flag
    (
    unsigned int instance_id,
    unsigned char *value
    );
int tlv_write_active
    (
    unsigned int instance_id,
    unsigned char value
    );
int tlv_write_version
    (
    unsigned int instance_id,
    unsigned int value
    );

#endif

