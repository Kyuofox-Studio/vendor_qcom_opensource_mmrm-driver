// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
 */

#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/soc/qcom/msm_mmrm.h>

#include "mmrm_internal.h"
#include "mmrm_debug.h"
#include "mmrm_clk_rsrc_mgr.h"

#define	VERIFY_PDEV(pdev)	\
{							\
	if (!pdev) {			\
		d_mpr_e("%s: null platform dev\n", __func__);\
		rc = -EINVAL;		\
		goto err_exit; \
	}						\
}

#define RESET_DRV_DATA(drv_data)	\
{									\
	kfree(drv_data);				\
	drv_data = (void *) -EPROBE_DEFER; \
}


struct mmrm_driver_data *drv_data = (void *) -EPROBE_DEFER;

struct mmrm_client *mmrm_client_register(struct mmrm_client_desc *client_desc)
{
	struct mmrm_client *client = NULL;

	d_mpr_h("%s: entering\n", __func__);

	/* check for null input */
	if (!client_desc) {
		d_mpr_e("%s: null input descriptor\n", __func__);
		goto err_exit;
	}

	if (drv_data == (void *) -EPROBE_DEFER) {
		d_mpr_e("%s: mmrm probe_init not done\n", __func__);
		goto err_exit;
	}

	/* check for client type, then register */
	if (client_desc->client_type == MMRM_CLIENT_CLOCK) {
		client = mmrm_clk_client_register(
					drv_data->clk_mgr, client_desc);
		if (!client) {
			d_mpr_e("%s: failed to register client\n", __func__);
			goto err_exit;
		}
	} else {
		d_mpr_e("%s: unknown client_type %d\n",
			__func__, client_desc->client_type);
		goto err_exit;
	}

	d_mpr_h("%s: exiting\n", __func__);
	return client;

err_exit:
	d_mpr_e("%s: error exit\n", __func__);
	return client;
}
EXPORT_SYMBOL(mmrm_client_register);

int mmrm_client_deregister(struct mmrm_client *client)
{
	int rc = 0;

	d_mpr_h("%s: entering\n", __func__);

	/* check for null input */
	if (!client) {
		d_mpr_e("%s: invalid input client\n", __func__);
		rc = -EINVAL;
		goto err_exit;
	}

	if (drv_data == (void *) -EPROBE_DEFER) {
		d_mpr_e("%s: mmrm probe_init not done\n", __func__);
		goto err_exit;
	}

	/* check for client type, then deregister */
	if (client->client_type == MMRM_CLIENT_CLOCK) {
		rc = mmrm_clk_client_deregister(drv_data->clk_mgr, client);
		if (rc != 0) {
			d_mpr_e("%s: failed to deregister client\n", __func__);
			goto err_exit;
		}
	} else {
		d_mpr_e("%s: unknown client_type %d\n",
			__func__, client->client_type);
	}

	d_mpr_h("%s: exiting\n", __func__);
	return rc;

err_exit:
	d_mpr_e("%s: error exit\n", __func__);
	return rc;
}
EXPORT_SYMBOL(mmrm_client_deregister);

int mmrm_client_set_value(struct mmrm_client *client,
	struct mmrm_client_data *client_data, unsigned long val)
{
	int rc = 0;

	d_mpr_h("%s: entering\n", __func__);

	/* check for null input */
	if (!client || !client_data) {
		d_mpr_e("%s: invalid input client(%pK) client_data(%pK)\n",
			__func__, client, client_data);
		rc = -EINVAL;
		goto err_exit;
	}

	if (drv_data == (void *) -EPROBE_DEFER) {
		d_mpr_e("%s: mmrm probe_init not done\n", __func__);
		goto err_exit;
	}

	/* check for client type, then set value */
	if (client->client_type == MMRM_CLIENT_CLOCK) {
		rc = mmrm_clk_client_setval(drv_data->clk_mgr, client,
				client_data, val);
		if (rc != 0) {
			d_mpr_e("%s: failed to set value for client\n", __func__);
			goto err_exit;
		}
	} else {
		d_mpr_e("%s: unknown client_type %d\n",
			__func__, client->client_type);
	}

	d_mpr_h("%s: exiting\n", __func__);
	return rc;

err_exit:
	d_mpr_e("%s: error exit\n", __func__);
	return rc;
}
EXPORT_SYMBOL(mmrm_client_set_value);

int mmrm_client_set_value_in_range(struct mmrm_client *client,
	struct mmrm_client_data *client_data,
	struct mmrm_client_res_value *val)
{
	int rc = 0;

	d_mpr_h("%s: entering\n", __func__);

	/* check for null input */
	if (!client || !client_data || !val) {
		d_mpr_e(
			"%s: invalid input client(%pK) client_data(%pK) val(%pK)\n",
			__func__, client, client_data, val);
		rc = -EINVAL;
		goto err_exit;
	}

	if (drv_data == (void *) -EPROBE_DEFER) {
		d_mpr_e("%s: mmrm probe_init not done\n", __func__);
		goto err_exit;
	}

	/* check for client type, then set value */
	if (client->client_type == MMRM_CLIENT_CLOCK) {
		rc = mmrm_clk_client_setval_inrange(drv_data->clk_mgr,
				client, client_data, val);
		if (rc != 0) {
			d_mpr_e("%s: failed to set value for client\n", __func__);
			goto err_exit;
		}
	} else {
		d_mpr_e("%s: unknown client_type %d\n",
			__func__, client->client_type);
	}

	d_mpr_h("%s: exiting\n", __func__);
	return rc;

err_exit:
	d_mpr_e("%s: error exit\n", __func__);
	return rc;
}
EXPORT_SYMBOL(mmrm_client_set_value_in_range);

int mmrm_client_get_value(struct mmrm_client *client,
	struct mmrm_client_res_value *val)
{
	int rc = 0;

	d_mpr_h("%s: entering\n", __func__);

	/* check for null input */
	if (!client || !val) {
		d_mpr_e("%s: invalid input client(%pK) val(%pK)\n",
			__func__, client, val);
		rc = -EINVAL;
		goto err_exit;
	}

	if (drv_data == (void *) -EPROBE_DEFER) {
		d_mpr_e("%s: mmrm probe_init not done\n", __func__);
		goto err_exit;
	}

	/* check for client type, then get value */
	if (client->client_type == MMRM_CLIENT_CLOCK) {
		rc = mmrm_clk_client_getval(drv_data->clk_mgr,
				client, val);
		if (rc != 0) {
			d_mpr_e("%s: failed to get value for client\n", __func__);
			goto err_exit;
		}
	} else {
		d_mpr_e("%s: unknown client_type %d\n",
			__func__, client->client_type);
	}

	d_mpr_h("%s: exiting\n", __func__);
	return rc;

err_exit:
	d_mpr_e("%s: error exit\n", __func__);
	return rc;
}
EXPORT_SYMBOL(mmrm_client_get_value);

static int msm_mmrm_probe_init(struct platform_device *pdev)
{
	int rc = 0;

	d_mpr_h("%s: entering\n", __func__);

	drv_data = kzalloc(sizeof(*drv_data), GFP_KERNEL);
	if (!drv_data) {
		d_mpr_e("%s: unable to allocate memory for mmrm driver\n",
			__func__);
		rc = -ENOMEM;
		goto err_no_mem;
	}

	drv_data->platform_data = mmrm_get_platform_data(&pdev->dev);
	if (!drv_data->platform_data) {
		d_mpr_e("%s: unable to get platform data\n",
			__func__);
		rc = -EINVAL;
		goto err_get_drv_data;
	}

	drv_data->debugfs_root = msm_mmrm_debugfs_init();
	if (!drv_data->debugfs_root)
		d_mpr_e("%s: failed to create debugfs for mmrm\n", __func__);

	dev_set_drvdata(&pdev->dev, drv_data);

	rc = mmrm_read_platform_resources(pdev, drv_data);
	if (rc) {
		d_mpr_e("%s: unable to read platform resources for mmrm\n",
			__func__);
		goto err_read_pltfrm_rsc;
	}

	rc = mmrm_init(drv_data);
	if (rc) {
		d_mpr_e("%s: failed to init mmrm\n",
			__func__);
		goto err_mmrm_init;
	}

	d_mpr_h("%s: exiting with success\n", __func__);
	return rc;

err_mmrm_init:
	msm_mmrm_debugfs_deinit(drv_data->debugfs_root);
err_read_pltfrm_rsc:
	mmrm_free_platform_resources(drv_data);
err_get_drv_data:
	RESET_DRV_DATA(drv_data);
err_no_mem:
	d_mpr_e("%s: error exit\n", __func__);
	return rc;
}

static int msm_mmrm_probe(struct platform_device *pdev)
{
	int rc = -EINVAL;

	d_mpr_h("%s: entering\n", __func__);

	VERIFY_PDEV(pdev)

	if (of_device_is_compatible(pdev->dev.of_node, "qcom,msm-mmrm"))
		return msm_mmrm_probe_init(pdev);

	d_mpr_h("%s: exiting: no compatible device node\n", __func__);
	return rc;

err_exit:
	d_mpr_e("%s: error exit\n", __func__);
	return rc;
}

static int msm_mmrm_remove(struct platform_device *pdev)
{
	int rc = 0;

	VERIFY_PDEV(pdev);

	drv_data = dev_get_drvdata(&pdev->dev);
	if (!drv_data) {
		d_mpr_e("%s: null driver data\n", __func__);
		return -EINVAL;
	}

	msm_mmrm_debugfs_deinit(drv_data->debugfs_root);
	mmrm_deinit(drv_data);
	mmrm_free_platform_resources(drv_data);
	dev_set_drvdata(&pdev->dev, NULL);
	RESET_DRV_DATA(drv_data);

	d_mpr_h("%s: exiting with success\n", __func__);
	return rc;

err_exit:
	d_mpr_e("%s: error exit\n", __func__);
	return rc;
}

static const struct of_device_id msm_mmrm_dt_match[] = {
	{.compatible = "qcom,msm-mmrm"},
	{}
};

MODULE_DEVICE_TABLE(of, msm_mmrm_dt_match);

static struct platform_driver msm_mmrm_driver = {
	.probe = msm_mmrm_probe,
	.remove = msm_mmrm_remove,
	.driver = {
		.name = "msm-mmrm",
		.of_match_table = msm_mmrm_dt_match,
	},
};

static int __init msm_mmrm_init(void)
{
	int rc = 0;

	d_mpr_h("%s: entering\n", __func__);

	rc = platform_driver_register(&msm_mmrm_driver);
	if (rc) {
		d_mpr_e("%s: failed to register platform driver\n",
			__func__);
		goto err_platform_drv_reg;
	}

	d_mpr_h("%s: exiting\n", __func__);
	return rc;

err_platform_drv_reg:
	d_mpr_e("%s: error exit\n", __func__);
	return rc;
}

static void __exit msm_mmrm_exit(void)
{
	d_mpr_h("%s: entering\n", __func__);
	platform_driver_unregister(&msm_mmrm_driver);
	d_mpr_h("%s: exiting\n", __func__);
}

module_init(msm_mmrm_init);
module_exit(msm_mmrm_exit);

MODULE_DESCRIPTION("QTI MMRM Driver");
MODULE_LICENSE("GPL v2");
