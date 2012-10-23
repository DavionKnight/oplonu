#ifndef _OPCONN_ATHEROS_H_
#define _OPCONN_ATHEROS_H_		1

typedef unsigned char    a_uint8_t;
typedef unsigned short   a_uint16_t;
typedef unsigned int   a_uint32_t;
typedef int   a_int32_t;

typedef a_uint32_t fal_port_t;
typedef a_uint32_t fal_pbmp_t;

#define SW_MAX_NR_DEV      1
#define SW_MAX_NR_PORT     32

#define HSL_LOCAL
#define HSL_RW 1
#define HSL_RO 0
#define SHIVA
#define HSL_STANDALONG
#define USER_MODE


typedef enum {
    HSL_MDIO = 1,
    HSL_HEADER,
} hsl_access_mode;

typedef enum {
    HSL_NO_CPU = 0,
    HSL_CPU_1,
    HSL_CPU_2,
    HSL_CPU_1_PLUS,
} hsl_init_mode;

typedef enum {
    A_FALSE = 0,
    A_TRUE           
}a_bool_t;

typedef enum {
    SW_OK              = 0,       /* Operation succeeded                 */
    SW_FAIL            = -1,      /* Operation failed                    */
    SW_BAD_VALUE       = -2,      /* Illegal value                       */
    SW_OUT_OF_RANGE    = -3,      /* Value is out of range               */
    SW_BAD_PARAM       = -4,      /* Illegal parameter(s)                */
    SW_BAD_PTR         = -5,      /* Illegal pointer value               */
    SW_BAD_LEN         = -6,      /* Wrong length                        */
    SW_BAD_STATE       = -7,      /* Wrong state of state machine        */
    SW_READ_ERROR      = -8,      /* Read operation failed               */
    SW_WRITE_ERROR     = -9,      /* Write operation failed              */
    SW_CREATE_ERROR    = -10,     /* Fail in creating an entry           */
    SW_DELETE_ERROR    = -11,     /* Fail in deleteing an entry          */  
    SW_NOT_FOUND       = -12,     /* Entry not found                     */
    SW_NO_CHANGE       = -13,     /* The parameter(s) is the same        */
    SW_NO_MORE         = -14,     /* No more entry found                 */
    SW_NO_SUCH         = -15,     /* No such entry                       */
    SW_ALREADY_EXIST   = -16,     /* Tried to create existing entry      */
    SW_FULL            = -17,     /* Table is full                       */
    SW_EMPTY           = -18,     /* Table is empty                      */
    SW_NOT_SUPPORTED   = -19,     /* This request is not support         */
    SW_NOT_IMPLEMENTED = -20,     /* This request is not implemented     */
    SW_NOT_INITIALIZED = -21,     /* The item is not initialized         */
    SW_BUSY            = -22,     /* Operation is still running          */
    SW_TIMEOUT         = -23,     /* Operation Time Out                  */
    SW_DISABLE         = -24,     /* Operation is disabled               */
    SW_NO_RESOURCE     = -25,     /* Resource not available (memory ...) */
    SW_INIT_ERROR      = -26,     /* Error occured while INIT process    */
    SW_NOT_READY       = -27,     /* The other side is not ready yet     */
    SW_OUT_OF_MEM      = -28,     /* Cpu memory allocation failed.       */
    SW_ABORTED         = -29      /* Operation has been aborted.         */
} sw_error_t;                    

typedef sw_error_t
    (*hsl_phy_get) (a_uint32_t dev_id, a_uint32_t phy_addr, a_uint8_t reg,
                    a_uint16_t * value);

typedef sw_error_t
    (*hsl_phy_set) (a_uint32_t dev_id, a_uint32_t phy_addr, a_uint8_t reg,
                    a_uint16_t value);

typedef sw_error_t
    (*hsl_reg_get) (a_uint32_t dev_id, a_uint32_t reg_addr,
                    a_uint8_t value[], a_uint8_t value_len);

typedef sw_error_t
    (*hsl_reg_set) (a_uint32_t dev_id, a_uint32_t reg_addr,
                    a_uint8_t value[], a_uint8_t value_len);

typedef sw_error_t
    (*hsl_reg_entry_get) (a_uint32_t dev_id, a_uint32_t reg_addr,
                          a_uint32_t entry_offset, a_uint16_t entry_len,
                          a_uint8_t value[], a_uint8_t value_len);


typedef sw_error_t
    (*hsl_reg_entry_set) (a_uint32_t dev_id, a_uint32_t reg_addr,
                          a_uint32_t entry_offset, a_uint16_t entry_len,
                          const a_uint8_t value[], a_uint8_t value_len);

typedef sw_error_t
    (*hsl_reg_field_get) (a_uint32_t dev_id, a_uint32_t reg_addr,
                          a_uint32_t entry_offset, a_uint16_t bit_offset,
                          a_uint16_t field_len, a_uint8_t value[],
                          a_uint8_t value_len);

typedef sw_error_t
    (*hsl_reg_field_set) (a_uint32_t dev_id, a_uint32_t reg_addr,
                          a_uint32_t entry_offset, a_uint16_t bit_offset,
                          a_uint16_t field_len, const a_uint8_t value[],
                          a_uint8_t value_len);

typedef sw_error_t
    (*hsl_reg_access_mode_set) (a_uint32_t dev_id, hsl_access_mode mode);



typedef struct {
    hsl_access_mode reg_access_mode;
    hsl_phy_get phy_get;
    hsl_phy_set phy_set;
    hsl_reg_get reg_get;
    hsl_reg_set reg_set;
    hsl_reg_entry_get reg_entry_get;
    hsl_reg_entry_set reg_entry_set;
    hsl_reg_field_get reg_field_get;
    hsl_reg_field_set reg_field_set;
    hsl_reg_access_mode_set reg_access_mode_set;
} hsl_reg_func_t;


typedef enum {
    FAL_SPEED_10    = 10,
    FAL_SPEED_100   = 100,
    FAL_SPEED_1000  = 1000,
    FAL_SPEED_10000 = 10000,
    FAL_SPEED_BUTT  = 0xffff,
} fal_port_speed_t;

typedef sw_error_t
    (*hsl_port_speed_set) (a_uint32_t dev_id, fal_port_t port_id,
                           fal_port_speed_t speed);


typedef struct {
    
    /* Port control */
    hsl_port_speed_set port_speed_set;

} hsl_api_t;


#define def_init_cfg  {.reg_mode = HSL_MDIO, .cpu_mode = HSL_CPU_1};


typedef struct {
    a_uint32_t dev_id;
    a_uint8_t cpu_port_nr;
    a_uint8_t nr_ports;
    a_uint8_t nr_phy;
    a_uint8_t nr_queue;
    a_uint16_t nr_vlans;
    a_bool_t hw_vlan_query;
    hsl_reg_func_t dev_func;
    hsl_api_t dev_api;
    hsl_init_mode  cpu_mode;
} hsl_dev_t;


typedef enum {
    HSL_PP_PHY = 0,    /* setting concerning phy */
    HSL_PP_INCL_CPU,   /* setting may include cpu port */
    HSL_PP_EXCL_CPU,   /* setting exclude cpu port */
    HSL_PP_BUTT
} hsl_port_prop_t;

/**
@brief This enum defines several forwarding command type.
* Field description:
    FAL_MAC_FRWRD      - packets are normally forwarded
    FAL_MAC_DROP       - packets are dropped
    FAL_MAC_CPY_TO_CPU - packets are copyed to cpu
    FAL_MAC_RDT_TO_CPU - packets are redirected to cpu
*/
typedef enum {
    FAL_MAC_FRWRD = 0,		/**<   packets are normally forwarded */
    FAL_MAC_DROP,			/**<   packets are dropped */
    FAL_MAC_CPY_TO_CPU,		/**<   packets are copyed to cpu */
    FAL_MAC_RDT_TO_CPU		/**<   packets are redirected to cpu */
} fal_fwd_cmd_t;

typedef struct {
    a_uint32_t phy_id[SW_MAX_NR_PORT];
    fal_pbmp_t dev_portmap;
    fal_pbmp_t property[HSL_PP_BUTT];
} port_info_t;


typedef sw_error_t
    (*mdio_reg_set) (a_uint8_t phy_addr, a_uint8_t reg, a_uint16_t data);

typedef sw_error_t
    (*mdio_reg_get) (a_uint8_t phy_addr, a_uint8_t reg, a_uint16_t *data);

typedef sw_error_t
    (*hdr_reg_set) (a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t *reg_data, a_uint8_t len);

typedef sw_error_t
    (*hdr_reg_get) (a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t *reg_data, a_uint8_t len);

typedef struct {
    mdio_reg_set    mdio_set;
    mdio_reg_get    mdio_get;
    hdr_reg_set     header_reg_set;
    hdr_reg_get     header_reg_get;
} hsl_reg_func;


typedef struct {
    hsl_init_mode   cpu_mode;
    hsl_access_mode reg_mode;
    hsl_reg_func    reg_func;

    /* os specific parameter */
    /* netlink protocol type for linux user-kernel space communication */
    a_uint32_t      nl_prot;

    /* chip specific parameter */
#if defined ATHENA
#elif defined GARUDA
    a_bool_t  mac0_rgmii;
    a_bool_t  mac5_rgmii;
    a_bool_t  rx_delay_s0;
    a_bool_t  rx_delay_s1;
    a_bool_t  tx_delay_s0;
    a_bool_t  tx_delay_s1;
    a_bool_t  rgmii_rxclk_delay;
    a_bool_t  rgmii_txclk_delay;
    a_bool_t  phy4_rx_delay;
    a_bool_t  phy4_tx_delay;
#elif defined SHIVA
#endif
} ssdk_init_cfg;

/* Port Control Register */
#define PORT_CTL                  "pctl"
#define PORT_CTL_ID               30
#define PORT_CTL_OFFSET           0x0104
#define PORT_CTL_E_LENGTH         4
#define PORT_CTL_E_OFFSET         0x0100
#define PORT_CTL_NR_E             7

#define HEAD_EN                                 "pctl_headen"
#define PORT_CTL_HEAD_EN_BOFFSET                11
#define PORT_CTL_HEAD_EN_BLEN                   1
#define PORT_CTL_HEAD_EN_FLAG                   HSL_RW

/* Cpu Port Register */
#define CPU_PORT                  "cpup"
#define CPU_PORT_ID               20
#define CPU_PORT_OFFSET           0x0078
#define CPU_PORT_E_LENGTH         4
#define CPU_PORT_E_OFFSET         0
#define CPU_PORT_NR_E             0

#define CPU_PORT_EN                           "cpup_cpupe"
#define CPU_PORT_CPU_PORT_EN_BOFFSET          8
#define CPU_PORT_CPU_PORT_EN_BLEN             1
#define CPU_PORT_CPU_PORT_EN_FLAG             HSL_RW

/* MIB Function Register */
#define MIB_FUNC                  "mibfunc"
#define MIB_FUNC_ID               21
#define MIB_FUNC_OFFSET           0x0080
#define MIB_FUNC_E_LENGTH         4
#define MIB_FUNC_E_OFFSET         0
#define MIB_FUNC_NR_E             1

#define MIB_EN                               "mib_en"
#define MIB_FUNC_MIB_EN_BOFFSET              30
#define MIB_FUNC_MIB_EN_BLEN                 1
#define MIB_FUNC_MIB_EN_FLAG                 HSL_RW


/**@brief This structure defines the mib infomation.
*/
typedef struct {
    a_uint32_t RxBroad;
    a_uint32_t RxPause;
    a_uint32_t RxMulti;
    a_uint32_t RxFcsErr;
    a_uint32_t RxAllignErr;
    a_uint32_t RxRunt;
    a_uint32_t RxFragment;
    a_uint32_t Rx64Byte;
    a_uint32_t Rx128Byte;
    a_uint32_t Rx256Byte;
    a_uint32_t Rx512Byte;
    a_uint32_t Rx1024Byte;
    a_uint32_t Rx1518Byte;
    a_uint32_t RxMaxByte;
    a_uint32_t RxTooLong;
    a_uint32_t RxGoodByte_lo; 		/**<  low 32 bits of RxGoodByte statistc item */
    a_uint32_t RxGoodByte_hi; 		/**<   high 32 bits of RxGoodByte statistc item*/
    a_uint32_t RxBadByte_lo; 		/**<   low 32 bits of RxBadByte statistc item */
    a_uint32_t RxBadByte_hi; 		/**<   high 32 bits of RxBadByte statistc item */
    a_uint32_t RxOverFlow;
    a_uint32_t Filtered;
    a_uint32_t TxBroad;
    a_uint32_t TxPause;
    a_uint32_t TxMulti;
    a_uint32_t TxUnderRun;
    a_uint32_t Tx64Byte;
    a_uint32_t Tx128Byte;
    a_uint32_t Tx256Byte;
    a_uint32_t Tx512Byte;
    a_uint32_t Tx1024Byte;
    a_uint32_t Tx1518Byte;
    a_uint32_t TxMaxByte;
    a_uint32_t TxOverSize;
    a_uint32_t TxByte_lo; 		/**<  low 32 bits of TxByte statistc item */
    a_uint32_t TxByte_hi; 		/**<  high 32 bits of TxByte statistc item */
    a_uint32_t TxCollision;
    a_uint32_t TxAbortCol;
    a_uint32_t TxMultiCol;
    a_uint32_t TxSingalCol;
    a_uint32_t TxExcDefer;
    a_uint32_t TxDefer;
    a_uint32_t TxLateCol;
}fal_mib_info_t;

/* mib memory info */
#define MIB_RXBROAD                       "RxBroad"
#define MIB_RXBROAD_ID                    34
#define MIB_RXBROAD_OFFSET                0x20000
#define MIB_RXBROAD_E_LENGTH              4
#define MIB_RXBROAD_E_OFFSET              0x100
#define MIB_RXBROAD_NR_E                  6

#define MIB_RXPAUSE                       "RxPause"
#define MIB_RXPAUSE_ID                    35
#define MIB_RXPAUSE_OFFSET                0x20004
#define MIB_RXPAUSE_E_LENGTH              4
#define MIB_RXPAUSE_E_OFFSET              0x100
#define MIB_RXPAUSE_NR_E                  6

#define MIB_RXMULTI                       "RxMulti"
#define MIB_RXMULTI_ID                    36
#define MIB_RXMULTI_OFFSET                0x20008
#define MIB_RXMULTI_E_LENGTH              4
#define MIB_RXMULTI_E_OFFSET              0x100
#define MIB_RXMULTI_NR_E                  6

#define MIB_RXFCSERR                      "RxFcsErr"
#define MIB_RXFCSERR_ID                   37
#define MIB_RXFCSERR_OFFSET               0x2000c
#define MIB_RXFCSERR_E_LENGTH             4
#define MIB_RXFCSERR_E_OFFSET             0x100
#define MIB_RXFCSERR_NR_E                 6

#define MIB_RXALLIGNERR                   "RxAllignErr"
#define MIB_RXALLIGNERR_ID                38
#define MIB_RXALLIGNERR_OFFSET            0x20010
#define MIB_RXALLIGNERR_E_LENGTH          4
#define MIB_RXALLIGNERR_E_OFFSET          0x100
#define MIB_RXALLIGNERR_NR_E              6

#define MIB_RXRUNT                        "RxRunt"
#define MIB_RXRUNT_ID                     39
#define MIB_RXRUNT_OFFSET                 0x20014
#define MIB_RXRUNT_E_LENGTH               4
#define MIB_RXRUNT_E_OFFSET               0x100
#define MIB_RXRUNT_NR_E                   6

#define MIB_RXFRAGMENT                    "RxFragment"
#define MIB_RXFRAGMENT_ID                 40
#define MIB_RXFRAGMENT_OFFSET             0x20018
#define MIB_RXFRAGMENT_E_LENGTH           4
#define MIB_RXFRAGMENT_E_OFFSET           0x100
#define MIB_RXFRAGMENT_NR_E               6

#define MIB_RX64BYTE                      "Rx64Byte"
#define MIB_RX64BYTE_ID                   41
#define MIB_RX64BYTE_OFFSET               0x2001c
#define MIB_RX64BYTE_E_LENGTH             4
#define MIB_RX64BYTE_E_OFFSET             0x100
#define MIB_RX64BYTE_NR_E                 6

#define MIB_RX128BYTE                     "Rx128Byte"
#define MIB_RX128BYTE_ID                  42
#define MIB_RX128BYTE_OFFSET              0x20020
#define MIB_RX128BYTE_E_LENGTH            4
#define MIB_RX128BYTE_E_OFFSET            0x100
#define MIB_RX128BYTE_NR_E                6

#define MIB_RX256BYTE                     "Rx256Byte"
#define MIB_RX256BYTE_ID                  43
#define MIB_RX256BYTE_OFFSET              0x20024
#define MIB_RX256BYTE_E_LENGTH            4
#define MIB_RX256BYTE_E_OFFSET            0x100
#define MIB_RX256BYTE_NR_E                6

#define MIB_RX512BYTE                     "Rx512Byte"
#define MIB_RX512BYTE_ID                  44
#define MIB_RX512BYTE_OFFSET              0x20028
#define MIB_RX512BYTE_E_LENGTH            4
#define MIB_RX512BYTE_E_OFFSET            0x100
#define MIB_RX512BYTE_NR_E                6

#define MIB_RX1024BYTE                    "Rx1024Byte"
#define MIB_RX1024BYTE_ID                 45
#define MIB_RX1024BYTE_OFFSET             0x2002c
#define MIB_RX1024BYTE_E_LENGTH           4
#define MIB_RX1024BYTE_E_OFFSET           0x100
#define MIB_RX1024BYTE_NR_E               6

#define MIB_RX1518BYTE                    "Rx1518Byte"
#define MIB_RX1518BYTE_ID                 45
#define MIB_RX1518BYTE_OFFSET             0x20030
#define MIB_RX1518BYTE_E_LENGTH           4
#define MIB_RX1518BYTE_E_OFFSET           0x100
#define MIB_RX1518BYTE_NR_E               6

#define MIB_RXMAXBYTE                     "RxMaxByte"
#define MIB_RXMAXBYTE_ID                  46
#define MIB_RXMAXBYTE_OFFSET              0x20034
#define MIB_RXMAXBYTE_E_LENGTH            4
#define MIB_RXMAXBYTE_E_OFFSET            0x100
#define MIB_RXMAXBYTE_NR_E                6

#define MIB_RXTOOLONG                     "RxTooLong"
#define MIB_RXTOOLONG_ID                  47
#define MIB_RXTOOLONG_OFFSET              0x20038
#define MIB_RXTOOLONG_E_LENGTH            4
#define MIB_RXTOOLONG_E_OFFSET            0x100
#define MIB_RXTOOLONG_NR_E                6

#define MIB_RXGOODBYTE_LO                 "RxGoodByteLo"
#define MIB_RXGOODBYTE_LO_ID              48
#define MIB_RXGOODBYTE_LO_OFFSET          0x2003c
#define MIB_RXGOODBYTE_LO_E_LENGTH        4
#define MIB_RXGOODBYTE_LO_E_OFFSET        0x100
#define MIB_RXGOODBYTE_LO_NR_E            6

#define MIB_RXGOODBYTE_HI                 "RxGoodByteHi"
#define MIB_RXGOODBYTE_HI_ID              49
#define MIB_RXGOODBYTE_HI_OFFSET          0x20040
#define MIB_RXGOODBYTE_HI_E_LENGTH        4
#define MIB_RXGOODBYTE_HI_E_OFFSET        0x100
#define MIB_RXGOODBYTE_HI_NR_E            6

#define MIB_RXBADBYTE_LO                  "RxBadByteLo"
#define MIB_RXBADBYTE_LO_ID               50
#define MIB_RXBADBYTE_LO_OFFSET           0x20044
#define MIB_RXBADBYTE_LO_E_LENGTH         4
#define MIB_RXBADBYTE_LO_E_OFFSET         0x100
#define MIB_RXBADBYTE_LO_NR_E             6

#define MIB_RXBADBYTE_HI                  "RxBadByteHi"
#define MIB_RXBADBYTE_HI_ID               51
#define MIB_RXBADBYTE_HI_OFFSET           0x20048
#define MIB_RXBADBYTE_HI_E_LENGTH         4
#define MIB_RXBADBYTE_HI_E_OFFSET         0x100
#define MIB_RXBADBYTE_HI_NR_E             6

#define MIB_RXOVERFLOW                    "RxOverFlow"
#define MIB_RXOVERFLOW_ID                 52
#define MIB_RXOVERFLOW_OFFSET             0x2004c
#define MIB_RXOVERFLOW_E_LENGTH           4
#define MIB_RXOVERFLOW_E_OFFSET           0x100
#define MIB_RXOVERFLOW_NR_E               6

#define MIB_FILTERED                      "Filtered"
#define MIB_FILTERED_ID                   53
#define MIB_FILTERED_OFFSET               0x20050
#define MIB_FILTERED_E_LENGTH             4
#define MIB_FILTERED_E_OFFSET             0x100
#define MIB_FILTERED_NR_E                 6

#define MIB_TXBROAD                       "TxBroad"
#define MIB_TXBROAD_ID                    54
#define MIB_TXBROAD_OFFSET                0x20054
#define MIB_TXBROAD_E_LENGTH              4
#define MIB_TXBROAD_E_OFFSET              0x100
#define MIB_TXBROAD_NR_E                  6

#define MIB_TXPAUSE                       "TxPause"
#define MIB_TXPAUSE_ID                    55
#define MIB_TXPAUSE_OFFSET                0x20058
#define MIB_TXPAUSE_E_LENGTH              4
#define MIB_TXPAUSE_E_OFFSET              0x100
#define MIB_TXPAUSE_NR_E                  6

#define MIB_TXMULTI                       "TxMulti"
#define MIB_TXMULTI_ID                    56
#define MIB_TXMULTI_OFFSET                0x2005c
#define MIB_TXMULTI_E_LENGTH              4
#define MIB_TXMULTI_E_OFFSET              0x100
#define MIB_TXMULTI_NR_E                  6

#define MIB_TXUNDERRUN                    "TxUnderRun"
#define MIB_TXUNDERRUN_ID                 57
#define MIB_TXUNDERRUN_OFFSET             0x20060
#define MIB_TXUNDERRUN_E_LENGTH           4
#define MIB_TXUNDERRUN_E_OFFSET           0x100
#define MIB_TXUNDERRUN_NR_E               6

#define MIB_TX64BYTE                      "Tx64Byte"
#define MIB_TX64BYTE_ID                   58
#define MIB_TX64BYTE_OFFSET               0x20064
#define MIB_TX64BYTE_E_LENGTH             4
#define MIB_TX64BYTE_E_OFFSET             0x100
#define MIB_TX64BYTE_NR_E                 6

#define MIB_TX128BYTE                     "Tx128Byte"
#define MIB_TX128BYTE_ID                  59
#define MIB_TX128BYTE_OFFSET              0x20068
#define MIB_TX128BYTE_E_LENGTH            4
#define MIB_TX128BYTE_E_OFFSET            0x100
#define MIB_TX128BYTE_NR_E                6

#define MIB_TX256BYTE                     "Tx256Byte"
#define MIB_TX256BYTE_ID                  60
#define MIB_TX256BYTE_OFFSET              0x2006c
#define MIB_TX256BYTE_E_LENGTH            4
#define MIB_TX256BYTE_E_OFFSET            0x100
#define MIB_TX256BYTE_NR_E                6

#define MIB_TX512BYTE                     "Tx512Byte"
#define MIB_TX512BYTE_ID                  61
#define MIB_TX512BYTE_OFFSET              0x20070
#define MIB_TX512BYTE_E_LENGTH            4
#define MIB_TX512BYTE_E_OFFSET            0x100
#define MIB_TX512BYTE_NR_E                6

#define MIB_TX1024BYTE                    "Tx1024Byte"
#define MIB_TX1024BYTE_ID                 62
#define MIB_TX1024BYTE_OFFSET             0x20074
#define MIB_TX1024BYTE_E_LENGTH           4
#define MIB_TX1024BYTE_E_OFFSET           0x100
#define MIB_TX1024BYTE_NR_E               6

#define MIB_TX1518BYTE                    "Tx1518Byte"
#define MIB_TX1518BYTE_ID                 62
#define MIB_TX1518BYTE_OFFSET             0x20078
#define MIB_TX1518BYTE_E_LENGTH           4
#define MIB_TX1518BYTE_E_OFFSET           0x100
#define MIB_TX1518BYTE_NR_E               6

#define MIB_TXMAXBYTE                     "TxMaxByte"
#define MIB_TXMAXBYTE_ID                  63
#define MIB_TXMAXBYTE_OFFSET              0x2007c
#define MIB_TXMAXBYTE_E_LENGTH            4
#define MIB_TXMAXBYTE_E_OFFSET            0x100
#define MIB_TXMAXBYTE_NR_E                6

#define MIB_TXOVERSIZE                    "TxOverSize"
#define MIB_TXOVERSIZE_ID                 64
#define MIB_TXOVERSIZE_OFFSET             0x20080
#define MIB_TXOVERSIZE_E_LENGTH           4
#define MIB_TXOVERSIZE_E_OFFSET           0x100
#define MIB_TXOVERSIZE_NR_E               6

#define MIB_TXBYTE_LO                     "TxByteLo"
#define MIB_TXBYTE_LO_ID                  65
#define MIB_TXBYTE_LO_OFFSET              0x20084
#define MIB_TXBYTE_LO_E_LENGTH            4
#define MIB_TXBYTE_LO_E_OFFSET            0x100
#define MIB_TXBYTE_LO_NR_E                6

#define MIB_TXBYTE_HI                     "TxByteHi"
#define MIB_TXBYTE_HI_ID                  66
#define MIB_TXBYTE_HI_OFFSET              0x20088
#define MIB_TXBYTE_HI_E_LENGTH            4
#define MIB_TXBYTE_HI_E_OFFSET            0x100
#define MIB_TXBYTE_HI_NR_E                6

#define MIB_TXCOLLISION                   "TxCollision"
#define MIB_TXCOLLISION_ID                67
#define MIB_TXCOLLISION_OFFSET            0x2008c
#define MIB_TXCOLLISION_E_LENGTH          4
#define MIB_TXCOLLISION_E_OFFSET          0x100
#define MIB_TXCOLLISION_NR_E              6

#define MIB_TXABORTCOL                    "TxAbortCol"
#define MIB_TXABORTCOL_ID                 68
#define MIB_TXABORTCOL_OFFSET             0x20090
#define MIB_TXABORTCOL_E_LENGTH           4
#define MIB_TXABORTCOL_E_OFFSET           0x100
#define MIB_TXABORTCOL_NR_E               6

#define MIB_TXMULTICOL                    "TxMultiCol"
#define MIB_TXMULTICOL_ID                 69
#define MIB_TXMULTICOL_OFFSET             0x20094
#define MIB_TXMULTICOL_E_LENGTH           4
#define MIB_TXMULTICOL_E_OFFSET           0x100
#define MIB_TXMULTICOL_NR_E               6

#define MIB_TXSINGALCOL                   "TxSingalCol"
#define MIB_TXSINGALCOL_ID                70
#define MIB_TXSINGALCOL_OFFSET            0x20098
#define MIB_TXSINGALCOL_E_LENGTH          4
#define MIB_TXSINGALCOL_E_OFFSET          0x100
#define MIB_TXSINGALCOL_NR_E              6

#define MIB_TXEXCDEFER                    "TxExcDefer"
#define MIB_TXEXCDEFER_ID                 71
#define MIB_TXEXCDEFER_OFFSET             0x2009c
#define MIB_TXEXCDEFER_E_LENGTH           4
#define MIB_TXEXCDEFER_E_OFFSET           0x100
#define MIB_TXEXCDEFER_NR_E               6

#define MIB_TXDEFER                       "TxDefer"
#define MIB_TXDEFER_ID                    72
#define MIB_TXDEFER_OFFSET                0x200a0
#define MIB_TXDEFER_E_LENGTH              4
#define MIB_TXDEFER_E_OFFSET              0x100
#define MIB_TXDEFER_NR_E                  6

#define MIB_TXLATECOL                     "TxLateCol"
#define MIB_TXLATECOL_ID                  73
#define MIB_TXLATECOL_OFFSET              0x200a4
#define MIB_TXLATECOL_E_LENGTH            4
#define MIB_TXLATECOL_E_OFFSET            0x100
#define MIB_TXLATECOL_NR_E                6

/* Garuda Mask Control Register */
#define MASK_CTL                  "mask"
#define MASK_CTL_ID               0
#define MASK_CTL_OFFSET           0x0000
#define MASK_CTL_E_LENGTH         4
#define MASK_CTL_E_OFFSET         0
#define MASK_CTL_NR_E             1

#define SOFT_RST                              "mask_rst"
#define MASK_CTL_SOFT_RST_BOFFSET             31
#define MASK_CTL_SOFT_RST_BLEN                1
#define MASK_CTL_SOFT_RST_FLAG                HSL_RW

#define DEVICE_ID                             "mask_did"
#define MASK_CTL_DEVICE_ID_BOFFSET            8
#define MASK_CTL_DEVICE_ID_BLEN               8
#define MASK_CTL_DEVICE_ID_FLAG               HSL_RO

#define REV_ID                                "mask_rid"
#define MASK_CTL_REV_ID_BOFFSET               0
#define MASK_CTL_REV_ID_BLEN                  8
#define MASK_CTL_REV_ID_FLAG                  HSL_RO

#if defined(CONFIG_ONU_1_1_RF400)
/* Port Status Register */
#define PORT_STATUS
#define PORT_STATUS_OFFSET        0x007c
#define PORT_STATUS_E_LENGTH      4
#define PORT_STATUS_E_OFFSET      0x0004
#define PORT_STATUS_NR_E          7

#define FLOW_LINK_EN
#define PORT_STATUS_FLOW_LINK_EN_BOFFSET        12
#define PORT_STATUS_FLOW_LINK_EN_BLEN           1
#define PORT_STATUS_FLOW_LINK_EN_FLAG           HSL_RW

#define AUTO_RX_FLOW
#define PORT_STATUS_AUTO_RX_FLOW_BOFFSET        11
#define PORT_STATUS_AUTO_RX_FLOW_BLEN           1
#define PORT_STATUS_AUTO_RX_FLOW_FLAG           HSL_RO

#define AUTO_TX_FLOW
#define PORT_STATUS_AUTO_TX_FLOW_BOFFSET        10
#define PORT_STATUS_AUTO_TX_FLOW_BLEN           1
#define PORT_STATUS_AUTO_TX_FLOW_FLAG           HSL_RO

#define LINK_EN
#define PORT_STATUS_LINK_EN_BOFFSET             9
#define PORT_STATUS_LINK_EN_BLEN                1
#define PORT_STATUS_LINK_EN_FLAG                HSL_RW

#define LINK
#define PORT_STATUS_LINK_BOFFSET                8
#define PORT_STATUS_LINK_BLEN                   1
#define PORT_STATUS_LINK_FLAG                   HSL_RO

#define TX_HALF_FLOW_EN
#define PORT_STATUS_TX_HALF_FLOW_EN_BOFFSET     7
#define PORT_STATUS_TX_HALF_FLOW_EN_BLEN        1
#define PORT_STATUS_TX_HALF_FLOW_EN_FLAG        HSL_RW

#define DUPLEX_MODE
#define PORT_STATUS_DUPLEX_MODE_BOFFSET         6
#define PORT_STATUS_DUPLEX_MODE_BLEN            1
#define PORT_STATUS_DUPLEX_MODE_FLAG            HSL_RW

#define RX_FLOW_EN
#define PORT_STATUS_RX_FLOW_EN_BOFFSET          5
#define PORT_STATUS_RX_FLOW_EN_BLEN             1
#define PORT_STATUS_RX_FLOW_EN_FLAG             HSL_RW

#define TX_FLOW_EN
#define PORT_STATUS_TX_FLOW_EN_BOFFSET          4
#define PORT_STATUS_TX_FLOW_EN_BLEN             1
#define PORT_STATUS_TX_FLOW_EN_FLAG             HSL_RW

#define RXMAC_EN
#define PORT_STATUS_RXMAC_EN_BOFFSET            3
#define PORT_STATUS_RXMAC_EN_BLEN               1
#define PORT_STATUS_RXMAC_EN_FLAG               HSL_RW

#define TXMAC_EN
#define PORT_STATUS_TXMAC_EN_BOFFSET            2
#define PORT_STATUS_TXMAC_EN_BLEN               1
#define PORT_STATUS_TXMAC_EN_FLAG               HSL_RW

#define SPEED_MODE
#define PORT_STATUS_SPEED_MODE_BOFFSET          0
#define PORT_STATUS_SPEED_MODE_BLEN             2
#define PORT_STATUS_SPEED_MODE_FLAG             HSL_RW
#else
/* Port Status Register */
#define PORT_STATUS               "ptsts"
#define PORT_STATUS_ID            29
#define PORT_STATUS_OFFSET        0x0100
#define PORT_STATUS_E_LENGTH      4
#define PORT_STATUS_E_OFFSET      0x0100
#define PORT_STATUS_NR_E          7

#define LINK_EN                                 "ptsts_linken"
#define PORT_STATUS_LINK_EN_BOFFSET             9
#define PORT_STATUS_LINK_EN_BLEN                1
#define PORT_STATUS_LINK_EN_FLAG                HSL_RW

#define RXMAC_EN                                "ptsts_rxmacen"
#define PORT_STATUS_RXMAC_EN_BOFFSET            3
#define PORT_STATUS_RXMAC_EN_BLEN               1
#define PORT_STATUS_RXMAC_EN_FLAG               HSL_RW

#define TXMAC_EN                                "ptsts_txmacen"
#define PORT_STATUS_TXMAC_EN_BOFFSET            2
#define PORT_STATUS_TXMAC_EN_BLEN               1
#define PORT_STATUS_TXMAC_EN_FLAG               HSL_RW

#define SPEED_MODE                              "ptsts_speed"
#define PORT_STATUS_SPEED_MODE_BOFFSET          0
#define PORT_STATUS_SPEED_MODE_BLEN             2
#define PORT_STATUS_SPEED_MODE_FLAG             HSL_RW

#define TX_HALF_FLOW_EN
#define PORT_STATUS_TX_HALF_FLOW_EN_BOFFSET     7
#define PORT_STATUS_TX_HALF_FLOW_EN_BLEN        1
#define PORT_STATUS_TX_HALF_FLOW_EN_FLAG        HSL_RW

#define DUPLEX_MODE                             "ptsts_dupmod"
#define PORT_STATUS_DUPLEX_MODE_BOFFSET         6
#define PORT_STATUS_DUPLEX_MODE_BLEN            1
#define PORT_STATUS_DUPLEX_MODE_FLAG            HSL_RW
#endif

/* Operational Mode Register 0 */
#define OPEMODE0                  "opemode0"
#define OPEMODE0_ID               0
#define OPEMODE0_OFFSET           0x0004
#define OPEMODE0_E_LENGTH         4
#define OPEMODE0_E_OFFSET         0
#define OPEMODE0_NR_E             1

#if defined(CONFIG_ONU_1_1_RF400)
#define MAC0_PHY_GMII_EN                          "postrip_mac0_phy_gmii"
#define OPEMODE0_MAC0_PHY_GMII_EN_BOFFSET          14
#define OPEMODE0_MAC0_PHY_GMII_EN_BLEN             1
#define OPEMODE0_MAC0_PHY_GMII_EN_FLAG             HSL_RW

#define MAC0_PHY_GMII_TXCLK_SEL                          "postrip_mac0_phy_gmii_txclk_sel"
#define OPEMODE0_MAC0_PHY_GMII_TXCLK_SEL_BOFFSET          13
#define OPEMODE0_MAC0_PHY_GMII_TXCLK_SEL_BLEN             1
#define OPEMODE0_MAC0_PHY_GMII_TXCLK_SEL_FLAG             HSL_RW

#define MAC0_PHY_GMII_RXCLK_SEL                          "postrip_mac0_phy_gmii_rxclk_sel"
#define OPEMODE0_MAC0_PHY_GMII_RXCLK_SEL_BOFFSET          12
#define OPEMODE0_MAC0_PHY_GMII_RXCLK_SEL_BLEN             1
#define OPEMODE0_MAC0_PHY_GMII_RXCLK_SEL_FLAG             HSL_RW
#else
#define MAC0_PHY_GMII_EN                          "postrip_mac0_phy_gmii"
#define OPEMODE0_MAC0_PHY_GMII_EN_BOFFSET          10
#define OPEMODE0_MAC0_PHY_GMII_EN_BLEN             1
#define OPEMODE0_MAC0_PHY_GMII_EN_FLAG             HSL_RW

#define MAC0_PHY_GMII_TXCLK_SEL                          "postrip_mac0_phy_gmii_txclk_sel"
#define OPEMODE0_MAC0_PHY_GMII_TXCLK_SEL_BOFFSET          9
#define OPEMODE0_MAC0_PHY_GMII_TXCLK_SEL_BLEN             1
#define OPEMODE0_MAC0_PHY_GMII_TXCLK_SEL_FLAG             HSL_RW

#define MAC0_PHY_GMII_RXCLK_SEL                          "postrip_mac0_phy_gmii_rxclk_sel"
#define OPEMODE0_MAC0_PHY_GMII_RXCLK_SEL_BOFFSET          8
#define OPEMODE0_MAC0_PHY_GMII_RXCLK_SEL_BLEN             1
#define OPEMODE0_MAC0_PHY_GMII_RXCLK_SEL_FLAG             HSL_RW
#endif

#define OPEMODE3                  "opemode3"
#define OPEMODE3_ID               0
#define OPEMODE3_OFFSET           0x000C
#define OPEMODE3_E_LENGTH         4
#define OPEMODE3_E_OFFSET         0
#define OPEMODE3_NR_E             1

#define MAC6_PHY_GMII_EN                          "postrip_mac6_phy_gmii_en"
#define OPEMODE3_MAC6_PHY_GMII_EN_BOFFSET          14
#define OPEMODE3_MAC6_PHY_GMII_EN_BLEN             1
#define OPEMODE3_MAC6_PHY_GMII_EN_FLAG             HSL_RW

#define MAC6_PHY_GMII_TXCLK_SEL                          "postrip_mac6_PHY_gmii_txclk_sel"
#define OPEMODE3_MAC6_PHY_GMII_TXCLK_SEL_BOFFSET          13
#define OPEMODE3_MAC6_PHY_GMII_TXCLK_SEL_BLEN             1
#define OPEMODE3_MAC6_PHY_GMII_TXCLK_SEL_FLAG             HSL_RW

#define MAC6_PHY_GMII_RXCLK_SEL                          "postrip_mac6_PHY_gmii_rxclk_sel"
#define OPEMODE3_MAC6_PHY_GMII_RXCLK_SEL_BOFFSET          12
#define OPEMODE3_MAC6_PHY_GMII_RXCLK_SEL_BLEN             1
#define OPEMODE3_MAC6_PHY_GMII_RXCLK_SEL_FLAG             HSL_RW

#define MAC6_MAC_GMII_EN                          "postrip_mac6_mac_gmii_en"
#define OPEMODE3_MAC6_MAC_GMII_EN_BOFFSET          6
#define OPEMODE3_MAC6_MAC_GMII_EN_BLEN             1
#define OPEMODE3_MAC6_MAC_GMII_EN_FLAG             HSL_RW

#define MAC6_MAC_GMII_TXCLK_SEL                          "postrip_mac6_mac_gmii_txclk_sel"
#define OPEMODE3_MAC6_MAC_GMII_TXCLK_SEL_BOFFSET          5
#define OPEMODE3_MAC6_MAC_GMII_TXCLK_SEL_BLEN             1
#define OPEMODE3_MAC6_MAC_GMII_TXCLK_SEL_FLAG             HSL_RW

#define MAC6_MAC_GMII_RXCLK_SEL                          "postrip_mac6_mac_gmii_rxclk_sel"
#define OPEMODE3_MAC6_MAC_GMII_RXCLK_SEL_BOFFSET          4
#define OPEMODE3_MAC6_MAC_GMII_RXCLK_SEL_BLEN             1
#define OPEMODE3_MAC6_MAC_GMII_RXCLK_SEL_FLAG             HSL_RW


#define POSTRIP                  "postrip"
#define POSTRIP_ID               0
#define POSTRIP_OFFSET           0x0008
#define POSTRIP_E_LENGTH         4
#define POSTRIP_E_OFFSET         0
#define POSTRIP_NR_E             1

#define MAC0_MAC_MODE                          "postrip_mac0_mac"
#define POSTRIP_MAC0_MAC_MODE_BOFFSET          4
#define POSTRIP_MAC0_MAC_MODE_BLEN             1
#define POSTRIP_MAC0_MAC_MODE_FLAG             HSL_RW

#define MAC0_GMII_EN                           "postrip_mac0_gmii"
#define POSTRIP_MAC0_GMII_EN_BOFFSET           0
#define POSTRIP_MAC0_GMII_EN_BLEN              1
#define POSTRIP_MAC0_GMII_EN_FLAG              HSL_RW

#define MAC5_PHY_MODE                          "postrip_mac5_phy"
#define POSTRIP_MAC5_PHY_MODE_BOFFSET          15
#define POSTRIP_MAC5_PHY_MODE_BLEN             1
#define POSTRIP_MAC5_PHY_MODE_FLAG             HSL_RW

#define MAC5_MAC_MODE                          "postrip_mac5_mac"
#define POSTRIP_MAC5_MAC_MODE_BOFFSET          14
#define POSTRIP_MAC5_MAC_MODE_BLEN             1
#define POSTRIP_MAC5_MAC_MODE_FLAG             HSL_RW

#define MAC0_RGMII_EN                          "postrip_mac0_rgmii"
#define POSTRIP_MAC0_RGMII_EN_BOFFSET          1
#define POSTRIP_MAC0_RGMII_EN_BLEN             1
#define POSTRIP_MAC0_RGMII_EN_FLAG             HSL_RW

#define PHY4_RGMII_EN                          "postrip_phy4_rgmii"
#define POSTRIP_PHY4_RGMII_EN_BOFFSET          3
#define POSTRIP_PHY4_RGMII_EN_BLEN             1
#define POSTRIP_PHY4_RGMII_EN_FLAG             HSL_RW

#define PHY4_GMII_EN                           "postrip_phy4_gmii"
#define POSTRIP_PHY4_GMII_EN_BOFFSET           2
#define POSTRIP_PHY4_GMII_EN_BLEN              1
#define POSTRIP_PHY4_GMII_EN_FLAG              HSL_RW

/* BIST control Register */
#define BIST_CTRL               "bctrl"
#define BIST_CTRL_ID            24
#define BIST_CTRL_OFFSET        0x00a0
#define BIST_CTRL_E_LENGTH      4
#define BIST_CTRL_E_OFFSET      0
#define BIST_CTRL_NR_E          1

#define BIST_BUSY                        "bctrl_bb"
#define BIST_CTRL_BIST_BUSY_BOFFSET      31
#define BIST_CTRL_BIST_BUSY_BLEN         1
#define BIST_CTRL_BIST_BUSY_FLAG         HSL_RW

#define PTN_EN2                          "bctrl_pe2"
#define BIST_CTRL_PTN_EN2_BOFFSET        22
#define BIST_CTRL_PTN_EN2_BLEN           1
#define BIST_CTRL_PTN_EN2_FLAG           HSL_RW


#define PTN_EN1                          "bctrl_pe1"
#define BIST_CTRL_PTN_EN1_BOFFSET        21
#define BIST_CTRL_PTN_EN1_BLEN           1
#define BIST_CTRL_PTN_EN1_FLAG           HSL_RW

#define PTN_EN0                          "bctrl_pe0"
#define BIST_CTRL_PTN_EN0_BOFFSET        20
#define BIST_CTRL_PTN_EN0_BLEN           1
#define BIST_CTRL_PTN_EN0_FLAG           HSL_RW

#define ERR_CNT                          "bctrl_ec"
#define BIST_CTRL_ERR_CNT_BOFFSET        13
#define BIST_CTRL_ERR_CNT_BLEN           2
#define BIST_CTRL_ERR_CNT_FLAG           HSL_RO

#define ONE_ERR                          "bctrl_oe"
#define BIST_CTRL_ONE_ERR_BOFFSET        30
#define BIST_CTRL_ONE_ERR_BLEN           1
#define BIST_CTRL_ONE_ERR_FLAG           HSL_RO

#define ERR_ADDR                         "bctrl_ea"
#define BIST_CTRL_ERR_ADDR_BOFFSET       0
#define BIST_CTRL_ERR_ADDR_BLEN          12
#define BIST_CTRL_ERR_ADDR_FLAG          HSL_RO

/* BIST recover Register */
#define BIST_RCV               "brcv"
#define BIST_RCV_ID            24
#define BIST_RCV_OFFSET        0x00a4
#define BIST_RCV_E_LENGTH      4
#define BIST_RCV_E_OFFSET      0
#define BIST_RCV_NR_E          1

#define RCV_EN                           "brcv_en"
#define BIST_RCV_RCV_EN_BOFFSET          31
#define BIST_RCV_RCV_EN_BLEN             1
#define BIST_RCV_RCV_EN_FLAG             HSL_RW

#define RCV_ADDR                         "brcv_addr"
#define BIST_RCV_RCV_ADDR_BOFFSET        0
#define BIST_RCV_RCV_ADDR_BLEN           12
#define BIST_RCV_RCV_ADDR_FLAG           HSL_RW

#define HSL_DEV_ID_CHECK(dev_id) \
do { \
    if (dev_id >= SW_MAX_NR_DEV) \
        return SW_OUT_OF_RANGE; \
} while (0)


#define HSL_REG_ENTRY_GET(rv, dev, reg, index, value, val_len) \
    do { \
        hsl_reg_func_t *p_reg; \
        if (index > reg##_NR_E) \
            return SW_OUT_OF_RANGE; \
        SW_RTN_ON_NULL(p_reg = hsl_reg_ptr_get(dev)); \
        rv = p_reg->reg_entry_get (dev, reg##_OFFSET, \
                                   ((a_uint32_t)index) * reg##_E_OFFSET,\
                                   reg##_E_LENGTH, (a_uint8_t*)value, \
                                   (a_uint8_t)val_len); \
    } while (0)

#define SW_RTN_ON_ERROR(rtn) \
			do { if (rtn != SW_OK) return(rtn); } while(0);

#define SW_RTN_ON_NULL(op)  \
    do { \
        if ((op) == NULL) \
            return SW_NOT_INITIALIZED;\
    }while(0);


/* register functions */
#define SW_BIT_MASK_U32(nr) (~(0xFFFFFFFF << (nr)))

#define SW_FIELD_MASK_U32(offset, len) \
    ((SW_BIT_MASK_U32(len) << (offset)))

#define SW_FIELD_MASK_NOT_U32(offset,len) \
    (~(SW_BIT_MASK_U32(len) << (offset)))

#define SW_FIELD_2_REG(field_val, bit_offset) \
    (field_val << (bit_offset) )

#define SW_REG_2_FIELD(reg_val, bit_offset, field_len) \
    (((reg_val) >> (bit_offset)) & ((1 << (field_len)) - 1))

#define SW_FIELD_MASK_U32(offset, len) \
    ((SW_BIT_MASK_U32(len) << (offset)))

#define SW_FIELD_MASK_NOT_U32(offset,len) \
    (~(SW_BIT_MASK_U32(len) << (offset)))

#define SW_REG_SET_BY_FIELD_U32(reg_value, field_value, bit_offset, field_len)\
	do { \
        (reg_value) = \
            (((reg_value) & SW_FIELD_MASK_NOT_U32((bit_offset),(field_len))) \
              | (((field_value) & SW_BIT_MASK_U32(field_len)) << (bit_offset)));\
    } while (0)

#define SW_FIELD_GET_BY_REG_U32(reg_value, field_value, bit_offset, field_len)\
	do { \
        (field_value) = \
            (((reg_value) >> (bit_offset)) & SW_BIT_MASK_U32(field_len)); \
    } while (0)

#define SW_SET_REG_BY_FIELD(reg, field, field_value, reg_value) \
    SW_REG_SET_BY_FIELD_U32(reg_value, field_value, reg##_##field##_BOFFSET, \
		                    reg##_##field##_BLEN)

#define SW_GET_FIELD_BY_REG(reg, field, field_value, reg_value) \
    SW_FIELD_GET_BY_REG_U32(reg_value, field_value, reg##_##field##_BOFFSET, \
		                    reg##_##field##_BLEN)

#define HSL_REG_ENTRY_SET(rv, dev, reg, index, value, val_len) \
    do { \
        hsl_reg_func_t *p_reg; \
        if (index > reg##_NR_E) \
            return SW_OUT_OF_RANGE; \
        SW_RTN_ON_NULL(p_reg = hsl_reg_ptr_get(dev)); \
        rv = p_reg->reg_entry_set (dev, reg##_OFFSET, \
                                   ((a_uint32_t)index) * reg##_E_OFFSET,\
                                   reg##_E_LENGTH, (a_uint8_t*)value, \
                                   (a_uint8_t)val_len); \
    } while (0)

#define HSL_REG_FIELD_SET(rv, dev, reg, index, field, value, val_len) \
			do { \
				hsl_reg_func_t *p_reg; \
				if (index > reg##_NR_E) \
					return SW_OUT_OF_RANGE; \
				SW_RTN_ON_NULL (p_reg = hsl_reg_ptr_get(dev)); \
				rv = p_reg->reg_field_set(dev, reg##_OFFSET, \
										  ((a_uint32_t)index) * reg##_E_OFFSET,\
										  reg##_##field##_BOFFSET, \
										  reg##_##field##_BLEN, (a_uint8_t*)value, \
										  (a_uint8_t)val_len);\
			} while (0)
		

#define HSL_REG_FIELD_GET(rv, dev, reg, index, field, value, val_len) \
					do { \
						hsl_reg_func_t *p_reg; \
						if (index > reg##_NR_E) \
							return SW_OUT_OF_RANGE; \
						SW_RTN_ON_NULL(p_reg = hsl_reg_ptr_get(dev)); \
						rv = p_reg->reg_field_get (dev, reg##_OFFSET, \
												   ((a_uint32_t)index) * reg##_E_OFFSET,\
												   reg##_##field##_BOFFSET, \
												   reg##_##field##_BLEN, (a_uint8_t*)value, \
												   (a_uint8_t)val_len); \
					} while (0)


/* port bitmap functions */
#define SW_IS_PBMP_MEMBER(pbm, port)  ((pbm & (1 << port)) ? A_TRUE: A_FALSE)
#define SW_IS_PBMP_EQ(pbm0, pbm1)  ((pbm0 == pbm1) ? A_TRUE: A_FALSE)


/* Flood Mask Register */
#define FLOOD_MASK                "fmask"
#define FLOOD_MASK_ID             5
#define FLOOD_MASK_OFFSET         0x002c
#define FLOOD_MASK_E_LENGTH       4
#define FLOOD_MASK_E_OFFSET       0
#define FLOOD_MASK_NR_E           1

#define BC_FLOOD_DP                         "fmask_bfdp"
#define FLOOD_MASK_BC_FLOOD_DP_BOFFSET      25
#define FLOOD_MASK_BC_FLOOD_DP_BLEN         7
#define FLOOD_MASK_BC_FLOOD_DP_FLAG         HSL_RW

#define UNI_FLOOD_DP                        "fmask_ufdp"
#define FLOOD_MASK_UNI_FLOOD_DP_BOFFSET     0
#define FLOOD_MASK_UNI_FLOOD_DP_BLEN        7
#define FLOOD_MASK_UNI_FLOOD_DP_FLAG        HSL_RW

#define MUL_FLOOD_DP                        "fmask_mfdp"
#define FLOOD_MASK_MUL_FLOOD_DP_BOFFSET     16
#define FLOOD_MASK_MUL_FLOOD_DP_BLEN        7
#define FLOOD_MASK_MUL_FLOOD_DP_FLAG        HSL_RW

#define ARP_LEAKY_EN                            "pbvlan_alen"
#define PORT_CTL_ARP_LEAKY_EN_BOFFSET           22
#define PORT_CTL_ARP_LEAKY_EN_BLEN              1
#define PORT_CTL_ARP_LEAKY_EN_FLAG              HSL_RW

#define QM_CTL                    "qmct"
#define QM_CTL_ID                 7
#define QM_CTL_OFFSET             0x003c
#define QM_CTL_E_LENGTH           4
#define QM_CTL_E_OFFSET           0
#define QM_CTL_NR_E               1

#define ARP_EN                                "qmct_arpe"
#define QM_CTL_ARP_EN_BOFFSET                 15
#define QM_CTL_ARP_EN_BLEN                    1
#define QM_CTL_ARP_EN_FLAG                    HSL_RW

#define ARP_CMD                               "qmct_arpc" 
#define QM_CTL_ARP_CMD_BOFFSET                14
#define QM_CTL_ARP_CMD_BLEN                   1
#define QM_CTL_ARP_CMD_FLAG                   HSL_RW

#endif

