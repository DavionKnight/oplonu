/* Atheros Driver Porting */

#include "stddef.h"
#include "opconn_atheros.h"
#include "exports.h"
#include "extra.h"

static hsl_dev_t dev_table[SW_MAX_NR_DEV];
static port_info_t *p_port_info[SW_MAX_NR_DEV] = { 0 };
a_uint8_t api_ifname[16] = { 0 };
static ssdk_init_cfg * garuda_cfg[SW_MAX_NR_DEV] = { 0 };

extern OPL_STATUS mdioRegisterWrite(UINT32 deviceAddr,UINT32 regAddr,UINT16 data0);
extern OPL_STATUS mdioRegisterRead(UINT32 deviceAddr,UINT32 regAddr,UINT16 * data0);

mdio_reg_set ssdk_mdio_set    = mdioRegisterWrite;
mdio_reg_get ssdk_mdio_get    = mdioRegisterRead;

hsl_reg_func_t *
hsl_reg_ptr_get(a_uint32_t dev_id)
{
    if (dev_id >= SW_MAX_NR_DEV)
        return NULL;

    return &(dev_table[dev_id].dev_func);
}

a_bool_t
hsl_port_prop_check(a_uint32_t dev_id, fal_port_t port_id,
                    hsl_port_prop_t p_type)
{
    fal_pbmp_t pbitmap;

    if (dev_id >= SW_MAX_NR_DEV) 
        return A_FALSE;

    if (HSL_PP_BUTT <= p_type) {
        return A_FALSE;
    }

    pbitmap = p_port_info[dev_id]->property[p_type];

    return SW_IS_PBMP_MEMBER(pbitmap, port_id);
}

a_bool_t
hsl_mports_prop_check(a_uint32_t dev_id, fal_pbmp_t port_bitmap,
                      hsl_port_prop_t p_type)
{
    fal_pbmp_t pbitmap;

    if (dev_id >= SW_MAX_NR_DEV) 
        return A_FALSE;

    if (HSL_PP_BUTT <= p_type) {
        return A_FALSE;
    }

    pbitmap = p_port_info[dev_id]->property[p_type];

    return (SW_IS_PBMP_INCLUDE(pbitmap, port_bitmap));
}

hsl_dev_t *
hsl_dev_ptr_get(a_uint32_t dev_id)
{
    if (dev_id >= SW_MAX_NR_DEV)
        return NULL;

    return &dev_table[dev_id];
}

/**
 * @brief Get cpu port status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code 
 */
HSL_LOCAL sw_error_t
garuda_cpu_port_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

    HSL_REG_FIELD_GET(rv, dev_id, CPU_PORT, 0, CPU_PORT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (1 == val) {
        *enable = A_TRUE;
    } else {
        *enable = A_FALSE;
    }

    return SW_OK;
}


/**
 * @brief Set cpu port status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code 
 */
HSL_LOCAL sw_error_t
garuda_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, CPU_PORT, 0, CPU_PORT_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

/**
 * @brief Set status of braodcast packets broadcasting to cpu on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code 
 */
HSL_LOCAL sw_error_t
garuda_bc_to_cpu_port_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, FLOOD_MASK, 0, BROAD_TO_CPU,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

static void
garuda_fdb_fill_addr(fal_mac_addr_t addr, a_uint32_t * reg0, a_uint32_t * reg1)
{
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_ADDR_BYTE0, addr.uc[0], *reg1);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_ADDR_BYTE1, addr.uc[1], *reg1);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_ADDR_BYTE2, addr.uc[2], *reg1);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC1, AT_ADDR_BYTE3, addr.uc[3], *reg1);

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_ADDR_BYTE4, addr.uc[4], *reg0);
    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_ADDR_BYTE5, addr.uc[5], *reg0);

    return;
}



static sw_error_t
garuda_atu_sw_to_hw(a_uint32_t dev_id, const fal_fdb_entry_t * entry,
                    a_uint32_t reg[])
{
    a_uint32_t port;

    if (A_FALSE == entry->portmap_en) {
        if (A_TRUE !=
            hsl_port_prop_check(dev_id, entry->port.id, HSL_PP_INCL_CPU)) {
            return SW_BAD_PARAM;
        }

        port = 0x1UL << entry->port.id;
    } else {
        if (A_FALSE ==
            hsl_mports_prop_check(dev_id, entry->port.map, HSL_PP_INCL_CPU)) {
            return SW_BAD_PARAM;
        }

        port = entry->port.map;
    }

    if (FAL_MAC_CPY_TO_CPU == entry->dacmd) {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, COPY_TO_CPU, 1, reg[2]);
    } else if (FAL_MAC_RDT_TO_CPU == entry->dacmd) {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, REDRCT_TO_CPU, 1, reg[2]);
    } else if (FAL_MAC_FRWRD != entry->dacmd) {
        return SW_NOT_SUPPORTED;
    }

    if (FAL_MAC_DROP == entry->sacmd) {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, SA_DROP_EN, 1, reg[2]);
    } else if (FAL_MAC_FRWRD != entry->sacmd) {
        return SW_NOT_SUPPORTED;
    }

    if (A_TRUE == entry->leaky_en) {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, LEAKY_EN, 1, reg[2]);
    } else {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, LEAKY_EN, 0, reg[2]);
    }

    if (A_TRUE == entry->static_en) {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_STATUS, 15, reg[2]);
    } else {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_STATUS, 7, reg[2]);
    }

    if (A_TRUE == entry->mirror_en) {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, MIRROR_EN, 1, reg[2]);
    }

    if (A_TRUE == entry->clone_en) {
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, CLONE_EN, 1, reg[2]);
    }

    if (A_TRUE == entry->da_pri_en) {
        hsl_dev_t *p_dev;

        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_PRI_EN, 1, reg[2]);

        SW_RTN_ON_NULL(p_dev = hsl_dev_ptr_get(dev_id));

        if (entry->da_queue > (p_dev->nr_queue - 1))
            return SW_BAD_PARAM;

        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, AT_PRI, entry->da_queue, reg[2]);
    }

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC2, DES_PORT, port, reg[2]);
    garuda_fdb_fill_addr(entry->addr, &reg[0], &reg[1]);

    return SW_OK;
}



static sw_error_t
garuda_fdb_commit(a_uint32_t dev_id, a_uint32_t op)
{
    sw_error_t rv;
    a_uint32_t busy = 1;
    a_uint32_t full_vio;
    a_uint32_t i = 1000;
    a_uint32_t entry;
    a_uint32_t hwop = op;

    while (busy && --i) {
        HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_FUNC0, 0, AT_BUSY,
                          (a_uint8_t *) (&busy), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
//        aos_udelay(5);
    }

    if (0 == i) {
        return SW_BUSY;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, ADDR_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_BUSY, 1, entry);

    if (ARL_FLUSH_PORT_AND_STATIC == hwop) {
        hwop = ARL_FLUSH_PORT_UNICAST;
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, FLUSH_ST_EN, 1, entry);
    }

    if (ARL_FLUSH_PORT_NO_STATIC == hwop) {
        hwop = ARL_FLUSH_PORT_UNICAST;
        SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, FLUSH_ST_EN, 0, entry);
    }

    SW_SET_REG_BY_FIELD(ADDR_TABLE_FUNC0, AT_FUNC, hwop, entry);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    busy = 1;
    i = 1000;
    while (busy && --i) {
        HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_FUNC0, 0, AT_BUSY,
                          (a_uint8_t *) (&busy), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
//        aos_udelay(5);
    }

    if (0 == i) {
        return SW_FAIL;
    }

    HSL_REG_FIELD_GET(rv, dev_id, ADDR_TABLE_FUNC0, 0, AT_FULL_VIO,
                      (a_uint8_t *) (&full_vio), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (full_vio) {
        /* must clear AT_FULL_VOI bit */
        entry = 0x1000;
        HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC0, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    
        if (ARL_LOAD_ENTRY == hwop) {
            return SW_FULL;
        } else if ((ARL_PURGE_ENTRY == hwop)
                    || (ARL_FLUSH_PORT_UNICAST == hwop)) {
            return SW_NOT_FOUND;
        } else {
            return SW_FAIL;
        }
    }

    return SW_OK;
}



/**
 * @brief Add a Fdb entry
 * @param[in] dev_id device id
 * @param[in] entry fdb entry
 * @return SW_OK or error code
 */

HSL_LOCAL sw_error_t
garuda_fdb_add(a_uint32_t dev_id, const fal_fdb_entry_t * entry)
{
    sw_error_t rv;
    a_uint32_t reg[3] = { 0, 0, 0 };

    HSL_DEV_ID_CHECK(dev_id);

    rv = garuda_atu_sw_to_hw(dev_id, entry, reg);
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC2, 0,
                      (a_uint8_t *) (&reg[2]), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC1, 0, (a_uint8_t *) (&reg[1]),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    HSL_REG_ENTRY_SET(rv, dev_id, ADDR_TABLE_FUNC0, 0, (a_uint8_t *) (&reg[0]),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    rv = garuda_fdb_commit(dev_id, ARL_LOAD_ENTRY);

    return rv;
}


static sw_error_t
garuda_reg_entry_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                     a_uint32_t entry_offset, a_uint16_t entry_len,
                     a_uint8_t value[], a_uint8_t value_len)
{
    hsl_reg_func_t *reg_func;

    if (entry_len != sizeof (a_uint32_t) || value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    if ((reg_func = hsl_reg_ptr_get(dev_id)) == NULL)
        return SW_NOT_INITIALIZED;

    SW_RTN_ON_ERROR(reg_func->reg_get
                    (dev_id, reg_addr + entry_offset, value,
                     sizeof (a_uint32_t)));

    return SW_OK;
}

static sw_error_t
garuda_reg_entry_set(a_uint32_t dev_id, a_uint32_t reg_addr,
                     a_uint32_t entry_offset, a_uint16_t entry_len,
                     const a_uint8_t value[], a_uint8_t value_len)
{
    hsl_reg_func_t *reg_func;
    a_uint32_t reg_val;

    if (entry_len != sizeof (a_uint32_t) || value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    if ((reg_func = hsl_reg_ptr_get(dev_id)) == NULL)
        return SW_NOT_INITIALIZED;

    reg_val = *((a_uint32_t *) value);
    SW_RTN_ON_ERROR(reg_func->
                    reg_set(dev_id, reg_addr + entry_offset,
                            (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));
    return SW_OK;
}

static sw_error_t
garuda_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                     a_uint32_t entry_offset, a_uint16_t bit_offset,
                     a_uint16_t field_len, a_uint8_t value[],
                     a_uint8_t value_len)
{
    hsl_reg_func_t *reg_func;
    a_uint32_t reg_val = 0;

    if ((bit_offset >= 32 || (field_len > 32)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    if ((reg_func = hsl_reg_ptr_get(dev_id)) == NULL)
        return SW_NOT_INITIALIZED;

    SW_RTN_ON_ERROR(reg_func->reg_get
                    (dev_id, reg_addr + entry_offset, (a_uint8_t *) & reg_val,
                     sizeof (a_uint32_t)));

    *((a_uint32_t *) value) = SW_REG_2_FIELD(reg_val, bit_offset, field_len);

    return SW_OK;
}

static sw_error_t
garuda_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr,
                     a_uint32_t entry_offset, a_uint16_t bit_offset,
                     a_uint16_t field_len, const a_uint8_t value[],
                     a_uint8_t value_len)
{
    hsl_reg_func_t *reg_func;
    a_uint32_t reg_val;
    a_uint32_t field_val = *((a_uint32_t *) value);

    if ((bit_offset >= 32 || (field_len > 32)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    if ((reg_func = hsl_reg_ptr_get(dev_id)) == NULL)
        return SW_NOT_INITIALIZED;

    SW_RTN_ON_ERROR(reg_func->
                    reg_get(dev_id, reg_addr + entry_offset,
                            (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    SW_REG_SET_BY_FIELD_U32(reg_val, field_val, bit_offset, field_len);

    SW_RTN_ON_ERROR(reg_func->
                    reg_set(dev_id, reg_addr + entry_offset,
                            (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

    return SW_OK;
}


sw_error_t
sd_reg_mdio_set(a_uint32_t phy, a_uint32_t reg, a_uint16_t data)
{
    sw_error_t rv = SW_NOT_SUPPORTED;

    if (NULL != ssdk_mdio_set) {
        rv = ssdk_mdio_set(phy, reg, data);
    } else {
        return SW_NOT_SUPPORTED;
    }

    if (SW_OK != rv) {
        rv = SW_FAIL;
    }

    return rv;
}


sw_error_t
sd_reg_mdio_get(a_uint32_t phy, a_uint32_t reg, a_uint16_t * data)
{
    int rv;

    if (NULL != ssdk_mdio_get) {
         rv = ssdk_mdio_get(phy, reg, data);   
    } else {
        return SW_NOT_SUPPORTED;
    }

    return rv;
}

static sw_error_t
garuda_phy_reg_get(a_uint32_t dev_id, a_uint32_t phy_addr,
                   a_uint8_t reg, a_uint16_t * value)
{
//    aos_lock_bh(&mdio_lock);
    sd_reg_mdio_get(phy_addr, reg, value);
//    aos_unlock_bh(&mdio_lock);
    return SW_OK;
}

static sw_error_t
garuda_phy_reg_set(a_uint32_t dev_id, a_uint32_t phy_addr,
                   a_uint8_t reg, a_uint16_t value)
{
//    aos_lock_bh(&mdio_lock);
    sd_reg_mdio_set(phy_addr, reg, value);
//    aos_unlock_bh(&mdio_lock);
    return SW_OK;
}


static sw_error_t
garuda_mdio_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                    a_uint8_t value[], a_uint8_t value_len)
{
    a_uint32_t reg_word_addr;
    a_uint32_t phy_addr, reg_val;
    a_uint16_t phy_val;
    a_uint8_t phy_reg;
    sw_error_t retVal;
	a_uint16_t	lowval, hival;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (a_uint16_t) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */

//    aos_lock_bh(&mdio_lock);
    retVal = sd_reg_mdio_set(phy_addr, phy_reg, phy_val);
    if (SW_OK != retVal)
    {
        printf("garuda_mdio_reg_get : sd_reg_mdio_set 1 error: %d\r\n", retVal);
        return retVal;
    }


    /* For some registers such as MIBs, since it is read/clear, we should */
    /* read the lower 16-bit register then the higher one */

    /* read register in lower address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */

	lowval = 0;
    retVal = sd_reg_mdio_get(phy_addr, phy_reg, &lowval);
    if (SW_OK != retVal)
    {
        printf("garuda_mdio_reg_get : sd_reg_mdio_get 2 error: %d\r\n", retVal);
        return retVal;
    }

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */

	hival = 0;
	retVal = sd_reg_mdio_get(phy_addr, phy_reg, &hival);
    if (SW_OK != retVal)
    {
        printf("garuda_mdio_reg_get : sd_reg_mdio_get 3 error: %d\r\n", retVal);
        return retVal;
    }
    reg_val = (hival << 16) + lowval;

//    aos_unlock_bh(&mdio_lock);
    memcpy(value, &reg_val, sizeof (a_uint32_t));

    return SW_OK;
}


static sw_error_t
garuda_mdio_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
                    a_uint8_t value_len)
{
    a_uint32_t reg_word_addr;
    a_uint32_t phy_addr, reg_val;
    a_uint16_t phy_val;
    a_uint8_t phy_reg;
    sw_error_t retVal;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    memcpy(&reg_val, value, sizeof (a_uint32_t));

    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (a_uint16_t) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */

//    aos_lock_bh(&mdio_lock);

    retVal = sd_reg_mdio_set(phy_addr, phy_reg, phy_val);
    if (SW_OK != retVal)
    {
        printf("garuda_mdio_reg_set : sd_reg_mdio_set 1 error: %d\r\n", retVal);
        return retVal;
    }

    /* For some registers such as ARL and VLAN, since they include BUSY bit */
    /* in lower address, we should write the higher 16-bit register then the */
    /* lower one */

    /* write register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (a_uint16_t) ((reg_val >> 16) & 0xffff);

    retVal = sd_reg_mdio_set(phy_addr, phy_reg, phy_val);
    if (SW_OK != retVal)
    {
        printf("garuda_mdio_reg_set : sd_reg_mdio_set 2 error: %d\r\n", retVal);
        return retVal;
    }

    /* write register in lower address */
    reg_word_addr--;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (a_uint16_t) (reg_val & 0xffff);

    retVal = sd_reg_mdio_set(phy_addr, phy_reg, phy_val); 
    if (SW_OK != retVal)
    {
        printf("garuda_mdio_reg_set : sd_reg_mdio_set 3 error: %d\r\n", retVal);
        return retVal;
    }

//    aos_unlock_bh(&mdio_lock);
    return SW_OK;
}


sw_error_t
garuda_reg_access_mode_init(a_uint32_t dev_id, hsl_access_mode mode)
{
    hsl_reg_func_t *reg_func;
    hsl_dev_t *dev;

    if ((dev = hsl_dev_ptr_get(dev_id)) == NULL)
        return SW_NOT_INITIALIZED;

    if ((reg_func = hsl_reg_ptr_get(dev_id)) == NULL)
        return SW_NOT_INITIALIZED;

    if (mode == reg_func->reg_access_mode)
        return SW_OK;

//    aos_lock_init(&mdio_lock);

    reg_func->phy_get = garuda_phy_reg_get;
    reg_func->phy_set = garuda_phy_reg_set;
	reg_func->reg_get = garuda_mdio_reg_get;
	reg_func->reg_set = garuda_mdio_reg_set;

    reg_func->reg_entry_get = garuda_reg_entry_get;
    reg_func->reg_entry_set = garuda_reg_entry_set;
    reg_func->reg_field_get = garuda_reg_field_get;
    reg_func->reg_field_set = garuda_reg_field_set;

    return SW_OK;
}


static sw_error_t
garuda_dev_init(a_uint32_t dev_id, hsl_init_mode cpu_mode)
{
    hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

    pdev->nr_ports = 6;
    pdev->nr_phy = 5;
    pdev->cpu_port_nr = 0;
    pdev->nr_vlans = 4096;
    pdev->nr_queue = 4;
    pdev->cpu_mode = cpu_mode;

    return SW_OK;
}

sw_error_t
hsl_port_prop_init(void)
{
    a_uint32_t i;

    for (i = 0; i < SW_MAX_NR_DEV; i++)
        p_port_info[i] = NULL;

    return SW_OK;
}

sw_error_t
hsl_port_prop_init_by_dev(a_uint32_t dev_id)
{
    port_info_t *p_mem;

    HSL_DEV_ID_CHECK(dev_id);

    p_mem = malloc(sizeof (port_info_t));
    if (p_mem == NULL)
        return SW_OUT_OF_MEM;

    bzero(p_mem, sizeof (port_info_t));
    p_port_info[dev_id] = p_mem;

    return SW_OK;
}


sw_error_t
hsl_port_prop_portmap_set(a_uint32_t dev_id, fal_port_t port_id)
{
    HSL_DEV_ID_CHECK(dev_id);

	if (port_id > SW_MAX_NR_PORT)
        return SW_OUT_OF_RANGE;

    SW_PBMP_ADD_PORT(p_port_info[dev_id]->dev_portmap, port_id);

    return SW_OK;
}

a_bool_t
hsl_port_validity_check(a_uint32_t dev_id, fal_port_t port_id)
{
    fal_pbmp_t pbitmap;

    if (dev_id >= SW_MAX_NR_DEV) 
        return A_FALSE;

    pbitmap = p_port_info[dev_id]->dev_portmap;

    return SW_IS_PBMP_MEMBER(pbitmap, port_id);
}


sw_error_t
hsl_port_prop_set(a_uint32_t dev_id, fal_port_t port_id, hsl_port_prop_t p_type)
{
    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_validity_check(dev_id, port_id)) {
        return SW_OUT_OF_RANGE;
    }

	if (HSL_PP_BUTT <= p_type) {
        return A_FALSE;
    }

    SW_PBMP_ADD_PORT(p_port_info[dev_id]->property[p_type], port_id);

    return SW_OK;
}


sw_error_t
hsl_port_prop_set_phyid(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t phy_id)
{
    HSL_DEV_ID_CHECK(dev_id);

	if (A_FALSE == hsl_port_validity_check(dev_id, port_id)) {
		return SW_BAD_PARAM;
	}

    p_port_info[dev_id]->phy_id[port_id] = phy_id;
    return SW_OK;
}


static sw_error_t
garuda_portproperty_init(a_uint32_t dev_id, hsl_init_mode mode)
{
    hsl_port_prop_t p_type;
    hsl_dev_t *pdev = NULL;
    fal_port_t port_id;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

    for (port_id = 0; port_id < pdev->nr_ports; port_id++) {
        hsl_port_prop_portmap_set(dev_id, port_id);

        for (p_type = HSL_PP_PHY; p_type < HSL_PP_BUTT; p_type++) {
            if (HSL_NO_CPU == mode) {
                SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                continue;
            }

            switch (p_type) {
            case HSL_PP_PHY:
                if (HSL_CPU_1 != mode) {
                    if ((port_id != pdev->cpu_port_nr)
                        && (port_id != (pdev->nr_ports -1))) {
                        SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                    }
                } else {
                    if (port_id != pdev->cpu_port_nr) {
                        SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                    }
                }
                break;

            case HSL_PP_INCL_CPU:
                /* include cpu port and wan port in some cases */
                if (!((HSL_CPU_2 == mode) && (port_id == (pdev->nr_ports - 1))))
                    SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));

                break;

            case HSL_PP_EXCL_CPU:
                /* exclude cpu port and wan port in some cases */
                if ((port_id != pdev->cpu_port_nr)
                     && (!((HSL_CPU_2 == mode) && (port_id == (pdev->nr_ports - 1)))))
                    SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                break;

            default:
                break;
            }
        }

        if (HSL_NO_CPU == mode) {
            SW_RTN_ON_ERROR(hsl_port_prop_set_phyid
                                (dev_id, port_id, port_id + 1));
        } else {
            if (port_id != pdev->cpu_port_nr) {
                SW_RTN_ON_ERROR(hsl_port_prop_set_phyid
                                (dev_id, port_id, port_id - 1));
            }
        }
    }

    return SW_OK;
}


sw_error_t
hsl_port_prop_get_phyid(a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * phy_id)
{
    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_validity_check(dev_id, port_id)) {
		return SW_BAD_PARAM;
	}

    *phy_id = p_port_info[dev_id]->phy_id[port_id];

    return SW_OK;
}


/*phy debug port1 register offset:29*/
/*phy debug port2 register offset:30*/

static a_uint16_t
_phy_reg_read(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint8_t reg)
{
    hsl_reg_func_t *reg_func;
    a_uint16_t val;

    if ((reg_func = hsl_reg_ptr_get(dev_id)) == NULL)
        return 0xFFFF;

    if (SW_OK != reg_func->phy_get(dev_id, phy_addr, reg, &val))
         return 0xFFFF;

     return val;
}


static void
_phy_reg_write(a_uint32_t dev_id, a_uint32_t phy_addr, a_uint8_t reg,
                a_uint16_t val)
{
    hsl_reg_func_t *reg_func;

    if ((reg_func = hsl_reg_ptr_get(dev_id)) == NULL)
        return;

    //reg_func->reg_set(dev_id, phy_addr, reg, val);
    reg_func->phy_set(dev_id, phy_addr, reg, val);
}


/******************************************************************************
*
* f1_get_autoneg_adv - get the phy autoneg Advertisement
*
*/
sw_error_t
f1_phy_get_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
                           a_uint32_t * autoneg)
{
    a_uint16_t phy_data = 0;

    *autoneg = 0;
    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_AUTONEG_ADVERT);

    if (phy_data & F1_ADVERTISE_100FULL)
        *autoneg |= FAL_PHY_ADV_100TX_FD;

    if (phy_data & F1_ADVERTISE_100HALF)
        *autoneg |= FAL_PHY_ADV_100TX_HD;

    if (phy_data & F1_ADVERTISE_10FULL)
        *autoneg |= FAL_PHY_ADV_10T_FD;

    if (phy_data & F1_ADVERTISE_10HALF)
        *autoneg |= FAL_PHY_ADV_10T_HD;

    if (phy_data & F1_ADVERTISE_PAUSE)
        *autoneg |= FAL_PHY_ADV_PAUSE;

    if (phy_data & F1_ADVERTISE_ASYM_PAUSE)
        *autoneg |= FAL_PHY_ADV_ASY_PAUSE;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_1000BASET_CONTROL);

    if (phy_data & F1_ADVERTISE_1000FULL)
        *autoneg |= FAL_PHY_ADV_1000T_FD;

    return SW_OK;
}



/******************************************************************************
*
* f1_phy_get_duplex - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/
sw_error_t
f1_phy_get_duplex(a_uint32_t dev_id, a_uint32_t phy_id,
                      fal_port_duplex_t * duplex)
{
    a_uint16_t phy_data;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_SPEC_STATUS);

    //read duplex
    if (phy_data & F1_STATUS_FULL_DUPLEX)
        *duplex = FAL_FULL_DUPLEX;
    else
        *duplex = FAL_HALF_DUPLEX;

    return SW_OK;
}


/******************************************************************************
*
* f1_set_autoneg_adv - set the phy autoneg Advertisement
*
*/
sw_error_t
f1_phy_set_autoneg_adv(a_uint32_t dev_id, a_uint32_t phy_id,
                           a_uint32_t autoneg)
{
    a_uint16_t phy_data = 0;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_AUTONEG_ADVERT);
    phy_data &= ~F1_ADVERTISE_ALL;
    phy_data &= ~(F1_ADVERTISE_PAUSE | F1_ADVERTISE_ASYM_PAUSE);

    if (autoneg & FAL_PHY_ADV_1000T_FD)
        phy_data |= F1_ADVERTISE_1000FULL;

    if (autoneg & FAL_PHY_ADV_100TX_FD)
        phy_data |= F1_ADVERTISE_100FULL;

    if (autoneg & FAL_PHY_ADV_100TX_HD)
        phy_data |= F1_ADVERTISE_100HALF;

    if (autoneg & FAL_PHY_ADV_10T_FD)
        phy_data |= F1_ADVERTISE_10FULL;

    if (autoneg & FAL_PHY_ADV_10T_HD)
        phy_data |= F1_ADVERTISE_10HALF;

    if (autoneg & FAL_PHY_ADV_PAUSE)
        phy_data |= F1_ADVERTISE_PAUSE;

    if (autoneg & FAL_PHY_ADV_ASY_PAUSE)
        phy_data |= F1_ADVERTISE_ASYM_PAUSE;

    f1_phy_reg_write(dev_id, phy_id, F1_AUTONEG_ADVERT, phy_data);

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_1000BASET_CONTROL);
    phy_data &= ~F1_ADVERTISE_1000FULL;
    phy_data &= ~F1_ADVERTISE_1000HALF;

    if (autoneg & FAL_PHY_ADV_1000T_FD)
        phy_data |= F1_ADVERTISE_1000FULL;

    f1_phy_reg_write(dev_id, phy_id, F1_1000BASET_CONTROL, phy_data);

    return SW_OK;
}


/******************************************************************************
*
* f1_restart_autoneg - restart the phy autoneg
*
*/
sw_error_t
f1_phy_restart_autoneg(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data = 0;

    phy_data = f1_phy_reg_read(dev_id, phy_id, F1_PHY_CONTROL);

    phy_data |= F1_CTRL_AUTONEGOTIATION_ENABLE;
    f1_phy_reg_write(dev_id, phy_id, F1_PHY_CONTROL,
                         phy_data | F1_CTRL_RESTART_AUTONEGOTIATION);

    return SW_OK;
}

/******************************************************************************
*
* f1_phy_set_speed - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*               AG7100_PHY_SPEED_10T, AG7100_PHY_SPEED_100TX;
*               AG7100_PHY_SPEED_1000T;
*/
sw_error_t
f1_phy_set_speed(a_uint32_t dev_id, a_uint32_t phy_id,
                     fal_port_speed_t speed)
{
    a_uint16_t phy_data = 0;
    a_uint32_t autoneg, oldneg;
    fal_port_duplex_t old_duplex;

    if (FAL_SPEED_1000 == speed) {
        phy_data |= F1_CTRL_SPEED_1000;
    } else if (FAL_SPEED_100 == speed) {
        phy_data |= F1_CTRL_SPEED_100;
    } else if (FAL_SPEED_10 == speed) {
        phy_data |= F1_CTRL_SPEED_10;
    } else {
        return SW_BAD_PARAM;
    }

    phy_data &= ~F1_CTRL_AUTONEGOTIATION_ENABLE;

    (void)f1_phy_get_autoneg_adv(dev_id, phy_id, &autoneg);
    oldneg = autoneg;
    autoneg &= ~FAL_PHY_ADV_GE_SPEED_ALL;

    (void)f1_phy_get_duplex(dev_id, phy_id, &old_duplex);

    if (old_duplex == FAL_FULL_DUPLEX) {
        phy_data |= F1_CTRL_FULL_DUPLEX;

        if (FAL_SPEED_1000 == speed) {
            autoneg |= FAL_PHY_ADV_1000T_FD;
        } else if (FAL_SPEED_100 == speed) {
            autoneg |= FAL_PHY_ADV_100TX_FD;
        } else {
            autoneg |= FAL_PHY_ADV_10T_FD;
        }
    } else if (old_duplex == FAL_HALF_DUPLEX) {
        phy_data &= ~F1_CTRL_FULL_DUPLEX;

        if (FAL_SPEED_100 == speed) {
            autoneg |= FAL_PHY_ADV_100TX_HD;
        } else {
            autoneg |= FAL_PHY_ADV_10T_HD;
        }
    } else {
        return SW_FAIL;
    }

    (void)f1_phy_set_autoneg_adv(dev_id, phy_id, autoneg);
    (void)f1_phy_restart_autoneg(dev_id, phy_id);
//    aos_mdelay(2000);
  
    f1_phy_reg_write(dev_id, phy_id, F1_PHY_CONTROL, phy_data);
    (void)f1_phy_set_autoneg_adv(dev_id, phy_id, oldneg);

    return SW_OK;

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
garuda_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FLOOD_MASK, 0, UNI_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable) {
        data &= (~((a_uint32_t)0x1 << port_id)); 
    } else if (A_FALSE == enable) {
        data |= (0x1 << port_id);
    } else {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FLOOD_MASK, 0, UNI_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}


/**
 * @brief Set speed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] speed port speed
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
                      fal_port_speed_t speed)
{
    sw_error_t rv;
    a_uint32_t phy_id = 0;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY)) {
        return SW_BAD_PARAM;
    }

    rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
    SW_RTN_ON_ERROR(rv);

    if (FAL_SPEED_1000 < speed) {
        return SW_BAD_PARAM;
    }

    rv = f1_phy_set_speed(dev_id, phy_id, speed);

    return rv;
}



/**
 * @brief Set status of Atheros header packets parsed on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_port_hdr_status_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, HEAD_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));

    return rv;
}

sw_error_t
garuda_port_ctrl_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

//        p_api->port_duplex_get = garuda_port_duplex_get;
//        p_api->port_duplex_set = garuda_port_duplex_set;
//        p_api->port_speed_get = garuda_port_speed_get;
        p_api->port_speed_set = garuda_port_speed_set;
        p_api->port_hdr_status_set = garuda_port_hdr_status_set;
//        p_api->port_hdr_status_get = garuda_port_hdr_status_get;
    }
#endif

    return SW_OK;
}


/**
 * @brief Set dynamic address learning status on a particular port.
 *    @details  Comments:
 *       This operation will enable or disable dynamic address learning
 *       feature on a particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable enable or disable
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_fdb_port_learn_set(a_uint32_t dev_id, fal_port_t port_id,
                          a_bool_t enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    if (A_TRUE == enable) {
        data = 1;
    } else if (A_FALSE == enable) {
        data = 0;
    } else {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, LEARN_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}



sw_error_t
garuda_fdb_init(a_uint32_t dev_id)
{
    HSL_DEV_ID_CHECK(dev_id);

#ifndef HSL_STANDALONG
    {
        hsl_api_t *p_api;

        SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

        p_api->fdb_add = garuda_fdb_add;
//        p_api->fdb_del_all = garuda_fdb_del_all;
//        p_api->fdb_del_by_port = garuda_fdb_del_by_port;
//        p_api->fdb_del_by_mac = garuda_fdb_del_by_mac;
//        p_api->fdb_first = garuda_fdb_first;
//        p_api->fdb_next = garuda_fdb_next;
//        p_api->fdb_find = garuda_fdb_find;
        p_api->port_learn_set = garuda_fdb_port_learn_set;
//        p_api->port_learn_get = garuda_fdb_port_learn_get;
//        p_api->age_ctrl_set = garuda_fdb_age_ctrl_set;
//        p_api->age_ctrl_get = garuda_fdb_age_ctrl_get;
//        p_api->age_time_set = garuda_fdb_age_time_set;
//        p_api->age_time_get = garuda_fdb_age_time_get;
    }
#endif

    return SW_OK;
}


hsl_api_t *
hsl_api_ptr_get(a_uint32_t dev_id)
{
    if (dev_id >= SW_MAX_NR_DEV)
        return NULL;

    return &(dev_table[dev_id].dev_api);
}


static sw_error_t
phy_dport_set(a_uint32_t dev_id, a_uint16_t phy_id, a_uint16_t dport_addr, a_uint16_t val_mask)
{
    hsl_dev_t    *pdev = NULL;
    a_uint16_t phy_data;
    
    pdev = hsl_dev_ptr_get(dev_id);
    if (NULL == pdev) {
        return SW_NOT_INITIALIZED;
    }

    pdev->dev_func.phy_set(dev_id, phy_id, F1_DEBUG_PORT_ADDRESS, dport_addr);
    pdev->dev_func.phy_get(dev_id, phy_id, F1_DEBUG_PORT_DATA, &phy_data);
    phy_data |= val_mask;
    pdev->dev_func.phy_set(dev_id, phy_id, F1_DEBUG_PORT_DATA, phy_data);

	return SW_OK;
}


static sw_error_t
phy_dport_clear(a_uint32_t dev_id, a_uint16_t phy_id, a_uint16_t dport_addr, a_uint16_t val_mask)
{
    hsl_dev_t    *pdev = NULL;
    a_uint16_t phy_data;
    
    pdev = hsl_dev_ptr_get(dev_id);
    if (NULL == pdev) {
        return SW_NOT_INITIALIZED;
    }

    pdev->dev_func.phy_set(dev_id, phy_id, F1_DEBUG_PORT_ADDRESS, dport_addr);
    pdev->dev_func.phy_get(dev_id, phy_id, F1_DEBUG_PORT_DATA, &phy_data);
    phy_data &= ~val_mask;
    pdev->dev_func.phy_set(dev_id, phy_id, F1_DEBUG_PORT_DATA, phy_data);

	return SW_OK;
}

static sw_error_t
garuda_hw_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    hsl_dev_t    *pdev = NULL;
    hsl_init_mode cpu_mode;
    a_uint32_t port_id;
    a_uint32_t data;
//    a_uint16_t phy_data;
    sw_error_t rv;

    pdev = hsl_dev_ptr_get(dev_id);
    if (NULL == pdev) {
        return SW_NOT_INITIALIZED;
    }
    cpu_mode = cfg->cpu_mode;

    HSL_REG_ENTRY_GET(rv, dev_id, POSTRIP, 0,
                  (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    /* phy pll on */
    SW_SET_REG_BY_FIELD(POSTRIP, PHY_PLL_ON, 1, data);

    /* delay */
    if (A_TRUE == cfg->rx_delay_s1) {
        SW_SET_REG_BY_FIELD(POSTRIP, RXDELAY_S1, 1, data);
    } else {
        SW_SET_REG_BY_FIELD(POSTRIP, RXDELAY_S1, 0, data);
    }

    if (A_TRUE == cfg->rx_delay_s0) {
        SW_SET_REG_BY_FIELD(POSTRIP, RXDELAY_S0, 1, data);
    } else {
        SW_SET_REG_BY_FIELD(POSTRIP, RXDELAY_S0, 0, data);
    }

    if (A_TRUE ==  cfg->tx_delay_s1) {
        SW_SET_REG_BY_FIELD(POSTRIP, TXDELAY_S1, 1, data);
    } else {
        SW_SET_REG_BY_FIELD(POSTRIP, TXDELAY_S1, 0, data);
    }
    
    if (A_TRUE ==  cfg->tx_delay_s0) {
        SW_SET_REG_BY_FIELD(POSTRIP, TXDELAY_S0, 1, data);
    } else {
        SW_SET_REG_BY_FIELD(POSTRIP, TXDELAY_S0, 0, data);
    }

    /* tx/rx delay enable */
    if (A_TRUE ==  cfg->rgmii_txclk_delay) {
        SW_SET_REG_BY_FIELD(POSTRIP, RGMII_TXCLK_DELAY_EN, 1, data);
    } else {
        SW_SET_REG_BY_FIELD(POSTRIP, RGMII_TXCLK_DELAY_EN, 0, data);
    }

    /* tx/rx delay enable */
    if (A_TRUE ==  cfg->rgmii_rxclk_delay) {
        SW_SET_REG_BY_FIELD(POSTRIP, RGMII_RXCLK_DELAY_EN, 1, data);
    } else {
        SW_SET_REG_BY_FIELD(POSTRIP, RGMII_RXCLK_DELAY_EN, 0, data);
    }

    /* mac0 default phy mode */
    SW_SET_REG_BY_FIELD(POSTRIP, MAC0_MAC_MODE, 0, data);

    /* mac0 default rgmii mode */
    SW_SET_REG_BY_FIELD(POSTRIP, MAC0_RGMII_EN, 0, data);
    SW_SET_REG_BY_FIELD(POSTRIP, MAC0_GMII_EN, 1, data);

    /* mac5 default mode */
    SW_SET_REG_BY_FIELD(POSTRIP, MAC5_PHY_MODE, 1, data);
    SW_SET_REG_BY_FIELD(POSTRIP, MAC5_MAC_MODE, 0, data);
    
    /* phy default mode */
    SW_SET_REG_BY_FIELD(POSTRIP, PHY4_RGMII_EN, 0, data);
    SW_SET_REG_BY_FIELD(POSTRIP, PHY4_GMII_EN, 1, data);

    /* modify default mode */
    if (A_FALSE == cfg->mac0_rgmii) {
        SW_SET_REG_BY_FIELD(POSTRIP, MAC0_RGMII_EN, 0, data);
        SW_SET_REG_BY_FIELD(POSTRIP, MAC0_GMII_EN,  1, data);
        
        /*invert clock output for port0 gmii pad.*/
        a_uint32_t temp;
        HSL_REG_ENTRY_GET(rv, dev_id, MASK_CTL, 0,
                  (a_uint8_t *) (&temp), sizeof (a_uint32_t));
        temp |= 1<<MASK_CTL_MII_CLK0_SEL_BOFFSET;
        HSL_REG_ENTRY_SET(rv, dev_id, MASK_CTL, 0,
                  (a_uint8_t *) (&temp), sizeof (a_uint32_t));
    }

    if(HSL_CPU_2 == cpu_mode) {
        if (A_TRUE == cfg->mac5_rgmii) {

            SW_SET_REG_BY_FIELD(POSTRIP, PHY4_RGMII_EN, 1, data);
            SW_SET_REG_BY_FIELD(POSTRIP, PHY4_GMII_EN, 0, data);

             a_uint16_t phy_id = 4;
            /* phy4 rgmii mode enable */
            phy_dport_set(dev_id, phy_id, F1_DEBUG_PORT_RGMII_MODE, F1_DEBUG_PORT_RGMII_MODE_EN);

            /* Rx delay enable */
            if (A_TRUE ==  cfg->phy4_rx_delay) {
                phy_dport_set(dev_id, phy_id, F1_DEBUG_PORT_RX_DELAY, F1_DEBUG_PORT_RX_DELAY_EN);
            } else {
                phy_dport_clear(dev_id, phy_id, F1_DEBUG_PORT_RX_DELAY, F1_DEBUG_PORT_RX_DELAY_EN);
            }
            
            /* Tx delay enable */
            if (A_TRUE ==  cfg->phy4_tx_delay) {
                phy_dport_set(dev_id, phy_id, F1_DEBUG_PORT_TX_DELAY, F1_DEBUG_PORT_TX_DELAY_EN);
            } else {
                phy_dport_clear(dev_id, phy_id, F1_DEBUG_PORT_TX_DELAY, F1_DEBUG_PORT_TX_DELAY_EN);
            }
            
        } else {
            SW_SET_REG_BY_FIELD(POSTRIP, PHY4_RGMII_EN, 0, data);
            SW_SET_REG_BY_FIELD(POSTRIP, PHY4_GMII_EN, 1, data);
        }
    } else if (HSL_CPU_1 == cpu_mode) {
        //SW_SET_REG_BY_FIELD(POSTRIP, TXDELAY_S0, 0, data);

    } else if (HSL_CPU_1_PLUS == cpu_mode) {
        SW_SET_REG_BY_FIELD(POSTRIP, MAC5_MAC_MODE, 1, data);

    } else if (HSL_NO_CPU == cpu_mode) {

    }

    HSL_REG_ENTRY_SET(rv, dev_id, POSTRIP, 0,
                  (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    for (port_id = 0; port_id < pdev->nr_ports; port_id++) {
        if (port_id == pdev->cpu_port_nr) {
            continue;
        }

        HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, port_id,
              (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        SW_SET_REG_BY_FIELD(PORT_STATUS, LINK_EN, 1, data);

        HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, port_id,
                  (a_uint8_t *) (&data), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

	/* configure cpu port mac */
	HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, 0,
		  (a_uint8_t *) (&data), sizeof (a_uint32_t));
	SW_SET_REG_BY_FIELD(PORT_STATUS, LINK_EN, 0, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, RXMAC_EN, 1, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, TXMAC_EN, 1, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, SPEED_MODE, 1, data);
	
	HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, 0,
			  (a_uint8_t *) (&data), sizeof (a_uint32_t));

    return SW_OK;
}



static sw_error_t
garuda_bist_test(a_uint32_t dev_id)
{
    a_uint32_t entry, data, i;
    sw_error_t rv;

    data = 1;
    i = 0x1000;
    while (data && --i) {
        HSL_REG_ENTRY_GET(rv, dev_id, BIST_CTRL, 0,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(BIST_CTRL, BIST_BUSY, data, entry);
        aos_udelay(5);
    }

    if (0 == i) {
        return SW_INIT_ERROR;
    }

    entry = 0;
    SW_SET_REG_BY_FIELD(BIST_CTRL, BIST_BUSY, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN2, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN1, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN0, 1, entry);

    HSL_REG_ENTRY_SET(rv, dev_id, BIST_CTRL, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data = 1;
    i = 0x1000;
    while (data && --i) {
        HSL_REG_ENTRY_GET(rv, dev_id, BIST_CTRL, 0,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(BIST_CTRL, BIST_BUSY, data, entry);
        aos_udelay(5);
    }

    if (0 == i) {
        return SW_INIT_ERROR;
    }

    SW_GET_FIELD_BY_REG(BIST_CTRL, ERR_CNT, data, entry);
    if (data) {
        SW_GET_FIELD_BY_REG(BIST_CTRL, ONE_ERR, data, entry);
        if (!data) {
            return SW_INIT_ERROR;
        }

        SW_GET_FIELD_BY_REG(BIST_CTRL, ERR_ADDR, data, entry);

        entry = 0;
        SW_SET_REG_BY_FIELD(BIST_RCV, RCV_EN, 1, entry);
        SW_SET_REG_BY_FIELD(BIST_RCV, RCV_ADDR, data, entry);
        HSL_REG_ENTRY_SET(rv, dev_id, BIST_RCV, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    } else {
        return SW_OK;
    }

    entry = 0;
    SW_SET_REG_BY_FIELD(BIST_CTRL, BIST_BUSY, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN2, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN1, 1, entry);
    SW_SET_REG_BY_FIELD(BIST_CTRL, PTN_EN0, 1, entry);

    HSL_REG_ENTRY_SET(rv, dev_id, BIST_CTRL, 0,
                      (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    data = 1;
    i = 0x1000;
    while (data && --i) {
        HSL_REG_ENTRY_GET(rv, dev_id, BIST_CTRL, 0,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
        SW_GET_FIELD_BY_REG(BIST_CTRL, BIST_BUSY, data, entry);
        aos_udelay(5);
    }

    if (0 == i) {
        return SW_INIT_ERROR;
    }

    SW_GET_FIELD_BY_REG(BIST_CTRL, ERR_CNT, data, entry);
    if (data) {
        return SW_INIT_ERROR;
    }

    return SW_OK;
}


/**
 * @brief Init hsl layer.
 * @details Comments:
 *   This operation will init hsl layer and hsl layer
 * @param[in] dev_id device id
 * @param[in] cfg configuration for initialization
 * @return SW_OK or error code
 */
sw_error_t
garuda_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    hsl_api_t *p_api;
    a_uint32_t i, entry;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (NULL == garuda_cfg[dev_id]) {
        garuda_cfg[dev_id] = malloc(sizeof (ssdk_init_cfg));
    }

    if (NULL == garuda_cfg[dev_id]) {
        return SW_OUT_OF_MEM;
    }

    memcpy(garuda_cfg[dev_id], cfg, sizeof (ssdk_init_cfg));

    SW_RTN_ON_ERROR(garuda_reg_access_mode_init(dev_id, cfg->reg_mode));

    SW_RTN_ON_ERROR(garuda_dev_init(dev_id, cfg->cpu_mode));

	/*reset Atheros 8306 before init */
	SW_RTN_ON_ERROR(garuda_bist_test(dev_id));

	entry = 0x1;
	HSL_REG_FIELD_SET(rv, dev_id, MASK_CTL, 0, SOFT_RST,
						  (a_uint8_t *) (&entry), sizeof (a_uint32_t));
	SW_RTN_ON_ERROR(rv);

	i = 0x10;
	do {
		HSL_REG_FIELD_GET(rv, dev_id, MASK_CTL, 0, SOFT_RST,
						  (a_uint8_t *) (&entry), sizeof (a_uint32_t));
		SW_RTN_ON_ERROR(rv);

        udelay (10000);
	} while (entry && --i);

	if (0 == i) {
		return SW_INIT_ERROR;
	}
    
#if !(defined(KERNEL_MODULE) && defined(USER_MODE))    
    SW_RTN_ON_ERROR(hsl_port_prop_init());
    SW_RTN_ON_ERROR(hsl_port_prop_init_by_dev(dev_id));
    SW_RTN_ON_ERROR(garuda_portproperty_init(dev_id, cfg->cpu_mode));

    SW_RTN_ON_ERROR(garuda_port_ctrl_init(dev_id));
    SW_RTN_ON_ERROR(garuda_fdb_init(dev_id));
//    SW_RTN_ON_ERROR(garuda_acl_init(dev_id));

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
//    p_api->dev_reset   = garuda_reset;
//    p_api->dev_version = garuda_version;

    SW_RTN_ON_ERROR(garuda_hw_init(dev_id, cfg));
#endif

    return SW_OK;
}

/**
 * @brief Get mib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                    fal_mib_info_t * mib_info)
{
    a_uint32_t val;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_OUT_OF_RANGE;
    }

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBROAD, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBroad = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXPAUSE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxPause = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXMULTI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxMulti = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXFCSERR, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxFcsErr = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXALLIGNERR, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxAllignErr = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXRUNT, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxRunt = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXFRAGMENT, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxFragment = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX64BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx64Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX128BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx128Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX256BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx256Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX512BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx512Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX1024BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx1024Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RX1518BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Rx1518Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXMAXBYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxMaxByte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXTOOLONG, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxTooLong = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXGOODBYTE_LO, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);    
    mib_info->RxGoodByte_lo = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXGOODBYTE_HI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxGoodByte_hi = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBADBYTE_LO, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBadByte_lo = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXBADBYTE_HI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxBadByte_hi = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_RXOVERFLOW, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->RxOverFlow = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_FILTERED, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Filtered = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBROAD, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxBroad = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXPAUSE, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxPause = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMULTI, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMulti = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXUNDERRUN, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxUnderRun = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX64BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);    
    mib_info->Tx64Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX128BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx128Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX256BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx256Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX512BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);    
    mib_info->Tx512Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX1024BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->Tx1024Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TX1518BYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);    
    mib_info->Tx1518Byte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMAXBYTE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMaxByte = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXOVERSIZE, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxOverSize = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBYTE_LO, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxByte_lo = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXBYTE_HI, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxByte_hi = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXCOLLISION, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxCollision = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXABORTCOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxAbortCol = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXMULTICOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxMultiCol = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXSINGALCOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxSingalCol = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXEXCDEFER, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxExcDefer = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXDEFER, port_id, (a_uint8_t *) (&val),
                      sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxDefer = val;

    HSL_REG_ENTRY_GET(rv, dev_id, MIB_TXLATECOL, port_id,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);
    mib_info->TxLateCol = val;

    return SW_OK;
}


/**
 * @brief Set mib status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
garuda_mib_status_set(a_uint32_t dev_id, a_bool_t enable)
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

    HSL_REG_FIELD_SET(rv, dev_id, MIB_FUNC, 0, MIB_EN,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}


sw_error_t
hsl_dev_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    sw_error_t rv;

    if (SW_MAX_NR_DEV <= dev_id) {
        return SW_BAD_PARAM;
    }

    memset(&dev_table[dev_id], 0, sizeof (hsl_dev_t));

    memcpy(api_ifname, cfg->ioctl_devname, 16);

    rv = garuda_init(dev_id, cfg);

    return rv;
}


sw_error_t
ssdk_init(a_uint32_t dev_id, ssdk_init_cfg * cfg)
{
    sw_error_t rv;

    rv = hsl_dev_init(dev_id, cfg);

    return rv;
}

sw_error_t atheros_wr_reg(a_uint32_t reg_addr, a_uint32_t value)
{	
	garuda_mdio_reg_set(0, reg_addr, (a_uint8_t *) &value, sizeof(a_uint32_t));
}

sw_error_t atheros_show_reg(a_uint32_t reg_addr)
{
	a_uint32_t value;
	
	garuda_mdio_reg_get(0, reg_addr, (a_uint8_t *) &value, sizeof(a_uint32_t));

	printf("atheros reg addr = 0x%X, value = 0x%X\n",
		reg_addr, value);
}

sw_error_t atheros_show_cnt(fal_port_t port_id)
{
	fal_mib_info_t mib_info;
	
	garuda_get_mib_info(0, port_id, &mib_info);

	printf("Counter info for port %d\n", port_id);
	printf("==================================================\n");
	printf("RxBroad     <0x%X>  RxPause     <0x%X>  RxMulti     <0x%X>  \n",
		mib_info.RxBroad, mib_info.RxPause, mib_info.RxMulti);
	printf("RxFcsErr    <0x%X>  RxAllignErr <0x%X>  RxRunt      <0x%X>\n",
		mib_info.RxFcsErr, mib_info.RxAllignErr, mib_info.RxRunt);
	printf("RxFragment  <0x%X>  Rx64Byte    <0x%X>  Rx128Byte   <0x%X>  \n",
		mib_info.RxFragment, mib_info.Rx64Byte, mib_info.Rx128Byte);
	printf("Rx256Byte   <0x%X>  Rx512Byte   <0x%X>  Rx1024Byte  <0x%X>  \n",
		mib_info.Rx256Byte, mib_info.Rx64Byte, mib_info.Rx1024Byte);
	printf("Rx1518Byte  <0x%X>  RxMaxByte   <0x%X>  RxTooLong   <0x%X>  \n",
		mib_info.Rx1518Byte, mib_info.RxMaxByte, mib_info.RxTooLong);
	printf("RxGoodByte  <0x%X>  RxGoodByte1 <0x%X>  RxBadByte   <0x%X>  \n",
		mib_info.RxGoodByte_hi, mib_info.RxGoodByte_lo,	mib_info.RxBadByte_hi);
	printf("RxBadByte1  <0x%X>  RxOverFlow  <0x%X>  Filtered    <0x%X>  \n",
		mib_info.RxBadByte_lo, mib_info.RxOverFlow, mib_info.Filtered);
	printf("TxBroad     <0x%X>  TxPause     <0x%X>  TxMulti     <0x%X>  \n",
		mib_info.TxBroad, mib_info.TxPause, mib_info.TxMulti);
	printf("TxUnderRun  <0x%X>  Tx64Byte    <0x%X>  Tx128Byte   <0x%X>  \n",
		mib_info.TxUnderRun, mib_info.Tx64Byte, mib_info.Tx128Byte);
	printf("Tx256Byte   <0x%X>  Tx512Byte   <0x%X>  Tx1024Byte  <0x%X>  \n",
		mib_info.Tx256Byte, mib_info.Tx512Byte, mib_info.Tx1024Byte);
	printf("Tx1518Byte  <0x%X>  TxMaxByte   <0x%X>  TxOverSize  <0x%X>  \n",
		mib_info.Tx1518Byte, mib_info.TxMaxByte, mib_info.TxOverSize);
	printf("TxByte      <0x%X>  TxByte1     <0x%X>  TxCollision <0x%X>  \n",
		mib_info.TxByte_hi, mib_info.TxByte_lo, mib_info.TxCollision);
	printf("TxAbortCol  <0x%X>  TxMultiCol  <0x%X>  TxSingalCol <0x%X>  \n",
		mib_info.TxAbortCol, mib_info.TxMultiCol, mib_info.TxSingalCol);
	printf("TxExcDefer  <0x%X>  TxDefer     <0x%X>  TxLateCol   <0x%X>  \n",
		mib_info.TxExcDefer, mib_info.TxDefer, mib_info.TxLateCol);
}

sw_error_t atheros_init()
{
	OPL_STATUS retVal = OPL_OK;
	a_bool_t	enable = 0;
    ssdk_init_cfg     cfg = def_init_cfg_cpu1;
	UINT32 ulData;
    sw_error_t rv;

	oplRegFieldRead(0, 4, 16, &ulData);
	printf("product id = 0x%x\n", ulData);

	/* register mdio APIs */
	cfg.reg_func.mdio_set = ssdk_mdio_set;
	cfg.reg_func.mdio_get = ssdk_mdio_get;
	cfg.reg_mode = HSL_MDIO;
    retVal = ssdk_init(0, &cfg);	
    if (SW_OK != retVal)
    {
        printf("ssdk_init error: %d\n",retVal);
        return retVal;
    }

    HSL_REG_FIELD_GET(rv, 0, MASK_CTL, 0, DEVICE_ID,
                      (a_uint8_t *) (&ulData), sizeof (a_uint32_t));
	printf("Atheros Device ID: %d\n", ulData);
	
    HSL_REG_FIELD_GET(rv, 0, MASK_CTL, 0, REV_ID,
                      (a_uint8_t *) (&ulData), sizeof (a_uint32_t));
	printf("Atheros Revision ID: %d\n", ulData);

	#ifdef CONFIG_IPMUX_ATHEROS_PKT_HEADTAG
	/* disable CPU header function */
	garuda_port_hdr_status_set(0, 0, 1);
	#else
	/* enable CPU header function */
	garuda_port_hdr_status_set(0, 0, 0);
	#endif

	retVal = garuda_cpu_port_status_set(0, 1);
    if (SW_OK != retVal)
    {
        printf("garuda_cpu_port_status_set2 error: %d\n",retVal);
        return retVal;
    }

	retVal = garuda_bc_to_cpu_port_set(0, 1);
    if (SW_OK != retVal)
    {
        printf("garuda_bc_to_cpu_port_set error: %d\n",retVal);
        return retVal;
    }

	retVal = garuda_port_unk_uc_filter_set(0, 0, 0);
    if (SW_OK != retVal)
    {
        printf("garuda_port_unk_uc_filter_set error: %d\n",retVal);
        return retVal;
    }

	garuda_mib_status_set(0, 1);

	

	return retVal;
}

