// SPDX-License-Identifier: GPL-2.0
/*
 * TI K3 Flash Subsystem Application Subsystem (FSS_FSAS) driver
 *
 * Copyright (C) 2025 Texas Instruments Incorporated - https://www.ti.com
 */

#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/pm.h>

#define FSAS_GENREGS_SYSCONFIG 0x04
#define FSAS_SYSCONFIG_DISXIP BIT(7)

struct k3_fsas {
	void __iomem *base;
};

static void k3_fsas_disable_xip_prefetch(struct k3_fsas *fsas)
{
	u32 val;

	val = readl(fsas->base + FSAS_GENREGS_SYSCONFIG);
	val |= FSAS_SYSCONFIG_DISXIP;
	writel(val, fsas->base + FSAS_GENREGS_SYSCONFIG);
}

static int k3_fsas_probe(struct platform_device *pdev)
{
	struct k3_fsas *fsas;

	fsas = devm_kzalloc(&pdev->dev, sizeof(*fsas), GFP_KERNEL);
	if (!fsas)
		return -ENOMEM;

	fsas->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(fsas->base))
		return PTR_ERR(fsas->base);

	platform_set_drvdata(pdev, fsas);

	k3_fsas_disable_xip_prefetch(fsas);

	return 0;
}

static int k3_fsas_resume(struct device *dev)
{
	k3_fsas_disable_xip_prefetch(dev_get_drvdata(dev));
	return 0;
}

static DEFINE_SIMPLE_DEV_PM_OPS(k3_fsas_pm_ops, NULL, k3_fsas_resume);

static const struct of_device_id k3_fsas_of_match[] = {
	{ .compatible = "ti,am62a-fsas" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, k3_fsas_of_match);

static struct platform_driver k3_fsas_driver = {
	.probe = k3_fsas_probe,
	.driver = {
		.name		= "k3-fsas",
		.of_match_table	= k3_fsas_of_match,
		.pm		= pm_ptr(&k3_fsas_pm_ops),
	},
};
module_platform_driver(k3_fsas_driver);

MODULE_DESCRIPTION("TI K3 Flash SubSystem Application Subsystem driver");
MODULE_LICENSE("GPL");
