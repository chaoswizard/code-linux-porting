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

static inline struct mtd_info *dev_to_mtd(struct yaffs_dev *dev)
{
	return (struct mtd_info *)(dev->driver_context);
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
    
    memset(&ops, 0, sizeof(ops));

    ops.mode = MTD_OPS_AUTO_OOB;
    ops.ooblen = oob_len;
    ops.len = (data) ? data_len : 0;
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
				   enum yaffs_ecc_result *ecc_result)
{
	struct mtd_info *mtd = dev_to_mtd(dev);
	struct mtd_oob_ops ops;
	loff_t addr = ((loff_t) nand_chunk) * dev->param.total_bytes_per_chunk;
	int ret;

    memset(&ops, 0, sizeof(ops));
    
    ops.mode = MTD_OPS_AUTO_OOB;
    ops.ooblen = oob_len;
    ops.len = (data) ? data_len : 0;
    ops.ooboffs = 0;
    ops.datbuf = data;
    ops.oobbuf = oob;
    ret = mtd->_read_oob(mtd, addr, &ops);

    
    if (ret) {
        yaffs_trace(YAFFS_TRACE_MTD,
                "read_oob failed, chunk %d, mtd error %d",
                nand_chunk, ret);
    }

    switch (ret) {
    case 0:
            /* no error */
            if(ecc_result)
                    *ecc_result = YAFFS_ECC_RESULT_NO_ERROR;
            break;

    case -EUCLEAN:// mtd_is_bitflip
            /* MTD's ECC fixed the data */
            if(ecc_result)
                    *ecc_result = YAFFS_ECC_RESULT_FIXED;
            break;

    case -EBADMSG:// mtd_is_eccerr
    default:
            /* MTD's ECC could not fix the data */
            if(ecc_result)
                    *ecc_result = YAFFS_ECC_RESULT_UNFIXED;
            return YAFFS_FAIL;
    }   
    
    return YAFFS_OK;
}

static int yaffs_nand_drv_EraseBlock(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = dev_to_mtd(dev);

    loff_t addr;
    u32 block_size;
	struct erase_info ei;
	int retval = 0;

    block_size = dev->param.total_bytes_per_chunk * dev->param.chunks_per_block;
    addr = ((loff_t ) block_no) * block_size;

	ei.mtd = mtd;
	ei.addr = addr;
	ei.len = block_size;
	ei.time = 1000;
	ei.retries = 2;
	ei.callback = NULL;
	ei.priv = (u_long) dev;


	retval = mtd->_erase(mtd, &ei);

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

static int yaffs_nand_drv_MarkBad(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = dev_to_mtd(dev);

    u32 block_size = dev->param.total_bytes_per_chunk * dev->param.chunks_per_block;
	int retval = 0;

    retval = mtd->_block_markbad(mtd, ((loff_t)block_no) * block_size);
    if (retval) 
        return YAFFS_FAIL ;
    else 
        return YAFFS_OK;
}


static int yaffs_nand_drv_CheckBad(struct yaffs_dev *dev, int block_no)
{
	struct mtd_info *mtd = dev_to_mtd(dev);
    u32 block_size = dev->param.total_bytes_per_chunk * dev->param.chunks_per_block;
	int retval = 0;


    retval= mtd->_block_isbad(mtd, ((loff_t)block_no) * block_size);
    return (retval) ? YAFFS_FAIL : YAFFS_OK;
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
