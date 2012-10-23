/***************************************************************************
*Ipmux_wz_xmodem.c
*
*                   this file used for updating program with xmodem .
*
*****************************************************************************/



/* #include <String.h> */
#include <common.h>
#include <command.h>
#if 0
#include "ipmux_2_uart.h"
#else
#include "ipmux_uart.h"
#endif

extern int Xdown(char *destaddr);

#if defined(CONFIG_IPMUX_ONU) 
#define UART0_ADDR  0xBF001800
#elif defined(CONFIG_ONU_2)
   #define UART0_ADDR  0xBF070000
#else
#define UART0_ADDR  0xBF004000
#endif
#define COEFFICIENT 500000   //(500000*(CPU_CLOCK_RATE/REF_CPU_CLOCK_RATE))  /**this coefficient may be not right,we should check it***CPU_CLOCK_RATE)/(203125000*2)*/
#define RESENDTIME 3

/***error type***/
#define XDOWNSUCCESSFUL 0
#define SYNCERROR  -1
#define RETRYTIMEOUT -2
 #define TRANSMISSIONABORLT  -3
// #define GETPACKETERROR  -4
// #define VERIFYCODEERROR  -5
// #define HEADERROR   -6

/* Line control codes */
#define SOH   0x01 /* start of header for Xmodem protocol  */
#define STX   0x02 /* start of header for 1K xmodem protocol*/
#define ACK   0x06 /* Acknowledge */
#define NAK   0x15 /* Negative acknowledge */
#define CAN   0x18 /* Cancel */
#define EOT   0x04 /* end of text */
#define TIMEOUT 1*COEFFICIENT
#define TIMEOUT_LONG 10*COEFFICIENT
#define XMODEM_DATA_SIZE_SOH 128  /* for Xmodem protocol */
#define XMODEM_DATA_SIZE_STX 1024 /* for 1K xmodem protocol */
#define MAXRETRANS 25
#define TRANSFINISH 1
#define TRANSABORT 2 
#define switch_State(x, y) (x = y)

#define zw_debug 0

#undef  ZWDEBUG
#ifdef  ZWDEBUG
#define ZWDEBUG
#define ZWHEAD 'h'
#define ZWCOUNT 'j'
#define ZW_CODE 'k'
#define ZWCKCODE'l'
#define ZWDATASIZE 16
#define ZWFINISH 'v'
#endif


// unsigned char xbuff[1030];
static const unsigned short crc16tab[256]= {
    0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
    0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
    0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
    0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
    0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
    0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
    0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
    0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
    0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
    0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
    0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
    0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
    0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
    0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
    0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
    0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
    0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
    0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
    0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
    0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
    0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
    0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
    0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
    0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
    0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
    0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
    0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
    0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
    0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
    0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
    0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
    0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};
  
/******************************************************************************
*
* xmodemPutc - transmitting a single chacter 
*
* INPUT:  c -- character to be transmitted.
* 
* RETURNS: N/A
*/
void xmodemPutc(const char cValue) 
{
serial_putc (cValue);
}

/******************************************************************************
*
* xmodemGetc - retrieve one character from buffer 
*
* input: wait for "timeout" secones.
*
* RETURNS:  receiving character.
*/
#if 0
int xmodemGetc (volatile unsigned int timeout,char * ptmpchar)
{
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
	while((timeout)--)
           if((pUart->lsr & LSR_DR ) != 0)
    	      {
               *ptmpchar  = pUart->w2.urhr;
	         return 1;
              } 
	  return 0;
}
#else
int xmodemGetc(volatile unsigned int timeout,char * ptmpchar) 
{    
      volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR); 
      while((timeout)--)
          if ((pUart->lsr & LSR_DR ) != 0)
          	{
             *ptmpchar = pUart->w0.rbr;	   
		  return 1;
          	} 
	  return 0;
  }
#endif	  
static void flushinput(void)
{
    volatile int c;
    while (xmodemGetc(TIMEOUT, &c) > 0)
        continue;
}

unsigned short crc16_ccitt(const void *buf, int len)
{
    int counter;
    unsigned short crc = 0;
    for( counter = 0; counter < len; counter++)
        crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
    return crc;
}


static int check(int crc, const unsigned char *buf, int sz)
{
    if (crc) {
        unsigned short crc = crc16_ccitt(buf, sz);
        unsigned short tcrc = (buf[sz]<<8)+buf[sz+1];
        if (crc == tcrc)
            return 1;
    }
    else {
        int i;
        unsigned char cks = 0;
        for (i = 0; i < sz; ++i) {
            cks += buf[i];
        }
        if (cks == buf[sz])
        return 1;
    }

    return 0;
}

/******************************************************************************
*
* Xdown - download a  file  from sender.
*
* input:  dest address
*
* RETURNS:  status.
*/


int Xdown(char *destaddr)
{
    unsigned char *p;
    int bufsz, crc = 0;
    volatile char tmpc;
    int zw=0;
  //  volatile char checksend;
    unsigned char tmpbuff[1030];
    unsigned char trychar = 'C';
    unsigned char packetno = 1;
    int i;
    char *dest = destaddr;
    int retry, retrans = MAXRETRANS;
    int state=0;
    char debugX[100];
    char debugxc=0;
//    char *pflend;
#if 0   /** used for debug link **/
   int in[100];
   unsigned int ii=0, j; 

   while (serial_tstc() == 0)
   {
	   xmodemPutc('C'); 
	   udelay(100000);
   }
   for(j=0;j<64;j++)
   {
 xmodemGetc(TIMEOUT, &tmpc);
     in[j]=tmpc;
   	in[j]=serial_getc();
	if(in[j]==0x01)
		{
		goto rcv_out;
		}
   }
  while(1) 
  	printf("no head\t");
 rcv_out:
in[0]=in[j];
    for(ii=1;ii<64;ii++)
   {
   	in[ii]=serial_getc();
	udelay(100);
   }
   while(1)
   {
	   for(ii=0;ii<64;ii++)
	   {
	   	printf("%d : %08x.\n", ii, in[ii]);
	   }   
	    udelay(10000000);
   }   
#endif   
   for(;;) {
        
        for( retry = 0; retry < 600; retry++) {  // approx 30 seconds allowed to make connection

            if (trychar) {
        
	       xmodemPutc('C'); 
#ifndef 	CONFIG_IPMUX_E2_C   /*zw 922 add*/
	       udelay(10000);
#else
	       udelay(10000);  
#endif  
            	}
            if ((xmodemGetc(TIMEOUT, &tmpc))) {
                                
                
                switch (tmpc) {
                case SOH:
			{
                     bufsz = XMODEM_DATA_SIZE_SOH;

                	}     
                    goto start_recv;
                case STX:
			{
                    bufsz = XMODEM_DATA_SIZE_STX;
	
                	}
                    
                    goto start_recv;
                case EOT:           
                    /* recieve finish */     
                    	{
                    flushinput();
                    xmodemPutc(ACK);

			 udelay(10000);
			 switch_State(state,TRANSFINISH);
			goto result;

               //     return (dest-destaddr); /* normal end */
                    	}
                case CAN:
                    
                    if (xmodemGetc(TIMEOUT, &tmpc)>0)
                    {
    
                        if (tmpc == CAN) {
                            flushinput();
                            xmodemPutc(ACK);

			 udelay(10000);
			 switch_State(state,TRANSABORT);
			goto result;				
                //            return -1; /* canceled by remote */
                        }
                    	}
                    break;
	    case 0x03:           
                    /* recieve finish */     
                    	{
                    flushinput();
           //         xmodemPutc(ACK);

			 udelay(10000);
			 switch_State(state,TRANSABORT);
			  udelay(1000000);
			goto result;

               //     return (dest-destaddr); /* normal end */
                    	}					

	
                default:
		{
#if	0
                   debugX[debugxc]=tmpc;
			debugxc++;   
			if(debugxc==0x5)
				{
				while(1)
					{
					for(debugxc=0;debugxc<=5;debugxc++)
					printf("debugX[0]=0x%x\n",debugX[debugxc]);
					}
				}
#endif			
                    break;
                	}
                }
            }
		else {
				}
        }
      
        if (trychar == 'C') { 
            trychar = NAK; 
            continue;
        }
        flushinput();
        xmodemPutc(CAN);
        xmodemPutc(CAN);
        xmodemPutc(CAN);
        
        return SYNCERROR; /* sync error */

    start_recv:
        
  
       if (trychar == 'C') 
            crc = 1;
        trychar = 0;
        p = tmpbuff;
        *p++ = tmpc;
        for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) {
 
            if (xmodemGetc(TIMEOUT, &tmpc) == 0) 
            {

                goto reject;
            }
            *p++ = tmpc;
#if 1                                               /*check packt number and complement*/			
	  if(i==1)
	      {
	       if(tmpbuff[1] != packetno )
	         goto reject;
		}
	   if(i==2)
	      {
	       if(tmpbuff[1] != (unsigned char)(~tmpbuff[2]) )
	         goto reject;
		}
#endif	
        }
        while(0) /*zw_debug*/
        {
        	for(zw=0;zw<(bufsz+(crc?1:0)+3);++zw)
			{
			printf("%d: 0x%x\n",zw,tmpbuff[zw]);

			udelay(100000);
        		}
        }
 
  /*      if (tmpbuff[1] == (unsigned char)(~tmpbuff[2]) && 
            (tmpbuff[1] == packetno || tmpbuff[1] == (unsigned char)packetno-1) &&
                check(crc, &tmpbuff[3], bufsz)) {  */
                
       if (check(crc, &tmpbuff[3], bufsz)) {               
           if (tmpbuff[1] == packetno)    {

                  memcpy (dest, &tmpbuff[3], bufsz);
		   dest+=bufsz;
    
                }
         	{
                ++packetno;
                retrans = /* MAXRETRANS*/ 25+1;
            	}
            if (--retrans <= 0) {
                
                flushinput();
                xmodemPutc(CAN);
                xmodemPutc(CAN);
                xmodemPutc(CAN);
              
                return RETRYTIMEOUT; /* too many retry error */
            }
            
          
            xmodemPutc(ACK);
            continue;
        	}
    reject:
        flushinput();
        xmodemPutc(NAK);
    }
   result:
   	switch(state){
		
		case TRANSFINISH:
		   {
                   
	//	     pflend=(unsigned char*)dest;
	//printf("finish!!\n");
	//printf("0x%x\n",dest);
	//printf("0x%x\n",dest-1);
		    while(0x1a==*(dest-1))
		    	{
		    	    
				dest--;
			//	*dest=0xff;
		    	}
		     printf("The total size of the file trasfered is %d kbytes\n",(dest-destaddr)/(1024)); 
		     return dest-destaddr;
		   }
		
		case TRANSABORT:
		   {
		   printf("\nThe sender aborted transmission,  %d kbytes transfered.\n",(dest-destaddr)/(1024));
		   return TRANSMISSIONABORLT;   
		  }
		
		default:
			break;
   		}
}

int do_Xdown(cmd_tbl_t *cmdtp,int flag,int argc, char *argv[])
{

	unsigned int  addr;	
	int rcode = 0;
	int Xresult;

	if (argc == 2) {
		addr = simple_strtoul(argv[1], NULL, 16);
	}

	puts ("## Ready for Xdown ...\n");  


	Xresult = Xdown (addr);
	
	if(Xresult>0)
	 {
   /*    printf("##debug:after Xdown, Start addr=%x\n",addr); */
	   rcode=0;
	 }
	else
	  rcode=-1;
	return rcode;

}


U_BOOT_CMD(
  Xdown,    2,    0,     do_Xdown,
 "Xdown      - load binary file over serial line (xmodem)\n",
 "Xdown(destaddr),destaddr appoint the address  in memory used for storeing the binary file\n"
);


#if 0

/*
 enum state_of_reciver{
 INVALID_STATE = -1,
 WAITING_START,
 WAIT_HEAD,
 RX_PACKET,
 RX_EOT,
 SEND_NAK
};

*/
/* gloadble variable*/
// static char  packetNumber;
// static int     PACKET_SIZE;
// static char  HEAD;
// static volatile unsigned int tmpchar;
// static  volatile unsigned int* ptmpchar=&tmpchar;
//static int     resend_count=0;
//static int     errorno=0;

//#define switch_State(x, y) (x = y)

/*	  
	for(count=0;count<(timeout*COEFFICIENT*2);count++ )                         
        {
          if ((pUart->lsr & LSR_DR ) != 0)
	   break;
	 }
       if(count<timeout*COEFFICIENT){
       *ptmpchar = pUart->w0.rbr;
	  return 1;
      	}
	  else
	  return 0;
*/
/*
    unsigned char c ;
    volatile struct uart *pUart = (volatile struct uart*)(UART0_ADDR);
    
    while((pUart->lsr & LSR_DR ) == 0);
    c = pUart->w0.rbr;

    return c;
*/
	  

/******************************************************************************
*
* xmodemCheckPakt - check data packet from sender.
*
* input: 
*
* RETURNS:  status.
*/
#if 0
 int xmodemGetPakt(char *xtmpBuf) 
{
 int  count;
/*  static char buffer[128]; */
int timeout=TIMEOUT;

 for (count = 0; count < PACKET_SIZE; count++)   /*start recieve data in file which we want to download*/
    {
      if (xmodemGetc(timeout))   
      	{
	  xtmpBuf[count]=*ptmpchar;          /*put the data in the buffer*/
#ifdef ZWDEBUG
        printf("get %d char\n",count+1);
#endif
      	}
      else
 	{
	  errorDebug(GETCHARFAIL);
         return 0;
       }
    }
/*  tmpBuf=buffer; */
 return (1);
}
#endif
/******************************************************************************
*
* getVerifyCode -get verify code of the packet from sender.
*
* input: 
*
* RETURNS:  status.
*/
/*
char getVerifyCode(char *databuffer)
{
 int verifydata,i;
 for(i=0;i<PACKET_SIZE-1;i++)
     {
     databuffer[i+1]+=databuffer[i];
     }
 verifydata=databuffer[i+1];
 return  verifydata;
}
*/


int XXXdown(char *destaddr) 
{
   int  state;
   char *tmpbuffer;
   char v_code;
   int retrytimes;
   int xtime;
  /*  tmpbuffer=malloc(PACKET_SIZE);*/
  int timeout=TIMEOUT;
  
 printf("start dowload......\n");  
    packetNumber = 1;
    state = WAITING_START;
    PACKET_SIZE = XMODEM_DATA_SIZE_SOH;
    HEAD = SOH;
   

   //flushinput();

  #if 0 
   int in[100];
   unsigned int i=0, j;  

   while (serial_tstc() == 0)
   {
	 /*  *( volatile unsigned int*)0xbf001800=0x43;   */
	  xmodemPutc('C'); 
	   udelay(100000);
   }
   
   for(j=0;j<10;j++)
   {
   	in[i++]=serial_getc();
   }
   
   while(1)
   {
	   for(j=0;j<10;j++)
	   {
	   	printf("%d : %08x.\n", j, in[j]);
	   }   
	    udelay(100000);
   }
   
#endif   
   for (;1;) 
   	{	
/*reciever state WAITING_START  */
WAIT_START:
       if (state == WAITING_START) 
	   {
	   for(retrytimes=0;retrytimes<30;retrytimes++)
	   	{
	     /*notify sender to send data*/
         //     xmodemPutc('C');  
         *( volatile unsigned int*)0xbf001800=0x43;
       
	  /*    *ptmpchar=0xff;   *//**zw add for debug.need be delete**/
               if (xmodemGetc(timeout))                /*GET THE FIRST BYTE*/
		 {
		  /*    printf("the char got currently is 0x%x\n",*ptmpchar);*/
		      while(1)
                   printf("\ninvalid!!!:@@@@@@the char got currently is %x\n",serial_getc()/* *ptmpchar*/); /**zw add for debug.need be delete**/
		
		    switch(*ptmpchar) 
		      {
                       case SOH :
                         {
                           PACKET_SIZE = XMODEM_DATA_SIZE_SOH;
                           HEAD = SOH;
                           switch_State(state, RX_PACKET);
			      goto RE_RCV  ;        
                           }
                    
                         case STX:						  
                          {
                            PACKET_SIZE = XMODEM_DATA_SIZE_STX;				    
                            HEAD = STX;
                            switch_State(state, RX_PACKET);
			      goto RE_RCV  ;    
                          }
		#ifdef ZWDEBUG					 
                             case ZWHEAD:						  
                          {
                            PACKET_SIZE = ZWDATASIZE;				    
                            HEAD = ZWHEAD;
                            switch_State(state, RX_PACKET);
				printf("ZWHEAD ZWHEAD ZWHEAD:the char got currently is 0x%x\n",*ptmpchar);
			      goto RE_RCV  ;      
				  	
                          }                    
		#endif 				 
			   default :
			   		{
		#ifndef ZWDEBUG	
		                    while(1)
				        printf("\ninvalid!!!:the char got currently is %x\n",*ptmpchar); /**zw add for debug.need be delete**/
					/* goto WAIT_START;*/ /*need it*/
		//			switch_State(state, RX_PACKET);  /**zw add for debug.need be delete**/
	        #endif  		
		#ifdef    ZWDEBUG
                                    printf("\ninvalid head!!!:the char got currently is %c\n",*ptmpchar);
		#endif
					 break; 
					 /*
					while(0)    
						{
						  xmodemGetc(timeout);
						  printf("the char got currently is %c\n",*ptmpchar);
					         printf("DEBUG: receiver had tried %d times\n",retrytimes+1);	 
						}
			                */
			   	    }
		    	 }
		 //  break;
                 }
		  else  /*can't get char from uart*/
		    {
		  /*  xmodemPutc(CAN);   */      /*notify sender to stop sending data*/
		     errorDebug(GETCHARFAIL);
		    
		    return GETCHARFAIL;
		
		    }
#ifdef ZWDEBUG
	 printf("DEBUG: receiver had tried %d times\n",retrytimes);
#endif		  
		  		  
	   	}
	 /*  	   return  0; *//**zw add for debug.need be delete**/

	  if(retrytimes==30)
	  	{
	  	return  HEADERROR;
	  	}
   } 
	   

printf("\nbefore RE_RCV:\n");
RE_RCV:	   
/*reciever state WAITING_START  */
    if (state == WAIT_HEAD) 
	{
         if (xmodemGetc( TIMEOUT_LONG))      /*GET THE FIRST BYTE*/ 
	   {   
        
	   switch(*ptmpchar) 
	   	{
                case SOH :  /*nextchar==SOH*/
                     {
                        PACKET_SIZE = XMODEM_DATA_SIZE_SOH;
                        HEAD = SOH;
                        switch_State(state, RX_PACKET);
			   break ;			
                       }
         /*   else */
                case STX :/*nextchar==STX*/
                     {
                       PACKET_SIZE = XMODEM_DATA_SIZE_STX;
                       HEAD = STX;
                       switch_State(state, RX_PACKET);
			  break ;	   
                      }
     
                case  EOT :
		       {
    /* TransitionState(state, RX_EOT);*/
                      xmodemPutc(ACK);
    /*             return (getCntPersent());  */
	               break ;	
                     }
#ifdef ZWDEBUG					 
                             case ZWHEAD:						  
                          {
                            PACKET_SIZE = ZWDATASIZE;				    
                            HEAD = ZWHEAD;
                            switch_State(state, RX_PACKET);
				printf("\nZWHEAD ZWHEAD ZWHEAD:the char got currently is 0x%x\n",*ptmpchar);	/**zw add for debug.need be delete**/		
			      break;
		
			   while(0)     
				  	printf("STX STX STX:the char got currently is 0x%x\n",*ptmpchar);
                          }        
		case ZWFINISH:
				{
			    printf("\ndebug finished\n");
			   return 0;
			}
#endif 	
		
		default :
				{
					 printf("\ninvalid!!!:the char got currently is %c\n",*ptmpchar); /**zw add for debug.need be delete**/
					/* goto WAIT_START;*/ /*need it*/
				//	switch_State(state, RX_PACKET);  /**zw add for debug.need be delete**/
			      break ;
			}
	   	}
		   
         }
	else
	 {
	   errorDebug(GETCHARFAIL);
	   return GETCHARFAIL;
	 }
  }

printf("\nbefore RECIEVE_PACKET:\n");
RECIEVE_PACKET:
 /*reciever state RX_PACKET  */
    if (state == RX_PACKET) 
	{
         if (xmodemGetc(timeout))               /*GET THE SECOND BYTE*/
	   {	 
/*check packet number,if not right tell sender resend*/
 printf("\nGET THE SECOND BYTE:the char got currently is %c\n",*ptmpchar);
#ifdef ZWDEBUG
            if (*ptmpchar == 'j')
#else
            if (*ptmpchar == packetNumber) 
#endif
			resend_count=0;			
	     else
              {
               if(resend_count<RESENDTIME)
                 {
                   printf("\nPACKETNUMERROR:resend_count = %d\n",resend_count);
                   xmodemPutc(NAK);
	            switch_State(state, WAIT_HEAD);
		     resend_count++;
                   goto RE_RCV;
                 }
		 else
		   {
		    errorDebug(PACKETNUMERROR);
		   printf("\nPACKETNUMERROR:timeout!!!!!\n");
	           return PACKETNUMERROR;
		    }
	     	 }
         }  
	   else
	  {
          errorDebug(GETCHARFAIL);
	   return GETCHARFAIL;
	   }
    	
	 if (xmodemGetc( timeout))       /*GET THE THIRD BYTE*/
          {
          printf("\nGET THE THIRD BYTE:the char got currently is %c\n",*ptmpchar);
	#ifdef ZWDEBUG
            if (*ptmpchar == 'k')
	#else
            if (*ptmpchar == (char)~packetNumber)  /*check packet numbe with complement code*/
	#endif			
               resend_count=0;
	     else
              {
               if(resend_count<RESENDTIME)
                 {
                   xmodemPutc(NAK);
	            switch_State(state, WAIT_HEAD);
		     resend_count++;
                   goto RE_RCV;
                  }
		 else
		   {
		    errorDebug(COMPLEMRNTCODEERROR);
	           return COMPLEMRNTCODEERROR;
		    }
	     	} 			
	     }
          else
	     {
             errorDebug(GETCHARFAIL);
	      return GETCHARFAIL;
           }
		  
      tmpbuffer=malloc(PACKET_SIZE);    /***malloc a peace of space as a tmp buffer***/
	  
     if (!xmodemGetPakt(tmpbuffer))     /*GET THE 4~131 BYTE*//*file data packet*/
     	{
     	 free(tmpbuffer);
        return GETCHARFAIL;
     	}
 
          v_code=0;/*getVerifyCode(tmpbuffer);*/
           if (xmodemGetc( timeout))      /*GET THE LAST BYTE*/
             {
               if(1)/*(*ptmpchar==v_code)     */       /*recieved a whole data packet*/
 	          { 
 	            memcpy(destaddr,tmpbuffer,PACKET_SIZE); 
		     free(tmpbuffer);
                   destaddr+=PACKET_SIZE;
                   packetNumber++;
 	            resend_count=0;
	            switch_State(state, WAIT_HEAD);
		     xmodemPutc(ACK);	
 	           }
               else 
 	           {
                     if(resend_count<RESENDTIME)
                       {
                         free(tmpbuffer);
			    switch_State(state, WAIT_HEAD);		
                         xmodemPutc(NAK); 
		           resend_count++;
                         goto RE_RCV;
                        }
		        else
		          {
		             errorDebug(VERIFYCODEERROR);
			      free(tmpbuffer);
	                    return VERIFYCODEERROR;
		           }
	     	       } 
              }	  
           else
	     {
             errorDebug(GETCHARFAIL);
	      free(tmpbuffer);
	      return GETCHARFAIL;
           }
      
 	}
	
    }

return (XDOWNSUCCESSFUL);
}



int Xdown(char *dest)
{
    unsigned char *p;
    int bufsz, crc = 0;
    volatile char tmpc;
    unsigned char trychar = 'C';
    unsigned char packetno = 1;
    int i, len = 0;
    int retry, retrans = MAXRETRANS;

    for(;;) {
        
        for( retry = 0; retry < 30; retry++) {  // approx 30 seconds allowed to make connection

            if (trychar) {
                
              /*  xmodemPutc(trychar);*/
	   while (serial_tstc() == 0)
            {
	 /*  *( volatile unsigned int*)0xbf001800=0x43;   */
	       xmodemPutc('C'); 
	       udelay(100000);
             }
            	}
            if ((xmodemGetc(TIMEOUT, &tmpc))) {
                                
                
                switch (tmpc) {
                case SOH:
			{
                     bufsz = 128;
		//	while(1)
             //       printf("SOH\n");
                	}
                  
                    goto start_recv;
                case STX:
			{
                    bufsz = 1024;
		//       while(1)
             //       printf("STX\n");
                	}
                    
                    goto start_recv;
                case EOT:           
                    /* recieve finish */     
                    	{
                    flushinput();
                    xmodemPutc(ACK);
           //        while(1)
           //         printf("EOT\n");
                    return len; /* normal end */
                    	}
                case CAN:
                    
                    if (xmodemGetc(TIMEOUT, &tmpc)>0)
                    {
             //       while(1)
              //      printf("CAN\n");
                        if (tmpc == CAN) {
                            flushinput();
                            xmodemPutc(ACK);
                            
                            return -1; /* canceled by remote */
                        }

                    }
                    break;
                default:

		//		   while(1)
            //        printf("0x%x\n",tmpc);
                    //return -4; /* invalid char received */
                    break;
                }
            }else {
        
            }
        }
        //printk("6 ");
        if (trychar == 'C') { 
            trychar = NAK; 
            continue;
        }
        flushinput();
        xmodemPutc(CAN);
        xmodemPutc(CAN);
        xmodemPutc(CAN);
        
        return -2; /* sync error */

    start_recv:
        
  while(zw_debug)
  	printf("get head!!!\n");
  
        if (trychar == 'C') 
            crc = 1;
        trychar = 0;
        p = xbuff;
        *p++ = tmpc;
        for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) {
            /* xwang : never <0, but maybe = 0 */
            if (xmodemGetc(TIMEOUT, &tmpc) == 0) 
            {
                //printk("module cant't get char, i = %d, quit.\n", i);
                goto reject;
            }
            *p++ = tmpc;
        }
        
 
        if (xbuff[1] == (unsigned char)(~xbuff[2]) && 
            (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1) &&
                check(crc, &xbuff[3], bufsz)) {
         /*   if (xbuff[1] == packetno)    {
                int count = destsz - len;
                if (count > bufsz) 
                    count = bufsz;
                if (count > 0) {
                    xmemcpy (&dest[len], &xbuff[3], count);
                    len += count;
                }*/
         	{
                ++packetno;
                retrans = /* MAXRETRANS*/ 25+1;
            	}
            if (--retrans <= 0) {
                
                flushinput();
                xmodemPutc(CAN);
                xmodemPutc(CAN);
                xmodemPutc(CAN);
              
                return -3; /* too many retry error */
            }
            
          
            xmodemPutc(ACK);
            continue;
        	}
    reject:
        flushinput();
        xmodemPutc(NAK);
    }
}

/*********************test perfect************************/
int Xdown(char *dest)
{
    unsigned char *p;
    int bufsz, crc = 0;
    volatile char tmpc;
    int zw,zwretry=0;
    volatile char checksend;
    unsigned char trychar = 'C';
    unsigned char packetno = 1;
    int i, len = 0;
    int retry, retrans = MAXRETRANS;



	
  #if 0 
   int in[100];
   unsigned int ii=0, j;  
#if 1
   while (serial_tstc() == 0)
   {
	 /*  *( volatile unsigned int*)0xbf001800=0x43;   */
	  xmodemPutc('C'); 
	   udelay(100000);
   }
 #endif
   for(j=0;j<10;j++)
   {
   	in[ii++]=serial_getc();
   }
   
   while(1)
   {
	   for(j=0;j<10;j++)
	   {
	   	printf("%d : %08x.\n", j, in[j]);
	   }   
	    udelay(100000);
   }
   
#endif   
#if  0
    for(;;) {
		  while (serial_tstc() == 0)
             {
	 /*  *( volatile unsigned int*)0xbf001800=0x43;   */
	        xmodemPutc('C'); 
	        udelay(100000);
             }
            if (xmodemGetc(TIMEOUT, &checksend)) {          
                
                switch (checksend) {
                case SOH:
			{
                     bufsz = 128;
		//	while(1)
             //       printf("SOH\n");
                	}
                  
                    goto start_recv;
                case STX:
			{
                    bufsz = 1024;
		//       while(1)
             //       printf("STX\n");
                	}
                    
                    goto start_recv;
                case EOT:           
                    /* recieve finish */     
                    	{
                    flushinput();
                    xmodemPutc(ACK);
           //        while(1)
           //         printf("EOT\n");
                    return len; /* normal end */
                    	}
                case CAN:
                    
                    if (xmodemGetc(TIMEOUT, &tmpc)>0)
                    {
             //       while(1)
              //      printf("CAN\n");
                        if (tmpc == CAN) {
                            flushinput();
                            xmodemPutc(ACK);
                            
                            return -1; /* canceled by remote */
                        }

                    }
                    break;
                default:
			{
			/*
					for(i=0;i<10;i++)
						{
                                            xmodemPutc('C'); 
						  udelay(1000);					
						}
						*/
	//			   while(1)
        //           printf("0x%x\n",tmpc);
                    //return -4; /* invalid char received */
                    if(zwretry<30)
                    	{zwretry++;
			goto RESEND;
                    	}else 
                    	return -1;
               //    break;
                	}
                }
            }else {
        
            
            	}
        //printk("6 ");
        RESEND:
        if (trychar == 'C') { 
    //        trychar = NAK; 
          xmodemPutc(NAK); 
	  udelay(100000);
            continue;
        }
#endif

   for(;;) {
        
        for( retry = 0; retry < 30; retry++) {  // approx 30 seconds allowed to make connection

            if (trychar) {
                
              /*  xmodemPutc(trychar);*/
	//   while (serial_tstc() == 0)
       //     {
	 /*  *( volatile unsigned int*)0xbf001800=0x43;   */
	       xmodemPutc('C'); 
	       udelay(10000);
         //    }
            	}
            if ((xmodemGetc(TIMEOUT, &tmpc))) {
                                
                
                switch (tmpc) {
                case SOH:
			{
                     bufsz = 128;
		//	while(1)
             //       printf("SOH\n");
                	}
                  
                    goto start_recv;
                case STX:
			{
                    bufsz = 1024;
		//       while(1)
             //       printf("STX\n");
                	}
                    
                    goto start_recv;
                case EOT:           
                    /* recieve finish */     
                    	{
                    flushinput();
                    xmodemPutc(ACK);
           //        while(1)
           //         printf("EOT\n");
                    return len; /* normal end */
                    	}
                case CAN:
                    
                    if (xmodemGetc(TIMEOUT, &tmpc)>0)
                    {
             //       while(1)
              //      printf("CAN\n");
                        if (tmpc == CAN) {
                            flushinput();
                            xmodemPutc(ACK);
                            
                            return -1; /* canceled by remote */
                        }

                    }
                    break;
                default:

		//		   while(1)
            //        printf("0x%x\n",tmpc);
                    //return -4; /* invalid char received */
                    break;
                }
            }else {
        
         }
        }
        //printk("6 ");
        if (trychar == 'C') { 
            trychar = NAK; 
            continue;
        }
        flushinput();
        xmodemPutc(CAN);
        xmodemPutc(CAN);
        xmodemPutc(CAN);
        
        return -2; /* sync error */

    start_recv:
        
  
       if (trychar == 'C') 
            crc = 1;
        trychar = 0;
        p = xbuff;
        *p++ = tmpc;
        for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) {
            /* xwang : never <0, but maybe = 0 */
            if (xmodemGetc(TIMEOUT, &tmpc) == 0) 
            {
                //printk("module cant't get char, i = %d, quit.\n", i);
                goto reject;
            }
            *p++ = tmpc;
        }
        while(zw_debug)
        {
        	for(zw=0;zw<(bufsz+(crc?1:0)+3);++zw)
			{
			printf("%d: 0x%x\n",zw,xbuff[zw]);

			udelay(100000);
        		}
        }
 
        if (xbuff[1] == (unsigned char)(~xbuff[2]) && 
            (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1) &&
                check(crc, &xbuff[3], bufsz)) {
         /*   if (xbuff[1] == packetno)    {
                int count = destsz - len;
                if (count > bufsz) 
                    count = bufsz;
                if (count > 0) {
                    xmemcpy (&dest[len], &xbuff[3], count);
                    len += count;
                }*/
         	{
                ++packetno;
                retrans = /* MAXRETRANS*/ 25+1;
            	}
            if (--retrans <= 0) {
                
                flushinput();
                xmodemPutc(CAN);
                xmodemPutc(CAN);
                xmodemPutc(CAN);
              
                return -3; /* too many retry error */
            }
            
          
            xmodemPutc(ACK);
            continue;
        	}
    reject:
        flushinput();
        xmodemPutc(NAK);
    }
}

#endif
#if 0
int Xdown(char *dest)
{
    unsigned char *p;
    int bufsz, crc = 0;
    volatile char tmpc;
    int zw,zwretry=0;
    volatile char checksend;
    unsigned char trychar = 'C';
    unsigned char packetno = 1;
    int i, len = 0;
    int retry, retrans = MAXRETRANS;


   for(;;) {
        
        for( retry = 0; retry < 30; retry++) {  // approx 30 seconds allowed to make connection

            if (trychar) {
        
	       xmodemPutc('C'); 
	       udelay(10000);
         
            	}
            if ((xmodemGetc(TIMEOUT, &tmpc))) {
                                
                
                switch (tmpc) {
                case SOH:
			{
                     bufsz = 128;

                	}
                  
                    goto start_recv;
                case STX:
			{
                    bufsz = 1024;
	
                	}
                    
                    goto start_recv;
                case EOT:           
                    /* recieve finish */     
                    	{
                    flushinput();
                    xmodemPutc(ACK);

                    return len; /* normal end */
                    	}
                case CAN:
                    
                    if (xmodemGetc(TIMEOUT, &tmpc)>0)
                    {
    
                        if (tmpc == CAN) {
                            flushinput();
                            xmodemPutc(ACK);
                            
                            return -1; /* canceled by remote */
                        }

                    }
                    break;
                default:


                    break;
                }
            }
	//		else { }
        }
      
        if (trychar == 'C') { 
            trychar = NAK; 
            continue;
        }
        flushinput();
        xmodemPutc(CAN);
        xmodemPutc(CAN);
        xmodemPutc(CAN);
        
        return -2; /* sync error */

    start_recv:
        
  
       if (trychar == 'C') 
            crc = 1;
        trychar = 0;
        p = xbuff;
        *p++ = tmpc;
        for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i) {
 
            if (xmodemGetc(TIMEOUT, &tmpc) == 0) 
            {

                goto reject;
            }
            *p++ = tmpc;
        }
        while(zw_debug)
        {
        	for(zw=0;zw<(bufsz+(crc?1:0)+3);++zw)
			{
			printf("%d: 0x%x\n",zw,xbuff[zw]);

			udelay(100000);
        		}
        }
 
        if (xbuff[1] == (unsigned char)(~xbuff[2]) && 
            (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1) &&
                check(crc, &xbuff[3], bufsz)) {
           if (xbuff[1] == packetno)    {

                  memcpy (dest, &xbuff[3], bufsz);
		   dest+=bufsz;
    
                }
         	{
                ++packetno;
                retrans = /* MAXRETRANS*/ 25+1;
            	}
            if (--retrans <= 0) {
                
                flushinput();
                xmodemPutc(CAN);
                xmodemPutc(CAN);
                xmodemPutc(CAN);
              
                return -3; /* too many retry error */
            }
            
          
            xmodemPutc(ACK);
            continue;
        	}
    reject:
        flushinput();
        xmodemPutc(NAK);
    }
}



/******************************************************************************
*
* getCntPersent - get current persent.
*
* input:  
*
* RETURNS:  .
*/

int getCntPersent()
{

}

/******************************************************************************
*
* errorRecord - record error to sdram.
*
* input:  
*
* RETURNS:  .
*/

void errorDebug(char *fmt, ...)
{
/* printf(const char *fmt, ...);*/
/* printf("errorno = %d\n",errorno);*/
}

#endif

