/*
 * Copyright (c) 2012 The Linux Foundation. All rights reserved.
 * Source : APQ8064 LK boot
 *
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <clock.h>
#include <gpio.h>
#include <gsbi.h>
#include <console/uart.h>
#include <ipq_uart.h>
#include <stdint.h>
#include <stdlib.h>

#define FIFO_DATA_SIZE	4

typedef struct {
	unsigned uart_dm_base;
	unsigned uart_gsbi_base;
	unsigned uart_gsbi;
	uart_clk_mnd_t mnd_value;
	gpio_func_data_t dbg_uart_gpio[NO_OF_DBG_UART_GPIOS];
} uart_params_t;

/*
 * All constants lifted from u-boot's
 * board/qcom/ipq806x_cdp/ipq806x_board_param.h
 */
static const uart_params_t uart_board_param = {
	.uart_dm_base = UART4_DM_BASE,
	.uart_gsbi_base = UART_GSBI4_BASE,
	.uart_gsbi = GSBI_4,
	.mnd_value = { 12, 625, 313 },
                .dbg_uart_gpio = {
                        {
                                .gpio = 10,
                                .func = 1,
                                .dir = GPIO_OUTPUT,
                                .pull = GPIO_NO_PULL,
                                .drvstr = GPIO_12MA,
                                .enable = GPIO_DISABLE
                        },
                        {
                                .gpio = 11,
                                .func = 1,
                                .dir = GPIO_INPUT,
                                .pull = GPIO_NO_PULL,
                                .drvstr = GPIO_12MA,
                                .enable = GPIO_DISABLE
                        },
                }
};

static unsigned int msm_boot_uart_dm_init(unsigned int  uart_dm_base);

/* Received data is valid or not */
static int valid_data = 0;

/* Received data */
static unsigned int word = 0;

/**
 * msm_boot_uart_dm_init_rx_transfer - Init Rx transfer
 * @uart_dm_base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_init_rx_transfer(unsigned int uart_dm_base)
{
	/* Reset receiver */
	writel_i(MSM_BOOT_UART_DM_CMD_RESET_RX,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	/* Enable receiver */
	writel_i(MSM_BOOT_UART_DM_CR_RX_ENABLE,
		MSM_BOOT_UART_DM_CR(uart_dm_base));
	writel_i(MSM_BOOT_UART_DM_DMRX_DEF_VALUE,
		MSM_BOOT_UART_DM_DMRX(uart_dm_base));

	/* Clear stale event */
	writel_i(MSM_BOOT_UART_DM_CMD_RES_STALE_INT,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	/* Enable stale event */
	writel_i(MSM_BOOT_UART_DM_GCMD_ENA_STALE_EVT,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

/**
 * msm_boot_uart_dm_read - reads a word from the RX FIFO.
 * @data: location where the read data is stored
 * @count: no of valid data in the FIFO
 * @wait: indicates blocking call or not blocking call
 *
 * Reads a word from the RX FIFO. If no data is available blocks if
 * @wait is true, else returns %MSM_BOOT_UART_DM_E_RX_NOT_READY.
 */
 #if 0 /* Not used yet */
static unsigned int
msm_boot_uart_dm_read(unsigned int *data, int *count, int wait)
{
	static int total_rx_data = 0;
	static int rx_data_read = 0;
	unsigned int  base = 0;
	uint32_t status_reg;

	base = uart_board_param.uart_dm_base;

	if (data == NULL)
		return MSM_BOOT_UART_DM_E_INVAL;

	status_reg = readl_i(MSM_BOOT_UART_DM_MISR(base));

	/* Check for DM_RXSTALE for RX transfer to finish */
	while (!(status_reg & MSM_BOOT_UART_DM_RXSTALE)) {
		status_reg = readl_i(MSM_BOOT_UART_DM_MISR(base));
		if (!wait)
			return MSM_BOOT_UART_DM_E_RX_NOT_READY;
	}

	/* Check for Overrun error. We'll just reset Error Status */
	if (readl_i(MSM_BOOT_UART_DM_SR(base)) &
			MSM_BOOT_UART_DM_SR_UART_OVERRUN) {
		writel_i(MSM_BOOT_UART_DM_CMD_RESET_ERR_STAT,
			MSM_BOOT_UART_DM_CR(base));
		total_rx_data = rx_data_read = 0;
		msm_boot_uart_dm_init(base);
		return MSM_BOOT_UART_DM_E_RX_NOT_READY;
	}

	/* Read UART_DM_RX_TOTAL_SNAP for actual number of bytes received */
	if (total_rx_data == 0)
		total_rx_data =  readl_i(MSM_BOOT_UART_DM_RX_TOTAL_SNAP(base));

	/* Data available in FIFO; read a word. */
	*data = readl_i(MSM_BOOT_UART_DM_RF(base, 0));

	/* WAR for http://prism/CR/548280 */
	if (*data == 0) {
		return MSM_BOOT_UART_DM_E_RX_NOT_READY;
	}

	/* increment the total count of chars we've read so far */
	rx_data_read += FIFO_DATA_SIZE;

	/* actual count of valid data in word */
	*count = ((total_rx_data < rx_data_read) ?
			(FIFO_DATA_SIZE - (rx_data_read - total_rx_data)) :
			FIFO_DATA_SIZE);

	/* If there are still data left in FIFO we'll read them before
	 * initializing RX Transfer again
	 */
	if (rx_data_read < total_rx_data)
		return MSM_BOOT_UART_DM_E_SUCCESS;

	msm_boot_uart_dm_init_rx_transfer(base);
	total_rx_data = rx_data_read = 0;

	return MSM_BOOT_UART_DM_E_SUCCESS;
}
#endif

void uart_tx_byte(int idx, unsigned char data)
{
	unsigned int base = uart_board_param.uart_dm_base;

	/* Wait until transmit FIFO is empty. */
	while (!(readl_i(MSM_BOOT_UART_DM_SR(base)) &
		MSM_BOOT_UART_DM_SR_TXEMT))
			udelay(1);

	/*
	 * TX FIFO is ready to accept new character(s). First write number of
	 * characters to be transmitted.
	 */
	writel_i(1, MSM_BOOT_UART_DM_NO_CHARS_FOR_TX(base));

	/* And now write the character(s) */
	writel_i(data, MSM_BOOT_UART_DM_TF(base, 0));
}

/*
 * msm_boot_uart_dm_reset - resets UART controller
 * @base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_reset(unsigned int base)
{
	writel_i(MSM_BOOT_UART_DM_CMD_RESET_RX, MSM_BOOT_UART_DM_CR(base));
	writel_i(MSM_BOOT_UART_DM_CMD_RESET_TX, MSM_BOOT_UART_DM_CR(base));
	writel_i(MSM_BOOT_UART_DM_CMD_RESET_ERR_STAT,
		 MSM_BOOT_UART_DM_CR(base));
	writel_i(MSM_BOOT_UART_DM_CMD_RES_TX_ERR, MSM_BOOT_UART_DM_CR(base));
	writel_i(MSM_BOOT_UART_DM_CMD_RES_STALE_INT, MSM_BOOT_UART_DM_CR(base));

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

/*
 * msm_boot_uart_dm_init - initilaizes UART controller
 * @uart_dm_base: UART controller base address
 */
static unsigned int msm_boot_uart_dm_init(unsigned int  uart_dm_base)
{
	/* Configure UART mode registers MR1 and MR2 */
	/* Hardware flow control isn't supported */
	writel_i(0x0, MSM_BOOT_UART_DM_MR1(uart_dm_base));

	/* 8-N-1 configuration: 8 data bits - No parity - 1 stop bit */
	writel_i(MSM_BOOT_UART_DM_8_N_1_MODE,
		 MSM_BOOT_UART_DM_MR2(uart_dm_base));

	/* Configure Interrupt Mask register IMR */
	writel_i(MSM_BOOT_UART_DM_IMR_ENABLED,
		 MSM_BOOT_UART_DM_IMR(uart_dm_base));

	/*
	 * Configure Tx and Rx watermarks configuration registers
	 * TX watermark value is set to 0 - interrupt is generated when
	 * FIFO level is less than or equal to 0
	 */
	writel_i(MSM_BOOT_UART_DM_TFW_VALUE,
		 MSM_BOOT_UART_DM_TFWR(uart_dm_base));

	/* RX watermark value */
	writel_i(MSM_BOOT_UART_DM_RFW_VALUE,
		 MSM_BOOT_UART_DM_RFWR(uart_dm_base));

	/* Configure Interrupt Programming Register */
	/* Set initial Stale timeout value */
	writel_i(MSM_BOOT_UART_DM_STALE_TIMEOUT_LSB,
		MSM_BOOT_UART_DM_IPR(uart_dm_base));

	/* Configure IRDA if required */
	/* Disabling IRDA mode */
	writel_i(0x0, MSM_BOOT_UART_DM_IRDA(uart_dm_base));

	/* Configure hunt character value in HCR register */
	/* Keep it in reset state */
	writel_i(0x0, MSM_BOOT_UART_DM_HCR(uart_dm_base));

	/*
	 * Configure Rx FIFO base address
	 * Both TX/RX shares same SRAM and default is half-n-half.
	 * Sticking with default value now.
	 * As such RAM size is (2^RAM_ADDR_WIDTH, 32-bit entries).
	 * We have found RAM_ADDR_WIDTH = 0x7f
	 */

	/* Issue soft reset command */
	msm_boot_uart_dm_reset(uart_dm_base);

	/* Enable/Disable Rx/Tx DM interfaces */
	/* Data Mover not currently utilized. */
	writel_i(0x0, MSM_BOOT_UART_DM_DMEN(uart_dm_base));

	/* Enable transmitter */
	writel_i(MSM_BOOT_UART_DM_CR_TX_ENABLE,
		MSM_BOOT_UART_DM_CR(uart_dm_base));

	/* Initialize Receive Path */
	msm_boot_uart_dm_init_rx_transfer(uart_dm_base);

	return 0;
}

/**
 * uart_init - initializes UART
 *
 * Initializes clocks, GPIO and UART controller.
 */
void uart_init(int idx)
{
	/* Note int idx isn't used in this driver. */
	unsigned int dm_base, gsbi_base;

	dm_base = uart_board_param.uart_dm_base;
	gsbi_base = uart_board_param.uart_gsbi_base;
	ipq_configure_gpio(uart_board_param.dbg_uart_gpio,
			   NO_OF_DBG_UART_GPIOS);

	/* Configure the uart clock */
        uart_clock_config(uart_board_param.uart_gsbi,
		uart_board_param.mnd_value.m_value,
		uart_board_param.mnd_value.n_value,
		uart_board_param.mnd_value.d_value,
		0);

	writel_i(GSBI_PROTOCOL_CODE_I2C_UART <<
		GSBI_CTRL_REG_PROTOCOL_CODE_S,
		GSBI_CTRL_REG(gsbi_base));
        writel_i(UART_DM_CLK_RX_TX_BIT_RATE, MSM_BOOT_UART_DM_CSR(dm_base));

	/* Intialize UART_DM */
	msm_boot_uart_dm_init(dm_base);
}

#if 0 /* Not used yet */
uint32_t uartmem_getbaseaddr(void)
{
	return uart_board_param.uart_dm_base;
}
#endif

/**
 * uart_tx_flush - transmits a string of data
 * @s: string to transmit
 */
void uart_tx_flush(int idx)
{
	unsigned int base = uart_board_param.uart_dm_base;

        while (!(readl_i(MSM_BOOT_UART_DM_SR(base)) &
		 MSM_BOOT_UART_DM_SR_TXEMT))
		;
}

/**
 * uart_can_rx_byte - checks if data available for reading
 *
 * Returns 1 if data available, 0 otherwise
 */
 #if 0 /* Not used yet */
int uart_can_rx_byte(void)
{
	/* Return if data is already read */
	if (valid_data)
		return 1;

	/* Read data from the FIFO */
	if (msm_boot_uart_dm_read(&word, &valid_data, 0) !=
	    MSM_BOOT_UART_DM_E_SUCCESS)
		return 0;

	return 1;
}
#endif

/**
 * ipq806x_serial_getc - reads a character
 *
 * Returns the character read from serial port.
 */
uint8_t uart_rx_byte(int idx)
{
	uint8_t byte;

#if 0 /* Not used yet */
	while (!uart_can_rx_byte()) {
		/* wait for incoming data */
	}
#endif
	byte = (uint8_t)(word & 0xff);
	word = word >> 8;
	valid_data--;

	return byte;
}
#ifndef __PRE_RAM__
/* TODO: Implement fuction */
void uart_fill_lb(void *data)
{
}
#endif
