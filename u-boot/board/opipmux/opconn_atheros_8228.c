/* Atheros Driver Porting */

#include "stddef.h"
#include "opconn_atheros_8228.h"
#include "exports.h"
#include "extra.h"

static hsl_dev_t dev_table[SW_MAX_NR_DEV];
static port_info_t *p_port_info[SW_MAX_NR_DEV] = { 0 };
a_uint8_t api_ifname[16] = { 0 };
static ssdk_init_cfg * shiva_cfg[SW_MAX_NR_DEV] = { 0 };

extern OPL_STATUS mdioRegisterWrite(UINT32 deviceAddr,UINT32 regAddr,UINT16 data0);
extern OPL_STATUS mdioRegisterRead(UINT32 deviceAddr,UINT32 regAddr,UINT16 * data0);

mdio_reg_set ssdk_mdio_set    = mdioRegisterWrite;
mdio_reg_get ssdk_mdio_get    = mdioRegisterRead;


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
sd_mdio_set_is_inited(void)
{
    if (ssdk_mdio_set) {
        return A_TRUE;
    } else {
        return A_FALSE;
    }
}

a_bool_t
sd_mdio_get_is_inited(void)
{
    if (ssdk_mdio_get) {
        return A_TRUE;
    } else {
        return A_FALSE;
    }
}


static sw_error_t
_shiva_mdio_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
                    a_uint8_t value_len)
{
    a_uint32_t reg_word_addr;
    a_uint32_t phy_addr, reg_val;
    a_uint16_t phy_val;
    a_uint8_t phy_reg;
    sw_error_t rv;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    memcpy(&reg_val, value, sizeof (a_uint32_t));

    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (a_uint16_t) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */

    rv = shiva_phy_reg_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);

    /* For some registers such as ARL and VLAN, since they include BUSY bit */
    /* in lower address, we should write the higher 16-bit register then the */
    /* lower one */

    /* write register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (a_uint16_t) ((reg_val >> 16) & 0xffff);
    rv = shiva_phy_reg_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);

    /* write register in lower address */
    reg_word_addr--;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    phy_val = (a_uint16_t) (reg_val & 0xffff);
    rv = shiva_phy_reg_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}


static sw_error_t
_shiva_mdio_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                    a_uint8_t value[], a_uint8_t value_len)
{
    a_uint32_t reg_word_addr;
    a_uint32_t phy_addr, reg_val;
    a_uint16_t phy_val, tmp_val;
    a_uint8_t phy_reg;
    sw_error_t rv;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    /* change reg_addr to 16-bit word address, 32-bit aligned */
    reg_word_addr = (reg_addr & 0xfffffffc) >> 1;

    /* configure register high address */
    phy_addr = 0x18;
    phy_reg = 0x0;
    phy_val = (a_uint16_t) ((reg_word_addr >> 8) & 0x3ff);  /* bit16-8 of reg address */

    rv = shiva_phy_reg_set(dev_id, phy_addr, phy_reg, phy_val);
    SW_RTN_ON_ERROR(rv);

    /* For some registers such as MIBs, since it is read/clear, we should */
    /* read the lower 16-bit register then the higher one */

    /* read register in lower address */
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    rv = shiva_phy_reg_get(dev_id, phy_addr, phy_reg, &tmp_val);
    SW_RTN_ON_ERROR(rv);
    reg_val = tmp_val;

    /* read register in higher address */
    reg_word_addr++;
    phy_addr = 0x10 | ((reg_word_addr >> 5) & 0x7); /* bit7-5 of reg address */
    phy_reg = (a_uint8_t) (reg_word_addr & 0x1f);   /* bit4-0 of reg address */
    rv = shiva_phy_reg_get(dev_id, phy_addr, phy_reg, &tmp_val);
    SW_RTN_ON_ERROR(rv);
    reg_val |= (((a_uint32_t)tmp_val) << 16);

    memcpy(value, &reg_val, sizeof (a_uint32_t));

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
shiva_phy_reg_get(a_uint32_t dev_id, a_uint32_t phy_addr,
                   a_uint8_t reg, a_uint16_t * value)
{
    if (A_TRUE == sd_mdio_get_is_inited()) {
//        aos_lock_bh(&mdio_lock);
		sd_reg_mdio_get(phy_addr, reg, value);
//        aos_unlock_bh(&mdio_lock);
    } else {
            return SW_NOT_SUPPORTED;
    }    
    return SW_OK;
}

static sw_error_t
shiva_phy_reg_set(a_uint32_t dev_id, a_uint32_t phy_addr,
                   a_uint8_t reg, a_uint16_t value)
{
    if (A_TRUE == sd_mdio_set_is_inited()) {
//        aos_lock_bh(&mdio_lock);
		sd_reg_mdio_set(phy_addr, reg, value);
//        aos_unlock_bh(&mdio_lock);
    } else {
        return SW_NOT_SUPPORTED;
    }
    return SW_OK;
}

static sw_error_t
shiva_mdio_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr,
                    a_uint8_t value[], a_uint8_t value_len)
{
    sw_error_t rv;

//    aos_lock_bh(&mdio_lock);
    rv = _shiva_mdio_reg_get(dev_id, reg_addr, value, value_len);    
//    aos_unlock_bh(&mdio_lock);

    return rv;
}

static sw_error_t
shiva_mdio_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
                    a_uint8_t value_len)
{
    sw_error_t rv;

//    aos_lock_bh(&mdio_lock);
    rv = _shiva_mdio_reg_set(dev_id, reg_addr, value, value_len);    
//    aos_unlock_bh(&mdio_lock);

    return rv;
}

hsl_reg_func_t *
hsl_reg_ptr_get(a_uint32_t dev_id)
{
    if (dev_id >= SW_MAX_NR_DEV)
        return NULL;

    return &(dev_table[dev_id].dev_func);
}


hsl_dev_t *
hsl_dev_ptr_get(a_uint32_t dev_id)
{
    if (dev_id >= SW_MAX_NR_DEV)
        return NULL;

    return &dev_table[dev_id];
}

sw_error_t
isis_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
                      a_uint32_t value_len)
{
    hsl_reg_func_t *reg_func;
    a_uint32_t reg_val;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    if ((reg_func = hsl_reg_ptr_get(dev_id)) == NULL)
        return SW_NOT_INITIALIZED;

    reg_val = *((a_uint32_t *) value);
    SW_RTN_ON_ERROR(reg_func->
                    reg_set(dev_id, reg_addr,
                            (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));
    return SW_OK;
}

sw_error_t
isis_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[],
                      a_uint32_t value_len)
{
    hsl_reg_func_t *reg_func;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    if ((reg_func = hsl_reg_ptr_get(dev_id)) == NULL)
        return SW_NOT_INITIALIZED;

    SW_RTN_ON_ERROR(reg_func->reg_get
                    (dev_id, reg_addr, value,
                     sizeof (a_uint32_t)));

    return SW_OK;
}

static sw_error_t
shiva_reg_entry_get(a_uint32_t dev_id, a_uint32_t reg_addr,
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
shiva_reg_entry_set(a_uint32_t dev_id, a_uint32_t reg_addr,
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
shiva_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr,
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
shiva_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr,
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
shiva_reg_access_mode_init(a_uint32_t dev_id, hsl_access_mode mode)
{
    hsl_reg_func_t *reg_func;
    hsl_dev_t *dev;

    if ((dev = hsl_dev_ptr_get(dev_id)) == NULL)
        return SW_NOT_INITIALIZED;

    if ((reg_func = hsl_reg_ptr_get(dev_id)) == NULL)
        return SW_NOT_INITIALIZED;

    if (mode == reg_func->reg_access_mode)
        return SW_OK;

	/* for Atheros 8228 ONU */
	reg_func->phy_get = shiva_phy_reg_get;
	reg_func->phy_set = shiva_phy_reg_set;

	if (mode == HSL_MDIO) {
		reg_func->reg_get = shiva_mdio_reg_get;
		reg_func->reg_set = shiva_mdio_reg_set;
	}

    reg_func->reg_entry_get = shiva_reg_entry_get;
    reg_func->reg_entry_set = shiva_reg_entry_set;
    reg_func->reg_field_get = shiva_reg_field_get;
    reg_func->reg_field_set = shiva_reg_field_set;

    return SW_OK;
}
/******************************************************************************
*
* f2_phy_poweroff - power off the phy 
*
* power the phy
*/
sw_error_t
f2_phy_poweroff(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    shiva_phy_reg_set(dev_id, phy_id, 0x1d, 0x3);
    shiva_phy_reg_set(dev_id, phy_id, 0x1e, 0x3eff);

	phy_data = 0x0800;
    shiva_phy_reg_set(dev_id, phy_id, 0, phy_data);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_poweron- power on the phy 
*
* power the phy
*/
sw_error_t
f2_phy_poweron(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

	phy_data = 0x1200;
    shiva_phy_reg_set(dev_id, phy_id, 0, phy_data);

    return SW_OK;
}

/******************************************************************************
*
* f2_phy_reset - reset the phy 
*
* reset the phy
*/
sw_error_t
f2_phy_reset(a_uint32_t dev_id, a_uint32_t phy_id)
{
    a_uint16_t phy_data;

    shiva_phy_reg_get(dev_id, phy_id, 0, &phy_data);
    shiva_phy_reg_set(dev_id, phy_id, 0,
                         phy_data | 0x8000);

    return SW_OK;
}


static sw_error_t
shiva_dev_init(a_uint32_t dev_id, hsl_init_mode cpu_mode)
{
    hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

    pdev->nr_ports = 7;
    pdev->nr_phy = 6;
    pdev->cpu_port_nr = 0;
    pdev->nr_vlans = 4096;
    pdev->nr_queue = 4;
    pdev->cpu_mode = cpu_mode;

    return SW_OK;
}


static sw_error_t
shiva_hw_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    hsl_dev_t *pdev = NULL;
    a_uint32_t port_id;
    a_uint32_t data;
    sw_error_t rv;

    pdev = hsl_dev_ptr_get(dev_id);
    if (NULL == pdev) {
        return SW_NOT_INITIALIZED;
    }

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

#if defined(CONFIG_ONU_1_1_RF400)
       /* mac0 GMII PHY mode */
	data = 0;
	SW_SET_REG_BY_FIELD(OPEMODE0, MAC0_PHY_GMII_EN, 1, data);
	//SW_SET_REG_BY_FIELD(OPEMODE0, MAC0_PHY_GMII_TXCLK_SEL, 1, data);
	//SW_SET_REG_BY_FIELD(OPEMODE0, MAC0_PHY_GMII_RXCLK_SEL, 1, data);
	HSL_REG_ENTRY_SET(rv, dev_id, OPEMODE0, 0,
			  (a_uint8_t *) (&data), sizeof (a_uint32_t));

	/* configure mac0 port mac */
	HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, 0,
		  (a_uint8_t *) (&data), sizeof (a_uint32_t));
	SW_SET_REG_BY_FIELD(PORT_STATUS, LINK_EN, 0, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, TX_HALF_FLOW_EN, 0, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, DUPLEX_MODE, 1, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, RXMAC_EN, 1, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, TXMAC_EN, 1, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, SPEED_MODE, 2, data);
	
	HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, 0,
			  (a_uint8_t *) (&data), sizeof (a_uint32_t));

#else
    /* mac0 default phy mode */
	HSL_REG_ENTRY_GET(rv, dev_id, OPEMODE0, 0,
		  (a_uint8_t *) (&data), sizeof (a_uint32_t));
	SW_SET_REG_BY_FIELD(OPEMODE0, MAC0_PHY_GMII_EN, 1, data);
	SW_SET_REG_BY_FIELD(OPEMODE0, MAC0_PHY_GMII_TXCLK_SEL, 1, data);
	SW_SET_REG_BY_FIELD(OPEMODE0, MAC0_PHY_GMII_RXCLK_SEL, 1, data);
	HSL_REG_ENTRY_SET(rv, dev_id, OPEMODE0, 0,
			  (a_uint8_t *) (&data), sizeof (a_uint32_t));

    /* mac6 GMII PHY mode */
	data = 0;
	SW_SET_REG_BY_FIELD(OPEMODE3, MAC6_PHY_GMII_EN, 1, data);
	SW_SET_REG_BY_FIELD(OPEMODE3, MAC6_PHY_GMII_TXCLK_SEL, 1, data);
	SW_SET_REG_BY_FIELD(OPEMODE3, MAC6_PHY_GMII_RXCLK_SEL, 1, data);
	HSL_REG_ENTRY_SET(rv, dev_id, OPEMODE3, 0,
			  (a_uint8_t *) (&data), sizeof (a_uint32_t));

//    SW_SET_REG_BY_FIELD(POSTRIP, MAC0_MAC_MODE, 0, data);

    /* mac0 default rgmii mode */
//    SW_SET_REG_BY_FIELD(POSTRIP, MAC0_RGMII_EN, 0, data);
//    SW_SET_REG_BY_FIELD(POSTRIP, MAC0_GMII_EN, 1, data);

    /* mac5 default mode */
//    SW_SET_REG_BY_FIELD(POSTRIP, MAC5_PHY_MODE, 1, data);
//    SW_SET_REG_BY_FIELD(POSTRIP, MAC5_MAC_MODE, 0, data);
    
    /* phy default mode */
//    SW_SET_REG_BY_FIELD(POSTRIP, PHY4_RGMII_EN, 0, data);
//    SW_SET_REG_BY_FIELD(POSTRIP, PHY4_GMII_EN, 1, data);

	/* configure cpu port mac */
	HSL_REG_ENTRY_GET(rv, dev_id, PORT_STATUS, 0,
		  (a_uint8_t *) (&data), sizeof (a_uint32_t));
	SW_SET_REG_BY_FIELD(PORT_STATUS, LINK_EN, 0, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, TX_HALF_FLOW_EN, 0, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, DUPLEX_MODE, 1, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, RXMAC_EN, 1, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, TXMAC_EN, 1, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, SPEED_MODE, 1, data);
	
	HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, 0,
			  (a_uint8_t *) (&data), sizeof (a_uint32_t));

	/* configure ge port mac */
	data = 0;
	SW_SET_REG_BY_FIELD(PORT_STATUS, LINK_EN, 0, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, DUPLEX_MODE, 1, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, RXMAC_EN, 1, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, TXMAC_EN, 1, data);
	SW_SET_REG_BY_FIELD(PORT_STATUS, SPEED_MODE, 2, data);
	
	HSL_REG_ENTRY_SET(rv, dev_id, PORT_STATUS, 6,
			  (a_uint8_t *) (&data), sizeof (a_uint32_t));
#endif
    return SW_OK;
}


static sw_error_t
shiva_bist_test(a_uint32_t dev_id)
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
        udelay(5);
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
        udelay(5);
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
        udelay(5);
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

/**
 * @brief Init hsl layer.
 * @details Comments:
 *   This operation will init hsl layer and hsl layer
 * @param[in] dev_id device id
 * @param[in] cfg configuration for initialization
 * @return SW_OK or error code
 */
sw_error_t
shiva_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    hsl_api_t *p_api;
    a_uint32_t i, entry;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (NULL == shiva_cfg[dev_id]) {
        shiva_cfg[dev_id] = malloc(sizeof (ssdk_init_cfg));
    }

    if (NULL == shiva_cfg[dev_id]) {
        return SW_OUT_OF_MEM;
    }

    memcpy(shiva_cfg[dev_id], cfg, sizeof (ssdk_init_cfg));

    SW_RTN_ON_ERROR(shiva_reg_access_mode_init(dev_id, cfg->reg_mode));

    SW_RTN_ON_ERROR(shiva_dev_init(dev_id, cfg->cpu_mode));

#if 1  /* modified by James.Kan - 2010/04/01 */
	/* reset phy */
	for (i = 0; i < 4; i++)
	{
		f2_phy_reset(dev_id, i);
	}
#else
	for (i = 0; i < 4; i++)
	{
		f2_phy_poweroff(dev_id, i);
	}
#endif /* #if 0 */

    /* reset mac */
    SW_RTN_ON_ERROR(shiva_bist_test(dev_id));

    entry = 0x1;
    HSL_REG_FIELD_SET(rv, dev_id, MASK_CTL, 0, SOFT_RST,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    i = 0x10;
    do {
        HSL_REG_FIELD_GET(rv, dev_id, MASK_CTL, 0, SOFT_RST,
                          (a_uint8_t *) (&entry), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);

        udelay(10);
    } while (entry && --i);

    if (0 == i) {
        return SW_INIT_ERROR;
    }

    SW_RTN_ON_ERROR(hsl_port_prop_init());
    SW_RTN_ON_ERROR(hsl_port_prop_init_by_dev(dev_id));
//    SW_RTN_ON_ERROR(shiva_portproperty_init(dev_id, cfg->cpu_mode));

//    SW_RTN_ON_ERROR(shiva_mib_init(dev_id));
//    SW_RTN_ON_ERROR(shiva_port_ctrl_init(dev_id));

//    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    SW_RTN_ON_ERROR(shiva_hw_init(dev_id, cfg));

    return SW_OK;
}


sw_error_t
hsl_dev_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    sw_error_t rv = SW_OK;

    if (SW_MAX_NR_DEV <= dev_id) {
        return SW_BAD_PARAM;
    }

    memset(&dev_table[dev_id], 0, sizeof (hsl_dev_t));

    rv = shiva_init(dev_id, cfg);

    return rv;
}

sw_error_t
ssdk_init(a_uint32_t dev_id, ssdk_init_cfg * cfg)
{
    sw_error_t rv;

    rv = hsl_dev_init(dev_id, cfg);

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
shiva_port_hdr_status_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t val;

    HSL_DEV_ID_CHECK(dev_id);

//    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
//        return SW_BAD_PARAM;
//    }

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


/**
 * @brief Set cpu port status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code 
 */
HSL_LOCAL sw_error_t
shiva_cpu_port_status_set(a_uint32_t dev_id, a_bool_t enable)
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
 * @brief Set mib status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_mib_status_set(a_uint32_t dev_id, a_bool_t enable)
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


/**
 * @brief Get mib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
HSL_LOCAL sw_error_t
shiva_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                    fal_mib_info_t * mib_info)
{
    a_uint32_t val;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

//    if (A_FALSE == hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
//        return SW_OUT_OF_RANGE;
//    }

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
shiva_port_bc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

//    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
//        return SW_BAD_PARAM;
//    }

    HSL_REG_FIELD_GET(rv, dev_id, FLOOD_MASK, 0, BC_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable) {
        data &= (~((a_uint32_t)0x1 << port_id));
    } else if (A_FALSE == enable) {
        data |= (0x1 << port_id);
    } else {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FLOOD_MASK, 0, BC_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
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
shiva_port_unk_uc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

//    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
//        return SW_BAD_PARAM;
//    }

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
 * @brief Set flooding status of unknown multicast packets on a particular port.
 * @details  Comments:
 *   If enable unknown multicast packets filter on one port then unknown
 *   multicast packets can't flood out from this port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
static inline sw_error_t
shiva_port_unk_mc_filter_set(a_uint32_t dev_id, fal_port_t port_id,
                           a_bool_t enable)
{
    sw_error_t rv;
    a_uint32_t data;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_INCL_CPU)) {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_GET(rv, dev_id, FLOOD_MASK, 0, MUL_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    if (A_TRUE == enable) {
        data &= (~((a_uint32_t)0x1 << port_id));
    } else if (A_FALSE == enable) {
        data |= (0x1 << port_id);
    } else {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, FLOOD_MASK, 0, MUL_FLOOD_DP,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
    return rv;
}



/**
 * @brief Set arp packets leaky status on a particular port.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
static inline sw_error_t
shiva_port_arp_leaky_set(a_uint32_t dev_id, fal_port_t port_id,
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

    HSL_REG_FIELD_SET(rv, dev_id, PORT_CTL, port_id, ARP_LEAKY_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));    
    return rv;
}


/**
 * @brief Set arp packets hardware acknowledgement status on a particular device.
 * @param[in] dev_id device id
 * @param[in] enable A_TRUE or A_FALSE
 * @return SW_OK or error code
 */
static inline sw_error_t
shiva_arp_status_set(a_uint32_t dev_id, a_bool_t enable)
{
    a_uint32_t data;
    sw_error_t rv;

    HSL_DEV_ID_CHECK(dev_id);

    if (A_TRUE == enable) {
        data = 1;
    } else if (A_FALSE == enable) {
        data = 0;
    } else {
        return SW_BAD_PARAM;
    }

    HSL_REG_FIELD_SET(rv, dev_id, QM_CTL, 0, ARP_EN,
                      (a_uint8_t *) (&data), sizeof (a_uint32_t));
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
static inline sw_error_t
shiva_arp_cmd_set(a_uint32_t dev_id, fal_fwd_cmd_t cmd)
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

    HSL_REG_FIELD_SET(rv, dev_id, QM_CTL, 0, ARP_CMD,
                      (a_uint8_t *) (&val), sizeof (a_uint32_t));
    return rv;
}

sw_error_t	atheros_init()
{
	sw_error_t retVal = OPL_OK;
	UINT8	port_id;
	UINT32 ulData;
    sw_error_t rv;

    ssdk_init_cfg     cfg = def_init_cfg;

	oplRegFieldRead(0, 4, 16, &ulData);
	printf("product id = 0x%x\n", ulData);

	/* register mdio APIs */
	cfg.reg_func.mdio_set = mdioRegisterWrite;
	cfg.reg_func.mdio_get = mdioRegisterRead;
	cfg.reg_mode = HSL_MDIO;
    retVal = ssdk_init(0, &cfg);	
	if(OPL_OK != retVal)
	{
		printf("init Atheros switch failed.\n");
		return retVal;
	}

    HSL_REG_FIELD_GET(rv, 0, MASK_CTL, 0, DEVICE_ID,
                      (a_uint8_t *) (&ulData), sizeof (a_uint32_t));
	printf("Atheros Device ID: %d\n", ulData);
	
    HSL_REG_FIELD_GET(rv, 0, MASK_CTL, 0, REV_ID,
                      (a_uint8_t *) (&ulData), sizeof (a_uint32_t));
	printf("Atheros Revision ID: %d\n", ulData);

#if defined(CONFIG_ONU_1_1_RF400)
       /* CPU port enable, arp enable */
	ulData = 0x4002400;
	isis_reg_set(0,0x620,&ulData,4);

       /* port 0 arp leaky enable */
	isis_reg_get(0,0x660,&ulData,4);
	ulData = ulData | 0x4000000;
	isis_reg_set(0,0x660,&ulData,4);

	/* enable CPU header function, no atheros header */
       ulData = 0x0;
       isis_reg_set(0,0x98,&ulData,4);

       ulData = 0x0;
	isis_reg_set(0,0x9c,&ulData,4);	

	ulData = 0x7f7f7f;
	isis_reg_set(0,0x624,&ulData,4);

	/* config AR8327's MAC0 work in the GMII mode */
	ulData = 0x40000000;
	isis_reg_set(0,0x10,&ulData,4);	

	ulData = 0x4000;
	isis_reg_set(0,0x4,&ulData,4);	

	ulData = 0x7e;
	isis_reg_set(0,0x7c,&ulData,4);
	   
       /* mib counter enable */
	ulData = 0x80000301;
	isis_reg_set(0,0x30,&ulData,4);
#else
	/* enable CPU header function */
	shiva_port_hdr_status_set(0, 0, 1);

	shiva_cpu_port_status_set(0, 1);

	rv = shiva_port_bc_filter_set(0, 0, 1);
	rv = shiva_port_bc_filter_set(0, 1, 0);
	rv = shiva_port_bc_filter_set(0, 2, 0);
	rv = shiva_port_bc_filter_set(0, 3, 0);
	rv = shiva_port_bc_filter_set(0, 4, 0);
    
	shiva_port_unk_uc_filter_set(0, 0, 1);
	shiva_port_unk_uc_filter_set(0, 1, 0);
	shiva_port_unk_uc_filter_set(0, 2, 0);
	shiva_port_unk_uc_filter_set(0, 3, 0);
	shiva_port_unk_uc_filter_set(0, 4, 0);

	shiva_port_unk_mc_filter_set(0, 0, 1);

	/* enable cpu port arp leaky */
	shiva_port_arp_leaky_set(0, 0, 1);
	shiva_arp_status_set(0, 1);
	shiva_arp_cmd_set(0, FAL_MAC_CPY_TO_CPU);

	shiva_mib_status_set(0, 1);

    /* enable 10M led control, 0xca35ca35 -> 0xcb35cb35 */
	ulData = 0xcb35cb35;
    shiva_mdio_reg_set(0, 0xb4, (a_uint8_t *)&ulData, sizeof(a_uint32_t));
#endif

	return SW_OK;
}


sw_error_t atheros_wr_reg(a_uint32_t reg_addr, a_uint32_t value)
{	
	shiva_mdio_reg_set(0, reg_addr, (a_uint8_t *) &value, sizeof(a_uint32_t));
}

sw_error_t atheros_show_reg(a_uint32_t reg_addr)
{
	a_uint32_t value;
	
	shiva_mdio_reg_get(0, reg_addr, (a_uint8_t *) &value, sizeof(a_uint32_t));

	printf("atheros reg addr = 0x%X, value = 0x%X\n",
		reg_addr, value);
}

sw_error_t atheros_show_cnt(fal_port_t port_id)
{
	fal_mib_info_t mib_info;
	
	shiva_get_mib_info(0, port_id, &mib_info);

	printf("Counter info for port %d\n", port_id);
	printf("==================================================\n");
	printf("RxBroad     <0x%X>  RxPause     <0x%X>  RxMulti     <0x%X>  \n",
		mib_info.RxBroad, mib_info.RxPause, mib_info.RxMulti);
	printf("RxFcsErr    <0x%X>  RxAllignErr <0x%X>  RxRunt      <0x%X>\n",
		mib_info.RxFcsErr, mib_info.RxAllignErr, mib_info.RxRunt);
	printf("RxFragment  <0x%X>  Rx64Byte    <0x%X>  Rx128Byte   <0x%X>  \n",
		mib_info.RxFragment, mib_info.Rx64Byte, mib_info.Rx128Byte);
	printf("Rx256Byte   <0x%X>  Rx512Byte   <0x%X>  Rx1024Byte  <0x%X>  \n",
		mib_info.Rx256Byte, mib_info.Rx512Byte, mib_info.Rx1024Byte);
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

void atheros_shutdown_phy()
{
    int i;
    for (i = 0; i < 4; i++)
    {
        f2_phy_poweroff(0, i);
    }
}

void atheros_poweron_phy()
{
    int i;
    for (i = 0; i < 4; i++)
    {
        f2_phy_poweron(0, i);
    }
}


