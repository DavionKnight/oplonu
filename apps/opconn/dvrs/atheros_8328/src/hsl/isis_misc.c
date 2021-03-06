/*
 * Copyright (c) 2007-2009 Atheros Communications, Inc.
 * All rights reserved.
 *
 */
/**
 * @defgroup isis_misc ISIS_MISC
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "isis_misc.h"
#include "isis_reg.h"

#define ISIS_MAX_FRMAE_SIZE      9216

#define ARP_REQ_EN_OFFSET    6
#define ARP_ACK_EN_OFFSET    5
#define DHCP_EN_OFFSET       4
#define EAPOL_EN_OFFSET      3

static sw_error_t
_isis_port_misc_property_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable, a_uint32_t item)
{
    sw_error_t rv;
    a_uint32_t reg, val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == enable) {
        val = 1;
    } else if (A_FALSE == enable) {
        val = 0;
    } else {
        return SW_BAD_PARAM;
    }

    if (3 >= port_id) {
        HSL_REG_ENTRY_GET(rv, dev_id, FRAME_ACK_CTL0, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        reg &= ~(0x1UL << ((port_id << 3) + item));
        reg |= (val << ((port_id << 3) + item));

        HSL_REG_ENTRY_SET(rv, dev_id, FRAME_ACK_CTL0, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    } else {
        HSL_REG_ENTRY_GET(rv, dev_id, FRAME_ACK_CTL1, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        reg &= ~(0x1UL << (((port_id - 4) << 3) + item));
        reg |= (val << (((port_id - 4) << 3) + item));

        HSL_REG_ENTRY_SET(rv, dev_id, FRAME_ACK_CTL1, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    }
    return rv;
}

static sw_error_t
_isis_port_misc_property_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable, a_uint32_t item)
{
    sw_error_t rv;
    a_uint32_t reg, val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    if (3 >= port_id) {
        HSL_REG_ENTRY_GET(rv, dev_id, FRAME_ACK_CTL0, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        val = (reg >> ((port_id << 3) + item)) & 0x1UL;
    } else {
        HSL_REG_ENTRY_GET(rv, dev_id, FRAME_ACK_CTL1, 0,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        val = (reg >> (((port_id - 4) << 3) + item)) & 0x1UL;
    }

    if (1 == val) {
        *enable = A_TRUE;
    } else {
        *enable = A_FALSE;
    }

    return SW_OK;
}

static sw_error_t
_isis_frame_max_size_set(a_uint32_t dev_id, a_uint32_t size)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (ISIS_MAX_FRMAE_SIZE < size) {
        return SW_BAD_PARAM;
    }

    data = size;
    HSL_REG_FIELD_SET(rv, dev_id, MAX_SIZE, 0, MAX_FRAME_SIZE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_frame_max_size_get(a_uint32_t dev_id, a_uint32_t * size)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, MAX_SIZE, 0, MAX_FRAME_SIZE,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    *size = data;
    return SW_OK;
}

static sw_error_t
_isis_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, UNI_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable) {
        data &= (~((a_uint32_t) 0x1 << port_id));
    } else if (A_FALSE == enable) {
        data |= (0x1 << port_id);
    } else {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL1, 0, UNI_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_port_unk_uc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t reg, field;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, UNI_FLOOD_DP,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    field = reg & (0x1 << port_id);
    if (field) {
        *enable = A_FALSE;
    } else {
        *enable = A_TRUE;
    }

    return SW_OK;
}

static sw_error_t
_isis_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, MUL_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable) {
        data &= (~((a_uint32_t) 0x1 << port_id));
    } else if (A_FALSE == enable) {
        data |= (0x1 << port_id);
    } else {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL1, 0, MUL_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_port_unk_mc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t reg, field;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, MUL_FLOOD_DP,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    field = reg & (0x1 << port_id);
    if (field) {
        *enable = A_FALSE;
    } else {
        *enable = A_TRUE;
    }

    return SW_OK;
}

static sw_error_t
_isis_port_bc_filter_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, BC_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable) {
        data &= (~((a_uint32_t) 0x1 << port_id));
    } else if (A_FALSE == enable) {
        data |= (0x1 << port_id);
    } else {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL1, 0, BC_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_port_bc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                         a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t reg, field;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL1, 0, BC_FLOOD_DP,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    field = reg & (0x1 << port_id);
    if (field) {
        *enable = A_FALSE;
    } else {
        *enable = A_TRUE;
    }

    return SW_OK;
}

static sw_error_t
_isis_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE == enable) {
        val = 1;
    } else if (A_FALSE == enable) {
        val = 0;
    } else {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, CPU_PORT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_cpu_port_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, CPU_PORT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val) {
        *enable = A_TRUE;
    } else {
        *enable = A_FALSE;
    }

    return SW_OK;
}

static sw_error_t
_isis_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_MAC_FRWRD == cmd) {
        val = 0;
    } else if (FAL_MAC_RDT_TO_CPU == cmd) {
        val = 1;
    } else {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, PPPOE_RDT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, PPPOE_RDT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val) {
        *cmd = FAL_MAC_RDT_TO_CPU;
    } else {
        *cmd = FAL_MAC_FRWRD;
    }

    return SW_OK;
}

static sw_error_t
_isis_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE == enable) {
        val = 1;
    } else if (A_FALSE == enable) {
        val = 0;
    } else {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FRAME_ACK_CTL1, 0, PPPOE_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FRAME_ACK_CTL1, 0, PPPOE_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val) {
        *enable = A_TRUE;
    } else {
        *enable = A_FALSE;
    }

    return SW_OK;
}

static sw_error_t
_isis_arp_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_MAC_CPY_TO_CPU == cmd) {
        val = 1;
    } else if (FAL_MAC_RDT_TO_CPU == cmd) {
        val = 0;
    } else if (FAL_MAC_FRWRD == cmd) {
        val = 2;
    } else {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, ARP_CMD,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_arp_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, ARP_CMD,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val) {
        *cmd = FAL_MAC_CPY_TO_CPU;
    } else if (0 == val) {
        *cmd = FAL_MAC_RDT_TO_CPU;
    } else {
        *cmd = FAL_MAC_FRWRD;
    }

    return SW_OK;
}

static sw_error_t
_isis_eapol_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    if (FAL_MAC_CPY_TO_CPU == cmd) {
        val = 0;
    } else if (FAL_MAC_RDT_TO_CPU == cmd) {
        val = 1;
    } else {
        return SW_NOT_SUPPORTED;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, EAPOL_CMD,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_eapol_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, EAPOL_CMD,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (0 == val) {
        *cmd = FAL_MAC_CPY_TO_CPU;
    } else {
        *cmd = FAL_MAC_RDT_TO_CPU;
    }

    return SW_OK;
}

#define ISIS_MAX_PPPOE_SESSION  16
#define ISIS_MAX_SESSION_ID     0xffff

static sw_error_t
_isis_pppoe_session_add(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;
    a_uint32_t reg, i, valid, id, entry_idx = ISIS_MAX_PPPOE_SESSION;

    HSL_DEV_ID_CHECK(dev_id);

    if (session_tbl->session_id > ISIS_MAX_SESSION_ID) {
        return SW_BAD_PARAM;
    }

    if ((A_TRUE == session_tbl->multi_session)
        && (A_TRUE == session_tbl->uni_session)) {
        return SW_BAD_PARAM;
    }

    if ((A_FALSE == session_tbl->multi_session)
        && (A_FALSE == session_tbl->uni_session)) {
        return SW_BAD_PARAM;
    }

    for (i = 0; i < ISIS_MAX_PPPOE_SESSION; i++) {
        HSL_REG_ENTRY_GET(rv, dev_id, PPPOE_SESSION, i,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(PPPOE_SESSION, ENTRY_VALID, valid, reg);
        SW_GET_FIELD_BY_REG(PPPOE_SESSION, SEESION_ID, id, reg);

        if (!valid) {
            entry_idx = i;
        } else if (id == session_tbl->session_id) {
            return SW_ALREADY_EXIST;
        }
    }

    if (ISIS_MAX_PPPOE_SESSION == entry_idx) {
        return SW_NO_RESOURCE;
    }

    if (A_TRUE == session_tbl->multi_session) {
        SW_SET_REG_BY_FIELD(PPPOE_SESSION, ENTRY_VALID, 1, reg);
    } else {
        SW_SET_REG_BY_FIELD(PPPOE_SESSION, ENTRY_VALID, 2, reg);
    }
    SW_SET_REG_BY_FIELD(PPPOE_SESSION, SEESION_ID, session_tbl->session_id,
                        reg);

    HSL_REG_ENTRY_SET(rv, dev_id, PPPOE_SESSION, entry_idx,
                      (a_uint8_t *) (&reg), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    reg = 0;
    SW_SET_REG_BY_FIELD(PPPOE_EDIT, EDIT_ID, session_tbl->session_id, reg);
    HSL_REG_ENTRY_SET(rv, dev_id, PPPOE_EDIT, entry_idx, (a_uint8_t *) (&reg),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    session_tbl->entry_id = entry_idx;
    return SW_OK;
}

static sw_error_t
_isis_pppoe_session_del(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;
    a_uint32_t reg, i, valid, id;

    HSL_DEV_ID_CHECK(dev_id);

    if (session_tbl->session_id > ISIS_MAX_SESSION_ID) {
        return SW_BAD_PARAM;
    }

    for (i = 0; i < ISIS_MAX_PPPOE_SESSION; i++) {
        HSL_REG_ENTRY_GET(rv, dev_id, PPPOE_SESSION, i,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(PPPOE_SESSION, ENTRY_VALID, valid, reg);
        SW_GET_FIELD_BY_REG(PPPOE_SESSION, SEESION_ID, id, reg);

        if (((1 == valid) || (2 == valid)) && (id == session_tbl->session_id)) {
            SW_SET_REG_BY_FIELD(PPPOE_SESSION, ENTRY_VALID, 0, reg);
            SW_SET_REG_BY_FIELD(PPPOE_SESSION, SEESION_ID, 0, reg);
            HSL_REG_ENTRY_SET(rv, dev_id, PPPOE_SESSION, i,
                              (a_uint8_t *) (&reg), sizeof (a_uint32_t));
            return rv;
        }
    }

    return SW_NOT_FOUND;
}

static sw_error_t
_isis_pppoe_session_get(a_uint32_t dev_id, fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;
    a_uint32_t reg, i, valid, id;

    HSL_DEV_ID_CHECK(dev_id);

    if (session_tbl->session_id > ISIS_MAX_SESSION_ID) {
        return SW_BAD_PARAM;
    }

    for (i = 0; i < ISIS_MAX_PPPOE_SESSION; i++) {
        HSL_REG_ENTRY_GET(rv, dev_id, PPPOE_SESSION, i,
                          (a_uint8_t *) (&reg), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_GET_FIELD_BY_REG(PPPOE_SESSION, ENTRY_VALID, valid, reg);
        SW_GET_FIELD_BY_REG(PPPOE_SESSION, SEESION_ID, id, reg);

        if (((1 == valid) || (2 == valid)) && (id == session_tbl->session_id)) {
            if (1 == valid) {
                session_tbl->multi_session = A_TRUE;
                session_tbl->uni_session = A_FALSE;
            } else {
                session_tbl->multi_session = A_FALSE;
                session_tbl->uni_session = A_TRUE;
            }

            session_tbl->entry_id = i;
            return SW_OK;
        }
    }

    return SW_NOT_FOUND;
}

static sw_error_t
_isis_ripv1_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, 0, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == enable) {
        data = 1;
    } else if (A_FALSE == enable) {
        data = 0;
    } else {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FORWARD_CTL0, 0, RIP_CPY_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}

static sw_error_t
_isis_ripv1_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, 0, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FORWARD_CTL0, 0, RIP_CPY_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == data) {
        *enable = A_TRUE;
    } else {
        *enable = A_FALSE;
    }

    return SW_OK;
}

/**
 * @brief Set max frame size which device can received on a particular device.
 * @details    Comments:
 * The granularity of packets size is byte. 
 * @param[in] dev_id device id
 * @param[in] size packet size
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_frame_max_size_set(a_uint32_t dev_id, a_uint32_t size)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_frame_max_size_set(dev_id, size);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get max frame size which device can received on a particular device.
 * @details   Comments:
 *    The unit of packets size is byte.
 * @param[in] dev_id device id
 * @param[out] size packet size
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_frame_max_size_get(a_uint32_t dev_id, a_uint32_t * size)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_frame_max_size_get(dev_id, size);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flooding status of unknown unicast packets on a particular port.
 * @details  Comments:
 *   If enable unknown unicast packets filter on one port then unknown
 *   unicast packets can't flood out from this port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_unk_uc_filter_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get flooding status of unknown unicast packets on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_unk_uc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_unk_uc_filter_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flooding status of unknown multicast packets on a particular port.
 * @details  Comments:
 *   If enable unknown multicast packets filter on one port then unknown
 *   multicast packets can't flood out from this port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_unk_mc_filter_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/** @brief Get flooding status of unknown multicast packets on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_unk_mc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_unk_mc_filter_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set flooding status of broadcast packets on a particular port.
 * @details  Comments:
 *   If enable unknown multicast packets filter on one port then unknown
 *   multicast packets can't flood out from this port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_bc_filter_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_bc_filter_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/** @brief Get flooding status of broadcast packets on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_bc_filter_get(a_uint32_t dev_id, fal_port_t port_id,
                        a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_bc_filter_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set cpu port status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code 
 */
HSL_LOCAL sw_error_t
isis_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_cpu_port_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get cpu port status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code 
 */
HSL_LOCAL sw_error_t
isis_cpu_port_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_cpu_port_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set pppoe packets forwarding command on a particular device.
 * @details     comments:
 *   Particular device may only support parts of forwarding commands.
 *   Ihis operation will take effect only after enabling pppoe packets
 *   hardware acknowledgement
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_pppoe_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_pppoe_cmd_set(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get pppoe packets forwarding command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code 
 */
HSL_LOCAL sw_error_t
isis_pppoe_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_pppoe_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set pppoe packets hardware acknowledgement status on particular device.
 * @details     comments:
 *   Particular device may only support parts of pppoe packets.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code 
 */
HSL_LOCAL sw_error_t
isis_pppoe_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_pppoe_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get pppoe packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_pppoe_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_pppoe_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set dhcp packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_dhcp_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_misc_property_set(dev_id, port_id, enable, DHCP_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get dhcp packets hardware acknowledgement status on particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_dhcp_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_misc_property_get(dev_id, port_id, enable, DHCP_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp packets forwarding command on a particular device.
 * @details     comments:
 *   Particular device may only support parts of forwarding commands.
 *   Ihis operation will take effect only after enabling arp packets
 *   hardware acknowledgement
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_arp_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_arp_cmd_set(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp packets forwarding command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code 
 */
HSL_LOCAL sw_error_t
isis_arp_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_arp_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set eapol packets forwarding command on a particular device.
 * @details     comments:
 *   Particular device may only support parts of forwarding commands.
 *   Ihis operation will take effect only after enabling eapol packets
 *   hardware acknowledgement
 * @param[in] dev_id device id
 * @param[in] cmd forwarding command
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_eapol_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_eapol_cmd_set(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get eapol packets forwarding command on a particular device.
 * @param[in] dev_id device id
 * @param[out] cmd forwarding command
 * @return SW_OK or error code 
 */
HSL_LOCAL sw_error_t
isis_eapol_cmd_get(a_uint32_t dev_id, fal_fwd_cmd_t * cmd)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_eapol_cmd_get(dev_id, cmd);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Add a pppoe session entry to a particular device.
 * @param[in] dev_id device id
 * @param[in] session_tbl pppoe session table
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_pppoe_session_table_add(a_uint32_t dev_id,
                              fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_pppoe_session_add(dev_id, session_tbl);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Delete a pppoe session entry from a particular device.
 * @param[in] dev_id device id
 * @param[in] session_tbl pppoe session table
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_pppoe_session_table_del(a_uint32_t dev_id,
                              fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_pppoe_session_del(dev_id, session_tbl);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get a pppoe session entry from a particular device.
 * @param[in] dev_id device id
 * @param[out] session_tbl pppoe session table
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_pppoe_session_table_get(a_uint32_t dev_id,
                              fal_pppoe_session_t * session_tbl)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_pppoe_session_get(dev_id, session_tbl);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set eapol packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_eapol_status_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_misc_property_set(dev_id, port_id, enable, EAPOL_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get eapol packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_eapol_status_get(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_misc_property_get(dev_id, port_id, enable, EAPOL_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set rip v1 packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ripv1_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ripv1_status_set(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get rip v1 packets hardware acknowledgement status on a particular port.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_ripv1_status_get(a_uint32_t dev_id, a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_ripv1_status_get(dev_id, enable);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp req packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_arp_req_status_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_misc_property_set(dev_id, port_id, enable,
                                      ARP_REQ_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp req packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_arp_req_status_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_misc_property_get(dev_id, port_id, enable,
                                      ARP_REQ_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Set arp ack packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_arp_ack_status_set(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_misc_property_set(dev_id, port_id, enable,
                                      ARP_ACK_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Get arp ack packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[out] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
isis_port_arp_ack_status_get(a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _isis_port_misc_property_get(dev_id, port_id, enable,
                                      ARP_ACK_EN_OFFSET);
    HSL_API_UNLOCK;
    return rv;
}

sw_error_t
isis_misc_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->frame_max_size_set = isis_frame_max_size_set;
        p_api->frame_max_size_get = isis_frame_max_size_get;
        p_api->port_unk_uc_filter_set = isis_port_unk_uc_filter_set;
        p_api->port_unk_uc_filter_get = isis_port_unk_uc_filter_get;
        p_api->port_unk_mc_filter_set = isis_port_unk_mc_filter_set;
        p_api->port_unk_mc_filter_get = isis_port_unk_mc_filter_get;
        p_api->port_bc_filter_set = isis_port_bc_filter_set;
        p_api->port_bc_filter_get = isis_port_bc_filter_get;
        p_api->cpu_port_status_set = isis_cpu_port_status_set;
        p_api->cpu_port_status_get = isis_cpu_port_status_get;
        p_api->pppoe_cmd_set = isis_pppoe_cmd_set;
        p_api->pppoe_cmd_get = isis_pppoe_cmd_get;
        p_api->pppoe_status_set = isis_pppoe_status_set;
        p_api->pppoe_status_get = isis_pppoe_status_get;
        p_api->port_dhcp_set = isis_port_dhcp_set;
        p_api->port_dhcp_get = isis_port_dhcp_get;
        p_api->arp_cmd_set = isis_arp_cmd_set;
        p_api->arp_cmd_get = isis_arp_cmd_get;
        p_api->eapol_cmd_set = isis_eapol_cmd_set;
        p_api->eapol_cmd_get = isis_eapol_cmd_get;
        p_api->pppoe_session_table_add = isis_pppoe_session_table_add;
        p_api->pppoe_session_table_del = isis_pppoe_session_table_del;
        p_api->pppoe_session_table_get = isis_pppoe_session_table_get;
        p_api->eapol_status_set = isis_eapol_status_set;
        p_api->eapol_status_get = isis_eapol_status_get;
        p_api->ripv1_status_set = isis_ripv1_status_set;
        p_api->ripv1_status_get = isis_ripv1_status_get;
        p_api->port_arp_req_status_set = isis_port_arp_req_status_set;
        p_api->port_arp_req_status_get = isis_port_arp_req_status_get;
        p_api->port_arp_ack_status_set = isis_port_arp_ack_status_set;
        p_api->port_arp_ack_status_get = isis_port_arp_ack_status_get;
    }
#endif

    return SW_OK;
}

/**
 * @}
 */
