#ifndef __OPL_REGMMAP_H__
#define __OPL_REGMMAP_H__

typedef unsigned int u32;


u32 reg_read(char *reg_base,int offset);
void reg_write(char *reg_base, int offset, u32 value);
void opl_reg_main(int argc,char ** argv);
int opl_reg_init(int *opl_reg_initialized);
int opl_reg_exit(int *opl_reg_initialized);

int cpldWrite(int add, unsigned char setData);
int cpldRead(int add, unsigned char *pucValue);

void opl_uart_main();
int opl_uart_init(int *opl_reg_initialized);
int opl_uart_exit(int *opl_reg_initialized);
int mmap_main();

#endif

