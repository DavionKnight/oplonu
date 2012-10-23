#include <common.h>

#ifdef INCLUDE_PERIPHA_TEST
#include "opl_driver.h"
#include "ipmux_rtc.h"
#include "ipmux_int.h"
void test_ipmux_rtc_AInt()
{

}

void ipmuxRtcClockSet()
{
    *((UINT32 *)0xbf0029c0)=0x17D7840;

}

void test_ipmux_rtc_IncInt()
{

    printf("--------------------------------------------------\n");
    printf("        RTC test Int ...\n");
    printf("--------------------------------------------------\n");
    
    ipmuxRtcReset();
    ipmuxRtcTimePrint();
    ipmuxRtcEnable(OPL_TRUE);
    /*int every minutue*/
    ipmuxRtcIncIntEnable(IPMUX_RTC_OFFSET_IMMIN);
    
}

void test_ipmux_rtc_time()
{
    printf("--------------------------------------------------\n");
    printf("        RTC test time ...\n");
    printf("--------------------------------------------------\n");
    ipmuxRtcReset();
    
    ipmuxRtcTimePrint();
    printf("Now enalbe rtc ...\n");
    ipmuxRtcEnable(OPL_TRUE);
    printf("    udelay  100s ...\n");
    udelay(100000000);
    ipmuxRtcTimePrint();
}

void test_ipmux_rtc_Reset()
{
    printf("--------------------------------------------------\n");
    printf("        RTC test reset ...\n");
    printf("--------------------------------------------------\n");

    ipmuxRtcTimePrint();
    printf("    counter =0x%x \n",ipmuxRtcCounterGet ());
        
    printf("Now enable rtc and wait for 15s...\n");
    ipmuxRtcEnable(OPL_TRUE);
    udelay(15000000);
    ipmuxRtcTimePrint();
    printf("    counter =0x%x \n",ipmuxRtcCounterGet ());   
    printf("Now reset rtc ...\n");
    ipmuxRtcReset();
    ipmuxRtcTimePrint();    
    printf("    counter =0x%x \n",ipmuxRtcCounterGet ());
}


void test_ipmux_rtc_basic()
{
    struct rtc_time tmp;    
    printf("--------------------------------------------------\n");
    printf("        IPMUX RTC basic test ...\n");
    printf("--------------------------------------------------\n");

    ipmuxRtcTimePrint();
    ipmuxRtcATimePrint();

    tmp.tm_year=2006;
    tmp.tm_mon=2;
    tmp.tm_wday=3;
    tmp.tm_mday=23;
    tmp.tm_yday=1;
    tmp.tm_isdst=0;
    tmp.tm_hour=10;
    tmp.tm_min=11;
    tmp.tm_sec=12;
    printf("Now set time to:\n");
    printTime(&tmp);
    ipmuxRtcSet(&tmp);

    tmp.tm_hour+=1;
    printf("Now set alarm time to:\n"); 
    printTime(&tmp);
    ipmuxRtcAlarmSet(&tmp);
        
    ipmuxRtcTimePrint();
    ipmuxRtcATimePrint();

    printf(" ----------------------DONE----------------------\n\n");    

}

void init_ipmux_rtc_int_ctrl()
{
    volatile INT_CTRL *p_int=(volatile INT_CTRL *)INT_CTRL_ADDR;
    printf("Now enable int ctrl ...\n");
    /* enable all interrupt */
    p_int->ENABLE1=0x01000000; /* rtc */

    /* IPMux Interrupt generator enable */
    p_int->CFG=0x1;//   0x2A18

    //printf(" p_int->ENABLE1=%x\n",p_int->ENABLE1);
}

void printTime( struct rtc_time *tmp)
{
    printf("    DATE: %04d-%02d-%02d (wday=%d,yday=%d)  TIME: %2d:%02d:%02d\n",\
        tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,tmp->tm_yday,\
        tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
}

void timeInc(struct rtc_time *tmp)
{
    tmp->tm_sec+=1;
    if(tmp->tm_sec>=60) 
    {
        tmp->tm_sec=0;
        tmp->tm_min+=1;
    }   
            
    tmp->tm_min+=1;
    if(tmp->tm_min>=60) 
    {
        tmp->tm_min=0;
        tmp->tm_hour+=1;
    }   

    tmp->tm_hour+=1;
    if(tmp->tm_hour>=24) 
    {
        tmp->tm_hour=0;
        tmp->tm_mday+=1;
    }   
    
    tmp->tm_mday+=1;
    if(tmp->tm_mday>=32) 
    {
        tmp->tm_mday=1;
        tmp->tm_mon+=1;
    }   
    
    tmp->tm_yday+=1;
    if(tmp->tm_yday>=357) 
    {
        tmp->tm_yday=1;
    }   

    tmp->tm_wday+=1;
    if(tmp->tm_wday>=7) 
    {
        tmp->tm_wday=0;
    }   

    tmp->tm_mon+=1; 
    if(tmp->tm_mon>=13) 
    {
        tmp->tm_mon=1;
        tmp->tm_year+=1;    
    }   
    
    tmp->tm_year+=1;    
}

int test_ipmux_rtc_inc_int ()
{

    struct rtc_time tmp;
    int iNo=0;
    record_time_def;

    tmp.tm_sec=0;
    tmp.tm_hour=0;

    tmp.tm_mday=1;
    tmp.tm_yday=1;
    tmp.tm_wday=0;

    tmp.tm_year=0;  
    tmp.tm_mon=1;   
    ipmuxRtcSet(tmp);
    
    printf("--------------------------------------------------\n");
    printf("        RTC test inc_int ...\n");
    printf("--------------------------------------------------\n");
    ipmuxRtcIncIntEnable(IPMUX_RTC_OFFSET_IYEAR_MASK | IPMUX_RTC_OFFSET_IMON_MASK  |\
        IPMUX_RTC_OFFSET_IDOY_MASK  | IPMUX_RTC_OFFSET_IDOM_MASK  | \
        IPMUX_RTC_OFFSET_IDOW_MASK  | IPMUX_RTC_OFFSET_IHOUR_MASK | \
        IPMUX_RTC_OFFSET_IMIN_MASK  | IPMUX_RTC_OFFSET_ISEC_MASK );
    
    /*disable alarm int*/
    ipmuxRtcAIntDisable(IPMUX_RTC_OFFSET_CMYEAR_MASK | IPMUX_RTC_OFFSET_CMMON_MASK   | \
        IPMUX_RTC_OFFSET_CMDOY_MASK   | IPMUX_RTC_OFFSET_CMDOM_MASK   | \
        IPMUX_RTC_OFFSET_CMDOW_MASK   | IPMUX_RTC_OFFSET_CMHOUR_MASK  | \
        IPMUX_RTC_OFFSET_CMMIN_MASK   | IPMUX_RTC_OFFSET_CMSEC_MASK );

    ipmuxRtcGet(&tmp);
    timeInc(&tmp);
    timeInc(&tmp);
    timeInc(&tmp);
    tmp.tm_sec+=40;
    ipmuxRtcAlarmSet(&tmp);
    
    record_time;
    
    printf("Clear all int\n");
    //ipmuxRtcIntHandle();  
    
    /*enable int */
    printf("enable mips int and int control \n");   
    
    init_ipmux_rtc_int_ctrl();              //enable int
    mips_cp0_set(CP0_ID_STATUS,0x40fb05);   //ie = bit0     
    
    print_cp0reg_int(); 
    ipmuxRtcIntClear();
    print_cp0reg_int();

    ipmuxRtcReset(); //reset to the second start.
    printf("int enalbe =%08x\n",rtcRegRead32BitMask((UINT32)IPMUX_RTC_OFFSET_IMYEAR,0x00FFFFFF));
    printf("int should appear every 10*1s,10*60*1s,10*60*60*1s...\n");
    record_time;
    while(1)
    {
        if(get_ipmux_hw0_int() )
        {       
            
                if (IPMUX_RTC_OFFSET_IMIN_MASK & ipmuxRtcIntHandle())
                {
                    print_record_time;
                    record_time;
                }
                
                printf("INT %03d occured, Please check the time passed\n",iNo);
                ipmuxRtcTimePrint();    
                print_cp0reg_int(); 
            
                printf("    Clear the int\n");
                
                //clear the int: RC
                print_cp0reg_int();             
                ipmuxRtcIntHandle();
                switch (iNo)
                {
                    case 5:
                        printf("configure to generate IMIN int...\n");

                        ipmuxRtcGet(&tmp);
                        tmp.tm_sec=58;
                        printTime(&tmp);                        
                        ipmuxRtcSet(&tmp);                      
                        
                        break;
                    case 10:
                        printf("configure to generate IHour int...\n");

                        ipmuxRtcGet(&tmp);
                        tmp.tm_sec=58;
                        tmp.tm_min=59;          
                        printTime(&tmp);                        
                        ipmuxRtcSet(&tmp);      
                        break;
                    case 15:
                        printf("configure to generate IMDAY int...\n");
                        ipmuxRtcGet(&tmp);
                        tmp.tm_sec=58;
                        tmp.tm_min=59;                      
                        tmp.tm_hour=23;                                             
                        printTime(&tmp);                        
                        ipmuxRtcSet(&tmp);      
                        break;
                }
                iNo++;
                //goto OUT;
        }
    }

OUT:                
    printf(" ----------------------DONE----------------------\n\n");

}
    
int test_ipmux_rtc_alarm_int (int testcase)
{

    struct rtc_time tmp;

    record_time_def;

    tmp.tm_sec=0;
    tmp.tm_hour=0;

    tmp.tm_mday=1;
    tmp.tm_yday=1;
    tmp.tm_wday=0;

    tmp.tm_year=0;  
    tmp.tm_mon=1;   
    ipmuxRtcSet(&tmp);
    
    printf("--------------------------------------------------\n");
    printf("        RTC test alarm_int %d...\n",testcase);
    printf("--------------------------------------------------\n");
    ipmuxRtcIncIntDisable(IPMUX_RTC_OFFSET_IYEAR_MASK | IPMUX_RTC_OFFSET_IMON_MASK  |\
        IPMUX_RTC_OFFSET_IDOY_MASK  | IPMUX_RTC_OFFSET_IDOM_MASK  | \
        IPMUX_RTC_OFFSET_IDOW_MASK  | IPMUX_RTC_OFFSET_IHOUR_MASK | \
        IPMUX_RTC_OFFSET_IMIN_MASK  | IPMUX_RTC_OFFSET_ISEC_MASK );

    ipmuxRtcAIntDisable(IPMUX_RTC_OFFSET_CMYEAR_MASK | IPMUX_RTC_OFFSET_CMMON_MASK   | \
        IPMUX_RTC_OFFSET_CMDOY_MASK   | IPMUX_RTC_OFFSET_CMDOM_MASK   | \
        IPMUX_RTC_OFFSET_CMDOW_MASK   | IPMUX_RTC_OFFSET_CMHOUR_MASK  | \
        IPMUX_RTC_OFFSET_CMMIN_MASK   | IPMUX_RTC_OFFSET_CMSEC_MASK );
    
    printf("Init int indicators\n");
    ipmuxRtcIntHandle();    
    /*enable int */
    printf("enable mips int and int control \n");   
    
    init_ipmux_rtc_int_ctrl();              //enable int
    mips_cp0_set(CP0_ID_STATUS,0x40fb05);   //ie = bit0     
    
    print_cp0reg_int(); 
    
    ipmuxRtcIntClear();
    print_cp0reg_int();

    ipmuxRtcReset(); //reset to the second start.
    /*set alarm time*/
    ipmuxRtcGet(&tmp);
    record_time;

    /*to check sec int unable*/
    timeInc(&tmp);
    
    
    switch (testcase){
    case 0:
        ipmuxRtcAIntEnable(IPMUX_RTC_OFFSET_CMSEC_MASK);
        printf("wait for SECOND int (should occur at 5*10s later) ...\n");  
        break;
    case 1:
        ipmuxRtcAIntEnable(IPMUX_RTC_OFFSET_CMMIN_MASK);
        printf("wait for MINUTUE int (should occur at 60*10s later) ...\n");    
        break;      
    case 2:
        ipmuxRtcAIntEnable(IPMUX_RTC_OFFSET_CMHOUR_MASK);   
        break;      
    case 3:
        ipmuxRtcAIntEnable(IPMUX_RTC_OFFSET_CMDOM_MASK);    
        break;      
    case 4:
        ipmuxRtcAIntEnable(IPMUX_RTC_OFFSET_CMMON_MASK);    
        break;      
    case 5:
        ipmuxRtcAIntEnable(IPMUX_RTC_OFFSET_CMYEAR_MASK);   
        break;      
    case 6:
        ipmuxRtcAIntEnable(IPMUX_RTC_OFFSET_CMDOW_MASK);    
        break;      
    case 7:
        ipmuxRtcAIntEnable(IPMUX_RTC_OFFSET_CMDOY_MASK);    
        break;      
    default:
        printf("INVALID PARAM...\n");           
        return;
        break;
    }

    printf("int enalbe =%08x\n",rtcRegRead32BitMask((UINT32)IPMUX_RTC_OFFSET_IMYEAR,0x00FFFFFF));

    /*after 10s the int will occured*/
    ipmuxRtcAlarmSet(&tmp);
    
    ipmuxRtcTimePrint();    
    ipmuxRtcATimePrint();   
    
    ipmuxRtcIntClear(); 
    print_cp0reg_int(); 

    printTime(&tmp);

    while(1)
    {
        //udelay(5000);
        if(period(5000))
        {
            if (0!=testcase)
            {
                ipmuxRtcSet(&tmp);
                ipmuxRtcTimePrint();
                ipmuxRtcATimePrint();
            }
        }

        if(get_ipmux_hw0_int() )
        {   
                print_record_time;
                printf("INT occured, Please check the time passed\n");
                print_cp0reg_int(); 
                ipmuxRtcIntHandle();
            
                ipmuxRtcTimePrint();
                ipmuxRtcATimePrint();

                //udelay(10000000);
                printf("Now clear the int ...\n");
                //timeInc(&tmp);
                /*To see if other int occured : just change current to disable all ints*/
                switch (testcase){
                    case 0:
                        tmp.tm_sec-=1;
                        break;
                    case 1:
                        tmp.tm_min-=1;
                        break;      
                    case 2:
                        tmp.tm_hour-=1;
                        break;      
                    case 3:
                        tmp.tm_mday-=1;
                        break;      
                    case 4:
                        tmp.tm_mon-=1;
                        break;      
                    case 5:
                        tmp.tm_year-=1;
                        break;      
                    case 6:
                        tmp.tm_wday-=1;
                        break;      
                    case 7:
                        tmp.tm_yday-=1;
                        break;      
                    default:
                        printf("INVALID PARAM...\n");           
                        return;
                        break;
                    }

                
                ipmuxRtcSet(&tmp);
                ipmuxRtcIntClear();     
                ipmuxRtcTimePrint();
                //clear the int: RC
                print_cp0reg_int();             
                if(get_ipmux_hw0_int() )                
                {
                    printf("Error : int should not occured!\n");
                }
                else
                {
                    printf("PASSED.\n");

                }
                ipmuxRtcIntClear();

                print_cp0reg_int();
                ipmuxRtcIntHandle();

                goto OUT;
        }
            
    }

OUT:                
    printf(" ----------------------DONE----------------------\n\n");

}


void test_ipmux_rtc()
{
    int i;
    record_time_def;    
    printf("--------------------------------------------------\n");
    printf("        RTC test ...\n");
    printf("--------------------------------------------------\n");
    //test_ipmux_rtc_basic();

    //record_time;
    //udelay(10000000);
    //print_record_time;
    ipmuxRtcClockSet();
    test_ipmux_rtc_inc_int();
    return;
    //test_ipmux_rtc_alarm_int (0); /*test second*/
    for(i=1;i<=7;i++)
    {
        test_ipmux_rtc_alarm_int (i);   /*test minutue*/
    }
    //test_ipmux_rtc_inc_int();

    //test_ipmux_rtc_Reset();
    
    //test_ipmux_rtc_time();    
}

#endif
