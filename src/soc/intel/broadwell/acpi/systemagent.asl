/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
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

#include <soc/iomap.h>

Name (_HID, EISAID ("PNP0A08"))	// PCIe
Name (_CID, EISAID ("PNP0A03"))	// PCI

Name (_ADR, 0)
Name (_BBN, 0)

Device (MCHC)
{
	Name (_ADR, 0x00000000)	// 0:0.0

	OperationRegion (MCHP, PCI_Config, 0x00, 0x100)
	Field (MCHP, DWordAcc, NoLock, Preserve)
	{
		Offset (0x70),	// ME Base Address
		MEBA,	 64,
		Offset (0xa0),	// Top of Used Memory
		TOM,	 64,
		Offset (0xbc),	// Top of Low Used Memory
		TLUD,	 32,
	}
}

// Current Resource Settings
Name (MCRS, ResourceTemplate()
{
	// Bus Numbers
	WordBusNumber (ResourceProducer, MinFixed, MaxFixed, PosDecode,
			0x0000, 0x0000, 0x00ff, 0x0000, 0x0100,,, PB00)

	// IO Region 0
	DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
			0x0000, 0x0000, 0x0cf7, 0x0000, 0x0cf8,,, PI00)

	// PCI Config Space
	Io (Decode16, 0x0cf8, 0x0cf8, 0x0001, 0x0008)

	// IO Region 1
	DWordIO (ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange,
			0x0000, 0x0d00, 0xffff, 0x0000, 0xf300,,, PI01)

	// VGA memory (0xa0000-0xbffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000a0000, 0x000bffff, 0x00000000,
			0x00020000,,, ASEG)

	// OPROM reserved (0xc0000-0xc3fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000c0000, 0x000c3fff, 0x00000000,
			0x00004000,,, OPR0)

	// OPROM reserved (0xc4000-0xc7fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000c4000, 0x000c7fff, 0x00000000,
			0x00004000,,, OPR1)

	// OPROM reserved (0xc8000-0xcbfff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000c8000, 0x000cbfff, 0x00000000,
			0x00004000,,, OPR2)

	// OPROM reserved (0xcc000-0xcffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000cc000, 0x000cffff, 0x00000000,
			0x00004000,,, OPR3)

	// OPROM reserved (0xd0000-0xd3fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000d0000, 0x000d3fff, 0x00000000,
			0x00004000,,, OPR4)

	// OPROM reserved (0xd4000-0xd7fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000d4000, 0x000d7fff, 0x00000000,
			0x00004000,,, OPR5)

	// OPROM reserved (0xd8000-0xdbfff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000d8000, 0x000dbfff, 0x00000000,
			0x00004000,,, OPR6)

	// OPROM reserved (0xdc000-0xdffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000dc000, 0x000dffff, 0x00000000,
			0x00004000,,, OPR7)

	// BIOS Extension (0xe0000-0xe3fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000e0000, 0x000e3fff, 0x00000000,
			0x00004000,,, ESG0)

	// BIOS Extension (0xe4000-0xe7fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000e4000, 0x000e7fff, 0x00000000,
			0x00004000,,, ESG1)

	// BIOS Extension (0xe8000-0xebfff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000e8000, 0x000ebfff, 0x00000000,
			0x00004000,,, ESG2)

	// BIOS Extension (0xec000-0xeffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000ec000, 0x000effff, 0x00000000,
			0x00004000,,, ESG3)

	// System BIOS (0xf0000-0xfffff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x000f0000, 0x000fffff, 0x00000000,
			0x00010000,,, FSEG)

	// PCI Memory Region (Top of memory-CONFIG_MMCONF_BASE_ADDRESS)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000,,, PM01)

	// TPM Area (0xfed40000-0xfed44fff)
	DWordMemory (ResourceProducer, PosDecode, MinFixed, MaxFixed,
			Cacheable, ReadWrite,
			0x00000000, 0xfed40000, 0xfed44fff, 0x00000000,
			0x00005000,,, TPMR)
})

Method (_CRS, 0, Serialized)
{
	// Find PCI resource area in MCRS
	CreateDwordField(MCRS, ^PM01._MIN, PMIN)
	CreateDwordField(MCRS, ^PM01._MAX, PMAX)
	CreateDwordField(MCRS, ^PM01._LEN, PLEN)

	// Fix up PCI memory region
	// Start with Top of Lower Usable DRAM
	Store (^MCHC.TLUD, Local0)
	Store (^MCHC.MEBA, Local1)

	// Check if ME base is equal
	If (LEqual (Local0, Local1)) {
		// Use Top Of Memory instead
		Store (^MCHC.TOM, Local0)
	}

	Store (Local0, PMIN)
	Store (Subtract(CONFIG_MMCONF_BASE_ADDRESS, 1), PMAX)
	Add(Subtract(PMAX, PMIN), 1, PLEN)

	Return (MCRS)
}

/* PCI Device Resource Consumption */
Device (PDRC)
{
	Name (_HID, EISAID("PNP0C02"))
	Name (_UID, 1)

	Name (PDRS, ResourceTemplate() {
		Memory32Fixed (ReadWrite, RCBA_BASE_ADDRESS, RCBA_BASE_SIZE)
		Memory32Fixed (ReadWrite, MCH_BASE_ADDRESS, MCH_BASE_SIZE)
		Memory32Fixed (ReadWrite, DMI_BASE_ADDRESS, DMI_BASE_SIZE)
		Memory32Fixed (ReadWrite, EP_BASE_ADDRESS, EP_BASE_SIZE)
		Memory32Fixed (ReadWrite, MCFG_BASE_ADDRESS, MCFG_BASE_SIZE)
		Memory32Fixed (ReadWrite, EDRAM_BASE_ADDRESS, EDRAM_BASE_SIZE)
		Memory32Fixed (ReadWrite, GDXC_BASE_ADDRESS, GDXC_BASE_SIZE)
	})

	// Current Resource Settings
	Method (_CRS, 0, Serialized)
	{
		Return (PDRS)
	}
}

/* PCI IRQ assignment */
#include "pci_irqs.asl"

/* Configurable TDP */
#include "ctdp.asl"

// Integrated graphics 0:2.0
#if IS_ENABLED(CONFIG_INTEL_GMA_ACPI)
#include <drivers/intel/gma/acpi/pch.asl>

/*Scope (GFX0){
	//  Define an OperationRegion to cover the IGD OpRegion layout.

	OperationRegion (IGDM, SystemMemory, ASLB, 0x2000)
    Field (IGDM, AnyAcc, NoLock, Preserve)
    {
        SIGN,   128, 
        SIZE,   32, 
        OVER,   32, 
        SVER,   256, 
        VVER,   128, 
        GVER,   128, 
        MBOX,   32, 
        DMOD,   32, 
        PCON,   32, 
        DVER,   64, 
        Offset (0x100), 
        DRDY,   32, 
        CSTS,   32, 
        CEVT,   32, 
        Offset (0x120), 
        DIDL,   32, 
        DDL2,   32, 
        DDL3,   32, 
        DDL4,   32, 
        DDL5,   32, 
        DDL6,   32, 
        DDL7,   32, 
        DDL8,   32, 
        CPDL,   32, 
        CPL2,   32, 
        CPL3,   32, 
        CPL4,   32, 
        CPL5,   32, 
        CPL6,   32, 
        CPL7,   32, 
        CPL8,   32, 
        CADL,   32, 
        CAL2,   32, 
        CAL3,   32, 
        CAL4,   32, 
        CAL5,   32, 
        CAL6,   32, 
        CAL7,   32, 
        CAL8,   32, 
        NADL,   32, 
        NDL2,   32, 
        NDL3,   32, 
        NDL4,   32, 
        NDL5,   32, 
        NDL6,   32, 
        NDL7,   32, 
        NDL8,   32, 
        ASLP,   32, 
        TIDX,   32, 
        CHPD,   32, 
        CLID,   32, 
        CDCK,   32, 
        SXSW,   32, 
        EVTS,   32, 
        CNOT,   32, 
        NRDY,   32, 
        DDL9,   32, 
        DD10,   32, 
        DD11,   32, 
        DD12,   32, 
        DD13,   32, 
        DD14,   32, 
        DD15,   32, 
        CPL9,   32, 
        CP10,   32, 
        CP11,   32, 
        CP12,   32, 
        CP13,   32, 
        CP14,   32, 
        CP15,   32, 
        Offset (0x200), 
        SCIE,   1, 
        GEFC,   4, 
        GXFC,   3, 
        GESF,   8, 
        Offset (0x204), 
        PARM,   32, 
        DSLP,   32, 
        Offset (0x300), 
        ARDY,   32, 
        ASLC,   32, 
        TCHE,   32, 
        ALSI,   32, 
        BCLP,   32, 
        PFIT,   32, 
        CBLV,   32, 
        BCLT,   320, 
        CPFM,   32, 
        EPFM,   32, 
        PLUT,   592, 
        PFMB,   32, 
        CCDV,   32, 
        PCFT,   32, 
        SROT,   32, 
        IUER,   32, 
        FDSP,   64, 
        FDSS,   32, 
        STAT,   32, 
        Offset (0x400), 
        GVD1,   49152, 
        PHED,   32, 
        BDDC,   2048
    }
}*/

#endif