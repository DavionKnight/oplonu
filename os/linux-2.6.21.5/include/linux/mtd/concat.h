/*
 * MTD device concatenation layer definitions
 *
 * (C) 2002 Robert Kaiser <rkaiser@sysgo.de>
 *
 * This code is GPL
 *
 * $Id: //depot/sw/abu/opconn/release/v3.2.1/os/linux-2.6.21.5/include/linux/mtd/concat.h#1 $
 */

#ifndef MTD_CONCAT_H
#define MTD_CONCAT_H


struct mtd_info *mtd_concat_create(
    struct mtd_info *subdev[],  /* subdevices to concatenate */
    int num_devs,               /* number of subdevices      */
    char *name);                /* name for the new device   */

void mtd_concat_destroy(struct mtd_info *mtd);

#endif

