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

#define BIT(x)			(1 << (x))

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
static void print_num(const char *name, ulong value);


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
    _delay_ms_(5);
}


/*
 * Output a single byte to the serial port.
 */
static __inline__ void _uart_putc_(const char c)
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

static int _uart_puts_(const char *str)
{
    int len = 0;
	while (*str) {
		if (*str == '\n')
			_uart_putc_('\r');
		_uart_putc_(*str++);
        len++;
	}

    return len;
}

static void print_x(unsigned int t)
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



#define DEBUG_LL_MODE_LED     0x01
#define DEBUG_LL_MODE_SOUND   0x02



#if defined(CONFIG_MINI2440_LED) || defined(CONFIG_MINI2440_SPEAKER)
static void speaker_led_play(unsigned int mode, int times, int delay)
{  
    struct s3c24x0_gpio *gpio = s3c24x0_get_base_gpio();
    unsigned long val_a, val_b,opt_bits;

    if (!times)
        return;
    
   //gpb 5,6,7,8 => led 1,2,3,4
    //gpb 0 =>speaker
    writel(0x00295551, &gpio->gpbcon);//[10:17]=01010101,set as gpio
    writel(0x000001FF, &gpio->gpbup);//[5:8]=1110 (1:disable pullup)

    val_a = val_b = 0;
#if defined(CONFIG_MINI2440_SPEAKER)
    if (mode & DEBUG_LL_MODE_SOUND) {
        opt_bits = (BIT(0) |BIT(8));//for debug, led 4, opend
        val_a |= opt_bits;
        val_b &= ~opt_bits;
    }
#endif

#if defined(CONFIG_MINI2440_LED)
    if (mode & DEBUG_LL_MODE_LED) {
        opt_bits = (BIT(5) |BIT(6) |BIT(7));
        val_a |= opt_bits;
        val_b &= ~opt_bits;
    }
#endif
    while (times--) {
    	writel(val_a, &gpio->gpbdat);
        _delay_ms_(delay);
        writel(val_b, &gpio->gpbdat);
        _delay_ms_(delay<<1);
    }
}
#endif



void debug_ll(unsigned int num1, unsigned int num2)
{
# if defined(CONFIG_MINI2440_LED)
     speaker_led_play(DEBUG_LL_MODE_LED, 3, 64);
#endif
	_uart_putc_('#');
	print_x(num1);
	_uart_putc_(':');
	print_x(num2);
	_uart_putc_('\r');
	_uart_putc_('\n');
# if defined(CONFIG_MINI2440_LED) || defined(CONFIG_MINI2440_SPEAKER)
	speaker_led_play(DEBUG_LL_MODE_LED | DEBUG_LL_MODE_SOUND, num1&0xf,192);
    _delay_ms_(1024);
#endif
}


void debug_ll_num(const char *name, ulong value)
{
    int len;
    len = _uart_puts_(name);
    while (len++ < 12) {
        _uart_putc_(' ');
    }
    
    if (value != 0)
        print_x(value);
	_uart_putc_('\r');
	_uart_putc_('\n');
}


#endif /* CONFIG_S3C24X0 */
