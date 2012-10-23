#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x4ff5453b, "struct_module" },
	{ 0x51493d94, "finish_wait" },
	{ 0x1000e51, "schedule" },
	{ 0x8085c7b1, "prepare_to_wait" },
	{ 0x57625764, "opl_host_dma0_waitq" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x7f4e08ba, "get_ipmux_dma0_rx_buf_data" },
	{ 0x9bd4684, "set_ipmux_dma0_tx_buf_data" },
	{ 0xb6091ec0, "__copy_user" },
	{ 0x62301634, "opl_host_dma0_irq_event" },
	{ 0x140f37af, "ipmux_irq_disable" },
	{ 0xb1db9f77, "ipmux_irq_enable" },
	{ 0xdd132261, "printk" },
	{ 0x93fca811, "__get_free_pages" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0x4302d0eb, "free_pages" },
	{ 0x2bc95bd4, "memset" },
	{ 0xe0bdf36a, "register_chrdev" },
	{ 0xcd360d1a, "cpu_data" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "7F814CD891AC20779A5D517");
