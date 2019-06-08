/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include "settings/settings.h"
#include "settings_priv.h"
#include <zephyr/types.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(settings, CONFIG_SETTINGS_LOG_LEVEL);

sys_slist_t settings_handlers;


void settings_store_init(void);

void settings_init(void)
{
	sys_slist_init(&settings_handlers);
	settings_store_init();
}

int settings_register(struct settings_handler *handler)
{
	struct settings_handler *ch;

	SYS_SLIST_FOR_EACH_CONTAINER(&settings_handlers, ch, node) {
		/* avoid registering a setting handler that is sub or a super of
		 * existing handler
		 */
		if ((settings_name_cmp(handler->name, ch->name, NULL) == 1) ||
		    (settings_name_cmp(ch->name, handler->name, NULL) == 1)) {
			return -EINVAL;
		}
		/* avoid registering existing handlers */
		if (settings_name_cmp(handler->name, ch->name, NULL) == 0) {
			return -EEXIST;
		}

	}
	sys_slist_prepend(&settings_handlers, &handler->node);

	return 0;
}

int settings_name_cmp(const char *name, const char *key, const char **next)
{
	if (next) {
		*next = NULL;
	}

	/* name might come from flash directly, in flash the name would end
	 * with '=' or '\0' depending how storage is done. Flash reading is
	 * limited to what can be read
	 */

	while ((*key != '\0') && (*key == *name) &&
	       (*name != '\0') && (*name != SETTINGS_NAME_END)) {
		key++;
		name++;
	}

	if (*key != '\0') {
		return -ENOENT;
	}

	if (*name == SETTINGS_NAME_SEPARATOR) {
		if (next) {
			*next = name + 1;
		}
		return 1;
	}

	if ((*name == SETTINGS_NAME_END) || (*name == '\0')) {
		return 0;
	}

	return -ENOENT;
}

int settings_name_split(const char *name, char *argv, const char **next)
{
	if (next) {
		*next = NULL;
	}

	/* name might come from flash directly, in flash the name would end
	 * with '=' or '\0' depending how storage is done. Flash reading is
	 * limited to what can be read
	 */
	while ((*name != '\0') && (*name != SETTINGS_NAME_END) &&
	       (*name != SETTINGS_NAME_SEPARATOR)) {
		*argv++ = *name++;
	}


	if (*name == SETTINGS_NAME_SEPARATOR) {
		*argv = '\0';
		if (next) {
			*next = name + 1;
		}
		return 1;
	}

	if ((*name == '\0') || (*name == SETTINGS_NAME_END)) {
		*argv = '\0';
		return 0;
	}

	return -ENOENT;
}

/*
 * Separate string into argv array.
 */
int settings_parse_name(char *name, int *name_argc, char *name_argv[])
{
	int i = 0;

	while (name) {
		name_argv[i++] = name;

		while (1) {
			if (*name == '\0') {
				name = NULL;
				break;
			}

			if (*name == SETTINGS_NAME_SEPARATOR) {
				*name = '\0';
				name++;
				break;
			}
			name++;
		}
	}

	*name_argc = i;

	return 0;
}

struct settings_handler *settings_parse_and_lookup(const char *name,
						   const char **next)
{
	struct settings_handler *ch;

	SYS_SLIST_FOR_EACH_CONTAINER(&settings_handlers, ch, node) {
		if (settings_name_cmp(name, ch->name, next) >= 0) {
			return ch;
		}
	}
	return NULL;
}

int settings_commit(void)
{
	struct settings_handler *ch;
	int rc;
	int rc2;

	rc = 0;
	SYS_SLIST_FOR_EACH_CONTAINER(&settings_handlers, ch, node) {
		if (ch->h_commit) {
			rc2 = ch->h_commit();
			if (!rc) {
				rc = rc2;
			}
		}
	}
	return rc;
}
