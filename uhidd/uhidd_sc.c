/*-
 * Copyright (c) 2009, 2010, 2012 Kai Wang
 * Copyright (c) 2016 Alexander Kabaev
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Generic system control device (multimedia keys) support.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <dev/usb/usb.h>
#include <dev/usb/usbhid.h>
#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "uhidd.h"

static uint8_t free_key[] =
{
	/* Free Keys. */
	0x54, 0x5A, 0x5F, 0x60, 0x62, 0x63, 0x6F, 0x71, 0x72, 0x74,
	0x75, 0x7A, 0x7C, 0x7F,

	/* F13 - F24 */
	0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
	0x6E, 0x76,

	/* Keyboard International 1-6 */
	0x73, 0x70, 0x7D, 0x79, 0x7B, 0x5C,

	/* Keyboard LANG 1-5 */
	0xF2, 0xF1, 0x78, 0x77, 0x76
};

#define	_FREE_KEY_COUNT	((int)(sizeof(free_key)/sizeof(free_key[0])))

static void
sc_write_keymap_file(struct hid_interface *hi)
{
	char fpath[256];
	FILE *fp;
	int i;

	snprintf(fpath, sizeof(fpath), "/var/run/uhidd.%s/sc_keymap",
	    hi->dev);
	fp = fopen(fpath, "w+");
	if (fp == NULL) {
		syslog(LOG_ERR, "%s[%d] fopen %s failed: %m",
		    hi->dev, hi->ndx, fpath);
		return;
	}
	fprintf(fp, "0x%04x:0x%04x={\n", hi->vendor_id, hi->product_id);
	fprintf(fp, "\tsc_keymap={\n");
	for (i = 0; i < usage_system_num && i < _MAX_MM_KEY; i++) {
		if (hi->sc_keymap[i]) {
			if (!strcasecmp(usage_system[i], "Reserved"))
				fprintf(fp, "\t\t0x%X=", (unsigned) i);
			else
				fprintf(fp, "\t\t%s=", usage_system[i]);
			fprintf(fp, "\"0x%02X\"\n", hi->sc_keymap[i]);
		}
	}
	fprintf(fp, "\t}\n}\n");
	fclose(fp);
}

static int
sc_tr(struct hid_appcol *ha, struct hid_key hk, int make,
    struct hid_scancode *c, int len)
{
	struct hid_interface *hi;
	struct device_config *dconfig;

	assert(c != NULL && len > 0);

	hi = hid_appcol_get_parser_private(ha);
	assert(hi != NULL);

	/*
	 * Some consumer controls report regular(HUP_KEYBOARD) keyboard
	 * keys.
	 */
	if (hk.up == HUP_KEYBOARD)
		return (kbd_hid2key(ha, hk, make, c, len));

	if (hk.up != HUP_GENERIC_DESKTOP)
		return (0);

	(*c).make = make;

	/*
	 * Check if there is a user provided keymap.
	 */
	dconfig = config_find_device(hi->vendor_id, hi->product_id, hi->ndx);
	if (dconfig != NULL && dconfig->sc_keymap_set) {
		if (dconfig->sc_keymap[hk.code] != 0) {
			(*c).sc = dconfig->sc_keymap[hk.code];
			return (1);
		} else
			return (0);
	}
	if (uconfig.gconfig.sc_keymap_set) {
		if (uconfig.gconfig.sc_keymap[hk.code] != 0) {
			(*c).sc = uconfig.gconfig.sc_keymap[hk.code];
			return (1);
		} else
			return (0);
	}

	/*
	 * Check if there is a key translation in the in-memory keymap.
	 */
	if (hi->sc_keymap[hk.code] != 0) {
		(*c).sc = hi->sc_keymap[hk.code];
		return (1);
	}

	/*
	 * Try allocating a free key for this "HID key".
	 */
	if (hi->free_key_pos < _FREE_KEY_COUNT) {
		hi->sc_keymap[hk.code] = free_key[hi->free_key_pos];
		hi->free_key_pos++;
		sc_write_keymap_file(hi);
		PRINT1(1, "remembered new hid key map: 0x%x => 0x%02x\n",
		    hk.code, hi->sc_keymap[hk.code]);
		(*c).sc = hi->sc_keymap[hk.code];
		return (1);
	} else {
		PRINT1(1, "no more free key for hid key: 0x%x\n", hk.code);
		return (0);
	}
}

int
sc_match(struct hid_appcol *ha)
{
	struct hid_interface *hi;
	struct hid_report *hr;
	struct hid_field *hf;
	unsigned int u, up;

	hi = hid_appcol_get_parser_private(ha);
	assert(hi != NULL);

	/* Let the customer control knob cover system one too */
	if (config_cc_attach(hi) <= ATTACH_NO)
		return (HID_MATCH_NONE);

	u = hid_appcol_get_usage(ha);
	if (u != HID_USAGE2(HUP_GENERIC_DESKTOP, HUG_SYSTEM_CONTROL))
		return (HID_MATCH_NONE);

	hr = NULL;
	while ((hr = hid_appcol_get_next_report(ha, hr)) != NULL) {
		hf = NULL;
		while ((hf = hid_report_get_next_field(hr, hf, HID_INPUT)) !=
		    NULL) {
			up = hid_field_get_usage_page(hf);
			if (up == HUP_GENERIC_DESKTOP)
				return (HID_MATCH_GENERAL);
		}
	}

	return (HID_MATCH_NONE);
}

int
sc_attach(struct hid_appcol *ha)
{
	struct hid_interface *hi;

	hi = hid_appcol_get_parser_private(ha);
	assert(hi != NULL);

	if (kbd_attach(ha) < 0)
		return (-1);
	kbd_set_tr(ha, sc_tr);

	return (0);
}

#define MAX_KEYCODE 256

static void
sc_process_key(struct hid_appcol *ha __unused, struct hid_report *hr __unused,
    unsigned usage, int value, int *total, int *cnt, struct hid_key *keycodes)
{

	/* Skip the keys this driver can't handle. */
	if (HID_PAGE(usage) != HUP_GENERIC_DESKTOP)
		return;

	/* "total" counts all keys, pressed and released. */
	if (*total >= MAX_KEYCODE)
		return;
	(*total)++;

	if (value) {
		/* "cnt" counts pressed keys. */
		if (*cnt >= MAX_KEYCODE)
			return;
		keycodes[*cnt].code = HID_USAGE(usage);
		keycodes[*cnt].up = HID_PAGE(usage);
		(*cnt)++;
	}
}

void
sc_recv(struct hid_appcol *ha, struct hid_report *hr)
{
	struct hid_interface *hi;
	struct hid_field *hf;
	struct hid_key keycodes[MAX_KEYCODE];
	unsigned usage;
	int i, value, cnt, flags, total;

	hi = hid_appcol_get_parser_private(ha);
	assert(hi != NULL);

	total = 0;
	cnt = 0;
	memset(keycodes, 0, sizeof(keycodes));

	hf = NULL;
	while ((hf = hid_report_get_next_field(hr, hf, HID_INPUT)) != NULL) {
		flags = hid_field_get_flags(hf);
		if (flags & HIO_CONST)
			continue;
		for (i = 0; i < hf->hf_count; i++) {
			hid_field_get_usage_value(hf, i, &usage, &value);
			sc_process_key(ha, hr, usage, value, &total, &cnt,
			    keycodes);
		}
	}

	if (total > 0 && verbose > 1) {
		PRINT1(2, "hid codes: ");
		if (cnt == 0)
			printf("none");
		for (i = 0; i < cnt; i++)
			printf("0x%02X(0x%02X) ", keycodes[i].code,
			    keycodes[i].up);
		putchar('\n');
	}

	kbd_input(ha, 0, keycodes, total);
}
