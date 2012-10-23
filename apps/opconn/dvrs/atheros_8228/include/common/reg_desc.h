/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */
 
#ifndef _REG_DESC_H
#define _REG_DESC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    unsigned short reg_id;
    unsigned short reg_addr;
    unsigned short entry_len;
    unsigned short entry_offset;
#ifdef SHELL_MODE
    char *entry_name;
#endif
}sw_reg_desc_t;

typedef struct  {
    unsigned short reg_id;
    unsigned short bit_offset;
    unsigned short bit_len;
    unsigned short flag;
#ifdef SHELL_MODE    
    char *field_name;
#endif
}sw_field_desc_t;

#ifdef SHELL_MODE    
#define sw_reg_entry_des(reg) {reg##_ID, reg##_OFFSET, reg##_E_LENGTH, \
                            reg##_E_OFFSET, reg}

#define sw_reg_field_des(reg, field) \
    {reg##_ID, reg##_##field##_BOFFSET, reg##_##field##_BLEN, \
     reg##_##field##_FLAG, field}

#else
#define sw_reg_entry_des(reg) {reg##_ID, reg##_OFFSET, reg##_E_LENGTH, \
                            reg##_E_OFFSET}

#define sw_reg_field_des(reg, field) \
    {reg##_ID, reg##_##field##_BOFFSET, reg##_##field##_BLEN, \
     reg##_##field##_FLAG}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
