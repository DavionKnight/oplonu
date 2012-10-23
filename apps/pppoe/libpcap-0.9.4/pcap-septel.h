/*
 * pcap-septel.c: Packet capture interface for Intel Septel card
 *
 * The functionality of this code attempts to mimic that of pcap-linux as much
 * as possible.  This code is only needed when compiling in the Intel/Septel
 * card code at the same time as another type of device.
 *
 * Authors: Gilbert HOYEK (gil_hoyek@hotmail.com), Elias M. KHOURY
 * (+961 3 485343);
 *
 * @(#) $Header: //depot/sw/abu/opconn/release/v3.2.1/apps/pppoe/libpcap-0.9.4/pcap-septel.h#1 $
 */

pcap_t *septel_open_live(const char *device, int snaplen, int promisc, int to_ms, char *ebuf);

