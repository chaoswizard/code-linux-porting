/*
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
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

#include <common.h>
#ifdef CONFIG_S3C24X0

#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>
#include <asm/arch/iomux.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SERIAL1
#define DEBUG_LL_UART_NR	S3C24X0_UART0

#elif defined(CONFIG_SERIAL2)
#define DEBUG_LL_UART_NR	S3C24X0_UART1

#elif defined(CONFIG_SERIAL3)
#define DEBUG_LL_UART_NR	S3C24X0_UART2

#else
#error "Bad: you didn't configure serial ..."
#endif

#include <asm/io.h>


#define PLL_M_VAL (((CONFIG_DEFAULT_MPLLCON & 0xFF000) >> 12) + 8)
#define PLL_P_VAL (((CONFIG_DEFAULT_MPLLCON & 0x003F0) >> 4) + 2)
#define PLL_S_VAL (CONFIG_DEFAULT_MPLLCON & 0x3)


/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * CONFIG_SYS_CLK_FREQ should be defined as the input frequency of the PLL.
 *
 * _get_fclk_(), _get_hclk_(), _get_pclk_() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */
#if defined(CONFIG_S3C2440)
#define _get_fclk_()  (2 * PLL_M_VAL * (CONFIG_SYS_CLK_FREQ / (PLL_P_VAL << PLL_S_VAL)))
#else
#define _get_fclk_()  ((CONFIG_SYS_CLK_FREQ * PLL_M_VAL) / (PLL_P_VAL << PLL_S_VAL))
#endif


#define _HCLK_MODE (CONFIG_DEFAULT_CLKDIV & 0x6)

#if (2 == _HCLK_MODE)
#define _get_hclk_() (_get_fclk_() / 2)
#elif (4 == _HCLK_MODE)
#define _get_hclk_() (_get_fclk_() / 4)
#else
#define _get_hclk_() _get_fclk_()
#endif

#if (CONFIG_DEFAULT_CLKDIV & 1)
#define _get_pclk_() (_get_hclk_() / 2)
#else
#define _get_pclk_() _get_hclk_() / 2
#endif


static void _delay_ms_(unsigned int t)
{
    int j,k;

    while(t--)
    {      
        k = 1000*40;   
        __asm__ __volatile__("": : :"memory");
        for (;k>0;k--) {
            j = 1000000;
            __asm__ __volatile__("": : :"memory");
            while(j--);
        }
   }
}




#ifdef CONFIG_HWFLOW
static int hwflow;
#endif


/* Initialise the serial port. The settings are always 8 data bits, no parity,
 * 1 stop bit, no start bits.
 */
void debug_ll_init(void)
{
	struct s3c24x0_uart *uart = s3c24x0_get_base_uart(DEBUG_LL_UART_NR);
    struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
	unsigned int reg = 0;
	int i;


    /* IOMUX Port H : UART Configuration */
    gpio->gphcon = IOMUXH_nCTS0 | IOMUXH_nRTS0 | IOMUXH_TXD0 | IOMUXH_RXD0 |
        IOMUXH_TXD1 | IOMUXH_RXD1 | IOMUXH_TXD2 | IOMUXH_RXD2;



#ifdef CONFIG_HWFLOW
	hwflow = 0;	/* turned off by default */
#endif

	/* FIFO enable, Tx/Rx FIFO clear */
	writel(0x07, &uart->ufcon);
	writel(0x0, &uart->umcon);

	/* Normal,No parity,1 stop,8 bit */
	writel(0x3, &uart->ulcon);
	/*
	 * tx=level,rx=edge,disable timeout int.,enable rx error int.,
	 * normal,interrupt or polling
	 */
	writel(0x245, &uart->ucon);

#ifdef CONFIG_HWFLOW
	writel(0x1, &uart->umcon);	/* rts up */
#endif

	/* FIXME: This is sooooooooooooooooooo ugly */
#if defined(CONFIG_ARCH_GTA02_v1) || defined(CONFIG_ARCH_GTA02_v2)
	/* we need auto hw flow control on the gsm and gps port */
	if (DEBUG_LL_UART_NR == 0 || DEBUG_LL_UART_NR == 1)
		writel(0x10, &uart->umcon);
#endif
	/* value is calculated so : (int)(PCLK/16./baudrate) -1 */
	reg = _get_pclk_() / (16 * CONFIG_BAUDRATE) - 1;

	writel(reg, &uart->ubrdiv);
	for (i = 0; i < 100; i++)
		/* Delay */ ;

}


/*
 * Output a single byte to the serial port.
 */
static void _uart_putc_(const char c)
{
	struct s3c24x0_uart *uart = s3c24x0_get_base_uart(DEBUG_LL_UART_NR);

	while (!(readl(&uart->utrstat) & 0x2))
		/* wait for room in the tx FIFO */ ;

#ifdef CONFIG_HWFLOW
	while (hwflow && !(readl(&uart->umstat) & 0x1))
		/* Wait for CTS up */ ;
#endif

    writeb(c, &uart->utxh);
}

#if 0
static void puts(const char *str)
{
	while (*str) {
		if (*str == '\n')
			_uart_putc_('\r');
		_uart_putc_(*str++);
	}
}
#endif

static void print_num(unsigned int t)
{
	int i, k=7;
	char buf[8];

	for (i=0; i<8; i++)
	    buf[i] = '0';

	if (t != 0)
	for(;;) {
		if (t > 15)
			i = t % 16;
		else
			i = t;

		if (i < 10)
			buf[k--] = i + '0';
		else if(i < 16)
	       buf[k--] = i - 10 +'a';

		if (t < 15)
			break;
		t >>= 4;
	}

	for(i =0 ;i < 8 ; i++)
		_uart_putc_(buf[i]);
}




#if defined(CONFIG_MINI2440_LED) || defined(CONFIG_MINI2440_SPEAKER)
#define LED_SPK_VAL(a,b,c,d,s) (((a)<<8) | ((b)<<7) | ((c)<<6) | ((d)<<5) | ((s)<<0))
static void speaker_led_play(int times)
{  
    struct s3c24x0_gpio *gpio = s3c24x0_get_base_gpio();
    
    if (!times)
        return;
    
   //gpb 5,6,7,8 => led 1,2,3,4
    //gpb 0 =>speaker
    writel(0x00295551, &gpio->gpbcon);//[10:17]=01010101,set as gpio
    writel(0x000001FF, &gpio->gpbup);//[5:8]=1110 (1:disable pullup)

       
    while (times--) {
#if defined(CONFIG_MINI2440_SPEAKER)
	writel(LED_SPK_VAL(0,1,1,1,1), &gpio->gpbdat);
#else
	writel(LED_SPK_VAL(1,1,1,1,0), &gpio->gpbdat);
#endif
        _delay_ms_(200);
        writel(LED_SPK_VAL(0,0,0,0,0), &gpio->gpbdat);
        _delay_ms_(400);
    }
   writel(LED_SPK_VAL(0,0,1,0,0), &gpio->gpbdat);
    _delay_ms_(1000);
}
#endif



void debug_ll(unsigned int num1, unsigned int num2)
{
	print_num(num1);
	_uart_putc_(':');
	print_num(num2);
	_uart_putc_('\r');
	_uart_putc_('\n');
# if defined(CONFIG_MINI2440_LED) || defined(CONFIG_MINI2440_SPEAKER)
	speaker_led_play(num1);
#endif
}













#endif /* CONFIG_S3C24X0 */
