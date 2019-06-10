/*
 * NSETTINGS Sample for Zephyr
 *
 * Copyright (c) 2018 Laczen
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <zephyr.h>
#include <misc/reboot.h>
#include <device.h>
#include <string.h>
#include <settings/settings.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME      2000

u8_t reset_counter;

static int ps_set(const char *key, size_t len, settings_read_cb read,
		  void *cb_arg)
{
	const char *dyn_name;
	char argv[32];

	if (key == NULL) {
		return 0;
	}

	/* use settings_name_cmp to compare with a fixed key */
	if (settings_name_cmp(key, "ra0", NULL)) {
		printk("Loading ra0\n");
	}

	/* use settings_name_cmp to compare with a fixed key */
	if (settings_name_cmp(key, "ra1", NULL)) {
		printk("Loading ra1\n");
	}

	/* use settings_name_cmp to compare with a fixed key */
	if (settings_name_cmp(key, "ra2", NULL)) {
		printk("Loading ra2\n");
	}

	/* use settings_name_cmp to compare with a fixed key */
	if (settings_name_cmp(key, "rb", &dyn_name)) {
		printk("Loading rb\n");
		while (dyn_name != NULL) {
			/* use settings_name_split to read generated names */
			settings_name_split(dyn_name, argv, &dyn_name);
			printk("  Found: [%s]\n", argv);
		}
	}
	return 0;
}

static struct settings_handler ps_settings = {
	.name = "ps",
	.h_set = ps_set,
};

static struct settings_handler pt_test_settings = {
	.name = "pt/test",
	.h_set = ps_set,
};

static struct settings_handler pt_test_t_settings = {
	.name = "pt/test/t",
	.h_set = ps_set,
};

int ps_settings_init(void)
{
	int err;

	err = settings_subsys_init();
	if (err) {
		printk("settings_subsys_init failed (err %d)\n", err);
		return err;
	}

	err = settings_register(&ps_settings);
	if (err) {
		printk("ps_settings_register failed (err %d)\n", err);
		return err;
	}

	err = settings_register(&pt_test_t_settings);
	if (err) {
		printk("pt_test_t_settings_register failed (err %d)\n", err);
		return err;
	}

	err = settings_register(&pt_test_settings);
	if (err) {
		printk("pt_test_settings_register failed (err %d)\n", err);
		return err;
	}
	return 0;
}

void main(void)
{
	reset_counter = 0U;
	int err;

	printk("Started\n");
	ps_settings_init();
	printk("Initialized...");
	settings_load();
	printk("Loaded\n");

	settings_save_one("ps/ra0", &reset_counter, sizeof(reset_counter));
	settings_save_one("ps/ra1", &reset_counter, sizeof(reset_counter));
	settings_save_one("ps/ra2", &reset_counter, sizeof(reset_counter));
	settings_save_one("ps/rb/0x7fff/0xffff", &reset_counter,
			  sizeof(reset_counter));
	settings_save_one("pt/test/ra0", &reset_counter, sizeof(reset_counter));
	settings_save_one("pt/test/ra1", &reset_counter, sizeof(reset_counter));

	printk("Basic load test - Everything\n");
	settings_load();

	printk("Subtree load test part 1 - Only ps\n");
	settings_load_subtree("ps");
	printk("Subtree load test part 2 - Only pt/test\n");
	settings_load_subtree("pt/test");
	printk("Subtree load test part 3 - Only ps/ra0\n");
	settings_load_subtree("ps/ra0");
	printk("Finished");

	err = settings_deregister(&pt_test_settings);
	printk("Load test - Everything with pt/test deregistered\n");
	settings_load();


}
