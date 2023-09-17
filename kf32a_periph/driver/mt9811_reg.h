#ifndef __MT9811_REG_H
#define __MT9811_REG_H

#define DEV_LITTLE_ENDIAN

#define MT9811_MAX_IC_NUM           (16U)
#define MT9811_MAX_TRANSFER_SIZE    (128U)

/* write reg comm */
#define MT9811_WRCOMM_H             (0X07U)
#define MT9811_WRCOMM_L             (0X21U)
#define MT9811_WRCOMM_PEC_H         (0x24U)
#define MT9811_WRCOMM_PEC_L         (0xb2U)
/* read reg comm */
#define MT9811_RDCOMM_H             (0X07U)
#define MT9811_RDCOMM_L             (0X22U)
#define MT9811_RDCOMM_PEC_H         (0x32U)
#define MT9811_RDCOMM_PEC_L         (0xd6U)
/* write reg cfg */
#define MT9811_WRCFG_H              (0x00U)
#define MT9811_WRCFG_L              (0x01U)
#define MT9811_WRCFG_PEC_H          (0x3dU)
#define MT9811_WRCFG_PEC_L          (0x6eU)
/* read reg cfg */
#define MT9811_RDCFG_H              (0x00U)
#define MT9811_RDCFG_L              (0x02U)
#define MT9811_RDCFG_PEC_H          (0x2BU)
#define MT9811_RDCFG_PEC_L          (0x0AU)
/* read reg voltage A */
#define MT9811_RDCVA_H              (0x00U)
#define MT9811_RDCVA_L              (0x04U)
#define MT9811_RDCVA_PEC_H          (0x07U)
#define MT9811_RDCVA_PEC_L          (0xC2U)
/* read reg voltage B */
#define MT9811_RDCVB_H              0x00U
#define MT9811_RDCVB_L              0x06U
#define MT9811_RDCVB_PEC_H          0x9AU
#define MT9811_RDCVB_PEC_L          0x94U
/* read reg voltage C */
#define MT9811_RDCVC_H              0x00U
#define MT9811_RDCVC_L              0x08U
#define MT9811_RDCVC_PEC_H          0x5EU
#define MT9811_RDCVC_PEC_L          0x52U
/* read reg voltage D */
#define MT9811_RDCVD_H              0x00U
#define MT9811_RDCVD_L              0x0AU
#define MT9811_RDCVD_PEC_H          0xC3U
#define MT9811_RDCVD_PEC_L          0x04U
/* read reg aux A */
#define MT9811_RDAUXA_H             0x00U
#define MT9811_RDAUXA_L             0x0CU
#define MT9811_RDAUXA_PEC_H         0xEFU
#define MT9811_RDAUXA_PEC_L         0xCCU
/* read reg aux B */
#define MT9811_RDAUXB_H             0x00U
#define MT9811_RDAUXB_L             0x0EU
#define MT9811_RDAUXB_PEC_H         0x72U
#define MT9811_RDAUXB_PEC_L         0x9AU
/* read reg status A */
#define MT9811_RDSTATA_H            0x00U
#define MT9811_RDSTATA_L            0x10U
#define MT9811_RDSTATA_PEC_H        0xEDU
#define MT9811_RDSTATA_PEC_L        0x72U
/* read reg status B */
#define MT9811_RDSTATB_H            0x00U
#define MT9811_RDSTATB_L            0x12U
#define MT9811_RDSTATB_PEC_H        0x70U
#define MT9811_RDSTATB_PEC_L        0x24U

enum {
    MT9811_CV_A = 0,
    MT9811_CV_B,
    MT9811_CV_C,
    MT9811_CV_D
};

enum {
    MT9811_AUX_A = 0,
    MT9811_AUX_B
};

enum {
    MT9811_STAT_A = 0,
    MT9811_STAT_B
};

enum {
    MD_422HZ_1KHZ = 0,
    MD_27KHZ_14KHZ,
    MD_7KHZ_3KHZ,
    MD_26HZ_2KHZ,
};

enum {
    DCP_DISABLED = 0,
    DCP_ENABLED,
};

enum {
    CELL_CH_ALL = 0,
    CELL_CH_1_7,
    CELL_CH_2_8,
    CELL_CH_3_9,
    CELL_CH_4_10,
    CELL_CH_5_11,
    CELL_CH_6_12
};

enum {
    AUX_CH_ALL = 0,
    AUX_CH_GPIO1,
    AUX_CH_GPIO2,
    AUX_CH_GPIO3,
    AUX_CH_GPIO4,
    AUX_CH_GPIO5,
    AUX_CH_VREF2
};

enum {
    STAT_CH_ALL = 0,
    STAT_CH_SOC,
    STAT_CH_ITEMP,
    STAT_CH_VREGA,
    STAT_CH_VREGD
};

typedef struct {
    uint8_t refon;
    uint8_t adcopt;
    uint8_t gpio[5];
    uint8_t dcc[12];
    uint8_t dcto[4];
    uint16_t uv;
    uint16_t ov;
} reg_cfg;

#if defined(DEV_LITTLE_ENDIAN)
#pragma pack (1)
typedef struct {
    uint8_t adcopt                  : 1;
    uint8_t swtrd                   : 1;
    uint8_t refon                   : 1;
    uint8_t gpio1                   : 1;
    uint8_t gpio2                   : 1;
    uint8_t gpio3                   : 1;
    uint8_t gpio4                   : 1;
    uint8_t gpio5                   : 1;
    uint16_t vuv                    : 12;
    uint16_t vov                    : 12;
    uint16_t dcc                    : 12;
    uint8_t tcto                    : 4;
} mt9811_cfgr;

typedef struct {
    uint16_t v0;
    uint16_t v1;
    uint16_t v2;
} mt9811_cvr;

typedef struct {
    uint16_t g1;
    uint16_t g2;
    uint16_t g3;
} mt9811_auxr_a;

typedef struct {
    uint16_t g4;
    uint16_t g5;
    uint16_t ref;
} mt9811_auxr_b;

typedef struct {
    uint16_t soc;
    uint16_t itmp;
    uint16_t va;
} mt9811_statr_a;

typedef struct {
    uint16_t vd;
    uint8_t c1uv                    : 1;
    uint8_t c1ov                    : 1;
    uint8_t c2uv                    : 1;
    uint8_t c2ov                    : 1;
    uint8_t c3uv                    : 1;
    uint8_t c3ov                    : 1;
    uint8_t c4uv                    : 1;
    uint8_t c4ov                    : 1;
    uint8_t c5uv                    : 1;
    uint8_t c5ov                    : 1;
    uint8_t c6uv                    : 1;
    uint8_t c6ov                    : 1;
    uint8_t c7uv                    : 1;
    uint8_t c7ov                    : 1;
    uint8_t c8uv                    : 1;
    uint8_t c8ov                    : 1;
    uint8_t c9uv                    : 1;
    uint8_t c9ov                    : 1;
    uint8_t c10uv                   : 1;
    uint8_t c10ov                   : 1;
    uint8_t c11uv                   : 1;
    uint8_t c11ov                   : 1;
    uint8_t c12uv                   : 1;
    uint8_t c12ov                   : 1;
    uint8_t thsd                    : 1;
    uint8_t muxfail                 : 1;
    uint8_t rsvd                    : 2;
    uint8_t rev                     : 4;
} mt9811_statr_b;

#pragma pack ()
#else
/* TODO */
#endif

typedef struct {
    struct {
        uint8_t cmd_h;
        uint8_t cmd_l;
        uint8_t pec_h;
        uint8_t pec_l;
        uint8_t param[MT9811_MAX_TRANSFER_SIZE - 4];
    } frm;
    uint8_t len;
} dev_tx;

typedef struct {
    struct {
        uint8_t dummy[4];
        uint8_t param[MT9811_MAX_TRANSFER_SIZE - 4];
    } frm;
    uint8_t len;
} dev_rx;

typedef uint8_t (*dev_transfer)(void *, const uint8_t *, uint8_t *, uint16_t);
typedef void (*dev_delay)(uint16_t);
typedef void (*dev_wakeup_pin)(uint8_t);

typedef struct {
    void *handle;
/* platform */
    dev_transfer transfer;
    dev_delay delay_us;
    dev_wakeup_pin wakeup_pin;
/* tx rx buffers */
    dev_tx tx;
    dev_rx rx;
/* cfgr */
    mt9811_cfgr *w_cfgr;
    mt9811_cfgr *r_cfgr;
/* cell voltage */
    mt9811_cvr *a_cvr;
    mt9811_cvr *b_cvr;
    mt9811_cvr *c_cvr;
    mt9811_cvr *d_cvr;
    uint8_t *a_cv_err;
    uint8_t *b_cv_err;
    uint8_t *c_cv_err;
    uint8_t *d_cv_err;
/* aux voltage*/
    mt9811_auxr_a *a_auxr;
    mt9811_auxr_b *b_auxr;
    uint8_t *a_aux_err;
    uint8_t *b_aux_err;
/* stat */
    mt9811_statr_a *a_statr;
    mt9811_statr_b *b_statr;
    uint8_t *a_stat_err;
    uint8_t *b_stat_err;
/* comm */
    /* TODO */
/* number of chips in daisy chain */
    uint8_t ic_num;
/* pec error */
    uint8_t pec_err;
} mt9811_ctx;


void mt9811_wakeup_sleep(mt9811_ctx *ctx);
uint8_t mt9811_wakeup_idle(mt9811_ctx *ctx);
uint8_t mt9811_write_cfgr(mt9811_ctx *ctx);
uint8_t mt9811_read_cfgr(mt9811_ctx *ctx);
uint8_t mt9811_read_cv(mt9811_ctx *ctx, uint8_t group);
uint8_t mt9811_start_cv(mt9811_ctx *ctx, uint8_t md, uint8_t dcp, uint8_t ch);
uint8_t mt9811_read_aux(mt9811_ctx *ctx, uint8_t group);
uint8_t mt9811_start_aux(mt9811_ctx *ctx, uint8_t md, uint8_t chg);
uint8_t mt9811_start_cv_aux(mt9811_ctx *ctx, uint8_t md, uint8_t dcp);
uint8_t mt9811_read_stat(mt9811_ctx *ctx, uint8_t group);
uint8_t mt9811_start_stat(mt9811_ctx *ctx, uint8_t md, uint8_t chst);
uint8_t mt9811_clear_cv(mt9811_ctx *ctx);
uint8_t mt9811_clear_aux(mt9811_ctx *ctx);

#endif

