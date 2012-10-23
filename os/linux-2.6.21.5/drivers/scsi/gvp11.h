#ifndef GVP11_H

/* $Id: //depot/sw/abu/opconn/release/v3.2.1/os/linux-2.6.21.5/drivers/scsi/gvp11.h#1 $
 *
 * Header file for the GVP Series II SCSI controller for Linux
 *
 * Written and (C) 1993, Ralf Baechle, see gvp11.c for more info
 * based on a2091.h (C) 1993 by Hamish Macdonald
 *
 */

#include <linux/types.h>

int gvp11_detect(struct scsi_host_template *);
int gvp11_release(struct Scsi_Host *);

#ifndef CMD_PER_LUN
#define CMD_PER_LUN 2
#endif

#ifndef CAN_QUEUE
#define CAN_QUEUE 16
#endif

#ifndef HOSTS_C

/*
 * if the transfer address ANDed with this results in a non-zero
 * result, then we can't use DMA.
 */
#define GVP11_XFER_MASK  (0xff000001)

typedef struct {
             unsigned char      pad1[64];
    volatile unsigned short     CNTR;
             unsigned char      pad2[31];
    volatile unsigned char      SASR;
             unsigned char      pad3;
    volatile unsigned char      SCMD;
             unsigned char      pad4[4];
    volatile unsigned short     BANK;
             unsigned char      pad5[6];
    volatile unsigned long      ACR;
    volatile unsigned short     secret1; /* store 0 here */
    volatile unsigned short     ST_DMA;
    volatile unsigned short     SP_DMA;
    volatile unsigned short     secret2; /* store 1 here */
    volatile unsigned short     secret3; /* store 15 here */
} gvp11_scsiregs;

/* bits in CNTR */
#define GVP11_DMAC_BUSY		(1<<0)
#define GVP11_DMAC_INT_PENDING	(1<<1)
#define GVP11_DMAC_INT_ENABLE	(1<<3)
#define GVP11_DMAC_DIR_WRITE	(1<<4)

#endif /* else def HOSTS_C */

#endif /* GVP11_H */
