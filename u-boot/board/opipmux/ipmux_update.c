/***************************************************************************
*Ipmux_update.c
*
*                   this file used for updating program with urat .
*
*****************************************************************************/

#include "ipmux_xdown.c"
#include <common.h>
#include <command.h>
#ifdef  OPULAN_IMAGE_HEADER
#include <image.h>

#define TYPE_FIRMWARE	 5	/* Firmware Image		*/
#define TYPE_KERNEL		 2	/* Kernel Image			*/
#define TYPE_FILESYSTEM	 7	/* Filesystem Image (any type)	*/
UINT32 image_type=0;
#define set_image_type(x)  image_type=x
#endif
#define UPUBOOT   0
#define UPENV        1
#define UPKER_1    2
#define UPCFG        3
#define UPKER_2     4

#define tmpaddr     0xa1000000
#if 0
typedef struct file_header{

     unsigned short     chipID;
     unsigned char      type;
     unsigned char      flags;
     unsigned int         revision;
     unsigned int         crc_word;
     unsigned int         imagesize;
     unsigned int         build_time;
     } IMAGE_HEADER;
#endif
extern  UINT32  CS0_REDEFINED;
extern  int imagePreCheck(unsigned int fileaddr);
extern  int bootUpdateSimpleCheck(unsigned int fileaddr);
extern int update(int  choice);

int update(int  choice)
{
    int fsize;
    int rcode;
    char cmd[50]="";
    volatile unsigned int tmpbase;
    volatile unsigned int cs0base;
    
//   reCfgCs0(FLASHSIZE);
//   ipMuxRegRead(REG_CS0_BASE_ADDR, &tmpbase);
//   cs0base=(tmpbase<<12)+0xa0000000;
   cs0base=PHYS_FLASH_1; /*zw change*/

   printf("flash base addr:0x%x\n",cs0base);
    
   if((choice<0)||(choice>4))
    {
    printf("error: invalid choice %d",choice);
    return -1;
    }
   
   fsize=Xdown(tmpaddr);
//   printf("fsize=0x%x\n",fsize);
   
   if(fsize<=0)
    {
    printf("update falied: load the binary file unsuccess...\n");
    return -1;
    }
   
  //  else  
  //    return 0 ;
#if  0  
    if(0!=imagePreCheck(tmpaddr))
	 	return -1; 
 #endif  
   
   switch (choice){
    case UPUBOOT:
        {
			
              if(0!=bootUpdateSimpleCheck(tmpaddr))
		 return -1;
              sprintf(cmd, "protect off 0x%x 0x%x", cs0base+uboffset, cs0base+uboffset + 0x3ffff); /*(fsize | 0xFFFF)*/
             printf("------ protec off  flash [%s]\t------\n", cmd);
             rcode=run_command(cmd,0);
             if (1!=rcode) return -1;           
	      sprintf(cmd, "era 0x%x 0x%x", cs0base+uboffset, cs0base+uboffset +  0x3ffff/*(size | 0xFFFF)*/);
             printf("###### Erase flash [%s]\t######", cmd);
      //   printf("start:0x%x\ndest:0x%x\n",cs0base+uboffset,cs0base+uboffset + (fsize | 0xFFFF));
             rcode=run_command(cmd,0);
	         printf("rcode = 0x%x\n",rcode);
      //       if (1!=rcode) return -1;
#if 0 //kxu
{
    unsigned addr1,addr2,usize;
    addr1=0xbfc40000;
    addr2=0xbfc00000;
    usize=fsize;
	      sprintf(cmd, "cmp 0x%x 0x%x 0x%x",addr2,addr1,usize);
             rcode=run_command(cmd,0);
}
#endif
         sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", tmpaddr, cs0base+uboffset, fsize);
            printf("------ Program flash [%s]\t------\n", cmd);
            rcode=run_command(cmd,0);
            if (1!=rcode) return -1;
#if 0 //kxu
{
    unsigned addr1,addr2,usize;
    addr1=0xbfc00000;
    addr2=tmpaddr;
    usize=fsize;
	      sprintf(cmd, "cmp 0x%x 0x%x 0x%x",addr2,addr1,usize);
             rcode=run_command(cmd,0);
}
#endif
            
         break;
            }

    case UPKER_1:
        {
    #if     0
           cs0base=PHYS_FLASH_2;  /*IPMUX_E2_C : OS not store in FLASH1,but FLASH2*/
	#endif
#ifdef  OPULAN_IMAGE_HEADER
    if(getOplHeadSwitchStatus())
    {
             if(0!=imagePreCheck(tmpaddr))
	 	 return -1; 
             if (image_type!=TYPE_KERNEL){
		 printf("ERROR:NOT KERNEL IMAGE\n");
		 return -1;
             	}
    }
#endif
        sprintf(cmd, "era 0x%x 0x%x", cs0base+knl_1offset, cs0base+knl_1offset + (fsize | 0xFFFF));
             printf("------ Erase flash [%s]\t------", cmd);
             rcode=run_command(cmd,0);
     //        if (1!=rcode) return -1;

         sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", tmpaddr, cs0base+knl_1offset, fsize);
            printf("------ Program flash [%s]\t------\n", cmd);
            rcode=run_command(cmd,0);
        if (1!=rcode) return -1;

        break;
        }
    case UPENV:
        {
            sprintf(cmd, "protect off 0x%x 0x%x", cs0base+envoffset, cs0base+envoffset + (fsize | 0xFFFF));
             printf("------ protec off  flash [%s]\t------\n", cmd);
             rcode=run_command(cmd,0);
             if (1!=rcode) return -1;
            
           sprintf(cmd, "era 0x%x 0x%x", cs0base+envoffset, cs0base+envoffset + (fsize | 0xFFFF));
             printf("------ Erase flash [%s]\t------", cmd);
             rcode=run_command(cmd,0);
  //           if (1!=rcode) return -1;

         sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", tmpaddr, cs0base+envoffset, fsize);
            printf("------ Program flash [%s]\t------\n", cmd);
            rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
        
        break;
        }
    
//#if  (FLASHSIZE>=flash_8M)
	case UPCFG:
        {
#ifdef  OPULAN_IMAGE_HEADER
    if(getOplHeadSwitchStatus())
    {
             if(0!=imagePreCheck(tmpaddr))
	 	 return -1; 
             if (image_type!=TYPE_FILESYSTEM){
		 printf("ERROR:NOT FOLESYSTEM IMAGE\n");
		 return -1;
             	}
    }
#endif
		sprintf(cmd, "era 0x%x 0x%x", cs0base+cfg_offset, cs0base+cfg_offset + (fsize | 0xFFFF));
		printf("0x%x,0x%c",cs0base+cfg_offset,cs0base+cfg_offset + (fsize | 0xFFFF));
             printf("------ Erase flash [%s]\t------", cmd);
             rcode=run_command(cmd,0);
  //           if (1!=rcode) return -1;

	     sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", tmpaddr, cs0base+cfg_offset, fsize);
            printf("------ Program flash [%s]\t------\n", cmd);
            rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
        
        break;
        }

    case UPKER_2:
        {
#ifdef  OPULAN_IMAGE_HEADER
    if(getOplHeadSwitchStatus())
    {
             if(0!=imagePreCheck(tmpaddr))
	 	 return -1; 
             if (image_type!=TYPE_KERNEL){
		 printf("ERROR:NOT KERNEL IMAGE\n");
		 return -1;
             	}
    }
#endif
        sprintf(cmd, "era 0x%x 0x%x", cs0base+knl_2offset, cs0base+knl_2offset + (fsize | 0xFFFF));
             printf("------ Erase flash [%s]\t------", cmd);
             rcode=run_command(cmd,0);
  //           if (1!=rcode) return -1;

         sprintf(cmd, "cp.b 0x%x 0x%x 0x%x", tmpaddr, cs0base+knl_2offset, fsize);
            printf("------ Program flash [%s]\t------\n", cmd);
            rcode=run_command(cmd,0);
        if (1!=rcode) return -1;
        
        break;
        }
//#endif    
       default:
           {
        printf("warning:unsupport!!!\n") ;      
            return -1;
            }
    }
  printf("------update successfull------\n") ;
   return 0;
   
}

extern void reCfgCs0( int flasize)
{
int cs0base,cs0size;
//if(flasize==flash_4M)
//  cs0base=0xbfc00000;
//else
    cs0base=PHYS_FLASH_1;
//cs0size=flasize;
switch(flasize){
	case flash_2M :
    case flash_4M :
        cs0size=0xfc00;
        break;
    case flash_8M:
        cs0size=0xf800;
        break;
    case flash_16M:
        cs0size=0xf000;
        break;
       case flash_32M:
        cs0size=0xe000;
        break;
    case flash_64M:
        cs0size=0xc000;
        break;
    case flash_128M:
        cs0size=0x8000;
        break;
    default:
		printf("error:undefined size\n");
        break;
}
ipMuxRegWrite(REG_CS0_BASE_ADDR, (cs0base&0x1FFFFFFF)>>12);
ipMuxRegWrite(REG_CS0_SIZE, cs0size);
changePtr1();
//  printf("cs0base:0x%x\ncs0size:0x%x\n",*(unsigned int*)0xbf003020,*(unsigned int*)0xbf003048);
}

extern void cs0Cfgdefault()
{
ipMuxRegWrite(REG_CS0_BASE_ADDR, (0xbfc00000&0x1FFFFFFF)>>12);
ipMuxRegWrite(REG_CS0_SIZE, 0xfc00);
changePtrback();
}
extern  int bootUpdateSimpleCheck(unsigned int fileaddr)
{ 
      if (0x100000ff!=*(unsigned int*)fileaddr){
	 printf("*******************************************************\n");
	 printf("                    chu              cuo                  le\n\n");
	 printf("                            NOT BOOT IMAGE\n\n");
	 printf("                    chu              cuo                  le\n");
	 printf("*******************************************************\n");
	 return -1;
       }
	 return 0;
}
#ifdef  OPULAN_IMAGE_HEADER
extern  int imagePreCheck(unsigned int fileaddr)
{ 
     image_header_t  * header=(image_header_t*)fileaddr;	
     if(getOplHeadSwitchStatus())
    {
		/* for union version, we only check product id */
        if ((SWAP(header->ih_chipid)&0xFFFF0)!=
			((*(unsigned int*)(0xbf000000))&0xFFFF0)){
            printf(" chip ID error;  ");
            printf("chipid=%x\n",SWAP(header->ih_chipid));
            return -1;
        }
        set_image_type(header->ih_type);
    }
     return 0;
 }
#endif
int do_update(cmd_tbl_t *cmdtp,int flag,int argc, char *argv[])
{

    unsigned int  option;   
    int rcode = 0;
    int status;

    if (argc == 2) {
        option = simple_strtoul(argv[1], NULL, 16);
    }

    puts ("## Ready for update ...\n");  


    status = update (option);
    
    if(status==0)
     {
   /*    printf("##debug:after Xdown, Start addr=%x\n",addr); */
       rcode=0;
     }
    else
      rcode=-1;
    return rcode;

}


U_BOOT_CMD(
  update,    2,    0,     do_update,
 "update      - update binary file over serial line (xmodem)\n",
 "update(choice),option appoint the choice you want to update\n"
);
