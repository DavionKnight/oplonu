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
	{ 0xfc3c0e79, "clear_irq_pending" },
	{ 0xbbf43597, "get_irq_pending" },
	{ 0x51493d94, "finish_wait" },
	{ 0x1000e51, "schedule" },
	{ 0x8085c7b1, "prepare_to_wait" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x2bc95bd4, "memset" },
	{ 0xb6c70a7d, "__wake_up" },
	{ 0x6cb34e5, "init_waitqueue_head" },
	{ 0xdd132261, "printk" },
	{ 0xe0bdf36a, "register_chrdev" },
	{ 0x2cf190e3, "request_irq" },
	{ 0xc192d491, "unregister_chrdev" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x140f37af, "ipmux_irq_disable" },
	{ 0xb1db9f77, "ipmux_irq_enable" },
	{ 0xb6091ec0, "__copy_user" },
	{ 0xcd360d1a, "cpu_data" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "A105CF3F19728B9271F8BAB");
