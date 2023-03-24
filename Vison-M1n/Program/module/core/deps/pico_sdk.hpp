#ifndef MODULE_CRE_DEPS_PICO_SDK_HPP
#define MODULE_CRE_DEPS_PICO_SDK_HPP

#include <stdint.h>

#define _REG_(x)
typedef volatile uint32_t io_rw_32;
typedef const volatile uint32_t io_ro_32;

typedef struct {
    _REG_(I2C_IC_CON_OFFSET)
    io_rw_32 con;
    _REG_(I2C_IC_TAR_OFFSET)
    io_rw_32 tar;
    _REG_(I2C_IC_SAR_OFFSET)
    io_rw_32 sar;
    uint32_t _pad0;
    _REG_(I2C_IC_DATA_CMD_OFFSET)
    io_rw_32 data_cmd;
    _REG_(I2C_IC_SS_SCL_HCNT_OFFSET)
    io_rw_32 ss_scl_hcnt;
    _REG_(I2C_IC_SS_SCL_LCNT_OFFSET)
    io_rw_32 ss_scl_lcnt;
    _REG_(I2C_IC_FS_SCL_HCNT_OFFSET)
    io_rw_32 fs_scl_hcnt;
    _REG_(I2C_IC_FS_SCL_LCNT_OFFSET)
    io_rw_32 fs_scl_lcnt;
    uint32_t _pad1[2];
    _REG_(I2C_IC_INTR_STAT_OFFSET)
    io_ro_32 intr_stat;
    _REG_(I2C_IC_INTR_MASK_OFFSET)
    io_rw_32 intr_mask;
    _REG_(I2C_IC_RAW_INTR_STAT_OFFSET)
    io_ro_32 raw_intr_stat;
    _REG_(I2C_IC_RX_TL_OFFSET)
    io_rw_32 rx_tl;
    _REG_(I2C_IC_TX_TL_OFFSET)
    io_rw_32 tx_tl;
    _REG_(I2C_IC_CLR_INTR_OFFSET)
    io_ro_32 clr_intr;
    _REG_(I2C_IC_CLR_RX_UNDER_OFFSET)
    io_ro_32 clr_rx_under;
    _REG_(I2C_IC_CLR_RX_OVER_OFFSET)
    io_ro_32 clr_rx_over;
    _REG_(I2C_IC_CLR_TX_OVER_OFFSET)
    io_ro_32 clr_tx_over;
    _REG_(I2C_IC_CLR_RD_REQ_OFFSET)
    io_ro_32 clr_rd_req;
    _REG_(I2C_IC_CLR_TX_ABRT_OFFSET)
    io_ro_32 clr_tx_abrt;
    _REG_(I2C_IC_CLR_RX_DONE_OFFSET)
    io_ro_32 clr_rx_done;
    _REG_(I2C_IC_CLR_ACTIVITY_OFFSET)
    io_ro_32 clr_activity;
    _REG_(I2C_IC_CLR_STOP_DET_OFFSET)
    io_ro_32 clr_stop_det;
    _REG_(I2C_IC_CLR_START_DET_OFFSET)
    io_ro_32 clr_start_det;
    _REG_(I2C_IC_CLR_GEN_CALL_OFFSET)
    io_ro_32 clr_gen_call;
    _REG_(I2C_IC_ENABLE_OFFSET)
    io_rw_32 enable;
    _REG_(I2C_IC_STATUS_OFFSET)
    io_ro_32 status;
    _REG_(I2C_IC_TXFLR_OFFSET)
    io_ro_32 txflr;
    _REG_(I2C_IC_RXFLR_OFFSET)
    io_ro_32 rxflr;
    _REG_(I2C_IC_SDA_HOLD_OFFSET)
    io_rw_32 sda_hold;
    _REG_(I2C_IC_TX_ABRT_SOURCE_OFFSET)
    io_ro_32 tx_abrt_source;
    _REG_(I2C_IC_SLV_DATA_NACK_ONLY_OFFSET)
    io_rw_32 slv_data_nack_only;
    _REG_(I2C_IC_DMA_CR_OFFSET)
    io_rw_32 dma_cr;
    _REG_(I2C_IC_DMA_TDLR_OFFSET)
    io_rw_32 dma_tdlr;
    _REG_(I2C_IC_DMA_RDLR_OFFSET)
    io_rw_32 dma_rdlr;
    _REG_(I2C_IC_SDA_SETUP_OFFSET)
    io_rw_32 sda_setup;
    _REG_(I2C_IC_ACK_GENERAL_CALL_OFFSET)
    io_rw_32 ack_general_call;
    _REG_(I2C_IC_ENABLE_STATUS_OFFSET)
    io_ro_32 enable_status;
    _REG_(I2C_IC_FS_SPKLEN_OFFSET)
    io_rw_32 fs_spklen;
    uint32_t _pad2;
    _REG_(I2C_IC_CLR_RESTART_DET_OFFSET)
    io_ro_32 clr_restart_det;
    uint32_t _pad3[18];
    _REG_(I2C_IC_COMP_PARAM_1_OFFSET)
    io_ro_32 comp_param_1;
    _REG_(I2C_IC_COMP_VERSION_OFFSET)
    io_ro_32 comp_version;
    _REG_(I2C_IC_COMP_TYPE_OFFSET)
    io_ro_32 comp_type;
} i2c_hw_t;


struct i2c_inst {
    i2c_hw_t *hw;
    bool restart_on_next;
};

typedef struct i2c_inst i2c_inst_t;

#endif
