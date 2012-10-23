/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */

#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_lock.h"
#include "sd.h"

#if defined ATHENA
#include "athena_init.h"
#elif defined GARUDA
#include "garuda_init.h"
#elif defined SHIVA
#include "shiva_init.h"
#endif
#include "sw_api.h"
#ifdef KERNEL_MODULE
#include "sw_api_ks.h"
#else
// uncommented by zran
//#include "sw_api_us.h"
#endif

static hsl_dev_t dev_table[SW_MAX_NR_DEV];
static ssdk_init_cfg *dev_ssdk_cfg[SW_MAX_NR_DEV] = { 0 };

hsl_dev_t *
hsl_dev_ptr_get(a_uint32_t dev_id)
{
    if (dev_id >= SW_MAX_NR_DEV)
        return NULL;

    return &dev_table[dev_id];
}

hsl_acl_func_t *
hsl_acl_ptr_get(a_uint32_t dev_id)
{
    if (dev_id >= SW_MAX_NR_DEV)
        return NULL;

    return &(dev_table[dev_id].acl_func);
}

sw_error_t
hsl_dev_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    sw_error_t rv = SW_OK;

    if (SW_MAX_NR_DEV <= dev_id) {
        return SW_BAD_PARAM;
    }

    aos_mem_set(&dev_table[dev_id], 0, sizeof (hsl_dev_t));

    SW_RTN_ON_ERROR(sd_init(dev_id,cfg));

#ifdef UK_IF
    SW_RTN_ON_ERROR(sw_uk_init(cfg->nl_prot));
#endif

#if defined API_LOCK
    SW_RTN_ON_ERROR(hsl_api_lock_init());
#endif

#if defined ATHENA
    rv = athena_init(dev_id, cfg);
#elif defined GARUDA
    rv = garuda_init(dev_id, cfg);
#elif defined SHIVA
    rv = shiva_init(dev_id, cfg);
#endif

    if (NULL == dev_ssdk_cfg[dev_id]) {
        dev_ssdk_cfg[dev_id] = aos_mem_alloc(sizeof (ssdk_init_cfg));
    }

    if (NULL == dev_ssdk_cfg[dev_id]) {
        return SW_OUT_OF_MEM;
    }

    aos_mem_copy(dev_ssdk_cfg[dev_id], cfg, sizeof (ssdk_init_cfg));

    return rv;
}

sw_error_t
hsl_dev_reduced_init(a_uint32_t dev_id, hsl_init_mode cpu_mode, hsl_access_mode reg_mode)
{
    sw_error_t rv = SW_OK;

    if (SW_MAX_NR_DEV <= dev_id) {
        return SW_BAD_PARAM;
    }

    aos_mem_set(&dev_table[dev_id], 0, sizeof (hsl_dev_t));

#if defined API_LOCK
    SW_RTN_ON_ERROR(hsl_api_lock_init());
#endif

#if defined ATHENA
{
    ssdk_init_cfg cfg = def_init_cfg;
    cfg.reg_mode = reg_mode;
    cfg.cpu_mode = cpu_mode;
    rv = athena_init(dev_id, &cfg);
}
#elif defined GARUDA
    switch (cpu_mode) {
        case HSL_NO_CPU:
        {
            ssdk_init_cfg cfg = def_init_cfg_nocpu;
            garuda_init_spec_cfg garuda_init_cfg = def_init_spec_cfg_nocpu;

            cfg.chip_spec_cfg = &garuda_init_cfg;
            cfg.reg_mode = reg_mode;
            rv = garuda_init(dev_id, &cfg);
            break;
        }
        case HSL_CPU_2:
        {
            ssdk_init_cfg cfg = def_init_cfg_cpu2;
            garuda_init_spec_cfg garuda_init_cfg = def_init_spec_cfg_cpu2;

            cfg.chip_spec_cfg = &garuda_init_cfg;
            cfg.reg_mode = reg_mode;
            rv = garuda_init(dev_id, &cfg);
            break;
        }
        case HSL_CPU_1:
        {
            ssdk_init_cfg cfg = def_init_cfg_cpu1;
            garuda_init_spec_cfg garuda_init_cfg = def_init_spec_cfg_cpu1;

            cfg.chip_spec_cfg = &garuda_init_cfg;
            cfg.reg_mode = reg_mode;
            rv = garuda_init(dev_id, &cfg);
            break;
        }

        case HSL_CPU_1_PLUS:
        {
            ssdk_init_cfg cfg = def_init_cfg_cpu1plus;
            garuda_init_spec_cfg garuda_init_cfg = def_init_spec_cfg_cpu1plus;

            cfg.chip_spec_cfg = &garuda_init_cfg;
            cfg.reg_mode = reg_mode;
            rv = garuda_init(dev_id, &cfg);
            break;
        }

        default:
            return SW_BAD_PARAM;
    }
#elif defined SHIVA
    ssdk_init_cfg cfg = def_init_cfg;
    cfg.reg_mode = reg_mode;
    cfg.cpu_mode = cpu_mode;
    rv = shiva_init(dev_id, &cfg);
#endif

    return rv;
}

sw_error_t
hsl_ssdk_cfg(a_uint32_t dev_id, ssdk_cfg_t *ssdk_cfg)
{
    aos_mem_set(&(ssdk_cfg->init_cfg), 0,  sizeof(ssdk_init_cfg));
    
    aos_mem_copy(&(ssdk_cfg->init_cfg), dev_ssdk_cfg[dev_id], sizeof(ssdk_init_cfg));

#ifdef VERSION
    aos_mem_copy(ssdk_cfg->build_ver, VERSION, sizeof(VERSION));
#endif

#ifdef BUILD_DATE
    aos_mem_copy(ssdk_cfg->build_date, BUILD_DATE, sizeof(BUILD_DATE));
#endif

#if defined ATHENA
    aos_mem_copy(ssdk_cfg->chip_type, "athena", sizeof("athena"));
#elif defined GARUDA
    aos_mem_copy(ssdk_cfg->chip_type, "garuda", sizeof("garuda"));
#elif defined SHIVA
    aos_mem_copy(ssdk_cfg->chip_type, "shiva", sizeof("shiva"));
#elif defined HORUS
    aos_mem_copy(ssdk_cfg->chip_type, "horus", sizeof("horus"));
#endif

#ifdef CPU
    aos_mem_copy(ssdk_cfg->cpu_type, CPU, sizeof(CPU));
#endif

#ifdef OS
    aos_mem_copy(ssdk_cfg->os_info, OS, sizeof(OS));
    #if defined KVER26
        aos_mem_copy(ssdk_cfg->os_info+sizeof(OS)-1, " version 2.6", sizeof(" version 2.6"));
    #elif defined KVER24
        aos_mem_copy(ssdk_cfg->os_info+sizeof(OS)-1, " version 2.4", sizeof(" version 2.4"));
    #else
        aos_mem_copy(ssdk_cfg->os_info+sizeof(OS)-1, " version unknown", sizeof(" version unknown"));
    #endif
#endif
    
#ifdef HSL_STANDALONG
    ssdk_cfg->fal_mod = A_FALSE;
#else
    ssdk_cfg->fal_mod = A_TRUE;
#endif 

#ifdef USER_MODE
    ssdk_cfg->kernel_mode = A_FALSE;
#else
    ssdk_cfg->kernel_mode = A_TRUE;
#endif

#ifdef UK_IF
    ssdk_cfg->uk_if = A_TRUE;
#else
    ssdk_cfg->uk_if = A_FALSE;
#endif

#ifdef IN_ACL
    ssdk_cfg->features.in_acl = A_TRUE;
#endif
#ifdef IN_FDB
    ssdk_cfg->features.in_fdb = A_TRUE;
#endif
#ifdef IN_IGMP
    ssdk_cfg->features.in_igmp = A_TRUE;
#endif
#ifdef IN_LEAKY
    ssdk_cfg->features.in_leaky = A_TRUE;
#endif
#ifdef IN_LED
    ssdk_cfg->features.in_led = A_TRUE;
#endif
#ifdef IN_MIB
    ssdk_cfg->features.in_mib = A_TRUE;
#endif
#ifdef IN_MIRROR
    ssdk_cfg->features.in_mirror = A_TRUE;
#endif
#ifdef IN_MISC
    ssdk_cfg->features.in_misc = A_TRUE;
#endif
#ifdef IN_PORTCONTROL
    ssdk_cfg->features.in_portcontrol = A_TRUE;
#endif
#ifdef IN_PORTVLAN
    ssdk_cfg->features.in_portvlan = A_TRUE;
#endif
#ifdef IN_QOS
    ssdk_cfg->features.in_qos = A_TRUE;
#endif
#ifdef IN_RATE
    ssdk_cfg->features.in_rate = A_TRUE;
#endif
#ifdef IN_STP
    ssdk_cfg->features.in_stp = A_TRUE;
#endif
#ifdef IN_VLAN
    ssdk_cfg->features.in_vlan = A_TRUE;
#endif
#ifdef IN_REDUCED_ACL
    ssdk_cfg->features.in_reduced_acl = A_TRUE;
#endif

    return SW_OK;
}

sw_error_t
hsl_dev_cleanup(void)
{
    sw_error_t rv = SW_OK;
    a_uint32_t dev_id;

#if defined ATHENA
    rv = athena_cleanup();
#elif defined GARUDA
    rv = garuda_cleanup();
#elif defined SHIVA
    rv = shiva_cleanup();
#endif

#ifdef UK_IF
    SW_RTN_ON_ERROR(sw_uk_cleanup());
#endif

    for (dev_id = 0; dev_id < SW_MAX_NR_DEV; dev_id++) {
        if (dev_ssdk_cfg[dev_id]) {
            aos_mem_free(dev_ssdk_cfg[dev_id]);
            dev_ssdk_cfg[dev_id] = NULL;
        }
    }

    return rv;
}

sw_error_t
hsl_access_mode_set(a_uint32_t dev_id, hsl_access_mode reg_mode)
{
    sw_error_t rv;

    if (SW_MAX_NR_DEV <= dev_id) {
        return SW_BAD_PARAM;
    }

#if defined ATHENA
    rv = athena_access_mode_set(dev_id, reg_mode);
#elif defined GARUDA
    rv = garuda_access_mode_set(dev_id, reg_mode);
#elif defined SHIVA
    rv = shiva_access_mode_set(dev_id, reg_mode);
#endif
    return rv;
}

