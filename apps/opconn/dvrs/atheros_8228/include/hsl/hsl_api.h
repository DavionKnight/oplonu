/*
 * Copyright (c) 2007-2008 Atheros Communications, Inc.
 * All rights reserved.
 *
 */

#ifndef _HSL_API_H
#define _HSL_API_H

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#include "fal.h"

/* Misc */
#define MISC_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_arp_status_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_arp_status_get) (a_uint32_t dev_id, a_bool_t *enable);

typedef sw_error_t
    (*hsl_frame_max_size_set) (a_uint32_t dev_id, a_uint32_t size);

typedef sw_error_t
    (*hsl_frame_max_size_get) (a_uint32_t dev_id, a_uint32_t *size);

typedef sw_error_t
    (*hsl_port_unk_sa_cmd_set) (a_uint32_t dev_id, fal_port_t port_id,
                                fal_fwd_cmd_t cmd);

typedef sw_error_t
    (*hsl_port_unk_sa_cmd_get) (a_uint32_t dev_id, fal_port_t port_id,
                                fal_fwd_cmd_t * cmd);

typedef sw_error_t
    (*hsl_port_unk_uc_filter_set) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_port_unk_uc_filter_get) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);

typedef sw_error_t
    (*hsl_port_unk_mc_filter_set) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_port_unk_mc_filter_get) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);

typedef sw_error_t
    (*hsl_port_bc_filter_set) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_port_bc_filter_get) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);

typedef sw_error_t
    (*hsl_cpu_port_status_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_cpu_port_status_get) (a_uint32_t dev_id, a_bool_t *enable);

typedef sw_error_t
    (*hsl_bc_to_cpu_port_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_bc_to_cpu_port_get) (a_uint32_t dev_id, a_bool_t * enable);

typedef sw_error_t
    (*hsl_port_dhcp_set) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_pppoe_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

typedef sw_error_t
    (*hsl_pppoe_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

typedef sw_error_t
    (*hsl_pppoe_status_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_pppoe_status_get) (a_uint32_t dev_id, a_bool_t * enable);

typedef sw_error_t
    (*hsl_port_dhcp_get) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);

typedef sw_error_t
    (*hsl_arp_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

typedef sw_error_t
    (*hsl_arp_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

typedef sw_error_t
    (*hsl_eapol_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

typedef sw_error_t
    (*hsl_eapol_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

typedef sw_error_t
    (*hsl_pppoe_session_add) (a_uint32_t dev_id, a_uint32_t session_id, a_bool_t strip_hdr);

typedef sw_error_t
    (*hsl_pppoe_session_del) (a_uint32_t dev_id, a_uint32_t session_id);

typedef sw_error_t
    (*hsl_pppoe_session_get) (a_uint32_t dev_id, a_uint32_t session_id, a_bool_t * strip_hdr);

typedef sw_error_t
    (*hsl_eapol_status_set) (a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_eapol_status_get) (a_uint32_t dev_id, a_uint32_t port_id, a_bool_t * enable);

typedef sw_error_t
    (*hsl_ripv1_status_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_ripv1_status_get) (a_uint32_t dev_id, a_bool_t * enable);

/* Port Control */
#define PORT_CONTROL_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_port_duplex_get) (a_uint32_t dev_id, fal_port_t port_id,
                            fal_port_duplex_t * pduplex);

typedef sw_error_t
    (*hsl_port_duplex_set) (a_uint32_t dev_id, fal_port_t port_id,
                            fal_port_duplex_t duplex);

typedef sw_error_t
    (*hsl_port_speed_get) (a_uint32_t dev_id, fal_port_t port_id,
                           fal_port_speed_t * pspeed);

typedef sw_error_t
    (*hsl_port_autoneg_status_get) (a_uint32_t dev_id, fal_port_t port_id,
                                    a_bool_t * status);

typedef sw_error_t
    (*hsl_port_speed_set) (a_uint32_t dev_id, fal_port_t port_id,
                           fal_port_speed_t speed);

typedef sw_error_t
    (*hsl_port_autoneg_enable) (a_uint32_t dev_id, fal_port_t port_id);

typedef sw_error_t
    (*hsl_port_autoneg_restart) (a_uint32_t dev_id, fal_port_t port_id);

typedef sw_error_t
    (*hsl_port_autoneg_adv_get) (a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t * autoadv);

typedef sw_error_t
    (*hsl_port_autoneg_adv_set) (a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t autoadv);

typedef sw_error_t
    (*hsl_port_hdr_status_set) (a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t enable);

typedef sw_error_t
    (*hsl_port_hdr_status_get) (a_uint32_t dev_id, fal_port_t port_id,
                                a_bool_t *enable);

typedef sw_error_t
    (*hsl_port_flowctrl_set) (a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable);

typedef sw_error_t
    (*hsl_port_flowctrl_get) (a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t * enable);

typedef sw_error_t
    (*hsl_port_flowctrl_forcemode_set) (a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable);

typedef sw_error_t
    (*hsl_port_flowctrl_forcemode_get) (a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t * enable);

typedef sw_error_t
    (*hsl_port_powersave_set) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


typedef sw_error_t
    (*hsl_port_powersave_get) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);


typedef sw_error_t
    (*hsl_port_hibernate_set) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);


typedef sw_error_t
    (*hsl_port_hibernate_get) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);


typedef sw_error_t
    (*hsl_port_cdt) (a_uint32_t dev_id, fal_port_t port_id, a_uint32_t mdi_pair,
                                            fal_cable_status_t *cable_status, a_uint32_t *cable_len);

/* VLAN */
#define VLAN_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_vlan_entry_append) (a_uint32_t dev_id, const fal_vlan_t * vlan_entry);

typedef sw_error_t
    (*hsl_vlan_create) (a_uint32_t dev_id, a_uint32_t vlan_id);

typedef sw_error_t
    (*hsl_vlan_next) (a_uint32_t dev_id, a_uint32_t vlan_id,
                      fal_vlan_t * p_vlan);

typedef sw_error_t
    (*hsl_vlan_find) (a_uint32_t dev_id, a_uint32_t vlan_id,
                      fal_vlan_t * p_vlan);

typedef sw_error_t
    (*hsl_vlan_member_update) (a_uint32_t dev_id, a_uint32_t vlan_id,
                               fal_pbmp_t member, fal_pbmp_t u_member);

typedef sw_error_t
    (*hsl_vlan_delete) (a_uint32_t dev_id, a_uint32_t vlan_id);

typedef sw_error_t
    (*hsl_vlan_flush) (a_uint32_t dev_id);

/* Port Vlan */
#define PORT_VLAN_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_port_1qmode_get) (a_uint32_t dev_id, fal_port_t port_id,
                            fal_pt_1qmode_t * pport_1qmode);

typedef sw_error_t
    (*hsl_port_1qmode_set) (a_uint32_t dev_id, fal_port_t port_id,
                            fal_pt_1qmode_t port_1qmode);

typedef sw_error_t
    (*hsl_port_egvlanmode_get) (a_uint32_t dev_id, fal_port_t port_id,
                                fal_pt_1q_egmode_t * pport_egvlanmode);

typedef sw_error_t
    (*hsl_port_egvlanmode_set) (a_uint32_t dev_id, fal_port_t port_id,
                                fal_pt_1q_egmode_t port_egvlanmode);

typedef sw_error_t
    (*hsl_portvlan_member_add) (a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t mem_port_id);

typedef sw_error_t
    (*hsl_portvlan_member_del) (a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t mem_port_id);

typedef sw_error_t
    (*hsl_portvlan_member_update) (a_uint32_t dev_id, fal_port_t port_id,
                                   fal_pbmp_t mem_port_map);

typedef sw_error_t
    (*hsl_portvlan_member_get) (a_uint32_t dev_id, fal_port_t port_id,
                                fal_pbmp_t * mem_port_map);

typedef sw_error_t
    (*hsl_port_nestvlan_set) (a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable);

typedef sw_error_t
    (*hsl_port_nestvlan_get) (a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t *enable);

typedef sw_error_t
    (*hsl_nestvlan_tpid_set) (a_uint32_t dev_id, a_uint32_t tpid);

typedef sw_error_t
    (*hsl_nestvlan_tpid_get) (a_uint32_t dev_id, a_uint32_t *tpid);

typedef sw_error_t
    (*hsl_port_default_vid_set) (a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t vid);

typedef sw_error_t
    (*hsl_port_default_vid_get) (a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t * vid);

typedef sw_error_t
    (*hsl_port_force_default_vid_set) (a_uint32_t dev_id,
                                       fal_port_t port_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_port_force_default_vid_get) (a_uint32_t dev_id,
                                       fal_port_t port_id, a_bool_t * enable);

typedef sw_error_t
    (*hsl_port_force_portvlan_set) (a_uint32_t dev_id, fal_port_t port_id,
                                    a_bool_t enable);

typedef sw_error_t
    (*hsl_port_force_portvlan_get) (a_uint32_t dev_id, fal_port_t port_id,
                                    a_bool_t * enable);

typedef sw_error_t
    (*hsl_port_invlan_mode_set) (a_uint32_t dev_id, fal_port_t port_id,
                                   fal_pt_invlan_mode_t mode);

typedef sw_error_t
    (*hsl_port_invlan_mode_get) (a_uint32_t dev_id, fal_port_t port_id,
                                   fal_pt_invlan_mode_t * mode);

typedef sw_error_t
    (*hsl_port_tls_set) (a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable);

typedef sw_error_t
    (*hsl_port_tls_get) (a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable);

typedef sw_error_t
    (*hsl_port_pri_propagation_set) (a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable);

typedef sw_error_t
    (*hsl_port_pri_propagation_get) (a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t * enable);

typedef sw_error_t
    (*hsl_port_default_svid_set) (a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t vid);

typedef sw_error_t
    (*hsl_port_default_svid_get) (a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * vid);

typedef sw_error_t
    (*hsl_port_default_cvid_set) (a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t vid);

typedef sw_error_t
    (*hsl_port_default_cvid_get) (a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t * vid);

typedef sw_error_t
    (*hsl_port_vlan_propagation_set) (a_uint32_t dev_id, fal_port_t port_id,
                               fal_vlan_propagation_mode_t mode);

typedef sw_error_t
    (*hsl_port_vlan_propagation_get) (a_uint32_t dev_id, fal_port_t port_id,
                               fal_vlan_propagation_mode_t * mode);

typedef sw_error_t
    (*hsl_port_vlan_trans_add) (a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry);

typedef sw_error_t
    (*hsl_port_vlan_trans_del) (a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry);

typedef sw_error_t
    (*hsl_port_vlan_trans_get) (a_uint32_t dev_id, fal_port_t port_id, fal_vlan_trans_entry_t *entry);

typedef sw_error_t
    (*hsl_qinq_mode_set) (a_uint32_t dev_id, fal_qinq_mode_t mode);

typedef sw_error_t
    (*hsl_qinq_mode_get) (a_uint32_t dev_id, fal_qinq_mode_t * mode);

typedef sw_error_t
    (*hsl_port_qinq_role_set) (a_uint32_t dev_id, fal_port_t port_id,
                                fal_qinq_port_role_t role);

typedef sw_error_t
    (*hsl_port_qinq_role_get) (a_uint32_t dev_id, fal_port_t port_id,
                                fal_qinq_port_role_t * role);

typedef sw_error_t
(*hsl_port_vlan_trans_iterate) (a_uint32_t dev_id, fal_port_t port_id,
                        a_uint32_t * iterator, fal_vlan_trans_entry_t * entry);


/* FDB */
#define FDB_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_fdb_add) (a_uint32_t dev_id, const fal_fdb_entry_t * entry);

typedef sw_error_t
    (*hsl_fdb_del_all) (a_uint32_t dev_id, a_uint32_t flag);

typedef sw_error_t
    (*hsl_fdb_del_by_port) (a_uint32_t dev_id, a_uint32_t port_id, a_uint32_t flag);

typedef sw_error_t
    (*hsl_fdb_del_by_mac) (a_uint32_t dev_id, const fal_fdb_entry_t * addr);

typedef sw_error_t
    (*hsl_fdb_first) (a_uint32_t dev_id, fal_fdb_entry_t * entry);

typedef sw_error_t
    (*hsl_fdb_next) (a_uint32_t dev_id, fal_fdb_entry_t * entry);

typedef sw_error_t
    (*hsl_fdb_find) (a_uint32_t dev_id, fal_fdb_entry_t * entry);

typedef sw_error_t
    (*hsl_fdb_port_learn_set) (a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable);

typedef sw_error_t
    (*hsl_fdb_port_learn_get) (a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t *enable);

typedef sw_error_t
    (*hsl_fdb_age_ctrl_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_fdb_age_ctrl_get) (a_uint32_t dev_id, a_bool_t *enable);

typedef sw_error_t
    (*hsl_fdb_age_time_set) (a_uint32_t dev_id, a_uint32_t * time);

typedef sw_error_t
    (*hsl_fdb_age_time_get) (a_uint32_t dev_id, a_uint32_t *time);

typedef sw_error_t
    (*hsl_fdb_iterate)(a_uint32_t dev_id, a_uint32_t * iterator,
                        fal_fdb_entry_t * entry);

/* QOS */
#define QOS_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_cosmap_up_queue_set) (a_uint32_t dev_id, a_uint32_t up,
                                fal_queue_t queue);

typedef sw_error_t
    (*hsl_cosmap_up_queue_get) (a_uint32_t dev_id, a_uint32_t up,
                                fal_queue_t * queue);

typedef sw_error_t
    (*hsl_cosmap_dscp_queue_set) (a_uint32_t dev_id, a_uint32_t dscp,
                                  fal_queue_t queue);

typedef sw_error_t
    (*hsl_cosmap_dscp_queue_get) (a_uint32_t dev_id, a_uint32_t dscp,
                                  fal_queue_t * queue);

typedef sw_error_t
    (*hsl_qos_port_mode_set) (a_uint32_t dev_id, fal_port_t port_id,
                                fal_qos_mode_t mode,
                                a_bool_t enable);

typedef sw_error_t
    (*hsl_qos_port_mode_get) (a_uint32_t dev_id, fal_port_t port_id,
                                fal_qos_mode_t mode,
                                a_bool_t * enable);

typedef sw_error_t
    (*hsl_qos_port_mode_pri_set) (a_uint32_t dev_id, fal_port_t port_id,
                                    fal_qos_mode_t mode,
                                    a_uint32_t pri);

typedef sw_error_t
    (*hsl_qos_port_mode_pri_get) (a_uint32_t dev_id, fal_port_t port_id,
                                    fal_qos_mode_t mode,
                                    a_uint32_t * pri);

typedef sw_error_t
    (*hsl_qos_port_default_up_set) (a_uint32_t dev_id, fal_port_t port_id,
                                    a_uint32_t up);

typedef sw_error_t
    (*hsl_qos_port_default_up_get) (a_uint32_t dev_id, fal_port_t port_id,
                                    a_uint32_t * up);

typedef sw_error_t
    (*hsl_qos_sch_mode_set) (a_uint32_t dev_id, 
                             fal_sch_mode_t mode, const a_uint32_t weight[]);

typedef sw_error_t
    (*hsl_qos_sch_mode_get) (a_uint32_t dev_id,
                             fal_sch_mode_t * mode, a_uint32_t weight[]);

typedef sw_error_t
    (*hsl_qos_queue_tx_buf_status_set) (a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t enable);

typedef sw_error_t
    (*hsl_qos_queue_tx_buf_status_get) (a_uint32_t dev_id, fal_port_t port_id,
                                  a_bool_t *enable);

typedef sw_error_t
    (*hsl_qos_queue_tx_buf_nr_set) (a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t queue_id, a_uint32_t *number);

typedef sw_error_t
    (*hsl_qos_queue_tx_buf_nr_get) (a_uint32_t dev_id, fal_port_t port_id,
                                  a_uint32_t queue_id, a_uint32_t *number);

typedef sw_error_t
    (*hsl_qos_port_tx_buf_status_set) (a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t enable);

typedef sw_error_t
    (*hsl_qos_port_tx_buf_status_get) (a_uint32_t dev_id, fal_port_t port_id,
                                 a_bool_t *enable);

typedef sw_error_t
    (*hsl_qos_port_tx_buf_nr_set) (a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t *number);

typedef sw_error_t
    (*hsl_qos_port_tx_buf_nr_get) (a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t *number);

typedef sw_error_t
    (*hsl_qos_port_rx_buf_nr_set) (a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t *number);

typedef sw_error_t
    (*hsl_qos_port_rx_buf_nr_get) (a_uint32_t dev_id, fal_port_t port_id,
                                 a_uint32_t *number);

typedef sw_error_t
    (*hsl_qos_port_sch_mode_set) (a_uint32_t dev_id, a_uint32_t port_id, 
                             fal_sch_mode_t mode, const a_uint32_t weight[]);

typedef sw_error_t
    (*hsl_qos_port_sch_mode_get) (a_uint32_t dev_id, a_uint32_t port_id,
                             fal_sch_mode_t * mode, a_uint32_t weight[]);

/* Rate */
#define RATE_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_rate_queue_egrl_set) (a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t queue_id, a_uint32_t * speed,
                               a_bool_t enable);

typedef sw_error_t
    (*hsl_rate_queue_egrl_get) (a_uint32_t dev_id, fal_port_t port_id,
                               a_uint32_t queue_id, a_uint32_t * speed,
                               a_bool_t * enable);

typedef sw_error_t
    (*hsl_rate_port_egrl_set) (a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * speed, a_bool_t enable);

typedef sw_error_t
    (*hsl_rate_port_egrl_get) (a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * speed, a_bool_t * enable);

typedef sw_error_t
    (*hsl_rate_port_inrl_set) (a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * speed, a_bool_t enable);

typedef sw_error_t
    (*hsl_rate_port_inrl_get) (a_uint32_t dev_id, fal_port_t port_id,
                              a_uint32_t * speed, a_bool_t * enable);

typedef sw_error_t
    (*hsl_storm_ctrl_frame_set) (a_uint32_t dev_id, fal_port_t port_id,
                                 fal_storm_type_t storm_type,
                                 a_bool_t enable);

typedef sw_error_t
    (*hsl_storm_ctrl_frame_get) (a_uint32_t dev_id, fal_port_t port_id,
                                 fal_storm_type_t storm_type,
                                 a_bool_t * enable);

typedef sw_error_t
    (*hsl_storm_ctrl_rate_set) (a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t * rate);

typedef sw_error_t
    (*hsl_storm_ctrl_rate_get) (a_uint32_t dev_id, fal_port_t port_id,
                                a_uint32_t * rate);

/* Mirror */
#define MIRROR_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_mirr_analysis_port_set) (a_uint32_t dev_id, fal_port_t port_id);

typedef sw_error_t
    (*hsl_mirr_analysis_port_get) (a_uint32_t dev_id, fal_port_t * port_id);

typedef sw_error_t
    (*hsl_mirr_port_in_set) (a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t enable);

typedef sw_error_t
    (*hsl_mirr_port_in_get) (a_uint32_t dev_id, fal_port_t port_id,
                             a_bool_t * enable);

typedef sw_error_t
    (*hsl_mirr_port_eg_set) (a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t enable);

typedef sw_error_t
    (*hsl_mirr_port_eg_get) (a_uint32_t dev_id, fal_port_t port_id,
                            a_bool_t * enable);

/* STP */
#define STP_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_stp_port_state_set) (a_uint32_t dev_id, a_uint32_t st_id,
                               fal_port_t port_id, fal_stp_state_t state);

typedef sw_error_t
    (*hsl_stp_port_state_get) (a_uint32_t dev_id, a_uint32_t st_id,
                               fal_port_t port_id, fal_stp_state_t * state);
/* IGMP */
#define IGMP_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_port_igmps_status_set) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_port_igmps_status_get) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable);

typedef sw_error_t
    (*hsl_igmp_mld_cmd_set) (a_uint32_t dev_id, fal_fwd_cmd_t cmd);

typedef sw_error_t
    (*hsl_igmp_mld_cmd_get) (a_uint32_t dev_id, fal_fwd_cmd_t * cmd);

typedef sw_error_t
    (*hsl_port_igmp_join_set) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_port_igmp_join_get) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);

typedef sw_error_t
    (*hsl_port_igmp_leave_set) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_port_igmp_leave_get) (a_uint32_t dev_id, fal_port_t port_id, a_bool_t * enable);

typedef sw_error_t
    (*hsl_igmp_rp_set) (a_uint32_t dev_id, fal_pbmp_t pts);

typedef sw_error_t
    (*hsl_igmp_rp_get) (a_uint32_t dev_id, fal_pbmp_t * pts);

typedef sw_error_t
    (*hsl_igmp_entry_creat_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_igmp_entry_creat_get) (a_uint32_t dev_id, a_bool_t * enable);

typedef sw_error_t
    (*hsl_igmp_entry_static_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_igmp_entry_static_get) (a_uint32_t dev_id, a_bool_t * enable);

typedef sw_error_t
    (*hsl_igmp_entry_leaky_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_igmp_entry_leaky_get) (a_uint32_t dev_id, a_bool_t * enable);

typedef sw_error_t
    (*hsl_igmp_entry_v3_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_igmp_entry_v3_get) (a_uint32_t dev_id, a_bool_t * enable);

typedef sw_error_t
    (*hsl_igmp_entry_queue_set) (a_uint32_t dev_id, a_bool_t enable, a_uint32_t queue);

typedef sw_error_t
    (*hsl_igmp_entry_queue_get) (a_uint32_t dev_id, a_bool_t * enable, a_uint32_t * queue);

/* Leaky */
#define LEAKY_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_uc_leaky_mode_set) (a_uint32_t dev_id,
                              fal_leaky_ctrl_mode_t ctrl_mode);

typedef sw_error_t
    (*hsl_uc_leaky_mode_get) (a_uint32_t dev_id,
                              fal_leaky_ctrl_mode_t * ctrl_mode);

typedef sw_error_t
    (*hsl_mc_leaky_mode_set) (a_uint32_t dev_id,
                              fal_leaky_ctrl_mode_t ctrl_mode);

typedef sw_error_t
    (*hsl_mc_leaky_mode_get) (a_uint32_t dev_id,
                              fal_leaky_ctrl_mode_t *ctrl_mode);

typedef sw_error_t
    (*hsl_port_arp_leaky_set) (a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t enable);

typedef sw_error_t
    (*hsl_port_arp_leaky_get) (a_uint32_t dev_id, fal_port_t port_id,
                               a_bool_t *enable);

typedef sw_error_t
    (*hsl_port_uc_leaky_set) (a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable);

typedef sw_error_t
    (*hsl_port_uc_leaky_get) (a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t *enable);

typedef sw_error_t
    (*hsl_port_mc_leaky_set) (a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t enable);

typedef sw_error_t
    (*hsl_port_mc_leaky_get) (a_uint32_t dev_id, fal_port_t port_id,
                              a_bool_t *enable);

/* Mib */
#define MIB_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_get_mib_info) (a_uint32_t dev_id, fal_port_t port_id,
                         fal_mib_info_t * mib_info);

typedef sw_error_t
    (*hsl_mib_status_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
    (*hsl_mib_status_get) (a_uint32_t dev_id, a_bool_t * enable);

/* Acl */
#define ACL_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_acl_list_creat) (a_uint32_t dev_id, a_uint32_t list_id,
                        a_uint32_t list_pri);

typedef sw_error_t
    (*hsl_acl_list_destroy) (a_uint32_t dev_id, a_uint32_t list_id);

typedef sw_error_t
    (*hsl_acl_rule_add) (a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id,
                        a_uint32_t rule_nr, fal_acl_rule_t * rule);

typedef sw_error_t
    (*hsl_acl_rule_delete) (a_uint32_t dev_id, a_uint32_t list_id,
                        a_uint32_t rule_id, a_uint32_t rule_nr);

typedef sw_error_t
    (*hsl_acl_rule_query) (a_uint32_t dev_id, a_uint32_t list_id, a_uint32_t rule_id,
                        fal_acl_rule_t * rule);

typedef sw_error_t
    (*hsl_acl_list_bind) (a_uint32_t dev_id, a_uint32_t list_id,
                        fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                        a_uint32_t obj_idx);

typedef sw_error_t
    (*hsl_acl_list_unbind) (a_uint32_t dev_id, a_uint32_t list_id,
                        fal_acl_direc_t direc, fal_acl_bind_obj_t obj_t,
                        a_uint32_t obj_idx);

typedef sw_error_t
	(*hsl_acl_status_set) (a_uint32_t dev_id, a_bool_t enable);

typedef sw_error_t
	(*hsl_acl_status_get) (a_uint32_t dev_id, a_bool_t * enable);

typedef sw_error_t
	(*hsl_acl_list_dump) (a_uint32_t dev_id);

typedef sw_error_t
	(*hsl_acl_rule_dump) (a_uint32_t dev_id);

typedef sw_error_t
	(*hsl_dev_reset) (a_uint32_t dev_id);

/* LED */
#define LED_FUNC_PROTOTYPE_DEF
typedef sw_error_t
(*hsl_led_ctrl_pattern_set) (a_uint32_t dev_id, led_pattern_group_t group,
                      led_pattern_id_t id, led_ctrl_pattern_t * pattern);

typedef sw_error_t
(*hsl_led_ctrl_pattern_get) (a_uint32_t dev_id, led_pattern_group_t group,
                      led_pattern_id_t id, led_ctrl_pattern_t * pattern);

/* REG */
#define REG_FUNC_PROTOTYPE_DEF
typedef sw_error_t
    (*hsl_phy_get) (a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg,
                    a_uint16_t * value);

typedef sw_error_t
    (*hsl_phy_set) (a_uint32_t dev_id, a_uint32_t phy_addr, a_uint32_t reg,
                    a_uint16_t value);

typedef sw_error_t
    (*hsl_reg_get) (a_uint32_t dev_id, a_uint32_t reg_addr,
                    a_uint8_t value[], a_uint32_t value_len);

typedef sw_error_t
    (*hsl_reg_set) (a_uint32_t dev_id, a_uint32_t reg_addr,
                    a_uint8_t value[], a_uint32_t value_len);

typedef sw_error_t
    (*hsl_reg_field_get) (a_uint32_t dev_id, a_uint32_t reg_addr,
                          a_uint32_t bit_offset, a_uint32_t field_len,
                          a_uint8_t value[], a_uint32_t value_len);

typedef sw_error_t
    (*hsl_reg_field_set) (a_uint32_t dev_id, a_uint32_t reg_addr,
                          a_uint32_t bit_offset, a_uint32_t field_len,
                          const a_uint8_t value[], a_uint32_t value_len);

typedef sw_error_t
    (*hsl_reg_entries_get) (a_uint32_t dev_id, a_uint32_t reg_addr,
                          a_uint32_t entry_len, a_uint8_t value[],
                          a_uint32_t value_len);

typedef sw_error_t
    (*hsl_reg_entries_set) (a_uint32_t dev_id, a_uint32_t reg_addr,
                          a_uint32_t entry_len, const a_uint8_t value[],
                          a_uint32_t value_len);

typedef struct {
    /* Misc */
    hsl_arp_status_set arp_status_set;
    hsl_arp_status_get arp_status_get;
    hsl_frame_max_size_set frame_max_size_set;
    hsl_frame_max_size_get frame_max_size_get;
    hsl_port_unk_sa_cmd_set port_unk_sa_cmd_set;
    hsl_port_unk_sa_cmd_get port_unk_sa_cmd_get;
    hsl_port_unk_uc_filter_set port_unk_uc_filter_set;
    hsl_port_unk_uc_filter_get port_unk_uc_filter_get;
    hsl_port_unk_mc_filter_set port_unk_mc_filter_set;
    hsl_port_unk_mc_filter_get port_unk_mc_filter_get;
    hsl_port_bc_filter_set port_bc_filter_set;
    hsl_port_bc_filter_get port_bc_filter_get;
    hsl_nestvlan_tpid_set nestvlan_tpid_set;
    hsl_nestvlan_tpid_get nestvlan_tpid_get;
    hsl_cpu_port_status_set cpu_port_status_set;
    hsl_cpu_port_status_get cpu_port_status_get;
    hsl_bc_to_cpu_port_set bc_to_cpu_port_set;
    hsl_bc_to_cpu_port_get bc_to_cpu_port_get;
	hsl_pppoe_cmd_set pppoe_cmd_set;
	hsl_pppoe_cmd_get pppoe_cmd_get;
	hsl_pppoe_status_set pppoe_status_set;
	hsl_pppoe_status_get pppoe_status_get;
	hsl_port_dhcp_set port_dhcp_set;
    hsl_port_dhcp_get port_dhcp_get;
    hsl_arp_cmd_set arp_cmd_set;
    hsl_arp_cmd_get arp_cmd_get;
    hsl_eapol_cmd_set eapol_cmd_set;
    hsl_eapol_cmd_get eapol_cmd_get;
    hsl_pppoe_session_add pppoe_session_add;
    hsl_pppoe_session_del pppoe_session_del;
    hsl_pppoe_session_get pppoe_session_get;
    hsl_eapol_status_set eapol_status_set;
    hsl_eapol_status_get eapol_status_get;
    hsl_ripv1_status_set ripv1_status_set;
    hsl_ripv1_status_get ripv1_status_get;
    
    /* Port control */
    hsl_port_duplex_set port_duplex_set;
    hsl_port_duplex_get port_duplex_get;
    hsl_port_speed_set port_speed_set;
    hsl_port_speed_get port_speed_get;
    hsl_port_autoneg_status_get port_autoneg_status_get;
    hsl_port_autoneg_enable port_autoneg_enable;
    hsl_port_autoneg_restart port_autoneg_restart;
    hsl_port_autoneg_adv_get port_autoneg_adv_get;
    hsl_port_autoneg_adv_set port_autoneg_adv_set;    
    hsl_port_hdr_status_set port_hdr_status_set;
    hsl_port_hdr_status_get port_hdr_status_get;    
    hsl_port_flowctrl_set port_flowctrl_set;
    hsl_port_flowctrl_get port_flowctrl_get;
    hsl_port_flowctrl_forcemode_set port_flowctrl_forcemode_set;
    hsl_port_flowctrl_forcemode_get port_flowctrl_forcemode_get;

    hsl_port_powersave_set port_powersave_set;
    hsl_port_powersave_get port_powersave_get;
    hsl_port_hibernate_set port_hibernate_set;
    hsl_port_hibernate_get port_hibernate_get; 
    hsl_port_cdt port_cdt;
    
    /* VLAN */
    hsl_vlan_entry_append vlan_entry_append;
    hsl_vlan_create vlan_creat;
    hsl_vlan_member_update vlan_member_update;
    hsl_vlan_delete vlan_delete;
    hsl_vlan_find vlan_find;
    hsl_vlan_next vlan_next;
    hsl_vlan_flush vlan_flush;

    /* Port VLAN */
    hsl_port_1qmode_set port_1qmode_set;
    hsl_port_1qmode_get port_1qmode_get;
    hsl_port_egvlanmode_get port_egvlanmode_get;
    hsl_port_egvlanmode_set port_egvlanmode_set;
    hsl_portvlan_member_add portvlan_member_add;
    hsl_portvlan_member_del portvlan_member_del;
    hsl_portvlan_member_update portvlan_member_update;
    hsl_portvlan_member_get portvlan_member_get;
    hsl_port_default_vid_set port_default_vid_set;
    hsl_port_default_vid_get port_default_vid_get;
    hsl_port_force_default_vid_set port_force_default_vid_set;
    hsl_port_force_default_vid_get port_force_default_vid_get;
    hsl_port_force_portvlan_set port_force_portvlan_set;
    hsl_port_force_portvlan_get port_force_portvlan_get;
    hsl_port_nestvlan_set port_nestvlan_set;
    hsl_port_nestvlan_get port_nestvlan_get;
    hsl_port_invlan_mode_set port_invlan_mode_set;
    hsl_port_invlan_mode_get port_invlan_mode_get;
    hsl_port_tls_set port_tls_set;
    hsl_port_tls_get port_tls_get;
    hsl_port_pri_propagation_set port_pri_propagation_set;
    hsl_port_pri_propagation_get port_pri_propagation_get;
    hsl_port_default_svid_set port_default_svid_set;
    hsl_port_default_svid_get port_default_svid_get;
    hsl_port_default_cvid_set port_default_cvid_set;
    hsl_port_default_cvid_get port_default_cvid_get;
    hsl_port_vlan_propagation_set port_vlan_propagation_set;
    hsl_port_vlan_propagation_get port_vlan_propagation_get;
    hsl_port_vlan_trans_add port_vlan_trans_add;
    hsl_port_vlan_trans_del port_vlan_trans_del;
    hsl_port_vlan_trans_get port_vlan_trans_get;
    hsl_qinq_mode_set qinq_mode_set;
    hsl_qinq_mode_get qinq_mode_get;
    hsl_port_qinq_role_set port_qinq_role_set;
    hsl_port_qinq_role_get port_qinq_role_get;
    hsl_port_vlan_trans_iterate port_vlan_trans_iterate;

    /* FDB */
    hsl_fdb_add fdb_add;
    hsl_fdb_del_all fdb_del_all;
    hsl_fdb_del_by_port fdb_del_by_port;
    hsl_fdb_del_by_mac fdb_del_by_mac;
    hsl_fdb_first fdb_first;
    hsl_fdb_next fdb_next;
    hsl_fdb_find fdb_find;
    hsl_fdb_port_learn_set port_learn_set;
    hsl_fdb_port_learn_get port_learn_get;
    hsl_fdb_age_ctrl_set age_ctrl_set;
    hsl_fdb_age_ctrl_get age_ctrl_get;
    hsl_fdb_age_time_set age_time_set;
    hsl_fdb_age_time_get age_time_get;
    hsl_fdb_iterate      fdb_iterate;
      
    /* QOS */
    hsl_qos_sch_mode_set qos_sch_mode_set;
    hsl_qos_sch_mode_get qos_sch_mode_get;
    hsl_qos_queue_tx_buf_status_set qos_queue_tx_buf_status_set;
    hsl_qos_queue_tx_buf_status_get qos_queue_tx_buf_status_get;
    hsl_qos_port_tx_buf_status_set qos_port_tx_buf_status_set;
    hsl_qos_port_tx_buf_status_get qos_port_tx_buf_status_get; 
    hsl_qos_queue_tx_buf_nr_set qos_queue_tx_buf_nr_set;
    hsl_qos_queue_tx_buf_nr_get qos_queue_tx_buf_nr_get;
    hsl_qos_port_tx_buf_nr_set qos_port_tx_buf_nr_set;
    hsl_qos_port_tx_buf_nr_get qos_port_tx_buf_nr_get;
    hsl_qos_port_rx_buf_nr_set qos_port_rx_buf_nr_set;
    hsl_qos_port_rx_buf_nr_get qos_port_rx_buf_nr_get; 
    hsl_cosmap_up_queue_set cosmap_up_queue_set;
    hsl_cosmap_up_queue_get cosmap_up_queue_get;
    hsl_cosmap_dscp_queue_set cosmap_dscp_queue_set;
    hsl_cosmap_dscp_queue_get cosmap_dscp_queue_get;
    hsl_qos_port_mode_set qos_port_mode_set;
    hsl_qos_port_mode_get qos_port_mode_get;
    hsl_qos_port_mode_pri_set qos_port_mode_pri_set;
    hsl_qos_port_mode_pri_get qos_port_mode_pri_get;
    hsl_qos_port_default_up_set qos_port_default_up_set;
    hsl_qos_port_default_up_get qos_port_default_up_get;
    hsl_qos_port_sch_mode_set qos_port_sch_mode_set;
    hsl_qos_port_sch_mode_get qos_port_sch_mode_get;

    /* Rate */
    hsl_storm_ctrl_frame_set storm_ctrl_frame_set;
    hsl_storm_ctrl_frame_get storm_ctrl_frame_get;
    hsl_storm_ctrl_rate_set storm_ctrl_rate_set;
    hsl_storm_ctrl_rate_get storm_ctrl_rate_get;
    hsl_rate_queue_egrl_set rate_queue_egrl_set;
    hsl_rate_queue_egrl_get rate_queue_egrl_get;
    hsl_rate_port_egrl_set rate_port_egrl_set;
    hsl_rate_port_egrl_get rate_port_egrl_get;
    hsl_rate_port_inrl_set rate_port_inrl_set;
    hsl_rate_port_inrl_get rate_port_inrl_get;

    /* Mirror */
    hsl_mirr_analysis_port_set mirr_analysis_port_set;
    hsl_mirr_analysis_port_get mirr_analysis_port_get;
    hsl_mirr_port_in_set mirr_port_in_set;
    hsl_mirr_port_in_get mirr_port_in_get;
    hsl_mirr_port_eg_set mirr_port_eg_set;
    hsl_mirr_port_eg_get mirr_port_eg_get;

    /* Stp */
    hsl_stp_port_state_set stp_port_state_set;
    hsl_stp_port_state_get stp_port_state_get;

    /* IGMP */
    hsl_port_igmps_status_set port_igmps_status_set;
    hsl_port_igmps_status_get port_igmps_status_get;
    hsl_igmp_mld_cmd_set igmp_mld_cmd_set;
    hsl_igmp_mld_cmd_get igmp_mld_cmd_get;
    hsl_port_igmp_join_set port_igmp_join_set;
    hsl_port_igmp_join_get port_igmp_join_get;
    hsl_port_igmp_leave_set port_igmp_leave_set;
    hsl_port_igmp_leave_get port_igmp_leave_get;
    hsl_igmp_rp_set igmp_rp_set;
    hsl_igmp_rp_get igmp_rp_get;
    hsl_igmp_entry_creat_set igmp_entry_creat_set;
    hsl_igmp_entry_creat_get igmp_entry_creat_get;
    hsl_igmp_entry_static_set igmp_entry_static_set;
    hsl_igmp_entry_static_get igmp_entry_static_get;
    hsl_igmp_entry_leaky_set igmp_entry_leaky_set;
    hsl_igmp_entry_leaky_get igmp_entry_leaky_get;
    hsl_igmp_entry_v3_set igmp_entry_v3_set;
    hsl_igmp_entry_v3_get igmp_entry_v3_get;
    hsl_igmp_entry_queue_set igmp_entry_queue_set;
    hsl_igmp_entry_queue_get igmp_entry_queue_get;

    /* Leaky */
    hsl_uc_leaky_mode_set uc_leaky_mode_set;
    hsl_uc_leaky_mode_get uc_leaky_mode_get;
    hsl_mc_leaky_mode_set mc_leaky_mode_set;
    hsl_mc_leaky_mode_get mc_leaky_mode_get;
    hsl_port_arp_leaky_set port_arp_leaky_set;
    hsl_port_arp_leaky_get port_arp_leaky_get;
    hsl_port_uc_leaky_set port_uc_leaky_set;
    hsl_port_uc_leaky_get port_uc_leaky_get;
    hsl_port_mc_leaky_set port_mc_leaky_set;
    hsl_port_mc_leaky_get port_mc_leaky_get;

    /* MIB API */
    hsl_get_mib_info get_mib_info;
    hsl_mib_status_set mib_status_set;
    hsl_mib_status_get mib_status_get;

    /* Acl */
    hsl_acl_list_creat     acl_list_creat;
    hsl_acl_list_destroy   acl_list_destroy;
    hsl_acl_rule_add       acl_rule_add;
    hsl_acl_rule_delete    acl_rule_delete;
    hsl_acl_rule_query     acl_rule_query;
    hsl_acl_list_bind      acl_list_bind;
    hsl_acl_list_unbind    acl_list_unbind;
    hsl_acl_status_set     acl_status_set;
    hsl_acl_status_get     acl_status_get;
    hsl_acl_list_dump      acl_list_dump;
    hsl_acl_rule_dump      acl_rule_dump;

    /* LED */
    hsl_led_ctrl_pattern_set   led_ctrl_pattern_set;
    hsl_led_ctrl_pattern_get   led_ctrl_pattern_get;

    /* REG */
    hsl_phy_get phy_get;
    hsl_phy_set phy_set;
    hsl_reg_get reg_get;
    hsl_reg_set reg_set;
    hsl_reg_field_get reg_field_get;
    hsl_reg_field_set reg_field_set;
    hsl_reg_entries_get reg_entries_get;
    hsl_reg_entries_set reg_entries_set;

    /*INIT*/
    hsl_dev_reset          dev_reset;
} hsl_api_t;

hsl_api_t *
hsl_api_ptr_get(a_uint32_t dev_id);

sw_error_t
hsl_api_init(a_uint32_t dev_id);

#if defined(SW_API_LOCK) && (!defined(HSL_STANDALONG))
extern  aos_lock_t sw_hsl_api_lock;
#define FAL_API_LOCK    aos_lock(&sw_hsl_api_lock)
#define FAL_API_UNLOCK  aos_unlock(&sw_hsl_api_lock)
#else
#define FAL_API_LOCK
#define FAL_API_UNLOCK
#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif                          /* _SW_API_H */
