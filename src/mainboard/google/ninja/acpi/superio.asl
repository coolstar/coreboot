/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* mainboard configuration */
#include <mainboard/google/ninja/ec.h>
#include <mainboard/google/ninja/onboard.h>

#define SIO_EC_MEMMAP_ENABLE	// EC Memory Map Resources
#define SIO_EC_HOST_ENABLE	// EC Host Interface Resources
#undef SIO_EC_ENABLE_PS2K       // Disable PS/2 Keyboard
#undef SIO_EC_PS2K_IRQ		// Unset its IRQ

/* ACPI code for EC SuperIO functions */
#include <ec/google/chromeec/acpi/superio.asl>
