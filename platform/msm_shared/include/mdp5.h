/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#ifndef _PLATFORM_MSM_SHARED_MDP_5_H_
#define _PLATFORM_MSM_SHARED_MDP_5_H_

#include <msm_panel.h>

#define MDP_VP_0_RGB_0_SSPP_SRC0_ADDR           REG_MDP(0x1E14)
#define MDP_VP_0_RGB_0_SSPP_SRC_YSTRIDE         REG_MDP(0x1E24)
#define MDP_VP_0_RGB_0_SSPP_SRC_IMG_SIZE        REG_MDP(0x1E04)
#define MDP_VP_0_RGB_0_SSPP_SRC_SIZE            REG_MDP(0x1E00)
#define MDP_VP_0_RGB_0_SSPP_SRC_OUT_SIZE        REG_MDP(0x1E0C)
#define MDP_VP_0_RGB_0_SSPP_SRC_XY              REG_MDP(0x1E08)
#define MDP_VP_0_RGB_0_SSPP_OUT_XY              REG_MDP(0x1E10)
#define MDP_VP_0_RGB_0_SSPP_SRC_FORMAT          REG_MDP(0x1E30)
#define MDP_VP_0_RGB_0_SSPP_SRC_UNPACK_PATTERN  REG_MDP(0x1E34)
#define MDP_VP_0_RGB_0_SSPP_SRC_OP_MODE         REG_MDP(0x1E38)

#define MDP_VP_0_LAYER_0_OUT_SIZE               REG_MDP(0x3204)
#define MDP_VP_0_LAYER_0_OP_MODE                REG_MDP(0x3200)
#define MDP_VP_0_LAYER_0_BLEND_OP               REG_MDP(0x3220)
#define MDP_VP_0_LAYER_0_BLEND0_FG_ALPHA        REG_MDP(0x3224)
#define MDP_VP_0_LAYER_1_BLEND_OP               REG_MDP(0x3250)
#define MDP_VP_0_LAYER_1_BLEND0_FG_ALPHA        REG_MDP(0x3254)
#define MDP_VP_0_LAYER_2_BLEND_OP               REG_MDP(0x3280)
#define MDP_VP_0_LAYER_2_BLEND0_FG_ALPHA        REG_MDP(0x3284)
#define MDP_VP_0_LAYER_3_BLEND_OP               REG_MDP(0x32B0)
#define MDP_VP_0_LAYER_3_BLEND0_FG_ALPHA        REG_MDP(0x32B4)


#define MDSS_MDP_HW_REV_100                     0x10000000
#define MDSS_MDP_HW_REV_102                     0x10020000

#define MDP_HW_REV                              REG_MDP(0x0100)
#define MDP_INTR_EN                             REG_MDP(0x0110)
#define MDP_INTR_CLEAR                          REG_MDP(0x0118)
#define MDP_HIST_INTR_EN                        REG_MDP(0x011C)

#define MDP_DISP_INTF_SEL                       REG_MDP(0x0104)
#define MDP_VIDEO_INTF_UNDERFLOW_CTL            REG_MDP(0x03E0)
#define MDP_UPPER_NEW_ROI_PRIOR_RO_START        REG_MDP(0x02EC)
#define MDP_LOWER_NEW_ROI_PRIOR_TO_START        REG_MDP(0x04F8)

#define MDP_INTF_1_TIMING_ENGINE_EN             REG_MDP(0x12700)

#define MDP_CTL_0_LAYER_0                       REG_MDP(0x600)
#define MDP_CTL_0_TOP                           REG_MDP(0x614)
#define MDP_CTL_0_FLUSH                         REG_MDP(0x618)

#define MDP_INTF_1_HSYNC_CTL                    REG_MDP(0x12708)
#define MDP_INTF_1_VSYNC_PERIOD_F0              REG_MDP(0x1270C)
#define MDP_INTF_1_VSYNC_PERIOD_F1              REG_MDP(0x12710)
#define MDP_INTF_1_VSYNC_PULSE_WIDTH_F0         REG_MDP(0x12714)
#define MDP_INTF_1_VSYNC_PULSE_WIDTH_F1         REG_MDP(0x12718)
#define MDP_INTF_1_DISPLAY_HCTL                 REG_MDP(0x1273C)
#define MDP_INTF_1_DISPLAY_V_START_F0           REG_MDP(0x1271C)
#define MDP_INTF_1_DISPLAY_V_START_F1           REG_MDP(0x12720)
#define MDP_INTF_1_DISPLAY_V_END_F0             REG_MDP(0x12724)
#define MDP_INTF_1_DISPLAY_V_END_F1             REG_MDP(0x12728)
#define MDP_INTF_1_ACTIVE_HCTL                  REG_MDP(0x12740)
#define MDP_INTF_1_ACTIVE_V_START_F0            REG_MDP(0x1272C)
#define MDP_INTF_1_ACTIVE_V_START_F1            REG_MDP(0x12730)
#define MDP_INTF_1_ACTIVE_V_END_F0              REG_MDP(0x12734)
#define MDP_INTF_1_ACTIVE_V_END_F1              REG_MDP(0x12738)
#define MDP_INTF_1_UNDERFFLOW_COLOR             REG_MDP(0x12748)
#define MDP_INTF_1_PANEL_FORMAT                 REG_MDP(0x12790)

#define MDP_CLK_CTRL0                           REG_MDP(0x03AC)
#define MDP_CLK_CTRL1                           REG_MDP(0x03B4)
#define MDP_CLK_CTRL2                           REG_MDP(0x03BC)
#define MDP_CLK_CTRL3                           REG_MDP(0x04A8)
#define MDP_CLK_CTRL4                           REG_MDP(0x04B0)

#define MMSS_MDP_SMP_ALLOC_W_0                  REG_MDP(0x0180)
#define MMSS_MDP_SMP_ALLOC_W_1                  REG_MDP(0x0184)
#define MMSS_MDP_SMP_ALLOC_R_0                  REG_MDP(0x0230)
#define MMSS_MDP_SMP_ALLOC_R_1                  REG_MDP(0x0234)

#define VBIF_VBIF_DDR_FORCE_CLK_ON              REG_MDP(0x24004)
#define VBIF_VBIF_DDR_OUT_MAX_BURST             REG_MDP(0x240D8)
#define VBIF_VBIF_DDR_ARB_CTRL                  REG_MDP(0x240F0)
#define VBIF_VBIF_DDR_RND_RBN_QOS_ARB           REG_MDP(0x24124)
#define VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF0        REG_MDP(0x24160)
#define VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF1        REG_MDP(0x24164)
#define VBIF_VBIF_DDR_OUT_AOOO_AXI_EN           REG_MDP(0x24178)
#define VBIF_VBIF_DDR_OUT_AX_AOOO               REG_MDP(0x2417C)

void mdp_set_revision(int rev);
int mdp_get_revision();
int mdp_dsi_video_config(struct msm_panel_info *pinfo, struct fbcon_config *fb);
int mipi_dsi_cmd_config(struct fbcon_config mipi_fb_cfg,
			unsigned short num_of_lanes);
int mdp_dsi_video_on(void);
int mdp_dma_on(void);
void mdp_disable(void);

#endif
