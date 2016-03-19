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
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xb9d3067e, "module_layout" },
	{ 0x562b9321, "usb_deregister" },
	{ 0xdeb87602, "usb_register_driver" },
	{ 0xec6c52c, "usb_deregister_dev" },
	{ 0x45f845f2, "_dev_info" },
	{ 0x22766ce1, "usb_register_dev" },
	{ 0x6a1ceaf2, "dev_set_drvdata" },
	{ 0xd2b09ce5, "__kmalloc" },
	{ 0xd8c3be51, "usb_get_dev" },
	{ 0xf432dd3d, "__init_waitqueue_head" },
	{ 0x3e18b1c5, "__mutex_init" },
	{ 0xa2e429a9, "kmem_cache_alloc_trace" },
	{ 0xd83ea029, "kmalloc_caches" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0x9f3dd05, "usb_find_interface" },
	{ 0x5ae4fa58, "mutex_lock" },
	{ 0x9297e8ae, "mutex_unlock" },
	{ 0x466649c3, "dev_get_drvdata" },
	{ 0x37a0cba, "kfree" },
	{ 0x2deed90e, "usb_put_dev" },
	{ 0x4f6b400b, "_copy_from_user" },
	{ 0x27e1a049, "printk" },
	{ 0xcbdb9f8, "usb_bulk_msg" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x4f8b5ddb, "_copy_to_user" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xc8fd129f, "usb_control_msg" },
	{ 0x8b68ce9f, "usb_kill_anchored_urbs" },
	{ 0x673a1863, "usb_wait_anchor_empty_timeout" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("usb:v0451p5535d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "D90BB73F89DD14F3ACA5315");
