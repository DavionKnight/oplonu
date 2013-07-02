#ifndef __TEST_H__
#define ___TEST_H__

//typedef unsigned int u32;

//u32 reg_read(char *reg_base,int offset);
//void reg_write(char *reg_base, int offset, u32 value);
//void opl_reg_main(int argc,char ** argv);
//int opl_reg_init(int *opl_reg_initialized);
//int opl_reg_exit(int *opl_reg_initialized);

int cpldcs2Write(int add, unsigned char setData);
int cpldcs2Read(int add, unsigned char *pucValue);

void opl_uart_main();
int opl_uart_init(int *opl_reg_initialized);
int opl_uart_exit(int *opl_reg_initialized);

#endif

