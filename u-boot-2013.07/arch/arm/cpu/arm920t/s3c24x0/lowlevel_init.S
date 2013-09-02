/*
 * Memory Setup stuff - taken from blob memsetup.S
 *
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl) and
 *                     Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *
 * Modified for the Samsung SMDK2410 by
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 * Copyright (C) 2013 pingfengluo@gmail.com
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


#include <config.h>
#include <version.h>


/* some parameters for the board */

/*wx: memery bank controler base address*/
#define BWSCON	0x48000000

/* BWSCON wx: data bus width(chipwidth)*/
#define DW8	      (0x0)
#define DW16			(0x1)
#define DW32			(0x2)
#define WAIT			(0x1<<2)
#define UBLB			(0x1<<3)

#define B1_BWSCON		(DW32)
#define B2_BWSCON		(DW16)
#define B3_BWSCON		(DW16 + WAIT + UBLB)
#define B4_BWSCON		(DW16)
#define B5_BWSCON		(DW16)
/*wx:mini2440 SDRAM use 2 piece K4S561632N(16bit) run parallely = 32*/
#define B6_BWSCON		(DW32) 
#define B7_BWSCON		(DW32)

/* BANK0CON */
#define B0_Tacs			0x0	/*  0clk */
#define B0_Tcos			0x0	/*  0clk */
#define B0_Tacc			0x7	/* 14clk */
#define B0_Tcoh			0x0	/*  0clk */
#define B0_Tah			0x0	/*  0clk */
#define B0_Tacp			0x0
#define B0_PMC			0x0	/* normal */

/* BANK1CON */
#define B1_Tacs			0x0	/*  0clk */
#define B1_Tcos			0x0	/*  0clk */
#define B1_Tacc			0x7	/* 14clk */
#define B1_Tcoh			0x0	/*  0clk */
#define B1_Tah			0x0	/*  0clk */
#define B1_Tacp			0x0
#define B1_PMC			0x0

#define B2_Tacs			0x0
#define B2_Tcos			0x0
#define B2_Tacc			0x7
#define B2_Tcoh			0x0
#define B2_Tah			0x0
#define B2_Tacp			0x0
#define B2_PMC			0x0

#define B3_Tacs			0xc
#define B3_Tcos			0x7
#define B3_Tacc			0xf
#define B3_Tcoh			0x1
#define B3_Tah			0x0
#define B3_Tacp			0x0
#define B3_PMC			0x0

/*wx:For LAN*/
#define B4_Tacs			0x1
#define B4_Tcos			0x1
#define B4_Tacc			0x7
#define B4_Tcoh			0x1
#define B4_Tah			0x1
#define B4_Tacp			0x0
#define B4_PMC			0x0	/* normal */

#define B5_Tacs			0xc
#define B5_Tcos			0x7
#define B5_Tacc			0xf
#define B5_Tcoh			0x1
#define B5_Tah			0x0
#define B5_Tacp			0x0
#define B5_PMC			0x0	/* normal */

/*wx:For SDRAM K4S561632N*/
#define B6_MT			0x3	/*SDRAM => 0x3 ()[00(rom:sram)/11(sdram)/other(reserved)]*/
#define B6_Trcd			0x1 /*20ns => [00(2clk)/01(3clk)/02(4clk)]*/
#define B6_SCAN			0x1	/*C0-C8 => 9bit [00(8bit)/01(9bit)/10(10bit)]*/

#define B7_MT			0x3	/* SDRAM */
#define B7_Trcd			0x1	/* 3clk */
#define B7_SCAN			0x1	/* 9bit */

/* REFRESH parameter */
#define REFEN			0x1	/* Refresh enable */
#define TREFMD			0x0	/* CBR(CAS before RAS)/Auto refresh */
#define Trc			   0x3	/* 65~70ns =>7clk */
#define Tchr			   0x2	/* 3clk, wx:sdram neednot this item */

#if defined(CONFIG_S3C2440)
#define Trp			    0x2	/* 20ns =>4clk */
#define REFCNT			1113	/* period=15.6us, HCLK=60Mhz, (2048+1-15.6*60) */
#else
#define Trp			0x0	/* 2clk */
#define REFCNT			0x0459
#endif


/**************************************/

_TEXT_BASE:
	.word	CONFIG_SYS_TEXT_BASE

.globl lowlevel_init
lowlevel_init:
	/* memory control configuration */
	/* make r0 relative the current location so that it */
	/* reads SMRDATA out of FLASH rather than memory ! */
	ldr     r0, =SMRDATA
	ldr	r1, _TEXT_BASE
	sub	r0, r0, r1/*calc the offset between entry and SMRDATA */
	ldr	r1, =BWSCON	/* Bus Width Status Controller */
	add     r2, r0, #13*4
0:
	ldr     r3, [r0], #4
	str     r3, [r1], #4
	cmp     r2, r0
	bne     0b

	/* everything is fine now */
	mov	pc, lr

	.ltorg
/* the literal pools origin */

SMRDATA:
/*wx:bank data bus(bank 0 neednot setup, it decide by the OM[1:0] PIN by board config)*/
/* BWSCON  */     .word (0+(B1_BWSCON<<4)+(B2_BWSCON<<8)+(B3_BWSCON<<12)+(B4_BWSCON<<16)+(B5_BWSCON<<20)+(B6_BWSCON<<24)+(B7_BWSCON<<28))
/* BANKCON0 */    .word ((B0_Tacs<<13)+(B0_Tcos<<11)+(B0_Tacc<<8)+(B0_Tcoh<<6)+(B0_Tah<<4)+(B0_Tacp<<2)+(B0_PMC))
/* BANKCON1 */    .word ((B1_Tacs<<13)+(B1_Tcos<<11)+(B1_Tacc<<8)+(B1_Tcoh<<6)+(B1_Tah<<4)+(B1_Tacp<<2)+(B1_PMC))
/* BANKCON2 */    .word ((B2_Tacs<<13)+(B2_Tcos<<11)+(B2_Tacc<<8)+(B2_Tcoh<<6)+(B2_Tah<<4)+(B2_Tacp<<2)+(B2_PMC))
/* BANKCON3 */    .word ((B3_Tacs<<13)+(B3_Tcos<<11)+(B3_Tacc<<8)+(B3_Tcoh<<6)+(B3_Tah<<4)+(B3_Tacp<<2)+(B3_PMC))
/* BANKCON4 */    .word ((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC))
/* BANKCON5 */    .word ((B5_Tacs<<13)+(B5_Tcos<<11)+(B5_Tacc<<8)+(B5_Tcoh<<6)+(B5_Tah<<4)+(B5_Tacp<<2)+(B5_PMC))
/* BANKCON6 */    .word ((B6_MT<<15)+(B6_Trcd<<2)+(B6_SCAN))
/* BANKCON7 */    .word ((B7_MT<<15)+(B7_Trcd<<2)+(B7_SCAN))
/* REFRESH  */    .word ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Trc<<18)+(Tchr<<16)+REFCNT)
    /*wx: bank6/7 size(mini2440 is 2 piece K4S561632N(32M:[4M x 16Bit x 4Bank]) Share Bank6(All CS Connected to nGCS6))
    *     therefor The Bank6 mapped to 64M Address, and the BK76MAP bit[2:0] = 001, means 64M byte.
    */
/* BANKSIZE */    .word 0xb1 /* enable burst operation */
/* MRSRB6   */    .word 0x30
/* MRSRB7   */    .word 0x30

