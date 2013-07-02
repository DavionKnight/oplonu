#ifndef __PLAT_CONFIG_H__
#define __PLAT_CONFIG_H__


/*
** Thread Parameters for application threads
*/
#define PCM_THREAD_STACKSIZE                (2 * 4096)
#define SHELL_THREAD_STACKSIZE              (2 * 4096)
#define AAL_THREAD_STACKSIZE                (2 * 4096)
#define TIMER_THREAD_STACK_SIZE             (2 * 4096)
#define VOIP_THREAD_STACKSIZE               (4 * 4096)
#define APPL_THREAD_STACKSIZE               (4 * 4096)
#define DAEMON_THREAD_STACKSIZE             (1 * 4096)
#define LOG_THREAD_STACKSIZE                (1 * 4096)
#define TELNETD_THREAD_STACKSIZE            (2 * 4096)
#define TERM_SERV_THREAD_STACKSIZE          (1 * 4096)
#define POLLING_THREAD_STACKSIZE            (1 * 4096)
#define PPP_THREAD_STACKSIZE            (1 * 4096)
#if 0
#define PCM_THREAD_PRIORITY         5
#define SHELL_THREAD_PRIORITY       6
#define AAL_THREAD_PRIORITY         7
#define TIMER_THREAD_PRIORITY       8
#define VOIP_THREAD_PRIORITY        9
#define APPL_THREAD_PRIORITY        10
#define ECOS_NET_FAST_THREAD_PRI    14
#define ECOS_NET_THREAD_PRI         15
#define ECOS_ATHTTPD_THREAD_PRI     16
#define DAEMON_THREAD_PRIORITY          17
#define LOG_THREAD_PRIORITY         18
#define TELNETD_THREAD_PRIORITY     19

#define POLLING_THREAD_PRIORITY     30

#endif
#define TERM_SERV_UART2NET_THREAD_PRIORITY 90
#define TERM_SERV_IP_RX_THREAD_PRIORITY 100
#define PPP_THREAD_PRIORITY     90
#define TERM_SERV_NET2UART_THREAD_PRIORITY 90


#define PCM_THREAD_NAME             "PCM Thread"
#define SHELL_THREAD_NAME           "Shell Thread"
#define AAL_THREAD_NAME             "AAL Thread"
#define TIMER_THREAD_NAME           "Timer Thread"
#define VOIP_THREAD_NAME            "VOIP Thread"
#define APPL_THREAD_NAME            "APP Thread"
#define DAEMON_THREAD_NAME         "Daemon Thread"
#define LOG_THREAD_NAME             "LOG Thread"
#define TELENTD_THREAD_NAME         "TELNETD Thread"
#define POLLING_THREAD_NAME         "POLLING Thread"
#define PPP_THREAD_NAME         "PPP Thread"


/*
** Init sequence for APP
*/
#define INIT_STEP_GLB       0
#define INIT_STEP_SDL       1
#define INIT_STEP_APP       2
#define INIT_STEP_SERVICE   3


/*
** Priority for application queue
*/
#define APP_QUEUE_PRI_NUM  4
#define APP_QUEUE_PRI_0 0
#define APP_QUEUE_PRI_1 1
#define APP_QUEUE_PRI_2 2
#define APP_QUEUE_PRI_3 3


#endif
