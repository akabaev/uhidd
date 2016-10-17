/*-
 * Copyright (c) 2010 Kai Wang
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

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include "uhidd.h"

static const char *_usage_system[] = {
/*==========================================*/
/* Usage ID      Usage Name                 */
/*==========================================*/
/*   0 */	"Unassigned",
/*   1 */	"Unassigned",
/*   2 */	"Unassigned",
/*   3 */	"Unassigned",
/*   4 */	"Unassigned",
/*   5 */	"Unassigned",
/*   6 */	"Unassigned",
/*   7 */	"Unassigned",
/*   8 */	"Unassigned",
/*   9 */	"Unassigned",
/*   A */	"Unassigned",
/*   B */	"Unassigned",
/*   C */	"Unassigned",
/*   D */	"Unassigned",
/*   E */	"Unassigned",
/*   F */	"Unassigned",
/*  10 */	"Unassigned",
/*  11 */	"Unassigned",
/*  12 */	"Unassigned",
/*  13 */	"Unassigned",
/*  14 */	"Unassigned",
/*  15 */	"Unassigned",
/*  16 */	"Unassigned",
/*  17 */	"Unassigned",
/*  18 */	"Unassigned",
/*  19 */	"Unassigned",
/*  1A */	"Unassigned",
/*  1B */	"Unassigned",
/*  1C */	"Unassigned",
/*  1D */	"Unassigned",
/*  1E */	"Unassigned",
/*  1F */	"Unassigned",
/*  20 */	"Unassigned",	/* +10 */
/*  21 */	"Unassigned",	/* +100 */
/*  22 */	"Unassigned",
/*  23 */	"Unassigned",
/*  24 */	"Unassigned",
/*  25 */	"Unassigned",
/*  26 */	"Unassigned",
/*  27 */	"Unassigned",
/*  28 */	"Unassigned",
/*  29 */	"Unassigned",
/*  2A */	"Unassigned",
/*  2B */	"Unassigned",
/*  2C */	"Unassigned",
/*  2D */	"Unassigned",
/*  2E */	"Unassigned",
/*  2F */	"Unassigned",
/*  30 */	"Unassigned",
/*  31 */	"Unassigned",
/*  32 */	"Unassigned",
/*  33 */	"Unassigned",
/*  34 */	"Unassigned",
/*  35 */	"Unassigned",
/*  36 */	"Unassigned",
/*  37 */	"Unassigned",
/*  38 */	"Unassigned",
/*  39 */	"Unassigned",
/*  3A */	"Unassigned",
/*  3B */	"Unassigned",
/*  3C */	"Unassigned",
/*  3D */	"Unassigned",
/*  3E */	"Unassigned",
/*  3F */	"Unassigned",
/*  40 */	"Unassigned",
/*  41 */	"Unassigned",
/*  42 */	"Unassigned",
/*  43 */	"Unassigned",
/*  44 */	"Unassigned",
/*  45 */	"Unassigned",
/*  46 */	"Unassigned",
/*  47 */	"Unassigned",
/*  48 */	"Unassigned",
/*  49 */	"Unassigned",
/*  4A */	"Unassigned",
/*  4B */	"Unassigned",
/*  4C */	"Unassigned",
/*  4D */	"Unassigned",
/*  4E */	"Unassigned",
/*  4F */	"Unassigned",
/*  50 */	"Unassigned",
/*  51 */	"Unassigned",
/*  52 */	"Unassigned",
/*  53 */	"Unassigned",
/*  54 */	"Unassigned",
/*  55 */	"Unassigned",
/*  56 */	"Unassigned",
/*  57 */	"Unassigned",
/*  58 */	"Unassigned",
/*  59 */	"Unassigned",
/*  5A */	"Unassigned",
/*  5B */	"Unassigned",
/*  5C */	"Unassigned",
/*  5D */	"Unassigned",
/*  5E */	"Unassigned",
/*  5F */	"Unassigned",
/*  60 */	"Unassigned",
/*  61 */	"Unassigned",
/*  62 */	"Unassigned",
/*  63 */	"Unassigned",
/*  64 */	"Unassigned",
/*  65 */	"Unassigned",
/*  66 */	"Unassigned",
/*  67 */	"Unassigned",
/*  68 */	"Unassigned",
/*  69 */	"Unassigned",
/*  6A */	"Unassigned",
/*  6B */	"Unassigned",
/*  6C */	"Unassigned",
/*  6D */	"Unassigned",
/*  6E */	"Unassigned",
/*  6F */	"Unassigned",
/*  70 */	"Unassigned",
/*  71 */	"Unassigned",
/*  72 */	"Unassigned",
/*  73 */	"Unassigned",
/*  74 */	"Unassigned",
/*  75 */	"Unassigned",
/*  76 */	"Unassigned",
/*  77 */	"Unassigned",
/*  78 */	"Unassigned",
/*  79 */	"Unassigned",
/*  7A */	"Unassigned",
/*  7B */	"Unassigned",
/*  7C */	"Unassigned",
/*  7D */	"Unassigned",
/*  7E */	"Unassigned",
/*  7F */	"Unassigned",
/*  80 */	"System_Control",
/*  81 */	"System_Power_Down",
/*  82 */	"System_Sleep",
/*  83 */	"Systems_Wake_Up",
};

int usage_system_num = sizeof(_usage_system) / sizeof(_usage_system[0]);
const char **usage_system = _usage_system;
