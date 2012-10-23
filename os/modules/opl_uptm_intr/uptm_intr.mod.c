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
	{ 0xcc5005fe, "msleep_interruptible" },
	{ 0x1d26aa98, "sprintf" },
	{ 0x2bc95bd4, "memset" },
	{ 0xe1eb5d41, "filp_close" },
	{ 0x97255bdf, "strlen" },
	{ 0xdd132261, "printk" },
	{ 0xf8da28a5, "filp_open" },
	{ 0xecb744e2, "wake_up_process" },
	{ 0x233670b0, "kthread_create" },
	{ 0x2cf190e3, "request_irq" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "47BF2B8AEDF507FFFD183BC");
