#include <string.h>
#include "common.h"
#include "def_pins.h"
#include "hw_gpio.h"
#include "hw_spi.h"
#include "mt9811_reg.h"
#include "debug_uart.h"
#include "hw_afe.h"

enum {
    MEASURE_CELL_VOLTAGE = 0,
    MEASURE_GPIO_VOLTAGE
};
enum {
    AFE_IC_0 = 0,
    AFE_IC_1,
    AFE_IC_2,
    AFE_IC_3,
    AFE_IC_4,
    AFE_IC_5,
    AFE_IC_MAX
};
enum {
    AFE_CV_GROUP_A = 0,
    AFE_CV_GROUP_B,
    AFE_CV_GROUP_C,
    AFE_CV_GROUP_D,
    AFE_CV_GROUP_MAX
};
enum {
    AFE_CV_IDX_0 = 0,
    AFE_CV_IDX_1,
    AFE_CV_IDX_2,
    AFE_CV_IDX_MAX
};
enum {
    AFE_AUX_G1 = 0,
    AFE_AUX_G2,
    AFE_AUX_G3,
    AFE_AUX_G4,
    AFE_AUX_G5,
    AFE_AUX_VREF2,
    AFE_AUX_MAX
};
typedef struct {
    uint8_t ch;
    uint8_t ic;
    uint8_t group;
    uint8_t idx;
} afe_cv_map;
typedef struct {
    uint8_t ch;
    uint8_t ic;
    uint8_t aux;
} afe_aux_map;
typedef struct {
    uint8_t ch;
    uint8_t ic;
    uint8_t offset;
} afe_dcc_map;

typedef struct {
    mt9811_ctx dev;

    uint8_t state;   /* run steps */
    uint8_t aux_cnt; /* timing of mesure aux */
} afe_ctl;

const static afe_cv_map g_cv_tbl[] = {
    {0, AFE_IC_0, AFE_CV_GROUP_A, AFE_CV_IDX_0}, /* batt 1~12 */
    {1, AFE_IC_0, AFE_CV_GROUP_A, AFE_CV_IDX_1},
    {2, AFE_IC_0, AFE_CV_GROUP_A, AFE_CV_IDX_2},
    {3, AFE_IC_0, AFE_CV_GROUP_B, AFE_CV_IDX_0},
    {4, AFE_IC_0, AFE_CV_GROUP_B, AFE_CV_IDX_1},
    {5, AFE_IC_0, AFE_CV_GROUP_B, AFE_CV_IDX_2},
    {6, AFE_IC_0, AFE_CV_GROUP_C, AFE_CV_IDX_0},
    {7, AFE_IC_0, AFE_CV_GROUP_C, AFE_CV_IDX_1},
    {8, AFE_IC_0, AFE_CV_GROUP_C, AFE_CV_IDX_2},
    {9, AFE_IC_0, AFE_CV_GROUP_D, AFE_CV_IDX_0},
    {10, AFE_IC_0, AFE_CV_GROUP_D, AFE_CV_IDX_1},
    {11, AFE_IC_0, AFE_CV_GROUP_D, AFE_CV_IDX_2},
    {12, AFE_IC_2, AFE_CV_GROUP_A, AFE_CV_IDX_0}, /* batt 13~24 */
    {13, AFE_IC_2, AFE_CV_GROUP_A, AFE_CV_IDX_1},
    {14, AFE_IC_2, AFE_CV_GROUP_A, AFE_CV_IDX_2},
    {15, AFE_IC_2, AFE_CV_GROUP_B, AFE_CV_IDX_0},
    {16, AFE_IC_2, AFE_CV_GROUP_B, AFE_CV_IDX_1},
    {17, AFE_IC_2, AFE_CV_GROUP_B, AFE_CV_IDX_2},
    {18, AFE_IC_2, AFE_CV_GROUP_C, AFE_CV_IDX_0},
    {19, AFE_IC_2, AFE_CV_GROUP_C, AFE_CV_IDX_1},
    {20, AFE_IC_2, AFE_CV_GROUP_C, AFE_CV_IDX_2},
    {21, AFE_IC_2, AFE_CV_GROUP_D, AFE_CV_IDX_0},
    {22, AFE_IC_2, AFE_CV_GROUP_D, AFE_CV_IDX_1},
    {23, AFE_IC_2, AFE_CV_GROUP_D, AFE_CV_IDX_2},
    {24, AFE_IC_4, AFE_CV_GROUP_A, AFE_CV_IDX_0}, /* batt 25~35 */
    {25, AFE_IC_4, AFE_CV_GROUP_A, AFE_CV_IDX_1},
    {26, AFE_IC_4, AFE_CV_GROUP_A, AFE_CV_IDX_2},
    {27, AFE_IC_4, AFE_CV_GROUP_B, AFE_CV_IDX_0},
    {28, AFE_IC_4, AFE_CV_GROUP_B, AFE_CV_IDX_1},
    {29, AFE_IC_4, AFE_CV_GROUP_B, AFE_CV_IDX_2},
    {30, AFE_IC_4, AFE_CV_GROUP_C, AFE_CV_IDX_0},
    {31, AFE_IC_4, AFE_CV_GROUP_C, AFE_CV_IDX_1},
    {32, AFE_IC_4, AFE_CV_GROUP_C, AFE_CV_IDX_2},
    {33, AFE_IC_4, AFE_CV_GROUP_D, AFE_CV_IDX_0},
    {34, AFE_IC_4, AFE_CV_GROUP_D, AFE_CV_IDX_1},
    {35, AFE_IC_5, AFE_CV_GROUP_A, AFE_CV_IDX_0}, /* batt 36~47 */
    {36, AFE_IC_5, AFE_CV_GROUP_A, AFE_CV_IDX_1},
    {37, AFE_IC_5, AFE_CV_GROUP_A, AFE_CV_IDX_2},
    {38, AFE_IC_5, AFE_CV_GROUP_B, AFE_CV_IDX_0},
    {39, AFE_IC_5, AFE_CV_GROUP_B, AFE_CV_IDX_1},
    {40, AFE_IC_5, AFE_CV_GROUP_B, AFE_CV_IDX_2},
    {41, AFE_IC_5, AFE_CV_GROUP_C, AFE_CV_IDX_0},
    {42, AFE_IC_5, AFE_CV_GROUP_C, AFE_CV_IDX_1},
    {43, AFE_IC_5, AFE_CV_GROUP_C, AFE_CV_IDX_2},
    {44, AFE_IC_5, AFE_CV_GROUP_D, AFE_CV_IDX_0},
    {45, AFE_IC_5, AFE_CV_GROUP_D, AFE_CV_IDX_1},
    {46, AFE_IC_5, AFE_CV_GROUP_D, AFE_CV_IDX_2},
    {47, AFE_IC_3, AFE_CV_GROUP_A, AFE_CV_IDX_0}, /* batt 48~59 */
    {48, AFE_IC_3, AFE_CV_GROUP_A, AFE_CV_IDX_1},
    {49, AFE_IC_3, AFE_CV_GROUP_A, AFE_CV_IDX_2},
    {50, AFE_IC_3, AFE_CV_GROUP_B, AFE_CV_IDX_0},
    {51, AFE_IC_3, AFE_CV_GROUP_B, AFE_CV_IDX_1},
    {52, AFE_IC_3, AFE_CV_GROUP_B, AFE_CV_IDX_2},
    {53, AFE_IC_3, AFE_CV_GROUP_C, AFE_CV_IDX_0},
    {54, AFE_IC_3, AFE_CV_GROUP_C, AFE_CV_IDX_1},
    {55, AFE_IC_3, AFE_CV_GROUP_C, AFE_CV_IDX_2},
    {56, AFE_IC_3, AFE_CV_GROUP_D, AFE_CV_IDX_0},
    {57, AFE_IC_3, AFE_CV_GROUP_D, AFE_CV_IDX_1},
    {58, AFE_IC_3, AFE_CV_GROUP_D, AFE_CV_IDX_2},
    {59, AFE_IC_1, AFE_CV_GROUP_A, AFE_CV_IDX_0}, /* batt 60~70 */
    {60, AFE_IC_1, AFE_CV_GROUP_A, AFE_CV_IDX_1},
    {61, AFE_IC_1, AFE_CV_GROUP_A, AFE_CV_IDX_2},
    {62, AFE_IC_1, AFE_CV_GROUP_B, AFE_CV_IDX_0},
    {63, AFE_IC_1, AFE_CV_GROUP_B, AFE_CV_IDX_1},
    {64, AFE_IC_1, AFE_CV_GROUP_B, AFE_CV_IDX_2},
    {65, AFE_IC_1, AFE_CV_GROUP_C, AFE_CV_IDX_0},
    {66, AFE_IC_1, AFE_CV_GROUP_C, AFE_CV_IDX_1},
    {67, AFE_IC_1, AFE_CV_GROUP_C, AFE_CV_IDX_2},
    {68, AFE_IC_1, AFE_CV_GROUP_D, AFE_CV_IDX_0},
    {69, AFE_IC_1, AFE_CV_GROUP_D, AFE_CV_IDX_1},
};

const static afe_aux_map g_aux_tbl[] = {
/* for cells */
    {0, AFE_IC_0, AFE_AUX_G1},
    {1, AFE_IC_0, AFE_AUX_G4},
    {2, AFE_IC_1, AFE_AUX_G1},
    {3, AFE_IC_1, AFE_AUX_G2},
    {4, AFE_IC_2, AFE_AUX_G1},
    {5, AFE_IC_2, AFE_AUX_G2},
    {6, AFE_IC_3, AFE_AUX_G1},
    {7, AFE_IC_3, AFE_AUX_G2},
    {8, AFE_IC_4, AFE_AUX_G1},
    {9, AFE_IC_4, AFE_AUX_G2},
    {10, AFE_IC_5, AFE_AUX_G1},
    {11, AFE_IC_5, AFE_AUX_G2},
/* for case */
    {12, AFE_IC_0, AFE_AUX_G2},
    {13, AFE_IC_0, AFE_AUX_G5},
};

const static afe_dcc_map g_dcc_tbl[] = {
    {0, AFE_IC_0, 0}, /* batt 1~12 */
    {1, AFE_IC_0, 1},
    {2, AFE_IC_0, 2},
    {3, AFE_IC_0, 3},
    {4, AFE_IC_0, 4},
    {5, AFE_IC_0, 5},
    {6, AFE_IC_0, 6},
    {7, AFE_IC_0, 7},
    {8, AFE_IC_0, 8},
    {9, AFE_IC_0, 9},
    {10, AFE_IC_0, 10},
    {11, AFE_IC_0, 11},
    {12, AFE_IC_2, 0}, /* batt 13~24 */
    {13, AFE_IC_2, 1},
    {14, AFE_IC_2, 2},
    {15, AFE_IC_2, 3},
    {16, AFE_IC_2, 4},
    {17, AFE_IC_2, 5},
    {18, AFE_IC_2, 6},
    {19, AFE_IC_2, 7},
    {20, AFE_IC_2, 8},
    {21, AFE_IC_2, 9},
    {22, AFE_IC_2, 10},
    {23, AFE_IC_2, 11},
    {24, AFE_IC_4, 0}, /* batt 25~35 */
    {25, AFE_IC_4, 1},
    {26, AFE_IC_4, 2},
    {27, AFE_IC_4, 3},
    {28, AFE_IC_4, 4},
    {29, AFE_IC_4, 5},
    {30, AFE_IC_4, 6},
    {31, AFE_IC_4, 7},
    {32, AFE_IC_4, 8},
    {33, AFE_IC_4, 9},
    {34, AFE_IC_4, 10},
    {35, AFE_IC_5, 0}, /* batt 36~47 */
    {36, AFE_IC_5, 1},
    {37, AFE_IC_5, 2},
    {38, AFE_IC_5, 3},
    {39, AFE_IC_5, 4},
    {40, AFE_IC_5, 5},
    {41, AFE_IC_5, 6},
    {42, AFE_IC_5, 7},
    {43, AFE_IC_5, 8},
    {44, AFE_IC_5, 9},
    {45, AFE_IC_5, 10},
    {46, AFE_IC_5, 11},
    {47, AFE_IC_3, 0}, /* batt 48~59 */
    {48, AFE_IC_3, 1},
    {49, AFE_IC_3, 2},
    {50, AFE_IC_3, 3},
    {51, AFE_IC_3, 4},
    {52, AFE_IC_3, 5},
    {53, AFE_IC_3, 6},
    {54, AFE_IC_3, 7},
    {55, AFE_IC_3, 8},
    {56, AFE_IC_3, 9},
    {57, AFE_IC_3, 10},
    {58, AFE_IC_3, 11},
    {59, AFE_IC_1, 0}, /* batt 60~70 */
    {60, AFE_IC_1, 1},
    {61, AFE_IC_1, 2},
    {62, AFE_IC_1, 3},
    {63, AFE_IC_1, 4},
    {64, AFE_IC_1, 5},
    {65, AFE_IC_1, 6},
    {66, AFE_IC_1, 7},
    {67, AFE_IC_1, 8},
    {68, AFE_IC_1, 9},
    {69, AFE_IC_1, 10},
};

static mt9811_cfgr g_w_cfgr[AFE_IC_NUM] = {
    {0,0,1,1,1,1,1,1,32000/1600,48000/1600, 0,0},
    {0,0,1,1,1,1,1,1,32000/1600,48000/1600, 0,0},
    {0,0,1,1,1,1,1,1,32000/1600,48000/1600, 0,0},
    {0,0,1,1,1,1,1,1,32000/1600,48000/1600, 0,0},
    {0,0,1,1,1,1,1,1,32000/1600,48000/1600, 0,0},
    {0,0,1,1,1,1,1,1,32000/1600,48000/1600, 0,0},
};

static mt9811_cfgr g_r_cfgr[AFE_IC_NUM];
static mt9811_cvr g_a_cvr[AFE_IC_NUM];
static mt9811_cvr g_b_cvr[AFE_IC_NUM];
static mt9811_cvr g_c_cvr[AFE_IC_NUM];
static mt9811_cvr g_d_cvr[AFE_IC_NUM];
static mt9811_auxr_a g_a_auxr[AFE_IC_NUM];
static mt9811_auxr_b g_b_auxr[AFE_IC_NUM];
static mt9811_statr_a g_a_statr[AFE_IC_NUM];
static mt9811_statr_b g_b_statr[AFE_IC_NUM];
static uint8_t g_a_cv_err[AFE_IC_NUM];
static uint8_t g_b_cv_err[AFE_IC_NUM];
static uint8_t g_c_cv_err[AFE_IC_NUM];
static uint8_t g_d_cv_err[AFE_IC_NUM];
static uint8_t g_a_aux_err[AFE_IC_NUM];
static uint8_t g_b_aux_err[AFE_IC_NUM];
static uint8_t g_a_stat_err[AFE_IC_NUM];
static uint8_t g_b_stat_err[AFE_IC_NUM];

static afe_ctl g_ctl;

void platform_delay_us(uint16_t us)
{
    uint32_t delay = 3 * us;
    do {
        asm("nop"); asm("nop"); asm("nop"); asm("nop");
        asm("nop"); asm("nop"); asm("nop"); asm("nop");
        asm("nop"); asm("nop"); asm("nop"); asm("nop");
        asm("nop"); asm("nop"); asm("nop"); asm("nop");
    } while (delay --);

    return;
}

static void platform_wakeup_pin(uint8_t state)
{
    (0 == state) ? (GPIO_Output_Low(GPIO_SPI1_CS1)):(GPIO_Output_High(GPIO_SPI1_CS1));

    return;
}

static uint8_t platform_spi_transfer(void *handle, const uint8_t *send, uint8_t *recv, uint16_t len)
{
    uint8_t ret = ERR_COMMON;

    if (NULL == send || NULL == recv || 0 == len) {
        goto exit;
    }

    GPIO_Output_Low(GPIO_SPI1_CS1);
    ret = hwSpiTransfer(1, (uint8_t *)send, recv, len);
    GPIO_Output_High(GPIO_SPI1_CS1);

exit:
    return ret;
}

static void _afeIsospiEnable(void)
{
    /* nothing to do */
    return;
}

void AfeInit(void)
{
    memset(&g_ctl, 0, sizeof(g_ctl));
    memset(g_r_cfgr, 0, sizeof(g_r_cfgr));
    memset(g_a_cvr, 0, sizeof(g_a_cvr));
    memset(g_b_cvr, 0, sizeof(g_b_cvr));
    memset(g_c_cvr, 0, sizeof(g_c_cvr));
    memset(g_d_cvr, 0, sizeof(g_d_cvr));
    memset(g_a_auxr, 0, sizeof(g_a_auxr));
    memset(g_b_auxr, 0, sizeof(g_b_auxr));
    memset(g_a_statr, 0, sizeof(g_a_statr));
    memset(g_b_statr, 0, sizeof(g_b_statr));
    memset(g_a_cv_err, 1, sizeof(g_a_cv_err));
    memset(g_b_cv_err, 1, sizeof(g_b_cv_err));
    memset(g_c_cv_err, 1, sizeof(g_c_cv_err));
    memset(g_d_cv_err, 1, sizeof(g_d_cv_err));
    memset(g_a_aux_err, 1, sizeof(g_a_aux_err));
    memset(g_b_aux_err, 1, sizeof(g_b_aux_err));
    memset(g_a_stat_err, 1, sizeof(g_a_stat_err));
    memset(g_b_stat_err, 1, sizeof(g_b_stat_err));
    g_ctl.dev.w_cfgr = g_w_cfgr; g_ctl.dev.r_cfgr = g_r_cfgr;
    g_ctl.dev.a_cvr = g_a_cvr; g_ctl.dev.b_cvr = g_b_cvr; g_ctl.dev.c_cvr = g_c_cvr; g_ctl.dev.d_cvr = g_d_cvr;
    g_ctl.dev.a_auxr = g_a_auxr; g_ctl.dev.b_auxr = g_b_auxr;
    g_ctl.dev.a_statr = g_a_statr; g_ctl.dev.b_statr = g_b_statr;
    g_ctl.dev.a_cv_err = g_a_cv_err; g_ctl.dev.b_cv_err = g_b_cv_err; g_ctl.dev.c_cv_err = g_c_cv_err; g_ctl.dev.d_cv_err = g_d_cv_err;
    g_ctl.dev.a_aux_err = g_a_aux_err; g_ctl.dev.b_aux_err = g_b_aux_err;
    g_ctl.dev.a_stat_err = g_a_stat_err; g_ctl.dev.b_stat_err = g_b_stat_err;
    g_ctl.dev.transfer = platform_spi_transfer;
    g_ctl.dev.wakeup_pin = platform_wakeup_pin;
    g_ctl.dev.delay_us = platform_delay_us;
    g_ctl.dev.ic_num = AFE_IC_NUM;
/* isospi enable */
    _afeIsospiEnable();
/* wakeup mt9811 */
    mt9811_wakeup_sleep(&g_ctl.dev);
/* write config */
    mt9811_write_cfgr(&g_ctl.dev);
    platform_delay_us(2000);
/* read cfgr */
    mt9811_read_cfgr(&g_ctl.dev);
    for (uint8_t i = 0; i < g_ctl.dev.ic_num; i++) {
        DbgPrintfV("adcopt:%d swtrd:%d refon:%d gpio1:%d gpio2:%d gpio3:%d gpio4:%d gpio5:%d vuv:%d vov:%d dcc:0x%02x tcto:0x%02x \n\r", \
            g_ctl.dev.r_cfgr[i].adcopt, g_ctl.dev.r_cfgr[i].swtrd, g_ctl.dev.r_cfgr[i].refon, g_ctl.dev.r_cfgr[i].gpio1, g_ctl.dev.r_cfgr[i].gpio2, g_ctl.dev.r_cfgr[i].gpio3, \
            g_ctl.dev.r_cfgr[i].gpio4, g_ctl.dev.r_cfgr[i].gpio5, g_ctl.dev.r_cfgr[i].vuv*1600, g_ctl.dev.r_cfgr[i].vov*1600, g_ctl.dev.r_cfgr[i].dcc, g_ctl.dev.r_cfgr[i].tcto);
    }

    mt9811_start_cv(&g_ctl.dev, MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL);
    platform_delay_us(4000);

    return;
}


void AfePoll(void)
{
    if (3 == ++g_ctl.aux_cnt) {
        g_ctl.aux_cnt = 0;
        mt9811_wakeup_idle(&g_ctl.dev);
        mt9811_write_cfgr(&g_ctl.dev);

        switch (g_ctl.state) {
            case MEASURE_CELL_VOLTAGE:
                mt9811_read_cv(&g_ctl.dev, MT9811_CV_A);
                mt9811_read_cv(&g_ctl.dev, MT9811_CV_B);
                mt9811_read_cv(&g_ctl.dev, MT9811_CV_C);
                mt9811_read_cv(&g_ctl.dev, MT9811_CV_D);
                mt9811_start_aux(&g_ctl.dev, MD_7KHZ_3KHZ, AUX_CH_ALL);
                g_ctl.state = MEASURE_GPIO_VOLTAGE;
                break;
            case MEASURE_GPIO_VOLTAGE:
                mt9811_read_aux(&g_ctl.dev, MT9811_AUX_A);
                mt9811_read_aux(&g_ctl.dev, MT9811_AUX_B);
                //mt9811_clear_cv(&g_ctl.dev); /* NOTE ... */
                mt9811_start_cv(&g_ctl.dev, MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL);
                g_ctl.state = MEASURE_CELL_VOLTAGE;
                break;
            default:
                break;
        }

    }

    return;
}

void _AfePoll(void)
{
    mt9811_wakeup_idle(&g_ctl.dev);
    switch (g_ctl.state) {
        case MEASURE_CELL_VOLTAGE:
            mt9811_read_cv(&g_ctl.dev, MT9811_CV_A);
            mt9811_read_cv(&g_ctl.dev, MT9811_CV_B);
            mt9811_read_cv(&g_ctl.dev, MT9811_CV_C);
            mt9811_read_cv(&g_ctl.dev, MT9811_CV_D);
            if (10 == ++g_ctl.aux_cnt) {
                g_ctl.aux_cnt = 0;
                mt9811_start_aux(&g_ctl.dev, MD_7KHZ_3KHZ, AUX_CH_ALL);
                g_ctl.state = MEASURE_GPIO_VOLTAGE;
            } else {
                mt9811_start_cv(&g_ctl.dev, MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL);
            }
            break;
        case MEASURE_GPIO_VOLTAGE:
            mt9811_read_aux(&g_ctl.dev, MT9811_AUX_A);
            mt9811_read_aux(&g_ctl.dev, MT9811_AUX_B);
            mt9811_start_cv(&g_ctl.dev, MD_7KHZ_3KHZ, DCP_DISABLED, CELL_CH_ALL);
            g_ctl.state = MEASURE_CELL_VOLTAGE;
            break;
        default:
            break;
    }

    return;
}


uint8_t AfeCellVoltageGet(uint8_t ch, uint16_t *voltage)
{
    uint8_t ret = ERR_COMMON;
    mt9811_cvr *pcv = NULL;
    uint8_t *cv_err = NULL;
    uint8_t ic = g_cv_tbl[ch].ic;
    uint8_t group = g_cv_tbl[ch].group;
    uint8_t idx = g_cv_tbl[ch].idx;

    if (ch < sizeof(g_cv_tbl)/sizeof(g_cv_tbl[0]) && ic < AFE_IC_MAX && group < AFE_CV_GROUP_MAX && idx < AFE_CV_IDX_MAX && NULL != voltage) {
        if (AFE_CV_GROUP_A == group) {
            pcv = g_a_cvr;
            cv_err = g_a_cv_err;
        } else if (AFE_CV_GROUP_B == group) {
            pcv = g_b_cvr;
            cv_err = g_b_cv_err;
        } else if (AFE_CV_GROUP_C == group) {
            pcv = g_c_cvr;
            cv_err = g_c_cv_err;
        } else if (AFE_CV_GROUP_D == group) {
            pcv = g_d_cvr;
            cv_err = g_d_cv_err;
        }
        if (0 == *(cv_err + ic)) {
            if (AFE_CV_IDX_0 == idx) {
                *voltage = pcv[ic].v0;
            } else if (AFE_CV_IDX_1 == idx) {
                *voltage = pcv[ic].v1;
            } else if (AFE_CV_IDX_2 == idx) {
                *voltage = pcv[ic].v2;
            }
            ret = ERR_OK;
        }
    }

    return ret;
}

/* reference voltage fixed 5v */
uint8_t _AfeGpioVoltageGet(uint8_t ch, uint16_t *voltage)
{
    uint8_t ret = ERR_COMMON;
    uint8_t ic = g_aux_tbl[ch].ic;
    uint8_t aux = g_aux_tbl[ch].aux;

    if (ch < sizeof(g_aux_tbl)/sizeof(g_aux_tbl[0]) && ic < AFE_IC_MAX && aux < AFE_AUX_MAX && NULL != voltage) {
        ret = ERR_OK;
        if (AFE_AUX_G1 == aux && 0 == g_a_aux_err[ic]) {
            *voltage = g_a_auxr[ic].g1;
        } else if (AFE_AUX_G2 == aux && 0 == g_a_aux_err[ic]) {
            *voltage = g_a_auxr[ic].g2;
        } else if (AFE_AUX_G3 == aux && 0 == g_a_aux_err[ic]) {
            *voltage = g_a_auxr[ic].g3;
        } else if (AFE_AUX_G4 == aux && 0 == g_b_aux_err[ic]) {
            *voltage = g_b_auxr[ic].g4;
        } else if (AFE_AUX_G5 == aux && 0 == g_b_aux_err[ic]) {
            *voltage = g_b_auxr[ic].g5;
        } else if (AFE_AUX_VREF2 == aux && 0 == g_b_aux_err[ic]) {
            *voltage = g_b_auxr[ic].ref;
        } else {
            ret = ERR_COMMON;
        }
    }

    return ret;
}

/* reference voltage from aux gpio3 */
uint8_t AfeGpioVoltageGet(uint8_t ch, uint16_t *voltage)
{
    uint8_t ret = ERR_COMMON;
    uint8_t ic = g_aux_tbl[ch].ic;
    uint8_t aux = g_aux_tbl[ch].aux;
    uint16_t tmpv = 0;

    if (ch < sizeof(g_aux_tbl)/sizeof(g_aux_tbl[0]) && ic < AFE_IC_MAX && aux < AFE_AUX_MAX && NULL != voltage) {
        ret = ERR_OK;
        if (AFE_AUX_G1 == aux && 0 == g_a_aux_err[ic] && 0 != g_a_auxr[ic].g3) {
            tmpv = (uint32_t)g_a_auxr[ic].g1*((uint32_t)(50000*1000)/(uint32_t)g_a_auxr[ic].g3)/1000;
            *voltage = tmpv;
        } else if (AFE_AUX_G2 == aux && 0 == g_a_aux_err[ic]) {
            tmpv = (uint32_t)g_a_auxr[ic].g2*((uint32_t)(50000*1000)/(uint32_t)g_a_auxr[ic].g3)/1000;
            *voltage = tmpv;
        } else if (AFE_AUX_G3 == aux && 0 == g_a_aux_err[ic]) {
            *voltage = g_a_auxr[ic].g3;
        } else if (AFE_AUX_G4 == aux && 0 == g_b_aux_err[ic]) {
            tmpv = (uint32_t)g_b_auxr[ic].g4*((uint32_t)(50000*1000)/(uint32_t)g_a_auxr[ic].g3)/1000;
            *voltage = tmpv;
        } else if (AFE_AUX_G5 == aux && 0 == g_b_aux_err[ic]) {
            tmpv = (uint32_t)g_b_auxr[ic].g5*((uint32_t)(50000*1000)/(uint32_t)g_a_auxr[ic].g3)/1000;
            *voltage = tmpv;
        } else if (AFE_AUX_VREF2 == aux && 0 == g_b_aux_err[ic]) {
            *voltage = g_b_auxr[ic].ref;
        } else {
            ret = ERR_COMMON;
        }
    }

    return ret;
}

uint8_t AfeSetDischarge(uint8_t ch)
{
    uint8_t ret = ERR_COMMON;
    uint8_t ic = g_dcc_tbl[ch].ic;
    uint8_t offset = g_dcc_tbl[ch].offset;

    if (ch < sizeof(g_dcc_tbl)/sizeof(g_dcc_tbl[0]) && ic < AFE_IC_MAX && offset < 12) {
        g_w_cfgr[ic].dcc |= (1 << offset);
        ret = ERR_OK;
    }

    return ret;
}

uint8_t AfeClearDischarge(uint8_t ch)
{
    uint8_t ret = ERR_COMMON;
    uint8_t ic = g_dcc_tbl[ch].ic;
    uint8_t offset = g_dcc_tbl[ch].offset;

    if (ch < sizeof(g_dcc_tbl)/sizeof(g_dcc_tbl[0]) && ic < AFE_IC_MAX && offset < 12) {
        g_w_cfgr[ic].dcc &= ~(1 << offset);
        ret = ERR_OK;
    }

    return ret;
}

uint8_t AfeGetDischarge(uint8_t ch, uint8_t *onoff)
{
    uint8_t ret = ERR_COMMON;
    uint8_t ic = g_dcc_tbl[ch].ic;
    uint8_t offset = g_dcc_tbl[ch].offset;

    if (ch < sizeof(g_dcc_tbl)/sizeof(g_dcc_tbl[0]) && ic < AFE_IC_MAX && offset < 12) {
        *onoff = (g_w_cfgr[ic].dcc >> offset) & 1;
        ret = ERR_OK;
    }

    return ret;
}

uint8_t AfePecErrGet(void)
{
    return g_ctl.dev.pec_err;
}

