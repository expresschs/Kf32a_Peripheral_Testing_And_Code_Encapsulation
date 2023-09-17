#include <string.h>
#include "common.h"
#include "mt9811_reg.h"

static const uint16_t crc15Table[256] = {0x0,0xc599, 0xceab, 0xb32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,  // precomputed CRC15 Table
                                0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
                                0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
                                0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
                                0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
                                0x2544, 0x2be, 0xc727, 0xcc15, 0x98c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
                                0x3d6e, 0xf8f7,0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2, 0xc25b, 0xc969, 0xcf0, 0xdf0d,
                                0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
                                0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
                                0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
                                0x4a88, 0x8f11, 0x57c, 0xc0e5, 0xcbd7, 0xe4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
                                0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
                                0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
                                0x85e9, 0xf84, 0xca1d, 0xc12f, 0x4b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528,
                                0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,
                                0x2ac0, 0xd3a, 0xc8a3, 0xc391, 0x608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01,
                                0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9,
                                0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,
                                0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,
                                0x2fbc, 0x846, 0xcddf, 0xc6ed, 0x374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0xaf8, 0xcf61, 0xc453,
                                0x1ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,
                                0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,
                                0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095 };

/* Calculates  and returns the CRC15 */
static uint16_t pec15_calc(uint8_t len, uint8_t *data)
{
    uint16_t addr = 0;
    uint16_t remainder = 16;//initialize the PEC

    for (uint8_t i = 0; i < len; i++) {
        addr = ((remainder >> 7) ^ data[i]) & 0xff;//calculate PEC table address
        remainder = (remainder << 8) ^ crc15Table[addr];
    }

    return (remainder * 2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}


#if 0
void mt9811_wakeup_sleep(mt9811_ctx *ctx)
{
    uint8_t ic = 0;

    for (ic = 0; ic < ctx->ic_num; ic++) {
        ctx->wakeup_pin(0);
        ctx->delay_us(300);
        ctx->wakeup_pin(1);
        ctx->delay_us(10);
        ctx->delay_us(400); /* NOTE ? */
    }

    return;
}
#else
void mt9811_wakeup_sleep(mt9811_ctx *ctx)
{
#if 0
    ctx->tx.frm.cmd_h = 0xFF;
    ctx->rx.len  = ctx->tx.len = 1;
    ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len);
    ctx->delay_us(500 * ctx->ic_num);
#endif
    uint8_t ic = 0;

    for (ic = 0; ic < ctx->ic_num + 4; ic++) {
        ctx->tx.frm.cmd_h = 0xFF;
        ctx->rx.len = ctx->tx.len = 1;
        ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len);
        ctx->delay_us(500);
    }

    return;
}
#endif

uint8_t mt9811_wakeup_idle(mt9811_ctx *ctx)
{
    uint8_t ic = 0;
    uint8_t ret = ERR_COMMON;

    ctx->tx.frm.cmd_h = 0xFF;
    ctx->rx.len  = ctx->tx.len = 1;
    for (ic = 0; ic < ctx->ic_num + 48; ic++) {
        if (ERR_COMMON == ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len)) {
            goto exit;
        }
    }
    ret = ERR_OK;

exit:
    return ret;
}

uint8_t mt9811_write_cfgr(mt9811_ctx *ctx)
{
    uint8_t ic = 0;
    uint16_t pec = 0;
    uint8_t ret = ERR_COMMON;

    ctx->tx.frm.cmd_h = MT9811_WRCFG_H;
    ctx->tx.frm.cmd_l = MT9811_WRCFG_L;
    ctx->tx.frm.pec_h = MT9811_WRCFG_PEC_H;
    ctx->tx.frm.pec_l = MT9811_WRCFG_PEC_L;

    for (ic = 0; ic < ctx->ic_num; ic++) {
        memcpy(&ctx->tx.frm.param[ic*8], &ctx->w_cfgr[ctx->ic_num - ic - 1], sizeof(mt9811_cfgr));
        pec = pec15_calc(6, &ctx->tx.frm.param[ic*8]); /* pec */
        ctx->tx.frm.param[6 + ic*8] = (uint8_t)(pec >> 8);
        ctx->tx.frm.param[7 + ic*8] = (uint8_t)pec;
    }
    ctx->rx.len = ctx->tx.len = 4 + 8*ctx->ic_num;
    ret = ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len);

    return ret;
}

uint8_t mt9811_read_cfgr(mt9811_ctx *ctx)
{
    uint8_t ic = 0;
    uint16_t pec = 0;
    uint16_t calc_pec = 0;
    uint8_t ret = ERR_COMMON;

    ctx->tx.frm.cmd_h = MT9811_RDCFG_H;
    ctx->tx.frm.cmd_l = MT9811_RDCFG_L;
    ctx->tx.frm.pec_h = MT9811_RDCFG_PEC_H;
    ctx->tx.frm.pec_l = MT9811_RDCFG_PEC_L;
    ctx->rx.len = ctx->tx.len = 4 + 8*ctx->ic_num;
    if (ERR_OK == (ret = ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len))) {
        for (ic = 0; ic < ctx->ic_num; ic++) {
            pec = (ctx->rx.frm.param[8*ic + 6] << 8) + ctx->rx.frm.param[8*ic + 7];
            calc_pec = pec15_calc(6, &ctx->rx.frm.param[8*ic]);
            if (pec == calc_pec) {
                memcpy(&ctx->r_cfgr[ic], &ctx->rx.frm.param[8*ic], sizeof(mt9811_cfgr));
            } else {
                ctx->pec_err += 1;
                ret = ERR_COMMON;
                goto exit;
            }
        }
    }

exit:
    return ret;
}

uint8_t mt9811_read_cv(mt9811_ctx *ctx, uint8_t group)
{
    uint8_t ic = 0;
    void *pcv = NULL;
    uint8_t *perr = NULL;
    uint16_t size = sizeof(mt9811_cvr);
    static uint16_t pec = 0;
    static uint16_t calc_pec = 0;
    uint8_t ret = ERR_COMMON;

    if (MT9811_CV_A == group) {
        ctx->tx.frm.cmd_h = MT9811_RDCVA_H;
        ctx->tx.frm.cmd_l = MT9811_RDCVA_L;
        ctx->tx.frm.pec_h = MT9811_RDCVA_PEC_H;
        ctx->tx.frm.pec_l = MT9811_RDCVA_PEC_L;
        pcv = ctx->a_cvr;
        perr = ctx->a_cv_err;
    } else if (MT9811_CV_B == group) {
        ctx->tx.frm.cmd_h = MT9811_RDCVB_H;
        ctx->tx.frm.cmd_l = MT9811_RDCVB_L;
        ctx->tx.frm.pec_h = MT9811_RDCVB_PEC_H;
        ctx->tx.frm.pec_l = MT9811_RDCVB_PEC_L;
        pcv = ctx->b_cvr;
        perr = ctx->b_cv_err;
    } else if (MT9811_CV_C == group) {
        ctx->tx.frm.cmd_h = MT9811_RDCVC_H;
        ctx->tx.frm.cmd_l = MT9811_RDCVC_L;
        ctx->tx.frm.pec_h = MT9811_RDCVC_PEC_H;
        ctx->tx.frm.pec_l = MT9811_RDCVC_PEC_L;
        pcv = ctx->c_cvr;
        perr = ctx->c_cv_err;
    } else if (MT9811_CV_D == group) {
        ctx->tx.frm.cmd_h = MT9811_RDCVD_H;
        ctx->tx.frm.cmd_l = MT9811_RDCVD_L;
        ctx->tx.frm.pec_h = MT9811_RDCVD_PEC_H;
        ctx->tx.frm.pec_l = MT9811_RDCVD_PEC_L;
        pcv = ctx->d_cvr;
        perr = ctx->d_cv_err;
    } else {
        goto exit;
    }
    ctx->rx.len = ctx->tx.len = 4 + 8*ctx->ic_num;
    if (ERR_OK == (ret = ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len))) {
        for (ic = 0; ic < ctx->ic_num; ic++) {
            pec = (ctx->rx.frm.param[8*ic + 6] << 8) + ctx->rx.frm.param[8*ic + 7];
            calc_pec = pec15_calc(6, &ctx->rx.frm.param[8*ic]);
            if (pec == calc_pec) {
                *(perr + ic) = 0;
                memcpy((uint8_t *)pcv + size*ic , &ctx->rx.frm.param[8*ic], size);
            } else {
                *(perr + ic) = 1;
                ctx->pec_err += 1;
            }
        }
    }

exit:
    return ret;
}

uint8_t mt9811_start_cv(mt9811_ctx *ctx, uint8_t md, uint8_t dcp, uint8_t ch)
{
    uint8_t md_bits = 0;
    uint16_t pec = 0;
    uint8_t ret = ERR_COMMON;

    md_bits = (md & 0x02) >> 1;
    ctx->tx.frm.cmd_h = md_bits + 0x02;
    md_bits = (md & 0x01) << 7;
    ctx->tx.frm.cmd_l =  md_bits + 0x60 + (dcp<<4) + ch;
    pec = pec15_calc(2, &ctx->tx.frm.cmd_h);
    ctx->tx.frm.pec_h = (uint8_t)(pec >> 8);
    ctx->tx.frm.pec_l = (uint8_t)(pec);
    ctx->rx.len = ctx->tx.len = 4;
    ret = ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len);

    return ret;
}

uint8_t mt9811_read_aux(mt9811_ctx *ctx, uint8_t group)
{
    uint8_t ic = 0;
    void *paux = NULL;
    uint8_t *perr = NULL;
    uint16_t size = 0;
    uint16_t pec = 0;
    uint16_t calc_pec = 0;
    uint8_t ret = ERR_COMMON;

    if (MT9811_AUX_A == group) {
        ctx->tx.frm.cmd_h = MT9811_RDAUXA_H;
        ctx->tx.frm.cmd_l = MT9811_RDAUXA_L;
        ctx->tx.frm.pec_h = MT9811_RDAUXA_PEC_H;
        ctx->tx.frm.pec_l = MT9811_RDAUXA_PEC_L;
        paux = ctx->a_auxr;
        perr = ctx->a_aux_err;
        size =sizeof(mt9811_auxr_a);
    } else if (MT9811_AUX_B == group) {
        ctx->tx.frm.cmd_h = MT9811_RDAUXB_H;
        ctx->tx.frm.cmd_l = MT9811_RDAUXB_L;
        ctx->tx.frm.pec_h = MT9811_RDAUXB_PEC_H;
        ctx->tx.frm.pec_l = MT9811_RDAUXB_PEC_L;
        paux = ctx->b_auxr;
        perr = ctx->b_aux_err;
        size =sizeof(mt9811_auxr_b);
    } else {
        goto exit;
    }
    ctx->rx.len = ctx->tx.len = 4 + 8*ctx->ic_num;
    if (ERR_OK == (ret = ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len))) {
        for (ic = 0; ic < ctx->ic_num; ic++) {
            pec = (ctx->rx.frm.param[8*ic + 6] << 8) + ctx->rx.frm.param[8*ic + 7];
            calc_pec = pec15_calc(6, &ctx->rx.frm.param[8*ic]);
            if (pec == calc_pec) {
                *(perr + ic) = 0;
                memcpy((uint8_t *)paux + size*ic, &ctx->rx.frm.param[8*ic], size);
            } else {
                *(perr + ic) = 1;
                ctx->pec_err += 1;
            }
        }
    }

exit:
    return ret;
}

uint8_t mt9811_start_aux(mt9811_ctx *ctx, uint8_t md, uint8_t chg)
{
    uint8_t md_bits = 0;
    uint16_t pec = 0;
    uint8_t ret = ERR_COMMON;

    md_bits = (md & 0x02) >> 1;
    ctx->tx.frm.cmd_h = md_bits + 0x04;
    md_bits = (md & 0x01) << 7;
    ctx->tx.frm.cmd_l =  md_bits + 0x60 + chg;
    pec = pec15_calc(2, &ctx->tx.frm.cmd_h);
    ctx->tx.frm.pec_h = (uint8_t)(pec >> 8);
    ctx->tx.frm.pec_l = (uint8_t)(pec);
    ctx->rx.len = ctx->tx.len = 4;
    ret = ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len);

    return ret;
}

uint8_t mt9811_start_cv_aux(mt9811_ctx *ctx, uint8_t md, uint8_t dcp)
{
    uint8_t md_bits = 0;
    uint16_t pec = 0;
    uint8_t ret = ERR_COMMON;

    md_bits = (md & 0x02) >> 1;
    ctx->tx.frm.cmd_h = md_bits | 0x04;
    md_bits = (md & 0x01) << 7;
    ctx->tx.frm.cmd_l =  md_bits | ((dcp & 0x01) << 4) + 0x6F;
    pec = pec15_calc(2, &ctx->tx.frm.cmd_h);
    ctx->tx.frm.pec_h = (uint8_t)(pec >> 8);
    ctx->tx.frm.pec_l = (uint8_t)(pec);
    ctx->rx.len = ctx->tx.len = 4;
    ret = ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len);

    return ret;
}

uint8_t mt9811_read_stat(mt9811_ctx *ctx, uint8_t group)
{
    uint8_t ic = 0;
    void *pstat = NULL;
    uint16_t size = 0;
    uint16_t pec = 0;
    uint16_t calc_pec = 0;
    uint8_t ret = ERR_COMMON;

    if (MT9811_STAT_A == group) {
        ctx->tx.frm.cmd_h = MT9811_RDSTATA_H;
        ctx->tx.frm.cmd_l = MT9811_RDSTATA_L;
        ctx->tx.frm.pec_h = MT9811_RDSTATA_PEC_H;
        ctx->tx.frm.pec_l = MT9811_RDSTATA_PEC_L;
        pstat = ctx->a_statr;
        size = sizeof(mt9811_statr_a);
    } else if (MT9811_STAT_B == group) {
        ctx->tx.frm.cmd_h = MT9811_RDSTATB_H;
        ctx->tx.frm.cmd_l = MT9811_RDSTATB_L;
        ctx->tx.frm.pec_h = MT9811_RDSTATB_PEC_H;
        ctx->tx.frm.pec_l = MT9811_RDSTATB_PEC_L;
        pstat = ctx->b_statr;
        size = sizeof(mt9811_statr_b);
    } else {
        goto exit;
    }
    ctx->rx.len = ctx->tx.len = 4 + 8*ctx->ic_num;
    if (ERR_OK == (ret = ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len))) {
        for (ic = 0; ic < ctx->ic_num; ic++) {
            pec = (ctx->rx.frm.param[8*ic + 6] << 8) + ctx->rx.frm.param[8*ic + 7];
            calc_pec = pec15_calc(6, &ctx->rx.frm.param[8*ic]);
            if (pec == calc_pec) {
                memcpy((uint8_t *)pstat + size*ic, &ctx->rx.frm.param[8*ic], size);
            } else {
                ctx->pec_err += 1;
                ret = ERR_COMMON;
                goto exit;
            }
        }
    }

exit:
    return ret;
}

uint8_t mt9811_start_stat(mt9811_ctx *ctx, uint8_t md, uint8_t chst)
{
    uint8_t md_bits = 0;
    uint16_t pec = 0;
    uint8_t ret = ERR_COMMON;

    md_bits = (md & 0x02) >> 1;
    ctx->tx.frm.cmd_h = md_bits + 0x04;
    md_bits = (md & 0x01) << 7;
    ctx->tx.frm.cmd_l =  md_bits + 0x68 + chst;
    pec = pec15_calc(2, &ctx->tx.frm.cmd_h);
    ctx->tx.frm.pec_h = (uint8_t)(pec >> 8);
    ctx->tx.frm.pec_l = (uint8_t)(pec);
    ctx->rx.len = ctx->tx.len = 4;
    ret = ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len);

    return ret;
}

uint8_t mt9811_clear_cv(mt9811_ctx *ctx)
{
    uint16_t pec = 0;
    uint8_t ret = ERR_COMMON;

    ctx->tx.frm.cmd_h = 0x07;
    ctx->tx.frm.cmd_l = 0x11;
    pec = pec15_calc(2, &ctx->tx.frm.cmd_h);
    ctx->tx.frm.pec_h = (uint8_t)(pec >> 8);
    ctx->tx.frm.pec_l = (uint8_t)(pec);
    ctx->rx.len = ctx->tx.len = 4;
    ret = ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len);

    return ret;
}

uint8_t mt9811_clear_aux(mt9811_ctx *ctx)
{
    uint16_t pec = 0;
    uint8_t ret = ERR_COMMON;

    ctx->tx.frm.cmd_h = 0x07;
    ctx->tx.frm.cmd_l = 0x12;
    pec = pec15_calc(2, &ctx->tx.frm.cmd_h);
    ctx->tx.frm.pec_h = (uint8_t)(pec >> 8);
    ctx->tx.frm.pec_l = (uint8_t)(pec);
    ctx->rx.len = ctx->tx.len = 4;
    ret = ctx->transfer(ctx, (const uint8_t *)&ctx->tx.frm, (uint8_t *)&ctx->rx.frm, ctx->tx.len);

    return ret;
}




