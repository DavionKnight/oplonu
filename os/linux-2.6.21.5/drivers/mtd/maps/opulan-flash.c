/********************************************************************************
 *
 * Filename: 
 *
 * Summary:
 *  Nor flash driver  for Onu Soc
 * 
 * Modification History:
 *  Date	  By	   Summary
 *  -------- -------- ----------------------------------------------------------
 *  2009-12-15      Figo.zhang	 Original Creation
 *
 ********************************************************************************/

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>

#include <asm/flash.h>

#ifdef CONFIG_ARCH_ONU2
#include <asm/mach-onu2/onu2.h>
#include <asm/mach-onu2/onu2_reg.h>
#include <asm/mach-onu2/onu2_irq.h>
#endif

struct opulan_flash_info {
	struct mtd_partition	*parts;
	int			nr_parts;
	struct mtd_info		*mtd;
	struct map_info		map;
};

#define FLASH_2M    0x200000
#define FLASH_4M    0x400000
#define FLASH_8M    0x800000
#define FLASH_16M   0x1000000
#define FLASH_32M   0x2000000
#define FLASH_64M   0x4000000
#define FLASH_128M 0x8000000

unsigned long get_flash_cs0_size(unsigned long size)
{
	unsigned long mask_size = 0;
	switch(size){
			case FLASH_2M:
			case FLASH_4M:
				mask_size = 0xfc00;
				break;
			case FLASH_8M:
				mask_size = 0xf800;
				break;
			case FLASH_16M:
				mask_size = 0xf000;
				break;
			case FLASH_32M:
				mask_size=0xe000;
				break;
			case FLASH_64M:
				mask_size=0xc000;
				break;
			case FLASH_128M:
				mask_size = 0x8000;
				break;
			default:
				printk("%s: unknow flash size\n", __FUNCTION__);
				break;
	}
	return mask_size;
}

static int __init opulan_flash_probe(struct platform_device *pdev)
{
	struct flash_platform_data *flash = pdev->dev.platform_data;
	struct opulan_flash_info *info;
	struct resource *res;


	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENODEV;

	info = kmalloc(sizeof(struct opulan_flash_info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	memset(info, 0, sizeof(struct opulan_flash_info));

	info->map.name = (char *) flash->name;
	info->map.bankwidth = flash->width;
	info->map.phys = res->start;
	info->map.size = res->end - res->start + 1;
	info->parts = flash->parts;
	info->nr_parts = flash->nr_parts;

	info->map.virt = info->map.phys;

//		printk("flash->map_name=%s,info->map.virt = 0x%x, map.size=0x%x, info->nr_parts=%d\n",flash->map_name, info->map.virt,
			//info->map.size, info->nr_parts );

#ifdef CONFIG_ARCH_ONU
#if 0
	*(volatile unsigned int *)0xbf003020 = (info->map.phys & 0x1FFFFFFF)>>12;
	/*32M*/
	*(volatile unsigned int *)0xbf003048 = (0xE000);
#endif
#endif

#ifdef CONFIG_ARCH_OPTRANN
	*(volatile unsigned int *)0xbf000100 = (info->map.phys & 0x1FFFFFFF)>>12;
	/*32M*/
	*(volatile unsigned int *)0xbf000104 = (0xE000);
#endif

#ifdef CONFIG_ARCH_ONU2
	onu_writel(HOST_BASE_ADDR+CS0_BASE_ADDR_REG, (info->map.phys & 0x1FFFFFFF)>>12);
	onu_writel(HOST_BASE_ADDR+CS0_ADDR_MASK_REG, get_flash_cs0_size(info->map.size));
#endif

	simple_map_init(&info->map);

	printk(KERN_NOTICE
			"Probing %s at physical address 0x%08lx (%d-bit bankwidth)\n",
			info->map.name, (unsigned long)info->map.phys,
			info->map.bankwidth * 8);

	info->mtd = do_map_probe(flash->map_name, &info->map);

	if (!info->mtd) {
		return -EIO;
	}
	info->mtd->owner = THIS_MODULE;

	if (info->nr_parts) {
		add_mtd_partitions(info->mtd, info->parts,
				info->nr_parts);
	} else {
		printk("Registering %s as whole device\n",
				info->map.name);
		add_mtd_device(info->mtd);
	}

	platform_set_drvdata(pdev, info);	

	return 0;
}

static int __devexit opulan_flash_remove(struct platform_device *dev)
{

	struct opulan_flash_info *info = platform_get_drvdata(dev);

	platform_set_drvdata(dev, NULL);

	del_mtd_device(info->mtd);

	map_destroy(info->mtd);
	kfree(info->parts);
	kfree(info);

	return 0;
}



static struct platform_driver opulan_flash_driver = {
	.driver = {
		.name		= "Opulan-flash",
		.owner		= THIS_MODULE,
	},
	.probe		= opulan_flash_probe,
	.remove		= __devexit_p(opulan_flash_remove),
};

static int __init init_opulan_flash(void)
{
	return platform_driver_register(&opulan_flash_driver);
}

static void __exit cleanup_opulan_flash(void)
{
	platform_driver_unregister(&opulan_flash_driver);
}

module_init(init_opulan_flash);
module_exit(cleanup_opulan_flash);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Figo.zhang <figo1802@gmail.com>");
MODULE_DESCRIPTION("MTD map driver for Opulan Soc");
