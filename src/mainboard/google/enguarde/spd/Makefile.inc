##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 Google Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

SPD_BIN = $(obj)/spd.bin

# Order matters for SPD sources. The following indicies
# define the SPD data to use.
# 0b000 - 4GiB total - 2 x 2GiB Micron MT41K256M16HA-125:E 1600MHz
# 0b001 - 4GiB total - 2 x 2GiB Hynix  H5TC4G63AFR-PBA 1600MHz
# 0b010 - 2GiB total - 2 x 1GiB Micron MT41K128M16JT-125:K 1600MHz
# 0b011 - 2GiB total - 2 x 1GiB Hynix  H5TC2G63FFR-PBA 1600MHz
# 0b100 - 2GiB total - 1 x 2GiB Micron MT41K256M16HA-125:E 1600MHz
# 0b101 - 2GiB total - 2 x 1GiB Samsung K4B2G1646Q-BYK0 1600MHz
# 0b110 - 4GiB total - 2 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
# 0b111 - 2GiB total - 1 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
SPD_SOURCES = micron_2GiB_dimm_MT41K256M16HA-125
SPD_SOURCES += hynix_2GiB_dimm_H5TC4G63AFR-PBA
SPD_SOURCES += micron_1GiB_dimm_MT41K128M16JT-125
SPD_SOURCES += hynix_1GiB_dimm_H5TC2G63FFR-PBA
SPD_SOURCES += micron_2GiB_dimm_MT41K256M16HA-125
SPD_SOURCES += samsung_1GiB_dimm_K4B2G1646Q-BYK0
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0
SPD_SOURCES += samsung_2GiB_dimm_K4B4G1646Q-HYK0

SPD_DEPS := $(foreach f, $(SPD_SOURCES), src/mainboard/$(MAINBOARDDIR)/spd/$(f).spd.hex)

# Include spd ROM data
$(SPD_BIN): $(SPD_DEPS)
	for f in $+; \
	  do for c in $$(cat $$f | grep -v ^#); \
	    do printf $$(printf '\%o' 0x$$c); \
	  done; \
	done > $@

cbfs-files-y += spd.bin
spd.bin-file := $(SPD_BIN)
spd.bin-type := spd
