PROG=proj

SRCS = proj.c drawing.c keyboard.c timer.c utils.c mouse.c Hero.c Terminix.c Game.c Zombie.c Sprite.c RTC.c Menu.c Serial_port.c highscores.c

# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CPPFLAGS += -pedantic #-D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>
