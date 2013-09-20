/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2011 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * This is an interface module for handling NAND in yaffs2 mode.
 */

/* This code calls a driver that accesses "generic" NAND. In the simulator
 * this is direceted at a file-backed NAND simulator. The NAND access functions
 * should also work with real NAND.
 *
 * This driver is designed for use in yaffs2 mode with a 2k page size and
 * 64 bytes of spare.
 *
 * The spare ares is used as follows:
 * offset 0: 2 bytes bad block marker.
 * offset 2: 8x3 bytes of ECC over the data.
 * offset 26: rest available to store Yaffs tags etc.
 */

#include "yaffs_nand_drv.h"
#include "yportenv.h"
#include "yaffs_trace.h"

#include "yaffs_flashif.h"
#include "yaffs_guts.h"
#include "yaffs_ecc.h"


static inline struct nand_chip *dev_to_chip(struct yaffs_dev *dev)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->driver_context);
    
	return (struct nand_chip *)mtd->priv;
}

static inline struct mtd_info *dev_to_mtd(struct yaffs_dev *dev)
{
	return (struct mtd_info *)(dev->driver_context);
}

static inline loff_t dev_block_offset(struct yaffs_dev *dev, int block_no)
{
    struct nand_chip *chip = dev_to_chip(dev);

    return block_no << chip->phys_erase_shift;
}

static inline uint64_t dev_block_size(struct yaffs_dev *dev)
{
    struct nand_chip *chip = dev_to_chip(dev);

    return 1 << chip->phys_erase_shift;
}


static int yaffs_nand_drv_WriteChunk(struct yaffs_dev *dev, int nand_chunk,
				   const u8 *data, int data_len,
				   const u8 *oob, int oob_len)
{
    struct mtd_info *mtd = dev_to_mtd(dev);
    struct mtd_oob_ops ops;
    loff_t addr = ((loff_t) nand_chunk) * dev->param.total_bytes_per_chunk;
    int ret;

    if(!data || !oob)
        return YAFFS_FAIL;


    ops.mode = MTD_OPS_AUTO_OOB;
    ops.ooblen = oob_len;
    ops.len = data_len;
    ops.ooboffs = 0;
    ops.datbuf = data;
    ops.oobbuf = oob;
    ret = mtd->_write_oob(mtd, addr, &ops);
    
    if (0 == ret) {
        return YAFFS_OK;
    } else  {
        return YAFFS_FAIL;
    }
}


static int yaffs_nand_drv_ReadChunk(struct yaffs_dev *dev, int nand_chunk,
				   u8 *data, int data_len,
				   u8 *oob, int oob_len,
				   enum yaffs_ecc_result *ecc_result_out)
{
	struct mtd_info *mtd = dev_to_mtd(dev);
	struct mtd_oob_ops ops;
	loff_t addr = ((loff_t) nand_chunk) * dev->param.total_bytes_per_chunk;
	int ret;
	enum yaffs_ecc_result ecc_result;

    ops.mode = MTD_OPS_AUTO_OOB;
    ops.ooblen = oob_len;
    ops.len = data_len;
    ops.ooboffs = 0;
    ops.datbuf = data;
    ops.oobbuf = oob;
    ret = mtd->_read_oob(mtd, addr, &ops);

    if (mtd_is_eccerr(ret)) {
       ecc_result = YAFFS_ECC_RESULT_UNFIXED;
    } else if (mtd_is_bitflip(ret)) {
       ecc_result = YAFFS_ECC_RESULT_FIXED;
    } else if (ret) {
        ecc_result = YAFFS_ECC_RESULT_UNKNOWN;
    } else {// 0
        ecc_result = YAFFS_ECC_RESULT_NO_ERROR;
    }

    if (ecc_result_out) {
        *ecc_result_out = ecc_result;
    }
    
	if ((ecc_result == YAFFS_ECC_RESULT_NO_ERROR) || \
        (ecc_result == YAFFS_ECC_RESULT_FIXED)) {
        return YAFFS_OK;
    } else {
        return YAFFS_FAIL;
    }
}

static int yaffs_nand_drv_EraseBlock(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = dev_to_mtd(dev);
	struct erase_info ei;
	int retval = 0;

	ei.mtd = mtd;
	ei.addr = dev_block_offset(dev, block_no);
	ei.len = dev_block_size(dev);
	ei.time = 1000;
	ei.retries = 2;
	ei.callback = NULL;
	ei.priv = (u_long) dev;

	/* Todo finish off the ei if required */


	retval = mtd->_erase(mtd, &ei);

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

static int yaffs_nand_drv_MarkBad(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = dev_to_mtd(dev);
	int retval = 0;

    retval = mtd->_block_markbad(mtd, dev_block_offset(dev, block_no));
	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}


static int yaffs_nand_drv_CheckBad(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = dev_to_mtd(dev);
    
    if (mtd->_block_isbad(mtd, dev_block_offset(dev, block_no))) {
        return YAFFS_FAIL;
    } else {
        return YAFFS_OK;
    }
}

static int yaffs_nand_drv_Initialise(struct yaffs_dev *dev)
{

	return YAFFS_OK;
}

static int yaffs_nand_drv_Deinitialise(struct yaffs_dev *dev)
{

	return YAFFS_OK;
}


int yaffs_nand_install_drv(struct yaffs_dev *dev, struct nand_chip *chip)
{
	struct yaffs_driver *drv = &dev->drv;


	drv->drv_write_chunk_fn = yaffs_nand_drv_WriteChunk;
	drv->drv_read_chunk_fn = yaffs_nand_drv_ReadChunk;
	drv->drv_erase_fn = yaffs_nand_drv_EraseBlock;
	drv->drv_mark_bad_fn = yaffs_nand_drv_MarkBad;
	drv->drv_check_bad_fn = yaffs_nand_drv_CheckBad;
	drv->drv_initialise_fn = yaffs_nand_drv_Initialise;
	drv->drv_deinitialise_fn = yaffs_nand_drv_Deinitialise;

	return YAFFS_OK;
}
