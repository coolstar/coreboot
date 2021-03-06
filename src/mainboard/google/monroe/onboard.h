/*
 * This file is part of the coreboot project.
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

#ifndef __MAINBOARD_ONBOARD_H
#define __MAINBOARD_ONBOARD_H

/* defines for programming the MAC address */
#define MONROE_NIC_VENDOR_ID		0x10EC
#define MONROE_NIC_DEVICE_ID		0x8168

/* 0x00: White LINK LED and Amber ACTIVE LED */
#define MONROE_NIC_LED_MODE		0x00

/* NIC wake is GPIO 8 */
#define MONROE_NIC_WAKE_GPIO		8

/* WLAN wake is GPIO 10 */
#define MONROE_WLAN_WAKE_GPIO		10

/* IT8772F defs */
#define IT8772F_BASE 0x2e
#define IT8772F_SERIAL_DEV PNP_DEV(IT8772F_BASE, IT8772F_SP1)
#define IT8772F_GPIO_DEV PNP_DEV(IT8772F_BASE, IT8772F_GPIO)
#define IT8772F_DUMMY_DEV PNP_DEV(IT8772F_BASE, 0)

#ifndef __ACPI__
void lan_init(void);
#endif

#endif /* __MAINBOARD_ONBOARD_H */
