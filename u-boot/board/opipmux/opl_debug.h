/*
=============================================================================
     Header Name: opl_debug.h

     General Description:
general definition for the Opulan Drvier debug system.
===============================================================================
                         Opulan Confidential Proprietary                     
                  ID and version: xxxxxxxxxxxxxx  Version 1.00
                  (c) Copyright Opulan XXXX - XXXX, All Rights Reserved
     

Revision History:
Author                Date              Description of Changes
----------------   ------------  ----------------------------------------------
 yzhang            07/04/2005      Initial Version
----------------   ------------  ----------------------------------------------
*/

#ifndef OPL_DRV_DEBUG_INCLUDE 
#define	OPL_DRV_DEBUG_INCLUDE 

#define	DEBUG_OUTPUT_LEN			178
/*#define FPGA_LOG_MEMORY*/

	/* used for DEBUG switch */
#if DEBUG_GRP_ID == 1
	#define DEBUG_GRP_ID_STR		"[Fpga %04d]"
	#define DEBUG_GRP_FD			ga_fpga_dbg_fd
#else
	#define DEBUG_GRP_ID_STR		"[Opl Driver  %04d:]"
	#define DEBUG_GRP_FD			opl_drv_dbg_fd 	/* global debug control switch */
#endif


#ifdef OPULAN_RTOS_VXWORKS
#ifdef FPGA_LOG_MEMORY
#define opllog(pCtrl, outputFd, fmt) 	{ \
									if (*(pCtrl) && DEBUG_GRP_FD) \
										{\
											gaDebugLog fmt; \
									} \
								}

#else
#define opllog(pCtrl, outputFd, fmt) 	{ \
									if (*(pCtrl) && DEBUG_GRP_FD) \
										{\
										sprintf (snoopyBuf, DEBUG_GRP_ID_STR"\n", __LINE__); \
										if (outputFd) \
											write (outputFd, snoopyBuf, strlen ((char *)snoopyBuf)); \
										gaDebugLog fmt; \
										if (outputFd) \
											write (outputFd, snoopyBuf, strlen ((char *)snoopyBuf)); \
									} \
							}
#endif /*#ifdef FPGA_LOG_MEMORY*/

#else 

	/* for Linux */
#define opllog(pCtrl, outputFd, fmt) 	{ \
									if (*(pCtrl) && DEBUG_GRP_FD) \
										{\
										sprintf (snoopyBuf, DEBUG_GRP_ID_STR, __LINE__); \
										if (outputFd) \
											{ \
											write (OPL_STDOUT, snoopyBuf, strlen ((char *)snoopyBuf)); \
											/*gaDebugLog*/ printf fmt; \
										} \
									} \
							}

#endif /* OPULAN_RTOS_VXWORKS */

	/* no debug control output */
#define oprintf(x)			printf x

/* typedefs */

	/* global */
extern int		opl_drv_dbg_fd;
extern int		snpFd;
extern char	snoopyBuf[DEBUG_OUTPUT_LEN + 1];

	/* module debug control definition */
extern int		nzDbgFd;	/* debug control for modules in charge of yzhang */
extern int		nyDbgFd;	/* debug control for modules in charge of cyang */

/* function declarations */

extern void gaDebugLog (const char * fmt, ...);
extern void bdump (char * base, int len);
extern int uSetTime (int nYear, int nMonth, int nDay, int nHour, int nMinute, int nSecond, unsigned int);
extern char * cdate(void);

#endif /* OPL_DRV_DEBUG_INCLUDE */	

