/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <assert.h>
#include <debug.h>
#include <reg.h>
#include <platform/timer.h>
#include <platform/iomap.h>
#include <mmc.h>
#include <clock.h>
#include <platform/clock.h>
#include <blsp_qup.h>

void hsusb_clock_init(void)
{
	int ret;
	struct clk *iclk, *cclk;

	ret = clk_get_set_enable("usb_iface_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb_iface_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("usb_core_clk", 75000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set usb_core_clk ret = %d\n", ret);
		ASSERT(0);
	}

	mdelay(20);

	iclk = clk_get("usb_iface_clk");
	cclk = clk_get("usb_core_clk");

	/* Disable USB all clock init */
	writel(0, USB_BOOT_CLOCK_CTL);

	clk_disable(iclk);
	clk_disable(cclk);

	mdelay(20);

	/* Start the block reset for usb */
	writel(1, USB_HS_BCR);

	mdelay(20);

	/* Take usb block out of reset */
	writel(0, USB_HS_BCR);

	mdelay(20);

	ret = clk_enable(iclk);

	if(ret)
    {
        dprintf(CRITICAL, "failed to set usb_iface_clk after async ret = %d\n", ret);
        ASSERT(0);
    }

	ret = clk_enable(cclk);

	if(ret)
    {
        dprintf(CRITICAL, "failed to set usb_iface_clk after async ret = %d\n", ret);
        ASSERT(0);
    }

}

void clock_init_mmc(uint32_t interface)
{
	char clk_name[64];
	int ret;

	snprintf(clk_name, 64, "sdc%u_iface_clk", interface);

	/* enable interface clock */
	ret = clk_get_set_enable(clk_name, 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set sdc1_iface_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

/* Configure MMC clock */
void clock_config_mmc(uint32_t interface, uint32_t freq)
{
	int ret;
	uint32_t reg;
	char clk_name[64];

	snprintf(clk_name, 64, "sdc%u_core_clk", interface);

	/* Disalbe MCI_CLK before changing the sdcc clock */
	mmc_boot_mci_clk_disable();

	if(freq == MMC_CLK_400KHZ)
	{
		ret = clk_get_set_enable(clk_name, 400000, 1);
	}
	else if(freq == MMC_CLK_50MHZ)
	{
		ret = clk_get_set_enable(clk_name, 50000000, 1);
	}
	else
	{
		dprintf(CRITICAL, "sdc frequency (%d) is not supported\n", freq);
		ASSERT(0);
	}


	if(ret)
	{
		dprintf(CRITICAL, "failed to set sdc1_core_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Enalbe MCI clock */
	mmc_boot_mci_clk_enable();
}

/* Configure UART clock based on the UART block id*/
void clock_config_uart_dm(uint8_t id)
{
	int ret;


    ret = clk_get_set_enable("uart2_iface_clk", 0, 1);
    if(ret)
	{
		dprintf(CRITICAL, "failed to set uart2_iface_clk ret = %d\n", ret);
		ASSERT(0);
	}

    ret = clk_get_set_enable("uart2_core_clk", 7372800, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set uart1_core_clk ret = %d\n", ret);
		ASSERT(0);
	}
}

/* Function to asynchronously reset CE.
 * Function assumes that all the CE clocks are off.
 */
static void ce_async_reset(uint8_t instance)
{
	if (instance == 1)
	{
		/* TODO: Add support for instance 1. */
		dprintf(CRITICAL, "CE instance not supported instance = %d", instance);
		ASSERT(0);
	}
	else if (instance == 2)
	{
		/* Start the block reset for CE */
		writel(1, GCC_CE2_BCR);

		udelay(2);

		/* Take CE block out of reset */
		writel(0, GCC_CE2_BCR);

		udelay(2);
	}
	else
	{
		dprintf(CRITICAL, "CE instance not supported instance = %d", instance);
		ASSERT(0);
	}
}

void clock_ce_enable(uint8_t instance)
{
	int ret;
	char clk_name[64];

	snprintf(clk_name, 64, "ce%u_src_clk", instance);
	ret = clk_get_set_enable(clk_name, 100000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set ce_src_clk ret = %d\n", ret);
		ASSERT(0);
	}

	snprintf(clk_name, 64, "ce%u_core_clk", instance);
    ret = clk_get_set_enable(clk_name, 0, 1);
    if(ret)
	{
		dprintf(CRITICAL, "failed to set ce_core_clk ret = %d\n", ret);
		ASSERT(0);
	}

	snprintf(clk_name, 64, "ce%u_ahb_clk", instance);
    ret = clk_get_set_enable(clk_name, 0, 1);
    if(ret)
	{
		dprintf(CRITICAL, "failed to set ce_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}

	snprintf(clk_name, 64, "ce%u_axi_clk", instance);
    ret = clk_get_set_enable(clk_name, 0, 1);
    if(ret)
	{
		dprintf(CRITICAL, "failed to set ce_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Wait for 48 * #pipes cycles.
	 * This is necessary as immediately after an access control reset (boot up)
	 * or a debug re-enable, the Crypto core sequentially clears its internal
	 * pipe key storage memory. If pipe key initialization writes are attempted
	 * during this time, they may be overwritten by the internal clearing logic.
	 */
	udelay(1);
}

void clock_ce_disable(uint8_t instance)
{
	struct clk *ahb_clk;
	struct clk *cclk;
	struct clk *axi_clk;
	struct clk *src_clk;
	char clk_name[64];

	snprintf(clk_name, 64, "ce%u_src_clk", instance);
	src_clk = clk_get(clk_name);

	snprintf(clk_name, 64, "ce%u_ahb_clk", instance);
	ahb_clk = clk_get(clk_name);

	snprintf(clk_name, 64, "ce%u_axi_clk", instance);
	axi_clk = clk_get(clk_name);

	snprintf(clk_name, 64, "ce%u_core_clk", instance);
	cclk    = clk_get(clk_name);

	clk_disable(ahb_clk);
	clk_disable(axi_clk);
	clk_disable(cclk);
	clk_disable(src_clk);

	/* Some delay for the clocks to stabalize. */
	udelay(1);
}

void clock_config_ce(uint8_t instance)
{
	/* Need to enable the clock before disabling since the clk_disable()
	 * has a check to default to nop when the clk_enable() is not called
	 * on that particular clock.
	 */
	clock_ce_enable(instance);

	clock_ce_disable(instance);

	ce_async_reset(instance);

	clock_ce_enable(instance);

}

void clock_config_blsp_i2c(uint8_t blsp_id, uint8_t qup_id)
{
	uint8_t ret = 0;
	char clk_name[64];

	struct clk *qup_clk;

	snprintf(clk_name, 64, "blsp%u_ahb_clk", blsp_id);

	ret = clk_get_set_enable(clk_name, 0 , 1);

	if (ret) {
		dprintf(CRITICAL, "Failed to enable %s clock\n", clk_name);
		return;
	}

	snprintf(clk_name, 64, "blsp%u_qup%u_i2c_apps_clk", blsp_id,
							(qup_id + 1));

	qup_clk = clk_get(clk_name);

	if (!qup_clk) {
		dprintf(CRITICAL, "Failed to get %s\n", clk_name);
		return;
	}

	ret = clk_enable(qup_clk);

	if (ret) {
		dprintf(CRITICAL, "Failed to enable %s\n", clk_name);
		return;
	}
}

void mdp_gdsc_ctrl(uint8_t enable)
{
	uint32_t reg = 0;
	reg = readl(MDP_GDSCR);
	if (enable) {
		if (reg & 0x1)
			writel((reg & ~0x1), MDP_GDSCR);

		while(readl(MDP_GDSCR) & ((GDSC_POWER_ON_BIT) | (GDSC_POWER_ON_STATUS_BIT)));
	} else
		ASSERT(1);
}

/* Configure MDP clock */
void mdp_clock_init(void)
{
	int ret;

	/* Set MDP clock to 200MHz */
	ret = clk_get_set_enable("mdp_ahb_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_ahb_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdss_mdp_clk_src", 75000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_clk_src ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdss_mdp_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdp_clk ret = %d\n", ret);
		ASSERT(0);
	}

	ret = clk_get_set_enable("mdss_mdp_lut_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set lut_mdp clk ret = %d\n", ret);
		ASSERT(0);
	}
}

/* Initialize all clocks needed by Display */
void mmss_clock_init(void)
{
	int ret;

	/* Configure Byte clock */
	writel(0x100, DSI_BYTE0_CFG_RCGR);
	writel(0x1, DSI_BYTE0_CMD_RCGR);
	writel(0x1, DSI_BYTE0_CBCR);

	/* Configure ESC clock */
	ret = clk_get_set_enable("mdss_esc0_clk", 0, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set esc0_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Configure MMSSNOC AXI clock */
	ret = clk_get_set_enable("mmss_mmssnoc_axi_clk", 100000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mmssnoc_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Configure MMSSNOC AXI clock */
	ret = clk_get_set_enable("mmss_s0_axi_clk", 100000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mmss_s0_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Configure AXI clock */
	ret = clk_get_set_enable("mdss_axi_clk", 100000000, 1);
	if(ret)
	{
		dprintf(CRITICAL, "failed to set mdss_axi_clk ret = %d\n", ret);
		ASSERT(0);
	}

	/* Configure Pixel clock */
	writel(0x102, DSI_PIXEL0_CFG_RCGR);
	writel(0x1, DSI_PIXEL0_CMD_RCGR);
	writel(0x1, DSI_PIXEL0_CBCR);
}
