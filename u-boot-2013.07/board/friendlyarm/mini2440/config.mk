#
# (C) Copyright 2002
# Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
# David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
#
# (C) Copyright 2008
# Guennadi Liakhovetki, DENX Software Engineering, <lg@denx.de>
#
# (C) Copyright 2013 pingfengluo.github.com
# pingfengluo@gmail.com
#

sinclude $(OBJTREE)/board/$(BOARDDIR)/config.tmp

ifndef CONFIG_NAND_SPL
CONFIG_SYS_TEXT_BASE =  0x33000000
else
CONFIG_SYS_TEXT_BASE = 0
endif
