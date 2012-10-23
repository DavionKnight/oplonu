# $Id: //depot/sw/abu/opconn/release/v3.2.1/apps/mtd/Makefile#1 $
CC = mips-linux-uclibc-gcc
all: mtd

%.o: %.c
	$(CC) -I. $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $^

mtd: mtd.o
	$(CC) -o $@ $^

clean:
	rm -f *.o mtd
