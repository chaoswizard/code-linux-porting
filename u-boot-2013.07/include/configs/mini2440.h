/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <gj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * (C) Copyright 2009-2010
 * Michel Pollet <buserror@gmail.com>
 *
 * (C) Copyright 2012
 * Gabriel Huau <contact@huau-gabriel.fr>
 *
 * Configuation settings for the MINI2440 board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#if 1
#define CONFIG_USE_NAND
#define CONFIG_SYS_NO_FLASH
#define CONFIG_ENV_IS_IN_NAND
#else
#define CONFIG_USE_NOR
#define CONFIG_ENV_IS_IN_FLASH
#endif

#define CONFIG_DEBUG_LL
//#define CONFIG_MINI2440_LED
//#define CONFIG_MINI2440_SPEAKER
//#define CONFIG_MTD_DEBUG
//#define CONFIG_MTD_DEBUG_VERBOSE 3

#ifdef CONFIG_DEBUG_LL
#define DEBUG_LL(x, y)       debug_ll((unsigned int)(x), (unsigned int)(y))
#define DEBUG_NUM_LL(x, y)   debug_ll_num((x), (unsigned int)(y))
#else
#define DEBUG_LL(x, y)
#define DEBUG_NUM_LL(x, y) 
#endif

/*
 * High Level Configuration Options
 */
#define CONFIG_ARM920T			/* This is an ARM920T Core	*/
#define CONFIG_S3C24X0			/* in a SAMSUNG S3C24X0 SoC */
#define CONFIG_S3C2440			/* in a SAMSUNG S3C2440 SoC */
#define CONFIG_MINI2440			/* on a MIN2440 Board       */

#define MACH_TYPE_MINI2440	1999
#define CONFIG_MACH_TYPE	MACH_TYPE_MINI2440

#define CONFIG_S3C2440_GPIO
#define CONFIG_SYS_ARM_CACHE_WRITETHROUGH
#define CONFIG_BOARD_EARLY_INIT_F
#undef CONFIG_USE_IRQ		/* we don't need IRQ/FIQ stuff */


#ifdef CONFIG_NAND_SPL     

#else
#define CONFIG_INTERRUPTS  
/*
 * We don't use lowlevel_init
 */
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif


/*
 * input clock of PLL
 */
/* MINI2440 has 12.0000MHz input clock */
#define CONFIG_SYS_CLK_FREQ	12000000
#define CONFIG_RTC_S3C24X0
/* MDIV | PSDIV, 405M*/
/*  
 * set mpll to 405Mhz, see C7P21  
 */  
#define CONFIG_DEFAULT_MPLLCON  ((0x7f << 12) | 0x21)
#define CONFIG_DEFAULT_CLKDIV   (5)




/*
 * the PWM TImer 4 uses a counter of 15625 for 10 ms, so we need
 * it to wrap 100 times (total 1562500) to get 1 sec.
 */
#define CONFIG_SYS_HZ			1562500


/*
 * Hardware drivers
 */
#define CONFIG_DRIVER_DM9000
#define CONFIG_DRIVER_DM9000_NO_EEPROM
#define CONFIG_DM9000_BASE				0x20000300
#define DM9000_IO		CONFIG_DM9000_BASE
#define DM9000_DATA		(CONFIG_DM9000_BASE+4)

/*
 * select serial console configuration
 */
#define CONFIG_S3C24X0_SERIAL
#define CONFIG_SERIAL1		1	/* we use SERIAL 1 on mini2440 */



/*
 * valid baudrates
 */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }
#define CONFIG_BAUDRATE		115200


/*
 * Command definition
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_DHCP
#define CONFIG_CMD_PORTIO
#define CONFIG_CMD_REGINFO
#define CONFIG_CMD_SAVES
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_NAND
/* Dynamic MTD partition support */
#define CONFIG_CMD_MTDPARTS
/* yaffs cmd support*/
#define CONFIG_CMD_NAND_YAFFS
/* use yaffs2*/
#define CONFIG_YAFFS2




/*
 * Miscellaneous configurable options
 */
#define CONFIG_LONGHELP
#define CONFIG_SYS_PROMPT	"ELVON_NLDR#"
#define CONFIG_SYS_CBSIZE	256
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	32
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

#define CONFIG_SYS_MEMTEST_START	   0x30000000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x34000000	/* 64 MB in DRAM */


/*
 * Stack sizes
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(8*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif


/*
 * Size of malloc() pool
 * BZIP2 / LZO / LZMA need a lot of RAM
 */
#define CONFIG_SYS_MALLOC_LEN	(CONFIG_ENV_SIZE + (2<<20))

/*
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS        1          /* we have 1 bank of DRAM */
#define PHYS_SDRAM_SIZE             (64 << 20) /* 64MB of DRAM */
#define CONFIG_SYS_SDRAM_BASE       0x30000000 /*(nGCS6)=0x30000000 */

/*
 * the GENERATED_GBL_DATA_SIZA(global_data) is create by kbulid tools as in asm-offsets.c
 * --------------------------------------- high address
 * |stack |   
 * |------|------------------------------- <stack base addr>=INIT_SP_ADDR
 * |      | +sizeof(struct global_data)
 * |  4K  |------------------------------- <gd base addr> = r8
 * |      |  unused
 * --------------------------------------- low address 
 */
#define CONFIG_MAX_CODE_SIZE   (8<<20)
#define CONFIG_MAX_STACK_SIZE  (64<<10)

#if (CONFIG_SYS_TEXT_BASE <= CONFIG_SYS_SDRAM_BASE)
// load from ram for nandboot(0x0) or producer(0x30000000)
/*         0x30000000->0x34000000 (0M:64M)
 *----------------------------------------------------------------|
 * code +  tmp stack |pad1 |boot param |gd |stack   |load addr|   |
 *----------------------------------------------------------------|
 */
/* default load address = (0x34000000 - (40<<20))*/                      
#define CONFIG_SYS_LOAD_ADDR	0x31800000
#define CONFIG_SYS_TMP_SP_ADDR  (CONFIG_SYS_SDRAM_BASE + CONFIG_MAX_CODE_SIZE)

#elif (CONFIG_SYS_TEXT_BASE >= 0x33000000)
//u-boot code
/*         0x33000000->0x34000000 (48M:16M)
 *------------ 48M -------|-----------------   16M  -------------------------|
 *---------------------------------------------------------------------------|
 * unused |load addr|pad2 |code +  tmp stack |pad1 |boot param |gd |stack    |
 *----------------------------------------------------------------------------
 */
/* default load address = (CONFIG_SYS_TEXT_BASE - (40<<20))*/                      
#define CONFIG_SYS_LOAD_ADDR	0x30008000
#define CONFIG_SYS_TMP_SP_ADDR  (CONFIG_SYS_TEXT_BASE + CONFIG_MAX_CODE_SIZE)

#elif (CONFIG_SYS_TEXT_BASE == 0x31800000)
// test u-boot code, load by producer
#define CONFIG_SYS_LOAD_ADDR	0x30008000
#define CONFIG_SYS_TMP_SP_ADDR  (CONFIG_SYS_TEXT_BASE + CONFIG_MAX_CODE_SIZE)
#else
#error "unknown memory layout of CONFIG_SYS_TEXT_BASE"
#endif

                                        
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_TMP_SP_ADDR + CONFIG_MAX_STACK_SIZE -\
                                 GENERATED_GBL_DATA_SIZE)


/*-----------------------------------------------------------------------
 * Nand FLASH organization
 */
#if defined(CONFIG_USE_NAND) || defined(CONFIG_NAND_SPL) || defined(CONFIG_CMD_NAND)

#define CONFIG_NAND_S3C2440
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0x4E000000

#define CONFIG_SYS_NAND_PAGE_SIZE	(2 << 10)
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 << 10)
#define CONFIG_SYS_NAND_PAGE_COUNT	64
#define CONFIG_SYS_NAND_5_ADDR_CYCLE

#define CONFIG_S3C2440_NAND_HWECC
#define CONFIG_SYS_NAND_BAD_BLOCK_POS	0

/* 
 *|--------------------------------------------------------------|
 *|                   nand flash block layout                    |
 *|--------------------------------------------------------------|
 *| block 0   [page0 |oob0 ] [page1 |oob1 ] [page(n-1) |oob(n-1)]|
 *|--------------------------------------------------------------|
 *| block 1   [page0 |oob0 ] [page1 |oob1 ] [page(n-1) |oob(n-1)]|
 *|--------------------------------------------------------------|
 *| block 2   [page0 |oob0 ] [page1 |oob1 ] [page(n-1) |oob(n-1)]|
 *|--------------------------------------------------------------|
 *| block(n-1)[page0 |oob0 ] [page1 |oob1 ] [page(n-1) |oob(n-1)]|
 *|--------------------------------------------------------------|
 */
#define CONFIG_SYS_NAND_ECCSIZE		CONFIG_SYS_NAND_PAGE_SIZE
#define CONFIG_SYS_NAND_ECCBYTES	4
#define CONFIG_SYS_NAND_OOBSIZE		64
#define CONFIG_SYS_NAND_ECCPOS		{40, 41, 42, 43, 44, 45, 46, 47, \
					48, 49, 50, 51, 52, 53, 54, 55, \
					56, 57, 58, 59, 60, 61, 62, 63}
#endif

#if ! defined(CONFIG_SYS_NO_FLASH)
/*
 * NOR FLASH organization
 */
#define CONFIG_SYS_FLASH_BASE		0x0 /* Flash Bank #0 */
#define CONFIG_SYS_FLASH_SIZE	   (2 << 20)

#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
/* uses the standard CFI interface*/
#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT
#define CONFIG_SYS_MONITOR_BASE		0x0
/* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_BANKS	1
/* 512 * 4096 sectors, or 32 * 64k blocks */
#define CONFIG_SYS_MAX_FLASH_SECT	512
#define CONFIG_FLASH_SHOW_PROGRESS  1
#endif




/* ====================================================
 * wx:image ROM layout, same as kernel mtd config
 */
/*mtdblock0 [512K]*/
#define  IMG_UBOOT_OFFSET     0
#define  IMG_UBOOT_SIZE       0x00080000

/*mtdblock1 [128K]*/
#define  IMG_PARAM_OFFSET     0x00080000
#define  IMG_PARAM_SIZE       0x00020000

/*mtdblock2 [5M]/[4M-8M]*/
#define  IMG_KERNEL_OFFSET    0x000A0000
#define  IMG_KERNEL_SIZE      0x00500000

/*mtdblock3 [-$]*/
#define  IMG_ROOTFS_OFFSET    0x005A0000

/* ====================================================
 * wx:image RAM layout, same as kernel link config
 */
/* boot kernel address */
#define CONFIG_BOOT_KERNEL_ADDR  0x30008000
/* boot parameters address */
#define CONFIG_BOOT_PARAM_ADDR	 0x30000100
/* ====================================================*/



#if defined(CONFIG_NAND_SPL)
#define CONFIG_SYS_NAND_U_BOOT_DST	0x33000000		/* uboot.bin load addr */
#define CONFIG_SYS_NAND_U_BOOT_START CONFIG_SYS_NAND_U_BOOT_DST	/* uboot.bin run-addr */
#define CONFIG_SYS_NAND_U_BOOT_OFFS	 (4 << 10)      		/* uboot.bin Offset on nand */
#define CONFIG_SYS_NAND_U_BOOT_SIZE	 IMG_UBOOT_SIZE		/* uboot.bin size on nand */
/* Put environment copies after the end of U-Boot owned RAM */
//#define CONFIG_NAND_ENV_DST	            (CONFIG_SYS_TEXT_BASE + IMG_PARAM_OFFSET)
#endif


#if defined(CONFIG_ENV_IS_IN_FLASH)
/* save on nor flash*/
#define CONFIG_ENV_SIZE		0x4000
#define CONFIG_ENV_ADDR		(CONFIG_SYS_FLASH_BASE + CONFIG_SYS_FLASH_SIZE - \
									 CONFIG_ENV_SIZE)

#elif defined(CONFIG_ENV_IS_IN_NAND)
/* save on nand flash*/
/* 128K, wx: MUST mutiply with Block Size(128K), orelse env will write failed*/
#define CONFIG_ENV_SIZE	 IMG_PARAM_SIZE 
#define CONFIG_ENV_OFFSET   IMG_PARAM_OFFSET 
#else
#error environments configure error.
#endif


/*
 * allow to overwrite serial and ethaddr
 */
#define CONFIG_ENV_OVERWRITE



/* ATAG configuration */
#define CONFIG_INITRD_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_LONGHELP


/* autoboot */
#define CONFIG_BOOTDELAY	9
#define CONFIG_BOOT_RETRY_TIME	-1
#define CONFIG_RESET_TO_RETRY
#define CONFIG_ZERO_BOOTDELAY_CHECK

#ifdef CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE		/* needed for mtdparts commands */
/* configure for boot application */
//wx: enable the nand defalut partition manage 
#define MTDIDS_DEFAULT "nand0=wxmtd"
//wx: define the defalut nand partion configure,
/** wx: !NOTE: 
* MUST make sure the offset of root partition same as the target system(linux-kernel defined)
*/
#define MTDPARTS_DEFAULT "mtdparts=wxmtd:512k@0(uboot)," \
	"128k(param)," \
	"5m(kernel)," \
	"-(rootfs)"
#endif

/*----------mtd config must refer as image layout table---------------*/
#define CONFIG_BOOTARGS_YAFFS         	\
	"root=/dev/mtdblock3 rw "\
	"rootfstype=yaffs "\
	"noinitrd "\
	"console=ttySAC0,115200 "\
	"init=/linuxrc"
/*----------------------------------------------------------------------------*/
#define CONFIG_BOOTARGS_CRAMFS         	\
	"root=/dev/mtdblock3 rw "\
	"rootfstype=cramfs "\
	"console=ttySAC0,115200 "\
	"noinitrd "\
	"mem=64M "\
	"init=/linuxrc"
/*----------------------------------------------------------------------------*/
#define CONFIG_BOOTARGS_RAMDISK        	\
	"initrd=0x31000000,0x400000 "\
	"mem=64M "\
	"root=/dev/ram rw "\
	"console=ttySAC0,115200 "\
	"init=/linuxrc"
/*----------------------------------------------------------------------------*/
/* ramfs  use init instead of linuxrc */
#define CONFIG_BOOTARGS_RAMFS         	\
	"root=ramfs "\
	"devfs=mount "\
	"console=ttySAC0,115200"
/*----------------------------------------------------------------------------*/

/* tftp server configuration */
//wx: the Head 3 bytes MUST is a valid Value,or else the MAC will set Failed.
#define CONFIG_ETHADDR      12:34:56:85:09:09
#define CONFIG_NETMASK	255.255.255.0
//wx: because my wifi has used 192.168.0.X subnet, and I seted WLAN on 192.168.9.X subnet
#define CONFIG_IPADDR	192.168.9.1
#define CONFIG_SERVERIP 	192.168.9.2
#define CONFIG_GATEWAYIP    192.168.9.2

/* 
 * command string
 * usage: input  "run <cmdname>" on console cmdline.
 */
/*----------------------------------------------------------------------------*/
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"name-uboot=u-boot-nand.bin\0"	\
	"name-kernel=uimage.bin\0"	\
	"name-rootfs=rootfs.bin\0"	\
	"ofs-uboot="  __stringify(IMG_UBOOT_OFFSET) "\0"	\
	"ofs-kernel="  __stringify(IMG_KERNEL_OFFSET) "\0"	\
	"ofs-rootfs="  __stringify(IMG_ROOTFS_OFFSET) "\0"	\
	"size-kernel=" __stringify(IMG_KERNEL_SIZE) "\0"	\
	"size-uboot=" __stringify(IMG_UBOOT_SIZE) "\0"	\
	"loadaddr="  __stringify(CONFIG_SYS_LOAD_ADDR) "\0"	\
	"kerneladdr="  __stringify(CONFIG_BOOT_KERNEL_ADDR) "\0"	\
	"fileaddr="  __stringify(CONFIG_SYS_LOAD_ADDR) "\0"	\
    "boot-kernel-nand=nand read ${kerneladdr} ${ofs-kernel} ${size-kernel};"\
       		"bootm ${kerneladdr}\0"\
	"boot-kernel-tftp=tftp ${kerneladdr} ${name-kernel}; "	\
	 	"bootm ${kerneladdr}\0"	\
	"install2nand-uboot="	\
        	"nand erase ${ofs-uboot} ${filesize};"	\
        	"nand write ${fileaddr} ${ofs-uboot} ${filesize}\0"	\
	"install2nand-kernel="	\
        	"nand erase ${ofs-kernel} ${filesize};"	\
        	"nand write ${fileaddr} ${ofs-kernel} ${filesize}\0"	\
	"install2nand-yaffs="	\
        	"nand erase ${ofs-rootfs} ${filesize};"	\
        	"nand write.yaffs ${fileaddr} ${ofs-rootfs} ${filesize}\0"	\
	"tftp2nand-uboot=tftp ${loadaddr} ${name-uboot};"	\
        	"run install2nand-uboot\0"	\
	"tftp2nand-kernel=tftp ${loadaddr} ${name-kernel};"	\
        	"run install2nand-kernel\0"	\
	"tftp2nand-yaffs=tftp ${loadaddr} ${name-rootfs};"	\
        	"run install2nand-yaffs\0"	\
	"uart2nand-uboot=loadb ${loadaddr};"	\
        	"run install2nand-uboot\0"	\
	"uart2nand-kernel=loadb ${loadaddr};"	\
        	"run install2nand-kernel\0"	\
	"uart2nand-yaffs=loadb ${loadaddr};"	\
        	"run install2nand-yaffs\0"	\
	"nand-erase-env=nand erase " __stringify(IMG_PARAM_OFFSET)" " __stringify(IMG_PARAM_SIZE) "\0"\
	"set-args-yaffs=setenv bootargs "  CONFIG_BOOTARGS_YAFFS "\0"\
	"set-args-ramdisk=setenv bootargs " CONFIG_BOOTARGS_RAMDISK "\0"\
	"set-args-cramfs=setenv bootargs " CONFIG_BOOTARGS_CRAMFS "\0"\
	"set-args-ramfs=setenv bootargs " CONFIG_BOOTARGS_RAMFS "\0"\
	""

/*---------------- boot default -------------------------------------*/
#define CONFIG_BOOTARGS	CONFIG_BOOTARGS_YAFFS
#define CONFIG_BOOTCOMMAND	"run boot-kernel-nand"

#endif	/* __CONFIG_H */
