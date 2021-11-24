## -*- mode: Makefile; tab-width: 4; -*-
## ======================================================================================
## Copyright (c) 2000 Palm Computing, Inc. or its subsidiaries. All rights reserved.
## ======================================================================================

R = ../Resources

CC = m68k-palmos-gcc
CFLAGS = -O2 -Wall -I$(R) -g -Wno-unknown-pragmas

ROM_Transfer.prc: ROM_Transfer tver0001.bin
	build-prc ROM_Transfer.prc "ROM Transfer" ROMX ROM_Transfer *.bin

ROM_Transfer: ROM_Transfer.o SerialPortTools.o
	$(CC) -v -g -o ROM_Transfer ROM_Transfer.o SerialPortTools.o

ROM_Transfer.o: ROM_Transfer.c ROM_Transfer.h SerialPortTools.h $(R)/ROM_Transfer.rsrc.h
	$(CC) $(CFLAGS) -c ROM_Transfer.c

SerialPortTools.o: SerialPortTools.c SerialPortTools.h $(R)/ROM_Transfer.rsrc.h
	$(CC) $(CFLAGS) -c SerialPortTools.c

tver0001.bin: $(R)/ROM_Transfer.rcp $(R)/ROM_Transfer_icon.pbitm $(R)/ROM_Transfer.rsrc.h
	pilrc -I $(R) $(R)/ROM_Transfer.rcp

clean:
	-rm -f ROM_Transfer *.o *.bin
