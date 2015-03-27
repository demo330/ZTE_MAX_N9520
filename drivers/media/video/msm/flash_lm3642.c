/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*
 * Created by ZTE_JIA_20140331 jia.jia
 */

#include <linux/module.h>
#include "flash_zte.h"

#define FLASH_NAME  "lm3642"

#define LM3642_FLAGS_REG  0x0B

static struct msm_flash_ctrl_t lm3642_s_ctrl;

static struct msm_camera_i2c_reg_conf lm3642_init_setting[] = {
	{ 0x0A, 0x00 }, // enable register, standby mode, strobe pin disabled
};

static struct msm_camera_i2c_reg_conf lm3642_release_setting[] = {
	{ 0x0A, 0x00 }, // enable register, standby mode, strobe pin disabled
};

static struct msm_camera_i2c_reg_conf lm3642_off_setting[] = {
	{ 0x0A, 0x00 }, // enable register, standby mode, strobe pin disabled
};

static struct msm_camera_i2c_reg_conf lm3642_low_setting[] = {
	{ 0x06, 0x00 }, // torch time, ramp-up time 16ms, ramp-down time 16ms
	{ 0x09, 0x28 }, // current control, torch current 140.63mA, flash current 843.75mA
	{ 0x01, 0x10 }, // IVFM mode, UVLO disabled, IVM-D threshold 3.3v
	{ 0x0A, 0x42 }, // enable register, IVFM disabled, TX pin enabled, strobe pin disabled, torch pin disabled, torch mode,
};

static struct msm_camera_i2c_reg_conf lm3642_high_setting[] = {
	{ 0x08, 0x57 }, // flash time, ramp time 1.024ms, time-out time 800ms
	{ 0x09, 0x29 }, // current control, torch current 140.63mA, flash current 937.5mA
	{ 0x01, 0x10 }, // IVFM mode, UVLO disabled, IVM-D threshold 3.3v
	{ 0x0A, 0xC3 }, // enable register, IVFM enabled, TX pin enabled, strobe pin disabled, torch pin disabled, flash mode
};

static int32_t lm3642_i2c_read(struct msm_camera_i2c_client *client,	uint16_t addr, uint16_t *data,
																		enum msm_camera_i2c_data_type data_type);
static int32_t lm3642_i2c_write(struct msm_camera_i2c_client *client, uint16_t addr, uint16_t data,
																		enum msm_camera_i2c_data_type data_type);
static int32_t lm3642_i2c_write_table(struct msm_flash_ctrl_t *fctrl,
																						struct msm_camera_i2c_reg_conf *table,
																						int32_t num);
static int32_t lm3642_flash_init(struct msm_flash_ctrl_t *fctrl);
static int32_t lm3642_flash_release(struct msm_flash_ctrl_t *fctrl);
static int32_t lm3642_flash_off(struct msm_flash_ctrl_t *fctrl);
static int32_t lm3642_flash_low(struct msm_flash_ctrl_t *fctrl);
static int32_t lm3642_flash_high(struct msm_flash_ctrl_t *fctrl);
static void lm3642_i2c_shutdown(struct i2c_client *client);

static int32_t lm3642_i2c_read(struct msm_camera_i2c_client *client,	uint16_t addr, uint16_t *data,
																		enum msm_camera_i2c_data_type data_type)
{
	int32_t rc = 0;

	*data = (uint16_t)i2c_smbus_read_byte_data(client->client, (uint8_t)addr);
	rc = (int32_t)*data;

	return rc;
}

static int32_t lm3642_i2c_write(struct msm_camera_i2c_client *client, uint16_t addr, uint16_t data,
																		enum msm_camera_i2c_data_type data_type)
{
	return i2c_smbus_write_byte_data(client->client, (uint8_t)addr, (uint8_t)data);
}

static int32_t lm3642_i2c_write_table(struct msm_flash_ctrl_t *fctrl,
																						struct msm_camera_i2c_reg_conf *table,
																						int32_t num)
{
	int32_t i = 0, rc = 0;

	for (i = 0; i < num; ++i) {
		rc = fctrl->i2c_ftbl->i2c_write(fctrl->i2c_client,
																		table->reg_addr,
																		table->reg_data,
																		fctrl->reg_setting->default_data_type);
		if (rc < 0) {
			break;
		}

		table++;
	}

	return rc;
}

static int32_t lm3642_flash_init(struct msm_flash_ctrl_t *fctrl)
{
	uint16_t flags = 0;
	int32_t rc = 0;

	CDBG("%s: E\n", __func__);

	/*
	 * Faults require a read-back of the "Flags Register" to resume operation.
	 * Flags report an event occurred, but do not inhibit future functionality.
	 * A read-back of the Flags Register will only get updated again if the fault or flags
	 * is still present upon a restart.
	 */
	rc = fctrl->i2c_ftbl->i2c_read(fctrl->i2c_client,
																 LM3642_FLAGS_REG,
																 &flags,
																 fctrl->reg_setting->default_data_type);
	if (rc < 0) {
		pr_err("%s: read flags failed\n", __func__);
		return rc;
	}
	CDBG("%s: flags: 0x%x\n", __func__, flags);

	CDBG("%s: X\n", __func__);

	return 0;
}

static int32_t lm3642_flash_release(struct msm_flash_ctrl_t *fctrl)
{
	uint16_t flags = 0;

	CDBG("%s: E\n", __func__);

	/*
	 * Faults require a read-back of the "Flags Register" to resume operation.
	 * Flags report an event occurred, but do not inhibit future functionality.
	 * A read-back of the Flags Register will only get updated again if the fault or flags
	 * is still present upon a restart.
	 */
	(void)fctrl->i2c_ftbl->i2c_read(fctrl->i2c_client,
																	LM3642_FLAGS_REG,
																	&flags,
																	fctrl->reg_setting->default_data_type);
	CDBG("%s: flags: 0x%x\n", __func__, flags);

	(void)lm3642_i2c_write_table(fctrl, fctrl->reg_setting->release_setting, fctrl->reg_setting->release_setting_size);

	CDBG("%s: X\n", __func__);

	return 0;
}

static int32_t lm3642_flash_off(struct msm_flash_ctrl_t *fctrl)
{
	uint16_t flags = 0;

	CDBG("%s: E\n", __func__);

	/*
	 * Faults require a read-back of the "Flags Register" to resume operation.
	 * Flags report an event occurred, but do not inhibit future functionality.
	 * A read-back of the Flags Register will only get updated again if the fault or flags
	 * is still present upon a restart.
	 */
	(void)fctrl->i2c_ftbl->i2c_read(fctrl->i2c_client,
																	LM3642_FLAGS_REG,
																	&flags,
																	fctrl->reg_setting->default_data_type);
	CDBG("%s: flags: 0x%x\n", __func__, flags);

	(void)lm3642_i2c_write_table(fctrl, fctrl->reg_setting->off_setting, fctrl->reg_setting->off_setting_size);

	CDBG("%s: X\n", __func__);

	return 0;
}

static int32_t lm3642_flash_low(struct msm_flash_ctrl_t *fctrl)
{
	uint16_t flags = 0;
	int32_t rc = 0;

	CDBG("%s: E\n", __func__);

	/*
	 * Faults require a read-back of the "Flags Register" to resume operation.
	 * Flags report an event occurred, but do not inhibit future functionality.
	 * A read-back of the Flags Register will only get updated again if the fault or flags
	 * is still present upon a restart.
	 */
	rc = fctrl->i2c_ftbl->i2c_read(fctrl->i2c_client,
																LM3642_FLAGS_REG,
																&flags,
																fctrl->reg_setting->default_data_type);
	if (rc < 0) {
		pr_err("%s: read flags failed\n", __func__);
		return rc;
	}
	CDBG("%s: flags: 0x%x\n", __func__, flags);

	rc = lm3642_i2c_write_table(fctrl, fctrl->reg_setting->low_setting, fctrl->reg_setting->low_setting_size);
	if (rc < 0) {
		pr_err("%s: i2c write table failed\n", __func__);
		return rc;
	}

	CDBG("%s: X\n", __func__);

	return 0;
}

static int32_t lm3642_flash_high(struct msm_flash_ctrl_t *fctrl)
{
	uint16_t flags = 0;
	int32_t rc = 0;

	CDBG("%s: E\n", __func__);

	/*
	 * Faults require a read-back of the "Flags Register" to resume operation.
	 * Flags report an event occurred, but do not inhibit future functionality.
	 * A read-back of the Flags Register will only get updated again if the fault or flags
	 * is still present upon a restart.
	 */
	rc = fctrl->i2c_ftbl->i2c_read(fctrl->i2c_client,
																LM3642_FLAGS_REG,
																&flags,
																fctrl->reg_setting->default_data_type);
	if (rc < 0) {
		pr_err("%s: read flags failed\n", __func__);
		return rc;
	}
	CDBG("%s: flags: 0x%x\n", __func__, flags);

	rc = lm3642_i2c_write_table(fctrl, fctrl->reg_setting->high_setting, fctrl->reg_setting->high_setting_size);
	if (rc < 0) {
		pr_err("%s: i2c write table failed\n", __func__);
		return rc;
	}

	CDBG("%s: X\n", __func__);

	return 0;
}

/*
 * Add shutdown implementation to fix potential exceptional status of flash IC
 * in power-down or reboot procedure.
 */
static void lm3642_i2c_shutdown(struct i2c_client *client)
{
	(void)lm3642_s_ctrl.flash_ftbl->flash_init(&lm3642_s_ctrl);
	(void)lm3642_s_ctrl.flash_ftbl->flash_off(&lm3642_s_ctrl);
}

static struct msm_camera_i2c_client lm3642_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_BYTE_ADDR,
};

static struct msm_camera_i2c_fn_t lm3642_i2c_ftbl = {
	.i2c_read = lm3642_i2c_read,
	.i2c_write = lm3642_i2c_write,
};

static struct msm_flash_fn_t lm3642_flash_ftbl = {
	.flash_init = lm3642_flash_init,
	.flash_release = lm3642_flash_release,
	.flash_off = lm3642_flash_off,
	.flash_low = lm3642_flash_low,
	.flash_high = lm3642_flash_high,
};

static struct msm_flash_reg_t lm3642_reg_setting = {
	.default_data_type = MSM_CAMERA_I2C_BYTE_DATA,
	.init_setting = lm3642_init_setting,
	.init_setting_size = ARRAY_SIZE(lm3642_init_setting),
	.release_setting = lm3642_release_setting,
	.release_setting_size = ARRAY_SIZE(lm3642_release_setting),
	.off_setting = lm3642_off_setting,
	.off_setting_size = ARRAY_SIZE(lm3642_off_setting),
	.low_setting = lm3642_low_setting,
	.low_setting_size = ARRAY_SIZE(lm3642_low_setting),
	.high_setting = lm3642_high_setting,
	.high_setting_size = ARRAY_SIZE(lm3642_high_setting),
};

static struct msm_flash_ctrl_t lm3642_s_ctrl = {
	.i2c_client = &lm3642_i2c_client,
	.i2c_ftbl = &lm3642_i2c_ftbl,
	.flash_ftbl = &lm3642_flash_ftbl,
	.reg_setting = &lm3642_reg_setting,
};

static const struct i2c_device_id lm3642_i2c_id[] = {
	{FLASH_NAME, (kernel_ulong_t)&lm3642_s_ctrl},
	{ },
};

static struct i2c_driver lm3642_i2c_driver = {
	.id_table = lm3642_i2c_id,
	.probe = msm_flash_i2c_probe,
	.remove = msm_flash_i2c_remove,
	.shutdown = lm3642_i2c_shutdown,
	.driver = {
		.name = FLASH_NAME,
		.owner = THIS_MODULE,
	},
};

static int __init lm3642_init_module(void)
{
	return i2c_add_driver(&lm3642_i2c_driver);
}

static void __exit lm3642_exit_module(void)
{
	i2c_del_driver(&lm3642_i2c_driver);
}

module_init(lm3642_init_module);
module_exit(lm3642_exit_module);

MODULE_DESCRIPTION("LM3642 Flash Driver");
MODULE_LICENSE("GPL v2");
