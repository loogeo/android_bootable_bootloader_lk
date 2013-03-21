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

#include <debug.h>
#include <platform/iomap.h>
#include <reg.h>
#include <target.h>
#include <platform.h>
#include <uart_dm.h>
#include <mmc.h>
#include <spmi.h>
#include <board.h>
#include <smem.h>
#include <baseband.h>
#include <dev/keys.h>
#include <pm8x41.h>
#include <crypto5_wrapper.h>
#include <hsusb.h>
#include <clock.h>
#include <partition_parser.h>
#include <scm.h>
#include <platform/clock.h>

extern  bool target_use_signed_kernel(void);

static unsigned int target_id;
static uint32_t pmic_ver;

#define PMIC_ARB_CHANNEL_NUM    0
#define PMIC_ARB_OWNER_ID       0

#define WDOG_DEBUG_DISABLE_BIT  17

#define CE_INSTANCE             2
#define CE_EE                   1
#define CE_FIFO_SIZE            64
#define CE_READ_PIPE            3
#define CE_WRITE_PIPE           2
#define CE_ARRAY_SIZE           20

#ifdef SSD_ENABLE
#define SSD_CE_INSTANCE_1       1
#define SSD_PARTITION_SIZE      8192
#endif

static uint32_t mmc_sdc_base[] =
	{ MSM_SDC1_BASE, MSM_SDC2_BASE, MSM_SDC3_BASE, MSM_SDC4_BASE };

void target_early_init(void)
{
#if WITH_DEBUG_UART
	uart_dm_init(1, 0, BLSP1_UART1_BASE);
#endif
}

/* Return 1 if vol_up pressed */
static int target_volume_up()
{
	uint8_t status = 0;
	struct pm8x41_gpio gpio;

	/* CDP vol_up seems to be always grounded. So gpio status is read as 0,
	 * whether key is pressed or not.
	 * Ignore volume_up key on CDP for now.
	 */
	if (board_hardware_id() == HW_PLATFORM_SURF)
		return 0;

	/* Configure the GPIO */
	gpio.direction = PM_GPIO_DIR_IN;
	gpio.function  = 0;
	gpio.pull      = PM_GPIO_PULL_UP_30;
	gpio.vin_sel   = 2;

	pm8x41_gpio_config(5, &gpio);

	/* Get status of P_GPIO_5 */
	pm8x41_gpio_get(5, &status);

	return !status; /* active low */
}

/* Return 1 if vol_down pressed */
uint32_t target_volume_down()
{
	/* Volume down button is tied in with RESIN on MSM8974. */
	if (pmic_ver == PMIC_VERSION_V2)
		return pm8x41_resin_bark_workaround_status();
	else
		return pm8x41_resin_status();
}

static void target_keystatus()
{
	keys_init();

	if(target_volume_down())
		keys_post_event(KEY_VOLUMEDOWN, 1);

	if(target_volume_up())
		keys_post_event(KEY_VOLUMEUP, 1);
}

/* Set up params for h/w CE. */
void target_crypto_init_params()
{
	struct crypto_init_params ce_params;

	/* Set up base addresses and instance. */
	ce_params.crypto_instance  = CE_INSTANCE;
	ce_params.crypto_base      = MSM_CE2_BASE;
	ce_params.bam_base         = MSM_CE2_BAM_BASE;

	/* Set up BAM config. */
	ce_params.bam_ee           = CE_EE;
	ce_params.pipes.read_pipe  = CE_READ_PIPE;
	ce_params.pipes.write_pipe = CE_WRITE_PIPE;

	/* Assign buffer sizes. */
	ce_params.num_ce           = CE_ARRAY_SIZE;
	ce_params.read_fifo_size   = CE_FIFO_SIZE;
	ce_params.write_fifo_size  = CE_FIFO_SIZE;

	crypto_init_params(&ce_params);
}

crypto_engine_type board_ce_type(void)
{
	return CRYPTO_ENGINE_TYPE_HW;
}

void target_init(void)
{
	uint32_t base_addr;
	uint8_t slot;

	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

	/* Save PM8941 version info. */
	pmic_ver = pm8x41_get_pmic_rev();

	target_keystatus();

	if (target_use_signed_kernel())
		target_crypto_init_params();
	/* Display splash screen if enabled */
#if DISPLAY_SPLASH_SCREEN
	dprintf(INFO, "Display Init: Start\n");
	display_init();
	dprintf(INFO, "Display Init: Done\n");
#endif

	/* Trying Slot 1*/
	slot = 1;
	base_addr = mmc_sdc_base[slot - 1];
	if (mmc_boot_main(slot, base_addr))
	{

		/* Trying Slot 2 next */
		slot = 2;
		base_addr = mmc_sdc_base[slot - 1];
		if (mmc_boot_main(slot, base_addr)) {
			dprintf(CRITICAL, "mmc init failed!");
			ASSERT(0);
		}
	}
}

unsigned board_machtype(void)
{
	return target_id;
}

/* Do any target specific intialization needed before entering fastboot mode */
#ifdef SSD_ENABLE
static uint32_t  buffer[SSD_PARTITION_SIZE] __attribute__ ((aligned(32)));
static void ssd_load_keystore_from_emmc()
{
	uint64_t           ptn    = 0;
	int                index  = -1;
	uint32_t           size   = SSD_PARTITION_SIZE;
	int                ret    = -1;

	index = partition_get_index("ssd");

	ptn   = partition_get_offset(index);
	if(ptn == 0){
		dprintf(CRITICAL,"ERROR: ssd parition not found");
		return;
	}

	if(mmc_read(ptn, buffer, size)){
		dprintf(CRITICAL,"ERROR:Cannot read data\n");
		return;
	}

	ret = scm_protect_keystore((uint32_t *)&buffer[0],size);
	if(ret != 0)
		dprintf(CRITICAL,"ERROR: scm_protect_keystore Failed");
}
#endif

void target_fastboot_init(void)
{
	/* Set the BOOT_DONE flag in PM8921 */
	pm8x41_set_boot_done();

#ifdef SSD_ENABLE
	clock_ce_enable(SSD_CE_INSTANCE_1);
	ssd_load_keystore_from_emmc();
#endif
}

/* Detect the target type */
void target_detect(struct board_data *board)
{
	board->target = LINUX_MACHTYPE_UNKNOWN;
}

/* Detect the modem type */
void target_baseband_detect(struct board_data *board)
{
	uint32_t platform;
	uint32_t platform_subtype;

	platform = board->platform;
	platform_subtype = board->platform_subtype;

	/*
	 * Look for platform subtype if present, else
	 * check for platform type to decide on the
	 * baseband type
	 */
	switch(platform_subtype) {
	case HW_PLATFORM_SUBTYPE_UNKNOWN:
		break;
	default:
		dprintf(CRITICAL, "Platform Subtype : %u is not supported\n",platform_subtype);
		ASSERT(0);
	};

	switch(platform) {
	case MSM8974:
		board->baseband = BASEBAND_MSM;
		break;
	case APQ8074:
		board->baseband = BASEBAND_APQ;
		break;
	default:
		dprintf(CRITICAL, "Platform type: %u is not supported\n",platform);
		ASSERT(0);
	};
}

void target_serialno(unsigned char *buf)
{
	unsigned int serialno;
	if (target_is_emmc_boot()) {
		serialno = mmc_get_psn();
		snprintf((char *)buf, 13, "%x", serialno);
	}
}

unsigned check_reboot_mode(void)
{
	uint32_t restart_reason = 0;
	uint32_t soc_ver = 0;
	uint32_t restart_reason_addr;

	soc_ver = board_soc_version();

	if (soc_ver >= BOARD_SOC_VERSION2)
		restart_reason_addr = RESTART_REASON_ADDR_V2;
	else
		restart_reason_addr = RESTART_REASON_ADDR;

	/* Read reboot reason and scrub it */
	restart_reason = readl(restart_reason_addr);
	writel(0x00, restart_reason_addr);

	return restart_reason;
}

void reboot_device(unsigned reboot_reason)
{
	uint32_t soc_ver = 0;

	soc_ver = board_soc_version();

	/* Write the reboot reason */
	if (soc_ver >= BOARD_SOC_VERSION2)
		writel(reboot_reason, RESTART_REASON_ADDR_V2);
	else
		writel(reboot_reason, RESTART_REASON_ADDR);

	/* Configure PMIC for warm reset */
	if (pmic_ver == PMIC_VERSION_V2)
		pm8x41_v2_reset_configure(PON_PSHOLD_WARM_RESET);
	else
		pm8x41_reset_configure(PON_PSHOLD_WARM_RESET);

	/* Disable Watchdog Debug.
	 * Required becuase of a H/W bug which causes the system to
	 * reset partially even for non watchdog resets.
	 */
	writel(readl(GCC_WDOG_DEBUG) & ~(1 << WDOG_DEBUG_DISABLE_BIT), GCC_WDOG_DEBUG);

	dsb();

	/* Wait until the write takes effect. */
	while(readl(GCC_WDOG_DEBUG) & (1 << WDOG_DEBUG_DISABLE_BIT));

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "Rebooting failed\n");
}

/* Do target specific usb initialization */
void target_usb_init(void)
{
	/* Enable secondary USB PHY on DragonBoard8074 */
	if (board_hardware_id() == HW_PLATFORM_DRAGON) {
		/* Route ChipIDea to use secondary USB HS port2 */
		writel_relaxed(1, USB2_PHY_SEL);

		/* Enable access to secondary PHY by clamping the low
		* voltage interface between DVDD of the PHY and Vddcx
		* (set bit16 (USB2_PHY_HS2_DIG_CLAMP_N_2) = 1) */
		writel_relaxed(readl_relaxed(USB_OTG_HS_PHY_SEC_CTRL)
				| 0x00010000, USB_OTG_HS_PHY_SEC_CTRL);

		/* Perform power-on-reset of the PHY.
		*  Delay values are arbitrary */
		writel_relaxed(readl_relaxed(USB_OTG_HS_PHY_CTRL)|1,
				USB_OTG_HS_PHY_CTRL);
		thread_sleep(10);
		writel_relaxed(readl_relaxed(USB_OTG_HS_PHY_CTRL) & 0xFFFFFFFE,
				USB_OTG_HS_PHY_CTRL);
		thread_sleep(10);

		/* Enable HSUSB PHY port for ULPI interface,
		* then configure related parameters within the PHY */
		writel_relaxed(((readl_relaxed(USB_PORTSC) & 0xC0000000)
				| 0x8c000004), USB_PORTSC);
	}
}

/* Returns 1 if target supports continuous splash screen. */
int target_cont_splash_screen()
{
	switch(board_hardware_id())
	{
		case HW_PLATFORM_SURF:
		case HW_PLATFORM_MTP:
		case HW_PLATFORM_FLUID:
			dprintf(SPEW, "Target_cont_splash=1\n");
			return 1;
			break;
		default:
			dprintf(SPEW, "Target_cont_splash=0\n");
			return 0;
	}
}

unsigned target_pause_for_battery_charge(void)
{
	uint8_t pon_reason = pm8x41_get_pon_reason();

        /* This function will always return 0 to facilitate
         * automated testing/reboot with usb connected.
         * uncomment if this feature is needed */
	/* if ((pon_reason == USB_CHG) || (pon_reason == DC_CHG))
		return 1;*/

	return 0;
}

void target_usb_stop(void)
{
#ifdef SSD_ENABLE
	clock_ce_disable(SSD_CE_INSTANCE_1);
#endif
}

void shutdown_device()
{
	dprintf(CRITICAL, "Going down for shutdown.\n");

	/* Configure PMIC for shutdown. */
	if (pmic_ver == PMIC_VERSION_V2)
		pm8x41_v2_reset_configure(PON_PSHOLD_SHUTDOWN);
	else
		pm8x41_reset_configure(PON_PSHOLD_SHUTDOWN);

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "Shutdown failed\n");

}
